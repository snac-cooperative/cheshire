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
*	Header Name:	in_load.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for keyword indexing routines
*                       This program loads all indexes for a given
*                       config file.
*                       
*	Usage:		in_load configfilename <startnumber> <maxrec>
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		3/5/95
*	Revised:	
*	Version:	2.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#ifdef WIN32
#include <stdlib.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#define MAIN
#include "cheshire.h"
#include <signal.h>

extern int in_dbms (char *filename, int batch_flag, char *temp_file_dir, 
		    char *dbms_password);

extern int in_kwadd (char *filename, int recnum, int batch_flag, 
		     char *temp_file_dir);


/* global config file structure */
config_file_info *cf_info_base;


/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

void scram();
void scram2();
 
int last_rec_proc = 0;


main (int argc, char *argv[])
{
  int filetype;
  int i, nargs, startrec, maxrec;
  extern SGML_Doclist *PRS_first_doc;
#ifndef WIN32
  struct rlimit limits;
#endif
  config_file_info *cf;
  int return_code;
  int batch_flag = 0;
  time_t startime;
  char *temp_file_dir = NULL;
  char *sort_args = NULL;
  char *log_file_name = NULL;
  char *dbms_password = NULL;
#ifndef NO_RUSAGE
  struct rusage ru;
#endif
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "w+b";
  opflags = "r+b";
#else
  crflags = "w+";
  opflags = "r+";
#endif

  if (argc < 2) {
    printf ("usage: %s <-b -T tmpdir -S \"sort flags\" -L logfilename -p database_passwrd> configfilename <startrec> <maxrec>\n", argv[0]);
    exit (0);
  }

  /* call scram() if user hits DELETE/BREAK and on seg or bus error */
#ifndef WIN32
  signal(SIGINT, scram);
  signal(SIGBUS, scram);
  signal(SIGSEGV, scram);
  signal(SIGTERM, scram);
  signal(SIGKILL, scram);
  signal(SIGHUP, scram);
#ifndef HP
  signal(SIGXCPU, scram2);
  signal(SIGXFSZ, scram2);
#endif
#endif


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

  /* set the starting time */
  time(&startime);
  
  nargs = argc;

  while (nargs > 0) {
    if (argv[1][0] == '-' && argv[1][1] == 'b') { /* use batch processing */
      batch_flag = 1;
      nargs--;
      argc--;
      argv += 1;
    }
      /* the -T option is only technically available within -b and 
	 gives the temp directory */
    else if (argv[1][0] == '-' && argv[1][1] == 'T') {
      temp_file_dir = argv[2];
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'S') {
      sort_args = argv[2];
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'L') {
      log_file_name = argv[2];
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'p') {
      dbms_password = argv[2];
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-') {
      printf ("usage: %s <-b -T tmpdir -L logfilename> configfilename <startrec> <maxrec>\n", argv[0]);
      exit (0);
    }
    else
      nargs-- ;
  }

  if (log_file_name)
    LOGFILE = fopen(log_file_name,"a+"); /* open the logfile to append */
  else
    LOGFILE = fopen("INDEX_LOGFILE","a+"); /* open the logfile to append */

  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }

  /* prints the interesting bits of the config info structure */
  /*  cf_print_info (cf_info_base); */

  /* initialize or create all files */
  if (cf_createall() != 0) {
    printf("Error in file creation -- see log file\n");
    exit(-1);
  }

  if (argc >= 3) {
    startrec = atoi(argv[2]);
    last_rec_proc = startrec - 1;
  }
  else
    startrec = 1;

  for (cf = cf_info_base; cf ; cf = cf->next_file_info) {

    if (argc == 4)
      maxrec = atoi(argv[3]);
    else {
      maxrec = -1; /* this will be changed to the max in each file */
    }
    if ((filetype = cf->file_type) == FILE_TYPE_SGMLFILE
	|| filetype == FILE_TYPE_XML
	|| filetype == FILE_TYPE_XML_NODTD
	|| filetype == FILE_TYPE_XML_DATASTORE
	|| filetype == FILE_TYPE_MARCFILE
	|| filetype == FILE_TYPE_SGML_DATASTORE
	|| filetype == FILE_TYPE_MARC_DATASTORE) { 
      /* only main SGML files are processed here                        */
      /* BUT if the MARCFILE type is on, it means SGML using USMARC DTD */
      if (filetype > 99 && cf->file_db == NULL) {
	cf_open_datastore(cf);
      }
      if (maxrec == -1) maxrec = cf_getnumdocs(cf->file_name);
      
      fprintf(LOGFILE,
	      "\nRun Date: %s \nfile: %s (%s) startrec: %d maxrec: %d\n",
	      ctime(&startime), cf->file_name, cf->nickname, 
	      startrec, maxrec);
      fflush(LOGFILE);

      for (i = startrec; i <= maxrec; i++) {

	if (i % 500 == 0) {
#ifndef NO_RUSAGE
	  getrusage(RUSAGE_SELF, &ru);
	  fprintf (LOGFILE,"processing record %d -- Memory %d: I/O In %d Out %d\n",
		   i,ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
#else
	  fprintf (LOGFILE,"processing record %d\n", i);
#endif
	  fflush(LOGFILE);
	}

	return_code = in_kwadd(cf->file_name,i, batch_flag, 
			       temp_file_dir);

	if (i != last_rec_proc +1) {

	  fprintf(LOGFILE,
		  "EXITING: Serious error from in_kwadd: i(%d) != last record(%d) %d\n",
		  i, last_rec_proc, return_code);
	  fflush(LOGFILE);
	  exit(return_code);
	}
	last_rec_proc = i;

	if (return_code <= 0) {/* some sort of error occurred */
	  if (return_code == -1 || return_code == -3) {
	    fprintf(LOGFILE,
		    "EXITING: Serious error from in_kwadd: return code %d\n",
		    return_code);
	    fflush(LOGFILE);
	    exit(return_code);
	  }
	  else if (return_code == -2) {
	    /* bad sgml record -- ignor it -- it is already logged */
	    fflush(LOGFILE);
	    }
	  else {
	    fprintf(LOGFILE,
		    "EXITING: unknown error: kwadd return code %d record %d\n",
		    return_code, last_rec_proc);
	    fflush(LOGFILE);
	    exit(return_code);
	  }
	}
	/* free any un-freed documents  and the doclist structures */
	/* free all the doc_list memory */
	if (PRS_first_doc) {
	  free_doc_list(PRS_first_doc);
	  PRS_first_doc = NULL;
	}
      } /* end of main for loop */

      if (batch_flag) {
	/* the processing has created temporary files that need to be */
	/* merged and the information stored in the proper indexes    */
	in_batch_load(cf, temp_file_dir, sort_args);
      }
      /* Make sure everything is on disk */
      in_sync_indexes(cf);
  
      fprintf(LOGFILE,"Last record processed %d\n", last_rec_proc); 

    }
    else if (filetype == FILE_TYPE_VIRTUALDB) {
      fprintf(LOGFILE,"Virtual Databases may not be indexed directly...\n"); 
      fprintf(LOGFILE,"Index each database making up the virtual db separately\n"); 
      
    }
    else if (filetype == FILE_TYPE_DBMS) {

      fprintf(LOGFILE,
	      "\nRun Date: %s \nfile: %s (%s) startrec 0 maxrec: determined for each index...\n",
	      ctime(&startime), cf->file_name, cf->nickname);
      fprintf(LOGFILE,"Indexing from DBMS connection...\n"); 
      fflush(LOGFILE);

      return_code = in_dbms (cf->file_name,batch_flag, temp_file_dir, 
			     dbms_password);
      if (return_code >= 0 && batch_flag) {
	/* the processing has created temporary files that need to be */
	/* merged and the information stored in the proper indexes    */
	in_batch_load(cf, temp_file_dir, sort_args);
      }
      /* Make sure everything is on disk */
      if (return_code >= 0)
	in_sync_indexes(cf);      
    }
  }

  /* free all the DTDs */
  free_all_dtds();

  cf_closeall();

  close_grid_data();

  time(&startime);

  fprintf(LOGFILE,"Last record processed %d at time: %s -- ", 
	  last_rec_proc, ctime(&startime)); 

#ifndef NO_RUSAGE
  getrusage(RUSAGE_SELF, &ru);
  fprintf (LOGFILE,"Memory %d: I/O In %d Out %d\n",
		   ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
#endif
  fclose(LOGFILE);
  return (0);
}


void scram (int insignal)
{
  cf_closeall(); /* this should sync all incore data to disk */
  fprintf(stderr, "Interupted by SIGNAL %d: Last record processed %d\n", 
	  insignal, last_rec_proc);
  fprintf(LOGFILE,
	  "Interupted by SIGNAL: Last record processed %d\n", 
	  last_rec_proc); 
  fflush(LOGFILE);
  fclose(LOGFILE);

  abort(); /* force a core dump */
  exit (1);

}

#ifndef WIN32
void scram2 ()
{
  struct rlimit limits;
#ifndef NO_RUSAGE
  struct rusage ru;
#endif


  fprintf(LOGFILE,
	  "LIMITATION SIGNAL: Last record processed %d\n", 
	  last_rec_proc); 

  /* dump number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);
  fprintf(LOGFILE, 
	  "curr fileno %d  max fileno %d\n",  
	  limits.rlim_cur, limits.rlim_max);
#ifndef HP
  getrlimit(RLIMIT_FSIZE, &limits);
  fprintf(LOGFILE, 
	  "curr file size %d  max file size %d\n",  
	  limits.rlim_cur, limits.rlim_max);

  getrlimit(RLIMIT_CPU, &limits);
  fprintf(LOGFILE, 
	  "curr CPU %d  max CPU %d\n",  
	  limits.rlim_cur, limits.rlim_max);

  getrlimit(RLIMIT_DATA, &limits);
  fprintf(LOGFILE, 
	  "curr DATA %d  max DATA %d\n",  
	  limits.rlim_cur, limits.rlim_max);

#ifndef NO_RUSAGE
  getrusage(RUSAGE_SELF, &ru);
  fprintf (LOGFILE,"Current Process Memory Used %d: I/O In %d Out %d\n",
		   ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
#endif
#endif

  fflush(LOGFILE);
  fclose(LOGFILE);
  cf_closeall();

  exit (1);

}

#endif

