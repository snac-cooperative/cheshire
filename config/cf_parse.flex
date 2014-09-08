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
*	Header Name:	cf_parse.flex
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	lexical parser for config files
*
*	Usage:		token = yylex(); -- after using flex
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7-15-94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
%{
#include <stdlib.h>
#ifndef WIN32
#ifndef SOLARIS
#include <strings.h> 
#endif
#endif

#include <sys/stat.h>
#include <string.h>
#include "CFpars.h"

/* #define DEBUGINCLUDE */

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifdef DMALLOC_INCLUDE
#define YY_MALLOC_DECL
#endif

int include_state = 0;
int include_flag = 0;
int prev_state[500];

int lineno = 1;
int save_state = 0;
extern int config_parse_error;

extern FILE *LOGFILE;

extern struct cf_LVAL { 
        int  tok_id;
        char *string;
        int intval;
 } cf_lval, cf_lval_pushback;

extern char *default_path;

#define MAX_INCLUDE_DEPTH 20
YY_BUFFER_STATE cf_include_stack[MAX_INCLUDE_DEPTH];
int cf_include_stack_ptr = 0;


%}

/* Definitions */

DIGIT  [0-9]
DIGITS [0-9]+	
QUOTED_DIGITS ['"][0-9]+['"]
DECIMALS "-"?{DIGITS}.{DIGITS}
FLOATS {DECIMALS}[Ee]{DIGITS}
LETTER [a-zA-Z]

nullmdo "<!>"
comments_start "<!-- "
comments_end ^.*"-->"[ \t]*$
one_line_comment "<!--".*"-->"

ws  [ \t]+
nl  \n
eq [ \t]*"="[ \t]*
name_token [a-zA-Z0-9][a-zA-Z0-9\-_.~/]*
dos_path_token [a-zA-Z0-9][a-zA-Z0-9\-_.~]*"\\"[a-zA-Z0-9][a-zA-Z0-9\-_.~]*
quoted_name_token ['"][a-zA-Z0-9][a-zA-Z0-9\-_.~/]*['"]
attr_string [a-zAZ\-_.]*
filename "/"[a-zA-Z][a-zA-Z0-9\-\_\/\.\~\']*
xpathspec [a-zA-Z0-9\-_./~@:*|&;,\[\]()=+'"]*
sqldata [a-zA-Z0-9\-_./~@:*|&;,\[\]()=+'" ]*
quoted_filename ['"]"/"[a-zA-Z][a-zA-Z0-9\-\_\/\.\~]*['"]
dosfilename [a-zA-Z]":"[a-zA-Z0-9\-\_\.\~\\]*
relfilename "."[./]*([^/ \n\t]*"/")*[^/ \n\t]*[ \n\t]
quoteddosfilename ['"][a-zA-Z]":"[a-zA-Z0-9 \-\_\.\~\\]*['"]
url [a-zA-Z]+":"[a-zA-Z0-9_/.~'-]*
tagpattern [a-zA-Z0-9_^$.:[*#\\{}()\]|-]*
anything [^<]*
attrsetoid ['"]"1.2.840.10003.3."[0-9.]*['"]

/* Start conditions */
%x sgml_comment
%x filedef
%x indxdef
%x clusdef
%x filedtd
%x filecont
%x fileenc
%x filecontinc
%x cfinclude 
%x indxcont
%x indxmap
%x indxmapdat
%x exturldef
%x indxdesc
%x access_type
%x extract_type
%x normal_type
%x primkey
%x attributeset
%x normal_clus
%s tags
%x maxnum
%x dispopts
%x formatdef
%x convertdef
%x excludedef
%x exclkey
%x explain
%x explaintext
%x components
%x explainintunit
%x xpath
%x sqldata
%x terminate
%x rank_params
%x ptags
%%

"<DBCONFIG>" {
	     if (include_flag == 0) { 
	        cf_include_stack_ptr = 0;
	        cf_include_stack[cf_include_stack_ptr++] = YY_CURRENT_BUFFER;
	        return (DBCONFIG_START);
	     }
        }
"<DBENV>" { 
	     if (include_flag == 0) 
	          return (DBENV_START);
	     else {
	        include_flag = 2; /* to skip the filepath */
	     }
}

"<FILEDEF" { BEGIN(filedef); return (FILEDEF_START);}
"<DEFAULTPATH>" { return (DEFAULTPATH_START);}
"<FILENAME>" { return (FILENAME_START);}
"<FILETAG>" { return (FILETAG_START);}
"<FILECONT" { BEGIN(filecont); return (FILECONT_START);}
"<FILEENCODING>" { BEGIN(fileenc); return (FILEENC_START);}
"<CONTINCLUDE>" { BEGIN(filecontinc);}
"<CONFIGINCLUDE>" { BEGIN(cfinclude);
		    include_flag = 1; }
"<FILEDTD" {BEGIN(filedtd); return (FILEDTD_START);}
"<XMLSCHEMANAME>" { return (FILEXMLSCHEMA_START);}
"<XMLSCHEMA>" { return (FILEXMLSCHEMA_START);}
"<DISTRIBRECCOUNT>" {return (DISTRECCOUNT_START);}
"<SGMLCAT>" { return (SGMLCAT_START);}
"<SGMLCATALOG>" { return (SGMLCAT_START);}
"<SGML-CATALOG>" { return (SGMLCAT_START);}
"<EXPLAIN>" { BEGIN(explain); return (EXPLAIN_START);}
"<ASSOCFIL>" { return (ASSOCFIL_START);}
"<HISTORY>" { return (HISTORY_START);}
"<INDEXES>" { return (INDEXES_START);}
"<INDEXDEF" { BEGIN(indxdef); return (INDEXDEF_START);}
"<INDXNAME>" { return (INDXNAME_START);}
"<INDXTAG>" { return (INDXTAG_START);}
"<INDXMAP" { BEGIN(attributeset); return (INDXMAP_START);}
"<INDXCONT>" {BEGIN(indxcont); return (INDXCONT_START);}
"<STOPLIST>" { return (STOPLIST_START);}
"<EXPANSION_TERMS>" { return (EXPANSIONTERMS_START);}
"<EXTERN_APP>" { BEGIN(exturldef); return (EXTERN_APP_START);}
"<INDXKEY>" { return (INDXKEY_START);}
"<INDXEXC>" { return (INDXEXC_START);}
"<INDEXNAME>" { return (INDXNAME_START);}
"<INDEXTAG>" { return (INDXTAG_START);}
"<INDEXMAP" { BEGIN(attributeset); return (INDXMAP_START);}
"<INDEXCONT>" {BEGIN(indxcont); return (INDXCONT_START);}
"<INDEXKEY>" { return (INDXKEY_START);}
"<INDEXEXC>" { return (INDXEXC_START);}
"<RANK_PARAMS" {BEGIN(rank_params); 
	       /* printf("FOUND RANK_PARAMS\n"); */
return (RANK_PARAMS_START);}
"<RANK_PARAMETERS" {BEGIN(rank_params); 
	       /* printf("FOUND RANK_PARAMETERS\n"); */
return (RANK_PARAMS_START);}
"<CLUSTERDEF>" { return (CLUSTERDEF_START);}
"<CLUSTERS>" { return (CLUSTERS_START);}
"<CLUSTER>" { return (CLUSTER_START);}
"<CLUSBASE>" {return (CLUSBASE_START);}
"<CLUSNAME>" {return (CLUSNAME_START);}
"<CLUSTAG>" {return (CLUSNAME_START);}
"<CLUSKEY"  {BEGIN(clusdef); return (CLUSKEY_START);}
"<CLUSMAP>" {return (CLUSMAP_START);}
"<FROM>" {BEGIN(tags); return (FROM_START);}
"<TO>" {BEGIN(tags); return (TO_START);}
"<SUMMARIZE>" {BEGIN(tags); return (SUMMARIZE_START);}
"<DISPOPTIONS>" {BEGIN(dispopts); return(DISPOPTIONS_START); }
"<DISPLAYS>" {return(DISPLAY_START); }
"<DISPLAY>" {return(DISPLAY_START); }
"<FORMAT"  {BEGIN(formatdef); cf_lval.intval = 0; return (FORMAT_START);}
"<DISPLAYDEF"  {BEGIN(formatdef); cf_lval.intval = 0; return (FORMAT_START);}
"<INCLUDE>" {BEGIN(tags); return (INCLUDE_START);}
"<EXCLUDE" {BEGIN(excludedef); return (EXCLUDE_START);}
"<CONVERT"  {BEGIN(convertdef); cf_lval.intval = 0; return (CONVERT_START);}
"<COMPONENTDEF>" {BEGIN(components); return (COMPONENTDEF_START);}
"<COMPONENTS>" { return (COMPONENTS_START);}
<components>"<COMPONENTNAME>" {return(COMPONENTNAME_START);}
<components>"<COMPONENTNORM>" {return(COMPONENTNORM_START);}
<components>"<COMPONENTSTORE>" {return(COMPONENTSTORE_START);}
<components>"<COMPONENTINDEXES>" {BEGIN(INITIAL); return(COMPINDEXES_START);}
<components>"<COMPINDEXES>" {BEGIN(INITIAL); return(COMPINDEXES_START);}


<components>"NONE" {return (NONE_NORM);}
<components>"BASIC" {return (NONE_NORM);}
<components>"<TAGSPEC>" {BEGIN(tags); return (TAGSPEC_START);}

<components>"<COMPSTARTTAG>" {return(COMPTAGSTART_START);}
<components>"<COMPONENTSTARTTAG>" {return(COMPTAGSTART_START);}
<components>"<COMPONENTSTART>" {return(COMPTAGSTART_START);}

<components>"<COMPENDTAG>" { return(COMPTAGEND_START);}
<components>"<COMPONENTENDTAG>" {return(COMPTAGEND_START);}
<components>"<COMPONENTEND>" {return(COMPTAGEND_START);}

<components>"STORE" {return (STORE_YES);} 
<components>"NOSTORE" {return (STORE_NO);} /* the default */

<components>"YES" {return (STORE_YES);} 
<components>"NO" {return (STORE_NO);} /* the default */

<excludedef>"COMPRESS"{eq} {BEGIN(exclkey);}
<excludedef,components>"COMPRESS" {return (COMPRESS_YES);} 
<excludedef,components>"NOCOMPRESS" {return (COMPRESS_NO);} /* the default */
<excludedef>">" {BEGIN(tags); return(ATTRIBUTES_END);}

<exclkey>"NO" {BEGIN(excludedef); return(COMPRESS_NO);}
<exclkey>"FALSE" {BEGIN(excludedef); return(COMPRESS_NO);}
<exclkey>"TRUE" {BEGIN(excludedef); return(COMPRESS_YES);}
<exclkey>"YES" {BEGIN(excludedef); return(COMPRESS_YES);}

<dispopts>"KEEP_AMP" { return (KEEPAMP);}
<dispopts>"KEEP_LT" { return (KEEPLT);}
<dispopts>"KEEP_GT" { return (KEEPGT);}
<dispopts>"KEEP_ALL" { return (KEEPALL);}
<dispopts>"KEEP_ENT" { return (KEEPENT);}
<dispopts>"KEEP_ENTITIES" { return (KEEPENT);}
<dispopts>"</DISPOPTIONS>" {BEGIN(INITIAL); return(DISPOPTIONS_END);}

"<TAGSPEC>" { BEGIN(tags); return (TAGSPEC_START);}
<tags>"<XPATH>" { BEGIN(xpath); return (XPATH_START);}
<tags>"<SQL>" {BEGIN(sqldata); return (SQL_START);}
<tags>"<FTAG>" { return (FTAG_START);}
<tags>"<S>" { return (S_START);}
<tags>"<ATTR>" {return (ATTR_START);}
<tags>"<VALUE>" {return (ATTRVAL_START);}
<tags>"<REPLACE>" {return (REPLACEVAL_START);}
<tags>"<MAXNUM>" {BEGIN(maxnum); return (MAXNUM_START);}


<tags>{tagpattern}  { cf_lval.string = strdup(cf_text); 
	   
	     return TAG_PATTERN;
	}


<tags>{relfilename}  { cf_lval.string = strdup(cf_text);
	     return TAG_PATTERN;
	}

"</DBCONFIG>" { 
	     if (include_flag == 0) 
	      return (DBCONFIG_END);
}

"</DBENV>" { 
	     if (include_flag == 0) 
	      return (DBENV_END);
	     else {
	        include_flag = 1;
	     }
}

"</FILEDEF>" {return (FILEDEF_END);}
"</DEFAULTPATH>" { return (DEFAULTPATH_END);}
"</FILENAME>" { return (FILENAME_END);}
"</FILETAG>" { return (FILETAG_END);}
<fileenc>"</FILEENCODING>" { BEGIN(INITIAL); return (FILEENC_END);}
"</FILECONT>" { return (FILECONT_END);}
"</CONTINCLUDE>" { /* just eat it */
  BEGIN(INITIAL);
}

"</CONFIGINCLUDE>" { /* just eat it */
  include_flag = 0;
  BEGIN(INITIAL);
}

"</FILEDTD>" { return (FILEDTD_END);}
"</XMLSCHEMANAME>" { return (FILEXMLSCHEMA_END);}
"</XMLSCHEMA>" { return (FILEXMLSCHEMA_END);}
"</DISTRIBRECCOUNT>" { return (DISTRECCOUNT_END);}
"</SGMLCAT>" { return (SGMLCAT_END);}
"</SGMLCATALOG>" { return (SGMLCAT_END);}
"</SGML-CATALOG>" { return (SGMLCAT_END);}
"</ASSOCFIL>" { return (ASSOCFIL_END);}
"</HISTORY>" { return (HISTORY_END);}
"</INDEXES>" { return (INDEXES_END);}
"</INDEXDEF>" { return (INDEXDEF_END);}
"</INDXNAME>" { return (INDXNAME_END);}
"</INDXTAG>" { return (INDXTAG_END);}
"</INDXCONT" { return (INDXCONT_END);}
"</STOPLIST>" { return (STOPLIST_END);}
"</EXPANSION_TERMS>" { return (EXPANSIONTERMS_END);}
<exturldef>"</EXTERN_APP>" { BEGIN(INITIAL); return (EXTERN_APP_END);}
"</INDXKEY>" { return (INDXKEY_END);}
"</INDXEXC>" { return (INDXEXC_END);}
"</INDEXNAME>" { return (INDXNAME_END);}
"</INDEXTAG>" { return (INDXTAG_END);}
"</INDEXCONT" { return (INDXCONT_END);}
"</INDEXKEY>" { return (INDXKEY_END);}
"</INDEXEXC>" { return (INDXEXC_END);}

<rank_params>"</RANK_PARAMS>" {BEGIN(INITIAL); return (RANK_PARAMS_END);}
<rank_params>"</RANK_PARAMETERS>" {BEGIN(INITIAL); return (RANK_PARAMS_END);}
<rank_params>"TYPE"{eq} {	
		       /* printf("FOUND TYPE=\n"); */
     
	     return(RANK_TYPE_ATTR);
	 }

<rank_params>"LOGISTIC_REGRESSION" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}
<rank_params>"LOGREG" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}
<rank_params>"LR" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}


<rank_params>"\"LOGISTIC_REGRESSION\"" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}
<rank_params>"\"LOGREG\"" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}
<rank_params>"\"LR\"" {	
	     cf_lval.intval = 0;
             return (LR_RANK);}


<rank_params>"LOGISTIC_REGRESSION_TREC2" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}
<rank_params>"LOGREGTREC2" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}
<rank_params>"TREC2" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}


<rank_params>"\"LOGISTIC_REGRESSION_TREC2\"" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}
<rank_params>"\"LOGREGTREC2\"" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}
<rank_params>"\"TREC2\"" {	
	     cf_lval.intval = 4;
             return (LR_RANK_TREC2);}

<rank_params>"LANGUAGE_MODELING" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}
<rank_params>"LANGMOD" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}
<rank_params>"LM" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}

