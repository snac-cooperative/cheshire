/*
 *  Copyright (c) 1990-2013 [see Other Notes, below]. The Regents of the
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
*	Header Name:	in_batch_load.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routine to load temporary index records
*                       extracted in in_kwadd.c (q.v.) into a true index
*                       files. 
*
*	Usage:		in_batch_load(cf, tempfiledir)
*                       
*                       
*	Variables:	
*                       
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or 0 (FAIL) on error;
*
*	Date:		10/31/93
*	Revised:	11/7/93 (bug fixes)
*                       07/19/93 (Major Revision to Handle SGML records)
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"
#include <sys/stat.h>
#include "bitmaps.h"

SGML_DTD *cf_getDTD(char *filename);

extern int errno;


void PutPositionInfoArray(idx_list_entry *idx, int recnum, int termid, 
		     int numprox, int *proxdata);

int *GetPositionInfoArray(idx_list_entry *idx, int recnum, int termid);

/* the following structures are used in accumulating the data */
/* for each temp file and index                               */


extern batch_files_info *batch_files;

extern int in_kwdel (char *filename, int recnum, int dedup_flag);
extern int fil_del (int recno, char *filename);




#define PROX_BUFFER_SIZE 1500000
#define TEMP_BUFFER_SIZE 50000
#define KEEP_FILES 0

