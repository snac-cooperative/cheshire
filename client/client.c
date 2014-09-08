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
/**************************************************************************
* 
* Cheshire II Online Catalog System
* 
* Copyright (c) 1994-7 Regents of the University of California
* 
* All rights reserved.
*
* Authors:      Ray Larson, ray@sherlock.berkeley.edu
*               Jerome McDonough, jmcd@lucien.sims.berkeley.edu
*               Aitao Chen,  aitao@info.sims.berkeley.edu
*               Lucy Kuntz,  lkuntz@info.sims.berkeley.edu
*               Ralph Moon,  rmoon@library.berkeley.edu
*
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
***************************************************************************/
#include <stdio.h>
#include <assert.h>
#include "connect.h"
#include "z3950_3.h"
#include "session.h"
#include "ccl.h"
#include "z_parameters.h"
#include "list.h"
#include "clientinter.h"
#include "z3950_intern.h"
#define	PASSWORD	"anonymous"

#ifndef MIN
#define	MIN(a,b) ((a<=b)?(a):(b))
#endif

extern int debug_query;
extern WHOAMI context;	/* Client progrom sets it to Z_CLIENT	*/

int ExecuteSearchCommand(ZSESSION *, Query *, char *, char *);
int ExecuteInitCommand(int, ZSESSION *, GSTR *);
int ExecuteResourceReportRequest(int, char *, char *);
int ExecuteTriggerResourceControlRequest(int, char *, int, char *, int);
void ExecuteDeleteResultSetRequest(int, char *);
extern void FreeOPACRecord(OPACRecord *rec);
extern void FreeExplain_Record(Explain_Record *rec);
extern void FreeGenericRecord(GenericRecord *rec);
extern void FreeTaskPackage(TaskPackage *tp);;
extern void FreeBriefBib(BriefBib *bb);
extern void FreeNonSurrogateDiagnostic(DefaultDiagFormat *dr);
extern void FreeDefaultDiagRec(DefaultDiagFormat *dr);
extern int ProcessSearchResponse (ZSESSION *session, 
				  SearchResponse  *searchResponse);
extern int CopyBitString(char *buffer, OctetString bitString);
extern int InitSession (ZSESSION *session);
extern int ProcessPresentResponse (ZSESSION *session, PresentResponse *pr);
extern int ProcessDeleteResultSetResponse (ZSESSION *session, 
					   DeleteResultSetResponse *dr);

extern PDU *ClientGetPDU(int fd, int timeoutsecs);
extern char *GetBitString(BitString);
extern char *GetOctetString(OctetString);

extern int debug; /* in ber_decode.c */

int CURRENT_FD;

int     DIAG_CONDITION;      /* Diagnostic Set Conditon      */
char    *DIAG_MESSAGES[] = {
#include "diag_messages.data"
};

void
FreeDocument (document)
     DOCUMENT *document;
{
  if (!document)
    return;
  
  switch (document->dtype) {
  case Z_US_MARC:
    (void)free((char *)document->data);
    break;
    
  case Z_OPAC_REC:             /* OPAC record syntax     */
    FreeOPACRecord((OPACRecord *)document->data);
    break;
    
  case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
    FreeExplain_Record((Explain_Record *)document->data);
    break;
    
  case Z_GRS_0_REC:            /* Generic record syntax 0*/
    (void)free((char *)document->data);
    break;
    
  case Z_GRS_1_REC:            /* Generic record syntax 1*/
    FreeGenericRecord((GenericRecord *)document->data);
    break;
    
  case Z_ES_REC:               /* Extended Services      */
    FreeTaskPackage((TaskPackage *)document->data);
    break;
    
  case Z_SUMMARY_REC:          /* Summary record syntax  */
    FreeBriefBib((BriefBib *)document->data);
    break;
    
  default:
    if (document->data) /* must be a SUTRS record ?? */
      (void)free((char *)document->data);
    break;
    /* printf("NOT IMPLEMENTED!\n"); */
  }
  
  (void)free((char *)document);
}

