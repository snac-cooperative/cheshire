
#include "Python.h"
#include "z3950_3.h"
#include "session.h"
#include "z3950_intern.h"
#include "z_parameters.h"
#include "list.h"

/* List of active Z sessions, current user session ptr */
/* These do not get reinitialised if the user tries to import */
/* the module more than once */

ZASSOCIATION zAssociations = {NULL, NULL, 0, 0 ,0, 0, 0, 0};
ZSESSION *TKZ_User_Session = NULL;
ZSESSION *TKZ_User_Session_TMP = NULL;
FILE *LOGFILE = NULL;   /* Shared logfile pointer */

/* Has the resultset changed since last present */
int result_set_changed = 0;
/* Last record processed */
int last_rec_proc = 0;

void ClearPreviousSearch();
ZSESSION *tk_reconnect();

/* Settable Global Parameters */
char ZCh_defaultDatabase[200] = "Default";


// ZCheshire specific error
static PyObject *ZCh_ErrorObject;

/* NB: GetOctetString returns char*, so strdup it to return in Py_BuildValue */

/* GRS1 stuff */
/* Forward declarations */
PyObject* buildTaggedElement();
PyObject* buildStringOrNumeric();
PyObject* buildElementData();
int isMetaDataElement();

PyObject*
buildLanguageCode(LanguageCode *in)
{
  PyObject *entry;
  GSTR *glang;

  if (in == NULL) 
    return Py_BuildValue("s", "unknown");
  
  glang = (GSTR *)in;
  entry = Py_BuildValue("s", glang->data);

  return entry;

}

PyObject*
buildHumanString(HumanString *in)
{
  /* Dictionary of language to string */
  PyObject *entry = PyDict_New();
  PyObject *data;
  PyObject *language;
  /* Need to recast language as GSTR to access data (Why??)*/

  if (in == NULL)
    return entry;

  data = Py_BuildValue("s", in->item.text->data);
  language = buildLanguageCode(in->item.language);

  PyDict_SetItem(entry, language, data);

  while (in->next) {
    in = in->next;
    data = Py_BuildValue("s", in->item.text->data);
    language = buildLanguageCode(in->item.language);
    PyDict_SetItem(entry, language, data);
  }

  return entry;

}

PyObject*
buildUnit(Unit *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *value;

  if (in->unitSystem != NULL) {
    nameStr = Py_BuildValue("s", "unitSystem");
    value = Py_BuildValue("s", in->unitSystem->data);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->scaleFactor != NULL) {
    nameStr = Py_BuildValue("s", "scaleFactor");
    value = Py_BuildValue("i", in->scaleFactor);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->unitType != NULL) {
    nameStr = Py_BuildValue("s", "unitType");
    value = buildStringOrNumeric(in->unitType);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->unit != NULL) {
    nameStr = Py_BuildValue("s", "unit");
    value = buildStringOrNumeric(in->unit);
    PyDict_SetItem(entry, nameStr, value);
  }

  return entry;
}


PyObject*
buildIntUnit(IntUnit *in)
{
  /* Tuple of Int and Unit. Unit is dictionary */

  PyObject *entry = PyList_New(0);
  PyObject *unitDict;

  if (in == NULL) 
    return entry;

  PyList_Append(entry, Py_BuildValue("i", in->value));
  unitDict = buildUnit(in->unitUsed);
  PyList_Append(entry, unitDict);

  return entry;
}

PyObject *
buildContactInfo(ContactInfo *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *value;

  if (in == NULL)
    return entry;

  if (in->name != NULL) {
    nameStr = Py_BuildValue("s", "name");
    value = Py_BuildValue("s", in->name->data);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->email != NULL) {
    nameStr = Py_BuildValue("s", "email");
    value = Py_BuildValue("s", in->email->data);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->phone != NULL) {
    nameStr = Py_BuildValue("s", "phone");
    value = Py_BuildValue("s", in->phone->data);
    PyDict_SetItem(entry, nameStr, value);
  }

  if (in->description != NULL) {
    nameStr = Py_BuildValue("s", "description");
    value = buildHumanString(in->description);
    PyDict_SetItem(entry, nameStr, value);
  }
    
  if (in->address != NULL) {
    nameStr = Py_BuildValue("s", "address");
    value = buildHumanString(in->address);
    PyDict_SetItem(entry, nameStr, value);
  }

  return entry;

}

PyObject*
buildNetworkAddress(NetworkAddress *in)
{
  PyObject *entry = PyList_New(0);
  PyObject *addrDict;

  switch (in->which) {
  case e41_internetAddress:
    PyList_Append(entry, Py_BuildValue("s", "internetAddress"));
    addrDict = Py_BuildValue("{s:s,s:i}", "host", in->u.internetAddress.hostAddress->data, "port", in->u.internetAddress.port);
    PyList_Append(entry, addrDict);
    break;
  case e41_osiPresentationAddress:
    PyList_Append(entry, Py_BuildValue("s", "osiAddress"));
    addrDict = Py_BuildValue("{s:s,s:s,s:s,s:s}", "pSel", in->u.osiPresentationAddress.pSel->data, "sSel", in->u.osiPresentationAddress.sSel->data, "tSel", in->u.osiPresentationAddress.tSel->data, "nSap", in->u.osiPresentationAddress.nSap->data);
    PyList_Append(entry, addrDict);
    break;
  case e41_other:
    PyList_Append(entry, Py_BuildValue("s", "otherAddress"));
    addrDict = Py_BuildValue("{s:s,s:s}", "type", in->u.other.type->data, "address", in->u.other.address->data);
    PyList_Append(entry, addrDict);
    break;
  }
  return entry;

}

PyObject*
buildPrivateCapabilities(PrivateCapabilities *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *valueList;
  PyObject *value;
  PyObject *valueDict;
  PyObject *valueName;
  struct operators_List77 *nextop, *iop;
  struct searchKeys_List78 *nextsk, *isk;
  struct description_List79 *nextds, *ids;

  nameStr = Py_BuildValue("s", "operators");
  valueList = PyList_New(0);
  for (iop = in->operators; iop != NULL; iop = nextop) {
    valueDict = PyDict_New();
    valueName = Py_BuildValue("s", "operator");
    value = Py_BuildValue("s", iop->item.operator_var->data);
    PyDict_SetItem(valueDict, valueName, value);

    valueName = Py_BuildValue("s", "description");
    value = buildHumanString(iop->item.description);
    PyDict_SetItem(valueDict, valueName, value);

    PyList_Append(valueList, valueDict);
    nextop = iop->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  nameStr = Py_BuildValue("s", "searchKeys");
  valueList = PyList_New(0);
  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    valueDict = PyDict_New();
    valueName = Py_BuildValue("s", "searchKey");
    value = Py_BuildValue("s", isk->item->searchKey->data);
    PyDict_SetItem(valueDict, valueName, value);

    valueName = Py_BuildValue("s", "description");
    value = buildHumanString(isk->item->description);
    PyDict_SetItem(valueDict, valueName, value);

    PyList_Append(valueList, valueDict);
    nextsk = isk->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  nameStr = Py_BuildValue("s", "descriptions");
  valueList = PyList_New(0);
  for (ids = in->description; ids != NULL; ids = nextds) {
    PyList_Append(valueList, buildHumanString(ids->item));
    nextds = ids->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  return entry;
}

PyObject*
buildProximitySupport(ProximitySupport *in)
{
   struct unitsSupported_List82 *nextus, *ius;
   PyObject *entry = Py_BuildValue("{s:i}", "anySupport", in->anySupport);
   PyObject *nameStr;
   PyObject *valueList = PyList_New(0);
   PyObject *value;
   PyObject *valueName;
   PyObject *description;

   nameStr = Py_BuildValue("s", "unitsSupported");
   for (ius = in->unitsSupported; ius != NULL; ius = nextus) {
     if (ius->item != NULL) {
       if (ius->item->which == e43_known) {
	 value = Py_BuildValue("{s:i}", "known", ius->item->u.known);
       } else {
	 value = Py_BuildValue("{s:i}", "unit", ius->item->u.private_var.unit);
	 valueName = Py_BuildValue("s", "description");
	 description = buildHumanString(ius->item->u.private_var.description);
	 PyDict_SetItem(value, valueName, description);
       }
       PyList_Append(valueList, value);
     }
     nextus = ius->next;
   }
   PyDict_SetItem(entry, nameStr, valueList);
   
   return entry;

}

PyObject*
buildRpnCapabilities(RpnCapabilities *in)
{
  struct operators_List80 *nextop, *iop;
  PyObject *entry;
  PyObject *nameStr;
  PyObject *valueList;
  
  entry = Py_BuildValue("{s:i,s:i}", "resultSetAsOperand", in->resultSetAsOperandSupported, "restrictionOperand", in->restrictionOperandSupported);
  
  nameStr = Py_BuildValue("s", "operators");
  valueList = PyList_New(0);
  for (iop = in->operators; iop != NULL; iop = nextop) {
    PyList_Append(valueList, Py_BuildValue("i", iop->item));
    nextop = iop->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  nameStr = Py_BuildValue("s", "proximity");
  valueList = buildProximitySupport(in->proximity);
  PyDict_SetItem(entry, nameStr, valueList);

  return entry;

}

PyObject*
buildIso8777Capabilities(Iso8777Capabilities *in)
{
  PyObject *entry = PyDict_New();
  struct searchKeys_List81 *nextsk, *isk;
  PyObject *nameStr;
  PyObject *valueList = PyList_New(0);
  PyObject *valueDict;
  PyObject *valueName;
  PyObject *value;

  nameStr = Py_BuildValue("s", "searchKeys");
  for (isk = in->searchKeys; isk != NULL; isk = nextsk) {
    valueDict = PyDict_New();
    valueName = Py_BuildValue("s", "searchKey");
    value = Py_BuildValue("s", isk->item->searchKey->data);
    PyDict_SetItem(valueDict, valueName, value);

    valueName = Py_BuildValue("s", "description");
    value = buildHumanString(isk->item->description);
    PyDict_SetItem(valueDict, valueName, value);

    PyList_Append(valueList, valueDict);
    
    nextsk = isk->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);
 
  nameStr = Py_BuildValue("s", "restrictions");
  value = buildHumanString(in->restrictions);
  PyDict_SetItem(entry, nameStr, value);

  return entry;

}

PyObject*
buildQueryTypeDetails(QueryTypeDetails *in)
{
  PyObject *entry = PyList_New(0);

  switch (in->which) {
  case e42_private:
    PyList_Append(entry, Py_BuildValue("s", "Type0"));
    PyList_Append(entry, buildPrivateCapabilities(in->u.private_var));
    break;
  case e42_rpn:
    PyList_Append(entry, Py_BuildValue("s", "Type1"));
    PyList_Append(entry, buildRpnCapabilities(in->u.rpn));
    break;
  case e42_iso8777:
    PyList_Append(entry, Py_BuildValue("s", "Type2"));
    PyList_Append(entry, buildIso8777Capabilities(in->u.iso8777));
    break;
  case e42_z39_58:
    PyList_Append(entry, Py_BuildValue("s", "Type100"));
    PyList_Append(entry, buildHumanString(in->u.z39_58));
    break;
  case e42_erpn:
    PyList_Append(entry, Py_BuildValue("s", "Type101"));
    PyList_Append(entry, buildRpnCapabilities(in->u.erpn));
    break;
  case e42_rankedList:
    PyList_Append(entry, Py_BuildValue("s", "Type102"));
    PyList_Append(entry, buildHumanString(in->u.rankedList));
    break;
  }
  return entry;
}

PyObject*
buildAccessInfo(AccessInfo *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *valueList;
  PyObject *value;

  struct queryTypesSupported_List68 *nextqt, *iqt;
  struct diagnosticsSets_List69 *nextds, *ids;
  struct attributeSetIds_List70 *nextas, *ias;
  struct schemas_List71 *nextsc, *isc;
  struct recordSyntaxes_List72 *nextrs, *irs;
  struct resourceChallenges_List73 *nextrc, *irc;
  struct variantSets_List74 *nextvs, *ivs;
  struct elementSetNames_List75 *nextes, *ies;
  struct unitSystems_List76 *nextus, *ius;


  /* Query Types */
  nameStr = Py_BuildValue("s", "queryTypes");
  valueList = PyList_New(0);
  for (iqt = in->queryTypesSupported; iqt != NULL; iqt = nextqt) {
    value = buildQueryTypeDetails(iqt->item);
    PyList_Append(valueList, value);
    nextqt = iqt->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* Diagnostics Sets */
  nameStr = Py_BuildValue("s", "diagnosticsSets");
  valueList = PyList_New(0);
  for (ids = in->diagnosticsSets; ids != NULL; ids = nextds) {
    value = Py_BuildValue("s", ids->item->data);
    PyList_Append(valueList, value);
    nextds = ids->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* Attribute Sets */
  nameStr = Py_BuildValue("s", "attributeSets");
  valueList = PyList_New(0);
  for (ias = in->attributeSetIds; ias != NULL; ias = nextas) {
    value = Py_BuildValue("s", ias->item->data);
    PyList_Append(valueList, value);
    nextas = ias->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* Schemas */
  nameStr = Py_BuildValue("s", "schemas");
  valueList = PyList_New(0);
  for (isc = in->schemas; isc != NULL; isc = nextsc) {
    value = Py_BuildValue("s", isc->item->data);
    PyList_Append(valueList, value);
    nextsc = isc->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* RecordSyntaxes */
  nameStr = Py_BuildValue("s", "recordSyntaxes");
  valueList = PyList_New(0);
  for (irs = in->recordSyntaxes; irs != NULL; irs = nextrs) {
    value = Py_BuildValue("s", irs->item->data);
    PyList_Append(valueList, value);
    nextrs = irs->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* Resource Challenges */
  nameStr = Py_BuildValue("s", "resourceChallenges");
  valueList = PyList_New(0);
  for (irc = in->resourceChallenges; irc != NULL; irc = nextrc) {
    value = Py_BuildValue("s", irc->item->data);
    PyList_Append(valueList, value);
    nextrc = irc->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* Variant Sets */
  nameStr = Py_BuildValue("s", "variantSets");
  valueList = PyList_New(0);
  for (ivs = in->variantSets; ivs != NULL; ivs = nextvs) {
    value = Py_BuildValue("s", ivs->item->data);
    PyList_Append(valueList, value);
    nextsc = ivs->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* ElementSetNames */
  nameStr = Py_BuildValue("s", "elementSetNames");
  valueList = PyList_New(0);
  for (ies = in->elementSetNames; ies != NULL; ies = nextes) {
    value = Py_BuildValue("s", ies->item->data);
    PyList_Append(valueList, value);
    nextes = ies->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* UnitSystems */
  nameStr = Py_BuildValue("s", "unitSystems");
  valueList = PyList_New(0);
  for (ius = in->unitSystems; ius != NULL; ius = nextus) {
    value = Py_BuildValue("s", ius->item->data);
    PyList_Append(valueList, value);
    nextus = ius->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  /* AccessRestrictions */

  /* Costs */

  return entry;
}

PyObject*
buildDatabaseList(DatabaseList *in)
{
  PyObject *entry = PyList_New(0);
  if (in == NULL)
    return entry;

  PyList_Append(entry, Py_BuildValue("s", in->item->data));
  if (in->next != NULL) 
    PyList_Append(entry, buildDatabaseList(in->next));

  return entry;

}

PyObject*
buildCommonInfo(CommonInfo *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *value;

  if (in == NULL) 
    return entry;

  if (in->dateAdded != NULL) {
    nameStr = Py_BuildValue("s", "dateAdded");
    value = Py_BuildValue("s", in->dateAdded->data);
    PyDict_SetItem(entry, nameStr, value);
  }
  if (in->dateChanged != NULL) {
    nameStr = Py_BuildValue("s", "dateChanged");
    value = Py_BuildValue("s", in->dateChanged->data);
    PyDict_SetItem(entry, nameStr, value);
  }
  if (in->expiry != NULL) {
    nameStr = Py_BuildValue("s", "expiry");
    value = Py_BuildValue("s", in->expiry->data);
    PyDict_SetItem(entry, nameStr, value);
  }
  if (in->humanString_Language != NULL) {
    nameStr = Py_BuildValue("s", "language");
    value = buildLanguageCode(in->humanString_Language);
    PyDict_SetItem(entry, nameStr, value);
  }

  return entry;

}


PyObject*
buildDatabaseInfo(DatabaseInfo *in)
{
  PyObject *entry, *nameStr, *value;
  struct nicknames_List34 *nextnn, *inn;
  struct keywords_List35 *nextkw, *ikw;


  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null DatabaseInfo.");
    return (PyObject*) NULL;
  }

  entry = Py_BuildValue("{s:s,s:i,s:i,s:i,s:i,s:i,s:i}", "databaseName", in->name->data, "explainDatabase", in->explainDatabase, "userFee", in->user_fee, "available", in->available, "averageRecordSize", in->avRecordSize, "maxRecordSize", in->maxRecordSize, "proprietary", in->proprietary);

  nameStr = Py_BuildValue("s", "title");
  value = buildHumanString(in->titleString);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "description");
  value = buildHumanString(in->description);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "disclaimers");
  value = buildHumanString(in->disclaimers);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "news");
  value = buildHumanString(in->news);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "defaultOrder");
  value = buildHumanString(in->defaultOrder);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "hours");
  value = buildHumanString(in->hours);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "bestTime");
  value = buildHumanString(in->bestTime);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "coverage");
  value = buildHumanString(in->coverage);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "copyrightText");
  value = buildHumanString(in->copyrightText);
  PyDict_SetItem(entry, nameStr, value);
  
  nameStr = Py_BuildValue("s", "copyrightNotice");
  value = buildHumanString(in->copyrightNotice);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "lastUpdate");
  if (in->lastUpdate) {
    value = Py_BuildValue("s", in->lastUpdate->data);
  } else {
    value = Py_BuildValue("s", "");
  }
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "updateInterval");
  value = buildIntUnit(in->updateInterval);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "associatedDatabases");
  value = buildDatabaseList(in->associatedDbs);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "subDatabases");
  value = buildDatabaseList(in->subDbs);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "producerContactInfo");
  value = buildContactInfo(in->producerContactInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "supplierContactInfo");
  value = buildContactInfo(in->supplierContactInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "submissionContactInfo");
  value = buildContactInfo(in->submissionContactInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "accessInfo");
  value = buildAccessInfo(in->accessInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "commonInfo");
  value = buildCommonInfo(in->commonInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "recordCount");
  if (in->recordCount) {
    if (in->recordCount->which == e35_actualNumber) {
      value = Py_BuildValue("{s:s,s:i}", "type", "actual", "number", in->recordCount->u.actualNumber);
    } else {
      value = Py_BuildValue("{s:s,s:i}", "type", "approx", "number", in->recordCount->u.approxNumber);
    }
  } else {
    value = Py_BuildValue("{s:s}", "type", "notSupplied");
  }
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "nickNames");
  value = PyList_New(0);
  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    PyList_Append(value, Py_BuildValue("s", inn->item->data));
    nextnn = inn->next;
  }
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "keywords");
  value = PyList_New(0);
  for (ikw = in->keywords; ikw != NULL; ikw = nextkw) {
    PyList_Append(value, buildHumanString(ikw->item));
    nextkw = ikw->next;
  }
  PyDict_SetItem(entry, nameStr, value);

  return entry;

}

