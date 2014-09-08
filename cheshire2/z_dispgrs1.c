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
/*
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:      Ray Larson, ray@sherlock.berkeley.edu
 *              School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************************************************************/
/* z_dispgrs1 - print GRS1 format records                                 */
/**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <tcl.h>

#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif

#include "z3950_3.h"
#include "zprimitive.h"
#include "z_parameters.h"

/* the following are defined in z_explain.c */
int GetResultGSTR (GSTR *gstr, Tcl_Interp *interp);
int GetLanguageCode(LanguageCode *in, Tcl_Interp *interp);
int GetInternationalString2(InternationalString in, Tcl_Interp *interp);
int GetInt(int in, Tcl_Interp *interp);
int GetBoolean(Boolean in, Tcl_Interp *interp);
int GetStringOrNumeric(StringOrNumeric *in, Tcl_Interp *interp);
int GetOid(ObjectIdentifier in, Tcl_Interp *interp);
int GetUnit(Unit *in, Tcl_Interp *interp);
int GetIntUnit(IntUnit *in, Tcl_Interp *interp);
int GetHumanString(HumanString *in, Tcl_Interp *interp);
int GetGeneralizedTime(GeneralizedTime in, Tcl_Interp *interp);
int GetElementData(ElementData *in, Tcl_Interp *interp, int format);
int GetOctetString2(OctetString in, Tcl_Interp *interp);



int
GetTerm(Term *in, Tcl_Interp *interp) {

  switch (in->which) {
  case e5_general:
    GetOctetString2(in->u.general, interp);
    break;

  case  e5_numeric:
    GetInt(in->u.numeric, interp);
    break;
    
  case  e5_characterString:
    GetInternationalString2(in->u.characterString, interp);
    break;
    
  case  e5_oid:
    GetOid(in->u.oid, interp);
    break;
    
  case  e5_dateTime:
    GetGeneralizedTime(in->u.dateTime, interp);
    break;
    
  case  e5_external:
    Tcl_AppendResult(interp, "EXTERNAL", (char *) NULL);
    /* GetExternal(in->u.external, interp); */
    break;

  case  e5_integerAndUnit:
    GetIntUnit(in->u.integerAndUnit, interp);
    break;

  case  e5_nullVal:
    Tcl_AppendResult(interp, "NULL", (char *) NULL);
    /* unsigned char	nullVal; */
    break;
  } 

  return(0);
}


int
GetTagPath(TagPath *in, Tcl_Interp *interp) {

  if (in == NULL)
    return(1);

  GetInt(in->item.tagType, interp);
  Tcl_AppendResult(interp, "} {tagValue:: ", (char *) NULL);
        GetStringOrNumeric(in->item.tagValue, interp);
  Tcl_AppendResult(interp, "} {tagOccurrence:: ", (char *) NULL);
        GetInt(in->item.tagOccurrence, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);

  GetTagPath(in->next, interp); /* tail recursion */
  return(0);
}

