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
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "z3950_3.h"
#include "z_parameters.h"
#include "list.h"

#define	INDENT	4
static char format[50];

extern FILE *LOGFILE;
extern void PrintMarc (FILE *file, char *data);

void PrintDatabaseNames(FILE *file, struct databaseNames_List1 *dn);
void PrintElementSetNames(FILE *file, ElementSetNames *esn);
void PrintQuery (FILE *file, Query *q);
void PrintNamePlusRecord(FILE *file, NamePlusRecord	*namePlusRecord);
void PrintResourceReport(FILE *file, ResourceReport *resourceReport);
void PrintNonSurrogateDiagnostic (FILE *file, DefaultDiagFormat *diagRec);
void PrintRetrievalRecord(FILE *file, External *records);
void PrintOPACRecord(FILE *file, OPACRecord *opacRecord);
void PrintHoldingsRecord(FILE *file, HoldingsRecord *holdingsRecord);
void PrintInternationlString(FILE *file, char *id, InternationalString is);
void PrintHoldingsAndCircData (FILE *file, HoldingsAndCircData *hcData);
void PrintVolume (FILE *file, Volume *vol);
void PrintCircRecord (FILE *file, CircRecord *circRec);
void PrintBibliographicRecord (FILE *file, External *bibRecord);
void PrintBoolean(FILE *file, char *id, Boolean value);

void 
bit_pattern_8(data, buf)
unsigned char 	data;
char	*buf;
{
    int i = 0;

    for (i=0; i<8; i++) {
        if (data & (1 << (7-i)))
	    buf[i] = '1';
        else
	    buf[i] = '0';
    }
    buf[8] = '\0';
}
 
void 
PrintBitPattern(file,data, length)
FILE *file;
unsigned char	*data;
int	length;
{
    unsigned char	buffer[10];
    unsigned char	ch; 
    int	i=0;

    for (i=0; i<length; i++) {
        ch = data[i];
        bit_pattern_8(ch, buffer);
        fprintf(file,"%s ", buffer);
    }
}

void bit_pattern_16(data, buf)
unsigned short 	data;
char	*buf;
{
    int i = 0;

    for (i=0; i<16; i++) {
        if (data & (1 << (15-i)))
	    buf[i] = '1';
        else
	    buf[i] = '0';
    }
    buf[16] = '\0';
}


void bit_pattern_32(data, buf)
unsigned int 	data;
char	*buf;
{
    int i = 0;

    for (i=0; i<32; i++) {
        if (data & (1 << (31-i)))
	    buf[i] = '1';
        else
	    buf[i] = '0';
    }
    buf[32] = '\0';
}

void
IndentPrint(FILE *file, int level, char *format, ...)
{
    va_list	args;
    char fmt[50];

    sprintf(fmt, "%%%ds", INDENT*level);
    fprintf(file,fmt, "");

    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);
}
void
PrintBitString(file,id, bs)
FILE *file;
char *id;
BitString bs;
{
    int i;
    fprintf(file,"%s:\t", id);
    for (i=0; i<bs->length; i++)
	fprintf(file,"%c", bs->data[i]);
    fprintf(file,"\n");
}

void
PrintOctetString(file,id, os)
FILE *file;
char *id;
OctetString os;
{
    int i;
    fprintf(file,"%s:\t", id);
    for (i=0; i<os->length; i++)
	fprintf(file,"%c", os->data[i]);
    fprintf(file,"\n");
}


void
PrintOID(file,id, oi)
FILE *file;
char *id;
ObjectIdentifier oi;
{
    if (!oi)
	return;
    fprintf(file,"%s:\t%s\n", id, oi->data);
}

void
PrintInitializeRequest(file,ir)
FILE *file;
InitializeRequest *ir;
{
    if (ir->referenceId != 0)
	PrintOctetString(file,"referenceId", ir->referenceId);
    PrintBitString(file,"protocolVersion", ir->protocolVersion);
    PrintBitString(file,"options", ir->options);
    fprintf(file,"preferredMessageSize:	%d\n", ir->preferredMessageSize);
    fprintf(file,"exceptionalRecordSize:		%d\n", ir->exceptionalRecordSize);
    if (ir->implementationId != 0)
    	PrintOctetString(file,"implementationId", ir->implementationId);
    if (ir->implementationName != 0)
    	PrintOctetString(file,"implementationName", ir->implementationName);
    if (ir->implementationVersion != 0)
    	PrintOctetString(file,"implementationVersion", ir->implementationVersion);
}


