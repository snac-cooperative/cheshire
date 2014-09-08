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
*	Header Name:	dumpds.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining keys of a datastore file
*
*	Usage:		dumpds configfile name
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		Aug 08, 1997
*	Revised:	
*	Version:	2.0
*	Copyright (c) 1993.  The Regents of the University of California
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

int last_rec_proc = 0;

extern config_file_info *find_file_config(char *filename);

main (int argc, char *argv[])
{
  FILE *test;
  DB *dbtest;
  DBT key, data;
  DBC *dbcursor;
  config_file_info *cf;
  dict_info tempdict;
  idx_result tempinfo;
  int outid;
  unsigned char *c;
  char digest[17];
  int count;

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

  if (argc < 3) {
    printf ("usage: %s configfilename mainfile_name \n", argv[0]);
    exit (0);
  }

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  cf = find_file_config(argv[2]);

  cf_open_datastore(cf);

  dbtest = (DB *) cf->file_db;

  if (dbtest == NULL) {
    printf("Unable to access Datastore file %s; Not DS file?\n", argv[2]);
    exit(1);
  }


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

    last_rec_proc++;

    if (key.size == sizeof(int)) {
      memcpy(&outid, key.data, sizeof(int));
      printf("Docid key: %d : Stored record size %d\n", outid, data.size);
    }
    else {
      memcpy(&outid,data.data,sizeof(int));

      printf("Digest key: ");

      memcpy(&digest, key.data, key.size);
      
      for (c = digest, count = 0; count < 17; count++, c++)
	printf("%02x ", (unsigned char)*c);

      printf(": for Docid %d\n",outid);
    }

  }

  if (errno != DB_NOTFOUND)
    fprintf(stderr, "Access error: get: %s\n", strerror(errno));

  (void)dbcursor->c_close(dbcursor);


  cf_closeall();
  
}









