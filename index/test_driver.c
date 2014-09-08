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
*       Header Name:    se_driver.c
*
*       Programmer:     Ray Larson 
*                       completely revised from the original by 
*                       Kuntz & O'Leary 
*
*       Purpose:        drive search engine 
*                      
*
*       Usage:          se_driver key
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/7/93
*       Revised:        11/28/93
*       Version:        0.99999999
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#define MAIN
/* #define TESTFORMATS */

/* Z39.50 Library includes */
#define PLATFORM_INC_MALLOC
#include "libz3950.h"
#include "ini.h"

#include <cheshire.h>
#include "z_parse.h"
#include "sgml.h"
#include "dispmarc.h"
#include <dmalloc.h>

int last_rec_proc; /* used in indexing and parsing code */

MARC_REC *DispMARCbuff(char *recbuffer, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)());

RPNQUERY *queryparse(char *query, char *attributeSetId);


extern int displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
		      char *format, char *oid, char *filename, char *add_tags,
		      char **outptr, int rank, int relevance, 
		      float raw_relevance, weighted_result *resultset);


void output_routine(char *line) {
  printf("%s",line);
}

main (argc, argv)
int argc;
char *argv[];
{
  RPNQUERY *RPN_Query_ptr;
  extern SGML_Doclist  *PRS_first_doc;

  char command[1000]; /* XXXX for simulated output from driver */
  
  int i;		  /* loop counter */
  char *cf_name;	  /* name of configuration file */
  char querystring[300];	/* string to hold query */
  char *filename;
  int num_request;
  int filetype;
  int set_number = 1;
  char result_set_name[50];
  char *result, *se_process_meta();
  char *diagnostic_string(), *diagnostic_addinfo();
  int resultlen;

  weighted_result *final_set;	/* result from query processing */
  
  display_result *present_set;     /* result from se_retrievemarc */
  
  
  /**** function prototypes ****/

  weighted_result *se_process_query(char *, RPNQUERY *);
  display_result  *se_retrievemarc(int, int, weighted_result *);
  char *se_get_result_filetag(char *);
  /*****************************/

  LOGFILE = stderr;
  
  if (argc < 3) { 
    fprintf(LOGFILE, 
	    "Wrong number of args should be: %s configfile filename query_string\n", 
	    argv[0]);
    exit (1);
  }

  cf_name = argv[1];
  cf_info_base = (config_file_info *) cf_initialize(cf_name, NULL, NULL);
  
  filename = argv[2];

  if (cf_info_base == NULL) {
    fprintf(LOGFILE, 
	    "bad config file ? should be: %s configfile filename query_string\n", 
	    argv[0]);
    exit (2);
  }
  
  filetype = cf_getfiletype(filename);

  if (argc > 3) {
    for (i=3; i<argc; i++) {
      strcat(querystring,argv[i]);
      strcat(querystring," ");
    }
  }
  else {
    printf("Query-> ");
    gets(querystring);
  }

  while (querystring[0] != 0) {

    /* printf("The Query is : '%s'\n", querystring); */

 
    RPN_Query_ptr = queryparse(querystring, "default"); /* defaults to bib-1 */

    clean_up_query_parse();  

/*  printf("Query in RPN form:\n");
    dump_rpn_query(RPN_Query_ptr);
    printf("\n");
*/
    if (strcasecmp(filename, "METADATA") == 0) {
      
      printf("Metadata Query --\n");
      result = se_process_meta(RPN_Query_ptr);
      if (result == NULL) {
	int code;
	if (code = diagnostic_get()) {
	  printf("Diagnostic code %d : %s : Addinfo = '%s'\n",
		 code,diagnostic_string(code), diagnostic_addinfo());
	  diagnostic_clear();
	  
	}
      }
      else {
	/* must have it -- so print it */
	printf("\n+++++++++++++\n");

	resultlen = strlen(result);
	if (resultlen == 0)
	  printf("No result-- must not be a clustered file");
	for (i = 0; i < resultlen; i++)
	  putchar(result[i]);

	printf("\n+++++++++++++\n");
      }
    }
    else {
      final_set = se_process_query(filename, RPN_Query_ptr);

      free_rpn_query(RPN_Query_ptr);

      if (final_set != NULL) {
	char *getfile;
	
	/************************************************************
	 * XXXX this code simulates a present request from display
	 ************************************************************/
	if (final_set->num_hits > 1) {
	  char temp_input[80];
	  
	  sprintf(result_set_name, "S%d", set_number++);
	  
	  if (se_store_result_set(final_set, result_set_name, filename,
				  "/tmp", 1) 
	      == FAIL) {
	    
	    set_number--;
	  }
	  
	  printf("\nThere are %d hits for set %s:  query = '%s' . . .\n",
		 final_set->num_hits, final_set->setid, querystring);
	  printf("\nHow many would you like to see?\n\n> ");
	  
	  gets(temp_input);
	  num_request = atoi(temp_input);
	  if (num_request > final_set->num_hits)
	    num_request = final_set->num_hits;
	}
	else {
	  
	  if ((num_request = final_set->num_hits) == 0) {
	    int code;
	    if (code = diagnostic_get()) {
	      printf("Diagnostic code %d : %s : Addinfo = '%s'\n",
		     code,diagnostic_string(code), diagnostic_addinfo());
	      diagnostic_clear();
	    }
	    printf("\nThere are no hits for query = '%s' \n",
		   querystring);
	    FREE(final_set);
	  }
	  else {
	    sprintf(result_set_name, "S%d", set_number++);
	    if (se_store_result_set(final_set, result_set_name, filename,
				    "/tmp", 1) 
		== FAIL) {
	      set_number--;
	    }
	    printf("\nThere are %d hits for set %s:  query = '%s' . . .\n",
		   final_set->num_hits, final_set->setid, querystring);
	  }
	}
	
	/* the following is just for testing -- the current result set */
        /* is available in final_set                                   */
	filetype = cf_getfiletype(final_set->filename);
	getfile = se_get_result_filetag(final_set->setid);

	for(i=0;i<num_request;i++) {
	  SGML_Document *doc, *GetSGML(), *sgml_parse_document();
	  SGML_Data *curdata, *GetData();
	  extern SGML_DTD *current_sgml_dtd;
	  int found;
	  char *p, *marcrec, *sgml_to_marc();


	  doc = GetSGML(getfile, final_set->entries[i].record_num);
	  
	  printf("rank value %f\n",final_set->entries[i].weight);

#ifdef TESTFORMATS
	  printf("*********** RAW SGML DOC **********\n%s\n\n",doc->bufer);
#endif

	  if (filetype == FILE_TYPE_MARCFILE
	      || filetype == FILE_TYPE_SGML_DATASTORE
	      || filetype == FILE_TYPE_XML
	      || filetype == FILE_TYPE_XML_NODTD
	      || filetype == FILE_TYPE_XML_DATASTORE
	      || filetype == FILE_TYPE_MARC_DATASTORE
	      || filetype == FILE_TYPE_SGMLFILE) { 
	    int result_recsize;
	    char *buf = NULL;


#ifdef TESTFORMATS
	    /* got the full doc from the file and parsed it ... now */
	    /* trim it according to the specs... (TEST ONLY)        */
	    if (doc) {	  

	      if ((result_recsize = displayrec(doc,
					       final_set->entries[i].record_num,
					       &buf, 70000,
					      "B", getfile, 0,0, 0.0)) == -1) {
		printf("record exceeds maxrecsize in displayrec\n");
	      }
	      else if (result_recsize == -2) {
		printf("invalid element set name for DB\n");
	      }
	      else if (result_recsize == -3) {
		printf("Nothing to include in specified format\n");
	      }
	      else if (result_recsize == 0) {
		printf("other error in display\n");
	      }
	      else {
		/* must be OK - just dump the buffer */
		printf("*********** TRIMMED SGML DOC **********\n%s\n\n",buf);

	      }
	    }
	    /* free the original doc and create a new one from the buffer */
	    free_doc(doc);

	    doc = sgml_parse_document(current_sgml_dtd, getfile,
				      buf, final_set->entries[i].record_num, 0);
#endif

	    if (doc) {
	      marcrec = sgml_to_marc(doc);
	  
	      DispMARCbuff(marcrec, marcformat, 
			   (int) (i+1), 78L, output_routine);
	      printf("\n");
	      FREE(marcrec);
	    }
	  }
	  else if (filetype == FILE_TYPE_CLUSTER
		   || filetype == FILE_TYPE_CLUSTER_DATASTORE ) {
	    int result_recsize;
	    char *buf = NULL;

	    if (doc) {	  
printf("Parsing cluster record\n");
	      if ((result_recsize = displayrec(doc, final_set->entries[i].record_num,
					       &buf, 70000,
					      "B", getfile, 0,0, 0.0)) == -1) {
		printf("record exceeds maxrecsize in displayrec\n");
	      }
	      else if (result_recsize == -2) {
		printf("invalid element set name for DB\n");
	      }
	      else if (result_recsize == -3) {
		printf("Nothing to include in specified format\n");
	      }
	      else if (result_recsize == 0) {
		printf("other error in display\n");
	      }
	      else {
		/* must be OK - just dump the buffer */
		printf("%s\n",buf);
	      }
	    }
	  }
	  /* free up the documents, should be finished with them */
	  free_doc(doc);
	  FREE(buf)
	}
      }
      else {
	int code;
	if (code = diagnostic_get()) {
	  printf("Diagnostic code %d : %s : Addinfo = '%s'\n",
		 code,diagnostic_string(code), diagnostic_addinfo());
	  diagnostic_clear();
	}
	else
	  printf("Null final set -- no diagnostic set.\n");
      }
  
      /* free the results set */
      /*    if (final_set)
	    se_free_result_name(result_set_name);
	    */
    }
    /* free all the doc_list memory */
    if (PRS_first_doc)
      free_doc_list(PRS_first_doc);
    
    PRS_first_doc = NULL;
    
    printf("Query-> ");
    /* reset file type to original filename */
    filetype = cf_getfiletype(filename);
    gets(querystring);
    
  }    
  /* clean up everything */
  
  /* free all the stored set info and remove the files */
  se_delete_all_result_sets();
  
  /* free all the DTDs */
  free_all_dtds();
  
  cf_closeall();
  
}


/* This function can be used to give the current "id" for the current user */
/* this is actually the connection file descriptor number in this version  */
/* when called with a non-NULL session pointer, it stores the file desc.   */
/* when called with a NULL session pointer it returns the current file desc*/
/* or zero if no file descriptor has been set                              */
int
current_user_id(ZSESSION *session)
{
  static int current_user_id_number=0;

  if (session == NULL)
    return (current_user_id_number);
  else
    current_user_id_number = session->fd;
  return(current_user_id_number);
}


