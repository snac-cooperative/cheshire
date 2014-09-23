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
*	Header Name:	dbms_intf.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Search and retrieve data from external relational
*                       databases under the control of
*                       the configfile structure and session structure.
*                       
*	Usage:		resultcode = *DBMS_search(session, sql_query)
*                      
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*                       various structures depending on DBMS used.
*
*	Return Conditions and Return Codes:	
*                       returns 0 on successful search, errorcode otherwise.
*
*	Date:		8/21/97
*	Revised:	
*	Version:	1.0 
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"
#include "configfiles.h"
#include "dispmarc.h"
#include "session.h"

#ifdef ILLUSTRA
#include <mi.h>
#endif

#ifdef POSTGRESQL
#include "libpq-fe.h"
#endif

#ifdef MYSQLDBMS
#include <mysql.h>
#endif

#include <dmalloc.h>

#define XML   0
#define SGML  0
#define SUTRS 1
#define MARC 2
#define GRS1 3


extern config_file_info *cf_info_base;
extern config_file_info *find_file_config(char *);
extern void *cf_open_DBMS(config_file_info *cf, char *login, char *pwd);
extern char *diagnostic_string(int code);
extern char *diagnostic_addinfo();
extern weighted_result *se_get_result_set(char *setid);


/* Open a connection to the database specified as the file name */
int
DBMS_search(ZSESSION *session, char *SQL_Query) 
{
#ifdef ILLUSTRA
  MI_CONNECTION  *conn;
#endif
#ifdef POSTGRESQL
  PGconn *conn;
  PGresult *res;
  char  *fullquery;
  int	nFields;
  int	i, j;

#endif

#ifdef MYSQLDBMS
  MYSQL *conn;  
  MYSQL_RES *results;
  int resultcode;
  unsigned int num_fields;
  unsigned int i;
#endif

  int count;
  config_file_info *cf;
  char newsql[5000];

  cf = find_file_config(session->databaseName);

#ifdef MYSQLDBMS

  /* fprintf(LOGFILE,"login id %s Pass %s\n", session->c_login_id ? session->c_login_id : "NULL",
   *  session->c_login_pwd ? session->c_login_pwd : "NULL");
   */

  if (cf->file_ptr != NULL) 
    conn = (MYSQL *)cf->file_ptr;
  else
    conn = (MYSQL *)cf_open_DBMS(cf, session->c_login_id,
				  session->c_login_pwd);
  
  if (conn == NULL) {
    SetError(session, 2,diagnostic_string(2),
	     "Unable to open MYSQL database, see log file");
    return(2);
  }

  /* must have an open connection...*/
  /* submit the query */
  if ((resultcode = mysql_query(conn, SQL_Query)) != 0) {

    SetError(session, 2,diagnostic_string(2),
	     "Unable to open MYSQL database, see log file");
    return(2);
  }
  results = mysql_store_result(conn);

  count = get_results(conn, results, session);

  if (count >= 0) {
    session->resultCount = count; 
    fprintf(LOGFILE,
	    "DBMS Result Set Num_hits = %d\n",count);

    if (session->c_DBMS_query == NULL) 
      session->c_DBMS_query = strdup(SQL_Query);
    else {
      char *tmp;
      tmp = strdup(SQL_Query);
      FREE(session->c_DBMS_query);
      session->c_DBMS_query = tmp;
    }
  }
  else {
    return (2);
  }
    

  
#endif


#ifdef ILLUSTRA

  if (cf->file_ptr != NULL) 
    conn = (MI_CONNECTION *)cf->file_ptr;
  else
    conn = (MI_CONNECTION *)cf_open_DBMS(cf, NULL, NULL);

  /* send the command */
  if (mi_exec(conn, SQL_Query, 0) == MI_ERROR) {
    fprintf(LOGFILE, "cannot send: %s\n", SQL_Query);
    return (2);
  }
  
  count = get_results(conn, session);

  if (count >= 0) {
    session->resultCount = count; 
    fprintf(LOGFILE,
	    "DBMS Result Set Num_hits = %d\n",count);
  }
  else {
    return (2);
  }
    
  /* make sure it finished */
  if (mi_query_finish(conn) == MI_ERROR) {
    fprintf(LOGFILE, "command %s could not complete\n", SQL_Query);
    return (2);
  }

#endif

#ifdef POSTGRESQL
/*
 * Based on from /src/test/examples/testlibpq.c in the postgresql distribution
 *
 */

  /*
   * begin, by setting the parameters for a backend connection if the
   * parameters are null, then the system will try to use reasonable
   * defaults by looking up environment variables or, failing that,
   * using hardwired constants
   */

  if (cf->file_ptr != NULL) 
    conn = (PGconn *)cf->file_ptr;
  else
    conn = (PGconn *)cf_open_DBMS(cf, session->c_login_id,
				  session->c_login_pwd);

  if (conn == NULL) {
    SetError(session, 2,diagnostic_string(2),
	     "Unable to open POSTGRESQL database");
    return(2);
  }

  /* start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "Postgres BEGIN command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
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

  fullquery = CALLOC(char, 30+strlen(SQL_Query));
  if (fullquery == NULL) {
    fprintf(LOGFILE, "Unable to allocate DBMS query space\n");
    PQfinish(conn);
    cf->file_ptr = NULL;

    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL query allocation Error");
    return(2);
  }
  sprintf(fullquery,"DECLARE myportal CURSOR FOR %s", SQL_Query);

  res = PQexec(conn, fullquery);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "DECLARE CURSOR command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;

    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
  }
  PQclear(res);

  /* save the query in the session info */
  if (session->c_DBMS_query != NULL) {
    /* replace the old query */
    FREE(session->c_DBMS_query);
  }

  session->c_DBMS_query = strdup(SQL_Query);

  res = PQexec(conn, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
    fprintf(LOGFILE, "FETCH ALL command didn't return tuples properly\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;

    SetError(session, 2,diagnostic_string(2),
	     "PostgreSQL Error");
    return(2);
  }


  count = get_results(conn, res, session);

  if (count >= 0) {
    session->resultCount = count; 
    fprintf(LOGFILE,
	    "DBMS Result Set Num_hits = %d\n",count);
  }
  else {
    return (2);
  }
    

  /* close the portal */
  res = PQexec(conn, "CLOSE myportal");
  PQclear(res);
  
  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);
  
#endif

  return (0);

}

