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
#ifndef ASN_DECODE_H
#define ASN_DECODE_H

#include "zprimitive.h"

/*
InitializeRequest * DecodeInitializeRequest (ASN_NODE *node);
InitializeResponse * DecodeInitializeResponse (ASN_NODE *node);
SearchRequest  * DecodeSearchRequest(ASN_NODE *node);
struct databaseNames_list * DecodeDatabaseNameList(ASN_NODE *node, int length);
AttributeElement * DecodeAttributeElement(ASN_NODE *node);
struct AttributeList * DecodeAttributeList(ASN_NODE *node, int length);
AttributesPlusTerm * DecodeAttributesPlusTerm(ASN_NODE *node, int length);
Operator * DecodeOperator(ASN_NODE *node, int len);
Operand * DecodeOperand(ASN_NODE *node, int length);
RPNStructure * DecodeRPNStructure(ASN_NODE *node);
RPNQuery * DecodeRPNQuery(ASN_NODE *node, int length);
Query * DecodeQuery(ASN_NODE *node, int length);
DiagRec * DecodeDiagRec(ASN_NODE *node, int length, int flag);
NamePlusRecord * DecodeNamePlusRecord (ASN_NODE *node);
struct dataBaseOrSurDiagnostics_list * DecodeNamePlusRecordList (ASN_NODE *node, int length);
Records * DecodeRecords (ASN_NODE *node, int length, int which);
SearchResponse  * DecodeSearchResponse(ASN_NODE *node);
ExtendedServicesRequest  *DecodeExtendedServicesRequest(ASN_NODE *node);
Close  *DecodeClose(ASN_NODE *node);
*/

#endif	/* ASN_DECODE_H	*/

