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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "z3950_3.h"
#include "list.h"
#include "z_parameters.h"
#include "session.h"

#include "z3950_intern.h"
#include "cheshire.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define	MIN(a, b)	(((a) < (b)) ? (a) : (b))
char	diagMessage[2*MAX_EM_LENGTH];

int RemoveListOfResultSets(ZSESSION *session, AC_LIST *resultSets, 
			   ListStatuses **liststatus,
			   int *not_deleted);
AC_LIST * ExtractResultSetList(struct resultSetList_List11	*resultSetList);
PDU *HandleSearchRequest(ZSESSION *, SearchRequest *);
PDU *HandleInitializeRequest(ZSESSION *, InitializeRequest *);

extern char *GetBitString(BitString);
extern char *GetOctetString(OctetString);
extern config_file_info *find_file_config(char *filename);
extern OctetString DupOstring (OctetString);
extern char *GetGSTR(GSTR *gstr);
    
ListEntries *CheshireScan(ZSESSION *session, char *attributeSetName, 
			AttributesPlusTerm *termListAndStartPoint,
			int stepSize,
			int numberOfTermsRequested,
			int preferredPositionInResponse,
			int *status, int *stepUsed);




PDU *
HandleInitializeRequest(session, initRequest)
     ZSESSION	*session;
     InitializeRequest	*initRequest;
{
  PDU *pdu;
  ZBOOL status = Z_TRUE;
  char *authentication_string = NULL;

  if ((!session) || (!initRequest))
    return (PDU *)NULL;
  
  /*
   * Decide the preferred message size and the maximum
   * record size. 
   */

  if (initRequest->preferredMessageSize == 0 
      && initRequest->exceptionalRecordSize == 0) {
    session->preferredMessageSize = session->s_preferredMessageSize;
    session->exceptionalRecordSize = session->s_exceptionalRecordSize;
  }
  else {
    
    session->preferredMessageSize = MIN(initRequest->preferredMessageSize, 
					session->s_preferredMessageSize);
    session->exceptionalRecordSize = MIN(initRequest->exceptionalRecordSize, 
					 session->s_exceptionalRecordSize);
  }
  /*
   * The preferredMessages and exceptionalRecordSize must not be
   * less than zero, and the exceptionalRecordSize must be greater
   * than or equal to preferredMessageSize.
   */
  if ((session->preferredMessageSize <= 0) ||
      (session->exceptionalRecordSize <= 0) ||
      (session->exceptionalRecordSize < session->preferredMessageSize))
    status = Z_FALSE;

  
  session->c_preferredMessageSize = initRequest->preferredMessageSize;
  session->c_exceptionalRecordSize = initRequest->exceptionalRecordSize;
  session->c_referenceId = DupOstring(initRequest->referenceId);
  session->c_protocolVersion = GetGSTR(initRequest->protocolVersion);
  session->c_options = GetGSTR(initRequest->options);
  SetClientOptions(session);
  session->c_implementationId = GetGSTR(initRequest->implementationId);
  session->c_implementationName = GetGSTR(initRequest->implementationName);
  session->c_implementationVersion = GetGSTR(initRequest->implementationVersion);
  
  if (initRequest->idAuthentication != NULL) {
    char *c;
    authentication_string = strdup(initRequest->idAuthentication);
    c = strchr(authentication_string, ':');
    if (c != NULL) {
      *c = '\0';
      c++;
      session->c_login_id = authentication_string;
      session->c_login_pwd = c;
    }
  }
  else {
    session->c_login_id = NULL;
    session->c_login_pwd = NULL;
  }

  /* What about userInformationField?	*/

  pdu = MakeInitializeResponse(
			       session->c_referenceId,
			       session->s_protocolVersion,
			       session->s_options,
			       session->preferredMessageSize,
			       session->exceptionalRecordSize,
			       status,
			       session->s_implementationId,
			       session->s_implementationName,
			       session->s_implementationVersion,
			       NULL);
  return pdu;
}

/*
 * This function returns 1 to indicate that the result set name 
 * exists, otherwise it returns 0.
 */
int
ResultSetExist(session, resultSetName)
     ZSESSION *session;
     char	*resultSetName;
{
  if (ac_list_search(session->c_resultSetNames, resultSetName) == 1)
    return 1;
  return 0;
}

