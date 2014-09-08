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
*	Header Name:	sgml_free_doc.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This module has routines to free up memory
*                       allocated for SGML data during parsing
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7/22/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "sgml.h"
#include "dmalloc.h"

extern SGML_Doclist  *PRS_first_doc;

void free_sgml_data(SGML_Data *dat );
void free_tag_data(SGML_Tag_Data *dat);
void free_sgml_error_data(SGML_Data *dat );

void
free_doc(SGML_Document *doc )
{
  SGML_Doclist *dl;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  if (doc == NULL) return;
	
  /* scan the doclist and NULL out the corresponding doc. */
  for (dl = PRS_first_doc; dl; dl = dl->next_doc) {
    if (doc == dl->doc) {
      dl->doc = NULL;
      break;
    }
  }

  free_sgml_data(doc->data);

  doc->data = NULL;

  if (doc->buffer) {
    FREE(doc->buffer);
    doc->buffer = NULL;
  }
  /* doc file name is part of config data, so not freed here */

  if (doc->Tag_hash_tab != NULL) {
    for (entry = Tcl_FirstHashEntry(doc->Tag_hash_tab,&hash_search); 
	 entry != NULL; 
	 entry = Tcl_NextHashEntry(&hash_search)) {
      free_tag_data((SGML_Tag_Data *) Tcl_GetHashValue(entry));
    }
    Tcl_DeleteHashTable(doc->Tag_hash_tab);
    FREE(doc->Tag_hash_tab);
  }

  FREE(doc);
 
}

void
free_doc_list(SGML_Doclist *DOCS)
{
  if (DOCS == NULL) return;

  if (DOCS == DOCS->next_doc) return;

  free_doc_list(DOCS->next_doc);

  DOCS->next_doc = NULL; /* in case free_doc gets this far in the list */

  free_doc(DOCS->doc);

  FREE(DOCS);
}

void
free_attrib_data(SGML_Attribute_Data *at)
{
  if (at == NULL) return;
  
  free_attrib_data(at->next_attribute_data);

  if (at->name)
    FREE(at->name);

  if (at->string_val)
    FREE(at->string_val);

  FREE(at);
}


void
free_error_doc(SGML_Document *doc )
{

  if (doc == NULL) return;
	
  free_sgml_error_data(doc->data);

  doc->data = NULL;

  if (doc->buffer) {
    FREE(doc->buffer);
    doc->buffer = NULL;
  }

  FREE(doc);
 
}

void
free_sgml_data(SGML_Data *dat )
{
  if (dat == NULL) return;

  free_sgml_data(dat->next_data_element);

  free_sgml_data(dat->sub_data_element);

  free_attrib_data(dat->attributes);

  FREE(dat);
	
}

void
free_tag_data(SGML_Tag_Data *dat)
{
  SGML_Tag_Data *lasttd, *td;

  if (dat == NULL) return;
  /* we used to do this recursively (like below) until a large record  */
  /* in the NT version overflowed the stack -- so this version does it */
  /* iteratively                                                       */
  /* free_tag_data(dat->next_tag_data); */
  /* no need to free the data item -- it is freed elsewhere */
  lasttd = dat;
  
  for (td = dat->next_tag_data; td != NULL; td = td->next_tag_data) {
    if (lasttd)
      FREE(lasttd);
    lasttd = td;
  }

  FREE(lasttd);

}

void
free_sgml_error_data(SGML_Data *dat )
{
  if (dat == NULL) return;

  free_sgml_error_data(dat->next_data_element);

  free_sgml_error_data(dat->sub_data_element);

  free_attrib_data(dat->attributes);

  /* remove names when linking is not done */
  if (dat->content_start == NULL && dat->start_tag != NULL)
    { if (dat->start_tag)
	FREE(dat->start_tag);
      if (dat->end_tag)
	FREE(dat->end_tag);
    }
  
  FREE(dat);
	
}