PyObject*
buildSchemaInfo(SchemaInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null SchemaInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildTagSetInfo(TagSetInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null TagSetInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildRecordSyntaxInfo(RecordSyntaxInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null RecordSyntaxInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildAttributeSetInfo(AttributeSetInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null AttributeSetInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildTermListInfo(TermListInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null TermListInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildExtendedServicesInfo(ExtendedServicesInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null ExtendedServicesInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}




PyObject*
buildAttributeOccurrence(AttributeOccurrence *in)
{
  struct specific_List86 *nextat, *iat;
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value, *valueList;
  
  if (in == NULL)
    return entry;

  nameStr = Py_BuildValue("s", "attributeSet");
  if (in->attributeSet) {
    value = Py_BuildValue("s", in->attributeSet->data);
  } else {
    value = Py_BuildValue("s", "");
  }
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "attributeType");
  value = Py_BuildValue("i", in->attributeType);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "mustBeSupplied");
  value = Py_BuildValue("i", in->mustBeSupplied);
  PyDict_SetItem(entry, nameStr, value);


  if (in->attributeValues.which == e44_any_or_none) {
    nameStr = Py_BuildValue("s", "anyOrNone");
    value = Py_BuildValue("i", in->attributeValues.u.any_or_none);
    PyDict_SetItem(entry, nameStr, value);
  } else {
    nameStr = Py_BuildValue("s", "specific");
    valueList = PyList_New(0);
    for (iat = in->attributeValues.u.specific; iat != NULL; iat = nextat) {
      value = buildStringOrNumeric(iat->item);
      PyList_Append(valueList, value);
      nextat = iat->next;
    }
    PyDict_SetItem(entry, nameStr, valueList);
  }

  return entry;
}

PyObject*
buildAttributeCombination(AttributeCombination *in)
{
  PyObject *entry = PyList_New(0);
  AttributeCombination *comb;

  if (in == NULL)
    return entry;
  
  for (comb = in; comb; comb = comb->next) {
    PyList_Append(entry, buildAttributeOccurrence(comb->item));
  }
  
  return entry;
}


PyObject*
buildAttributeCombinations(AttributeCombinations *in)
{
  struct legalCombinations_List85 *nextat, *iat;
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value, *valueList;
  
  if (in == NULL)
    return entry;

  nameStr = Py_BuildValue("s", "defaultAttributeSet");
  value = Py_BuildValue("s", in->defaultAttributeSet->data);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "legalCombinations");
  valueList = PyList_New(0);
  for (iat = in->legalCombinations; iat != NULL; iat = nextat) {
    value = buildAttributeCombination(iat->item);
    PyList_Append(valueList, value);
    nextat = iat->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  return entry;

}


PyObject*
buildOmittedAttributeInterpretation(OmittedAttributeInterpretation *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value;
  
  if (in == NULL)
    return entry;

  nameStr = Py_BuildValue("s", "defaultValue");
  value = buildStringOrNumeric(in->defaultValue);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "defaultDescription");
  value = buildHumanString(in->defaultDescription);
  PyDict_SetItem(entry, nameStr, value);

  return entry;
}

PyObject*
buildAttributeValue(AttributeValue *in)
{
  struct subAttributes_List52  *nextat1, *iat1;
  struct superAttributes_List53 *nextat2, *iat2;
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value, *valueList;
  
  if (in == NULL)
    return entry;

  nameStr = Py_BuildValue("s", "value");
  value = buildStringOrNumeric(in->value);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "description");
  value = buildHumanString(in->description);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "partialSupport");
  value = Py_BuildValue("i", in->partialSupport);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "subAttributes");
  valueList = PyList_New(0);
  for (iat1 = in->subAttributes; iat1 != NULL; iat1 = nextat1) {
    value = buildStringOrNumeric(iat1->item);
    PyList_Append(valueList, value);
    nextat1 = iat1->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  nameStr = Py_BuildValue("s", "superAttributes");
  valueList = PyList_New(0);
  for (iat2 = in->subAttributes; iat2 != NULL; iat2 = nextat2) {
    value = buildStringOrNumeric(iat2->item);
    PyList_Append(valueList, value);
    nextat2 = iat2->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  return entry;

}

PyObject*
buildAttributeTypeDetails(AttributeTypeDetails *in)
{
  struct attributeValues_List51 *nextat, *iat;
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value, *valueList;

  if (in == NULL)
    return entry;

  nameStr = Py_BuildValue("s", "attributeType");
  value = Py_BuildValue("i", in->attributeType);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "defaultIfOmitted");
  value = buildOmittedAttributeInterpretation(in->defaultIfOmitted);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "attributeValues");
  valueList = PyList_New(0);
  for (iat = in->attributeValues; iat != NULL; iat = nextat) {
    value = buildAttributeValue(iat->item);
    PyList_Append(valueList, value);
    nextat = iat->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);
   
  return entry;

}

PyObject*
buildAttributeSetDetails(AttributeSetDetails *in)
{

  struct attributesByType_List50 *nextat, *iat;
  PyObject *entry = PyDict_New();
  PyObject *nameStr, *value, *valueList;

  if (in == NULL) 
    return entry;

  nameStr = Py_BuildValue("s", "attributeSet");
  value = Py_BuildValue("s", in->attributeSet->data);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "attributesByType");
  valueList = PyList_New(0);
  for (iat = in->attributesByType; iat != NULL; iat = nextat) {
    value = buildAttributeTypeDetails(iat->item);
    PyList_Append(valueList, value);
    nextat = iat->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  return entry;

}


PyObject*
buildAttributeDetails(AttributeDetails *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *value;
  PyObject *valueList;
  struct attributesBySet_List49 *nextat, *iat;

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null AttributeDetails.");
    return (PyObject*) NULL;
  }

  nameStr = Py_BuildValue("s", "databaseName");
  value = Py_BuildValue("s", in->databaseName->data);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "commonInfo");
  value = buildCommonInfo(in->commonInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "attributesBySet");
  valueList = PyList_New(0);
  for (iat = in->attributesBySet; iat != NULL; iat = nextat) {
    value = buildAttributeSetDetails(iat->item);
    PyList_Append(valueList, value);
    nextat = iat->next;
  }
  PyDict_SetItem(entry, nameStr, valueList);

  nameStr = Py_BuildValue("s", "attributeCombinations");
  value = buildAttributeCombinations(in->attributeCombinations);
  PyDict_SetItem(entry, nameStr, value);

  return entry;

}

