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
 * zAppInit.c --  By Aitao Chen, v. 1.0, Dec. 1995
 *                Heavily modified by
 *                Ray Larson, v. 2.0 June 1996.
 *
 *	Tcl command extensions for Z39.50
 *
 *      Changes and bug fixes by Ray R. Larson, Feb. 1996
 *
 * Copyright (c) 1995-1996 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "tcl.h"
#include "list.h"
#include "connect.h"
#include "z3950_3.h"
#include "z3950_intern.h"
#include "session.h"
#include "z_parameters.h"
#include "cheshire_version.h"

int debug_query = 0;	/* Print out tree representaion of the search query */
extern int debug_asn;	/* Print out the asn tree representation of a pdu */
extern int debug;	/* somewhere in the ber_decode.c file		*/

extern int MultipleConnectRequests(DESTREC *, int);
extern int ExecuteParallelSearch (ZASSOCIATION *, char *, char *);
extern int PrintDocument (FILE *, ZSESSION *, int,  char *);

static char *ZBSTR[] = {
	"NO",
	"YES",
	"UNKNOWN"
};


extern ZSESSION *TKZ_User_Session_TMP;
extern ZSESSION *TKZ_User_Session;


int
InitSession (session)
ZSESSION *session;
{


	if (!session)
		return Z_ERROR;
	/* Info about the client should be read from an initialization
	 * file. TO BE IMPLEMENTED.
	 */
	if (TKZ_User_Session_TMP != NULL) {
	  /* this means things were already initialized before attempting */
          /* to make a connection -- so we will use whatever values are   */
          /* in the structure.                                            */
	  return Z_OK;
	}
	else { /* initializing from scratch */
	  /* don't free the following -- constant is used as value */
	  session->preferredRecordSyntax = MARCRECSYNTAX;
	  strcpy(session->resultSetName, "Default");
	  strcpy(session->presentElementSet, "F");
	  session->numberOfRecordsRequested = 10;
	  session->startPosition = 1;

	  session->c_attributeSetId = OID_BIB1;
#ifdef Z3950_2001
	  session->c_preferredMessageSize = 320000;
	  session->c_exceptionalRecordSize = 20000000;
#else
	  if (session->c_preferredMessageSize == 0)
	    session->c_preferredMessageSize = 32000;

	  if (session->c_exceptionalRecordSize == 0)
	    session->c_exceptionalRecordSize = 2000000;
#endif
	  session->c_implementationId = strdup("UCB_Z_V3_2004");
	  session->c_implementationName = strdup("UC Berkeley Z39.50 Version 3 Client");
	  session->c_implementationVersion = strdup(CHESHIRE_VERSION);
	  session->c_protocolVersion = strdup("111");
	  /* The options bitstring is 
	   * Search = (options[0] == '1')
	   * Present = (options[1] == '1')
	   * DeleteResultSet = (options[2] == '1')
	   * ResourceReport = (options[3] == '1')
	   * TriggerResourceControl = (options[4] == '1')
	   * ResourceControl = (options[5] == '1')
	   * AccessControl = (options[6] == '1')
	   * Scan = (options[7] == '1')
	   * Sort = (options[8] == '1')
	   * ExtendedServices = (options[10] == '1') 
	   * Level_1_Segmentation = (options[11] == '1')
	   * Level_2_Segmentation = (options[12] == '1')
	   * ConcurrentOperations = (options[13] == '1')
	   * NamedResultSets = (options[14] == '1')
	   *                                     11111        
	   *                           012345678901234                 */
	  session->c_options = strdup("111000011000001");
	  session->c_timeOut = 300;	/* in seconds	*/
	  session->c_smallSetUpperBound = SMALL_SET_UPPER_BOUND;
	  session->c_largeSetLowerBound = LARGE_SET_LOWER_BOUND;
	  session->c_mediumSetPresentNumber = MEDIUM_SET_PRESENT_NUMBER;

	  session->c_supportVersion1 = Z_TRUE;
	  session->c_supportVersion2 = Z_TRUE;
	  session->c_supportVersion3 = Z_TRUE;
	  
	  session->c_requestSearch = Z_TRUE;
	  session->c_requestPresent = Z_TRUE;
	  session->c_requestDeleteResultSet = Z_TRUE;
	  session->c_requestResourceReport = Z_FALSE;
	  session->c_requestTriggerResourceControl = Z_FALSE;
	  session->c_requestResourceControl = Z_FALSE;
	  session->c_requestAccessControl = Z_FALSE;
	  session->c_requestScan =  Z_TRUE;
	  session->c_requestSort = Z_TRUE;
	  session->c_requestExtendedServices = Z_FALSE;
	  session->c_requestLevel_1_Segmentation = Z_FALSE;
	  session->c_requestLevel_2_Segmentation = Z_FALSE;
	  session->c_requestConcurrentOperations = Z_FALSE;
	  session->c_requestNamedResultSets = Z_TRUE;

	  session->s_supportType0Query = Z_UNKNOWN;
	  session->s_supportType2Query = Z_UNKNOWN;
	  session->s_supportType100Query = Z_UNKNOWN;
	  session->s_supportType101Query = Z_UNKNOWN;
	  session->s_supportElementSetNames = Z_UNKNOWN;
	  session->s_supportSingleElementSetName = Z_UNKNOWN;
	  session->multipleDatabasesSearch = Z_UNKNOWN;
	  session->namedResultSet = Z_UNKNOWN;
      }
	return Z_OK;
}

