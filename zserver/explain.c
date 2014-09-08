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
/************************************************************************
*
*       Header Name:    explain.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        Handle search and retrieval of explain information
*                      
*
*       Usage:          
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/7/93
*       Revised:        11/28/93
*       Version:        0.99999999
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif 

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"


#include "cheshire.h"
#include "configfiles.h"
#include "z_parse.h"

#include "session.h"
#include <dmalloc.h>


RPNQuery *queryparse(char *query, char *attributeSetId);
weighted_result *se_process_query(char *, RPNQuery *);
char *GetOctetString(OctetString);
char *ac_list_first(AC_LIST *l);
char *DefaultElementSetName(char *databaseName);
Explain_Record *DupExplainRecord(Explain_Record *inrec);
extern GeneralizedTime NewGeneralizedTime(char* str) ;
extern ObjectIdentifier DupOID(GSTR *oid);
#define MAXEXPLAINRECORDS 1000
Explain_Record *ExplainRecords[MAXEXPLAINRECORDS];
int LastExplainRecord = 0;

extern char *ctime(const time_t *clock);

#define STRINGVAL 0
#define INTVAL 1

/* linking information for bib-1 attributes used to build attribute details */
#define MAXUSE 10000
#define MAXREL 200
#define MAXPOS 5
#define MAXSTR 200
#define MAXTRN 200
#define MAXCMP 5
char *attr_use_string[MAXUSE];
char *attr_relation_string[MAXREL];
char *attr_position_string[MAXPOS];
char *attr_structure_string[MAXSTR];
char *attr_truncation_string[MAXTRN];
char *attr_completeness_string[MAXCMP];

typedef struct attrsettab {
  char *oid;
  int setid;
  char *name;
  char *desc;
  int maxrange;
  int minrange;
} attrsettab;

attrsettab attrsettable[] = {
 {OID_BIB1, BIB_1_ATTR_SET, "Bib-1", "Basic Bibliographic attributes", 1999, 1},
 {OID_EXP1,EXP_1_ATTR_SET, "Exp-1", "Explain Service attributes", 27, 1},
 {OID_EXT1,EXT_1_ATTR_SET, "Ext-1", "Extended Services Attributes", 7, 1},
 {OID_CCL1,CCL_1_ATTR_SET, "CCL-1", "Common Command Language attributes", 1000, 1},
 {OID_GILS,GILS_ATTR_SET, "GILS", "Government Information Locator Service attributes", 2028, 1},
 {OID_STAS, STAS_ATTR_SET, "STAS", "Scientific and Technical Attribute Set", 3379, 1},
 {OID_GEO,GEO_ATTR_SET, "GEO", "Geographic Attribute set (GEO profile)", 3999, 1},
 {OID_CIMI1, (1<<7), "CIMI-1", "Computer and Museum attribute set", 0, 0},
 {OID_ZBIG, (1<<8), "ZBig", "", 0, 0},
 {OID_UTIL, (1<<9), "Util", "Utility Attribute Set", 15, 1},
 {OID_XD1, (1<<10), "XD-1", "Cross-Domain Attribute Set", 13, 1},
 {OID_ZTHES, (1<<11),"ZThes", "Thesaurus Navigation Attribute Set", 4, 1},
 {OID_FIN1, (1<<12), "Fin-1", "Finnish Attribute Set", 0, 0},
 {OID_DAN1, (1<<13), "Dan-1", "Danish Attribute Set", 0, 0},
 {OID_HOLD, (1<<14), "Hold", "Holdings Attribute Set", 12, 1},
 {"",0, "", "", 0, 0}
};

void 
BuildBIB1Table() {
  
  int i;

  memset(attr_use_string, 0, sizeof(attr_use_string));
  memset(attr_relation_string, 0, sizeof(attr_relation_string));
  memset(attr_position_string, 0, sizeof(attr_position_string));
  memset(attr_structure_string, 0, sizeof(attr_structure_string));
  memset(attr_truncation_string, 0, sizeof(attr_truncation_string));
  memset(attr_completeness_string, 0, sizeof(attr_completeness_string));

  /* RELATION Attributes = 2 */
  attr_relation_string[/*LT*/    1] = "Less Than";
  attr_relation_string[/*LTEQ*/  2] = "Less Than or Equal To";
  attr_relation_string[/*EQUAL*/ 3] = "Equal";
  attr_relation_string[/*GTEQ*/  4] = "Greater Than or Equal To";
  attr_relation_string[/*GT*/    5] = "Greater Than";
  attr_relation_string[/*NEQ*/   6] = "Not Equal";
  attr_relation_string[/*PHONETIC*/  100] = "Phonetic";
  attr_relation_string[/*STEM*/     101] = "Stem";
  attr_relation_string[/*RELEVANCE*/ 102] = "Relevance";


  /* POSITION Attributes = 3 */
  attr_position_string[/* FIRST_IN_FIELD */ 1 ] = "First in Field";
  attr_position_string[/* FIRST_IN_SUBFIELD*/ 2 ] = "First in Subfield";
  attr_position_string[/* ANY_POSITION*/ 3 ] = "Any Position";

  /* STRUCTURE Attributes = 4*/
  attr_structure_string[/* PHRASE */ 1  ] = "phrase";
  attr_structure_string[/* WORD */ 2 ] = "word";
  attr_structure_string[/* KEY */ 3 ] = "key";
  attr_structure_string[/* YEAR */ 4 ] = "year";
  attr_structure_string[/* DATE_NORM */ 5 ] = "date (normalized)";
  attr_structure_string[/* WORD_LIST */ 6 ] = "word list";
  attr_structure_string[/* DATE_UNORM */ 100  ] = "date (un-normalized)";
  attr_structure_string[/* NAME_NORM */ 101 ] = "name (normalized)";
  attr_structure_string[/* NAME_UNORM */ 102 ] = "name (un-normalized)";
  attr_structure_string[/* STRUCTURE */ 103 ] = "structure";
  attr_structure_string[/* URX */ 104 ] = "URX";
  attr_structure_string[/* FREE_TEXT */ 105 ] = "free-form-text";
  attr_structure_string[/* DOCUMENT_TEXT */ 106 ] = "document-text";
  attr_structure_string[/* LOCAL_NUMBER */ 107 ] = "local-number";
  attr_structure_string[/* STRING */ 108 ] = "string";
  attr_structure_string[/* NUMERIC_STRING */ 109  ] = "numeric string";
  
  /* TRUNCATION attributes */
  attr_truncation_string[/* RIGHT_TRUNC */ 1 ] = "right truncation";
  attr_truncation_string[/* LEFT_TRUNC */ 2 ] = "left truncation";
  attr_truncation_string[/* LEFT_RIGHT_TRUNC */ 3 ] = "left and right truncation";
  attr_truncation_string[/* DO_NOT_TRUNCATE */ 100  ] = "do not truncate";
  attr_truncation_string[/* PROCESS_POUND */ 101 ] = "process # in search term";
  attr_truncation_string[/* REG_EXPR_ONE */ 102 ] = "regExpr-1";
  attr_truncation_string[/* REG_EXPR_TWO */ 103 ] = "regExpr-2";
  
  /* COMPLETENESS attributes */
  attr_completeness_string[/* INCOMPLETE_SUBF */ 1 ] = "incomplete subfield";
  attr_completeness_string[/* COMPLETE_SUBF */ 2 ] = "complete subfield";
  attr_completeness_string[/* COMPLETE_FIELD */ 3 ] = "complete field";
  
  /* arrange the z_parse.h entries so the PREFERRED name is first */
  for (i = 0; all_indexes[i].attributes[1] != 0; i++) {
    if (all_indexes[i].attributes[1] > 9999) {
      fprintf(stderr,"Attribute Number exceeds available space in BuildBIB1Tabe (%d)\n", all_indexes[i].attributes[1]);
      continue;
    }

    if (attr_use_string[all_indexes[i].attributes[1]] == NULL
	&& (all_indexes[i].attributes[0] & BIB_1_ATTR_SET == BIB_1_ATTR_SET))
      attr_use_string[all_indexes[i].attributes[1]] = 
	all_indexes[i].name;
  } 


}

StringOrNumeric *
BuildStringOrNumeric(char *string, int numeric, int which)
{
  StringOrNumeric *tmp;

  tmp = CALLOC(StringOrNumeric, 1);

  if (which == STRINGVAL) {
    tmp->which = e25_string;
    tmp->u.string = NewInternationalString(string);
  }
  else {
    tmp->which = e25_numeric;
    tmp->u.numeric = numeric;
  }
  
  return (tmp);
}

HumanString *
BuildHumanString(char *text, char *language)
{
  HumanString *tmp;
  
  if (text == NULL)
    return NULL;

  tmp = CALLOC(HumanString, 1);
  tmp->item.language = (LanguageCode *)NewInternationalString(language);
  tmp->item.text = NewInternationalString(text);

  return(tmp);

}

IntUnit *
BuildIntUnit(int value, char *unittype, char *unit)
{
  IntUnit *tmp;

  tmp = CALLOC(IntUnit, 1);
  tmp->value = value;
  tmp->unitUsed = CALLOC(Unit, 1);
  tmp->unitUsed->unitType = BuildStringOrNumeric(unittype, 0, STRINGVAL);
  tmp->unitUsed->unit = BuildStringOrNumeric(unit, 0, STRINGVAL);

  return(tmp);
}

ContactInfo *
BuildContactInfo(char *name, char *description,  char *address,
		 char *email, char *phone)
{
  ContactInfo *tmp;
  tmp = CALLOC(ContactInfo, 1);

  if (name != NULL && name[0] != '\0') 
    tmp->name = NewInternationalString(name);
  if (description != NULL && description[0] != '\0') 
    tmp->description = BuildHumanString(description, "eng");
  if (address != NULL && address[0] != '\0') 
    tmp->address = BuildHumanString(address, "eng");
  if (email != NULL && email[0] != '\0') 
    tmp->email = NewInternationalString(email);
  if (phone != NULL && phone[0] != '\0') 
    tmp->phone = NewInternationalString(phone);

  return (tmp);
}


NetworkAddress *
BuildNetworkAddress(char *address, int port)
{
  NetworkAddress *tmp;

  tmp = CALLOC(NetworkAddress, 1);

  tmp->which = e41_internetAddress;

  tmp->u.internetAddress.hostAddress = NewInternationalString(address);
  tmp->u.internetAddress.port = port;

  return(tmp);

}


