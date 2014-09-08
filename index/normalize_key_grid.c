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
*	Header Name:  Normalize_key
*
*	Programmer:   Ray R. Larson
*                     Jerome P. McDonough
*
*	Purpose:      normalizing the form of keywords for storage in
*                     an index, this uses the wordnet normalization
*
*	Usage:	      normed_key = normalize_key(raw_word,idx,flag1, flag2)
*
*	Variables:    raw_word is the key to be normalized.
*                     idx is the index entry
*                     flag1 is a pointer to an integer to indicate when
*                          certain operations have been performed on the
*                          key.
*                     flag2 says whether to convert diacritics
*
*	Return Conditions and Return Codes: NULL when stopword
*
*	Date:		10/4/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "string.h"
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "grid_defs.h"


extern char *parse_date(char *databuf, char *patptr);
extern char *in_proc_instr(SGML_Data *dat, char *in_buffer, 
		    int buff_offset, idx_list_entry *idx);

extern char *parse_latlong(char *databuf, char *patptr);

extern int ToLowerCase(char *data);

extern char *snowball_stem(char *inword, int langcode);

extern char *wn_morph(char *word);

extern char *conv_diacritics (char *data, int index_type, idx_list_entry *idx);
extern char *normalize_classnum(char *classnum);
extern char *trim_xkey(char *key);

extern int in_grid_data(SGML_Document *sgml_rec, SGML_Data *sgml_dat, int which_file, char *data, int token_offset, idx_list_entry *idx);

extern char *detag_data_block (SGML_Data *data, int index_type, idx_list_entry *idx);

