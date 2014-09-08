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
#include "z_parameters.h"

#define	GENERALIZEDTYPE	24

InitializeRequest *DecodeInitializeRequest(ASN_NODE *node);
InitializeResponse *DecodeInitializeResponse(ASN_NODE *node);
SearchRequest *DecodeSearchRequest(ASN_NODE *node);
SearchResponse *DecodeResponse(ASN_NODE *node);
Query *DecodeQuery(ASN_NODE *node);
RPNQuery *DecodeRPNQuery(ASN_NODE *node);
RPNStructure *DecodeRPNStructure(ASN_NODE *node);
Operand *DecodeOperand(ASN_NODE *node);
AttributesPlusTerm *DecodeAttributesPlusTerm(ASN_NODE *node);
ResultSetPlusAttributes *DecodeResultSetPlusAttributes(ASN_NODE *node);
AttributeList *DecodeAttributeList(ASN_NODE *node);
Term *DecodeTerm(ASN_NODE *node);
Operator *DecodeOperator(ASN_NODE *node);
AttributeElement *DecodeAttributeElement(ASN_NODE *node);
ProximityOperator *DecodeProximityOperator(ASN_NODE *node);
KnownProximityUnit *DecodeKnownProximityUnit(ASN_NODE *node);

PresentRequest *DecodePresentRequest(ASN_NODE *node);
PresentResponse *DecodePresentResponse(ASN_NODE *node);
Segment *DecodeSegment(ASN_NODE *node);
Records *DecodeRecords(ASN_NODE *node, int *count);
NamePlusRecord *DecodeNamePlusRecord(ASN_NODE *node);
DiagRec *DecodeDiagRec(ASN_NODE *node);
DefaultDiagFormat *DecodeDefaultDiagFormat(ASN_NODE *node);
Range *DecodeRange(ASN_NODE *node);
ElementSetNames *DecodeElementSetNames(ASN_NODE *node);
CompSpec *DecodeCompSpec(ASN_NODE *node);
Specification *DecodeSpecification(ASN_NODE *node);

DeleteResultSetRequest *DecodeDeleteResultSetRequest(ASN_NODE *node);
DeleteResultSetResponse *DecodeDeleteResultSetResponse(ASN_NODE *node);
AccessControlRequest *DecodeAccessControlRequest(ASN_NODE *node);
AccessControlResponse *DecodeAccessControlResponse(ASN_NODE *node);

ResourceControlRequest *DecodeResourceControlRequest(ASN_NODE *node);
ResourceControlResponse *DecodeResourceControlResponse(ASN_NODE *node);
TriggerResourceControlRequest *DecodeTriggerResourceControlRequest(ASN_NODE *node);
ResourceReportRequest *DecodeResourceReportRequest(ASN_NODE *node);
ResourceReportResponse *DecodeResourceReportResponse(ASN_NODE *node);

ScanRequest *DecodeScanRequest(ASN_NODE *node);
ScanResponse *DecodeScanResponse(ASN_NODE *node);

ListEntries *DecodeListEntries(ASN_NODE *node);
Entry *DecodeEntry(ASN_NODE *node);
TermInfo *DecodeTermInfo(ASN_NODE *node);
OccurrenceByAttributes *DecodeOccurrenceByAttributes(ASN_NODE *node);

InternationalString DecodeInternationalString(ASN_NODE *node);
OtherInformation *DecodeOtherInformation(ASN_NODE *node);

SortRequest *DecodeSortRequest(ASN_NODE *node);
SortResponse *DecodeSortResponse(ASN_NODE *node);
SortKeySpec *DecodeSortKeySpec(ASN_NODE *node);
SortElement *DecodeSortElement(ASN_NODE *node);
SortKey *DecodeSortKey(ASN_NODE *node);
StringOrNumeric *DecodeStringOrNumeric(ASN_NODE *node);
DefaultDiagFormat *DecodeDefaultDiagFormat(ASN_NODE *node);
InfoCategory *DecodeInfoCategory(ASN_NODE *node);
IntUnit *DecodeIntUnit(ASN_NODE *node);
Unit *DecodeUnit(ASN_NODE *node);
ExtendedServicesRequest *DecodeExtendedServicesRequest(ASN_NODE *node);
ExtendedServicesResponse *DecodeExtendedServicesResponse(ASN_NODE *node);
Permissions *DecodePermissions(ASN_NODE *node);
Close *DecodeClose(ASN_NODE *node);
Range *DecodeRange(ASN_NODE *node);
FragmentSyntax *DecodeFragmentSyntax(ASN_NODE *node);
DiagnosticFormat *DecodeDiagnosticFormat(ASN_NODE *node);
DiagFormat *DecodeDiagFormat(ASN_NODE *node);
DatabaseInfo *DecodeDatabaseInfo(ASN_NODE *node);
SchemaInfo *DecodeSchemaInfo(ASN_NODE *node);
ElementInfo *DecodeElementInfo(ASN_NODE *node);
Path *DecodePath(ASN_NODE *node);
ElementDataType *DecodeElementDataType(ASN_NODE *node);
TagSetInfo *DecodeTagSetInfo(ASN_NODE *node);
RecordSyntaxInfo *DecodeRecordSyntaxInfo(ASN_NODE *node);
AttributeSetInfo *DecodeAttributeSetInfo(ASN_NODE *node);
AttributeType *DecodeAttributeType(ASN_NODE *node);
AttributeDescription *DecodeAttributeDescription(ASN_NODE *node);
TermListInfo *DecodeTermListInfo(ASN_NODE *node);
ExtendedServicesInfo *DecodeExtendedServicesInfo(ASN_NODE *node);

Explain_Record *DecodeExplain_Record(ASN_NODE *node);
TargetInfo *DecodeTargetInfo(ASN_NODE *node);

AttributeDetails *DecodeAttributeDetails(ASN_NODE *node);
AttributeSetDetails *DecodeAttributeSetDetails(ASN_NODE *node);
AttributeTypeDetails *DecodeAttributeTypeDetails(ASN_NODE *node);
OmittedAttributeInterpretation *DecodeOmittedAttributeInterpretation(ASN_NODE *node);
AttributeValue *DecodeAttributeValue(ASN_NODE *node);
TermListDetails *DecodeTermListDetails(ASN_NODE *node);
ElementSetDetails *DecodeElementSetDetails(ASN_NODE *node);
RetrievalRecordDetails *DecodeRetrievalRecordDetails(ASN_NODE *node);
PerElementDetails *DecodePerElementDetails(ASN_NODE *node);
SortDetails *DecodeSortDetails(ASN_NODE *node);
SortKeyDetails *DecodeSortKeyDetails(ASN_NODE *node);
ProcessingInformation *DecodeProcessingInformation(ASN_NODE *node);
VariantSetInfo *DecodeVariantSetInfo(ASN_NODE *node);
VariantClass *DecodeVariantClass(ASN_NODE *node);
VariantType *DecodeVariantType(ASN_NODE *node);
VariantValue *DecodeVariantValue(ASN_NODE *node);
UnitInfo *DecodeUnitInfo(ASN_NODE *node);
UnitType *DecodeUnitType(ASN_NODE *node);
Units *DecodeUnits(ASN_NODE *node);
CategoryList *DecodeCategoryList(ASN_NODE *node);
CategoryInfo *DecodeCategoryInfo(ASN_NODE *node);
CommonInfo *DecodeCommonInfo(ASN_NODE *node);
HumanString *DecodeHumanString(ASN_NODE *node);
IconObject *DecodeIconObject(ASN_NODE *node);
ContactInfo *DecodeContactInfo(ASN_NODE *node);
NetworkAddress *DecodeNetworkAddress(ASN_NODE *node);
AccessInfo *DecodeAccessInfo(ASN_NODE *node);
QueryTypeDetails *DecodeQueryTypeDetails(ASN_NODE *node);
PrivateCapabilities *DecodePrivateCapabilities(ASN_NODE *node);
RpnCapabilities *DecodeRpnCapabilities(ASN_NODE *node);
Iso8777Capabilities *DecodeIso8777Capabilities(ASN_NODE *node);
ProximitySupport *DecodeProximitySupport(ASN_NODE *node);
SearchKey *DecodeSearchKey(ASN_NODE *node);
AccessRestrictions *DecodeAccessRestrictions(ASN_NODE *node);

Costs *DecodeCosts(ASN_NODE *node);
Charge *DecodeCharge(ASN_NODE *node);
DatabaseList *DecodeDatabaseList(ASN_NODE *node);
AttributeCombinations *DecodeAttributeCombinations(ASN_NODE *node);
AttributeOccurrence *DecodeAttributeOccurrence(ASN_NODE *node);

OPACRecord *DecodeOPACRecord(ASN_NODE *node);
HoldingsRecord *DecodeHoldingsRecord(ASN_NODE *node);
HoldingsAndCircData *DecodeHoldingsAndCircData(ASN_NODE *node);
Volume *DecodeVolume(ASN_NODE *node);
CircRecord *DecodeCircRecord(ASN_NODE *node);
BriefBib *DecodeBriefBib(ASN_NODE *node);
FormatSpec *DecodeFormatSpec(ASN_NODE *node);

GenericRecord *DecodeGenericRecord(ASN_NODE *node);
TaggedElement *DecodeTaggedElement(ASN_NODE *node);
ElementData *DecodeElementData(ASN_NODE *node);
ElementMetaData *DecodeElementMetaData(ASN_NODE *node);
TagPath *DecodeTagPath(ASN_NODE *node);
Order *DecodeOrder(ASN_NODE *node);
Usage *DecodeUsage(ASN_NODE *node);
HitVector *DecodeHitVector(ASN_NODE *node);
Variant *DecodeVariant(ASN_NODE *node);
GeneralString DecodeGeneralString(ASN_NODE *node);
VisibleString DecodeVisibleString(ASN_NODE *node);
HumanString *DecodeHumanString(ASN_NODE *node);

GeneralizedTime DecodeGeneralizedTime(ASN_NODE *node);
RecordTag *DecodeRecordTag(ASN_NODE *node);
ValueSet *DecodeValueSet(ASN_NODE *node);
ValueRange *DecodeValueRange(ASN_NODE *node);
ValueDescription *DecodeValueDescription(ASN_NODE *node);
AttributeCombinations *DecodeAttributeCombinations(ASN_NODE *node);
AttributeCombination *DecodeAttributeCombination(ASN_NODE *node);


ResourceReport *
DecodeResourceReport(node)
     ASN_NODE *node;
{
  return NULL;
}

InternationalString 
DecodeInternationalString(node)
     ASN_NODE *node;
{
  return (InternationalString)DecodeGeneralString(node);
}

InternationalString 
DecodeLanguageCode(node)
     ASN_NODE *node;
{
  return DecodeInternationalString(node);
}

ResultSetId 
DecodeResultSetId(node)
     ASN_NODE *node;
{
  return (ResultSetId)DecodeInternationalString(node);
}

ElementSetName 
DecodeElementSetName(node)
     ASN_NODE *node;
{
  return (ElementSetName)DecodeInternationalString(node);
}

DatabaseName 
DecodeDatabaseName(node)
     ASN_NODE *node;
{
  return (DatabaseName)DecodeInternationalString(node);
}

AttributeSetId 
DecodeAttributeSetId(node)
     ASN_NODE *node;
{
  return (AttributeSetId)DecodeOid(node);
}

ObjectIdentifier 
DecodeResourceReportId(node)
     ASN_NODE *node;
{
  return (ObjectIdentifier)DecodeOid(node);
}

PDU *
DecodePDU(node)
     ASN_NODE *node;
{
  int  tagno;
  PDU *pDU;
  pDU = CALLOC(PDU,1);
  if ((!node) || (!pDU))
    return (PDU *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 20:
    pDU->which = e1_initRequest;
    pDU->u.initRequest = DecodeInitializeRequest(node);
    break;
    
  case 21:
    pDU->which = e1_initResponse;
    pDU->u.initResponse = DecodeInitializeResponse(node);
    break;
    
  case 22:
    pDU->which = e1_searchRequest;
    pDU->u.searchRequest = DecodeSearchRequest(node);
    break;
    
  case 23:
    pDU->which = e1_searchResponse;
    pDU->u.searchResponse = DecodeSearchResponse(node);
    break;
    
  case 24:
    pDU->which = e1_presentRequest;
    pDU->u.presentRequest = DecodePresentRequest(node);
    break;
    
  case 25:
    pDU->which = e1_presentResponse;
    pDU->u.presentResponse = DecodePresentResponse(node);
    break;
    
  case 26:
    pDU->which = e1_deleteResultSetRequest;
    pDU->u.deleteResultSetRequest = DecodeDeleteResultSetRequest(node);
    break;
    
  case 27:
    pDU->which = e1_deleteResultSetResponse;
    pDU->u.deleteResultSetResponse = DecodeDeleteResultSetResponse(node);
    break;
    
  case 28:
    pDU->which = e1_accessControlRequest;
    pDU->u.accessControlRequest = DecodeAccessControlRequest(node);
    break;
    
  case 29:
    pDU->which = e1_accessControlResponse;
    pDU->u.accessControlResponse = DecodeAccessControlResponse(node);
    break;
    
  case 30:
    pDU->which = e1_resourceControlRequest;
    pDU->u.resourceControlRequest = DecodeResourceControlRequest(node);
    break;
    
  case 31:
    pDU->which = e1_resourceControlResponse;
    pDU->u.resourceControlResponse = DecodeResourceControlResponse(node);
    break;
    
  case 32:
    pDU->which = e1_triggerResourceControlRequest;
    pDU->u.triggerResourceControlRequest = DecodeTriggerResourceControlRequest(node);
    break;
    
  case 33:
    pDU->which = e1_resourceReportRequest;
    pDU->u.resourceReportRequest = DecodeResourceReportRequest(node);
    break;
    
  case 34:
    pDU->which = e1_resourceReportResponse;
    pDU->u.resourceReportResponse = DecodeResourceReportResponse(node);
    break;
    
  case 35:
    pDU->which = e1_scanRequest;
    pDU->u.scanRequest = DecodeScanRequest(node);
    break;
    
  case 36:
    pDU->which = e1_scanResponse;
    pDU->u.scanResponse = DecodeScanResponse(node);
    break;
    
  case 43:
    pDU->which = e1_sortRequest;
    pDU->u.sortRequest = DecodeSortRequest(node);
    break;
    
  case 44:
    pDU->which = e1_sortResponse;
    pDU->u.sortResponse = DecodeSortResponse(node);
    break;
    
  case 45:
    pDU->which = e1_segmentRequest;
    pDU->u.segmentRequest = DecodeSegment(node);
    break;
    
  case 46:
    pDU->which = e1_extendedServicesRequest;
    pDU->u.extendedServicesRequest = DecodeExtendedServicesRequest(node);
    break;
    
  case 47:
    pDU->which = e1_extendedServicesResponse;
    pDU->u.extendedServicesResponse = DecodeExtendedServicesResponse(node);
    break;
    
  case 48:
    pDU->which = e1_close;
    pDU->u.close = DecodeClose(node);
    break;
    
  }
  return pDU;
}

InitializeRequest *
DecodeInitializeRequest(node)
     ASN_NODE *node;
{
  int tagno;
  InitializeRequest *initializeRequest;
  ASN_NODE *child0;
  initializeRequest = CALLOC(InitializeRequest,1);
  if ((!node) || (!initializeRequest))
    return (InitializeRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      initializeRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      initializeRequest->protocolVersion = DecodeBitString(child0);
      break;
      
    case 4:
      initializeRequest->options = DecodeBitString(child0);
      break;
      
    case 5:
      initializeRequest->preferredMessageSize = DecodeInteger(child0);
      break;
      
    case 6:
      initializeRequest->exceptionalRecordSize = DecodeInteger(child0);
      break;
      
    case 7:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	initializeRequest->idAuthentication = DecodeAny(child1);
      }
      break;
      
    case 110:
      initializeRequest->implementationId = DecodeInternationalString(child0);
      break;
      
    case 111:
      initializeRequest->implementationName = DecodeInternationalString(child0);
      break;
      
    case 112:
      initializeRequest->implementationVersion = DecodeInternationalString(child0);
      break;
      
    case 11:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	initializeRequest->userInformationField = DecodeExternal(child1);
      }
      break;
      
    case 201:
      initializeRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return initializeRequest;
}

InitializeResponse *
DecodeInitializeResponse(node)
     ASN_NODE *node;
{
  int tagno;
  InitializeResponse *initializeResponse;
  ASN_NODE *child0;
  initializeResponse = CALLOC(InitializeResponse,1);
  if ((!node) || (!initializeResponse))
    return (InitializeResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      initializeResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      initializeResponse->protocolVersion = DecodeBitString(child0);
      break;
      
    case 4:
      initializeResponse->options = DecodeBitString(child0);
      break;
      
    case 5:
      initializeResponse->preferredMessageSize = DecodeInteger(child0);
      break;
      
    case 6:
      initializeResponse->exceptionalRecordSize = DecodeInteger(child0);
      break;
      
    case 12:
      initializeResponse->result = DecodeBoolean(child0);
      break;
      
    case 110:
      initializeResponse->implementationId = DecodeInternationalString(child0);
      break;
      
    case 111:
      initializeResponse->implementationName = DecodeInternationalString(child0);
      break;
      
    case 112:
      initializeResponse->implementationVersion = DecodeInternationalString(child0);
      break;
      
    case 11:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
        initializeResponse->userInformationField = DecodeExternal(child1);
      }
      break;
      
    case 201:
      initializeResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return initializeResponse;
}


SearchRequest *
DecodeSearchRequest(node)
     ASN_NODE *node;
{
  int tagno;
  SearchRequest *searchRequest;
  ASN_NODE *child0;
  searchRequest = CALLOC(SearchRequest,1);
  if ((!node) || (!searchRequest))
    return (SearchRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      searchRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 13:
      searchRequest->smallSetUpperBound = DecodeInteger(child0);
      break;
      
    case 14:
      searchRequest->largeSetLowerBound = DecodeInteger(child0);
      break;
      
    case 15:
      searchRequest->mediumSetPresentNumber = DecodeInteger(child0);
      break;
      
    case 16:
      searchRequest->replaceIndicator = DecodeBoolean(child0);
      break;
      
    case 17:
      searchRequest->resultSetName = DecodeInternationalString(child0);
      break;
      
    case 18:
      {
	struct databaseNames_List1 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct databaseNames_List1,1);
	  tmp->item = DecodeDatabaseName(child1);
	  tmp->next = NULL;
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	searchRequest->databaseNames = head;
      }
      break;
      
    case 100:
      {
	ASN_NODE *child1 = child0->head;
	searchRequest->smallSetElementSetNames = DecodeElementSetNames(child1);
      }
      break;
      
    case 101:
      {
	ASN_NODE *child1 = child0->head;
	searchRequest->mediumSetElementSetNames = DecodeElementSetNames(child1);
      }
      break;
      
    case 104:
      searchRequest->preferredRecordSyntax = DecodeOid(child0);
      break;
      
    case 21:
      {
	ASN_NODE *child1 = child0->head;
	searchRequest->query = DecodeQuery(child1);
      }
      break;
      
    case 203:
      searchRequest->additionalSearchInfo = DecodeOtherInformation(child0);
      break;
      
    case 201:
      searchRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return searchRequest;
}

Query *
DecodeQuery(node)
     ASN_NODE *node;
{
  int tagno;
  Query *query;
  query = CALLOC(Query,1);
  if ((!node) || (!query))
    return (Query *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    query->which = e2_type_0;
    {
      ASN_NODE *child1 = node->head;
      query->u.type_0 = DecodeAny(child1);
    }
    break;
    
  case 1:
    query->which = e2_type_1;
    query->u.type_1 = DecodeRPNQuery(node);
    break;
    
  case 2:
    query->which = e2_type_2;
    {
      ASN_NODE *child1 = node->head;
      query->u.type_2 = DecodeOctetString(child1);
    }
    break;
    
  case 100:
    query->which = e2_type_100;
    {
      ASN_NODE *child1 = node->head;
      query->u.type_100 = DecodeOctetString(child1);
    }
    break;
    
  case 101:
    query->which = e2_type_101;
    query->u.type_101 = DecodeRPNQuery(node);
    break;
    
  case 102:
    query->which = e2_type_102;
    {
      ASN_NODE *child1 = node->head;
      query->u.type_102 = DecodeOctetString(child1);
    }
    break;
    
  }
  return query;
}

RPNQuery *
DecodeRPNQuery(node)
     ASN_NODE *node;
{
  int tagno;
  RPNQuery *rPNQuery;
  ASN_NODE *child0;
  rPNQuery = CALLOC(RPNQuery,1);
  if ((!node) || (!rPNQuery))
    return (RPNQuery *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case OBJECTIDENTIFIER:
      rPNQuery->attributeSet = DecodeOid(child0);
      break;
      
    case 0:
      rPNQuery->rpn = DecodeRPNStructure(child0);
      break;
      
    case 1:
      rPNQuery->rpn = DecodeRPNStructure(child0);
      break;
      
    }
  }
  return rPNQuery;
}

RPNStructure *
DecodeRPNStructure(node)
     ASN_NODE *node;
{
  int tagno;
  RPNStructure *rPNStructure;
  rPNStructure = CALLOC(RPNStructure,1);
  if ((!node) || (!rPNStructure))
    return (RPNStructure *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    rPNStructure->which = e3_op;
    {
      ASN_NODE *child1 = node->head;
      rPNStructure->u.op = DecodeOperand(child1);
    }
    break;
    
  case 1:
    rPNStructure->which = e3_rpnRpnOp;
    {
      ASN_NODE *child1;
      if ((child1 = node->head) == NULL)
	return NULL;
      rPNStructure->u.rpnRpnOp.rpn1 = DecodeRPNStructure(child1);
      if ((child1 = child1->next) == NULL)
	return NULL;
      rPNStructure->u.rpnRpnOp.rpn2 = DecodeRPNStructure(child1);
      if ((child1 = child1->next) == NULL)
	return NULL;
      rPNStructure->u.rpnRpnOp.op = DecodeOperator(child1);
    }
    break;
    
  }
  return rPNStructure;
}

Operand *
DecodeOperand(node)
     ASN_NODE *node;
{
  int tagno;
  Operand *operand;
  operand = CALLOC(Operand,1);
  if ((!node) || (!operand))
    return (Operand *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 102:
    operand->which = e4_attrTerm;
    operand->u.attrTerm = DecodeAttributesPlusTerm(node);
    break;
    
  case 31:
    operand->which = e4_resultSet;
    operand->u.resultSet = DecodeResultSetId(node);
    break;
    
  case 214:
    operand->which = e4_resultAttr;
    operand->u.resultAttr = DecodeResultSetPlusAttributes(node);
    break;
    
  }
  return operand;
}

AttributesPlusTerm *
DecodeAttributesPlusTerm(node)
     ASN_NODE *node;
{
  AttributesPlusTerm *attributesPlusTerm;
  ASN_NODE *child0;
  attributesPlusTerm = CALLOC(AttributesPlusTerm,1);
  if ((!node) || (!attributesPlusTerm))
    return (AttributesPlusTerm *)NULL;
  if ((child0=node->head) == NULL)
    return NULL;
  attributesPlusTerm->attributes = DecodeAttributeList(child0);
  if ((child0=child0->next) == NULL)
    return NULL;
  attributesPlusTerm->term = DecodeTerm(child0);
  
  return attributesPlusTerm;
}

ResultSetPlusAttributes *
DecodeResultSetPlusAttributes(node)
     ASN_NODE *node;
{
  int tagno;
  ResultSetPlusAttributes *resultSetPlusAttributes;
  ASN_NODE *child0;
  resultSetPlusAttributes = CALLOC(ResultSetPlusAttributes,1);
  if ((!node) || (!resultSetPlusAttributes))
    return (ResultSetPlusAttributes *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 31:
      resultSetPlusAttributes->resultSet = DecodeResultSetId(child0);
      break;
      
    case 44:
      resultSetPlusAttributes->attributes = DecodeAttributeList(child0);
      break;
      
    }
  }
  return resultSetPlusAttributes;
}