void
PrintInitializeResponse (file,ir)
FILE *file;
InitializeResponse *ir;
{
    fprintf(file,"connection accepted: %s\n", (ir->result)?"YES":"NO");
    if (ir->referenceId != 0)
	PrintOctetString(file,"referenceId", ir->referenceId);
    PrintBitString(file,"protocolVersion", ir->protocolVersion);
    PrintBitString(file,"options", ir->options);
    fprintf(file,"preferredMessageSize:	%d\n", ir->preferredMessageSize);
    fprintf(file,"exceptionalRecordSize:		%d\n", ir->exceptionalRecordSize);
    if (ir->implementationId != 0)
    	PrintOctetString(file,"implementationId", ir->implementationId);
    if (ir->implementationName != 0)
    	PrintOctetString(file,"implementationName", ir->implementationName);
    if (ir->implementationVersion != 0)
    	PrintOctetString(file,"implementationVersion", ir->implementationVersion);
}

void
PrintSearchRequest(file,sr)
FILE *file;
SearchRequest *sr;
{
    if (sr == (SearchRequest *)NULL)
	return;
    if (sr->referenceId != 0)
	PrintOctetString(file,"referenceId", sr->referenceId);
    fprintf(file,"smallSetUpperBound:	%d\n", sr->smallSetUpperBound);
    fprintf(file,"largeSetLowerBound:		%d\n", sr->largeSetLowerBound);
    fprintf(file,"mediumSetPresentNumber:	%d\n", sr->mediumSetPresentNumber);
    fprintf(file,"replaceIndicator:	%d\n", sr->replaceIndicator);
    PrintOctetString(file,"result set name", sr->resultSetName);
    PrintDatabaseNames(file,sr->databaseNames);
    PrintElementSetNames (file,sr->smallSetElementSetNames);
    PrintElementSetNames (file,sr->mediumSetElementSetNames);
    PrintQuery(file,sr->query);
}

void
PrintDatabaseNames(file, dn)
FILE *file;
struct databaseNames_List1 *dn;
{
    struct databaseNames_List1 *tmp;
    if (!dn)
	return;

    for (tmp = dn; tmp != NULL; tmp = tmp->next)
	PrintOctetString(file,"database name", tmp->item);
}

void
PrintAttributeElement(file,ae, level)
FILE *file;
AttributeElement *ae;
int level;
{
    if (!ae)
	return;
    PrintOID (file,"Attribute Set", ae->attributeSet);
    IndentPrint(file,level, "attribute type :\t%d\n", ae->attributeType);
    if (ae->attributeValue.which == e7_numeric)
        IndentPrint(file,level, "attribute value:\t%d\n", 
		ae->attributeValue.u.numeric);
}

void
PrintAttributeList(file,al, level)
FILE *file;
AttributeList *al;
int level;
{
    AttributeList *tmp;
    if (!al)
	return;
    for (tmp = al; tmp != NULL; tmp = tmp->next)
	PrintAttributeElement(file,tmp->item, level);
}
void
PrintTerm(file,term)
FILE *file;
Term *term;
{
    if (!term)
	return;
    switch (term->which) {
	case e5_general:
	    PrintOctetString(file,"general", term->u.general);
	    break;
	case e5_numeric:
	    fprintf(file,"%d", term->u.numeric);
	    break;
	case e5_characterString:
	    PrintOctetString(file,"characterString", term->u.characterString);
	    break;
	default:
	    break;
    }
}

void
PrintAttributePlusTerm(file,apt, level)
FILE *file;
AttributesPlusTerm *apt;
int level;
{
    if (!apt)
	return;
    PrintAttributeList(file,apt->attributes, level);
    sprintf(format, "%%%ds", INDENT*level);
    fprintf(file,format, "");
    PrintTerm(file,apt->term);
}

void
PrintOperand(file,op, level)
FILE *file;
Operand *op;
int level;
{
    if (op == (Operand *)NULL)
	return;
    switch (op->which) {
	case e4_attrTerm:
	    PrintAttributePlusTerm(file,op->u.attrTerm, level);
	    break;
	case e4_resultSet:
	    PrintOctetString(file,"result set id",op->u.resultSet);
	    break;
	default:
	    break;
    }
}

void
PrintOperator(file,op, level)
FILE *file;
Operator *op;
int level;
{
  if (!op)
    return;
  switch (op->which) {
  case e6_and:
    IndentPrint(file,level, "AND\n");
    break;
  case e6_or:
    IndentPrint(file,level, "OR\n");
    break;
  case e6_and_not:
    IndentPrint(file,level, "NOT\n");
    break;
  case e6_prox:
    IndentPrint(file,level, "{Proximity Operator}\n");    
    break;
  }
}