void
FreeSession (session)
ZSESSION *session;
{
  RESULT_SET *rescurr, *resnext;
  DOCUMENT *document;
  int i;

  if (session->queryString) 
    free(session->queryString);		/* the last search query */

  for (i=0; i<session->numberResultSets; i++)
    if (session->resultSetList[i])
      free(session->resultSetList[i]);
  

  for (rescurr = session->s_resultSets; rescurr!=NULL; rescurr=resnext) {
    resnext=rescurr->next;
    free (rescurr);
  }

	
  ac_list_free(session->c_resultSetNames);	/* list of result set names that the
					 * server maintains for this session.
					 */

  FreePDU((PDU *)session->lastInPDU);   /* the last incoming pdu	*/
  FreePDU((PDU *)session->lastOutPDU);  /* the last outgoing pdu	*/


  /* free any documents from last retrieval */
  if (session->documentList != NULL) {
    for (i=1; i <= session->resultCount; i++) {
      document = session->documentList[i];
      if (document != NULL) {
	if (document->data) free((char *)document->data);
	free(document);
      }
    }
    free((char *)session->documentList);
  }

  FreeDefaultDiagRec(session->diagRec);	/* diagnostic record.		*/


  ac_list_free(session->DIDList);		/* List of document ids.*/

  if(session->searchResult) {
    if(session->searchResult->resultSet)
      ac_list_free(session->searchResult->resultSet);
    free((char *)session->searchResult);
  }

  if(session->s_serverMachineName)
    free((char *)session->s_serverMachineName);
  if(session->s_serverIP)
    free((char *)session->s_serverIP);
  if(session->s_protocolVersion)
    free((char *)session->s_protocolVersion);
  if(session->s_options)
    free((char *)session->s_options);
  if(session->s_implementationId)
    free((char *)session->s_implementationId);
  if(session->s_implementationName)
    free((char *)session->s_implementationName);
  if(session->s_implementationVersion)
    free((char *)session->s_implementationVersion);

  ac_list_free(session->s_databaseNames);
  ac_list_free(session->s_accessPoints);

  /* these only occur in the server and shouldn't waste much space 
   *ELEMENT_SETS	session->s_smallElementSetNames;
   *ELEMENT_SETS	session->s_mediumElementSetNames;
   */

  /* This is not allocated dynamically -- a single buffer is used
   * if(*s_logFileName)
   *    free((char *)*s_logFileName); 
   */

  for(i = 0; i < 10; i++)
    if(session->c_databaseNames[i])
      free((char *)session->c_databaseNames[i]);

  if(session->s_resultSetDirectory)
    free((char *)session->s_resultSetDirectory);

  /* This will be set to a constant only -- don't free it
   * if(session->s_attributeSetId)
   *    free((char *)session->s_attributeSetId);
   */

  if(session->c_clientMachineName)
    free((char *)session->c_clientMachineName);
  if(session->c_clientIP)
    free((char *)session->c_clientIP);
  
  if(session->c_referenceId)
    free((char *)session->c_referenceId);
  if(session->c_protocolVersion)
    free((char *)session->c_protocolVersion);
  if(session->c_options)
    free((char *)session->c_options);
  if(session->c_implementationId)
    free((char *)session->c_implementationId);
  if(session->c_implementationName)
    free((char *)session->c_implementationName);
  if(session->c_implementationVersion)
    free((char *)session->c_implementationVersion);
  
  free((char *)session);

}

ZSESSION *
SearchAssociation (assocs, hostName, portNumber, database)
     ZASSOCIATION *assocs;
     char 	*hostName;
     int	portNumber;
     char       *database;
{
  ZSESSION 	*session;
  if (assocs == NULL)
    return (ZSESSION *)NULL;
  for (session=assocs->head; session!=NULL; session=session->next)
    if ((session->s_portNumber == portNumber) &&
	(strcasecmp(session->s_serverMachineName, hostName) == 0) &&
	(strcasecmp(session->databaseName, database) == 0))
      return session;
  return (ZSESSION *)NULL;
}

