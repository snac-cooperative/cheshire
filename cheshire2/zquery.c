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
/*
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Information Management and Systems, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************************************************************/
/* zquery.c -- handle search and retrieval of data via z39.50             */
/*               tcl return value                                         */
/**************************************************************************/
/* debugging -- define for debug output to stdout */
/* #define DEBUGGING */
#ifndef SOLARIS
#ifndef WIN32
#include "strings.h"
#endif
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifndef WIN32
#include <sys/param.h>
#ifdef LINUX
#define _XOPEN_SOURCE
#endif
#ifdef MACOSX
#define _STDC_
#endif
#include <unistd.h>
#endif

#include "tcl.h"

#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "z_parse.h"

#include "dmalloc.h"

/* global structures used for the Z39.50 session -- declared in loadcmd.c */
extern ZSESSION         *TKZ_User_Session;
extern ZASSOCIATION zAssociations;
ZSESSION *TKZ_User_Session_TMP = NULL;

char *TKZquery_buffer; /* query buffer in tcl interface */
FILE *LOGFILE = NULL; /* This is the client log file ptr (shared) */
char LOGFILENAME[200] = "tkzclient.log";
int LOGGING_ON = 0; /* this controls whether or not logging takes place */
                    /* for now -- default is no logging                 */
/* forward declarations */
void TKZ_DisplayRecords(ZSESSION *Session, 
			Tcl_Interp *interp, int type);
int TKZ_Present(ZSESSION *session);
int TKZ_Search(ZSESSION *session);
int TKZ_SearchResults(ZSESSION *Session, Tcl_Interp *interp);

AC_LIST *ac_list_alloc();

/* Utility routines */
extern void PrintDiagRec(FILE *, DefaultDiagFormat *);
extern char *MakeOPACListElement(OPACRecord *);
extern char *MakeExplainListElement(Explain_Record *, Tcl_Interp *interp);
/* note that we violate the ** declaration for these following */
extern ElementSetNames *MakeElementSetNames(char **, char **);
extern ZSESSION *LookupAssociation(ZASSOCIATION *, int);

extern PDU *MakeSortRequest(char *refid, char **in_results, char *out_results,
			         struct sortSequence_List18 *sort_keys);

RPNQuery *queryparse(char *query, char *attributeSetId);

extern int ProcessSearchResponse(ZSESSION *session, 
				 SearchResponse *searchResponse);

extern int ProcessPresentResponse (ZSESSION *session, PresentResponse *pr);

extern char *diagnostic_string(int code);

extern PDU *ClientGetPDU(int fd, int timeoutsecs);

void ClearPreviousSearch();
void LogPDU (FILE *file, PDU *pdu);

extern OctetString NewOctetString(char *in_string);  

int last_rec_proc = 0;
int result_set_changed = 0;

#ifndef SOLARIS
#define strdup(s) (char *)(strcpy(((char *)malloc((strlen(s)+1))),(s)))
#endif

char *
up_case_name (char *s)
{
  char *c, *temp;
  
  if (s == NULL) return (NULL);

  temp = strdup(s);
 
  for(c = temp; *c; c++)
    *c = toupper(*c);

  return (temp);
}

  
/* TKZ_SelectCmd --
 *
 *  Opens a Z39.50 Host for interaction. If the name, address, database
 *  and port are given as arguments, it adds the host to the hosts table
 *
 * Results:
 *	A standard Tcl result.
 *
 *----------------------------------------------------------------------
 */

int
TKZ_SelectCmd (inSession, interp, argc, argv)
     ClientData inSession;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  char *host_table_element;
  char **argvPtr = NULL;
  int  argcPtr;
  char *tempname;
  char *servername;
  char *hostaddress;
  char *databasename;
  int  portnumber;
  char *portstring;
  char connectionID[10];
  ZSESSION *SearchAssociation(), *tk_reconnect();
  GSTR *authentication = NULL, *NewGSTR();
  char *authstring;
  
  
  if (argc != 2 && argc != 3&& argc != 5 && argc != 6) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " servername {hostaddress databasename",
		     " portnumber} {idauthentication}\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  tempname = up_case_name(argv[1]);

  if (argc == 2 || argc == 3) {
    
    /* check the table of servers -- set up in TkZClientInit */
    
    if ((host_table_element = Tcl_GetVar2(interp, "Z_HOSTS", tempname,
					  TCL_GLOBAL_ONLY )) == NULL) {
      Tcl_AppendResult(interp, "Server name ", argv[1],
		       " not found in Z_HOSTS table --",
		       " use parray Z_HOSTS to see table",
		       (char *) NULL);
      return TCL_ERROR;
      
    }
    /* otherwise use the data in host_table_element */
    /* to set up the connection                     */
    if (Tcl_SplitList(interp, host_table_element, &argcPtr, &argvPtr) 
	== TCL_ERROR) {
      Tcl_AppendResult(interp, 
		       "Badly formed Z_HOSTS element for Server name ", 
		       argv[1],
		       " found in Z_HOSTS table --",
		       " use parray Z_HOSTS to see table",
		       (char *) NULL);
      return TCL_ERROR;
      
    }
    
    servername = argvPtr[0];
    hostaddress  = argvPtr[1];
    databasename = argvPtr[2];
    portstring = argvPtr[3];
    if (argc == 3) {
      authentication = NewGSTR(argv[2]);
      authstring = argv[2];
    }
    else 
      authstring = "";

    if (Tcl_GetInt(interp, argvPtr[3], &portnumber) == TCL_ERROR)
      return TCL_ERROR;
    
  }
  else if (argc == 5 || argc == 6) {  
    servername = tempname;
    hostaddress  = argv[2];
    databasename = argv[3];
    portstring = argv[4];
    if (Tcl_GetInt(interp, argv[4], &portnumber) == TCL_ERROR)
      return TCL_ERROR;
    if (argc == 6) {
      authentication = NewGSTR(argv[5]);
      authstring = argv[5];
    }
    else
      authstring = "";
  } 

  /* check to see if this server is in the table, and if not, add it */
  if ((host_table_element = Tcl_GetVar2(interp, "Z_HOSTS", servername,
					TCL_GLOBAL_ONLY )) == NULL) {
    Tcl_SetVar2(interp,"Z_HOSTS",servername, servername, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT );
    Tcl_SetVar2(interp,"Z_HOSTS",servername, hostaddress, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    Tcl_SetVar2(interp,"Z_HOSTS",servername, databasename, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    Tcl_SetVar2(interp,"Z_HOSTS",servername, portstring, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    Tcl_SetVar2(interp,"Z_HOSTS",servername, authstring, 
		TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
  }
  
  /* set the requested items in the current Session structure */
  
  /* If there is already an active connection to the same host machine
   * at the same port, return the connection id (i.e. the file descriptor
   * for the connection). However multiple connections to the same machine
   * with different port addresses are allowed.
   */
  if ((TKZ_User_Session = 
       SearchAssociation(&zAssociations, hostaddress, 
			 portnumber, databasename))!= NULL) {
    if (TKZ_User_Session->connectionStatus == Z_OPEN) {
      sprintf(connectionID, "%d", TKZ_User_Session->fd);
      Tcl_AppendResult(interp, connectionID, 
		       " {Connection with ", servername, " (",
		       hostaddress, ") database '",
		       databasename, "' at port ", portstring,
		       " is open as connection #", connectionID, "}", 
		       (char *) NULL);

#ifdef WIN32
      if (TKZ_User_Session->c_timeOut == 0) {
	TKZ_User_Session->c_timeOut = 6000;

      }
#endif
      return TCL_OK;
    } else {		/* connection has been shut down.	*/
      /* might want to log the host switch here */
      TKZ_User_Session = tk_reconnect(TKZ_User_Session, authentication);
      if (TKZ_User_Session != NULL
	  && TKZ_User_Session->state == S_OPEN) {
	sprintf(connectionID, "%d", TKZ_User_Session->fd);
	Tcl_AppendResult(interp, connectionID, 
			 " {Reconnection with ", servername, " (",
			 hostaddress, ") database '",
			 databasename, "' at port ", portstring,
			 " is open as connection #", connectionID, "}", 
			 (char *) NULL);
	TKZ_User_Session->connectionStatus = Z_OPEN;
	return TCL_OK;
      }
    }
  }

  /* connect to the server requested - make it the current session */
  TKZ_User_Session = tk_connect(hostaddress, portnumber, authentication);
  
  if (TKZ_User_Session == NULL) {	/* happens only when out of memory. */
    Tcl_AppendResult(interp, "out of memory" , (char *) NULL);
    return TCL_ERROR;
  } 
  
  if (TKZ_User_Session->status != Z_OK) {
    if (TKZ_User_Session->connectionStatus == Z_CLOSE)
      Tcl_AppendResult(interp, 
		       "-1 {Couldn't open connection to ",
		       argv[1], "}", (char *) NULL);
    else
      Tcl_AppendResult(interp, 
		       "-2 {Did not get Z39.50 INIT response from ",
		       argv[1], "}", (char *) NULL);


    free ((char *)TKZ_User_Session);
    TKZ_User_Session = NULL;

    return TCL_ERROR;
  }
  
  /* seem to be open for business...*/
  TKZ_User_Session->connectionStatus = Z_OPEN;

  /* might want to log the host switch here */
  if (LOGFILE == NULL && LOGGING_ON) { /* open the log file */
    TKZ_User_Session->s_logFileName = LOGFILENAME;
    if ((TKZ_User_Session->s_lfptr = 
	 fopen(TKZ_User_Session->s_logFileName,"w")) != NULL)
      LOGFILE = TKZ_User_Session->s_lfptr;
    else {
      LOGFILE = stderr;
      TKZ_User_Session->s_lfptr = stderr;
    }
  }
  else {
    TKZ_User_Session->s_lfptr = LOGFILE;
  }
  
  
  

  sprintf(connectionID, "%d", TKZ_User_Session->fd);
  Tcl_AppendResult(interp, connectionID, 
		   " {Connection with ", servername, " (",
		   hostaddress, ") database '",
		   databasename, "' at port ", portstring,
		   " is open as connection #", connectionID, "}", 
		   (char *) NULL);
  
  /* link the session info into the associations list */
  TKZ_User_Session->prev = NULL;
  TKZ_User_Session->next = NULL;
  if (zAssociations.head == NULL) {
    zAssociations.head = TKZ_User_Session;
    zAssociations.tail = zAssociations.head;
    zAssociations.counts = 1;
  } else {
    TKZ_User_Session->prev = zAssociations.tail;
    zAssociations.tail->next = TKZ_User_Session;
    zAssociations.tail = TKZ_User_Session;
    zAssociations.counts += 1;
  }
  
  /* set file descriptor mask for select matching */
  FD_SET(TKZ_User_Session->fd, &(zAssociations.active_fd_set));
  zAssociations.no_fd_set += 1;
  if (TKZ_User_Session->fd > zAssociations.maximum_fd)
    zAssociations.maximum_fd = TKZ_User_Session->fd;
  
  /* set the (default) database name */
  strcpy(TKZ_User_Session->databaseName,databasename);
  
  if (argvPtr)
    Tcl_Free((char *)argvPtr);

  if (tempname)
    free(tempname);
  
  return TCL_OK;
  
}


/* TKZ_CloseCmd --
 *
 *  Handles the ZCLOSE tcl command, closing a z39.50 session.
 *  
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_CloseCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  ZSESSION *sess;
  int sessionid;


  if (argc == 1) {

    if (TKZ_User_Session == NULL 
	|| TKZ_User_Session->connectionStatus != Z_OPEN)
      return TCL_OK;

    /* close the current session */
    if(tk_disconnect(TKZ_User_Session) == Z_OK) {
	  if (TKZ_User_Session->s_serverMachineName != NULL)
          Tcl_AppendResult(interp, "Connection with ", 
		       TKZ_User_Session->s_serverMachineName,
		       " was closed", (char *) NULL);
      /* DON'T free the session structure here -- may want to re-open */
      TKZ_User_Session = NULL; /* indicates no current session */
      TKZ_User_Session_TMP = NULL;
      return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, "Error in closing connection", (char *) NULL);
      return TCL_ERROR;
    }
  }
  else if (argc == 2) { /* close a session by session ID or 'ALL' */
    if (strcasecmp("ALL", argv[1]) == 0) {
      /* go through the entire session list and disconnect */
      for (sess = zAssociations.head; sess != NULL; sess = sess->next) {
	if(tk_disconnect(sess) == Z_OK) {
	  Tcl_AppendResult(interp, "Connection with ", 
			   sess->s_serverMachineName,
			   " was closed\n", (char *) NULL);
	}
	else {
	  Tcl_AppendResult(interp, "Error in closing connection with ",
			   sess->s_serverMachineName,
			   " -- No current session active\n", (char *) NULL);
	}
      }
      return TCL_OK;
    }
    else {
      /* must be a request to close a non_current session */
      if (Tcl_GetInt(interp, argv[1], &sessionid) == TCL_ERROR) {
	Tcl_AppendResult(interp, "Error in ZCLOSE: should be \"", argv[0],
			 " <sessionIDnumber | ALL>\"", (char *) NULL);
	return TCL_ERROR;
      }
      sess = LookupAssociation(&zAssociations, sessionid);
      if(tk_disconnect(sess) == Z_OK) {
	Tcl_AppendResult(interp, "Connection with ", 
			 sess->s_serverMachineName,
			 " was closed", (char *) NULL);
      }
      else {
	Tcl_AppendResult(interp, "Error in closing connection with ",
			 sess->s_serverMachineName,
			 " -- No current session active", (char *) NULL);
      }
    }
    return TCL_OK;
  }
  else { /* wrong number of args */
    Tcl_AppendResult(interp, "Error in ZCLOSE: should be \"", argv[0],
		     " <sessionIDnumber | ALL>\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  return TCL_OK;  

}


/* TKZ_DisplayCmd --
 *
 *  Handles the ZDISPLAY tcl command, calling present to retrieve
 *  Z39.50 records from the remote host.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_DisplayCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int Result;
  char resultswork[1000];
  int fudge = 0;
  

  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, "There is no current connection to ", 
		     " any servers -- use ZSELECT or zConnect.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (TKZ_User_Session->connectionStatus != Z_OPEN) {
    Tcl_AppendResult(interp, "Connection to ", 
		     TKZ_User_Session->s_serverMachineName,
		     " is closed, you need to reestablish the connection.",
		     (char *) NULL);
    return TCL_ERROR;
  }
  if (TKZ_User_Session->s_supportPresent != Z_TRUE) {
    Tcl_AppendResult(interp, "Server ", 
		     TKZ_User_Session->s_serverMachineName,
		     " doesn't support present.",
		     (char *) NULL);
	return TCL_ERROR;
    }
  if (TKZ_User_Session->searchStatus == 0) {
    Tcl_AppendResult(interp, "There are no records to be presented. ",
		     "A successful search must be done first.", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (argc > 5) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " <resultsetid> <n_recs> <start_rec>\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }


  if (argc > 1 && isalpha(*argv[1])) {
    if (strcmp(TKZ_User_Session->resultSetName,argv[1]) != 0)
      result_set_changed = 1;
    else
      result_set_changed = 0;

    strncpy(TKZ_User_Session->resultSetName, argv[1], MAX_RSN_LENGTH);
    fudge = 1;
    argc--;
  }

  if (argc >= 2)
    TKZ_User_Session->numberOfRecordsRequested = atoi(argv[1+fudge]);

  if (argc >= 3) 
    TKZ_User_Session->startPosition = atoi(argv[2+fudge]);

  if ((Result = TKZ_Present(TKZ_User_Session)) != Z_OK) {
    sprintf(resultswork, 
	    "ERR {Status -1} {Received 0} {Position %d} {Set %s} {NextPosition %d}",
	    TKZ_User_Session->startPosition, 
	    TKZ_User_Session->resultSetName,
	    TKZ_User_Session->nextResultSetPosition);  
    Tcl_AppendElement(interp, resultswork);
    if (TKZ_User_Session->diagRec) {
      if (TKZ_User_Session->diagRec->addinfo.u.v2Addinfo) {
        sprintf( resultswork, "Present failed: %s",
                 TKZ_User_Session->diagRec->addinfo.u.v2Addinfo->data );
      } else {
        sprintf( resultswork, "Present failed: Server diagnostic condition %d", TKZ_User_Session->diagRec->condition );
      }
      Tcl_AppendElement(interp, resultswork);
    } 
    else { /* local error */
      sprintf( resultswork, "Present failed: %s", 
	      TKZ_User_Session->errorMessage );
      Tcl_AppendElement(interp, resultswork);
    }
    return TCL_ERROR;
  }
  if (Result != Z_OK) {
    sprintf(resultswork, 
	    "ERR {Status 0} {Received 0} {Position %d} {Set %s} {NextPosition %d}",
	    TKZ_User_Session->startPosition, 
	    TKZ_User_Session->resultSetName,
	    TKZ_User_Session->nextResultSetPosition);  
    Tcl_AppendElement(interp, resultswork);
    Tcl_AppendElement(interp, "No more records in result set");
    return TCL_ERROR;

  }
  if(TKZ_User_Session->noRecordsReturned > 0)
    TKZ_DisplayRecords(TKZ_User_Session, interp, 1);	
  return TCL_OK;

}