void
PrintRPNStructure(file,rpns, level)
FILE *file;
RPNStructure *rpns;
int level;
{
    if (!rpns)
	return;
    switch (rpns->which) {
	case e3_op:
	    PrintOperand(file,rpns->u.op, level);
	    break;
	case e3_rpnRpnOp:
	    PrintOperator(file,rpns->u.rpnRpnOp.op, level);
	    PrintRPNStructure(file,rpns->u.rpnRpnOp.rpn1, level+1);
	    PrintRPNStructure(file,rpns->u.rpnRpnOp.rpn2, level+1);
	    break;
    }
}

void
PrintRPNQuery(file,rpn)
FILE *file;
RPNQuery *rpn;
{
    if (!rpn)
	return;
    PrintBitString(file,"attribute set id", rpn->attributeSet);
    PrintRPNStructure(file,rpn->rpn, 0);
}

void
PrintQueryType2 (file,type2Query)
FILE *file;
OctetString	type2Query;
{
    PrintOctetString(file,"type 2 query", type2Query);
}
void
PrintQuery(file,q)
FILE *file;
Query *q;
{
  if (!q)
    return;
  switch (q->which) {
  case e2_type_0:
    break;
  case e2_type_1:
    PrintRPNQuery(file,q->u.type_1);
    break;
  case e2_type_2:
    PrintQueryType2(file,q->u.type_2);
    break;
  case e2_type_100:
    fprintf(file,"Type 100 query: %s", q->u.type_100->data);
    break;
  case e2_type_101:
    PrintRPNQuery(file,q->u.type_101);
    break;
  case e2_type_102:
    fprintf(file,"Type 102 query: NO PRINT ROUTINE\n");
    break;
  }
}
void
PrintDiagRec(file,dr)
FILE *file;
DiagRec *dr;
{
    if (!dr)
	return;
    if (dr->which == e13_defaultFormat)
	PrintNonSurrogateDiagnostic (file,dr->u.defaultFormat);
}

void
PrintNonSurrogateDiagnostic (file,diagRec)
FILE *file;
DefaultDiagFormat	*diagRec;
{
    if (!diagRec)
	return;
    PrintOID(file,"diagnostic set id", diagRec->diagnosticSetId);
    fprintf(file,"condition: %d\n", diagRec->condition);
    if (diagRec->addinfo.which == e14_v2Addinfo)
    	PrintOctetString(file,"addinfo", diagRec->addinfo.u.v2Addinfo);
    else
    	PrintOctetString(file,"addinfo", diagRec->addinfo.u.v3Addinfo);
}

void
PrintNamePlusRecord(file,npr)
FILE *file;
NamePlusRecord	*npr;
{
  if (!npr)
    return;
  if (npr->name)
    PrintOctetString(file, "Database Name", npr->name);
  switch (npr->record.which) {
  case e11_retrievalRecord:
    /*
     * PrintMarc(file, npr->record.u.retrievalRecord->u.octetAligned->data);
     */
    PrintRetrievalRecord(file,npr->record.u.retrievalRecord);
    break;
  case e11_surrogateDiagnostic:
    PrintDiagRec(file,npr->record.u.surrogateDiagnostic);
    break;
  case e11_startingFragment:  
  case e11_intermediateFragment:  
  case e11_finalFragment:  
    fprintf(file,"PrintNamePlusRecord: Fragment printing not supported\n");
    break;

  }
}

void
PrintRetrievalRecord(file,records)
FILE *file;
External *records;
{
    char *oid;
    if (!records)
	return;

    switch (records->which) {
	case t_singleASN1type:
	    if (records->directReference) {
		oid = records->directReference->data;
	  	if (!oid) {
		    /* empty oid */
		    return;
		}
		if (strcmp(oid, OPACRECSYNTAX) == 0) {
		    PrintOPACRecord(file, (OPACRecord *)records->u.singleASN1type);
		} else if (strcmp(oid, EXPLAINRECSYNTAX) == 0) {
		    fprintf(file,"print explain records: not implemented.\n");
	 	} else if (strcmp(oid, GRS1RECSYNTAX) == 0) {
		    fprintf(file, "print GRS1 records: not implemented.\n");
	        } else if (strcmp(oid, SUMMARYRECSYNTAX) == 0) {
		    fprintf(file, "print summary records: not implemented.\n");
		} else {
		    fprintf(file,  "unknown oid: %s in PrintRetrievalRecord\n", oid);
		} 
	    }
	    break;
	case t_octetAligned:
	    if ((records->directReference) &&
		(records->directReference->data) && 
	   	(strcmp(records->directReference->data,OID_USMARC)==0)) {
			/* It is a marc record.	*/
		PrintMarc(file, records->u.octetAligned->data);
	    } else {	/* some octet string data	*/
		PrintOctetString(file, "content", records->u.octetAligned);
	    }
	    break;
	case t_arbitrary:
	    PrintBitString(file, "content", records->u.arbitrary);
	    break;
	default:
	    fprintf(file, "invalid type in PrintRetrievalRecord.\n");
	    break;
    }
}

