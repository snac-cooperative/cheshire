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
/* this is a LLgen grammar file */
{

/************************************************************************
*
*	Header Name:	sgml_gram.g
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	LLgen format grammar for parsing 
*			SGML declarations, DTDs, and documents
*	Usage:		Must be processed through LLgen -- see makefile
*	Variables:	the "current_sgml_..." variables are declared
*			below -- these are globals that can be used
*			elsewhere, but are not recommended.
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

#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#ifndef SOLARIS
#include "strings.h"
#endif
#endif
#include <string.h>

#include <sgml.h> 
#include <dmalloc.h>

#define ALLOC_ERROR 100
#define UNDEFINED_TAG_ERROR 200
#define PARSE_ERROR 1
#define INFORM_ERROR 0

/* #define DEBUGDTD 1 */
/* #define DEBUGCAT 1 */
/* #define DEBUGDOC 1 */

extern FILE *LOGFILE;
extern  int last_rec_proc;

/* sgml_yylval contains the current values from the lexical analysis */
struct yyLVAL { 
        int  tok_id;
        char *string;
        int intval;
	Tcl_HashEntry *hash_entry; 
 } sgml_yylval, sgml_yylval_pushback, sgml_yylval_include_back ;
 
 extern FILE *sgml_in;
 FILE *orig_sgml_in = NULL, *new_sgml_in = NULL;
 extern char *sgml_in_name;

 int sgml_parse_error = 0;

 int expect_otm;
 int expect_document;
 int expect_decl_value;

#define INCLUDERCSECTION 2
#define INCLUDESECTION 1
#define IGNORESECTION 0

 int marked_section_type;
 char *marked_section_contents;

 int current_entity_data_type;
 
 char *current_doctype_name;
 int system_id_token = 0;
 int public_id_token = 0;
 char *system_id_name = NULL;
 char *public_id_name = NULL;
 int default_entity = 0;
 int in_doc_type_decl = 0;


SGML_Declaration *last_sgml_declaration = NULL, *current_sgml_declaration = NULL;

SGML_Declaration Default_Declaration = {
 "DEFAULT", /*filename*/
 "ISO 8879:1986", /*version*/
 "", /* encoding */
 {150000,0,0,0,150000,0,0,0,0,0,0,0,0,0,0,0}, /* capacity */
 0, /* datatag */
 1, /* omittag */ 
 0, /* rank */
 0, /* shorttag */
 0, /* subdoc */
 0, /* concur */
 1, /* formal */
 1, /* shortref */
 {1,0}, /* namecase */
 /* quantities */
 {1000, 1000, 1000, 1000, 1000, 50, 64, 150, 100, 2000, 200, 0, 200, 2000, 100}
};

SGML_Declaration XML_Declaration = {
 "XML", /*filename*/
 "1.0", /*version*/
 "UTF-8", /*encoding*/
 {150000,0,0,0,150000,0,0,0,0,0,0,0,0,0,0,0}, /* capacity */
 0, /* datatag */
 1, /* omittag */ 
 0, /* rank */
 0, /* shorttag */
 0, /* subdoc */
 0, /* concur */
 1, /* formal */
 1, /* shortref */
 {1,0}, /* namecase */
 /* quantities */
 {1000, 1000, 1000, 1000, 1000, 50, 64, 150, 100, 2000, 200, 0, 200, 2000, 100}
};

SGML_Element *last_sgml_element = NULL, *current_sgml_element = NULL;

SGML_Sub_Element_List *last_sgml_sub_element = NULL, *first_sub_element,
	*current_sgml_sub_element = NULL;

SGML_Entity  *last_sgml_entity = NULL, *current_sgml_entity = NULL, 
	*active_sgml_entity = NULL;

SGML_Attribute  *First_sgml_attribute = NULL, *last_sgml_attribute = NULL, 
	*current_sgml_attribute = NULL;

SGML_Notation  *last_sgml_notation = NULL, *current_sgml_notation = NULL;

SGML_DTD *DTD_list = NULL, *last_sgml_dtd = NULL, *current_sgml_dtd = NULL;

SGML_Tag_List *first_tag = NULL, *last_tag = NULL, *current_tag = NULL;

SGML_Attribute_Value *first_attr_value = NULL, *last_attr_value = NULL, 
                     *current_attr_value = NULL;
SGML_Document *current_sgml_document = NULL;

SGML_Doclist  *first_doc = NULL, *current_doc = NULL, *last_doc = NULL;

SGML_Processing_Inst *current_pi = NULL, *last_pi = NULL;

SGML_Data  *current_sgml_data = NULL, *last_sgml_data = NULL, *error_sgml_data = NULL, *tag_match_doc(SGML_Data *, char *);

SGML_Attribute_Data *current_attr_data = NULL, *last_attr_data = NULL,
			*first_attr_data = NULL;

Tcl_HashTable *Current_SGML_Catalog = NULL;
extern Tcl_HashTable *Current_Doc_Namespaces;
int Namespace_Attribute_Flag = 0;

SGML_Catalog  *temp_SGML_Catalog_Entry;
int data_element_id = 0;
int group_nesting_level = 0;
int lookahead_tag = 0;
int document_tag_nesting = 0;
int force_document_end = 0;
int content_model_flag = 0;
extern int parsed_file_lineno;
extern int parsed_file_chars;
extern int save_parsed_file_chars;
int namespaceverified;

void SGML_ERROR();
char *SubEntities(char *instring);
extern void sgml_restart();
void link_data_model();
void link_sgml_data();
int attach_attributes();
extern int include_buffer_switch();
extern int force_empty_end_tag();
extern int switch_input_to_entity();
/* forward declaration */
void *find_ns_hash(char *name, Tcl_HashTable *tab) ;
int verify_namespace(char *name, Tcl_HashTable *tab, 
                     Tcl_HashTable *doctab, char **ns_ptr);

extern SGML_Element *xml_schema_add_any_el(SGML_DTD *in_dtd, char *tagname,
                                           int force);

#ifdef WIN32
#define strcasecmp _stricmp
#define strdup _strdup
#endif

}

/* character literals */
%token STAGO;
%token ETAGO;
%token TAGC;
%token EMPTYTAGC;
%token EMPTY_START_TAG; 
%token EMPTY_TAG_CONTENTS; 
%token EMPTY_END_TAG;
/* %token NET; */

/* %token MDO;  had to combine with other tokens to avoid conflicts */
%token MDC;
%token NULLMDO;
%token MARKED_SECTION_START;
%token MARKED_SECTION_END;
%token DSO;
%token DSC;
%token PIO;
%token PIC;
/* %token MSC; */
%token COM;
%token COMDCLS;
%token COMDCLE;
%token PERO;
%token PERO_REF;
%token ERO_REF;
%token REFC;
%token GRPO;
%token GRPC;
%token RNI;
%token PLUS;
/* %token MINUS; not used */
%token EXPLUS;
%token EXMINUS;
%token VI;
%token OPT;
%token REP;
%token AND;
%token OR;
%token SEQ;
%token DTGO;
%token DTGC;
%token CRO;

/* string values */
%token PARAMETER_LITERAL;
%token ATTR_VAL_LITERAL;
%token SGMLNAME;
%token NAME_TOKEN;
%token NAME_LIST;
%token NUMBER_LIST;
%token INSTRUCTION;
%token INSTRUCTION_TYPE;
%token CHESHIRE_INSTRUCTION;
%token ARGS;
%token QSTRING;
%token OTM;
%token JUNK;
%token TOKEN_SEP;
%token RCDATA_DATA;
%token CDATA_DATA;
%token PCDATA_DATA;
%token SGMLCHAR;

/* integer values */
%token NUMBER;

/* string literals */
%token XML_DECL;
%token VERSIONINFO;
%token ENCODINGDECL;
%token SDDECL;
%token XML_ENDDECL;
%token MDO_SGML;
%token MDO_DOCTYPE;
%token MDO_ELEMENT;
%token MDO_ENTITY;
%token MDO_SHORTREF;
%token MDO_ATTLIST;
%token MDO_USELINK;

%token CHARSET_DCL;
%token BASESET_DCL;
%token DESCSET_DCL;
%token UNUSED_DCL;
%token CAPACITY_DCL;

%token TOTALCAP_DCL;
%token ENTCAP_DCL;
%token ENTCHCAP_DCL;
%token ELEMCAP_DCL;
%token GRPCAP_DCL;
%token EXGRPCAP_DCL;
%token EXNMCAP_DCL;
%token ATTCAP_DCL;
%token ATTCHCAP_DCL;
%token AVGRPCAP_DCL;
%token NOTCAP_DCL;
%token NOTCHCAP_DCL;
%token IDCAP_DCL;
%token IDREFCAP_DCL;
%token MAPCAP_DCL;
%token LKSETCAP_DCL;
%token LKNMCAP_DCL;

%token SGMLREF_DCL;
%token SCOPE_DCL;
%token DOCUMENT_DCL;
%token INSTANCE_DCL;
%token SYNTAX_DCL;
%token SWITCHES_DCL;
%token SHUNCHAR_DCL;
%token CONTROLS_DCL;
%token FUNCTION_DCL;
%token RE_DCL;
%token RS_DCL;
%token SPACE_DCL;

%token FUNCHAR_DCL;
%token MSICHAR_DCL;
%token MSOCHAR_DCL;
%token MSSCHAR_DCL;
%token SEPCHAR_DCL;
%token NAMING_DCL;
%token LCNMSTRT_DCL;
%token UCNMSTRT_DCL;
%token LCNMCHAR_DCL;
%token UCNMCHAR_DCL;
%token NAMECASE_DCL;
%token GENERAL_DCL;
%token ENTITY_DCL;
%token NO_DCL;
%token YES_DCL;
%token NONE_DCL;
%token DELIM_DCL;
%token NAMES_DCL;
%token QUANTITY_DCL;

%token ATTCNT_DCL;
%token ATTSPLEN_DCL;
%token BSEQLEN_DCL;
%token DTAGLEN_DCL;
%token DTEMPLEN_DCL;
%token ENTLVL_DCL;
%token GRPCNT_DCL;
%token GRPGTCNT_DCL;
%token GRPLVL_DCL;
%token LITLEN_DCL;
%token NAMELEN_DCL;
%token NORMSEP_DCL;
%token PILEN_DCL;
%token TAGLEN_DCL;
%token TAGLVL_DCL;

%token FEATURES_DCL;
%token MINIMIZE_DCL;
%token DATATAG_DCL;
%token OMITTAG_DCL;
%token RANK_DCL;
%token SHORTTAG_DCL;
%token LINK_DCL;
%token SIMPLE_DCL;
%token IMPLICIT_DCL;
%token EXPLICIT_DCL;
%token OTHER_DCL;
%token CONCUR_DCL;
%token SUBDOC_DCL;
%token FORMAL_DCL;
%token APPINFO_DCL;
%token SHORTREF_DCL;

%token CDATA_DCL;
%token SDATA_DCL;
%token NDATA_DCL;
%token PI_DCL;
%token RCDATA_DCL;
%token EMPTY_DCL;
%token PCDATA_DCL;
%token ENTITIES_DCL;
%token ID_DCL;
%token IDREF_DCL;
%token IDREFS_DCL;
%token NAME_DCL;
%token NMTOKEN_DCL;
%token NMTOKENS_DCL;
%token NUMBER_DCL;
%token NUMBERS_DCL;
%token NUTOKEN_DCL;
%token NUTOKENS_DCL;
%token RNI_IMPLIED_DCL;
%token RNI_FIXED_DCL;
%token RNI_REQUIRED_DCL;
%token RNI_CURRENT_DCL;
%token RNI_CONREF_DCL;
%token DEFAULT_DCL;
%token RNI_RESTORE_DCL;
%token RNI_INITIAL_DCL;
%token MDO_USEMAP_DCL;
%token SYSTEM_DCL;
%token PUBLIC_DCL;
%token MDO_NOTATION_DCL;
%token NOTATION_DCL;
%token STARTTAG_DCL;
%token ENDTAG_DCL;
%token MS_DCL;
%token MD_DCL;
%token TEMP_DCL;
%token IGNORE_DCL;
%token END_INCLUDE;
%token INCLUDE_DCL;
%token ANY;
%token ENTITY_END;
/* %token DTD_ENTITY_END; */

/* catalog type declarations */
%token CATALOG_PUBLIC; 
%token CATALOG_ENTITY; 
%token CATALOG_DOCTYPE;
%token CATALOG_LINKTYPE;
%token CATALOG_NOTATION;
%token CATALOG_OVERRIDE;
%token CATALOG_SYSTEM;
%token CATALOG_SGMLDECL;
%token CATALOG_DOCUMENT;
%token CATALOG_CATALOG;
%token CATALOG_BASE;
%token CATALOG_DELEGATE;
%token CATALOG_NAME;
%token CATALOG_EOF;

%prefix sgml_ ;

%start sgml_dtd_parse, SGML_document_entity ;
%start sgml_doc_parse, document_instance_set;
%start sgml_cat_parse, SGML_catalog;

SGML_document_entity:
        [ sgml_declaration | implied_sgml_declaration | XML_declaration]
	prolog 
	/* document_instance_set -- all docs should be read separately */
	ENTITY_END ;

XML_declaration:
	XML_DECL {
	  last_sgml_declaration = current_sgml_declaration;
	  current_sgml_declaration = &XML_Declaration; 
        }
	[VERSIONINFO
	TOKEN_SEP* 
	VI { 
#ifdef DEBUGDOC
		printf(" = "); 
#endif
		}
	TOKEN_SEP* 
	parameter_literal
	 {current_sgml_declaration->version_id = sgml_yylval.string;
#ifdef DEBUGDOC
	 printf("XML version: %s\n",current_sgml_declaration->version_id);
#endif
	 }
	]?
	encodingdecl? { }
	sddecl? { }
	XML_ENDDECL;

encodingdecl: [ENCODINGDECL
	       
	       TOKEN_SEP* 
	       VI { 
#ifdef DEBUGDOC
		    printf(" = "); 
#endif
	          }
		TOKEN_SEP* 
		parameter_literal
	        {
             if (current_sgml_document != 0)
                current_sgml_document->encoding = sgml_yylval.string;
#ifdef DEBUGDOC
	 printf("XML Encoding: %s\n",current_sgml_document->encoding);
#endif
	 }
      ] ;


sddecl: [SDDECL
	       
	       TOKEN_SEP* 
	       VI { 
#ifdef DEBUGDOC
		    printf(" = "); 
#endif
	          }
		TOKEN_SEP* 
		parameter_literal
	        { if (strcasecmp(sgml_yylval.string,"yes") == 0)
		     current_sgml_document->standalone = 1;
#ifdef DEBUGDOC
	 printf("XML standalone: %d\n",current_sgml_document->standalone);
#endif
	 }
      ] ;



sgml_declaration: 
	ps_plus
	MDO_SGML {
#ifdef DEBUGSGML
	  printf ("SGML Declaration started\n"); 
#endif
	  last_sgml_declaration = current_sgml_declaration;

	  if ((current_sgml_declaration = CALLOC(SGML_Declaration,1)) 
	      == NULL) {
	    SGML_ERROR(ALLOC_ERROR,
		       "Unable to allocate SGML_declaration","","","");
	  }
	}
	ps_plus
	minimum_literal { if (sgml_yylval.string) FREE(sgml_yylval.string);}
	ps_plus
	document_character_set
	ps_plus
	capacity_set
	ps_plus
	concrete_syntax_scope
	ps_plus
	concrete_syntax 
	/* already have trailing ps_plus */
	feature_use
	ps_plus
	application_specific_information
	ps_plus
	MDC  {
#ifdef DEBUGSGML
	  printf("End of SGML Declaration\n"); 
#endif
	 } ;


document_character_set: 
	CHARSET_DCL
	ps_plus
	character_set_description ;

character_set_description: 
	base_character_set
	ps_plus
	described_char_set_portion
	ps_plus
	[ base_character_set
	  ps_plus
	  described_char_set_portion
	  ps_plus
	]* ;

base_character_set:
	BASESET_DCL
	ps_plus
	public_identifier { if (sgml_yylval.string) FREE(sgml_yylval.string);} ;

	