ProximitySupport *
BuildProximitySupport(ZSESSION *session, config_file_info *config) 
{
  ProximitySupport *tmp;

  tmp = CALLOC(ProximitySupport, 1);

  /* all zeroes -- no current support */
  return (tmp);

}

RpnCapabilities *
BuildRpnCapabilities(ZSESSION *session, config_file_info *config, int type) 
{
  RpnCapabilities *tmp;

  tmp = CALLOC(RpnCapabilities, 1);
  /* we could leave it null for all operators, but... */
  tmp->operators = CALLOC(struct operators_List80, 1);
  tmp->operators->item = 0;
  tmp->operators->next = CALLOC(struct operators_List80, 1);
  tmp->operators->next->item = 1;
  tmp->operators->next->next = CALLOC(struct operators_List80, 1);
  tmp->operators->next->next->item = 2;
  tmp->resultSetAsOperandSupported = 1;
  tmp->restrictionOperandSupported = 0;
  tmp->proximity = BuildProximitySupport(session, config);
  return (tmp);
}



PrivateCapabilities *
BuildPrivateCapabilities(ZSESSION *session, config_file_info *config) 
{
  PrivateCapabilities *tmp;
  idx_list_entry *idx;
  struct searchKeys_List78 *key=NULL, *keyhead=NULL, *keylast=NULL;
  char msg[200];

  tmp = CALLOC(PrivateCapabilities, 1);

  tmp->operators = CALLOC(struct operators_List77, 1);
  tmp->operators->item.operator_var = NewInternationalString("AND");
  tmp->operators->item.description = BuildHumanString("Boolean AND -- SQL Queries",
						      "eng");
  
  tmp->operators->next = CALLOC(struct operators_List77, 1);
  tmp->operators->next->item.operator_var = NewInternationalString("OR");
  tmp->operators->next->item.description = BuildHumanString("Boolean OR -- SQL Queries",
						      "eng");
  
  tmp->operators->next->next = CALLOC(struct operators_List77, 1);
  tmp->operators->next->next->item.operator_var = NewInternationalString("NOT");
  tmp->operators->next->next->item.description = BuildHumanString("Boolean NOT -- SQL Queries",
						      "eng");
  
  for (idx = config->indexes; idx; idx = idx->next_entry) {
    keylast = key;

    key = CALLOC(struct searchKeys_List78, 1);
    key->item = CALLOC(SearchKey, 1);
    key->item->searchKey = NewInternationalString(idx->tag);
    key->item->description = BuildHumanString("Searchable column name for SQL",
						      "eng");
    if (keyhead == NULL)
      keyhead = key;
    else
      keylast->next = key;

  }

  tmp->searchKeys = keyhead;
  tmp->description = CALLOC(struct description_List79, 1);
  sprintf(msg,"SQL Queries Supported for Relational DB %s", config->file_name);
  tmp->description->item = BuildHumanString(msg, "eng");

  return (tmp);
}


QueryTypeDetails *
BuildQueryTypeDetails(ZSESSION *session, config_file_info *config,int type)
{
  QueryTypeDetails *tmp;

  tmp = CALLOC(QueryTypeDetails, 1);

  switch (type) {
  case 1:
    tmp->which = e42_rpn;
    tmp->u.erpn = BuildRpnCapabilities(session,config, type);
    break;
  case 101:
    tmp->which = e42_erpn;
    tmp->u.erpn = BuildRpnCapabilities(session,config, type);
    break;
  case 100:
    tmp->which = e42_z39_58;
    tmp->u.z39_58 = BuildHumanString(
     "Use BIB-1 USE Attribute Names for Z39.58 index specifications", "eng"); 
    break;
  case 0:
    tmp->u.private_var = BuildPrivateCapabilities(session,config); 
    break;
  }

  return(tmp);
}


struct queryTypesSupported_List68 *
BuildQueryTypes(ZSESSION *session, config_file_info *config, int file_type)
{
  struct queryTypesSupported_List68 *tmp;

 /* type 1 queries */
  tmp = CALLOC(struct queryTypesSupported_List68, 1);
  tmp->item = BuildQueryTypeDetails(session, config, 1);

#ifdef TESTINGSTUFF
  /* type 101 queries */
  tmp->next = CALLOC(struct queryTypesSupported_List68, 1);
  tmp->next->item = BuildQueryTypeDetails(session, config, 101); 

  /* type 100 (z39.58) queries */
  tmp->next->next = CALLOC(struct queryTypesSupported_List68, 1);
  tmp->next->next->item =  BuildQueryTypeDetails(session, config, 100); 

  /* type 0 SQL queries */
  if (file_type == FILE_TYPE_DBMS) {
    tmp->next->next->next = CALLOC(struct queryTypesSupported_List68, 1);
  tmp->next->next->next->item = BuildQueryTypeDetails(session, config, 0);
  }

#endif
  return (tmp);
  
}

struct attributeSetIds_List70 *
BuildExtraAttrSetIds(config_file_info *config, int common)
{
  struct attributeSetIds_List70 *ats, *first, *last;
  AttributeSetId *attsid;
  attr_map *map;
  char *attroids[30];
  config_file_info *cf;
  int i,j, lastattr, found;
  
  cf = config;

  lastattr = 0;

  ats = first = last = NULL;

  memset(attroids,0, sizeof(attroids));

  for (map = cf->bib1_attr_map; map != NULL; ) {

    if (map->attributeSetOID) {
      found = 0;
      for (j=0; j<lastattr; j++) {
	if (strcmp(attroids[j], map->attributeSetOID) == 0) {
	  found = 1;
	  break;
	}
      }
      if (found == 0) {
	/* add it to the lists */
	attroids[lastattr++] = map->attributeSetOID;
	/* creat an entry */
	ats = CALLOC(struct attributeSetIds_List70, 1);
	ats->item = NewAttributeSetId(map->attributeSetOID);
	  
	if (first == NULL) {
	  first = ats;
	  last = ats;
	}
	else {
	  last->next = ats;
	  last = ats;
	}
      }
    }

    for (i=0; i < MAXATTRTYPE; i++) {
      if (map->attr_set_oid[i] != NULL) {
	found = 0;
	for (j=0; j<lastattr; j++) {
	  if (strcmp(attroids[j], map->attr_set_oid[i]) == 0) {
	    found = 1;
	    break;
	  }
	}
	if (found == 0) {
	  /* add it to the lists */
	  attroids[lastattr++] = map->attr_set_oid[i];
	  /* creat an entry */
	  ats = CALLOC(struct attributeSetIds_List70, 1);
	  ats->item = NewAttributeSetId(map->attr_set_oid[i]);
	  
	  if (first == NULL) {
	    first = ats;
	    last = ats;
	  }
	  else {
	    last->next = ats;
	    last = ats;
	  }
	}
      }
    }

    if (map->next_attr_map_file == NULL) {
      if (common) {
	/* goto the next configfile if possible */
	if (cf->next_file_info == NULL)
	  map = map->next_attr_map_file; /* end of the line */
	else {
	  cf = cf->next_file_info;
	  map = cf->bib1_attr_map;
	}
      }
      else {
	map = map->next_attr_map_file;
      }
    }
    else {
      map = map->next_attr_map_file;
    }
  }

  /* creat an entry for the default "local index names" attribute set */
  ats = CALLOC(struct attributeSetIds_List70, 1);
  ats->item = NewAttributeSetId("1.2.840.10003.3.1000.171.9999");
	  
  if (first == NULL) {
    first = ats;
  }
  else {
    last->next = ats;
  }
    
  return (first);

}

