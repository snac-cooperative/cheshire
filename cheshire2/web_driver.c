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
*       Header Name:    se_web_driver.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive search engine as a cgi script
*                      
*       Usage:          webcheshire searchkey
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/1/96
*       Revised:        
*       Version:        1.0
*       Copyright (c) 1996.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>
#include <limits.h>
#endif

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#define MAIN
#include "cheshire.h"
#include "session.h"
#include "z_parse.h"
#include "bitmaps.h"
#include "dispmarc.h"
#include "dmalloc.h"

#define STATMAIN
#include "search_stat.h"

#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifdef POSTGRESQL
#include "libpq-fe.h"
#endif

#ifdef MYSQLDBMS
/* MySQL includes go here */
#include <mysql.h>
#endif

/* normally in configfiles.h */
#define BITMAPPED_TYPE (1<<29) 

SGML_DTD *main_dtd = NULL;

int last_rec_proc; /* used in indexing and parsing code */
int web_cheshire_first_pass = 1;
char *last_config = NULL;

Tcl_HashTable *current_page_hash_table;

/**** function prototypes ****/
extern weighted_result *se_process_query(char *, RPNQuery *);
extern weighted_result *se_pagemerge (int num_items, weighted_result *inset1,
                                weighted_result *inset2, Tcl_HashTable **ht);
extern weighted_result *se_andmerge (weighted_result *inset1,
			      weighted_result *inset2);
extern weighted_result *se_rank_docs(weighted_result *inset);
extern weighted_result *se_comp_minmax(int type, weighted_result *inset);
extern weighted_result *se_get_page_docs(weighted_result *final_set,
				  Tcl_HashTable **current_page_hash_table,
				  int flag);
extern weighted_result *se_regex_scan(weighted_result *in_set, char *regex[], 
			      int nregex);
extern int se_count_comp_docs(weighted_result *inset);
extern char *se_get_result_filetag(char *);
extern int cf_open_datastore(config_file_info *cf);
extern RPNQuery *queryparse(char *query, char *attributeSetId);

extern component_data_item *get_component_record(int component_id, 
						 component_list_entry *comp);

extern SGML_Data *get_component_data(int component_id,  
				     component_list_entry *comp, 
				     SGML_Document **doc);

extern char *sgml_to_marc(SGML_Document *sgmlrec);
extern char *se_dbms_query(char *database_name, RPNQuery *RPN_Query);


extern weighted_result *se_bitmap_to_result(weighted_result *inres, 
					    int n_wanted, int n_start);

extern SGML_Document *GetSGML(char *filename, int recordnum);

extern int displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
		      char *format, char *oid, char *filename, char *add_tags,
		      char **outptr, int rank, int relevance, 
		      float raw_relevance, weighted_result *resultset);


unsigned char *UTF8toISO8859(unsigned char *instring);
unsigned char *ISO8859toUTF8(unsigned char *instring);

extern int displayComponent(SGML_Document *parent_doc, SGML_Data *data, 
			    int docid, long maxlen, 
			    char *format, char **oid, char *filename, 
			    component_list_entry *comp,
			    char **outptr, int rank, int relevance, 
			    float raw_relevance);


extern int displaystring(char *doc, int docid, char **buf, int maxlen, 
			 char *format, char *oid, char *filename,
			 int rank, int relevance, float raw_relevance);

extern char *se_get_all_result_set_names();

extern config_file_info *find_file_config_quiet(char *filename);

extern int SortResults(int *result_set_status, ZSESSION *session, 
		       SortRequest *sortRequest, int *result_set_size);

extern Explain_Record *ExplainRecords[];

extern char *LastResultSetID; /* set in the command parser */

extern void free_doc(SGML_Document *doc );
extern void FreeGenericRecord(GenericRecord *data);

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);

extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);

extern float se_min_max_norm(weighted_result *set, int item);


/*****************************/

int 
Cheshire_CloseUp(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  extern SGML_Doclist  *PRS_first_doc;

  /* free all the outstanding docs  */
  free_doc_list(PRS_first_doc);

  /* free all the DTDs  */
  free_all_dtds();
  
  cf_closeall();

  return TCL_OK;


}



int
Cheshire_Search(dummy, interp, argc, argv)
     ClientData dummy;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  RPNQuery *RPN_Query_ptr;
  Query QueryWrapper;
  int i;		  /* loop counter */
  ZSESSION  Session;
  ExplainStrings explaindata;
  char *cf_name;	  /* name of configuration file */
  config_file_info *current_cf;
  unsigned char querystring[50000];	/* string to hold query */
  char *filename;
  char *logfile;
  char *attributeSetId;
  char *num_req_str, *num_start_str, *force_pagedoc, *tempcomp;
  int num_request, num_start, num_end;
  int filetype;
  int set_number = 1;
  int recnum;
  int result_rec_num;
  int result_component_num;
  char *result, *se_process_meta();
  char *current_result_set_id;
  int diagnostic;
  char *diagnostic_string(), *diagnostic_addinfo();
  char resultswork[10000];
  char *buffer;
  char *filter_pattern;
  int resultlen;
  weighted_result *final_set;	/* result from query processing */
  weighted_result *virtual_result, *work_set; /* virtual working sets */
  weighted_result *se_get_result_set();
  weighted_result *page_final_result; /* forced pagedoc results */
  char *element_set_name;
  char *recsyntax;
  char *in_syntax;
  int element_set_spec;
  int recsyntax_spec;
  int stored_result_set;
  int relevance_score;
  int rank;
  int compdataonly=0;
  char *encoding;
  char *login=NULL, *pwd=NULL;
#ifndef WIN32
  struct rlimit limits;
#endif

  extern SGML_Document *current_sgml_document;
  OtherInformation *addsearchinfo = NULL;
  char *addsearchstr = NULL;

  SGML_Document *compdoc=NULL;
  SGML_Data *subdoc=NULL;

  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *page;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;

  FILE *dummy_file;
  int BADFLAG=0;
  stored_result_set = 0;

  LOGFILE = stderr;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " indexname1 search_string1 <boolop> <indexname2>",
		     " <search_string2> <boolop2> etc...\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  
  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
			    TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }
  
  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
			     TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }
  
  
  if ((num_req_str = Tcl_GetVar(interp, "CHESHIRE_NUMREQUESTED",
				TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not set ", (char *) NULL);
    return TCL_ERROR;
  }
  
  if (Tcl_GetInt(interp, num_req_str, &num_request) == TCL_ERROR) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not a number ", 
		     (char *) NULL);
    return TCL_ERROR;
  }	 
  
  force_pagedoc = Tcl_GetVar(interp, "CHESHIRE_RETURN_PAGEDOCS",
			     TCL_GLOBAL_ONLY );
  
  
  if ((num_start_str = Tcl_GetVar(interp, "CHESHIRE_NUM_START",
				  TCL_GLOBAL_ONLY )) == NULL) {
    num_start = 1;
  }
  else {
    if (Tcl_GetInt(interp, num_start_str, &num_start) == TCL_ERROR) {
      Tcl_AppendResult(interp, "CHESHIRE_NUM_START not a number ", 
		       (char *) NULL);
      return TCL_ERROR;
    }	 
  }
  
  
  if ((encoding = Tcl_GetVar(interp, "CHESHIRE_ENCODING",
			     TCL_GLOBAL_ONLY )) == NULL) {
    encoding = "NONE";
  } 

  /* printf("IN C ENCODING IS %s\n", encoding); */

  /* get any result-limiting regular expressions */
  filter_pattern = Tcl_GetVar(interp, "CHESHIRE_REGEX_FILTER",
			      TCL_GLOBAL_ONLY);
  
  if (filter_pattern != NULL && *filter_pattern == '\0') {
    /* ignore empty patterns */
    filter_pattern = NULL;
  }

  if ((tempcomp = Tcl_GetVar(interp, "CHESHIRE_COMPONENT_DATA_ONLY",
			     TCL_GLOBAL_ONLY )) != NULL) {
    compdataonly = 1;
  } 


  if (Tcl_GetVar(interp, "CHESHIRE_SEARCH_STAT_DUMP",
		 TCL_GLOBAL_ONLY) == NULL) {
    statflag = 0;
    Query_Stats = NULL;
  }
  else {
    /* the variable was set -- so collect stats */
    /* NOTE: stat collection is only accurate for queries that access a */
    /* single index for prob, cori, or okapi searching. Queries that */
    /* merge multiple sub-results WILL NOT have correct data, but at most */
    /* the data for the last processed single indexes */
    statflag = 1;
    /* new query, so if there are old stats, clear them */
    if (Query_Stats != NULL) {
      FREE (Query_Stats);
      Query_Stats = NULL;
    }
  }
  
  if ((element_set_name = Tcl_GetVar(interp, "CHESHIRE_ELEMENTSET",
				     TCL_GLOBAL_ONLY )) == NULL) {
    element_set_name = "F"; /* default to full */
    element_set_spec = 0;
  }
  else
    element_set_spec = 1;
  
  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
			    TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
      chmod(logfile, _S_IREAD | _S_IWRITE);
