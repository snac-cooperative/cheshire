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
*                     Jerome P. McDonough
*
*	Purpose:      Indexing all keywords in an SGML field.
*
*	Usage:	      idxdata(data, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
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
#ifndef WIN32
#include <strings.h>
#endif
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"
#include "grid_defs.h"

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics, 
			   SGML_Document *sgml_rec, SGML_Data *data, 
			   int offset);

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);

extern int ToLowerCase(char *data);

extern int in_grid_data(SGML_Document *sgml_rec, SGML_Data *sgml_dat, int which_file, char *data, int token_offset, idx_list_entry *idx);


int idxdata_grid(SGML_Data *data, Tcl_HashTable *hash_tab, idx_list_entry *idx,
		 SGML_Document *sgml_rec)
{
  
  char *word_ptr, *next_tok, *end_buffer, *result_word, *hyphen, *c, *c2;
  char *databuf=NULL, *expansion=NULL, *new_exp=NULL;
  int morphed = 0;
  int exists;
  int buffer_offset, word_offset;
  char *wn_morph();
  Tcl_HashEntry *entry, *expentry;
  Tcl_HashTable *stoplist_hash;
  Tcl_HashTable *expansion_hash=NULL;
  DB *index_db;
  int index_type;
  char *breakletters;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n\r\a\b\v\f<>";
  char *filenameletters = " \t\n\r\a\b\v\f<>";
  char *noletters = " \t\n\r\a\b\v\f";

  HT_DATA *ht_data;
  POSDATA *posdata;
  

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  expansion_hash = &idx->expansion_hash;
  index_type = idx->type;
  index_db = idx->db;

  databuf = detag_data_block(data, idx->type, idx);

  if (databuf == NULL) return(0);

  if (strlen(databuf) == 0) {
    FREE(databuf);
    return (0);
  }

  buffer_offset =  data->content_start_offset;

  end_buffer = databuf + strlen(databuf);

  /* We will tokenize the string and remove stopwords, punctuation, etc. */
  /* But, because we are retaining hyphenated words there needs to be a  */
  /* special step to remove the double hyphens used as an em-dash in     */
  /* subtitles, etc.                                                     */
  c = databuf;
  while ((hyphen = strchr(c,'-')) != NULL) {
    if (*(hyphen+1) == '-') {
      *hyphen = ' ';
      *(hyphen+1) = ' ';
    }
    c = hyphen + 1;
  }

  
  if (index_type & KEYWORD) breakletters = keywordletters; 
  if (index_type & URL_KEY) breakletters = urlletters;
  if (index_type & FILENAME_KEY) breakletters = filenameletters;
  if (index_type & NORM_DO_NOTHING) breakletters = noletters;
  if (index_type & NORM_MIN) breakletters = noletters;


  /* find first token */
  word_ptr = strtok_r (databuf, breakletters, &next_tok);
  /* a buffer full of blanks or punct? */
  if (word_ptr == NULL) {
    FREE(databuf);
    return(0);
  }

  hyphen = NULL;

  do {
    char *tempwordptr;
    /* normalize the word according to the specs for this index */
    /* (as defined in indextype)                                */
    tempwordptr = word_ptr;


    if (new_exp == NULL)
      word_offset = buffer_offset + (int)(word_ptr - databuf);
    else
      word_offset = word_offset; /* for expansions keep the original pos */

    /* For CLEF-GRID output the raw token at this point */
    
    in_grid_data(sgml_rec, data,GRID_TOKENS, word_ptr, word_offset, idx);


    /* diacritics already converted by detag if needed */
    result_word = normalize_key_grid(word_ptr, idx, &morphed, 0, sgml_rec, data, word_offset);

    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL) {
      int result_len;
      char *tempword;

      if ((result_len = strlen(result_word)) > 0) {
	/* We have a normalized word, is it in the hash table? */
	entry = Tcl_FindHashEntry(hash_tab,result_word);
      
	if (entry == NULL) { /* nope, it is new word */
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
	  result_word = NULL;

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
	  result_word = NULL;
	}
      }
      else {
	/* zero lenth result? */
	if (result_word != NULL) { 
	  FREE(result_word);
	  result_word = NULL;
	}
      }
    }
    else {/* log the original word the stemmer stomped */
#ifdef DEBUGIND
      printf("Null stemmer output from %s\n", tempwordptr);
      /* no longer logging these */
      fprintf(LOGFILE, "Null stemmer output from %s\n", tempwordptr);
#endif
    }
    
    /* if the last word was hyphenated get the second part */
    if (hyphen != NULL && (idx->type & PRIMARYKEY) == 0) {
      word_ptr = hyphen;
      hyphen = NULL;
    }
    else if ((hyphen = strchr(word_ptr, '-')) != NULL
	     && (idx->type & PRIMARYKEY) == 0) {
      /* if the word is hyphenated double extract without hyphens */
      *hyphen = '\0';
      hyphen++;
    }
    else if (expansion_hash) { 
      /* there is an expansion table for this index */
      /* clear the working result_word */
      if (result_word && result_word != word_ptr) { 
	FREE(result_word);
	result_word = NULL;
      }

      if (expansion) {
	/* we are already processing an expansion list... */
	word_ptr = strsep(&expansion,' ');
	if (word_ptr == NULL) {
	  /* end of the expansion list, so reset */
	  FREE(new_exp);
	  new_exp = NULL;
	  expansion = NULL;
	  /* not a candidate for expansion, so get the next token...*/
	  word_ptr = strtok_r (NULL, breakletters, &next_tok);
	}
      }
      else { /* don't have a current expansion */ 
	/* check the unstemmed version of the word changed to lowercase */
	for(c = word_ptr; c && *c; c++)
	  *c = tolower(*c);
	
	expentry = Tcl_FindHashEntry(expansion_hash, word_ptr);
	
	if (expentry) { /* A candidate for expansion */
	  expansion = 
	    strdup((char *) Tcl_GetHashValue(expentry));
	  /* printf("Expanding '%s' as '%s'\n", word_ptr, expansion); */
	  new_exp = strsep(&expansion,' ');
    
	  word_ptr = new_exp;
	}
	else {
	  /* no expansion, so get next data word */
	  word_ptr = strtok_r (NULL, breakletters, &next_tok);
	}
      }
    }
    else
    /* get the next word from the current buffer     */
    word_ptr = strtok_r (NULL, breakletters, &next_tok);
  } while (word_ptr != NULL);  
  
  if (databuf)
    FREE(databuf);
  return(0);
}