AttributeSetInfo *
BuildAttributeSetInfo(config_file_info *config, GSTR *atsid)
{
  AttributeSetInfo *tmp;
  int i, found; 
  int setid;
  char *setname;
  char *setdesc;
  int firstuse, lastuse;
  struct attributes_List43 *atypel;
  struct attributeValues_List44 *firstval, *lastval, *val;
  AttributeType *attype;
  AttributeDescription *atdesc;
  config_file_info *cf;
  idx_list_entry *idx;

  if (config == NULL)
    return (NULL);

  tmp = CALLOC(AttributeSetInfo, 1);  

  if (atsid != NULL) {

    found = 0;
    for(i=0; found == 0 && attrsettable[i].setid != 0; i++) {
      if (strcmp(atsid->data, attrsettable[i].oid) == 0) {
	setid = attrsettable[i].setid;
	setname = attrsettable[i].name;
	setdesc = attrsettable[i].desc;
	firstuse = attrsettable[i].minrange;
	lastuse = attrsettable[i].maxrange;
	found = 1;
      }
    }
    
    if (found) {
      /* A known attribute set */ 
      tmp->attributeSet = DupOID(atsid);
      tmp->name = NewInternationalString(setname);
      tmp->description = BuildHumanString(setdesc,"eng");
      
      /* OK go through the attributes in z_parse.h and pull out the ones */
      /* for this attributeset...                                        */
      tmp->attributes = atypel = CALLOC(struct attributes_List43, 1);
      attype = atypel->item = CALLOC(AttributeType, 1);
      attype->name = NewInternationalString("Access Point");
      attype->description = BuildHumanString("USE or Access Point, i.e,Searchable Elements for this attribute set","eng");
      attype->attributeType = 1;
      firstval = lastval = val = NULL;

      for (i=0; all_indexes[i].name[0] != '\0'; i++) {
	if ((all_indexes[i].attributes[0] & setid) == setid
	    && all_indexes[i].attributes[1] >= firstuse
	    && all_indexes[i].attributes[1] <= lastuse) {
	  /* match... */

	  val = CALLOC(struct attributeValues_List44, 1);
	  atdesc = val->item = CALLOC(AttributeDescription,1);
	  atdesc->attributeValue = BuildStringOrNumeric(NULL,
                               all_indexes[i].attributes[1], INTVAL);
	  atdesc->name = NewInternationalString(&all_indexes[i].name[0]);
	  atdesc->description = BuildHumanString(&all_indexes[i].desc[0], "eng");	  
	  if (firstval == NULL) {
	    firstval = val;
	    lastval = val;
	  }
	  else {
	    lastval->next = val;
	    lastval = val;
	  }
	}
      }
      attype->attributeValues = firstval;
      
    } 
    else {
      /* handle the unrecognized or local attributesets 
       * (excluding index names, which are handled below...) 
       */
      tmp->attributeSet = DupOID(atsid);
      tmp->name = NewInternationalString("Local");
      tmp->description = BuildHumanString("Local Attribute Set","eng");
      tmp->attributes = atypel = CALLOC(struct attributes_List43, 1);
      attype = atypel->item = CALLOC(AttributeType, 1);
      attype->name = NewInternationalString("Access Point");
      attype->description = BuildHumanString("USE or Access Point, i.e,Searchable Elements for this attribute set","eng");
      attype->attributeType = 1;
      firstval = lastval = val = NULL;


      attype->attributeValues = firstval;
      
    }
  }
  else { /* null is a signal to generate the "local attribute set" */
    tmp->attributeSet = NewOID("1.2.840.10003.3.1000.171.9999");
    tmp->name = NewInternationalString("Local");
    tmp->description = BuildHumanString("Local Index Names for searching","eng");
    /* scan the configfile information for the info */
    /* generate the the Type information...         */
    tmp->attributes = atypel = CALLOC(struct attributes_List43, 1);
    attype = atypel->item = CALLOC(AttributeType, 1);
    attype->name = NewInternationalString("Access Point");
    attype->description = BuildHumanString("USE or Access Point, i.e,Searchable Elements for this attribute set","eng");
    attype->attributeType = 1;
    
    firstval = lastval = val = NULL;
    for (i=0; all_indexes[i].name[0] != '\0'; i++) {
      if (all_indexes[i].attributes[1] >= 5000
	  && all_indexes[i].attributes[1] <= 9999) {
	
	val = CALLOC(struct attributeValues_List44, 1);
	atdesc = val->item = CALLOC(AttributeDescription,1);
	atdesc->attributeValue = 
	  BuildStringOrNumeric(NULL,all_indexes[i].attributes[1], INTVAL);
	atdesc->name = NewInternationalString(&all_indexes[i].name[0]);
	atdesc->description = 
	  BuildHumanString(&all_indexes[i].desc[0], "eng");	
      
	if (firstval == NULL) {
	  firstval = val;
	  lastval = val;
	}
	else {
	  lastval->next = val;
	  lastval = val;
	}
      }
    }
    
    for (cf = config; cf != NULL; cf = cf->next_file_info) {
      for (idx = cf->indexes; idx != NULL; idx = idx->next_entry) {
	/* generate index name entry */
	val = CALLOC(struct attributeValues_List44, 1);
	atdesc = val->item = CALLOC(AttributeDescription,1);
	atdesc->attributeValue = 
	  BuildStringOrNumeric(idx->tag, 0, STRINGVAL);
	atdesc->name = NewInternationalString(idx->name);
	atdesc->description = 
	  BuildHumanString("Local Index Name", "eng");	

	if (firstval == NULL) {
	  firstval = val;
	  lastval = val;
	}
	else {
	  lastval->next = val;
	  lastval = val;
	}
      }
    }

    attype->attributeValues = firstval;

  }


    return (tmp);
}

int
not_in_esn_list(struct elementSetNames_List75 *head, char *name) 
{
  struct elementSetNames_List75 *esn;

  for (esn = head; esn != NULL; esn = esn->next) {
    if (strcasecmp(esn->item->data, name) == 0) 
      return (0);
  }

  return (1);

}

AccessInfo *
BuildAccessInfo(ZSESSION *session, config_file_info *config, int common)
{
  AccessInfo *tmp;
  int more = 1;
  config_file_info *cf;
  struct queryTypesSupported_List68 *qtl=NULL, *qtlhead=NULL, *qtllast=NULL;
  struct elementSetNames_List75 *esn=NULL, *esnhead=NULL, *esnlast=NULL;
  display_format *disp;
  int dbmsflag = 0;;

  if (config == NULL)
    return (NULL);
  
  tmp = CALLOC(AccessInfo, 1);

  /* if this is target-level do for all config files */
  if (common == 0) {
    qtlhead = BuildQueryTypes(session,config,config->file_type);
  }
  else {
    for(cf = config;cf != NULL &&  more; cf = cf->next_file_info) {
      if (cf->file_type == FILE_TYPE_DBMS && dbmsflag == 0) {
	dbmsflag = 1;
	qtlhead = BuildQueryTypes(session,cf,cf->file_type);
      }
    }
    if (dbmsflag == 0)
      qtlhead = BuildQueryTypes(session,config,FILE_TYPE_SGMLFILE);
  }

  tmp->queryTypesSupported = qtlhead;

  tmp->diagnosticsSets = CALLOC(struct diagnosticsSets_List69, 1);
  tmp->diagnosticsSets->item = NewOID(OID_DS_BIB1);
  
  tmp->attributeSetIds = BuildExtraAttrSetIds(config, common);

  tmp->recordSyntaxes = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->item = NewOID(MARCRECSYNTAX);
  tmp->recordSyntaxes->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->item = NewOID(EXPLAINRECSYNTAX);
  tmp->recordSyntaxes->next->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->next->item = NewOID(SUTRECSYNTAX);
  tmp->recordSyntaxes->next->next->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->next->next->item = NewOID(GRS1RECSYNTAX);
  tmp->recordSyntaxes->next->next->next->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->next->next->next->item = NewOID(SGML_RECSYNTAX);
  tmp->recordSyntaxes->next->next->next->next->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->next->next->next->next->item = NewOID(XML_RECSYNTAX);
  tmp->recordSyntaxes->next->next->next->next->next->next = CALLOC(struct recordSyntaxes_List72, 1);
  tmp->recordSyntaxes->next->next->next->next->next->next->item = NewOID(HTML_RECSYNTAX);

  more = 1;

  /* add the two default display formats... */
  esnhead = esn = CALLOC(struct elementSetNames_List75, 1);
  esn->item = NewInternationalString("F");
  esnlast = esn->next = CALLOC(struct elementSetNames_List75, 1);
  esn->next->item = NewInternationalString("B");

  for(cf = config;cf != NULL &&  more; cf = cf->next_file_info) {

    for (disp = cf->display; disp; disp = disp->next_format) {
      if (not_in_esn_list(esnhead, disp->name)) { 
	esn = CALLOC(struct elementSetNames_List75, 1);
	esn->item = NewInternationalString(disp->name);
	esnlast->next = esn;
	esnlast = esn;
      }
    }
    if (common == 0) /* we only do a single loop */
      more = 0;
  }

  tmp->elementSetNames = esnhead;

  return(tmp);

}

DatabaseInfo *
BuildDatabaseInfo(ZSESSION *session, config_file_info *config)
{
  DatabaseInfo *tmp;
  cluster_list_entry *cl;
  DatabaseList *head=NULL, *last=NULL, *db=NULL;
  DatabaseList *subhead=NULL, *sublast=NULL, *subdb=NULL;
  filelist *fl;
  struct nicknames_List34 *nn_head, *nn_tail, *nn;
  struct stat filestatus;
  int statresult;


  if (config->databaseinfo == NULL)
    tmp = CALLOC(DatabaseInfo, 1);
  else
    tmp = config->databaseinfo;

  tmp->name = NewInternationalString(config->nickname);

  if (config->file_type == FILE_TYPE_EXPLAIN)
    tmp->explainDatabase = 1;

  tmp->nicknames = CALLOC(struct nicknames_List34, 1);
  tmp->nicknames->item = NewInternationalString(config->file_name);

  nn_tail = tmp->nicknames;

  if (config->file_type == FILE_TYPE_VIRTUALDB) {
    for (fl = config->othernames; fl; fl = fl->next_filename) {
      sublast = subdb;
      subdb = CALLOC(DatabaseList, 1);
      subdb->item = NewInternationalString(fl->filename);
    if (subhead == NULL)
      subhead = subdb;
    else
      sublast->next = subdb;
    }
    tmp->subDbs = subhead;
  }
  else {
    for (fl = config->othernames; fl; fl = fl->next_filename) {
      nn_tail->next = nn = CALLOC(struct nicknames_List34, 1);
      nn->item = NewInternationalString(fl->filename);
      nn_tail = nn;
    }
  }

  /* IconObject	*icon; */
  /* Boolean	user_fee; */
  tmp->available = 1;
  /* HumanString	*titleString; may be in set in config*/
  /*  struct keywords_List35 {
      struct keywords_List35	*next;
      HumanString	*item;
      } *keywords;
    */
  /* HumanString	*description;may be in set in config */
  for (cl = config->clusters; cl; cl = cl->next_entry) {
    last = db;
    db = CALLOC(DatabaseList, 1);
    db->item = NewInternationalString(cl->name);
    if (head == NULL)
      head = db;
    else
      last->next = db;
  }
  tmp->associatedDbs = head;

  /*  DatabaseList	*subDbs; */
  /*  HumanString	*disclaimers; may be set in config*/
  /*  HumanString	*news; may be in set in config */
  tmp->recordCount = CALLOC(struct recordCount, 1);
  if (config->file_type != FILE_TYPE_DBMS) {
    tmp->recordCount->which = e35_actualNumber;
    /* the following actually opens all the files so we need */
    /* to close them to avoid exceeding the file number limits */
    tmp->recordCount->u.actualNumber = cf_getnumdocs(config->nickname);
    /* cf_close(config->nickname, 0);*/
  }
  else {
    tmp->recordCount->which = e35_approxNumber;
    tmp->recordCount->u.actualNumber = 5000; /* bogus */
  }
  tmp->defaultOrder = BuildHumanString("Accession Order","eng");
  tmp->avRecordSize = 0;
  tmp->maxRecordSize = 0;
  if (config->databaseinfo == NULL) {
    tmp->hours = BuildHumanString("Whenever Target is available","eng");
  /*  HumanString	*bestTime;may be in set in config */
  /*    GeneralizedTime	lastUpdate;may be in set in config */
  /*  IntUnit	*updateInterval;may be in set in config */
  /*  HumanString	*coverage;may be in set in config */
  /*  Boolean	proprietary;may be in set in config */
  /*  HumanString	*copyrightText;may be in set in config */
  /*  HumanString	*copyrightNotice; may be in set in config*/
  /*  ContactInfo	*producerContactInfo;may be in set in config */
  /*  ContactInfo	*supplierContactInfo;may be in set in config */
  /*  ContactInfo	*submissionContactInfo; may be in set in config*/    

  }

  if (tmp->lastUpdate == NULL) {
    if (config->file_type < 99) {
      /* get the lastUpdate time from the assoc file */
      if (stat(config->assoc_name, &filestatus) == 0) {
	tmp->lastUpdate = NewGeneralizedTime(ctime(&filestatus.st_mtime));
      }
    }
    else {
      /* it is a datastore file -- check the date on the db file */
      if (stat(config->file_name, &filestatus) == 0) {
	tmp->lastUpdate = NewGeneralizedTime(ctime(&filestatus.st_mtime));
      }
    }
  }
    
  tmp->accessInfo = BuildAccessInfo(session, config, 0);
  return(tmp);
}

