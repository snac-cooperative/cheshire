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
*	Header Name:  idxextern.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing all keywords in an external text or
*                     html document
*
*	Usage:	      idxextern(data, stoplist_hash, hash_tab,
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

#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"
#ifdef WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#endif

FILE *in_ft_in; /* input file for full-text parsing using the 
                   in_fulltext.flex scanner (generated with -Pin_ft_) */

extern int in_ft_lex();
extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);


FILE *in_fetch_extern(idx_list_entry *idx, char *url) {
  /* this function grabs a URL from the web/ftp or whatever and copies */
  /* them to a temporary file then passes that open file back          */
  FILE *result_file;
  char *percent;
  char *cmdfmt;
  char *cmd;
  int cmdresult;

  /* there should be a command needing substitution in the index entry */
  if (idx->extern_app_name == NULL)
    return (NULL);
  cmdfmt = CALLOC(char, strlen(idx->extern_app_name) + 20);
  sprintf(cmdfmt,"%s > /tmp/externtmp", idx->extern_app_name);
  
  /* now we find the % in the command which should be the beginning of the */
  /* url spot in the command                                               */
  percent = strchr(cmdfmt,'%');
  if (percent == NULL || (strncmp(percent,"%~URL~%",7) != 0 )) {
    fprintf(LOGFILE, "No URL replacement string in EXTERN_APP command: %s",
	    cmdfmt);
    FREE(cmdfmt);
    return (NULL);
  } 
  
  percent++;
  *percent++ = 's';
  *percent++ = ' ';
  *percent++ = ' ';
  *percent++ = ' ';
  *percent++ = ' ';
  *percent++ = ' ';
    
  cmd = CALLOC(char, strlen(cmdfmt) + strlen(url));  
  sprintf(cmd, cmdfmt, url);

  FREE(cmdfmt);
  /* pass it to the system for execution */
  system(cmd);

  result_file = fopen("/tmp/externtmp", "r");

  FREE(cmd);
  return (result_file);
  
}


