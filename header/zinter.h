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
#ifndef	ZINTER_H
#define ZINTER_H

#include "session.h"
#include "zprimitive.h"

int HandleClientConnection (ZSESSION *sess, int num);

void bit_pattern_8(unsigned char data, char *buf);
void print_bit_pattern(unsigned char *data, int length);
void bit_pattern_16(unsigned short data, char *buf);
void bit_pattern_32(unsigned int data, char *buf);

void print_BitString(char *id, BitString bs);
void print_OctetString(char *id, ObjectIdentifier oi);
void print_ir (struct InitializeRequest *ir);
void print_init_response (struct InitializeResponse *ir);
void print_search_request (struct SearchRequest *sr);
/*
void print_database_names (struct SearchRequest_list *dn);
*/

void print_attr_element (AttributeElement *ae);
void print_attr_list (AttributeList *al);
void print_attr_plus_term (AttributesPlusTerm *apt);
void print_operand (Operand *op);
void print_rpn_structure (RPNStructure *rpns);
/*
void print_rpn_query (PRNQuery *rpn);
*/
void print_query (Query *q);

void print_tag (int tag, int indent);

void print_delete_result_set_request (DeleteResultSetRequest *rsr);
void print_list_statuses (ListStatuses *ls);

void print_delete_result_set_response (DeleteResultSetResponse *rsr);
void print_element_set_names (ElementSetNames *esn);

void print_present_request (PresentRequest *pr);
void print_present_response (PresentResponse *pr);
void PrintPDU(FILE *file, PDU *pdu);

void FreePDU (PDU *);


void get_one_marc (int start);

PDU *MakeInitializeRequest(char *, char *, char *, int, int, any, 
			char *, char *, char *, External *);
PDU *MakeInitializeResponse(OctetString, char *, char *, int, int, Boolean, 
			char *, char *, char *, External *);

PDU *MakeSearchRequest(char *, int, int, int, Boolean, char *, char **,
		       ElementSetNames *, ElementSetNames *, char *, 
		       Query *, OtherInformation *);
PDU *MakeSearchResponse(OctetString, int, int, int, Boolean, int, int, Records *);

PDU *MakePresentRequest(char *, char *, int, int, char*, char *, OtherInformation *);
PDU *MakePresentResponse(OctetString, int, int, int, Records *);

PDU *MakeDeleteResultSetRequest(char *, int, char **, int);
PDU *MakeDeleteResultSetResponse(OctetString, int, ListStatuses *, int, 
			ListStatuses *, char *);

PDU *MakeScanRequest(char *referenceId, char **dbnames,
		     char *attributeSetId, AttributesPlusTerm *terms,
		     int stepsize, int numberofterms, int preferredpos);

PDU *MakeScanResponse(OctetString referenceId, int stepUsed, int status, 
		      int position, char *attributesetid,
		      ListEntries *listEntries);

PDU *MakeSortRequest(char *referenceId, char **in_names, char *out_name,
		     struct sortSequence_List18 *sort_seq);
PDU *MakeSortResponse(OctetString referenceId, int status, int resultstatus,
		      struct diagnostics_List19 *diagnostics, int resultsetsize);


PDU *MakeResouceControlResponse();

Records *MakeDatabaseRecords(AC_LIST *, int, int, char *);
Records *MakeDiagRecords();
Records *MakeNonSurrogateDiagRecords(char *, int, char *);

DiagRec *MakeDiagRec(char *, int, char *);

ElementSetNames *MakeElementSetNames (char **databaseNames,
				      char **elementSetNames);

AttributesPlusTerm *test_attrs();
RPNStructure *test_rpn();
Query *test_query();
SearchRequest *test_request();
void print_search_response(SearchResponse *sr);
int lookup_command (char *com);


ASN_NODE  *NewAsnNode(int, int, int);
ASN_NODE  *Attach(ASN_NODE *, ASN_NODE *);
void PrintAsnTree(ASN_NODE *, int);
int LengthAsnTree(ASN_NODE *);
int LengthAsnTree2(ASN_NODE *);
int BerAsnTree(ASN_NODE *, DBUF *);
void FreeAsnNode(ASN_NODE *);
DBUF *SerializeAsnTree (ASN_NODE *);


int db_put(DBUF *, unsigned char *, int);
int db_get(DBUF *, unsigned char *, int);
DBUF *NewDBuf();
void FreeDbuf (DBUF *);
void PrintDbuf(DBUF *);