<rank_params>"\"LANGUAGE_MODELING\"" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}
<rank_params>"\"LANGMOD\"" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}
<rank_params>"\"LM\"" {	
	     cf_lval.intval = 1;
             return (LM_RANK);}


<rank_params>"OKAPI" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"OK" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"BM25" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"BM-25" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}


<rank_params>"\"OKAPI\"" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"\"OK\"" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"\"BM25\"" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}
<rank_params>"\"BM-25\"" {	
	     cf_lval.intval = 2;
             return (OK_RANK);}


<rank_params>"TFIDF" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}
<rank_params>"TF" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}
<rank_params>"VECTOR" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}


<rank_params>"\"TFIDF\"" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}
<rank_params>"\"TF\"" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}
<rank_params>"\"VECTOR\"" {	
	     cf_lval.intval = 3;
             return (TFIDF_RANK);}


<rank_params>{DECIMALS} {
	     cf_lval.string = strdup(cf_text);  
	     cf_lval.intval = -1;
             return (PARAM_VALUE);}

<rank_params>"<PARAM" {BEGIN(ptags) ; return(PARAM_START);}

<rank_params>"</PARAM>" {return(PARAM_END);}

<ptags>"ID"{eq} {
          /* printf("parser ID =\n"); */        
          return (PARAM_ID_ATTR);}

<ptags>{DIGITS} { cf_lval.intval = atoi(cf_text);
		          /* printf("parser DIGITS\n"); */

                     return NUMBER;
                   }
<ptags>{QUOTED_DIGITS} { cf_lval.intval = atoi(cf_text+1);
          /* printf("parser QUOTED_DIGITS\n"); */

                     return NUMBER;
                   }
<ptags,rank_params>">" {
          BEGIN(rank_params);
          return(ATTRIBUTES_END);
          }


<tags,xpath>"</TAGSPEC>" {
       BEGIN(INITIAL); 
       return (TAGSPEC_END);}
<sqldata>"</SQL>" {BEGIN(tags); return (SQL_END);}
<xpath>"</XPATH>" { BEGIN(tags); return (XPATH_END);}
<tags>"</FTAG>" { return (FTAG_END);}
<tags>"</S>" { return (S_END);}
<tags>"</ATTR>" {return (ATTR_END);}
<tags>"</VALUE>" {return (ATTRVAL_END);}
<tags>"</REPLACE>" {return (REPLACEVAL_END);}
<maxnum>"</MAXNUM>" {BEGIN(tags); return (MAXNUM_END);}
<maxnum>{DIGITS} { cf_lval.intval = atoi(cf_text);
                     return NUMBER;
                   }

"</CLUSTERS>" { return (CLUSTERS_END);}
"</CLUSTERDEF>" { return (CLUSTERDEF_END);}
"</CLUSTER>" { return (CLUSTER_END);}
"</CLUSBASE>" {return (CLUSBASE_END);}
"</CLUSNAME>" {return (CLUSNAME_END);}
"</CLUSTAG>" {return (CLUSNAME_END);}
"</CLUSKEY>"  {BEGIN(INITIAL); return (CLUSKEY_END);}
"</CLUSMAP>" {return (CLUSMAP_END);}
"</FROM>" {BEGIN(INITIAL); return (FROM_END);}
"</TO>" {BEGIN(INITIAL); return (TO_END);}
"</SUMMARIZE>" {BEGIN(INITIAL); return (SUMMARIZE_END);}
"</DISPLAYS>" {BEGIN(INITIAL); return(DISPLAY_END);}
"</DISPLAY>" {BEGIN(INITIAL); return(DISPLAY_END);}
"</FORMAT>"  {BEGIN(INITIAL); return (FORMAT_END);}
"</DISPLAYDEF>"  {BEGIN(INITIAL); return (FORMAT_END);}
"</INCLUDE>" {BEGIN(tags); return (INCLUDE_END);}
"</EXCLUDE>" {BEGIN(tags); return (EXCLUDE_END);}
"</CONVERT>"  {BEGIN(INITIAL); return (CONVERT_END);}

"</COMPONENTS>" { return (COMPONENTS_END);}
<components>"</COMPONENTDEF>" {BEGIN(INITIAL); return (COMPONENTDEF_END);}
<components>"</COMPONENTNAME>" {return(COMPONENTNAME_END);}
<components>"</COMPONENTNORM>" {return(COMPONENTNORM_END);}
<components>"</COMPONENTSTORE>" {return(COMPONENTSTORE_END);}

"</COMPONENTINDEXES>" {BEGIN(components); return(COMPINDEXES_END);}
"</COMPINDEXES>" {BEGIN(components); return(COMPINDEXES_END);}

"</COMPSTARTTAG>" {BEGIN(components); return(COMPTAGSTART_END);}
"</COMPONENTSTARTTAG>" {BEGIN(components); return(COMPTAGSTART_END);}
"</COMPONENTSTART>" {BEGIN(components); return(COMPTAGSTART_END);}

"</COMPENDTAG>" {BEGIN(components); return(COMPTAGEND_END);}
"</COMPONENTENDTAG>" {BEGIN(components); return(COMPTAGEND_END);}
"</COMPONENTEND>" {BEGIN(components); return(COMPTAGEND_END);}

<filedef,filedtd>"TYPE"{eq} { return (TYPE_ATTR);}

<indxdef>"ACCESS"{eq} {
                    BEGIN(access_type);   
                    return (ACCESS_ATTR);}
<indxdef>"EXTRACT"{eq} {BEGIN(extract_type); return (EXTRACT_ATTR);}
<indxdef>"NORMAL"{eq} {BEGIN(normal_type); return (NORMAL_ATTR);}
<indxdef>"PRIMARYKEY"{eq} {BEGIN(primkey); return (PRIMARYKEY_ATTR);}
<indxdef>"PRIMARYKEY" {return (PRIMARYKEY_IGNORE);} 
<indxdef>"NOTPRIMARYKEY" {return (PRIMARYKEY_NO);} /* the default */

<clusdef>"NORMAL"{eq} {BEGIN(normal_clus); return (NORMAL_ATTR);}

<formatdef>"NAME"{eq} {
                    return (FORMAT_NAME_ATTR);}
<formatdef>"OID"{eq} {
		return (OID_NAME_ATTR); }
<formatdef>"1.2.840.10003.5.10" {
		return (MARC_OID); }
<formatdef>"1.2.840.10003.5.100" {
		return (EXP_OID); }
<formatdef>"1.2.840.10003.5.101" {
		return (SUTRS_OID); }
<formatdef>"1.2.840.10003.5.109.9" {
		return (SGML_OID); }
<formatdef>"1.2.840.10003.5.109.10" {
		return (XML_OID); }
<formatdef>"1.2.840.10003.5.109.3" {
		return (HTML_OID); }
<formatdef>"1.2.840.10003.5.102" {
		return (OPAC_OID); }
<formatdef>"1.2.840.10003.5.103" {
		return (SUMM_OID); }
<formatdef>"1.2.840.10003.5.104" {
		return (GRS0_OID); }
<formatdef>"1.2.840.10003.5.105" {
		return (GRS1_OID); }
<formatdef>"1.2.840.10003.5.106" {
		return (ES_OID); }

<formatdef>"\"1.2.840.10003.5.10\"" {
		return (MARC_OID); }
<formatdef>"\"1.2.840.10003.5.100\"" {
		return (EXP_OID); }
<formatdef>"\"1.2.840.10003.5.101\"" {
		return (SUTRS_OID); }
<formatdef>"\"1.2.840.10003.5.102\"" {
		return (OPAC_OID); }
<formatdef>"\"1.2.840.10003.5.109.9\"" {
		return (SGML_OID); }
<formatdef>"\"1.2.840.10003.5.109.10\"" {
		return (XML_OID); }
<formatdef>"\"1.2.840.10003.5.109.3\"" {
		return (HTML_OID); }
<formatdef>"\"1.2.840.10003.5.103\"" {
		return (SUMM_OID); }