#else
    chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif
    
  }

  if ((addsearchstr = Tcl_GetVar(interp, "CHESHIRE_ADDSEARCHINFO_OID",
				 TCL_GLOBAL_ONLY )) != NULL) {
    addsearchinfo = CALLOC(OtherInformation, 1);
    addsearchinfo->item.information.which = e24_oid;
    addsearchinfo->item.information.u.oid = NewOID(addsearchstr);
  }
  
  if ((addsearchstr = Tcl_GetVar(interp, "CHESHIRE_ADDSEARCHINFO_CHAR",
				 TCL_GLOBAL_ONLY )) != NULL) {
    addsearchinfo = CALLOC(OtherInformation, 1);
    addsearchinfo->item.information.which = e24_characterInfo;
    addsearchinfo->item.information.u.characterInfo = 
      NewInternationalString(addsearchstr);
  }
  
  if ((in_syntax = Tcl_GetVar(interp, "CHESHIRE_RECSYNTAX",
			      TCL_GLOBAL_ONLY )) == NULL) {
    recsyntax = NULL; /* default */
    in_syntax = "XML";
    recsyntax_spec = 0;
  }
  else {
    recsyntax_spec = 1;    
    if (strcasecmp("MARC",in_syntax) == 0 
        || strcasecmp(MARCRECSYNTAX,in_syntax) == 0
	|| strcasecmp("USMARC",in_syntax) == 0)
      recsyntax = MARCRECSYNTAX;
    else if ((strcasecmp("GRS1",in_syntax) == 0)
	     || strcasecmp("GRS-1",in_syntax) == 0
	     || strcasecmp(GRS1RECSYNTAX,in_syntax) == 0
	     || (strcasecmp("GENERIC",in_syntax) == 0))
      recsyntax = GRS1RECSYNTAX;
    else if ((strcasecmp("SUTR",in_syntax) == 0)
	     || (strcasecmp("SUTRS",in_syntax) == 0)
	     || (strcasecmp(SUTRECSYNTAX,in_syntax) == 0))
      recsyntax = SUTRECSYNTAX;
    else if ((strcasecmp(SGML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("SGML",in_syntax) == 0))
      recsyntax = SGML_RECSYNTAX;
    else if ((strcasecmp(XML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("XML",in_syntax) == 0))
      recsyntax = XML_RECSYNTAX;
    else if (strcasecmp("OPAC",in_syntax) == 0
	     || strcasecmp(OPACRECSYNTAX,in_syntax) == 0)
      recsyntax = OPACRECSYNTAX;
    else if (strcasecmp("EXPLAIN",in_syntax) == 0
	     || strcasecmp(EXPLAINRECSYNTAX,in_syntax) == 0)
      recsyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp("SUMMARY",in_syntax) == 0
	     || strcasecmp(SUMMARYRECSYNTAX,in_syntax) == 0)
      recsyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp("GRS0",in_syntax) == 0        
	     || strcasecmp(GRS0RECSYNTAX,in_syntax) == 0)
      recsyntax = GRS0RECSYNTAX;
    else if ((strcasecmp("ES",in_syntax) == 0)
	     || strcasecmp(ESRECSYNTAX,in_syntax) == 0
	     || (strcasecmp("ESRECS",in_syntax) == 0))
      recsyntax = ESRECSYNTAX;
    else {
      Tcl_AppendResult(interp, "CHESHIRE_RECSYNTAX is not known syntax", (char *) NULL);
      return TCL_ERROR;
    }      

  }
  if ((attributeSetId = Tcl_GetVar(interp, "CHESHIRE_ATTRIBUTESET",
					  TCL_GLOBAL_ONLY )) == NULL) {
    attributeSetId = OID_BIB1; /* default to BIB-1 */
  }
  else { /* an attribute set -- but is it OK? */
    if (strcasecmp("BIB1",attributeSetId) == 0 
	|| strcasecmp("BIB-1",attributeSetId) == 0
	|| strcasecmp(OID_BIB1,attributeSetId) == 0)
      attributeSetId = OID_BIB1;
    else if ((strcasecmp("EXPLAIN1",attributeSetId) == 0)
	     || (strcasecmp("EXPLAIN",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXP1",attributeSetId) == 0))
      attributeSetId = OID_EXP1;
    else if ((strcasecmp("EXTENDED",attributeSetId) == 0)
	     || (strcasecmp("EXT",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXT1",attributeSetId) == 0))
      attributeSetId = OID_EXT1;
    else if ((strcasecmp("COMMAND",attributeSetId) == 0)
	     || (strcasecmp("COMMON",attributeSetId) == 0)
	     || (strcasecmp("CCL1",attributeSetId) == 0)
	     || (strcasecmp(OID_CCL1,attributeSetId) == 0)
	     || (strcasecmp("CCL",attributeSetId) == 0))
      attributeSetId = OID_CCL1;
    else if ((strcasecmp("GILS",attributeSetId) == 0)
	     || (strcasecmp("GOVERNMENT",attributeSetId) == 0)
	     || (strcasecmp(OID_GILS,attributeSetId) == 0)
	     || (strcasecmp("GILS1",attributeSetId) == 0))
      attributeSetId = OID_GILS;
    else if ((strcasecmp("GEO",attributeSetId) == 0)
	     || (strcasecmp("GEO_PROFILE_ATTR",attributeSetId) == 0)
	     || (strcasecmp(OID_GEO,attributeSetId) == 0))
      attributeSetId = OID_GEO;
    else if ((strcasecmp("STAS",attributeSetId) == 0)
	     || (strcasecmp("SCIENTIFIC",attributeSetId) == 0)
	     || (strcasecmp(OID_STAS,attributeSetId) == 0)
	     || (strcasecmp("STAS1",attributeSetId) == 0))
      attributeSetId = OID_STAS;
    else  /* default to BIB-1 if we can't recognize it */
      attributeSetId = OID_BIB1;
  }

  if ((login = Tcl_GetVar(interp, "CHESHIRE_DATABASE_LOGIN",
					  TCL_GLOBAL_ONLY )) == NULL) {
    login = NULL;
  }

  if ((pwd = Tcl_GetVar(interp, "CHESHIRE_DATABASE_PWD",
					  TCL_GLOBAL_ONLY )) == NULL) {
    pwd = NULL;
  }


  /* we will count records from 1 */
  if (num_start == 0) num_start = 1;
  
  
  /* if we got this far all the required global stuff is in place */
  /* so we can begin to process the query                         */
  if (web_cheshire_first_pass) {
    char *crflags, *opflags;
#ifdef WIN32
    crflags = "rb";
    opflags = "rb";
#else
    crflags = "r";
    opflags = "r";
#endif
    
    diagnostic_clear();
    
    cf_info_base = (config_file_info *) cf_initialize(cf_name, 
						      crflags, 
						      opflags);
    current_cf = find_file_config_quiet(filename);

    if (current_cf == NULL) {
      if (strcasecmp(filename, "METADATA") != 0
	  && strcasecmp(filename, "IR-Explain-1") != 0) {

	Tcl_AppendResult(interp, "Cannot find the CHESHIRE_DATABASE name set in CHESHIRE_CONFIGFILE: ", cf_name, (char *) NULL);
	return TCL_ERROR;
      }
    }
    /* to fully initialize things we need to open a particular database */
    if (strcasecmp(filename, "METADATA") != 0
	&& strcasecmp(filename, "IR-Explain-1") != 0
	&& current_cf->file_type != FILE_TYPE_VIRTUALDB
	&& current_cf->file_type != FILE_TYPE_DBMS
	&& current_cf->file_type < 99 /* not datastore */)
      dummy_file = cf_open(filename, MAINFILE);		
    else if (current_cf && current_cf->file_type > 99) {
      /* cf_open_datastore(current_cf); done later */
    }
    else if (current_cf == NULL) {
      if (cf_info_base && strcasecmp(filename, "IR-Explain-1") == 0) {
	current_cf = cf_info_base; /* just in case */
      }
    }
    
    
    web_cheshire_first_pass = 0;
    
    
    last_config = strdup(cf_name);
    
  } 
  else {

    current_cf = find_file_config_quiet(filename);
    
    if (last_config && strcasecmp(cf_name,last_config) != 0) {
      /* we have to process another config file... */
      char *crflags, *opflags;
#ifdef WIN32
      crflags = "rb";
      opflags = "rb";
#else
      crflags = "r";
      opflags = "r";
#endif
      
      if (current_cf == NULL) {
	diagnostic_clear();
	
	/* to permit new DTD_parsing */
	current_sgml_document = NULL;
	
	cf_info_base = (config_file_info *) cf_initialize(cf_name, 
							crflags, opflags);

	current_cf = find_file_config_quiet(filename);

	/* to fully initialize things we need to open a particular database */
	if (strcasecmp(filename, "METADATA") != 0
	    && strcasecmp(filename, "IR-Explain-1") != 0
	    && current_cf->file_type != FILE_TYPE_VIRTUALDB
	    && current_cf->file_type < 99 /* not datastore */)
	  dummy_file = cf_open(filename, MAINFILE);		
	else if (current_cf->file_type > 99) {
	  /* cf_open_datastore(current_cf); done later */
	}
	
      }
    }
    else if (last_config == NULL) {
      last_config = strdup(cf_name);
    }
  }
  
  /* check for errors in config file processing */
  if (cf_info_base == NULL || current_cf == NULL) {
      if (current_cf == NULL 
	  && strcasecmp(filename, "METADATA") != 0
	  && strcasecmp(filename, "IR-Explain-1") != 0) {

	Tcl_AppendResult(interp, "Error processing CHESHIRE_CONFIGFILE '", 
			 cf_name, "'", (char *) NULL);
	return TCL_ERROR;
      }
  }
  
  /* assemble all the parts of the query */

  querystring[0] = '\0';
  for (i=1; i<argc; i++) {

    /* printf("arg %d %s\n", i, argv[i]); */

    strcat(querystring,argv[i]);
    strcat(querystring," ");
  }

  if (strncasecmp(argv[0], "SQL", 3) == 0 
      || strncasecmp(argv[0], "LSQL", 3) == 0) {
    /* query string is an sql command for local db */
    RPN_Query_ptr = NULL;
  }
  else {

#ifdef DUMPDEBUG
    FILE *dumpfile;
    int d, dumplen;

    dumpfile = fopen("/tmp/cheshire_dump_debug_file_webdriver", "w");

    fprintf(dumpfile, "QUERYSTRING BEFORECONV= '%s'\n", querystring);
  
    dumplen = strlen(querystring);
    fprintf(dumpfile, "QUERYSTRING HEX= ", querystring);  
    for (d = 0; d < dumplen; d++) {
      fprintf(dumpfile, "%x ", querystring[d]);  
    }
    fprintf(dumpfile, "\n", querystring);  
  
#endif

    if (strcasecmp(encoding, "ISO8859") == 0
        || strcasecmp(encoding, "ISO8859-1") == 0
        || strcasecmp(encoding, "ISO-8859-1") == 0) {
      unsigned char *tmp;
      tmp = (unsigned char *)strdup(querystring);
      strcpy(querystring,UTF8toISO8859(tmp));

      /* printf("%s", "converting to ISO8859-1\n"); */

    } 

#ifdef DUMPDEBUG

  fprintf(dumpfile, "QUERYSTRING AFTERCONV= '%s'\n", querystring);

  dumplen = strlen(querystring);
  fprintf(dumpfile, "QUERYSTRING HEX= ", querystring);  
  for (d = 0; d < dumplen; d++) {
    fprintf(dumpfile, "%x ", querystring[d]);  
  }
  fprintf(dumpfile, "\n", querystring);  

  fclose(dumpfile);
  
#endif

  
    RPN_Query_ptr = queryparse(querystring, attributeSetId);
    clean_up_query_parse();  
    current_result_set_id = LastResultSetID; /* set in parser by strdup */
  }  
  
  if (strcasecmp(filename, "METADATA") == 0) {
    
    Tcl_AppendElement( interp, "Metadata Query");
    result = se_process_meta(RPN_Query_ptr);
    if (result == NULL) {
      int code;
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork);	
      }
    }
    else {
      /* must have it -- so print it */
      
      resultlen = strlen(result);
      if (resultlen == 0)
	Tcl_AppendElement( interp,"No result-- must not be a clustered file");
      else
	Tcl_AppendElement( interp, result);
    }
  }
  else if (strcasecmp(filename,"IR-EXPLAIN-1") == 0) {
    char *outptr;
    /* process an explain request */
    /* create a "session" */
    memset(&Session, 0, sizeof(ZSESSION));
    memset(&explaindata, 0, sizeof(ExplainStrings));
    Session.s_lfptr = LOGFILE;
    Session.s_explainstrings = &explaindata;
    Session.s_DBConfigFileNames = ac_list_alloc();
    ac_list_add(Session.s_DBConfigFileNames, cf_name);
    
    Session.s_implementationName = "Webcheshire";
    
    explaindata.targetinfo_news = "No news";
    Session.namedResultSet = Z_FALSE;
    Session.multipleDatabasesSearch = Z_FALSE;
    Session.maximumNumberResultSets = 1;
    Session.s_timeOut = 9600;
    Session.s_serverMachineName = "Hostname Unknown";
    Session.s_portNumber = 210;
    Session.s_serverIP = "IP UNKNOWN";
    strcpy(Session.resultSetName,"EXPLAINRESULTS");
    strcpy(Session.databaseName,filename);
    
    explaindata.targetinfo_welcome_msg = "Welcome";
    explaindata.targetinfo_contact_name = "Name";
    explaindata.targetinfo_contact_description = "Placeholder contact";
    explaindata.targetinfo_contact_address = "Address";
    explaindata.targetinfo_contact_email = "Email";
    explaindata.targetinfo_contact_phone = "Phone";
    explaindata.targetinfo_description = "Description";
    explaindata.targetinfo_usage_restriction = "No known restrictions";
    explaindata.targetinfo_payment_address = "";
    explaindata.targetinfo_hours = "Not Known";
    explaindata.targetinfo_languages = ac_list_alloc();
    ac_list_add(explaindata.targetinfo_languages,"eng");
    
    
    QueryWrapper.which = e2_type_1;
    QueryWrapper.u.type_1 = RPN_Query_ptr;
    
    /* actually do the explain search */
    diagnostic = ExplainSearch(&Session, &QueryWrapper);
    if (diagnostic != 0) {
      sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
	      diagnostic,diagnostic_string(diagnostic), diagnostic_addinfo());
      diagnostic_clear();
      Tcl_AppendElement( interp, resultswork );
      /* even though it fails we return OK */
      return(TCL_OK);
    }
    /* No errors in query processing, so we format the explain data */

    final_set = se_get_result_set(Session.resultSetName);
    
    if (final_set == NULL) {
      sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
	      diagnostic_get(),diagnostic_string(diagnostic), diagnostic_addinfo());
      diagnostic_clear();
      Tcl_AppendElement( interp, resultswork );
      /* even though it fails we return OK */
      return(TCL_OK);
    }
    
    filename = se_get_result_filetag(Session.resultSetName);
  
    if (strcasecmp(filename, "IR-EXPLAIN-1") != 0) {
      sprintf(resultswork, "Result set %s not from Explain Search", 
	      Session.resultSetName);
      diagnostic_set(128,0,resultswork);
      sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
	      diagnostic_get(),diagnostic_string(diagnostic), diagnostic_addinfo());
      diagnostic_clear();
      Tcl_AppendElement( interp, resultswork );
      /* even though it fails we return OK */
      return(TCL_OK);
    }

    Session.presentStatus = 0;
    Session.noRecordsReturned = final_set->num_hits;
    Session.startPosition = 1;
    Session.nextResultSetPosition = 0;

    sprintf(resultswork, 
	    "OK {Status %d} {Received %d} {Position %d} {Set %s} {NextPosition %d} {Recordsyntax EXPLAIN 1.2.840.10003.5.100}",
	    Session.presentStatus, Session.noRecordsReturned, 
	    Session.startPosition, 
	    Session.resultSetName,
	    Session.nextResultSetPosition);  
    
    Tcl_AppendElement(interp, resultswork);

    for (recnum = 0; recnum < final_set->num_hits; recnum++) {
    
      result_rec_num = final_set->entries[recnum].record_num;
      
      
      MakeExplainListElement(ExplainRecords[result_rec_num],interp);
    }
    return(TCL_OK);
			    
  }
  else  {
    if (current_cf->file_type == FILE_TYPE_VIRTUALDB) {
      /* This is a virtual database, so we process the search for each
       * of the actual databases and combine or summarize results
       */
      config_file_info *virt_conf, *config;
      filelist *fl;
      char tempresultname[MAX_RSN_LENGTH];
      char virtualfilename[500];
      int i, total_hits;
      int dbcount, db, res_count;
      


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

      virt_conf = current_cf;
      total_hits = 0;
      dbcount = 0;
      strcpy(virtualfilename, filename);
      
      for (fl = virt_conf->othernames; fl; fl = fl->next_filename) {
	dbcount++;
	
	final_set = se_process_query(fl->filename, RPN_Query_ptr);
	
	/*fprintf(LOGFILE,"Processed query for database %s \n",
	 *      session->databaseName);
	 */

	if (final_set != NULL && (final_set->result_type & VIRTUAL_RESULT)) {
	  break;
	}
	
	if (final_set != NULL) {
	  if (current_result_set_id == NULL) {
	    /* store even 0 hit resultsets, for later access */
	    sprintf(tempresultname, "Default_%s", fl->filename);
	  }
	  else {
	    /* store even 0 hit resultsets, for later access */
	    sprintf(tempresultname, "%s_%s", current_result_set_id, fl->filename);
	  }
	  
#ifndef WIN32
	  se_store_result_set(final_set, tempresultname, filename, 
			      "/tmp", 5);
#else
	  se_store_result_set(final_set, tempresultname, filename, 
			      "c:\\temp", 5);
#endif
	  
	  total_hits += final_set->num_hits;
	}  
	else { /* NULL from search */ 
	  
	}
	cf_close_files(fl->filename);
      }

      if (final_set != NULL && (final_set->result_type & VIRTUAL_RESULT)) {
	/* do nothing -- it was a set merge query and is finished */
	virtual_result = final_set;
      }
      else {
	/* now we store combined resultset for the virtual DB */
	virtual_result = (weighted_result *) 
	  CALLOC (char, (sizeof(weighted_result) + 
			 ((total_hits+1) * sizeof(weighted_entry))));
	
	strcpy(virtual_result->filename, virtualfilename);
	virtual_result->doc_hits = dbcount;
	virtual_result->num_hits = total_hits;	  
	db = 0;
	res_count = 0;
	
	for (fl = virt_conf->othernames; fl; fl = fl->next_filename) {
	  db++;
	  if (current_result_set_id == NULL) {
	    sprintf(tempresultname, "Default_%s", fl->filename);
	  }
	  else {
	    sprintf(tempresultname, "%s_%s", current_result_set_id, fl->filename);
	  }
	  work_set = se_get_result_set(tempresultname);
	
	  if (work_set == NULL) {
	    continue;
	  }
	  /* type will be combined for all resultsets */
	  virtual_result->result_type |= work_set->result_type | VIRTUAL_RESULT;
	  if (work_set->result_type & BITMAP_RESULT) {
	    work_set = se_bitmap_to_result(work_set, num_request, 1);
	    work_set->num_hits = num_request;
	  }

	  for (i = 0; i < work_set->num_hits; i++) {
	    float tempweight;
	    virtual_result->entries[res_count].record_num = i;
	    virtual_result->entries[res_count].xtra = work_set;
	    /* We are now doing min-max norming of the component weights */
	    /* OLD version... virtual_result->entries[res_count++].weight =
	       work_set->entries[i].weight; */
	    tempweight = se_min_max_norm(work_set,i);
	    if (tempweight < 0.0) 
	      tempweight = 0;
	    virtual_result->entries[res_count++].weight = tempweight;
	    if (virtual_result->max_weight == 0.0) 
	      virtual_result->max_weight = tempweight;
	    else if (virtual_result->max_weight < tempweight)
	      virtual_result->max_weight = tempweight;
	    if (virtual_result->min_weight == 0.0) 
	      virtual_result->min_weight = tempweight;
	    else if (virtual_result->min_weight > tempweight)
	      virtual_result->min_weight = tempweight;

	  }
	} 
	/* have all of the results in the new list -- sort by weight */
	
	if (virtual_result->result_type & PROBABILISTIC_RESULT
	    || virtual_result->result_type & OKAPI_RESULT
	    || virtual_result->result_type & TFIDF_RESULT
	    || virtual_result->result_type & CORI_RESULT
	    || virtual_result->result_type & BLOB_RESULT
	    || virtual_result->result_type & GEORANK_RESULT
	    || virtual_result->result_type & FUZZY_RESULT) {
	  se_rank_docs(virtual_result);
	}
	/* now display however many were asked for */
	final_set = virtual_result;
      }
    }  
    else if (current_cf->file_type == FILE_TYPE_DBMS) {
      char *sql;
      /* this is a query of an external relational DBMS */
      if (RPN_Query_ptr != NULL)
	sql = se_dbms_query(filename, RPN_Query_ptr);
      else
	sql = querystring;

      if (sql == NULL) {
	Tcl_AppendResult(interp, "Unable to convert SQL Query", (char *) NULL);
	return TCL_ERROR;
      } 
      else {
        if (strchr(sql,'/') == NULL)
	  return (DBMS_local_search(interp, current_cf, recsyntax, 
				    sql, login, pwd));
      }
      /* If we haven't returned yet, it means that this is query with */
      /* RPN and slashes in the SQL -- indicating a dbms_index query  */
      final_set = se_process_query(filename, RPN_Query_ptr);

    }
    else {
      /* Most normal processing ends up here -- regular files or datastore */
      /* diagnostic_clear(); */
      if (cf_info_base->file_type > 99) {
	cf_open_datastore(cf_info_base);
      }
      
      /* actually process the query and retrieve the results */
      final_set = se_process_query(filename, RPN_Query_ptr);
    }
    FreeRPNQuery(RPN_Query_ptr);
  }
  if (final_set != NULL) {
    
    if (LastResultSetID) {
#ifndef WIN32
      se_store_result_set(final_set, LastResultSetID, filename, 
			  "/tmp", 5);
#else
      se_store_result_set(final_set, LastResultSetID, filename, 
			  "c:\\temp", 5);
#endif
      stored_result_set = 1;
      
    }
    
    if (final_set->num_hits == 0) { /* No hits */
      int code;

      if (final_set->result_type & COMPONENT_RESULT)
	sprintf( resultswork, "{ComponentHits 0 Docs 0} {Returning 0} {Start 0} {Resultset %s} {RecordSyntax UNKNOWN}", (LastResultSetID == NULL ? "NONE" : LastResultSetID));
      else
	sprintf( resultswork, "{Hits 0} {Returning 0} {Start 0} {Resultset %s} {RecordSyntax UNKNOWN}", (LastResultSetID == NULL ? "NONE" : LastResultSetID));
      
      Tcl_AppendElement( interp, resultswork );
      code = diagnostic_get();
      if (code > 0 && code != 30) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork );
      }
      Tcl_AppendElement( interp, "No matching records found" );
      if (!LastResultSetID)  
	FREE(final_set);

      return TCL_OK;
    }
      
    /* One or more hits */
    if (final_set->result_type & BITMAP_RESULT) {
      final_set = se_bitmap_to_result(final_set, num_request, num_start-1);
      num_start = 1;
    }


    if (filter_pattern) {
      char *regex[1];
      regex[0] = filter_pattern; 
      /* replace the final_set with a new one. NOTE: THE ORIGINAL IS FREED */
      final_set = se_regex_scan(final_set, regex, 1);
    }
			 

    if ((num_request + (num_start-1)) > final_set->num_hits)
      num_end = final_set->num_hits;
    else
      num_end = (num_start-1) + num_request;
    
    if ((num_request = num_end - (num_start-1)) == 0)
      num_request = 1;
    
    if (num_request < 0) {
      num_start = 1;
      num_request = 0;
    }
    
    if ((final_set->result_type & PAGED_RESULT) 
	&& (force_pagedoc == NULL)) {
      
      sprintf(resultswork, "{PAGEHits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
	      final_set->num_hits, num_request, num_start,
	      (LastResultSetID == NULL ? "NONE" : LastResultSetID),
	      in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
      Tcl_AppendElement(interp, resultswork);
      
      
      for(i=(num_start-1);i<num_end; i++) {
	char *pseudo_doc, *se_pseudo_doc();
	pseudo_doc = se_pseudo_doc(i, final_set);
	Tcl_AppendElement(interp, pseudo_doc);
	if (pseudo_doc) FREE(pseudo_doc);
      }
    } else if (final_set->result_type & PAGED_RESULT ) {
      
      page_final_result = se_get_page_docs(final_set, 
					   &current_page_hash_table,
					   1);
      FREE(final_set);
      final_set = se_rank_docs(page_final_result);
      page_final_result->result_type |= PAGED_DOC_RESULT;
    }
    
    if (final_set->result_type & PAGED_DOC_RESULT) {
      
      sprintf(resultswork, "{PageDocHits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
	      final_set->num_hits, num_request, num_start,
	      (LastResultSetID == NULL ? "NONE" : LastResultSetID),
	      in_syntax,  (recsyntax == NULL ? XML_RECSYNTAX : recsyntax));   
      Tcl_AppendElement(interp, resultswork);
      
      for(i=(num_start-1);i<num_end;i++) {
	char *doc, *GetRawSGML();
	
	recnum =  final_set->entries[i].record_num;
	
	doc = GetRawSGML(filename, recnum);
	
	/* Add the final rank and the raw document to the results */
	if (doc) {
	  buffer = CALLOC(char, strlen(doc)+100);
	  
	  sprintf (buffer, "%f {%s} {", final_set->entries[i].weight, 
		   doc);
	  
	  /* get the page hash info for final_set1 */
	  entry = Tcl_FindHashEntry(current_page_hash_table, 
				    (void *)recnum);    
	  if (entry == NULL) {
	    /* there are no page hits for this doc */
	    strcat(buffer, "}");
	  }
	  else {
	    doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
	    buffer = REALLOC(buffer, char, strlen(buffer) + 10000); /* overkill?*/
	    /* we won't include weights for this type of search */
	    for (page = doc_wts->page_ptr; page != NULL; page = page->next) {
	      sprintf(resultswork, "%d ", page->pagenum);
	      strcat(buffer, resultswork);
	    }
	    buffer[strlen(buffer)-1] = '}'; 
	  }
	}
	/* free up the document, should be finished with it */
	FREE(doc);
	
	/* add to the return set */
	Tcl_AppendElement(interp, buffer);
	FREE(buffer);
      }
      
      
    }	else if ((final_set->result_type & COMPONENT_RESULT)
		 || final_set->result_type & HYBRID_RESULT ) {
      weighted_result *temp_set;
      int minmax;
      char *minmaxstr;
      
      if ((minmaxstr = Tcl_GetVar(interp, "CHESHIRE_COMP_MINMAX",
				  TCL_GLOBAL_ONLY )) == NULL) {
	minmax = 0;
      }
      else {
	  /* there is a min_max expression */
	minmax = 0;
	
	if (strcasecmp(minmaxstr, "NO") == 0) {
	  minmax = 0;
	}
	if (strcasecmp(minmaxstr, "NONE") == 0) {
	  minmax = 0;
	}
	if (strcasecmp(minmaxstr, "MAX") == 0) {
	  minmax = 2;
	}
	if (strcasecmp(minmaxstr, "MIN") == 0) {
	  minmax = 1;
	}
	if (strcasecmp(minmaxstr, "TOPLEVEL") == 0) {
	  minmax = 2;
	}
	if (strcasecmp(minmaxstr, "LOWESTLEVEL") == 0) {
	  minmax = 1;
	}
	if (strcasecmp(minmaxstr, "LOWLEVEL") == 0) {
	  minmax = 1;
	}
	if (strcasecmp(minmaxstr, "BOTTOMLEVEL") == 0) {
	  minmax = 1;
	}
	if (strcasecmp(minmaxstr, "2") == 0) {
	  minmax = 2;
	}
	if (strcasecmp(minmaxstr, "1") == 0) {
	  minmax = 1;
	}
	if (strcasecmp(minmaxstr, "0") == 0) {
	  minmax = 0;
	}
	if (minmax > 0) {
	  temp_set = se_comp_minmax(minmax, final_set);
	  if (stored_result_set) {
	    temp_set->setid[0] = '\0';
#ifndef WIN32
	    se_store_result_set(temp_set, LastResultSetID, filename, 
				"/tmp", 5);
#else
	    se_store_result_set(temp_set, LastResultSetID, filename, 
				"c:\\temp", 5);
#endif
	  } 
	  else 
	    FREE(final_set);
	  final_set = temp_set;
	}
      }
      /* return components -- not full documents */
      sprintf(resultswork, "{ComponentHits %d Docs %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
	      final_set->num_hits, se_count_comp_docs(final_set), 
	      num_request, num_start,
	      (LastResultSetID == NULL ? "NONE" : LastResultSetID),
	      in_syntax,  (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
      Tcl_AppendElement(interp, resultswork);
      
      filetype = cf_getfiletype(filename);
      
      for(i=(num_start-1);i<num_end;i++) {
	int record_num;
	char *buf;
	weighted_result *tempset;
	struct component_list_entry *component;
  

	if ((final_set->result_type & HYBRID_RESULT) == HYBRID_RESULT) {
	  tempset = (weighted_result *)final_set->entries[i].xtra;
	  component = ((weighted_result *)final_set->entries[i].xtra)->component;
	}
	else { 
	  tempset = final_set;
	  component = final_set->component;
	}
	
	buf = NULL;
	
	result_component_num = final_set->entries[i].record_num;
	
	rank = i+1;
	/* the relevance score is scaled to the best in the set (1 for boolean)*/
	if (final_set->result_type & PROBABILISTIC_RESULT		    
	    || final_set->result_type & OKAPI_RESULT
	    || final_set->result_type & TFIDF_RESULT
	    || final_set->result_type & CORI_RESULT
	    || final_set->result_type & BLOB_RESULT
	    || final_set->result_type & GEORANK_RESULT
	    || final_set->result_type & FUZZY_RESULT) 
	  relevance_score = 
	    (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
	else
	  relevance_score = 1000;

	if (component != NULL) {

	  if (compdataonly == 0) {
	    subdoc = get_component_data(result_component_num,
					component, &compdoc);

	    if (subdoc) {
	      displayComponent(compdoc, subdoc, final_set->entries[i].record_num, 
			       8000000,
			       element_set_name,
			       &recsyntax,
			       filename, component, &buf, rank,
			       relevance_score,
			       final_set->entries[i].weight);
	      /* either outptr or buffer will be filled with the results */
	      /* and both are referenced by buf                          */
	      if (recsyntax != NULL && strcmp(recsyntax, GRS1RECSYNTAX) == 0) {
		/* convert for Tcl usage (client-side code) */
		GetGenericRecord(buf, interp);
		FreeGenericRecord((GenericRecord *)buf);
	    
	      } else {
	    
		Tcl_AppendElement(interp, buf);
		if (buf) FREE(buf);
	      }
	      FREE(subdoc);
	    }
	  } 
	  else { /*compdataonly is selected -- don't actually fetch records */
	    component_data_item *component_data;
	    char *realfilename = NULL;
	    char *componentname = NULL;
	    char componentid[2000];

	    
	    component_data = get_component_record(result_component_num,
					component);
	    
#ifdef WIN32
	    if ((componentname = strrchr(component->name, '\\')) != NULL) {
	      componentname++;
	    }
#else
	    if ((componentname = strrchr(component->name, '/')) != NULL) {
	      componentname++;
	    }
#endif
	    else { /* already a short name */
	      componentname = component->name;
	    }


	    realfilename = cf_getdatafilename(filename, component_data->record_id);
	    if (realfilename == NULL) {
	      realfilename = filename;
	    }

	    
	    sprintf(componentid, "<?%s:%d|PARENT-DOCID %d|FILEPATH %s|RANK %d|REL %'.6f|START %d|END %d ?>", componentname, 
		    result_component_num, component_data->record_id, realfilename, 
		    rank, final_set->entries[i].weight, component_data->start_offset,
		    component_data->end_offset);

	    Tcl_AppendElement(interp, componentid);
 
	    /* filenames from filecont databases are allocated... */
	    if (current_cf->filecont != NULL 
		&& current_cf->filecont->dbflag == 1) {
	      FREE(realfilename);
	    }
	    FREE(component_data);

	  }
	}	
	else { /* not really a component, must be hybrid... */
	  SGML_Document *doc;

	  doc = GetSGML(filename, final_set->entries[i].record_num);

	  if (doc) {
	    displayrec(doc, final_set->entries[i].record_num, 
		       &buf, 2000000,
		       element_set_name,
		       recsyntax,
		       filename, addsearchstr, &buf, rank, relevance_score,
		       final_set->entries[i].weight, final_set);
	    /* either outptr or buffer will be filled with the results */
	    /* and both are referenced by buf                          */
	  }
	  if (recsyntax != NULL && strcmp(recsyntax,GRS1RECSYNTAX) == 0) {
	    /* convert for Tcl usage (client-side code) */
	    GetGenericRecord(buf, interp);
	    FreeGenericRecord((GenericRecord *)buf);
	  } 
	  else {		  
	    Tcl_AppendElement(interp, buf);
	    if (buf) FREE(buf);
	  }
	  free_doc(doc);
	}
      }
      /* all the components are output -- check to see if stats 
       * were collected and output those...
       */
      if (statflag == 1) {
	int n;
	if (Query_Stats != NULL) {
	  
	  sprintf(resultswork,"QUERY_STAT_DUMP for %d records",
		  Query_Stats->totaldocs);
	  Tcl_AppendElement(interp, resultswork);
	  
	  for (n = 0; n < Query_Stats->totaldocs; n++) {
	    
	    sprintf(resultswork,
		    "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %s",
		    Query_Stats->docstat[n].docid,
		    Query_Stats->docstat[n].compid,
		    Query_Stats->docstat[n].doclength,
		    Query_Stats->docstat[n].querylength,
		    Query_Stats->docstat[n].num_match_terms,
		    Query_Stats->docstat[n].ndocs,
		    Query_Stats->docstat[n].dist_ndocs,
		    Query_Stats->docstat[n].min_cf,
		    Query_Stats->docstat[n].max_cf,
		    Query_Stats->docstat[n].min_tf,
		    Query_Stats->docstat[n].max_tf,
		    Query_Stats->docstat[n].sum_entries,
		    Query_Stats->docstat[n].min_entries,
		    Query_Stats->docstat[n].max_entries,
		    Query_Stats->docstat[n].X1,
		    Query_Stats->docstat[n].X2,
		    Query_Stats->docstat[n].X3,
		    Query_Stats->docstat[n].X4,
		    Query_Stats->docstat[n].X5,
		    Query_Stats->docstat[n].X6,
		    Query_Stats->docstat[n].logodds,
		    Query_Stats->docstat[n].docwt,
		    (Query_Stats->comp_name == NULL ? "NULL" : Query_Stats->comp_name));
	    Tcl_AppendElement(interp, resultswork);
	    
	  }
	}
      }

      if (!LastResultSetID)  
	FREE(final_set);

    }
    else {
      sprintf(resultswork, "{Hits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
	      final_set->num_hits, num_request, num_start,
	      (LastResultSetID == NULL ? "NONE" : LastResultSetID),
	      in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
      Tcl_AppendElement(interp, resultswork);
      
      filetype = cf_getfiletype(filename);
      
      if (filetype != FILE_TYPE_VIRTUALDB) {
	for(i=(num_start-1);i<num_end;i++) {

	  rank = i+1;

	  if (filetype == FILE_TYPE_MARCFILE 
	      || filetype == FILE_TYPE_SGMLFILE
	      || filetype == FILE_TYPE_XML
	      || filetype == FILE_TYPE_XML_NODTD
	      || filetype == FILE_TYPE_XML_DATASTORE
	      || filetype == FILE_TYPE_SGML_DATASTORE
	      || filetype == FILE_TYPE_MARC_DATASTORE
	      || filetype == FILE_TYPE_CLUSTER_DATASTORE
	      || filetype == FILE_TYPE_CLUSTER) { 
	    
	    if ((element_set_spec == 0 
		 && (recsyntax_spec == 0
		     || (recsyntax_spec == 1 
			 && (recsyntax == SGML_RECSYNTAX
			     || recsyntax == XML_RECSYNTAX))))
		|| ((element_set_spec == 1
		     && (strncasecmp(element_set_name,"STRING_SEGMENT",13) == 0)))) {
	      char *doc, *GetRawSGML();
	      
	      doc = GetRawSGML(filename, final_set->entries[i].record_num);
	      /* just append the raw document to the results */
	      if (strncasecmp(element_set_name,"STRING_SEGMENT",13) == 0) {
		char *buf;
		buf = NULL;

		if (final_set->result_type & PROBABILISTIC_RESULT
		    || final_set->result_type & OKAPI_RESULT
		    || final_set->result_type & TFIDF_RESULT
		    || final_set->result_type & CORI_RESULT
		    || final_set->result_type & BLOB_RESULT
		    || final_set->result_type & GEORANK_RESULT
		    || final_set->result_type & FUZZY_RESULT) 
		  relevance_score = 
		    (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
		else
		  relevance_score = 1000;
		
		displaystring(doc, final_set->entries[i].record_num, 
			      &buf,  2000000, element_set_name,
			      recsyntax, filename, rank, 
			      relevance_score, final_set->entries[i].weight);
		
		if (buf != NULL) {
		  Tcl_AppendElement(interp, buf);
		  if (doc) FREE(doc);
		}
	      }
	      else if (doc) {
		Tcl_AppendElement(interp, doc);
		FREE(doc);
	      }
	    }
	    else { /* There is a specified elementsetname or recsyntax */
	      SGML_Document *doc;
	      char *buf;
	      
	      
	      buf = NULL;
	      
	      doc = GetSGML(filename, final_set->entries[i].record_num);
	      rank = i+1;
	      
	      /* the relevance score is scaled to the best in the set (1 for boolean)*/
	      if (final_set->result_type & PROBABILISTIC_RESULT
		  || final_set->result_type & OKAPI_RESULT
		  || final_set->result_type & TFIDF_RESULT
		  || final_set->result_type & CORI_RESULT
		  || final_set->result_type & BLOB_RESULT
		  || final_set->result_type & GEORANK_RESULT
		  || final_set->result_type & FUZZY_RESULT)		
		relevance_score = 
		  (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
	      else
		relevance_score = 1000;
	      
	      if (doc) {
		if (recsyntax != NULL && 
		    strcmp(recsyntax, MARCRECSYNTAX) == 0) {
		  if (filetype != FILE_TYPE_MARCFILE 
		      && filetype != FILE_TYPE_MARC_DATASTORE
		      && element_set_spec == 0) {
		    Tcl_AppendResult(interp, "Not available in MARC format -- Try SUTRS or SGML", 
				     (char *) NULL);
		    return TCL_ERROR;
		  }
		  else if (element_set_spec == 1) {
		    displayrec(doc, final_set->entries[i].record_num, 
			       &buf, 2000000,
			       element_set_name,
			       recsyntax,
			       filename, addsearchstr, &buf, rank, relevance_score,
			       final_set->entries[i].weight, final_set);		    
		  }
		  else {
		    buf = sgml_to_marc(doc);
		  }
		} 
		else {
		  displayrec(doc, final_set->entries[i].record_num, 
			     &buf, 2000000,
			     element_set_name,
			     recsyntax,
			     filename, addsearchstr, &buf, rank, relevance_score,
			     final_set->entries[i].weight, final_set);
		  /* either outptr or buffer will be filled with the results */
		  /* and both are referenced by buf                          */
		}
		if (recsyntax != NULL && strcmp(recsyntax,GRS1RECSYNTAX) == 0) {
		  /* convert for Tcl usage (client-side code) */
		  GetGenericRecord(buf, interp);
		  FreeGenericRecord((GenericRecord *)buf);
		  
		} 
		else {
		  
		  Tcl_AppendElement(interp, buf);
		  if (buf) FREE(buf);
		}
		free_doc(doc);
	      }
	    }
	  }
	  else if (filetype == FILE_TYPE_DBMS) {

	    return (DBMS_local_index_display(interp, current_cf, recsyntax, 
					     final_set, login, pwd, num_request));
	    
	  }
	}
	/* All records are output, dump the stats if requested */
	if (statflag == 1) {
	  int n;
	  if (Query_Stats != NULL) {
	    
	    sprintf(resultswork,"QUERY_STAT_DUMP for %d records",
		    Query_Stats->totaldocs);
	    Tcl_AppendElement(interp, resultswork);
	    
	    for (n = 0; n < Query_Stats->totaldocs; n++) {
	      

	      sprintf(resultswork,
		      "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %s",
		      Query_Stats->docstat[n].docid,
		      Query_Stats->docstat[n].compid,
		      Query_Stats->docstat[n].doclength,
		      Query_Stats->docstat[n].querylength,
		      Query_Stats->docstat[n].num_match_terms,
		      Query_Stats->docstat[n].ndocs,
		      Query_Stats->docstat[n].dist_ndocs,
		      Query_Stats->docstat[n].min_cf,
		      Query_Stats->docstat[n].max_cf,
		      Query_Stats->docstat[n].min_tf,
		      Query_Stats->docstat[n].max_tf,
		      Query_Stats->docstat[n].sum_entries,
		      Query_Stats->docstat[n].min_entries,
		      Query_Stats->docstat[n].max_entries,
		      Query_Stats->docstat[n].X1,
		      Query_Stats->docstat[n].X2,
		      Query_Stats->docstat[n].X3,
		      Query_Stats->docstat[n].X4,
		      Query_Stats->docstat[n].X5,
		      Query_Stats->docstat[n].X6,
		      Query_Stats->docstat[n].logodds,
		      Query_Stats->docstat[n].docwt,
		      (Query_Stats->comp_name == NULL ? "NULL" : Query_Stats->comp_name));
	      Tcl_AppendElement(interp, resultswork);
	    }
	  }
	}
      }
      else { /* virtual databases need different processing */
	int rec_number;
	
	if (virtual_result->num_hits > 0) {
	  for(i=(num_start-1);i<num_end;i++) {


	    work_set = virtual_result->entries[i].xtra;
	    rec_number = virtual_result->entries[i].record_num;
	    
	    
	    filetype =  cf_getfiletype(work_set->filename);
	    
	    if (filetype == FILE_TYPE_MARCFILE 
		|| filetype == FILE_TYPE_SGMLFILE
		|| filetype == FILE_TYPE_XML
		|| filetype == FILE_TYPE_XML_NODTD
		|| filetype == FILE_TYPE_XML_DATASTORE
		|| filetype == FILE_TYPE_SGML_DATASTORE
		|| filetype == FILE_TYPE_MARC_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER) { 
	      
	      if (element_set_spec == 0 && (recsyntax_spec == 0
					    || (recsyntax_spec == 1 
						&& (recsyntax == SGML_RECSYNTAX
						    || recsyntax == XML_RECSYNTAX)))) {
		char *doc, *GetRawSGML();
		
		doc = GetRawSGML(work_set->filename, work_set->entries[rec_number].record_num);
		/* just append the raw document to the results */
		if (doc) {
		  Tcl_AppendElement(interp, doc);
		  FREE(doc);
		}
		
	      } 
	      else { /* There is a specified elementsetname or recsyntax */
		SGML_Document *doc;
		char *buf = NULL;
		
		doc = GetSGML(work_set->filename, work_set->entries[rec_number].record_num);
		rank = i+1;
		
		/* the relevance score is scaled to the best in the set (1 for boolean)*/
		if (work_set->result_type & PROBABILISTIC_RESULT
		    || work_set->result_type & OKAPI_RESULT
		    || work_set->result_type & TFIDF_RESULT
		    || work_set->result_type & CORI_RESULT
		    || work_set->result_type & BLOB_RESULT
		    || work_set->result_type & GEORANK_RESULT
		    || work_set->result_type & FUZZY_RESULT) 
		  relevance_score = 
		    (int)((virtual_result->entries[i].weight/virtual_result->entries[0].weight)*1000.00);
		else
		  relevance_score = 1000;
		
		if (doc) {
		  displayrec(doc, work_set->entries[rec_number].record_num, 
			     &buf, 2000000,
			     element_set_name,
			     recsyntax,
			     work_set->filename, addsearchstr, &buf, rank, 
			     relevance_score,
			     virtual_result->entries[i].weight, virtual_result);
		  /* either outptr or buffer will be filled with the results */
		  /* and both are referenced by buf                          */
		  if (recsyntax != NULL && strcmp(recsyntax,GRS1RECSYNTAX) == 0) {
		    /* convert for Tcl usage (client-side code) */
		    GetGenericRecord(buf, interp);
		    FreeGenericRecord((GenericRecord *)buf);
		    
		  } else {
		    
		    Tcl_AppendElement(interp, buf);
		    if (buf) FREE(buf);
		  }
		  free_doc(doc);
		}
	      }
	    }
	  }
	}
      }
    }
  }
  else { /* NULL final-set from search */
    int code;
    Tcl_AppendElement( interp, "ERROR {Hits 0} {Returning 0} {Start 0} {Resultset NONE} {RecordSyntax UNKNOWN}");
    if (code = diagnostic_get()) {
      sprintf(resultswork, 
	      "Diagnostic code %d : %s : Addinfo = '%s'",
	      code,diagnostic_string(code), diagnostic_addinfo());
      diagnostic_clear();
      Tcl_AppendElement( interp, resultswork );
    } else {
      Tcl_AppendElement( interp,
			 "Unrecoverable error in query processing");
    }
  }
  
  return TCL_OK;
  
}




int
Cheshire_Scan(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int i, j, more;		  /* loop counters */
  char *cf_name;	  /* name of configuration file */
  char querystring[5000];	/* string to hold query */
  char *filename;
  char *logfile;
  char *attributeSetId;
  char resultswork[10000];
  char *element_set_name;
  char *recsyntax;
  char *in_syntax;
  int element_set_spec;
  int recsyntax_spec;
  FILE *dummy_file;
  config_file_info *current_cf = NULL;
  idx_list_entry *idx;
  DB *index_db;
  int Result;
  char *index_name, *term;
  RPNQuery *sRPNQuery;
  Term *parsed_term;
  char *parsed_term_string;
  AttributesPlusTerm *attrterms;
  AttributeList *l;
  AttributeElement *elem; 
  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  TermInfo *ti;
  struct list_List2 *l1;
  char *local_index_name;
  DBT keyval, dataval;
  DBC *dbcursor;
  idx_result tempinfo;
  int stepsize, number_of_terms, preferred_position, db_errno;
  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];
  char *login=NULL, *pwd=NULL;
  extern SGML_Document *current_sgml_document;

  LOGFILE = stderr;



  if (argc < 6) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " indexname1 term stepsize number_of_terms preferred_position",
		       "\"", (char *) NULL);
	return TCL_ERROR;
      }



  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
			    TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }
  
  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
			     TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }
  
  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
			    TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
      chmod(logfile, _S_IREAD | _S_IWRITE);
#else
    chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif
    
  }
  
  if ((in_syntax = Tcl_GetVar(interp, "CHESHIRE_RECSYNTAX",
			      TCL_GLOBAL_ONLY )) == NULL) {
    recsyntax = NULL  ; /* default */
    in_syntax = "XML";
    recsyntax_spec = 0;
  }
  else {
    recsyntax_spec = 1;    
    if (strcasecmp("MARC",in_syntax) == 0 
        || strcasecmp(MARCRECSYNTAX,in_syntax) == 0
	|| strcasecmp("USMARC",in_syntax) == 0)
      recsyntax = MARCRECSYNTAX;
    else if ((strcasecmp("SUTR",in_syntax) == 0)
	     || (strcasecmp("SUTRS",in_syntax) == 0)
	     || (strcasecmp(SUTRECSYNTAX,in_syntax) == 0))
      recsyntax = SUTRECSYNTAX;
    else if ((strcasecmp(SGML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("SGML",in_syntax) == 0))
      recsyntax = SGML_RECSYNTAX;
    else if ((strcasecmp(XML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("XML",in_syntax) == 0))
      recsyntax = XML_RECSYNTAX;
    else if (strcasecmp("OPAC",in_syntax) == 0
	     || strcasecmp(OPACRECSYNTAX,in_syntax) == 0)
      recsyntax = OPACRECSYNTAX;
    else if (strcasecmp("EXPLAIN",in_syntax) == 0
	     || strcasecmp(EXPLAINRECSYNTAX,in_syntax) == 0)
      recsyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp("SUMMARY",in_syntax) == 0
	     || strcasecmp(SUMMARYRECSYNTAX,in_syntax) == 0)
      recsyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp("GRS0",in_syntax) == 0        
	     || strcasecmp(GRS0RECSYNTAX,in_syntax) == 0)
      recsyntax = GRS0RECSYNTAX;
    else if ((strcasecmp("GRS1",in_syntax) == 0)
	     || strcasecmp(GRS1RECSYNTAX,in_syntax) == 0
	     || (strcasecmp("GENERIC",in_syntax) == 0))
      recsyntax = GRS1RECSYNTAX;
    else if ((strcasecmp("ES",in_syntax) == 0)
	     || strcasecmp(ESRECSYNTAX,in_syntax) == 0
	     || (strcasecmp("ESRECS",in_syntax) == 0))
      recsyntax = ESRECSYNTAX;
    else {
      Tcl_AppendResult(interp, "CHESHIRE_RECSYNTAX is not known syntax", (char *) NULL);
      return TCL_ERROR;
    }      

  }
  if ((attributeSetId = Tcl_GetVar(interp, "CHESHIRE_ATTRIBUTESET",
					  TCL_GLOBAL_ONLY )) == NULL) {
    attributeSetId = OID_BIB1; /* default to BIB-1 */
  }
  else { /* an attribute set -- but is it OK? */
    if (strcasecmp("BIB1",attributeSetId) == 0 
	|| strcasecmp("BIB-1",attributeSetId) == 0
	|| strcasecmp(OID_BIB1,attributeSetId) == 0)
      attributeSetId = OID_BIB1;
    else if ((strcasecmp("EXPLAIN1",attributeSetId) == 0)
	     || (strcasecmp("EXPLAIN",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXP1",attributeSetId) == 0))
      attributeSetId = OID_EXP1;
    else if ((strcasecmp("EXTENDED",attributeSetId) == 0)
	     || (strcasecmp("EXT",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXT1",attributeSetId) == 0))
      attributeSetId = OID_EXT1;
    else if ((strcasecmp("COMMAND",attributeSetId) == 0)
	     || (strcasecmp("COMMON",attributeSetId) == 0)
	     || (strcasecmp("CCL1",attributeSetId) == 0)
	     || (strcasecmp(OID_CCL1,attributeSetId) == 0)
	     || (strcasecmp("CCL",attributeSetId) == 0))
      attributeSetId = OID_CCL1;
    else if ((strcasecmp("GILS",attributeSetId) == 0)
	     || (strcasecmp("GOVERNMENT",attributeSetId) == 0)
	     || (strcasecmp(OID_GILS,attributeSetId) == 0)
	     || (strcasecmp("GILS1",attributeSetId) == 0))
      attributeSetId = OID_GILS;
    else if ((strcasecmp("GEO",attributeSetId) == 0)
	     || (strcasecmp("GEO_PROFILE_ATTR",attributeSetId) == 0)
	     || (strcasecmp(OID_GEO,attributeSetId) == 0))
      attributeSetId = OID_GEO;
    else if ((strcasecmp("STAS",attributeSetId) == 0)
	     || (strcasecmp("SCIENTIFIC",attributeSetId) == 0)
	     || (strcasecmp(OID_STAS,attributeSetId) == 0)
	     || (strcasecmp("STAS1",attributeSetId) == 0))
      attributeSetId = OID_STAS;
    else  /* default to BIB-1 if we can't recognize it */
      attributeSetId = OID_BIB1;
  }

  if ((login = Tcl_GetVar(interp, "CHESHIRE_DATABASE_LOGIN",
					  TCL_GLOBAL_ONLY )) == NULL) {
    login = NULL;
  }

  if ((pwd = Tcl_GetVar(interp, "CHESHIRE_DATABASE_PWD",
					  TCL_GLOBAL_ONLY )) == NULL) {
    pwd = NULL;
  }


  index_name = argv[1];
  term = argv[2];
  stepsize = atoi(argv[3]);
  number_of_terms = atoi(argv[4]);
  preferred_position = atoi(argv[5]);

  if (number_of_terms == 0) {
    sprintf(resultswork, 
	    "SCAN {Status %d} {Terms %d} {StepSize %d} {Position %d}",
	    0, number_of_terms,stepsize, preferred_position); 
    Tcl_AppendElement(interp, resultswork);
    return TCL_OK;    
  }

  /* if we got this far all the required global stuff is in place */
  /* so we can begin to process the query                         */
  if (web_cheshire_first_pass) {
    char *crflags, *opflags;
#ifdef WIN32
    crflags = "rb";
    opflags = "rb";
#else
    crflags = "r";
    opflags = "r";
#endif
    
    cf_info_base = current_cf = 
      (config_file_info *) cf_initialize(cf_name, crflags, opflags);
    /* to fully initialize things we need to open a particular database */
    if (current_cf->file_type == FILE_TYPE_DBMS) {
      /* this is a relational DB... */
    }
    else if (current_cf->file_type != FILE_TYPE_VIRTUALDB
	     && current_cf->file_type < 99 )
      dummy_file = cf_open(filename, MAINFILE);
    else if (current_cf->file_type == FILE_TYPE_VIRTUALDB) {
      Tcl_AppendResult(interp, "VIRTUAL DB Scan not supported currently '", 
		       cf_name, "'", (char *) NULL);
      return TCL_ERROR;
    }
    web_cheshire_first_pass = 0;
  }
  else {
    
    current_cf = find_file_config_quiet(filename);
    
    if (last_config && strcasecmp(cf_name,last_config) != 0) {
      /* we have to process another config file... */
      char *crflags, *opflags;
#ifdef WIN32
      crflags = "rb";
      opflags = "rb";
#else
      crflags = "r";
      opflags = "r";
#endif
      
      if (current_cf == NULL) {
	diagnostic_clear();
	
	/* to permit new DTD_parsing */
	current_sgml_document = NULL;
	
	cf_info_base = (config_file_info *) cf_initialize(cf_name, 
							  crflags, opflags);
	
	current_cf = find_file_config_quiet(filename);
	
	/* to fully initialize things we need to open a particular database */
	if (strcasecmp(filename, "METADATA") != 0
	    && strcasecmp(filename, "IR-Explain-1") != 0
	    && current_cf->file_type != FILE_TYPE_VIRTUALDB
	    && current_cf->file_type < 99 /* not datastore */)
	  dummy_file = cf_open(filename, MAINFILE);		
	else if (current_cf->file_type > 99) {
	  /* cf_open_datastore(current_cf); done later */
	}
	
      }
    }
    else if (last_config == NULL) {
      last_config = strdup(cf_name);
    }
  }
  
  /* check for errors in config file processing */
  if (cf_info_base == NULL || current_cf == NULL) {
    Tcl_AppendResult(interp, "Error processing CHESHIRE_CONFIGFILE '", 
		     cf_name, "'", (char *) NULL);
    return TCL_ERROR;
  } 
  
  sprintf(querystring, "%s {%s}", index_name, term);
  
  /* call the command language parser to build an RPN query */
  sRPNQuery = queryparse(querystring, attributeSetId);
  
  /* any problems are signaled by the external value parse_errors */
  if (sRPNQuery == NULL) {
    Tcl_AppendResult(interp, "Unable to match index in LSCAN", (char *) NULL);
    return TCL_ERROR;
  }
  
  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }
  
  clean_up_query_parse();  
  
  attrterms = sRPNQuery->rpn->u.op->u.attrTerm;
  parsed_term = attrterms->term;
  parsed_term_string = GetOctetString(parsed_term->u.general);
  
  for (l = attrterms->attributes; l ; l = l->next) {
    elem = l->item;
    
    if (elem->attributeValue.which == e7_numeric) {
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = elem->attributeValue.u.numeric;
	attr_oid[elem->attributeType] = elem->attributeSet;
      } else {
	/* diagnostic unsupported attribute type... */
	Tcl_AppendResult(interp, "Unable to match attribute type in LSCAN", (char *) NULL);
	return TCL_ERROR;
      }
    }
    else {  /* complex attribute -- Must be local index name or string valued attribute type */
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = -1;
	for (l1 = elem->attributeValue.u.complex.list;
	     l1; l1 = l1->next) {
	  if (l1->item->which == e25_numeric) {
	    /* we don't handle any numeric types */
	    Tcl_AppendResult(interp, "Unable to match index in LSCAN", (char *) NULL);
	    return TCL_ERROR;
	  } 
	  else { /* string type, so set the local name... */
	    /* in principle there can be multiple items, take only last */
	    if (l1->item == NULL || l1->item->u.string == NULL) {
	      Tcl_AppendResult(interp, "Unable to match index in LSCAN", (char *) NULL);
	      return TCL_ERROR;
	      
	    }
	    attr_str[elem->attributeType] =
	      local_index_name = l1->item->u.string->data;
	  }	  
	}
      } else {
	/* diagnostic unsupported attribute type... */
	Tcl_AppendResult(interp, "Unable to match attribute type in LSCAN", (char *) NULL);
	return TCL_ERROR;
      }
    }
  }  
  
  if (attr_nums[USE_ATTR] == -1) {
    idx = cf_getidx_entry(filename, local_index_name);
  }
  else 
 idx = cf_getidx_mapattr(filename,
			 attr_nums, 
			 attr_str,
			 attr_oid);
  
      
  if (idx == NULL) {
    Tcl_AppendResult(interp, "Unable to match index in LSCAN", (char *) NULL);
    return TCL_ERROR;
  }
  
  free(sRPNQuery->rpn);
  free(sRPNQuery);
  /* have a matching index entry so start the scan process */
  /* make sure everything is open */
  if (current_cf->file_type == FILE_TYPE_DBMS) {
#if defined(POSTGRESQL)
    PGconn *conn;
    PGresult *res;
    int result;
    int nFields;
    int nTuples;
    int i, j, count;
    char SQLString[500];
    
    if (current_cf->file_ptr != NULL) 
      conn = (PGconn *)current_cf->file_ptr;
    else
      conn = (PGconn *)cf_open_DBMS(current_cf, login, pwd);

    if (conn == NULL) {
      Tcl_AppendResult(interp, "Unable to open Postgres connection", (char *) NULL);
      return TCL_ERROR;

    }
    
    /* start a transaction block */
    res = PQexec(conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      PQclear(res);
      PQfinish(conn);
      current_cf->file_ptr = NULL;

      Tcl_AppendResult(interp, "Postgres BEGIN command failed", (char *) NULL);
      return TCL_ERROR;
    }

    /*
     * should PQclear PGresult whenever it is no longer needed to avoid
     * memory leaks
     */
    PQclear(res);
    
    /*
     * fetch instances from the pg_database, the system catalog of
     * databases
     */
    
    sprintf (SQLString, "DECLARE myportal CURSOR FOR SELECT %s, count(%s) FROM %s GROUP BY %s ", 
	     idx->tag, idx->tag, idx->name, idx->tag);
    
    res = PQexec(conn, SQLString);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      PQclear(res);
      PQfinish(conn);
      current_cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "DECLARE CURSOR command failed", 
		       (char *) NULL);
      return TCL_ERROR;

    }
    PQclear(res);

    res = PQexec(conn, "FETCH ALL in myportal");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
      fprintf(LOGFILE, "FETCH ALL command didn't return tuples properly\n");
      PQclear(res);
      PQfinish(conn);
      current_cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "FETCH ALL command didn't return tuples properly", 
		       (char *) NULL);
      return TCL_ERROR;
    }



    nFields = PQnfields(res);
    nTuples = PQntuples(res);
    count = 0;
    /* for each row ...*/
    sprintf(resultswork, 
	    "SCAN {Status %d} {Terms %d} {StepSize %d} {Position %d}",
	    0, number_of_terms,stepsize, preferred_position); 
    Tcl_AppendElement(interp, resultswork);

    

    for (i = 0; i < PQntuples(res) && count < number_of_terms ; i++) {
      count++;
      sprintf(resultswork,"%s %s",PQgetvalue(res,i,0), PQgetvalue(res,i,1) );
      Tcl_AppendElement( interp, resultswork );
      i+= stepsize;
    }    

    return TCL_OK;
