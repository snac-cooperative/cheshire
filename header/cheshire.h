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
*	Header Name:	cheshire.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Global constants and structure declarations 
*                       for the Cheshire II system.
*	Usage:		#include cheshire.h
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/19/93
*	Revised:	
*	Version:	1.0 10/19/93
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* some standard includes */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <db.h>
#include <string.h>
#include <marclib.h>
#include <lcctree.h>
#include <sgml.h>
#include <time.h>
#include "z3950_3.h" /* we are using DatabaseInfo for explain stuff */

#include <tk.h>

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifdef ALPHA
#include <alpha.h>
#endif

/*********************************************************************/
/* Definitions for common constants                                  */
/*********************************************************************/
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef SUCCEED
#define SUCCEED 1
#endif

#ifndef FAIL
#define FAIL 0
#endif


/*********************************************************************/
/* special char definitions  for MARC records                        */
/*********************************************************************/
#ifndef SUBFDELIM
#define SUBFDELIM '\037'
#define FIELDTERM '\036'
#define RECTERM   '\035'
#endif

/**************************************************************/
/* definitions for query processing 			      */
/* designed to conform to Z39.50, indexes are not included    */
/* see the config file definitions below for index handling   */
/**************************************************************/  
/* RELATION Attributes */
#define LT    1
#define LTEQ  2
#define EQUAL 3
#define GTEQ  4
#define GT    5
#define NEQ   6
#define PHONETIC  100
#define STEM      101
#define RELEVANCE 102
#define ALWAYSMATCHES 103

/* POSITION Attributes */
#define FIRST_IN_FIELD     1
#define FIRST_IN_SUBFIELD  2
#define ANY_POSITION       3

/* STRUCTURE Attributes */
#define PHRASE        1
#define WORD          2
#define KEY           3
#define YEAR          4
#define DATE_NORM     5
#define WORD_LIST     6
#define DATE_UNORM    100
#define NAME_NORM     101
#define NAME_UNORM    102
#define STRUCTURE     103
#define URX           104
#define FREE_TEXT     105
#define DOCUMENT_TEXT 106
#define LOCAL_NUMBER  107
#define STRING        108
#define NUMERIC_STRING 109

/* TRUNCATION attributes */
#define RIGHT_TRUNC      1
#define LEFT_TRUNC       2
#define LEFT_RIGHT_TRUNC 3
#define DO_NOT_TRUNCATE  100
#define PROCESS_POUND    101
#define REG_EXPR_ONE     102
#define REG_EXPR_TWO     103

/* COMPLETENESS attributes */
#define INCOMPLETE_SUBF  1
#define COMPLETE_SUBF    2
#define COMPLETE_FIELD   3


/*********************************************************************/
/* structure for postings/retrieval results                          */
/*********************************************************************/

typedef struct prox_idx_pos_key {
  int termid;  
  int recnum;
} prox_idx_pos_key;

typedef struct idx_posting_entry {
  int record_no;
  int term_freq;
} idx_posting_entry;


typedef struct idx_results {
  int termid;                  /* The index term id from the index      */
  char *term;                   /* the index term                        */
  int  num_entries;             /* number of docs/entries with this term */
  int  tot_freq;                /* sum of term_frequency for this term   */
  idx_posting_entry entries[1]; /* other entries may follow              */
} idx_result;                   /* this is dynamically allocated, so     */
                                /* the actual size is variable           */
                                /* there will actually be num_entries +1 */
                                /* actual entries -- the last with 0, 0  */


/*********************************************************************/
/* structures for receiving present requests and sending results     */ 
/*********************************************************************/

typedef struct display_entry {
	int record_num;
	float weight;
	char *record_data;
} display_entry;

typedef struct display_result {
	char setid[100];
	int num_hits;		/* total number of hits */
	int num_entries;	/* number included in structure */
	display_entry entries[1];
} display_result;