<formatdef>"\"1.2.840.10003.5.104\"" {
		return (GRS0_OID); }
<formatdef>"\"1.2.840.10003.5.105\"" {
		return (GRS1_OID); }
<formatdef>"\"1.2.840.10003.5.106\"" {
		return (ES_OID); }


<formatdef>"DEFAULT" { cf_lval.intval = 1; 
                    return (DEFAULT_ATTR);}

<formatdef>"MARC_DTD"{eq} {
                    return (MARC_DTD_FILE_ATTR);}

<convertdef>"FUNCTION"{eq} {
                    return (FUNCTION_ATTR);}


<convertdef>"OUTFILE"{eq} {
                    return (OUTFILE_ATTR);}


<convertdef>"ALL" {  cf_lval.intval = 1;
                    return (ALL_ATTR);}

<filedef,filedtd>"SGML"  { 
	     return SGML_TYPE;
	}
<filedef,filedtd>"XML"  { 
	     return XML_TYPE;
	}
<filedef,filedtd>"XML_NODTD"  { 
	     return XML_NODTD_TYPE;
	}
<filedtd>"XMLSCHEMA"  { 
	     return XMLSCHEMA_TYPE;
	}
<filedef>"MARC"  { 
	     return MARC_TYPE;
	}
<filedef>"MARCSGML"  { 
	     return MARC_TYPE;
	}
<filedef>"AUTH"  { 
	     return AUTH_TYPE;
	}
<filedef>"CLUSTER"  { 
	     return CLUSTER_TYPE;
	}
<filedef>"LCCTREE"  { 
	     return LCCTREE_TYPE;
	}
<filedef>"MAPPED"  { 
	     return MAPPED_TYPE;
	}
<filedef>"MAP"  { 
	     return MAPPED_TYPE;
	}
<filedef>"DBMS"  { 
	     return DBMS_TYPE;
	}
<filedef>"SQL"  { 
	     return DBMS_TYPE;
	}
<filedef>"RDBMS"  { 
	     return DBMS_TYPE;
	}
<filedef>"EXPLAIN"  { 
	     return EXPLAIN_TYPE;
	}

<filedef>"VIRTUAL"  { 
	     return VIRTUAL_TYPE;
	}

<filedef>"VIRTUALDB"  { 
	     return VIRTUAL_TYPE;
	}

<filedef,filedtd>"\"SGML\""  { 
	     return SGML_TYPE;
	}
<filedef,filedtd>"\"XML\""  { 
	     return XML_TYPE;
	}
<filedtd>"\"XMLSCHEMA\""  { 
	     return XMLSCHEMA_TYPE;
	}
<filedef>"\"MARC\""  { 
	     return MARC_TYPE;
	}
<filedef>"\"MARCSGML\""  { 
	     return MARC_TYPE;
	}
<filedef>"\"AUTH\""  { 
	     return AUTH_TYPE;
	}
<filedef>"\"CLUSTER\""  { 
	     return CLUSTER_TYPE;
	}
<filedef>"\"LCCTREE\""  { 
	     return LCCTREE_TYPE;
	}
<filedef>"\"MAPPED\""  { 
	     return MAPPED_TYPE;
	}
<filedef>"\"MAP\""  { 
	     return MAPPED_TYPE;
	}
<filedef>"\"DBMS\""  { 
	     return DBMS_TYPE;
	}
<filedef>"\"SQL\""  { 
	     return DBMS_TYPE;
	}
<filedef>"\"RDBMS\""  { 
	     return DBMS_TYPE;
	}
<filedef>"\"EXPLAIN\""  { 
	     return EXPLAIN_TYPE;
	}

<filedef>"\"VIRTUAL\""  { 
	     return VIRTUAL_TYPE;
	}

<filedef>"\"VIRTUALDB\""  { 
	     return VIRTUAL_TYPE;
	}

<filedef>"SGML_DATASTORE"  { 
	     return SGML_DATASTORE_TYPE;
	}
<filedef>"XML_DATASTORE"  { 
	     return SGML_DATASTORE_TYPE;
	}
<filedef>"MARC_DATASTORE"  { 
	     return MARC_DATASTORE_TYPE;
	}
<filedef>"MARCSGML_DATASTORE"  { 
	     return MARC_DATASTORE_TYPE;
	}
<filedef>"CLUSTER_DATASTORE"  { 
	     return CLUSTER_DATASTORE_TYPE;
	}
<filedef>"EXPLAIN_DATASTORE"  { 
	     return EXPLAIN_DATASTORE_TYPE;
	}

<filedef>"\"SGML_DATASTORE\""  { 
	     return SGML_DATASTORE_TYPE;
	}
<filedef>"\"XML_DATASTORE\""  { 
	     return SGML_DATASTORE_TYPE;
	}
<filedef>"\"MARC_DATASTORE\""  { 
	     return MARC_DATASTORE_TYPE;
	}
<filedef>"\"MARCSGML_DATASTORE\""  { 
	     return MARC_DATASTORE_TYPE;
	}
<filedef>"\"CLUSTER_DATASTORE\""  { 
	     return CLUSTER_DATASTORE_TYPE;
	}
<filedef>"\"EXPLAIN_DATASTORE\""  { 
	     return EXPLAIN_DATASTORE_TYPE;
	}

<fileenc>"UTF-8" {
             return UTF_8_ENC;        
}		 
<fileenc>"UTF_8" {
             return UTF_8_ENC;        
}		 
<fileenc>"UTF8" {
             return UTF_8_ENC;        
}		 
<fileenc>"KOI-8" {
             return KOI8_ENC;        
}		 
<fileenc>"KOI8" {
             return KOI8_ENC;        
}		 
<fileenc>"ISO-8859-1" {
             return ISO_8859_1_ENC;        
}		 
<fileenc>"ISO8859-1" {
             return ISO_8859_1_ENC;        
}		 
<fileenc>"ISO_8859_1" {
             return ISO_8859_1_ENC;        
}		 
<fileenc>"ISO8859_1" {
             return ISO_8859_1_ENC;        
}		 


<access_type>"BTREE" { BEGIN(indxdef); return (BTREE);}
<access_type>"HASH" { BEGIN(indxdef); return (HASH);}
<access_type>"VECTOR" { BEGIN(indxdef); return (VECTOR);}
<access_type>"DBMS" { BEGIN(indxdef); return (DBMS_TYPE);}
<access_type>"DBMS_BTREE" { BEGIN(indxdef); return (DBMS_BTREE);}
<access_type>"BITMAPPED" { BEGIN(indxdef); return (BITMAPPED);}
<access_type>"BITMAP" { BEGIN(indxdef); return (BITMAPPED);}

<extract_type>"KEYWORD" { BEGIN(indxdef); return (KEYWORD_EXTRACT);}
<extract_type>"KEYWORD_EXTERNAL" { BEGIN(indxdef); return (KEYWORD_EXTERN_EXTRACT);}
<extract_type>"PROXIMITY" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"KEYWORD_PROX" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"KEYWORD_PROXIMITY" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"KEYWORD_EXTERNAL_PROX" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"KEYWORD_EXTERNAL_PROXIMITY" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"KEYWORD_PROX_EXTERNAL" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"KEYWORD_PROXIMITY_EXTERNAL" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"EXACTKEY" { BEGIN(indxdef); return (EXACTKEY_EXTRACT);}
<extract_type>"EXACTKEY_PROX" { BEGIN(indxdef); return (EXACTKEY_PROX_EXTRACT);}
<extract_type>"EXACTKEY_PROXIMITY" { BEGIN(indxdef); return (EXACTKEY_PROX_EXTRACT);}

<extract_type>"FLD008KEY" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"FLD008_KEY" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"FLD008" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"FLD008_DATE" { BEGIN(indxdef); return (FLD008_DATE);}
<extract_type>"FLD008_DATERANGE" { BEGIN(indxdef); return (FLD008_DATERANGE);}
<extract_type>"INTEGER_KEY" { BEGIN(indxdef); return (INTEGER_EXTRACT);}
<extract_type>"INTEGER" { BEGIN(indxdef); return (INTEGER_EXTRACT);}
<extract_type>"DECIMAL_KEY" { BEGIN(indxdef); return (DECIMAL_EXTRACT);}
<extract_type>"DECIMAL" { BEGIN(indxdef); return (DECIMAL_EXTRACT);}
<extract_type>"FLOAT_KEY" { BEGIN(indxdef); return (FLOAT_EXTRACT);}
<extract_type>"FLOAT" { BEGIN(indxdef); return (FLOAT_EXTRACT);}
<extract_type>"URL" { BEGIN(indxdef); return (URL_EXTRACT);}
<extract_type>"URLS" { BEGIN(indxdef); return (URL_EXTRACT);}
<extract_type>"FILENAME" { BEGIN(indxdef); return (FILENAME_EXTRACT);}
<extract_type>"FILE" { BEGIN(indxdef); return (FILENAME_EXTRACT);}
<extract_type>"DATE" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"DATE_KEY" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"DATE_TIME" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"DATE_RANGE" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"DATE_RANGE_KEY" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"DATE_TIME_RANGE" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"DATE_TIME_RANGE_KEY" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"LAT_LONG" { BEGIN(indxdef); return(LAT_LONG_EXTRACT);}
<extract_type>"LATITUDE_LONGITUDE" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"LATITUDE/LONGITUDE" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"GEO_POINT" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"GEO_BOX" { BEGIN(indxdef); return (BOUNDING_BOX_EXTRACT);}
<extract_type>"BOUNDING_BOX" { BEGIN(indxdef); return (BOUNDING_BOX_EXTRACT);}

<extract_type>"GEOTEXT" { BEGIN(indxdef); return(GEOTEXT_EXTRACT);}

