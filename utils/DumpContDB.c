/*
 *  Copyright (c) 1990-2008 [see Other Notes, below]. The Regents of the
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
/* This program loads CONT file data into a BerkeleyDB file in parsed form    
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define MAIN

#include "cheshire.h"
#include <dirent.h>
#include "dmalloc.h"

DB_ENV *gb_dbenv;

/*
 * db_init --
 *      Initialize the environment.
 */
int
db_init(char *home, DB_ENV **dbenvp)
{
  int ret;
  DB_ENV *dbenv;

  if ((ret = db_env_create(&dbenv, 0)) != 0) 
    return (ret);
  
  /* dbenv->set_lk_max(dbenv, 10000); */
  dbenv->set_cachesize(dbenv, 0, 16 * 1024 * 1024, 0);
  /* new version only ... dbenv->set_flags(dbenv, DB_CDB_ALLDB, 1); */
  
  if (home == NULL) {
    home = getenv("CHESHIRE_DB_HOME");
    if (home == NULL) {
      fprintf(stderr, "CHESHIRE_DB_HOME must be set OR config file <DBENV> set\n");
      fprintf(LOGFILE, "CHESHIRE_DB_HOME must be set OR config file <DBENV> set\n");
      return(1);
    }
  }
  if ((ret = dbenv->open(dbenv, home,
			 DB_INIT_MPOOL | DB_INIT_CDB
			 | DB_CREATE | DB_USE_ENVIRON, 
			 0)) == 0) {
    *dbenvp = dbenv;
    dbenv->set_errfile(dbenv, LOGFILE);
    dbenv->set_errpfx(dbenv, "BerkeleyDB");
    return (0);
  }
  /* this goes to stdout and screws up z39.50 connections -- hangs them */
  /* dbenv->err(dbenv, ret, "Could not open DB environment: %s", home); */
  fprintf(LOGFILE,"Could not open DB environment: %s\n",home);
  (void)dbenv->close(dbenv, 0);
  return (ret);
}


int
main (int argc, char **argv)
{

  int i;
  DB *db_handle;
  DBTYPE dbtype;
  int db_errno;
  DBT keyval;
  DBT dataval;
  DBC *dbcursor;
  int recid;

  LOGFILE = stderr;

  if (argc < 2) {
    fprintf(LOGFILE, "usage: %s CONTFILEDBNAME DBENV_DIR\n Dumps a DB file of the CONT data",argv[0], argv[0]);
    exit(1);
  }

  if (db_init(argv[2], &gb_dbenv) != 0) {
    fprintf(LOGFILE,"Error opening DBENV\n");
    return(0);
  }

  
  if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
    gb_dbenv->err(gb_dbenv, db_errno,
		  "db_create failed to create handle\n");
    return(0);
  }
  
  db_handle->set_pagesize(db_handle, 8 * 1024);
  
  if (db_errno = db_handle->open(db_handle,
				 argv[1], NULL, DB_BTREE, 
				 DB_CREATE, 0664) != 0) {
    db_handle->err(db_handle, db_errno, 
		   "db_handle->open failed to create %s \n", 
		   argv[1]);
  }


  /* Acquire a cursor for the database. */
  if ((errno = db_handle->cursor(db_handle, NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "dumpdb: cursor: %s\n", strerror(errno));
    exit (1);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));

  /* Walk through the database and print out the key/data pairs. */
  while ((errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT)) == 0) {
    /* move to aligned storage */
    memcpy((char *)&recid, (char *)keyval.data, keyval.size);
    printf("recordID: %d = %s\n", recid, dataval.data);
  }

  db_handle->close(db_handle,0);

  gb_dbenv->close(gb_dbenv, 0);

  return 0;

}