/* TKZ_SearchCmd --
 *
 *  Handles the ZFIND tcl command, calling search to retrieve
 *  Z39.50 records from the remote host, or at least build a
 *  result set if records are found.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_SearchCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int i;
  char resultswork[10000];

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " indexname1 search_string1 <boolop> <indexname2>",
		       " <search_string2> <boolop2> etc...\"", (char *) NULL);
	return TCL_ERROR;
      }

  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, "There is no current connection to ", 
		     " any servers -- use ZSELECT or zConnect.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (TKZ_User_Session->connectionStatus != Z_OPEN) {
    Tcl_AppendResult(interp, "Connection to ", 
		     TKZ_User_Session->s_serverMachineName,
		     " is closed, you need to reestablish the connection.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  /* clear the session structure of previous search information */
  ClearPreviousSearch(TKZ_User_Session);
  
  /* build the query string -- just concatenate the args for this search */
  strcpy(resultswork, argv[1]);
  for (i = 2; i < argc; i++) {
    strcat (resultswork," ");
    strcat (resultswork, argv[i]);
  }
  TKZ_User_Session->queryString = strdup(resultswork);
  
  if (TKZ_Search(TKZ_User_Session)==(Z_ERROR)) {
    if (TKZ_User_Session->state == S_RESOURCE_CONTROL_RECEIVED) {
      sprintf( resultswork, "ERR {Status 1} {Hits 0} {Received 0} {Set %s}",
	      TKZ_User_Session->resultSetName );
      Tcl_AppendElement( interp, resultswork );
      sprintf( resultswork, 
	      "Resource Control Request received -- Continue Search? (use zContinue)", 
	      TKZ_User_Session->errorMessage);
      Tcl_AppendElement( interp, resultswork );
    }
    else {
      sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	      TKZ_User_Session->resultSetName );
      Tcl_AppendElement( interp, resultswork );
      sprintf( resultswork, "Search failure: %s", 
	      TKZ_User_Session->errorMessage);
      Tcl_AppendElement( interp, resultswork );
    }
    return TCL_ERROR;
  }
  
  /* handle successful search results (and diagnostics)*/
  return (TKZ_SearchResults(TKZ_User_Session, interp));

}

/* TKZ_SQLSearchCmd --
 *
 *  Handles the ZQL tcl command, submitting a type-0 SQL query 
 *  to the remote host, or at least build a result set if records are found.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_SQLSearchCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int i;
  char resultswork[1000];
  Query *Z3950Query;
  PDU *RequestPDU, *ResponsePDU; 
  ElementSetNames *ssesn, *msesn;
  char **databaseNames;
  extern char *LastResultSetID;

  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " SQL_QUERY_STATEMENT\"", (char *) NULL);
	return TCL_ERROR;
      }

  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, "There is no current connection to ", 
		     " any servers -- use ZSELECT or zConnect.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  /* clear the session structure of previous search information */
  ClearPreviousSearch(TKZ_User_Session);
  
  /* build the query string -- just concatenate the args for this search */
  strcpy(resultswork, argv[1]);
  for (i = 2; i < argc; i++) {
    strcat (resultswork," ");
    strcat (resultswork, argv[i]);
  }
  TKZ_User_Session->queryString = strdup(resultswork);

  if(strlen(TKZ_User_Session->queryString)==0) {
    strcpy(TKZ_User_Session->errorMessage,
	   "You must supply a valid query term(s)");
    sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	     TKZ_User_Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Search failure: %s", 
	     TKZ_User_Session->errorMessage);
    Tcl_AppendElement( interp, resultswork );
    
    return TCL_ERROR;
  }

  /* build a type 0 query */
  Z3950Query = MakeQueryType0(TKZ_User_Session->queryString);

  if (LastResultSetID) {
    /* resultset name included with query */
    strcpy (TKZ_User_Session->resultSetName, LastResultSetID);
  } /* otherwise leave it as is */

  /* convert the string of database names (or a single name) */
  /* to a null-terminated list.                              */
  databaseNames = StringToArgv (TKZ_User_Session->databaseName);

  /* could add element set specifications --- later */
  ssesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_smallElementSetNames);
  msesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_mediumElementSetNames);
  
  

  /* Build the search request PDU */
  RequestPDU = MakeSearchRequest("SEARCH_REFID",  /* reference ID */
				 TKZ_User_Session->c_smallSetUpperBound,
				 TKZ_User_Session->c_largeSetLowerBound,
				 TKZ_User_Session->c_mediumSetPresentNumber,
				 Z_TRUE, /* replace indicator */
				 TKZ_User_Session->resultSetName,
				 databaseNames, /* list of databases */
				 ssesn,		/*	ssesn	*/
				 msesn,		/*	msesn	*/
				 TKZ_User_Session->preferredRecordSyntax,
				 Z3950Query, NULL);

  if (RequestPDU == NULL) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to create a search request apdu.");
    sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	     TKZ_User_Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Search failure: %s", 
	     TKZ_User_Session->errorMessage);
    Tcl_AppendElement( interp, resultswork );
    
    return TCL_ERROR;
  }
  if (PutPDU (TKZ_User_Session->fd, RequestPDU) != Z_OK) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to transmit search request. Connection may have closed.");
    sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	     TKZ_User_Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Search failure: %s", 
	     TKZ_User_Session->errorMessage);
    Tcl_AppendElement( interp, resultswork );
    
    return TCL_ERROR;
  }
  LogPDU(TKZ_User_Session->s_lfptr, RequestPDU);

  (void) FreePDU (RequestPDU);
  (void) FreeArgv(databaseNames);
  TKZ_User_Session->state = S_SEARCH_SENT;
  
  do {
    if ((ResponsePDU = ClientGetPDU(TKZ_User_Session->fd, TKZ_User_Session->c_timeOut)) == NULL) {
      strcpy(TKZ_User_Session->errorMessage, 
	     "Didn't get a search response pdu. Connection closed.");
      /* something serious happened. close the connection.
       */
      tk_disconnect(TKZ_User_Session);
      TKZ_User_Session->connectionStatus = Z_CLOSE;

      sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	       TKZ_User_Session->resultSetName );
      Tcl_AppendElement( interp, resultswork );
      sprintf( resultswork, "Search failure: %s", 
	       TKZ_User_Session->errorMessage);
      Tcl_AppendElement( interp, resultswork );
      
      return TCL_ERROR;
    }

    LogPDU(TKZ_User_Session->s_lfptr, ResponsePDU);

    if (ResponsePDU->which  == e1_resourceControlRequest) {	
#ifdef DEBUGGING
      LogPDU (stderr, ResponsePDU);
#endif
      TKZ_User_Session->state = S_RESOURCE_CONTROL_RECEIVED;
      sprintf( resultswork, "ERR {Status 1} {Hits 0} {Received 0} {Set %s}",
	      TKZ_User_Session->resultSetName );
      Tcl_AppendElement( interp, resultswork );
      sprintf( resultswork, 
	      "Resource Control Request received -- Continue Search? (use zContinue)", 
	      TKZ_User_Session->errorMessage);
      Tcl_AppendElement( interp, resultswork );
      return TCL_ERROR;
    } 
  } while (ResponsePDU->which != e1_searchResponse);

#ifdef DEBUGGING
  LogPDU(stderr,ResponsePDU);
#endif
  TKZ_User_Session->state = S_SEARCH_RECEIVED;

  if (ProcessSearchResponse (TKZ_User_Session, ResponsePDU->u.searchResponse) != Z_OK){
    strcpy(TKZ_User_Session->errorMessage, 
	   "Error in processing search response.");
    (void) FreePDU (ResponsePDU);

    sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	     TKZ_User_Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Search failure: %s", 
	     TKZ_User_Session->errorMessage);
    Tcl_AppendElement( interp, resultswork );
    
    return TCL_ERROR;
  }

  /* Should be all set now...*/
  (void) FreePDU (ResponsePDU);
  
  /* handle successful search results (and diagnostics)*/
  return (TKZ_SearchResults(TKZ_User_Session, interp));

}


/* TKZ_SearchResults --
 *
 *  Handles error messages and result reports from a search response
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Negative numbers on error
 *      1 on success
 *
 *----------------------------------------------------------------------
 */  

