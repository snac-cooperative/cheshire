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
#include "zprimitive.h"
#include "z3950_3.h"
#include "encode.h"

#define	ATTACH(node1,node2)\
	if (!Attach(node1,node2))\
		return (ASN_NODE *)NULL;



ASN_NODE *
EncodeInternationalString(class_var, tagno, internationalString)
int class_var;
int tagno;
InternationalString internationalString;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeGeneralString(class_var,tagno,internationalString);
    else
        node0 = EncodeGeneralString(BER_CONTEXT,GENERALSTRING,internationalString);
    return node0;
}

ASN_NODE *
EncodeLanguageCode(class_var, tagno, internationalString)
int class_var;
int tagno;
InternationalString internationalString;
{
    return EncodeInternationalString(class_var, tagno, internationalString);
}

ASN_NODE *
EncodeProtocolVersion(class_var, type, tagno, protocolVersion)
int class_var;
int type;
int tagno;
ProtocolVersion protocolVersion;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeBitString(class_var,tagno,protocolVersion);
    else
        node0 = EncodeBitString(BER_CONTEXT,3,protocolVersion);
    return node0;
}

ASN_NODE *
EncodeOptions(class_var, type, tagno, options)
int class_var;
int type;
int tagno;
Options options;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeBitString(class_var,tagno,options);
    else
        node0 = EncodeBitString(BER_CONTEXT,4,options);
    return node0;
}


ASN_NODE *
EncodeReferenceId(class_var, type, tagno, referenceId)
int class_var;
int type;
int tagno;
ReferenceId referenceId;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeOctetString(class_var,tagno,referenceId);
    else
        node0 = EncodeOctetString(BER_CONTEXT,2,referenceId);
    return node0;
}


ASN_NODE *
EncodeResultSetId(class_var, type, tagno, resultSetId)
int class_var;
int type;
int tagno;
ResultSetId resultSetId;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInternationalString(class_var,tagno,resultSetId);
    else
        node0 = EncodeInternationalString(BER_CONTEXT,31,resultSetId);
    return node0;
}


ASN_NODE *
EncodeElementSetName(class_var, type, tagno, elementSetName)
int class_var;
int type;
int tagno;
ElementSetName elementSetName;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInternationalString(class_var,tagno,elementSetName);
    else
        node0 = EncodeInternationalString(BER_CONTEXT,103,elementSetName);
    return node0;
}


ASN_NODE *
EncodeDatabaseName(class_var, type, tagno, databaseName)
int class_var;
int type;
int tagno;
DatabaseName databaseName;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInternationalString(class_var,tagno,databaseName);
    else
        node0 = EncodeInternationalString(BER_CONTEXT,105,databaseName);
    return node0;
}


ASN_NODE *
EncodeAttributeSetId(class_var, type, tagno, attributeSetId)
int class_var;
int type;
int tagno;
AttributeSetId attributeSetId;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeOid(class_var,tagno,attributeSetId);
    else
        node0 = EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,attributeSetId);
    return node0;
}

ASN_NODE *
EncodePDU(pDU)
PDU *pDU;
{
    ASN_NODE *node0;
    switch(pDU->which) {
        case e1_initRequest: {
            node0 = EncodeInitializeRequest(BER_CONTEXT,CONSTRUCTED,20,pDU->u.initRequest);
            break;
        }
        case e1_initResponse: {
            node0 = EncodeInitializeResponse(BER_CONTEXT,CONSTRUCTED,21,pDU->u.initResponse);
            break;
        }
        case e1_searchRequest: {
            node0 = EncodeSearchRequest(BER_CONTEXT,CONSTRUCTED,22,pDU->u.searchRequest);
            break;
        }
        case e1_searchResponse: {
            node0 = EncodeSearchResponse(BER_CONTEXT,CONSTRUCTED,23,pDU->u.searchResponse);
            break;
        }
        case e1_presentRequest: {
            node0 = EncodePresentRequest(BER_CONTEXT,CONSTRUCTED,24,pDU->u.presentRequest);
            break;
        }
        case e1_presentResponse: {
            node0 = EncodePresentResponse(BER_CONTEXT,CONSTRUCTED,25,pDU->u.presentResponse);
            break;
        }
        case e1_deleteResultSetRequest: {
            node0 = EncodeDeleteResultSetRequest(BER_CONTEXT,CONSTRUCTED,26,pDU->u.deleteResultSetRequest);
            break;
        }
        case e1_deleteResultSetResponse: {
            node0 = EncodeDeleteResultSetResponse(BER_CONTEXT,CONSTRUCTED,27,pDU->u.deleteResultSetResponse);
            break;
        }
        case e1_accessControlRequest: {
            node0 = EncodeAccessControlRequest(BER_CONTEXT,CONSTRUCTED,28,pDU->u.accessControlRequest);
            break;
        }
        case e1_accessControlResponse: {
            node0 = EncodeAccessControlResponse(BER_CONTEXT,CONSTRUCTED,29,pDU->u.accessControlResponse);
            break;
        }
        case e1_resourceControlRequest: {
            node0 = EncodeResourceControlRequest(BER_CONTEXT,CONSTRUCTED,30,pDU->u.resourceControlRequest);
            break;
        }
        case e1_resourceControlResponse: {
            node0 = EncodeResourceControlResponse(BER_CONTEXT,CONSTRUCTED,31,pDU->u.resourceControlResponse);
            break;
        }
        case e1_triggerResourceControlRequest: {
            node0 = EncodeTriggerResourceControlRequest(BER_CONTEXT,CONSTRUCTED,32,pDU->u.triggerResourceControlRequest);
            break;
        }
        case e1_resourceReportRequest: {
            node0 = EncodeResourceReportRequest(BER_CONTEXT,CONSTRUCTED,33,pDU->u.resourceReportRequest);
            break;
        }
        case e1_resourceReportResponse: {
            node0 = EncodeResourceReportResponse(BER_CONTEXT,CONSTRUCTED,34,pDU->u.resourceReportResponse);
            break;
        }
        case e1_scanRequest: {
            node0 = EncodeScanRequest(BER_CONTEXT,CONSTRUCTED,35,pDU->u.scanRequest);
            break;
        }
        case e1_scanResponse: {
            node0 = EncodeScanResponse(BER_CONTEXT,CONSTRUCTED,36,pDU->u.scanResponse);
            break;
        }
        case e1_sortRequest: {
            node0 = EncodeSortRequest(BER_CONTEXT,CONSTRUCTED,43,pDU->u.sortRequest);
            break;
        }
        case e1_sortResponse: {
            node0 = EncodeSortResponse(BER_CONTEXT,CONSTRUCTED,44,pDU->u.sortResponse);
            break;
        }
        case e1_segmentRequest: {
            node0 = EncodeSegment(BER_CONTEXT,CONSTRUCTED,45,pDU->u.segmentRequest);
            break;
        }
        case e1_extendedServicesRequest: {
            node0 = EncodeExtendedServicesRequest(BER_CONTEXT,CONSTRUCTED,46,pDU->u.extendedServicesRequest);
            break;
        }
        case e1_extendedServicesResponse: {
            node0 = EncodeExtendedServicesResponse(BER_CONTEXT,CONSTRUCTED,47,pDU->u.extendedServicesResponse);
            break;
        }
        case e1_close: {
            node0 = EncodeClose(BER_CONTEXT,CONSTRUCTED,48,pDU->u.close);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeInitializeRequest(class_var, type, tagno, initializeRequest)
int class_var;
int type;
int tagno;
InitializeRequest *initializeRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (initializeRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeRequest->referenceId));
    }
    ATTACH(node0, EncodeProtocolVersion(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeRequest->protocolVersion));
    ATTACH(node0, EncodeOptions(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeRequest->options));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,initializeRequest->preferredMessageSize));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,initializeRequest->exceptionalRecordSize));
    if (initializeRequest->idAuthentication != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,7);
    ATTACH(node1, EncodeAny(UNIVERSAL, VISIBLESTRING, initializeRequest->idAuthentication));
    ATTACH(node0, node1);
    }
    }
    if (initializeRequest->implementationId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,110,initializeRequest->implementationId));
    }
    if (initializeRequest->implementationName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,111,initializeRequest->implementationName));
    }
    if (initializeRequest->implementationVersion != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,112,initializeRequest->implementationVersion));
    }
    if (initializeRequest->userInformationField != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,11);
    ATTACH(node1, EncodeExternal(UNIVERSAL, EXTERNAL, initializeRequest->userInformationField));
    ATTACH(node0, node1);
    }
    }
    if (initializeRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeInitializeResponse(class_var, type, tagno, initializeResponse)
int class_var;
int type;
int tagno;
InitializeResponse *initializeResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (initializeResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeResponse->referenceId));
    }
    ATTACH(node0, EncodeProtocolVersion(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeResponse->protocolVersion));
    ATTACH(node0, EncodeOptions(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeResponse->options));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,initializeResponse->preferredMessageSize));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,initializeResponse->exceptionalRecordSize));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,12,initializeResponse->result));
    if (initializeResponse->implementationId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,110,initializeResponse->implementationId));
    }
    if (initializeResponse->implementationName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,111,initializeResponse->implementationName));
    }
    if (initializeResponse->implementationVersion != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,112,initializeResponse->implementationVersion));
    }
    if (initializeResponse->userInformationField != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,11);
    ATTACH(node1, EncodeExternal(UNIVERSAL, EXTERNAL, initializeResponse->userInformationField));
    ATTACH(node0, node1);
    }
    }
    if (initializeResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,initializeResponse->otherInfo));
    }
    return node0;
}


ASN_NODE *
EncodeSearchRequest(class_var, type, tagno, searchRequest)
int class_var;
int type;
int tagno;
SearchRequest *searchRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (searchRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchRequest->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,13,searchRequest->smallSetUpperBound));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,14,searchRequest->largeSetLowerBound));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,15,searchRequest->mediumSetPresentNumber));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,16,searchRequest->replaceIndicator));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,17,searchRequest->resultSetName));
    {
    struct databaseNames_List1 *tmp1 = searchRequest->databaseNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,18);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    if (searchRequest->smallSetElementSetNames != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,100);
    ATTACH(node1, EncodeElementSetNames(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchRequest->smallSetElementSetNames));
    ATTACH(node0, node1);
    }
    }
    if (searchRequest->mediumSetElementSetNames != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,101);
    ATTACH(node1, EncodeElementSetNames(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchRequest->mediumSetElementSetNames));
    ATTACH(node0, node1);
    }
    }
    if (searchRequest->preferredRecordSyntax != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,104,searchRequest->preferredRecordSyntax));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,21);
    ATTACH(node1, EncodeQuery(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchRequest->query));
    ATTACH(node0, node1);
    }
    if (searchRequest->additionalSearchInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(BER_CONTEXT,CONSTRUCTED,203,searchRequest->additionalSearchInfo));
    }
    if (searchRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeQuery(class_var, type, tagno, query)
int class_var;
int type;
int tagno;
Query *query;
{
    ASN_NODE *node0;
    switch(query->which) {
        case e2_type_0: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
            ATTACH(node1, EncodeAny(UNIVERSAL, ANY, query->u.type_0));
            node0 = node1;
            }
            break;
        }
        case e2_type_1: {
            node0 = EncodeRPNQuery(BER_CONTEXT,CONSTRUCTED,1,query->u.type_1);
            break;
        }
        case e2_type_2: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeOctetString(UNIVERSAL, OCTETSTRING, query->u.type_2));
            node0 = node1;
            }
            break;
        }
        case e2_type_100: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,100);
            ATTACH(node1, EncodeOctetString(UNIVERSAL, OCTETSTRING, query->u.type_100));
            node0 = node1;
            }
            break;
        }
        case e2_type_101: {
            node0 = EncodeRPNQuery(BER_CONTEXT,CONSTRUCTED,101,query->u.type_101);
            break;
        }
        case e2_type_102: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,102);
            ATTACH(node1, EncodeOctetString(UNIVERSAL, OCTETSTRING, query->u.type_102));
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeRPNQuery(class_var, type, tagno, rPNQuery)
int class_var;
int type;
int tagno;
RPNQuery *rPNQuery;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeAttributeSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNQuery->attributeSet));
    ATTACH(node0, EncodeRPNStructure(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNQuery->rpn));
    return node0;
}

ASN_NODE *
EncodeRPNStructure(class_var, type, tagno, rPNStructure)
int class_var;
int type;
int tagno;
RPNStructure *rPNStructure;
{
    ASN_NODE *node0;
    switch(rPNStructure->which) {
        case e3_op: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
            ATTACH(node1, EncodeOperand(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNStructure->u.op));
            node0 = node1;
            }
            break;
        }
        case e3_rpnRpnOp: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node1, EncodeRPNStructure(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNStructure->u.rpnRpnOp.rpn1));
            ATTACH(node1, EncodeRPNStructure(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNStructure->u.rpnRpnOp.rpn2));
            ATTACH(node1, EncodeOperator(CLASS_NONE,TYPE_NONE,TAGNO_NONE,rPNStructure->u.rpnRpnOp.op));
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOperand(class_var, type, tagno, operand)
int class_var;
int type;
int tagno;
Operand *operand;
{
    ASN_NODE *node0;
    switch(operand->which) {
        case e4_attrTerm: {
            node0 = EncodeAttributesPlusTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,operand->u.attrTerm);
            break;
        }
        case e4_resultSet: {
            node0 = EncodeResultSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,operand->u.resultSet);
            break;
        }
        case e4_resultAttr: {
            node0 = EncodeResultSetPlusAttributes(CLASS_NONE,TYPE_NONE,TAGNO_NONE,operand->u.resultAttr);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeAttributesPlusTerm(class_var, type, tagno, attributesPlusTerm)
int class_var;
int type;
int tagno;
AttributesPlusTerm *attributesPlusTerm;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,102);
    ATTACH(node0, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,attributesPlusTerm->attributes));
    ATTACH(node0, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,attributesPlusTerm->term));
    return node0;
}

ASN_NODE *
EncodeResultSetPlusAttributes(class_var, type, tagno, resultSetPlusAttributes)
int class_var;
int type;
int tagno;
ResultSetPlusAttributes *resultSetPlusAttributes;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,214);
    ATTACH(node0, EncodeResultSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resultSetPlusAttributes->resultSet));
    ATTACH(node0, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resultSetPlusAttributes->attributes));
    return node0;
}

ASN_NODE *
EncodeAttributeList(class_var, type, tagno, attributeList)
int class_var;
int type;
int tagno;
AttributeList *attributeList;
{
    struct AttributeList *tmp1 = attributeList;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,44);
    while (tmp1 != 0) {
        ATTACH(node0, EncodeAttributeElement(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeTerm(class_var, type, tagno, term)
int class_var;
int type;
int tagno;
Term *term;
{
    ASN_NODE *node0;
    switch(term->which) {
        case e5_general: {
            node0  = EncodeOctetString(BER_CONTEXT,45,term->u.general);
            break;
        }
        case e5_numeric: {
            node0  = EncodeInteger(BER_CONTEXT,215,term->u.numeric);
            break;
        }
        case e5_characterString: {
            node0 = EncodeInternationalString(BER_CONTEXT,216,term->u.characterString);
            break;
        }
        case e5_oid: {
            node0  = EncodeOid(BER_CONTEXT,217,term->u.oid);
            break;
        }
        case e5_dateTime: {
            node0 = EncodeGeneralizedTime(BER_CONTEXT,218,term->u.dateTime);
            break;
        }
        case e5_external: {
            node0  = EncodeExternal(BER_CONTEXT,219,term->u.external);
            break;
        }
        case e5_integerAndUnit: {
            node0 = EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,220,term->u.integerAndUnit);
            break;
        }
        case e5_nullVal: {
            node0 = EncodeNull(BER_CONTEXT,221);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOperator(class_var, type, tagno, operator_var)
int class_var;
int type;
int tagno;
Operator *operator_var;
{
    ASN_NODE *node0 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,46);
    switch(operator_var->which) {
        case e6_and: {
            ATTACH(node0, EncodeNull(BER_CONTEXT,0));
            break;
        }
        case e6_or: {
            ATTACH(node0, EncodeNull(BER_CONTEXT,1));
            break;
        }
        case e6_and_not: {
            ATTACH(node0, EncodeNull(BER_CONTEXT,2));
            break;
        }
        case e6_prox: {
            ATTACH(node0, EncodeProximityOperator(BER_CONTEXT,CONSTRUCTED,3,operator_var->u.prox));
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeAttributeElement(class_var, type, tagno, attributeElement)
int class_var;
int type;
int tagno;
AttributeElement *attributeElement;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (attributeElement->attributeSet != 0) {
    ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,1,attributeElement->attributeSet));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,120,attributeElement->attributeType));
    switch (attributeElement->attributeValue.which) {
        case e7_numeric: {
            ASN_NODE *node1;
            node1  = EncodeInteger(BER_CONTEXT,121,attributeElement->attributeValue.u.numeric);
	    ATTACH(node0, node1);
            break;
        }
        case e7_complex: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,224);
            {
            struct list_List2 *tmp1 = attributeElement->attributeValue.u.complex.list;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            while (tmp1 != 0) {
                ATTACH(node2, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            if (attributeElement->attributeValue.u.complex.semanticAction != 0) {
            {
            struct semanticAction_List3 *tmp1 = attributeElement->attributeValue.u.complex.semanticAction;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            while (tmp1 != 0) {
                ATTACH(node2, EncodeInteger(UNIVERSAL,INTEGER,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            }
            ATTACH(node0, node1);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeKnownProximityUnit(class_var, type, tagno, knownProximityUnit)
int class_var;
int type;
int tagno;
KnownProximityUnit knownProximityUnit;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,knownProximityUnit);
    else
        node0 = EncodeInteger(UNIVERSAL,INTEGER,knownProximityUnit);
    return node0;
}

ASN_NODE *
EncodeProximityOperator(class_var, type, tagno, proximityOperator)
int class_var;
int type;
int tagno;
ProximityOperator *proximityOperator;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (proximityOperator->exclusion != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,proximityOperator->exclusion));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,proximityOperator->distance));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,3,proximityOperator->ordered));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,proximityOperator->relationType));
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    switch (proximityOperator->proximityUnitCode.which) {
        case e8_known: {
            ATTACH(node1, EncodeKnownProximityUnit(BER_CONTEXT,CONSTRUCTED,1,proximityOperator->proximityUnitCode.u.known));
            break;
        }
        case e8_private: {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,2,proximityOperator->proximityUnitCode.u.private_var));
            break;
        }
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodePresentStatus(class_var, type, tagno, presentStatus)
int class_var;
int type;
int tagno;
PresentStatus presentStatus;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,presentStatus);
    else
        node0 = EncodeInteger(BER_CONTEXT,27,presentStatus);
    return node0;
}

