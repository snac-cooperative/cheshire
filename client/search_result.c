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
#include <assert.h>
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

extern NamePlusRecord *DupNamePlusRecord (NamePlusRecord *);
extern DiagRec *DupDiagRec (DiagRec *);
extern ObjectIdentifier DupOID (ObjectIdentifier);
extern OctetString DupOstring (OctetString);
extern BitString DupBstring (BitString);
extern InternationalString DupInternationalString(InternationalString);
extern any DupASN1type(DOCTYPE, any) ;
extern External *DupExternal (External *);
extern OPACRecord *DupOPACRecord(OPACRecord *);
extern Explain_Record *DupExplainRecord(Explain_Record *);
extern GenericRecord * DupGRS1Record(GenericRecord *);
extern TaskPackage *DupESRecord(TaskPackage *);
extern BriefBib *DupSummaryRecord(BriefBib *);
extern DefaultDiagFormat *DupNonSurrogateDiagnostic (DefaultDiagFormat *);
extern void PrintNamePlusRecord (NamePlusRecord *);
extern void PrintOctetString (FILE *, char *, OctetString);
extern void PrintMarc (FILE *, char *);
extern void PrintMarcTitle (FILE *, char *);
extern void PrintMarcLong (FILE *, char *);
extern void PrintDiagRec(FILE *, DefaultDiagFormat *);
void FreeDefaultDiagRec (DefaultDiagFormat *);
void FreeNonSurrogateDiagnostic (DefaultDiagFormat *);
char *GetDataExternal (External *, DOCTYPE, int *);
extern DOCTYPE GetTypeExternal (External *); /* moved to zpdu/utils.c */
int UpdateDocumentList(ZSESSION *, Records *, int, int, int);


void
PrintDocumentList (session)
     ZSESSION *session;
{
  DOCUMENT *document;
  int i;
  printf("resultCount:	%d\n", session->resultCount);
  printf("noRecordsReturned:	%d\n", session->noRecordsReturned);
  printf("nextResultSetPosition: %d\n", session->nextResultSetPosition);
  if (session->resultSetName)
    printf("resultSetName: 	%s\n", session->resultSetName);
  for (i=1; i < session->listSize; i++) {
    document = session->documentList[i];
    if (document != NULL) {
      printf("record: %d\n", i);
      if (document->dtype == Z_US_MARC) {
	if (document->data)
	  PrintMarc (stdout, document->data);
      } else if (document->dtype == Z_DIAG_REC) {
	PrintDiagRec(stdout,document->data);
      } else {
	fprintf(stderr, "unknown document type.\n");
      }
    }
  }
}

int
PrintDocument (fptr, session, documentNumber, format)
     FILE *fptr;
     ZSESSION *session;
     int documentNumber;
     char 	*format;
{
  DOCUMENT *document;
  
  if (session == NULL)
    return Z_ERROR;
  if (!fptr)
    return Z_ERROR;
  
  if (documentNumber > session->resultCount) {
    fprintf(fptr, "%05d out of range.\n", documentNumber);
    return Z_OK;
  }
  if (strcasecmp(format, "LONG") == 0)
    fprintf(fptr, "%-5d\n", documentNumber);
  else if (strcasecmp(format, "SHORT") == 0)
    fprintf(fptr, "%-5d ", documentNumber);
  document = session->documentList[documentNumber];
  if (document == NULL) {
    fprintf(stderr, "Not available: %d\n", documentNumber);
    return Z_OK;
  }
  if (document->dtype == Z_US_MARC) {
    if (document->data) {
      if (strcasecmp(format, "MARC") == 0)
	PrintMarc (fptr, document->data);
      else if (strcasecmp(format, "LONG") == 0)
	PrintMarcLong (fptr, document->data);
      else
	PrintMarcTitle (fptr, document->data);
    }
  } else if (document->dtype == Z_DIAG_REC) {
    if (document->data)
      PrintDiagRec(stdout,document->data);
  } else {
    fprintf(stderr, "unknown document type.\n");
  }
  return Z_OK;
}


