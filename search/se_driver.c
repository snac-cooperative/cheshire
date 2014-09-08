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

#include <stdlib.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include <cheshire.h>
#include "z_parse.h"

#include "dispsgml.h"
#include "dispmarc.h"

#define STATMAIN
#include "search_stat.h"


#define NUM_FORMAT_SLOTS 100

FORMAT_TAB FormatTable[NUM_FORMAT_SLOTS] = {
  {"FULL", "",defaultformat},
  {"LONG", "",defaultformat},
  {"TAGGED","", defaultformat},
  {"BRIEF","", shortformat},
  {"SHORT","", shortformat},
  {"MARC","", marcformat},
  {"FULLMARC","", marcformat},
  {"REVIEW","", evalformat},
  {"EVAL", "",evalformat},
  {"LIST", "",tcllistformat},
  {"TCLLIST","", tcllistformat},
  {"SGML", "",dummyformat},
  {"HTML", "",htmlformat},
  {"SHORTHTML","", shorthtmlformat},
  {"REVIEWHTML","", reviewhtmlformat},
  {"","", NULL} /* default to brief format */
};

#define LAST_BUILTIN_FORMAT 15
int last_format_slot = LAST_BUILTIN_FORMAT ;

#include <dmalloc.h>

SGML_DTD *main_dtd = NULL;


int last_rec_proc; /* used in indexing and parsing code */

/**** function prototypes ****/
extern MARC_REC *DispMARCbuff(char *recbuffer, DISP_FORMAT *format, 
		       long displaynum, long maxlen, void (*output)());

extern RPNQuery *queryparse(char *query, char *attributeSetId);

extern weighted_result *se_process_query(char *, RPNQuery *);
extern display_result  *se_retrievemarc(int, int, weighted_result *);
extern char *se_get_result_filetag(char *);


extern SGML_Data *get_component_data(int component_id,  
				     component_list_entry *comp, 
				     SGML_Document **doc);

extern int displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
		      char *format, char *oid, char *filename, char *add_tags,
		      char **outptr, int rank, int relevance, 
		      float raw_relevance, weighted_result *resultset);

/*****************************/


void output_routine(char *line) {
  printf("%s",line);
}