/*
 * Return the session record for the connection fd.
 */

ZSESSION *
LookupAssociation (assocs, sessionID)
ZASSOCIATION *assocs;
int	sessionID;	/* It is the same as the file descriptor.	*/
{
    ZSESSION 	*session;
    if (!assocs)
	return (ZSESSION *)NULL;
    for (session=assocs->head; session!=NULL; session=session->next)
	if (session->fd == sessionID)
	    return session;
    return (ZSESSION *)NULL;
}

int
PrintConnectionInfo (assocs)
ZASSOCIATION *assocs;
{
    ZSESSION 	*session;
    if (!assocs)
	return Z_ERROR;
    printf("%10s\t%-30s\t%s\n", "Session ID", "Host", "Port");
    printf("%10s\t%-30s\t%s\n", "----------", "----", "----");
    for (session=assocs->head; session!=NULL; session=session->next) {
	printf("%-10d\t%-30s\t%d\n", 
		session->fd,
	    	session->s_serverMachineName,
		session->s_portNumber);
    }
    return Z_OK;
}


/*
 * Remove the session record from the linked list.
 */
int
RemoveAssociation (assocs, sessionID)
ZASSOCIATION *assocs;
int	sessionID;	/* It's the same as the file descriptor.	*/
{
    ZSESSION *tmp;
    if (assocs == NULL)
	return Z_ERROR;
    tmp = assocs->head;
    if ((tmp->fd == sessionID) && (tmp == assocs->tail)) {
	assocs->head = NULL;
	assocs->tail = NULL;
	assocs->counts = 0;
	return Z_OK;
    }

    if ((tmp->fd == sessionID) && (tmp != assocs->tail)) {
	assocs->head = tmp->next;
	tmp->next->prev = NULL;
	assocs->counts -= 1;
	return Z_OK;
    }

    for(tmp = assocs->head; tmp!=NULL; tmp=tmp->next) {
	if (tmp->fd == sessionID) {
	    if (tmp == assocs->tail)	{
		assocs->tail = tmp->prev;
		tmp->prev->next = NULL;
	    } else {
		tmp->prev->next = tmp->next;
		tmp->next->prev = tmp->prev;
	    }
	    assocs->counts -= 1;
	    return Z_OK;
	}
    }
    return Z_ERROR;
}

int
ConnectCmd(clientData, interp, argc, argv)
ClientData clientData;               
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session;
    ZASSOCIATION *associations;
    int portNumber;
    char *hostName;
    char temp[50];
    GSTR *authentication = NULL, *NewGSTR();
    

    if (argc < 3 || argc > 4) {
	Tcl_AppendResult(interp, "Syntax ", argv[0],
		" hostName portNumber {authentication} \"", (char *) NULL);
	return TCL_ERROR;
    }
    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    
    if (Tcl_GetInt(interp, argv[2], &portNumber) != TCL_OK)
	return TCL_ERROR;
    hostName = argv[1];

    if (argc == 4) {
      authentication = NewGSTR(argv[3]);
    }
    else
      authentication = NULL;

    /* If there is already an active connection to the same host machine
     * at the same port, return the connection id (i.e. the file descriptor
     * for the connection). However multiple connections to the same machine
     * with different port addresses are allowed.
     */
    if ((session = SearchAssociation(associations, hostName, portNumber)) 
		!= NULL) {
	if (session->connectionStatus == Z_OPEN) {
	    sprintf(temp, "%d", session->fd);
	    Tcl_AppendResult(interp, temp, (char *) NULL);
	    return TCL_OK;
	} else {		/* connection has been shut down.	*/
            RemoveAssociation (associations, session->fd);
            (void) FreeSession (session);
	}
    }
    session = tk_connect(hostName, portNumber, authentication);
    if (session == NULL) {	/* happens only when out of memory.	*/
      Tcl_AppendResult(interp, "out of memory" , (char *) NULL);
      return TCL_ERROR;
    } 

    if (session->status != Z_OK) {
	Tcl_AppendResult(interp, "-1" , (char *) NULL);
	free ((char *)session);
	return TCL_ERROR;
    }
    session->connectionStatus = Z_OPEN;
    sprintf(temp, "%d", session->fd);
    Tcl_AppendResult(interp, temp, (char *) NULL);

    session->prev = NULL;
    session->next = NULL;
    if (associations->head == NULL) {
	associations->head = session;
	associations->tail = associations->head;
	associations->counts = 1;
    } else {
	session->prev = associations->tail;
	associations->tail->next = session;
	associations->tail = session;
	associations->counts += 1;
    }
    FD_SET(session->fd, &(associations->active_fd_set));
    associations->no_fd_set += 1;
    if (session->fd > associations->maximum_fd)
	associations->maximum_fd = session->fd;
    return TCL_OK;
}

