/*
 *  Copyright (c) 1990-2002 [see Other Notes, below]. The Regents of the
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
*	Header Name:	xml_schema_2_dtd.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_Document structure
*                       that contains an XMLSchema document and generates
*                       SGML_DTD structures from it...
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		4/30/2002
*	Revised:	
*	Version:	1.0
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#if (!defined(WIN32) && !defined(MACOSX))
#include <values.h>
#else
#define HIBITI  ((unsigned)1 << 31)
#define MAXINT ((int)(~HIBITI))
#endif
#include "cheshire.h"

#define DEBUGSCHEMA 1

#define ALLOC_ERROR 100
#define UNDEFINED_TAG_ERROR 200
#define PARSE_ERROR 1
#define INFORM_ERROR 0

extern void cheshire_exit (int code);

extern SGML_DTD *DTD_list, *last_sgml_dtd, *current_sgml_dtd;
extern SGML_Declaration *current_sgml_declaration, XML_Declaration;

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern SGML_Tag_Data *exclude_tag_data( SGML_Tag_Data *exclude_tags, 
				 SGML_Tag_Data *data_tags);

int xml_schema_defines_ANY = 0;
Tcl_HashTable *Current_Doc_Namespaces = NULL;

SGML_Sub_Element_List *
xml_create_primitive_dm(XML_type_def *current_def)
{
  SGML_Sub_Element_List *curr;

  curr = CALLOC(SGML_Sub_Element_List, 1);
  curr->sub_element_type = current_def->primitivebasetype;
  return(curr);

}


SGML_Sub_Element_List *
xml_schema_dup_model(SGML_Element *parent, SGML_Sub_Element_List *in_list)
{
  SGML_Sub_Element_List *curr, *next;

  if (in_list == NULL)
    return NULL;

  next = xml_schema_dup_model(parent, in_list->next_sub_element);

  curr = CALLOC(SGML_Sub_Element_List, 1);
  /* first the simple copies */
  curr->element_parent = parent; 
  curr->sub_element_type = in_list->sub_element_type;
  if (in_list->element_name)
    curr->element_name = strdup(in_list->element_name);
  curr->element = in_list->element; 
  curr->group = xml_schema_dup_model(parent, in_list->group);
  if (in_list->group_repetition)
    curr->group_repetition = strdup(in_list->group_repetition);
  if (in_list->repetition)
    curr->repetition = strdup(in_list->repetition);
  curr->min_occurs = in_list->min_occurs; 
  curr->max_occurs = in_list->max_occurs; 
  if (in_list->xml_type_name)
    curr->xml_type_name = strdup(in_list->xml_type_name);
  if (in_list->connector)
    curr->connector = strdup(in_list->connector);
  curr->next_sub_element = next;

  return(curr);

}



int init_xml_builtin_types(Tcl_HashTable *tab) 
{
  Tcl_HashEntry *entry; 
  int exists;
  char *name;
  XML_type_def *current_def;
  int i;

  /* This is called once for the schema/dtd definition */


  for (i = 1; i < 45; i++) {
	  
    /* XML Schema Primitive and Derived datatypes */
    switch (i) {
    case XML_string_Type: name = strdup("string"); 
      break;
    case XML_boolean_Type: name = strdup("boolean"); 
      break;
    case XML_float_Type: name = strdup("float"); 
      break;
    case XML_double_Type: name = strdup("double"); 
      break;
    case XML_decimal_Type: name = strdup("decimal"); 
      break;
    case XML_duration_Type: name = strdup("duration"); 
      break;
    case XML_dateTime_Type: name = strdup("dateTime"); 
      break;
    case XML_time_Type: name = strdup("time"); 
      break;
    case XML_date_Type: name = strdup("date"); 
      break;
    case XML_gYearMonth_Type: name = strdup("gYearMonth"); 
      break;
    case XML_gYear_Type: name = strdup("gYear"); 
      break;
    case XML_gMonthDay_Type: name = strdup("gMonthDay"); 
      break;
    case XML_gMonth_Type: name = strdup("gMonth"); 
      break;
    case XML_gDay_Type: name = strdup("gDay"); 
      break;
    case XML_hexBinary_Type: name = strdup("hexBinary"); 
      break;
    case XML_base64Binary_Type: name = strdup("base64Binary"); 
      break;
    case XML_anyURI_Type: name = strdup("anyURI"); 
      break;
    case XML_QName_Type: name = strdup("QName"); 
      break;
    case XML_NOTATION_Type: name = strdup("NOTATION"); 
      break;
    case XML_normalizedString_Type: name = strdup("normalizedString"); 
      break;
    case XML_token_Type: name = strdup("token"); 
      break;
    case XML_language_Type: name = strdup("language"); 
      break;
    case XML_IDREFS_Type: name = strdup("IDREFS"); 
      break;
    case XML_ENTITIES_Type: name = strdup("ENTITIES"); 
      break;
    case XML_NMTOKEN_Type: name = strdup("NMTOKEN"); 
      break;
    case XML_NMTOKENS_Type: name = strdup("NMTOKENS"); 
      break;
    case XML_Name_Type: name = strdup("Name"); 
      break;
    case XML_NCName_Type: name = strdup("NCName"); 
      break;
    case XML_ID_Type: name = strdup("ID"); 
      break;
    case XML_IDREF_Type: name = strdup("IDREF"); 
      break;
    case XML_ENTITY_Type: name = strdup("ENTITY"); 
      break;
    case XML_integer_Type: name = strdup("integer"); 
      break;
    case XML_nonPositiveInteger_Type: name = strdup("nonPositiveInteger"); 
      break;
    case XML_negativeInteger_Type: name = strdup("negativeInteger"); 
      break;
    case XML_long_Type: name = strdup("long"); 
      break;
    case XML_int_Type: name = strdup("int"); 
      break;
    case XML_short_Type: name = strdup("short"); 
      break;
    case XML_byte_Type: name = strdup("byte"); 
      break;
    case XML_nonNegativeInteger_Type: name = strdup("nonNegativeInteger"); 
      break;
    case XML_unsignedLong_Type: name = strdup("unsignedLong"); 
      break;
    case XML_unsignedInt_Type: name = strdup("unsignedInt"); 
      break;
    case XML_unsignedShort_Type: name = strdup("unsignedShort"); 
      break;
    case XML_unsignedByte_Type: name = strdup("unsignedByte"); 
      break;
    case XML_positiveInteger_Type: name = strdup("positiveInteger"); 
      break;
    }

    entry = Tcl_CreateHashEntry(tab, name, &exists);

    if ((current_def = CALLOC(XML_type_def,1)) == NULL) {
      SGML_ERROR(ALLOC_ERROR,
		 "Unable to allocate XML_type_def struct","","","");
    }
    current_def->typename = name;
    current_def->primitivebasetype = i;

    if (entry != NULL)
      Tcl_SetHashValue(entry, 
		       (ClientData)current_def);
  }
}


char *
decode_xml_builtin_types(int code) 
{
  char *name;

  /* this is called to turn datatype codes into strings... */

  switch (code) {
  case XML_string_Type: name = strdup("string"); 
    break;
  case XML_boolean_Type: name = strdup("boolean"); 
    break;
  case XML_float_Type: name = strdup("float"); 
    break;
  case XML_double_Type: name = strdup("double"); 
    break;
  case XML_decimal_Type: name = strdup("decimal"); 
    break;
  case XML_duration_Type: name = strdup("duration"); 
    break;
  case XML_dateTime_Type: name = strdup("dateTime"); 
    break;
  case XML_time_Type: name = strdup("time"); 
    break;
  case XML_date_Type: name = strdup("date"); 
    break;
  case XML_gYearMonth_Type: name = strdup("gYearMonth"); 
    break;
  case XML_gYear_Type: name = strdup("gYear"); 
    break;
  case XML_gMonthDay_Type: name = strdup("gMonthDay"); 
    break;
  case XML_gMonth_Type: name = strdup("gMonth"); 
    break;
  case XML_gDay_Type: name = strdup("gDay"); 
    break;
  case XML_hexBinary_Type: name = strdup("hexBinary"); 
    break;
  case XML_base64Binary_Type: name = strdup("base64Binary"); 
    break;
  case XML_anyURI_Type: name = strdup("anyURI"); 
    break;
  case XML_QName_Type: name = strdup("QName"); 
    break;
  case XML_NOTATION_Type: name = strdup("NOTATION"); 
    break;
  case XML_normalizedString_Type: name = strdup("normalizedString"); 
    break;
  case XML_token_Type: name = strdup("token"); 
    break;
  case XML_language_Type: name = strdup("language"); 
    break;
  case XML_IDREFS_Type: name = strdup("IDREFS"); 
    break;
  case XML_ENTITIES_Type: name = strdup("ENTITIES"); 
    break;
  case XML_NMTOKEN_Type: name = strdup("NMTOKEN"); 
    break;
  case XML_NMTOKENS_Type: name = strdup("NMTOKENS"); 
    break;
  case XML_Name_Type: name = strdup("Name"); 
    break;
  case XML_NCName_Type: name = strdup("NCName"); 
    break;
  case XML_ID_Type: name = strdup("ID"); 
    break;
  case XML_IDREF_Type: name = strdup("IDREF"); 
    break;
  case XML_ENTITY_Type: name = strdup("ENTITY"); 
    break;
  case XML_integer_Type: name = strdup("integer"); 
    break;
  case XML_nonPositiveInteger_Type: name = strdup("nonPositiveInteger"); 
    break;
  case XML_negativeInteger_Type: name = strdup("negativeInteger"); 
    break;
  case XML_long_Type: name = strdup("long"); 
    break;
  case XML_int_Type: name = strdup("int"); 
    break;
  case XML_short_Type: name = strdup("short"); 
    break;
  case XML_byte_Type: name = strdup("byte"); 
    break;
  case XML_nonNegativeInteger_Type: name = strdup("nonNegativeInteger"); 
    break;
  case XML_unsignedLong_Type: name = strdup("unsignedLong"); 
    break;
  case XML_unsignedInt_Type: name = strdup("unsignedInt"); 
    break;
  case XML_unsignedShort_Type: name = strdup("unsignedShort"); 
    break;
  case XML_unsignedByte_Type: name = strdup("unsignedByte"); 
    break;
  case XML_positiveInteger_Type: name = strdup("positiveInteger"); 
    break;
  }
  return (name);

}