#elif defined(MYSQLDBMS)

#else 
    Tcl_AppendResult(interp, "RDBMS Support not available", (char *) NULL);
    return TCL_ERROR;    
#endif
  }
  if (idx->db == NULL) {
    index_db = 
      (DB *) cf_index_open(filename, idx->tag, INDEXFL);
      
    if (index_db == NULL) {
      Tcl_AppendResult(interp, "Unable to open index", (char *) NULL);
      return TCL_ERROR;
    }
  }
  else
    index_db = idx->db;
    
  /* Acquire a cursor for the database. */
  if ((db_errno = index_db->cursor(index_db, NULL, &dbcursor, 0)) != 0) {
    if (db_errno == DB_LOCK_NOTGRANTED) {
      Tcl_AppendResult(interp, "Lock not granted in DB access", (char *) NULL);
      db_errno = index_db->close(index_db, 0);
      return (TCL_ERROR);
    }
    if (db_errno == DB_LOCK_DEADLOCK) {
      Tcl_AppendResult(interp, "Deadlock in DB detected", (char *) NULL);
      db_errno = index_db->close(index_db, 0);
      return (TCL_ERROR);
    }
    
    Tcl_AppendResult(interp, "Unable to get index curson for scan", (char *) NULL);
    db_errno = index_db->close(index_db, 0);
    return TCL_ERROR;
  }
    
  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
    
  keyval.data = (void *) parsed_term_string;
  keyval.size = strlen(parsed_term_string)+1;
    
  if (keyval.size < 1)  { /* don't permit searches on NULL strings */
    Tcl_AppendResult(interp, "Null term in scan", (char *) NULL);
    return TCL_ERROR;
  }
    
  db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
  if (db_errno == DB_NOTFOUND && preferred_position <= 1) {
    Tcl_AppendResult(interp, "No matching terms found in scan (past end of index?)", (char *) NULL);
    dbcursor->c_close(dbcursor);
    return TCL_ERROR;
  }
  if (db_errno == DB_LOCK_NOTGRANTED) {
    Tcl_AppendResult(interp, "Lock not granted in DB access", (char *) NULL);
    dbcursor->c_close(dbcursor);
    return (TCL_ERROR);
  }
  if (db_errno == DB_LOCK_DEADLOCK) {
    Tcl_AppendResult(interp, "Deadlock in DB detected", (char *) NULL);
    dbcursor->c_close(dbcursor);
    return (TCL_ERROR);
  }

  more = number_of_terms;
  
  if (preferred_position > 1) {
    /* back up so that the item appears in the correct position */
    for (i = preferred_position; i > 1; i--) {
      for (j = 0; j <= stepsize; j++) { 
	db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV);
	if (db_errno == DB_NOTFOUND)
	  Result = 5;
	if (db_errno == DB_LOCK_DEADLOCK) {
	  Tcl_AppendResult(interp, "Deadlock in DB detected", (char *) NULL);
	  dbcursor->c_close(dbcursor);
	  return (TCL_ERROR);
	}
	if (db_errno == DB_LOCK_NOTGRANTED) {
	  Tcl_AppendResult(interp, "Lock not granted in DB access", (char *) NULL);
	  dbcursor->c_close(dbcursor);
	  return (TCL_ERROR);
	}
      }
    }
  }

  sprintf(resultswork, 
	  "SCAN {Status %d} {Terms %d} {StepSize %d} {Position %d}",
	  0, number_of_terms,stepsize, preferred_position); 
  Tcl_AppendElement(interp, resultswork);
  
  do {
      
    if (dataval.size < sizeof(idx_result) 
	|| dataval.size == bitmap_blocksize_bytes) {
    }
    else {
      memcpy((char *)&tempinfo, dataval.data,sizeof(idx_result));
    }
    
    if ((idx->type & BITMAPPED_TYPE && dataval.size == bitmap_blocksize_bytes)
	|| strcmp(keyval.data,">>>Global Data Values<<<") == 0) {
    }
    else {
      sprintf(resultswork,"%s %d", keyval.data, tempinfo.num_entries);
      Tcl_AppendElement( interp, resultswork );
      
      more--;
    }
    for (i = 0; i <= stepsize; i++) {
      if(db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT) != 0) {

	if (db_errno == DB_LOCK_DEADLOCK) {
	  Tcl_AppendResult(interp, "Deadlock in DB detected", (char *) NULL);
	  dbcursor->c_close(dbcursor);
	  return (TCL_ERROR);
	}
	if (db_errno == DB_LOCK_NOTGRANTED) {
	  Tcl_AppendResult(interp, "Lock not granted in DB access", (char *) NULL);
	  dbcursor->c_close(dbcursor);
	  return (TCL_ERROR);
	}
	more = 0; 
	Result = 5; /* end of file so missing some */

      }
    } 
    
  } while (more); 

  dbcursor->c_close(dbcursor);  
  return TCL_OK;
  
}


