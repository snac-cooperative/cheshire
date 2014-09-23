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
#include "z3950_3.h"
#include "dmalloc.h"

/* don't really need these if using the garbage collector */
void FreeOctetString (OctetString);
void FreeBitString (BitString);
void FreeInternationalString (InternationalString);
void FreeAny (any);
void FreeExternal (External *);
void FreeSearchRequest (SearchRequest *);

void FreeOid (ObjectIdentifier);
void FreeInitializeRequest(InitializeRequest       *initrequest);
void FreeInitializeResponse(InitializeResponse      *initResponse);
void FreeSearchRequest(SearchRequest   *searchRequest);
void FreeSearchResponse(SearchResponse  *searchResponse);
void FreePresentRequest(PresentRequest  *presentRequest);
void FreePresentResponse(PresentResponse *presentResponse);
void FreeDeleteResultSetRequest(DeleteResultSetRequest  *deleteResultSetRequest);
void FreeDeleteResultSetResponse(DeleteResultSetResponse *deleteResultSetResponse);
void FreeAccessControlRequest(AccessControlRequest    *accessControlRequest);
void FreeAccessControlResponse(AccessControlResponse    *accessControlResponse);
void FreeResourceControlRequest(ResourceControlRequest  *resourceControlRequest);
void FreeResourceControlResponse(ResourceControlResponse *resourceControlResponse);
void FreeResourceReportRequest(ResourceReportRequest *);
void FreeResourceReportResponse(ResourceReportResponse  *rrr);
void FreeResourceReport (ResourceReport  *rr);
void FreeTriggerResourceControlRequest(TriggerResourceControlRequest *trcr);
void FreeQuery (Query   *query);
void FreeRPNQuery (RPNQuery        *rpn);
void FreeRPNStructure (RPNStructure    *rpns);
void FreeOperator (Operator        *op);
void FreeOperand (Operand *operand);
void FreeAttributeList (AttributeList   *al);
void FreeAttributeElement (AttributeElement   *ae);
void FreeRecords (Records *records);
void FreeDiagRec (DiagRec *dr);
void FreeNamePlusRecord (NamePlusRecord *npr);
void FreeScanRequest(ScanRequest *sr);
void FreeScanResponse(ScanResponse *sr);
void FreePDU(PDU     *pdu);
void FreeElementSetNames (ElementSetNames *);
void FreeAttributesPlusTerm (AttributesPlusTerm *apt);
void FreeListStatuses (ListStatuses *ls);
void FreeResultSetPlusAttr(struct ResultSetPlusAttributes *resAttr);
void FreeDefaultDiagRec(DefaultDiagFormat *ddr);
void FreeSortRequest(SortRequest *s);
void FreeSortResponse(SortResponse *s);
void FreeScanResponse(ScanResponse *s);
void FreeScanRequest(ScanRequest *s);
void FreeClose(Close *cl);
void FreeUnit(Unit *u);
void FreeStringOrNumeric(StringOrNumeric *se);
void FreeIntUnit(IntUnit *iu);
void FreeSegment(Segment *s);
void FreeExtendedServicesRequest(ExtendedServicesRequest *r);
void FreeExtendedServicesResponse(ExtendedServicesResponse *r);
void FreeOtherInformation(OtherInformation *);
void FreeTerm(Term *);
void FreeSpecification(Specification *);
/* OPAC record syntax in External items   */
void FreeOPACRecord(OPACRecord *);     
/* EXPLAIN record syntax  */
void FreeExplain_Record(Explain_Record *data);
/* Generic record syntax 1*/
void FreeGenericRecord(GenericRecord *data);
/* Extended Services      */
void FreeTaskPackage(TaskPackage *data);
/* Summary (brief bib) record syntax  */
void FreeBriefBib(BriefBib *data);