/* Totally dependant on session parameters. */
int TKZ_SearchResults(ZSESSION *Session, Tcl_Interp *interp) 
{
  char resultswork[500];
  char *diagstring;
  char oid_string[200];


  if (Session->diagRecPresent) { /* Diagnostic message returned */
    sprintf(resultswork, "ERR {Status %d} {Hits %d} {Received %d} {Set %s}",
	    Session->searchStatus, Session->resultCount, 
	    Session->noRecordsReturned, 
	    Session->resultSetName);
    Tcl_AppendElement( interp, resultswork );
    
    /* Its a nonSurrogateDiagnostic -- grab the message part of it...*/
    if (Session->diagRec == NULL) 
      diagstring = strdup("Unknown Server Error");
    else
      diagstring =
	GetOctetString((OctetString)
		       Session->diagRec->addinfo.u.v3Addinfo);
    if (diagstring[0] == '\0') {
      /* use the code to lookup the string */
      free(diagstring);
      diagstring = strdup(diagnostic_string(Session->diagRec->condition));
    }
    Tcl_AppendElement( interp, diagstring);
    free(diagstring);
    return TCL_ERROR;
  }  
#ifdef OLDRESULTHANDLING
  if (Session->resultCount==0) { /* No hits */
    sprintf( resultswork, "ERR {Status %d} {Hits 0} {Received 0} {Set %s}",
	     Session->searchStatus, Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    Tcl_AppendElement( interp, "No matching records found" );
    return TCL_ERROR;
  }
#endif  
  /* One or more hits */

  /* Try to set recsyntax from first document */
  sprintf(oid_string, "UNKNOWN");
  

  if (Session->documentList != NULL && Session->documentList[1] != NULL) {
    /* there may be other formats, but not in the standard ...    */
    switch (Session->documentList[1]->dtype) {
    case Z_US_MARC:		/* USMARC record	  */    
      sprintf(oid_string, "MARC %s", MARCRECSYNTAX);
      break;

    case Z_OPAC_REC:             /* OPAC record syntax     */
      sprintf(oid_string, "OPAC %s",OPACRECSYNTAX);
      break;


    case Z_SUTRS_REC:            /* SUTRS record syntax    */
      sprintf(oid_string, "SUTRS %s",SUTRECSYNTAX);
      break;
      

    case Z_SGML_REC:            /* SGML record syntax    */
      sprintf(oid_string, "SGML %s",SGML_RECSYNTAX);
      break;
      
      
    case Z_XML_REC:            /* XML record syntax    */
      sprintf(oid_string, "XML %s",XML_RECSYNTAX);
      break;
      
      
    case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
      sprintf(oid_string, "EXPLAIN %s",EXPLAINRECSYNTAX);
      break;
      
      
    case Z_GRS_0_REC:            /* Generic record syntax 0*/
      sprintf(oid_string, "GRS0 %s",GRS0RECSYNTAX);
      break;
      

    case Z_GRS_1_REC:            /* Generic record syntax 1*/
      sprintf(oid_string, "GRS1 %s",GRS1RECSYNTAX);
      break;
 

    case Z_ES_REC:               /* Extended Services      */
      sprintf(oid_string, "ES %s",ESRECSYNTAX);
      break;
      
      
    case Z_SUMMARY_REC:          /* Summary record syntax  */
      sprintf(oid_string, "SUMMARY %s",SUMMARYRECSYNTAX);
      break;
      
    }
    
  }



  sprintf(resultswork, "OK {Status %d} {Hits %d} {Received %d} {Set %s} {RecordSyntax %s}",
	  Session->searchStatus, Session->resultCount, 
	  Session->noRecordsReturned, 
	  Session->resultSetName, oid_string);
  Tcl_AppendElement(interp, resultswork);
  /* record the resultset name */
  if (ac_list_search(Session->c_resultSetNames, Session->resultSetName) != 1) {
    
    if (Session->c_resultSetNames == NULL) 
      Session->c_resultSetNames = ac_list_alloc();
    ac_list_add(Session->c_resultSetNames, Session->resultSetName);
  }
  /* output all the records returned */
  if(Session->noRecordsReturned > 0 && Session->documentList == NULL) {
    /* Another error with badly behaved servers... */
    sprintf( resultswork, "ERR {Status %d} {Hits 0} {Received 0} {Set %s}",
	     99, Session->resultSetName );
    Tcl_ResetResult(interp);
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Server reports %d records returned, but no data was received", Session->noRecordsReturned);
    Tcl_AppendElement( interp, resultswork );
    return TCL_ERROR;    
  }
  else if (Session->noRecordsReturned > 0)
    TKZ_DisplayRecords(Session, interp, 0);	

  return TCL_OK;

}

/* TKZ_Search --
 *
 *  Builds and sends the search PDU to retrieve
 *  Z39.50 records from the remote host.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Negative numbers on error
 *      1 on success
 *
 *----------------------------------------------------------------------
 */

/* Totally dependant on session parameters. */
int TKZ_Search(ZSESSION *Session) {
  RPNQuery *sRPNQuery;
  Query *Z3950Query;
  PDU *RequestPDU, *ResponsePDU;
  ElementSetNames *ssesn, *msesn;
  char **databaseNames;
  /* these are set in command (query) parsing */
  extern int parse_errors;
  extern char *LastResultSetID;
  OtherInformation *addsearchinfo = NULL;
  

  if (Session == NULL)
    return Z_ERROR;

  if(strlen(Session->queryString)==0) {
    strcpy(Session->errorMessage,"You must supply a valid query term(s)");
    return(Z_ERROR);
  
  }
  /* call the command language parser to build an RPN query */
  sRPNQuery = queryparse(Session->queryString, Session->c_attributeSetId);

  /* any problems are signaled by the external value parse_errors */
  if (sRPNQuery == NULL) {
    strcpy(Session->errorMessage,"Syntax error in query");
    return(Z_ERROR);
  }

  Z3950Query = MakeQueryType1(sRPNQuery, NULL);/* attributes already set */

  if (LastResultSetID) {
    /* resultset name included with query */
    strcpy (Session->resultSetName, LastResultSetID);
  } /* otherwise leave it as is */

  /* convert the string of database names (or a single name) */
  /* to a null-terminated list.                              */
  databaseNames = StringToArgv (Session->databaseName);

  /* could add element set specifications --- later */
  ssesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_smallElementSetNames);
  msesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_mediumElementSetNames);

  if (Session->SearchAddInfo[0] != '\0') {
    addsearchinfo = CALLOC(OtherInformation, 1);
    if (Session->SearchAddInfoType == 1) {
      addsearchinfo->item.information.which = e24_oid;
      addsearchinfo->item.information.u.oid = NewOID(Session->SearchAddInfo);
    }
    else {
      addsearchinfo->item.information.which = e24_characterInfo;
      addsearchinfo->item.information.u.characterInfo = 
	NewInternationalString(Session->SearchAddInfo);
    }
  }

  /* Build the search request PDU */
  RequestPDU = MakeSearchRequest("SEARCH_REFID",  /* reference ID */
				 Session->c_smallSetUpperBound,
				 Session->c_largeSetLowerBound,
				 Session->c_mediumSetPresentNumber,
				 Z_TRUE, /* replace indicator */
				 Session->resultSetName,
				 databaseNames, /* list of databases */
				 ssesn,		/*	ssesn	*/
				 msesn,		/*	msesn	*/
				 Session->preferredRecordSyntax,
				 Z3950Query, addsearchinfo);

  if (RequestPDU == NULL) {
    strcpy(Session->errorMessage, 
	   "Failed to create a search request apdu.");
    return Z_ERROR;
  }
  if (PutPDU (Session->fd, RequestPDU) != Z_OK) {
    strcpy(Session->errorMessage, 
	   "Failed to transmit search request. Connection may have closed.");
    return Z_ERROR;	
  }
  LogPDU(Session->s_lfptr, RequestPDU);

  (void) FreePDU (RequestPDU);
  (void) FreeArgv(databaseNames);
  Session->state = S_SEARCH_SENT;
  
  do {
    if ((ResponsePDU = ClientGetPDU(Session->fd,Session->c_timeOut)) == NULL) {
      strcpy(Session->errorMessage, 
	     "Didn't get a search response pdu. Connection closed.");
      /* something serious happened. close the connection.
       */
      tk_disconnect(Session);
      Session->connectionStatus = Z_CLOSE;
      return Z_ERROR;
    }
    if (ResponsePDU->which  == e1_resourceControlRequest) {	
#ifdef DEBUGGING
      LogPDU (stderr, ResponsePDU);
#endif
      Session->state = S_RESOURCE_CONTROL_RECEIVED;
      return Z_ERROR;
    } 
  } while (ResponsePDU->which != e1_searchResponse);

#ifdef DEBUGGING
  LogPDU(stderr,ResponsePDU);
#endif
  Session->state = S_SEARCH_RECEIVED;

  if (ProcessSearchResponse (Session, ResponsePDU->u.searchResponse) != Z_OK){
    strcpy(Session->errorMessage, 
	   "Error in processing search response.");
    (void) FreePDU (ResponsePDU);
    return Z_ERROR;
  }

  /* Should be all set now...*/
  (void) FreePDU (ResponsePDU);
  return Z_OK;
  
}



/* TKZ_SearchResults --
 *
 *  Handles error messages and result reports from a search response
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Negative numbers on error
 *      1 on success
 *
 *----------------------------------------------------------------------
 */  

/* Totally dependant on session parameters. */
int TKZ_ScanResults(ZSESSION *Session, ScanResponse *scanResponse, 
		    Tcl_Interp *interp) 
{
  char resultswork[500];
  char *diagstring;
  char *term;
  ListEntries *le;
  struct entries_List13 *entries;
  TermInfo *ti;
  
  le = scanResponse->entries;
  
  if (scanResponse->scanStatus == 6) { /* Diagnostic message returned */
    sprintf(resultswork, "ERR {Status %d} {Terms 0} {StepSize 0}",
	    scanResponse->scanStatus);
    Tcl_AppendElement( interp, resultswork );
    
    /* Its a nonSurrogateDiagnostic -- grab the message part of it...*/
    if (le != NULL) {
      if (le->nonsurrogateDiagnostics) {

	diagstring = diagnostic_string(le->nonsurrogateDiagnostics->item->u.defaultFormat->condition);
	Tcl_AppendElement( interp, diagstring);

	diagstring =
	  GetOctetString((OctetString)
			 le->nonsurrogateDiagnostics->item->u.defaultFormat->addinfo.u.v3Addinfo);
	Tcl_AppendElement( interp, diagstring);
	free(diagstring);
	
	return TCL_ERROR;
      }
    }
    else { /* No diagnostic returned!!! */
      Tcl_AppendElement( interp, "No diagnostic information provided by server");
      return TCL_ERROR;
    }
  }  
  
    /* One or more hits */
  sprintf(resultswork, 
	  "SCAN {Status %d} {Terms %d} {StepSize %d} {Position %d}",
	  scanResponse->scanStatus, scanResponse->numberOfEntriesReturned, 
	  scanResponse->stepSize, 
	  scanResponse->positionOfTerm); 
  Tcl_AppendElement(interp, resultswork);
  
  if (le != NULL) {
    for (entries = le->entries; entries; entries = entries->next) {
      
      if (entries->item->which == e19_surrogateDiagnostic){
	diagstring =
	  GetOctetString((OctetString)
			 entries->item->u.surrogateDiagnostic->u.defaultFormat->addinfo.u.v3Addinfo);
	Tcl_AppendElement( interp, diagstring);
	free(diagstring);
	
      }
      else {
	char *tempbuff;
	ti = entries->item->u.termInfo;
	
	if (ti->term != NULL && ti->displayTerm == NULL) {
	  tempbuff = CALLOC(char, ti->term->u.general->length + 50);
	  term = GetOctetString(ti->term->u.general);
	}
	else if (ti->displayTerm != NULL) {
	  tempbuff = CALLOC(char, ti->displayTerm->length + 50);
	  term = GetOctetString(ti->displayTerm);
	}
	sprintf (tempbuff, "%s %d", term, ti->globalOccurrences);
	Tcl_AppendElement(interp, tempbuff);

	free(term);
	free(tempbuff);
      }
    }
  }
  
  return TCL_OK;

}