int
MConnectCmd(dummy, interp, argc, argv)
ClientData dummy;                   /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    int portNumber;
    int numberServers;
    int i;
    DESTREC *drec;

    if (argc < 3) {
	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		" hostname portnumber \"", (char *) NULL);
	return TCL_ERROR;
    }

    numberServers = (argc-1)/2;
    drec = (DESTREC *)malloc(numberServers * sizeof(DESTREC));
    for (i=0; i<numberServers; i++) {
	strcpy(drec[i].hostname, argv[1+i*2]);
      	Tcl_GetInt(interp, argv[2+i*2], &portNumber);
	drec[i].port = portNumber;
    } 
    MultipleConnectRequests(drec, numberServers);
    /*
     * Free the space of drec.
     */
    return TCL_OK;
}

int
DisconnectCmd(clientData, interp, argc, argv)
ClientData clientData;              /* Client data. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session, *tmp;
    ZASSOCIATION *associations;
    int sessionID;

    if (argc < 2) {
	Tcl_AppendResult(interp, "Syntax: ", argv[0],
		" [all | sessionID sessionID ...]", (char *) NULL);
	return TCL_ERROR;
    }
    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    if (strcasecmp(argv[1], "all") == 0) {
	for (session=associations->head; session!=NULL; session=session->next) {
    	    if (tk_disconnect(session) == Z_OK)
    		printf("connection to %s at %d has been closed.\n",
		    session->s_serverMachineName, session->s_portNumber);
	}
	for (session=associations->head; session!=NULL; ) {
	    tmp = session;
	    session = session->next;
    	    (void)FreeSession (tmp);
	}
	return TCL_OK;
    }

    argc--;
    argv++;
    for (; argc>0; argc--,argv++) {
        if (Tcl_GetInt(interp, argv[0], &sessionID) != TCL_OK) 
	    continue;
        if ((session = LookupAssociation (associations, sessionID)) == NULL) {
	    printf("connection %d is not closed.\n", sessionID);
	    continue;
	}
        if (tk_disconnect(session) == Z_OK)
	    printf("connection to %s at %d has been closed.\n",
	        session->s_serverMachineName, session->s_portNumber);
        if (RemoveAssociation (associations, sessionID) != Z_OK)
	    continue;
        (void) FreeSession (session);
    }
    return TCL_OK;
}

int
QuitCmd(clientData, interp, argc, argv)
ClientData clientData;              /* Client data. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session, *tmp;
    ZASSOCIATION *associations;

    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    for (session=associations->head; session!=NULL; session=session->next) {
        if (tk_disconnect(session) == Z_OK)
	    printf("connection to %s at %d has been closed.\n",
	        session->s_serverMachineName, session->s_portNumber);
    }
    for (session=associations->head; session!=NULL; ) {
        tmp = session;
        session = session->next;
        (void)FreeSession (tmp);
    }
    associations->head = NULL;
    associations->tail = NULL;
    associations->counts = 0;
	
    return TCL_OK;
}

int
SearchCmd(clientData, interp, argc, argv)
ClientData clientData;                   /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session;
    ZASSOCIATION *associations;
    int sessionID;
    int queryType;
    char *databaseName;
    char *resultSetName;
    char *queryString;

    if (argc != 6) {
	Tcl_AppendResult(interp, "Syntax: ", argv[0],
	    " sessionID queryType database resultset {query} ",(char *) NULL);
	return TCL_ERROR;
    }
    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK) 
	return TCL_ERROR;
    if (Tcl_GetInt(interp, argv[2], &queryType) != TCL_OK) 
	return TCL_ERROR;
    databaseName = argv[3];
    resultSetName = argv[4];
    queryString = argv[5];

	printf("%s\n", queryString);

    /* There must be a connection to the server before a search request
     * can be sent out.
     */
    if ((session = LookupAssociation (associations, sessionID)) == NULL)
	return TCL_ERROR; 
    if (session->connectionStatus != Z_OPEN) {
	printf("connection to %s at port %d is closed, you need to reestablish the connection.\n", session->s_serverMachineName, session->s_portNumber);
	return TCL_ERROR;
    }
    if (session->s_supportSearch != Z_TRUE) {
	printf("server %s does not support search.\n", session->s_serverMachineName);
	return TCL_ERROR;
    }

    if (!((queryType == 0) ||
	  (queryType == 1) ||
	  (queryType == 2) ||
	  (queryType == 100) ||
	  (queryType == 101) ||
	  (queryType == 1000))) {
      Tcl_AppendResult(interp, 
		       "invalid query type.",
		       " query type takes on value: 0, 1, 2, 100 and 101", 
		       (char *) NULL);

	return TCL_ERROR;
    }

    /* Now use type 1 query only.	*/
    if (queryType != 1) {
      Tcl_AppendResult(interp, 
		       "Please use type 1 query only.",
		       (char *) NULL);
		       
      return TCL_ERROR;
    }

    if (tk_search(session, queryType, databaseName, resultSetName, queryString) != Z_OK)
      return TCL_ERROR;

    if (session->searchStatus != 0) {	/* OK.	*/
      char temp[100];
      sprintf(temp, 
	      "%d records retrieved,\t%d records returned,\tnext result set position: %d\n",
	      session->resultCount, 
	      session->noRecordsReturned,
	      session->nextResultSetPosition);
      session->searchStatus = 1;
      Tcl_AppendResult(interp, temp, (char *) NULL);
      return TCL_OK;
    } 
    /* A diag record returned. Print out the content of the diag record.
     * result is limited to 200 bytes.
     * NOTE: Some systems may not return a diag record.
     */
    
    if ((session->diagRecPresent == 1) && (session->diagRec != NULL)) {
	PrintNonSurrogateDiagnostic (session->diagRec);
    }
    return TCL_OK;
}

