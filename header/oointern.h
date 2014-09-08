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
ASN_NODE * EncodeRecords(Records *);
ASN_NODE * EncodeNamePlusRecord(int, int, int, NamePlusRecord *);
ASN_NODE * EncodeDiagRec(int, int, int, DiagRec *);
ASN_NODE * EncodeElementSetNames(int, int, int, ElementSetNames *);
ASN_NODE * EncodeDeleteResultSetRequest(int, int, int, DeleteResultSetRequest *);
ASN_NODE * EncodeDeleteResultSetResponse(int, int, int, DeleteResultSetResponse *);
ASN_NODE * EncodeListStatuses(int, int, int, ListStatuses *);
ASN_NODE *
EncodeAccessControlRequest(int, int, int, AccessControlRequest *);
ASN_NODE * EncodeAccessControlResponse(int, int, int, AccessControlResponse *);
ASN_NODE * EncodeResourceControlRequest(int, int, int, ResourceControlRequest *);
ASN_NODE * EncodeResourceControlResponse(int, int, int, ResourceControlResponse *);
ASN_NODE * EncodeTriggerResourceControlRequest(int, int, int, TriggerResourceControlRequest *);
ASN_NODE * EncodeResourceReportRequest(int, int, int, ResourceReportRequest *);
ASN_NODE * EncodeResourceReportResponse(int, int, int, ResourceReportResponse *);
ASN_NODE * EncodeResourceReport(int, int, int, ResourceReport *);
ASN_NODE * EncodeEstimate(int, int, int, Estimate *);
ASN_NODE * EncodePDU(PDU *);
ASN_NODE * EncodeInitializeRequest(int, int, int, InitializeRequest *);
ASN_NODE * EncodeInitializeResponse(int, int, int, InitializeResponse *);
ASN_NODE * EncodeSearchRequest(int, int, int, SearchRequest *);
ASN_NODE * EncodeQuery(int, int, int, Query *);
ASN_NODE * EncodeRPNQuery(int, int, int, RPNQuery *);
ASN_NODE * EncodeRPNStructure(ASN_NODE *asnNode, RPNStructure *);
ASN_NODE * EncodeOperand(int, int, int, Operand *);
ASN_NODE * EncodeAttributesPlusTerm(int, int, int, AttributesPlusTerm *);
ASN_NODE * EncodeAttributeList(int, int, int, AttributeList *);
ASN_NODE * EncodeOperator(int, int, int, Operator *);
ASN_NODE * EncodeAttributeElement(int, int, int, AttributeElement *);
ASN_NODE * EncodeSearchResponse(int, int, int, SearchResponse *);
ASN_NODE * EncodePresentRequest(int, int, int, PresentRequest *);
ASN_NODE * EncodePresentResponse(int, int, int, PresentResponse *);