int
Cheshire_Sort(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  ZSESSION  Session; /* Dummy session to pass to Sort */
  int i, j, more;		  /* loop counters */
  char *cf_name;	  /* name of configuration file */
  char *filename;
  char *logfile;
  PDU *RequestPDU;
  struct sortSequence_List18 *sort_head, *sort_tail, *sort_keys;
  struct SortKey *curr_sortkey;
  struct SortElement *curr_sortelement;
  struct SortKeySpec *curr_sortkeyspec;
  RPNQuery *sRPNQuery;
  AttributeList *attributes;
  char temp[500];
  char *missing_value = NULL;
  char *in_result_string = NULL;
  char **in_results;
  char *out_results = NULL;
  char *sortstatusmsg, *resultsetmsg, *othermsg, *num;
  int sortstatus = 0, result_set_status = 0, result_set_size = 0;
  int diagnostic_code;
  char *diagnostic_string(), *diagnostic_addinfo();


  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " -IN_RESULTS {list_of_input_resultsetnames}",
		       " -OUT_RESULTS Output_resultsetname",
		       " [-TAG sgml/xml_tag_1 |",
		       " -ATTRIBUTE attribute_1 |",
		       " -ELEMENTSETNAME setname_1 ]",
		       " {-IGNORE_CASE -CASE_SENSITIVE -ASCENDING -DESCENDING -ASCENDING_FREQ",
		       " -DESCENDING_FREQ -MISSING_NULL -MISSING_QUIT",
		       " -MISSING_VALUE \"value\" }",
		       " ... -TAG sgml/xml_tag_2 ... -ATTR attribute_2 ...etc...\"", 
		       "\n Default is -IGNORECASE -ASCENDING -MISSING_NULL"
		       " with latest resultsetname as both input and ",
		       "outputname. May abbreviate options if unique.",
		       (char *) NULL);

	return TCL_ERROR;
      }


  if (cf_info_base == NULL) {
    Tcl_AppendResult(interp, "Must do a search with a named resultset before", 
		     " sorting is possible", (char *) NULL);
    return TCL_ERROR;
  } 

  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
					  TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
	  chmod(logfile, _S_IREAD | _S_IWRITE);
