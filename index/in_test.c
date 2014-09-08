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
*	Header Name:	in_test.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for keyword indexing routines
*
*	Usage:		in_test configfilename file start maxrec
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/31/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>

#define MAIN
#include "cheshire.h"

/* global config file structure */
config_file_info *cf_info_base;
 
int last_rec_proc = 0;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;

main (int argc, char *argv[])
{
  FILE *test;
  int filetype;
  DB *dbtest;
  int i, startrec, maxrec;
  extern SGML_Doclist *PRS_first_doc;
  struct rlimit limits;
  config_file_info *cf;
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "w+b";
  opflags = "r+b";
#else
  crflags = "w+";
  opflags = "r+";
#endif


  LOGFILE = stderr;

  if (argc < 3) {
    printf ("usage: %s configfilename file <startrec> <maxrec>\n", argv[0]);
    exit (0);
  }

  /* Crank up the number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);

#ifdef DEBUGIND
  printf("curr fileno %d  max fileno %d\n",  limits.rlim_cur, limits.rlim_max);
#endif

  if (limits.rlim_cur < limits.rlim_max) {
    limits.rlim_cur = limits.rlim_max;
    setrlimit(RLIMIT_NOFILE, &limits);
  }

#ifdef DEBUGIND
  getrlimit(RLIMIT_NOFILE, &limits);
  printf("curr fileno %d  max fileno %d\n",  limits.rlim_cur, limits.rlim_max);
#endif


  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }

  /* prints the interesting bits of the config info structure */
  /*  cf_print_info (cf_info_base); */

  /* initialize or create all files */
  cf_createall();
  

  if (argc >= 4) 
    startrec = atoi(argv[3]);
  else
    startrec = 1;

  if (argc == 5)
    maxrec = atoi(argv[4]);
  else
    maxrec = cf_getnumdocs(argv[2]);
  
  printf("file: %s startrec: %d maxrec: %d\n", 
	 argv[2], startrec, maxrec);
      
  for (i = startrec; i <= maxrec; i++) {
    printf ("processing record %d\n",i);
    in_kwadd(argv[2],i);

  }
  
  
  /* free any un-freed documents  and the doclist structures */
  /* free all the doc_list memory */
  if (PRS_first_doc)
    free_doc_list(PRS_first_doc);
  
  PRS_first_doc = NULL;

#ifdef NEEDTOTRYWITHOUT
  /* create the cluster files */
  if ((filetype = cf_getfiletype(argv[2])) == FILE_TYPE_SGMLFILE
      || filetype == FILE_TYPE_XML
      || filetype == FILE_TYPE_XML_NODTD
      || filetype == FILE_TYPE_XML_DATASTORE
      || filetype == FILE_TYPE_MARCFILE
      || filetype == FILE_TYPE_SGML_DATASTORE
      || filetype == FILE_TYPE_MARC_DATASTORE) { 
    /* only main SGML files are processed here */
    in_clusmerg(argv[2]);
  }
#endif
  /* free all the DTDs */
  free_all_dtds();
  
  cf_closeall();

}



