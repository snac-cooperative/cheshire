/*
 *  Copyright (c) 1990-2003 [see Other Notes, below]. The Regents of the
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
*	Header Name:	se_bitmap.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Process bitmap searches.
*                       build an EMPTY weighted result set.
*
*	Usage:		se_bitmap(databasename, search_term(s))
*
*	Variables:	
*                       databasename - the filename or file tag of the
*                           desired database.
*
*	Return Conditions and Return Codes:	
*
*	Date:		04/01/2003
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2003.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include <stdlib.h>
#include <string.h>
#include "bitmaps.h"

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

extern bitmap_t *se_fetch_bitmap_block(weighted_result *inset, int recordno, 
				int *block, bitmap_t *last_block);


extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);



weighted_result *
se_bitmap(idx_list_entry *idx,  char *filename, char *insearchstring, 
		 int relation, int position, 
		 int structure, int truncation, 
		 int completeness, int attr_nums[],
		 char *attr_str[],  ObjectIdentifier attr_oid[])
{

  char *word_ptr, *norm_word_ptr;
  
  int i, exists;
  int tot_docs;
  int record_num;
  int num_search_words = 0;
  int max_hits = 0;
  int num_found_terms = 0;
  int morphed;
  int termidlist[100]; 
  int proxtermidlist[100]; 
  int proxtermlength[100];
  char *searchstring = NULL;
  int max_record_num = 0;
  idx_result *index_result;
  weighted_result *wt_res, *se_sortwtset();
  char *breakletters;
  char *keywordletters = " \t\n`~!@$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n<>";
  char *filenameletters = " \t\n<>";
  int search_stopword_count;

  char *lastptr, *strtok_r();

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
  if (idx->type & URL_KEY) breakletters = urlletters;
  if (idx->type & FILENAME_KEY) breakletters = filenameletters;

  /* processes exactkeys as phrases by default */
  if ((idx->type & EXACTKEY) && structure == 0) {
    structure = PHRASE;
  }

  if (structure == WORD_LIST || 
      (structure == 0 && 
       (idx->type & KEYWORD)
       || (idx->type & URL_KEY)
       || (idx->type & FILENAME_KEY))) { 
    /* handle each word in the list */
    
    /* find first token */
    word_ptr = strtok_r (searchstring, breakletters, &lastptr);
    if (word_ptr == NULL) { /* no words in search string */
      FREE(searchstring);
      return (NULL);
    }
    else { /* process each word */

      do {
   
	num_search_words++;

	if ((truncation == PROCESS_POUND && strchr(word_ptr,'#') != NULL)
	    || truncation == RIGHT_TRUNC)
	  index_result = se_gettrunc_idx(idx,word_ptr);
	else 
	  index_result = se_getterm_idx(idx,word_ptr, 1/* norm*/, 
					&search_stopword_count);

	if (index_result != NULL) {
	  
	  if (num_found_terms < 100)
	    termidlist[num_found_terms++] = index_result->termid;
	  
	  if (index_result->term)
	    FREE(index_result->term);
	  FREE(index_result);

	  if (index_result->entries[0].record_no > max_record_num) 
	    max_record_num = index_result->entries[0].record_no;

	} /* if not null index_result... */

	/* get the next word */
	word_ptr = strtok_r (NULL, breakletters, &lastptr);
      } while (word_ptr != NULL);
      
    }
    
    if ((num_search_words - search_stopword_count) <= 0) {
      /* all search words were stopwords */
      diagnostic_set(4,0,NULL); 
      FREE(searchstring);
      return(NULL);
    }

    
    wt_res = (weighted_result *) 
      CALLOC (char, (sizeof(weighted_result)));
    
    if (idx->type & COMPONENT_INDEX) {
      wt_res->result_type |= COMPONENT_RESULT;
      wt_res->component = idx->comp_parent;
    }

    wt_res->result_type |= BITMAP_RESULT;    
    wt_res->entries[0].record_num = max_record_num;
    for (i = 0; i < num_found_terms; i++) {
      wt_res->termid[i] = termidlist[i];
      wt_res->index_ptr[i] = idx;
    }
    i = 0;
    
    if ((num_search_words - search_stopword_count) != num_found_terms) {
      /* no match */
      return (wt_res);
    }
    else {
      wt_res->num_hits = -1;
    }

  }
  else {

    if ((truncation == PROCESS_POUND && strchr(searchstring,'#') != NULL)
	|| truncation == RIGHT_TRUNC)
      index_result = se_gettrunc_idx(idx,searchstring);
    else
      index_result = se_getterm_idx(idx, searchstring, 1 /*normalize*/,
				    &search_stopword_count);
    

    if (index_result != NULL) {
       
      termidlist[num_found_terms++] = index_result->termid;

      wt_res = (weighted_result *) 
	CALLOC(char, (sizeof(weighted_result)));
      wt_res->num_hits = index_result->num_entries;
      wt_res->entries[0].record_num = index_result->entries[0].record_no;
  
      if (idx->type & COMPONENT_INDEX) {
	wt_res->result_type = COMPONENT_RESULT;
	wt_res->component = idx->comp_parent;
      }
      
      wt_res->result_type |= BITMAP_RESULT;

      for (i = 0; i < num_found_terms; i++) {
	wt_res->termid[i] = termidlist[i];
        wt_res->index_ptr[i] = idx;
      }

      FREE(index_result);
    }
    else {
      wt_res = CALLOC(weighted_result,1);
      wt_res->num_hits = 0;
    }
  }

  FREE(searchstring);
  return(wt_res);
  
}


