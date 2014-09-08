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

/* the following items are for freeing non-MARC ASN1-defined records */


void FreeGSTR (GSTR *);
void FreeOctetString (OctetString);
void FreeBitString (BitString);
void FreeInternationalString (InternationalString);
void FreeOid (ObjectIdentifier);
void FreeASN1type(DOCTYPE, any);
void FreeExternal (External *);
void FreeCircRecord(CircRecord *);
void FreeVolume(Volume *);
void FreeHoldingsAndCirc(HoldingsAndCircData *);
void FreeHoldingsRecord(HoldingsRecord *);
void FreeOPACRecord(OPACRecord *);
void FreeStringOrNumeric(StringOrNumeric *);
void FreeUnit(Unit *);
void FreeIntUnit(IntUnit *);
void FreeElementData(ElementData *);
void FreeVariant(Variant *);
void FreeTaggedElement(TaggedElement *);
void FreeTagPath(TagPath *);
void FreeOrder(Order *);
void FreeUsage(Usage *);
void FreeTerm(Term *);
void FreeHitVector(HitVector *);
void FreeElementMetaData(ElementMetaData *);
void FreeGRS1Record(GenericRecord *);
void FreeInfoCategory(InfoCategory *);
void FreeOtherInformation(OtherInformation *);
void FreeFormatSpec(FormatSpec *);
void FreeSummaryRecord(BriefBib *);
void FreeLanguageCode(LanguageCode *);
void FreeGeneralizedTime(GeneralizedTime in);
void FreeHumanString(HumanString *);
void FreeCommonInfo(CommonInfo *);
void FreeIconObject(IconObject *);
void FreeContactInfo(ContactInfo *);
void FreeDatabaseList(DatabaseList *);
void FreeNetworkAddress(NetworkAddress *);
void FreeAccessRestrictions(AccessRestrictions *);
void FreeSearchKey(SearchKey *);
void FreePrivateCapabilities(PrivateCapabilities *);
void FreeProximitySupport(ProximitySupport *);
void FreeRpnCapabilities(RpnCapabilities *);
void FreeIso8777Capabilities(Iso8777Capabilities *);
void FreeQueryTypeDetails(QueryTypeDetails *);
void FreeCharge(Charge *);
void FreeCosts(Costs *);
void FreeAccessInfo(AccessInfo *);
void FreeTargetInfo(TargetInfo *);
void FreeDatabaseInfo(DatabaseInfo *);
void FreePath(Path *);
void FreeElementDataType(ElementDataType *);
void FreeElementInfo(ElementInfo *);
void FreeSchemaInfo(SchemaInfo *);
void FreeTagSetInfo(TagSetInfo *);
void FreeRecordSyntaxInfo(RecordSyntaxInfo *);
void FreeAttributeDescription(AttributeDescription *);
void FreeAttributeType(AttributeType *);
void FreeAttributeSetInfo(AttributeSetInfo *);
void FreeTermListInfo(TermListInfo *);
void FreeExtendedServicesInfo(ExtendedServicesInfo *);
void FreeOmittedAttributeInterpretation(OmittedAttributeInterpretation *);
void FreeAttributeValue(AttributeValue *);
void FreeAttributeTypeDetails(AttributeTypeDetails *);
void FreeAttributeSetDetails(AttributeSetDetails *);
void FreeAttributeOccurrence(AttributeOccurrence *);
void FreeAttributeCombination(AttributeCombination *);
void FreeAttributeCombinations(AttributeCombinations *);
void FreeAttributeDetails(AttributeDetails *);
void FreeTermListDetails(TermListDetails *);
void FreeRecordTag(RecordTag *);
void FreePerElementDetails(PerElementDetails *);
void FreeElementSetDetails(ElementSetDetails *);
void FreeRetrievalRecordDetails(RetrievalRecordDetails *);
void FreeSpecification(Specification *);
void FreeSortKeyDetails(SortKeyDetails *);
void FreeSortDetails(SortDetails *);
void FreeProcessingInformation(ProcessingInformation *);
void FreeValueDescription(ValueDescription *);
void FreeValueRange(ValueRange *);
void FreeValueSet(ValueSet *);
void FreeVariantValue(VariantValue *);
void FreeVariantType(VariantType *);
void FreeVariantClass(VariantClass *);
void FreeVariantSetInfo(VariantSetInfo *);
void FreeUnits(Units *);
void FreeUnitType(UnitType *);
void FreeUnitInfo(UnitInfo *);
void FreeCategoryInfo(CategoryInfo *);
void FreeCategoryList(CategoryList *);
void FreeExplain_Record(Explain_Record *);
void FreeNonSurrogateDiagnostic (DefaultDiagFormat *);
void FreeDefaultDiagFormat(DefaultDiagFormat *);
void FreeDiagRec (DiagRec *);
void FreeNamePlusRecord (NamePlusRecord *);
void FreePermissions(Permissions *);
void FreeESRecord(TaskPackage *);


void
FreeASN1type(DOCTYPE doctype, any data) 
{

  if (data == NULL)
    return;

  switch (doctype) {
  case Z_OPAC_REC:             /* OPAC record syntax     */
    FreeOPACRecord((OPACRecord *)data);
    break;
  case Z_EXPLAIN_REC:          /* EXPLAIN record syntax  */
    FreeExplain_Record((Explain_Record *)data);
    break;
  case Z_GRS_1_REC:            /* Generic record syntax 1*/
    FreeGRS1Record((GenericRecord *)data);
    break;
  case Z_ES_REC:               /* Extended Services      */
    FreeESRecord((TaskPackage *)data);
    break;
  case Z_SUMMARY_REC:          /* Summary record syntax  */
    FreeSummaryRecord((BriefBib *)data);
    break;
  default:
    break;
  }
}

void
FreeExternal (ex)
External *ex;
{
  DOCTYPE GetTypeExternal();

  if (ex == NULL)
    return;

  switch (ex->which) {
  case t_singleASN1type:
    /* currently these pointers are retained in client "hits" lists
     * so the data should be freed directly when finished.
     *  FreeASN1type (GetTypeExternal(ex),
     *		  ex->u.singleASN1type);
     */
    break;

  case t_octetAligned:
    FreeOctetString (ex->u.octetAligned);
    break;

  case t_arbitrary:
    FreeBitString (ex->u.arbitrary);
    break;
  }
  FreeOid (ex->directReference);
  FreeOid (ex->dataValueDescriptor);


}