void
FreePDU(pdu)
     PDU	*pdu;
{
  if (pdu == NULL)
    return;
  switch(pdu->which) {
  case e1_initRequest: 
    (void) FreeInitializeRequest(pdu->u.initRequest);
    break;
  case e1_initResponse: 
    (void) FreeInitializeResponse(pdu->u.initResponse);
    break;
  case e1_searchRequest: 
    (void) FreeSearchRequest(pdu->u.searchRequest);
    break;
  case e1_searchResponse:
    (void) FreeSearchResponse(pdu->u.searchResponse);
    break;
  case e1_presentRequest:
    (void) FreePresentRequest(pdu->u.presentRequest);
    break;
  case e1_presentResponse:
    (void) FreePresentResponse(pdu->u.presentResponse);
    break;
  case e1_deleteResultSetRequest:
    (void) FreeDeleteResultSetRequest(pdu->u.deleteResultSetRequest);
    break;
  case e1_deleteResultSetResponse:
    (void) FreeDeleteResultSetResponse(pdu->u.deleteResultSetResponse);
    break;
  case e1_accessControlRequest:
    (void) FreeAccessControlRequest(pdu->u.accessControlRequest);
    break;
  case e1_accessControlResponse:
    (void) FreeAccessControlResponse(pdu->u.accessControlResponse);
    break;
  case e1_resourceControlRequest:
    (void) FreeResourceControlRequest(pdu->u.resourceControlRequest);
    break;
  case e1_resourceControlResponse:
    (void) FreeResourceControlResponse(pdu->u.resourceControlResponse);
    break;
  case e1_triggerResourceControlRequest:
    (void) FreeTriggerResourceControlRequest(pdu->u.triggerResourceControlRequest);
    break;
  case e1_resourceReportRequest:
    (void) FreeResourceReportRequest(pdu->u.resourceReportRequest);
    break;
  case e1_resourceReportResponse:
    (void) FreeResourceReportResponse(pdu->u.resourceReportResponse);
    break;
  case e1_scanRequest:
    (void) FreeScanRequest(pdu->u.scanRequest);
    break;
  case e1_scanResponse: 
    (void) FreeScanResponse(pdu->u.scanResponse);
    break;
  case e1_sortRequest:
    (void) FreeSortRequest(pdu->u.sortRequest);
    break;
  case e1_sortResponse:
    (void) FreeSortResponse(pdu->u.sortResponse);
    break;
  case e1_segmentRequest:
    (void) FreeSegment(pdu->u.segmentRequest);
    break;
  case e1_extendedServicesRequest:
    (void) FreeExtendedServicesRequest(pdu->u.extendedServicesRequest);
    break;
  case e1_extendedServicesResponse:
    (void) FreeExtendedServicesResponse(pdu->u.extendedServicesResponse);
    break;
  case e1_close:
    (void) FreeClose(pdu->u.close);
    break;
    
    
  default:
    break;
  }
  FREE ((char *)pdu);
}


void
FreeInitializeRequest(InitializeRequest	*initRequest)
{
  if (initRequest == NULL)
    return;
  FreeOctetString(initRequest->referenceId);
  FreeBitString(initRequest->protocolVersion);
  FreeBitString(initRequest->options);
  FreeAny(initRequest->idAuthentication);
  FreeOctetString(initRequest->implementationId);
  FreeOctetString(initRequest->implementationName);
  FreeOctetString(initRequest->implementationVersion);
  FreeExternal(initRequest->userInformationField);
  FREE ((char *)initRequest);
}

void
FreeInitializeResponse(initResponse)
     InitializeResponse	*initResponse;
{
  if (initResponse == NULL)
    return;
  FreeOctetString(initResponse->referenceId);
  FreeBitString(initResponse->protocolVersion);
  FreeBitString(initResponse->options);
  FreeOctetString(initResponse->implementationId);
  FreeOctetString(initResponse->implementationName);
  FreeOctetString(initResponse->implementationVersion);
  FreeExternal(initResponse->userInformationField);
  (void) ((char *)initResponse);
}


void
FreeSearchRequest(searchRequest)
     SearchRequest	*searchRequest;
{
  struct databaseNames_List1	*curr, *next;
  if (searchRequest == NULL)
    return;
  FreeOctetString(searchRequest->referenceId);
  FreeOctetString(searchRequest->resultSetName);
  for (curr=searchRequest->databaseNames; curr!=NULL;curr=next) {
    next = curr->next;
    FreeOctetString (curr->item);
    FREE ((char *)curr);
  }
  FreeElementSetNames(searchRequest->smallSetElementSetNames);
  FreeElementSetNames(searchRequest->mediumSetElementSetNames);
  FreeOid(searchRequest->preferredRecordSyntax);
  FreeQuery(searchRequest->query);
  FREE ((char *)searchRequest);
}

void
FreeSearchResponse(searchResponse)
     SearchResponse	*searchResponse;
{
  if (searchResponse == NULL)
    return;
  FreeOctetString(searchResponse->referenceId);
  FreeRecords(searchResponse->records);
  FREE ((char *)searchResponse);
}


