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
*	Header Name:	se_bool_element.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	boolean search processing for a single index
*                       element of the cheshire catalog
*
*	Usage:		se_bool_element (index_struct, filename, 
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
*	Date:		12/12/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include <stdlib.h>
#include <string.h>

void diagnostic_set();

/*forward declaration */
extern weighted_result *se_phrase_match (idx_list_entry *idx,  char *searchstring, 
				  int relation, int position, 
				  int structure, int truncation, 
				  int completeness);


extern weighted_result *se_bool_range (idx_list_entry *idx,  char *searchstring, 
				int relation, int position, 
				int structure, int truncation, 
				int completeness, int attr_nums[],
				char *attr_str[], ObjectIdentifier attr_oid[]);


extern int se_prox_phrase(DB *dbprox, int num_prox_terms, int proxtermidlist[], 
		   int proxtermlength[], int record_num, 
		   int phrase_prox_length);


extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);


extern weighted_result *se_bitmap(idx_list_entry *idx,  char *filename, 
				  char *insearchstring, 
				  int relation, int position, 
				  int structure, int truncation, 
				  int completeness, int attr_nums[],
				  char *attr_str[],  
				  ObjectIdentifier attr_oid[]);


extern weighted_result *se_boolngrams(idx_list_entry *idx,  char *filename, 
				  char *insearchstring, 
				  int relation, int position, 
				  int structure, int truncation, 
				  int completeness, int attr_nums[],
				  char *attr_str[],  
				  ObjectIdentifier attr_oid[]);