AttributeList *
DecodeAttributeList(node)
     ASN_NODE *node;
{
  AttributeList *attributeList;
  struct AttributeList *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  attributeList = CALLOC(AttributeList,1);
  if ((!node) || (!attributeList))
    return (AttributeList *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct AttributeList,1);
    tmp->item = DecodeAttributeElement(child0);
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

Term *
DecodeTerm(node)
     ASN_NODE *node;
{
  int tagno;
  Term *term;
  term = CALLOC(Term,1);
  if ((!node) || (!term))
    return (Term *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 45:
    term->which = e5_general;
    term->u.general = DecodeOctetString(node);
    break;
    
  case 215:
    term->which = e5_numeric;
    term->u.numeric = DecodeInteger(node);
    break;
    
  case 216:
    term->which = e5_characterString;
    term->u.characterString = DecodeInternationalString(node);
    break;
    
  case 217:
    term->which = e5_oid;
    term->u.oid = DecodeOid(node);
    break;
    
  case 218:
    term->which = e5_dateTime;
    term->u.dateTime = DecodeGeneralizedTime(node);
    break;
    
  case 219:
    term->which = e5_external;
    term->u.external = DecodeExternal(node);
    break;
    
  case 220:
    term->which = e5_integerAndUnit;
    term->u.integerAndUnit = DecodeIntUnit(node);
    break;
    
  case 221:
    term->which = e5_nullVal;
    term->u.nullVal = DecodeNull(node);
    break;
    
  }
  return term;
}

Operator *
DecodeOperator(node)
     ASN_NODE *node;
{
  int tagno;
  Operator *operator_var;
  ASN_NODE *child0;
  operator_var = CALLOC(Operator,1);
  if ((!node) || (!operator_var))
    return (Operator *)NULL;
  child0 = node->head;
  tagno = child0->tagno;
  switch(tagno) {
  case 0:
    operator_var->which = e6_and;
    operator_var->u.and = DecodeNull(child0);
    break;
    
  case 1:
    operator_var->which = e6_or;
    operator_var->u.or = DecodeNull(child0);
    break;
    
  case 2:
    operator_var->which = e6_and_not;
    operator_var->u.and_not = DecodeNull(child0);
    break;
    
  case 3:
    operator_var->which = e6_prox;
    operator_var->u.prox = DecodeProximityOperator(child0);
    break;
    
  }
  return operator_var;
}

AttributeElement *
DecodeAttributeElement(node)
     ASN_NODE *node;
{
  int tagno;
  AttributeElement *attributeElement;
  ASN_NODE *child0;
  attributeElement = CALLOC(AttributeElement,1);
  if ((!node) || (!attributeElement))
    return (AttributeElement *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      attributeElement->attributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 120:
      attributeElement->attributeType = DecodeInteger(child0);
      break;
      
    case 121:
      attributeElement->attributeValue.which = e7_numeric;
      attributeElement->attributeValue.u.numeric = DecodeInteger(child0);
      break;
      
    case 224:
      attributeElement->attributeValue.which = e7_complex;
      {
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tagno = child1->tagno;
	  switch (tagno) {
	  case 1:
	    {
	      struct list_List2 *tmp, *head=NULL, *tail=NULL;
	      ASN_NODE *child2;
	      for(child2=child1->head; child2!=NULL; child2=child2->next) {
		tmp = CALLOC(struct list_List2,1);
		tmp->item = DecodeStringOrNumeric(child2);
		tmp->next = NULL;
		if (head == NULL) {
		  head = tmp;
		  tail = tmp;
		} else {
		  tail->next = tmp;
		  tail = tmp;
		}
	      }
	      attributeElement->attributeValue.u.complex.list = head;
	    }
	    break;
	    
	  case 2:
	    {
	      struct semanticAction_List3 *tmp, *head=NULL, *tail=NULL;
	      ASN_NODE *child2;
	      for(child2=node->head; child2!=NULL; child2=child2->next) {
		tmp = CALLOC(struct semanticAction_List3,1);
		tmp->item = DecodeInteger(child2);
		tmp->next = NULL;
		if (head == NULL) {
		  head = tmp;
		  tail = tmp;
		} else {
		  tail->next = tmp;
		  tail = tmp;
		}
	      }
	      attributeElement->attributeValue.u.complex.semanticAction = head;
	    }
	    break;
	    
	  }
	}
      }
      break;
      
    }
  }
  return attributeElement;
}

ProximityOperator *
DecodeProximityOperator(node)
     ASN_NODE *node;
{
  int tagno;
  ProximityOperator *proximityOperator;
  ASN_NODE *child0;
  proximityOperator = CALLOC(ProximityOperator,1);
  if ((!node) || (!proximityOperator))
    return (ProximityOperator *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      proximityOperator->exclusion = DecodeBoolean(child0);
      break;
      
    case 2:
      proximityOperator->distance = DecodeInteger(child0);
      break;
      
    case 3:
      proximityOperator->ordered = DecodeBoolean(child0);
      break;
      
    case 4:
      proximityOperator->relationType = DecodeInteger(child0);
      break;
      
    case 5:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  proximityOperator->proximityUnitCode.which = e8_known;
	  proximityOperator->proximityUnitCode.u.known = DecodeInteger(child1);
	  break;
	  
	case 2:
	  proximityOperator->proximityUnitCode.which = e8_private;
	  proximityOperator->proximityUnitCode.u.private_var = DecodeInteger(child1);
	  break;
	  
	}
      }
      break;
      
    }
  }
  return proximityOperator;
}


SearchResponse *
DecodeSearchResponse(node)
     ASN_NODE *node;
{
  int tagno;
  int countrecs=0;
  SearchResponse *searchResponse;
  ASN_NODE *child0;
  searchResponse = CALLOC(SearchResponse,1);
  if ((!node) || (!searchResponse))
    return (SearchResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      searchResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 23:
      searchResponse->resultCount = DecodeInteger(child0);
      break;
      
    case 24:
      searchResponse->numberOfRecordsReturned = DecodeInteger(child0);
      break;
      
    case 25:
      searchResponse->nextResultSetPosition = DecodeInteger(child0);
      break;
      
    case 22:
      searchResponse->searchStatus = DecodeBoolean(child0);
      break;
      
    case 26:
      searchResponse->resultSetStatus = DecodeInteger(child0);
      break;
      
    case 27:
      searchResponse->presentStatus = DecodeInteger(child0);
      break;
      
    case 28:
      searchResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 130:
      searchResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 205:
      searchResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 203:
      searchResponse->additionalSearchInfo = DecodeOtherInformation(child0);
      break;
      
    case 201:
      searchResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }

  if (searchResponse->numberOfRecordsReturned != countrecs)
    searchResponse->numberOfRecordsReturned = countrecs;

  return searchResponse;
}

PresentRequest *
DecodePresentRequest(node)
     ASN_NODE *node;
{
  int tagno;
  PresentRequest *presentRequest;
  ASN_NODE *child0;
  presentRequest = CALLOC(PresentRequest,1);

  presentRequest->recordComposition = CALLOC(struct RecordComposition, 1);

  if ((!node) || (!presentRequest))
    return (PresentRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      presentRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 31:
      presentRequest->resultSetId = DecodeResultSetId(child0);
      break;
      
    case 30:
      presentRequest->resultSetStartPoint = DecodeInteger(child0);
      break;
      
    case 29:
      presentRequest->numberOfRecordsRequested = DecodeInteger(child0);
      break;
      
    case 212:
      {
	struct additionalRanges_List4 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct additionalRanges_List4,1);
	  tmp->item = DecodeRange(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	presentRequest->additionalRanges = head;
      }
      break;
      
    case 19:
      presentRequest->recordComposition->which = e9_simple;
      {
	ASN_NODE *child1 = child0->head;
	presentRequest->recordComposition->u.simple = DecodeElementSetNames(child1);
      }
      break;
      
    case 209:
      presentRequest->recordComposition->which = e9_complex;
      presentRequest->recordComposition->u.complex = DecodeCompSpec(child0);
      break;
      
    case 104:
      presentRequest->preferredRecordSyntax = DecodeOid(child0);
      break;
      
    case 204:
      presentRequest->maxSegmentCount = DecodeInteger(child0);
      break;
      
    case 206:
      presentRequest->maxRecordSize = DecodeInteger(child0);
      break;
      
    case 207:
      presentRequest->maxSegmentSize = DecodeInteger(child0);
      break;
      
    case 201:
      presentRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return presentRequest;
}

Segment *
DecodeSegment(node)
     ASN_NODE *node;
{
  int tagno;
  Segment *segment;
  ASN_NODE *child0;
  segment = CALLOC(Segment,1);
  if ((!node) || (!segment))
    return (Segment *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      segment->referenceId = DecodeOctetString(child0);
      break;
      
    case 24:
      segment->numberOfRecordsReturned = DecodeInteger(child0);
      break;
      
    case 0:
      {
	struct segmentRecords_List5 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct segmentRecords_List5,1);
	  tmp->item = DecodeNamePlusRecord(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	segment->segmentRecords = head;
      }
      break;
      
    case 201:
      segment->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return segment;
}

PresentResponse *
DecodePresentResponse(node)
     ASN_NODE *node;
{
  int tagno;
  int countrecs=0;
  PresentResponse *presentResponse;
  ASN_NODE *child0;
  presentResponse = CALLOC(PresentResponse,1);
  if ((!node) || (!presentResponse))
    return (PresentResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      presentResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 24:
      presentResponse->numberOfRecordsReturned = DecodeInteger(child0);
      break;
      
    case 25:
      presentResponse->nextResultSetPosition = DecodeInteger(child0);
      break;
      
    case 27:
      presentResponse->presentStatus = DecodeInteger(child0);
      break;
      
    case 28:
      presentResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 130:
      presentResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 205:
      presentResponse->records = DecodeRecords(child0, &countrecs);
      break;
      
    case 201:
      presentResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  if (presentResponse->numberOfRecordsReturned != countrecs) 
    presentResponse->numberOfRecordsReturned = countrecs;

  return presentResponse;
}

Records *
DecodeRecords(node, count)
     ASN_NODE *node;
     int *count;
{
  int tagno;
  Records *records;
  *count = 0;
  records = CALLOC(Records,1);
  if ((!node) || (!records))
    return (Records *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 28:
    records->which = e10_responseRecords;
    {
      struct responseRecords_List6 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tmp = CALLOC(struct responseRecords_List6,1);
	tmp->item = DecodeNamePlusRecord(child1);
	*count = *count + 1;
	if (head == NULL) {
	  head = tmp;
	  tail = tmp;
	} else {
	  tail->next = tmp;
	  tail = tmp;
	}
      }
      records->u.responseRecords = head;
    }
    break;
    
  case 130:
    records->which = e10_nonSurrogateDiagnostic;
    records->u.nonSurrogateDiagnostic = DecodeDefaultDiagFormat(node);
    *count = *count + 1;
    break;
    
  case 205:
    records->which = e10_multipleNonSurDiagnostics;
    {
      struct multipleNonSurDiagnostics_List7 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tmp = CALLOC(struct multipleNonSurDiagnostics_List7,1);
	tmp->item = DecodeDiagRec(child1);
	*count = *count + 1;
	if (head == NULL) {
	  head = tmp;
	  tail = tmp;
	} else {
	  tail->next = tmp;
	  tail = tmp;
	}
      }
      records->u.multipleNonSurDiagnostics = head;
    }
    break;
    
  }
  return records;
}

NamePlusRecord *
DecodeNamePlusRecord(node)
     ASN_NODE *node;
{
  int tagno;
  NamePlusRecord *namePlusRecord;
  ASN_NODE *child0;
  namePlusRecord = CALLOC(NamePlusRecord,1);
  if ((!node) || (!namePlusRecord))
    return (NamePlusRecord *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      namePlusRecord->name = DecodeDatabaseName(child0);
      break;
      
    case 1:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  namePlusRecord->record.which = e11_retrievalRecord;
	  {
	    ASN_NODE *child2 = child1->head;
	    namePlusRecord->record.u.retrievalRecord = DecodeExternal(child2);
	  }
	  break;
	  
	case 2:
	  namePlusRecord->record.which = e11_surrogateDiagnostic;
	  {
	    ASN_NODE *child2 = child1->head;
	    namePlusRecord->record.u.surrogateDiagnostic = DecodeDiagRec(child2);
	  }
	  break;
	  
	case 3:
	  namePlusRecord->record.which = e11_startingFragment;
	  {
	    ASN_NODE *child2 = child1->head;
	    namePlusRecord->record.u.startingFragment = DecodeFragmentSyntax(child2);
	  }
	  break;
	  
	case 4:
	  namePlusRecord->record.which = e11_intermediateFragment;
	  {
	    ASN_NODE *child2 = child1->head;
	    namePlusRecord->record.u.intermediateFragment = DecodeFragmentSyntax(child2);
	  }
	  break;
	  
	case 5:
	  namePlusRecord->record.which = e11_finalFragment;
	  {
	    ASN_NODE *child2 = child1->head;
	    namePlusRecord->record.u.finalFragment = DecodeFragmentSyntax(child2);
	  }
	  break;
	  
	}
      }
      break;
      
    }
  }
  return namePlusRecord;
}

FragmentSyntax *
DecodeFragmentSyntax(node)
     ASN_NODE *node;
{
  int tagno;
  FragmentSyntax *fragmentSyntax;
  fragmentSyntax = CALLOC(FragmentSyntax,1);
  if ((!node) || (!fragmentSyntax))
    return (FragmentSyntax *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case EXTERNAL:
    fragmentSyntax->which = e12_externallyTagged;
    fragmentSyntax->u.externallyTagged = DecodeExternal(node);
    break;
    
  case OCTETSTRING:
    fragmentSyntax->which = e12_notExternallyTagged;
    fragmentSyntax->u.notExternallyTagged = DecodeOctetString(node);
    break;
  }
  return fragmentSyntax;
}

DiagRec *
DecodeDiagRec(node)
     ASN_NODE *node;
{
  int tagno;
  DiagRec *diagRec;
  diagRec = CALLOC(DiagRec,1);
  if ((!node) || (!diagRec))
    return (DiagRec *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case SEQUENCE:
    diagRec->which = e13_defaultFormat;
    diagRec->u.defaultFormat = DecodeDefaultDiagFormat(node);
    break;
    
  case EXTERNAL:
    diagRec->which = e13_externallyDefined;
    diagRec->u.externallyDefined = DecodeExternal(node);
    break;
  }
  return diagRec;
}

DefaultDiagFormat *
DecodeDefaultDiagFormat(node)
     ASN_NODE *node;
{
  int tagno;
  DefaultDiagFormat *defaultDiagFormat;
  ASN_NODE *child0;
 defaultDiagFormat = CALLOC(DefaultDiagFormat,1);
  if ((!node) || (!defaultDiagFormat))
    return (DefaultDiagFormat *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case OBJECTIDENTIFIER:
    defaultDiagFormat->diagnosticSetId = DecodeOid(child0);
    break;
    
  case INTEGER:
  defaultDiagFormat->condition = DecodeInteger(child0);
    break;
    
  case VISIBLESTRING:
  defaultDiagFormat->addinfo.which = e14_v2Addinfo;
  defaultDiagFormat->addinfo.u.v2Addinfo = DecodeVisibleString(child0);
    break;
    
  case GENERALSTRING:
  defaultDiagFormat->addinfo.which = e14_v3Addinfo;
  defaultDiagFormat->addinfo.u.v3Addinfo = DecodeInternationalString(child0);
    break;
    
  }
  }
  return defaultDiagFormat;
}

Range *
DecodeRange(node)
     ASN_NODE *node;
{
  int tagno;
  Range *range;
  ASN_NODE *child0;
  range = CALLOC(Range,1);
  if ((!node) || (!range))
    return (Range *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      range->startingPosition = DecodeInteger(child0);
      break;
      
    case 2:
      range->numberOfRecords = DecodeInteger(child0);
      break;
      
    }
  }
  return range;
}

ElementSetNames *
DecodeElementSetNames(node)
     ASN_NODE *node;
{
  int tagno;
  ElementSetNames *elementSetNames;
  elementSetNames = CALLOC(ElementSetNames,1);
  if ((!node) || (!elementSetNames))
    return (ElementSetNames *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    elementSetNames->which = e15_genericElementSetName;
    elementSetNames->u.genericElementSetName = DecodeInternationalString(node);
    break;
    
  case 1:
    elementSetNames->which = e15_databaseSpecific;
    {
      struct databaseSpecific_List8 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tmp = CALLOC(struct databaseSpecific_List8,1);
	{
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
	    tagno = child2->tagno;
	    switch (tagno) {
	    case 105:
	      tmp->item.dbName = DecodeDatabaseName(child2);
	      break;
	      
	    case 103:
	      tmp->item.esn = DecodeElementSetName(child2);
	      break;
	    }
	  }
	}
	if (head == NULL) {
	  head = tmp;
	  tail = tmp;
	} else {
	  tail->next = tmp;
	  tail = tmp;
	}
      }
      elementSetNames->u.databaseSpecific = head;
    }
    break;
    
  }
  return elementSetNames;
}


CompSpec *
DecodeCompSpec(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  CompSpec *compSpec;
  compSpec = CALLOC(CompSpec,1);
  if ((!node) || (!compSpec))
    return (CompSpec *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      compSpec->selectAlternativeSyntax = DecodeBoolean(child0);
      break;
      
    case 2:
      compSpec->generic = DecodeSpecification(child0);
      break;
      
    case 3:
      {
	struct dbSpecific_List9 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct dbSpecific_List9,1);
	  {
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 1:
		{
		  ASN_NODE *child3 = child2->head;
		  tmp->item.db = DecodeDatabaseName(child3);
		}
		break;
		
	      case 2:
		tmp->item.spec = DecodeSpecification(child2);
		break;
	      }
	    }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	compSpec->dbSpecific = head;
      }
      break;
      
    case 4:
      {
	struct recordSyntax_List10 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct recordSyntax_List10,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	compSpec->recordSyntax = head;
      }
      break;
      
    }
  }
  return compSpec;
}

Specification *
DecodeSpecification(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Specification *specification;
  specification = CALLOC(Specification,1);
  specification->elementSpec = CALLOC(struct elementSpec, 1);
  if ((!node) || (!specification) || (!specification->elementSpec))
    return (Specification *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      specification->schema = DecodeOid(child0);
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  specification->elementSpec->which = e16_elementSetName;
	  specification->elementSpec->u.elementSetName = DecodeInternationalString(child1);
	  break;
	  
	case 2:
	  specification->elementSpec->which = e16_externalEspec;
	  specification->elementSpec->u.externalEspec = DecodeExternal(child1);
	  break;
	  
	}
      }
      break;
      
    }
  }
  return specification;
}

DeleteResultSetRequest *
DecodeDeleteResultSetRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  DeleteResultSetRequest *deleteResultSetRequest;
  deleteResultSetRequest = CALLOC(DeleteResultSetRequest,1);
  if ((!node) || (!deleteResultSetRequest))
    return (DeleteResultSetRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      deleteResultSetRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 32:
      deleteResultSetRequest->deleteFunction = DecodeInteger(child0);
      break;
      
    case SEQUENCEOF:
      {
	struct resultSetList_List11 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct resultSetList_List11,1);
	  tmp->item = DecodeResultSetId(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	deleteResultSetRequest->resultSetList = head;
      }
      break;
      
    case 201:
      deleteResultSetRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return deleteResultSetRequest;
}

DeleteResultSetResponse *
DecodeDeleteResultSetResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  DeleteResultSetResponse *deleteResultSetResponse;
  deleteResultSetResponse = CALLOC(DeleteResultSetResponse,1);
  if ((!node) || (!deleteResultSetResponse))
    return (DeleteResultSetResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      deleteResultSetResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 0:
      deleteResultSetResponse->deleteOperationStatus = DecodeInteger(child0);
      break;
      
    case 1:
      deleteResultSetResponse->deleteListStatuses = DecodeListStatuses(child0);
      break;
      
    case 34:
      deleteResultSetResponse->numberNotDeleted = DecodeInteger(child0);
      break;
      
    case 35:
      deleteResultSetResponse->bulkStatuses = DecodeListStatuses(child0);
      break;
      
    case 36:
      deleteResultSetResponse->deleteMessage = DecodeInternationalString(child0);
      break;
      
    case 201:
      deleteResultSetResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return deleteResultSetResponse;
}

ListStatuses *
DecodeListStatuses(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ListStatuses *listStatuses;
  struct ListStatuses *tmp, *head=NULL, *tail=NULL;
  listStatuses = CALLOC(ListStatuses,1);
  if ((!node) || (!listStatuses))
    return (ListStatuses *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct ListStatuses,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 31:
	  tmp->item.id = DecodeResultSetId(child1);
	  break;
	  
	case 33:
	  tmp->item.status = DecodeInteger(child1);
	  break;
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}


AccessControlRequest *
DecodeAccessControlRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AccessControlRequest *accessControlRequest;
  accessControlRequest = CALLOC(AccessControlRequest,1);
  if ((!node) || (!accessControlRequest))
    return (AccessControlRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      accessControlRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 37:
      accessControlRequest->securityChallenge.which = e17_simpleForm;
      accessControlRequest->securityChallenge.u.simpleForm = DecodeOctetString(child0);
      break;
      
    case 0:
      accessControlRequest->securityChallenge.which = e17_externallyDefined;
      {
	ASN_NODE *child1 = child0->head;
	accessControlRequest->securityChallenge.u.externallyDefined = DecodeExternal(child1);
      }
      break;
      
    case 201:
      accessControlRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return accessControlRequest;
}

AccessControlResponse *
DecodeAccessControlResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AccessControlResponse *accessControlResponse;
  accessControlResponse = CALLOC(AccessControlResponse,1);
  if ((!node) || (!accessControlResponse))
    return (AccessControlResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      accessControlResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 38:
      accessControlResponse->securityChallengeResponse = 
	CALLOC(struct SecurityChallengeResponse,1);
      accessControlResponse->securityChallengeResponse->which = e18_simpleForm;
      accessControlResponse->securityChallengeResponse->u.simpleForm = DecodeOctetString(child0);
      break;
      
    case 0:
      accessControlResponse->securityChallengeResponse = 
	CALLOC(struct SecurityChallengeResponse,1);
      accessControlResponse->securityChallengeResponse->which = 
	e18_externallyDefined;
      {
	ASN_NODE *child1 = child0->head;
	accessControlResponse->securityChallengeResponse->u.externallyDefined 
	  = DecodeExternal(child1);
      }
      break;
      
    case 223:
      {
	ASN_NODE *child1 = child0->head;
	accessControlResponse->diagnostic = DecodeDiagRec(child1);
      }
      break;
      
    case 201:
      accessControlResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return accessControlResponse;
}

ResourceControlRequest *
DecodeResourceControlRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ResourceControlRequest *resourceControlRequest;
  resourceControlRequest = CALLOC(ResourceControlRequest,1);
  if ((!node) || (!resourceControlRequest))
    return (ResourceControlRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      resourceControlRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 39:
      resourceControlRequest->suspendedFlag = DecodeBoolean(child0);
      break;
      
    case 40:
      {
	ASN_NODE *child1 = child0->head;
	resourceControlRequest->resourceReport = DecodeResourceReport(child1);
      }
      break;
      
    case 41:
      resourceControlRequest->partialResultsAvailable = DecodeInteger(child0);
      break;
      
    case 42:
      resourceControlRequest->responseRequired = DecodeBoolean(child0);
      break;
      
    case 43:
      resourceControlRequest->triggeredRequestFlag = DecodeBoolean(child0);
      break;
      
    case 201:
      resourceControlRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return resourceControlRequest;
}

ResourceControlResponse *
DecodeResourceControlResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ResourceControlResponse *resourceControlResponse;
  resourceControlResponse = CALLOC(ResourceControlResponse,1);
  if ((!node) || (!resourceControlResponse))
    return (ResourceControlResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      resourceControlResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 44:
      resourceControlResponse->continueFlag = DecodeBoolean(child0);
      break;
      
    case 45:
      resourceControlResponse->resultSetWanted = DecodeBoolean(child0);
      break;
      
    case 201:
      resourceControlResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return resourceControlResponse;
}

TriggerResourceControlRequest *
DecodeTriggerResourceControlRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TriggerResourceControlRequest *triggerResourceControlRequest;
  triggerResourceControlRequest = CALLOC(TriggerResourceControlRequest,1);
  if ((!node) || (!triggerResourceControlRequest))
    return (TriggerResourceControlRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      triggerResourceControlRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 46:
      triggerResourceControlRequest->requestedAction = DecodeInteger(child0);
      break;
      
    case 47:
      triggerResourceControlRequest->prefResourceReportFormat = DecodeResourceReportId(child0);
      break;
      
    case 48:
      triggerResourceControlRequest->resultSetWanted = DecodeBoolean(child0);
      break;
      
    case 201:
      triggerResourceControlRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return triggerResourceControlRequest;
}

ResourceReportRequest *
DecodeResourceReportRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ResourceReportRequest *resourceReportRequest;
  resourceReportRequest = CALLOC(ResourceReportRequest,1);
  if ((!node) || (!resourceReportRequest))
    return (ResourceReportRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      resourceReportRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 210:
      resourceReportRequest->opId = DecodeOctetString(child0);
      break;
      
    case 49:
      resourceReportRequest->prefResourceReportFormat = DecodeResourceReportId(child0);
      break;
      
    case 201:
      resourceReportRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return resourceReportRequest;
}

ResourceReportResponse *
DecodeResourceReportResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ResourceReportResponse *resourceReportResponse;
  resourceReportResponse = CALLOC(ResourceReportResponse,1);
  if ((!node) || (!resourceReportResponse))
    return (ResourceReportResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      resourceReportResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 50:
      resourceReportResponse->resourceReportStatus = DecodeInteger(child0);
      break;
      
    case 51:
      {
	ASN_NODE *child1 = child0->head;
	resourceReportResponse->resourceReport = DecodeResourceReport(child1);
      }
      break;
      
    case 201:
      resourceReportResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return resourceReportResponse;
}


StringOrNumeric *
DecodeStringOrNumeric(node)
     ASN_NODE *node;
{
  int tagno;
  StringOrNumeric *stringOrNumeric;
  stringOrNumeric = CALLOC(StringOrNumeric,1);
  if ((!node) || (!stringOrNumeric))
    return (StringOrNumeric *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 1:
    stringOrNumeric->which = e25_string;
    stringOrNumeric->u.string = DecodeInternationalString(node);
    break;
    
  case 2:
    stringOrNumeric->which = e25_numeric;
    stringOrNumeric->u.numeric = DecodeInteger(node);
    break;
    
  }
  return stringOrNumeric;
}

OtherInformation *
DecodeOtherInformation(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  struct OtherInformation *tmp, *head=NULL, *tail=NULL;
  OtherInformation *otherInformation;
  otherInformation = CALLOC(OtherInformation,1);
  if ((!node) || (!otherInformation))
    return (OtherInformation *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct OtherInformation,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  tmp->item.category = DecodeInfoCategory(child1);
	  break;
	  
	case 2:
	  tmp->item.information.which = e24_characterInfo;
	  tmp->item.information.u.characterInfo = DecodeInternationalString(child1);
	  break;
	  
	case 3:
	  tmp->item.information.which = e24_binaryInfo;
	  tmp->item.information.u.binaryInfo = DecodeOctetString(child1);
	  break;
	  
	case 4:
	  tmp->item.information.which = e24_externallyDefinedInfo;
	  tmp->item.information.u.externallyDefinedInfo = DecodeExternal(child1);
	  break;
	  
	case 5:
	  tmp->item.information.which = e24_oid;
	  tmp->item.information.u.oid = DecodeOid(child1);
	  break;
	  
	}
      }
      if (head == NULL) {
	head = tmp;
	tail = tmp;
      } else {
	tail->next = tmp;
	tail = tmp;
      }
    }
  }
  return head;
}