#else
      chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif

  }


  sort_head = NULL;

  for (i = 1; i < argc; i++) {
    /* skip nulls */
    if (strlen(argv[i]) == 0)
      continue;

    if (strncasecmp("-IGNORE_CASE", argv[i], strlen(argv[i])) == 0) {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->caseSensitivity = 1;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -IGNORE_CASE",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-CASE_SENSITIVE", argv[i], strlen(argv[i])) == 0) {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->caseSensitivity = 0;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -CASE_SENSITIVE",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-ASCENDING", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->sortRelation = 0;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -ASCENDING",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-DESCENDING", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->sortRelation = 1;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -DESCENDING",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-IN_RESULTS", argv[i], strlen(argv[i])) == 0) {
      in_result_string = argv[i+1];
      if (in_result_string[0] == '\0') {
	/* error zero hits -- no input resultsets */
	Tcl_AppendResult(interp, "Must specify a resultset name after",
			 " the -IN_RESULT flag", 
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-OUT_RESULTS", argv[i], strlen(argv[i])) == 0) {
      out_results = argv[i+1];
      if (out_results[0] == '\0') {
	Tcl_AppendResult(interp, "-OUT_RESULTS must be followed by a ",
			 "result set name (null string provided)",
			 (char *) NULL);
	return TCL_ERROR;
      }

    }
    else if (strncasecmp("-ASCENDING_FREQ", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->sortRelation = 3;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -ASCENDING_FREQ",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-DESCENDING_FREQ", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->sortRelation = 4;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -DESCENDING_FREQ",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-MISSING_NULL", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->missingValueAction.which = e21_nullVal;
	curr_sortkeyspec->missingValueAction.u.nullVal = (unsigned char)1;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -MISSING_NULL",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-MISSING_QUIT", argv[i], strlen(argv[i])) == 0)  {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->missingValueAction.which = e21_abort;
	curr_sortkeyspec->missingValueAction.u.abort = (unsigned char)1;
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -MISSING_NULL",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-MISSING_VALUE", argv[i], strlen(argv[i])) == 0) {
      if (curr_sortkeyspec != NULL) {
	curr_sortkeyspec->missingValueAction.which = e21_missingValueData;

	if (argv[i+1] != NULL) 
	curr_sortkeyspec->missingValueAction.u.missingValueData = 
	  NewOctetString(argv[i+1]);
	else {
	  Tcl_AppendResult(interp, "Must specify a value following",
			   " -MISSING_VALUE",
			   (char *) NULL);
	  return TCL_ERROR;
	}
      }
      else {
	Tcl_AppendResult(interp, "Must specify a -TAG, -ATTRIBUTE or -ELEMENTSETNAME",
			 " before -MISSING_NULL",
			 (char *) NULL);
	return TCL_ERROR;
      }
    }
    else if (strncasecmp("-TAG", argv[i], strlen(argv[i])) == 0) {

      curr_sortkey = CALLOC(struct SortKey, 1);
      curr_sortkey->which = e23_sortfield;
      curr_sortkey->u.sortfield = NewInternationalString(argv[i+1]);
      i++;
      
      curr_sortelement = CALLOC(struct SortElement, 1);
      curr_sortelement->which = e22_generic;
      curr_sortelement->u.generic = curr_sortkey;

      curr_sortkeyspec = CALLOC(struct SortKeySpec, 1);
      curr_sortkeyspec->caseSensitivity = 1;
      curr_sortkeyspec->missingValueAction.which = e21_nullVal;
      curr_sortkeyspec->missingValueAction.u.nullVal = (unsigned char)1;
      curr_sortkeyspec->sortElement = curr_sortelement;

      sort_keys = CALLOC(struct sortSequence_List18, 1);
      
      sort_keys->item = curr_sortkeyspec;

      if (sort_head == NULL) {
	sort_head = sort_keys;
	sort_tail = sort_keys;
      }
      else {
	sort_tail->next = sort_keys;
	sort_tail = sort_keys;
      }
    }
    else if (strncasecmp("-ATTRIBUTE", argv[i], strlen(argv[i])) == 0) {

      curr_sortkey = CALLOC(struct SortKey, 1);
      curr_sortkey->which = e23_sortAttributes;
      /* call the command language parser to build an RPN query */
      sprintf(temp,"%s xxx", argv[i+1]);
      sRPNQuery = queryparse(temp, "1.2.840.10003.3.1"); /* use BIB-1 */
      i++;
      /* any problems are signaled by the external value parse_errors */
      if (sRPNQuery == NULL) {
	Tcl_AppendResult(interp, "Unable to match attribute name",
			 (char *) NULL);
	return TCL_ERROR;
      }

      curr_sortkey->u.sortAttributes.id = 
	sRPNQuery->attributeSet;
      curr_sortkey->u.sortAttributes.list = 
	sRPNQuery->rpn->u.op->u.attrTerm->attributes;
      free(sRPNQuery->rpn->u.op->u.attrTerm);
      free(sRPNQuery->rpn->u.op);
      free(sRPNQuery->rpn);
      free(sRPNQuery);
      
      curr_sortelement = CALLOC(struct SortElement, 1);
      curr_sortelement->which = e22_generic;
      curr_sortelement->u.generic = curr_sortkey;

      curr_sortkeyspec = CALLOC(struct SortKeySpec, 1);
      curr_sortkeyspec->caseSensitivity = 1;
      curr_sortkeyspec->missingValueAction.which = e21_nullVal;
      curr_sortkeyspec->missingValueAction.u.nullVal = (unsigned char)1;
      curr_sortkeyspec->sortElement = curr_sortelement;

      sort_keys = CALLOC(struct sortSequence_List18, 1);
      
      sort_keys->item = curr_sortkeyspec;

      if (sort_head == NULL) {
	sort_head = sort_keys;
	sort_tail = sort_keys;
      }
      else {
	sort_tail->next = sort_keys;
	sort_tail = sort_keys;
      }
    }
    else if (strncasecmp("-ELEMENTSETNAME", argv[i], strlen(argv[i])) == 0) {

      curr_sortkey = CALLOC(struct SortKey, 1);
      curr_sortkey->which = e23_elementSpec;

      curr_sortkey->u.elementSpec = CALLOC(Specification, 1);
      curr_sortkey->u.elementSpec->elementSpec 
	= CALLOC(struct elementSpec, 1);
      curr_sortkey->u.elementSpec->elementSpec->which = e16_elementSetName;  
      curr_sortkey->u.elementSpec->elementSpec->u.elementSetName 
	= NewInternationalString(argv[i+1]);
      i++;

      curr_sortelement = CALLOC(struct SortElement, 1);
      curr_sortelement->which = e22_generic;
      curr_sortelement->u.generic = curr_sortkey;

      curr_sortkeyspec = CALLOC(struct SortKeySpec, 1);
      curr_sortkeyspec->caseSensitivity = 1;
      curr_sortkeyspec->missingValueAction.which = e21_nullVal;
      curr_sortkeyspec->missingValueAction.u.nullVal = (unsigned char)1;
      curr_sortkeyspec->sortElement = curr_sortelement;

      sort_keys = CALLOC(struct sortSequence_List18, 1);
      
      sort_keys->item = curr_sortkeyspec;

      if (sort_head == NULL) {
	sort_head = sort_keys;
	sort_tail = sort_keys;
      }
      else {
	sort_tail->next = sort_keys;
	sort_tail = sort_keys;
      }
    }
  }

    
  if (sort_head == NULL) {
    Tcl_AppendResult(interp, "No tags or attributes specified ", 
		     "for sorting.",
		     (char *) NULL);
  }

  if ((in_result_string == NULL || out_results == NULL) 
      && LastResultSetID == NULL) {

    Tcl_AppendResult(interp, "Must supply input and output result set names ", 
		     "for sorting or have a resultset name from the previous search",
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (in_result_string == NULL && LastResultSetID != NULL) {
    in_result_string = LastResultSetID;
  }
  if (out_results == NULL && LastResultSetID != NULL) {
    out_results = LastResultSetID;
  } 

  in_results = StringToArgv(in_result_string);

  /* Build the sort request PDU */
  RequestPDU = MakeSortRequest("SORT_REFID", in_results, out_results,
			       sort_head);

  if (RequestPDU == NULL) {
    Tcl_AppendResult(interp, "Unable to process sort request", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  /* just the bits we need for the session structure... */
  memset(&Session, 0, sizeof(ZSESSION));
  Session.s_lfptr = LOGFILE;
  Session.disable_saving_result_sets = 1;
#ifndef WIN32
  strcpy(Session.c_resultSetDir, "/tmp");
#else
  strcpy(Session.c_resultSetDir, "c:\\temp");
#endif

  /* Actually do the sort now */
  sortstatus = SortResults(&result_set_status, &Session, 
			   RequestPDU->u.sortRequest, &result_set_size);


  (void) FreePDU (RequestPDU);
  (void) FreeArgv(in_results);

  /* handle successful sort results (and diagnostics)*/  

  switch (sortstatus) {
  case 0:
    sortstatusmsg = "success";
    break;
  case 1:
    sortstatusmsg = "partial";
    break;
  case 2:
    sortstatusmsg = "failure";
    break;
  }

  switch (result_set_status) {
  case 0:
    resultsetmsg = "";
    break;
  case 1:
    resultsetmsg = "empty";
    break;
  case 2:
    resultsetmsg = "interim";
    break;
  case 3:
    resultsetmsg = "unchanged";
    break;
  case 4:
    resultsetmsg = "none";
    out_results = "NO RESULTSET";
    break;
  }



  sprintf(temp, 
	  "SORT {Status %d %s} {ResultSetStatus %d %s} {SortResultSet %s} {ResultSetSize %d}",
	  sortstatus, sortstatusmsg, result_set_status, resultsetmsg,
	  out_results, result_set_size);



  /* handle some error messages */

  diagnostic_code = diagnostic_get();

  if (diagnostic_code) {
    sprintf(temp,"%s -- %s", 
	    diagnostic_string(diagnostic_code),
	    diagnostic_addinfo(diagnostic_code));
    Tcl_AppendElement(interp, temp);
    return TCL_ERROR;

  }

  return TCL_OK;
  
}


/* TileBar_Search implements a search for two sets of  concepts/terms */
/* from a paged index                                                 */
/* these are then returned as a Tcl list in a format for presentation */
/* via the DLIB java client                                           */
int
TileBar_Search(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  RPNQuery *RPN_Query_ptr1, *RPN_Query_ptr2, *RPN_Query_ptr3;

  int i;		  /* loop counter */

  char *cf_name;	  /* name of configuration file */
  char querystring1[1000];	/* string to hold query */
  char querystring2[1000];	/* string to hold query */
  char elib_id[100];	/* string to hold query */
  char *filename;
  char *logfile;
  char *attributeSetId;
  char *num_req_str, *num_start_str;
  int num_request, num_start, num_end, recnum;
  int filetype;
  int set_number = 1;
  char *result, *se_process_meta();
  char *diagnostic_string(), *diagnostic_addinfo();
  char resultswork[500];
  char *buffer;
  int resultlen;

  FILE *dummy_file;
  int BADFLAG=0;

  /* results from query processing */
  weighted_result *concept_set1, *concept_set2; 
  weighted_result *merged_doc_set, *elib_id_set;
  
  Tcl_HashTable *mergehashtab = NULL;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage, *page;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;


  LOGFILE = stderr;

  if (argc < 4 || argc > 5) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     "index_name {concept1 terms} {concept2 terms} {elib_id}\n", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((num_req_str = Tcl_GetVar(interp, "CHESHIRE_NUMREQUESTED",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, num_req_str, &num_request) == TCL_ERROR) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not a number ", 
		     (char *) NULL);
    return TCL_ERROR;
  }	 

  if ((num_start_str = Tcl_GetVar(interp, "CHESHIRE_NUM_START",
					  TCL_GLOBAL_ONLY )) == NULL) {
    num_start = 1;
  }
  else {
    if (Tcl_GetInt(interp, num_start_str, &num_start) == TCL_ERROR) {
      Tcl_AppendResult(interp, "CHESHIRE_NUM_START not a number ", 
		       (char *) NULL);
      return TCL_ERROR;
    }	 
  }

  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
					  TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
	  chmod(logfile, _S_IREAD | _S_IWRITE);
#else
      chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif

  }

  if ((attributeSetId = Tcl_GetVar(interp, "CHESHIRE_ATTRIBUTESET",
					  TCL_GLOBAL_ONLY )) == NULL) {
    attributeSetId = OID_BIB1;
  }
  else { /* an attribute set -- but is it OK? */
    if (strcasecmp("BIB1",attributeSetId) == 0 
	|| strcasecmp("BIB-1",attributeSetId) == 0
	|| strcasecmp(OID_BIB1,attributeSetId) == 0)
      attributeSetId = OID_BIB1;
    else if ((strcasecmp("EXPLAIN1",attributeSetId) == 0)
	     || (strcasecmp("EXPLAIN",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXP1",attributeSetId) == 0))
      attributeSetId = OID_EXP1;
    else if ((strcasecmp("EXTENDED",attributeSetId) == 0)
	     || (strcasecmp("EXT",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXT1",attributeSetId) == 0))
      attributeSetId = OID_EXT1;
    else if ((strcasecmp("COMMAND",attributeSetId) == 0)
	     || (strcasecmp("COMMON",attributeSetId) == 0)
	     || (strcasecmp("CCL1",attributeSetId) == 0)
	     || (strcasecmp(OID_CCL1,attributeSetId) == 0)
	     || (strcasecmp("CCL",attributeSetId) == 0))
      attributeSetId = OID_CCL1;
    else if ((strcasecmp("GILS",attributeSetId) == 0)
	     || (strcasecmp("GOVERNMENT",attributeSetId) == 0)
	     || (strcasecmp(OID_GILS,attributeSetId) == 0)
	     || (strcasecmp("GILS1",attributeSetId) == 0))
      attributeSetId = OID_GILS;
    else if ((strcasecmp("STAS",attributeSetId) == 0)
	     || (strcasecmp("SCIENTIFIC",attributeSetId) == 0)
	     || (strcasecmp(OID_STAS,attributeSetId) == 0)
	     || (strcasecmp("STAS1",attributeSetId) == 0))
      attributeSetId = OID_STAS;
    else  /* default to BIB-1 if we can't recognize it */
      attributeSetId = OID_BIB1;
  }

  /* we will count records from 1 */
  if (num_start == 0) num_start = 1;

  /* if we got this far all the required global stuff is in place */
  /* so we can begin to process the query                         */
  if (web_cheshire_first_pass) {
    char *crflags, *opflags;
#ifdef WIN32
    crflags = "rb";
    opflags = "rb";
#else
    crflags = "r";
    opflags = "r";
#endif

    cf_info_base = (config_file_info *) cf_initialize(cf_name,
						      crflags, opflags);
    /* to fully initialize things we need to open a particular database */
    if (cf_info_base->file_type != FILE_TYPE_VIRTUALDB
	&& cf_info_base->file_type < 99 )
      dummy_file = cf_open(filename, MAINFILE);
    web_cheshire_first_pass = 0;
  }

  if (cf_info_base == NULL) {
    Tcl_AppendResult(interp, "Error processing CHESHIRE_CONFIGFILE '", 
		     cf_name, "'", (char *) NULL);
    return TCL_ERROR;
  } 

  elib_id_set = NULL;

  /* assemble all the parts of the query         */
  /* the concept strings are bracketed so that   */
  /* boolean ops are treated as stopwords        */
  querystring1[0] = '\0';
  strcat(querystring1, argv[1]); /* argv[1] should be the index to use */
  strcat(querystring1," @ {");
  strcat(querystring1, argv[2]);
  strcat(querystring1, "}");
  RPN_Query_ptr1 = queryparse(querystring1, attributeSetId);
  clean_up_query_parse();  

  querystring2[0] = '\0';
  strcat(querystring2, argv[1]); /* argv[1] should be the index to use */
  strcat(querystring2," @ {");
  strcat(querystring2, argv[3]);
  strcat(querystring2, "}");
  RPN_Query_ptr2 = queryparse(querystring2, attributeSetId);
  clean_up_query_parse();  

  if (argc == 5) {
    elib_id[0] = '\0';
    strcat(elib_id, "localnum ");
    strcat(elib_id, argv[4]);
    RPN_Query_ptr3 = queryparse(elib_id, attributeSetId);
    clean_up_query_parse();  
    
  }

  
   /* actually process the queries and retrieve the results */
  concept_set1 = se_process_query(filename, RPN_Query_ptr1);
  FreeRPNQuery(RPN_Query_ptr1);
  concept_set2 = se_process_query(filename, RPN_Query_ptr2);
  FreeRPNQuery(RPN_Query_ptr2);
  
  if (argc == 5) {
    elib_id_set = se_process_query(filename, RPN_Query_ptr3);
    FreeRPNQuery(RPN_Query_ptr3);
    if (elib_id_set == NULL || elib_id_set->num_hits == 0) {
      int code;
      sprintf( resultswork, "{TBHits 0} {Returning 0} {Start 0}");
      Tcl_AppendElement( interp, resultswork );
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork );
      }
      if (elib_id_set->num_hits == 0) {
	sprintf(resultswork, "No matching records found for Elib ID %s",
		argv[4]);
	Tcl_AppendElement( interp, resultswork);
      }
      FREE(concept_set1);
      FREE(concept_set2);
      FREE(elib_id_set);
      return TCL_ERROR;
    }
  }

  if (concept_set1 != NULL && concept_set2 != NULL) {
      
    if ((concept_set1->result_type & PAGED_RESULT) != PAGED_RESULT
        || (concept_set2->result_type & PAGED_RESULT) != PAGED_RESULT) {
      Tcl_AppendResult(interp, "Index didn't return PAGES ",
		       (char *) NULL);
      FREE(concept_set1);
      FREE(concept_set2);
      return TCL_ERROR;
    }

    if (concept_set1->num_hits == 0
	|| concept_set2->num_hits == 0) { /* No hits */
      int code;
      sprintf( resultswork, "{TBHits 0} {Returning 0} {Start 0}");
      Tcl_AppendElement( interp, resultswork );
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork );
      }
      
      if (concept_set1->num_hits == 0)
	Tcl_AppendElement( interp, "No matching records found for concept 1" );
      if (concept_set2->num_hits == 0)
	Tcl_AppendElement( interp, "No matching records found for concept 2" );
      FREE(concept_set1);
      FREE(concept_set2);
      return TCL_OK;
    }

    merged_doc_set = se_pagemerge( num_request, concept_set1, 
				   concept_set2, &mergehashtab);


    if (elib_id_set != NULL) { /* restrict to the elib id */
      merged_doc_set = se_andmerge(merged_doc_set, elib_id_set);
    }

    if (merged_doc_set == NULL) { /* No pages with both concepts */
      int code;
      sprintf( resultswork, "{TBHits 0} {Returning 0} {Start 0}");
      Tcl_AppendElement( interp, resultswork );
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork );
      }
      Tcl_AppendElement( interp, 
			"No pages containing both concepts" );
      FREE(concept_set1);
      FREE(concept_set2);
      free_page_hash_tab(mergehashtab);
      return TCL_OK;
    }
    
    if (merged_doc_set->num_hits == 0) { /* No hits */
      int code;
      sprintf( resultswork, "{TBHits 0} {Returning 0} {Start 0}");
      Tcl_AppendElement( interp, resultswork );
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork );
      }
      Tcl_AppendElement( interp, 
			"No matching records containing both concepts" );
      FREE(concept_set1);
      FREE(concept_set2);
      free_page_hash_tab(mergehashtab);
      return TCL_OK;
    }


    /* One or more hits */
    if ((num_request + (num_start-1)) > merged_doc_set->num_hits)
      num_end = merged_doc_set->num_hits;
    else
      num_end = (num_start-1) + num_request;
    
    if ((num_request = num_end - (num_start-1)) == 0)
      num_request = 1;
    
    if (num_request < 0) {
      num_start = 1;
      num_request = 0;
    }
    
    sprintf(resultswork, "{TBHits %d} {Returning %d} {Start %d}",
	    merged_doc_set->num_hits, num_request, num_start); 
    Tcl_AppendElement(interp, resultswork);
    
    for(i=(num_start-1);i<num_end;i++) {
      char *doc, *GetRawSGML();
      
      recnum =  merged_doc_set->entries[i].record_num;
      
      doc = GetRawSGML(filename, recnum);

      /* just append the raw document to the results */
      if (doc) {
	buffer = CALLOC(char, strlen(doc)+100);
	sprintf (buffer, "%f {%s} {", merged_doc_set->entries[i].weight, 
		 doc);
	
	/* get the page hash info for docs */
	entry = Tcl_FindHashEntry(mergehashtab, (void *)recnum);    
	if (entry == NULL) {
	  /* there are no page hits for this doc */
	  strcat(buffer, "} {}");
	}
	else {/* output each group of page matches */
	  buffer = REALLOC(buffer, char, strlen(buffer) + 10000); /* overkill?*/
	  doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
	  /* skip the merged list -- coded as zero */
	  for (page = doc_wts->page_ptr; page != NULL && page->pageid == 0; 
	       page = page->next) {}
	  if (page == NULL || page->pageid == 2)
	    strcat(buffer, " "); /* this handles empty pageid 1 lists */
	  for ( ; page != NULL && page->pageid == 1; page = page->next) {
	    sprintf(resultswork, "{%08d %f} ", page->pagenum,
		    concept_set1->entries[page->setindex].weight);
	    strcat(buffer, resultswork);
	  }
	  buffer[strlen(buffer)-1] = '}';
	  strcat(buffer, " {");
	  if (page == NULL)
	    strcat(buffer, "}");
	  else {
	    for ( ; page != NULL && page->pageid == 2; page = page->next) {
	      sprintf(resultswork, "{%08d %f} ", page->pagenum,
		      concept_set2->entries[page->setindex].weight);
	      strcat(buffer, resultswork);
	    }
	    buffer[strlen(buffer)-1] = '}';
	  }
	}

	Tcl_AppendElement(interp, buffer);
	/* free up the document, should be finished with it */
	FREE(doc);
	if (buffer) FREE(buffer);
      }
    }
    /* free the hash tables and result sets */
    FREE(concept_set1);
    FREE(concept_set2);
    free_page_hash_tab(mergehashtab);
    FREE(merged_doc_set);
  }
  else { /* NULL final-set from search */
    sprintf( resultswork, 
	    "{TBHits 0} {Returning 0} {Start 0} {NO SUCH INDEX?}");
    Tcl_AppendElement( interp, resultswork );
  }

  return TCL_OK;

}