ASN_NODE *
EncodeSearchResponse(class_var, type, tagno, searchResponse)
int class_var;
int type;
int tagno;
SearchResponse *searchResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (searchResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchResponse->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,23,searchResponse->resultCount));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,24,searchResponse->numberOfRecordsReturned));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,25,searchResponse->nextResultSetPosition));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,22,searchResponse->searchStatus));
    if (searchResponse->resultSetStatus != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,26,searchResponse->resultSetStatus));
    }
    if (searchResponse->presentStatus != 0) {
    ATTACH(node0, EncodePresentStatus(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchResponse->presentStatus));
    }
    if (searchResponse->records != 0) {
    ATTACH(node0, EncodeRecords(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchResponse->records));
    }
    if (searchResponse->additionalSearchInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(BER_CONTEXT,CONSTRUCTED,203,searchResponse->additionalSearchInfo));
    }
    if (searchResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,searchResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodePresentRequest(class_var, type, tagno, presentRequest)
int class_var;
int type;
int tagno;
PresentRequest *presentRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (presentRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentRequest->referenceId));
    }
    ATTACH(node0, EncodeResultSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentRequest->resultSetId));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,30,presentRequest->resultSetStartPoint));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,29,presentRequest->numberOfRecordsRequested));
    if (presentRequest->additionalRanges != 0) {
    {
    struct additionalRanges_List4 *tmp1 = presentRequest->additionalRanges;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,212);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeRange(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (presentRequest->recordComposition != 0) {
    switch (presentRequest->recordComposition->which) {
        case e9_simple: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,19);
            ATTACH(node1, EncodeElementSetNames(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentRequest->recordComposition->u.simple));
            ATTACH(node0, node1);
            }
            break;
        }
        case e9_complex: {
            ATTACH(node0, EncodeCompSpec(BER_CONTEXT,CONSTRUCTED,209,presentRequest->recordComposition->u.complex));
            break;
        }
    }
    }
    if (presentRequest->preferredRecordSyntax != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,104,presentRequest->preferredRecordSyntax));
    }
    if (presentRequest->maxSegmentCount != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,204,presentRequest->maxSegmentCount));
    }
    if (presentRequest->maxRecordSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,206,presentRequest->maxRecordSize));
    }
    if (presentRequest->maxSegmentSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,207,presentRequest->maxSegmentSize));
    }
    if (presentRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeSegment(class_var, type, tagno, segment)
int class_var;
int type;
int tagno;
Segment *segment;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (segment->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,segment->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,24,segment->numberOfRecordsReturned));
    {
    struct segmentRecords_List5 *tmp1 = segment->segmentRecords;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeNamePlusRecord(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    if (segment->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,segment->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodePresentResponse(class_var, type, tagno, presentResponse)
int class_var;
int type;
int tagno;
PresentResponse *presentResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (presentResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentResponse->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,24,presentResponse->numberOfRecordsReturned));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,25,presentResponse->nextResultSetPosition));
    ATTACH(node0, EncodePresentStatus(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentResponse->presentStatus));
    if (presentResponse->records != 0) {
    ATTACH(node0, EncodeRecords(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentResponse->records));
    }
    if (presentResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,presentResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeRecords(class_var, type, tagno, records)
int class_var;
int type;
int tagno;
Records *records;
{
    ASN_NODE *node0;
    switch(records->which) {
        case e10_responseRecords: {
            {
            struct responseRecords_List6 *tmp1 = records->u.responseRecords;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,28);
            while (tmp1 != 0) {
                ATTACH(node1, EncodeNamePlusRecord(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
        case e10_nonSurrogateDiagnostic: {
            node0 = EncodeDefaultDiagFormat(BER_CONTEXT,CONSTRUCTED,130,records->u.nonSurrogateDiagnostic);
            break;
        }
        case e10_multipleNonSurDiagnostics: {
            {
            struct multipleNonSurDiagnostics_List7 *tmp1 = records->u.multipleNonSurDiagnostics;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,205);
            while (tmp1 != 0) {
                ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}


ASN_NODE *
EncodeNamePlusRecord(class_var, type, tagno, namePlusRecord)
int class_var;
int type;
int tagno;
NamePlusRecord *namePlusRecord;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (namePlusRecord->name != 0) {
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,0,namePlusRecord->name));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    switch (namePlusRecord->record.which) {
        case e11_retrievalRecord: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeExternal(UNIVERSAL, EXTERNAL, namePlusRecord->record.u.retrievalRecord));
            ATTACH(node1, node2);
            }
            break;
        }
        case e11_surrogateDiagnostic: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,namePlusRecord->record.u.surrogateDiagnostic));
            ATTACH(node1, node2);
            }
            break;
        }
        case e11_startingFragment: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
            ATTACH(node2, EncodeFragmentSyntax(CLASS_NONE,TYPE_NONE,TAGNO_NONE,namePlusRecord->record.u.startingFragment));
            ATTACH(node1, node2);
            }
            break;
        }
        case e11_intermediateFragment: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
            ATTACH(node2, EncodeFragmentSyntax(CLASS_NONE,TYPE_NONE,TAGNO_NONE,namePlusRecord->record.u.intermediateFragment));
            ATTACH(node1, node2);
            }
            break;
        }
        case e11_finalFragment: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
            ATTACH(node2, EncodeFragmentSyntax(CLASS_NONE,TYPE_NONE,TAGNO_NONE,namePlusRecord->record.u.finalFragment));
            ATTACH(node1, node2);
            }
            break;
        }
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeFragmentSyntax(class_var, type, tagno, fragmentSyntax)
int class_var;
int type;
int tagno;
FragmentSyntax *fragmentSyntax;
{
    ASN_NODE *node0;
    switch(fragmentSyntax->which) {
        case e12_externallyTagged: {
            node0 = EncodeExternal(UNIVERSAL, EXTERNAL, fragmentSyntax->u.externallyTagged);
            break;
        }
        case e12_notExternallyTagged: {
            node0 = EncodeOctetString(UNIVERSAL, OCTETSTRING, fragmentSyntax->u.notExternallyTagged);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeDiagRec(class_var, type, tagno, diagRec)
int class_var;
int type;
int tagno;
DiagRec *diagRec;
{
    ASN_NODE *node0;
    switch(diagRec->which) {
        case e13_defaultFormat: {
            node0 = EncodeDefaultDiagFormat(CLASS_NONE,TYPE_NONE,TAGNO_NONE,diagRec->u.defaultFormat);
            break;
        }
        case e13_externallyDefined: {
            node0 = EncodeExternal(UNIVERSAL, EXTERNAL, diagRec->u.externallyDefined);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeDefaultDiagFormat(class_var, type, tagno, defaultDiagFormat)
int class_var;
int type;
int tagno;
DefaultDiagFormat *defaultDiagFormat;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, defaultDiagFormat->diagnosticSetId));
    ATTACH(node0, EncodeInteger(UNIVERSAL, INTEGER, defaultDiagFormat->condition));
    switch (defaultDiagFormat->addinfo.which) {
        case e14_v2Addinfo: {
            ATTACH(node0, EncodeVisibleString(UNIVERSAL, VISIBLESTRING, defaultDiagFormat->addinfo.u.v2Addinfo));
            break;
        }
        case e14_v3Addinfo: {
            ATTACH(node0, EncodeInternationalString(UNIVERSAL,GENERALSTRING,defaultDiagFormat->addinfo.u.v3Addinfo));
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeRange(class_var, type, tagno, range)
int class_var;
int type;
int tagno;
Range *range;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,range->startingPosition));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,range->numberOfRecords));
    return node0;
}

ASN_NODE *
EncodeElementSetNames(class_var, type, tagno, elementSetNames)
int class_var;
int type;
int tagno;
ElementSetNames *elementSetNames;
{
    ASN_NODE *node0;
    switch(elementSetNames->which) {
        case e15_genericElementSetName: {
            node0 = EncodeInternationalString(BER_CONTEXT,0,elementSetNames->u.genericElementSetName);
            break;
        }
        case e15_databaseSpecific: {
            {
            struct databaseSpecific_List8 *tmp1 = elementSetNames->u.databaseSpecific;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            while (tmp1 != 0) {
                ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
                ATTACH(node2, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.dbName));
                ATTACH(node2, EncodeElementSetName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.esn));
                ATTACH(node1, node2);
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeCompSpec(class_var, type, tagno, compSpec)
int class_var;
int type;
int tagno;
CompSpec *compSpec;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,compSpec->selectAlternativeSyntax));
    if (compSpec->generic != 0) {
    ATTACH(node0, EncodeSpecification(BER_CONTEXT,CONSTRUCTED,2,compSpec->generic));
    }
    if (compSpec->dbSpecific != 0) {
    {
    struct dbSpecific_List9 *tmp1 = compSpec->dbSpecific;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        ATTACH(node3, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.db));
        ATTACH(node2, node3);
        }
        ATTACH(node2, EncodeSpecification(BER_CONTEXT,CONSTRUCTED,2,tmp1->item.spec));
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (compSpec->recordSyntax != 0) {
    {
    struct recordSyntax_List10 *tmp1 = compSpec->recordSyntax;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeSpecification(class_var, type, tagno, specification)
int class_var;
int type;
int tagno;
Specification *specification;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (specification->schema != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,specification->schema));
    }
    if (specification->elementSpec != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    switch (specification->elementSpec->which) {
        case e16_elementSetName: {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,specification->elementSpec->u.elementSetName));
            break;
        }
        case e16_externalEspec: {
            ATTACH(node1, EncodeExternal(BER_CONTEXT,2,specification->elementSpec->u.externalEspec));
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeDeleteResultSetRequest(class_var, type, tagno, deleteResultSetRequest)
int class_var;
int type;
int tagno;
DeleteResultSetRequest *deleteResultSetRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (deleteResultSetRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,deleteResultSetRequest->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,32,deleteResultSetRequest->deleteFunction));
    if (deleteResultSetRequest->resultSetList != 0) {
    {
    struct resultSetList_List11 *tmp1 = deleteResultSetRequest->resultSetList;
    ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeResultSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (deleteResultSetRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,deleteResultSetRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeDeleteResultSetResponse(class_var, type, tagno, deleteResultSetResponse)
int class_var;
int type;
int tagno;
DeleteResultSetResponse *deleteResultSetResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (deleteResultSetResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,deleteResultSetResponse->referenceId));
    }
    ATTACH(node0, EncodeDeleteSetStatus(BER_CONTEXT,CONSTRUCTED,0,deleteResultSetResponse->deleteOperationStatus));
    if (deleteResultSetResponse->deleteListStatuses != 0) {
    ATTACH(node0, EncodeListStatuses(BER_CONTEXT,CONSTRUCTED,1,deleteResultSetResponse->deleteListStatuses));
    }
    if (deleteResultSetResponse->numberNotDeleted != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,34,deleteResultSetResponse->numberNotDeleted));
    }
    if (deleteResultSetResponse->bulkStatuses != 0) {
    ATTACH(node0, EncodeListStatuses(BER_CONTEXT,CONSTRUCTED,35,deleteResultSetResponse->bulkStatuses));
    }
    if (deleteResultSetResponse->deleteMessage != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,36,deleteResultSetResponse->deleteMessage));
    }
    if (deleteResultSetResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,deleteResultSetResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeListStatuses(class_var, type, tagno, listStatuses)
int class_var;
int type;
int tagno;
ListStatuses *listStatuses;
{
    struct ListStatuses *tmp1 = listStatuses;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node1, EncodeResultSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.id));
        ATTACH(node1, EncodeDeleteSetStatus(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.status));
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeDeleteSetStatus(class_var, type, tagno, deleteSetStatus)
int class_var;
int type;
int tagno;
DeleteSetStatus deleteSetStatus;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,deleteSetStatus);
    else
        node0 = EncodeInteger(BER_CONTEXT,33,deleteSetStatus);
    return node0;
}

ASN_NODE *
EncodeAccessControlRequest(class_var, type, tagno, accessControlRequest)
int class_var;
int type;
int tagno;
AccessControlRequest *accessControlRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (accessControlRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,accessControlRequest->referenceId));
    }
    switch (accessControlRequest->securityChallenge.which) {
        case e17_simpleForm: {
            ATTACH(node0, EncodeOctetString(BER_CONTEXT,37,accessControlRequest->securityChallenge.u.simpleForm));
            break;
        }
        case e17_externallyDefined: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
            ATTACH(node1, EncodeExternal(UNIVERSAL, EXTERNAL, accessControlRequest->securityChallenge.u.externallyDefined));
            ATTACH(node0, node1);
            }
            break;
        }
    }
    if (accessControlRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,accessControlRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeAccessControlResponse(class_var, type, tagno, accessControlResponse)
int class_var;
int type;
int tagno;
AccessControlResponse *accessControlResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (accessControlResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,accessControlResponse->referenceId));
    }
    if (accessControlResponse->securityChallengeResponse != 0) {
    switch (accessControlResponse->securityChallengeResponse->which) {
        case e18_simpleForm: {
            ATTACH(node0, EncodeOctetString(BER_CONTEXT,38,accessControlResponse->securityChallengeResponse->u.simpleForm));
            break;
        }
        case e18_externallyDefined: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
            ATTACH(node1, EncodeExternal(UNIVERSAL, EXTERNAL, accessControlResponse->securityChallengeResponse->u.externallyDefined));
            ATTACH(node0, node1);
            }
            break;
        }
    }
    }
    if (accessControlResponse->diagnostic != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,223);
    ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,accessControlResponse->diagnostic));
    ATTACH(node0, node1);
    }
    }
    if (accessControlResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,accessControlResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeResourceControlRequest(class_var, type, tagno, resourceControlRequest)
int class_var;
int type;
int tagno;
ResourceControlRequest *resourceControlRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (resourceControlRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceControlRequest->referenceId));
    }
    if (resourceControlRequest->suspendedFlag != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,39,resourceControlRequest->suspendedFlag));
    }
    if (resourceControlRequest->resourceReport != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,40);
    ATTACH(node1, EncodeResourceReport(CLASS_NONE,TYPE_NONE,TAGNO_NONE,
				       (External *)
				       resourceControlRequest->resourceReport));
    ATTACH(node0, node1);
    }
    }
    if (resourceControlRequest->partialResultsAvailable != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,41,resourceControlRequest->partialResultsAvailable));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,42,resourceControlRequest->responseRequired));
    if (resourceControlRequest->triggeredRequestFlag != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,43,resourceControlRequest->triggeredRequestFlag));
    }
    if (resourceControlRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceControlRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeResourceControlResponse(class_var, type, tagno, resourceControlResponse)
int class_var;
int type;
int tagno;
ResourceControlResponse *resourceControlResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (resourceControlResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceControlResponse->referenceId));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,44,resourceControlResponse->continueFlag));
    if (resourceControlResponse->resultSetWanted != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,45,resourceControlResponse->resultSetWanted));
    }
    if (resourceControlResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceControlResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeTriggerResourceControlRequest(class_var, type, tagno, triggerResourceControlRequest)
int class_var;
int type;
int tagno;
TriggerResourceControlRequest *triggerResourceControlRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (triggerResourceControlRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,triggerResourceControlRequest->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,46,triggerResourceControlRequest->requestedAction));
    if (triggerResourceControlRequest->prefResourceReportFormat != 0) {
    ATTACH(node0, EncodeResourceReportId(BER_CONTEXT,CONSTRUCTED,47,triggerResourceControlRequest->prefResourceReportFormat));
    }
    if (triggerResourceControlRequest->resultSetWanted != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,48,triggerResourceControlRequest->resultSetWanted));
    }
    if (triggerResourceControlRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,triggerResourceControlRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeResourceReportRequest(class_var, type, tagno, resourceReportRequest)
int class_var;
int type;
int tagno;
ResourceReportRequest *resourceReportRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (resourceReportRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceReportRequest->referenceId));
    }
    if (resourceReportRequest->opId != 0) {
    ATTACH(node0, EncodeReferenceId(BER_CONTEXT,CONSTRUCTED,210,resourceReportRequest->opId));
    }
    if (resourceReportRequest->prefResourceReportFormat != 0) {
    ATTACH(node0, EncodeResourceReportId(BER_CONTEXT,CONSTRUCTED,49,resourceReportRequest->prefResourceReportFormat));
    }
    if (resourceReportRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceReportRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeResourceReportResponse(class_var, type, tagno, resourceReportResponse)
int class_var;
int type;
int tagno;
ResourceReportResponse *resourceReportResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (resourceReportResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceReportResponse->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,50,resourceReportResponse->resourceReportStatus));
    if (resourceReportResponse->resourceReport != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,51);
    ATTACH(node1, EncodeResourceReport(CLASS_NONE,TYPE_NONE,TAGNO_NONE,
				       (External *)
				       resourceReportResponse->resourceReport));
    ATTACH(node0, node1);
    }
    }
    if (resourceReportResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,resourceReportResponse->otherInfo));
    }
    return node0;
}
ASN_NODE *
EncodeOtherInformation(class_var, type, tagno, otherInformation)
int class_var;
int type;
int tagno;
OtherInformation *otherInformation;
{
  struct OtherInformation *tmp1 = otherInformation;
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,201);

  while (tmp1 != 0) {
    ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (tmp1->item.category != 0) {
      ATTACH(node1, EncodeInfoCategory(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.category));
    }
    switch (tmp1->item.information.which) {
    case e24_characterInfo: {
      ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.information.u.characterInfo));
      break;
    }
    case e24_binaryInfo: {
      ATTACH(node1, EncodeOctetString(BER_CONTEXT,3,tmp1->item.information.u.binaryInfo));
      break;
    }
    case e24_externallyDefinedInfo: {
      ATTACH(node1, EncodeExternal(BER_CONTEXT,4,tmp1->item.information.u.externallyDefinedInfo));
      break;
    }
    case e24_oid: {
      ATTACH(node1, EncodeOid(BER_CONTEXT,5,tmp1->item.information.u.oid))
      break;
    }
    }
    ATTACH(node0, node1);
    tmp1 = tmp1->next;
  }
    return node0;
}

ASN_NODE *
EncodeInfoCategory(class_var, type, tagno, infoCategory)
int class_var;
int type;
int tagno;
InfoCategory *infoCategory;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (infoCategory->categoryTypeId != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,infoCategory->categoryTypeId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,infoCategory->categoryValue));
    return node0;
}

ASN_NODE *
EncodeStringOrNumeric(class_var, type, tagno, stringOrNumeric)
int class_var;
int type;
int tagno;
StringOrNumeric *stringOrNumeric;
{
    ASN_NODE *node0;
    switch(stringOrNumeric->which) {
        case e25_string: {
            node0 = EncodeInternationalString(BER_CONTEXT,1,stringOrNumeric->u.string);
            break;
        }
        case e25_numeric: {
            node0  = EncodeInteger(BER_CONTEXT,2,stringOrNumeric->u.numeric);
            break;
        }
    }
    return node0;
}


ASN_NODE *
EncodeResourceReport(class_var, type, tagno, resourceReport)
int class_var;
int type;
int tagno;
External *resourceReport;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeExternal(class_var,tagno,resourceReport);
    else
        node0 = EncodeExternal(UNIVERSAL,EXTERNAL,resourceReport);
    return node0;
}

ASN_NODE *
EncodeResourceReportId(class_var, type, tagno, resourceReportId)
int class_var;
int type;
int tagno;
ResourceReportId resourceReportId;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeOid(class_var,tagno,resourceReportId);
    else
        node0 = EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,resourceReportId);
    return node0;
}
ASN_NODE *
EncodeIntUnit(class_var, type, tagno, intUnit)
int class_var;
int type;
int tagno;
IntUnit *intUnit;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,intUnit->value));
    ATTACH(node0, EncodeUnit(BER_CONTEXT,CONSTRUCTED,2,intUnit->unitUsed));
    return node0;
}

ASN_NODE *
EncodeUnit(class_var, type, tagno, unit)
int class_var;
int type;
int tagno;
Unit *unit;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (unit->unitSystem != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,unit->unitSystem));
    ATTACH(node0, node1);
    }
    }
    if (unit->unitType != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,unit->unitType));
    ATTACH(node0, node1);
    }
    }
    if (unit->unit != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,unit->unit));
    ATTACH(node0, node1);
    }
    }
    if (unit->scaleFactor != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,unit->scaleFactor));
    }
    return node0;
}