InfoCategory *
DecodeInfoCategory(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  InfoCategory *infoCategory;
  infoCategory = CALLOC(InfoCategory,1);
  if ((!node) || (!infoCategory))
    return (InfoCategory *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      infoCategory->categoryTypeId = DecodeOid(child0);
      break;
      
    case 2:
      infoCategory->categoryValue = DecodeInteger(child0);
      break;
      
    }
  }
  return infoCategory;
}
IntUnit *
DecodeIntUnit(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  IntUnit *intUnit;
  intUnit = CALLOC(IntUnit,1);
  if ((!node) || (!intUnit))
    return (IntUnit *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      intUnit->value = DecodeInteger(child0);
      break;
      
    case 2:
      intUnit->unitUsed = DecodeUnit(child0);
      break;
      
    }
  }
  return intUnit;
}

Unit *
DecodeUnit(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Unit *unit;
  unit = CALLOC(Unit,1);
  if ((!node) || (!unit))
    return (Unit *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      {
	ASN_NODE *child1 = child0->head;
	unit->unitSystem = DecodeInternationalString(child1);
      }
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	unit->unitType = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 3:
      {
	ASN_NODE *child1 = child0->head;
	unit->unit = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 4:
      unit->scaleFactor = DecodeInteger(child0);
      break;
      
    }
  }
  return unit;
}

ScanRequest *
DecodeScanRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ScanRequest *scanRequest;
  scanRequest = CALLOC(ScanRequest,1);
  if ((!node) || (!scanRequest))
    return (ScanRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      scanRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      {
	struct databaseNames_List12 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct databaseNames_List12,1);
	  tmp->item = DecodeDatabaseName(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	scanRequest->databaseNames = head;
      }
      break;
      /* ----
	 case OBJECTIDENTIFIER:
	 scanRequest->attributeSet = DecodeOid(child0);
	 break;
	 ----- */
    case 102:
      scanRequest->termListAndStartPoint = DecodeAttributesPlusTerm(child0);
      break;
      
    case 5:
      scanRequest->stepSize = DecodeInteger(child0);
      break;
      
    case 6:
      if (child0->class_var == UNIVERSAL) {
	scanRequest->attributeSet = DecodeOid(child0);
      } else {
	scanRequest->numberOfTermsRequested = DecodeInteger(child0);
      }
      break;
      
    case 7:
      scanRequest->preferredPositionInResponse = DecodeInteger(child0);
      break;
      
    case 201:
      scanRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return scanRequest;
}

ScanResponse *
DecodeScanResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ScanResponse *scanResponse;
  scanResponse = CALLOC(ScanResponse,1);
  if ((!node) || (!scanResponse))
    return (ScanResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      scanResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      scanResponse->stepSize = DecodeInteger(child0);
      break;
      
    case 4:
      scanResponse->scanStatus = DecodeInteger(child0);
      break;
      
    case 5:
      scanResponse->numberOfEntriesReturned = DecodeInteger(child0);
      break;
      
    case 6:
      scanResponse->positionOfTerm = DecodeInteger(child0);
      break;
      
    case 7:
      scanResponse->entries = DecodeListEntries(child0);
      break;
      
    case 8:
      scanResponse->attributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 201:
      scanResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return scanResponse;
}

ListEntries *
DecodeListEntries(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ListEntries *listEntries;
  listEntries = CALLOC(ListEntries,1);
  if ((!node) || (!listEntries))
    return (ListEntries *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      {
	struct entries_List13 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct entries_List13,1);
	  tmp->item = DecodeEntry(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	listEntries->entries = head;
      }
      break;
      
    case 2:
      {
	struct nonsurrogateDiagnostics_List14 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct nonsurrogateDiagnostics_List14,1);
	  tmp->item = DecodeDiagRec(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	listEntries->nonsurrogateDiagnostics = head;
      }
      break;
      
    }
  }
  return listEntries;
}

Entry *
DecodeEntry(node)
     ASN_NODE *node;
{
  int tagno;
  Entry *entry;
  entry = CALLOC(Entry,1);
  if ((!node) || (!entry))
    return (Entry *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 1:
    entry->which = e19_termInfo;
    entry->u.termInfo = DecodeTermInfo(node);
    break;
    
  case 2:
    entry->which = e19_surrogateDiagnostic;
    {
      ASN_NODE *child1 = node->head;
      entry->u.surrogateDiagnostic = DecodeDiagRec(child1);
    }
    break;
    
  }
  return entry;
}

TermInfo *
DecodeTermInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TermInfo *termInfo;
  termInfo = CALLOC(TermInfo,1);
  if ((!node) || (!termInfo))
    return (TermInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 45:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 215:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 216:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 217:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 218:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 219:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 220:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 221:
      termInfo->term = DecodeTerm(child0);
      break;
      
    case 0:
      termInfo->displayTerm = DecodeInternationalString(child0);
      break;
      
    case 44:
      termInfo->suggestedAttributes = DecodeAttributeList(child0);
      break;
      
    case 4:
      {
	struct alternativeTerm_List15 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct alternativeTerm_List15,1);
	  tmp->item = DecodeAttributesPlusTerm(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	termInfo->alternativeTerm = head;
      }
      break;
      
    case 2:
      termInfo->globalOccurrences = DecodeInteger(child0);
      break;
      
    case 3:
      termInfo->byAttributes = DecodeOccurrenceByAttributes(child0);
      break;
      
    case 201:
      termInfo->otherTermInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return termInfo;
}

OccurrenceByAttributes *
DecodeOccurrenceByAttributes(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  OccurrenceByAttributes *occurrenceByAttributes;
  struct OccurrenceByAttributes *tmp, *head=NULL, *tail=NULL;
  occurrenceByAttributes = CALLOC(OccurrenceByAttributes,1);
  if ((!node) || (!occurrenceByAttributes))
    return (OccurrenceByAttributes *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    ASN_NODE *child1;
    tmp = CALLOC(struct OccurrenceByAttributes,1);
    for(child1=child0->head; child1!=NULL; child1=child1->next) {
      tagno = child1->tagno;
      switch (tagno) {
      case 1:
	{
	  ASN_NODE *child2 = child1->head;
	  tmp->item.attributes = DecodeAttributeList(child2);
	}
	break;
	
      case 2:
	tmp->item.occurrences->which = e20_global;
	{
	  ASN_NODE *child2 = child1->head;
	  tmp->item.occurrences->u.global = DecodeInteger(child2);
	}
	break;
	
      case 3:
	tmp->item.occurrences->which = e20_byDatabase;
	{
	  struct byDatabase_List16 *tmp1, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
	    ASN_NODE *child3;
	    tmp1 = CALLOC(struct byDatabase_List16,1);
	    for(child3=child2->head; child3!=NULL; child3=child3->next) {
	      tagno = child3->tagno;
	      switch (tagno) {
	      case 105:
		tmp1->item.db = DecodeDatabaseName(child3);
		break;
		
	      case 1:
		tmp1->item.num = DecodeInteger(child3);
		break;
		
	      case 201:
		tmp1->item.otherDbInfo = DecodeOtherInformation(child3);
		break;
		
	      }
	    }
	    if (head == NULL) {
	      head = tmp1;
	      tail = tmp1;
	    } else {
	      tail->next = tmp1;
	      tail = tmp1;
	    }
	    tmp->item.occurrences->u.byDatabase = head;
	  }
	}
	break;
	
      case 201:
	tmp->item.otherOccurInfo = DecodeOtherInformation(child1);
	break;
	
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

SortRequest *
DecodeSortRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SortRequest *sortRequest;
  sortRequest = CALLOC(SortRequest,1);
  if ((!node) || (!sortRequest))
    return (SortRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      sortRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      {
	struct inputResultSetNames_List17 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct inputResultSetNames_List17,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	sortRequest->inputResultSetNames = head;
      }
      break;
      
    case 4:
      sortRequest->sortedResultSetName = DecodeInternationalString(child0);
      break;
      
    case 5:
      {
	struct sortSequence_List18 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct sortSequence_List18,1);
	  tmp->item = DecodeSortKeySpec(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	sortRequest->sortSequence = head;
      }
      break;
      
    case 201:
      sortRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return sortRequest;
}

SortResponse *
DecodeSortResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SortResponse *sortResponse;
  sortResponse = CALLOC(SortResponse,1);
  if ((!node) || (!sortResponse))
    return (SortResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      sortResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      sortResponse->sortStatus = DecodeInteger(child0);
      break;
      
    case 4:
      sortResponse->resultSetStatus = DecodeInteger(child0);
      break;
      
    case 5:
      {
	struct diagnostics_List19 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct diagnostics_List19,1);
	  tmp->item = DecodeDiagRec(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	sortResponse->diagnostics = head;
      }
      break;

#ifdef Z3950_2001
    case 6:
      sortResponse->resultCount = DecodeInteger(child0);
      break;
#endif
       
    case 201:
      sortResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return sortResponse;
}

SortKeySpec *
DecodeSortKeySpec(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SortKeySpec *sortKeySpec;
  sortKeySpec = CALLOC(SortKeySpec,1);
  if ((!node) || (!sortKeySpec))
    return (SortKeySpec *)NULL;
  
  child0=node->head;
  sortKeySpec->sortElement = DecodeSortElement(child0);
  
  child0=child0->next;
  for(; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      sortKeySpec->sortRelation = DecodeInteger(child0);
      break;
      
    case 2:
      sortKeySpec->caseSensitivity = DecodeInteger(child0);
      break;
      
    case 3:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  sortKeySpec->missingValueAction.which = e21_abort;
	  sortKeySpec->missingValueAction.u.abort = DecodeNull(child1);
	  break;
	  
	case 2:
	  sortKeySpec->missingValueAction.which = e21_nullVal;
	  sortKeySpec->missingValueAction.u.nullVal = DecodeNull(child1);
	  break;
	  
	case 3:
	  sortKeySpec->missingValueAction.which = e21_missingValueData;
	  sortKeySpec->missingValueAction.u.missingValueData = DecodeOctetString(child1);
	  break;
	  
	}
      }
      break;
      
    }
  }
  return sortKeySpec;
}

SortElement *
DecodeSortElement(node)
     ASN_NODE *node;
{
  int tagno;
  SortElement *sortElement;
  sortElement = CALLOC(SortElement,1);
  if ((!node) || (!sortElement))
    return (SortElement *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 1:
    sortElement->which = e22_generic;
    {
      ASN_NODE *child1 = node->head;
      sortElement->u.generic = DecodeSortKey(child1);
    }
    break;
    
  case 2:
    sortElement->which = e22_datbaseSpecific;
    {
      struct datbaseSpecific_List20 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
        tmp = CALLOC(struct datbaseSpecific_List20,1);
	{
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tagno = child2->tagno;
            switch (tagno) {
            case 105:
	      tmp->item.databaseName = DecodeDatabaseName(child2);
	      break;
	      
            case 0:
            case 1:
            case 2:
	      tmp->item.dbSort = DecodeSortKey(child2);
	      break;
	      
            }
	  }
        }
        if (head == NULL) {
	  head = tmp;
	  tail = tmp;
        } else {
	  tail->next = tmp;
	  tail = tmp;
        }
      }
      sortElement->u.datbaseSpecific = head;
    }
    break;
    
  }
  return sortElement;
}

SortKey *
DecodeSortKey(node)
     ASN_NODE *node;
{
  int tagno;
  SortKey *sortKey;
  sortKey = CALLOC(SortKey,1);
  if ((!node) || (!sortKey))
    return (SortKey *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    sortKey->which = e23_sortfield;
    sortKey->u.sortfield = DecodeInternationalString(node);
    break;
    
  case 1:
    sortKey->which = e23_elementSpec;
    sortKey->u.elementSpec = DecodeSpecification(node);
    break;
    
  case 2:
    sortKey->which = e23_sortAttributes;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case OBJECTIDENTIFIER:
	  sortKey->u.sortAttributes.id = DecodeOid(child1);
	  break;
	  
	case 44:
	  sortKey->u.sortAttributes.list = DecodeAttributeList(child1);
	  break;
	  
	}
      }
    }
    break;
    
  }
  return sortKey;
}

ExtendedServicesRequest *
DecodeExtendedServicesRequest(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ExtendedServicesRequest *extendedServicesRequest;
  extendedServicesRequest = CALLOC(ExtendedServicesRequest,1);
  if ((!node) || (!extendedServicesRequest))
    return (ExtendedServicesRequest *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      extendedServicesRequest->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      extendedServicesRequest->function = DecodeInteger(child0);
      break;
      
    case 4:
      extendedServicesRequest->packageType = DecodeOid(child0);
      break;
      
    case 5:
      extendedServicesRequest->packageName = DecodeInternationalString(child0);
      break;
      
    case 6:
      extendedServicesRequest->userId = DecodeInternationalString(child0);
      break;
      
    case 7:
      extendedServicesRequest->retentionTime = DecodeIntUnit(child0);
      break;
      
    case 8:
      extendedServicesRequest->permissions = DecodePermissions(child0);
      break;
      
    case 9:
      extendedServicesRequest->description = DecodeInternationalString(child0);
      break;
      
    case 10:
      extendedServicesRequest->taskSpecificParameters = DecodeExternal(child0);
      break;
      
    case 11:
      extendedServicesRequest->waitAction = DecodeInteger(child0);
      break;
      
    case 103:
      extendedServicesRequest->elements = DecodeElementSetName(child0);
      break;
      
    case 201:
      extendedServicesRequest->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return extendedServicesRequest;
}

ExtendedServicesResponse *
DecodeExtendedServicesResponse(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ExtendedServicesResponse *extendedServicesResponse;
  extendedServicesResponse = CALLOC(ExtendedServicesResponse,1);
  if ((!node) || (!extendedServicesResponse))
    return (ExtendedServicesResponse *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      extendedServicesResponse->referenceId = DecodeOctetString(child0);
      break;
      
    case 3:
      extendedServicesResponse->operationStatus = DecodeInteger(child0);
      break;
      
    case 4:
      {
	struct diagnostics_List21 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct diagnostics_List21,1);
	  tmp->item = DecodeDiagRec(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	extendedServicesResponse->diagnostics = head;
      }
      break;
      
    case 5:
      extendedServicesResponse->taskPackage = DecodeExternal(child0);
      break;
      
    case 201:
      extendedServicesResponse->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return extendedServicesResponse;
}

Permissions *
DecodePermissions(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  struct Permissions *tmp, *head=NULL, *tail=NULL;
  Permissions *permissions;
  if (!node)
    return (Permissions *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct Permissions,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  tmp->item.userId = DecodeInternationalString(child1);
	  break;
	  
	case 2:
	  {
	    struct allowableFunctions_List22 *tmp2, *head=NULL, *tail=NULL;
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tmp2 = CALLOC(struct allowableFunctions_List22,1);
	      tmp2->item = DecodeInteger(child2);
	      if (head == NULL) {
		head = tmp2;
		tail = tmp2;
	      } else {
		tail->next = tmp2;
		tail = tmp2;
	      }
	    }
	    tmp->item.allowableFunctions = head;
	  }
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  permissions = head;
  return permissions;
}

Close *
DecodeClose(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Close *close;
  close = CALLOC(Close,1);
  if ((!node) || (!close))
    return (Close *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 2:
      close->referenceId = DecodeOctetString(child0);
      break;
      
    case 211:
      close->closeReason = DecodeInteger(child0);
      break;
      
    case 3:
      close->diagnosticInformation = DecodeInternationalString(child0);
      break;
      
    case 4:
      close->resourceReportFormat = DecodeResourceReportId(child0);
      break;
      
    case 5:
      {
	ASN_NODE *child1 = child0->head;
	close->resourceReport = DecodeResourceReport(child1);
      }
      break;
      
    case 201:
      close->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return close;
}


DiagnosticFormat *
DecodeDiagnosticFormat(node)
     ASN_NODE *node;
{
  int tagno;
  struct DiagnosticFormat *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (DiagnosticFormat *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct DiagnosticFormat,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  {
	    ASN_NODE *child2 = child1->head;
	    tagno = child2->tagno;
	    switch (tagno) {
	    case 1:
	      tmp->item.diagnostic->which = e26_defaultDiagRec;
	      tmp->item.diagnostic->u.defaultDiagRec = DecodeDefaultDiagFormat(child2);
	      break;
	      
	    case 2:
	      tmp->item.diagnostic->which = e26_explicitDiagnostic;
	      {
		ASN_NODE *child3 = child2->head;
		tmp->item.diagnostic->u.explicitDiagnostic = DecodeDiagFormat(child3);
	      }
	      break;
	      
	    }
	  }
	  break;
	  
	case 2:
	  tmp->item.message = DecodeInternationalString(child1);
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

DiagFormat *
DecodeDiagFormat(node)
     ASN_NODE *node;
{
  int tagno;
  DiagFormat *diagFormat;
  diagFormat = CALLOC(DiagFormat,1);
  if ((!node) || (!diagFormat))
    return (DiagFormat *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 1000:
    diagFormat->which = e27_tooMany;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.tooMany.tooManyWhat = DecodeInteger(child1);
	  break;
	  
	case 2:
	  diagFormat->u.tooMany.max = DecodeInteger(child1);
	  break;
	  
	}
      }
    }
    break;
    
  case 1001:
    diagFormat->which = e27_badSpec;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.badSpec.spec = DecodeSpecification(child1);
	  break;
	  
	case 2:
	  diagFormat->u.badSpec.db = DecodeDatabaseName(child1);
	  break;
	  
	case 3:
	  {
	    struct goodOnes_List23 *tmp, *head=NULL, *tail=NULL;
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tmp = CALLOC(struct goodOnes_List23,1);
	      tmp->item = DecodeSpecification(child2);
	      if (head == NULL) {
		head = tmp;
		tail = tmp;
	      } else {
		tail->next = tmp;
		tail = tmp;
	      }
	    }
	    diagFormat->u.badSpec.goodOnes = head;
	  }
	  break;
	  
	}
      }
    }
    break;
    
  case 1002:
    diagFormat->which = e27_dbUnavail;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.dbUnavail.db = DecodeDatabaseName(child1);
	  break;
	  
	case 2:
	  {
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 1:
		diagFormat->u.dbUnavail.why.reasonCode = DecodeInteger(child2);
		break;
		
	      case 2:
		diagFormat->u.dbUnavail.why.message = DecodeInternationalString(child2);
		break;
		
	      }
	    }
	  }
	  break;
	  
	}
      }
    }
    break;
    
  case 1003:
    diagFormat->which = e27_unSupOp;
    diagFormat->u.unSupOp = DecodeInteger(node);
    break;
    
  case 1004:
    diagFormat->which = e27_attribute;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.attribute.id = DecodeOid(child1);
	  break;
	  
	case 2:
	  diagFormat->u.attribute.type = DecodeInteger(child1);
	  break;
	  
	case 3:
	  diagFormat->u.attribute.value = DecodeInteger(child1);
	  break;
	  
	case 4:
	  {
	    ASN_NODE *child2 = child1->head;
	    diagFormat->u.attribute.term = DecodeTerm(child2);
	  }
	  break;
	  
	}
      }
    }
    break;
    
  case 1005:
    diagFormat->which = e27_attCombo;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.attCombo.unsupportedCombination = DecodeAttributeList(child1);
	  break;
	  
	case 2:
	  {
	    struct recommendedAlternatives_List24 *tmp, *head=NULL, *tail=NULL;
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tmp = CALLOC(struct recommendedAlternatives_List24,1);
	      tmp->item = DecodeAttributeList(child2);
	      
	      if (head == NULL) {
		head = tmp;
		tail = tmp;
	      } else {
		tail->next = tmp;
		tail = tmp;
	      }
	    }
	    diagFormat->u.attCombo.recommendedAlternatives = head;
	  }
	  break;
	  
	}
      }
    }
    break;
    
  case 1006:
    diagFormat->which = e27_term;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.term.problem = DecodeInteger(child1);
	  break;
	  
	case 2:
	  {
	    ASN_NODE *child2 = child1->head;
	    diagFormat->u.term.term = DecodeTerm(child2);
	  }
	  break;
	  
	}
      }
    }
    break;
    
  case 1007:
    diagFormat->which = e27_proximity;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 1:
	diagFormat->u.proximity.which = e28_resultSets;
	diagFormat->u.proximity.u.resultSets = DecodeNull(child1);
	break;
	
      case 2:
	diagFormat->u.proximity.which = e28_badSet;
	diagFormat->u.proximity.u.badSet = DecodeInternationalString(child1);
	break;
	
      case 3:
	diagFormat->u.proximity.which = e28_relation;
	diagFormat->u.proximity.u.relation = DecodeInteger(child1);
	break;
	
      case 4:
	diagFormat->u.proximity.which = e28_unit;
	diagFormat->u.proximity.u.unit = DecodeInteger(child1);
	break;
	
      case 5:
	diagFormat->u.proximity.which = e28_distance;
	diagFormat->u.proximity.u.distance = DecodeInteger(child1);
	break;
	
      case 6:
	diagFormat->u.proximity.which = e28_attributes;
	{
	  ASN_NODE *child2 = child1->head;
	  diagFormat->u.proximity.u.attributes = DecodeAttributeList(child2);
	}
	break;
	
      case 7:
	diagFormat->u.proximity.which = e28_ordered;
	diagFormat->u.proximity.u.ordered = DecodeNull(child1);
	break;
	
      case 8:
	diagFormat->u.proximity.which = e28_exclusion;
	diagFormat->u.proximity.u.exclusion = DecodeNull(child1);
	break;
	
      }
    }
    break;
    
  case 1008:
    diagFormat->which = e27_scan;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 0:
	diagFormat->u.scan.which = e29_nonZeroStepSize;
	diagFormat->u.scan.u.nonZeroStepSize = DecodeNull(child1);
	break;
	
      case 1:
	diagFormat->u.scan.which = e29_specifiedStepSize;
	diagFormat->u.scan.u.specifiedStepSize = DecodeNull(child1);
	break;
	
      case 3:
	diagFormat->u.scan.which = e29_termList1;
	diagFormat->u.scan.u.termList1 = DecodeNull(child1);
	break;
	
      case 4:
	diagFormat->u.scan.which = e29_termList2;
	{
	  struct termList2_List25 *tmp, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tmp = CALLOC(struct termList2_List25,1);
            tmp->item = DecodeAttributeList(child2);
	    
            if (head == NULL) {
	      head = tmp;
	      tail = tmp;
            } else {
	      tail->next = tmp;
	      tail = tmp;
            }
	  }
	  diagFormat->u.scan.u.termList2 = head;
	}
	break;
	
      case 5:
	diagFormat->u.scan.which = e29_posInResponse;
	diagFormat->u.scan.u.posInResponse = DecodeInteger(child1);
	break;
	
      case 6:
	diagFormat->u.scan.which = e29_resources;
	diagFormat->u.scan.u.resources = DecodeNull(child1);
	break;
	
      case 7:
	diagFormat->u.scan.which = e29_endOfList;
	diagFormat->u.scan.u.endOfList = DecodeNull(child1);
	break;
	
      }
    }
    break;
    
  case 1009:
    diagFormat->which = e27_sort;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 0:
	diagFormat->u.sort.which = e30_sequence;
	diagFormat->u.sort.u.sequence = DecodeNull(child1);
	break;
	
      case 1:
	diagFormat->u.sort.which = e30_noRsName;
	diagFormat->u.sort.u.noRsName = DecodeNull(child1);
	break;
	
      case 2:
	diagFormat->u.sort.which = e30_tooMany;
	diagFormat->u.sort.u.tooMany = DecodeInteger(child1);
	break;
	
      case 3:
	diagFormat->u.sort.which = e30_incompatible;
	diagFormat->u.sort.u.incompatible = DecodeNull(child1);
	break;
	
      case 4:
	diagFormat->u.sort.which = e30_generic;
	diagFormat->u.sort.u.generic = DecodeNull(child1);
	break;
	
      case 5:
	diagFormat->u.sort.which = e30_dbSpecific;
	diagFormat->u.sort.u.dbSpecific = DecodeNull(child1);
	break;
	
      case 6:
	diagFormat->u.sort.which = e30_sortElement;
	{
	  ASN_NODE *child2 = child1->head;
	  diagFormat->u.sort.u.sortElement = DecodeSortElement(child2);
	}
	break;
	
      case 7:
	diagFormat->u.sort.which = e30_key;
	diagFormat->u.sort.u.key = DecodeInteger(child1);
	break;
	
      case 8:
	diagFormat->u.sort.which = e30_action;
	diagFormat->u.sort.u.action = DecodeNull(child1);
	break;
	
      case 9:
	diagFormat->u.sort.which = e30_illegal;
	diagFormat->u.sort.u.illegal = DecodeInteger(child1);
	break;
	
      case 10:
	diagFormat->u.sort.which = e30_inputTooLarge;
	{
	  struct inputTooLarge_List26 *tmp, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tmp = CALLOC(struct inputTooLarge_List26,1);
            tmp->item = DecodeInternationalString(child2);
	    
            if (head == NULL) {
	      head = tmp;
	      tail = tmp;
            } else {
	      tail->next = tmp;
	      tail = tmp;
            }
	  }
	  diagFormat->u.sort.u.inputTooLarge = head;
	}
	break;
	
      case 11:
	diagFormat->u.sort.which = e30_aggregateTooLarge;
	diagFormat->u.sort.u.aggregateTooLarge = DecodeNull(child1);
	break;
	
      }
    }
    break;
    
  case 1010:
    diagFormat->which = e27_segmentation;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 0:
	diagFormat->u.segmentation.which = e31_segmentCount;
	diagFormat->u.segmentation.u.segmentCount = DecodeNull(child1);
	break;
	
      case 1:
	diagFormat->u.segmentation.which = e31_segmentSize;
	diagFormat->u.segmentation.u.segmentSize = DecodeInteger(child1);
	break;
	
      }
    }
    break;
    
  case 1011:
    diagFormat->which = e27_extServices;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 1:
	diagFormat->u.extServices.which = e32_req;
	diagFormat->u.extServices.u.req = DecodeInteger(child1);
	break;
	
      case 2:
	diagFormat->u.extServices.which = e32_permission;
	diagFormat->u.extServices.u.permission = DecodeInteger(child1);
	break;
	
      case 3:
	diagFormat->u.extServices.which = e32_immediate;
	diagFormat->u.extServices.u.immediate = DecodeInteger(child1);
	break;
	
      }
    }
    break;
    
  case 1012:
    diagFormat->which = e27_accessCtrl;
    {
      ASN_NODE *child1 = node->head;
      tagno = child1->tagno;
      switch (tagno) {
      case 1:
	diagFormat->u.accessCtrl.which = e33_noUser;
	diagFormat->u.accessCtrl.u.noUser = DecodeNull(child1);
	break;
	
      case 2:
	diagFormat->u.accessCtrl.which = e33_refused;
	diagFormat->u.accessCtrl.u.refused = DecodeNull(child1);
	break;
	
      case 3:
	diagFormat->u.accessCtrl.which = e33_simple;
	diagFormat->u.accessCtrl.u.simple = DecodeNull(child1);
	break;
	
      case 4:
	diagFormat->u.accessCtrl.which = e33_oid;
	{
	  struct oid_List27 *tmp, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tmp = CALLOC(struct oid_List27,1);
            if (head == NULL) {
	      head = tmp;
	      tail = tmp;
            } else {
	      tail->next = tmp;
	      tail = tmp;
            }
	  }
	  diagFormat->u.accessCtrl.u.oid = head;
	}
	break;
	
      case 5:
	diagFormat->u.accessCtrl.which = e33_alternative;
	{
	  struct alternative_List28 *tmp, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tmp = CALLOC(struct alternative_List28,1);
	    tmp->item = DecodeOid(child2);
            if (head == NULL) {
	      head = tmp;
	      tail = tmp;
            } else {
	      tail->next = tmp;
	      tail = tmp;
            }
	  }
	  diagFormat->u.accessCtrl.u.alternative = head;
	}
	break;
	
      case 6:
	diagFormat->u.accessCtrl.which = e33_pwdInv;
	diagFormat->u.accessCtrl.u.pwdInv = DecodeNull(child1);
	break;
	
      case 7:
	diagFormat->u.accessCtrl.which = e33_pwdExp;
	diagFormat->u.accessCtrl.u.pwdExp = DecodeNull(child1);
	break;
	
      }
    }
    break;
    
  case 1013:
    diagFormat->which = e27_recordSyntax;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  diagFormat->u.recordSyntax.unsupportedSyntax = DecodeOid(child1);
	  break;
	  
	case 2:
	  {
	    struct suggestedAlternatives_List29 *tmp, *head=NULL, *tail=NULL;
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tmp = CALLOC(struct suggestedAlternatives_List29,1);
	      tmp->item = DecodeOid(child2);
	      if (head == NULL) {
		head = tmp;
		tail = tmp;
	      } else {
		tail->next = tmp;
		tail = tmp;
	      }
	    }
	    diagFormat->u.recordSyntax.suggestedAlternatives = head;
	  }
	  break;
	  
	}
      }
    }
    break;
    
  }
  return diagFormat;
}

