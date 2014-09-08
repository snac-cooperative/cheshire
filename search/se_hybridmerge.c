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
*       Header Name:    se_hybridmerge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Merges two sets of retrieved postings
*			from a database when one set is document-level
*                       and the other is component level, or two different
*                       component sets, or doc-level or component level with
*                       another hybrid set (or two hybrids). The
*                       merges can be done using Boolean operators, or 
*                       proximity/merger operators. 
*                       It functions by calling the normal Boolean functions
*                       on the sub-sets for each real database making up the
*                       virtual database.
*
*       Usage:          weighted_result *se_hybridmerge 
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

static int temp_resultsetid_count = 0;

void reset_resultsetid_count()
{
  temp_resultsetid_count = 0;
}

/* the real thing */
weighted_result  *se_hybridmerge (weighted_result *inset1, 
				   weighted_result *inset2, int oper, int add_weights)
{

  int i = 0, j = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int prob_flag = 0;
  int both_hybrid = 0;
  int hybrid_flag = 0;
  int newset_size;
  weighted_result *set1, *set2, *tempset, *work_set, *newset; /* pointer to merged set */
  char tempresultname[MAX_RSN_LENGTH];
  int num_sub_sets;
  int res_count;
  typedef struct set_pairs {
    weighted_result *set1;
    struct component_list_entry *set1_comp;
    weighted_result *set2;
    struct component_list_entry *set2_comp;
    char *fname;
    weighted_result *merged;
  } set_pairs;
  set_pairs *sets;
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  int exists;
  int setnum;
  int keepworkset = 0;
  char *filename;
  struct component_list_entry *component;


  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);
  
  set1 = inset1;
  set2 = inset2;
  



  /* check to see if either of the sets is from a hybrid search    */
  if ((set1->result_type & HYBRID_RESULT) == HYBRID_RESULT
      && (set2->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
    both_hybrid = 1;
  }
  else if ((set1->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
    hybrid_flag = 1;
  }
  else if ((set2->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
    hybrid_flag = 2;
  }
  else {
    /* this should only be called for hybrid DB searches */
    hybrid_flag = 3; /* both normal results result will be hybrid */
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

  /* store the original sets no matter what...( for mergers and output later */

  if (set1->setid[0] == '\0') {

    if (LastResultSetID == NULL) {
      /* store even 0 hit resultsets, for later access */
      sprintf(tempresultname, "Default_hybridtmp_%d_%s", temp_resultsetid_count, set1->filename);
    }
    else {
      /* store even 0 hit resultsets, for later access */
      sprintf(tempresultname, "%s_hybridtmp_%d_%s", LastResultSetID, 
	      temp_resultsetid_count, set1->filename);
    }
    
#ifndef WIN32
    se_store_result_set(set1, tempresultname, 
			set1->filename, "/tmp", 5);
#else
    se_store_result_set(set1, tempresultname, 
			set1->filename, "c:\\temp", 5);
#endif
      
    temp_resultsetid_count++;
  }


  if (set2->setid[0] == '\0') {
    if (LastResultSetID == NULL) {
      /* store even 0 hit resultsets, for later access */
      sprintf(tempresultname, "Default_hybridtmp_%d_%s", temp_resultsetid_count, set2->filename);
    }
    else {
      /* store even 0 hit resultsets, for later access */
      sprintf(tempresultname, "%s_hybridtmp_%d_%s", LastResultSetID, 
	      temp_resultsetid_count, set1->filename);
    }
    
#ifndef WIN32
    se_store_result_set(set2, tempresultname, 
			set2->filename, "/tmp", 5);
#else
    se_store_result_set(set2, tempresultname, 
			set2->filename, "c:\\temp", 5);
#endif
    temp_resultsetid_count++;
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


  /* the number of set pairs to allocate is based on doc_hits, plus */
  /* new extra (or 2 alone if the hybrid is being made of two normal results*/
  sets = CALLOC(set_pairs, set1->doc_hits + set2->doc_hits + 2);

  /* the cases with empty incoming sets have been handled, from here on */
  /* we try to match up the appropriate subsets in each incoming set    */

  i = 0;



  for (i1 = 0; i1 < set1->num_hits; i1++) {

    if (set1->entries[i1].xtra == NULL) {
      tempset = set1;
      if (set1->component != NULL) {
	component = set1->component;
	filename = set1->component->name;
      }
      else {
	component = NULL;
	filename = set1->filename;
      }
      
    } 
    else {
      tempset = (weighted_result *)set1->entries[i1].xtra;
      component = ((weighted_result *)set1->entries[i1].xtra)->component;
      if (component == NULL)
	filename = ((weighted_result *)set1->entries[i1].xtra)->filename;
      else 
	filename = ((weighted_result *)set1->entries[i1].xtra)->component->name;
    }
    
    entry = Tcl_FindHashEntry(hash_tab, 
			      (void *)filename);

    if (entry == NULL){
      sets[i].set1 = tempset;
      sets[i].fname = filename;
      sets[i].set1_comp = component;
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   hash_tab,
					   (void *)filename,
					   &exists),
		       (ClientData)i);
      i++;
      
    }
    else {
      /* do nothing -- it is already in the table */
    }
    
  }
  

  for (i2 = 0; i2 < set2->num_hits; i2++) {

    if (set2->entries[i2].xtra == NULL) {
      tempset = set2;
      if (set2->component != NULL) {
	component = set2->component;
	filename = set2->component->name;
      }
      else {
	component = NULL;
	filename = set2->filename;
      }
      
    } 
    else {
      tempset = (weighted_result *)set2->entries[i2].xtra;
      component = ((weighted_result *)set2->entries[i2].xtra)->component;
      if (component == NULL)
	filename = ((weighted_result *)set2->entries[i2].xtra)->filename;
      else 
	filename = ((weighted_result *)set2->entries[i2].xtra)->component->name;
    }
    
    
    entry = Tcl_FindHashEntry(hash_tab, 
			      (void *)filename);
    
    if (entry == NULL){
      /* this means a subset NOT in the first result set*/
      sets[i].set2 = tempset;
      sets[i].fname = filename;
      sets[i].set2_comp = component;
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   hash_tab,
					   (void *)filename,
					   &exists),
		       (ClientData)i);
      i++;
      
    }
    else { /* add the second set */
      int tabitem;

      tabitem = (int) Tcl_GetHashValue(entry);
      
      sets[tabitem].set2 = tempset;

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
    }

  }


      /* now we store combined resultset for the hybrid DB */
  newset = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + 
		   ((newset_size+1) * sizeof(weighted_entry))));
  
  strcpy(newset->filename, set1->filename);
  newset->doc_hits = num_sub_sets;
  newset->num_hits = newset_size;	  
  
  newset->result_type = set1->result_type | set2->result_type | HYBRID_RESULT;
  strcpy(newset->filename, set1->filename);
  
  /* add the termids */
  for (i = 0; set1->termid[i] != 0; i++)
    newset->termid[i] = set1->termid[i]; 
  for (j = 0; set2->termid[j] != 0; j++)
    newset->termid[i++] = set2->termid[j]; 
  
  /* add the index pointers */
  for (i = 0; set1->index_ptr[i] != NULL; i++)
    newset->index_ptr[i] = set1->index_ptr[i];
  for (j = 0; set2->index_ptr[j] != NULL; j++)
    newset->index_ptr[i++] = set2->index_ptr[j];
   

  res_count = 0;
    
  for (i = 0; i < num_sub_sets; i++) {
    work_set = sets[i].merged;
    if (work_set == NULL) {
      continue;
    }
    /* type will be combined for all resultsets */
    newset->result_type |= work_set->result_type | HYBRID_RESULT;

    keepworkset = 0;
	
    for (i1 = 0; i1 < work_set->num_hits; i1++) {
      float tempweight;


      newset->entries[res_count].record_num = work_set->entries[i1].record_num;
      if (work_set->entries[i1].xtra == NULL) {
	keepworkset = 1;
	newset->entries[res_count].xtra = work_set;       
      }
      else
	newset->entries[res_count].xtra = work_set->entries[i1].xtra;

      /* OLD: newset->entries[res_count++].weight = work_set->entries[i1].weight; */
      /* We are now doing min-max normalization of the component weights */
      /* NO NORMING tempweight = se_min_max_norm(work_set,i1);  */
      tempweight = se_min_max_norm(work_set,i1); 
      /* at the present We will NOT normalize... (for trec2 probs */
      /* tempweight = work_set->entries[i1].weight; */

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
    if (keepworkset == 0) {
      FREE(work_set);
      sets[i].merged = NULL;
    }
    else {
      
      if (LastResultSetID == NULL) {
	/* store even 0 hit resultsets, for later access */
	sprintf(tempresultname, "Default_hybridtmp_%d_%s", temp_resultsetid_count, work_set->filename);
      }
      else {
	/* store even 0 hit resultsets, for later access */
	sprintf(tempresultname, "%s_hybridtmp_%d_%s", LastResultSetID, 
		temp_resultsetid_count, work_set->filename);
      }
      
#ifndef WIN32
      se_store_result_set(work_set, tempresultname, 
			  work_set->filename, "/tmp", 5);
#else
      se_store_result_set(work_set, tempresultname, 
			  work_set->filename, "c:\\temp", 5);
#endif
      
      temp_resultsetid_count++;
    } 
  }
  
  FREE(sets);
    
  /* newset is NOT saved here, and should be saved by the caller */

  if (prob_flag) {
    newset->result_type |= prob_flag;
    return (se_rank_docs(newset));
  }
  else
    return(newset);
}