ASN_NODE *
EncodeScanRequest(class_var, type, tagno, scanRequest)
int class_var;
int type;
int tagno;
ScanRequest *scanRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (scanRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanRequest->referenceId));
    }
    {
    struct databaseNames_List12 *tmp1 = scanRequest->databaseNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    if (scanRequest->attributeSet != 0) {
    ATTACH(node0, EncodeAttributeSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanRequest->attributeSet));
    }
    ATTACH(node0, EncodeAttributesPlusTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanRequest->termListAndStartPoint));
    if (scanRequest->stepSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,scanRequest->stepSize));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,scanRequest->numberOfTermsRequested));
    if (scanRequest->preferredPositionInResponse != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,7,scanRequest->preferredPositionInResponse));
    }
    if (scanRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeScanResponse(class_var, type, tagno, scanResponse)
int class_var;
int type;
int tagno;
ScanResponse *scanResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (scanResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanResponse->referenceId));
    }
    if (scanResponse->stepSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,scanResponse->stepSize));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,scanResponse->scanStatus));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,scanResponse->numberOfEntriesReturned));
    if (scanResponse->positionOfTerm != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,scanResponse->positionOfTerm));
    }
    if (scanResponse->entries != 0) {
    ATTACH(node0, EncodeListEntries(BER_CONTEXT,CONSTRUCTED,7,scanResponse->entries));
    }
    if (scanResponse->attributeSet != 0) {
    ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,8,scanResponse->attributeSet));
    }
    if (scanResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,scanResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeListEntries(class_var, type, tagno, listEntries)
int class_var;
int type;
int tagno;
ListEntries *listEntries;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (listEntries->entries != 0) {
    {
    struct entries_List13 *tmp1 = listEntries->entries;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeEntry(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (listEntries->nonsurrogateDiagnostics != 0) {
    {
    struct nonsurrogateDiagnostics_List14 *tmp1 = listEntries->nonsurrogateDiagnostics;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeEntry(class_var, type, tagno, entry)
int class_var;
int type;
int tagno;
Entry *entry;
{
    ASN_NODE *node0;
    switch(entry->which) {
        case e19_termInfo: {
            node0 = EncodeTermInfo(BER_CONTEXT,CONSTRUCTED,1,entry->u.termInfo);
            break;
        }
        case e19_surrogateDiagnostic: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,entry->u.surrogateDiagnostic));
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeTermInfo(class_var, type, tagno, termInfo)
int class_var;
int type;
int tagno;
TermInfo *termInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,termInfo->term));
    if (termInfo->displayTerm != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,termInfo->displayTerm));
    }
    if (termInfo->suggestedAttributes != 0) {
    ATTACH(node0, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,termInfo->suggestedAttributes));
    }
    if (termInfo->alternativeTerm != 0) {
    {
    struct alternativeTerm_List15 *tmp1 = termInfo->alternativeTerm;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributesPlusTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (termInfo->globalOccurrences != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,termInfo->globalOccurrences));
    }
    if (termInfo->byAttributes != 0) {
    ATTACH(node0, EncodeOccurrenceByAttributes(BER_CONTEXT,CONSTRUCTED,3,termInfo->byAttributes));
    }
    if (termInfo->otherTermInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,termInfo->otherTermInfo));
    }
    return node0;
}

ASN_NODE *
EncodeOccurrenceByAttributes(class_var, type, tagno, occurrenceByAttributes)
int class_var;
int type;
int tagno;
OccurrenceByAttributes *occurrenceByAttributes;
{
    struct OccurrenceByAttributes *tmp1 = occurrenceByAttributes;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        ATTACH(node2, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.attributes));
        ATTACH(node1, node2);
        }
        if (tmp1->item.occurrences != 0) {
        switch (tmp1->item.occurrences->which) {
            case e20_global: {
                {
                ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
                ATTACH(node2, EncodeInteger(UNIVERSAL, INTEGER, tmp1->item.occurrences->u.global));
                ATTACH(node1, node2);
                }
                break;
            }
            case e20_byDatabase: {
                {
                struct byDatabase_List16 *tmp2 = tmp1->item.occurrences->u.byDatabase;
                ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
                while (tmp2 != 0) {
                    ASN_NODE *node3 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
                    ATTACH(node3, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp2->item.db));
                    if (tmp2->item.num != 0) {
                    ATTACH(node3, EncodeInteger(BER_CONTEXT,1,tmp2->item.num));
                    }
                    if (tmp2->item.otherDbInfo != 0) {
                    ATTACH(node3, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp2->item.otherDbInfo));
                    }
                    ATTACH(node2, node3);
                    tmp2 = tmp2->next;
                }
                ATTACH(node1, node2);
                }
                break;
            }
        }
        }
        if (tmp1->item.otherOccurInfo != 0) {
        ATTACH(node1, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.otherOccurInfo));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeSortRequest(class_var, type, tagno, sortRequest)
int class_var;
int type;
int tagno;
SortRequest *sortRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (sortRequest->referenceId != 0) {
      ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortRequest->referenceId));
     }
    {
    struct inputResultSetNames_List17 *tmp1 = sortRequest->inputResultSetNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(UNIVERSAL,GENERALSTRING,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,sortRequest->sortedResultSetName));
    {
    struct sortSequence_List18 *tmp1 = sortRequest->sortSequence;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeSortKeySpec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    if (sortRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeSortResponse(class_var, type, tagno, sortResponse)
int class_var;
int type;
int tagno;
SortResponse *sortResponse;
{
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
  if (sortResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortResponse->referenceId));
  }
  ATTACH(node0, EncodeInteger(BER_CONTEXT,3,sortResponse->sortStatus));
  if (sortResponse->resultSetStatus != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,sortResponse->resultSetStatus));
  }
  if (sortResponse->diagnostics != 0) {
    {
      struct diagnostics_List19 *tmp1 = sortResponse->diagnostics;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }

#ifdef Z3950_2001
  if (sortResponse->resultCount != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,sortResponse->resultCount));
  }
#endif

  if (sortResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortResponse->otherInfo));
  }
  return node0;
}

ASN_NODE *
EncodeSortKeySpec(class_var, type, tagno, sortKeySpec)
int class_var;
int type;
int tagno;
SortKeySpec *sortKeySpec;
{
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
  ATTACH(node0, EncodeSortElement(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortKeySpec->sortElement));
  ATTACH(node0, EncodeInteger(BER_CONTEXT,1,sortKeySpec->sortRelation));
  ATTACH(node0, EncodeInteger(BER_CONTEXT,2,sortKeySpec->caseSensitivity));
  {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    switch (sortKeySpec->missingValueAction.which) {
    case e21_abort: {
      ATTACH(node1, EncodeNull(BER_CONTEXT,1));
      break;
    }
    case e21_nullVal: {
      ATTACH(node1, EncodeNull(BER_CONTEXT,2));
      break;
    }
    case e21_missingValueData: {
      ATTACH(node1, EncodeOctetString(BER_CONTEXT,3,sortKeySpec->missingValueAction.u.missingValueData));
      break;
    }
    }
    ATTACH(node0, node1);
    
  }
  return node0;
}

ASN_NODE *
EncodeSortElement(class_var, type, tagno, sortElement)
int class_var;
int type;
int tagno;
SortElement *sortElement;
{
    ASN_NODE *node0;
    switch(sortElement->which) {
        case e22_generic: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node1, EncodeSortKey(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortElement->u.generic));
            node0 = node1;
            }
            break;
        }
        case e22_datbaseSpecific: {
            {
            struct datbaseSpecific_List20 *tmp1 = sortElement->u.datbaseSpecific;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            while (tmp1 != 0) {
                ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
                ATTACH(node2, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.databaseName));
                ATTACH(node2, EncodeSortKey(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.dbSort));
                ATTACH(node1, node2);
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeSortKey(class_var, type, tagno, sortKey)
int class_var;
int type;
int tagno;
SortKey *sortKey;
{
    ASN_NODE *node0;
    switch(sortKey->which) {
        case e23_sortfield: {
            node0 = EncodeInternationalString(BER_CONTEXT,0,sortKey->u.sortfield);
            break;
        }
        case e23_elementSpec: {
            node0 = EncodeSpecification(BER_CONTEXT,CONSTRUCTED,1,sortKey->u.elementSpec);
            break;
        }
        case e23_sortAttributes: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeAttributeSetId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortKey->u.sortAttributes.id));
            ATTACH(node1, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,sortKey->u.sortAttributes.list));
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeExtendedServicesRequest(class_var, type, tagno, extendedServicesRequest)
int class_var;
int type;
int tagno;
ExtendedServicesRequest *extendedServicesRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (extendedServicesRequest->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,extendedServicesRequest->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,extendedServicesRequest->function));
    ATTACH(node0, EncodeOid(BER_CONTEXT,4,extendedServicesRequest->packageType));
    if (extendedServicesRequest->packageName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,5,extendedServicesRequest->packageName));
    }
    if (extendedServicesRequest->userId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,6,extendedServicesRequest->userId));
    }
    if (extendedServicesRequest->retentionTime != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,7,extendedServicesRequest->retentionTime));
    }
    if (extendedServicesRequest->permissions != 0) {
    ATTACH(node0, EncodePermissions(BER_CONTEXT,CONSTRUCTED,8,extendedServicesRequest->permissions));
    }
    if (extendedServicesRequest->description != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,9,extendedServicesRequest->description));
    }
    if (extendedServicesRequest->taskSpecificParameters != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,10,extendedServicesRequest->taskSpecificParameters));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,11,extendedServicesRequest->waitAction));
    if (extendedServicesRequest->elements != 0) {
    ATTACH(node0, EncodeElementSetName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,extendedServicesRequest->elements));
    }
    if (extendedServicesRequest->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,extendedServicesRequest->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeExtendedServicesResponse(class_var, type, tagno, extendedServicesResponse)
int class_var;
int type;
int tagno;
ExtendedServicesResponse *extendedServicesResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (extendedServicesResponse->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,extendedServicesResponse->referenceId));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,extendedServicesResponse->operationStatus));
    if (extendedServicesResponse->diagnostics != 0) {
    {
    struct diagnostics_List21 *tmp1 = extendedServicesResponse->diagnostics;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (extendedServicesResponse->taskPackage != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,5,extendedServicesResponse->taskPackage));
    }
    if (extendedServicesResponse->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,extendedServicesResponse->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodePermissions(class_var, type, tagno, permissions)
int class_var;
int type;
int tagno;
Permissions *permissions;
{
    struct Permissions *tmp1 = permissions;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.userId));
        {
        struct allowableFunctions_List22 *tmp2 = tmp1->item.allowableFunctions;
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        while (tmp2 != 0) {
	    ATTACH(node2, EncodeInteger(UNIVERSAL, INTEGER, tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node1, node2);
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeClose(class_var, type, tagno, close)
int class_var;
int type;
int tagno;
Close *close;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (close->referenceId != 0) {
    ATTACH(node0, EncodeReferenceId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,close->referenceId));
    }
    ATTACH(node0, EncodeCloseReason(CLASS_NONE,TYPE_NONE,TAGNO_NONE,close->closeReason));
    if (close->diagnosticInformation != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,close->diagnosticInformation));
    }
    if (close->resourceReportFormat != 0) {
    ATTACH(node0, EncodeResourceReportId(BER_CONTEXT,CONSTRUCTED,4,close->resourceReportFormat));
    }
    if (close->resourceReport != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    ATTACH(node1, EncodeResourceReport(CLASS_NONE,TYPE_NONE,TAGNO_NONE,
				       (External *)close->resourceReport));
    ATTACH(node0, node1);
    }
    }
    if (close->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,close->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeCloseReason(class_var, type, tagno, closeReason)
int class_var;
int type;
int tagno;
CloseReason closeReason;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,closeReason);
    else
        node0 = EncodeInteger(BER_CONTEXT,211,closeReason);
    return node0;
}

ASN_NODE *
EncodeDiagnosticFormat(class_var, type, tagno, diagnosticFormat)
int class_var;
int type;
int tagno;
DiagnosticFormat *diagnosticFormat;
{
    struct DiagnosticFormat *tmp1 = diagnosticFormat;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.diagnostic != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        switch (tmp1->item.diagnostic->which) {
            case e26_defaultDiagRec: {
                ATTACH(node2, EncodeDefaultDiagFormat(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.diagnostic->u.defaultDiagRec));
                break;
            }
            case e26_explicitDiagnostic: {
                {
                ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
                ATTACH(node3, EncodeDiagFormat(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.diagnostic->u.explicitDiagnostic));
                ATTACH(node2, node3);
                }
                break;
            }
        }
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.message != 0) {
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.message));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeDiagFormat(class_var, type, tagno, diagFormat)
int class_var;
int type;
int tagno;
DiagFormat *diagFormat;
{
    ASN_NODE *node0;
    switch(diagFormat->which) {
        case e27_tooMany: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1000);
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,diagFormat->u.tooMany.tooManyWhat));
            if (diagFormat->u.tooMany.max != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,2,diagFormat->u.tooMany.max));
            }
            node0 = node1;
            break;
        }
        case e27_badSpec: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1001);
            ATTACH(node1, EncodeSpecification(BER_CONTEXT,CONSTRUCTED,1,diagFormat->u.badSpec.spec));
            if (diagFormat->u.badSpec.db != 0) {
            ATTACH(node1, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,2,diagFormat->u.badSpec.db));
            }
            if (diagFormat->u.badSpec.goodOnes != 0) {
            {
            struct goodOnes_List23 *tmp1 = diagFormat->u.badSpec.goodOnes;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
            while (tmp1 != 0) {
                ATTACH(node2, EncodeSpecification(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
        case e27_dbUnavail: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1002);
            ATTACH(node1, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,diagFormat->u.dbUnavail.db));
	    {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            if (diagFormat->u.dbUnavail.why.reasonCode != 0) {
            ATTACH(node2, EncodeInteger(BER_CONTEXT,1,diagFormat->u.dbUnavail.why.reasonCode));
            }
            if (diagFormat->u.dbUnavail.why.message != 0) {
            ATTACH(node2, EncodeInternationalString(BER_CONTEXT,2,diagFormat->u.dbUnavail.why.message));
            }
            ATTACH(node1, node2);
	    }
            node0 = node1;
            break;
        }
        case e27_unSupOp: {
            node0  = EncodeInteger(BER_CONTEXT,1003,diagFormat->u.unSupOp);
            break;
        }
        case e27_attribute: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1004);
            ATTACH(node1, EncodeOid(BER_CONTEXT,1,diagFormat->u.attribute.id));
            if (diagFormat->u.attribute.type != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,2,diagFormat->u.attribute.type));
            }
            if (diagFormat->u.attribute.value != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,3,diagFormat->u.attribute.value));
            }
            if (diagFormat->u.attribute.term != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
            ATTACH(node2, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,diagFormat->u.attribute.term));
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
        case e27_attCombo: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1005);
            ATTACH(node1, EncodeAttributeList(BER_CONTEXT,CONSTRUCTED,1,diagFormat->u.attCombo.unsupportedCombination));
            if (diagFormat->u.attCombo.recommendedAlternatives != 0) {
            {
            struct recommendedAlternatives_List24 *tmp1 = diagFormat->u.attCombo.recommendedAlternatives;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            while (tmp1 != 0) {
                ATTACH(node2, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
        case e27_term: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1006);
            if (diagFormat->u.term.problem != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,diagFormat->u.term.problem));
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,diagFormat->u.term.term));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e27_proximity: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1007);
            switch (diagFormat->u.proximity.which) {
                case e28_resultSets: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,1));
                    break;
                }
                case e28_badSet: {
                    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,diagFormat->u.proximity.u.badSet));
                    break;
                }
                case e28_relation: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,3,diagFormat->u.proximity.u.relation));
                    break;
                }
                case e28_unit: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,4,diagFormat->u.proximity.u.unit));
                    break;
                }
                case e28_distance: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,5,diagFormat->u.proximity.u.distance));
                    break;
                }
                case e28_attributes: {
                    {
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
                    ATTACH(node2, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,diagFormat->u.proximity.u.attributes));
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e28_ordered: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,7));
                    break;
                }
                case e28_exclusion: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,8));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_scan: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1008);
            switch (diagFormat->u.scan.which) {
                case e29_nonZeroStepSize: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,0));
                    break;
                }
                case e29_specifiedStepSize: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,1));
                    break;
                }
                case e29_termList1: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,3));
                    break;
                }
                case e29_termList2: {
                    {
                    struct termList2_List25 *tmp1 = diagFormat->u.scan.u.termList2;
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
                    while (tmp1 != 0) {
                        ATTACH(node2, EncodeAttributeList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e29_posInResponse: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,5,diagFormat->u.scan.u.posInResponse));
                    break;
                }
                case e29_resources: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,6));
                    break;
                }
                case e29_endOfList: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,7));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_sort: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1009);
            switch (diagFormat->u.sort.which) {
                case e30_sequence: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,0));
                    break;
                }
                case e30_noRsName: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,1));
                    break;
                }
                case e30_tooMany: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,2,diagFormat->u.sort.u.tooMany));
                    break;
                }
                case e30_incompatible: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,3));
                    break;
                }
                case e30_generic: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,4));
                    break;
                }
                case e30_dbSpecific: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,5));
                    break;
                }
                case e30_sortElement: {
                    {
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
                    ATTACH(node2, EncodeSortElement(CLASS_NONE,TYPE_NONE,TAGNO_NONE,diagFormat->u.sort.u.sortElement));
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e30_key: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,7,diagFormat->u.sort.u.key));
                    break;
                }
                case e30_action: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,8));
                    break;
                }
                case e30_illegal: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,9,diagFormat->u.sort.u.illegal));
                    break;
                }
                case e30_inputTooLarge: {
                    {
                    struct inputTooLarge_List26 *tmp1 = diagFormat->u.sort.u.inputTooLarge;
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,10);
                    while (tmp1 != 0) {
                        ATTACH(node2, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e30_aggregateTooLarge: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,11));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_segmentation: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1010);
            switch (diagFormat->u.segmentation.which) {
                case e31_segmentCount: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,0));
                    break;
                }
                case e31_segmentSize: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,1,diagFormat->u.segmentation.u.segmentSize));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_extServices: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1011);
            switch (diagFormat->u.extServices.which) {
                case e32_req: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,1,diagFormat->u.extServices.u.req));
                    break;
                }
                case e32_permission: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,2,diagFormat->u.extServices.u.permission));
                    break;
                }
                case e32_immediate: {
                    ATTACH(node1, EncodeInteger(BER_CONTEXT,3,diagFormat->u.extServices.u.immediate));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_accessCtrl: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1012);
            switch (diagFormat->u.accessCtrl.which) {
                case e33_noUser: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,1));
                    break;
                }
                case e33_refused: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,2));
                    break;
                }
                case e33_simple: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,3));
                    break;
                }
                case e33_oid: {
                    {
                    struct oid_List27 *tmp1 = diagFormat->u.accessCtrl.u.oid;
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
                    while (tmp1 != 0) {
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e33_alternative: {
                    {
                    struct alternative_List28 *tmp1 = diagFormat->u.accessCtrl.u.alternative;
                    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
                    while (tmp1 != 0) {
			ATTACH(node2, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node1, node2);
                    }
                    break;
                }
                case e33_pwdInv: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,6));
                    break;
                }
                case e33_pwdExp: {
                    ATTACH(node1, EncodeNull(BER_CONTEXT,7));
                    break;
                }
            }
            node0 = node1;
            }
            break;
        }
        case e27_recordSyntax: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1013);
            ATTACH(node1, EncodeOid(BER_CONTEXT,1,diagFormat->u.recordSyntax.unsupportedSyntax));
            if (diagFormat->u.recordSyntax.suggestedAlternatives != 0) {
            {
            struct suggestedAlternatives_List29 *tmp1 = diagFormat->u.recordSyntax.suggestedAlternatives;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            while (tmp1 != 0) {
		ATTACH(node2, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeExplain_Record(class_var, type, tagno, explain_Record)
int class_var;
int type;
int tagno;
Explain_Record *explain_Record;
{
    ASN_NODE *node0;
    switch(explain_Record->which) {
        case e34_targetInfo: {
            node0 = EncodeTargetInfo(BER_CONTEXT,CONSTRUCTED,0,explain_Record->u.targetInfo);
            break;
        }
        case e34_databaseInfo: {
            node0 = EncodeDatabaseInfo(BER_CONTEXT,CONSTRUCTED,1,explain_Record->u.databaseInfo);
            break;
        }
        case e34_schemaInfo: {
            node0 = EncodeSchemaInfo(BER_CONTEXT,CONSTRUCTED,2,explain_Record->u.schemaInfo);
            break;
        }
        case e34_tagSetInfo: {
            node0 = EncodeTagSetInfo(BER_CONTEXT,CONSTRUCTED,3,explain_Record->u.tagSetInfo);
            break;
        }
        case e34_recordSyntaxInfo: {
            node0 = EncodeRecordSyntaxInfo(BER_CONTEXT,CONSTRUCTED,4,explain_Record->u.recordSyntaxInfo);
            break;
        }
        case e34_attributeSetInfo: {
            node0 = EncodeAttributeSetInfo(BER_CONTEXT,CONSTRUCTED,5,explain_Record->u.attributeSetInfo);
            break;
        }
        case e34_termListInfo: {
            node0 = EncodeTermListInfo(BER_CONTEXT,CONSTRUCTED,6,explain_Record->u.termListInfo);
            break;
        }
        case e34_extendedServicesInfo: {
            node0 = EncodeExtendedServicesInfo(BER_CONTEXT,CONSTRUCTED,7,explain_Record->u.extendedServicesInfo);
            break;
        }
        case e34_attributeDetails: {
            node0 = EncodeAttributeDetails(BER_CONTEXT,CONSTRUCTED,8,explain_Record->u.attributeDetails);
            break;
        }
        case e34_termListDetails: {
            node0 = EncodeTermListDetails(BER_CONTEXT,CONSTRUCTED,9,explain_Record->u.termListDetails);
            break;
        }
        case e34_elementSetDetails: {
            node0 = EncodeElementSetDetails(BER_CONTEXT,CONSTRUCTED,10,explain_Record->u.elementSetDetails);
            break;
        }
        case e34_retrievalRecordDetails: {
            node0 = EncodeRetrievalRecordDetails(BER_CONTEXT,CONSTRUCTED,11,explain_Record->u.retrievalRecordDetails);
            break;
        }
        case e34_sortDetails: {
            node0 = EncodeSortDetails(BER_CONTEXT,CONSTRUCTED,12,explain_Record->u.sortDetails);
            break;
        }
        case e34_processing: {
            node0 = EncodeProcessingInformation(BER_CONTEXT,CONSTRUCTED,13,explain_Record->u.processing);
            break;
        }
        case e34_variants: {
            node0 = EncodeVariantSetInfo(BER_CONTEXT,CONSTRUCTED,14,explain_Record->u.variants);
            break;
        }
        case e34_units: {
            node0 = EncodeUnitInfo(BER_CONTEXT,CONSTRUCTED,15,explain_Record->u.units);
            break;
        }
        case e34_categoryList: {
            node0 = EncodeCategoryList(BER_CONTEXT,CONSTRUCTED,100,explain_Record->u.categoryList);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeTargetInfo(class_var, type, tagno, targetInfo)
int class_var;
int type;
int tagno;
TargetInfo *targetInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (targetInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,targetInfo->commonInfo));
    }
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,targetInfo->name));
    if (targetInfo->recent_news != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,targetInfo->recent_news));
    }
    if (targetInfo->icon != 0) {
    ATTACH(node0, EncodeIconObject(BER_CONTEXT,CONSTRUCTED,3,targetInfo->icon));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,4,targetInfo->namedResultSets));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,5,targetInfo->multipleDBsearch));
    if (targetInfo->maxResultSets != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,targetInfo->maxResultSets));
    }
    if (targetInfo->maxResultSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,7,targetInfo->maxResultSize));
    }
    if (targetInfo->maxTerms != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,8,targetInfo->maxTerms));
    }
    if (targetInfo->timeoutInterval != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,9,targetInfo->timeoutInterval));
    }
    if (targetInfo->welcomeMessage != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,10,targetInfo->welcomeMessage));
    }
    if (targetInfo->contactInfo != 0) {
    ATTACH(node0, EncodeContactInfo(BER_CONTEXT,CONSTRUCTED,11,targetInfo->contactInfo));
    }
    if (targetInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,12,targetInfo->description));
    }
    if (targetInfo->nicknames != 0) {
    {
    struct nicknames_List30 *tmp1 = targetInfo->nicknames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,13);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (targetInfo->usage_restrictions != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,14,targetInfo->usage_restrictions));
    }
    if (targetInfo->paymentAddr != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,15,targetInfo->paymentAddr));
    }
    if (targetInfo->hours != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,16,targetInfo->hours));
    }
    if (targetInfo->dbCombinations != 0) {
    {
    struct dbCombinations_List31 *tmp1 = targetInfo->dbCombinations;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,17);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDatabaseList(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (targetInfo->addresses != 0) {
    {
    struct addresses_List32 *tmp1 = targetInfo->addresses;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,18);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeNetworkAddress(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (targetInfo->languages != 0) {
    {
    struct languages_List33 *tmp1 = targetInfo->languages;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,101);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (targetInfo->commonAccessInfo != 0) {
    ATTACH(node0, EncodeAccessInfo(BER_CONTEXT,CONSTRUCTED,19,targetInfo->commonAccessInfo));
    }
    return node0;
}

ASN_NODE *
EncodeDatabaseInfo(class_var, type, tagno, databaseInfo)
int class_var;
int type;
int tagno;
DatabaseInfo *databaseInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (databaseInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,databaseInfo->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,databaseInfo->name));
    if (databaseInfo->explainDatabase != 0) {
    ATTACH(node0, EncodeNull(BER_CONTEXT,2));
    }
    if (databaseInfo->nicknames != 0) {
    {
    struct nicknames_List34 *tmp1 = databaseInfo->nicknames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (databaseInfo->icon != 0) {
    ATTACH(node0, EncodeIconObject(BER_CONTEXT,CONSTRUCTED,4,databaseInfo->icon));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,5,databaseInfo->user_fee));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,6,databaseInfo->available));
    if (databaseInfo->titleString != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,7,databaseInfo->titleString));
    }
    if (databaseInfo->keywords != 0) {
    {
    struct keywords_List35 *tmp1 = databaseInfo->keywords;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,8);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeHumanString(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (databaseInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,9,databaseInfo->description));
    }
    if (databaseInfo->associatedDbs != 0) {
    ATTACH(node0, EncodeDatabaseList(BER_CONTEXT,CONSTRUCTED,10,databaseInfo->associatedDbs));
    }
    if (databaseInfo->subDbs != 0) {
    ATTACH(node0, EncodeDatabaseList(BER_CONTEXT,CONSTRUCTED,11,databaseInfo->subDbs));
    }
    if (databaseInfo->disclaimers != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,12,databaseInfo->disclaimers));
    }
    if (databaseInfo->news != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,13,databaseInfo->news));
    }
    if (databaseInfo->recordCount != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,14);
    switch (databaseInfo->recordCount->which) {
        case e35_actualNumber: {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,0,databaseInfo->recordCount->u.actualNumber));
            break;
        }
        case e35_approxNumber: {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,databaseInfo->recordCount->u.approxNumber));
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    if (databaseInfo->defaultOrder != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,15,databaseInfo->defaultOrder));
    }
    if (databaseInfo->avRecordSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,16,databaseInfo->avRecordSize));
    }
    if (databaseInfo->maxRecordSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,17,databaseInfo->maxRecordSize));
    }
    if (databaseInfo->hours != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,18,databaseInfo->hours));
    }
    if (databaseInfo->bestTime != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,19,databaseInfo->bestTime));
    }
    if (databaseInfo->lastUpdate != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,20,databaseInfo->lastUpdate));
    }
    if (databaseInfo->updateInterval != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,21,databaseInfo->updateInterval));
    }
    if (databaseInfo->coverage != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,22,databaseInfo->coverage));
    }
    if (databaseInfo->proprietary != 0) {
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,23,databaseInfo->proprietary));
    }
    if (databaseInfo->copyrightText != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,24,databaseInfo->copyrightText));
    }
    if (databaseInfo->copyrightNotice != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,25,databaseInfo->copyrightNotice));
    }
    if (databaseInfo->producerContactInfo != 0) {
    ATTACH(node0, EncodeContactInfo(BER_CONTEXT,CONSTRUCTED,26,databaseInfo->producerContactInfo));
    }
    if (databaseInfo->supplierContactInfo != 0) {
    ATTACH(node0, EncodeContactInfo(BER_CONTEXT,CONSTRUCTED,27,databaseInfo->supplierContactInfo));
    }
    if (databaseInfo->submissionContactInfo != 0) {
    ATTACH(node0, EncodeContactInfo(BER_CONTEXT,CONSTRUCTED,28,databaseInfo->submissionContactInfo));
    }
    if (databaseInfo->accessInfo != 0) {
    ATTACH(node0, EncodeAccessInfo(BER_CONTEXT,CONSTRUCTED,29,databaseInfo->accessInfo));
    }
    return node0;
}