void
FreeCircRecord(CircRecord *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->availablityDate);
  FreeInternationalString(in->availableThru);
  FreeInternationalString(in->restrictions);
  FreeInternationalString(in->itemId);
  FreeInternationalString(in->enumAndChron);
  FreeInternationalString(in->midspine);
  FreeInternationalString(in->temporaryLocation);

  (void)free(in);
}

void
FreeVolume(Volume *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->enumeration);
  FreeInternationalString(in->chronology);
  FreeInternationalString(in->enumAndChron);

  (void)free(in);

}
void
FreeHoldingsAndCirc(HoldingsAndCircData *in)
{
  struct volumes_List88 *nextv, *currv;
  struct circulationData_List89 *nextc, *currc;

  if (in == NULL)
    return;

  FreeInternationalString(in->typeOfRecord);
  FreeInternationalString(in->encodingLevel);
  FreeInternationalString(in->format);
  FreeInternationalString(in->receiptAcqStatus);
  FreeInternationalString(in->generalRetention);
  FreeInternationalString(in->completeness);
  FreeInternationalString(in->dateOfReport);
  FreeInternationalString(in->nucCode);
  FreeInternationalString(in->localLocation);
  FreeInternationalString(in->shelvingLocation);
  FreeInternationalString(in->callNumber);
  FreeInternationalString(in->shelvingData);
  FreeInternationalString(in->copyNumber);
  FreeInternationalString(in->publicNote);
  FreeInternationalString(in->reproductionNote);
  FreeInternationalString(in->termsUseRepro);
  FreeInternationalString(in->enumAndChron);
  for (currv = in->volumes; currv != NULL; currv = nextv) {
    nextv = currv->next;
    FreeVolume(currv->item);  
    (void) free ((char *)currv);
  }

  for (currc = in->circulationData; currc != NULL; currc = nextc) {
    FreeCircRecord(currc->item);  
    nextc = currc->next;
    (void) free ((char *)currc);
  }

  (void)free(in);

}

void
FreeHoldingsRecord(HoldingsRecord *in_rec)
{
  if (in_rec == NULL)
    return;

  if (in_rec->which == e45_marcHoldingsRecord)
    FreeExternal(in_rec->u.marcHoldingsRecord);
  else
    FreeHoldingsAndCirc(in_rec->u.holdingsAndCirc);

  (void)free(in_rec);

}

void
FreeOPACRecord(OPACRecord *in_opac)
{
  struct holdingsData_List87 *next, *in;

  if (in_opac == NULL)
    return;
 
  FreeExternal(in_opac->bibliographicRecord);
 
  for (in = in_opac->holdingsData; in != NULL; in = next) {
    FreeHoldingsRecord(in->item);
    next = in->next;
    (void) free ((char *)in);
  }
  return;
}

void
FreeStringOrNumeric(StringOrNumeric *in)
{
  if (in == NULL)
    return;

  if(in->which ==  e25_string) 
    FreeInternationalString(in->u.string);

  (void)free(in);
}

void
FreeUnit(Unit *in)
{
  if (in == NULL)
    return;
  FreeInternationalString(in->unitSystem);
  FreeStringOrNumeric(in->unitType);
  FreeStringOrNumeric(in->unit);

  (void)free(in);
}

void
FreeIntUnit(IntUnit *in)
{
  if (in == NULL)
    return;

  FreeUnit(in->unitUsed);
  (void)free(in);
}

void
FreeElementData(ElementData *in)
{
  struct subtree_List91 *next, *inl;

  if (in == NULL)
    return;

  switch(in->which) {
  
  case e46_octets:
    FreeOctetString(in->u.octets);
    break;
  case e46_numeric:
    break;
  case e46_date:
    FreeOctetString((OctetString)in->u.date);
    break;
  case e46_ext:
    FreeExternal(in->u.ext);
    break;
  case e46_string:
    FreeInternationalString(in->u.string);
    break;
  case e46_trueOrFalse:
    break;
  case e46_oid:
    FreeOid(in->u.oid);
    break;
  case e46_intUnit:
    FreeIntUnit(in->u.intUnit);
    break;
  case e46_elementNotThere:
    break;
  case e46_elementEmpty:
    break;
  case e46_noDataRequested:
    break;
  case e46_diagnostic:
    FreeExternal(in->u.diagnostic);
    break;
  case e46_subtree:
    for (inl = in->u.subtree; inl != NULL; inl = next) {
      FreeTaggedElement(inl->item);
      next = inl->next;
      (void) free ((char *)inl);
    }
    break;
  }

  (void)free(in);
}
void
FreeVariant(Variant *in)
{
  struct triples_List94	*next, *i;

  if (in == NULL)
    return;

  FreeOid(in->globalVariantSetId);

  for (i = in->triples; i != NULL; i = next) {
  
    FreeOid(i->item.variantSetId);
  
    switch(i->item.value.which) {
    case e47_intVal:
      break;
    case e47_interStringVal:
      FreeInternationalString(i->item.value.u.interStringVal);
      break;
    case e47_octStringVal:
      FreeOctetString(i->item.value.u.octStringVal);
      break;
    case e47_oidVal:
      FreeOid(i->item.value.u.oidVal);
      break;
    case e47_boolVal:
      break;
    case e47_nullVal:
      break;
    case e47_unit:
      FreeUnit(i->item.value.u.unit);
      break;
    case e47_valueAndUnit:
      FreeIntUnit(i->item.value.u.valueAndUnit);
      break;
    }
    next = i->next;
    (void) free ((char *)i);

  }
  (void)free(in);
}

void
FreeTaggedElement(TaggedElement *in)
{
  if (in == NULL)
    return;

  FreeStringOrNumeric(in->tagValue);
  FreeElementData(in->content);
  FreeElementMetaData(in->metaData);
  FreeVariant(in->appliedVariant);

  (void)free(in);
}
void
FreeTagPath(TagPath *in) 
{
  if (in == NULL)
    return;

  FreeTagPath(in->next);
  FreeStringOrNumeric(in->item.tagValue);

  (void)free(in);
}