<extract_type>"GEOTEXT_LAT_LONG" { BEGIN(indxdef); return(GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"GEOTEXT_LATITUDE_LONGITUDE" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"GEOTEXT_LATITUDE/LONGITUDE" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"GEOTEXT_POINT" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"GEOTEXT_BOX" { BEGIN(indxdef); return (GEOTEXT_BOUNDING_BOX_EXTRACT);}
<extract_type>"GEOTEXT_BOUNDING_BOX" { BEGIN(indxdef); return (GEOTEXT_BOUNDING_BOX_EXTRACT);}

<extract_type>"ELEMENT_EXISTS" { BEGIN(indxdef); 
     	cf_lval.intval = 1 ;
        return (CODED_ELEMENT);}
<extract_type>"ATTRIBUTE_EXISTS" { BEGIN(indxdef);
	cf_lval.intval = 2 ; 
        return (CODED_ELEMENT);}
<extract_type>"EXISTS" { BEGIN(indxdef);
     	cf_lval.intval = 1 ; 
        return (CODED_ELEMENT);}
<extract_type>"CODED_ELEMENT" { BEGIN(indxdef);
     	cf_lval.intval = 1 ; 
        return (CODED_ELEMENT);}
<extract_type>"CODED" { BEGIN(indxdef);
     	cf_lval.intval = 1 ; 
        return (CODED_ELEMENT);}
<extract_type>"CONTAINS" { BEGIN(indxdef);
     	cf_lval.intval = 1 ; 
        return (CODED_ELEMENT);}
<extract_type>"EXISTS_ELEMENT" { BEGIN(indxdef);
     	cf_lval.intval = 1 ;
        return (CODED_ELEMENT);}
<extract_type>"EXISTS_ATTRIBUTE" { BEGIN(indxdef);
     	cf_lval.intval = 2 ; 
        return (CODED_ELEMENT);}
<extract_type>"CONTAINS_ATTRIBUTE" { BEGIN(indxdef);
     	cf_lval.intval = 2 ; 
        return (CODED_ELEMENT);}

<extract_type>"NGRAM" { BEGIN(indxdef);
     	cf_lval.intval = 3 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"NGRAMS" { BEGIN(indxdef);
     	cf_lval.intval = 3 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"SHINGLE" { BEGIN(indxdef);
     	cf_lval.intval = 3 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"SHINGLES" { BEGIN(indxdef);
     	cf_lval.intval = 3 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"NGRAM4" { BEGIN(indxdef);
     	cf_lval.intval = 4 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"NGRAM5" { BEGIN(indxdef);
     	cf_lval.intval = 5 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"NGRAMS4" { BEGIN(indxdef);
     	cf_lval.intval = 4 ; 
        return (NGRAM_EXTRACT);}
<extract_type>"NGRAMS5" { BEGIN(indxdef);
     	cf_lval.intval = 5 ; 
        return (NGRAM_EXTRACT);}

<normal_type>"STEM" {BEGIN(indxdef); return (STEM_NORM);}
<normal_type>"STEMFREQ" {BEGIN(indxdef); return (STEMFREQ_NORM);}
<normal_type>"STEM_FREQ" {BEGIN(indxdef); return (STEMFREQ_NORM);}
<normal_type>"SSTEM" {BEGIN(indxdef); return (SSTEM_NORM);}
<normal_type>"SSTEMFREQ" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"SSTEM_FREQ" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"PLURALSTEM" {BEGIN(indxdef); return (SSTEM_NORM);}
<normal_type>"PLURALSTEMFREQ" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"PLURALSTEM_FREQ" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}



<normal_type>"PORTER_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 1;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"PORTER_STEMFREQ" {
	cf_lval.intval = 1;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"ENGLISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 2;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"ENGLISH_STEMFREQ" {
	cf_lval.intval = 2;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"FRENCH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 3;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"FRENCH_STEMFREQ" {
	cf_lval.intval = 3;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"GERMAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 4;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"GERMAN_STEMFREQ" {
	cf_lval.intval = 4;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"DUTCH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 5;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"DUTCH_STEMFREQ" {
	cf_lval.intval = 5;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"SPANISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 6;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"SPANISH_STEMFREQ" {
	cf_lval.intval = 6;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"ITALIAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 7;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"ITALIAN_STEMFREQ" {
	cf_lval.intval = 7;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"SWEDISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 8;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"SWEDISH_STEMFREQ" {
	cf_lval.intval = 8;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"PORTUGUESE_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 9;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"PORTUGUESE_STEMFREQ" {
	cf_lval.intval = 9;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"RUSSIAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 10;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"RUSSIAN_STEMFREQ" {
	cf_lval.intval = 10;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}


<normal_type>"RUSSIAN_KOI8_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 13;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"RUSSIAN_KOI8_STEMFREQ" {
	cf_lval.intval = 13;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"RUSSIAN_UTF8_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 10;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"RUSSIAN_UTF8_STEMFREQ" {
	cf_lval.intval = 10;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"DANISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 11;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"DANISH_STEMFREQ" {
	cf_lval.intval = 11;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"NORWEGIAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 12;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"NORWEGIAN_STEMFREQ" {
	cf_lval.intval = 12;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"FINNISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 14;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"FINNISH_STEMFREQ" {
	cf_lval.intval = 14;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"HUNGARIAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 15;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"HUNGARIAN_STEMFREQ" {
	cf_lval.intval = 15;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"TURKISH_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 16;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"TURKISH_STEMFREQ" {
	cf_lval.intval = 16;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"ROMANIAN_STEM" {BEGIN(indxdef); 
	cf_lval.intval = 17;
	return (SNOWBALL_STEM_NORM);}
<normal_type>"ROMANIAN_STEMFREQ" {
	cf_lval.intval = 17;
	BEGIN(indxdef); 
	return (SNOWBALL_STEMFREQ_NORM);}

<normal_type>"WORDNET" {BEGIN(indxdef); return (WORDNET_NORM);}
<normal_type>"CLASSCLUS" {BEGIN(indxdef); return (CLASSCLUS_NORM);}
<normal_type>"XKEY" {BEGIN(indxdef); return (XKEY_NORM);}
<normal_type>"XKEYFREQ" {BEGIN(indxdef); return (XKEYFREQ_NORM);}
<normal_type>"XKEY_FREQ" {BEGIN(indxdef); return (XKEYFREQ_NORM);}
<normal_type>"EXACTKEY" {BEGIN(indxdef); return (XKEY_NORM);}
<normal_type>"EXACTKEYFREQ" {BEGIN(indxdef); return (XKEYFREQ_NORM);}
<normal_type>"EXACTKEY_FREQ" {BEGIN(indxdef); return (XKEYFREQ_NORM);}
<normal_type>"BASIC" {BEGIN(indxdef); return (NONE_NORM);}
<normal_type>"BASICFREQ" {BEGIN(indxdef); return (NONEFREQ_NORM);}
<normal_type>"BASIC_FREQ" {BEGIN(indxdef); return (NONEFREQ_NORM);}
<normal_type>"NONE" {BEGIN(indxdef); return (NONE_NORM);}
<normal_type>"NONEFREQ" {BEGIN(indxdef); return (NONEFREQ_NORM);}
<normal_type>"NONE_FREQ" {BEGIN(indxdef); return (NONEFREQ_NORM);}
<normal_type>"REMOVE_TAGS_ONLY" {BEGIN(indxdef); return (MIN_NORMALIZE);}
<normal_type>"TAGS_ONLY" {BEGIN(indxdef); return (MIN_NORMALIZE);}
<normal_type>"REMOVE_TAGS_ONLY_FREQ" {BEGIN(indxdef); return (MIN_NORMALIZE_FREQ);}
<normal_type>"TAGS_ONLY_FREQ" {BEGIN(indxdef); return (MIN_NORMALIZE_FREQ);}
<normal_type>"DO_NOT_NORMALIZE" {BEGIN(indxdef); return (DONT_NORMALIZE);}
<normal_type>"DO_NOT_NORMALIZE_FREQ" {BEGIN(indxdef); return (DONT_NORMALIZE_FREQ);}

<normal_type>"STEM_NOMAP" {BEGIN(indxdef); return (STEM_NOMAP_NORM);}
<normal_type>"SSTEM_NOMAP" {BEGIN(indxdef); return (SSTEM_NOMAP_NORM);}
<normal_type>"PLURALSTEMSTEM_NOMAP" {BEGIN(indxdef); return (SSTEM_NOMAP_NORM);}
<normal_type>"WORDNET_NOMAP" {BEGIN(indxdef); return (WORDNET_NOMAP_NORM);}
<normal_type>"CLASSCLUS_NOMAP" {BEGIN(indxdef); return (CLASSCLUS_NOMAP_NORM);}
<normal_type>"XKEY_NOMAP" {BEGIN(indxdef); return (XKEY_NOMAP_NORM);}
<normal_type>"EXACTKEY_NOMAP" {BEGIN(indxdef); return (XKEY_NOMAP_NORM);}
<normal_type>"NONE_NOMAP" {BEGIN(indxdef); return (NONE_NOMAP_NORM);}
<normal_type>"BASIC_NOMAP" {BEGIN(indxdef); return (NONE_NOMAP_NORM);}

<access_type>"\"BTREE\"" { BEGIN(indxdef); return (BTREE);}
<access_type>"\"HASH\"" { BEGIN(indxdef); return (HASH);}
<access_type>"\"VECTOR\"" { BEGIN(indxdef); return (VECTOR);}
<access_type>"\"DBMS\"" { BEGIN(indxdef); return (DBMS_TYPE);}
<access_type>"\"BITMAPPED\"" { BEGIN(indxdef); return (BITMAPPED);}
<access_type>"\"BITMAP\"" { BEGIN(indxdef); return (BITMAPPED);}

<extract_type>"\"KEYWORD\"" { BEGIN(indxdef); return (KEYWORD_EXTRACT);}
<extract_type>"\"KEYWORD_EXTERNAL\"" { BEGIN(indxdef); return (KEYWORD_EXTERN_EXTRACT);}
<extract_type>"\"PROXIMITY\"" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_PROX\"" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_PROXIMITY\"" { BEGIN(indxdef); return (KEYWORD_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_EXTERNAL_PROX\"" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_EXTERNAL_PROXIMITY\"" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_PROX_EXTERNAL\"" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"\"KEYWORD_PROXIMITY_EXTERNAL\"" { BEGIN(indxdef); return (KEYWORD_EXTERN_PROX_EXTRACT);}
<extract_type>"\"EXACTKEY\"" { BEGIN(indxdef); return (EXACTKEY_EXTRACT);}
<extract_type>"\"FLD008KEY\"" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"\"FLD008_KEY\"" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"\"FLD008\"" { BEGIN(indxdef); return (FLD008_EXTRACT);}
<extract_type>"\"INTEGER_KEY\"" { BEGIN(indxdef); return (INTEGER_EXTRACT);}
<extract_type>"\"INTEGER\"" { BEGIN(indxdef); return (INTEGER_EXTRACT);}
<extract_type>"\"DECIMAL_KEY\"" { BEGIN(indxdef); return (DECIMAL_EXTRACT);}
<extract_type>"\"DECIMAL\"" { BEGIN(indxdef); return (DECIMAL_EXTRACT);}
<extract_type>"\"FLOAT_KEY\"" { BEGIN(indxdef); return (FLOAT_EXTRACT);}
<extract_type>"\"FLOAT\"" { BEGIN(indxdef); return (FLOAT_EXTRACT);}
<extract_type>"\"URL\"" { BEGIN(indxdef); return (URL_EXTRACT);}
<extract_type>"\"URLS\"" { BEGIN(indxdef); return (URL_EXTRACT);}
<extract_type>"\"FILENAME\"" { BEGIN(indxdef); return (FILENAME_EXTRACT);}
<extract_type>"\"FILE\"" { BEGIN(indxdef); return (FILENAME_EXTRACT);}
<extract_type>"\"DATE\"" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"\"DATE_KEY\"" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"\"DATE_TIME\"" { BEGIN(indxdef); return (DATE_EXTRACT);}
<extract_type>"\"DATE_RANGE\"" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"\"DATE_RANGE_KEY\"" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"\"DATE_TIME_RANGE\"" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"\"DATE_TIME_RANGE_KEY\"" { BEGIN(indxdef); return (DATE_RANGE_EXTRACT);}
<extract_type>"\"LAT_LONG\"" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"\"LATITUDE_LONGITUDE\"" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"\"LATITUDE/LONGITUDE\"" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"\"GEO_POINT\"" { BEGIN(indxdef); return (LAT_LONG_EXTRACT);}
<extract_type>"\"GEO_BOX\"" { BEGIN(indxdef); return (BOUNDING_BOX_EXTRACT);}
<extract_type>"\"BOUNDING_BOX\"" { BEGIN(indxdef); return (BOUNDING_BOX_EXTRACT);}

<extract_type>"\"GEOTEXT\"" { BEGIN(indxdef); return(GEOTEXT_EXTRACT);}
<extract_type>"\"GEOTEXT_LAT_LONG\"" { BEGIN(indxdef); return(GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"\"GEOTEXT_LATITUDE_LONGITUDE\"" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"\"GEOTEXT_LATITUDE/LONGITUDE\"" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"\"GEOTEXT_POINT\"" { BEGIN(indxdef); return (GEOTEXT_LAT_LONG_EXTRACT);}
<extract_type>"\"GEOTEXT_BOX\"" { BEGIN(indxdef); return (GEOTEXT_BOUNDING_BOX_EXTRACT);}
<extract_type>"\"GEOTEXT_BOUNDING_BOX\"" { BEGIN(indxdef); return (GEOTEXT_BOUNDING_BOX_EXTRACT);}


<normal_type>"\"STEM\"" {BEGIN(indxdef); return (STEM_NORM);}
<normal_type>"\"SSTEM\"" {BEGIN(indxdef); return (SSTEM_NORM);}
<normal_type>"\"PLURALSTEM\"" {BEGIN(indxdef); return (SSTEM_NORM);}

<normal_type>"\"STEMFREQ\"" {BEGIN(indxdef); return (STEMFREQ_NORM);}
<normal_type>"\"STEM_FREQ\"" {BEGIN(indxdef); return (STEMFREQ_NORM);}
<normal_type>"\"SSTEMFREQ\"" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"\"SSTEM_FREQ\"" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"\"PLURALSTEMFREQ\"" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}
<normal_type>"\"PLURALSTEM_FREQ\"" {BEGIN(indxdef); return (SSTEMFREQ_NORM);}


<normal_type>"\"WORDNET\"" {BEGIN(indxdef); return (WORDNET_NORM);}
<normal_type>"\"CLASSCLUS\"" {BEGIN(indxdef); return (CLASSCLUS_NORM);}
<normal_type>"\"XKEY\"" {BEGIN(indxdef); return (XKEY_NORM);}
<normal_type>"\"EXACTKEY\"" {BEGIN(indxdef); return (XKEY_NORM);}
<normal_type>"\"NONE\"" {BEGIN(indxdef); return (NONE_NORM);}
<normal_type>"\"NONE_FREQ\"" {BEGIN(indxdef); return (NONEFREQ_NORM);}
<normal_type>"\"REMOVE_TAGS_ONLY\"" {BEGIN(indxdef); return (MIN_NORMALIZE);}
<normal_type>"\"TAGS_ONLY\"" {BEGIN(indxdef); return (MIN_NORMALIZE);}
<normal_type>"\"REMOVE_TAGS_ONLY_FREQ\"" {BEGIN(indxdef); return (MIN_NORMALIZE_FREQ);}
<normal_type>"\"TAGS_ONLY_FREQ\"" {BEGIN(indxdef); return (MIN_NORMALIZE_FREQ);}
<normal_type>"\"DO_NOT_NORMALIZE\"" {BEGIN(indxdef); return (DONT_NORMALIZE);}
<normal_type>"\"DO_NOT_NORMALIZE_FREQ\"" {BEGIN(indxdef); return (DONT_NORMALIZE_FREQ);}
<normal_type>"\"STEM_NOMAP\"" {BEGIN(indxdef); return (STEM_NOMAP_NORM);}
<normal_type>"\"SSTEM_NOMAP\"" {BEGIN(indxdef); return (SSTEM_NOMAP_NORM);}
<normal_type>"\"PLURALSTEM_NOMAP\"" {BEGIN(indxdef); return (SSTEM_NOMAP_NORM);}
<normal_type>"\"WORDNET_NOMAP\"" {BEGIN(indxdef); return (WORDNET_NOMAP_NORM);}
<normal_type>"\"CLASSCLUS_NOMAP\"" {BEGIN(indxdef); return (CLASSCLUS_NOMAP_NORM);}
<normal_type>"\"XKEY_NOMAP\"" {BEGIN(indxdef); return (XKEY_NOMAP_NORM);}
<normal_type>"\"EXACTKEY_NOMAP\"" {BEGIN(indxdef); return (XKEY_NOMAP_NORM);}
<normal_type>"\"NONE_NOMAP\"" {BEGIN(indxdef); return (NONE_NOMAP_NORM);}
<normal_type>"\"BASIC_NOMAP\"" {BEGIN(indxdef); return (NONE_NOMAP_NORM);}
<normal_type>"YYMMDD" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYYMMDD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY-MM-DD HH:MM:SS" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"YYYY-MM-DD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY/MM/DD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY.MM.DD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MONTH DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MM DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MM DD HH MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY MONTH DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MONTH DD, YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MONTH DD, YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYMMDD HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"YYYYMMDD HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YY HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YYYY HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY-MM-DD HH:MM:SS" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYDDD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DECADE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"CENTURY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MILLENNIUM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MIXED YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MIXED_YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"UNIX_TIME" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"UNIX_CTIME" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"UNIX TIME" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"UNIX CTIME" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DAY MONTH DD HH:MM:SS YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DAY MMM DD HH:MM:SS YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}

<normal_type>"YYYY-YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YYYY to YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YYYY through YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YYYYMMDD-YYYYMMDD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YYYY/MM/DD-YYYY/MM/DD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY.MM.DD-YYYY.MM.DD" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YY-MM/DD/YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YYYY-MM/DD/YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YY-DD/MM/YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YYYY-DD/MM/YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YY-MM.DD.YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YYYY-MM.DD.YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YY-DD.MM.YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YYYY-DD.MM.YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YYYY-DD MMM YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YEAR-DD MMM YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MMM YY-DD MMM YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YY-DD MONTH YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YYYY-DD MONTH YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD MONTH YEAR-DD MONTH YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MONTH DD-YEAR MONTH DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MM DD-YEAR MM DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YEAR MM DD HH MM-YEAR MM DD HH MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY MONTH DD-YYYY MONTH DD" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MONTH DD, YY-MONTH DD, YY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MONTH DD, YYYY-MONTH DD, YYYY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYMMDD HH:MM-YYMMDD HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"YYYYMMDD HH:MM-YYYYMMDD HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YY HH:MM-MM/DD/YY HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM/DD/YYYY HH:MM-MM/DD/YYYY HH:MM" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YY HH:MM-DD/MM/YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD/MM/YYYY HH:MM-DD/MM/YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YY HH:MM-MM.DD.YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"MM.DD.YYYY HH:MM-MM.DD.YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"YYYY-MM-DD HH:MM:SS-YYYY-MM-DD HH:MM:SS" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YY HH:MM-DD.MM.YY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"DD.MM.YYYY HH:MM-DD.MM.YYYY HH:MM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}

<normal_type>"YEAR-YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YEAR to YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"YEAR through YEAR" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"DECADE-DECADE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"DECADE to DECADE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"DECADE through DECADE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"CENTURY-CENTURY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"CENTURY to CENTURY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"CENTURY through CENTURY" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"MILLENNIUM-MILLENNIUM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"MILLENNIUM to MILLENNIUM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"MILLENNIUM through MILLENNIUM" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"MIXED YEAR RANGE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"MIXED_YEAR_RANGE" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}

<normal_type>"\"YYMMDD\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYYMMDD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY-MM-DD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY/MM/DD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY.MM.DD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MONTH DD, YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MONTH DD, YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MONTH DD, YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYMMDD HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"YYYYMMDD HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YY HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY-MM-DD HH:MM:SS\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YYYY HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYDDD\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DECADE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"CENTURY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MILLENNIUM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MIXED YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MIXED_YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"UNIX_TIME\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"UNIX_CTIME\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"UNIX TIME\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"UNIX CTIME\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DAY MONTH DD HH:MM:SS YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DAY MMM DD HH:MM:SS YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}

<normal_type>"\"YYYY-YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YYYY to YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YYYY through YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YYYYMMDD-YYYYMMDD\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YYYY/MM/DD-YYYY/MM/DD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY.MM.DD-YYYY.MM.DD\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YY-MM/DD/YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YYYY-MM/DD/YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YY-DD/MM/YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YYYY-DD/MM/YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YY-MM.DD.YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YYYY-MM.DD.YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YY-DD.MM.YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YYYY-DD.MM.YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YYYY-DD MMM YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YEAR-DD MMM YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MMM YY-DD MMM YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YY-DD MONTH YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YYYY-DD MONTH YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD MONTH YEAR-DD MONTH YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YEAR MONTH DD-YEAR MONTH DD\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY MONTH DD-YYYY MONTH DD\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MONTH DD, YY-MONTH DD, YY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MONTH DD, YYYY-MONTH DD, YYYY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYMMDD HH:MM-YYMMDD HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"YYYYMMDD HH:MM-YYYYMMDD HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YY HH:MM-MM/DD/YY HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM/DD/YYYY HH:MM-MM/DD/YYYY HH:MM\"" {BEGIN(indxdef);
 	cf_lval.string = strdup(cf_text); 
	 return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YY HH:MM-DD/MM/YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD/MM/YYYY HH:MM-DD/MM/YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YY HH:MM-MM.DD.YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"MM.DD.YYYY HH:MM-MM.DD.YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YYYY-MM-DD HH:MM:SS-YYYY-MM-DD HH:MM:SS\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YY HH:MM-DD.MM.YY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"DD.MM.YYYY HH:MM-DD.MM.YYYY HH:MM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_NORMALIZE);}
<normal_type>"\"YEAR-YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YEAR to YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"YEAR through YEAR\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"DECADE-DECADE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"DECADE to DECADE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"DECADE through DECADE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"CENTURY-CENTURY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"CENTURY to CENTURY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"CENTURY through CENTURY\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"MILLENNIUM-MILLENNIUM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"MILLENNIUM to MILLENNIUM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"MILLENNIUM through MILLENNIUM\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"MIXED YEAR RANGE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}
<normal_type>"\"MIXED_YEAR_RANGE\"" {BEGIN(indxdef); 
 	cf_lval.string = strdup(cf_text); 
	return (DATE_RANGE_NORM);}

<normal_type>"DECIMAL_LAT_LONG" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (LAT_LONG_NORM);}

<normal_type>"DECIMAL_LONG_LAT" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (LAT_LONG_NORM);}

<normal_type>"DDoMM'SS''NS DDDoMM'SS''EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (LAT_LONG_NORM);}

<normal_type>"DD-MM-SS NS DDD-MM-SS EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (LAT_LONG_NORM);}

<normal_type>"DD-MM-SS-NS DDD-MM-SS-EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (LAT_LONG_NORM);}

<normal_type>"\"DECIMAL_LAT_LONG\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DECIMAL_LAT_LONG"); 
	return (LAT_LONG_NORM);}

<normal_type>"\"DECIMAL_LONG_LAT\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DECIMAL_LONG_LAT"); 
	return (LAT_LONG_NORM);}

<normal_type>"\"DDoMM'SS''NS DDDoMM'SS''EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DDoMM'SS''NS DDDoMM'SS''EW"); 
	return (LAT_LONG_NORM);}