ASN_NODE *
EncodeSchemaInfo(class_var, type, tagno, schemaInfo)
int class_var;
int type;
int tagno;
SchemaInfo *schemaInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (schemaInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,schemaInfo->commonInfo));
    }
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,schemaInfo->schema));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,schemaInfo->name));
    if (schemaInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,3,schemaInfo->description));
    }
    if (schemaInfo->tagTypeMapping != 0) {
    {
    struct tagTypeMapping_List36 *tmp1 = schemaInfo->tagTypeMapping;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node2, EncodeInteger(BER_CONTEXT,0,tmp1->item.tagType));
        if (tmp1->item.tagSet != 0) {
        ATTACH(node2, EncodeOid(BER_CONTEXT,1,tmp1->item.tagSet));
        }
        if (tmp1->item.defaultTagType != 0) {
        ATTACH(node2, EncodeNull(BER_CONTEXT,2));
        }
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (schemaInfo->recordStructure != 0) {
    {
    struct recordStructure_List37 *tmp1 = schemaInfo->recordStructure;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeElementInfo(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeElementInfo(class_var, type, tagno, elementInfo)
int class_var;
int type;
int tagno;
ElementInfo *elementInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,elementInfo->elementName));
    ATTACH(node0, EncodePath(BER_CONTEXT,CONSTRUCTED,2,elementInfo->elementTagPath));
    if (elementInfo->dataType != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    ATTACH(node1, EncodeElementDataType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,elementInfo->dataType));
    ATTACH(node0, node1);
    }
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,4,elementInfo->required));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,5,elementInfo->repeatable));
    if (elementInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,6,elementInfo->description));
    }
    return node0;
}

ASN_NODE *
EncodePath(class_var, type, tagno, path)
int class_var;
int type;
int tagno;
Path *path;
{
    struct Path *tmp1 = path;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node1, EncodeInteger(BER_CONTEXT,1,tmp1->item.tagType));
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        ATTACH(node2, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.tagValue));
        ATTACH(node1, node2);
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeElementDataType(class_var, type, tagno, elementDataType)
int class_var;
int type;
int tagno;
ElementDataType *elementDataType;
{
    ASN_NODE *node0;
    switch(elementDataType->which) {
        case e36_primitive: {
            node0 = EncodePrimitiveDataType(BER_CONTEXT,CONSTRUCTED,0,elementDataType->u.primitive);
            break;
        }
        case e36_structured: {
            {
            struct structured_List38 *tmp1 = elementDataType->u.structured;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            while (tmp1 != 0) {
                ATTACH(node1, EncodeElementInfo(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodePrimitiveDataType(class_var, type, tagno, primitiveDataType)
int class_var;
int type;
int tagno;
PrimitiveDataType primitiveDataType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,primitiveDataType);
    else
        node0 = EncodeInteger(UNIVERSAL,INTEGER,primitiveDataType);
    return node0;
}

ASN_NODE *
EncodeTagSetInfo(class_var, type, tagno, tagSetInfo)
int class_var;
int type;
int tagno;
TagSetInfo *tagSetInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (tagSetInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,tagSetInfo->commonInfo));
    }
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,tagSetInfo->tagSet));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,tagSetInfo->name));
    if (tagSetInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,3,tagSetInfo->description));
    }
    if (tagSetInfo->elements != 0) {
    {
    struct elements_List39 *tmp1 = tagSetInfo->elements;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node2, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.elementname));
        if (tmp1->item.nicknames != 0) {
        {
        struct nicknames_List40 *tmp2 = tmp1->item.nicknames;
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        while (tmp2 != 0) {
            ATTACH(node3, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node2, node3);
        }
        }
        {
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
        ATTACH(node3, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.elementTag));
        ATTACH(node2, node3);
        }
        if (tmp1->item.description != 0) {
        ATTACH(node2, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,4,tmp1->item.description));
        }
        if (tmp1->item.dataType != 0) {
        {
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
        ATTACH(node3, EncodePrimitiveDataType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.dataType));
        ATTACH(node2, node3);
        }
        }
        if (tmp1->item.otherTagInfo != 0) {
        ATTACH(node2, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.otherTagInfo));
        }
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeRecordSyntaxInfo(class_var, type, tagno, recordSyntaxInfo)
int class_var;
int type;
int tagno;
RecordSyntaxInfo *recordSyntaxInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (recordSyntaxInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,recordSyntaxInfo->commonInfo));
    }
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,recordSyntaxInfo->recordSyntax));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,recordSyntaxInfo->name));
    if (recordSyntaxInfo->transferSyntaxes != 0) {
    {
    struct transferSyntaxes_List41 *tmp1 = recordSyntaxInfo->transferSyntaxes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (recordSyntaxInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,4,recordSyntaxInfo->description));
    }
    if (recordSyntaxInfo->asn1Module != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,5,recordSyntaxInfo->asn1Module));
    }
    if (recordSyntaxInfo->abstractStructure != 0) {
    {
    struct abstractStructure_List42 *tmp1 = recordSyntaxInfo->abstractStructure;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeElementInfo(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeAttributeSetInfo(class_var, type, tagno, attributeSetInfo)
int class_var;
int type;
int tagno;
AttributeSetInfo *attributeSetInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (attributeSetInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,attributeSetInfo->commonInfo));
    }
    ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,1,attributeSetInfo->attributeSet));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,attributeSetInfo->name));
    if (attributeSetInfo->attributes != 0) {
    {
    struct attributes_List43 *tmp1 = attributeSetInfo->attributes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributeType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (attributeSetInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,4,attributeSetInfo->description));
    }
    return node0;
}

ASN_NODE *
EncodeAttributeType(class_var, type, tagno, attributeType)
int class_var;
int type;
int tagno;
AttributeType *attributeType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (attributeType->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,attributeType->name));
    }
    if (attributeType->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,attributeType->description));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,attributeType->attributeType));
    {
    struct attributeValues_List44 *tmp1 = attributeType->attributeValues;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributeDescription(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeAttributeDescription(class_var, type, tagno, attributeDescription)
int class_var;
int type;
int tagno;
AttributeDescription *attributeDescription;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (attributeDescription->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,attributeDescription->name));
    }
    if (attributeDescription->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,attributeDescription->description));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,attributeDescription->attributeValue));
    ATTACH(node0, node1);
    }
    if (attributeDescription->equivalentAttributes != 0) {
    {
    struct equivalentAttributes_List45 *tmp1 = attributeDescription->equivalentAttributes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeTermListInfo(class_var, type, tagno, termListInfo)
int class_var;
int type;
int tagno;
TermListInfo *termListInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (termListInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,termListInfo->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,termListInfo->databaseName));
    {
    struct termLists_List46 *tmp1 = termListInfo->termLists;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node2, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.name));
        if (tmp1->item.title != 0) {
        ATTACH(node2, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,tmp1->item.title));
        }
        if (tmp1->item.searchCost != 0) {
        ATTACH(node2, EncodeInteger(BER_CONTEXT,3,tmp1->item.searchCost));
        }
        ATTACH(node2, EncodeBoolean(BER_CONTEXT,4,tmp1->item.scanable));
        if (tmp1->item.broader != 0) {
        {
        struct broader_List47 *tmp2 = tmp1->item.broader;
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
        while (tmp2 != 0) {
            ATTACH(node3, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node2, node3);
        }
        }
        if (tmp1->item.narrower != 0) {
        {
        struct narrower_List48 *tmp2 = tmp1->item.narrower;
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
        while (tmp2 != 0) {
            ATTACH(node3, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node2, node3);
        }
        }
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeExtendedServicesInfo(class_var, type, tagno, extendedServicesInfo)
int class_var;
int type;
int tagno;
ExtendedServicesInfo *extendedServicesInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (extendedServicesInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,extendedServicesInfo->commonInfo));
    }
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,extendedServicesInfo->type));
    if (extendedServicesInfo->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,extendedServicesInfo->name));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,3,extendedServicesInfo->privateType));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,5,extendedServicesInfo->restrictionsApply));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,6,extendedServicesInfo->feeApply));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,7,extendedServicesInfo->available));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,8,extendedServicesInfo->retentionSupported));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,9,extendedServicesInfo->waitAction));
    if (extendedServicesInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,10,extendedServicesInfo->description));
    }
    if (extendedServicesInfo->specificExplain != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,11,extendedServicesInfo->specificExplain));
    }
    if (extendedServicesInfo->esASN != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,12,extendedServicesInfo->esASN));
    }
    return node0;
}

ASN_NODE *
EncodeAttributeDetails(class_var, type, tagno, attributeDetails)
int class_var;
int type;
int tagno;
AttributeDetails *attributeDetails;
{
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
  if (attributeDetails->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,attributeDetails->commonInfo));
  }
  ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,attributeDetails->databaseName));
  if (attributeDetails->attributesBySet != 0) {
    {
      struct attributesBySet_List49 *tmp1 = attributeDetails->attributesBySet;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributeSetDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (attributeDetails->attributeCombinations != 0) {
    ATTACH(node0, EncodeAttributeCombinations(BER_CONTEXT,CONSTRUCTED,3,attributeDetails->attributeCombinations));
  }
  return node0;
}

ASN_NODE *
EncodeAttributeSetDetails(class_var, type, tagno, attributeSetDetails)
int class_var;
int type;
int tagno;
AttributeSetDetails *attributeSetDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,0,attributeSetDetails->attributeSet));
    {
    struct attributesByType_List50 *tmp1 = attributeSetDetails->attributesByType;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributeTypeDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeAttributeTypeDetails(class_var, type, tagno, attributeTypeDetails)
int class_var;
int type;
int tagno;
AttributeTypeDetails *attributeTypeDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,0,attributeTypeDetails->attributeType));
    if (attributeTypeDetails->defaultIfOmitted != 0) {
    ATTACH(node0, EncodeOmittedAttributeInterpretation(BER_CONTEXT,CONSTRUCTED,1,attributeTypeDetails->defaultIfOmitted));
    }
    if (attributeTypeDetails->attributeValues != 0) {
    {
    struct attributeValues_List51 *tmp1 = attributeTypeDetails->attributeValues;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeAttributeValue(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeOmittedAttributeInterpretation(class_var, type, tagno, omittedAttributeInterpretation)
int class_var;
int type;
int tagno;
OmittedAttributeInterpretation *omittedAttributeInterpretation;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (omittedAttributeInterpretation->defaultValue != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,omittedAttributeInterpretation->defaultValue));
    ATTACH(node0, node1);
    }
    }
    if (omittedAttributeInterpretation->defaultDescription != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,omittedAttributeInterpretation->defaultDescription));
    }
    return node0;
}

