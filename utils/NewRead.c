/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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
/* This program reads data stored into a BerkeleyDB file in parsed form    
 * and dumps the SGML to stdout.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef MACOSX
#include <sys/time.h>
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
#include "md5.h"
#include "dmalloc.h"
#include "flatrec.h"


int last_rec_proc = 0;

extern DB_ENV *gb_dbenv;


extern SGML_Document *rebuild_flat_record(Output_Record *inrec, 
					  int length,
					  SGML_DTD *dtd);

int
main (int argc, char **argv)
{
  int result, get_next_sgmlrec_data();
#ifndef WIN32
  struct rlimit limits;
#endif
  config_file_info *cf;
  char *crflags, *opflags;
  char *fmread, *fmwrite;
  SGML_Document *doc;
  SGML_DTD *main_dtd, *cf_getDTD();
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the record to be displayed */
  
  struct MD5Context mdcontext;
  char digestbuff[17];
  DB *db_handle;
  DBTYPE dbtype;
  int db_errno;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  int outrec_length;
  int i;
  int exactflag = 0;
  int numberflag = 0;
  
  LOGFILE = stderr;
  
  recno = 0;
  high_recno = 0;
  
#ifndef WIN32
  /* Crank up the number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);
  
  if (limits.rlim_cur < limits.rlim_max) {
    limits.rlim_cur = limits.rlim_max - 1;
    setrlimit(RLIMIT_NOFILE, &limits);
  }
  
#ifdef DEBUGIND
  getrlimit(RLIMIT_NOFILE, &limits);
  printf("curr fileno %d  max fileno %d\n",  limits.rlim_cur, limits.rlim_max);
#endif
#endif

#ifdef WIN32
  crflags = "wb";
  opflags = "rb";
#else
  crflags = "w";
  opflags = "r";
#endif
  
  
  if (argc < 3) {
    fprintf(LOGFILE, "usage: %s configfile filename <docid_number> <\"EXACT\" or \"NUMBER\">\n", 
	    argv[0]);
    exit(1);
  }
  
#ifdef WIN32
  /* have to define the file as binary */
  fmread = "rb";
  fmwrite = "wb+";
#else
  fmread = "r";
  fmwrite = "w+";
#endif
  
  if (strcasecmp(argv[argc-1], "EXACT") == 0) 
    exactflag = 1;

  if (strcasecmp(argv[argc-1], "NUMBER") == 0) 
    numberflag = 1;

  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);
  
  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }
  
  cf_init_db_env(cf_info_base); 
  
  /* prints the interesting bits of the config info structure */
  /* cf_print_info (cf_info_base); */
  
  if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
    gb_dbenv->err(gb_dbenv, db_errno,
		  "db_create failed to create handle\n");
    return(0);
  }
  
  db_handle->set_pagesize(db_handle, 8 * 1024);
  
  if (db_errno = db_handle->open(db_handle,
				 cf_info_base->file_name, NULL, DB_BTREE, 
				 0, 0664) != 0) {
    db_handle->err(db_handle, db_errno, 
		   "db_handle->open failed to open %s \n", 
		   cf_info_base->file_name);
  }
  
  /* should have opened the database file */
  
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
  keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&high_recno,dataval.data, sizeof(int));
  }
  else if (returncode == DB_NOTFOUND) { 
    printf("Database not initialized -- no MAXIMUM_RECORD_NUMBER entry\n");
    exit(0);
  }

  if (numberflag) {
    printf("Maximum Record Number is %d\n", high_recno);
    exit(0);
  }

  if (argc >= 4) {
    recno = atoi(argv[3]);
    if (recno == 0 && exactflag == 0) {
      printf("record number argument not a number\n");
      exit(0);
    }
  }
  
  if (recno > 0) {
    printf ("Reading record %d from database file %s...\n", 
	    recno, cf_info_base->file_name); 
    high_recno = recno;
    
  }
  else {
    recno = 1;
  }
  
  
  main_dtd = (SGML_DTD *) cf_getDTD(argv[2]);
  if (main_dtd == NULL) {
    printf("Couldn't get main dtd '%s' for filename or filetag '%s'\n", 
	   cf_info_base->DTD_name, argv[2]);
    exit(0);
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  
  for (i = recno; i <= high_recno; i++) {
    
    /* set up the new key */
    keyval.data = (void *)&i;
    keyval.size = sizeof(int);
    
    
    returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
    if (returncode == 0) { /* found the record */
      outrec = (Output_Record *)CALLOC(char, dataval.size);       
      memcpy(outrec,dataval.data, dataval.size);
    }
    else if (returncode == DB_NOTFOUND) { 
      /* a new record not in the database */
      printf("Record #%d not found in database\n", i);
      exit(0);
    }
    
    doc = rebuild_flat_record(outrec, dataval.size, main_dtd); 

    FREE(outrec);

    if (exactflag) {
      /* just dump the buffer */
      printf("%s\n",doc->buffer);
    }
    else 
      dump_doc(doc);

    free_doc(doc);

    last_rec_proc = recno;
  }
  
  /* close the database -- flushing all of the buffers to disk */
  db_handle->close(db_handle,0);
  
  exit(0);
  
}





