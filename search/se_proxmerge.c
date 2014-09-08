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
*       Header Name:    se_proxmerge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Merges two sub-sets of retrieved postings
*			with boolean AND and proximity testing. 
*                       The "weight" of each posting
*			in the merged set is the sum of the weight
*			of each posting in the original sets.  This
*			formula should be adjusted after relevance
*			testing.
*                       
*
*       Usage:          weighted_result *se_proxmerge 
*			   (ProximityOperator *, weighted_result *, 
*			   weighted_result *)
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

/* the following includes the Z39.50 defs */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include <cheshire.h>

/* external functions */
extern weighted_result *se_rank_docs(weighted_result *newset);
extern weighted_result *se_get_page_docs(weighted_result *newset, Tcl_HashTable **ht,
				  int idnumber);

extern weighted_result *se_ormerge (weighted_result *inset1, 
			      weighted_result *inset2, int addweights);

extern weighted_result *se_andmerge (weighted_result *inset1, 
			      weighted_result *inset2);

extern weighted_result *se_notmerge (weighted_result *inset1, 
			      weighted_result *inset2);


extern weighted_result  *se_hybridmerge (weighted_result *inset1, 
					 weighted_result *inset2, int oper, 
					 int add_weights);

extern weighted_result *se_sortwtset (weighted_result *set);

extern DB *cf_component_open(char *filename, char *componentname);

extern float se_min_max_norm(weighted_result *set, int item);

extern int compare_wt_entries (weighted_entry *e1, weighted_entry *e2);

extern Tcl_HashTable *current_page_hash_table;

int
ProxTest(ProximityOperator *op, weighted_result *inset1, 
	      weighted_result *inset2, int rec_num);

#define MERGESUM 1
#define FUZZY 2
#define MERGEMEAN 3
#define MERGENORM 4
#define MERGECMBZ 5
#define MERGENSUM 6
#define MERGENPRV 7
#define MERGEPIVOT 8
#define MERGEHYBRID 9

weighted_result  *
se_component_doc_restrict(ProximityOperator *op, weighted_result *inset1, 
		      weighted_result *inset2) 
{
  weighted_result *compset, *docset, *newset; /* pointer to merged set */
  int dochits;
  int comphits;
  int i,j, maxval, minval;
  int newset_size;
  int hitcount;
  component_data_item return_data, *comp_data;
  component_list_entry *comp;
  DB *db;
  DBT keyval;
  DBT dataval;
  int returncode;
  char temp[50];
  
  /* check to see if either of the sets is from a component search */
  if ((inset1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
    compset = inset1;
    docset = inset2;
  }
  else {
    compset = inset2;
    docset = inset1;
  }
  
  comp = compset->component;

  if (comp->comp_db == NULL) { /* have to open the component file */
    db = cf_component_open(docset->filename, comp->name);
    
  }
  
  hitcount = 0;
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) 
			       * (compset->num_hits+1));
  newset = (weighted_result *) CALLOC(char, newset_size);
  
  /* we go through each of the doc hits and add the corresponding */
  /* component hits to the output set                             */
  for (i = 0; i < docset->num_hits; i++) {
    
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    sprintf(temp,"DOC %012d", docset->entries[i].record_num );
    keyval.data = (void *)temp;
    keyval.size = strlen(temp) + 1; /* include null */
    returncode = comp->comp_db->get(comp->comp_db, NULL, &keyval, 
				    &dataval, 0);
    if (returncode == 0) { 
      /* OK got the data */
      comp_data = &return_data;
      memcpy(comp_data, dataval.data, dataval.size);
    } 
    else {
      /* unsupported index combination  */
      if (returncode == DB_NOTFOUND) {
	return_data.end_offset = 0;
	return_data.start_offset = 0;
      } 
      else {
	diagnostic_set(201,0,"");  
	return (NULL);
      }
    }

    /* We now have the data for this document -- go through the list of */
    /* components and see what can be found                             */
    maxval = comp_data->end_offset;
    minval = comp_data->start_offset;

    for (j = 0; j < compset->num_hits; j++) {
      if (compset->entries[j].record_num >= minval 
	  && compset->entries[j].record_num <= maxval) {
	/* its a match copy it over to newset */
	newset->entries[hitcount].record_num = 
	  compset->entries[j].record_num; 
	newset->entries[hitcount++].weight = 
	  compset->entries[j].weight; 

      }
    }
  }

  newset->num_hits = hitcount;
  newset->setid[0] = '\0';
  newset->result_type = compset->result_type;
  newset->component = compset->component;

  strcpy(newset->filename, compset->filename);

  /* add the termids */
  for (i = 0; compset->termid[i] != 0; i++)
    newset->termid[i] = compset->termid[i]; 

  /* add the index pointers */
  for (i = 0; compset->index_ptr[i] != NULL; i++)
    newset->index_ptr[i] = compset->index_ptr[i];
  
  
  return(newset);
}


