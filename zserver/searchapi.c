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
*       Header Name:    searchapi.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive search engine from ZSERVER
*                      
*
*       Usage:          
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
#include <stdlib.h>
#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"
#include "cheshire.h"
#include "configfiles.h"
#include "z_parse.h"
#include "bitmaps.h"
#include "dispmarc.h"
#include "session.h"
#include <dmalloc.h>

MARC_REC *DispMARCbuff(char *recbuffer, DISP_FORMAT *format, 
		       long displaynum, long maxlen, void *(output)());
RPNQuery *queryparse(char *query, char *attributeSetId);
weighted_result *se_process_query(char *, RPNQuery *);
char *se_dbms_query(char *dbname, RPNQuery *RPN_Query_ptr);
char *GetOctetString(OctetString);
char *ac_list_first(AC_LIST *l);
int ExplainPresent(char **outptr, ZSESSION *Session, 
		   int record_id, int *result_recsize);

extern int displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
		      char *format, char *oid, char *filename, char *add_tags,
		      char **outptr, int rank, int relevance, 
		      float raw_relevance, weighted_result *resultset);

extern int displaystring(char *doc, int docid, char **buf, int maxlen, 
	   char *format, char *oid, char *filename,
			 int rank, int relevance, float raw_relevance);

#define DONT_SAVE_RESULT_SETS 4

extern SGML_Data *get_component_data(int component_id,  component_list_entry *comp, 
		   SGML_Document **doc);

extern config_file_info *find_file_config(char *filename);

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);

extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);

extern weighted_result *se_bitmap_to_result(weighted_result *inres, 
					    int n_wanted, int n_start);

extern weighted_result *se_get_result_set(char *setid);
extern char *diagnostic_string(int code);
extern char *LastResultSetID; /* set in the command parser or in search */

char *metadata_buffer = NULL;

int last_rec_proc = 0; /* used in indexing and parsing code */

char *DefaultElementSetName(char *databaseName);


int 
InitializeSearchEngine(ZSESSION *session) {
  char *config_file_name;
  config_file_info *newbase;
  char *crflags, *opflags;
  int first = 1;

#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif



  newbase = NULL;

  /* initialize using the DEFAULT -- first database in the list */
  LOGFILE = session->s_lfptr; /* all error, etc. output to the same place */
  
  for (config_file_name = ac_list_first(session->s_DBConfigFileNames);
       config_file_name != NULL;
       config_file_name = ac_list_next(session->s_DBConfigFileNames)) {

    fprintf(LOGFILE,"Initializing config file: %s\n", 
	    config_file_name);

    newbase = (config_file_info *) 
      cf_initialize(config_file_name, crflags, opflags);
    
    if (first) {
      cf_info_base = newbase;
      first = 0;
    }

    if (newbase == NULL) {
      fprintf(LOGFILE,"Unable to initialize the database in config file  %s?\n", 
	      config_file_name);
    } 
    /* first file bad??? */
    if (first == 0 && cf_info_base == NULL && newbase != NULL)
      cf_info_base = newbase;
  }

  if (cf_info_base == NULL) 
    return(-1);

  return (0);
}

/*
 * clean up everything 
 */
int 
CloseSearchEngine(ZSESSION *session) {
  /* free all the stored set info and remove the files */
  se_delete_all_result_sets();

  /* free all the DTDs */
  free_all_dtds();

  cf_closeall();
}