described_char_set_portion:
	DESCSET_DCL
	ps_plus
	[ 
	  character_description
	  ps_plus
  	]+ ;

character_description:
	described_set_char_number
	ps_plus
	number_of_chars
	ps_plus
	[ base_set_char_number 
	| minimum_literal { if (sgml_yylval.string) FREE(sgml_yylval.string);}
	| UNUSED_DCL
	] ;

described_set_char_number:
	character_number ;


base_set_char_number:
	character_number ;

character_number: NUMBER ;

number_of_chars: NUMBER  ;


capacity_set: 
	CAPACITY_DCL
	ps_plus
	[
		[ PUBLIC_DCL
		 ps_plus
		 public_identifier
		]
		|
		[
		 SGMLREF_DCL
			ps_plus
		 	[
			  [ TOTALCAP_DCL ps_plus number 
				{ TOTALCAP = sgml_yylval.intval; }  ps_plus ]
			| [ ENTCAP_DCL ps_plus number 
				{ ENTCAP = sgml_yylval.intval; }  ps_plus ]
			| [ ENTCHCAP_DCL ps_plus number 
				{ ENTCHCAP = sgml_yylval.intval; }  ps_plus ]
			| [ ELEMCAP_DCL ps_plus number 
				{ ELEMCAP = sgml_yylval.intval; }  ps_plus ]
			| [ GRPCAP_DCL ps_plus number 
				{ GRPCAP = sgml_yylval.intval; }  ps_plus ]
			| [ EXGRPCAP_DCL ps_plus number 
				{ EXGRPCAP = sgml_yylval.intval; }  ps_plus ]
			| [ EXNMCAP_DCL ps_plus number 
				{ EXNMCAP = sgml_yylval.intval; }  ps_plus ]
			| [ ATTCAP_DCL ps_plus number 
				{ ATTCAP = sgml_yylval.intval; }  ps_plus ]
			| [ ATTCHCAP_DCL ps_plus number 
				{ ATTCHCAP = sgml_yylval.intval; }  ps_plus ]
			| [ AVGRPCAP_DCL ps_plus number 
				{ AVGRPCAP = sgml_yylval.intval; }  ps_plus ]
			| [ NOTCAP_DCL ps_plus number 
				{ NOTCAP = sgml_yylval.intval; }  ps_plus ]
			| [ NOTCHCAP_DCL ps_plus number 
				{ NOTCHCAP = sgml_yylval.intval; }  ps_plus ]
			| [ IDCAP_DCL ps_plus number 
				{ IDCAP = sgml_yylval.intval; }  ps_plus ]
			| [ IDREFCAP_DCL ps_plus number 
				{ IDREFCAP = sgml_yylval.intval; }  ps_plus ]
			| [ MAPCAP_DCL ps_plus number 
				{ MAPCAP = sgml_yylval.intval; }  ps_plus ]
			| [ LKSETCAP_DCL ps_plus number 
				{ LKSETCAP = sgml_yylval.intval; }  ps_plus ]
			| [ LKNMCAP_DCL ps_plus number 
				{ LKNMCAP = sgml_yylval.intval; }  ps_plus ]
			]+
		]
	]	;

concrete_syntax_scope: 
	SCOPE_DCL
	ps_plus
	[DOCUMENT_DCL
	| INSTANCE_DCL
	] ;

concrete_syntax:
	SYNTAX_DCL
	ps_plus
	[
		shunned_char_number_identification
		ps_plus
		syntax_reference_char_set 
		ps_plus
		function_char_identification
		ps_plus
		naming_rules 
		ps_plus
		delimiter_set
		ps_plus
		reserved_name_use
		ps_plus
		quantity_set
	   ]
	   | public_concrete_syntax ;

public_concrete_syntax: 
	PUBLIC_DCL
	ps_plus
	public_identifier
	ps_plus
	[
	  SWITCHES_DCL
	  ps_plus
	  [ 
	    character_number
	    ps_plus
	    character_number
	    ps_plus
	  ]+
	]? ;

shunned_char_number_identification: 
	SHUNCHAR_DCL
	ps_plus
	[ NONE_DCL
	| [ 
	    [ CONTROLS_DCL 
	    | character_number
	    ]
	    ps_plus
	    [
		character_number
	        ps_plus
	    ]*
	  ]
	] ;

syntax_reference_char_set: 
	character_set_description ;

function_char_identification:
	FUNCTION_DCL
	ps_plus
	RE_DCL
	ps_plus
	character_number 
	ps_plus
	RS_DCL
	ps_plus
	character_number 
	ps_plus
	SPACE_DCL
	ps_plus
	character_number 
	ps_plus
	[ 
	  added_function
	  ps_plus
	  function_class
	  ps_plus
	  character_number
	  ps_plus
	]*  ;

added_function: SGMLNAME {if (sgml_yylval.string) FREE(sgml_yylval.string);} ;

function_class:
	 FUNCHAR_DCL 
	| MSICHAR_DCL
	| MSOCHAR_DCL
	| MSSCHAR_DCL
	| SEPCHAR_DCL ;

naming_rules: 
	NAMING_DCL
	ps_plus
	LCNMSTRT_DCL
	ps_plus [ parameter_literal { if (sgml_yylval.string) FREE(sgml_yylval.string);}
		ps_plus]+
	UCNMSTRT_DCL
	ps_plus [ parameter_literal  { if (sgml_yylval.string) FREE(sgml_yylval.string);}
		ps_plus]+
	LCNMCHAR_DCL
	ps_plus [ parameter_literal  { if (sgml_yylval.string) FREE(sgml_yylval.string);}
		ps_plus]+
	UCNMCHAR_DCL
	ps_plus [ parameter_literal  { if (sgml_yylval.string) FREE(sgml_yylval.string);}
		ps_plus]+
	NAMECASE_DCL
	ps_plus
	GENERAL_DCL
		ps_plus 
		[ NO_DCL 
		| YES_DCL
		]
	ps_plus 
	ENTITY_DCL
		ps_plus 
		[ NO_DCL 
		| YES_DCL
		] ;

delimiter_set: 
	DELIM_DCL
	ps_plus
	general_delimiters
	ps_plus
	short_reference_delimiters ;

general_delimiters:
	GENERAL_DCL
	ps_plus
	SGMLREF_DCL
	ps_plus
		[ name
		  ps_plus
		  parameter_literal {/** may need to fix, but... */  
				     if (sgml_yylval.string) FREE(sgml_yylval.string); 
				    }
		  ps_plus
		]* ;

short_reference_delimiters:
	SHORTREF_DCL
	ps_plus
	[ SGMLREF_DCL { current_sgml_declaration->shortref = YES; }
	| NONE_DCL { current_sgml_declaration->shortref = NO; }
	]
	ps_plus
	[ parameter_literal {/** may need to fix, but... */  
				     if (sgml_yylval.string) FREE(sgml_yylval.string); 
				    }
	  ps_plus
	]* ;

reserved_name_use: 
	NAMES_DCL
	ps_plus
	SGMLREF_DCL
	ps_plus
		[ 
		name
		ps_plus
		parameter_literal {/** may need to fix, but... */  
				     if (sgml_yylval.string) FREE(sgml_yylval.string); 
				    }
		ps_plus
		]* ;

