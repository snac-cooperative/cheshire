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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "z3950_3.h"
#include "z_parameters.h"

ObjectIdentifier DupOID (ObjectIdentifier);
OctetString DupOstring (OctetString);
BitString DupBstring (BitString);
InternationalString DupInternationalString(InternationalString);
any DupASN1type(DOCTYPE, any) ;
External *DupExternal (External *);
OPACRecord *DupOPACRecord(OPACRecord *);
Explain_Record *DupExplainRecord(Explain_Record *);
GenericRecord * DupGRS1Record(GenericRecord *);
TaskPackage *DupESRecord(TaskPackage *);
BriefBib *DupSummaryRecord(BriefBib *);
TaggedElement *DupTaggedElement(TaggedElement *);
ElementMetaData *DupElementMetaData(ElementMetaData *);
ElementInfo *DupElementInfo(ElementInfo *);


GSTR * 
NewGSTR(char *s) {
  /* s MUST be NULL-terminated string.	*/
  GSTR *gstr;
  if (s == NULL)
    return (GSTR *)NULL;
  if ((gstr = CALLOC(GSTR,1)) == NULL)
    return (GSTR *)NULL;
  gstr->length = strlen(s);
  gstr->data = (UCHAR *)calloc(1, strlen(s)+1);
  if (gstr->data == NULL)
    return (GSTR *)NULL;
  strcpy(gstr->data, s);
  gstr->data[gstr->length] = '\0';
  return gstr;
}

OctetString 
NewOctetString(char *str) 
{
	return (OctetString)NewGSTR(str);
}

OctetString 
NewOctetStringN(char *str, int N) 
{
  char *tmp;
  OctetString s;
  
  tmp = CALLOC(UCHAR, N+1);

  strncpy(tmp, str, N);
  s = (OctetString)NewGSTR(tmp);
  FREE (tmp);
  return (s);
}

BitString 
NewBitString(char* str) 
{
	return (BitString)NewGSTR(str);
}

ObjectIdentifier 
NewOID(char* str) 
{
	return (ObjectIdentifier)NewGSTR(str);
}
ObjectDescriptor 
NewObjectDescriptor(char* str) 
{
	return (ObjectDescriptor)NewGSTR(str);
}
AttributeSetId 
NewAttributeSetId(char* str) 
{
	return (AttributeSetId)NewGSTR(str);
}
NumericString 
NewNumeric(char* str) 
{
	return (NumericString)NewGSTR(str);
}
PrintableString 
NewPrintableString(char* str) 
{
	return (PrintableString)NewGSTR(str);
}
IA5String 
NewIA5String(char* str) 
{
	return (IA5String)NewGSTR(str);
}
UTCTime 
NewUTCTime(char* str) 
{
	return (UTCTime)NewGSTR(str);
}
GeneralizedType 
NewGeneralizedType(char* str) 
{
	return (GeneralizedType)NewGSTR(str);
}
GeneralizedTime 
NewGeneralizedTime(char* str) 
{
	return (GeneralizedTime)NewGSTR(str);
}
VisibleString 
NewVisibleString(char* str) 
{
	return (VisibleString)NewGSTR(str);
}
InternationalString 
NewInternationalString(char* str) 
{
	return (InternationalString)NewGSTR(str);
}
GeneralString 
NewGeneralString(char* str) 
{
	return (GeneralString)NewGSTR(str);
}


char *
GetGSTR(gstr)
GSTR	*gstr;
{
	char *buf;
    	if (gstr == NULL) {
		return (char *)NULL;
    	}
	if ((buf = (char *)calloc(1, gstr->length+1)) == NULL)
		return (char *)NULL;
    	strncpy(buf, gstr->data, gstr->length);
    	buf[gstr->length] = '\0';
	return buf;
}

char *
GetOctetString(octetString)
OctetString	octetString;
{
	return GetGSTR((GSTR *)octetString);
}

char *
GetBitString(bitString)
BitString	bitString;
{
	return GetGSTR((GSTR *)bitString);
}

char *
GetOidStr(object_id)
ObjectIdentifier	object_id;
{
	return GetGSTR((GSTR *)object_id);
}

char *
GetInternationalString(istring)
InternationalString   istring;
{
	return GetGSTR((GSTR *)istring);
}

int
CopyGSTR(buffer, gstr)
char	*buffer;	/* SPACE must be alloced outside of this routine. */
GSTR	*gstr;
{
    	if (buffer == NULL)
		return -1;
    	if (gstr == NULL) {
		*buffer = '\0';
		return 0;
    	}
    	strncpy(buffer, gstr->data, gstr->length);
    	buffer[gstr->length] = '\0';
    	return gstr->length;
}

/*
 * Return -1 to indicate an error has occurred.
 */
int
CopyOctetString(buffer, octetString)
char	*buffer;
OctetString	octetString;
{
	return CopyGSTR(buffer, (GSTR *)octetString);
}

int
CopyBitString(buffer, bitString)
char	*buffer;
OctetString	bitString;
{
	return CopyGSTR(buffer, (GSTR *)bitString);
}

int
CopyOidStr(buffer, object_id)
char	*buffer;
ObjectIdentifier	object_id;
{
	return CopyGSTR(buffer, (GSTR *)object_id);
}

void 
FreeGSTR (gstr)
GSTR	*gstr;
{
 	if (gstr == NULL)
		return;
	if (gstr->data != NULL)
		(void) free ((char *)gstr->data);
	(void) free ((char *)gstr);
}

void 
FreeOctetString (ostr)
OctetString ostr;
{
	FreeGSTR((GSTR *)ostr);
}
void 
FreeBitString (bstr)
BitString bstr;
{
	FreeGSTR((GSTR *)bstr);
}
void 
FreeInternationalString (istr)
InternationalString istr;
{
	FreeGSTR((GSTR *)istr);
}
void 
FreeOid (ostr)
ObjectIdentifier ostr;
{
	FreeGSTR((GSTR *)ostr);
}
void 
FreeObjectDescriptor (ostr)
ObjectDescriptor ostr;
{
	FreeGSTR((GSTR *)ostr);
}
void 
FreeNumericString (nstr)
NumericString nstr;
{
	FreeGSTR((GSTR *)nstr);
}
void 
FreePrintableString (pstr)
PrintableString pstr;
{
	FreeGSTR((GSTR *)pstr);
}
void 
FreeIA5String (istr)
IA5String istr;
{
	FreeGSTR((GSTR *)istr);
}
void 
FreeUTCTime (ustr)
UTCTime ustr;
{
	FreeGSTR((GSTR *)ustr);
}
void 
FreeGeneralizedType (gstr)
GeneralizedType gstr;
{
	FreeGSTR((GSTR *)gstr);
}
void 
FreeVisibleString (vstr)
VisibleString vstr;
{
	FreeGSTR((GSTR *)vstr);
}
void 
FreeGeneralString (gstr)
GeneralString gstr;
{
	FreeGSTR((GSTR *)gstr);
}


void FreeAny (any *an)
{
    	/* To be completed.	*/
}

/*
 * 	Convert the dot-notation of an object id (e.g. 1.2.840.10003.5.10) into
 *	an array of integers.
 *	return NULL if an error has occurred.
 */
struct internal_oid *
oid_convert(oidstr)
char	*oidstr;
{
    	char *cptr, *nptr;
    	char number[50];
    	struct internal_oid  *in_oid;

    	if (oidstr == NULL)
		return NULL;
    	in_oid = CALLOC(struct internal_oid,1);
	if (in_oid == NULL)
		return NULL;
    	in_oid->counts = 0;
    	for (cptr=oidstr; *cptr != '\0'; ) {
		while (!isdigit(*cptr) && (*cptr++ != '\0'))
	    		;
        	memset(number, '\0', sizeof(number));
        	nptr = number;
		while ((*cptr != '\0') && isdigit(*cptr))
	    		*nptr++ = *cptr++;
		in_oid->ids[in_oid->counts] = atoi(number); 
		in_oid->counts++;
    	}
    	return in_oid;
}

void
PrintInternalOID(the_oid)
struct internal_oid *the_oid;
{
    	int i;
    	if (the_oid == NULL)
		return;
    	for (i=0; i<the_oid->counts; i++)
		printf("%d ", the_oid->ids[i]);
    	printf("\n");
}

GSTR *
DupGSTR(gstr)
GSTR	*gstr;
{
    	GSTR *tmp;
    	if (gstr == NULL)
		return (GSTR *)NULL;
    	tmp = CALLOC(GSTR,1);
	if (tmp == NULL)
		return (GSTR *)NULL;
    	tmp->length = gstr->length;
    	tmp->data = (UCHAR *)calloc(1, tmp->length+1);
	if (tmp->data == NULL)
		return (GSTR *)NULL;
    	strncpy(tmp->data, gstr->data, tmp->length);
    	tmp->data[tmp->length] = '\0';
    	return tmp;
}


OctetString
DupOstring(ostr)
OctetString	ostr;
{
	return (OctetString)DupGSTR((GSTR *)ostr);
}

BitString
DupBstring(bstr)
BitString	bstr;
{
	return (BitString)DupGSTR((GSTR *)bstr);
}

ObjectIdentifier
DupOID(id)
ObjectIdentifier	id;
{
	return (ObjectIdentifier)DupGSTR((GSTR *)id);
}

InternationalString
DupInternationalString(is)
InternationalString is;
{
	return (InternationalString)DupGSTR((GSTR *)is);
}


/* the following items are for handling non-MARC ASN1-defined records */

any
DupASN1type(DOCTYPE doctype, any data) 
{
  
  if (data == NULL)
    return (NULL);

  switch (doctype) {
  case Z_OPAC_REC:             /* OPAC record syntax     */
    return ((any)DupOPACRecord((OPACRecord *)data));
    break;
  case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
    return ((any *)DupExplainRecord((Explain_Record *)data));
    break;
  case Z_GRS_1_REC:            /* Generic record syntax 1*/
    return ((any *)DupGRS1Record((GenericRecord *)data));
    break;
  case Z_ES_REC:               /* Extended Services      */
    return ((any *)DupESRecord((TaskPackage *)data));
    break;
  case Z_SUMMARY_REC:          /* Summary record syntax  */
    return ((any *)DupSummaryRecord((BriefBib *)data));
    break;
  default:
    fprintf(stderr,"UNKNOWN OR UNSUPPORTED ASN1 FORMAT");
    return (NULL);
  }


}

DOCTYPE
GetTypeExternal (ex)
     External *ex;
{
  char *OIDstring, *GetOidStr();
  DOCTYPE result;
  
  if (ex == NULL)
    return ((DOCTYPE)0);
  
  if (ex->directReference) {
    OIDstring = GetOidStr(ex->directReference);
    if (OIDstring == NULL)
      return ((DOCTYPE)0);
    
    if (strcmp(OIDstring, MARCRECSYNTAX)==0)
      result = Z_US_MARC;		/* USMARC record	  */
    else if (strcmp(OIDstring,OPACRECSYNTAX)==0)
      result = Z_OPAC_REC;             /* OPAC record syntax     */
    else if (strcmp(OIDstring,SUTRECSYNTAX)==0)
      result = Z_SUTRS_REC;            /* SUTRS record syntax    */
    else if (strcmp(OIDstring,SGML_RECSYNTAX)==0)
      result = Z_SGML_REC;            /* SGML record syntax    */
    else if (strcmp(OIDstring,XML_RECSYNTAX)==0)
      result = Z_XML_REC;            /* XML record syntax    */
    else if (strcmp(OIDstring,EXPLAINRECSYNTAX)==0)
      result = Z_EXPLAIN_REC;          /* EXPLAIN record syntax  */
    else if (strcmp(OIDstring,GRS0RECSYNTAX)==0)
      result = Z_GRS_0_REC;            /* Generic record syntax 0*/
    else if (strcmp(OIDstring,GRS1RECSYNTAX)==0)
      result = Z_GRS_1_REC;            /* Generic record syntax 1*/
    else if (strcmp(OIDstring,ESRECSYNTAX)==0)
      result = Z_ES_REC;               /* Extended Services      */
    else if (strcmp(OIDstring,SUMMARYRECSYNTAX)==0)
      result = Z_SUMMARY_REC;          /* Summary record syntax  */
    /* there may be other formats, but not in the standard ...    */
    else
      result = Z_OTHER_MARC;		/* Other MARC formats */
    
    free(OIDstring);
    return result;
  }
  else 
    return (Z_US_MARC); /* default format */
}