int idxextern(SGML_Data *data, Tcl_HashTable *hash_tab, 
	      idx_list_entry *idx,
	      int batch_flag, idx_key *replacekey)
{
  
  char *pathbuf, *tempbuf, *firstchar, *word_ptr, *result_word; 
  int morphed = 0;
  int exists, buffsize, tokentype, position ;
  extern char *FULL_TEXT_TOKEN;
  extern int FULL_TEXT_POSITION; 
  char *wn_morph();
  Tcl_HashEntry *entry;
  Tcl_HashTable *stoplist_hash;
  DB *index_db;
  int index_type;
  HT_DATA *ht_data;
  POSDATA *posdata;
  idx_key *attr_key, *k, *sk;
  pathbuf = NULL;
  tempbuf = NULL;
  
  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;


  /* The data should be a file path on the local machine or a URL */
  if (data == NULL) return (0);

  FULL_TEXT_POSITION = 0;

  buffsize = (int)((long)data->content_end - (long)data->content_start);
  if (buffsize == 0) return (0);

  /* calloc should initialize the pathbuf to nulls */
  pathbuf = CALLOC(char, buffsize+2); 

  if (pathbuf == NULL) {
    fprintf(LOGFILE, "Unable to allocate path buffer in idxextern\n");
    exit(1);
  }
  strncpy(pathbuf, data->content_start, buffsize);
  /* pathbuf[buffsize] = '\0'; */

  if (replacekey != NULL) {
    if (replacekey->subkey != NULL) {
      if (replacekey->attribute_flag == 3 
	  && replacekey->subkey->attribute_flag == 4) {
	/* OK, this is a valid replace, so look for the replacekey */
	/* and in the pathbuf and replace it with the subkey value */
	tempbuf = CALLOC(char, buffsize+2+strlen(replacekey->subkey->key)); 
	firstchar = strchr(pathbuf, replacekey->key[0]);
	while (firstchar != NULL) {
	  if (strncmp(firstchar, replacekey->key, strlen(replacekey->key)) 
	      == 0) {
	    /* have a match for the pattern */
	    *firstchar = '\0';
	    strcpy(tempbuf, pathbuf);
	    strcat(tempbuf, replacekey->subkey->key);
	    strcat(tempbuf, (firstchar + strlen(replacekey->key)));
	    firstchar = NULL;
	    FREE(pathbuf);
	    pathbuf = strdup(tempbuf);
	    FREE(tempbuf);
	  }
	  else
	    firstchar = strchr(pathbuf, replacekey->key[0]);
	}
      }
    }
    else {
      SGML_Attribute_Data *attr;
      /* replacekey without a subkey is probably a URL in attribute */
      FREE(pathbuf);
      if (data->attributes == NULL)
	return (0);
      
      buffsize = (int)((long)data->content_start - (long)data->start_tag);
      if (buffsize == 0) 
	return (0);
      
      /* calloc should initialize the pathbuf to nulls */
      pathbuf = CALLOC(char, buffsize+2); 

      if (pathbuf == NULL) {
	fprintf(LOGFILE, "Unable to allocate path buffer in idxextern\n");
	exit(1);
      }
      /* have to find URL or file in the attributes */
      attr_key = NULL;
      for (k = idx->keys; k != NULL; k = k->next_key) {
	for (sk = k->subkey ; sk != NULL; sk = sk->subkey) {
	  if (sk->attribute_flag == 1 
	      && sk->subkey != NULL && sk->subkey->key != NULL
	      && (strcmp("EXTERNAL_URL_REF", sk->subkey->key) == 0
		  || strcmp("TEXT_FILE_REF", sk->subkey->key) == 0)) {
	    attr_key = sk;
	    break;
	  }
	}
	if (attr_key != NULL) 
	  break;
      }
      for (attr = data->attributes; attr != NULL; 
	   attr = attr->next_attribute_data) {
	if (strcasecmp(attr->name, attr_key->key) == 0)
	  strncpy(pathbuf, attr->string_val, buffsize);
      }
      if (*pathbuf == '\0')
	return (0);
    }
  }
  
  /* open the file */
  if ((in_ft_in = fopen(pathbuf,"r")) == NULL) {
    if ((in_ft_in = in_fetch_extern(idx, pathbuf)) == NULL) {
      fprintf(LOGFILE, "Unable to open file '%s' in idxextern\n",pathbuf);
      fprintf(LOGFILE, "and couldn't find or download as URL\n");
      return(0);
    }
  }

  /* The lexical analyzer for full text returns a 1 for a text token */
  /* or a 2 for an sgml/html tag -- it returns 0 at EOF              */
  while (tokentype = in_ft_lex()) {
    
    if (tokentype == 1 || tokentype == 4) {
      /* tokentype 1 is a normal word or a "year" = 19nn or 20nn  */
      /* currently all numbers other than years are ignored       */
      /* normalize the word according to the specs for this index */
      /* (as defined in indextype)                                */
      word_ptr = FULL_TEXT_TOKEN;
      position = FULL_TEXT_POSITION; /* minus length??? */
      /* normalize diacritics if needed */
      result_word = normalize_key(word_ptr, idx, &morphed, 1);
      
      /* NULL means a stop word or other problem, so skip it */
      if (result_word != NULL) {
	int result_len;

	if ((result_len = strlen(result_word)) > 0) {
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
	      posdata->position = FULL_TEXT_POSITION;
	      ht_data->pos = posdata;
	      ht_data->last = posdata;
	    }

	    /* put it into the hash table */
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(hash_tab,
						 result_word, &exists), 
			     (ClientData)ht_data);
	    FREE(result_word);
	    FREE(word_ptr);
	    
	  }
	  else { /* word already in hash table, so just increment it */
	    ht_data =  
	      (struct hash_term_data *) Tcl_GetHashValue(entry);
	    ht_data->termfreq++;
	    if (index_type & PROXIMITY) {
	      /* add the offset info to the structure */
	      posdata = CALLOC(struct posdata, 1);
	      posdata->position = FULL_TEXT_POSITION;
	      ht_data->last->next = posdata;
	      ht_data->last = posdata;
	    }
	    FREE(result_word);
	    FREE(word_ptr);
	  }
	}
	else {/* log the original word the stemmer stomped */
#ifdef DEBUGIND
	  printf("Null stemmer output from %s\n", word_ptr);
	  /* no longer logging these */
	  fprintf(LOGFILE, "Null stemmer output from %s\n", word_ptr);
#endif
	  FREE(result_word);
	  FREE(word_ptr);
	}
      }
      else { /* it is a stopword, so free the word */
	FREE(word_ptr);
      }
    }
    else if (tokentype == 2) { /* an sgml tag */
#ifdef DEBUGIND
      fprintf(LOGFILE, "TAG VALUE: %s\n", FULL_TEXT_TOKEN);
#endif
      FREE(FULL_TEXT_TOKEN);
    }
    else {
      fprintf(LOGFILE, "Unknown token type number %d in idxextern.c\n", tokentype);
    }
  } 
  fclose (in_ft_in);
  free(pathbuf);
  return(0);
}







