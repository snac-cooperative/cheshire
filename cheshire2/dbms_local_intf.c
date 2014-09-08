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
/* MySQL includes go here */
#include <mysql.h>
#endif

#include <dmalloc.h>

#define XML   0
#define SGML  0
#define SUTRS 1
#define MARC 2
#define GRS1 3


extern config_file_info *cf_info_base;
config_file_info *find_file_config(char *);
void *cf_open_DBMS(config_file_info *cf, char *login, char *pwd) ;
char *diagnostic_string(int code);
char *diagnostic_addinfo();



/* Open a connection to the database specified as the file name */
int
DBMS_local_search(Tcl_Interp *interp, config_file_info *cf, 
		  char *inrecsyntax, char *SQL_Query, char *login, char *pwd) 
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
/* MySQL includes go here */
  MYSQL *conn;  
  MYSQL_RES *results;
  int resultcode;
  unsigned int num_fields;
  unsigned int i;
#endif
  int count;
  char newsql[5000];
  char buffer[500];

#ifdef MYSQLDBMS

  if (cf->file_ptr != NULL) 
    conn = (MYSQL *)cf->file_ptr;
  else
    conn = (MYSQL *)cf_open_DBMS(cf, login, pwd);

  if (conn == NULL) {
    Tcl_AppendResult(interp, "Unable to open MYSQL database", (char *) NULL);
    return TCL_ERROR;   
  }

  /* must have an open connection...*/
  if (strncasecmp(SQL_Query, "SELECT", 6) == 0) {
    /* this is a select, so go ahead with the usual... */
    /* submit the query */
    if ((resultcode = mysql_query(conn, SQL_Query)) != 0) {
      Tcl_AppendResult(interp, "Error in SQL Query: ", mysql_error(conn),
		       (char *) NULL);
      return TCL_ERROR;   
    }
    results = mysql_store_result(conn);
    count = get_local_results(conn, results, inrecsyntax, interp);
    mysql_free_result(results);
  }
  else { /* Not a Select */
    if ((resultcode = mysql_query(conn, SQL_Query)) != 0) {
      Tcl_AppendResult(interp, "MySQL command \"", SQL_Query, 
		       "\" failed: Message = ", mysql_error(conn), 
		       (char *) NULL);
      return TCL_ERROR;   
    }

    sprintf(buffer, "{SQL_Command_Result %d} {Result_String {%u Rows in database affected}}",
	  resultcode, mysql_affected_rows(conn));
    Tcl_AppendElement(interp, buffer);
    
  }
  /* tcl result code returned at function end */
  
#endif

#ifdef ILLUSTRA

  /* first clear the result data */
  Tcl_ResetResult(interp);

  if (cf->file_ptr != NULL) 
    conn = (MI_CONNECTION *)cf->file_ptr;
  else
    conn = (MI_CONNECTION *)cf_open_DBMS(cf);

  /* send the command */
  if (mi_exec(conn, SQL_Query, 0) == MI_ERROR) {
    fprintf(LOGFILE, "cannot send: %s\n", SQL_Query);
    return (2);
  }
  
  count = get_local_results(conn, session);

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
    conn = (PGconn *)cf_open_DBMS(cf, login, pwd);

  if (conn == NULL) {
    Tcl_AppendResult(interp, "Unable to open POSTGRESQL database ", (char *) NULL);
    return TCL_ERROR;   
  }

  /* start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    Tcl_AppendResult(interp, "Postgres BEGIN command failed ", (char *) NULL);
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
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

  fullquery = CALLOC(char, 30+strlen(SQL_Query));
  if (fullquery == NULL) {
    PQfinish(conn);
    cf->file_ptr = NULL;
    Tcl_AppendResult(interp, "Unable to allocate DBMS query space ", (char *) NULL);
    return TCL_ERROR;   
  }

  if (strncasecmp(SQL_Query, "SELECT", 6) == 0) {
    /* this is a select, so go ahead with the usual... */
    
    sprintf(fullquery,"DECLARE myportal CURSOR FOR %s", SQL_Query);
    
    res = PQexec(conn, fullquery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "DECLARE CURSOR command failed  for '",
		       fullquery, "'", (char *) NULL);
      return TCL_ERROR;   
    }
    PQclear(res);

    res = PQexec(conn, "FETCH ALL in myportal");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "FETCH ALL command didn't return tuples properly", (char *) NULL);
      return TCL_ERROR;   
    }


    count = get_local_results(conn, res, inrecsyntax, interp);

    if (count >= 0) {
      /* fprintf(LOGFILE,
	 "DBMS Result Set Num_hits = %d\n",count); */
    }
    else {
      Tcl_AppendResult(interp, "Error in fetching DBMS records", (char *) NULL);
      return TCL_ERROR;   
    }
    

    /* close the portal */
    res = PQexec(conn, "CLOSE myportal");
    PQclear(res);
  }
  else { /* not a select -- We execute the SQL directly */
    res = PQexec(conn, SQL_Query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      Tcl_AppendResult(interp, "SQL command \"", SQL_Query, 
		       "\" failed ", PQresultErrorMessage(res), 
		       (char *) NULL);
      
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      return TCL_ERROR;   
    }


    sprintf(buffer, "{SQL_Command_Result %d} {Result_String {%s}}",
	  PQresultStatus(res), PQresStatus(PQresultStatus(res)));
    Tcl_AppendElement(interp, buffer);

    PQclear(res);
  }
  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);
  /* close the connection to the database and cleanup */
  PQfinish(conn);
  cf->file_ptr = NULL;

