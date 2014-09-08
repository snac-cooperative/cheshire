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
 *  Created by Ray R. Larson
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
*	Header Name:	convert2bitmap.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for converting a cheshire index inverted file
*                       to a bitmap form.
*	Usage:		convert2bitmap configfile mainfilename indexfilename bitmapindex_path
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		4/4/03
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993-2003.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#define MAIN
#include "cheshire.h"
#include "bitmaps.h"
#include "configfiles.h"

/* global config file structure */
config_file_info *cf_info_base;
int last_rec_proc = 0;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

extern int create_indexes(idx_list_entry *idx, int *num_open, idx_list_entry *head) ;
extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);

 
main (int argc, char *argv[])
{
  FILE *test;
  DB *dbin;
  DBT inkey, indata;
  DBC *dbcursor;
  DB *dbout;
  DBT outkey, outdata;
  idx_list_entry *in_idx, *out_idx;
  dict_info tempdict;
  idx_result *oldinfo, tempinfo;
  char *index_key;
  bitmap_t *work_block=NULL;
  int bitblock_count = 0;
  bitblock_key block_key;
  idx_list_entry *idx2;
  int num_open;
  char *last_key;
  int last_recnum=0, last_bitblock=0;
  int entry_count = 0;
  int i;
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "w+b";
  opflags = "r+b";
#else
  crflags = "w+";
  opflags = "r+";
#endif

  LOGFILE = stderr;


  if (argc < 5) {
    printf ("usage: %s configfilename in_mainfile_name in_index_name out_index_pathname\n", argv[0]);
    exit (0);
  }

  out_idx = CALLOC(idx_list_entry, 1);
  out_idx->name = strdup(argv[4]);
  out_idx->tag = "TMPCONVERSIONTAG";
  out_idx->type = BITMAPPED_TYPE;

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  /* attach out_idx to the current configfile */
  for (idx2 = cf_info_base->indexes; idx2 ; idx2 = idx2->next_entry) {
    if (idx2->next_entry == NULL) {
      idx2->next_entry = out_idx;
      idx2 = out_idx;
    }
  }
  
  create_indexes(out_idx, &num_open, cf_info_base->indexes); 

  /* open the input index */
  in_idx = cf_getidx_entry(argv[2], argv[3]); 

  dbin = (DB *) cf_index_open(argv[2], argv[3] , INDEXFL);
  if (dbin == NULL) {
    printf("Error in opening index input file %s\n", argv[3]);
    exit(1);
  }


  dbout = (DB *) cf_index_open(argv[2], argv[4] , INDEXFL);
  if (dbout == NULL) {
    printf("Error in opening new index file %s\n", argv[4]);
    exit(1);
  }

  /* Acquire a cursor for the database. */
  if ((errno = dbin->cursor(dbin, NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "dumpdb: cursor: %s\n", strerror(errno));
    exit (1);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&inkey, 0, sizeof(inkey));
  memset(&indata, 0, sizeof(indata));


  /* Walk through the database and print out the key/data pairs. */
  while ((errno = dbcursor->c_get(dbcursor, &inkey, &indata, DB_NEXT)) == 0) {

    if (indata.size < sizeof(idx_result)) {
      memcpy((char *)&tempdict,indata.data,indata.size);
    }
    else {
      printf("creating bitmap entry for term: %s ", (char *)inkey.data);


      /* allocate space for the posting info */
      oldinfo = (idx_result *) 
	CALLOC(int, (indata.size + sizeof(idx_posting_entry))/sizeof(int));
      memcpy((char *)oldinfo, (char *)indata.data, indata.size);
      
      printf("termid %d, tot_freq %d, num_entries %d\n",
	     oldinfo->termid, oldinfo->tot_freq, 
	     oldinfo->num_entries);
      
      /* new term -- write the current block and make a new one */
      if (work_block) {
	in_add_bitmap_entry(out_idx, last_recnum, last_key,
			    work_block, entry_count);
	FREE(work_block);
	work_block = NULL;
	FREE(last_key);
	last_key = strdup(inkey.data);
	last_recnum = 0;
	entry_count = 0;
	last_bitblock = bitblock_id(oldinfo->entries[0].record_no);
      }
      else 
	last_key = strdup(inkey.data);

      for (i = 0; i < oldinfo->num_entries; i++) {
	
	if (work_block == NULL) {
	  work_block = bitmap_block_alloc();
	  last_recnum = 0;
	  entry_count = 0;
	}
	
	if (bitblock_id(oldinfo->entries[i].record_no) == last_bitblock) {
	  /* adding to the existing block */
	  bitblock_set(work_block, oldinfo->entries[i].record_no);
	  last_recnum = oldinfo->entries[i].record_no;
	  entry_count++;
	}
	else {
	  /* new block needed -- */
	  /* have to write the current block */
	  in_add_bitmap_entry(out_idx, last_recnum, last_key,
			      work_block, entry_count);
	  FREE(work_block);
	  work_block = bitmap_block_alloc();
	  bitblock_set(work_block, oldinfo->entries[i].record_no);
	  last_bitblock = bitblock_id(oldinfo->entries[i].record_no);
	  last_recnum = oldinfo->entries[i].record_no;
	  entry_count = 1;
	  
	}
      }
    }
  }

  if (work_block) {
    in_add_bitmap_entry(out_idx, last_recnum, last_key,
			work_block, entry_count);
    FREE(work_block);
    work_block = NULL;
    FREE(last_key);
    last_key = strdup(inkey.data);
  }
  
  (void)dbcursor->c_close(dbcursor);

  cf_closeall();
  
}