quantity_set: 
	QUANTITY_DCL
	ps_plus
	SGMLREF_DCL
	ps_plus
		[ 
		  [ ATTCNT_DCL ps_plus number 
				{ ATTCNT = sgml_yylval.intval; }  ps_plus ] 
		| [ ATTSPLEN_DCL ps_plus number 
				{ ATTSPLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ BSEQLEN_DCL ps_plus number 
				{ BSEQLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ DTAGLEN_DCL ps_plus number 
				{ DTAGLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ DTEMPLEN_DCL ps_plus number 
				{ DTEMPLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ ENTLVL_DCL ps_plus number 
				{ ENTLVL = sgml_yylval.intval; }  ps_plus ] 
		| [ GRPCNT_DCL ps_plus number 
				{ GRPCNT = sgml_yylval.intval; }  ps_plus ]
		| [ GRPGTCNT_DCL ps_plus number 
				{ GRPGTCNT = sgml_yylval.intval; }  ps_plus ] 
		| [ GRPLVL_DCL ps_plus number 
				{ GRPLVL = sgml_yylval.intval; }  ps_plus ] 
		| [ LITLEN_DCL ps_plus number 
				{ LITLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ NAMELEN_DCL ps_plus number 
				{ NAMELEN = sgml_yylval.intval; }  ps_plus ] 
		| [ NORMSEP_DCL ps_plus number 
				{ NORMSEP = sgml_yylval.intval; }  ps_plus ] 
		| [ PILEN_DCL ps_plus number 
				{ PILEN = sgml_yylval.intval; }  ps_plus ] 
		| [ TAGLEN_DCL ps_plus number 
				{ TAGLEN = sgml_yylval.intval; }  ps_plus ] 
		| [ TAGLVL_DCL ps_plus number 
				{ TAGLVL = sgml_yylval.intval; }  ps_plus ] 
		]* ;


feature_use: 
	FEATURES_DCL
	ps_plus
	markup_minimization_features
	ps_plus
	link_type_features
	ps_plus
	other_features ;

markup_minimization_features:
	MINIMIZE_DCL
	ps_plus
	DATATAG_DCL
		ps_plus 
		[ NO_DCL  { current_sgml_declaration->datatag = NO; }
		| YES_DCL { current_sgml_declaration->datatag = YES; }
		] 
	ps_plus
	OMITTAG_DCL
		ps_plus 
		[ NO_DCL  { current_sgml_declaration->omittag = NO; }
		| YES_DCL { current_sgml_declaration->omittag = YES; }
		] 
	ps_plus
	RANK_DCL
		ps_plus 
		[ NO_DCL  { current_sgml_declaration->rank = NO; }
		| YES_DCL { current_sgml_declaration->rank = YES; }
		] 
	ps_plus
	SHORTTAG_DCL
		ps_plus 
		[ NO_DCL  { current_sgml_declaration->shorttag = NO; }
		| YES_DCL { current_sgml_declaration->shorttag = YES; }
		]  ;

link_type_features:
	LINK_DCL
	ps_plus
	SIMPLE_DCL
		ps_plus 
		[ NO_DCL 
		| [ YES_DCL ps_plus number ]
		] 
	ps_plus
	IMPLICIT_DCL
		ps_plus 
		[ NO_DCL 
		| [ YES_DCL ps_plus number ]
		] 
	ps_plus
	EXPLICIT_DCL
		ps_plus 
		[ NO_DCL
		| [ YES_DCL ps_plus number ]
		] ;

other_features: 
	OTHER_DCL
	ps_plus
	CONCUR_DCL
		ps_plus 
		[ NO_DCL  { current_sgml_declaration->concur = NO; }
		| [ YES_DCL ps_plus number  
			{ current_sgml_declaration->concur = sgml_yylval.intval; }]
		]
	ps_plus 
	SUBDOC_DCL
		ps_plus 
		[ NO_DCL { current_sgml_declaration->subdoc = NO; }
		| [ YES_DCL ps_plus number 
			{ current_sgml_declaration->subdoc = sgml_yylval.intval; }]
		]
	ps_plus
	FORMAL_DCL 
	ps_plus 
		[ NO_DCL { current_sgml_declaration->formal = NO; }
		| YES_DCL { current_sgml_declaration->formal = YES; }
		] ;


application_specific_information: 
	APPINFO_DCL
	ps_plus
	[ NONE_DCL
	| minimum_literal { if (sgml_yylval.string) FREE(sgml_yylval.string); }
	] ;

implied_sgml_declaration:
	MDO_DOCTYPE { /* this DTD is missing an explicit sgml decl */
#ifdef DEBUGDTD
         printf("implied declaration\n");
#endif
		orig_sgml_in = sgml_in;
		/* Try to find an SGMLDECL entry in the catalog*/
		if (Current_SGML_Catalog) {
			Tcl_HashEntry *entry; 

		  	entry = Tcl_FindHashEntry(
				Current_SGML_Catalog,
				"SGMLDECL");

			if (entry != NULL){
			   temp_SGML_Catalog_Entry =
				(SGML_Catalog *) Tcl_GetHashValue(entry);
			   new_sgml_in = fopen(
					temp_SGML_Catalog_Entry->system_name,
					"r");
#ifdef DEBUGDTD
         printf("Using SGML declaration from catalog\n");
#endif

			}
		}
		
		if (new_sgml_in == NULL) { 
			/* check for "default_sgml_dcl" in current dir */
			new_sgml_in = fopen("default_sgml_dcl","r");
		}

		if (new_sgml_in == NULL) {
			fprintf(stderr,"No XML declaration or SGML declaration in catalog or in 'default_sgml_dcl' (required for SGML DTD parsing -- exiting\n");
			fprintf(LOGFILE,"No XML declaration or SGML declaration in catalog or in 'default_sgml_dcl' (required for SGML DTD parsing -- exiting\n");
			cheshire_exit(100);
		}
		/* Otherwise we have an sgml declaration... */
		sgml_restart(new_sgml_in);

	} 
	sgml_declaration {
		fclose(new_sgml_in);
		new_sgml_in = NULL;
		fclose(orig_sgml_in);
		orig_sgml_in = NULL;
		sgml_in = fopen(sgml_in_name,"r");
		sgml_restart(sgml_in);} ;

prolog: other_prolog*
	base_document_type_declaration
	other_prolog*
	[ document_type_declaration 
	  other_prolog
	]* ;



/* link_type_declaration: { printf("LINKS ARE NOT IMPLEMENTED\n"); } 
	JUNK ;
*/

other_prolog: comment_dcl | processing_instruction | TOKEN_SEP ;

doc_prolog: comment_dcl | processing_instruction | TOKEN_SEP
	| in_doc_document_type_declaration ;

base_document_type_declaration: document_type_declaration ;


document_type_declaration: 
	MDO_DOCTYPE 
		{ 
#ifdef DEBUGDTD
 printf("DOCTYPE-start\n"); 
#endif
		  last_sgml_dtd = current_sgml_dtd;
		  if ((current_sgml_dtd = CALLOC(SGML_DTD,1)) == NULL) {
		     SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_DTD struct","","","");
		     
	  	  }

		  if (last_sgml_dtd == NULL) /* first DTD */
			DTD_list = current_sgml_dtd;
		  else last_sgml_dtd->next_dtd = current_sgml_dtd;

		  current_sgml_dtd->SGML_Element_hash_tab = 
			CALLOC(Tcl_HashTable,1);

		  Tcl_InitHashTable(current_sgml_dtd->SGML_Element_hash_tab,
			TCL_STRING_KEYS);

		  current_sgml_dtd->SGML_Entity_hash_tab = 
			CALLOC(Tcl_HashTable,1);

		  Tcl_InitHashTable(current_sgml_dtd->SGML_Entity_hash_tab,
			TCL_STRING_KEYS);
		  
		  /* If there is one, the catalog will have been created */
		  /* as Current_SGML_Catalog;                            */
		  current_sgml_dtd->SGML_Catalog_hash_tab =
			Current_SGML_Catalog;

		  current_sgml_dtd->DTD_file_name = sgml_in_name;

		  expect_document = 0; 
		  /* re-initialize for a new dtd */
		  current_sgml_element = NULL;

		  first_sub_element = current_sgml_sub_element = NULL;

		  current_sgml_entity = NULL;

		  current_sgml_attribute = NULL;

		  First_sgml_attribute = NULL;

		  current_sgml_notation = NULL;

		  first_tag = current_tag = NULL;

		  first_attr_value = current_attr_value = NULL;

		} 
      ps_plus 
      SGMLNAME  {
#ifdef DEBUGDTD
       if (sgml_yylval.string)
	   printf("name = %s\n",sgml_yylval.string); 
#endif
		current_sgml_dtd->DTD_name = sgml_yylval.string;
		 current_doctype_name = sgml_yylval.string;
		}
      ps_plus 
      [%default 
	[ DSO 
          dtd_body 
          DSC {
#ifdef DEBUGDTD
 printf("DSC - end of dtd def\n"); 
#endif
		}
	  ps_star
        ]
       |
        [external_identifier 
		{ 
#ifdef DEBUGDTD
 printf("external id\n"); 
#endif
                  if (system_id_token && system_id_name) {
			current_sgml_dtd->DTD_system_name = system_id_name;
				include_buffer_switch (system_id_name);
		  }
		  else if (system_id_token) {
			current_sgml_dtd->DTD_system_name = 
				current_doctype_name;
			include_buffer_switch (current_doctype_name);
		  }
		  else if (public_id_token && public_id_name) {
			current_sgml_dtd->DTD_public_name = public_id_name;
				include_buffer_switch (public_id_name);
		  }
		}
	 include_body
	 [ DSO 
           dtd_body 
           DSC {
#ifdef DEBUGDTD
 printf("DSC - end of dtd def\n"); 
#endif
		}
	   ps_star

          ]?
	]
      ]
      MDC
	{ 
	  SGML_Element *el;


	 
	  /* Print the DTD contents */
	  /* dump_dtd(current_sgml_dtd); */

	  for ( el = current_sgml_dtd->Elements; 
			el != NULL ; el = el->next_element) {
		/* printf("element - %s model %d\n", el->tag,el->data_model); */
	  	link_data_model(el->data_model);
		if (el->data_type == EL_PCDATA && el->data_model != NULL) {
			/* if there is more than just a PCDATA subelement */
			/* then the content model is MIXED -- not PCDATA  */
			if (el->data_model->next_sub_element != NULL ||
			    el->data_model->group != NULL)
				el->data_type = EL_MIXED;
	        }
	  }
	
	  attach_attributes(First_sgml_attribute);


	} ;


in_doc_document_type_declaration: 
	MDO_DOCTYPE 
		{ 
#ifdef DEBUGDOC
 printf("In- DOCTYPE-start\n"); 
 
#endif
		  in_doc_type_decl = 1;
	  	  }
      ps_plus 
      SGMLNAME  {
		 current_doctype_name = sgml_yylval.string;
		 if (strcasecmp(current_doctype_name,
				current_sgml_dtd->DTD_name) != 0)
			fprintf(LOGFILE,"In-Doc doctype name not same as current DTD\n");
#ifdef DEBUGDOC
		else printf("name %s matches current DTD\n",sgml_yylval.string); 
#endif
		}
      ps_plus 
      [external_identifier { 
#ifdef DEBUGDOC
 printf("external id\n"); 
#endif
          if (system_id_token && system_id_name 
	      && current_sgml_dtd->DTD_system_name != NULL) {
	     if (strcasecmp(current_sgml_dtd->DTD_system_name, 
		system_id_name) != 0)
		fprintf(LOGFILE,"In-Doc system name not same as current DTD\n");
#ifdef DEBUGDOC
	     else printf("SYS name %s matches current DTD\n",system_id_name); 
#endif
				
	  }
	  else if (system_id_token && current_sgml_dtd->DTD_system_name != NULL) {
	     if (strcasecmp(current_sgml_dtd->DTD_system_name, 
		current_doctype_name) != 0)
		fprintf(LOGFILE,"In-Doc system name not same as current doctype name\n");
#ifdef DEBUGDOC
	     else printf("SYS name %s matches current DTD\n",current_doctype_name); 
#endif
	  }
	  else if (public_id_token && public_id_name 
		 && current_sgml_dtd->DTD_public_name != NULL) {
	    if (strcasecmp(current_sgml_dtd->DTD_public_name, 
		public_id_name) != 0)
		fprintf(LOGFILE,"In-Doc public name not same as current doctype name\n");
#ifdef DEBUGDOC
	    else printf("PUB name %s matches current DTD\n",public_id_name); 
#endif
	  }
	}
	]?
	ps_star
	[ DSO {
#ifdef DEBUGDOC
 printf("included DTD body -- DSO - start of dtd def\n"); 
#endif
	      }
          dtd_body 
          DSC {
#ifdef DEBUGDOC
 printf("included DTD body -- DSC - end of dtd def\n"); 
#endif
		}
	  ps_star
        ]?
     
      MDC {
#ifdef DEBUGDOC
 printf("included DTD body -- MDC - end of doctype def\n"); 
#endif
	start_document_proper();
	in_doc_type_decl = 0;

	} ;



 document_instance_set:  { document_tag_nesting = 0; 
			   force_document_end = 0; 
			   data_element_id = 0;
			   
			   if (Current_Doc_Namespaces == NULL) {
			       if ((Current_Doc_Namespaces = 
			            CALLOC(Tcl_HashTable,1)) == NULL) {
			           SGML_ERROR(ALLOC_ERROR,
				     "Unable to allocate Current_doc_namespace","","","");
			       }
			       else
		               Tcl_InitHashTable(Current_Doc_Namespaces,
			                         TCL_STRING_KEYS);
		           }

			 }
		doc_prolog* 
		base_document_element
		other_prolog* ;

 base_document_element: document_element ;

 /* assuming that docs will be completed -- we're skipping a bunch */
 /* of nasty complications by using only simple start and end tags */
 /* 
  * document_element: start_tag? 
  *		content
  *		end_tag? ;
  */


 document_element: start_tag	{ document_tag_nesting++ ;
#ifdef DEBUGDOC
				   printf( "tag nesting %d\n",
					document_tag_nesting);
#endif
				}

		   content
		   end_tag	{ document_tag_nesting--;
#ifdef DEBUGDOC
				  if (current_sgml_data 
				     && current_sgml_data->start_tag)
				     printf( "AT END %10s  nest %d\n",
					current_sgml_data->start_tag,
					document_tag_nesting);
#endif
				  if (document_tag_nesting == 0) {
				    force_document_end = 1;
#ifdef DEBUGDOC
				     printf( "Forcing doc end\n");
				     fflush(stdout);
#endif
				    /* make sure things are cleaned up */
				    current_sgml_data = NULL;
				    last_sgml_data = NULL;
				  }
				} ;


 start_tag: 
     [ STAGO 
     {  last_sgml_data = current_sgml_data;
        namespaceverified = 0;
     /* create a new data structure and link it up */
     if ((current_sgml_data = CALLOC(SGML_Data,1)) == NULL) {
       SGML_ERROR(ALLOC_ERROR,
		  "Unable to allocate SGML_Data","","","");
	  
     }
     current_sgml_data->data_element_id = data_element_id++;
     current_sgml_data->occurrence_no = 1; /* default to 1 */

     current_sgml_document->element_count = data_element_id - 1;

     if (current_sgml_document->data == NULL)
       current_sgml_document->data = current_sgml_data;
     
     /* This stores the OFFSET from the start of the current doc */
     current_sgml_data->start_tag_offset = parsed_file_chars - 1;
     }
     document_type_specification
     generic_identifier_specification 
     { Tcl_HashEntry *entry; 
     SGML_Element *el;
     
#ifdef DEBUGDOC
     if (sgml_yylval.string == NULL)
       printf("Start Tag: <(NULL)> :"); 
     else
       printf("Start Tag: <%s> :", sgml_yylval.string); 
     if (last_sgml_data == NULL) 
       printf("last_sgml_data is NULL!!!\n");
     else 
       printf("last_sgml_data is %s\n", last_sgml_data->element->tag);
     fflush(stdout);
#endif
     if (sgml_yylval.string == NULL){
       SGML_ERROR(PARSE_ERROR,
		  "Tag element NULL","","","");
       error_sgml_data = current_sgml_data;
       sgml_yylval.string = "NULL";
       entry = NULL;
       error_sgml_data->start_tag = sgml_yylval.string;
     }
     
     el = current_sgml_data->element = (SGML_Element *)
       find_ns_hash(sgml_yylval.string, 
		    current_sgml_dtd->SGML_Element_hash_tab);
     
#ifdef DEBUGDOC
     if (el != NULL)
       printf("element %s found in dtd\n", el->tag); 
     else
       printf("element not found in dtd\n", sgml_yylval.string); 
     fflush(stdout);
#endif
     /* for XML documents and schema docs... */	
     if (current_sgml_dtd->type == 2 || current_sgml_dtd->type == 3) {
       char *docnamespace = NULL;
       SGML_Element *anyel;
       
#ifdef DEBUGDOC
       printf("verifying namespaces (current_sgml_dtd->type = %i)\n",current_sgml_dtd->type); 
       fflush(stdout);
#endif    
       namespaceverified = verify_namespace(sgml_yylval.string, 
			    current_sgml_dtd->XML_Namespace_hash_tab,
			    Current_Doc_Namespaces, &docnamespace);
    
       if ((el == NULL && namespaceverified == 0)
           || (el == NULL && namespaceverified == 1 
	       && current_sgml_dtd->type == 3)
	   || (el != NULL 
	       && el->namespace != NULL
	       && docnamespace != NULL
	       && strcmp(docnamespace, el->namespace) != 0)){
         /* this is XML, so maybe ANY is being used */
           anyel = (SGML_Element *)
               find_ns_hash("##ANY##", 
                            current_sgml_dtd->SGML_Element_hash_tab);
           if (anyel != NULL && el != NULL) {
               el = current_sgml_data->element = 
                   xml_schema_add_any_el(current_sgml_dtd,sgml_yylval.string, 1);
               if (el)
                   el->namespace = docnamespace;
           }
           else if (anyel != NULL) {
               el = current_sgml_data->element = 
                   xml_schema_add_any_el(current_sgml_dtd,sgml_yylval.string, 0);
               if (el)
                   el->namespace = docnamespace;
           }
       }     
     }

     if (el == NULL ) {
       SGML_ERROR(UNDEFINED_TAG_ERROR,
		  "Tag element '",
		  sgml_yylval.string, "' not defined", "");
       /* SHould free it, but it may not be real */
       if (sgml_yylval.string) FREE(sgml_yylval.string);			
       error_sgml_data = current_sgml_data;
       error_sgml_data->start_tag = sgml_yylval.string;
     }
     else {
       
       if (active_sgml_entity) 
	 current_sgml_data->local_entities = active_sgml_entity;
       
       current_sgml_data->start_tag = sgml_yylval.string;
       /* temporarily */
       
       /* figure out the place for this element in the structure */
       if (last_sgml_data != NULL 
	   && element_is_child(el, last_sgml_data->element)) {
#ifdef DEBUGDOC 
	 if (last_sgml_data->element->tag != NULL) 
	   printf("element is child of %s\n", last_sgml_data->element->tag); 
	 else if (last_sgml_data->element->tag_list)
	   printf("element is child of (tag_list first element): %s \n", last_sgml_data->element->tag_list->tag); 
	 else 
	   printf("element identified as child but NO TAG!!!!\n");

	 fflush(stdout);
#endif

	 
	 if (last_sgml_data->element->tag) {
	   
	   if ((el->tag && strcasecmp(el->tag, 
				      last_sgml_data->element->tag) == 0
		&& last_sgml_data->end_tag != NULL )
	       || last_sgml_data->end_tag != NULL) {
	     /* same tag repeating (and closed) -- 
		or closed tag on same level
		must be sibs */
#ifdef DEBUGDOC
	     if (last_sgml_data->element->tag != NULL) {
	       printf("Probable sibs %s\n", last_sgml_data->element->tag);
	       printf("last_sgml_data->end_tag is %d\n", last_sgml_data->end_tag);
	     }
	     else if (last_sgml_data->element->tag_list)
	       printf("Probable sibs (tag_list first element): %s \n", last_sgml_data->element->tag_list->tag); 
	     fflush(stdout);
#endif
	     if (last_sgml_data)
	       last_sgml_data->next_data_element 
		 = current_sgml_data;
	     current_sgml_data->parent = 
	       last_sgml_data->parent;
	   }
	   else {

#ifdef DEBUGDOC 
	     printf("setting parent to last_sgml_data\n");
#endif
	     current_sgml_data->parent = last_sgml_data;
	     if (last_sgml_data->sub_data_element == NULL)
	       last_sgml_data->sub_data_element = 
		 current_sgml_data;
	   }
	   
	 }
	 else {
	   /* last_sgml_data->element is NULL */
	   /* so it must be a list of tags    */
	   SGML_Tag_List *t, *o;
	   char *tag;
	   if (el->tag) {				
	     for (t = last_sgml_data->element->tag_list; 
		  t ; t = t->next_tag) {
	       
	       if (strcasecmp(el->tag, t->tag) == 0
		   || last_sgml_data->end_tag != NULL) {
		 /* same tag repeating -- 
		    or closed tag on same level
		    must be sibs */
#ifdef DEBUGDOC
		 printf("Probable sibs (in taglist) %s\n", t->tag); 
		 fflush(stdout);
#endif
		 if (last_sgml_data)
		   last_sgml_data->next_data_element 
		     = current_sgml_data;
		 current_sgml_data->parent = 
		   last_sgml_data->parent;
	       }
	       
	     } /* for */
	   } /* el->tag */
	   else {
	     /* have to compare a tag_list to another */
	     /* tag_list                              */
	     for (t = last_sgml_data->element->tag_list; 
		  t ; t = t->next_tag) {
	       
	       for (o = el->tag_list; o ; o = o->next_tag) {
		 if (strcasecmp(o->tag, t->tag) == 0
		     || last_sgml_data->end_tag != NULL) {
		   /* same tag repeating -- 
		      or closed tag on same level
		      must be sibs */
#ifdef DEBUGDOC
		   printf("Probable sibs %s\n", t->tag); 
		   fflush(stdout);
#endif
		   if (last_sgml_data)
		     last_sgml_data->next_data_element 
		       = current_sgml_data;
		   current_sgml_data->parent = 
		     last_sgml_data->parent;
		 }
	       } /* inner for */
	       
	     } /* for */
	     
	   } /* else two tag lists */
	   
	   if (current_sgml_data->parent != 
	       last_sgml_data->parent) {
	     current_sgml_data->parent = 
	       last_sgml_data;
	     if (last_sgml_data->sub_data_element == NULL)
	       last_sgml_data->sub_data_element = 
		 current_sgml_data;
	   }
	   
	   
	 } /* else */
       }		
       else if (last_sgml_data != NULL
		&& last_sgml_data->end_tag != NULL) {
	 /* last tag is closed, must be siblings... (KLUDGE) */
#ifdef DEBUGDOC
	 if (last_sgml_data->element->tag != NULL) 
	   printf("element is sibling of %s\n", last_sgml_data->element->tag); 
	 else if (last_sgml_data->element->tag_list)
	   printf("element is sibling (tag_list first element): %s \n", last_sgml_data->element->tag_list->tag); 
	 if (last_sgml_data->parent == NULL)
	   printf("parent of last sibling is NULL\n");
	 else
	   printf("Parent for siblings is %s\n",last_sgml_data->parent->element->tag);
	   
	 fflush(stdout);
#endif
	 if (last_sgml_data)
	   last_sgml_data->next_data_element 
	     = current_sgml_data;
	 current_sgml_data->parent = 
	   last_sgml_data->parent;
       }	
    
       else if (last_sgml_data != NULL 
		&& current_sgml_dtd->type == 2 
		&& current_sgml_data->element->data_type == XML_ANY) {
       
	 if (last_sgml_data->end_tag == NULL
	     && last_sgml_data->sub_data_element == NULL) {
	   last_sgml_data->sub_data_element = current_sgml_data;
	   current_sgml_data->parent = last_sgml_data;
	 } 
	 else if (last_sgml_data->end_tag != NULL) {
	   /* more siblings */
	   last_sgml_data->next_data_element = current_sgml_data;
	   current_sgml_data->parent = last_sgml_data->parent;
	  
	 }
       }
     }
     }
     [%while (1) TOKEN_SEP]*
     attribute_specification_list 
     {
       current_sgml_data->attributes = first_attr_data;
       first_attr_data = NULL;
       current_attr_data = NULL;
     }
     TOKEN_SEP*
     [ TAGC {

#ifdef DEBUGDOC
       printf("start_tag end %d\n", parsed_file_chars);
       fflush(stdout);
#endif
       if (current_sgml_data->parent == NULL) {
	 if (last_sgml_data != NULL) {
	   /* if parent wasn't set SET IT NOW */
	   if (last_sgml_data->end_tag != NULL) {
	     /* if it is closed this is a sibling */
	     last_sgml_data->next_data_element = current_sgml_data;
	     current_sgml_data->parent = last_sgml_data->parent;
	   }
	   else {
	     /* otherwise we assume that the last data is a parent */
	     current_sgml_data->parent = last_sgml_data;
	   }
	 }
       }
#ifdef DEBUGDOC
	 if (current_sgml_data->parent == NULL)
	   printf("parent is STILL SET TO NULL after last ditch (top of tree?)\n");
	 else
	   printf("Parent is %s\n",current_sgml_data->parent->element->tag);
       fflush(stdout);
#endif

       current_sgml_data->content_start_offset = parsed_file_chars ;
       /* special handling for elements declared "EMPTY" */
       if (current_sgml_data->element->data_type == EL_EMPTY) {
	 /* set the end tag same as the start tag */
	 current_sgml_data->end_tag = 
	   strdup(current_sgml_data->start_tag);
	 /* set the end tag offset the same as the content start */
	 current_sgml_data->end_tag_offset = 
	   current_sgml_data->content_start_offset;
	 /* set the content end same as the content start */
	 current_sgml_data->content_end_offset =
	   current_sgml_data->content_start_offset;
	 current_sgml_data->data_end_offset =
	   current_sgml_data->content_start_offset;
	 /* force the "empty_end_tag" to be returned on the */
	 /* next call to the lexical scanner                */
	 /* set tag nesting up one... */
	 force_empty_end_tag();
	 
       }  
     }
     | EMPTYTAGC { /* this is for the XML empty tag convention /> */
       /* set the end tag same as the start tag */
#ifdef DEBUGDOC
       printf("Parser: GOT AN XML EMPTY TAG CLOSE!!!!! for %s : content_start %d\n", current_sgml_data->start_tag, current_sgml_data->content_start_offset );
#endif
       current_sgml_data->end_tag = 
	 strdup(current_sgml_data->start_tag);
       /* set the end tag offset the same as the content start */
       if (current_sgml_data->content_start_offset == 0) {
          /* get it from the scannner */
	  current_sgml_data->content_start_offset = parsed_file_chars;
       }
       current_sgml_data->end_tag_offset = 
	 current_sgml_data->content_start_offset;
       /* set the content end same as the content start */
       current_sgml_data->content_end_offset =
	 current_sgml_data->content_start_offset;
       /* force the "empty_end_tag" to be returned on the */
       /* next call to the lexical scanner                */
       force_empty_end_tag();
       
     } ]
     ] 
     | empty_start_tag ;

 end_tag: 
	[ ETAGO {
#ifdef DEBUGDOC
		 printf("end tag start %d\n", parsed_file_chars); 
	         fflush(stdout);
#endif
		}
	document_type_specification
	generic_identifier_specification
		{ 
#ifdef DEBUGDOC
  if (sgml_yylval.string != NULL)
      printf("</%s",sgml_yylval.string);
  else
      printf("sgml_yylval.string is NULL in endtag name??\n");

  printf("> current open tag: %s end_offset %d content_start %d\n",current_sgml_data->start_tag,
		  current_sgml_data->end_tag_offset, 
                  current_sgml_data->content_start_offset);

#endif

		
			if (sgml_yylval.string != NULL && strcasecmp(sgml_yylval.string, 
			    current_sgml_data->start_tag) == 0
			    && (current_sgml_data->end_tag_offset == 0
			       ||  current_sgml_data->end_tag_offset == 
			   current_sgml_data->content_start_offset /*EMPTY*/) ) {
				current_sgml_data->end_tag = sgml_yylval.string;					
			}
			else {
#ifdef DEBUGDOC
  printf("\nchecking parents current_sgml_data = %d",current_sgml_data);
  fflush(stdout);
#endif


			 for (current_sgml_data = current_sgml_data->parent;
			      current_sgml_data != NULL; 
			      current_sgml_data = current_sgml_data->parent) {


#ifdef DEBUGDOC
printf("\ncurrent_sgml_data = %d tag %s\n",current_sgml_data, current_sgml_data->start_tag);
  fflush(stdout);
#endif
				 if (sgml_yylval.string != NULL && strcasecmp(sgml_yylval.string, 
				     current_sgml_data->start_tag) == 0 ) {
				   
				   if (current_sgml_data->end_tag_offset == 0) {
				     current_sgml_data->end_tag = 
				       sgml_yylval.string;
				     
				     break;
				   }
				   else { /* tag already closed */
				     continue;
				   }
				}
			    }
			}

			if (current_sgml_data == NULL) {
			   /* have to do a full search of the doc */
			   current_sgml_data = 
				tag_match_doc(current_sgml_document->data, 
					      sgml_yylval.string);
			   
			   if (current_sgml_data != NULL) {
			      current_sgml_data->end_tag = sgml_yylval.string;
			   }
			   else {
			     SGML_ERROR(UNDEFINED_TAG_ERROR,
				   "End Tag element '",
				   sgml_yylval.string, "' not defined","");

			   }

			}
	
			/* now that we have the right element, set the offsets */
			current_sgml_data->end_tag_offset = 
			     current_sgml_data->content_end_offset =
		 	     parsed_file_chars - (strlen (sgml_yylval.string) + 2);

			if (current_sgml_data->local_entities != NULL
			    && current_sgml_data->parent != NULL
			    && current_sgml_data->parent->local_entities == NULL) {
				active_sgml_entity = NULL;
#ifdef DEBUGDOC
printf(" active_sgml_entity set to NULL... ");
#endif
			}

		}
	TOKEN_SEP*
	TAGC { 
#ifdef DEBUGDOC
printf("> matched\n");
  /* printf("end tag end %d\n", parsed_file_chars); */
#endif
		  current_sgml_data->data_end_offset = parsed_file_chars;
		  } 
	]
	| [EMPTY_END_TAG {
#ifdef DEBUGDOC
		     printf( "Empty end_tag\n");
		     fflush(stdout);
#endif

	          /* ensure that parents are attached */
		  if (current_sgml_data != NULL 
		      && current_sgml_data->parent == NULL) {
		     if (last_sgml_data->end_tag == NULL) {
		        current_sgml_data->parent == last_sgml_data;
		     }
		     else /* siblings... */
		        current_sgml_data->parent == last_sgml_data->parent;
		  }
	  }
	  ] ;

empty_start_tag: EMPTY_START_TAG;

/* empty_end_tag: EMPTY_END_TAG; */

/**** the following is how it is REALLY supposed to be defined
      BUT this application mandates that all SGML be in a "completed"
      form with both begin and end tags for each element and all references
      and such expanded.

 start_tag: 
	%if (SHORTTAG == 1) minimized_start_tag 
 	| [ STAGO
	  document_type_specification
	  generic_identifier_specification
	  attribute_specification_list
	  TOKEN_SEP*
	  TAGC
	] ;

 end_tag: 
	%if (SHORTTAG == 1) minimized_end_tag 
	| [ ETAGO
	document_type_specification
	generic_identifier_specification
	TOKEN_SEP*
	TAGC] ;

minimized_start_tag: empty_start_tag 
	| %if (lookahead_tag == 1) unclosed_start_tag 
	| net_enabling_start_tag ;

minimized_end_tag: empty_end_tag 
	| unclosed_end_tag 
	| null_end_tag ;

empty_start_tag: EMPTY_START_TAG;

empty_end_tag: EMPTY_END_TAG;

unclosed_start_tag: STAGO 
	document_type_specification
	generic_identifier_specification
	attribute_specification_list ;

unclosed_end_tag: ETAGO 
	document_type_specification
	generic_identifier_specification ;

null_end_tag: NET ;

net_enabling_start_tag: STAGO 
	document_type_specification
	generic_identifier_specification
	attribute_specification_list 
	NET ;
****/

/* for concurrent doc types */
document_type_specification: name_group? ;
		

content: EMPTY_TAG_CONTENTS
	| %if (current_sgml_data != NULL && (current_sgml_data->element->data_type == EL_MIXED 
		|| current_sgml_data->element->data_type == CONTENT_MODEL
		|| current_sgml_data->element->data_type == EL_PCDATA)) 
		mixed_content
	| 	replaceable_character_data {
#ifdef DEBUGDOC
			printf("RCDATA--%s--\n",sgml_yylval.string); 
#endif
			if (sgml_yylval.string) FREE(sgml_yylval.string);
			}
	| 	character_data 
	| 	doc_element_content /* content model only -- no data */;


mixed_content:
	[ data_character {
#ifdef DEBUGDOC

  		printf("mixed content PCDATA = '%s' buffer chars %d\n",
			sgml_yylval.string, parsed_file_chars); 
#endif
		if (sgml_yylval.string) FREE(sgml_yylval.string);
		sgml_yylval.string = NULL;
		}
	| other_content 
	| document_element
	| end_include  {
#ifdef DEBUGDOC
  		printf("mixed content: END_INCLUDE\n"); 
#endif
		}
	]*  ;

doc_element_content: 
	[ document_element  {
#ifdef DEBUGDOC
		printf ("doc_element_content-doc_element\n");
#endif
			}
	| other_content
	| TOKEN_SEP
	| end_include  {
#ifdef DEBUGDOC
  		printf("doc_element_content: END_INCLUDE\n"); 
#endif
		} 
	]*  ;

other_content: 
	  comment_dcl
	| processing_instruction
	| short_ref_use_dcl
	| link_set_use_declaration
/*	| shortref */
	| character_reference 
	| general_entity_reference 
	| marked_section_dcl
	| ENTITY_END ;


link_set_use_declaration: 
	MDO_USELINK
	ps_plus
	link_set_specification
	ps_plus
	link_type_name
	ps_plus
	MDC  
	  { SGML_ERROR(PARSE_ERROR,
		"link set use not supported yet <!USELINK...","","","");
	  };

link_set_specification:
	link_set_name
	| [RNI EMPTY_DCL]
	| RNI_RESTORE_DCL ;

link_set_name: 
	name
	| RNI_INITIAL_DCL ;

link_type_name: name ;

/* not sure what this needs
shortref:  ;
*/

character_reference: 
	CRO
	[ function_name
	| character_number 
	]
	REFC ;

function_name:
	  RE_DCL
	| RS_DCL
	| SPACE_DCL
	| name ;

replaceable_character_data: RCDATA_DATA ;

character_data: CDATA_DATA ;

data_character: PCDATA_DATA | SGMLCHAR ;



dtd_body: [%default ds | dtd_dcl_subset]* ;

include_body: [%default ds | dtd_dcl_subset]+ {
#ifdef DEBUGDTD
 printf("include body\n"); 
#endif
               /* put end-include into ds END_INCLUDE */ } ;

 
 dtd_dcl_subset: [element_set | entity_set | shortref_set ] ;

 entity_set: entity_dcl;

 entity_dcl: MDO_ENTITY 
	{ 	
#ifdef DEBUGDTD
 printf("ENTITY def start\n"); 
#endif
		last_sgml_entity = current_sgml_entity;
		if ((current_sgml_entity = CALLOC(SGML_Entity,1)) 
			== NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Entity","","","");
	  	}
		if (last_sgml_entity == NULL) /* first for DTD */
			current_sgml_dtd->Entities = current_sgml_entity;
		else last_sgml_entity->next_entity = current_sgml_entity;
	}
	    ps_plus 
            entity_name 
                 {/* name assignments to structure are handled elsewhere */
		  if (sgml_yylval.string) {
                     default_entity = 0;
		  }
		  else {
#ifdef DEBUGDTD
 printf("external entity def\n"); 
#endif
			default_entity = 1;
			/* include_buffer_switch ("CATALOG"); */
                       }
		 }
            ps_plus
	    entity_text 
            ps_star MDC  
                 { 
#ifdef DEBUGDTD
 printf("ENTITY def end\n"); 
#endif 
		 } ;

 element_set: element_dcl | attrlist_set | notation_set ;

 element_dcl: 
	MDO_ELEMENT 
		{ 
		  last_sgml_element = current_sgml_element;
		  if ((current_sgml_element = CALLOC(SGML_Element,1))
			 == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Element","","","");
		   
	  	  }

		  if (last_sgml_element == NULL) /* first for DTD */
			current_sgml_dtd->Elements = current_sgml_element;
		  else last_sgml_element->next_element = current_sgml_element;
		}
	ps_plus { expect_otm = 0;}
	element_type 
		{ Tcl_HashEntry *entry; 
		  int exists;
		  SGML_Tag_List *t;

		  /* add the element name(s) to the hash table */
		  if (current_sgml_element->tag) { 
			/* single name for element */
 		     char *namepart;
	             namepart = strchr(current_sgml_element->tag, ':');
	             if (namepart == NULL)
	                 namepart = current_sgml_element->tag;
	             else
                         namepart++;


#ifdef DEBUGDTD
 printf("ELEMENT name = '%s'\n",sgml_yylval.string); 
#endif
			entry =	Tcl_CreateHashEntry(
				current_sgml_dtd->SGML_Element_hash_tab,
				namepart, &exists
				);
			if (entry != NULL)
				Tcl_SetHashValue(entry, 
					(ClientData)current_sgml_element);
			else if (exists)
			     SGML_ERROR(INFORM_ERROR,
				"ELEMENT NAME",	current_sgml_element->tag,
				 "ALREADY EXISTS IN HASH TABLE!","");
			}
		else for (t = current_sgml_element->tag_list; t ; 
				t = t->next_tag) {
 		        char *namepart;
	                namepart = strchr(t->tag, ':');
	                if (namepart == NULL)
	                    namepart = t->tag;
	                else
                             namepart++;

			entry =	Tcl_CreateHashEntry(
				current_sgml_dtd->SGML_Element_hash_tab,
				namepart, &exists
				);
			if (entry != NULL)
				Tcl_SetHashValue(entry, 
					(ClientData)current_sgml_element);
			else if (exists)
			     SGML_ERROR(INFORM_ERROR,
				"ELEMENT NAME", t->tag,
				 "EXISTS IN HASH TABLE!","");
			}

		  content_model_flag = 0;
		  expect_otm = 1;
		}
	ps_plus 
	otm?  { expect_otm = 0; 
#ifdef DEBUGDTD
		printf("OTM matched???\n");
#endif          
              }
	element_content 
	ps_star 
	MDC { 
#ifdef DEBUGDTD
 printf("ELEMENTSET\n"); 
#endif
		} ;

 attrlist_set: 
	MDO_ATTLIST 
		{ 
		  last_sgml_attribute = current_sgml_attribute;
		  if ((current_sgml_attribute = CALLOC(SGML_Attribute,1))
			== NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Attribute","","","");
		   
	  	  }
		  if (First_sgml_attribute == NULL) {
			First_sgml_attribute = current_sgml_attribute;
			
		  }
		  else {
			if (last_sgml_attribute) 
			   last_sgml_attribute->next_attribute =
				current_sgml_attribute;
		  }
		}
		ps_plus 
		attlist_dcl {
		  /* attach to appropriate element */
		  /* this is now done for all attributes at the end of */
                  /* DTD parsing                                       */
		  /* attach_attributes(current_sgml_attribute); */
                }
		
		ps_plus 
		attlist_def
		ps_star 
		MDC 
			{ 
#ifdef DEBUGDTD
 printf("ATTLIST\n"); 
#endif
			  FREE(current_sgml_attribute);
			  last_sgml_attribute->next_attribute = NULL;
			  current_sgml_attribute = last_sgml_attribute;

			}  ;

 notation_set: MDO_NOTATION_DCL  {
		  last_sgml_notation = current_sgml_notation;
		  if ((current_sgml_notation = CALLOC(SGML_Notation,1))
		      == NULL) {
		    SGML_ERROR(ALLOC_ERROR,
			       "Unable to allocate SGML_Notation","","","");
		    
		  }

		  if (last_sgml_notation == NULL) /* first for DTD */
		    current_sgml_dtd->Notation = current_sgml_notation;
		  else last_sgml_notation->next_notation = current_sgml_notation;
		  
                }
                ps_star
		notation_name 
                    {
		      current_sgml_notation->name = sgml_yylval.string;
#ifdef DEBUGDTD	
		      printf("notation name = %s\n",sgml_yylval.string); 
#endif
		    }
		ps_star
		notation_id
 		    { if (system_id_token)
		      current_sgml_notation->type = SYSTEM_IDENT; 
		    if (public_id_token)
		      current_sgml_notation->type += PUBLIC_IDENT; 
		    
		    if (public_id_name) {
		      current_sgml_notation->public_name = public_id_name;
		      if (current_sgml_dtd->SGML_Catalog_hash_tab) {
			Tcl_HashEntry *entry; 
			
		  	entry = Tcl_FindHashEntry(
						  current_sgml_dtd->SGML_Catalog_hash_tab,
						  public_id_name);

			if (entry != NULL){
			  temp_SGML_Catalog_Entry =
			    (SGML_Catalog *) Tcl_GetHashValue(entry);
			  current_sgml_notation->system_name =	
			    strdup(temp_SGML_Catalog_Entry->system_name);
			}
		      }
		    }
		    else if (system_id_name)
		      current_sgml_notation->system_name = system_id_name;
		} 

		ps_star
		MDC
                  { 
#ifdef DEBUGDTD
		    printf("NOTATION declaration\n"); 
#endif
		  }  ;

 shortref_set: short_ref_mapping_dcl | short_ref_use_dcl ;

 short_ref_mapping_dcl: MDO_SHORTREF ps_plus 
               map_name 
                   {
#ifdef DEBUGDTD
		     printf("SHORTREF name = %s\n",sgml_yylval.string); 
#endif
		   }
               ps_plus
               map_set+ MDC { 
#ifdef DEBUGDTD
		 printf("SHORTREF DEFINED\n"); 
#endif
	       };

short_ref_use_dcl: MDO_USEMAP_DCL ps_plus 
               map_spec  
                  {
#ifdef DEBUGDTD
		    printf("USEMAP spec name = %s\n",sgml_yylval.string); 
#endif
		  }
	       ps_plus
               usemap_assoc_element_type? ps_star MDC  
                   { 
#ifdef DEBUGDTD
		     printf("USEMAP declaration\n"); 
#endif			
		   }  ;

 usemap_assoc_element_type: name | name_group ;


 entity_name: parameter_entity_name | general_entity_name ;

 general_entity_name: 
    name 
      { Tcl_HashEntry *entry; 
      int exists;

		
#ifdef DEBUGDTD
      printf("general entity name = %s\n",sgml_yylval.string); 
#endif
      current_sgml_entity->name = sgml_yylval.string;
      
      entry =	Tcl_CreateHashEntry(
				    current_sgml_dtd->SGML_Entity_hash_tab,
				    sgml_yylval.string, &exists
				    );
      if (entry != NULL)
	Tcl_SetHashValue(entry, 
			 (ClientData)current_sgml_entity);
      else if (exists)
	SGML_ERROR(INFORM_ERROR,
		   "GEN ENTITY NAME ALREADY IN HASH TABLE","","","");
      
      current_sgml_entity->type = GENERAL_ENTITY; 
      }

   | RNI DEFAULT_DCL 
      { 
#ifdef DEBUGDTD
	printf("#DEFAULT entity name\n"); 
#endif
	current_sgml_entity->name = NULL;
	current_sgml_entity->type = DEFAULT_ENTITY; 
	sgml_yylval.string = NULL; 
      } ;

 parameter_entity_name: 
	PERO 
	ps+
	name 
        { Tcl_HashEntry *entry; 
	int exists;

		
#ifdef DEBUGDTD
	printf("Parameter ENTITY name = '%s'\n",sgml_yylval.string); 
#endif
	current_sgml_entity->name = sgml_yylval.string;
	entry = Tcl_CreateHashEntry(
				    current_sgml_dtd->SGML_Entity_hash_tab,
				    sgml_yylval.string,&exists);
	if (entry != NULL)
	  Tcl_SetHashValue( entry, (ClientData)current_sgml_entity);
	else if (exists)
	  SGML_ERROR(INFORM_ERROR, 
		     "PARAM ENTITY NAME", sgml_yylval.string, 
		     "ALREADY EXISTS IN HASH TABLE.","");
	
	
	current_sgml_entity->type = PARAMETER_ENTITY; 
	} ;
	  

 entity_text: parameter_literal
		{ 
#ifdef DEBUGDOC
 printf("entity text PARAM LIT = '%s'\n",sgml_yylval.string); 
#endif
		  current_sgml_entity->entity_text_type = PARAMETER_LIT_TEXT;
		  current_sgml_entity->entity_text_string = 
				 SubEntities(sgml_yylval.string);
		}
          | data_text
		{ 
#ifdef DEBUGDOC
 printf("entity text DATA_TEXT = %s\n",sgml_yylval.string); 
#endif
                 current_sgml_entity->entity_text_type = DATA_TEXT;}
          | bracketed_text
		{ 
#ifdef DEBUGDOC
 printf("entity text BRACKETED_TEXT = %s\n",sgml_yylval.string); 
#endif
	          current_sgml_entity->entity_text_type = BRACKETED_TEXT;}
          | external_entity_spec 
		{ current_sgml_entity->entity_text_type = 
			EXTERNAL_ENTITY_SPEC; 
		};


 data_text: [CDATA_DCL 
		{ current_sgml_entity->entity_text_subtype = CDATA_TEXT;}
	| SDATA_DCL 
		{ current_sgml_entity->entity_text_subtype = SDATA_TEXT;}
	| PI_DCL
		{ current_sgml_entity->entity_text_subtype = PI_TEXT;}
	] 
	ps_plus 
	parameter_literal 
		{ current_sgml_entity->entity_text_string = sgml_yylval.string;} ;

 bracketed_text: 
 	[STARTTAG_DCL 
		{ current_sgml_entity->entity_text_subtype = STARTTAG_TEXT;}
	| ENDTAG_DCL 
		{ current_sgml_entity->entity_text_subtype = ENDTAG_TEXT;}
	| MS_DCL 
		{ current_sgml_entity->entity_text_subtype = MS_TEXT;}
	| MD_DCL
		{ current_sgml_entity->entity_text_subtype = MD_TEXT;}
	] 
	ps_star 
	parameter_literal
		{ char *temp;
			
                  temp = CALLOC(char, strlen(sgml_yylval.string)+8);
		  if (current_sgml_entity->entity_text_subtype == STARTTAG_TEXT)
			sprintf(temp,"<%s>", sgml_yylval.string);
		  else if (current_sgml_entity->entity_text_subtype == ENDTAG_TEXT)
			sprintf(temp,"</%s>", sgml_yylval.string);
		  else if (current_sgml_entity->entity_text_subtype == MS_TEXT)
			sprintf(temp,"<![%s]]>", sgml_yylval.string);
		  else if (current_sgml_entity->entity_text_subtype == MD_TEXT)
			sprintf(temp,"<!%s>", sgml_yylval.string);

		  current_sgml_entity->entity_text_string = temp;
		  FREE(sgml_yylval.string);
		} ;
 
 external_entity_spec: 
	external_identifier 
		{ if (system_id_token)
		  current_sgml_entity->entity_text_subtype = SYSTEM_IDENT; 
		  else if (public_id_token)
		  current_sgml_entity->entity_text_subtype = PUBLIC_IDENT; 

		  if (public_id_name) {
		     current_sgml_entity->public_name = public_id_name;
		     current_sgml_entity->system_name =	NULL;
		     if (current_sgml_dtd->SGML_Catalog_hash_tab) {
			Tcl_HashEntry *entry; 

		  	entry = Tcl_FindHashEntry(
				current_sgml_dtd->SGML_Catalog_hash_tab,
				public_id_name);

			if (entry != NULL){
			   temp_SGML_Catalog_Entry =
				(SGML_Catalog *) Tcl_GetHashValue(entry);
			  /* use the catalog system name first... */
			  current_sgml_entity->system_name =	
			  	 strdup(temp_SGML_Catalog_Entry->system_name);
			}
		     }
		     /* Public declaration with system included... */
		
		     if (current_sgml_entity->system_name == NULL 
			&& system_id_name != NULL) 
			current_sgml_entity->system_name = system_id_name;
		  }
		  else if (system_id_name)
		  current_sgml_entity->system_name = system_id_name;
		} 
	ps_plus 
	entity_type? ;

 entity_type: 
 	SUBDOC_DCL 
 	| 
 	[
 		[ CDATA_DCL 
 		| NDATA_DCL 
 		| SDATA_DCL
 		] ps_plus 
 	  notation_name 
 	  ps_plus 
 	  data_attribute_spec?
 	] {
#ifdef DEBUGDTD
 printf("entity_type declaration not used\n"); 
#endif
	} ;

 data_attribute_spec: DSO ts_star attribute_specification_list  DSC ;

 attribute_specification_list: attribute_specification* ;

 attribute_specification: 
           [name 
	       { SGML_Attribute *at;
		
#ifdef DEBUGDOC
	       printf("ATTRIBUTE: %s",sgml_yylval.string);
#endif
	       last_attr_data = current_attr_data;
	       /* create a new data structure and link it up */
	       if ((current_attr_data = CALLOC(SGML_Attribute_Data,1)) == NULL) {
		 SGML_ERROR(ALLOC_ERROR,
			    "Unable to allocate SGML_Attribute_Data",
			    "","","");
		 
	       }
	       if (last_attr_data == NULL)
		 first_attr_data = current_attr_data;
	       else
		 last_attr_data->next_attribute_data = current_attr_data;
		  	

	       if (sgml_yylval.string == NULL)	{	
		 SGML_ERROR(PARSE_ERROR,
			    "Problem with attributes in tag ",
			    current_sgml_data->start_tag,"","");
		   cheshire_exit(1);
	       }

	       /* match the attribute name */	
	       for (at = current_sgml_data->element->attribute_list; 
		    at && current_attr_data->attr_def == NULL ;
		    at = at->next_attribute) {

		 if (compare_nstags(at->name, sgml_yylval.string) == 0) {
		   current_attr_data->attr_def = at;
		   current_attr_data->name = sgml_yylval.string;
		 }
	       }
	       if (current_attr_data->attr_def == NULL) {
		 if (strncmp(sgml_yylval.string, "xmlns", 5) == 0) {
		    Namespace_Attribute_Flag = 1;
		    current_attr_data->name = sgml_yylval.string;
		 }
		 else if (compare_nstags(sgml_yylval.string, "schemaLocation") == 0) {
		    current_attr_data->name = sgml_yylval.string;
		 }
		 else if (current_sgml_data->element->data_type == XML_ANY) {
		     current_attr_data->name = sgml_yylval.string;
		 }
         else if (strcmp(sgml_yylval.string, "xsi:type") == 0) {
             /* xsi type is another of those basically worthless things that */
             /* crop up when least expected */
             current_attr_data->name = sgml_yylval.string;
         }
 		 else {
#ifdef DEBUGDOC
             printf("Current_data (%s) type is %d\n", 
		     current_sgml_data->element->tag,
		     current_sgml_data->element->data_type);
#endif
		     SGML_ERROR(PARSE_ERROR,"No attribute '", 
		                  sgml_yylval.string,
			          "' associated with tag ", 
			          current_sgml_data->start_tag);
	    }		   
      }
	}
	TOKEN_SEP* 
	VI { 
#ifdef DEBUGDOC
		printf(" = "); 
#endif
		}
	TOKEN_SEP* 
	]
	attribute_value_specification 
		{ 
#ifdef DEBUGDOC
 		printf("\"%s\" ",sgml_yylval.string);
#endif
		current_attr_data->string_val = sgml_yylval.string;
		current_attr_data->int_val = sgml_yylval.intval;
		if (Namespace_Attribute_Flag) {
		    Add_Doc_Namespace(current_attr_data, 
                                      Current_Doc_Namespaces);
		    Namespace_Attribute_Flag = 0;

		}
	   }
	[%while (1) TOKEN_SEP]* ;

 ts: [%default TOKEN_SEP | parameter_entity_reference | end_include ] ;

 end_include: END_INCLUDE {
#ifdef DEBUGDTD
	 printf("End of Entity Include\n"); 
#endif

		active_sgml_entity = NULL;
	} ;
 
 ts_star: [%while(1) ts]*;


 ps: [%default TOKEN_SEP | parameter_entity_reference | comment | end_include ] ;
 
 ps_plus: [%while(1) ps ]*;
 ps_star: [%while(1) ps ]*;


 ds: [ TOKEN_SEP 
   | parameter_entity_reference
   | comment_dcl
   | processing_instruction
   | marked_section_dcl 
   | end_include
   ] ;


 processing_instruction: [PIO instructions PIC] | XML_declaration {
#ifdef DEBUGDOC
	printf(" -- processing instruction or XML Declaration\n"); 
#endif
	} ;

 instructions: [cheshire_instruction {
#ifdef DEBUGDOC
   printf("End of Cheshire_instruction\n");
   printf("Current Processing Instruction_type %d\n",
	  current_pi->Instruction_Type);
   printf("  Instruction %d : Tag '%s' ",
	  current_pi->Instruction,
	  current_pi->tag);
   if (current_pi->attr) 
     printf(": Attribute '%s'",
	    current_pi->attr);

   if (current_pi->index_name) 
     printf(": For Index '%s'\n",
	    current_pi->attr);
   else
     printf("\n");
   fflush(stdout);

#endif
   last_pi = current_pi;

 } ]
 | [ INSTRUCTION  {
#ifdef DEBUGDOC
   printf(" in instructions\n");
   fflush(stdout);
   if (sgml_yylval.string != NULL)
      printf("INSTRUCTIONS = %s",sgml_yylval.string);  
   fflush(stdout);
#endif
   FREE(sgml_yylval.string);
 } ]  ;


cheshire_instruction: [CHESHIRE_INSTRUCTION {
#ifdef DEBUGDOC
  printf("Cheshire_instruction type = %d\n", 
	 sgml_yylval.intval);
  fflush(stdout);
#endif
  if ((current_pi = CALLOC(SGML_Processing_Inst,1)) 
      == NULL) {
    SGML_ERROR(ALLOC_ERROR,
	       "Unable to allocate SGML_Processing_Inst","","","");
		   
  }
  if (last_pi == NULL 
      && current_sgml_document->Processing_Inst == NULL) {

  /* printf("Should be adding Cheshire_instruction\n"); */
  /* fflush(stdout); */

    current_sgml_document->Processing_Inst = current_pi;
  }
  else {
    last_pi->next_processing_inst = current_pi;
  }
  current_pi->Instruction_Type = sgml_yylval.intval;
 } 
 INSTRUCTION_TYPE {

#ifdef DEBUGDOC
   printf("instruction = '%s'\n", sgml_yylval.string);
   printf("instruction num = %d\n", sgml_yylval.intval);
#endif
   current_pi->Instruction = sgml_yylval.intval;
 }
 ARGS {
#ifdef DEBUGDOC
   if (sgml_yylval.string)
     printf("ARGS = '%s'\n", sgml_yylval.string);
#endif

   if (current_pi->Instruction > PROC_DELETE_TAG) {
     /* index-specific PIs have at least two args */
     char *t;

     current_pi->index_name = sgml_yylval.string;

     /* get the tag */
     t = strchr(sgml_yylval.string, ' ');
     if (t) 
       *t++ = '\0';
     while (*t == ' ' || *t == '\t' || *t == '\n') t++;
     current_pi->tag = t;


     if (current_pi->Instruction == PROC_INDEX_SUBST_ATTR) {
       /* INDEX_SUBST_ATTR has three args */
       t = strchr(t, ' ');
       if (t) 
         *t++ = '\0';
       while (*t == ' ' || *t == '\t' || *t == '\n') t++;
       current_pi->attr = t;
     }
   } 
   else {  
     /* otherwise it is a global PI which have one or two args */
     current_pi->tag = sgml_yylval.string;

     if (current_pi->Instruction == PROC_SUBST_ATTR) {
       /* SUBST_ATTR has two args */
       char *t;
       t = strchr(sgml_yylval.string, ' ');
       if (t) 
         *t++ = '\0';
       while (*t == ' ' || *t == '\t' || *t == '\n') t++;
       current_pi->attr = t;
     }
   }
 }
];


 marked_section_dcl:	marked_section_start {
#ifdef DEBUGDOC
				 printf("marked section start\n"); 
#endif
                        }
			ps_plus 
			status_keyword_spec
			ps_plus DSO { 
#ifdef DEBUGDOC
				printf ("DSO start of marked data\n"); 
#endif
			}
			marked_section
			marked_section_end ;

 marked_section_start: MARKED_SECTION_START ; 
 
 marked_section_end: MARKED_SECTION_END {
#ifdef DEBUGDOC
   printf("marked section end\n"); 
#endif
   if (marked_section_type == INCLUDESECTION) {
     string_buffer_switch(marked_section_contents,1 /*nested*/);
   }
 } ;

marked_section: [QSTRING {
#ifdef DEBUGDOC
	printf("marked section CONTENTS:\n%s\n",sgml_yylval.string); 
#endif
	if (marked_section_type == INCLUDESECTION)
	  marked_section_contents = sgml_yylval.string;
	else {
	  marked_section_contents = NULL;
	  FREE(sgml_yylval.string);
	  sgml_yylval.string = NULL;
	}
  }]* ;

 status_keyword_spec: status_keyword 
			| TEMP_DCL {marked_section_type = IGNORESECTION;} ;

 status_keyword: CDATA_DCL {marked_section_type = IGNORESECTION;}
		| IGNORE_DCL {marked_section_type = IGNORESECTION;}
		| INCLUDE_DCL {marked_section_type = INCLUDESECTION;}
		| RCDATA_DCL {marked_section_type = INCLUDERCSECTION;} ;

 parameter_entity_reference: PERO_REF
 	{ Tcl_HashEntry *entry; 
	  SGML_Entity *ent;

	  
#ifdef DEBUGDTD
 printf("Parameter Entity Ref Name = '%s'\n", sgml_yylval.string); 
#endif
	  entry = Tcl_FindHashEntry(
			current_sgml_dtd->SGML_Entity_hash_tab,
			sgml_yylval.string);
	  if (entry == NULL){
		SGML_ERROR(PARSE_ERROR,"Entity '", 
			sgml_yylval.string,"' not defined");
		
	  }
	  else
		ent = (	SGML_Entity *) Tcl_GetHashValue(entry);
	  /* have an entity struct  link it to the current data */

#ifdef DEBUGDTD
 if ( ent != NULL) {
    if (ent->name != NULL)
       printf("Entity name %s type %d ", ent->name, ent->type);

 if (ent->entity_text_string != NULL)
       printf("string '%s' ", ent->entity_text_string);
 
 if (ent->public_name != NULL)
       printf("pub %s ", ent->public_name);
 
 if (ent->system_name != NULL)
       printf("sys %s", ent->system_name); 

 printf("\n");
 fflush(stdout);
 }
	  
#endif
	  /* switch input to the entity referenced */
	  if (sgml_yylval.string) FREE(sgml_yylval.string);

	  switch_input_to_entity (ent);

	} 
	 ;

 
 general_entity_reference: ERO_REF
 	{ Tcl_HashEntry *entry; 
	  SGML_Entity *ent;

	  
#ifdef DEBUGDOC
 printf("General Entity Ref Name = %s\n", sgml_yylval.string); 
#endif

	  if (sgml_yylval.hash_entry == NULL){
		SGML_ERROR(PARSE_ERROR,"Entity '",
			sgml_yylval.string,"' not defined","");
		
	  }
	  else
		ent = (	SGML_Entity *) Tcl_GetHashValue(sgml_yylval.hash_entry);
	  /* have an entity struct */
	  active_sgml_entity = ent;
	  
#ifdef DEBUGDOC
 if ( ent != NULL) {
    if (ent->name != NULL)
       printf("Entity name %s type %d ", ent->name, ent->type);

 if (ent->entity_text_string != NULL)
       printf("string '%s' ", ent->entity_text_string);
 
 if (ent->public_name != NULL)
       printf("pub %s ", ent->public_name);
 
 if (ent->system_name != NULL)
       printf("sys %s", ent->system_name); 

 printf("\n");
}
	  
#endif
#ifdef DEBUGDOC
	 printf("Start of Entity Include\n"); 
#endif

	  /* switch input to the entity referenced */
	  save_parsed_file_chars = parsed_file_chars;
	  switch_input_to_entity (ent); 

	} 
	 ;

 
 map_spec: map_name 
          | RNI EMPTY_DCL ;

 map_name: name ;

 map_set:  parameter_literal {
#ifdef DEBUGDTD
	     printf("map param = '%s'\n", sgml_yylval.string);
#endif
           if (sgml_yylval.string) FREE(sgml_yylval.string);
	}
           ps_plus 
           name {
#ifdef DEBUGDTD
 printf("map tag = '%s'\n", sgml_yylval.string); 
#endif
	}
           ps_plus ;
 
 notation_name: name ;

 notation_id: external_identifier ;

 attlist_dcl: assoc_element_type
            | assoc_notation_name;

 attlist_def: [
		attribute_def 
		{ /* allocate and link a new attribute struct    */
		  /* the final one will need to be "trimmed off" */
		  last_sgml_attribute = current_sgml_attribute;
		  if ((current_sgml_attribute = CALLOC(SGML_Attribute,1)) 
			== NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Attribute","","","");
		   
	  	  }

		  last_sgml_attribute->next_attribute = 
				current_sgml_attribute;

		  /* all attributes apply to the same tags */
		  current_sgml_attribute->assoc_tag = 
				last_sgml_attribute->assoc_tag;
		  current_sgml_attribute->assoc_tag_list = 
				last_sgml_attribute->assoc_tag_list;
		}
	 
  
	      ]* ;

 assoc_element_type: 
	name { 	current_sgml_attribute->assoc_tag = sgml_yylval.string;
		
#ifdef DEBUGDTD
 printf("attribute name %s\n", sgml_yylval.string); 
#endif
		} 
	| 
	name_group {current_sgml_attribute->assoc_tag_list = first_tag;} ;
 
 assoc_notation_name: RNI notation ;

 attribute_def:
	attribute_name {expect_decl_value = 1; }
	ps_plus 
	declared_value {expect_decl_value = 0;}
	ps_plus 
	default_value
	ps_plus
             {
#ifdef DEBUGDTD
 printf("ATTRIBUTE DEF\n"); 
 fflush(stdout);
#endif
		} ;

 attribute_name: name  
			{
#ifdef DEBUGDTD
 printf("attribute name '%s'\n",sgml_yylval.string); 
#endif
			 current_sgml_attribute->name = sgml_yylval.string;
			} ;

 declared_value: CDATA_DCL
		{current_sgml_attribute->declared_value_type = CDATA_ATTR;} 
                | ENTITY_DCL
		{current_sgml_attribute->declared_value_type = ENTITY_ATTR;} 
                | ENTITIES_DCL 
		{current_sgml_attribute->declared_value_type = ENTITIES_ATTR;} 
                | ID_DCL
		{current_sgml_attribute->declared_value_type = ID_ATTR;} 
                | IDREF_DCL
		{current_sgml_attribute->declared_value_type = IDREF_ATTR;} 
                | IDREFS_DCL   
		{current_sgml_attribute->declared_value_type = IDREFS_ATTR;} 
                | NAME_DCL   
		{current_sgml_attribute->declared_value_type = NAME_ATTR;} 
                | NAMES_DCL   
		{current_sgml_attribute->declared_value_type = NAMES_ATTR;} 
                | NMTOKEN_DCL   
		{current_sgml_attribute->declared_value_type = NMTOKEN_ATTR;} 
                | NMTOKENS_DCL   
		{current_sgml_attribute->declared_value_type = NMTOKENS_ATTR;} 
                | NUMBER_DCL   
		{current_sgml_attribute->declared_value_type = NUMBER_ATTR;} 
                | NUMBERS_DCL   
		{current_sgml_attribute->declared_value_type = NUMBERS_ATTR;} 
                | NUTOKEN_DCL   
		{current_sgml_attribute->declared_value_type = NUTOKEN_ATTR;} 
                | NUTOKENS_DCL   
		{current_sgml_attribute->declared_value_type = NUTOKENS_ATTR;} 
                | notation
		{current_sgml_attribute->declared_value_type = NOTATION_ATTR;} 
                | name_token_group 
		{current_sgml_attribute->declared_value_type = 
			NAME_TOKEN_GROUP_ATTR; 
		 current_sgml_attribute->value_list = first_attr_value; } ;
 

 default_value: 
		[ RNI_FIXED_DCL 
		  {current_sgml_attribute->default_value_type = FIXED_DEFAULT;}
		  ps_plus
		]? 
                attribute_value_specification   
		  {current_sgml_attribute->default_value_type = 
			SPECIFIED_DEFAULT;
		   current_sgml_attribute->default_value = sgml_yylval.string;
   
#ifdef DEBUGDTD
 printf("attr default value: '%s'\n",sgml_yylval.string); 
 fflush(stdout);
#endif
			} 

                | RNI_IMPLIED_DCL
		  {current_sgml_attribute->default_value_type = 
			IMPLIED_DEFAULT;
#ifdef DEBUGDTD
 printf("#IMPLIED value \n"); 
 fflush(stdout);
#endif
			} 
                | RNI_REQUIRED_DCL
		  {current_sgml_attribute->default_value_type = 
			REQUIRED_DEFAULT;
#ifdef DEBUGDTD
 printf("#REQUIRED value \n"); 
 fflush(stdout);
#endif

			}
                | RNI_CURRENT_DCL 
		  {current_sgml_attribute->default_value_type =
			 CURRENT_DEFAULT;
#ifdef DEBUGDTD
 printf("#CURRENT value \n"); 
 fflush(stdout);
#endif
			}
                | RNI_CONREF_DCL
		  {current_sgml_attribute->default_value_type = 
			CONREF_DEFAULT;
#ifdef DEBUGDTD
 printf("#CONREF value \n"); 
 fflush(stdout);
#endif
			} ;


/* this is not as the standard specs it */
 attribute_value_specification:  attribute_value | attribute_value_literal ;
 
 attribute_value_literal: ATTR_VAL_LITERAL 
		{
#ifdef DEBUGDTD
 printf("attribute value literal"); 
#endif
		} ;

 attribute_value: 
	SGMLNAME 
		{
#ifdef DEBUGDTD
 printf("SGMLNAME attr value: '%s'\n",sgml_yylval.string); 
#endif
		}
	| NAME_TOKEN 
		{
#ifdef DEBUGDTD
 printf("SGMLNAME attr value: '%s'\n",sgml_yylval.string); 
#endif
		}
	| number 
		{
#ifdef DEBUGDTD
 printf("number attr value: '%s'\n",sgml_yylval.string); 
#endif
		}
	| NAME_LIST
		{
#ifdef DEBUGDTD
 printf("attr value name list: '%s'\n",sgml_yylval.string); 
#endif
		}
	| NUMBER_LIST
		{
#ifdef DEBUGDTD
 printf("attr value number list: '%s'\n",sgml_yylval.string); 
#endif 
		} ;

 notation: NOTATION_DCL ps_plus name_group;

 name_token_group: GRPO ts_star name_token_list GRPC ;


 name_token_list: 
	name_token 
		{ 
#ifdef DEBUGDTD
 printf("name_token_list token = '%s'\n",sgml_yylval.string); 
#endif
		  if ((current_attr_value = CALLOC(SGML_Attribute_Value,1)) 
			== NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Attribute_Value","","","");
		   
	  	  }
		  first_attr_value = current_attr_value; 
		  current_attr_value->value = sgml_yylval.string;
		}
	ts_star 
	[ connector 
                { 
#ifdef DEBUGDTD
 printf("name_token_list connector = '%s'\n",sgml_yylval.string); 
#endif
		  current_attr_value->connector = sgml_yylval.string;
		} 
	ts_star 
	name_token 
                { 
#ifdef DEBUGDTD
 printf("name_token_list token = '%s'\n",sgml_yylval.string); 
#endif
		  last_attr_value = current_attr_value;
		  if ((current_attr_value = CALLOC(SGML_Attribute_Value,1)) 
			== NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Attribute_Value","","","");
		   
	  	  }
		  last_attr_value->next_value = current_attr_value;
		  current_attr_value->value = sgml_yylval.string;
		} 
	ts_star
	]* { 
#ifdef DEBUGDTD
 printf(" NAME_TOKEN_LIST\n"); 
#endif
		 } ; 

 name_token: SGMLNAME | NAME_TOKEN | NUMBER ;

 
 comment_dcl: [COMDCLS junk* COMDCLE] | NULLMDO {
#ifdef DEBUGDTD
 printf(" COMMENT -- NULL MDO???\n"); 
#endif
 } ;
    
/* null_comment: NULLMDO {printf("matched NULLMDO");} ; */

 comment: COM junk* COM ;
				 
				 
 external_identifier: 
	[SYSTEM_DCL 
		{ 
		  system_id_token = 1; 
		  public_id_token = 0;
		  public_id_name = NULL;
		  system_id_name = NULL;
		} 
	| [PUBLIC_DCL 
		{ 
		  public_id_token = 1;
		  system_id_token = 0;
		  public_id_name = NULL;
		  system_id_name = NULL;
		  /* printf("\nPUBLIC ID: "); */
		} 
            ps_plus 
            public_identifier 
		{ 
		  public_id_name = sgml_yylval.string;
		  /* printf("%s :", public_id_name); */
		} 
	  ]
	] 
	ps_plus 
        [system_identifier 
		{ 
		  system_id_name = sgml_yylval.string;
		  /* printf("%s :", system_id_name); */
		} 
	]? 
                    ;

 public_identifier: minimum_literal ;

 system_identifier: parameter_literal ;

 minimum_literal: PARAMETER_LITERAL { 
#ifdef DEBUGDTD
 printf("minimum literal\n"); 
#endif
		} ;
                      

 element_content: declared_content | content_model ;

 declared_content: CDATA_DCL {current_sgml_element->data_type = EL_CDATA;}
                  | RCDATA_DCL {current_sgml_element->data_type = EL_RCDATA;}
                  | EMPTY_DCL {current_sgml_element->data_type = EL_EMPTY;} ;

 content_model: [model_group 
			{ 
			  last_sgml_sub_element = NULL;
			  current_sgml_sub_element = NULL;
			  first_sub_element = NULL;
			
			}
		| ANY   		
		  { 
		  if ((current_sgml_sub_element = 
			CALLOC(SGML_Sub_Element_List,1)) == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Sub_Element","","","");
		   
	  	  }

		  if (last_sgml_sub_element == NULL) /* this should always be */
			current_sgml_element->data_model = 
				current_sgml_sub_element;
		  else last_sgml_sub_element->next_sub_element = 
				current_sgml_sub_element;
		  current_sgml_sub_element->element_parent = 
				current_sgml_element;
		  current_sgml_sub_element->sub_element_type = EL_ANY;
		}

		] 
		ps_plus 
		[exceptions] 			
			{ last_sgml_sub_element = NULL;
			  current_sgml_sub_element = NULL;
			  first_sub_element = NULL;
			}
 ;

 model_group:  GRPO 
		{  /* first element in a new group */
		  group_nesting_level++;
		  last_sgml_sub_element = current_sgml_sub_element;
		  if ((current_sgml_sub_element = 
			CALLOC(SGML_Sub_Element_List,1)) == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Sub_Element","","","");
		   
	  	  }

		  if (last_sgml_sub_element == NULL) {
			/* no previous element means this is first */
			first_sub_element =
		  	current_sgml_element->data_model = 
				current_sgml_sub_element;
			current_sgml_sub_element->element_parent = 
				current_sgml_element;
		  }
		  else {last_sgml_sub_element->group = 
				current_sgml_sub_element;
			current_sgml_sub_element->group_parent = 
				last_sgml_sub_element;
			current_sgml_sub_element->element_parent = NULL;
		  }
		  current_sgml_sub_element->sub_element_type = EL_GROUP;
		}
		ts_star 
		content_token_list 
		GRPC 
			{ /* move up any nesting level */
			  group_nesting_level--;
			  if (current_sgml_sub_element->group_parent != NULL)
				 current_sgml_sub_element =
					current_sgml_sub_element->group_parent;
			}

		[occurrence 
			{ 
#ifdef DEBUGDTD
 printf(" rep %s\n",sgml_yylval.string); 
#endif
			  if (current_sgml_sub_element->element_parent != NULL
				&& group_nesting_level == 0)
				 current_sgml_element->model_rep = 
					sgml_yylval.string;
			  else if (group_nesting_level > 0) 
				current_sgml_sub_element->group_repetition = 
				sgml_yylval.string;}
		]? 
               ;


 content_token_list: 
	content_token 
	ts_star 
	[connector 
		{  /* if there is a connector must be more in the list */
		  current_sgml_sub_element->connector = sgml_yylval.string;
		  last_sgml_sub_element = current_sgml_sub_element;
		  if ((current_sgml_sub_element = 
			CALLOC(SGML_Sub_Element_List,1)) == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_Sub_Element","","","");
		   
	  	  }

		  last_sgml_sub_element->next_sub_element = 
				current_sgml_sub_element;
		  current_sgml_sub_element->group_parent = 
				last_sgml_sub_element->group_parent;
		  current_sgml_sub_element->element_parent = 
				last_sgml_sub_element->element_parent;
		 
		}
	ts_star 
	content_token 
	ts_star
	]* ;


 content_token: prim_content_token
               | model_group ;

 prim_content_token: RNI PCDATA_DCL {
#ifdef DEBUGDTD
 printf("primary content PCDATA\n"); 
#endif

		 if (current_sgml_element->data_type == CONTENT_MODEL
		      && content_model_flag == 1)
			  current_sgml_element->data_type = EL_MIXED;
		  else
		  	current_sgml_element->data_type = EL_PCDATA;

		  current_sgml_sub_element->sub_element_type = EL_PCDATA;}
               | element_token  { content_model_flag = 1; }
               | data_tag_group 
			{ SGML_ERROR(PARSE_ERROR, 
				"Data tag groups not implemented","","","");
		};

 element_token: name
                   { 
#ifdef DEBUGDTD
 printf("element token name = %s\n",sgml_yylval.string); 
#endif

			current_sgml_sub_element->element_name = sgml_yylval.string;
			current_sgml_sub_element->sub_element_type = EL_NAMED;
			}
                [occurrence 
			{current_sgml_sub_element->repetition = sgml_yylval.string;}
		]? 
                ;

 data_tag_group: DTGO ts_star name ts_star SEQ ts_star data_tag_pattern ts_star DTGC [occurrence]? ;

 data_tag_pattern: [data_tag_template | data_tag_template_group] ts_star SEQ ts_star data_tag_padding ;

 data_tag_template_group: GRPO ts_star data_tag_template_list ts_star GRPC ;

 data_tag_template_list: data_tag_template ts_star [ OR ts_star data_tag_template ts_star]* ;

 data_tag_template: parameter_literal  { if (sgml_yylval.string) FREE(sgml_yylval.string);} ;

 data_tag_padding: parameter_literal  { if (sgml_yylval.string) FREE(sgml_yylval.string);} ;

 parameter_literal: PARAMETER_LITERAL ;
 
 occurrence:  OPT {
#ifdef DEBUGDTD
 printf("optional occurance\n"); 
#endif
		}
            | PLUS {
#ifdef DEBUGDTD
 printf("Required and repeating occurance\n"); 
#endif
		}
            | REP {
#ifdef DEBUGDTD
 printf("zero or more repeating occurance\n"); 
#endif
		} ;

 exceptions: [exclusions ps_star]? [inclusions]?  ;


 exclusions: 	EXMINUS 
		name_group {
			current_sgml_element->exclusions = first_tag;
			first_tag = NULL;
			};


 inclusions: 	EXPLUS 
		name_group {
			current_sgml_element->inclusions = first_tag;
			first_tag = NULL;
			};

 element_type: 
	generic_id 
		{ 
#ifdef DEBUGDTD
 printf("element type = generic id\n"); 
#endif
		  current_sgml_element->tag = sgml_yylval.string;
		} 
		  
             | name_group 
		{ 
#ifdef DEBUGDTD
 printf("element type = name_group\n"); 
#endif
  		  current_sgml_element->tag = NULL; /* tag_list used instead */
		  current_sgml_element->tag_list = first_tag;
		  first_tag = NULL;
		}  ;
/*           | ranked_element { printf("element type = ranked_element\n"); } 
             | ranked_group { printf("element type = ranked_group\n"); } ;
*/

 generic_identifier_specification: generic_id ;

 generic_id: name ;

 name_group: GRPO ts_star name_list GRPC ;

/* ranked_element: rank_stem ts_star rank_suffix ; */

/* ranked_group: GRPO ts_star rank_stem [ts_star connector ts_star rank_stem]* ts_star GRPC ts_star rank_suffix ;  */

/* rank_stem: name ; */

/* rank_suffix:  NUMBER  {  printf("rank_suffix\n"); } ; */


 name_list:
	name 
		{ 
#ifdef DEBUGDTD
 printf("name_list token = '%s'\n",sgml_yylval.string); 
#endif
		  if ((current_tag = CALLOC(SGML_Tag_List,1)) == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_tag_list","","","");
		   
	  	  }

		  first_tag = current_tag; 
		  current_tag->tag = sgml_yylval.string;
		}
            ts_star 
            [ connector 
                { 
#ifdef DEBUGDTD
 printf("name_list connector = '%s'\n",sgml_yylval.string); 
#endif
		  current_tag->connector = sgml_yylval.string;
		} 
              ts_star 
              name 
                { 
#ifdef DEBUGDTD
 printf("name_list token = '%s'\n",sgml_yylval.string); 
#endif
		  last_tag = current_tag;
		  if ((current_tag = CALLOC(SGML_Tag_List,1)) == NULL) {
		   SGML_ERROR(ALLOC_ERROR,
			"Unable to allocate SGML_tag_list","","","");
		   
	  	  }

		  last_tag->next_tag = current_tag;
		  current_tag->tag = sgml_yylval.string;
		} 
              ts_star
            ]* { 
#ifdef DEBUGDTD
 printf(" NAME_LIST\n"); 
#endif
		 } ; 

 name: SGMLNAME ;

 otm: [ start_tag_min TOKEN_SEP* end_tag_min ps_plus ] ;

 start_tag_min: OTM 	{ 

		    if (sgml_yylval.string == NULL) {
		        SGML_ERROR(PARSE_ERROR,
		                 "DTD: Invalid tag minimization options for tag: ",current_sgml_element->tag,"","");
		        cheshire_exit(0);
		   }
#ifdef DEBUGDTD
 printf("start_OTM = '%s'\n",sgml_yylval.string); 
#endif
			if (sgml_yylval.string[0] == 'o' || sgml_yylval.string[0] == 'O')
			   current_sgml_element->start_tag_min = 1;
			else if (sgml_yylval.string[0] == '-')
			   current_sgml_element->start_tag_min = 0;
			if (sgml_yylval.string) FREE(sgml_yylval.string);
			sgml_yylval.string = NULL;
			} ;

 end_tag_min: OTM { 
		    if (sgml_yylval.string == NULL) {
		        SGML_ERROR(PARSE_ERROR,
		                 "DTD: Invalid tag minimization options for tag: ",current_sgml_element->tag,"","");
		        cheshire_exit(0);
		   }
#ifdef DEBUGDTD
 printf("end_OTM = '%s'\n",sgml_yylval.string); 
#endif

			if (sgml_yylval.string[0] == 'o' || sgml_yylval.string[0] == 'O')
			   current_sgml_element->end_tag_min = 1;
			else if (sgml_yylval.string[0] == '-')
			   current_sgml_element->end_tag_min = 0;
			if (sgml_yylval.string) FREE(sgml_yylval.string);
			sgml_yylval.string = NULL;
			} ;

 connector: AND | OR | SEQ ;

 junk: JUNK ;

 number: NUMBER; 

 /* mdo: MDO; not used - combined with other elements */


 /* read and store information from the SGML catalog */
SGML_catalog:	{
		  if ((Current_SGML_Catalog = 
			CALLOC(Tcl_HashTable,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Data","","","");
		  }
		  Tcl_InitHashTable(Current_SGML_Catalog,
			TCL_STRING_KEYS);
		}
		[ sgml_cat_PUBLIC 
		|sgml_cat_ENTITY 
		|sgml_cat_DOCTYPE
		|sgml_cat_LINKTYPE
		|sgml_cat_NOTATION
		|sgml_cat_OVERRIDE
		|sgml_cat_SYSTEM
		|sgml_cat_SGMLDECL
		|sgml_cat_DOCUMENT
		|sgml_cat_CATALOG
		|sgml_cat_BASE
		|sgml_cat_DELEGATE
		]* 
		CATALOG_EOF ;

sgml_cat_PUBLIC: CATALOG_PUBLIC {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "PUBLIC";
		  temp_SGML_Catalog_Entry->typeid = 1;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);
		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {

#ifdef DEBUGCAT
 printf("PUBLIC Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }
		} ; 

sgml_cat_ENTITY: CATALOG_ENTITY {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "ENTITY";
		  temp_SGML_Catalog_Entry->typeid = 2;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);
		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("ENTITY Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 


sgml_cat_DOCTYPE: CATALOG_DOCTYPE {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "DOCTYPE";
		  temp_SGML_Catalog_Entry->typeid = 3;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("DOCTYPE Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 



sgml_cat_LINKTYPE: CATALOG_LINKTYPE{
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "LINKTYPE";
		  temp_SGML_Catalog_Entry->typeid = 4;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("LINKTYPE Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_NOTATION: CATALOG_NOTATION {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "NOTATION";
		  temp_SGML_Catalog_Entry->typeid = 5;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("NOTATION Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }


		} ; 

sgml_cat_OVERRIDE: CATALOG_OVERRIDE {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "OVERRIDE";
		  temp_SGML_Catalog_Entry->typeid = 6;

		}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->Type);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("OVERRIDE Catalog entry: %s Pubname = %s SysName = NULL\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->Type, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_SYSTEM: CATALOG_SYSTEM {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "SYSTEM";
		  temp_SGML_Catalog_Entry->typeid = 7;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;

	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("SYSTEM Catalog entry: %s name1 = %s name1 = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_SGMLDECL: CATALOG_SGMLDECL {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "SGMLDECL";
		  temp_SGML_Catalog_Entry->typeid = 8;

		}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->Type);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("SGMLDECL Catalog entry: %s Pubname = (NULL) SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->system_name); 
#endif

			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->Type, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_DOCUMENT: CATALOG_DOCUMENT {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "DOCUMENT";
		  temp_SGML_Catalog_Entry->typeid = 9;

		}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;
	
			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->Type);

		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("DOCUMENT Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif

			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->Type, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_CATALOG: CATALOG_CATALOG {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "CATALOG";
		  temp_SGML_Catalog_Entry->typeid = 10;

		}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;
			static int catalog_count=0;
			
			temp_SGML_Catalog_Entry->public_name = 
				CALLOC(char,20);
			sprintf(temp_SGML_Catalog_Entry->public_name,
				"CATALOG_%d", catalog_count++);

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;

	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);


		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("ENTITY Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
	
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		} ; 

sgml_cat_BASE: CATALOG_BASE {
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "BASE";
		  temp_SGML_Catalog_Entry->typeid = 11;

		}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->Type);


		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("BASE Catalog entry: %s Pubname = NULL SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->Type, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }


		entry = Tcl_CreateHashEntry(
				Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->Type, &exists);
		if (entry != NULL)
			Tcl_SetHashValue( entry, 
				(ClientData)temp_SGML_Catalog_Entry);
		else if (exists)
			 SGML_ERROR(INFORM_ERROR, 
				"BASE NAME", sgml_yylval.string, 
				"ALREADY EXISTS IN CATALOG HASH TABLE.","");
				} ; 

sgml_cat_DELEGATE: CATALOG_DELEGATE{
		  if ((temp_SGML_Catalog_Entry = 
			CALLOC(SGML_Catalog,1)) == NULL) {
			SGML_ERROR(ALLOC_ERROR,
				"Unable to allocate SGML_Catalog_entry"
				,"","","");
		  }
		  temp_SGML_Catalog_Entry->Type = "DELEGATE";
		  temp_SGML_Catalog_Entry->typeid = 12;

		}
		CATALOG_NAME { 
			temp_SGML_Catalog_Entry->public_name = 
				sgml_yylval.string;
			}
		CATALOG_NAME {
			Tcl_HashEntry *entry; 
			int exists;

			temp_SGML_Catalog_Entry->system_name = 
				sgml_yylval.string;
	
	  	entry = Tcl_FindHashEntry(Current_SGML_Catalog,
				temp_SGML_Catalog_Entry->public_name);


		/* if it is a new entry create, otherwise Free */
		if (entry == NULL) {
#ifdef DEBUGCAT
 printf("DELEGATE Catalog entry: %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
#endif
			entry = Tcl_CreateHashEntry(
					Current_SGML_Catalog,
			                temp_SGML_Catalog_Entry->public_name, &exists);
			if (entry != NULL)
				  Tcl_SetHashValue( entry, 
				  (ClientData)temp_SGML_Catalog_Entry);
			else if (exists)
			          SGML_ERROR(INFORM_ERROR, 
				      "PUBLIC NAME", sgml_yylval.string, 
				      "ALREADY EXISTS IN CATALOG HASH TABLE.","");
		 }
		 else {
		      FREE(temp_SGML_Catalog_Entry->system_name);
		      FREE(temp_SGML_Catalog_Entry->public_name);
		      FREE(temp_SGML_Catalog_Entry);
		 }

		};

%lexical sgml_next_token ;

{  /* additional routines associated with the parser */

/* The following routines are for finding relationships between */
/* data elements as specified in a DTD. Some are recursive to   */
/* handle the nesting of elements.                              */


SGML_Sub_Element_List
*search_data_model(SGML_Sub_Element_List *mod, char *name)
{
   SGML_Sub_Element_List *sub=NULL;

   if (mod == NULL) 
	return (NULL);

   if (mod->element_name != NULL 
		&& strcasecmp(mod->element_name, name) == 0) 
	return (mod);

   if (mod->group)	
    	sub = search_data_model(mod->group, name);

   if (sub != NULL)
	return (sub);

   /* always search the next items */
   return(search_data_model(mod->next_sub_element,name));

}


int element_is_child(SGML_Element *element, SGML_Element *parent) 
{
  SGML_Sub_Element_List *sub = NULL;
  SGML_Tag_List *t=NULL;
  
  if (element == NULL || parent == NULL)
    return (0);
  
  /* empty elements have no children */
  if (parent->data_type == EL_EMPTY) 
    return (0);
  
  if (element->tag != NULL) {
     sub = search_data_model(parent->data_model, element->tag);
  }
  else {
     for (t = element->tag_list; t && sub == NULL; t = t->next_tag) {
	sub = search_data_model(parent->data_model, t->tag);
     }
  }
  
  if (sub != NULL) 
    return (1);
  else
    return (0);
}

void
link_data_model(SGML_Sub_Element_List *mod)
{
  Tcl_HashEntry *entry; 
  
  
  if (mod == NULL) return;
  
  if (mod->group) {
    link_data_model(mod->group);
  }
  /* must be an element, so find the actual element declaration */
  if (mod->element_name) {
    mod->element = (SGML_Element *)find_ns_hash(mod->element_name, 
				current_sgml_dtd->SGML_Element_hash_tab);
    if (mod->element == NULL){
      SGML_ERROR(PARSE_ERROR,
		 "Error in link_data_model -- tag element '",
		 mod->element_name,"' not defined","\n");
    }
  }
   
  link_data_model(mod->next_sub_element);
  
}


SGML_Tag_List 
*dup_taglist(SGML_Tag_List *in_tags) {

SGML_Tag_List *new_tags;

  if (in_tags == NULL)
     return NULL;

  if ((new_tags = CALLOC(SGML_Tag_List,1)) == NULL) {
   SGML_ERROR(ALLOC_ERROR,
		"Unable to duplicate SGML_tag_list","","","");
  }

  new_tags->next_tag = dup_taglist(in_tags->next_tag);
  if (in_tags->tag)
     new_tags->tag = strdup(in_tags->tag);
  if (in_tags->connector)  
     new_tags->connector = strdup(in_tags->connector);
  
  return (new_tags);

}

SGML_Attribute_Value
*dup_valuelist(SGML_Attribute_Value *in_vals) {

SGML_Attribute_Value *new;

 if (in_vals == NULL)
    return NULL;

 if ((new = CALLOC(SGML_Attribute_Value,1)) == NULL) {
   SGML_ERROR(ALLOC_ERROR,
		"Unable to allocate SGML_Attribute_Value","","","");
 }

 if (in_vals->next_value != NULL)
    new->next_value = dup_valuelist(in_vals->next_value);
 if (in_vals->value)
    new->value = strdup(in_vals->value);
 if (in_vals->connector)  
    new->connector = strdup(in_vals->connector);

}


SGML_Attribute 
*dup_attributes(SGML_Attribute  *in_attr) {

SGML_Attribute *new;

	
  if (in_attr == NULL)
	return NULL;

  if ((new = CALLOC(SGML_Attribute,1)) == NULL) {
     SGML_ERROR(ALLOC_ERROR, "Unable to duplicate SGML_Attribute","","","");
  }

  if (in_attr->assoc_tag)
     new->assoc_tag = strdup(in_attr->assoc_tag);
  new->assoc_tag_list = dup_taglist(in_attr->assoc_tag_list);
  if (in_attr->name)
     new->name = strdup(in_attr->name);
  new->declared_value_type = in_attr->declared_value_type;
  new->value_list = dup_valuelist(in_attr->value_list);
  new->default_value_type = in_attr->default_value_type;
  if (in_attr->default_value)
     new->default_value = strdup(in_attr->default_value);
  return (new);

}

int
attach_attributes(SGML_Attribute  *first_attr) {
   Tcl_HashEntry *entry; 
   SGML_Element *elem;
   SGML_Tag_List *tags;
   SGML_Attribute *at, *attr, *next_attr;

   if (first_attr == NULL)
	return (1);

   for (attr = first_attr; attr; attr = next_attr) {

      next_attr = attr->next_attribute;

      if (attr->assoc_tag) {

	elem = (SGML_Element *)
	  find_ns_hash(attr->assoc_tag,
		       current_sgml_dtd->SGML_Element_hash_tab);

	if (elem == NULL){
	  SGML_ERROR(PARSE_ERROR,
		     "Error in attach_attributes -- tag element '",
		     attr->assoc_tag,"' not defined","\n");
	  
	}

	if (elem->attribute_list == NULL) {
	  /* first for element */
	  elem->attribute_list = attr;
	  attr->next_attribute = NULL;
	}
	else {
	  for (at = elem->attribute_list; at->next_attribute; 
	       at = at->next_attribute)
	    if (at == attr) return(0);/* if already attached ignore */
	  /* otherwise we are at the last element */
	  at->next_attribute = attr;
	  attr->next_attribute = NULL;
	  
	}
      }
      else if (attr->assoc_tag_list) { /* not a single tag, but a list */
	for (tags = attr->assoc_tag_list; tags; tags = tags->next_tag ) {

	  elem = (SGML_Element *)
	    find_ns_hash(tags->tag, current_sgml_dtd->SGML_Element_hash_tab);

	  if (elem == NULL){
	    SGML_ERROR(PARSE_ERROR,
		       "Error in attach_attributes -- tag element '",
		       tags->tag,"' not defined","\n");
	    
	  }
	  
	  if (elem->attribute_list == NULL) {
	    /* first for element */
	    elem->attribute_list = dup_attributes(attr);
	    attr->next_attribute = NULL;
	  }
	  else {
	    for (at = elem->attribute_list; at->next_attribute; 
		 at = at->next_attribute)
	      if (at == attr) return(0);/* if already attached ignore */
	    /* otherwise we are at the last element */
	    at->next_attribute = dup_attributes(attr);
	    at->next_attribute->next_attribute = NULL;
	  }
	  
        }	
      }
      else {
	SGML_ERROR(PARSE_ERROR,
		   "Error in attach_attributes -- ",
		   "No associated element tag(s)","\n");
	
      }
      
   }
   
   return(0); 
}

SGML_Processing_Inst *
DupPI(SGML_Processing_Inst *pi)
{
  SGML_Processing_Inst *new;

  if (pi == NULL)
     return NULL;

  new = CALLOC(SGML_Processing_Inst,1);
  /* structure assignment */
  *new = *pi;
  new->next_processing_inst = NULL;
  return (new);
}

/* compare_nstags is a way to compare sgml/xml tags ignoring the namespace part */
int
compare_nstags(char *tag1, char *tag2)
{
  char *t1_start;
  char *t2_start;
  int casesense = 0;

  if (tag1 == NULL || tag2 == NULL) {
    return (99);
  }

  t1_start = strchr(tag1, ':');
  if (t1_start) {
    t1_start++;
    casesense = 1;
  }
  else
    t1_start = tag1;
 
  t2_start = strchr(tag2, ':');
  if (t2_start) {
    t2_start++;
    casesense = 1;
  }
  else
    t2_start = tag2;
 
  /* may need to change this for case sensitivity */
  if (casesense)
    return (strcmp(t1_start, t2_start));
  else
    return (strcasecmp(t1_start, t2_start));
}

void *
find_ns_hash(char *name, Tcl_HashTable *tab) 
{
  char *namepart;
  Tcl_HashEntry *entry; 

  namepart = strchr(name, ':');
  if (namepart == NULL)
    namepart = name;
  else
    namepart++;

  entry = Tcl_FindHashEntry(tab, namepart);

  if (entry == NULL)
    return (NULL);
  else 
    return ((void *) Tcl_GetHashValue(entry));
}


int
verify_namespace(char *name, Tcl_HashTable *tab, Tcl_HashTable *doctab,
		      char **ns_ptr) 
{
  char *namespace, *sep;
  Tcl_HashEntry *entry, *entry2; 
  char *full_namespace;
  char *target;

  if (name == NULL || tab == NULL)
    return(1);
  
  namespace = strdup(name); /* don't want to clobber it */
  sep = strchr(namespace, ':');
  if (sep == NULL) {
    FREE(namespace);
    return(1); /* no namespace */
  }
  else
    *sep = '\0';;
  
  entry = Tcl_FindHashEntry(doctab, namespace);
  
  if (entry == NULL) {
    FREE(namespace);
    return (0);
  }
  else {
    full_namespace = (char *) Tcl_GetHashValue(entry);
    *ns_ptr = full_namespace;
    entry = Tcl_FindHashEntry(tab, full_namespace);
    
    if (entry == NULL) {
      FREE(namespace);
      return (0);
    }
    else {
      FREE(namespace);
      return (1);
    }
  }
}

int
Add_Doc_Namespace(SGML_Attribute_Data *atd, Tcl_HashTable *tab)
{
  char *namespace, *sep;
  char *namepart;
  Tcl_HashEntry *entry; 
  int exists;
  char *newnamespace;

  if (atd == NULL || tab == NULL)
     return(1);

  namepart = strchr(atd->name, ':');
  if (namepart == NULL)
    return(1);
  else
    namepart++;

  entry = Tcl_FindHashEntry(tab, namepart);

  if (entry == NULL) {
     newnamespace = strdup(atd->string_val);
     /* add the namespace info... */
     entry = Tcl_CreateHashEntry(tab, namepart, &exists );
     if (entry != NULL)
	 Tcl_SetHashValue(entry, 
		     (ClientData)newnamespace);
	  
     entry = Tcl_CreateHashEntry(tab, newnamespace, &exists );
     if (entry != NULL)
	Tcl_SetHashValue(entry, 
		     (ClientData)strdup(namepart));
  }

  return(0);
}


void
link_sgml_data(SGML_Document *doc, SGML_Data *dat, 
	       char *buffer_ptr, SGML_Processing_Inst *pi)
{
  char *c;
  char *buffer;
  SGML_Processing_Inst *i, *temp;
  SGML_Data *d;
  SGML_Attribute *at;
  SGML_Attribute_Data *atd, *new_atd;
  Tcl_HashEntry *entry; 
  SGML_Tag_Data *new_data, *tmp_data;
  int exists = 0;
  char *namepart;
  
  if (dat == NULL) return;
  
  if (buffer_ptr == NULL) {
    SGML_ERROR(PARSE_ERROR,
	       "Null buffer pointer in link_sgml_data\n","","","");
    
  }
  
  if (doc->Tag_hash_tab == NULL) {
    doc->Tag_hash_tab = CALLOC(Tcl_HashTable,1);
    Tcl_InitHashTable(doc->Tag_hash_tab, TCL_STRING_KEYS);
  }
  
  /* this is no longer likely to occur since sgml_elim_entities 
   * now unifies the buffer. 
   */
  if (dat->local_entities && dat->local_entities->type == 1)
    buffer = dat->local_entities->entity_text_string;
  else
    buffer = buffer_ptr;
  
  if (dat->sub_data_element) {
    link_sgml_data(doc, dat->sub_data_element, buffer_ptr, pi);
  }
  
  namepart = strchr(dat->start_tag, ':');
  if (namepart == NULL)
    namepart = dat->start_tag;
  else
    namepart++;
  
  /* handle flags for processing instructions that affect this data 
   */
  if (dat->start_tag != NULL && pi != NULL) {
    for (i = pi; i != NULL ; i = i->next_processing_inst) {
      if (compare_nstags(i->tag, dat->start_tag) == 0) {
	/* this tag needs to be flagged */
	dat->processing_flags |= 1<<(i->Instruction - 1);
	if (dat->Processing_instr == NULL)
	  dat->Processing_instr = DupPI(i);   
	else {
	  temp = DupPI(i);
	  temp->next_processing_inst = dat->Processing_instr;
	  dat->Processing_instr = temp;
	}
	
	/* flag all parents (that aren't already flagged) */
	for (d = dat->parent; d != NULL; d = d->parent) {
	  d->processing_flags |= PROC_FLAG_IN_CHILD;
	}		   
      }
    }
  }
  /* Add the tag to the hash table...
   */
  entry = Tcl_FindHashEntry(doc->Tag_hash_tab, namepart);
  if (entry == NULL) {
    /* creat new hash entry */
    entry = Tcl_CreateHashEntry(doc->Tag_hash_tab,   
				namepart, &exists );
    new_data = CALLOC(SGML_Tag_Data, 1);
    new_data->item = dat;
    Tcl_SetHashValue(entry, (ClientData)new_data);
  } 
  else {
    SGML_Tag_Data *td, *foundtd;
    
    new_data = CALLOC(SGML_Tag_Data, 1);
    new_data->item = dat;
    tmp_data =
      (SGML_Tag_Data *) Tcl_GetHashValue(entry);
    new_data->next_tag_data = tmp_data;
    foundtd = NULL;
    for (td = tmp_data; td != NULL && foundtd == NULL; td = td->next_tag_data) {
        if (td->item->parent == dat->parent) {  
	   foundtd = td;
        }
    }

    if (foundtd) 
       new_data->item->occurrence_no = foundtd->item->occurrence_no + 1;

    Tcl_SetHashValue(entry, 
		     (ClientData)new_data);
  }
  
  /* set the pointers into the buffer from the offsets */
  if (dat->start_tag) FREE(dat->start_tag);
  dat->start_tag = buffer +   dat->start_tag_offset;
  dat->content_start = buffer +   dat->content_start_offset;
  dat->content_end = buffer +   dat->content_end_offset;
  if (dat->end_tag) FREE(dat->end_tag);
  dat->end_tag = buffer +   dat->end_tag_offset;
  dat->data_end = buffer +   dat->data_end_offset;
  
  if (dat->element == NULL) {
    /* this SHOULD never happen, but just in case ... */
    SGML_ERROR(PARSE_ERROR,
	       "Null element pointer in link_sgml_data\n","","","");      
  }
  else {
    for (at = dat->element->attribute_list; at != NULL; 
	 at = at->next_attribute) {
      if (at->default_value_type == SPECIFIED_DEFAULT
	  || at->default_value_type == FIXED_DEFAULT) {
	int match = 0;
	/* We don't deal with #IMPLIED, #REQUIRED, etc. */
	/* scan the data attribute defs to see if it is declared */
	for (atd = dat->attributes; atd != NULL; 
	     atd = atd->next_attribute_data) {
	  if (compare_nstags(at->name, atd->name) == 0) {
	    /* names match */
	    match = 1;
	  }
	}
	if (match == 0) {
	  /* we have to add the default value to the data attributes */
	  if ((new_atd = CALLOC(SGML_Attribute_Data,1)) == NULL) {
	    SGML_ERROR(ALLOC_ERROR,
		       "Unable to allocate DEFAULT SGML_Attribute_Data",
		       "","","");
	    
	  }
	  if (dat->attributes == NULL)
	    dat->attributes = new_atd;
	  else {
	    /* stick it in front */
	    new_atd->next_attribute_data = dat->attributes;
	    dat->attributes = new_atd;
	  }
	  new_atd->name = strdup(at->name);
	  new_atd->string_val = strdup(at->default_value);
	}
      }      
    }
  }
  
  
  link_sgml_data(doc, dat->next_data_element, buffer_ptr, pi);
  
}

SGML_Data *
tag_match_doc(SGML_Data *dat, char *tag)
{
  SGML_Data *tmp;
  
  /* recursively search for an UNCLOSED matching tag */
  if (dat == NULL)
    return NULL;
  if (tag == NULL)
    return NULL;
  
  if (strcasecmp(tag, dat->start_tag) == 0 && 
      dat->end_tag_offset == 0) {
    return (dat);
  } 
  
  tmp = tag_match_doc(dat->sub_data_element, tag);
  if (tmp == NULL)
    tmp = tag_match_doc(dat->next_data_element, tag);
  
  return (tmp);
  
}

void
SGML_ERROR(int severity, char *errormsg, char *arg1, char *arg2, char *arg3)
{
  char error_context[200];
  int offset;

  if (parsed_file_chars > 10)
    offset = parsed_file_chars - 10;
  else
    offset = 0;

  memset(error_context, 0, 200);

  if (current_sgml_document != NULL) {
      strncpy(error_context,    
	      (char *)(current_sgml_document->buffer + offset - 20), 160);
  }
  else
      strcpy(error_context, "NO CONTEXT AVAILABLE");

  if (LOGFILE != NULL) {
      fprintf(LOGFILE,"SGML Parser Error: %s %s %s %s RECORD: %d CONTEXT: ||%s||\n", errormsg, arg1, arg2, arg3, last_rec_proc+1, error_context);
      fflush(LOGFILE);
  } 
  else 
      fprintf(stderr,"SGML Parser Error: %s %s %s %s RECORD: %d CONTEXT: ||%s||\n", errormsg, arg1, arg2, arg3, last_rec_proc+1, error_context);

  if (severity > 99)
    cheshire_exit (1);
  else if (severity > 0)
    sgml_parse_error = 1;
  
}

char *SubEntities(char *instring)
{
  char *temp, *c, *estart, *eend, *newstr;
  Tcl_HashEntry *entry;	
  SGML_Entity *ent;  
  int templen, newlen;
  char errormsg[500];

  if (instring == NULL)
    return NULL;

  temp = instring;
  
  while ((estart = strchr(temp, '%')) != NULL) {
    /* if we enter the loop there is an entity reference in the string */
    templen = strlen(temp);
    
    *estart++ = '\0';
    eend = strchr(estart, ';');
    *eend = '\0';

    entry = Tcl_FindHashEntry(	    
			      current_sgml_dtd->SGML_Entity_hash_tab,
			      estart);
    if (entry == NULL){
      sprintf(errormsg, "' not defined in entity contents '%s'\n", instring);
      SGML_ERROR(PARSE_ERROR,"Nested Entity '", 
		 estart, errormsg,"");
      return NULL;
    }
    else
      ent = (SGML_Entity *) Tcl_GetHashValue(entry);
    
    if (ent->entity_text_string != NULL) {
      newlen = templen + strlen(ent->entity_text_string);
      newstr = CALLOC(char, newlen);
      sprintf(newstr, "%s%s%s", temp, ent->entity_text_string,
	      eend+1);
#ifdef DEBUGDTD
      printf(" Substituted entity value '%s'\n", newstr); 
#endif
      
    }
    else {
      SGML_ERROR(PARSE_ERROR,"Nested Entity '", 
		 estart,"' is not a string entity\n","");
      return NULL;
    }
    
    /* move to temp and free the old string */
    FREE(temp);
    temp = newstr;

  }

  return (temp);
  
}
/* end of additional routines */ }