void 
PrintOPACRecord(file, opacRecord)
FILE *file;
OPACRecord *opacRecord;
{
    struct holdingsData_List87 *tmp;

    if (!opacRecord)
	return;
    PrintBibliographicRecord (file, opacRecord->bibliographicRecord);

    for (tmp = opacRecord->holdingsData; tmp!=NULL; tmp = tmp->next)
    	PrintHoldingsRecord (file, tmp->item);
}

void
PrintHoldingsRecord(file, holdingsRecord)
FILE *file;
HoldingsRecord *holdingsRecord;
{
    if (!holdingsRecord)
    	return;
    if (holdingsRecord->which == e45_marcHoldingsRecord) {
	fprintf(stderr, "print marc holdings record: not implemented.\n");
    } else if  (holdingsRecord->which == e45_holdingsAndCirc) {
	PrintHoldingsAndCircData (file, holdingsRecord->u.holdingsAndCirc);
    } else {
	fprintf(stderr, "unknown data type in PrintHoldingsRecord.\b");
    }
}

void
PrintInternationalString(file, id, is)
FILE *file;
char *id;
InternationalString is;
{
    if (!is)
	return;
    PrintOctetString(file, id, is);
}

void
PrintHoldingsAndCircData (file,hcData)
FILE *file;
HoldingsAndCircData *hcData;
{
    struct volumes_List88 *tmp1;
    struct circulationData_List89 *tmp2;
    if (!hcData)
	return;

    PrintInternationalString(file, "typeOfRecord",hcData->typeOfRecord);
    PrintInternationalString(file,"encodingLevel",hcData->encodingLevel);
    PrintInternationalString(file,"format",hcData->format);
    PrintInternationalString(file, "receiptAcqStatus",hcData->receiptAcqStatus);
    PrintInternationalString(file, "generalRetention",hcData->generalRetention);
    PrintInternationalString(file, "completeness",hcData->completeness);
    PrintInternationalString(file, "dateOfReport",hcData->dateOfReport);
    PrintInternationalString(file, "nucCode",hcData->nucCode);
    PrintInternationalString(file, "localLocation",hcData->localLocation);
    PrintInternationalString(file, "shelvingLocation",hcData->shelvingLocation);
    PrintInternationalString(file, "callNumber",hcData->callNumber);
    PrintInternationalString(file, "shelvingData",hcData->shelvingData);
    PrintInternationalString(file, "copyNumber",hcData->copyNumber);
    PrintInternationalString(file, "publicNote",hcData->publicNote);
    PrintInternationalString(file, "reproductionNote",hcData->reproductionNote);
    PrintInternationalString(file, "termsUseRepro",hcData->termsUseRepro);
    PrintInternationalString(file, "enumAndChron",hcData->enumAndChron);

    for (tmp1=hcData->volumes; tmp1!=NULL; tmp1=tmp1->next)
	PrintVolume (file, tmp1->item);
    
    for (tmp2=hcData->circulationData; tmp2!=NULL; tmp2=tmp2->next)
	PrintCircRecord (file, tmp2->item);
}

void
PrintVolume (file, vol)
FILE *file;
Volume *vol;
{
    if (!vol)
	return;
    PrintInternationalString(file, "enumeration",vol->enumeration);
    PrintInternationalString(file, "chronology",vol->chronology);
    PrintInternationalString(file, "enumAndChron",vol->enumAndChron);
}

void
PrintCircRecord (file,circRec)
FILE *file;
CircRecord *circRec;
{
    if (!circRec)
	return;
    PrintBoolean(file, "availableNow", circRec->availableNow); 
    PrintInternationalString(file, "availablityDate",circRec->availablityDate);
    PrintInternationalString(file, "availableThru",circRec->availableThru);
    PrintInternationalString(file, "restrictions",circRec->restrictions);
    PrintInternationalString(file, "itemId",circRec->itemId);
    PrintBoolean(file, "renewable", circRec->renewable); 
    PrintBoolean(file, "onHold", circRec->onHold); 
    PrintInternationalString(file, "enumAndChron",circRec->enumAndChron);
    PrintInternationalString(file, "midspine",circRec->midspine);
    PrintInternationalString(file, "temporaryLocation",circRec->temporaryLocation);
}

