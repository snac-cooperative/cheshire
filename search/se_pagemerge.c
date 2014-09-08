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
*       Header Name:    se_pagemerge.c
*
*       Programmer:     Ray R. Larson
*
*       Purpose:        Merges two weighted result sets of paged docs with
*                       their associated hash tables.
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
*       Date:           11/10/96
*       Revised:        3/21/97
*       Version:        1.000001
*       Copyright (c) 1997.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include <cheshire.h>

/* external functions */
weighted_result *se_rank_docs(weighted_result *newset);
weighted_result *se_get_page_docs(weighted_result *newset, Tcl_HashTable **ht,
				  int idnum);
weighted_result *se_andmerge (weighted_result *set1, weighted_result *set2);


weighted_result  *se_pagemerge (int maxresults, weighted_result *inset1, 
				weighted_result *inset2, Tcl_HashTable **comb) 
{

  int i = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int set1_flag = 0, set2_flag = 0;
  int total_postings, newset_size, exists;
  weighted_result *set1, *set2, *newset; /* pointer to merged set */
  weighted_result *merged_concepts_AND, *merged_docs_AND;  
  Tcl_HashTable *merghash, *ht1, *ht2;
  Tcl_HashEntry *entry_merge, *entry1, *entry2;
  Tcl_HashSearch hash_search;
  int merg_recnum, merg_pages;

  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage, *tmp;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts_merge, *doc_wts_set1, *doc_wts_set2;



  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);
  
  /* check to see both  of the sets is from a paged search    */
  if ((inset1->result_type & PAGED_RESULT) != PAGED_RESULT
      || (inset2->result_type & PAGED_RESULT) != PAGED_RESULT) 
    return (NULL);

  merged_concepts_AND = se_andmerge(inset1, inset2);

  if (merged_concepts_AND->num_hits == 0) {
    return NULL;
  }    

  merged_docs_AND = se_get_page_docs(merged_concepts_AND, &merghash, 0);

  set1 = se_get_page_docs(inset1,&ht1, 1);
  set2 = se_get_page_docs(inset2,&ht2, 2);

  if (set1 == NULL || set2 == NULL) {
    printf ("null from getting page docs in se_pagemerge.c\n");
  }


  /* merge the individual page info for those items that are in the */
  /* ANDed merged list                                              */
  for (entry_merge = Tcl_FirstHashEntry(merghash,&hash_search);
       entry_merge != NULL; entry_merge = Tcl_NextHashEntry(&hash_search)) {

    merg_recnum = (int)Tcl_GetHashKey(merghash,entry_merge);
    doc_wts_merge = (struct docsum *) Tcl_GetHashValue(entry_merge);
    merg_pages = doc_wts_merge->num_pages;

    /* find a matching docnum in set 1 */
    entry1 = Tcl_FindHashEntry(ht1, (void *)merg_recnum);
    if (entry1 != NULL) {
      /* add the data to the merged entry and eliminate the old entry */
      doc_wts_set1 = (struct docsum *) Tcl_GetHashValue(entry1);
      doc_wts_merge->num_pages = doc_wts_set1->num_pages;
      doc_wts_merge->sum_wts += doc_wts_set1->sum_wts;
      doc_wts_merge->lastpage->next = doc_wts_set1->page_ptr;
      doc_wts_merge->lastpage =  doc_wts_set1->lastpage;
      doc_wts_set1->page_ptr = NULL; /* this disconnects the page data */
    }

    /* find a matching docnum in set 1 */
    entry2 = Tcl_FindHashEntry(ht2, (void *)merg_recnum);
    if (entry2 != NULL) {
      /* add the data to the merged entry and eliminate the old entry */
      doc_wts_set2 = (struct docsum *) Tcl_GetHashValue(entry2);
      doc_wts_merge->num_pages += (doc_wts_set2->num_pages - merg_pages);
      doc_wts_merge->sum_wts += doc_wts_set2->sum_wts;
      doc_wts_merge->lastpage->next = doc_wts_set2->page_ptr;
      doc_wts_merge->lastpage =  doc_wts_set2->lastpage;
      doc_wts_set2->page_ptr = NULL; /* this disconnects the page data */
    }
   
  }

  if (merged_docs_AND->num_hits >= maxresults) {
    /* no need to get very fancy, there are already enough hits */
    free_page_hash_tab(ht1);
    free_page_hash_tab(ht2);
    *comb = merghash;
    return (merged_docs_AND);
  }

  
  /* find size of largest possible merged set, allocate memory */
  total_postings = set1->num_hits + set2->num_hits + 1;
  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * (maxresults + 1));
  newset = (weighted_result *) CALLOC(char, newset_size);

  *newset = *merged_docs_AND;
  for (i = 0; i < merged_docs_AND->num_hits; i++) {
    newset->entries[i] = merged_docs_AND->entries[i];
  }  
  
  /* The sets should be in order by weight, so we will take the highest */
  /* weights and merge the existing data into newset                    */

  while ((i1 < set1->num_hits) && (i2 < set2->num_hits) && (i < maxresults)) {
    
    set1_flag = set2_flag = 0;
    
    if (set1->entries[i1].weight > set2->entries[i2].record_num) {
      merg_recnum = set1->entries[i1].record_num;
      set1_flag = 1;
    }
    else {
      merg_recnum = set2->entries[i2].record_num;
      set2_flag = 1;
    }
    
    /* is there already an entry for the record in the merged list? */
    entry_merge = Tcl_FindHashEntry(merghash, (void *)merg_recnum);
    
    if (entry_merge == NULL) { /* no existing entry in merged table */
      if (set1_flag) {
	newset->entries[i] = set1->entries[i1];
	newset->entries[i++].weight =
	  set1->entries[i1++].weight;
      }
      else {
	newset->entries[i] = set2->entries[i2];
	newset->entries[i++].weight =
	  set2->entries[i2++].weight;
      }      
      /* add a new entry to the merged hash table */
      doc_wts_merge = CALLOC(struct docsum,1);
      
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   merghash,
					   (void *)merg_recnum,
					   &exists),
		       (ClientData)doc_wts_merge);
      
      /* find a matching docnum in set 1 */
      entry1 = Tcl_FindHashEntry(ht1, (void *)merg_recnum);
      if (entry1 != NULL) {
	/* add the data to the merged entry and eliminate the old entry */
	doc_wts_set1 = (struct docsum *) Tcl_GetHashValue(entry1);
	if (doc_wts_set1->page_ptr != NULL) { 
	  /* add data for pages not already merged */
	  doc_wts_merge->num_pages += doc_wts_set1->num_pages;
	  doc_wts_merge->sum_wts += doc_wts_set1->sum_wts;
	  if (doc_wts_merge->page_ptr == NULL)
	    doc_wts_merge->page_ptr = doc_wts_set1->page_ptr;
	  else 
	    doc_wts_merge->lastpage->next = doc_wts_set1->page_ptr;
	  doc_wts_merge->lastpage =  doc_wts_set1->lastpage;
	  doc_wts_set1->page_ptr = NULL; /* this disconnects the page data */
	}
      }
      
      /* find a matching docnum in set 2 */
      entry2 = Tcl_FindHashEntry(ht2, (void *)merg_recnum);
      if (entry2 != NULL) {
	/* add the data to the merged entry and eliminate the old entry */
	doc_wts_set2 = (struct docsum *) Tcl_GetHashValue(entry2);
	if (doc_wts_set2->page_ptr != NULL) { 
	  /* add data for pages not already merged */
	  doc_wts_merge->num_pages += doc_wts_set2->num_pages;
	  doc_wts_merge->sum_wts += doc_wts_set2->sum_wts;
	  if (doc_wts_merge->page_ptr == NULL)
	    doc_wts_merge->page_ptr = doc_wts_set2->page_ptr;
	  else 
	    doc_wts_merge->lastpage->next = doc_wts_set2->page_ptr;
	  doc_wts_merge->lastpage =  doc_wts_set2->lastpage;
	  doc_wts_set2->page_ptr = NULL; /* this disconnects the page data */
	}
      }
    }
    else {  /* there is already an entry in merged table */
      if (set1_flag)
	i1++;
      else
	i2++;

    }
  }
  
  if (i < maxresults) {
  /* Load remainder of longer array of entries.		*
   * (Only one of these while loops executes.)		*/
    while (i1 < set1->num_hits) {

      merg_recnum = set1->entries[i1].record_num;
      
      /* is there already an entry? */
      entry_merge = Tcl_FindHashEntry(merghash, (void *)merg_recnum);
    
      if (entry_merge == NULL) { /* no existing entry in merged table */
	/* find a matching docnum in set 1 */
	entry1 = Tcl_FindHashEntry(ht1, (void *)merg_recnum);
      
	if (entry1 != NULL) {
	  doc_wts_set1 = (struct docsum *) Tcl_GetHashValue(entry1);
	  newset->entries[i++] = set1->entries[i1++];
	
	  /* add a new entry to the merged hash table */
	  doc_wts_merge = CALLOC(struct docsum,1);
      
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       merghash,
					       (void *)merg_recnum,
					       &exists),
			   (ClientData)doc_wts_merge);
	  
	  /* add the data to the merged entry and eliminate the old entry */
	  *doc_wts_merge = *doc_wts_set1;
	  doc_wts_set1->page_ptr = NULL; /* this disconnects the page data */
	
	}
      }
      else {      /* otherwise there is already an entry */
	i1++;
      }
    }

    while (i2 < set2->num_hits) {
      merg_recnum = set2->entries[i2].record_num;
      /* is there already an entry? */
      entry_merge = Tcl_FindHashEntry(merghash, (void *)merg_recnum);
    
      if (entry_merge == NULL) { /* no existing entry in merged table */
	/* find a matching docnum in set 2 */
	entry2 = Tcl_FindHashEntry(ht2, (void *)merg_recnum);
      
	if (entry2 != NULL) {
	  newset->entries[i++] = set2->entries[i2++];
	  /* add a new entry to the merged hash table */
	  doc_wts_merge = CALLOC(struct docsum,1);
      
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       merghash,
					       (void *)merg_recnum,
					       &exists),
			   (ClientData)doc_wts_merge);
	  

	  *doc_wts_merge = *doc_wts_set2;
	  doc_wts_set2->page_ptr = NULL; /* this disconnects the page data */
	}
      }
      else {
	i2++;
      }      
    }
  }

  newset->num_hits = i;
  newset->setid[0] = '\0';
  strcpy(newset->filename, inset1->filename);
  newset->result_type = PAGED_DOC_RESULT | PROBABILISTIC_RESULT;
  *comb = merghash;
  return (se_rank_docs(newset));

}


