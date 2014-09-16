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
*       Header Name:    displayrec.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        ZSERVER handling for element set names
*                       and record syntax conversions
*
*       Usage:          displayrec(SGML_Document *doc, int docid, 
*                                  char **buf, long maxlen, 
*                                  char *format, char *oid, char *filename, 
*                                  char *add_tags,
*                                  char **outptr, int rank, int relevance, 
*                                  float raw_relevance, 
*                                  weighted_result *resultset)

*                       doc = SGML Document structure
*                       docid = internal document id number
*                       buffer = buffer pointer for resulting document
*                       format = element set name or recsyntax name to use
*                       oid = recsyntax oid for the format
*                       filename = name of the current datafile.
*                       add_tags = tags to add to the buffer.
*                       outptr = pointer to pointer for returning structured
*                                records (GRS-1, etc.)
*
*       Variables:
*
*       Return Conditions and Return Codes: return 0 on error or
*
*       Date:           11/7/93
*       Revised:        4/8/98
*       Version:        1.4
*       Copyright (c) 1993, 1998.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
#include <strings.h>
#include <netdb.h>
#endif
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"
#include "cheshire.h"
#include "configfiles.h"
#include "dispmarc.h"
#include <dmalloc.h>
#include "tagsets.h"
#define EXCLUDE_INIT
#include "indicator.h"
extern SGMLINDS indicators[];


#define RECMAP 0
#define TAGSET_M 1
#define TAGSET_G 2
#define MIXED    3 /* MIXED uses tagset G but expands the subtrees */
#define MARC_CONV 4
#define XPATH_ONLY_OUTPUT 5
#define SUBTREES 0
#define FLATTEN  1



struct data_list {
  SGML_Data *item;
  int required;
  int haskeys;
  char *keywords;
  idx_key *key;
  cluster_map_entry *map;
  SGML_Tag_Data *from_list_map;
   struct data_list *groupend;
  struct data_list *next;
} *first_data_list, *current_data_list, *last_data_list;


SGML_DTD *marc_dtd = NULL; /* used in MARC conversions */ 

/* I tried, I really tried several different approaches but static doesn't */
/* seem to work either globally or inside functions for some damn reason */
/*static*/ Tcl_HashTable *kw_highlight_hash = NULL;
/*static*/ char *kw_highlight_string = NULL;
/*static*/ int **kw_offset_table = NULL;


/* extern and local functions */
TaggedElement *MakeTaggedElement(SGML_Data *data, int type, int tag, char *stoptag);
extern char *detag_data_block(SGML_Data *data, int index_type,
			      idx_list_entry *idx);

void display_build_list_keys(SGML_Document *doc, idx_key *key, 
			     cluster_map_entry *map, int docid, int type);

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern config_file_info *find_file_config(char *filename);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);

void display_build_list_map(SGML_Document *doc, 
			    cluster_map_entry *map, int docid, int rank,
			    int relevance, float raw_relevance, 
			    char *conv_name, char *format, char *filename);

extern SGML_DTD *sgml_parse_dtd(char *filename, char *sgml_catalog_name, 
			 char *schema_file_name, filelist *fl, int dtd_type);

extern SGML_Document *sgml_parse_document(SGML_DTD *dtd, char *filename, 
				   char *buffer, 
				   int recordnum, int keep_buffer);


extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int dia_flag);

int display_convert(display_format *df, SGML_Document *doc, 
		    char **buf, long maxlen,
		    struct data_list *convert, char *funcname, char *add_tags,
		    char **outptr, char *source_file_name, 
		    char *compname, int compid);

char *cf_getdatafilename(char *fname, int docid);

int display_exclude(SGML_Document *doc, char **buf, long maxlen,
		    struct data_list *exclude, int compress, char *addtags);

StringOrNumeric *MakeStringOrNumeric(char *string, int number, int which);

extern char *sgml_to_marc(SGML_Document *sgmlrec);

OctetString NewOctetStringN(char *inchar, int length);

int 
displayComponent(SGML_Document *parent_doc, SGML_Data *data, int docid, long maxlen, 
		 char *format, char **oid, char *filename, 
		 component_list_entry *comp,
		 char **outptr, int rank, int relevance, float raw_relevance);


void
free_offset_data(int **data) {
  int i, nlists;
  
  nlists = data[0][0];

  for (i = 1; i <= nlists; i++) {
    FREE(data[i]);
  }
  FREE(data);
}

int **
fetch_prox_info(int docid, weighted_result *resultset)
{
  DB *dbprox;
  DBT key, data;
  DBT proxkey, proxdata;
  int **proxlist;
  int *listsize;
  int *matchpos;
  int numints;
  int num_prox_terms;
  int result = 0;
  int minpos;
  int fail;
  int returncode;
  int listnum, list, i, j;
  idx_list_entry *idx;
  
  prox_idx_pos_key poskeyval;

  /* just count up the number of terms... */
  for (i = 0; resultset->termid[i] > 0; i++) {}

  num_prox_terms = i;
  proxlist = NULL;
  listsize = NULL;

  /* allocate an array of pointers for the actual prox lists for each term */
  if (num_prox_terms > 0) {
    proxlist = CALLOC(int *, num_prox_terms + 2);
    listsize = CALLOC(int, num_prox_terms +1);
    proxlist[0] = listsize;
    listsize[0] = num_prox_terms;
  }

  /* Initialize the prox key/data pair so the flags aren't set. */
  for (listnum = 0; listnum < num_prox_terms; listnum++) {
    idx = resultset->index_ptr[listnum];	

    if ((idx->type & (KEYWORD | PROXIMITY)) == (KEYWORD | PROXIMITY)) {
      /* this is a proximity index, so the following should work... */
      
      dbprox = idx->prox_db;

      if (dbprox == NULL)
	return 0;
      
      memset(&proxkey, 0, sizeof(proxkey));
      memset(&proxdata, 0, sizeof(proxdata));
      
      poskeyval.recnum = docid;
      poskeyval.termid = resultset->termid[listnum];
      
      proxkey.data = (void *) &poskeyval;
      proxkey.size = sizeof( prox_idx_pos_key);;
      
      returncode = dbprox->get(dbprox, NULL, &proxkey, &proxdata, 0); 
      
      if (returncode == 0) { /* found the word in the index */
	numints = proxdata.size / sizeof(int);
	proxlist[listnum + 1] = CALLOC(int, numints + 1);
	listsize[listnum + 1] = numints;
	/* get the existing data (Move to aligned space) */
	memcpy(proxlist[listnum + 1], (int *)proxdata.data, proxdata.size);
      }
    }
    /* we now have each of the lists for the term proximity info */
  }

  /* if nothing was done, the return is NULL */
  return (proxlist);
}


void
free_datalist(struct data_list *in)
{
  if (in == NULL) return;
  free_datalist(in->next);
  free_tag_data_list(in->from_list_map);
  FREE(in);
}

/* the following two functions are no longer used, since only terms matching */
/* the search are flagged                                                    */
void 
clear_highlight_hash(Tcl_HashTable *hashtab) {

  if (hashtab)
    Tcl_DeleteHashTable(hashtab);

}

void
build_highlight_hash(char *word_string, weighted_result *resultset) 
{

  char *word_ptr, *result_word, *next_tok, *tmpwordptr;
  int exists, i;
  char *breakletters;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n\r\a\b\v\f<>";
  char *filenameletters = " \t\n\r\a\b\v\f<>";
  char *noletters = " \t\n\r\a\b\v\f";
  int morphed;
  Tcl_HashEntry *entry;
  idx_list_entry *idx;

  morphed = 0;

  tmpwordptr = strdup(word_string);

  if (resultset != NULL && resultset->index_ptr[0] != NULL) {
    idx = resultset->index_ptr[0];
  } 
  else {
    idx = NULL;
  }

  if (tmpwordptr == NULL || tmpwordptr[0] == '\0') {
    return;
  }

  next_tok = NULL;
  breakletters = keywordletters;

  kw_highlight_hash = CALLOC(Tcl_HashTable,1);
  Tcl_InitHashTable(kw_highlight_hash,TCL_STRING_KEYS);
  
  /* find first token */
  word_ptr = strtok_r (tmpwordptr, breakletters, &next_tok);
  /* a buffer full of blanks or punct? */
  if (word_ptr == NULL)
    return;
  
  do {
    char *tempwordptr;
    /* normalize the word according to the specs for this index */
    /* (as defined in indextype)                                */
    tempwordptr = word_ptr;
    
    /* diacritics already converted by detag if needed */
    result_word = normalize_key(word_ptr, idx, &morphed, 0);
    
    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL) {
      int result_len;
      
      if ((result_len = strlen(result_word)) > 0) {
	/* We have a normalized word, is it in the hash table? */
	entry = Tcl_FindHashEntry(kw_highlight_hash,result_word);
	
	
	if (entry == NULL){ /* nope, it is new word */
	  /* create the hash table structure */
	  
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       kw_highlight_hash,
					       word_ptr,
					       &exists),
			   (ClientData)result_word);
	  
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       kw_highlight_hash,
					       result_word,
					       &exists),
			   (ClientData)word_ptr);
	  
	  
	  /* we aren't freeing the keys or data because they are needed in the hash */
	  /* table */
	}
	else {
	  /* word already in the table */
#ifdef DEBUGDISPLAY
	  printf("Null stemmer output from %s\n", tempwordptr);
	  /* no longer logging these */
	  fprintf(LOGFILE, "Null stemmer output from %s\n", tempwordptr);
#endif
	  Tcl_SetHashValue(
			   Tcl_CreateHashEntry(
					       kw_highlight_hash,
					       word_ptr,
					       &exists),
			   (ClientData)tempwordptr);
	}
      }
      else {
	/* zero length after normalization -- should not happen */
#ifdef DEBUGDISPLAY
	printf("Zero length normalization result %s\n", tempwordptr);
#endif
      }
    }
    /* get the next word from the current buffer     */
    word_ptr = strtok_r (NULL, breakletters, &next_tok);
  } while (word_ptr != NULL);  
 
  return;
  
}

int 
displayrec(SGML_Document *doc, int docid, char **buf, long maxlen, 
	   char *format, char *oid, char *filename, char *add_tags,
	   char **outptr, int rank, int relevance, float raw_relevance,
	   weighted_result *resultset /* resultset from query processing */)
{
	
  SGML_Data *GetData();
  int result_recsize;
  display_format *df, *cf_getdisplay(), *cf_getdefaultdisplay();
  display_format fake_df;


  first_data_list = current_data_list = last_data_list = NULL;

  if (format[0] != '\0')
    df = cf_getdisplay(filename, oid, format);
  else
    df = cf_getdefaultdisplay(filename);

  if (df == NULL || df->include != NULL) { 
    /* no (usable) display stuff in config file... */
    if (df == NULL && format != NULL 
	&&(strcmp(format,"F") == 0 || strcmp(format,"B") == 0)
	&& (oid == NULL || strcmp(oid, SUTRECSYNTAX) == 0 
	    || strcmp(oid, XML_RECSYNTAX) == 0
	    || strcmp(oid, SGML_RECSYNTAX) == 0)) {
      /* the generic specs apply -- return the whole record */
      result_recsize = strlen(doc->buffer);
      if (result_recsize < maxlen) {
	*buf = CALLOC(char, result_recsize+1);
	strcpy(*buf,doc->buffer);
      }
      else
	result_recsize = -1; /* error code for rec too long */
      
      return (result_recsize);
    }
    else if (df == NULL && oid != NULL) { /* try to convert... */
      fake_df.name = "dummy";
      fake_df.oid = oid;
      fake_df.default_format = 0;
      fake_df.include = NULL;
      fake_df.exclude = NULL;
      fake_df.convert = NULL;
      fake_df.convert_name = "RECMAP";
      result_recsize = display_convert(&fake_df, doc, buf, maxlen, 
				       NULL, fake_df.convert_name,
				       add_tags, outptr, filename, NULL, 0);
      return result_recsize;
    }
    else /* not generic and not recognized */
      return (-2);
  }


  if (add_tags != NULL) {
    if (kw_highlight_string != NULL
	&& (strcmp(kw_highlight_string, add_tags) != 0)) {
      /* there is a old and different hash table and string, so clean it out */
      FREE(kw_highlight_string);
      kw_highlight_string = NULL;
      /* clear_highlight_hash(kw_highlight_hash); */
      kw_highlight_hash = NULL;
      if (kw_offset_table) {
	free_offset_data(kw_offset_table);
      }

    } 
    /* add the highlight criteria */
    if (kw_highlight_string == NULL 
	&& strcasecmp(add_tags, "HIGHLIGHT_INFO_SEARCH_WORDS") == 0) {
      /* if they are not NULL they are still valid */
      kw_highlight_string = strdup(add_tags);
      /* build_highlight_hash(kw_highlight_string, resultset); */
      kw_offset_table = fetch_prox_info(docid,resultset);
    }
    
  }

  /* OK, there is a display format for this file, so we apply it */
  if (df->exclude) { /* exclusion spec */
    display_build_list_keys(doc, df->exclude, NULL, docid, 0);
    result_recsize = display_exclude(doc, buf, maxlen, first_data_list,
				     df->exclude_compress, add_tags);
    *outptr = *buf;
  }
  if (df->convert_name) { /* conversion spec */
    display_build_list_map(doc, df->convert, docid, rank, relevance, 
			   raw_relevance,
			   df->convert_name, format, filename);
    result_recsize = display_convert(df, doc, buf, maxlen, 
				     first_data_list, df->convert_name,
				     add_tags, outptr, filename, NULL, 0);
  }

  if (first_data_list) free_datalist(first_data_list);

  return (result_recsize);
}