ASN_NODE *
EncodeAttributeValue(class_var, type, tagno, attributeValue)
int class_var;
int type;
int tagno;
AttributeValue *attributeValue;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,attributeValue->value));
    ATTACH(node0, node1);
    }
    if (attributeValue->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,attributeValue->description));
    }
    if (attributeValue->subAttributes != 0) {
    {
    struct subAttributes_List52 *tmp1 = attributeValue->subAttributes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (attributeValue->superAttributes != 0) {
    {
    struct superAttributes_List53 *tmp1 = attributeValue->superAttributes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (attributeValue->partialSupport != 0) {
    ATTACH(node0, EncodeNull(BER_CONTEXT,4));
    }
    return node0;
}

ASN_NODE *
EncodeTermListDetails(class_var, type, tagno, termListDetails)
int class_var;
int type;
int tagno;
TermListDetails *termListDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (termListDetails->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,termListDetails->commonInfo));
    }
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,termListDetails->termListName));
    if (termListDetails->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,termListDetails->description));
    }
    if (termListDetails->attributes != 0) {
    ATTACH(node0, EncodeAttributeCombinations(BER_CONTEXT,CONSTRUCTED,3,termListDetails->attributes));
    }
    if (termListDetails->scanInfo != 0) {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    if (termListDetails->scanInfo->maxStepSize != 0) {
    ATTACH(node1, EncodeInteger(BER_CONTEXT,0,termListDetails->scanInfo->maxStepSize));
    }
    if (termListDetails->scanInfo->collatingSequence != 0) {
    ATTACH(node1, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,termListDetails->scanInfo->collatingSequence));
    }
    if (termListDetails->scanInfo->increasing != 0) {
    ATTACH(node1, EncodeBoolean(BER_CONTEXT,2,termListDetails->scanInfo->increasing));
    }
    ATTACH(node0, node1);
    }
    if (termListDetails->estNumberTerms != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,termListDetails->estNumberTerms));
    }
    if (termListDetails->sampleTerms != 0) {
    {
    struct sampleTerms_List54 *tmp1 = termListDetails->sampleTerms;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeElementSetDetails(class_var, type, tagno, elementSetDetails)
int class_var;
int type;
int tagno;
ElementSetDetails *elementSetDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (elementSetDetails->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,elementSetDetails->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,elementSetDetails->databaseName));
    ATTACH(node0, EncodeElementSetName(BER_CONTEXT,CONSTRUCTED,2,elementSetDetails->elementSetName));
    ATTACH(node0, EncodeOid(BER_CONTEXT,3,elementSetDetails->recordSyntax));
    ATTACH(node0, EncodeOid(BER_CONTEXT,4,elementSetDetails->schema));
    if (elementSetDetails->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,5,elementSetDetails->description));
    }
    if (elementSetDetails->detailsPerElement != 0) {
    {
    struct detailsPerElement_List55 *tmp1 = elementSetDetails->detailsPerElement;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
    while (tmp1 != 0) {
        ATTACH(node1, EncodePerElementDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeRetrievalRecordDetails(class_var, type, tagno, retrievalRecordDetails)
int class_var;
int type;
int tagno;
RetrievalRecordDetails *retrievalRecordDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (retrievalRecordDetails->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,retrievalRecordDetails->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,retrievalRecordDetails->databaseName));
    ATTACH(node0, EncodeOid(BER_CONTEXT,2,retrievalRecordDetails->schema));
    ATTACH(node0, EncodeOid(BER_CONTEXT,3,retrievalRecordDetails->recordSyntax));
    if (retrievalRecordDetails->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,4,retrievalRecordDetails->description));
    }
    if (retrievalRecordDetails->detailsPerElement != 0) {
    {
    struct detailsPerElement_List56 *tmp1 = retrievalRecordDetails->detailsPerElement;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ATTACH(node1, EncodePerElementDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodePerElementDetails(class_var, type, tagno, perElementDetails)
int class_var;
int type;
int tagno;
PerElementDetails *perElementDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (perElementDetails->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,perElementDetails->name));
    }
    if (perElementDetails->recordTag != 0) {
    ATTACH(node0, EncodeRecordTag(BER_CONTEXT,CONSTRUCTED,1,perElementDetails->recordTag));
    }
    if (perElementDetails->schemaTags != 0) {
    {
    struct schemaTags_List57 *tmp1 = perElementDetails->schemaTags;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodePath(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (perElementDetails->maxSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,perElementDetails->maxSize));
    }
    if (perElementDetails->minSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,perElementDetails->minSize));
    }
    if (perElementDetails->avgSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,5,perElementDetails->avgSize));
    }
    if (perElementDetails->fixedSize != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,perElementDetails->fixedSize));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,8,perElementDetails->repeatable));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,9,perElementDetails->required));
    if (perElementDetails->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,12,perElementDetails->description));
    }
    if (perElementDetails->contents != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,13,perElementDetails->contents));
    }
    if (perElementDetails->billingInfo != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,14,perElementDetails->billingInfo));
    }
    if (perElementDetails->restrictions != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,15,perElementDetails->restrictions));
    }
    if (perElementDetails->alternateNames != 0) {
    {
    struct alternateNames_List58 *tmp1 = perElementDetails->alternateNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,16);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (perElementDetails->genericNames != 0) {
    {
    struct genericNames_List59 *tmp1 = perElementDetails->genericNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,17);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (perElementDetails->searchAccess != 0) {
    ATTACH(node0, EncodeAttributeCombinations(BER_CONTEXT,CONSTRUCTED,18,perElementDetails->searchAccess));
    }
    return node0;
}

ASN_NODE *
EncodeRecordTag(class_var, type, tagno, recordTag)
int class_var;
int type;
int tagno;
RecordTag *recordTag;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (recordTag->qualifier != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,recordTag->qualifier));
    ATTACH(node0, node1);
    }
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,recordTag->tagValue));
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeSortDetails(class_var, type, tagno, sortDetails)
int class_var;
int type;
int tagno;
SortDetails *sortDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (sortDetails->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,sortDetails->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,sortDetails->databaseName));
    if (sortDetails->sortKeys != 0) {
    {
    struct sortKeys_List60 *tmp1 = sortDetails->sortKeys;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeSortKeyDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeSortKeyDetails(class_var, type, tagno, sortKeyDetails)
int class_var;
int type;
int tagno;
SortKeyDetails *sortKeyDetails;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (sortKeyDetails->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,0,sortKeyDetails->description));
    }
    if (sortKeyDetails->elementSpecifications != 0) {
    {
    struct elementSpecifications_List61 *tmp1 = sortKeyDetails->elementSpecifications;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeSpecification(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (sortKeyDetails->attributeSpecifications != 0) {
    ATTACH(node0, EncodeAttributeCombinations(BER_CONTEXT,CONSTRUCTED,2,sortKeyDetails->attributeSpecifications));
    }
    if (sortKeyDetails->sortType != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    switch (sortKeyDetails->sortType->which) {
        case e37_character: {
            ATTACH(node1, EncodeNull(BER_CONTEXT,0));
            break;
        }
        case e37_numeric: {
            ATTACH(node1, EncodeNull(BER_CONTEXT,1));
            break;
        }
        case e37_structured: {
            ATTACH(node1, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,sortKeyDetails->sortType->u.structured));
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    if (sortKeyDetails->caseSensitivity != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,sortKeyDetails->caseSensitivity));
    }
    return node0;
}

ASN_NODE *
EncodeProcessingInformation(class_var, type, tagno, processingInformation)
int class_var;
int type;
int tagno;
ProcessingInformation *processingInformation;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (processingInformation->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,processingInformation->commonInfo));
    }
    ATTACH(node0, EncodeDatabaseName(BER_CONTEXT,CONSTRUCTED,1,processingInformation->databaseName));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,processingInformation->processingContext));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,processingInformation->name));
    ATTACH(node0, EncodeOid(BER_CONTEXT,4,processingInformation->oid));
    if (processingInformation->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,5,processingInformation->description));
    }
    if (processingInformation->instructions != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,6,processingInformation->instructions));
    }
    return node0;
}

ASN_NODE *
EncodeVariantSetInfo(class_var, type, tagno, variantSetInfo)
int class_var;
int type;
int tagno;
VariantSetInfo *variantSetInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (variantSetInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,variantSetInfo->commonInfo));
    }
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,variantSetInfo->variantSet));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,variantSetInfo->name));
    if (variantSetInfo->variants != 0) {
    {
    struct variants_List62 *tmp1 = variantSetInfo->variants;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeVariantClass(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeVariantClass(class_var, type, tagno, variantClass)
int class_var;
int type;
int tagno;
VariantClass *variantClass;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (variantClass->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,variantClass->name));
    }
    if (variantClass->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,variantClass->description));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,variantClass->variantClass));
    {
    struct variantTypes_List63 *tmp1 = variantClass->variantTypes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeVariantType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeVariantType(class_var, type, tagno, variantType)
int class_var;
int type;
int tagno;
VariantType *variantType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (variantType->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,variantType->name));
    }
    if (variantType->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,variantType->description));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,variantType->variantType));
    if (variantType->variantValue != 0) {
    ATTACH(node0, EncodeVariantValue(BER_CONTEXT,CONSTRUCTED,3,variantType->variantValue));
    }
    return node0;
}

ASN_NODE *
EncodeVariantValue(class_var, type, tagno, variantValue)
int class_var;
int type;
int tagno;
VariantValue *variantValue;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    ATTACH(node1, EncodePrimitiveDataType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,variantValue->dataType));
    ATTACH(node0, node1);
    }
    if (variantValue->values != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeValueSet(CLASS_NONE,TYPE_NONE,TAGNO_NONE,variantValue->values));
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeValueSet(class_var, type, tagno, valueSet)
int class_var;
int type;
int tagno;
ValueSet *valueSet;
{
    ASN_NODE *node0;
    switch(valueSet->which) {
        case e38_range: {
            node0 = EncodeValueRange(BER_CONTEXT,CONSTRUCTED,0,valueSet->u.range);
            break;
        }
        case e38_enumerated: {
            {
            struct enumerated_List64 *tmp1 = valueSet->u.enumerated;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            while (tmp1 != 0) {
                ATTACH(node1, EncodeValueDescription(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeValueRange(class_var, type, tagno, valueRange)
int class_var;
int type;
int tagno;
ValueRange *valueRange;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (valueRange->lower != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    ATTACH(node1, EncodeValueDescription(CLASS_NONE,TYPE_NONE,TAGNO_NONE,valueRange->lower));
    ATTACH(node0, node1);
    }
    }
    if (valueRange->upper != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeValueDescription(CLASS_NONE,TYPE_NONE,TAGNO_NONE,valueRange->upper));
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeValueDescription(class_var, type, tagno, valueDescription)
int class_var;
int type;
int tagno;
ValueDescription *valueDescription;
{
    ASN_NODE *node0;
    switch(valueDescription->which) {
        case e39_integer: {
            node0 = EncodeInteger(UNIVERSAL, INTEGER, valueDescription->u.integer);
            break;
        }
        case e39_string: {
            node0 = EncodeInternationalString(CLASS_NONE,TAGNO_NONE,valueDescription->u.string);
            break;
        }
        case e39_octets: {
            node0 = EncodeOctetString(UNIVERSAL, OCTETSTRING, valueDescription->u.octets);
            break;
        }
        case e39_oid: {
            node0 = EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, valueDescription->u.oid);
            break;
        }
        case e39_unit: {
            node0 = EncodeUnit(BER_CONTEXT,CONSTRUCTED,1,valueDescription->u.unit);
            break;
        }
        case e39_valueAndUnit: {
            node0 = EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,2,valueDescription->u.valueAndUnit);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeUnitInfo(class_var, type, tagno, unitInfo)
int class_var;
int type;
int tagno;
UnitInfo *unitInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (unitInfo->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,unitInfo->commonInfo));
    }
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,unitInfo->unitSystem));
    if (unitInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,unitInfo->description));
    }
    if (unitInfo->units != 0) {
    {
    struct units_List65 *tmp1 = unitInfo->units;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeUnitType(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeUnitType(class_var, type, tagno, unitType)
int class_var;
int type;
int tagno;
UnitType *unitType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (unitType->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,unitType->name));
    }
    if (unitType->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,unitType->description));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,unitType->unitType));
    ATTACH(node0, node1);
    }
    {
    struct units_List66 *tmp1 = unitType->units;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeUnits(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeUnits(class_var, type, tagno, units)
int class_var;
int type;
int tagno;
Units *units;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (units->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,units->name));
    }
    if (units->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,units->description));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,units->unit));
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeCategoryList(class_var, type, tagno, categoryList)
int class_var;
int type;
int tagno;
CategoryList *categoryList;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (categoryList->commonInfo != 0) {
    ATTACH(node0, EncodeCommonInfo(BER_CONTEXT,CONSTRUCTED,0,categoryList->commonInfo));
    }
    {
    struct categories_List67 *tmp1 = categoryList->categories;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeCategoryInfo(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeCategoryInfo(class_var, type, tagno, categoryInfo)
int class_var;
int type;
int tagno;
CategoryInfo *categoryInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,categoryInfo->category));
    if (categoryInfo->originalCategory != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,categoryInfo->originalCategory));
    }
    if (categoryInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,3,categoryInfo->description));
    }
    if (categoryInfo->asn1Module != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,categoryInfo->asn1Module));
    }
    return node0;
}

ASN_NODE *
EncodeCommonInfo(class_var, type, tagno, commonInfo)
int class_var;
int type;
int tagno;
CommonInfo *commonInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (commonInfo->dateAdded != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,0,commonInfo->dateAdded));
    }
    if (commonInfo->dateChanged != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,1,commonInfo->dateChanged));
    }
    if (commonInfo->expiry != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,2,commonInfo->expiry));
    }
    if (commonInfo->humanString_Language != 0) {
    ATTACH(node0, EncodeLanguageCode(BER_CONTEXT,3,
				     (InternationalString)
				     commonInfo->humanString_Language));
    }
    if (commonInfo->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,commonInfo->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeHumanString(class_var, type, tagno, humanString)
int class_var;
int type;
int tagno;
HumanString *humanString;
{
    struct HumanString *tmp1 = humanString;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.language != 0) {
        ATTACH(node1, EncodeLanguageCode(BER_CONTEXT,0,
					 (InternationalString)
					 tmp1->item.language));
        }
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.text));
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeIconObject(class_var, type, tagno, iconObject)
int class_var;
int type;
int tagno;
IconObject *iconObject;
{
    struct IconObject *tmp1 = iconObject;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        switch (tmp1->item.bodyType.which) {
            case e40_ianaType: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.bodyType.u.ianaType));
                break;
            }
            case e40_z3950type: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.bodyType.u.z3950type));
                break;
            }
            case e40_otherType: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,3,tmp1->item.bodyType.u.otherType));
                break;
            }
        }
        ATTACH(node1, node2);
        }
        ATTACH(node1, EncodeOctetString(BER_CONTEXT,2,tmp1->item.content));
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}


ASN_NODE *
EncodeContactInfo(class_var, type, tagno, contactInfo)
int class_var;
int type;
int tagno;
ContactInfo *contactInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (contactInfo->name != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,contactInfo->name));
    }
    if (contactInfo->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,contactInfo->description));
    }
    if (contactInfo->address != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,2,contactInfo->address));
    }
    if (contactInfo->email != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,contactInfo->email));
    }
    if (contactInfo->phone != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,contactInfo->phone));
    }
    return node0;
}