void
FreeOrder(Order *in) 
{
  if (in == NULL)
    return;

  (void)free(in);
}

void
FreeUsage(Usage *in) 
{
  if (in == NULL)
    return;

  FreeInternationalString(in->restriction);
  (void)free(in);
}

void
FreeTerm(Term *in)
{
  if (in == NULL)
    return;

  switch(in->which) {
  case e5_general:
    FreeOctetString(in->u.general);
    break;
  case e5_numeric:
    break;
  case e5_characterString:
    FreeInternationalString(in->u.characterString);
    break;
  case e5_oid:
    FreeOid(in->u.oid);
    break;
  case e5_dateTime:
    FreeOctetString((OctetString)in->u.dateTime);
    break;
  case e5_external:
    FreeExternal(in->u.external);
    break;
  case e5_integerAndUnit:
    FreeIntUnit(in->u.integerAndUnit);
    break;
  case e5_nullVal:
    break;
  }
  (void)free(in);
}

void
FreeHitVector(HitVector *in) 
{
  if (in == NULL)
    return;

  FreeTerm(in->satisfier);
  FreeIntUnit(in->offsetIntoElement);
  FreeIntUnit(in->length);
  FreeOctetString(in->targetToken);

  (void)free(in);
}
void
FreeElementMetaData(ElementMetaData *in)
{
  struct hits_List92 *nextl, *i;
  struct supportedVariants_List93 *nextv, *iv;

  if (in == NULL)
    return;

  FreeOrder(in->seriesOrder);
  FreeUsage(in->usageRight);

  for (i = in->hits; i != NULL; i = nextl) {
    FreeHitVector(i->item);
    nextl = i->next;
    (void) free ((char *)i);
  }

  FreeInternationalString(in->displayName);

  for (iv = in->supportedVariants; iv != NULL; iv = nextv) {
    FreeVariant(iv->item);
    nextv = iv->next;
    (void) free ((char *)iv);
  }

  FreeInternationalString(in->message);
  FreeOctetString(in->elementDescriptor);
  FreeTagPath(in->surrogateFor);
  FreeTagPath(in->surrogateElement);
  FreeExternal(in->other);

  (void)free(in);

}
void
FreeGRS1Record(GenericRecord *in)
{
  if (in == NULL)
    return;

  FreeGRS1Record(in->next);

  FreeTaggedElement(in->item);

  (void)free(in);
}

void 
FreeGenericRecord(GenericRecord *in)
{
  FreeGRS1Record(in);
}

void
FreeInfoCategory(InfoCategory *in)
{
  if (in == NULL)
    return;
 
  FreeOid(in->categoryTypeId);
  (void)free(in);
}

void
FreeOtherInformation(OtherInformation *in)
{
  if (in == NULL)
    return;

  FreeOtherInformation(in->next);
  FreeInfoCategory(in->item.category);

  switch (in->item.information.which) {
  case e24_characterInfo:
    FreeInternationalString(in->item.information.u.characterInfo);
    break;
  case e24_binaryInfo:
      FreeOctetString(in->item.information.u.binaryInfo);
    break;
  case e24_externallyDefinedInfo:
    FreeExternal(in->item.information.u.externallyDefinedInfo);
    break;
  case e24_oid:
    FreeOid(in->item.information.u.oid);
    break;
  }
  (void)free(in);
}

void
FreeFormatSpec(FormatSpec *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->type);
  (void)free(in);
}

void
FreeSummaryRecord(BriefBib *in)
{
  struct format_List90 *next, *i;

  if (in == NULL)
    return;

  FreeInternationalString(in->title);
  FreeInternationalString(in->author);
  FreeInternationalString(in->callNumber);
  FreeInternationalString(in->recordType);
  FreeInternationalString(in->bibliographicLevel);
  FreeInternationalString(in->publicationPlace);
  FreeInternationalString(in->publicationDate);
  FreeInternationalString(in->targetSystemKey);
  FreeInternationalString(in->satisfyingElement);
  FreeInternationalString(in->documentId);
  FreeInternationalString(in->abstract);
  FreeOtherInformation(in->otherInfo);

  for (i = in->format; i != NULL; i = next) {
    FreeFormatSpec(i->item);
    next = i->next;
    (void) free ((char *)i);
  }
  (void)free(in);
}


void
FreeBriefBib(BriefBib *in)
{
  FreeSummaryRecord(in);
}

void
FreeLanguageCode(LanguageCode *in)
{
  FreeGSTR((GSTR *)in);
}

void
FreeGeneralizedTime(GeneralizedTime in)
{
  FreeGSTR((GSTR *)in);
}

void
FreeHumanString(HumanString *in)
{
  if (in == NULL)
    return;
 
  FreeHumanString(in->next);
  FreeLanguageCode(in->item.language);
  FreeInternationalString(in->item.text);

  (void)free(in);
}

void
FreeCommonInfo(CommonInfo *in)
{
  if (in == NULL)
    return;
 
  FreeGeneralizedTime(in->dateAdded);
  FreeGeneralizedTime(in->dateChanged);
  FreeGeneralizedTime(in->expiry);
  FreeLanguageCode(in->humanString_Language);
  FreeOtherInformation(in->otherInfo);
  (void)free(in);
}

void
FreeIconObject(IconObject *in)
{
  if (in == NULL)
    return;
  FreeIconObject(in->next);
  FreeOctetString(in->item.content);

  switch (in->item.bodyType.which) {
  case e40_ianaType:
    FreeInternationalString(in->item.bodyType.u.ianaType);
    break;
  case e40_z3950type:
    FreeInternationalString(in->item.bodyType.u.z3950type);
    break;
  case e40_otherType:
    FreeInternationalString(in->item.bodyType.u.otherType);
    break;
  }
  (void)free(in);
}
void
FreeContactInfo(ContactInfo *in)
{
  if (in == NULL)
    return;
 
  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeHumanString(in->address);
  FreeInternationalString(in->email);
  FreeInternationalString(in->phone);
  (void)free(in);

}