int
ClearPreviousSearch (session)
ZSESSION *session;
{
    int i;
    if (!session)
	return Z_ERROR;
    session->searchStatus = 0;
    session->resultSetStatus = 0;
    session->presentStatus = 0;
    session->noRecordsReturned = 0;
    session->nextResultSetPosition = 0;
    session->totalNumberRecordsReturned = 0;

    session->diagRecPresent = 0;
    if (session->diagRec != NULL)
	(void)FreeNonSurrogateDiagnostic (session->diagRec);
    session->diagRec = NULL;

    if (session->queryString)
	(void)free(session->queryString);
    session->queryString = (char *)NULL;

    /* this is commented out -- we will keep the result set name around */
    /* this means that it will be reused for each search unless replaced*/
    /* memset (session->resultSetName, '\0', MAX_RSN_LENGTH);           */

    for (i=1; i< session->listSize; i++) {
	(void) FreeDocument (session->documentList[i]);
	session->documentList[i] = NULL;
    }
    session->resultCount = 0;
    return Z_OK;
}

int
tk_search(session, queryType, databaseName, resultSetName, queryString)
     ZSESSION *session;	/* session or z_association id.	*/
     int	queryType;	/* what kind of query	*/
     char	*databaseName;	/* One database name. Should be a 
                                        list of db names. */
     char	*resultSetName;	/* result set name 	*/
     char	*queryString;	/* Query in common command language.	*/
{
  Query *query;
  RPNQuery *RPN_Query_ptr, *queryparse();
  char *attrsetid;

  attrsetid = "1.2.840.10003.3.1"; /* bib-1 oid */

  if ((!session) || (!databaseName) || (!resultSetName) || (!queryString))
    return Z_ERROR;
  
    ClearPreviousSearch (session);
  session->queryString = (char *)strdup(queryString);
  if (strlen(resultSetName) >= MAX_RSN_LENGTH)
    strncpy(session->resultSetName, resultSetName, MAX_RSN_LENGTH-1);
  else
    strcpy(session->resultSetName, resultSetName);
  
  switch (queryType) {
  case 0:   /* prior agreement (Can use for SQL?)*/
    query = MakeQueryType0 ((any)queryString);
    break;
    
  case 1:   /* Default RPN query */
    RPN_Query_ptr = queryparse(queryString, attrsetid);
    query = MakeQueryType1(RPN_Query_ptr, attrsetid);
    break;
    
  case 2:  /* ISO 8777 type query */
    query = MakeQueryType2 (queryString);
    break;
    
  case 100: /* Z39.58 (CCL) type query */
    query = MakeQueryType100 (queryString);
    break;
    
  case 101: /* Extended RPN Query */
    RPN_Query_ptr = queryparse(queryString, attrsetid);
    query = MakeQueryType101(RPN_Query_ptr, attrsetid);
    break;
    
  case 102: /* Ranked List Query (proposed v4 extension */
    query = MakeQueryType102 (queryString);
    break;

    /* ----
       case 1000:
       qs = &queryString;
       bt = prefix_to_binary (qs);
       query = PrefixToInfix (bt);
       (void) FreeBinaryTree (bt);
       break;
       ---- */
  default:
    fprintf(stderr, "Client: Unknown query type.\n");
    return Z_ERROR;
  }
  if (query == (Query *)NULL) {
    fprintf(stderr, "Client: Empty query.\n");
    return Z_ERROR;
  }
  return ExecuteSearchCommand(session, query, databaseName, resultSetName);
}