/*********************************************************************/
/* structure for dictionary entry contents of keyword or exact index */
/*********************************************************************/
typedef struct dict_info   /* dictionary info structure stored in btree */
{
  int  tot_numrecs; /* total number of differ records with this key */
  int  tot_occur;   /* total number of occurences of this key i.e.
			weights of each record with this key added up */
  int recptr;   /* logical id or file pointer of 
                          index record or mapping file*/
} dict_info ;


/*********************************************************************/
/* structures for config file information about indexes               */
/*********************************************************************/

typedef struct idx_key {
  char *key;
  int attribute_flag; /* indicates that the key is a sgml attribute name 
		       * or value, etc. these occur as the final sub-keys 
		       * after a sequence of one or more element keys   */
  int occurrence;     /* specifies which occurrence of siblings with the 
		       * the same parent path is to be a match -- if 0 
		       * (the default) it will be all of them     */
  struct idx_key *subkey;
  struct idx_key *next_key;
} idx_key;

typedef struct ranking_parameters {
  int type;
  int id;
  float val;
  struct ranking_parameters *next_parm;
} ranking_parameters;

typedef struct continuation 
{
  char *name;
  int id_number;   /* Sequence/id for this continuation              */
  int docid_min;   /* lowest docid in the file                       */
  int docid_max;   /* highest docid in the file                      */
  int dbflag;
  DB *db;          /* used when this is an index file continuation or 
		      DataStore continuation. When dbflag is 1 this is a 
		      DB of filename data keyed on (each) docid */
  FILE *file;      /* used for all other file continuations          */
  struct continuation *next_cont; /* next continuation for this file */
} continuation;

/* Component subelements extraction and indexing information is stored */
/* in the structure below                                             */

typedef struct component_list_entry {
  char *name;             /* name of the component, i.e., file name  */
  DB   *comp_db;          /* DBopen access structure for components  */
  struct config_file_info *config_info; /* parent config file info   */
  int normalization;      /* special processing flags for component  */
  int max_component_id;   /* used to keep track during indexing */
  idx_key *start_tag;    /* definition of start element for component*/
  idx_key *end_tag;      /* definition of start element for component*/
  struct idx_list_entry *indexes; /* the head of the linked list of index entries */
  struct component_list_entry *next_entry;
} component_list_entry;


/* the following is the data actually stored in the component DB files */
/* note that the same structure is used for storing document-level info*/
/* indicating what range of component numbers occurs in the document   */
/* in this later case the "start_offset" is the beginning component id */
/* and the "end_offset" is the ending component ID.                    */
typedef struct component_data_item {
  int record_id;
  int start_offset;  /* these are offsets in the SGML_Document buffer     */
  int end_offset;    /* AFTER substituting entities and processing instr. */
} component_data_item;

#define MAXATTRTYPE 13
#define USE_ATTR 1
#define RELATION_ATTR 2
#define POSITION_ATTR 3
#define STRUCTURE_ATTR 4
#define TRUNC_ATTR 5
#define COMPLETENESS_ATTR 6
#define ACCESS_POINT_ATTR 1
#define SEMANTIC_QUAL_ATTR 2
#define LANGUAGE_ATTR_TYPE 3
#define CONTENT_AUTH_ATTR 4
#define EXPANSION_ATTR 5
#define NORMALIZED_WEIGHT_ATTR 6
#define HIT_COUNT_ATTR 7
#define COMPARISON_ATTR 8
#define FORMAT_ATTR 9
#define OCCURRENCE_ATTR 10
#define INDIRECTION_ATTR 11
#define FUNC_QUAL_ATTR 12
/* structure to map from Z39.50 attributes to indexes */
typedef struct attr_map {
  int attr_nums[MAXATTRTYPE];
  char *attr_string[MAXATTRTYPE];
  char *attr_set_oid[MAXATTRTYPE];
  char *attributeSetOID;
  char *altdescription;
  struct idx_list_entry *idx_entry;
  struct attr_map *next_attr_map;
  struct attr_map *next_attr_map_file;
} attr_map;

