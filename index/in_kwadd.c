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
*	Header Name:	in_kwadd.c
*
*	Programmer:	Ray R. Larson
*                       Jerome McDonough
*
*	Purpose:	routine to add all keyword index and postings 
*                       as well as all exact key index and postings
*                       entries for the Cheshire II system. This uses 
*                       the configfile structure to open the appropriate
*                       index files.
*
*	Usage:		in_kwadd (filename, recnum, batchflag, tempdir)
*                       filename can be the full pathname or short "nickname"
*                       recnum is the logical record ID number for the record.
*                       if batch_flag is not 0 batch processing is being used.
*                       if batch_processing is on, tempdir can be used to
*                        specify where temporary sort files are put.
*          
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or >0 (FAIL) on error;
*                       unable to find record = -1
*                       parsing error         = -2
*                       other error           = -3
*
*	Date:		10/31/93
*	Revised:	11/7/93 (bug fixes)
*                       07/19/93 (Major Revision to Handle SGML records)
*                       
*	Version:	1.0
*	Copyright (c) 1999.  The Regents of the University of California
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
extern int errno;

batch_files_info *batch_files = NULL;

/* external function declarations */

extern int cf_getfiletype(char *filename);

extern void PutPositionInfo(idx_list_entry *idx, int recnum, int termid, 
		     HT_DATA *ht_data);

extern int in_components (config_file_info *cf, char *filename, int recnum, 
		   SGML_Document *sgmlrec, int batch_flag, 
		   char *temp_file_dir);

extern int in_extract(SGML_Document *sgml_rec, char **xkey_buffer, idx_key *in_key,
		      Tcl_HashTable *hash_tab, int recnum, 
		      idx_list_entry *idx, SGML_Tag_Data *comptagdata, 
		      int batch_flag, batch_files_info *bf);

extern int in_kwdel (char *filename, int recnum, int dedup_flag);

extern int fil_del (int recno, char *filename);

extern void free_doc(SGML_Document *doc );

extern int in_cluster (char *filename, int recnum, SGML_Document *in_sgmlrec);


int in_add_bitmap_entry(idx_list_entry *idx, int recnum, char *word_ptr,
			bitmap_t *in_bitmap, int n_new_entries);

/* The real thing follows */

