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

/* This program parses each document in a collection and counts the
 * number of XML elements found. 
 */

#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#define MAIN
#include "cheshire.h"

extern char *temp_parse_buffer;
extern int cf_getnumdocs (char *filename);
extern SGML_DTD *DTD_list, *current_sgml_dtd;

extern FILE *sgml_in;
char *sgml_in_name;
int last_rec_proc = 0;

extern char *GetRawSGML(char *file, int i);

extern SGML_Document *sgml_parse_document(SGML_DTD *dtd, char *filename,
					  char *buffer,
					  int recordnum, int keep_buffer);

extern SGML_DTD *cf_getDTD(char *filename);


main (int argc, char **argv)
{

  SGML_Document *parsed_doc;
  SGML_DTD *dtd ;
  extern SGML_Doclist  *PRS_first_doc; /* set in document parsing */
  SGML_Data *sgmldata;

  FILE *infile;
  int morerecs = 1;
  int numdocs = 0;
  int num_to_process = 0;
  int startrec = 0;
  int i = 0;
  int number_of_elements;
  int number_of_records;
  int number_of_docs;
  char temp_buffer[TEMP_PARSE_BUFFER_SIZE];
  char *real_buffer;
  char *crflags, *opflags;
#ifndef WIN32
  struct rlimit limits;
#endif


#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif


  LOGFILE = stderr;

  ++argv, --argc; /* skip program name */

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


  if (argc > 0 ) {
    
    /* args should be a configfile name */
    
    /* the following line reads the config info from its file */
    cf_info_base = cf_initialize(argv[0], crflags, opflags);

    /* prints the interesting bits of the config info structure */
    /* cf_print_info (cf_info_base); */

    /* initialize or create all files */
    cf_createall();
    printf("All files created\n");

    printf("getting dtd\n");
    
    if (argc > 1)
      printf("databasename: %s\n", argv[1]);
    else {
      printf("usage: parser configfile databasename [num_to_process startrec]\n");
      exit(0);
    }
    
    if (argc >= 2) 
      dtd = cf_getDTD(argv[1]);
    else
      dtd = cf_getDTD("bibfile");

    if (argc >= 3)
      num_to_process = atoi(argv[2]);
    
    if (argc == 4)
      startrec = atoi(argv[3]);

    printf ("NOT dumping dtd\n");
    /* dump_dtd(dtd); */

    printf ("NOT DUMPING Completion pattern file to 'xx.completeout'\n");
    /* dump_completer(DTD_list, "xx.completeout"); */
    
    printf("\n\nStarting document parsing\n");

    if (argc >= 3)
      printf("Doing all via a buffer...\n");
    
    infile = cf_open(argv[1],MAINFILE);

    numdocs = cf_getnumdocs(argv[1]);
    number_of_docs = numdocs;


    if (num_to_process != 0 && startrec !=0)
      numdocs = num_to_process + startrec - 1;


    number_of_records = numdocs;

    if (startrec != 0)
      i = startrec - 1;

    number_of_elements = 0;

    while (i++ < numdocs) {
      printf("Processing Document #%d", i);
      last_rec_proc = i;
      real_buffer = GetRawSGML(argv[1], i);
      parsed_doc = sgml_parse_document(dtd, argv[1], real_buffer, i, 0);
      
      if (parsed_doc) {
	/* dump_doc(parsed_doc); */	
	number_of_elements += parsed_doc->element_count;
	printf(" = %d elements \n", parsed_doc->element_count);
	
      }

      free_doc(parsed_doc);
    }
    
    printf("freeing documents\n");
    free_doc_list(PRS_first_doc); /* free all the memory */
    
    
    
    /* close the config files and such */
    cf_closeall(); 
    
    /* free all the DTDs */
    printf("freeing DTD\n");
    free_all_dtds();

    if (number_of_records == number_of_docs) {
      printf("Total Elements (exact) = %d\n", number_of_elements);
    } else {

      printf("Total Elements (estimated) = %d\n", (number_of_elements/number_of_records)*number_of_docs);

    }

    
  }
  else printf("usage: parser configfile databasename [num_to_process startrec]\n");
}









