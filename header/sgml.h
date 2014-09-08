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
*	Header Name:	sgml.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	data structures for storing information about
*                       SGML declarations and DTDs
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7/22/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/


/* include tcl.h for hash table defs */
#include <tcl.h>

/* a few constants */

#define TEMP_PARSE_BUFFER_SIZE 100000


/* The following is adapted from sgmldecl.h in sgmls */
#define QATTCNT 0 
#define QATTSPLEN 1 
#define QBSEQLEN 2 
#define QDTAGLEN 3 
#define QDTEMPLEN 4 
#define QENTLVL 5 
#define QGRPCNT 6 
#define QGRPGTCNT 7 
#define QGRPLVL 8 
#define QLITLEN 9 
#define QNAMELEN 10
#define QNORMSEP 11
#define QPILEN 12
#define QTAGLEN 13
#define QTAGLVL 14

#define NQUANTITY (QTAGLVL+1)

#define C_TOTALCAP 0 
#define C_ENTCAP 1 
#define C_ENTCHCAP 2 
#define C_ELEMCAP 3 
#define C_GRPCAP 4 
#define C_EXGRPCAP 5 
#define C_EXNMCAP 6 
#define C_ATTCAP 7 
#define C_ATTCHCAP 8 
#define C_AVGRPCAP 9 
#define C_NOTCAP 10
#define C_NOTCHCAP 11
#define C_IDCAP 12
#define C_IDREFCAP 13
#define C_MAPCAP 14
#define C_LKSETCAP 15
#define C_LKNMCAP 16

#define NCAPACITY (C_LKNMCAP+1)


#define YES 1
#define NO 0

#define UNUSED -1
#define UNKNOWN -2
#define UNDESC -3
#define UNKNOWN_SET -4

extern int iso646charset[];
extern int iso646G0charset[];
extern int iso646C0charset[];
extern int iso8859_1charset[];
extern int iso6429C1charset[];


typedef struct SGML_CHARSET {
  char *BASESET_id;
  
} SGML_CHARSET ;


typedef struct SGML_Declaration {
  char *filename;
  char *version_id;
  char *encoding; /* primarily used for XML */
  int capacity[NCAPACITY];
  int  datatag;
  int  omittag;
  int  rank;
  int shorttag;
  int subdoc;
  int concur;
  int  formal;
  int shortref;
  int namecase[2];		/* case translation of general/entity names */
  int quantity[NQUANTITY];
} SGML_Declaration ;

extern SGML_Declaration *current_sgml_declaration;

typedef struct SGML_Tag_List {
  char *tag;
  char *connector;
  struct SGML_Tag_List *next_tag;
} SGML_Tag_List;

#define CDATA_ATTR             1
#define ENTITY_ATTR            2
#define ENTITIES_ATTR          3
#define ID_ATTR                4
#define IDREF_ATTR             5
#define IDREFS_ATTR            6
#define NAME_ATTR              7
#define NAMES_ATTR             8
#define NMTOKEN_ATTR           9
#define NMTOKENS_ATTR         10
#define NUMBER_ATTR           11
#define NUMBERS_ATTR          12
#define NUTOKEN_ATTR          13
#define NUTOKENS_ATTR         14
#define NOTATION_ATTR         15
#define NAME_TOKEN_GROUP_ATTR 16



#define FIXED_DEFAULT          1
#define SPECIFIED_DEFAULT      2
#define REQUIRED_DEFAULT       3
#define CURRENT_DEFAULT        4
#define CONREF_DEFAULT         5
#define IMPLIED_DEFAULT        6
#define PROHIBITED_DEFAULT     7

/* XML Schema Primitive and Derived datatypes */
#define XML_string_Type 1
#define XML_boolean_Type 2
#define XML_float_Type 3
#define XML_double_Type 4
#define XML_decimal_Type 5
#define XML_duration_Type 6
#define XML_dateTime_Type 7
#define XML_time_Type 8
#define XML_date_Type 9
#define XML_gYearMonth_Type 10
#define XML_gYear_Type 11
#define XML_gMonthDay_Type 12
#define XML_gMonth_Type 13
#define XML_gDay_Type 14
#define XML_hexBinary_Type 15
#define XML_base64Binary_Type 16
#define XML_anyURI_Type 17
#define XML_QName_Type 18
#define XML_NOTATION_Type 19
#define XML_normalizedString_Type 20
#define XML_token_Type 21
#define XML_language_Type 22
#define XML_IDREFS_Type 23
#define XML_ENTITIES_Type 24
#define XML_NMTOKEN_Type 25
#define XML_NMTOKENS_Type 26
#define XML_Name_Type 27
#define XML_NCName_Type 28
#define XML_ID_Type 29
#define XML_IDREF_Type 30
#define XML_ENTITY_Type 31
#define XML_integer_Type 32
#define XML_nonPositiveInteger_Type 33
#define XML_negativeInteger_Type 34
#define XML_long_Type 35
#define XML_int_Type 36
#define XML_short_Type 37
#define XML_byte_Type 38
#define XML_nonNegativeInteger_Type 39
#define XML_unsignedLong_Type 40
#define XML_unsignedInt_Type 41
#define XML_unsignedShort_Type 42
#define XML_unsignedByte_Type 43
#define XML_positiveInteger_Type 44

