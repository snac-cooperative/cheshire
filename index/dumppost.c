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
*	Header Name:	dumppost.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining an index inverted file
*
*	Usage:		dumppost configfile mainfilename indexfilename
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		Aug 08, 1997
*	Revised:	
*	Version:	2.0
*	Copyright (c) 1993-7.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#define MAIN
#include "cheshire.h"
#include "configfiles.h"
#include "bitmaps.h"

/* global config file structure */
config_file_info *cf_info_base;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

int last_rec_proc = 0;
 
main (int argc, char *argv[])
{
  FILE *test;
  DB *dbtest;
  DBT key, data;
  DBC *dbcursor;
  idx_list_entry *idx;
  int bitblock_count = 0;
  bitblock_key block_key;

  dict_info tempdict;
  idx_result *newinfo, tempinfo;

  int i, startrec, numrecs;
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif

  LOGFILE = stderr;

  if (argc < 4) {
    printf ("usage: %s configfilename mainfile_name index_name\n", argv[0]);
    exit (0);
  }

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  idx = cf_getidx_entry(argv[2], argv[3]); 

  dbtest = (DB *) cf_index_open(argv[2], argv[3] , INDEXFL);

  if (dbtest == NULL)
    exit(1);

  /* Acquire a cursor for the database. */
  if ((errno = dbtest->cursor(dbtest, NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "dumpdb: cursor: %s\n", strerror(errno));
    exit (1);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));

  /* Walk through the database and print out the key/data pairs. */
  while ((errno = dbcursor->c_get(dbcursor, &key, &data, DB_NEXT)) == 0) {

    if (idx->type & BITMAPPED_TYPE && data.size == bitmap_blocksize_bytes) {
      bitblock_count++;
      memcpy((char *)&block_key,key.data,key.size);
      printf("Bitblock key: Block %d, Termid %d\n", block_key.blockid, block_key.termid);
      continue;
    }
    else if (idx->type & BITMAPPED_TYPE) {
      if (data.size < sizeof(idx_result)) {
	memcpy((char *)&tempdict,data.data,data.size);
      }
      else {
	printf("key: %s ", (char *)key.data);
	
	/* allocate space for the posting info */
	newinfo = (idx_result *) 
	  CALLOC(int, (data.size + sizeof(idx_posting_entry))/sizeof(int));
	memcpy((char *)newinfo, (char *)data.data, data.size);
	
	printf("termid %d, tot_freq %d, num_entries %d\n",
	       newinfo->termid, newinfo->tot_freq, 
	       newinfo->num_entries);

	printf("\tPOSTINGS DATA FOR BITMAPS NOT YET AVAILABLE\n");
      }
    }
    else {
      
      if (data.size < sizeof(idx_result)) {
	memcpy((char *)&tempdict,data.data,data.size);
      }
      else {
	printf("key: %s ", (char *)key.data);
	
	/* allocate space for the posting info */
	newinfo = (idx_result *) 
	  CALLOC(int, (data.size + sizeof(idx_posting_entry))/sizeof(int));
	memcpy((char *)newinfo, (char *)data.data, data.size);
	
	printf("termid %d, tot_freq %d, num_entries %d\n",
	       newinfo->termid, newinfo->tot_freq, 
	       newinfo->num_entries);
	
	for (i = 0; i < newinfo->num_entries; i++) {
	  printf("\tRecord id %d : tot_freq %d\n",
		 newinfo->entries[i].record_no, newinfo->entries[i].term_freq);
	}
      }
    }
  }

  printf("GLOBAL info: Next recptr %d, tot_occur %d, tot_numrecs %d\n",
	 tempdict.recptr, tempdict.tot_occur, 
	 tempdict.tot_numrecs);
  
  (void)dbcursor->c_close(dbcursor);

  cf_closeall();
  
}








