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
*	Header Name:	in_extract.c
*
*	Programmer:	Ray Larson
*
*	Purpose:	To obtain data from a tree of SGML_Data structs
*                       for a data struct matching a particular key or
*                       key and subkey. This version calls upon 
*                       get_comp_data in in_components to build the
*                       list of data for each key in the list provided
*                       
*
*	Usage:		in_extract(curdata, buffer, key, ...);
*
*	Variables:	SGML_Data *curdata; Pointer to the root
*                                           in tree of SGML_Data structs
*                       char *buffer; buffer to handle extracted data to index
*                       idx_key *key; list of index key regular expressions
*                                  that we're currently searching for. 
*
*                       ... and other things just being passed along to lower
*                       level routines that do index extraction, etc.
*
*
*	Return Conditions and Return Codes:   1 if successful match, else 0
*
*
*	Date:		12/8/2000
*	Revised:	
*	Version:	2.0
*	Copyright (c) 2000.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>


/* #define DEBUGIND */

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "cheshire.h"
#include "configfiles.h"


extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern SGML_Tag_Data *exclude_tag_data( SGML_Tag_Data *exclude_tags, 
				 SGML_Tag_Data *data_tags);

extern int idxdateattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);


extern int idxdate(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);

extern int idxlatlongattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);


extern int idxlatlong(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx);

extern int idxattr(SGML_Data *data, char *attr_name, Tcl_HashTable *hash_tab,
		   idx_list_entry *idx);

extern int idx008(SGML_Data *data, char *attr_name, Tcl_HashTable *hash_tab,
		  idx_list_entry *idx);

extern int idxextern(SGML_Data *data, Tcl_HashTable *hash_tab,
		     idx_list_entry *idx,
		     int batch_flag, idx_key *replacekey);

extern int idxpages(SGML_Data *data, Tcl_HashTable *hash_tab, 
		    int recnum, idx_list_entry *idx, int batch_flag);

extern int idxdata(SGML_Data *data, Tcl_HashTable *hash_tab, 
		   idx_list_entry *idx);


extern int idxxkey( SGML_Data *data, Tcl_HashTable *hash_tab,
		    idx_list_entry *idx,
		    char **current_buffer, int addkey);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);


/* finally -- the actual program... */