void FreeRange(range)
     Range *range;
{
  if (range == NULL)
    return;

  FREE(range);
}

void FreeAdditionalRanges(range)
     struct additionalRanges_List4  *range;
{
  if (range == NULL)
    return;

  if (range->next) 
    FreeAdditionalRanges(range->next);
  FreeRange(range->item);
}

void FreeRecordComposition(recordComposition)
     struct RecordComposition *recordComposition;
{
  if (recordComposition == NULL) 
    return;

  if (recordComposition->which == e9_complex) {
    /* DO NOTHING RIGHT NOW -- WILL NEED TO FIX THIS */
  }
  else 
    FreeElementSetNames(recordComposition->u.simple);
}

void
FreePresentRequest(presentRequest)
     PresentRequest	*presentRequest;
{
  if (presentRequest == NULL)
    return;
  FreeOctetString(presentRequest->referenceId);
  FreeOctetString(presentRequest->resultSetId);
  FreeAdditionalRanges(presentRequest->additionalRanges);
  FreeRecordComposition(presentRequest->recordComposition);
  FreeOid(presentRequest->preferredRecordSyntax);
  FREE ((char *)presentRequest);
}

void
FreePresentResponse(presentResponse)
     PresentResponse	*presentResponse;
{
  if (presentResponse == NULL)
    return;
  FreeOctetString(presentResponse->referenceId);
  FreeRecords(presentResponse->records);
  FreeOtherInformation(presentResponse->otherInfo);
  FREE ((char *)presentResponse);
}


void
FreeDeleteResultSetRequest(deleteResultSetRequest)
     DeleteResultSetRequest	*deleteResultSetRequest;
{
  struct resultSetList_List11	*curr, *next;
  if (deleteResultSetRequest == NULL)
    return;
  FreeOctetString(deleteResultSetRequest->referenceId);
  for (curr=deleteResultSetRequest->resultSetList; curr!=NULL; curr=next) {
    next = curr->next;
    FreeOctetString (curr->item);
    FREE ((char *)curr);
  }
  FREE ((char *)deleteResultSetRequest);
}

void
FreeDeleteResultSetResponse(deleteResultSetResponse)
     DeleteResultSetResponse	*deleteResultSetResponse;
{
  if (deleteResultSetResponse == NULL)
    return;
  FreeOctetString(deleteResultSetResponse->referenceId);
  FreeListStatuses(deleteResultSetResponse->deleteListStatuses);
  FreeListStatuses(deleteResultSetResponse->bulkStatuses);
  FreeOctetString(deleteResultSetResponse->deleteMessage);
  FREE ((char *)deleteResultSetResponse);
}

void
FreeSecurityChallenge(securityChallenge)
     struct SecurityChallenge *securityChallenge;
{
  if (securityChallenge == NULL) return;

  if (securityChallenge->which == e17_simpleForm)
    FreeOctetString(securityChallenge->u.simpleForm); 
  else
    FreeExternal(securityChallenge->u.externallyDefined);
  /* don't free securityChallenge -- it is part of accesscontrolrequest */
}
void
FreeAccessControlRequest(accessControlRequest)
     AccessControlRequest	*accessControlRequest;
{
  if (accessControlRequest == NULL)
    return;
  FreeOctetString(accessControlRequest->referenceId);
  FreeSecurityChallenge(&accessControlRequest->securityChallenge);
  FREE ((char *)accessControlRequest);
}

void
FreeSecurityChallengeResponse(securityChallengeResponse)
     struct SecurityChallengeResponse *securityChallengeResponse;
{
  if (securityChallengeResponse == NULL) return;

  if (securityChallengeResponse->which == e18_simpleForm)
    FreeOctetString(securityChallengeResponse->u.simpleForm); 
  else
    FreeExternal(securityChallengeResponse->u.externallyDefined);
  FREE (securityChallengeResponse);
}

void
FreeAccessControlResponse(accessControlResponse)
     AccessControlResponse    *accessControlResponse;
{
  if (accessControlResponse == NULL)
    return;
  FreeOctetString(accessControlResponse->referenceId);
  FreeSecurityChallengeResponse(
		  &accessControlResponse->securityChallengeResponse);
  FREE ((char *)accessControlResponse);
}