char *
GetDataExternal (ex, doctype, data_size)
     External *ex;
     DOCTYPE doctype;
     int *data_size;
{
  int size;
  char *buf = NULL;
  InternationalString is;

  if (ex == NULL)
    return NULL;
  switch (ex->which) {
  case t_singleASN1type:
    if (ex->u.singleASN1type) {
      *data_size = -1; /* don't really know with these */
      /* the dup code should work -- but not really needed */
      switch (doctype) {
      case Z_OPAC_REC:             /* OPAC record syntax     */
	/* return ((char *)ex->u.singleASN1type); */
        return ((char *)DupOPACRecord(ex->u.singleASN1type));
	break;
      case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
	return ((char *)ex->u.singleASN1type);
	/* return ((char *)DupExplainRecord(ex->u.singleASN1type)); */
	break;
      case Z_SUTRS_REC:          /* SUTRS record syntax  */
	if (ex->u.singleASN1type) {
	  is = (InternationalString)ex->u.singleASN1type; 
	  size = is->length;
	  buf = CALLOC(char, size+1);
	  memmove(buf, is->data, size);
	  buf[size] = '\0';
	  *data_size = size;
	}
	/* return ((char *)DupExplainRecord(ex->u.singleASN1type)); */
	break;
      case Z_GRS_1_REC:            /* Generic record syntax 1*/
	return ((char *)ex->u.singleASN1type);
	/* return ((char *)DupGRS1Record(ex->u.singleASN1type)); */
	break;
      case Z_ES_REC:               /* Extended Services      */
	return ((char *)ex->u.singleASN1type);
	/* return ((char *)DupESRecord(ex->u.singleASN1type)); */
	break;
      case Z_SUMMARY_REC:          /* Summary record syntax  */
	return ((char *)ex->u.singleASN1type);
	/* return ((char *)DupSummaryRecord(ex->u.singleASN1type)); */
	break;
      default:
	return ("UNKNOWN OR UNSUPPORTED ASN1 FORMAT");
      }
    }   
    else
      break;  
  case t_octetAligned:
    if (ex->u.octetAligned) {
      size = ex->u.octetAligned->length;
      buf = (unsigned char *)calloc(1, size+1);
      memmove(buf, ex->u.octetAligned->data, size);
      buf[size] = '\0';
      *data_size = size;
    }
    break;
    
  case t_arbitrary:
    if (ex->u.arbitrary) {
      size = ex->u.arbitrary->length;
      buf = (unsigned char *)calloc(1, size+1);
      memmove(buf, ex->u.arbitrary->data, size);
      buf[size] = '\0';
      *data_size = size;
    }
    break;
    
  default:
    /* print an error message.	*/
    fprintf(stderr, "unable to determine type for external data.\n");
    return NULL;
    *data_size = 0;
    break;
  }
  return buf;
}



External *
DupExternal (ex)
External *ex;
{
    External *tmp;

    if (ex == NULL)
	return NULL;
    tmp = CALLOC(External,1);
    if (tmp == NULL)
	return NULL;
    tmp->directReference = DupOID (ex->directReference);
    tmp->which = ex->which;
    switch (ex->which) {
	case t_singleASN1type:
	    tmp->u.singleASN1type = DupASN1type (GetTypeExternal(ex),
						 ex->u.singleASN1type);
	    break;

	case t_octetAligned:
	    tmp->u.octetAligned = DupOstring (ex->u.octetAligned);
	    break;

	case t_arbitrary:
	    tmp->u.arbitrary = DupBstring (ex->u.arbitrary);
	    break;
	
	default:
	     /* print an error message.	*/
	    fprintf(stderr,"Unknown type in DupExternal\n");
	    break;
    }
    return tmp;
}

CircRecord *
DupCircRecord(CircRecord *in)
{
  CircRecord *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(CircRecord,1);

  if (new_var == NULL)
    return NULL;


  new_var->availableNow = in->availableNow;
  new_var->availablityDate = DupInternationalString(in->availablityDate);
  new_var->availableThru = DupInternationalString(in->availableThru);
  new_var->restrictions = DupInternationalString(in->restrictions);
  new_var->itemId = DupInternationalString(in->itemId);
  new_var->renewable = in->renewable;
  new_var->onHold = in->onHold;
  new_var->enumAndChron = DupInternationalString(in->enumAndChron);
  new_var->midspine = DupInternationalString(in->midspine);
  new_var->temporaryLocation = DupInternationalString(in->temporaryLocation);

  return (new_var);
}

Volume *
DupVolume(Volume *in)
{
  Volume *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(Volume,1);

  if (new_var == NULL)
    return NULL;

  
  new_var->enumeration = DupInternationalString(in->enumeration);
  new_var->chronology = DupInternationalString(in->chronology);
  new_var->enumAndChron = DupInternationalString(in->enumAndChron);

  return (new_var);

}


HoldingsAndCircData *
DupHoldingsAndCirc(HoldingsAndCircData *in)
{
  HoldingsAndCircData *new_var;
  struct volumes_List88	*newv, *lastv, *currv;
  struct circulationData_List89	*newc, *lastc, *currc;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(HoldingsAndCircData,1);

  if (new_var == NULL)
    return NULL;

  
  new_var->typeOfRecord = DupInternationalString(in->typeOfRecord);
  new_var->encodingLevel = DupInternationalString(in->encodingLevel);
  new_var->format = DupInternationalString(in->format);
  new_var->receiptAcqStatus = DupInternationalString(in->receiptAcqStatus);
  new_var->generalRetention = DupInternationalString(in->generalRetention);
  new_var->completeness = DupInternationalString(in->completeness);
  new_var->dateOfReport = DupInternationalString(in->dateOfReport);
  new_var->nucCode = DupInternationalString(in->nucCode);
  new_var->localLocation = DupInternationalString(in->localLocation);
  new_var->shelvingLocation = DupInternationalString(in->shelvingLocation);
  new_var->callNumber = DupInternationalString(in->callNumber);
  new_var->shelvingData = DupInternationalString(in->shelvingData);
  new_var->copyNumber = DupInternationalString(in->copyNumber);
  new_var->publicNote = DupInternationalString(in->publicNote);
  new_var->reproductionNote = DupInternationalString(in->reproductionNote);
  new_var->termsUseRepro = DupInternationalString(in->termsUseRepro);
  new_var->enumAndChron = DupInternationalString(in->enumAndChron);


  lastv = NULL;  

  for (currv = in->volumes; currv != NULL; currv = currv->next) {
    /* create new_var list entry */
    newv = CALLOC(struct volumes_List88,1);

    if (newv == NULL)
      return NULL;

    /* link it in */
    if (lastv == NULL) 
      new_var->volumes = lastv = newv;
    else
      lastv->next = newv;
    
    newv->item = DupVolume(currv->item);    
    lastv = newv;
  }

  lastc = NULL;  

  for (currc = in->circulationData; currc != NULL; currc = currc->next) {
    /* create new_var list entry */
    newc = CALLOC(struct circulationData_List89,1);

    if (newc == NULL)
      return NULL;

    /* link it in */
    if (lastc == NULL) 
      new_var->circulationData = lastc = newc;
    else
      lastc->next = newc;
    
    newc->item = DupCircRecord(currc->item);    
    lastc = newc;
  }

  return (new_var);

}

HoldingsRecord *
DupHoldingsRecord(HoldingsRecord *in_rec)
{
  HoldingsRecord *new_var;

  if (in_rec == NULL)
    return NULL;

  new_var = CALLOC(HoldingsRecord,1);

  if (new_var == NULL)
    return NULL;

  new_var->which = in_rec->which;
  if (in_rec->which == e45_marcHoldingsRecord)
    new_var->u.marcHoldingsRecord = DupExternal(in_rec->u.marcHoldingsRecord);
  else
    new_var->u.holdingsAndCirc = DupHoldingsAndCirc(in_rec->u.holdingsAndCirc);

  return (new_var);

}

OPACRecord *
DupOPACRecord(OPACRecord *in_opac)
{
  OPACRecord *oPACRecord;
  struct holdingsData_List87 *last, *new_var, *in;

  if (in_opac == NULL)
    return NULL;

  oPACRecord = CALLOC(OPACRecord,1);

  if (oPACRecord == NULL)
    return NULL;
 
  oPACRecord->bibliographicRecord = DupExternal(in_opac->bibliographicRecord);
 
  last = NULL;  

  for (in = in_opac->holdingsData; in != NULL; in = in->next) {
    /* create new_var list entry */
    new_var = CALLOC(struct holdingsData_List87,1);

    if (new_var == NULL)
    return NULL;
 
    /* link it in */
    if (last == NULL) 
      oPACRecord->holdingsData = last = new_var;
    else
      last->next = new_var;
    
    new_var->item = DupHoldingsRecord(in->item);
    last = new_var;
    
  }
  
  return (oPACRecord);

}



StringOrNumeric *
DupStringOrNumeric(StringOrNumeric *in)
{
  StringOrNumeric *new_var;
  
  if (in == NULL)
    return NULL;

  new_var = CALLOC(StringOrNumeric,1);


  if (new_var == NULL)
    return NULL;
 
  new_var->which = in->which;

  if(in->which ==  e25_string) 
    new_var->u.string = DupInternationalString(in->u.string);
  else
    new_var->u.numeric = in->u.numeric;
  
  return(new_var);
}

StringOrNumeric *
MakeStringOrNumeric(char *string, int number, int which)
{
  StringOrNumeric *out;
  
  out = CALLOC(StringOrNumeric,1);

  if (out == NULL)
    return NULL;

  if (which == 0) {
    out->which = e25_string;
    out->u.string = NewInternationalString(string);
  }
  else {
    out->which = e25_numeric;
    out->u.numeric = number;
  }
  return(out);
}



Unit*
DupUnit(Unit *in)
{
  Unit *new_var;
  
  if (in == NULL)
    return NULL;

  new_var = CALLOC(Unit,1);

  if (new_var == NULL)
    return NULL;
 
  
  new_var->unitSystem = DupInternationalString(in->unitSystem);
  new_var->unitType = DupStringOrNumeric(in->unitType);
  new_var->unit = DupStringOrNumeric(in->unit);
  new_var->scaleFactor = in->scaleFactor;
  
  return (new_var);
}

