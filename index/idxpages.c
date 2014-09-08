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
*	Header Name:  idxpages.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing all keywords in a set of "pages", that is
*                     individual components of a document stored in a
*                     directory with names containing a number that
*                     indicates the sequence of the pages.
*
*	Usage:	      idxpages(SGML_Data *data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, DB *index_db, int index_type,
*                     int recnum, idx_list_entry *idx)
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
*
*	Return Conditions and Return Codes:	
*
*	Date:		03/01/97
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "ht_info.h"
#include "dmalloc.h"
#ifdef SOLARIS
#include <sys/dirent.h>
#endif
#ifndef WIN32
#include <dirent.h>
#endif
#ifdef WIN32
#include "dirent.h"
#include <stdlib.h>
#endif

extern int in_ft_lex();
extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);


extern batch_files_info *batch_files;

int page_file_delete(int recnum, idx_list_entry *idx);

FILE *in_ft_in; /* input file for full-text parsing using the 
                   in_fulltext.flex scanner (generated with -Pin_ft_) */

void PutPositionInfo(idx_list_entry *idx, int recnum, int termid, 
		     HT_DATA *ht_data);


int idxpages(SGML_Data *data, Tcl_HashTable *hash_tab,
	     int recnum, idx_list_entry *idx, int batch_flag)
{
  
  char *pathbuf, *word_ptr, *result_word; 
  int morphed = 0;
  int returncode, exists, buffsize, tokentype ;
  extern char *FULL_TEXT_TOKEN;
  extern int FULL_TEXT_POSITION; 
  char *wn_morph();
  Tcl_HashEntry *entry;
  idx_result *newinfo, tempinfo;
  DBT keyval;
  DBT dataval;
  DIR *dirptr;
  struct dirent *direntry;
  int items_output = 0;
  char *filenamebuff;
  Tcl_HashTable *stoplist_hash;
  Tcl_HashSearch hash_search;
  DB *index_db;
  int index_type;
  char *pagenumstr, pagenumbuff[20];
  int pagenum, start, numlen;
  int intern_pagenum;
  int total_words;
  int dirsize;
  int dirname_size;
  int delete_flag;
  batch_files_info *bf = NULL, *prev = NULL;

  FILE *page_rec_assoc_file;
  FILE *page_file_names;
  
  page_assoc_rec page_assoc_rec_data;
  HT_DATA *ht_data;
  POSDATA *posdata;


  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;


  /* The data should be a file path on the local machine */
  if (data == NULL) return (0);

  if (batch_flag == -1) {
    return (page_file_delete(recnum, idx));
      }
  else {
    delete_flag = 0;
  }

  buffsize = (int)(((long)data->content_end) - ((long)data->content_start));
  if (buffsize == 0) return (0);

  /* calloc should initialize the pathbuf to nulls */
  pathbuf = CALLOC(char, buffsize+2); 

  if (pathbuf == NULL) {
    fprintf(LOGFILE, "Unable to allocate path buffer in idxpages\n");
    exit(1);
  }
  strncpy(pathbuf, data->content_start, buffsize);

  /* We need to scan the directory and grab each file -- getting its  */
  /* page number from the name and generating the appropriate mapping */
  /* info in the associated special index files                       */
  if ((dirptr = opendir(pathbuf)) == NULL)  {
    fprintf(LOGFILE, "Unable to open directory '%s' in idxpages\n",pathbuf);
    return(0);
  }

  if ((page_rec_assoc_file = idx->page_rec_assoc_file) == NULL) {
    fprintf(LOGFILE, 
	    "NULL index file pointer for pagefile associator in idxpages\n");
    return(0);
  }
  if ((page_file_names = idx->page_file_names) == NULL) {
    fprintf(LOGFILE, "NULL index file pointer for page names in idxpages\n");
    return(0);
  }
  
  /* get each file in the directory... */
  while ((direntry = readdir(dirptr)) != NULL) {

#if (defined SOLARIS || defined LINUX || defined WIN32)
    dirsize = direntry->d_reclen + buffsize + 3;
    dirname_size = direntry->d_reclen - sizeof(struct dirent) + 2;
#else
    dirsize = buffsize+direntry->d_namlen+3;
    dirname_size = direntry->d_namlen + 1;
#endif


    /* skip dot and dotdot */
    if (strcmp(direntry->d_name,".") == 0 
	|| (strcmp(direntry->d_name, "..") == 0)) {
      continue;
    }

    start = strcspn(direntry->d_name, "0123456789");

    if (start == -1 || start == dirname_size) {
      continue; /* no page number for this file */
    }

    pagenumstr = direntry->d_name + start;
    numlen = strspn(pagenumstr, "0123456789");

    if (numlen == -1 || numlen == 0) {
      continue; /* no page number for this file */
    }
    
    strncpy(pagenumbuff, pagenumstr, numlen);
    pagenumbuff[numlen] = '\0';

    pagenum = atoi(pagenumbuff);

    filenamebuff = CALLOC(char, dirsize); 
    if (filenamebuff == NULL) {
      fprintf(LOGFILE, "Unable to allocate file name buffer in idxpages\n");
      return(0);
    }
#ifndef WIN32
    sprintf(filenamebuff, "%s/%s", pathbuf, direntry->d_name);
#else
    sprintf(filenamebuff, "%s\\%s", pathbuf, direntry->d_name);
#endif

    /* open the file */
    if ((in_ft_in = fopen(filenamebuff,"r")) == NULL) {
      fprintf(LOGFILE, "Unable to open file '%s' in idxpages\n",filenamebuff);
      FREE(filenamebuff);
      return(0);
    }
    
    /* increment the internal page number to identify the page */
    idx->highest_page_number++; 
    intern_pagenum = idx->highest_page_number; 
    total_words = 0;

    /* The lexical analyzer for full text returns a 1 for a text token */
    /* or a 2 for an sgml/html tag -- it returns 0 at EOF              */
    while (tokentype = in_ft_lex()) {
      
      if (tokentype == 1 || tokentype == 4) {
	/* tokentype 1 is a normal word or a "year"= 19nn.          */
	/* currently all numbers other than years are ignored       */
	/* normalize the word according to the specs for this index */
	/* (as defined in indextype)                                */
	word_ptr = FULL_TEXT_TOKEN;
	/* convert diacritics in normalization */
	result_word = normalize_key(word_ptr, idx, &morphed, 1);
	
	/* NULL means a stop word or other problem, so skip it */
	if (result_word != NULL) {
	  int result_len;
	  
	  if ((result_len = strlen(result_word)) > 0) {
	    /* We have a normalized word, is it in the hash table? */
	    entry = Tcl_FindHashEntry(hash_tab,result_word);
	    
	    if (entry == NULL){ /* nope, it is new word */

	      /* create the hash table structure */
	      ht_data = CALLOC(struct hash_term_data, 1) ;
	      ht_data->termid = -1;
	      ht_data->termfreq = 1;

	      if (index_type & PROXIMITY) {
		/* add the offset info to the structure */
		posdata = CALLOC(struct posdata, 1);
		posdata->position = FULL_TEXT_POSITION;
		ht_data->pos = posdata;
		ht_data->last = posdata;
	      }
	      
	      /* put it into the hash table */
	      Tcl_SetHashValue(
			       Tcl_CreateHashEntry(hash_tab,
						   result_word, &exists), 
			       (ClientData)ht_data);
	      FREE(result_word);
	      FREE(word_ptr);
	      
	    }
	    else { /* word already in hash table, so just increment it */
	      ht_data =  
		(struct hash_term_data *) Tcl_GetHashValue(entry);
	      ht_data->termfreq++;

	      if (index_type & PROXIMITY) {
		/* add the offset info to the structure */
		posdata = CALLOC(struct posdata, 1);
		posdata->position = FULL_TEXT_POSITION;
		ht_data->last->next = posdata;
		ht_data->last = posdata;
	      }
	      
	      FREE(result_word);
	      FREE(word_ptr);
	    }
	  }
	  else {/* log the original word the stemmer stomped */
#ifdef DEBUG
	    printf("Null stemmer output from %s\n", word_ptr);
	    /* no longer logging these */
	    fprintf(LOGFILE, "Null stemmer output from %s\n", word_ptr);
#endif
	  }
	}
	else { /* stop word -- so free the word ptr */
	  FREE(word_ptr);
	}
      }
      else if (tokentype == 2) { /* an sgml tag */
#ifdef DEBUG
	fprintf(LOGFILE, "TAG VALUE: %s\n", FULL_TEXT_TOKEN);
#endif
	FREE(FULL_TEXT_TOKEN);
      }
    } 
    /* we have now extracted all the words from the file */
    /* so we can close it and output the appropriate data*/
    fclose (in_ft_in);

    /* go through the hash table for this index and record */
    /* the new terms and the postings information          */
    items_output = 0;

    /* Initialize the key/data pair so the flags aren't set. */
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    
    if (batch_flag) { /* this is a batch run, set up the file for output */

      for (bf = batch_files; bf != NULL; bf = bf->next) {
	if (idx == bf->idx)
	  break;
	prev = bf;
      }
      if (bf == NULL || batch_files == NULL) {
	  fprintf(LOGFILE, "Couldn't open batch file in idxpages\n");
	  return (0);
      }
    }    

    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search); 
	 entry != NULL; 
	 entry = Tcl_NextHashEntry(&hash_search)) {

      /* get the word */
      word_ptr = Tcl_GetHashKey(hash_tab,entry); 
      /* and the frequency info */
      ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
      
      total_words += ht_data->termfreq;

      if (batch_flag) { /* if this is a batch run put out the data */
	fprintf(bf->outfile, "%s\t%012d\t%d\n", word_ptr, intern_pagenum, 
		ht_data->termfreq);

      }
      else { /* not a batch run */
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
	  
	  /* now increment the counters */
	  newinfo->tot_freq += ht_data->termfreq;
	  newinfo->num_entries++;
	  idx->GlobalData->tot_occur += ht_data->termfreq;      
	  newinfo->entries[newinfo->num_entries-1].record_no = intern_pagenum;
	  newinfo->entries[newinfo->num_entries-1].term_freq 
	    = ht_data->termfreq;

	  dataval.data = newinfo;
	  dataval.size += sizeof(idx_posting_entry);

	  /* and write it to the index */
	  returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
	  if (returncode != 0) { 
#ifdef DEBUG
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
	  items_output++;

	  PutPositionInfo(idx, intern_pagenum, newinfo->termid, ht_data);

	  FREE(newinfo);
	}
	else if (returncode == DB_NOTFOUND) { 
	  /* a new word that has no freq data */
	  keyval.data = (void *) word_ptr;
	  keyval.size = strlen(word_ptr) + 1;
		
	  tempinfo.termid = idx->GlobalData->recptr++;
	  tempinfo.tot_freq = ht_data->termfreq;
	  tempinfo.entries[0].record_no = intern_pagenum;
	  tempinfo.entries[0].term_freq = ht_data->termfreq;
	  tempinfo.num_entries = 1;
	  idx->GlobalData->tot_occur += ht_data->termfreq;
	  dataval.size = sizeof(tempinfo);
	  dataval.data = (void *)&tempinfo;
	
	  /* put the new word into the index */
	  returncode = index_db->put(index_db, NULL, &keyval, &dataval, 
				     DB_NOOVERWRITE); 
	  items_output++;
	  
	  if (returncode != 0) { 
	    if(returncode == 1)
	      fprintf(LOGFILE, "Duplicate term in db put : %s\n",word_ptr);
	    else
	      fprintf(LOGFILE, "Non-zero return in db put of new term: %d\n", 
		      returncode);
	  }
	  PutPositionInfo(idx, intern_pagenum, tempinfo.termid, ht_data);

	/* no output to postings for single record items */
	} 
      }
      /* all done with this term data hash entry */
      /* clear out out the current hash table entry for this item */
      FREE(ht_data);
      Tcl_DeleteHashEntry(entry);  
    }
    
    if (items_output != 0) {
      idx->GlobalData->tot_numrecs++;
    }

    /* put out the linking information */
    fseek(page_file_names, 0, 2); /* goto eof */
    page_assoc_rec_data.nameoffset = ftell(page_file_names);
    fwrite(direntry->d_name, strlen(direntry->d_name)+1, 1, page_file_names);
    
    page_assoc_rec_data.recsize = total_words;
    page_assoc_rec_data.parentid = recnum;
    page_assoc_rec_data.pagenum = pagenum; /* "external" page number */

    fseek(page_rec_assoc_file, (intern_pagenum * sizeof(page_assoc_rec)), 0);
    fwrite(&page_assoc_rec_data, sizeof(page_assoc_rec), 
	   1, page_rec_assoc_file);
	  
    /* free the filename... */
    FREE(filenamebuff);

  }

  /* all finished with this directory, so close things up   */
  /* it is somewhat dangerous not to sync things more often */
  /* but it should keep things a bit faster                 */
  closedir(dirptr);
  free(pathbuf);
  return(0);
}







