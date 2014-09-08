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
*	Header Name:  idxcoded.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing existence (or not) of XML/SGML elements or attributes.
*
*	Version:	1.0
*	Copyright (c) 2012.  The Regents of the University of California
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
idxcoded(SGML_Data *data,
	  Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  int *exists;
  char buffer[2] = "1";
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


  /* put it into the hash table - just the char 1 */
  Tcl_SetHashValue(
		   Tcl_CreateHashEntry(hash_tab,
				       buffer, &exists), 
		   ht_data);
  
  
  FREE(databuf);
  return(0);
}

