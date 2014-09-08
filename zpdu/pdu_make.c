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
#include <ctype.h>
#include <string.h>
#include "z3950_3.h"
#include "list.h"
#include "ccl.h"
#include "z_parameters.h"
#include "z3950_intern.h"

extern FILE *LOGFILE;

extern OctetString DupOstring(OctetString in);

PDU *
MakeInitializeRequest(referenceId, protocolVersion, options,
		      preferredMessageSize, exceptionalRecordSize,
		      idAuthentication, implementationId,
		      implementationName, implementationVersion,
		      userInformationField)
     char	*referenceId;	/* OPTIONAL:	NULL indicates absence.	*/
     char	*protocolVersion;
     char	*options;
     int	preferredMessageSize;
     int	exceptionalRecordSize;
     any	idAuthentication;/* OPTIONAL: NULL indicates absence. */
     char	*implementationId;/* OPTIONAL: NULL indicates absence. */
     char	*implementationName; 	/* OPTIONAL */
     char	*implementationVersion; /* OPTIONAL */
     External *userInformationField; /* OPTIONAL	*/
{
  PDU *pdu;
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_initRequest;
  pdu->u.initRequest = CALLOC(InitializeRequest,1);
  
  pdu->u.initRequest->referenceId = NewOctetString(referenceId);
  pdu->u.initRequest->protocolVersion = NewBitString(protocolVersion);
  pdu->u.initRequest->options = NewBitString(options);
  pdu->u.initRequest->preferredMessageSize = preferredMessageSize;
  pdu->u.initRequest->exceptionalRecordSize = exceptionalRecordSize;
  pdu->u.initRequest->idAuthentication = idAuthentication;
  pdu->u.initRequest->implementationId = NewOctetString(implementationId);
  pdu->u.initRequest->implementationName = 
    NewOctetString(implementationName);
  pdu->u.initRequest->implementationVersion = 
    NewOctetString(implementationVersion);
  pdu->u.initRequest->userInformationField = userInformationField;
  return pdu;
}


PDU *
MakeInitializeResponse(referenceId, protocolVersion, options,
		       preferredMessageSize, exceptionalRecordSize,
		       result, implementationId,
		       implementationName, implementationVersion,
		       userInformationField)
     OctetString referenceId;	/* OPTIONAL:	NULL indicates absence.	*/
     char	*protocolVersion;
     char	*options;
     int	preferredMessageSize;
     int	exceptionalRecordSize;
     Boolean	result;	/* 0 means false, anything else true	*/
     char	*implementationId;/* OPTIONAL: NULL indicates absence. */
     char	*implementationName; 	/* OPTIONAL */
     char	*implementationVersion; /* OPTIONAL */
     External *userInformationField; /* OPTIONAL	*/
{
  PDU *pdu;
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_initResponse;
  pdu->u.initResponse = CALLOC(InitializeResponse,1);
  pdu->u.initResponse->referenceId = DupOstring(referenceId);
  pdu->u.initResponse->protocolVersion = NewBitString(protocolVersion);
  pdu->u.initResponse->options = NewBitString(options);
  pdu->u.initResponse->preferredMessageSize = preferredMessageSize;
  pdu->u.initResponse->exceptionalRecordSize = exceptionalRecordSize;
  pdu->u.initResponse->result = result;
  pdu->u.initResponse->implementationId = NewOctetString(implementationId);
  pdu->u.initResponse->implementationName = 
    NewOctetString(implementationName);
  pdu->u.initResponse->implementationVersion = 
    NewOctetString(implementationVersion);
  pdu->u.initResponse->userInformationField = userInformationField;
  
  return pdu;
}