void
PrintBibliographicRecord (file,bibRecord)
FILE *file;
External *bibRecord;
{
    if (!bibRecord)
	return;
    if ((bibRecord->directReference) &&
	(bibRecord->directReference->data) && 
	(strcmp(bibRecord->directReference->data,OID_USMARC)==0)) {
		/* It is a marc record.	*/
	PrintMarc(file, bibRecord->u.octetAligned->data);
    } else {	/* some octet string data	*/
	PrintOctetString(file,"content", bibRecord->u.octetAligned);
    }
}


void 
PrintRecords(file,records)
FILE *file;
Records *records;
{
  struct responseRecords_List6  *resp;
  struct multipleNonSurDiagnostics_List7 *md;

  if (records == NULL)
    return;
  
  switch (records->which) {
  case e10_responseRecords:
    resp = records->u.responseRecords;
    while(resp) {
      PrintNamePlusRecord(file,resp->item);
      resp = resp->next;
    }
    break;
  case e10_nonSurrogateDiagnostic:
    PrintNonSurrogateDiagnostic(file,records->u.nonSurrogateDiagnostic);
    break;
  case e10_multipleNonSurDiagnostics:
    for(md = records->u.multipleNonSurDiagnostics; md != NULL; md = md->next) {
      PrintDiagRec(file, md->item);
    }
    break;
  }
}

void
PrintSearchResponse(file,sr)
FILE *file;
SearchResponse *sr;
{
    if (!sr)
	return;
    fprintf(file,"Hits:\t%d\tReturned:\t%d\n", 
	sr->resultCount,
	sr->numberOfRecordsReturned);
    fprintf(file,"searchStatus:\t\t%d\n", sr->searchStatus);
    PrintRecords(file,sr->records);
}

void
PrintDeleteResultSetRequest(file,rsr)
FILE *file;
DeleteResultSetRequest *rsr;
{
  struct resultSetList_List11  *tmp = rsr->resultSetList;
  if (!rsr)
    return;
  
  if (rsr->referenceId)
    PrintOctetString(file,"reference id", rsr->referenceId);
  fprintf(file,"delete operation:\t%d\n", rsr->deleteFunction);
  
  while (tmp) {
    PrintOctetString(file,"result set", tmp->item);
    tmp = tmp->next;
  }
}

void
PrintListStatuses(file,ls)
FILE *file;
ListStatuses *ls;
{
    ListStatuses *tmp = ls;
    if (!ls)
	return;
    while (tmp) {
	PrintOctetString(file,"result set id", tmp->item.id);
	fprintf(file,"delete set status:\t%d\n", tmp->item.status);
	tmp = tmp->next;
    }
}

void
PrintDeleteResultSetResponse(file,rsr)
FILE *file;
DeleteResultSetResponse *rsr;
{
    if (rsr->referenceId)
	PrintOctetString(file,"reference id", rsr->referenceId);
    fprintf(file,"delete operation status:\t%d\n", rsr->deleteOperationStatus);
    if (rsr->deleteListStatuses)
	PrintListStatuses(file,rsr->deleteListStatuses);
    if (rsr->numberNotDeleted)
	fprintf(file,"number not deleted:\t%d\n", rsr->numberNotDeleted);
    if (rsr->bulkStatuses)
	PrintListStatuses(file,rsr->bulkStatuses);
    if (rsr->deleteMessage)
	PrintOctetString(file,"delete message", rsr->deleteMessage);
}

void
PrintElementSetNames(file,esn)
FILE *file;
ElementSetNames *esn;
{
    if (esn == NULL)
	return;
    switch (esn->which) {
	case e15_genericElementSetName:
	    PrintOctetString(file,"element set names", esn->u.genericElementSetName);
	    break;
	case e15_databaseSpecific:
	    {
	    struct databaseSpecific_List8	*tmp;
	    for (tmp=esn->u.databaseSpecific; tmp!=NULL; tmp=tmp->next) {
		PrintOctetString(file,"database name",tmp->item.dbName);
		PrintOctetString(file,"element set name",tmp->item.esn);
	    }
	    }
	default:
	    break;
    }
}

