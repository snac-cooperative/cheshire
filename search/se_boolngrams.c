
/*
 *  Copyright (c) 1990-2012 [see Other Notes, below]. The Regents of the
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
*	Header Name:	se_bool_ngrams.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	boolean search processing for a single ngram index
*                       element of the cheshire catalog
*
*	Usage:		se_bool_ngrams (index_struct, filename, 
*                                 term, relation,
*                                 position, structure, truncation, 
*                                 completeness)
*
*	Variables:	index_struct is an idx_list_entry for the index
*                       term is the word, phrase or wordlist to be searched
*                       the relation, position, structure, truncation, 
*                        and completeness variables are the z39.50 attributes
*                        specified in the query.
*
*	Return Conditions and Return Codes:	
*                        returns a weighted list on success, and NULL on
*                        failure. Note that zero hits is NOT failure, and
*                        returns a list with num_hits == 0.
*
*	Date:		02/08/12
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2012.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include <stdlib.h>
#include <string.h>

void diagnostic_set();

/*forward declaration */
extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern char *
normalize_key(char *raw_word, idx_list_entry *idx, int *morphflag, int dia_flag);

extern weighted_result *se_rank_docs(weighted_result *src);



weighted_result *
se_boolngrams (idx_list_entry *idx,  char *filename, char *insearchstring, 
		 int relation, int position, 
		 int structure, int truncation, 
		 int completeness, int attr_nums[],
		 char *attr_str[],  ObjectIdentifier attr_oid[])
{
  
  char *word_ptr;
  
  int i, exists;
  int  tot_docs;
  int record_num;
  int num_search_words = 0;
  int num_found_terms = 0;
  int num_ngrams_found = 0;
  int total_query_ngrams = 0;
  int search_stopword_count = 0;
  int morphed;
  int ngram_size = 3;
  int last_stop_count = 0;
  int termidlist[100]; 
  int base_pool_num = 2;
  int base_pool;

  struct temp_results {
    int weight;
    int tot_freq;
  } *storage ;

  char *searchstring = NULL;

  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  idx_result *index_result;
  int NGRAM_FULL  = 0;

  weighted_result *wt_res=NULL, *se_sortwtset();

  char *breakletters;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char ngram[10] = "";
  char *lastptr, *strtok_r();

  /* we reuse snowball_stem_type to store ngram size from config */
  if (idx->snowball_stem_type > 1)  {
    /* currently the only options are 3, 4, and 5 */
    ngram_size = idx->snowball_stem_type;
  }

  if (relation == 580) { 
    NGRAM_FULL = 1;
    base_pool_num = 500;
  }

  searchstring = strdup(insearchstring);

  search_stopword_count = 0;
  breakletters = keywordletters;

  /* Allocate the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
  
  /* find first token */
  word_ptr = strtok_r (searchstring, breakletters, &lastptr);

  if (word_ptr == NULL) { /* no words in search string */
    FREE(searchstring);
    return (NULL);
  }


  do {
    char   *result_word;
    int result_len;
    int i;
    int j;
    int k;
    

    base_pool = base_pool_num;


    result_word = normalize_key(word_ptr, idx, &morphed, 0);
    num_search_words++;    
    
    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL) {
      
      result_len = strlen(result_word);
      
      for (k=-1; k < result_len; k++) {	
	
	/* now we make the ngrams */
	if (k == -1) {
	  /* create a special start of word ngram */
	  ngram[0] = ' '; 
	  j = 1;
	}
	else j = 0;
	
	for (; j < ngram_size; j++) {
	  if (result_word[j+k] == '\0') {
	    ngram[j] = ' ';
	  }
	  else 
	    ngram[j] = result_word[j+k];
	}
	ngram[j] = '\0';
	

	if (strcmp(ngram," 19") == 0)
	  continue;
	if (strcmp(ngram," 18") == 0)
	  continue;
	if (strcmp(ngram," gt") == 0)
	  continue;
	if (strcmp(ngram,"gt ") == 0)
	  continue;
	if (strcmp(ngram," lt") == 0)
	  continue;
	if (strcmp(ngram,"lt ") == 0)
	  continue;

	if (base_pool == base_pool_num && result_len == 1) {
	  base_pool = 0;
	}


	if ((k+j) <= result_len + 1) {

	  total_query_ngrams++;

	  index_result = se_getterm_idx(idx, ngram, 0/*normalize*/,
					&search_stopword_count);
	  
	  if (index_result != NULL) {

	    num_ngrams_found++;

	    if (num_found_terms < 100) {
	      termidlist[num_found_terms] = index_result->termid;
	      num_found_terms++;
	    }

	    for (i = 0; i < index_result->num_entries; i++) {
	      
	      record_num = index_result->entries[i].record_no;
	      if (record_num > 0) { /* negative is a deleted entry */
		entry = Tcl_FindHashEntry(hash_tab, (void *)record_num);
		
		if (entry == NULL && base_pool > 0) { /* new record number */
		  storage = CALLOC(struct temp_results,1);
		  storage->weight = 1.0;
		  storage->tot_freq = index_result->entries[i].term_freq;
		  Tcl_SetHashValue(
				   Tcl_CreateHashEntry(
						       hash_tab,
						       (void *)record_num,
						       &exists),
				   (ClientData)storage);
		}
		else if (entry != NULL) {
		  storage = (struct temp_results *) Tcl_GetHashValue(entry);
		  storage->weight += 1.0;
		  storage->tot_freq += index_result->entries[i].term_freq;
		}
	      }
	    }
	    /* done with the index results from the getterm call */	
	    FREE(index_result);
	    base_pool--;
	    
	  } /* if not null index_result... */
	}
      }
    }
    else {
      search_stopword_count++;
    }
    
    /* get the next word */
    /*FREE(result_word); */
    word_ptr = strtok_r (NULL, breakletters, &lastptr);
  } while (word_ptr != NULL);
    
  tot_docs = hash_tab->numEntries;

  if ((num_search_words - search_stopword_count) <= 0) {
    /* all search words were stopwords */
    diagnostic_set(4,0,NULL); 
    FREE(searchstring);
    return(NULL);
  }
  
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + ((tot_docs+1) * sizeof(weighted_entry))));
    
  wt_res->num_hits = 0;
  wt_res->setid[0] = '\0';
  wt_res->max_weight = 1.0;
  wt_res->min_weight = 1.0;
  
  for (i = 0; i < num_found_terms; i++) {
    wt_res->termid[i] = termidlist[i];
    wt_res->index_ptr[i] = idx;
  }
  i = 0;
  
  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
    float tempwt;
    
    record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
    storage = (struct temp_results *) Tcl_GetHashValue(entry);
    tempwt = (float)storage->weight / (float)total_query_ngrams;

    if (tempwt > 0.5) {
      wt_res->entries[i].record_num = record_num;
      wt_res->num_hits++;
      wt_res->entries[i].weight = (float)storage->weight / (float)total_query_ngrams;
      if (wt_res->entries[i].weight > wt_res->max_weight) 
	wt_res->max_weight =  wt_res->entries[i].weight;

      if (wt_res->entries[i].weight < wt_res->min_weight) 
	wt_res->min_weight =  wt_res->entries[i].weight;

      /* wt_res->entries[i].weight = (float)storage->tot_freq; */
      i++;
    }
    FREE(storage); /* not needed any more */
  }
  
  Tcl_DeleteHashTable(hash_tab); /* clean up */
  
  FREE(searchstring);

  return(se_rank_docs(wt_res));
  
}







