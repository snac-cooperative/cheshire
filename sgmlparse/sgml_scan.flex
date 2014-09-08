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
/* test scanner code for dtds */
%{
#include <stdlib.h>
#ifndef WIN32
#ifndef SOLARIS
#include <strings.h>
#endif
#endif
#include <string.h>
#include <errno.h>
#include "sgml.h"
#include <sys/stat.h>

#include "sgml_pars.h"

#ifdef WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif


extern FILE *LOGFILE;

/* uncomment for tons of tracing info */
/* #define DEBUGPARSERMSG 1 */
/* #define DEBUGPARSER 1 */
/* #define DEBUGPARSERSWITCH 1 */

/* these need to match the defines in sgml.h */
#define INSTR_INDEXING 1
#define INSTR_DISPLAY  2
#define PROC_SUBST_TAG 1
#define PROC_SUBST_ATTR 2
#define PROC_DELETE_TAG 3
#define PROC_INDEX_SUBST_TAG 4
#define PROC_INDEX_SUBST_ATTR 5
#define PROC_INDEX_DELETE_TAG 6


int parsed_file_chars = 0;
int save_parsed_file_chars = 0;

int current_record_id_number = 0;
char *current_file_name = NULL;

char *temp_parse_buffer = NULL;
int  temp_parse_buffer_size = TEMP_PARSE_BUFFER_SIZE;
int  temp_parse_buffer_differs = 0;

extern struct sgml_yylval { 
        int  tok_id;
        char *string;
        int intval;
	Tcl_HashEntry *hash_entry; 
 } sgml_yylval, sgml_yylval_pushback, sgml_yylval_include_back;

extern SGML_DTD *current_sgml_dtd; /* declared in sgml_gram.g */
extern SGML_Document *current_sgml_document;
extern SGML_Entity *active_sgml_entity;


#define YY_USER_ACTION 	sgml_realloc_buffer(parsed_file_chars, yytext, yyleng); \
			parsed_file_chars += yyleng; \
		        sgml_yylval.string = NULL; \
			sgml_yylval.hash_entry = NULL;

int parsed_file_lineno = 1; 
char char_insert;
int  char_insert_size;

extern int sgml_parse_error;
extern int in_doc_type_decl;

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifdef DMALLOC_INCLUDE
#define YY_MALLOC_DECL
#endif


int sgml_prev_state[500];
int sgml_state = 0;
int sys_state; /* state when SYSTEM include call is made */
 
#define MAX_INCLUDE_DEPTH 20
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
int include_entity = 0;
int string_entity = 0;
int templen;
FILE *tmp_yyin = NULL;

char *backup_string;

extern int expect_otm; /* flag set in grammar file newgrammar.g */
extern int expect_document; /* flag set in grammar file newgrammar.g */
extern int default_entity;  /* set when #DEFAULT is used as entity name */
extern int expect_decl_value; /* set after the name in a attribute list */
extern char *up_case_string();

%}

/* Definitions */

DIGIT  [0-9]
DIGITS [0-9]+	
LETTER [a-zA-Z]

stago "<"
etago "</"
tagc  ">"
net "/"
mdo "<!"
mdc ">"
nullmdo "<!>"
comdcls "<!--"
comdcle "-->"
marked_sec_start "<!["
marked_sec_end   "]]>"
pio "<?"
pic ">"
xmlc "?>"
emptytagc "/>"
dso "["
dsc "]"
com "--"
cro "&#"
ws  [ \t]+
space " "
newline "\n"|"\r\n"
vi "="


/* note that this doesn't allow names starting with . or - */
sgmlname [a-zA-Z][a-zA-Z0-9.:_-]*
name_token [a-zA-Z0-9][a-zA-Z0-9.:_-]*
param_lit (\'[^']*\')|(\"[^"]*\")
sysid_token ([a-zA-Z0-9._/~+-]*)
num_token ([0-9.:,&()[\]\-]*)
entity_token ({sgmlname})|(%{sgmlname})
subtag ("SUBSTITUTE_TAG"|"SUBST_TAG"|"substitute_tag"|"subst_tag")
igntag ("IGNORE_TAG"|"IGN_TAG"|"ignore_tag"|"ign_tag")
deltag ("DELETE_TAG"|"DEL_TAG"|"delete_tag"|"del_tag")
subattr ("SUBSTITUTE_ATTR"|"SUBST_ATTR"|"substitute_attr"|"subst_attr")
indxsubtag ("INDEX_SUBSTITUTE_TAG"|"INDEX_SUBST_TAG"|"index_substitute_tag"|"index_subst_tag")
indxigntag ("INDEX_IGNORE_TAG"|"INDEX_IGN_TAG"|"index_ignore_tag"|"index_ign_tag")
indxdeltag ("INDEX_DELETE_TAG"|"INDEX_DEL_TAG"|"index_delete_tag"|"index_del_tag")
indxsubattr ("INDEX_SUBSTITUTE_ATTR"|"INDEX_SUBST_ATTR"|"index_substitute_attr"|"index_subst_attr")
grpo "("
grpc ")"
rni  "#"
plus "+"
minus "-"
explus "+("
exminus "-("
equals "="
rep "*"
opt "?"
and "&"
or "|"
seq ","
dtgo "["
dtgc "]"
procinstr ([a-zABD-Z][^?>]*)
ero  "&"
ampersand_space  "& "
pero "%"
refc ";"

/* Start conditions */
%x sgml_comment
%x param_comment
%x sgml_decl
%x xml_decl
%x xml_version
%x xml_encoding
%x xml_standalone
%s sgml_doctype
%s sgml_attlist
%s sgml_element
%s sgml_entity
%s sgml_shortref
%s sgml_usemap
%x sgml_marked
%s sgml_notation
%x sgml_proc_inst
%x cheshire_proc_inst
%x cheshire_proc_inst_args
%x groups
%x mdo_start
%s newinclude
%x document
%s document_start
%x charref
%x docendtag
%x docstarttag
%x sgml_catalog
%x empty_end_tag
%x empty_tag_contents
%x marked_data
%%

<sgml_decl,sgml_doctype,sgml_attlist,sgml_element,sgml_entity,sgml_notation,groups,mdo_start,sgml_shortref,sgml_usemap,INITIAL,newinclude>{ws} {
         return TOKEN_SEP;
	}



<sgml_comment,param_comment>[^->\n]*  {return JUNK;} 
                                    /* get rid of anything but - and > */
<sgml_comment>{comdcle} {
		         if (sgml_state >= 1)
	     			BEGIN(sgml_prev_state[--sgml_state]); 	             
                         return COMDCLE;}


<param_comment>{com} 	{
		         if (sgml_state >= 1)
	     			BEGIN(sgml_prev_state[--sgml_state]);
                         return COM;}

<sgml_comment,param_comment>"-"      {return JUNK;}
<sgml_comment,param_comment>">"      {return JUNK;}
<sgml_comment,param_comment>{newline} {
	parsed_file_lineno++;
	}



{comdcls} { 
         sgml_prev_state[sgml_state++] = YY_START;
         BEGIN(sgml_comment); 
         return COMDCLS;
}

<document,document_start>{nullmdo} {
	  return NULLMDO;
}