<normal_type>"\"DD-MM-SS NS DDD-MM-SS EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DD-MM-SS NS DDD-MM-SS EW"); 
	return (LAT_LONG_NORM);}

<normal_type>"\"DD-MM-SS-NS DDD-MM-SS-EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DD-MM-SS-NS DDD-MM-SS-EW"); 
	return (LAT_LONG_NORM);}

<normal_type>"DECIMAL_BOUNDING_BOX" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"FGDC_BOUNDING_BOX" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (FGDC_BOUNDING_BOX_NORM);}

<normal_type>"DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"DD-MM-SS NS DDD-MM-SS EW DD-MM-SS NS DDD-MM-SS EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"DD-MM-SS-NS DDD-MM-SS-EW DD-MM-SS-NS DDD-MM-SS-EW" {BEGIN(indxdef); 	
	cf_lval.string = strdup(cf_text); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"\"DECIMAL_BOUNDING_BOX\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DECIMAL_BOUNDING_BOX"); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"\"FGDC_BOUNDING_BOX\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("FGDC_BOUNDING_BOX"); 
	return (FGDC_BOUNDING_BOX_NORM);}

<normal_type>"\"DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW"); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"\"DD-MM-SS NS DDD-MM-SS EW DD-MM-SS NS DDD-MM-SS EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DD-MM-SS NS DDD-MM-SS EW DD-MM-SS NS DDD-MM-SS EW"); 
	return (BOUNDING_BOX_NORM);}

<normal_type>"\"DD-MM-SS-NS DDD-MM-SS-EW DD-MM-SS-NS DDD-MM-SS-EW\"" {BEGIN(indxdef); 	
	cf_lval.string = strdup("DD-MM-SS-NS DDD-MM-SS-EW DD-MM-SS-NS DDD-MM-SS-EW"); 
	return (BOUNDING_BOX_NORM);}