typedef struct idx_list_entry {
  int type;                    /* type of index file                 */
  char *tag;                   /* tag for finding the entry usually  */
                               /*   same as index language tag       */
  char *name;                  /* name of the index file             */
  char *stopwords_name;        /* name of the stopwords file         */
  char *expansion_name;        /* name of the expansion words file   */
  char *extern_app_name;       /* URL reader for external data       */
  continuation *indxcont;      /* index file continuation            */
  idx_key *exclude;            /* fields excluded from indexing      */
  idx_key *keys;               /* fields or functions indexed        */
  DB   *db;                    /* pointer to dbopen access structure */
  DB   *prox_db;               /* DBopen access structure for Proximity */
  DB   *vector_db;             /* DBopen access structure for Vectors */
  FILE *stopwords_file;        /* file pointer for postings          */
  FILE *expansion_file;        /* file pointer for postings          */
  char *dateformat;
  /* the following is used if this type is a COMPONENT_INDEX to point*/
  /* to the component definition from the config file                */
  component_list_entry *comp_parent;

  /* The following two files are used only for indexes with the      */
  /* KEYWORD_EXTERNAL (EXTERNKEY) and PAGEDINDEX flags set in the    */
  /* type variable. They are generated automatically at indexing     */
  /* time based on the index file name with the extensions PAGEDMAP  */
  /* PAGEDFILES                                                      */
  FILE *page_rec_assoc_file;   /* file pointer for file containing   */
                               /*   mapping from page_docid to docid */
                               /*   and the page size (in words)     */
                               /*   and the offset in page_file_names*/
                               /*   for the name of the page file    */
                               /*   see the page_assoc_rec struct    */
  FILE *page_file_names;       /* file pointer for file containing   */
                               /*  page file names (path in main rec)*/
  int highest_page_number;    /* max page number assigned to a page */
                               /*  file (in parentid)                */
  dict_info *GlobalData;       /* global frequency info for the index*/
                               /* recptr is used for max termid      */
                               /* the allocated item is shared by all*/
                               /* indexes using the same db file     */
  Tcl_HashTable stopwords_hash; /* base hash table for stopwords     */
  Tcl_HashTable expansion_hash; /* base hash table for expansion words */

  ranking_parameters *ranking_parameters; /* coefficients to pass to ranking*/

  attr_map *bib1_attr_map; /* list of z39.50 attributes */

  int type2;  /* additional type definitions */
  int snowball_stem_type;  /* see defines below... */

  /* the following are mainly for gazetteer indexing */
  struct config_file_info *gaz_config_file; 
  struct idx_list_entry *gaz_idx;
  char *gaz_data_tag;

  struct idx_list_entry *next_entry; /* pointer to next index_list_entry*/
} idx_list_entry;



/*********************************************************************/
/* structures for vector index storage                               */
/*********************************************************************/

typedef struct vec_term_id {
  int id;
  char code;
} vec_term_id;

typedef struct vec_term_overlay {
  int coll_freq;
  char term[1];
} vec_term_overlay;

typedef struct vec_term_info {
  int termid;
  int termfreq;
} vec_term_info;

typedef struct vector_data_info {
  float sqrt_sum_tfidf_sq;
  float avg_term_freq;
  int max_term_freq;
  int num_terms;
  vec_term_info entries[1];

} vector_data_info;


/*********************************************************************/
/* structures for interim weighted results                           */
/*********************************************************************/

typedef struct weighted_entry {
  int record_num;
  void *xtra;
  float weight;
} weighted_entry;