int
MSearchCmd(clientData, interp, argc, argv)
ClientData clientData;              /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    int sessionID;
    ZSESSION *session;
    ZASSOCIATION *associations;
    int i;
    char *resultSet;
    char *queryString;
    int queryType;

    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    if (argc < 6) {
	Tcl_AppendResult(interp, "Syntax: ", argv[0],
	   " {query} queryType resultSet sessionID database ...",(char *)NULL);
	return TCL_ERROR;
    }
    queryString = argv[1];

    if (Tcl_GetInt(interp, argv[2], &queryType) != TCL_OK)
	return TCL_ERROR;

    if (queryType != 1) {
	Tcl_AppendResult(interp,"ERROR: query type must be 1" , (char *)NULL);
	return TCL_ERROR;
    }
    resultSet = argv[3];
    for (session=associations->head; session!=NULL; session=session->next) {
	session->parallelSearch = Z_FALSE;
    }
    for (i=4; i<argc; ) {
    	if (Tcl_GetInt(interp, argv[i], &sessionID) != TCL_OK) 
	    return TCL_ERROR;
    	if ((session = LookupAssociation (associations, sessionID)) == NULL)
	    return TCL_ERROR; 
	i++;
	if (i == argc) {	/* Database name is missing	*/
	  char temp[60];
	  sprintf(temp, 
		  "ERROR: database name is missing for session %d", 
		  sessionID);
	  Tcl_AppendResult(interp,temp , (char *)NULL);
	  return TCL_ERROR;
	}
	strcpy (session->databaseName, argv[i]);
	i++;
	session->parallelSearch = Z_TRUE;
    }
    ExecuteParallelSearch(associations, queryString, resultSet);

    /* print out the search responses from all the hosts.
     */
    for (session=associations->head; session!=NULL; session=session->next) {
	if ((session->parallelSearch == Z_TRUE) && 
	    (session->state == S_SEARCH_RECEIVED)) {
	    if ((session->diagRecPresent) && (session->diagRec != NULL))  {
		printf("%d\t", session->fd);
		PrintDiagRec (session->diagRec);
	    } else {
		printf("%d\t%d records retrieved, %d records returned\n",
		    	session->fd, 
			session->resultCount,
			session->noRecordsReturned);
	    }
	}
    }
    return TCL_OK;
}

int
PresentCmd(clientData, interp, argc, argv)
ClientData clientData;                   /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session;
    ZASSOCIATION *associations;
    int sessionID;
    int startPoint;
    int numberOfRecordsRequested;
    char *resultSet;

    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;

    if (argc != 5) {
	Tcl_AppendResult(interp, "Syntax: ", argv[0],
	" sessionID resultSet startPosition numberRecordsRequested", 
			 (char *) NULL);
	return TCL_ERROR;
    }

    if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK) {
      char temp[60];
      sprintf(temp, "invalid session id: %s", argv[1]);
      Tcl_AppendResult(interp, temp,  (char *) NULL);
      return TCL_ERROR;
    }
    /* There must be a connection to the server before a present request
     * can be sent out. A present request should be issued only after a
     * search request.
     */
    if ((session = LookupAssociation (associations, sessionID)) == NULL)
	return TCL_ERROR; 
    if (session->connectionStatus != Z_OPEN) {
	printf("connection to %s at port %d is closed, you need to reestablish the connection.\n", session->s_serverMachineName, session->s_portNumber);
	return TCL_ERROR;
    }
    if (session->s_supportPresent != Z_TRUE) {
	printf("server %s doesn't support present.\n",session->s_serverMachineName);
	return TCL_ERROR;
    }
    if (session->searchStatus == 0) {
	printf("There are no records to be presented.\n");
	printf("You haven't done a search successfully.\n");
	return TCL_ERROR;
    }

    resultSet = argv[2];
    if (Tcl_GetInt(interp, argv[3], &startPoint) != TCL_OK)
	return TCL_ERROR;
    if (Tcl_GetInt(interp, argv[4], &numberOfRecordsRequested) != TCL_OK)
	return TCL_ERROR;

    tk_present(session, resultSet, startPoint, numberOfRecordsRequested, NULL, NULL);
    if (session->presentStatus == 5) {	/* failure.	*/
	/* result is limited to 200 bytes.	*/
	if ((session->diagRecPresent) && (session->diagRec != NULL))
		PrintNonSurrogateDiagnostic(session->diagRec);
    } else {			/* some records should have been returned. */
      char temp[60];
      sprintf(temp, " %d Records Returned, Next Result Set Position: %d",
	      session->noRecordsReturned,
	      session->nextResultSetPosition);
      Tcl_AppendResult(interp, temp,  (char *) NULL);

    }
    return TCL_OK;
}