int
TermFreq_Search(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  RPNQuery *RPN_Query_ptr;

  int i;		  /* loop counter */

  char *cf_name;	  /* name of configuration file */
  char querystring[1000];	/* string to hold query */
  char *filename;
  char *logfile;
  char *attributeSetId;
  char *num_req_str, *num_start_str;
  int num_request, num_start, num_end;
  int filetype;
  int set_number = 1;
  char *result, *se_process_meta();
  char *diagnostic_string(), *diagnostic_addinfo();
  char resultswork[500];
  char *buffer;
  int resultlen;
  weighted_result *final_set;	/* result from query processing */
  FILE *dummy_file;
  
  LOGFILE = stderr;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " indexname1 search_string1 <boolop> <indexname2>",
		       " <search_string2> <boolop2> etc...\"", (char *) NULL);
	return TCL_ERROR;
      }


  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }


  if ((num_req_str = Tcl_GetVar(interp, "CHESHIRE_NUMREQUESTED",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, num_req_str, &num_request) == TCL_ERROR) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not a number ", 
		     (char *) NULL);
    return TCL_ERROR;
  }	 

  if ((num_start_str = Tcl_GetVar(interp, "CHESHIRE_NUM_START",
					  TCL_GLOBAL_ONLY )) == NULL) {
    num_start = 1;
  }
  else {
    if (Tcl_GetInt(interp, num_start_str, &num_start) == TCL_ERROR) {
      Tcl_AppendResult(interp, "CHESHIRE_NUM_START not a number ", 
		       (char *) NULL);
      return TCL_ERROR;
    }	 
  }

  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
					  TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
	  chmod(logfile, _S_IREAD | _S_IWRITE);
