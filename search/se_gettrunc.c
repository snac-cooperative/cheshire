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
*	Header Name:	se_gettrunc.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Truncated term or phrase searching code
*
*	Usage:          se_gettrunc (filename, indexname, search_string)
*
*			(or -- more efficient)
*                       se_gettrunc_idx (idx, search_string)
*
*	Variables:	idx is an idx_list_entry pointer
*
*	Return Conditions and Return Codes:	
*
*	Date:		12/15/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "cheshire.h"
#include "configfiles.h"
#ifdef WIN32
#define strncasecmp _strnicmp
#endif

/* forward declarations */
idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring);

extern void diagnostic_set();
extern char *conv_diacritics (char *data, int index_type, 
			      idx_list_entry *idx);

idx_result *se_gettrunc(char *filename, char *indexname, char *searchstring)
{

  DB *index_db;
  idx_list_entry *idx;

  /* make sure everything is open */
  index_db = 
    (DB *) cf_index_open(filename, indexname, INDEXFL);

  if (index_db == NULL)
    return (NULL);

  idx = cf_getidx_entry(filename, indexname);

  return(se_gettrunc_idx(idx,searchstring));

}

idx_result *se_gettrunc_idx(idx_list_entry *idx, char *searchstring)
{

  DB *index_db;
  int index_type;

  struct temp_results {
    int weight;
    int tot_freq;
  } *temp, *storage ;
  
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  idx_result *newinfo, *results;

  DBT keyval;
  DBT dataval;
  DBC *dbcursor;

  int returncode, exists, firstblock;
  int total_num_entries, sum_tot_freq;
  
  int entrynum;
  int resultentry;
  int i, more_matches;
  char *c;
  char *tempsearch;

  long record_num;
  int db_errno;
  int morphed=0;

  if (idx == NULL) {
    fprintf(LOGFILE, "idx is NULL in se_gettrunc_idx\n");
    return NULL;
  }

  if (idx->db == NULL) {
    fprintf(LOGFILE, "index %s not open in se_gettrunc_idx\n", idx->tag);
    return NULL;
  }
  
  index_db = idx->db;
  index_type = idx->type;

  /* if it is a numeric type, we just don't do it */
  if ((idx->type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) != 0) {
    return NULL;
  }

  /* Acquire a cursor for the database. */
  if ((db_errno = index_db->cursor(index_db, NULL, &dbcursor, 0)) != 0) {
    fprintf(LOGFILE, "se_gettrunc: no cursor?: %s\n", strerror(db_errno));
    return NULL;
  }
  
  /* do (or redo) normalization: this duplicates the string */
  tempsearch = normalize_key(searchstring, idx, &morphed, 1);

  if ((c = (char *)strrchr(tempsearch,'#')) != NULL)
    *c = '\0'; /* replace the last truncation symbol with null */

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));

  keyval.data = (void *) tempsearch;
  keyval.size = strlen(tempsearch) + 1;

  if (keyval.size < 2)  { /* don't permit searches on NULL strings */
    diagnostic_set(9,0,NULL);
    return(NULL);
  }

  db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
  
  if (db_errno == DB_NOTFOUND) { /* no hit in the index */
    dbcursor->c_close(dbcursor);
    return(NULL);
  }

  if (db_errno != 0) { /* DB error */
    diagnostic_set(2,db_errno,strerror(db_errno));
    dbcursor->c_close(dbcursor);
    return(NULL);
  }

  /* otherwise assume that the scan found some match */
  more_matches = 1;
  total_num_entries = 0;
  sum_tot_freq = 0;

  /* Allocate the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);

  do {
    if (strncasecmp(tempsearch, keyval.data, strlen(tempsearch)) != 0) 
      more_matches = 0;
    else {
      if (dataval.data != NULL) {
	/* allocate space for the posting info */
	newinfo = (idx_result *) 
	  CALLOC(int, 
		 ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
	memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
      
	if (newinfo->termid == 0) {
	  continue; /* get the next matching one */
	}
	
	else {
	  int i;

	  for (i = 0; i < newinfo->num_entries; i++) {

	    record_num = newinfo->entries[i].record_no;

	    entry = Tcl_FindHashEntry(hash_tab, (void *)record_num);
		
	    if (entry == NULL){ /* new record number */
	      storage = CALLOC(struct temp_results,1);
	      storage->weight = 1;
	      storage->tot_freq = newinfo->entries[i].term_freq;
	      sum_tot_freq  += newinfo->entries[i].term_freq;
	      total_num_entries++;
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
	      storage->tot_freq += newinfo->entries[i].term_freq;
	      sum_tot_freq  += newinfo->entries[i].term_freq;
	    }
	  }
	}
      }
      else { /* dataval.data was null */
	diagnostic_set(2,0,"dataval.data null from db->get in se_gettrunc");
 	return (NULL);
      }
      
      if(dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT) != 0)
	more_matches = 0; 
    } 
  } while (more_matches); 

  (void)dbcursor->c_close(dbcursor);

  if (total_num_entries > 0) {
    
    /* allocate a structure to hold the data */
    results = (idx_result *) CALLOC(char, (sizeof(idx_result) + 
				    (total_num_entries 
				     * sizeof(idx_posting_entry))));
    results->num_entries = total_num_entries;
    results->termid = 0;
    results->tot_freq = sum_tot_freq;
    results->term = tempsearch;
    
    /* scan the hash table and fill in each entry */
    i = 0;

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
      
      record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
      storage = (struct temp_results *) Tcl_GetHashValue(entry);
      
      results->entries[i].record_no = record_num;
      results->entries[i].term_freq = (float)storage->tot_freq;
      i++;
      free(storage); /* not needed any more */
    }
    
    Tcl_DeleteHashTable(hash_tab); /* clean up */
    diagnostic_clear(); /* ? */
    return(results);
  }
  else
    return(NULL); /* no matches in index */

}