int
ExecuteSearchCommand(session, query, databaseName, resultSetName)
ZSESSION *session;
Query	*query;
char	*databaseName;
char	*resultSetName;
{
    int fd;
    PDU *out_pdu, *in_pdu;
    /* ElementSetNames *ssesn, *msesn; */
    char **dbnames;
    char **esnames;
    char **databaseNames;

    if (!session)
	return Z_ERROR;
    fd = session->fd;		/* file descriptor	*/
    dbnames = StringToArgv("cat pen");
    esnames = StringToArgv("F B");
    databaseNames = StringToArgv (databaseName);
    /*
     * ssesn = MakeElementSetNames(t_databaseSpecific, dbnames, esnames);
     *  msesn = MakeElementSetNames(t_databaseSpecific, dbnames, esnames);
     */
    out_pdu = MakeSearchRequest("SEARCH_REFID", 
		session->c_smallSetUpperBound,
		session->c_largeSetLowerBound,
		session->c_mediumSetPresentNumber,
		Z_TRUE,
		resultSetName,
		databaseNames,
		NULL,		/*	ssesn	*/
		NULL,		/*	msesn	*/
		OID_USMARC,
		query, NULL);

    if (out_pdu == NULL) {
       	printf("Failed to create a search request apdu.\n");
	return Z_ERROR;
    }
    if (PutPDU (fd, out_pdu) != Z_OK) {
	return Z_ERROR;	
    }
    do {
    	if ((in_pdu = ClientGetPDU(fd, session->c_timeOut)) == NULL) {
	    fprintf(stderr, "Didn't get a search response pdu.\n");
	    /* something serious happened. close the connection.
	     */
	    tk_disconnect(session);
	    session->connectionStatus = Z_CLOSE;
	    return Z_ERROR;
    	}
 	if (in_pdu->which  == e1_resourceControlRequest) {	
	    if (debug) PrintPDU(stdout,in_pdu); 
	    if (in_pdu->u.resourceControlRequest->responseRequired != 0) {
	        out_pdu = MakeResourceControlResponse(NULL, 1, 1);
	        if (debug) PrintPDU(stdout,out_pdu); 
  	        PutPDU (fd, out_pdu);
    	        (void) FreePDU (out_pdu);
	    }
	} 
    } while (in_pdu->which != e1_searchResponse);

    (void) FreePDU (out_pdu);
    (void) FreeArgv(databaseNames);
    (void) FreeArgv (dbnames);
    (void) FreeArgv (esnames);

    if (debug) PrintPDU(stdout,in_pdu);
    if (ProcessSearchResponse (session, in_pdu->u.searchResponse) != Z_OK)
	fprintf(stderr, "Error in processing search response.\n");

    (void) FreePDU (in_pdu);
    return Z_OK;
}

int
tk_disconnect(session)
ZSESSION *session;
{
    int fd;
    if (session == NULL)
	return Z_ERROR;
    fd = session->fd;
#ifndef WIN32
    close (fd);		/* terminate the connection.	*/
#else
    closesocket(fd);
#endif
    session->connectionStatus = Z_CLOSE;
    return Z_OK;
}

ZSESSION *
tk_connect(host, port, authentication)
char	*host;
int	port;
GSTR    *authentication;
{
    	int fd;
    	ZSESSION *session;
        extern ZSESSION *TKZ_User_Session_TMP;

	if (TKZ_User_Session_TMP == NULL) {
	  if ((session = CALLOC(ZSESSION,1)) == NULL)
	    return (ZSESSION *)NULL;
	}
	else
	  session = TKZ_User_Session_TMP;
	InitSession (session);

    	session->s_portNumber = port;
    	session->s_serverMachineName = (char *)strdup (host);
    	if ((fd = ConnectToServer(host, port)) < 0) {
		session->status = Z_ERROR;
		session->connectionStatus = Z_CLOSE;
		return session;
    	}
    	session->fd = fd;
    	CURRENT_FD = fd;
    	if (ExecuteInitCommand(fd, session, authentication) == Z_OK) {
		session->connectionStatus = Z_OPEN;
		session->state	= S_OPEN;
		session->status = Z_OK;
    	} else {
		session->state	= S_OPEN;
		session->connectionStatus = Z_OPEN;
		session->status = Z_ERROR;
    	}
	context = Z_CLIENT;
    	return session;
}

/* reconnect to a former session */
ZSESSION *
tk_reconnect(session, authentication)
ZSESSION *session;
GSTR    *authentication;
{
  int fd;
  int port;
  char *host;
  char *tmpauth;

  port = session->s_portNumber;
  host = session->s_serverMachineName;
  
  if (host == NULL)
    return(NULL);
  
  if ((fd = ConnectToServer(host, port)) < 0) {
    session->status = Z_ERROR;
    session->connectionStatus = Z_CLOSE;
    return session;
  }
  session->fd = fd;
  CURRENT_FD = fd;

  

  if (ExecuteInitCommand(fd, session, authentication) == Z_OK) {
    session->connectionStatus = Z_OPEN;
    session->state	= S_OPEN;
    session->status = Z_OK;
    	} else {
	  session->state	= S_CLOSE;
	  session->connectionStatus = Z_CLOSE;
	  session->status = Z_ERROR;
    	}
  context = Z_CLIENT;
  return session;
}