#else
      chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif

  }

  if ((attributeSetId = Tcl_GetVar(interp, "CHESHIRE_ATTRIBUTESET",
					  TCL_GLOBAL_ONLY )) == NULL) {
    attributeSetId = OID_BIB1;
  }
  else { /* an attribute set -- but is it OK? */
    if (strcasecmp("BIB1",attributeSetId) == 0 
	|| strcasecmp("BIB-1",attributeSetId) == 0
	|| strcasecmp(OID_BIB1,attributeSetId) == 0)
      attributeSetId = OID_BIB1;
    else if ((strcasecmp("EXPLAIN1",attributeSetId) == 0)
	     || (strcasecmp("EXPLAIN",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXP1",attributeSetId) == 0))
      attributeSetId = OID_EXP1;
    else if ((strcasecmp("EXTENDED",attributeSetId) == 0)
	     || (strcasecmp("EXT",attributeSetId) == 0)
	     || (strcasecmp(OID_EXP1,attributeSetId) == 0)
	     || (strcasecmp("EXT1",attributeSetId) == 0))
      attributeSetId = OID_EXT1;
    else if ((strcasecmp("COMMAND",attributeSetId) == 0)
	     || (strcasecmp("COMMON",attributeSetId) == 0)
	     || (strcasecmp("CCL1",attributeSetId) == 0)
	     || (strcasecmp(OID_CCL1,attributeSetId) == 0)
	     || (strcasecmp("CCL",attributeSetId) == 0))
      attributeSetId = OID_CCL1;
    else if ((strcasecmp("GILS",attributeSetId) == 0)
	     || (strcasecmp("GOVERNMENT",attributeSetId) == 0)
	     || (strcasecmp(OID_GILS,attributeSetId) == 0)
	     || (strcasecmp("GILS1",attributeSetId) == 0))
      attributeSetId = OID_GILS;
    else if ((strcasecmp("STAS",attributeSetId) == 0)
	     || (strcasecmp("SCIENTIFIC",attributeSetId) == 0)
	     || (strcasecmp(OID_STAS,attributeSetId) == 0)
	     || (strcasecmp("STAS1",attributeSetId) == 0))
      attributeSetId = OID_STAS;
    else  /* default to BIB-1 if we can't recognize it */
      attributeSetId = OID_BIB1;
  }

  /* we will count records from 1 */
  if (num_start == 0) num_start = 1;

  /* if we got this far all the required global stuff is in place */
  /* so we can begin to process the query                         */
  if (web_cheshire_first_pass) {
    char *crflags, *opflags;
#ifdef WIN32
    crflags = "rb";
    opflags = "rb";
#else
    crflags = "r";
    opflags = "r";
#endif

    cf_info_base = (config_file_info *) cf_initialize(cf_name, 
						      crflags, opflags);
    /* to fully initialize things we need to open a particular database */
    if (cf_info_base->file_type != FILE_TYPE_VIRTUALDB
	&& cf_info_base->file_type < 99 )
      dummy_file = cf_open(filename, MAINFILE);
    web_cheshire_first_pass = 0;
  }

  if (cf_info_base == NULL) {
    Tcl_AppendResult(interp, "Error processing CHESHIRE_CONFIGFILE '", 
		     cf_name, "'", (char *) NULL);
    return TCL_ERROR;
  } 
  
  /* assemble all the parts of the query */
  querystring[0] = '\0';
  for (i=1; i<argc; i++) {
    strcat(querystring,argv[i]);
    strcat(querystring," ");
  }

  RPN_Query_ptr = queryparse(querystring, attributeSetId);

  clean_up_query_parse();  
  
  if (strcasecmp(filename, "METADATA") == 0) {
  
    Tcl_AppendElement( interp, "Metadata Query");
    result = se_process_meta(RPN_Query_ptr);
    if (result == NULL) {
      int code;
      if (code = diagnostic_get()) {
	sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		code,diagnostic_string(code), diagnostic_addinfo());
	diagnostic_clear();
	Tcl_AppendElement( interp, resultswork);	
      }
    }
    else {
      /* must have it -- so print it */
      
      resultlen = strlen(result);
      if (resultlen == 0)
	Tcl_AppendElement( interp,"No result-- must not be a clustered file");
      else
	Tcl_AppendElement( interp, result);
    }
  }
  else {
    /* actually process the query and retrieve the results */
    final_set = se_process_query(filename, RPN_Query_ptr);
    
    FreeRPNQuery(RPN_Query_ptr);
    
    if (final_set != NULL) {
      
      /* we won't store result sets in the web version... */
      /* if (final_set->num_hits > 1) {                   */
      /* char temp_input[80];                             */
      /*                                                  */	
      /* sprintf(result_set_name, "S%d", set_number++);   */
      /*                                                  */
      /* if (se_store_result_set(final_set, result_set_name, */
      /*                         filename, "/tmp", 1)     */
      /*    == FAIL) {                                    */
      /*                                                  */
      /*  set_number--;                                   */
      /* }                                                */
	  
      if (final_set->num_hits == 0) { /* No hits */
	int code;
	sprintf( resultswork, "{Hits 0} {Returning 0} {Start 0}");
	Tcl_AppendElement( interp, resultswork );
	if (code = diagnostic_get()) {
	  sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		  code,diagnostic_string(code), diagnostic_addinfo());
	  diagnostic_clear();
	  Tcl_AppendElement( interp, resultswork );
	}
	Tcl_AppendElement( interp, "No matching records found" );
	FREE(final_set);
	return TCL_OK;
      }

      /* One or more hits */
      if ((num_request + (num_start-1)) > final_set->num_hits)
	num_end = final_set->num_hits;
      else
	num_end = (num_start-1) + num_request;

      if ((num_request = num_end - (num_start-1)) == 0)
	num_request = 1;

      if (num_request < 0) {
	num_start = 1;
	num_request = 0;
      }

      if (final_set->result_type & PAGED_RESULT) {

	sprintf(resultswork, "{PAGEHits %d} {Returning %d} {Start %d}",
		final_set->num_hits, num_request, num_start); 
	Tcl_AppendElement(interp, resultswork);
	

	for(i=(num_start-1);i<num_end; i++) {
	  char *pseudo_doc, *se_pseudo_doc();
	  pseudo_doc = se_pseudo_doc(i, final_set);
	  Tcl_AppendElement(interp, pseudo_doc);
	  if (pseudo_doc) FREE(pseudo_doc);
	}
      }
      else {
	sprintf(resultswork, "{Hits %d} {Returning %d} {Start %d}",
		final_set->num_hits, num_request, num_start); 
	Tcl_AppendElement(interp, resultswork);
	
	for(i=(num_start-1);i<num_end;i++) {
	  char *doc, *GetRawSGML();
	
	  doc = GetRawSGML(filename, final_set->entries[i].record_num);
	  /* just append the raw document to the results */
	  if (doc) 
	    Tcl_AppendElement(interp, doc);
	}      
      }
    }
    else { /* NULL final-set from search */
      sprintf( resultswork, "{Hits 0} {Returning 0} {NO SUCH INDEX?}");
      Tcl_AppendElement( interp, resultswork );
    }
  }
  
  return TCL_OK;

}


/* this function can be used to give the current "id" for the current user */
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
/* but are also in explain.c, so they are now commented out                 */
#ifdef NEEDSTRINGORNUMERIC
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

#endif


/* the following is a dummy needed for explain in webcheshire */
int
InitializeSearchEngine(ZSESSION *session) {
  return(0);
}


