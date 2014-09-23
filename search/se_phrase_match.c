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
*	Header Name:	se_phrase_match.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	boolean search processing for a single index
*                       phrase or exact key of the cheshire catalog
*
*	Usage:		se_phrase_match (index_struct, phrase, relation,
*                                 position, structure, truncation, 
*                                 completeness)
*
*	Variables:	index_struct is an idx_list_entry for the index
*                       term is the phrase to be searched
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

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);


/*forward declaration */
weighted_result *se_phrase_match (idx_list_entry *idx,  char *searchstring, 
				  int relation, int position, 
				  int structure, int truncation, 
				  int completeness)
{
  
  char *word_ptr, *next_tok, *result_word, *end_buffer;
  char *current_buffer; 
  int i, exists, morphed;
  int  tot_docs;
  int record_num;
  int n_stopwords = 0;
  
  idx_result *index_result;
  long docid;
  float docwt;
  weighted_result *wt_res;

  char *lastptr;

  if (searchstring == NULL) return (NULL);

  /* We will tokenize the string and remove stopwords, punctuation, etc. */
  current_buffer = CALLOC(char, strlen(searchstring)+200);

  if (idx->type & CLASSCLUS) {

    result_word = normalize_key(searchstring, idx, &morphed, 0);
      
    /* NULL means a problem, so skip it */
    if (result_word != NULL) {
      strcpy(current_buffer, result_word);
      FREE(result_word);

      /* handle explicit truncation of the call number */
      /* otherwise assume that exact key is wanted     */
      if (strchr(searchstring,'#') != NULL) {
	for (i = strlen(current_buffer)-1; current_buffer[i] == ' '; i--)
	  current_buffer[i] = '\0';
      }
    }
    /* current_buffer now contains the normalized phrase */
    if (truncation == 100)
      index_result = se_getterm_idx(idx, current_buffer, 0/*normalize*/,
				    &n_stopwords);
    else
      index_result = se_gettrunc_idx(idx, current_buffer);
    
  }
  else if (idx->type & NORM_DO_NOTHING) {
    if (truncation == 100)
      index_result = se_getterm_idx(idx, searchstring, 0/*normalize*/,
				    &n_stopwords);
    else
      index_result = se_gettrunc_idx(idx, searchstring);
  }
  else {
    word_ptr = strtok_r (searchstring, 
			 " \t\n`~!@#$%^&*()_=|\\{[]};:'\",<.>?/", &next_tok);
    
    do {
      /* normalize the word according to the specs for this index */
      /* (as defined in indextype) SHOULD be EXACTKEY             */
      
      result_word = normalize_key(word_ptr, idx, &morphed, 1);
      
      /* NULL means a stop word or other problem, so skip it */
      if (result_word != NULL) {
	/* append the word to the current buffer */
	if (current_buffer[0] != '\0')
	  strcat(current_buffer, " ");
	strcat(current_buffer, result_word);
	FREE(result_word);
      }

      /* get the next word from the current buffer     */
      word_ptr = strtok_r (NULL, 
			   " \t\n`~!@#$%^&*()_+=|\\{[]};:'\",<.>?/", &next_tok);
    } while (word_ptr != NULL);  
    /* current_buffer now contains the normalized phrase */
    if (truncation == 100)
      index_result = se_getterm_idx(idx, current_buffer, 0/*normalize*/,
				    &n_stopwords);
    else
      index_result = se_gettrunc_idx(idx, current_buffer);
  }

  FREE(current_buffer);

  if (index_result != NULL) {
       
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
      
    if (idx->type & PROXIMITY) {
      wt_res->result_type |= PROXIMITY_RESULT;
      wt_res->termid[0] = index_result->termid;;
      wt_res->index_ptr[0] = idx;
    }
	

    for (i = 0; i < index_result->num_entries; i++) {
	
      wt_res->entries[i].record_num = 
	index_result->entries[i].record_no;
      wt_res->entries[i].weight = (float)
	index_result->entries[i].term_freq;
    }
    FREE(index_result);
  }
  else {
    /* clear any diagnostics  zero results is NOT an error */
    diagnostic_clear();
    wt_res = CALLOC(weighted_result,1);
    wt_res->num_hits = 0;
  }
  wt_res->setid[0] = '\0';
  return(wt_res);
}