#define BOOLEAN_RESULT 0
#define PROBABILISTIC_RESULT 1
#define FEEDBACK_RESULT (1<<1)
#define CLUSTER_RESULT (1<<2)   /*4*/
#define PAGED_RESULT (1<<3)     /*8*/
#define PAGED_DOC_RESULT (1<<4) /*16*/
#define COMPONENT_RESULT (1<<5) /*32*/
#define PROXIMITY_RESULT (1<<6) /*64*/
#define VIRTUAL_RESULT (1<<7)   /*128*/
#define SORTED_RESULT_SET (1<<8)/*256*/
#define OKAPI_RESULT (1<<9)     /*512*/
#define CORI_RESULT (1<<10)     /*1024*/
#define BITMAP_RESULT (1<<11)   /*2048*/
#define FUZZY_RESULT (1<<12)    /*4096*/
#define FUZZY_REQUEST (1<<13) /*8192*/
#define BLOB_RESULT (1<<14) /*16384*/
#define GEORANK_RESULT (1<<15) /*32k*/
#define TFIDF_RESULT (1<<16) /*64k*/
#define HYBRID_RESULT (1<<17) /*128k*/

#define BOOLEAN_AND 0
#define BOOLEAN_OR  1
#define BOOLEAN_NOT 2
#define PROX_OPERATOR 3


#define SNOWBALL_PORTER 1
#define SNOWBALL_ENGLISH 2
#define SNOWBALL_FRENCH 3
#define SNOWBALL_GERMAN 4
#define SNOWBALL_DUTCH 5
#define SNOWBALL_SPANISH 6
#define SNOWBALL_ITALIAN 7
#define SNOWBALL_SWEDISH 8
#define SNOWBALL_PORTUGUESE 9
#define SNOWBALL_RUSSIAN 10
#define SNOWBALL_DANISH 11
#define SNOWBALL_NORWEGIAN 12
#define SNOWBALL_FINNISH 13
#define SNOWBALL_HUNGARIAN 14
#define SNOWBALL_TURKISH 15
#define SNOWBALL_ROMANIAN 16

#define SNOWBALL_PORTER_UTF8 101
#define SNOWBALL_ENGLISH_UTF8 102
#define SNOWBALL_FRENCH_UTF8 103
#define SNOWBALL_GERMAN_UTF8 104
#define SNOWBALL_DUTCH_UTF8 105
#define SNOWBALL_SPANISH_UTF8 106
#define SNOWBALL_ITALIAN_UTF8 107
#define SNOWBALL_SWEDISH_UTF8 108
#define SNOWBALL_PORTUGUESE_UTF8 109
#define SNOWBALL_RUSSIAN_UTF8 110
#define SNOWBALL_DANISH_UTF8 111
#define SNOWBALL_NORWEGIAN_UTF8 112
#define SNOWBALL_FINNISH_UTF8 113
#define SNOWBALL_HUNGARIAN_UTF8 114
#define SNOWBALL_TURKISH_UTF8 115
#define SNOWBALL_ROMANIAN_UTF8 116




typedef struct weighted_result {
  char setid[200];
  char filename[500];
  int result_type;
  struct component_list_entry *component;
  int termid[100]; /* terms included in result -- up to first 100 */
  idx_list_entry *index_ptr[100];
  int num_hits;
  int doc_hits;
  float result_weight;
  float max_weight;
  float min_weight;
  weighted_entry entries[1];
} weighted_result;



/*********************************************************************/
/* structure for config file information about clusters              */
/*********************************************************************/
typedef struct cluster_map_entry {
  idx_key *from;
  idx_key *to;
  idx_key *summarize;
  int sum_maxnum;
  struct cluster_map_entry *next_clusmap;
} cluster_map_entry;

typedef struct cluster_list_entry {
  char *name;                  /* name of the cluster file             */
  FILE *temp_file;
  char *temp_file_name;
  int normalization;
  idx_key *cluster_key;        /* definition of key element for cluster*/
  FILE *cluster_stoplist;
  char *cluster_stoplist_name;
  Tcl_HashTable stopwords_hash; /* base hash table for stopwords     */
  cluster_map_entry *field_map;/* mapping of file tags to cluster file tags */
  struct cluster_list_entry *next_entry;
} cluster_list_entry;