Explain_Record *
DecodeExplain_Record(node)
     ASN_NODE *node;
{
  int tagno;
  Explain_Record *explain_Record;
  explain_Record = CALLOC(Explain_Record,1);
  if ((!node) || (!explain_Record))
    return (Explain_Record *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    explain_Record->which = e34_targetInfo;
    explain_Record->u.targetInfo = DecodeTargetInfo(node);
    break;
    
  case 1:
    explain_Record->which = e34_databaseInfo;
    explain_Record->u.databaseInfo = DecodeDatabaseInfo(node);
    break;
    
  case 2:
    explain_Record->which = e34_schemaInfo;
    explain_Record->u.schemaInfo = DecodeSchemaInfo(node);
    break;
    
  case 3:
    explain_Record->which = e34_tagSetInfo;
    explain_Record->u.tagSetInfo = DecodeTagSetInfo(node);
    break;
    
  case 4:
    explain_Record->which = e34_recordSyntaxInfo;
    explain_Record->u.recordSyntaxInfo = DecodeRecordSyntaxInfo(node);
    break;
    
  case 5:
    explain_Record->which = e34_attributeSetInfo;
    explain_Record->u.attributeSetInfo = DecodeAttributeSetInfo(node);
    break;
    
  case 6:
    explain_Record->which = e34_termListInfo;
    explain_Record->u.termListInfo = DecodeTermListInfo(node);
    break;
    
  case 7:
    explain_Record->which = e34_extendedServicesInfo;
    explain_Record->u.extendedServicesInfo = DecodeExtendedServicesInfo(node);
    break;
    
  case 8:
    explain_Record->which = e34_attributeDetails;
    explain_Record->u.attributeDetails = DecodeAttributeDetails(node);
    break;
    
  case 9:
    explain_Record->which = e34_termListDetails;
    explain_Record->u.termListDetails = DecodeTermListDetails(node);
    break;
    
  case 10:
    explain_Record->which = e34_elementSetDetails;
    explain_Record->u.elementSetDetails = DecodeElementSetDetails(node);
    break;
    
  case 11:
    explain_Record->which = e34_retrievalRecordDetails;
    explain_Record->u.retrievalRecordDetails = DecodeRetrievalRecordDetails(node);
    break;
    
  case 12:
    explain_Record->which = e34_sortDetails;
    explain_Record->u.sortDetails = DecodeSortDetails(node);
    break;
    
  case 13:
    explain_Record->which = e34_processing;
    explain_Record->u.processing = DecodeProcessingInformation(node);
    break;
    
  case 14:
    explain_Record->which = e34_variants;
    explain_Record->u.variants = DecodeVariantSetInfo(node);
    break;
    
  case 15:
    explain_Record->which = e34_units;
    explain_Record->u.units = DecodeUnitInfo(node);
    break;
    
  case 100:
    explain_Record->which = e34_categoryList;
    explain_Record->u.categoryList = DecodeCategoryList(node);
    break;
    
  }
  return explain_Record;
}


SutrsRecord *
DecodeSUTRS_Record(node)
     ASN_NODE *node;
{
  return((SutrsRecord *)DecodeInternationalString(node));
}

TargetInfo *
DecodeTargetInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TargetInfo *targetInfo;
  targetInfo = CALLOC(TargetInfo,1);
  if ((!node) || (!targetInfo))
    return (TargetInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      targetInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      targetInfo->name = DecodeInternationalString(child0);
      break;
      
    case 2:
      targetInfo->recent_news = DecodeHumanString(child0);
      break;
      
    case 3:
      targetInfo->icon = DecodeIconObject(child0);
      break;
      
    case 4:
      targetInfo->namedResultSets = DecodeBoolean(child0);
      break;
      
    case 5:
      targetInfo->multipleDBsearch = DecodeBoolean(child0);
      break;
      
    case 6:
      targetInfo->maxResultSets = DecodeInteger(child0);
      break;
      
    case 7:
      targetInfo->maxResultSize = DecodeInteger(child0);
      break;
      
    case 8:
      targetInfo->maxTerms = DecodeInteger(child0);
      break;
      
    case 9:
      targetInfo->timeoutInterval = DecodeIntUnit(child0);
      break;
      
    case 10:
      targetInfo->welcomeMessage = DecodeHumanString(child0);
      break;
      
    case 11:
      targetInfo->contactInfo = DecodeContactInfo(child0);
      break;
      
    case 12:
      targetInfo->description = DecodeHumanString(child0);
      break;
      
    case 13:
      {
	struct nicknames_List30 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct nicknames_List30,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	targetInfo->nicknames = head;
      }
      break;
      
    case 14:
      targetInfo->usage_restrictions = DecodeHumanString(child0);
      break;
      
    case 15:
      targetInfo->paymentAddr = DecodeHumanString(child0);
      break;
      
    case 16:
      targetInfo->hours = DecodeHumanString(child0);
      break;
      
    case 17:
      {
	struct dbCombinations_List31 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct dbCombinations_List31,1);
	  tmp->item = DecodeDatabaseList(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	targetInfo->dbCombinations = head;
      }
      break;
      
    case 18:
      {
	struct addresses_List32 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct addresses_List32,1);
	  tmp->item = DecodeNetworkAddress(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	targetInfo->addresses = head;
      }
      break;
      
    case 101:
      {
	struct languages_List33 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct languages_List33,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	targetInfo->languages = head;
      }
      break;
      
    case 19:
      targetInfo->commonAccessInfo = DecodeAccessInfo(child0);
      break;
      
    }
  }
  return targetInfo;
}

DatabaseInfo *
DecodeDatabaseInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  DatabaseInfo *databaseInfo;
  databaseInfo = CALLOC(DatabaseInfo,1);
  if ((!node) || (!databaseInfo))
    return (DatabaseInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      databaseInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      databaseInfo->name = DecodeDatabaseName(child0);
      break;
      
    case 2:
      databaseInfo->explainDatabase = DecodeNull(child0);
      break;
      
    case 3:
      {
	struct nicknames_List34 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct nicknames_List34,1);
	  tmp->item = DecodeDatabaseName(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	databaseInfo->nicknames = head;
      }
      break;
      
    case 4:
      databaseInfo->icon = DecodeIconObject(child0);
      break;
      
    case 5:
      databaseInfo->user_fee = DecodeBoolean(child0);
      break;
      
    case 6:
      databaseInfo->available = DecodeBoolean(child0);
      break;
      
    case 7:
      databaseInfo->titleString = DecodeHumanString(child0);
      break;
      
    case 8:
      {
	struct keywords_List35 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct keywords_List35,1);
	  tmp->item = DecodeHumanString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	databaseInfo->keywords = head;
      }
      break;
      
    case 9:
      databaseInfo->description = DecodeHumanString(child0);
      break;
      
    case 10:
      databaseInfo->associatedDbs = DecodeDatabaseList(child0);
      break;
      
    case 11:
      databaseInfo->subDbs = DecodeDatabaseList(child0);
      break;
      
    case 12:
      databaseInfo->disclaimers = DecodeHumanString(child0);
      break;
      
    case 13:
      databaseInfo->news = DecodeHumanString(child0);
      break;
      
    case 14:
      {
	ASN_NODE *child1 = child0->head;
	databaseInfo->recordCount = CALLOC(struct recordCount,1);
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  databaseInfo->recordCount->which = e35_actualNumber;
	  databaseInfo->recordCount->u.actualNumber = DecodeInteger(child1);
	  break;
	  
	case 1:
	  databaseInfo->recordCount->which = e35_approxNumber;
	  databaseInfo->recordCount->u.approxNumber = DecodeInteger(child1);
	  break;
	  
	}
      }
      break;
      
    case 15:
      databaseInfo->defaultOrder = DecodeHumanString(child0);
      break;
      
    case 16:
      databaseInfo->avRecordSize = DecodeInteger(child0);
      break;
      
    case 17:
      databaseInfo->maxRecordSize = DecodeInteger(child0);
      break;
      
    case 18:
      databaseInfo->hours = DecodeHumanString(child0);
      break;
      
    case 19:
      databaseInfo->bestTime = DecodeHumanString(child0);
      break;
      
    case 20:
      databaseInfo->lastUpdate = DecodeGeneralizedTime(child0);
      break;
      
    case 21:
      databaseInfo->updateInterval = DecodeIntUnit(child0);
      break;
      
    case 22:
      databaseInfo->coverage = DecodeHumanString(child0);
      break;
      
    case 23:
      databaseInfo->proprietary = DecodeBoolean(child0);
      break;
      
    case 24:
      databaseInfo->copyrightText = DecodeHumanString(child0);
      break;
      
    case 25:
      databaseInfo->copyrightNotice = DecodeHumanString(child0);
      break;
      
    case 26:
      databaseInfo->producerContactInfo = DecodeContactInfo(child0);
      break;
      
    case 27:
      databaseInfo->supplierContactInfo = DecodeContactInfo(child0);
      break;
      
    case 28:
      databaseInfo->submissionContactInfo = DecodeContactInfo(child0);
      break;
      
    case 29:
      databaseInfo->accessInfo = DecodeAccessInfo(child0);
      break;
      
    }
  }
  return databaseInfo;
}

SchemaInfo *
DecodeSchemaInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SchemaInfo *schemaInfo;
  schemaInfo = CALLOC(SchemaInfo,1);
  if ((!node) || (!schemaInfo))
    return (SchemaInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      schemaInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      schemaInfo->schema = DecodeOid(child0);
      break;
      
    case 2:
      schemaInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      schemaInfo->description = DecodeHumanString(child0);
      break;
      
    case 4:
      {
	struct tagTypeMapping_List36 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct tagTypeMapping_List36,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 0:
		tmp->item.tagType = DecodeInteger(child2);
		break;
		
	      case 1:
		tmp->item.tagSet = DecodeOid(child2);
		break;
		
	      case 2:
		tmp->item.defaultTagType = DecodeNull(child2);
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	schemaInfo->tagTypeMapping = head;
      }
      break;
      
    case 5:
      {
	struct recordStructure_List37 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=node->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct recordStructure_List37,1);
	  tmp->item = DecodeElementInfo(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	schemaInfo->recordStructure = head;
      }
      break;
      
    }
  }
  return schemaInfo;
}

ElementInfo *
DecodeElementInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ElementInfo *elementInfo;
  elementInfo = CALLOC(ElementInfo,1);
  if ((!node) || (!elementInfo))
    return (ElementInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      elementInfo->elementName = DecodeInternationalString(child0);
      break;
      
    case 2:
      elementInfo->elementTagPath = DecodePath(child0);
      break;
      
    case 3:
      {
	ASN_NODE *child1 = child0->head;
	elementInfo->dataType = DecodeElementDataType(child1);
      }
      break;
      
    case 4:
      elementInfo->required = DecodeBoolean(child0);
      break;
      
    case 5:
      elementInfo->repeatable = DecodeBoolean(child0);
      break;
      
    case 6:
      elementInfo->description = DecodeHumanString(child0);
      break;
      
    }
  }
  return elementInfo;
}

Path *
DecodePath(node)
     ASN_NODE *node;
{
  int tagno;
  struct Path *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (Path *)NULL;
  tagno = node->tagno;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct Path,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  tmp->item.tagType = DecodeInteger(child1);
	  break;
	  
	case 2:
	  {
	    ASN_NODE *child2 = child1->head;
	    tmp->item.tagValue = DecodeStringOrNumeric(child2);
	  }
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

ElementDataType *
DecodeElementDataType(node)
     ASN_NODE *node;
{
  int tagno;
  ElementDataType *elementDataType;
  elementDataType = CALLOC(ElementDataType,1);
  if ((!node) || (!elementDataType))
    return (ElementDataType *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    elementDataType->which = e36_primitive;
    elementDataType->u.primitive = DecodeInteger(node);
    break;
    
  case 1:
    elementDataType->which = e36_structured;
    {
      struct structured_List38 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
        tmp = CALLOC(struct structured_List38,1);
        tmp->item = DecodeElementInfo(child1);
        if (head == NULL) {
	  head = tmp;
	  tail = tmp;
        } else {
	  tail->next = tmp;
	  tail = tmp;
        }
      }
      elementDataType->u.structured = head;
    }
    break;
    
  }
  return elementDataType;
}


TagSetInfo *
DecodeTagSetInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TagSetInfo *tagSetInfo;
  tagSetInfo = CALLOC(TagSetInfo,1);
  
  if ((!node) || (!tagSetInfo))
    return (TagSetInfo *)NULL;
  
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      tagSetInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      tagSetInfo->tagSet = DecodeOid(child0);
      break;
      
    case 2:
      tagSetInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      tagSetInfo->description = DecodeHumanString(child0);
      break;
      
    case 4:
      {
	struct elements_List39 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct elements_List39,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 1:
		tmp->item.elementname = DecodeInternationalString(child2);
		break;
		
	      case 2:
		{
		  struct nicknames_List40 *tmp2, *head=NULL, *tail=NULL;
		  ASN_NODE *child3;
		  for(child3=child2->head; child3!=NULL; child3=child3->next) {
                    tmp2 = CALLOC(struct nicknames_List40,1);
                    tmp2->item = DecodeInternationalString(child3);
		    
                    if (head == NULL) {
		      head = tmp2;
		      tail = tmp2;
                    } else {
		      tail->next = tmp2;
		      tail = tmp2;
                    }
		  }
		  tmp->item.nicknames =  head;
		}
		break;
		
	      case 3:
		{
		  ASN_NODE *child3 = child2->head;
		  tmp->item.elementTag = DecodeStringOrNumeric(child3);
		}
		break;
		
	      case 4:
		tmp->item.description = DecodeHumanString(child2);
		break;
		
	      case 5:
		{
		  ASN_NODE *child3 = child2->head;
		  tmp->item.dataType = DecodeInteger(child3);
		}
		break;
		
	      case 201:
		tmp->item.otherTagInfo = DecodeOtherInformation(child2);
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	tmp->item.nicknames = (struct nicknames_List40 *) head;
      }
      break;
      
    }
  }
  return tagSetInfo;
}

RecordSyntaxInfo *
DecodeRecordSyntaxInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  RecordSyntaxInfo *recordSyntaxInfo;
  recordSyntaxInfo = CALLOC(RecordSyntaxInfo,1);
  if ((!node) || (!recordSyntaxInfo))
    return (RecordSyntaxInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      recordSyntaxInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      recordSyntaxInfo->recordSyntax = DecodeOid(child0);
      break;
      
    case 2:
      recordSyntaxInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      {
	struct transferSyntaxes_List41 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct transferSyntaxes_List41,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	recordSyntaxInfo->transferSyntaxes = head;
      }
      break;
      
    case 4:
      recordSyntaxInfo->description = DecodeHumanString(child0);
      break;
      
    case 5:
      recordSyntaxInfo->asn1Module = DecodeInternationalString(child0);
      break;
      
    case 6:
      {
	struct abstractStructure_List42 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=node->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct abstractStructure_List42,1);
	  tmp->item = DecodeElementInfo(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	recordSyntaxInfo->abstractStructure = head;
      }
      break;
      
    }
  }
  return recordSyntaxInfo;
}

AttributeSetInfo *
DecodeAttributeSetInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeSetInfo *attributeSetInfo;
  attributeSetInfo = CALLOC(AttributeSetInfo,1);
  if ((!node) || (!attributeSetInfo))
    return (AttributeSetInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeSetInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      attributeSetInfo->attributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 2:
      attributeSetInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      {
	struct attributes_List43 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributes_List43,1);
	  tmp->item = DecodeAttributeType(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeSetInfo->attributes = head;
      }
      break;
      
    case 4:
      attributeSetInfo->description = DecodeHumanString(child0);
      break;
      
    }
  }
  return attributeSetInfo;
}

AttributeType *
DecodeAttributeType(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeType *attributeType;
  attributeType = CALLOC(AttributeType,1);
  if ((!node) || (!attributeType))
    return (AttributeType *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeType->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      attributeType->description = DecodeHumanString(child0);
      break;
      
    case 2:
      attributeType->attributeType = DecodeInteger(child0);
      break;
      
    case 3:
      {
	struct attributeValues_List44 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributeValues_List44,1);
	  tmp->item = DecodeAttributeDescription(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeType->attributeValues = head;
      }
      break;
      
    }
  }
  return attributeType;
}

AttributeDescription *
DecodeAttributeDescription(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeDescription *attributeDescription;
  attributeDescription = CALLOC(AttributeDescription,1);
  if ((!node) || (!attributeDescription))
    return (AttributeDescription *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeDescription->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      attributeDescription->description = DecodeHumanString(child0);
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	attributeDescription->attributeValue = DecodeStringOrNumeric(child0);
      }
      break;
      
    case 3:
      {
	struct equivalentAttributes_List45 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct equivalentAttributes_List45,1);
	  tmp->item = DecodeStringOrNumeric(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeDescription->equivalentAttributes = head;
      }
      break;
      
    }
  }
  return attributeDescription;
}

