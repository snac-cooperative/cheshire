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
/* ****************** TREC III AD HOC RETRIEVAL FORMULA ***************
 *  Y = -3.51 + [ 37.4*X1 + .330*X2 - .1937*X3 ] / (1+N^.5) + .0929 N
 * From: wcooper (William S. Cooper)
 *	AD HOC FORMULA FOR TREC-3
 * Logodds of relevance of doc to query = 
 *  -3.70 + 1.269 X1 -0.310 X2 + 0.679 X3 -0.0674 X4 + 0.223 X5 + 2.01 X6
 * where
 * X1 = mean logged absolute frequency of match stems in query, i.e. the
 *   sum of log(QAF) over all stems common to both query and doc,
 *   divided by the number n of such terms.
 * X2 = square root of query length, i.e. square root of number of
 *   (nonunique) stems in the query.  (Not meaned, summed or logged.)
 * X3 = mean logged absolute frequency of match stems in doc, i.e.
 *   sum of log(DAF) over all stems common to both query and doc,
 *   all divided by n.
 * X4 = square root of doc length, i.e. square root of number of
 *   (nonunique) stems in the doc.  (Not meaned, summed or logged.)
 * X5 = mean logged IDF of match stems, i.e. sum of 
 *   log(749000/ # of docs with term), all divided by n
 * X6 = log(n)
 * Explanation: X1, X3, and X5 each tells something about the average
 * `quality' of the match terms; X6 reflects the `quantity' of the match
 * terms.
 * To implement this equation I suggest you store with each query stem 
 * the query stem weight
 *   1.269 x1 -0.310 x2 + 0.223 x5
 * where x1 is that individual stem's logged QAF, x2 is the square root
 * of the query length, and x5 is the stem's logged IDF.  With each doc
 * stem, store the weight
 *   0.679 x3 - 0.0674 x4
 * where x3 is the stem's logged DAF and x4 is the square root of the doc
 * length.  Then at run time you can compute the logodds of relevance as
 * Logodds = -3.70 + [ sum over all match stems of (query stem weight
 *                 + doc stem weight) ] / n + 2.01 log(n)
 * and rank the output by the logodds as usual.
 * To compute the probabilities of relevance for the ranking
 * coefficient column of the final entry we will submit, you can use the
 * formula
 *                            exp( Logodds )
 * Probability =  0.75 * ---------------------
 *                          exp( Logodds ) + 1
 * where the 0.75 is an arbitrary fudge factor to compensate for the
 * shift from learning data to real data.
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


weighted_result *se_prob (char *filename, char *indexname, char *insearchstring)
{

  char *word_ptr, *norm_word_ptr;

  float sumwt;
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
  float docwt;
  float logIDF;
  float logQAF;
  float logDAF;
  double log_odds;
  int total_q_terms = 0;
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
  /* TREC (default) coefficients */
  float c0 = -3.70;
  float c1 = 1.269;
  float c2 = -0.310;
  float c3 = 0.679;
  /* #define c4 -0.0674 */
  /* tweaked value was: float c4 = -0.021; */
  /* Orig: float c4 = -0.0674; */
  float c4 = -0.0674;
  float c5 = 0.223;
  /* #define c6 2.01 */
  /* tweaked value was: float c6 = 4.01; */
  /* Orig: float c6 = 2.01; */
  float c6 = 2.01;
  ranking_parameters *rank_parm;

  struct docsum{
    int num_qterms;
    float sum_logQAF;
    float sum_logDAF;
    float sum_logIDF;
    int min_tf;
    int max_tf;
  } *doc_wts;


  
  weighted_result *wt_res, *ranked_res, *se_rank_docs();

  char *lastptr;


  
#ifdef DUMPDEBUG
  FILE *dumpfile;
  int d, dumplen;

  dumpfile = fopen("/tmp/cheshire_dump_debug_file", "w");

  fprintf(dumpfile, "INSEARCHSTRING= '%s'\n", insearchstring);

  dumplen = strlen(insearchstring);
  fprintf(dumpfile, "INSEARCHSTRING HEX= ", insearchstring);  
  for (d = 0; d < dumplen; d++) {
    fprintf(dumpfile, "%x ", insearchstring[d]);  
  }
  fprintf(dumpfile, "\n", insearchstring);  

