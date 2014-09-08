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
*	Header Name:	in_kwdel.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routine to delete items from keyword index and 
*                       postings entries for the Cheshire II system. This uses 
*                       the configfile structure to open the appropriate
*                       index files.
*
*	Usage:		in_kwdel (filename, recnum, DEDUPONLY)
*                       filename can be the full pathname or short "nickname"
*                       recnum is the logical record ID number for the record.
*                       dedup_flag is a flag that forces ONLY DUPLICATE 
*                       entries in postings to be removed.
*
*                       NOTE that in_kwdel must be called before the record
*                       is deleted from the marc file.
*
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or 0 (FAIL) on error;
*
*	Date:		11/21/93
*	Revised:	11/2/94, 10/6/96
*	Version:	2.0
*	Copyright (c) 1993, 1994 , 1996.  The Regents of the University 
*                                         of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"
#include "bitmaps.h"

extern config_file_info *cf_info_base;
extern int cf_getfiletype(char *filename);
extern int in_extract(SGML_Document *sgml_rec, char **xkey_buffer, 
		      idx_key *in_key,
		      Tcl_HashTable *hash_tab, int recnum, idx_list_entry *idx,
		      SGML_Tag_Data *comptagdata, int batch_flag);
extern int cf_sync_global_data(idx_list_entry *idx) ;


int in_del_bitmap_entry(idx_list_entry *idx, int recnum, int termid);