int in_batch_load_main (config_file_info *cf, batch_files_info *bf, 
			Tcl_HashTable *hash_tab, char *temp_file_dir,
			char *sort_args)
{

  int count = 0;
  int line_num;
  char *tmp_buffer;
  char *in_buffer;
  char temp_file_name[2056];
  char sort_file_name[2056];
  char *last_key;
  char *word_ptr;
  int  termfreq;
  Tcl_HashEntry *entry;

  FILE *sort_temp_file;

  struct stat filestatus, filestatus2;
  int statresult, statresult2;
  time_t source_mod_time;
  idx_result *newinfo = NULL, *tmpinfo;
  DB *index_db;
  DBT keyval;
  DBT dataval;
  int returncode;
  int old_record;
  int recnum;
  int last_bitblock = 0;
  int last_recnum = 0;
  int entry_count = 0;
  bitmap_t *current_bitblock = NULL;
  int exists;
  char *file_name;
  char *fmode;
  int *newprox;
  int numprox;
  char *proxptr;
  char *local_sort_args;
  char *templocale;
  char *clocale;

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  tmp_buffer = CALLOC(char, TEMP_BUFFER_SIZE);
  in_buffer = CALLOC(char, PROX_BUFFER_SIZE);

  if (in_buffer == NULL || tmp_buffer == NULL) {
    fprintf(LOGFILE,
	    "Unable to allocate buffers in in_batch_load_main \n");
    cheshire_exit(1);
  }
  
  if (bf->outfile)
    fclose(bf->outfile);
    
  if (temp_file_dir) {
    file_name = strrchr(bf->index_name, '/');
    file_name++;
    sprintf(temp_file_name,"%s/%s_BATCHTEMP", temp_file_dir, file_name);
  }
  else
    sprintf(temp_file_name,"%s_BATCHTEMP", bf->index_name);


  if (stat(temp_file_name, &filestatus) != 0) {
    if (bf->load_flag == 0) {
      fprintf(LOGFILE,
	      "stat for BATCHTEMP data file %s failed in in_batch_Load_main \n",
	      temp_file_name);
      exit(1);
    }
  }
  source_mod_time = filestatus.st_mtime;
    
  if (temp_file_dir) {
    file_name = strrchr(bf->index_name, '/');
    file_name++;
    sprintf(sort_file_name,"%s/%s_BATCHSORT", temp_file_dir, file_name);
  }
  else
    sprintf(sort_file_name,"%s_BATCHSORT", bf->index_name);
  
  statresult = stat(sort_file_name, &filestatus);
  
  if ((statresult == 0 && source_mod_time > filestatus.st_mtime)
      || (statresult == -1 && errno == ENOENT)) { 
    /* either there is no such file, or the tmp file is newer than */
    /* an existing sort file, so we will create it by a batch sort */
    if (sort_args == NULL)
      local_sort_args = "";
    else
      local_sort_args = sort_args;
    
    if (temp_file_dir) {

#ifndef WIN32
      sprintf(tmp_buffer,"export LC_ALL=C ; sort -T %s %s %s > %s",
	      temp_file_dir, local_sort_args, temp_file_name, sort_file_name);
#else
      sprintf(tmp_buffer,"sort %s < \"%s\" > \"%s\"",
	      local_sort_args, temp_file_name, sort_file_name);
#endif
    }
    else
#ifndef WIN32 
      sprintf(tmp_buffer,"export LC_ALL=C ; sort %s %s > %s",
	      local_sort_args, temp_file_name, sort_file_name);
#else
      sprintf(tmp_buffer,"sort %s < \"%s\" > \"%s\"",
	      local_sort_args, temp_file_name, sort_file_name);

#endif
    fprintf(LOGFILE,"SORT COMMAND: %s\n", tmp_buffer);
    fflush(LOGFILE);

    /* submit the sort */
    system(tmp_buffer);

  }
  /* otherwise use the existing .sort file */

  /* check to see if the sorted file and source file are the same size...*/
  statresult = stat(sort_file_name, &filestatus);
  statresult2 = stat(temp_file_name, &filestatus2);
  if (statresult == 0 && statresult2 == 0 
      && filestatus.st_size == filestatus2.st_size) {
    /* files are the same size -- OK to delete the temp file and proceed */
    if (KEEP_FILES == 0)
      unlink(temp_file_name);
  }
  else {
#ifdef WIN32
    if (statresult == 0 && statresult2 == 0 
	&& filestatus.st_size > filestatus2.st_size) {
      /* files are ALMOST the same size - NT sort sometimes adds characters */
      /* like carraige returns at the end of file */
      /* OK to delete the temp file and proceed */
      if (KEEP_FILES == 0)
	unlink(temp_file_name);
    } else if (bf->load_flag != 2) {
      fprintf(LOGFILE,"ERROR:Sorted file size and temp size do not match\n");
      fflush(LOGFILE);
      exit(1);
    }
#else
    if (bf->load_flag != 2) {
      fprintf(LOGFILE,"ERROR:Sorted file size and temp size do not match\n");
      fflush(LOGFILE);
      exit(1);
    }
#endif
  }

      
  /* now we read in each record and accumulate those with the same */
  /* index key.                                                    */
#ifdef WIN32
  fmode = "rb";
#else 
  fmode = "r";
#endif

  sort_temp_file = fopen(sort_file_name, fmode);
  
  if (sort_temp_file == NULL) {
    fprintf(LOGFILE,"sort file is null in in_batch_load \n");
    fflush(LOGFILE);
    exit(1);
  }
  
  line_num = 0;
  
  if (bf->idx->db == NULL) 
    index_db = (DB *) cf_index_open(cf->file_name, bf->idx->tag, INDEXFL);
  else
    index_db = bf->idx->db;
  
  last_key = NULL;
  
  word_ptr = tmp_buffer;
  numprox = 0;
  
  setvbuf (sort_temp_file, NULL, _IOFBF, 16 *1024 * 1024);
 
  while (fgets(in_buffer,PROX_BUFFER_SIZE,sort_temp_file)) {

    if (strlen(in_buffer) > TEMP_BUFFER_SIZE) {
      fprintf(LOGFILE,
	      "Extracted term input line exceeds buffer size (%d) in in_batch_load: \n %s \n\n",
	      TEMP_BUFFER_SIZE, in_buffer);
      fflush(LOGFILE);
      continue;
    }

    sscanf(in_buffer,"%[^	]\t%d\t%d", 
		tmp_buffer, &recnum, &termfreq);
    
    line_num++;
  
    if (line_num % 10000 == 0) {
      fprintf(LOGFILE,
	      "%d temp index lines processed in in_batch_load \n",
	      line_num);
      fflush(LOGFILE);
    }
    
    if (bf->idx->type & BITMAPPED_TYPE) {
      /* Bitmapped indexes need special handling... */
      if (last_key != NULL && (strcmp(last_key, word_ptr) == 0)) {
	
	  if (current_bitblock == NULL)
	    current_bitblock = bitmap_block_alloc();

	  if (bitblock_id(recnum) == last_bitblock) {
	    /* adding to the existing block */
	    bitblock_set(current_bitblock, recnum);
	    last_recnum = recnum;
	    entry_count++;
	  }
	  else {
	    /* new block needed -- */
	    /* have to write the current block */
	    in_add_bitmap_entry(bf->idx, last_recnum, word_ptr,
				current_bitblock, entry_count);
	    FREE(current_bitblock);
	    current_bitblock = bitmap_block_alloc();
	    bitblock_set(current_bitblock, recnum);
	    last_bitblock = bitblock_id(recnum);
	    last_recnum = recnum;
	    entry_count = 1;
	  }
      }
      else {

	if (current_bitblock == NULL) {
	  /* first pass */
	  entry_count = 1;
	  current_bitblock = bitmap_block_alloc();
	  bitblock_set(current_bitblock, recnum);
	  last_bitblock = bitblock_id(recnum);
	  last_recnum = recnum;
	  if (last_key) FREE(last_key);
	  last_key = strdup(word_ptr);
	} 
	else { 
	  /* new term -- write the current block and make a new one */
	  in_add_bitmap_entry(bf->idx, last_recnum, last_key,
			      current_bitblock, entry_count);
	  FREE(current_bitblock);
	  current_bitblock = bitmap_block_alloc();
	  if (last_key) FREE(last_key);
	  last_key = strdup(word_ptr);
	  bitblock_set(current_bitblock, recnum);
	  last_bitblock = bitblock_id(recnum);
	  last_recnum = recnum;
	  entry_count = 1;
	}

      }
    }
    else {
      /* all other index types */
      if (hash_tab) {
	entry = Tcl_FindHashEntry(hash_tab, (void *)recnum);
	if (entry != NULL) /* it is from a duplicate record */
	  continue;
      }
    
      /* check the index file entry */
      if (last_key != NULL && (strcmp(last_key, word_ptr) == 0)) {
	/* term is the same as the previous entry, so just add the stats */

	if (cf->primary_key == bf->idx) {
	  /* PRIMARY KEY PROCESSING -- SHOULDN't OCCUR OFTEN*/
	  if (cf->primary_key->type & PRIMARYREPLACE) { 
	    /* replace the old index items with the new ones */
	    fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	    /* replace the existing info */
	    old_record = newinfo->entries[0].record_no;
	    
	    newinfo->entries[0].record_no = recnum;
	    newinfo->entries[0].term_freq = termfreq;
	    
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(hash_tab,
						 (void *)old_record, &exists), 
			     (ClientData)1);
	    fprintf(LOGFILE, "Record number %d will REPLACE %d\n", 
		    recnum, old_record);
	    
	    in_kwdel(cf->nickname, old_record, 0);
	    fil_del(old_record, cf->nickname);

	    /* now just continue and index the new record */
	  } 
	  else {
	    /* otherwise we ignore the new record, noting it in the logs */
	    fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	    fprintf(LOGFILE, "Record number %d IGNORED\n", recnum);
	    /* put it into the hash table */
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(hash_tab,
						 (void *)recnum, &exists), 
			     (ClientData)1);
	    


	  }
	}
	else { /* not a primary key */
	  tmpinfo = (idx_result *) 
	    REALLOC(newinfo, int,
		    ((dataval.size + sizeof(idx_posting_entry))/sizeof(int)));
	  newinfo = tmpinfo;
	  newinfo->tot_freq += termfreq;
	  newinfo->num_entries++;
	  bf->idx->GlobalData->tot_occur += termfreq;
	  newinfo->entries[newinfo->num_entries-1].record_no = recnum;
	  newinfo->entries[newinfo->num_entries-1].term_freq 
	    = termfreq;      
	  dataval.data = newinfo;
	  dataval.size += sizeof(idx_posting_entry);
	  
	  /* if this is a PROXIMITY index extract the other data */
	  if (bf->idx->type & PROXIMITY) {
	    int pos, i;
	    
	    proxptr = strchr(in_buffer,'\001');
	    proxptr++;
	    
	    newprox = (int *) 
	      CALLOC(int, termfreq+1);
	    for (i = 0; i < termfreq; i++) {
	      if (proxptr) {
		sscanf(proxptr," %d", &pos);
		newprox[i] = pos;
		proxptr = strchr(proxptr+2, ' ');
	      }
	      else
		newprox[i] = 0;
	    }
	    /* output the data -- should be unique for termid recnum pairs */
	    PutPositionInfoArray(bf->idx, recnum, newinfo->termid, 
				 termfreq, newprox);
	    FREE(newprox);
	  } 
	}
      }
      else {/* no existing data or different word -- so search the index*/
      
	if (newinfo != NULL) { /* previous data to write */
	  if (last_key == NULL)
	    last_key = strdup(word_ptr);
	  
	  keyval.data = (void *) last_key;
	  keyval.size = strlen(last_key) + 1;
	  returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
	  if (returncode != 0) {
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
	  FREE(newinfo);
	  newinfo = NULL;
	  /* output prox info */

	}
      
	if (last_key) FREE(last_key);
	last_key = strdup(word_ptr);
     
	keyval.data = (void *) word_ptr;
	keyval.size = strlen(word_ptr) + 1;
      
	returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
      
	if (cf->primary_key == bf->idx) {
	  /* PRIMARY KEY PROCESSING */
	  if (returncode == 0) { /* found the word in the index already */
	    if (cf->primary_key->type & PRIMARYREPLACE) { 
	      /* replace the old index items with the new ones */
	      fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	      /* allocate space for the posting info */
	      newinfo = (idx_result *) 
		CALLOC(int, (dataval.size / sizeof(int)));
	      
	      memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
	      
	      /* printf("primary key: dataval.size is %d, newinfo->num_entries is %d\n",
	       *   dataval.size, newinfo->num_entries);
	       */
	      
	      if (newinfo->num_entries == 1) { /* it better be for primary key! */
		old_record = newinfo->entries[0].record_no;
		fprintf(LOGFILE, "Record number %d will REPLACE %d\n", 
			recnum, old_record);
		/* first "delete" all of the index info for the old record */
		in_kwdel(cf->file_name, old_record, 0);
		fil_del(old_record, cf->file_name);
		
	      }
	      /* now just continue and index the new record */
	    } 
	    else {
	      /* otherwise we ignore the new record, noting it in the logs */
	      fprintf(LOGFILE, "Duplicate primary key: %s\n",word_ptr);
	      fprintf(LOGFILE, "Record number %d IGNORED\n", recnum);
	    }
	  }
	  
	  if (returncode == DB_NOTFOUND || returncode == 0) { 
	    /* new primary key, so allocate it or replace */
	    newinfo = (idx_result *) 
	      CALLOC(int, (sizeof(idx_result) / sizeof(int)));
	    newinfo->termid = bf->idx->GlobalData->recptr++;
	    newinfo->tot_freq = termfreq;
	    newinfo->entries[0].record_no = recnum;
	    newinfo->entries[0].term_freq = termfreq;
	    newinfo->num_entries = 1;
	    bf->idx->GlobalData->tot_occur += termfreq;
	    dataval.size = sizeof(idx_result);
	    dataval.data = (void *)newinfo;
	  } 
	  else if (returncode == DB_RUNRECOVERY) {
	    fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
	    exit(999);
	  } 
	  else if (returncode == DB_LOCK_NOTGRANTED) {
	    fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
	    exit(999);
	  } 
	  else if (returncode == DB_LOCK_DEADLOCK) {
	    fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
	    exit(999);
	  } 
	  
	  
	} /* end of primary key processing */
	else { /* this is not a primary key */
	  
	  if (returncode == 0) { /* found the word in the index already */
	  
	    /* allocate space for the posting info (only one here)*/
	    newinfo = (idx_result *) 
	      CALLOC(int, 
		     ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
	    memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
	    
	    /* now increment the counters */
	    newinfo->tot_freq += termfreq;
	    newinfo->num_entries++;
	    bf->idx->GlobalData->tot_occur += termfreq;
	    newinfo->entries[newinfo->num_entries-1].record_no = recnum;
	    newinfo->entries[newinfo->num_entries-1].term_freq 
	      = termfreq;
	    dataval.data = newinfo;
	    dataval.size += sizeof(idx_posting_entry);
	    
	  }
	  else if (returncode == DB_NOTFOUND) { 
	    /* a new word that has no freq data */
	    newinfo = (idx_result *) 
	      CALLOC(int, (sizeof(idx_result) / sizeof(int)));
	    newinfo->termid = bf->idx->GlobalData->recptr++;
	    newinfo->tot_freq = termfreq;
	    newinfo->entries[0].record_no = recnum;
	    newinfo->entries[0].term_freq = termfreq;
	    newinfo->num_entries = 1;
	    bf->idx->GlobalData->tot_occur += termfreq;
	    dataval.size = sizeof(idx_result);
	    dataval.data = (void *)newinfo;
	  }
	  /* if this is a PROXIMITY index extract the other data */
	  if (bf->idx->type & PROXIMITY) {
	    int pos, i;
	    
	    proxptr = strchr(in_buffer,'\001');
	    proxptr++;
	    
	    newprox = (int *) 
	      CALLOC(int, termfreq+1);
	    for (i = 0; i < termfreq; i++) {
	      if (proxptr) {
		sscanf(proxptr," %d", &pos);
		newprox[i] = pos;
		proxptr = strchr(proxptr+2, ' ');
	      }
	      else
		newprox[i] = 0;
	    }
	    /* output the data -- should be unique for termid recnum pairs */
	    PutPositionInfoArray(bf->idx, recnum, newinfo->termid, 
				 termfreq, newprox);
	    FREE(newprox);
	  } 
	
	  
	} /* non primary key processing */
      } /* end new data element processing */
    }
  }

  /* put out the last record */

  if (bf->idx->type & BITMAPPED_TYPE) {
    /* have to write the current block */
    if (current_bitblock != NULL) {
      in_add_bitmap_entry(bf->idx, last_recnum, word_ptr,
			  current_bitblock, entry_count);
      FREE(current_bitblock);
    }
  }
  else {
    if (newinfo != NULL) { /* previous data to write */
      if (last_key == NULL)
	last_key = strdup(word_ptr);
      keyval.data = (void *) last_key;
      keyval.size = strlen(last_key) + 1;
      returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
      if (returncode != 0) {
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
      FREE(newinfo);
      newinfo = NULL;
      FREE(last_key);
      last_key = NULL;
    }
  }

  /* remove the sort file (since the real data in now in the index) */
  fclose(sort_temp_file);
  if (KEEP_FILES == 0)
    unlink(sort_file_name);
  FREE(in_buffer);
  FREE(tmp_buffer);
  return (SUCCEED);
}

int 
in_batch_load (config_file_info *cf, char *temp_file_dir, char *sort_args)
{

  batch_files_info *bf;
  Tcl_HashTable *hash_tab;
  idx_list_entry *idx;
  component_list_entry *comp;

  /* flush all output to the temp files (just in case) */
#ifdef WIN32
  flushall();
#else
  fflush(NULL);
#endif

  if (cf->primary_key) {
    /* set up a hashtable for superceeded primary keys */
    hash_tab = CALLOC(Tcl_HashTable, 1);

    Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
    /* process the primary key file and put dups into hash table */
    for (bf = batch_files; bf != NULL; bf = bf->next) {
      if (cf->primary_key == bf->idx) 
	in_batch_load_main (cf, bf, hash_tab, temp_file_dir, sort_args);
    }

  }
  else
    hash_tab = NULL;

  /* the first step is to run a sort on the temp index file */
  /* unless the sort file already exists.                   */
  for (bf = batch_files; bf != NULL; bf = bf->next) {
    /* we should have already done the primary key */
    if (cf->primary_key == bf->idx) 
      continue;

    for (idx = cf->indexes; idx; idx = idx->next_entry) {
      /* only process if there is a match for the bf index */
      /* since multiple cf entries may be processed in one */
      /* run, not all will be the RIGHT cf                 */
      if (idx == bf->idx)
	in_batch_load_main (cf, bf, hash_tab, temp_file_dir, sort_args);
    }

    for (comp = cf->components; comp; comp = comp->next_entry) {
      for (idx = comp->indexes; idx; idx = idx->next_entry) {
	/* only process if there is a match for the bf index */
	/* since multiple cf entries may be processed in one */
	/* run, not all will be the RIGHT cf                 */
	if (idx == bf->idx)
	  in_batch_load_main (cf, bf, hash_tab, temp_file_dir, sort_args);
      }
    }
  } /* end of loop to handle each batch file */

  /* clear out out the current hash table */
  if (hash_tab) 
    Tcl_DeleteHashTable(hash_tab);

  return (SUCCEED);
}


