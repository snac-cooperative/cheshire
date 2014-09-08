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
*	Header Name:  idxnumber.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing formatted numbers in an SGML field.
*
*	Usage:	      idxdate(data, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
*
*	Return Conditions and Return Codes:	
*
*	Date:		09/21/98
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1998.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"
#include "ctype.h"


char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);

int 
idxnumber(SGML_Data *data,
	  Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  char *end_buffer, *patptr, *date_key;
  int scanresult;
  int type;
  int *exists;
  int integer_key;
  int decimal_part;
  double double_key;

  char buffer[50];


  HT_DATA *ht_data;

  databuf = detag_data_block(data, idx->type, idx);

  if (databuf == NULL) return(0);

  if (strlen(databuf) == 0) {
    FREE(databuf);
    return (0);
  }

  /* create the hash table structure */
  ht_data = CALLOC(struct hash_term_data, 1) ;
  ht_data->termid = -1;
  ht_data->termfreq = 1;

  switch (idx->type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) {
  case INTEGER_KEY:
    scanresult = sscanf(databuf,"%d", &integer_key);
    if (scanresult == 0) {
      FREE(databuf);
      return (0);
    }
    else
      sprintf(buffer,"%010d", integer_key);
    break;

  case DECIMAL_KEY:
    integer_key = 0;
    decimal_part = 0;
    scanresult = sscanf(databuf,"%d.%d", &integer_key, &decimal_part);
    
    if (scanresult == 0) {
      FREE(databuf);
      return (0);
    }
    else
      sprintf(buffer,"%010d.%d", integer_key, decimal_part);
    break;

  case FLOAT_KEY:
    scanresult = sscanf(databuf,"%g", &double_key);
    if (scanresult == 0) {
      FREE(databuf);
      return (0);
    }
    else
      sprintf(buffer,"%016.6f", double_key);
    break;
  }

  
  

  /* put it into the hash table */
  Tcl_SetHashValue(
		   Tcl_CreateHashEntry(hash_tab,
				       buffer, &exists), 
		   ht_data);
  
  
  FREE(databuf);
  return(0);
}

