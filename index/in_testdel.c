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
*	Header Name:	in_testdel.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for keyword indexing routines to
*                       delete the index entries for individual records
*                       and postings in a index.
*                     
*                       You must also supply a list of records
*                       to be deleted by the program as
*                       command line arguments
*                       
*                       
*	Usage:		in_testdel configfilename rec# rec# ...
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/06/96
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
#define strcasecmp _stricmp
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#define MAIN
#include "cheshire.h"
#include <signal.h>

struct MemBlock *RememberKey;

/* global config file structure */
config_file_info *cf_info_base;
 
int last_rec_proc = 0;

extern int FIXONLY; /* special flag for this version of indexing */

extern int in_comp_del(config_file_info *cf, char *filename, int recnum); 
			

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;


/* special flag for de-duping only (otherwise */
/* the records and index entries would be deleted */
#define DEDUPONLY 0

void scram();
void scram2();


main (int argc, char *argv[])
{
  FILE *test;
  int filetype;
  DB *dbtest;
  int i, count, startrec, maxrec;
  extern SGML_Doclist *PRS_first_doc;
#ifndef WIN32
  struct rlimit limits;
#endif
  config_file_info *cf;
  int return_code;
  time_t startime;
  char *testfilename = NULL;
  char *log_file_name = NULL;
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

  /* set the global FIXONLY variable to force only the non-roman stuff */
  /* to be re-indexed                                                  */
  FIXONLY = 0;


  if (argc < 4) {
    printf ("usage: %s [-L logfilename] configfilename filename record_number1 record_number2 ...\n", argv[0]);
    exit (0);
  }



  if (argv[1][0] == '-' && argv[1][1] == 'L') {
    log_file_name = argv[2];
    argc -= 2;
    argv += 2;
  }

  if (log_file_name)
    LOGFILE = fopen(log_file_name,"a+"); /* open the logfile to append */
  else
    LOGFILE = fopen("INDEX_LOGFILE","a+"); /* open the logfile to append */


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

  if (cf_info_base->defaultpath != NULL) { 
    testfilename = CALLOC(char, 
			  (strlen(cf_info_base->defaultpath) 
			   + strlen(argv[2]) + 2));
#ifndef WIN32
    sprintf(testfilename, "%s/%s",
	    cf_info_base->defaultpath, argv[2]);
#else
    sprintf(testfilename, "%s\\%s",
	    cf_info_base->defaultpath, argv[2]);
#endif
    
  }
  else 
    testfilename = "";
  
  for (cf = cf_info_base; cf ; cf = cf->next_file_info) {

    if (strcasecmp(argv[2], cf->nickname) == 0
	|| strcasecmp(argv[2], cf->file_name) == 0
	|| strcasecmp(testfilename, cf->file_name) == 0 ) {
      if ((filetype = cf_getfiletype(cf->file_name)) == FILE_TYPE_SGMLFILE 
	  || filetype == FILE_TYPE_SGML_DATASTORE
	  || filetype == FILE_TYPE_XML
	  || filetype == FILE_TYPE_XML_NODTD
	  || filetype == FILE_TYPE_XML_DATASTORE
	  || filetype == FILE_TYPE_MARC_DATASTORE
	  || filetype == FILE_TYPE_MARCFILE) { 
	/* only main SGML files are processed here */

	if (filetype > 99 && cf->file_db == NULL) {
	  cf_open_datastore(cf);
	}

	if (maxrec == -1) maxrec = cf_getnumdocs(cf->file_name);

	fprintf(LOGFILE,
		"\nDELETE Run Date: %s \nfile: %s (%s) : first delete: %d\n",
		ctime(&startime), cf->file_name, cf->nickname, 
		argv[2]);

	fflush(LOGFILE);

	i=0;
	count=0;

	for (count = 3; count < argc; count++) {

	  i = atoi(argv[count]);
#ifndef NO_RUSAGE
	  getrusage(RUSAGE_SELF, &ru);
	  fprintf (LOGFILE,"Deleting record %d -- Memory %d: I/O In %d Out %d\n",
		   i,ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	

#else
	  fprintf (LOGFILE,"Deleting record %d \n", i);

#endif
	  fflush(LOGFILE);

	  return_code = in_kwdel(cf->file_name, i, 0);

	  if (return_code <= 0) {/* some sort of error occurred */
	    if (return_code == -1 || return_code == -3) {
	      fprintf(LOGFILE,
		      "EXITING: Serious error from in_kwdel: return code %d\n",
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
		      "EXITING: unknown error: kwdel return code %d record %d\n",
		      return_code, last_rec_proc);
	      fflush(LOGFILE);
	      exit(return_code);
	    }
	  }

          return_code = in_comp_del(cf,cf->file_name, i); 

	  /* delete the record using the fileio routines */
	  fil_del(i,cf->file_name);

	  last_rec_proc = i;

	  /* free any un-freed documents  and the doclist structures */
	  /* free all the doc_list memory */
	  if (PRS_first_doc) {
	    free_doc_list(PRS_first_doc);
	    PRS_first_doc = NULL;
	  }
	} /* end of main for loop */
	
	in_sync_indexes(cf);
	
	fprintf(LOGFILE,"Last record processed %d\n", last_rec_proc); 
	
      }
      else if (filetype == FILE_TYPE_VIRTUALDB) {
	fprintf(LOGFILE,"Virtual Databases may not be indexed or manipulated directly...\n"); 
	fprintf(LOGFILE,"Index or delete from each database making up the virtual db separately\n"); 
	
      }

    }
    else {
      printf("Filename or Filetag '%s' not found in configfile %s\n",
	     argv[2], argv[1]);
    }
  }
  /* free all the DTDs */
  free_all_dtds();
  
  cf_closeall();
  
  time(&startime);
  
  fprintf(LOGFILE,"Last record processed %d at time: %s\n", 
	  last_rec_proc, ctime(&startime)); 

  fclose(LOGFILE);

  exit(0);

}

#ifndef WIN32
void scram ()
{
  cf_closeall();

  fprintf(LOGFILE,
	  "Interupted by SIGNAL: Last record processed %d\n", 
	  last_rec_proc); 
  fflush(LOGFILE);
  fclose(LOGFILE);

  abort(); /* force a core dump */
  exit (1);

}

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