int
GetOrder(Order *in, Tcl_Interp *interp) {
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {ascending:: ", (char *) NULL);
  GetBoolean(in->ascending, interp);
  Tcl_AppendResult(interp, "} {order:: ", (char *) NULL);
  GetInt(in->order, interp);
  Tcl_AppendResult(interp, "}", (char *) NULL);
  return(0);

}

int
GetUsage(Usage *in, Tcl_Interp *interp) { 
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {type:: ", (char *) NULL);
  GetInt(in->type, interp);
  Tcl_AppendResult(interp, "} {restriction:: {", (char *) NULL);
  GetInternationalString2(in->restriction, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  return 0;
}

int
GetHitVector(HitVector *in, Tcl_Interp *interp) {
  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {HitVector:: {satisfier:: ", (char *) NULL);
  GetTerm(in->satisfier, interp);
  Tcl_AppendResult(interp, "} {offsetIntoElement:: ", (char *) NULL);
  GetIntUnit(in->offsetIntoElement, interp);
  Tcl_AppendResult(interp, "} {length:: ", (char *) NULL);
  GetIntUnit(in->length, interp);
  Tcl_AppendResult(interp, "} {hitRank:: ", (char *) NULL);
  GetInt(in->hitRank, interp);
  Tcl_AppendResult(interp, "} {targetToken:: ", (char *) NULL);
  GetOctetString2(in->targetToken, interp);
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  return 0;
}

int
GetVariant(Variant *in, Tcl_Interp *interp) {

  struct triples_List94	*tl;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {Variant:: {globalVariantSetId:: ", (char *) NULL);
  GetOid(in->globalVariantSetId, interp);
  Tcl_AppendResult(interp, "} {triples:: ", (char *) NULL);

  for (tl = in->triples; tl;  tl = tl->next) {

    Tcl_AppendResult(interp, " {{varianSetId:: ", (char *) NULL);
    GetOid(tl->item.variantSetId, interp);
    Tcl_AppendResult(interp, "} {class:: ", (char *) NULL);
    GetInt(tl->item.class_var, interp);
    Tcl_AppendResult(interp, "} {type:: ", (char *) NULL);
    GetInt(tl->item.type, interp);
    Tcl_AppendResult(interp, "} {value:: ", (char *) NULL);
    
    switch (tl->item.value.which) {
      
    case e47_intVal:
      GetInt(tl->item.value.u.intVal, interp);
      break;
      
    case  e47_interStringVal:
      GetInternationalString2(tl->item.value.u.interStringVal, interp);
      break;
      
    case  e47_octStringVal:
      GetOctetString2(tl->item.value.u.octStringVal, interp);
      break;
      
    case  e47_oidVal:
      GetOid(tl->item.value.u.oidVal, interp);
      break;
      
    case  e47_boolVal:
      GetBoolean(tl->item.value.u.boolVal, interp);
      break;
      
    case  e47_nullVal:
      Tcl_AppendResult(interp, "NULL", (char *) NULL);
      /* unsigned char	nullVal; */
      break;
      
    case  e47_unit:
      GetUnit(tl->item.value.u.unit, interp);
      break;
      
    case  e47_valueAndUnit:
      GetIntUnit(tl->item.value.u.valueAndUnit, interp);
      break;
    }
  }
  Tcl_AppendResult(interp, "}}", (char *) NULL);
  return 0;
}


int
GetElementMetaData(ElementMetaData *in, Tcl_Interp *interp) {

  struct hits_List92	*hv;
  struct supportedVariants_List93 *vl;

  if (in == NULL)
    return 1;

  Tcl_AppendResult(interp, " {seriesOrder:: ", (char *) NULL);
  GetOrder(in->seriesOrder, interp);
  Tcl_AppendResult(interp, "} {usageRight:: ", (char *) NULL);
  GetUsage(in->usageRight, interp);
  
  Tcl_AppendResult(interp, "} {hits:: ", (char *) NULL);
  for (hv = in->hits; hv; hv = hv->next) {
    GetHitVector(hv->item, interp);
  } 
  Tcl_AppendResult(interp, "} {displayName:: ", (char *) NULL);
  GetInternationalString2(in->displayName, interp);

  Tcl_AppendResult(interp, "} {supportedVariants:: ", (char *) NULL);
  for (vl = in->supportedVariants; vl; vl = vl->next) {
    GetVariant(vl->item, interp);
  }
  Tcl_AppendResult(interp, "} {message:: ", (char *) NULL);
  GetInternationalString2(in->message, interp);
  Tcl_AppendResult(interp, "} {elementDescriptor:: ", (char *) NULL);
  GetOctetString2(in->elementDescriptor, interp);
  Tcl_AppendResult(interp, "} {surrogateFor:: ", (char *) NULL);
  GetTagPath(in->surrogateFor, interp);
  Tcl_AppendResult(interp, "} {surrogateElement:: ", (char *) NULL);
  GetTagPath(in->surrogateElement, interp);
  Tcl_AppendResult(interp, "} {other:: external", (char *) NULL);
  /* GetExternal(in->other, interp); */
  Tcl_AppendResult(interp, "}", (char *) NULL);
  return 0;
}


int
CheckElementMetaData(ElementMetaData *in, Tcl_Interp *interp) {

  struct hits_List92	*hv;
  struct supportedVariants_List93 *vl;

  if (in == NULL || in->message == NULL || in->message->data == NULL)
    return 0;

  if (strncasecmp(in->message->data, "Attribute", 9) == 0)
    return 1;
  else 
    return 0;
}


int
GetTaggedElement(TaggedElement *in, Tcl_Interp *interp, int format) {

  if (format == 1) {
    Tcl_AppendResult(interp, " {{", (char *) NULL);
    GetStringOrNumeric(in->tagValue, interp);
  }
  else {
    Tcl_AppendResult(interp, " {{{tagType:: ", (char *) NULL);
    GetInt(in->tagType, interp);
    Tcl_AppendResult(interp, "} {tagValue:: ", (char *) NULL);
    GetStringOrNumeric(in->tagValue, interp);
  }
  if (in->tagType == 1) {
    /* this is using tagSet-M */
    if (in->tagValue->which == e25_numeric) {
      switch (in->tagValue->u.numeric) {
      case 1:
	Tcl_AppendResult(interp, " schemaIdentifier", (char *) NULL);
	break;
      case 2:
	Tcl_AppendResult(interp, " elementsOrdered", (char *) NULL);
	break;
      case 3:
	Tcl_AppendResult(interp, " elementOrdering", (char *) NULL);
	break;
      case 4:
	Tcl_AppendResult(interp, " defaultTagType", (char *) NULL);
	break;
      case 5:
	Tcl_AppendResult(interp, " defaultVariantSetId", (char *) NULL);
	break;
      case 6:
	Tcl_AppendResult(interp, " defaultVariantSpec", (char *) NULL);
	break;
      case 7:
	Tcl_AppendResult(interp, " processingInstructions", (char *) NULL);
	break;
      case 8:
	Tcl_AppendResult(interp, " recordUsage", (char *) NULL);
	break;
      case 9:
	Tcl_AppendResult(interp, " restriction", (char *) NULL);
	break;
      case 10:
	Tcl_AppendResult(interp, " rank", (char *) NULL);
	break;
      case 11:
	Tcl_AppendResult(interp, " userMessage", (char *) NULL);
	break;
      case 12:
	Tcl_AppendResult(interp, " url", (char *) NULL);
	break;
      case 13:
	Tcl_AppendResult(interp, " record", (char *) NULL);
	break;
      case 14:
	Tcl_AppendResult(interp, " localControlNumber", (char *) NULL);
	break;
      case 15:
	Tcl_AppendResult(interp, " creationDate", (char *) NULL);
	break;
      case 16:
	Tcl_AppendResult(interp, " dateOfLastModification", (char *) NULL);
	break;
      case 17:
	Tcl_AppendResult(interp, " dateOfLastReview", (char *) NULL);
	break;
      case 18:
	Tcl_AppendResult(interp, " score", (char *) NULL);
	break;
      case 19:
	Tcl_AppendResult(interp, " wellKnown", (char *) NULL);
	break;
      case 20:
	Tcl_AppendResult(interp, " recordWrapper", (char *) NULL);
	break;
      case 21:
	Tcl_AppendResult(interp, " defaultTagSetId", (char *) NULL);
	break;
      case 22:
	Tcl_AppendResult(interp, " LanguageOfRecord", (char *) NULL);
	break;
      case 23:
	Tcl_AppendResult(interp, " type", (char *) NULL);
	break;
      case 24:
	Tcl_AppendResult(interp, " Scheme", (char *) NULL);
	break;
      case 25:
	Tcl_AppendResult(interp, " costInfo", (char *) NULL);
	break;
      case 26:
	Tcl_AppendResult(interp, " costFlag", (char *) NULL);
	break;
      }
    }
  }
  else if (in->tagType == 2) {
    /* this is using tagSet-G */
    if (in->tagValue->which == e25_numeric) {
      switch (in->tagValue->u.numeric) {
      case 1:
	Tcl_AppendResult(interp, " title", (char *) NULL);
	break;
      case 2:
	Tcl_AppendResult(interp, " author", (char *) NULL);
	break;
      case 3:
	Tcl_AppendResult(interp, " publicationPlace", (char *) NULL);
	break;
      case 4:
	Tcl_AppendResult(interp, " publicationDate", (char *) NULL);
	break;
      case 5:
	Tcl_AppendResult(interp, " documentId", (char *) NULL);
	break;
      case 6:
	Tcl_AppendResult(interp, " abstract", (char *) NULL);
	break;
      case 7:
	Tcl_AppendResult(interp, " name", (char *) NULL);
	break;
      case 8:
	Tcl_AppendResult(interp, " dateTime", (char *) NULL);
	break;
      case 9:
	Tcl_AppendResult(interp, " displayObject", (char *) NULL);
	break;
      case 10:
	Tcl_AppendResult(interp, " organization", (char *) NULL);
	break;
      case 11:
	Tcl_AppendResult(interp, " postalAddress", (char *) NULL);
	break;
      case 12:
	Tcl_AppendResult(interp, " networkAddress", (char *) NULL);
	break;
      case 13:
	Tcl_AppendResult(interp, " eMailAddress", (char *) NULL);
	break;
      case 14:
	Tcl_AppendResult(interp, " phoneNumber", (char *) NULL);
	break;
      case 15:
	Tcl_AppendResult(interp, " faxNumber", (char *) NULL);
	break;
      case 16:
	Tcl_AppendResult(interp, " country", (char *) NULL);
	break;
      case 17:
	Tcl_AppendResult(interp, " description", (char *) NULL);
	break;
      case 18:
	Tcl_AppendResult(interp, " time", (char *) NULL);
	break;
      case 19:
	Tcl_AppendResult(interp, " DocumentContent", (char *) NULL);
	break;
      case 20:
	Tcl_AppendResult(interp, " language", (char *) NULL);
	break;
      case 21:
	Tcl_AppendResult(interp, " subject", (char *) NULL);
	break;
      case 22:
	Tcl_AppendResult(interp, " resourceType", (char *) NULL);
	break;
      case 23:
	Tcl_AppendResult(interp, " city", (char *) NULL);
	break;
      case 24:
	Tcl_AppendResult(interp, " stateOrProvince", (char *) NULL);
	break;
      case 25:
	Tcl_AppendResult(interp, " zipOrPostalCode", (char *) NULL);
	break;
      case 26:
	Tcl_AppendResult(interp, " cost", (char *) NULL);
	break;
      case 27:
	Tcl_AppendResult(interp, " format", (char *) NULL);
	break;
      case 28:
	Tcl_AppendResult(interp, " identifier", (char *) NULL);
	break;
      case 29:
	Tcl_AppendResult(interp, " rights", (char *) NULL);
	break;
      case 30:
	Tcl_AppendResult(interp, " relation", (char *) NULL);
	break;
      case 31:
	Tcl_AppendResult(interp, " publisher", (char *) NULL);
	break;
      case 32:
	Tcl_AppendResult(interp, " contributor", (char *) NULL);
	break;
      case 33:
	Tcl_AppendResult(interp, " source", (char *) NULL);
	break;
      case 34: 
	Tcl_AppendResult(interp, " coverage", (char *) NULL);
	break;
      case 35:
	Tcl_AppendResult(interp, " private_var", (char *) NULL);
	break;
      }
    }
  }
  else 
    Tcl_AppendResult(interp, " local", (char *) NULL);
  
  if (format == 1) {
    if (CheckElementMetaData(in->metaData, interp))
      Tcl_AppendResult(interp, " ATTR} {CONT:", (char *) NULL);
    else
      Tcl_AppendResult(interp, "} {CONT:", (char *) NULL);
    GetElementData(in->content, interp, format);
    Tcl_AppendResult(interp, "}", (char *) NULL);
  }
  else {
    Tcl_AppendResult(interp, "} {tagOccurrence:: ", (char *) NULL);
    GetInt(in->tagOccurrence, interp);
    Tcl_AppendResult(interp, "}} {content:: {", (char *) NULL);
    GetElementData(in->content, interp, format);
    Tcl_AppendResult(interp, "}}", (char *) NULL);
    if (in->metaData) {
      Tcl_AppendResult(interp, " {metaData {", (char *) NULL);
      GetElementMetaData(in->metaData, interp);
      Tcl_AppendResult(interp, "}}", (char *) NULL);
    }
    if (in->appliedVariant) {
      Tcl_AppendResult(interp, " {appliedVariant {", (char *) NULL);
      GetVariant(in->appliedVariant, interp);
      Tcl_AppendResult(interp, "}}", (char *) NULL);
    }
  }
  Tcl_AppendResult(interp, "}", (char *) NULL);

  return 0;
}


int
GetElementData(ElementData *in, Tcl_Interp *interp, int format) {
  struct subtree_List91	*sub;

  if (in->which != e46_subtree 
      && format == 1) {
    Tcl_AppendResult(interp, " {", (char *) NULL);
  }

  switch (in->which) {
  case e46_octets:
    GetOctetString2(in->u.octets, interp);
    break;
  case e46_numeric:
    GetInt(in->u.numeric, interp);
    break;
  case e46_date:
    GetGeneralizedTime(in->u.date, interp);
    break;
  case e46_ext:
    Tcl_AppendResult(interp, "External", (char *) NULL);
    /* GetExternal(in->u.ext, interp); */
    break;
  case e46_string:
    GetInternationalString2(in->u.string, interp);
    break;
  case e46_trueOrFalse:
    GetBoolean(in->u.trueOrFalse, interp);
    break;
  case e46_oid:
    GetOid(in->u.oid, interp);
    break;
  case e46_intUnit:
    GetIntUnit(in->u.intUnit, interp);
    break;
  case e46_elementNotThere:
    Tcl_AppendResult(interp, "elementNotThere", (char *) NULL);
    break;
  case e46_elementEmpty:
    Tcl_AppendResult(interp, "elementEmpty", (char *) NULL);
    break;
  case e46_noDataRequested:
    Tcl_AppendResult(interp, "elementEmpty", (char *) NULL);
    break;
  case e46_diagnostic:
    Tcl_AppendResult(interp, "diagnostic", (char *) NULL);
    /*    GetExternal(in->diagnostic, interp); */
    break;
  case e46_subtree:
    for (sub = in->u.subtree; sub; sub = sub->next) {
      if (format == 1) {
	Tcl_AppendResult(interp, " {SUB:", (char *) NULL);
	GetTaggedElement(sub->item, interp, format);
	Tcl_AppendResult(interp, "}", (char *) NULL);

      }
      else {
	Tcl_AppendResult(interp, " {SUBELEMENT::", (char *) NULL);
	GetTaggedElement(sub->item, interp, format);
	Tcl_AppendResult(interp, "}", (char *) NULL);
      }
    }
    break;

  }

  if (in->which != e46_subtree && format == 1) {
    Tcl_AppendResult(interp, "}", (char *) NULL);
  }
  
  return 0;
}


int
GetGenericRecord(GenericRecord *in, Tcl_Interp *interp) {
    GenericRecord	*rec;
    TaggedElement	*item;
    char *formattype;
    int format;

    if ((formattype = Tcl_GetVar(interp, "CHESHIRE_GRSFORMAT",
				TCL_GLOBAL_ONLY )) == NULL) {
      format = 0; /* long */
    }
    else {
      if (strcasecmp("SHORT",formattype) == 0 
	  || strcasecmp("SMALL",formattype) == 0
	  || strcasecmp("S",formattype) == 0)
	format = 1;
      else
	format = 0;
    }
    
    if (format == 1)
      Tcl_AppendResult(interp, " {GRS-1::", (char *) NULL);
    else
      Tcl_AppendResult(interp, " {GenericRecord::", (char *) NULL);
    for (rec = in; rec ; rec = rec->next) {      
      GetTaggedElement(rec->item, interp, format);
    }
    Tcl_AppendResult(interp, "}", (char *) NULL);
    return 0;
}


int
MakeGRS1Recs(GenericRecord *genrec, Tcl_Interp *interp)
{
  
  if (genrec == NULL)
    return 1;
  
  GetGenericRecord(genrec, interp);
  
  return 0;
}