/* TKZ_ScanCmd --
 *
 *  Handles the ZSCAN tcl command, calling search to retrieve
 *  Z39.50 records from the remote host, or at least build a
 *  result set if records are found.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_ScanCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  int Result;
  char *index_name, *term;
  int stepsize, number_of_terms, preferred_position;
  char **databaseNames;
  PDU *RequestPDU, *ResponsePDU;
  RPNQuery *sRPNQuery;
  AttributesPlusTerm *attrterms;
  char querystring[500];

  if (argc < 6) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " indexname1 term stepsize number_of_terms preferred_position",
		       "\"", (char *) NULL);
	return TCL_ERROR;
      }

  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, "There is no current connection to ", 
		     " any servers -- use ZSELECT or zConnect.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  index_name = argv[1];
  term = argv[2];
  stepsize = atoi(argv[3]);
  number_of_terms = atoi(argv[4]);
  preferred_position = atoi(argv[5]);


  sprintf(querystring, "%s {%s}", index_name, term);

  /* call the command language parser to build an RPN query */
  sRPNQuery = queryparse(querystring, TKZ_User_Session->c_attributeSetId);

  /* any problems are signaled by the external value parse_errors */
  if (sRPNQuery == NULL) {
    strcpy(TKZ_User_Session->errorMessage,"Unable to match index");
    return(Z_ERROR);
  }

  attrterms = sRPNQuery->rpn->u.op->u.attrTerm;

  free(sRPNQuery->rpn);
  free(sRPNQuery);


  /* convert the string of database names (or a single name) */
  /* to a null-terminated list.                              */
  databaseNames = StringToArgv (TKZ_User_Session->databaseName);
  


  /* Build the scan request PDU */
  RequestPDU = MakeScanRequest("SCAN_REFID", databaseNames,
			       TKZ_User_Session->c_attributeSetId, attrterms,
			       stepsize, number_of_terms, preferred_position);


  if (RequestPDU == NULL) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to create a scan request apdu.");
    return Z_ERROR;
  }
  if (PutPDU (TKZ_User_Session->fd, RequestPDU) != Z_OK) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to transmit scan request. Connection may have closed.");
    return Z_ERROR;	
  }
  LogPDU(TKZ_User_Session->s_lfptr, RequestPDU);

  (void) FreePDU (RequestPDU);
  (void) FreeArgv(databaseNames);
  TKZ_User_Session->state = S_SCAN_SENT;
  
  do {
    if ((ResponsePDU = ClientGetPDU(TKZ_User_Session->fd,TKZ_User_Session->c_timeOut)) == NULL) {
      strcpy(TKZ_User_Session->errorMessage, 
	     "Didn't get a search response pdu. Connection closed.");
      /* something serious happened. close the connection.
       */
      tk_disconnect(TKZ_User_Session);
      TKZ_User_Session->connectionStatus = Z_CLOSE;
      return Z_ERROR;
    }
    if (ResponsePDU->which  == e1_resourceControlRequest) {	
      TKZ_User_Session->state = S_RESOURCE_CONTROL_RECEIVED;
      return Z_ERROR;
    } 
  } while (ResponsePDU->which != e1_scanResponse);

  TKZ_User_Session->state = S_SCAN_RECEIVED;

  /* handle successful scan results (and diagnostics)*/  
  Result = TKZ_ScanResults(TKZ_User_Session, 
			   ResponsePDU->u.scanResponse, interp);

  /* Should be all set now...*/
  (void) FreePDU (ResponsePDU);

  return (Result);

}


/* TKZ_ResourceCmd --
 *
 *  Handles the zResource tcl command, passing along the OK or NO
 *  and resulting in getting Z39.50 records from the remote host, 
 *  or at least build a result set if records are found.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_ResourceCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  PDU *RequestPDU, *ResponsePDU = NULL;
  ZSESSION *Session = TKZ_User_Session; /* assume current session */
  char resultswork[500];

  /* any response is assumed to be a continue */
  RequestPDU = MakeResourceControlResponse(NewOctetString("RESOURCE_REFID"), 1, 1);
#ifdef DEBUGGING
  LogPDU (stderr,RequestPDU); 
#endif
  PutPDU (Session->fd, RequestPDU);
  (void) FreePDU (RequestPDU);

  do {
    /* in case of further resource prompts */
    if (ResponsePDU != NULL 
	&& ResponsePDU->which  == e1_resourceControlRequest) {
      if (ResponsePDU->u.resourceControlRequest->responseRequired != 0) {
	RequestPDU = MakeResourceControlResponse(NewOctetString("RESOURCE_REFID"), 1, 1);
#ifdef DEBUGGING
	LogPDU (stderr, RequestPDU); 
#endif
	PutPDU (Session->fd, RequestPDU);
	(void) FreePDU (RequestPDU);
      }
    }
    /* get the result -- SHOULD be a search response */
    if ((ResponsePDU = ClientGetPDU(Session->fd,Session->c_timeOut)) == NULL) {
      sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	      TKZ_User_Session->resultSetName );
      Tcl_AppendElement( interp, resultswork );
      sprintf( resultswork, "Search Continuation failure: %s", 
	      TKZ_User_Session->errorMessage);
      Tcl_AppendElement( interp, resultswork );
      tk_disconnect(Session);
      Session->connectionStatus = Z_CLOSE;
      return TCL_ERROR;
    }
  } while (ResponsePDU->which != e1_searchResponse);

  Session->state = S_SEARCH_RECEIVED;

  /* now we treat it just like a search response */
#ifdef DEBUGGING
  LogPDU(stderr,ResponsePDU);
#endif
  if (ProcessSearchResponse (Session, ResponsePDU->u.searchResponse) != Z_OK){
    sprintf( resultswork, "ERR {Status -1} {Hits 0} {Received 0} {Set %s}",
	    TKZ_User_Session->resultSetName );
    Tcl_AppendElement( interp, resultswork );
    sprintf( resultswork, "Error in search response processing.", 
	    TKZ_User_Session->errorMessage);
    Tcl_AppendElement( interp, resultswork );
    return TCL_ERROR;
  }
  /* should be done with this one...*/
  (void) FreePDU (ResponsePDU);

  /* process the search result set */
  return (TKZ_SearchResults(Session, interp));

}


/* TKZ_Present --
 *
 *  Builds and sends the Present PDU to retrieve additional
 *  Z39.50 records from the remote host.
 *
 *  Totally dependant on Session parameters. 
 *
 * Results:
 *	Negative numbers on error
 *      1 on success
 *
 *----------------------------------------------------------------------
 */

/* Totally dependant on Session parameters. */
int TKZ_Present(ZSESSION *Session) {
  PDU *RequestPDU,*ResponsePDU;
  int recStartPoint = Session->startPosition;
  int numRecsRequested = Session->numberOfRecordsRequested;
  int OrigNumRecsRequested  = Session->numberOfRecordsRequested;
  int presentneeded = 1;
  OtherInformation *addsearchinfo = NULL;


  if (Session == NULL)
    return Z_ERROR;

  if (result_set_changed == 0) {
    /* the data in the session structure is current */
    /* Check for some potential errors */
    if(recStartPoint > Session->resultCount) {
      sprintf(Session->errorMessage, "Maximum record number is %d.",
	      Session->resultCount);
      return Z_ERROR;
    }

    /* if more are requested than are available, reduce the number requested */
    if(numRecsRequested > ((Session->resultCount-recStartPoint)+1))
      numRecsRequested = (Session->resultCount - recStartPoint) + 1;	
  }
  /* otherwise we will assume (for now) external tracking to avoid errors  */
  /* from the target -- This really should be handled better on the client */


  if(numRecsRequested <= 0)
    return Z_ERROR;
  

  if (Session->SearchAddInfo[0] != '\0') {
    addsearchinfo = CALLOC(OtherInformation, 1);
    if (Session->SearchAddInfoType == 1) {
      addsearchinfo->item.information.which = e24_oid;
      addsearchinfo->item.information.u.oid = NewOID(Session->SearchAddInfo);
    }
    else {
      addsearchinfo->item.information.which = e24_characterInfo;
      addsearchinfo->item.information.u.characterInfo = 
	NewInternationalString(Session->SearchAddInfo);
    }
  }
  
  Session->noRecordsReturned = 0; /* no records returned for this call yet */
    
  Session->startPosition = recStartPoint;
  Session->numberOfRecordsRequested = numRecsRequested;

  /* Build the PresentRequest PDU */
  RequestPDU = MakePresentRequest(
				  "PRESENT_REFID", 
				  Session->resultSetName, 
				  recStartPoint,
				  numRecsRequested,
				  /* providing only a single string */
				  /* instead of an array            */
				  /* it does handle this...         */
				  Session->presentElementSet,
				  Session->preferredRecordSyntax,
				  addsearchinfo);
  
  /* did the Make work?? */
  if (RequestPDU == NULL)
    return Z_ERROR;
  
  LogPDU(Session->s_lfptr, RequestPDU);
  
  /* transmit it to the server */
  Session->state = S_PRESENT_SENT;
  if (PutPDU (Session->fd, RequestPDU) != Z_OK)
    return Z_ERROR;
  
  /* don't need it any more */
  (void) FreePDU (RequestPDU);
  
  /* get the response from the server */
  if ((ResponsePDU = ClientGetPDU(Session->fd,Session->c_timeOut)) == NULL) {
    fprintf(stderr, "Failed to get a present response pdu.\n"); 
    tk_disconnect(Session);
    return Z_ERROR;
  }
  Session->state = S_PRESENT_RECEIVED;
  
  /* move the results into the Session structure */
  if (ProcessPresentResponse(Session, ResponsePDU->u.presentResponse) 
      != Z_OK) {
    (void) FreePDU (ResponsePDU);
    return Z_ERROR;
  }
  LogPDU(Session->s_lfptr,ResponsePDU); 
  
#ifdef DEBUGGING
  LogPDU(stderr,ResponsePDU);
#endif
  
  (void) FreePDU (ResponsePDU);
  
  return Z_OK;
}


/* TKZ_DisplayRecords --
 *
 *  Appends the records returned by a Search or Present to the TCL result
 *
 *  Totally dependant on Session parameters. 
 *
 * Results:
 *      none
 *
 *----------------------------------------------------------------------
 */

