/*
 *  Copyright (c) 1990-2005 [see Other Notes, below]. The Regents of the
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
*	Header Name:	dumpvector.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining an index inverted file
*                       and any associated vectorimity info file.
*
*	Usage:		dumpvector configfile mainfilename indexfilename
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		June 27, 2005
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2005.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#define MAIN
#include "cheshire.h"

/* global config file structure */
config_file_info *cf_info_base;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
 
int last_rec_proc = 0;

main (int argc, char *argv[])
{
  FILE *test;
  DB *dbtest;
  DB *dbvector;
  DBT key, data;
  DBT vectorkey, vectordata;
  DBC *dbcursor;
  int *outdata;
  idx_list_entry *idx;
  vec_term_id termid;
  int recordnum;
  char *rawvectordata;
  vector_data_info *vector_data;
  vec_term_overlay *term_overlay;
  int i, j, startrec, numrecs, numints, returncode;
  char *crflags, *opflags;
  int dbflag;

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

  dbtest = (DB *) cf_index_open(argv[2], argv[3] , INDEXFL);

  if (dbtest == NULL) 
    exit(0);
  
  idx = cf_getidx_entry(argv[2], argv[3]);
  
  if (idx == NULL)
    exit(0);

  dbvector = idx->vector_db;


  if (dbtest == NULL)
    exit(1);

  /* Acquire a cursor for the database. */
  if ((errno = dbvector->cursor(dbvector, NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "dumpdb: cursor: %s\n", strerror(errno));
    exit (1);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&vectorkey, 0, sizeof(vectorkey));
  memset(&vectordata, 0, sizeof(vectordata));


  /* Walk through the database and print out the vector info. */
  while ((errno = dbcursor->c_get(dbcursor, &vectorkey, &vectordata, DB_NEXT)) == 0) {

    if (vectorkey.size == sizeof(vec_term_id)) {
      continue;
    }
    else if (vectorkey.size == sizeof(int)) {
      memcpy((char *)&recordnum, (char *)vectorkey.data, vectorkey.size);

      vector_data = (vector_data_info *)CALLOC(char, vectordata.size);
      memcpy((char *)vector_data, (char *)vectordata.data, vectordata.size);
      
      printf("RecordNum %d, TFIDF %f, avg_term_freq %f, max_term_freq %d, numterms %d: \n", recordnum, 
	     vector_data->sqrt_sum_tfidf_sq, 
	     vector_data->avg_term_freq,
	     vector_data->max_term_freq,
	     vector_data->num_terms);
      printf("    termid      freq\n");
      for (i = 0; i < vector_data->num_terms; i++) {
	printf("%10d%10d\n", vector_data->entries[i].termid,
	       vector_data->entries[i].termfreq);
      }
    }
  }

  dbflag = DB_FIRST;
 

  /* Walk through the database and print out the key/data pairs. */
  while ((errno = dbcursor->c_get(dbcursor, &vectorkey, &vectordata, dbflag)) == 0) {

    if (vectorkey.size == sizeof(vec_term_id)) {
      memcpy((char *)&termid, (char *)vectorkey.data, vectorkey.size);
    
      printf("Termid %d, code %c = ",
	     termid.id, termid.code);
      
      term_overlay = vectordata.data;

      printf("%s (%d)\n", &term_overlay->term[0], term_overlay->coll_freq);   

    
    }
    dbflag = DB_NEXT;
      
  }

  
  (void)dbcursor->c_close(dbcursor);

  cf_closeall();
  
}