void 
add_data_list (SGML_Data *data, idx_key *key, cluster_map_entry *map, 
	       int RETAINORDER) {
  /* this adds entries to the global data list IN the sequence they appear */
  /* in the data UNLESS the RETAINORDER flag is set                        */
  struct data_list *prev_dl, *dl;
  int set = 0;

  current_data_list = CALLOC(struct data_list, 1);
    
  current_data_list->item = data;
  current_data_list->key = key;
  current_data_list->map = map;

  if (last_data_list == NULL) {
    last_data_list = first_data_list = current_data_list;
  }
  else {
    if (RETAINORDER) {
      last_data_list->next = current_data_list;
      last_data_list = current_data_list;
    }
    else {

      prev_dl = NULL;

      for (dl = first_data_list; dl ; dl = dl->next) {
	if (prev_dl == NULL && data->start_tag_offset < dl->item->start_tag_offset) {
	  /* it becomes the first item */
	  current_data_list->next = first_data_list;
	  first_data_list = current_data_list;
	  set = 1;
	}
	else if (data->start_tag_offset < dl->item->start_tag_offset && set == 0 &&
		 data->start_tag_offset > prev_dl->item->start_tag_offset) {
	  prev_dl->next = current_data_list;
	  current_data_list->next = dl;
	  set = 1;
	}

	prev_dl = dl;
      }
      last_data_list = prev_dl;
      
      if (set == 0) 
	last_data_list->next = current_data_list;
      else
	last_data_list->next = NULL;
      
    }
  }
}

struct data_list *
first_map_data_list(cluster_map_entry *map) {
  struct data_list *dl;
  for (dl = first_data_list; dl ; dl = dl->next) {
    if (dl->map == map)
      return (dl);
  }
  return (NULL);
}

void 
append_data_list (struct data_list *dl, SGML_Data *data) 
{
  struct data_list *prev_dl;
  int set = 0;
  SGML_Tag_Data *newtagdata, *td, *lasttd;
  
  lasttd = NULL;

  for (td = dl->from_list_map; td ; td = td->next_tag_data) {
    lasttd = td;
  }
  newtagdata = CALLOC(SGML_Tag_Data, 1);
  newtagdata->item = data;

  if (lasttd == NULL) {
    /* add an entry for the base item */
    td = CALLOC(SGML_Tag_Data,1);
    td->item = dl->item;
    td->next_tag_data = newtagdata;
    dl->from_list_map = td;
  }
  else
    lasttd->next_tag_data = newtagdata;

  return;

}

void 
add_data_attr (SGML_Data *data, idx_key *key, idx_key *attrkey,
	       cluster_map_entry *map, int RETAINORDER) 
{
  /* this adds entries to the global data list IN the sequence they appear */
  /* in the data UNLESS the RETAINORDER flag is set                        */
  int set = 0;
  SGML_Data *newdata;
  char *dummy;
  int length;
  SGML_Attribute_Data *at;
  char *tempptr;
  
  
  if (attrkey == NULL)
    return;
  
  tempptr = NULL;
  
  for (at = data->attributes; at; at = at->next_attribute_data) {
    if (strcasecmp(at->name, attrkey->key) == 0) {
      tempptr = at->string_val;
    }
  }    
  
  if (tempptr == NULL)
    return;
  
  dummy = CALLOC(char, (strlen(tempptr) + 15));
  sprintf(dummy, "<ATTR>%s</ATTR>",tempptr);    
  length = strlen(tempptr);

  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+6;
  newdata->content_end = dummy+6+length;
  newdata->end_tag = dummy+6+length;
  newdata->data_end = dummy+6+length+7;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 6;
  newdata->content_end_offset = 6+length;
  newdata->end_tag_offset = 6+length+1;
  newdata->data_end_offset = 6+length+7;
  
  add_data_list (newdata, key, map, RETAINORDER);

  return;
  
}


int
Xpathlen(SGML_Data *dat) {
  int elementlen;
  if (dat == NULL) 
    return 0;
  else {
    elementlen = (dat->content_start_offset - dat->start_tag_offset);
    if (elementlen < 0) {
      /* an empty element... */
      elementlen = 50;
    }
    return (Xpathlen(dat->parent) + elementlen + 4);
  }
}

int
Xpathcpy(SGML_Data *dat, char *buf) {
  int pathlen;
  char occur[20];
  char *tagptr;
  char *buffend;

  if (dat == NULL) {
    return (0);
  }
  else {
    pathlen = Xpathcpy(dat->parent, buf);
    
    strcat(buf, "/");
    pathlen++;
    buffend = buf + strlen(buf);

    for (tagptr = dat->start_tag + 1; 
	 *tagptr != '>' && *tagptr != ' '; tagptr++) {
      *buffend = *tagptr;
      buffend++;
      pathlen++;
    }
    sprintf(occur,"[%d]",dat->occurrence_no);
    strcat(buf, occur);
    pathlen += strlen(occur);

    return (pathlen); 
  }
}


idx_key *
buildxpathkey(char *element_name) 
{
  idx_key *k, *firstkey, *lastkey, *valkey;
  char *work, *start;

  work = strdup(element_name);
  /* sometimes tcl is too helpful...*/
  if (*work == '{')
    work++;

  start = strtok(work, "/[");
  k = firstkey = lastkey = NULL;

  do {
    if (strchr(start,']') != NULL) {
      /* an occurrence indicator */
      if (lastkey == NULL) {
	/* error -- occurrence without a tag */
	fprintf (LOGFILE,"Error in XPATH specification\n");
	diagnostic_set(227,0, "Error in XPATH specification for display -- Occurrence indicator with no tag?");
	return NULL;
      }
      lastkey->occurrence = atoi(start);
    }
    else {
      /* another step in the path */
      k = CALLOC(idx_key, 1);
      k->key = start;
      if (firstkey == NULL) {
	firstkey = k;
	lastkey = k;
      }
      else {
	lastkey->subkey = k;
	lastkey = k;
      }
    }
  } while (start = strtok(NULL, "/["));


  /* now step through and handle attribute specs */
  for (k = firstkey; k ; k = k->subkey) {
    if ((start = strchr(k->key,'@')) != NULL) {
      start++;
      k->key = start;
      k->attribute_flag = 1;
      if ((start = strchr(k->key,'=')) != NULL) {
	*start++ = '\0';
	while (*start == '"' || *start == '\'') 
	  start++;
	valkey = CALLOC(idx_key, 1);
	valkey->attribute_flag = 2;
	valkey->key = start;
	if (k->subkey != NULL)
	  valkey->subkey = k->subkey;
	k->subkey = valkey;
	/* terminate it */
        if ((work = strchr(start,'"')) || (work = strchr(start,'\''))) {
	  *work = '\0';
	}
      }
    } 
  }

  if ((start = strchr(lastkey->key,'}')) != NULL) {
    *start = '\0';
  }

  return(firstkey);

}


void
display_build_list(SGML_Document *doc, idx_key *key, cluster_map_entry *map,
		   int docid, SGML_Tag_Data *component_data, int type)
{

  SGML_Tag_Data *td, *tag_data;
  idx_key *k, *attrkey, *valuekey;
  int displayfirst = 0;
  int count_disp = 0;
  int casesensitive;
  struct data_list *dl;

  if (doc == NULL)
    return;

  if (key == NULL)
    return;

  attrkey = NULL;
  valuekey = NULL;

  if (doc->DTD->type > 0)
    casesensitive = 1;
  else
    casesensitive = 0;

  /* check for display directive to include only the first match */

  for (k = key; k ; k = k->subkey) {
    if (k->attribute_flag == 1) {
      /* An attribute may be several things -- right now we */
      /* check for the display directive to NOT include all */
      /* matching subtags that match, but only the first    */
      if ((strcasecmp("DISPLAY_FIRST_ONLY", k->key)) == 0) {
	displayfirst = 1;
	if (k->occurrence == 0) 
	  k->occurrence = 1;
      }
      else
	attrkey = k;
    } else if (k->attribute_flag == 2) {
      valuekey = k;
    }
  }


  tag_data = comp_tag_list(key, doc->Tag_hash_tab, component_data, 
			   casesensitive);

  if (type == MARC_CONV && last_data_list != NULL 
      && last_data_list->map == map && tag_data != NULL) {
    /* Special handling for marc mappings */
    td = tag_data;

    for (dl = first_map_data_list(map); dl ; dl = dl->next) {
      append_data_list(dl, td->item);
      if (td->next_tag_data)
	td = td->next_tag_data;
    }

  }
  else if (type == MARC_CONV && tag_data == NULL) 
    add_data_list(NULL,key,map,type);
  else {
    for (td = tag_data; td ; td = td->next_tag_data) {
      /* we have a match -- add it to the list */
      if (attrkey == NULL)
	add_data_list(td->item, key, map, type);
      else if (valuekey != NULL) 
	add_data_list(td->item, key, map, type);
      else /* we are converting the attribute value */
	add_data_attr(td->item,key, attrkey,map, type);

      count_disp++;
      if (map != NULL && map->sum_maxnum > 0 && count_disp == map->sum_maxnum) {
	/* we will be doing substitutions for some number of these tags later */
	displayfirst = 1;
      }

      if (displayfirst) break;
    }
  }
  /* free the tag data */
  free_tag_data_list(tag_data);
  
  return;
}



void
display_build_list_docid(idx_key *key, int docid, char *compid, 
			 cluster_map_entry *map)
{

  SGML_Data *newdata;
  char *dummy;
  int length;

  if (key == NULL)
    return;

  dummy = CALLOC(char, 200);
  if (docid != 0) {
    sprintf(dummy, "<DOCID>%d</DOCID>",docid);    
    length = strlen(dummy)-15;
  }
  else if (compid != NULL) {
    sprintf(dummy, "<DOCID>%s</DOCID>",compid);    
    length = strlen(compid);
  }
  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+7;
  newdata->content_end = dummy+7+length;
  newdata->end_tag = dummy+7+length;
  newdata->data_end = dummy+7+length+8;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 7;
  newdata->content_end_offset = 7+length;
  newdata->end_tag_offset = 7+length+1;
  newdata->data_end_offset = 7+length+8;
  
  add_data_list (newdata, key, map, 1);

  return;
}

void
display_build_list_filename(idx_key *key, int docid, SGML_Document *doc, 
			 cluster_map_entry *map)
{

  SGML_Data *newdata;
  char *dummy;
  int length;

  if (key == NULL)
    return;

  if (doc->file_name == NULL)
    return;

  length = strlen(doc->file_name);

  dummy = CALLOC(char, length+30);
  if (docid != 0) {
    sprintf(dummy, "<FILENAME>%s</FILENAME>",doc->file_name);    
  }

  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+11;
  newdata->content_end = dummy+11+length;
  newdata->end_tag = dummy+11+length;
  newdata->data_end = dummy+11+length+12;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 10;
  newdata->content_end_offset = 10+length;
  newdata->end_tag_offset = 10+length+1;
  newdata->data_end_offset = 10+length+11;
  
  add_data_list (newdata, key, map, 1);

  return;
}


void
display_build_list_xpath(idx_key *key, int docid, SGML_Data *data, 
			 cluster_map_entry *map)
{

  SGML_Data *newdata;
  char *dummy;
  int length;

  length = Xpathlen(data);

  dummy = CALLOC(char, length+20);

  if (length == 0) {
    sprintf(dummy, "<XPATH>//</XPATH>");    
  }
  else {
    sprintf(dummy, "<XPATH>");
    length = Xpathcpy(data, dummy);
    strcat(dummy, "</XPATH>");
  }
 
  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+7;
  newdata->content_end = dummy+7+length;
  newdata->end_tag = dummy+7+length;
  newdata->data_end = dummy+7+length+8;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 6;
  newdata->content_end_offset = 6+length;
  newdata->end_tag_offset = 6+length+1;
  newdata->data_end_offset = 6+length+7;
  
  add_data_list (newdata, key, map, 1);

  return;
}



void
display_build_list_dbname(idx_key *key, int docid, char *dbname, 
			  cluster_map_entry *map)
{

  SGML_Data *newdata;
  char *dummy;
  int length;
  int sysreturn;
  char hostname[500];
  struct hostent *hostinfo;

  if (key == NULL)
    return;

  if (dbname == NULL)
    return;

  gethostname(hostname, 500);
  hostinfo = gethostbyname(hostname);


  /* server machine name.     
   * session->s_serverMachineName = strdhostinfo->h_name);
   * ip address in dot format   
   * in_ad.s_addr = *(u_int *)hostinfo->h_addr_list[0];
   * session->s_serverIP = strdup(inet_ntoa(in_ad));		
   */

  length = strlen(dbname) + strlen(hostinfo->h_name) + 11;

  dummy = CALLOC(char, length+20);
  if (docid != 0) {
    sprintf(dummy, "<DBNAME>z39.50s://%s/%s</DBNAME>",hostinfo->h_name, dbname);    
  }

  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+8;
  newdata->content_end = dummy+8+length;
  newdata->end_tag = dummy+8+length;
  newdata->data_end = dummy+8+length+9;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 8;
  newdata->content_end_offset = 8+length;
  newdata->end_tag_offset = 8+length+1;
  newdata->data_end_offset = 8+length+9;
  
  add_data_list (newdata, key, map, 1);

  return;
}



void
display_build_list_parent_docid(idx_key *key, int docid, 
			 cluster_map_entry *map)
{

  SGML_Data *newdata;
  char *dummy;
  int length;

  if (key == NULL)
    return;

  dummy = CALLOC(char, 200);
  if (docid != 0) {
    sprintf(dummy, "<PARENT-DOCID>%d</PARENT-DOCID>",docid);    
    length = strlen(dummy)-15;
  }
  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+14;
  newdata->content_end = dummy+14+length;
  newdata->end_tag = dummy+14+length;
  newdata->data_end = dummy+14+length+15;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 14;
  newdata->content_end_offset = 14+length;
  newdata->end_tag_offset = 14+length+1;
  newdata->data_end_offset = 14+length+15;
  
  add_data_list (newdata, key, map, 1);

  return;
}



void
display_build_list_rank(idx_key *key, int rank, 
			 cluster_map_entry *map)
{

  SGML_Data *newdata;

  char *dummy;
  int length;

  if (key == NULL)
    return;

  dummy = CALLOC(char, 30);
  sprintf(dummy, "<RANK>%d</RANK>",rank);    
  length = strlen(dummy)-13 ;

  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+6;
  newdata->content_end = dummy+6+length;
  newdata->end_tag = dummy+6+length;
  newdata->data_end = dummy+6+length+7;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 6;
  newdata->content_end_offset = 6+length;
  newdata->end_tag_offset = 6+length+1;
  newdata->data_end_offset = 7+length+7;
  
  add_data_list (newdata, key, map, 1);

  return;
}