int 
CheshireSearch (ZSESSION *session, Query *query)
{
  extern SGML_Doclist  *PRS_first_doc;
  RPNQuery *RPN_Query_ptr;
  char *se_process_meta();
  int returncode;		  /* loop counter */
  int replaceflag;
  any SQL_Query = NULL;
  weighted_result *final_set;	/* result from query processing */
  int filetype;
  
  /* remove any diagnostics from the last search */
  diagnostic_clear();
  
  current_user_id(session);    /* set the "user id" e.g. connection number */
  
  cheshire_log_file = session->s_lfptr;
  LOGFILE = session->s_lfptr; /* all error, etc. output to the same place */
  
  fprintf(LOGFILE,"Starting query processing\n");
  
  if (cf_info_base == NULL) {
    InitializeSearchEngine(session);
    
  }
  
  if (query->which == e2_type_1) {
    RPN_Query_ptr = query->u.type_1;
  }
  else if (query->which == e2_type_101) {
    RPN_Query_ptr = query->u.type_101;
  }
  else if (query->which == e2_type_100) {
    /* assume bib-1 for these */
    RPN_Query_ptr = queryparse(GetOctetString(query->u.type_100), OID_BIB1);
  }
  else if (query->which == e2_type_0) {
    SQL_Query = (char *)query->u.type_0;
  } 
  else if (query->which == e2_type_102) {
    /*RPN_Query_ptr = queryparse(GetOctetString(query->u.type_102,OID_BIB1);*/
    fprintf(LOGFILE, "ERROR: Type 102 Query not supported yet\n");
    diagnostic_set(3,0,"Type 102 Query not supported yet");
    return(diagnostic_get()); 
	    
  }

  
  fprintf(LOGFILE,"databasename: %s\n",session->databaseName);
  filetype = cf_getfiletype(session->databaseName);
  
  if (strcasecmp(session->databaseName, "METADATA") == 0) {
    char *result;
    
    
    fprintf(LOGFILE,"METADATA Query\n");

    if (metadata_buffer)
      FREE(metadata_buffer);
    
    metadata_buffer = result = se_process_meta(RPN_Query_ptr);
    
    if (result == NULL) {
      returncode = diagnostic_get();  
    }
    else {
      /* must have it -- so set a result */
      if (strlen(result) >  session->s_exceptionalRecordSize) {
	/* problem -- too big */
	diagnostic_set(17,strlen(result),NULL); /* exceeds max record size */
	returncode = diagnostic_get();  
      }
      else if (result[0] == '\0') 
	session->resultCount = 0;
      else
	session->resultCount = 1; 
      returncode = 0;
    }
  }
  else if (strcasecmp(session->databaseName, "IR-EXPLAIN-1") == 0) {
    /* this should never happen -- explain is normally trapped */
    /* and processed before the search gets here.              */
    diagnostic_set(109,0,"Explain not yet available"); 
    returncode = 109;
  }
  else if (filetype == FILE_TYPE_DBMS) {
    char *sql;
    /* this is a query of an external relational DBMS */
    if (query->which == e2_type_0) 
      sql = SQL_Query;
    else
      sql = se_dbms_query(session->databaseName, RPN_Query_ptr);
    
    if (sql == NULL) {
      diagnostic_set(108,0,"Unable to create SQL query"); 
      returncode = 108;
    } 
    else {
      fprintf(LOGFILE,"SQL EXTERNAL DB COMMAND: %s\n", sql);    
      if (strchr(sql,'/') == NULL) 
	returncode = DBMS_search(session, sql);
      else {
	final_set = se_process_query(session->databaseName, RPN_Query_ptr);
	
	fprintf(LOGFILE,"Processed query\n");
	
	if (final_set != NULL) {

	  if (diagnostic_get() == 0) {
	    /* search was OK so store things (even zero hits */
	    if (session->replaceIndicator) 
	      replaceflag = 1;
	    else
	      replaceflag = 0;
	    
	    if (session->disable_saving_result_sets)
	      replaceflag |= 4; /* no save flag */
	    
	    se_store_result_set(final_set, session->resultSetName, 
				session->databaseName, 
				session->c_resultSetDir, replaceflag);
	    
	    session->resultCount = final_set->num_hits; 
	    
	    if (LOGFILE != stderr)
	      fprintf(LOGFILE,
		      "Final Set Num_hits = %d\n",final_set->num_hits);
	    
	    returncode = 0;
	  }
	  else {/* num_hits is 0 and diagnostic */
	    if (LOGFILE != stderr)
	      fprintf(LOGFILE,
		      "Final Set Num_hits = %d\n",final_set->num_hits);
	    session->resultCount = 0;
	    returncode = diagnostic_get();
	  }
	}
	else { /* NULL from search */
	  if (LOGFILE != stderr)
	    fprintf(LOGFILE,"Final Set NULL\n");
	  returncode = diagnostic_get();
	}
      }
      return(returncode);
      
      /* return (DBMS_local_search(interp, current_cf, recsyntax, 
       *			  sql, login, pwd));
       */
    }
  }
  else if (filetype == FILE_TYPE_VIRTUALDB) {
    /* This is a virtual database, so we process the search for each
     * of the actual databases and combine or summarize results
     */
    config_file_info *virt_conf, *config;
    filelist *fl;
    char tempresultname[MAX_RSN_LENGTH];
    char virtualfilename[500];
    weighted_result *virtual_result, *work_set;
    int i, total_hits;
    int dbcount, db, res_count;

    virt_conf = find_file_config(session->databaseName);
    session->resultCount = 0; 
    total_hits = 0;
    dbcount = 0;
    strcpy(virtualfilename, session->databaseName);
    
    /* We use LastResultSetID to set resultsetnames for virtual boolean */
    LastResultSetID = session->resultSetName;

    for (fl = virt_conf->othernames; fl; fl = fl->next_filename) {
      strcpy(session->databaseName, fl->filename);

      dbcount++;

      final_set = se_process_query(session->databaseName, RPN_Query_ptr);
      
      fprintf(LOGFILE,"Processed query for database %s \n",
	      session->databaseName);

      if (final_set != NULL && (final_set->result_type & VIRTUAL_RESULT)) {
	/* all done -- was a set combination */
	dbcount = final_set->doc_hits;
	total_hits = final_set->num_hits;
	session->resultCount = final_set->num_hits; 
	break;
      }

      if (final_set != NULL) {
	/* store even 0 hit resultsets, for later access */
	sprintf(tempresultname, "%s_%s", 
		session->resultSetName, session->databaseName);
	se_store_result_set(final_set, tempresultname, 
			    session->databaseName, 
			    session->c_resultSetDir, 1);
	
	session->resultCount += final_set->num_hits; 
	total_hits += final_set->num_hits;
	
	if (LOGFILE != stderr)
	  fprintf(LOGFILE,
		  "%s Final Set Num_hits = %d\n",
		  session->databaseName,final_set->num_hits);
	
	returncode = 0;
      }  
      else { /* NULL from search */
	if (LOGFILE != stderr)
	  fprintf(LOGFILE,"Final Set NULL\n");
	diagnostic_clear();
      }

      cf_close_files(fl->filename);
    }

    if (final_set != NULL && (final_set->result_type & VIRTUAL_RESULT)) {
      /* do nothing -- the search must have been a combination of sets */
      virtual_result = final_set;
      returncode = 0;
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
      if (total_hits > 0) {
	for (fl = virt_conf->othernames; fl; fl = fl->next_filename) {
	  db++;
	  sprintf(tempresultname, "%s_%s", 
		  session->resultSetName, fl->filename);
	  work_set = se_get_result_set(tempresultname);
	  
	  if (work_set == NULL) {
	    /* may be a bad database */
	    diagnostic_clear();
	    continue;
	  }
	  /* type will be combined for all resultsets */
	  virtual_result->result_type |= work_set->result_type | VIRTUAL_RESULT;

	  for (i = 0; i < work_set->num_hits; i++) {
	    virtual_result->entries[res_count].record_num = i;
	    virtual_result->entries[res_count].xtra = work_set;
	    virtual_result->entries[res_count++].weight =
	      work_set->entries[i].weight;
	  }
	}
	/* have all of the results in the new list -- sort by weight */
      
	if (virtual_result->result_type & PROBABILISTIC_RESULT) {
	  se_rank_docs(virtual_result);
	}
      }
    }

    returncode = 0;

    se_store_result_set(virtual_result, session->resultSetName, 
			virtual_result->filename, 
			session->c_resultSetDir, 1);
    
    /* restore the original filename */
    strcpy(session->databaseName, virtualfilename);
  }
  else {
    if (strcasecmp(session->databaseName, "Default") == 0) {
      /* use the first database in the configfile */
      strcpy(session->databaseName, cf_info_base->nickname);
    } 

    final_set = se_process_query(session->databaseName, RPN_Query_ptr);
    
    fprintf(LOGFILE,"Processed query\n");
    
    if (final_set != NULL) {
#ifdef OLDRESULTHANDLING      
      if (final_set->num_hits != 0) {
#else
      if (diagnostic_get() == 0) {
#endif	
	/* search was OK so store things (even zero hits */
	if (session->replaceIndicator) 
	  replaceflag = 1;
	else
	  replaceflag = 0;
	
	if (session->disable_saving_result_sets)
	  replaceflag |= 4; /* no save flag */
	
	se_store_result_set(final_set, session->resultSetName, 
			    session->databaseName, 
			    session->c_resultSetDir, replaceflag);
	
	session->resultCount = final_set->num_hits; 
	
	if (LOGFILE != stderr)
	  fprintf(LOGFILE,
		  "Final Set Num_hits = %d\n",final_set->num_hits);
	
	returncode = 0;
      }
      else {/* num_hits is 0 and diagnostic */
	if (LOGFILE != stderr)
	  fprintf(LOGFILE,
		  "Final Set Num_hits = %d\n",final_set->num_hits);
	session->resultCount = 0;
	returncode = diagnostic_get();
      }
    }  
    else { /* NULL from search */
      if (LOGFILE != stderr)
	fprintf(LOGFILE,"Final Set NULL\n");
      returncode = diagnostic_get();
    }
  }
  return(returncode);
}

/* the currently recognized record syntax OIDs for cheshire 
 #define MARCRECSYNTAX "1.2.840.10003.5.10"
 #define EXPLAINRECSYNTAX "1.2.840.10003.5.100"
 #define SUTRECSYNTAX "1.2.840.10003.5.101"
 #define GRS1RECSYNTAX    "1.2.840.10003.5.105"
 #define SGML_RECSYNTAX "1.2.840.10003.5.109.9"
 #define XML_RECSYNTAX "1.2.840.10003.5.109.10"
 #define HTML_RECSYNTAX "1.2.840.10003.5.109.3"

 -- not yet recognized OIDs 
 #define OPACRECSYNTAX    "1.2.840.10003.5.102"
 #define SUMMARYRECSYNTAX "1.2.840.10003.5.103"
 #define GRS0RECSYNTAX    "1.2.840.10003.5.104"
 #define ESRECSYNTAX      "1.2.840.10003.5.106"

---------------------------------------------------------*/
/* handle retrieval from virtual databases... */

int /* diagnostic_code */
CheshireFetchVirtual(char **buf, char **outptr, ZSESSION *Session, 
		     int record_id, int *result_recsize, int elementsetreq,
		     weighted_result *virtual_set, char *vfilename, int vfiletype)
{
  char tempresultname[MAX_RSN_LENGTH];
  weighted_result *work_set;
  int rec_number;
  int real_docid;
  int fetchresult = 0;

  if (virtual_set->num_hits < record_id) { 
    diagnostic_set(13,0,NULL);
    return(diagnostic_get());
  }

  work_set = virtual_set->entries[record_id-1].xtra;
  rec_number = virtual_set->entries[record_id-1].record_num + 1;

  /* put the information into the Session structure */
  strcpy(tempresultname,  Session->resultSetName);
  strcpy(Session->databaseName, work_set->filename);
  strcpy(Session->resultSetName, work_set->setid);
  fetchresult = CheshireFetchRecs(buf, outptr, Session, 
				  rec_number, result_recsize, elementsetreq);

  /* put back the info for the virtual db */
  strcpy(Session->resultSetName,tempresultname);
  strcpy(Session->databaseName,vfilename);

  return (fetchresult);
     
}

int /* diagnostic_code */
CheshireFetchRecs(char **buf, char **outptr, ZSESSION *Session, 
		  int record_id, int *result_recsize, int elementsetreq)
{
  
  extern SGML_DTD *current_sgml_dtd;
  SGML_Document *doc, *GetSGML(), *sgml_parse_document();
  SGML_Document *compdoc=NULL;
  SGML_Data *subdoc=NULL;
  SGML_Data *GetData();
  int filetype;
  char *marcrec = NULL, *rawrec=NULL, *GetRawSGML(), *sgml_to_marc();
  char *filename, *se_get_result_filetag();
  display_format *cf_getdisplay();
  char *element_set_name = NULL;
  float doc_rank;
  char *tmp_buf;
  char *recsyntax;
  int relevance_score;

  /* results from previous query processing */
  weighted_result *final_set;
  
  diagnostic_clear();
  
  current_user_id(Session);    /* set the "user id" e.g. connection number */

  if (strcasecmp(Session->databaseName, "METADATA") == 0) {
    
    fprintf(LOGFILE,"METADATA Present\n");
    
    if (metadata_buffer) {
      *result_recsize = strlen(metadata_buffer);
      *buf = CALLOC(char, *result_recsize+1);
      if (*result_recsize > 0) {
      
	if (*result_recsize < Session->s_exceptionalRecordSize)
	  strcpy(*buf,metadata_buffer);
	else {
	  *result_recsize = 0;
	  diagnostic_set(17,0,NULL);
	}
      }
    }
    else {
      /* Null metadata length 0 should not happen */
      *result_recsize = 0;
      diagnostic_set(14,0,"No data for metadata results");
    }
  }
  else if (strcasecmp(Session->databaseName, "IR-EXPLAIN-1") == 0) {
    return(ExplainPresent(outptr, Session, record_id, result_recsize));
  }
  else { /* not a metadata query -- a normal query */
    
    final_set = se_get_result_set(Session->resultSetName);
    
    if (final_set == NULL) {
      return(diagnostic_get());
    }

    filename = se_get_result_filetag(Session->resultSetName);
    filetype = cf_getfiletype(filename);
    
    if ((final_set->result_type & VIRTUAL_RESULT) == VIRTUAL_RESULT) {

      return(CheshireFetchVirtual(buf, outptr, Session, record_id, 
				  result_recsize, elementsetreq, final_set,
				  filename, filetype));
    }
    
    doc_rank = final_set->entries[record_id-1].weight;
    
    /* the relevance score is scaled to the best in the set (1 for boolean)*/
    if (final_set->result_type & PROBABILISTIC_RESULT)
      relevance_score = 
	(int)((doc_rank/final_set->entries[0].weight)*1000.00);
    else
      relevance_score = 1000;
    
    if (final_set->result_type & COMPONENT_RESULT) {
      /* return components -- not full documents */
      
      if (strcmp(Session->preferredRecordSyntax, MARCRECSYNTAX) == 0) {
	*result_recsize = 0;
	diagnostic_set(227,0, "Components not (currently) available in MARC format -- Try SGML or SUTRS");
	return(diagnostic_get());
      }
      
      subdoc = get_component_data(final_set->entries[record_id-1].record_num,
				  final_set->component, &compdoc);
      
      if (subdoc) {
	displayComponent(compdoc, subdoc, final_set->entries[record_id-1].record_num, 
			 Session->exceptionalRecordSize,
			 Session->presentElementSet,
			 &(Session->preferredRecordSyntax),
			 filename, final_set->component, buf, 
			 record_id-1, relevance_score, doc_rank);
	/* buffer will be filled with the results */
	/* and both are referenced by buf         */
	if (strcmp(Session->preferredRecordSyntax, GRS1RECSYNTAX) == 0) {
	  *outptr = *buf;
	  *result_recsize = 500;
	  FREE(subdoc);
	  return(diagnostic_get());
	}
	else {
	  if (strcasecmp(Session->s_options, "JSERVER") == 0) {
	    /* store ranks for JSERVER requests */

	    tmp_buf = CALLOC(char, strlen(*buf)+1+10);
	    sprintf(tmp_buf, "%010.3f", doc_rank);
	    strcat(tmp_buf,*buf);
	    FREE(*buf);
	    *buf = tmp_buf;
	  }
	  *result_recsize = strlen(*buf);
	  FREE(subdoc);
	  return(diagnostic_get());
	}	
      }
    }  
    else { /* not components -- normal retrieval */
      element_set_name = Session->presentElementSet;

      if (filetype > 99) {
	cf_open_datastore(find_file_config(Session->databaseName));
      }

      if ((elementsetreq == 0 
	   || ((elementsetreq == 1 && strcasecmp(element_set_name,"F") == 0))
	   ||  (elementsetreq == 1 && strncasecmp(element_set_name,"STRING_SEGMENT",13) == 0))
	  && (strcmp(Session->preferredRecordSyntax, SUTRECSYNTAX) == 0
	      || strcmp(Session->preferredRecordSyntax, XML_RECSYNTAX) == 0
	      || strcmp(Session->preferredRecordSyntax, HTML_RECSYNTAX) == 0
	      || strcmp(Session->preferredRecordSyntax, SGML_RECSYNTAX) == 0)
	  && (cf_getdisplay(filename, Session->preferredRecordSyntax, 
			    element_set_name) == NULL)) {
	/* return the raw record */
	
	rawrec = GetRawSGML(filename, 
			    final_set->entries[record_id-1].record_num);
	
	if (rawrec == NULL) {
	  /* no record found -- system error?? */
	  *result_recsize = 0;
	  diagnostic_set(14,0,NULL);
	}
	else {/*  have a raw record */
	  
	  if (strncasecmp(element_set_name,"STRING_SEGMENT_",15) == 0) {
	    *result_recsize =
	      displaystring(rawrec, 
			    final_set->entries[record_id-1].record_num, 
			    buf, Session->exceptionalRecordSize,
			    element_set_name,
			    Session->preferredRecordSyntax,
			    filename,  record_id, 
			    relevance_score, doc_rank ); 
	
	    FREE (rawrec);
	    return(diagnostic_get());

	  }
	  else { /* send back the full record */
	    *result_recsize = strlen(rawrec);
	    if (strcasecmp(Session->s_options, "JSERVER") == 0) {
	      *buf = CALLOC(char, *result_recsize+1+10);
	      *result_recsize += 10;
	      sprintf(*buf, "%010.3f", doc_rank);
	      strcat(*buf,rawrec);
	      FREE (rawrec);		  
	    } 
	    else {
	      *buf = CALLOC(char, *result_recsize+1);
	      strcpy(*buf,rawrec);
	      FREE (rawrec);
	      return(diagnostic_get());
	    }
	  }
	}
      }
      else {

	doc = GetSGML(filename,  
		      final_set->entries[record_id-1].record_num);
	
	/* Process requests wanting MARC records returned */
	if (strcmp(Session->preferredRecordSyntax, MARCRECSYNTAX) == 0
	    && (cf_getdisplay(filename, Session->preferredRecordSyntax, 
			      element_set_name) == NULL) 
	    && (element_set_name == NULL
		|| strcasecmp(element_set_name, "F") == 0
		|| strcasecmp(element_set_name, "B") == 0)) {
	  if (filetype != FILE_TYPE_MARCFILE 
	      && filetype != FILE_TYPE_MARC_DATASTORE) {
	    *result_recsize = 0;
	    diagnostic_set(227,0, "Not available in MARC format -- Try SUTRS or SGML");
	    return(diagnostic_get());
	  }
	  marcrec = sgml_to_marc(doc);
	  if (marcrec == NULL) {
	    *result_recsize = 0;
	    diagnostic_set(227,0, "Not available in MARC format -- Try SUTRS or SGML");
	    return(diagnostic_get());
	  }
	  else
	    *result_recsize = strlen(marcrec);
	
	  if (*result_recsize < Session->exceptionalRecordSize) {
	    *buf = CALLOC(char, *result_recsize+1);
	    strcpy(*buf,marcrec);
	  }
	  else {
	    *result_recsize = 0;
	    diagnostic_set(17,0,NULL);
	  }
	  /* free up the documents, should be finished with them */
	  free_doc(doc);
	  FREE(marcrec);
	}
	else {
	  /* Process requests wanting non-MARC  or MARC conversion records returned */
	  if (Session->presentElementSet[0] == '\0') 
	    element_set_name = DefaultElementSetName(filename);
	
	  recsyntax = Session->preferredRecordSyntax;
	
	  if (filetype == FILE_TYPE_CLUSTER)
	    recsyntax = NULL;
	  
	  *result_recsize = 
	    displayrec(doc, final_set->entries[record_id-1].record_num, 
		       buf, Session->exceptionalRecordSize,
		       element_set_name,
		       recsyntax,
		       filename, Session->SearchAddInfo, outptr, record_id, 
		       relevance_score, doc_rank, final_set ); 
	
	
	  if (*result_recsize > Session->exceptionalRecordSize
	      || *result_recsize == -1) {
	    *result_recsize = 0;
	    diagnostic_set(17,0,NULL);
	  } 
	  else if (*result_recsize == -2) {
	    diagnostic_set(25,0,NULL); /* invalid element set name for DB */
	  }
	  else if (*result_recsize == -3) {
	    diagnostic_set(26,0,NULL); /* only default elements supported...*/
	  }
	  else if (*result_recsize == -4) { 
	    /* conversion error already set */
	  }
	
	
	  if (strcasecmp(Session->s_options, "JSERVER") == 0) {
	    if (strcmp(Session->preferredRecordSyntax, SUTRECSYNTAX) == 0
		|| strcmp(Session->preferredRecordSyntax, XML_RECSYNTAX) == 0
		|| strcmp(Session->preferredRecordSyntax, HTML_RECSYNTAX) == 0
		|| strcmp(Session->preferredRecordSyntax, SGML_RECSYNTAX) == 0) {	     
	      tmp_buf = *buf;
	      *buf = CALLOC(char, *result_recsize+1+10);
	      *result_recsize += 10;
	      sprintf(*buf, "%010.3f", doc_rank);
	      strcat(*buf,tmp_buf);
	      FREE (tmp_buf);		  
	    }
	  }

	  if (*buf == NULL && *outptr != NULL && *result_recsize > 0) {
	    *buf = *outptr;
	  }

	  free_doc(doc);
        }
      } /* raw record processing */
    }  /* end non-components */
  } /* end normal queries */
  return(diagnostic_get());
}



ListEntries *
CheshireScan(ZSESSION *session, char *attributeSetName, 
	     AttributesPlusTerm *termList,
	     int stepSize,
	     int numberOfTermsRequested,
	     int preferredPositionInResponse,
	     int *status, int *stepUsed)
{

  ListEntries *le;
  struct entries_List13	*entry, *entry_head, *entry_last;
  AttributeList *l;
  AttributeElement *elem; 
  char diagMessage[100];
  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  idx_list_entry *idx;
  DB *index_db;
  char *dbname;
  int dbnum;
  int db_errno;
  int more;
  int i, j;
  TermInfo *ti;
  struct list_List2 *l1;
  char *local_index_name;
  DBT keyval;
  DBT dataval;
  DBC *dbcursor;
  int db_type;

  idx_result tempinfo;

  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];


  if (termList == NULL) {
    *status = 6; /* failure */
    goto error;
  }

  /* default to BIB1 */
  if (attributeSetName == NULL)
    attributeSetName = OID_BIB1;
  
  
  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }
  
  
  le = CALLOC(ListEntries, 1);
  
  entry = entry_head = entry_last = NULL;
  
  if (stepSize < 0) {
    *stepUsed = 0;
    *status = 5;
    diagnostic_set(206,stepSize,"Negative Stepsize");
    return (NULL);
  }
  else
    *stepUsed = stepSize;
  
  *status = 0;
  
  /* if no results are requested return the PDU empty, and don't */
  /* bother with the index...                                    */
  
  if (numberOfTermsRequested > 0) {
    
    /* Check the terms and index info */ 
    for (dbnum = 0; session->c_databaseNames[dbnum] != NULL; dbnum++) {
      dbname = session->c_databaseNames[dbnum];
      

      db_type = cf_getfiletype(dbname);
      
      if (db_type == FILE_TYPE_VIRTUALDB) {
	SetError2 (session,228, "SCAN not currently supported for Virtual DBs");
	return NULL;
      }
      
      for (l = termList->attributes; l ; l = l->next) {
	elem = l->item;
	
	if (elem->attributeValue.which == e7_numeric) {
	  if (elem->attributeType < MAXATTRTYPE) {
	    attr_nums[elem->attributeType] = elem->attributeValue.u.numeric;
	    attr_oid[elem->attributeType] = elem->attributeSet;
	  } else {
	    /* diagnostic unsupported attribute type... */
	    diagnostic_set(113,elem->attributeType,NULL);
	    return NULL;
	  }
	}	  
	else {  /* complex attribute -- Must be local index name or string valued attribute type */
	  if (elem->attributeType < MAXATTRTYPE) {
	    attr_nums[elem->attributeType] = -1;
	    for (l1 = elem->attributeValue.u.complex.list;
		 l1; l1 = l1->next) {
	      if (l1->item->which == e25_numeric) {
		/* we don't handle any numeric types */
		diagnostic_set(123,0,NULL);
		return NULL;
	      } 
	      else { /* string type, so set the local name... */
		/* in principle there can be multiple items, take only last */
		if (l1->item == NULL || l1->item->u.string == NULL) {
		  diagnostic_set(126,0,"NULL String as attribute name");
		  return NULL;
		}
		attr_str[elem->attributeType] =
		  local_index_name = l1->item->u.string->data;
		attr_oid[elem->attributeType] = elem->attributeSet;
	      }    
	    }
	  } else {
	    /* diagnostic unsupported attribute type... */
	    diagnostic_set(113,elem->attributeType,NULL);
	    return NULL;
	  }
	}
      }  
      
      use = attr_nums[USE_ATTR];
      
      /* convert the specified elements to an index list entry, if possible */
      
      if (use == -1) {
	idx = cf_getidx_entry(dbname, local_index_name);
      }
      else 
	idx = cf_getidx_mapattr(dbname,
				attr_nums, 
				attr_str,
				attr_oid);
      
      relation = attr_nums[RELATION_ATTR];
      if (idx == NULL) { /* try without relation operator */
	attr_nums[RELATION_ATTR] = 0;
	idx = cf_getidx_mapattr(dbname,
				attr_nums, 
				attr_str,
				attr_oid);
	
	if (idx == NULL) { /* try without relation or truncation operator */
	  attr_nums[RELATION_ATTR] = 0;
	  attr_nums[TRUNC_ATTR] = 0;
	  idx = cf_getidx_mapattr(dbname,
				  attr_nums, 
				  attr_str,
				  attr_oid);
	  
	  if (idx == NULL) { /* try with only USE */
	    for (i = 2; i < MAXATTRTYPE; i++)
	      attr_nums[i] = 0;
	    idx = cf_getidx_mapattr(dbname,
				    attr_nums, 
				    attr_str,
				    attr_oid);
	  }
	  
	}
	
      }
      
      if (idx == NULL) {
	int code;
	
	code = diagnostic_get();
	if (code == 0)
	  code = 114;
	SetError2 (session, code, "No Indexes match specified attributes");
	goto error;
      }
      
      /* have a matching index entry so start the scan process */
      /* make sure everything is open */
      if (db_type == FILE_TYPE_DBMS) {
	int result;
	if ((result = se_dbms_scan(session, dbname, idx, termList->term,  le, 
				   stepSize, numberOfTermsRequested,
				   preferredPositionInResponse)) == 0)
	  return le;
	else {
	  SetError2(session, diagnostic_get(), 
		    diagnostic_string(diagnostic_get()));
	  goto error;
	}
      }
      
      if (idx->db == NULL) {
	index_db = 
	  (DB *) cf_index_open(dbname, idx->tag, INDEXFL);
	
	if (index_db == NULL) {
	  SetError2 (session, 109, "Couldn't Open index for scanning");
	  goto error;
	}
      }
      else
	index_db = idx->db;
      
      /* Acquire a cursor for the database. */
      if ((db_errno = index_db->cursor(index_db, NULL, &dbcursor, 0)) != 0) {
	SetError2 (session, 109, "Couldn't Open index for scanning");
	goto error;
      }
      
      /* Initialize the key/data pair so the flags aren't set. */
      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));
      
      keyval.data = (void *) termList->term->u.general->data;
      keyval.size = termList->term->u.general->length;
      
      if (keyval.size < 1)  { /* don't permit searches on NULL strings */
	SetError2 (session, 9, "Null term in scan");
	goto error;
      }
      
      db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
      
      if (db_errno == DB_LOCK_DEADLOCK) { /* no hit in the index */
	SetError2 (session, 9, "Locking problem with database");
	dbcursor->c_close(dbcursor);
	goto error;
      }
      
      more = numberOfTermsRequested;
      
      if (preferredPositionInResponse > 1) {
	/* back up so that the item appears in the correct position */
	for (i = preferredPositionInResponse; i > 1; i--)
	  for (j = 0; j <= stepSize; j++) 
	    if (dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV) 
		== DB_NOTFOUND)
	      *status = 5;
	
      }
      
      
      do {
	
	if ((idx->type & BITMAPPED_TYPE && dataval.size == bitmap_blocksize_bytes)
	    || strcmp(keyval.data,">>>Global Data Values<<<") == 0) {
	  /* do nothing */
	}
	else {
	  entry_last = entry;      
	  entry = CALLOC(struct entries_List13, 1);
	  
	  if (entry_head == NULL)
	    entry_head = entry;
	  else
	    entry_last->next = entry;
	  
	  entry->item = CALLOC(Entry, 1);
	  
	  entry->item->which = e19_termInfo;
	  ti = entry->item->u.termInfo =  CALLOC(TermInfo, 1);
	  
	  ti->term = CALLOC(Term, 1);
	  ti->term->which = e5_general;
	  ti->term->u.general = NewOctetString(keyval.data);
	  
	  if (dataval.size < sizeof(idx_result)) {
	  }
	  else {
	    memcpy((char *)&tempinfo,dataval.data,sizeof(idx_result));
	    
	    ti->globalOccurrences = tempinfo.num_entries;
	    /* ti->globalOccurrences = tempinfo.tot_freq; this is frequency */
	    
	  }
	  
	  more--;
	}
	for (i = 0; i <= stepSize; i++) {
	  if(dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT) != 0) {
	    more = 0; 
	    *status = 5; /* end of file so missing some */
	  }
	} 
	
      } while (more); 
      
    } /* loop for databases */
    
    dbcursor->c_close(dbcursor);
    
    le->entries = entry_head;
    return le;
  }
  else { /* no terms requested */
    return le;
  }
  
  