void
FreeDatabaseList(DatabaseList *in)
{
  if (in == NULL)
    return;

  FreeDatabaseList(in->next);

  FreeGSTR((GSTR *)in->item);

  (void)free(in);
}

void
FreeNetworkAddress(NetworkAddress *in)
{
  if (in == NULL)
    return;

  switch(in->which) {
  case e41_internetAddress:
    FreeInternationalString(in->u.internetAddress.hostAddress);
    break;
  case e41_osiPresentationAddress:
    FreeInternationalString(in->u.osiPresentationAddress.pSel);
    FreeInternationalString(in->u.osiPresentationAddress.sSel);
    FreeInternationalString(in->u.osiPresentationAddress.tSel);
    FreeInternationalString(in->u.osiPresentationAddress.nSap);
    break;
  case e41_other:
    FreeInternationalString(in->u.other.type);
    FreeInternationalString(in->u.other.address);
    break;
  }
  (void)free(in);
}

void
FreeAccessRestrictions(AccessRestrictions *in)
{
  struct accessChallenges_List83 *nextac, *iac;

  if (in == NULL)
    return;
 
  FreeAccessRestrictions(in->next);

  FreeHumanString(in->item.accessText);

  for (iac = in->item.accessChallenges; iac != NULL; iac = nextac) {
    FreeOid(iac->item);
    nextac = iac->next;
    (void) free ((char *)iac);
  }
  (void)free(in);
}
void
FreeSearchKey(SearchKey *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->searchKey);
  FreeHumanString(in->description);

  (void)free(in);
}
void
FreePrivateCapabilities(PrivateCapabilities *in)
{
  struct operators_List77 *nextop, *iop;
  struct searchKeys_List78 *nextsk, *isk;
  struct description_List79 *nextds, *ids;

  if (in == NULL)
    return;

  for (iop = in->operators; iop != NULL; iop = nextop) {
    FreeInternationalString(iop->item.operator_var);
    FreeHumanString(iop->item.description);
    nextop = iop->next;
    (void) free ((char *)iop);
  }

  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    FreeSearchKey(isk->item);
    nextsk = isk->next;
    (void) free ((char *)isk);
  }

  for (ids = in->description; ids != NULL; ids = nextds) {
    FreeHumanString(ids->item);
    nextds = ids->next;
    (void) free ((char *)ids);
  }
  (void)free(in);
}

void
FreeProximitySupport(ProximitySupport *in)
{
  struct unitsSupported_List82 *nextus, *ius;

  if (in == NULL)
    return;

  for (ius = in->unitsSupported; ius != NULL; ius = nextus) {
    if (ius->item != NULL) {
      if (ius->item->which != e43_known) 
	FreeHumanString(ius->item->u.private_var.description);
      (void)free((char *)ius->item);
    }
    nextus = ius->next;
    (void) free ((char *)ius);
  }
  (void)free(in);
}
void
FreeRpnCapabilities(RpnCapabilities *in)
{
  struct operators_List80 *nextop, *iop;;

  if (in == NULL)
    return;

  for (iop = in->operators; iop != NULL; iop = nextop) {
    nextop = iop->next;
    (void) free ((char *)iop);
  }

  FreeProximitySupport(in->proximity);

  (void)free(in);
}

void
FreeIso8777Capabilities(Iso8777Capabilities *in)
{
  struct searchKeys_List81 *nextsk, *isk;;

  if (in == NULL)
    return;

  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    FreeSearchKey(isk->item);
    nextsk = isk->next;
    (void) free ((char *)isk);
  }

  FreeHumanString(in->restrictions);

  (void)free(in);
}
void
FreeQueryTypeDetails(QueryTypeDetails *in)
{
  if (in == NULL)
    return;

  switch (in->which) {
  case e42_private:
    FreePrivateCapabilities(in->u.private_var);
    break;
  case e42_rpn:
    FreeRpnCapabilities(in->u.rpn);
    break;
  case e42_iso8777:
    FreeIso8777Capabilities(in->u.iso8777);
    break;
  case e42_z39_58:
    FreeHumanString(in->u.z39_58);
    break;
  case e42_erpn:
    FreeRpnCapabilities(in->u.erpn);
    break;
  case e42_rankedList:
    FreeHumanString(in->u.rankedList);
    break;
  }
  (void)free(in);
}

void
FreeCharge(Charge *in)
{
  if (in == NULL)
    return;
 
  FreeIntUnit(in->cost);
  FreeUnit(in->perWhat);
  FreeHumanString(in->text);

  (void)free(in);
}
void
FreeCosts(Costs *in)
{
  struct otherCharges_List84 *nextoc, *ioc;

  if (in == NULL)
    return;

  FreeCharge(in->connectCharge);
  FreeCharge(in->connectTime);
  FreeCharge(in->displayCharge);
  FreeCharge(in->searchCharge);
  FreeCharge(in->subscriptCharge);

  for (ioc = in->otherCharges; ioc != NULL; ioc = nextoc) {
    FreeHumanString(ioc->item.forWhat);
    FreeCharge(ioc->item.charge);
    nextoc = ioc->next;
    (void) free ((char *)ioc);
  }
  (void)free(in);
}
void
FreeAccessInfo(AccessInfo *in)
{
  struct queryTypesSupported_List68 *nextqt, *iqt;
  struct diagnosticsSets_List69 *nextds, *ids;
  struct attributeSetIds_List70 *nextas, *ias;
  struct schemas_List71 *nextsc, *isc;
  struct recordSyntaxes_List72 *nextrs, *irs;
  struct resourceChallenges_List73 *nextrc, *irc;
  struct variantSets_List74 *nextvs, *ivs;
  struct elementSetNames_List75 *nextes, *ies;
  struct unitSystems_List76 *nextus, *ius;

  if (in == NULL)
    return;
 
  for (iqt = in->queryTypesSupported; iqt != NULL; iqt = nextqt) {
    FreeQueryTypeDetails(iqt->item);
    nextqt = iqt->next;
    (void) free ((char *)iqt);
  }

  for (ids = in->diagnosticsSets; ids != NULL; ids = nextds) {
    FreeOid(ids->item);
    nextds = ids->next;
    (void) free ((char *)ids);
  }

  for (ias = in->attributeSetIds; ias != NULL; ias = nextas) {
    FreeOid((ObjectIdentifier)ias->item);
    nextas = ias->next;
    (void) free ((char *)ias);
  }

  for (isc = in->schemas; isc != NULL; isc = nextsc) {
    FreeOid(isc->item);
    nextsc = isc->next;
    (void) free ((char *)isc);
  }

  for (irs = in->recordSyntaxes; irs != NULL; irs = nextrs) {
    FreeOid(irs->item);
    nextrs = irs->next;
    (void) free ((char *)irs);
  }

  for (irc = in->resourceChallenges; irc != NULL; irc = nextrc) {
    FreeOid(irc->item);
    nextrc= irc->next;
    (void) free ((char *)irc);
  }

  FreeAccessRestrictions(in->restrictedAccess);

  FreeCosts(in->costInfo);

  for (ivs = in->variantSets; ivs != NULL; ivs = nextvs) {
    FreeOid(ivs->item);
    nextvs = ivs->next;
    (void) free ((char *)ivs);
  }

  for (ies = in->elementSetNames; ies != NULL; ies = nextes) {
    FreeInternationalString((InternationalString)ies->item);
    nextes = ies->next;
    (void) free ((char *)ies);
  }

  for (ius = in->unitSystems; ius != NULL; ius = nextus) {
    FreeInternationalString(ius->item);
    nextus = ius->next;
    (void) free ((char *)ius);
  }
  (void)free(in);
}