PDU * 
MakeSearchRequest(referenceId, smallSetUpperBound, largeSetLowerBound, 
		  mediumSetPresentNumber, replaceIndicator, resultSetName, 
		  databaseNames, smallSetElementSetNames, 
		  mediumSetElementSetNames,preferredRecordSyntax, 
		  query, additionalSearchInfo) 
     char	*referenceId;	/* Pass NULL to indicate optional.	*/
     int	smallSetUpperBound;
     int 	largeSetLowerBound;
     int	mediumSetPresentNumber;
     Boolean    replaceIndicator;	/* 0 means false, anything else true.	*/
     char	*resultSetName;
     char	**databaseNames;	/* NULL-terminated array of database names. */ 
     ElementSetNames	*smallSetElementSetNames;   /* NULL to indicate optional. */
     ElementSetNames	*mediumSetElementSetNames;  /* NULL to indicate optional. */
     char	*preferredRecordSyntax;		/* Pass NULL to indicate optional. */
     Query	*query;
     OtherInformation *additionalSearchInfo;			
{
  PDU *pdu;
    struct databaseNames_List1 *head, *tail, *db;
  int i;
  
  SearchRequest* tmp = CALLOC(SearchRequest,1);
  pdu = CALLOC(PDU,1);
  pdu->which = e1_searchRequest;
  pdu->u.searchRequest = tmp;
  
  if (referenceId != NULL)
    tmp->referenceId = NewOctetString(referenceId);
  else
    tmp->referenceId = NULL;
  tmp->smallSetUpperBound = smallSetUpperBound;
  tmp->largeSetLowerBound= largeSetLowerBound;
  tmp->mediumSetPresentNumber = mediumSetPresentNumber;
  tmp->replaceIndicator = replaceIndicator;
  tmp->resultSetName = NewOctetString(resultSetName);
  head = NULL;
  for (i=0; databaseNames[i] != NULL; i++) {
    db = CALLOC(struct databaseNames_List1,1);
    db->item = NewInternationalString(databaseNames[i]);
    db->next = 0;
    if (head == NULL) {
      head = db;
      tail = db;
    } else {
      tail->next = db;
      tail = db;
    }
  }
  tmp->databaseNames = head;
  tmp->smallSetElementSetNames = smallSetElementSetNames;
  tmp->mediumSetElementSetNames = mediumSetElementSetNames;
  if (preferredRecordSyntax != NULL)
    tmp->preferredRecordSyntax = NewOID(preferredRecordSyntax);
  else
    tmp->preferredRecordSyntax = NULL;
  tmp->query = query;
  tmp->additionalSearchInfo = additionalSearchInfo;
  return pdu;
}

PDU *
MakeSearchResponse(referenceId, resultCount, numberOfRecordsReturned,
		   nextResultSetPosition, searchStatus, resultSetStatus,
		   presentStatus, records)
     OctetString referenceId;		/* OPTIONAL	*/
     int	resultCount;
     int	numberOfRecordsReturned;
     int	nextResultSetPosition;
     Boolean	searchStatus;
     int	resultSetStatus;	/* OPTIONAL	*/
     int	presentStatus;		/* OPTIONAL	*/
     Records	*records;	/* OPTIONAL	*/
{
  PDU *pdu;
  SearchResponse	*sr = CALLOC(SearchResponse,1);
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_searchResponse;
  pdu->u.searchResponse = sr;
  
  sr->referenceId = DupOstring(referenceId);
  sr->resultCount = resultCount;
  sr->numberOfRecordsReturned = numberOfRecordsReturned;
  sr->nextResultSetPosition = nextResultSetPosition;
  sr->searchStatus = searchStatus;
  sr->resultSetStatus = resultSetStatus;
  sr->presentStatus = presentStatus;
  sr->records = records;
  
  return pdu;
}

PDU * 
MakePresentRequest(referenceId, resultSetId, resultSetStartPoint, 
		   numberOfRecordsRequested,
		   elementSetNames, 
		   preferredRecordSyntax, otherinfo) 
     char	*referenceId;	/* OPTIONAL.	*/
     char 	*resultSetId;
     int 	resultSetStartPoint;
     int 	numberOfRecordsRequested;
     char       *elementSetNames;
     char	*preferredRecordSyntax;		/* OPTIONAL.	*/
     OtherInformation *otherinfo;               /* OPTIONAL.    */
{
  PDU *pdu;
  
  PresentRequest* tmp = CALLOC(PresentRequest,1);
  pdu = CALLOC(PDU,1);
  pdu->which = e1_presentRequest;
  pdu->u.presentRequest = tmp;
  
  
  tmp->referenceId = NewOctetString(referenceId);
  tmp->resultSetId = NewOctetString(resultSetId);
  tmp->resultSetStartPoint = resultSetStartPoint;
  tmp->numberOfRecordsRequested = numberOfRecordsRequested;

  /* this only will work for single elementset names right now */
  tmp->recordComposition = CALLOC(struct RecordComposition,1);
  tmp->recordComposition->which =  e9_simple;
  tmp->recordComposition->u.simple = 
    MakeElementSetNames(NULL, (char **)elementSetNames);
  tmp->preferredRecordSyntax = NewOID(preferredRecordSyntax);
  tmp->otherInfo = otherinfo;
  return pdu;
}