#define XML_GROUP 101
#define XML_SEQUENCE 102
#define XML_CHOICE 103
#define XML_ALL 104 
#define XML_ANY 105 
#define XML_EMPTY 106 


typedef struct SGML_Attribute_Value {
  char *value;
  char *connector;
  struct SGML_Attribute_Value *next_value;
} SGML_Attribute_Value;


typedef struct SGML_Attribute {
  char *assoc_tag;
  SGML_Tag_List *assoc_tag_list;
  char *name;
  int declared_value_type;
  SGML_Attribute_Value *value_list;
  int  default_value_type;
  char *default_value;
  struct SGML_Attribute *next_attribute;
} SGML_Attribute ;


/* data types for SGML elements */
#define CONTENT_MODEL 0
#define EL_PCDATA 1
#define EL_CDATA  2
#define EL_RCDATA 3
#define EL_EMPTY  4
#define EL_ANY    5
#define EL_NAMED  6
#define EL_GROUP  7
#define EL_MIXED  8

typedef struct SGML_Element {
  char *tag;
  SGML_Tag_List *tag_list;
  int  start_tag_min;
  int  end_tag_min;
  int  data_type; /* One of the fixed types, e.g. PCDATA, or 0 if subelements*/
  char *namespace;
  SGML_Attribute *attribute_list; /* Attributes associated with this tag */
  struct SGML_Sub_Element_List *data_model;
  char *model_rep; /* repetition of data model group */
  int  max_occur; /* XML Schema additions */
  int  min_occur; /* XML Schema additions */
  struct SGML_Tag_List *exclusions;
  struct SGML_Tag_List *inclusions;
  int secondary_start_flag;
  struct SGML_Element *next_element; /* to keep list of all elements */
} SGML_Element ;

typedef struct SGML_Sub_Element_List {
  SGML_Element *element_parent;
  struct SGML_Sub_Element_List *group_parent;
  int sub_element_type;  /* use of EL_ANY and EL_PCDATA ok here */
  char *element_name;    /* used for access */
  SGML_Element *element; /* Null if group */
  struct SGML_Sub_Element_List *group;
  char *group_repetition;
  char *repetition;
  int min_occurs; /* XML Schema additions */
  int max_occurs; /* XML Schema additions */
  char *xml_type_name; /* used in schema parsing */
  char *connector; /* type of connection with next sub_element */
  struct SGML_Sub_Element_List *next_sub_element; /* next in group */
  
} SGML_Sub_Element_List;


#define GENERAL_ENTITY   1
#define PARAMETER_ENTITY 2
#define DEFAULT_ENTITY   3

#define PARAMETER_LIT_TEXT 1
#define DATA_TEXT         2
#define BRACKETED_TEXT    3
#define EXTERNAL_ENTITY_SPEC 4

#define CDATA_TEXT 1
#define SDATA_TEXT 2
#define PI_TEXT 3

#define STARTTAG_TEXT 1
#define ENDTAG_TEXT   2
#define MS_TEXT       3
#define MD_TEXT       4

#define PUBLIC_IDENT  1
#define SYSTEM_IDENT  2

typedef struct SGML_Entity {
  char *name;
  int type; 
  int entity_text_type;
  int entity_text_subtype;
  char *entity_text_string;
  char *public_name;
  char *system_name;
  struct SGML_Entity *next_entity;
} SGML_Entity;

typedef struct SGML_Notation {
  char *name;
  int type;
  char *public_name;
  char *system_name;
  struct SGML_Notation *next_notation;
} SGML_Notation;

typedef struct SGML_Catalog {
  char *Type;
  int  typeid;
  char *public_name;
  char *system_name;
} SGML_Catalog;


typedef struct XML_type_val_list {
  struct XML_type_val_list *next_value;
  char *item; /* this will take casting to use */
} XML_type_val_list;