/*********************************************************************/
/* structure for config file information about display formats       */
/* actually, it is element sets for Z39.50 search and present        */
/*********************************************************************/
typedef struct disp_format_list {
  char *name;            /* Name of the format (element set name)    */
  char *oid;             /* Z39.50 OID for recsyntax conversion (optional) */
  char *marc_dtd_file_name; /* used for marc conversions */
  int default_format;    /* is it the default?                       */
  idx_key *include;      /* tag specifications to INCLUDE in format  */
  idx_key *exclude;      /* tag specifications to EXCLUDE in format  */
  int exclude_compress;  /* collapse data in required exclude sections */
  char *convert_name;    /* Name for conversion function to run on   */
  cluster_map_entry *convert;   /* this list of tags, or everything if null */
  struct disp_format_list *next_format; /* another format */
} display_format;

/* defines for file_type information */
#define FILE_TYPE_SGMLFILE 0
#define FILE_TYPE_MARCFILE 1
#define FILE_TYPE_AUTHORITYFILE 2
#define FILE_TYPE_CLUSTER 3
#define FILE_TYPE_LCCTREE 4
#define FILE_TYPE_MAPPED 5
#define FILE_TYPE_DBMS 6
#define FILE_TYPE_EXPLAIN 7
#define FILE_TYPE_VIRTUALDB 8
#define FILE_TYPE_XML 9
#define EXTERNAL_LOOKUP_FILE 10
#define FILE_TYPE_XML_NODTD 11

#define FILE_TYPE_SGML_DATASTORE 100
#define FILE_TYPE_MARC_DATASTORE 101
#define FILE_TYPE_CLUSTER_DATASTORE 103
#define FILE_TYPE_EXPLAIN_DATASTORE 107
#define FILE_TYPE_XML_DATASTORE 109


typedef struct filelist {
  char *filename;
  struct filelist *next_filename;
} filelist;

/************************************************************************/
/* config_file_info is the base structure for configfile information    */
/* it includes all the important file-related and nameing information   */
/* about a database.                                                    */
/************************************************************************/
typedef struct config_file_info {
  char *file_name;                /* name of the data file              */
  char *nickname;                 /* short name for finding the entry   */
  filelist *othernames;           /* aliases for this file              */
  char *defaultpath;              /* default directory for files        */
  int  file_type;                 /* type of file (see defines) below   */
  FILE *file_ptr;                 /* file pointer from f_open           */
                                  /* file_ptr is a SGML file when       */ 
				  /* filecont is NULL and points to ONE */
                                  /* continuation file when filecont is */
                                  /* not NULL                           */
  DB   *file_db;                  /* DBopen access structure for DataStore */
  continuation *filecont;         /* list of continuations of this file */
  char *dbenv_path;               /* path of DB environment             */
  int cont_id;                    /* id of current cont file open as    */
                                  /* file_ptr                           */
  continuation *current_cont;     /* point to currently open cont file  */

  char *assoc_name;               /* associator file name               */
  FILE *assoc_ptr;

  char *history_name;
  FILE *history_ptr;
  idx_key *distrib_docsize_key;
  FILE *docsize_ptr;  /* special file used with KEYWORD_EXTERNAL files  */

  char *DTD_name;
  int XML_Schema;     /* if this flag is not 0 then this is an XML_Schema */
  char *XML_Schema_Name;
  filelist *included_schemas;
  char *SGML_Catalog_name;
  SGML_DTD *DTD_parsed;

  int  num_indexes;
  int  encoding;        /* default (0) encoding is ISO-8859-1 */

  idx_list_entry *indexes; /* the head of the linked list of index entries */
  idx_list_entry *primary_key; /* primary key index (or NULL) */
  cluster_list_entry *clusters; /* head of a list of cluster entries */
  int display_options;
  display_format *display;

  component_list_entry *components; /* head of a list of sub-components */

  char *clustered_file; /* name of the file clustered by this file if this */
                        /* file has a type of CLUSTER                      */

  int use_attr[300]; /* z39.50 use attributes for this file (quick check)    */
  /* this array is a bit mapping of z39.50 use attributes */
  /* i.e. if bit 2 is "on" use attribute 2 is supported   */
  /* for this file -- use the USE_BIT_ON macro to test    */
  /* and the USE_BIT_SET macro to set. (defined below)    */
  /* Note that the MAXIMUM USE attribute value is 9600    */

  attr_map *bib1_attr_map; /* complete list of supported bib1 attributes    */
                           /* follow via the attr_map *next_attr_map_file   */
                           /* chain of attr_map structures                  */
  
  /* This hash table is allocated and initialized when an index is opened */
  Tcl_HashTable *IndexNamesHash;

  /* Explain information for this database/file (filled in by config parser */
  /* and from the other information here in config_file_info                */
  DatabaseInfo *databaseinfo;

  struct config_file_info *next_file_info;
  
} config_file_info;    

