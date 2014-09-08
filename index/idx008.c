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
*	Purpose:      Indexing an element of a MARC 008 field
*
*	Usage:	      idx008(data, attr_name, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*                     NOTE: this stuff will only work with the USMARC DTD.
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
#include <string.h>
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"

extern int idxdatestring (char *databuf, 
			  Tcl_HashTable *hash_tab, idx_list_entry *idx);

int idx008(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  char *databuf, *normalize_008();
  int exists;
  Tcl_HashEntry *entry;
  int keylen;
  Tcl_HashTable *stoplist_hash;
  DB *index_db;
  int index_type;
  int result;

  HT_DATA *ht_data;

  if (idx == NULL) return(0);

  stoplist_hash = &idx->stopwords_hash;
  index_type = idx->type;
  index_db = idx->db;

  databuf = normalize_008(data->content_start, attr_name);

  if ((idx->type & DATE_KEY) || (idx->type & DATE_RANGE_KEY)) {
    result = idxdatestring (databuf, hash_tab,idx);
    FREE(databuf);
    return(result);
  }

  if (databuf == NULL) return(0);

  if ((keylen=strlen(databuf)) == 0) return(0);

  /* ignor all blank keys */
  if ((int)(strspn(databuf, " ")) == keylen) return(0);

  /* We have a normalized word, is it in the hash table? */
  entry = Tcl_FindHashEntry(hash_tab,databuf);
      
  if (entry == NULL){ /* nope, it is new word */
    /* create the hash table structure */
    ht_data = CALLOC(struct hash_term_data, 1) ;
	
    ht_data->termid = -1;
    ht_data->termfreq = 1;
    /* put it into the hash table */
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 databuf, &exists), 
		     (ClientData)ht_data);
  }
  else { /* word already in hash table, so just increment it */
    ht_data =  
      (struct hash_term_data *) Tcl_GetHashValue(entry);
    ht_data->termfreq++;
  }
  
  FREE(databuf);
  return(0);
}