TargetInfo *
BuildTargetInfo(ZSESSION *session, config_file_info *config)
{
  TargetInfo *tmp;
  char *c;
  struct languages_List33 *lang_head = NULL, *lang_last = NULL, *lang = NULL;
  /* struct dbCombinations_List31 *dbhead = NULL, *dblast = NULL, *db = NULL;*/

  tmp = CALLOC(TargetInfo, 1);
  if (session->s_explainstrings->targetinfo_name == NULL)
    tmp->name = 
      NewInternationalString(session->s_implementationName);
  else
    tmp->name = 
      NewInternationalString(session->s_explainstrings->targetinfo_name);
  tmp->recent_news = 
    BuildHumanString(session->s_explainstrings->targetinfo_news, "eng");
  tmp->icon = NULL;
  tmp->namedResultSets = session->namedResultSet; 
  tmp->multipleDBsearch = session->multipleDatabasesSearch;
  tmp->maxResultSets = session->maximumNumberResultSets;
  tmp->maxResultSize = 0;
  tmp->maxTerms = 0;
  tmp->timeoutInterval = BuildIntUnit(session->s_timeOut, "time", "seconds");


  tmp->addresses = CALLOC(struct addresses_List32, 1);
  tmp->addresses->item = BuildNetworkAddress(session->s_serverMachineName,
					     session->s_portNumber);

  tmp->addresses->next = CALLOC(struct addresses_List32, 1);
  tmp->addresses->next->item = 
    BuildNetworkAddress(session->s_serverIP,
			session->s_portNumber);


  tmp->welcomeMessage =     
    BuildHumanString(session->s_explainstrings->targetinfo_welcome_msg, "eng");

  tmp->contactInfo =
    BuildContactInfo(session->s_explainstrings->targetinfo_contact_name,
		     session->s_explainstrings->targetinfo_contact_description,
		     session->s_explainstrings->targetinfo_contact_address,
		     session->s_explainstrings->targetinfo_contact_email,
		     session->s_explainstrings->targetinfo_contact_phone);

  tmp->description = 
    BuildHumanString(session->s_explainstrings->targetinfo_description, "eng");

  tmp->usage_restrictions =
    BuildHumanString(session->s_explainstrings->targetinfo_usage_restriction,
		     "eng");

  tmp->paymentAddr =
    BuildHumanString(session->s_explainstrings->targetinfo_payment_address,
		     "eng");

  tmp->hours =
    BuildHumanString(session->s_explainstrings->targetinfo_hours, "eng");


  for (c = ac_list_first(session->s_explainstrings->targetinfo_languages); 
       c != NULL;
       c = ac_list_next(session->s_explainstrings->targetinfo_languages)) {
    lang_last = lang;
    lang = CALLOC(struct languages_List33, 1);
    lang->item = NewInternationalString(c); 
    if (lang_head == NULL)
      lang_head = lang;
    else
      lang_last->next = lang;
  } 
 
  tmp->commonAccessInfo = BuildAccessInfo(session, config, 1);

  return (tmp);

}

AttributeDetails *
BuildExplainDetails () {
  AttributeDetails *tmp;
  attr_map *map;
  AttributeCombinations *acomb = NULL;
  AttributeCombination *atc = NULL;
  AttributeOccurrence *ato = NULL;
  AttributeSetDetails *asd = NULL;
  struct legalCombinations_List85 *lcomb_head=NULL,*lcomb=NULL,*lcomb_last=NULL;
  struct attributesByType_List50 *abt=NULL, *abt_head=NULL, *abt_last=NULL;
  AttributeTypeDetails *atd = NULL;
  struct attributeValues_List51 *avl=NULL, *avl_head=NULL, *avl_last=NULL;
  int i;

  char *exp_attr_string[] = {"NULL",
    "ExplainCategory" /* 1 */,
    "HumanStringLanguage" /* 2 */,
    "DatabaseName" /* 3 */,
    "TargetName" /* 4 */,
    "AttributeSetOID" /* 5 */,
    "RecordSyntaxOID" /* 6 */,
    "TagSetOID" /* 7 */,
    "ExtendedServiceOID" /* 8 */,
    "DateAdded" /* 9 */,
    "DateChanged" /* 10 */,
    "DateExpires" /* 11 */,
    "ElementSetName" /* 12 */,
    "ProcessingContext" /* 13 */,
    "ProcessingName" /* 14 */,
    "TermListName" /* 15 */,
    "SchemaOID" /* 16 */,
    "Producer" /* 17 */,
    "Supplier" /* 18 */,
    "Availability" /* 19 */,
    "Proprietary" /* 20 */,
    "UserFee" /* 21 */,
    "VariantSetOID" /* 22 */,
    "UnitSystem" /* 23 */,
    "Keyword" /* 25 */,
    "ExplainDatabase" /* 26 */,
    "ProcessingOID" /* 27 */,
  };

  int exp_attr[28];
  
  exp_attr[1] = 1;
  exp_attr[3] = 1;
  exp_attr[4] = 1;
  exp_attr[5] = 1;
  exp_attr[6] = 1;
  exp_attr[7] = 1;
  exp_attr[12] = 1;
  exp_attr[25] = 1;
  

  /* build details for a database */
  tmp = CALLOC(AttributeDetails, 1);
  tmp->databaseName = NewInternationalString("IR-Explain-1");

  /* now build the attribute by set */
  tmp->attributesBySet = CALLOC(struct attributesBySet_List49, 1);
  tmp->attributesBySet->item = asd = CALLOC(AttributeSetDetails, 1);
  asd->attributeSet = NewOID(OID_EXP1);
  
  asd->attributesByType = abt = CALLOC(struct attributesByType_List50, 1);
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 1;  /* USE */

  for (i = 1; i < 27; i++) {
    if (exp_attr[i] > 0) {
      avl_last = avl;
      avl = CALLOC(struct attributeValues_List51, 1);
      if (avl_head == NULL) {
	avl_head = avl;
      }
      else
	avl_last->next = avl;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, i, INTVAL);
      avl->item->description = BuildHumanString(exp_attr_string[i], "eng");
    }
  }
  
  atd->attributeValues = avl_head;


  abt->next = CALLOC(struct attributesByType_List50, 1);
  abt = abt->next;
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 2;  /* RELATION */

  atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 3, INTVAL);
  avl->item->description = BuildHumanString("Equal", "eng");
  

  abt->next = CALLOC(struct attributesByType_List50, 1);
  abt = abt->next;
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 3;  /* POSITION */

  atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 3, INTVAL);
  avl->item->description = BuildHumanString("Any Position", "eng");
  

  abt->next = CALLOC(struct attributesByType_List50, 1);
  abt = abt->next;
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 4;  /* STRUCTURE */

  atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 3, INTVAL);
  avl->item->description = BuildHumanString("Key", "eng");
  


  return (tmp);

}


AttributeDetails *
BuildAttributeDetails(ZSESSION *session, config_file_info *config,
		      struct attributeSetIds_List70 *attrid_list)
{
  AttributeDetails *tmp;
  attr_map *map;
  attr_map *non_bib1[500];
  int count_non_bib1 = 0;
  AttributeCombinations *acomb = NULL;
  AttributeCombination *atc=NULL, *atc_head=NULL, *atc_last=NULL;
  AttributeOccurrence *ato = NULL;
  AttributeSetDetails *asd = NULL;
  AttributeSetId attrsetid = NULL;
  idx_list_entry *idx;

  struct legalCombinations_List85 *lcomb_head=NULL,*lcomb=NULL,*lcomb_last=NULL;
  struct attributesByType_List50 *abt=NULL, *abt_head=NULL, *abt_last=NULL;

  AttributeTypeDetails *atd = NULL;
  struct attributeValues_List51 *avl=NULL, *avl_head=NULL, *avl_last=NULL;
  struct attributesBySet_List49 *atbyset=NULL, *atbyset_head=NULL, *atbyset_last=NULL;
  struct attributeSetIds_List70 *asl;
  
  int attr_use[MAXUSE];
  char *attr_desc[MAXUSE];
  int attr_relation[MAXREL];
  int attr_position[MAXPOS];
  int attr_structure[MAXSTR];
  int attr_truncation[MAXTRN];
  int attr_completeness[MAXCMP];
  int i;

  /* return (NULL); remove this !!! */

  memset(attr_use, 0, sizeof(attr_use));
  memset(attr_desc,0, sizeof(attr_desc)); 
  memset(attr_relation, 0, sizeof(attr_relation));
  memset(attr_position, 0, sizeof(attr_position));
  memset(attr_structure, 0, sizeof(attr_structure));
  memset(attr_truncation, 0, sizeof(attr_truncation));
  memset(attr_completeness, 0, sizeof(attr_completeness));

  attr_truncation[3] = 1;

  /* build details for a database */
  tmp = CALLOC(AttributeDetails, 1);
  tmp->databaseName = NewInternationalString(config->nickname);

  tmp->attributeCombinations = acomb = CALLOC(AttributeCombinations, 1);
  acomb->defaultAttributeSet = NewOID(OID_BIB1);