weighted_result *
se_bool_element (idx_list_entry *idx,  char *filename, char *insearchstring, 
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
  int max_hits = 0;
  int num_found_terms = 0;
  int phrase_prox_matching = 0;
  int phrase_prox_length = 0;
  int prox_term_length = 0;
  int num_prox_terms = 0;
  int last_phrase_prox_term = 0;
  char *dollar;
  int first_term;
  int search_stopword_count = 0;
  int last_stop_count = 0;
  int termidlist[100]; 
  int proxtermidlist[100]; 
  int proxtermlength[100];

  struct temp_results {
    int weight;
    int tot_freq;
  } *storage ;

  char *searchstring = NULL;

  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  idx_result *index_result;

  weighted_result *wt_res=NULL, *se_sortwtset();

  char *breakletters;
  char *keywordletters = " \t\n`~!@$%^&*()_=|\\{[]};:'\",<>?/";
  char *keywordphraseletters = " \t\n`~!@%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n<>";
  char *filenameletters = " \t\n<>";

  char *lastptr;


  /* for bitmap indexes, use se_bitmap */
  if (idx->type & BITMAPPED_TYPE) {
    return (se_bitmap(idx, filename, insearchstring, relation, position, 
		      structure, truncation, completeness, attr_nums,
		      attr_str, attr_oid));
  }



  /* for ngram indexes, use se_boolngrams */
  if (idx->type2 & NGRAMS) {
    return (se_boolngrams(idx, filename, insearchstring, relation, position, 
		      structure, truncation, completeness, attr_nums,
		      attr_str, attr_oid));
  }

  
  /* we make a copy because the processing is destructive and if a virtual */
  /* query is being used, it will mess up secondary searches to clobber the */
  /* the original query */
  if (insearchstring == NULL) 
    return (NULL);
  else
    searchstring = strdup(insearchstring);

  search_stopword_count = 0;


  if (relation != 0 && relation != 3 && relation != 6) {
    wt_res = se_bool_range (idx, searchstring, 
			    relation, position, 
			    structure, truncation, 
			    completeness, attr_nums,
			    attr_str, attr_oid);
    FREE(searchstring);
    return(wt_res);
  }
  
  if (idx->type & KEYWORD) breakletters = keywordletters; 
  if ((idx->type & (KEYWORD | PROXIMITY)) == (KEYWORD | PROXIMITY))
      breakletters = keywordphraseletters; 
  if (idx->type & URL_KEY) breakletters = urlletters;
  if (idx->type & FILENAME_KEY) breakletters = filenameletters;

  /* processes exactkeys as phrases by default */
  if ((idx->type & EXACTKEY) && structure == 0) {
    structure = PHRASE;
    if ((idx->type & PROXIMITY) == PROXIMITY)
      truncation = 100; /* do not truncate */
  }

  if (structure == WORD_LIST || 
      (structure == 0 && 
       (idx->type & KEYWORD)
       || (idx->type & URL_KEY)
       || (idx->type & FILENAME_KEY))) { 
    /* handle each word in the list */
    
    /* Allocate the hash table for collecting weights for each document */
    hash_tab = &hash_tab_data;
    Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
    
    /* find first token */
    word_ptr = strtok_r (searchstring, breakletters, &lastptr);
    if (word_ptr == NULL) { /* no words in search string */
      FREE(searchstring);
      return (NULL);
    }
    else { /* process each word */
      first_term = 1;
      do {
	
	num_search_words++;
	
	if ( (dollar = strchr(word_ptr,'$')) != NULL 
	     && phrase_prox_matching == 0) {
	  /* we are processing a within-index quoted phrase */
	  phrase_prox_matching = 1;
	  /* set up */
	  word_ptr++ ; /* $ must be first char */
	}
	if ( (dollar = strchr(word_ptr,'$')) != NULL 
	     && phrase_prox_matching == 1) {
	  /* This must be the phrase end */
	  *dollar = '\0';
	  last_phrase_prox_term = 1;
	}
	
	
	/* for phrase matching, we use the unnormalized word length...*/
	if (phrase_prox_matching) {
	  prox_term_length = strlen(word_ptr);
	  phrase_prox_length += (prox_term_length + 1);
	}
	if ((truncation == PROCESS_POUND && strchr(word_ptr,'#') != NULL)
	    || truncation == RIGHT_TRUNC)
	  index_result = se_gettrunc_idx(idx,word_ptr);
	else
	  index_result = se_getterm_idx(idx, word_ptr, 1/*normalize*/,
					&search_stopword_count);
	
	if (index_result != NULL) {
	  
	  if (num_found_terms < 100)
	    termidlist[num_found_terms++] = index_result->termid;
	  
	  /* build the list of prox terms (in order) */
	  if (phrase_prox_matching && num_prox_terms < 100) {
	    proxtermidlist[num_prox_terms] = index_result->termid;
	    proxtermlength[num_prox_terms++] = prox_term_length;
	  }
	  
	  for (i = 0; i < index_result->num_entries; i++) {
	    
	    /* we will be anding these, so need to know the max size */
	    if (index_result->num_entries < max_hits || max_hits == 0)
	      max_hits = index_result->num_entries;
	    
	    record_num = index_result->entries[i].record_no;
	    if (record_num > 0) { /* negative is a deleted entry */
	      entry = Tcl_FindHashEntry(hash_tab, (void *)record_num);
	      
	      if (entry == NULL && first_term == 1) { /* new record number */
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
	      else if (entry != NULL) {
		storage = (struct temp_results *) Tcl_GetHashValue(entry);
		if(storage->weight < num_search_words)
		  storage->weight++;
		storage->tot_freq += index_result->entries[i].term_freq;
	      }
	    }
	  }
	  /* done with the index results from the getterm call */	
	  if (index_result->term)
	    FREE(index_result->term);
	  FREE(index_result);
	  /* we have not encountered the first non-stop term */
	  first_term = 0;
	  
	} /* if not null index_result... */
	
	if (last_phrase_prox_term)
	  phrase_prox_matching = 0;
	
       	if (phrase_prox_matching && 
	    last_stop_count != search_stopword_count) {
	  /* stopword in a prox phrase -- add the stopword length to the
	   * last term -- if there is one
	   */
	  if (num_prox_terms > 0) {
	    proxtermlength[num_prox_terms-1] += prox_term_length + 1;
	  }
	  phrase_prox_length += (prox_term_length + 1);
	} 
	last_stop_count = search_stopword_count;
	/* get the next word */
	word_ptr = strtok_r (NULL, breakletters, &lastptr);
      } while (word_ptr != NULL);
      
      tot_docs = hash_tab->numEntries;
    }
    
    if ((num_search_words - search_stopword_count) <= 0) {
      /* all search words were stopwords */
      diagnostic_set(4,0,NULL); 
      FREE(searchstring);
      return(NULL);
    }
    
    wt_res = (weighted_result *) 
      CALLOC (char, (sizeof(weighted_result) + ((max_hits+1) * sizeof(weighted_entry))));
    
    wt_res->num_hits = 0;
    wt_res->setid[0] = '\0';
    wt_res->max_weight = 1.0;
    wt_res->min_weight = 1.0;

    if (idx->type & PAGEDINDEX) {
      wt_res->result_type |= PAGED_RESULT;
    }

    if (idx->type & COMPONENT_INDEX) {
      wt_res->result_type |= COMPONENT_RESULT;
      wt_res->component = idx->comp_parent;
    }

    if (idx->type & PROXIMITY) {
      wt_res->result_type |= PROXIMITY_RESULT;
    }
	
    for (i = 0; i < num_found_terms; i++) {
      wt_res->termid[i] = termidlist[i];
      wt_res->index_ptr[i] = idx;
    }
    i = 0;

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
	 entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

      record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
      storage = (struct temp_results *) Tcl_GetHashValue(entry);
      


      if (storage->weight >= (num_search_words - search_stopword_count)) {
	if (num_prox_terms > 0) {
	  if (se_prox_phrase(idx->prox_db, num_prox_terms, 
			     proxtermidlist, proxtermlength, record_num, 
			     phrase_prox_length)) {
	    wt_res->entries[i].record_num = record_num;
	    wt_res->num_hits++;
	    wt_res->entries[i].weight = 1.0;
	    /* wt_res->entries[i].weight = (float)storage->tot_freq; */
	    i++;
	  }
	}
	else { /* no proximity processing needed */
	  wt_res->entries[i].record_num = record_num;
	  wt_res->num_hits++;
	  wt_res->entries[i].weight = 1.0;
	  /* wt_res->entries[i].weight = (float)storage->tot_freq; */
	  i++;
	}
      }
      free(storage); /* not needed any more */
    }

    Tcl_DeleteHashTable(hash_tab); /* clean up */
  }
  else if (structure == PHRASE) { 
    /* a simple single key or phrase search */
    wt_res = se_phrase_match(idx, searchstring, relation, position, 
			   structure, truncation, completeness);

  }
  else if (structure == WORD     /* single keyword -- not multiple words */
	   || structure == KEY 
	   || structure == YEAR
	   || structure == DATE_NORM
	   || structure == DATE_UNORM
	   || structure == NAME_NORM
	   || structure == NAME_UNORM
	   || structure == URX
	   || structure == LOCAL_NUMBER
	   || (structure == 0 && idx->type & INTEGER_KEY)
	   || (structure == 0 && idx->type & DECIMAL_KEY)
	   || (structure == 0 && idx->type & FLOAT_KEY)
	   || (structure == 0 && idx->type & DATE_KEY)
	   || (structure == 0 && idx->type & DATE_RANGE_KEY)
	   || (structure == 0 && idx->type & LAT_LONG_KEY)
	   || (structure == 0 && idx->type & BOUNDING_BOX_KEY)
	   || (structure == 0 && idx->type & GEOTEXT)
           || (structure == 0 && idx->type & FLD008_KEY) ) {     

    if ((truncation == PROCESS_POUND && strchr(searchstring,'#') != NULL)
	|| truncation == RIGHT_TRUNC)
      index_result = se_gettrunc_idx(idx,searchstring);
    else
      index_result = se_getterm_idx(idx, searchstring, 1 /*normalize*/,
				    &search_stopword_count);

    if (index_result != NULL) {
       
      termidlist[num_found_terms++] = index_result->termid;

      tot_docs = index_result->num_entries;
      wt_res = (weighted_result *) 
	CALLOC(char, (sizeof(weighted_result) 
		      + (tot_docs * sizeof(weighted_entry))));
      wt_res->num_hits = tot_docs;
      if (idx->type & PAGEDINDEX) {
	wt_res->result_type = PAGED_RESULT;
      }
      if (idx->type & COMPONENT_INDEX) {
	wt_res->result_type = COMPONENT_RESULT;
	wt_res->component = idx->comp_parent;
      }
      
      for (i = 0; i < num_found_terms; i++) {
	wt_res->termid[i] = termidlist[i];
        wt_res->index_ptr[i] = idx;
      }
      for (i = 0; i < index_result->num_entries; i++) {
	
	wt_res->entries[i].record_num = 
	  index_result->entries[i].record_no;
	wt_res->entries[i].weight = 1.0;
	/* wt_res->entries[i].weight = (float)
	 * index_result->entries[i].term_freq;
	 */
      }
      FREE(index_result);
    }
    else {
      wt_res = CALLOC(weighted_result,1);
      wt_res->num_hits = 0;
    }
  }
  se_sortwtset(wt_res);

  if (relation == 6) {
    int ndocs;
    weighted_result *tmp_wt_res;    
    int result;
    int n, m;
    /* we have the results in wt_res, but this is NOT EQUAL, so */
    /* we generate a list with EVERYTHING BUT the wt_res set    */
    if (idx->type & COMPONENT_INDEX) {
      if (idx->comp_parent->comp_db == NULL) {
	/* open up the component info index */
	idx->comp_parent->comp_db = 
	  cf_component_open(idx->comp_parent->config_info->nickname, 
			    idx->comp_parent->name);
      }
      ndocs = idx->comp_parent->max_component_id;
    }
    else
      ndocs = cf_getnumdocs(filename);
    
    tmp_wt_res = (weighted_result *)
      CALLOC(char, (sizeof(weighted_result) 
		    + (ndocs * sizeof(weighted_entry))));
    
    tmp_wt_res->result_type = wt_res->result_type;
    tmp_wt_res->component = wt_res->component;
    
    /* put in all of the items before the first match */
    if (wt_res->num_hits == 0)
      wt_res->entries[0].record_num = ndocs;
    
    result = 0;
    m = 1;
    for (i = 0; m < wt_res->entries[0].record_num; i++, m++) {
      tmp_wt_res->entries[i].record_num = m;
      tmp_wt_res->entries[i].weight = 1.0;
      result++;
    }	
    
    n = 0;
    while (n < wt_res->num_hits) {
      if (wt_res->entries[n].record_num == m) {
	m++;
	n++;
	continue;
      }
      else {
	tmp_wt_res->entries[i].record_num = m++;
	tmp_wt_res->entries[i++].weight = 1.0;
	result++;
      }
    }
    
    for (; m < ndocs; m++) {
      tmp_wt_res->entries[i].record_num = m;
      tmp_wt_res->entries[i++].weight = 1.0;
      result++;
    }	
    
    tmp_wt_res->num_hits = result;
    
    FREE(wt_res);
    FREE(searchstring);
    return(tmp_wt_res);
    
  }
  else {
    FREE(searchstring);
    return(wt_res);
  }
  
}