error:
    /*
     *	An error has occurred during the retrieval process.
     */
  *status = 6; /* failure */
  sprintf(diagMessage,"Scan failure -- %s",session->errorAddInfo);
  
  le->nonsurrogateDiagnostics = 
    CALLOC(struct nonsurrogateDiagnostics_List14, 1);
  
  le->entries = NULL;
  
  le->nonsurrogateDiagnostics->item     
    = MakeDiagRec(OID_DS_BIB1, session->errorCode, diagMessage);
  
  return le;
  
}



int
CheshireFreeDocs()
{
  extern SGML_Doclist  *PRS_first_doc;
  /* free all the doc_list memory */
  if (PRS_first_doc)
    free_doc_list(PRS_first_doc);
  
  PRS_first_doc = NULL;
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

/* The following is the API to the search engine from the V.3 code
 */


/*
 * ExecuteQuery returns Z_OK if the search has completed sucessfully,
 * otherwise returns Z_ERROR.
 */
int
ExecuteQuery(ZSESSION *session, Query *query, 
	     char **databases, char *resultSetId)
{
  int diagnostic;

  /* we later extract databases and resultSetId from the session */
  /* for now only the first database name is used... */
  strcpy(session->databaseName, databases[0]);
  strcpy(session->resultSetName, resultSetId);
 
  if (strcasecmp(session->databaseName,"IR-EXPLAIN-1") == 0) 
    /* process an explain request */
    diagnostic = ExplainSearch(session, query);
  else
    diagnostic = CheshireSearch(session, query); 
  
  if (diagnostic != 0) {
    SetError(session, diagnostic,diagnostic_string(diagnostic),
	     diagnostic_addinfo());

    return (Z_ERROR);
  }
  else
    return (Z_OK);
      
}


RECORD_LISTS *
RetrieveRecords2(ZSESSION *session, char *resultSetId, int start, int number,
		 int elementSetsRequested)
{
  int record_id;
  int diagnostic;
  int result_size;
  char *buffer = NULL; /* is now dynamically allocated
                          so it can handle metadata, big records etc. */
  char *outptr = NULL;      /* used for non-text output records like GRS-1 */
  char *actual_result_id;
  int real_start;
  weighted_result *bitmap_results, *tmpresults;
  int TEMP_BITMAP_RESULT_FLAG = 0;

  RECS *current_rec, *last_rec;

  RECORD_LISTS *reclist, *DBMS_GetRecs();

  if (session == NULL || resultSetId == NULL) {
    fprintf(LOGFILE,"session or resultsetid null in RetrieveRecords2\n");
    return NULL;
  }

  if (cf_getfiletype(session->databaseName) == FILE_TYPE_DBMS) {
    /* send back DBMS records */
    return(DBMS_GetRecs(session, resultSetId, start, number));
  }

  if (se_get_result_type(resultSetId) & BITMAP_RESULT) {
    /* have to create a temp resultset... */
    tmpresults = se_get_result_set(resultSetId);
    bitmap_results = se_bitmap_to_result(tmpresults, number, start);
    se_store_result_set(bitmap_results, "***TMP*BITMAP*RESULTS***",&tmpresults->filename,
			"DUMMY", DONT_SAVE_RESULT_SETS);
    real_start = 1;    
    TEMP_BITMAP_RESULT_FLAG = 1;
    actual_result_id = "***TMP*BITMAP*RESULTS***";
  }
  else {
    actual_result_id = resultSetId;
    real_start = start;
  }

  if ((strcasecmp(MARCRECSYNTAX, session->preferredRecordSyntax) == 0) 
      || (strcasecmp(SUTRECSYNTAX, session->preferredRecordSyntax) == 0) 
      || (strcasecmp(SGML_RECSYNTAX, session->preferredRecordSyntax) == 0) 
      || (strcasecmp(XML_RECSYNTAX, session->preferredRecordSyntax) == 0) 
      || (strcasecmp(HTML_RECSYNTAX, session->preferredRecordSyntax) == 0) 
      || (strcasecmp(OPACRECSYNTAX, session->preferredRecordSyntax) == 0)) {

    reclist = CALLOC(RECORD_LISTS, 1);  
    strcpy(reclist->databaseName, session->databaseName);
    reclist->numRecords = 0;
    reclist->records = CALLOC(RECS, 1);
    reclist->records->which = rec_type_list;
    /* allocates and initializes the list */
    reclist->records->u.list = ac_list_alloc(); 
  
    strcpy(session->resultSetName, actual_result_id);


    /* Fetch each record and add it to the output buffer */
    for (record_id = real_start; record_id < (real_start + number); record_id++) {
      /* clear the buffer */
      buffer = NULL;    
      diagnostic = CheshireFetchRecs(&buffer, &outptr, session, 
				     record_id, &result_size,
				     elementSetsRequested);
      
      if (diagnostic == 0 && 
	  (ac_list_append(reclist->records->u.list, 
		       buffer, result_size) != LIST_ERROR)) {  
	reclist->numRecords++;
	if (buffer) FREE(buffer);
      }
      else {
	/* free the list and treat as an error?? */
	ac_list_free(reclist->records->u.list);
	FREE(reclist->records);
	FREE(reclist);
	SetError(session, diagnostic,diagnostic_string(diagnostic),
		 diagnostic_addinfo());
	if (buffer) FREE(buffer);
	return NULL;
      }
    }
  }
  else { /* must be GRS1, Explain, or other structured format */

    reclist = CALLOC(RECORD_LISTS, 1);  
    strcpy(reclist->databaseName, session->databaseName);
    reclist->numRecords = 0;
    /* allocates and initializes the list */
  
    strcpy(session->resultSetName, actual_result_id);


    /* Fetch each record and add it to the output stuff */
    for (record_id = real_start; record_id < (real_start + number); record_id++) {
      /* clear the buffer */
      buffer = NULL;    
      diagnostic = CheshireFetchRecs(&buffer, &outptr, session, 
				     record_id, &result_size, 
				     elementSetsRequested);
      
      if (diagnostic == 0) {
	if (reclist->records == NULL)
	  current_rec = reclist->records = CALLOC(RECS, 1);
	else {
	  last_rec = current_rec;
	  current_rec = CALLOC(RECS, 1);
	  last_rec->next = current_rec;
	}
	current_rec->which = rec_type_asn1;
	current_rec->u.ASN1 = (any)outptr; 
	reclist->numRecords++;
	
      }
      else {
	/* free the list and treat as an error?? */
	FREE(reclist->records);
	FREE(reclist);
	SetError(session, diagnostic,diagnostic_string(diagnostic),
		 diagnostic_addinfo());
	return NULL;
      }
    }

  }

  if (TEMP_BITMAP_RESULT_FLAG) {
    /* get rid of the temp results  */
    se_delete_result_set("***TMP*BITMAP*RESULTS***");
    FREE(bitmap_results);
    strcpy(session->resultSetName, resultSetId);
  }

  return(reclist);

}