void
FreeTargetInfo(TargetInfo *in)
{
  struct nicknames_List30 *nextnn, *inn;
  struct dbCombinations_List31 *nextdb, *idb;
  struct addresses_List32 *nextad, *iad;
  struct languages_List33 *nextln, *iln;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeInternationalString(in->name);
  FreeHumanString(in->recent_news);
  FreeIconObject(in->icon);
  FreeIntUnit(in->timeoutInterval);
  FreeHumanString(in->welcomeMessage);
  FreeContactInfo(in->contactInfo);
  FreeHumanString(in->description);

  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    FreeInternationalString(inn->item);
    nextnn = inn->next;
    (void) free ((char *)inn);
  }

  FreeHumanString(in->usage_restrictions);
  FreeHumanString(in->paymentAddr);
  FreeHumanString(in->hours);

  for (idb = in->dbCombinations; idb != NULL; idb = nextdb) {
    FreeDatabaseList(idb->item);
    nextdb = idb->next;
    (void) free ((char *)idb);
  }

  for (iad = in->addresses; iad != NULL; iad = nextad) {
    FreeNetworkAddress(iad->item);
    nextad = iad->next;
    (void) free ((char *)iad);
  }

  for (iln = in->languages; iln != NULL; iln = nextln) {
    FreeInternationalString(iln->item);
    nextln = iln->next;
    (void) free ((char *)iln);
  }

  FreeAccessInfo(in->commonAccessInfo);

  (void)free(in);
}

void
FreeDatabaseInfo(DatabaseInfo *in)
{
  struct nicknames_List34 *nextnn, *inn;
  struct keywords_List35 *nextkw, *ikw;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->name);

  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    FreeGSTR((GSTR *)inn->item);
    nextnn = inn->next;
    (void) free ((char *)inn);
  }

  FreeIconObject(in->icon);
  FreeHumanString(in->titleString);

  for (ikw = in->keywords; ikw != NULL; ikw = nextkw) {
    FreeHumanString(ikw->item);
    nextkw = ikw->next;
    (void) free ((char *)ikw);
  }

  FreeHumanString(in->description);
  FreeDatabaseList(in->associatedDbs);
  FreeDatabaseList(in->subDbs);
  FreeHumanString(in->disclaimers);
  FreeHumanString(in->news);

  FreeHumanString(in->defaultOrder);
  FreeHumanString(in->hours);
  FreeHumanString(in->bestTime);
  FreeGeneralizedTime(in->lastUpdate);
  FreeIntUnit(in->updateInterval);
  FreeHumanString(in->coverage);
  FreeHumanString(in->copyrightText);
  FreeHumanString(in->copyrightNotice);
  FreeContactInfo(in->producerContactInfo);
  FreeContactInfo(in->supplierContactInfo);
  FreeContactInfo(in->submissionContactInfo);
  FreeAccessInfo(in->accessInfo);

  (void)free(in);
}
void
FreePath(Path *in)
{
  if (in == NULL)
    return;

  FreePath(in->next);

  FreeStringOrNumeric(in->item.tagValue);

  (void)free(in);
}

void
FreeElementDataType(ElementDataType *in)
{
  struct structured_List38 *nextrs, *irs;;

  if (in == NULL)
    return;

  if (in->which != e36_primitive) {
    for (irs = in->u.structured; irs != NULL; irs = nextrs) {
      FreeElementInfo(irs->item);
      nextrs = irs->next;
      (void) free ((char *)irs);
    }
  }
  (void)free(in);
}
void
FreeElementInfo(ElementInfo *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->elementName);
  FreePath(in->elementTagPath);
  FreeElementDataType(in->dataType);
  FreeHumanString(in->description);

  (void)free(in);
}
void
FreeSchemaInfo(SchemaInfo *in)
{
  struct tagTypeMapping_List36 *nexttt, *itt;
  struct recordStructure_List37 *nextrs, *irs;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeOid(in->schema);
  FreeInternationalString(in->name);
  FreeHumanString(in->description);

  for (itt = in->tagTypeMapping; itt != NULL; itt = nexttt) {
    FreeOid(itt->item.tagSet);
    nexttt = itt->next;
    (void) free ((char *)itt);
  }

  for (irs = in->recordStructure; irs != NULL; irs = nextrs) {
    FreeElementInfo(irs->item);
    nextrs = irs->next;
    (void) free ((char *)irs);
  }
  (void)free(in);
}