void TKZ_DisplayRecords(ZSESSION *Session, Tcl_Interp *interp, int type) {
  char resultswork[500];
  char oid_string[200];
  DOCUMENT *document;
  int i;
  
  /* Try to set recsyntax from first document */
  sprintf(oid_string, "UNKNOWN");
  
  document = Session->documentList[1];

  if (document != NULL) {
    /* there may be other formats, but not in the standard ...    */
    switch (document->dtype) {
    case Z_US_MARC:		/* USMARC record	  */    
      sprintf(oid_string, "MARC %s", MARCRECSYNTAX);
      break;

    case Z_OPAC_REC:             /* OPAC record syntax     */
      sprintf(oid_string, "OPAC %s",OPACRECSYNTAX);
      break;


    case Z_SUTRS_REC:            /* SUTRS record syntax    */
      sprintf(oid_string, "SUTRS %s",SUTRECSYNTAX);
      break;
      

    case Z_SGML_REC:            /* SGML record syntax    */
      sprintf(oid_string, "SGML %s",SGML_RECSYNTAX);
      break;
      
      
    case Z_XML_REC:            /* XML record syntax    */
      sprintf(oid_string, "XML %s",XML_RECSYNTAX);
      break;
      
      
    case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
      sprintf(oid_string, "EXPLAIN %s",EXPLAINRECSYNTAX);
      break;
      
      
    case Z_GRS_0_REC:            /* Generic record syntax 0*/
      sprintf(oid_string, "GRS0 %s",GRS0RECSYNTAX);
      break;
      

    case Z_GRS_1_REC:            /* Generic record syntax 1*/
      sprintf(oid_string, "GRS1 %s",GRS1RECSYNTAX);
      break;
 

    case Z_ES_REC:               /* Extended Services      */
      sprintf(oid_string, "ES %s",ESRECSYNTAX);
      break;
      
      
    case Z_SUMMARY_REC:          /* Summary record syntax  */
      sprintf(oid_string, "SUMMARY %s",SUMMARYRECSYNTAX);
      break;
      
    }
    
  }

  if (type) { /* set when this is a call from a present */

    sprintf(resultswork, 
	    "OK {Status %d} {Received %d} {Position %d} {Set %s} {NextPosition %d} {RecordSyntax %s}",
	    Session->presentStatus, Session->noRecordsReturned, 
	    Session->startPosition, 
	    Session->resultSetName,
	    Session->nextResultSetPosition, oid_string);  
      
    Tcl_AppendElement(interp, resultswork);
  }
  
#ifdef OLDVERSION
  for (i=Session->startPosition; 
       i < (Session->noRecordsReturned+Session->startPosition) 
       && i <= Session->totalNumberRecordsReturned; i++) {
#endif

  for (i=1; i <= Session->noRecordsReturned; i++) {
    document = Session->documentList[i];
    if (document != NULL) {
      if (document->dtype == Z_OPAC_REC) {
	if (document->data)
	  /* PrintOPACRecord(stdout, document->data); */
	  Tcl_AppendElement(interp, MakeOPACListElement(document->data));
      } 
      else if (document->dtype == Z_EXPLAIN_REC) {
	MakeExplainListElement(document->data,interp);
	/*
	Tcl_AppendElement(interp, MakeExplainListElement(document->data,interp));
	*/
      }
      else  if (document->dtype == Z_GRS_1_REC) {
	MakeGRS1Recs(document->data,interp);
      }
      else  if (document->dtype == Z_DIAG_REC) {
	PrintDiagRec(stderr, document->data);
      } 
      else if (document->dtype != Z_DIAG_REC) {
	if (document->data)
	  if (document->dsize > 0)
	    Tcl_AppendElement(interp, document->data);
      } 
      else {
	fprintf(stderr, "unknown document type.\n");
      }
    }
  }
  /* update the start position now  */
  Session->startPosition = Session->nextResultSetPosition;
  
}



/* TKZ_SortCmd --
 *
 *  Handles the ZSORT tcl command, sending a SORT request to the connected
 *  target system, and interpreting the results.
 *
 *  Totally dependant on session parameters. 
 *
 * Results:
 *	Normal TCL return
 *
 *----------------------------------------------------------------------
 */

int
TKZ_SortCmd(session, interp, argc, argv)
    ClientData session;			/* Current Session */
    Tcl_Interp *interp;			/* Current interpreter. */
    int argc;				/* Number of arguments. */
    char **argv;			/* Argument strings. */
{
  PDU *RequestPDU, *ResponsePDU;
  struct sortSequence_List18 *sort_head, *sort_tail, *sort_keys;
  struct SortKey *curr_sortkey;
  struct SortElement *curr_sortelement;
  struct SortKeySpec *curr_sortkeyspec;
  RPNQuery *sRPNQuery;
  char temp[500];
  int i;
  char *missing_value = NULL;
  char *in_result_string = NULL;
  char **in_results;
  char *out_results = NULL;
  char *sortstatusmsg, *resultsetmsg, *othermsg, *num;
  int result_set_size = 0;
  struct diagnostics_List19 *dl;

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

  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, "There is no current connection to ", 
		     " any servers -- use ZSELECT or zConnect.",
		     (char *) NULL);
    return TCL_ERROR;
  }

  sort_head = NULL;

  for (i = 1; i < argc; i++) {
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
	Tcl_AppendResult(interp, "-IN_RESULTS must be followed by a ",
			 "resultsetid (resultsetid is NULL)",
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
      sRPNQuery = queryparse(temp, TKZ_User_Session->c_attributeSetId);
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
    return TCL_ERROR;
  }

  if (in_result_string == NULL && out_results == NULL) {
    out_results = TKZ_User_Session->resultSetName;
    in_results = StringToArgv(TKZ_User_Session->resultSetName);
  }
  else if (in_result_string == NULL) {
    in_results = StringToArgv(TKZ_User_Session->resultSetName);
  }
  else if (out_results == NULL) {
    out_results = "Default";
    in_results = StringToArgv(in_result_string);
  }
  else {/* out_results already set */
    in_results = StringToArgv(in_result_string);
  }

  /* Build the sort request PDU */
  RequestPDU = MakeSortRequest("SORT_REFID", in_results, out_results,
			       sort_head);


  if (RequestPDU == NULL) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to create a sort request apdu.");
    return Z_ERROR;
  }
  if (PutPDU (TKZ_User_Session->fd, RequestPDU) != Z_OK) {
    strcpy(TKZ_User_Session->errorMessage, 
	   "Failed to transmit sort request. Connection may have closed.");
    return Z_ERROR;	
  }
  LogPDU(TKZ_User_Session->s_lfptr, RequestPDU);

  (void) FreePDU (RequestPDU);
  (void) FreeArgv(in_results);

  do {
    if ((ResponsePDU = ClientGetPDU(TKZ_User_Session->fd,TKZ_User_Session->c_timeOut)) == NULL) {
      strcpy(TKZ_User_Session->errorMessage, 
	     "Didn't get a sort response pdu. Connection closed.");
      /* something serious happened. close the connection.
       */
      tk_disconnect(TKZ_User_Session);
      TKZ_User_Session->connectionStatus = Z_CLOSE;
      return Z_ERROR;
    }
  } while (ResponsePDU->which != e1_sortResponse);


  if (ResponsePDU->u.sortResponse->sortStatus == 0) {
      
    if(ResponsePDU->u.sortResponse->otherInfo != NULL) {
      if (ResponsePDU->u.sortResponse->otherInfo->item.information.which 
	  == e24_characterInfo) {
	othermsg = GetInternationalString(ResponsePDU->u.sortResponse->otherInfo->item.information.u.characterInfo);
	if (othermsg != NULL) {
	  /* scan the message for a number */
	  num = strpbrk(othermsg,"0123456789");
	  if (num != NULL)
	    sscanf(num,"%d",&result_set_size);
	}
      }
    }
    else {
#ifdef Z3950_2001
      result_set_size = ResponsePDU->u.sortResponse->resultCount;
#endif
    }
  }

  /* handle successful sort results (and diagnostics)*/  

  switch (ResponsePDU->u.sortResponse->sortStatus) {
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

  switch (ResponsePDU->u.sortResponse->resultSetStatus) {
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
	  ResponsePDU->u.sortResponse->sortStatus,
	  sortstatusmsg,
	  ResponsePDU->u.sortResponse->resultSetStatus,
	  resultsetmsg,
	  out_results, result_set_size);
      
  Tcl_AppendElement(interp, temp);

  if (ResponsePDU->u.sortResponse->diagnostics != NULL) {
    /* add the diagnostics to the response */
    for (dl = ResponsePDU->u.sortResponse->diagnostics; dl; dl = dl->next) {
      if (dl->item->which == e13_defaultFormat) {
	if (dl->item->u.defaultFormat->addinfo.u.v2Addinfo &&
	    dl->item->u.defaultFormat->addinfo.u.v2Addinfo->data)
	  sprintf(temp, "%s", 
		  dl->item->u.defaultFormat->addinfo.u.v2Addinfo->data);
	else
	  sprintf(temp, "%s", "Malformed diagnostic record from Server.");

	Tcl_AppendElement(interp, temp);
      }
      else { /* external */
	sprintf(temp, "Unknown Target-defined error");
	Tcl_AppendElement(interp, temp);
      }
    }
  }
  /* Should be all set now...*/
  (void) FreePDU (ResponsePDU);
  
  return (TCL_OK);

}