IntUnit *
DupIntUnit(IntUnit *in)
{
  IntUnit *new_var;
  
  if (in == NULL)
    return NULL;

  new_var = CALLOC(IntUnit,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->value = in->value;
  new_var->unitUsed = DupUnit(in->unitUsed);
  return (new_var);

}

ElementData *
DupElementData(ElementData *in)
{
  ElementData *new_var;
  struct subtree_List91 *lastl, *newl, *inl;
  
  if (in == NULL)
    return NULL;

  new_var = CALLOC(ElementData,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->which = in->which;

  switch(in->which) {
    
  case e46_octets:
    new_var->u.octets = DupOstring(in->u.octets);
    break;
  case e46_numeric:
    new_var->u.numeric = in->u.numeric;
    break;
  case e46_date:
    new_var->u.date = (GeneralizedTime)DupOstring((OctetString)in->u.date);
    break;
  case e46_ext:
    new_var->u.ext = DupExternal(in->u.ext);
    break;
  case e46_string:
    new_var->u.string = DupInternationalString(in->u.string);
    break;
  case e46_trueOrFalse:
    new_var->u.trueOrFalse = in->u.trueOrFalse;
    break;
  case e46_oid:
    new_var->u.oid = DupOID(in->u.oid);
    break;
  case e46_intUnit:
    new_var->u.intUnit = DupIntUnit(in->u.intUnit);
    break;
  case e46_elementNotThere:
    new_var->u.elementNotThere = in->u.elementNotThere;
    break;
  case e46_elementEmpty:
    new_var->u.elementEmpty = in->u.elementEmpty;
    break;
  case e46_noDataRequested:
    new_var->u.noDataRequested = in->u.noDataRequested;
    break;
  case e46_diagnostic:
    new_var->u.diagnostic = DupExternal(in->u.diagnostic);
    break;
  case e46_subtree:
    lastl = NULL;  
    for (inl = in->u.subtree; inl != NULL; inl = inl->next) {
      /* create new_var list entry */
      newl = CALLOC(struct subtree_List91,1);

      if (newl == NULL)
	return NULL;
 
      /* link it in */
      if (lastl == NULL) 
	new_var->u.subtree = lastl = newl;
      else
	lastl->next = newl;
      newl->item = DupTaggedElement(inl->item);
      lastl = newl;
    }
    break;
  }

  return (new_var);
}


Variant *
DupVariant(Variant *in)
{
  Variant *new_var;
  struct triples_List94	*lastl, *newl, *i;;
  if (in == NULL)
    return NULL;

  new_var = CALLOC(Variant,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->globalVariantSetId = DupOID(in->globalVariantSetId);


  lastl = NULL;  

  for (i = in->triples; i != NULL; i = i->next) {
    /* create new_var list entry */
    newl = CALLOC(struct triples_List94,1);

    if (newl == NULL)
      return NULL;
 
    /* link it in */
    if (lastl == NULL) 
      new_var->triples = lastl = newl;
    else
      lastl->next = newl;
    
    newl->item.variantSetId = DupOID(i->item.variantSetId);
    newl->item.class_var = i->item.class_var;
    newl->item.type = i->item.type;
    newl->item.value.which = i->item.value.which;
    
    switch(i->item.value.which) {
    case e47_intVal:
      newl->item.value.u.intVal = i->item.value.u.intVal;
      break;
    case e47_interStringVal:
      newl->item.value.u.interStringVal = 
	DupInternationalString(i->item.value.u.interStringVal);
      break;
    case e47_octStringVal:
      newl->item.value.u.octStringVal = 
	DupOstring(i->item.value.u.octStringVal);
      break;
    case e47_oidVal:
      newl->item.value.u.oidVal = DupOID(i->item.value.u.oidVal);
      break;
    case e47_boolVal:
      newl->item.value.u.boolVal = i->item.value.u.boolVal;
      break;
    case e47_nullVal:
      newl->item.value.u.nullVal = i->item.value.u.nullVal;
      break;
    case e47_unit:
      newl->item.value.u.unit = DupUnit(i->item.value.u.unit);
      break;
    case e47_valueAndUnit:
      newl->item.value.u.valueAndUnit = 
	DupIntUnit(i->item.value.u.valueAndUnit);
      break;
    }
    lastl = newl;

  }
  return (new_var);
}



TaggedElement *
DupTaggedElement(TaggedElement *in)
{
  TaggedElement *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TaggedElement,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->tagType = in->tagType;

  new_var->tagValue = DupStringOrNumeric(in->tagValue);
  new_var->tagOccurrence = in->tagOccurrence;
  new_var->content = DupElementData(in->content);
  new_var->metaData = DupElementMetaData(in->metaData);
  new_var->appliedVariant = DupVariant(in->appliedVariant);
  
  return(new_var);
}


TagPath *
DupTagPath(TagPath *in) 
{
  TagPath *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TagPath,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->next = DupTagPath(in->next);
  new_var->item.tagType = in->item.tagType;
  new_var->item.tagValue = DupStringOrNumeric(in->item.tagValue);
  new_var->item.tagOccurrence = in->item.tagOccurrence;

  return (new_var);
}

Order *
DupOrder(Order *in) 
{
  Order *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Order,1);

  if (new_var == NULL)
    return NULL;
 
  
  new_var->ascending = in->ascending;
  new_var->order = in->order;
  return(new_var);
}

Usage *
DupUsage(Usage *in) 
{
  Usage *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Usage,1);

  if (new_var == NULL)
    return NULL;
 
  
  new_var->type = in->type;
  new_var->restriction = DupInternationalString(in->restriction);
  return(new_var);

}

Term *
DupTerm(Term *in)
{
  Term *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Term,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->which = in->which;
  switch(in->which) {

  case e5_general:
    new_var->u.general = DupOstring(in->u.general);
    break;
  case e5_numeric:
    new_var->u.numeric = in->u.numeric;
    break;
  case e5_characterString:
    new_var->u.characterString = DupInternationalString(in->u.characterString);
    break;
  case e5_oid:
    new_var->u.oid = DupOID(in->u.oid);
    break;
  case e5_dateTime:
    new_var->u.dateTime = 
      (GeneralizedTime)DupOstring((OctetString)in->u.dateTime);
    break;
  case e5_external:
    new_var->u.external = DupExternal(in->u.external);
    break;
  case e5_integerAndUnit:
    new_var->u.integerAndUnit = DupIntUnit(in->u.integerAndUnit);
    break;
  case e5_nullVal:
    new_var->u.nullVal = in->u.nullVal;
    break;
  }

  return(new_var);

}

HitVector *
DupHitVector(HitVector *in) 
{
  HitVector *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(HitVector,1);

  if (new_var == NULL)
    return NULL;
 
  
  new_var->satisfier = DupTerm(in->satisfier);
  new_var->offsetIntoElement = DupIntUnit(in->offsetIntoElement);
  new_var->length = DupIntUnit(in->length);
  new_var->hitRank = in->hitRank;
  new_var->targetToken = DupOstring(in->targetToken);
  
  return(new_var);
}


ElementMetaData *
DupElementMetaData(ElementMetaData *in)
{
  ElementMetaData *new_var;
  struct hits_List92 *lastl, *newl, *i;
  struct supportedVariants_List93 *lastv, *newv, *iv;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ElementMetaData,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->seriesOrder = DupOrder(in->seriesOrder);
  new_var->usageRight = DupUsage(in->usageRight);

  lastl = NULL;

  for (i = in->hits; i != NULL; i = i->next) {
    /* create new_var list entry */
    newl = CALLOC(struct hits_List92,1);

    if (newl == NULL)
      return NULL;
 
    /* link it in */
    if (lastl == NULL) 
      new_var->hits = lastl = newl;
    else
      lastl->next = newl;
    newl->item = DupHitVector(i->item);
    lastl = newl;
  }

  new_var->displayName = DupInternationalString(in->displayName);

  lastv = NULL;

  for (iv = in->supportedVariants; iv != NULL; iv = iv->next) {
    /* create new_var list entry */
    newv = CALLOC(struct supportedVariants_List93,1);

    if (newv == NULL)
      return NULL;
 
    /* link it in */
    if (lastv == NULL) 
      new_var->supportedVariants = lastv = newv;
    else
      lastv->next = newv;
    newv->item = DupVariant(iv->item);
    lastv = newv;
  }
  new_var->message = DupInternationalString(in->message);
  new_var->elementDescriptor = DupOstring(in->elementDescriptor);
  new_var->surrogateFor = DupTagPath(in->surrogateFor);
  new_var->surrogateElement = DupTagPath(in->surrogateElement);
  new_var->other = DupExternal(in->other);
  
  return(new_var);

}


GenericRecord * 
DupGRS1Record(GenericRecord *in)
{
  GenericRecord *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(GenericRecord,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->next = DupGRS1Record(in->next);

  new_var->item = DupTaggedElement(in->item);

  return(new_var);

}

InfoCategory *
DupInfoCategory(InfoCategory *in)
{
  InfoCategory *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(InfoCategory,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->categoryTypeId = DupOID(in->categoryTypeId);
  new_var->categoryValue = in->categoryValue;

  return (new_var);
}

OtherInformation *
DupOtherInformation(OtherInformation *in)
{
  OtherInformation *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(OtherInformation,1);

  if (new_var == NULL)
    return NULL;
 

  if (in->next != NULL) 
    new_var->next = DupOtherInformation(in->next);

  new_var->item.category = 
    DupInfoCategory(in->item.category);

  switch (in->item.information.which) {
  case e24_characterInfo:
    new_var->item.information.u.characterInfo = 
      DupInternationalString(in->item.information.u.characterInfo);
    break;
  case e24_binaryInfo:
    new_var->item.information.u.binaryInfo = 
      DupOstring(in->item.information.u.binaryInfo);
    break;
  case e24_externallyDefinedInfo:
    new_var->item.information.u.externallyDefinedInfo = 
      DupExternal(in->item.information.u.externallyDefinedInfo);
    break;
  case e24_oid:
    new_var->item.information.u.oid = DupOID(in->item.information.u.oid);
    break;
  }
  return (new_var);
}

FormatSpec *
DupFormatSpec(FormatSpec *in)
{
  FormatSpec *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(FormatSpec,1);

  if (new_var == NULL)
    return NULL;
 
  
  new_var->type = DupInternationalString(in->type);
  new_var->size = in->size;
  new_var->bestPosn = in->bestPosn;
 
  return (new_var);
}

BriefBib *
DupSummaryRecord(BriefBib *in)
{
  BriefBib *newbib;
  struct format_List90 *last, *new_var, *i;

  if (in == NULL)
    return NULL;

  newbib = CALLOC(BriefBib,1);

  if (newbib == NULL)
    return NULL;
 
  
  newbib->title = DupInternationalString(in->title);
  newbib->author = DupInternationalString(in->author);
  newbib->callNumber = DupInternationalString(in->callNumber);
  newbib->recordType = DupInternationalString(in->recordType);
  newbib->bibliographicLevel = 
    DupInternationalString(in->bibliographicLevel);
  newbib->publicationPlace = DupInternationalString(in->publicationPlace);
  newbib->publicationDate = DupInternationalString(in->publicationDate);
  newbib->targetSystemKey = DupInternationalString(in->targetSystemKey);
  newbib->satisfyingElement = DupInternationalString(in->satisfyingElement);
  newbib->rank = in->rank;
  newbib->documentId = DupInternationalString(in->documentId);
  newbib->abstract = DupInternationalString(in->abstract);
  newbib->otherInfo = DupOtherInformation(in->otherInfo);
  
 
  last = NULL;  
  
  for (i = in->format; i != NULL; i = i->next) {
    /* create new_var list entry */
    new_var = CALLOC(struct format_List90,1);

    if (new_var == NULL)
      return NULL;
 
    /* link it in */
    if (last == NULL) 
      newbib->format = last = new_var;
    else
      last->next = new_var;
    
    new_var->item = DupFormatSpec(i->item);
    last = new_var;

  }
  
  return (newbib);
}

LanguageCode *
DupLanguageCode(LanguageCode *in)
{
  return (LanguageCode *)DupGSTR((GSTR *)in);
}  

GeneralizedTime
DupGeneralizedTime(GeneralizedTime in)
{
  return (GeneralizedTime)DupGSTR((GSTR *)in);
}  

HumanString *
DupHumanString(HumanString *in)
{
  HumanString *new_var;

  if (in == NULL)
    return NULL;

  
  new_var = CALLOC(HumanString,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->next = DupHumanString(in->next);

  new_var->item.language = DupLanguageCode(in->item.language);
  new_var->item.text = DupInternationalString(in->item.text);

  return(new_var);
}

CommonInfo *
DupCommonInfo(CommonInfo *in)
{
  CommonInfo *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(CommonInfo,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->dateAdded = DupGeneralizedTime(in->dateAdded);
  new_var->dateChanged = DupGeneralizedTime(in->dateChanged);
  new_var->expiry = DupGeneralizedTime(in->expiry);
  new_var->humanString_Language = DupLanguageCode(in->humanString_Language);
  new_var->otherInfo = DupOtherInformation(in->otherInfo);


  return(new_var);
}

IconObject *
DupIconObject(IconObject *in)
{
  IconObject *new_var;
  
  if (in == NULL)
    return NULL;

  new_var = CALLOC(IconObject,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->next = DupIconObject(in->next);

  new_var->item.content = DupOstring(in->item.content);

  new_var->item.bodyType.which = in->item.bodyType.which;

  switch (in->item.bodyType.which) {
  case e40_ianaType:
    new_var->item.bodyType.u.ianaType = 
      DupInternationalString(in->item.bodyType.u.ianaType);
    break;
  case e40_z3950type:
    new_var->item.bodyType.u.z3950type = 
      DupInternationalString(in->item.bodyType.u.z3950type);
    break;
  case e40_otherType:
    new_var->item.bodyType.u.otherType = 
      DupInternationalString(in->item.bodyType.u.otherType);
    break;
  }

  return(new_var);
}


ContactInfo *
DupContactInfo(ContactInfo *in)
{
  ContactInfo *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ContactInfo,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->address = DupHumanString(in->address);
  new_var->email = DupInternationalString(in->email);
  new_var->phone = DupInternationalString(in->phone);

  return(new_var);

}

DatabaseList *
DupDatabaseList(DatabaseList *in)
{
  DatabaseList *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(DatabaseList,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->next = DupDatabaseList(in->next);

  new_var->item = (DatabaseName)DupGSTR((GSTR *)in->item);

  return(new_var);
}

NetworkAddress *
DupNetworkAddress(NetworkAddress *in)
{
  NetworkAddress *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(NetworkAddress,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->which = in->which;

  switch(in->which) {
  case e41_internetAddress:
    new_var->u.internetAddress.hostAddress = 
      DupInternationalString(in->u.internetAddress.hostAddress);
    new_var->u.internetAddress.port = in->u.internetAddress.port;
    break;
  case e41_osiPresentationAddress:
    new_var->u.osiPresentationAddress.pSel = 
      DupInternationalString(in->u.osiPresentationAddress.pSel);
    new_var->u.osiPresentationAddress.sSel = 
      DupInternationalString(in->u.osiPresentationAddress.sSel);
    new_var->u.osiPresentationAddress.tSel = 
      DupInternationalString(in->u.osiPresentationAddress.tSel);
    new_var->u.osiPresentationAddress.nSap = 
      DupInternationalString(in->u.osiPresentationAddress.nSap);
    break;
  case e41_other:
    new_var->u.other.type = DupInternationalString(in->u.other.type);
    new_var->u.other.address = DupInternationalString(in->u.other.address);
    break;
    
  }
  
  return (new_var);

}

AccessRestrictions *
DupAccessRestrictions(AccessRestrictions *in)
{
  AccessRestrictions *new_var;
  struct accessChallenges_List83 *lastac, *newac, *iac;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(AccessRestrictions,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->next = DupAccessRestrictions(in->next);


  new_var->item.accessType = in->item.accessType;
  new_var->item.accessText = DupHumanString(in->item.accessText);

  lastac = NULL;  

  for (iac = in->item.accessChallenges; iac != NULL; iac = iac->next) {
    /* create new_var list entry */
    newac = CALLOC(struct accessChallenges_List83,1);
    /* link it in */
    if (lastac == NULL) 
      new_var->item.accessChallenges = lastac = newac;
    else
      lastac->next = newac;
    
    newac->item = DupOID(iac->item);
    lastac = newac;

  }

  return(new_var);

}


SearchKey *
DupSearchKey(SearchKey *in)
{
  SearchKey *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(SearchKey,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->searchKey = DupInternationalString(in->searchKey);
  new_var->description = DupHumanString(in->description);

  return(new_var);
}


PrivateCapabilities *
DupPrivateCapabilities(PrivateCapabilities *in)
{
  PrivateCapabilities *new_var;
  struct operators_List77 *lastop, *newop, *iop;
  struct searchKeys_List78 *lastsk, *newsk, *isk;
  struct description_List79 *lastds, *newds, *ids;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(PrivateCapabilities,1);

  if (new_var == NULL)
    return NULL;
 

  lastop = NULL;  

  for (iop = in->operators; iop != NULL; iop = iop->next) {
    /* create new_var list entry */
    newop = CALLOC(struct operators_List77,1);

    if (newop == NULL)
    return NULL;
 
    /* link it in */
    if (lastop == NULL) 
      new_var->operators = lastop = newop;
    else
      lastop->next = newop;
    
    newop->item.operator_var = DupInternationalString(iop->item.operator_var);
    newop->item.description = DupHumanString(iop->item.description);
    lastop = newop;
  }

  lastsk = NULL;  

  for (isk = in->searchKeys; isk != NULL; isk = isk->next) {
    /* create new_var list entry */
    newsk = CALLOC(struct searchKeys_List78,1);

    if (newsk == NULL)
      return NULL;
 
    /* link it in */
    if (lastsk == NULL) 
      new_var->searchKeys = lastsk = newsk;
    else
      lastsk->next = newsk;
    
    newsk->item = DupSearchKey(isk->item);
    lastsk = newsk;
    
  }

  lastds = NULL;  

  for (ids = in->description; ids != NULL; ids = ids->next) {
    /* create new_var list entry */
    newds = CALLOC(struct description_List79,1);

    if (newds == NULL)
      return NULL;
 
    /* link it in */
    if (lastds == NULL) 
      new_var->description = lastds = newds;
    else
      lastds->next = newds;
    
    newds->item = DupHumanString(ids->item);
    lastds = newds;

  }

  return (new_var);
}

ProximitySupport *
DupProximitySupport(ProximitySupport *in)
{
  ProximitySupport *new_var;
  struct unitsSupported_List82 *lastus, *newus, *ius;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(ProximitySupport,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->anySupport = in->anySupport;

  lastus = NULL;  

  for (ius = in->unitsSupported; ius != NULL; ius = ius->next) {
    /* create new_var list entry */
    newus = CALLOC(struct unitsSupported_List82,1);

    if (newus == NULL)
      return NULL;
 
    /* link it in */
    if (lastus == NULL) 
      new_var->unitsSupported = lastus = newus;
    else
      lastus->next = newus;
    
    if (ius->item != NULL) {
      newus->item = CALLOC(struct unitstypes,1);
      newus->item->which = ius->item->which;
      if (ius->item->which == e43_known)
      newus->item->u.known = ius->item->u.known;
      else {
	newus->item->u.private_var.unit = ius->item->u.private_var.unit;
	newus->item->u.private_var.description = 
	  DupHumanString(ius->item->u.private_var.description);
      }
    }
    lastus = newus;
  }

  return (new_var);

};


RpnCapabilities *
DupRpnCapabilities(RpnCapabilities *in)
{
  RpnCapabilities *new_var;
  struct operators_List80 *lastop, *newop, *iop;;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(RpnCapabilities,1);

  if (new_var == NULL)
    return NULL;
 

  lastop = NULL;  

  for (iop = in->operators; iop != NULL; iop = iop->next) {
    /* create new_var list entry */
    newop = CALLOC(struct operators_List80,1);

    if (newop == NULL)
      return NULL;
 
    /* link it in */
    if (lastop == NULL) 
      new_var->operators = lastop = newop;
    else
      lastop->next = newop;
    
    newop->item = iop->item;
    lastop = newop;
  }

  new_var->resultSetAsOperandSupported = in->resultSetAsOperandSupported;
  new_var->restrictionOperandSupported = in->restrictionOperandSupported;
  new_var->proximity = DupProximitySupport(in->proximity);

  return(new_var);
}

Iso8777Capabilities *
DupIso8777Capabilities(Iso8777Capabilities *in)
{
  Iso8777Capabilities *new_var;
  struct searchKeys_List81 *lastsk, *newsk, *isk;;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(Iso8777Capabilities,1);

  if (new_var == NULL)
    return NULL;
 

  lastsk = NULL;  

  for (isk = in->searchKeys; isk != NULL; isk = isk->next) {
    /* create new_var list entry */
    newsk = CALLOC(struct searchKeys_List81,1);

    if (newsk == NULL)
      return NULL;
 
    /* link it in */
    if (lastsk == NULL) 
      new_var->searchKeys = lastsk = newsk;
    else
      lastsk->next = newsk;
    
    newsk->item = DupSearchKey(isk->item);
    lastsk = newsk;
  }

  new_var->restrictions = DupHumanString(in->restrictions);

  return (new_var);
}


QueryTypeDetails *
DupQueryTypeDetails(QueryTypeDetails *in)
{
  QueryTypeDetails *new_var;
  
  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(QueryTypeDetails,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->which = in->which;

  switch (in->which) {
  case e42_private:
    new_var->u.private_var = DupPrivateCapabilities(in->u.private_var);
    break;
  case e42_rpn:
    new_var->u.rpn = DupRpnCapabilities(in->u.rpn);
    break;
  case e42_iso8777:
    new_var->u.iso8777 = DupIso8777Capabilities(in->u.iso8777);
    break;
  case e42_z39_58:
    new_var->u.z39_58 = DupHumanString(in->u.z39_58);
    break;
  case e42_erpn:
    new_var->u.erpn = DupRpnCapabilities(in->u.erpn);
    break;
  case e42_rankedList:
    new_var->u.rankedList = DupHumanString(in->u.rankedList);
    break;
  }

  return(new_var);
}

Charge *
DupCharge(Charge *in)
{
  Charge *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(Charge,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->cost = DupIntUnit(in->cost);
  new_var->perWhat = DupUnit(in->perWhat);
  new_var->text = DupHumanString(in->text);

  return(new_var);
}


Costs *
DupCosts(Costs *in)
{
  Costs *new_var;
  struct otherCharges_List84 *lastoc, *newoc, *ioc;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(Costs,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->connectCharge = DupCharge(in->connectCharge);
  new_var->connectTime = DupCharge(in->connectTime);
  new_var->displayCharge = DupCharge(in->displayCharge);
  new_var->searchCharge = DupCharge(in->searchCharge);
  new_var->subscriptCharge = DupCharge(in->subscriptCharge);
  
  lastoc = NULL;  
  
  for (ioc = in->otherCharges; ioc != NULL; ioc = ioc->next) {
    /* create new_var list entry */
    newoc = CALLOC(struct otherCharges_List84,1);

    if (newoc == NULL)
      return NULL;
 
    /* link it in */
    if (lastoc == NULL) 
      new_var->otherCharges = lastoc = newoc;
    else
      lastoc->next = newoc;
    
    newoc->item.forWhat = DupHumanString(ioc->item.forWhat);
    newoc->item.charge = DupCharge(ioc->item.charge);
    lastoc = newoc;
  }
  return (new_var);
}


AccessInfo *
DupAccessInfo(AccessInfo *in)
{
  AccessInfo *new_var;
  struct queryTypesSupported_List68 *lastqt, *newqt, *iqt;
  struct diagnosticsSets_List69 *lastds, *newds, *ids;
  struct attributeSetIds_List70 *lastas, *newas, *ias;
  struct schemas_List71 *lastsc, *newsc, *isc;
  struct recordSyntaxes_List72 *lastrs, *newrs, *irs;
  struct resourceChallenges_List73 *lastrc, *newrc, *irc;
  struct variantSets_List74 *lastvs, *newvs, *ivs;
  struct elementSetNames_List75 *lastes, *newes, *ies;
  struct unitSystems_List76 *lastus, *newus, *ius;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(AccessInfo,1);

  if (new_var == NULL)
    return NULL;
 
  lastqt = NULL;  

  for (iqt = in->queryTypesSupported; iqt != NULL; iqt = iqt->next) {
    /* create new_var list entry */
    newqt = CALLOC(struct queryTypesSupported_List68,1);

    if (newqt == NULL)
      return NULL;
 
    /* link it in */
    if (lastqt == NULL) 
      new_var->queryTypesSupported = lastqt = newqt;
    else
      lastqt->next = newqt;
    
    newqt->item = DupQueryTypeDetails(iqt->item);
    lastqt = newqt;

  }

  lastds = NULL;  

  for (ids = in->diagnosticsSets; ids != NULL; ids = ids->next) {
    /* create new_var list entry */
    newds = CALLOC(struct diagnosticsSets_List69,1);

    if (newds == NULL)
      return NULL;
 
    /* link it in */
    if (lastds == NULL) 
      new_var->diagnosticsSets = lastds = newds;
    else
      lastds->next = newds;
    
    newds->item = DupOID(ids->item);
    lastds = newds;

  }

  lastas = NULL;  

  for (ias = in->attributeSetIds; ias != NULL; ias = ias->next) {
    /* create new_var list entry */
    newas = CALLOC(struct attributeSetIds_List70,1);

    if (newas == NULL)
      return NULL;
 
    /* link it in */
    if (lastas == NULL) 
      new_var->attributeSetIds = lastas = newas;
    else
      lastas->next = newas;
    
    newas->item = (AttributeSetId)DupOID((ObjectIdentifier)ias->item);
    lastas = newas;

  }

  lastsc = NULL;  

  for (isc = in->schemas; isc != NULL; isc = isc->next) {
    /* create new_var list entry */
    newsc = CALLOC(struct schemas_List71,1);

    if (newsc == NULL)
      return NULL;
 
    /* link it in */
    if (lastsc == NULL) 
      new_var->schemas = lastsc = newsc;
    else
      lastsc->next = newsc;
    
    newsc->item = DupOID(isc->item);
    lastsc = newsc;

  }

  lastrs = NULL;  

  for (irs = in->recordSyntaxes; irs != NULL; irs = irs->next) {
    /* create new_var list entry */
    newrs = CALLOC(struct recordSyntaxes_List72,1);

    if (newrs == NULL)
      return NULL;
 
    /* link it in */
    if (lastrs == NULL) 
      new_var->recordSyntaxes = lastrs = newrs;
    else
      lastrs->next = newrs;
    
    newrs->item = DupOID(irs->item);
    lastrs = newrs;

  }

  lastrc = NULL;  

  for (irc = in->resourceChallenges; irc != NULL; irc = irc->next) {
    /* create new_var list entry */
    newrc = CALLOC(struct resourceChallenges_List73,1);

    if (newrc == NULL)
      return NULL;
 
    /* link it in */
    if (lastrc == NULL) 
      new_var->resourceChallenges = lastrc = newrc;
    else
      lastrc->next = newrc;
    
    newrc->item = DupOID(irc->item);
    lastrc= newrc;

  }

  new_var->restrictedAccess = DupAccessRestrictions(in->restrictedAccess);

  new_var->costInfo = DupCosts(in->costInfo);


  lastvs = NULL;  

  for (ivs = in->variantSets; ivs != NULL; ivs = ivs->next) {
    /* create new_var list entry */
    newvs = CALLOC(struct variantSets_List74,1);

    if (newvs == NULL)
      return NULL;
 
    /* link it in */
    if (lastvs == NULL) 
      new_var->variantSets = lastvs = newvs;
    else
      lastvs->next = newvs;
    
    newvs->item = DupOID(ivs->item);
    lastvs = newvs;

  }

  lastes = NULL;  

  for (ies = in->elementSetNames; ies != NULL; ies = ies->next) {
    /* create new_var list entry */
    newes = CALLOC(struct elementSetNames_List75,1);

    if (newes == NULL)
      return NULL;
 
    /* link it in */
    if (lastes == NULL) 
      new_var->elementSetNames = lastes = newes;
    else
      lastes->next = newes;
    
    newes->item = (ElementSetName)
      DupInternationalString((InternationalString)ies->item);
    lastes = newes;

  }

  lastus = NULL;  

  for (ius = in->unitSystems; ius != NULL; ius = ius->next) {
    /* create new_var list entry */
    newus = CALLOC(struct unitSystems_List76,1);

    if (newus == NULL)
      return NULL;
 
    /* link it in */
    if (lastus == NULL) 
      new_var->unitSystems = lastus = newus;
    else
      lastus->next = newus;
    
    newus->item = DupInternationalString(ius->item);
    lastus = newus;

  }

  return(new_var);

}

TargetInfo *
DupTargetInfo(TargetInfo *in)
{
  TargetInfo *new_var;
  struct nicknames_List30 *lastnn, *newnn, *inn;
  struct dbCombinations_List31 *lastdb, *newdb, *idb;
  struct addresses_List32 *lastad, *newad, *iad;
  struct languages_List33 *lastln, *newln, *iln;
  

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TargetInfo,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->name = DupInternationalString(in->name);
  new_var->recent_news = DupHumanString(in->recent_news);
  new_var->icon = DupIconObject(in->icon);
  new_var->namedResultSets = in->namedResultSets;
  new_var->multipleDBsearch = in->multipleDBsearch;
  new_var->maxResultSets = in->maxResultSets;
  new_var->maxResultSize = in->maxResultSize;
  new_var->maxTerms = in->maxTerms;
  new_var->timeoutInterval = DupIntUnit(in->timeoutInterval);
  new_var->welcomeMessage = DupHumanString(in->welcomeMessage);
  new_var->contactInfo = DupContactInfo(in->contactInfo);
  new_var->description = DupHumanString(in->description);

  lastnn = NULL;  

  for (inn = in->nicknames; inn != NULL; inn = inn->next) {
    /* create new_var list entry */
    newnn = CALLOC(struct nicknames_List30,1);

    if (newnn == NULL)
      return NULL;
 
    /* link it in */
    if (lastnn == NULL) 
      new_var->nicknames = lastnn = newnn;
    else
      lastnn->next = newnn;
    
    newnn->item = DupInternationalString(inn->item);
    lastnn = newnn;

  }
  

  new_var->usage_restrictions = DupHumanString(in->usage_restrictions);
  new_var->paymentAddr = DupHumanString(in->paymentAddr);
  new_var->hours = DupHumanString(in->hours);
  
  lastdb = NULL;  
  
  for (idb = in->dbCombinations; idb != NULL; idb = idb->next) {
    /* create new_var list entry */
    newdb = CALLOC(struct dbCombinations_List31,1);

    if (newdb == NULL)
      return NULL;
 
    /* link it in */
    if (lastdb == NULL) 
      new_var->dbCombinations = lastdb = newdb;
    else
      lastdb->next = newdb;
    
    newdb->item = DupDatabaseList(idb->item);
    lastdb = newdb;

  }
  
  lastad = NULL;  

  for (iad = in->addresses; iad != NULL; iad = iad->next) {
    /* create new_var list entry */
    newad = CALLOC(struct addresses_List32,1);

    if (newad == NULL)
      return NULL;
 
    /* link it in */
    if (lastad == NULL) 
      new_var->addresses = lastad = newad;
    else
      lastad->next = newad;
    
    newad->item = DupNetworkAddress(iad->item);
    lastad = newad;

  }

  lastln = NULL;  

  for (iln = in->languages; iln != NULL; iln = iln->next) {
    /* create new_var list entry */
    newln = CALLOC(struct languages_List33,1);

    if (newln == NULL)
      return NULL;
 
    /* link it in */
    if (lastln == NULL) 
      new_var->languages = lastln = newln;
    else
      lastln->next = newln;
    
    newln->item = DupInternationalString(iln->item);
    lastln = newln;

  }
  
  new_var->commonAccessInfo = DupAccessInfo(in->commonAccessInfo);

  return(new_var);
}

DatabaseInfo *
DupDatabaseInfo(DatabaseInfo *in)
{
  DatabaseInfo *new_var;
  struct nicknames_List34 *lastnn, *newnn, *inn;
  struct keywords_List35 *lastkw, *newkw, *ikw;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(DatabaseInfo,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->name = (DatabaseName)DupGSTR((GSTR *)in->name);
  new_var->explainDatabase = in->explainDatabase;

  lastnn = NULL;  

  for (inn = in->nicknames; inn != NULL; inn = inn->next) {
    /* create new_var list entry */
    newnn = CALLOC(struct  nicknames_List34,1);

    if (newnn == NULL)
      return NULL;
 
    /* link it in */
    if (lastnn == NULL) 
      new_var->nicknames = lastnn = newnn;
    else
      lastnn->next = newnn;
    
    newnn->item = (DatabaseName)DupGSTR((GSTR *)inn->item);
    lastnn = newnn;

  }

  new_var->icon = DupIconObject(in->icon);
  new_var->user_fee = in->user_fee;
  new_var->available = in->available;
  new_var->titleString = DupHumanString(in->titleString);

  lastkw = NULL;  
  
  for (ikw = in->keywords; ikw != NULL; ikw = ikw->next) {
    /* create new_var list entry */
    newkw = CALLOC(struct keywords_List35,1);
    
    if (newkw == NULL)
      return NULL;
 
    /* link it in */
    if (lastkw == NULL) 
      new_var->keywords = lastkw = newkw;
    else
      lastkw->next = newkw;
    
    newkw->item = DupHumanString(ikw->item);
    lastkw = newkw;
    
  }
  
  new_var->description = DupHumanString(in->description);
  new_var->associatedDbs = DupDatabaseList(in->associatedDbs);
  new_var->subDbs = DupDatabaseList(in->subDbs);
  new_var->disclaimers = DupHumanString(in->disclaimers);
  new_var->news = DupHumanString(in->news);

  if (in->recordCount != NULL) {
    new_var->recordCount = CALLOC(struct recordCount,1);
    new_var->recordCount->which = in->recordCount->which;
    if (in->recordCount->which == e35_actualNumber)
      new_var->recordCount->u.actualNumber = in->recordCount->u.actualNumber;
    if (in->recordCount->which == e35_approxNumber)
    new_var->recordCount->u.approxNumber = in->recordCount->u.approxNumber;
  }
  new_var->defaultOrder = DupHumanString(in->defaultOrder);
  new_var->avRecordSize = in->avRecordSize;
  new_var->maxRecordSize = in->maxRecordSize;
  new_var->hours = DupHumanString(in->hours);
  new_var->bestTime = DupHumanString(in->bestTime);
  new_var->lastUpdate = DupGeneralizedTime(in->lastUpdate);
  new_var->updateInterval = DupIntUnit(in->updateInterval);
  new_var->coverage = DupHumanString(in->coverage);
  new_var->proprietary = in->proprietary;
  new_var->copyrightText = DupHumanString(in->copyrightText);
  new_var->copyrightNotice = DupHumanString(in->copyrightNotice);
  new_var->producerContactInfo = DupContactInfo(in->producerContactInfo);
  new_var->supplierContactInfo = DupContactInfo(in->supplierContactInfo);
  new_var->submissionContactInfo = DupContactInfo(in->submissionContactInfo);
  new_var->accessInfo = DupAccessInfo(in->accessInfo);

  return(new_var);
  
}


Path *
DupPath(Path *in)
{
  Path *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Path,1);
  
  if (new_var == NULL)
    return NULL;


  new_var->next = DupPath(in->next);

  new_var->item.tagType = in->item.tagType;
  new_var->item.tagValue = DupStringOrNumeric(in->item.tagValue);

  return(new_var);

}

ElementDataType *
DupElementDataType(ElementDataType *in)
{
  ElementDataType *new_var;
  struct structured_List38 *lastrs, *newrs, *irs;;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(ElementDataType,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->which = in->which;

  if (in->which == e36_primitive)
    new_var->u.primitive = in->u.primitive;
  else {
    lastrs = NULL;  
  
    for (irs = in->u.structured; irs != NULL; irs = irs->next) {
      /* create new_var list entry */
      newrs = CALLOC(struct  structured_List38,1);
      
      if (newrs == NULL)
	return NULL;
      
      /* link it in */
      if (lastrs == NULL) 
	new_var->u.structured = lastrs = newrs;
      else
	lastrs->next = newrs;
      
      newrs->item = DupElementInfo(irs->item);
      lastrs = newrs;
    
    }
  }

  return (new_var);
}


ElementInfo *
DupElementInfo(ElementInfo *in)
{
  ElementInfo *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ElementInfo,1);
  
  if (new_var == NULL)
    return NULL;


  new_var->elementName = DupInternationalString(in->elementName);
  new_var->elementTagPath = DupPath(in->elementTagPath);
  new_var->dataType = DupElementDataType(in->dataType);
  new_var->required = in->required;
  new_var->repeatable = in->repeatable;
  new_var->description = DupHumanString(in->description);

  return(new_var);
}


SchemaInfo *
DupSchemaInfo(SchemaInfo *in)
{
  SchemaInfo *new_var;
  struct tagTypeMapping_List36 *lasttt, *newtt, *itt;
  struct recordStructure_List37 *lastrs, *newrs, *irs;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(SchemaInfo,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->schema = DupOID(in->schema);
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);

  lasttt = NULL;  
  
  for (itt = in->tagTypeMapping; itt != NULL; itt = itt->next) {
    /* create new_var list entry */
    newtt = CALLOC(struct tagTypeMapping_List36,1);
    
    if (newtt == NULL)
      return NULL;
 
    /* link it in */
    if (lasttt == NULL) 
      new_var->tagTypeMapping = lasttt = newtt;
    else
      lasttt->next = newtt;
    
    newtt->item.tagType = itt->item.tagType;
    newtt->item.tagSet = DupOID(itt->item.tagSet);
    newtt->item.defaultTagType = itt->item.defaultTagType;
    lasttt = newtt;
    
  }

  lastrs = NULL;  
  
  for (irs = in->recordStructure; irs != NULL; irs = irs->next) {
    /* create new_var list entry */
    newrs = CALLOC(struct recordStructure_List37,1);
    
    if (newrs == NULL)
      return NULL;
 
    /* link it in */
    if (lastrs == NULL) 
      new_var->recordStructure = lastrs = newrs;
    else
      lastrs->next = newrs;
    
    newrs->item = DupElementInfo(irs->item);
    lastrs = newrs;
    
  }

  return(new_var);

}

TagSetInfo *
DupTagSetInfo(TagSetInfo *in)
{
  TagSetInfo *new_var;
  struct nicknames_List40 *lastnn, *newnn, *inn;
  struct elements_List39 *lastel, *newel, *iel;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TagSetInfo,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->tagSet = DupOID(in->tagSet);
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);


  lastel = NULL;  

  for (iel = in->elements; iel != NULL; iel = iel->next) {
    /* create new_var list entry */
    newel = CALLOC(struct elements_List39,1);

    if (newel == NULL)
      return NULL;
 
    /* link it in */
    if (lastel == NULL) 
      new_var->elements = lastel = newel;
    else
      lastel->next = newel;
    

    newel->item.elementname = DupInternationalString(iel->item.elementname);

    lastnn = NULL;  

    for (inn = iel->item.nicknames; inn != NULL; inn = inn->next) {
      /* create new_var list entry */
      newnn = CALLOC(struct nicknames_List40,1);

      if (newnn == NULL)
	return NULL;
      
      /* link it in */
      if (lastnn == NULL) 
	newel->item.nicknames = lastnn = newnn;
      else
	lastnn->next = newnn;
    
      newnn->item = DupInternationalString(inn->item);
      lastnn = newnn;
    }
    newel->item.elementTag = DupStringOrNumeric(iel->item.elementTag);
    newel->item.description = DupHumanString(iel->item.description);
    newel->item.dataType = iel->item.dataType;
    newel->item.otherTagInfo = DupOtherInformation(iel->item.otherTagInfo);

    lastel = newel;

  }

  return(new_var);
  
}

RecordSyntaxInfo *
DupRecordSyntaxInfo(RecordSyntaxInfo *in)
{
  RecordSyntaxInfo *new_var;
  struct transferSyntaxes_List41 *lastts, *newts, *its;
  struct abstractStructure_List42 *lastas, *newas, *ias;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(RecordSyntaxInfo,1);

  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->recordSyntax = DupOID(in->recordSyntax);
  new_var->name = DupInternationalString(in->name);

  lastts = NULL;  
  
  for (its = in->transferSyntaxes; its != NULL; its = its->next) {
    /* create new_var list entry */
    newts = CALLOC(struct transferSyntaxes_List41,1);
    
    if (newts == NULL)
      return NULL;
 
    /* link it in */
    if (lastts == NULL) 
      new_var->transferSyntaxes = lastts = newts;
    else
      lastts->next = newts;
    
    newts->item = DupOID(its->item);
    lastts = newts;
    
  }

  new_var->description = DupHumanString(in->description);
  new_var->asn1Module = DupInternationalString(in->asn1Module);

  lastas = NULL;  
  
  for (ias = in->abstractStructure; ias != NULL; ias = ias->next) {
    /* create new_var list entry */
    newas = CALLOC(struct abstractStructure_List42,1);
    
    if (newas == NULL)
      return NULL;
 
    /* link it in */
    if (lastas == NULL) 
      new_var->abstractStructure = lastas = newas;
    else
      lastas->next = newas;
    
    newas->item = DupElementInfo(ias->item);
    lastas = newas;
  }

  return(new_var);

}


AttributeDescription *
DupAttributeDescription(AttributeDescription *in)
{
  AttributeDescription *new_var;
  struct equivalentAttributes_List45 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeDescription,1);

  if (new_var == NULL)
    return NULL;

  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->attributeValue = DupStringOrNumeric(in->attributeValue);

  lastat = NULL;  
  
  for (iat = in->equivalentAttributes; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct equivalentAttributes_List45,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->equivalentAttributes = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupStringOrNumeric(iat->item);
    lastat = newat;
  }
  return (new_var);

}


AttributeType *
DupAttributeType(AttributeType *in)
{
  AttributeType *new_var;
  struct attributeValues_List44	*lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeType,1);

  if (new_var == NULL)
    return NULL;


  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->attributeType = in->attributeType;

  lastat = NULL;  
  
  for (iat = in->attributeValues; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct attributeValues_List44,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->attributeValues = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeDescription(iat->item);
    lastat = newat;
  }

  return(new_var);

}


AttributeSetInfo *
DupAttributeSetInfo(AttributeSetInfo *in)
{
  AttributeSetInfo *new_var;
  struct attributes_List43 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeSetInfo,1);

  if (new_var == NULL)
    return NULL;


  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->attributeSet = DupOID(in->attributeSet);
  new_var->name = DupInternationalString(in->name);
  
  lastat = NULL;  
  
  for (iat = in->attributes; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct attributes_List43,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->attributes = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeType(iat->item);
    lastat = newat;
  }
  
  new_var->description = DupHumanString(in->description);
  
  return(new_var);
  
}

TermListInfo *
DupTermListInfo(TermListInfo *in)
{
  TermListInfo *new_var;
  struct termLists_List46 *lasttl, *newtl, *itl;
  struct broader_List47 *lastbl, *newbl, *ibl;
  struct narrower_List48 *lastnl, *newnl, *inl;

  new_var = CALLOC(TermListInfo,1);

  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);

  lasttl = NULL;  
  
  for (itl = in->termLists; itl != NULL; itl = itl->next) {
    /* create new_var list entry */
    newtl = CALLOC(struct termLists_List46,1);
    
    if (newtl == NULL)
      return NULL;
    
    /* link it in */
    if (lasttl == NULL) 
      new_var->termLists = lasttl = newtl;
    else
      lasttl->next = newtl;
    
    newtl->item.name = DupInternationalString(itl->item.name);
    newtl->item.title = DupHumanString(itl->item.title);
    newtl->item.searchCost = itl->item.searchCost;
    newtl->item.scanable = itl->item.scanable;
    
    lastbl = NULL;  
  
    for (ibl = itl->item.broader; ibl != NULL; ibl = ibl->next) {
      /* create new_var list entry */
      newbl = CALLOC(struct broader_List47,1);
      
      if (newbl == NULL)
	return NULL;
      
      /* link it in */
      if (lastbl == NULL) 
	newtl->item.broader = lastbl = newbl;
      else
	lastbl->next = newbl;
      
      newbl->item = DupInternationalString(ibl->item);
      lastbl = newbl;
    }

    lastnl = NULL;  
    
    for (inl = itl->item.narrower; inl != NULL; inl = inl->next) {
      /* create new_var list entry */
      newnl = CALLOC(struct narrower_List48,1);
      
      if (newnl == NULL)
	return NULL;
      
      /* link it in */
      if (lastnl == NULL) 
	newtl->item.narrower = lastnl = newnl;
      else
	lastnl->next = newnl;
      
      newnl->item = DupInternationalString(inl->item);
      lastnl = newnl;
    }
    lasttl = newtl;
  }
  return(new_var);
}

ExtendedServicesInfo *
DupExtendedServicesInfo(ExtendedServicesInfo *in)
{
  ExtendedServicesInfo *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ExtendedServicesInfo,1);

  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->type = DupOID(in->type);
  new_var->name = DupInternationalString(in->name);
  new_var->privateType = in->privateType;
  new_var->restrictionsApply = in->restrictionsApply;
  new_var->feeApply = in->feeApply;
  new_var->available = in->available;
  new_var->retentionSupported = in->retentionSupported;
  new_var->waitAction = in->waitAction;
  new_var->description = DupHumanString(in->description);
  new_var->specificExplain = DupExternal(in->specificExplain);
  new_var->esASN = DupInternationalString(in->esASN);
 
  return(new_var);
  
}

OmittedAttributeInterpretation *
DupOmittedAttributeInterpretation(OmittedAttributeInterpretation *in)
{
  OmittedAttributeInterpretation *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(OmittedAttributeInterpretation,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->defaultValue = DupStringOrNumeric(in->defaultValue);
  new_var->defaultDescription = DupHumanString(in->defaultDescription);
  
  return(new_var);
}

AttributeValue *
DupAttributeValue(AttributeValue *in)
{
  AttributeValue *new_var;
  struct subAttributes_List52  *lastat1, *newat1, *iat1;
  struct superAttributes_List53 *lastat2, *newat2, *iat2;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(AttributeValue,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->value = DupStringOrNumeric(in->value);
  new_var->description = DupHumanString(in->description);
  
  lastat1 = NULL;  
  
  for (iat1 = in->subAttributes; iat1 != NULL; iat1 = iat1->next) {
    /* create new_var list entry */
    newat1 = CALLOC(struct subAttributes_List52,1);
    
    if (newat1 == NULL)
      return NULL;
    
    /* link it in */
    if (lastat1 == NULL) 
      new_var->subAttributes = lastat1 = newat1;
    else
      lastat1->next = newat1;
    
    newat1->item = DupStringOrNumeric(iat1->item);
    lastat1 = newat1;
  }

  lastat2 = NULL;  
  
  for (iat2 = in->superAttributes; iat2 != NULL; iat2 = iat2->next) {
    /* create new_var list entry */
    newat2 = CALLOC(struct superAttributes_List53,1);
    
    if (newat2 == NULL)
      return NULL;
    
    /* link it in */
    if (lastat2 == NULL) 
      new_var->superAttributes = lastat2 = newat2;
    else
      lastat2->next = newat2;
    
    newat2->item = DupStringOrNumeric(iat2->item);
    lastat2 = newat2;
  }

  new_var->partialSupport = in->partialSupport;

  return(new_var);

}



AttributeTypeDetails *
DupAttributeTypeDetails(AttributeTypeDetails *in)
{
  AttributeTypeDetails *new_var;
  struct attributeValues_List51 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeTypeDetails,1);

  if (new_var == NULL)
    return NULL;

  new_var->attributeType = in->attributeType;
  
  new_var->defaultIfOmitted = 
    DupOmittedAttributeInterpretation(in->defaultIfOmitted);
  
  lastat = NULL;  
  
  for (iat = in->attributeValues; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct attributeValues_List51,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->attributeValues = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeValue(iat->item);
    lastat = newat;
  }

  return(new_var);  

}


AttributeSetDetails *
DupAttributeSetDetails(AttributeSetDetails *in)
{
  AttributeSetDetails *new_var;
  struct attributesByType_List50 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeSetDetails,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->attributeSet = DupOID(in->attributeSet);

  lastat = NULL;  
  
  for (iat = in->attributesByType; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct attributesByType_List50,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->attributesByType = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeTypeDetails(iat->item);
    lastat = newat;
  }

  return(new_var);  

}


AttributeOccurrence *
DupAttributeOccurrence(AttributeOccurrence *in)
{
  AttributeOccurrence *new_var;
  struct specific_List86 *lastat, *newat, *iat;
  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(AttributeOccurrence,1);
  
  if (new_var == NULL)
    return NULL;
  
  new_var->attributeSet = DupOID(in->attributeSet);
  new_var->attributeType = in->attributeType;
  new_var->mustBeSupplied = in->mustBeSupplied;
  
  new_var->attributeValues.which = in->attributeValues.which;
  
  if (in->attributeValues.which == e44_any_or_none)
    new_var->attributeValues.u.any_or_none = in->attributeValues.u.any_or_none;
  else {
    lastat = NULL;  
    
    for (iat = in->attributeValues.u.specific; iat != NULL; iat = iat->next) {
      /* create new_var list entry */
      newat = CALLOC(struct specific_List86,1);
      
      if (newat == NULL)
	return NULL;
      
      /* link it in */
      if (lastat == NULL) 
	new_var->attributeValues.u.specific = lastat = newat;
      else
	lastat->next = newat;
      
      newat->item = DupStringOrNumeric(iat->item);
      lastat = newat;
    }
  }
  return(new_var);
}

AttributeCombination *
DupAttributeCombination(AttributeCombination *in)
{
  AttributeCombination *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(AttributeCombination,1);

  if (new_var == NULL)
    return NULL;

  new_var->next = DupAttributeCombination(in->next);
  new_var->item = DupAttributeOccurrence(in->item);

  return(new_var);

};


AttributeCombinations *
DupAttributeCombinations(AttributeCombinations *in)
{
  AttributeCombinations *new_var;
  struct legalCombinations_List85 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeCombinations,1);

  if (new_var == NULL)
    return NULL;

  new_var->defaultAttributeSet = DupOID(in->defaultAttributeSet);

  lastat = NULL;  
  
  for (iat = in->legalCombinations; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct legalCombinations_List85,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->legalCombinations = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeCombination(iat->item);
    lastat = newat;
  }

  return(new_var);  

}


AttributeDetails *
DupAttributeDetails(AttributeDetails *in)
{
  AttributeDetails *new_var;
  struct attributesBySet_List49 *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(AttributeDetails,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);

  lastat = NULL;  
  
  for (iat = in->attributesBySet; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct attributesBySet_List49,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->attributesBySet = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupAttributeSetDetails(iat->item);
    lastat = newat;
  }

  new_var->attributeCombinations = 
    DupAttributeCombinations(in->attributeCombinations);

  return(new_var);  
}

TermListDetails *
DupTermListDetails(TermListDetails *in)
{
  TermListDetails *new_var;
  struct sampleTerms_List54  *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TermListDetails,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->termListName = DupInternationalString(in->termListName);
  new_var->description = DupHumanString(in->description);
  new_var->attributes = DupAttributeCombinations(in->attributes);

  if (in->scanInfo) {
    new_var->scanInfo = CALLOC(struct scanInfo,1);
    if (new_var->scanInfo == NULL)
      return NULL;
    
    new_var->scanInfo->maxStepSize =  in->scanInfo->maxStepSize;
  new_var->scanInfo->increasing =  in->scanInfo->increasing;
  new_var->scanInfo->collatingSequence =  
    DupHumanString(in->scanInfo->collatingSequence);
  }

  new_var->estNumberTerms = in->estNumberTerms;

  lastat = NULL;  
  
  for (iat = in->sampleTerms; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct sampleTerms_List54,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->sampleTerms = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupTerm(iat->item);
    lastat = newat;
  }

  
  return(new_var);
}

RecordTag *
DupRecordTag(RecordTag *in)
{
  RecordTag *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(RecordTag,1);

  if (new_var == NULL)
    return NULL;

  new_var->qualifier = DupStringOrNumeric(in->qualifier);
  new_var->tagValue = DupStringOrNumeric(in->tagValue);

  return(new_var);
}


PerElementDetails *
DupPerElementDetails(PerElementDetails *in)
{
  PerElementDetails *new_var;
  struct schemaTags_List57 *lastst, *newst, *ist;
  struct alternateNames_List58 *lastalt, *newalt, *ialt;
  struct genericNames_List59 *lastgen, *newgen, *igen;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(PerElementDetails,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->name = DupInternationalString (in->name);
  new_var->recordTag = DupRecordTag (in->recordTag);

  lastst = NULL;  
  
  for (ist = in->schemaTags; ist != NULL; ist = ist->next) {
    /* create new_var list entry */
    newst = CALLOC(struct schemaTags_List57,1);
    
    if (newst == NULL)
      return NULL;
    
    /* link it in */
    if (lastst == NULL) 
      new_var->schemaTags = lastst = newst;
    else
      lastst->next = newst;
    
    newst->item = DupPath(ist->item);
    lastst = newst;
  }
  
  
  new_var->maxSize = in->maxSize;
  new_var->minSize = in->minSize;
  new_var->avgSize = in->avgSize;
  new_var->fixedSize = in->fixedSize;
  new_var->repeatable = in->repeatable;
  new_var->required = in->required;
  
  new_var->description = DupHumanString (in->description);
  new_var->contents = DupHumanString (in->contents);
  new_var->billingInfo = DupHumanString (in->billingInfo);
  new_var->restrictions = DupHumanString (in->restrictions);
  
  lastalt = NULL;  
  
  for (ialt = in->alternateNames; ialt != NULL; ialt = ialt->next) {
    /* create new_var list entry */
    newalt = CALLOC(struct alternateNames_List58,1);
    
    if (newalt == NULL)
      return NULL;
    
    /* link it in */
    if (lastalt == NULL) 
      new_var->alternateNames = lastalt = newalt;
    else
      lastalt->next = newalt;
    
    newalt->item = DupInternationalString(ialt->item);
    lastalt = newalt;
  }
  
  
  lastgen = NULL;  
  
  for (igen = in->genericNames; igen != NULL; igen = igen->next) {
    /* create new_var list entry */
    newgen = CALLOC(struct genericNames_List59,1);
    
    if (newgen == NULL)
      return NULL;
    
    /* link it in */
    if (lastgen == NULL) 
      new_var->genericNames = lastgen = newgen;
    else
      lastgen->next = newgen;
    
    newgen->item = DupInternationalString(igen->item);
    lastgen = newgen;
  }

  new_var->searchAccess = DupAttributeCombinations(in->searchAccess);
  
  return(new_var);

}

ElementSetDetails *
DupElementSetDetails(ElementSetDetails *in)
{
  ElementSetDetails *new_var;
  struct detailsPerElement_List55   *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ElementSetDetails,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);
  new_var->elementSetName = (ElementSetName)
    DupInternationalString((InternationalString)in->elementSetName);
  new_var->recordSyntax = DupOID(in->recordSyntax);
  new_var->schema = DupOID(in->schema);
  new_var->description = DupHumanString(in->description);

  lastat = NULL;  
  
  for (iat = in->detailsPerElement; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct detailsPerElement_List55,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->detailsPerElement = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupPerElementDetails(iat->item);
    lastat = newat;
  }

  
  return(new_var);
}

RetrievalRecordDetails *
DupRetrievalRecordDetails(RetrievalRecordDetails *in)
{
  RetrievalRecordDetails *new_var;
  struct detailsPerElement_List56  *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(RetrievalRecordDetails,1);

  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);
  new_var->recordSyntax = DupOID(in->recordSyntax);
  new_var->schema = DupOID(in->schema);
  new_var->description = DupHumanString(in->description);
  
  lastat = NULL;  
  
  for (iat = in->detailsPerElement; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct detailsPerElement_List56,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->detailsPerElement = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupPerElementDetails(iat->item);
    lastat = newat;
  }
  return(new_var);
}

Specification *
DupSpecification(Specification *in)
{
  Specification *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(Specification,1);

  if (new_var == NULL)
    return NULL;

  new_var->schema = DupOID(in->schema);

  if (in->elementSpec != NULL) {
    new_var->elementSpec = CALLOC(struct elementSpec,1);
    if (new_var->elementSpec == NULL)
      return NULL;
    new_var->elementSpec->which = in->elementSpec->which;
    if (in->elementSpec->which == e16_elementSetName)
      new_var->elementSpec->u.elementSetName =
	DupInternationalString(in->elementSpec->u.elementSetName);
    else
      new_var->elementSpec->u.externalEspec =
	DupExternal(in->elementSpec->u.externalEspec);
  }
  return(new_var);
}


SortKeyDetails *
DupSortKeyDetails(SortKeyDetails *in)
{
  SortKeyDetails *new_var;
  struct elementSpecifications_List61  *lastat, *newat, *iat;
  struct sortType *st;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(SortKeyDetails,1);

  if (new_var == NULL)
    return NULL;
  
  new_var->description = DupHumanString(in->description);
  
  lastat = NULL;  
  
  for (iat = in->elementSpecifications; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct elementSpecifications_List61,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->elementSpecifications = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupSpecification(iat->item);
    lastat = newat;
  }

  new_var->attributeSpecifications = 
    DupAttributeCombinations(in->attributeSpecifications);
  
  if (in->sortType != NULL) {
    new_var->sortType = st = CALLOC(struct sortType,1);
    
    if (st == NULL)
      return NULL;
    
    st->which = in->sortType->which;
    
    switch (in->sortType->which) {
    case e37_character: 
      st->u.character = in->sortType->u.character;
      break;
    case e37_numeric:
      st->u.numeric = in->sortType->u.numeric;
      break;
    case e37_structured:
      st->u.structured = DupHumanString(in->sortType->u.structured);
      break;
    }
  }
  new_var->caseSensitivity = in->caseSensitivity;
  
  return(new_var);
}


SortDetails *
DupSortDetails(SortDetails *in)
{
  SortDetails *new_var;
  struct sortKeys_List60    *lastat, *newat, *iat;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(SortDetails,1);

  if (new_var == NULL)
    return NULL;

  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);

  lastat = NULL;  
  
  for (iat = in->sortKeys; iat != NULL; iat = iat->next) {
    /* create new_var list entry */
    newat = CALLOC(struct sortKeys_List60,1);
    
    if (newat == NULL)
      return NULL;
    
    /* link it in */
    if (lastat == NULL) 
      new_var->sortKeys = lastat = newat;
    else
      lastat->next = newat;
    
    newat->item = DupSortKeyDetails(iat->item);
    lastat = newat;
  }
  return(new_var);
}

ProcessingInformation *
DupProcessingInformation(ProcessingInformation *in)
{
  ProcessingInformation *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(ProcessingInformation,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->databaseName = (DatabaseName)DupGSTR((GSTR *)in->databaseName);
  new_var->processingContext = in->processingContext;
  new_var->name = DupInternationalString(in->name);
  new_var->oid = DupOID(in->oid);
  new_var->description = DupHumanString(in->description);
  new_var->instructions = DupExternal(in->instructions);

  return(new_var);
}


ValueDescription *
DupValueDescription(ValueDescription *in)
{
  ValueDescription *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(ValueDescription,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->which = in->which;
  
  switch (in->which) {
  case e39_integer:
    new_var->u.integer = in->u.integer;
    break;
  case e39_string:
    new_var->u.string = DupInternationalString(in->u.string);
    break;
  case e39_octets:
    new_var->u.octets = DupOstring(in->u.octets);
    break;
  case e39_oid:
    new_var->u.oid = DupOID(in->u.oid);
    break;
  case e39_unit:
    new_var->u.unit = DupUnit(in->u.unit);
    break;
  case e39_valueAndUnit:
    new_var->u.valueAndUnit = DupIntUnit(in->u.valueAndUnit);
    break;
  }

  return(new_var);

}

ValueRange *
DupValueRange(ValueRange *in)
{
  ValueRange *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(ValueRange,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->lower = DupValueDescription(in->lower);
  new_var->upper = DupValueDescription(in->upper);
    
  return(new_var);

}

ValueSet *
DupValueSet(ValueSet *in)
{
  ValueSet *new_var;
  struct enumerated_List64 *lastvr, *newvr, *ivr;


  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(ValueSet,1);
  
  if (new_var == NULL)
    return NULL;
  
  new_var->which = in->which;
  if (in->which == e38_range)
    new_var->u.range = DupValueRange(in->u.range);
  else {
    lastvr = NULL;  
  
    for (ivr = in->u.enumerated; ivr != NULL; ivr = ivr->next) {
      /* create new_var list entry */
      newvr = CALLOC(struct enumerated_List64,1);
      
      if (newvr == NULL)
	return NULL;
      
      /* link it in */
      if (lastvr == NULL) 
	new_var->u.enumerated = lastvr = newvr;
      else
	lastvr->next = newvr;
      
      newvr->item = DupValueDescription(ivr->item);
      lastvr = newvr;
    }

  }

  return(new_var);
}

VariantValue *
DupVariantValue(VariantValue *in)
{
  VariantValue *new_var;


  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(VariantValue,1);
  
  if (new_var == NULL)
    return NULL;

  new_var->dataType = in->dataType;
  new_var->values = DupValueSet(in->values);
  return(new_var);
}

VariantType *
DupVariantType(VariantType *in)
{
  VariantType *new_var;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(VariantType,1);
  
  if (new_var == NULL)
    return NULL;
  
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->variantType = in->variantType;
  new_var->variantValue = DupVariantValue(in->variantValue);
  return(new_var);
}

VariantClass *
DupVariantClass(VariantClass *in)
{
  VariantClass *new_var;
  struct variantTypes_List63 *lastvr, *newvr, *ivr;

  if (in == NULL)
    return NULL;
  
  new_var = CALLOC(VariantClass,1);
  
  if (new_var == NULL)
    return NULL;
  
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->variantClass = in->variantClass;

  lastvr = NULL;  
  
  for (ivr = in->variantTypes; ivr != NULL; ivr = ivr->next) {
    /* create new_var list entry */
    newvr = CALLOC(struct variantTypes_List63,1);
    
    if (newvr == NULL)
      return NULL;
    
    /* link it in */
    if (lastvr == NULL) 
      new_var->variantTypes = lastvr = newvr;
    else
      lastvr->next = newvr;
    
    newvr->item = DupVariantType(ivr->item);
    lastvr = newvr;
  }


  return(new_var);
}


VariantSetInfo *
DupVariantSetInfo(VariantSetInfo *in)
{
  VariantSetInfo *new_var;
  struct variants_List62 *lastvr, *newvr, *ivr;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(VariantSetInfo,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->name = DupInternationalString(in->name);
  new_var->variantSet = DupOID(in->variantSet);

  lastvr = NULL;  
  
  for (ivr = in->variants; ivr != NULL; ivr = ivr->next) {
    /* create new_var list entry */
    newvr = CALLOC(struct variants_List62,1);
    
    if (newvr == NULL)
      return NULL;
    
    /* link it in */
    if (lastvr == NULL) 
      new_var->variants = lastvr = newvr;
    else
      lastvr->next = newvr;
    
    newvr->item = DupVariantClass(ivr->item);
    lastvr = newvr;
  }
  
  return(new_var);
}

Units *
DupUnits(Units *in)
{
  Units *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Units,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->unit = DupStringOrNumeric(in->unit);

  return(new_var);

}
 

UnitType *
DupUnitType(UnitType *in)
{
  UnitType *new_var;
  struct units_List66 *lastvr, *newvr, *ivr;;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(UnitType,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->name = DupInternationalString(in->name);
  new_var->description = DupHumanString(in->description);
  new_var->unitType = DupStringOrNumeric(in->unitType);

  lastvr = NULL;  
  
  for (ivr = in->units; ivr != NULL; ivr = ivr->next) {
    /* create new_var list entry */
    newvr = CALLOC(struct units_List66,1);
    
    if (newvr == NULL)
      return NULL;
    
    /* link it in */
    if (lastvr == NULL) 
      new_var->units = lastvr = newvr;
    else
      lastvr->next = newvr;
    
    newvr->item = DupUnits(ivr->item);
    lastvr = newvr;
  }
  
  return(new_var);
}


UnitInfo *
DupUnitInfo(UnitInfo *in)
{
  UnitInfo *new_var;
  struct units_List65 *lastvr, *newvr, *ivr;;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(UnitInfo,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->commonInfo = DupCommonInfo(in->commonInfo);
  new_var->unitSystem = DupInternationalString(in->unitSystem);
  new_var->description = DupHumanString(in->description);


  lastvr = NULL;  
  
  for (ivr = in->units; ivr != NULL; ivr = ivr->next) {
    /* create new_var list entry */
    newvr = CALLOC(struct units_List65,1);
    
    if (newvr == NULL)
      return NULL;
    
    /* link it in */
    if (lastvr == NULL) 
      new_var->units = lastvr = newvr;
    else
      lastvr->next = newvr;
    
    newvr->item = DupUnitType(ivr->item);
    lastvr = newvr;
  }
  
  return(new_var);
}


CategoryInfo *
DupCategoryInfo(CategoryInfo *in)
{
  CategoryInfo *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(CategoryInfo,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->category = DupInternationalString(in->category);
  new_var->originalCategory = DupInternationalString(in->originalCategory);
  new_var->description = DupHumanString(in->description);
  new_var->asn1Module =  DupInternationalString(in->asn1Module);
  
  return(new_var);
}


CategoryList *
DupCategoryList(CategoryList *in)
{
  CategoryList *new_var;
  struct categories_List67 *lastvr, *newvr, *ivr;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(CategoryList,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->commonInfo = DupCommonInfo(in->commonInfo);

  lastvr = NULL;  
  
  for (ivr = in->categories; ivr != NULL; ivr = ivr->next) {
    /* create new_var list entry */
    newvr = CALLOC(struct categories_List67,1);
    
    if (newvr == NULL)
      return NULL;
    
    /* link it in */
    if (lastvr == NULL) 
      new_var->categories = lastvr = newvr;
    else
      lastvr->next = newvr;
    
    newvr->item = DupCategoryInfo(ivr->item);
    lastvr = newvr;
  }
  
  return(new_var);
}


Explain_Record *
DupExplainRecord(Explain_Record *in)
{
  Explain_Record *new_var;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(Explain_Record,1);

  new_var->which = in->which;
  switch (in->which) {
  case e34_targetInfo:
    new_var->u.targetInfo = DupTargetInfo(in->u.targetInfo);
    break;
  case e34_databaseInfo:
    new_var->u.databaseInfo = DupDatabaseInfo(in->u.databaseInfo);
    break;
  case e34_schemaInfo:
    new_var->u.schemaInfo = DupSchemaInfo(in->u.schemaInfo);
    break;
  case e34_tagSetInfo:
    new_var->u.tagSetInfo = DupTagSetInfo(in->u.tagSetInfo);
    break;
  case e34_recordSyntaxInfo:
    new_var->u.recordSyntaxInfo = DupRecordSyntaxInfo(in->u.recordSyntaxInfo);
    break;
  case e34_attributeSetInfo:
    new_var->u.attributeSetInfo = DupAttributeSetInfo(in->u.attributeSetInfo);
    break;
  case e34_termListInfo:
    new_var->u.termListInfo = DupTermListInfo(in->u.termListInfo);
    break;
  case e34_extendedServicesInfo:
    new_var->u.extendedServicesInfo = 
      DupExtendedServicesInfo(in->u.extendedServicesInfo);
    break;
  case e34_attributeDetails:
    new_var->u.attributeDetails = DupAttributeDetails(in->u.attributeDetails);
    break;
  case e34_termListDetails:
    new_var->u.termListDetails = DupTermListDetails(in->u.termListDetails);
    break;
  case e34_elementSetDetails:
    new_var->u.elementSetDetails = DupElementSetDetails(in->u.elementSetDetails);
    break;
  case e34_retrievalRecordDetails:
    new_var->u.retrievalRecordDetails = 
      DupRetrievalRecordDetails(in->u.retrievalRecordDetails);
    break;
  case e34_sortDetails:
    new_var->u.sortDetails = DupSortDetails(in->u.sortDetails);
    break;
  case e34_processing:
    new_var->u.processing = DupProcessingInformation(in->u.processing);
    break;
  case e34_variants:
    new_var->u.variants = DupVariantSetInfo(in->u.variants);
    break;
  case e34_units:
    new_var->u.units = DupUnitInfo(in->u.units);
    break;
  case e34_categoryList:
    new_var->u.categoryList = DupCategoryList(in->u.categoryList);
    break;

  }
  return (new_var);
}



DefaultDiagFormat *
DupNonSurrogateDiagnostic (dr)
DefaultDiagFormat *dr;
{
    DefaultDiagFormat *diagRec;
    if (dr == NULL)
	return NULL;

    diagRec = CALLOC(DefaultDiagFormat,1);

    if (diagRec == NULL)
	return NULL;

    diagRec->diagnosticSetId = DupOID (dr->diagnosticSetId);
    diagRec->condition = dr->condition;
    diagRec->addinfo.which = dr->addinfo.which;
    diagRec->addinfo.u.v3Addinfo = DupOstring (dr->addinfo.u.v3Addinfo);
    return diagRec;
}

DefaultDiagFormat *
DupDefaultDiagFormat(DefaultDiagFormat *in)
{
  return(DupNonSurrogateDiagnostic(in));
}

DiagRec *
DupDiagRec (dr)
DiagRec *dr;
{
    DiagRec *diagRec;

    if (dr == NULL)
      return NULL;

    diagRec = CALLOC(DiagRec,1);

    if (diagRec == NULL)
      return NULL;

    diagRec->which = dr->which;

    if (dr->which == e13_defaultFormat) 
      diagRec->u.defaultFormat = DupDefaultDiagFormat(dr->u.defaultFormat);
    else
      diagRec->u.externallyDefined = DupExternal(dr->u.externallyDefined);

    return diagRec;
}


NamePlusRecord *
DupNamePlusRecord (npr)
NamePlusRecord *npr;
{
    NamePlusRecord *namePlusRecord;

    if (!npr)
	return NULL;

    namePlusRecord = CALLOC(NamePlusRecord,1);
    if (namePlusRecord == NULL)
	return NULL;
    namePlusRecord->name = DupOstring (npr->name);
    if (npr->record.which == e11_retrievalRecord) {
    	namePlusRecord->record.which = e11_retrievalRecord;
	namePlusRecord->record.u.retrievalRecord = 
		DupExternal(npr->record.u.retrievalRecord);
    } 
    else if (npr->record.which == e11_surrogateDiagnostic) {
    	namePlusRecord->record.which = e11_surrogateDiagnostic;
	namePlusRecord->record.u.surrogateDiagnostic = 
		DupDiagRec(npr->record.u.surrogateDiagnostic);
    }

    return namePlusRecord;
}

Permissions *
DupPermissions(Permissions *in)
{
  Permissions *new_var;
  struct allowableFunctions_List22 *lastaf, *newaf, *iaf;
  if (in == NULL)
    return NULL;

  new_var = CALLOC(Permissions,1);

  if (new_var == NULL)
    return NULL;

  new_var->next = DupPermissions(in->next);

  new_var->item.userId = DupInternationalString(in->item.userId);

  lastaf = NULL;  
  
  for (iaf = in->item.allowableFunctions; iaf != NULL; iaf = iaf->next) {
    /* create new_var list entry */
    newaf = CALLOC(struct allowableFunctions_List22,1);
    /* link it in */
    if (lastaf == NULL) 
      new_var->item.allowableFunctions = lastaf = newaf;
    else
      lastaf->next = newaf;
    
    newaf->item = iaf->item;
    lastaf = newaf;    
  }
  return (new_var);
}


TaskPackage *
DupESRecord(TaskPackage *in)
{
  TaskPackage *new_var;
  struct packageDiagnostics_List95 *lastpd, *newpd, *ipd;

  if (in == NULL)
    return NULL;

  new_var = CALLOC(TaskPackage,1);

  if (new_var == NULL)
    return NULL;

  new_var->packageType = DupOID(in->packageType);
  new_var->packageName = DupInternationalString(in->packageName);
  new_var->userId = DupInternationalString(in->userId);
  new_var->retentionTime = DupIntUnit(in->retentionTime);
  new_var->permissions = DupPermissions(in->permissions);
  new_var->description = DupInternationalString(in->description);
  new_var->targetReference = DupOstring(in->targetReference);
  new_var->creationDateTime = DupGeneralizedTime(in->creationDateTime);
  new_var->taskStatus = in->taskStatus;

  lastpd = NULL;  
  
  for (ipd = in->packageDiagnostics; ipd != NULL; ipd = ipd->next) {
    /* create new_var list entry */
    newpd = CALLOC(struct packageDiagnostics_List95,1);
    /* link it in */
    if (lastpd == NULL) 
      new_var->packageDiagnostics = lastpd = newpd;
    else
      lastpd->next = newpd;
    
    newpd->item = DupDiagRec(ipd->item);
    lastpd = newpd;
    
  }
  
  
  new_var->taskSpecificParameters = DupExternal(in->taskSpecificParameters);
  
  return(new_var);
}




