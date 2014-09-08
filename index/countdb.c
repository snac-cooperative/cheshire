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
*	Header Name:	countdb.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility to show term freq characteristics of an
*                       index
*
*	Usage:		countdb configfilename mainfilename indexname
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		8/20/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
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
  int entrycount = 0;
  double sum = 0.0;

  int freqs[200];
  int morethan200 = 0;

  int maxfreq = 0;
  char maxword[200];
  
  dict_info tempdict;
  idx_result tempinfo;
  idx_list_entry *idx;

  int i, startrec, numrecs;
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "r+b";
  opflags = "r+b";
#else
  crflags = "r+";
  opflags = "r+";
#endif


  LOGFILE = stderr;

  if (argc < 4) {
    printf ("usage: %s configfilename mainfile_name index_name\n", argv[0]);
    exit (0);
  }
  
  for (i = 1 ; i < 200; i++)
    freqs[i] = 0;

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], "r", "r");

  idx = cf_getidx_entry(argv[2], argv[3]);   

  dbtest = (DB *) cf_index_open(argv[2], argv[3] , INDEXFL);

  if (dbtest == NULL)
    exit(1);

  /* Acquire a cursor for the database. */
  if ((errno = dbtest->cursor(dbtest, NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "countdb: cursor: %s\n", strerror(errno));
    exit (1);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));

  while ((errno = dbcursor->c_get(dbcursor, &key, &data, DB_NEXT)) == 0) {

    if (idx->type & BITMAPPED_TYPE && data.size == bitmap_blocksize_bytes) {
      continue;
    }

    if (data.size < sizeof(idx_result)) 
      continue;

    entrycount++;

    memcpy((char *)&tempinfo,data.data,sizeof(idx_result));
    
    if (tempinfo.num_entries > maxfreq) {
	maxfreq = tempinfo.num_entries;
	strncpy(maxword, (char *)key.data, 199);
    }

    if (tempinfo.num_entries < 200) 
      freqs[tempinfo.num_entries]++;
    else
      morethan200++;

    sum += (double)tempinfo.num_entries;

  }

  (void)dbcursor->c_close(dbcursor);

  cf_closeall();
  
  printf("Frequency summary information for file %s index %s\n",
	 argv[2], argv[3]);
  
  printf("Total index entries = %d\n", entrycount);
  printf("Mean records per entry = %f\n", sum/(double)entrycount);
  printf("Max records per entry = %d (%s)\n", maxfreq, maxword);
  printf("Top 200 frequency classes:\n");
  printf("Freq\tCount\n");
  for (i = 1; i < 200; i++) {
    printf("%d\t%d\n", i, freqs[i]);
  }
  printf (">=200\t%d\n", morethan200);
}