void
FreeResourceControlRequest(resourceControlRequest)
     ResourceControlRequest	*resourceControlRequest;
{
  if (resourceControlRequest == NULL)
    return;
  FreeOctetString(resourceControlRequest->referenceId);
  FreeResourceReport(resourceControlRequest->resourceReport);
  FREE ((char *)resourceControlRequest);
}

void
FreeResourceControlResponse(resourceControlResponse)
     ResourceControlResponse	*resourceControlResponse;
{
  if (resourceControlResponse == NULL)
    return;
  FreeOctetString(resourceControlResponse->referenceId);
  FREE ((char *)resourceControlResponse);
}


void
FreeTriggerResourceControlRequest(TriggerResourceControlRequest *trcr)
{
  if (trcr == NULL)
    return;
  FreeOctetString (trcr->referenceId);
  FreeOid (trcr->prefResourceReportFormat);
  FREE ((char *)trcr);
}

void
FreeResourceReportRequest(ResourceReportRequest *rrr)
{
  if (rrr == NULL)
    return;
  FreeOctetString (rrr->referenceId);
  FreeOid (rrr->prefResourceReportFormat);
  FREE ((char *)rrr);
}


void
FreeResourceReportResponse(rrr)
     ResourceReportResponse	*rrr;
{
  if (rrr == NULL)
    return;
  FreeOctetString(rrr->referenceId);
  FreeResourceReport(rrr->resourceReport);
  FREE ((char *)rrr);
}

void 
FreeResourceReport (rr)
     ResourceReport	*rr;
{
  struct estimates_List96  *curr, *next;
  
  if (rr == NULL)
    return;
  FreeOctetString (rr->message);
  for (curr=rr->estimates; curr != NULL; curr=next) {
    next = curr->next;
    FREE ((char *)curr->item);
    FREE ((char *)curr);
  }
  FREE ((char *)rr);
}

void
FreeQuery (query)
     Query	*query;
{
  if (query == NULL)
    return;
  switch (query->which) {
  case e2_type_0:
    FreeAny (query->u.type_0);
    break;
  case e2_type_1:
    FreeRPNQuery (query->u.type_1);
    break;
  case e2_type_2:
    FreeOctetString (query->u.type_2);
    break;
  case e2_type_100:
    FreeOctetString (query->u.type_100);
    break;
  case e2_type_101:
    FreeRPNQuery (query->u.type_101);
    break;
  case e2_type_102:
    FreeOctetString (query->u.type_102);
    break;
  default:
    break;
  }
  FREE ((char *)query);
}

void
FreeRPNQuery (rpn)
     RPNQuery	*rpn;
{
  if (rpn==NULL)
    return;
  FreeOid (rpn->attributeSet);
  FreeRPNStructure (rpn->rpn);
  FREE ((char *)rpn);
}

void
FreeRPNStructure (rpns)
     RPNStructure	*rpns;
{
  if (rpns == NULL)
    return;
  switch (rpns->which) {
  case e3_op:
    FreeOperand (rpns->u.op);
    break;
  case e3_rpnRpnOp:
    FreeRPNStructure (rpns->u.rpnRpnOp.rpn1);
    FreeRPNStructure (rpns->u.rpnRpnOp.rpn2);
    FreeOperator (rpns->u.rpnRpnOp.op);
    break;
  default:
    break;
  }
  FREE ((char *)rpns);
}

void
FreeOperator (op)
     Operator	*op;
{
  if (op == NULL)
    return;
  FREE ((char *)op);
}

void
FreeOperand (operand)
     Operand	*operand;
{
  if (operand == NULL)
    return;
  switch (operand->which) {
  case e4_attrTerm:
    FreeAttributesPlusTerm (operand->u.attrTerm);
    break;
  case e4_resultSet:
    FreeOctetString (operand->u.resultSet);
    break;
  case e4_resultAttr:
    FreeResultSetPlusAttr(operand->u.resultAttr);
    break;
  default:
    break;
  }
  FREE ((char *)operand);
}

void
FreeAttributeElement (AttributeElement	*ae) {
  if (ae == NULL)
    return;
  if (ae->attributeSet != NULL) {
    FreeOid(ae->attributeSet);
  }

  FREE(ae);
  
}


void 
FreeAttributesPlusTerm (AttributesPlusTerm *apt)
{
  void FreeTerm();

  if (apt == NULL)
    return;
  FreeTerm(apt->term);
  FreeAttributeList (apt->attributes);
  FREE ((char *)apt);
}