#ifdef ILLUSTRA

/* get_results */
int get_results(conn, session)
     MI_CONNECTION *conn;
     ZSESSION *session;
{
  int count;
  int result;
  char *cmd;
  
  while ((result = mi_get_result(conn)) != MI_NO_MORE_RESULTS)
    {
      switch(result)
	{
        case MI_ERROR:
	  fprintf(LOGFILE, "MI_ERROR: could not get results\n");
	  return(-1);
        case MI_DDL:
	  break;
        case MI_DML:
	  /* what kind of command was it? */
	  cmd = mi_result_command_name(conn);
	  
	  /* get # rows affected by last command */
	  if ((count = mi_result_row_count(conn)) == MI_ERROR)
            {
	      fprintf(LOGFILE, "MI_ERROR:cannot get row count\n");
	      return(-1);
            }
	  else
	    fprintf(LOGFILE, "%d rows affected by %s command\n", 
		    count, cmd);
	  break;
	case MI_ROWS:
	  get_data(conn, session);
	  break;
        default:
	  fprintf(LOGFILE, "unknown results\n");
	}
    }
  return (count);
}            


int get_data(conn, session)
  MI_CONNECTION *conn;
  ZSESSION *session;
{
    MI_ROW         *row;
    MI_ROW_DESC    *rowdesc;
    int            error;
    int            numcols;
    int            i;
    char           *colname;
    int            collen;
    char           *colval;
    int recsyntax;
    FILE *resultsetfile;
    char fname[1000];
    char **colnames;

    /* get the description of the row */
    rowdesc = mi_get_row_desc_without_row(conn);

    /* get the number of columns in the row */
    numcols = mi_column_count(rowdesc);

    colnames = CALLOC(char *, numcols);
    

    /* A simple SUTRS record is wanted */
    if (strcmp(session->preferredRecordSyntax, SUTRECSYNTAX) == 0) 
      recsyntax = SUTRS;
    else if (strcmp(session->preferredRecordSyntax, GRS1RECSYNTAX) == 0)
      recsyntax = GRS1;
    else if (strcmp(session->preferredRecordSyntax, MARCRECSYNTAX) == 0)
      recsyntax = MARC;

    sprintf(fname, "%s/%s", session->c_resultSetDir,
	    session->resultSetName);

    if (session->replaceIndicator) {

      if ((resultsetfile = fopen(fname, "w")) == NULL) {
	fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
	SetError(session, 128 ,diagnostic_string(128),
		 diagnostic_addinfo());
	return (-1);
      }
    }
    else { /* replace not on, so check for the result set */
      if ((resultsetfile = fopen(fname, "r")) == NULL) {
	if ((resultsetfile = fopen(fname, "w")) == NULL) {
	  fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
	  SetError(session, 128 ,diagnostic_string(128),
		   diagnostic_addinfo());
	  return (-1);
	}
      }
      else { /* result set already exists */
	fprintf (LOGFILE, 
		 "No Replace indicator and resultset file %s exists\n", fname);
	SetError(session, 128 ,diagnostic_string(128),
		 diagnostic_addinfo());
	return (-1);
      }
    }
    /* we have the file... */
    
    /* get the column names */
    for (i = 0; i < numcols; i++)
      {
	colnames[i] = mi_column_name(rowdesc, i);
	/* fprintf(resultsetfile, "%s\t", colname); */
      }
    /* fprintf(resultsetfile, "\n\n"); */
    
    /* for each row */
    while ((row = mi_next_row(conn, &error)) != NULL)
      {    
	/* for each column */
	for (i = 0; i < numcols; i++)
	  {
	    switch(mi_value(row, i, &colval, &collen))
	      {
	      case MI_ERROR:
		fprintf(resultsetfile, "\ncannot get value\n");
		return(-1);
	      case MI_NULL_VALUE:
		colval = "NULL";
		break;
	      case MI_NORMAL_VALUE:
		break;
	      default:
		fprintf(resultsetfile, "\nunknown value\n");
		return(-1);
	      }
	    
	    fprintf(resultsetfile, "<%s>%s</%s>", colnames[i], 
		    colval, colnames[i]);
	  }
	fprintf(resultsetfile, "\n");
      }
    /* fprintf(resultsetfile, "\n"); */
    fclose (resultsetfile);
    return(1);
}