TermListInfo *
DecodeTermListInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TermListInfo *termListInfo;
  termListInfo = CALLOC(TermListInfo,1);
  if ((!node) || (!termListInfo))
    return (TermListInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      termListInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      termListInfo->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      {
	struct termLists_List46 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct termLists_List46,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 1:
		tmp->item.name = DecodeInternationalString(child2);
		break;
		
	      case 2:
		tmp->item.title = DecodeHumanString(child2);
		break;
		
	      case 3:
		tmp->item.searchCost = DecodeInteger(child2);
		break;
		
	      case 4:
		tmp->item.scanable = DecodeBoolean(child2);
		break;
		
	      case 5:
		{
		  struct broader_List47 *tmp2, *head=NULL, *tail=NULL;
		  ASN_NODE *child3;
		  for(child3=child2->head; child3!=NULL; child3=child3->next) {
                    tmp2 = CALLOC(struct broader_List47,1);
                    tmp2->item = DecodeInternationalString(child3);
		    
                    if (head == NULL) {
		      head = tmp2;
		      tail = tmp2;
                    } else {
		      tail->next = tmp2;
		      tail = tmp2;
                    }
		  }
		  tmp->item.broader = head;
		}
		break;
		
	      case 6:
		{
		  struct narrower_List48 *tmp2, *head=NULL, *tail=NULL;
		  ASN_NODE *child3;
		  for(child3=child2->head; child3!=NULL; child3=child3->next) {
                    tmp2 = CALLOC(struct narrower_List48,1);
                    tmp2->item = DecodeInternationalString(child3);
		    
                    if (head == NULL) {
		      head = tmp2;
		      tail = tmp2;
                    } else {
		      tail->next = tmp2;
		      tail = tmp2;
                    }
		  }
		  tmp->item.narrower = head;
		}
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	tmp->item.narrower = (struct narrower_List48 *)head;
      }
      break;
      
    }
  }
  return termListInfo;
}

ExtendedServicesInfo *
DecodeExtendedServicesInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ExtendedServicesInfo *extendedServicesInfo;
  extendedServicesInfo = CALLOC(ExtendedServicesInfo,1);
  if ((!node) || (!extendedServicesInfo))
    return (ExtendedServicesInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      extendedServicesInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      extendedServicesInfo->type = DecodeOid(child0);
      break;
      
    case 2:
      extendedServicesInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      extendedServicesInfo->privateType = DecodeBoolean(child0);
      break;
      
    case 5:
      extendedServicesInfo->restrictionsApply = DecodeBoolean(child0);
      break;
      
    case 6:
      extendedServicesInfo->feeApply = DecodeBoolean(child0);
      break;
      
    case 7:
      extendedServicesInfo->available = DecodeBoolean(child0);
      break;
      
    case 8:
      extendedServicesInfo->retentionSupported = DecodeBoolean(child0);
      break;
      
    case 9:
      extendedServicesInfo->waitAction = DecodeInteger(child0);
      break;
      
    case 10:
      extendedServicesInfo->description = DecodeHumanString(child0);
      break;
      
    case 11:
      extendedServicesInfo->specificExplain = DecodeExternal(child0);
      break;
      
    case 12:
      extendedServicesInfo->esASN = DecodeInternationalString(child0);
      break;
      
    }
  }
  return extendedServicesInfo;
}

AttributeDetails *
DecodeAttributeDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeDetails *attributeDetails;
  attributeDetails = CALLOC(AttributeDetails,1);
  if ((!node) || (!attributeDetails))
    return (AttributeDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeDetails->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      attributeDetails->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      {
	struct attributesBySet_List49 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributesBySet_List49,1);
	  tmp->item = DecodeAttributeSetDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeDetails->attributesBySet = head;
      }
      break;
      
    case 3:
      attributeDetails->attributeCombinations = 
	DecodeAttributeCombinations(child0);
      break;
      
    }
  }
  return attributeDetails;
}

AttributeSetDetails *
DecodeAttributeSetDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeSetDetails *attributeSetDetails;
  attributeSetDetails = CALLOC(AttributeSetDetails,1);
  if ((!node) || (!attributeSetDetails))
    return (AttributeSetDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeSetDetails->attributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 1:
      {
	struct attributesByType_List50 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributesByType_List50,1);
	  tmp->item = DecodeAttributeTypeDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeSetDetails->attributesByType = head;
      }
      break;
      
    }
  }
  return attributeSetDetails;
}

AttributeTypeDetails *
DecodeAttributeTypeDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeTypeDetails *attributeTypeDetails;
  attributeTypeDetails = CALLOC(AttributeTypeDetails,1);
  if ((!node) || (!attributeTypeDetails))
    return (AttributeTypeDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeTypeDetails->attributeType = DecodeInteger(child0);
      break;
      
    case 1:
      attributeTypeDetails->defaultIfOmitted = DecodeOmittedAttributeInterpretation(child0);
      break;
      
    case 2:
      {
	struct attributeValues_List51 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributeValues_List51,1);
	  tmp->item = DecodeAttributeValue(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeTypeDetails->attributeValues = head;
      }
      break;
      
    }
  }
  return attributeTypeDetails;
}

OmittedAttributeInterpretation *
DecodeOmittedAttributeInterpretation(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  OmittedAttributeInterpretation *omittedAttributeInterpretation;
  omittedAttributeInterpretation = CALLOC(OmittedAttributeInterpretation,1);
  if ((!node) || (!omittedAttributeInterpretation))
    return (OmittedAttributeInterpretation *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	ASN_NODE *child1 = child0->head;
	omittedAttributeInterpretation->defaultValue = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 1:
      omittedAttributeInterpretation->defaultDescription = DecodeHumanString(child0);
      break;
      
    }
  }
  return omittedAttributeInterpretation;
}

AttributeValue *
DecodeAttributeValue(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeValue *attributeValue;
  attributeValue = CALLOC(AttributeValue,1);
  if ((!node) || (!attributeValue))
    return (AttributeValue *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	attributeValue->value = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 1:
      attributeValue->description = DecodeHumanString(child0);
      break;
      
    case 2:
      {
	struct subAttributes_List52 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct subAttributes_List52,1);
	  tmp->item = DecodeStringOrNumeric(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeValue->subAttributes = head;
      }
      break;
      
    case 3:
      {
	struct superAttributes_List53 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct superAttributes_List53,1);
	  tmp->item = DecodeStringOrNumeric(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeValue->superAttributes = head;
      }
      break;
      
    case 4:
      attributeValue->partialSupport = DecodeNull(child0);
      break;
      
    }
  }
  return attributeValue;
}

TermListDetails *
DecodeTermListDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TermListDetails *termListDetails;
  termListDetails = CALLOC(TermListDetails,1);
  if ((!node) || (!termListDetails))
    return (TermListDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      termListDetails->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      termListDetails->termListName = DecodeInternationalString(child0);
      break;
      
    case 2:
      termListDetails->description = DecodeHumanString(child0);
      break;
      
    case 3:
      termListDetails->attributes = DecodeAttributeCombinations(child0);
      break;
      
    case 4:
      {
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tagno = child1->tagno;
	  switch (tagno) {
	  case 0:
	    termListDetails->scanInfo->maxStepSize = DecodeInteger(child1);
	    break;
	    
	  case 1:
	    termListDetails->scanInfo->collatingSequence = DecodeHumanString(child1);
	    break;
	    
	  case 2:
	    termListDetails->scanInfo->increasing = DecodeBoolean(child1);
	    break;
	    
	  }
	}
      }
      break;
      
    case 5:
      termListDetails->estNumberTerms = DecodeInteger(child0);
      break;
      
    case 6:
      {
	struct sampleTerms_List54 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct sampleTerms_List54,1);
	  tmp->item = DecodeTerm(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	termListDetails->sampleTerms = head;
      }
      break;
      
    }
  }
  return termListDetails;
}

ElementSetDetails *
DecodeElementSetDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ElementSetDetails *elementSetDetails;
  elementSetDetails = CALLOC(ElementSetDetails,1);
  if ((!node) || (!elementSetDetails))
    return (ElementSetDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      elementSetDetails->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      elementSetDetails->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      elementSetDetails->elementSetName = DecodeElementSetName(child0);
      break;
      
    case 3:
      elementSetDetails->recordSyntax = DecodeOid(child0);
      break;
      
    case 4:
      elementSetDetails->schema = DecodeOid(child0);
      break;
      
    case 5:
      elementSetDetails->description = DecodeHumanString(child0);
      break;
      
    case 6:
      {
	struct detailsPerElement_List55 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct detailsPerElement_List55,1);
	  tmp->item = DecodePerElementDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	elementSetDetails->detailsPerElement = head;
      }
      break;
      
    }
  }
  return elementSetDetails;
}

RetrievalRecordDetails *
DecodeRetrievalRecordDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  RetrievalRecordDetails *retrievalRecordDetails;
  retrievalRecordDetails = CALLOC(RetrievalRecordDetails,1);
  if ((!node) || (!retrievalRecordDetails))
    return (RetrievalRecordDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      retrievalRecordDetails->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      retrievalRecordDetails->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      retrievalRecordDetails->schema = DecodeOid(child0);
      break;
      
    case 3:
      retrievalRecordDetails->recordSyntax = DecodeOid(child0);
      break;
      
    case 4:
      retrievalRecordDetails->description = DecodeHumanString(child0);
      break;
      
    case 5:
      {
	struct detailsPerElement_List56 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct detailsPerElement_List56,1);
	  tmp->item = DecodePerElementDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	retrievalRecordDetails->detailsPerElement = head;
      }
      break;
      
    }
  }
  return retrievalRecordDetails;
}

PerElementDetails *
DecodePerElementDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  PerElementDetails *perElementDetails;
  perElementDetails = CALLOC(PerElementDetails,1);
  if ((!node) || (!perElementDetails))
    return (PerElementDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      perElementDetails->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      perElementDetails->recordTag = DecodeRecordTag(child0);
      break;
      
    case 2:
      {
	struct schemaTags_List57 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct schemaTags_List57,1);
	  tmp->item = DecodePath(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	perElementDetails->schemaTags = head;
      }
      break;
      
    case 3:
      perElementDetails->maxSize = DecodeInteger(child0);
      break;
      
    case 4:
      perElementDetails->minSize = DecodeInteger(child0);
      break;
      
    case 5:
      perElementDetails->avgSize = DecodeInteger(child0);
      break;
      
    case 6:
      perElementDetails->fixedSize = DecodeInteger(child0);
      break;
      
    case 8:
      perElementDetails->repeatable = DecodeBoolean(child0);
      break;
      
    case 9:
      perElementDetails->required = DecodeBoolean(child0);
      break;
      
    case 12:
      perElementDetails->description = DecodeHumanString(child0);
      break;
      
    case 13:
      perElementDetails->contents = DecodeHumanString(child0);
      break;
      
    case 14:
      perElementDetails->billingInfo = DecodeHumanString(child0);
      break;
      
    case 15:
      perElementDetails->restrictions = DecodeHumanString(child0);
      break;
      
    case 16:
      {
	struct alternateNames_List58 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct alternateNames_List58,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	perElementDetails->alternateNames = head;
      }
      break;
      
    case 17:
      {
	struct genericNames_List59 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct genericNames_List59,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	perElementDetails->genericNames = head;
      }
      break;
      
    case 18:
      perElementDetails->searchAccess = 
	DecodeAttributeCombinations(child0);
      break;
      
    }
  }
  return perElementDetails;
}

RecordTag *
DecodeRecordTag(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  RecordTag *recordTag;
  recordTag = CALLOC(RecordTag,1);
  if ((!node) || (!recordTag))
    return (RecordTag *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	ASN_NODE *child1 = child0->head;
	recordTag->qualifier = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 1:
      {
	ASN_NODE *child1 = child0->head;
	recordTag->tagValue = DecodeStringOrNumeric(child1);
      }
      break;
      
    }
  }
  return recordTag;
}

SortDetails *
DecodeSortDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SortDetails *sortDetails;
  sortDetails = CALLOC(SortDetails,1);
  if ((!node) || (!sortDetails))
    return (SortDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      sortDetails->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      sortDetails->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      {
	struct sortKeys_List60 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct sortKeys_List60,1);
	  tmp->item = DecodeSortKeyDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	sortDetails->sortKeys = head;
      }
      break;
      
    }
  }
  return sortDetails;
}

SortKeyDetails *
DecodeSortKeyDetails(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SortKeyDetails *sortKeyDetails;
  sortKeyDetails = CALLOC(SortKeyDetails,1);
  if ((!node) || (!sortKeyDetails))
    return (SortKeyDetails *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      sortKeyDetails->description = DecodeHumanString(child0);
      break;
      
    case 1:
      {
	struct elementSpecifications_List61 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct elementSpecifications_List61,1);
	  tmp->item = DecodeSpecification(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	sortKeyDetails->elementSpecifications = head;
      }
      break;
      
    case 2:
      sortKeyDetails->attributeSpecifications = 
	DecodeAttributeCombinations(child0);
      break;
      
    case 3:
      {
	ASN_NODE *child1 = child0->head;
	sortKeyDetails->sortType = CALLOC(struct sortType,1);
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  sortKeyDetails->sortType->which = e37_character;
	  sortKeyDetails->sortType->u.character = DecodeNull(child1);
	  break;
	  
	case 1:
	  sortKeyDetails->sortType->which = e37_numeric;
	  sortKeyDetails->sortType->u.numeric = DecodeNull(child1);
	  break;
	  
	case 2:
	  sortKeyDetails->sortType->which = e37_structured;
	  sortKeyDetails->sortType->u.structured = DecodeHumanString(child1);
	  break;
	  
	}
      }
      break;
      
    case 4:
      sortKeyDetails->caseSensitivity = DecodeInteger(child0);
      break;
      
    }
  }
  return sortKeyDetails;
}

ProcessingInformation *
DecodeProcessingInformation(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ProcessingInformation *processingInformation;
  processingInformation = CALLOC(ProcessingInformation,1);
  if ((!node) || (!processingInformation))
    return (ProcessingInformation *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      processingInformation->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      processingInformation->databaseName = DecodeDatabaseName(child0);
      break;
      
    case 2:
      processingInformation->processingContext = DecodeInteger(child0);
      break;
      
    case 3:
      processingInformation->name = DecodeInternationalString(child0);
      break;
      
    case 4:
      processingInformation->oid = DecodeOid(child0);
      break;
      
    case 5:
      processingInformation->description = DecodeHumanString(child0);
      break;
      
    case 6:
      processingInformation->instructions = DecodeExternal(child0);
      break;
      
    }
  }
  return processingInformation;
}

VariantSetInfo *
DecodeVariantSetInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  VariantSetInfo *variantSetInfo;
  variantSetInfo = CALLOC(VariantSetInfo,1);
  if ((!node) || (!variantSetInfo))
    return (VariantSetInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      variantSetInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      variantSetInfo->variantSet = DecodeOid(child0);
      break;
      
    case 2:
      variantSetInfo->name = DecodeInternationalString(child0);
      break;
      
    case 3:
      {
	struct variants_List62 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct variants_List62,1);
	  tmp->item = DecodeVariantClass(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	variantSetInfo->variants = head;
      }
      break;
      
    }
  }
  return variantSetInfo;
}

VariantClass *
DecodeVariantClass(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  VariantClass *variantClass;
  variantClass = CALLOC(VariantClass,1);
  if ((!node) || (!variantClass))
    return (VariantClass *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      variantClass->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      variantClass->description = DecodeHumanString(child0);
      break;
      
    case 2:
      variantClass->variantClass = DecodeInteger(child0);
      break;
      
    case 3:
      {
	struct variantTypes_List63 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct variantTypes_List63,1);
	  tmp->item = DecodeVariantType(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	variantClass->variantTypes = head;
      }
      break;
      
    }
  }
  return variantClass;
}

VariantType *
DecodeVariantType(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  VariantType *variantType;
  variantType = CALLOC(VariantType,1);
  if ((!node) || (!variantType))
    return (VariantType *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      variantType->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      variantType->description = DecodeHumanString(child0);
      break;
      
    case 2:
      variantType->variantType = DecodeInteger(child0);
      break;
      
    case 3:
      variantType->variantValue = DecodeVariantValue(child0);
      break;
      
    }
  }
  return variantType;
}

VariantValue *
DecodeVariantValue(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  VariantValue *variantValue;
  variantValue = CALLOC(VariantValue,1);
  if ((!node) || (!variantValue))
    return (VariantValue *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	ASN_NODE *child1 = child0->head;
	variantValue->dataType = DecodeInteger(child1);
      }
      break;
      
    case 1:
      {
	ASN_NODE *child1 = child0->head;
	variantValue->values = DecodeValueSet(child1);
      }
      break;
      
    }
  }
  return variantValue;
}

ValueSet *
DecodeValueSet(node)
     ASN_NODE *node;
{
  int tagno;
  ValueSet *valueSet;
  valueSet = CALLOC(ValueSet,1);
  if ((!node) || (!valueSet))
    return (ValueSet *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    valueSet->which = e38_range;
    valueSet->u.range = DecodeValueRange(node);
    break;
    
  case 1:
    valueSet->which = e38_enumerated;
    {
      struct enumerated_List64 *tmp, *head=NULL, *tail=NULL;
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
        tmp = CALLOC(struct enumerated_List64,1);
        tmp->item = DecodeValueDescription(child1);
	
        if (head == NULL) {
	  head = tmp;
	  tail = tmp;
        } else {
	  tail->next = tmp;
	  tail = tmp;
        }
      }
      valueSet->u.enumerated = head;
    }
    break;
    
  }
  return valueSet;
}

ValueRange *
DecodeValueRange(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ValueRange *valueRange;
  valueRange = CALLOC(ValueRange,1);
  if ((!node) || (!valueRange))
    return (ValueRange *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	ASN_NODE *child1 = child0->head;
	valueRange->lower = DecodeValueDescription(child1);
      }
      break;
      
    case 1:
      {
	ASN_NODE *child1 = child0->head;
	valueRange->upper = DecodeValueDescription(child1);
      }
      break;
      
    }
  }
  return valueRange;
}

ValueDescription *
DecodeValueDescription(node)
     ASN_NODE *node;
{
  int tagno;
  ValueDescription *valueDescription;
  valueDescription = CALLOC(ValueDescription,1);
  if ((!node) || (!valueDescription))
    return (ValueDescription *)NULL;
  tagno = node->tagno;
  switch(tagno) {
    /* ------
       case INTEGER:
       valueDescription->which = e39_integer;
       valueDescription->u.integer = DecodeInteger(node);
       break;
       ------- */
  case GENERALSTRING:
    valueDescription->which = e39_string;
    valueDescription->u.string = DecodeInternationalString(node);
    break;
    
  case OCTETSTRING:
    valueDescription->which = e39_octets;
    valueDescription->u.octets = DecodeOctetString(node);
    break;
    
  case OBJECTIDENTIFIER:
    valueDescription->which = e39_oid;
    valueDescription->u.oid = DecodeOid(node);
    break;
    
  case 1:
    valueDescription->which = e39_unit;
    valueDescription->u.unit = DecodeUnit(node);
    break;
    
  case 2:
    if (node->class_var == UNIVERSAL) {
      valueDescription->which = e39_integer;
      valueDescription->u.integer = DecodeInteger(node);
    } else {
      valueDescription->which = e39_valueAndUnit;
      valueDescription->u.valueAndUnit = DecodeIntUnit(node);
    }
    break;
    
  }
  return valueDescription;
}

UnitInfo *
DecodeUnitInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  UnitInfo *unitInfo;
  unitInfo = CALLOC(UnitInfo,1);
  if ((!node) || (!unitInfo))
    return (UnitInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      unitInfo->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      unitInfo->unitSystem = DecodeInternationalString(child0);
      break;
      
    case 2:
      unitInfo->description = DecodeHumanString(child0);
      break;
      
    case 3:
      {
	struct units_List65 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct units_List65,1);
	  tmp->item = DecodeUnitType(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	unitInfo->units = head;
      }
      break;
      
    }
  }
  return unitInfo;
}

UnitType *
DecodeUnitType(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  UnitType *unitType;
  unitType = CALLOC(UnitType,1);
  if ((!node) || (!unitType))
    return (UnitType *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      unitType->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      unitType->description = DecodeHumanString(child0);
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	tagno = child1->tagno;
	unitType->unitType = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 3:
      {
	struct units_List66 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct units_List66,1);
	  tmp->item = DecodeUnits(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	unitType->units = head;
      }
      break;
      
    }
  }
  return unitType;
}

Units *
DecodeUnits(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Units *units;
  units = CALLOC(Units,1);
  if ((!node) || (!units))
    return (Units *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      units->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      units->description = DecodeHumanString(child0);
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	units->unit = DecodeStringOrNumeric(child1);
      }
      break;
      
    }
  }
  return units;
}

CategoryList *
DecodeCategoryList(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  CategoryList *categoryList;
  categoryList = CALLOC(CategoryList,1);
  if ((!node) || (!categoryList))
    return (CategoryList *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      categoryList->commonInfo = DecodeCommonInfo(child0);
      break;
      
    case 1:
      {
	struct categories_List67 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct categories_List67,1);
	  tmp->item = DecodeCategoryInfo(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	categoryList->categories = head;
      }
      break;
      
    }
  }
  return categoryList;
}

CategoryInfo *
DecodeCategoryInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  CategoryInfo *categoryInfo;
  categoryInfo = CALLOC(CategoryInfo,1);
  if ((!node) || (!categoryInfo))
    return (CategoryInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      categoryInfo->category = DecodeInternationalString(child0);
      break;
      
    case 2:
      categoryInfo->originalCategory = DecodeInternationalString(child0);
      break;
      
    case 3:
      categoryInfo->description = DecodeHumanString(child0);
      break;
      
    case 4:
      categoryInfo->asn1Module = DecodeInternationalString(child0);
      break;
      
    }
  }
  return categoryInfo;
}

CommonInfo *
DecodeCommonInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  CommonInfo *commonInfo;
  commonInfo = CALLOC(CommonInfo,1);
  if ((!node) || (!commonInfo))
    return (CommonInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      commonInfo->dateAdded = DecodeGeneralizedTime(child0);
      break;
      
    case 1:
      commonInfo->dateChanged = DecodeGeneralizedTime(child0);
      break;
      
    case 2:
      commonInfo->expiry = DecodeGeneralizedTime(child0);
      break;
      
    case 3:
      commonInfo->humanString_Language = 
	(LanguageCode *)DecodeLanguageCode(child0);
      break;
      
    case 201:
      commonInfo->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return commonInfo;
}

HumanString *
DecodeHumanString(node)
     ASN_NODE *node;
{
  int tagno;
  struct HumanString *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (HumanString *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct HumanString,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  tmp->item.language = 
	    (LanguageCode *)DecodeLanguageCode(child1);
	  break;
	  
	case 1:
	  tmp->item.text = DecodeInternationalString(child1);
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

IconObject *
DecodeIconObject(node)
     ASN_NODE *node;
{
  int tagno;
  struct IconObject *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (IconObject *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct IconObject,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  {
	    ASN_NODE *child2 = child1->head;
	    tagno = child2->tagno;
	    switch (tagno) {
	    case 1:
	      tmp->item.bodyType.which = e40_ianaType;
	      tmp->item.bodyType.u.ianaType = DecodeInternationalString(child2);
	      break;
	      
	    case 2:
	      tmp->item.bodyType.which = e40_z3950type;
	      tmp->item.bodyType.u.z3950type = DecodeInternationalString(child2);
	      break;
	      
	    case 3:
	      tmp->item.bodyType.which = e40_otherType;
	      tmp->item.bodyType.u.otherType = DecodeInternationalString(child2);
	      break;
	      
	    }
	  }
	  break;
	  
	case 2:
	  tmp->item.content = DecodeOctetString(child1);
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}


ContactInfo *
DecodeContactInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ContactInfo *contactInfo;
  contactInfo = CALLOC(ContactInfo,1);
  if ((!node) || (!contactInfo))
    return (ContactInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      contactInfo->name = DecodeInternationalString(child0);
      break;
      
    case 1:
      contactInfo->description = DecodeHumanString(child0);
      break;
      
    case 2:
      contactInfo->address = DecodeHumanString(child0);
      break;
      
    case 3:
      contactInfo->email = DecodeInternationalString(child0);
      break;
      
    case 4:
      contactInfo->phone = DecodeInternationalString(child0);
      break;
      
    }
  }
  return contactInfo;
}

NetworkAddress *
DecodeNetworkAddress(node)
     ASN_NODE *node;
{
  int tagno;
  NetworkAddress *networkAddress;
  networkAddress = CALLOC(NetworkAddress,1);
  if ((!node) || (!networkAddress))
    return (NetworkAddress *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    networkAddress->which = e41_internetAddress;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  networkAddress->u.internetAddress.hostAddress = DecodeInternationalString(child1);
	  break;
	  
	case 1:
	  networkAddress->u.internetAddress.port = DecodeInteger(child1);
	  break;
	  
	}
      }
    }
    break;
    
  case 1:
    networkAddress->which = e41_osiPresentationAddress;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  networkAddress->u.osiPresentationAddress.pSel = DecodeInternationalString(child1);
	  break;
	  
	case 1:
	  networkAddress->u.osiPresentationAddress.sSel = DecodeInternationalString(child1);
	  break;
	  
	case 2:
	  networkAddress->u.osiPresentationAddress.tSel = DecodeInternationalString(child1);
	  break;
	  
	case 3:
	  networkAddress->u.osiPresentationAddress.nSap = DecodeInternationalString(child1);
	  break;
	  
	}
      }
    }
    break;
    
  case 2:
    networkAddress->which = e41_other;
    {
      ASN_NODE *child1;
      for(child1=node->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  networkAddress->u.other.type = DecodeInternationalString(child1);
	  break;
	  
	case 1:
	  networkAddress->u.other.address = DecodeInternationalString(child1);
	  break;
	  
	}
      }
    }
    break;
    
  }
  return networkAddress;
}