PyObject*
buildTermListDetails(TermListDetails *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null TermListDetails.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildElementSetDetails(ElementSetDetails *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null ElementSetDetails.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildRetrievalRecordDetails(RetrievalRecordDetails *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null RetrievalRecordDetails.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildSortDetails(SortDetails *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null SortDetails.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildProcessingInfo(ProcessingInformation *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null ProcessingInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildVariantSetInfo(VariantSetInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null VariantSetInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildUnitInfo(UnitInfo *in)
{
  PyObject *entry = PyDict_New();

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null UnitInfo.");
    return (PyObject*) NULL;
  }

  return entry;

}

PyObject*
buildCategoryInfo(CategoryInfo *in)
{
  PyObject *entry = Py_BuildValue("{s:s}", "category", in->category->data);
  PyObject *nameStr;
  PyObject *value;

  if (in->originalCategory != NULL) {
    nameStr = Py_BuildValue("s", "originalCategory");
    value = Py_BuildValue("s", in->originalCategory->data);
    PyDict_SetItem(entry, nameStr, value);
  }
  if (in->description != NULL) {
    nameStr = Py_BuildValue("s", "description");
    value = buildHumanString(in->description);
    PyDict_SetItem(entry, nameStr, value);
  }
  if (in->asn1Module != NULL) {
    nameStr = Py_BuildValue("s", "asn1Module");
    value = Py_BuildValue("s", in->asn1Module->data);
    PyDict_SetItem(entry, nameStr, value);

  }

  return entry;
}

PyObject*
buildCategoryList(CategoryList *in)
{
  PyObject *entry = PyDict_New();
  PyObject *nameStr;
  PyObject *value;
  struct categories_List67 *nextvr, *ivr;

  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null CategoryList.");
    return (PyObject*) NULL;
  }

  nameStr = Py_BuildValue("s","commonInfo");
  value = buildCommonInfo(in->commonInfo);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "categories");
  value = PyList_New(0);
  for (ivr = in->categories; ivr != NULL; ivr = nextvr) {
    PyList_Append(value, buildCategoryInfo(ivr->item));
    nextvr = ivr->next;
  }
  PyDict_SetItem(entry, nameStr, value);

  return entry;

}


PyObject*
buildTargetInfo(TargetInfo *in)
{
  PyObject *entry;
  PyObject *nameStr;
  PyObject *value;

  struct nicknames_List30 *nextnn, *inn;
  PyObject *PyNn;
  struct dbCombinations_List31 *nextdb, *idb;
  struct addresses_List32 *nextad, *iad;
  struct languages_List33 *nextln, *iln;


  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null TargetInfo.");
    return (PyObject*) NULL;
  }

  /* commonInfo  */
  
  entry = Py_BuildValue("{s:s,s:i,s:i,s:i,s:i,s:i}", "name", in->name->data, "namedResultSets", in->namedResultSets, "multipleDBSearch", in->multipleDBsearch, "maxResultSets", in->maxResultSets, "maxResultSize", in->maxResultSize, "maxTerms", in->maxTerms);

  /* Add Human Strings as Dictionaries */
  nameStr = Py_BuildValue("s", "news");
  value = buildHumanString(in->recent_news);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "welcomeMessage");
  value = buildHumanString(in->welcomeMessage);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "description");
  value = buildHumanString(in->description);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "usageRestrictions");
  value = buildHumanString(in->usage_restrictions);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "paymentAddress");
  value = buildHumanString(in->paymentAddr);
  PyDict_SetItem(entry, nameStr, value);

  nameStr = Py_BuildValue("s", "hours");
  value = buildHumanString(in->hours);
  PyDict_SetItem(entry, nameStr, value);

  /* IntUnit for TimeoutInterval */
  nameStr = Py_BuildValue("s", "timeoutInterval");
  value = buildIntUnit(in->timeoutInterval);
  PyDict_SetItem(entry, nameStr, value);

  /* ContactInfo for ContactInfo */
  nameStr = Py_BuildValue("s", "contactInfo");
  value = buildContactInfo(in->contactInfo);
  PyDict_SetItem(entry, nameStr, value);

  /* List of Nicknames */
  nameStr = Py_BuildValue("s", "nicknames");
  value = PyList_New(0);
  for (inn = in->nicknames; inn != NULL; inn = nextnn) {
    PyNn = Py_BuildValue("s", inn->item->data);
    PyList_Append(value, PyNn);
    nextnn = inn->next;
  }
  PyDict_SetItem(entry, nameStr, value);


  /* List of Network Addresses */
  nameStr = Py_BuildValue("s", "networkAddresses");
  value = PyList_New(0);
  for (iad = in->addresses; iad != NULL; iad = nextad) {
    PyNn = buildNetworkAddress(iad->item);
    PyList_Append(value, PyNn);
    nextad = iad->next;
  }
  PyDict_SetItem(entry, nameStr, value);

  /* List of Languages */
  nameStr = Py_BuildValue("s", "languages");
  value = PyList_New(0);
  for (iln = in->languages; iln != NULL; iln = nextln) {
    PyList_Append(value, Py_BuildValue("s", iln->item->data));
    nextln = iln->next;
  }
  PyDict_SetItem(entry, nameStr, value);
  
  /* CommonAccessInfo */

  nameStr = Py_BuildValue("s", "commonAccessInfo");
  value = buildAccessInfo(in->commonAccessInfo);
  PyDict_SetItem(entry, nameStr, value);

  /* DB combinations */

  nameStr = Py_BuildValue("s", "databaseCombinations");
  value = PyList_New(0);
  for (idb = in->dbCombinations; idb != NULL; idb = nextdb) {
    PyList_Append(value, buildDatabaseList(idb->item));
    nextdb = idb->next;
  }
  PyDict_SetItem(entry, nameStr, value);

  /* CommonInfo */
  nameStr = Py_BuildValue("s", "commonInfo");
  value = buildCommonInfo(in->commonInfo);
  PyDict_SetItem(entry, nameStr, value);


  return entry;

}

PyObject*
buildExplainRecord(Explain_Record *in)
{
  
  PyObject *recordList;
  PyObject *entry;
  PyObject *recType;
  
  if (in == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null Explain Record.");
    return (PyObject*) NULL;
  }

  recordList = PyList_New(0);

  switch (in->which) {
  case e34_targetInfo:
    entry = buildTargetInfo(in->u.targetInfo);
    recType = Py_BuildValue("s", "targetInfo");
    break;
  case e34_databaseInfo:
    entry = buildDatabaseInfo(in->u.databaseInfo);
    recType = Py_BuildValue("s", "databaseInfo");
    break;
  case e34_schemaInfo:
    entry = buildSchemaInfo(in->u.schemaInfo);
    recType = Py_BuildValue("s", "schemaInfo");
    break;
  case e34_tagSetInfo:
    entry = buildTagSetInfo(in->u.tagSetInfo);
    recType = Py_BuildValue("s", "tagSetInfo");
    break;
  case e34_recordSyntaxInfo:
    entry = buildRecordSyntaxInfo(in->u.recordSyntaxInfo);
    recType = Py_BuildValue("s", "recordSyntaxInfo");
    break;
  case e34_attributeSetInfo:
    entry = buildAttributeSetInfo(in->u.attributeSetInfo);
    recType = Py_BuildValue("s", "attributeSetInfo");
    break;
  case e34_termListInfo:
    entry = buildTermListInfo(in->u.termListInfo);
    recType = Py_BuildValue("s", "termListInfo");
    break;
  case e34_extendedServicesInfo:
    entry = buildExtendedServicesInfo(in->u.extendedServicesInfo);
    recType = Py_BuildValue("s", "extendedServicesInfo");
    break;
  case e34_attributeDetails:
    entry = buildAttributeDetails(in->u.attributeDetails);
    recType = Py_BuildValue("s", "attributeDetails");
    break;
  case e34_termListDetails:
    entry = buildTermListDetails(in->u.termListDetails);
    recType = Py_BuildValue("s", "termListDetails");
    break;
  case e34_elementSetDetails:
    entry = buildElementSetDetails(in->u.elementSetDetails);
    recType = Py_BuildValue("s", "elementSetDetails");
    break;
  case e34_retrievalRecordDetails:
    entry = buildRetrievalRecordDetails(in->u.retrievalRecordDetails);
    recType = Py_BuildValue("s", "retrievalRecordDetails");
    break;
  case e34_sortDetails:
    entry = buildSortDetails(in->u.sortDetails);
    recType = Py_BuildValue("s", "sortDetails");
    break;
  case e34_processing:
    entry = buildProcessingInfo(in->u.processing);
    recType = Py_BuildValue("s", "processingInfo");
    break;
  case e34_variants:
    entry = buildVariantSetInfo(in->u.variants);
    recType = Py_BuildValue("s", "variants");
    break;
  case e34_units:
    entry = buildUnitInfo(in->u.units);
    recType = Py_BuildValue("s", "unitInfo");
    break;
  case e34_categoryList:
    entry = buildCategoryList(in->u.categoryList);
    recType = Py_BuildValue("s", "categoryList");
    break;
  default:
    PyErr_SetString(ZCh_ErrorObject, "Unknown Explain record type.");
    return (PyObject*) NULL;
  }

  PyList_Append(recordList, recType);
  PyList_Append(recordList, entry);
  return recordList;
  
}


PyObject*
buildGRS1Record(GenericRecord *genrec)
{
  GenericRecord *rec;
  PyObject *recordList;
  PyObject *entry;
  PyObject *recType = Py_BuildValue("s", "GRS1");
  
  if (genrec == NULL) {
    PyErr_SetString(ZCh_ErrorObject, "Null GRS1 Record!");
    return (PyObject*) NULL;
  }

  recordList = PyList_New(0);
  PyList_Append(recordList, recType);

  for (rec = genrec; rec ; rec = rec->next) {
    entry = buildTaggedElement(rec->item);
    if (entry == NULL) {
      PyErr_SetString(ZCh_ErrorObject, "Null GRS1 Element!");
      return NULL;
    }

    PyList_Append(recordList, entry);
  }

  return recordList;
}