main (argc, argv)
int argc;
char *argv[];
{
  RPNQuery *RPN_Query_ptr;
  extern SGML_Doclist  *PRS_first_doc;

  char command[1000]; /* XXXX for simulated output from driver */
  
  int i;		  /* loop counter */
  char *cf_name;	  /* name of configuration file */
  char querystring[300];	/* string to hold query */
  char *filename;
  int num_request;
  int filetype;
  int batch = 0;
  int set_number = 1;
  int argp = 1;
  char result_set_name[50];
  char *result, *se_process_meta();
  char *diagnostic_string(), *diagnostic_addinfo();
  char buffer[70000];
  int resultlen;
  int rank;
  int relevance_score;
  char *dummy = NULL;

  weighted_result *final_set;	/* result from query processing */
  
  display_result *present_set;     /* result from se_retrievemarc */
  SGML_Document *compdoc=NULL;
  SGML_Data *subdoc=NULL;
  
  char *crflags, *opflags;
#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif

  LOGFILE = stderr;

  num_request = 0;

  if (argc < 2) { 
    fprintf(LOGFILE, 
	    "Wrong number of args should be: %s [-n num_recs_wanted] configfile filename [query_string]\n", 
	    argv[0]);
    exit (1);
  }

  if ((argc > 2 && (argv[1][0] == '-' && argv[1][1] == 'n'))) {
    batch = 1;
    argp = 3;
    num_request = atoi(argv[2]);
  }

  cf_name = argv[argp];
  cf_info_base = (config_file_info *) cf_initialize(cf_name, crflags, opflags);
  
  filename = argv[argp+1];

  if (cf_info_base == NULL) {
    fprintf(LOGFILE, 
	    "bad config file ? should be: %s configfile filename query_string\n", 
	    argv[0]);
    exit (2);
  }
  
  filetype = cf_getfiletype(filename);
  querystring[0] = '\0';

  if (argc > 3) {/* this could fail */
    for (i=(argp+2); i<argc; i++) {
      strcat(querystring,argv[i]);
      strcat(querystring," ");
    }
  }
  else {
    if (batch == 0) {
      printf("Query-> ");
      gets(querystring);
    }
    else {
      fprintf(LOGFILE, 
	      "Wrong number of args should be: %s [-n num_recs_wanted] configfile filename [query_string]\n", 
	      argv[0]);
      exit (1);
    }
      
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
      
      if (batch == 0)
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

      FreeRPNQuery(RPN_Query_ptr);

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
	  
	  if (batch == 0)
	    printf("\nThere are %d hits for set %s:  query = '%s' . . .\n",
		   final_set->num_hits, final_set->setid, querystring);
	  else
	    printf("\nThere are %d hits for set %s: %d being returned:  query = '%s' . . .\n",
		   final_set->num_hits, final_set->setid,  num_request, querystring);

	  if (batch == 0)
	    printf("\nHow many would you like to see?\n\n> ");
	  
	  if (batch == 0) {
	    gets(temp_input);
	    num_request = atoi(temp_input);
	  }
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
	    if (batch)
	      exit(0);
	  }
	  else {
	    sprintf(result_set_name, "S%d", set_number++);
	    if (se_store_result_set(final_set, result_set_name, filename, 
				    "/tmp", 1) 
		== FAIL) {
	      set_number--;
	    }
	    printf("\nThere are %d hits for set %s: %d being returned:  query = '%s' . . .\n",
		   final_set->num_hits, final_set->setid,  num_request, querystring);
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

	  if (final_set->result_type & COMPONENT_RESULT) {
	    /* return components -- not full documents */
	    char *buf;
	    subdoc = get_component_data(final_set->entries[i].record_num,
					final_set->component, &compdoc);
	    rank = i+1;
	    /* the relevance score is scaled to the best in the set (1 for boolean)*/
	    if (final_set->result_type & PROBABILISTIC_RESULT)

	      relevance_score = 
		(int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
	    else
	      relevance_score = 1000;


	      if (subdoc) {
		displayComponent(compdoc, subdoc, final_set->entries[i].record_num, 
				 70000,
				 "F",
				 &dummy,
				 filename, final_set->component, &buf, rank,
				 relevance_score,final_set->entries[i].weight);
		/* buffer will be filled with the results */
		/* and both are referenced by buf                          */
		printf("+++++++++\nCOMPONENT rank value: %f\n",final_set->entries[i].weight);
		printf("%s\n",buf);
		FREE(subdoc);
	      }
	    }  
	    else {

	      doc = GetSGML(getfile, final_set->entries[i].record_num);
	      rank = i+1;
	      /* the relevance score is scaled to the best in the set (1 for boolean)*/
	      if (final_set->result_type & PROBABILISTIC_RESULT)

		relevance_score = 
		  (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
	      else
		relevance_score = 1000;


	  
	      printf("+++++++++\nrank value: %f\n",final_set->entries[i].weight);

#ifdef TESTFORMATS
	      printf("*********** RAW SGML DOC **********\n%s\n\n",doc->buffer);
#endif

	      if (filetype == FILE_TYPE_MARCFILE 
		  || filetype == FILE_TYPE_XML
		  || filetype == FILE_TYPE_XML_NODTD
		  || filetype == FILE_TYPE_XML_DATASTORE
		  || filetype == FILE_TYPE_SGML_DATASTORE
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
					    "B", NULL, getfile, 
					    NULL, &buf, rank,
					    relevance_score,
				   final_set->entries[i].weight)) == -1) {

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
					  buf,final_set->entries[i].record_num, 0);
#endif

		if (doc) {
		  if (batch == 0) {
		    marcrec = sgml_to_marc(doc);
		    
		    if (marcrec == NULL) {
		      printf("**** RAW SGML DOC *****\n%s\n",doc->buffer);
		    }
		    else
		      DispMARCbuff(marcrec, marcformat, 
				   (long) (i+1), 78L, output_routine);
		    printf("\n");
		    if (marcrec)
		      FREE(marcrec);
		  }
		  else { /* Batch mode -- just output SGML */
		    printf("%s\n",doc->buffer);
		  }
		}
	      }
	      else if (filetype == FILE_TYPE_CLUSTER 
		       || filetype == FILE_TYPE_CLUSTER_DATASTORE) {
		int result_recsize;
		char *buf = NULL;
		
		if (doc) {	  
		  if (batch == 0)
		    printf("Parsing cluster record\n");
		  if ((result_recsize = 
		       displayrec(doc, final_set->entries[i].record_num, 
				  &buf, 70000,
				  "B", NULL, getfile, 
				  NULL, &buf, rank,
				  relevance_score,
				  final_set->entries[i].weight,
				  final_set)) == -1) {
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
		if (buf) FREE(buf);
	      }
	      /* free up the documents, should be finished with them */
	      free_doc(doc);

	    }
	}
      
      }
      else { /* final set is NULL */
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
    
    if (batch == 0)
      printf("Query-> ");
    /* reset file type to original filename */
    filetype = cf_getfiletype(filename);

    if (batch == 0) {
      gets(querystring);
    }
  
    if (batch)
      querystring[0] = (char)0;

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

/* the following are used in building some stuff during config file parsing */


StringOrNumeric *
BuildStringOrNumeric(char *string, int numeric, int which)
{
  StringOrNumeric *tmp;

  tmp = CALLOC(StringOrNumeric, 1);

  if (which == 0) {
    tmp->which = e25_string;
    tmp->u.string = NewInternationalString(string);
  }
  else {
    tmp->which = e25_numeric;
    tmp->u.numeric = numeric;
  }
  
  return (tmp);
}

HumanString *
BuildHumanString(char *text, char *language)
{
  HumanString *tmp;
  
  if (text == NULL)
    return NULL;

  tmp = CALLOC(HumanString, 1);
  tmp->item.language = (LanguageCode *)NewInternationalString(language);
  tmp->item.text = NewInternationalString(text);

  return(tmp);

}

