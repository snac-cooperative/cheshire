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
*	Header Name:	se_prob.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	probabilistic search of the cheshire catalog
*
*	Usage:		se_prob (filename, indexname, searchstring)
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/14/93
*	Revised:	11/16/93 (added code to free memory when done)
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/*
 * ********************************************************************
 * New version (see se_prob_trec2.c) this version uses the description
 * in Aitao's mulir documentation
 * 
 * log O(R|D,Q) = -3.51 + 37.4 * x1 + 0.330 * x2 + -0.1937 * x3 + 0.0929 * x4
 *
 *    where:
 *          x1 = (1/sqrt(n)+1) * SUM(qtf/ql + 35 )
 *          x2 = (1/sqrt(n)+1) * SUM(log(dtf/dl + 80))
 *          x3 = (1/sqrt(n)+1) * SUM(log(ctf/cl))
 *          x4 = n
 *
 *    and:
 *          n = number of matching terms between query and document
 *          qtf = within-query freq of a term
 *          dtf = within-doc freq of a term
 *          ctf = occurrence freq in collection of a term
 *          ql = number of terms in a query
 *          dl = number of terms in a document
 *          cl = number of terms in the collection
 *
 *
 * ********************************************************************
 */

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
			       SGML_Document *doc, config_file_info *cf, 
			       int doclen);

extern weighted_result *se_rank_docs(weighted_result *src);

extern char *se_get_term_by_id(idx_list_entry *idx, int termnum, int *coll_freq);

extern vector_data_info *se_get_vector_data(idx_list_entry *idx, int docid);




typedef struct query_term_info {
  float freq_wt;
  float fb_wt;
  float coll_freq;
  float rel_weight;
  int countdocs;
  int orig_flag;
  int keep;
} query_term_info;


typedef struct sortset {
  query_term_info *ptr;
  char *termptr;
} sortset;



int compare_qt_data(sortset *in1, sortset *in2)
{
  if (in1->ptr->rel_weight < in2->ptr->rel_weight)
    return (1);
  else if (in1->ptr->rel_weight == in2->ptr->rel_weight)
    return (0);
  else
    return (-1);
  
}