PDU *
MakePresentResponse(referenceId, numberOfRecordsReturned,
		    nextResultSetPosition, presentStatus,
		    records)
     OctetString referenceId;
     int	numberOfRecordsReturned;
     int	nextResultSetPosition;
     int	presentStatus;
     Records	*records;
{
  PDU *pdu;
  PresentResponse *pr = CALLOC(PresentResponse,1);
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_presentResponse;
  pdu->u.presentResponse = pr;
  
  pr->referenceId = DupOstring(referenceId);
  pr->numberOfRecordsReturned = numberOfRecordsReturned;
  pr->nextResultSetPosition = nextResultSetPosition;
  if ((presentStatus < 0) || (presentStatus > 5)) {
    fprintf(LOGFILE, "present status out of bound.\n");
    return NULL;
  }
  pr->presentStatus = presentStatus;
  pr->records = records;
  
  return pdu;
}


PDU *
MakeScanRequest(char *referenceId, char **dbnames,
		char *attributeSetId, AttributesPlusTerm *terms,
		int stepsize, int numberofterms, int preferredpos)
{

  PDU *pdu;
  struct databaseNames_List12 *head, *tail, *db;
  int i;
  
  ScanRequest* tmp = CALLOC(ScanRequest,1);
  pdu = CALLOC(PDU,1);
  pdu->which = e1_scanRequest;
  pdu->u.scanRequest = tmp;
  
  
  tmp->referenceId = NewOctetString(referenceId);
  tmp->attributeSet = NewOID(attributeSetId);
  tmp->termListAndStartPoint = terms;
  tmp->stepSize = stepsize;
  tmp->numberOfTermsRequested = numberofterms;
  tmp->preferredPositionInResponse = preferredpos;

  head = NULL;
  for (i=0; dbnames[i] != NULL; i++) {
    db = CALLOC(struct databaseNames_List12,1);
    db->item = NewInternationalString(dbnames[i]);
    db->next = 0;
    if (head == NULL) {
      head = db;
      tail = db;
    } else {
      tail->next = db;
      tail = db;
    }
  }
  tmp->databaseNames = head;


  return pdu;
}

PDU *
MakeScanResponse(OctetString referenceId, int stepUsed, int status, int position,
		 char *attributesetid,
		      ListEntries *listEntries)
{
  PDU *pdu;
  ScanResponse *sr = CALLOC(ScanResponse,1);
  struct entries_List13	*entry;
  int entrycount;

  pdu = CALLOC(PDU,1);
  pdu->which = e1_scanResponse;
  pdu->u.scanResponse = sr;
  
  sr->referenceId = DupOstring(referenceId);
  sr->stepSize = stepUsed;
  sr->scanStatus = status;

  entrycount = 0;

  if (listEntries->entries != NULL)
    for (entry = listEntries->entries; entry; entry = entry->next)
      entrycount++;

  sr->numberOfEntriesReturned = entrycount;
  sr->positionOfTerm = position;
  sr->attributeSet = NewOID(attributesetid);

  sr->entries = listEntries;

  return pdu;

}


PDU *
MakeSortRequest(char *referenceId, char **in_names, char *out_name,
		struct sortSequence_List18 *sort_seq)
{

  PDU *pdu;
  struct inputResultSetNames_List17 *in_head, *in_tail, *in_results;
  int i;
  
  SortRequest* tmp = CALLOC(SortRequest,1);
  pdu = CALLOC(PDU,1);
  pdu->which = e1_sortRequest;
  pdu->u.sortRequest = tmp;
  
  
  tmp->referenceId = NewOctetString(referenceId);
  
  in_head = NULL;
  for (i=0; in_names[i] != NULL; i++) {
    in_results = CALLOC(struct inputResultSetNames_List17,1);
    in_results->item = NewInternationalString(in_names[i]);
    in_results->next = 0;
    if (in_head == NULL) {
      in_head = in_results;
      in_tail = in_results;
    } else {
      in_tail->next = in_results;
      in_tail = in_results;
    }
  }
  tmp->inputResultSetNames = in_head;
  tmp->sortedResultSetName = NewInternationalString(out_name);
  tmp->sortSequence = sort_seq;
  
  return pdu;
}

PDU *
MakeSortResponse(OctetString referenceId, int status, int resultstatus,
		 struct diagnostics_List19 *diagnostics, int result_set_size)
{
  PDU *pdu;
  SortResponse *sr = CALLOC(SortResponse,1);
  char buffer[50];

  pdu = CALLOC(PDU,1);
  pdu->which = e1_sortResponse;
  pdu->u.sortResponse = sr;
  
  sr->referenceId = DupOstring(referenceId);
  sr->sortStatus = status;
  sr->resultSetStatus = resultstatus;
#ifdef Z3950_2001
  sr->resultCount = result_set_size;
#endif
  sr->diagnostics = diagnostics;
  if (status == 0) {
    sr->otherInfo = CALLOC(struct OtherInformation, 1);
    sr->otherInfo->item.information.which = e24_characterInfo;
    sprintf(buffer,"Result-count: %d", result_set_size);
    sr->otherInfo->item.information.u.characterInfo = 
      NewInternationalString(buffer);
   }