#endif


#ifdef POSTGRESQL

/* get_results from a PostgreSQL search */
int get_results(PGconn *conn, PGresult *res, ZSESSION *session)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j;

  int recsyntax;
  FILE *resultsetfile;
  char fname[2000];
  char **colnames;


  nFields = PQnfields(res);
  count = nTuples = PQntuples(res);

  /* A simple SUTRS record is wanted */
  if (strcmp(session->preferredRecordSyntax, SUTRECSYNTAX) == 0) 
    recsyntax = SUTRS;
  else if (strcmp(session->preferredRecordSyntax, GRS1RECSYNTAX) == 0)
    recsyntax = GRS1;
  else if (strcmp(session->preferredRecordSyntax, MARCRECSYNTAX) == 0)
    recsyntax = MARC;
  else 
    recsyntax = 0; /* xml/sgml */

  sprintf(fname, "%s/%s", session->c_resultSetDir,
	  session->resultSetName);

  if (session->replaceIndicator) {

    if ((resultsetfile = fopen(fname, "w")) == NULL) {
      fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
      SetError(session, 128 ,diagnostic_string(128),
	       diagnostic_addinfo());
      return (-1);
    }
  }
  else { /* replace not on, so check for the result set */
    if ((resultsetfile = fopen(fname, "r")) == NULL) {
      if ((resultsetfile = fopen(fname, "w")) == NULL) {
	fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
	SetError(session, 128 ,diagnostic_string(128),
		 diagnostic_addinfo());
	return (-1);
      }
    }
    else { /* result set already exists */
      fprintf (LOGFILE, 
	       "No Replace indicator and resultset file %s exists\n", fname);
      SetError(session, 128 ,diagnostic_string(128),
	       diagnostic_addinfo());
      return (-1);
    }
  }
  /* we have the file... */
  /* save the syntax for this file */
  session->c_DBMS_recsyntax = recsyntax;

  /* for each row put out an XML record */

  for (i = 0; i < PQntuples(res); i++) {
    if (recsyntax == SUTRS) 
      fprintf(resultsetfile, "|");
    else
      fprintf(resultsetfile, "<DBDOC>");

    for (j = 0; j < nFields; j++)
    if (recsyntax == SUTRS)
      fprintf(resultsetfile, "%s |", PQgetvalue(res,i,j));
    else
      fprintf(resultsetfile, "<%s>%s</%s>", PQfname(res, j), 
	      PQgetvalue(res,i,j), PQfname(res,j));
    if (recsyntax == SUTRS)
      fprintf(resultsetfile, "\n");
    else
      fprintf(resultsetfile, "</DBDOC>\n");
  }


  PQclear(res);
  fclose (resultsetfile);
  
  return (count);
}            