#endif

  return (TCL_OK);

}

#ifdef ILLUSTRA

/* get_local_results */
int get_local_results(MI_CONNECTION *conn, Tcl_Interp *interp)
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

/* these are probably not worth fixing since illustra is basically dead */
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
int get_local_results(PGconn *conn, PGresult *res, char *inrecsyntax, 
		Tcl_Interp *interp)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j;
  char buffer[10000];

  int recsyntax;

  char **colnames;


  nFields = PQnfields(res);
  count = nTuples = PQntuples(res);

  /* A simple SUTRS record is wanted */
  if (inrecsyntax != NULL && strcmp(inrecsyntax, SUTRECSYNTAX) == 0) {
    recsyntax = SUTRS;
    sprintf(buffer, "{Hits %d} {Returning %d} {Start %d} {Resultset Default} {RecordSyntax SUTRS %s}",
	    count, count, 1, SUTRECSYNTAX);
  }
  else if (inrecsyntax != NULL && strcmp(inrecsyntax, GRS1RECSYNTAX) == 0)
    recsyntax = GRS1;
  else if (inrecsyntax != NULL && strcmp(inrecsyntax, MARCRECSYNTAX) == 0)
    recsyntax = MARC;
  else 
    sprintf(buffer, "{Hits %d} {Returning %d} {Start %d} {Resultset Default} {RecordSyntax XML %s}",
	    count, count, 1, XML_RECSYNTAX);

  Tcl_AppendElement(interp, buffer);
  Tcl_AppendResult(interp, " ",(char *) NULL);
      

  /* for each row put out an XML record */
  for (i = 0; i < PQntuples(res); i++) {
    if (recsyntax == SUTRS)
      Tcl_AppendResult(interp, "{|",(char *) NULL);
    else
      Tcl_AppendResult(interp, "{<DBDOC>",(char *) NULL);
    
    for (j = 0; j < nFields; j++) {
      if (recsyntax == SUTRS) {
	Tcl_AppendResult(interp, PQgetvalue(res,i,j),"|",(char *) NULL);

      }
      else {
	sprintf(buffer, "<%s>%s</%s>", PQfname(res, j), 
		PQgetvalue(res,i,j), PQfname(res,j));
	Tcl_AppendResult(interp, buffer,(char *) NULL);
      }
    }
    if (recsyntax == SUTRS)
      Tcl_AppendResult(interp, "} ",(char *) NULL);
    else
      Tcl_AppendResult(interp, "</DBDOC>} ",(char *) NULL);
    
  }
  
  
  PQclear(res);
  
  return (TCL_OK);
}            

#endif

#ifdef MYSQLDBMS

/* handler for mysql results */