  for (map = config->bib1_attr_map; map; map = map->next_attr_map_file) {
    
    if (map->attributeSetOID == NULL
	|| strcmp(map->attributeSetOID, OID_BIB1) == 0) {
      attr_use[map->attr_nums[USE_ATTR]]++;
      if (attr_desc[map->attr_nums[USE_ATTR]] == NULL)
	attr_desc[map->attr_nums[USE_ATTR]] = map->altdescription; 
      attr_relation[map->attr_nums[RELATION_ATTR]]++;
      attr_position[map->attr_nums[POSITION_ATTR]]++;
      attr_structure[map->attr_nums[STRUCTURE_ATTR]]++;
      attr_truncation[map->attr_nums[TRUNC_ATTR]]++;
      attr_completeness[map->attr_nums[COMPLETENESS_ATTR]]++;
    }
    else {
      non_bib1[count_non_bib1++] = map;
    }
    
    lcomb = CALLOC(struct legalCombinations_List85, 1);
    
    if (lcomb_head == NULL) {
      lcomb_head = lcomb;
      lcomb_last = lcomb;
    }
    else {
      lcomb_last->next = lcomb;
      lcomb_last = lcomb;
    }
    
    atc_head = atc_last = atc = NULL;
    
    for (i = 1; i < MAXATTRTYPE; i++) {
      if (map->attr_nums[i] != 0
	  || (map->attr_nums[i] == 0 && map->attr_string[i] != NULL)) {
	
	atc = CALLOC(AttributeCombination, 1);
	atc->item = ato = CALLOC(AttributeOccurrence, 1);
	
	if (atc_head == NULL) {
	  atc_head = atc;
	  atc_last = atc;
	}
	else {
	  atc_last->next = atc;
	  atc_last = atc;
	}
	
	ato->attributeType = i;
	
	if (map->attr_set_oid[i]) 
	  ato->attributeSet = NewOID(map->attr_set_oid[i]);
	else if (strcmp(map->attributeSetOID, OID_BIB1) != 0) {
	  ato->attributeSet = NewOID(map->attributeSetOID);
	}

	ato->attributeValues.which = e44_specific;
	ato->attributeValues.u.specific = CALLOC(struct specific_List86, 1);
	if (map->attr_nums[i] != 0)
	  ato->attributeValues.u.specific->item = 
	    BuildStringOrNumeric(NULL, map->attr_nums[i], INTVAL);
	else 
	  ato->attributeValues.u.specific->item = 
	    BuildStringOrNumeric(map->attr_string[i], 0, STRINGVAL);
	
	if (i == 1) 
	  ato->mustBeSupplied = (unsigned char) 1;
      }
    }
    /* attach this to the legal combinations */
    lcomb->item = atc_head; 
  }  

  acomb->legalCombinations = lcomb_head;

  if (lcomb_head == NULL && config->bib1_attr_map == NULL) {
    tmp->attributeCombinations = NULL;
    return (tmp);
  }


  /* now build the attributes by set */
  atbyset = atbyset_head = atbyset_last = NULL;
  
  for (asl = attrid_list; asl != NULL; asl = asl->next) {
    
    attrsetid = asl->item; 

    avl = avl_last = avl_head = NULL;

    if (attrsetid) {
      atbyset = CALLOC(struct attributesBySet_List49, 1);
            
      atbyset->item = asd = CALLOC(AttributeSetDetails, 1);
      asd->attributeSet = NewOID(attrsetid->data);
      
      asd->attributesByType = abt = CALLOC(struct attributesByType_List50, 1);
      abt->item = atd = CALLOC(AttributeTypeDetails, 1);
      atd->attributeType = 1;  /* USE */
      
      for (i = 1; i < MAXUSE; i++) {
	if (attr_use[i] > 0) {
	  avl_last = avl;
	  avl = CALLOC(struct attributeValues_List51, 1);
	  if (avl_head == NULL) {
	    avl_head = avl;
	  }
	  else
	    avl_last->next = avl;
	  avl->item = CALLOC(AttributeValue, 1);
	  avl->item->value = BuildStringOrNumeric(NULL, i, INTVAL);
	  if (attr_desc[i])
	    avl->item->description = BuildHumanString(attr_desc[i], "eng");
	  else
	    avl->item->description = BuildHumanString(attr_use_string[i], "eng");
	  
	}
      }
      
      atd->attributeValues = avl_head;

      abt->next = CALLOC(struct attributesByType_List50, 1);
      abt = abt->next;
      abt->item = atd = CALLOC(AttributeTypeDetails, 1);
      atd->attributeType = 2;  /* RELATION */
      
      atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 1, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[1], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 2, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[2], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 3, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[3], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 4, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[4], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 5, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[5], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 6, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[6], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 101, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[101], "eng");
      
      avl->next = CALLOC(struct attributeValues_List51, 1);
      avl = avl->next;
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 102, INTVAL);
      avl->item->description = BuildHumanString(attr_relation_string[102], "eng");
      
      
      abt->next = CALLOC(struct attributesByType_List50, 1);
      abt = abt->next;
      abt->item = atd = CALLOC(AttributeTypeDetails, 1);
      atd->attributeType = 3;  /* POSITION */
      
      avl_head = avl = avl_last = NULL;
      
      for (i = 1; i < MAXPOS; i++) {
	if (attr_position[i] != 0) {
	  avl_last = avl;
	  avl = CALLOC(struct attributeValues_List51, 1);
	  if (avl_head == NULL)
	    avl_head = avl;
	  avl->item = CALLOC(AttributeValue, 1);
	  avl->item->value = BuildStringOrNumeric(NULL, i, INTVAL);
	  avl->item->description = BuildHumanString(attr_position_string[i], "eng");
	  if (avl_last)
	    avl_last->next = avl;
	}
      }
      
      atd->attributeValues = avl_head;
  

      abt->next = CALLOC(struct attributesByType_List50, 1);
      abt = abt->next;
      abt->item = atd = CALLOC(AttributeTypeDetails, 1);
      atd->attributeType = 4;  /* STRUCTURE */

      avl_head = avl = avl_last = NULL;
      
      for (i = 1; i < MAXSTR; i++) {
	if (attr_structure[i] != 0) {
	  avl_last = avl;
	  avl = CALLOC(struct attributeValues_List51, 1);
	  if (avl_head == NULL)
	    avl_head = avl;
	  avl->item = CALLOC(AttributeValue, 1);
	  avl->item->value = BuildStringOrNumeric(NULL, i, INTVAL);
	  avl->item->description = BuildHumanString(attr_structure_string[i], "eng");
	  if (avl_last)
	    avl_last->next = avl;
	}
      }
      
      atd->attributeValues = avl_head;
      
      
      abt->next = CALLOC(struct attributesByType_List50, 1);
      abt = abt->next;
      abt->item = atd = CALLOC(AttributeTypeDetails, 1);
      atd->attributeType = 5;  /* truncation */
      
      /* Only handle pound sign trucation */
      atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
      avl->item = CALLOC(AttributeValue, 1);
      avl->item->value = BuildStringOrNumeric(NULL, 101, INTVAL);
      avl->item->description = BuildHumanString(attr_truncation_string[101], "eng");
      
      for (i = 1; i < MAXCMP; i++) {
	if (attr_completeness[i] != 0) {
	  
	  abt->next = CALLOC(struct attributesByType_List50, 1);
	  abt = abt->next;
	  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
	  atd->attributeType = 6;  /* completeness */
	  
	  avl_head = avl = avl_last = NULL;
	  
	  for (i = 1; i < MAXCMP; i++) {
	    if (attr_completeness[i] != 0) {
	      avl_last = avl;
	      avl = CALLOC(struct attributeValues_List51, 1);
	      if (avl_head == NULL)
		avl_head = avl;
	      avl->item = CALLOC(AttributeValue, 1);
	      avl->item->value = BuildStringOrNumeric(NULL, i, INTVAL);
	      avl->item->description = BuildHumanString(attr_structure_string[i], "eng");
	      if (avl_last)
		avl_last->next = avl;
	    }
	  }
	  atd->attributeValues = avl_head;
	  break;
	}
      }
      if (atbyset_head == NULL) {
	atbyset_head = atbyset;
	atbyset_last = atbyset;
      } 
      else {
	atbyset_last->next = atbyset;
	atbyset_last = atbyset;
      }
    } 
  }

  /* add one more set for the local names */
  /*  tmp->attributeSet = NewOID("1.2.840.10003.3.1000.171.1");
   * tmp->name = NewInternationalString("Local");
   * tmp->description = BuildHumanString("Local Index Names for searching","eng");
   */

  atbyset = CALLOC(struct attributesBySet_List49, 1);
    
  atbyset->item = asd = CALLOC(AttributeSetDetails, 1);
  asd->attributeSet = NewOID("1.2.840.10003.3.1000.171.9999"); 
    
  asd->attributesByType = abt = CALLOC(struct attributesByType_List50, 1);
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 1;  /* USE */

  avl = avl_last = avl_head = NULL;

  for (idx = config->indexes; idx != NULL; idx = idx->next_entry) {
    /* generate index name entry */
    avl = CALLOC(struct attributeValues_List51, 1);
    avl->item = CALLOC(AttributeValue, 1);
    avl->item->value =  BuildStringOrNumeric(idx->tag, 0, STRINGVAL);
    avl->item->description =  BuildHumanString(idx->name, "eng");
    if (avl_head == NULL) {
      avl_head = avl;
      avl_last = avl;
    }
    else {
      avl_last->next = avl;
      avl_last = avl;
    }
  }
      
  atd->attributeValues = avl_head;

  abt->next = CALLOC(struct attributesByType_List50, 1);
  abt = abt->next;
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 2;  /* RELATION */
      
  atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 1, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[1], "eng");
      
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 2, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[2], "eng");
      
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 3, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[3], "eng");
      
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 4, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[4], "eng");
      
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 5, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[5], "eng");
  
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 6, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[6], "eng");
  
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 101, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[101], "eng");
      
  avl->next = CALLOC(struct attributeValues_List51, 1);
  avl = avl->next;
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 102, INTVAL);
  avl->item->description = BuildHumanString(attr_relation_string[102], "eng");
      
  abt->next = CALLOC(struct attributesByType_List50, 1);
  abt = abt->next;
  abt->item = atd = CALLOC(AttributeTypeDetails, 1);
  atd->attributeType = 5;  /* truncation */
      
  /* Only handle pound sign trucation */
  atd->attributeValues = avl = CALLOC(struct attributeValues_List51, 1);
  avl->item = CALLOC(AttributeValue, 1);
  avl->item->value = BuildStringOrNumeric(NULL, 101, INTVAL);
  avl->item->description = BuildHumanString(attr_truncation_string[101], "eng");
  if (atbyset_head == NULL) {
    atbyset_head = atbyset;
    atbyset_last = atbyset;
  } 
  else {
    atbyset_last->next = atbyset;
    atbyset_last = atbyset;
  }

  tmp->attributesBySet = atbyset_head;
  
  return (tmp);
}