/* the following puts out a tagged version of the 008 fixed fields */
char *normalize_008(char *fielddata, char *field_name)
{
  extern SGML_Document *current_sgml_document;
  SGML_Data *top;
  MARC_008_OVER *m;
  static char line[20];
  int record_is_marc;

  /* the following determines if this is really a MARC record in */
  /* the USMARC DTD form. The current MARC spec has the 008 for  */
  /* all record types identical... but older records may have    */
  /* different forms of the 008. NULL is returned if the record  */
  /* is not MARC                                                 */
  /* have normal 008 structures                                  */
  /* this is obviously hard code that needs to be changed if the */
  /* DTD changes for USMARC                                      */
  record_is_marc = 0;

  top = current_sgml_document->data;

  if (top && top->element) {
    if (strcasecmp("USMARC",top->element->tag) == 0) {
      record_is_marc = 1;
    }
    else
      return (NULL);
  }
  
  if (record_is_marc) {

    m = (MARC_008_OVER *)fielddata;
  
    line[0] = '\0';

    if (strcasecmp(field_name, "008_entry_date") == 0) 
      sprintf(line,"%c%c%c%c%c%c", 
	      m->entry_date[0]/* [6] */,
	      m->entry_date[1]/* [6] */,
	      m->entry_date[2]/* [6] */,
	      m->entry_date[3]/* [6] */,
	      m->entry_date[4]/* [6] */,
	      m->entry_date[5]/* [6] */ );
    
    if (strcasecmp(field_name, "008_date_type") == 0) 
      sprintf(line,"%c",m->date_type);

    if (strcasecmp(field_name, "008_date1") == 0) 
      sprintf(line,"%c%c%c%c", 
	      m->dates[0]/* [8] */,
	      m->dates[1]/* [8] */,
	      m->dates[2]/* [8] */,
	      m->dates[3]/* [8] */ );
    
    if (strcasecmp(field_name, "008_date2") == 0) 
      sprintf(line,"%c%c%c%c", 
	      m->dates[4]/* [8] */,
	      m->dates[5]/* [8] */,
	      m->dates[6]/* [8] */,
	      m->dates[7]/* [8] */ );
    
    if (strcasecmp(field_name, "008_daterange") == 0) { 
      if (m->date_type == 'i' || m->date_type == 'k' || m->date_type == 'm'
	  || m->date_type == 'q' || m->date_type == 'c' || m->date_type == 'd')
	sprintf(line,"%c%c%c%c-%c%c%c%c", 
		m->dates[0]/* [8] */,
		m->dates[1]/* [8] */,
		m->dates[2]/* [8] */,
		m->dates[3]/* [8] */,
		m->dates[4]/* [8] */,
		m->dates[5]/* [8] */,
		m->dates[6]/* [8] */,
		m->dates[7]/* [8] */ );
      else 
	if (m->date_type == 't' || m->date_type == 'u' || m->date_type == 'd'
	    || m->date_type == ' ' || m->date_type == 'p' || m->date_type == 's')
	  sprintf(line,"%c%c%c%c-%c%c%c%c", 
		  m->dates[0]/* [8] */,
		  m->dates[1]/* [8] */,
		  m->dates[2]/* [8] */,
		  m->dates[3]/* [8] */,
		  m->dates[0]/* [8] */,
		  m->dates[1]/* [8] */,
		  m->dates[2]/* [8] */,
		  m->dates[3]/* [8] */ );

    }

    if (strcasecmp(field_name, "008_country_code") == 0) 
      sprintf(line,"%c%c%c", 
	      m->country_code[0]/* [3] */,
	      m->country_code[1]/* [3] */,
	      m->country_code[2]/* [3] */  );
    
    if (strcasecmp(field_name, "008_illus_code") == 0) 
      sprintf(line,"%c%c%c%c", 
	      m->illus_code[0]/* [4] */,
	      m->illus_code[1]/* [4] */,
	      m->illus_code[2]/* [4] */,
	      m->illus_code[3]/* [4] */ );
    
    if (strcasecmp(field_name, "008_intellectual_level") == 0) 
      sprintf(line,"%c", m->intellectual_level);
    
    if (strcasecmp(field_name, "008_form_of_reproduction") == 0) 
      sprintf(line,"%c", m->form_of_reproduction );
    
    if (strcasecmp(field_name, "008_nature_of_contents") == 0) 
      sprintf(line,"%c%c%c%c", 
	      m->nature_of_contents[0]/* [4] */,
	      m->nature_of_contents[1]/* [4] */,
	      m->nature_of_contents[2]/* [4] */,
	      m->nature_of_contents[3]/* [4] */ );
    
    if (strcasecmp(field_name, "008_government_pub_code") == 0) 
      sprintf(line,"%c", m->government_pub_code);
    
    if (strcasecmp(field_name, "008_conference_indicator") == 0) 
      sprintf(line,"%c", m->conference_indicator );
    
    if (strcasecmp(field_name, "008_festschrift_indicator") == 0) 
      sprintf(line,"%c", m->festschrift_indicator);
    
    if (strcasecmp(field_name, "008_index_indicator") == 0) 
      sprintf(line,"%c", m->index_indicator);
    
    if (strcasecmp(field_name, "008_main_entry_in_body") == 0) 
      sprintf(line,"%c", m->main_entry_in_body);
    
    if (strcasecmp(field_name, "008_fiction_indicator") == 0) 
      sprintf(line,"%c", m->fiction_indicator );
    
    if (strcasecmp(field_name, "008_biography_indicator") == 0) 
      sprintf(line,"%c", m->biography_indicator);
    
    if (strcasecmp(field_name, "008_language_code") == 0) 
      sprintf(line,"%c%c%c",
	      m->language_code[0]/* [3] */,
	      m->language_code[1]/* [3] */,
	      m->language_code[2]/* [3] */);
    
    if (strcmp(field_name, "008_modified_record_code") == 0) 
      sprintf(line,"%c", m->modified_record_code);
    
    if (strcasecmp(field_name, "008_cataloging_source") == 0) 
      sprintf(line,"%c", m->cataloging_source );
    
    return(strdup(line));
  }
  else { /* record is NOT in USMARC dtd format */
    return(NULL);
  }
}