int 
in_kwadd (char *filename, int recnum, int batch_flag, char *temp_file_dir)
{

  DB *index_db;
  FILE *index_stoplist;
  FILE *index_expansion;
  FILE *index_docsizes;

  config_file_info *cf, *find_file_config();
  idx_list_entry *idx, *cf_getidx();
  
  char *file_name;
  char temp[500];
  char *word_ptr;
  char *xkey_buffer = NULL;
  int old_record; 
  char *tempptr;
  batch_files_info *bf = NULL, *prev = NULL;

  SGML_Document *sgmlrec, *GetSGML();    /* SGML record from mainfile */
  extern int sgml_parse_error;

  Tcl_HashTable *hash_tab, hash_tab_data, *stoplist_hash;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  
  int items_output = 0;
  int total_docsize = 0;
  int docsize_set_flag = 0;

  HT_DATA *ht_data;
  POSDATA *posdata, *prevpos;

  idx_result *newinfo, tempinfo;
  DBT keyval;
  DBT dataval;
  int returncode;
  int main_file_type;  
  char *fmode;
  
  if (cf_info_base == NULL) {
    fprintf(LOGFILE,"cf_info_base is NULL in in_kwadd -- need to initialize\n");
    fflush(LOGFILE);
    return(-3);
  }
  
  main_file_type = cf_getfiletype(filename);
  if (main_file_type != FILE_TYPE_SGMLFILE
      && main_file_type != FILE_TYPE_XML
      && main_file_type != FILE_TYPE_XML_NODTD
      && main_file_type != FILE_TYPE_XML_DATASTORE
      && main_file_type != FILE_TYPE_MARCFILE
      && main_file_type != FILE_TYPE_MARC_DATASTORE
      && main_file_type != FILE_TYPE_SGML_DATASTORE
      && main_file_type != FILE_TYPE_CLUSTER) {
    fprintf(LOGFILE,"file %s is not SGML or CLUSTER in in_kwadd\n", filename);
    fflush(LOGFILE);
    return(-3);
  }
  
  /* Read the record from the sgml file and parse */
  sgmlrec = GetSGML(filename,recnum);
  
  if (sgmlrec == NULL) { 
    if (sgml_parse_error != 0) {
      fprintf(LOGFILE, "in_kwadd: SGML parse error for record %d\n",recnum);
      fflush(LOGFILE);
      return(-2);
    }
    else {
      if (errno == 1000) /* deleted record -- ignore it */
	return (SUCCEED);
      else { /* some unknown error */
	fprintf(stderr, "in_kwadd: error reading record %d errno %d\n",
		recnum, errno);
	fprintf(LOGFILE, "in_kwadd: error reading record %d errno %d\n",
		recnum, errno);
	fflush(LOGFILE);
	return(-1);
      }
    }
    
  }
  
  cf = find_file_config(filename);
  
  /* Allocate the hash table for collecting the words for each document */
  hash_tab = &hash_tab_data;
  
  if (cf->primary_key && batch_flag == 0) {
    /* this file has a primary key index, so we process it first */
    index_db = (DB *) cf_index_open(filename, 
				    cf->primary_key->tag, INDEXFL);

    index_stoplist = (FILE *) cf_index_open(filename, 
					    cf->primary_key->tag, 
					    INDEXSTOPWORDS);
    
    Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);
    
    if (index_stoplist != NULL) {
      stoplist_hash = (Tcl_HashTable *) &cf->primary_key->stopwords_hash;
    }
    else 
      stoplist_hash = NULL;
    
    in_extract(sgmlrec, &xkey_buffer, cf->primary_key->keys,
	       hash_tab, recnum, cf->primary_key, NULL, batch_flag, NULL);

    /* Initialize the key/data pair so the flags aren't set. */
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search); 
	 entry != NULL; 
	 entry = Tcl_NextHashEntry(&hash_search)) {
      /* get the word */
      word_ptr = Tcl_GetHashKey(hash_tab,entry); 
      /* and the frequency info */
      ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
      /* note: proximity information not used in primary keys */
      
      keyval.data = (void *) word_ptr;
      keyval.size = strlen(word_ptr) + 1;
      
      returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
	
      if (returncode == 0) { /* found the word in the index already */
	if (cf->primary_key->type & PRIMARYREPLACE) { 
	  /* replace the old index items with the new ones */
	  fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	  /* allocate space for the posting info */
	  newinfo = (idx_result *) 
 	    CALLOC(int, (dataval.size / sizeof(int)));
	  memcpy((char *)newinfo, (char *)dataval.data, dataval.size);

	  if (newinfo->num_entries == 1) { /* it better be for primary key! */
	    old_record = newinfo->entries[0].record_no;
	    if (old_record > recnum) {
	      fprintf(LOGFILE, "Record number %d will REPLACE %d\n", 
		      old_record, recnum);

	      /* just return -- don't replace newer records with old ones */
	      return 0; 
	    }
	    fprintf(LOGFILE, "Record number %d will REPLACE %d\n", 
		    recnum, old_record);
	    /* first "delete" all of the index info for the old record */
	    in_kwdel(filename, old_record, 0);
	    fil_del(old_record, filename);
	    FREE(newinfo);
	  }
	  /* now just continue and index the new record */
	} 
	else {
	  /* otherwise we just ignore the new record, noting it in the logs */
	  fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	  fprintf(LOGFILE, "Record number %d IGNORED\n", recnum);
	  /* all done with this term data hash entry */
	  FREE(ht_data);
	  /* clear out out the current hash table for this index */
	  Tcl_DeleteHashTable(hash_tab);
	  
	  if (sgmlrec)  { 
	    free_doc(sgmlrec);
	  }

	  return (SUCCEED);
	}
      }
      /* all done with this term data hash entry */
      FREE(ht_data);
    }
    /* clear out out the current hash table for this index */
    Tcl_DeleteHashTable(hash_tab);      
    /* if the key is not found we just continue processing the record */
  }
  
  for (idx = cf_getidx(filename) ; idx ; idx = idx->next_entry) {
#ifdef DEBUGIND
    printf("index tag = %s\n", idx->tag);
#endif

    index_db = (DB *) cf_index_open(filename, idx->tag, INDEXFL);
    index_stoplist = (FILE *) cf_index_open(filename, idx->tag, INDEXSTOPWORDS);
    index_expansion = (FILE *) cf_index_open(filename, idx->tag, INDEXEXPANSION);

    Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);
    
    if (batch_flag) { /* this is a batch run, set up the file for output */

      for (bf = batch_files; bf != NULL; bf = bf->next) {
	if (idx == bf->idx)
	  break;
	prev = bf;
      }
      if (bf == NULL || batch_files == NULL) {
	/* first entry */
	bf = CALLOC(batch_files_info, 1);
	if (batch_files == NULL) 
	  batch_files = bf;
	else
	  prev->next = bf;
	bf->idx = idx;
	bf-> index_name = idx->name;
	bf->load_flag = 0; /* always zero except in batch_load fixes */
	if (temp_file_dir) {
	  file_name = strrchr(bf->index_name, '/');
	  file_name++;
	  sprintf(temp,"%s/%s_BATCHTEMP", temp_file_dir, file_name);
	}
	else
	  sprintf(temp,"%s_BATCHTEMP", bf->index_name);
#ifdef WIN32
	  fmode = "wb";
#else 
	  fmode = "w";
#endif

	if ((bf->outfile = fopen(temp, fmode)) == NULL) {
	  fprintf(LOGFILE, "Couldn't open batch file %s\n", temp);
	  return (FAIL);
	}
      }
    }
 
    if (index_stoplist != NULL) {
      stoplist_hash = (Tcl_HashTable *) &idx->stopwords_hash;
    }
    else 
      stoplist_hash = NULL;
    
    /* Go through the sgml data and match against index keys           */
    /* for this index -- extract matching keys and put them into the   */
    /* hash table for addition to the index file.                      */

    in_extract(sgmlrec, &xkey_buffer, idx->keys, hash_tab, recnum, 
	       idx, NULL, batch_flag, bf);
    
    if (idx->type & PAGEDINDEX || idx->type & NORM_WITH_FREQ) {
      /* for both Paged index files and Freq files (extracted by scan) */
      /* do nothing -- already output at a lower level than in_extract */
      /* except clear out out the current hash table for this index */
      Tcl_DeleteHashTable(hash_tab);
      if (idx->type & NORM_WITH_FREQ) {
	/* output the total docsize from the source file */
	index_docsizes = (FILE *) cf_open(filename, DOCSIZEFILE);
	fseek(index_docsizes, (recnum * sizeof(int)), 0);
	sgmlrec->dist_docsize = cf_get_dist_docsize(filename, recnum,
						    sgmlrec, cf);
	fwrite(&(sgmlrec->dist_docsize), sizeof(int), 1, index_docsizes);
      }
    }
    else {
      /* go through the hash table for this index and record */
      /* the new terms and the postings information          */
      items_output = 0;
      
      /* Initialize the key/data pair so the flags aren't set. */
      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));

      for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search); 
	   entry != NULL; 
	   entry = Tcl_NextHashEntry(&hash_search)) {
	/* get the word */
	word_ptr = Tcl_GetHashKey(hash_tab,entry); 
	/* and the frequency info */
	ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
	
#ifdef DEBUGIND
	printf("word: %s\n", word_ptr);
#endif

	if (idx->type & EXTERNKEY) {
	  /* for external files accumulate the raw word counts for */
	  /* recording document size for retrieval calculations    */
	  total_docsize += ht_data->termfreq ;
	}

	/* if this is NOT a batch load, insert the terms directly */
	if (batch_flag == 0) {
	  if (idx->type & BITMAPPED_TYPE) {
	    in_add_bitmap_entry(idx, recnum, word_ptr, NULL, 1);
	  }
	  else {
	    /* directly add terms to the index */
	    keyval.data = (void *) word_ptr;
	    keyval.size = strlen(word_ptr) + 1;

	    returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
	  
	    if (returncode == 0) { /* found the word in the index already */
	      int temprecnum = 0;
	      int tempfreq = 0;
	      
	      /* allocate space for the posting info */
	      newinfo = (idx_result *) 
		CALLOC(int, 
		       ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
	      memcpy((char *)newinfo, (char *)dataval.data, dataval.size);

#ifdef DEBUGIND
	      printf("outputting hash table word: %s - termid: %d\n", 
		     word_ptr,newinfo->termid);
#endif
	      /* now increment the counters */
	      newinfo->tot_freq += ht_data->termfreq;
	      newinfo->num_entries++;
	      idx->GlobalData->tot_occur += ht_data->termfreq;
	      newinfo->entries[newinfo->num_entries-1].record_no = recnum;
	      newinfo->entries[newinfo->num_entries-1].term_freq 
		= ht_data->termfreq;
	      dataval.data = newinfo;
	      dataval.size += sizeof(idx_posting_entry);

	      /* and write it to the index */
	      returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
	      items_output++;
	    
	      if (returncode != 0) { 
#ifdef DEBUGIND
		printf("\nNon-zero return in db put of found term: %d\n", 
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
	      if (idx->type & PROXIMITY)
		PutPositionInfo(idx, recnum, newinfo->termid, ht_data);

	      FREE(newinfo);
	    }
	    else if (returncode == DB_NOTFOUND) { 
	      /* a new word that has no freq data */
	      tempinfo.termid = idx->GlobalData->recptr++;
	      tempinfo.tot_freq = ht_data->termfreq;
	      tempinfo.entries[0].record_no = recnum;
	      tempinfo.entries[0].term_freq = ht_data->termfreq;
	      tempinfo.num_entries = 1;
	      idx->GlobalData->tot_occur += ht_data->termfreq;
	      dataval.size = sizeof(tempinfo);
	      dataval.data = (void *)&tempinfo;
	    
#ifdef DEBUGIND
	      printf("output new hash table word: %s - termid: %d\n", 
		     word_ptr,tempinfo.termid);
#endif
	      /* put the new word into the index */
	      returncode = index_db->put(index_db, NULL, &keyval, &dataval, 
					 DB_NOOVERWRITE); 
	      items_output++;
	  
	      if (returncode != 0) { 
		if(returncode == 1)
		  fprintf(LOGFILE, "Duplicate term in db put : %s\n",word_ptr);
		else {
		  fprintf(LOGFILE, "Non-zero return in db put of new term: %d\n", 
			  returncode);
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
	      if (idx->type & PROXIMITY)
		PutPositionInfo(idx, recnum, tempinfo.termid, ht_data);

	      /* no output to postings for single record items */
	    }  
	  }
	}
	else { /* BATCH -- output the temp entries for this index */
	  /* first remove any 001 chars from the word, just in case... */
	  for (tempptr = strchr(word_ptr, (int)'\001');
	       tempptr != NULL; tempptr = strchr(word_ptr, (int)'\001')) {
	    *tempptr = '?';
	  }
	    
	  fprintf(bf->outfile, "%s\t%012d\t%d", word_ptr, recnum, 
		  ht_data->termfreq);

	  if (idx->type & PROXIMITY) {
	    /* output the positional info for each occurrence */
	    fprintf(bf->outfile,"\001");
	    for (posdata = ht_data->pos; posdata; posdata = posdata->next) {
	      fprintf(bf->outfile," %d", posdata->position);
	    }
	  }
#ifndef WIN32
	  fprintf(bf->outfile, "\n");
#else
	  /* to ensure size matching after sorting... */
	  fprintf(bf->outfile, "\r\n");
#endif

#ifdef DEBUGIND
	  printf("PUTTING %s\t%d\t%d\n", word_ptr, recnum, 
		  ht_data->termfreq);
#endif
	}
	
	/* all done with this term data hash entry */
	if (idx->type & PROXIMITY) {
	  prevpos = ht_data->pos;
	  for (posdata = ht_data->pos->next; posdata; posdata = posdata->next) {
	    FREE(prevpos);
	    prevpos = posdata;
	  }
	  FREE(ht_data->last);
	}
	FREE(ht_data);
      }
      
      /* clear out out the current hash table for this index */
      Tcl_DeleteHashTable(hash_tab);
      
      if (items_output != 0) {
	idx->GlobalData->tot_numrecs++;
	/* cf_sync_global_data(idx); don't sync here */
      }
    }
  }

  /* add information for components for this record */
  if (in_components (cf, filename, recnum, sgmlrec, batch_flag, temp_file_dir)
      == FAIL) {
    fprintf(LOGFILE, "in_kwadd: Component indexing failed for record %d\n",
	    recnum);
    fflush(LOGFILE);
  }
  /* add cluster information to cluster temp files */
  if (in_cluster (filename, recnum, sgmlrec) == FAIL) {
    fprintf(LOGFILE, "in_kwadd: cluster add failed\n");
    fflush(LOGFILE);
  }

  /* output the total docsize for external files */
  if (total_docsize > 0) {
    index_docsizes = (FILE *) cf_open(filename, DOCSIZEFILE);
    fseek(index_docsizes, (recnum * sizeof(int)), 0);
    fwrite(&total_docsize, sizeof(int), 1, index_docsizes);
  }

  /* free up the document, should be finished with it */
  if (sgmlrec)  { 
    free_doc(sgmlrec);
  }

  /*  We no longer sync on each record, but only at closing or on error  
   *   for (idx = cf_getidx(filename) ; idx ; idx = idx->next_entry) 
   *   cf_sync_global_data(idx);
   */
  return (SUCCEED);
}




/* PutPositionInfo outputs the supplied POSDATA info attached to */
/* ht_data to the PROX DB file for the index                     */
void 
PutPositionInfo(idx_list_entry *idx, int recnum, int termid, 
		     HT_DATA *ht_data)
{
  prox_idx_pos_key poskeyval;
  
  int *outdata;
  POSDATA *posdata;
  DBT keyval;
  DBT dataval;
  int returncode;
  int start;
  int numints;


  if (idx->prox_db == NULL) {
    /* this should never happen because the index should have been opened */
    fprintf(LOGFILE, "in_kwadd:PutPositionInfo: Prox file not open!\n");
    fflush(LOGFILE);
    return;
    
  }
  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));

  poskeyval.recnum = recnum;
  poskeyval.termid = termid;

  keyval.data = (void *) &poskeyval;
  keyval.size = sizeof(prox_idx_pos_key);;

  returncode = idx->prox_db->get(idx->prox_db, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the word in the index already */
    /* calculate the space for the new data + old data */
    numints = dataval.size / sizeof(int);
    outdata = CALLOC(int, ht_data->termfreq + numints);
    /* get the existing data (Move to aligned space) */
    memcpy(outdata, (int *)dataval.data, dataval.size);
    start = numints - 1; /* before the final null */
  }
  else {
    /* allocate space for the data */
    outdata = CALLOC(int, ht_data->termfreq+1); /* put a null at the end */
    start = 0;
  }

  /* append the new data */
  for (posdata = ht_data->pos; posdata; posdata = posdata->next) {
    outdata[start] = posdata->position;
    start++;
  }

  dataval.size = start * sizeof(int);
  dataval.data = (void *) outdata;
  returncode = idx->prox_db->put(idx->prox_db, NULL, &keyval, &dataval, 0); 
  
  if (returncode != 0) { 
#ifdef DEBUGIND
    printf("\nNon-zero return in prox_db put: %d\n", 
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
  FREE(outdata);
}


/* PutPositionInfo outputs the supplied POSDATA info attached to */
/* ht_data to the PROX DB file for the index                     */
void 
PutPositionInfoArray(idx_list_entry *idx, int recnum, int termid, 
		     int numprox, int *proxdata)
{
  prox_idx_pos_key poskeyval;
  
  int *outdata;
  DBT keyval;
  DBT dataval;
  int returncode;
  int start;
  int numints;
  int i;

  if (idx->prox_db == NULL) {
    /* this should never happen because the index should have been opened */
    fprintf(LOGFILE, "in_kwadd:PutPositionInfo: Prox file not open!\n");
    fflush(LOGFILE);
    return;
    
  }
  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));

  poskeyval.recnum = recnum;
  poskeyval.termid = termid;

  keyval.data = (void *) &poskeyval;
  keyval.size = sizeof(prox_idx_pos_key);;

  returncode = idx->prox_db->get(idx->prox_db, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the word in the index already */
    numints = dataval.size / sizeof(int);
    outdata = CALLOC(int, numprox + numints);
    /* get the existing data (Move to aligned space) */
    memcpy(outdata, (int *)dataval.data, dataval.size);
    start = numints - 1; /* before the final null */
  }
  else {
    /* allocate space for the data */
    outdata = CALLOC(int, numprox+1); /* put a null at the end */
    start = 0;
  }

  /* append the new data */
  for (i = 0; i < numprox ; i++) {
    outdata[start] = proxdata[i];
    start++;
  }

  dataval.size = start * sizeof(int);
  dataval.data = (void *) outdata;
  returncode = idx->prox_db->put(idx->prox_db, NULL, &keyval, &dataval, 0); 
  
  if (returncode != 0) { 
#ifdef DEBUGIND
    printf("\nNon-zero return in prox_db put: %d\n", 
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
  FREE (outdata);

}