int in_kwdel (char *filename, int recnum, int dedup_flag)
{
  DB *index_db;
  FILE *index_stoplist;
  
  idx_list_entry *idx, *cf_getidx();
  
  SGML_Document *sgmlrec;   /* SGML record from mainfile */
  SGML_Document *GetSGML();
  extern int sgml_parse_error;
  
  char *word_ptr;
  char *xkey_buffer = NULL;
  Tcl_HashTable *hash_tab, hash_tab_data, *stoplist_hash;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  
  HT_DATA *ht_data;
  
  idx_result *newinfo; 
  
  DBT keyval;
  DBT dataval;
  int returncode, postreturn;
  int main_file_type;  
  int dedup_single = 0;
  int reduce_rec_count = 0;
  
  if (cf_info_base == NULL) {
    fprintf(LOGFILE,"cf_info_base is NULL in in_kwdel -- need to initialize\n");
    fflush(LOGFILE);
    return(-3);
  }
  
  main_file_type = cf_getfiletype(filename);
  if (main_file_type != FILE_TYPE_SGMLFILE 
      && main_file_type != FILE_TYPE_SGML_DATASTORE
      && main_file_type != FILE_TYPE_XML
      && main_file_type != FILE_TYPE_XML_NODTD
      && main_file_type != FILE_TYPE_XML_DATASTORE
      && main_file_type != FILE_TYPE_MARC_DATASTORE
      && main_file_type != FILE_TYPE_CLUSTER_DATASTORE
      && main_file_type != FILE_TYPE_MARCFILE
      && main_file_type != FILE_TYPE_CLUSTER) {
    fprintf(LOGFILE,"file %s is not SGML or CLUSTER in in_kwdel\n", filename);
    fflush(LOGFILE);
    return(-3);
  }
  
  
  
  /* Read the record from the sgml file and parse */
  sgmlrec = GetSGML(filename,recnum);
  
  if (sgmlrec == NULL) { 
    if (sgml_parse_error != 0) {
      fprintf(LOGFILE, "in_kwdel: SGML parse error for record %d\n",recnum);
      fflush(LOGFILE);
      return(-2);
    }
    else {
      if (errno == 1000) /* deleted record -- ignore it */
	return (SUCCEED);
      else { /* some unknown error */
	fprintf(stderr, "in_kwdel: error reading record %d errno %d\n",
		recnum, errno);
	fprintf(LOGFILE, "in_kwdel: error reading record %d errno %d\n",
		recnum, errno);
	fflush(LOGFILE);
	return(-1);
      }
    }
    
  }
  
  /* Allocate the hash table for collecting the words for each document */
  hash_tab = &hash_tab_data;
  
  
  for (idx = cf_getidx(filename) ; idx ; idx = idx->next_entry) {
    
    index_db = 
      (DB *) cf_index_open(filename, idx->tag, INDEXFL);
    index_stoplist =
      (FILE *) cf_index_open(filename, idx->tag, INDEXSTOPWORDS);
    
    Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);
    
    if (index_stoplist != NULL) {
      stoplist_hash = (Tcl_HashTable *) &idx->stopwords_hash;
    }
    
    reduce_rec_count = 0;
    
    /* recursively traverse the sgml data and match against index keys */
    /* for this index -- extract matching keys and put them into the   */
    /* hash table for addition to the index file.                      */
    
    in_extract(sgmlrec, &xkey_buffer, idx->keys,
	       hash_tab, recnum, idx, NULL,
	       0 /* batch flag */);
    
    
    /* Initialize the key/data pair so the flags aren't set. */
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    
    /* go through the hash table for this index and remove */
    /* the new terms and the postings information          */

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
	 entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
      
      postreturn = FAIL;
      
      /* get the word */
      word_ptr = Tcl_GetHashKey(hash_tab,entry); 
      /* and the frequency info */
      ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
      
      keyval.data = (void *) word_ptr;
      keyval.size = strlen(word_ptr) + 1;
      
      returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
      
      if (returncode == 0) { /* the word is in the index */
	int temprecnum = 0;
        int tempfreq = 0;
	int singleton_id = 0;
	int singleton_freq = 0;
	
	dedup_single = 0;
	
	/* allocate space for the posting info */
	newinfo = (idx_result *) 
	  CALLOC(int, 
		 ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
	memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
	
	if (newinfo->num_entries == 1) {
	  /* delete it from the index */
	  returncode = index_db->del(index_db, NULL, &keyval, 0);
	  if (returncode != 0)
	    fprintf(LOGFILE,"error on index delete errno is %d: %s\n", 
		    errno, strerror(errno)); 
	  returncode = index_db->sync(index_db, 0); 
	  if (returncode != 0)
	    fprintf(LOGFILE,"error on index sync errno is %d\n", errno); 
	}
	else { /* there will be postings records */
	  int i;
	  int foundflag = 0;

	  if (idx->type & BITMAPPED_TYPE) {
	    in_del_bitmap_entry(idx, recnum, newinfo->termid);
	    foundflag = 1;
	    ht_data->termfreq = 1;
	  }
	  else {
	    /* change the postings information */
	    for (i = 0; i < newinfo->num_entries; i++) {
	      if (foundflag) { /* migrate all the entries */
		newinfo->entries[i-1] = newinfo->entries[i];
	      }
	      if (newinfo->entries[i].record_no == recnum) 
		foundflag = 1;
	    }
	  }
	  /* OK to delete, there was a matching record number */
	  if (foundflag) {
	    newinfo->tot_freq -= ht_data->termfreq;
	    newinfo->num_entries--;
	    idx->GlobalData->tot_occur -= ht_data->termfreq;      
	    reduce_rec_count = 1;
	      
	    dataval.data = newinfo;
	    /* don't reduce size of the entry for bitmap data */
	    if (idx->type & BITMAPPED_TYPE != BITMAPPED_TYPE)
	      dataval.size -= sizeof(idx_posting_entry);
	    /* and re-write it to the index */
	    returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
	    
	    if (returncode != 0) { 
#ifdef DEBUGIND
	      printf(LOGFILE, "\nNon-zero return in db put of found term: %d\n",
		     returncode);
#endif
	      if (returncode == DB_RUNRECOVERY) {
		fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
		exit(999);
	      } 
	      if (returncode == DB_LOCK_NOTGRANTED) {
		fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
		exit(999);
	      } 
	      if (returncode == DB_LOCK_DEADLOCK) {
		fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
		exit(999);
	      } 
	    }
	  }
	}
      } 
      
      /* all done with this term data hash entry */ 	
      FREE(ht_data);
      
    }
    
    /* clear out out the current hash table for this index */
    Tcl_DeleteHashTable(hash_tab);
    
    /* update the Global information for the index */
    if (reduce_rec_count == 1) {
      if (dedup_single == 0) {
	idx->GlobalData->tot_numrecs--;
	cf_sync_global_data(idx);
      }
    }
  }
  return (SUCCEED);
  
}







