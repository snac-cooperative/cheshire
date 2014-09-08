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
*       Header Name:    se_comp2doc.c
*
*       Programmer:     Ray R. Larson
*              
*
*       Purpose:        Convert a list of components to a list of documents
*                       containing those components
*
*       Usage:          se_comp2doc(weighted_result *inset) 
*
*       Variables:	
*
*       Return Conditions and Return Codes:
*			returns pointer to new DOCS weighted_result set
*			or NULL on error
*
*       Date:           4/3/2001
*       Version:        1.0
*       Copyright (c) 2001.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <cheshire.h>

/* external functions */
weighted_result *se_rank_docs(weighted_result *newset);
weighted_result *se_sortwtset(weighted_result *newset);
component_data_item *get_component_record(int component_id, 
					  component_list_entry *comp);

weighted_result *se_comp2doc(weighted_result *inset)
{

  int i = 0; /* loop and array indices */
  int j = 0; /* loop and array indices */
  int prob_flag = 0;
  int doc_count = 0;
  int exists;
  int compnum, recnum;
  int num_comp;
  float sum_wts;
  int total_postings, newset_size;
  weighted_result *newset, *tempset, *compset; /* result set*/
  component_data_item *comp_data;

  if (inset == NULL)
    return(NULL);
  
  /* check to see if the set is a probabilistic search */
  if (inset->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT)) {
      prob_flag = (inset->result_type 
		   & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		      | CORI_RESULT |FUZZY_RESULT | BLOB_RESULT));
  }

  /* find size of largest possible merged set, allocate memory */
  /* for mapping structure                                     */
  total_postings = inset->num_hits;

  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * total_postings);
  tempset = (weighted_result *) CALLOC(char, newset_size);
 

  for (i = 0; i < total_postings; i++) {
    
    if ((inset->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
      
      tempset->entries[i] = inset->entries[i];
      compset = (weighted_result *)inset->entries[i].xtra;
      if (compset->component != NULL) {
	comp_data = get_component_record(inset->entries[i].record_num,
					 compset->component);
	tempset->entries[i].record_num = comp_data->record_id; 
	FREE(comp_data);
      }
      
    }
    else { /* just a component set */
      
      tempset->entries[i] = inset->entries[i];
      comp_data = get_component_record(inset->entries[i].record_num,
				       inset->component);
      tempset->entries[i].record_num = comp_data->record_id; 
      FREE(comp_data);
      
    }
    
  }
  tempset->num_hits = inset->num_hits;
  
  tempset = se_sortwtset (tempset);
  
  
  /* we don't know how many docs there are... */
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * inset->num_hits);
  newset = (weighted_result *) CALLOC(char, newset_size);
  
  newset->setid[0] = '\0';
  newset->result_type = 0; /* default Boolean */
  /* copy the max and min scores */
  newset->max_weight = inset->max_weight;
  newset->min_weight = inset->min_weight;
  newset->result_weight = inset->result_weight;
  
  strcpy(newset->filename, inset->filename);
  
  newset->result_type |= (inset->result_type & (~COMPONENT_RESULT));
  
  i = 0;
  doc_count = 0;
  
  newset->entries[0].weight = tempset->entries[0].weight;
  newset->entries[0].record_num = tempset->entries[0].record_num;
  recnum = tempset->entries[0].record_num;
  doc_count = 1;
  
  for (j = 1; j < inset->num_hits; j++) {
    if (recnum == tempset->entries[j].record_num) {
      if (prob_flag)
	newset->entries[i].weight = 
	  (tempset->entries[j].weight > tempset->entries[i].weight) ? 
	  tempset->entries[j].weight : tempset->entries[i].weight;
      else
	newset->entries[i].weight = 1; /* Boolean result */
    }
    else {
      i++;
      doc_count++;
      newset->entries[i].weight = tempset->entries[j].weight;
      newset->entries[i].record_num = 
	recnum = tempset->entries[j].record_num;
    }
  }

  newset->num_hits = doc_count;

  if (prob_flag) {
    newset->result_type |= prob_flag;
    return(se_rank_docs(newset));
  }
  else
    return(newset);

}



/* count the number of documents represented in a list of components */
int se_count_comp_docs(weighted_result *inset) 
{
  weighted_result *docresult;
  int resultnum;

  if (inset->component == NULL
      || (inset->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
    return (inset->num_hits);
  }
  
  /* must be a regular component resultset */
  docresult = se_comp2doc(inset);

  if (docresult == NULL)
    return 0;

  resultnum = docresult->num_hits;

  FREE(docresult);

  return (resultnum);

}