typedef struct XML_type_def {
  int primitivebasetype;
  struct XML_type_def *derivedbase;
  char *typename; /* null if anonymous */
  XML_type_val_list *patterns;
  XML_type_val_list *enumeration;
  /* the facets defined for this type */
  char *length;
  char *minlength;
  char *maxlength;
  char *minInclusive;
  char *maxInclusive;
  char *minExclusive;
  char *maxExclusive;
  SGML_Element *element_def;
  SGML_Attribute *associated_attributes;  
  SGML_Sub_Element_List *model;
} XML_type_def;

typedef struct SGML_DTD {
  int type; /* sgml or xml or xmlschema */
  char *DTD_name;
  char *DTD_system_name;
  char *DTD_public_name;
  char *DTD_file_name;
  char *namespace;          /* XML-Schema */
  char *targetnamespace;    /* XML-Schema */
  int  elementFormDefault;    /* XML-Schema 1=qualified 0=unqualified */
  int  attributeFormDefault;  /* XML-Schema 1=qualified 0=unqualified */
  int  blockDefault;          /* XML-Schema 1=specified 0=not spec. */
  int  finalDefault;          /* XML-Schema 1=specified 0=not spec. */
  char *XML_encoding;        /* XML-Schema */
  SGML_Declaration *sgml_declaration;
  Tcl_HashTable *SGML_Element_hash_tab;
  Tcl_HashTable *SGML_Entity_hash_tab;
  Tcl_HashTable *SGML_Catalog_hash_tab;
  Tcl_HashTable *XML_Namespace_hash_tab; /* XML & XML-Schema */
  Tcl_HashTable *XML_Typedef_hash_tab;   /* XML & XML-Schema */
  SGML_Element *Elements;
  SGML_Entity  *Entities;
  SGML_Notation *Notation;
  int  display_options;
  struct SGML_DTD *next_dtd;
} SGML_DTD;


/*  SGML_Attribute_Data for attributes associated with tags */
/*  in SGML_Data.                                           */
typedef struct SGML_Attribute_Data {
  SGML_Attribute *attr_def;
  char *name;
  char *string_val;
  int  int_val;
  struct SGML_Attribute_Data *next_attribute_data;
 } SGML_Attribute_Data;
  

#define INSTR_INDEXING 1
#define INSTR_DISPLAY  2
#define PROC_SUBST_TAG 1
#define PROC_SUBST_ATTR 2
#define PROC_DELETE_TAG 3
#define PROC_INDEX_SUBST_TAG 4
#define PROC_INDEX_SUBST_ATTR 5
#define PROC_INDEX_DELETE_TAG 6

typedef struct SGML_Processing_Inst {
  int Instruction_Type ;    /* INDEXING or DISPLAY processing */
  int Instruction ;         /* subst_tag or subst_attr (for now) */
  char *tag; 
  char *attr;
  char *index_name;
  struct SGML_Processing_Inst *next_processing_inst;
} SGML_Processing_Inst;


#define PROC_FLAG_SUBST_TAG 1<<(PROC_SUBST_TAG-1)
#define PROC_FLAG_SUBST_ATTR 1<<(PROC_SUBST_ATTR-1)
#define PROC_FLAG_DELETE_TAG 1<<(PROC_DELETE_TAG-1)
#define PROC_FLAG_INDEX_SUBST_TAG 1<<(PROC_INDEX_SUBST_TAG-1)
#define PROC_FLAG_INDEX_SUBST_ATTR 1<<(PROC_INDEX_SUBST_ATTR-1)
#define PROC_FLAG_INDEX_DELETE_TAG 1<<(PROC_INDEX_DELETE_TAG-1)
#define PROC_FLAG_IN_CHILD 1<<30


typedef struct SGML_Data {
  SGML_Element *element;
  char *start_tag;         /* pointer to beginning of start tag in buffer */
  char *content_start;     /* pointer to beginning of contents in buffer  */ 
  char *content_end;       /* pointer to end  of contents in buffer       */ 
  char *end_tag;           /* pointer to beginning of end tag in buffer   */ 
  char *data_end;          /* pointer to end of end tag in buffer         */ 
  int data_element_id;
  int occurrence_no;       /* for same tag under same parents...          */
  int start_tag_offset;    /* offset to beginning of start tag in buffer  */
  int content_start_offset;/* offset to beginning of contents in buffer   */ 
  int content_end_offset;  /* offset to end  of contents in buffer        */ 
  int end_tag_offset;      /* offset to beginning of end tag in buffer    */ 
  int data_end_offset;     /* offset to end of end tag in buffer          */ 
  int processing_flags;    /* for handling processing instructions        */
  SGML_Attribute_Data *attributes;  /* list of attributes                 */
  SGML_Processing_Inst *Processing_instr; /* ONE processing instr for now */
  SGML_Entity *local_entities;
  struct SGML_Data *parent;
  struct SGML_Data *sub_data_element;
  struct SGML_Data *next_data_element;
} SGML_Data;