#endif

#ifdef MYSQLDBMS

/* handler for mysql results */

int get_results(MYSQL *conn, MYSQL_RES *res, ZSESSION *session)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j;

  int recsyntax;
  FILE *resultsetfile;
  char fname[2000];
  char **colnames;

  MYSQL_FIELD *fields;  
  MYSQL_ROW *row;

  if (res == NULL) 
    return (2);

  /* handle the results... */
  nFields = mysql_num_fields(res);
  nTuples = mysql_num_rows(res);

  /* fprintf(LOGFILE,"Nfields = %d Ntuples = %d\n", nFields, nTuples); */

  /* A simple SUTRS record is wanted */
  if (strcmp(session->preferredRecordSyntax, SUTRECSYNTAX) == 0) 
    recsyntax = SUTRS;
  else if (strcmp(session->preferredRecordSyntax, GRS1RECSYNTAX) == 0)
    recsyntax = GRS1;
  else if (strcmp(session->preferredRecordSyntax, MARCRECSYNTAX) == 0)
    recsyntax = MARC;
  else 
    recsyntax = 0; /* xml/sgml */
  
  sprintf(fname, "%s/%s", session->c_resultSetDir,
	  session->resultSetName);

  if (session->replaceIndicator) {

    if ((resultsetfile = fopen(fname, "w")) == NULL) {
      fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
      SetError(session, 128 ,diagnostic_string(128),
	       diagnostic_addinfo());
      return (-1);
    }
  }
  else { /* replace not on, so check for the result set */
    if ((resultsetfile = fopen(fname, "r")) == NULL) {
      if ((resultsetfile = fopen(fname, "w")) == NULL) {
	fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", fname);
	SetError(session, 128 ,diagnostic_string(128),
		 diagnostic_addinfo());
	return (-1);
      }
    }
    else { /* result set already exists */
      fprintf (LOGFILE, 
	       "No Replace indicator and resultset file %s exists\n", fname);
      SetError(session, 128 ,diagnostic_string(128),
	       diagnostic_addinfo());
      return (-1);
    }
  }
  /* we have the file... */
  /* save the syntax for this file */
  session->c_DBMS_recsyntax = recsyntax;

  /* for each row put out an XML record */
  while ((row = (MYSQL_ROW *)mysql_fetch_row(res))) {
    unsigned long *lengths;
    
    if (recsyntax == SUTRS) 
      fprintf(resultsetfile, "|");
    else
      fprintf(resultsetfile, "<DBDOC>");
    
    lengths = mysql_fetch_lengths(res);
    fields = mysql_fetch_fields(res);
    for(i = 0; i < nFields; i++) {
      /* fprintf(LOGFILE,"Field (%u)%s is %u long: CONTENTS = %s\n", i, fields[i].name, lengths[i], row[i] ? row[i] : "NULL"); */

      if (recsyntax == SUTRS)
	fprintf(resultsetfile, "%s |", (char *)
		((char *)row[i] ? (char *)row[i] : "NULL"));
      else
	fprintf(resultsetfile, "<%s>%s</%s>", fields[i].name, 
		(char *)((char *)row[i] ? (char *)row[i] : "NULL"), fields[i].name);
    }
    if (recsyntax == SUTRS)
      fprintf(resultsetfile, "\n");
    else
      fprintf(resultsetfile, "</DBDOC>\n");    
  }
  

  fclose (resultsetfile);
  mysql_free_result(res);
  
  return (nTuples);
}            
#endif


#define MAXBUFFSIZE 50000



#ifdef MYSQLDBMS

/* handler for mysql results */