PDU *GetPDU(int fd, int timeoutsecs);
int TransmitPDU(int, DBUF *);
int WaitForRead(int, int, int);
int PutPDU (int fd, PDU *pdu);

DBUF *EncodePDU2(PDU *);

/*
 * routines defined in file	asn_decode.c
 */
int EOB(DBUF *);
External *DecodeExternal (ASN_NODE *node);

InitializeRequest *DecodeInitializeRequest (ASN_NODE *node);
InitializeResponse *DecodeInitializeResponse (ASN_NODE *node);
SearchRequest  *DecodeSearchRequest(ASN_NODE *node);
struct databaseNames_list *DecodeDatabaseNameList(ASN_NODE *node, int length);
struct databaseSpecific_list *DecodeDatabaseSpecificList(ASN_NODE *node, int length);
ElementSetNames *DecodeElementSetNames(ASN_NODE *node);
AttributeElement *DecodeAttributeElement(ASN_NODE *node);
struct AttributeList *DecodeAttributeList(ASN_NODE *node);
AttributesPlusTerm *DecodeAttributesPlusTerm(ASN_NODE *node);
Operator *DecodeOperator(ASN_NODE *node);
Operand *DecodeOperand(ASN_NODE *node);
RPNStructure *DecodeRPNStructure(ASN_NODE *node);
RPNQuery *DecodeRPNQuery(ASN_NODE *node);
Query *DecodeQuery(ASN_NODE *node);
DiagRec *DecodeDiagRec(ASN_NODE *node);
NamePlusRecord *DecodeNamePlusRecord (ASN_NODE *node);
struct dataBaseOrSurDiagnostics_list *DecodeNamePlusRecordList (ASN_NODE *node, int length);
Records *DecodeRecords (ASN_NODE *node, int *count);
SearchResponse  *DecodeSearchResponse(ASN_NODE *node);
PresentRequest  *DecodePresentRequest(ASN_NODE *node);
PresentResponse  *DecodePresentResponse(ASN_NODE *node);
struct resultSetList_list *DecodeResultSetList (ASN_NODE *node);
DeleteResultSetRequest  *DecodeDeleteResultSetRequest(ASN_NODE *node);
struct ListStatuses *DecodeListStatuses (ASN_NODE *node);
DeleteResultSetResponse  *DecodeDeleteResultSetResponse(ASN_NODE *node);
AccessControlRequest  *DecodeAccessControlRequest(ASN_NODE *node);
AccessControlResponse  *DecodeAccessControlResponse(ASN_NODE *node);
struct Estimate *DecodeEstimate (ASN_NODE *node);
struct estimates_list *DecodeEstimatesList (ASN_NODE *node, int length);
ResourceReport *DecodeResourceReport(ASN_NODE *node);
ResourceControlRequest  *DecodeResourceControlRequest(ASN_NODE *node);
ResourceControlResponse  *DecodeResourceControlResponse(ASN_NODE *node);
TriggerResourceControlRequest  *DecodeTriggerResourceControlRequest(ASN_NODE *node);
ResourceReportRequest  *DecodeResourceReportRequest(ASN_NODE *node);
ResourceReportResponse  *DecodeResourceReportResponse(ASN_NODE *node);
ScanRequest  *DecodeScanRequest(ASN_NODE *node);
ScanResponse  *DecodeScanResponse(ASN_NODE *node);
SortRequest  *DecodeSortRequest(ASN_NODE *node);
SortResponse  *DecodeSortResponse(ASN_NODE *node);
Segment  *DecodeSegment(ASN_NODE *node);
PDU *DecodePDU(ASN_NODE *node);


/*
 * routines defined in file	asn_encode.c
 */