PyObject*
buildTaggedElement(TaggedElement *in) 
{
  /* eD["name": "type": "attr": "contents": eCL()] */

  PyObject *elemDict = PyDict_New();
  PyObject *elemName = buildStringOrNumeric(in->tagValue);
  PyObject *elemIsAttr = Py_BuildValue("i", isMetaDataElement(in->metaData));
  PyObject *elemContentsList = buildElementData(in->content);

  PyObject *elemType = NULL;

  PyObject *nameString = Py_BuildValue("s", "name");
  PyObject *typeString = Py_BuildValue("s", "type");
  PyObject *attrString = Py_BuildValue("s", "attr");
  PyObject *contentsString = Py_BuildValue("s", "contents");

  /* Type is from tagset or 'local' */

  if (in->tagType == 1) {
    if (in->tagValue->which == e25_numeric) {
      switch (in->tagValue->u.numeric) {
      case 1:
	elemType = Py_BuildValue("s", "schemaIdentifier");
	break;
      case 2:
	elemType = Py_BuildValue("s", "elementsOrdered");
	break;
      case 3:
	elemType = Py_BuildValue("s", "elementOrdering");
	break;
      case 4:
	elemType = Py_BuildValue("s", "defaultTagType");
	break;
      case 5:
	elemType = Py_BuildValue("s", "defaultVariantSetId");
	break;
      case 6:
	elemType = Py_BuildValue("s", "defaultVariantSpec");
	break;
      case 7:
	elemType = Py_BuildValue("s", "processingInstructions");
	break;
      case 8:
	elemType = Py_BuildValue("s", "recordUsage");
	break;
      case 9:
	elemType = Py_BuildValue("s", "restriction");
	break;
      case 10:
	elemType = Py_BuildValue("s", "rank");
	break;
      case 11:
	elemType = Py_BuildValue("s", "userMessage");
	break;
      case 12:
	elemType = Py_BuildValue("s", "url");
	break;
      case 13:
	elemType = Py_BuildValue("s", "record");
	break;
      case 14:
	elemType = Py_BuildValue("s", "localControlNumber");
	break;
      case 15:
	elemType = Py_BuildValue("s", "creationDate");
	break;
      case 16:
	elemType = Py_BuildValue("s", "dateOfLastModification");
	break;
      case 17:
	elemType = Py_BuildValue("s", "dateOfLastReview");
	break;
      case 18:
	elemType = Py_BuildValue("s", "score");
	break;
      case 19:
	elemType = Py_BuildValue("s", "wellKnown");
	break;
      case 20:
	elemType = Py_BuildValue("s", "recordWrapper");
	break;
      case 21:
	elemType = Py_BuildValue("s", "defaultTagSetId");
	break;
      case 22:
	elemType = Py_BuildValue("s", "LanguageOfRecord");
	break;
      case 23:
	elemType = Py_BuildValue("s", "type");
	break;
      case 24:
	elemType = Py_BuildValue("s", "Scheme");
	break;
      case 25:
	elemType = Py_BuildValue("s", "costInfo");
	break;
      case 26:
	elemType = Py_BuildValue("s", "costFlag");
	break;
      }
    }
  } else if (in->tagType == 2) {
    /* this is using tagSet-G */
    if (in->tagValue->which == e25_numeric) {
      switch (in->tagValue->u.numeric) {
      case 1:
	elemType = Py_BuildValue("s", "title");
	break;
      case 2:
	elemType = Py_BuildValue("s", "author");
	break;
      case 3:
	elemType = Py_BuildValue("s", "publicationPlace");
	break;
      case 4:
	elemType = Py_BuildValue("s", "publicationDate");
	break;
      case 5:
	elemType = Py_BuildValue("s", "documentId");
	break;
      case 6:
	elemType = Py_BuildValue("s", "abstract");
	break;
      case 7:
	elemType = Py_BuildValue("s", "name");
	break;
      case 8:
	elemType = Py_BuildValue("s", "dateTime");
	break;
      case 9:
	elemType = Py_BuildValue("s", "displayObject");
	break;
      case 10:
	elemType = Py_BuildValue("s", "organization");
	break;
      case 11:
	elemType = Py_BuildValue("s", "postalAddress");
	break;
      case 12:
	elemType = Py_BuildValue("s", "networkAddress");
	break;
      case 13:
	elemType = Py_BuildValue("s", "eMailAddress");
	break;
      case 14:
	elemType = Py_BuildValue("s", "phoneNumber");
	break;
      case 15:
	elemType = Py_BuildValue("s", "faxNumber");
	break;
      case 16:
	elemType = Py_BuildValue("s", "country");
	break;
      case 17:
	elemType = Py_BuildValue("s", "description");
	break;
      case 18:
	elemType = Py_BuildValue("s", "time");
	break;
      case 19:
	elemType = Py_BuildValue("s", "DocumentContent");
	break;
      case 20:
	elemType = Py_BuildValue("s", "language");
	break;
      case 21:
	elemType = Py_BuildValue("s", "subject");
	break;
      case 22:
	elemType = Py_BuildValue("s", "resourceType");
	break;
      case 23:
	elemType = Py_BuildValue("s", "city");
	break;
      case 24:
	elemType = Py_BuildValue("s", "stateOrProvince");
	break;
      case 25:
	elemType = Py_BuildValue("s", "zipOrPostalCode");
	break;
      case 26:
	elemType = Py_BuildValue("s", "cost");
	break;
      case 27:
	elemType = Py_BuildValue("s", "format");
	break;
      case 28:
	elemType = Py_BuildValue("s", "identifier");
	break;
      case 29:
	elemType = Py_BuildValue("s", "rights");
	break;
      case 30:
	elemType = Py_BuildValue("s", "relation");
	break;
      case 31:
	elemType = Py_BuildValue("s", "publisher");
	break;
      case 32:
	elemType = Py_BuildValue("s", "contributor");
	break;
      case 33:
	elemType = Py_BuildValue("s", "source");
	break;
      case 34: 
	elemType = Py_BuildValue("s", "coverage");
	break;
      case 35:
	elemType = Py_BuildValue("s", "private_var");
	break;
      }
    }
  }
  else
    elemType = Py_BuildValue("s", "local");
  
  if (elemType == NULL)
    elemType = Py_BuildValue("s", "unknown");

  
  PyDict_SetItem(elemDict, nameString, elemName);
  PyDict_SetItem(elemDict, typeString, elemType);
  PyDict_SetItem(elemDict, attrString, elemIsAttr);
  PyDict_SetItem(elemDict, contentsString, elemContentsList);

  return elemDict;
  
}

PyObject*
buildStringOrNumeric(StringOrNumeric *in)
{
  PyObject *SoN;

  if (in == NULL)
    return (PyObject*) NULL;

  if (in->which == e25_string) {
    SoN = Py_BuildValue("s", in->u.string->data);
  } else {
    SoN = Py_BuildValue("i", in->u.numeric);
  }
  return SoN;

}

PyObject*
buildElementData(ElementData *in)
{
  PyObject *value;
  struct subtree_List91 *sub;

  switch (in->which) {
  case e46_octets:
    value = Py_BuildValue("s", in->u.octets->data);
    break;
  case e46_numeric:
    value = Py_BuildValue("i", in->u.numeric);
    break;
  case e46_date:
    value = Py_BuildValue("s", in->u.date->data);
    break;
  case e46_ext:
    value = Py_BuildValue("s", "EXTERNAL");
    break;
  case e46_string:
    value = Py_BuildValue("s", in->u.string->data);
    break;
  case e46_trueOrFalse:
    value = Py_BuildValue("i", in->u.trueOrFalse);
    break;
  case e46_oid:
    value = Py_BuildValue("s", in->u.oid->data);
    break;
  case e46_intUnit:
    /* Int + Unit */
    value = PyList_New(0);
    PyList_Append(value, Py_BuildValue("i", in->u.intUnit->value));
    PyList_Append(value, Py_BuildValue("s", in->u.intUnit->unitUsed));
    break;
  case e46_elementNotThere:
    value = Py_BuildValue("s", "elementNotThere");
    break;
  case e46_elementEmpty:
    value = Py_BuildValue("s", "elementEmpty");
    break;
  case e46_noDataRequested:
    value = Py_BuildValue("s", "elementEmpty");
    break;
  case e46_diagnostic:
    value = Py_BuildValue("s", "DIAGNOSTIC");
    break;
  case e46_subtree:
    value = PyList_New(0);

    for (sub = in->u.subtree; sub; sub = sub->next) {
      PyList_Append(value, buildTaggedElement(sub->item));
    }
    break;
    
  }
  
  return value;
}


int
isMetaDataElement(ElementMetaData *in)
{

  if (in == NULL || in->message == NULL || in->message->data == NULL)
    return 0;

  if (strncasecmp(in->message->data, "Attribute", 9) == 0)
    return 1;
  else 
    return 0;

}




/* ---------------------------------------------------- */
/* Copied in from zAppInit.c, check_result_set.c, zquery.c */


int TKZ_Present(ZSESSION *Session) {
  PDU *RequestPDU,*ResponsePDU;
  int recStartPoint = Session->startPosition;
  int numRecsRequested = Session->numberOfRecordsRequested;
  OtherInformation *addsearchinfo = NULL;

  if (Session == NULL)
    return Z_ERROR;
  
  if (result_set_changed == 0) {
    /* the data in the session structure is current */
    /* Check for some potential errors */
    if(recStartPoint > Session->resultCount) {
      sprintf(Session->errorMessage, "Maximum record number is %d.",
	      Session->resultCount);
      return Z_ERROR;
    }
    
    /* if more are requested than are available, reduce the number requested */
    if(numRecsRequested > ((Session->resultCount-recStartPoint)+1))
      numRecsRequested = (Session->resultCount - recStartPoint) + 1;	
  }
  /* otherwise we will assume (for now) external tracking to avoid errors  */
  /* from the target -- This really should be handled better on the client */
  
  
  if(numRecsRequested <= 0)
    return Z_ERROR;
  
  if (Session->SearchAddInfo[0] != '\0') {
    addsearchinfo = CALLOC(OtherInformation, 1);
    if (Session->SearchAddInfoType == 1) {
      addsearchinfo->item.information.which = e24_oid;
      addsearchinfo->item.information.u.oid = NewOID(Session->SearchAddInfo);
    }
    else {
      addsearchinfo->item.information.which = e24_characterInfo;
      addsearchinfo->item.information.u.characterInfo = 
	NewInternationalString(Session->SearchAddInfo);
    }
  }
  
  
  Session->noRecordsReturned = 0; /* no records returned for this call yet */
  
  Session->startPosition = recStartPoint;
  Session->numberOfRecordsRequested = numRecsRequested;
  
  /* Build the PresentRequest PDU */
  RequestPDU = MakePresentRequest(
				  "PRESENT_REFID", 
				  Session->resultSetName, 
				  recStartPoint,
				  numRecsRequested,
				  /* providing only a single string */
				  /* instead of an array            */
				  /* it does handle this...         */
				  Session->presentElementSet,
				  Session->preferredRecordSyntax,
				  addsearchinfo);
  
  /* did the Make work?? */
  if (RequestPDU == NULL)
    return Z_ERROR;
  
  /* transmit it to the server */
  Session->state = S_PRESENT_SENT;
  if (PutPDU (Session->fd, RequestPDU) != Z_OK)
    return Z_ERROR;
  
  /* don't need it any more */
  (void) FreePDU (RequestPDU);
  
  /* get the response from the server */
  if ((ResponsePDU = ClientGetPDU(Session->fd,Session->c_timeOut)) == NULL) {
    fprintf(stderr, "Failed to get a present response pdu.\n"); 
    tk_disconnect(Session);
    return Z_ERROR;
  }
  Session->state = S_PRESENT_RECEIVED;
  
  /* move the results into the Session structure */
  if (ProcessPresentResponse(Session, ResponsePDU->u.presentResponse) 
      != Z_OK) {
    (void) FreePDU (ResponsePDU);
    return Z_ERROR;
  }
  
  (void) FreePDU (ResponsePDU);
  
  return Z_OK;
}



int TKZ_Search(ZSESSION *Session) {
  RPNQuery *sRPNQuery;
  Query *Z3950Query;
  PDU *RequestPDU, *ResponsePDU;
  ElementSetNames *ssesn, *msesn;
  char **databaseNames;
  /* these are set in command (query) parsing */
  extern char *LastResultSetID;
  OtherInformation *addsearchinfo = NULL;
  
  
  if (Session == NULL)
    return Z_ERROR;
  
  if(strlen(Session->queryString)==0) {
    strcpy(Session->errorMessage,"You must supply a valid query term(s)");
    return(Z_ERROR);
    
  }
  /* call the command language parser to build an RPN query */
  sRPNQuery = queryparse(Session->queryString, Session->c_attributeSetId);
  
  /* any problems are signaled by the external value parse_errors */
  if (sRPNQuery == NULL) {
    strcpy(Session->errorMessage,"Syntax error in query");
    return(Z_ERROR);
  }
  
  Z3950Query = MakeQueryType1(sRPNQuery, NULL);/* attributes already set */
  
  if (LastResultSetID) {
    /* resultset name included with query */
    strcpy (Session->resultSetName, LastResultSetID);
  } /* otherwise leave it as is */
  
  /* convert the string of database names (or a single name) */
  /* to a null-terminated list.                              */
  databaseNames = StringToArgv (Session->databaseName);
  
  /* could add element set specifications --- later */
  ssesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_smallElementSetNames);
  msesn = MakeElementSetNames(NULL, (char **)TKZ_User_Session->s_mediumElementSetNames);
  
  if (Session->SearchAddInfo[0] != '\0') {
    addsearchinfo = CALLOC(OtherInformation, 1);
    if (Session->SearchAddInfoType == 1) {
      addsearchinfo->item.information.which = e24_oid;
      addsearchinfo->item.information.u.oid = NewOID(Session->SearchAddInfo);
    }
    else {
      addsearchinfo->item.information.which = e24_characterInfo;
      addsearchinfo->item.information.u.characterInfo = 
	NewInternationalString(Session->SearchAddInfo);
    }
  }
  
  /* Build the search request PDU */
  RequestPDU = MakeSearchRequest("SEARCH_REFID",  /* reference ID */
				 Session->c_smallSetUpperBound,
				 Session->c_largeSetLowerBound,
				 Session->c_mediumSetPresentNumber,
				 Z_TRUE, /* replace indicator */
				 Session->resultSetName,
				 databaseNames, /* list of databases */
				 ssesn,		/*	ssesn	*/
				 msesn,		/*	msesn	*/
				 Session->preferredRecordSyntax,
				 Z3950Query, addsearchinfo);

  if (RequestPDU == NULL) {
    strcpy(Session->errorMessage, 
	   "Failed to create a search request apdu.");
    return Z_ERROR;
  }
  if (PutPDU (Session->fd, RequestPDU) != Z_OK) {
    strcpy(Session->errorMessage, 
	   "Failed to transmit search request. Connection may have closed.");
    return Z_ERROR;	
  }
  
  (void) FreePDU (RequestPDU);
  (void) FreeArgv(databaseNames);
  Session->state = S_SEARCH_SENT;
  
  do {
    if ((ResponsePDU = ClientGetPDU(Session->fd,Session->c_timeOut)) == NULL) {
      strcpy(Session->errorMessage, 
	     "Didn't get a search response pdu. Connection closed.");
      /* something serious happened. close the connection. */
      tk_disconnect(Session);
      Session->connectionStatus = Z_CLOSE;
      return Z_ERROR;
    }
    if (ResponsePDU->which  == e1_resourceControlRequest) {	
      Session->state = S_RESOURCE_CONTROL_RECEIVED;
      return Z_ERROR;
    } 
  } while (ResponsePDU->which != e1_searchResponse);
  
  Session->state = S_SEARCH_RECEIVED;
  
  if (ProcessSearchResponse (Session, ResponsePDU->u.searchResponse) != Z_OK){
    strcpy(Session->errorMessage, 
	   "Error in processing search response.");
    (void) FreePDU (ResponsePDU);
    return Z_ERROR;
  }

  /* Should be all set now...*/
  (void) FreePDU (ResponsePDU);
  return Z_OK;
  
}