SGML_Element *
xml_schema_add_any_el(SGML_DTD *in_dtd, char *tagname, int force)
{
  SGML_Element *current_sgml_element = NULL, *el;
  Tcl_HashEntry *entry; 
  int exists;
  char *namepart;

  /* first check to see if this element is already defined... */
  if (force == 0)
    current_sgml_element = (SGML_Element *) 
      find_ns_hash(tagname, 
		   in_dtd->SGML_Element_hash_tab);
    
  if (current_sgml_element == NULL) {
	  

    if ((current_sgml_element = CALLOC(SGML_Element,1))
	== NULL) {
      SGML_ERROR(ALLOC_ERROR,
		 "Unable to allocate SGML_Element","","","");
    }
	  
    current_sgml_element->tag = strdup(tagname);
    current_sgml_element->data_type = XML_ANY;

    for(el = in_dtd->Elements; 
	el->next_element != NULL; 
	el = el->next_element) ;
    
    el->next_element = current_sgml_element;

    /* add the element name(s) to the hash table */
    /* single name for element */
    if (force == 0) {
      namepart = strchr(current_sgml_element->tag, ':');
      if (namepart == NULL)
	namepart = current_sgml_element->tag;
      else
	namepart++;
    }
    else
      namepart = current_sgml_element->tag;
	  
    entry = Tcl_CreateHashEntry(
				in_dtd->SGML_Element_hash_tab,
				namepart, &exists );
    if (entry != NULL)
      Tcl_SetHashValue(entry, 
		       (ClientData)current_sgml_element);
    else if (exists)
      SGML_ERROR(INFORM_ERROR,
		 "ELEMENT NAME", current_sgml_element->tag,
		 "ALREADY EXISTS IN SCHEMA!","");
  }

  return(current_sgml_element);

}

char *
xml_schema_get_namespace(SGML_DTD *dtd, char *inname)
{
  char *tname, *sep;
  Tcl_HashEntry *entry;

  if (dtd == NULL)
    return (NULL);
   
  if (dtd->XML_Namespace_hash_tab == NULL)
    return(NULL);

  tname = strdup(inname);
  
  sep = strchr(tname, ':');

  if (sep == NULL) {
    FREE (tname);
    return(dtd->targetnamespace);
  }

  *sep = '\0';
  
  entry = Tcl_FindHashEntry(dtd->XML_Namespace_hash_tab, tname);
  
  if (entry == NULL) {
    FREE(tname);
    return (NULL);
  }
  else {
    FREE(tname);
    return((char *) Tcl_GetHashValue(entry));
  }
}


SGML_Attribute_Value *
xml_schema_dup_attrvals(SGML_Attribute_Value *inval) {
  SGML_Attribute_Value *next, *new;

  if (inval == NULL)
    return (NULL);

  next = xml_schema_dup_attrvals(inval->next_value);
  new = CALLOC(SGML_Attribute_Value, 1);
  new->value = strdup(inval->value);
  if (next != NULL)
    new->connector = strdup(inval->connector);
  new->next_value = next;

  return(new);
}

SGML_Attribute *
xml_schema_dup_attributes(SGML_Element *element, SGML_Attribute *in_attr)
{
  SGML_Attribute *next, *new;

  if (in_attr == NULL)
    return NULL;

  next = xml_schema_dup_attributes(element, in_attr->next_attribute);

  new = CALLOC(SGML_Attribute, 1);
  if (element != NULL)
    new->assoc_tag = strdup(element->tag);
  new->name = strdup(in_attr->name);
  new->declared_value_type = in_attr->declared_value_type;
  new->value_list = xml_schema_dup_attrvals(in_attr->value_list);
  new->default_value_type = in_attr->default_value_type;
  if (in_attr->default_value)
    new->default_value = strdup(in_attr->default_value);
  new->next_attribute = next;

  return(new);
}


XML_type_val_list *
xml_schema_get_val_list(char *matchkey, SGML_Data *parent, 
			    Tcl_HashTable *tag_hash)
{
  SGML_Tag_Data *td, *td2, temptd;
  SGML_Attribute_Data *atd;
  idx_key key;
  XML_type_val_list *firstval, *lastval, *val;

  firstval = lastval = val = NULL;

  memset(&key, 0, sizeof(idx_key));
  key.key = matchkey;
  key.occurrence = 0;
  temptd.next_tag_data = NULL;
  temptd.item = parent;
  /* get the restriction for the current simpleType (if any) */
  for (td = comp_tag_list(&key, tag_hash, &temptd, 1); 
       td != NULL;  
       td = td->next_tag_data) {
    /* got some values */
    if ((atd = td->item->attributes) != NULL 
	&& compare_nstags(atd->name, "value") == 0){
      /* since value is required... */
      val = CALLOC(XML_type_val_list, 1);
      val->item = atd->string_val;
      /* since comp_tag_list build them in reverse order... */
      val->next_value = lastval;
      lastval = val;
    }
  }
  
  return (val);

}

int
xml_schema_get_minOccurs(SGML_Attribute_Data *atd) 
{
  SGML_Attribute_Data *a; 
  
  for (a = atd ; a; a = a->next_attribute_data) {
    if (strcmp(a->name,"minOccurs") == 0) {
      return(atoi(a->string_val));
    }
  }  
  /* if it is not found return the default value == 1 */
  return (1);
}



int
xml_schema_get_maxOccurs(SGML_Attribute_Data *atd) 
{
  SGML_Attribute_Data *a; 
  int returnval;

  for (a = atd ; a; a = a->next_attribute_data) {
    if (strcmp(a->name,"maxOccurs") == 0) {
      if (strcmp(a->string_val,"unbounded") == 0)
	return(MAXINT);
      else
	return(atoi(a->string_val));
    }
  }
  /* if it is not found return the default value == 1 */
  return (1); 
}

XML_type_def *
xml_schema_get_basetype (char *name, Tcl_HashTable *type_hash)
{
  Tcl_HashEntry *entry; 
  int exists;
  char *namepart;
  XML_type_def *basetype;

  
  basetype = (XML_type_def *) 
    find_ns_hash(name, 
		 type_hash);

  if (basetype == NULL) {
    /* no existing definition for this type name -- so we create it */
    namepart = strchr(name, ':');
    if (namepart == NULL)
      namepart = name;
    else
      namepart++;
    
    entry = Tcl_CreateHashEntry( type_hash,
				 namepart, &exists );
    if (entry != NULL) {
      
      if ((basetype = CALLOC(XML_type_def,1)) == NULL) {
	SGML_ERROR(ALLOC_ERROR,
		   "Unable to allocate XML_type_def struct","","","");
      }
      basetype->typename = namepart;
      
      Tcl_SetHashValue(entry, 
		       (ClientData)basetype);
    }
  }
  
  return(basetype);
}