int get_index_results(MYSQL *conn, MYSQL_RES *res, char ***lines, FILE *dtdfile)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i=0, j=0, k=0;
  int sumlength;
  char *recdata;
  char **datalines;  
  MYSQL_FIELD *fields;  
  MYSQL_ROW *row;
  int first = 1;
  
  if (res == NULL) 
    return (2);
  
  /* handle the results... */
  nFields = mysql_num_fields(res);
  nTuples = mysql_num_rows(res);
  
  datalines = CALLOC(char *, nTuples+1);

  /* fprintf(LOGFILE,"Nfields = %d Ntuples = %d\n", nFields, nTuples); */
  first = 1;

  /* for each row put out an XML record */
  while ((row = (MYSQL_ROW *)mysql_fetch_row(res))) {
    unsigned long *lengths;
    /* MYSQL_ROW_OFFSET rowid; */
    /* get field lengths and metadata... */
    sumlength = 0;
    /* rowid = mysql_row_tell(res); */
    lengths = mysql_fetch_lengths(res);
    fields = mysql_fetch_fields(res);

    

    for (j = 0; j < nFields; j++)
      sumlength += lengths[j] + 7 + 8 + 5 + (2 * strlen(fields[j].name));
    
    recdata = CALLOC(char, sumlength+2);
    
    strcat(recdata, "<DBDOC>");
    
    for (j = 0; j < nFields; j++) {
      
      strcat(recdata,"<");
      strcat(recdata, fields[j].name);
      strcat(recdata, ">");
      strcat(recdata, ((char *)row[j] ? (char *)row[j] : "NULL"));
      strcat(recdata,"</");
      strcat(recdata, fields[j].name);
      strcat(recdata, ">");
    }
    strcat(recdata, "</DBDOC>");

    /* printf("ROWID %d: %s\n", rowid, recdata); */

    if (first == 1 && dtdfile) {
      /* GAAAAAWWWDDDD what a pain -- apparently the length and field    */
      /* data only exist in a row-by-row situation so we need to do this */
      /* for the FIRST ROW ONLY...                                       */

      /* The following thing needs to be at the top of the DTD file... */
      /* this is provided by the calling function now (for naming)     */
      /* fprintf(dtdfile,"<!doctype dbdoc [\n");                       */
      fprintf(dtdfile,"<!ELEMENT DBDOC (%s", fields[0].name);
      for (k = 1; k < nFields; k++) 
	fprintf(dtdfile,", %s", fields[k].name);
      fprintf(dtdfile,")>\n");
      
      for (k = 0; k < nFields; k++)
	fprintf(dtdfile,"<!ELEMENT %s (#PCDATA)>\n", fields[k].name);
      
      /* The following thing needs to be at the bottom of the DTD file... */
      /* this is provided by the calling function now (for naming)        */
      /* fprintf(dtdfile,"\n]>\n");                                       */
      first = 0;
    }
    
    datalines[i++] = recdata;
  }
  
  mysql_free_result(res);

   *lines = datalines;
    
   return (nTuples);
}   
#endif

#ifndef INDEXONLY


#ifdef MYSQLDBMS
/* handler for mysql DBMS index results */