<normal_clus>"STEM" {BEGIN(clusdef); return (STEM_NORM);}
<normal_clus>"SSTEM" {BEGIN(clusdef); return (SSTEM_NORM);}
<normal_clus>"PLURALSTEM" {BEGIN(clusdef); return (SSTEM_NORM);}
<normal_clus>"WORDNET" {BEGIN(clusdef); return (WORDNET_NORM);}
<normal_clus>"CLASSCLUS" {BEGIN(clusdef); return (CLASSCLUS_NORM);}
<normal_clus>"XKEY" {BEGIN(clusdef); return (XKEY_NORM);}
<normal_clus>"EXACTKEY" {BEGIN(clusdef); return (XKEY_NORM);}
<normal_clus>"NONE" {BEGIN(clusdef); return (NONE_NORM);}
<normal_clus>"BASIC" {BEGIN(indxdef); return (NONE_NORM);}
<normal_clus>"TAGS_ONLY_FREQ" {BEGIN(indxdef); return (MIN_NORMALIZE_FREQ);}
<normal_clus>"DO_NOT_NORMALIZE" {BEGIN(indxdef); return (DONT_NORMALIZE);}
<normal_clus>"STEM_NOMAP" {BEGIN(clusdef); return (STEM_NOMAP_NORM);}
<normal_clus>"SSTEM_NOMAP" {BEGIN(clusdef); return (SSTEM_NOMAP_NORM);}
<normal_clus>"PLURALSTEM_NOMAP" {BEGIN(clusdef); return (SSTEM_NOMAP_NORM);}
<normal_clus>"WORDNET_NOMAP" {BEGIN(clusdef); return (WORDNET_NOMAP_NORM);}
<normal_clus>"CLASSCLUS_NOMAP" {BEGIN(clusdef); return (CLASSCLUS_NOMAP_NORM);}
<normal_clus>"XKEY_NOMAP" {BEGIN(clusdef); return (XKEY_NOMAP_NORM);}
<normal_clus>"EXACTKEY_NOMAP" {BEGIN(clusdef); return (XKEY_NOMAP_NORM);}
<normal_clus>"NONE_NOMAP" {BEGIN(clusdef); return (NONE_NOMAP_NORM);}



<primkey>"NO" {BEGIN(indxdef); return(PRIMARYKEY_NO);}
<primkey>"NONE" {BEGIN(indxdef); return(PRIMARYKEY_NO);}
<primkey>"IGNORE" {BEGIN(indxdef); return(PRIMARYKEY_IGNORE);}
<primkey>"REJECT" {BEGIN(indxdef); return(PRIMARYKEY_IGNORE);}
<primkey>"REPLACE" {BEGIN(indxdef); return(PRIMARYKEY_REPLACE);}


<normal_clus>"\"STEM\"" {BEGIN(clusdef); return (STEM_NORM);}
<normal_clus>"\"SSTEM\"" {BEGIN(clusdef); return (SSTEM_NORM);}
<normal_clus>"\"PLURALSTEM\"" {BEGIN(clusdef); return (SSTEM_NORM);}
<normal_clus>"\"WORDNET\"" {BEGIN(clusdef); return (WORDNET_NORM);}
<normal_clus>"\"CLASSCLUS\"" {BEGIN(clusdef); return (CLASSCLUS_NORM);}
<normal_clus>"\"XKEY\"" {BEGIN(clusdef); return (XKEY_NORM);}
<normal_clus>"\"EXACTKEY\"" {BEGIN(clusdef); return (XKEY_NORM);}
<normal_clus>"\"NONE\"" {BEGIN(clusdef); return (NONE_NORM);}
<normal_clus>"\"STEM_NOMAP\"" {BEGIN(clusdef); return (STEM_NOMAP_NORM);}
<normal_clus>"\"SSTEM_NOMAP\"" {BEGIN(clusdef); return (SSTEM_NOMAP_NORM);}
<normal_clus>"\"PLURALSTEM_NOMAP\"" {BEGIN(clusdef); return (SSTEM_NOMAP_NORM);}
<normal_clus>"\"WORDNET_NOMAP\"" {BEGIN(clusdef); return (WORDNET_NOMAP_NORM);}
<normal_clus>"\"CLASSCLUS_NOMAP\"" {BEGIN(clusdef); return (CLASSCLUS_NOMAP_NORM);}
<normal_clus>"\"XKEY_NOMAP\"" {BEGIN(clusdef); return (XKEY_NOMAP_NORM);}
<normal_clus>"\"EXACTKEY_NOMAP\"" {BEGIN(clusdef); return (XKEY_NOMAP_NORM);}
<normal_clus>"\"NONE_NOMAP\"" {BEGIN(clusdef); return (NONE_NOMAP_NORM);}

<primkey>"\"NO\"" {BEGIN(indxdef); return(PRIMARYKEY_NO);}
<primkey>"\"NONE\"" {BEGIN(indxdef); return(PRIMARYKEY_NO);}
<primkey>"\"IGNORE\"" {BEGIN(indxdef); return(PRIMARYKEY_IGNORE);}
<primkey>"\"REJECT\"" {BEGIN(indxdef); return(PRIMARYKEY_IGNORE);}
<primkey>"\"REPLACE\"" {BEGIN(indxdef); return(PRIMARYKEY_REPLACE);}

<filecont>"ID=" {return(FILECONT_NUMBER);}
<filecont>"MIN=" {return(FILECONT_MIN);}
<filecont>"MAX=" {return(FILECONT_MAX);}
<filecont>"DB=" {return(FILECONT_DB);}

<filecont>{DIGITS} { cf_lval.intval = atoi(cf_text);
                     return NUMBER;
                   }
<indxcont>"ID=" {return(INDXCONT_NUMBER);}
<indxcont>{DIGITS} { cf_lval.intval = atoi(cf_text);
                     return NUMBER;
                   }

<indxmap>"</INDXMAP>" { BEGIN(INITIAL); return (INDXMAP_END);}
<indxmap>"<USE"  { BEGIN(attributeset); return (USE_START);}
<indxmap>"<RELATION"  { BEGIN(attributeset); return (RELATION_START);}
<indxmap>"<RELAT"  { BEGIN(attributeset); return (RELATION_START);}
<indxmap>"<REL"  { BEGIN(attributeset); return (RELATION_START);}
<indxmap>"<POSITION"  { BEGIN(attributeset); return (POSITION_START);}
<indxmap>"<POSIT"  { BEGIN(attributeset); return (POSITION_START);}
<indxmap>"<POS"  { BEGIN(attributeset); return (POSITION_START);}
<indxmap>"<STRUCTURE"  { BEGIN(attributeset); return (STRUCT_START);}
<indxmap>"<STRUCT"  { BEGIN(attributeset); return (STRUCT_START);}
<indxmap>"<STR"  { BEGIN(attributeset); return (STRUCT_START);}
<indxmap>"<TRUNCATION"  { BEGIN(attributeset); return (TRUNC_START);}
<indxmap>"<TRUNCAT"  { BEGIN(attributeset); return (TRUNC_START);}
<indxmap>"<TRUNC"  { BEGIN(attributeset); return (TRUNC_START);}
<indxmap>"<TRU"  { BEGIN(attributeset); return (TRUNC_START);}
<indxmap>"<COMPLETENESS"  { BEGIN(attributeset); return (COMPLET_START);}
<indxmap>"<COMPLETE"  { BEGIN(attributeset); return (COMPLET_START);}
<indxmap>"<COMPLET"  { BEGIN(attributeset); return (COMPLET_START);}
<indxmap>"<COMP"  { BEGIN(attributeset); return (COMPLET_START);}
<indxmap>"<COM"  { BEGIN(attributeset); return (COMPLET_START);}
<indxmap>"<DESCRIPTION>"  { BEGIN(indxdesc); return (DESCRIPT_START);}
<indxmap>"<DESC>"  {BEGIN(indxdesc); return (DESCRIPT_START);}
<indxmap>"</USE>"  {return (USE_END);}
<indxmap>"</RELATION>"  {return (RELATION_END);}
<indxmap>"</RELAT>"  {return (RELATION_END);}
<indxmap>"</REL>"  {return (RELATION_END);}
<indxmap>"</POSITION>"  {return (POSITION_END);}
<indxmap>"</POSIT>"  {return (POSITION_END);}
<indxmap>"</POS>"  {return (POSITION_END);}
<indxmap>"</STRUCTURE>"  {return (STRUCT_END);}
<indxmap>"</STRUCT>"  {return (STRUCT_END);}
<indxmap>"</STR>"  {return (STRUCT_END);}
<indxmap>"</TRUNCATION>"  {return (TRUNC_END);}
<indxmap>"</TRUNC>"  {return (TRUNC_END);}
<indxmap>"</TRU>"  {return (TRUNC_END);}
<indxmap>"</COMPLETENESS>"  {return (COMPLET_END);}
<indxmap>"</COMPLETE>"  {return (COMPLET_END);}
<indxmap>"</COMPLET>"  {return (COMPLET_END);}
<indxmap>"</COMP>"  {return (COMPLET_END);}
<indxmap>"</COM>"  {return (COMPLET_END);}

<indxmap>"<access_point" { BEGIN(attributeset); return ( ACCESS_START); }
<indxmap>"<access" { BEGIN(attributeset); return ( ACCESS_START); }
<indxmap>"<semantic_qualifier" { BEGIN(attributeset); return ( SEMANTIC_START); }
<indxmap>"<semantic" { BEGIN(attributeset); return ( SEMANTIC_START); }
<indxmap>"<language" { BEGIN(attributeset); return ( LANGUAGE_START); }
<indxmap>"<content_authority" { BEGIN(attributeset); return ( CONT_AUTH_START); }
<indxmap>"<authority" { BEGIN(attributeset); return ( CONT_AUTH_START); }
<indxmap>"<expansion" { BEGIN(attributeset); return ( EXPANSION_START); }
<indxmap>"<normalized_weight" { BEGIN(attributeset); return ( NORM_WEIGHT_START); }
<indxmap>"<weight" { BEGIN(attributeset); return ( NORM_WEIGHT_START); }
<indxmap>"<hit_count" { BEGIN(attributeset); return ( HIT_COUNT_START); }
<indxmap>"<hits" { BEGIN(attributeset); return ( HIT_COUNT_START); }
<indxmap>"<comparison" { BEGIN(attributeset); return ( COMPARISON_START); }
<indxmap>"<format" { BEGIN(attributeset); return ( FORMAT_ATTR_START); }
<indxmap>"<occurrence" { BEGIN(attributeset); return ( OCCURRENCE_START); }
<indxmap>"<indirection" { BEGIN(attributeset); return ( INDIRECTION_START); }
<indxmap>"<functional_qualifier" { BEGIN(attributeset); return ( FUNC_QUAL_START); }
<indxmap>"<functional" { BEGIN(attributeset); return ( FUNC_QUAL_START); }
<indxmap>"<function" { BEGIN(attributeset); return ( FUNC_QUAL_START); }


<indxmap>"</access_point>" {return ( ACCESS_END); }
<indxmap>"</access>" {return ( ACCESS_END); }
<indxmap>"</semantic_qualifier>" {return ( SEMANTIC_END); }
<indxmap>"</semantic>"  {return ( SEMANTIC_END); }
<indxmap>"</language>" {return ( LANGUAGE_END); }
<indxmap>"</content_authority>" {return ( CONT_AUTH_END); }
<indxmap>"</authority>" {return ( CONT_AUTH_END); }
<indxmap>"</expansion>" {return ( EXPANSION_END); }
<indxmap>"</normalized_weight>" {return ( NORM_WEIGHT_END); }
<indxmap>"</weight>" {return ( NORM_WEIGHT_END); }
<indxmap>"</hit_count>" {return ( HIT_COUNT_END); }
<indxmap>"</hits>" {return ( HIT_COUNT_END); }
<indxmap>"</comparison>" {return ( COMPARISON_END); }
<indxmap>"</format>" {return ( FORMAT_ATTR_END); }
<indxmap>"</occurrence>" {return ( OCCURRENCE_END); }
<indxmap>"</indirection>" {return ( INDIRECTION_END); }
<indxmap>"</functional_qualifier>" {return ( FUNC_QUAL_END); }
<indxmap>"</functional>" {return ( FUNC_QUAL_END); }
<indxmap>"</function>" {return ( FUNC_QUAL_END); }