ASN_NODE *
EncodeNetworkAddress(class_var, type, tagno, networkAddress)
int class_var;
int type;
int tagno;
NetworkAddress *networkAddress;
{
    ASN_NODE *node0;
    switch(networkAddress->which) {
        case e41_internetAddress: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,0,networkAddress->u.internetAddress.hostAddress));
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,networkAddress->u.internetAddress.port));
            node0 = node1;
            break;
        }
        case e41_osiPresentationAddress: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,0,networkAddress->u.osiPresentationAddress.pSel));
            if (networkAddress->u.osiPresentationAddress.sSel != 0) {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,networkAddress->u.osiPresentationAddress.sSel));
            }
            if (networkAddress->u.osiPresentationAddress.tSel != 0) {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,networkAddress->u.osiPresentationAddress.tSel));
            }
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,3,networkAddress->u.osiPresentationAddress.nSap));
            node0 = node1;
            break;
        }
        case e41_other: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,0,networkAddress->u.other.type));
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,networkAddress->u.other.address));
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeAccessInfo(class_var, type, tagno, accessInfo)
int class_var;
int type;
int tagno;
AccessInfo *accessInfo;
{
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
  if (accessInfo->queryTypesSupported != 0) {
    {
      struct queryTypesSupported_List68 *tmp1 = accessInfo->queryTypesSupported;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeQueryTypeDetails(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->diagnosticsSets != 0) {
    {
      struct diagnosticsSets_List69 *tmp1 = accessInfo->diagnosticsSets;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
      while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->attributeSetIds != 0) {
    {
      struct attributeSetIds_List70 *tmp1 = accessInfo->attributeSetIds;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, 
				(ObjectIdentifier)tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->schemas != 0) {
    {
      struct schemas_List71 *tmp1 = accessInfo->schemas;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
      while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->recordSyntaxes != 0) {
    {
      struct recordSyntaxes_List72 *tmp1 = accessInfo->recordSyntaxes;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
      while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->resourceChallenges != 0) {
    {
      struct resourceChallenges_List73 *tmp1 = accessInfo->resourceChallenges;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
      while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->restrictedAccess != 0) {
    ATTACH(node0, EncodeAccessRestrictions(BER_CONTEXT,CONSTRUCTED,6,accessInfo->restrictedAccess));
  }
  if (accessInfo->costInfo != 0) {
    ATTACH(node0, EncodeCosts(BER_CONTEXT,CONSTRUCTED,8,accessInfo->costInfo));
  }
  if (accessInfo->variantSets != 0) {
    {
      struct variantSets_List74 *tmp1 = accessInfo->variantSets;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,9);
      while (tmp1 != 0) {
	ATTACH(node1, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->elementSetNames != 0) {
    {
      struct elementSetNames_List75 *tmp1 = accessInfo->elementSetNames;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,10);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeElementSetName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  if (accessInfo->unitSystems != 0) {
    {
      struct unitSystems_List76 *tmp1 = accessInfo->unitSystems;
      ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,11);
      while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
      }
      ATTACH(node0, node1);
    }
  }
  return node0;
}

ASN_NODE *
EncodeQueryTypeDetails(class_var, type, tagno, queryTypeDetails)
int class_var;
int type;
int tagno;
QueryTypeDetails *queryTypeDetails;
{
    ASN_NODE *node0;
    switch(queryTypeDetails->which) {
        case e42_private: {
            node0 = EncodePrivateCapabilities(BER_CONTEXT,CONSTRUCTED,0,queryTypeDetails->u.private_var);
            break;
        }
        case e42_rpn: {
            node0 = EncodeRpnCapabilities(BER_CONTEXT,CONSTRUCTED,1,queryTypeDetails->u.rpn);
            break;
        }
        case e42_iso8777: {
            node0 = EncodeIso8777Capabilities(BER_CONTEXT,CONSTRUCTED,2,queryTypeDetails->u.iso8777);
            break;
        }
        case e42_z39_58: {
            node0 = EncodeHumanString(BER_CONTEXT,CONSTRUCTED,100,queryTypeDetails->u.z39_58);
            break;
        }
        case e42_erpn: {
            node0 = EncodeRpnCapabilities(BER_CONTEXT,CONSTRUCTED,101,queryTypeDetails->u.erpn);
            break;
        }
        case e42_rankedList: {
            node0 = EncodeHumanString(BER_CONTEXT,CONSTRUCTED,102,queryTypeDetails->u.rankedList);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodePrivateCapabilities(class_var, type, tagno, privateCapabilities)
int class_var;
int type;
int tagno;
PrivateCapabilities *privateCapabilities;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (privateCapabilities->operators != 0) {
    {
    struct operators_List77 *tmp1 = privateCapabilities->operators;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node2, EncodeInternationalString(BER_CONTEXT,0,tmp1->item.operator_var));
        if (tmp1->item.description != 0) {
        ATTACH(node2, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.description));
        }
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (privateCapabilities->searchKeys != 0) {
    {
    struct searchKeys_List78 *tmp1 = privateCapabilities->searchKeys;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeSearchKey(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (privateCapabilities->description != 0) {
    {
    struct description_List79 *tmp1 = privateCapabilities->description;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeHumanString(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeRpnCapabilities(class_var, type, tagno, rpnCapabilities)
int class_var;
int type;
int tagno;
RpnCapabilities *rpnCapabilities;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (rpnCapabilities->operators != 0) {
    {
    struct operators_List80 *tmp1 = rpnCapabilities->operators;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    while (tmp1 != 0) {
	ATTACH(node1, EncodeInteger(PRIMITIVE,INTEGER,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,rpnCapabilities->resultSetAsOperandSupported));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,2,rpnCapabilities->restrictionOperandSupported));
    if (rpnCapabilities->proximity != 0) {
    ATTACH(node0, EncodeProximitySupport(BER_CONTEXT,CONSTRUCTED,3,rpnCapabilities->proximity));
    }
    return node0;
}

ASN_NODE *
EncodeIso8777Capabilities(class_var, type, tagno, iso8777Capabilities)
int class_var;
int type;
int tagno;
Iso8777Capabilities *iso8777Capabilities;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    struct searchKeys_List81 *tmp1 = iso8777Capabilities->searchKeys;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeSearchKey(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    if (iso8777Capabilities->restrictions != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,iso8777Capabilities->restrictions));
    }
    return node0;
}

ASN_NODE *
EncodeProximitySupport(class_var, type, tagno, proximitySupport)
int class_var;
int type;
int tagno;
ProximitySupport *proximitySupport;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,0,proximitySupport->anySupport));
    if (proximitySupport->unitsSupported != 0) {
    {
    struct unitsSupported_List82 *tmp1 = proximitySupport->unitsSupported;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
	switch (tmp1->item->which) {
        case e43_known: {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,tmp1->item->u.known));
            break;
        }
        case e43_private: {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeInteger(BER_CONTEXT,0,tmp1->item->u.private_var.unit));
            if (tmp1->item->u.private_var.description != 0) {
            {
            ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node3, EncodeHumanString(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item->u.private_var.description));
            ATTACH(node2, node3);
            }
            }
            ATTACH(node1, node2);
            break;
        }
	}
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}


ASN_NODE *
EncodeSearchKey(class_var, type, tagno, searchKey)
int class_var;
int type;
int tagno;
SearchKey *searchKey;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,0,searchKey->searchKey));
    if (searchKey->description != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,searchKey->description));
    }
    return node0;
}

ASN_NODE *
EncodeAccessRestrictions(class_var, type, tagno, accessRestrictions)
int class_var;
int type;
int tagno;
AccessRestrictions *accessRestrictions;
{
    struct AccessRestrictions *tmp1 = accessRestrictions;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,0);
        ATTACH(node2, EncodeInteger(UNIVERSAL, INTEGER, tmp1->item.accessType));
        ATTACH(node1, node2);
        }
        if (tmp1->item.accessText != 0) {
        ATTACH(node1, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.accessText));
        }
        if (tmp1->item.accessChallenges != 0) {
        {
        struct accessChallenges_List83 *tmp2 = tmp1->item.accessChallenges;
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        while (tmp2 != 0) {
	    ATTACH(node2, EncodeOid(UNIVERSAL,OBJECTIDENTIFIER,tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node1, node2);
        }
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeCosts(class_var, type, tagno, costs)
int class_var;
int type;
int tagno;
Costs *costs;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (costs->connectCharge != 0) {
    ATTACH(node0, EncodeCharge(BER_CONTEXT,CONSTRUCTED,0,costs->connectCharge));
    }
    if (costs->connectTime != 0) {
    ATTACH(node0, EncodeCharge(BER_CONTEXT,CONSTRUCTED,1,costs->connectTime));
    }
    if (costs->displayCharge != 0) {
    ATTACH(node0, EncodeCharge(BER_CONTEXT,CONSTRUCTED,2,costs->displayCharge));
    }
    if (costs->searchCharge != 0) {
    ATTACH(node0, EncodeCharge(BER_CONTEXT,CONSTRUCTED,3,costs->searchCharge));
    }
    if (costs->subscriptCharge != 0) {
    ATTACH(node0, EncodeCharge(BER_CONTEXT,CONSTRUCTED,4,costs->subscriptCharge));
    }
    if (costs->otherCharges != 0) {
    {
    struct otherCharges_List84 *tmp1 = costs->otherCharges;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        ATTACH(node2, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.forWhat));
        ATTACH(node2, EncodeCharge(BER_CONTEXT,CONSTRUCTED,2,tmp1->item.charge));
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeCharge(class_var, type, tagno, charge)
int class_var;
int type;
int tagno;
Charge *charge;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,1,charge->cost));
    if (charge->perWhat != 0) {
    ATTACH(node0, EncodeUnit(BER_CONTEXT,CONSTRUCTED,2,charge->perWhat));
    }
    if (charge->text != 0) {
    ATTACH(node0, EncodeHumanString(BER_CONTEXT,CONSTRUCTED,3,charge->text));
    }
    return node0;
}

ASN_NODE *
EncodeDatabaseList(class_var, type, tagno, databaseList)
int class_var;
int type;
int tagno;
DatabaseList *databaseList;
{
    struct DatabaseList *tmp1 = databaseList;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
    while (tmp1 != 0) {
        ATTACH(node0, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeAttributeCombinations(class_var, type, tagno, attributeCombinations)
int class_var;
int type;
int tagno;
AttributeCombinations *attributeCombinations;
{
  ASN_NODE *node0;
  if (class_var != CLASS_NONE)
    node0 = NewAsnNode(class_var,type,tagno);
  else
    node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);

  ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,0,attributeCombinations->defaultAttributeSet));

  {
    struct legalCombinations_List85 *tmp1 = attributeCombinations->legalCombinations;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
      ATTACH(node1, EncodeAttributeCombination(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
      tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
  }
  return node0;
}

ASN_NODE *
EncodeAttributeCombination(class_var, type, tagno, attributeCombination)
int class_var;
int type;
int tagno;
AttributeCombination *attributeCombination;
{
    struct AttributeCombination *tmp1 = attributeCombination;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
    while (tmp1 != 0) {
        ATTACH(node0, EncodeAttributeOccurrence(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeAttributeOccurrence(class_var, type, tagno, attributeOccurrence)
int class_var;
int type;
int tagno;
AttributeOccurrence *attributeOccurrence;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (attributeOccurrence->attributeSet != 0) {
    ATTACH(node0, EncodeAttributeSetId(BER_CONTEXT,CONSTRUCTED,0,attributeOccurrence->attributeSet));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,attributeOccurrence->attributeType));
    if (attributeOccurrence->mustBeSupplied != 0) {
    ATTACH(node0, EncodeNull(BER_CONTEXT,2));
    }
    switch (attributeOccurrence->attributeValues.which) {
        case e44_any_or_none: {
            ATTACH(node0, EncodeNull(BER_CONTEXT,3));
            break;
        }
        case e44_specific: {
            {
            struct specific_List86 *tmp1 = attributeOccurrence->attributeValues.u.specific;
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
            while (tmp1 != 0) {
                ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                tmp1 = tmp1->next;
            }
            ATTACH(node0, node1);
            }
            break;
        }
    }
    return node0;
}


ASN_NODE *
EncodeOPACRecord(class_var, type, tagno, oPACRecord)
int class_var;
int type;
int tagno;
OPACRecord *oPACRecord;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (oPACRecord->bibliographicRecord != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,1,oPACRecord->bibliographicRecord));
    }
    if (oPACRecord->holdingsData != 0) {
    {
    struct holdingsData_List87 *tmp1 = oPACRecord->holdingsData;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeHoldingsRecord(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeHoldingsRecord(class_var, type, tagno, holdingsRecord)
int class_var;
int type;
int tagno;
HoldingsRecord *holdingsRecord;
{
    ASN_NODE *node0;
    switch(holdingsRecord->which) {
        case e45_marcHoldingsRecord: {
            node0  = EncodeExternal(BER_CONTEXT,1,holdingsRecord->u.marcHoldingsRecord);
            break;
        }
        case e45_holdingsAndCirc: {
            node0 = EncodeHoldingsAndCircData(BER_CONTEXT,CONSTRUCTED,2,holdingsRecord->u.holdingsAndCirc);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeHoldingsAndCircData(class_var, type, tagno, holdingsAndCircData)
int class_var;
int type;
int tagno;
HoldingsAndCircData *holdingsAndCircData;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (holdingsAndCircData->typeOfRecord != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,holdingsAndCircData->typeOfRecord));
    }
    if (holdingsAndCircData->encodingLevel != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,holdingsAndCircData->encodingLevel));
    }
    if (holdingsAndCircData->format != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,holdingsAndCircData->format));
    }
    if (holdingsAndCircData->receiptAcqStatus != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,holdingsAndCircData->receiptAcqStatus));
    }
    if (holdingsAndCircData->generalRetention != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,5,holdingsAndCircData->generalRetention));
    }
    if (holdingsAndCircData->completeness != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,6,holdingsAndCircData->completeness));
    }
    if (holdingsAndCircData->dateOfReport != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,7,holdingsAndCircData->dateOfReport));
    }
    if (holdingsAndCircData->nucCode != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,8,holdingsAndCircData->nucCode));
    }
    if (holdingsAndCircData->localLocation != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,9,holdingsAndCircData->localLocation));
    }
    if (holdingsAndCircData->shelvingLocation != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,10,holdingsAndCircData->shelvingLocation));
    }
    if (holdingsAndCircData->callNumber != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,11,holdingsAndCircData->callNumber));
    }
    if (holdingsAndCircData->shelvingData != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,12,holdingsAndCircData->shelvingData));
    }
    if (holdingsAndCircData->copyNumber != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,13,holdingsAndCircData->copyNumber));
    }
    if (holdingsAndCircData->publicNote != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,14,holdingsAndCircData->publicNote));
    }
    if (holdingsAndCircData->reproductionNote != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,15,holdingsAndCircData->reproductionNote));
    }
    if (holdingsAndCircData->termsUseRepro != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,16,holdingsAndCircData->termsUseRepro));
    }
    if (holdingsAndCircData->enumAndChron != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,17,holdingsAndCircData->enumAndChron));
    }
    if (holdingsAndCircData->volumes != 0) {
    {
    struct volumes_List88 *tmp1 = holdingsAndCircData->volumes;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,18);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeVolume(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (holdingsAndCircData->circulationData != 0) {
    {
    struct circulationData_List89 *tmp1 = holdingsAndCircData->circulationData;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,19);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeCircRecord(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeVolume(class_var, type, tagno, volume)
int class_var;
int type;
int tagno;
Volume *volume;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (volume->enumeration != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,volume->enumeration));
    }
    if (volume->chronology != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,volume->chronology));
    }
    if (volume->enumAndChron != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,volume->enumAndChron));
    }
    return node0;
}

ASN_NODE *
EncodeCircRecord(class_var, type, tagno, circRecord)
int class_var;
int type;
int tagno;
CircRecord *circRecord;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,circRecord->availableNow));
    if (circRecord->availablityDate != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,circRecord->availablityDate));
    }
    if (circRecord->availableThru != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,circRecord->availableThru));
    }
    if (circRecord->restrictions != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,circRecord->restrictions));
    }
    if (circRecord->itemId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,5,circRecord->itemId));
    }
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,6,circRecord->renewable));
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,7,circRecord->onHold));
    if (circRecord->enumAndChron != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,8,circRecord->enumAndChron));
    }
    if (circRecord->midspine != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,9,circRecord->midspine));
    }
    if (circRecord->temporaryLocation != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,10,circRecord->temporaryLocation));
    }
    return node0;
}

ASN_NODE *
EncodeBriefBib(class_var, type, tagno, briefBib)
int class_var;
int type;
int tagno;
BriefBib *briefBib;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,briefBib->title));
    if (briefBib->author != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,briefBib->author));
    }
    if (briefBib->callNumber != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,briefBib->callNumber));
    }
    if (briefBib->recordType != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,briefBib->recordType));
    }
    if (briefBib->bibliographicLevel != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,5,briefBib->bibliographicLevel));
    }
    if (briefBib->format != 0) {
    {
    struct format_List90 *tmp1 = briefBib->format;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeFormatSpec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (briefBib->publicationPlace != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,7,briefBib->publicationPlace));
    }
    if (briefBib->publicationDate != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,8,briefBib->publicationDate));
    }
    if (briefBib->targetSystemKey != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,9,briefBib->targetSystemKey));
    }
    if (briefBib->satisfyingElement != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,10,briefBib->satisfyingElement));
    }
    if (briefBib->rank != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,11,briefBib->rank));
    }
    if (briefBib->documentId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,12,briefBib->documentId));
    }
    if (briefBib->abstract != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,13,briefBib->abstract));
    }
    if (briefBib->otherInfo != 0) {
    ATTACH(node0, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,briefBib->otherInfo));
    }
    return node0;
}

ASN_NODE *
EncodeFormatSpec(class_var, type, tagno, formatSpec)
int class_var;
int type;
int tagno;
FormatSpec *formatSpec;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,formatSpec->type));
    if (formatSpec->size != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,formatSpec->size));
    }
    if (formatSpec->bestPosn != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,formatSpec->bestPosn));
    }
    return node0;
}

ASN_NODE *
EncodeGenericRecord(class_var, type, tagno, genericRecord)
int class_var;
int type;
int tagno;
GenericRecord *genericRecord;
{
    struct GenericRecord *tmp1 = genericRecord;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);

    while (tmp1 != 0) {
        ATTACH(node0, EncodeTaggedElement(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }

    return node0;
}

ASN_NODE *
EncodeTaggedElement(class_var, type, tagno, taggedElement)
int class_var;
int type;
int tagno;
TaggedElement *taggedElement;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (taggedElement->tagType != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,taggedElement->tagType));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,taggedElement->tagValue));
    ATTACH(node0, node1);
    }
    if (taggedElement->tagOccurrence != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,taggedElement->tagOccurrence));
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    ATTACH(node1, EncodeElementData(CLASS_NONE,TYPE_NONE,TAGNO_NONE,taggedElement->content));
    ATTACH(node0, node1);
    }
    if (taggedElement->metaData != 0) {
    ATTACH(node0, EncodeElementMetaData(BER_CONTEXT,CONSTRUCTED,5,taggedElement->metaData));
    }
    if (taggedElement->appliedVariant != 0) {
    ATTACH(node0, EncodeVariant(BER_CONTEXT,CONSTRUCTED,6,taggedElement->appliedVariant));
    }
    return node0;
}

ASN_NODE *
EncodeElementData(class_var, type, tagno, elementData)
int class_var;
int type;
int tagno;
ElementData *elementData;
{
    ASN_NODE *node0;
    switch(elementData->which) {
        case e46_octets: {
            node0 = EncodeOctetString(UNIVERSAL, OCTETSTRING, elementData->u.octets);
            break;
        }
        case e46_numeric: {
            node0 = EncodeInteger(UNIVERSAL, INTEGER, elementData->u.numeric);
            break;
        }
        case e46_date: {
            node0 = EncodeGeneralizedTime(CLASS_NONE,TAGNO_NONE,elementData->u.date);
            break;
        }
        case e46_ext: {
            node0 = EncodeExternal(UNIVERSAL, VISIBLESTRING, elementData->u.ext);
            break;
        }
        case e46_string: {
	  if (elementData->u.string->length != 0)
            node0 = EncodeInternationalString(UNIVERSAL,GENERALSTRING,elementData->u.string);
	  else
            node0 = EncodeNull(BER_CONTEXT,3);

	  break;
        }
        case e46_trueOrFalse: {
            node0 = EncodeBoolean(UNIVERSAL, VISIBLESTRING, elementData->u.trueOrFalse);
            break;
        }
        case e46_oid: {
            node0 = EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, elementData->u.oid);
            break;
        }
        case e46_intUnit: {
            node0 = EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,1,elementData->u.intUnit);
            break;
        }
        case e46_elementNotThere: {
            node0 = EncodeNull(BER_CONTEXT,2);
            break;
        }
        case e46_elementEmpty: {
            node0 = EncodeNull(BER_CONTEXT,3);
            break;
        }
        case e46_noDataRequested: {
            node0 = EncodeNull(BER_CONTEXT,4);
            break;
        }
        case e46_diagnostic: {
            node0  = EncodeExternal(BER_CONTEXT,5,elementData->u.diagnostic);
            break;
        }
        case e46_subtree: {
	  ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
	  struct subtree_List91 *tmp1 = elementData->u.subtree;
	  ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
	  
	  while (tmp1 != 0) {
	    ATTACH(node2, EncodeTaggedElement(CLASS_NONE,
					      TYPE_NONE,
					      TAGNO_NONE,tmp1->item));
	    tmp1 = tmp1->next;
	  }
	  ATTACH(node1, node2);
	  node0 = node1;
	  
	  break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeElementMetaData(class_var, type, tagno, elementMetaData)
int class_var;
int type;
int tagno;
ElementMetaData *elementMetaData;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (elementMetaData->seriesOrder != 0) {
    ATTACH(node0, EncodeOrder(BER_CONTEXT,CONSTRUCTED,1,elementMetaData->seriesOrder));
    }
    if (elementMetaData->usageRight != 0) {
    ATTACH(node0, EncodeUsage(BER_CONTEXT,CONSTRUCTED,2,elementMetaData->usageRight));
    }
    if (elementMetaData->hits != 0) {
    {
    struct hits_List92 *tmp1 = elementMetaData->hits;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeHitVector(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (elementMetaData->displayName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,elementMetaData->displayName));
    }
    if (elementMetaData->supportedVariants != 0) {
    {
    struct supportedVariants_List93 *tmp1 = elementMetaData->supportedVariants;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeVariant(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (elementMetaData->message != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,6,elementMetaData->message));
    }
    if (elementMetaData->elementDescriptor != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,7,elementMetaData->elementDescriptor));
    }
    if (elementMetaData->surrogateFor != 0) {
    ATTACH(node0, EncodeTagPath(BER_CONTEXT,CONSTRUCTED,8,elementMetaData->surrogateFor));
    }
    if (elementMetaData->surrogateElement != 0) {
    ATTACH(node0, EncodeTagPath(BER_CONTEXT,CONSTRUCTED,9,elementMetaData->surrogateElement));
    }
    if (elementMetaData->other != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,99,elementMetaData->other));
    }
    return node0;
}

ASN_NODE *
EncodeTagPath(class_var, type, tagno, tagPath)
int class_var;
int type;
int tagno;
TagPath *tagPath;
{
    struct TagPath *tmp1 = tagPath;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.tagType != 0) {
        ATTACH(node1, EncodeInteger(BER_CONTEXT,1,tmp1->item.tagType));
        }
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        ATTACH(node2, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.tagValue));
        ATTACH(node1, node2);
        }
        if (tmp1->item.tagOccurrence != 0) {
        ATTACH(node1, EncodeInteger(BER_CONTEXT,3,tmp1->item.tagOccurrence));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}


ASN_NODE *
EncodeOrder(class_var, type, tagno, order)
int class_var;
int type;
int tagno;
Order *order;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,order->ascending));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,order->order));
    return node0;
}

ASN_NODE *
EncodeUsage(class_var, type, tagno, usage)
int class_var;
int type;
int tagno;
Usage *usage;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,usage->type));
    if (usage->restriction != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,usage->restriction));
    }
    return node0;
}

ASN_NODE *
EncodeHitVector(class_var, type, tagno, hitVector)
int class_var;
int type;
int tagno;
HitVector *hitVector;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (hitVector->satisfier != 0) {
    ATTACH(node0, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,hitVector->satisfier));
    }
    if (hitVector->offsetIntoElement != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,1,hitVector->offsetIntoElement));
    }
    if (hitVector->length != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,2,hitVector->length));
    }
    if (hitVector->hitRank != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,hitVector->hitRank));
    }
    if (hitVector->targetToken != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,4,hitVector->targetToken));
    }
    return node0;
}