RECORD_LISTS *
get_dbms_search_results(ZSESSION *session, MYSQL *conn, MYSQL_RES *res, 
			int inrecsyntax, weighted_result *final_set, 
			int start, int num_wanted)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j, docid;
  int recsyntax;
  FILE *resultsetfile;
  char **colnames;
  int sumlength = 0;
  char *recdata;
  RECORD_LISTS *reclist = NULL;

  MYSQL_FIELD *fields;  
  MYSQL_ROW *row;

  if (res == NULL) 
    return (NULL);

  /* handle the results... */
  nFields = mysql_num_fields(res);
  nTuples = mysql_num_rows(res);

  /* fprintf(LOGFILE,"Nfields = %d Ntuples = %d\n", nFields, nTuples); */

  if (num_wanted > final_set->num_hits) 
    num_wanted = final_set->num_hits;

  reclist = CALLOC(RECORD_LISTS, 1);  
  strcpy(reclist->databaseName, session->databaseName);
  reclist->numRecords = 0;
  reclist->records = CALLOC(RECS, 1);
  reclist->records->which = rec_type_list;
  /* allocates and initializes the list */
  reclist->records->u.list = ac_list_alloc(); 
  
  /* for each row put out an XML record */
  for (i = start; i < final_set->num_hits 
	 && i < (num_wanted + start) /*final_set->num_hits*/; i++) {
    unsigned long *lengths;
    
    docid = final_set->entries[i].record_num;

    mysql_data_seek(res, (my_ulonglong) docid-1);
    row = (MYSQL_ROW *)mysql_fetch_row(res);
   
    lengths = mysql_fetch_lengths(res);
    fields = mysql_fetch_fields(res);


    for (j = 0; j < nFields; j++)
      sumlength += lengths[j] + 7 + 8 + 5 + (2 * strlen(fields[j].name));
    
    recdata = CALLOC(char, sumlength+2);
    
    if (recsyntax == SUTRS) 
       strcat(recdata, "|");
    else
      strcat(recdata, "<DBDOC>");
    
    for (j = 0; j < nFields; j++) {
      
      if (recsyntax == SUTRS) {
	strcat(recdata, ((char *)row[j] ? (char *)row[j] : "NULL"));
	strcat(recdata, "|");
      }
      else {
	strcat(recdata,"<");
	strcat(recdata, fields[j].name);
	strcat(recdata, ">");
	strcat(recdata, ((char *)row[j] ? (char *)row[j] : "NULL"));
	strcat(recdata,"</");
	strcat(recdata, fields[j].name);
	strcat(recdata, ">");
      }
    }

    if (recsyntax != SUTRS) 
      strcat(recdata, "</DBDOC>");

    
    if (ac_list_append(reclist->records->u.list, recdata, strlen(recdata)) 
	!= LIST_ERROR) {  
      reclist->numRecords++;
    }
    else {
      /* free the list and treat as an error?? */
      ac_list_free(reclist->records->u.list);
      FREE(reclist->records);
      FREE(reclist);
      SetError(session, 2 ,diagnostic_string(2),
	       diagnostic_addinfo());
      return NULL;
    }
  }
  
  return (reclist);
}            
         
#endif