void
InitString (str, size)
char *str;
int size;
{
    memset (str, '\0', size);
}

int
ClientProtocolVersion (session, protocolVersion)
ZSESSION *session;
char	*protocolVersion;
{
    if (!session)
	return Z_ERROR;

    protocolVersion[0] = '0' + (session->c_supportVersion1==Z_TRUE) ? 1 : 0;
    protocolVersion[1] = '0' + (session->c_supportVersion2==Z_TRUE) ? 1 : 0;
    protocolVersion[2] = '0' + (session->c_supportVersion3==Z_TRUE) ? 1 : 0;
    return Z_OK;
}
    
int
ClientOptions (session, options)
ZSESSION *session;
char	*options;
{
    if (!session)
	return Z_ERROR;

    options[0] = '0' + (session->c_requestSearch==Z_TRUE) ? 1 : 0;
    options[1] = '0' + (session->c_requestPresent==Z_TRUE) ? 1 : 0;
    options[2] = '0' + (session->c_requestDeleteResultSet==Z_TRUE) ? 1 : 0;
    options[3] = '0' + (session->c_requestResourceReport==Z_TRUE) ? 1 : 0;
    options[4] = '0' + (session->c_requestTriggerResourceControl==Z_TRUE) ? 1 : 0;
    options[5] = '0' + (session->c_requestResourceControl==Z_TRUE) ? 1 : 0;
    options[6] = '0' + (session->c_requestAccessControl==Z_TRUE) ? 1 : 0;
    options[7] = '0' + (session->c_requestScan==Z_TRUE) ? 1 : 0;
    options[8] = '0' + (session->c_requestSort==Z_TRUE) ? 1 : 0;
    options[10] = '0' + (session->c_requestExtendedServices==Z_TRUE) ? 1 : 0;
    options[11] = '0' + 
      (session->c_requestLevel_1_Segmentation==Z_TRUE) ? 1 : 0;
    options[12] = '0' + 
      (session->c_requestLevel_2_Segmentation==Z_TRUE) ? 1 : 0;
    options[13] = '0' + 
      (session->c_requestConcurrentOperations==Z_TRUE) ? 1 : 0;
    options[14] = '0' + (session->c_requestNamedResultSets==Z_TRUE) ? 1 : 0;

    return Z_OK;
}
    
