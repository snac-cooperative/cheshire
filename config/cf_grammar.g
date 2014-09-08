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
*	Header Name:	cf_grammar.g
*                       this is a test LLgen grammar file 
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	defines the grammar for config files
*
*	Usage:		LLgen the file then call LLparse
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
{
#include <stdlib.h>
#include <stdio.h>
#include "cheshire.h"
#include "configfiles.h"
#include "z_parameters.h"
#include "z3950_3.h"

/* Solaris doesn't have bzero, so the following maps it */
#ifdef SOLARIS
#ifndef DMALLOC_INCLUDE
#define bzero(x,y)  memset(x, 0, y)
#endif
#endif
#ifdef ALPHA
#ifndef DMALLOC_INCLUDE
#define bzero(x,y)  memset(x, 0, y)
#endif
#endif

#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif


/* cf_lval contains the current values from the lexical analysis */

struct cf_LVAL { 
        int  tok_id;
        char *string;
        int intval;
 } cf_lval, cf_lval_pushback;
 
  extern config_file_info *cf_info_base; /* global should be in main */
  int config_parse_error = 0;
  config_file_info *cf_info, *tmp_cf_info, *base_cf_info;
  idx_list_entry *idx_list, *prev_idx_list;
  display_format *format, *prev_format;
  cluster_list_entry *clus_list, *prev_clus_list;
  component_list_entry *comp_list, *prev_comp_list;
  cluster_map_entry *clus_map, *prev_clus_map, temp_clus_map; 
  idx_key *idx_keys, *prev_idx_key, *idx_subkey, *prev_subkey, 
	*subkey_parent, *head_idx_keys;
  ranking_parameters *head_rank_parms, *rank_parm;
  int current_rank_type = 0;
  attr_map *attr_maps = NULL, *prev_attr_map = NULL, *last_attr_map = NULL ;
  int n_indexes;
  continuation *new_cont, *last_cont;
  int subkey_nesting = 0;
  char *default_path;
  static char *explain_lang;
  char *default_attr_set = NULL;	
  HumanString *BuildHumanString(char *string, char *language);
  InternationalString NewInternationalString(char *string);
  StringOrNumeric *BuildStringOrNumeric(char *string, int numeric, int which);
  GSTR *NewGSTR(char *string);
  GeneralizedTime NewGeneralizedTime(char* str); 
  idx_key *cf_build_xpath_key(char *element_name) ;

  ContactInfo *tmp_contact;
  char *db_env_pathname = NULL;
}
/* force the prefix of generated routines to CF instead of LL */
%prefix CF;

/* Config file tag defs */
%token DBCONFIG_START;
%token DBCONFIG_END;

%token DBENV_START;
%token DBENV_END;

%token FILEDEF_START;
%token FILENAME_START;
%token FILETAG_START;
%token FILECONT_START;
%token FILEENC_START;
%token DEFAULTPATH_START;
%token FILEDTD_START;
%token FILEXMLSCHEMA_START;
%token SGMLCAT_START;
%token ASSOCFIL_START;
%token HISTORY_START;
%token DISTRECCOUNT_START;

%token INDEXES_START;
%token INDEXDEF_START;
%token INDXNAME_START;
%token INDXTAG_START;
%token INDXMAP_START;
%token INDXCONT_START;
%token STOPLIST_START;
%token EXPANSIONTERMS_START;
%token INDXKEY_START;
%token EXTERN_APP_START;
%token RANK_PARAMS_START;
%token RANK_TYPE_ATTR;
%token LR_RANK;
%token LR_RANK_TREC2;
%token LM_RANK;
%token OK_RANK;
%token TFIDF_RANK;
%token PARAM_START;
%token PARAM_ID_ATTR;
%token PARAM_VALUE;
%token INDXEXC_START;
%token TAGSPEC_START;
%token XPATH_START;
%token SQL_START;
%token FTAG_START;
%token S_START;
%token ATTR_START;
%token ATTRVAL_START;
%token USE_START;
%token RELATION_START;
%token POSITION_START;
%token STRUCT_START;
%token TRUNC_START;
%token COMPLET_START;
%token ACCESS_START;
%token SEMANTIC_START;
%token LANGUAGE_START;
%token CONT_AUTH_START;
%token EXPANSION_START;
%token NORM_WEIGHT_START;
%token HIT_COUNT_START;
%token COMPARISON_START;
%token FORMAT_ATTR_START;
%token OCCURRENCE_START;
%token INDIRECTION_START;
%token FUNC_QUAL_START;
%token DESCRIPT_START;
%token DESCRIPTION;
%token COMPONENTS_START;
%token COMPONENTDEF_START;
%token COMPONENTNAME_START;
%token COMPONENTNORM_START;
%token COMPONENTSTORE_START;
%token COMPTAGSTART_START;
%token COMPTAGEND_START;
%token COMPINDEXES_START;
%token CLUSTERS_START;
%token CLUSTER_START;
%token CLUSTERDEF_START;
%token CLUSBASE_START;
%token CLUSNAME_START;
%token CLUSKEY_START;
%token CLUSMAP_START;
%token FROM_START;
%token TO_START;
%token SUMMARIZE_START;
%token MAXNUM_START;
%token DISPOPTIONS_START;
%token KEEPAMP;
%token KEEPLT;
%token KEEPGT;
%token KEEPALL;
%token KEEPENT;
%token DISPLAY_START;
%token FORMAT_START;
%token INCLUDE_START;
%token EXCLUDE_START;
%token COMPRESS_YES;
%token COMPRESS_NO;
%token STORE_YES;
%token STORE_NO;
%token CONVERT_START;

%token EXPLAIN_START;
%token TITLESTRING_START;
%token DESCRIPTION_START;
%token DISCLAIMERS_START;
%token NEWS_START;
%token HOURS_START;
%token BESTTIME_START;
%token LASTUPDATE_START;
%token UPDATEINTERVAL_START;
%token VALUE_START;
%token REPLACEVAL_START;
%token UNIT_START;
%token COVERAGE_START;
%token PROPRIETARY_START;
%token COPYRIGHTTEXT_START;
%token COPYRIGHTNOTICE_START;
%token PRODUCERCONTACTINFO_START;
%token SUPPLIERCONTACTINFO_START;
%token SUBMISSIONCONTACTINFO_START;
%token CONTACT_NAME_START;
%token CONTACT_DESCRIPTION_START;
%token CONTACT_ADDRESS_START;
%token CONTACT_EMAIL_START;
%token CONTACT_PHONE_START;

%token FILEDEF_END;
%token FILENAME_END;
%token FILETAG_END;
%token FILECONT_END;
%token FILEENC_END;
%token UTF_8_ENC;
%token KOI8_ENC;
%token ISO_8859_1_ENC;
%token DEFAULTPATH_END;
%token FILEDTD_END;
%token FILEXMLSCHEMA_END;
%token SGMLCAT_END;
%token ASSOCFIL_END;
%token HISTORY_END;
%token OUTFILE_ATTR;
%token DISTRECCOUNT_END;

%token INDEXES_END;
%token INDEXDEF_END;
%token INDXNAME_END;
%token INDXTAG_END;
%token INDXMAP_END;
%token INDXCONT_END;
%token STOPLIST_END;
%token EXPANSIONTERMS_END;
%token EXTERN_APP_END;
%token INDXKEY_END;
%token RANK_PARAMS_END;
%token PARAM_END;
%token INDXEXC_END;
%token TAGSPEC_END;
%token XPATH_END;
%token SQL_END;
%token XPATH_SPEC;
%token SQL_DATA;
%token FTAG_END;
%token S_END;
%token ATTR_END;
%token ATTRVAL_END;
%token USE_END;
%token RELATION_END;
%token POSITION_END;
%token STRUCT_END;
%token TRUNC_END;
%token COMPLET_END;
%token ACCESS_END;
%token SEMANTIC_END;
%token LANGUAGE_END;
%token CONT_AUTH_END;
%token EXPANSION_END;
%token NORM_WEIGHT_END;
%token HIT_COUNT_END;
%token COMPARISON_END;
%token FORMAT_ATTR_END;
%token OCCURRENCE_END;
%token INDIRECTION_END;
%token FUNC_QUAL_END;
%token DESCRIPT_END;
%token COMPONENTS_END;
%token COMPONENTDEF_END;
%token COMPONENTNAME_END;
%token COMPONENTNORM_END;
%token COMPONENTSTORE_END;
%token COMPTAGSTART_END;
%token COMPTAGEND_END;
%token COMPINDEXES_END;
%token CLUSTERS_END;
%token CLUSTER_END;
%token CLUSTERDEF_END;
%token CLUSBASE_END;
%token CLUSNAME_END;
%token CLUSKEY_END;
%token CLUSMAP_END;
%token FROM_END;
%token TO_END;
%token SUMMARIZE_END;
%token MAXNUM_END;
%token DISPOPTIONS_END;
%token DISPLAY_END;
%token FORMAT_END;
%token INCLUDE_END;
%token EXCLUDE_END;
%token CONVERT_END;

%token EXPLAIN_END;
%token TITLESTRING_END;
%token DESCRIPTION_END;
%token DISCLAIMERS_END;
%token NEWS_END;
%token HOURS_END;
%token BESTTIME_END;
%token LASTUPDATE_END;
%token UPDATEINTERVAL_END;
%token VALUE_END;
%token REPLACEVAL_END;
%token UNIT_END;
%token COVERAGE_END;
%token PROPRIETARY_END;
%token COPYRIGHTTEXT_END;
%token COPYRIGHTNOTICE_END;
%token PRODUCERCONTACTINFO_END;
%token SUPPLIERCONTACTINFO_END;
%token SUBMISSIONCONTACTINFO_END;
%token CONTACT_NAME_END;
%token CONTACT_DESCRIPTION_END;
%token CONTACT_ADDRESS_END;
%token CONTACT_EMAIL_END;
%token CONTACT_PHONE_END;
%token EXPLAIN_TEXT;


%token LANGUAGE_ATTR;
%token TYPE_ATTR;
%token ACCESS_ATTR;
%token EXTRACT_ATTR;
%token NORMAL_ATTR;
%token PRIMARYKEY_ATTR;
%token FORMAT_NAME_ATTR;
%token OID_NAME_ATTR;
%token MARC_OID;
%token EXP_OID;
%token SUTRS_OID;
%token SGML_OID;
%token XML_OID;
%token HTML_OID;
%token OPAC_OID;
%token SUMM_OID;
%token GRS0_OID;
%token GRS1_OID;
%token ES_OID;

%token MARC_DTD_FILE_ATTR;
%token DEFAULT_ATTR;
%token FUNCTION_ATTR;
%token ATTRIB_ATTR;
%token ALL_ATTR;