<indxdesc>"</DESCRIPTION>"  {BEGIN(indxmap); return (DESCRIPT_END);}
<indxdesc>"</DESC>"  {BEGIN(indxmap); return (DESCRIPT_END);}


<indxmap>{DIGITS} { cf_lval.intval = atoi(cf_text);
		     cf_lval.string = NULL;
                     return NUMBER;
                   }

<indxmap>{attr_string}  { 
	     cf_lval.string = strdup(cf_text);  
	     cf_lval.intval = -1;
	     return QUOTED_STRING;
	}

<indxmap>{quoted_name_token}  { 
	     cf_text[strlen(cf_text)-1] = '\0'; /* kill final quote */
	     cf_lval.string = strdup(cf_text+1); 
	     cf_lval.intval = -1;
	     return QUOTED_STRING;
	}


<indxdesc>{anything} {

                     cf_lval.string = strdup(cf_text); 
		     return (DESCRIPTION);
		     }

<exturldef>{anything} {

                     cf_lval.string = strdup(cf_text); 
		     return (URL_TOKEN);
		     }

<explain>"<TITLESTRING" {return( TITLESTRING_START);}
<explain>"<DESCRIPTION" {return( DESCRIPTION_START);}
<explain>"<DISCLAIMERS" {return( DISCLAIMERS_START);}
<explain>"<NEWS" { return( NEWS_START);}
<explain>"<HOURS" { return( HOURS_START);}
<explain>"<BESTTIME" { return( BESTTIME_START);}
<explain>"<LASTUPDATE>" {BEGIN(explaintext); return( LASTUPDATE_START);}
<explain>"<UPDATEINTERVAL>" {
	BEGIN(explainintunit); return( UPDATEINTERVAL_START);}
<explain>"<COVERAGE" { return( COVERAGE_START);}
<explain>"<PROPRIETARY>" {return( PROPRIETARY_START);}
<explain>"TRUE" {return( PROP_TRUE);}
<explain>"YES" {return( PROP_TRUE);}
<explain>"Y" {return( PROP_TRUE);}
<explain>"FALSE" {return( PROP_FALSE);}
<explain>"NO" {return( PROP_FALSE);}
<explain>"N" {return( PROP_FALSE);}

<explain>"<COPYRIGHTTEXT" { return( COPYRIGHTTEXT_START);}
<explain>"<COPYRIGHTNOTICE" { return( COPYRIGHTNOTICE_START);}
<explain>"<PRODUCERCONTACTINFO>" { return( PRODUCERCONTACTINFO_START);}
<explain>"<SUPPLIERCONTACTINFO>" { return( SUPPLIERCONTACTINFO_START);}
<explain>"<SUBMISSIONCONTACTINFO>" { return( SUBMISSIONCONTACTINFO_START);}
<explain>"<CONTACT_NAME>" {BEGIN(explaintext); return( CONTACT_NAME_START);}
<explain>"<CONTACT_DESCRIPTION" { return( CONTACT_DESCRIPTION_START);}
<explain>"<CONTACT_ADDRESS" { return( CONTACT_ADDRESS_START);}
<explain>"<CONTACT_EMAIL>" {BEGIN(explaintext); return( CONTACT_EMAIL_START);}
<explain>"<CONTACT_PHONE>" {BEGIN(explaintext); return( CONTACT_PHONE_START);}

<explain>"LANGUAGE"{eq} { return (LANGUAGE_ATTR);}
<explain>"LANG"{eq} { return (LANGUAGE_ATTR);}
<explain>">" {BEGIN(explaintext); return(ATTRIBUTES_END);}

<explaintext>[^<]* {char *c;
	cf_lval.string = strdup(cf_text); 
	for (c = strchr(cf_text, '\n'); c ; c = strchr(c+1, '\n'))
	   lineno++; 
	BEGIN(explain);
	return EXPLAIN_TEXT;
	}

<explainintunit>"<VALUE>" {return(VALUE_START);}
<explainintunit>"</VALUE>" {return(VALUE_END);}
<explainintunit>"<UNIT>" {return(UNIT_START);}
<explainintunit>"</UNIT>" {return(UNIT_END);}
<explainintunit>"<UNITS>" {return(UNIT_START);}
<explainintunit>"</UNITS>" {return(UNIT_END);}
<explainintunit>{DIGITS} {cf_lval.intval = atoi(cf_text);
                     return NUMBER;
                   }


<explain>"</EXPLAIN>" {BEGIN(INITIAL); return( EXPLAIN_END);}
<explain>"</TITLESTRING>" {return( TITLESTRING_END);}
<explain>"</DESCRIPTION>" {return( DESCRIPTION_END);}
<explain>"</DISCLAIMERS>" {return( DISCLAIMERS_END);}
<explain>"</NEWS>" {return( NEWS_END);}
<explain>"</HOURS>" {return( HOURS_END);}
<explain>"</BESTTIME>" {return( BESTTIME_END);}
<explain>"</LASTUPDATE>" {return( LASTUPDATE_END);}
<explainintunit>"</UPDATEINTERVAL>" {
	BEGIN(explain); return( UPDATEINTERVAL_END);}
<explain>"</COVERAGE>" {return( COVERAGE_END);}
<explain>"</PROPRIETARY>" {return( PROPRIETARY_END);}
<explain>"</COPYRIGHTTEXT>" {return( COPYRIGHTTEXT_END);}
<explain>"</COPYRIGHTNOTICE>" {return( COPYRIGHTNOTICE_END);}
<explain>"</PRODUCERCONTACTINFO>" {return( PRODUCERCONTACTINFO_END);}
<explain>"</SUPPLIERCONTACTINFO>" {return( SUPPLIERCONTACTINFO_END);}
<explain>"</SUBMISSIONCONTACTINFO>" {return( SUBMISSIONCONTACTINFO_END);}
<explain>"</CONTACT_NAME>" {return( CONTACT_NAME_END);}
<explain>"</CONTACT_DESCRIPTION>" {return( CONTACT_DESCRIPTION_END);}
<explain>"</CONTACT_ADDRESS>" {return( CONTACT_ADDRESS_END);}
<explain>"</CONTACT_EMAIL>" {return( CONTACT_EMAIL_END);}
<explain>"</CONTACT_PHONE>" {return( CONTACT_PHONE_END);}


<attributeset>"ATTRIBUTESET"{eq} {return(ATTRIB_ATTR);}
<attributeset>"ATTRIBUTE"{eq} {return(ATTRIB_ATTR);}
<attributeset>"ATTRIB"{eq} {return(ATTRIB_ATTR);}
<attributeset>"ATTR"{eq} {return(ATTRIB_ATTR);}

<attributeset>"1.2.840.10003.3.1" {return (BIB_1);}
<attributeset>"BIB1" {return (BIB_1);}
<attributeset>"BIB-1" {return (BIB_1);}
<attributeset>"BIB_1" {return (BIB_1);}
<attributeset>"1.2.840.10003.3.2" {return (EXP_1);}
<attributeset>"EXP1" {return (EXP_1);}
<attributeset>"EXP-1" {return (EXP_1);}
<attributeset>"EXP_1" {return (EXP_1);}
<attributeset>"1.2.840.10003.3.3" {return (EXT_1);}
<attributeset>"EXT1" {return (EXT_1);}
<attributeset>"EXT-1" {return (EXT_1);}
<attributeset>"EXT_1" {return (EXT_1);}
<attributeset>"1.2.840.10003.3.4" {return (CCL_1);}
<attributeset>"CCL1" {return (CCL_1);}
<attributeset>"CCL-1" {return (CCL_1);}
<attributeset>"CCL_1" {return (CCL_1);}
<attributeset>"1.2.840.10003.3.5" {return (GILS);}
<attributeset>"GILS" {return (GILS);}
<attributeset>"1.2.840.10003.3.6" {return (STAS);}
<attributeset>"STAS" {return (STAS);}
<attributeset>"1.2.840.10003.3.7" {return (COLLECTIONS_1);}
<attributeset>"COLLECTIONS" {return (COLLECTIONS_1);}
<attributeset>"COLLECTIONS-1" {return (COLLECTIONS_1);}
<attributeset>"COLLECTIONS_1" {return (COLLECTIONS_1);}
<attributeset>"1.2.840.10003.3.8" {return (CIMI_1);}
<attributeset>"CIMI" {return (CIMI_1);}
<attributeset>"CIMI-1" {return (CIMI_1);}
<attributeset>"CIMI_1" {return (CIMI_1);}
<attributeset>"1.2.840.10003.3.9" {return (GEO);}
<attributeset>"GEO" {return (GEO);}
<attributeset>"GEO-1" {return (GEO);}
<attributeset>"GEO_1" {return (GEO);}

<attributeset>"1.2.840.10003.3.10" {return (ZBIG);}
<attributeset>"ZBIG" {return (ZBIG);}
<attributeset>"1.2.840.10003.3.11" {return (UTIL);}
<attributeset>"UTIL" {return (UTIL);}
<attributeset>"UTILITY" {return (UTIL);}
<attributeset>"1.2.840.10003.3.12" {return (XD_1);}
<attributeset>"XD" {return (XD_1);}
<attributeset>"CROSS_DOMAIN" {return (XD_1);}
<attributeset>"XDOMAIN" {return (XD_1);}
<attributeset>"XD-1" {return (XD_1);}
<attributeset>"XD_1" {return (XD_1);}
<attributeset>"1.2.840.10003.3.13" {return (ZTHES);}
<attributeset>"ZTHES" {return (ZTHES);}
<attributeset>"THESAURUS" {return (ZTHES);}
<attributeset>"1.2.840.10003.3.14" {return (FIN_1);}
<attributeset>"FIN" {return (FIN_1);}
<attributeset>"FIN-1" {return (FIN_1);}
<attributeset>"FIN_1" {return (FIN_1);}
<attributeset>"1.2.840.10003.3.15" {return (DAN_1);}
<attributeset>"DAN" {return (DAN_1);}
<attributeset>"DAN-1" {return (DAN_1);}
<attributeset>"DAN_1" {return (DAN_1);}
<attributeset>"1.2.840.10003.3.16" {return (HOLDINGS);}
<attributeset>"HOLD" {return (HOLDINGS);}
<attributeset>"HOLDING" {return (HOLDINGS);}
<attributeset>"HOLDINGS" {return (HOLDINGS);}

<attributeset>"\"1.2.840.10003.3.1\"" {return (BIB_1);}
<attributeset>"\"BIB1\"" {return (BIB_1);}
<attributeset>"\"BIB-1\"" {return (BIB_1);}
<attributeset>"\"BIB_1\"" {return (BIB_1);}
<attributeset>"\"1.2.840.10003.3.2\"" {return (EXP_1);}
<attributeset>"\"EXP1\"" {return (EXP_1);}
<attributeset>"\"EXP-1\"" {return (EXP_1);}
<attributeset>"\"EXP_1\"" {return (EXP_1);}
<attributeset>"\"1.2.840.10003.3.3\"" {return (EXT_1);}
<attributeset>"\"EXT1\"" {return (EXT_1);}
<attributeset>"\"EXT-1\"" {return (EXT_1);}
<attributeset>"\"EXT_1\"" {return (EXT_1);}
<attributeset>"\"1.2.840.10003.3.4\"" {return (CCL_1);}
<attributeset>"\"CCL1\"" {return (CCL_1);}
<attributeset>"\"CCL-1\"" {return (CCL_1);}
<attributeset>"\"CCL_1\"" {return (CCL_1);}
<attributeset>"\"1.2.840.10003.3.5\"" {return (GILS);}
<attributeset>"\"GILS\"" {return (GILS);}
<attributeset>"\"1.2.840.10003.3.6\"" {return (STAS);}
<attributeset>"\"STAS\"" {return (STAS);}
<attributeset>"\"1.2.840.10003.3.7\"" {return (COLLECTIONS_1);}
<attributeset>"\"COLLECTIONS\"" {return (COLLECTIONS_1);}
<attributeset>"\"COLLECTIONS-1\"" {return (COLLECTIONS_1);}
<attributeset>"\"COLLECTIONS_1\"" {return (COLLECTIONS_1);}
<attributeset>"\"1.2.840.10003.3.8\"" {return (CIMI_1);}
<attributeset>"\"CIMI\"" {return (CIMI_1);}
<attributeset>"\"CIMI-1\"" {return (CIMI_1);}
<attributeset>"\"CIMI_1\"" {return (CIMI_1);}
<attributeset>"\"1.2.840.10003.3.9\"" {return (GEO);}
<attributeset>"\"GEO\"" {return (GEO);}
<attributeset>"\"GEO-1\"" {return (GEO);}
<attributeset>"\"GEO_1\"" {return (GEO);}