void
FreeAttributeList (AttributeList	*al) {
  AttributeList   *curr, *next;
  if (al == NULL)
    return;
  for (curr=al; curr!=NULL; curr=next) {
    next = curr->next;
    FreeAttributeElement(curr->item); 
    FREE ((char *)curr);
  }
}

void
FreeResultSetPlusAttr(resAttr)
     struct ResultSetPlusAttributes *resAttr;
{
  
  FreeOctetString (resAttr->resultSet);
  FreeAttributeList (resAttr->attributes);
  FREE ((char *)resAttr);
}



void
FreeRecords (records)
     Records	*records;
{
  struct responseRecords_List6 	*curr, *next;
  struct multipleNonSurDiagnostics_List7 *curr2, *next2;

  if (records == NULL)
    return;
  switch (records->which) {

  case  e10_responseRecords:
    for (curr=records->u.responseRecords; curr!=NULL;curr=next) {
      next = curr->next;
      FreeNamePlusRecord (curr->item);
      FREE ((char *)curr);
    }
    break;
    
  case e10_nonSurrogateDiagnostic:
    FreeDefaultDiagRec (records->u.nonSurrogateDiagnostic);
    break;

  case e10_multipleNonSurDiagnostics:
    for (curr2=records->u.multipleNonSurDiagnostics; curr2!=NULL;curr2=next2) {
      next2 = curr2->next;
      FreeDiagRec (curr2->item);
      FREE ((char *)curr2);
    }
    break;

  default:
    break;
  }
  FREE ((char *)records);
}


void
FreeDiagRec (dr)
     DiagRec	*dr;
{
  if (dr == NULL)
    return;
  
  if (dr->which == e13_defaultFormat)
    FreeDefaultDiagRec(dr->u.defaultFormat);
  else
    FreeExternal(dr->u.externallyDefined);

  FREE ((char *)dr);
}

void
FreeDefaultDiagRec(ddr)
     DefaultDiagFormat *ddr;
{
  if (ddr == NULL)
    return;

  FreeOid(ddr->diagnosticSetId);
  if (ddr->addinfo.which == e14_v2Addinfo)
    FreeOctetString(ddr->addinfo.u.v2Addinfo);
  else
    FreeInternationalString(ddr->addinfo.u.v3Addinfo);
  FREE ((char *) ddr);
}

void
FreeFragment(fr)
     FragmentSyntax *fr;
{
  if (fr == NULL)
    return;

  if (fr->which == e12_notExternallyTagged)
    FreeOctetString(fr->u.notExternallyTagged);
  else
    FreeExternal(fr->u.externallyTagged);

  FREE ((char *) fr);
}
	    
void
FreeNamePlusRecord (npr)
     NamePlusRecord *npr;
{
  if (npr == NULL)
    return;
  FreeOctetString (npr->name);
  switch (npr->record.which) {
  case e11_retrievalRecord:
    FreeExternal (npr->record.u.retrievalRecord);
    break;
  case e11_surrogateDiagnostic:
    FreeDiagRec (npr->record.u.surrogateDiagnostic);
    break;
  case e11_startingFragment:
    FreeFragment(npr->record.u.startingFragment);
    break;
  case e11_intermediateFragment:
    FreeFragment(npr->record.u.intermediateFragment);
    break;
  case e11_finalFragment:
    FreeFragment(npr->record.u.finalFragment);
    break;

  default:
    break;
  }
  FREE ((char *)npr);
} 

void 
FreeScanRequest(sr)
     ScanRequest *sr;
{
  struct databaseNames_List12 	*curr, *next;
  
  if (sr == NULL)
    return;

  FreeOctetString(sr->referenceId);

  for (curr=sr->databaseNames; curr!=NULL;curr=next) {
    next = curr->next;
    FreeOctetString(curr->item);
    FREE ((char *)curr);
  }

  FreeOid(sr->attributeSet);
  FreeAttributesPlusTerm(sr->termListAndStartPoint);
  FreeOtherInformation(sr->otherInfo);
  FREE ((char *)sr);
}