RecordSyntaxInfo *
BuildRecordSyntaxInfo(char *syntax, char *name, char *description)
{
  RecordSyntaxInfo *tmp;

  /* build details for RecordSyntax */
  tmp = CALLOC(RecordSyntaxInfo, 1);
  
  tmp->recordSyntax = NewOID(syntax);
  tmp->name = NewInternationalString(name);
  tmp->description = BuildHumanString(description, "eng");

  if (strcmp(syntax, EXPLAINRECSYNTAX) == 0) {
    tmp->transferSyntaxes = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->item = NewOID(EXPLAINRECSYNTAX);
  }
  else { /* we can always do GRS-1, SUTRS, and SGML */
    tmp->transferSyntaxes = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->item = NewOID(SUTRECSYNTAX);
    tmp->transferSyntaxes->next = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->next->item = NewOID(GRS1RECSYNTAX);
    tmp->transferSyntaxes->next->next = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->next->next->item = NewOID(SGML_RECSYNTAX);
    tmp->transferSyntaxes->next->next->next = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->next->next->next->item = NewOID(XML_RECSYNTAX);
    tmp->transferSyntaxes->next->next->next->next = CALLOC(struct transferSyntaxes_List41, 1);
    tmp->transferSyntaxes->next->next->next->next->item = NewOID(HTML_RECSYNTAX);
    
    if (strcmp(syntax, MARCRECSYNTAX) == 0) {
      tmp->transferSyntaxes->next->next->next->next = 
	CALLOC(struct transferSyntaxes_List41, 1);
      tmp->transferSyntaxes->next->next->next->next->item = NewOID(MARCRECSYNTAX);
 
    }
  }

  return (tmp);
}

CategoryList *
BuildCategoryList(ZSESSION *session)
{
  /* this needs to be expanded as each new_var category of information is added */
  CategoryList *tmp;
  char tmpstr[400];

  tmp = CALLOC(CategoryList, 1);

  tmp->categories = CALLOC(struct categories_List67, 1);
  tmp->categories->item = CALLOC(CategoryInfo, 1);
  tmp->categories->item->category = NewInternationalString("targetInfo");
  sprintf(tmpstr,"Description of the %s target", 
	  session->s_implementationName);
  tmp->categories->item->description = 
    BuildHumanString(tmpstr,"eng");

  tmp->categories->next = CALLOC(struct categories_List67, 1);
  tmp->categories->next->item = CALLOC(CategoryInfo, 1);
  tmp->categories->next->item->category = NewInternationalString("databaseInfo");
  tmp->categories->next->item->description = 
    BuildHumanString("Description of Each Database","eng");

  tmp->categories->next->next = CALLOC(struct categories_List67, 1);
  tmp->categories->next->next->item = CALLOC(CategoryInfo, 1);
  tmp->categories->next->next->item->category = 
    NewInternationalString("AttributeDetails");
  tmp->categories->next->next->item->description = 
    BuildHumanString("Attributes and Combinations for each Database","eng");

  tmp->categories->next->next->next = CALLOC(struct categories_List67, 1);
  tmp->categories->next->next->next->item = CALLOC(CategoryInfo, 1);
  tmp->categories->next->next->next->item->category = 
    NewInternationalString("recordSyntaxInfo");
  tmp->categories->next->next->next->item->description = 
    BuildHumanString("Available Record Syntaxes","eng");

  tmp->categories->next->next->next->next = CALLOC(struct categories_List67, 1);
  tmp->categories->next->next->next->next->item = CALLOC(CategoryInfo, 1);
  tmp->categories->next->next->next->next->item->category = 
    NewInternationalString("categoryInfo");
  tmp->categories->next->next->next->next->item->description = 
    BuildHumanString("List of supported Explain categories","eng");

  return(tmp);

}

Explain_Record *
TrimExplain(Explain_Record *inrec)
{
  Explain_Record *outrec;
  struct nicknames_List30 *nextnn, *inn;
  struct dbCombinations_List31 *nextdb, *idb;
  struct addresses_List32 *nextad, *iad;
  struct languages_List33 *nextln, *iln;
  
  outrec = DupExplainRecord(inrec);

  switch (outrec->which) {
  case e34_targetInfo:
    FreeContactInfo(outrec->u.targetInfo->contactInfo); 
    outrec->u.targetInfo->contactInfo = NULL;
    FreeHumanString(outrec->u.targetInfo->description); 
    outrec->u.targetInfo->description = NULL;
    for (inn = outrec->u.targetInfo->nicknames; inn != NULL; inn = nextnn) {
      FreeInternationalString(inn->item);
      nextnn = inn->next;
      (void) free ((char *)inn);
    }    
    outrec->u.targetInfo->nicknames = NULL;
    FreeHumanString(outrec->u.targetInfo->usage_restrictions);
    outrec->u.targetInfo->usage_restrictions = NULL;
    FreeHumanString(outrec->u.targetInfo->paymentAddr);
    outrec->u.targetInfo->paymentAddr = NULL;
    FreeHumanString(outrec->u.targetInfo->usage_restrictions);
    outrec->u.targetInfo->usage_restrictions = NULL;
    FreeHumanString(outrec->u.targetInfo->paymentAddr);
    outrec->u.targetInfo->paymentAddr = NULL;
    FreeHumanString(outrec->u.targetInfo->hours);
    outrec->u.targetInfo->hours = NULL;
    
    for (idb = outrec->u.targetInfo->dbCombinations; idb != NULL; idb = nextdb) {
      FreeDatabaseList(idb->item);
      nextdb = idb->next;
      (void) free ((char *)idb);
    }
    outrec->u.targetInfo->dbCombinations = NULL;

    for (iad = outrec->u.targetInfo->addresses; iad != NULL; iad = nextad) {
      FreeNetworkAddress(iad->item);
      nextad = iad->next;
      (void) free ((char *)iad);
    }
    
    outrec->u.targetInfo->addresses = NULL;

    for (iln = outrec->u.targetInfo->languages; iln != NULL; iln = nextln) {
      FreeInternationalString(iln->item);
      nextln = iln->next;
      (void) free ((char *)iln);
    }
    outrec->u.targetInfo->languages = NULL;
    FreeAccessInfo(outrec->u.targetInfo->commonAccessInfo);
    outrec->u.targetInfo->commonAccessInfo = NULL;

    break;

  case e34_databaseInfo:

    FreeHumanString(outrec->u.databaseInfo->description);
    outrec->u.databaseInfo->description = NULL;
    FreeDatabaseList(outrec->u.databaseInfo->associatedDbs);
    outrec->u.databaseInfo->associatedDbs = NULL;
    FreeDatabaseList(outrec->u.databaseInfo->subDbs);
    outrec->u.databaseInfo->subDbs = NULL;
    FreeHumanString(outrec->u.databaseInfo->disclaimers);
    outrec->u.databaseInfo->disclaimers = NULL;
    FreeHumanString(outrec->u.databaseInfo->news);
    outrec->u.databaseInfo->news = NULL;
    FreeHumanString(outrec->u.databaseInfo->defaultOrder);
    outrec->u.databaseInfo->defaultOrder = NULL;
    FreeHumanString(outrec->u.databaseInfo->hours);
    outrec->u.databaseInfo->hours = NULL;
    FreeHumanString(outrec->u.databaseInfo->bestTime);
    outrec->u.databaseInfo->bestTime = NULL;
    FreeGeneralizedTime(outrec->u.databaseInfo->lastUpdate);
    outrec->u.databaseInfo->lastUpdate = NULL;
    FreeIntUnit(outrec->u.databaseInfo->updateInterval);
    outrec->u.databaseInfo->updateInterval = NULL;
    FreeHumanString(outrec->u.databaseInfo->coverage);
    outrec->u.databaseInfo->coverage = NULL;
    FreeHumanString(outrec->u.databaseInfo->copyrightText);
    outrec->u.databaseInfo->copyrightText = NULL;
    FreeHumanString(outrec->u.databaseInfo->copyrightNotice);
    outrec->u.databaseInfo->copyrightNotice = NULL;
    FreeContactInfo(outrec->u.databaseInfo->producerContactInfo);
    outrec->u.databaseInfo->producerContactInfo = NULL;
    FreeContactInfo(outrec->u.databaseInfo->supplierContactInfo);
    outrec->u.databaseInfo->supplierContactInfo = NULL;
    FreeContactInfo(outrec->u.databaseInfo->submissionContactInfo);
    outrec->u.databaseInfo->submissionContactInfo = NULL;
    FreeAccessInfo(outrec->u.databaseInfo->accessInfo);
    outrec->u.databaseInfo->accessInfo = NULL;
    
    break;
  case e34_schemaInfo:
    break;
  case e34_tagSetInfo:
    break;
  case e34_recordSyntaxInfo: {
    RecordSyntaxInfo *in;
    struct transferSyntaxes_List41 *nextts, *its;
    struct abstractStructure_List42 *nextas, *ias;

    in = outrec->u.recordSyntaxInfo;

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
  }
  break;
  
  case e34_attributeSetInfo:
    break;
  case e34_termListInfo:
    break;
  case e34_extendedServicesInfo:
    break;
  case e34_attributeDetails:
    break;
  case e34_termListDetails:
    break;
  case e34_elementSetDetails:
    break;
  case e34_retrievalRecordDetails:
    break;
  case e34_sortDetails:
    break;
  case e34_processing:
    break;
  case e34_variants:
    break;
  case e34_units:
    break;
  case e34_categoryList:
    break;
  }

  return (outrec);
}  
  