int get_local_results(MYSQL *conn, MYSQL_RES *res, char *inrecsyntax, 
		Tcl_Interp *interp)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j;
  char buffer[10000];
  int recsyntax;
  FILE *resultsetfile;
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
  if (inrecsyntax != NULL && strcmp(inrecsyntax, SUTRECSYNTAX) == 0) {
    recsyntax = SUTRS;
    sprintf(buffer, "{Hits %d} {Returning %d} {Start %d} {Resultset Default} {RecordSyntax SUTRS %s}",
	    nTuples, nTuples, 1, SUTRECSYNTAX);
  }
  else if (inrecsyntax != NULL && strcmp(inrecsyntax, GRS1RECSYNTAX) == 0)
    recsyntax = GRS1;
  else if (inrecsyntax != NULL && strcmp(inrecsyntax, MARCRECSYNTAX) == 0)
    recsyntax = MARC;
  else 
    sprintf(buffer, "{Hits %d} {Returning %d} {Start %d} {Resultset Default} {RecordSyntax XML %s}",
	    nTuples, nTuples, 1, XML_RECSYNTAX);

  Tcl_AppendElement(interp, buffer);
  Tcl_AppendResult(interp, " ",(char *) NULL);
      
  /* for each row put out an XML record */

  while ((row = (MYSQL_ROW *)mysql_fetch_row(res))) {
    unsigned long *lengths;
    
    if (recsyntax == SUTRS) 
      Tcl_AppendResult(interp, "{|",(char *) NULL);
    else
      Tcl_AppendResult(interp, "{<DBDOC>",(char *) NULL);
    
    lengths = mysql_fetch_lengths(res);
    fields = mysql_fetch_fields(res);
    for(i = 0; i < nFields; i++) {
      /* fprintf(LOGFILE,"Field (%u)%s is %u long: CONTENTS = %s\n", i, fields[i].name, lengths[i], row[i] ? row[i] : "NULL"); */

      if (recsyntax == SUTRS)
	Tcl_AppendResult(interp,(row[i] ? row[i] : "NULL"),"|",(char *) NULL);
      else {
	sprintf(buffer, "<%s>%s</%s>", fields[i].name, 
		(row[i] ? row[i] : ""), fields[i].name);
	Tcl_AppendResult(interp, buffer,(char *) NULL);
      }

    }
    if (recsyntax == SUTRS)
      Tcl_AppendResult(interp, "} ",(char *) NULL);
    else
      Tcl_AppendResult(interp, "</DBDOC>} ",(char *) NULL);
    
  }
  
  return (nTuples);
}            

/* handler for mysql DBMS index results */

int get_local_index_results(MYSQL *conn, MYSQL_RES *res, char *inrecsyntax, 
			    Tcl_Interp *interp, weighted_result *final_set, 
			    int num_wanted)
{
  int count;
  int result;
  int nFields;
  int nTuples;
  int i, j, docid;
  char buffer[10000];
  int recsyntax;
  FILE *resultsetfile;
  char **colnames;

  MYSQL_FIELD *fields;  
  MYSQL_ROW *row;

  if (res == NULL) 
    return (2);

  /* handle the results... */
  nFields = mysql_num_fields(res);
  nTuples = mysql_num_rows(res);

  /* fprintf(LOGFILE,"Nfields = %d Ntuples = %d\n", nFields, nTuples); */

  Tcl_AppendResult(interp, " ",(char *) NULL);

  if (num_wanted > final_set->num_hits) 
    num_wanted = final_set->num_hits;
     
  /* for each row put out an XML record */
  for (j = 0; j < num_wanted /*final_set->num_hits*/; j++) {
    unsigned long *lengths;

    docid = final_set->entries[j].record_num;

    mysql_data_seek(res, (my_ulonglong) docid-1);
    row = (MYSQL_ROW *)mysql_fetch_row(res);
   
    if (recsyntax == SUTRS) 
      Tcl_AppendResult(interp, "{|",(char *) NULL);
    else
      Tcl_AppendResult(interp, "{<DBDOC>",(char *) NULL);
    
    lengths = mysql_fetch_lengths(res);
    fields = mysql_fetch_fields(res);
    for(i = 0; i < nFields; i++) {
      /* fprintf(LOGFILE,"Field (%u)%s is %u long: CONTENTS = %s\n", i, fields[i].name, lengths[i], row[i] ? row[i] : "NULL"); */

      if (recsyntax == SUTRS)
	Tcl_AppendResult(interp,(row[i] ? row[i] : "NULL"),"|",(char *) NULL);
      else {
	sprintf(buffer, "<%s>%s</%s>", fields[i].name, 
		(row[i] ? row[i] : ""), fields[i].name);
	Tcl_AppendResult(interp, buffer,(char *) NULL);
      }

    }
    if (recsyntax == SUTRS)
      Tcl_AppendResult(interp, "} ",(char *) NULL);
    else
      Tcl_AppendResult(interp, "</DBDOC>} ",(char *) NULL);
    
  }
  
  return (nTuples);
}            

#endif

/* for local display of indexed DBMS data, we need to retrieve all */
/* of the indexed items and then pick only the matching ones...    */
/* this is not really scalable solution for VERY large databases   */
/* but for preliminary work it seems to be the most effective      */

