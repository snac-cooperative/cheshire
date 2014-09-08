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
*	Header Name:	dump_dtd.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_DTD structure
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
#include "sgml.h"

void print_data_model(  SGML_Sub_Element_List *mod );
extern char *decode_xml_builtin_types(int code);

void
dump_dtd(SGML_DTD *DTD)
{
  SGML_Entity *ent; 
  SGML_Element *el; 
  SGML_Tag_List *tags;
  SGML_Attribute *at;
  SGML_Attribute_Value *v;

  char startmin, endmin;

  if (DTD == NULL)
    return;

  printf("DOCTYPE MATCHED -- Dumping Elements\n"); 
  for ( el = DTD->Elements; el != NULL ; el = el->next_element) {

    if (el->start_tag_min)
      startmin = 'O';
    else 
      startmin = '-';
    if (el->end_tag_min)
      endmin = 'O';
    else
      endmin = '-';

    if (el->tag != NULL)
      printf("Element tag '%s'  min %c %c type %d \n",
	     el->tag, startmin, endmin,
	     el->data_type); 
    else { 
      printf("Element tag list: '( "); 
      for (tags = el->tag_list ; tags ; tags = tags->next_tag) {
	printf("%s ", tags->tag); 
	if (tags->connector)
	  printf("%s ", tags->connector); 
      }
      printf(" )' min %c %c type %d \n",
	     startmin, endmin,
	     el->data_type);      
    }

    printf("\tData Model:\n\t");
    printf("(");
    print_data_model(el->data_model); /* recursively print data_model */
    printf(")");
    if (el->model_rep)
      printf("%s",el->model_rep);
 
    if (el->attribute_list) 
      printf("\n\tAttributes associated:\n"); 
    else 
      printf("\n\n");
    for (at = el->attribute_list; at ; at = at->next_attribute) {
      if (at->assoc_tag != NULL) 
	printf("\ttag '%s' attr name '%s' type %d ",
	       at->assoc_tag, at->name, at->declared_value_type); 
      else if (at->assoc_tag_list) {
	printf("\ttag list: '( "); 
	for (tags = at->assoc_tag_list ; tags ; tags = tags->next_tag) {
	  printf("%s ", tags->tag); 
	  if (tags->connector)
	    printf("%s ", tags->connector); 
	}
	printf(")  attr name '%s' type %d ",
	       at->name, at->declared_value_type); 
      }
      if (at->value_list != NULL) {
	printf("defined values: (");
	for (v = at->value_list; v ; v = v->next_value) {
	  printf("%s ", v->value); 
	  if (v->connector) printf("%s ", v->connector); 
	}
	printf(") ");
      }
      printf("default_type %d",at->default_value_type); 
      if (at->default_value) 
	printf(" default value : %s",at->default_value); 
      printf("\n\n");
    }
  }

  printf("\nDumping Entities\n"); 
  for ( ent = DTD->Entities; ent != NULL; ent = ent->next_entity)  {
    if (ent->name)
      printf("Entity name '%s' ", ent->name); 
    else
      printf("Entity name #DEFAULT ");
    printf("type %d ", ent->type);
    if (ent->entity_text_string)
      printf("string '%s' \n", ent->entity_text_string); 
    if (ent->public_name) 
      printf("pub %s \n",	ent->public_name); 
    if (ent->system_name) 
      printf("sys %s \n",	ent->system_name); 
    if (ent->entity_text_subtype == SYSTEM_IDENT && ent->system_name == NULL)
      printf("SYSTEM \n"); 
  }
}

void
print_data_model(  SGML_Sub_Element_List *mod )
{
   if (mod == NULL) return;

   if (mod->group) {
     printf("( ");
     print_data_model(mod->group);
     printf(" )");
     if(mod->group_repetition)
       printf("%s",mod->group_repetition);
   }
   /* must be an element */
   if (mod->sub_element_type == EL_PCDATA && mod->element_name == NULL 
       && mod->xml_type_name == NULL) {
     printf("#PCDATA");
   }
   else if (mod->sub_element_type == EL_ANY && mod->element_name == NULL
	    && mod->xml_type_name == NULL) {
     printf("#ANY ");
   }
   else if (mod->sub_element_type == EL_NAMED
	    || (mod->xml_type_name != NULL
		&& mod->element_name != NULL)) {
     if (mod->element_name)
       printf("%s", mod->element_name);
     if (mod->repetition)
       printf("%s", mod->repetition);
     printf(" ");
   }
   else if (mod->sub_element_type > 0 && mod->sub_element_type < 100 &&
	    mod->element_name == NULL && mod->xml_type_name == NULL) {
     char *tmp;
     tmp = decode_xml_builtin_types(mod->sub_element_type);
     printf("XMLSchema_Type:%s", tmp);
   }
   if (mod->connector)
     printf("%s ", mod->connector);
   
   print_data_model(mod->next_sub_element);

}





