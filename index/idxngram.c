/*
 *  Copyright (c) 1990-2012 [see Other Notes, below]. The Regents of the
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
*	Purpose:      Indexing all contents of field as ngrams.
*
*	Usage:	      idxngram(data, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
*
*	Return Conditions and Return Codes:	
*
*	Date:		02/08/12
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2012.  The Regents of the University of California
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

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);

extern int ToLowerCase(char *data);


int idxngram(SGML_Data *data, Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *word_ptr, *next_tok, *end_buffer, *result_word, *hyphen, *c, *c2;
  char *databuf=NULL;
  int morphed = 0;
  int exists;
  int buffer_offset, word_offset;
  char *wn_morph();
  Tcl_HashEntry *entry, *expentry;
  Tcl_HashTable *stoplist_hash;
  DB *index_db;
  int index_type;
  int index_type2;
  int ngram_size = 3; /* default to trigrams */
  char *breakletters;
  char *keywordletters = " \t\n\r\a\b\v\f`~!@#$%^&*()_=|\\{[]};:'\",<>?/";
  char *urlletters = " \t\n\r\a\b\v\f<>";
  char *filenameletters = " \t\n\r\a\b\v\f<>";
  char *noletters = " \t\n\r\a\b\v\f";
  char ngram[10] = "";
  HT_DATA *ht_data;
  POSDATA *posdata;
  
  /* The basic treatment of the elements to be indexed is pretty much */
  /* the same as in idxdata. The elements are normalized according to */
  /* the config specifications, and tokenized - but then each of the  */
  /* tokens is segmented into overlapping ngrams before being put in  */
  /* the hash table for accumulating the results                      */

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_type2 = idx->type2;
  index_db = idx->db;

  /* we reuse snowball_stem_type to store ngram size from config */
  if (idx->snowball_stem_type > 1)  {
    /* currently the only options are 3, 4, and 5 */
    ngram_size = idx->snowball_stem_type;
  }
  
  breakletters = keywordletters; 
  
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
    
    /* diacritics already converted by detag if needed */
    result_word = normalize_key(word_ptr, idx, &morphed, 0);
    
    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL) {
      int result_len;
      int i;
      int j;
      
      result_len = strlen(result_word);
      
      for (i=-1; i < result_len; i++) {	
	
	/* now we make the ngrams */
	if (i == -1) {
	  /* create a special start of word ngram */
	  ngram[0] = '_'; 
	  j = 1;
	}
	else j = 0;

	for (; j < ngram_size; j++) {
	  if (j+i <= result_len && result_word[j+i] == '\0') {
	    ngram[j] = '_';
	  }
	  else 
	    if (j+i <= result_len)
	      ngram[j] = result_word[j+i];
	}
	ngram[j] = '\0';
	  

	if ((i+j) <= result_len + 1) {

	  /* We have a ngram, is it in the hash table? */
	  entry = Tcl_FindHashEntry(hash_tab,ngram);
	
	  if (entry == NULL) { /* nope, it is new word */
	    /* create the hash table structure */
	    ht_data = CALLOC(struct hash_term_data, 1) ;
	    ht_data->termid = -1;
	    ht_data->termfreq = 1;
	    /* put it into the hash table */
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(hash_tab,
						 ngram, &exists), 
			     (ClientData)ht_data);
	  }
	  else { /* word already in hash table, so just increment it */
	    ht_data =  
	      (struct hash_term_data *) Tcl_GetHashValue(entry);
	    ht_data->termfreq++;
	  }
	}
      }
      
      FREE(result_word);
      result_word = NULL;     
    }
    /* next data word */
    word_ptr = strtok_r (NULL, breakletters, &next_tok);
    
  } while (word_ptr != NULL);  
  
  if (databuf)
    FREE(databuf);
  return(0);
}