int
ProcessSearchResponse (session, searchResponse)
     ZSESSION *session;
     SearchResponse	*searchResponse;
{
  Records *records;
  
  if (!searchResponse)
    return Z_ERROR;

  /* with any search response we should reset a few things...*/
  session->nextResultSetPosition = 1;
  session->startPosition = 1;


  session->searchStatus = searchResponse->searchStatus;
  if (session->searchStatus == 0) {		/* Failure.	*/
    session->resultSetStatus = searchResponse->resultSetStatus;
    if (session->diagRec != (DefaultDiagFormat *)NULL)
      (void )FreeNonSurrogateDiagnostic (session->diagRec);	
    /* Watch out here. Some systems don't return a diagnostic record.
     */
    if (searchResponse->records != NULL) {
      if (searchResponse->records->which == e10_nonSurrogateDiagnostic)
	session->diagRec = DupNonSurrogateDiagnostic (searchResponse->records->u.nonSurrogateDiagnostic);
      else {
	if (searchResponse->records->which == e10_multipleNonSurDiagnostics) {
	  if (searchResponse->records->u.multipleNonSurDiagnostics != NULL
	      && searchResponse->records->u.multipleNonSurDiagnostics->item != NULL
	      && searchResponse->records->u.multipleNonSurDiagnostics->item->which == e13_defaultFormat)
	    session->diagRec =  
	      DupNonSurrogateDiagnostic(searchResponse->records->u.multipleNonSurDiagnostics->item->u.defaultFormat);
	  else {
	    session->diagRec = NULL;
	  }
	}
      }
      session->diagRecPresent = 1;
    }
    return Z_OK;
  }
  /* Success.	*/
  session->diagRecPresent = 0;
  session->presentStatus = searchResponse->presentStatus;
  session->resultCount = searchResponse->resultCount;
  session->noRecordsReturned = searchResponse->numberOfRecordsReturned;
  
  /*
   * Some systems may not return any records in the search response 
   * message.
   */
  if (searchResponse->numberOfRecordsReturned > 0) {
    records = searchResponse->records;
    UpdateDocumentList (session, records, 1, 
			searchResponse->numberOfRecordsReturned, 
			searchResponse->nextResultSetPosition);
  }
  return Z_OK;
}

DOCUMENT *
NewDocument (npr, record_id)
     NamePlusRecord *npr;
     int record_id; /* id number in result set of this record */
{
  DOCUMENT *document;
  int data_size = 0;

  if (!npr) 
    return NULL;
  document = CALLOC(DOCUMENT,1);
  if (!document)
    return NULL;
  CopyOctetString (document->databaseName, npr->name);
  if (npr->record.which == e11_retrievalRecord) {
    document->dtype = GetTypeExternal(npr->record.u.retrievalRecord);
    document->data = GetDataExternal(npr->record.u.retrievalRecord,
				     document->dtype, &data_size);
    document->dsize = data_size;
    document->docid = record_id;
  } 
  
  return document;
}

int
UpdateDocumentList(session, records, startPosition, numberOfRecordsReturned, 
		   nextPosition)
     ZSESSION *session;
     Records	*records;
     int startPosition;
     int numberOfRecordsReturned;	/* number of records returned.	*/
     int nextPosition;			/* next result set position.	*/
{
  struct responseRecords_List6 *tmp;
  int start;
  int recid;
  int i;
  
  if (session == NULL)
    return Z_ERROR;
  if (records == (Records *)NULL)
    return Z_OK;
  
  switch (records->which) {
  case e10_responseRecords:
    
    if (session->resultCount > 0 
	|| (session->resultCount == 0 && numberOfRecordsReturned > 0)) {
      if (!session->documentList) {		/* has not allocated space. */	
	session->documentList = 
	  (DOCUMENT **)calloc(numberOfRecordsReturned+1, 
			      sizeof(DOCUMENT *));
	session->listSize = numberOfRecordsReturned+1;
	session->listUse = 0;
      }
      
      if (numberOfRecordsReturned > session->listSize) {
	session->documentList = 
	  (DOCUMENT **) realloc(session->documentList,
				(numberOfRecordsReturned+1)
				*sizeof(DOCUMENT *));
	session->listSize = numberOfRecordsReturned+1;
	/* initialize these to nulls */
	for (i = 0; i < session->listSize; i++)
	  session->documentList[i] = NULL;
      }
      
      if (!session->documentList) {
	fprintf(stderr, "calloc: not enough memory.\n");
	return Z_ERROR;
      }
    }


    start = 1;
    recid = startPosition;
    tmp = records->u.responseRecords;
    for (; tmp!=NULL; tmp=tmp->next) {
      session->documentList[start] = NewDocument(tmp->item, recid);
      start++;
      recid++;
    }
    break;
    
  case e10_nonSurrogateDiagnostic:
    session->diagRec = DupNonSurrogateDiagnostic(records->u.nonSurrogateDiagnostic);
    break;
    
  default:
    /* print out an error message.	*/
    printf("unknown record type.\n");
    break;
  }
  session->noRecordsReturned = numberOfRecordsReturned;
  session->nextResultSetPosition = nextPosition;
  /* session->nextResultSetPosition = start; */
  session->totalNumberRecordsReturned += numberOfRecordsReturned;
  return Z_OK;
}


