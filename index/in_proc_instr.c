/*
 *  Copyright (c) 1990-1999 [see Other Notes, below]. The Regents of the
 *  University of California (Regents). All Rights Reserved.
 *  
 *  Permission to use, copy, modify, and distribute this software and its
 *  documentation for educational, research, and not-for-profit purposes,
 *  without fee and without a signed licensing agreement, is hereby
 *  granted, provided that the above copyright notice, this paragraph and
 *  the following two paragraphs appear in all copies, modifications, and
 *  distributions. Contact The Office of Technology Licensing, UC
 *  Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
 *  (510) 643-7201, for commercial licensing opportunities. 
 *  
 *  Created by Ray R. Larson, Aitao Chen, and Jerome McDonough, 
 *             School of Information Management and Systems, 
 *             University of California, Berkeley.
 *  
 *    
 *       IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT,
 *       INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
 *       INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE
 *       AND ITS DOCUMENTATION, EVEN IF REGENTS HAS BEEN ADVISED OF THE
 *       POSSIBILITY OF SUCH DAMAGE. 
 *    
 *       REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
 *       NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 *       FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE AND
 *       ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS
 *       PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
 *       MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
 */
/************************************************************************
*
*	Header Name:  in_proc_instr.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      perform processing instructions on the (detagged) buffer
*                     during index processsing
*
*	Usage:	      result_buffer = in_proc_instr(SGML_Data dat, char *in_buff);
*
*	Variables:    
*
*
*	Return Conditions and Return Codes: returns in_buff if not reallocated
*                    otherwise returns new allocated buffer
*
*	Date:		5/20/2000
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2000.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <string.h>
#include "cheshire.h"
#include "dmalloc.h"
#ifdef WIN32
#define strcasecmp _stricmp
#endif


char *
in_proc_instr(SGML_Data *dat, char *in_buffer, 
	      int buff_offset, idx_list_entry *idx)
{
  extern int last_rec_proc;
  extern FILE *LOGFILE;
  char *result_buffer, *c, *work_area;
  int stag_start, etag_end, data_start, data_end;
  int i, j, k, data_len, buff_len;
  SGML_Attribute_Data *ad;
  SGML_Processing_Inst *pi;

  if (in_buffer == NULL) {
    fprintf(LOGFILE,"Null buffer in in_proc_instr.c: record #%d\n",
	    last_rec_proc+1);
    return NULL;
  }

  if (dat == NULL) {
    return in_buffer;
  }
  

  if (dat->processing_flags == PROC_FLAG_IN_CHILD) {
    /* process the child(ren) * of this tag */
    /* Since this can only shrink the size of the buffer, we    */
    /* work directly on the input buffer.                       */
    result_buffer = in_proc_instr(dat->sub_data_element, in_buffer, 
				  buff_offset, idx);
  } 
  else {

    /* currently all instructions are done in place on the input buffer */
    result_buffer = in_buffer;
    stag_start = dat->start_tag_offset - buff_offset;
    if (stag_start < 0) {
      /* this must be the defining tag of a component -- already removed */
    } 
    else {
      data_start = dat->content_start_offset - buff_offset;
      if (dat->content_end_offset == 0) {
	char tempbuf[200];
	
	strncpy(tempbuf, dat->start_tag,199);
	printf ("ERROR in data? Tag starting: \"%s\"...  appears to be unclosed.\n",
		tempbuf);
	printf ("SOME PROCESSING INSTRUCTIONS MAY NOT BE PERFORMED\n\n");
	fprintf (LOGFILE,"ERROR in data? Tag starting: \"%s\"... appears to be unclosed.\n",
		tempbuf);
	fprintf (LOGFILE, "SOME PROCESSING INSTRUCTIONS MAY NOT BE PERFORMED\n\n");
	return in_buffer;
      }
      data_end = dat->content_end_offset - buff_offset;
      etag_end = dat->data_end_offset - buff_offset;
      buff_len = strlen(result_buffer);
      data_len = data_end - data_start;

      work_area = result_buffer+(stag_start - 10);
      
      for (pi = dat->Processing_instr; pi; pi = pi->next_processing_inst) {
	
	/* first verify that there are processing instructions here */
	switch (1<<(pi->Instruction - 1)) {
	  
	case PROC_FLAG_INDEX_SUBST_TAG:
	  if (idx != NULL &&
	      strcasecmp(pi->index_name, idx->tag) == 0) {
	    /* drop through */
	  }
	  else
	    break;
	  
	case PROC_FLAG_SUBST_TAG:
	  /* do a tag substitution for this tag:                      */
	  /* that is, remove the space for the tag itself and join up */
	  /* the tag contents with the content on either end          */
	  /* Since this can only shrink the size of the buffer, we    */
	  /* work directly on the input buffer and shift the data.    */
	  j = data_start;

	  /* if (j < 0) printf("J negative = %d\n", j); */
	  
	  /* copy the data... */
	  for (i = stag_start; i < (stag_start + data_len); i++, j++)
	    result_buffer[i] = result_buffer[j];
	  
	  /* blank out the rest */
	  while (i < etag_end)
	    result_buffer[i++] = ' ';
	
	  /* shift the next word over flush with the contents */
	  j = i;
	  k = i = stag_start + data_len;

	  /* special handling for words split over two lines */
	  k--;
	  if (k < 0) {
	    k = stag_start + data_len;
	    /* printf ("negative k now %d stag_start %d data_len %d\n", k, stag_start, data_len); */
	  }
	  else {
	    while ((result_buffer[k] == ' ' || result_buffer[k] == '\001'
		    || result_buffer[k] == '\t') && k > 0) k--;

	    if (result_buffer[k] == '\n' && result_buffer[k-1] == '-') {
	      result_buffer[k] = ' ';
	      i = k-1;
	    }
	    else if (result_buffer[k] == '\n' && result_buffer[k+1] == '\001')
	      result_buffer[k+1] = ' ';
	  }
	  while (result_buffer[j] != ' ' && result_buffer[j] != '<' && j < buff_len) {
	    result_buffer[i++] = result_buffer[j++];
	    result_buffer[j-1] = ' ';
	  }
	  /* if (i < 0) printf("NEGATIVE I = %d K=%d\n", i, k);	*/

	  if (result_buffer[j] == '<')
	    result_buffer[i++] = '\001';

	  break;
	  
	case PROC_FLAG_INDEX_DELETE_TAG:
	  if (idx != NULL &&
	      strcasecmp(pi->index_name, idx->tag) == 0) {
	    /* drop through */
	  }
	  else
	    break;
	
	case PROC_FLAG_DELETE_TAG:
	  /* remove the tag and it's contents */
	  /* that is, remove the space for the tag and contents and join up */
	  /* the contents on either end                                     */
	  /* Since this can only shrink the size of the buffer, we    */
	  /* work directly on the input buffer.                       */
	
	  /* blank out the entire tag */
	  i = stag_start;
	  while (i < etag_end)
	    result_buffer[i++] = ' ';
	  
	  /* shift the next word over flush with the preceding data */
	  j = i;
	  i = stag_start;
	  while (result_buffer[j] != ' ' && result_buffer[j] != '<' && j < buff_len) {
	    result_buffer[i++] = result_buffer[j++];
	    result_buffer[j-1] = ' ';
	  }
	
	  /* if we did a delete and did not move things (another tag began */
	  /* immediately after), then we flag the end of the delete */
	  if (i == stag_start && result_buffer[j] == '<') {
	    i--;
  
	    while (result_buffer[i] == ' '
		   || result_buffer[i] == '\001') i--;

	    i++;
	    result_buffer[i++] = '\001';
	    result_buffer[i++] = '\002';
	    result_buffer[i++] = '\003';
	  }
	  
	  break;
	  
	case PROC_FLAG_INDEX_SUBST_ATTR:
	  if (idx != NULL &&
	      strcasecmp(pi->index_name, idx->tag) == 0) {
	    /* drop through */
	  }
	  else
	    break;
	  
	case PROC_FLAG_SUBST_ATTR:
	  /* do a tag attribute substitution for this tag */
	  /* that is, remove the space for the tag itself and join up */
	  /* the contents on either end with the specified attribute contents */
	  /* Since this can only shrink the size of the buffer, we    */
	  /* work directly on the input buffer.                       */
	  /* blank out the tag */
	  i = stag_start;
	  while (i < etag_end)
	    result_buffer[i++] = ' ';
	  
	  j = i;
	
	  /* locate and copy the attribute value */
	  if (pi->attr == NULL) {
	    fprintf(LOGFILE,"Null attribute in in_proc_instr.c: record #%d\n",
		    last_rec_proc+1);
	    return NULL;
	  }
	  /* should be OK, so find the appropriate attribute in the data */
	  k = 0;
	  for (ad = dat->attributes; ad; ad = ad->next_attribute_data) {
	    if (strcasecmp(ad->name, pi->attr) == 0) {
	      int lastdata;
	      c = ad->string_val;
	      lastdata = stag_start;
	      while (result_buffer[lastdata] == ' '
		     || result_buffer[lastdata] == '\t'
		     || result_buffer[lastdata] == '\n') lastdata--;
	      if (lastdata < 0) {
		lastdata = stag_start;
	      }
	      
	      
	      if (result_buffer[lastdata] != '\001')
		lastdata = stag_start; /* not shifted */
	      
	      
	      for (k = lastdata; *c; c++, k++)
		result_buffer[k] = *c;
	    }
	  }
	  /* shift the next word over flush with the attribute data */
	  if (j > k) {
	    i = k;
	    while (result_buffer[j] != ' ' && result_buffer[j] != '<' && j < buff_len){
	      result_buffer[i++] = result_buffer[j++];
	      result_buffer[j-1] = ' ';
	    }
	  } 
	  /* else no matching attribute */
	  break;
	  
	case 0:
	  /* this will happen in processing NEXT elements */
	  break;
	}
      }
    }
  }
  /* if this is a sub-element of the original data with siblings */
  /* then process them too                                       */
  if (buff_offset < dat->content_start_offset &&
      dat->next_data_element != NULL)
    result_buffer = in_proc_instr(dat->next_data_element, in_buffer, 
				  buff_offset, idx);
  
  return (result_buffer);
}