  return pdu;

}



PDU *
MakeDeleteResultSetRequest(referenceId, deleteFunction, 
			   resultSetList, numberResultSets)
     char       *referenceId;		/* OPTIONAL	*/
     int	deleteFunction;	/* 0 means list, 1 means all	*/
     char	**resultSetList;	/* OPTIONAL.	*/
     int	numberResultSets;
{
  PDU *pdu;
  DeleteResultSetRequest *rsr = CALLOC(DeleteResultSetRequest,1);
  struct resultSetList_List11 *tail = NULL, *head = NULL, *tmp;
  int i;
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_deleteResultSetRequest;
  pdu->u.deleteResultSetRequest = rsr;
  if ((deleteFunction < 0) || (deleteFunction > 1)) {
    fprintf(LOGFILE, "Invalid delete operation code.\n");
    return NULL;
  }
  rsr->deleteFunction = deleteFunction;
  if (deleteFunction == 1) {
    rsr->resultSetList = NULL;
    return pdu;
  }
  if (deleteFunction == 0) {
    if ((numberResultSets <= 0) || (resultSetList == (char **)NULL)) {
      fprintf(LOGFILE,"No result sets to be deleted in MakeDeleteResultSetRequest.\n");
      return NULL;
    }
  }
  
  /*
   * call a utility routine to convert a list of result
   * set names in the string format to a linked list of
   * result set names.
   */
  tail = NULL;
  head = NULL;
  for (i=0; i<numberResultSets; i++) { 
    tmp = CALLOC(struct resultSetList_List11,1);
    if (!tmp) {
      fprintf(LOGFILE,"out of memory.\n");
      return NULL;
    }
    tmp->item = NewOctetString(resultSetList[i]);
    tmp->next = NULL;
    if (head == NULL) {
      head = tmp;
      tail = head;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  rsr->resultSetList = head;
  return pdu;
}

PDU *
MakeDeleteResultSetResponse(referenceId, deleteOperationStatus, 
			    deleteListStatuses, numberNotDeleted,
			    bulkStatuses, deleteMessage)
     OctetString referenceId;	/* OPTIONAL	*/
     int	deleteOperationStatus;
     ListStatuses	*deleteListStatuses;	/* OPTIONAL	*/
     int	numberNotDeleted;	/* OPTIONAL	*/
     ListStatuses	*bulkStatuses;	/* OPTIONAL	*/
     char	*deleteMessage;		/* OPTIONAL	*/
{
  PDU *pdu = CALLOC(PDU,1);
  DeleteResultSetResponse *rsp = CALLOC(DeleteResultSetResponse,1);
  
  pdu->which = e1_deleteResultSetResponse;
  pdu->u.deleteResultSetResponse = rsp;
  
  rsp->referenceId = DupOstring(referenceId);
  rsp->deleteOperationStatus = deleteOperationStatus;
  rsp->deleteListStatuses = deleteListStatuses;
  rsp->numberNotDeleted = numberNotDeleted;
  rsp->bulkStatuses = bulkStatuses;
  rsp->deleteMessage = NewOctetString(deleteMessage);
  
  return pdu;
}
/*
   PDU *
   MakeAccessControlRequest(referenceId, securityChallenge)
   char	*referenceId;	
   char	*securityChallenge;
   {
   PDU *pdu = CALLOC(PDU,1);
   AccessControlRequest *acr = CALLOC(AccessControlRequest,1);
   
   pdu->which = e1_accessControlRequest;
   pdu->u.accessControlRequest = acr;
   acr->referenceId = NewOctetString(referenceId);
   acr->securityChallenge = NewOctetString(securityChallenge);
   
   return pdu;
   }
   */

PDU *
MakeAccessControlResponse(referenceId, securityChallengeResponse)
     OctetString referenceId;	/* OPTIONAL	*/
     char	*securityChallengeResponse;
{
  PDU *pdu = CALLOC(PDU,1);
  AccessControlResponse *acr = CALLOC(AccessControlResponse,1);

  pdu->which = e1_accessControlResponse;
  pdu->u.accessControlResponse = acr;
  acr->referenceId = DupOstring(referenceId);
  /* in v3 this is really a structure... */
  acr->securityChallengeResponse = CALLOC(struct SecurityChallengeResponse,1);  
  acr->securityChallengeResponse->which = e18_simpleForm;  
  acr->securityChallengeResponse->u.simpleForm =   
    NewOctetString(securityChallengeResponse);  
  
  /* this is not doing diagnostics right now */

  return pdu;
}

PDU *
MakeResourceControlRequest(referenceId, suspendedFlag, resourceReport,
			   partialResultsAvailable, responseRequired,
			   triggeredRequestFlag)
     char	*referenceId;		/* OPTIONAL	*/
     Boolean	suspendedFlag;		/* OPTIONAL	*/
     ResourceReport	*resourceReport;	/* OPTIONAL	*/
     int	partialResultsAvailable;/* OPTIONAL	*/
     Boolean	responseRequired;
     Boolean	triggeredRequestFlag;	/* OPTIONAL	*/
{
  PDU *pdu = CALLOC(PDU,1);
  ResourceControlRequest *rcr = CALLOC(ResourceControlRequest,1);
  
  pdu->which = e1_resourceControlRequest;
  pdu->u.resourceControlRequest = rcr;
  rcr->referenceId = NewOctetString(referenceId);
  rcr->suspendedFlag = suspendedFlag;
  rcr->resourceReport = resourceReport;
  rcr->partialResultsAvailable = partialResultsAvailable;
  rcr->responseRequired = responseRequired;
  rcr->triggeredRequestFlag = triggeredRequestFlag;
  
  return pdu;
}

PDU *
MakeResourceControlResponse(referenceId, continueFlag, resultSetWanted)
     OctetString referenceId;		/* OPTIONAL	*/
     Boolean	continueFlag;
     Boolean	resultSetWanted;	/* OPTIONAL	*/
{
  PDU *pdu = CALLOC(PDU,1);
  ResourceControlResponse *rcr = CALLOC(ResourceControlResponse,1);
  
  pdu->which = e1_resourceControlResponse;
  pdu->u.resourceControlResponse = rcr;
  rcr->referenceId = DupOstring(referenceId);
  rcr->continueFlag = continueFlag;
  rcr->resultSetWanted = resultSetWanted;
  
  return pdu;
}


PDU *
MakeTriggerResourceControlRequest(referenceId, requestedAction, 
				  prefResourceReportFormat, resultSetWanted)
     char	*referenceId;		/* OPTIONAL	*/
     int	requestedAction;
     char	*prefResourceReportFormat;	/* OPTIONAL	*/
Boolean	resultSetWanted;	/* OPTIONAL	*/
/* TRUE=1, FALSE=0, OPTIONAL=-1	*/
{
  PDU *pdu = CALLOC(PDU,1);
  TriggerResourceControlRequest *trcr = CALLOC(TriggerResourceControlRequest,1);
  
  pdu->which = e1_triggerResourceControlRequest;
  pdu->u.triggerResourceControlRequest = trcr;
  trcr->referenceId = NewOctetString(referenceId);
  trcr->requestedAction = requestedAction;
  trcr->prefResourceReportFormat = NewOID(prefResourceReportFormat);
  trcr->resultSetWanted = resultSetWanted;
  
  return pdu;
}


PDU *
MakeResourceReportRequest(referenceId, prefResourceReportFormat)
     char	*referenceId;		/* OPTIONAL	*/
     char	*prefResourceReportFormat;	/* OPTIONAL	*/
{
  PDU *pdu = CALLOC(PDU,1);
  ResourceReportRequest *rrr = CALLOC(ResourceReportRequest,1);
  
  pdu->which = e1_resourceReportRequest;
  pdu->u.resourceReportRequest = rrr;
  rrr->referenceId = NewOctetString(referenceId);
  rrr->prefResourceReportFormat = NewOID(prefResourceReportFormat);
  
  return pdu;
}


PDU *
MakeResourceReportResponse(referenceId, resourceReportStatus, resourceReport)
     OctetString referenceId;		/* OPTIONAL	*/
     int	resourceReportStatus;
     ResourceReport *resourceReport;	/* OPTIONAL	*/
{
  PDU *pdu = CALLOC(PDU,1);
  ResourceReportResponse *rrr = CALLOC(ResourceReportResponse,1);
  
  pdu->which = e1_resourceReportResponse;
  pdu->u.resourceReportResponse = rrr;
  rrr->referenceId = DupOstring(referenceId);
  rrr->resourceReportStatus = resourceReportStatus;
  rrr->resourceReport = resourceReport;
  
  return pdu;
}


PDU *
MakeCloseResponse(OctetString referenceId, 
		  int closeReason, char *diagnosticInfo, 
		  char *resourceReportFormat, 
		  ResourceReport *resourceReport)
{
  PDU *pdu;
  
  pdu = CALLOC(PDU,1);
  pdu->which = e1_close;
  pdu->u.close = CALLOC(Close,1);
  
  pdu->u.close->referenceId = DupOstring(referenceId);
  pdu->u.close->closeReason = closeReason;
  pdu->u.close->diagnosticInformation = NewInternationalString(diagnosticInfo);
  pdu->u.close->resourceReportFormat = NewOID(resourceReportFormat);
  pdu->u.close->resourceReport = resourceReport;

  return pdu;
}


NamePlusRecord *
MakeNamePlusRecord(databaseName, which, record)
     char	*databaseName;
     int	which;
     void	*record;
{
  NamePlusRecord	*npr = CALLOC(NamePlusRecord,1);
  npr->name = NewOctetString(databaseName);
  npr->record.which = which;
  if (which ==  e11_retrievalRecord)
    npr->record.u.retrievalRecord = (External *)record;
  else
    npr->record.u.surrogateDiagnostic = (DiagRec *)record;
  return npr;
}


Records *
AddDatabaseRecords(records, mlist, start_point, number_records, databaseName)
     Records 	*records;
     AC_LIST *mlist;			
     int start_point;		
     int number_records;
     char	*databaseName;
{
  return records;
}
/*
   Records *
   MakeDatabaseRecords(mlist, start_point, number_records, databaseName)
   AC_LIST *mlist;			
   int start_point;		
   int number_records;
   char	*databaseName;
   {
   char *marc;
   Records *tmp;
   struct dataBaseOrSurDiagnostics_list *r1 = CALLOC(struct dataBaseOrSurDiagnostics_list,1);
   struct dataBaseOrSurDiagnostics_list *head = NULL, *tail = NULL;
   External *databaseRecord;
   int i;
   
   tmp = CALLOC(Records,1);
   tmp->which = e1_dataBaseOrSurDiagnostics;
   
   marc = ac_list_first(mlist);
   for (i=0; i<number_records; i++) {
   marc = ac_list_nth(mlist, start_point+i-1);
   r1 = CALLOC(struct dataBaseOrSurDiagnostics_list,1);
   r1->next = NULL;
   databaseRecord = CALLOC (External,1);
   databaseRecord->directReference = NewOID(OID_USMARC);
   databaseRecord->which = t_octetAligned;
   databaseRecord->u.octetAligned = NewOctetString(marc);
   r1->item = MakeNamePlusRecord(databaseName, e1_databaseRecord,
   databaseRecord);
   if (head == NULL) {
   head = r1;
   tail = r1;
   } else {
   tail->next = r1;
   tail = r1;
   }
   }
   tmp->u.dataBaseOrSurDiagnostics = head;
   return tmp;
   }
   
   extern int DIAG_CONDITION;
   extern char *DIAG_MESSAGES[];
   Records *
   MakeSurrogateDiagRecords()
   {
   Records *tmp;
   struct dataBaseOrSurDiagnostics_list *r1 = CALLOC(struct dataBaseOrSurDiagnostics_list,1);
   DiagRec *diagRecord = CALLOC(DiagRec,1);
   
   tmp = CALLOC(Records,1);
   tmp->which = e1_dataBaseOrSurDiagnostics;
   tmp->u.dataBaseOrSurDiagnostics = r1;
   r1->next = 0;
   r1->item = CALLOC(NamePlusRecord,1);
   r1->item->databaseName = NewOctetString("cat");
    r1->item->record.which = e1_surrogateDiagnostic;
    r1->item->record.u.surrogateDiagnostic = diagRecord;
    diagRecord->diagnosticSetId = NewOID (OID_BIB1);
    diagRecord->condition = DIAG_CONDITION;
    diagRecord->addinfo = NewOctetString(DIAG_MESSAGES[DIAG_CONDITION]);
    
    return tmp;
}
----- */

Records *
MakeNonSurrogateDiagRecords(diagnosticSetId, condition, addinfo)
     char	*diagnosticSetId;
     int	condition;
     char	*addinfo;
{
  Records *tmp;
  DefaultDiagFormat *diagRecord = CALLOC(DefaultDiagFormat,1);
  
  tmp = CALLOC(Records,1);
  tmp->which = e10_nonSurrogateDiagnostic;
  tmp->u.nonSurrogateDiagnostic = diagRecord;
  diagRecord->diagnosticSetId = NewOID (diagnosticSetId);
  diagRecord->condition = condition;
  diagRecord->addinfo.which = e14_v3Addinfo;
  diagRecord->addinfo.u.v3Addinfo = NewOctetString(addinfo);
  return tmp;
}

DiagRec *
MakeDiagRec(char *diagnosticSetId, int condition, char *addinfo)
{
  DiagRec *diagRec;
  DefaultDiagFormat *defdiag;
 
  diagRec = CALLOC(DiagRec, 1);
  diagRec->which = e13_defaultFormat;

  defdiag = CALLOC(DefaultDiagFormat, 1);
  diagRec->u.defaultFormat = defdiag;

  defdiag->diagnosticSetId = NewOID (diagnosticSetId);
  defdiag->condition = condition;
  defdiag->addinfo.which = e14_v3Addinfo;
  defdiag->addinfo.u.v3Addinfo = NewInternationalString(addinfo);

  return(diagRec);

}


/*
 * Arguments:
 *    which		Must be e1_generic or e1_databaseSpecific.
 *    databaseNames	Array of pointers to database names. The
 *			last element in the array must be NULL.
 *    elementSetNames	Array of pointers to element set names. The
 *			last element in the array must be NULL. This
 *			argument will be ignored if the first argument
 *			<which> takes as values e1_generic.
*/

ElementSetNames *
MakeElementSetNames (databaseNames, elementSetNames)
char	**databaseNames;
char	**elementSetNames;

{

    

    ElementSetNames	*esn;

    if (elementSetNames == NULL)
      return (ElementSetNames *)NULL;

    if ((esn = CALLOC(ElementSetNames,1)) == NULL)
      return (ElementSetNames *)NULL;

    if (databaseNames == NULL) {
      esn->which =  e15_genericElementSetName;
      esn->u.genericElementSetName = NewOctetString((char *)elementSetNames);
    }
    else {
      struct databaseSpecific_List8	*tmp, *head, *tail;	
      int i=0;
      esn->which = e15_databaseSpecific;

      head = NULL; 
      tail = NULL;
      for (i=0; databaseNames[i] != NULL; i++) {
	if ((tmp = CALLOC (struct databaseSpecific_List8,1)) == NULL)
	  return (ElementSetNames *)NULL;
	tmp->next = NULL;
	tmp->item.dbName = NewOctetString(databaseNames[i]); 
	tmp->item.esn = NewOctetString(elementSetNames[i]);
	if (head == NULL) {
	  head = tmp;
	  tail = tmp;
	} else {
	  tail->next = tmp;
	  tail = tmp;
	}
      }
      esn->u.databaseSpecific = head;
    }
    return esn;
}


Query *
MakeQueryType1 (rpn_query, attributeset)
     RPNQuery *rpn_query;
     char *attributeset;
{
  Query  *query;
  if (rpn_query == NULL)
    return (Query *)NULL;
  query = CALLOC(Query,1);
  query->which = e2_type_1;
  query->u.type_1 = rpn_query;
  /* if no attribute set defined, use the default (currently BIB-1) */
  if (rpn_query->attributeSet == NULL) {
    if (attributeset == NULL) 
      query->u.type_1->attributeSet = NewOID(OID_DEFAULT);
    else
      query->u.type_1->attributeSet = NewOID(attributeset); 
  }
  return query;
}

Query *
MakeQueryType2 (data)
     char	*data;
{
  Query  *query;
  if (data == NULL)
    return (Query *)NULL;
  query = CALLOC(Query,1);
  query->which = e2_type_2;
  query->u.type_2 = NewOctetString(data);
  return query;
}

Query *
MakeQueryType100 (data)
     char	*data;
{
  Query  *query;
  if (data == NULL)
    return (Query *)NULL;
  query = CALLOC(Query,1);
  query->which = e2_type_100;
  query->u.type_100 = NewOctetString(data);
  return query;
}

Query *
MakeQueryType101  (rpn_query, attributeset)
     RPNQuery *rpn_query;
     char *attributeset;
{
  Query  *query;

  if (rpn_query == NULL)
    return (Query *)NULL;

  query = CALLOC(Query,1);
  query->which = e2_type_101; 
  query->u.type_101 =  rpn_query;
  /* if no attribute set defined, use the default (currently BIB-1) */
  if (attributeset == NULL) 
     query->u.type_1->attributeSet = NewOID(OID_DEFAULT);
  else
    query->u.type_1->attributeSet = NewOID(attributeset); 

  return query;
}

Query *
MakeQueryType102 (data)
     char	*data;
{
  Query  *query;
  if (data == NULL)
    return (Query *)NULL;
  query = CALLOC(Query,1);
  query->which = e2_type_102;
  query->u.type_102 = NewOctetString(data);
  return query;
}

Query *
MakeQueryType0 (data)
     char	*data;
{
  Query  *query;
  if (data == NULL)
    return (Query *)NULL;
  query = CALLOC(Query,1);
  query->which = e2_type_0;
  query->u.type_0 = NewOctetString(data);
  return query;
}

/*
Estimate *
MakeEstimate (type, value, code)
int	type;
int	value;
int	code;
{
    Estimate	*estimate;
    if ((estimate = CALLOC(Estimate,1)) == NULL)
	return (Estimate *)NULL;
    estimate->estimateType = type;
    estimate->estimateValue = value;
    estimate->currency_code = code;
    return estimate;
}
*/

ResourceReport *
MakeResourceReport (message)
     char	*message;
{
  ResourceReport *report;
  if ((report = CALLOC(ResourceReport,1)) == NULL)
    return (ResourceReport *)NULL;
  report->estimates = NULL;
  report->message = NewOctetString(message);
  return report;
}

/*
ResourceReport *
ExpandResourceReport(resourceReport, estimateType, estimateValue, currency_code)
ResourceReport	*resourceReport;
int 	estimateType;
int	estimateValue;
int	currency_code;
{
    Estimate *estimate;
    struct estimates_list  *curr, *prev, *tmp;
    ResourceReport *report;

    if ((report = resourceReport) == NULL)
	return (ResourceReport *)NULL;
    estimate = MakeEstimate (estimateType, estimateValue, currency_code);
    tmp = CALLOC(struct estimates_list,1);
    tmp->next = NULL;
    tmp->item = estimate;
    for (curr=report->estimates, prev=curr; curr!=NULL; prev=curr, curr=curr->next)
	;
    if (prev == NULL)
	report->estimates = tmp;
    else
	prev->next = tmp;
    return report;
}
*/


/*
 * Convert a list of items to a NULL-terminated array of pointers to items.
 */
char **
ListToArgv (list)
     AC_LIST	*list;
{
  char **argv;
  int	 argc;
  char *item;
  int	 i;
  
  if (list == (AC_LIST *)NULL)
    return (char **)NULL;
  argc = ac_list_size(list);
  argv = (char **)calloc(sizeof(char *), argc+1);
  for (i=0; i<argc; i++) {
    item = ac_list_nth(list, i);
    argv[i] = (char *)calloc(strlen(item)+1, 1);
    strcpy(argv[i], item);
  }
  argv[argc] = (char *)NULL;
  return argv;
}

/*
 * The items in data must be seperated by a blank space.
 * (e.g. "cat ten pen")
 */
char **
StringToArgv (data)
     char	*data;		/* Must be NULL-terminated string.	*/
{
  int argc;
  char **argv;
  char *cp;
  char *ep;
  char *wp;
  int start;
  int i;


  if (data == NULL)
    return (char **)NULL;
  argc = 0;
  cp = data;
  ep = data + strlen(data);
  while (cp < ep) {
    while ((cp < ep) && isspace(*cp))
      cp++;
    start = 0;
    while ((cp < ep) && !isspace(*cp)) {
      cp++;
      start = 1;
    }
    if (start)
      argc++;
  }
  
  /*
   * Extract all the items in string data.
   */
  argv = (char **)calloc(sizeof(char *), argc+1);
  argv[argc] = (char *)NULL;
  if (argc == 0)
    return argv;
  
  i = 0;
  cp = data;
  ep = data + strlen(data);
  while (cp < ep) {
    while ((cp < ep) && isspace(*cp))
      cp++;
    wp = cp;
    while ((cp < ep) && !isspace(*cp)) 
      cp++;
    if (cp > wp) {
      argv[i] = (char *)calloc(cp-wp+1, 1);
      strncpy(argv[i], wp, cp-wp);
      i++;
    }
  }
  argv[argc] = (char *)NULL;
  return argv;
}


void
FreeArgv(argv)
     char	**argv;
{
  int i;
  if (argv == (char **)NULL)
    return;
  for (i=0; argv[i]!=NULL; i++)
    (void) free((char *)argv[i]);
  (void) free((char *)argv);
}

void
PrintArgv (argv)
     char	**argv;
{
  int i;
  if (argv == (char **)NULL)
    return;
  for(i=0; argv[i]!=NULL; i++)
    printf("%s\n", argv[i]);
}

int
SizeArgv (argv)
     char	**argv;
{
  int i;
  if (argv == (char **)NULL)
    return 0;
  for (i=0; argv[i]!=NULL; i++)
    ;
  return i;
} 

char *
NthArgv (argv, n)
     char	**argv;
     int	n;
{
  if (argv == (char **)NULL)
    return (char *)NULL;
  if ((n < 0) || (n > SizeArgv(argv)))
    return (char *)NULL;
  return (argv[n]);
}


/*
 * return 1 if found, 0 otherwise.
 */
int
SearchArgv (argv, item)
     char	**argv;
     char	*item;
{
  int i;
  if ((argv == (char **)NULL) || (item == NULL))
    return 0;
  for (i=0; argv[i]!=NULL; i++)
    if (strcmp(argv[i], item) == 0)
      return 1;
  return 0;
} 