AccessInfo *
DecodeAccessInfo(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AccessInfo *accessInfo;
  accessInfo = CALLOC(AccessInfo,1);
  if ((!node) || (!accessInfo))
    return (AccessInfo *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	struct queryTypesSupported_List68 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct queryTypesSupported_List68,1);
	  tmp->item = DecodeQueryTypeDetails(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->queryTypesSupported = head;
      }
      break;
      
    case 1:
      {
	struct diagnosticsSets_List69 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct diagnosticsSets_List69,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->diagnosticsSets = head;
      }
      break;
      
    case 2:
      {
	struct attributeSetIds_List70 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct attributeSetIds_List70,1);
	  tmp->item =  (AttributeSetId) DecodeAttributeSetId(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->attributeSetIds = head;
      }
      break;
      
    case 3:
      {
	struct schemas_List71 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct schemas_List71,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->schemas = head;
      }
      break;
      
    case 4:
      {
	struct recordSyntaxes_List72 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct recordSyntaxes_List72,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->recordSyntaxes = head;
      }
      break;
      
    case 5:
      {
	struct resourceChallenges_List73 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct resourceChallenges_List73,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->resourceChallenges = head;
      }
      break;
      
    case 6:
      accessInfo->restrictedAccess = DecodeAccessRestrictions(child0);
      break;
      
    case 8:
      accessInfo->costInfo = DecodeCosts(child0);
      break;
      
    case 9:
      {
	struct variantSets_List74 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct variantSets_List74,1);
	  tmp->item = DecodeOid(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->variantSets = head;
      }
      break;
      
    case 10:
      {
	struct elementSetNames_List75 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct elementSetNames_List75,1);
	  tmp->item = DecodeElementSetName(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->elementSetNames = head;
      }
      break;
      
    case 11:
      {
	struct unitSystems_List76 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct unitSystems_List76,1);
	  tmp->item = DecodeInternationalString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	accessInfo->unitSystems = head;
      }
      break;
      
    }
  }
  return accessInfo;
}

QueryTypeDetails *
DecodeQueryTypeDetails(node)
     ASN_NODE *node;
{
  int tagno;
  QueryTypeDetails *queryTypeDetails;
  queryTypeDetails = CALLOC(QueryTypeDetails,1);
  if ((!node) || (!queryTypeDetails))
    return (QueryTypeDetails *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 0:
    queryTypeDetails->which = e42_private;
    queryTypeDetails->u.private_var = DecodePrivateCapabilities(node);
    break;
    
  case 1:
    queryTypeDetails->which = e42_rpn;
    queryTypeDetails->u.rpn = DecodeRpnCapabilities(node);
    break;
    
  case 2:
    queryTypeDetails->which = e42_iso8777;
    queryTypeDetails->u.iso8777 = DecodeIso8777Capabilities(node);
    break;
    
  case 100:
    queryTypeDetails->which = e42_z39_58;
    queryTypeDetails->u.z39_58 = DecodeHumanString(node);
    break;
    
  case 101:
    queryTypeDetails->which = e42_erpn;
    queryTypeDetails->u.erpn = DecodeRpnCapabilities(node);
    break;
    
  case 102:
    queryTypeDetails->which = e42_rankedList;
    queryTypeDetails->u.rankedList = DecodeHumanString(node);
    break;
    
  }
  return queryTypeDetails;
}

PrivateCapabilities *
DecodePrivateCapabilities(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  PrivateCapabilities *privateCapabilities;
  privateCapabilities = CALLOC(PrivateCapabilities,1);
  if ((!node) || (!privateCapabilities))
    return (PrivateCapabilities *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	struct operators_List77 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct operators_List77,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 0:
		tmp->item.operator_var = DecodeInternationalString(child2);
		break;
		
	      case 1:
		tmp->item.description = DecodeHumanString(child2);
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	privateCapabilities->operators = head;
      }
      break;
      
    case 1:
      {
	struct searchKeys_List78 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct searchKeys_List78,1);
	  tmp->item = DecodeSearchKey(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	privateCapabilities->searchKeys = head;
      }
      break;
      
    case 2:
      {
	struct description_List79 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct description_List79,1);
	  tmp->item = DecodeHumanString(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	privateCapabilities->description = head;
      }
      break;
      
    }
  }
  return privateCapabilities;
}

RpnCapabilities *
DecodeRpnCapabilities(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  RpnCapabilities *rpnCapabilities;
  rpnCapabilities = CALLOC(RpnCapabilities,1);
  if ((!node) || (!rpnCapabilities))
    return (RpnCapabilities *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	struct operators_List80 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct operators_List80,1);
	  tmp->item = DecodeInteger(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	rpnCapabilities->operators = head;
      }
      break;
      
    case 1:
      rpnCapabilities->resultSetAsOperandSupported = DecodeBoolean(child0);
      break;
      
    case 2:
      rpnCapabilities->restrictionOperandSupported = DecodeBoolean(child0);
      break;
      
    case 3:
      rpnCapabilities->proximity = DecodeProximitySupport(child0);
      break;
      
    }
  }
  return rpnCapabilities;
}

Iso8777Capabilities *
DecodeIso8777Capabilities(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Iso8777Capabilities *iso8777Capabilities;
  iso8777Capabilities = CALLOC(Iso8777Capabilities,1);
  if ((!node) || (!iso8777Capabilities))
    return (Iso8777Capabilities *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      {
	struct searchKeys_List81 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct searchKeys_List81,1);
	  tmp->item = DecodeSearchKey(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	iso8777Capabilities->searchKeys = head;
      }
      break;
      
    case 1:
      iso8777Capabilities->restrictions = DecodeHumanString(child0);
      break;
      
    }
  }
  return iso8777Capabilities;
}

ProximitySupport *
DecodeProximitySupport(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ProximitySupport *proximitySupport;
  proximitySupport = CALLOC(ProximitySupport,1);
  if ((!node) || (!proximitySupport))
    return (ProximitySupport *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      proximitySupport->anySupport = DecodeBoolean(child0);
      break;
      
    case 1:
      {
	struct unitsSupported_List82 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct unitsSupported_List82,1);
	  switch(child1->tagno) {
	  case 1:
	    tmp->item = CALLOC(struct unitstypes,1);
	    tmp->item->which = e43_known;
	    tmp->item->u.known = DecodeInteger(child1);
	    break;
	    
	  case 2:
	    tmp->item = CALLOC(struct unitstypes,1);
	    tmp->item->which = e43_private;
	    {
	      ASN_NODE *child2;
	      for(child2=child1->head; child2!=NULL; child2=child2->next) {
		tagno = child2->tagno;
		switch (tagno) {
		case 0:
		  tmp->item->u.private_var.unit = DecodeInteger(child2);
		  break;
		  
		case 1:
		  {
		    ASN_NODE *child3 = child2->head;
		    tmp->item->u.private_var.description = DecodeHumanString(child3);
		  }
		  break;
		  
		}
	      }
	    }
	    break;
	  }
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	proximitySupport->unitsSupported = head;
      }
      break;
      
    }
  }
  return proximitySupport;
}

SearchKey *
DecodeSearchKey(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  SearchKey *searchKey;
  searchKey = CALLOC(SearchKey,1);
  if ((!node) || (!searchKey))
    return (SearchKey *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      searchKey->searchKey = DecodeInternationalString(child0);
      break;
      
    case 1:
      searchKey->description = DecodeHumanString(child0);
      break;
      
    }
  }
  return searchKey;
}

AccessRestrictions *
DecodeAccessRestrictions(node)
     ASN_NODE *node;
{
  int tagno;
  struct AccessRestrictions *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (AccessRestrictions *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct AccessRestrictions,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 0:
	  {
	    ASN_NODE *child2 = child1->head;
	    tmp->item.accessType = DecodeInteger(child2);
	  }
	  break;
	  
	case 1:
	  tmp->item.accessText = DecodeHumanString(child1);
	  break;
	  
	case 2:
	  {
	    struct accessChallenges_List83 *tmp2, *head=NULL, *tail=NULL;
	    ASN_NODE *child2;
	    for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tmp2 = CALLOC(struct accessChallenges_List83,1);
	      tmp2->item = DecodeOid(child2);
	      if (head == NULL) {
		head = tmp2;
		tail = tmp2;
	      } else {
		tail->next = tmp2;
		tail = tmp2;
	      }
	    }
	    tmp->item.accessChallenges = head;
	  }
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

Costs *
DecodeCosts(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Costs *costs;
  costs = CALLOC(Costs,1);
  if ((!node) || (!costs))
    return (Costs *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      costs->connectCharge = DecodeCharge(child0);
      break;
      
    case 1:
      costs->connectTime = DecodeCharge(child0);
      break;
      
    case 2:
      costs->displayCharge = DecodeCharge(child0);
      break;
      
    case 3:
      costs->searchCharge = DecodeCharge(child0);
      break;
      
    case 4:
      costs->subscriptCharge = DecodeCharge(child0);
      break;
      
    case 5:
      {
	struct otherCharges_List84 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct otherCharges_List84,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 1:
		tmp->item.forWhat = DecodeHumanString(child2);
		break;
		
	      case 2:
		tmp->item.charge = DecodeCharge(child2);
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	costs->otherCharges = head;
      }
      break;
      
    }
  }
  return costs;
}

Charge *
DecodeCharge(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Charge *charge;
  charge = CALLOC(Charge,1);
  if ((!node) || (!charge))
    return (Charge *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      charge->cost = DecodeIntUnit(child0);
      break;
      
    case 2:
      charge->perWhat = DecodeUnit(child0);
      break;
      
    case 3:
      charge->text = DecodeHumanString(child0);
      break;
      
    }
  }
  return charge;
}

DatabaseList *
DecodeDatabaseList(node)
     ASN_NODE *node;
{
  struct DatabaseList *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (DatabaseList *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct DatabaseList,1);
    tmp->item = DecodeDatabaseName(child0);
    
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

AttributeCombinations *
DecodeAttributeCombinations(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeCombinations *attributeCombinations;
  attributeCombinations = CALLOC(AttributeCombinations,1);
  if ((!node) || (!attributeCombinations))
    return (AttributeCombinations *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeCombinations->defaultAttributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 1:
      {
	struct legalCombinations_List85 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct legalCombinations_List85,1);
	  tmp->item = DecodeAttributeCombination(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeCombinations->legalCombinations = head;
      }
      break;
      
    }
  }
  return attributeCombinations;
}

AttributeCombination *
DecodeAttributeCombination(node)
     ASN_NODE *node;
{
  struct AttributeCombination *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (AttributeCombination *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct AttributeCombination,1);
    tmp->item = DecodeAttributeOccurrence(child0);
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

AttributeOccurrence *
DecodeAttributeOccurrence(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  AttributeOccurrence *attributeOccurrence;
  attributeOccurrence = CALLOC(AttributeOccurrence,1);
  if ((!node) || (!attributeOccurrence))
    return (AttributeOccurrence *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 0:
      attributeOccurrence->attributeSet = DecodeAttributeSetId(child0);
      break;
      
    case 1:
      attributeOccurrence->attributeType = DecodeInteger(child0);
      break;
      
    case 2:
      attributeOccurrence->mustBeSupplied = DecodeNull(child0);
      break;
      
    case 3:
      attributeOccurrence->attributeValues.which = e44_any_or_none;
      attributeOccurrence->attributeValues.u.any_or_none = DecodeNull(child0);
      break;
      
    case 4:
      attributeOccurrence->attributeValues.which = e44_specific;
      {
	struct specific_List86 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct specific_List86,1);
	  tmp->item = DecodeStringOrNumeric(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	attributeOccurrence->attributeValues.u.specific = head;
      }
      break;
      
    }
  }
  return attributeOccurrence;
}


OPACRecord *
DecodeOPACRecord(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  OPACRecord *oPACRecord;
  oPACRecord = CALLOC(OPACRecord,1);
  if ((!node) || (!oPACRecord))
    return (OPACRecord *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      oPACRecord->bibliographicRecord = DecodeExternal(child0);
      break;
      
    case 2:
      {
	struct holdingsData_List87 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct holdingsData_List87,1);
	  tmp->item = DecodeHoldingsRecord(child1);
	  
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	oPACRecord->holdingsData = head;
      }
      break;
      
    }
  }
  return oPACRecord;
}

HoldingsRecord *
DecodeHoldingsRecord(node)
     ASN_NODE *node;
{
  int tagno;
  HoldingsRecord *holdingsRecord;
  holdingsRecord = CALLOC(HoldingsRecord,1);
  if ((!node) || (!holdingsRecord))
    return (HoldingsRecord *)NULL;
  tagno = node->tagno;
  switch(tagno) {
  case 1:
    holdingsRecord->which = e45_marcHoldingsRecord;
    holdingsRecord->u.marcHoldingsRecord = DecodeExternal(node->head);
    break;
    
  case 2:
    holdingsRecord->which = e45_holdingsAndCirc;
    holdingsRecord->u.holdingsAndCirc = DecodeHoldingsAndCircData(node);
    break;
    
  }
  return holdingsRecord;
}

HoldingsAndCircData *
DecodeHoldingsAndCircData(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  HoldingsAndCircData *holdingsAndCircData;
  holdingsAndCircData = CALLOC(HoldingsAndCircData,1);
  if ((!node) || (!holdingsAndCircData))
    return (HoldingsAndCircData *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      holdingsAndCircData->typeOfRecord = DecodeInternationalString(child0);
      break;
      
    case 2:
      holdingsAndCircData->encodingLevel = DecodeInternationalString(child0);
      break;
      
    case 3:
      holdingsAndCircData->format = DecodeInternationalString(child0);
      break;
      
    case 4:
      holdingsAndCircData->receiptAcqStatus = DecodeInternationalString(child0);
      break;
      
    case 5:
      holdingsAndCircData->generalRetention = DecodeInternationalString(child0);
      break;
      
    case 6:
      holdingsAndCircData->completeness = DecodeInternationalString(child0);
      break;
      
    case 7:
      holdingsAndCircData->dateOfReport = DecodeInternationalString(child0);
      break;
      
    case 8:
      holdingsAndCircData->nucCode = DecodeInternationalString(child0);
      break;
      
    case 9:
      holdingsAndCircData->localLocation = DecodeInternationalString(child0);
      break;
      
    case 10:
      holdingsAndCircData->shelvingLocation = DecodeInternationalString(child0);
      break;
      
    case 11:
      holdingsAndCircData->callNumber = DecodeInternationalString(child0);
      break;
      
    case 12:
      holdingsAndCircData->shelvingData = DecodeInternationalString(child0);
      break;
      
    case 13:
      holdingsAndCircData->copyNumber = DecodeInternationalString(child0);
      break;
      
    case 14:
      holdingsAndCircData->publicNote = DecodeInternationalString(child0);
      break;
      
    case 15:
      holdingsAndCircData->reproductionNote = DecodeInternationalString(child0);
      break;
      
    case 16:
      holdingsAndCircData->termsUseRepro = DecodeInternationalString(child0);
      break;
      
    case 17:
      holdingsAndCircData->enumAndChron = DecodeInternationalString(child0);
      break;
      
    case 18:
      {
	struct volumes_List88 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct volumes_List88,1);
	  tmp->item = DecodeVolume(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	holdingsAndCircData->volumes = head;
      }
      break;
      
    case 19:
      {
	struct circulationData_List89 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct circulationData_List89,1);
	  tmp->item = DecodeCircRecord(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	holdingsAndCircData->circulationData = head;
      }
      break;
      
    }
  }
  return holdingsAndCircData;
}

Volume *
DecodeVolume(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Volume *volume;
  volume = CALLOC(Volume,1);
  if ((!node) || (!volume))
    return (Volume *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      volume->enumeration = DecodeInternationalString(child0);
      break;
      
    case 2:
      volume->chronology = DecodeInternationalString(child0);
      break;
      
    case 3:
      volume->enumAndChron = DecodeInternationalString(child0);
      break;
      
    }
  }
  return volume;
}

CircRecord *
DecodeCircRecord(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  CircRecord *circRecord;
  circRecord = CALLOC(CircRecord,1);
  if ((!node) || (!circRecord))
    return (CircRecord *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      circRecord->availableNow = DecodeBoolean(child0);
      break;
      
    case 2:
      circRecord->availablityDate = DecodeInternationalString(child0);
      break;
      
    case 3:
      circRecord->availableThru = DecodeInternationalString(child0);
      break;
      
    case 4:
      circRecord->restrictions = DecodeInternationalString(child0);
      break;
      
    case 5:
      circRecord->itemId = DecodeInternationalString(child0);
      break;
      
    case 6:
      circRecord->renewable = DecodeBoolean(child0);
      break;
      
    case 7:
      circRecord->onHold = DecodeBoolean(child0);
      break;
      
    case 8:
      circRecord->enumAndChron = DecodeInternationalString(child0);
      break;
      
    case 9:
      circRecord->midspine = DecodeInternationalString(child0);
      break;
      
    case 10:
      circRecord->temporaryLocation = DecodeInternationalString(child0);
      break;
      
    }
  }
  return circRecord;
}

BriefBib *
DecodeBriefBib(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  BriefBib *briefBib;
  briefBib = CALLOC(BriefBib,1);
  if ((!node) || (!briefBib))
    return (BriefBib *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      briefBib->title = DecodeInternationalString(child0);
      break;
      
    case 2:
      briefBib->author = DecodeInternationalString(child0);
      break;
      
    case 3:
      briefBib->callNumber = DecodeInternationalString(child0);
      break;
      
    case 4:
      briefBib->recordType = DecodeInternationalString(child0);
      break;
      
    case 5:
      briefBib->bibliographicLevel = DecodeInternationalString(child0);
      break;
      
    case 6:
      {
	struct format_List90 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct format_List90,1);
	  tmp->item = DecodeFormatSpec(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	briefBib->format = head;
      }
      break;
      
    case 7:
      briefBib->publicationPlace = DecodeInternationalString(child0);
      break;
      
    case 8:
      briefBib->publicationDate = DecodeInternationalString(child0);
      break;
      
    case 9:
      briefBib->targetSystemKey = DecodeInternationalString(child0);
      break;
      
    case 10:
      briefBib->satisfyingElement = DecodeInternationalString(child0);
      break;
      
    case 11:
      briefBib->rank = DecodeInteger(child0);
      break;
      
    case 12:
      briefBib->documentId = DecodeInternationalString(child0);
      break;
      
    case 13:
      briefBib->abstract = DecodeInternationalString(child0);
      break;
      
    case 201:
      briefBib->otherInfo = DecodeOtherInformation(child0);
      break;
      
    }
  }
  return briefBib;
}

FormatSpec *
DecodeFormatSpec(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  FormatSpec *formatSpec;
  formatSpec = CALLOC(FormatSpec,1);
  if ((!node) || (!formatSpec))
    return (FormatSpec *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      formatSpec->type = DecodeInternationalString(child0);
      break;
      
    case 2:
      formatSpec->size = DecodeInteger(child0);
      break;
      
    case 3:
      formatSpec->bestPosn = DecodeInteger(child0);
      break;
      
    }
  }
  return formatSpec;
}

GenericRecord *
DecodeGenericRecord(node)
     ASN_NODE *node;
{
  struct GenericRecord *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (GenericRecord *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct GenericRecord,1);
    tmp->item = DecodeTaggedElement(child0);
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

TaggedElement *
DecodeTaggedElement(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  TaggedElement *taggedElement;
  taggedElement = CALLOC(TaggedElement,1);
  if ((!node) || (!taggedElement))
    return (TaggedElement *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      taggedElement->tagType = DecodeInteger(child0);
      break;
      
    case 2:
      {
	ASN_NODE *child1 = child0->head;
	taggedElement->tagValue = DecodeStringOrNumeric(child1);
      }
      break;
      
    case 3:
      taggedElement->tagOccurrence = DecodeInteger(child0);
      break;
      
    case 4:
      {
	ASN_NODE *child1 = child0->head;
	taggedElement->content = DecodeElementData(child1);
      }
      break;
      
    case 5:
      taggedElement->metaData = DecodeElementMetaData(child0);
      break;
      
    case 6:
      taggedElement->appliedVariant = DecodeVariant(child0);
      break;
      
    }
  }
  return taggedElement;
}

ElementData *
DecodeElementData(node)
     ASN_NODE *node;
{
  int tagno;
  ElementData *elementData;
  elementData = CALLOC(ElementData,1);
  if ((!node) || (!elementData))
    return (ElementData *)NULL;
  tagno = node->tagno;
  switch(tagno) {
    /* ----
       case OCTETSTRING:
       elementData->which = e46_octets;
       elementData->u.octets = DecodeOctetString(node);
       break;
       ------ */
    /* ----
       case INTEGER:
       elementData->which = e46_numeric;
       elementData->u.numeric = DecodeInteger(node);
       break;
       ----- */
  case GENERALIZEDTYPE:
    elementData->which = e46_date;
    elementData->u.date = DecodeGeneralizedTime(node);
    break;
    
  case EXTERNAL:
    elementData->which = e46_ext;
    elementData->u.ext = DecodeExternal(node);
    break;
    
  case GENERALSTRING:
    elementData->which = e46_string;
    elementData->u.string = DecodeInternationalString(node);
    break;
    /* -----
       case ASN_BOOLEAN:
       elementData->which = e46_trueOrFalse;
       elementData->u.trueOrFalse = DecodeBoolean(node);
       break;
       ----- */
    /* -----
       case OBJECTIDENTIFIER:
       elementData->which = e46_oid;
       elementData->u.oid = DecodeOid(node);
       break;
       ----- */
  case 1:
    if (node->class_var == UNIVERSAL) {
      elementData->which = e46_trueOrFalse;
      elementData->u.trueOrFalse = DecodeBoolean(node);
    } else {
      elementData->which = e46_intUnit;
      elementData->u.intUnit = DecodeIntUnit(node);
    }
    break;
    
  case 2:
    if (node->class_var == UNIVERSAL) {
      elementData->which = e46_numeric;
      elementData->u.numeric = DecodeInteger(node);
    } else {
      elementData->which = e46_elementNotThere;
      elementData->u.elementNotThere = DecodeNull(node);
    }
    break;
    
  case 3:
    elementData->which = e46_elementEmpty;
    elementData->u.elementEmpty = DecodeNull(node);
    break;
    
  case 4:
    if (node->class_var == UNIVERSAL) {
      elementData->which = e46_octets;
      elementData->u.octets = DecodeOctetString(node);
    } else {
      elementData->which = e46_noDataRequested;
      elementData->u.noDataRequested = DecodeNull(node);
    }
    break;
    
  case 5:
    elementData->which = e46_diagnostic;
    elementData->u.diagnostic = DecodeExternal(node);
    break;
    
  case 6:
    if (node->class_var == UNIVERSAL) {
      elementData->which = e46_oid;
      elementData->u.oid = DecodeOid(node);
    } else {
      elementData->which = e46_subtree;
      {
	ASN_NODE *child1 = node->head;
	{
	  struct subtree_List91 *tmp, *head=NULL, *tail=NULL;
	  ASN_NODE *child2;
	  for(child2=child1->head; child2!=NULL; child2=child2->next) {
            tmp = CALLOC(struct subtree_List91,1);
            tmp->item = DecodeTaggedElement(child2);
            if (head == NULL) {
	      head = tmp;
	      tail = tmp;
            } else {
	      tail->next = tmp;
	      tail = tmp;
            }
	  }
	  elementData->u.subtree = head;
	}
      }
    }
    break;
    
  }
  return elementData;
}