int
BuildExplainRecords(ZSESSION *session, config_file_info *config)
{
  config_file_info *cf;
  struct attributeSetIds_List70 *atsid;

  ExplainRecords[0] = CALLOC(Explain_Record, 1);
  ExplainRecords[0]->which = e34_targetInfo;
  ExplainRecords[0]->u.targetInfo = 
    BuildTargetInfo(session, config);

  LastExplainRecord = 1;

  BuildBIB1Table();

  for (cf = config; cf ; cf = cf->next_file_info) {
    ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
    ExplainRecords[LastExplainRecord]->which = e34_databaseInfo;
    ExplainRecords[LastExplainRecord++]->u.databaseInfo = 
      BuildDatabaseInfo(session, cf);

    ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
    ExplainRecords[LastExplainRecord]->which = e34_attributeDetails;
    ExplainRecords[LastExplainRecord++]->u.attributeDetails = 
      BuildAttributeDetails(session, cf, 
	 ExplainRecords[0]->u.targetInfo->commonAccessInfo->attributeSetIds);

    cf_close(cf->nickname, 0);
  }

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_attributeDetails;
  ExplainRecords[LastExplainRecord++]->u.attributeDetails = 
    BuildExplainDetails();
  
  for (atsid = ExplainRecords[0]->u.targetInfo->commonAccessInfo->attributeSetIds;
       atsid != NULL; atsid = atsid->next) {
    ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
    ExplainRecords[LastExplainRecord]->which = e34_attributeSetInfo;
    ExplainRecords[LastExplainRecord++]->u.attributeSetInfo = 
      BuildAttributeSetInfo(config, (GSTR *)atsid->item);
    
  }
  /* one more, for the "local attribute set" with index names */
  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_attributeSetInfo;
  ExplainRecords[LastExplainRecord++]->u.attributeSetInfo = 
    BuildAttributeSetInfo(config, (GSTR *)NULL);
    

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(SGML_RECSYNTAX, "SGML", "All Databases are SGML or XML"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(XML_RECSYNTAX, "XML", "All Databases are XML or SGML"); 
  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(HTML_RECSYNTAX, "HTML", "HTML conversion available for some databases"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(GRS1RECSYNTAX, "GRS-1", "GRS-1 Translations"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(SUTRECSYNTAX, "SUTRS", "SUTRS -- actually SGML"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(MARCRECSYNTAX, "USMARC", 
			  "Available for MARCSGML databases"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_recordSyntaxInfo;
  ExplainRecords[LastExplainRecord++]->u.recordSyntaxInfo = 
    BuildRecordSyntaxInfo(EXPLAINRECSYNTAX, "EXP-1", 
			  "Explain Database only"); 

  ExplainRecords[LastExplainRecord] = CALLOC(Explain_Record, 1);
  ExplainRecords[LastExplainRecord]->which = e34_categoryList;
  ExplainRecords[LastExplainRecord++]->u.categoryList = 
    BuildCategoryList(session);

  LastExplainRecord--;
  return (0);
}

weighted_result *
GetExplainType(int type, char *DBNAME)
{
  int i;
  int count = 0;
  weighted_result *wt_res;

  /* first do a count */
  for (i = 0; i <= LastExplainRecord; i++) {
    if (ExplainRecords[i]->which == type) {
      count++;
    }
  }
  
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + (count * sizeof(weighted_entry))));
    
  wt_res->num_hits = 0;
  wt_res->setid[0] = '\0';
    
  for (i = 0; i <= LastExplainRecord; i++) {
    if (ExplainRecords[i]->which == type) {
      if (DBNAME == NULL) {
	wt_res->entries[wt_res->num_hits].record_num = i;
	wt_res->entries[wt_res->num_hits++].weight = 1.0;
      } else {
	/* have a database name */
	if ((ExplainRecords[i]->which == e34_attributeDetails
	     || ExplainRecords[i]->which == e34_databaseInfo)
	    && strncasecmp(DBNAME, 
			   ExplainRecords[i]->u.attributeDetails->databaseName->data,
			   ExplainRecords[i]->u.attributeDetails->databaseName->length) == 0) {
	  wt_res->entries[wt_res->num_hits].record_num = i;
	  wt_res->entries[wt_res->num_hits++].weight = 1.0;
	}

      }
    }
  }
    
  return(wt_res);

}


weighted_result *
Process_Operand (Operand *op, char *databasename, int *diagnostic) {

  weighted_result *final_set;	/* result from query processing */
  Term *term;
  AttributeList *l;
  AttributeElement *elem; 
  char diag[100];
  char *termval;
  int  termintval;
  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  int i;

  term = op->u.attrTerm->term;
  switch (term->which) {
    
  case e5_general:
    termval = GetOctetString(term->u.general);
    break;
    
  case e5_numeric:
    termintval = term->u.numeric;
    break;
    
  case e5_characterString:
    termval = GetInternationalString(term->u.characterString);
    break;
    
  case e5_oid:
    termval = GetOidStr(term->u.oid);      
    break;
    
  case e5_dateTime:
  case e5_external:
  case e5_integerAndUnit:
  case e5_nullVal:
    diagnostic_set(124,0,"Term type not supported");
    *diagnostic = 1;
    return(NULL);           
    break;
    
  }
  
  for (l = op->u.attrTerm->attributes; l; l = l->next) {
    
    elem = l->item;
    
    if (elem->attributeValue.which == e7_numeric) {
      switch (elem->attributeType) {
      case 1: use = elem->attributeValue.u.numeric;
	break;
      case 2: relation = elem->attributeValue.u.numeric;
	break;
      case 3: position = elem->attributeValue.u.numeric;
	break;
      case 4: structure = elem->attributeValue.u.numeric;
	break;
      case 5: truncation = elem->attributeValue.u.numeric;
	break;
      case 6: completeness = elem->attributeValue.u.numeric;
	break;
      }
    }
    else {
      /* we don't handle complex types yet ??? */
      diagnostic_set(113,0,"Complex attribute values not supported");
    *diagnostic = 1;
      return(NULL);
    }
  }
  

  switch (use) { /* From : Table A-3-7: Exp-1 Use Attributes */
      
      /* ExplainCategory */
  case 1:
      
      
    if (strcasecmp(termval,"TargetInfo") == 0) {
      final_set = GetExplainType(e34_targetInfo, NULL);
    }
    else if (strcasecmp(termval,"DatabaseInfo") == 0) {
      final_set = GetExplainType(e34_databaseInfo, databasename);
    }
    else if (strcasecmp(termval,"RecordSyntaxInfo") == 0) {
      final_set = GetExplainType(e34_recordSyntaxInfo, databasename);
    }
    else if (strcasecmp(termval,"AttributeSetInfo") == 0) {
      final_set = GetExplainType(e34_attributeSetInfo, databasename);
    }
    else if (strcasecmp(termval,"CategoryList") == 0) {
      final_set = GetExplainType(e34_categoryList, NULL);
    }
    else if (strcasecmp(termval,"SchemaInfo") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"TagSetInfo") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"TermListInfo") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"extendedServicesInfo") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"AttributeDetails") == 0) {
      final_set = GetExplainType(e34_attributeDetails, databasename);

    }
    else if (strcasecmp(termval,"TermListDetails") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"ElementSetDetails") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"RetrievalRecordDetails") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"SortDetails") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"Processing") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"VariantSetInfo") == 0) {
      final_set = NULL;
    }
    else if (strcasecmp(termval,"UnitInfo") == 0) {
      final_set = NULL;
    }
    else {
      final_set = NULL;
    }
    
    break;
      
      
    /* HumanStringLanguage */
  case 2:
    if (strcasecmp(termval,"eng") !=0 )
      final_set = NULL;
    else
      final_set = NULL; /* we arent doing this yet -- need full RPN parsing*/
    break;
    
      
    /* DatabaseName */
  case 3:
    if (termval != NULL) {
      for (i = 0; i < LastExplainRecord; i++) {
	if (ExplainRecords[i]->which == e34_databaseInfo) {
	  if (strcasecmp(ExplainRecords[i]->u.databaseInfo->name->data, 
			 termval) == 0) {
	    final_set = CALLOC(weighted_result, 1);
	    final_set->num_hits = 1;
	    final_set->entries[0].record_num = i;
	    final_set->entries[0].weight = 1.0;
	    break;
	  }
	}
      }
    }
    else {
      diagnostic_set(126,0,"Database name required");
    *diagnostic = 1;
      return(NULL);
    }
    break;
    
    
    /* TargetName */
  case 4:
    final_set = NULL;
    break;
    
    
    /* AttributeSetOID */
  case 5:
    final_set = NULL;
    break;
    
    
    /* RecordSyntaxOID */
  case 6:
    if (termval != NULL) {
      for (i = 0; i < LastExplainRecord; i++) {
	if (ExplainRecords[i]->which == e34_recordSyntaxInfo) {
	  if (strcasecmp(ExplainRecords[i]->u.recordSyntaxInfo->recordSyntax->data, 
			 termval) == 0) {
	    final_set = CALLOC(weighted_result, 1);
	    final_set->num_hits = 1;
	    final_set->entries[0].record_num = i;
	    final_set->entries[0].weight = 1.0;
	    break;
	  }
	}
      }
    }
    else {
      diagnostic_set(126,0,"Record Syntax OID required");
    *diagnostic = 1;
      return(NULL);
    }
    break;
    
    
    /* TagSetOID */
  case 7:
    final_set = NULL;
    break;
    
    
    /* ExtendedServiceOID */
  case 8:
    final_set = NULL;
    break;
    
    
    /* DateAdded */
  case 9:
    final_set = NULL;
    break;
    
    
    /* DateChanged */
  case 10:
    final_set = NULL;
    break;
    
    
    /* DateExpires */
  case 11:
    final_set = NULL;
    break;
    
    
    /* ElementSetName */
  case 12:
    final_set = NULL;
    break;
    
    
    /* ProcessingContext */
  case 13:
    /*Table A-3-9: Search terms associated with use attribute 
      ProcessingContext */
    if (strcasecmp(termval, "Access") == 0) {
    }
    else if (strcasecmp(termval, "Search") == 0) {
    }
    else if (strcasecmp(termval, "Retrieval") == 0) {
    }
    else if (strcasecmp(termval, "RecordPresentation") == 0) {
    }
    else if (strcasecmp(termval, "RecordHandling") == 0) {
    }
    
    final_set = NULL;
    
    break;
    
    
    /* ProcessingName */
  case 14:
    final_set = NULL;
    break;
    
    
    /* TermListName */
  case 15:
    final_set = NULL;
    break;
    
    
    /* SchemaOID */
  case 16:
    final_set = NULL;
    break;
    
    
    /* Producer */
  case 17:
    final_set = NULL;
    break;
    
    
    /* Supplier */
  case 18:
    final_set = NULL;
    break;
    
    
    /* Availability */
  case 19:
    final_set = NULL;
    break;
    
    
    /* Proprietary */
  case 20:
    final_set = NULL;
    break;
    
    
    /* UserFee */
  case 21:
    final_set = NULL;
    break;
    
    
    /* VariantSetOID */
  case 22:
    final_set = NULL;
    break;
    
      
    /* UnitSystem */
  case 23:
    final_set = NULL;
    break;
      
    
    /* Keyword */
  case 25:
    final_set = NULL;
    break;
    
    
    /* ExplainDatabase */
  case 26:
    final_set = NULL;
    break;
    
    
    /* ProcessingOID */
  case 27:
    final_set = NULL;
    break;
    
  default:
    sprintf(diag, "%d", use);
    diagnostic_set(114,0,diag);
    *diagnostic = 1;
    return(NULL);
    
    
  }

  return (final_set);

}