#define NBITS (sizeof(int) * 8)

#define USE_BIT_ON(ptr,bit) ((ptr->use_attr[(bit/NBITS)]) & ((int)1 << (bit % NBITS))) == ((int)1 << bit % NBITS)

#define USE_BIT_SET(ptr,bit) ptr->use_attr[(bit/NBITS)] |= ((int)1 << bit % NBITS)

/* constants for file type identification */
#define MAINFILE 1
#define ASSOCFILE 2
#define POSTINGFILE 3
#define HISTORYFILE 4
#define DTDFILE 5
#define DOCSIZEFILE 6


/* index file types */
#define INDEXFL 1
#define INDEXASSOC 2
#define INDEXPOSTINGS 3
#define INDEXSTOPWORDS 4
#define INDEXEXPANSION 11

/* cluster file types */
#define CLUSTERTEMP 5
#define CLUSTERFILE 6

/* paged file types */
#define PAGEASSOCFILE 7
#define PAGENAMEFILE 8

/* proximity data file */
#define INDEXPROXFL 9

/* vector data file */
#define INDEXVECTORFL 10

/* Structure for associator files */
typedef struct assoc_rec {
  unsigned int offset;
  unsigned int recsize;
} assoc_rec;


/* Structure for page associator files */
typedef struct page_assoc_rec {
  int parentid;
  int pagenum;
  int recsize;
  int nameoffset; /* offset to beginning of page file name */
} page_assoc_rec;


/* Structure for History files */
typedef struct history_file_info {
  int recno;
  int reclen;
  int offset;
  int transaction;
  time_t time;
}  history_file_info;

typedef struct batch_files {
  idx_list_entry *idx;
  char *index_name;
  FILE *outfile;
  int  load_flag;
  struct batch_files *next;
} batch_files_info;

/* history file transactions types */
#define H_ADD     1
#define H_DELETE  2
#define H_REPLACE 4
#define H_VACUUMED  8

/* forward declarations */
FILE *cf_open(char *filename, int which);
FILE *cf_open_cont(char *filename, int which, int recordid);
void *cf_index_open(char *filename, char *indexname, int which);
FILE *cf_cluster_open(char *filename, char *clustername, int which);
DB *init_index_db (char *idxname, int idxtype, int *num_open);
config_file_info *cf_initialize(char *cf_name, char *createflags, 
				char *openflags);

#ifdef MAIN 
/*instances of variables that should be declared only in main or its */
/* equivalent.                                                       */

/* global config file structure */
config_file_info *cf_info_base = NULL;
Tcl_HashTable *IndexNamesHash = NULL;
Tcl_HashTable *cf_file_names = NULL, cf_file_names_data;

FILE *LOGFILE;

FILE *cheshire_log_file = NULL;
#endif
#ifndef MAIN

extern Tcl_HashTable *IndexNamesHash;
extern config_file_info *cf_info_base;
extern Tcl_HashTable *cf_file_names, cf_file_names_data;
 
extern FILE *LOGFILE;

extern FILE *cheshire_log_file;
extern DB_ENV *gb_dbenv; /* only in new_var version */

#endif