ElementMetaData *
DecodeElementMetaData(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  ElementMetaData *elementMetaData;
  elementMetaData = CALLOC(ElementMetaData,1);
  if ((!node) || (!elementMetaData))
    return (ElementMetaData *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      elementMetaData->seriesOrder = DecodeOrder(child0);
      break;
      
    case 2:
      elementMetaData->usageRight = DecodeUsage(child0);
      break;
      
    case 3:
      {
	struct hits_List92 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct hits_List92,1);
	  tmp->item = DecodeHitVector(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	elementMetaData->hits = head;
      }
      break;
      
    case 4:
      elementMetaData->displayName = DecodeInternationalString(child0);
      break;
      
    case 5:
      {
	struct supportedVariants_List93 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct supportedVariants_List93,1);
	  tmp->item = DecodeVariant(child1);
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	elementMetaData->supportedVariants = head;
      }
      break;
      
    case 6:
      elementMetaData->message = DecodeInternationalString(child0);
      break;
      
    case 7:
      elementMetaData->elementDescriptor = DecodeOctetString(child0);
      break;
      
    case 8:
      elementMetaData->surrogateFor = DecodeTagPath(child0);
      break;
      
    case 9:
      elementMetaData->surrogateElement = DecodeTagPath(child0);
      break;
      
    case 99:
      elementMetaData->other = DecodeExternal(child0);
      break;
      
    }
  }
  return elementMetaData;
}

TagPath *
DecodeTagPath(node)
     ASN_NODE *node;
{
  int tagno;
  struct TagPath *tmp, *head=NULL, *tail=NULL;
  ASN_NODE *child0;
  if (!node)
    return (TagPath *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tmp = CALLOC(struct TagPath,1);
    {
      ASN_NODE *child1;
      for(child1=child0->head; child1!=NULL; child1=child1->next) {
	tagno = child1->tagno;
	switch (tagno) {
	case 1:
	  tmp->item.tagType = DecodeInteger(child1);
	  break;
	  
	case 2:
	  {
	    ASN_NODE *child2 = child1->head;
	    tmp->item.tagValue = DecodeStringOrNumeric(child2);
	  }
	  break;
	  
	case 3:
	  tmp->item.tagOccurrence = DecodeInteger(child1);
	  break;
	  
	}
      }
    }
    if (head == NULL) {
      head = tmp;
      tail = tmp;
    } else {
      tail->next = tmp;
      tail = tmp;
    }
  }
  return head;
}

Order *
DecodeOrder(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Order *order;
  order = CALLOC(Order,1);
  if ((!node) || (!order))
    return (Order *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      order->ascending = DecodeBoolean(child0);
      break;
      
    case 2:
      order->order = DecodeInteger(child0);
      break;
      
    }
  }
  return order;
}

Usage *
DecodeUsage(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Usage *usage;
  usage = CALLOC(Usage,1);
  if ((!node) || (!usage))
    return (Usage *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      usage->type = DecodeInteger(child0);
      break;
      
    case 2:
      usage->restriction = DecodeInternationalString(child0);
      break;
      
    }
  }
  return usage;
}

HitVector *
DecodeHitVector(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  HitVector *hitVector;
  hitVector = CALLOC(HitVector,1);
  if ((!node) || (!hitVector))
    return (HitVector *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 45:
    case 215:
    case 216:
    case 217:
    case 218:
    case 219:
    case 220:
    case 221:
      hitVector->satisfier = DecodeTerm(child0);
      break;
      
    case 1:
      hitVector->offsetIntoElement = DecodeIntUnit(child0);
      break;
      
    case 2:
      hitVector->length = DecodeIntUnit(child0);
      break;
      
    case 3:
      hitVector->hitRank = DecodeInteger(child0);
      break;
      
    case 4:
      hitVector->targetToken = DecodeOctetString(child0);
      break;
      
    }
  }
  return hitVector;
}

