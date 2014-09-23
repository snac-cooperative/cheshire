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
*	Header Name:	q_scan.flex
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	lexical parser for the z3950 query language
*
*	Usage:		token = z__lex(); -- after using flex
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		9-6-94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
%{
/* not in solaris #include <strings.h> */
#include <stdlib.h>
#include <string.h>
#include "z__pars.h"
#include "z_parse.h"
#include "z_parameters.h"
#include <dmalloc.h>

#ifdef WIN32
#define strcasecmp _stricmp
#endif

int prev_z__state[500];
int z__state = 0;
int z__lineno = 1;
int paren_nesting = 0;
int attrnum, attrval;
char strattrval[500];

struct Z__LVAL { 
        int  tok_id;
        char *string;
        int intval;
	int attr_set;
	int useval;
	int relval;
	int posval;
	int structval;
	int truncval;
	int compval;
	int other_attr_type;
	int other_attr_val;
 } z__lval, z__lval_pushback;

int parse_attribute_set; /* this is set in queryparse */


/* set up for string input */
#undef YY_INPUT
#define YY_INPUT(b, r, ms)  (r = z__yyinput(b, ms))

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifdef DMALLOC_INCLUDE
#define YY_MALLOC_DECL
#endif


%}

/* Definitions */

DIGIT  [0-9]
DIGITS [0-9]+	
LETTER [a-zA-Z]


ws  [ \t\n]+
nl  \n
lt  "<"
lteq "<="
equal "="
gteq  ">="
gt    ">"
slash "/"

phonetic "??"
stem  "%"
relevant "@"
trunc "#"
notrunc "!"
within "<=>"
dashes "-""-"*

