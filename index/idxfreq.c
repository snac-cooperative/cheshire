/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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
*	Purpose:      Indexing with frequency information
*
*	Usage:	      idxdata(data, stoplist_hash, idx)
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     index_list_entry *idx
*
*	Return Conditions and Return Codes:	
*
*	Date:		08/09/01
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2001.  The Regents of the University of California
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
			   int *morphflag, int diacritic);

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);


char *strtok_freq(char *s1, const char *s2, char **saveptr, 
		  char **freqptr, char **endptr, int exactkey)
{
  char *start;
  int nomatch, match, length;
  char *freq = NULL, *endfreq = NULL;

  if (s1 == NULL) /* scan from saveptr */
    start = *saveptr;
  else 
    start = s1;

  if (*freqptr != NULL && *saveptr == *freqptr)
    *saveptr = start = *endptr;

#ifdef TESTINGTHISSTUFF

  length = strlen(start);

  if (length < 5) /* Can't be the minimal {a 1} */
      return (NULL);
#endif

  nomatch = strspn(start,s2);

  if (nomatch == -1)
    return (NULL);

  start += nomatch;

  if (*start == '{') {
    /* set the end and freq_value */
    endfreq = strchr(start, '}');
    if (endfreq == NULL)
      return(NULL);
    *endfreq++ = '\0';

    freq = strrchr(start, ' ');
    if (freq == NULL)
      return (NULL);
    *freq++ = '\0';
    start++;
    *freqptr = freq;
    *endptr = endfreq;
  }
  else {
    /* badly formed freq */
    return (NULL);
  }
  
  if (exactkey == 0) {
    match = strcspn(start,s2);
    if (match == -1)
      return NULL;

    /* if (match == length)
     * *saveptr = start + match;
     *else
     * *saveptr = start + match + 1;
     */
    start[match] = '\0';
    *saveptr = start + match + 1;
  }
  else {/* exact key */
    if (*start == '\0')
      return (NULL);

    *saveptr = *freqptr;
  }

  return (start);
  
}



int idxfreq(SGML_Data *data, int recnum, idx_list_entry *idx,
	    batch_files_info *bf)
{
  
  char *word_ptr, *next_tok, *result_word, *hyphen, *c, *c2;
  char *freqinfo;
  char *next_data;
  int freqdata;
  char *databuf;
  int buffsize;
  int morphed = 0;
  int exists;
  int buffer_offset, word_offset;
  Tcl_HashEntry *entry;
  Tcl_HashTable *stoplist_hash;
  DB *index_db;
  int index_type;
  char *breakletters = " \t\n\r\a\b\v\f<>";
  int xkey;

  HT_DATA *ht_data;
  POSDATA *posdata;
  

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;

  if (index_type & EXACTKEY)
    xkey = 1;
  else
    xkey = 0;

  if (index_type & NORM_DO_NOTHING)
    xkey = 2;

  if (index_type & NORM_MIN) 
    xkey = 2;

  freqinfo = NULL;
  next_data = NULL;


  buffsize = (long)data->content_end - (long)data->content_start;

  if (buffsize <= 0) return (NULL);

  databuf = CALLOC(char, buffsize + 40); 
  strncpy(databuf, data->content_start, buffsize);

  if (databuf == NULL) return(0);

  if (strlen(databuf) == 0) return (0);

  buffer_offset =  data->content_start_offset;

  /* find first token */
  word_ptr = strtok_freq (databuf, breakletters, &next_tok, 
			  &freqinfo, &next_data, xkey);
  /* a buffer full of blanks or punct? */
  if (word_ptr == NULL)
    return(0);

  hyphen = NULL;

  do {
    char *tempwordptr;
    /* normalize the word according to the specs for this index */
    /* (as defined in indextype)                                */
    tempwordptr = word_ptr;
    word_offset = buffer_offset + (int)(word_ptr - databuf);

    freqdata = atoi(freqinfo);
    /* diacritics already converted by detag ... */
    result_word = normalize_key(word_ptr, idx, &morphed, 0);

    /* NULL means a stop word or other problem, so skip it */
    if (result_word != NULL) {
      char *tempptr;

      for (tempptr = strchr(result_word, (int)'\001');
	   tempptr != NULL; tempptr = strchr(result_word, (int)'\001')) {
	*tempptr = '?';
      }
      
      fprintf(bf->outfile, "%s\t%012d\t%d\n", word_ptr, recnum, 
	      freqdata);
      
    }
    /* if the last word was hyphenated get the second part */
    if (hyphen != NULL && idx->type & PRIMARYKEY == 0) {
      word_ptr = hyphen;
      hyphen = NULL;
    }
    else if ((hyphen = strchr(word_ptr, '-')) != NULL
	     && idx->type & PRIMARYKEY == 0) {
      /* if the word is hyphenated double extract without hyphens */
      *hyphen = '\0';
      hyphen++;
    }
    else
    /* get the next word from the current buffer     */
    word_ptr = strtok_freq (NULL, breakletters, &next_tok, 
			  &freqinfo, &next_data, xkey);

  } while (word_ptr != NULL);  
  
  FREE(databuf);
  return(0);
}







