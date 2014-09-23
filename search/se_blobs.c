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
*	Header Name:	se_blobs.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:        search processing for BlobWorld image searching
*
*	Usage:		se_blobs (index_struct, terms);
*
*	Variables:	index_struct is an idx_list_entry for the index
*                       term is list of blob "bin" tokens to search
*
*	Return Conditions and Return Codes:	
*                        returns a weighted list on success, and NULL on
*                        failure. Note that zero hits is NOT failure, and
*                        returns a list with num_hits == 0.
*
*	Date:		8/5/99
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1999.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"

void diagnostic_set();

extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);

extern weighted_result *se_rank_docs(weighted_result *src);


weighted_result *se_blobs (idx_list_entry *idx,  char *searchstring)
{
  
  char *word_ptr;
  
  int i, exists;
  int  tot_docs;
  int record_num;
  int num_search_words = 0;
  int max_hits = 0;
  int num_found_terms = 0;
  int search_stopword_count;

  int termidlist[8000]; 

  struct temp_results {
    int weight;
    int tot_freq;
  } *storage ;
  
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  idx_result *index_result;

  weighted_result *wt_res, *se_sortwtset();

  char *breakletters;
  char *keywordletters = " \t\n`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n<>";
  char *filenameletters = " \t\n<>";

  char *lastptr;

  if (searchstring == NULL) return (NULL);
  
  search_stopword_count = 0;


  if (idx->type & URL_KEY) breakletters = urlletters;
  else if (idx->type & FILENAME_KEY) breakletters = filenameletters;
  else breakletters = keywordletters; 


  /* Allocate the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);

  /* find first token */
  word_ptr = strtok_r (searchstring, breakletters, &lastptr);
  if (word_ptr == NULL) { /* no words in search string */
    return (NULL);
  }
  else { /* process each word */
      
    do {

      num_search_words++;
	
      index_result = se_getterm_idx(idx, word_ptr, 1 /* normalize*/,
				    &search_stopword_count);
	
      if (index_result != NULL) {

	  if (num_found_terms < 7999)
	    termidlist[num_found_terms++] = index_result->termid;

	  for (i = 0; i < index_result->num_entries; i++) {
	    
	    /* we will be anding these, so need to know the max size */
	    if (index_result->num_entries > max_hits)
	      max_hits = index_result->num_entries;

	    record_num = index_result->entries[i].record_no;
	    if (record_num > 0) { /* negative is a deleted entry */
	      entry = Tcl_FindHashEntry(hash_tab, (void *)record_num);
		
	      if (entry == NULL){ /* new record number */
		storage = CALLOC(struct temp_results,1);
		storage->weight = 1;
		storage->tot_freq = index_result->entries[i].term_freq;
		Tcl_SetHashValue(
				 Tcl_CreateHashEntry(
						     hash_tab,
						     (void *)record_num,
						     &exists),
				 (ClientData)storage);
	      }
	      else {
		storage = (struct temp_results *) Tcl_GetHashValue(entry);
		storage->weight++;
		storage->tot_freq += index_result->entries[i].term_freq;
	      }
	    }
	  }
	  /* done with the index results from the getterm call */	
	  if (index_result->term)
	    FREE(index_result->term);
	  FREE(index_result);
	} /* if not null index_result... */

	/* get the next word */
	word_ptr = strtok_r (NULL, breakletters, &lastptr);
      } while (word_ptr != NULL);
    
      tot_docs = hash_tab->numEntries;
    }

  /* we now have a hash table that has all of the blobs in it... */
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + (tot_docs * sizeof(weighted_entry))));
    
    wt_res->num_hits = 0;
    wt_res->setid[0] = '\0';
    wt_res->result_type |= BLOB_RESULT;

    if (idx->type & COMPONENT_INDEX) {
      wt_res->result_type |= COMPONENT_RESULT;
      wt_res->component = idx->comp_parent;
    }                                                                             
    if (idx->type & PROXIMITY) {
      wt_res->result_type |= PROXIMITY_RESULT;
    }

    if (idx->type & PAGEDINDEX) {
      wt_res->result_type |= PAGED_RESULT;
    }

    for (i = 0; i < num_found_terms; i++)
      wt_res->termid[i] = termidlist[i];

    i = 0;

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
	 entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

      record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
      storage = (struct temp_results *) Tcl_GetHashValue(entry);

      if (storage->weight >= (num_search_words / 4)) {
	wt_res->entries[i].record_num = record_num;
	wt_res->num_hits++;
	wt_res->entries[i].weight = (float)storage->tot_freq;

	if (wt_res->max_weight == 0.0) 
	  wt_res->max_weight = wt_res->entries[i].weight;
	else if (	wt_res->max_weight < wt_res->entries[i].weight)
	  wt_res->max_weight = wt_res->entries[i].weight;
	if (wt_res->min_weight == 0.0) 
	  wt_res->min_weight = wt_res->entries[i].weight;
	else if (wt_res->min_weight > wt_res->entries[i].weight)
	  wt_res->min_weight = wt_res->entries[i].weight;
	i++;
      }
      free(storage); /* not needed any more */
    }

    Tcl_DeleteHashTable(hash_tab); /* clean up */
  
  return(se_rank_docs(wt_res));
  /* BOOL return(se_sortwtset(wt_res)); */
}