int xml_schema_get_simpleType(XML_type_def *current_def, SGML_Data *parent,
			      Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Tag_Data *td, *td2, temptd;
  SGML_Attribute_Data *atd;
  idx_key key;
  XML_type_def *basetype;
  XML_type_val_list *templist;
  

  memset(&key, 0, sizeof(idx_key));
  key.key = "restriction";
  key.occurrence = 0;
  temptd.next_tag_data = NULL;
  temptd.item = parent;
  /* get the restriction for the current simpleType (if any) */
  if (td = comp_tag_list(&key, tag_hash, &temptd, 1)) {
    /* Its a restriction for simpletype %s\n", current_def->typename */

    for (atd = td->item->attributes; atd; atd = atd->next_attribute_data) {
      if (strcmp(atd->name,"base") == 0) {
	
	basetype = xml_schema_get_basetype(atd->string_val, type_hash);

	/* basetype has been created or found, so link the types */
	current_def->derivedbase = basetype;
	current_def->primitivebasetype = basetype->primitivebasetype;
	current_def->enumeration = 
	  xml_schema_get_val_list("enumeration",td->item, tag_hash);
	current_def->patterns = 
	  xml_schema_get_val_list("pattern",td->item, tag_hash);
	templist = 
	  xml_schema_get_val_list("minInclusive",td->item, tag_hash);
	if (templist) {
	  current_def->minInclusive = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("minExclusive",td->item, tag_hash);
	if (templist) {
	  current_def->minExclusive = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("maxInclusive",td->item, tag_hash);
	if (templist) {
	  current_def->maxInclusive = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("maxExclusive",td->item, tag_hash);
	if (templist) {
	  current_def->maxExclusive = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("minLength",td->item, tag_hash);
	if (templist) {
	  current_def->minlength = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("maxLength",td->item, tag_hash);
	if (templist) {
	  current_def->maxlength = templist->item;
	  FREE(templist);
	}
	templist = 
	  xml_schema_get_val_list("length",td->item, tag_hash);
	if (templist) {
	  current_def->length = templist->item;
	  FREE(templist);
	}

      }
    }

  }  /* end of restriction */
  else { /* not a restriction */
    key.key = "list";
    key.occurrence = 0;

    /* get the list for the current simpleType (if any) */
    if (td =  comp_tag_list(&key, tag_hash, &temptd, 1)) {
      for (atd = td->item->attributes; atd; atd = atd->next_attribute_data) {
	if (strcmp(atd->name,"itemType") == 0) {
	  basetype = xml_schema_get_basetype(atd->string_val, type_hash);
	}
      }
    }  /* end of list def */
    else {  /* not a list */
      key.key = "union";
      key.occurrence = 0;

      /* get the union for the current simpleType (if any) */
      if (td =  comp_tag_list(&key, tag_hash, &temptd, 1)) {
	/* not doing anything yet */
      }  /* end of union def */
      else {  /* not a restriction, list or union -- it is invalid */
	SGML_ERROR(PARSE_ERROR,
		   "SimpleType definition ",current_def->typename," is not a restriction, list, or union","");
	return(-1);
	  

      }
    }
  }
}


int complex_child_type(SGML_Data *child) 
{
  char *childname;
  
  if (child == NULL 
      || child->element == NULL 
      || child->element->tag == NULL)
    return 0;
 
  if (compare_nstags(child->element->tag, "annotation") == 0)
    return 0;

  /* otherwise we have something here */
  if (compare_nstags(child->element->tag, "element") == 0)
    return 10;
  else if (compare_nstags(child->element->tag, "simpleContent") == 0)
    return 1;
  else if (compare_nstags(child->element->tag, "complexContent") == 0)
    return 2;
  else if (compare_nstags(child->element->tag, "all") == 0)
    return 3;
  else if (compare_nstags(child->element->tag, "choice") == 0)
    return 4;
  else if (compare_nstags(child->element->tag, "sequence") == 0)
    return 5;
  else if (compare_nstags(child->element->tag, "group") == 0)
    return 6;
  else if (compare_nstags(child->element->tag, "attribute") == 0)
    return 7;
  else if (compare_nstags(child->element->tag, "attributeGroup") == 0)
    return 8;
  else if (compare_nstags(child->element->tag, "anyAttribute") == 0)
    return 9;
  else if (compare_nstags(child->element->tag, "extension") == 0)
    return 11;
  else if (compare_nstags(child->element->tag, "restriction") == 0)
    return 12;
  else if (compare_nstags(child->element->tag, "any") == 0)
    return 13;


}

char *
xml_schema_get_repchars(int min_occurs, int max_occurs)
{
  if (min_occurs == 0) {
    if (max_occurs == 1) 
      return(strdup("?"));
    else if (max_occurs == 0)
      return NULL;
    else
      return(strdup("*"));
  } 
  else {
    if (min_occurs == 1 && max_occurs == 1)
      return NULL;
    else
      return (strdup("+"));
  }
}

int xml_schema_get_attribute(XML_type_def *current_def, SGML_Data *parent,
			     Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Attribute *curr, *last, *test;
  int resultcode;
  int hasname, hastype, hasref;
  XML_type_def *basetype;
  
  /* we expect a group to be a list of siblings that contain */
  /* choice, sequence or 'all' definitions (or annotation) */
  /* Group Definitions must be global, group references are used in */
  /* declaring the group in complextypes, etc. */
  
  curr = last = NULL;
  hasname = hastype = hasref = 0;
  
  /* set the last attribute for this def... */
  for (last = current_def->associated_attributes; 
       last != NULL && last->next_attribute != NULL; 
       last = last->next_attribute) ;

  for (atd = parent->attributes; atd; atd = atd->next_attribute_data) {
    
    if (strcmp(atd->name, "name") == 0) {
      hasname = 1;
      for (test = current_def->associated_attributes; 
       test != NULL; test = test->next_attribute) 
	if (strcmp(test->name, atd->string_val) == 0) {
	  /* duplicate attribute defs, so continue */
	  return (0);
	}
      
      curr = CALLOC(SGML_Attribute, 1);
      curr->name = strdup(atd->string_val);
      /* printf("found attribute name %s defined\n", atd->string_val); */
      if (last)
	last->next_attribute = curr;
      else {
	current_def->associated_attributes = curr;
	last = curr;
      }
    }
    else if (strcmp(atd->name, "ref") == 0) {
      hasref = 1;
      basetype = (XML_type_def *) 
	find_ns_hash(atd->string_val, 
		     type_hash);
      
      if (basetype != NULL) {
	/* printf("found attr ref %s defined\n", atd->string_val); */
	if (last)
	  last->next_attribute = 
	    xml_schema_dup_attributes(NULL, basetype->associated_attributes);
	else {
	  current_def->associated_attributes = 
	    xml_schema_dup_attributes(NULL, basetype->associated_attributes);
	}
	
      }
      else {
	printf("attr ref %s NOT defined\n", atd->string_val);   
      }
    }
    else if (strcmp(atd->name, "type") == 0) {
      hastype = 1;
      
      basetype = (XML_type_def *) 
	find_ns_hash(atd->string_val, 
		     type_hash);
      
      if (basetype != NULL) {
	if (curr) {
	  curr->declared_value_type = basetype->primitivebasetype;
	}
	/* printf("found attr type %s defined - no CURRENT attribute def\n", 
	   atd->string_val); */
      }
      
    }
    else if (strcmp(atd->name, "fixed") == 0) {
      /* fixed attribute value */
      if (curr) {
	curr->default_value_type = FIXED_DEFAULT;
	curr->default_value = strdup(atd->string_val);
      }
      /* printf("fixed attr value %s but no CURRENT attribute def\n", 
	 atd->string_val); */
    }
    else if (strcmp(atd->name, "default") == 0) {
      /* fixed attribute value */
      if (curr) {
	curr->default_value_type = SPECIFIED_DEFAULT;
	curr->default_value = strdup(atd->string_val);
      }
     
      /* printf("default attribute value %s\n", atd->string_val); */
    }
    else if (strcmp(atd->name, "use") == 0) {
      /* fixed attribute value */
      if (curr) {
	if (strcmp(atd->string_val, "required") == 0)
	  curr->default_value_type = REQUIRED_DEFAULT;
	else if (strcmp(atd->string_val, "optional") == 0)
	  curr->default_value_type = IMPLIED_DEFAULT;
	else if (strcmp(atd->string_val, "prohibited") == 0)
	  curr->default_value_type = PROHIBITED_DEFAULT;
      }
     
      /* printf("attribute use value %s\n", atd->string_val); */
    }
    
  }


  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {
    
    if (compare_nstags(child->element->tag, "simpleType") == 0) {
      /* OK we have named simpleType definition -- now we extract its data */
      XML_type_def tempdef;
      XML_type_val_list *list;
      SGML_Attribute_Value *firstval, *lastval, *newval;

      memset((char *)&tempdef, '\0', sizeof(XML_type_def));
      
      xml_schema_get_simpleType(&tempdef, child, tag_hash,type_hash);

      firstval = lastval = newval = NULL;
      
      for (list = tempdef.enumeration; list; list = list->next_value) {
	/* printf("*** enumeration : %s\n", list->item); */
	
	newval = CALLOC(SGML_Attribute_Value, 1);
	newval->value = strdup(list->item);
	newval->connector = strdup("|");
	if (firstval == NULL) {
	  firstval = lastval = newval;
	}
	else {
	  lastval->next_value = newval;
	  lastval = newval;
	}
      }

      if (curr) {
	/* add to the current definition */
	curr->value_list = firstval;
      } 
	
    }
  }

  return(0);

}


int xml_schema_get_attributegroup(XML_type_def *current_def, 
				  SGML_Data *parent,
				  Tcl_HashTable *tag_hash, 
				  Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Attribute *curr, *last;
  int resultcode;
  int hasname, hastype, hasref;
  XML_type_def *basetype;
  
  /* we expect a group to be a list of siblings that contain */
  /* choice, sequence or 'all' definitions (or annotation) */
  /* Group Definitions must be global, group references are used in */
  /* declaring the group in complextypes, etc. */
  
  curr = last = NULL;
  hasname = hastype = hasref = 0;
  
  /* set the last attribute for this def... */

  if (parent->sub_data_element == NULL
      && parent->attributes != NULL
      && strcmp(parent->attributes->name, "ref") == 0) {

    basetype = (XML_type_def *) 
      find_ns_hash(parent->attributes->string_val, 
		   type_hash);
    
    if (basetype != NULL) {
      /* set the last attribute for this def... */
      for (last = current_def->associated_attributes; 
	   last != NULL && last->next_attribute != NULL; 
	   last = last->next_attribute) ;
      
      if (last == NULL)
	current_def->associated_attributes = 
	  xml_schema_dup_attributes(NULL,
				    basetype->associated_attributes);
      else
	last->next_attribute = xml_schema_dup_attributes(NULL,
					    basetype->associated_attributes);

    }
    
  }
  else {

    for (child = parent->sub_data_element; child != NULL; 
	 child = child->next_data_element) {    
      if (compare_nstags(child->element->tag, "attribute") == 0) {
	xml_schema_get_attribute(current_def, child, tag_hash, type_hash);
      }
      else if (compare_nstags(child->element->tag, "attributeGroup") == 0) {
	if (child->attributes != NULL 
	    && strcmp(child->attributes->name, "ref") == 0) {      

	  basetype = (XML_type_def *) 
	    find_ns_hash(child->attributes->string_val, 
			 type_hash);
	  
	  if (basetype != NULL) {
	    /* printf("found attributegroup ref %s defined\n", atd->string_val); */
	    /* set the last attribute for this def... */
	    for (last = current_def->associated_attributes; 
		 last != NULL && last->next_attribute != NULL; 
		 last = last->next_attribute) ;
	    
      
	    if (last == NULL)
	      current_def->associated_attributes = 
		xml_schema_dup_attributes(NULL,
					  basetype->associated_attributes);
	    else
	      last->next_attribute = xml_schema_dup_attributes(NULL,
					    basetype->associated_attributes);
	  }
	}
      }
    }
  }

  return(0);

}


int xml_schema_get_restriction(XML_type_def *current_def, SGML_Data *parent,
			       SGML_Sub_Element_List *parent_sub,
			       Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *choice;
  SGML_Attribute *firstattr, *currattr, *lastattr;
  int resultcode;
  XML_type_def *basetype;

  /* we expect a choice to be a list of siblings that contain element */
  /* group, choice sequence or any definitions or annotation */


  /* printf("processing restriction...\n");*/

  first = curr = last = NULL;
  firstattr = currattr = lastattr = NULL;
  for (atd = parent->attributes; atd; atd = atd->next_attribute_data) {
    
    if (strcmp(atd->name, "base") == 0) {
      basetype = (XML_type_def *) 
	find_ns_hash(atd->string_val, 
		     type_hash);
      if (basetype != NULL) {
	current_def->primitivebasetype = basetype->primitivebasetype;
	current_def->derivedbase = basetype;
      }
    }
  }

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    if (child->element != NULL && child->element->tag != NULL
	&& compare_nstags(child->element->tag, "attribute") != 0
	&& compare_nstags(child->element->tag, "attributeGroup") != 0) {
      
      last = curr;
      curr = CALLOC(SGML_Sub_Element_List, 1);
      if (first == NULL) {
	first = last = curr;
      }
      else {
	/* Since this is CHOICE the connector is an OR */
	last->connector = strdup("|");
	last->next_sub_element = curr;
      }
      
      switch (complex_child_type(child)) {
      case 4: /* choice */
	resultcode = xml_schema_get_choice(current_def, child, curr,
					   tag_hash, type_hash);
	break;
      case 5: /* sequence */
	resultcode = xml_schema_get_sequence(current_def, child, curr,
					     tag_hash, type_hash);
	break;
      case 6: /* group */
	resultcode = xml_schema_get_group(current_def, child, curr,
					  tag_hash, type_hash);
	
	break;

      default:
	/* printf ("complex_child default\n"); */
	break;
	
      }
    }
    else if (compare_nstags(child->element->tag, "attribute") == 0) {
      
      /* this is an attribute definition for this item */
      /* printf("attribute in restriction:\n"); */
      
      xml_schema_get_attribute(current_def, child, tag_hash, type_hash);

    }
    else if (compare_nstags(child->element->tag, "attributeGroup") == 0) {
      
      /* this is a named set of attribute definitions for this item */
      /* printf("attributeGroup in restriction:\n"); */
      
      xml_schema_get_attributegroup(current_def, child, tag_hash, type_hash);

    }
  }
  
  if (parent_sub == NULL) {

    choice = CALLOC(SGML_Sub_Element_List, 1);
    choice->group = first;
    choice->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    choice->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = choice;
    choice->sub_element_type = XML_CHOICE;
    choice->group_repetition = xml_schema_get_repchars(choice->min_occurs, 
					       choice->max_occurs);


    current_def->model = choice;
  }
  else {
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  
  
  return(0);
  
}


int xml_schema_get_extension(XML_type_def *current_def, SGML_Data *parent,
			       SGML_Sub_Element_List *parent_sub,
			       Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *choice;
  int resultcode;
  XML_type_def *basetype;

  /* we expect a choice to be a list of siblings that contain element */
  /* group, choice sequence or any definitions or annotation */
  first = curr = last = NULL;

  /* printf("processing extension...\n"); */

  for (atd = parent->attributes; atd; atd = atd->next_attribute_data) {
    
    if (strcmp(atd->name, "base") == 0) {
      basetype = (XML_type_def *) 
	find_ns_hash(atd->string_val, 
		     type_hash);
      if (basetype != NULL) {
	current_def->primitivebasetype = basetype->primitivebasetype;
	current_def->derivedbase = basetype;
      }
      else {
	/* create an empty type for this... */
	Tcl_HashEntry *entry; 
	char *namepart;
	int exists;

	/* printf("basetype %s not defined (yet?)\n", atd->string_val); */
	/* no existing definition for this type name -- so we create it */
	namepart = strchr(atd->string_val, ':');
	if (namepart == NULL)
	  namepart = atd->string_val;
	else
	  namepart++;
	  
	entry = Tcl_CreateHashEntry(type_hash, namepart, &exists );
	if (entry != NULL) {
	  
	  if ((basetype = CALLOC(XML_type_def,1)) == NULL) {
	    SGML_ERROR(ALLOC_ERROR,
		       "Unable to allocate XML_type_def struct","","","");
	  }
	  basetype->typename = strdup(namepart);
	  
	  Tcl_SetHashValue(entry, 
			   (ClientData)basetype);
	  
	  current_def->primitivebasetype = 99999;
	  current_def->derivedbase = basetype;
	  
	}
	
      }
    }
  }


  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    if (child->element != NULL && child->element->tag != NULL
	&& compare_nstags(child->element->tag, "attribute") != 0
	&& compare_nstags(child->element->tag, "attributeGroup") != 0) {

      last = curr;
      curr = CALLOC(SGML_Sub_Element_List, 1);
      if (first == NULL) {
	first = last = curr;
      }
      else {
	
	/* Since this is CHOICE the connector is an OR */
	last->connector = strdup("|");
	last->next_sub_element = curr;
      }
      
      switch (complex_child_type(child)) {
      case 3:
	/* printf ("all 3\n"); */
	resultcode = xml_schema_get_all(current_def, child, curr,
					tag_hash, type_hash);

	break;
      case 4:
	/* printf ("choice 4\n"); */
	resultcode = xml_schema_get_choice(current_def, child, curr,
					   tag_hash, type_hash);
	
	break;
      case 5:
	/* printf ("sequence 5\n"); */
	resultcode = xml_schema_get_sequence(current_def, child, curr,
					     tag_hash, type_hash);
	break;
      case 6:
	/* printf ("group 6\n"); */
	resultcode = xml_schema_get_group(current_def, child, curr,
					  tag_hash, type_hash);

	break;
	
      default:
	/* printf ("complex_child default\n"); */
	break;
	
      }
    }
    else if (compare_nstags(child->element->tag, "attribute") == 0) {
      
      /* this is an attribute definition for this item */
      /* printf("attribute in extension:\n"); */
      
      xml_schema_get_attribute(current_def, child, tag_hash, type_hash);

    }
    else if (compare_nstags(child->element->tag, "attributeGroup") == 0) {
      
      /* this is a named set of attribute definitions for this item */
      /* printf("attributeGroup in restriction:\n"); */
      
      xml_schema_get_attributegroup(current_def, child, tag_hash, type_hash);

    }

  }


  if (parent_sub == NULL) {

    choice = CALLOC(SGML_Sub_Element_List, 1);
    choice->group = first;
    choice->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    choice->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = choice;
    choice->sub_element_type = XML_CHOICE;
    choice->group_repetition = xml_schema_get_repchars(choice->min_occurs, 
					       choice->max_occurs);


    current_def->model = choice;
  }
  else {
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  
  
  return(0);
  
}


int xml_schema_get_complex_or_simpleContent(XML_type_def *current_def, SGML_Data *parent,
				 SGML_Sub_Element_List *parent_sub,
				 Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *choice;
  int resultcode;
  XML_type_def *basetype;

  /* we expect a choice to be a list of siblings that contain element */
  /* group, choice sequence or any definitions or annotation */
  first = curr = last = NULL;

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

      
    switch (complex_child_type(child)) {
    case 11:
      /* printf ("complex or SimpleContent -- extension\n"); */
      resultcode = xml_schema_get_extension(current_def, child, curr,
					 tag_hash, type_hash);

      break;
    case 12:
      /* printf ("complex or SimpleContent -- restriction\n"); */
      resultcode = xml_schema_get_restriction(current_def, child, curr,
					 tag_hash, type_hash);
      break;
    }
  }

  return (0);
  
}


int xml_schema_get_all(XML_type_def *current_def, SGML_Data *parent,
			    SGML_Sub_Element_List *parent_sub,
			    Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *seq;
  int resultcode;
  XML_type_def *basetype;
  
  /* we expect all to be a list of siblings that contain elements */
  /* which may appear in any order in the document (with 0 or 1 occurrence) */
  first = curr = last = NULL;

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    last = curr;
    curr = CALLOC(SGML_Sub_Element_List, 1);
    if (first == NULL) {
      first = last = curr;
    }
    else {

    /* Since this is ALL the connector is ampersand */
      last->connector = strdup("&");
      last->next_sub_element = curr;
    }
      
    switch (complex_child_type(child)) {
      
    case 10:
      
      /* printf ("ALL element\n"); */

      for (atd = child->attributes; atd; atd = atd->next_attribute_data) {
	
	if (strcmp(atd->name,"name") == 0 || strcmp(atd->name,"ref") == 0) {
	  char *namepart;
	  
	  namepart = strchr(atd->string_val, ':');
	  if (namepart == NULL)
	    namepart = atd->string_val;
	  else
	    namepart++;
	  
	  curr->element_name = strdup(namepart);
	  curr->min_occurs = xml_schema_get_minOccurs(child->attributes);
	  curr->max_occurs = xml_schema_get_maxOccurs(child->attributes);
	  curr->group_parent = parent_sub;
          curr->sub_element_type = EL_NAMED;
	  curr->repetition = xml_schema_get_repchars(curr->min_occurs, 
						     curr->max_occurs);
	 

	} /* end name attribute */
	else if (strcmp(atd->name,"type") == 0) {
	  basetype = xml_schema_get_basetype(atd->string_val, type_hash);
	  curr->sub_element_type = basetype->primitivebasetype;
	  curr->xml_type_name = strdup(atd->string_val);
	}
	  
	
      } /* end attributes loop */
      
      break;
      
    default:
      /* printf ("all_child default\n"); */
      break;
      
    }

  }


  if (parent_sub == NULL) {

    seq = CALLOC(SGML_Sub_Element_List, 1);
    seq->group = first;
    seq->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    seq->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = seq;
    seq->sub_element_type = XML_ALL;
    seq->group_repetition = xml_schema_get_repchars(seq->min_occurs, 
					       seq->max_occurs);


    current_def->model = seq;
  }
  else {
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  
  
  return(0);
  
}


int xml_schema_get_any(XML_type_def *current_def, SGML_Data *parent,
		       SGML_Sub_Element_List *parent_sub,
		       Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *curr, *seq;
  int resultcode;
  XML_type_def *basetype;

  /* we expect any to have no other sub elements except annotation...*/
  curr = CALLOC(SGML_Sub_Element_List, 1);

  for (atd = parent->attributes; atd; atd = atd->next_attribute_data) {
    
    if (strcmp(atd->name,"namespace") == 0) {

      curr->element_name = strdup("##ANY##");
      curr->min_occurs = xml_schema_get_minOccurs(parent->attributes);
      curr->max_occurs = xml_schema_get_maxOccurs(parent->attributes);
      curr->group_parent = parent_sub;
      curr->sub_element_type = EL_NAMED;
      curr->repetition = xml_schema_get_repchars(curr->min_occurs, 
						 curr->max_occurs);

      basetype = xml_schema_get_basetype(atd->string_val, type_hash);
      curr->sub_element_type = basetype->primitivebasetype;
      curr->xml_type_name = strdup(atd->string_val);

    }
  }


  if (parent_sub == NULL) {

    seq = CALLOC(SGML_Sub_Element_List, 1);
    seq->group = curr;
    curr->group_parent = seq;
    seq->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    seq->max_occurs = xml_schema_get_maxOccurs(parent->attributes);
    seq->sub_element_type = XML_ANY;
    seq->group_repetition = xml_schema_get_repchars(seq->min_occurs, 
						    seq->max_occurs);
    current_def->model = seq;
  }
  else { 
    parent_sub->group = curr;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  

  xml_schema_defines_ANY = 1;

  return(0);

}

int xml_schema_get_group(XML_type_def *current_def, SGML_Data *parent,
			 SGML_Sub_Element_List *parent_sub,
			 Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *group;
  int resultcode;
  
  /* we expect a group to be a list of siblings that contain */
  /* choice, sequence or 'all' definitions (or annotation) */
  /* Group Definitions must be global, group references are used in */
  /* declaring the group in complextypes, etc. */

  first = curr = last = NULL;

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    last = curr;
    curr = CALLOC(SGML_Sub_Element_List, 1);
    if (first == NULL) {
      first = last = curr;
    }
    else {

    /* Since this is GROUP the connector is sequence */
      last->connector = strdup(",");
      last->next_sub_element = curr;
    }
      
    switch (complex_child_type(child)) {
    case 4:
      /* printf ("choice 4\n"); */
      resultcode = xml_schema_get_choice(current_def, child, curr,
					 tag_hash, type_hash);

      break;
    case 5:
      /* printf ("sequence 5\n"); */
      resultcode = xml_schema_get_sequence(current_def, child, curr,
					 tag_hash, type_hash);
      break;
      
    default:
      /* printf ("group_child default - do nothing\n"); */
      break;
      
    }

  }


  if (parent_sub == NULL) {

    group = CALLOC(SGML_Sub_Element_List, 1);
    group->group = first;
    group->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    group->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = group;
    group->sub_element_type = XML_GROUP;
    group->group_repetition = xml_schema_get_repchars(group->min_occurs, 
					       group->max_occurs);


    current_def->model = group;
  }
  else {
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  

  return(0);
  
}


int xml_schema_get_sequence(XML_type_def *current_def, SGML_Data *parent,
			    SGML_Sub_Element_List *parent_sub,
			    Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *seq;
  int resultcode;
  XML_type_def *basetype;
  
  /* we expect a sequence to be a list of siblings that contain element */
  /* group, choice sequence or any definitions or annotation */
  first = curr = last = NULL;

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    last = curr;
    curr = CALLOC(SGML_Sub_Element_List, 1);
    if (first == NULL) {
      first = last = curr;
    }
    else {

    /* Since this is CHOICE the connector is an OR */
      last->connector = strdup(",");
      last->next_sub_element = curr;
    }
      
    switch (complex_child_type(child)) {
    case 4:
      /* printf ("choice 4\n"); */
      resultcode = xml_schema_get_choice(current_def, child, curr,
					 tag_hash, type_hash);

      break;
    case 5:
      /* printf ("sequence 5\n"); */
      resultcode = xml_schema_get_sequence(current_def, child, curr,
					 tag_hash, type_hash);
      break;
    case 6:
      /* printf ("group 6\n"); */
      resultcode = xml_schema_get_group(current_def, child, curr,
					 tag_hash, type_hash);

      break;
      
    case 10:
      
      /* printf ("element 10\n"); */

      for (atd = child->attributes; atd; atd = atd->next_attribute_data) {
	
	if (strcmp(atd->name,"name") == 0 || strcmp(atd->name,"ref") == 0) {
	  char *namepart;
	  
	  namepart = strchr(atd->string_val, ':');
	  if (namepart == NULL)
	    namepart = atd->string_val;
	  else
	    namepart++;
	  
	  curr->element_name = strdup(namepart);
	  curr->min_occurs = xml_schema_get_minOccurs(child->attributes);
	  curr->max_occurs = xml_schema_get_maxOccurs(child->attributes);
	  curr->group_parent = parent_sub;
          curr->sub_element_type = EL_NAMED;
	  curr->repetition = xml_schema_get_repchars(curr->min_occurs, 
						     curr->max_occurs);

	} /* end name attribute */
	else if (strcmp(atd->name,"type") == 0) {
	  basetype = xml_schema_get_basetype(atd->string_val, type_hash);
	  curr->sub_element_type = basetype->primitivebasetype;
	  curr->xml_type_name = strdup(atd->string_val);
	}
	  
      } /* end attributes loop */
      
      break;

    case 13:
      /* printf ("ANY 13\n"); */
      resultcode = xml_schema_get_any(current_def, child, curr,
				      tag_hash, type_hash);
      
    default:
      /* printf ("sequence_child default\n"); */
      break;
      
    }

  }


  if (parent_sub == NULL) {

    seq = CALLOC(SGML_Sub_Element_List, 1);
    seq->group = first;
    seq->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    seq->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = seq;
    seq->sub_element_type = XML_SEQUENCE;
    seq->group_repetition = xml_schema_get_repchars(seq->min_occurs, 
					       seq->max_occurs);


    current_def->model = seq;
  }
  else { 
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }  
  return(0);
  
}



int xml_schema_get_choice(XML_type_def *current_def, SGML_Data *parent,
			  SGML_Sub_Element_List *parent_sub,
			      Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Data *child;
  SGML_Attribute_Data *atd;
  SGML_Sub_Element_List *first, *curr, *last, *choice;
  int resultcode;
  XML_type_def *basetype;

  /* we expect a choice to be a list of siblings that contain element */
  /* group, choice sequence or any definitions or annotation */
  first = curr = last = NULL;

  for (child = parent->sub_data_element; child != NULL; 
       child = child->next_data_element) {

    last = curr;
    curr = CALLOC(SGML_Sub_Element_List, 1);
    if (first == NULL) {
      first = last = curr;
    }
    else {

    /* Since this is CHOICE the connector is an OR */
      last->connector = strdup("|");
      last->next_sub_element = curr;
    }
      
    switch (complex_child_type(child)) {
    case 4:
      /* printf ("choice 4\n"); */
      resultcode = xml_schema_get_choice(current_def, child, curr,
					 tag_hash, type_hash);

      break;
    case 5:
      /* printf ("sequence 5\n"); */
      resultcode = xml_schema_get_sequence(current_def, child, curr,
					 tag_hash, type_hash);
      break;
    case 6:
      /* printf ("group 6\n"); */
      resultcode = xml_schema_get_group(current_def, child, curr,
					 tag_hash, type_hash);

      break;
      
    case 10:
      
      /* printf ("element 10\n"); */

      for (atd = child->attributes; atd; atd = atd->next_attribute_data) {
	
	if (strcmp(atd->name,"name") == 0 || strcmp(atd->name,"ref") == 0) {
	  char *namepart;
	  
	  namepart = strchr(atd->string_val, ':');
	  if (namepart == NULL)
	    namepart = atd->string_val;
	  else
	    namepart++;
	  
	  curr->element_name = strdup(namepart);
	  curr->min_occurs = xml_schema_get_minOccurs(child->attributes);
	  curr->max_occurs = xml_schema_get_maxOccurs(child->attributes);
	  curr->group_parent = parent_sub;
          curr->sub_element_type = EL_NAMED;
	  curr->repetition = xml_schema_get_repchars(curr->min_occurs, 
						     curr->max_occurs);

	} /* end name attribute */
	else if (strcmp(atd->name,"type") == 0) {
	  basetype = xml_schema_get_basetype(atd->string_val, type_hash);
	  curr->sub_element_type = basetype->primitivebasetype;
	  curr->xml_type_name = strdup(atd->string_val);
	}
	  
      } /* end attributes loop */
      
      break;
      
    case 13:
      /* printf ("ANY 13\n"); */
      resultcode = xml_schema_get_any(current_def, child, curr,
					 tag_hash, type_hash);

      break;
    default:
      /* printf ("complex_child default\n"); */
      break;
      
    }

  }


  if (parent_sub == NULL) {

    choice = CALLOC(SGML_Sub_Element_List, 1);
    choice->group = first;
    choice->min_occurs = xml_schema_get_minOccurs(parent->attributes);
    choice->max_occurs = xml_schema_get_maxOccurs(parent->attributes);

    for (curr = first; curr; curr = curr->next_sub_element) 
      curr->group_parent = choice;
    choice->sub_element_type = XML_CHOICE;
    choice->group_repetition = xml_schema_get_repchars(choice->min_occurs, 
					       choice->max_occurs);


    current_def->model = choice;
  }
  else {
    parent_sub->group = first;
    parent_sub->group_repetition = 
      xml_schema_get_repchars(xml_schema_get_minOccurs(parent->attributes),
			      xml_schema_get_maxOccurs(parent->attributes));
  }
  return(0);
  
}

int
xml_schema_link_types(Tcl_HashTable *type_hash)
{
  Tcl_HashEntry *entry; 
  Tcl_HashSearch hash_search;
  int exists;
  char *type_name;
  XML_type_def *current_def, *basetype;

  for (entry = Tcl_FirstHashEntry(type_hash,&hash_search); 
       entry != NULL; 
       entry = Tcl_NextHashEntry(&hash_search)) {
    /* get the name */
    type_name = Tcl_GetHashKey(type_hash,entry); 
    /* and the def info */
    current_def =  (XML_type_def *) Tcl_GetHashValue(entry);


    if (current_def->primitivebasetype == 99999 
	&& current_def->derivedbase != NULL) {
      /* This was a reference to a type encounted before the full definition */
      /* printf("linking -- missed type def %s\n", current_def->typename); */
      current_def->model = current_def->derivedbase->model;
      if (current_def->associated_attributes == NULL)
	current_def->associated_attributes = 
	  current_def->derivedbase->associated_attributes;
      else {
	SGML_Attribute *last;

	for (last = current_def->associated_attributes; 
	     last != NULL && last->next_attribute != NULL; 
	     last = last->next_attribute) ;
	
	last->next_attribute = 
	  current_def->derivedbase->associated_attributes;
      }
      current_def->primitivebasetype = 
	current_def->derivedbase->primitivebasetype;
    }

  }
    

}

int xml_schema_get_complexType(XML_type_def *current_def, SGML_Data *parent,
			      Tcl_HashTable *tag_hash, Tcl_HashTable *type_hash)
{
  SGML_Tag_Data *td, *td2, temptd;
  SGML_Attribute_Data *atd;
  idx_key key;
  XML_type_def *basetype;
  XML_type_val_list *templist;
  char *childname;
  SGML_Data *dat;
  int resultcode;
  int first = 1;

  dat = parent->sub_data_element;

  if (dat == NULL) {
    /* fprintf(LOGFILE,"Schema ComplexType %s is not defined (or empty)\n",
     *	current_def->typename);
     */
    /* we treat this as an implicit EMPTY definition */
    current_def->primitivebasetype = XML_EMPTY;
    return(1);
  }

  do {
    switch (complex_child_type(dat)) {
    case 1:
      /* printf ("simpleContent 1\n"); */
      resultcode = xml_schema_get_complex_or_simpleContent(current_def, dat, 
							   NULL,
							   tag_hash, type_hash);
      break;
    case 2:
      /* printf ("complexContent 2\n"); */
      resultcode = xml_schema_get_complex_or_simpleContent(current_def, dat, 
							   NULL,
							   tag_hash, type_hash);
      break;
    case 3:
      /* printf ("all 3\n"); */
      current_def->primitivebasetype = XML_ALL;
      resultcode = xml_schema_get_all(current_def, dat, NULL,
					 tag_hash, type_hash);

      break;
    case 4:
      /* printf ("choice 4\n"); */
      current_def->primitivebasetype = XML_CHOICE;
      resultcode = xml_schema_get_choice(current_def, dat, NULL, 
				       tag_hash, type_hash);
    
    break;
    case 5:
      /* printf ("sequence 5\n"); */
      current_def->primitivebasetype = XML_SEQUENCE;
      resultcode = xml_schema_get_sequence(current_def, dat, NULL,
					 tag_hash, type_hash);
      break;
    case 6:
      /* printf ("group 6\n"); */
      current_def->primitivebasetype = XML_GROUP;
      resultcode = xml_schema_get_group(current_def, dat, NULL,
					tag_hash, type_hash);
      
      break;
    case 7:
      /* printf ("ComplexType attribute def\n");  */
      resultcode = xml_schema_get_attribute(current_def, dat,
					tag_hash, type_hash);
      
      break;
    case 8:
      /* printf ("attributeGroup 8\n"); */
      resultcode = xml_schema_get_attributegroup(current_def, dat,
						 tag_hash, type_hash);
      break;
    case 9:
      printf ("anyAttribute encountered in complextype def...\n");
      break;
    default:
      /* printf ("complex_child default (do nothing)\n"); */
      break;
    
    }

    first = 0;
  } while (dat = dat->next_data_element);

}


SGML_DTD *
xml_schema_init_dtd(SGML_Document *schemadoc)
{
  /* this uses global structures almost exclusively */
  /* in case there wasn't a declaration */
  current_sgml_declaration = &XML_Declaration; 
  
  last_sgml_dtd = current_sgml_dtd;

  if ((current_sgml_dtd = CALLOC(SGML_DTD,1)) == NULL) {
    SGML_ERROR(ALLOC_ERROR,
	       "Unable to allocate SGML_DTD struct","","","");
		     
  }

  if (last_sgml_dtd == NULL) /* first DTD */
    DTD_list = current_sgml_dtd;
  else last_sgml_dtd->next_dtd = current_sgml_dtd;

  /* for fast lookup of element defs */
  current_sgml_dtd->SGML_Element_hash_tab = 
    CALLOC(Tcl_HashTable,1);
  
  Tcl_InitHashTable(current_sgml_dtd->SGML_Element_hash_tab,
		    TCL_STRING_KEYS);
  
  /* for fast lookup of entity defs */
  current_sgml_dtd->SGML_Entity_hash_tab = 
    CALLOC(Tcl_HashTable,1);
  
  Tcl_InitHashTable(current_sgml_dtd->SGML_Entity_hash_tab,
		    TCL_STRING_KEYS);
  
  /* for fast lookup of namespace defs and info */
  current_sgml_dtd->XML_Namespace_hash_tab = 
    CALLOC(Tcl_HashTable,1);
  
  Tcl_InitHashTable(current_sgml_dtd->XML_Namespace_hash_tab,
		    TCL_STRING_KEYS);
  
  /* for fast lookup of XML Schema type defs  and groups */
  current_sgml_dtd->XML_Typedef_hash_tab = 
    CALLOC(Tcl_HashTable,1);
  
  Tcl_InitHashTable(current_sgml_dtd->XML_Typedef_hash_tab,
		    TCL_STRING_KEYS);

  init_xml_builtin_types(current_sgml_dtd->XML_Typedef_hash_tab);

  /* If there is one, the catalog will have been created */
  /* as Current_SGML_Catalog;                            */
  if (schemadoc->DTD)
    current_sgml_dtd->SGML_Catalog_hash_tab =
      schemadoc->DTD->SGML_Catalog_hash_tab;
    
  current_sgml_dtd->DTD_file_name = schemadoc->file_name;
  current_sgml_dtd->type = 2;
  
  if (current_sgml_declaration)
    current_sgml_dtd->XML_encoding =
      current_sgml_declaration->encoding;

  return (current_sgml_dtd);

}

XML_type_def *
xml_schema_create_typedef(SGML_Attribute_Data *in_atd, 
			  Tcl_HashTable *type_hash)
{
  SGML_Attribute_Data *atd;
  XML_type_def *current_def;

  current_def = NULL;

  for (atd = in_atd; atd; atd = atd->next_attribute_data) {
      
    if (strcmp(atd->name,"name") == 0) {
      Tcl_HashEntry *entry; 
      int exists;
      char *namepart;
      
      current_def = (XML_type_def *)
	find_ns_hash(atd->string_val, type_hash);
      
      if (current_def == NULL) {
	/* no existing definition for this type name -- so we create it */
	namepart = strchr(atd->string_val, ':');
	if (namepart == NULL)
	  namepart = atd->string_val;
	else
	  namepart++;
	  
	entry = Tcl_CreateHashEntry(type_hash, namepart, &exists );
	if (entry != NULL) {

	  if ((current_def = CALLOC(XML_type_def,1)) == NULL) {
	    SGML_ERROR(ALLOC_ERROR,
		       "Unable to allocate XML_type_def struct","","","");
	  }
	  current_def->typename = strdup(namepart);

	  Tcl_SetHashValue(entry, 
			   (ClientData)current_def);
	}
      }
    } /* end name attribute */
  } /* end attributes loop */

  return(current_def); /* NULL is returned if no name is found */
}

int
xml_schema_merge_schemas(SGML_DTD *primary_dtd, SGML_DTD *secondary_dtd) 
{
  SGML_Element *p, *s;
  Tcl_HashEntry *entry, *newentry; 
  Tcl_HashSearch hash_search;
  int exists;
  char *s_el_name;
  char *skey;

  if (primary_dtd == NULL || secondary_dtd == NULL)
    return(-1);

  for (p = primary_dtd->Elements; p != NULL &&
       p->next_element != NULL; p = p->next_element) ;

  

  if ((p->next_element = secondary_dtd->Elements) != NULL) {
    secondary_dtd->Elements->secondary_start_flag = 1;
  }

  /* go through the secondary and add links in the element hash tab */
  for (entry = Tcl_FirstHashEntry(secondary_dtd->SGML_Element_hash_tab ,
				  &hash_search); 
       entry != NULL; 
       entry = Tcl_NextHashEntry(&hash_search)) {
    /* get the name */
    s_el_name = Tcl_GetHashKey(secondary_dtd->SGML_Element_hash_tab,entry); 
    /* and the def info */
    s = (SGML_Element *) Tcl_GetHashValue(entry);

    /* first check to see if this element is already defined... */
    p = (SGML_Element *) find_ns_hash(s_el_name, 
				      primary_dtd->SGML_Element_hash_tab);

    if (p == NULL) {
	  
	  newentry = Tcl_CreateHashEntry(
				      primary_dtd->SGML_Element_hash_tab,
				      s_el_name, &exists );
	  if (newentry != NULL)
	    Tcl_SetHashValue(newentry, 
			     (ClientData)s);

    } 
    else { /* we have name duplication between schemas... */
      if (strcmp(s_el_name,"##ANY##") != 0)
	fprintf(LOGFILE,"Duplicate element name in merging schemas ... %s\n",
		s_el_name);
    }
  }

  return (0);

}


SGML_DTD *
xml_schema_2_dtd(SGML_Document *schemadoc, int first_pass)
{

  SGML_Element *last_sgml_element = NULL, *current_sgml_element = NULL;

  SGML_Sub_Element_List *last_sgml_sub_element = NULL, *first_sub_element,
    *current_sgml_sub_element = NULL;

  SGML_Entity  *last_sgml_entity = NULL, *current_sgml_entity = NULL, 
    *active_sgml_entity = NULL;
  
  SGML_Attribute  *First_sgml_attribute = NULL, *last_sgml_attribute = NULL, 
    *current_sgml_attribute = NULL;
  
  SGML_Notation  *last_sgml_notation = NULL, *current_sgml_notation = NULL;
  

  SGML_Tag_List *first_tag = NULL, *last_tag = NULL, *current_tag = NULL;
  
  SGML_Attribute_Value *first_attr_value = NULL, *last_attr_value = NULL, 
    *current_attr_value = NULL;
  
  SGML_Attribute_Data *atd;
  SGML_Element *new_element;
  SGML_DTD *curr_dtd;
  SGML_Data *schema_data_ptr;

  char *c;
  SGML_Tag_Data *td, *el, *td2, *eldata, *temptd;
  idx_key key, subkey, subsubkey, *k;
  char *elementname;
  XML_type_def *sourcetype;
  SGML_Element *subst_element;
  
  if (schemadoc == NULL) 
    return (NULL);
  
  curr_dtd = xml_schema_init_dtd(schemadoc);

  current_sgml_element = NULL;
  first_sub_element = current_sgml_sub_element = NULL;
  current_sgml_entity = NULL;
  current_sgml_attribute = NULL;
  First_sgml_attribute = NULL;
  current_sgml_notation = NULL;
  first_tag = current_tag = NULL;
  first_attr_value = current_attr_value = NULL;

  /* get the SCHEMA tag and extract the namespace info */

  memset(&key, 0, sizeof(idx_key));
  key.key = "schema";
  key.occurrence = 0;

  td =  comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
  if (td == NULL || td->item == NULL) {
    SGML_ERROR(PARSE_ERROR,
                "No schema tag in XMLSchema document","","","");
    return(NULL);
  } 
  /* Go through the attributes and get the namespaces, etc */
  schema_data_ptr = td->item;

  for (atd = schema_data_ptr->attributes; atd; 
       atd = atd->next_attribute_data) {
    if (strcmp(atd->name,"targetNamespace") == 0) {
      Tcl_HashEntry *entry; 
      int exists;
      char *newspaceid;
      
      newspaceid = strdup(atd->string_val);
      curr_dtd->targetnamespace = newspaceid;
      entry = Tcl_CreateHashEntry(
				  curr_dtd->XML_Namespace_hash_tab,
				  "TARGETNAMESPACE", &exists );
      if (entry != NULL)
	Tcl_SetHashValue(entry, 
			 (ClientData)newspaceid);
	  
	  
      entry = Tcl_CreateHashEntry(
				  curr_dtd->XML_Namespace_hash_tab,
				  newspaceid, &exists );
      if (entry != NULL)
	Tcl_SetHashValue(entry, 
			 (ClientData)strdup("TARGETNAMESPACE"));
	  

    }
    else if (strncmp(atd->name,"xmlns",5) == 0) {
      if (strlen(atd->name) == 5) /* default namespace */
	curr_dtd->namespace = strdup(atd->string_val);
      else {
	Tcl_HashEntry *entry; 
	int exists;
	char *namepart;
	char *newid; 
	namepart = strchr(atd->name, ':');
	if (namepart != NULL) {
	  namepart++;
	  
	  newid = strdup(atd->string_val);

	  entry = Tcl_CreateHashEntry(
				      curr_dtd->XML_Namespace_hash_tab,
				      namepart, &exists );
	  if (entry != NULL)
	    Tcl_SetHashValue(entry, 
			     (ClientData)newid);
	  
	  entry = Tcl_CreateHashEntry(
				      curr_dtd->XML_Namespace_hash_tab,
				    newid, &exists );
	  if (entry != NULL)
	    Tcl_SetHashValue(entry, 
			     (ClientData)strdup(namepart));
	  
	}
	else {
	  SGML_ERROR(PARSE_ERROR,
		     "Unknown schema tag attribute in XMLSchema document ",atd->name,"","");
	  return(NULL);
	  
	}
      }
    }
    else if (strncmp(atd->name,"xml:lang",8) == 0)  {
      /* ignore language specs for now */
      
      }
    else if (strcmp(atd->name,"blockDefault") == 0) {
      curr_dtd->blockDefault = 1;
    }
    else if (strcmp(atd->name,"finalDefault") == 0) {
      curr_dtd->finalDefault = 1;
    }
    else if (strcmp(atd->name,"elementFormDefault") == 0) {
      if (strcmp(atd->string_val,"qualified") == 0) {
	curr_dtd->elementFormDefault = 1;
      }
    }
    else if (strcmp(atd->name,"attributeFormDefault") == 0) {
      if (strcmp(atd->string_val,"qualified") == 0) {
	curr_dtd->attributeFormDefault = 1;
      }
    }
    else {
#ifdef DEBUGSCHEMA
      printf("Unknown attribute of schema = '%s = %s'\n", 
	     atd->name, atd->string_val);
#endif      
    }
    
  }
  
  free_tag_data_list(td);
  
  /* first we extract and store information about the defined SIMPLETYPES */
  memset(&key, 0, sizeof(idx_key));
  key.key = "attribute";
  key.occurrence = 0;

  for (temptd = td = comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
       td ; td = td->next_tag_data) {
    XML_type_def *current_def;
    /* only create entries for top-level defs (which will be referred to */
    /* by attribute ref="name" elsewhere */
    if (td->item->parent != schema_data_ptr)
      continue;
    
    current_def = xml_schema_create_typedef(td->item->attributes,
					    curr_dtd->XML_Typedef_hash_tab);
    
    if (current_def == NULL)
      continue;
    /* OK we have named attribute definition -- now we extract its data */
    xml_schema_get_attribute(current_def, td->item,
			     schemadoc->Tag_hash_tab,
			     curr_dtd->XML_Typedef_hash_tab);
    
  } /* end top-level attributes loop */
  
  free_tag_data_list(temptd);
  


  /* first we extract and store information about the defined attr groups */
  memset(&key, 0, sizeof(idx_key));
  key.key = "attributeGroup";
  key.occurrence = 0;
  
  for (temptd = td = comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
       td ; td = td->next_tag_data) {
    XML_type_def *current_def;
    /* only create entries for top-level defs (which will be referred to */
    /* by attribute ref="name" elsewhere */
    if (td->item->parent != schema_data_ptr)
      continue;
    
    current_def = xml_schema_create_typedef(td->item->attributes,
					    curr_dtd->XML_Typedef_hash_tab);
    
    if (current_def == NULL)
      continue;
    
    xml_schema_get_attributegroup(current_def,td->item,
				  schemadoc->Tag_hash_tab,
				  curr_dtd->XML_Typedef_hash_tab);
  } /* end top-level attributegroups loop */
  
  free_tag_data_list(temptd);
  
  /* Next we extract and store information about the defined SIMPLETYPES */
  memset(&key, 0, sizeof(idx_key));
  key.key = "simpleType";
  key.occurrence = 0;

  for (temptd = td = comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
       td ; td = td->next_tag_data) {
    XML_type_def *current_def;

    current_def = xml_schema_create_typedef(td->item->attributes,
					    curr_dtd->XML_Typedef_hash_tab);
    
    if (current_def == NULL) {
      /* an unnamed type -- probably part of an ELEMENT def */
      if (td->item->parent != NULL 
	  && td->item->parent->element != NULL
	  && td->item->parent->element->tag != NULL
	  && compare_nstags(td->item->parent->element->tag, "element") == 0) {
	
	/* we get the name of the parent element (it SHOULD have one) */
	current_def = xml_schema_create_typedef(td->item->parent->attributes,
						curr_dtd->XML_Typedef_hash_tab);
    
      }
    }
    /* if no def by this point, just continue the loop */
    if (current_def == NULL) 
      continue;

    /* OK we have named simpleType definition -- now we extract its data */
    xml_schema_get_simpleType(current_def, td->item,
			      schemadoc->Tag_hash_tab,
			      curr_dtd->XML_Typedef_hash_tab);

  } /* end Simpletypes loop */

  free_tag_data_list(temptd);

  /* Next we extract and store information about the defined COMPLEXTYPES */
  memset(&key, 0, sizeof(idx_key));
  key.key = "complexType";
  key.occurrence = 0;

  
  for (temptd = td = comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
       td ; td = td->next_tag_data) {
    XML_type_def *current_def;
    
    current_def = xml_schema_create_typedef(td->item->attributes,
					    curr_dtd->XML_Typedef_hash_tab);
    if (current_def == NULL) {
      /* an unnamed type -- probably part of an ELEMENT def */
      if (td->item->parent != NULL 
	  && td->item->parent->element != NULL
	  && td->item->parent->element->tag != NULL
	  && compare_nstags(td->item->parent->element->tag, "element") == 0) {
	
	/* we get the name of the parent element (it SHOULD have one) */
	current_def = xml_schema_create_typedef(td->item->parent->attributes,
						curr_dtd->XML_Typedef_hash_tab);
    
      }
    }
    /* if no def by this point, just continue the loop */
    if (current_def == NULL) 
      continue;

    /* OK we have named ComplexType definition -- now we extract its data */
    xml_schema_get_complexType(current_def, td->item,
			      schemadoc->Tag_hash_tab,
			      curr_dtd->XML_Typedef_hash_tab);

  } /* end types loop */

  free_tag_data_list(temptd);

  /* Now we go though all of the defined types with subelements and */
  /* link everything together                                       */
  xml_schema_link_types(curr_dtd->XML_Typedef_hash_tab);
  

  /* Check the elements and build the DTD element structures */
  memset(&key, 0, sizeof(idx_key));
  key.key = "element";
  key.occurrence = 0;

  eldata = comp_tag_list(&key, schemadoc->Tag_hash_tab, NULL, 1);
  /* we will do two passes -- first get all of the named elements */
  /* then get the references and create data models for them      */
  for (el = eldata; el ; el = el->next_tag_data) {
    int nameflag ;
    int typeflag ;
    nameflag = 0;
    typeflag = 0;
    elementname = NULL;
    new_element = NULL;
    /* check whether this is a name attribute or a ref attribute */
    for (atd = el->item->attributes; atd; atd = atd->next_attribute_data) {
      if (strcmp(atd->name,"ref") == 0) {
	continue;
      } 
      else if (strcmp(atd->name,"type") == 0) {
	if (current_sgml_element->data_type == 0 
	    && current_sgml_element->data_model == NULL) {
	  typeflag = 1;
	  sourcetype = (XML_type_def *)
	    find_ns_hash(atd->string_val, 
			 curr_dtd->XML_Typedef_hash_tab);
	  /* should be a current element already */
	  if (sourcetype) {
	    
	    if (sourcetype->primitivebasetype == XML_EMPTY) {
	      /* This is an empty element meaning model is NULL */
	      current_sgml_element->data_type = EL_EMPTY;
	    }
	    else if (sourcetype->primitivebasetype > 100) {
	    
	      current_sgml_element->data_type = 0; /* data model */
	      current_sgml_element->data_model =
		xml_schema_dup_model(current_sgml_element, 
				     sourcetype->model->group);
	      if (sourcetype->model->group_repetition)
		current_sgml_element->model_rep = 
		  strdup(sourcetype->model->group_repetition);
	      current_sgml_element->min_occur = sourcetype->model->min_occurs;
	      current_sgml_element->max_occur = sourcetype->model->max_occurs;
	      current_sgml_element->attribute_list = 
		xml_schema_dup_attributes(current_sgml_element,
					  sourcetype->associated_attributes);
	    }
	    else {
	      current_sgml_element->data_type = sourcetype->primitivebasetype;
	      current_sgml_element->data_model = 
		xml_create_primitive_dm(sourcetype);
	      current_sgml_element->attribute_list = 
		xml_schema_dup_attributes(current_sgml_element,
					  sourcetype->associated_attributes);
	    }
	  }
	}
      }
      else if (strcmp(atd->name, "name") == 0) {
	/* this is a named element */
	Tcl_HashEntry *entry; 
	int exists;
	char *namepart;
	char *tempns;
	
	nameflag = 1;
	
	/* first check to see if this element is already defined... */
	current_sgml_element = (SGML_Element *) 
	  find_ns_hash(atd->string_val, 
		       curr_dtd->SGML_Element_hash_tab);
	
	if (current_sgml_element == NULL) {
	  
	  
	  if ((current_sgml_element = CALLOC(SGML_Element,1))
	      == NULL) {
	    SGML_ERROR(ALLOC_ERROR,
		       "Unable to allocate SGML_Element","","","");
	  }
	  
	  new_element = current_sgml_element;

	  
	  current_sgml_element->tag = strdup(atd->string_val);
	  current_sgml_element->namespace = 
	    xml_schema_get_namespace(curr_dtd, atd->string_val);

	  if (last_sgml_element == NULL) { /* first for DTD */
	    curr_dtd->Elements = current_sgml_element;
	    last_sgml_element = current_sgml_element;
	  }
	  else {
	    last_sgml_element->next_element = current_sgml_element;
	    last_sgml_element = current_sgml_element;
	  }
	  /* add the element name(s) to the hash table */
	  /* single name for element */
	  namepart = strchr(current_sgml_element->tag, ':');
	  if (namepart == NULL)
	    namepart = current_sgml_element->tag;
	  else
	    namepart++;
	  
	  entry = Tcl_CreateHashEntry(
				      curr_dtd->SGML_Element_hash_tab,
				      namepart, &exists );
	  if (entry != NULL)
	    Tcl_SetHashValue(entry, 
			     (ClientData)current_sgml_element);
	  else if (exists)
	    SGML_ERROR(INFORM_ERROR,
		       "ELEMENT NAME",	current_sgml_element->tag,
		       "ALREADY EXISTS IN SCHEMA!","");
	}
      } 
      if (strcmp(atd->name, "substitutionGroup") == 0) {
	/* this is a name substitution -- add a duplicated element */
	Tcl_HashEntry *entry; 
	int exists;
	char *namepart;
	char *tempns;

	nameflag = 1;
	
	/* first check to see if this element is already defined... */
	subst_element = (SGML_Element *)
	  find_ns_hash(atd->string_val, 
		       curr_dtd->SGML_Element_hash_tab);
	/* should be a current element already */
	if (subst_element) {
	  
	  current_sgml_element->data_type = subst_element->data_type;
	  current_sgml_element->data_model =
	      xml_schema_dup_model(subst_element, 
				   subst_element->data_model);
	  if (subst_element->model_rep) {
	    current_sgml_element->model_rep = strdup(subst_element->model_rep);
	  }
	  current_sgml_element->min_occur = subst_element->min_occur;
	  current_sgml_element->max_occur = subst_element->max_occur;
	  current_sgml_element->attribute_list = 
	    xml_schema_dup_attributes(current_sgml_element,
				      subst_element->attribute_list);
	 
	}
	else {
	  SGML_ERROR(INFORM_ERROR,
		     "SUBSTITUTUION GROUP ELEMENT NAME", atd->string_val, 
		       "NOT FOUND IN SCHEMA! (Element order problem??)","");
	}
      }
    }
    /* if a new element has been created... */
    if (new_element) {
      new_element->min_occur = 
	xml_schema_get_minOccurs(el->item->attributes);
      new_element->max_occur = 
	xml_schema_get_maxOccurs(el->item->attributes);
      if (nameflag == 1 && typeflag == 0) {
	/* check for an in-context def/type for this element */
	sourcetype = (XML_type_def *)
	  find_ns_hash(current_sgml_element->tag,
		       curr_dtd->XML_Typedef_hash_tab);
	/* should be a current element already */
	if (sourcetype) {

	  if (sourcetype->primitivebasetype == XML_EMPTY) {
	    /* This is an empty element meaning model is NULL */
	    current_sgml_element->data_type = EL_EMPTY;
	  }
	  else if (sourcetype->primitivebasetype > 100) {
	    current_sgml_element->data_type = 0; /* data model */
	    current_sgml_element->data_model =
	      xml_schema_dup_model(current_sgml_element, 
				   sourcetype->model->group);
	    if (sourcetype->model->group_repetition)
	      current_sgml_element->model_rep = 
		strdup(sourcetype->model->group_repetition);
	    current_sgml_element->min_occur = sourcetype->model->min_occurs;
	    current_sgml_element->max_occur = sourcetype->model->max_occurs;
	    current_sgml_element->attribute_list = 
	      xml_schema_dup_attributes(current_sgml_element,
					sourcetype->associated_attributes);
	  }
	  else {
	    current_sgml_element->data_type = sourcetype->primitivebasetype;
	    current_sgml_element->data_model = 
	      xml_create_primitive_dm(sourcetype);
	    current_sgml_element->attribute_list = 
	      xml_schema_dup_attributes(current_sgml_element,
					sourcetype->associated_attributes);
	  }
	}
      } /*end nameflag but no typeflag block */
    } /* end new_element additions */
  } /* end element loop */
  
  /* now do the second pass for references */
  for (el = eldata; el ; el = el->next_tag_data) {
    elementname = NULL;
    new_element = NULL;
    /* check whether this is a name attribute or a ref attribute */
    for (atd = el->item->attributes; atd; atd = atd->next_attribute_data) {
      if (strcmp(atd->name,"ref") == 0) {
	current_sgml_element = (SGML_Element *) 
	  find_ns_hash(atd->string_val, 
		       curr_dtd->SGML_Element_hash_tab);

	if (current_sgml_element == NULL) {
	  /* printf("REF=%s not found\n", atd->string_val); */
	}
      } 
    }
  }



  free_tag_data_list(eldata);

  if (xml_schema_defines_ANY) 
    xml_schema_add_any_el(curr_dtd, "##ANY##", 0);

  return (curr_dtd);

}