Variant *
DecodeVariant(node)
     ASN_NODE *node;
{
  int tagno;
  ASN_NODE *child0;
  Variant *variant;
  variant = CALLOC(Variant,1);
  if ((!node) || (!variant))
    return (Variant *)NULL;
  for(child0=node->head; child0!=NULL; child0=child0->next) {
    tagno = child0->tagno;
    switch (tagno) {
    case 1:
      variant->globalVariantSetId = DecodeOid(child0);
      break;
      
    case 2:
      {
	struct triples_List94 *tmp, *head=NULL, *tail=NULL;
	ASN_NODE *child1;
	for(child1=child0->head; child1!=NULL; child1=child1->next) {
	  tmp = CALLOC(struct triples_List94,1);
	  {
            ASN_NODE *child2;
            for(child2=child1->head; child2!=NULL; child2=child2->next) {
	      tagno = child2->tagno;
	      switch (tagno) {
	      case 0:
		tmp->item.variantSetId = DecodeOid(child2);
		break;
		
	      case 1:
		tmp->item.class_var = DecodeInteger(child2);
		break;
		
	      case 2:
		tmp->item.type = DecodeInteger(child2);
		break;
		
	      case 3:
		{
		  ASN_NODE *child3 = child2->head;
		  tagno = child3->tagno;
		  switch (tagno) {
		  case INTEGER:
		    if (child3->class_var == UNIVERSAL) {
		      tmp->item.value.which = e47_intVal;
		      tmp->item.value.u.intVal = DecodeInteger(child3);
		    } else {
		      tmp->item.value.which = e47_valueAndUnit;
		      tmp->item.value.u.valueAndUnit = DecodeIntUnit(child3);
		    }
		    break;
		    
		  case GENERALSTRING:
		    tmp->item.value.which = e47_interStringVal;
		    tmp->item.value.u.interStringVal = DecodeInternationalString(child3);
		    break;
		    
		  case OCTETSTRING:
		    tmp->item.value.which = e47_octStringVal;
		    tmp->item.value.u.octStringVal = DecodeOctetString(child3);
		    break;
		    
		  case OBJECTIDENTIFIER:
		    tmp->item.value.which = e47_oidVal;
		    tmp->item.value.u.oidVal = DecodeOid(child3);
		    break;
		    
		  case ASN_BOOLEAN:
		    if (child3->class_var == UNIVERSAL) {
		      tmp->item.value.which = e47_boolVal;
		      tmp->item.value.u.boolVal = DecodeBoolean(child3);
		    } else {
		      tmp->item.value.which = e47_unit;
		      tmp->item.value.u.unit = DecodeUnit(child3);
		    }
		    break;
		    
		  case 0:
		    tmp->item.value.which = e47_nullVal;
		    tmp->item.value.u.nullVal = DecodeNull(child3);
		    break;
		    /* -----
		       case 1:
		       tmp->item.value.which = e47_unit;
		       tmp->item.value.u.unit = DecodeUnit(child3);
		       break;
		       
		       case 2:
		       tmp->item.value.which = e47_valueAndUnit;
		       tmp->item.value.u.valueAndUnit = DecodeIntUnit(child3);
		       break;
		       ------ */
		  }
		}
		break;
		
	      }
            }
	  }
	  if (head == NULL) {
	    head = tmp;
	    tail = tmp;
	  } else {
	    tail->next = tmp;
	    tail = tmp;
	  }
	}
	variant->triples = head;
      }
      break;
      
    }
  }
  return variant;
}
/* ------------
   TaskPackage *
   DecodeTaskPackage(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TaskPackage *taskPackage;
   taskPackage = CALLOC(TaskPackage,1);
   if ((!node) || (!taskPackage))
   return (TaskPackage *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   taskPackage->packageType = DecodeOid(child0);
   break;
   
   case 2:
   taskPackage->packageName = DecodeInternationalString(child0);
   break;
   
   case 3:
   taskPackage->userId = DecodeInternationalString(child0);
   break;
   
   case 4:
   taskPackage->retentionTime = DecodeIntUnit(child0);
   break;
   
   case 5:
   taskPackage->permissions = DecodePermissions(child0);
   break;
   
   case 6:
   taskPackage->description = DecodeInternationalString(child0);
   break;
   
   case 7:
   taskPackage->targetReference = DecodeOctetString(child0);
   break;
   
   case 8:
   taskPackage->creationDateTime = DecodeGeneralizedTime(child0);
   break;
   
   case 9:
   taskPackage->taskStatus = DecodeInteger(child0);
   break;
   
   case 10:
   {
   struct packageDiagnostics_List95 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct packageDiagnostics_List95,1);
   tmp->item = DecodeDiagRec(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   taskPackage->packageDiagnostics = head;
   }
   break;
   
   case 11:
   taskPackage->taskSpecificParameters = DecodeExternal(child0);
   break;
   
   }
   }
   return taskPackage;
   }
   
   ResourceReport *
   DecodeResourceReport(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   ResourceReport *resourceReport;
   resourceReport = CALLOC(ResourceReport,1);
   if ((!node) || (!resourceReport))
   return (ResourceReport *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   struct estimates_List96 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct estimates_List96,1);
   tmp->item = DecodeEstimate(child1);
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   resourceReport->estimates = head;
   }
   break;
   
   case 2:
   resourceReport->message = DecodeInternationalString(child0);
   break;
   
   }
   }
   return resourceReport;
   }
   
   Estimate *
   DecodeEstimate(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   Estimate *estimate;
   estimate = CALLOC(Estimate,1);
   if ((!node) || (!estimate))
   return (Estimate *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   estimate->type = DecodeEstimateType(child0);
   break;
   
   case 2:
   estimate->value = DecodeInteger(child0);
   break;
   
   case 3:
   estimate->currency_code = DecodeInteger(child0);
   break;
   
   }
   }
   return estimate;
   }
   
   
   ResourceReport *
   DecodeResourceReport(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   ResourceReport *resourceReport;
   resourceReport = CALLOC(ResourceReport,1);
   if ((!node) || (!resourceReport))
   return (ResourceReport *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   struct estimates_List97 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct estimates_List97,1);
   tmp->item = DecodeEstimate(child1);
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   resourceReport->estimates = head;
   }
   break;
   
   case 2:
   resourceReport->message = DecodeInternationalString(child0);
   break;
   
   }
   }
   return resourceReport;
   }
   
   Estimate *
   DecodeEstimate(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   Estimate *estimate;
   estimate = CALLOC(Estimate,1);
   if ((!node) || (!estimate))
   return (Estimate *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1 = child0->head;
   estimate->type = DecodeStringOrNumeric(child1);
   }
   break;
   
   case 2:
   estimate->value = DecodeIntUnit(child0);
   break;
   
   }
   }
   return estimate;
   }
   
   PromptObject *
   DecodePromptObject(node)
   ASN_NODE *node;
   {
   int tagno;
   PromptObject *promptObject;
   promptObject = CALLOC(PromptObject,1);
   if ((!node) || (!promptObject))
   return (PromptObject *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   promptObject->which = e47_challenge;
   promptObject->u.challenge = DecodeChallenge(child0);
   break;
   
   case 2:
   promptObject->which = e47_response;
   promptObject->u.response = DecodeResponse(child0);
   break;
   
   }
   return promptObject;
   }
   
   Challenge *
   DecodeChallenge(node)
   ASN_NODE *node;
   {
   int tagno;
   struct Challenge *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (Challenge *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct Challenge,1);
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item.promptId = DecodePromptId(child2);
   }
   break;
   
   case 2:
   tmp->item.defaultResponse = DecodeInternationalString(child1);
   break;
   
   case 3:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   tmp->item.promptInfo->which = e48_character;
   tmp->item.promptInfo->u.character = DecodeInternationalString(child2);
   break;
   
   case 2:
   tmp->item.promptInfo->which = e48_encrypted;
   tmp->item.promptInfo->u.encrypted = DecodeEncryption(child2);
   break;
   
   }
   }
   break;
   
   case 4:
   tmp->item.regExpr = DecodeInternationalString(child1);
   break;
   
   case 5:
   tmp->item.u.responseRequired = DecodeNull(child1);
   break;
   
   case 6:
   {
   struct allowedValues_List98 *tmp2, *head=NULL, *tail=NULL;
   ASN_NODE *child2;
   for(child2=child1->head; child2!=NULL; child2=child2->next) {
   tmp2 = CALLOC(struct allowedValues_List98,1);
   tmp2->item = DecodeInternationalString(child2);
   
   if (head == NULL) {
   head = tmp2;
   tail = tmp2;
   } else {
   tail->next = tmp2;
   tail = tmp2;
   }
   }
   tmp->item.allowedValues = head;
   }
   break;
   
   case 7:
   tmp->item.shouldSave = DecodeNull(child1);
   break;
   
   case 8:
   tmp->item.dataType = DecodeInteger(child1);
   break;
   
   case 9:
   tmp->item.diagnostic = DecodeExternal(child1);
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   Response *
   DecodeResponse(node)
   ASN_NODE *node;
   {
   int tagno;
   struct Response *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (Response *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct Response,1);
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item.promptId = DecodePromptId(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   tmp->item.promptResponse.which = e49_string;
   tmp->item.promptResponse.u.string = DecodeInternationalString(child2);
   break;
   
   case 2:
   tmp->item.promptResponse.which = e49_accept;
   tmp->item.promptResponse.u.accept = DecodeBoolean(child2);
   break;
   
   case 3:
   tmp->item.promptResponse.which = e49_acknowledge;
   tmp->item.promptResponse.u.acknowledge = DecodeNull(child2);
   break;
   
   case 4:
   tmp->item.promptResponse.which = e49_diagnostic;
   {
   ASN_NODE *child3 = child2->head;
   tmp->item.promptResponse.u.diagnostic = DecodeDiagRec(child3);
   }
   break;
   
   case 5:
   tmp->item.promptResponse.which = e49_encrypted;
   tmp->item.promptResponse.u.encrypted = DecodeEncryption(child2);
   break;
   
   }
   }
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   PromptId *
   DecodePromptId(node)
   ASN_NODE *node;
   {
   int tagno;
   PromptId *promptId;
   promptId = CALLOC(PromptId,1);
   if ((!node) || (!promptId))
   return (PromptId *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   promptId->which = e50_enummeratedPrompt;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   promptId->u.enummeratedPrompt.type = DecodeInteger(child1);
   break;
   
   case 2:
   promptId->u.enummeratedPrompt.suggestedString = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   break;
   
   case 2:
   promptId->which = e50_nonEnumeratedPrompt;
   promptId->u.nonEnumeratedPrompt = DecodeInternationalString(child0);
   break;
   
   }
   return promptId;
   }
   
   Encryption *
   DecodeEncryption(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   Encryption *encryption;
   encryption = CALLOC(Encryption,1);
   if ((!node) || (!encryption))
   return (Encryption *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   encryption->cryptType = DecodeOctetString(child0);
   break;
   
   case 2:
   encryption->credential = DecodeOctetString(child0);
   break;
   
   case 3:
   encryption->data = DecodeOctetString(child0);
   break;
   
   }
   }
   return encryption;
   }
   
   DES_RN_Object *
   DecodeDES_RN_Object(node)
   ASN_NODE *node;
   {
   int tagno;
   DES_RN_Object *dES_RN_Object;
   dES_RN_Object = CALLOC(DES_RN_Object,1);
   if ((!node) || (!dES_RN_Object))
   return (DES_RN_Object *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   dES_RN_Object->which = e51_challenge;
   dES_RN_Object->u.challenge = DecodeDRNType(child0);
   break;
   
   case 2:
   dES_RN_Object->which = e51_response;
   dES_RN_Object->u.response = DecodeDRNType(child0);
   break;
   
   }
   return dES_RN_Object;
   }
   
   DRNType *
   DecodeDRNType(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   DRNType *dRNType;
   dRNType = CALLOC(DRNType,1);
   if ((!node) || (!dRNType))
   return (DRNType *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   dRNType->userId = DecodeOctetString(child0);
   break;
   
   case 2:
   dRNType->salt = DecodeOctetString(child0);
   break;
   
   case 3:
   dRNType->randomNumber = DecodeOctetString(child0);
   break;
   
   }
   }
   return dRNType;
   }
   
   KRBObject *
   DecodeKRBObject(node)
   ASN_NODE *node;
   {
   int tagno;
   KRBObject *kRBObject;
   kRBObject = CALLOC(KRBObject,1);
   if ((!node) || (!kRBObject))
   return (KRBObject *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   kRBObject->which = e52_challenge;
   kRBObject->u.challenge = DecodeKRBRequest(child0);
   break;
   
   case 2:
   kRBObject->which = e52_response;
   kRBObject->u.response = DecodeKRBResponse(child0);
   break;
   
   }
   return kRBObject;
   }
   
   KRBRequest *
   DecodeKRBRequest(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   KRBRequest *kRBRequest;
   kRBRequest = CALLOC(KRBRequest,1);
   if ((!node) || (!kRBRequest))
   return (KRBRequest *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   kRBRequest->service = DecodeInternationalString(child0);
   break;
   
   case 2:
   kRBRequest->instance = DecodeInternationalString(child0);
   break;
   
   case 3:
   kRBRequest->realm = DecodeInternationalString(child0);
   break;
   
   }
   }
   return kRBRequest;
   }
   
   KRBResponse *
   DecodeKRBResponse(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   KRBResponse *kRBResponse;
   kRBResponse = CALLOC(KRBResponse,1);
   if ((!node) || (!kRBResponse))
   return (KRBResponse *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   kRBResponse->userid = DecodeInternationalString(child0);
   break;
   
   case 2:
   kRBResponse->ticket = DecodeOctetString(child0);
   break;
   
   }
   }
   return kRBResponse;
   }
   
   PersistentResultSet *
   DecodePersistentResultSet(node)
   ASN_NODE *node;
   {
   int tagno;
   PersistentResultSet *persistentResultSet;
   persistentResultSet = CALLOC(PersistentResultSet,1);
   if ((!node) || (!persistentResultSet))
   return (PersistentResultSet *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   persistentResultSet->which = e53_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   persistentResultSet->u.esRequest.toKeep = DecodeNull(child1);
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   persistentResultSet->u.esRequest.notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   persistentResultSet->which = e53_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   persistentResultSet->u.taskPackage.originPart = DecodeNull(child1);
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   persistentResultSet->u.taskPackage.targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   }
   return persistentResultSet;
   }
   
   OriginPartNotToKeep *
   DecodeOriginPartNotToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartNotToKeep *originPartNotToKeep;
   originPartNotToKeep = CALLOC(OriginPartNotToKeep,1);
   if ((!node) || (!originPartNotToKeep))
   return (OriginPartNotToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartNotToKeep->originSuppliedResultSet = DecodeInternationalString(child0);
   break;
   
   case 2:
   originPartNotToKeep->replaceOrAppend = DecodeInteger(child0);
   break;
   
   }
   }
   return originPartNotToKeep;
   }
   
   TargetPart *
   DecodeTargetPart(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TargetPart *targetPart;
   targetPart = CALLOC(TargetPart,1);
   if ((!node) || (!targetPart))
   return (TargetPart *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   targetPart->targetSuppliedResultSet = DecodeInternationalString(child0);
   break;
   
   case 2:
   targetPart->numberOfRecords = DecodeInteger(child0);
   break;
   
   }
   }
   return targetPart;
   }
   
   PersistentQuery *
   DecodePersistentQuery(node)
   ASN_NODE *node;
   {
   int tagno;
   PersistentQuery *persistentQuery;
   persistentQuery = CALLOC(PersistentQuery,1);
   if ((!node) || (!persistentQuery))
   return (PersistentQuery *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   persistentQuery->which = e54_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   persistentQuery->u.esRequest.toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   persistentQuery->u.esRequest.notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   persistentQuery->which = e54_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   persistentQuery->u.taskPackage.originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   persistentQuery->u.taskPackage.targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   }
   return persistentQuery;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 2:
   {
   struct dbNames_List99 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct dbNames_List99,1);
   tmp->item = DecodeInternationalString(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   originPartToKeep->dbNames = head;
   }
   break;
   
   case 3:
   {
   ASN_NODE *child1 = child0->head;
   originPartToKeep->additionalSearchInfo = DecodeOtherInformation(child1);
   }
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   OriginPartNotToKeep *
   DecodeOriginPartNotToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   OriginPartNotToKeep *originPartNotToKeep;
   originPartNotToKeep = CALLOC(OriginPartNotToKeep,1);
   if ((!node) || (!originPartNotToKeep))
   return (OriginPartNotToKeep *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   originPartNotToKeep->which = e55_package;
   originPartNotToKeep->u.package = DecodeInternationalString(child0);
   break;
   
   case 2:
   originPartNotToKeep->which = e55_query;
   {
   ASN_NODE *child1 = child0->head;
   originPartNotToKeep->u.query = DecodeQuery(child1);
   }
   break;
   
   }
   return originPartNotToKeep;
   }
   
   
   PeriodicQuerySchedule *
   DecodePeriodicQuerySchedule(node)
   ASN_NODE *node;
   {
   int tagno;
   PeriodicQuerySchedule *periodicQuerySchedule;
   periodicQuerySchedule = CALLOC(PeriodicQuerySchedule,1);
   if ((!node) || (!periodicQuerySchedule))
   return (PeriodicQuerySchedule *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   periodicQuerySchedule->which = e56_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   periodicQuerySchedule->u.esRequest.toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   periodicQuerySchedule->u.esRequest.notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   periodicQuerySchedule->which = e56_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   periodicQuerySchedule->u.taskPackage.originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   periodicQuerySchedule->u.taskPackage.targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break; 
   
   }
   return periodicQuerySchedule;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->activeFlag = DecodeBoolean(child0);
   break;
   
   case 2:
   {
   struct databaseNames_List100 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct databaseNames_List100,1);
   tmp->item = DecodeInternationalString(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   originPartToKeep->databaseNames = head;
   }
   break;
   
   case 3:
   originPartToKeep->resultSetDisposition = DecodeInteger(child0);
   break;
   
   case 4:
   {
   ASN_NODE *child1 = child0->head;
   originPartToKeep->alertDestination = DecodeDestination(child1);
   }
   break;
   
   case 5:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->exportParameters.which = e57_packageName;
   originPartToKeep->exportParameters.u.packageName = DecodeInternationalString(child1);
   break;
   
   case 2:
   originPartToKeep->exportParameters.which = e57_exportPackage;
   {
   ASN_NODE *child2 = child1->head;
   originPartToKeep->exportParameters.u.exportPackage = DecodeExportSpecification(child2);
   }
   break;
   
   }
   }
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   OriginPartNotToKeep *
   DecodeOriginPartNotToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartNotToKeep *originPartNotToKeep;
   originPartNotToKeep = CALLOC(OriginPartNotToKeep,1);
   if ((!node) || (!originPartNotToKeep))
   return (OriginPartNotToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartNotToKeep->querySpec->which = e58_actualQuery;
   {
   ASN_NODE *child2 = child1->head;
   originPartNotToKeep->querySpec->u.actualQuery = DecodeQuery(child2);
   }
   break;
   
   case 2:
   originPartNotToKeep->querySpec->which = e58_packageName;
   originPartNotToKeep->querySpec->u.packageName = DecodeInternationalString(child1);
   break;
   
   }
   }
   break;
   
   case 2:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   originPartNotToKeep->originSuggestedPeriod = DecodePeriod(child1);
   }
   break;
   
   case 3:
   originPartNotToKeep->expiration = DecodeGeneralizedTime(child0);
   break;
   
   case 4:
   originPartNotToKeep->resultSetPackage = DecodeInternationalString(child0);
   break;
   
   }
   }
   return originPartNotToKeep;
   }
   
   TargetPart *
   DecodeTargetPart(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TargetPart *targetPart;
   targetPart = CALLOC(TargetPart,1);
   if ((!node) || (!targetPart))
   return (TargetPart *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1 = child0->head;
   targetPart->actualQuery = DecodeQuery(child1);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child1 = child0->head;
   targetPart->targetStatedPeriod = DecodePeriod(child1);
   }
   break;
   
   case 3:
   targetPart->expiration = DecodeGeneralizedTime(child0);
   break;
   
   case 4:
   targetPart->resultSetPackage = DecodeInternationalString(child0);
   break;
   
   case 5:
   targetPart->lastQueryTime = DecodeGeneralizedTime(child0);
   break;
   
   case 6:
   targetPart->lastResultNumber = DecodeInteger(child0);
   break;
   
   case 7:
   targetPart->numberSinceModify = DecodeInteger(child0);
   break;
   
   }
   }
   return targetPart;
   }
   
   Period *
   DecodePeriod(node)
   ASN_NODE *node;
   {
   int tagno;
   Period *period;
   period = CALLOC(Period,1);
   if ((!node) || (!period))
   return (Period *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   period->which = e59_unit;
   period->u.unit = DecodeIntUnit(child0);
   break;
   
   case 2:
   period->which = e59_businessDaily;
   period->u.businessDaily = DecodeNull(child0);
   break;
   
   case 3:
   period->which = e59_continuous;
   period->u.continuous = DecodeNull(child0);
   break;
   
   case 4:
   period->which = e59_other;
   period->u.other = DecodeInternationalString(child0);
   break;
   
   }
   return period;
   }
   
   ItemOrder *
   DecodeItemOrder(node)
   ASN_NODE *node;
   {
   int tagno;
   ItemOrder *itemOrder;
   itemOrder = CALLOC(ItemOrder,1);
   if ((!node) || (!itemOrder))
   return (ItemOrder *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   itemOrder->which = e60_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   itemOrder->u.esRequest->toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   itemOrder->u.esRequest->notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   itemOrder->which = e60_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   itemOrder->u.taskPackage->originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   itemOrder->u.taskPackage->targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   }
   return itemOrder;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->supplDescription = DecodeExternal(child0);
   break;
   
   case 2:
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->contact->name = DecodeInternationalString(child1);
   break;
   
   case 2:
   originPartToKeep->contact->phone = DecodeInternationalString(child1);
   break;
   
   case 3:
   originPartToKeep->contact->email = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   break;
   
   case 3:
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 0:
   originPartToKeep->addlBilling->paymentMethod->which = e61_billInvoice;
   originPartToKeep->addlBilling->paymentMethod->u.billInvoice = DecodeNull(child2);
   break;
   
   case 1:
   originPartToKeep->addlBilling->paymentMethod->which = e61_prepay;
   originPartToKeep->addlBilling->paymentMethod->u.prepay = DecodeNull(child2);
   break;
   
   case 2:
   originPartToKeep->addlBilling->paymentMethod->which = e61_depositAccount;
   originPartToKeep->addlBilling->paymentMethod->u.depositAccount = DecodeNull(child2);
   break;
   
   case 3:
   originPartToKeep->addlBilling->paymentMethod->which = e61_creditCard;
   originPartToKeep->addlBilling->paymentMethod->u.creditCard = DecodeCreditCardInfo(child2);
   break;
   
   case 4:
   originPartToKeep->addlBilling->paymentMethod->which = e61_cardInfoPreviouslySupplied;
   originPartToKeep->addlBilling->paymentMethod->u.cardInfoPreviouslySupplied = DecodeNull(child2);
   break;
   
   case 5:
   originPartToKeep->addlBilling->paymentMethod->which = e61_privateKnown;
   originPartToKeep->addlBilling->paymentMethod->u.privateKnown = DecodeNull(child2);
   break;
   
   case 6:
   originPartToKeep->addlBilling->paymentMethod->which = e61_privateNotKnown;
   originPartToKeep->addlBilling->paymentMethod->u.privateNotKnown = DecodeExternal(child2);
   break;
   
   }
   }
   break;
   
   case 2:
   originPartToKeep->addlBilling->customerReference = DecodeInternationalString(child1);
   break;
   
   case 3:
   originPartToKeep->addlBilling->customerPONumber = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   CreditCardInfo *
   DecodeCreditCardInfo(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   CreditCardInfo *creditCardInfo;
   creditCardInfo = CALLOC(CreditCardInfo,1);
   if ((!node) || (!creditCardInfo))
   return (CreditCardInfo *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   creditCardInfo->nameOnCard = DecodeInternationalString(child0);
   break;
   
   case 2:
   creditCardInfo->expirationDate = DecodeInternationalString(child0);
   break;
   
   case 3:
   creditCardInfo->cardNumber = DecodeInternationalString(child0);
   break;
   
   }
   }
   return creditCardInfo;
   }
   
   OriginPartNotToKeep *
   DecodeOriginPartNotToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartNotToKeep *originPartNotToKeep;
   originPartNotToKeep = CALLOC(OriginPartNotToKeep,1);
   if ((!node) || (!originPartNotToKeep))
   return (OriginPartNotToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartNotToKeep->resultSetItem->resultSetId = DecodeInternationalString(child1);
   break;
   
   case 2:
   originPartNotToKeep->resultSetItem->item = DecodeInteger(child1);
   break;
   
   }
   }
   }
   break;
   
   case 2:
   originPartNotToKeep->itemRequest = DecodeExternal(child0);
   break;
   
   }
   }
   return originPartNotToKeep;
   }
   
   TargetPart *
   DecodeTargetPart(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TargetPart *targetPart;
   targetPart = CALLOC(TargetPart,1);
   if ((!node) || (!targetPart))
   return (TargetPart *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   targetPart->itemRequest = DecodeExternal(child0);
   break;
   
   case 2:
   targetPart->statusOrErrorReport = DecodeExternal(child0);
   break;
   
   case 3:
   targetPart->auxiliaryStatus = DecodeInteger(child0);
   break;
   
   }
   }
   return targetPart;
   }
   
   Update *
   DecodeUpdate(node)
   ASN_NODE *node;
   {
   int tagno;
   Update *update;
   update = CALLOC(Update,1);
   if ((!node) || (!update))
   return (Update *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   update->which = e62_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   update->u.esRequest.toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   update->u.esRequest.notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   update->which = e62_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   update->u.taskPackage.originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   update->u.taskPackage.targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   }
   return update;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->action = DecodeInteger(child0);
   break;
   
   case 2:
   originPartToKeep->databaseName = DecodeInternationalString(child0);
   break;
   
   case 3:
   originPartToKeep->schema = DecodeOid(child0);
   break;
   
   case 4:
   originPartToKeep->elementSetName = DecodeInternationalString(child0);
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   
   TargetPart *
   DecodeTargetPart(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TargetPart *targetPart;
   targetPart = CALLOC(TargetPart,1);
   if ((!node) || (!targetPart))
   return (TargetPart *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   targetPart->updateStatus = DecodeInteger(child0);
   break;
   
   case 2:
   {
   struct globalDiagnostics_List101 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct globalDiagnostics_List101,1);
   tmp->item = DecodeDiagRec(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   targetPart->globalDiagnostics = head;
   }
   break;
   
   case 3:
   {
   struct taskPackageRecords_List102 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct taskPackageRecords_List102,1);
   tmp->item = DecodeTaskPackageRecordStructure(child1);
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   targetPart->taskPackageRecords = head;
   }
   break;
   
   }
   }
   return targetPart;
   }
   
   SuppliedRecords *
   DecodeSuppliedRecords(node)
   ASN_NODE *node;
   {
   int tagno;
   struct SuppliedRecords *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (SuppliedRecords *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct SuppliedRecords,1);
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   tmp->item->recordId->which = e63_number;
   tmp->item->recordId->u.number = DecodeInteger(child2);
   break;
   
   case 2:
   tmp->item->recordId->which = e63_string;
   tmp->item->recordId->u.string = DecodeInternationalString(child2);
   break;
   
   case 3:
   tmp->item->recordId->which = e63_opaque;
   tmp->item->recordId->u.opaque = DecodeOctetString(child2);
   break;
   
   }
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   tmp->item->supplementalId->which = e64_timeStamp;
   tmp->item->supplementalId->u.timeStamp = DecodeGeneralizedTime(child2);
   break;
   
   case 2:
   tmp->item->supplementalId->which = e64_versionNumber;
   tmp->item->supplementalId->u.versionNumber = DecodeInternationalString(child2);
   break;
   
   case 3:
   tmp->item->supplementalId->which = e64_previousVersion;
   tmp->item->supplementalId->u.previousVersion = DecodeExternal(child2);
   break;
   
   }
   }
   break;
   
   case 3:
   tmp->item->correlationInfo = DecodeCorrelationInfo(child1);
   break;
   
   case 4:
   tmp->item->u.record = DecodeExternal(child1);
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   CorrelationInfo *
   DecodeCorrelationInfo(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   CorrelationInfo *correlationInfo;
   correlationInfo = CALLOC(CorrelationInfo,1);
   if ((!node) || (!correlationInfo))
   return (CorrelationInfo *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   correlationInfo->note = DecodeInternationalString(child0);
   break;
   
   case 2:
   correlationInfo->id = DecodeInteger(child0);
   break;
   
   }
   }
   return correlationInfo;
   }
   
   TaskPackageRecordStructure *
   DecodeTaskPackageRecordStructure(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TaskPackageRecordStructure *taskPackageRecordStructure;
   taskPackageRecordStructure = CALLOC(TaskPackageRecordStructure,1);
   if ((!node) || (!taskPackageRecordStructure))
   return (TaskPackageRecordStructure *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   taskPackageRecordStructure->recordOrSurDiag->which = e65_record;
   taskPackageRecordStructure->recordOrSurDiag->u.record = DecodeExternal(child1);
   break;
   
   case 2:
   taskPackageRecordStructure->recordOrSurDiag->which = e65_diagnostic;
   {
   ASN_NODE *child2 = child1->head;
   taskPackageRecordStructure->recordOrSurDiag->u.diagnostic = DecodeDiagRec(child2);
   }
   break;
   
   }
   }
   break;
   
   case 2:
   taskPackageRecordStructure->correlationInfo = DecodeCorrelationInfo(child0);
   break;
   
   case 3:
   taskPackageRecordStructure->recordStatus = DecodeInteger(child0);
   break;
   
   }
   }
   return taskPackageRecordStructure;
   }
   
   ExportSpecification *
   DecodeExportSpecification(node)
   ASN_NODE *node;
   {
   int tagno;
   ExportSpecification *exportSpecification;
   exportSpecification = CALLOC(ExportSpecification,1);
   if ((!node) || (!exportSpecification))
   return (ExportSpecification *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   exportSpecification->which = e66_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   exportSpecification->u.esRequest.toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   exportSpecification->u.esRequest.notToKeep = DecodeNull(child1);
   break;
   
   }
   }
   }
   break;
   
   case 2:
   exportSpecification->which = e66_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   exportSpecification->u.taskPackage.originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   exportSpecification->u.taskPackage.targetPart = DecodeNull(child1);
   break;
   
   }
   }
   }
   break;
   
   }
   return exportSpecification;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->composition = DecodeCompSpec(child0);
   break;
   
   case 2:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   originPartToKeep->exportDestination = DecodeDestination(child1);
   }
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   Destination *
   DecodeDestination(node)
   ASN_NODE *node;
   {
   int tagno;
   Destination *destination;
   destination = CALLOC(Destination,1);
   if ((!node) || (!destination))
   return (Destination *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   destination->which = e67_phoneNumber;
   destination->u.phoneNumber = DecodeInternationalString(child0);
   break;
   
   case 2:
   destination->which = e67_faxNumber;
   destination->u.faxNumber = DecodeInternationalString(child0);
   break;
   
   case 3:
   destination->which = e67_x400address;
   destination->u.x400address = DecodeInternationalString(child0);
   break;
   
   case 4:
   destination->which = e67_emailAddress;
   destination->u.emailAddress = DecodeInternationalString(child0);
   break;
   
   case 5:
   destination->which = e67_pagerNumber;
   destination->u.pagerNumber = DecodeInternationalString(child0);
   break;
   
   case 6:
   destination->which = e67_ftpAddress;
   destination->u.ftpAddress = DecodeInternationalString(child0);
   break;
   
   case 7:
   destination->which = e67_ftamAddress;
   destination->u.ftamAddress = DecodeInternationalString(child0);
   break;
   
   case 8:
   destination->which = e67_printerAddress;
   destination->u.printerAddress = DecodeInternationalString(child0);
   break;
   
   case 100:
   destination->which = e67_other;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   destination->u.other.vehicle = DecodeInternationalString(child1);
   break;
   
   case 2:
   destination->u.other.destination = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   break;
   
   }
   return destination;
   }
   
   ExportInvocation *
   DecodeExportInvocation(node)
   ASN_NODE *node;
   {
   int tagno;
   ExportInvocation *exportInvocation;
   exportInvocation = CALLOC(ExportInvocation,1);
   if ((!node) || (!exportInvocation))
   return (ExportInvocation *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   exportInvocation->which = e68_esRequest;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   exportInvocation->u.esRequest.toKeep = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   exportInvocation->u.esRequest.notToKeep = DecodeOriginPartNotToKeep(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   exportInvocation->which = e68_taskPackage;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   exportInvocation->u.taskPackage.originPart = DecodeOriginPartToKeep(child2);
   }
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   exportInvocation->u.taskPackage.targetPart = DecodeTargetPart(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   }
   return exportInvocation;
   }
   
   OriginPartToKeep *
   DecodeOriginPartToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartToKeep *originPartToKeep;
   originPartToKeep = CALLOC(OriginPartToKeep,1);
   if ((!node) || (!originPartToKeep))
   return (OriginPartToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartToKeep->exportSpec.which = e69_packageName;
   originPartToKeep->exportSpec.u.packageName = DecodeInternationalString(child1);
   break;
   
   case 2:
   originPartToKeep->exportSpec.which = e69_packageSpec;
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   originPartToKeep->exportSpec.u.packageSpec = DecodeExportSpecification(child2);
   }
   break;
   
   }
   }
   break;
   
   case 2:
   originPartToKeep->numberOfCopies = DecodeInteger(child0);
   break;
   
   }
   }
   return originPartToKeep;
   }
   
   OriginPartNotToKeep *
   DecodeOriginPartNotToKeep(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   OriginPartNotToKeep *originPartNotToKeep;
   originPartNotToKeep = CALLOC(OriginPartNotToKeep,1);
   if ((!node) || (!originPartNotToKeep))
   return (OriginPartNotToKeep *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   originPartNotToKeep->resultSetId = DecodeInternationalString(child0);
   break;
   
   case 2:
   {
   ASN_NODE *child1 = child0->head;
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   originPartNotToKeep->records.which = e70_all;
   originPartNotToKeep->records.u.all = DecodeNull(child1);
   break;
   
   case 2:
   originPartNotToKeep->records.which = e70_ranges;
   {
   struct ranges_List103 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child2;
   for(child2=child1->head; child2!=NULL; child2=child2->next) {
   tmp = CALLOC(struct ranges_List103,1);
   {
   ASN_NODE *child3;
   for(child3=node->head; child3!=NULL; child3=child3->next) {
   tagno = child3->tagno;
   switch (tagno) {
   case 1:
   tmp->item.u.start = DecodeInteger(child3);
   break;
   
   case 2:
   tmp->item.u.count = DecodeInteger(child3);
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   originPartNotToKeep->records.u.ranges = head;
   }
   break;
   
   }
   }
   break;
   
   }
   }
   return originPartNotToKeep;
   }
   
   TargetPart *
   DecodeTargetPart(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   TargetPart *targetPart;
   targetPart = CALLOC(TargetPart,1);
   if ((!node) || (!targetPart))
   return (TargetPart *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   targetPart->estimatedQuantity = DecodeIntUnit(child0);
   break;
   
   case 2:
   targetPart->quantitySoFar = DecodeIntUnit(child0);
   break;
   
   case 3:
   targetPart->estimatedCost = DecodeIntUnit(child0);
   break;
   
   case 4:
   targetPart->costSoFar = DecodeIntUnit(child0);
   break;
   
   }
   }
   return targetPart;
   }
   
   SearchInfoReport *
   DecodeSearchInfoReport(node)
   ASN_NODE *node;
   {
   int tagno;
   struct SearchInfoReport *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (SearchInfoReport *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct SearchInfoReport,1);
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   tmp->item.subqueryId = DecodeInternationalString(child1);
   break;
   
   case 2:
   tmp->item.u.fullQuery = DecodeBoolean(child1);
   break;
   
   case 3:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item.subqueryExpression = DecodeQueryExpression(child2);
   }
   break;
   
   case 4:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item.subqueryInterpretation = DecodeQueryExpression(child2);
   }
   break;
   
   case 5:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item.subqueryRecommendation = DecodeQueryExpression(child2);
   }
   break;
   
   case 6:
   tmp->item.u.subqueryCount = DecodeInteger(child1);
   break;
   
   case 7:
   tmp->item.subqueryWeight = DecodeIntUnit(child1);
   break;
   
   case 8:
   tmp->item.resultsByDB = DecodeResultsByDB(child1);
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   ResultsByDB *
   DecodeResultsByDB(node)
   ASN_NODE *node;
   {
   int tagno;
   struct ResultsByDB *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (ResultsByDB *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct ResultsByDB,1);
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   tmp->item.databases.which = e71_all;
   tmp->item.databases.u.all = DecodeNull(child2);
   break;
   
   case 2:
   tmp->item.databases.which = e71_list;
   {
   struct list_List104 *tmp2, *head=NULL, *tail=NULL;
   ASN_NODE *child3;
   for(child3=node->head; child3!=NULL; child3=child3->next) {
   tmp2 = CALLOC(struct list_List104,1);
   tmp2->item = DecodeDatabaseName(child3);
   
   if (head == NULL) {
   head = tmp2;
   tail = tmp2;
   } else {
   tail->next = tmp2;
   tail = tmp2;
   }
   }
   tmp->item.databases.u.list = head;
   }
   break;
   
   }
   }
   break;
   
   case 2:
   tmp->item.u.count = DecodeInteger(child1);
   break;
   
   case 3:
   tmp->item.resultSetName = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   QueryExpression *
   DecodeQueryExpression(node)
   ASN_NODE *node;
   {
   int tagno;
   QueryExpression *queryExpression;
   queryExpression = CALLOC(QueryExpression,1);
   if ((!node) || (!queryExpression))
   return (QueryExpression *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   queryExpression->which = e72_term;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   queryExpression->u.term->queryTerm = DecodeTerm(child2);
   }
   break;
   
   case 2:
   queryExpression->u.term->termComment = DecodeInternationalString(child1);
   break;
   
   }
   }
   }
   break;
   
   case 2:
   queryExpression->which = e72_query;
   {
   ASN_NODE *child1 = child0->head;
   queryExpression->u.query = DecodeQuery(child1);
   }
   break;
   
   }
   return queryExpression;
   }
   
   Espec_1 *
   DecodeEspec_1(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   Espec_1 *espec_1;
   espec_1 = CALLOC(Espec_1,1);
   if ((!node) || (!espec_1))
   return (Espec_1 *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   {
   struct elementSetNames_List105 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct elementSetNames_List105,1);
   tmp->item = DecodeInternationalString(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   espec_1->elementSetNames = head;
   }
   break;
   
   case 2:
   espec_1->defaultVariantSetId = DecodeOid(child0);
   break;
   
   case 3:
   espec_1->defaultVariantRequest = DecodeVariant(child0);
   break;
   
   case 4:
   espec_1->defaultTagType = DecodeInteger(child0);
   break;
   
   case 5:
   {
   struct elements_List106 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tmp = CALLOC(struct elements_List106,1);
   tmp->item = DecodeElementRequest(child1);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   espec_1->elements = head;
   }
   break;
   
   }
   }
   return espec_1;
   }
   
   ElementRequest *
   DecodeElementRequest(node)
   ASN_NODE *node;
   {
   int tagno;
   ElementRequest *elementRequest;
   elementRequest = CALLOC(ElementRequest,1);
   if ((!node) || (!elementRequest))
   return (ElementRequest *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   elementRequest->which = e73_simpleElement;
   elementRequest->u.simpleElement = DecodeSimpleElement(child0);
   break;
   
   case 2:
   elementRequest->which = e73_compositeElement;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   {
   ASN_NODE *child2 = child1->head;
   tagno = child2->tagno;
   switch (tagno) {
   case 1:
   elementRequest->u.compositeElement.elementList.which = e74_primitives;
   {
   struct primitives_List107 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child3;
   for(child3=child2->head; child3!=NULL; child3=child3->next) {
   tmp = CALLOC(struct primitives_List107,1);
   tmp->item = DecodeInternationalString(child3);
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   elementRequest->u.compositeElement.elementList.u.primitives = head;
   }
   break;
   
   case 2:
   elementRequest->u.compositeElement.elementList.which = e74_specs;
   {
   struct specs_List108 *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child3;
   for(child3=child2->head; child3!=NULL; child3=child3->next) {
   tmp = CALLOC(struct specs_List108,1);
   tmp->item = DecodeSimpleElement(child3);
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   elementRequest->u.compositeElement.elementList.u.specs = head;
   }
   break;
   
   }
   }
   break;
   
   case 2:
   elementRequest->u.compositeElement.deliveryTag = DecodeTagPath(child1);
   break;
   
   case 3:
   elementRequest->u.compositeElement.variantRequest = DecodeVariant(child1);
   break;
   
   }
   }
   }
   break;
   
   }
   return elementRequest;
   }
   
   SimpleElement *
   DecodeSimpleElement(node)
   ASN_NODE *node;
   {
   int tagno;
   ASN_NODE *child0;
   SimpleElement *simpleElement;
   simpleElement = CALLOC(SimpleElement,1);
   if ((!node) || (!simpleElement))
   return (SimpleElement *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   simpleElement->path = DecodeTagPath(child0);
   break;
   
   case 2:
   simpleElement->variantRequest = DecodeVariant(child0);
   break;
   
   }
   }
   return simpleElement;
   }
   
   TagPath *
   DecodeTagPath(node)
   ASN_NODE *node;
   {
   int tagno;
   struct TagPath *tmp, *head=NULL, *tail=NULL;
   ASN_NODE *child0;
   if (!node)
   return (TagPath *)NULL;
   for(child0=node->head; child0!=NULL; child0=child0->next) {
   tmp = CALLOC(struct TagPath,1);
   tagno = child0->tagno;
   switch (tagno) {
   case 1:
   tmp->item->which = e74_specificTag;
   {
   ASN_NODE *child1;
   for(child1=child0->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   tmp->item->u.specificTag.tagType = DecodeInteger(child1);
   break;
   
   case 2:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item->u.specificTag.tagValue = DecodeStringOrNumeric(child2);
   }
   break;
   
   case 3:
   {
   ASN_NODE *child2 = child1->head;
   tmp->item->u.specificTag.occurrence = DecodeOccurrences(child2);
   }
   break;
   
   }
   }
   }
   break;
   
   case 2:
   tmp->item->which = e74_wildThing;
   {
   ASN_NODE *child1 = child0->head;
   tmp->item->u.wildThing = DecodeOccurrences(child1);
   }
   break;
   
   case 3:
   tmp->item->which = e74_wildPath;
   tmp->item->u.wildPath = DecodeNull(child0);
   break;
   }
   
   if (head == NULL) {
   head = tmp;
   tail = tmp;
   } else {
   tail->next = tmp;
   tail = tmp;
   }
   }
   return head;
   }
   
   Occurrences *
   DecodeOccurrences(node)
   ASN_NODE *node;
   {
   int tagno;
   Occurrences *occurrences;
   occurrences = CALLOC(Occurrences,1);
   if ((!node) || (!occurrences))
   return (Occurrences *)NULL;
   tagno = node->tagno;
   switch(tagno) {
   case 1:
   occurrences->which = e75_all;
   occurrences->u.all = DecodeNull(child0);
   break;
   
   case 2:
   occurrences->which = e75_last;
   occurrences->u.last = DecodeNull(child0);
   break;
   
   case 3:
   occurrences->which = e75_values;
   {
   ASN_NODE *child1;
   for(child1=node->head; child1!=NULL; child1=child1->next) {
   tagno = child1->tagno;
   switch (tagno) {
   case 1:
   occurrences->u.values.start = DecodeInteger(child1);
   break;
   
   case 2:
   occurrences->u.values.howMany = DecodeInteger(child1);
   break;
   
   }
   }
   }
   break;
   
   }
   return occurrences;
   }
   ----------- */

void
DecodeASN1Records(external, node)
     External *external;
     ASN_NODE *node;
{
  ASN_NODE *child0;
  char *oid;
  
  if (external == NULL || node == NULL)
    return;
  
  child0 = node->head;
  
  oid = (char *)external->directReference->data;
  /* Assuming that this record is defined as an ASN1 structure 
   * and that we have decoding for it, find out from the OID 
   * which type it is and do it.
   */
  /* do we have an OID? */
  if (external->directReference != NULL) {
    /* OPAC Records (likely most common form) */
    if (strcmp(oid, OPACRECSYNTAX) == 0)
      external->u.singleASN1type = (any) DecodeOPACRecord(child0);
    /* Explain Records (also common RSN?) */
    else if (strcmp(oid, EXPLAINRECSYNTAX) == 0)
      external->u.singleASN1type = 
	(any) DecodeExplain_Record(child0);
    /* Explain Records (also common RSN?) */
    else if (strcmp(oid, SUTRECSYNTAX) == 0)
      external->u.singleASN1type = 
	(any) DecodeSUTRS_Record(child0);
    /* Generic records */
    else if (strcmp(oid, GRS1RECSYNTAX) == 0)
      external->u.singleASN1type = 
	(any) DecodeGenericRecord(child0);
    /* Summary records */
    else if (strcmp(oid, SUMMARYRECSYNTAX) == 0)
      external->u.singleASN1type = 
	(any) DecodeBriefBib(child0);
    /* Extended Services 
     * else if (strcmp(oid, ESRECSYNTAX) == 0)
     * external->u.singleASN1type = 
     *  (any) DecodeTaskPackage(child0);
     */
    else 
      external->u.singleASN1type = NULL; /* This will signal that
					  * an unknown OID is in
					  * use
					  */
  }
}