PDU *HandleSearchRequest(session, searchRequest)
     ZSESSION	*session;
     SearchRequest	*searchRequest;
{
  Records	*records;
  PDU	*pdu;
  char databaseName[500];
  char resultSetName[500];
  char elementSetName[500];	/* 'F' means full record.	*/
  char TMPelementSetName[500];	/* 'F' means full record.	*/
  ElementSetNames	*elementNames;
  struct databaseNames_List1 *dbNames;
  int numberDatabases;	/* Number of Databases Specified 
			 * in the search request.
			 */
  int status;
  char prefRecordSyntax[80];	/* preferred record syntax.	*/
  RECORD_LISTS *rlist = NULL;
  int numberRecordsReturned;
  int presentStatus;
  int nextResultSetPosition;
  int searchStatus;
  int resultSetStatus;
  int elementSetRequested = 0;
  int filetype;

  if ((!session) || (!searchRequest))
    return NULL;
  if (session->c_referenceId) {
    FreeOctetString(session->c_referenceId);
  }
  session->c_referenceId = DupOstring(searchRequest->referenceId);
  session->c_smallSetUpperBound = searchRequest->smallSetUpperBound;
  session->c_largeSetLowerBound = searchRequest->largeSetLowerBound;
  session->c_mediumSetPresentNumber = searchRequest->mediumSetPresentNumber;
  session->replaceIndicator = searchRequest->replaceIndicator;
  
  session->resultCount = 0;
  if (searchRequest->query == NULL) {
    SetError2 (session, 108, "empty query");
    goto error;
  }
  /*
   * Check the replace indicator. If the replace indicator is set to off,
   * and the result set name already exists, then return a diagnostic
   * message to the client.
   */
  memset(resultSetName, '\0', sizeof(resultSetName));
  CopyOctetString(resultSetName, searchRequest->resultSetName);
  if ((searchRequest->replaceIndicator == Z_FALSE) && 
      (ResultSetExist(session, resultSetName) == 1)) {
    SetError2 (session, 21, resultSetName);
    goto error;
  }
  
  if ((session->namedResultSet == 0) && 
      (strcasecmp(resultSetName, "default") != 0)) {
    SetError2 (session, 22, resultSetName);
    goto error;
  }
  
  /* Validate the database names to be searched.
   */
  numberDatabases = 0;



  for (dbNames=searchRequest->databaseNames; dbNames != NULL;
       dbNames = dbNames->next) {
    CopyOctetString(databaseName, dbNames->item);
    if (strcasecmp(databaseName,"METADATA") != 0 &&
	strcasecmp(databaseName,"DEFAULT") != 0 &&
	strcasecmp(databaseName,"IR-EXPLAIN-1") != 0) {
      if (find_file_config(databaseName) == NULL) {
	SetError2 (session, 109, databaseName);
	goto error;
      }
    }
    session->c_databaseNames[numberDatabases] =(char *)strdup(databaseName);
    numberDatabases += 1;
  }
  session->c_databaseNames[numberDatabases] = (char *)NULL;

  filetype = cf_getfiletype(databaseName);
  
  if (numberDatabases > session->maximumNumberDatabases) {
    sprintf(diagMessage, "Maximum %d", session->maximumNumberDatabases);
    SetError2 (session, 111, diagMessage);
    goto error;
  }
  
  if (session->s_supportElementSetNames == 0) {
    if ((searchRequest->smallSetElementSetNames != NULL) ||
	(searchRequest->mediumSetElementSetNames != NULL)) {
      SetError2 (session, 24, " ");
      goto error;
    }
  } else {
    /*
     * Check the small set element set names.
     */
    if ((elementNames = searchRequest->smallSetElementSetNames) != NULL) {
      if (elementNames->which == e15_genericElementSetName) {
	session->s_smallElementSetNames =
	  GetOctetString(elementNames->u.genericElementSetName);
	for (dbNames=searchRequest->databaseNames; dbNames != NULL;
	     dbNames = dbNames->next) {
	  CopyOctetString(databaseName, dbNames->item);
	  if (SearchElementSetNames (databaseName, 
				     session->s_smallElementSetNames, 
				     0) != 1 
	      && filetype != FILE_TYPE_VIRTUALDB) {
	    SetError2 (session, 25, session->s_smallElementSetNames);
	    goto error;
	  }
	}
      } else {	/* element set names are database specific. */
	struct databaseSpecific_List8 *tmp;
	for (tmp = elementNames->u.databaseSpecific; tmp!=NULL;
	     tmp = tmp->next) {
	  /* This will need to change -- doesn't really work */
	  CopyOctetString(databaseName, tmp->item.dbName);
	  CopyOctetString(elementSetName, tmp->item.esn);
	  if (SearchElementSetNames (databaseName, elementSetName, 0) != 1 
	      && filetype != FILE_TYPE_VIRTUALDB) {
	    SetError2 (session, 25, elementSetName);
	    goto error;
	  }
	}
      }
    }
    
    /*
     * Check the medium set element set names.
     */
    if ((elementNames = searchRequest->mediumSetElementSetNames) != NULL) {
      if (elementNames->which == e15_genericElementSetName) {
	session->s_mediumElementSetNames =
	  GetOctetString(elementNames->u.genericElementSetName);
	for (dbNames=searchRequest->databaseNames; dbNames != NULL;
	     dbNames = dbNames->next) {
	  CopyOctetString(databaseName, dbNames->item);
	  if (SearchElementSetNames (databaseName, 
				     session->s_mediumElementSetNames, 
				     1) != 1 
	      && filetype != FILE_TYPE_VIRTUALDB) {
	    SetError2 (session, 25, session->s_mediumElementSetNames);
	    goto error;
	  }
	}
      } else {	/* element set names are database specific. */
	struct databaseSpecific_List8 *tmp;
	for (tmp = elementNames->u.databaseSpecific; tmp!=NULL;
	     tmp = tmp->next) {
	  /* needs work ... */
	  CopyOctetString(databaseName, tmp->item.dbName);
	  CopyOctetString(elementSetName, tmp->item.esn);
	  if (SearchElementSetNames (databaseName, elementSetName, 1) != 1 
	      && filetype != FILE_TYPE_VIRTUALDB) {
	    SetError2 (session, 25, elementSetName);
	    goto error;
	  }
	}
      }
    }
  }
  
  if (searchRequest->additionalSearchInfo != NULL) {
    char *tmpadd;
    tmpadd = NULL;
    if (searchRequest->additionalSearchInfo->item.information.which 
	== e24_oid) {
      tmpadd = GetOidStr(searchRequest->additionalSearchInfo->item.information.u.oid);
    } else {
      tmpadd = GetInternationalString(searchRequest->additionalSearchInfo->item.information.u.characterInfo);
    }
    if (tmpadd) {
      strncpy(session->SearchAddInfo,tmpadd,MAX_EM_LENGTH-1);
      FREE(tmpadd);
    }
  }

  if (searchRequest->preferredRecordSyntax != NULL) {
    CopyOidStr (prefRecordSyntax, searchRequest->preferredRecordSyntax);
    /*
     * Check for the record formats we can handle
     */      
    if (strcasecmp(MARCRECSYNTAX,prefRecordSyntax) == 0)    
      session->preferredRecordSyntax = MARCRECSYNTAX;
    else if (strcasecmp(SUTRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SUTRECSYNTAX;
    else if (strcasecmp(SGML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SGML_RECSYNTAX;
    else if (strcasecmp(XML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = XML_RECSYNTAX;
    else if (strcasecmp(HTML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = HTML_RECSYNTAX;
    else if (strcasecmp(OPACRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = OPACRECSYNTAX;
    else if (strcasecmp(EXPLAINRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp(SUMMARYRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp(GRS0RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = GRS0RECSYNTAX;
    else if (strcasecmp(GRS1RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = GRS1RECSYNTAX;
    else if (strcasecmp(ESRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = ESRECSYNTAX;
    else {
      SetError2 (session, 106, prefRecordSyntax);
      goto error;
    }
  }
    
    status = ExecuteQuery(session, searchRequest->query, 
			  session->c_databaseNames, resultSetName);

    if (status == Z_OK) {	/* Search completed normally.	*/
      /* Add the current result set name to the list of result set names
       * belonging to the current client. 
       */
      if ((session->resultCount > 0) && 
	  (ac_list_search(session->c_resultSetNames, resultSetName) != 1))
	ac_list_add(session->c_resultSetNames, resultSetName);
      /*
       * Look up preferredMessageSize, exceptionalRecordSize,
       * smallSetUpperBound, largeSetLowerBound, mediumSetPresentNumber
       * to decide how many records should be returned in the
       * search response pdu.
       */
      searchStatus = Z_TRUE;

      if (session->resultCount > session->c_smallSetUpperBound) {
	if (session->resultCount < session->c_largeSetLowerBound) {
	  /* this is within the medium set size */
	  numberRecordsReturned = session->c_mediumSetPresentNumber;
	  presentStatus = 2;
	  nextResultSetPosition = session->c_mediumSetPresentNumber + 1;
	  if (session->s_mediumElementSetNames != NULL) {
	    elementSetRequested = 1;
	    strcpy(TMPelementSetName, session->presentElementSet);
	    strcpy(session->presentElementSet,session->s_mediumElementSetNames);
	  }
	} 
	else { /* Greater than LargeSet upper bound return 0 */
	  numberRecordsReturned = 0;
	  presentStatus = 0;
	  nextResultSetPosition = 0;
	}
      }
      else { /* smaller than smallSetUpperBound -- return with search */
	  numberRecordsReturned = session->resultCount;
	  presentStatus = 2;
	  nextResultSetPosition = 0;
	  if (session->s_smallElementSetNames != NULL) {
	    elementSetRequested = 1;
	    strcpy(TMPelementSetName, session->presentElementSet);
	    strcpy(session->presentElementSet,session->s_smallElementSetNames);
	  }
      }
      if (numberRecordsReturned > 0) {
	
	rlist = RetrieveRecords2(session, resultSetName, 1, 
				 numberRecordsReturned, elementSetRequested);
	if (rlist == NULL) { /* present error */
	  goto error;
	}
	records = MakeDatabaseRecords2(rlist,session->preferredRecordSyntax);
      } else {
	records = NULL;
      }
      /* restore the elementset name for presents */
      if (TMPelementSetName[0] != '\0')
	strcpy(session->presentElementSet,TMPelementSetName);

      resultSetStatus = 0;
      pdu = MakeSearchResponse(
			       session->c_referenceId,
			       session->resultCount,
			       numberRecordsReturned,
			       nextResultSetPosition,
			       searchStatus,
			       resultSetStatus,
			       presentStatus,
			       records);
      if ((session->resultCount > 0) && (rlist != NULL))
	(void) FreeRecordLists (rlist);
      return pdu;
    }
    
error:
    /*
     *	case 2:	An error has occurred during the retrieval process.
     */
    searchStatus = Z_FALSE;
    resultSetStatus = 3;
    session->resultCount = 0;
    numberRecordsReturned = 1;
    nextResultSetPosition = 0,
      presentStatus = 0;
    sprintf(diagMessage,"%s -- %s",session->errorMessage,session->errorAddInfo);
    records = MakeNonSurrogateDiagRecords(OID_DS_BIB1,
					  session->errorCode,diagMessage);
    pdu = MakeSearchResponse(
			     session->c_referenceId,
			     0,
			     numberRecordsReturned,
			     nextResultSetPosition,
			     searchStatus,
			     resultSetStatus,
			     presentStatus,
			     records);
    return pdu;
}

void
FreeRecordLists (recs)
     RECORD_LISTS *recs;
{
  RECORD_LISTS  *tmp;
  
  while (recs != NULL) {
    tmp = recs;
    recs=recs->next;
    if (tmp->records->which == rec_type_list) {
      (void) ac_list_free (tmp->records->u.list);
      (void) free ((char *)tmp->records);
    }
    else {
      /* should free each record here */

      (void) free ((char *)tmp->records);      
    }
    (void) free ((char *)tmp);
  }
}

PDU *
HandlePresentRequest(session, presentRequest)
     ZSESSION	*session;
     PresentRequest	*presentRequest;
{
  PDU *pdu = NULL;
  int startPoint;
  int numberRecordsRequested;
  char resultSetId[100];
  char prefRecordSyntax[100];
  char databaseName[100];
  Records	*records = NULL;
  int resultCount;	/* number of records in a result set.	*/
  char elementSetName[500];	/* 'F' means full record.	*/
  char TMPelementSetName[500];	/* 'F' means full record.	*/
  ElementSetNames	*elementNames;
  RECORD_LISTS *rlist = NULL;
  int filetype;
  int numberRecordsReturned;
  int nextResultSetPosition;
  int presentStatus;
  int elementSetRequested;
  
  if ((!session) || (!presentRequest))
    return (PDU *)NULL;
  
  startPoint = presentRequest->resultSetStartPoint;
  numberRecordsRequested = presentRequest->numberOfRecordsRequested;
  if (session->c_referenceId) {
    FreeOctetString(session->c_referenceId);
  }
  session->c_referenceId = DupOstring(presentRequest->referenceId);
  nextResultSetPosition = 0;
  numberRecordsReturned = 0;
  presentStatus = 5;		/* failure.	*/

  filetype = cf_getfiletype(session->databaseName);
  

  /*
   * Validate the result set name.
   */
  if (presentRequest->resultSetId == NULL) {
    SetError2 (session, 30, "Result set no specified.");
    goto error;
  }
  CopyOctetString(resultSetId, presentRequest->resultSetId);

  /*
   * validate the element set names.
   * To be completed.
   */
  elementSetRequested = 0;
  if (presentRequest->recordComposition != NULL) {
    if (presentRequest->recordComposition->which == e9_complex) {
      /* there is no specific error for this... */
      SetError2 (session, 14, 
		 "Record Composition Specifications not supported");
      goto error;
      
    }
    else { 
      /* it is a simple elementsetname */
      if ((elementNames = 
	   presentRequest->recordComposition->u.simple) != NULL) {
	if (elementNames->which == e15_genericElementSetName) {
	  CopyOctetString(elementSetName, 
			  elementNames->u.genericElementSetName);
	  if (SearchElementSetNames (session->databaseName, 
				     elementSetName, 0) != 1 
	      && filetype != FILE_TYPE_VIRTUALDB) {
	    SetError2 (session, 25, elementSetName);
	    goto error;
	  }
	  /* make this the present element name */
	  strcpy (session->presentElementSet, elementSetName);
	  elementSetRequested = 1;
	} else {	/* element set names are database specific. */
	  struct databaseSpecific_List8 *tmp;
	  for (tmp = elementNames->u.databaseSpecific; tmp!=NULL;
	       tmp = tmp->next) {
	    CopyOctetString(databaseName, tmp->item.dbName);
	    CopyOctetString(elementSetName, tmp->item.esn);
	    if (SearchElementSetNames (databaseName, elementSetName, 0) != 1 
		&& filetype != FILE_TYPE_VIRTUALDB) {
	      SetError2 (session, 25, elementSetName);
	      goto error;
	    }
	  }
	  /* we only take the last one... needs fixing later*/
	  strcpy (session->presentElementSet, elementSetName);
	  elementSetRequested = 1;
	}
      }
    }
  }
  
  /*
   * validate the preferred record syntax.
   */
  if (presentRequest->preferredRecordSyntax != NULL) {
    CopyOidStr (prefRecordSyntax, presentRequest->preferredRecordSyntax);

    /*
     * Check for the record formats we can handle
     */      
    if (strcasecmp(session->databaseName, "IR-Explain-1") == 0)
       strcpy(prefRecordSyntax, EXPLAINRECSYNTAX);

    if (strcasecmp(MARCRECSYNTAX,prefRecordSyntax) == 0)    
      session->preferredRecordSyntax = MARCRECSYNTAX;
    else if (strcasecmp(SUTRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SUTRECSYNTAX;
    else if (strcasecmp(SGML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SGML_RECSYNTAX;
    else if (strcasecmp(XML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = XML_RECSYNTAX;
    else if (strcasecmp(HTML_RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = HTML_RECSYNTAX;
    else if (strcasecmp(OPACRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = OPACRECSYNTAX;
    else if (strcasecmp(EXPLAINRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = EXPLAINRECSYNTAX;
    else if (strcasecmp(SUMMARYRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = SUMMARYRECSYNTAX;
    else if (strcasecmp(GRS0RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = GRS0RECSYNTAX;
    else if (strcasecmp(GRS1RECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = GRS1RECSYNTAX;
    else if (strcasecmp(ESRECSYNTAX,prefRecordSyntax) == 0)
      session->preferredRecordSyntax = ESRECSYNTAX;
    else {
      SetError2 (session, 106, prefRecordSyntax);
      goto error;
    }
  }
  else { /* no record syntax information */
    if (strcasecmp(session->databaseName, "IR-Explain-1") == 0)
      session->preferredRecordSyntax = EXPLAINRECSYNTAX;
    else
      /* set the default syntax to SGML */
      session->preferredRecordSyntax = SGML_RECSYNTAX;
  }
  
  /* various special requests may be passed via otherinfo */
  if (presentRequest->otherInfo != NULL) {
    char *tmpadd;
    tmpadd = NULL;
    if (presentRequest->otherInfo->item.information.which 
	== e24_oid) {
      tmpadd = GetOidStr(presentRequest->otherInfo->item.information.u.oid);
    } else {
      tmpadd = GetInternationalString(presentRequest->otherInfo->item.information.u.characterInfo);
    }
    if (tmpadd) {
      strncpy(session->SearchAddInfo,tmpadd,MAX_EM_LENGTH-1);
      FREE(tmpadd);
    }
  }


  /*
   * Check for the range of present request.
   * Get the number of records for the requested result set.
   */
  if (strcmp(session->resultSetName, resultSetId) != 0) {
    /*different resultset -- need to change some things */
    resultCount = se_get_result_hits(resultSetId);
    if (resultCount < 0) {
      sprintf(diagMessage, "%s", resultSetId);
      SetError2 (session, 30, diagMessage);
      goto error;
    }
      
  }
  else
    resultCount = session->resultCount;

  if ((startPoint < 1) || 
      (startPoint > resultCount) || 
      (numberRecordsRequested < 1)) {
    sprintf(diagMessage, "%s has %d records", resultSetId, resultCount);
    SetError2 (session, 13, diagMessage);
    goto error;
  }

  if ((startPoint + numberRecordsRequested) > resultCount + 1) {
    numberRecordsRequested = resultCount + 1 - startPoint;
    sprintf(diagMessage, "%s has only %d records. The start point is %d, then the maximum number of records you may request is %d", 
	    resultSetId, resultCount,
	    startPoint, numberRecordsRequested);
    SetError2 (session, 13, diagMessage);
    goto error;
  }

  /*
   * The number of records that are returned in the 
   * present response should be decided based on the values of the 
   * preferredMessageSize, the exceptionalRecordSize, et al.
   * TO BE IMPLEMENTED.
   */
  numberRecordsReturned = MIN(numberRecordsRequested,resultCount);
  presentStatus = 0;	/* all of requested records returned.	*/
  nextResultSetPosition = startPoint+numberRecordsReturned;
  
  if (nextResultSetPosition > resultCount)
    nextResultSetPosition = 0;
  
  rlist = RetrieveRecords2(session, resultSetId, startPoint, 
			   numberRecordsReturned, elementSetRequested);
  if (!rlist) 
    goto error;

  records = MakeDatabaseRecords2(rlist, session->preferredRecordSyntax);

  if (!records) {
    SetError2 (session, 14, "system error");
    goto error;
  }
  pdu = MakePresentResponse(
			    session->c_referenceId, 
			    numberRecordsReturned, 
			    nextResultSetPosition,
			    presentStatus, 
			    records);
  if (rlist != NULL)
    FreeRecordLists (rlist);
  return pdu;
  
error:
  presentStatus = 5;
  numberRecordsReturned = 0;
  nextResultSetPosition = 0;
  sprintf(diagMessage,"%s -- %s",session->errorMessage,session->errorAddInfo);
  records = MakeNonSurrogateDiagRecords(OID_DS_BIB1,
					session->errorCode,diagMessage);
  pdu = MakePresentResponse(
			    session->c_referenceId, 
			    numberRecordsReturned, 
			    nextResultSetPosition,
			    presentStatus, 
			    records);
  return pdu;
}

Records *
MakeDatabaseRecords2(recordLists, oid)
     RECORD_LISTS *recordLists;
     char *oid;
{
  AC_LIST *mlist;			
  char	*databaseName;
  char *marc;
  RECS *recs;
  Records *tmp;
  struct responseRecords_List6 *r1 = CALLOC(struct responseRecords_List6, 1);
  struct responseRecords_List6 *head = NULL, *tail = NULL;
  External *databaseRecord;
  RECORD_LISTS    *recList;
  
  tmp = CALLOC(Records, 1);
  tmp->which = e10_responseRecords;
  for (recList = recordLists; recList != NULL; recList = recList->next) {
    if (recList->records->which == rec_type_list) {
      /* List structured records (MARC or SUTRS/SGML) */
      mlist = recList->records->u.list;
      databaseName = recList->databaseName;
      for (marc=ac_list_first(mlist); marc!=NULL; marc=ac_list_next(mlist)) {
	r1 = CALLOC(struct responseRecords_List6,1);
	r1->next = NULL;
	databaseRecord = CALLOC(External,1);
	databaseRecord->directReference = NewOID(oid);
	if (strcmp(oid, MARCRECSYNTAX) == 0
	    || strcmp(oid,XML_RECSYNTAX) == 0
	    || strcmp(oid,HTML_RECSYNTAX) == 0
	    || strcmp(oid,SGML_RECSYNTAX) == 0) {
	  databaseRecord->which = t_octetAligned;
	  databaseRecord->u.octetAligned = NewOctetString(marc);
	}
	else if (strcmp(oid, SUTRECSYNTAX) == 0) {
	  databaseRecord->which = t_singleASN1type;
	  databaseRecord->u.singleASN1type = NewInternationalString(marc);
	}
	r1->item = MakeNamePlusRecord(databaseName, e11_retrievalRecord,
				      databaseRecord);
	if (head == NULL) {
	  head = r1;
	  tail = r1;
	} else {
	  tail->next = r1;
	  tail = r1;
	}
      }
    }
    else { /* ASN1 structured type records (GRS1, Explain, etc)*/

      for (recs=recList->records; recs!=NULL; recs=recs->next) {
	databaseName = recList->databaseName;
	r1 = CALLOC(struct responseRecords_List6,1);
	r1->next = NULL;
	databaseRecord = CALLOC(External,1);
	databaseRecord->directReference = NewOID(oid);
	databaseRecord->which = t_singleASN1type;
	databaseRecord->u.singleASN1type = recs->u.ASN1;
	
	r1->item = MakeNamePlusRecord(databaseName, e11_retrievalRecord,
				      databaseRecord);
	if (head == NULL) {
	  head = r1;
	  tail = r1;
	} else {
	  tail->next = r1;
	  tail = r1;
	}
      }
    }
  }
  tmp->u.responseRecords = head;
  return tmp;
}


PDU *
HandleDeleteResultSetRequest(session, deleteResultSetRequest)
     ZSESSION	*session;
     DeleteResultSetRequest	*deleteResultSetRequest;
{
  PDU *pdu;
  AC_LIST *resultSetList;
  int	deleteOperation;
  struct ListStatuses *liststat;
  int  overallresult;
  int  number_not_deleted = 0;

  liststat = NULL;

  deleteOperation = deleteResultSetRequest->deleteFunction;
  if ((deleteOperation < 0) || (deleteOperation > 1)) {
    pdu = MakeDeleteResultSetResponse(NULL, 1, NULL, 0, NULL,
				      "Invalid delete operation");
    return pdu;
  }
  
  if (deleteOperation == 1) {		/* Delete all result sets	*/
    resultSetList = ac_list_dup(session->c_resultSetNames);
    overallresult = RemoveListOfResultSets(session, resultSetList, 
					   &liststat, &number_not_deleted);
    pdu = MakeDeleteResultSetResponse(NULL, overallresult, NULL, 
				      number_not_deleted, liststat, "success");
    
  }
  
  if (deleteOperation == 0) {		/* Delete a list of result sets	*/
    resultSetList = 
      ExtractResultSetList(deleteResultSetRequest->resultSetList);	
    overallresult = RemoveListOfResultSets(session, resultSetList,
					   &liststat, &number_not_deleted);
    if (overallresult == 8)
      overallresult = 9;

    pdu = MakeDeleteResultSetResponse(NULL, overallresult, liststat,
				      number_not_deleted, NULL, "success");

  }
  
  /*
   * send deleting result set response to the client.
   */

  return pdu;
}

AC_LIST *
ExtractResultSetList(resultSetList)
     struct resultSetList_List11 *resultSetList;
{
     AC_LIST *list;
     struct resultSetList_List11 *rsList;
     list = ac_list_alloc();
     for (rsList = resultSetList; rsList != NULL; rsList = rsList->next)
       ac_list_append(list, rsList->item->data, rsList->item->length);
     return list;

}

int
RemoveListOfResultSets(session, resultSets, liststatus, not_deleted)
     ZSESSION	*session;
     AC_LIST	*resultSets;	/* A list of result sets to be deleted.	*/
     struct ListStatuses **liststatus;
     int *not_deleted;
{
  char *resultSetId;
  int delete_result_code = 0;
  int failure_result_code = 0;
  int checked=0,  deleted=0;
  struct ListStatuses *first, *current, *last;
  InternationalString NewInternationalString();
  *not_deleted = 0;
  first = current = last = NULL;

  if (resultSets == NULL)
    return(1);

  for (resultSetId = ac_list_first(resultSets); resultSetId != NULL;
       resultSetId = ac_list_next(resultSets)) {
    /*
     * Should return a status value for each result set to be
     * deleted.
     */
    current = CALLOC(struct ListStatuses, 1);
    if (first == NULL)
      first = last = current;
    else {
      last->next = current;
      last = current;
    }

    current->item.id = (ResultSetId)NewInternationalString(resultSetId);
    
    checked++;
    if (ac_list_search(session->c_resultSetNames, resultSetId) == 1) {
      se_delete_result_set(resultSetId);
      delete_result_code = diagnostic_get();
      switch (delete_result_code) {
      case 0: current->item.status = failure_result_code = 0;
	deleted++;
	break;
      case 30: current->item.status = failure_result_code = 1;
	break;
      case 27: current->item.status = failure_result_code = 2;
	break;
      case 2: current->item.status = failure_result_code = 3;
	break;
      case 101: current->item.status = failure_result_code = 4;
	break;
      case 105: current->item.status = failure_result_code = 5;
	break;
      case 31: current->item.status = failure_result_code = 6;
	break;
      case 28: current->item.status = failure_result_code = 10;
	break;
      }
      /* reset diagnostic */
      diagnostic_clear();

      /*
       * remove the result set name from the list of result set
       * names in session->c_resultSetNames.
       */
      ac_list_delete(session->c_resultSetNames, resultSetId);
    }
  }
  if (checked > deleted) {
    failure_result_code = 8;
    *not_deleted = checked - deleted;
  }
  *liststatus = first;

  return (failure_result_code);
}

PDU *
HandleResourceReportRequest(session, resourceReportRequest)
     ZSESSION	*session;
     ResourceReportRequest	*resourceReportRequest;
{
  char	referenceID[50];
  ResourceReport *resourceReport;
  
  PDU	*pdu=NULL;
  /* ----------
     CopyOctetString(referenceID, resourceReportRequest->referenceId);
     resourceReport = MakeResourceReport("Here is a sample resource report.");
     ExpandResourceReport (resourceReport, 1, 100, 0);
     ExpandResourceReport (resourceReport, 7, 10, 0);
     pdu = MakeResourceReportResponse(referenceID,
     0,			
     resourceReport);	
     ------------ */
  return pdu;
}

#define SORT_SUCCESS 0
#define SORT_PARTIAL 1
#define SORT_FAILURE 2
#define RESULT_EMPTY 1
#define RESULT_INTERIM 2
#define RESULT_UNCHANGED 3
#define RESULT_NONE 4

PDU *
HandleSortRequest(ZSESSION *session, SortRequest *sortRequest)
{
  PDU *pdu;
  int diagnostic_code;
  int sort_status;
  int result_set_status;
  struct diagnostics_List19 *dl = NULL;
  char diagmessage[500];
  int result_set_size;

  if (session->c_referenceId) {
    FreeOctetString(session->c_referenceId);
  }
  session->c_referenceId = DupOstring(sortRequest->referenceId);

  /* reset diagnostic */
  diagnostic_clear();

  sort_status = SortResults(&result_set_status, session, sortRequest, 
			    &result_set_size);

  diagnostic_code = diagnostic_get();

  if (diagnostic_code) {

    sprintf(diagmessage,"%s -- %s", 
	    diagnostic_string(diagnostic_code),
	    diagnostic_addinfo(diagnostic_code));

    dl = CALLOC(struct diagnostics_List19, 1);
    dl->item
      = MakeDiagRec(OID_DS_BIB1, diagnostic_code, diagmessage);
  
    diagnostic_clear();
  }

  pdu = MakeSortResponse(sortRequest->referenceId, 
			 sort_status,
			 result_set_status, dl, result_set_size);
  
  return (pdu);

}

PDU *
HandleScanRequest(ZSESSION *session, ScanRequest *scanRequest)
{
  PDU	*pdu;
  char databaseName[100];
  char attributeSetName[100];
  char diagMessage[200];
  struct databaseNames_List12 *dbNames;
  ListEntries *listEntries;
  int numberDatabases;	/* Number of Databases Specified 
			 * in the scan request.
			 */
  int status = 0;
  int stepUsed = 0;
  
  if ((!session) || (!scanRequest))
    return NULL;

  if (session->c_referenceId) {
    FreeOctetString(session->c_referenceId);
  }
  session->c_referenceId = DupOstring(scanRequest->referenceId);

  /* Validate the database names to be searched.
   */
  numberDatabases = 0;

  for (dbNames=scanRequest->databaseNames; dbNames != NULL;
       dbNames = dbNames->next) {
    CopyOctetString(databaseName, dbNames->item);
    if (strcasecmp(databaseName,"METADATA") != 0 &&
	strcasecmp(databaseName,"DEFAULT") != 0 &&
	strcasecmp(databaseName,"IR-EXPLAIN-1") != 0) {
      if (find_file_config(databaseName) == NULL) {
	SetError2 (session, 109, databaseName);
	goto error;
      }
    }
    session->c_databaseNames[numberDatabases] =(char *)strdup(databaseName);
    numberDatabases += 1;
  }
  session->c_databaseNames[numberDatabases] = (char *)NULL;
  
  if (numberDatabases > session->maximumNumberDatabases) {
    sprintf(diagMessage, "Maximum %d", session->maximumNumberDatabases);
    SetError2 (session, 111, diagMessage);
    goto error;
  }


  if (scanRequest->attributeSet != NULL) {
    CopyOidStr (attributeSetName, scanRequest->attributeSet);
  }
    
  listEntries = CheshireScan(session, attributeSetName, 
			scanRequest->termListAndStartPoint,
			scanRequest->stepSize,
			scanRequest->numberOfTermsRequested,
			scanRequest->preferredPositionInResponse,
			&status, &stepUsed);
  
  if (listEntries == NULL && status > 0)
    goto error;

  pdu = MakeScanResponse(
			 session->c_referenceId,
			 stepUsed, status, 
			 scanRequest->preferredPositionInResponse,
			 attributeSetName,
			 listEntries);

  return pdu;
    
error:
    /*
     *	case 2:	An error has occurred during the retrieval process.
     */
  sprintf(diagMessage,"%s -- %s",session->errorMessage, session->errorAddInfo);

  listEntries = CALLOC(ListEntries, 1);
  listEntries->nonsurrogateDiagnostics = 
    CALLOC(struct nonsurrogateDiagnostics_List14, 1);
    
  listEntries->nonsurrogateDiagnostics->item     
    = MakeDiagRec(OID_DS_BIB1, session->errorCode, diagMessage);

  pdu = MakeScanResponse(
			 session->c_referenceId,
			 0, 6,/* failure scanStatus code */
			 0, attributeSetName,
			 listEntries);
  return pdu;
  
}

PDU *
HandleCloseRequest(ZSESSION *session, Close *close)
{
  PDU *out_pdu;

  out_pdu = MakeCloseResponse(NULL, 9 /* eg, unspecified */, 
			      "Close Request from Origin", 
			      NULL, NULL);

  return (out_pdu);
}

PDU *
HandleExtendedServicesRequest(ZSESSION *session, 
			      ExtendedServicesRequest *esr)
{
  return NULL;
}
 


int
HandleClientConnection(session, cfd)
     ZSESSION	*session;
     int cfd;
{
  /*
   *  int responseRequired;
   */
  PDU *in_pdu, *out_pdu;
  PDU *rc_in_pdu, *rc_out_pdu;
  extern FILE *LOGFILE;

  session->c_fd = cfd;
  while (1) {
    in_pdu = GetPDU(cfd, session->s_timeOut);
    if (in_pdu == NULL) {
      fprintf(LOGFILE, "Nothing returned from GetPDU call. Connection closed.\n");
      fflush(LOGFILE);
      
      break;
    }
    fprintf(LOGFILE, "Incoming PDU:\n");
    PrintPDU(LOGFILE,in_pdu);

    ClearError (session);
    switch (in_pdu->which) {
    case e1_initRequest: 
      /*
	rc_out_pdu = MakeAccessControlRequest (NULL,"password");
	PrintPDU(LOGFILE,rc_out_pdu);
	PutPDU(cfd, rc_out_pdu);
	FreePDU(rc_out_pdu);
	
	rc_in_pdu = GetPDU(cfd, session->s_timeOut);
	assert (rc_in_pdu->which == e1_accessControlResponse);
	fprintf(LOGFILE, "Incoming pdu.\n");
	PrintPDU(LOGFILE,rc_in_pdu);
	FreePDU(rc_in_pdu);
	*/
      
      session->state = S_INIT_RECEIVED;
      out_pdu = HandleInitializeRequest(session, in_pdu->u.initRequest);
      if (out_pdu != NULL) {
	session->s_state = Z_OPEN;
      }
      break;
      
    case e1_searchRequest:
      /*
	responseRequired = 1;
	rc_out_pdu = MakeResourceControlRequest(NULL,1,NULL,3,responseRequired, 0);
	PrintPDU(LOGFILE,rc_out_pdu);
	PutPDU(cfd, rc_out_pdu);
	FreePDU(rc_out_pdu);
	if (responseRequired != 0) {
	rc_in_pdu = GetPDU(cfd, session->s_timeOut);
	assert (rc_in_pdu->which == e1_resourceControlResponse);
	fprintf(LOGFILE, "Incoming pdu.\n");
	PrintPDU(LOGFILE,rc_in_pdu);
	FreePDU(rc_in_pdu);
	}
	*/
      session->state = S_SEARCH_RECEIVED; 
      out_pdu = HandleSearchRequest(session, in_pdu->u.searchRequest);
      break;
      
    case e1_presentRequest:
      session->state = S_PRESENT_RECEIVED; 
      out_pdu = HandlePresentRequest(session, in_pdu->u.presentRequest);
      break;
      
    case e1_deleteResultSetRequest:
      session->state = S_DELETE_RECEIVED; 
      out_pdu = HandleDeleteResultSetRequest(session, in_pdu->u.deleteResultSetRequest);
      break;
      
    case e1_resourceReportRequest:
      session->state = S_RESOURCE_REPORT_RECEIVED; 
      out_pdu = HandleResourceReportRequest(session, in_pdu->u.resourceReportRequest);
      break;
      
    case e1_triggerResourceControlRequest:
      /* special case: */
      session->state = S_TRIGGER_RESOURCE_CONTROL_RECEIVED;
      /*
	out_pdu = HandleTriggerResourceControlRequest(session, in_pdu->u.triggerResourceControlRequest);
	*/
      continue;

    case e1_sortRequest:
      session->state = S_SORT_RECEIVED; 
      out_pdu = HandleSortRequest(session, in_pdu->u.sortRequest);
      break;

    case e1_scanRequest:
      session->state = S_SCAN_RECEIVED; 
      out_pdu = HandleScanRequest(session, in_pdu->u.scanRequest);
      break;

    case e1_close:
      session->state = S_CLOSE_RECEIVED; 
      out_pdu = HandleCloseRequest(session, in_pdu->u.close);
      /* cleanup */
      out_pdu = NULL;

      break;

    case e1_extendedServicesRequest:
      session->state = S_EXTENDED_SERVICES_RECEIVED; 
      out_pdu = HandleExtendedServicesRequest(session, 
					      in_pdu->u.extendedServicesRequest);
      break;

    default:
      /*	To be implemented.
       */
      fprintf(LOGFILE,"illegal client request received.\n");
      break;
    }
    /* out_pdu should never be NULL. If it is NULL, then terminate
     * the connection. 
     */
    if (out_pdu == NULL) {
      fprintf(LOGFILE,"Empty response pdu - system error.\n");
      break;
    }
    fprintf(LOGFILE, "Outgoing PDU.\n");
   
    PrintPDU(LOGFILE,out_pdu);
   
    /*
     *	Transmit the response to the client.
     */
    if (PutPDU(cfd, out_pdu) != Z_OK)
      break;
    fprintf(LOGFILE, "Finished transmitting outgoing PDU.\n");
    fflush(LOGFILE);

    FreePDU(in_pdu);
    FreePDU(out_pdu);
    
  }
  /*
   * free the space taken by session variable, remove the result
   * set files kept for this client.
   *
   *	TO BE IMPLEMENTED.
   */
  /* session->connectionStatus = Z_CLOSE; ? */
  return 0;
}

int
SetClientOptions(session)
     ZSESSION	*session;
{

  char *options;

  if (!session)
    return Z_ERROR;

  options = session->c_options;

  if (!options)
    return Z_ERROR;

  session->c_supportVersion2 = 
    (session->c_protocolVersion[1]=='1') ? Z_TRUE : Z_FALSE;
  session->c_supportVersion3 = 
    (session->c_protocolVersion[1]=='1') ? Z_TRUE : Z_FALSE;


  if (session->c_supportVersion2) {
    session->c_requestSearch = (options[0] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestPresent = (options[1] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestDeleteResultSet = (options[2] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestResourceReport = (options[3] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestTriggerResourceControl = 
      (options[4] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestResourceControl = (options[5] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestAccessControl = (options[6] == '1') ? Z_TRUE : Z_FALSE;
    
  }
  if (session->c_supportVersion3) {
    session->c_requestScan = (options[7] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestSort = (options[8] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestExtendedServices = 
      (options[10] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestLevel_1_Segmentation = 
      (options[11] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestLevel_2_Segmentation = 
      (options[12] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestConcurrentOperations = 
      (options[13] == '1') ? Z_TRUE : Z_FALSE;
    session->c_requestNamedResultSets = 
      (options[14] == '1') ? Z_TRUE : Z_FALSE;
  }
  return Z_OK;

}