void
display_build_list_relevance(idx_key *key, int relevance, float raw_relevance,
			 cluster_map_entry *map, int which)
{

  SGML_Data *newdata;

  char *dummy;
  int length;
  int taglen;

  if (key == NULL)
    return;

  dummy = CALLOC(char, 50);

  if (map != NULL && map->to != NULL && map->to->key != NULL &&
      atoi(map->to->key) == 0) { /* NOT a numeric key... indicates GRS */
    if (which == 1) 
      sprintf(dummy, "<%s>%f</%s>",map->to->key,raw_relevance,map->to->key); 
    else
      sprintf(dummy, "<%s>%d</%s>",map->to->key,relevance,map->to->key); 
    taglen = strlen(map->to->key);
    length = strlen(dummy)-(5+(2*taglen));
    newdata = CALLOC(SGML_Data, 1);
    /* we now create a DUMMY ENTRY */
    newdata->start_tag = dummy;
    newdata->content_start = dummy+taglen+2;
    newdata->content_end = dummy+taglen+2+length;
    newdata->end_tag = dummy+taglen+2+length;
    newdata->data_end = dummy+taglen+2+length+taglen+3;
    newdata->start_tag_offset = 0;
    newdata->content_start_offset = taglen+2;
    newdata->content_end_offset = taglen+2+length;
    newdata->end_tag_offset = taglen+2+length+1;
    newdata->data_end_offset = taglen+2+length+taglen+3;
  }
  else {
    if (which == 1) 
      sprintf(dummy, "<RELEVANCE>%f</RELEVANCE>",raw_relevance); 
    else
      sprintf(dummy, "<RELEVANCE>%d</RELEVANCE>",relevance); 
    length = strlen(dummy)-23;
    newdata = CALLOC(SGML_Data, 1);
    /* we now create a DUMMY ENTRY */
    newdata->start_tag = dummy;
    newdata->content_start = dummy+11;
    newdata->content_end = dummy+11+length;
    newdata->end_tag = dummy+11+length;
    newdata->data_end = dummy+11+length+12;
    newdata->start_tag_offset = 0;
    newdata->content_start_offset = 11;
    newdata->content_end_offset = 11+length;
    newdata->end_tag_offset = 11+length+1;
    newdata->data_end_offset = 11+length+12;
  }

  add_data_list (newdata, key, map, 1);

  return;
}


void
display_build_list_MARC_HEADER(idx_key *key, cluster_map_entry *map)
{

  SGML_Data *newdata;

  char *dummy;
  int length;

  if (key == NULL)
    return;

  dummy = CALLOC(char, 15);
  sprintf(dummy, "<tmp> </tmp>"); 

  newdata = CALLOC(SGML_Data, 1);
  /* we now create a DUMMY ENTRY */
  newdata->start_tag = dummy;
  newdata->content_start = dummy+5;
  newdata->content_end = dummy+6;
  newdata->end_tag = dummy+6;
  newdata->data_end = dummy+12;
  newdata->start_tag_offset = 0;
  newdata->content_start_offset = 5;
  newdata->content_end_offset = 6;
  newdata->end_tag_offset = 6;
  newdata->data_end_offset = 12;
  
  add_data_list (newdata, key, map, 1);

  return;
}




void
comp_display_build_list_parent(SGML_Document *doc, idx_key *key, 
			       cluster_map_entry *map)
{

  SGML_Tag_Data *tag_data;
  SGML_Data *newdata;

  char *dummy = NULL, *tmp = NULL, *c;
  char *data;
  char *tagname;
  int casesensitive;
  int length, datalength, taglen;

  if (key == NULL)
    return;
  if (doc == NULL)
    return;

  
  if (doc->DTD->type > 0)
    casesensitive = 1;
  else
    casesensitive = 0;

  /* special case of getting parent record's DOCID */
  if (strcasecmp(key->key,"#DOCID#") == 0)
    display_build_list_parent_docid(key, doc->record_id, map);

  else {

    tag_data = comp_tag_list(key, doc->Tag_hash_tab, NULL, casesensitive);

    if (tag_data == NULL)
      return;

    /* we only take the FIRST item */
    length = tag_data->item->data_end_offset - tag_data->item->start_tag_offset
      + 50;
    
    taglen =  (tag_data->item->content_start-1) - (tag_data->item->start_tag+1);
    tagname = CALLOC(char, taglen+1);

    strncpy(tagname,tag_data->item->start_tag+1, taglen);
    
    tagname[taglen] = '\0';

    data = detag_data_block(tag_data->item, NORM_NOMAP, NULL);

    if (data == NULL)
      return;

    datalength = strlen(data);
    
    dummy = CALLOC(char, length);
    
    sprintf(dummy,"<PARENT-%s>%s", tagname, data);

    for (c = tagname; *c ; c++) {
      if (*c == ' ' || *c == '\t' || *c == '\n') {
	*c = '\0';
	break;
      }
    }

    tmp = CALLOC(char, strlen(tagname) + 15);

    sprintf(tmp,"</PARENT-%s>", tagname);
    strcat(dummy, tmp);

    newdata = CALLOC(SGML_Data, 1);
    /* we now create a DUMMY ENTRY */
    newdata->start_tag = dummy;
    newdata->content_start = strchr(dummy,'>') + 1;
    newdata->content_end = newdata->content_start + datalength;
    newdata->end_tag = newdata->content_start + datalength;
    newdata->data_end = strchr(newdata->content_end,'>')+1;
    newdata->start_tag_offset = 0;
    newdata->content_start_offset = newdata->content_start - dummy;
    newdata->content_end_offset = newdata->content_end - dummy;
    newdata->end_tag_offset = newdata->end_tag - dummy;
    newdata->data_end_offset = newdata->data_end - dummy;
  
    add_data_list (newdata, key, map, 1);

    if (tmp) FREE(tmp);
    if (tagname) FREE(tagname);
    if (data) FREE(data);
  }
  return;
}



void
display_build_list_keys(SGML_Document *doc, idx_key *key, 
			cluster_map_entry *map, int docid, int type)
{

  idx_key *k;

  /* Check multiple keys... for this data element */
  for (k = key; k != NULL; k = k->next_key)
    display_build_list(doc, k, map, docid, NULL, type);

  return;
}


/* build the display for XML elements... */
void
display_build_list_element(SGML_Document *doc, char *elementname, 
			   cluster_map_entry *map, int docid,
			   int type, SGML_Tag_Data *tag_data)
{

  idx_key *k, *first=NULL, *last=NULL ;


  if (strchr(elementname, '/') != NULL || strchr(elementname, '[') != NULL) {
    if (strchr(elementname, '|') == NULL) {
      first = k = buildxpathkey(elementname);
    } 
    else {
      char *temp, *bar, *start;

      temp = start = strdup(elementname);
      while (bar = strchr(start, '|')) {
	*bar = '\0';
	if (first == NULL) {
	  first = k = buildxpathkey(start);
	  last = k;
	}
	else
	  last->next_key = buildxpathkey(start);
	start = bar + 1;
      }
      
      last->next_key = buildxpathkey(start);
      FREE(temp);
    }
  }
  else {
    first = k = CALLOC(idx_key, 1);
    k->key = elementname;
  }

  for (k = first; k != NULL; k = k->next_key)
    display_build_list(doc, k, map, docid, tag_data, type);

  return;
}


void
display_build_list_element_flags(SGML_Document *doc, char *elementname, 
				 cluster_map_entry *map, int docid,
				 int type, int **kw_offset_table, 
				 SGML_Tag_Data *tag_data)
{

  idx_key *k, *first=NULL, *last=NULL ;


  if (strchr(elementname, '/') != NULL || strchr(elementname, '[') != NULL) {
    if (strchr(elementname, '|') == NULL) {
      first = k = buildxpathkey(elementname);
    } 
    else {
      char *temp, *bar, *start;

      temp = start = strdup(elementname);
      while (bar = strchr(start, '|')) {
	*bar = '\0';
	if (first == NULL) {
	  first = k = buildxpathkey(start);
	  last = k;
	}
	else
	  last->next_key = buildxpathkey(start);
	start = bar + 1;
      }
      
      last->next_key = buildxpathkey(start);
      FREE(temp);
    }
  }
  else {
    first = k = CALLOC(idx_key, 1);
    k->key = elementname;
  }

  for (k = first; k != NULL; k = k->next_key)
    display_build_list(doc, k, map, docid, tag_data, type);

  return;
}



void
comp_display_build_list_keys(SGML_Document *doc, SGML_Tag_Data *tag_data, 
			     idx_key *key, cluster_map_entry *map, int type)
{

  idx_key *k;

  /* Check multiple keys... for this data element */
  for (k = key; k != NULL; k = k->next_key)
    display_build_list(doc, k, map, 0, tag_data, type);

  return;
}

void
display_build_list_map(SGML_Document *doc, cluster_map_entry *map, 
		       int docid, int rank, int relevance, 
		       float raw_relevance, char *conv_name, 
		       char *format, char *filename)
{

  struct data_list *last=NULL, *new_var = NULL;
  idx_key *key, *tokey; 
  int type;
  char *filetag;
  char *element;


  if (doc == NULL)
    return;

  if (map == NULL)
    return;

  key = map->from;
  tokey = map->to;

  if (key == NULL)
    return;

  last = current_data_list;

  if (conv_name != NULL && (strcasecmp(conv_name, "TAGSET-G") == 0)) {
    type = TAGSET_G;
  }
  else if (conv_name != NULL && (strcasecmp(conv_name, "MARC") == 0)) {
    type = MARC_CONV;
  }
  else 
    type = 0;

  if (strcasecmp(key->key,"#DOCID#") == 0 ||
      strcasecmp(key->key,"#COMPONENTID") == 0 ||
      strcasecmp(key->key,"#COMPID") == 0 )
    display_build_list_docid(key, docid, NULL, map);
  else if (strcasecmp(key->key,"#RANK#") == 0)
    display_build_list_rank(key, rank, map);
  else if (strcasecmp(key->key,"#FILENAME#") == 0)
    display_build_list_filename(key, docid, doc, map);
  else if (strcasecmp(key->key,"#XPATH#") == 0)
    display_build_list_xpath(key, docid, doc->data, map);
  else if (strcasecmp(key->key,"#DBNAME#") == 0) {
    config_file_info *cf;
    cf = find_file_config(filename);
    filetag = cf->nickname;
    display_build_list_dbname(key, docid, filetag, map);
  }
  else if (strcasecmp(key->key,"#SCORE#") == 0 
	   || strcasecmp(key->key,"#RELEVANCE#") == 0)
    display_build_list_relevance(key, relevance, raw_relevance, map, 0);
  else if (strcasecmp(key->key,"#RAWSCORE#") == 0 
	   || strcasecmp(key->key,"#RAWRELEVANCE#") == 0)
    display_build_list_relevance(key, relevance, raw_relevance, map, 1);
  else if (strcasecmp(key->key, "SUBST_ELEMENT") == 0) {
    element = format + 12;
    display_build_list_element(doc,element,map,docid, type, NULL);
  }
  else if (strcasecmp(key->key, "#FLAG_KEYWORDS#") == 0) {
    element = format + 12;
    display_build_list_element_flags(doc,element,map, docid, type, kw_offset_table, NULL);
  }
  else if (strcasecmp("MARC_HEADER_MATERIAL_TYPE", key->key) == 0
	   || strcasecmp("MARC_HEADER_RECSTAT", key->key) == 0
	   || strcasecmp("MARC_HEADER_RECTYPE", key->key) == 0
	   || strcasecmp("MARC_HEADER_BIBLEVEL", key->key) == 0) 
	display_build_list_MARC_HEADER(key, map);
  else if (strcasecmp("#COMMENTS#", tokey->key)) {
    
  }
  else
    display_build_list_keys(doc, key, map, docid, type);


  if (last != NULL) {
    new_var = last->next;
    if (new_var != NULL && new_var != current_data_list 
	&& new_var->map == current_data_list->map) 
      new_var->groupend = current_data_list;
  }

  display_build_list_map(doc, map->next_clusmap, docid, rank, relevance,
			 raw_relevance, conv_name, format, filename);
  
}


void
comp_display_build_list_map(SGML_Document *doc, SGML_Tag_Data *tag_data, 
			    cluster_map_entry *map, char *compid,
			    int rank, int relevance, float raw_relevance,
			    char *conv_name, char *format, char *filename,
			    int docid)
{

  struct data_list *last=NULL, *new_var = NULL;
  idx_key *key, *tokey; 
  char *element;
  int type;

  if (doc == NULL)
    return;

  if (map == NULL)
    return;

  key = map->from;
  tokey = map->to;

  if (key == NULL)
    return;

  last = current_data_list;

  if (conv_name != NULL && (strcasecmp(conv_name, "TAGSET-G") == 0)){
    type = TAGSET_G;
  }
  else 
    type = 0;



  if (strcasecmp(key->key,"#DOCID#") == 0 ||
      strcasecmp(key->key,"#COMPONENTID") == 0 ||
      strcasecmp(key->key,"#COMPID") == 0 )
    display_build_list_docid(key, 0, compid, map);
  else if (strcasecmp(key->key,"#RANK#") == 0)
    display_build_list_rank(key, rank, map);
  else if (strcasecmp(key->key,"#FILENAME#") == 0)
    display_build_list_filename(key, docid, doc, map);
  else if (strcasecmp(key->key,"#XPATH#") == 0)
    display_build_list_xpath(key, docid, tag_data->item, map);
  else if (strcasecmp(key->key,"#DBNAME#") == 0) {
    config_file_info *cf;
    cf = find_file_config(filename);
    display_build_list_dbname(key, docid,cf->nickname, map);
  }
  else if (strcasecmp(key->key,"#SCORE#") == 0 
	   || strcasecmp(key->key,"#RELEVANCE#") == 0)
    display_build_list_relevance(key, relevance, raw_relevance, map, 0);
  else if (strcasecmp(key->key,"#RAWSCORE#") == 0 
	   || strcasecmp(key->key,"#RAWRELEVANCE#") == 0)
    display_build_list_relevance(key, relevance, raw_relevance, map, 1);
  else if (strncasecmp(key->key,"#PARENT#",8) == 0) {
    /* The subkey should be the REAL key */
    comp_display_build_list_parent(doc, key->subkey, map);
  }
  else if (strcasecmp(key->key, "SUBST_ELEMENT") == 0) {
    element = format + 12;
    display_build_list_element(doc,element,map, docid, type, tag_data);
  }
  else if (strcasecmp(key->key, "#FLAG_KEYWORDS#") == 0) {
    element = format + 12;
    display_build_list_element_flags(doc,element,map, docid, type, kw_offset_table, tag_data);
  }
  else if (strcasecmp("#COMMENTS#", tokey->key)) {
    
  }
  else
    comp_display_build_list_keys(doc, tag_data, key, map, type);


  if (last != NULL) {
    new_var = last->next;
    if (new_var != NULL && new_var != current_data_list 
	&& new_var->map == current_data_list->map) 
      new_var->groupend = current_data_list;
  }

  comp_display_build_list_map(doc, tag_data, map->next_clusmap, compid,
			      rank, relevance, raw_relevance, conv_name, 
			      format, filename, docid);
  
}