int
PresentCmd2(clientData, interp, argc, argv)
ClientData clientData;                   /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
    ZSESSION *session;
    ZASSOCIATION *associations;
    int sessionID;
    int startPoint;
    int numberRecordsRequested;
    int preferredNumberRecords;
    int numberRecords;
    int remainingRecords;
    char *resultSet;

    if ((associations = (ZASSOCIATION *)clientData) == NULL)
	return TCL_ERROR;
    if (argc != 5) {
	Tcl_AppendResult(interp, "Syntax: ", argv[0],
	" sessionID resultSet startPosition numberRecordsRequested", (char *) NULL);
	return TCL_ERROR;
    }
    if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK) {
      char temp[60];
      sprintf(temp, "invalid session id: %s", argv[1]);
      Tcl_AppendResult(interp, temp,  (char *) NULL);
      return TCL_ERROR;
    }
    if ((session = LookupAssociation (associations, sessionID)) == NULL)
      return TCL_ERROR; 
    if (session->connectionStatus != Z_OPEN) {
      printf("connection to %s at port %d is closed, you need to reestablish the connection.\n", session->s_serverMachineName, session->s_portNumber);
      return TCL_ERROR;
    }
    if (session->searchStatus == 0) {
      printf("There is no records to be presented.\n");
      printf("You haven't done a search successfully.\n");
      return TCL_ERROR;
    }
    if (session->s_supportPresent != Z_TRUE) {
      printf("server %s doesn't support present.\n",session->s_serverMachineName);
      return TCL_ERROR;
    }
    resultSet = argv[2];
    if (Tcl_GetInt(interp, argv[3], &startPoint) != TCL_OK)
      return TCL_ERROR;
    if (Tcl_GetInt(interp, argv[4], &numberRecords) != TCL_OK)
      return TCL_ERROR;
    if ((startPoint > session->resultCount) || (startPoint <= 0)) {
      printf("startPoint (%d) is out of range. There are only %d records retrieved.\n", startPoint, session->resultCount);
      return TCL_ERROR;
    }
    if (numberRecords <= 0) {
      printf("numberRecords (%d) is out of range.\n", numberRecords);
      return TCL_ERROR;
    }
    if (numberRecords + startPoint > session->resultCount + 1)
      numberRecords = session->resultCount + 1 - startPoint;

    preferredNumberRecords = session->s_preferredMessageSize / 1000;
    remainingRecords = numberRecords;
    while (remainingRecords > 0) {
      if (remainingRecords < preferredNumberRecords)
	numberRecordsRequested = remainingRecords;
      else
	numberRecordsRequested = preferredNumberRecords;
      if (numberRecordsRequested+startPoint > session->resultCount+1)
	numberRecordsRequested = session->resultCount + 1 - startPoint;
      tk_present(session, resultSet, startPoint, numberRecordsRequested, 
		 NULL, NULL);
      if (session->presentStatus == 5) {		/* failure.	*/
	if ((session->diagRecPresent) && (session->diagRec != NULL)) {
	  PrintNonSurrogateDiagnostic(session->diagRec);
	  printf("total %d records returned.\n",
		 numberRecords - remainingRecords); 
	  break;
	}
      } else {		/* some records should have been returned. */
	printf("%d records returned. Next result set position: %d\n",
	       session->noRecordsReturned,
	       session->nextResultSetPosition);
	/* The nextResultSetPosition returned is not reliable, some
	 * systems return the wrong result set position after a search
	 * or present request.
	 */
	/*
	   startPoint = session->nextResultSetPosition;
	   */
	startPoint += session->noRecordsReturned;
	remainingRecords -= session->noRecordsReturned;
	
	/* Watch out for infinite loop. If no record is returned, do not
	 * continue to send present request to the server. Something is
	 * wrong with the server.
	 */
	if (session->noRecordsReturned <= 0)
	  break;
      }
    }
    return TCL_OK;
}