weighted_result *
se_bitmap_to_result(weighted_result *inres, int n_wanted, int n_start)
{
  int returncode;
  bitmap_t *result_block = NULL;
  int i=0, j=0, bit;
  int max_docid;
  int max_blockno;
  int doc_count = 0;
  int all_finished = 0;
  weighted_result *wt_res; /* final result */
  int skip = 0;
  int dummy_recno = 1;
  int current_blockno = 0;

  if (inres == NULL)
    return (NULL);

  max_docid = inres->entries[0].record_num;
  max_blockno = bitblock_id(max_docid) + 1;
    
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + ((n_wanted+1) * sizeof(weighted_entry))));
  *wt_res = *inres;
  doc_count = inres->num_hits;
  wt_res->num_hits = 0;
  wt_res->setid[0] = '\0';
  /* the results with be a real resultset and not bitmaps */
  wt_res->result_type &= ~BITMAP_RESULT;
  wt_res->max_weight = 1.0;
  wt_res->min_weight = 1.0;

  for (dummy_recno = 1; 
       dummy_recno < max_docid
	 && wt_res->num_hits < n_wanted;
       dummy_recno += bitmap_blocksize_bits) {

    result_block = se_fetch_bitmap_block(inres, dummy_recno, 
					 &current_blockno, result_block);
    
    /* accum block should now contain any possible results */
    
    for (j=0; j < bitmap_blocksize_bytes
	   && wt_res->num_hits < n_wanted; j++) {
      if (result_block[j]) {
	for (bit = 0; bit < 8; bit++) {
	  if(result_block[j] & (1 << bit)) {
	    /* add the record number to the result list */
	    if (skip < n_start) {
	      skip++;
	      continue;
	    }
	    if (wt_res->num_hits < n_wanted) {
	      wt_res->entries[wt_res->num_hits].record_num =
		(current_blockno * bitmap_blocksize_bits) +
		(j * 8) + bit + 1;
	      wt_res->entries[wt_res->num_hits].weight = 1.0;
	      wt_res->num_hits++;
	    }
	  }
	}
      }
    }
  }

  if (doc_count > 0) {
    wt_res->num_hits = doc_count;
  }

  return(wt_res);
  
}



