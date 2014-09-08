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

extern continuation *first_cont;
extern int cont_restart_scanner(FILE *newfile);
extern void CONTparse(); 

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

  FILE *cfile, *fopen();
  char *fmread;
  int i;
  DB *db_handle;
  DBTYPE dbtype;
  int db_errno;
  DBT keyval;
  DBT dataval;
  int returncode;
  continuation *cont;
  char cfilename[500];

 
  LOGFILE = stderr;

  if (argc < 2) {
    fprintf(LOGFILE, "usage: %s {-q} CONTFILENAME DBENV_DIR\n Creates a DB file of the CONT data",argv[0], argv[0]);
    fflush(LOGFILE);
    exit(1);
  }

#ifdef WIN32
  /* have to define the file as binary */
  fmread = "rb";
#else
  fmread = "r";
#endif

  /* read the entire contfile?? */  
  if ((cfile = fopen((char *)argv[1],fmread)) == NULL) {
    fprintf(LOGFILE, "unable to open %s\n",(char *)argv[1]);
    exit(1);
  }

  sprintf(cfilename,"%s_DB",argv[1]);

    /* parse the EXISTING continuation input file */
  fseek(cfile, 0, SEEK_SET);
  cont_restart_scanner(cfile);
  CONTparse();

  fclose(cfile);

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
				 cfilename, NULL, DB_BTREE, 
				 DB_CREATE, 0664) != 0) {
    db_handle->err(db_handle, db_errno, 
		   "db_handle->open failed to create %s \n", 
		   cfilename);
  }

  /* should have opened the database file */
  
  for (cont = first_cont; cont; cont = cont->next_cont) {
 
    for (i = cont->docid_min; i <= cont->docid_max; i++) {
      

      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));
	  
      keyval.data = (void *)&i;
      keyval.size = sizeof(int);
	  
      returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
      if (returncode == 0) { /* found the key in the index already */
	
      }
      else if (returncode == DB_NOTFOUND) { 
	/* a new record not in the database */
	dataval.size = strlen(cont->name)+1;
	dataval.data = (void *)cont->name;
	/* put the high recno (0) into the database */
	returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 
    
      }
    }
  }
  /* sync the database -- flushing all of the buffers to disk */
  db_handle->sync(db_handle,0);
  
  /* close the database -- flushing all of the buffers to disk */
  db_handle->close(db_handle,0);

  gb_dbenv->close(gb_dbenv, 0);

  return 0;

}