/*
ASN_NODE *
EncodeExternal(int class_var, int tagno, External *external);
ASN_NODE *EncodeRecords(Records *records);
ASN_NODE *EncodeNamePlusRecord(int class_var, int type, int tagno, NamePlusRecord *namePlusRecord);
ASN_NODE *EncodeDiagRec(int class_var, int type, int tagno, DiagRec *diagRec);
ASN_NODE *EncodeElementSetNames(int class_var, int type, int tagno, ElementSetNames *elementSetNames);
ASN_NODE *EncodeDeleteResultSetRequest(int class_var, int type, int tagno, DeleteResultSetRequest *deleteResultSetRequest);
ASN_NODE *EncodeDeleteResultSetResponse(int class_var, int type, int tagno, DeleteResultSetResponse *deleteResultSetResponse);
int GetTAG(DBUF *dbuf, int *class_var, int *type, int *tagno);
int GetLEN(DBUF *dbuf);
int GetTAGFromStream2(int fd, int *class_var, int *type, int *tagno, int *size);
int GetLENFromStream2(int fd, int *size);
ASN_NODE *GetPDUFromStream(int fd, int *size);
OctetString DecodeOctetString(DBUF *dbuf, int length);
OctetString DecodeVisibleString(DBUF *dbuf, int length);
BitString DecodeBitString(DBUF *dbuf, int length);
int DecodeInteger(DBUF *dbuf, int length);
char DecodeBoolean(DBUF *dbuf, int length);
ObjectIdentifier DecodeOid(DBUF *dbuf, int length);
any DecodeAny(DBUF *dbuf, int length);
char DecodeNull(DBUF *dbuf, int length);
*/

/*
 * routines defined in file	asn_encode.c
 */
/*
int PutLEN(DBUF *dbuf, unsigned int length);
int PutTAG(DBUF *dbuf, int class_var, int type, int tagno);
void encode_octstr(DBUF *dbuf, int class_var, int tagno, OctetString obj);
void encode_bitstr(DBUF *dbuf, int class_var, int tagno, OctetString obj);
void encode_integer(DBUF *dbuf, int class_var, int tagno, int obj);
void encode_oid_component(DBUF *buf, int component);
void encode_oid(DBUF *dbuf, int class_var, int tagno, ObjectIdentifier obj);
void encode_boolean(DBUF *dbuf, int class_var, int tagno, UCHAR bl);
void encode_null(DBUF *dbuf, int class_var, int tagno);
ASN_NODE *EncodeBoolean(int class_var, int tagno, unsigned char obj);
ASN_NODE *EncodeNull(int class_var, int tagno);
ASN_NODE *EncodeAny(int class_var, int tagno, any obj);
ASN_NODE *EncodeOid(int class_var, int tagno, ObjectIdentifier obj);
ASN_NODE *EncodeInteger(int class_var, int tagno, int obj);
ASN_NODE *EncodeOctetString(int class_var, int tagno, OctetString obj);
ASN_NODE *EncodeBitString(int class_var, int tagno, BitString obj);
*/
/*
 * routines defined in file	utils.c
 */

char* DEO(OctetString o);
OctetString O(char* s);
OctetString NewOctetString(char* s);
BitString NewBitString(char* s);
ObjectIdentifier NewOID(char* s);
AttributeSetId NewAttributeSetId(char* s);
struct internal_oid *oid_convert(char *oidstr);
void PrintInternalOID(struct internal_oid *the_oid);
char* zap_oid(char* buf,int val);
ObjectIdentifier OID(int count,int* a);
int CopyOctetString(char *buffer, OctetString octetString);


/*
 * routines defined in file	exec_query.c
 */
int ExecuteQuery(ZSESSION *session, Query *query, char **database, char *resultSetId);
AC_LIST *RetrieveRecords(ZSESSION *session, char *resultSetId, int start, int number, int elementset);

/*
 * routines defined in file	z_find.c
 */
AC_LIST *ZFind(ZSESSION *session, Query *query, char *databases);
AC_LIST *FindType1Query(ZSESSION *session, char *database, RPNQuery *rpnQuery);
AC_LIST *ExecuteType1Query(ZSESSION *session, char *database, RPNStructure *rpnStructure);

/*
 * routines defined in file	search_key.c
 */
void save_accnos(ZSESSION *session, char *file, AC_LIST *alist);
AC_LIST *GetResultSet(ZSESSION *session, char *file);
AC_LIST *GetPartialResultSet(ZSESSION *session, char *file, int start, int number);

/*
 * routines defined in file	server_utils.c	
 */
PDU *HandleInitializeRequest (ZSESSION *session, InitializeRequest *initRequest);
PDU *HandleSearchRequest (ZSESSION *session, SearchRequest *searchRequest);
PDU *HandlePresentRequest (ZSESSION *session, PresentRequest *presentRequest);
PDU *HandleDeleteResultSetRequest (ZSESSION *session, DeleteResultSetRequest *deleteResultSetRequest);

#endif	/* 	ZINTER_H	*/