%token SGML_TYPE;
%token XML_TYPE;
%token XML_NODTD_TYPE;
%token MARC_TYPE;
%token AUTH_TYPE;
%token CLUSTER_TYPE;
%token LCCTREE_TYPE;
%token MAPPED_TYPE;
%token DBMS_TYPE;
%token DBMS_BTREE;
%token EXPLAIN_TYPE;
%token VIRTUAL_TYPE;
%token SGML_DATASTORE_TYPE;
%token MARC_DATASTORE_TYPE;
%token CLUSTER_DATASTORE_TYPE;
%token EXPLAIN_DATASTORE_TYPE;
%token XMLSCHEMA_TYPE;

%token BTREE;
%token HASH;
%token VECTOR;
%token BITMAPPED;
%token CODED_ELEMENT;
%token KEYWORD_EXTRACT;
%token KEYWORD_PROX_EXTRACT;
%token KEYWORD_EXTERN_EXTRACT;
%token KEYWORD_EXTERN_PROX_EXTRACT;
%token EXACTKEY_EXTRACT;
%token EXACTKEY_PROX_EXTRACT;
%token FLD008_EXTRACT;
%token FLD008_DATE;
%token FLD008_DATERANGE;
%token INTEGER_EXTRACT;
%token DECIMAL_EXTRACT;
%token FLOAT_EXTRACT;
%token URL_EXTRACT;
%token FILENAME_EXTRACT;
%token LAT_LONG_EXTRACT;
%token BOUNDING_BOX_EXTRACT;
%token GEOTEXT_EXTRACT;
%token GEOTEXT_LAT_LONG_EXTRACT;
%token GEOTEXT_BOUNDING_BOX_EXTRACT;
%token DATE_EXTRACT;
%token DATE_RANGE_EXTRACT;
%token NGRAM_EXTRACT;
%token STEM_NORM;
%token STEMFREQ_NORM;
%token SSTEM_NORM;
%token SNOWBALL_STEM_NORM;
%token SNOWBALL_STEMFREQ_NORM;
%token SSTEMFREQ_NORM;
%token WORDNET_NORM;
%token CLASSCLUS_NORM;
%token XKEY_NORM;
%token XKEYFREQ_NORM;
%token NONE_NORM;
%token NONEFREQ_NORM;
%token DONT_NORMALIZE;
%token DONT_NORMALIZE_FREQ;
%token MIN_NORMALIZE;
%token MIN_NORMALIZE_FREQ;
%token STEM_NOMAP_NORM;
%token SSTEM_NOMAP_NORM;
%token WORDNET_NOMAP_NORM;
%token CLASSCLUS_NOMAP_NORM;
%token XKEY_NOMAP_NORM;
%token NONE_NOMAP_NORM;
%token LAT_LONG_NORM;
%token BOUNDING_BOX_NORM;
%token FGDC_BOUNDING_BOX_NORM;
%token DATE_NORMALIZE;
%token DATE_RANGE_NORM;
%token PRIMARYKEY_IGNORE;
%token PRIMARYKEY_REPLACE;
%token PRIMARYKEY_NO;
%token BIB_1;
%token EXP_1;
%token EXT_1;
%token CCL_1;
%token GILS;
%token GEO;
%token STAS;
%token COLLECTIONS_1;
%token CIMI_1;
%token ZBIG;
%token UTIL;
%token XD_1;
%token ZTHES;
%token FIN_1;
%token DAN_1;
%token HOLDINGS;
%token ATTRSETOID;
%token PROP_TRUE;
%token PROP_FALSE;

%token FILECONT_NUMBER;
%token FILECONT_MIN;
%token FILECONT_MAX;
%token FILECONT_DB;
%token INDXCONT_NUMBER;
%token QUOTED_STRING;
%token ATTRIBUTES_END;

/* string values */
%token FILENAME_LITERAL;
%token URL_TOKEN;
%token NAME_TOKEN;
%token TAG_PATTERN;

/* number values */
%token NUMBER;

%start CFparse, dbconfig ;

 dbconfig: DBCONFIG_START {	
 		/* initialize some variables */
		if (cf_info_base == NULL) {
			cf_info = NULL;
			default_path = NULL;
			base_cf_info = NULL;
			idx_list = NULL;
			prev_idx_list = NULL;
			clus_list = NULL;
			prev_clus_list = NULL;
			clus_map = NULL;
			prev_clus_map = NULL;
  			idx_keys = NULL;
  			prev_idx_key = NULL;
  			n_indexes = 0;
		}
		else {/* this is an additional config file */
                      /* probably for another database     */
			base_cf_info = cf_info_base;
			n_indexes = 0;
			idx_list = NULL;
			prev_idx_list = NULL;
			attr_maps = NULL;
			prev_attr_map = NULL;
			last_attr_map = NULL;
			clus_list = NULL;
			prev_clus_list = NULL;
			clus_map = NULL;
			prev_clus_map = NULL;
  			idx_keys = NULL;
  			prev_idx_key = NULL;
		}
	   }
	   dbenv?			
           filedef+
           
           DBCONFIG_END ;

dbenv: DBENV_START
       [FILENAME_LITERAL | NAME_TOKEN]  {
			db_env_pathname = cf_lval.string;
			} 
	DBENV_END ;
       

filedef: FILEDEF_START {
	 		config_parse_error = 0;
 		  	if ((tmp_cf_info = CALLOC(config_file_info,1)) 
			   == NULL) {
    			fprintf(LOGFILE, "No malloc space for cf_info\n");
    			exit (1);
  			}
			if (cf_file_names == NULL) {
			    cf_file_names = &cf_file_names_data;
			    Tcl_InitHashTable(cf_file_names,TCL_STRING_KEYS);
			}
			/* save the pointer to the head of the list */
  	  		if (base_cf_info == NULL)
  				base_cf_info = cf_info = tmp_cf_info;
  			else {
  			    cf_info->next_file_info = tmp_cf_info;
  			    cf_info = tmp_cf_info;
  			} 
  			/* set the DB environment name most recent (or null) */
  			cf_info->dbenv_path = db_env_pathname; 
  			}
         TYPE_ATTR 
         	[ SGML_TYPE { cf_info->file_type = FILE_TYPE_SGMLFILE;}
         	| XML_TYPE { cf_info->file_type = FILE_TYPE_XML;}
         	| XML_NODTD_TYPE { cf_info->file_type = FILE_TYPE_XML_NODTD;}
         	| MARC_TYPE { cf_info->file_type = FILE_TYPE_MARCFILE;}
         	| AUTH_TYPE { cf_info->file_type = FILE_TYPE_AUTHORITYFILE;}
		| CLUSTER_TYPE { cf_info->file_type = FILE_TYPE_CLUSTER;}
		| MAPPED_TYPE { cf_info->file_type = FILE_TYPE_MAPPED;}
		| LCCTREE_TYPE { cf_info->file_type = FILE_TYPE_LCCTREE;}
		| DBMS_TYPE {cf_info->file_type = FILE_TYPE_DBMS;}
		| EXPLAIN_TYPE {cf_info->file_type = FILE_TYPE_EXPLAIN;}
		| VIRTUAL_TYPE {cf_info->file_type = FILE_TYPE_VIRTUALDB;}
		| SGML_DATASTORE_TYPE { cf_info->file_type = FILE_TYPE_SGML_DATASTORE;}
         	| MARC_DATASTORE_TYPE { cf_info->file_type = FILE_TYPE_MARC_DATASTORE;}
		| CLUSTER_DATASTORE_TYPE { cf_info->file_type = FILE_TYPE_CLUSTER_DATASTORE;}
		| EXPLAIN_DATASTORE_TYPE {cf_info->file_type = FILE_TYPE_EXPLAIN_DATASTORE;}
		] ATTRIBUTES_END
	 defaultpath?
         [filename | filetag]+ 
         filecont* 
         [filedtd | sgmlcat | explain | filexmlschema | distreccount | fileenc]*
         assocfil?
         history?
         indexes* 
	 clusters* 
	 components*
	 dispoptions*
	 displays*
         FILEDEF_END { 

	        if (config_parse_error == 1) {
		/* something broken in this filedef */
		   if (tmp_cf_info == base_cf_info) {
		      base_cf_info = NULL;
		   }
		   else {
		      for (cf_info = base_cf_info; 
  			    cf_info->next_file_info != tmp_cf_info;
			    cf_info = cf_info->next_file_info) ;
		      cf_info->next_file_info = NULL;
		   } 
		   cf_clear_tables(tmp_cf_info);
		   cf_free_info(tmp_cf_info);
		}
		config_parse_error = 0;
		prev_clus_list = NULL; clus_list = NULL;
	 } ;


defaultpath: DEFAULTPATH_START
       [FILENAME_LITERAL | NAME_TOKEN]  {
	cf_info->defaultpath = strdup(cf_lval.string);
	default_path = cf_info->defaultpath;
	if (cf_info->defaultpath[strlen(cf_info->defaultpath)-1] == '/'
	    || cf_info->defaultpath[strlen(cf_info->defaultpath)-1] == '\\')
	   cf_info->defaultpath[strlen(cf_info->defaultpath)-1] = '\0';
	} 
	DEFAULTPATH_END ;
       