int
ProcessPresentResponse (session, pr)
     ZSESSION *session;
     PresentResponse	*pr;
{
  
  if ((session == NULL) || (pr == NULL))
    return Z_ERROR;
  
  session->presentStatus = pr->presentStatus;
  if (pr->presentStatus == 5) {	/* Failure.	*/
    if (session->diagRec != (DefaultDiagFormat *)NULL)
      (void )FreeNonSurrogateDiagnostic (session->diagRec);	
    if (pr->records) {
      session->diagRec = 
	DupNonSurrogateDiagnostic(pr->records->u.nonSurrogateDiagnostic);
    }
    session->diagRecPresent = 1;
    return Z_ERROR;

  } else {				/* Success.	*/
    session->diagRecPresent = 0;
    UpdateDocumentList (session, pr->records, session->startPosition,
			pr->numberOfRecordsReturned, pr->nextResultSetPosition);
  }
  return Z_OK;
}


int
ProcessDeleteResultSetResponse (session, pr)
     ZSESSION *session;
     DeleteResultSetResponse	*pr;
{
  RESULT_SET *curr, *tmp, *prev;
  char *resultSetName;
  int i;
  int match=0;
  
  if ((session == NULL) || (pr == NULL))
    return Z_ERROR;
  if ((pr->deleteOperationStatus == 3) ||
      (pr->deleteOperationStatus == 4) ||
      (pr->deleteOperationStatus == 5) ||
      (pr->deleteOperationStatus == 6)) {
    printf("Result sets requested are not deleted.\n");
    return Z_OK;
  }
  
  if (session->deleteAllResultSets) {		/* bulk delete	*/
    if (pr->deleteOperationStatus == 0) {	/* sucesses	*/
      printf("all result sets have been deleted successfully.\n");
      for (curr=session->s_resultSets; curr!=NULL; ) {
	tmp = curr;
	curr = curr->next;
	(void) free ((char *)tmp);
      }
    } else if (pr->deleteOperationStatus == 7) {	
      printf("Sorry the server doesn't support bulk deletion.\n");
    } else if (pr->deleteOperationStatus == 8) {
      if (pr->numberNotDeleted) {
	printf("%d result sets not deleted.\n", pr->numberNotDeleted);
      }
      /* walk through the buldStatuses list to see which result set
       * has been removed, which hasn't been deleted.
       */
      /* TO BE IMPLEMENTED.	*/
    }
  } else {
    if (pr->deleteOperationStatus == 0) {	/* successes	*/
      for (i=0; i<session->numberResultSets; i++) {
	resultSetName =  session->resultSetList[i]; 
	match = 0;
	for (curr=session->s_resultSets, prev=curr; curr!=NULL;
	     prev=curr, curr=curr->next) {
	  if (strcmp(resultSetName, curr->resultSetName) == 0) {
	    match = 1;
	    break;
	  }
	}
	if (match == 1) {
	  if (curr == session->s_resultSets) {
	    tmp = curr;
	    session->s_resultSets = curr->next;
	    (void) free((char *)tmp);
	  } else {
	    prev->next = curr->next;
	    (void) free ((char *)curr);
	  }
	}
      }	
    } else if (pr->deleteOperationStatus == 9) {
      /* walk through the deleteListStatuses */
      /* TO BE IMPLEMENTED.	*/
    } 
  }
  
  /* print out the delete message if one is returned.
   */
  PrintOctetString (stdout,"Delete message:",pr->deleteMessage);
  return Z_OK;
}