weighted_result  *
se_pivot_doc_normalize(ProximityOperator *op, weighted_result *inset1, 
		       weighted_result *inset2) 
{
  weighted_result *compset, *docset, *newset; /* pointer to merged set */
  weighted_entry key, *res;
  int dochits;
  int comphits;
  int i,j, maxval, minval;
  int newset_size;
  int hitcount;
  int compflag=0;
  int record_id=0;
  int component_id=0;
  float comp_score, doc_score, result_score;
  float minscore, maxscore;
  component_data_item return_data, *comp_data;
  component_list_entry *comp;
  DB *db;
  DBT keyval;
  DBT dataval;
  int returncode;
  char temp[50];
  float pivot_val=0.70;
  

  /* if compset is NULL we return NULL */
  if (inset1 == NULL) 
    return (NULL);
  /* if docset is NULL, we return compset, with no normalization */
  if (inset2 == NULL)
    return (inset1);
  
  /* Always treat the right-hand branch as the DOCUMENT scores (docset) */  
  compset = inset1;
  docset = se_sortwtset(inset2);
  
  /* if these are COMPONENT results, set compflag */
  if ((compset->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
    compflag = 1;
    comp = compset->component;
    if (comp->comp_db == NULL) { /* have to open the component file */
      db = cf_component_open(docset->filename, comp->name);
    }
  }
  else 
    compflag = 0;
  
  if (op->distance > 0.00 && op->distance < 100.00) {
    pivot_val = ((float)op->distance) / 100.0;

  }
  else if (op->distance > 100.00 && op->distance < 1000.00) {
    pivot_val = ((float)op->distance) / 1000.0;
  }
  /* otherwise keep the default pivot_val of 0.70 */
  
  
  /* build the result set based on the size of the compset */
  
  hitcount = 0;
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) 
			       * (compset->num_hits+1));
  newset = (weighted_result *) CALLOC(char, newset_size);
  
  /* we go through each of the comp hits and normalize the score based */
  /* on the document results for the appropriate doc...                */
  for (i = 0; i < compset->num_hits; i++) {
    
    /* pre-normalize the score to the range 0-1 -- just in case */
    comp_score = se_min_max_norm(compset, i);
    
    newset->entries[i].record_num = 
      compset->entries[i].record_num; 
    
    
    if (compflag) {
      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));
      component_id = compset->entries[i].record_num;
      keyval.data = (void *)&component_id;
      keyval.size = sizeof(component_id);
      returncode = comp->comp_db->get(comp->comp_db, NULL, &keyval, 
				      &dataval, 0);
      if (returncode == 0) { 
	/* found the entry... */
     	comp_data = &return_data;
     	memcpy(comp_data, dataval.data, dataval.size);
	record_id = comp_data->record_id; 
	maxval = comp_data->end_offset;
	minval = comp_data->start_offset;
      } 
      else {
	/* unsupported index combination  */
	if (returncode == DB_NOTFOUND) {
     	  return_data.end_offset = 0;
	  return_data.start_offset = 0;
     	} 
	else {
	  diagnostic_set(201,0,"");  
     	  return (NULL);
     	}
      }
    } 
    else {
      /* just plain old doc-level index -- so use the docid */
      record_id = compset->entries[i].record_num;
      minval = 0;
      maxval = 0;
    }

    /* new bsearch code */
    key.record_num = record_id;
    key.xtra = NULL;
    

    res = bsearch(&key, docset->entries, docset->num_hits,
		  sizeof(weighted_entry), compare_wt_entries);
    if (res == NULL)
      newset->entries[i].weight = (1-pivot_val) * comp_score;     
    else {
      doc_score = (res->weight - docset->min_weight) /(docset->max_weight - docset->min_weight);
   
      result_score = (pivot_val * doc_score) + ((1-pivot_val) * comp_score); 
	
      newset->entries[i].weight = result_score;
    }
  

    if (newset->max_weight == 0.0) 
      newset->max_weight = newset->entries[i].weight;
    else if (	newset->max_weight < newset->entries[i].weight)
      newset->max_weight = newset->entries[i].weight;
    if (newset->min_weight == 0.0) 
      newset->min_weight = newset->entries[i].weight;
    else if (newset->min_weight > newset->entries[i].weight)
      newset->min_weight = newset->entries[i].weight;

  }
    
  newset->num_hits = compset->num_hits;
  newset->setid[0] = '\0';
  newset->result_type = compset->result_type;
  newset->component = compset->component;
  
  strcpy(newset->filename, compset->filename);
  
  /* add the termids */
  for (i = 0; compset->termid[i] != 0; i++)
    newset->termid[i] = compset->termid[i]; 
  
  /* add the index pointers */
  for (i = 0; compset->index_ptr[i] != NULL; i++)
    newset->index_ptr[i] = compset->index_ptr[i];
   
  return(newset);

}