filename:  FILENAME_START
           [FILENAME_LITERAL | NAME_TOKEN]   { int exists;
					       int i;
					       char *tmp;
					       int firstname;
					       filelist *newname;
			if (cf_info->file_name == NULL) {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       cf_info->file_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       cf_info->file_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(cf_info->file_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(cf_info->file_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       cf_info->file_name = strdup(cf_lval.string);
			    }
			   firstname = 1;
			}			    
			else {
			   firstname = 0;
			    newname = CALLOC(filelist, 1);
			    newname->filename = strdup(cf_lval.string);
			    if (cf_info->othernames == NULL) {
			       cf_info->othernames = newname;
			    }
			    else {
			       newname->next_filename = cf_info->othernames;
			       cf_info->othernames = newname;
			    }
			}
			for (i = 0; i < strlen(cf_lval.string); i++) {
			      cf_lval.string[i] = toupper(cf_lval.string[i]);
			}

		       if ((cf_info->file_type == FILE_TYPE_VIRTUALDB &&
		           firstname == 1) || cf_info->file_type != FILE_TYPE_VIRTUALDB ) {
		         Tcl_SetHashValue(
			     Tcl_CreateHashEntry(
				  	       cf_file_names,
					       cf_lval.string,
					       &exists),
			     (ClientData)cf_info);

			  tmp = strdup(cf_info->file_name);
			  for (i = 0; i < strlen(tmp); i++) {
			        tmp[i] = toupper(tmp[i]);
			  }
		          Tcl_SetHashValue(
			      Tcl_CreateHashEntry(
				 	       cf_file_names,
					       tmp,
					       &exists),
			      (ClientData)cf_info);

		        FREE(tmp);
		       }
	   } 
           FILENAME_END;

filetag: FILETAG_START 
         NAME_TOKEN  { int exists;
            int i;
            int firstname;
            filelist *newname;
            
            if (cf_info->nickname == NULL) {
                cf_info->nickname = strdup(cf_lval.string);
                firstname = 1;
            }
			else {
			    firstname = 0;
			    newname = CALLOC(filelist, 1);
			    newname->filename = strdup(cf_lval.string);
			    if (cf_info->othernames == NULL) {
                    cf_info->othernames = newname;
			    }
			    else {
                    newname->next_filename = cf_info->othernames;
                    cf_info->othernames = newname;
			    }
			}
            
            if ((cf_info->file_type == FILE_TYPE_VIRTUALDB &&
                firstname == 1) || cf_info->file_type != FILE_TYPE_VIRTUALDB ) {
                for (i = 0; i < strlen(cf_lval.string); i++) {
			        cf_lval.string[i] = toupper(cf_lval.string[i]);
                }
                Tcl_SetHashValue(
                    Tcl_CreateHashEntry(
                        cf_file_names,
                        cf_lval.string,
                    &exists),
                    (ClientData)cf_info);
            }

        }
        FILETAG_END ;

filecont: FILECONT_START  { 
			  	if ((new_cont = CALLOC(continuation, 1))
					 == NULL) {
    				fprintf(LOGFILE, "No malloc space for file continuation\n");
    				exit (1);
  				}
  				/* initialize the stucture area to zeros or NULLs */
	  			/* bzero(new_cont, sizeof(struct continuation)); */
  			
  				if (cf_info->filecont == NULL) {
  					cf_info->filecont = new_cont;
  					last_cont = new_cont;
  				}
	  			else {
  				    last_cont->next_cont = new_cont;
  				    last_cont = new_cont;
  				}
			} 
          FILECONT_NUMBER 
		  NUMBER { last_cont->id_number = cf_lval.intval; }
          FILECONT_MIN 
		  NUMBER { last_cont->docid_min = cf_lval.intval; }
          FILECONT_MAX 
		  NUMBER { last_cont->docid_max = cf_lval.intval; }
          [FILECONT_DB 
           NUMBER { last_cont->dbflag = 1; }
          ]?         
          ATTRIBUTES_END
          [FILENAME_LITERAL | NAME_TOKEN] {

			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       last_cont->name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       last_cont->name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(last_cont->name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(last_cont->name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       last_cont->name = strdup(cf_lval.string);
			    }
	  }
          FILECONT_END ;


fileenc: FILEENC_START 
         [UTF_8_ENC { 
	   cf_info->encoding = 1; 
	    
         }
         | KOI8_ENC { 
	   cf_info->encoding = 2; 
	    
         }
         | ISO_8859_1_ENC {
	   cf_info->encoding = 0; /* the default encoding */
         }]
         FILEENC_END ;


filedtd: FILEDTD_START 
         [TYPE_ATTR
         	[ SGML_TYPE { cf_info->XML_Schema = 0;}
		  | XML_TYPE { cf_info->XML_Schema = 1;}
		  | XMLSCHEMA_TYPE { cf_info->XML_Schema = 2;}
		]			
	  ]? ATTRIBUTES_END

        [FILENAME_LITERAL | NAME_TOKEN] {

            if (cf_lval.string[0] == '/'
            || (cf_lval.string[1] == ':'
                && cf_lval.string[2] == '\\'))
            cf_info->DTD_name = strdup(cf_lval.string);
            else if (cf_info->defaultpath) {
                cf_info->DTD_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
                sprintf(cf_info->DTD_name, "%s/%s",
                    cf_info->defaultpath, cf_lval.string);
#else
                sprintf(cf_info->DTD_name, "%s\\%s",
                    cf_info->defaultpath, cf_lval.string);
#endif
            } else {
                cf_info->DTD_name = strdup(cf_lval.string);
            }
        }
        FILEDTD_END;
		 
filexmlschema: FILEXMLSCHEMA_START 

	  [FILENAME_LITERAL | NAME_TOKEN] {
            filelist *newschema;

            if (cf_info->XML_Schema_Name == NULL) {
                if (cf_lval.string[0] == '/'
                || (cf_lval.string[1] == ':'
                    && cf_lval.string[2] == '\\'))
                cf_info->XML_Schema_Name = strdup(cf_lval.string);
                else if (cf_info->defaultpath) {
                    cf_info->XML_Schema_Name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
                    sprintf(cf_info->XML_Schema_Name, "%s/%s",
                        cf_info->defaultpath, cf_lval.string);
#else
                    sprintf(cf_info->XML_Schema_Name, "%s\\%s",
                        cf_info->defaultpath, cf_lval.string);
#endif
                } else {
                    cf_info->XML_Schema_Name = strdup(cf_lval.string);
                }
            }
            else { /* this is an added schema part... */
			    newschema = CALLOC(filelist, 1);

                if (cf_lval.string[0] == '/'
                || (cf_lval.string[1] == ':'
                    && cf_lval.string[2] == '\\'))
                newschema->filename = strdup(cf_lval.string);
                else if (cf_info->defaultpath) {
                    newschema->filename = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
                    sprintf(newschema->filename, "%s/%s",
                        cf_info->defaultpath, cf_lval.string);
#else
                    sprintf(newschema->filename, "%s\\%s",
                        cf_info->defaultpath, cf_lval.string);
#endif
                } else {
                    newschema->filename = strdup(cf_lval.string);
                }

			    if (cf_info->included_schemas == NULL) {
                    cf_info->included_schemas = newschema;
			    }
			    else {
                    newschema->next_filename = cf_info->included_schemas;
                    cf_info->included_schemas = newschema;
			    }
                
            }
        }
        FILEXMLSCHEMA_END;

distreccount: DISTRECCOUNT_START
              tagspec
              DISTRECCOUNT_END {
                cf_info->distrib_docsize_key = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
       
              };

sgmlcat: SGMLCAT_START 
		 [FILENAME_LITERAL | NAME_TOKEN] {

			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       cf_info->SGML_Catalog_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       cf_info->SGML_Catalog_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+5);
#ifndef WIN32
			       sprintf(cf_info->SGML_Catalog_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(cf_info->SGML_Catalog_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif

			    } else {
			       cf_info->SGML_Catalog_name = strdup(cf_lval.string);
			    }
		 }
		 SGMLCAT_END;
		 
assocfil: ASSOCFIL_START 
		  [FILENAME_LITERAL | NAME_TOKEN] {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       cf_info->assoc_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       cf_info->assoc_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(cf_info->assoc_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(cf_info->assoc_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       cf_info->assoc_name = strdup(cf_lval.string);
			    }
		  }
		  ASSOCFIL_END;

history: HISTORY_START 
		 [FILENAME_LITERAL | NAME_TOKEN] {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       cf_info->history_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       cf_info->history_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(cf_info->history_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(cf_info->history_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       cf_info->history_name = strdup(cf_lval.string);
			    }
		 }
		 HISTORY_END;

indexes: 	INDEXES_START
			indexdef+ 
			INDEXES_END {
				cf_info->num_indexes = n_indexes;
				idx_list = NULL;
				prev_idx_list = NULL;
			};

indexdef: INDEXDEF_START {
				/* start new idx_list_entry stucture */
				n_indexes++;
                head_rank_parms = NULL; 
				prev_attr_map = NULL;
				attr_maps = NULL;

				prev_idx_list = idx_list;	
				if ((idx_list = CALLOC(idx_list_entry,1)) 
					    == NULL) {
				  fprintf(LOGFILE, "No malloc space for idx_list_entry\n");
				  exit (1);
				}
				if (prev_idx_list == NULL) {
				  /* must be the first index entry in the list */
				  cf_info->indexes = idx_list;
				  
				}
				else
				  prev_idx_list->next_entry = idx_list;

			}
			

          ACCESS_ATTR 
          	[BTREE	{idx_list->type |= DB_BTREE_TYPE;}
          	| HASH	{idx_list->type |= DB_HASH_TYPE;}
          	| VECTOR{idx_list->type |= VECTOR_TYPE ;}
		| DBMS_TYPE {idx_list->type |= DB_DBMS_TYPE ;}
		| DBMS_BTREE {idx_list->type |= DB_DBMS_TYPE | DB_MASK ;}
		| BITMAPPED {idx_list->type |= BITMAPPED_TYPE ;}
          	]

          EXTRACT_ATTR 
          	[KEYWORD_EXTRACT	{idx_list->type |= KEYWORD;}
          	| KEYWORD_EXTERN_EXTRACT	{idx_list->type |= (KEYWORD | EXTERNKEY);}
          	| KEYWORD_PROX_EXTRACT	{idx_list->type |= (KEYWORD | PROXIMITY);}
          	| KEYWORD_EXTERN_PROX_EXTRACT	{idx_list->type |= (KEYWORD | EXTERNKEY | PROXIMITY);}
          	| EXACTKEY_EXTRACT	{idx_list->type |= EXACTKEY;}
          	| EXACTKEY_PROX_EXTRACT	{idx_list->type |= EXACTKEY | PROXIMITY;}
		| FLD008_EXTRACT	{idx_list->type |= FLD008_KEY;}
		| FLD008_DATE	{idx_list->type |= (FLD008_KEY | DATE_KEY);}
		| FLD008_DATERANGE	{idx_list->type |= (FLD008_KEY | DATE_RANGE_KEY);}
		| INTEGER_EXTRACT	{idx_list->type |= INTEGER_KEY;}
		| DECIMAL_EXTRACT	{idx_list->type |= DECIMAL_KEY;}
		| FLOAT_EXTRACT	        {idx_list->type |= FLOAT_KEY;}
		| URL_EXTRACT	        {idx_list->type |= URL_KEY;}
		| FILENAME_EXTRACT	{idx_list->type |= FILENAME_KEY;}
		| DATE_EXTRACT	        {idx_list->type |= DATE_KEY;}
		| DATE_RANGE_EXTRACT	        {idx_list->type |= DATE_RANGE_KEY;}
		| LAT_LONG_EXTRACT      {idx_list->type |= LAT_LONG_KEY; }
		| BOUNDING_BOX_EXTRACT  {idx_list->type |= BOUNDING_BOX_KEY;}
		| GEOTEXT_EXTRACT      {idx_list->type |= GEOTEXT; }
		| GEOTEXT_LAT_LONG_EXTRACT      {idx_list->type |= (GEOTEXT | LAT_LONG_KEY); }
		| GEOTEXT_BOUNDING_BOX_EXTRACT  {idx_list->type |= (GEOTEXT | BOUNDING_BOX_KEY);}
		| CODED_ELEMENT {idx_list->type |= BITMAPPED_TYPE | NORM_NOMAP;
                     idx_list->type2 |= cf_lval.intval;}
		| NGRAM_EXTRACT {idx_list->type2 |= NGRAMS | NORM_NOMAP;
                     idx_list->snowball_stem_type = cf_lval.intval;}
       	]

          NORMAL_ATTR
          	[STEM_NORM 	  	{idx_list->type |= STEMS | NORM_NOMAP;}
		| STEMFREQ_NORM	  	{idx_list->type |= STEMS | NORM_WITH_FREQ | NORM_NOMAP;}
		| SSTEM_NORM	  	{idx_list->type |= SSTEMS | NORM_NOMAP;}
		| SSTEMFREQ_NORM	  	{idx_list->type |= SSTEMS | NORM_WITH_FREQ;}
		| SNOWBALL_STEM_NORM	{idx_list->type |= SNOWBALL_STEMS | NORM_NOMAP;
                                 idx_list->snowball_stem_type = cf_lval.intval;
				 if (cf_info->encoding == 1) {
				   idx_list->snowball_stem_type = 
				     idx_list->snowball_stem_type + 100;
				 }
				 if (cf_info->encoding == 2) {
				   idx_list->snowball_stem_type = 13;
				 }
                                }
		| SNOWBALL_STEMFREQ_NORM {idx_list->type |= SNOWBALL_STEMS | NORM_WITH_FREQ | NORM_NOMAP;
                                 idx_list->snowball_stem_type = cf_lval.intval;
				 if (cf_info->encoding == 1) {
				   idx_list->snowball_stem_type = 
				     idx_list->snowball_stem_type + 100;
				 }
                                }
        | WORDNET_NORM 	{idx_list->type |= WORDNET;}
		| CLASSCLUS_NORM {idx_list->type |= CLASSCLUS;}
		| XKEY_NORM
        | XKEYFREQ_NORM {idx_list->type |= NORM_WITH_FREQ;}
        | NONE_NORM   {idx_list->type |= NORM_NOMAP;}
        | NONEFREQ_NORM {idx_list->type |= NORM_WITH_FREQ;}
        | DONT_NORMALIZE {idx_list->type |= NORM_DO_NOTHING | NORM_NOMAP;}
        | DONT_NORMALIZE_FREQ {idx_list->type |= (NORM_DO_NOTHING | NORM_WITH_FREQ | NORM_NOMAP);}
        | MIN_NORMALIZE {idx_list->type |= NORM_MIN | NORM_NOMAP;}
        | MIN_NORMALIZE_FREQ {idx_list->type |= (NORM_MIN | NORM_WITH_FREQ);}
		| STEM_NOMAP_NORM {idx_list->type |= STEMS | NORM_NOMAP;}
		| SSTEM_NOMAP_NORM {idx_list->type |= SSTEMS | NORM_NOMAP;}
        | WORDNET_NOMAP_NORM 	{idx_list->type |= WORDNET | NORM_NOMAP;}
		| CLASSCLUS_NOMAP_NORM {idx_list->type |= CLASSCLUS | NORM_NOMAP;}
		| XKEY_NOMAP_NORM {idx_list->type |= NORM_NOMAP;}
        | NONE_NOMAP_NORM {idx_list->type |= NORM_NOMAP;}
		| LAT_LONG_NORM       {idx_list->dateformat = cf_lval.string;}
		| BOUNDING_BOX_NORM   {idx_list->dateformat = cf_lval.string;}
		| FGDC_BOUNDING_BOX_NORM   {idx_list->dateformat = cf_lval.string;}
		| DATE_NORMALIZE      {idx_list->dateformat = cf_lval.string;}
		| DATE_RANGE_NORM      {idx_list->dateformat = cf_lval.string;}
        ] 
	  primarykey*	 ATTRIBUTES_END
          	
          [indxname | indxtag]+
	  indxmap*
          indxcont*
          stoplist?
          expansion?
	  extern_app?
	  indxexc?
	  rankparams*
          indxkey+ 

          INDEXDEF_END ;

primarykey: PRIMARYKEY_ATTR 
		[ PRIMARYKEY_NO
		| PRIMARYKEY_IGNORE 
          	     {idx_list->type |= PRIMARYIGNORE;
		      cf_info->primary_key = idx_list; } 
		| PRIMARYKEY_REPLACE 
          	     {idx_list->type |= PRIMARYREPLACE;
		      cf_info->primary_key = idx_list; } 
		]
	    | PRIMARYKEY_NO 
	    | PRIMARYKEY_IGNORE {
		idx_list->type |= PRIMARYIGNORE ;
		 cf_info->primary_key = idx_list;
		} ;

indxname: 	INDXNAME_START 
			[FILENAME_LITERAL | NAME_TOKEN] {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       idx_list->name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       idx_list->name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(idx_list->name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(idx_list->name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       idx_list->name = strdup(cf_lval.string);
			    }
			}
			INDXNAME_END;
			
indxtag: 	INDXTAG_START 
			NAME_TOKEN {idx_list->tag = cf_lval.string;}
			INDXTAG_END ;

indxmap: 	INDXMAP_START {
			
		/* allocate space for a new attribute and fill it in */
		prev_attr_map = attr_maps;	
		if ((attr_maps = CALLOC(attr_map,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for attr_map\n");
		  exit (1);
		}
		if (prev_attr_map == NULL && last_attr_map == NULL) {
		  /* must be the first index entry in the list */
		  idx_list->bib1_attr_map = attr_maps;
		  if (cf_info->bib1_attr_map == NULL) 
			cf_info->bib1_attr_map = attr_maps;
		}
		else if (prev_attr_map == NULL && last_attr_map != NULL) {
		  /* must be the first index entry in the list */
		  idx_list->bib1_attr_map = attr_maps;
		  if (cf_info->bib1_attr_map == NULL) /* should never happen */
			cf_info->bib1_attr_map = attr_maps;
		  last_attr_map->next_attr_map_file = attr_maps;
		}
		else {
			prev_attr_map->next_attr_map = attr_maps;
			prev_attr_map->next_attr_map_file = attr_maps;
			attr_maps->next_attr_map = NULL;
			attr_maps->next_attr_map_file = NULL;
		}
		attr_maps->idx_entry = idx_list;
		last_attr_map = attr_maps;
		
		/* set the default attributeset OID */
		attr_maps->attributeSetOID = OID_BIB1;
		default_attr_set = OID_BIB1;
	
		}
			
		attributeset?  ATTRIBUTES_END {
		   if (strcmp(attr_maps->attributeSetOID,default_attr_set) != 0){
		       default_attr_set = attr_maps->attributeSetOID;
		   }
		}
		[[USE_START 
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[USE_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER { /* note that this has hard coded values for */
				 /* range checking, as do all the other parts*/
			  int i;

			  if (cf_lval.intval >= 0 && cf_lval.intval < 8000) {
			    attr_maps->attr_nums[USE_ATTR] = cf_lval.intval;
			    USE_BIT_SET(cf_info, cf_lval.intval);
			    cf_lval.intval = 0;
                          }
			  else {
			     attr_maps->attr_nums[USE_ATTR] = cf_lval.intval;
			     cf_lval.intval = 0;
			  }
			}
		USE_END]

		| [RELATION_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[RELATION_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER {
			  if (cf_lval.intval >= 0 && cf_lval.intval < 1000)
			    attr_maps->attr_nums[RELATION_ATTR] = cf_lval.intval;
			}
		RELATION_END]

		| [POSITION_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[POSITION_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER {
			  if (cf_lval.intval >= 0 && cf_lval.intval < 100)
			    attr_maps->attr_nums[POSITION_ATTR] = cf_lval.intval;
			}
		POSITION_END]

		| [STRUCT_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[STRUCTURE_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER {
			  if (cf_lval.intval >= 0 && cf_lval.intval < 1000)
			    attr_maps->attr_nums[STRUCTURE_ATTR] = cf_lval.intval;
			}
		STRUCT_END]

		| [TRUNC_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[TRUNC_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER {
			  if (cf_lval.intval >= 0 && cf_lval.intval < 1000)
			    attr_maps->attr_nums[TRUNC_ATTR] = cf_lval.intval;
			}
		TRUNC_END]

		| [COMPLET_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[COMPLETENESS_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			NUMBER {
			  if (cf_lval.intval >= 0 && cf_lval.intval < 1000)
			    attr_maps->attr_nums[COMPLETENESS_ATTR] 
			         = cf_lval.intval;
			}
		COMPLET_END]

		| [DESCRIPT_START
			DESCRIPTION {
			       attr_maps->altdescription = cf_lval.string;
			}
		DESCRIPT_END]

		| [ACCESS_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[ACCESS_POINT_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[ACCESS_POINT_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[ACCESS_POINT_ATTR] 
				    = cf_lval.string;
			}
		ACCESS_END]
		| [SEMANTIC_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[SEMANTIC_QUAL_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[SEMANTIC_QUAL_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[SEMANTIC_QUAL_ATTR] 
				    = cf_lval.string;
			  
			}
		SEMANTIC_END]
		| [LANGUAGE_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[LANGUAGE_ATTR_TYPE]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[LANGUAGE_ATTR_TYPE] = cf_lval.intval;
			  attr_maps->attr_string[LANGUAGE_ATTR_TYPE] 
				    = cf_lval.string;
			}
		LANGUAGE_END]
		| [CONT_AUTH_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[CONTENT_AUTH_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER  | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[CONTENT_AUTH_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[CONTENT_AUTH_ATTR] 
				    = cf_lval.string; 
			}
		CONT_AUTH_END]
		| [EXPANSION_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[EXPANSION_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[EXPANSION_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[EXPANSION_ATTR] 
				    = cf_lval.string;
			  
			}
		EXPANSION_END]
		| [NORM_WEIGHT_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[NORMALIZED_WEIGHT_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[NORMALIZED_WEIGHT_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[NORMALIZED_WEIGHT_ATTR] 
				    = cf_lval.string;
			}
		NORM_WEIGHT_END]
		| [HIT_COUNT_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[HIT_COUNT_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[HIT_COUNT_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[HIT_COUNT_ATTR] 
				    = cf_lval.string;
			}
		HIT_COUNT_END]
		| [COMPARISON_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[COMPARISON_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING]  {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[COMPARISON_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[COMPARISON_ATTR] 
				    = cf_lval.string;
			}
		COMPARISON_END]
		| [FORMAT_ATTR_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[FORMAT_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[FORMAT_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[FORMAT_ATTR] 
				    = cf_lval.string;
			}
		FORMAT_ATTR_END]
		| [OCCURRENCE_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[OCCURRENCE_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER  | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[OCCURRENCE_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[OCCURRENCE_ATTR] 
				    = cf_lval.string;
			}
		OCCURRENCE_END]
		| [INDIRECTION_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[INDIRECTION_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[INDIRECTION_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[INDIRECTION_ATTR] 
				    = cf_lval.string;
			}
		INDIRECTION_END]
		| [FUNC_QUAL_START
		attributeset?  ATTRIBUTES_END {
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
		           attr_maps->attr_set_oid[FUNC_QUAL_ATTR]
                                = attr_maps->attributeSetOID;
			}
			}
			[NUMBER | QUOTED_STRING] {
			if (cf_lval.intval >= 0)
				attr_maps->attr_nums[FUNC_QUAL_ATTR] = cf_lval.intval;
			  attr_maps->attr_string[FUNC_QUAL_ATTR] 
				    = cf_lval.string;
			}
		FUNC_QUAL_END]
		]*

		INDXMAP_END { 
			if (strcmp(attr_maps->attributeSetOID, 
			   default_attr_set) != 0){
			  attr_maps->attributeSetOID = default_attr_set;
			}
		};

attributeset: [ATTRIB_ATTR
		[BIB_1 {attr_maps->attributeSetOID = OID_BIB1;}
		| EXP_1 {attr_maps->attributeSetOID = OID_EXP1;}
		| EXT_1 {attr_maps->attributeSetOID = OID_EXT1;}
		| CCL_1 {attr_maps->attributeSetOID = OID_CCL1;}
		| GILS {attr_maps->attributeSetOID = OID_GILS;}
		| GEO {attr_maps->attributeSetOID = OID_GEO;}
		| STAS {attr_maps->attributeSetOID = OID_STAS;}
		| COLLECTIONS_1 {attr_maps->attributeSetOID = OID_COLLECTIONS1;}
		| CIMI_1 {attr_maps->attributeSetOID = OID_CIMI1;}
		| ZBIG {attr_maps->attributeSetOID = OID_ZBIG;}
		| UTIL {attr_maps->attributeSetOID = OID_UTIL;}
		| XD_1 {attr_maps->attributeSetOID = OID_XD1;}
		| ZTHES {attr_maps->attributeSetOID = OID_ZTHES;}
		| FIN_1 {attr_maps->attributeSetOID = OID_FIN1;}
		| DAN_1 {attr_maps->attributeSetOID = OID_DAN1;}
		| HOLDINGS {attr_maps->attributeSetOID = OID_HOLD;}
		| ATTRSETOID {attr_maps->attributeSetOID = cf_lval.string;}
		] ] ;
	      

indxcont: 	INDXCONT_START {
			  	if ((new_cont = CALLOC(continuation,1))
       					== NULL) {
    				fprintf(LOGFILE, "No malloc space for file continuation\n");
    				exit (1);
  				}
  				/* initialize the stucture area to zeros or NULLs */
	  			/* bzero(new_cont, sizeof(struct continuation));*/
  				/* add it to the list of continuation files */
  				if (idx_list->indxcont == NULL) {
  					idx_list->indxcont = new_cont;
  					last_cont = new_cont;
  				}
	  			else {
  				    last_cont->next_cont = new_cont;
  				    last_cont = new_cont;
  				}
			} 
          	INDXCONT_NUMBER 
          	NUMBER { last_cont->id_number = cf_lval.intval; }
          	ATTRIBUTES_END
		[FILENAME_LITERAL | NAME_TOKEN] {last_cont->name = cf_lval.string;}
          	INDXCONT_END;

stoplist: 	STOPLIST_START 
		[FILENAME_LITERAL | NAME_TOKEN] {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       idx_list->stopwords_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       idx_list->stopwords_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(idx_list->stopwords_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(idx_list->stopwords_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       idx_list->stopwords_name = strdup(cf_lval.string);
			    }

		}
		STOPLIST_END;


expansion: 	EXPANSIONTERMS_START 
		[FILENAME_LITERAL | NAME_TOKEN] {
			    if (cf_lval.string[0] == '/'
			       || (cf_lval.string[1] == ':'
			       && cf_lval.string[1] == '\\'))
			       idx_list->expansion_name = strdup(cf_lval.string);
			    else if (cf_info->defaultpath) {
			       idx_list->expansion_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(idx_list->expansion_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(idx_list->expansion_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    } else {
			       idx_list->expansion_name = strdup(cf_lval.string);
			    }

		}
		EXPANSIONTERMS_END;


indxkey: 	INDXKEY_START
		tagspec*
		INDXKEY_END  {
			/* end of the index key information for this index */
			idx_list->keys = head_idx_keys;
			idx_keys = NULL;
			prev_idx_key = NULL;
			head_idx_keys = NULL;
		} ;

rankparams: RANK_PARAMS_START {
	       rank_parm = NULL; 
           current_rank_type = 0;}
       [RANK_TYPE_ATTR {/* printf("Got RANK_TYPE_ATTR\n");*/ }
	[LR_RANK { 
                    /* printf("Got LR_RANK\n");  */
	    current_rank_type = cf_lval.intval;
	 }
	| LR_RANK_TREC2 {
                /* printf("Got LM_RANK\n"); */
            current_rank_type = cf_lval.intval;
	 }
	| LM_RANK {
                /* printf("Got LM_RANK\n"); */
            current_rank_type = cf_lval.intval;
	 }
	| OK_RANK {
                /* printf("Got OK_RANK\n"); */
            current_rank_type = cf_lval.intval;
	 }
	| TFIDF_RANK {
                /* printf("Got OK_RANK\n"); */
            current_rank_type = cf_lval.intval;
	 }
    ]
       ]? ATTRIBUTES_END
       [[PARAM_START { /*printf("Got PARAM_START\n");*/ }
	PARAM_ID_ATTR {/* printf("Got PARAM_ID_ATTR\n");*/ }
	    NUMBER {
	      if ((rank_parm = CALLOC(ranking_parameters,1)) == NULL) {
		fprintf(LOGFILE, "No malloc space for ranking_parameters\n");
		exit (1);
	      }
	      if (head_rank_parms == NULL) {
		head_rank_parms = rank_parm;
	      } else {
		rank_parm->next_parm = head_rank_parms;
		head_rank_parms = rank_parm;
	      }
	      rank_parm->type = current_rank_type;
	      rank_parm->id = cf_lval.intval;
            }
         ] ATTRIBUTES_END
         PARAM_VALUE {
	    rank_parm->val = atof(cf_lval.string);
            }
         PARAM_END
        ]*
    RANK_PARAMS_END  {
	/* end of the index key information for this index */
			idx_list->ranking_parameters = head_rank_parms;
			rank_parm = NULL;
	} ;

extern_app:    	EXTERN_APP_START
                URL_TOKEN {
                     idx_list->extern_app_name = strdup(cf_lval.string);
                }
                EXTERN_APP_END ;


indxexc: 	INDXEXC_START
		tagspec*
		INDXEXC_END  {
			/* end of the index key information for this index */
			idx_list->exclude = head_idx_keys;
			idx_keys = NULL;
			prev_idx_key = NULL;
			head_idx_keys = NULL;
		} ;

clusters:  [CLUSTERS_START
	    clusdef*
	    CLUSTERS_END
	    ]
	    | oldclusdef ;

clusdef: 	CLUSTERDEF_START
			[clusterdef
			| clusterbase 
			]
		CLUSTERDEF_END { 
			clus_map = NULL;
			prev_clus_map = NULL;
			};

oldclusdef: 	CLUSTER_START
			[clusterdef
			| clusterbase 
			]
		CLUSTER_END { 
			clus_map = NULL;
			prev_clus_map = NULL;
			};

clusterbase: 	CLUSBASE_START 
		NAME_TOKEN {cf_info->clustered_file = cf_lval.string;}
		CLUSBASE_END;


clusterdef: clustername
	    clusterkey
	    clusstoplist?
	    clustermap+ ;

clustername: CLUSNAME_START {
			/* start new cluster_list_entry stucture */
			prev_clus_list = clus_list;	
			if ((clus_list = CALLOC(cluster_list_entry,1)) 
				    == NULL) {
			  fprintf(LOGFILE, "No malloc space for clus_list_entry\n");
			  exit (1);
			}
			if (prev_clus_list == NULL) {
			  /* must be the first index entry in the list */
			  cf_info->clusters = clus_list;
			}
			else
			  prev_clus_list->next_entry = clus_list;
			}

	  NAME_TOKEN {clus_list->name = cf_lval.string;}
          CLUSNAME_END ;

		
clusstoplist: 	STOPLIST_START 
		[FILENAME_LITERAL | NAME_TOKEN] { 
			if (cf_lval.string[0] == '/'
			   || (cf_lval.string[1] == ':'
			   && cf_lval.string[1] == '\\'))
			       clus_list->cluster_stoplist_name = strdup(cf_lval.string);
			 else if (cf_info->defaultpath) {
			   clus_list->cluster_stoplist_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			   sprintf(clus_list->cluster_stoplist_name, "%s/%s",
			           cf_info->defaultpath, cf_lval.string);
#else
			   sprintf(clus_list->cluster_stoplist_name, "%s\\%s",
			           cf_info->defaultpath, cf_lval.string);
#endif
			 } else {
			    clus_list->cluster_stoplist_name = strdup(cf_lval.string);
			 }
		}
		STOPLIST_END;
	
clusterkey:  CLUSKEY_START

          NORMAL_ATTR 
          	[STEM_NORM 	{clus_list->normalization |= STEMS;}
          	| SSTEM_NORM 	{clus_list->normalization |= SSTEMS;}
          	| WORDNET_NORM 	{clus_list->normalization |= WORDNET;}
		| CLASSCLUS_NORM {clus_list->normalization |= CLASSCLUS;}
		| XKEY_NORM {clus_list->normalization |= EXACTKEY;}
          	| MIN_NORMALIZE {clus_list->normalization |= NORM_MIN;}
         	| DONT_NORMALIZE {clus_list->normalization |= NORM_DO_NOTHING;}
          	| NONE_NORM
		| STEM_NOMAP_NORM {clus_list->normalization |= STEMS | NORM_NOMAP;}
		| SSTEM_NOMAP_NORM {clus_list->normalization |= SSTEMS | NORM_NOMAP;}
          	| WORDNET_NOMAP_NORM 	{clus_list->normalization |= WORDNET | NORM_NOMAP;}
		| CLASSCLUS_NOMAP_NORM {clus_list->normalization |= CLASSCLUS | NORM_NOMAP;}
		| XKEY_NOMAP_NORM {clus_list->normalization |= EXACTKEY | NORM_NOMAP;}
          	| NONE_NOMAP_NORM {clus_list->normalization |= NORM_NOMAP;}


        ] ATTRIBUTES_END
          tagspec
	  CLUSKEY_END {
			/* end of the cluster key information */
			clus_list->cluster_key = head_idx_keys;
			idx_keys = NULL;
			prev_idx_key = NULL;
			head_idx_keys = NULL;
		} ;


clustermap: CLUSMAP_START
	    [from to summary?]+
	    CLUSMAP_END;

from: FROM_START { /* start new idx_list_entry stucture */

		prev_clus_map = clus_map;	
		if ((clus_map = CALLOC(cluster_map_entry,1)) 
			    == NULL) {
		  fprintf(LOGFILE, "No malloc space for clus_map_entry\n");
		  exit (1);
		}
		if (prev_clus_map == NULL) {
		  /* must be the first entry in the list */
		  clus_list->field_map = clus_map;
		}
		else
		  prev_clus_map->next_clusmap = clus_map;
		}

      tagspec

      FROM_END {
		/* end of the from key information for this index */
		clus_map->from = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		} ;


to: TO_START
      tagspec
      TO_END {
		/* end of the from key information for this index */
		clus_map->to = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		} ;

summary: SUMMARIZE_START 
	maxnum 
	tagspec
	SUMMARIZE_END {
		/* end of the summarize key information for this index */
		clus_map->summarize = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		} ;

dispoptions: DISPOPTIONS_START {cf_info->display_options = 0;}
	  [KEEPAMP {cf_info->display_options |= KEEP_AMP;}
	  | KEEPLT {cf_info->display_options |= KEEP_LT;}
	  | KEEPGT {cf_info->display_options |= KEEP_GT;}
          | KEEPALL {cf_info->display_options |= KEEP_ALL;}
          | KEEPENT {cf_info->display_options |= KEEP_ENT;}
	  ]*
	  DISPOPTIONS_END ;

displays: DISPLAY_START {prev_format = NULL; format = NULL;}
	  [format_spec]+
	  DISPLAY_END ;

format_spec: FORMAT_START {
		/* start new display_format stucture */
		prev_format = format;	
		if ((format = CALLOC(display_format,1)) 
			    == NULL) {
		  fprintf(LOGFILE, "No malloc space for display_format\n");
		  exit (1);
		}
		if (prev_format == NULL) {
		  /* must be the first index entry in the list */
		  cf_info->display = format;
		}
		else
		  prev_format->next_format = format;
	}
			

          FORMAT_NAME_ATTR 
	      NAME_TOKEN { format->name = cf_lval.string; }
          [OID_NAME_ATTR
		[MARC_OID {format->oid = MARCRECSYNTAX;}
		| EXP_OID {format->oid = EXPLAINRECSYNTAX;}
		| SUTRS_OID {format->oid = SUTRECSYNTAX;}
		| SGML_OID {format->oid = SGML_RECSYNTAX;}
		| XML_OID {format->oid = XML_RECSYNTAX;}
		| HTML_OID {format->oid = HTML_RECSYNTAX;}
		| OPAC_OID {format->oid = OPACRECSYNTAX;}
		| SUMM_OID {format->oid = SUMMARYRECSYNTAX;}
		| GRS0_OID {format->oid = GRS0RECSYNTAX;}
		| GRS1_OID {format->oid = GRS1RECSYNTAX;}
		| ES_OID {format->oid = ESRECSYNTAX;}
		] 
	  ]?
	  [DEFAULT_ATTR  { format->default_format = cf_lval.intval;
		}
	  ]?
	  [MARC_DTD_FILE_ATTR
		[FILENAME_LITERAL | NAME_TOKEN] {
		    if (cf_lval.string[0] == '/'
		       || (cf_lval.string[1] == ':'
		       && cf_lval.string[1] == '\\'))
		       format->marc_dtd_file_name = strdup(cf_lval.string);
		    else if (cf_info->defaultpath) {
		       format->marc_dtd_file_name = 
		            CALLOC(char, strlen(cf_info->defaultpath)
					     +strlen(cf_lval.string)+3);
#ifndef WIN32
		       sprintf(format->marc_dtd_file_name, "%s/%s",
		              cf_info->defaultpath, cf_lval.string);
#else
		       sprintf(format->marc_dtd_file_name, "%s\\%s",
		              cf_info->defaultpath, cf_lval.string);
#endif
		    } else {
		       format->marc_dtd_file_name = strdup(cf_lval.string);
		    }
		}
 	  ]?
	  
	  ATTRIBUTES_END
		
          [include | exclude | convert]*
	
	  FORMAT_END ;	

include: INCLUDE_START
	 tagspec
	 INCLUDE_END {
		/* end of the from key information for this index */
		extern int lineno; /* in cf_parse.flex */
		format->include = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		fprintf(stderr, 
			"CONFIGFILE PARSER WARNING line #%d : \n", lineno);
		fprintf(stderr, 
                       "\tINCLUDE display specification for display format '%s' not \n\tcurrently supported (use EXCLUDE)\n", format->name);
		} ;

exclude: EXCLUDE_START
	 [COMPRESS_YES { format->exclude_compress = 1;}
	  | COMPRESS_NO {format->exclude_compress = 0;}
	 ]?
	 ATTRIBUTES_END
	 tagspec
	 EXCLUDE_END {
		/* end of the from key information for this index */
		format->exclude = head_idx_keys;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		} ;

convert: CONVERT_START 
          FUNCTION_ATTR 
	      [NAME_TOKEN { 
			if (cf_lval.string[0] == '/'
			  || (cf_lval.string[2] == ':'
			      && cf_lval.string[1] == '\\')
			  || (strcasecmp("RECMAP", cf_lval.string) == 0
			     || strcasecmp("MIXED", cf_lval.string) == 0
			     || strcasecmp("TAGSET-G", cf_lval.string) == 0
			     || strcasecmp("PAGE_PATH", cf_lval.string) == 0
			     || strcasecmp("RELATIONAL", cf_lval.string) == 0
			     || strcasecmp("COMMADELIMITED", cf_lval.string) == 0
			     || strcasecmp("MARC", cf_lval.string) == 0
			     || strncasecmp("XML_ELEMENT", cf_lval.string, 11) == 0
			     || strcasecmp("TAGSET-M", cf_lval.string) == 0))
			       format->convert_name = cf_lval.string;
			    else if (cf_info->defaultpath) {
			       format->convert_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(format->convert_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(format->convert_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			    }
		 }
	      | FILENAME_LITERAL {
			if (cf_lval.string[0] == '/'
			  || (cf_lval.string[1] == ':'
			      && cf_lval.string[1] == '\\')
			  || (strcasecmp("RECMAP", cf_lval.string) == 0
			     || strcasecmp("MIXED", cf_lval.string) == 0
			     || strcasecmp("TAGSET-G", cf_lval.string) == 0
			     || strcasecmp("PAGE_PATH", cf_lval.string) == 0
			     || strcasecmp("TAGSET-M", cf_lval.string) == 0))
			       format->convert_name = cf_lval.string;
			    else if (cf_info->defaultpath) {
			       format->convert_name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			       sprintf(format->convert_name, "%s/%s",
			              cf_info->defaultpath, cf_lval.string);
#else
			       sprintf(format->convert_name, "%s\\%s",
			              cf_info->defaultpath, cf_lval.string);
#endif
			   }
		}
	      ]
	  ALL_ATTR?  { format->convert = NULL;}

	  ATTRIBUTES_END { 
		clus_map = &temp_clus_map;
		prev_clus_map = &temp_clus_map;
		}

	 clustermap?

	 CONVERT_END {
		/* end of the conversion key information for this index */
		format->convert = temp_clus_map.next_clusmap;
                temp_clus_map.next_clusmap = NULL;
		idx_keys = NULL;
		prev_idx_key = NULL;
		head_idx_keys = NULL;
		clus_map = NULL;	
		prev_clus_map = NULL;
		} ;


components: COMPONENTS_START {
        /* initialize for this configfile */
		prev_comp_list = comp_list = NULL;
		n_indexes = cf_info->num_indexes;
        }
	    componentdef*
	    COMPONENTS_END ;

componentdef: 	COMPONENTDEF_START
		componentname 
		[componentnorm | componentstore]*
		compstart
		compend?
		compindexes
		COMPONENTDEF_END { 

			};

compindexes:  COMPINDEXES_START
	      compindexdef+ 
	      COMPINDEXES_END { 
				cf_info->num_indexes = n_indexes;
				idx_list = NULL;
				prev_idx_list = NULL;
	      } ;


compindexdef: INDEXDEF_START {
				/* start new idx_list_entry stucture */
				n_indexes++;
				prev_attr_map = NULL;
				attr_maps = NULL;
                head_rank_parms = NULL; 

				prev_idx_list = idx_list;	
				if ((idx_list = CALLOC(idx_list_entry,1)) 
					    == NULL) {
				  fprintf(LOGFILE, "No malloc space for idx_list_entry\n");
				  exit (1);
				}
				if (prev_idx_list == NULL) {
				  /* must be the first index entry in the list */
				  comp_list->indexes = idx_list;
				  
				}
				else
				  prev_idx_list->next_entry = idx_list;


			      idx_list->type |= COMPONENT_INDEX;
			      idx_list->comp_parent = comp_list;
			}
			

          ACCESS_ATTR 
          	[BTREE	{idx_list->type |= DB_BTREE_TYPE;}
          	| HASH	{idx_list->type |= DB_HASH_TYPE;}
          	| VECTOR {idx_list->type |= VECTOR_TYPE ;}
		| DBMS_TYPE {idx_list->type |= DB_DBMS_TYPE ;}
          	]

          EXTRACT_ATTR 
          	[KEYWORD_EXTRACT	{idx_list->type |= KEYWORD;}
          	| KEYWORD_EXTERN_EXTRACT	{idx_list->type |= (KEYWORD | EXTERNKEY);}
          	| KEYWORD_PROX_EXTRACT	{idx_list->type |= (KEYWORD | PROXIMITY);}
          	| KEYWORD_EXTERN_PROX_EXTRACT	{idx_list->type |= (KEYWORD | EXTERNKEY | PROXIMITY);}
          	| EXACTKEY_EXTRACT	{idx_list->type |= EXACTKEY;}
		| FLD008_EXTRACT	{idx_list->type |= FLD008_KEY;}
		| INTEGER_EXTRACT	{idx_list->type |= INTEGER_KEY;}
		| DECIMAL_EXTRACT	{idx_list->type |= DECIMAL_KEY;}
		| FLOAT_EXTRACT	        {idx_list->type |= FLOAT_KEY;}
		| URL_EXTRACT	        {idx_list->type |= URL_KEY;}
		| FILENAME_EXTRACT      {idx_list->type |= FILENAME_KEY;}
		| DATE_EXTRACT	        {idx_list->type |= DATE_KEY; }
		| DATE_RANGE_EXTRACT    {idx_list->type |= DATE_RANGE_KEY; }
		| LAT_LONG_EXTRACT      {idx_list->type |= LAT_LONG_KEY; }
		| BOUNDING_BOX_EXTRACT  {idx_list->type |= BOUNDING_BOX_KEY;}
          	]

          NORMAL_ATTR 
          	[STEM_NORM 	  	{idx_list->type |= STEMS;}
		| STEMFREQ_NORM	  	{idx_list->type |= STEMS | NORM_WITH_FREQ;}
                | SSTEM_NORM {idx_list->type |= SSTEMS;}
		| SSTEMFREQ_NORM	{idx_list->type |= SSTEMS | NORM_WITH_FREQ;}
          	| WORDNET_NORM 	{idx_list->type |= WORDNET;}
		| CLASSCLUS_NORM {idx_list->type |= CLASSCLUS;}
		| XKEY_NORM
          	| XKEYFREQ_NORM {idx_list->type |= NORM_WITH_FREQ;}
          	| NONE_NORM
          	| NONEFREQ_NORM {idx_list->type |= NORM_WITH_FREQ;}
          	| DONT_NORMALIZE {idx_list->type |= NORM_DO_NOTHING;}
          	| DONT_NORMALIZE_FREQ {idx_list->type |= (NORM_DO_NOTHING | NORM_WITH_FREQ);}
          	| MIN_NORMALIZE {idx_list->type |= NORM_MIN;}
          	| MIN_NORMALIZE_FREQ {idx_list->type |= (NORM_MIN | NORM_WITH_FREQ);}
		| STEM_NOMAP_NORM {idx_list->type |= STEMS | NORM_NOMAP;}
		| SSTEM_NOMAP_NORM {idx_list->type |= SSTEMS | NORM_NOMAP;}
          	| WORDNET_NOMAP_NORM 	{idx_list->type |= WORDNET | NORM_NOMAP;}
		| CLASSCLUS_NOMAP_NORM {idx_list->type |= CLASSCLUS | NORM_NOMAP;}
		| XKEY_NOMAP_NORM {idx_list->type |= NORM_NOMAP;}
          	| NONE_NOMAP_NORM {idx_list->type |= NORM_NOMAP;}
		| LAT_LONG_NORM       {idx_list->dateformat = cf_lval.string;}
		| BOUNDING_BOX_NORM   {idx_list->dateformat = cf_lval.string;}
		| FGDC_BOUNDING_BOX_NORM   {idx_list->dateformat = cf_lval.string;}
		| DATE_NORMALIZE      {idx_list->dateformat = cf_lval.string;}
		| DATE_RANGE_NORM      {idx_list->dateformat = cf_lval.string;}
          	] 
	  primarykey*	 ATTRIBUTES_END
          	
      [indxname | indxtag]+
	  indxmap*
      indxcont*
      stoplist?
	  extern_app?
	  indxexc?
	  rankparams*
      indxkey+ 

    INDEXDEF_END ;



compstart: COMPTAGSTART_START {prev_idx_key = NULL; 
	         head_idx_keys = NULL;
	   }
	   [tagspec]
	   COMPTAGSTART_END {
	         comp_list->start_tag = head_idx_keys;
		 prev_idx_key = NULL;
		 head_idx_keys = NULL;
		 idx_keys = NULL;
	   };


compend: COMPTAGEND_START {
	        /* definition of an end tag is optional -- if it */
		/* isn't defined the starttag is used for begin and */
		/* its matching end tags as the end --  */
		/* If specified, things will be treated as the component */
		/* up to the endtag */
		prev_idx_key = NULL; 
	        head_idx_keys = NULL;
	   }
	 [tagspec]
	 COMPTAGEND_END {
	         comp_list->end_tag = head_idx_keys;
		 prev_idx_key = NULL;
		 head_idx_keys = NULL;
		 idx_keys = NULL;
	   };

componentname: COMPONENTNAME_START {
			/* start new component_list_entry stucture */
			prev_comp_list = comp_list;	
			if ((comp_list = CALLOC(component_list_entry,1)) 
				    == NULL) {
			  fprintf(LOGFILE, "No malloc space for component_list_entry\n");
			  exit (1);
			}
			if (prev_comp_list == NULL) {
			  /* must be the first index entry in the list */
			  cf_info->components = comp_list;
			}
			else
			  prev_comp_list->next_entry = comp_list;

			/* include a pointer to the config file */
			comp_list->config_info = cf_info;

			}

	  [NAME_TOKEN | FILENAME_LITERAL] {
			if (cf_lval.string[0] == '/'
			   || (cf_lval.string[1] == ':'
			   && cf_lval.string[1] == '\\'))
			       comp_list->name = strdup(cf_lval.string);
			 else if (cf_info->defaultpath) {
			   comp_list->name = CALLOC(char, strlen(cf_info->defaultpath)+strlen(cf_lval.string)+3);
#ifndef WIN32
			   sprintf(comp_list->name, "%s/%s",
			           cf_info->defaultpath, cf_lval.string);
#else
			   sprintf(comp_list->name, "%s\\%s",
			           cf_info->defaultpath, cf_lval.string);
#endif
			 } else {
			    comp_list->name = strdup(cf_lval.string);
			 }
	  }
          COMPONENTNAME_END ;

componentstore: COMPONENTSTORE_START
	       	[ STORE_YES	{comp_list->normalization |= COMPONENT_STORE;}
          	| STORE_NO	{ }
          	]
	       COMPONENTSTORE_END { };

componentnorm: COMPONENTNORM_START
	       	[NONE_NORM	{ }
          	| COMPRESS_YES	{comp_list->normalization |= COMPONENT_COMPRESS;}
          	| COMPRESS_NO	{ }
          	]
	       COMPONENTNORM_END { };

tagspec: TAGSPEC_START 
	[[xpath] | [sql] |
	[ftag s* attr?]]+
	TAGSPEC_END;

maxnum: MAXNUM_START 
	NUMBER {clus_map->sum_maxnum = cf_lval.intval ;}
	MAXNUM_END;

ftag: 	FTAG_START 
      	[NAME_TOKEN | NUMBER | TAG_PATTERN | URL_TOKEN] {
		/* allocate space for a new key entry and fill it in */
		prev_idx_key = idx_keys;	
		if ((idx_keys = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_key\n");
		  exit (1);
		}
		if (prev_idx_key == NULL) {
		  /* must be the first index entry in the list */
		  head_idx_keys = idx_keys;
		  idx_keys->subkey = NULL; /* no subkeys yet */
		}
		else
		prev_idx_key->next_key = idx_keys;
		idx_keys->next_key = NULL;
		idx_keys->key =  cf_lval.string;
	} 

      	FTAG_END { /* initialize for possible subkeys */
		subkey_nesting = 0;
      		prev_subkey = NULL; 
      		idx_subkey = NULL;
		subkey_parent = idx_keys; } ;


s:	S_START { subkey_nesting++; }
	[NAME_TOKEN | NUMBER | TAG_PATTERN | URL_TOKEN] {
		/* allocate space for a new key entry and fill it in */
		/* this is recursive and adds keys for               */
		/* any number of nesting levels...                   */
		prev_subkey = idx_subkey;	
		if ((idx_subkey = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}
		if (prev_subkey == NULL) {
		  /* must be the first index entry in the list */
		  idx_keys->subkey = idx_subkey;
		}
		else
		    prev_subkey->subkey = idx_subkey;

		     idx_subkey->next_key = NULL;
		     idx_subkey->key =  cf_lval.string;
	} 
	s?
	value? 
	S_END  {subkey_nesting--; } ;

attr:	ATTR_START { subkey_nesting++; }
	[NAME_TOKEN | NUMBER | TAG_PATTERN | URL_TOKEN] {
		/* allocate space for a new key entry and fill it in */
		/* this is recursive and adds keys for               */
		/* any number of nesting levels...                   */
		prev_subkey = idx_subkey;	
		if ((idx_subkey = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}
		if (prev_subkey == NULL) {
		  /* must be the first index entry in the list */
		  idx_keys->subkey = idx_subkey;
		}
		else
		    prev_subkey->subkey = idx_subkey;

		idx_subkey->next_key = NULL;
		idx_subkey->key =  cf_lval.string;
		idx_subkey->attribute_flag = 1;

		if (idx_list != NULL && idx_list->type & EXTERNKEY) {
		   if (strcasecmp("TEXT_FILE_REF" , idx_subkey->key) == 0
		   || strcasecmp("TEXT_DIRECTORY_REF" , idx_subkey->key) == 0
		   || strcasecmp("PAGED_DIRECTORY_REF" , idx_subkey->key) == 0
		   || strcasecmp("EXTERNAL_URL_REF" , idx_subkey->key) == 0)
		      idx_subkey->attribute_flag = 6; 
		}

		if (idx_list != NULL && idx_list->type & FLD008_KEY) {
		   if (strcasecmp("008_entry_date" , idx_subkey->key) == 0 
		   || strcasecmp("008_date_type" , idx_subkey->key) == 0 
		   || strcasecmp("008_date1" , idx_subkey->key) == 0 
		   || strcasecmp("008_date2" , idx_subkey->key) == 0 
		   || strcasecmp("008_daterange" , idx_subkey->key) == 0 
		   || strcasecmp("008_country_code" , idx_subkey->key) == 0 
		   || strcasecmp("008_illus_code" , idx_subkey->key) == 0 
		   || strcasecmp("008_intellectual_level" , idx_subkey->key) == 0 
		   || strcasecmp("008_form_of_reproduction" , idx_subkey->key) == 0 
		   || strcasecmp("008_nature_of_contents" , idx_subkey->key) == 0 
		   || strcasecmp("008_government_pub_code" , idx_subkey->key) == 0 
		   || strcasecmp("008_conference_indicator" , idx_subkey->key) == 0 
		   || strcasecmp("008_festschrift_indicator" , idx_subkey->key) == 0 
		   || strcasecmp("008_index_indicator" , idx_subkey->key) == 0 
		   || strcasecmp("008_main_entry_in_body" , idx_subkey->key) == 0 
		   || strcasecmp("008_fiction_indicator" , idx_subkey->key) == 0 
		   || strcasecmp("008_biography_indicator" , idx_subkey->key) == 0 
		   || strcasecmp("008_language_code" , idx_subkey->key) == 0 
		   || strcasecmp("008_cataloging_source" , idx_subkey->key) == 0 )
		      idx_subkey->attribute_flag = 7; 
                 }

	} 
	value*
	replaceval*
	ATTR_END { subkey_nesting--; } ;

value:  ATTRVAL_START { subkey_nesting++;}
	[NAME_TOKEN | NUMBER | TAG_PATTERN | URL_TOKEN | QUOTED_STRING] {
		/* allocate space for a new key entry and fill it in */
		/* this is recursive and adds keys for               */
		/* any number of nesting levels...                   */
		prev_subkey = idx_subkey;	
		if ((idx_subkey = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}

		if (prev_subkey == NULL) {
		  /* must be the first index entry in the list */
		  idx_keys->subkey = idx_subkey;
		}
		else
		    prev_subkey->subkey = idx_subkey;

		if (prev_subkey->attribute_flag == 0)
		   prev_subkey->attribute_flag = 5; /* sub key used only in value testing */
		idx_subkey->next_key = NULL;
		idx_subkey->key =  cf_lval.string;
		idx_subkey->attribute_flag = 2; /* indicates value */
	} 
	[[NAME_TOKEN | NUMBER | TAG_PATTERN | URL_TOKEN | QUOTED_STRING] {
	        char *newstring;

		if ((newstring = CALLOC(char, 
		      (strlen(cf_lval.string)+strlen(idx_subkey->key)+2))) 
		      == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}
                sprintf(newstring, "%s %s", idx_subkey->key, cf_lval.string);
		/* printf("TESTING NEWSTRING:%s %s\nCF_LVAL %s  SUBKEY %s\n",
                 *         idx_subkey->key, cf_lval.string, idx_subkey->key);
		 * fflush(stdout); 
                 */
		FREE(cf_lval.string);
		FREE(idx_subkey->key);
		idx_subkey->key = newstring;
        }
	]*
	ATTRVAL_END { subkey_nesting--;} ;

replaceval:  REPLACEVAL_START { subkey_nesting++;}
	[NAME_TOKEN | FILENAME_LITERAL | URL_TOKEN | QUOTED_STRING] {
		/* allocate space for a new key entry and fill it in */
		/* this is recursive and adds keys for               */
		/* any number of nesting levels...                   */
		prev_subkey = idx_subkey;	
		if ((idx_subkey = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}

		if (prev_subkey == NULL) {
		  /* must be the first index entry in the list */
		  idx_keys->subkey = idx_subkey;
		}
		else
		    prev_subkey->subkey = idx_subkey;

		idx_subkey->next_key = NULL;
		idx_subkey->key =  cf_lval.string;
		idx_subkey->attribute_flag = 3; /* indicates pattern value */
	} 
	[NAME_TOKEN | FILENAME_LITERAL | URL_TOKEN] {
		/* allocate space for a new key entry and fill it in */
		/* this is recursive and adds keys for               */
		/* any number of nesting levels...                   */
		prev_subkey = idx_subkey;	
		if ((idx_subkey = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_subkey\n");
		  exit (1);
		}

		if (prev_subkey == NULL) {
		  /* must be the first index entry in the list */
		  idx_keys->subkey = idx_subkey;
		}
		else
		    prev_subkey->subkey = idx_subkey;

		idx_subkey->next_key = NULL;
		idx_subkey->key =  cf_lval.string;
		idx_subkey->attribute_flag = 4; /* indicates replacement value */
	} 
	REPLACEVAL_END { subkey_nesting--;} ;



xpath: 	XPATH_START 
      	[XPATH_SPEC | FILENAME_LITERAL] {
		/* allocate space for a new key entry and fill it in */
		prev_idx_key = idx_keys;	
		/* if ((idx_keys = CALLOC(idx_key,1)) == NULL) {
		 *  fprintf(LOGFILE, "No malloc space for idx_key\n");
		 * exit (1);
		 *}
		 */

		idx_keys = cf_build_xpath_key(cf_lval.string);

		if (prev_idx_key == NULL) {
		  /* must be the first index entry in the list */
		  head_idx_keys = idx_keys;
		}
		else
		  prev_idx_key->next_key = idx_keys;
                /* initialize for possible subkeys */
		subkey_nesting = 0;
      		prev_subkey = NULL; 
      		idx_subkey = NULL;

	} 
      	XPATH_END ;

sql: 	SQL_START 
      	[SQL_DATA | FILENAME_LITERAL | XPATH_SPEC] {
		/* allocate space for a new key entry and fill it in */
		prev_idx_key = idx_keys;	
		if ((idx_keys = CALLOC(idx_key,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for idx_key\n");
		  exit (1);
		}
		if (prev_idx_key == NULL) {
		  /* must be the first index entry in the list */
		  head_idx_keys = idx_keys;
		  idx_keys->subkey = NULL; /* no subkeys yet */
		}
		else
		prev_idx_key->next_key = idx_keys;
		idx_keys->next_key = NULL;
		idx_keys->key =  cf_lval.string;
        idx_keys->attribute_flag = 11; /* actually SQL flag */
	} 
      	SQL_END ;


explain: EXPLAIN_START {
	if ((cf_info->databaseinfo = CALLOC(DatabaseInfo,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for databaseinfo\n");
		  exit (1);
		}
	cf_lval.string = NULL;
	explain_lang = NULL;
	}
	[title 
	| description 
	| disclaimers
	| news
	| hours
	| besttime
	| lastupdate
	| updateinterval
	| coverage
	| proprietary
	| copyrighttext
	| copyrightnotice
	| producercontact
	| suppliercontact
	| submissioncontact]*
	EXPLAIN_END ;

title: TITLESTRING_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string;}
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->titleString = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->titleString = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);

				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	TITLESTRING_END ;

description: DESCRIPTION_START 
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->description = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->description = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	DESCRIPTION_END ;

disclaimers: DISCLAIMERS_START 
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; } 
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->disclaimers = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->disclaimers = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	DISCLAIMERS_END ;

news: NEWS_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->news = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->news = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	NEWS_END ;

hours: HOURS_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->hours = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->hours = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	HOURS_END ;

besttime: BESTTIME_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->bestTime = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->bestTime = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	BESTTIME_END ;

lastupdate: LASTUPDATE_START {explain_lang = NULL;}

	EXPLAIN_TEXT { cf_info->databaseinfo->lastUpdate = 
			 NewGeneralizedTime(cf_lval.string);
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	LASTUPDATE_END ;

updateinterval: UPDATEINTERVAL_START {
	if ((cf_info->databaseinfo->updateInterval = CALLOC(IntUnit,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for updateInterval\n");
		  exit (1);
		}

	}
	intervalval
	intervalunit
	UPDATEINTERVAL_END ;

intervalval: VALUE_START
	     NUMBER {cf_info->databaseinfo->updateInterval->value =
			cf_lval.intval ;
			if (cf_lval.string)	
				FREE(cf_lval.string);	
			cf_lval.string = NULL;
		}
	     VALUE_END ;

intervalunit: UNIT_START
	      NAME_TOKEN {
		if ((cf_info->databaseinfo->updateInterval->unitUsed = 
			CALLOC(Unit,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for updateInterval units\n");
		  exit (1);
		}
		cf_info->databaseinfo->updateInterval->unitUsed->unitType =
		        BuildStringOrNumeric("time",0,0);
		
		cf_info->databaseinfo->updateInterval->unitUsed->unit =
		        BuildStringOrNumeric(cf_lval.string,0,0);

		cf_info->databaseinfo->updateInterval->unitUsed->scaleFactor = 1;
		FREE(cf_lval.string);
		cf_lval.string = NULL;
		
		}
	      UNIT_END;

coverage: COVERAGE_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->coverage = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->coverage = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	COVERAGE_END ;

proprietary: PROPRIETARY_START
	PROP_TRUE? { cf_info->databaseinfo->proprietary = 1;}
	PROP_FALSE? { cf_info->databaseinfo->proprietary = 0;}
	PROPRIETARY_END ;

copyrighttext: COPYRIGHTTEXT_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->copyrightText = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->copyrightText = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	COPYRIGHTTEXT_END ;

copyrightnotice: COPYRIGHTNOTICE_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 cf_info->databaseinfo->copyrightNotice = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			     cf_info->databaseinfo->copyrightNotice = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	COPYRIGHTNOTICE_END ;

producercontact: PRODUCERCONTACTINFO_START {
		if ((tmp_contact = CALLOC(ContactInfo,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for contactinfo\n");
		  exit (1);
		}
	}
	contact {cf_info->databaseinfo->producerContactInfo = tmp_contact;
		 tmp_contact = NULL;
		}
	PRODUCERCONTACTINFO_END;

suppliercontact: SUPPLIERCONTACTINFO_START {
		if ((tmp_contact = CALLOC(ContactInfo,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for contactinfo\n");
		  exit (1);
		}
	}
	contact {cf_info->databaseinfo->supplierContactInfo = tmp_contact;
		 tmp_contact = NULL;
		}
	SUPPLIERCONTACTINFO_END;

submissioncontact: SUBMISSIONCONTACTINFO_START {
		if ((tmp_contact = CALLOC(ContactInfo,1)) == NULL) {
		  fprintf(LOGFILE, "No malloc space for contactinfo\n");
		  exit (1);
		}
	}
	contact {cf_info->databaseinfo->submissionContactInfo = tmp_contact;
		 tmp_contact = NULL;
		}
	 SUBMISSIONCONTACTINFO_END ;

contact:
	cname?
	cdesc?
	caddress?
	cemail?
	cphone? ;

cname: CONTACT_NAME_START
	EXPLAIN_TEXT {tmp_contact->name = 
			NewInternationalString(cf_lval.string);
			FREE(cf_lval.string);
			cf_lval.string = NULL;
			}
	CONTACT_NAME_END ;

cdesc:	CONTACT_DESCRIPTION_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 tmp_contact->description = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			 tmp_contact->description = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	CONTACT_DESCRIPTION_END ;

caddress: CONTACT_ADDRESS_START
       LANGUAGE_ATTR?
	NAME_TOKEN? { explain_lang = cf_lval.string; }
        ATTRIBUTES_END 
	EXPLAIN_TEXT { if (explain_lang == NULL)
			 tmp_contact->address = 
				BuildHumanString(cf_lval.string, "eng");
			else {
			 tmp_contact->address = 
				   BuildHumanString(cf_lval.string, 
						    explain_lang);
				FREE(explain_lang);
			        explain_lang = NULL;
			}
			FREE(cf_lval.string);
			cf_lval.string = NULL;
		}	
	 CONTACT_ADDRESS_END ;

cemail: CONTACT_EMAIL_START
	EXPLAIN_TEXT {tmp_contact->email = 
			NewInternationalString(cf_lval.string);
			FREE(cf_lval.string);
			cf_lval.string = NULL;
			}
	 CONTACT_EMAIL_END ;

cphone: CONTACT_PHONE_START
	EXPLAIN_TEXT {tmp_contact->phone = 
			NewInternationalString(cf_lval.string);
			FREE(cf_lval.string);
			cf_lval.string = NULL;
			}
	 CONTACT_PHONE_END ;



%lexical next_token ;