int
is_string_result_set (char *searchstring)
{
  /* any resultset name with a colon in it MUST be a resultsetid */
  if (strchr(searchstring,':') != NULL)
    return 1;
  
  if (TKZ_User_Session == NULL)
    return 0;
  
  if (TKZ_User_Session->c_resultSetNames == NULL)
    return 0;
  
  return (ac_list_search(TKZ_User_Session->c_resultSetNames, searchstring));
}

void
FreeSession (session)
     ZSESSION *session;
{
  RESULT_SET *rescurr, *resnext;
  DOCUMENT *document;
  int i;
  
  if (session->queryString) 
    free(session->queryString);		/* the last search query */
  
  for (i=0; i<session->numberResultSets; i++)
    if (session->resultSetList[i])
      free(session->resultSetList[i]);
  
  for (rescurr = session->s_resultSets; rescurr!=NULL; rescurr=resnext) {
    resnext=rescurr->next;
    free (rescurr);
  }
  
  ac_list_free(session->c_resultSetNames);	/* list of result set names that the
					 * server maintains for this session.
					 */
  
  FreePDU((PDU *)session->lastInPDU);   /* the last incoming pdu	*/
  FreePDU((PDU *)session->lastOutPDU);  /* the last outgoing pdu	*/
  
  /* free any documents from last retrieval */
  if (session->documentList != NULL) {
    for (i=1; i <= session->resultCount; i++) {
      document = session->documentList[i];
      if (document != NULL) {
	if (document->data) free((char *)document->data);
	free(document);
      }
    }
    free((char *)session->documentList);
  }
  
  FreeDefaultDiagRec(session->diagRec);	/* diagnostic record.		*/
  
  ac_list_free(session->DIDList);		/* List of document ids.*/
  
  if(session->searchResult) {
    if(session->searchResult->resultSet)
      ac_list_free(session->searchResult->resultSet);
    free((char *)session->searchResult);
  }
  
  if(session->s_serverMachineName)
    free((char *)session->s_serverMachineName);
  if(session->s_serverIP)
    free((char *)session->s_serverIP);
  if(session->s_protocolVersion)
    free((char *)session->s_protocolVersion);
  if(session->s_options)
    free((char *)session->s_options);
  if(session->s_implementationId)
    free((char *)session->s_implementationId);
  if(session->s_implementationName)
    free((char *)session->s_implementationName);
  if(session->s_implementationVersion)
    free((char *)session->s_implementationVersion);
  
  ac_list_free(session->s_databaseNames);
  ac_list_free(session->s_accessPoints);
  
  /* these only occur in the server and shouldn't waste much space 
   *ELEMENT_SETS	session->s_smallElementSetNames;
   *ELEMENT_SETS	session->s_mediumElementSetNames;
   */
  
  /* This is not allocated dynamically -- a single buffer is used
   * if(*s_logFileName)
   *    free((char *)*s_logFileName); 
   */
  
  for(i = 0; i < 10; i++)
    if(session->c_databaseNames[i])
      free((char *)session->c_databaseNames[i]);
  
  if(session->s_resultSetDirectory)
    free((char *)session->s_resultSetDirectory);
  
  /* This will be set to a constant only -- don't free it
   * if(session->s_attributeSetId)
   *    free((char *)session->s_attributeSetId);
   */
  
  if(session->c_clientMachineName)
    free((char *)session->c_clientMachineName);
  if(session->c_clientIP)
    free((char *)session->c_clientIP);
  
  if(session->c_referenceId)
    free((char *)session->c_referenceId);
  if(session->c_protocolVersion)
    free((char *)session->c_protocolVersion);
  if(session->c_options)
    free((char *)session->c_options);
  if(session->c_implementationId)
    free((char *)session->c_implementationId);
  if(session->c_implementationName)
    free((char *)session->c_implementationName);
  if(session->c_implementationVersion)
    free((char *)session->c_implementationVersion);
  
  free((char *)session);

}

ZSESSION *
LookupAssociation (assocs, sessionID)
     ZASSOCIATION *assocs;
     int     sessionID;      /* It is the same as the file descriptor.       */
{
  ZSESSION    *session;
  if (!assocs)
    return (ZSESSION *)NULL;
  for (session=assocs->head; session!=NULL; session=session->next)
    if (session->fd == sessionID)
      return session;
  return (ZSESSION *)NULL;
}

ZSESSION *
SearchAssociation (assocs, hostName, portNumber)
     ZASSOCIATION *assocs;
     char 	*hostName;
     int	portNumber;
{
  ZSESSION 	*session;
  if (assocs == NULL)
    return (ZSESSION *)NULL;
  for (session=assocs->head; session!=NULL; session=session->next)
    if ((session->s_portNumber == portNumber) &&
	(strcasecmp(session->s_serverMachineName, hostName) == 0))
      return session;
  return (ZSESSION *)NULL;
}

int
RemoveAssociation (assocs, sessionID)
     ZASSOCIATION *assocs;
     int	sessionID;	/* It's the same as the file descriptor.	*/
{
  ZSESSION *tmp;
  if (assocs == NULL)
    return Z_ERROR;
  tmp = assocs->head;
  if ((tmp->fd == sessionID) && (tmp == assocs->tail)) {
    assocs->head = NULL;
    assocs->tail = NULL;
    assocs->counts = 0;
    return Z_OK;
  }
  
  if ((tmp->fd == sessionID) && (tmp != assocs->tail)) {
    assocs->head = tmp->next;
    tmp->next->prev = NULL;
    assocs->counts -= 1;
    return Z_OK;
  }
  
  for(tmp = assocs->head; tmp!=NULL; tmp=tmp->next) {
    if (tmp->fd == sessionID) {
      if (tmp == assocs->tail)	{
	assocs->tail = tmp->prev;
	tmp->prev->next = NULL;
      } else {
	tmp->prev->next = tmp->next;
	tmp->next->prev = tmp->prev;
      }
      assocs->counts -= 1;
      return Z_OK;
    }
  }
  return Z_ERROR;
}

int
InitSession (session)
     ZSESSION *session;
{
  
  if (!session)
    return Z_ERROR;
  if (TKZ_User_Session_TMP != NULL) {
    return Z_OK;
  } else { /* initializing from scratch */
    /* don't free the following -- constant is used as value */
    session->preferredRecordSyntax = MARCRECSYNTAX;

    strcpy(session->resultSetName, "Default");
    strcpy(session->presentElementSet, "F");
    session->numberOfRecordsRequested = 10;
    session->startPosition = 1;

    session->c_attributeSetId = OID_BIB1;

    if (session->c_preferredMessageSize == 0)
      session->c_preferredMessageSize = 16000;
    
    if (session->c_exceptionalRecordSize == 0)
      session->c_exceptionalRecordSize = 2000000;
    
    session->c_implementationId = strdup("UCB_UoL_Z_V3_2001");
    session->c_implementationName = strdup("ZCheshire Python Client ");
    session->c_implementationVersion = strdup("0.1");
    session->c_protocolVersion = strdup("111");
    session->c_options = strdup("111000011000001");
    session->c_timeOut = 300;	/* in seconds	*/
    session->c_smallSetUpperBound = SMALL_SET_UPPER_BOUND;
    session->c_largeSetLowerBound = LARGE_SET_LOWER_BOUND;
    session->c_mediumSetPresentNumber = MEDIUM_SET_PRESENT_NUMBER;
    
    session->c_supportVersion1 = Z_TRUE;
    session->c_supportVersion2 = Z_TRUE;
    session->c_supportVersion3 = Z_TRUE;
    
    session->c_requestSearch = Z_TRUE;
    session->c_requestPresent = Z_TRUE;
    session->c_requestDeleteResultSet = Z_TRUE;
    session->c_requestResourceReport = Z_FALSE;
    session->c_requestTriggerResourceControl = Z_FALSE;
    session->c_requestResourceControl = Z_FALSE;
    session->c_requestAccessControl = Z_FALSE;
    session->c_requestScan =  Z_TRUE;
    session->c_requestSort = Z_TRUE;
    session->c_requestExtendedServices = Z_FALSE;
    session->c_requestLevel_1_Segmentation = Z_FALSE;
    session->c_requestLevel_2_Segmentation = Z_FALSE;
    session->c_requestConcurrentOperations = Z_FALSE;
    session->c_requestNamedResultSets = Z_TRUE;
    
    session->s_supportType0Query = Z_UNKNOWN;
    session->s_supportType2Query = Z_UNKNOWN;
    session->s_supportType100Query = Z_UNKNOWN;
    session->s_supportType101Query = Z_UNKNOWN;
    session->s_supportElementSetNames = Z_UNKNOWN;
    session->s_supportSingleElementSetName = Z_UNKNOWN;
    session->multipleDatabasesSearch = Z_UNKNOWN;
    session->namedResultSet = Z_UNKNOWN;
  }
  return Z_OK;
}



/* ----------------------------------------------------- */

static char ZCh_connect__doc__[] =
"ZCheshire.connect(host, port, authentication?)"
;

static PyObject *
ZCh_connect(self, args)
	PyObject *self;	/* Not used */
	PyObject *args;
{
  char *host;
  int port;
  char *authentication = NULL;
  GSTR *gAuthentication = NewGSTR(NULL);
  ZSESSION *session;
  char errorMsg[200];
  char temp[500];
  ZASSOCIATION *assoc = &zAssociations;



  if (!PyArg_ParseTuple(args, "si|s", &host, &port, &authentication))
    return NULL;
  
  if (port < 1) {
    PyErr_SetString(ZCh_ErrorObject, "Port paramater must be greater than 0.");
    return NULL;
  } else if (strlen(host) == 0) {
    PyErr_SetString(ZCh_ErrorObject, "Host parameter must not be empty.");
    return NULL;
  }
  
  if (authentication != NULL) {
    gAuthentication = NewGSTR(authentication);
  }

  /* Code for connect */

  if ((session = SearchAssociation(assoc, host, port)) != NULL ) {
    if (session->connectionStatus == Z_OPEN) {
      TKZ_User_Session = session;
      sprintf(temp, "Switching to connection '%d' to %s:%d.", session->fd, session->s_serverMachineName, session->s_portNumber);
      return Py_BuildValue("s", temp);
      
    } else {

      session = tk_reconnect(TKZ_User_Session);
      if (session != NULL && session->state == S_OPEN) {
	session->connectionStatus = Z_OPEN;
	TKZ_User_Session = session;
	sprintf(temp, "Reopened connection '%d' to %s:%d.", session->fd, session->s_serverMachineName, session->s_portNumber);
	return Py_BuildValue("s", temp);
      }
    }
  } 

  session = tk_connect(host, port, gAuthentication);

  if (session == NULL) {
    /* Out of memory */
    PyErr_SetString(ZCh_ErrorObject, "Could not connect, out of resources.");
    return NULL;
  } else if (session->status != Z_OK) {
    /* Couldn't connect */
    if (session->state != Z_OPEN) {
      sprintf(errorMsg, "Could not open connection to %s:%d.", session->s_serverMachineName, session->s_portNumber);
    } else {
      sprintf(errorMsg, "Did not get an initResponse from %s:%d.", session->s_serverMachineName, session->s_portNumber);
    }
    PyErr_SetString(ZCh_ErrorObject, errorMsg);
    free ((char *)session);
    return NULL;
  } else {
    /* Connected */

    TKZ_User_Session = session;
    
    session->connectionStatus = Z_OPEN;
    session->prev = NULL;
    session->next = NULL;
    if (assoc->head == NULL) {
      assoc->head = session;
      assoc->tail = session;
      assoc->counts = 1;
    } else {
      session->prev = assoc->tail;
      assoc->tail->next = session;
      assoc->tail = session;
      assoc->counts += 1;
    }
    
    FD_SET(session->fd, &(assoc->active_fd_set)); 
    assoc->no_fd_set += 1;
    if (session->fd > assoc->maximum_fd)
      assoc->maximum_fd = session->fd;
    
    /* Set the default database name */
    strcpy(session->databaseName,  ZCh_defaultDatabase);
    
    sprintf(temp, "Connection '%d' opened to %s:%d", session->fd, session->s_serverMachineName, session->s_portNumber);
    return Py_BuildValue("s", temp);
  }
}


/* ----- SEARCH ----- */

static char ZCh_search__doc__[] =
"ZCheshire.search(query, resultSetId?) => dictionary"
;

