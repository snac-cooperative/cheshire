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

extern char *parse_date(char *databuf, char *patptr);
extern char *in_proc_instr(SGML_Data *dat, char *in_buffer, 
		    int buff_offset, idx_list_entry *idx);

extern char *parse_latlong(char *databuf, char *patptr);

extern int ToLowerCase(char *data);

extern char *snowball_stem(char *inword, int langcode);

extern char *wn_morph(char *word);


/* forward defs */
char *conv_diacritics (char *data, int index_type, idx_list_entry *idx);
char *normalize_classnum(char *classnum);
char *trim_xkey(char *key);

int FIXONLY = 0; /* this flag will only be set if index_fix is being run */

char *
normalize_key(char *raw_word, idx_list_entry *idx, int *morphflag, int dia_flag)
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
  
  if (stoplist_hash == NULL)
    entry = NULL;
  else
    entry = Tcl_FindHashEntry(stoplist_hash, srchbuffer);
  if (entry == NULL) { /* not a stopword */
   
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
      result_word = srchbuffer; /* error or unchanged, return it */
      *morphflag = 0;
    }
    else if (index_type & SSTEMS) {
      /* This index wants to have simple plural "S" Stemming done      */
      /* the stemmed word is left is srchbuffer, errors in stemming    */
      /* such as numbers in the word, leave the original contents of   */
      /* of srchbuffer as they were.                                   */
      sstem(srchbuffer);
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


/* the following function takes an SGML_data structure and turns everything  */
/* in it into a string allocated string after removing any imbedded SGML tags*/
/* It has the added effect of removing diacritics (of the ALA/LC character   */
/* set). We substitute romanized versions of selected non-roman non-ascii    */
/* characters.                                                               */

char *detag_data_block (SGML_Data *data, int index_type, idx_list_entry *idx)
{
  char *c, t, *databuf=NULL, *orig_databuf=NULL;
  int buffsize, i;
  unsigned char u;
  int contains_non_roman;

  if (data == NULL) return (NULL);

  if (idx == NULL && index_type == 0) {
    index_type = index_type | NORM_NOMAP;
  }

  buffsize = (long)data->content_end - (long)data->content_start;

  if (buffsize <= 0) return (NULL);

  /* calloc should initialize the databuf to nulls */
  /* we add extra space for char substitutions     */
  orig_databuf = CALLOC(char, buffsize + 10); 
  databuf = CALLOC(char, buffsize + 40); 
  strncpy(orig_databuf, data->content_start, buffsize);

  /* printf ("buffsize=%d\n",buffsize+40); */

  if ((index_type & NORM_DO_NOTHING) == NORM_DO_NOTHING) {
    FREE(databuf);
    return(orig_databuf);
  }

  /* if there are processing instructions associated with this data */
  /* then do them on the substituted buffer                         */
  if (data->processing_flags) {
    orig_databuf = in_proc_instr(data, orig_databuf, 
				 data->content_start_offset, idx);
  } 


  /* the following has potential problems when there is a literal */
  /* 'greater than' sign in the data -- this SHOULD be avoided by */
  /* using entity references for the > symbol                     */
  contains_non_roman = 0 ;

  for (i = 0, c = orig_databuf; *c ; c++, i++) { 
    t = u = *c;

    /* if processing instructions were handled check for  the following */
    if (data->processing_flags) {

      if (t == '\001' && *(c+1) == '\002' && *(c+2) == '\003') {
	/* special flag from processing instruction handling */
	/* to close up any gap of blanks in the data         */
	/* blanks, etc preceding these characters should have*/
	/* been removed already                              */
	c += 3;
	while (*c == ' ') c++;
	t = u = *c;
      }
      else if (t == '\001') {
	/* if there are extraneous 001's in the data, skip them */
	c+=1;
	t = u = *c;
      }
    }

    /* treat CDATA sections as data, and ignore any "tags" */
    if (strncmp(c,"<![CDATA[",9) == 0) {
      while (*c != '\0' && strncmp(c,"]]>",3) != 0)
	databuf[i++] = *c++;
      i--;
      continue;
    }

    /* in copying stuff skip over imbedded tags */
    if (t == '<') {
      /* squash out the remaining tags (not handled by proc instr) */
      while (t != '>' && *c ) {
	c++ ;
	t = *c;
	if (index_type & PROXIMITY) {
	  /* need to replace with spaces if this is a prox index */
	        databuf[i++] = ' ';
	}
	if (t == '>' && *(c+1) == '<') {
	  /* if immediately followed by another tag keep going... */
	  c++ ;
	  t = *c;
	  databuf[i++] = ' '; /* insert a space -- just in case */
	}

      }
      if (index_type & PROXIMITY) {
	/* need to replace with spaces if this is a prox index */
	t = ' ';
      } 
      else { /* step over to the next char */
	c++;
	t = *c;
      }
      
    }
#ifndef WIN32
    if (t == '\r') {
      /* squash out the carriage returns */
      while (t == '\r' && *c ) {
	c++ ;
	t = *c;
	if (index_type & PROXIMITY) {
	        databuf[i++] = ' ';
	}

      }
    }
#endif
    if ((u > 128) && ((index_type & (NORM_NOMAP | NORM_MIN)) == 0)) {
      if (u < 192) { /* Might be ALA */
	contains_non_roman = 1;
	switch (u) {
	case 161: t = 'L';
	  break;
	case 162: t = 'O';
	  break;
	case 163: t = 'D';
	  break;
	case 164: 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 165: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 166: 
	  databuf[i++] = 'O';
	  t = 'E';
	  break;
	case 167: t = '_';
	  break;
	case 168: t = '_';
	  break;
	case 169: t = '_';
	  break;
	case 170: t = '_';
	  break;
	case 171: t = '_';
	  break;
	case 172: t = 'O';
	  break;
	case 173: t = 'U';
	  break;
	case 174: t = '_';
	  break;
	case 175: t = '_';
	  break;
	case 176: t = '_';
	  break;
	case 177: t = 'l';
	  break;
	case 178: t = 'o';
	  break;
	case 179: t = 'd';
	  break;
	case 180: 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 181: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 182: 
	  databuf[i++] = 'o';
	  t = 'e';
	  break;
	case 183: t = '_';
	  break;
	case 184: t = 'i';
	  break;
	case 185: t = '_';
	  break;
	case 186: t = 'd'; /* this is eth  fixed now */
	  break;
	case 187: t = '_';
	  break;
	case 188: t = 'o';
	  break;
	case 189: t = 'u';
	  break;
	}
	databuf[i] = t;
      }
      else if (u > 191) {
	/* assume these are IS0 8859-1 (Latin-1) */
	switch (u) {
	case 192:
	case 193:
	case 194:
	case 195:
	case 196:
	case 197: t = 'A';
	  break;
	case 198: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 199: t = 'C';
	  break;
	case 200:
	case 201:
	case 202:
	case 203: t = 'E';
	  break;
	case 204:
	case 205:
	case 206:
	case 207: t = 'I';
	  break;
	case 208: /*Eth*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 209: t = 'N';
	  break;
	case 210:
	case 211:
	case 212:
	case 214:
	case 216: t = 'O';
	  break;
	case 215: t = ' ';
	  break;
	case 217:
	case 218:
	case 219:
	case 220: t = 'U';
	  break;
	case 221: t = 'Y';
	  break;
	case 222: /*Thorn*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 223: /*ss*/ 
	  databuf[i++] = 's';
	  t = 's';
	  break;
	case 224:
	case 225:
	case 226:
	case 227:
	case 228:
	case 229: t = 'a';
	  break;
	case 230: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 231: t = 'c';
	  break;
	case 232:
	case 233:
	case 234:
	case 235: t = 'e';
	  break;
	case 236:
	case 237:
	case 238:
	case 239: t = 'i';
	  break;
	case 240: /*Eth*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 241: t = 'n';
	  break;
	case 242:
	case 243:
	case 244:
	case 245:
	case 246:
	case 248: t = 'o';
	  break;
	case 247: t = ' ';
	  break;
	case 250:
	case 251:
	case 252: t = 'u';
	  break;
	case 253:
	case 255: t = 'Y';
	  break;
	case 254: /*thorn*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	}
	databuf[i] = t;
      } 
      else { /* although these are non-roman -- they are already handled */
	/* if it isn't in the range -- ignor it and re-use the position */ 
	i--; 
      }
    }
    else {
      if (i > buffsize+40) {
	continue;
	/* printf ("ERROR IN DETAG_DATABLOCK: Try NOMAP options: i=%d\n",i); */
      }
      databuf[i] = t;
    }
  }

  /* The following is a special fix (that should no longer be needed) */
  /* for adding ONLY index items with non-roman substituted characters*/
  if (FIXONLY == 1) {
    if (contains_non_roman == 1) {
      FREE(orig_databuf);
      return (databuf);
    }
    else { /* ignor normal words */
      FREE(databuf);
      FREE(orig_databuf);
      return (NULL);
    }
  }

  /* if FIXONLY is NOT on return the buffer */
  FREE(orig_databuf);
  return (databuf);
}

/* the following is pretty much identical to above, but is intended to */
/* process strings instead of SGML_data. and only convert diacritics */

char *conv_diacritics (char *data, int index_type, idx_list_entry *idx)
{
  char *c, t, *databuf, *orig_databuf;
  int buffsize, i;
  unsigned char u;
  int contains_non_roman;

  if (data == NULL) return (NULL);

  buffsize = strlen(data);

  /* calloc should initialize the databuf to nulls */
  /* we add extra space for char substitutions     */
  orig_databuf = strdup(data);

  if ((index_type & NORM_DO_NOTHING) == NORM_DO_NOTHING) {
    return(orig_databuf);
  }

  databuf = CALLOC(char, buffsize + 40); 

  /* the following has potential problems when there is a literal */
  /* 'greater than' sign in the data -- this SHOULD be avoided by */
  /* using entity references for the > symbol                     */
  contains_non_roman = 0 ;

  for (i = 0, c = orig_databuf; *c ; c++, i++) { 
    t = u = *c;

#ifndef WIN32
    if (t == '\r') {
      /* squash out the carriage returns */
      while (t == '\r' && *c ) {
	c++ ;
	t = *c;
      }
    }
#endif
    if (u > 128 && ((index_type & (NORM_NOMAP | NORM_MIN)) == 0)) {
      if (u < 223) { /* might be substitutable */
	contains_non_roman = 1;
	switch (u) {
	case 161: t = 'L';
	  break;
	case 162: t = 'O';
	  break;
	case 163: t = 'D';
	  break;
	case 164: 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 165: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 166: 
	  databuf[i++] = 'O';
	  t = 'E';
	  break;
	case 167: t = '_';
	  break;
	case 168: t = '_';
	  break;
	case 169: t = '_';
	  break;
	case 170: t = '_';
	  break;
	case 171: t = '_';
	  break;
	case 172: t = 'O';
	  break;
	case 173: t = 'U';
	  break;
	case 174: t = '_';
	  break;
	case 175: t = '_';
	  break;
	case 176: t = '_';
	  break;
	case 177: t = 'l';
	  break;
	case 178: t = 'o';
	  break;
	case 179: t = 'd';
	  break;
	case 180: 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 181: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 182: 
	  databuf[i++] = 'o';
	  t = 'e';
	  break;
	case 183: t = '_';
	  break;
	case 184: t = 'i';
	  break;
	case 185: t = '_';
	  break;
	case 186: t = 'd'; /* this is eth  fixed now */
	  break;
	case 187: t = '_';
	  break;
	case 188: t = 'o';
	  break;
	case 189: t = 'u';
	  break;
	}
	databuf[i] = t;
      }
      else if (u > 191) {
	/* assume these are IS0 8859-1 (Latin-1) */
	switch (u) {
	case 192:
	case 193:
	case 194:
	case 195:
	case 196:
	case 197: t = 'A';
	  break;
	case 198: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 199: t = 'C';
	  break;
	case 200:
	case 201:
	case 202:
	case 203: t = 'E';
	  break;
	case 204:
	case 205:
	case 206:
	case 207: t = 'I';
	  break;
	case 208: /*Eth*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 209: t = 'N';
	  break;
	case 210:
	case 211:
	case 212:
	case 214:
	case 216: t = 'O';
	  break;
	case 215: t = ' ';
	  break;
	case 217:
	case 218:
	case 219:
	case 220: t = 'U';
	  break;
	case 221: t = 'Y';
	  break;
	case 222: /*Thorn*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 223: /*ss*/ 
	  databuf[i++] = 's';
	  t = 's';
	  break;
	case 224:
	case 225:
	case 226:
	case 227:
	case 228:
	case 229: t = 'a';
	  break;
	case 230: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 231: t = 'c';
	  break;
	case 232:
	case 233:
	case 234:
	case 235: t = 'e';
	  break;
	case 236:
	case 237:
	case 238:
	case 239: t = 'i';
	  break;
	case 240: /*Eth*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 241: t = 'n';
	  break;
	case 242:
	case 243:
	case 244:
	case 245:
	case 246:
	case 248: t = 'o';
	  break;
	case 247: t = ' ';
	  break;
	case 250:
	case 251:
	case 252: t = 'u';
	  break;
	case 253:
	case 255: t = 'Y';
	  break;
	case 254: /*thorn*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	}
	databuf[i] = t;
      } 
      else { /* although these are non-roman -- they are already handled */
	/* if it isn't in the range -- ignor it and re-use the position */ 
	i--; 
      }
    }
    else
      databuf[i] = t;
  }

  FREE(orig_databuf);
  return (databuf);
}

/* The following routine (adapted from old marclib), normalizes LC */
/* Class numbers. The input buffer should contain ONLY the $a topic*/
/* portion of a class number.                                      */
char *normalize_classnum(char *in)
{
  char mainclass[4], mainsub[10];
  char decimal[10], subcutter[12];
  int  valmainsub, ofs, decflag;
  char sep;
  char *out;
  
  for (ofs = 0; ofs < sizeof mainclass - 1 && isalpha(*in); ofs++, in++)
    *(mainclass+ofs) = islower(*in) ? toupper(*in):*in;
  if (ofs && ofs < sizeof mainclass)
    *(mainclass+ofs) = '\0';
  else
    return(NULL);

  while (*in && isspace(*in))
    in++;                    /* skip blanks */

  for (ofs = 0; ofs < sizeof mainsub - 1 && isdigit(*in); ofs++, in++)
    *(mainsub+ofs) = *in;
  if (ofs && ofs < sizeof mainsub)
    *(mainsub+ofs) = '\0';
  else
    return(NULL);
  valmainsub = atoi(mainsub);

  while (*in && isspace(*in))
    in++;               /* skip blanks */

  if (*in == '\0') {
    decflag = 0;
    decimal[0] = '\0';
    subcutter[0] = '\0';
  }
  else {                      /* still more of the call number */
    if (*in == '.' && isdigit(*(in+1))) {
      decflag = 1;
      in++;
    }
    else
      decflag = 0;

    decimal[0] = '\0';
    if (decflag) {
      while(*in && isspace(*in))
	in++;          /* skip blanks */
      for (ofs = 0; ofs < sizeof decimal - 1 && isdigit(*in); ofs++, in++)
	*(decimal+ofs) = *in;
      if (ofs < sizeof decimal)
	*(decimal+ofs) = '\0';
      else
	return(NULL);
    }

    /* this could be changed to separate successive cutters */
    while (*in && isspace(*in))
      in++;               /* skip blanks */
    if (*in == '.' && isalnum(*(in+1)))
      in++;
    for (ofs = 0; ofs < sizeof subcutter - 1 && isalnum(*in); ofs++, in++)
      *(subcutter+ofs) = islower(*in) ? toupper(*in):*in;
    if (ofs < sizeof subcutter)
      *(subcutter+ofs) = '\0';
    else
      return(NULL);
  }

  decflag ? (sep = '.') : (sep = ' ');

  /* It seems that we have processed the class number OK, so build */
  /* the output normalized class number.                           */
  if ((out = CALLOC(char,26)) == NULL) {
    fprintf(LOGFILE, 
	    "Normalize_key: couldn't allocate normalized class number\n");
    return (NULL);
  }
  sprintf(out, "%-3s%05d%c%-5s %-10s", mainclass,
    valmainsub, sep, decimal, subcutter);
  return(out);
}


char *trim_xkey(char *key) {

  char *next_tok, *word_ptr;
  char *current_buffer;
  int test;

  current_buffer = CALLOC(char,strlen(key)+1);


  /* We will tokenize the string and remove stopwords, punctuation, etc. */

  word_ptr = strtok_r (key, 
		       " \t\n\r\v\f`~!@#$%^&*()_=|\\{[]};:'\",<.>?/", &next_tok);
    
  if (word_ptr) {
    do {
      /* normalize the word according to the specs for this index */
      /* (as defined in indextype) SHOULD be EXACTKEY             */
      test = *word_ptr;
      if (*current_buffer != '\0')
	strcat(current_buffer, " ");
      strcat(current_buffer, word_ptr);
      
      /* get the next word from the current buffer     */
      word_ptr = strtok_r (NULL, 
			   " \t\n\r\v\f`~!@#$%^&*()_+=|\\{[]};:'\",<.>?/", &next_tok);
    } while (word_ptr != NULL && *word_ptr != 0);  
  
  strcpy(key,current_buffer);
  }
  FREE(current_buffer);
  return(key);

}