char *
normalize_key_grid(char *raw_word, idx_list_entry *idx, int *morphflag, int dia_flag, SGML_Document *sgml_rec, SGML_Data *data, int offset)
{
  char *srchbuffer;
  int bufflen, i;
  Tcl_HashEntry *entry;
  char *result_word;
  char *c, *d;
  char *morph_word;
  extern int last_rec_proc;
  int index_type;
  Tcl_HashTable *stoplist_hash;
  char *date_format;
  int scanresult;
  int integer_key;
  int decimal_part;
  double double_key;
  int stop_numbers = 0;


  if (idx == NULL) return NULL;

  if (idx->stopwords_hash.buckets == NULL)
    stoplist_hash = NULL;
  else {
    stoplist_hash = &idx->stopwords_hash;
    entry = Tcl_FindHashEntry(stoplist_hash, "#NONUMBERS#");
    if (entry != NULL) { /* removal of numbers requested */
      stop_numbers = 1;
    }
    
  }
  index_type = idx->type;
  date_format = idx->dateformat;

  if (raw_word == NULL) {
    fprintf(LOGFILE,"Null word in normalize_key: record #%d\n",
	    last_rec_proc+1);
    return NULL;
  }
  

  if ((idx->type & NORM_DO_NOTHING) == NORM_DO_NOTHING) {
    return(strdup(raw_word));
  }

  if ((idx->type & NORM_MIN) == NORM_MIN) {
    return(strdup(raw_word));
  }


  bufflen = strlen(raw_word);
  
  c = raw_word;

  if (stop_numbers) {
    while (*c== '+' || *c == '-' || *c == ' ' || *c == '\n' || *c == '\t' 
	   || *c == '.' || *c == '0' || *c == '1' || *c == '2' || *c == '3'
	   || *c == '4' || *c == '5' || *c == '6' || *c == '7' || *c == '8'
	   || *c == '9' || *c == '\r')
      c++;
    
  }
  
  if ((idx->type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) == 0) {
    while (*c== '+' || *c == '-' || *c == ' ' || *c == '\n' || *c == '\r' || *c == '\t' || *c == '.')
      c++;
  }
  else { /* the key is one of the numeric key types, so create the key */
    
    switch (idx->type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) {
    case INTEGER_KEY:
      scanresult = sscanf(raw_word,"%d", &integer_key);
      if (scanresult == 0)
	srchbuffer = NULL;
      else {
	srchbuffer = CALLOC(char, 11);
	sprintf(srchbuffer,"%010d", integer_key);
      }
      break;
      
    case DECIMAL_KEY:
      integer_key = 0;
      decimal_part = 0;
      scanresult = sscanf(raw_word,"%d.%d", &integer_key, &decimal_part);
      
      if (scanresult == 0)
	srchbuffer = NULL;
      else {
	srchbuffer = CALLOC(char, 30);
	sprintf(srchbuffer,"%010d.%d", integer_key, decimal_part);
      }
      break;
      
    case FLOAT_KEY:
      scanresult = sscanf(raw_word,"%g", &double_key);
      if (scanresult == 0)
	srchbuffer = NULL;
      else {
	srchbuffer = CALLOC(char, 30);
	sprintf(srchbuffer,"%016.6f", double_key);
      }
      break;
    }
    /* if it is numeric we are finished */
    return (srchbuffer);
  }

  /* The text was entirely hyphens and whitespace or periods (or numbers)*/
  if (*c == '\0' || c == raw_word + bufflen)
    return (NULL);

  /* the following should be common to all the normalization methods */
  
  if (dia_flag) 
    srchbuffer = conv_diacritics (c, idx->type, idx);
  else {
    srchbuffer = CALLOC(char, bufflen+1);
    strcpy(srchbuffer,c);
    srchbuffer[bufflen] = '\0';
    bufflen = strlen(srchbuffer);
  }
  
  /* trim trailing junk */
  for (d = srchbuffer+(bufflen-1); *d == ' ' || *d == '.' || *d == '-'; d--)
    *d = '\0';

  ToLowerCase(srchbuffer);

  /* For CLEF-GRID output the lowercase token at this point */
  in_grid_data(sgml_rec, data,GRID_LOWERNORM, srchbuffer, offset, idx);
  
  if (stoplist_hash == NULL)
    entry = NULL;
  else
    entry = Tcl_FindHashEntry(stoplist_hash, srchbuffer);
  if (entry == NULL) { /* not a stopword */

    /* For CLEF-GRID output the lowercase token at this point */
    in_grid_data(sgml_rec, data,GRID_STOP, srchbuffer, offset, idx);
   
    if (index_type & WORDNET) {
      /* This index wants to have WordNet Morphing done on the words */  
      morph_word = wn_morph(srchbuffer);
      if ((morph_word == NULL) || strcmp(morph_word,srchbuffer) == 0) 
	result_word = srchbuffer; /* i.e., add "new" words to the index */
      else 
	result_word = morph_word; /* or add the normalized word */
      /* pass back the fact that this was morphed */
      *morphflag = 1;
    }
    else if (index_type & STEMS) {
      /* This index wants to have Stemming done (using Porter stemmer) */
      /* the stemmed word is left is srchbuffer, errors in stemming    */
      /* such as numbers in the word, leave the original contents of   */
      /* of srchbuffer as they were.                                   */
      Stem(srchbuffer);
      /* For CLEF-GRID output the stemmed word at this point */
      in_grid_data(sgml_rec, data,GRID_STEM, srchbuffer, offset, idx);

      result_word = srchbuffer; /* error or unchanged, return it */
      *morphflag = 0;
    }
    else if (index_type & SSTEMS) {
      /* This index wants to have simple plural "S" Stemming done      */
      /* the stemmed word is left is srchbuffer, errors in stemming    */
      /* such as numbers in the word, leave the original contents of   */
      /* of srchbuffer as they were.                                   */
      sstem(srchbuffer);
      /* For CLEF-GRID output the stemmed word at this point */
      in_grid_data(sgml_rec, data,GRID_STEM, srchbuffer, offset, idx);

      result_word = srchbuffer; /* error or unchanged, return it */
      *morphflag = 0;
    }
    else if (index_type & SNOWBALL_STEMS) {
      /* This index wants to have language-specific stemming done.     */
      /* the stemmed word is left is srchbuffer, errors in stemming    */
      /* such as numbers in the word, leave the original contents of   */
      /* of srchbuffer as they were.                                   */
      /* language code to apply is in idx->snowball_stem_type          */
      result_word = snowball_stem(srchbuffer, idx->snowball_stem_type);
      /* For CLEF-GRID output the stemmed word at this point */
      in_grid_data(sgml_rec, data,GRID_STEM, srchbuffer, offset, idx);

      if (result_word != srchbuffer)
	FREE(srchbuffer);
     
      *morphflag = 0;
    }
    else if (index_type & CLASSCLUS) {
      /* this index had better be an LC class number, and the key should */
      /* be just the $a portion of the class number. Anything else, like */
      /* additional <tags> will be stripped out and a fixed length key   */
      /* will be returned.                                               */

      result_word = normalize_classnum(srchbuffer);
      if (result_word == NULL) {
	if ((result_word = CALLOC(char,26)) == NULL) 
	  fprintf(LOGFILE, 
		"Normalize_key: couldn't allocate normalized class number\n");
	else {
	  /* we create a dummy key for abnormal call numbers using whatever */
	  /* is in the field and padding it with blanks                     */
	  strncat(result_word,srchbuffer,25) ;
	  result_word[25] = '\0';
	  for (i = 0; i < (int)strlen(result_word); i++) 
	    result_word[i] = toupper(result_word[i]);

	  for (i = strlen(result_word); i < 25; i++) result_word[i] = ' '; 
	}
      }
      FREE(srchbuffer);
      *morphflag = 0;
    }
    else if (index_type & EXACTKEY) {
      if (strchr(srchbuffer,' ')) { /* this is multword buffer */
	result_word = trim_xkey(srchbuffer);
      }
      else 
	result_word = srchbuffer;
      *morphflag = 0;
    }
    else if (index_type & DATE_KEY || index_type & DATE_RANGE_KEY) {
      /* this is a date index that requires special normalization */
      result_word = parse_date(srchbuffer, date_format);
      *morphflag = 0;

    }
    else if (index_type & LAT_LONG_KEY || index_type & BOUNDING_BOX_KEY) {
      /* this is a geographic index that requires special normalization */
      result_word = parse_latlong(srchbuffer, date_format);
      *morphflag = 0;

    }
    else {/* This index doesn't want any normalization             */
          /* so just send back the lower-case version of the input */
      result_word = srchbuffer;
      *morphflag = 0;
    }
    /* send back any results (including NULLs) */
    return (result_word);
  }
  else {/* word was in the stopword table ... */
    if (srchbuffer) FREE(srchbuffer);
    return (NULL);
  }
}