int
ExecuteInitCommand(fd, session, authentication)
int fd;
ZSESSION *session;
GSTR *authentication;
{
    int code;
    PDU *out_pdu, *in_pdu;
    InitializeResponse *initResponse;


    /* The client parameter values really should be read into session
     * record from the client init file.
     */
    out_pdu =  MakeInitializeRequest("INIT_REFID",
			session->c_protocolVersion,
			session->c_options,
			session->c_preferredMessageSize,
			session->c_exceptionalRecordSize,
			authentication,
			session->c_implementationId,
			session->c_implementationName,
			session->c_implementationVersion,
			NULL);

    if (out_pdu == NULL) {
	printf("Empty init request pdu!\n");
	return Z_ERROR;
    }

    if (PutPDU (fd, out_pdu) != Z_OK)
	return Z_ERROR;
    if (debug)
    	PrintPDU(stdout,out_pdu);

    do {
      if ((in_pdu = ClientGetPDU(fd, session->c_timeOut)) == NULL) {
	fprintf(stderr, "Didn't get the init response pdu.\n");
	return Z_ERROR;
      }
      /* this is not the right way to handle resource and access control */
      /* this function should just return and arrange for the response   */
      /* to be made externally -- see how it is handled for search in    */
      /* cheshire2/zquery.                                               */
      
      if (in_pdu->which  == e1_resourceControlRequest) {	
	if (debug) PrintPDU(stdout,in_pdu); 
	if (in_pdu->u.resourceControlRequest->responseRequired != 0) {
	  out_pdu = MakeResourceControlResponse(
				    NewOctetString("RESOURCE_REFID"), 1, 1);
	  if (debug) PrintPDU(stdout,out_pdu); 
	  PutPDU (fd, out_pdu);
	  (void) FreePDU (out_pdu);
	}
      } else if (in_pdu->which  == e1_accessControlRequest) {	
	if (debug) PrintPDU(stdout,in_pdu); 
	/* getPassword();   use bogus password: anonymous	*/
	out_pdu = MakeAccessControlResponse (NULL, PASSWORD);
	if (debug) PrintPDU(stdout,out_pdu); 
	PutPDU (fd, out_pdu);
	(void) FreePDU (out_pdu);
      }
    } while (in_pdu->which != e1_initResponse);
    
    (void) FreePDU (out_pdu);
    
    if (debug)
      if (debug) PrintPDU(stdout,in_pdu);
    
    initResponse = in_pdu->u.initResponse;
    if (initResponse->result == 0) {	/* connection rejected.	*/
      code = Z_ERROR;
    } else {				/* connection accepted.	*/
      /* record server info.	*/
      
      session->s_protocolVersion = GetBitString(initResponse->protocolVersion);
      session->s_options = GetBitString(initResponse->options);
      session->s_implementationId = 
	GetOctetString(initResponse->implementationId);
      session->s_implementationName = 
	GetOctetString(initResponse->implementationName);
      session->s_implementationVersion = 
	GetOctetString(initResponse->implementationVersion);
      session->s_preferredMessageSize = initResponse->preferredMessageSize;
      session->s_exceptionalRecordSize = initResponse->exceptionalRecordSize;
      
      if (session->s_protocolVersion) {
	session->s_supportVersion1 = 
	  (session->s_protocolVersion[0]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportVersion2 = 
	  (session->s_protocolVersion[1]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportVersion3 = 
	  (session->s_protocolVersion[2]=='1') ? Z_TRUE : Z_FALSE;
      }
      if (session->s_options) {
	session->s_supportSearch = 
	  (session->s_options[0]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportPresent = 
	  (session->s_options[1]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportDeleteResultSet = 
	  (session->s_options[2]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportResourceReport = 
	  (session->s_options[3]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportTriggerResourceControl = 
	  (session->s_options[4]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportResourceControl = 
	  (session->s_options[5]=='1') ? Z_TRUE : Z_FALSE;
	session->s_supportAccessControl = 
	  (session->s_options[6]=='1') ? Z_TRUE : Z_FALSE;
	
	if (session->s_supportVersion3) {
	  session->s_supportScan = 
	    (session->s_options[7] == '1') ? Z_TRUE : Z_FALSE;
	  session->s_supportSort = 
	    (session->s_options[8] == '1') ? Z_TRUE : Z_FALSE;
	  session->s_supportExtendedServices = 
	    (session->s_options[10] == '1') ? Z_TRUE : Z_FALSE;
	  session->s_supportLevel_1_Segmentation = 
	    (session->s_options[11] == '1') ? Z_TRUE : Z_FALSE;
	  session->s_supportLevel_2_Segmentation = 
	    (session->s_options[12] == '1') ? Z_TRUE : Z_FALSE;
	  session->s_supportConcurrentOperations = 
	    (session->s_options[13] == '1') ? Z_TRUE : Z_FALSE;
	  session->namedResultSet = 
	    session->s_supportNamedResultSets = 
	    (session->s_options[14] == '1') ? Z_TRUE : Z_FALSE;
	}
      }
      code = Z_OK;
    }
    session->preferredMessageSize = session->s_preferredMessageSize;
    session->exceptionalRecordSize = MIN (session->c_exceptionalRecordSize,
					  session->s_exceptionalRecordSize);
    (void ) FreePDU (in_pdu);
    return code;
}

int
ProcessDeleteResultSetRequest(session)
ZSESSION *session;
{
    int fd;
    PDU *out_pdu, *in_pdu;

    if (session == NULL)
	return Z_ERROR;

    fd = session->fd;
    if (session->deleteAllResultSets == Z_TRUE)
    	out_pdu = MakeDeleteResultSetRequest("DELETE_REFID", 1, (char **)NULL, 0);
    else
    	out_pdu = MakeDeleteResultSetRequest("DELETE_REFID", 0, 
			session->resultSetList, session->numberResultSets);
    if (out_pdu == NULL)
	return Z_ERROR;
    if (PutPDU (fd, out_pdu) != Z_OK) {
        (void) FreePDU (out_pdu);
	return Z_ERROR;
    }
    if ((in_pdu = ClientGetPDU(fd, session->c_timeOut)) == NULL)
	return Z_ERROR;
    if (debug)
    	PrintPDU(stdout,in_pdu);
    ProcessDeleteResultSetResponse(session, in_pdu->u.deleteResultSetResponse);
    (void) FreePDU (out_pdu);
    (void) FreePDU (in_pdu);
    return Z_OK;
}

int
tk_ResourceReport(referenceId, resourceReportFormat)
char	*referenceId;
char	*resourceReportFormat;
{
    return ExecuteResourceReportRequest(CURRENT_FD, referenceId, resourceReportFormat);
}

int
ExecuteResourceReportRequest(fd, referenceId, resourceReportFormat)
int	fd;
char	*referenceId;
char	*resourceReportFormat;
{
    PDU *out_pdu, *in_pdu;

    out_pdu = MakeResourceReportRequest(referenceId, resourceReportFormat);
    if (debug) PrintPDU(stdout,out_pdu);
    PutPDU (fd, out_pdu);

    in_pdu = ClientGetPDU(fd, 120); /* this should be session->c_timeOut... */
    if (debug) PrintPDU(stdout,in_pdu);
    (void) FreePDU (out_pdu);
    (void) FreePDU (in_pdu);
    return 1;
}


int
tk_TriggerResourceControlRequest (referenceId, requestedOperation,
	preferredResourceReportFormat, resultSetWanted)
char	*referenceId;
int	requestedOperation;
char	*preferredResourceReportFormat;
int	resultSetWanted;	/* 	1=TRUE; 0=FALSE; -1=OPTIONAL	*/
{
    return ExecuteTriggerResourceControlRequest(CURRENT_FD, referenceId, requestedOperation, preferredResourceReportFormat, resultSetWanted);
}

int
ExecuteTriggerResourceControlRequest(fd, referenceId, requestedOperation,
	preferredResourceReportFormat, resultSetWanted)
int	fd;
char	*referenceId;
int	requestedOperation;
char	*preferredResourceReportFormat;
int	resultSetWanted;	/* 	1=TRUE; 0=FALSE; -1=OPTIONAL	*/
{
    PDU *out_pdu;

    out_pdu = MakeTriggerResourceControlRequest(referenceId, requestedOperation,
				preferredResourceReportFormat, resultSetWanted);
    if (debug) PrintPDU(stdout,out_pdu);
    PutPDU (fd, out_pdu);
    (void) FreePDU (out_pdu);

    return 1;
}

int
tk_present(session, resultSetId, startPoint, numberOfRecordsRequested, 
		elementSetNames, preferredRecordSyntax)
ZSESSION *session;
char *resultSetId;
int startPoint;
int numberOfRecordsRequested;
void *elementSetNames;		/* optional	*/
ObjectIdentifier preferredRecordSyntax;	/* optional	*/
{
    int	fd;
    PDU *out_pdu, *in_pdu;

    if (session == NULL)
	return Z_ERROR;

    session->startPosition = startPoint;
    session->numberOfRecordsRequested = numberOfRecordsRequested;
    fd = session->fd;
    out_pdu = MakePresentRequest(
			"PRESENT_REFID", 
			resultSetId, 
			startPoint, 
			numberOfRecordsRequested, 
			"F",
			OID_USMARC, NULL);
    if (out_pdu == NULL)
	return Z_ERROR;
    if (PutPDU (fd, out_pdu) != Z_OK)
	return Z_ERROR;

    if ((in_pdu = ClientGetPDU(fd, session->c_timeOut)) == NULL) {
	fprintf(stderr, "Failed to get a present response pdu.\n");
	tk_disconnect(session);
	return Z_ERROR;
    }
    (void) FreePDU (out_pdu);

    if (ProcessPresentResponse (session, in_pdu->u.presentResponse) != Z_OK) {
	(void) FreePDU (in_pdu);
	return Z_ERROR;
    }
    if (debug) PrintPDU(stdout,in_pdu);
    (void) FreePDU (in_pdu);
    return Z_OK;
}

