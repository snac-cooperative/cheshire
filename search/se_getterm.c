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
*	Header Name:	se_getterm.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	simple single-term searching code
*
*	Usage:		(old version)
*                       se_getterm (filename, indexname, search_string)
*
*	Usage:		(new version -- more efficient)
*                       se_getterm_idx (idx, search_string)
*
*	Variables:	idx is an idx_list_entry pointer
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/14/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"

/* forward declarations */
Tcl_HashTable *cf_getstoplist(char *filename, char *indexname);
idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern void diagnostic_set();
extern void diagnostic_clear();

idx_result *se_getterm_idx(idx_list_entry *idx, char *searchstring, 
			   int normalize, int *search_stopword_count)
{

  DB *index_db;
  int index_type;

  char *word_ptr;

  Tcl_HashTable *stoplist_hash;

  idx_result *newinfo;

  DBT keyval;
  DBT dataval;
  int returncode;
  
  int offset;	   /* offset retrieved from assoc file */
  int postoffset; /* offset retrieved from assoc file */
  
  idx_result *results;

  int morphed;

  offset = 0L;
  postoffset = 0L;

  if (idx == NULL) {
    diagnostic_set(2,0,"idx is NULL in se_getterm_idx");
    return NULL;
  }

  if (idx->db == NULL) {
    diagnostic_set(2,0,"index not open in se_getterm_idx");
    return NULL;
  }
  
  index_db = idx->db;
  index_type = idx->type;

  if (idx->stopwords_file != NULL)
    stoplist_hash = &idx->stopwords_hash;
  else
    stoplist_hash = NULL;

  if (normalize ) {
    word_ptr = 
      normalize_key(searchstring, idx, &morphed, 1);
  }
  else
    word_ptr = searchstring;

  if (word_ptr != NULL) {
  
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));

    keyval.data = (void *) word_ptr;
    keyval.size = strlen(word_ptr) + 1;

    returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 

    if (returncode == DB_NOTFOUND) { 
      /* no hit in the index */
      return(NULL);
    }

    if (returncode != 0) { /* DB error */
      char x[100];

      sprintf (x,"DB error in se_getterm -- %s ", strerror(returncode));
      diagnostic_set(2,returncode,x);
      return(NULL);
    }

    /* otherwise assume that the get was OK */
    if (dataval.data != NULL) {

      /* allocate space for the posting info */
      newinfo = (idx_result *) 
	CALLOC(int, 
	       ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
      memcpy((char *)newinfo, (char *)dataval.data, dataval.size);

      newinfo->term = word_ptr;
      
      return(newinfo);
    } 
    else { /* dataval.data was null */
      diagnostic_set(2,0,"dataval.data null from db->get in se_getterm");
      return (NULL);
    }
    /* otherwise should have filled in results */
    diagnostic_clear(); 
    return(results);
  } 
  else {
    *search_stopword_count += 1;
    return (NULL); /* word was normalized out of existence (stopword) */
  }
}


/* this is the old interface to retrieve terms via index names */
idx_result *se_getterm(char *filename, char *indexname, char *searchstring)
{

  DB *index_db;
  idx_list_entry *idx;
  int n_stops = 0;

  /* make sure everything is open */
  index_db = 
    (DB *) cf_index_open(filename, indexname, INDEXFL);

  if (index_db == NULL)
    return (NULL);

  idx = cf_getidx_entry(filename, indexname);

  return(se_getterm_idx(idx,searchstring, 1, &n_stops));

}