void
PrintOtherInformation(FILE *file, OtherInformation *in)
{
  OtherInformation *oi;

  if (!in)
    return;

  fprintf(file, "OtherInformation:\n");
  for (oi = in; oi != NULL; oi = oi->next) {
    if (oi->item.category) {
      PrintOID(file,"InfoCategory", oi->item.category->categoryTypeId);
      fprintf(file,"CategoryValue\t%d\n",oi->item.category->categoryValue);
    }
    if (oi->item.information.which == e24_characterInfo) 
      PrintOctetString(file,"Info: ", oi->item.information.u.characterInfo);
    else if (oi->item.information.which == e24_binaryInfo) 
      PrintOctetString(file,"Info: ", oi->item.information.u.binaryInfo);
    else if (oi->item.information.which == e24_oid) 
      PrintOID(file,"Info: OID ", oi->item.information.u.oid);
    else
      fprintf(file,"Externally Defined\n");
  }
  
}


void
PrintPresentRequest(file,pr)
FILE *file;
PresentRequest *pr;
{
    if (pr->referenceId)
	PrintOctetString(file,"reference id", pr->referenceId);
    PrintOctetString(file,"Result Set", pr->resultSetId);
    fprintf(file,"Start Point\t%d\n", pr->resultSetStartPoint);
    fprintf(file,"Number Of Records Requested\t%d\n", pr->numberOfRecordsRequested);
    if (pr->preferredRecordSyntax)
	PrintOID(file,"Preferred Record Syntax", pr->preferredRecordSyntax);
}

void
PrintPresentResponse(file,pr)
FILE *file;
PresentResponse *pr;
{
    if (pr->referenceId)
	PrintOctetString(file,"reference id", pr->referenceId);
    fprintf(file,"Number Of Records Returned:\t%d\n", pr->numberOfRecordsReturned);
    fprintf(file,"Next Result Set Position:\t%d\n", pr->nextResultSetPosition);
    fprintf(file,"Present Status:\t%d\n", pr->presentStatus);
    PrintRecords(file,pr->records);
}

void 
PrintSortKey(FILE *file, SortKey *sk) {
  Specification *spec;
  
  if (sk->which == e23_sortfield) {
    PrintOctetString(file,"SortField: ", sk->u.sortfield);
  } else if (sk->which == e23_elementSpec) {
    spec = sk->u.elementSpec;
    fprintf(file,"ElementSpecification: \n");
    PrintOID(file, "Schema", spec->schema);
    if (spec->elementSpec->which == e16_elementSetName) 
      PrintOctetString(file,"ElementSetName: ", spec->elementSpec->u.elementSetName);
    else 
      fprintf(file, "    External specification\n");
  } else if (sk->which == e23_sortAttributes) {
    fprintf(file,"SortAttributes: \n");
    PrintOID(file,"AttributeSet OID", sk->u.sortAttributes.id);
    PrintAttributeList(file,sk->u.sortAttributes.list,2);
  } 
  
}

void
PrintSortKeySpec(FILE *file, SortKeySpec *sk)
{
  struct datbaseSpecific_List20 *dl;

  if (sk->sortElement->which == e22_generic) {
    fprintf(file,"Generic Sortkey: \n");
    PrintSortKey(file, sk->sortElement->u.generic);
  }
  else if (sk->sortElement->which == e22_datbaseSpecific) {
    fprintf(file,"Database Specific Sortkeys: \n");
    for(dl = sk->sortElement->u.datbaseSpecific; dl != NULL; dl = dl->next) {
      PrintOctetString(file,"DB: ", dl->item.databaseName);
      PrintSortKey(file, dl->item.dbSort);
    }
  }
  
}


void
PrintSortRequest(file,sr)
FILE *file;
SortRequest *sr;
{
  struct inputResultSetNames_List17 *in;
  struct sortSequence_List18 *ss;
  if (sr->referenceId)
    PrintOctetString(file,"reference id", sr->referenceId);
  fprintf(file,"Input ResultSet(s):\n");
  for (in = sr->inputResultSetNames; in != NULL ; in = in->next) {
    PrintOctetString(file,"   ", in->item);
  }
  PrintOctetString(file,"Output ResultSetName: ", sr->sortedResultSetName);
  
  fprintf(file,"Sort Key Specs:\n");
  for (ss = sr->sortSequence; ss != NULL; ss = ss->next) {
    PrintSortKeySpec(file, ss->item);
    fprintf(file,"SortRelation: %d\n", ss->item->sortRelation);
    fprintf(file,"CaseSensitivity: %d\n", ss->item->caseSensitivity);

    if (ss->item->missingValueAction.which == e21_abort) {
      fprintf(file, "Missing Values: Abort\n");
    }
    if (ss->item->missingValueAction.which == e21_nullVal) {
      fprintf(file, "Missing Values: Null\n");
    }
    if (ss->item->missingValueAction.which == e21_missingValueData) {
      PrintOctetString(file,"Missing Values Data: ", ss->item->missingValueAction.u.missingValueData);
    }
  }
  
}