static PyObject *
ZCh_search(self, args)
     PyObject *self;	/* Not used */
     PyObject *args;
{
  char *query;
  char *resultset = NULL;
  char errorMsg[200];
  char resultsWork[10000];
  
  if (!PyArg_ParseTuple(args, "s|s", &query, &resultset))
    return NULL;

  if (resultset != NULL) {
    /* Append resultsetid=resultset to query */
    strcpy(resultsWork, query);
    strcat(resultsWork, " resultsetid ");
    strcat(resultsWork, resultset);
    query = resultsWork;
  }
  

  /* No current connection */
  if (TKZ_User_Session == NULL)
    PyErr_SetString(ZCh_ErrorObject, "There is no connection to a server to search over.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN) {
    sprintf(errorMsg, "Connection '%d' to %s:%d has closed.", TKZ_User_Session->fd, TKZ_User_Session->s_serverMachineName, TKZ_User_Session->s_portNumber);
    PyErr_SetString(ZCh_ErrorObject, errorMsg);
  } else if (strlen(query) == 0) 
    PyErr_SetString(ZCh_ErrorObject, "Query parameter must not be empty.");
  else {
  
    /* Init for Search */
    ClearPreviousSearch(TKZ_User_Session);
    TKZ_User_Session->queryString = strdup(query);
    
    /* Error if search errors. Reason in session->errorMessage */
    if (TKZ_Search(TKZ_User_Session) == Z_ERROR) {
      sprintf(errorMsg, "%s", TKZ_User_Session->errorMessage);
      PyErr_SetString(ZCh_ErrorObject, errorMsg);
      return NULL;
    }
    
    if (TKZ_User_Session->diagRecPresent) {
      /* Got a Diagnostic */
      if (TKZ_User_Session->diagRec == NULL) {
	PyErr_SetString(ZCh_ErrorObject, "Unknown Server Error");
      }  else {
	sprintf(errorMsg, "Diagnostic %d: %s", TKZ_User_Session->diagRec->condition, GetOctetString(TKZ_User_Session->diagRec->addinfo.u.v3Addinfo));
	PyErr_SetString(ZCh_ErrorObject, errorMsg);
      }
      return NULL;
    }
    
    /* Add resultset to list, even if no hits according to 200X spec */
    
    if (ac_list_search(TKZ_User_Session->c_resultSetNames, TKZ_User_Session->resultSetName) != 1) {
      if (TKZ_User_Session->c_resultSetNames == NULL)
	TKZ_User_Session->c_resultSetNames = ac_list_alloc();
      ac_list_add(TKZ_User_Session->c_resultSetNames, TKZ_User_Session->resultSetName);
    }
    
    /* Return Information. */
    return Py_BuildValue("{s:i,s:i,s:s}", "status", TKZ_User_Session->searchStatus, "hits", TKZ_User_Session->resultCount, "resultSet", TKZ_User_Session->resultSetName);
  }
  return NULL;

}

static char ZCh_scan__doc__[] =
"ZCheshire.scan(index, term, stepsize, numRequested, position)"
;