<attributeset>"\"1.2.840.10003.3.10\"" {return (ZBIG);}
<attributeset>"\"ZBIG\"" {return (ZBIG);}
<attributeset>"\"1.2.840.10003.3.11\"" {return (UTIL);}
<attributeset>"\"UTIL\"" {return (UTIL);}
<attributeset>"\"UTILITY\"" {return (UTIL);}
<attributeset>"\"1.2.840.10003.3.12\"" {return (XD_1);}
<attributeset>"\"XD\"" {return (XD_1);}
<attributeset>"\"CROSS_DOMAIN\"" {return (XD_1);}
<attributeset>"\"XDOMAIN\"" {return (XD_1);}
<attributeset>"\"XD-1\"" {return (XD_1);}
<attributeset>"\"XD_1\"" {return (XD_1);}
<attributeset>"\"1.2.840.10003.3.13\"" {return (ZTHES);}
<attributeset>"\"ZTHES\"" {return (ZTHES);}
<attributeset>"\"THESAURUS\"" {return (ZTHES);}
<attributeset>"\"1.2.840.10003.3.14\"" {return (FIN_1);}
<attributeset>"\"FIN\"" {return (FIN_1);}
<attributeset>"\"FIN-1\"" {return (FIN_1);}
<attributeset>"\"FIN_1\"" {return (FIN_1);}
<attributeset>"\"1.2.840.10003.3.15\"" {return (DAN_1);}
<attributeset>"\"DAN\"" {return (DAN_1);}
<attributeset>"\"DAN-1\"" {return (DAN_1);}
<attributeset>"\"DAN_1\"" {return (DAN_1);}
<attributeset>"\"1.2.840.10003.3.16\"" {return (HOLDINGS);}
<attributeset>"\"HOLD\"" {return (HOLDINGS);}
<attributeset>"\"HOLDING\"" {return (HOLDINGS);}
<attributeset>"\"HOLDINGS\"" {return (HOLDINGS);}


<attributeset>{attrsetoid} {
	     cf_text[strlen(cf_text)-1] = '\0'; /* kill final quote */
	     cf_lval.string = strdup(cf_text+1);
	     return (ATTRSETOID);}

<attributeset>">" { BEGIN(indxmap); return (ATTRIBUTES_END);}


<filedef,filedtd,indxdef,filecont,indxcont,clusdef,formatdef,convertdef>">" {
          BEGIN(INITIAL);
          return(ATTRIBUTES_END);
          }


<filecontinc,cfinclude>{url}  { 
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{filename}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{relfilename}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{quoted_filename}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{dosfilename}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{quoteddosfilename}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{quoted_name_token}  { 
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}

<filecontinc,cfinclude>{name_token}  {
          cf_include_buffer_switch(cf_text); 
          BEGIN(INITIAL);
	}


<sqldata>{sqldata}  { cf_lval.string = strdup(cf_text); 
	     return SQL_DATA;
	}

<xpath>{xpathspec}  { cf_lval.string = strdup(cf_text); 
	     return XPATH_SPEC;
	}

<*>{url}  { 
	     if (include_flag != 2) {
	     cf_lval.string = strdup(cf_text); 
	     return URL_TOKEN;
	     }
	}

<*>{filename}  { 
	     if (include_flag != 2) {
	     cf_lval.string = strdup(cf_text); 
	     return FILENAME_LITERAL;
	     }
	}

<*>{relfilename}  { 
	     if (include_flag != 2) {
	     cf_lval.string = strdup(cf_text); 
	     return FILENAME_LITERAL;
	     }
	}

<*>{quoted_filename}  {
	     if (include_flag != 2) {

             cf_lval.string = strdup(cf_text); 
	     cf_text[strlen(cf_text)-1] = '\0'; /* kill final quote */
	     cf_lval.string = strdup(cf_text+1); 
	     cf_lval.intval = FILENAME_LITERAL;
	     return FILENAME_LITERAL;
	     }
	}

<*>{dosfilename}  { 
		     if (include_flag != 2) {
		         cf_lval.string = strdup(cf_text); 
			 return FILENAME_LITERAL;
		  }
	}

<*>{dos_path_token}  { 

		     if (include_flag != 2) {
		       cf_lval.string = strdup(cf_text); 
		       return FILENAME_LITERAL;
		     }
	}

<*>{quoteddosfilename}  { 
	     if (include_flag != 2) {
	     cf_lval.string = strdup(cf_text); 
	     cf_text[strlen(cf_text)-1] = '\0'; /* kill final quote */
	     cf_lval.string = strdup(cf_text+1); 
	     cf_lval.intval = FILENAME_LITERAL;
	     return FILENAME_LITERAL;
	     }
	}

<*>{quoted_name_token}  { 
	     cf_text[strlen(cf_text)-1] = '\0'; /* kill final quote */
	     cf_lval.string = strdup(cf_text+1); 
	     cf_lval.intval = NAME_TOKEN;
	     return NAME_TOKEN;
	}


<*>{name_token}  { cf_lval.string = strdup(cf_text); 
	     cf_lval.intval = NAME_TOKEN;
	     return NAME_TOKEN;
	}

<*>{nl}  { lineno++; /* count and ignore newlines */}

<*>{one_line_comment} ; 

<*>{comments_start} {
   save_state = YY_START;
   BEGIN(sgml_comment);} /* ignore comments */


<sgml_comment>{comments_end} {
   BEGIN(save_state);
}

<sgml_comment>.* { ; /*ignore any other char*/ }


<*>{ws} /* ignore whitespace */;

.	{ printf("Unrecognized char ='%c' in cf_parser\n",
		cf_text[0]); 
	  if (strlen(cf_text) > 1)
	     printf("Unmatched string: %s\n", cf_text);
	  return cf_text[0];
	}


<<EOF>> { /* end of an input file */	
#ifdef DEBUGINCLUDE
  fprintf(stdout,"<<EOF>> encountered for include stack ptr %d\n",
	  include_state); 
#endif
  if (--cf_include_stack_ptr <= 0) {
#ifdef DEBUGINCLUDE
    printf("EOF HIT ptr == %d -- terminating\n", cf_include_stack_ptr); 
#endif
    cf_include_stack_ptr = 0;
    yyterminate();
  }
  else {
#ifdef DEBUGINCLUDE
      printf("EOF HIT include stack ptr %d\n", cf_include_stack_ptr ); 
#endif
      fclose(yyin);
      yy_delete_buffer(YY_CURRENT_BUFFER); 
      yy_switch_to_buffer(cf_include_stack[cf_include_stack_ptr]);
      yyin = cf_include_stack[cf_include_stack_ptr]->yy_input_file;
      /* cf_include_stack_ptr = 0; */

      BEGIN(INITIAL);
  }

}



%%



int 
next_token()
{
   int token;
   /* check if there is a token pushed back or read ahead */
   if (cf_lval_pushback.tok_id != 0) {
	cf_lval = cf_lval_pushback;
        cf_lval_pushback.tok_id = 0;
	return (cf_lval.tok_id);
   }
   cf_lval.intval = 0;
   /* otherwise read the next token */
   token = cf_lex();

   /* printf("TOKEN %d\n", token); */

   cf_lval.tok_id = token;
   return (token);
} 

int push_back_token(int token)
{
    cf_lval_pushback = cf_lval;
    cf_lval_pushback.tok_id = token;
}


/* this routine is called by the parser created by LLgen */
int CFmessage(int token)
{
   extern int CFsymb;

  /* fprintf(stdout,"CFmessage called: %d\n", token); */

   switch (token) {
	case -1: 
		break;
	
	case 0: 
		fprintf(LOGFILE,"Config file parsing problem at line %d\n",
			lineno);
		config_parse_error = 1;
		/* fprintf(LOGFILE,"EXITING\n"); */
		/* exit(1); */
		break;
	default: 
		fprintf(LOGFILE,"Config file parsing problem at line %d\n",
			lineno);
		fprintf(LOGFILE,"pushing back token type %d\n",token);

		push_back_token(token);

		break;
   }

}

int cf_restart_scanner(FILE *newfile) 
{
	yyrestart(newfile);
	cf_include_stack_ptr = 0;
	BEGIN(INITIAL);
}


char *cf_build_full_name( char *in_name)
{
  char *path_name = NULL; 
  char *getcwd();
  char *full_name = NULL;
  int path_len = 0;
  int name_len = 0;
  struct stat filestatus;
  
  name_len = strlen(in_name);
  
  if (in_name[0] == '/') /* assume it is already a path name */
    return (strdup(in_name));
  
  /* Under NT must handle 'C:\...' type names */
  if (in_name[1] == ':' 
      && (in_name[2] == '\\' || in_name[2] == '/'))
    return (strdup(in_name));
  
  /* 
   * The name is not a full path name... so...
   */
  if (default_path != NULL) {
    /* if this is set, it should be the real data file */
    /* we will use it as the path name for the default */
    /* directory for this file                         */
    full_name = CALLOC(char, (strlen(default_path)
			      + name_len + 10));
#ifdef WIN32
    sprintf(full_name,"%s\\%s",default_path,in_name);
#else
    sprintf(full_name,"%s/%s",default_path,in_name);
#endif

  } else {
    /* otherwise assume that the name only is provided   */
    path_name = getcwd (NULL, 500);
    path_len = strlen(path_name);
    
    full_name = CALLOC(char, (path_len + name_len + 10));
#ifdef WIN32
    sprintf(full_name,"%s\\%s",path_name,in_name);
#else
    sprintf(full_name,"%s/%s",path_name,in_name);
#endif
  }
  

  if (stat(full_name, &filestatus) != 0) {
    FREE(full_name);
    return (NULL);
  }	  
  
  return(full_name);
}



int cf_include_buffer_switch (char *new_file_name)
{
  char *path_name = NULL, *tmp = NULL;
  extern int sgml_symb;

  cf_include_stack[cf_include_stack_ptr++] = YY_CURRENT_BUFFER;

  path_name = cf_build_full_name(new_file_name);

  if (path_name == NULL) {
    fprintf(LOGFILE,
    	    "cf_parse.flex: Could not open config continuation include file '%s'\n",
	        new_file_name); 
    
    cheshire_exit(1);
  }

  yyin = fopen(path_name,"r");

  if (!yyin) {
    fprintf(LOGFILE,
	    "cf_parse.flex: Could not open include file '%s'\n",
	    new_file_name); 
    
    cheshire_exit(1);
  }

#ifdef DEBUGINCLUDE
  printf("opened include file %s yyin = %x stack ptr %d\n", 
          path_name, yyin, cf_include_stack_ptr);	
#endif

  yy_switch_to_buffer(yy_create_buffer(yyin,YY_BUF_SIZE));

  prev_state[include_state++]= YY_START;
#ifdef DEBUGINCLUDE
  fprintf(stdout,"cf_include buffer switch: include_state = %d\n", include_state); 
#endif
  
  BEGIN(INITIAL);

  FREE(path_name);
}



int cf_wrap () 
{
 /*  fprintf(LOGFILE,"cf_wrap -- End of input file\n"); */
 /*  yy_delete_buffer(YY_CURRENT_BUFFER); */
  return (1); 
}