weighted_result  *
se_hybrid_merge_normalize(ProximityOperator *op, weighted_result *inset1, 
		       weighted_result *inset2) 
{
  weighted_result *set1, *set2, *newset; /* pointer to merged set */
  weighted_entry key, *res;
  int dochits;
  int comphits;
  int i,j, newi, maxval, minval;
  int newset_size;
  int hitcount;
  int compflag=0;
  int record_id=0;
  int component_id=0;
  float set1_score, set2_score, result_score;
  float minscore, maxscore;
  component_data_item return_data, *comp_data;
  component_list_entry *comp;
  DB *db;
  DBT keyval;
  DBT dataval;
  int returncode;
  char temp[50];
  float pivot_val=0.70;
  

  /* if both are NULL we return NULL */
  if (inset1 == NULL && inset2 == NULL) 
    return (NULL);

  /* if one is NULL, we return the other, with no normalization */
  if (inset2 == NULL)
    return (inset1);
  if (inset1 == NULL)
    return (inset2);
  
  /* Always treat the right-hand branch as the DOCUMENT scores (set2) */  
  set1 = inset1;
  set2 = inset2;

  /* do a normalized merge of paired subsets of the hybrid sets */
  newset = (weighted_result *)se_hybridmerge (inset1, inset2, BOOLEAN_OR, 
					      MERGENORM);

  return(newset);

}