int DBMS_local_index_display(Tcl_Interp *interp, config_file_info *cf, 
			     char *inrecsyntax, weighted_result *final_set, 
			     char *login, char *pwd, int num_wanted)
{

#ifdef POSTGRESQL
  PGconn *conn;
  PGresult *res;
  char  *fullquery;
  int	nFields;
  int	i, j;
#endif
#ifdef MYSQLDBMS
/* MySQL includes go here */
  MYSQL *conn;  
  MYSQL_RES *results;
  int resultcode;
  unsigned int num_fields;
  unsigned int i;
#endif
  int count;
  char newsql[5000];
  char buffer[500];
  char *SQL_Query = NULL;

  if (final_set == NULL) {
    Tcl_AppendResult(interp, "No results in DBMS_local_index_display", 
		     (char *) NULL);
    return TCL_ERROR;   
  }
  else {
    /* get the SQL indexing query from the index structure (via the    */
    /* resultset. Assume a single index entry (better be the case ...) */
    if (final_set->index_ptr[0] != NULL 
	&& final_set->index_ptr[0]->keys != NULL
	&& final_set->index_ptr[0]->keys->key != NULL)
      SQL_Query = strdup(final_set->index_ptr[0]->keys->key);
    else {
      Tcl_AppendResult(interp, "No SQL data in Index config for this index?",
		       (char *) NULL);
      return TCL_ERROR;   
    }
  }


#ifdef MYSQLDBMS

  if (cf->file_ptr != NULL) 
    conn = (MYSQL *)cf->file_ptr;
  else
    conn = (MYSQL *)cf_open_DBMS(cf, login, pwd);

  if (conn == NULL) {
    Tcl_AppendResult(interp, "Unable to open MYSQL database", (char *) NULL);
    return TCL_ERROR;   
  }
  

  /* must have an open connection...*/
  if (strncasecmp(SQL_Query, "SELECT", 6) == 0) {
    /* this is a select, so go ahead with the usual... */
    /* submit the query */
    if ((resultcode = mysql_query(conn, SQL_Query)) != 0) {
      Tcl_AppendResult(interp, "Error in SQL Query: ", mysql_error(conn),
		       (char *) NULL);
      return TCL_ERROR;   
    }
    results = mysql_store_result(conn);
    /* count = get_local_results(conn, results, inrecsyntax, interp); */
    count = get_local_index_results(conn, results, inrecsyntax, interp, final_set, num_wanted);
    mysql_free_result(results);
  }
  else { /* Not a Select */
    Tcl_AppendResult(interp, "MySQL indexing retrieval command \"", SQL_Query, 
		     "\" failed: Message = ", mysql_error(conn), 
		     (char *) NULL);
    return TCL_ERROR;   
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
    conn = (PGconn *)cf_open_DBMS(cf, login, pwd);

  if (conn == NULL) {
    Tcl_AppendResult(interp, "Unable to open POSTGRESQL database ", (char *) NULL);
    return TCL_ERROR;   
  }

  /* start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    Tcl_AppendResult(interp, "Postgres BEGIN command failed ", (char *) NULL);
    PQclear(res);
    PQfinish(conn);
    cf->file_ptr = NULL;
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

  fullquery = CALLOC(char, 30+strlen(SQL_Query));
  if (fullquery == NULL) {
    PQfinish(conn);
    cf->file_ptr = NULL;
    Tcl_AppendResult(interp, "Unable to allocate DBMS query space ", (char *) NULL);
    return TCL_ERROR;   
  }

  if (strncasecmp(SQL_Query, "SELECT", 6) == 0) {
    /* this is a select, so go ahead with the usual... */
    
    sprintf(fullquery,"DECLARE myportal CURSOR FOR %s", SQL_Query);
    
    res = PQexec(conn, fullquery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "DECLARE CURSOR command failed  for '",
		       fullquery, "'", (char *) NULL);
      return TCL_ERROR;   
    }
    PQclear(res);

    res = PQexec(conn, "FETCH ALL in myportal");
    if (PQresultStatus(res) != PGRES_TUPLES_OK)  {
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      Tcl_AppendResult(interp, "FETCH ALL command didn't return tuples properly", (char *) NULL);
      return TCL_ERROR;   
    }


    count = get_local_results(conn, res, inrecsyntax, interp);

    if (count >= 0) {
      /* fprintf(LOGFILE,
	 "DBMS Result Set Num_hits = %d\n",count); */
    }
    else {
      Tcl_AppendResult(interp, "Error in fetching DBMS records", (char *) NULL);
      return TCL_ERROR;   
    }
    

    /* close the portal */
    res = PQexec(conn, "CLOSE myportal");
    PQclear(res);
  }
  else { /* not a select -- We execute the SQL directly */
    res = PQexec(conn, SQL_Query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
      Tcl_AppendResult(interp, "SQL command \"", SQL_Query, 
		       "\" failed ", PQresultErrorMessage(res), 
		       (char *) NULL);
      
      PQclear(res);
      PQfinish(conn);
      cf->file_ptr = NULL;
      return TCL_ERROR;   
    }


    sprintf(buffer, "{SQL_Command_Result %d} {Result_String {%s}}",
	  PQresultStatus(res), PQresStatus(PQresultStatus(res)));
    Tcl_AppendElement(interp, buffer);

    PQclear(res);
  }
  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);
  /* close the connection to the database and cleanup */
  PQfinish(conn);
  cf->file_ptr = NULL;

#endif

  return (TCL_OK);


}
	    