<document,document_start>{comdcls} { 
         sgml_prev_state[sgml_state++] = YY_START;
         BEGIN(sgml_comment); 
         return COMDCLS;
}

<sgml_decl,mdo_start,sgml_attlist,sgml_element,sgml_entity,sgml_shortref,sgml_usemap>{com} {
        if (expect_otm) {
            sgml_yylval.string = up_case_string(strdup(yytext));
	    sgml_yylval.string[1] = '\0';
	    yyless(1); 
   	    return OTM;
	 }

         sgml_prev_state[sgml_state++] = YY_START;
         BEGIN(param_comment); 
         return COM;
}


<sgml_decl>{mdc} {
	     BEGIN(INITIAL);
	  return MDC;
	 }

<sgml_attlist,sgml_doctype,sgml_entity,sgml_element,sgml_shortref,sgml_notation,sgml_usemap,document_start,INITIAL>{mdc} {
         BEGIN(INITIAL);
         return MDC;
}

<sgml_doctype,sgml_attlist,sgml_marked,INITIAL>{dsc}	 { 
       if (in_doc_type_decl)
              BEGIN(sgml_prev_state[--sgml_state]);
        return DSC;}


<sgml_doctype,sgml_attlist>{dso} {
       if (in_doc_type_decl)
         sgml_prev_state[sgml_state++] = YY_START;
	return DSO;
}

<sgml_marked>{dso} {
	BEGIN(marked_data);
	/* printf("Position of DSO %d\n", parsed_file_chars); */
	return DSO;
}

<sgml_entity,sgml_attlist,groups,sgml_usemap>{rni} { return RNI;}

<sgml_decl,sgml_doctype,sgml_attlist,sgml_element,sgml_entity,sgml_marked,groups,mdo_start,sgml_shortref,sgml_usemap,INITIAL,newinclude>{pero}[a-zA-Z][a-zA-Z0-9.-]*[; )>\n]  {
	char *c;
	templen = yyleng - 2; 
	sgml_yylval.string = CALLOC(char, templen+1);
	strncpy(sgml_yylval.string, &yytext[1],templen);
	sgml_yylval.string[templen] = '\0';
	/* entities no longer case insensitive */
	/* for (c = sgml_yylval.string; *c; c++) *c = toupper(*c); */

	if (yytext[yyleng-1] == ')' || yytext[yyleng-1] == '>') {
	   yyless(yyleng-1);
	}

	return PERO_REF;
	}

{pero}	 { return PERO;}


<sgml_doctype,sgml_attlist,sgml_element,sgml_entity,groups,mdo_start,sgml_shortref,sgml_usemap,INITIAL>{grpo}	{
         sgml_prev_state[sgml_state++] = YY_START;
         BEGIN(groups);
         return GRPO;
	}

<groups>{grpc}	{
         if (sgml_state >= 1)
	 BEGIN(sgml_prev_state[--sgml_state]); 	             
	 return GRPC;
        }



