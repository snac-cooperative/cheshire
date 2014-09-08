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
*	Header Name:	sgml_free_dtd.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_DTD structure
*                       and frees all the memory and data associated with
*                       it.
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:	        10/29/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "sgml.h"
#include "dmalloc.h"


void free_dtd_list(SGML_DTD *DTD);


/* recursively free all of the DTDs that have been read */
free_all_dtds()
{
  extern SGML_DTD *DTD_list;

  free_sgml_catalog();
  free_dtd_list(DTD_list);

  DTD_list = NULL;

}

free_sgml_catalog()
{
  extern Tcl_HashTable *Current_SGML_Catalog;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  SGML_Catalog  *temp_SGML_Catalog_Entry;
  
  if (Current_SGML_Catalog == NULL)
    return;

  for (entry = Tcl_FirstHashEntry(Current_SGML_Catalog, &hash_search); 
       entry != NULL; 
       entry = Tcl_NextHashEntry(&hash_search)) {
    
    temp_SGML_Catalog_Entry =  (SGML_Catalog *) Tcl_GetHashValue(entry);
    /* Type is not freed -- they are constants */
#ifdef DEBUGCATALOG
    printf("Freeing Catalog entry %s Pubname = %s SysName = %s\n",
			temp_SGML_Catalog_Entry->Type,
			temp_SGML_Catalog_Entry->public_name, 
			temp_SGML_Catalog_Entry->system_name); 
    fflush(stdout);
#endif

    if (temp_SGML_Catalog_Entry->public_name)
      FREE(temp_SGML_Catalog_Entry->public_name);
    if (temp_SGML_Catalog_Entry->system_name)
      FREE(temp_SGML_Catalog_Entry->system_name);
    FREE(temp_SGML_Catalog_Entry);
  }
  Tcl_DeleteHashTable(Current_SGML_Catalog);
  FREE(Current_SGML_Catalog);
}
  

void
free_dtd_list(SGML_DTD *DTD)
{

  if (DTD == NULL) return;


  free_dtd_list(DTD->next_dtd);

  /* All XML Schema elements are to be freed from the primary DTD */
  /* so we do not free them when this is a secondary schema       */
  if (DTD->Elements != NULL && DTD->Elements->secondary_start_flag != 1)
    free_dtd_elements(DTD->Elements);
  
  free_dtd_entities(DTD->Entities);
  
  free_dtd_notation(DTD->Notation);
  
  if (DTD->DTD_name)
    FREE(DTD->DTD_name); 
  
  /* DTD_file_name is freed with the configfile info */
  
  if (DTD->DTD_file_name) { /* this is now allocated in sgml_intrface */
    FREE(DTD->DTD_file_name);
  }

  if (DTD->DTD_public_name) 
    FREE(DTD->DTD_public_name); 

  if (DTD->DTD_system_name) 
    FREE(DTD->DTD_system_name); 
  
  if (DTD->SGML_Element_hash_tab) {
    Tcl_DeleteHashTable(DTD->SGML_Element_hash_tab);
    FREE(DTD->SGML_Element_hash_tab);
  }

  if (DTD->SGML_Entity_hash_tab) {
    Tcl_DeleteHashTable(DTD->SGML_Entity_hash_tab);
    FREE(DTD->SGML_Entity_hash_tab);
  }

  /* this MAY be a problem when multiple DTDs use the same Declaration */
  free_sgml_dcl(DTD->sgml_declaration);

  FREE(DTD);

}

/* recursively free notation list */
free_dtd_notation(SGML_Notation *notat)
{
  if (notat == NULL) return;

  free_dtd_notation(notat->next_notation);

  if (notat->name)
    FREE(notat->name); 
  if (notat->public_name) 
    FREE(notat->public_name); 
  if (notat->system_name) 
    FREE(notat->system_name); 
  FREE(notat);

}
/* recursively free entity list */
free_dtd_entities(SGML_Entity *ent)
{
  if (ent == NULL) return;  

  free_dtd_entities(ent->next_entity);

  if (ent->name)
    FREE(ent->name); 
  if (ent->entity_text_string)
    FREE(ent->entity_text_string); 
  if (ent->public_name) 
    FREE(ent->public_name); 
  if (ent->system_name) 
    FREE(ent->system_name); 
  FREE(ent);
}

/* recursively free data_model sub_element list */
free_dtd_data_model( SGML_Sub_Element_List *mod )
{
   SGML_Sub_Element_List *l;

   if (mod == NULL) return;

   free_dtd_data_model(mod->next_sub_element);

   mod->next_sub_element = NULL;

   if (mod->group) 
     free_dtd_data_model(mod->group);

   mod->group = NULL;

   if(mod->group_repetition)
     FREE(mod->group_repetition);
   
   if (mod->element_name)
     FREE(mod->element_name);

   if (mod->repetition)
     FREE(mod->repetition);

   if (mod->connector)
     FREE(mod->connector);

   FREE(mod);

}


/* recursively free list of elements */
free_dtd_elements(SGML_Element *el)
{
 
   if (el == NULL) return;

   free_dtd_elements(el->next_element);

   if (el->tag)
     FREE(el->tag);
   else 
     free_dtd_tag_list(el->tag_list); 

   free_dtd_data_model(el->data_model);

   if (el->model_rep)
     FREE(el->model_rep);
 
   free_dtd_attribute_list(el->attribute_list);

   free_dtd_tag_list(el->exclusions);
   free_dtd_tag_list(el->inclusions);

   FREE(el);
 }


/* recursively free attribute list */
free_dtd_attribute_list(SGML_Attribute *at)
{
  if (at == NULL) return;

  free_dtd_attribute_list(at->next_attribute);

  if(at->next_attribute == NULL) {
    /* only need to do these once -- they are shared */
    if (at->assoc_tag)
      FREE(at->assoc_tag);

    if (at->assoc_tag_list)
      free_dtd_tag_list(at->assoc_tag_list);
  }

  if (at->name)
    FREE(at->name);

  if (at->value_list)
    free_dtd_value_list(at->value_list);
      
  if (at->default_value) 
    FREE(at->default_value); 
  
  FREE(at);
}

/* free a values list from attributes */
free_dtd_value_list(SGML_Attribute_Value *v)
{
  if (v == NULL) return;

  if (v->next_value)
    return;

  free_dtd_value_list(v->next_value);

  if(v->value) 
    FREE(v->value);

  if (v->connector) 
    FREE(v->connector); 
  
  FREE(v);
}

/* free up a tag list */
free_dtd_tag_list(SGML_Tag_List *tags)
{
  if (tags == NULL) return;

  free_dtd_tag_list(tags->next_tag);

  if (tags->tag)
    FREE(tags->tag); 
  if (tags->connector)
    FREE(tags->connector); 

  FREE(tags);
}