int in_extract(SGML_Document *sgml_rec, char **xkey_buffer, idx_key *in_key,
	       Tcl_HashTable *hash_tab, int recnum, idx_list_entry *idx,
	       SGML_Tag_Data *comptagdata, int batch_flag, batch_files_info *bf)
{
  int match_flag = 0, xkey_topkey = 0;
  idx_key *key, *k;
  idx_key *attrval_subkey;
  idx_key *attr_subkey;
  int free_tag_data_flag = 0;

  SGML_Tag_Data *tag_data, *exclude_data;
  SGML_Tag_Data *td;
  int attribute_flag = 0;
  int casesensitive;

  if (sgml_rec == NULL)
    return (0);
  
  if (sgml_rec->DTD->type > 0)
    casesensitive = 1;
  else
    casesensitive = 0;

  /* call the "comp_tag_list()" function to get the entries  */
  /* matching the exclude key  */
  exclude_data = comp_tag_list(idx->exclude, 
			       sgml_rec->Tag_hash_tab, NULL, casesensitive);


  for (key = in_key; key ; key = key->next_key) {

    attribute_flag = 0;
    attr_subkey = NULL;
    attrval_subkey = NULL;

    for (k = key; k ; k = k->subkey) {
      if (k->attribute_flag == 1 || k->attribute_flag == 5
	  || k->attribute_flag == 6 || k->attribute_flag == 7) {
	attribute_flag = k->attribute_flag;
	attr_subkey = k;
	/* the subkey is the beginning of the value list */
	/* to match within a specific tag                */
	attrval_subkey = k->subkey;
      }
      
    }

    /* get the list of candidate tags */
    if (comptagdata && (idx->type & COMPONENT_INDEX) 
	&& strcmp(key->key, ".*") == 0)
      tag_data = comptagdata; /* this is a span needing special treatment */
    else {
      tag_data = 
	exclude_tag_data(exclude_data, 
			 comp_tag_list(key, sgml_rec->Tag_hash_tab, 
				       comptagdata, casesensitive));
      free_tag_data_flag = 1;
    }
    /*
     * printf("IN_EXTRACT: Output tag count is %d\n",count_tag_data(tag_data));
     */

    for (td = tag_data; td; td = td->next_tag_data) {

      /* check for processing instructions that affect this tag or it's */
      /* contents -- We only look at indexing instructions here         */
      if (sgml_rec->Processing_Inst != NULL) {
	

      }
      
      if (attribute_flag == 1 && attrval_subkey == NULL) {
	/* it is a specification to extract the attribute as the index item */
	if (idx->type & DATE_KEY || idx->type & DATE_RANGE_KEY) 
	  idxdateattr(td->item, attr_subkey->key, hash_tab, idx);
	else if (idx->type & LAT_LONG_KEY || idx->type & BOUNDING_BOX_KEY 
		 || idx->type & GEOTEXT) {
	  idxlatlongattr(td->item, attr_subkey->key, hash_tab, idx);
	}
	else
	  idxattr(td->item, attr_subkey->key, hash_tab, idx);
      }
      else if (idx->type & FLD008_KEY) {
	idx008(td->item, attr_subkey->key, hash_tab, idx);
      }
      else if(idx->type & EXTERNKEY) {
	if (attr_subkey) {
	  if ((strcasecmp("TEXT_FILE_REF",
			  attr_subkey->key) == 0)
	      || (strcasecmp("EXTERNAL_URL_REF",
			     attr_subkey->key) == 0)
	      || (attrval_subkey != NULL 
		  && (strcasecmp("TEXT_FILE_REF",
				 attr_subkey->subkey->key) == 0)
		  || (strcasecmp("EXTERNAL_URL_REF",
				 attr_subkey->subkey->key) == 0))) {
	    idxextern(td->item, hash_tab, idx, 
		      batch_flag, attr_subkey->subkey);
	  }
	  else if ((strcasecmp("TEXT_DIRECTORY_REF",
			       attr_subkey->key)) == 0) {
	    fprintf(LOGFILE,"TEXT_DIRECTORY_REF Not yet supported.\n");
	  }
	  else if ((strcasecmp("PAGED_DIRECTORY_REF",
			       attr_subkey->key)) == 0) {
	    idxpages(td->item, hash_tab, recnum, idx, batch_flag);
	  }
	  else {
	    fprintf(LOGFILE,"EXTERNAL_URL_REF Not yet supported.\n");	    
	  }
	}  else if (idx->type & KEYWORD
		    || idx->type & URL_KEY
		    || idx->type & FILENAME_KEY) { /* NOT external */

	  idxdata(td->item, hash_tab, idx);
	
	}
	else if (idx->type & EXACTKEY) {
	  idxxkey(td->item, hash_tab, idx, xkey_buffer, 0);

	  if (*xkey_buffer != NULL) {
	    idxxkey(NULL, hash_tab, idx, xkey_buffer, 1);
	    FREE(*xkey_buffer);
	    *xkey_buffer = NULL;
	  }
	}
      }
      else if (idx->type & NORM_WITH_FREQ) { /* NOT external */
	idxfreq(td->item, recnum, idx, bf);
      }
      else if (idx->type & KEYWORD 
	       || idx->type & URL_KEY
	       || idx->type & FILENAME_KEY) { /* NOT external */
	idxdata(td->item, hash_tab, idx);
	
      }
      else if (idx->type & EXACTKEY) {
	idxxkey(td->item, hash_tab, idx, xkey_buffer, 0);

	if (*xkey_buffer != NULL) {
	  idxxkey(NULL, hash_tab, idx, xkey_buffer, 1);
	  FREE(*xkey_buffer);
	  *xkey_buffer = NULL;
	}
      }
      else if ((idx->type & DATE_KEY) 
	       || (idx->type & DATE_RANGE_KEY)) {
	/* date type indexing */
	idxdate(td->item, hash_tab, idx);
      }
      else if ((idx->type & LAT_LONG_KEY) 
	       || (idx->type & BOUNDING_BOX_KEY)
	       || (idx->type & GEOTEXT)) {
	/* geographic indexing */
	idxlatlong(td->item, hash_tab, idx);
      }
      else if ((idx->type & INTEGER_KEY) 
	       || (idx->type & DECIMAL_KEY)
	       || (idx->type & FLOAT_KEY)) {
	/* numerical indexing */
	idxnumber(td->item, hash_tab, idx);
      }
      else if (idx->type & BITMAPPED_TYPE && (idx->type2 & BITMAP_CODED_ELEMENT || idx->type2 & BITMAP_CODED_ATTRIBUTE)) {
	/* Existence bitmap coded indexing */
	idxcoded(td->item, hash_tab, idx);
      }
      else if (idx->type2 & NGRAMS) {
	/* NGRAM indexing */
	idxngram(td->item, hash_tab, idx);
      }
    }

    if (free_tag_data_flag)
      free_tag_data_list(tag_data);

  }

  free_tag_data_list(exclude_data);
  return (0);

}







  