#endif

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
    if (rank_parm->type == 0) {
      /* it is Logistic regression parameter */
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
	break;
      case 6:
	c6 = rank_parm->val;
	break;
	/* getting to here means that the defaults above are used */
      }
    }
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

  /* find first token */
  word_ptr = strtok_r (searchstring, breakletters, &lastptr);
  do { /* build the query elements */

    norm_word_ptr = 
      normalize_key(word_ptr, idx, &dummy, 1);
    
    if (norm_word_ptr != NULL) {
	q_entry = Tcl_FindHashEntry(q_hash_tab,norm_word_ptr);
	
#ifdef DUMPDEBUG
  fprintf(dumpfile, "NORMWORD= '%s'\n", norm_word_ptr);
  dumplen = strlen(norm_word_ptr);
  fprintf(dumpfile, "NORMWORD HEX= ");  
  for (d = 0; d < dumplen; d++) {
    fprintf(dumpfile, "%x ", norm_word_ptr[d]);  
  }
  fprintf(dumpfile, "\n", insearchstring);  

#endif


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
      double IDF;

      if (num_found_terms < 100)
	termidlist[num_found_terms++] = index_result->termid;

      /* have the frequency information for this term, so we'll */
      /* figure out the term weight                             */


      sumwt = (float) index_result->tot_freq;
      IDF = 
	(double)ndocs / (double)index_result->num_entries;

      if (min_cf == 0 || min_cf > index_result->tot_freq) 
	min_cf = index_result->tot_freq;
      if (max_cf == 0 || max_cf < index_result->tot_freq)
	max_cf = index_result->tot_freq;

      sum_entries =+ index_result->num_entries;
	
      if (min_entries == 0 || min_entries > index_result->num_entries) 
	min_entries = index_result->num_entries;
      if (max_entries == 0 || max_entries < index_result->num_entries)
	max_entries = index_result->num_entries;

      for (i = 0; i < index_result->num_entries; i++) {

	if (index_result->entries[i].record_no > 0) { /* forget deleted ones */
	  entry = Tcl_FindHashEntry(hash_tab, 
				    (void *)index_result->entries[i].record_no);

	  if (entry == NULL){
	    doc_wts = CALLOC(struct docsum,1);
	    
	    doc_wts->num_qterms = 1;
	    doc_wts->sum_logQAF = (float)log((float)q_term_freq);
	    doc_wts->sum_logDAF = (float)log((float)
					     index_result->entries[i].term_freq);
	    doc_wts->sum_logIDF = (float)log(IDF);
	    doc_wts->min_tf = index_result->entries[i].term_freq;
	    

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
	    doc_wts->sum_logQAF += log((float)q_term_freq);
	    doc_wts->sum_logDAF += log((float)index_result->entries[i].term_freq);
	    doc_wts->sum_logIDF += log(IDF);

	    if (doc_wts->min_tf > index_result->entries[i].term_freq) 
	      doc_wts->min_tf = index_result->entries[i].term_freq;
	    if (doc_wts->max_tf < index_result->entries[i].term_freq) 
	      doc_wts->max_tf = index_result->entries[i].term_freq;
	    
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

#ifdef DUMPDEBUG
  fprintf(dumpfile, "TOTAL DOCS= '%d'\n", tot_docs);
  fclose(dumpfile);
#endif


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
    double X1, X2, X3, X4, X5, X6;
    int doclen, reallength;


    wt_res->entries[i].record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);

    doclen = se_getdoclen(wt_res->entries[i].record_num, filename, idx);
    reallength = doclen;
    
    dist_ndocs = cf_get_dist_docsize(filename, 
				     wt_res->entries[i].record_num, 
				     NULL, NULL, doclen);

    X1 =  (double)doc_wts->sum_logQAF / (double)doc_wts->num_qterms;
    X2 =  sqrt((double)total_q_terms);
    X3 =  (double) doc_wts->sum_logDAF /(double)doc_wts->num_qterms;
    /* doclen is not stems, but chars so it may need adjustment later */
    /* should be size of doc in words  */
    X4 =  (double) sqrt((double)(doclen/10)); 
    X5 =  (double) doc_wts->sum_logIDF /(double) doc_wts->num_qterms;
    X6 = (double) log((double)doc_wts->num_qterms);
    
    if (idx->type & NORM_WITH_FREQ) {
      /* Collection weighting needs to be adjusted for collections */
      /* change the weights based on query size */
      
      if (doc_wts->num_qterms < 10)
	/* test weighting */
	/* log_odds = (float)(1.12201 + (-0.00000634 * (double)doclen) 
	   + (-0.03244 * (double)total_q_terms) 
	   * + (-0.1860 * (double)doc_wts->num_qterms) *
	   + (0.00113 * (double)dist_ndocs) 
	   + (-0.000033 * (double)min_cf)
	   + (-0.000001 * (double)max_cf) 
	   + (0.00129 * (double)doc_wts->min_tf)
	   + (0.000274 * (double)doc_wts->max_tf) 
	   + (-0.00069 * (double)sum_entries)
	   + (0.00712 * (double)min_entries) 
	   + (-0.01567 * (double)max_entries)
	   + (-0.55762 + X1) + (0.70367 * X2) 
	   + (0.91733 * X3) + (1.3671 * X5) 
	   + (-0.52116 * X6));
	*/
	
	/*  Original distrib weights 
	 *   c0 = -3.70
	 *  c1 = 1.269
	 *  c2 = -0.310
	 *  c3 = 0.679
	 *  c4 = -0.021
	 *  c5 = 0.223
	 *  c6 = 4.01
	 */
	log_odds = (float) c0 + (c1 * X1) + (c2 * X2) + (c3 * X3) 
	  + (c4 * 316.227766) + (c5 * X5) + (c6 * X6);
	 
      /* Test with smaller sample  
       * log_odds = (float) -1.4335 + (-1.9839 * X2) + (0.5953 * X3) 
       * + (0.00536 * X4) + (2.4004 * X5) + (1.6455 * X6);
       */
      /* orig tiered
       *!       log_odds = (float) 0.1261 + (-4.5088 * X2) + (0.6675 * X3)
       *!         + (0.00283 * X4) + (3.1829 * X5) + (2.9530 * X6);
       */
      else if (doc_wts->num_qterms >= 10 && doc_wts->num_qterms < 50) 
	log_odds = (float) -7.0103 + (2.3188 * X1) + (-1.1257 * X2) + (1.0695 * X3) 
	  + (-0.00294 * X4) + (5.9174 * X5) + (2.3612 * X6);
      else 
	log_odds = (float) -20.9850 + (9.6801 * X1) + (-1.8669 * X2) + (1.1921 * X3) 
	  + (-0.00537 * X4) + (6.2501 * X5) + (7.5491 * X6);

      /* this is a fudge adding some boolean weighting */
      /*if (doc_wts->num_qterms >= (total_q_terms / 2))
       *	log_odds += 3;
       */

      wt_res->entries[i].weight = 0.75 * (exp(log_odds) / (1 + exp(log_odds)));
    }
    else {
      /* the original standard formulation */
      log_odds = (float) c0 + (c1 * X1) + (c2 * X2) + (c3 * X3) 
	+ (c4 * X4) + (c5 * X5) + (c6 * X6);
    
      wt_res->entries[i].weight = 0.75 * (exp(log_odds) / (1 + exp(log_odds)));
    }

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
      Query_Stats->docstat[i].querylength = total_q_terms;
      Query_Stats->docstat[i].num_match_terms = doc_wts->num_qterms;
      Query_Stats->docstat[i].ndocs = ndocs;
      Query_Stats->docstat[i].dist_ndocs = dist_ndocs;
      Query_Stats->docstat[i].min_cf = min_cf;
      Query_Stats->docstat[i].max_cf = max_cf;
      Query_Stats->docstat[i].min_tf = doc_wts->min_tf;
      Query_Stats->docstat[i].max_tf = doc_wts->max_tf;
      Query_Stats->docstat[i].sum_entries = sum_entries;
      Query_Stats->docstat[i].min_entries = min_entries;
      Query_Stats->docstat[i].max_entries = max_entries;
      Query_Stats->docstat[i].X1 = X1;
      Query_Stats->docstat[i].X2 = X2;
      Query_Stats->docstat[i].X3 = X3;
      Query_Stats->docstat[i].X4 = X4;
      Query_Stats->docstat[i].X5 = X5;
      Query_Stats->docstat[i].X6 = X6;
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

  FREE(searchstring);

  return(se_rank_docs(wt_res));
}












