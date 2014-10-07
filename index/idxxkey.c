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
*	Header Name:  idxxkey
*
*	Programmer:   Ray R. Larson
*                     Jerome P. McDonough
*
*	Purpose:      Indexing exact keys made up of one or more SGML fields.
*
*	Usage:	      idxxkey(data, stoplist_hash, hash_tab, 
*                             index_db, current_buffer);
*
*	Variables:    SGMLdata *data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab,
*                     DB *index_db, int index_type, char **currentbuffer;
*
*	Return Conditions and Return Codes:	
*
*	Date:		08/29/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <string.h>
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);



int idxxkey( SGML_Data *data, Tcl_HashTable *hash_tab, idx_list_entry *idx,
	     char **current_buffer, int addkey)
{
  char *word_ptr, *end_buffer, *next_tok, *result_word, *hyphen;
  char *databuf;
  int exists;
  Tcl_HashEntry *entry;
  int morphed;
  int databuf_length;
  int buffer_offset;
  Tcl_HashTable *stoplist_hash;
  DB *index_db;
  int index_type;

  HT_DATA *ht_data;
  POSDATA *posdata;

  databuf = NULL;

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;

  if (data != NULL) {
    databuf = detag_data_block(data, idx->type, idx);
    
    if (databuf == NULL) return(0);

#ifdef DEBUGIND
    printf("databuf in idxxkey: '%s'\n",databuf); 
#endif
    
    if ((databuf_length = strlen(databuf)) == 0) {
      FREE(databuf);
      return(0);
    }
    end_buffer = databuf + databuf_length;
    buffer_offset =  data->content_start_offset;
    
    /* ignor all blank keys */
    if ((int)(strspn(databuf, " \n\t\r\v\f")) == databuf_length) {
      FREE(databuf);
      return(0);
    }
    
    /* There are occasional indexable subfields with no words */
    /* check for a buffer with ONLY punctuation (it does happen) */
    if (databuf_length == 1 && ispunct(*databuf)) {
      FREE(databuf);
      return (0);
    }
    /* if this is a CLASSCLUS key, just normalize it  */
    if (index_type & CLASSCLUS) {
      
      *current_buffer = normalize_key(databuf, idx, &morphed, 0);
      
    }
    else { /* if not a CLASSCLUS key */
      
      if (*current_buffer == NULL) {
	*current_buffer = CALLOC(char,databuf_length+1);
      }
      else {
	/* append to the current buffer (add a space and the new data) */
	*current_buffer = REMALLOC(*current_buffer, 
				   (strlen(*current_buffer) 
				    + databuf_length + 2));
      }
      
      
      /* We will tokenize the string and remove stopwords, punctuation, etc. */
      /* But, because we are retaining hyphenated words there needs to be a  */
      /* special step to remove the double hyphens used as an em-dash in     */
      /* subtitles, etc.                                                     */
      
      if ((idx->type & NORM_DO_NOTHING) == NORM_DO_NOTHING
	  || (idx->type & NORM_MIN) == NORM_MIN) {
	word_ptr = strtok_r (databuf, 
			     " \t\n\r\a\b\v\f", &next_tok);
	
      }
      else {
	while ((hyphen = strchr(databuf,'-')) != NULL && *(hyphen+1) == '-') {
	  *hyphen = ' ';
	  *(hyphen+1) = ' ';
	}
	
	word_ptr = strtok_r (databuf, 
			     " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<.>?/", &next_tok);
      }
      
      if (word_ptr == NULL) {
	FREE(databuf);
	return (0);
      }
      
      do {
	/* normalize the word according to the specs for this index */
	/* (as defined in indextype) SHOULD be EXACTKEY             */
	
	result_word = normalize_key(word_ptr, idx, &morphed, 0);
	
	/* NULL means a stop word or other problem, so skip it */
	if (result_word != NULL) {
	  /* append the word to the current buffer */
	  if (**current_buffer != '\0')
	    strcat(*current_buffer, " ");
	  strcat(*current_buffer, result_word);
	  FREE(result_word);
	}
	
	/* get the next word from the current buffer     */
	if ((idx->type & NORM_DO_NOTHING) == NORM_DO_NOTHING
	    || (idx->type & NORM_MIN) == NORM_MIN)
	  word_ptr = strtok_r (NULL, 
			       " \t\n\r\a\b\v\f", &next_tok);
	else 
	  word_ptr = strtok_r (NULL, 
			       " \t\n\r\a\b\v\f`~!@#$%^&*()_+=|\\{[]};:'\",<.>?/", &next_tok);
	
      } while (word_ptr != NULL);  
    }
  }
  if (addkey && *current_buffer != NULL) {
    /* add the current buffer to the hash table */
      
    entry = Tcl_FindHashEntry(hash_tab,*current_buffer);
      
    if (entry == NULL){ /* nope, it is new key*/
      /* create the hash table structure */
      if (strlen(*current_buffer) != 0) {

	ht_data = CALLOC(struct hash_term_data, 1) ;
      
	ht_data->termid = -1;
	ht_data->termfreq = 1;

	if (index_type & PROXIMITY) {
	  /* add the offset info to the structure */
	  posdata = CALLOC(struct posdata, 1);
	  posdata->position = buffer_offset;
	  ht_data->pos = posdata;
	  ht_data->last = posdata;
	}

	/* put it into the hash table */
	Tcl_SetHashValue(
			 Tcl_CreateHashEntry(hash_tab,
					     *current_buffer, &exists), 
			 (ClientData)ht_data);
      }
    }
    else { /* word already in hash table, so just increment it */
      ht_data =  
	(struct hash_term_data *) Tcl_GetHashValue(entry);
      ht_data->termfreq++;
      if (index_type & PROXIMITY) {
	/* add the offset info to the structure */
	posdata = CALLOC(struct posdata, 1);
	posdata->position = buffer_offset;
	ht_data->last->next = posdata;
	ht_data->last = posdata;
      }
    }
  }
  /* databuf was allocated in detagging the raw sgml */
  if (databuf != NULL) 
    FREE(databuf);
  return(0);  
}



