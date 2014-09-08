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
*	Header Name:	in_fix.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for keyword indexing routines to
*                       provide a special fix for non-roman characters
*                       that were removed by a (former) bug in the
*                       regular index_cheshire.
*                     
*                       This program loads all indexes for a given
*                       config file BUT ONLY ADDS WORDS or XKEYS 
*                       CONTAINING ANSEL CHARACTERS IN THE RANGE
*                       161-190 Decimal (A1-BE Hex)
*                       You must also supply a list of records
*                       to be re-indexed by the program as
*                       a command line argument
*                       
*                       
*	Usage:		index_fix configfilename record_numbers_file
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/05/96
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
#include <sys/time.h>
#include <sys/resource.h>

#define MAIN
#include "cheshire.h"
#include <signal.h>

struct MemBlock *RememberKey;

/* global config file structure */
config_file_info *cf_info_base;
 
int last_rec_proc = 0;

extern int FIXONLY; /* special flag for this version of indexing */

FILE *stdoutfileptr, *stderrfileptr;
FILE *record_num_file;

main (int argc, char *argv[])
{
  FILE *test;
  int filetype;
  DB *dbtest;
  int i, count, startrec, maxrec;
  extern SGML_Doclist *PRS_first_doc;
  struct rlimit limits;
  config_file_info *cf;
  int return_code;
  time_t startime;
  struct rusage ru;
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "w+b";
  opflags = "r+b";
#else
  crflags = "w+";
  opflags = "r+";
#endif

  void scram();
  void scram2();

  /* call scram() if user hits DELETE/BREAK and on seg or bus error */
  signal(SIGINT, scram);
  signal(SIGBUS, scram);
  signal(SIGSEGV, scram);
  signal(SIGTERM, scram);
  signal(SIGKILL, scram);
  signal(SIGHUP, scram);

  signal(SIGXCPU, scram2);
  signal(SIGXFSZ, scram2);

  stderrfileptr = stderr;
  stdoutfileptr = stdout;

  /* set the global FIXONLY variable to force only the non-roman stuff */
  /* to be re-indexed                                                  */
  FIXONLY = 1;

  LOGFILE = fopen("INDEX_LOGFILE","a+"); /* open the logfile to append */

  if (argc != 3) {
    printf ("usage: %s configfilename record_numbers_file_name\n", argv[0]);
    exit (0);
  }


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

  /* open the file of bad records */
  if ((record_num_file = fopen(argv[2], "r")) == NULL) {
    printf("Unable to open record number file '%s'\n", argv[2]);
    exit(-1);
  }

  
  
  for (cf = cf_info_base; cf ; cf = cf->next_file_info) {
    filetype = cf_getfiletype(cf->file_name);
    if (filetype == FILE_TYPE_SGMLFILE 
	|| filetype == FILE_TYPE_SGML_DATASTORE
	|| filetype == FILE_TYPE_XML
	|| filetype == FILE_TYPE_XML_NODTD
	|| filetype == FILE_TYPE_XML_DATASTORE
	|| filetype == FILE_TYPE_MARC_DATASTORE
	|| filetype == FILE_TYPE_MARCFILE) { 
      /* only main SGML files are processed here */

      if (maxrec == -1) maxrec = cf_getnumdocs(cf->file_name);

      fprintf(LOGFILE,
	      "\nFIXER Run Date: %s \nfile: %s (%s) : fixfile: %s\n",
	      ctime(&startime), cf->file_name, cf->nickname, 
	      argv[2]);

      fflush(LOGFILE);


      /* file is open, so do the fix for each number in the file */
      i=0;
      count=0;

      while (fscanf(record_num_file,"%d",&i) != EOF) {

	count++;
	if (count % 100 == 0) {
	  getrusage(RUSAGE_SELF, &ru);
	  fprintf (LOGFILE,"processing record %d -- Memory %d: I/O In %d Out %d\n",
		   i,ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
	  fflush(LOGFILE);
	}

	return_code = in_kwadd(cf->file_name,i);

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

      in_sync_indexes(cf);
  
      fprintf(LOGFILE,"Last record processed %d\n", last_rec_proc); 

    }
  }

  /* free all the DTDs */
  free_all_dtds();

  cf_closeall();

  time(&startime);
  getrusage(RUSAGE_SELF, &ru);
  fprintf(LOGFILE,"Last record processed %d at time: %s -- ", 
	  last_rec_proc, ctime(&startime)); 
  fprintf (LOGFILE,"Memory %d: I/O In %d Out %d\n",
		   ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
  fclose(LOGFILE);
  fclose(record_num_file);
}


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
  struct rusage ru;


  fprintf(LOGFILE,
	  "LIMITATION SIGNAL: Last record processed %d\n", 
	  last_rec_proc); 

  /* dump number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);
  fprintf(LOGFILE, 
	  "curr fileno %d  max fileno %d\n",  
	  limits.rlim_cur, limits.rlim_max);

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

  getrusage(RUSAGE_SELF, &ru);
  fprintf (LOGFILE,"Current Process Memory Used %d: I/O In %d Out %d\n",
		   ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	

  fflush(LOGFILE);
  fclose(LOGFILE);
  cf_closeall();

  exit (1);

}