void
FreeOccurrenceByAttr(occ)
     OccurrenceByAttributes *occ;
{
   OccurrenceByAttributes *curr, *next;

   if (occ == NULL)
     return;

  for (curr=occ; curr!=NULL;curr=next) {
    next = curr->next;

    FreeAttributeList(occ->item.attributes);

    if (curr->item.occurrences)
      if (curr->item.occurrences->which == e20_byDatabase) {
	struct byDatabase_List16 *curr2, *next2;
	for (curr2=curr->item.occurrences->u.byDatabase; 
	     curr2!=NULL;curr2=next2) {
	  next2 = curr2->next;
	  FreeOctetString(curr2->item.db);
	  FreeOtherInformation(curr2->item.otherDbInfo);
	
	}
      }
    
    FreeOtherInformation(curr->item.otherOccurInfo);
    FREE ((char *)curr);
  }
   
}

void
FreeTermInfo(ti)
     TermInfo *ti;
{
  struct alternativeTerm_List15 *curr, *next;

  if (ti == NULL)
    return;

  FreeTerm(ti->term);
  FreeInternationalString(ti->displayTerm);
  FreeAttributeList(ti->suggestedAttributes);
  
  for (curr=ti->alternativeTerm; curr!=NULL;curr=next) {
    next = curr->next;
    FreeAttributesPlusTerm(curr->item);
    FREE ((char *)curr);
  }

  FreeOccurrenceByAttr(ti->byAttributes);
  FreeOtherInformation(ti->otherTermInfo);
  FREE ((char *)ti);
}

void
FreeEntry(ent)
     Entry *ent;
{
  if (ent == NULL)
    return;

  if (ent->which == e19_termInfo)
    FreeTermInfo(ent->u.termInfo);
  else
    FreeDiagRec(ent->u.surrogateDiagnostic);
  FREE ((char *) ent);

}

void
FreeListEntries(le)
     ListEntries *le;
{
  struct entries_List13 	*curr, *next;
  struct nonsurrogateDiagnostics_List14 *curr2, *next2;

  if (le == NULL)
    return;

  for (curr=le->entries; curr!=NULL;curr=next) {
    next = curr->next;
    FreeEntry(curr->item);
    FREE ((char *)curr);
  }

  for (curr2=le->nonsurrogateDiagnostics; curr2!=NULL;curr2=next2) {
    next2 = curr2->next;
    FreeDiagRec(curr2->item);
    FREE ((char *)curr2);
  }

  FREE ((char *)le);
  
}


void 
FreeScanResponse(sr)
     ScanResponse *sr;
{
  if (sr == NULL)
    return;

  FreeOctetString(sr->referenceId);
  FreeListEntries(sr->entries);
  FreeOid(sr->attributeSet);
  FreeOtherInformation(sr->otherInfo);

  FREE ((char *)sr);

}


void
FreeElementSetNames (ElementSetNames *esn)
{
  struct databaseSpecific_List8	*curr, *next;
  if (esn == NULL)
    return;
  switch (esn->which) {
  case e15_genericElementSetName:
    FreeInternationalString (esn->u.genericElementSetName);
    break;
  case e15_databaseSpecific:
    for (curr=esn->u.databaseSpecific; curr!=NULL; curr=next) {
      next = curr->next;
      FreeInternationalString (curr->item.dbName);
      FreeInternationalString (curr->item.esn);
      FREE ((char *)curr);
    }
  }
  FREE ((char *)esn);
}

void FreeListStatuses (ListStatuses *ls)
{
  struct ListStatuses	*curr, *next;
  
  for (curr=ls; curr!=NULL; curr=next) {
    next = curr->next;
    FreeOctetString (curr->item.id);
    FREE ((char *)curr);
  }
}

void
FreeSortKey(key)
     SortKey *key;
{
  if (key == NULL)
    return;

  switch (key->which) {
  case e23_sortfield:
    FreeInternationalString(key->u.sortfield);
    break;
  case e23_elementSpec:
    FreeSpecification(key->u.elementSpec);
    break;
  case e23_sortAttributes:
    FreeOid(key->u.sortAttributes.id);
    FreeAttributeList(key->u.sortAttributes.list);
    break;
  }

  FREE ((char *)key);

}

void
FreeSortElement(se)
     SortElement *se;
{
  struct datbaseSpecific_List20	*curr, *next;
  
  if (se == NULL)
    return;
  
  if (se->which == e22_generic) 
    FreeSortKey(se->u.generic);
  else {
    for (curr=se->u.datbaseSpecific; curr!=NULL; curr=next) {
      next = curr->next;
      FreeOctetString (curr->item.databaseName);
      FreeSortKey(curr->item.dbSort);
      FREE ((char *)curr);
    }   
  }
  FREE ((char *)se);
}