int 
is_required_sgml_element(char *name, SGML_Sub_Element_List *data_model)
{
  SGML_Sub_Element_List *dm;
  int result = 1;

  if (data_model == NULL)
    return(1); /* assume it isn't required (bogus assumption, but...) */
  
  
  
  for (dm = data_model; dm; dm = dm->next_sub_element) {
    if (dm->group != NULL) {
      result = is_required_sgml_element(name, data_model->group);
      if (dm->group_repetition != NULL &&
	  (*dm->group_repetition == '?' || *dm->group_repetition == '*'))
	result = 0;
      return(result);
    }
    else { /* a regular element */
      if (strcmp (name, dm->element_name) == 0) {
	/* found the matching datamodel element */
	if (dm->repetition != NULL && 
	    (*dm->repetition == '?' || *dm->repetition == '*'))
	  result = 0;
      
      
	if (dm->connector != NULL && *dm->connector == '|')
	  result = 0;
	return(result);  
      }
    }
  }
  return (result);
}

int 
verify_display_list (struct data_list *list)
{
  struct data_list *dl;
  for (dl = list; dl; dl = dl->next) {
    if (dl->item->parent != NULL) { /* it is a subtag of something */
      dl->required = 
	is_required_sgml_element(dl->item->element->tag,
				 dl->item->parent->element->data_model);
    }
    else /* main tag is always required */
      dl->required = 1;
  }  
  return 0;
}

char *
sgml_squash(SGML_Data *indata, int last)
{
  char *work;
  char *tmp, *tmp2;

  int templen;
  int datalen;

  if (indata == NULL || last < indata->data_end_offset) {
    tmp = CALLOC(char, 1);
    return (tmp);
  }

  if (is_required_sgml_element(indata->element->tag,
			       indata->parent->element->data_model)) {

    /* it is required ... is elipsis going to be more that the data? */
    datalen = indata->content_end_offset - indata->content_start_offset;
    templen = indata->data_end_offset - indata->start_tag_offset;

    if (datalen < 5) {
      work = CALLOC(char, templen+1);
      /* it can't have any subelements because even a single char tag would */
      /* be datalen > 7 or an empty element */
      strncpy(work, indata->start_tag, templen);
    }
    else {
      work = CALLOC(char, templen+50);
      strncpy(work, indata->start_tag, 
	      indata->content_start_offset - indata->start_tag_offset);

      strcat(work," ... ");
      tmp = sgml_squash(indata->sub_data_element, last);
      if (templen < (int)(strlen(work) + strlen(tmp) + 5 + 
		     (indata->data_end_offset - indata->end_tag_offset))) {
	templen = (int)(strlen(work) + strlen(tmp) +
		   (indata->data_end_offset - indata->end_tag_offset)
		   + 10 );
	tmp2 = CALLOC(char, templen);
	strcpy(tmp2, work);
	strcat(tmp2, tmp);
	FREE(tmp);
	tmp = work;
	work = tmp2;
	FREE(tmp);
      }
      else
	strcat(work,tmp);

      strncat(work, indata->end_tag, 
	      indata->data_end_offset - indata->end_tag_offset);
    }
    /* do the next element -- */
    tmp = sgml_squash(indata->next_data_element, last);

    if (templen < (int)(strlen(work) + strlen(tmp) + 1)) {
      
      templen = (strlen(work) + strlen(tmp) + 1);
      tmp2 = CALLOC(char, templen);
      strcpy(tmp2, work);
      strcat(tmp2, tmp);
      FREE(tmp);
      tmp = work;
      work = tmp2;
      FREE(tmp);
      return(work);
    } 
    else
      return(strcat(work,tmp));

  }
  else { /* not required element */
    return(sgml_squash(indata->next_data_element, last));
  }
  
}

int 
display_exclude(SGML_Document *doc, char **buf, long maxlen,
		struct data_list *exclude, int compress, char *addtags)
{
  struct data_list *dl;
  int result_recsize, last_len;
  char *buf_end, *last_data_start;

  verify_display_list(exclude);

  if (exclude == NULL) {
    /* nothing to exclude, so copy the whole record */
    result_recsize = strlen(doc->buffer);
    if (result_recsize < maxlen)
      *buf = strdup(doc->buffer);
    else
      result_recsize = -1; /* error code for rec too long */

    return (result_recsize);
  }
  
  /* put everything up to the first exclusion into the buffer */
  last_data_start = doc->buffer;
  *buf = CALLOC(char, strlen(doc->buffer)+1);
  buf_end = *buf;
  *buf_end = '\0';

  
  for (dl = exclude; dl; dl = dl->next) {
    last_len = dl->item->start_tag - last_data_start;
    if (dl->required == 0) {
      if (last_len == 1 && 
	  (*last_data_start == ' '
	   || *last_data_start == '\n'
	   || *last_data_start == '\t')) {
	/* don't copy irrelevant white space */
	last_data_start = dl->item->data_end;
      }
      else {
	if (last_len > 0) { /* out of sequence?  need to sort these ...*/
	  strncat(buf_end, last_data_start, last_len);
	  last_data_start = dl->item->data_end;
	}
	else if (last_len == 0)
	  last_data_start = dl->item->data_end;
      }
    }
    else { /* exclude tag is required by DTD */
      if (compress) {
	/* squash the contents as much as possible */
	/* copy the tag itself */
	strncat(buf_end, last_data_start, last_len);
	strncat(buf_end, dl->item->start_tag, 
	       dl->item->content_start_offset - dl->item->start_tag_offset);
	strcat(buf_end, "...ELEMENT DATA DELETED...");
        strcat(buf_end, sgml_squash(dl->item->sub_data_element, 
				    dl->item->end_tag_offset));
	strncat(buf_end, dl->item->end_tag, 
		dl->item->data_end_offset - dl->item->end_tag_offset);
	last_data_start = dl->item->data_end;
	       
      }
    }
    /* don't leave whitespace at the beginning of a tag */
    while (*last_data_start == ' '
	|| *last_data_start == '\n'
	|| *last_data_start == '\t')
      last_data_start++;
  }
  strcat(buf_end, last_data_start);
  
  return (strlen(*buf));
}

ElementMetaData *
MakeAttrMetaData(SGML_Attribute_Data *attr)
{

  ElementMetaData *new_var;
  char attmsg[200];

  if (attr == NULL)
    return NULL;

  new_var = CALLOC(ElementMetaData,1);

  if (new_var == NULL)
    return NULL;
 
  if (attr->attr_def != NULL && attr->attr_def->assoc_tag != NULL)
    sprintf(attmsg,"Attribute %s of tag %s", attr->name,
	    attr->attr_def->assoc_tag);
  else
    sprintf(attmsg,"Attribute %s of (multiple tags)", attr->name);
  
  new_var->displayName = NewInternationalString(attmsg);
  new_var->message = NewInternationalString("Attribute");
  
  return(new_var);
}


TaggedElement *
MakeAttrTaggedElement(SGML_Attribute_Data *attr)
{
  ElementData *newED;
  TaggedElement *new_var;

  if (attr == NULL)
    return NULL;

  new_var = CALLOC(TaggedElement,1);

  if (new_var == NULL)
    return NULL;
 

  new_var->tagType = 3;

  new_var->tagValue = MakeStringOrNumeric(attr->name, 0, 0);
  new_var->tagOccurrence = 0;
  new_var->metaData = MakeAttrMetaData(attr);
  new_var->appliedVariant = NULL; /* we don't do variants now */;

  newED = CALLOC(ElementData,1);

  if (newED == NULL)
    return NULL;
 
  if (attr->string_val == NULL) { /* numeric value */
    newED->which = e46_numeric;
    newED->u.numeric = attr->int_val;
  }
  else { /* string attribute value */
    newED->which = e46_string;
    newED->u.string = NewInternationalString(attr->string_val);
  }

  new_var->content = newED;

  return (new_var);
}

struct subtree_List91 *
MakeAttrSubtree(SGML_Attribute_Data *attr)
{
  struct subtree_List91 *new_var;

  if (attr == NULL)
    return NULL;

  new_var = CALLOC(struct subtree_List91, 1);
  new_var->next = MakeAttrSubtree(attr->next_attribute_data);
  new_var->item = MakeAttrTaggedElement(attr);
  return (new_var);
}

struct subtree_List91 *
MakeElementSubtree(SGML_Data *data)
{
  struct subtree_List91 *new_var;

  if (data == NULL)
    return NULL;

  new_var = CALLOC(struct subtree_List91, 1);
  new_var->next = MakeElementSubtree(data->next_data_element);
  new_var->item = MakeTaggedElement(data, 0, 0, NULL);
  return (new_var);
}

struct subtree_List91 *

MakeOneElementSubtree(SGML_Data *data)
{
  struct subtree_List91 *new_var;

  if (data == NULL)
    return NULL;

  new_var = CALLOC(struct subtree_List91, 1);
  new_var->next = NULL;
  new_var->item = MakeTaggedElement(data, 0, 0, NULL);
  return (new_var);
}

int
GetDataLength(SGML_Data *data, char *start)
{
  SGML_Data *d;
  for (d = data; d; d = d->next_data_element) {
    if (start >= d->content_end)
      continue;
    else
      return (d->start_tag - start);
  } 
  return 0; /* end of enclosing tag with no new_var subtags */
}

TaggedElement *
MakeTaggedData(SGML_Data *data, int *start)
{
  TaggedElement *new_var;
  char *tmp;
  int diff, datlength;

  if (data == NULL)
    return NULL;

  new_var = CALLOC(TaggedElement,1);

  if (new_var == NULL)
    return NULL;

  new_var->tagType = 2;
  new_var->tagValue = MakeStringOrNumeric(NULL, 19, 1);
  new_var->tagOccurrence = 0;
  new_var->content = CALLOC(ElementData,1);
  new_var->content->which = e46_octets;

  if (*start == 0) {
    *start = data->content_start_offset;
    tmp = data->content_start;
    if (data->sub_data_element == NULL)
      datlength = data->content_end_offset - data->content_start_offset;
    else
      datlength = data->sub_data_element->start_tag_offset - *start;
  }
  else { 
    diff = *start - data->start_tag_offset;
    tmp = data->start_tag + diff;
    datlength = GetDataLength(data->sub_data_element, tmp);
  }

  if (datlength < 0) {
    fprintf(LOGFILE, "GRS-1 Conversion: Negative offsets in data extraction\n");
    FREE (new_var->content);
    FREE (new_var);
    return (NULL);
  }
  /* if this is the last element before end tag, datlength is 0 */
  if (datlength == 0) {
    if (*tmp != '<')
      datlength = data->end_tag - tmp;
    else {
      if (*(tmp+1) == '?') /* processing instructions... */
	datlength = data->end_tag - tmp;
      else {
	/* Another tag starting... Not DATA (happens with EMPTY TAGS */
	FREE (new_var->content);
	FREE (new_var);
	return (NULL);
      }
    }
  }
  
  new_var->content->u.octets = NewOctetStringN(tmp, datlength);

  new_var->metaData = NULL; /* MakeElementMetaData(data); */
  new_var->appliedVariant = NULL; /* we don't do variants now */;
  *start += datlength;

  return(new_var);
}


struct subtree_List91 *
MakeDataSubtree(SGML_Data *data, int *start)
{
  struct subtree_List91 *new_var;
  new_var = CALLOC(struct subtree_List91, 1);
  /* handle only leading text here */
  new_var->next = NULL;  
  new_var->item = MakeTaggedData(data, start);
  if (new_var->item == NULL) {
    FREE(new_var);
    return(NULL);
  }

  return (new_var);
}

SGML_Data *
GetNextSubData(int *start, SGML_Data **first) 
{
  SGML_Data *dat;

  for (dat = *first; dat; dat = dat->next_data_element) {
    if (*start > dat->content_end_offset)
      continue;
    else {
      *start = dat->data_end_offset;
      if (dat->next_data_element)
	*first = dat->next_data_element;
      return (dat);

    }
  }
  return NULL;
} 