static PyObject *
ZCh_scan(self, args, keywds)
     PyObject *self;	/* Not used */
     PyObject *args;
     PyObject *keywds;
{
  char *index;
  char *term;
  int stepsize = 0;
  int numRequested = 20;
  int position = 1;


  ScanResponse *scanResponse;
  char **databaseNames;
  PDU *RequestPDU, *ResponsePDU;
  RPNQuery *sRPNQuery;
  AttributesPlusTerm *attrterms;
  char querystring[500];
  ListEntries *le;
  struct entries_List13 *entries;
  TermInfo *ti;
  char errorMsg[200];


  /* We want to return a dictionary of status (int), position (int), stepsize (int), terms (list of strings) */

  PyObject *returnDict;
  PyObject *entryList;
  PyObject *entryString;
  PyObject *dictString = Py_BuildValue("s", "terms");
  
   static char *kwlist[] = {"index", "term", "stepSize", "numRequested", "position", NULL};

   if (!PyArg_ParseTupleAndKeywords(args, keywds,  "ss|iii", kwlist,  &index, &term, &stepsize, &numRequested, &position))
    return NULL;
  
  if (stepsize < 0)
    PyErr_SetString(ZCh_ErrorObject, "Stepsize paramater must not be negative.");
  else if (numRequested < 0) 
    PyErr_SetString(ZCh_ErrorObject, "NumRequested parameter must be greater than 0.");
  else if (position < 0 || position > (numRequested + 1))
    PyErr_SetString(ZCh_ErrorObject, "Position parameter must be between 0 and (numRequested + 1).");
  else if (strlen(index) == 0) 
    PyErr_SetString(ZCh_ErrorObject, "Index parameter must not be empty.");
  else if (TKZ_User_Session == NULL) 
    PyErr_SetString(ZCh_ErrorObject, "There is no current connection.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN) 
    PyErr_SetString(ZCh_ErrorObject, "Connection has closed.");
  else {

    /* Build RPN query, then free */
    sprintf(querystring, "%s {%s}", index, term);
    sRPNQuery = queryparse(querystring, TKZ_User_Session->c_attributeSetId);
    if (sRPNQuery == NULL) 
      PyErr_SetString(ZCh_ErrorObject, "Unable to match index.");
    else { 
      attrterms = sRPNQuery->rpn->u.op->u.attrTerm;
      free(sRPNQuery->rpn);
      free(sRPNQuery);
      
      databaseNames = StringToArgv(TKZ_User_Session->databaseName);
      RequestPDU = MakeScanRequest("SCAN_REFID", databaseNames, TKZ_User_Session->c_attributeSetId, attrterms, stepsize, numRequested, position);

      if (RequestPDU == NULL)
	PyErr_SetString(ZCh_ErrorObject, "Failed to create request.");
      else if (PutPDU(TKZ_User_Session->fd, RequestPDU) != Z_OK)
	PyErr_SetString(ZCh_ErrorObject, "Failed to transmit scan request.");
      else {

	(void) FreePDU(RequestPDU);
	(void) FreeArgv(databaseNames);
	TKZ_User_Session->state = S_SCAN_SENT;
	
	/* Wait for response */
	do {
	  if ((ResponsePDU = ClientGetPDU(TKZ_User_Session->fd,TKZ_User_Session->c_timeOut)) == NULL) {
	    PyErr_SetString(ZCh_ErrorObject, "Didn't get a scan response. Connection closed.");
	    tk_disconnect(TKZ_User_Session);
	    TKZ_User_Session->connectionStatus = Z_CLOSE;
	    return NULL;
	  } else  if (ResponsePDU->which  == e1_resourceControlRequest) {	
	    PyErr_SetString(ZCh_ErrorObject, "Resource Control Recieved.");
	    TKZ_User_Session->state = S_RESOURCE_CONTROL_RECEIVED;
	    return NULL;
	  } 
	} while (ResponsePDU->which != e1_scanResponse);
	
	TKZ_User_Session->state = S_SCAN_RECEIVED;

	/* ScanResults */
	scanResponse = ResponsePDU->u.scanResponse;
	le = scanResponse->entries;

	if (scanResponse->scanStatus == 6) {
	  /* Error condition */
	  if (le != NULL) {
	    if (le->nonsurrogateDiagnostics) {
	      sprintf(errorMsg, "Diagnostic %d: %s", le->nonsurrogateDiagnostics->item->u.defaultFormat->condition, GetOctetString(le->nonsurrogateDiagnostics->item->u.defaultFormat->addinfo.u.v3Addinfo));
	      PyErr_SetString(ZCh_ErrorObject, errorMsg);
	    }
	    
	  } else {
	    PyErr_SetString(ZCh_ErrorObject, "No diagnostic information available.");
	  }
	} else {

	  /* Got data */

	  returnDict = Py_BuildValue("{s:i,s:i,s:i,s:i}", "status", scanResponse->scanStatus, "numTerms", scanResponse->numberOfEntriesReturned, "stepSize", scanResponse->stepSize, "position", scanResponse->positionOfTerm);

	  entryList = PyList_New(0);

	  for (entries = le->entries; entries; entries = entries->next) {
	    if (entries->item->which == e19_surrogateDiagnostic) {
	      /* diagstring = GetOctetString((OctetString) entries->item->u.surrogateDiagnostic->u.defaultFormat->addinfo.u.v3Addinfo); */

	      /* free(diagstring); */
	    } else {
	      char *tempbuff;
	      ti = entries->item->u.termInfo;
	      tempbuff = CALLOC(char, ti->term->u.general->length + 200);
	      term = GetOctetString(ti->term->u.general);
	      if (ti->displayTerm)
		sprintf (tempbuff, "%s (%d) '%s'", term, ti->globalOccurrences, GetInternationalString(ti->displayTerm));
	      else
		sprintf (tempbuff, "%s (%d)", term, ti->globalOccurrences); 
 	      entryString = Py_BuildValue("s", tempbuff);
	      PyList_Append(entryList, entryString);
	      free(term);
	      free(tempbuff);

	    }
	  }

	  
	  PyDict_SetItem(returnDict, dictString, entryList);

	  (void) FreePDU (ResponsePDU);

	  /* Return Result List*/
	  return returnDict;

	}
      }
    }
  }
  return NULL;
}

static char ZCh_present__doc__[] =
"ZCheshire.present(position, numRecords?, resultset?)"
;

static PyObject *
ZCh_present(self, args, keywds)
     PyObject *self;	/* Not used */
     PyObject *args;
     PyObject *keywds;
{
  char *resultset = NULL;
  int numRecords = 1;
  int position;
  int i;
  int Result;

  char errorMsg[200];
  DOCUMENT *document;

  PyObject *returnDict;
  PyObject *entryList;
  PyObject *entryDict;
  PyObject *recordString;
  PyObject *entryObject;
  PyObject *dictString = Py_BuildValue("s", "records");


  static char *kwlist[] = {"position", "numRecords", "resultset", NULL};
  
  if (!PyArg_ParseTupleAndKeywords(args, keywds, "i|is", kwlist, &position, &numRecords, &resultset))
    return NULL;
  else if (numRecords < 0)
    PyErr_SetString(ZCh_ErrorObject, "numRecords parameter must be greater than -1.");
  else if ( position < 1)     
    PyErr_SetString(ZCh_ErrorObject, "position paramter must be greater than 0.");
  else if (TKZ_User_Session == NULL)
    PyErr_SetString(ZCh_ErrorObject, "There is no current session.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN) 
    PyErr_SetString(ZCh_ErrorObject, "The connection has closed.");
  else if (TKZ_User_Session->s_supportPresent != Z_TRUE)
    PyErr_SetString(ZCh_ErrorObject, "The server does not support present.");
  else if (TKZ_User_Session->searchStatus == 0)
    PyErr_SetString(ZCh_ErrorObject, "There has not yet been a successful search performed.");
  else {

    /* Check if we're presenting out of the same resultset as previously */
    if (resultset != NULL) {
      if (strcmp(TKZ_User_Session->resultSetName, strdup(resultset)) != 0) 
	result_set_changed = 1;
      else
	result_set_changed = 0;
      
      strncpy(TKZ_User_Session->resultSetName, strdup(resultset), MAX_RSN_LENGTH);
    }
    
    TKZ_User_Session->numberOfRecordsRequested = numRecords;

    /*  XXX: Make this use nextResultSetPosition if no args given */
    TKZ_User_Session->startPosition = position;
    
    if ((Result = TKZ_Present(TKZ_User_Session)) != Z_OK) {
      /* Errored Present */
      
      if (TKZ_User_Session->diagRec) {
	/* Got diagnostics */
	if (TKZ_User_Session->diagRec->addinfo.u.v2Addinfo) {
	  sprintf(errorMsg, "Diagnostic %d: %s", TKZ_User_Session->diagRec->condition, GetOctetString(TKZ_User_Session->diagRec->addinfo.u.v2Addinfo));
	} else {
	  sprintf(errorMsg, "Diagnostic %d: (No Additional Information)", TKZ_User_Session->diagRec->condition);
	}
	PyErr_SetString(ZCh_ErrorObject, errorMsg);
      } else {
	/* Local error */
	sprintf(errorMsg, "%s", TKZ_User_Session->errorMessage);
	PyErr_SetString(ZCh_ErrorObject, errorMsg);
      }
      return NULL;
    }

    
    /* Fetch records */

    returnDict = Py_BuildValue("{s:i,s:i,s:i,s:i,s:s}", "status", TKZ_User_Session->presentStatus, "received", TKZ_User_Session->noRecordsReturned, "position", TKZ_User_Session->startPosition, "nextPosition", TKZ_User_Session->nextResultSetPosition, "resultSet", TKZ_User_Session->resultSetName);
    
    entryList = PyList_New(0);
    entryDict = PyDict_New();
    
    if (TKZ_User_Session->noRecordsReturned > 0) {
      for (i=1; i <= TKZ_User_Session->noRecordsReturned; i++) {
	document = TKZ_User_Session->documentList[i];
	if (document != NULL) {
	  if (document->dtype == Z_OPAC_REC && document->data) {
	    /* PrintOPACRecord(stdout, document->data); */
	    /* Tcl_AppendElement(interp, MakeOPACListElement(document->data)); */
	  } 
	  else if (document->dtype == Z_EXPLAIN_REC) {
	    /* MakeExplainListElement(document->data,interp); */
	    entryObject = buildExplainRecord(document->data);
	    entryDict = Py_BuildValue("{s:s}", "recordSyntax", "Explain");
	    recordString = Py_BuildValue("s", "record");
	    PyDict_SetItem(entryDict, recordString, entryObject);
	    PyList_Append(entryList, entryDict);
	  }
	  else  if (document->dtype == Z_DIAG_REC) {
	    /* Should have already trapped this?! */
	  } 
	  else  if (document->dtype == Z_GRS_1_REC) {
	    entryObject = buildGRS1Record(document->data);
	    entryDict = Py_BuildValue("{s:s}", "recordSyntax", "GRS1");
	    recordString = Py_BuildValue("s", "record");
	    if (entryObject) {
	      PyDict_SetItem(entryDict, recordString, entryObject);
	    } else {
	      PyDict_SetItem(entryDict, recordString, Py_BuildValue("s", ""));
	    }
	    PyList_Append(entryList, entryDict);
	  }
	  else {
	    if (document->data && document->dsize > 0) {
	      switch (document->dtype) {
	      case Z_US_MARC:
		sprintf(errorMsg, "MARC");
		break;
	      case Z_SUTRS_REC:
		sprintf(errorMsg, "SUTRS");
		break;
	      case Z_SGML_REC:
		sprintf(errorMsg, "SGML");
		break;
	      case Z_XML_REC:
		sprintf(errorMsg, "XML");
		break;
	      case Z_SUMMARY_REC:
		sprintf(errorMsg, "SUMMARY");
		break;
	      default:
		sprintf(errorMsg, "UnknownRecSyntax");
		break;
	      }

	      entryObject = Py_BuildValue("{s:s,s:s}", "recordSyntax", errorMsg, "record", document->data);
	      PyList_Append(entryList, entryObject);
	    }
	  }
	}
      }
      /* update the start position now  */
      TKZ_User_Session->startPosition = TKZ_User_Session->nextResultSetPosition;
    }

    PyDict_SetItem(returnDict, dictString, entryList);
    
    return returnDict;

  }
  
  return NULL;
}

static char ZCh_sort__doc__[] =
"ZCheshire.sort(inResultsets, outResultSet, sortType, sortSpec, case, descending, missingValue)"
;

static PyObject *
ZCh_sort(self, args)
     PyObject *self;	/* Not used */
     PyObject *args;
{
  char *inResultSets;
  char *outResultSet;
  char *sortType;
  char *sortSpec;
  int caseSensitive = 0;
  int descending = 1;
  char *missingValue = NULL;
  PyObject *returnObject;

  PDU *RequestPDU, *ResponsePDU;
  struct sortSequence_List18 *sort_head, *sort_tail, *sort_keys;
  struct SortKey *curr_sortkey;
  struct SortElement *curr_sortelement;
  struct SortKeySpec *curr_sortkeyspec;
  RPNQuery *sRPNQuery;
  char temp[500];
  char *sortstatusmsg = "", *resultsetmsg = "", *othermsg = "", *num;
  int result_set_size = 0;
  char **in_results;

  if (!PyArg_ParseTuple(args, "ssss|iis", &inResultSets, &outResultSet, &sortType, &sortSpec, &caseSensitive, &descending, &missingValue))
    return NULL;

  if (caseSensitive != 1 && caseSensitive != 0) 
    PyErr_SetString(ZCh_ErrorObject, "CaseSensitive parameter must be 1 or 0");
  else if (descending != 1 && descending != 0)
    PyErr_SetString(ZCh_ErrorObject, "Descending parameter must be 1 or 0.");
  else if (strlen(outResultSet) == 0)
    PyErr_SetString(ZCh_ErrorObject, "OutResultSet parameter must not be empty.");
  else if (strlen(inResultSets) == 0)
    PyErr_SetString(ZCh_ErrorObject, "InResultSets parameter must not be empty.");
  else if (strlen(sortType) < 5 || strlen(sortType) > 7)
    PyErr_SetString(ZCh_ErrorObject, "SortType parameter must be one of 'field', 'index' or 'private'.");
  else if (strlen(sortSpec) == 0)
    PyErr_SetString(ZCh_ErrorObject, "SortSpec parameter must not be empty.");
  else if (TKZ_User_Session == NULL)
    PyErr_SetString(ZCh_ErrorObject, "There is no connection to sort on.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN)
    PyErr_SetString(ZCh_ErrorObject, "The connection has closed.");
  else {

    /* We only allow one operation at a time, so just alloc stuff up front */

    curr_sortkey = CALLOC(struct SortKey, 1);

    curr_sortelement = CALLOC(struct SortElement, 1);
    curr_sortelement->which = e22_generic;
    curr_sortelement->u.generic = curr_sortkey;

    curr_sortkeyspec = CALLOC(struct SortKeySpec, 1);
    curr_sortkeyspec->caseSensitivity = caseSensitive;
    curr_sortkeyspec->sortRelation = descending;
    curr_sortkeyspec->sortElement = curr_sortelement;

    if (missingValue == NULL) {
      curr_sortkeyspec->missingValueAction.which = e21_nullVal;
      curr_sortkeyspec->missingValueAction.u.nullVal = (unsigned char)1;
    } else {
      curr_sortkeyspec->missingValueAction.which = e21_missingValueData;
      curr_sortkeyspec->missingValueAction.u.missingValueData = NewOctetString(strdup(missingValue));
    }

    sort_keys = CALLOC(struct sortSequence_List18, 1);
    sort_keys->item = curr_sortkeyspec;

    sort_head = sort_keys;
    sort_tail = sort_keys;
    

    if (!strcasecmp(strdup(sortType), "private")) {

      curr_sortkey->which = e23_sortfield;
      curr_sortkey->u.sortfield = NewInternationalString(sortSpec);

    } else if (!strcasecmp(strdup(sortType), "index")) {

      curr_sortkey->which = e23_sortAttributes;
      curr_sortkey->which = e23_sortAttributes;
      /* call the command language parser to build an RPN query */
      sprintf(temp,"%s xxx", strdup(sortSpec));
      sRPNQuery = queryparse(temp, TKZ_User_Session->c_attributeSetId);

      if (sRPNQuery == NULL) {
	PyErr_SetString(ZCh_ErrorObject, "Unable to parse sortSpec.");
	return NULL;
      }
      
      curr_sortkey->u.sortAttributes.id = sRPNQuery->attributeSet;
      curr_sortkey->u.sortAttributes.list = sRPNQuery->rpn->u.op->u.attrTerm->attributes;
      free(sRPNQuery->rpn->u.op->u.attrTerm);
      free(sRPNQuery->rpn->u.op);
      free(sRPNQuery->rpn);
      free(sRPNQuery);
      
    } else if (!strcasecmp(strdup(sortType), "field")) {

      curr_sortkey->which = e23_elementSpec;
      curr_sortkey->u.elementSpec = CALLOC(Specification, 1);
      curr_sortkey->u.elementSpec->elementSpec = CALLOC(struct elementSpec, 1);
      curr_sortkey->u.elementSpec->elementSpec->which = e16_elementSetName;  
      curr_sortkey->u.elementSpec->elementSpec->u.elementSetName = NewInternationalString(strdup(sortSpec));

    } else {
      PyErr_SetString(ZCh_ErrorObject, "SortType parameter must be one of 'field', 'index' or 'private'.");
      return NULL;
    }


    in_results = StringToArgv(strdup(inResultSets));
    RequestPDU = MakeSortRequest("SORT_REFID", in_results, strdup(outResultSet), sort_head);

    if (RequestPDU == NULL) 
      PyErr_SetString(ZCh_ErrorObject, "Failed to create a sort request PDU.");
    else if (PutPDU(TKZ_User_Session->fd, RequestPDU) != Z_OK)
      PyErr_SetString(ZCh_ErrorObject, "Failed to transmit sort request. Connection may have closed.");
    else {

      (void) FreePDU (RequestPDU);

      do {
	if ((ResponsePDU = ClientGetPDU(TKZ_User_Session->fd,TKZ_User_Session->c_timeOut)) == NULL) {
	  strcpy(TKZ_User_Session->errorMessage,  "Didn't get a sort response pdu. Connection closed.");
	  PyErr_SetString(ZCh_ErrorObject, "Did not receive a sort response. Connection closed.");
	  tk_disconnect(TKZ_User_Session);
	  TKZ_User_Session->connectionStatus = Z_CLOSE;
	  return NULL;
	}
      } while (ResponsePDU->which != e1_sortResponse);
      
      
      if (ResponsePDU->u.sortResponse->sortStatus == 0  && ResponsePDU->u.sortResponse->otherInfo != NULL) {
	if (ResponsePDU->u.sortResponse->otherInfo->item.information.which   == e24_characterInfo) {
	  othermsg = GetInternationalString(ResponsePDU->u.sortResponse->otherInfo->item.information.u.characterInfo);
	  if (othermsg != NULL) {
	    /* scan the message for a number */
	    num = strpbrk(othermsg,"0123456789");
	    if (num != NULL)
	      sscanf(num,"%d",&result_set_size);
	  }
	}
      }
      
      switch (ResponsePDU->u.sortResponse->sortStatus) {
      case 0:
	sortstatusmsg = "success";
	break;
      case 1:
	sortstatusmsg = "partial";
	break;
      case 2:
	sortstatusmsg = "failure";
	break;
      }
      
      switch (ResponsePDU->u.sortResponse->resultSetStatus) {
      case 0:
	resultsetmsg = "";
	break;
      case 1:
	resultsetmsg = "empty";
	break;
      case 2:
	resultsetmsg = "interim";
	break;
      case 3:
	resultsetmsg = "unchanged";
	break;
      case 4:
	resultsetmsg = "none";
	break;
      }
      

      returnObject = Py_BuildValue("{s:i,s:s,s:i,s:s,s:s,s:i}", "status", ResponsePDU->u.sortResponse->sortStatus, "statusMsg", sortstatusmsg, "resultSetStatus", ResponsePDU->u.sortResponse->resultSetStatus, "resultSetStatusMsg", resultsetmsg, "resultSet", strdup(outResultSet), "resultSetSize", result_set_size);

      (void) FreePDU (ResponsePDU);

      return returnObject;

    }
  }
  return NULL;
}


/*  Close the current session  */

static char ZCh_close__doc__[] =
"ZCheshire.close()"
;

static PyObject *
ZCh_close(self, args)
	PyObject *self;	/* Not used */
	PyObject *args;
{

  char returnMsg[500];
  
  if (!PyArg_ParseTuple(args, ""))
    return NULL;
  
  if (TKZ_User_Session == NULL)
    PyErr_SetString(ZCh_ErrorObject, "No connection to close.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN)
    PyErr_SetString(ZCh_ErrorObject, "Connection already closed.");
  else {
    
    if (tk_disconnect(TKZ_User_Session) == Z_OK) {
      if (TKZ_User_Session->s_serverMachineName != NULL) {
	sprintf(returnMsg, "Connection closed to %s:%d.", TKZ_User_Session->s_serverMachineName, TKZ_User_Session->s_portNumber);
      } else {
	sprintf(returnMsg, "Connection closed.");
      }

      TKZ_User_Session = NULL;
      TKZ_User_Session_TMP = NULL;
      
      return Py_BuildValue("s", returnMsg);

    } else {
      PyErr_SetString(ZCh_ErrorObject, "Error closing connection.");
    }
  }
  return NULL;
}


static char ZCh_delete__doc__[] =
"ZCheshire.delete(resultSetId)"
;

static PyObject *
ZCh_delete(self, args)
     PyObject *self;	/* Not used */
     PyObject *args;
{
  char *resultsetid;

  PDU *out_pdu, *in_pdu;
  DeleteResultSetResponse	*pr;

  if (!PyArg_ParseTuple(args, "s", &resultsetid))
    return NULL;
  else if (strlen(resultsetid) == 0)
    PyErr_SetString(ZCh_ErrorObject, "ResultSetId parameter must not be empty.");
  else if (TKZ_User_Session == NULL) 
    PyErr_SetString(ZCh_ErrorObject, "Not connected.");
  else if (TKZ_User_Session->connectionStatus != Z_OPEN)
    PyErr_SetString(ZCh_ErrorObject, "Connection has closed.");
  else if (TKZ_User_Session->s_supportDeleteResultSet != Z_TRUE) 
    PyErr_SetString(ZCh_ErrorObject, "Server does not support result set deletetion.");
  else {

    TKZ_User_Session->deleteAllResultSets = Z_FALSE;
    TKZ_User_Session->resultSetList = (char **)calloc(2, sizeof(char*));
    TKZ_User_Session->numberResultSets = 1;
    TKZ_User_Session->resultSetList[0] = strdup(resultsetid);

    out_pdu = MakeDeleteResultSetRequest("DELETE_REFID", 0, TKZ_User_Session->resultSetList, TKZ_User_Session->numberResultSets);

    if (out_pdu == NULL)
      PyErr_SetString(ZCh_ErrorObject, "Could not create delete request.");
    else if (PutPDU(TKZ_User_Session->fd, out_pdu) != Z_OK) 
      PyErr_SetString(ZCh_ErrorObject, "Could not send delete request.");
    else if ((in_pdu = ClientGetPDU(TKZ_User_Session->fd, TKZ_User_Session->c_timeOut)) == NULL)
      PyErr_SetString(ZCh_ErrorObject, "Did not get delete response.");
    else {

      (void) FreePDU(out_pdu);
      pr = in_pdu->u.deleteResultSetResponse;

      /* As we only try to delete 1 at a time, just check numberNotDeleted */
      if (pr->deleteOperationStatus != 0) 
	PyErr_SetString(ZCh_ErrorObject, "Result set was not deleted by server.");
      else 
	return Py_BuildValue("s", "Result set deleted.");
    }
  }
  return NULL;
}

static char ZCh_set__doc__[] =
"ZCheshire.set(type, value)"
;

static PyObject *
ZCh_set(self, args)
     PyObject *self;	/* Not used */
     PyObject *args;
{
  char *type;
  char *value;
  char returnMsg[500];

  
  if (!PyArg_ParseTuple(args, "ss", &type, &value))
    return NULL;
  else if (strlen(type) == 0)
    PyErr_SetString(ZCh_ErrorObject, "Type parameter must not be empty.");
  else if (strlen(value) == 0)
    PyErr_SetString(ZCh_ErrorObject, "Value parameter must not be empty.");
  else if (TKZ_User_Session == NULL) 
    PyErr_SetString(ZCh_ErrorObject, "No session to set variables for.");
  else {

  /* Set code here */

    if (!strcasecmp(strdup(type), "database")) {
      strcpy(TKZ_User_Session->databaseName, strdup(value));
      sprintf(returnMsg, "Set database to %s.", value);
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "session")) {
      /* Switch sessions */
      TKZ_User_Session = LookupAssociation(&zAssociations, atoi(strdup(value)));
      if (TKZ_User_Session == NULL) {
	PyErr_SetString(ZCh_ErrorObject, "Session not found.");
      } else {
	sprintf(returnMsg, "Set session to %s.", value);
	return Py_BuildValue("s", returnMsg);
      }	      

    } else if (!strcasecmp(strdup(type), "elementSet")) {
      strcpy(TKZ_User_Session->presentElementSet, strdup(value));
      sprintf(returnMsg, "Set elementSet to '%s'.", strdup(value));
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "resultSet")) {
      strcpy(TKZ_User_Session->resultSetName, strdup(value));
      sprintf(returnMsg, "Set resultSet to '%s'.", strdup(value));
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "startPosition")) {
      TKZ_User_Session->nextResultSetPosition = TKZ_User_Session->startPosition = atoi(strdup(value));
      sprintf(returnMsg, "Set startPosition to %s.", strdup(value));
      return Py_BuildValue("s", returnMsg);
      
    } else if (!strcasecmp(strdup(type), "numRequested")) {
      TKZ_User_Session->numberOfRecordsRequested = atoi(strdup(value));
      sprintf(returnMsg, "Set numRequested to %s.", strdup(value));
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "recordSyntax")) {
      if (!strcasecmp(strdup(value), "MARC")) 
	TKZ_User_Session->preferredRecordSyntax = MARCRECSYNTAX;
      else if (!strcasecmp(strdup(value), "SUTRS")) 
	TKZ_User_Session->preferredRecordSyntax = SUTRECSYNTAX;
      else if (!strcasecmp(strdup(value), "SGML")) 
	TKZ_User_Session->preferredRecordSyntax = SGML_RECSYNTAX;
      else if (!strcasecmp(strdup(value), "XML")) 
	TKZ_User_Session->preferredRecordSyntax = XML_RECSYNTAX;
      else if (!strcasecmp(strdup(value), "HTML")) 
	TKZ_User_Session->preferredRecordSyntax = HTML_RECSYNTAX;
      else if (!strcasecmp(strdup(value), "OPAC")) 
	TKZ_User_Session->preferredRecordSyntax = OPACRECSYNTAX;
      else if (!strcasecmp(strdup(value), "EXPLAIN")) 
	TKZ_User_Session->preferredRecordSyntax = EXPLAINRECSYNTAX;
      else if (!strcasecmp(strdup(value), "SUMMARY")) 
	TKZ_User_Session->preferredRecordSyntax = SUMMARYRECSYNTAX;
      else if (!strcasecmp(strdup(value), "GRS0")) 
	TKZ_User_Session->preferredRecordSyntax = GRS0RECSYNTAX;
      else if (!strcasecmp(strdup(value), "GRS1")) 
	TKZ_User_Session->preferredRecordSyntax = GRS1RECSYNTAX;
      else if (!strcasecmp(strdup(value), "ES")) 
	TKZ_User_Session->preferredRecordSyntax = ESRECSYNTAX;
      else {
	PyErr_SetString(ZCh_ErrorObject, "Unknown recordSyntax.");
	return NULL;
      }
      sprintf(returnMsg, "Set recordSyntax to %s.", strdup(value));
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "attributeSet")) {
      if (!strcasecmp(strdup(value), "BIB1")) 
	TKZ_User_Session->c_attributeSetId = OID_BIB1;
      else if (!strcasecmp(strdup(value), "EXP1")) 
	TKZ_User_Session->c_attributeSetId = OID_EXP1;
      else if (!strcasecmp(strdup(value), "GILS")) 
	TKZ_User_Session->c_attributeSetId = OID_GILS;
      else if (!strcasecmp(strdup(value), "GEO")) 
	TKZ_User_Session->c_attributeSetId = OID_GEO;
      else if (!strcasecmp(strdup(value), "STAS")) 
	TKZ_User_Session->c_attributeSetId = OID_STAS;
      else if (!strcasecmp(strdup(value), "XD")) 
	TKZ_User_Session->c_attributeSetId = OID_XD1;
      else if (!strcasecmp(strdup(value), "UTIL")) 
	TKZ_User_Session->c_attributeSetId = OID_UTIL;
      else if (!strcasecmp(strdup(value), "EXT1")) 
	TKZ_User_Session->c_attributeSetId = OID_EXT1;
      else if (!strcasecmp(strdup(value), "CCL1")) 
	TKZ_User_Session->c_attributeSetId = OID_CCL1;
      else {
	PyErr_SetString(ZCh_ErrorObject, "Unknown attributeSet.");
	return NULL;
      }
      sprintf(returnMsg, "Set attributeSet to %s.", strdup(value));
      return Py_BuildValue("s", returnMsg);

    } else if (!strcasecmp(strdup(type), "timeout")) {
      TKZ_User_Session->c_timeOut = atoi(strdup(value));
      return Py_BuildValue("s", "Set client timeout.");
      
      
    } else {
      PyErr_SetString(ZCh_ErrorObject, "Unknown set option.");
    }
  }
  return NULL;
}