int
ResourceReportCmd(clientData, interp, argc, argv)
     ClientData clientData;              /* Not used. */
     Tcl_Interp *interp;                 /* Current interpreter. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings. */
{
  int rc;
  
  if (argc != 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " sessionID resourceReportFormat\"", (char *) NULL);
    return TCL_ERROR;
  }
  rc = tk_ResourceReport(argv[1], argv[2]);
  return TCL_OK;
}

int
TriggerResourceControlCmd(dummy, interp, argc, argv)
ClientData dummy;                   /* Not used. */
Tcl_Interp *interp;                 /* Current interpreter. */
int argc;                           /* Number of arguments. */
char **argv;                        /* Argument strings. */
{
  int rc;
  int operation, resultSetWanted;
  if (argc != 5) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " referenceID requestedOperation preferredResourceReportFormat resultSetWanted\"", (char *) NULL);
    return TCL_ERROR;
  }
  Tcl_GetInt(interp, argv[2], &operation);
  Tcl_GetInt(interp, argv[4], &resultSetWanted);
  rc = tk_TriggerResourceControlRequest(argv[1],operation,argv[3],resultSetWanted);
  return TCL_OK;
}

int
Buf_Tcl(interp, arry, buf)
     Tcl_Interp *interp;
     char *arry;             /* associate array in tcl. */
     AC_LIST *buf;      /* list of items        */
{
  int i;
  char subs[10];
  char *data;
  
  if (buf == NULL) {
    Tcl_SetVar2(interp, arry, "0", "Empty Content", TCL_GLOBAL_ONLY);
    return TCL_OK;
  }
  for (i=0, data=ac_list_first(buf); data != NULL; data=ac_list_next(buf), i++) {
    sprintf(subs, "%d", i);
    Tcl_SetVar2(interp, arry, subs, data, TCL_GLOBAL_ONLY);
  }   
  return TCL_OK;
}
 
int
parseMarcCmd(dummy, interp, argc, argv)
     ClientData dummy;                   /* Not used. */
     Tcl_Interp *interp;                 /* Current interpreter. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings. */
{
  AC_LIST *slicedMarc;
  
  if (argc != 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " fullMarc slicedMarc\"", (char *) NULL);
    return TCL_ERROR;
  }
  slicedMarc = (AC_LIST *)parseMarc(argv[1]);
  Tcl_UnsetVar(interp, argv[2], TCL_GLOBAL_ONLY);
  Buf_Tcl(interp, argv[2], slicedMarc);
  ac_list_free(slicedMarc);
  return TCL_OK;
}


int
GetPassword (interp, file)
     Tcl_Interp *interp;
     char *file;
{
  int code;
  
  code = Tcl_EvalFile (interp, file);
  if (code != TCL_OK) {
    fprintf(stderr, "%s\n", interp->result);
  }
  return TCL_OK;
}

int
DisplayCmd(clientData, interp, argc, argv)
     ClientData clientData;               
     Tcl_Interp *interp;                 /* Current interpreter. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings. */
{
  int sessionID;
  ZSESSION *session;
  ZASSOCIATION *associations;
  int startRecord;
  int endRecord;			/* no of records requested	*/
  char *format;			/* MARC or SHORT or LONG	*/
  int i;
  char temp[80];

  if (argc != 5) {
    Tcl_AppendResult(interp, "Syntax: ", argv[0],
		     " sessionID startRecord endRecord format", (char *) NULL);
    return TCL_ERROR;
  }
  if ((associations = (ZASSOCIATION *)clientData) == NULL)
    return TCL_ERROR;
  if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK)
    return TCL_ERROR;
  if ((session = LookupAssociation (associations, sessionID)) == NULL)
    return TCL_ERROR; 
  if (Tcl_GetInt(interp, argv[2], &startRecord) != TCL_OK)
    return TCL_ERROR;
  if (Tcl_GetInt(interp, argv[3], &endRecord) != TCL_OK)
    return TCL_ERROR;
  format = argv[4];
  if (!((strcasecmp(format, "MARC") == 0) ||
	(strcasecmp(format, "SHORT") == 0) ||
	(strcasecmp(format, "LONG") == 0))) {
    sprintf(temp, "format takes as value: MARC, LONG, SHORT");
    Tcl_AppendResult(interp, temp,  (char *) NULL);
    return TCL_ERROR;
    } 
  if (endRecord < startRecord) {
    printf("<endRecord> must be at least as large as <startRecord>.\n");
    return TCL_ERROR;
  }
  if ((endRecord > session->resultCount) || (endRecord <= 0))
    endRecord = session->resultCount;
  if (startRecord <= 0)
    startRecord = 1;
  for (i=startRecord; i<=endRecord; i++) 
    PrintDocument (stdout, session, i, format);
  return TCL_OK;
}

