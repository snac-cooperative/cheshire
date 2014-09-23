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
*	Header Name:	se_cori.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	probabilistic CORI distributed search weighting
*	Usage:		se_cori (filename, indexname, searchstring)
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:	        3/6/2003
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2003.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include "search_stat.h"

#include <math.h>

extern int se_getdoclen(long docid, char *filename, idx_list_entry *idx) ;
extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern DB *cf_component_open(char *filename, char *componentname);

extern idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
				  int normalize, int *search_stopword_count);

extern idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);

extern int cf_get_dist_docsize(char *filename, int recnum, 
			       SGML_Document *doc, config_file_info *cf);

double se_get_avg_document_len(char *filename);


weighted_result *se_cori (char *filename, char *indexname, char *insearchstring)
{

  char *word_ptr, *norm_word_ptr;
  float ndocs;
  int i, exists;
  int  tot_docs;
  int tot_qterms;
  int q_term_freq;
  int dummy = 0;
  int termidlist[100]; 
  int proxtermidlist[100]; 
  int proxtermlength[100];
  int num_found_terms = 0;
  char *searchstring = NULL;
  double T, I, NumDB, df, cf;
  int n_stopwords = 0;

  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  Tcl_HashTable *q_hash_tab, q_hash_tab_data;
  Tcl_HashEntry *q_entry;
  Tcl_HashSearch q_hash_search;

  Tcl_HashTable *stoplist_hash;

  idx_result *index_result;
  DB *index_db;
  idx_list_entry *idx, *cf_getidx_entry();

  long docid;

  int total_q_terms = 0;
  int index_type, morphed;
  char *breakletters;
  char *keywordletters = " \t\n`~!@$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n<>";
  char *filenameletters = " \t\n<>";
  int min_cf = 0;
  int max_cf = 0;
  int sum_entries = 0;
  int min_entries = 0;
  int max_entries = 0;

  struct docsum{
    int num_qterms;
    double sum_wts;
    double sum_I;
    double sum_T;
    double avg_doclen;
    int doclen;
    int min_tf;
    int max_tf;
  } *doc_wts;

  double avg_doclen;
  
  weighted_result *wt_res, *ranked_res, *se_rank_docs();

  char *lastptr;

  if (insearchstring == NULL)
    return NULL;
  else
    searchstring = strdup(insearchstring);

  /* Init the hash table for collecting query terms */
  q_hash_tab = &q_hash_tab_data;
  Tcl_InitHashTable(q_hash_tab,TCL_STRING_KEYS);

  /* Init the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);

  index_db = 
    (DB *) cf_index_open(filename, indexname, INDEXFL);

  if (index_db == NULL)
    return (NULL);

  idx = cf_getidx_entry(filename, indexname);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;

  if (index_type & URL_KEY) breakletters = urlletters;
  else if (index_type & FILENAME_KEY) breakletters = filenameletters;
  else breakletters = keywordletters; 

  /* get the total number of "documents": components or records indexed */
  if (index_type & COMPONENT_INDEX) {
    if (idx->comp_parent->comp_db == NULL) {
    /* open up the component info index */
      idx->comp_parent->comp_db = cf_component_open(idx->comp_parent->config_info->nickname, 
						    idx->comp_parent->name);
    }
    ndocs = idx->comp_parent->max_component_id;
  }
  else
    ndocs = (float) cf_getnumdocs(filename);

  NumDB = (double)ndocs;

  /* find first token */
  word_ptr = strtok_r (searchstring, breakletters, &lastptr);
  do { /* build the query elements */

    norm_word_ptr = 
      normalize_key(word_ptr, idx, &dummy, 1);
    
    if (norm_word_ptr != NULL) {
	q_entry = Tcl_FindHashEntry(q_hash_tab,norm_word_ptr);
	
	if (q_entry == NULL){
	  total_q_terms++;
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       q_hash_tab,
					       norm_word_ptr,
					       &exists),
			   (ClientData)1);
	  FREE(norm_word_ptr); /* this was allocated in normalize_key */
	}
	else {
	  q_term_freq = (int) Tcl_GetHashValue(q_entry);
	  q_term_freq++;
	  total_q_terms++;
	  Tcl_SetHashValue(q_entry,
			   (ClientData)q_term_freq);
	  FREE(norm_word_ptr); /* this was allocated in normalize_key */
	}
    }

    /* get the next word */
    word_ptr = strtok_r (NULL, breakletters, &lastptr);
  } while (word_ptr != NULL);

   
  /* get the average doclength for this database  */
  /* this is currently done VERY EXPENSIVELY and should be changed */

  avg_doclen = (se_get_avg_document_len(filename) / 10.00);
  
  for (q_entry = Tcl_FirstHashEntry(q_hash_tab,&q_hash_search);
       q_entry != NULL; q_entry = Tcl_NextHashEntry(&q_hash_search)) {

    /* get the word/stem and it's frequency in the query from the hash tab */
    word_ptr = (char *)Tcl_GetHashKey(q_hash_tab,q_entry); 
    q_term_freq = (int) Tcl_GetHashValue(q_entry);


    /* find it in the index */
    if (strchr(word_ptr,'#') != NULL)
      index_result = se_gettrunc_idx(idx,word_ptr);
    else
      index_result = se_getterm_idx(idx,
				    word_ptr, 0 /* don't normalize twice.*/,
				    &n_stopwords);

    if (index_result != NULL) {

      if (num_found_terms < 100)
	termidlist[num_found_terms++] = index_result->termid;

      cf = (double)index_result->num_entries;

      I = (log(((NumDB + 0.5)/cf)))/(log(NumDB+1.0));

      for (i = 0; i < index_result->num_entries; i++) {

	if (index_result->entries[i].record_no > 0) { /* forget deleted ones */
	  entry = Tcl_FindHashEntry(hash_tab, 
				    (void *)index_result->entries[i].record_no);

	  if (entry == NULL){
	    doc_wts = CALLOC(struct docsum,1);
	    
	    doc_wts->num_qterms = 1;
	    doc_wts->doclen = se_getdoclen(index_result->entries[i].record_no,
					   filename, idx);
	    
	    df = (double)index_result->entries[i].term_freq;

	    T = df/(df+ 50.0 +
		    ((150.0 * ((double)doc_wts->doclen/10.0))/avg_doclen));
		    
	    doc_wts->sum_wts = 0.4 + (0.6 * T * I);
	    doc_wts->sum_I = I;
	    doc_wts->sum_T = T;
	    doc_wts->avg_doclen = avg_doclen;
	    
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(
						 hash_tab,
						 (void *)index_result->entries[i].record_no,
						 &exists),
			     (ClientData)doc_wts);
	  }
	  else {
	    /* add to an existing doc entry */
	    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
	    doc_wts->num_qterms++;

	    df = (double)index_result->entries[i].term_freq;

	    T = df/(df + 50.0 +
		    ((150.0 * ((double)doc_wts->doclen/10.0))/avg_doclen));
	    
	    /* Try with multiplying... maybe adding is better.*/

	    doc_wts->sum_wts += 0.4 + (0.6 * T * I);
	    doc_wts->sum_I += I;
	    doc_wts->sum_T += T;
	  }
	}
      }
      /* FREE the temp index results */
      free(index_result);
    } /* if not null index_result... */
  }
  /* all done with the query terms */
  Tcl_DeleteHashTable(q_hash_tab); /* clean up */
  
  tot_docs = hash_tab->numEntries;
  
  if (diagnostic_get() == 4 && tot_docs > 0) {
    /* there were stopwords -- ignore */
    diagnostic_clear();
  }
  
  
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + 
		   (tot_docs * sizeof(weighted_entry))));

  wt_res->num_hits = tot_docs;

  for (i = 0; i < num_found_terms; i++) {
    wt_res->termid[i] = termidlist[i];
    wt_res->index_ptr[i] = idx;
  }

  i = 0;

  /* if requested, allocate structure for collecting stats on q/doc */
  if (statflag == 1) {
    if (Query_Stats != NULL) {
      FREE(Query_Stats);
      Query_Stats = NULL;
    }
    if (Query_Stats == NULL)
      Query_Stats = (query_stats *)CALLOC(char, 
					 (sizeof(sstat_info) * tot_docs+1));
    
  }

  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

    wt_res->entries[i].record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
    wt_res->entries[i].weight = (float)doc_wts->sum_wts / (double)total_q_terms ;
   
    if (statflag == 1) {
      Query_Stats->totaldocs = tot_docs;
      Query_Stats->totaldocs = tot_docs;
      if (index_type & COMPONENT_INDEX) {
	Query_Stats->docstat[i].docid = 0;
	Query_Stats->docstat[i].compid = wt_res->entries[i].record_num;
	Query_Stats->comp_name = idx->comp_parent->name;
      }
      else {
	Query_Stats->docstat[i].docid = wt_res->entries[i].record_num;
	Query_Stats->docstat[i].compid = 0;
	Query_Stats->comp_name = NULL;
      }
      Query_Stats->docstat[i].doclength = doc_wts->doclen;
      Query_Stats->docstat[i].querylength = total_q_terms;
      Query_Stats->docstat[i].num_match_terms = doc_wts->num_qterms;
      Query_Stats->docstat[i].ndocs = ndocs;
      Query_Stats->docstat[i].dist_ndocs = 
	cf_get_dist_docsize(filename, 
			    wt_res->entries[i].record_num, 
			    NULL, NULL);
      Query_Stats->docstat[i].min_cf = min_cf;
      Query_Stats->docstat[i].max_cf = max_cf;
      Query_Stats->docstat[i].min_tf = doc_wts->min_tf;
      Query_Stats->docstat[i].max_tf = doc_wts->max_tf;
      Query_Stats->docstat[i].sum_entries = sum_entries;
      Query_Stats->docstat[i].min_entries = min_entries;
      Query_Stats->docstat[i].max_entries = max_entries;
      Query_Stats->docstat[i].X1 = doc_wts->sum_I;
      Query_Stats->docstat[i].X2 = doc_wts->sum_T;
      Query_Stats->docstat[i].X3 = doc_wts->avg_doclen;
      Query_Stats->docstat[i].X4 = 0.0;
      Query_Stats->docstat[i].X5 = 0.0;
      Query_Stats->docstat[i].X6 = 0.0;
      Query_Stats->docstat[i].logodds = 0.0;
      Query_Stats->docstat[i].docwt = (double)wt_res->entries[i].weight;
    }

    if (wt_res->max_weight == 0.0) 
      wt_res->max_weight = wt_res->entries[i].weight;
    else if (	wt_res->max_weight < wt_res->entries[i].weight)
      wt_res->max_weight = wt_res->entries[i].weight;
    if (wt_res->min_weight == 0.0) 
      wt_res->min_weight = wt_res->entries[i].weight;
    else if (wt_res->min_weight > wt_res->entries[i].weight)
      wt_res->min_weight = wt_res->entries[i].weight;
    

    free(doc_wts); /* not needed any more */
    i++;
  }
  Tcl_DeleteHashTable(hash_tab); /* clean up */
  
  wt_res->setid[0] = '\0';

  wt_res->result_type = CORI_RESULT;

  if (index_type & PAGEDINDEX) {
    wt_res->result_type |= PAGED_RESULT;
  }
  
  if (idx->type & COMPONENT_INDEX) {
    wt_res->result_type |= COMPONENT_RESULT;
    wt_res->component = idx->comp_parent;
  }
	
  if (idx->type & PROXIMITY) {
    wt_res->result_type |= PROXIMITY_RESULT;
  }

  FREE(searchstring);

  return(se_rank_docs(wt_res));
}












