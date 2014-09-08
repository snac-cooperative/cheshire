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
*       Header Name:    pseudo_doc.c
*
*       Programmer:     Ray R. Larson
*              
*
*       Purpose:        create a "paged" document
*                       
*
*       Usage:          char *pseudo_doc (weighted_result *);
*
*       Variables:	
*
*       Return Conditions and Return Codes:
*		        returns point to the pseudo document buffer.
*			or NULL on error
*
*       Date:           11/20/96
*       Version:        1.0
*       Copyright (c) 1996.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include <cheshire.h>

/* external functions */
extern display_format *cf_getdisplay(char *, char *);
extern page_assoc_rec *se_get_page_assoc(char *filename, char *indextag, 
				  int pagenum);



extern int displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
		      char *format, char *oid, char *filename, char *add_tags,
		      char **outptr, int rank, int relevance, 
		      float raw_relevance, weighted_result *resultset);

char *se_pseudo_doc(int page_index, weighted_result *inset)
{
  int i = 0; /* loop and array indices */
  int prob_flag = 0;

 FILE *pagenames;

  page_assoc_rec *page_data;

  long pagenum, recnum;
  int recsize;

  SGML_Document *doc, *GetSGML();
  char *c_ptr;
  char *index_tag;
  char filename[100];
  char *record_buffer = NULL;
  char name_data[100];
  char workstring[500];
  int rank, relevance_score;

  idx_list_entry *idx;
  display_format *df;

  if (inset == NULL)
    return(NULL);
  
  /* check to see if the set is from a paged search    */
  if ((inset->result_type & PAGED_RESULT) != PAGED_RESULT) {
    fprintf(LOGFILE, "Not PAGED_RESULT in se_pseudo_doc\n");
    return (NULL);
  }

  /* check to see if the set is a probabilistic search */
  if (inset->result_type & PROBABILISTIC_RESULT) {
    prob_flag = 1;
  }

  /* we need to open up the appropriate index file stuff */
  /* the index name should be stored as part of the file */
  /* name in the input set...                            */
  strcpy(filename, inset->filename);

  if ((c_ptr = strchr(filename, ':')) != NULL) {
    if (*(c_ptr+1) == ':') {
      *c_ptr = '\0';
      index_tag = (char *)(c_ptr + 2);
    }
    else {
      fprintf(LOGFILE, "Can't find index name in se_pseudo_doc\n");
      return (NULL);
    }
  }

  pagenames = (FILE *) cf_index_open(filename, index_tag, PAGENAMEFILE);

  /* use the "internal" page/doc number */
  pagenum = inset->entries[page_index].record_num;

  rank = page_index + 1;
  /* the relevance score is scaled to the best in the set (1 for boolean)*/
  if (prob_flag)
    relevance_score = 
      (int)((inset->entries[page_index].weight/inset->entries[0].weight)*1000.00);
  else
    relevance_score = 1000;


  page_data = se_get_page_assoc(filename, index_tag, pagenum);

  recnum = page_data->parentid;
  pagenum = page_data->pagenum; /* the "external" page number */

  doc = GetSGML(filename, recnum);

  /* got the full doc from the file and parsed it ... now */
  /* trim it according to the specs...                    */
  if (doc) {
    fseek (pagenames, page_data->nameoffset, 0);
    fread (&name_data, 100, 1, pagenames);
   
    sprintf(workstring, "\n<PAGENUM>%d</PAGENUM>\n<PAGENAME>%s</PAGENAME>\n", 
	    pagenum);
	   
    if ((recsize = displayrec(doc, recnum, &record_buffer, 
			      10000, "PAGED_DEFAULT", 
			      NULL, filename, workstring, NULL, rank,
			      relevance_score,
			      inset->entries[page_index].weight, inset)) == -1) {
      diagnostic_set(17,0,NULL);
      printf("record exceeds maxrecsize in displayrec\n");
    }
    else if (recsize == -2) {
      diagnostic_set(25,0,NULL);
      printf("invalid element set name for DB\n");
    }
    else if (recsize == -3) {
      diagnostic_set(26,0,NULL); /* only default elements supported...*/
      printf("Nothing to include in PAGED_DEFAULT format\n");
    }
    else if (recsize == 0) {
      diagnostic_set(14,0,NULL);
      printf("Problem in converting to PAGED_DEFAULT format\n");
      
    }
    
    return (record_buffer);
  }
  
  /* if no doc, no record */
  return(NULL);

}