RECORD_LISTS *
DBMS_GetRecs(ZSESSION *session, char *resultSetId, int start, int number)
{
  int count;
  config_file_info *cf;  
  int record_id;
  int diagnostic;
  int result_size;
  char *buffer; /* so it can handle metadata, etc. */
  char *result;
  int recsyntax;
  FILE *resultsetfile;
  weighted_result *final_set;
  RECORD_LISTS *reclist;
#ifdef MYSQLDBMS
  MYSQL *conn;  
  MYSQL_RES *results;
  int resultcode;
  unsigned int num_fields;
  unsigned int i;
#endif



  if (session == NULL || resultSetId == NULL) {
    fprintf(LOGFILE,"session or resultsetid null in DBMS_GetRecs\n");
    return NULL;
  }
    
  /* A simple SUTRS record is wanted */
  if (strcmp(session->preferredRecordSyntax, SUTRECSYNTAX) == 0) 
    recsyntax = SUTRS;
  else if (strcmp(session->preferredRecordSyntax, XML_RECSYNTAX) == 0) 
    recsyntax = XML;
  else if (strcmp(session->preferredRecordSyntax, SGML_RECSYNTAX) == 0) 
    recsyntax = SGML;
  else if (strcmp(session->preferredRecordSyntax, GRS1RECSYNTAX) == 0) {
    recsyntax = GRS1;
    SetError(session, 227 ,diagnostic_string(227),
	     diagnostic_addinfo());
    return (NULL);
  }
  else if (strcmp(session->preferredRecordSyntax, MARCRECSYNTAX) == 0) {
    recsyntax = MARC;
    SetError(session, 227 ,diagnostic_string(227),
	     diagnostic_addinfo());
    return (NULL);
  }
  else {
    SetError(session, 227 ,diagnostic_string(227),
	     diagnostic_addinfo());
    return (NULL);
  }

  if ((final_set = se_get_result_set(resultSetId)) == NULL) {

    buffer = CALLOC(char, MAXBUFFSIZE);


    if (session->c_DBMS_recsyntax != recsyntax) {
      /* the previously saved records are in a different syntax */
      /* To fix this we must re-run the query -- expensive but  */
      /* the only other way is to try to keep all of the data and */
      /* reformat as needed...                                    */
      DBMS_search(session, session->c_DBMS_query);

    }


    sprintf(buffer, "%s/%s", session->c_resultSetDir,
	    resultSetId);
    
    if ((resultsetfile = fopen(buffer, "r")) == NULL) {
      fprintf (LOGFILE, "Unable to open DBMS resultset file %s\n", buffer);
      SetError(session, 30 ,diagnostic_string(30),
	       diagnostic_addinfo());
      return (NULL);
    }

    reclist = CALLOC(RECORD_LISTS, 1);  
    strcpy(reclist->databaseName, session->databaseName);
    reclist->numRecords = 0;
    reclist->records = CALLOC(RECS, 1);
    reclist->records->which = rec_type_list;
    /* allocates and initializes the list */
    reclist->records->u.list = ac_list_alloc(); 
    
    
    strcpy(session->resultSetName, resultSetId);
    
    /* skip all the records before "start" */
    for (record_id = 1; record_id < start; record_id++) {
      result = fgets(buffer, MAXBUFFSIZE, resultsetfile);
    }
    
    /* Fetch each record and add it to the output stuff */
    for (record_id = start; record_id < (start + number); record_id++) {
      
      if ((result = fgets(buffer, MAXBUFFSIZE, resultsetfile)) != NULL) {
	result_size = strlen(result);
	if (ac_list_append(reclist->records->u.list, buffer, result_size) != LIST_ERROR) {  
	  reclist->numRecords++;
	}
	else {
	  /* free the list and treat as an error?? */
	  ac_list_free(reclist->records->u.list);
	  FREE(reclist->records);
	  FREE(reclist);
	  SetError(session, 2 ,diagnostic_string(2),
		   diagnostic_addinfo());
	  return NULL;
	}
      }
    }
  }
  else {
    /* We have a matching resultset from a DBMS index search */
    
#ifdef MYSQLDBMS
    
    cf = find_file_config(session->databaseName);
    
    if (cf->file_ptr != NULL) 
      conn = (MYSQL *)cf->file_ptr;
    else
      conn = (MYSQL *)cf_open_DBMS(cf, session->c_login_id,
				   session->c_login_pwd);
    
    if (conn == NULL) {
      SetError(session, 2,diagnostic_string(2),
	       "Unable to open MYSQL database, see log file");
      return(NULL);
    }
    
    /* must have an open connection...*/
    /* submit the query */
    
    /* The SQL query to be run is the one used for the indexing */
    
    if (final_set->index_ptr[0] == NULL 
	|| final_set->index_ptr[0]->keys == NULL
	|| final_set->index_ptr[0]->keys->key == NULL) {
      SetError(session, 2,diagnostic_string(2),
	       "Unable to open MYSQL database, No SQL data in Index config for this index?");
      return(NULL);
    }
    
    if ((resultcode = 
	 mysql_query(conn, final_set->index_ptr[0]->keys->key)) != 0) {
      
      SetError(session, 2,diagnostic_string(2),
	       "Unable to open MYSQL database, see log file");
      return(NULL);
    }
    results = mysql_store_result(conn);
    
    
    reclist = get_dbms_search_results(session, conn, results, recsyntax,
				      final_set, start, number);
#endif    

  }

  return(reclist);

}

/* end for NOT INDEXONLY */
#endif

#ifdef POSTGRESQL

/* get_results from a PostgreSQL search */
int get_index_results(PGconn *conn, PGresult *res, char ***lines, FILE *dtdfile)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i=0, j=0;
  char **datalines;  
  int sumlength=0;
  char *recdata;

  nFields = PQnfields(res);
  count = nTuples = PQntuples(res);

  datalines = CALLOC(char *, nTuples+1);

  /* fprintf(LOGFILE,"Nfields = %d Ntuples = %d\n", nFields, nTuples); */

  /* for each row put out an XML record */

  for (i = 0; i < count; i++) {
    
    sumlength = 0;
    
    for (j = 0; j < nFields; j++)
      sumlength =+ PQgetlength(res,i,j) + 7 + 8 + 5 + (2 * strlen(PQfname(res,j)));
    
    recdata = CALLOC(char, sumlength+2);

    strcat(recdata, "<DBDOC>");

    for (j = 0; j < nFields; j++) {
      
      strcat(recdata,"<");
      strcat(recdata, PQfname(res, j));
      strcat(recdata, ">");
      strcat(recdata,PQgetvalue(res,i,j));
      strcat(recdata,"</");
      strcat(recdata, PQfname(res, j));
      strcat(recdata, ">");
    }
    strcat(recdata, "</DBDOC>");

    datalines[i] = recdata;

  }

  if (dtdfile) {
    /* The following thing needs to be at the top of the DTD file... */
    /* this is provided by the calling function now (for naming)     */
    /* fprintf(dtdfile,"<!doctype dbdoc [\n");                       */
    fprintf(dtdfile,"<!ELEMENT DBDOC (%s", PQfname(res,0));
    for (j = 1; j < nFields; j++) 
      fprintf(dtdfile,", %s", PQfname(res,j));
    fprintf(dtdfile,")>\n");
    
    for (j = 0; j < nFields; j++)
      fprintf(dtdfile,"<!ELEMENT %s (#PCDATA)>\n", PQfname(res,j));
    
  }

  PQclear(res);

  *lines = datalines;
  
  return (count);
}            