void
FreeTagSetInfo(TagSetInfo *in)
{
  struct nicknames_List40 *nextnn, *inn;
  struct elements_List39 *nextel, *iel;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeOid(in->tagSet);
  FreeInternationalString(in->name);
  FreeHumanString(in->description);

  for (iel = in->elements; iel != NULL; iel = nextel) {
    FreeInternationalString(iel->item.elementname);

    for (inn = iel->item.nicknames; inn != NULL; inn = nextnn) {
      FreeInternationalString(inn->item);
      nextnn = inn->next;
      (void) free ((char *)iel);
    }
    FreeStringOrNumeric(iel->item.elementTag);
    FreeHumanString(iel->item.description);
    FreeOtherInformation(iel->item.otherTagInfo);

    nextel = iel->next;
  }
  (void)free(in);
}

void
FreeRecordSyntaxInfo(RecordSyntaxInfo *in)
{
  struct transferSyntaxes_List41 *nextts, *its;
  struct abstractStructure_List42 *nextas, *ias;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeOid(in->recordSyntax);
  FreeInternationalString(in->name);

  for (its = in->transferSyntaxes; its != NULL; its = nextts) {
    FreeOid(its->item);
    nextts = its->next;
    (void) free ((char *)its);
  }

  FreeHumanString(in->description);
  FreeInternationalString(in->asn1Module);

  for (ias = in->abstractStructure; ias != NULL; ias = nextas) {
    FreeElementInfo(ias->item);
    nextas = ias->next;
    (void) free ((char *)ias);
  }
  (void)free(in);
}
void
FreeAttributeDescription(AttributeDescription *in)
{
  struct equivalentAttributes_List45 *nextat, *iat;

  if (in == NULL)
    return;

  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeStringOrNumeric(in->attributeValue);

  for (iat = in->equivalentAttributes; iat != NULL; iat = nextat) {
    FreeStringOrNumeric(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}
void
FreeAttributeType(AttributeType *in)
{
  struct attributeValues_List44	*nextat, *iat;

  if (in == NULL)
    return;

  FreeInternationalString(in->name);
  FreeHumanString(in->description);

  for (iat = in->attributeValues; iat != NULL; iat = nextat) {
    FreeAttributeDescription(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}
void
FreeAttributeSetInfo(AttributeSetInfo *in)
{
  struct attributes_List43 *nextat, *iat;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeOid(in->attributeSet);
  FreeInternationalString(in->name);

  for (iat = in->attributes; iat != NULL; iat = nextat) {
    FreeAttributeType(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }

  FreeHumanString(in->description);

  (void)free(in);
}

void
FreeTermListInfo(TermListInfo *in)
{
  struct termLists_List46 *nexttl, *itl;
  struct broader_List47 *nextbl, *ibl;
  struct narrower_List48 *nextnl, *inl;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);

  for (itl = in->termLists; itl != NULL; itl = nexttl) {
    FreeInternationalString(itl->item.name);
    FreeHumanString(itl->item.title);
  
    for (ibl = itl->item.broader; ibl != NULL; ibl = nextbl) {
      FreeInternationalString(ibl->item);
      nextbl = ibl->next;
      (void) free ((char *)itl);
    }
  

  
    for (inl = itl->item.narrower; inl != NULL; inl = nextnl) {
      FreeInternationalString(inl->item);
      nextnl = inl->next;
      (void) free ((char *)inl);
    }
    nexttl = itl->next;
  }
  (void)free(in);
}

void
FreeExtendedServicesInfo(ExtendedServicesInfo *in)
{
  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeOid(in->type);
  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeExternal(in->specificExplain);
  FreeInternationalString(in->esASN);
 
  (void)free(in);
}

void
FreeOmittedAttributeInterpretation(OmittedAttributeInterpretation *in)
{
  if (in == NULL)
    return;

  FreeStringOrNumeric(in->defaultValue);
  FreeHumanString(in->defaultDescription);

  (void)free(in);
}

void
FreeAttributeValue(AttributeValue *in)
{
  struct subAttributes_List52  *nextat1, *iat1;
  struct superAttributes_List53 *nextat2, *iat2;

  if (in == NULL)
    return;

  FreeStringOrNumeric(in->value);
  FreeHumanString(in->description);

  for (iat1 = in->subAttributes; iat1 != NULL; iat1 = nextat1) {
    FreeStringOrNumeric(iat1->item);
    nextat1 = iat1->next;
    (void) free ((char *)iat1);
  }

  for (iat2 = in->superAttributes; iat2 != NULL; iat2 = nextat2) {
    FreeStringOrNumeric(iat2->item);
    nextat2 = iat2->next;
    (void) free ((char *)iat2);
  }
  (void)free(in);
}

void
FreeAttributeTypeDetails(AttributeTypeDetails *in)
{
  struct attributeValues_List51 *nextat, *iat;

  if (in == NULL)
    return;

  FreeOmittedAttributeInterpretation(in->defaultIfOmitted);

  for (iat = in->attributeValues; iat != NULL; iat = nextat) {
    FreeAttributeValue(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}
void
FreeAttributeSetDetails(AttributeSetDetails *in)
{
  struct attributesByType_List50 *nextat, *iat;

  if (in == NULL)
    return;

  FreeOid(in->attributeSet);

  for (iat = in->attributesByType; iat != NULL; iat = nextat) {
    FreeAttributeTypeDetails(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}
void
FreeAttributeOccurrence(AttributeOccurrence *in)
{
  struct specific_List86 *nextat, *iat;
  if (in == NULL)
    return;

  FreeOid(in->attributeSet);

  if (in->attributeValues.which != e44_any_or_none) {

  
    for (iat = in->attributeValues.u.specific; iat != NULL; iat = nextat) {
      FreeStringOrNumeric(iat->item);
      nextat = iat->next;
      (void) free ((char *)iat);
    }
  }
  (void)free(in);
}

void
FreeAttributeCombination(AttributeCombination *in)
{
  if (in == NULL)
    return;

  FreeAttributeCombination(in->next);
  FreeAttributeOccurrence(in->item);

  (void)free(in);
}
void
FreeAttributeCombinations(AttributeCombinations *in)
{
  struct legalCombinations_List85 *nextat, *iat;

  if (in == NULL)
    return;

  FreeOid(in->defaultAttributeSet);

  for (iat = in->legalCombinations; iat != NULL; iat = nextat) {
    FreeAttributeCombination(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}
void
FreeAttributeDetails(AttributeDetails *in)
{
  struct attributesBySet_List49 *nextat, *iat;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);

  for (iat = in->attributesBySet; iat != NULL; iat = nextat) {
    FreeAttributeSetDetails(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  FreeAttributeCombinations(in->attributeCombinations);
  (void)free(in);
}

void
FreeTermListDetails(TermListDetails *in)
{
  struct sampleTerms_List54  *nextat, *iat;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeInternationalString(in->termListName);
  FreeHumanString(in->description);
  FreeAttributeCombinations(in->attributes);

  if (in->scanInfo) {
    FreeHumanString(in->scanInfo->collatingSequence);
    (void)free(in->scanInfo);
  }

  for (iat = in->sampleTerms; iat != NULL; iat = nextat) {
    FreeTerm(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}

void
FreeRecordTag(RecordTag *in)
{
  if (in == NULL)
    return;
  FreeStringOrNumeric(in->qualifier);
  FreeStringOrNumeric(in->tagValue);

  (void)free(in);
}
void
FreePerElementDetails(PerElementDetails *in)
{
  struct schemaTags_List57 *nextst, *ist;
  struct alternateNames_List58 *nextalt, *ialt;
  struct genericNames_List59 *nextgen, *igen;

  if (in == NULL)
    return;
 
  FreeInternationalString (in->name);
  FreeRecordTag (in->recordTag);

  for (ist = in->schemaTags; ist != NULL; ist = nextst) {
    FreePath(ist->item);
    nextst = ist->next;
    (void) free ((char *)ist);
  }

  FreeHumanString (in->description);
  FreeHumanString (in->contents);
  FreeHumanString (in->billingInfo);
  FreeHumanString (in->restrictions);

  for (ialt = in->alternateNames; ialt != NULL; ialt = nextalt) {
    FreeInternationalString(ialt->item);
    nextalt = ialt->next;
    (void) free ((char *)ialt);
  }


  for (igen = in->genericNames; igen != NULL; igen = nextgen) {
    FreeInternationalString(igen->item);
    nextgen = igen->next;
    (void) free ((char *)igen);
  }

  FreeAttributeCombinations(in->searchAccess);

  (void)free(in);
}

void
FreeElementSetDetails(ElementSetDetails *in)
{
  struct detailsPerElement_List55   *nextat, *iat;

  if (in == NULL)
    return;
 
  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);
  FreeInternationalString((InternationalString)in->elementSetName);
  FreeOid(in->recordSyntax);
  FreeOid(in->schema);
  FreeHumanString(in->description);

  for (iat = in->detailsPerElement; iat != NULL; iat = nextat) {
    FreePerElementDetails(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}

void
FreeRetrievalRecordDetails(RetrievalRecordDetails *in)
{
  struct detailsPerElement_List56  *nextat, *iat;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);
  FreeOid(in->recordSyntax);
  FreeOid(in->schema);
  FreeHumanString(in->description);

  for (iat = in->detailsPerElement; iat != NULL; iat = nextat) {
    FreePerElementDetails(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}

void
FreeSpecification(Specification *in)
{
  if (in == NULL)
    return;

  FreeOid(in->schema);

  if (in->elementSpec != NULL) {
    if (in->elementSpec->which == e16_elementSetName)
      FreeInternationalString(in->elementSpec->u.elementSetName);
    else
      FreeExternal(in->elementSpec->u.externalEspec);
  }
  (void)free(in);
}

void
FreeSortKeyDetails(SortKeyDetails *in)
{
  struct elementSpecifications_List61  *nextat, *iat;

  if (in == NULL)
    return;

  FreeHumanString(in->description);

  for (iat = in->elementSpecifications; iat != NULL; iat = nextat) {
    FreeSpecification(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }

  FreeAttributeCombinations(in->attributeSpecifications);

  if (in->sortType != NULL) {
    switch (in->sortType->which) {
    case e37_character: 
    case e37_numeric:
      break;
    case e37_structured:
      FreeHumanString(in->sortType->u.structured);
      break;
    }
    (void)free(in->sortType);
  }
  (void)free(in);
}
void
FreeSortDetails(SortDetails *in)
{
  struct sortKeys_List60    *nextat, *iat;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);

  for (iat = in->sortKeys; iat != NULL; iat = nextat) {
    FreeSortKeyDetails(iat->item);
    nextat = iat->next;
    (void) free ((char *)iat);
  }
  (void)free(in);
}

void
FreeProcessingInformation(ProcessingInformation *in)
{
  if (in == NULL)
    return;
 
  FreeCommonInfo(in->commonInfo);
  FreeGSTR((GSTR *)in->databaseName);
  FreeInternationalString(in->name);
  FreeOid(in->oid);
  FreeHumanString(in->description);
  FreeExternal(in->instructions);

  (void)free(in);
}
void
FreeValueDescription(ValueDescription *in)
{
  if (in == NULL)
    return;

  switch (in->which) {
  case e39_integer:
    break;
  case e39_string:
    FreeInternationalString(in->u.string);
    break;
  case e39_octets:
    FreeOctetString(in->u.octets);
    break;
  case e39_oid:
    FreeOid(in->u.oid);
    break;
  case e39_unit:
    FreeUnit(in->u.unit);
    break;
  case e39_valueAndUnit:
    FreeIntUnit(in->u.valueAndUnit);
    break;
  }
  (void)free(in);
}

void
FreeValueRange(ValueRange *in)
{
  if (in == NULL)
    return;

  FreeValueDescription(in->lower);
  FreeValueDescription(in->upper);
  
  (void)free(in);
}

void
FreeValueSet(ValueSet *in)
{
  struct enumerated_List64 *nextvr, *ivr;

  if (in == NULL)
    return;

  if (in->which == e38_range)
    FreeValueRange(in->u.range);
  else {
    for (ivr = in->u.enumerated; ivr != NULL; ivr = nextvr) {
      FreeValueDescription(ivr->item);
      nextvr = ivr->next;
      (void) free ((char *)ivr);
    }
  }
  (void)free(in);
}

void
FreeVariantValue(VariantValue *in)
{
  if (in == NULL)
    return;

  FreeValueSet(in->values);
  (void)free(in);
}

void
FreeVariantType(VariantType *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeVariantValue(in->variantValue);
  (void)free(in);
}

void
FreeVariantClass(VariantClass *in)
{
  struct variantTypes_List63 *nextvr, *ivr;

  if (in == NULL)
    return;

  FreeInternationalString(in->name);
  FreeHumanString(in->description);

  for (ivr = in->variantTypes; ivr != NULL; ivr = nextvr) {
    FreeVariantType(ivr->item);
    nextvr = ivr->next;
    (void) free ((char *)ivr);
  }
  (void)free(in);
}
void
FreeVariantSetInfo(VariantSetInfo *in)
{
  struct variants_List62 *nextvr, *ivr;

  if (in == NULL)
    return;
 
  FreeCommonInfo(in->commonInfo);
  FreeInternationalString(in->name);
  FreeOid(in->variantSet);

  for (ivr = in->variants; ivr != NULL; ivr = nextvr) {
    FreeVariantClass(ivr->item);
    nextvr = ivr->next;
    (void) free ((char *)ivr);
  }
  (void)free(in);
}

void
FreeUnits(Units *in)
{
  if (in == NULL)
    return;
 
  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeStringOrNumeric(in->unit);

  (void)free(in);
}
 

void
FreeUnitType(UnitType *in)
{
  struct units_List66 *nextvr, *ivr;;

  if (in == NULL)
    return;
 
  FreeInternationalString(in->name);
  FreeHumanString(in->description);
  FreeStringOrNumeric(in->unitType);

  for (ivr = in->units; ivr != NULL; ivr = nextvr) {
    FreeUnits(ivr->item);
    nextvr = ivr->next;
    (void) free ((char *)ivr);
  }
  (void)free(in);
}
void
FreeUnitInfo(UnitInfo *in)
{
  struct units_List65 *nextvr, *ivr;;

  if (in == NULL)
    return;

  FreeCommonInfo(in->commonInfo);
  FreeInternationalString(in->unitSystem);
  FreeHumanString(in->description);

  for (ivr = in->units; ivr != NULL; ivr = nextvr) {
    FreeUnitType(ivr->item);
    nextvr = ivr->next;
    (void) free ((char *)ivr);
  }
  (void)free(in);
}
void
FreeCategoryInfo(CategoryInfo *in)
{
  if (in == NULL)
    return;

  FreeInternationalString(in->category);
  FreeInternationalString(in->originalCategory);
  FreeHumanString(in->description);
  FreeInternationalString(in->asn1Module);

  (void)free(in);
}
void
FreeCategoryList(CategoryList *in)
{
  struct categories_List67 *nextvr, *ivr;

  if (in == NULL)
    return;
 
  FreeCommonInfo(in->commonInfo);

  for (ivr = in->categories; ivr != NULL; ivr = nextvr) {
    FreeCategoryInfo(ivr->item);
    nextvr = ivr->next;
    (void) free ((char *)ivr);
  }

  (void)free(in);
}
void
FreeExplain_Record(Explain_Record *in)
{
  if (in == NULL)
    return;

  switch (in->which) {
  case e34_targetInfo:
    FreeTargetInfo(in->u.targetInfo);
    break;
  case e34_databaseInfo:
    FreeDatabaseInfo(in->u.databaseInfo);
    break;
  case e34_schemaInfo:
    FreeSchemaInfo(in->u.schemaInfo);
    break;
  case e34_tagSetInfo:
    FreeTagSetInfo(in->u.tagSetInfo);
    break;
  case e34_recordSyntaxInfo:
    FreeRecordSyntaxInfo(in->u.recordSyntaxInfo);
    break;
  case e34_attributeSetInfo:
    FreeAttributeSetInfo(in->u.attributeSetInfo);
    break;
  case e34_termListInfo:
    FreeTermListInfo(in->u.termListInfo);
    break;
  case e34_extendedServicesInfo:
    FreeExtendedServicesInfo(in->u.extendedServicesInfo);
    break;
  case e34_attributeDetails:
    FreeAttributeDetails(in->u.attributeDetails);
    break;
  case e34_termListDetails:
    FreeTermListDetails(in->u.termListDetails);
    break;
  case e34_elementSetDetails:
    FreeElementSetDetails(in->u.elementSetDetails);
    break;
  case e34_retrievalRecordDetails:
    FreeRetrievalRecordDetails(in->u.retrievalRecordDetails);
    break;
  case e34_sortDetails:
    FreeSortDetails(in->u.sortDetails);
    break;
  case e34_processing:
    FreeProcessingInformation(in->u.processing);
    break;
  case e34_variants:
    FreeVariantSetInfo(in->u.variants);
    break;
  case e34_units:
    FreeUnitInfo(in->u.units);
    break;
  case e34_categoryList:
    FreeCategoryList(in->u.categoryList);
    break;

  }
  (void)free(in);
}

void
FreeNonSurrogateDiagnostic (dr)
DefaultDiagFormat *dr;
{
  if (dr == NULL)
    return;

  FreeOid (dr->diagnosticSetId);
  FreeOctetString (dr->addinfo.u.v3Addinfo);
  (void)free(dr);
}

void
FreeDefaultDiagFormat(DefaultDiagFormat *in)
{
  FreeNonSurrogateDiagnostic(in);
}

void
FreeESRecord(TaskPackage *in)
{
  struct packageDiagnostics_List95 *nextpd, *ipd;

  if (in == NULL)
    return;

  FreeOid(in->packageType);
  FreeInternationalString(in->packageName);
  FreeInternationalString(in->userId);
  FreeIntUnit(in->retentionTime);
  FreePermissions(in->permissions);
  FreeInternationalString(in->description);
  FreeOctetString(in->targetReference);
  FreeGeneralizedTime(in->creationDateTime);

  for (ipd = in->packageDiagnostics; ipd != NULL; ipd = nextpd) {
    FreeDiagRec(ipd->item);
    nextpd = ipd->next;
    (void) free ((char *)ipd);
  }

  FreeExternal(in->taskSpecificParameters);

  (void)free(in);
}


void
FreeTaskPackage(TaskPackage *in)
{
  FreeESRecord(in);
}