int
TKZ_SetCmd(Session, interp, argc, argv)
     ClientData Session;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  
  if (argc < 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " parametername newvalue\"", (char *) NULL);
    return TCL_ERROR;
  }
  
 
  if (TKZ_User_Session == NULL) {
    ZSESSION *session;

    if ((session = CALLOC(ZSESSION,1)) == NULL) {
      Tcl_AppendResult(interp, argv[0], " is unable to allocate a ",
		       "session structure.",
		       (char *) NULL);
      return TCL_ERROR;

    }
    InitSession (session);
    TKZ_User_Session_TMP = TKZ_User_Session = session;
  }

  
  if (strcasecmp("session",argv[1]) == 0) {
    if (argc != 3) {
      Tcl_AppendResult(interp, "Error in Set Session should be \"", argv[0],
		       " session sessionIDnumber\"", (char *) NULL);
      return TCL_ERROR;
    }
    if (argc == 3) {
      TKZ_User_Session = LookupAssociation(&zAssociations, atoi(argv[2]));
    }
    if (TKZ_User_Session == NULL) {
      Tcl_AppendResult(interp, "Error in Set Session: Session number ", 
		       argv[2], " not found -- Did connection fail?", 
		       (char *) NULL);
      return TCL_ERROR;
    }
    return TCL_OK;
    
  }
  else if (strcasecmp("sResultSetName",argv[1]) == 0
      || strcasecmp("ResultSetName",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    strcpy(TKZ_User_Session->resultSetName, argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("Database",argv[1]) == 0
      || strcasecmp("Databasenames",argv[1]) == 0
      || strcasecmp("Databasename",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    strcpy(TKZ_User_Session->databaseName, argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("ElementSetNames",argv[1]) == 0
	   ||  strncasecmp("ELEMENTSETNAME", argv[1], strlen(argv[1])) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    strcpy(TKZ_User_Session->presentElementSet, argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("MediumElementSet",argv[1]) == 0
	   || strcasecmp("MedElementSet",argv[1]) == 0
	   || strcasecmp("MediumElementSetNames",argv[1]) == 0
	   || strcasecmp("MediumSetElementSetNames",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->s_mediumElementSetNames = strdup(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("SmallElementSet",argv[1]) == 0
	   || strcasecmp("SmallElementSetNames",argv[1]) == 0
	   || strcasecmp("SmallElementSetName",argv[1]) == 0
	   || strcasecmp("SmallSetElementSetNames",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->s_smallElementSetNames = strdup(argv[2]);
    return TCL_OK;
  }
  else if ((strcasecmp("PreferredRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("pPreferredRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("sPreferredRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecordSyntax",argv[1]) == 0)
	   || (strcasecmp("pRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecordFormat",argv[1]) == 0)
	   || (strcasecmp("RecFormat",argv[1]) == 0)
	   || (strcasecmp("sRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecSyntax",argv[1]) == 0)) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET RecSyntax should be ",
		       " MARC, SUTRS, OPAC, SGML, XML, HTML, EXPLAIN, SUMMARY",
		       " GRS1, GRS0, GENERIC, or ES",
		       (char *) NULL);
      return TCL_ERROR;
    }    
    if (strcasecmp("MARC",argv[2]) == 0 
        || strcasecmp(MARCRECSYNTAX,argv[2]) == 0
	|| strcasecmp("USMARC",argv[2]) == 0)
      TKZ_User_Session->preferredRecordSyntax = MARCRECSYNTAX;
    else if ((strcasecmp("SUTR",argv[2]) == 0)
	     || (strcasecmp("SUTRS",argv[2]) == 0)
	     || (strcasecmp(SUTRECSYNTAX,argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = SUTRECSYNTAX;
    else if ((strcasecmp(SGML_RECSYNTAX,argv[2]) == 0)
	     || (strcasecmp("SGML",argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = SGML_RECSYNTAX;
    else if ((strcasecmp(XML_RECSYNTAX,argv[2]) == 0)
	     || (strcasecmp("XML",argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = XML_RECSYNTAX;
    else if ((strcasecmp(HTML_RECSYNTAX,argv[2]) == 0)
	     || (strcasecmp("HTML",argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = HTML_RECSYNTAX;
    else if (strcasecmp("OPAC",argv[2]) == 0
	     || strcasecmp(OPACRECSYNTAX,argv[2]) == 0)
      TKZ_User_Session->preferredRecordSyntax = OPACRECSYNTAX;
    else if (strcasecmp("EXPLAIN",argv[2]) == 0
	     || strcasecmp(EXPLAINRECSYNTAX,argv[2]) == 0)
      TKZ_User_Session->preferredRecordSyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp("SUMMARY",argv[2]) == 0
	     || strcasecmp(SUMMARYRECSYNTAX,argv[2]) == 0)
      TKZ_User_Session->preferredRecordSyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp("GRS0",argv[2]) == 0        
	     || strcasecmp(GRS0RECSYNTAX,argv[2]) == 0)
      TKZ_User_Session->preferredRecordSyntax = GRS0RECSYNTAX;
    else if ((strcasecmp("GRS1",argv[2]) == 0)
	     || strcasecmp(GRS1RECSYNTAX,argv[2]) == 0
	     || strcasecmp("GRS-1",argv[2]) == 0
	     || (strcasecmp("GENERIC",argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = GRS1RECSYNTAX;
    else if ((strcasecmp("ES",argv[2]) == 0)
	     || strcasecmp(ESRECSYNTAX,argv[2]) == 0
	     || (strcasecmp("ESRECS",argv[2]) == 0))
      TKZ_User_Session->preferredRecordSyntax = ESRECSYNTAX;
    else {
      Tcl_AppendResult(interp, "ZSET RecSyntax should be ",
		       " MARC, SUTRS, OPAC, SGML, XML, HTML, EXPLAIN, SUMMARY",
		       " GRS1, GRS0, GENERIC, or ES",
		       (char *) NULL);
      return TCL_ERROR;
    }
    
    return TCL_OK;
  }
  else if (strcasecmp("AttributeSet",argv[1]) == 0
	   || strcasecmp("Attributes",argv[1]) == 0) {
    
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    if (strcasecmp("BIB1",argv[2]) == 0 
	|| strcasecmp(OID_BIB1,argv[2]) == 0
	|| strcasecmp("BIB-1",argv[2]) == 0)
      TKZ_User_Session->c_attributeSetId = OID_BIB1;
    else if ((strcasecmp("EXPLAIN1",argv[2]) == 0)
	     || (strcasecmp("EXPLAIN",argv[2]) == 0)
	     || (strcasecmp(OID_EXP1,argv[2]) == 0)
	     || (strcasecmp("EXP1",argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_EXP1;
    else if ((strcasecmp("EXTENDED",argv[2]) == 0)
	     || (strcasecmp("EXT",argv[2]) == 0)
	     || (strcasecmp(OID_EXT1,argv[2]) == 0)
	     || (strcasecmp("EXT1",argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_EXT1;
    else if ((strcasecmp("COMMAND",argv[2]) == 0)
	     || (strcasecmp("COMMON",argv[2]) == 0)
	     || (strcasecmp("CCL1",argv[2]) == 0)
	     || (strcasecmp(OID_CCL1,argv[2]) == 0)
	     || (strcasecmp("CCL",argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_CCL1;
    else if ((strcasecmp("GILS",argv[2]) == 0)
	     || (strcasecmp("GOVERNMENT",argv[2]) == 0)
	     || (strcasecmp(OID_GILS,argv[2]) == 0)
	     || (strcasecmp("GILS1",argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_GILS;
    else if ((strcasecmp("GEO",argv[2]) == 0)
	     || (strcasecmp("GEO_PROFILE_ATTR",argv[2]) == 0)
	     || (strcasecmp(OID_GILS,argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_GEO;
    else if ((strcasecmp("STAS",argv[2]) == 0)
	     || (strcasecmp("SCIENTIFIC",argv[2]) == 0)
	     || (strcasecmp(OID_STAS,argv[2]) == 0)
	     || (strcasecmp("STAS1",argv[2]) == 0))
      TKZ_User_Session->c_attributeSetId = OID_STAS;
    else {
      Tcl_AppendResult(interp, "ZSET Attributes should be ", argv[0],
		       "BIB1, EXPLAIN, EXT, CCL, GILS, GEO or STAS",
		       (char *) NULL);
      return TCL_ERROR;
    }
    
    return TCL_OK;
  }
  else if (strcasecmp("AdditionalSearchInfo",argv[1]) == 0
	   || strcasecmp("AddedSearchInfo",argv[1]) == 0
	   || strcasecmp("SearchInfo",argv[1]) == 0) {
    
    if (argc <= 3) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a TYPE ",
		       "(CHAR or OID) and a value. E.g.: ",
		       "zset additionalsearchinfo oid ", OID_GILS,
		       (char *) NULL);
      return TCL_ERROR;
    }
    /* Additional information type (OID or CHAR).*/
    if (strcasecmp("CHAR",argv[2]) == 0) {
      TKZ_User_Session->SearchAddInfoType = 0;	
    }
    else if (strcasecmp("OID",argv[2]) == 0) {
      TKZ_User_Session->SearchAddInfoType = 1;
    }
    /* Additional information.*/
    strcpy(TKZ_User_Session->SearchAddInfo, argv[3]);	
    return TCL_OK;
  }
  else if (strcasecmp("Host",argv[1]) == 0) {
    /* We don't want to fiddle directly with some of these -- they */
    /* just pass along arguments to other functions.               */
    return(TKZ_SelectCmd(Session,interp,argc-1,argv+1));
  }
  else if (strcasecmp("QueryFormat", argv[1]) == 0) {
    
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a format name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    if (strcasecmp("rpn", argv[2]) == 0
	|| strcasecmp("1", argv[2]) == 0) {
      TKZ_User_Session->queryType = 1;
    }
    else if (strcasecmp("z39.58", argv[2]) == 0
	     || strcasecmp("ccl", argv[2]) == 0
	     || strcasecmp("100", argv[2]) == 0) {
      TKZ_User_Session->queryType = 100;
    }
    else if (strcasecmp("ISO8777", argv[2]) == 0 
	     || strcasecmp("2", argv[2]) == 0) {
      TKZ_User_Session->queryType = 2;
    }
    else if (strcasecmp("ERPN", argv[2]) == 0 
	     || strcasecmp("101", argv[2]) == 0) {
      TKZ_User_Session->queryType = 101;
    }
    else if (strcasecmp("RANKED", argv[2]) == 0 
	     || strcasecmp("102", argv[2]) == 0) {
      TKZ_User_Session->queryType = 102;
    }
    else if (strcasecmp("SQL",argv[2]) == 0 
	     || strcasecmp("0",argv[2]) == 0) {
      TKZ_User_Session->queryType = 0;
    }
    else 
      TKZ_User_Session->queryType = 1;
    
    return TCL_OK;
  }
  else if (strcasecmp("PreferredMessageSize",argv[1]) == 0
      || strcasecmp("ipreferredMsgSize",argv[1]) == 0 
      || strcasecmp("ipreferredMessageSize",argv[1]) == 0 
      || strcasecmp("preferredMsgSize",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a size",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->c_preferredMessageSize =
      TKZ_User_Session->preferredMessageSize = atoi(argv[2]);
   if (TKZ_User_Session->c_exceptionalRecordSize 
       < TKZ_User_Session->c_preferredMessageSize) {
     Tcl_AppendResult(interp, "ZSET ", argv[1], " must be less than ",
		      "exceptionalRecordSize", (char *) NULL);
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  else if (strcasecmp("exceptionalRecordSize",argv[1]) == 0
      || strcasecmp("imaxRecSize",argv[1]) == 0 
      || strcasecmp("maxRecSize",argv[1]) == 0 
      || strcasecmp("maxRecordSize",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a size",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->exceptionalRecordSize =
      TKZ_User_Session->c_exceptionalRecordSize = atoi(argv[2]);
    if (TKZ_User_Session->c_exceptionalRecordSize 
	< TKZ_User_Session->c_preferredMessageSize) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must be greater than",
		       " or equal to PreferredMessageSize", (char *) NULL);
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  else if (strcasecmp("sSmallSetUpperBound",argv[1]) == 0
      || strcasecmp("SmallSetUpperBound",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->c_smallSetUpperBound = atoi(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("sLargeSetLowerBound",argv[1]) == 0
      || strcasecmp("LargeSetLowerBound",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->c_largeSetLowerBound = atoi(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("sMediumSetPresentNum",argv[1]) == 0
      || strcasecmp("MediumSetPresentNumber",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->c_mediumSetPresentNumber = atoi(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("timeout",argv[1]) == 0
      || strcasecmp("clienttimeout",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number of seconds",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->c_timeOut = atoi(argv[2]);
    return TCL_OK;
  }
  else  if (strcasecmp("ReplaceIndicator",argv[1]) == 0
      || strcasecmp("sReplaceIndicator",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number (0 or 1)",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->replaceIndicator = argv[2][0] ? 1:0 ;
    return TCL_OK;
  }
  else if (strcasecmp("ResultSetStartPoint",argv[1]) == 0
      || strcasecmp("StartPosition",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->nextResultSetPosition =
      TKZ_User_Session->startPosition = atoi(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("NumrecsRequested",argv[1]) == 0
	   || strcasecmp("NumRequested",argv[1]) == 0
	   || strcasecmp("NumRecs",argv[1]) == 0
	   || strcasecmp("NumberOfRecordsRequested",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a number",
		       (char *) NULL);
      return TCL_ERROR;
    }
    TKZ_User_Session->numberOfRecordsRequested = atoi(argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("pElementSetNames",argv[1]) == 0
      || strcasecmp("ElementSetNames",argv[1]) == 0) {
    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    strcpy(TKZ_User_Session->presentElementSet, argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("ResultSetName",argv[1]) == 0
	   || strcasecmp("pResultSetid",argv[1]) == 0
	   || strcasecmp("sResultSetName",argv[1]) == 0) {

    if (argc == 2) {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " must include a name",
		       (char *) NULL);
      return TCL_ERROR;
    }
    strcpy(TKZ_User_Session->resultSetName, argv[2]);
    return TCL_OK;
  }
  else if (strcasecmp("logfile",argv[1]) == 0
	   || strcasecmp("logfilename",argv[1]) == 0) {

    if (strlen(argv[2]) > 200) {
      sprintf(LOGFILENAME, "%s", argv[2]);
      TKZ_User_Session->s_logFileName = LOGFILENAME;
      return TCL_OK;
    }
    else {
      Tcl_AppendResult(interp, "ZSET ", argv[1], " : Logfile name too long (more than 200 characters) ",
		       (char *) NULL);
      return TCL_ERROR;
    }
  }
  else if (strcasecmp("logging",argv[1]) == 0
	   || strcasecmp("log",argv[1]) == 0
	   || strcasecmp("logs",argv[1]) == 0) {

    if (argc == 2 || strcasecmp("on", argv[2]) == 0 
	|| strcasecmp("1", argv[2]) == 0) {

      if (LOGFILE == NULL) { /* open the log file */
	TKZ_User_Session->s_logFileName = LOGFILENAME;
	TKZ_User_Session->s_lfptr = fopen(TKZ_User_Session->s_logFileName,"w");
	LOGFILE = TKZ_User_Session->s_lfptr;
      }
      else {
	TKZ_User_Session->s_lfptr = LOGFILE;
      }
      LOGGING_ON = 1; /* this controls whether or not logging takes place */
    }
    else if (strcasecmp("off", argv[2]) == 0 
	|| strcasecmp("0", argv[2]) == 0) {
      LOGGING_ON = 0; /* stop logging, but leave the file open */
    }
    return TCL_OK;
  }
  else
    Tcl_AppendResult(interp, "ZSET ", argv[1],
		     " is not a valid option. Type ZSHOW Setable.",
		     (char *) NULL);
  return TCL_ERROR;
}
  


int
TKZ_ShowCmd(session, interp, argc, argv)
     ClientData session;	      /* Current Session */
     Tcl_Interp *interp;	      /* Current interpreter. */
     int argc;			      /* Number of arguments. */
     char **argv;		      /* Argument strings. */
{
  char allbuffer[3000];
  int printall = 0;
  
 
  if (TKZ_User_Session == NULL) {
    Tcl_AppendResult(interp, argv[0], " is not usable before a connection ",
		     "is made.", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (argc == 1 || (argc == 2  && 
		    (strcasecmp("ALL",argv[1]) == 0) 
		    || strcasecmp("SETABLE",argv[1]) == 0))
    printall = 1;

  if (printall || strcasecmp("SERVER",argv[1]) == 0
      || strcasecmp("ServerInfo",argv[1]) == 0) {
    sprintf(allbuffer,
	    "SERVER Connection Variables:\n\
Host Name %s\n\
Host IP Address %s\n\
Port %d\n\
Socket %d\n\
Server ImplementationId %s\n\
Server ImplementationName %s\n\
Server ImplementationVersion %s\n\
Server ProtocolVersion %s\n\
Server SupportVersion1 %d\n\
Server SupportVersion2 %d\n\
Server SupportVersion3 %d\n\
Server Options %s\n\
Support Search %d\n\
Support Present %d\n\
Support DeleteResultSet %d\n\
Support ResourceReport %d\n\
Support TriggerResourceControl %d\n\
Support ResourceControl %d\n\
Support AccessControl %d\n\
Support Scan %d\n\
Support Sort %d\n\
Support Extended Services %d\n\
Support Level-1 Segmentation %d\n\
Support Level-2 Segmentation %d\n\
Support Concurrent Operations %d\n\
Support Named Result Sets %d\n\
Support Type0Query %d\n\
Support Type1Query %d\n\
Support Type2Query %d\n\
Support Type100Query %d\n\
Support Type101Query %d\n\
Support Type102Query %d\n\
Support ElementSetNames %d\n\
Support SingleElementSetName %d\n\
PreferredMessageSize %d\n\
ExceptionalRecordSize %d\n\n", 
	    (TKZ_User_Session->s_serverMachineName != NULL ?
	    TKZ_User_Session->s_serverMachineName : "None Set") , 
 	    (TKZ_User_Session->s_serverIP != NULL ? 
	    TKZ_User_Session->s_serverIP : "None Set"),
 	    TKZ_User_Session->s_portNumber,
	    TKZ_User_Session->fd, 
 	    (TKZ_User_Session->s_implementationId != NULL ? 
	    TKZ_User_Session->s_implementationId : "None Set"),
 	    (TKZ_User_Session->s_implementationName != NULL ? 
	    TKZ_User_Session->s_implementationName : "None Set"),
 	    (TKZ_User_Session->s_implementationVersion != NULL ? 
	    TKZ_User_Session->s_implementationVersion : "None Set"),
 	    (TKZ_User_Session->s_protocolVersion != NULL ? 
	    TKZ_User_Session->s_protocolVersion : "None Set"),
 	    TKZ_User_Session->s_supportVersion1,
 	    TKZ_User_Session->s_supportVersion2,
 	    TKZ_User_Session->s_supportVersion3,
 	    (TKZ_User_Session->s_options != NULL ? 
	    TKZ_User_Session->s_options : "None Set"),
	    TKZ_User_Session->s_supportSearch,
	    TKZ_User_Session->s_supportPresent,
	    TKZ_User_Session->s_supportDeleteResultSet,
	    TKZ_User_Session->s_supportResourceReport,
	    TKZ_User_Session->s_supportTriggerResourceControl,
	    TKZ_User_Session->s_supportResourceControl,
	    TKZ_User_Session->s_supportAccessControl,
	    TKZ_User_Session->s_supportScan,
	    TKZ_User_Session->s_supportSort,
	    TKZ_User_Session->s_supportExtendedServices,
	    TKZ_User_Session->s_supportLevel_1_Segmentation,
	    TKZ_User_Session->s_supportLevel_2_Segmentation,
	    TKZ_User_Session->s_supportConcurrentOperations,
	    TKZ_User_Session->s_supportNamedResultSets,
	    TKZ_User_Session->s_supportType0Query,
	    TKZ_User_Session->s_supportType1Query,
	    TKZ_User_Session->s_supportType2Query,
	    TKZ_User_Session->s_supportType100Query,
	    TKZ_User_Session->s_supportType101Query,
	    TKZ_User_Session->s_supportType102Query,
	    TKZ_User_Session->s_supportElementSetNames,
	    TKZ_User_Session->s_supportSingleElementSetName,
	    TKZ_User_Session->s_preferredMessageSize,
	    TKZ_User_Session->s_exceptionalRecordSize); 

    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    if (!printall) return TCL_OK;
  }


  if (printall || strcasecmp("CLIENT",argv[1]) == 0
      || strcasecmp("ClientInfo",argv[1]) == 0) {
    sprintf(allbuffer,
	    "CLIENT Connection Variables:\n\
Host %s\n\
Client IP %s\n\
Client Socket %d\n\
Client ImplementationId %s\n\
Client ImplementationName %s\n\
Client ImplementationVersion %s\n\
Client ProtocolVersion %s\n\
Client SupportVersion1 %d\n\
Client SupportVersion2 %d\n\
Client SupportVersion3 %d\n\
Client Options %s\n\
Support Search %d\n\
Support Present %d\n\
Support DeleteResultSet %d\n\
Support ResourceReport %d\n\
Support TriggerResourceControl %d\n\
Support ResourceControl %d\n\
Support AccessControl %d\n\
Timeout %d\n\
PreferredMessageSize %d\n\
ExceptionalRecordSize %d\n\n",
 	    (TKZ_User_Session->c_clientMachineName != NULL ?
	     TKZ_User_Session->c_clientMachineName : "None Set") , 
            (TKZ_User_Session->c_clientIP != NULL ? 
	     TKZ_User_Session->c_clientIP : "None Set"),
            TKZ_User_Session->c_fd, 
            (TKZ_User_Session->c_implementationId != NULL ? 
	     TKZ_User_Session->c_implementationId : "None Set"),
            (TKZ_User_Session->c_implementationName != NULL ? 
	     TKZ_User_Session->c_implementationName : "None Set"),
            (TKZ_User_Session->c_implementationVersion != NULL ? 
	     TKZ_User_Session->c_implementationVersion : "None Set"),
            (TKZ_User_Session->c_protocolVersion != NULL ?
	     TKZ_User_Session->c_protocolVersion : "None Set"),
            TKZ_User_Session->c_supportVersion1,
            TKZ_User_Session->c_supportVersion2,
            TKZ_User_Session->c_supportVersion3,
            (TKZ_User_Session->c_options != NULL ? 
	     TKZ_User_Session->c_options : "None Set"),
	    TKZ_User_Session->c_requestSearch,
	    TKZ_User_Session->c_requestPresent,
	    TKZ_User_Session->c_requestDeleteResultSet,
	    TKZ_User_Session->c_requestResourceReport,
	    TKZ_User_Session->c_requestTriggerResourceControl,
	    TKZ_User_Session->c_requestResourceControl,
	    TKZ_User_Session->c_requestAccessControl,
            TKZ_User_Session->c_timeOut,
            TKZ_User_Session->c_preferredMessageSize,
            TKZ_User_Session->c_exceptionalRecordSize); 

    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    if (!printall) return TCL_OK;
  }

  if (printall || strcasecmp("Search",argv[1]) == 0
      || strcasecmp("SearchInfo",argv[1]) == 0) {
    sprintf(allbuffer,"SEARCH Variables: \n\
SmallSetUpperBound %d\n\
LargeSetLowerBound %d\n\
MediumSetPresentNumber %d\n\
ReplaceIndicator %d\n\
ResultSetName %s\n\
Database %s\n\
SmallSetElementSetNames %s\n\
MediumSetElementSetNames %s\n\
PreferredRecordSyntax %s\n\
Last Query %s\n\
Last AttributeSet %s\n\
Last Search Status %d (0=failure 1=success)\n\
Last Search Result Set Status %d (0=Not appl., 1=sub, 2=interim 3=none) \n\
Last Search Present Status %d \n\
Last Search ResultCount %d\n\
Last Search RecordsReturned %d\n\
Last Search Next ResultSet Position %d\n\
Additional Search Info %s\n\n",

	    TKZ_User_Session->c_smallSetUpperBound,
	    TKZ_User_Session->c_largeSetLowerBound,
	    TKZ_User_Session->c_mediumSetPresentNumber,
	    TKZ_User_Session->replaceIndicator, 
            TKZ_User_Session->resultSetName,
            TKZ_User_Session->databaseName,
            (TKZ_User_Session->s_smallElementSetNames ? 
	    TKZ_User_Session->s_smallElementSetNames : "None Set"),
            (TKZ_User_Session->s_mediumElementSetNames ? 
	    TKZ_User_Session->s_mediumElementSetNames : "None Set"),
            TKZ_User_Session->preferredRecordSyntax ? 
            TKZ_User_Session->preferredRecordSyntax : "None Set",
	    TKZ_User_Session->queryString ?
	    TKZ_User_Session->queryString : "No Query",
	    TKZ_User_Session->c_attributeSetId,
	    TKZ_User_Session->searchStatus,
	    TKZ_User_Session->resultSetStatus,
	    TKZ_User_Session->presentStatus,
	    TKZ_User_Session->resultCount,
	    TKZ_User_Session->noRecordsReturned,
	    TKZ_User_Session->nextResultSetPosition,
	    TKZ_User_Session->SearchAddInfo);

    Tcl_AppendResult(interp, allbuffer, (char *) NULL);

    if (!printall) return TCL_OK;
    
  }


  if (printall || strcasecmp("Present",argv[1]) == 0
      || strcasecmp("PresentInfo",argv[1]) == 0) {
    sprintf(allbuffer,"PRESENT Variables:\n\
ResultSet Name %s\n\
ResultSetStartPoint %d\n\
NumberOfRecordsRequested %d\n\
ElementSetNames %s\n\
preferredRecordSyntax(recsyntax) %s\n\
NumberOfRecordsReturned (records) %d\n\
NextResultSetPosition (nextrec) %d\n\
Total Records Returned (totrecords) %d\n\n",
	    TKZ_User_Session->resultSetName, 
	    TKZ_User_Session->nextResultSetPosition,
	    TKZ_User_Session->numberOfRecordsRequested, 
	    TKZ_User_Session->presentElementSet,
            TKZ_User_Session->preferredRecordSyntax ? 
            TKZ_User_Session->preferredRecordSyntax : "None Set",
	    TKZ_User_Session->noRecordsReturned, 
	    TKZ_User_Session->nextResultSetPosition,
	    TKZ_User_Session->totalNumberRecordsReturned);

    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    
    if (!printall) return TCL_OK;
  }


  if (printall || strcasecmp("ResultSets",argv[1]) == 0
      || strcasecmp("Results",argv[1]) == 0) {
    char *resultSetId;
    
    if (printall) Tcl_AppendResult(interp, "ResultSets Stored on Server:",
				   (char *)NULL);
    if (TKZ_User_Session->c_resultSetNames == NULL) 
      Tcl_AppendElement(interp, "NONE");
    else {
      /* print the client-side list of result set names */
      for (resultSetId = ac_list_first(TKZ_User_Session->c_resultSetNames); 
	   resultSetId != NULL;
	   resultSetId = ac_list_next(TKZ_User_Session->c_resultSetNames)) {
	
	Tcl_AppendElement(interp, resultSetId);
      }
    }
    return TCL_OK;
  }

  if ((strcasecmp("preferredRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("pRecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecordSyntax",argv[1]) == 0)
	   || (strcasecmp("RecSyntax",argv[1]) == 0)) {
    
    Tcl_AppendResult(interp, argv[1], " ",
		     TKZ_User_Session->preferredRecordSyntax, (char *) NULL);
    
    if (TKZ_User_Session->preferredRecordSyntax) {
      char *s = TKZ_User_Session->preferredRecordSyntax;
      if (strcasecmp(s,MARCRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " MARC", (char *) NULL);
      else if (strcasecmp(s,SUTRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " SUTRS", (char *) NULL);
      else if (strcasecmp(s,SGML_RECSYNTAX) == 0)
	Tcl_AppendResult(interp, " SGML", (char *) NULL);
      else if (strcasecmp(s,XML_RECSYNTAX) == 0)
	Tcl_AppendResult(interp, " XML", (char *) NULL);
      else if (strcasecmp(s,HTML_RECSYNTAX) == 0)
	Tcl_AppendResult(interp, " HTML", (char *) NULL);
      else if (strcasecmp(s,EXPLAINRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " EXPLAIN", (char *) NULL);
      else if (strcasecmp(s,OPACRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " OPAC", (char *) NULL);
      else if (strcasecmp(s,SUMMARYRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " SUMMARY", (char *) NULL);
      else if (strcasecmp(s,GRS0RECSYNTAX) == 0)
	Tcl_AppendResult(interp, " GRS0", (char *) NULL);
      else if (strcasecmp(s,GRS1RECSYNTAX) == 0)
	Tcl_AppendResult(interp, " GRS1 (generic)", (char *) NULL);
      else if (strcasecmp(s,ESRECSYNTAX) == 0)
	Tcl_AppendResult(interp, " ES (Extended Services)", (char *) NULL);
      else
	Tcl_AppendResult(interp, argv[1], " UNKNOWN", (char *) NULL);
      
    }
    else
      Tcl_AppendResult(interp, argv[1], "NOT SET", (char *) NULL);
    
    return TCL_OK;
  }
  else if (strcasecmp("sDBNames",argv[1]) == 0
	   || strcasecmp("database",argv[1]) == 0) {
    Tcl_AppendResult(interp, argv[1], " ",
		     TKZ_User_Session->databaseName, (char *) NULL);
    return TCL_OK;
  }  
  else if (strcasecmp("hits",argv[1]) == 0 
      || strcasecmp("resultcount",argv[1]) == 0 
      || strcasecmp("numhits",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->resultCount);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("records",argv[1]) == 0 
      || strcasecmp("numrecords",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->noRecordsReturned);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("nextrec",argv[1]) == 0 
      || strcasecmp("nextResultSetPosition",argv[1]) == 0 
      || strcasecmp("nextrecordpos",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->nextResultSetPosition);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("totalrecs",argv[1]) == 0 
      || strcasecmp("totalrecords",argv[1]) == 0 
      || strcasecmp("totalNumberRecordsReturned",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->totalNumberRecordsReturned);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("Hosts",argv[1]) == 0
      || strcasecmp("Servers",argv[1]) == 0) {
    ZSESSION *sess;
    /* this shows ALL connected servers */
    for (sess = zAssociations.head; sess != NULL; sess = sess->next) {
      
      Tcl_AppendResult(interp, argv[1], " ",
		       sess->s_serverMachineName, " ",
		       (sess->connectionStatus == Z_OPEN ?"OPEN\n":"CLOSED\n"),
		       (char *) NULL);
    }
    return TCL_OK;
  }
  else if (strcasecmp("Host",argv[1]) == 0
      || strcasecmp("Servername",argv[1]) == 0) {
    Tcl_AppendResult(interp, argv[1], " ",
		     TKZ_User_Session->s_serverMachineName, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("Port",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->s_portNumber);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("StatsFile",argv[1]) == 0
      || strcasecmp("LogFile",argv[1]) == 0)  {
    if (TKZ_User_Session->s_logFileName)
      Tcl_AppendResult(interp, argv[1], " ",
		       TKZ_User_Session->s_logFileName, (char *) NULL);
    else
      Tcl_AppendResult(interp, argv[1], "NOT SET", (char *) NULL);
    return TCL_OK;
  }
  else  if (strcasecmp("QueryFormat",argv[1]) == 0
      || strcasecmp("QueryType",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->queryType);
    Tcl_AppendResult(interp, argv[1], allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("AdditionalSearchInfo",argv[1]) == 0
	   || strcasecmp("AddedSearchInfo",argv[1]) == 0
	   || strcasecmp("SearchInfo",argv[1]) == 0) {
    
    if (TKZ_User_Session->SearchAddInfoType == 1) {
      Tcl_AppendResult(interp, argv[1], " OID", (char *) NULL);
    } else {
      Tcl_AppendResult(interp, argv[1], " CHAR", (char *) NULL);
    }

    Tcl_AppendResult(interp, " ",
		     TKZ_User_Session->SearchAddInfo, (char *) NULL);
    return TCL_OK;
  }
  else  if (strcasecmp("PreferredMessageSize",argv[1]) == 0
	    || strcasecmp("ipreferredMsgSize",argv[1]) == 0 
	    || strcasecmp("ipreferredMessageSize",argv[1]) == 0 
	    || strcasecmp("preferredMsgSize",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->preferredMessageSize);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("iMaxRecSize",argv[1]) == 0
	   || strcasecmp("imaxRecSize",argv[1]) == 0 
	   || strcasecmp("maxRecSize",argv[1]) == 0 
	   || strcasecmp("maxRecordSize",argv[1]) == 0
	   || strcasecmp("exceptionalRecordSize",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->exceptionalRecordSize);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("sSmallSetUpperBound",argv[1]) == 0
      || strcasecmp("SmallSetUpperBound",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->c_smallSetUpperBound);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("sLargeSetLowerBound",argv[1]) == 0
      || strcasecmp("LargeSetLowerBound",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->c_largeSetLowerBound);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("sMediumSetPresentNum",argv[1]) == 0
      || strcasecmp("MediumSetPresentNumber",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->c_mediumSetPresentNumber);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("ReplaceIndicator",argv[1]) == 0) {
    sprintf(allbuffer,"%s '%c'", argv[1],
	    TKZ_User_Session->replaceIndicator);
    return TCL_OK;
  }
  else if (strcasecmp("ResultSetName",argv[1]) == 0
      || strcasecmp("pResultSetid",argv[1]) == 0
      || strcasecmp("sResultSetName",argv[1]) == 0) {
    Tcl_AppendResult(interp, argv[1], " ",
		     TKZ_User_Session->resultSetName, (char *) NULL);
    return TCL_OK;
  }
  if (strcasecmp("SmallSetElementSetNames",argv[1]) == 0
      || strcasecmp("SmallElementSetNames",argv[1]) == 0
      || strcasecmp("SmallElementSet",argv[1]) == 0)  {

    Tcl_AppendResult(interp, argv[1], " ",
		     (TKZ_User_Session->s_smallElementSetNames ? 
		      TKZ_User_Session->s_smallElementSetNames : "None Set"), 
		     (char *) NULL);
    return TCL_OK; 
  }
  if (strcasecmp("sMediumSetElementSetNames",argv[1]) == 0
      || strcasecmp("SmallElementSetNames",argv[1]) == 0
      || strcasecmp("SmallElementSet",argv[1]) == 0)  {
    Tcl_AppendResult(interp, argv[1], " ", 
		     (TKZ_User_Session->s_mediumElementSetNames ? 
		      TKZ_User_Session->s_mediumElementSetNames : "None Set"), 
		     (char *) NULL); 
    return TCL_OK; 
  } 
  
  else if (strcasecmp("PreferredRecordSyntax",argv[1]) == 0
      || strcasecmp("PreferredRecordSyntax",argv[1]) == 0
      || strcasecmp("PreferredRecordSyntax",argv[1]) == 0) {
    return TCL_OK;
  }

  else if (strcasecmp("Query",argv[1]) == 0) {
    if (TKZ_User_Session->queryString)
      Tcl_AppendResult(interp, argv[1], " ",
		       TKZ_User_Session->queryString, (char *) NULL);
    else
      Tcl_AppendResult(interp, argv[1], " NOT SET", (char *) NULL);
    return TCL_OK;
  }
  
  else if (strcasecmp("AttributeSet",argv[1]) == 0) {
    if (TKZ_User_Session->c_attributeSetId)
      Tcl_AppendResult(interp, argv[1], " ",
		       TKZ_User_Session->c_attributeSetId, (char *) NULL);
    else
      Tcl_AppendResult(interp, argv[1], " NOT SET", (char *) NULL);
    return TCL_OK;
  }
  
  else if (strcasecmp("pResultSetStartPoint",argv[1]) == 0
	   || strcasecmp("nextResultSetPosition",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->nextResultSetPosition);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("pNumRecsReq",argv[1]) == 0
	   || strcasecmp("numberOfRecordsRequested",argv[1]) == 0) {
    sprintf(allbuffer,"%s %d", argv[1],
	    TKZ_User_Session->numberOfRecordsRequested);
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("logging",argv[1]) == 0) {
    if (LOGGING_ON) 
      sprintf(allbuffer,"logging on");
    else
      sprintf(allbuffer,"logging off");
    Tcl_AppendResult(interp, allbuffer, (char *) NULL);
    return TCL_OK;
  }
  else if (strcasecmp("pElementSetNames",argv[1]) == 0
      || strcasecmp("ElementSetNames",argv[1]) == 0) {
    if (TKZ_User_Session->presentElementSet)
      Tcl_AppendResult(interp, argv[1], " ",
		       TKZ_User_Session->presentElementSet, (char *) NULL);
    else
      Tcl_AppendResult(interp, argv[1], " NOT SET", (char *) NULL);

    return TCL_OK;
  }

  /* the ZSHOW command wasn't matched, so... */
  Tcl_AppendResult(interp, "Show option \"", argv[1],
		   "\" not recognized (use ALL to display all variables)", 
		   (char *) NULL);
  return TCL_ERROR;

}


/* TKZ_DeleteCmd --
 *
 *  Delete one or all result sets stored on server
 *
 * Results:
 *	A standard Tcl result.
 *
 *----------------------------------------------------------------------
 */

int
TKZ_DeleteCmd(clientData, interp, argc, argv)
     ClientData clientData;              /* Not used. */
     Tcl_Interp *interp;                 /* Current interpreter. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings. */
{
  int sessionID;
  PDU *out_pdu, *in_pdu;
  ZSESSION *session;
  ZASSOCIATION *associations;
  char *resultSet;
  int numberResultSets;
  int i = 0, j=0;
  char temp[60];
  int set_arg;
  DeleteResultSetResponse	*pr;
  int match=0;
  ListStatuses *ls;

  if ((associations = (ZASSOCIATION *)clientData) == NULL)
    return TCL_ERROR;
  
  if (argc < 2) {
    Tcl_AppendResult(interp, "Syntax: ", argv[0],
		     " <sessionID> [all | resultSet1 <resultSet2> ...]", 
		     (char *) NULL);
    return TCL_ERROR;
  }
  
  set_arg = 2;
  
  if (argc >= 3) {
      /* argv[1] MAY be a session id number */
    if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK) {
      /* not a number, clear the error message in the result */
      Tcl_ResetResult(interp);
      if (TKZ_User_Session == NULL) {
	Tcl_AppendResult(interp, "No current Session, must first connect ",
			 "to a server",  (char *) NULL);
	return TCL_ERROR; 
      }
      session = TKZ_User_Session;
      set_arg = 1;
      sessionID = TKZ_User_Session->fd;
    }
    else { /* had a number as first arg -- so lookup the session */
      if ((session = LookupAssociation (&zAssociations, sessionID)) == NULL) {
	sprintf(temp, "invalid session id: %s", argv[1]);
	Tcl_AppendResult(interp, temp,  (char *) NULL);
	return TCL_ERROR; 
      }
      set_arg = 2;
      sessionID = session->fd;
    }
  } 
  else { /* Only two args -- so it is just a single resultset name */
    session = TKZ_User_Session;
    set_arg = 1;
    sessionID = TKZ_User_Session->fd;
  }
  
  if (session->s_supportDeleteResultSet != Z_TRUE) {
    sprintf(temp, "Server %s doesn't support result set deletion.", 
	    session->s_serverMachineName);
	Tcl_AppendResult(interp, temp,  (char *) NULL);
        return TCL_ERROR;
  }
  resultSet = argv[set_arg];
  
  if ((session->numberResultSets != 0) && 
      (session->resultSetList != (char **)NULL)) {
    for (i=0; i<session->numberResultSets; i++)
      if (session->resultSetList[i])
	(void)free(session->resultSetList[i]);
    (void) free ((char *)session->resultSetList);
    session->resultSetList = (char **)NULL;
    session->numberResultSets = 0;
  }
  if (strncasecmp(resultSet, "all", 3) == 0)
    session->deleteAllResultSets = Z_TRUE;
  else {
    /* there are one or more resultset names in the command line... */
    session->deleteAllResultSets = Z_FALSE;
    numberResultSets = argc - set_arg;
    session->resultSetList = (char **)calloc(numberResultSets+1,sizeof(char *));
    if (session->resultSetList == (char **)NULL) {
      printf("out of memory!\n");
      return TCL_ERROR;
    }
    session->numberResultSets = numberResultSets;
    
    for (i=set_arg, j=0; i<argc; i++, j++)
      session->resultSetList[j] = (char *)strdup(argv[i]);
    /* set the end of the list to null */
    session->resultSetList[j] = (char *)NULL;
  }
  
  if (session->deleteAllResultSets == Z_TRUE)
    out_pdu = MakeDeleteResultSetRequest("DELETE_REFID", 1, (char **)NULL, 0);
  else
    out_pdu = MakeDeleteResultSetRequest("DELETE_REFID", 0, 
					 session->resultSetList, 
					 session->numberResultSets);
  if (out_pdu == NULL) {
    Tcl_AppendResult(interp, "Error -- couldn't make delete request",  
		     (char *) NULL);
    return TCL_ERROR; 
  }
  
  if (PutPDU (session->fd, out_pdu) != Z_OK) {
    (void) FreePDU (out_pdu);
    Tcl_AppendResult(interp, "Error -- couldn't send delete request",  
		     (char *) NULL);
    return TCL_ERROR; 
  }
  if ((in_pdu = ClientGetPDU(session->fd,session->c_timeOut)) == NULL) {
    Tcl_AppendResult(interp, "Error -- didn't get delete response",  
		     (char *) NULL);
    return TCL_ERROR; 
  }
  (void) FreePDU (out_pdu);
  
  pr = in_pdu->u.deleteResultSetResponse;

  if ((pr->deleteOperationStatus == 3) ||
      (pr->deleteOperationStatus == 4) ||
      (pr->deleteOperationStatus == 5) ||
      (pr->deleteOperationStatus == 6)) {
    Tcl_AppendResult(interp, "Result sets requested are not deleted.",
		     (char *) NULL);
    return TCL_ERROR; 
  }
  
  if (session->deleteAllResultSets) {		/* bulk delete	*/
    if (pr->deleteOperationStatus == 0) {	/* sucesses	*/

      Tcl_AppendResult(interp, 
		       "All result sets have been deleted successfully.",
		       (char *) NULL);
      /* remove all resultset names from list */
      ac_list_free(session->c_resultSetNames);
      session->c_resultSetNames = NULL;

    } else if (pr->deleteOperationStatus == 7) {	
      Tcl_AppendResult(interp, 
		       "Sorry -- the server doesn't support bulk deletion.",
		       (char *) NULL);
    } else if (pr->deleteOperationStatus == 8) {
      if (pr->numberNotDeleted) {
	sprintf(temp, "%d result sets not deleted.", 
		pr->numberNotDeleted);
	Tcl_AppendElement(interp, temp);
	return TCL_ERROR; 
      }
      /* walk through the buldStatuses list to see which result set
       * has been removed, which hasn't been deleted.
       */
      for (ls = pr->bulkStatuses; ls; ls = ls->next) {
	if (ls->item.status == 0) {
	  sprintf(temp, "%s successfully_deleted", 
		  GetOctetString(ls->item.id), ls->item.status);
	  Tcl_AppendElement(interp, temp);
	}
	else {
	  sprintf(temp, "%s failure-%d", 
		  GetOctetString(ls->item.id), ls->item.status);
	  Tcl_AppendElement(interp, temp);
	}
      }
    }
  } else {
    if (pr->deleteOperationStatus == 0) {	/* successes	*/
      for (ls = pr->deleteListStatuses; ls; ls = ls->next) {
	if (ls->item.status == 0) {
	  sprintf(temp, "%s successfully_deleted", 
		  GetOctetString(ls->item.id), ls->item.status);
	  Tcl_AppendElement(interp, temp);
	  if (ac_list_search(session->c_resultSetNames, 
			  GetOctetString(ls->item.id)) == 1) {
	    ac_list_delete(session->c_resultSetNames, 
			GetOctetString(ls->item.id));
	  }  
	}
      }

    } else if (pr->deleteOperationStatus == 9 
	       || pr->deleteOperationStatus == 8) {
      /* walk through the deleteListStatuses */
      for (ls = pr->deleteListStatuses; ls; ls = ls->next) {
	if (ls->item.status == 0) {
	  sprintf(temp, "%s successfully_deleted", 
		  GetOctetString(ls->item.id), ls->item.status);
	  Tcl_AppendElement(interp, temp);
	  if (ac_list_search(session->c_resultSetNames, 
			  GetOctetString(ls->item.id)) == 1) {
	    ac_list_delete(session->c_resultSetNames, 
			GetOctetString(ls->item.id));
	  }  
	}
	else {
	  sprintf(temp, "%s failure-%d", 
		  GetOctetString(ls->item.id), ls->item.status);
	  Tcl_AppendElement(interp, temp);
	}
      }
    } 
  }
  
  return TCL_OK;
}
  

void
LogPDU (FILE *file, PDU *pdu) 
{
  if (LOGGING_ON)
    PrintPDU(file,pdu);
}


#ifndef WIN32
/* TKZ_EncryptCmd --
 *
 *  Handles the creation of encrypted keys 
 *
 *
 * Results:
 *	the encrypted keyword
 *
 * This code was provided by Ashley Sanders a.sanders@mcc.ac.uk
 *                           of Manchester computing
 * Thanks!
 *----------------------------------------------------------------------
 */


int
TKZ_EncryptCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
  char *pcText, *pcSalt = "DP";
  char *Tcl_GetStringFromObj(), *crypt();
  int iLen;
  if (objc < 2 || objc > 3)
    {
      Tcl_WrongNumArgs (interp, 1, objv, "plaintext ?salt?");
      return TCL_ERROR;
    }
  pcText = Tcl_GetStringFromObj (objv[1], &iLen);
  if (objc > 2)
    pcSalt = Tcl_GetStringFromObj (objv[2], &iLen);
  Tcl_SetResult (interp, crypt(pcText, pcSalt), TCL_VOLATILE);
  return TCL_OK;
}

#endif