void
FreeSortKeySpec(sp)
     SortKeySpec *sp;
{
  if (sp == NULL)
    return;

  FreeSortElement(sp->sortElement);

  if (sp->missingValueAction.which == e21_missingValueData) {
    FreeOctetString(sp->missingValueAction.u.missingValueData);
  }
  
  FREE ((char *) sp);
}

void 
FreeSortRequest(sr)
     SortRequest *sr;
{
  struct inputResultSetNames_List17 	*curr, *next;
  struct sortSequence_List18 *curr2, *next2;

  if (sr == NULL)
    return;

  FreeOctetString(sr->referenceId);

  for (curr=sr->inputResultSetNames; curr!=NULL;curr=next) {
    next = curr->next;
    FreeInternationalString(curr->item);
    FREE ((char *)curr);
  }
  
  FreeInternationalString(sr->sortedResultSetName);

  for (curr2=sr->sortSequence; curr2!=NULL;curr2=next2) {
    next2 = curr2->next;
    FreeSortKeySpec(curr2->item);
    FREE ((char *)curr2);
  }

  FreeOtherInformation(sr->otherInfo);
  FREE ((char *)sr);
}

void 
FreeSortResponse(sr)
     SortResponse *sr;
{
  struct diagnostics_List19	*curr, *next;

  if (sr == NULL)
    return;

  FreeOctetString(sr->referenceId);

  for (curr=sr->diagnostics; curr!=NULL;curr=next) {
    next = curr->next;
    FreeDiagRec(curr->item);
    FREE ((char *)curr);
  }

  FreeOtherInformation(sr->otherInfo);
  FREE ((char *)sr);
}


void
FreeSegment(s)
     Segment *s;
{
  struct segmentRecords_List5 *curr, *next;

  if (s == NULL)
    return;
  
  FreeOctetString(s->referenceId);
  
  for (curr=s->segmentRecords; curr!=NULL;curr=next) {
    next = curr->next;
    FreeNamePlusRecord(curr->item);
    FREE ((char *)curr);
  }

 
  FreeOtherInformation(s->otherInfo);
  FREE ((char *)s);
}

void
FreeClose(cl) 
     Close *cl;
{
  if (cl == NULL)
    return;

  FreeOctetString(cl->referenceId);
  FreeInternationalString(cl->diagnosticInformation);
  FreeOid(cl->resourceReportFormat);
  FreeResourceReport(cl->resourceReport);
  FreeOtherInformation(cl->otherInfo);
  FREE ((char *)cl);
}

void
FreePermissions(p)
Permissions *p;
{
  Permissions *curr, *next;
  struct allowableFunctions_List22 *curr2, *next2;

  if (p == NULL)
    return;

  for (curr=p; curr!=NULL;curr=next) {
    next = curr->next;
    FreeInternationalString(curr->item.userId);
    for (curr2=curr->item.allowableFunctions; curr2!=NULL;curr2=next2) {
      next2 = curr2->next;
      FREE ((char *)curr2);
      
    }    
    FREE ((char *)curr);
  }

  
}

void
FreeExtendedServicesRequest(r) 
    ExtendedServicesRequest *r;
{
  if (r == NULL)
    return;

  FreeOctetString(r->referenceId);
  FreeOid(r->packageType);
  FreeInternationalString(r->packageName);
  FreeInternationalString(r->userId);
  FreeIntUnit(r->retentionTime);
  FreePermissions(r->permissions);
  FreeInternationalString(r->description);
  FreeExternal(r->taskSpecificParameters);
  FreeInternationalString(r->elements);
  FreeOtherInformation(r->otherInfo);
  FREE ((char *)r);

}

void
FreeExtendedServicesResponse(r) 
    ExtendedServicesResponse *r;
{ 
  struct diagnostics_List21 *curr, *next;

  if (r == NULL)
    return;

  FreeOctetString(r->referenceId);

  for (curr=r->diagnostics; curr!=NULL;curr=next) {
    next = curr->next;
    FreeDiagRec(curr->item);
    FREE ((char *)curr);
  }

  FreeExternal(r->taskPackage);
  FreeOtherInformation(r->otherInfo);

  FREE ((char *)r);

}





