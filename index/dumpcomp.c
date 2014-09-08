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
*	Header Name:	dumpcomp.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining an component file
*
*	Usage:		dumpcomp configfile filename componentfile
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


main (int argc, char *argv[])
{
  FILE *test;
  DB *dbtest;
  DBT key, data;
  DBC *dbcursor;

  component_data_item comp_data;
  int comp_id;
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
    printf ("usage: %s configfilename filename component_file_name\n", argv[0]);
    exit (0);
  }

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  dbtest = (DB *) cf_component_open(argv[2], argv[3]);

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

    last_rec_proc++;

    if (key.size == sizeof(int)) {
      memcpy(&comp_id, key.data, key.size);
      printf ("Component ID: %d ",comp_id);
      memcpy((char *)&comp_data,data.data,data.size);
      
      printf(": record_id %d, start_offset %d, end_offset %d\n",
	     comp_data.record_id, comp_data.start_offset, 
	     comp_data.end_offset);

    }
    else { /* either a document entry or the max id */
      printf("%s ", (char *)key.data);

      if (data.size != sizeof(component_data_item)) {
	memcpy((char *)&comp_data,data.data,data.size);
	printf(": Next Component ID %d\n", comp_data.record_id);

      }
      else {
	memcpy((char *)&comp_data,data.data,data.size);
      
	printf(": record_id %d, First Component ID %d, Last Component ID %d\n",
	     comp_data.record_id, comp_data.start_offset, 
	     comp_data.end_offset);
      }
      
      
    }
    
  }
  
  if (errno != DB_NOTFOUND)
    fprintf(stderr, "Access error: get: %s\n", strerror(errno));
  
  (void)dbcursor->c_close(dbcursor);
  
  
  cf_closeall();
  
}