value_token [a-zA-Z0-9/\200-\377][a-zA-Z0-9\200-\377\-_,.:*/?]* 
/* value_token [[:alnum:]][[:alnum:]\-_,.:/]*  -- Posix doesn't work... */
use_token [0-9]*
quoted_string (\'[^']*\')|(\"[^"]*\")|(\{[^}]*\})
phrase_string (\$[^$]*\$)
number_string [0-9\-][0-9.]*
attrib [0-9]+[ \t\n]*"="[ \t\n]*[0-9]+
strattrib [0-9]+[ \t\n]*"="[ \t\n]*(('[^']*')|(\"[^"]*\"))
partattrib [0-9]+[ \t\n]*"="[ \t\n]*
attrsetoid "1.2.840.10003.3."[0-9.]*

/* Start conditions */
%x relop
%s val
%x boolean
%x newindex
%x attributespec
%x resultsetspec
%x proxspec

%%

<INITIAL,newindex>{use_token}  { int indval;
	z__lval.string = strdup(z__text);
	/* check for index name*/
	if ((indval = z__num_is_Index(atoi(z__lval.string),
		parse_attribute_set)) >= 0) {
		 /* attribute sets value */
		z__lval.attr_set = (int) 
			all_indexes[indval].attributes[0];
		 /* Use value */
		z__lval.intval = z__lval.useval = (int) 
			all_indexes[indval].attributes[1];
		/* relation value from table */
		z__lval.relval = (int) 
			all_indexes[indval].attributes[2];
		/* position value from table */
		z__lval.posval = (int) 
			all_indexes[indval].attributes[3];
		/* structure value from table */
		z__lval.structval = (int) 
			all_indexes[indval].attributes[4];
		/* truncation value from table */
		z__lval.truncval = (int) 
			all_indexes[indval].attributes[5];
		/* completeness value from table */
		z__lval.compval = (int) 
			all_indexes[indval].attributes[6];
		BEGIN(relop);
		return INDEXNAME;
	}	
	else { /* check of this is a resultsetname */
		   BEGIN(boolean);
		   return RESULTSETID;
	}

}

<INITIAL,newindex>{value_token}  { int indval;
	z__lval.string = strdup(z__text);
	/* check for index name*/
	if ((indval = z__is_Index(z__text,parse_attribute_set)) >= 0) {
		 /* attribute sets value */
		z__lval.attr_set = (int) 
			all_indexes[indval].attributes[0];
		 /* Use value */
		z__lval.intval = z__lval.useval = (int) 
			all_indexes[indval].attributes[1];
		/* relation value from table */
		z__lval.relval = (int) 
			all_indexes[indval].attributes[2];
		/* position value from table */
		z__lval.posval = (int) 
			all_indexes[indval].attributes[3];
		/* structure value from table */
		z__lval.structval = (int) 
			all_indexes[indval].attributes[4];
		/* truncation value from table */
		z__lval.truncval = (int) 
			all_indexes[indval].attributes[5];
		/* completeness value from table */
		z__lval.compval = (int) 
			all_indexes[indval].attributes[6];
		BEGIN(relop);
		return INDEXNAME;
	}	
	else {
	        /* check for the string as a resultsetid */
	      if (is_string_result_set(z__lval.string)) {
		   BEGIN(boolean);
		   return RESULTSETID;
              }
              else { /* pass the string back as an possible local index */
		 /* Use value */
		z__lval.intval = -1;
		z__lval.useval = 0;
		z__lval.relval = 0;
		z__lval.posval = 0;
		z__lval.structval = 6;
		z__lval.truncval = 0;
		z__lval.compval = 0;
		BEGIN(relop);
		return INDEXNAME;
              }
	}
}
<INITIAL,newindex>{quoted_string}  { int templen;
      /* check for the string as a resultsetid */
      if (is_string_result_set(z__text)) {
	templen = yyleng - 2; 
	z__lval.string = CALLOC(char, templen+1);
	strncpy(z__lval.string, &yytext[1],templen);
	z__lval.string[templen] = '\0';
        BEGIN(boolean);
	return RESULTSETID;
      }
      else {
      }

}
<INITIAL,newindex>"[" { 
		z__lval.string = NULL;
		z__lval.intval = 0;
		z__lval.useval = 0;
		z__lval.relval = 0;
		z__lval.posval = 0;
		z__lval.structval = 0;
		z__lval.truncval = 0;
		z__lval.compval = 0;
		BEGIN(relop);
		yyless(0);
		return INDEXNAME;}

<INITIAL,newindex>"("   { paren_nesting++; return '('; }

<val,boolean>"AND"|".AND."|"&&" { BEGIN(newindex); return AND; }
<val,boolean>"OR"|".OR."|"||"  { BEGIN(newindex); return OR;}
<val,boolean>"NOT"|".NOT."|"ANDNOT"|".ANDNOT."  { BEGIN(newindex); return NOT;}
<val,boolean>"!FUZZY" {
	z__lval.useval = 0;
	z__lval.compval = 0;
	z__lval.intval = 0;
	z__lval.truncval = 1;
	BEGIN(proxspec);
	return(PROXOP);
} 

<val,boolean>"!RESTRICT" {
	z__lval.useval = 0;
	z__lval.compval = 0;
	z__lval.intval = 0;
	z__lval.truncval = 1;
	BEGIN(proxspec);
	return(PROXOP);
} 

<val,boolean>"!MERGE" {
	z__lval.useval = 0;
	z__lval.compval = 0;
	z__lval.intval = 0;
	z__lval.truncval = 1;
	BEGIN(proxspec);
	return(PROXOP);
} 

<val,boolean>"!PROX" { 
	z__lval.useval = 0;
	z__lval.compval = 2;
	z__lval.intval = 0;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!OPROX" {
	z__lval.useval = 0;
	z__lval.compval = 2;
	z__lval.intval = 1;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!ADJ" { 
	z__lval.useval = 0;
	z__lval.compval = 2;
	z__lval.intval = 0;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!OADJ" {
	z__lval.useval = 0;
	z__lval.compval = 2;
	z__lval.intval = 1;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!BEFORE" {
	z__lval.useval = 0;
	z__lval.compval = 2;
	z__lval.intval = 1;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!NEAR" {
	z__lval.useval = 0;
	z__lval.compval = 20;
	z__lval.intval = 0;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!ONEAR" {
	z__lval.useval = 0;
	z__lval.compval = 20;
	z__lval.intval = 1;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!FAR" {
	z__lval.useval = 1;
	z__lval.compval = 20;
	z__lval.intval = 0;
	BEGIN(proxspec);
	return(PROXOP);
}
<val,boolean>"!OFAR" {
	z__lval.useval = 1;
	z__lval.compval = 20;
	z__lval.intval = 1;
	BEGIN(proxspec);
	return(PROXOP);
}

<proxspec>{ws} { /* end of proximity spec */
        yyless(0); /* rescan it */
	BEGIN(newindex);
	return(PROXEND);
}

<proxspec>"/C" {
	z__lval.intval = 1;
	return(PROXTYPE);
}
<proxspec>"/CHAR" {
	z__lval.intval = 1;
	return(PROXTYPE);
}
<proxspec>"/CHARACTERS" {
	z__lval.intval = 1;
	return(PROXTYPE);
}
<proxspec>"/CHARACTER" {
	z__lval.intval = 1;
	return(PROXTYPE);
}
<proxspec>"/W" {
	z__lval.intval = 2;
	return(PROXTYPE);
}
<proxspec>"/WORD" {
	z__lval.intval = 2;
	return(PROXTYPE);
}
<proxspec>"/WORDS" {
	z__lval.intval = 2;
	return(PROXTYPE);
}
<proxspec>"/S" {
	z__lval.intval = 3;
	return(PROXTYPE);
}
<proxspec>"/SENT" {
	z__lval.intval = 3;
	return(PROXTYPE);
}
<proxspec>"/SENTENCE" {
	z__lval.intval = 3;
	return(PROXTYPE);
}
<proxspec>"/SENTENCES" {
	z__lval.intval = 3;
	return(PROXTYPE);
}
<proxspec>"/P" {
	z__lval.intval = 4;
	return(PROXTYPE);
}
<proxspec>"/PARA" {
	z__lval.intval = 4;
	return(PROXTYPE);
}
<proxspec>"/PARAGRAPH" {
	z__lval.intval = 4;
	return(PROXTYPE);
}
<proxspec>"/PARAGRAPHS" {
	z__lval.intval = 4;
	return(PROXTYPE);
}
<proxspec>"/SECTION" {
	z__lval.intval = 5;
	return(PROXTYPE);
}
<proxspec>"/SECTIONS" {
	z__lval.intval = 5;
	return(PROXTYPE);
}
<proxspec>"/CHAPTER" {
	z__lval.intval = 6;
	return(PROXTYPE);
}
<proxspec>"/CHAPTERS" {
	z__lval.intval = 6;
	return(PROXTYPE);
}
<proxspec>"/DOC" {
	z__lval.intval = 7;
	return(PROXTYPE);
}
<proxspec>"/DOCUMENT" {
	z__lval.intval = 7;
	return(PROXTYPE);
}
<proxspec>"/ELEMENT" {
	z__lval.intval = 8;
	return(PROXTYPE);
}
<proxspec>"/SUBELEMENT" {
	z__lval.intval = 9;
	return(PROXTYPE);
}
<proxspec>"/ELEMENTTYPE" {
	z__lval.intval = 10;
	return(PROXTYPE);
}
<proxspec>"/BYTE" {
	z__lval.intval = 11;
	return(PROXTYPE);
}
<proxspec>"_AND" {
	z__lval.intval = 91;
	return(PROXTYPE);
}
<proxspec>"_OR" {
	z__lval.intval = 92;
	return(PROXTYPE);
}
<proxspec>"_NOT" {
	z__lval.intval = 93;
	return(PROXTYPE);
}
<proxspec>"_FROM" {
	z__lval.intval = 94;
	return(PROXTYPE);
}
<proxspec>"_TO" {
	z__lval.intval = 95;
	return(PROXTYPE);
}
<proxspec>"_SUM" {
	z__lval.intval = 96;
	return(PROXTYPE);
}
<proxspec>"_MEAN" {
	z__lval.intval = 97;
	return(PROXTYPE);
}
<proxspec>"_NORM" {
	z__lval.intval = 98;
	return(PROXTYPE);
}
<proxspec>"_CMBZ" {
	z__lval.intval = 99;
	return(PROXTYPE);
}
<proxspec>"_NSUM" {
	z__lval.intval = 100;
	return(PROXTYPE);
}
<proxspec>"_NPRV" {
	z__lval.intval = 101;
	return(PROXTYPE);
}
<proxspec>"_PIVT" {
	z__lval.intval = 102;
	return(PROXTYPE);
}
<proxspec>"_PVT" {
	z__lval.intval = 102;
	return(PROXTYPE);
}
<proxspec>"_PIVOT" {
	z__lval.intval = 102;
	return(PROXTYPE);
}
<proxspec>"_HYBRID" {
	z__lval.intval = 103;
	return(PROXTYPE);
}

<proxspec>"/"[0-9]+ {
	sscanf(z__text, "/%d", &z__lval.intval); 
	return(PROXDIST);
}

<val,boolean>"RESULTSETID" {BEGIN(resultsetspec); return RESULTSETSPEC;}
<val,boolean>"RESULTSET" {BEGIN(resultsetspec); return RESULTSETSPEC;}
<val,boolean>"SETID" {BEGIN(resultsetspec); return RESULTSETSPEC;}

<relop>">=<"|".OVERLAPS." { return GEO_OVERLAPS;}
<relop>">#<"|".FULLY_ENCLOSED_WITHIN." { return GEO_FULLY_ENCLOSED_WITHIN;}
<relop>"<#>"|".ENCLOSES." { return GEO_ENCLOSES;}
<relop>"<>#"|".OUTSIDE_OF." { return GEO_FULLY_OUTSIDE_OF;}
<relop>"+-+"|".NEAR." { return GEO_NEAR;}
<relop>".#."|".MEMBERS_CONTAIN." { return GEO_MEMBERS_CONTAIN;}
<relop>"!.#."|".MEMBERS_NOT_CONTAIN." { return GEO_MEMBERS_NOT_CONTAIN;}
<relop>"@>=<"|".OVERLAPS_RANK." { return GEO_OVERLAPS_RANK;}
<relop>"@>#<"|".FULLY_ENCLOSED_WITHIN_RANK." { return GEO_FULLY_ENCLOSED_WITHIN_RANK;}
<relop>"@<#>"|".ENCLOSES_RANK." { return GEO_ENCLOSES_RANK;}
<relop>":<:"|".BEFORE." { return TIME_BEFORE;}
<relop>":<=:"|".BEFORE_OR_DURING." { return TIME_BEFORE_OR_DURING;}
<relop>":=:"|".DURING." { return TIME_DURING;}
<relop>":>=:"|".DURING_OR_AFTER." { return TIME_DURING_OR_AFTER;}
<relop>":>:"|".AFTER." { return TIME_AFTER;}

<relop>"@+"|".OKAPI." { return OKAPI_RANK;}
<relop>"@#"|".CORI." { return CORI_RANK;}
<relop>"@@"|".TREC2." { return TREC2_RANK;}
<relop>"@*"|".TREC2FBK." { return TREC2_FBK_RANK;}
<relop>"@/"|".TFIDF." { return TFIDF_RANK;}
<relop>"@&"|".TFIDFLUCENE."|".LUCENE." { return TFIDF_LUCENE_RANK;}
<relop>"@!"|".LANGMOD."|".LM." { return LANGMOD_RANK;}
<relop>"+"|".NGRAM_FULL." {return NGRAM_FULL;}

<relop>".WITHIN."|{within}      { return WITHIN;}
<relop>"LT"|".LT."|{lt}              { return LESS_THAN;}
<relop>"LE"|".LE."|{lteq}            { return LESS_THAN_OR_EQUAL;}
<relop>"EQ"|".EQ."|"="               { return EQUAL;}
<relop>"GE"|".GE."|{gteq}            { return GREATER_THAN_OR_EQUAL;}
<relop>"GT"|".GT."|{gt}              { return GREATER_THAN;}
<relop>"NE"|".NE."|"<>"|"!="         { return NOT_EQUAL;}
<relop>".PHON."|".PHONETIC."|{phonetic}  { return PHONETIC;}
<relop>".STEM."|{stem}        { return STEM;}
<relop>"REL"|".REL."|{relevant}      { return RELEVANCE;}

<relop>"[" { BEGIN (attributespec); return START_ATTRIBUTES;}

<attributespec>{attrsetoid} {
	z__lval.string = strdup(z__text); 
	return ATTRSETOID;
}
<attributespec>"BIB1"|"BIB-1" {
	z__lval.string = strdup(OID_BIB1); 
	return ATTRSETOID;
}
<attributespec>"EXP1"|"EXP-1"|"EXP" {
	z__lval.string = strdup(OID_EXP1); 
	return ATTRSETOID;
}
<attributespec>"EXT1"|"EXT-1"|"EXT" {
	z__lval.string = strdup(OID_EXT1); 
	return ATTRSETOID;
}
<attributespec>"CCL1"|"CCL-1"|"CCL1"|"CCL" {
	z__lval.string = strdup(OID_CCL1); 
	return ATTRSETOID;
}
<attributespec>"GILS" {
	z__lval.string = strdup(OID_GILS); 
	return ATTRSETOID;
}
<attributespec>"STAS" {
	z__lval.string = strdup(OID_STAS); 
	return ATTRSETOID;
}
<attributespec>"GEO" {
	z__lval.string = strdup(OID_GEO); 
	return ATTRSETOID;
}

<attributespec>"COLLECTIONS_1"|"COLLECTIONS-1"|"COLLECTIONS1"|"COLLECTIONS" {
	z__lval.string = strdup(OID_COLLECTIONS1);
	return ATTRSETOID;
}
<attributespec>"CIMI_1"|"CIMI-1"|"CIMI1"|"CIMI" {
	z__lval.string = strdup(OID_CIMI1);
	return ATTRSETOID;
}
<attributespec>"ZBIG" {
        z__lval.string = strdup(OID_ZBIG);
	return ATTRSETOID;
}
<attributespec>"UTIL" {
        z__lval.string = strdup(OID_UTIL);
	return ATTRSETOID;
}
<attributespec>"XD_1"|"XD-1"|"XD1"|"XD" {
        z__lval.string = strdup(OID_XD1);
	return ATTRSETOID;
}
<attributespec>"ZTHES" {
        z__lval.string = strdup(OID_ZTHES);
	return ATTRSETOID;
}
<attributespec>"FIN_1"|"FIN-1"|"FIN1"|"FIN" {
        z__lval.string = strdup(OID_FIN1);
	return ATTRSETOID;
}
<attributespec>"DAN_1"|"DAN-1"|"DAN1"|"DAN" {
        z__lval.string = strdup(OID_DAN1);
	return ATTRSETOID;
}
<attributespec>"HOLDINGS" {
        z__lval.string = strdup(OID_HOLD);
	return ATTRSETOID;
}

<attributespec>{attrib} {
	attrnum = 0;
	attrval = 0;
	sscanf(z__text, "%d = %d", &attrnum, &attrval);
        /* printf("attrnum = %d attrval = %d\n", attrnum, attrval);*/
	switch (attrnum) {
	case 1: /* use attribute */
		z__lval.intval = z__lval.useval = attrval;
		return USE_ATTR;
		break;
	case 2: /* relation attr */
		z__lval.relval = attrval;
		return REL_ATTR;
		break;
	case 3: /* position attr */
		z__lval.posval = attrval;
		return POS_ATTR;
		break;
	case 4: /* Structure attr */
		z__lval.structval = attrval;
		return STRUCT_ATTR;
		break;
	case 5:	/* truncation attr */
		z__lval.truncval = attrval;
		return TRUNC_ATTR;
		break;
	case 6:	/* completeness attr */
		z__lval.compval = attrval;
		return COMP_ATTR;
		break;
	case 0: /* error in parsing */
                z__lval.compval = 999999;
		return OTHER_ATTR;
		break;
	default: /* not in the 1-6 range */
		 /* must be new extended attribute arch */
	        z__lval.other_attr_type = attrnum;
		z__lval.other_attr_val = attrval;
		return OTHER_ATTR;	    
	}
	
	
    }

<attributespec>{partattrib} {
	attrnum = 0;
	attrval = 999999; /* we use this for an error signal */
	sscanf(z__text, "%d =", &attrnum);
	/* printf("PARTIAL attrnum = %d attrval = %d\n", attrnum, attrval); */
	switch (attrnum) {
	case 1: /* use attribute */
		z__lval.intval = z__lval.useval = attrval;
		return USE_ATTR;
		break;
	case 2: /* relation attr */
		z__lval.relval = attrval;
		return REL_ATTR;
		break;
	case 3: /* position attr */
		z__lval.posval = attrval;
		return POS_ATTR;
		break;
	case 4: /* Structure attr */
		z__lval.structval = attrval;
		return STRUCT_ATTR;
		break;
	case 5:	/* truncation attr */
		z__lval.truncval = attrval;
		return TRUNC_ATTR;
		break;
	case 6:	/* completeness attr */
		z__lval.compval = attrval;
		return COMP_ATTR;
		break;
	case 0: /* error in parsing */
                z__lval.compval = 999999;
		return OTHER_ATTR;
		break;
	default: /* not in the 1-6 range */
		 /* must be new extended attribute arch */
	        z__lval.other_attr_type = attrnum;
		z__lval.other_attr_val = attrval;
		return OTHER_ATTR;	    
	}
	
	
    }


<attributespec>{strattrib} {
	sscanf(z__text, "%d = %s", &attrnum, &strattrval);

	switch (attrnum) {
	case 1: /* use attribute */
		z__lval.intval = z__lval.useval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return USE_ATTR;
		break;
	case 2: /* relation attr */
		z__lval.relval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return REL_ATTR;
		break;
	case 3: /* position attr */
		z__lval.posval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return POS_ATTR;
		break;
	case 4: /* Structure attr */
		z__lval.structval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return STRUCT_ATTR;
		break;
	case 5:	/* truncation attr */
		z__lval.truncval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return TRUNC_ATTR;
		break;
	case 6:	/* completeness attr */
		z__lval.compval = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return COMP_ATTR;
		break;
	default: /* not in the 1-6 range */
		 /* must be new extended attribute arch */
	        z__lval.other_attr_type = attrnum;
		z__lval.other_attr_val = -1;
		z__lval.string = strdup(strattrval+1);
		z__lval.string[strlen(z__lval.string)-1] = '\0';
		return OTHER_ATTR;	    
	}
	
	
    }

<attributespec>"," /* ignore commas */;

<attributespec>"]" { BEGIN (relop); return END_ATTRIBUTES;}

<relop,val>{quoted_string}  {
	int templen;

/* printf("QUOTED STRING????\n"); */
	if (YY_START == relop)
	   BEGIN(val);
	templen = yyleng - 2; 
	z__lval.string = CALLOC(char, templen+1);
	strncpy(z__lval.string, &yytext[1],templen);
	z__lval.string[templen] = '\0';
	return VALUE;
	}


<relop,val>{number_string}  {
	int templen;
	z__lval.string = strdup(z__text); 
	if (YY_START == relop)
	     BEGIN(val);
	return VALUE;
	}


<relop,val>{value_token} { 
	/* printf ("VALUE_TOKEN\n"); */
	z__lval.string = strdup(z__text); 
	if (YY_START == relop)
	     BEGIN(val);
	     return VALUE;
	}

<relop,val>{phrase_string} { 
	z__lval.string = strdup(z__text); 
	if (YY_START == relop)
	     BEGIN(val);
	     return VALUE;
	}

<val>{trunc} {return TRUNC;}

<val>{notrunc} {return NOTRUNC;}

<val>"("   { paren_nesting--; return ')'; }

<resultsetspec>{value_token}  { 
	z__lval.string = strdup(z__text); 
	BEGIN(INITIAL);
	return RESULTSETNAME;
	}

<*>'\n'  { z__lineno++;}

<*>{ws}     /* ignore whitespace */;

<*>{dashes} /* ignore double+ dashes */;

<*>{slash} /* ignore slash */;

<*>.	{return z__text[0];}



%%


int z__wrap () 
{
  /* printf("z__wrap -- End of input \n"); */
  BEGIN(INITIAL);
  return (1); 
}

int reset_scanner()
{
  BEGIN(INITIAL);
}


/* set up the scanner to read from the z__inputdata string */
char *z__inputdata; /* query buffer for scanning */
char *z__inputptr; /* current position in z__inputdata */
char *z__inputlim;  /* end of data */

int 
z__yyinput( char *buf, int max_size) {
   int n, chars_left;


   chars_left = z__inputlim - z__inputptr;
   n = (max_size < chars_left)? max_size : chars_left;
	
   /* printf ("z__yyinput: buf is '%s' chars_left is %d n is %d\n", buf, chars_left,n); */

   if (n > 0) {
	memcpy(buf,z__inputptr, n);
	z__inputptr += n;
   }
   if (n < 0) 
	n = 0;

	return n;
}

int z__is_Index(char *iname, int attr_set_id) {
   int i;
   for (i = 0; all_indexes[i].name[0] != 0; i++) {
	if (strcasecmp(all_indexes[i].name, iname) == 0) {
		if ((all_indexes[i].attributes[0] & attr_set_id)
		    == attr_set_id)
		return(i);
	}
   }
   /* if it wasn't found ... */
   return (-1);
}

int z__num_is_Index(int index, int attr_set_id) {
   int i;
   for (i = 0; all_indexes[i].name[0] != 0; i++) {
	if (all_indexes[i].attributes[1] == index) {
		if ((all_indexes[i].attributes[0] & attr_set_id)
		    == attr_set_id)
		return(i);
	}
   }
   /* if it wasn't found ... */
   return (-1);
}

int clean_up_query_parse() {
  yy_delete_buffer(YY_CURRENT_BUFFER);
  YY_CURRENT_BUFFER = NULL;
  parser_cleanup ();
  return(0);
}