/* this matches components that are sub-elements of other components */
/* it returns a component resultset of the CONTAINING components     */
weighted_result  *
se_component_sub_restrict(ProximityOperator *op, weighted_result *inset1, 
		      weighted_result *inset2) 
{
  weighted_result *parentcomp, *childcomp, *newset; /* pointer to merged set */
  int parenthits;
  int childhits;
  int i,j, maxval, minval;
  int newset_size;
  int hitcount;
  component_data_item return_data, *comp_data, *child_data;
  component_list_entry *parententry, *childentry;
  DB *db;
  DBT keyval;
  DBT dataval;
  int returncode;
  char temp[50];
  
  /* if the ELEMENT type is used it is PARENT / CHILD */
  if ((op->proximityUnitCode.which == e8_known 
       && op->proximityUnitCode.u.known == 8 && op->distance <= 1)
      || (op->proximityUnitCode.which == e8_private 
	  && op->proximityUnitCode.u.private_var == 4)) {
    parentcomp = inset1;
    childcomp = inset2;
  } 
  else if ((op->proximityUnitCode.which == e8_known 
	    && op->proximityUnitCode.u.known == 9 && op->distance <= 1)
	   || (op->proximityUnitCode.which == e8_private 
	       && op->proximityUnitCode.u.private_var == 5)) {
    /* this is CHILD/PARENT */
    parentcomp = inset2;
    childcomp = inset1;
    
  } 
  else {
    /* Not the right type of proximity */
    diagnostic_set(132,op->proximityUnitCode.u.known,""); /* unsupported unit */
    return (NULL);
  }

  
  parententry = parentcomp->component;

  if (parententry->comp_db == NULL) { /* have to open the component file */
    db = cf_component_open(parentcomp->filename, parententry->name);
    
  }
  
  childentry = childcomp->component;

  if (childentry->comp_db == NULL) { /* have to open the component file */
    db = cf_component_open(childcomp->filename, childentry->name);
    
  }
  
  hitcount = 0;
  /* can't have more hits than the parent */
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) 
			       * (parentcomp->num_hits+1));
  newset = (weighted_result *) CALLOC(char, newset_size);
  

  /* this is expensive, but it can't be avoided given the way things  */
  /* are set up -- we read and store the component_data for each child*/
  for (j = 0; j < childcomp->num_hits; j++) {
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    /* printf("I = %d, Rec = %d\n", j, childcomp->entries[j].record_num); */
    keyval.data = (void *)&(childcomp->entries[j].record_num);
    keyval.size = sizeof(int);
    returncode = childentry->comp_db->get(childentry->comp_db, NULL, &keyval, 
				    &dataval, 0);
    if (returncode == 0) { 
      /* OK got the data */
      comp_data = CALLOC(component_data_item, 1);
      memcpy(comp_data, dataval.data, dataval.size);
      if (childcomp->entries[j].xtra == NULL)
	childcomp->entries[j].xtra = comp_data;
      else {
	/* printf("Component data problem with entry %d\n", j); */
	diagnostic_set(2,0,"Component Proximity and Virtual databases can't be combined"); 
	return (NULL);
      }
    } 
    else {
      if (returncode == DB_NOTFOUND) {
	fprintf(LOGFILE, "DB_NOTFOUND signalled -- trying again\n");
        diagnostic_set(2,0,"Component data not in database?"); 
	return (NULL);
      }
    }      
  }
  
  /* we go through each of the doc hits and add the corresponding */
  /* component hits to the output set                             */
  for (i = 0; i < parentcomp->num_hits; i++) {
    
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    keyval.data = (void *)&(parentcomp->entries[i].record_num);
    keyval.size = sizeof(int);
    returncode = parententry->comp_db->get(parententry->comp_db, NULL, &keyval, 
				    &dataval, 0);
    if (returncode == 0) { 
      /* OK got the data */
      comp_data = &return_data;
      memcpy(comp_data, dataval.data, dataval.size);
    } 
    else {
      diagnostic_set(2,0,"Component data not in database?"); /* System error */
      return (NULL);
    }
    for (j = 0; j < childcomp->num_hits; j++) {
      child_data = (component_data_item *)childcomp->entries[j].xtra;
      /* now check to see if this parent contains this child */
      if (comp_data->record_id == child_data->record_id
	  && comp_data->start_offset <= child_data->start_offset
	  && comp_data->end_offset >= child_data->end_offset) {
	/* it is contained add this item to the outlist */
	newset->entries[hitcount].record_num = 
	  parentcomp->entries[i].record_num;
	newset->entries[hitcount++].weight = 
	  parentcomp->entries[i].weight; 
	break;
      }
    }

  }

  newset->num_hits = hitcount;
  newset->setid[0] = '\0';
  newset->result_type = parentcomp->result_type;
  newset->component = parentcomp->component;

  strcpy(newset->filename, parentcomp->filename);


  /* add the termids */
  for (i = 0; parentcomp->termid[i] != 0; i++)
    newset->termid[i] = parentcomp->termid[i]; 

  /* add the index pointers */
  for (i = 0; parentcomp->index_ptr[i] != NULL; i++)
    newset->index_ptr[i] = parentcomp->index_ptr[i];
  
  /* free up the temp data used in matching */
  for (j = 0; j < childcomp->num_hits; j++) {
    if (childcomp->entries[j].xtra)
      FREE(childcomp->entries[j].xtra);
    childcomp->entries[j].xtra = NULL;
  }

  return(newset);
}