void
PrintSortResponse(file,sr)
FILE *file;
SortResponse *sr;
{
  struct diagnostics_List19 *dr;

    if (sr->referenceId)
	PrintOctetString(file,"reference id", sr->referenceId);

    fprintf(file,"Sort Status:\t%d\n", sr->sortStatus);
    fprintf(file,"ResultSet Status:\t%d\n", sr->resultSetStatus);
    for (dr = sr->diagnostics; dr != NULL; dr = dr->next) {
      fprintf(file,"Sort Diagnostic: \n");
      PrintDiagRec(file,dr->item);
    }
    PrintOtherInformation(file, sr->otherInfo);
}

void
PrintResourceReportRequest(file,rrr)
FILE *file;
ResourceReportRequest *rrr;
{
    if (rrr->referenceId)
	PrintOctetString(file,"reference id", rrr->referenceId);
    if (rrr->opId)
	PrintOctetString(file,"Op id", rrr->opId);
    if (rrr->prefResourceReportFormat)
	PrintOctetString(file,"Preferred Format", 
			 rrr->prefResourceReportFormat);

}

void
PrintResourceReportResponse(file,rrr)
FILE *file;
ResourceReportResponse *rrr;
{
    if (rrr->referenceId)
	PrintOctetString(file,"reference id", rrr->referenceId);
    fprintf(file,"Resource Report Status:\t%d\n", rrr->resourceReportStatus);
    PrintResourceReport(file,rrr->resourceReport);
}

void
PrintEstimate(file,estimate)
FILE *file;
Estimate	*estimate;
{
    if (estimate == (Estimate *)NULL)
	return;
    fprintf(file,"estimateType:\t%d\n", estimate->type);
    fprintf(file,"estimateValue:\t%d\n", estimate->value);
    fprintf(file,"currency_code:\t%d\n", estimate->currency_code);
}

void 
PrintResourceReport(file,resourceReport)
FILE *file;
ResourceReport *resourceReport;
{
    struct estimates_List96 *tmp;

    if (resourceReport == (ResourceReport *)NULL)
	return;
    for (tmp = resourceReport->estimates; tmp!=NULL; tmp = tmp->next)
	PrintEstimate(file,tmp->item);
    PrintOctetString(file,"message", resourceReport->message);
    return;
}

void
PrintResourceControlRequest(file,rcr)
FILE *file;
ResourceControlRequest *rcr;
{
    if (rcr == (ResourceControlRequest *)NULL)
	return;
    if (rcr->referenceId)
	PrintOctetString(file,"reference id", rcr->referenceId);
    PrintBoolean(file,"suspendedFlag", rcr->suspendedFlag); 
    if (rcr->resourceReport)
	PrintResourceReport(file,rcr->resourceReport);
    if (rcr->partialResultsAvailable)
	fprintf(file,"partialResultsAvailable:\t%d\n", rcr->partialResultsAvailable);
    PrintBoolean(file,"responseRequired", rcr->responseRequired);
    PrintBoolean(file,"triggeredRequestFlag", rcr->triggeredRequestFlag);
}

void
PrintResourceControlResponse(file,rcr)
FILE *file;
ResourceControlResponse *rcr;
{
    if (rcr == (ResourceControlResponse *)NULL)
	return;
    if (rcr->referenceId)
	PrintOctetString(file,"reference id", rcr->referenceId);
    PrintBoolean(file,"Continue Flag", rcr->continueFlag); 
    PrintBoolean(file,"Result Set Wanted", rcr->resultSetWanted);
}

void
PrintAccessControlRequest(file,acr)
FILE *file;
AccessControlRequest *acr;
{
    if (acr == (AccessControlRequest *)NULL)
	return;
    if (acr->referenceId)
	PrintOctetString(file,"reference id", acr->referenceId);
    PrintOctetString(file,"security challenge", acr->securityChallenge);
}

void
PrintAccessControlResponse(file,acr)
FILE *file;
AccessControlResponse *acr;
{
    if (acr == (AccessControlResponse *)NULL)
	return;
    if (acr->referenceId)
	PrintOctetString(file,"reference id", acr->referenceId);
    PrintOctetString(file,"security challenge response", acr->securityChallengeResponse);
}