<cheshire_proc_inst>{subtag} {
/* printf("sub tag found\n"); */

        sgml_yylval.intval = PROC_SUBST_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{igntag} {
/* printf("ignore tag found\n"); */

        sgml_yylval.intval = PROC_SUBST_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{indxsubtag} {
/* printf("indexsub tag found\n"); */
        sgml_yylval.intval = PROC_INDEX_SUBST_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{indxigntag} {
/* printf("index ign tag found\n"); */
        sgml_yylval.intval = PROC_INDEX_SUBST_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}


<cheshire_proc_inst>{deltag} {
/* printf("del tag found\n"); */
        sgml_yylval.intval = PROC_DELETE_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{indxdeltag} {
/* printf("index del tag found\n"); */
        sgml_yylval.intval = PROC_INDEX_DELETE_TAG;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst,sgml_proc_inst>{newline} {
/* printf("pi newline\n"); */
/* fflush(stdout); */
	parsed_file_lineno++;

}

<document_start>{newline} {
	parsed_file_lineno++;
	return TOKEN_SEP;

}

<cheshire_proc_inst>{subattr} {
/* printf("sub attr tag found\n"); */
        sgml_yylval.intval = PROC_SUBST_ATTR;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{indxsubattr} {
/* printf("index sub attr tag found\n"); */
        sgml_yylval.intval = PROC_INDEX_SUBST_ATTR;
        sgml_yylval.string = strdup(yytext);	
        BEGIN(cheshire_proc_inst_args); 
	return INSTRUCTION_TYPE;
}

<cheshire_proc_inst>{ws} { }

<cheshire_proc_inst_args>[^?>]*  {/* get anything but > or ?> */
     char *c, *e;
     sgml_yylval.intval = 0;
     c = yytext;
     /* skip leading blanks */
     while (*c == ' ' || *c == '\t' || *c == '\n') c++;
     /* and remove trailing blanks */
     e = c + (strlen(c) - 1);
     while (e > c && (*e == ' ' || *e == '\t' || *e == '\n')) {
	 *e = '\0';
	 e--;
     }
     sgml_yylval.string = strdup(c);		
     /* printf("cheshire PI args: %s\n", sgml_yylval.string); */

     BEGIN(sgml_proc_inst); 
     return ARGS;
}

<sgml_proc_inst>"CHESHIREINDEX" {
/* printf("CHESHIREINDEX instruction\n"); */
     BEGIN(cheshire_proc_inst); 
     sgml_yylval.string = strdup(yytext);	
     sgml_yylval.intval = INSTR_INDEXING;
     return CHESHIRE_INSTRUCTION;				
}

<sgml_proc_inst>"CHESHIREDISPLAY" {     
     BEGIN(cheshire_proc_inst); 
     sgml_yylval.string = strdup(yytext);	
     sgml_yylval.intval = INSTR_DISPLAY;
     return CHESHIRE_INSTRUCTION;				
}

<sgml_proc_inst>{procinstr} {/* get anything but > or ?> */

        /* printf("NON-CHESHIRE INSTRUCTION???\n"); */
        sgml_yylval.intval = 0;
	sgml_yylval.string = strdup(yytext);	
	return INSTRUCTION;
}
 
<sgml_proc_inst>{xmlc} 	{
		         if (sgml_state >= 1)
	     			BEGIN(sgml_prev_state[--sgml_state]);
                         return PIC;}

<sgml_proc_inst>{pic} 	{
		         if (sgml_state >= 1)
	     			BEGIN(sgml_prev_state[--sgml_state]); 	             
                         return PIC;}

<document,document_start,INITIAL,newinclude>{pio}   {
/* printf("PIO found\n"); */
         sgml_prev_state[sgml_state++] = YY_START;
         BEGIN(sgml_proc_inst); 
         return PIO;
}

<xml_decl>("VERSION"|"version") {
				BEGIN(xml_version);
				return VERSIONINFO;}

<xml_version,xml_encoding,xml_standalone>{equals}  {return VI;}

<xml_decl,xml_version,xml_encoding,xml_standalone>{space}  {}


<xml_version,xml_decl>("ENCODING"|"encoding") {
				BEGIN(xml_encoding);
				return ENCODINGDECL;}


<xml_version,xml_encoding,xml_decl>("STANDALONE"|"standalone") {
				BEGIN(xml_standalone);
				return SDDECL;}


<xml_version,xml_decl,xml_encoding,xml_standalone>{xmlc} { 
	 if (sgml_state >= 1)
	     BEGIN(sgml_prev_state[--sgml_state]);
	 else
	     BEGIN(INITIAL);
	return XML_ENDDECL;}



<document,document_start,INITIAL,newinclude>{pio}("XML"|"xml") {
	sgml_prev_state[sgml_state++] = YY_START;
	BEGIN(xml_decl);
	return XML_DECL;
}


{pio}("XML"|"xml") {
				BEGIN(xml_decl);
				return XML_DECL;}



{mdo}("SGML"|"sgml")  {
				BEGIN(sgml_decl);
				return MDO_SGML;}

<document_start,INITIAL>{mdo}("DOCTYPE"|"doctype")  {
			        sgml_prev_state[sgml_state++] = YY_START;
				BEGIN(sgml_doctype);
				return MDO_DOCTYPE;}
{mdo}("ELEMENT"|"element")  {
				BEGIN(sgml_element); 
				return MDO_ELEMENT;}
{mdo}("ENTITY"|"entity")    {
				 BEGIN(sgml_entity);
			        /* sgml_prev_state[sgml_state++] = YY_START;*/
				 return MDO_ENTITY;}

{mdo}("SHORTREF"|"shortref") {
			      BEGIN(sgml_shortref);
			      return MDO_SHORTREF;}

{mdo}("ATTLIST"|"attlist")  {
				BEGIN(sgml_attlist); 
				return MDO_ATTLIST;}

<marked_data>{marked_sec_end}  {
                          /* BEGIN(INITIAL); */
			  BEGIN(sgml_prev_state[--sgml_state]);
                          return MARKED_SECTION_END;}

<marked_data>([^\]]*"]]>") {
		sgml_yylval.string = strdup(yytext); 
		sgml_yylval.string[strlen(sgml_yylval.string)-3] = '\0';
		/* printf("MARKED SECTION DATA IS: %s\n", yytext); */
	        yyless(yyleng-3);
		parsed_file_chars -= 3;		
		return QSTRING;
	}

<*>{marked_sec_start}  { 
	        sgml_prev_state[sgml_state++] = YY_START;
		BEGIN(sgml_marked);
	        return MARKED_SECTION_START;
	}

<*>"CDATA" { return CDATA_DCL;}
<*>"(CDATA)" { return CDATA_DCL;}
<*>"SDATA" { return SDATA_DCL;}
<*>"NDATA" { return NDATA_DCL;}

<sgml_entity>"PI" { return PI_DCL;}
<sgml_entity>"STARTTAG" { return STARTTAG_DCL;}
<sgml_entity>"ENDTAG" { return ENDTAG_DCL;}
<sgml_entity>"MS" { return MS_DCL;}
<sgml_entity>"MD" { return MD_DCL;}
<sgml_entity>"DEFAULT" { return DEFAULT_DCL;}
<sgml_entity>"SUBDOC" { return SUBDOC_DCL;}

<sgml_marked>"TEMP" { return TEMP_DCL;}
<sgml_marked>"IGNORE" {  /* printf("IGNORE found & recognized \n");*/ 
		return IGNORE_DCL;}
<sgml_marked>"INCLUDE" {  /* printf("INCLUDE found & recognized \n"); */ 
		return INCLUDE_DCL;}

<sgml_marked,sgml_entity,sgml_attlist,sgml_element>"RCDATA" {
                              return RCDATA_DCL;}
<sgml_entity,sgml_attlist,groups>"PCDATA" { return PCDATA_DCL;}
<sgml_usemap,sgml_element>"EMPTY" { return EMPTY_DCL;}
"ANY"   { return ANY;}


<sgml_attlist>"ENTITY" {if (expect_decl_value) return ENTITY_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"ENTITIES" {if (expect_decl_value) return ENTITIES_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"ID" {if (expect_decl_value) return ID_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"IDREF" {if (expect_decl_value) return IDREF_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"IDREFS" {if (expect_decl_value) return IDREFS_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NAME" {if (expect_decl_value) return NAME_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NAMES" {if (expect_decl_value) return NAMES_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NMTOKEN" {if (expect_decl_value) return NMTOKEN_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NMTOKENS" {if (expect_decl_value) return NMTOKENS_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NUMBER" {if (expect_decl_value) return NUMBER_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NUMBERS" {if (expect_decl_value) return NUMBERS_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NUTOKEN" {if (expect_decl_value) return NUTOKEN_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}
<sgml_attlist>"NUTOKENS" {if (expect_decl_value) return NUTOKENS_DCL;
			else {	sgml_yylval.string = up_case_string(strdup(yytext)); 
			        return SGMLNAME;
			}
		}


<sgml_attlist>"#FIXED"|"#fixed" {return RNI_FIXED_DCL;}
<sgml_attlist>"#CURRENT"|"#current" {return RNI_CURRENT_DCL;}
<sgml_attlist>"#CONREF"|"#conref" {return RNI_CONREF_DCL;}
<sgml_attlist>"#IMPLIED"|"#implied" {return RNI_IMPLIED_DCL;}
<sgml_attlist>"#REQUIRED"|"#required" {return RNI_REQUIRED_DCL;}
<sgml_attlist>"NOTATION"|"notation" {return NOTATION_DCL;}
<sgml_attlist>{equals}  {return VI;}


<sgml_decl>"CHARSET" {return CHARSET_DCL;}
<sgml_decl>"BASESET" {return BASESET_DCL;}
<sgml_decl>"DESCSET" {return DESCSET_DCL;}
<sgml_decl>"UNUSED" {return UNUSED_DCL;}
<sgml_decl>"CAPACITY" {return CAPACITY_DCL;}

<sgml_decl>"TOTALCAP" {return TOTALCAP_DCL;}
<sgml_decl>"ENTCAP" {return ENTCAP_DCL;}
<sgml_decl>"ENTCHCAP" {return ENTCHCAP_DCL;}
<sgml_decl>"ELEMCAP" {return ELEMCAP_DCL;}
<sgml_decl>"GRPCAP" {return GRPCAP_DCL;}
<sgml_decl>"EXGRPCAP" {return EXGRPCAP_DCL;}
<sgml_decl>"EXNMCAP" {return EXNMCAP_DCL;}
<sgml_decl>"ATTCAP" {return ATTCAP_DCL;}
<sgml_decl>"ATTCHCAP" {return ATTCHCAP_DCL;}
<sgml_decl>"AVGRPCAP" {return AVGRPCAP_DCL;}
<sgml_decl>"NOTCAP" {return NOTCAP_DCL;}
<sgml_decl>"NOTCHCAP" {return NOTCHCAP_DCL;}
<sgml_decl>"IDCAP" {return IDCAP_DCL;}
<sgml_decl>"IDREFCAP" {return IDREFCAP_DCL;}
<sgml_decl>"MAPCAP" {return MAPCAP_DCL;}
<sgml_decl>"LKSETCAP" {return LKSETCAP_DCL;}
<sgml_decl>"LKNMCAP" {return LKNMCAP_DCL;}

<sgml_decl>"SGMLREF" {return SGMLREF_DCL;}
<sgml_decl>"SHORTREF" {return SHORTREF_DCL;}
<sgml_decl>"SCOPE" {return SCOPE_DCL;}
<sgml_decl>"DOCUMENT" {return DOCUMENT_DCL;}
<sgml_decl>"INSTANCE" {return INSTANCE_DCL;}
<sgml_decl>"SYNTAX" {return SYNTAX_DCL;}
<sgml_decl>"SWITCHES" {return SWITCHES_DCL;}
<sgml_decl>"SHUNCHAR" {return SHUNCHAR_DCL;}
<sgml_decl>"CONTROLS" {return CONTROLS_DCL;}
<sgml_decl>"FUNCTION" {return FUNCTION_DCL;}
<sgml_decl>"RE" {return RE_DCL;}
<sgml_decl>"RS" {return RS_DCL;}
<sgml_decl>"SPACE" {return SPACE_DCL;}

<sgml_decl>"FUNCHAR" {return FUNCHAR_DCL;}
<sgml_decl>"MSICHAR" {return MSICHAR_DCL;}
<sgml_decl>"MSOCHAR" {return MSOCHAR_DCL;}
<sgml_decl>"MSSCHAR" {return MSSCHAR_DCL;}
<sgml_decl>"SEPCHAR" {return SEPCHAR_DCL;}
<sgml_decl>"NAMING" {return NAMING_DCL;}
<sgml_decl>"LCNMSTRT" {return LCNMSTRT_DCL;}
<sgml_decl>"UCNMSTRT" {return UCNMSTRT_DCL;}
<sgml_decl>"UCNMCHAR" {return UCNMCHAR_DCL;}
<sgml_decl>"LCNMCHAR" {return LCNMCHAR_DCL;}
<sgml_decl>"NAMECASE" {return NAMECASE_DCL;}
<sgml_decl>"GENERAL" {return GENERAL_DCL;}
<sgml_decl>"ENTITY" {return ENTITY_DCL;}
<sgml_decl>"NO" {return NO_DCL;}
<sgml_decl>"YES" {return YES_DCL;}
<sgml_decl>"NONE" {return NONE_DCL;}
<sgml_decl>"DELIM" {return DELIM_DCL;}
<sgml_decl>"NAMES" {return NAMES_DCL;}
<sgml_decl>"QUANTITY" {return QUANTITY_DCL;}

<sgml_decl>"ATTCNT" {return ATTCNT_DCL;}
<sgml_decl>"ATTSPLEN" {return ATTSPLEN_DCL;}
<sgml_decl>"BSEQLEN" {return BSEQLEN_DCL;}
<sgml_decl>"DTAGLEN" {return DTAGLEN_DCL;}
<sgml_decl>"DTEMPLEN" {return DTEMPLEN_DCL;}
<sgml_decl>"ENTLVL" {return ENTLVL_DCL;}
<sgml_decl>"GRPCNT" {return GRPCNT_DCL;}
<sgml_decl>"GRPGTCNT" {return GRPGTCNT_DCL;}
<sgml_decl>"GRPLVL" {return GRPLVL_DCL;}
<sgml_decl>"LITLEN" {return LITLEN_DCL;}
<sgml_decl>"NAMELEN" {return NAMELEN_DCL;}
<sgml_decl>"NORMSEP" {return NORMSEP_DCL;}
<sgml_decl>"PILEN" {return PILEN_DCL;}
<sgml_decl>"TAGLEN" {return TAGLEN_DCL;}
<sgml_decl>"TAGLVL" {return TAGLVL_DCL;}

<sgml_decl>"FEATURES" {return FEATURES_DCL;}
<sgml_decl>"MINIMIZE" {return MINIMIZE_DCL;}
<sgml_decl>"DATATAG" {return DATATAG_DCL;}
<sgml_decl>"OMITTAG" {return OMITTAG_DCL;}
<sgml_decl>"RANK" {return RANK_DCL;}
<sgml_decl>"SHORTTAG" {return SHORTTAG_DCL;}
<sgml_decl>"LINK" {return LINK_DCL;}
<sgml_decl>"SIMPLE" {return SIMPLE_DCL;}
<sgml_decl>"IMPLICIT" {return IMPLICIT_DCL;}
<sgml_decl>"EXPLICIT" {return EXPLICIT_DCL;}
<sgml_decl>"OTHER" {return OTHER_DCL;}
<sgml_decl>"CONCUR" {return CONCUR_DCL;}
<sgml_decl>"SUBDOC" {return SUBDOC_DCL;}
<sgml_decl>"FORMAL" {return FORMAL_DCL;}
<sgml_decl>"APPINFO" {return APPINFO_DCL;}

<sgml_decl>[0-9]+  {/* string not neededsgml_yylval.string = (yytext); */
	    sgml_yylval.string = NULL;
            sgml_yylval.intval = atoi(yytext);
            return NUMBER;
          }


{mdo}"USEMAP" {BEGIN(sgml_usemap); return MDO_USEMAP_DCL;}
{mdo}"NOTATION" {BEGIN(sgml_notation); return MDO_NOTATION_DCL;}


<sgml_doctype>"SYSTEM" {
        
        sys_state = YY_START;
	return SYSTEM_DCL;
	}

<sgml_notation,sgml_entity>"SYSTEM" {
        
	return SYSTEM_DCL;
	}

<sgml_doctype,sgml_entity,sgml_notation,sgml_decl>"PUBLIC" {return PUBLIC_DCL;}


<sgml_element>[-oO]  {	sgml_yylval.string = up_case_string(strdup(yytext)); 
	     if (expect_otm) {
		return OTM;
	     }
             else {
		return SGMLNAME;
	     }
	}

<sgml_entity>{sgmlname} {
	/* Entity names are no longer moved to upper case */
	sgml_yylval.string = strdup(yytext);
	return SGMLNAME;
}

<sgml_decl,sgml_doctype,sgml_attlist,sgml_element,sgml_notation,groups,mdo_start,sgml_shortref,sgml_usemap,INITIAL,newinclude>{sgmlname} { 
	sgml_yylval.string = up_case_string(strdup(yytext)); 
	if (expect_otm && (yytext[0] == 'o' || yytext[0] == 'O')) {
		return OTM;
	}
        else {
		return SGMLNAME;
	}
    }

<sgml_attlist,groups>{name_token}  { 
	char *c;
	sgml_yylval.string = up_case_string(strdup(yytext)); 
	for (c = sgml_yylval.string; *c ; c++) {
	    if (isalpha(*c)) 	return NAME_TOKEN;
	}
	/* if here there are no alpha chars */
	sgml_yylval.intval = atoi(sgml_yylval.string);
	return NUMBER;
	}

<groups>{and} {sgml_yylval.string = strdup(yytext); return AND;}
<groups>{or}  {sgml_yylval.string = strdup(yytext); return OR;}
<groups>{seq} {sgml_yylval.string = strdup(yytext); return SEQ;}
<groups>{dtgo} {return DTGO;}
<groups>{dtgc} {return DTGC;}
<groups,sgml_element>{rep}	{sgml_yylval.string = strdup(yytext);  return REP;}
<groups>{opt}	{sgml_yylval.string = strdup(yytext);  return OPT;}
<groups,sgml_element>{plus}  {sgml_yylval.string = strdup(yytext);  return PLUS;}
<sgml_element>{exminus}	{/* put back the open parenthesis */ 
		yyless(1);  
		return EXMINUS;
	}
<sgml_element>{explus}	{/* put back the open parenthesis */
		yyless(1);
		return EXPLUS;
	}

<sgml_decl,xml_decl,xml_version,xml_encoding,xml_standalone,groups,sgml_element,sgml_doctype,sgml_entity,sgml_notation,sgml_shortref>{param_lit}  {
	char *c; /* for line counting */
         
	templen = yyleng - 2; 
	sgml_yylval.string = CALLOC(char, templen+1);
	strncpy(sgml_yylval.string, &yytext[1],templen);
	sgml_yylval.string[templen] = '\0';
	/* keep the count of lines */
	for (c = sgml_yylval.string; *c ; c++) 
		if (*c == '\n') parsed_file_lineno++;
	return PARAMETER_LITERAL;
	}

<sgml_attlist,docstarttag>{param_lit}  {
	char *c;
       	templen = yyleng - 2; 
	sgml_yylval.string = CALLOC(char, templen+1);
	strncpy(sgml_yylval.string, &yytext[1],templen);
	sgml_yylval.string[templen] = '\0';
	/* keep the count of lines */
	for (c = sgml_yylval.string; *c ; c++) 
		if (*c == '\n') parsed_file_lineno++;

	return ATTR_VAL_LITERAL;
      }


<document,document_start,newinclude>{stago} {
#ifdef DEBUGPARSER
        printf("START TAG BEGIN\n"); 
#endif
	BEGIN(docstarttag);
	return STAGO;
	}

<document,newinclude>{etago} { 
	BEGIN(docendtag);
	return ETAGO;
	}

<docendtag,docstarttag>{tagc} { 
	BEGIN(document);
	return TAGC;
	}

<docstarttag>{emptytagc} {
	BEGIN(document);
	return EMPTYTAGC;
	}

<docendtag,docstarttag>{sgmlname} {
	sgml_yylval.string = up_case_string(strdup(yytext));
	sgml_yylval.intval = -1;
	/* printf("%s|",yytext); */
	return SGMLNAME;
	}

<empty_tag_contents>{space} {
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	/* printf("SPACE in EMPTY TAG CONTENTS\n");*/
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{newline} {
        parsed_file_chars -= yyleng;	 
	yyless(0);
	/* parsed_file_lineno++; */
	/* printf("NEWLINE in EMPTY TAG CONTENTS\n"); */
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{pio} {
	/* printf("PIO in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{stago} {
#ifdef DEBUGPARSER
	printf("STAGO in EMPTY TAG CONTENTS\n"); 
#endif
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{etago} {
	/* printf("ETAGO in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>"$" {
	/* printf("dollar in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>"\\" {
	/* printf("dollar in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{sgmlname} {
	/* printf("data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{name_token} {
	/* printf("data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{param_lit} {
	/* printf("data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_tag_contents>{num_token} {
	/* printf("numeric data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(empty_end_tag);
	return EMPTY_TAG_CONTENTS;
	}

<empty_end_tag>{space} {
	/* printf("SPACE in EMPTY END TAG\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{newline} {
	/* printf("NEWLINE in EMPTY END TAG\n"); */
	parsed_file_lineno++;
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{stago} {
	/* printf("STAGO in EMPTY END TAG\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{etago} {
	/* printf("ETAGO in EMPTY END TAG\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	/* return EMPTY_END_TAG; -- empty tag appears to have end tag */
	}


<empty_end_tag>"$" {
	/* printf("dollar in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>"\\" {
	/* printf("dollar in EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{sgmlname} {
	/* printf("data in (after) EMPTY TAG\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{name_token} {
	/* printf("data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{param_lit} {
	/* printf("data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}

<empty_end_tag>{num_token} {
	/* printf("numeric data in (after) EMPTY TAG CONTENTS\n"); */
        parsed_file_chars -= yyleng;	 
	yyless(0);
	BEGIN(document);
	return EMPTY_END_TAG;
	}


<docstarttag>{name_token}  { 
	char *c;
	sgml_yylval.string = up_case_string(strdup(yytext)); 
        
	for (c = sgml_yylval.string; *c ; c++) {
	    if (isalpha(*c)) {
		sgml_yylval.intval = -1;
		return NAME_TOKEN;
	    }
	}
	/* if here there are no alpha chars */
	sgml_yylval.intval = atoi(sgml_yylval.string);
	return NUMBER;
	}

<docendtag,docstarttag>{ws} {
	return TOKEN_SEP;
	}

<docendtag,docstarttag>{newline} {
	parsed_file_lineno++;
	return TOKEN_SEP;
	}

<docstarttag>{vi} {
	return VI;
	}

<document>{newline} {
	parsed_file_lineno++;
	sgml_yylval.string = strdup(yytext);
	return PCDATA_DATA;	
	}

<document>"$" {
	sgml_yylval.string = strdup(yytext);
	return PCDATA_DATA;	
	}

<document>"\\" {
	sgml_yylval.string = strdup(yytext);
	return PCDATA_DATA;	
	}

<document>{ampersand_space} {
	sgml_yylval.string = strdup(yytext);
	return PCDATA_DATA;	
	}

<document>"#RESTORE"|"#restore" {return RNI_RESTORE_DCL;}
<document>"#INITIAL"|"#initial" {return RNI_INITIAL_DCL;}
<document>{cro} {
        if ((current_sgml_dtd->display_options & 16) == 16) {
	/* this means that dispoptions was set to KEEP_ENTITIES */
	/* so we treat as PCDATA                                */
	   sgml_yylval.string = strdup(yytext);
	   return PCDATA_DATA;	
	}
	BEGIN(charref); 
	char_insert = 0; 
	char_insert_size = yyleng;
	return CRO; }

<document,docstarttag,docendtag>{ero}[a-zA-Z][a-zA-Z0-9\-.]*[; ]  {
	char *c;
	char insertchar = 0;
	int end_blank = 0;

        if ((current_sgml_dtd->display_options & 16) == 16) {
	/* this means that dispoptions was set to KEEP_ENTITIES */
	/* so we treat as PCDATA                                */
		sgml_yylval.string = strdup(yytext);
		return PCDATA_DATA;	
	}
	templen = yyleng - 2; 
	sgml_yylval.string = CALLOC(char, templen+1);
	strncpy(sgml_yylval.string, &yytext[1],templen);
	sgml_yylval.string[templen] = '\0';
	/* entities no longer case insensitive */
	/* for (c = sgml_yylval.string; *c; c++) *c = toupper(*c); */
	
	c = sgml_yylval.string;

	if (yytext[yyleng-1] == ' ') {
	   yyless(yyleng-1);
	   parsed_file_chars -= 1;
	   end_blank = 1;
	}

        if (current_sgml_dtd->display_options == 0
	    || current_sgml_dtd->display_options == 2
	    || current_sgml_dtd->display_options == 4
	    || current_sgml_dtd->display_options == 6) {
	    if (strcmp(sgml_yylval.string, "amp") == 0) {
	       insertchar = '&';
	    }
        }

        if (current_sgml_dtd->display_options == 0
	    || current_sgml_dtd->display_options == 1
	    || current_sgml_dtd->display_options == 4
	    || current_sgml_dtd->display_options == 5) {
	   /* handle a couple cases where markup may have been translated */
	   if (strcmp(sgml_yylval.string, "lt") == 0) {
	      insertchar = '<';
	   }
	}

        if (current_sgml_dtd->display_options == 0
	    || current_sgml_dtd->display_options == 1
	    || current_sgml_dtd->display_options == 2
	    || current_sgml_dtd->display_options == 3) {
	    if (strcmp(sgml_yylval.string, "gt") == 0) {
	       insertchar = '>';
	    }
        }

	if (insertchar) {
		/* take back the size of the matched reference */
       		parsed_file_chars -= yyleng; 
		temp_parse_buffer[parsed_file_chars++] = insertchar;
		/* set a flag showing the incoming buffer will be different */
		/* from the source buffer                                   */
		temp_parse_buffer_differs = 1;
		sgml_yylval.string[0] = insertchar;
		sgml_yylval.string[1] = '\0';
		return PCDATA_DATA;
	}


	sgml_yylval.hash_entry = Tcl_FindHashEntry(
			current_sgml_dtd->SGML_Entity_hash_tab,
			sgml_yylval.string);
	
	if (sgml_yylval.hash_entry != NULL)
		return ERO_REF;
	else { /* this must be an ampersand followed by a letter
		c = sgml_yylval.string; /* save old string ptr */
		sgml_yylval.string = strdup(yytext);
		if (c != NULL) FREE(c); 
		return PCDATA_DATA;	
	     }
	}

<document>[^<&\n]* { 
	sgml_yylval.string = strdup(yytext);
	return PCDATA_DATA;
	}

<document>{ero} {
	sgml_yylval.string = strdup("&");
        return PCDATA_DATA;
	}

<charref>"RE" {
	char_insert_size += yyleng;
	char_insert = (char)13; 
	return RE_DCL;}

<charref>"RS" {
	char_insert_size += yyleng;
	char_insert = (char)10; 
	return RS_DCL;}

<charref>"SPACE" {
	char_insert_size += yyleng;
	char_insert = (char)32; 
	return SPACE_DCL;}

<charref>[a-zA-Z][a-zA-Z0-9\-.]* { 
	char_insert_size = 0;
	return SGMLNAME;}

<charref>[0-9]* { 
	char_insert_size += yyleng;
	sgml_yylval.intval = char_insert = (char) atoi(yytext);
	return NUMBER; }

<charref>{refc} {
	if (char_insert_size > 0) { /* this ignors names of functions */
		/* take back the size of the matched reference */
       		parsed_file_chars -= (yyleng + char_insert_size) ; 
		temp_parse_buffer[parsed_file_chars++] = char_insert;
		/* set a flag showing the incoming buffer will be different */
		/* from the source buffer                                   */
		temp_parse_buffer_differs = 1;
	}
	BEGIN(document); 
	return REFC;
     }


<charref>{space} {
	if (char_insert_size > 0) { /* this ignors names of functions */
		/* take back the size of the matched reference */
       		parsed_file_chars -= (yyleng + char_insert_size) ; 
		temp_parse_buffer[parsed_file_chars++] = char_insert;
		/* set a flag showing the incoming buffer will be different */
		/* from the source buffer                                   */
		temp_parse_buffer_differs = 1;
	}
	BEGIN(document); 
	return REFC;
     }

<sgml_decl,sgml_doctype,sgml_attlist,sgml_element,sgml_entity,groups,mdo_start,sgml_shortref,sgml_usemap,sgml_notation,INITIAL,newinclude>{newline}  { 
	parsed_file_lineno++; /* this won't be accurate */}

.	{
	/* printf("MATCHING ANY CHAR...\n"); */
	 return yytext[0]; 
	}


<sgml_catalog>"PUBLIC" { return CATALOG_PUBLIC;}
 
<sgml_catalog>"ENTITY" { return CATALOG_ENTITY;}
 
<sgml_catalog>"DOCTYPE" { return CATALOG_DOCTYPE;}

<sgml_catalog>"LINKTYPE" { return CATALOG_LINKTYPE;}

<sgml_catalog>"NOTATION" { return CATALOG_NOTATION;}

<sgml_catalog>"OVERRIDE" { return CATALOG_OVERRIDE;}

<sgml_catalog>"SYSTEM" { return CATALOG_SYSTEM;}

<sgml_catalog>"SGMLDECL" { return CATALOG_SGMLDECL;}

<sgml_catalog>"DOCUMENT" { return CATALOG_DOCUMENT;}

<sgml_catalog>"CATALOG" { return CATALOG_CATALOG;}

<sgml_catalog>"BASE" { return CATALOG_BASE;}

<sgml_catalog>"DELEGATE" { return CATALOG_DELEGATE;}


<sgml_catalog>{param_lit}  {
	templen = yyleng - 2; 
	sgml_yylval.string = CALLOC(char, templen+1);
	strncpy(sgml_yylval.string, &yytext[1],templen);
	sgml_yylval.string[templen] = '\0';
        /* printf("cat name parsed as param_lit: %s\n", sgml_yylval.string); */
	return CATALOG_NAME;
	}

<sgml_catalog>{sysid_token}  {
	sgml_yylval.string = strdup(yytext);
	/* printf("cat name parsed as sysid_token: %s\n", sgml_yylval.string); */
	return CATALOG_NAME;
	}

<sgml_catalog>{entity_token}  {
	sgml_yylval.string = strdup(yytext);
	/* printf("cat name parsed as entity_token: %s\n", sgml_yylval.string); */
	return CATALOG_NAME;
	}



<sgml_catalog>^"--".*"--"$ { } /* do nothing -- eats comments */
<sgml_catalog>{ws} { }
<sgml_catalog>{newline} { }

<sgml_catalog><<EOF>> {
			yyterminate();
			return CATALOG_EOF; }


<newinclude><<EOF>> { /* this is needed -- don't know why */
#ifdef DEBUGPARSER
   fprintf(stdout,"<<EOF>> #1 encountered sgml_state %d\n",
	sgml_state); 
#endif
		active_sgml_entity = NULL;
		BEGIN(sgml_prev_state[--sgml_state]); 
        }

<<EOF>> { /* end of an input file */	

#ifdef DEBUGPARSER
   fprintf(stdout,"<<EOF>> encountered for include stack ptr %d\n",
	include_stack_ptr); 
#endif
	if (--include_stack_ptr < 0) {
		yyterminate();
		include_stack_ptr = 0;
		active_sgml_entity = NULL;
		return(ENTITY_END);
	}
	else {
		if (tmp_yyin != NULL 
		   && YY_CURRENT_BUFFER->yy_input_file != tmp_yyin) {
			fclose(YY_CURRENT_BUFFER->yy_input_file);
		}
		yy_delete_buffer(YY_CURRENT_BUFFER); 
		yy_switch_to_buffer(include_stack[include_stack_ptr]);
		yyin = include_stack[include_stack_ptr]->yy_input_file;
		BEGIN(sgml_prev_state[--sgml_state]); 
		active_sgml_entity = NULL;
		parsed_file_chars = save_parsed_file_chars;
#ifdef DEBUGPARSER
   fprintf(stdout,"<<EOF>> begin sgml_state = %d\n", sgml_state); 
#endif

		if (string_entity) {
#ifdef DEBUGPARSER
   fprintf(stdout,"<<EOF>> is for string entity\n"); 
#endif
			active_sgml_entity = NULL;
			return(END_INCLUDE);
		}

	        if (include_entity) {
#ifdef DEBUGPARSER
   fprintf(stdout,"<<EOF>> is for include entity\n"); 
#endif
		        if (include_stack_ptr == 0) 
				include_entity = 0;
			active_sgml_entity = NULL;
		        return(END_INCLUDE);
		}
		/* this seems to be required for system includes of  */
		/* things like DTD references -- the entity includes */
		/* don't need it --- probably a bug someplace        */
		sgml_push_back_token(sgml_yylval_include_back.tok_id); 
	        return(END_INCLUDE);
	}

	}

%%



int 
sgml_next_token()
{
  extern int document_tag_nesting;
  extern int force_document_end;
  int token;
  extern int sgml_parse_error;

  if (sgml_parse_error) {
	yyterminate();
	return(0);
  }
 

   if (document_tag_nesting == 0 && force_document_end == 1) {
	yyterminate();
	return(0);
   }

  /* check if there is a token pushed back or read ahead */
  if (sgml_yylval_pushback.tok_id != 0) {
        /* printf("PUSHED BACK TOKEN %d\n", sgml_yylval_pushback.tok_id); */
	sgml_yylval = sgml_yylval_pushback;
        sgml_yylval_pushback.tok_id = 0;
	return (sgml_yylval.tok_id);
   }

   /* otherwise read the next token */
   token = yylex();

   sgml_yylval.tok_id = token;
   return (token);
} 

int sgml_push_back_token(int token)
{
    sgml_yylval_pushback = sgml_yylval;
    sgml_yylval_pushback.tok_id = token;
}


/* this routine is called by the parser created by LLgen */
int sgml_message(int token)
{
   extern int sgml_symb;
   extern SGML_DTD *current_sgml_dtd;

#ifdef DEBUGPARSERMSG
   fprintf(stdout,"sgml_message called: %d\n", token); 
#endif

   switch (token) {
	case -1: 
#ifdef DEBUGPARSERMSG
		fprintf(stdout,"Supposed to be end of file??\n"); 
#endif
		break;
	
	case 0: 
#ifdef DEBUGPARSERMSG
          if (sgml_yylval.string != NULL)
		fprintf(stdout,
		  "token deleted? at line %d: sgml_symb = %d value %s \n",
		  parsed_file_lineno, sgml_symb, sgml_yylval.string); 
	  else
		fprintf(stdout,
		  "token deleted? at line %d: sgml_symb = %d \n",
		  parsed_file_lineno, sgml_symb); 
		
                /* cheshire_exit(1); */
#endif
		break;
	default: 
#ifdef DEBUGPARSERMSG
          if (sgml_yylval.string != NULL)
		fprintf(stdout,"token pushed back at line %d: sgml_symb = %d value %s \n",
			parsed_file_lineno, sgml_symb, sgml_yylval.string);  
	  else
		fprintf(stdout,"token pushed back at line %d: sgml_symb = %d\n",
			parsed_file_lineno, sgml_symb);  
#endif                
		sgml_push_back_token(token);
		break;
   }

}


char *build_full_name( char *in_name)
{
  char *path_name = NULL; 
  char *getcwd();
  char *full_name = NULL;
  char *c;
  int path_len = 0;
  int name_len = 0;
  struct stat filestatus, filestatus2;
  int statresult, statresult2;

        name_len = strlen(in_name);

        if (in_name[0] == '/') /* assume it is already a path name */
		return (strdup(in_name));

        /* Under NT must handle 'C:\...' type names */
        if (in_name[1] == ':' 
	    && (in_name[2] == '\\' || in_name[2] == '/'))
		return (strdup(in_name));

	/* for some XML there may be a URL form file name */
	if (strncasecmp(in_name,"file:///",8) == 0) {
	  full_name = CALLOC(char, strlen(in_name));
#ifdef WIN32
	  strcpy(full_name,(current_sgml_document->file_name+8));
#else
	  strcpy(full_name,(current_sgml_document->file_name+7));
#endif
	  return (full_name);
	}
	if (strncasecmp(in_name,"http://",7) == 0) {
	  /* we aren't doing full URL fetching, but look for the file */
	  c = strrchr(in_name, '/');		
	  if (c) {
	    /* use the last part of the URL as a new call */
	    return(build_full_name(c+1));
          } else {
	    /* no slashes in name??? */
	    return(in_name);
   	  }
	}
	/* 
         * The name is not a full path name... so...
         */
	if (current_sgml_document != NULL 
	   && current_sgml_document->file_name != NULL) {
	  /* if this is set, it should be the real data file */
	  /* we will use it as the path name for the default */
          /* directory for this file                         */
	  full_name = CALLOC(char, (strlen(current_sgml_document->file_name)
					 + name_len + 10));
	  strcpy(full_name,current_sgml_document->file_name);
	  c = strrchr(full_name, '/');
	  if (c) {
	    strcpy(c+1,in_name);
          } else {
	    /* no slashes in name??? */
	    strcpy(full_name, in_name);
   	  }
	  
	} else {
	  /* otherwise assume that the name only is provided   */
	  /* and that we may not be doing a doc parse, but dtd */
	  /* first try to get things from the same directory   */
          path_name = getcwd (NULL, 500);
          path_len = strlen(path_name);

          full_name = CALLOC(char, (path_len + name_len + 10));
#ifdef WIN32
          sprintf(full_name,"%s\\%s",path_name,in_name);
#else
          sprintf(full_name,"%s/%s",path_name,in_name);
#endif

	  if (stat(full_name, &filestatus) != 0) {
	    /* assume that the file did not exist */
	    /* so we will try the current DTD's directory */
	    FREE(full_name); 

	    path_name = current_sgml_dtd->DTD_file_name;
	    path_len = strlen(path_name);
	    
	    full_name = CALLOC(char, (path_len + name_len + 10));

	    strcpy(full_name, path_name);
	    c = strrchr(full_name, '/');
	    if (c) {
	      strcpy(c+1,in_name);
	    } else {
	      /* no slashes in name -- try backslash for NT */
	      c = strrchr(full_name, '\\');
	      if (c) {
		strcpy(c+1,in_name);
	      } 
	      else /* just send back the input name */
		strcpy(full_name, in_name);
	    }
	  }	  
	}

	return(full_name);
}


int include_buffer_switch (char *new_file_name)
{
  char *path_name = NULL, *tmp = NULL;
  extern int sgml_symb;

	if (include_stack_ptr >= MAX_INCLUDE_DEPTH) {
		fprintf(LOGFILE,"Too deeply nested on SYSTEM include\n"); 
		cheshire_exit(1);
	}
#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"adding include stack entry: %d\n", include_stack_ptr); 
#endif
	include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;

   	/* assume that the last token was the name */

        path_name = build_full_name(new_file_name);

	yyin = fopen(path_name,"r");

        if (!yyin) { /* try adding default extension .dtd */
		tmp = CALLOC(char, strlen(path_name)+5);
		strcpy(tmp, path_name);
		strcat(tmp, ".dtd");
		FREE(path_name);
		path_name = tmp;
		yyin = fopen(path_name,"r");
	}

	if (!yyin) {
		fprintf(LOGFILE,
		      "SGML_SCAN: Could not open SYSTEM include '%s'\n",
			new_file_name); 
		perror(NULL);

		cheshire_exit(1);
	}

#ifdef DEBUGPARSERSWITCH
	printf("opened include file %s yyin = %x\n", path_name, yyin);	
#endif

	yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));

	sgml_prev_state[sgml_state++] = YY_START;
#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"include buffer switch: sgml_state = %d\n", sgml_state); 
#endif

	BEGIN(newinclude);
	
        sgml_yylval_include_back.tok_id = sgml_symb;

	FREE(path_name);
}


int string_buffer_switch (char *new_contents, int nested)
{
	YY_BUFFER_STATE string_buffer;
	int new_content_len;

#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"Switching to string buffer for string entity\n"); 
#endif

	if (new_contents == NULL) {
		fprintf(LOGFILE, "Null contents in string buffer switch\n");
		cheshire_exit(1);
	}

	new_content_len = strlen(new_contents);

	if (include_stack_ptr >= MAX_INCLUDE_DEPTH) {
		fprintf(LOGFILE,"Too deeply nested on SYSTEM include\n"); 
		cheshire_exit(1);
	}
	
	if (nested)
		include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;

#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"include stack ptr changed to %d\n", include_stack_ptr); 
#endif

	if (!tmp_yyin) {
#ifndef WIN32
		tmp_yyin = tmpfile();
#else
		tmp_yyin = fopen("C:\\TEMP\\DUMMY","w+");
#endif
		if (!tmp_yyin) {
			perror("Couldn't open tmpfile in sgml_scan");
			cheshire_exit(1);
	        }
	}


	string_buffer = yy_create_buffer(tmp_yyin,new_content_len+3);

	/* copy data to input buffer */
	strcpy(string_buffer->yy_ch_buf, new_contents);
	string_buffer->yy_ch_buf[new_content_len] = '\0';
	string_buffer->yy_ch_buf[new_content_len+1] = '\0';

	/* set current position in input buffer to beginning */
	string_buffer->yy_buf_pos = string_buffer->yy_ch_buf;

	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
 	/* string_buffer->yy_buf_size; should be OK already */

	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	string_buffer->yy_n_chars = new_content_len;

	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	string_buffer->yy_fill_buffer = 0;  

	/* Whether we've seen an EOF on this buffer. */
	/* string_buffer->yy_eof_status = 1; */

	/* #define EOF_PENDING 1 */
	/* #define EOF_NOT_SEEN 0 */
	/* "Pending" happens when the EOF has been seen but there's still
	 * some text to process.  Note that when we actually see the EOF,
	 * we switch the status back to "not seen" (via sgml_restart()), so
	 * that the user can continue scanning by just pointing yyin at
	 * a new input file.
	 */

	/* switch scanning to the new buffer */
	yy_switch_to_buffer(string_buffer);
	
	string_entity = 1;
	/* save state, but no need to change it     */
	/* this is so it can be "restored" at "EOF" */
	sgml_prev_state[sgml_state++] = YY_START;
#ifdef DEBUGPARSER
   fprintf(stdout,"String entity switch sgml_state = %d\n", sgml_state); 
#endif


}


start_document_parsing () 
{
  extern int document_tag_nesting;
  extern int force_document_end;
  extern int sgml_parse_error;
  extern SGML_Processing_Inst *current_pi, *last_pi;



        current_pi = NULL;	      
        last_pi = NULL;	      
	parsed_file_lineno = 1;
	parsed_file_chars = 0;
	document_tag_nesting = 0;
	force_document_end = 0;
	sgml_parse_error = 0;
	sgml_state = 0;
	include_stack_ptr = 0;
	sgml_yylval_pushback.tok_id = 0;
	
	BEGIN(document_start);

}


start_document_proper () {
	/* any prolog should have been taken care of... */
	BEGIN(document);
}

start_dtd_parsing () 
{
  extern int document_tag_nesting;
  extern int force_document_end;
  extern int sgml_parse_error;

	parsed_file_lineno = 1;
	parsed_file_chars = 0;
	document_tag_nesting = 0;
	force_document_end = 0;
	sgml_parse_error = 0;
	sgml_state = 0;
	include_stack_ptr = 0;

	BEGIN(INITIAL);

}


start_schema_parsing () 
{
  extern int document_tag_nesting;
  extern int force_document_end;
  extern int sgml_parse_error;
  extern SGML_Processing_Inst *current_pi, *last_pi;



        current_pi = NULL;	      
        last_pi = NULL;	      
	parsed_file_lineno = 1;
	parsed_file_chars = 0;
	document_tag_nesting = 0;
	force_document_end = 0;
	sgml_parse_error = 0;
	sgml_state = 0;
	include_stack_ptr = 0;
	sgml_yylval_pushback.tok_id = 0;
	
	BEGIN(document_start);

}

start_catalog_parsing () 
{
  extern int document_tag_nesting;
  extern int force_document_end;
  extern int sgml_parse_error;
	parsed_file_lineno = 1;
	parsed_file_chars = 0;
	document_tag_nesting = 0;
	force_document_end = 0;
	sgml_parse_error = 0;
	sgml_state = 0;
	include_stack_ptr = 0;

	BEGIN(sgml_catalog);

}

/* switch input to the entity referenced */
switch_input_to_entity (SGML_Entity *ent)
{
  
  save_parsed_file_chars = parsed_file_chars;
  /* this gets set back to the saved value at the end of the include */
  parsed_file_chars = 0;

#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"Entity Text type = : %d\n", ent->entity_text_type); 
#endif

  if (ent->entity_text_type == EXTERNAL_ENTITY_SPEC ) {
	if (ent->system_name != NULL) {
		include_buffer_switch (ent->system_name);
#ifdef DEBUGPARSERSWITCH
   fprintf(stdout,"Switched input to system file: %s\n", ent->system_name); 
#endif

	}
	else if (ent->public_name != NULL) {
		/* include_buffer_switch (ent->public_name); */
		fprintf(LOGFILE, "Public Entity Name with no matching catalog entry: \n'%s'\n", ent->public_name);
#ifdef DEBUGPARSERSWITCH
	fprintf(stdout, "Public Entity Name with no matching catalog entry: \n'%s'\n", ent->public_name);
#endif

	}
  }
  else if (ent->entity_text_type == PARAMETER_LIT_TEXT 
	   || ent->entity_text_type == DATA_TEXT
	   || ent->entity_text_type == BRACKETED_TEXT) {
#ifdef DEBUGPARSERSWITCH
	fprintf(stdout, "String Entity Contents: '%s'\n", ent->entity_text_string);
#endif

	string_buffer_switch(ent->entity_text_string,1 /* this is nested */);
  }
  include_entity = 1;
}

int yywrap () 
{
  return (1); 
}

int scan_backup() {
	BEGIN(document);
}

int force_empty_end_tag() {
	BEGIN(empty_tag_contents);
}

int clean_up_scan() {
  if (YY_CURRENT_BUFFER) {
     yy_delete_buffer(YY_CURRENT_BUFFER);
     /* YY_CURRENT_BUFFER = NULL; */
     yy_init = 1;
  }
  
}