#endif


#ifdef INDEXONLY

/* Open a connection to the database specified as the file name */
/* for indexing of the data specified in the SQL query          */
int
DBMS_index_search(config_file_info *cf, char *SQL_Query, 
		  char *passwd, char ***lines, FILE *dtdfile) 
{
#ifdef POSTGRESQL
  PGconn *conn;
  PGresult *res;
  char  *fullquery;
  int	nFields;
  int	i, j;

#endif

#ifdef MYSQLDBMS
  MYSQL *conn;  
  MYSQL_RES *results;
  int resultcode;
  unsigned int num_fields;
  unsigned int i;
#endif

  int count=0;


  if (cf == NULL || SQL_Query == NULL)  {
    fprintf(LOGFILE,"NULL configfile or SQL_Query in DBMS_index_search\n");
    return(-2);
  }

#ifdef MYSQLDBMS

  if (cf->file_ptr != NULL) 
    conn = (MYSQL *)cf->file_ptr;
  else
    conn = (MYSQL *)cf_open_DBMS(cf, cf->file_name,
				 passwd);
  
  if (conn == NULL) {
    fprintf(LOGFILE,
	    "Unable to open MYSQL database, No connection\n");
    return(-2);
  }

  /* must have an open connection...*/
  /* submit the query */
  if ((resultcode = mysql_query(conn, SQL_Query)) != 0) {
    fprintf(LOGFILE,
	    "Unable to execute MYSQL query (%s) Reason: %s\n", 
	    SQL_Query, mysql_error(conn));
    return(-2);
  }
  results = mysql_store_result(conn);

  count = get_index_results(conn, results, lines, dtdfile);

  if (count >= 0) {
    fprintf(LOGFILE,
	    "DBMS Result Set Num_hits = %d\n",count);
  }
  else {
    return (-2);
  }
#endif

#ifdef POSTGRESQL
/*
 * Based on from /src/test/examples/testlibpq.c in the postgresql distribution
 *
 */

  /*
   * begin, by setting the parameters for a backend connection if the
   * parameters are null, then the system will try to use reasonable
   * defaults by looking up environment variables or, failing that,
   * using hardwired constants
   */

  if (cf->file_ptr != NULL) 
    conn = (PGconn *)cf->file_ptr;
  else
    conn = (PGconn *)cf_open_DBMS(cf, cf->filename
				  passwd);
  
  if (conn == NULL) {
    fprintf(LOGFILE,
	     "Unable to open POSTGRESQL database\n");
    return(-2);
  }

  /* start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "Postgres BEGIN command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
    return(-2);
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

  fullquery = CALLOC(char, 30+strlen(SQL_Query));
  if (fullquery == NULL) {
    fprintf(LOGFILE, "Unable to allocate DBMS query space\n");
    PQfinish(conn);
    cf->file_ptr = NULL;
    fprintf(LOGFILE,
	     "PostgreSQL query allocation Error\n");
    return(-2);
  }
  sprintf(fullquery,"DECLARE myportal CURSOR FOR %s", SQL_Query);

  res = PQexec(conn, fullquery);
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(LOGFILE, "DECLARE CURSOR command failed\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
    return(-2);
  }
  PQclear(res);


  res = PQexec(conn, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
    fprintf(LOGFILE, "FETCH ALL command didn't return tuples properly\n");
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
    return(-2);
  }


  count = get_index_results(conn, res, lines);

  if (count >= 0) {
    fprintf(LOGFILE,
	    "DBMS Result Set Num_hits = %d\n",count);
  }
  else {
    return (-2);
  }
    

  /* close the portal */
  res = PQexec(conn, "CLOSE myportal");
  PQclear(res);
  
  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);
  
#endif

  return (count);

}

#endif
