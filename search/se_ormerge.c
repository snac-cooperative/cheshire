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
*       Header Name:    se_ormerge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:	Merges two ordered sets of postings 
*			with boolean OR.
*                       
*
*       Usage:          weighted_result *se_ormerge 
*			    (weighted_result *, weighted_result *, int)
*
*       Variables:	set1 & set2 are pointers to original sets
*			newset is pointer to merged set
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
				   weighted_result *inset2, int oper, int add);

extern weighted_result  *se_bitmap_merge (weighted_result *inset1, 
					  weighted_result *inset2, int oper);

extern weighted_result *se_comp2doc(weighted_result *inset);

extern Tcl_HashTable *current_page_hash_table;

#define MERGESUM 1
#define FUZZY 2
#define MERGEMEAN 3
#define MERGENORM 4
#define MERGECMBZ 5
#define MERGENSUM 6
#define MERGENPRV 7

/* min_max normalization for result scores */
float se_min_max_norm(weighted_result *set, int item) {
  float result;
  
  if (set->result_type & PROBABILISTIC_RESULT
      || set->result_type & OKAPI_RESULT
      || set->result_type & TFIDF_RESULT
      || set->result_type & CORI_RESULT 
      || set->result_type & BLOB_RESULT
      || set->result_type & GEORANK_RESULT 
      || set->result_type & FUZZY_RESULT) {
    result = (set->entries[item].weight - set->min_weight)/(set->max_weight - set->min_weight);
  }
  else 
    result = 0.5;
  return (result);
}

