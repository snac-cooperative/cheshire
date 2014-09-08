/*
 *  Copyright (c) 1990-2002 [see Other Notes, below]. The Regents of the
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
*       Header Name:    se_virtualmerge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Merges two sets of retrieved postings
*			from a virtual database with Boolean operators. 
*                       It functions by calling the normal Boolean functions
*                       on the sub-sets for each real database making up the
*                       virtual database.
*
*       Usage:          weighted_result *se_virtualmerge 
*				(weighted_result *, weighted_result *, 
*                                resultname, oper)
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
extern weighted_result *se_sortwtset (weighted_result *set);

extern weighted_result  *se_andmerge (weighted_result *inset1, weighted_result *inset2);
extern weighted_result  *se_ormerge (weighted_result *inset1, weighted_result *inset2, int add_weights);
extern weighted_result  *se_notmerge (weighted_result *inset1, weighted_result *inset2);


extern weighted_result *se_dup_result_set(weighted_result *set);

extern float se_min_max_norm(weighted_result *set, int item);

extern char *LastResultSetID; /* set in the command parser */

/* the real thing */
weighted_result  *se_virtualmerge (weighted_result *inset1, 
				   weighted_result *inset2, int oper, int add_weights)
{

  int i = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int prob_flag = 0;
  int both_virt = 0;
  int virt_flag = 0;
  int newset_size;
  weighted_result *set1, *set2, *work_set, *newset; /* pointer to merged set */
  char tempresultname[MAX_RSN_LENGTH];
  int num_sub_sets;
  int res_count;
  typedef struct set_pairs {
    weighted_result *set1;
    weighted_result *set2;
    char *fname;
    weighted_result *merged;
  } set_pairs;
  set_pairs *sets;
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  int exists;



  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);
  
  set1 = inset1;
  set2 = inset2;
  
  /* check to see if either of the sets is from a virtual search    */
  if ((set1->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT
      && (set2->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {
    both_virt = 1;
  }
  else if ((set1->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {
    virt_flag = 1;
  }
  else if ((set2->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {
    virt_flag = 2;
  }
  else {
    /* this should only be called for virtual DB searches */
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

  /* Some shortcuts to avoid unnecessary processing */

  if (set1->num_hits == 0 && set2->num_hits == 0) {
    /* No hits and NO POSSIBLE HITS in combined result */
    newset = se_dup_result_set(set1);
    newset->doc_hits = set1->doc_hits;
    return (newset);
  } 
  else if (set1->num_hits == 0 && oper == BOOLEAN_OR) {
    /* for OR just return the other set */
    newset = se_dup_result_set(set2);
    return (newset);
  }
  else if (set2->num_hits == 0 && oper == BOOLEAN_OR) {
    /* for OR just return the other set */
    newset = se_dup_result_set(set1);
    return (newset);
  }
    

  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);


  sets = CALLOC(set_pairs, set1->doc_hits + set2->doc_hits);

  /* the cases with empty incoming sets have been handled, from here on */
  /* we try to match up the appropriate subsets in each incoming set    */

  i = 0;



  for (i1 = 0; i1 < set1->num_hits; i1++) {
    
    entry = Tcl_FindHashEntry(hash_tab, 
			      (void *)((weighted_result *)set1->entries[i1].xtra)->filename);

    if (entry == NULL){
      
      sets[i].set1 = (weighted_result *)set1->entries[i1].xtra;
      sets[i].fname = ((weighted_result *)set1->entries[i1].xtra)->filename;
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   hash_tab,
					   (void *)((weighted_result *)set1->entries[i1].xtra)->filename,
					   &exists),
		       (ClientData)i);
      i++;
      
    }
    else {
      /* do nothing -- it is already in the table */
    }    
  }
 

  for (i2 = 0; i2 < set2->num_hits; i2++) {
	
    entry = Tcl_FindHashEntry(hash_tab, 
			      (void *)((weighted_result *)set2->entries[i2].xtra)->filename);

    if (entry == NULL){
      /* this means a subset NOT in the first result set*/
      sets[i].set2 = (weighted_result *)set2->entries[i2].xtra;
      sets[i].fname = ((weighted_result *)set2->entries[i2].xtra)->filename;
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   hash_tab,
					   (void *)((weighted_result *)set2->entries[i2].xtra)->filename,
					   &exists),
		       (ClientData)i);
      i++;

      
    }
    else {
      int tabitem;

      tabitem = (int) Tcl_GetHashValue(entry);
      
      sets[tabitem].set2 = (weighted_result *)set2->entries[i2].xtra;

    }    
  }
 

  num_sub_sets = i;

  /* now we combine on a sub-set by sub-set basis... */
  newset_size = 0;

  for (i = 0; i < num_sub_sets; i++) {
  
    switch (oper) {
    case BOOLEAN_AND:
      sets[i].merged = se_andmerge(sets[i].set1, sets[i].set2);
      break;
    case BOOLEAN_OR:
      sets[i].merged = se_ormerge(sets[i].set1, sets[i].set2, add_weights);
      break;
    case BOOLEAN_NOT:
	sets[i].merged = se_notmerge(sets[i].set1, sets[i].set2);
      break;
    case PROX_OPERATOR:
      break;
    }
    
    
    if (sets[i].merged != NULL) {
      newset_size += sets[i].merged->num_hits;

      if (LastResultSetID == NULL) {
	/* store even 0 hit resultsets, for later access */
	sprintf(tempresultname, "Default_%s", sets[i].merged->filename);
      }
      else {
	/* store even 0 hit resultsets, for later access */
	sprintf(tempresultname, "%s_%s", LastResultSetID, 
		sets[i].merged->filename);
      }
      
#ifndef WIN32
      se_store_result_set(sets[i].merged, tempresultname, 
			  sets[i].merged->filename, "/tmp", 5);
#else
      se_store_result_set(sets[i].merged, tempresultname, 
			  sets[i].merged->filename, "c:\\temp", 5);
#endif
      
    }

  }


      /* now we store combined resultset for the virtual DB */
  newset = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + 
		   ((newset_size+1) * sizeof(weighted_entry))));
  
  strcpy(newset->filename, set1->filename);
  newset->doc_hits = num_sub_sets;
  newset->num_hits = newset_size;	  
  res_count = 0;
    
  for (i = 0; i < num_sub_sets; i++) {
    work_set = sets[i].merged;
    if (work_set == NULL) {
      continue;
    }
    /* type will be combined for all resultsets */
    newset->result_type |= work_set->result_type | VIRTUAL_RESULT;
	
    for (i1 = 0; i1 < work_set->num_hits; i1++) {
      float tempweight;
      newset->entries[res_count].record_num = i1;
      newset->entries[res_count].xtra = work_set;
      /* OLD: newset->entries[res_count++].weight = work_set->entries[i1].weight; */
      /* We are now doing min-max normalization of the component weights */
      tempweight = se_min_max_norm(work_set,i1);
      newset->entries[res_count++].weight = tempweight;

      if (newset->max_weight == 0.0) 
	newset->max_weight = tempweight;
      else if (newset->max_weight < tempweight)
	newset->max_weight = tempweight;
      if (newset->min_weight == 0.0) 
	newset->min_weight = tempweight;
      else if (newset->min_weight > tempweight)
	newset->min_weight = tempweight;
      
    }
  } 
  
  FREE(sets);

  if (prob_flag) {
    newset->result_type |= prob_flag;
    return (se_rank_docs(newset));
  }
  else
    return(newset);
}






