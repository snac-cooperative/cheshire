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
*	Header Name:	batch_load.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for special situation where a batch
*                       indexing run has aborted for some reason
*                       leaving various BATCHTEMP or BATCHSORT
*                       files unloaded into the appropriate indexes
*                       
*	Usage:		batch_load configfilename
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
#include <sys/time.h>
#include <machine/types.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>

#define MAIN
#include "cheshire.h"
#include <signal.h>
#include <sys/stat.h>

/* global config file structure */
config_file_info *cf_info_base; 
int last_rec_proc = 0;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;


extern batch_files_info *batch_files;

void scram();
void scram2();



main (int argc, char *argv[])
{
  FILE *test, *tempfile, *sortfile;
  int filetype;
  DB *index_db;
  int i, startrec, maxrec;
  extern SGML_Doclist *PRS_first_doc;
  struct rlimit limits;
  config_file_info *cf;
  int return_code;
  int batch_flag = 0;
  time_t startime;
  char *temp_file_dir = NULL;
  char *log_file_name = NULL;
  char *sort_args = NULL;
  char *file_name;
  char *fmode;
  char *crflags, *opflags;
  int nargs;

#ifndef NO_RUSAGE
  struct rusage ru;
#endif
  char tempfilename[500];
  batch_files_info *bf, *prev;
  idx_list_entry *idx, *cf_getidx();
  FILE *index_stoplist;
  component_list_entry *comp;

#ifdef WIN32
    crflags = "w+b";
    opflags = "r+b";
#else
    crflags = "w+";
    opflags = "r+";
#endif


  if (argc < 2) {
    printf ("usage: %s {-b -T tempfiledir -L log_file_name -S \"added sort args\"} configfilename\n", argv[0]);
    exit (0);
  }


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
  
  /* call scram() if user hits DELETE/BREAK and on seg or bus error */
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


  /* Crank up the number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);

  if (limits.rlim_cur < limits.rlim_max) {
    limits.rlim_cur = limits.rlim_max - 1;
    setrlimit(RLIMIT_NOFILE, &limits);
  }

#ifdef DEBUG
  getrlimit(RLIMIT_NOFILE, &limits);
  printf("curr fileno %d  max fileno %d\n",  limits.rlim_cur, limits.rlim_max);
#endif

  /* set the starting time */
  time(&startime);

  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags,opflags);

  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }

  /* initialize or create all files */
  if (cf_createall() != 0) {
    printf("Error in file creation -- see log file\n");
    exit(-1);
  }

  for (cf = cf_info_base; cf ; cf = cf->next_file_info) {

    if ((filetype = cf_getfiletype(cf->file_name)) == FILE_TYPE_SGMLFILE
	|| filetype == FILE_TYPE_SGML_DATASTORE
	|| filetype == FILE_TYPE_XML
	|| filetype == FILE_TYPE_XML_NODTD
	|| filetype == FILE_TYPE_XML_DATASTORE
	|| filetype == FILE_TYPE_MARC_DATASTORE
	|| filetype == FILE_TYPE_MARCFILE) { 
      
      
      fprintf(LOGFILE,
	      "\nBatch Load Run Date: %s \nfile: %s (%s)\n",
	      ctime(&startime), cf->file_name, cf->nickname);
      
      for (idx = cf_getidx(cf->file_name) ; idx ; idx = idx->next_entry) {
	
	tempfile = NULL;
	sortfile = NULL;
	
	index_db = (DB *) cf_index_open(cf->file_name, idx->tag, INDEXFL);
	index_stoplist = (FILE *) cf_index_open(cf->file_name, idx->tag, INDEXSTOPWORDS);
	
	if (temp_file_dir) {
	  file_name = strrchr(idx->name, '/');
	  file_name++;
	  sprintf(tempfilename, "%s/%s_BATCHTEMP", temp_file_dir, file_name);
	}
	else
	  sprintf(tempfilename, "%s_BATCHTEMP", idx->name);
	
#ifdef WIN32
	fmode = "rb";
#else 
	fmode = "r";
#endif
	
	if ((tempfile = fopen(tempfilename, fmode)) == NULL) {
	  /* no BATCH_TEMP -- try BATCHSORT */
	  
	  if (temp_file_dir) {
	    file_name = strrchr(idx->name, '/');
	    file_name++;
	    sprintf(tempfilename,"%s/%s_BATCHSORT", temp_file_dir, file_name);
	  }
	  else
	    sprintf(tempfilename,"%s_BATCHSORT", idx->name);
	  if ((sortfile = fopen(tempfilename, fmode)) == NULL) 
	    continue; /* no batch files for this index */
	}
	
	bf = CALLOC(batch_files_info, 1);
	if (batch_files == NULL) {
	  batch_files = bf;
	  prev = bf;
	}
	else
	  prev->next = bf;
	
	bf->idx = idx;
	bf->index_name = idx->name;
	if (tempfile) {
	  bf->outfile = tempfile;
	  bf->load_flag = 1;
	  fprintf(LOGFILE, "Found unfinished BATCHTEMP file %s\n", tempfilename);
	}
	if (sortfile) {
	  bf->outfile = sortfile;
	  bf->load_flag = 2;
	  fprintf(LOGFILE, "Found unfinished BATCHSORT file %s\n", tempfilename);
	}
	prev = bf;
	
      }
	
      /* Now process the batchfiles */
      in_batch_load(cf, temp_file_dir, sort_args);
      
      /* Make sure everything is on disk */
      in_sync_indexes(cf);
      
      
      for (comp = cf->components; comp; comp = comp->next_entry) {
	fprintf(LOGFILE,"Batch update for component %s\n", comp->name); 

	bf = NULL; /* start with new bf set... */
	
	for (idx = comp->indexes; idx; idx = idx->next_entry) {
	  /* only process if there is a match for the bf index */
	  /* since multiple cf entries may be processed in one */
	  /* run, not all will be the RIGHT cf                 */
	  tempfile = NULL;
	  sortfile = NULL;
	  
	  index_db = (DB *) cf_index_open(cf->file_name, idx->tag, INDEXFL);
	  index_stoplist = (FILE *) cf_index_open(cf->file_name, idx->tag, INDEXSTOPWORDS);
	  
	  if (temp_file_dir) {
	    file_name = strrchr(idx->name, '/');
	    file_name++;
	    sprintf(tempfilename, "%s/%s_BATCHTEMP", temp_file_dir, file_name);
	  }
	  else
	    sprintf(tempfilename, "%s_BATCHTEMP", idx->name);
	  
#ifdef WIN32
	  fmode = "rb";
#else 
	  fmode = "r";
#endif
	  
	  if ((tempfile = fopen(tempfilename, fmode)) == NULL) {
	    /* no BATCH_TEMP -- try BATCHSORT */
	    
	    if (temp_file_dir) {
	      file_name = strrchr(idx->name, '/');
	      file_name++;
	      sprintf(tempfilename,"%s/%s_BATCHSORT", temp_file_dir, file_name);
	    }
	    else
	      sprintf(tempfilename,"%s_BATCHSORT", idx->name);
	    if ((sortfile = fopen(tempfilename, fmode)) == NULL) 
	      continue; /* no batch files for this index */
	  }
	  
	  bf = CALLOC(batch_files_info, 1);
	  if (batch_files == NULL) {
	    batch_files = bf;
	    prev = bf;
	  }
	  else
	    prev->next = bf;
	  
	  bf->idx = idx;
	  bf->index_name = idx->name;
	  if (tempfile) {
	    bf->outfile = tempfile;
	    bf->load_flag = 1;
	    fprintf(LOGFILE, "Found unfinished component BATCHTEMP file %s\n", tempfilename);
	  }
	  if (sortfile) {
	    bf->outfile = sortfile;
	    bf->load_flag = 2;
	    fprintf(LOGFILE, "Found unfinished component BATCHSORT file %s\n", tempfilename);
	  }
	  prev = bf;
	  

	}
	  
	/* Now process the batchfiles */
	in_batch_load(cf, temp_file_dir, sort_args);
	
	/* Make sure everything is on disk */
	in_sync_indexes(cf);
	
      }
	
    }
    else if (filetype == FILE_TYPE_VIRTUALDB) {
      fprintf(LOGFILE,"Virtual Databases may not be indexed directly...\n"); 
      fprintf(LOGFILE,"Index or load each database making up the virtual db separately\n"); 
      
    }
    
  }
  
  
  /* free all the DTDs */
  free_all_dtds();

  cf_closeall();

  time(&startime);

  fprintf(LOGFILE,"Batch Load Processing completed at time: %s -- ", ctime(&startime)); 

#ifndef NO_RUSAGE
  getrusage(RUSAGE_SELF, &ru);
  fprintf (LOGFILE,"Memory %d: I/O In %d Out %d\n",
		   ru.ru_maxrss, ru.ru_inblock,ru.ru_oublock);	
#endif

  fclose(LOGFILE);

}


void scram ()
{
  cf_closeall(); /* this should sync all incore data to disk */
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