ASN_NODE *
EncodeVariant(class_var, type, tagno, variant)
int class_var;
int type;
int tagno;
Variant *variant;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (variant->globalVariantSetId != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,variant->globalVariantSetId));
    }
    {
    struct triples_List94 *tmp1 = variant->triples;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ASN_NODE *node2 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.variantSetId != 0) {
        ATTACH(node2, EncodeOid(BER_CONTEXT,0,tmp1->item.variantSetId));
        }
        ATTACH(node2, EncodeInteger(BER_CONTEXT,1,tmp1->item.class_var));
        ATTACH(node2, EncodeInteger(BER_CONTEXT,2,tmp1->item.type));
        {
        ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
        switch (tmp1->item.value.which) {
            case e47_intVal: {
                ATTACH(node3, EncodeInteger(UNIVERSAL, INTEGER, tmp1->item.value.u.intVal));
                break;
            }
            case e47_interStringVal: {
                ATTACH(node3, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item.value.u.interStringVal));
                break;
            }
            case e47_octStringVal: {
                ATTACH(node3, EncodeOctetString(UNIVERSAL, OCTETSTRING, tmp1->item.value.u.octStringVal));
                break;
            }
            case e47_oidVal: {
                ATTACH(node3, EncodeOid(UNIVERSAL, OBJECTIDENTIFIER, tmp1->item.value.u.oidVal));
                break;
            }
            case e47_boolVal: {
                ATTACH(node3, EncodeBoolean(UNIVERSAL, VISIBLESTRING, tmp1->item.value.u.boolVal));
                break;
            }
            case e47_nullVal: {
                ATTACH(node3, EncodeNull(UNIVERSAL, VISIBLESTRING));
                break;
            }
            case e47_unit: {
                ATTACH(node3, EncodeUnit(BER_CONTEXT,CONSTRUCTED,1,tmp1->item.value.u.unit));
                break;
            }
            case e47_valueAndUnit: {
                ATTACH(node3, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,2,tmp1->item.value.u.valueAndUnit));
                break;
            }
        }
        ATTACH(node2, node3);
        }
        ATTACH(node1, node2);
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeTaskPackage(class_var, type, tagno, taskPackage)
int class_var;
int type;
int tagno;
TaskPackage *taskPackage;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeOid(BER_CONTEXT,1,taskPackage->packageType));
    if (taskPackage->packageName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,taskPackage->packageName));
    }
    if (taskPackage->userId != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,taskPackage->userId));
    }
    if (taskPackage->retentionTime != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,4,taskPackage->retentionTime));
    }
    if (taskPackage->permissions != 0) {
    ATTACH(node0, EncodePermissions(BER_CONTEXT,CONSTRUCTED,5,taskPackage->permissions));
    }
    if (taskPackage->description != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,6,taskPackage->description));
    }
    if (taskPackage->targetReference != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,7,taskPackage->targetReference));
    }
    if (taskPackage->creationDateTime != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,8,taskPackage->creationDateTime));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,9,taskPackage->taskStatus));
    if (taskPackage->packageDiagnostics != 0) {
    {
    struct packageDiagnostics_List95 *tmp1 = taskPackage->packageDiagnostics;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,10);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    ATTACH(node0, EncodeExternal(BER_CONTEXT,11,taskPackage->taskSpecificParameters));
    return node0;
}
/* -----
ASN_NODE *
EncodeResourceReport(class_var, type, tagno, resourceReport)
int class_var;
int type;
int tagno;
ResourceReport *resourceReport;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    struct estimates_List96 *tmp1 = resourceReport->estimates;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeEstimate(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,resourceReport->message));
    return node0;
}

ASN_NODE *
EncodeEstimate(class_var, type, tagno, estimate)
int class_var;
int type;
int tagno;
Estimate *estimate;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeEstimateType(BER_CONTEXT,CONSTRUCTED,1,estimate->type));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,estimate->value));
    if (estimate->currency_code != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,estimate->currency_code));
    }
    return node0;
}
 

ASN_NODE *
EncodeEstimateType(class_var, type, tagno, estimateType)
int class_var;
int type;
int tagno;
EstimateType estimateType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = EncodeInteger(class_var,tagno,estimateType);
    else
        node0 = EncodeInteger(UNIVERSAL,INTEGER,estimateType);
    return node0;
}

ASN_NODE *
EncodeResourceReport(class_var, type, tagno, resourceReport)
int class_var;
int type;
int tagno;
ResourceReport *resourceReport;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (resourceReport->estimates != 0) {
    {
    struct estimates_List97 *tmp1 = resourceReport->estimates;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeEstimate(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (resourceReport->message != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,resourceReport->message));
    }
    return node0;
}


ASN_NODE *
EncodeEstimate(class_var, type, tagno, estimate)
int class_var;
int type;
int tagno;
Estimate *estimate;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,estimate->type));
    ATTACH(node0, node1);
    }
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,2,estimate->value));
    return node0;
}

ASN_NODE *
EncodePromptObject(class_var, type, tagno, promptObject)
int class_var;
int type;
int tagno;
PromptObject *promptObject;
{
    ASN_NODE *node0;
    switch(promptObject->which) {
        case e48_challenge: {
            node0 = EncodeChallenge(BER_CONTEXT,CONSTRUCTED,1,promptObject->u.challenge);
            break;
        }
        case e48_response: {
            node0 = EncodeResponse(BER_CONTEXT,CONSTRUCTED,2,promptObject->u.response);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeChallenge(class_var, type, tagno, challenge)
int class_var;
int type;
int tagno;
Challenge *challenge;
{
    struct Challenge *tmp1 = challenge;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCEOF);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        ATTACH(node2, EncodePromptId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.promptId));
        ATTACH(node1, node2);
        }
        if (tmp1->item.defaultResponse != 0) {
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.defaultResponse));
        }
        if (tmp1->item.promptInfo != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
        switch (tmp1->item.promptInfo->which) {
            case e49_character: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.promptInfo->u.character));
                break;
            }
            case e49_encrypted: {
                ATTACH(node2, EncodeEncryption(BER_CONTEXT,CONSTRUCTED,2,tmp1->item.promptInfo->u.encrypted));
                break;
            }
        }
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.regExpr != 0) {
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,4,tmp1->item.regExpr));
        }
        if (tmp1->item.responseRequired != 0) {
        ATTACH(node1, EncodeNull(BER_CONTEXT,5));
        }
        if (tmp1->item.allowedValues != 0) {
        {
        struct allowedValues_List98 *tmp2 = tmp1->item.allowedValues;
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,6);
        while (tmp2 != 0) {
            ATTACH(node2, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp2->item));
            tmp2 = tmp2->next;
        }
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.shouldSave != 0) {
        ATTACH(node1, EncodeNull(BER_CONTEXT,7));
        }
        if (tmp1->item.dataType != 0) {
        ATTACH(node1, EncodeInteger(BER_CONTEXT,8,tmp1->item.dataType));
        }
        if (tmp1->item.diagnostic != 0) {
        ATTACH(node1, EncodeExternal(BER_CONTEXT,9,tmp1->item.diagnostic));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeResponse(class_var, type, tagno, response)
int class_var;
int type;
int tagno;
Response *response;
{
    struct Response *tmp1 = response;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        ATTACH(node2, EncodePromptId(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.promptId));
        ATTACH(node1, node2);
        }
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        switch (tmp1->item.promptResponse.which) {
            case e50_string: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.promptResponse.u.string));
                break;
            }
            case e50_accept: {
                ATTACH(node2, EncodeBoolean(BER_CONTEXT,2,tmp1->item.promptResponse.u.accept));
                break;
            }
            case e50_acknowledge: {
                ATTACH(node2, EncodeNull(BER_CONTEXT,3));
                break;
            }
            case e50_diagnostic: {
                {
                ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
                ATTACH(node3, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.promptResponse.u.diagnostic));
                ATTACH(node2, node3);
                }
                break;
            }
            case e50_encrypted: {
                ATTACH(node2, EncodeEncryption(BER_CONTEXT,CONSTRUCTED,5,tmp1->item.promptResponse.u.encrypted));
                break;
            }
        }
        ATTACH(node1, node2);
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodePromptId(class_var, type, tagno, promptId)
int class_var;
int type;
int tagno;
PromptId *promptId;
{
    ASN_NODE *node0;
    switch(promptId->which) {
        case e51_enummeratedPrompt: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,promptId->u.enummeratedPrompt.type));
            if (promptId->u.enummeratedPrompt.suggestedString != 0) {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,promptId->u.enummeratedPrompt.suggestedString));
            }
            node0 = node1;
            break;
        }
        case e51_nonEnumeratedPrompt: {
            node0 = EncodeInternationalString(BER_CONTEXT,2,promptId->u.nonEnumeratedPrompt);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeEncryption(class_var, type, tagno, encryption)
int class_var;
int type;
int tagno;
Encryption *encryption;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (encryption->cryptType != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,1,encryption->cryptType));
    }
    if (encryption->credential != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,2,encryption->credential));
    }
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,3,encryption->data));
    return node0;
}

ASN_NODE *
EncodeDES_RN_Object(class_var, type, tagno, dES_RN_Object)
int class_var;
int type;
int tagno;
DES_RN_Object *dES_RN_Object;
{
    ASN_NODE *node0;
    switch(dES_RN_Object->which) {
        case e52_challenge: {
            node0 = EncodeDRNType(BER_CONTEXT,CONSTRUCTED,1,dES_RN_Object->u.challenge);
            break;
        }
        case e52_response: {
            node0 = EncodeDRNType(BER_CONTEXT,CONSTRUCTED,2,dES_RN_Object->u.response);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeDRNType(class_var, type, tagno, dRNType)
int class_var;
int type;
int tagno;
DRNType *dRNType;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (dRNType->userId != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,1,dRNType->userId));
    }
    if (dRNType->salt != 0) {
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,2,dRNType->salt));
    }
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,3,dRNType->randomNumber));
    return node0;
}

ASN_NODE *
EncodeKRBObject(class_var, type, tagno, kRBObject)
int class_var;
int type;
int tagno;
KRBObject *kRBObject;
{
    ASN_NODE *node0;
    switch(kRBObject->which) {
        case e53_challenge: {
            node0 = EncodeKRBRequest(BER_CONTEXT,CONSTRUCTED,1,kRBObject->u.challenge);
            break;
        }
        case e53_response: {
            node0 = EncodeKRBResponse(BER_CONTEXT,CONSTRUCTED,2,kRBObject->u.response);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeKRBRequest(class_var, type, tagno, kRBRequest)
int class_var;
int type;
int tagno;
KRBRequest *kRBRequest;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,kRBRequest->service));
    if (kRBRequest->instance != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,kRBRequest->instance));
    }
    if (kRBRequest->realm != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,kRBRequest->realm));
    }
    return node0;
}

ASN_NODE *
EncodeKRBResponse(class_var, type, tagno, kRBResponse)
int class_var;
int type;
int tagno;
KRBResponse *kRBResponse;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (kRBResponse->userid != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,kRBResponse->userid));
    }
    ATTACH(node0, EncodeOctetString(BER_CONTEXT,2,kRBResponse->ticket));
    return node0;
}

ASN_NODE *
EncodePersistentResultSet(class_var, type, tagno, persistentResultSet)
int class_var;
int type;
int tagno;
PersistentResultSet *persistentResultSet;
{
    ASN_NODE *node0;
    switch(persistentResultSet->which) {
        case e54_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node1, EncodeNull(BER_CONTEXT,1));
            if (persistentResultSet->u.esRequest.notToKeep != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentResultSet->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
        case e54_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeNull(BER_CONTEXT,1));
            if (persistentResultSet->u.taskPackage.targetPart != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentResultSet->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartNotToKeep(class_var, type, tagno, originPartNotToKeep)
int class_var;
int type;
int tagno;
OriginPartNotToKeep *originPartNotToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (originPartNotToKeep->originSuppliedResultSet != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,originPartNotToKeep->originSuppliedResultSet));
    }
    if (originPartNotToKeep->replaceOrAppend != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,originPartNotToKeep->replaceOrAppend));
    }
    return node0;
}

ASN_NODE *
EncodeTargetPart(class_var, type, tagno, targetPart)
int class_var;
int type;
int tagno;
TargetPart *targetPart;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (targetPart->targetSuppliedResultSet != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,targetPart->targetSuppliedResultSet));
    }
    if (targetPart->numberOfRecords != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,targetPart->numberOfRecords));
    }
    return node0;
}


ASN_NODE *
EncodePersistentQuery(class_var, type, tagno, persistentQuery)
int class_var;
int type;
int tagno;
PersistentQuery *persistentQuery;
{
    ASN_NODE *node0;
    switch(persistentQuery->which) {
        case e54_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            if (persistentQuery->u.esRequest.toKeep != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentQuery->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentQuery->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e54_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            if (persistentQuery->u.taskPackage.originPart != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentQuery->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,persistentQuery->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (originPartToKeep->dbNames != 0) {
    {
    struct dbNames_List99 *tmp1 = originPartToKeep->dbNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (originPartToKeep->additionalSearchInfo != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    ATTACH(node1, EncodeOtherInformation(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartToKeep->additionalSearchInfo));
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartNotToKeep(class_var, type, tagno, originPartNotToKeep)
int class_var;
int type;
int tagno;
OriginPartNotToKeep *originPartNotToKeep;
{
    ASN_NODE *node0;
    switch(originPartNotToKeep->which) {
        case e56_package: {
            node0 = EncodeInternationalString(BER_CONTEXT,1,originPartNotToKeep->u.package);
            break;
        }
        case e56_query: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeQuery(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartNotToKeep->u.query));
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodePeriodicQuerySchedule(class_var, type, tagno, periodicQuerySchedule)
int class_var;
int type;
int tagno;
PeriodicQuerySchedule *periodicQuerySchedule;
{
    ASN_NODE *node0;
    switch(periodicQuerySchedule->which) {
        case e57_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,periodicQuerySchedule->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,periodicQuerySchedule->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e57_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,periodicQuerySchedule->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,periodicQuerySchedule->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeBoolean(BER_CONTEXT,1,originPartToKeep->activeFlag));
    if (originPartToKeep->databaseNames != 0) {
    {
    struct databaseNames_List100 *tmp1 = originPartToKeep->databaseNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (originPartToKeep->resultSetDisposition != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,originPartToKeep->resultSetDisposition));
    }
    if (originPartToKeep->alertDestination != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
    ATTACH(node1, EncodeDestination(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartToKeep->alertDestination));
    ATTACH(node0, node1);
    }
    }
    if (originPartToKeep->exportParameters != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    switch (originPartToKeep->exportParameters->which) {
        case e57_packageName: {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,originPartToKeep->exportParameters->u.packageName));
            break;
        }
        case e57_exportPackage: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeExportSpecification(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartToKeep->exportParameters->u.exportPackage));
            ATTACH(node1, node2);
            }
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartNotToKeep(class_var, type, tagno, originPartNotToKeep)
int class_var;
int type;
int tagno;
OriginPartNotToKeep *originPartNotToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (originPartNotToKeep->querySpec != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    switch (originPartNotToKeep->querySpec->which) {
        case e58_actualQuery: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeQuery(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartNotToKeep->querySpec->u.actualQuery));
            ATTACH(node1, node2);
            }
            break;
        }
        case e58_packageName: {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,originPartNotToKeep->querySpec->u.packageName));
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    if (originPartNotToKeep->originSuggestedPeriod != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodePeriod(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartNotToKeep->originSuggestedPeriod));
    ATTACH(node0, node1);
    }
    }
    if (originPartNotToKeep->expiration != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,CONSTRUCTED,3,originPartNotToKeep->expiration));
    }
    if (originPartNotToKeep->resultSetPackage != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,originPartNotToKeep->resultSetPackage));
    }
    return node0;
}

ASN_NODE *
EncodeTargetPart(class_var, type, tagno, targetPart)
int class_var;
int type;
int tagno;
TargetPart *targetPart;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeQuery(CLASS_NONE,TYPE_NONE,TAGNO_NONE,targetPart->actualQuery));
    ATTACH(node0, node1);
    }
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodePeriod(CLASS_NONE,TYPE_NONE,TAGNO_NONE,targetPart->targetStatedPeriod));
    ATTACH(node0, node1);
    }
    if (targetPart->expiration != 0) {
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,CONSTRUCTED,3,targetPart->expiration));
    }
    if (targetPart->resultSetPackage != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,targetPart->resultSetPackage));
    }
    ATTACH(node0, EncodeGeneralizedTime(BER_CONTEXT,CONSTRUCTED,5,targetPart->lastQueryTime));
    ATTACH(node0, EncodeInteger(BER_CONTEXT,6,targetPart->lastResultNumber));
    if (targetPart->numberSinceModify != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,7,targetPart->numberSinceModify));
    }
    return node0;
}


ASN_NODE *
EncodePeriod(class_var, type, tagno, period)
int class_var;
int type;
int tagno;
Period *period;
{
    ASN_NODE *node0;
    switch(period->which) {
        case e60_unit: {
            node0 = EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,1,period->u.unit);
            break;
        }
        case e60_businessDaily: {
            node0 = EncodeNull(BER_CONTEXT,2);
            break;
        }
        case e60_continuous: {
            node0 = EncodeNull(BER_CONTEXT,3);
            break;
        }
        case e60_other: {
            node0 = EncodeInternationalString(BER_CONTEXT,4,period->u.other);
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeItemOrder(class_var, type, tagno, itemOrder)
int class_var;
int type;
int tagno;
ItemOrder *itemOrder;
{
    ASN_NODE *node0;
    switch(itemOrder->which) {
        case e61_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            if (itemOrder->u.esRequest.toKeep != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,itemOrder->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,itemOrder->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e61_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            if (itemOrder->u.taskPackage.originPart != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,itemOrder->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,itemOrder->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (originPartToKeep->supplDescription != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,1,originPartToKeep->supplDescription));
    }
    if (originPartToKeep->contact != 0) {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    if (originPartToKeep->contact.name != 0) {
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,originPartToKeep->contact.name));
    }
    if (originPartToKeep->contact.phone != 0) {
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,originPartToKeep->contact.phone));
    }
    if (originPartToKeep->contact.email != 0) {
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,3,originPartToKeep->contact.email));
    }
    ATTACH(node0, node1);
    }
    if (originPartToKeep->addlBilling != 0) {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    {
    ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    switch (originPartToKeep->addlBilling.paymentMethod.which) {
        case e62_billInvoice: {
            ATTACH(node2, EncodeNull(BER_CONTEXT,0));
            break;
        }
        case e62_prepay: {
            ATTACH(node2, EncodeNull(BER_CONTEXT,1));
            break;
        }
        case e62_depositAccount: {
            ATTACH(node2, EncodeNull(BER_CONTEXT,2));
            break;
        }
        case e62_creditCard: {
            ATTACH(node2, EncodeCreditCardInfo(BER_CONTEXT,CONSTRUCTED,3,originPartToKeep->addlBilling.paymentMethod.u.creditCard));
            break;
        }
        case e62_cardInfoPreviouslySupplied: {
            ATTACH(node2, EncodeNull(BER_CONTEXT,4));
            break;
        }
        case e62_privateKnown: {
            ATTACH(node2, EncodeNull(BER_CONTEXT,5));
            break;
        }
        case e62_privateNotKnown: {
            ATTACH(node2, EncodeExternal(BER_CONTEXT,6,originPartToKeep->addlBilling.paymentMethod.u.privateNotKnown));
            break;
        }
    }
    ATTACH(node1, node2);
    }
    if (originPartToKeep->addlBilling.customerReference != 0) {
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,originPartToKeep->addlBilling.customerReference));
    }
    if (originPartToKeep->addlBilling.customerPONumber != 0) {
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,3,originPartToKeep->addlBilling.customerPONumber));
    }
    ATTACH(node0, node1);
    }
    return node0;
}


ASN_NODE *
EncodeCreditCardInfo(class_var, type, tagno, creditCardInfo)
int class_var;
int type;
int tagno;
CreditCardInfo *creditCardInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,creditCardInfo->nameOnCard));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,creditCardInfo->expirationDate));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,3,creditCardInfo->cardNumber));
    return node0;
}

ASN_NODE *
EncodeOriginPartNotToKeep(class_var, type, tagno, originPartNotToKeep)
int class_var;
int type;
int tagno;
OriginPartNotToKeep *originPartNotToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (originPartNotToKeep->resultSetItem != 0) {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,originPartNotToKeep->resultSetItem.resultSetId));
    ATTACH(node1, EncodeInteger(BER_CONTEXT,2,originPartNotToKeep->resultSetItem.item));
    ATTACH(node0, node1);
    }
    if (originPartNotToKeep->itemRequest != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,2,originPartNotToKeep->itemRequest));
    }
    return node0;
}

ASN_NODE *
EncodeTargetPart(class_var, type, tagno, targetPart)
int class_var;
int type;
int tagno;
TargetPart *targetPart;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (targetPart->itemRequest != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,1,targetPart->itemRequest));
    }
    if (targetPart->statusOrErrorReport != 0) {
    ATTACH(node0, EncodeExternal(BER_CONTEXT,2,targetPart->statusOrErrorReport));
    }
    if (targetPart->auxiliaryStatus != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,targetPart->auxiliaryStatus));
    }
    return node0;
}


ASN_NODE *
EncodeUpdate(class_var, type, tagno, update)
int class_var;
int type;
int tagno;
Update *update;
{
    ASN_NODE *node0;
    switch(update->which) {
        case e63_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,update->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,update->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e63_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,update->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,update->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}


ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,originPartToKeep->action));
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,2,originPartToKeep->databaseName));
    if (originPartToKeep->schema != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,3,originPartToKeep->schema));
    }
    if (originPartToKeep->elementSetName != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,4,originPartToKeep->elementSetName));
    }
    return node0;
}


ASN_NODE *
EncodeTargetPart(class_var, type, tagno, targetPart)
int class_var;
int type;
int tagno;
TargetPart *targetPart;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInteger(BER_CONTEXT,1,targetPart->updateStatus));
    if (targetPart->globalDiagnostics != 0) {
    {
    struct globalDiagnostics_List101 *tmp1 = targetPart->globalDiagnostics;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    {
    struct taskPackageRecords_List102 *tmp1 = targetPart->taskPackageRecords;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeTaskPackageRecordStructure(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    return node0;
}


ASN_NODE *
EncodeSuppliedRecords(class_var, type, tagno, suppliedRecords)
int class_var;
int type;
int tagno;
SuppliedRecords *suppliedRecords;
{
    struct SuppliedRecords *tmp1 = suppliedRecords;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.recordId != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        switch (tmp1->item.recordId->which) {
            case e64_number: {
                ATTACH(node2, EncodeInteger(BER_CONTEXT,1,tmp1->item.recordId->u.number));
                break;
            }
            case e64_string: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.recordId->u.string));
                break;
            }
            case e64_opaque: {
                ATTACH(node2, EncodeOctetString(BER_CONTEXT,3,tmp1->item.recordId->u.opaque));
                break;
            }
        }
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.supplementalId != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
        switch (tmp1->item.supplementalId->which) {
            case e65_timeStamp: {
                ATTACH(node2, EncodeGeneralizedTime(BER_CONTEXT,1,tmp1->item.supplementalId->u.timeStamp));
                break;
            }
            case e65_versionNumber: {
                ATTACH(node2, EncodeInternationalString(BER_CONTEXT,2,tmp1->item.supplementalId->u.versionNumber));
                break;
            }
            case e65_previousVersion: {
                ATTACH(node2, EncodeExternal(BER_CONTEXT,3,tmp1->item.supplementalId->u.previousVersion));
                break;
            }
        }
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.correlationInfo != 0) {
        ATTACH(node1, EncodeCorrelationInfo(BER_CONTEXT,CONSTRUCTED,3,tmp1->item.correlationInfo));
        }
        ATTACH(node1, EncodeExternal(BER_CONTEXT,4,tmp1->item.record));
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeCorrelationInfo(class_var, type, tagno, correlationInfo)
int class_var;
int type;
int tagno;
CorrelationInfo *correlationInfo;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (correlationInfo->note != 0) {
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,correlationInfo->note));
    }
    if (correlationInfo->id != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,correlationInfo->id));
    }
    return node0;
}

ASN_NODE *
EncodeTaskPackageRecordStructure(class_var, type, tagno, taskPackageRecordStructure)
int class_var;
int type;
int tagno;
TaskPackageRecordStructure *taskPackageRecordStructure;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (taskPackageRecordStructure->recordOrSurDiag != 0) {
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    switch (taskPackageRecordStructure->recordOrSurDiag->which) {
        case e66_record: {
            ATTACH(node1, EncodeExternal(BER_CONTEXT,1,taskPackageRecordStructure->recordOrSurDiag->u.record));
            break;
        }
        case e66_diagnostic: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeDiagRec(CLASS_NONE,TYPE_NONE,TAGNO_NONE,taskPackageRecordStructure->recordOrSurDiag->u.diagnostic));
            ATTACH(node1, node2);
            }
            break;
        }
    }
    ATTACH(node0, node1);
    }
    }
    if (taskPackageRecordStructure->correlationInfo != 0) {
    ATTACH(node0, EncodeCorrelationInfo(BER_CONTEXT,CONSTRUCTED,2,taskPackageRecordStructure->correlationInfo));
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,3,taskPackageRecordStructure->recordStatus));
    return node0;
}

ASN_NODE *
EncodeExportSpecification(class_var, type, tagno, exportSpecification)
int class_var;
int type;
int tagno;
ExportSpecification *exportSpecification;
{
    ASN_NODE *node0;
    switch(exportSpecification->which) {
        case e67_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportSpecification->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            ATTACH(node1, EncodeNull(BER_CONTEXT,2));
            node0 = node1;
            break;
        }
        case e67_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportSpecification->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            ATTACH(node1, EncodeNull(BER_CONTEXT,2));
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeCompSpec(BER_CONTEXT,CONSTRUCTED,1,originPartToKeep->composition));
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    ATTACH(node1, EncodeDestination(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartToKeep->exportDestination));
    ATTACH(node0, node1);
    }
    return node0;
}


ASN_NODE *
EncodeDestination(class_var, type, tagno, destination)
int class_var;
int type;
int tagno;
Destination *destination;
{
    ASN_NODE *node0;
    switch(destination->which) {
        case e68_phoneNumber: {
            node0 = EncodeInternationalString(BER_CONTEXT,1,destination->u.phoneNumber);
            break;
        }
        case e68_faxNumber: {
            node0 = EncodeInternationalString(BER_CONTEXT,2,destination->u.faxNumber);
            break;
        }
        case e68_x400address: {
            node0 = EncodeInternationalString(BER_CONTEXT,3,destination->u.x400address);
            break;
        }
        case e68_emailAddress: {
            node0 = EncodeInternationalString(BER_CONTEXT,4,destination->u.emailAddress);
            break;
        }
        case e68_pagerNumber: {
            node0 = EncodeInternationalString(BER_CONTEXT,5,destination->u.pagerNumber);
            break;
        }
        case e68_ftpAddress: {
            node0 = EncodeInternationalString(BER_CONTEXT,6,destination->u.ftpAddress);
            break;
        }
        case e68_ftamAddress: {
            node0 = EncodeInternationalString(BER_CONTEXT,7,destination->u.ftamAddress);
            break;
        }
        case e68_printerAddress: {
            node0 = EncodeInternationalString(BER_CONTEXT,8,destination->u.printerAddress);
            break;
        }
        case e68_other: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,100);
            if (destination->u.other.vehicle != 0) {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,destination->u.other.vehicle));
            }
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,destination->u.other.destination));
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeExportInvocation(class_var, type, tagno, exportInvocation)
int class_var;
int type;
int tagno;
ExportInvocation *exportInvocation;
{
    ASN_NODE *node0;
    switch(exportInvocation->which) {
        case e69_esRequest: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportInvocation->u.esRequest.toKeep));
            ATTACH(node1, node2);
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeOriginPartNotToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportInvocation->u.esRequest.notToKeep));
            ATTACH(node1, node2);
            }
            node0 = node1;
            break;
        }
        case e69_taskPackage: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeOriginPartToKeep(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportInvocation->u.taskPackage.originPart));
            ATTACH(node1, node2);
            }
            if (exportInvocation->u.taskPackage.targetPart != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeTargetPart(CLASS_NONE,TYPE_NONE,TAGNO_NONE,exportInvocation->u.taskPackage.targetPart));
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}


ASN_NODE *
EncodeOriginPartToKeep(class_var, type, tagno, originPartToKeep)
int class_var;
int type;
int tagno;
OriginPartToKeep *originPartToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    switch (originPartToKeep->exportSpec.which) {
        case e70_packageName: {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,originPartToKeep->exportSpec.u.packageName));
            break;
        }
        case e70_packageSpec: {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeExportSpecification(CLASS_NONE,TYPE_NONE,TAGNO_NONE,originPartToKeep->exportSpec.u.packageSpec));
            ATTACH(node1, node2);
            }
            break;
        }
    }
    ATTACH(node0, node1);
    }
    ATTACH(node0, EncodeInteger(BER_CONTEXT,2,originPartToKeep->numberOfCopies));
    return node0;
}

ASN_NODE *
EncodeOriginPartNotToKeep(class_var, type, tagno, originPartNotToKeep)
int class_var;
int type;
int tagno;
OriginPartNotToKeep *originPartNotToKeep;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeInternationalString(BER_CONTEXT,1,originPartNotToKeep->resultSetId));
    {
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
    switch (originPartNotToKeep->records.which) {
        case e71_all: {
            ATTACH(node1, EncodeNull(BER_CONTEXT,1));
            break;
        }
        case e71_ranges: {
            {
            struct ranges_List103 *tmp1 = originPartNotToKeep->records.u.ranges;
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            while (tmp1 != 0) {
                ASN_NODE *node3 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
                ATTACH(node3, EncodeInteger(BER_CONTEXT,1,tmp1->item.start));
                if (tmp1->item.count != 0) {
                ATTACH(node3, EncodeInteger(BER_CONTEXT,2,tmp1->item.count));
                }
                ATTACH(node2, node3);
                tmp1 = tmp1->next;
            }
            ATTACH(node1, node2);
            }
            break;
        }
    }
    ATTACH(node0, node1);
    }
    return node0;
}

ASN_NODE *
EncodeTargetPart(class_var, type, tagno, targetPart)
int class_var;
int type;
int tagno;
TargetPart *targetPart;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (targetPart->estimatedQuantity != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,1,targetPart->estimatedQuantity));
    }
    if (targetPart->quantitySoFar != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,2,targetPart->quantitySoFar));
    }
    if (targetPart->estimatedCost != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,3,targetPart->estimatedCost));
    }
    if (targetPart->costSoFar != 0) {
    ATTACH(node0, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,4,targetPart->costSoFar));
    }
    return node0;
}


ASN_NODE *
EncodeSearchInfoReport(class_var, type, tagno, searchInfoReport)
int class_var;
int type;
int tagno;
SearchInfoReport *searchInfoReport;
{
    struct SearchInfoReport *tmp1 = searchInfoReport;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        if (tmp1->item.subqueryId != 0) {
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,1,tmp1->item.subqueryId));
        }
        ATTACH(node1, EncodeBoolean(BER_CONTEXT,2,tmp1->item.fullQuery));
        if (tmp1->item.subqueryExpression != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
        ATTACH(node2, EncodeQueryExpression(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.subqueryExpression));
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.subqueryInterpretation != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,4);
        ATTACH(node2, EncodeQueryExpression(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.subqueryInterpretation));
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.subqueryRecommendation != 0) {
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
        ATTACH(node2, EncodeQueryExpression(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item.subqueryRecommendation));
        ATTACH(node1, node2);
        }
        }
        if (tmp1->item.subqueryCount != 0) {
        ATTACH(node1, EncodeInteger(BER_CONTEXT,6,tmp1->item.subqueryCount));
        }
        if (tmp1->item.subqueryWeight != 0) {
        ATTACH(node1, EncodeIntUnit(BER_CONTEXT,CONSTRUCTED,7,tmp1->item.subqueryWeight));
        }
        if (tmp1->item.resultsByDB != 0) {
        ATTACH(node1, EncodeResultsByDB(BER_CONTEXT,CONSTRUCTED,8,tmp1->item.resultsByDB));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeResultsByDB(class_var, type, tagno, resultsByDB)
int class_var;
int type;
int tagno;
ResultsByDB *resultsByDB;
{
    struct ResultsByDB *tmp1 = resultsByDB;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        ASN_NODE *node1 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
        {
        ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
        switch (tmp1->item.databases.which) {
            case e72_all: {
                ATTACH(node2, EncodeNull(BER_CONTEXT,1));
                break;
            }
            case e72_list: {
                {
                struct list_List104 *tmp2 = tmp1->item.databases.u.list;
                ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
                while (tmp2 != 0) {
                    ATTACH(node3, EncodeDatabaseName(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp2->item));
                    tmp2 = tmp2->next;
                }
                ATTACH(node2, node3);
                }
                break;
            }
        }
        ATTACH(node1, node2);
        }
        if (tmp1->item.count != 0) {
        ATTACH(node1, EncodeInteger(BER_CONTEXT,2,tmp1->item.count));
        }
        if (tmp1->item.resultSetName != 0) {
        ATTACH(node1, EncodeInternationalString(BER_CONTEXT,3,tmp1->item.resultSetName));
        }
        ATTACH(node0, node1);
        tmp1 = tmp1->next;
    }
    return node0;
}

ASN_NODE *
EncodeQueryExpression(class_var, type, tagno, queryExpression)
int class_var;
int type;
int tagno;
QueryExpression *queryExpression;
{
    ASN_NODE *node0;
    switch(queryExpression->which) {
        case e73_term: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            ATTACH(node2, EncodeTerm(CLASS_NONE,TYPE_NONE,TAGNO_NONE,queryExpression->u.term.queryTerm));
            ATTACH(node1, node2);
            }
            if (queryExpression->u.term.termComment != 0) {
            ATTACH(node1, EncodeInternationalString(BER_CONTEXT,2,queryExpression->u.term.termComment));
            }
            node0 = node1;
            break;
        }
        case e73_query: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeQuery(CLASS_NONE,TYPE_NONE,TAGNO_NONE,queryExpression->u.query));
            node0 = node1;
            }
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeEspec_1(class_var, type, tagno, espec_1)
int class_var;
int type;
int tagno;
Espec_1 *espec_1;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    if (espec_1->elementSetNames != 0) {
    {
    struct elementSetNames_List105 *tmp1 = espec_1->elementSetNames;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    if (espec_1->defaultVariantSetId != 0) {
    ATTACH(node0, EncodeOid(BER_CONTEXT,2,espec_1->defaultVariantSetId));
    }
    if (espec_1->defaultVariantRequest != 0) {
    ATTACH(node0, EncodeVariant(BER_CONTEXT,CONSTRUCTED,3,espec_1->defaultVariantRequest));
    }
    if (espec_1->defaultTagType != 0) {
    ATTACH(node0, EncodeInteger(BER_CONTEXT,4,espec_1->defaultTagType));
    }
    if (espec_1->elements != 0) {
    {
    struct elements_List106 *tmp1 = espec_1->elements;
    ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,5);
    while (tmp1 != 0) {
        ATTACH(node1, EncodeElementRequest(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
        tmp1 = tmp1->next;
    }
    ATTACH(node0, node1);
    }
    }
    return node0;
}

ASN_NODE *
EncodeElementRequest(class_var, type, tagno, elementRequest)
int class_var;
int type;
int tagno;
ElementRequest *elementRequest;
{
    ASN_NODE *node0;
    switch(elementRequest->which) {
        case e74_simpleElement: {
            node0 = EncodeSimpleElement(BER_CONTEXT,CONSTRUCTED,1,elementRequest->u.simpleElement);
            break;
        }
        case e74_compositeElement: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            switch (elementRequest->u.compositeElement.elementList.which) {
                case e75_primitives: {
                    {
                    struct primitives_List107 *tmp1 = elementRequest->u.compositeElement.elementList.u.primitives;
                    ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
                    while (tmp1 != 0) {
                        ATTACH(node3, EncodeInternationalString(CLASS_NONE,TAGNO_NONE,tmp1->item));
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node2, node3);
                    }
                    break;
                }
                case e75_specs: {
                    {
                    struct specs_List108 *tmp1 = elementRequest->u.compositeElement.elementList.u.specs;
                    ASN_NODE *node3 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
                    while (tmp1 != 0) {
                        ATTACH(node3, EncodeSimpleElement(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item));
                        tmp1 = tmp1->next;
                    }
                    ATTACH(node2, node3);
                    }
                    break;
                }
            }
            ATTACH(node1, node2);
            }
            ATTACH(node1, EncodeTagPath(BER_CONTEXT,CONSTRUCTED,2,elementRequest->u.compositeElement.deliveryTag));
            if (elementRequest->u.compositeElement.variantRequest != 0) {
            ATTACH(node1, EncodeVariant(BER_CONTEXT,CONSTRUCTED,3,elementRequest->u.compositeElement.variantRequest));
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}

ASN_NODE *
EncodeSimpleElement(class_var, type, tagno, simpleElement)
int class_var;
int type;
int tagno;
SimpleElement *simpleElement;
{
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    ATTACH(node0, EncodeTagPath(BER_CONTEXT,CONSTRUCTED,1,simpleElement->path));
    if (simpleElement->variantRequest != 0) {
    ATTACH(node0, EncodeVariant(BER_CONTEXT,CONSTRUCTED,2,simpleElement->variantRequest));
    }
    return node0;
}

ASN_NODE *
EncodeTagPath(class_var, type, tagno, tagPath)
int class_var;
int type;
int tagno;
TagPath *tagPath;
{
    struct TagPath *tmp1 = tagPath;
    ASN_NODE *node0;
    if (class_var != CLASS_NONE)
        node0 = NewAsnNode(class_var,type,tagno);
    else
        node0 = NewAsnNode(UNIVERSAL,CONSTRUCTED,SEQUENCE);
    while (tmp1 != 0) {
        case e76_specificTag: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,1);
            if (tmp1->item->u.specificTag.tagType != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,tmp1->item->u.specificTag.tagType));
            }
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node2, EncodeStringOrNumeric(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item->u.specificTag.tagValue));
            ATTACH(node1, node2);
            }
            if (tmp1->item->u.specificTag.occurrence != 0) {
            {
            ASN_NODE *node2 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
            ATTACH(node2, EncodeOccurrences(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item->u.specificTag.occurrence));
            ATTACH(node1, node2);
            }
            }
            node0 = node1;
            break;
        }
        case e76_wildThing: {
            {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,2);
            ATTACH(node1, EncodeOccurrences(CLASS_NONE,TYPE_NONE,TAGNO_NONE,tmp1->item->u.wildThing));
            node0 = node1;
            }
            break;
        }
        case e76_wildPath: {
            node0 = EncodeNull(BER_CONTEXT,3);
            break;
        }
        tmp1 = tmp1->next;
    }
    return node0;
}


ASN_NODE *
EncodeOccurrences(class_var, type, tagno, occurrences)
int class_var;
int type;
int tagno;
Occurrences *occurrences;
{
    ASN_NODE *node0;
    switch(occurrences->which) {
        case e77_all: {
            node0 = EncodeNull(BER_CONTEXT,1);
            break;
        }
        case e77_last: {
            node0 = EncodeNull(BER_CONTEXT,2);
            break;
        }
        case e77_values: {
            ASN_NODE *node1 = NewAsnNode(BER_CONTEXT,CONSTRUCTED,3);
            ATTACH(node1, EncodeInteger(BER_CONTEXT,1,occurrences->u.values.start));
            if (occurrences->u.values.howMany != 0) {
            ATTACH(node1, EncodeInteger(BER_CONTEXT,2,occurrences->u.values.howMany));
            }
            node0 = node1;
            break;
        }
    }
    return node0;
}
---- */

