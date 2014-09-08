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
*	Header Name:	dump_doc.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_Document structure
*                       and prints out the information in it
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7/31/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "sgml.h"

extern void cheshire_exit (int code);


void
print_sgml_data(SGML_Data *dat )
{
  SGML_Attribute_Data *atd;
  char *c;
  static int nest = 0;

  if (dat == NULL) return;

  if (dat->local_entities != NULL) {
    printf("<%s>", dat->element->tag);
    printf("\n<!-- Local Included Entities from ");
      
    if (dat->local_entities->name != NULL)
      printf("Entity name %s type %d ", 
	     dat->local_entities->name, dat->local_entities->type);
      
    if (dat->local_entities->entity_text_string != NULL)
      printf("string '%s' ", 
	     dat->local_entities->entity_text_string);
    
    if (dat->local_entities->public_name != NULL)
      printf("pub %s ", dat->local_entities->public_name);
    
    if (dat->local_entities->system_name != NULL)
      printf("sys %s", dat->local_entities->system_name); 
    
    printf("-->\n");
    
    if (dat->sub_data_element)
      print_sgml_data(dat->sub_data_element);

    printf("</%s>", dat->element->tag);

    print_sgml_data(dat->next_data_element);
    return;
  } 

	
  /* print the start tag and attributes */
  for (c = dat->start_tag; *c && c < dat->content_start; c++) 
    putchar(*c);
#ifdef TESTATTRIBUTES
  /* verify attributes ... */
  if (dat->attributes != NULL)
    printf("<!-- Attribute verification:\n");
  for (atd = dat->attributes; atd ; atd = atd->next_attribute_data) {
    if (atd->int_val == -1)
      printf("\t%s = '%s'\n",atd->name, atd->string_val);
    else
      printf("\t%s = %d [e.g. '%s']\n",atd->name, atd->int_val,
	     atd->string_val);
  }
  if (dat->attributes != NULL)
    printf("\t End of Attribute verification -->\n");
#endif
		
  if ((dat->processing_flags & PROC_SUBST_TAG)
      || (dat->processing_flags & PROC_INDEX_SUBST_TAG))
    printf("\t<!-- Flagged for TAG substitution processing -->\n");
  if ((dat->processing_flags & PROC_DELETE_TAG)
      || (dat->processing_flags & PROC_INDEX_DELETE_TAG))
    printf("\t<!-- Flagged for TAG deletion processing -->\n");
  if ((dat->processing_flags & PROC_SUBST_ATTR)
      || (dat->processing_flags & PROC_INDEX_SUBST_ATTR))
    printf("\t<!-- Flagged for TAG/ATTRIBUTE substitution processing -->\n");
  /* print the contents -- if it is PCDATA or other DATA */
  if (dat->element->data_type == EL_PCDATA
      || dat->element->data_type == EL_CDATA
      || dat->element->data_type == EL_RCDATA
      || (dat->element->data_type == EL_MIXED 
	  && dat->sub_data_element == NULL))
    for (c = dat->content_start; *c && c < dat->content_end; c++) 
      putchar(*c);
  else if (dat->element->data_type == EL_MIXED) {
    /* Mixed type with imbedded subelements */
    printf("<!-- Mixed data and subelements -- first data is... -->\n");
    for (c = dat->content_start; *c != '<' && c < dat->content_end; c++) 
      putchar(*c);
    printf("<!-- Mixed data and subelements -- first subelement is... -->\n");
  }
  else putchar('\n'); /* just for formatting for output */
		
  if (dat->sub_data_element)
      print_sgml_data(dat->sub_data_element);

  /* print the end tag */
  for (c = dat->end_tag; *c && c < dat->data_end; c++) 
    putchar(*c);
	
  putchar('\n'); /* just for formatting */

  print_sgml_data(dat->next_data_element);

  return;
}

void
dump_doc(SGML_Document *doc )
{
  printf("Document dump: DTD = '%s'\n",  doc->DTD->DTD_name) ;
  printf("Note that not all data is output -- just tag structure and partial data\n");
  /*	uncomment to have a straight buffer dump
  printf("++++++++ BUFFER DUMP +++++++++++++\n%s\n++++++++++++++++++\n", 
	 doc->buffer) ;
  */
	
  if (doc->buffer != NULL && doc->data != NULL) 
    print_sgml_data(doc->data);
  else
    printf("Null buffer or data pointer in dump_doc\n") ;
}


void
dump_doc_list(SGML_Doclist *DOCS)
{
  SGML_Doclist *d;
  SGML_Document *doc; 
  int i = 1;
  

  for ( d = DOCS; d != NULL ; d = d->next_doc) {
  	
    doc = d->doc;
    printf("Document %d: DTD = '%s'\n", i++, doc->DTD->DTD_name) ;
  	
    if (doc->buffer != NULL && doc->data != NULL) 
      print_sgml_data(doc->data);
    else {
      printf("Null buffer or data pointer in dump_doc_list\n");
      cheshire_exit(99);
    }
  }
}



