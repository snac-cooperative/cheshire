
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
*	Header Name:	cf_DBMS.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to open and close external relational
*                       databases under the control of
*                       the configfile structure.
*                       
*	Usage:		FILE *cf_open_DBMS(config_file_info *cf, char *login, char *pwd)
*                       (defined in cheshire.h)
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*                       various structures depending on DBMS used.
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		8/21/97
*	Revised:	
*	Version:	1.0 
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "cheshire.h"
#include "configfiles.h"

#ifdef ILLUSTRA
#include <mi.h>

/* DB_ENV *db_init(char *); Definition conflicts with new DB */
#endif

#ifdef POSTGRESQL
#include "libpq-fe.h"
#endif 

#ifdef MYSQLDBMS
  /* MySQL structure definitions go here */
#include <mysql.h>
#endif  


extern config_file_info *cf_info_base;

/* Open a connection to the database specified as the file name */
void *
cf_open_DBMS(config_file_info *cf, char *login, char *pwd) 
{
#ifdef MYSQLDBMS
  /* MySQL access definitions go here */
  MYSQL *conn;
  char *passwd;
  char *userid;

  if (cf->file_ptr != NULL) { /* already opened this once */
    return (cf->file_ptr);
  }
  
  if (cf->file_name == NULL) {
    fprintf(LOGFILE, "No database name in cf_open_DBMS\n");
    return(NULL);
  }

  /* fprintf(LOGFILE, "database name in cf_open_DBMS is %s\n", cf->file_name);
   */

  conn = mysql_init(NULL);

  if (login == NULL) {
    userid = cf->file_name;
  }
  else 
    userid = login;

  if (pwd != NULL && strlen(pwd) == 0) {
    passwd = NULL;
  }
  else
    passwd = pwd;

  if (!mysql_real_connect(conn,"localhost", userid, passwd, cf->file_name,
			  0,NULL,0)) {
    fprintf(LOGFILE, "MYSQL: Failed to connect to database: Error: %s\n",
	    mysql_error(conn));
    return (NULL);
  }
  else {
    /* fprintf(LOGFILE, "connection OK in cf_open_DBMS\n");*/
  }
  cf->file_ptr = (FILE *)conn;
  return ((void *)conn);

#endif  

#ifdef ILLUSTRA
  MI_CONNECTION  *conn;
  char *mi_env, *getenv();
  char *envbuf;

  /* check for environment variables */
  if ((mi_env = getenv("MI_SYSPARAMS")) == NULL) { 
    /* try the assoc_name ... */
    if (cf->assoc_name != NULL && strcasecmp(cf->assoc_name, "NONE") != 0) {
      envbuf = CALLOC(char, strlen(cf->assoc_name)+15);
      sprintf(envbuf, "MI_SYSPARAMS=%s",cf->assoc_name);
      putenv(envbuf);
      mi_env = getenv("MI_SYSPARAMS");
    }
    else {
      /* There is no info on MI_SYSPARAMS... can't open the DBMS */ 
      fprintf(LOGFILE, "MI_SYSPARAMS = NULL  in cf_open_DBMS\n"); 
      exit(-1);
    }
  }


  if (cf->file_ptr != NULL) { /* already opened this once */
    return (cf->file_ptr);
  }

  if (cf->file_name == NULL) {
    fprintf(LOGFILE, "No database name in cf_open_DBMS\n");
    exit(-2);
  }

  if ((conn = mi_open(cf->file_name, NULL, NULL)) == NULL) { 
    fprintf(LOGFILE, "cannot open database: %s in cf_open_DBMS\n", 
	    cf->file_name);
    exit(-3);
  }

  cf->file_ptr = (FILE *)conn;
  return ((void *)conn);
  
#endif
#ifdef POSTGRESQL
  PGconn *conn;
  PGresult *res;
  char	   *pghost,
    *pgport,
    *pgoptions,
    *pgtty;

  char	   *dbName;
  /*
   * begin, by setting the parameters for a backend connection if the
   * parameters are null, then the system will try to use reasonable
   * defaults by looking up environment variables or, failing that,
   * using hardwired constants
   */
  pghost = NULL;       /* host name of the backend server */
  pgport = NULL;       /* port of the backend server */
  pgoptions = NULL;    /* special options to start up the backend
		        * server */
  pgtty = NULL;	       /* debugging tty for the backend server */
  dbName = cf->file_name;


  if (cf->file_ptr != NULL) { /* already opened this once */
    return (cf->file_ptr);
  }

  if (dbName == NULL) {
    fprintf(LOGFILE, "No database name in cf_open_DBMS\n");
    return(NULL);
  }


  /* make a connection to the database */
  conn = PQsetdbLogin(pghost, pgport, pgoptions, pgtty, dbName, login, pwd);

  /* check to see that the backend connection was successfully made */
  if (PQstatus(conn) == CONNECTION_BAD)
    {
      fprintf(LOGFILE, "Connection to database '%s' failed.\n", dbName);
      fprintf(LOGFILE, "%s", PQerrorMessage(conn));
      PQfinish(conn);
      
      return (NULL);
    }

  /* we pretend that this is a file pointer -- just to pass it via cf */
  cf->file_ptr = (FILE *)conn;
  return ((void *)conn);
  

#endif

}

/* Close the Database connection */
int
cf_close_DBMS(config_file_info *cf)
{
#ifdef ILLUSTRA
  MI_CONNECTION  *conn;

  if (cf == NULL) return 0;

  if (cf->file_ptr == NULL) return 0;

  conn = (MI_CONNECTION *)cf->file_ptr;

  mi_close(conn);

  cf->file_ptr = NULL;

  return (0);
#endif
#ifdef POSTGRESQL
  PGconn *conn;

  if (cf == NULL) return 0;

  if (cf->file_ptr == NULL) return 0;

  conn = (PGconn *)cf->file_ptr;

  PQfinish(conn);

  cf->file_ptr = NULL;

  return (0);
#endif
#ifdef MYSQLDBMS
  /* MySQL access definitions go here */
  MYSQL *conn;
  
  if (cf == NULL) return 0;

  if (cf->file_ptr == NULL) return 0;
  
  mysql_close((MYSQL *)cf->file_ptr);

  return(0);

#endif  
}