static char ZCh_get__doc__[] =
"ZCheshire.get(type)"
;

static PyObject *
ZCh_get(self, args)
     PyObject *self;	/* Not used */
     PyObject *args;
{
  char *type;
  
  if (!PyArg_ParseTuple(args, "s", &type))
    return NULL;
  else if (strlen(type) == 0)
    PyErr_SetString(ZCh_ErrorObject, "Type parameter must not be empty.");
  else if (TKZ_User_Session == NULL) 
    PyErr_SetString(ZCh_ErrorObject, "No session to retreive variables from.");
  else {

    if (!strcasecmp(strdup(type), "database")) 
      return Py_BuildValue("s", TKZ_User_Session->databaseName);
    else if (!strcasecmp(strdup(type), "elementSet")) 
      return Py_BuildValue("s", TKZ_User_Session->presentElementSet);
    else if (!strcasecmp(strdup(type), "resultSet")) 
      return Py_BuildValue("s", TKZ_User_Session->resultSetName);
    else if (!strcasecmp(strdup(type), "startPosition")) 
      return Py_BuildValue("i", TKZ_User_Session->startPosition);
    else if (!strcasecmp(strdup(type), "numRequested")) 
      return Py_BuildValue("i", TKZ_User_Session->numberOfRecordsRequested);
    else if (!strcasecmp(strdup(type), "session")) 
      return Py_BuildValue("i", TKZ_User_Session->fd);
    else if (!strcasecmp(strdup(type), "recordSyntax")) {

      if (TKZ_User_Session->preferredRecordSyntax == MARCRECSYNTAX) 
	return Py_BuildValue("s", "MARC");
      else if (TKZ_User_Session->preferredRecordSyntax == SUTRECSYNTAX) 
	return Py_BuildValue("s", "SUTRS");
      else if (TKZ_User_Session->preferredRecordSyntax == SGML_RECSYNTAX) 
	return Py_BuildValue("s", "SGML");
      else if (TKZ_User_Session->preferredRecordSyntax == XML_RECSYNTAX) 
	return Py_BuildValue("s", "XML");
      else if (TKZ_User_Session->preferredRecordSyntax == HTML_RECSYNTAX) 
	return Py_BuildValue("s", "HTML");
      else if (TKZ_User_Session->preferredRecordSyntax == OPACRECSYNTAX) 
	return Py_BuildValue("s", "OPAC");
      else if (TKZ_User_Session->preferredRecordSyntax == EXPLAINRECSYNTAX) 
	return Py_BuildValue("s", "EXPLAIN");
      else if (TKZ_User_Session->preferredRecordSyntax == SUMMARYRECSYNTAX) 
	return Py_BuildValue("s", "SUMMARY");
      else if (TKZ_User_Session->preferredRecordSyntax == GRS0RECSYNTAX) 
	return Py_BuildValue("s", "GRS0");
      else if (TKZ_User_Session->preferredRecordSyntax == GRS1RECSYNTAX) 
	return Py_BuildValue("s", "GRS1");
      else if (TKZ_User_Session->preferredRecordSyntax == ESRECSYNTAX) 
	return Py_BuildValue("s", "ES");
      else 
	PyErr_SetString(ZCh_ErrorObject, "RecordSyntax is unknown!");

    } else if (!strcasecmp(strdup(type), "attributeSet")) {

      if (TKZ_User_Session->c_attributeSetId == OID_BIB1) 
	return Py_BuildValue("s", "BIB1");
      else if (TKZ_User_Session->c_attributeSetId == OID_EXP1) 
	return Py_BuildValue("s", "EXP1");
      else if (TKZ_User_Session->c_attributeSetId == OID_GILS) 
	return Py_BuildValue("s", "GILS");
      else if (TKZ_User_Session->c_attributeSetId == OID_GEO) 
	return Py_BuildValue("s", "GEO");
      else if (TKZ_User_Session->c_attributeSetId == OID_STAS) 
	return Py_BuildValue("s", "STAS");
      else if (TKZ_User_Session->c_attributeSetId == OID_XD1) 
	return Py_BuildValue("s", "XD");
      else if (TKZ_User_Session->c_attributeSetId == OID_UTIL) 
	return Py_BuildValue("s", "UTIL");
      else if (TKZ_User_Session->c_attributeSetId == OID_EXT1) 
	return Py_BuildValue("s", "EXT1");
      else if (TKZ_User_Session->c_attributeSetId == OID_CCL1) 
	return Py_BuildValue("s", "CCL1");
      else
	PyErr_SetString(ZCh_ErrorObject, "AttributeSet is unknown!");

    } else 
      PyErr_SetString(ZCh_ErrorObject, "Unknown get option.");


  }
  return NULL;
}



/* List of methods defined in the module */

static struct PyMethodDef ZCh_methods[] = {
  {"connect",	(PyCFunction)ZCh_connect,	METH_VARARGS,	ZCh_connect__doc__},
  {"search",	(PyCFunction)ZCh_search,	METH_VARARGS,	ZCh_search__doc__},
  {"scan",	(PyCFunction)ZCh_scan,	METH_VARARGS|METH_KEYWORDS,	ZCh_scan__doc__},
  {"present",	(PyCFunction)ZCh_present,	METH_VARARGS|METH_KEYWORDS,	ZCh_present__doc__},
  {"sort",	(PyCFunction)ZCh_sort,	METH_VARARGS|METH_KEYWORDS,	ZCh_sort__doc__},
  {"close",	(PyCFunction)ZCh_close,	METH_VARARGS,	ZCh_close__doc__},
  {"delete",	(PyCFunction)ZCh_delete,	METH_VARARGS,	ZCh_delete__doc__},
  {"set",	(PyCFunction)ZCh_set,	METH_VARARGS,	ZCh_set__doc__},
  {"get",	(PyCFunction)ZCh_get,	METH_VARARGS,	ZCh_get__doc__},
  {NULL,	(PyCFunction)NULL, 0, NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initZCheshire) */

static char ZCheshire_module_documentation[] = 
"Here is documentation for the ZCheshire Python module."
;

void
initZCheshire()
{
  PyObject *m, *d;
  
  /* Create the module and add the functions */
  m = Py_InitModule4("ZCheshire", ZCh_methods,
		     ZCheshire_module_documentation,
		     (PyObject*)NULL,PYTHON_API_VERSION);
  
  /* Add some symbolic constants to the module */
  d = PyModule_GetDict(m);
  ZCh_ErrorObject = PyString_FromString("ZCheshire.error");
  PyDict_SetItemString(d, "error", ZCh_ErrorObject);
  
  /* XXXX Add constants here */
  
  /* Check for errors */
  if (PyErr_Occurred())
    Py_FatalError("Can't initialize module ZCheshire");
}