weighted_result  *
se_proxmerge (ProximityOperator *op, weighted_result *inset1, 
	      weighted_result *inset2) 
{

  int i = 0, i1 = 0, i2 = 0, j = 0; /* loop and array indices */
  int prob_flag = 0;
  int paged_flag = 0;
  int both_paged = 0;
  int both_component = 0;
  int total_postings, newset_size;
  weighted_result *set1, *set2, *newset; /* pointer to merged set */
  
  if (inset1 == NULL && inset2 == NULL) 
    return(NULL);

  newset = NULL;

  /* New Operators for Fuzzy and or and not */
  if (op->proximityUnitCode.which != e8_known) {
    switch (op->proximityUnitCode.u.private_var) {
    case 1:
      /* fuzzy AND */
      if (inset1 == NULL && inset2 != NULL) {
	newset_size = 
	  sizeof(weighted_result) + 
	  (sizeof(weighted_entry) * inset2->num_hits);
	newset = (weighted_result *) CALLOC(char, newset_size);
	memcpy((char *)newset,(char *)inset2,newset_size);
	return(newset);
      } else if (inset2 == NULL) {
	newset_size = 
	  sizeof(weighted_result) + 
	  (sizeof(weighted_entry) * inset1->num_hits);
	newset = (weighted_result *) CALLOC(char, newset_size);
	memcpy((char *)newset,(char *)inset1,newset_size);
	return(newset);
      }
      inset1->result_type |= FUZZY_REQUEST;
      inset2->result_type |= FUZZY_REQUEST;
      return(se_rank_docs(se_andmerge (inset1, inset2)));      
      break;
    case 2:
      /* fuzzy OR */
      if (inset1 == NULL && inset2 != NULL) {
	newset_size = 
	  sizeof(weighted_result) + 
	  (sizeof(weighted_entry) * inset2->num_hits);
	newset = (weighted_result *) CALLOC(char, newset_size);
	memcpy((char *)newset,(char *)inset2,newset_size);
	return(newset);
      } else if (inset2 == NULL) {
	newset_size = 
	  sizeof(weighted_result) + 
	  (sizeof(weighted_entry) * inset1->num_hits);
	newset = (weighted_result *) CALLOC(char, newset_size);
	memcpy((char *)newset,(char *)inset1,newset_size);
	return(newset);
      }
      inset1->result_type |= FUZZY_REQUEST;
      inset2->result_type |= FUZZY_REQUEST;
      return(se_rank_docs(se_ormerge (inset1, inset2, 2)));
      break;
    case 3:
      /* fuzzy NOT */
      if (inset2 == NULL) {
	newset_size = 
	  sizeof(weighted_result) + 
	  (sizeof(weighted_entry) * inset1->num_hits);
	newset = (weighted_result *) CALLOC(char, newset_size);
	memcpy((char *)newset,(char *)inset1,newset_size);
	return(newset);
      }
      inset1->result_type |= FUZZY_REQUEST;
      inset2->result_type |= FUZZY_REQUEST;
      return(se_rank_docs(se_notmerge (inset1, inset2)));      
      break;
    case 4:
    case 5:
      if (inset1 == NULL || inset2 == NULL) 
	return(NULL);  
      /* fuzzy RESTRICT/LIMIT TO or FROM */
      if ((((inset1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT)
	   && ((inset2->result_type & COMPONENT_RESULT) != COMPONENT_RESULT))
	  || (((inset2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT)
	      && ((inset1->result_type & COMPONENT_RESULT) != COMPONENT_RESULT))) {
	return(se_rank_docs(se_component_doc_restrict(op, inset1, inset2))); 
      }
      else if ((inset1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT
	       && (inset2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
	return(se_rank_docs(se_component_sub_restrict(op, inset1, inset2)));
      }
      break;

    case 6: /* MERGE_SUM */
      return(se_rank_docs(se_ormerge (inset1, inset2, 1)));
      break;

    case 7: /* MERGE_MEAN */
      return(se_rank_docs(se_ormerge (inset1, inset2, 3)));
      break;

    case 8: /* MERGE_NORM */
      return(se_rank_docs(se_ormerge (inset1, inset2, 4)));
      break;

    case 9: /* MERGE_CMBZ */
      return(se_rank_docs(se_ormerge (inset1, inset2, 5)));
      break;

    case 10: /* MERGE_NSUM */
      return(se_rank_docs(se_ormerge (inset1, inset2, 6)));
      break;

    case 11: /* MERGE_NPRV */
      return(se_rank_docs(se_ormerge (inset1, inset2, 7)));
      break;

    case 12: /* MERGE_PIVOT -- handled above */
      return(se_rank_docs(se_pivot_doc_normalize(op, inset1, inset2))); 
      break;

    case 13: /* MERGE_HYBRID -- handled above */
      return(se_rank_docs(se_hybrid_merge_normalize(op, inset1, inset2))); 
      break;

    default:
      /* only support "known" unit codes */
      diagnostic_set(132,op->proximityUnitCode.u.private_var,"Private proximity code not supported"); /* Unsupported unit code */
      return (NULL);
    }
    
    return (newset);
  }

  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);


  /* check to see of both sides of the are proximity results */
  if (!((inset1->result_type & PROXIMITY_RESULT) == PROXIMITY_RESULT
	&& (inset2->result_type & PROXIMITY_RESULT) == PROXIMITY_RESULT)) {
    /* We support one other type of "proximity" which is subelement */
    /* restriction for component searches                           */
    if ((((inset1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT)
	 && ((inset2->result_type & COMPONENT_RESULT) != COMPONENT_RESULT))
	|| (((inset2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT)
	    && ((inset1->result_type & COMPONENT_RESULT) != COMPONENT_RESULT))) {
      return(se_component_doc_restrict(op, inset1, inset2)); 
    }
    else if ((inset1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT
	     && (inset2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
      return(se_component_sub_restrict(op, inset1, inset2));
    }
    else {
      diagnostic_set(201,0,""); /* unsupported index combination */
      return (NULL);
    }
  }

  if (op->relationType != 2) {
    /* only support less than or equal relation */
    diagnostic_set(131,op->relationType,""); /* unsupported proximity relation */
    return (NULL);  
  }
  
  set1 = inset1;
  set2 = inset2;
  
  /* check to see if either of the sets is from a paged search    */
  if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT
      && (set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    both_paged = 1;
  }
  else if ((set1->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 1;
    set1 = se_get_page_docs(inset1,&current_page_hash_table, 1);
  }
  else if ((set2->result_type & PAGED_RESULT) == PAGED_RESULT) {
    paged_flag = 2;
    set2 = se_get_page_docs(inset2,&current_page_hash_table, 2);
  }
  
  /* check to see if either of the sets is from a component search */
  if ((set1->result_type & COMPONENT_RESULT) == COMPONENT_RESULT
      && (set2->result_type & COMPONENT_RESULT) == COMPONENT_RESULT) {
    both_component = 1;
  }


  /* check to see if either of the sets is a probabilistic search */
  if (set1->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT)) {
    se_sortwtset(set1);
    prob_flag = (set1->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		    | CORI_RESULT |FUZZY_RESULT | BLOB_RESULT));
  }
  if (set2->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT)) {
    se_sortwtset(set2);
    prob_flag |= (set2->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		    | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT)); 
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
      
      /* This is now the Boolean AND of the entries. We now */
      /* do the proximity testing between these items       */
      
      if (ProxTest(op,set1,set2,newset->entries[i].record_num)) {
	/* XXXX */
	/* "weight" of record in merged set is 		*
	 * sum of weights of terms in two original sets *
	 * (Change formula based on relevance testing!)	*/
	if (prob_flag)
	  newset->entries[i].weight =
	    set1->entries[i1].weight + set2->entries[i2].weight;
	else
	  newset->entries[i].weight = 1.0;

	
	/* 
	 * This was an attempt to normalize -- but since 
	 * the probabilistic weights are higher that 1 sometimes,
	 * and the Booleans are now set to 1.0 ONLY ...
	 * if (prob_flag && newset->entries[i].weight >= 3.0)
	 *  newset->entries[i].weight = 
	 *  newset->entries[i].weight - 
	 *  (float)((int)newset->entries[i].weight - 2) ;
	 */

	i++;	/* increment all indices	*/  
      }
      /* since the record ids match, we increment, but unless the proxtest */
      /* succeeded, there will be nothing added to the result list         */
      /* Otherwise, this is the same as a Boolean AND                      */
      i1++;
      i2++;
    }
  }
  
  newset->num_hits = i;
  newset->setid[0] = '\0';
  newset->result_type = 0; /* default Boolean */

  strcpy(newset->filename, inset1->filename);

  /* add the termids */
  for (i = 0; set1->termid[i] != 0; i++)
    newset->termid[i] = set1->termid[i];

  /* merge the second list of termids */
  for (j = 0; set2->termid[j] != 0; j++) {

    for (i = 0; newset->termid[i] != 0; i++) {
      if (set2->termid[j] == newset->termid[i])
	break;
    }
    if (newset->termid[i] == 0)
      newset->termid[i] = set2->termid[j];
  }

  /* always set the proximity flag */
  newset->result_type |= PROXIMITY_RESULT;

  if (both_paged)
    newset->result_type |= PAGED_RESULT;
  if (paged_flag)
    newset->result_type |= PAGED_DOC_RESULT;

  if (prob_flag) {
    newset->result_type = prob_flag;
    return (se_rank_docs(newset));
  }
  else
    return(newset);
}



int 
se_prox_ordered(int num_prox_terms, int proxtermidlist[], 
	        DB *dbprox[], int record_num, int prox_length)
{
  
  DBT key, data;
  DBT proxkey, proxdata;
  DB *db_prox;
  int **proxlist;
  int *listsize;
  int *matchpos;
  int numints;
  int result = 0;
  int minpos;
  int fail;
  int returncode;
  int listnum, list, i, j;

  prox_idx_pos_key poskeyval;


  if (dbprox == NULL)
    return 0;

  /* allocate an array of pointers for the actual prox lists for each term */
  proxlist = CALLOC(int *, num_prox_terms + 1);
  listsize = CALLOC(int, num_prox_terms +1);
  matchpos = CALLOC(int, num_prox_terms +1);

  /* Initialize the prox key/data pair so the flags aren't set. */
  for (listnum = 0; listnum < num_prox_terms; listnum++) {
    
    memset(&proxkey, 0, sizeof(proxkey));
    memset(&proxdata, 0, sizeof(proxdata));
    
    poskeyval.recnum = record_num;
    poskeyval.termid = proxtermidlist[listnum];
    
    proxkey.data = (void *) &poskeyval;
    proxkey.size = sizeof( prox_idx_pos_key);;

    returncode = dbprox[listnum]->get(dbprox[listnum], NULL, &proxkey, &proxdata, 0); 
	  
    if (returncode == 0) { /* found the word in the index */
      numints = proxdata.size / sizeof(int);
      proxlist[listnum] = CALLOC(int, numints + 1);
      listsize[listnum] = numints;
      /* get the existing data (Move to aligned space) */
      memcpy(proxlist[listnum], (int *)proxdata.data, proxdata.size);
    }
  }
  /* we now have each of the lists for the term proximity info */
  /* we process the lists in order so the match is in order    */
  
  for (i = 0; i < listsize[0]; i++) {
    /* set an anchor */
    minpos = proxlist[0][i];
    matchpos[0] = minpos;
    fail = 0;

    for(list = 1; list < num_prox_terms; list++) {
      for (j = 0; j < listsize[list]; j++) {
	if (proxlist[list][j] < minpos) {
	  fail = 1; /* set in case we run out of data */
	  continue;
	}
	else if ((proxlist[list][j] - minpos)
		 > prox_length) {
	  /* the "following" term too far past the current anchor */
	  fail = 1;
	  break;
	} else if (proxlist[list][j] == minpos && 
		   proxtermidlist[j] == proxtermidlist[j-1]) {
	  /* looking for doubled terms -- try the next item */
	  fail = 1;
	  continue;
	} else { 
	  /* it is in the appropriate range, so set a match and */
	  /* go on                                              */
	  fail = 0;
	  matchpos[list] = proxlist[list][j];
	  minpos = proxlist[list][j];
	  break;
	}
      }
      if (fail)
	break;
    }
    /* if any one of the term lists doesn't match we go on and try */
    /* a new anchor */
    if (fail == 0) {
      /* do the fine test... */
      result = 1; /* we think it is ok... */
      for (j = 1; j < num_prox_terms; j++) {
	/* this is fuzzy and may fail if there is a lot of whitespace */
	if (((matchpos[j]-matchpos[j-1])) <= prox_length)
	  continue;
	result = 0;
      }
    }
    if (result == 1) {
      result = matchpos[0]; /* return the start position of the match */
      break;
    }
  }
  
  /* clean up */
  for (listnum = 0; listnum < num_prox_terms; listnum++) 
    FREE(proxlist[listnum]);
  FREE (proxlist);
  FREE (listsize);

  return (result);

}



int 
se_prox_unordered(int num_prox_terms, int proxtermidlist[], 
		  DB *dbprox[], int record_num, int prox_length)
{
  
  DBT key, data;
  DBT proxkey, proxdata;
  DB  *db_prox;
  int **proxlist;
  int *listsize;
  int *matchpos;
  int numints;
  int result = 0;
  int minpos;
  int fail;
  int returncode;
  int listnum, list, i, j;

  prox_idx_pos_key poskeyval;


  if (dbprox == NULL)
    return 0;

  /* allocate an array of pointers for the actual prox lists for each term */
  proxlist = CALLOC(int *, num_prox_terms + 1);
  listsize = CALLOC(int, num_prox_terms +1);
  matchpos = CALLOC(int, num_prox_terms +1);

  /* Initialize the prox key/data pair so the flags aren't set. */
  for (listnum = 0; listnum < num_prox_terms; listnum++) {
    
    memset(&proxkey, 0, sizeof(proxkey));
    memset(&proxdata, 0, sizeof(proxdata));
    
    poskeyval.recnum = record_num;
    poskeyval.termid = proxtermidlist[listnum];
    
    proxkey.data = (void *) &poskeyval;
    proxkey.size = sizeof( prox_idx_pos_key);;

    returncode = dbprox[listnum]->get(dbprox[listnum], NULL, &proxkey, &proxdata, 0); 
	  
    if (returncode == 0) { /* found the word in the index */
      numints = proxdata.size / sizeof(int);
      proxlist[listnum] = CALLOC(int, numints + 1);
      listsize[listnum] = numints;
      /* get the existing data (Move to aligned space) */
      memcpy(proxlist[listnum], (int *)proxdata.data, proxdata.size);
    }
  }
  /* we now have each of the lists for the term proximity info */
  /* we process the lists in order so the match is in order    */
  
  for (i = 0; i < listsize[0]; i++) {
    /* set an anchor */
    minpos = proxlist[0][i];
    matchpos[0] = minpos;
    fail = 0;
    
    for(list = 1; list < num_prox_terms; list++) {
      for (j = 0; j < listsize[list]; j++) {
	if (proxlist[list][j] < minpos && 
	    (minpos - proxlist[list][j]) > prox_length) {
	  fail = 1; /* set in case we run out of data */
	  continue;
	}
	else if ((proxlist[list][j] - minpos) > prox_length) {
	  /* the "following" term too far past the current anchor */
	  fail = 1;
	  break;
	} else if (proxlist[list][j] == minpos && 
		   proxtermidlist[j] == proxtermidlist[j-1]) {
	  /* looking for doubled terms -- try the next item */
	  fail = 1;
	  continue;
	} else { 
	  /* it is in the appropriate range, so set a match and */
	  /* go on                                              */
	  fail = 0;
	  matchpos[list] = proxlist[list][j];
	  minpos = proxlist[list][j];
	  break;
	}
      }
      if (fail)
	break;
    }
    
    if (fail == 0) {
      /* do the fine test... */
      result = 1; /* we think it is ok... */
      for (j = 1; j < num_prox_terms; j++) {
	/* this is fuzzy and may fail if there is a lot of whitespace */
	if (((matchpos[j] > matchpos[j-1]) 
	     && (matchpos[j]-matchpos[j-1]) <= prox_length)
	    || ((matchpos[j-1] > matchpos[j])
		&& (matchpos[j-1]-matchpos[j]) <= prox_length))
	  continue;
	result = 0;
      }
    }
    if (result == 1) {
      result = matchpos[0]; /* return the start position of the match */
      break;
    }
  }
  
  /* clean up */
  for (listnum = 0; listnum < num_prox_terms; listnum++) 
    FREE(proxlist[listnum]);
  FREE (proxlist);
  FREE (listsize);
  
  return (result);
  
}


/* this is the primary testing for proximity */
int
ProxTest(ProximityOperator *op, weighted_result *inset1, 
	 weighted_result *inset2, int rec_num)
{

  int i, n;
  int terms[200];
  DB *proxdb[200];
  int result;
  int distance;

  n = 0;

  for (i = 0; inset1->termid[i] != 0; i++) {
    terms[n] = inset1->termid[i];
    if (inset1->index_ptr[i] != NULL)
      proxdb[n++] = inset1->index_ptr[i]->prox_db;
    else {
      /* otherwise we basically ignore this term */
      terms[n] = 0;
    }
  }

  for (i = 0; inset2->termid[i] != 0; i++) {
    terms[n] = inset2->termid[i];
    if (inset2->index_ptr[i] != NULL)
      proxdb[n++] = inset2->index_ptr[i]->prox_db;
    else {
      /* otherwise we basically ignore this term */
      terms[n] = 0;
    }
  }
  /* null out the n-th entry (end of list) */
  terms[n] = 0;
  proxdb[n] = NULL;

  
  /* we now have a list of terms and (open) proximity databases */

  /* All of the main errors should have already been trapped or handled */

  switch (op->proximityUnitCode.u.known) {
  case 1: /*characters*/
  case 11: /* byte */
    /* We do a character distance match for the words */
    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, op->distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, op->distance);
      return(result);
    }
    break;
  case 2: /* words */
    /* We do a character distance match for the words */
    /* approximate word length */
    distance = 6*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 3: /* sentence */
    /* We do a character distance match for the words */
    /* approximate word length */
    distance = 80*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 4: /* paragraph */
    /* We do a character distance match for the words */
    /* approximate word length */
    distance = 300*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 5: /* section */
    /* We do a character distance match for the words */
    /* approximate word length */
    distance = 1000*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 6: /* chapter */
    /* We do a character distance match for the words */
    /* approximate word length */
    distance = 3000*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 7: /* document */
    /* this is basically already tested by the AND */
    /* but we check for EXCLUSION and ORDER        */
    if (op->exclusion == 0 && op->ordered == 0)
      return (1);
    else if (op->exclusion == 1 && op->ordered == 0)
      return (0);
    else if (op->ordered) {
      /* have to check the positions of the set members */
      distance = 1000000;
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;
  
  case 8: /* element  -- more fudging */
    distance = 40*op->distance;

    if (op->ordered) {
      result = se_prox_ordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);

    }
    else {
      result = se_prox_unordered(n, terms, proxdb, 
			       rec_num, distance);
      return(result);
    }
    break;

  case 9: /* subelement -- eventually will use this for proximity of component info */
  case 10: /* elementType */
    return (0);
    break;

  default: /* fail */
    return (0);
  }

}







