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
*	Header Name:	configtest/test_config
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for configfiles routines
*
*	Usage:		configtest configfilename
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/10/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
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

/* global config file structure */
config_file_info *cf_info_base;
int last_rec_proc = 0; 


main (int argc, char *argv[])
{
  int i;
#ifndef WIN32
  struct rlimit limits;
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


  LOGFILE = stderr;

  if (argc < 2) {
    printf ("usage: %s configfilename <configfilename>...{\"test lc number\"}\n", argv[0]);
    exit (0);
  }

  /* the following line reads the config info from its file */

printf("Validating Configuration file(s)\n");

  for (i = 1; i < argc; i++) 
    cf_info_base = cf_initialize(argv[i],NULL,NULL);

  /* prints the interesting bits of the config info structure */
  cf_print_info (cf_info_base);

  cf_closeall();
printf("\nConfiguration file checking completed\n");
   
}

/* the following is copied from zserver/displayrec.c for test_config */

idx_key *
buildxpathkey(char *element_name) 
{
  idx_key *k, *firstkey, *lastkey, *valkey;
  char *work, *start;

  work = strdup(element_name);
  /* sometimes tcl is too helpful...*/
  if (*work == '{')
    work++;

  start = strtok(work, "/[");
  k = firstkey = lastkey = NULL;

  do {
    if (strchr(start,']') != NULL) {
      /* an occurrence indicator */
      if (lastkey == NULL) {
	/* error -- occurrence without a tag */
	fprintf (LOGFILE,"Error in XPATH specification\n");
	diagnostic_set(227,0, "Error in XPATH specification for display -- Occurrence indicator with no tag?");
	return NULL;
      }
      lastkey->occurrence = atoi(start);
    }
    else {
      /* another step in the path */
      k = CALLOC(idx_key, 1);
      k->key = start;
      if (firstkey == NULL) {
	firstkey = k;
	lastkey = k;
      }
      else {
	lastkey->subkey = k;
	lastkey = k;
      }
    }
  } while (start = strtok(NULL, "/["));


  /* now step through and handle attribute specs */
  for (k = firstkey; k ; k = k->subkey) {
    if ((start = strchr(k->key,'@')) != NULL) {
      start++;
      k->key = start;
      k->attribute_flag = 1;
      if ((start = strchr(k->key,'=')) != NULL) {
	*start++ = '\0';
	while (*start == '"' || *start == '\'') 
	  start++;
	valkey = CALLOC(idx_key, 1);
	valkey->attribute_flag = 2;
	valkey->key = start;
	if (k->subkey != NULL)
	  valkey->subkey = k->subkey;
	k->subkey = valkey;
	/* terminate it */
        if ((work = strchr(start,'"')) || (work = strchr(start,'\''))) {
	  *work = '\0';
	}
      }
    } 
  }

  if ((start = strchr(lastkey->key,'}')) != NULL) {
    *start = '\0';
  }

  return(firstkey);

}










