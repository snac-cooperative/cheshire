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
*	Header Name:	se_process_feedback.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Process a weighted result set as a nearest neighbor
*                       or relevance feedback query and 
*                       build a weighted result set. 
*
*	Usage:		se_process_feedback(databasename, weighted_results)
*
*	Variables:	
*                       databasename - the filename or file tag of the
*                           desired database.
*                       weighted_results the records to be "fed back"
*
*	Return Conditions and Return Codes: NULL on errors	
*
*	Date:		12/5/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* the following includes the Z39.50 defs */
#include <stdlib.h>
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"

#ifdef WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#endif

/* external declarations */
extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);
extern int get_key_data( SGML_Data *data, char **current_buffer);
extern void cluster_key_extract(SGML_Document *sgmlrec, char **key_buffer);
extern weighted_result *se_prob(char *file, char *indexname, char *searchstring);
extern int cf_getfiletype();
extern void diagnostic_set();

char *se_feedback_extract(char *filename, 
			  idx_list_entry *idx, 
			  weighted_result *wt_in, 
			  int filetype);

extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);



weighted_result *se_process_feedback(char *file, weighted_result *wt_in)
{
  idx_list_entry *idx, *idx_main;
  char *search_buffer;
  char *mainfile, *cf_getclustered_name();
  weighted_result *prob_result;
  int filetype;
  int cluster_feedback_flag;
  int i;

  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];


  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }


  /* convert the specified elements to an index list entry, if possible */

  attr_nums[USE_ATTR] = -2;
  attr_nums[RELATION_ATTR] = RELEVANCE;

  idx = cf_getidx_mapattr(file,
			  attr_nums, 
			  attr_str,
			  attr_oid);
  

  filetype = cf_getfiletype(file);

  if (filetype == FILE_TYPE_CLUSTER 
      || filetype == FILE_TYPE_CLUSTER_DATASTORE) { 
    cluster_feedback_flag = 1;
    mainfile = cf_getclustered_name(file);
    idx_main = cf_getidx_mapattr(mainfile,
				 attr_nums, 
				 attr_str,
				 attr_oid);
    
  } 
  else {
    cluster_feedback_flag = 0;
    mainfile = file;
    idx_main = idx;
  }


  if (idx) {

    /* first perform an extraction on all the words from appropriate */
    /* fields in each record */
    search_buffer = se_feedback_extract(file,idx, wt_in, filetype);
    
    if (search_buffer != NULL) {
      prob_result = se_prob(mainfile, idx_main->tag, search_buffer);
      FREE(search_buffer);
      if (prob_result) {
	/* Add code to indicate feedback results */
	prob_result->result_type += FEEDBACK_RESULT;
	if (cluster_feedback_flag) {
	   /* Add code to indicate cluster result */
	  prob_result->result_type += CLUSTER_RESULT;
	}
	strcpy(prob_result->filename, mainfile); /* put in the REAL file */
      }
      return (prob_result);
    }
    else
      return (NULL);
  }
  else { /* no matching index with RELEVANCE support criteria */
    /* set diagnostic */
    diagnostic_set(117,0,"No 'RELEVANCE' index mapping found for feedback processing");
    return(NULL);
  }

}

char *se_feedback_extract(char *filename, 
			  idx_list_entry *idx, 
			  weighted_result *wt_in,
			  int filetype)
{
  char *key_buffer;
  int i;

  idx_key *key;
  idx_key *top_key;
  SGML_Tag_Data *matchdata, *td;
  int casesensitive;
  SGML_Document *sgmlrec, *GetSGML();    /* SGML record from mainfile */

  key_buffer = NULL;


  for (i = 0; i < wt_in->num_hits; i++) {
    /* Read the record from the sgml file and parse */
    sgmlrec = GetSGML(filename,wt_in->entries[i].record_num);

    if (sgmlrec == NULL) { 
      fprintf(LOGFILE, "se_feedback_extract: SGML parse error for record %d\n",
	      wt_in->entries[i].record_num);
      return(NULL);
    }
    
    if (sgmlrec->DTD->type > 0) 
      casesensitive = 1;
    else
      casesensitive = 0;
  
    if (filetype == FILE_TYPE_CLUSTER 
	|| filetype == FILE_TYPE_CLUSTER_DATASTORE) { 
      
      cluster_key_extract(sgmlrec,&key_buffer);

    }
    else {
      for (key = idx->keys; key; key = key->next_key) {   
	
	matchdata = comp_tag_list(key,sgmlrec->Tag_hash_tab, 
				  NULL, casesensitive);

	for (td = matchdata; td; td = td->next_tag_data) {
	  get_key_data(td->item, &key_buffer);
	}
	free_tag_data_list(matchdata);
      }
    }
  
/*    printf("KEY BUFFER IS : \n%s\n\n",key_buffer); */
  }

  return(key_buffer);
}




/************************************************************************
*
*	Header Name:	cluster_key_extract --
*
*	Programmer:	Ray R. Larson
*
*	Purpose:        Traversal of a cluster record's
*                       SGML_data structures extracting keywords from 
*                       the cluster key and summary fields in the record.  
*
************************************************************************/
void
cluster_key_extract(  SGML_Document *sgmlrec, char **key_buffer)

{
  SGML_Data *cur_data, *sum_data;   
  int current_frequency, i;

  /* This routine relies on the defined structure of cluster records */
  /* if that structure changes, this routine will need to change too */

  cur_data = sgmlrec->data;

  if (strncasecmp(cur_data->start_tag+1,"CLUSTER", 7) != 0) {
    *key_buffer = NULL;
    return;
  }
  if (cur_data->sub_data_element) { 

    cur_data = cur_data->sub_data_element;

    /* OK, should be cluskey here */
    if (strncasecmp(cur_data->start_tag+1,"CLUSKEY", 7) == 0) {
      /* add the cluster key ... three times */
      /* get_key_data is defined in indexing/key_extract.c */
      get_key_data(cur_data, key_buffer); 
      get_key_data(cur_data, key_buffer); 
      get_key_data(cur_data, key_buffer); 
    }
    for (cur_data = cur_data->next_data_element; 
	 cur_data != NULL; 
	 cur_data = cur_data->next_data_element) {
      
      /* the MAXNUM attribute is required for all summary fields, soo...*/
      if (cur_data->attributes != NULL && 
	  strcasecmp(cur_data->attributes->name, "MAXNUM") == 0) {

	/* this is a summary area -- so go through and extract info */
	for (sum_data = cur_data->sub_data_element; sum_data; 
	     sum_data = sum_data->next_data_element) {

	  /* There are only two sub-elements of summary fields */
	  /* FREQUENCY and SUMDATA, we use them both           */
	  if (strncasecmp(sum_data->start_tag+1,"FREQUENCY", 9) == 0) {
	    sscanf(sum_data->content_start, "%d", &current_frequency);
	  } 
	  else if (strncasecmp(sum_data->start_tag+1,"SUMDATA", 7) == 0) {
	    for (i = 0; i < current_frequency; i++) {
	      get_key_data(sum_data, key_buffer); 
	    }
	  }
	  
	} /* end of loop for sum_data */
      }
    } /* end of loop for cur_data */
  }

}