int
DisplayCmd2(clientData, interp, argc, argv)
     ClientData clientData;               
     Tcl_Interp *interp;                 /* Current interpreter. */
     int argc;                           /* Number of arguments. */
     char **argv;                        /* Argument strings. */
{
  int sessionID;
  ZSESSION *session;
  ZASSOCIATION *associations;
  int start;
  int numberRecords;			/* no of records requested	*/
  char *format;			/* MARC or SHORT or LONG	*/
  char *resultSet;
  int i;
  int startRecord;
  int endRecord;
  int preferredNumberRecords;
  char temp[60];

  if (argc != 6) {
    Tcl_AppendResult(interp, "Syntax: ", argv[0],
		     " sessionID resultSet startRecord endRecord format", (char *) NULL);
    return TCL_ERROR;
  }
  if ((associations = (ZASSOCIATION *)clientData) == NULL)
    return TCL_ERROR;
  if (Tcl_GetInt(interp, argv[1], &sessionID) != TCL_OK)
    return TCL_ERROR;
  resultSet = argv[2];
  if ((session = LookupAssociation (associations, sessionID)) == NULL)
    return TCL_ERROR; 
  if (Tcl_GetInt(interp, argv[3], &startRecord) != TCL_OK)
    return TCL_ERROR;
  if (Tcl_GetInt(interp, argv[4], &endRecord) != TCL_OK)
    return TCL_ERROR;
  format = argv[5];
  if (!((strcasecmp(format, "MARC") == 0) ||
	(strcasecmp(format, "SHORT") == 0) ||
	(strcasecmp(format, "LONG") == 0))) {
    sprintf(temp, "format takes as value: MARC, LONG, SHORT");
    Tcl_AppendResult(interp, temp,  (char *) NULL);
    return TCL_ERROR;
  } 
  if (session->connectionStatus != Z_OPEN) {
    printf("connection to %s at port %d is closed, you need to reestablish the connection.\n", session->s_serverMachineName, session->s_portNumber);
    return TCL_ERROR;
  }
  if (session->searchStatus == 0) {
    printf("There is no records to be presented.\n");
    printf("You haven't done a search successfully.\n");
    return TCL_ERROR;
    }
  if (endRecord < startRecord) {
    printf("<to> must be at least as large as <from>.\n");
    return TCL_ERROR;
  }
  if ((endRecord > session->resultCount) || (endRecord <= 0))
    endRecord = session->resultCount;
  if (startRecord <= 0)
    startRecord = 1;
  
  /* If there is no local copy of the records requested, call present
   * request to get the records from the source, then display the records
   * in appropriate format.
   */
  preferredNumberRecords = session->preferredMessageSize/1000;
  numberRecords = 0;
  start = 0;
  for (i=startRecord; i<=endRecord; i++) {
    if (session->documentList[i] == NULL) {
      if (start == 0)
	start = i;			/* start from record i	*/
      numberRecords++;
      if ((i < endRecord) && (numberRecords < preferredNumberRecords))
	continue;
    }
    /* call present request to get those missing records from the source.
     */
    if ((numberRecords > 0) && (start > 0)) {
      if (start+numberRecords > session->resultCount+1)
	numberRecords = session->resultCount + 1 - start;
      tk_present(session, resultSet, start, numberRecords, NULL, NULL);
      if (session->presentStatus == 5) {		/* failure.	*/
	if ((session->diagRecPresent) && (session->diagRec != NULL))
	  PrintNonSurrogateDiagnostic(session->diagRec);
	/*
	   printf("%d - %s\n", 
	   session->diagRec->condition,
	   session->diagRec->addinfo->data);
	   */
	break;
      } else {		/* some records should have been returned. */
	printf("%d Records Returned, Next Result Set Position: %d\n",
	       session->noRecordsReturned,
	       session->nextResultSetPosition);
	if (session->noRecordsReturned <= 0) {	/* something is wrong */
	  printf("The server %s should have returned some records.\n",
		 session->s_serverMachineName);	
	  break;
	}
      }
      /*
	 start = session->nextResultSetPosition;
	 */
      start += session->noRecordsReturned;
      if (i >= start)
	numberRecords = i - start + 1;
      else
	numberRecords = 0;
    }
  }
  
  for (i=startRecord; i<=endRecord; i++) 
    PrintDocument (stdout, session, i, format);
  return TCL_OK;
}