int
SubElementSpan (SGML_Data *data) {
  
  int Main_start, Main_end;
  int last_end;
  char *last_ptr, *p;
  SGML_Data *d;

  /* check the simple cases */
  if (data->sub_data_element == NULL)
    return 0;

  Main_start = data->content_start_offset;
  Main_end = data->content_end_offset;

  if (Main_start == data->sub_data_element->start_tag_offset 
      && Main_end == data->sub_data_element->data_end_offset)
    return 1;

  /* otherwise we check */
  last_end = Main_start;
  last_ptr = data->content_start;

  for (d = data->sub_data_element; d ; d = d->next_data_element) {
    if (last_end == d->start_tag_offset) {
      last_end = d->data_end_offset;
      last_ptr = d->data_end;
      continue;
    } 
    else {
      /* Check for non-white_space between subelements */
      for (p = last_ptr; p < d->start_tag; p++) {
	if (isspace((int)*p))
	  continue;
	else
	  return (0);
      }
      /* if we get to here there is only white space -- so continue */
      last_end = d->data_end_offset;
      last_ptr = d->data_end;
      
    }
  }
  
  /* If we get to here, the subelements span the entire main space  */
  /* with the possible exception of TRAILING PCDATA                 */
  for (p = last_ptr; p < data->content_end; p++) {
    if (isspace((int)*p))
      continue;
    else
      return (0);
  }

  /* it spans ... */
  return (1);

}