/* The main event...  */
weighted_result  *
se_ormerge (weighted_result *inset1, 
	    weighted_result *inset2, int addweights) {

  int i = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int prob_flag = 0;
  int fuzz_flag = 0;
  int paged_flag = 0;
  int both_paged = 0;
  int both_component = 0;
  int component_flag = 0;
  int total_postings, newset_size;
  float save_sum;
  weighted_result *set1, *set2, *newset; /* pointer to merged set */
  
  if (inset1 == NULL && inset2 == NULL) 
    return(NULL);
  
  set1 = inset1;
  set2 = inset2;

  /* if either set is null, just return a dup of the other */
  if (set1 == NULL && set2 != NULL) {
    newset_size = 
      sizeof(weighted_result) + 
	(sizeof(weighted_entry) * set2->num_hits);
    newset = (weighted_result *) CALLOC(char, newset_size);
    memcpy((char *)newset,(char *)set2,newset_size);
    if (addweights) 
      newset->result_weight = newset->result_weight + 1.0;
    return(newset);
  }
  
  if (set1 != NULL && set2 == NULL) {
    newset_size = 
      sizeof(weighted_result) + 
	(sizeof(weighted_entry) * set1->num_hits);
    newset = (weighted_result *) CALLOC(char, newset_size);
    memcpy((char *)newset,(char *)set1,newset_size);
    if (addweights) 
      newset->result_weight = newset->result_weight + 1.0;
    return(newset);
  }

  /* Should have 2 resultsets */

  /* strict or FUZZY boolean operations? */
  if (set1 != NULL 
      && set1->result_type & FUZZY_REQUEST
      && set2 != NULL
      && set2->result_type & FUZZY_REQUEST)
    fuzz_flag = 1;

  /* check to see if either of the sets is from a virtual search    */
  if ((set1->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT
      || (set2->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {
    return(se_virtualmerge(set1, set2, BOOLEAN_OR, addweights));
  }


  /* check to see if either of the sets is from a bitmap search    */
  if ((set1->result_type & BITMAP_RESULT) == BITMAP_RESULT
      || (set2->result_type & BITMAP_RESULT) == BITMAP_RESULT) {
    return(se_bitmap_merge(set1, set2, BOOLEAN_OR));
  }
  
  /* check to see if either of the sets is from a paged search    */
  if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT
      && (set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    both_paged = 1;
  }
  else if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 1;
    set1 = se_get_page_docs(inset1, &current_page_hash_table, 1);
    /* we don't need the hash table here ...??... */
#ifdef MAYBENOT
    if (current_page_hash_table) 
      free_page_hash_tab(current_page_hash_table);
#endif
  }
  else if ((set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 2;
    set2 = se_get_page_docs(inset2, &current_page_hash_table, 2);
    /* we don't need the hash table here ...??... */
#ifdef MAYBENOT
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
  else {
    if ((set1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
      component_flag = 1;
      set1 = se_comp2doc(inset1);
    }
    if ((set2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
      if (component_flag == 1)
	component_flag = 3;
      else
	component_flag = 2;
      set2 = se_comp2doc(inset2);
    }
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
  
  total_postings = set1->num_hits + set2->num_hits;
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * total_postings);
  newset = (weighted_result *) CALLOC(char, newset_size);
  
  if (addweights) {
    if (set1->result_weight > 1.0)
      set1->result_weight = 1.0;
    if (set2->result_weight > 1.0)
      set2->result_weight = 1.0;
    newset->result_weight = set1->result_weight + set2->result_weight;
  }
  /* error check for same set id confirming sets from same query*/
  /* if (strcmp(set1->setid,set2->setid) != 0) return(NULL);    */
  
  /* merge ordered sets into newset */
  save_sum = 0.0;

  while ((i1 < set1->num_hits) && (i2 < set2->num_hits)) {
    if (set1->entries[i1].record_num == set2->entries[i2].record_num) {
      if (addweights == MERGESUM) {
	/* MERGE_SUM */
	if (set1->result_type & PROBABILISTIC_RESULT
	    || set1->result_type & OKAPI_RESULT
	    || set1->result_type & TFIDF_RESULT
	    || set1->result_type & BLOB_RESULT
	    || set1->result_type & GEORANK_RESULT
	    || set1->result_type & CORI_RESULT) {
	  save_sum = 1.0 + set1->entries[i1].weight;
	}
	else
	  save_sum = 1.5;

	if (set2->result_type & PROBABILISTIC_RESULT 
	    || set2->result_type & OKAPI_RESULT
	    || set2->result_type & TFIDF_RESULT
	    || set2->result_type & BLOB_RESULT
	    || set2->result_type & GEORANK_RESULT
	    || set2->result_type & CORI_RESULT) {
	  save_sum += 1.0 + set2->entries[i2].weight;
	}
	else
	  save_sum += 1.5;
      }
      else if  (addweights == FUZZY && fuzz_flag) {
	/* do a fuzzy or match */
	if (set1->result_type & PROBABILISTIC_RESULT
	    || set1->result_type & OKAPI_RESULT
	    || set1->result_type & TFIDF_RESULT
	    || set1->result_type & CORI_RESULT 
	    || set1->result_type & BLOB_RESULT
	    || set1->result_type & GEORANK_RESULT 
	    || set1->result_type & FUZZY_RESULT) {
	  save_sum = set1->entries[i1].weight;
	}
	else
	  save_sum = 0.5;

	if (set2->result_type & PROBABILISTIC_RESULT 
	    || set2->result_type & OKAPI_RESULT
	    || set2->result_type & TFIDF_RESULT
	    || set2->result_type & CORI_RESULT
	    || set2->result_type & BLOB_RESULT
	    || set2->result_type & GEORANK_RESULT
	    || set2->result_type & FUZZY_RESULT) {
	  if (set2->entries[i2].weight > save_sum)
	    save_sum = set2->entries[i2].weight;
	}

      } 
      else if  (addweights == MERGEMEAN) {
	/* MERGE_MEAN : take the mean weight */
	if ((set1->result_type & PROBABILISTIC_RESULT
	     || set1->result_type & OKAPI_RESULT
	     || set1->result_type & TFIDF_RESULT
	     || set1->result_type & CORI_RESULT 
	     || set1->result_type & BLOB_RESULT
	     || set1->result_type & GEORANK_RESULT 
	     || set1->result_type & FUZZY_RESULT) 
	    || (set2->result_type & PROBABILISTIC_RESULT 
		|| set2->result_type & OKAPI_RESULT
		|| set2->result_type & TFIDF_RESULT
		|| set2->result_type & CORI_RESULT
		|| set2->result_type & BLOB_RESULT
		|| set2->result_type & GEORANK_RESULT
		|| set2->result_type & FUZZY_RESULT)) {
	  save_sum = (set1->entries[i1].weight + set2->entries[i2].weight) / 2.0;
	}
	else /* boolean results */
	  save_sum = 0.5;
      } 
      else if  (addweights == MERGENORM) {
	/* MERGE_NORM : NORMALIZE and take the mean weight */
	if ((set1->result_type & PROBABILISTIC_RESULT
	     || set1->result_type & OKAPI_RESULT
	     || set1->result_type & TFIDF_RESULT
	     || set1->result_type & CORI_RESULT 
	     || set1->result_type & BLOB_RESULT
	     || set1->result_type & GEORANK_RESULT 
	     || set1->result_type & FUZZY_RESULT) 
	    || (set2->result_type & PROBABILISTIC_RESULT 
		|| set2->result_type & OKAPI_RESULT
		|| set2->result_type & TFIDF_RESULT
		|| set2->result_type & CORI_RESULT
		|| set2->result_type & BLOB_RESULT
		|| set2->result_type & GEORANK_RESULT
		|| set2->result_type & FUZZY_RESULT)) {
	  save_sum = (se_min_max_norm(set1,i1) + se_min_max_norm(set2,i2)) / 2.0;
	}
	else /* boolean results */
	  save_sum = 0.5;
      } 
      else if  (addweights == MERGECMBZ) {
	/* MERGE_CMBZ : NORMALIZE and rescale/adjust the weights */
	save_sum = (se_min_max_norm(set1,i1) + se_min_max_norm(set2,i2)) * 2.0;      
      } 
      else if  (addweights == MERGENSUM) {
	/* MERGE_NSUM : NORMALIZE and SUM the weights - original SUM */
	save_sum = (se_min_max_norm(set1,i1) + se_min_max_norm(set2,i2));
      } 
      else if  (addweights == MERGENPRV) {
	/* MERGE_NSUM : NORMALIZE and SUM the weights - original SUM */
	save_sum = (se_min_max_norm(set1,i1) + se_min_max_norm(set2,i2)) * 2.0;
      } 
      else { /* not addweights */
	save_sum = set1->entries[i1].weight 
	  + set2->entries[i2].weight; /* old boolean */
      }
      
      memcpy((char *)&newset->entries[i], (char *)&set1->entries[i1++], 
	     sizeof(weighted_entry));

      newset->entries[i++].weight = save_sum;
      save_sum = 0.0;
      /* step past BOTH entries to avoid dups */
      i2++;

    }
    else if (set1->entries[i1].record_num < set2->entries[i2].record_num) {
      memcpy((char *)&newset->entries[i++], (char *)&set1->entries[i1++], 
	     sizeof(weighted_entry));
      if (addweights == MERGEMEAN) /* MERGE_MEAN : doing mean weights */
	newset->entries[i-1].weight = newset->entries[i-1].weight / 2.0 ;
      else if (addweights == MERGENORM) /* MERGE_NORM */
	newset->entries[i-1].weight = se_min_max_norm(set1,i1-1) / 2.0;
      else if (addweights == MERGECMBZ || addweights == MERGENSUM) /*MERGE_CMBZ & MERGE_NSUM*/
	newset->entries[i-1].weight = se_min_max_norm(set1,i1-1) / 2.0;
      else if (addweights == MERGENPRV) {
	float tempval;
	/* MERGE_NPRV penalizes lower-ranked items more than high ranks */
	/* compared to CMBZ or MERGENSUM */
	tempval = se_min_max_norm(set1,i1-1);
	if ((set1->num_hits > 150 && i1 < 100) || (set1->num_hits < 150 && i1 < (set1->num_hits/2)))
	  newset->entries[i-1].weight = tempval;
	else 
	  newset->entries[i-1].weight = tempval / 2.0;
      }
    }
    else {
      memcpy((char *)&newset->entries[i++], (char *)&set2->entries[i2++], 
	     sizeof(weighted_entry));
      if (addweights == MERGEMEAN) /* MERGE_MEAN : doing mean weights */
	newset->entries[i-1].weight = newset->entries[i-1].weight / 2.0 ;
      else if (addweights == MERGENORM) /* MERGE_NORM */
	newset->entries[i-1].weight = se_min_max_norm(set2,i2-1) / 2.0;
      else if (addweights == MERGECMBZ || addweights == MERGENSUM) /*MERGE_CMBZ & MERGE_NSUM*/
	newset->entries[i-1].weight = se_min_max_norm(set2,i2-1) / 2.0;
      else if (addweights == MERGENPRV) {
	float tempval;
	/* MERGE_NPRV penalizes lower-ranked items more than high ranks */
	/* compared to CMBZ or MERGENSUM */
	tempval = se_min_max_norm(set2,i2-1);
	if ((set2->num_hits > 150 && i2 < 100) || (set2->num_hits < 150 && i2 < (set2->num_hits/2)))
	  newset->entries[i-1].weight = tempval;
	else 
	  newset->entries[i-1].weight = tempval / 2.0;
      }

    }
    if (newset->max_weight == 0.0) 
      newset->max_weight = newset->entries[i-1].weight;
    else if (	newset->max_weight < newset->entries[i-1].weight)
      newset->max_weight = newset->entries[i-1].weight;
    if (newset->min_weight == 0.0) 
      newset->min_weight = newset->entries[i-1].weight;
    else if (newset->min_weight > newset->entries[i-1].weight)
      newset->min_weight = newset->entries[i-1].weight;
   
  }
  
  /* Load remainder of longer array of entries.		*
   * (Only one of these while loops executes.)		*/
  while (i1 < set1->num_hits) {
    memcpy((char *)&newset->entries[i++], (char *)&set1->entries[i1++],
	   sizeof(weighted_entry));
    if (addweights == MERGEMEAN) /* MERGE_MEAN :doing mean weights */
      newset->entries[i-1].weight = newset->entries[i-1].weight / 2.0 ;
    else if (addweights == MERGENORM || addweights == MERGECMBZ) /*MERGE_NORM or MERGE_CMBZ*/
      newset->entries[i-1].weight = se_min_max_norm(set1,i1-1) / 2.0;
    else if (addweights == MERGENSUM) /* MERGE_NSUM */
      newset->entries[i-1].weight = se_min_max_norm(set1,i1-1) ;
    else if (addweights == MERGENPRV) {
      float tempval;
      /* MERGE_NPRV penalizes lower-ranked items more than high ranks */
      /* compared to CMBZ or MERGENSUM */
      tempval = se_min_max_norm(set1,i1-1);
      if (tempval > 0.5)  
	newset->entries[i-1].weight = tempval;
      else 
	newset->entries[i-1].weight = tempval / 2.0;
    }

    
    if (newset->max_weight == 0.0) 
      newset->max_weight = newset->entries[i-1].weight;
    else if (	newset->max_weight < newset->entries[i-1].weight)
      newset->max_weight = newset->entries[i-1].weight;
    if (newset->min_weight == 0.0) 
      newset->min_weight = newset->entries[i-1].weight;
    else if (newset->min_weight > newset->entries[i-1].weight)
      newset->min_weight = newset->entries[i-1].weight;

  }

  while (i2 < set2->num_hits) {
    memcpy((char *)&newset->entries[i++], (char *)&set2->entries[i2++],
	   sizeof(weighted_entry));
    if (addweights == MERGEMEAN) /*MERGE_MEAN : doing mean weights */
      newset->entries[i-1].weight = newset->entries[i-1].weight / 2.0 ;
    else if (addweights == MERGENORM || addweights == MERGECMBZ)/*MERGE_NORM or MERGE_CMBZ*/
      newset->entries[i-1].weight = se_min_max_norm(set2,i2-1) / 2.0;
    else if (addweights == MERGENSUM)/* MERGE_NSUM */
      newset->entries[i-1].weight = se_min_max_norm(set2,i2-1) ;
    else if (addweights == MERGENPRV) {
      float tempval;
      /* MERGE_NPRV penalizes lower-ranked items more than high ranks */
      /* compared to CMBZ or MERGENSUM */
      tempval = se_min_max_norm(set2,i2-1);
      if (tempval > 0.5)  
	newset->entries[i-1].weight = tempval;
      else 
	newset->entries[i-1].weight = tempval / 2.0;
    }


    if (newset->max_weight == 0.0) 
      newset->max_weight = newset->entries[i-1].weight;
    else if (	newset->max_weight < newset->entries[i-1].weight)
      newset->max_weight = newset->entries[i-1].weight;
    if (newset->min_weight == 0.0) 
      newset->min_weight = newset->entries[i-1].weight;
    else if (newset->min_weight > newset->entries[i-1].weight)
      newset->min_weight = newset->entries[i-1].weight;

  }

  newset->num_hits = i;
  newset->setid[0] = '\0';
  newset->result_type = 0; /* default Boolean */
  strcpy(newset->filename, set1->filename);

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