weighted_result *se_prob_trec2 (char *filename, char *indexname, 
				char *insearchstring, int blindfeedback_flag)
{

  char *word_ptr, *norm_word_ptr;

  float sumwt;
  float ndocs;
  int i, j, exists;
  int  tot_docs;
  int q_term_freq;
  int dummy = 0;
  int termidlist[100]; 
  int proxtermidlist[100]; 
  int proxtermlength[100];
  int num_found_terms = 0;
  char *searchstring = NULL;
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

  double log_odds;
  float total_q_length = 0.0;
  int count_q_terms = 0;
  int index_type, morphed;
  char *breakletters;
  char *keywordletters = " \t\n`~!@$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n<>";
  char *filenameletters = " \t\n<>";
  int dist_ndocs = 0; /* see search_stat.h for more details on these */
  int min_cf = 0;
  int max_cf = 0;
  int sum_entries = 0;
  int min_entries = 0;
  int max_entries = 0;
  double collection_length = 0.0;
  /* TREC 2 (default) coefficients */
  float c0 = -3.51;
  float c1 = 37.4;
  float c2 = 0.330;
  float c3 = -0.1937;
  float c4 = 0.0929;
  float c5 = 0.00;
  float c6 = 0.00;
  ranking_parameters *rank_parm;
  float CONST1 = 0.0;
  double exp_log_odds;
  int blindfeedback_nrecs = 0;
  int blindfeedback_nterms = 0;
  int bf_docid_1, bf_docid_2;
  float bf_weight_1, bf_weight_2;
  vector_data_info *fb_vector;
  query_term_info *r_wt;
  int relwtentry=0;
  int blindfeedback = 0;
  int count_bf_terms;

  struct docsum {
    int	doc_no;
    int	numberMatchTerms;
    int min_tf;
    int max_tf;
    float doc_length;
    float doc_term_wt;    /* sum log doc freq/doclen +80 */
    float query_term_wt;
    float collection_term_wt;
    float prob;               /* probability of relevance.   */
  } *doc_wts;


  query_term_info *query_term_data;
  sortset *rel_weights;
  
  weighted_result *wt_res, *ranked_res;

  char *lastptr, *strtok_r();

  if (blindfeedback_flag)
    blindfeedback = 1;


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
  
  for (rank_parm = idx->ranking_parameters; rank_parm != NULL; rank_parm = rank_parm->next_parm) {
    if (rank_parm->type == 4) {
      /* it is Logistic regression TREC2  parameter */
      switch(rank_parm->id) {
      case 0:
	c0 = rank_parm->val;
	break;
      case 1:
	c1 = rank_parm->val;
	break;
      case 2:
	c2 = rank_parm->val;
	break;
      case 3:
	c3 = rank_parm->val;
	break;
      case 4:
	c4 = rank_parm->val;
	break;
      case 5:
	c5 = rank_parm->val;
	if (c5 >= 1.0)
	  blindfeedback_nrecs = (int)c5;
	
	break;
      case 6:
	c6 = rank_parm->val;
	if (c6 >= 1.0)
	  blindfeedback_nterms = (int)c6;

	break;
	/* getting to here means that the defaults above are used */
      }
    }
  }

  if (blindfeedback && blindfeedback_nrecs == 0) {
    /* settings from Aitao's CLEF work */
    blindfeedback_nrecs = 10;
    blindfeedback_nterms = 10;
  }


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

  /* collection_length (cl in description) is the size in terms of current
   * index definition.
   */
  collection_length = (double)((unsigned int)idx->GlobalData->tot_occur);


  /* find first token */
  word_ptr = strtok_r (searchstring, breakletters, &lastptr);
  do { /* build the query elements */

    norm_word_ptr = 
      normalize_key(word_ptr, idx, &dummy, 1);
    
    if (norm_word_ptr != NULL) {
	q_entry = Tcl_FindHashEntry(q_hash_tab,norm_word_ptr);
	
	if (q_entry == NULL){
	  total_q_length += 1.0;
	  count_q_terms++;
	  query_term_data = CALLOC(query_term_info, 1);
	  query_term_data->freq_wt = 1.0;
	  query_term_data->orig_flag = 1;
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       q_hash_tab,
					       norm_word_ptr,
					       &exists),
			   (ClientData)query_term_data);
	  FREE(norm_word_ptr); /* this was allocated in normalize_key */
	}
	else {
	  query_term_data = (query_term_info *) Tcl_GetHashValue(q_entry);
	  query_term_data->freq_wt = query_term_data->freq_wt + 1.0;
	  total_q_length += 1.0;
	  count_q_terms++;
	  FREE(norm_word_ptr); /* this was allocated in normalize_key */
	}
    }

    /* get the next word */
    word_ptr = strtok_r (NULL, breakletters, &lastptr);
  } while (word_ptr != NULL);

  /* the following should be looped a max of TWO TIMES for blind feedback */

  do {
  
    num_found_terms = 0;

    for (q_entry = Tcl_FirstHashEntry(q_hash_tab,&q_hash_search);
	 q_entry != NULL; q_entry = Tcl_NextHashEntry(&q_hash_search)) {
      
      /* get the word/stem and it's frequency in the query from the hash tab */
      word_ptr = (char *)Tcl_GetHashKey(q_hash_tab,q_entry); 
      query_term_data = (query_term_info *) Tcl_GetHashValue(q_entry);
      
      
      /* find it in the index */
      if (strchr(word_ptr,'#') != NULL)
	index_result = se_gettrunc_idx(idx,word_ptr);
      else
	index_result = se_getterm_idx(idx,
				      word_ptr, 0 /* don't normalize twice.*/,
				      &n_stopwords);	      


      if (index_result != NULL) {
	
	query_term_data->coll_freq = (float)index_result->num_entries;


      if (num_found_terms < 100)
	termidlist[num_found_terms++] = index_result->termid;
	
	for (i = 0; i < index_result->num_entries; i++) {
	  
	  if (index_result->entries[i].record_no > 0) { /* forget deleted ones */
	    entry = Tcl_FindHashEntry(hash_tab, 
				      (void *)index_result->entries[i].record_no);
	    
	    if (entry == NULL){
	      doc_wts = CALLOC(struct docsum,1);
	      
	      /* initialize the doc length info */
	      doc_wts->doc_length = 
		((se_getdoclen(index_result->entries[i].record_no, filename, idx) + 1) / 10) + 80;
	      
	      doc_wts->query_term_wt =
		(query_term_data->freq_wt + query_term_data->fb_wt)/(float)(total_q_length + 35.0);
	      
	      doc_wts->doc_term_wt = (float)
		log((double)((float)index_result->entries[i].term_freq/(float)doc_wts->doc_length));
	      
	      doc_wts->collection_term_wt = (float) 
	      log((double)((float)index_result->tot_freq/collection_length));
	      
	      doc_wts->numberMatchTerms = 1;
	      
	      
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
	      doc_wts->numberMatchTerms++;
	      
	      
	      doc_wts->query_term_wt += 
		(query_term_data->freq_wt + query_term_data->fb_wt)/(float)(total_q_length + 35.0);
	      
	      doc_wts->doc_term_wt += (float)
		log((double)((float)index_result->entries[i].term_freq/(float)doc_wts->doc_length));
	      
	      doc_wts->collection_term_wt += (float) 
		log((double)((float)index_result->tot_freq/collection_length));
	      
	    }    
	  }
	}
	/* FREE the temp index results */
	FREE(index_result);
      } /* if not null index_result... */
    }
    
    tot_docs = hash_tab->numEntries;
    
    if (diagnostic_get() == 4 && tot_docs > 0) {
      /* there were stopwords -- ignore */
      diagnostic_clear();
    }
  
    if (blindfeedback == 2)
      FREE(wt_res);
  
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
      
      int doclen, reallength;
      
      
      wt_res->entries[i].record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
      doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
      
      doclen = doc_wts->doc_length;
      reallength = doclen;
      
      CONST1 = 1.0 / ((float)(sqrt((double)doc_wts->numberMatchTerms)) + 1.0);
      
      log_odds = (c0 
		  + (c1 * (CONST1 * doc_wts->query_term_wt))
		  + (c2 * (CONST1 * doc_wts->doc_term_wt))
		  + (c3 * (CONST1 * doc_wts->collection_term_wt))
		  + (c4 * (float)doc_wts->numberMatchTerms));
      
      exp_log_odds = exp((double)-log_odds);
      
      wt_res->entries[i].weight = (float) 1.0/(1.0 + exp((double)-log_odds));
      
      
      if (statflag == 1) {
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
	Query_Stats->docstat[i].doclength = reallength;
	Query_Stats->docstat[i].querylength = (int)total_q_length;
	Query_Stats->docstat[i].num_match_terms = doc_wts->numberMatchTerms;
	Query_Stats->docstat[i].ndocs = ndocs;
	Query_Stats->docstat[i].dist_ndocs = dist_ndocs;
	Query_Stats->docstat[i].min_cf = min_cf;
	Query_Stats->docstat[i].max_cf = max_cf;
	Query_Stats->docstat[i].min_tf = doc_wts->min_tf;
	Query_Stats->docstat[i].max_tf = doc_wts->max_tf;
	Query_Stats->docstat[i].sum_entries = sum_entries;
	Query_Stats->docstat[i].min_entries = min_entries;
	Query_Stats->docstat[i].max_entries = max_entries;
	Query_Stats->docstat[i].X1 = doc_wts->query_term_wt;
	Query_Stats->docstat[i].X2 = doc_wts->doc_term_wt;
	Query_Stats->docstat[i].X3 = doc_wts->collection_term_wt;
	Query_Stats->docstat[i].X4 = c4 * (float)doc_wts->numberMatchTerms;
	Query_Stats->docstat[i].X5 = 0.0;
	Query_Stats->docstat[i].X6 = 0.0;
	Query_Stats->docstat[i].logodds = log_odds;
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
    
    wt_res->result_type = PROBABILISTIC_RESULT;
    
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
    
    ranked_res = se_rank_docs(wt_res);

    if ((idx->type & VECTOR_TYPE) == VECTOR_TYPE) { 

      if (blindfeedback == 1) {

	
	bf_docid_1 = ranked_res->entries[0].record_num;
	bf_docid_2 = ranked_res->entries[1].record_num;
	
	for (i = 0; i < blindfeedback_nrecs && i < ranked_res->num_hits; i++) {
	  
	  fb_vector = se_get_vector_data(idx, ranked_res->entries[i].record_num);
	  
	  for (j = 0; j < fb_vector->num_terms; j++) {
	    char *term;
	    int coll_freq;
	    
	    term = se_get_term_by_id(idx, fb_vector->entries[j].termid, 
				     &coll_freq);
	    
	    q_entry = Tcl_FindHashEntry(q_hash_tab,term);
	    
	    if (q_entry == NULL){
	      query_term_data = CALLOC(query_term_info, 1);
	      query_term_data->countdocs = 1;
	      query_term_data->coll_freq = (float)coll_freq;
	      Tcl_SetHashValue(
			       Tcl_CreateHashEntry(
						   q_hash_tab,
						   term,
						   &exists),
			       (ClientData)query_term_data);
	      count_q_terms++;
	      FREE(term);
	    }
	    else {
	      query_term_data = (query_term_info *) 
		Tcl_GetHashValue(q_entry);
	      query_term_data->countdocs++;
	      FREE(term); /* this was allocated in se_get_term_by_id */
	    }
	    
	  }
	  
	}
	
	/* set up an array of pointers for the term data */
	rel_weights = (sortset *)CALLOC(sortset, count_q_terms);
	relwtentry=0;
	
	for (q_entry = Tcl_FirstHashEntry(q_hash_tab,&q_hash_search);
	     q_entry != NULL; q_entry = Tcl_NextHashEntry(&q_hash_search)) {
	  float N, Nt, R, Rt;
	  
	  /* adapted from Aitao's term weighting formula */
	  /* get the word/stem from the hash tab */
	  word_ptr = (char *)Tcl_GetHashKey(q_hash_tab,q_entry); 
	  query_term_data = (query_term_info *) Tcl_GetHashValue(q_entry);
	  
	  N = (float)ndocs;                       /* collection size */
	  Nt = (float)query_term_data->coll_freq; /* collection freq of term */
	  R = (float)blindfeedback_nrecs;         /* number of feedback docs */
	  Rt = (float)query_term_data->countdocs; /* top docs with term      */
	  
	  query_term_data->rel_weight = 
	    (float)(log(Rt + 0.5) + 
		    log(N - R - Nt + Rt + 0.5) -
		    log(R - Rt + 0.5) -
		    log(Nt - Rt + 0.5));
	  
	  /*  log((double)((query_term_data->countdocs + 0.5) 
	   *	       / ((float)blindfeedback_nrecs 
	   *	  - query_term_data->countdocs + 0.5)) 
	   *  / ((query_term_data->coll_freq - query_term_data->countdocs + 0.5) 
	   *  / (ndocs - query_term_data->coll_freq 
	   *    - (float)blindfeedback_nrecs + query_term_data->countdocs 
	   *    + 0.5)));
	   */
	  
	  rel_weights[relwtentry].ptr = query_term_data;
	  rel_weights[relwtentry++].termptr = word_ptr;
	  
	}
	
	
	/* now sort the term data on rel_weight */
	qsort ((void *)&rel_weights[0], (size_t)relwtentry,
	       (size_t)sizeof(sortset), compare_qt_data);
	
	/* mark the top ranked terms as keepers */
	count_bf_terms = 0;
	total_q_length = 0.0;
	
	for (i = 0; i < relwtentry; i++) {
	  if (rel_weights[i].ptr->countdocs < 3)
	    continue;
	  
	  if (count_bf_terms < blindfeedback_nterms) { 
	    
	    rel_weights[i].ptr->keep = 1;
	  }
	  if (rel_weights[i].ptr->orig_flag) {
	    /* keep it and set FB weight */
	    rel_weights[i].ptr->keep = 1;
	    query_term_data->fb_wt = query_term_data->freq_wt * 0.5;
	    total_q_length += query_term_data->freq_wt + query_term_data->fb_wt;
	  } 
	  else {
	    /* new term being added */
	    rel_weights[i].ptr->fb_wt = 0.5;
	    total_q_length += 0.5;
	    count_q_terms++;
	  }
	  count_bf_terms++;
	  
	  if (count_bf_terms > blindfeedback_nterms)
	    break;
	}
	
	
	/* free the extra points, will now access the data via the hash table */
	FREE(rel_weights);
	
	/* trim the rest of the terms from the hash table */      
	for (q_entry = Tcl_FirstHashEntry(q_hash_tab,&q_hash_search);
	     q_entry != NULL; q_entry = Tcl_NextHashEntry(&q_hash_search)) {
	  
	  /* get the word/stem from the hash tab */
	  word_ptr = (char *)Tcl_GetHashKey(q_hash_tab,q_entry); 
	  query_term_data = (query_term_info *) Tcl_GetHashValue(q_entry);
	  
	  if (query_term_data->keep)
	    continue;
	  
	  if (query_term_data->orig_flag) {
	    /* an original query term that didnt do well in the ranking */
	    total_q_length += query_term_data->freq_wt;
	    continue;
	  }
	  
	  /* keep not on, so trim it... */
	  Tcl_DeleteHashEntry(q_entry);
	  FREE(query_term_data);
	}

	
	/* Re-Init the hash table for collecting weights for each document */
	hash_tab = &hash_tab_data;
	Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
      

	blindfeedback = 2;
      }
      else {
	if (blindfeedback == 2) 
	  blindfeedback = 3;
	
      }

    }
    else {
      /* asked for blind feedback, but no vectors available, just do TREC2 */
      blindfeedback = 0;
    }

  } while (blindfeedback > 0 && blindfeedback <= 2);


  if (blindfeedback > 2) {
    /* we have done the blind feedback, insert the original top 2 docs */
    for (i = 0; i < ranked_res->num_hits; i++) {
      if (ranked_res->entries[i].record_num == bf_docid_1) {
	wt_res->entries[i].weight = wt_res->max_weight + 0.015;
	bf_weight_1 = wt_res->entries[i].weight;
      }
      if (ranked_res->entries[i].record_num == bf_docid_2) {
	wt_res->entries[i].weight = wt_res->max_weight + 0.01;
	bf_weight_2 = wt_res->entries[i].weight;	
      }	  
    }
    wt_res->max_weight = bf_weight_1;
    ranked_res = se_rank_docs(ranked_res);
  }


  /* all done with the query terms */
  FREE(searchstring);

      
  for (q_entry = Tcl_FirstHashEntry(q_hash_tab,&q_hash_search);
       q_entry != NULL; q_entry = Tcl_NextHashEntry(&q_hash_search)) {
	
    /* get the word/stem from the hash tab */
    query_term_data = (query_term_info *) Tcl_GetHashValue(q_entry);
    FREE(query_term_data);
  }

  Tcl_DeleteHashTable(q_hash_tab); /* clean up */
  

  return(ranked_res);
}