ElementData *
MakeElementData(SGML_Data *data, int flatten, int *typeflag)
{
  ElementData *new_var;
  struct subtree_List91 *lastl, *newl, *inl;
  char *stringdata;
  int startdata = 0;
  SGML_Data *subdata;
  SGML_Data *first_subdata;
  static int tagcount = 0;
  if (data == NULL)
    return NULL;

  new_var = CALLOC(ElementData,1);

  if (new_var == NULL)
    return NULL;
 
  *typeflag = 0;

  if (data->attributes && flatten == 0) {
   
    new_var->which = e46_subtree;
    new_var->u.subtree = MakeAttrSubtree(data->attributes);
      
  }
  
  if (data->sub_data_element && flatten == 0) {
   
    new_var->which = e46_subtree;
    newl = NULL;

    if (data->element->data_type == CONTENT_MODEL) {
      newl = MakeElementSubtree(data->sub_data_element);
      if (new_var->u.subtree != NULL) {
	for (inl = new_var->u.subtree; inl; inl = inl->next)
	  lastl = inl;
	lastl->next = newl;
      }
      else
	new_var->u.subtree = newl;
    } 
    else if (data->element->data_type == EL_MIXED) {
      /* It claims to be mixed, but if there is a single subelement*/
      /* filling the content, or if the entire content is spanned  */
      /* by subelements (and whitespace) we will treat it like content model*/
      if (SubElementSpan(data)) {
	newl = MakeElementSubtree(data->sub_data_element);
	if (new_var->u.subtree != NULL) {
	  for (inl = new_var->u.subtree; inl; inl = inl->next)
	    lastl = inl;
	  lastl->next = newl;
	}
	else
	  new_var->u.subtree = newl;
      } 
      else {
	first_subdata = data->sub_data_element;
	/* handle the case where the subelement is the first thing */
	if (data->content_start_offset == 
	    data->sub_data_element->start_tag_offset) {
	  
	  subdata = GetNextSubData(&startdata, &first_subdata); 
	  if (subdata) {
	    newl = MakeOneElementSubtree(subdata);
	    if (new_var->u.subtree != NULL) {
	      for (inl = new_var->u.subtree; inl; inl = inl->next)
		lastl = inl;
	      lastl->next = newl;
	    }
	    else
	      new_var->u.subtree = newl;
	  }
	 
	}
	while (startdata < data->content_end_offset) {
	
	  newl = MakeDataSubtree(data, &startdata);
	  if (newl != NULL) {
	    if (new_var->u.subtree != NULL) {
	      for (inl = new_var->u.subtree; inl; inl = inl->next)
		lastl = inl;
	      lastl->next = newl;
	    }
	    else
	      new_var->u.subtree = newl;
	  }
	  
	  subdata = GetNextSubData(&startdata, &first_subdata); 
	  if (subdata != NULL && subdata->element->data_type == EL_EMPTY) {
	    /* handle SGML EMPTY tags */
	    newl = CALLOC(struct subtree_List91, 1);
	    if (newl != NULL) {
	      newl->next = NULL;  
	      newl->item = CALLOC(TaggedElement,1);
	      /* newl->item->tagType = 2; */
	      newl->item->tagType = 3;
	      newl->item->tagValue = MakeStringOrNumeric(subdata->element->tag, 0, 0);
	      /* newl->item->tagValue = MakeStringOrNumeric(NULL, 19, 1); */
	      newl->item->tagOccurrence = 0;
	      newl->item->content = CALLOC(ElementData,1);
	      newl->item->content->which = e46_elementEmpty;
	      newl->item->content->u.elementEmpty = (char)1;
	      startdata = subdata->end_tag_offset+1;
	      
	      if (new_var->u.subtree != NULL) {
		for (inl = new_var->u.subtree; inl; inl = inl->next)
		  lastl = inl;
		lastl->next = newl;
	      }
	      else
		new_var->u.subtree = newl;
	    }
	  } 
	  else if (subdata) {
	    newl = MakeOneElementSubtree(subdata);
	    if (new_var->u.subtree != NULL) {
	      for (inl = new_var->u.subtree; inl; inl = inl->next)
		lastl = inl;
	      lastl->next = newl;
	    }
	    else
	      new_var->u.subtree = newl;
	    
	    newl = NULL;
	  }
	  else
	    startdata = data->data_end_offset;
	}
      }
    }
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<DOCID>", 7) == 0) {
    /* no longer numeric only... */
    new_var->which = e46_string;
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<RANK>", 6) == 0) {
    new_var->which = e46_numeric;
    *typeflag = 10; /* the 'RANK' tag number */

    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.numeric = atoi(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<RELEVANCE>", 11) == 0) {
    new_var->which = e46_string;
    *typeflag = 18; /* the 'SCORE' tag number */
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<FILENAME>", 10) == 0) {
    new_var->which = e46_string;
    *typeflag = 35; /* the 'PRIVATE' tag number */
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<XPATH>", 7) == 0) {
    new_var->which = e46_string;
    *typeflag = 35; /* the 'PRIVATE' tag number */
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<DBNAME>", 8) == 0) {
    new_var->which = e46_string;
    *typeflag = 36; /* the 'databaseName' tag number */
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<PARENT-", 8) == 0) {
    new_var->which = e46_string;
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL 
	   && strncmp(data->start_tag, "<ATTR>", 6) == 0) {
    new_var->which = e46_string;
    stringdata = data->content_start; /* turn off char mapping */
    *(data->end_tag) = '\0';
    new_var->u.string = NewInternationalString(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data); 
  }
  else if (data->element == NULL) {
    /* OK including arbitrary tags for output mapping of internal data has */
    /* made this clause necessary --  */
    new_var->which = e46_string;
    *typeflag = 35; /* the 'PRIVATE' tag number */
    stringdata = detag_data_block(data, NORM_NOMAP, NULL); /* turn off char mapping */
    new_var->u.string = NewInternationalString(stringdata);
    FREE(stringdata);
    /* free the dummy data element here */
    FREE(data->start_tag);
    FREE(data);     
  }
  else if (data->element != NULL && data->element->data_type == EL_EMPTY) {
    /* handle SGML EMPTY tags */
    if (new_var->which == e46_subtree) {
      newl = CALLOC(struct subtree_List91, 1);

      newl->next = NULL;  
      newl->item = CALLOC(TaggedElement,1);
      /* newl->item->tagType = 2; */
      newl->item->tagType = 3;
      newl->item->tagValue = MakeStringOrNumeric(data->element->tag, 0, 0);
      /* newl->item->tagValue = MakeStringOrNumeric(NULL, 19, 1); */
      newl->item->tagOccurrence = 0;
      newl->item->content = CALLOC(ElementData,1);
      newl->item->content->which = e46_elementEmpty;
      newl->item->content->u.elementEmpty = (char)1;

      if (newl != NULL) {
	if (new_var->u.subtree != NULL) {
	  for (inl = new_var->u.subtree; inl; inl = inl->next)
	    lastl = inl;
	  lastl->next = newl;
	}
	else
	  new_var->u.subtree = newl;
      }
    }
    else {
      new_var->which = e46_elementEmpty;
      new_var->u.elementEmpty = (char)1;
    }
  }
  else { /* this is just PCDATA or flatten is in effect */
    if (new_var->which == e46_subtree) {
      int tempstart;
      tempstart = 0;
      newl = MakeDataSubtree(data, &tempstart);
      if (newl != NULL) {
	if (new_var->u.subtree != NULL) {
	  for (inl = new_var->u.subtree; inl; inl = inl->next)
	    lastl = inl;
	  lastl->next = newl;
	}
	else
	  new_var->u.subtree = newl;
      }
    }
    else {
      new_var->which = e46_string;
      
      stringdata = detag_data_block(data, NORM_NOMAP, NULL);
      
      if (stringdata == NULL) { /* this can happen with an empty tagged field */
	new_var->which = e46_elementEmpty;
      }
      else {
	new_var->u.string = NewInternationalString(stringdata);
	
	FREE(stringdata);
      }
    }
  }

  /* ------------ Here are the types and selection switches not used...
  case e46_octets:
    new_var->u.octets = DupOstring(in->u.octets);
    break;
  case e46_numeric:
    new_var->u.numeric = in->u.numeric;
    break;
  case e46_date:
    new_var->u.date = (GeneralizedTime)DupOstring((OctetString)in->u.date);
    break;
  case e46_ext:
    new_var->u.ext = DupExternal(in->u.ext);
    break;
  case e46_string:
    new_var->u.string = DupInternationalString(in->u.string);
    break;
  case e46_trueOrFalse:
    new_var->u.trueOrFalse = in->u.trueOrFalse;
    break;
  case e46_oid:
    new_var->u.oid = DupOID(in->u.oid);
    break;
  case e46_intUnit:
    new_var->u.intUnit = DupIntUnit(in->u.intUnit);
    break;
  case e46_elementNotThere:
    new_var->u.elementNotThere = in->u.elementNotThere;
    break;
  case e46_elementEmpty:
    new_var->u.elementEmpty = in->u.elementEmpty;
    break;
  case e46_noDataRequested:
    new_var->u.noDataRequested = in->u.noDataRequested;
    break;
  case e46_diagnostic:
    new_var->u.diagnostic = DupExternal(in->u.diagnostic);
    break;
 ---------------------------------------------------------------*/

  return (new_var);
}



TaggedElement *
MakeTaggedElement(SGML_Data *data, int type, int tag, char *stoptag)
{
  TaggedElement *new_var;
  char *tmp;
  int typeflag;
  int save_flags;

  if (data == NULL)
    return NULL;

  typeflag = 0;

  new_var = CALLOC(TaggedElement,1);

  if (new_var == NULL)
    return NULL;

  if (type == MIXED && tag > 0)
    new_var->tagType = 2;
  else
    new_var->tagType = type;

  if (stoptag) {
    save_flags = data->element->data_type;
    data->element->data_type = EL_MIXED;
  }
  
  if (type == 0 || (type == MIXED && tag == 0 && data->element != NULL)) {
    new_var->tagType = 3;

    if (data->element->tag == NULL) {
      /* this is a kludge because tag groups don't use element->tag */
      char *space;
      tmp = CALLOC(char, data->content_start_offset - 
		   data->start_tag_offset);
      
      strncpy(tmp, &data->start_tag[1], data->content_start_offset - 
	      data->start_tag_offset - 2);
      /* chop off any attributes */
      space = strchr(tmp, ' ');
      if (space != NULL)
	*space = '\0';
      if (stoptag != NULL && strcasecmp(stoptag,tmp) == 0) {
	data->element->data_type = save_flags;
	return (NULL);
      }
      new_var->tagValue = MakeStringOrNumeric(tmp, 0, 0);
      FREE(tmp);
      
    }
    else {
      if (stoptag != NULL && strcasecmp(stoptag,data->element->tag) == 0){
	data->element->data_type = save_flags;
	return (NULL);
      }
      new_var->tagValue = MakeStringOrNumeric(data->element->tag, 0, 0);
    }
    new_var->tagOccurrence = 0;
    new_var->content = MakeElementData(data, SUBTREES, &typeflag);
    if (typeflag) {
      /* This is a TagSet-M tag and the type is in typeflag */
      new_var->tagType = 1;
      FreeStringOrNumeric(new_var->tagValue);
      new_var->tagValue = MakeStringOrNumeric(NULL, typeflag, 1);
    }

    new_var->metaData = NULL; /* MakeElementMetaData(data); */
    new_var->appliedVariant = NULL; /* we don't do variants now */;
  }
  else { /* tagvalue is supplied as an int tag parameter */
    new_var->tagOccurrence = 0;
    if (type == MIXED)
      new_var->content = MakeElementData(data, SUBTREES, &typeflag);
    else
      new_var->content = MakeElementData(data, FLATTEN, &typeflag);

    if (typeflag) {
      /* This is a TagSet-M tag and the type is in typeflag */
      new_var->tagType = 1;
      new_var->tagValue = MakeStringOrNumeric(NULL, typeflag, 1);
    }
    else
      new_var->tagValue = MakeStringOrNumeric(NULL, tag, 1);

    new_var->metaData = NULL; /* MakeElementMetaData(data); */
    new_var->appliedVariant = NULL; /* we don't do variants now */;
  }

  if (stoptag != NULL)
    data->element->data_type = save_flags;

  return(new_var);
}


GenericRecord * 
MakeGRS1Record(SGML_Data *data, int type, int tag, char *stoptag)
{
  GenericRecord *new_var;

  if (data == NULL)
    return NULL;

  if (stoptag != NULL) {

    if (tag < data->start_tag_offset ||
	tag <= (data->content_start_offset + 3)) /*mininal tag size */
      return (NULL);

    if (data->element->tag == NULL) {
      /* this is a kludge because tag groups don't use element->tag */
      char *space, *tmp;
      tmp = CALLOC(char, data->content_start_offset - 
		   data->start_tag_offset);
      
      strncpy(tmp, &data->start_tag[1], data->content_start_offset - 
	      data->start_tag_offset - 2);
      /* chop off any attributes */
      space = strchr(tmp, ' ');
      if (space != NULL)
	*space = '\0';
      if (strcasecmp(stoptag,tmp) == 0) {
	FREE(tmp);
	return (NULL);
      }
      else
	FREE(tmp);
      
    }
    else {
      if (stoptag != NULL && strcasecmp(stoptag,data->element->tag) == 0)
	return (NULL);
    }
  }

  new_var = CALLOC(GenericRecord,1);

  if (new_var == NULL)
    return NULL;
 
  new_var->next = MakeGRS1Record(data->next_data_element, type, tag, stoptag);
    
  new_var->item = MakeTaggedElement(data, type, 0, stoptag);

  return(new_var);

}

int
mergegroup(struct data_list *conv, TaggedElement *elem)
{
  struct data_list *final, *cur;
  int size = 0;
  int count = 0;
  InternationalString olddata;
  int flag;
  char *tmp;


  /* if the element is a subtree we are in the wrong place */
  if (elem->content->which == e46_subtree)
    return (0);
  
  if (conv == NULL || conv->groupend == NULL)
    return(0);

  final = conv->groupend;

  /* find size */
  flag = 1;
  for (cur = conv; flag && cur != NULL ; cur = cur->next) {
    size += cur->item->content_end_offset -
      cur->item->content_start_offset;
    count++;
    if (cur == final) flag = 0;
  }

  /* allocate a buffer and concatenate the data elements */
  tmp = CALLOC(char, (size + (count * 5) + 1));

  flag = 1;
  for (cur = conv; flag ; cur = cur->next) {
    if (cur->item->content_start != NULL)
      strncat(tmp, cur->item->content_start, cur->item->content_end_offset - 
	      cur->item->content_start_offset);
      
    if (cur == final) 
      flag = 0;
    else
      strcat(tmp, ", ");
  }


  olddata = elem->content->u.string;
  elem->content->u.string = NewInternationalString(tmp);
  FREE(tmp);
  FreeInternationalString(olddata);

  return(1);
}


GenericRecord *
MakePartialGRS1Record(display_format *df, SGML_Document *doc, 
		      struct data_list *convert, int type)
{
  GenericRecord *new_var, *last, *head;
  struct data_list *conv;
  int i, tag;

  if (convert == NULL)
    return (NULL);

  if (type == TAGSET_G || type == MIXED ) {

    new_var = last = NULL;
    /* locate the first matching document element to convert */
    for (conv = convert; conv != NULL; conv = conv->next) {
      last = new_var;
      new_var = CALLOC(GenericRecord,1);
      
      if (new_var == NULL)
	return NULL;
      
      /* Get the TAGSET-G tag for this conversion element */
      tag = atoi(conv->map->to->key);
      if (tag == 0) {
	/* search for the tagset_g element name */
	for (i = 0; tagset_g[i].element_name != NULL; i++ ) {
	  if (strcasecmp(tagset_g[i].element_name, conv->map->to->key) == 0) {
	    tag = tagset_g[i].tag;
	    break;
	  }
	}
	if (tag == 0 && type == TAGSET_G) {
	  fprintf (LOGFILE,"Unknown TagSet-G tag in MakePartialGRS1Record\n");
	  return (NULL);
	} 
	else if (type == MIXED) {
	}
      }

      if (conv->map->summarize == NULL) {
	new_var->item = MakeTaggedElement(conv->item, type, tag, NULL);
      }
      else {
	/* summarize means to replace the field contents with the
	 * the summarize tag name...
	 */
	TaggedElement *newtag;


	if (conv->item == NULL) {
	  new_var->item = NULL;
	}
	else {

	  newtag = CALLOC(TaggedElement,1);
	  newtag->tagType = TAGSET_G;
	  newtag->tagValue = MakeStringOrNumeric(NULL, tag, 1);
	  newtag->tagOccurrence = 0;
	  newtag->content = CALLOC(ElementData,1);
	  newtag->content->which = e46_string;
	  newtag->content->u.string = 
	    NewInternationalString(conv->map->summarize->key);

	  newtag->metaData = NULL; 
	  newtag->appliedVariant = NULL; 
	  new_var->item = newtag;
	}
      }
 
      if (new_var->item->content->which != e46_subtree
	  && conv->groupend != NULL && conv->map->summarize == NULL) {
	mergegroup(conv, new_var->item);
	conv = conv->groupend;
      }

      if (last != NULL) {
	last->next = new_var;
      }
      else {
	head = new_var;
      }
    }
    
    return (head);

  }
  else if (type == TAGSET_M) {
  }
  else if (type == RECMAP) {
  }

  fprintf (LOGFILE,"Unknown conversion function in MakePartialGRS1Record\n");
  return (NULL);

}

GenericRecord *
ConvertGRS(display_format *df, SGML_Document *doc, 
		    struct data_list *convert, int type)
{
  GenericRecord *returnrec;

  returnrec = NULL;

  if (convert == NULL && type == RECMAP) {
    /* no display list, so we convert the whole record */
    returnrec = MakeGRS1Record(doc->data, RECMAP, 0, NULL);    
  }
  else {
    returnrec = MakePartialGRS1Record(df, doc, convert, type);
  }
  
  return(returnrec);
}

/*************************************************************
 * The following routines handle arbitrary conversion from SGML/XML
 * to MARC (MARC21-ish) 
 *************************************************************/
int 
EstimateMARClength(struct data_list *convert)
{
  struct data_list *conv;
  int sumlength = 0;
  int elementlen = 0;

  for (conv = convert; conv != NULL; conv = conv->next) {
    SGML_Tag_Data *td;
    for (td = conv->from_list_map; td ; td = td->next_tag_data) {
      if (td->item != NULL) { 
	elementlen = td->item->data_end_offset - td->item->start_tag_offset;
	sumlength += elementlen + 45; /* 45 is for MARC tag + indicator attr */
      }
    }
    if (conv->item) { /* since there may be no from_list_map... */
      elementlen = conv->item->data_end_offset - conv->item->start_tag_offset;
      sumlength += elementlen + 45;
    }
  }
  /* triple the size for leader info, etc. */
  return(sumlength * 3);
}

/* struct data_list {
 * SGML_Data *item;
 * int required;
 * idx_key *key;
 * cluster_map_entry *map;
 * struct data_list *groupend;
 * struct data_list *next; };
 */


void
Create_MARCSGML_header(char *buffer, int *start_offset, struct data_list *convert, int docid)
{
  char *material;
  char recstat, rectype, biblevel;
  struct data_list *conv;
  
  /* set defaults */
  material = "BK";
  recstat = 'n';
  rectype = 'a';
  biblevel = 'm';

  for (conv = convert; conv != NULL; conv = conv->next) {
    if (conv->map && conv->map->from != NULL && conv->map->from->key != NULL
	&& conv->map->to != NULL && conv->map->to->key != NULL) {
      if (strcasecmp("MARC_HEADER_MATERIAL_TYPE", conv->map->from->key) == 0) {
	material = conv->map->to->key;
      }
      else if (strcasecmp("MARC_HEADER_RECSTAT", conv->map->from->key) == 0) {
	recstat = conv->map->to->key[0];
      }
      else if (strcasecmp("MARC_HEADER_RECTYPE", conv->map->from->key) == 0) {
	rectype = conv->map->to->key[0];
      }
      else if (strcasecmp("MARC_HEADER_BIBLEVEL", conv->map->from->key) == 0) {
	biblevel = conv->map->to->key[0];
      }
    }
  }  

  sprintf(buffer, "<USMARC Material=\"%s\" ID=\"%d\"><leader><LRL>00000</LRL><RecStat>%c</RecStat><RecType>%c</RecType><BibLevel>%c</BibLevel><UCP></UCP><IndCount>2</IndCount><SFCount>2</SFCount><BaseAddr>00000</BaseAddr><EncLevel> </EncLevel><DscCatFm> </DscCatFm><LinkRec> </LinkRec><EntryMap><FLength>4</Flength><SCharPos>5</SCharPos><IDLength>0</IDLength><EMUCP></EMUCP></EntryMap></Leader><Directry> </Directry>", material, docid, recstat, rectype, biblevel);
  *start_offset = strlen(buffer);

  return;
}

char *
MapMARCtag(char *buffer, int *start_offset, struct data_list *convert)
{
  SGML_Data *dat = convert->item;
  char *datptr=NULL;
  int i, found, offset;
  SGMLINDS *matchind = NULL;

  char *ind1, *ind2;
  char *ind1_val, *ind2_val;
  char *start_ptr;
  char *out_tag;

  if (convert == NULL || convert->map == NULL || convert->map->to == NULL
      || convert->map->from == NULL || convert->map->to->key == NULL
      || convert->map->from->key == NULL) {
    fprintf (LOGFILE,"Error in Display syntax for MARC Conversion\n");
    diagnostic_set(227,0, "Unknown Record Syntax -- error in display syntax for MARC conversion?");
    return (NULL);
  }

  if (dat == NULL && convert->from_list_map == NULL)
    return (NULL);

  start_ptr = buffer+*start_offset;

  found = 0;

  /* here we handle a single element -- Should be a tag and subfield */
  /* first get the tag and indicator format... */
  for (i = 0; indicators[i].tag != NULL && found == 0; i++) {
    if (strcmp(indicators[i].tag, convert->map->to->key) == 0) {
      found = 1;
      matchind = &indicators[i];
    }
  }

  
  if (matchind == NULL && atoi(convert->map->to->key) > 9) {
    fprintf (LOGFILE,"Error in Display syntax for MARC Conversion (invalid tag %s)\n", convert->map->to->key);
    diagnostic_set(227,0, "Unknown Record Syntax -- error in display syntax for MARC conversion?");
    return (NULL);
  } 
  else if (atoi(convert->map->to->key) < 10) {
    /* OK, No indicators for the < 10 tags */
    out_tag = convert->map->to->key;
    sprintf(buffer+*start_offset, "<Fld%s>", out_tag);
    *start_offset = strlen(buffer);
  }
  else {
    /* OK must have the indicator values -- so figure out HOW to lay it out */
    /*first check to see if there are specified indicator values */
    ind1 = matchind->attrib1;
    ind2 = matchind->attrib2;
    /* if no subkeys -- take default blank values for indicators */
    ind1_val = matchind->att1vals[10];
    ind2_val = matchind->att2vals[10];
    out_tag = matchind->tag;

    if (convert->map->to->subkey != NULL
	&& convert->map->to->subkey->attribute_flag == 1
	&& convert->map->to->subkey->subkey != NULL
	&& convert->map->to->subkey->subkey->attribute_flag == 2) {
      
      if (strncmp(convert->map->to->subkey->key, "1", 1) == 0) 
	ind1_val = convert->map->to->subkey->subkey->key;
      else  if (strncmp(convert->map->to->subkey->key, "2", 1) == 0) 
	ind2_val = convert->map->to->subkey->subkey->key; 
      else {
	fprintf (LOGFILE,"Error in Display syntax for MARC Conversion (invalid indicator number %s)\n", convert->map->to->subkey->key);
	diagnostic_set(227,0, "Unknown Record Syntax -- error in display syntax for MARC conversion?");
	return (NULL);
      }
      /* if there are subkeys of the attribute value -- then it SHOULD be the */
      /* second indicator                                                     */
      if (convert->map->to->subkey->subkey->subkey != NULL
	  && convert->map->to->subkey->subkey->subkey->attribute_flag == 1
	  && convert->map->to->subkey->subkey->subkey->subkey != NULL
	  && convert->map->to->subkey->subkey->subkey->subkey->attribute_flag == 2) {
	
	if (strncmp(convert->map->to->subkey->subkey->subkey->key, "1", 1) == 0) 
	  ind1_val = convert->map->to->subkey->subkey->subkey->subkey->key;
	else  if (strncmp(convert->map->to->subkey->subkey->subkey->key, "2", 1) == 0) 
	  ind2_val = convert->map->to->subkey->subkey->subkey->subkey->key;
	else {
	  fprintf (LOGFILE,"Error in Display syntax for MARC Conversion (invalid indicator number %s)\n", convert->map->to->subkey->subkey->subkey->key);
	  diagnostic_set(227,0, "Unknown Record Syntax -- error in display syntax for MARC conversion?");
	  return (NULL);
	}
      }
    }
  
    /* OK, should have the tag and indicators sorted for this one */
    sprintf(buffer+*start_offset, "<Fld%s %s=\"%s\" %s=\"%s\">",
	    out_tag, ind1, ind1_val, ind2, ind2_val);
    *start_offset = strlen(buffer);
  }

  if (convert->map->to->next_key == NULL) { 
    offset = *start_offset;
    if (atoi(out_tag) > 9) {
      /* printf("put out fake $a?"); */
      sprintf(buffer+offset, "<a>");
      offset += 3;
    }
    /* next put the data out */
    /* skip leading whitespace... */
    for (datptr = dat->content_start; isspace((int)*datptr); datptr++);
    /* next put the data out */
    for (; datptr < dat->end_tag; datptr++) {
      if (*datptr == '\n' || *datptr == '\t' || *datptr == '\f') {
      buffer[offset++] = ' ';
      }
      else if (*datptr == '<') {
	/* skip imbedded tags here... */
	while (*datptr != '>' && datptr < dat->end_tag) datptr++;
      }
      else {
	buffer[offset++] = *datptr;
      }
    }

    if (atoi(out_tag) > 9) {
      /* printf("put out fake $a?"); */
      sprintf(buffer+offset, "</a>");
      offset += 4;
    }
  }
  else {
    idx_key *tokey, *fromkey;
    SGML_Tag_Data *td;

    /* there is a next key -- I.e. the subfields of the MARC record */
    offset = *start_offset;

    if (convert->from_list_map != NULL) {
      td = convert->from_list_map;
      dat = td->item;
    }
    else
      td = NULL;

    for (tokey = convert->map->to->next_key; tokey; tokey = tokey->next_key) {
      /* if dat is null it means a subtag had no match, so skip it */
      /* output the subtag */
      if (dat != NULL) {
	sprintf(buffer+offset, "<%s>", tokey->key);
	offset += 2 + strlen(tokey->key);
	/* next put the data out */

	/* skip leading whitespace... */
	for (datptr = dat->content_start; isspace((int)*datptr); datptr++);
	/* next put the data out */
	for (; datptr < dat->end_tag; datptr++) {
	  if (*datptr == '\n' || *datptr == '\t' || *datptr == '\f') {
	    buffer[offset++] = ' ';
	  }
	  else if (*datptr == '<') {
	    /* skip imbedded tags here... */
	    while (*datptr != '>' && datptr < dat->end_tag) datptr++;
	  }
	  else {
	    buffer[offset++] = *datptr;
	  }
	}

	sprintf(buffer+offset, "</%s>", tokey->key);
	offset += 3 + strlen(tokey->key);
      }
      if (td != NULL)
	td = td->next_tag_data;
      if (td != NULL)
	dat = td->item;
      else if (td == NULL && dat == convert->item) {
	/* only a single subfield, so don't keep outputting things */
	dat = NULL;
      }
    }
  }

  /* put out the end tag... */
  sprintf(buffer+offset, "</Fld%s>", out_tag);
  *start_offset = strlen(buffer);

  return(start_ptr);
     
}

void
GenerateTagRange(char *starttags, char *endtags, char *buffer, 
		 int *buffer_len, int mintag, int maxtag, 
		 struct data_list *convert)
{
  struct data_list *conv;

  /* Put out the tags -- regardless */
  sprintf(buffer+*buffer_len, starttags);
  *buffer_len = strlen(buffer);

  for (conv = convert; conv != NULL; conv = conv->next) {
    char *added;
    /* Map the conversion list to the specified MARC tags in the order
     * they appear SHOULD appear in the record
     */
    if (conv->map && conv->map->to && conv->map->to->key &&
	atoi(conv->map->to->key) > mintag 
	&& atoi(conv->map->to->key) < maxtag) {
      added = MapMARCtag(buffer, buffer_len, conv);
    }
  }

  sprintf(buffer+*buffer_len, endtags);
  *buffer_len = strlen(buffer);
  return;
}

char *
ConvertMARC(display_format *df, SGML_Document *doc, 
		    struct data_list *convert, char *source_file)
{
  struct data_list *conv;
  int i, tag;
  char *marcsgml;
  int marcsgml_length;
  SGML_Document *marcdoc;
  SGML_DTD *save_dtd = NULL;
  extern SGML_DTD *DTD_list, *current_sgml_dtd;
  char *marcrec;
  int buffer_start;
  char *tmpfname = NULL;

  /* This is a little convoluted -- but it seemed that the simplest 
   * way to do the conversion was to generate a MARCSGML record
   * then to parse the SGML and pass it to sgml2marc for the final
   * conversion, we also need to add the fields in a particular order
   * with inserted "group" tags for the bib areas -- this means lots
   * of passes through the conversion specs...
   */
      
  marcdoc = NULL;

  if (convert == NULL)
    return (NULL);

  if (df->marc_dtd_file_name == NULL) {
    /* have to have the dtd... */
    fprintf (LOGFILE,"MARC Conversion Error -- NO DTD specified in format/displaydef\n");
    diagnostic_set(227,0, df->oid);
    return (NULL);
  }

  /* estimate the marc size from the conv data and allocate a buffer */
  marcsgml_length = EstimateMARClength(convert);

  marcsgml = CALLOC(char, marcsgml_length);
  buffer_start = 0;
  


  Create_MARCSGML_header(marcsgml,&buffer_start, convert, doc->record_id);

  GenerateTagRange("<VarFlds><VarCFlds>", "</VarCFlds>",
		   marcsgml, &buffer_start, 0, 10, convert);

  GenerateTagRange("<VarDFlds><NumbCode>", "</NumbCode>",
		   marcsgml, &buffer_start, 9, 100, convert);

  GenerateTagRange("<MainEnty>", "</MainEnty>",
		   marcsgml, &buffer_start, 99, 200, convert);

  GenerateTagRange("<Titles>", "</Titles>",
		   marcsgml, &buffer_start, 199, 260, convert);

  GenerateTagRange("<EdImprnt>", "</EdImprnt>",
		   marcsgml, &buffer_start, 259, 261, convert);

  GenerateTagRange("<PhysDesc>", "</PhysDesc>",
		   marcsgml, &buffer_start, 299, 400, convert);

  GenerateTagRange("<Series>", "</Series>",
		   marcsgml, &buffer_start, 399, 500, convert);

  GenerateTagRange("<Notes>", "</Notes>",
		   marcsgml, &buffer_start, 499, 600, convert);

  GenerateTagRange("<SubjAccs>", "</SubjAccs>",
		   marcsgml, &buffer_start, 599, 700, convert);

  GenerateTagRange("<AddEnty>", "</AddEnty>",
		   marcsgml, &buffer_start, 699, 760, convert);

  GenerateTagRange("<LinkEnty>", "</LinkEnty>",
		   marcsgml, &buffer_start, 759, 800, convert);

  GenerateTagRange("<SAddEnty>", "</SAddEnty>",
		   marcsgml, &buffer_start, 799, 841, convert);

  GenerateTagRange("<HoldAltG>", "</HoldAltG>",
		   marcsgml, &buffer_start, 840, 900, convert);

  GenerateTagRange("<Fld9XX>", "</Fld9XX>",
		   marcsgml, &buffer_start, 899, 1000, convert);

  sprintf(marcsgml+buffer_start, "</VarDFlds></VarFlds></USMARC>");

  /* Parse the MARC dtd provided */
  save_dtd = current_sgml_dtd; /* keep the 'current' one */

  if (marc_dtd == NULL) 
    marc_dtd = sgml_parse_dtd(df->marc_dtd_file_name, NULL,
			      NULL, NULL, 0 /* regular SGML dtd -- not schema */);
  
  marcdoc = sgml_parse_document(marc_dtd, source_file, marcsgml, 
				doc->record_id, 0);

  FREE(tmpfname);

  current_sgml_dtd = save_dtd;

  marcrec = sgml_to_marc(marcdoc);


  /* after all the work building it, we discard now... */
  free_doc(marcdoc);

  return(marcrec);
}



int
ConvertXML_Element(display_format *df, SGML_Document *doc, 
		   char **buf, long maxlen,
		   struct data_list *convert, char *compname, int compid)
{
  struct data_list *dl;
  SGML_Data *dat;
  cluster_map_entry *map;
  int result_recsize, last_len;
  char header[200];
  char *last_data_start;
  int pathlen;
  int maxpath;
  int elementsize;
  int type = 0;
  idx_key *key, *tokey; 


  if (convert == NULL) {
    /* no data found for conversion -- create empty result record*/
    sprintf(header, "<RESULT_DATA DOCID=\"%d\"></RESULT_DATA>", doc->record_id);
    
    *buf = CALLOC(char, strlen(header) + 1);
    strcat(*buf, header);
    return (strlen(*buf));
  }
  
  result_recsize = 0;
  
    
  for (dl = convert; dl != NULL; dl = dl->next) {
    dat = dl->item;
    elementsize = (dat->data_end_offset - dat->start_tag_offset);
    if (elementsize < 0 ) {
      /* empty tag ???? */
      elementsize = 500;
    }

    result_recsize += elementsize + 10;
    result_recsize += Xpathlen(dat) + 30;
  }

  if (compname == NULL)
    sprintf(header, "<RESULT_DATA DOCID=\"%d\">\n", doc->record_id);
  else
    sprintf(header, 
	    "<RESULT_DATA DOCID=\"%d\" COMPNAME=\"%s\" COMPID=\"%d\">\n", 
	    doc->record_id, compname, compid);
  
  *buf = CALLOC(char, result_recsize + strlen(header) + 16 );
  strcat(*buf, header);
  
  for (dl = convert; dl != NULL; dl = dl->next) {
    dat = dl->item;
    map = dl->map;
    tokey = map->to;

    strcat(*buf,"<ITEM XPATH=\"");
    Xpathcpy(dat, *buf);
    strcat(*buf,"\">\n");
    if (tokey != NULL && tokey->key != NULL 
	&& ((strcasecmp(tokey->key, "XPATH_ONLY") == 0)
	    ||(strcasecmp(tokey->key, "#XPATH_ONLY#") == 0))) {
      type = XPATH_ONLY_OUTPUT;
    }
    else {
      elementsize = (dat->data_end_offset - dat->start_tag_offset);
      
      if (elementsize > 0) 
	strncat(*buf, dat->start_tag, (dat->data_end_offset - dat->start_tag_offset));
    }
    strcat(*buf,"\n</ITEM>\n");
  }    

  strcat (*buf, "</RESULT_DATA>\n");

  return (strlen(*buf));
}



int
Convert_Relational(display_format *df, SGML_Document *doc, 
		   char **buf, long maxlen,
		   struct data_list *convert, char *compname, int compid, 
		   int delimiter_flag)
{
  struct data_list *dl;
  SGML_Data *dat;
  cluster_map_entry *map;
  int result_recsize, last_len;
  char header[200];
  char *last_data_start;
  int pathlen;
  int maxpath;
  int elementsize;
  int type = 0;
  idx_key *key, *tokey; 


  if (convert == NULL) {
    /* no data found for conversion -- create empty result record*/
    sprintf(header, "<RESULT_DATA DOCID=\"%d\"></RESULT_DATA>", doc->record_id);
    
    *buf = CALLOC(char, strlen(header) + 1);
    strcat(*buf, header);
    return (strlen(*buf));
  }
  
  result_recsize = 0;
  
    
  for (dl = convert; dl != NULL; dl = dl->next) {
    dat = dl->item;
    elementsize = (dat->data_end_offset - dat->start_tag_offset);
    if (elementsize < 0 ) {
      /* empty tag ???? */
      elementsize = 500;
    }

    result_recsize += elementsize + 10;
    result_recsize += Xpathlen(dat) + 30;
  }

  if (compname == NULL)
    sprintf(header, "<RESULT_DATA DOCID=\"%d\">\n", doc->record_id);
  else
    sprintf(header, 
	    "<RESULT_DATA DOCID=\"%d\" COMPNAME=\"%s\" COMPID=\"%d\">\n", 
	    doc->record_id, compname, compid);
  
  *buf = CALLOC(char, result_recsize + strlen(header) + 16 );
  strcat(*buf, header);
  
  for (dl = convert; dl != NULL; dl = dl->next) {
    dat = dl->item;
    map = dl->map;
    tokey = map->to;

    strcat(*buf,"<ITEM XPATH=\"");
    Xpathcpy(dat, *buf);
    strcat(*buf,"\">\n");
    if (tokey != NULL && tokey->key != NULL 
	&& ((strcasecmp(tokey->key, "XPATH_ONLY") == 0)
	    ||(strcasecmp(tokey->key, "#XPATH_ONLY#") == 0))) {
      type = XPATH_ONLY_OUTPUT;
    }
    else {
      elementsize = (dat->data_end_offset - dat->start_tag_offset);
      
      if (elementsize > 0) 
	strncat(*buf, dat->start_tag, (dat->data_end_offset - dat->start_tag_offset));
    }
    strcat(*buf,"\n</ITEM>\n");
  }    

  strcat (*buf, "</RESULT_DATA>\n");

  return (strlen(*buf));
}


int 
display_convert(display_format *df, SGML_Document *doc, 
		char **buf, long maxlen, struct data_list *convert, 
		char *funcname, char *add_tags, char **outptr, 
		char *rec_file_name, char *compname,int compid)
{
  int result_recsize;
  extern FILE *LOGFILE;
  char *tmp_path;
  char infilename[100];
  FILE *infile;
  char outfilename[100];
  FILE *outfile;
  char conv_cmd[500];
  struct stat filestatus;
  int systemresult;


  if (strcasecmp(funcname, "PAGE_PATH") == 0) {
    result_recsize = display_exclude(doc, buf, maxlen, convert,
				     df->exclude_compress, add_tags);
    if (add_tags != NULL) {
      *buf = REMALLOC(*buf,strlen(*buf)+strlen(add_tags)+1);
      strcat(*buf, add_tags);
      result_recsize = strlen(*buf);
    }
  }
  else if (strcasecmp(funcname, "RECMAP") == 0) {
    /* check what format to map this record to... */
    if (strcmp(df->oid, GRS1RECSYNTAX) == 0) {
      *outptr = (char *) ConvertGRS(df, doc, convert, RECMAP);
      result_recsize = strlen(doc->buffer);
    }
    else if (strcmp(df->oid, MARCRECSYNTAX) == 0) {
      diagnostic_set(227,0,MARCRECSYNTAX );
      return (-4);
    }
    else if (strcmp(df->oid, EXPLAINRECSYNTAX) == 0) {
      diagnostic_set(227,0, EXPLAINRECSYNTAX);
      return (-4);
    }
    else if (strcmp(df->oid, OPACRECSYNTAX) == 0) {
      diagnostic_set(227,0,OPACRECSYNTAX);
      return (-4);
    }
    else if (strcmp(df->oid, SUMMARYRECSYNTAX) == 0) {
      diagnostic_set(227,0,SUMMARYRECSYNTAX);
      return (-4);
    }
    else if (strcmp(df->oid, GRS0RECSYNTAX) == 0) {
      diagnostic_set(227,0, GRS0RECSYNTAX);
      return (-4);
    }
    else if (strcmp(df->oid, ESRECSYNTAX) == 0) {
      diagnostic_set(227,0,ESRECSYNTAX);
      return (-4);
    }
    else {
      fprintf (LOGFILE,"Unknown record syntax requested in display_convert\n");
      diagnostic_set(227,0, "Unknown Record Syntax");
      return (-4);
    }
  }
  else if (strcasecmp(funcname, "TAGSET-G") == 0) {
    /* check what format to map this record to... */
    if (strcmp(df->oid, GRS1RECSYNTAX) == 0) { 
      *outptr = (char *) ConvertGRS(df, doc, convert, TAGSET_G);
      result_recsize = strlen(doc->buffer);
    } 
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strcasecmp(funcname, "MIXED") == 0) {
    /* check what format to map this record to... */
    if (strcmp(df->oid, GRS1RECSYNTAX) == 0) {
      *outptr = (char *) ConvertGRS(df, doc, convert, MIXED);
      result_recsize = strlen(doc->buffer);
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strncasecmp(funcname, "XML_ELEMENT", 11) == 0) {
    /* check what format to map this record to... */
    if (df->oid != NULL && strcmp(df->oid, GRS1RECSYNTAX) == 0) {
      *outptr = (char *) ConvertGRS(df, doc, convert, MIXED);
      result_recsize = strlen(doc->buffer);
    }
    else if (df->oid != NULL && (strcmp(df->oid, XML_RECSYNTAX) == 0
	     || strcmp(df->oid, SGML_RECSYNTAX) == 0)) {
      return(ConvertXML_Element(df, doc, buf, maxlen,
				convert, compname, compid ));
      
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strncasecmp(funcname, "XML_ELEMENT_INEX", 16) == 0) {
    /* check what format to map this record to... */
    if (df->oid != NULL && (strcmp(df->oid, XML_RECSYNTAX) == 0
			    || strcmp(df->oid, SGML_RECSYNTAX) == 0)) {
      return(ConvertXML_Element(df, doc, buf, maxlen,
				convert, compname, compid ));
      
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strcasecmp(funcname, "MARC") == 0) {
    /* check what format to map this record to... */
    if (df->oid != NULL && strcmp(df->oid, MARCRECSYNTAX) == 0) {
      *outptr = (char *) ConvertMARC(df, doc, convert, rec_file_name);
      result_recsize = strlen(doc->buffer);
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strcasecmp(funcname, "RELATIONAL") == 0) {
    /* check what format to map this record to... */
    if (df->oid != NULL && (strcmp(df->oid, SUTRECSYNTAX) == 0
			    || strcmp(df->oid, XML_RECSYNTAX) == 0
			    || strcmp(df->oid, SGML_RECSYNTAX) == 0)) {
      return(Convert_Relational(df, doc, buf, maxlen,
				convert, compname, compid, 0 ));
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (strcasecmp(funcname, "COMMADELIMITED") == 0) {
    if (df->oid != NULL && (strcmp(df->oid, SUTRECSYNTAX) == 0
			    || strcmp(df->oid, XML_RECSYNTAX) == 0
			    || strcmp(df->oid, SGML_RECSYNTAX) == 0)) {
      return(Convert_Relational(df, doc, buf, maxlen,
				convert, compname, compid, 1 ));
    }
    else {
      diagnostic_set(227,0,df->oid);
      return (-4);
    } 
  }
  else if (funcname[0] == '/') { 
    /* function is a pathname of an external function */
    /* the raw data from the record is passed to that function */
    /* in this case, add_tags should contain directory used for result sets */
    if (add_tags == NULL) {
      sprintf(infilename, "/usr/tmp/CONVXXXXXX");
      sprintf(outfilename, "/usr/tmp/CONVXXXXXX");
    }
    else {
      sprintf(infilename, "%s/CONVXXXXXX", add_tags);
      sprintf(outfilename, "%s/CONVXXXXXX", add_tags);
    }
    
    /* create two temporary filenames */

#ifdef WIN32
    mktemp(infilename);
    mktemp(outfilename);
#else
    mkstemp(infilename);
    mkstemp(outfilename);
#endif
    
    /* this stuff probably needs to be redone in an NT version */
    /* put the data into the temp file */
    infile = fopen(infilename, "w");
    fwrite(doc->buffer, strlen(doc->buffer), 1, infile);
    fclose(infile);
    
#ifdef WIN32
    /* create the command -- this needs testing on NT */
    sprintf(conv_cmd,"%s < %s > %s 2>&1", funcname, infilename, outfilename);
#else
    /* create the command */
    sprintf(conv_cmd,"%s < %s > %s 2>&1", funcname, infilename, outfilename);
#endif
    /* run it... */
    systemresult = system(conv_cmd);
    
    if (systemresult == 127 || systemresult == 256) { 
      /* shell couldn't exec program (ALPHA) */
      diagnostic_set(14,0,"External Conversion Program not executable or generated error");
      unlink(infilename);
      return (-4);
    }
    else if (systemresult == -1) {
      diagnostic_set(14,0,"Error in running external conversion");
      unlink(infilename);
      return (-4);
    } 
    else {
      /* check the output file... */
      if (stat(outfilename, &filestatus) != 0) {
	fprintf(LOGFILE,
		"stat for conversion output file %d failed in display_convert\n",
		outfilename);
	diagnostic_set(14,0,"Output file not created in display conversion");
	unlink(infilename);
	return (-4);
      }
    
      result_recsize = filestatus.st_size + 1;
      *outptr = CALLOC(char, result_recsize);
      outfile = fopen(outfilename, "r");
      fread(*outptr,filestatus.st_size,1, outfile);
      fclose(outfile);
      unlink(outfilename);
      unlink(infilename);
      *buf = *outptr;
    }
  }
  else {
    fprintf (LOGFILE,"Unknown conversion function in display_covert '%s'\n",
	     funcname);
  }
  return (result_recsize);
}


int 
displayComponent(SGML_Document *parent_doc, SGML_Data *data, int docid, 
		 long maxlen, char *format, char **oid, char *filename, 
		 component_list_entry *comp, char **buf, 
		 int rank, int relevance, float raw_relevance)
{
  GenericRecord *returnrec;
  int result_recsize = 0;
  char componentid[2000];
  char *compnamestr;
  char *stopkey;
  int idlength;
  int save_type = 0;
  display_format *df, *cf_getdisplay(), *cf_getdefaultdisplay();
  SGML_Tag_Data tag_data;
  char *realfilename = NULL;

  realfilename = cf_getdatafilename(filename, parent_doc->record_id);

  if (realfilename == NULL) {
    realfilename = filename;
  }


  /* we have limited options for component display currently */
  /* either output as plain text or SGML/XML without DTD     */
  /* or do a GRS-1 conversion                                */
  
  first_data_list = current_data_list = last_data_list = NULL;

  if (format[0] != '\0')
    df = cf_getdisplay(filename, *oid, format);
  else
    df = cf_getdefaultdisplay(filename);

  /* this is needed for conversions */
  if (df != NULL && df->oid == NULL && *oid != NULL) 
    df->oid = *oid;
  

  /* and this is needed to pass back the record syntax when conversions  */
  /* are done without as specified record syntax and it is supplied from */
  /* the display format                                                  */
  if (*oid == NULL && df != NULL && df->oid != NULL)
    *oid = df->oid;

  /* if EVERYTHING is null set to SGML to avoid crashing later */
  if (df == NULL && *oid == NULL) {
    *oid = SGML_RECSYNTAX;
  }

#ifdef WIN32
  if ((compnamestr = strrchr(comp->name, '\\')) != NULL) {
    compnamestr++;
  }
#else
  if ((compnamestr = strrchr(comp->name, '/')) != NULL) {
    compnamestr++;
  }
#endif
  else { /* already a short name */
    compnamestr = comp->name;
  }
      

  if (df == NULL || df->include != NULL) { 
    /* no (usable) display stuff in config file... */
    if (df == NULL && format != NULL 
	&&(strcmp(format,"F") == 0 || strcmp(format,"B") == 0)
	&& (*oid == NULL || strcmp(*oid, SUTRECSYNTAX) == 0 
	    || strcmp(*oid, XML_RECSYNTAX) == 0
	    || strcmp(*oid, SGML_RECSYNTAX) == 0)) {
      /* the generic specs apply -- return the whole component */
      /* these are set in get_component_data for spans         */
      result_recsize = data->data_end_offset - data->start_tag_offset;

      if (result_recsize < maxlen) {
	*buf = CALLOC(char, result_recsize+2010); /* add space for the ID */
	
	sprintf(componentid, "<?%s:%d|PARENT-DOCID %d|FILEPATH %s|RANK %d|REL %'.7f|START %d|END %d ?>", compnamestr, 
		docid, parent_doc->record_id, realfilename, 
                rank, raw_relevance, data->start_tag_offset,
		data->data_end_offset);
	idlength = strlen(componentid);
	strcpy(*buf, componentid);
	strncpy(*buf+idlength,data->start_tag, result_recsize);
      }
      else
	result_recsize = -1; /* error code for rec too long */
      
      return (result_recsize);
    }
    else if (*oid != NULL && strcmp(*oid, GRS1RECSYNTAX) == 0
	     /* && comp->end_tag == NULL */) {

      /* here is the component ID... */
      sprintf(componentid, "%s:%d", compnamestr, docid);

      /* GRS conversion */
      if (comp->end_tag) {
	idx_key *k;
	/* we provide the last subelement of the key */
	for (k = comp->end_tag ; k->subkey != NULL ; k = k->subkey) ;
	stopkey = k->key;
      }
      else
	stopkey = NULL;

      /* When converting components that start at an empty tag, we need */
      /* to fake some structure.                                        */
      if (data->element != NULL && data->element->data_type == EL_EMPTY) {
	save_type = EL_EMPTY;
	data->element->data_type = EL_MIXED;
	/* it is OK to mess with main component record pointers -- the */
	/* record is fake anyway and will be removed after the display */
	data->sub_data_element = data->next_data_element ;
	data->next_data_element = NULL;
      }

      returnrec = MakeGRS1Record(data, RECMAP, data->data_end_offset, stopkey);    
      if (save_type)
	data->element->data_type = save_type;
      
      *buf = (char *)returnrec;
      return (0); /* another indication of GRS-1 conversion */
    }
    else { /* not currently supported */
      fprintf(LOGFILE,
	      "Unsupported conversion in displayComponent: %s\n",
	      format);
      return (-2);
    }
  }
  else /* df != NULL */ {
    /* OK, there is a display format for this file, so we apply it */
    if (df->exclude) { /* exclusion spec */
      /* display_build_list_keys(doc, df->exclude, NULL, docid);
       * result_recsize = display_exclude(doc, buf, maxlen, first_data_list,
       *		       df->exclude_compress, add_tags);
       * *outptr = *buf;
       */
    }
    if (df->convert_name) { /* conversion spec */
      /* here is the component ID... */
      sprintf(componentid, "%s:%d", compnamestr, docid);
      tag_data.next_tag_data = NULL;
      tag_data.item = data;

      comp_display_build_list_map(parent_doc, &tag_data, df->convert, 
				  componentid, rank, relevance, raw_relevance,
				  df->convert_name, format, filename, docid);

      result_recsize = display_convert(df, parent_doc, buf, 100000, 
				       first_data_list, df->convert_name,
				       NULL, buf, filename, compnamestr, docid);
    }
    
    if (first_data_list) free_datalist(first_data_list);
        
  }

  return (result_recsize);
}

/* handle "string_segment" formatting directions */
int 
displaystring(char *doc, int docid, char **buf, int maxlen, 
	   char *format, char *oid, char *filename,
	   int rank, int relevance, float raw_relevance)
{
  int result_recsize;
  display_format *df, *cf_getdisplay();
  display_format fake_df;
  char *format_names;
  int first_num = 0;
  int second_num = 0;
  int doc_len;
  char *first;
  char *end;
  char rankdatabuf[5000];
  int databuflen;
  char *realfilename = NULL;

  realfilename = cf_getdatafilename(filename, docid);

  if (realfilename == NULL) {
    realfilename = filename;
  }

  sprintf(rankdatabuf,"docid %d|rank %d|relv %d|rawrel %12.10f|filename %s|", 
	  docid, rank, relevance, raw_relevance, realfilename);


  databuflen = strlen(rankdatabuf)+1;

  result_recsize = 0;

  if (doc == NULL) {
    fprintf (LOGFILE,"Error in STRING_SEGMENT processing (NULL document)\n");
    diagnostic_set(227,0, "Error in STRING_SEGMENT processing (NULL document)");
    return (-1);
  }
  
  if (strlen(format) < 16) {
    fprintf (LOGFILE,"Error in STRING_SEGMENT specification\n");
    diagnostic_set(227,0, "Error in STRING_SEGMENT_ specification for display -- No string specification?");
    return (-1);
  }
  
  /* OK, we allow two kinds -- a simple substring, or the FIRST matching */
  /* tag (as XML/SGML)                                                   */
  doc_len = strlen(doc);
  
  format_names = format+15;
  
  if (isdigit(*format_names)) {
    sscanf(format_names, "%d_%d", &first_num, &second_num);
    
    if (first_num >= 0 && second_num > 0) {
      if (second_num > doc_len)
	second_num = doc_len;
      
      result_recsize = second_num - first_num;
      
    }
    else if (first_num > 0 && second_num == 0) {
      second_num = first_num;
      first_num = 0;
      result_recsize = second_num;
    }
    
    if (result_recsize <= 0) {
      fprintf (LOGFILE,"Error in STRING_SEGMENT specification\n");
      diagnostic_set(227,0, "Error in STRING_SEGMENT processing (zero or negative length specification)");
      result_recsize = -2; /* error code for negative or zero */
      return (result_recsize);
    }
    
    if (result_recsize < maxlen) {
      *buf = CALLOC(char, result_recsize+databuflen);
      strcpy(*buf,rankdatabuf);
      strncat(*buf, doc+first_num, (second_num - first_num));
    }
    else
      result_recsize = -1; /* error code for rec too long */
  }
  else {
    /* match a tag ? */
    /*    diagnostic_set(227,0, "Error in STRING_SEGMENT -- only numeric ranges available"); */
    result_recsize = 0; /* error code for negative or zero */

    first = doc+1;

    while ((first = strstr(first+1, format_names)) != NULL 
	   && *(first-1) != '<') ;

    if (first == NULL) {
      /* No tag */
      result_recsize = 45; /* error code for negative or zero */
      *buf = CALLOC(char, 45);
      sprintf(*buf, "*** NO MATCHING TAGS IN MATCHING RECORD ***");
    } else {
      /* have a match */
      end = strstr(first+1, format_names);
      result_recsize = ((end+(strlen(format_names)+1))-(first-1));
      *buf = CALLOC(char, result_recsize+databuflen);
      strcpy(*buf,rankdatabuf);
      strncat(*buf, first-1, result_recsize);
    }
    
    
  }
  return (result_recsize);
}


