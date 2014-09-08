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
*       Header Name:    se_andmerge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Merges two sub-sets of retrieved postings
*			with boolean AND. The "weight" of each posting
*			in the merged set is the sum of the weight
*			of each posting in the original sets.  This
*			formula should be adjusted after relevance
*			testing.
*                       
*
*       Usage:          weighted_result *se_andmerge 
				(weighted_result *, weighted_result *)
*
*       Variables:	newset is pointer to merged set
*			set1 & set2 are original sets
*
*       Return Conditions and Return Codes:
*			returns pointer to merged weighted_result set
*			or NULL on error
*
*       Date:           11/7/93
*       Revised:        11/10/93
*       Version:        1.000001
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include <cheshire.h>

/* external functions */
extern weighted_result *se_rank_docs(weighted_result *newset);
extern weighted_result *se_get_page_docs(weighted_result *newset, Tcl_HashTable **ht,
				  int idnumber);

extern weighted_result  *se_virtualmerge (weighted_result *inset1, 
				   weighted_result *inset2, int oper, int addweights);

extern weighted_result  *se_bitmap_merge (weighted_result *inset1, 
					  weighted_result *inset2, int oper);

extern weighted_result *se_sortwtset (weighted_result *set);

extern weighted_result *se_comp2doc(weighted_result *inset);

Tcl_HashTable *current_page_hash_table = NULL;

extern float se_min_max_norm(weighted_result *set, int item);


/* The main event...  */
weighted_result *
se_andmerge (weighted_result *inset1, weighted_result *inset2) 
{
  
  int i = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int prob_flag = 0;
  int fuzz_flag = 0;
  int paged_flag = 0;
  int both_paged = 0;
  int both_component = 0;
  int component_flag = 0;
  int total_postings, newset_size;
  weighted_result *set1, *set2, *newset; /* pointer to merged set */

  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);
  
  set1 = inset1;
  set2 = inset2;
  
  /* strict or FUZZY boolean operations? */
  if (set1->result_type & FUZZY_REQUEST && set1->result_type & FUZZY_REQUEST)
    fuzz_flag = 1;

  /* check to see if either of the sets is from a virtual search    */
  if ((set1->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT
      || (set2->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {
    return(se_virtualmerge(set1, set2, BOOLEAN_AND, 0));
  }

  /* check to see if either of the sets is from a bitmap search    */
  if ((set1->result_type & BITMAP_RESULT) == BITMAP_RESULT
      || (set2->result_type & BITMAP_RESULT) == BITMAP_RESULT) {
    return(se_bitmap_merge(set1, set2, BOOLEAN_AND));
  }

  /* check to see if either of the sets is from a paged search    */
  if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT
      && (set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    both_paged = 1;
  }
  else if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 1;
    set1 = se_get_page_docs(inset1,&current_page_hash_table, 1);
#ifdef MAYBEWEDONEEDIT
    /* we don't need the hash table here ...??... */
    if (current_page_hash_table) 
      free_page_hash_tab(current_page_hash_table);
#endif
  }
  else if ((set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 2;
    set2 = se_get_page_docs(inset2,&current_page_hash_table, 2);
    /* we don't need the hash table here ...??... */
#ifdef MAYBEWEDONEEDIT
    if (current_page_hash_table) 
      free_page_hash_tab(current_page_hash_table);
#endif
  }
  

  /* check to see if both of the sets are from a component search */
  if ((set1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT
      && (set2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT
      && set1->component == set2->component) {
    both_component = 1;
  }
  else if ((set1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
    component_flag = 1;
    set1 = se_comp2doc(inset1);
  }
  else if ((set2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
    component_flag = 2;
    set2 = se_comp2doc(inset2);
  }

  /* check to see if either of the sets is a probabilistic search */
  if (set1->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT | GEORANK_RESULT)) {
    se_sortwtset(set1);
    prob_flag = (set1->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		    | CORI_RESULT |FUZZY_RESULT | BLOB_RESULT | GEORANK_RESULT));
  }
  if (set2->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT | GEORANK_RESULT)) {
    se_sortwtset(set2);
    prob_flag |= (set2->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		    | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT | GEORANK_RESULT)); 
  }
  
  /* find size of largest possible merged set, allocate memory */
  if (set1->num_hits < set2->num_hits)
    total_postings = set1->num_hits;
  else
    total_postings = set2->num_hits;
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * total_postings);
  newset = (weighted_result *) CALLOC(char, newset_size);
  

  /* error check for same setid to confirm that both	*
   * sets are from the same query				*/	
  /* if (strcmp(set1->setid,set2->setid) != 0) return(NULL); */
  
  /* merge ordered sets into newset */
  while ((i1 < set1->num_hits) && (i2 < set2->num_hits)) {
    if (set1->entries[i1].record_num < set2->entries[i2].record_num){
      i1++;
      continue;
    }
    if (set1->entries[i1].record_num > set2->entries[i2].record_num){
      i2++;
      continue;
    }
    if (set1->entries[i1].record_num == set2->entries[i2].record_num){
      newset->entries[i].record_num = set1->entries[i1].record_num; 
      
      /* XXXX */
      /* "weight" of record in merged set is 		*
       * sum of weights of terms in two original sets *
       * (Change formula based on relevance testing!)	*/
      if (fuzz_flag) {
	if (prob_flag) { /* take the MEAN for fuzzy and */

	  newset->entries[i].weight = 
	    (set1->entries[i1].weight + set2->entries[i2].weight) * 0.5 ;
	}
	else
	  newset->entries[i].weight = 0.5;
      }
      else {
	if (prob_flag)
	  newset->entries[i].weight =
	    set1->entries[i1].weight + set2->entries[i2].weight;
	else
	  newset->entries[i].weight = 1.0;
      }
      /* 
       * This was an attempt to normalize -- but since 
       * the probabilistic weights are higher that 1 sometimes,
       * and the Booleans are now set to 1.0 ONLY ...
       * if (prob_flag && newset->entries[i].weight >= 3.0)
       *  newset->entries[i].weight = 
       *  newset->entries[i].weight - 
       *  (float)((int)newset->entries[i].weight - 2) ;
       */
      if (newset->max_weight == 0.0) 
	newset->max_weight = newset->entries[i].weight;
      else if (	newset->max_weight < newset->entries[i].weight)
	newset->max_weight = newset->entries[i].weight;
      if (newset->min_weight == 0.0) 
	newset->min_weight = newset->entries[i].weight;
      else if (	newset->min_weight > newset->entries[i].weight)
	newset->min_weight = newset->entries[i].weight;
      

      i++;	/* increment all indices	*/  
      i1++;
      i2++;
    }
  }
  
  newset->num_hits = i;
  newset->setid[0] = '\0';
  newset->result_type = 0; /* default Boolean */

  strcpy(newset->filename, inset1->filename);
  
  if (both_component) {
    newset->result_type |= COMPONENT_RESULT;
    newset->component = set1->component;
  }
  if (both_paged)
    newset->result_type |= PAGED_RESULT;
  if (paged_flag)
    newset->result_type |= PAGED_DOC_RESULT;

  if (fuzz_flag)
    newset->result_type |= FUZZY_RESULT;

  if (prob_flag) {
    newset->result_type |= prob_flag;
    return (se_rank_docs(newset));
  }
  else
    return(newset);
}






