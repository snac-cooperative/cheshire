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
*	Header Name:	
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing an attribute in an SGML tag.
*
*	Usage:	      idxattr(data, attr_name, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*
*	Variables:    SGML_data data, char *attr_name,
*                     Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
*
*	Return Conditions and Return Codes:	
*
*	Date:		01/10/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#endif
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

char *get_attr_value (SGML_Data *data, char * name);

int idxattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *word_ptr, *next_tok, *result_word;
  char *databuf;
  int morphed = 0;
  int exists;
  char *wn_morph();
  Tcl_HashEntry *entry;
  Tcl_HashTable *stoplist_hash;
  DB *index_db; 
  int index_type;
  char buffer[40];
  int proxvalue;
  int buffer_offset, word_offset;

  char *breakletters;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n\r\a\b\v\f<>";
  char *filenameletters = " \t\n\r\a\b\v\f<>";
  char *noletters = " \t\n\r\a\b\v\f";

  HT_DATA *ht_data;
  POSDATA *posdata;
  
  databuf = get_attr_value(data, attr_name);

  buffer_offset =  data->start_tag_offset + 
    ((data->content_start_offset - data->start_tag_offset)/2); /*VERY approx*/

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;

  if (databuf == NULL) return(0);

  if (strlen(databuf) == 0) return(0);

  if (index_type & KEYWORD) breakletters = keywordletters; 
  if (index_type & URL_KEY) breakletters = urlletters;
  if (index_type & FILENAME_KEY) breakletters = filenameletters;
  if (index_type & NORM_DO_NOTHING) breakletters = noletters;
  if (index_type & NORM_MIN) breakletters = noletters;

  if (index_type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) 
    breakletters = noletters;

  /* find first token */
  if (index_type & EXACTKEY) 
    word_ptr = databuf;
  else
    word_ptr = strtok_r (databuf, breakletters, &next_tok);
  /* a buffer full of blanks or punct? */
  if (word_ptr == NULL)
    return(0);

  word_offset = buffer_offset + (int)(word_ptr - databuf);
  
  do {
    /* normalize the word according to the specs for this index */
    /* (as defined in indextype)                                */
    
    result_word = normalize_key(word_ptr, idx, &morphed, 1);
    
    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL && strlen(result_word) > 0) {
      
      /* We have a normalized word, is it in the hash table? */
      entry = Tcl_FindHashEntry(hash_tab,result_word);
      
      if (entry == NULL){ /* nope, it is new word */
	/* create the hash table structure */
	ht_data = CALLOC(struct hash_term_data, 1) ;
	
	ht_data->termid = -1;
       	ht_data->termfreq = 1;


	if (index_type & PROXIMITY) {
	  /* add the offset info to the structure */
	  posdata = CALLOC(struct posdata, 1);
	  posdata->position = word_offset;
	  ht_data->pos = posdata;
	  ht_data->last = posdata;
	}
	
	/* put it into the hash table */
	Tcl_SetHashValue(
			 Tcl_CreateHashEntry(hash_tab,
					     result_word, &exists), 
			 (ClientData)ht_data);
	FREE(result_word);
      }
      else { /* word already in hash table, so just increment it */
	ht_data =  
	  (struct hash_term_data *) Tcl_GetHashValue(entry);
	ht_data->termfreq++;
	if (index_type & PROXIMITY) {
	  /* add the offset info to the structure */
	  posdata = CALLOC(struct posdata, 1);
	  posdata->position = word_offset;
	  ht_data->last->next = posdata;
	  ht_data->last = posdata;
	}
	FREE(result_word);
      }
    }
    /* get the next word from the current buffer     */
    if (index_type & EXACTKEY)
      word_ptr = NULL;
    else
      word_ptr = strtok_r (NULL, breakletters, &next_tok);

    word_offset = buffer_offset + (int)(word_ptr - databuf);

  } while (word_ptr != NULL);  
  
  FREE(databuf);
  return(0);
}



/* the following function takes an SGML_data structure and an attribute name */
/* and returns the value for the attribute.                                  */
char *get_attr_value (SGML_Data *data, char * name)
{
  SGML_Attribute_Data *a;
  int casesensitive;

  if (data == NULL) return (NULL);
  if (data->element && data->element->start_tag_min == -1)
    casesensitive = 1;
  else
    casesensitive = 0;

  for (a = data->attributes; a ; a = a->next_attribute_data) {
    if (casesensitive) {
      if (strcmp(a->name, name) == 0
	  || match(name, a->name, casesensitive)) {
	return(strdup(a->string_val));
      }
    }
    else {
      if (strcasecmp(a->name, name) == 0
	  || match(name, a->name, casesensitive)) {
	return(strdup(a->string_val));
      }
    }
  }
  return (NULL);
}