void 
PrintTriggerResourceControlRequest(file,trcr)
FILE *file;
TriggerResourceControlRequest *trcr;
{
    if (trcr == (TriggerResourceControlRequest *)NULL)
	return;
    if (trcr->referenceId)
	PrintOctetString(file,"reference id", trcr->referenceId);
    fprintf(file,"Requested Operation\t%d\n", trcr->requestedAction);
    PrintOID(file,"report format", trcr->prefResourceReportFormat);
    PrintBoolean(file,"Result Set Wanted", trcr->resultSetWanted);
    return;
}

void
PrintPDU(file, pdu)
FILE *file;
PDU  *pdu;
{
	if (!pdu) {
		fprintf(file, "Empty PDU.\n");
		return;
	}
	switch(pdu->which) {
	    case e1_initRequest: {
	        fprintf(file,"INIT Request:\n");
	        PrintInitializeRequest(file,pdu->u.initRequest);
	        break;
	    }
	    case e1_initResponse: {
	        fprintf(file,"INIT Response:\n");
	        PrintInitializeResponse(file,pdu->u.initResponse);
	        break;
	    }
	    case e1_searchRequest: {
	        fprintf(file,"SEARCH Request:\n");
	        PrintSearchRequest(file,pdu->u.searchRequest);
	        break;
	    }

	    case e1_searchResponse: {
	        fprintf(file,"SEARCH Response:\n");
	        PrintSearchResponse(file,pdu->u.searchResponse);
	        break;
	    }
	    case e1_presentRequest: {
	        fprintf(file,"PRESENT Request:\n");
	        PrintPresentRequest(file,pdu->u.presentRequest);
		break;
	      }
	    case e1_presentResponse: {
	      fprintf(file,"PRESENT Response:\n");
	      PrintPresentResponse(file,pdu->u.presentResponse);
	      break;
	    }
	    case e1_sortRequest: {
	        fprintf(file,"SORT Request:\n");
	        PrintSortRequest(file,pdu->u.sortRequest);
		break;
	      }
	    case e1_sortResponse: {
	      fprintf(file,"SORT Response:\n");
	      PrintSortResponse(file,pdu->u.sortResponse);
	      break;
	    }
	    case e1_deleteResultSetRequest: {
	      fprintf(file,"DELETE Request:\n");
	      PrintDeleteResultSetRequest(file,pdu->u.deleteResultSetRequest);
	      break;
	    }
	    case e1_deleteResultSetResponse: {
	      fprintf(file,"PRESENT Response:\n");
	      PrintDeleteResultSetResponse(file,pdu->u.deleteResultSetResponse);
	      break;
	    }
	    case e1_accessControlRequest: {
	      fprintf(file,"ACCESSCONTROL Request:\n");
	      PrintAccessControlRequest(file,pdu->u.accessControlRequest);
	      break;
	    }
	    case e1_accessControlResponse: {
	      fprintf(file,"ACCESSCONTROL Response:\n");
	      PrintAccessControlResponse(file,pdu->u.accessControlResponse);
	      break;
	    }
	    case e1_resourceControlRequest: {
	      fprintf(file,"RESOURCECONTROL Request:\n");
	      PrintResourceControlRequest(file,pdu->u.resourceControlRequest);
	      break;
	    }
	    case e1_resourceControlResponse: {
	      fprintf(file,"RESOURCECONTROL Response:\n");
	      PrintResourceControlResponse(file,pdu->u.resourceControlResponse);
	      break;
	    }
	    case e1_triggerResourceControlRequest: {
	      fprintf(file,"TRIGGERRESOURCECONTROL Request:\n");
	      PrintTriggerResourceControlRequest(file,pdu->u.triggerResourceControlRequest);
	      break;
	    }
	    case e1_resourceReportRequest: {
	      fprintf(file,"RESOURCEREPORT Request:\n");
	      PrintResourceReportRequest(file,pdu->u.resourceReportRequest);
	      break;
	    }
	    case e1_resourceReportResponse: {
	      fprintf(file,"RESOURCEREPORT Request:\n");
	      PrintResourceReportResponse(file,pdu->u.resourceReportResponse);
	      break;
	    }
	    default:
	        break;
	}
}
void
PrintBoolean(file,id, value)
FILE *file;
char *id;
Boolean value;
{
    fprintf(file,"%s:\t%s\n", id, value?"TRUE":"FALSE");
}