int
check_attrlist_for_explain(AttributeList *attr)
{
  AttributeList *at;
  AttributeElement *elem;

  for (at = attr; at != NULL; at = at->next) {
    elem = at->item;
    if (elem != NULL && elem->attributeSet != NULL) {
      if (strcasecmp(elem->attributeSet->data, OID_EXP1) == 0) {
	return (1);
      }
    }
  }
  return (0);
}

int
check_rpn_for_explain(RPNStructure *s)
{
  int returnval = 0;

  /* have to do some additional checking for attribute set */
  /* unsupported attribute set for explain */
  
  if (s->which == e3_rpnRpnOp) {
    if (s->u.rpnRpnOp.rpn1->which != e3_op 
	|| s->u.rpnRpnOp.rpn2->which != e3_op) {
      /* can't have nested junk */
      return (0);
    }
    
    returnval = check_attrlist_for_explain(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->attributes);
    if (returnval == 1) 
      return(1);
    else
      return(check_attrlist_for_explain(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->attributes));

  }
  else if (s->which == e3_op) {
    return(check_attrlist_for_explain(s->u.op->u.attrTerm->attributes));
  }

}


int 
ExplainSearch (ZSESSION *session, Query *query)
{
  extern SGML_Doclist  *PRS_first_doc;
  RPNQuery *RPN_Query_ptr = NULL;
  RPNStructure *s;
  int diagnostic;
  char *QueryAttrSet = NULL;
  int replaceflag;
  int i;
  any SQL_Query = NULL;
  char diag[100];
  char *databasename = NULL;
  weighted_result *final_set = NULL; /* result from query processing */
  
  /* remove any diagnostics from the last search */
  diagnostic_clear();

  diagnostic = 0;

  current_user_id(session);    /* set the "user id" e.g. connection number */

  cheshire_log_file = session->s_lfptr;
  LOGFILE = session->s_lfptr; /* all error, etc. output to the same place */

  if (LOGFILE != stderr) /* don't put out stuff like this in web output */
    fprintf(LOGFILE,"Starting Explain query processing\n");

  if (cf_info_base == NULL) {
    InitializeSearchEngine(session);
    
  }

  if (LastExplainRecord == 0) {
    BuildExplainRecords(session, cf_info_base);
  }

  if (query->which == e2_type_1) {
    RPN_Query_ptr = query->u.type_1;
  }
  else if (query->which == e2_type_101) {
    RPN_Query_ptr = query->u.type_101;
  }
  else if (query->which == e2_type_100) {
    /* assume exp1 for these */
    RPN_Query_ptr = queryparse(GetOctetString(query->u.type_100), OID_EXP1);
  }
  else if (query->which == e2_type_0) {
    SQL_Query = (char *)query->u.type_0;
  } 
  else if (query->which == e2_type_102) {
    /*RPN_Query_ptr = queryparse(GetOctetString(query->u.type_102,OID_BIB1);*/
    fprintf(LOGFILE, "ERROR: Type 102 Query not supported yet\n");
    diagnostic_set(3,0,"Type 102 Query not supported yet");
    return(diagnostic_get()); 
    
  }
  
  if (RPN_Query_ptr == NULL) {
    diagnostic_set(107,0,"Only Type 1, 100, or 101 queries supported");
    return(diagnostic_get()); 
  }

  s = RPN_Query_ptr->rpn;

  QueryAttrSet = GetOidStr(RPN_Query_ptr->attributeSet);

  if (QueryAttrSet == NULL || strcasecmp(QueryAttrSet, OID_EXP1) != 0) {
    if (check_rpn_for_explain(s) == 0) {
      diagnostic_set(121,0,QueryAttrSet);
      return(diagnostic_get()); 
    }
  }
  
  /* need to handle boolean combinations */
  if (s->which == e3_rpnRpnOp) {
    if (s->u.rpnRpnOp.rpn1->which == e3_op &&
	s->u.rpnRpnOp.rpn2->which == e3_op &&
	s->u.rpnRpnOp.op->which == e6_and) {
      /* it is a boolean AND */
      /* these make BIG assumptions that the data will really be there... */
      /* this will need MUCH MORE work if other booleans are to be added  */
      if (strcasecmp(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->term->u.general->data, 
		     "Attributedetails") == 0) {
	
	/* verify that the other branch is a databasename... */
	databasename = GetOctetString(s->u.rpnRpnOp.rpn2->u.op->u.attrTerm->term->u.general);
	final_set = Process_Operand (s->u.rpnRpnOp.rpn1->u.op, databasename, 
				     &diagnostic);
	
      } else if (strcasecmp(s->u.rpnRpnOp.rpn2->u.op->u.attrTerm->term->u.general->data, 
			    "Attributedetails") == 0) {
	databasename = GetOctetString(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->term->u.general);
	final_set = Process_Operand (s->u.rpnRpnOp.rpn2->u.op, databasename, 
				     &diagnostic);
      } else if (strcasecmp(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->term->u.general->data, 
			    "DatabaseInfo") == 0) {
	
	/* verify that the other branch is a databasename... */
	databasename = GetOctetString(s->u.rpnRpnOp.rpn2->u.op->u.attrTerm->term->u.general);
	final_set = Process_Operand (s->u.rpnRpnOp.rpn1->u.op, databasename, 
				     &diagnostic);
	
      } else if (strcasecmp(s->u.rpnRpnOp.rpn2->u.op->u.attrTerm->term->u.general->data, 
			    "DatabaseInfo") == 0) {
	databasename = GetOctetString(s->u.rpnRpnOp.rpn1->u.op->u.attrTerm->term->u.general);
	final_set = Process_Operand (s->u.rpnRpnOp.rpn2->u.op, databasename, 
				     &diagnostic);
      } else {
	diagnostic_set(110,0,"Limited Boolean support in Explain DB (database name plus DatabaseInfo or AttributeDetails ONLY)");
	return(diagnostic_get()); 
      }
 
    }
    else {
      diagnostic_set(110,0,"Only SINGLE AND supported in Explain DB");
      return(diagnostic_get()); 
    }
  }
  else if (s->which == e3_op) {
    
    if (s->u.op->which == e4_resultSet) {
      /* this is a query syntax error */
      diagnostic_set(18,0,"query syntax error?");
      return(diagnostic_get()); 
    }

    final_set = Process_Operand (s->u.op, NULL, &diagnostic);

  }


    
  /***********************************************************************
    Notes:
    (1) The search terms for Use attribute ExplainCategory are listed in table
    A-3-8.
    
    (2) The search term when the Use attribute is HumanStringLanguage is the
    three-character language code from ANSI/NISO Z39.53-1994 -- Codes for
    the Representation of Languages for Information Interchange.
    
    (3) The search terms when the Use attribute is ProcessingContext are
    listed in table A-3-9.
    
    (4) Where the search term is an object identifier (where the name of the
    Use attribute ends with "OID"): for version 2, it is recommended that
    the term be a character string representing a sequence of integers
    (each represented by a character string) separated by periods. For
    version 3, it is recommended that the term be represented as ASN.1
    type OBJECT IDENTIFIER. 
    
    (5) Use attribute Keyword is used when searching for DatabaseInfo records
    (i.e. in combination with an operand where Use is ExplainCategory and
    term is DatabaseInfo). Its use is to search in the keyword element,
    for terms that match one of the query terms. 
    
    (6) Use attribute ExplainDatabase is used when searching for DatabaseInfo
    records (i.e. in combination with an operand where Use is
    ExplainCategory and term is DatabaseInfo). The term should be NULL,
    for version 3, or otherwise ignored by the target. The Relation
    attribute either should be omitted or should be AlwaysMatches.
    **********************************************************************/  
  
  if (final_set == NULL) {
    diagnostic_set(124,0,"Term type not supported");
    return(diagnostic_get());       
  }
  replaceflag = 1;

  if (session->disable_saving_result_sets)
    replaceflag |= 4; /* no save flag */

  sprintf(final_set->filename, "IR-Explain-1");

  se_store_result_set(final_set, session->resultSetName, 
		      session->databaseName, 
		      session->c_resultSetDir, replaceflag);
  
  session->resultCount = final_set->num_hits; 
  
  if (LOGFILE != stderr)
    fprintf(LOGFILE,
	    "Final Set Num_hits = %d\n",final_set->num_hits);
  
  return(0);
}


int 
ExplainPresent(char **outptr, ZSESSION *Session, 
		   int record_id, int *result_recsize)
{
  weighted_result *final_set, *se_get_result_set();
  char *filename, *se_get_result_filetag();
  int result_rec_num;
  char diag[100];

  final_set = se_get_result_set(Session->resultSetName);
    
  if (final_set == NULL) {
    return(diagnostic_get());
  }
    
  filename = se_get_result_filetag(Session->resultSetName);
  
  if (strcasecmp(filename, "IR-EXPLAIN-1") != 0) {
    sprintf(diag, "Result set %s not from Explain Search", 
	    Session->resultSetName);
    diagnostic_set(128,0,diag);
    return(diagnostic_get());
  }
  if (record_id >= 1 && record_id <= final_set->num_hits) {
    
    result_rec_num = final_set->entries[record_id-1].record_num;
    if (strcmp(Session->presentElementSet, "B") == 0) {
      *outptr = (char *) TrimExplain(ExplainRecords[result_rec_num]);
    }
    else
    *outptr = (char *)ExplainRecords[result_rec_num];
    *result_recsize = 1; /* we don't calculate this here */
    
    return(0);
  }
  else {
    sprintf(diag, "Record %d not in result set", record_id);
    diagnostic_set(13,0,diag);
    return(13);
  }

}