int
Cheshire_Fetch_Results(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int i;		  /* loop counter */
  ZSESSION  Session;
  char *cf_name;	  /* name of configuration file */
  config_file_info *current_cf;
  char querystring[50000];	/* string to hold query */
  char *filename;
  char *logfile;
  char *num_req_str, *num_start_str, *force_pagedoc;
  int num_request, num_start, num_end;
  int filetype;
  int set_number = 1;
  int recnum;
  int result_rec_num;
  int result_component_num;
  char *result, *se_process_meta();
  int diagnostic;
  char *diagnostic_string(), *diagnostic_addinfo();
  char resultswork[10000];
  char *buffer;
  int resultlen;
  weighted_result *final_set;	/* result from query processing */
  weighted_result *virtual_result, *work_set; /* virtual working sets */
  weighted_result *se_get_result_set();
  weighted_result *page_final_result; /* forced pagedoc results */
  char *element_set_name;
  char *recsyntax;
  char *in_syntax;
  int element_set_spec;
  int recsyntax_spec;
  int relevance_score;
  int rank;
  extern SGML_Document *current_sgml_document;
  OtherInformation *addsearchinfo = NULL;
  char *addsearchstr = NULL;

  SGML_Document *compdoc=NULL;
  SGML_Data *subdoc=NULL;

  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage, *page;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;

  FILE *dummy_file;
  int BADFLAG=0;
  char *result_set_name;

  LOGFILE = stderr;

  if (argc < 1) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " result_set_name <number_of_records>"
		       " <start_position_in_resultset> \n",
		     " If number and start are not supplied then",
		     " the CHESHIRE_NUMREQUESTED and CHESHIRE_NUM_START",
		     " variables will be used.",
		       (char *) NULL);
	return TCL_ERROR;
      }

  if (argc == 4) {
    num_start_str = argv[3];
    num_req_str = argv[2];
  }
  else {
    num_start_str = NULL;
    num_req_str = NULL;
  }
  result_set_name = argv[1];


  if ((cf_name = Tcl_GetVar(interp, "CHESHIRE_CONFIGFILE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_CONFIGFILE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_DATABASE not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if (num_req_str == NULL && (num_req_str = Tcl_GetVar(interp, "CHESHIRE_NUMREQUESTED",
					  TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not set ", (char *) NULL);
    return TCL_ERROR;
  }

  if (Tcl_GetInt(interp, num_req_str, &num_request) == TCL_ERROR) {
    Tcl_AppendResult(interp, "CHESHIRE_NUMREQUESTED not a number ", 
		     (char *) NULL);
    return TCL_ERROR;
  }	 

  force_pagedoc = Tcl_GetVar(interp, "CHESHIRE_RETURN_PAGEDOCS",
			     TCL_GLOBAL_ONLY );
  

  if (num_start_str == NULL && 
      (num_start_str = Tcl_GetVar(interp, "CHESHIRE_NUM_START",
					  TCL_GLOBAL_ONLY )) == NULL) {
    num_start = 1;
  }
  else {
    if (Tcl_GetInt(interp, num_start_str, &num_start) == TCL_ERROR) {
      Tcl_AppendResult(interp, "CHESHIRE_NUM_START not a number ", 
		       (char *) NULL);
      return TCL_ERROR;
    }	 
  }


  if ((element_set_name = Tcl_GetVar(interp, "CHESHIRE_ELEMENTSET",
					  TCL_GLOBAL_ONLY )) == NULL) {
    element_set_name = "F"; /* default to full */
    element_set_spec = 0;
  }
  else
    element_set_spec = 1;

  if ((logfile = Tcl_GetVar(interp, "CHESHIRE_LOGFILE",
					  TCL_GLOBAL_ONLY )) != NULL) {
    LOGFILE = fopen(logfile, "w");
    if (LOGFILE == NULL)
      LOGFILE = stderr;
    else
#ifdef WIN32
	  chmod(logfile, _S_IREAD | _S_IWRITE);
#else
      chmod(logfile, S_IRWXO | S_IRWXG | S_IRWXU);
#endif

  }

  if ((in_syntax = Tcl_GetVar(interp, "CHESHIRE_RECSYNTAX",
					  TCL_GLOBAL_ONLY )) == NULL) {
    recsyntax = NULL ; /* default */
    in_syntax = "XML";
    recsyntax_spec = 0;
  }
  else {
    recsyntax_spec = 1;    
    if (strcasecmp("MARC",in_syntax) == 0 
        || strcasecmp(MARCRECSYNTAX,in_syntax) == 0
	|| strcasecmp("USMARC",in_syntax) == 0)
      recsyntax = MARCRECSYNTAX;
    else if ((strcasecmp("GRS1",in_syntax) == 0)
	     || strcasecmp("GRS-1",in_syntax) == 0
	     || strcasecmp(GRS1RECSYNTAX,in_syntax) == 0
	     || (strcasecmp("GENERIC",in_syntax) == 0))
      recsyntax = GRS1RECSYNTAX;
    else if ((strcasecmp("SUTR",in_syntax) == 0)
	     || (strcasecmp("SUTRS",in_syntax) == 0)
	     || (strcasecmp(SUTRECSYNTAX,in_syntax) == 0))
      recsyntax = SUTRECSYNTAX;
    else if ((strcasecmp(SGML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("SGML",in_syntax) == 0))
      recsyntax = SGML_RECSYNTAX;
    else if ((strcasecmp(XML_RECSYNTAX,in_syntax) == 0)
	     || (strcasecmp("XML",in_syntax) == 0))
      recsyntax = XML_RECSYNTAX;
    else if (strcasecmp("OPAC",in_syntax) == 0
	     || strcasecmp(OPACRECSYNTAX,in_syntax) == 0)
      recsyntax = OPACRECSYNTAX;
    else if (strcasecmp("EXPLAIN",in_syntax) == 0
	     || strcasecmp(EXPLAINRECSYNTAX,in_syntax) == 0)
      recsyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp("SUMMARY",in_syntax) == 0
	     || strcasecmp(SUMMARYRECSYNTAX,in_syntax) == 0)
      recsyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp("GRS0",in_syntax) == 0        
	     || strcasecmp(GRS0RECSYNTAX,in_syntax) == 0)
      recsyntax = GRS0RECSYNTAX;
    else if ((strcasecmp("ES",in_syntax) == 0)
	     || strcasecmp(ESRECSYNTAX,in_syntax) == 0
	     || (strcasecmp("ESRECS",in_syntax) == 0))
      recsyntax = ESRECSYNTAX;
    else {
      Tcl_AppendResult(interp, "CHESHIRE_RECSYNTAX is not known syntax", (char *) NULL);
      return TCL_ERROR;
    }      

  }

  if ((addsearchstr = Tcl_GetVar(interp, "CHESHIRE_ADDSEARCHINFO_OID",
				 TCL_GLOBAL_ONLY )) != NULL) {
    addsearchinfo = CALLOC(OtherInformation, 1);
    addsearchinfo->item.information.which = e24_oid;
    addsearchinfo->item.information.u.oid = NewOID(addsearchstr);
  }
  
  if ((addsearchstr = Tcl_GetVar(interp, "CHESHIRE_ADDSEARCHINFO_CHAR",
				 TCL_GLOBAL_ONLY )) != NULL) {
    addsearchinfo = CALLOC(OtherInformation, 1);
    addsearchinfo->item.information.which = e24_characterInfo;
    addsearchinfo->item.information.u.characterInfo = 
      NewInternationalString(addsearchstr);
  }
  


  /* we will count records from 1 */
  if (num_start == 0) num_start = 1;
  
  diagnostic_clear();
    
  if (cf_info_base == NULL) {
      Tcl_AppendResult(interp, "Fetch attempted without preceding search", (char *) NULL);
      return TCL_ERROR;
    }      

  if (strcasecmp(filename, "METADATA") == 0) {
      Tcl_AppendResult(interp, "Fetch not supported for metadata results", (char *) NULL);
      return TCL_ERROR;
  }
  else if (strcasecmp(filename,"IR-EXPLAIN-1") == 0) {
    Tcl_AppendResult(interp, "Fetch not supported for Explain results", (char *) NULL);
    return TCL_ERROR;
  }
  else {

    final_set = se_get_result_set(result_set_name);
 
    if (final_set != NULL) {
      
      if (final_set->num_hits == 0) { /* No hits */
	int code;

	if (final_set->result_type & COMPONENT_RESULT) 
	  sprintf( resultswork, "{ComponentHits 0 Docs 0} {Returning 0} {Start 0} {Resultset NONE} {RecordSyntax UNKNOWN}");
	else
	  sprintf( resultswork, "{Hits 0} {Returning 0} {Start 0} {Resultset NONE} {RecordSyntax UNKNOWN}");
	Tcl_AppendElement( interp, resultswork );
	if (code = diagnostic_get()) {
	  sprintf(resultswork, "Diagnostic code %d : %s : Addinfo = '%s'\n",
		  code,diagnostic_string(code), diagnostic_addinfo());
	  diagnostic_clear();
	  Tcl_AppendElement( interp, resultswork );
	}
	Tcl_AppendElement( interp, "No matching records found" );
	return TCL_OK;
      }

      /* One or more hits */
      if ((num_request + (num_start-1)) > final_set->num_hits)
	num_end = final_set->num_hits;
      else
	num_end = (num_start-1) + num_request;
      
      if ((num_request = num_end - (num_start-1)) == 0)
	num_request = 1;

      if (num_request < 0) {
	num_start = 1;
	num_request = 0;
      }
      
      if ((final_set->result_type & PAGED_RESULT) 
	  && (force_pagedoc == NULL)) {
	
	sprintf(resultswork, "{PAGEHits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
		final_set->num_hits, num_request, num_start,
		result_set_name, in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
	Tcl_AppendElement(interp, resultswork);
	

	for(i=(num_start-1);i<num_end; i++) {
	  char *pseudo_doc, *se_pseudo_doc();
	  pseudo_doc = se_pseudo_doc(i, final_set);
	  Tcl_AppendElement(interp, pseudo_doc);
	  if (pseudo_doc) FREE(pseudo_doc);
	}
      } else if (final_set->result_type & PAGED_RESULT ) {

	page_final_result = se_get_page_docs(final_set, 
					     &current_page_hash_table,
					     1);

	final_set = se_rank_docs(page_final_result);
	page_final_result->result_type |= PAGED_DOC_RESULT;

      } else if (final_set->result_type & PAGED_DOC_RESULT) {

	sprintf(resultswork, "{PageDocHits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
		final_set->num_hits, num_request, num_start,
		result_set_name, in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
	Tcl_AppendElement(interp, resultswork);
	
	for(i=(num_start-1);i<num_end;i++) {
	  char *doc, *GetRawSGML();

	  recnum =  final_set->entries[i].record_num;
	  
	  doc = GetRawSGML(filename, recnum);

	  /* Add the final rank and the raw document to the results */
	  if (doc) {
	    buffer = CALLOC(char, strlen(doc)+100);
	    
	    sprintf (buffer, "%f {%s} {", final_set->entries[i].weight, 
		     doc);
	    
	    /* get the page hash info for final_set1 */
	    entry = Tcl_FindHashEntry(current_page_hash_table, 
				      (void *)recnum);    
	    if (entry == NULL) {
	      /* there are no page hits for this doc */
	      strcat(buffer, "}");
	    }
	    else {
	      doc_wts = (struct docsum *) Tcl_GetHashValue(entry);
	      buffer = REALLOC(buffer, char, strlen(buffer) + 10000); /* overkill?*/
	      /* we won't include weights for this type of search */
	      for (page = doc_wts->page_ptr; page != NULL; page = page->next) {
		sprintf(resultswork, "%d ", page->pagenum);
		strcat(buffer, resultswork);
	      }
	      buffer[strlen(buffer)-1] = '}'; 
	    }
	  }
	  /* free up the document, should be finished with it */
	  FREE(doc);
	  
	  /* add to the return set */
	  Tcl_AppendElement(interp, buffer);
	  FREE(buffer);
	}

	
      }	else if (final_set->result_type & COMPONENT_RESULT) {
	/* return components -- not full documents */
	sprintf(resultswork, "{ComponentHits %d Docs %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
		final_set->num_hits, se_count_comp_docs(final_set), 
		num_request, num_start,
		result_set_name, in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
	Tcl_AppendElement(interp, resultswork);

	filetype = cf_getfiletype(filename);
	
	for(i=(num_start-1);i<num_end;i++) {
	  int record_num;
	  char *buf = NULL;
	  
	  result_component_num = final_set->entries[i].record_num;
	  
	  rank = i+1;
	  /* the relevance score is scaled to the best in the set (1 for boolean)*/
	  if (final_set->result_type & PROBABILISTIC_RESULT
	      || final_set->result_type & OKAPI_RESULT
	      || final_set->result_type & TFIDF_RESULT
	      || final_set->result_type & CORI_RESULT
	      || final_set->result_type & BLOB_RESULT
	      || final_set->result_type & GEORANK_RESULT
	      || final_set->result_type & FUZZY_RESULT) 
	    relevance_score = 
	      (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
	  else
	    relevance_score = 1000;


	  subdoc = get_component_data(result_component_num,
					    final_set->component, &compdoc);

	  if (subdoc) {
	    displayComponent(compdoc, subdoc, final_set->entries[i].record_num, 
			     8000000,
			     element_set_name,
			     &recsyntax,
			     filename, final_set->component, &buf, rank,
			     relevance_score,
			     final_set->entries[i].weight);
	    /* either outptr or buffer will be filled with the results */
	    /* and both are referenced by buf                          */
	    if (recsyntax != NULL && strcmp(recsyntax, GRS1RECSYNTAX) == 0) {
	      /* convert for Tcl usage (client-side code) */
	      GetGenericRecord(buf, interp);
	      FreeGenericRecord((GenericRecord *)buf);

	    } else {
	      
	      Tcl_AppendElement(interp, buf);
	      if (buf) FREE(buf);
	    }
	    FREE(subdoc);
	  }
	}  
      }
      else {
	sprintf(resultswork, "{Hits %d} {Returning %d} {Start %d} {Resultset %s} {RecordSyntax %s %s}",
		final_set->num_hits, num_request, num_start,
		result_set_name, in_syntax, (recsyntax == NULL ? XML_RECSYNTAX : recsyntax)); 
	Tcl_AppendElement(interp, resultswork);

	filetype = cf_getfiletype(filename);
	
	if (filetype != FILE_TYPE_VIRTUALDB) {
	  for(i=(num_start-1);i<num_end;i++) {

	    if (filetype == FILE_TYPE_MARCFILE 
		|| filetype == FILE_TYPE_SGMLFILE
		|| filetype == FILE_TYPE_XML
		|| filetype == FILE_TYPE_XML_NODTD
		|| filetype == FILE_TYPE_XML_DATASTORE
		|| filetype == FILE_TYPE_SGML_DATASTORE
		|| filetype == FILE_TYPE_MARC_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER) { 
	      
	      if (element_set_spec == 0 && (recsyntax_spec == 0
					    || (recsyntax_spec == 1 
						&& (recsyntax == SGML_RECSYNTAX
						    || recsyntax == XML_RECSYNTAX)))) {
		char *doc, *GetRawSGML();
		
		doc = GetRawSGML(filename, final_set->entries[i].record_num);
		/* just append the raw document to the results */
		if (doc) {
		  Tcl_AppendElement(interp, doc);
		  FREE(doc);
		}
	      } 
	      else { /* There is a specified elementsetname or recsyntax */
		SGML_Document *doc;
		char *buf = NULL;
		
		doc = GetSGML(filename, final_set->entries[i].record_num);
		rank = i+1;
		
		/* the relevance score is scaled to the best in the set (1 for boolean)*/
		if (final_set->result_type & PROBABILISTIC_RESULT
		    || final_set->result_type & OKAPI_RESULT
		    || final_set->result_type & TFIDF_RESULT
		    || final_set->result_type & CORI_RESULT
		    || final_set->result_type & BLOB_RESULT
		    || final_set->result_type & GEORANK_RESULT
		    || final_set->result_type & FUZZY_RESULT)
		  relevance_score = 
		    (int)((final_set->entries[i].weight/final_set->entries[0].weight)*1000.00);
		else
		  relevance_score = 1000;
		
		if (doc) {
		  displayrec(doc, final_set->entries[i].record_num, 
			     &buf, 2000000,
			     element_set_name,
			     recsyntax,
			     filename, addsearchstr, &buf, rank, relevance_score,
			     final_set->entries[i].weight, final_set);
		  /* either outptr or buffer will be filled with the results */
		  /* and both are referenced by buf                          */
		  if (recsyntax == GRS1RECSYNTAX) {
		    /* convert for Tcl usage (client-side code) */
		    GetGenericRecord(buf, interp);
		    FreeGenericRecord((GenericRecord *)buf);
		    
		  } else {
		    if (buf) {
		      Tcl_AppendElement(interp, buf);
		      FREE(buf);
		    } else {
		      sprintf(resultswork, 
			"NULL RECORD RETURNED FROM DISPLAYREC FOR DOCID %d ", 
			final_set->entries[i].record_num);
		      Tcl_AppendElement(interp, resultswork);
		    }
		  }
		  free_doc(doc);
		
		}
	      }
	    }
	  }
	}
	else { /* virtual databases need different processing */
	  int rec_number;
	
	  virtual_result = final_set;

	  for(i=(num_start-1);i<num_end;i++) {
	  
	    work_set = virtual_result->entries[i].xtra;
	    rec_number = virtual_result->entries[i].record_num;
	    
	    filetype =  cf_getfiletype(work_set->filename);
	    
	    if (filetype == FILE_TYPE_MARCFILE 
		|| filetype == FILE_TYPE_SGMLFILE
		|| filetype == FILE_TYPE_XML
		|| filetype == FILE_TYPE_XML_NODTD
		|| filetype == FILE_TYPE_XML_DATASTORE
		|| filetype == FILE_TYPE_SGML_DATASTORE
		|| filetype == FILE_TYPE_MARC_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER_DATASTORE
		|| filetype == FILE_TYPE_CLUSTER) { 
	      
	      if (element_set_spec == 0 && (recsyntax_spec == 0
					    || (recsyntax_spec == 1 
						&& (recsyntax == SGML_RECSYNTAX
						    || recsyntax == XML_RECSYNTAX)))) {
		char *doc, *GetRawSGML();
		
		doc = GetRawSGML(work_set->filename, work_set->entries[rec_number].record_num);
		/* just append the raw document to the results */
		if (doc) {
		  Tcl_AppendElement(interp, doc);
		  FREE(doc);
		}
		
	      } 
	      else { /* There is a specified elementsetname or recsyntax */
		SGML_Document *doc;
		char *buf = NULL;
		
		doc = GetSGML(work_set->filename, work_set->entries[rec_number].record_num);
		rank = i+1;
		
		/* the relevance score is scaled to the best in the set (1 for boolean)*/
		if (work_set->result_type & PROBABILISTIC_RESULT
		    || work_set->result_type & OKAPI_RESULT
		    || work_set->result_type & TFIDF_RESULT
		    || work_set->result_type & CORI_RESULT
		    || work_set->result_type & BLOB_RESULT
		    || work_set->result_type & GEORANK_RESULT
		    || work_set->result_type & FUZZY_RESULT) 
		  relevance_score = 
		    (int)((virtual_result->entries[i].weight/virtual_result->entries[0].weight)*1000.00);
		else
		  relevance_score = 1000;
		
		if (doc) {
		  displayrec(doc, work_set->entries[rec_number].record_num, 
			     &buf, 2000000,
			     element_set_name,
			     recsyntax,
			     work_set->filename, addsearchstr, &buf, rank, 
			     relevance_score, 
			     virtual_result->entries[i].weight,
			     virtual_result);
		  /* either outptr or buffer will be filled with the results */
		  /* and both are referenced by buf                          */
		  if (recsyntax != NULL && strcmp(recsyntax,GRS1RECSYNTAX) == 0) {
		    /* convert for Tcl usage (client-side code) */
		    GetGenericRecord(buf, interp);
		    FreeGenericRecord((GenericRecord *)buf);
		    
		  } else {
		    
		    Tcl_AppendElement(interp, buf);
		    if (buf) FREE(buf);
		  }
		  free_doc(doc);
		}
	      }
	    }
	  }
	} /* end of virtual files processing */
      } /* end of normal processing */
    }
    else { /* NULL final-set from get results */
      sprintf( resultswork, "{Hits 0} {Returning 0} {NO SUCH RESULTSET: %s}",
	       result_set_name);
      Tcl_AppendElement( interp, resultswork );
    }
  }

  
  return TCL_OK;
  
}



int
Cheshire_Delete_Results(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  char *result_set_name;
 
  if (argc < 1) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " <result_set_name>|ALL\n",
		     (char *) NULL);
    return TCL_ERROR;
  }

  result_set_name = argv[1];

  if (strcasecmp(result_set_name, "ALL") == 0)
    se_delete_all_result_sets();
  else
    se_delete_result_set(result_set_name);

  return TCL_OK;
  
}


int
Cheshire_Get_Result_Names(dummy, interp, argc, argv)
    ClientData dummy;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{

 
  if (argc > 1) {
    Tcl_AppendResult(interp, "wrong # args: should be ", argv[0],
		     " only\n",
		     (char *) NULL);
    return TCL_ERROR;
  }


  Tcl_AppendResult(interp, "{", se_get_all_result_set_names(), "}",
		   (char *) NULL);

  return TCL_OK;
  
}