/* lists of this structure are stored in the Tag_hash_tab for each */
/* unique tag in SGML_Document's data structures                   */
typedef struct SGML_Tag_Data {
  struct SGML_Tag_Data *next_tag_data;
  struct SGML_Data *item;
} SGML_Tag_Data;

typedef struct SGML_Document {
  SGML_Declaration *DCL;
  SGML_DTD *DTD;
  SGML_Data *data;  /* pointers into the buffer */
  char *buffer;     /* data buffer for this record/document */
  char *file_name;  /* data file for the document */
  int record_id;   /* logical record number/id for the document */ 
  int file_offset; /* offset within the file of document */
  char *encoding; /* encoding name: used for XML */
  int  standalone; /* used for XML */
  int  dist_docsize; /* used for distributed collection docs */
  int  element_count; /* incremented during parsing */
  SGML_Processing_Inst *Processing_Inst;
  Tcl_HashTable *Tag_hash_tab;
} SGML_Document;
/* assignment macros for SGML_Declaration */

typedef struct SGML_Doclist 
{
	SGML_Document *doc;
	struct SGML_Doclist *next_doc;
} SGML_Doclist ;



#define OMITTAG (current_sgml_declaration->omittag)
#define SUBDOC (current_sgml_declaration->subdoc)
#define SHORTTAG (current_sgml_declaration->shorttag)
#define FORMAL (current_sgml_declaration->formal)

#define ATTCNT (current_sgml_declaration->quantity[QATTCNT])
#define ATTSPLEN (current_sgml_declaration->quantity[QATTSPLEN])
#define BSEQLEN (current_sgml_declaration->quantity[QBSEQLEN])
#define DTAGLEN (current_sgml_declaration->quantity[QDTAGLEN])
#define DTEMPLEN (current_sgml_declaration->quantity[QDTEMPLEN])
#define ENTLVL (current_sgml_declaration->quantity[QENTLVL])
#define GRPGTCNT (current_sgml_declaration->quantity[QGRPGTCNT])
#define GRPCNT (current_sgml_declaration->quantity[QGRPCNT])
#define GRPLVL (current_sgml_declaration->quantity[QGRPLVL])
#define LITLEN (current_sgml_declaration->quantity[QLITLEN])
#define NAMELEN (current_sgml_declaration->quantity[QNAMELEN])
#define NORMSEP (current_sgml_declaration->quantity[QNORMSEP])
#define PILEN (current_sgml_declaration->quantity[QPILEN])
#define TAGLEN (current_sgml_declaration->quantity[QTAGLEN])
#define TAGLVL (current_sgml_declaration->quantity[QTAGLVL])

#define NAMECASE (current_sgml_declaration->namecase[0])
#define ENTCASE (current_sgml_declaration->namecase[1])



#define TOTALCAP (current_sgml_declaration->capacity[C_TOTALCAP])
#define ENTCAP (current_sgml_declaration->capacity[C_ENTCAP])
#define ENTCHCAP (current_sgml_declaration->capacity[C_ENTCHCAP])
#define ELEMCAP (current_sgml_declaration->capacity[C_ELEMCAP])
#define GRPCAP (current_sgml_declaration->capacity[C_GRPCAP])
#define EXGRPCAP (current_sgml_declaration->capacity[C_EXGRPCAP])
#define EXNMCAP (current_sgml_declaration->capacity[C_EXNMCAP])
#define ATTCAP (current_sgml_declaration->capacity[C_ATTCAP])
#define ATTCHCAP (current_sgml_declaration->capacity[C_ATTCHCAP])
#define AVGRPCAP (current_sgml_declaration->capacity[C_AVGRPCAP])
#define NOTCAP (current_sgml_declaration->capacity[C_NOTCAP])
#define NOTCHCAP (current_sgml_declaration->capacity[C_NOTCHCAP])
#define IDCAP (current_sgml_declaration->capacity[C_IDCAP])
#define IDREFCAP (current_sgml_declaration->capacity[C_IDREFCAP])
#define MAPCAP (current_sgml_declaration->capacity[C_MAPCAP])
#define LKSETCAP (current_sgml_declaration->capacity[C_LKSETCAP])
#define LKNMCAP (current_sgml_declaration->capacity[C_LKNMCAP])




















