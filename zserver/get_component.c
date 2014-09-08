/*
 *  Copyright (c) 1990-2000 [see Other Notes, below]. The Regents of the
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
*	Header Name:	get_component.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Get information about components based on the 
*                       component ID and component info structure.
*                       
*	Usage:		int get_component_record(component_id, component_info)
*       
*	Date:		11/1/2000
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2000.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"

extern DB *cf_component_open(char *filename, char *componentname);

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

component_data_item *
get_component_record(int component_id, component_list_entry *comp)
{
  int component_num, returncode;
  DBT keyval;
  DBT dataval;
  component_data_item *return_data;
  
  if (comp == NULL)
    return (NULL);

  if (comp->comp_db == NULL) {
    /* open up the component info index */
    comp->comp_db = cf_component_open(comp->config_info->nickname, 
				      comp->name);
    if (comp->comp_db == NULL)
      return(NULL);
  }

  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));

  component_num = component_id;
  keyval.data = (void *)&(component_num);
  keyval.size = sizeof(int);

  /* now we search the component ID in the component index file */
  returncode = comp->comp_db->get(comp->comp_db, NULL, &keyval, &dataval, 0);

  if (returncode == 0) { /* found it */

    return_data = CALLOC(component_data_item, 1);

    memcpy((char *)return_data, (char *)dataval.data, dataval.size);

    return(return_data);

  } 
  else { /* nothing found in the component file -- return 0 */
    fprintf(LOGFILE, "Non-zero return (%d) in get of componenent ID %d from %s\n", 
	    returncode, component_id, comp->name);
    return(NULL);
  }
}


SGML_Data *
find_component_start(int start_offset, SGML_Document *in_doc,
		     component_list_entry *comp)
{
  SGML_Data *result = NULL;
  SGML_Tag_Data *td, *tag_data;
  int casesensitive;

  if (in_doc == NULL)
    return (NULL);

  if (comp == NULL)
    return (NULL);

  
  if (in_doc->DTD->type > 0)
    casesensitive = 1;
  else
    casesensitive = 0;

  /* check for display directive to include only the first match */

  tag_data = comp_tag_list(comp->start_tag, 
			   in_doc->Tag_hash_tab, NULL, casesensitive);

  for (td = tag_data; td ; td = td->next_tag_data) {
    /* we have a match -- add it to the list */
    /* THIS HAS BEEN KLUDGED BECAUSE SOME DAMN HUGE RECORDS DON"T REPARSE */
    /* WITH EXACTLY THE SAME OFFSETS AND I DON"T F(*&%^%$&% KNOW WHY      */
    if (start_offset == td->item->start_tag_offset
	|| start_offset == (td->item->start_tag_offset + 1)
	|| start_offset == (td->item->start_tag_offset - 1))
      result = td->item;
    if (result) break;
  }
  /* free the tag data */
  free_tag_data_list(tag_data);
  return (result);
}


SGML_Data *
get_component_data(int component_id,  component_list_entry *comp, 
		   SGML_Document **doc)
{
  component_data_item *component_data;
  SGML_Document *d, *GetSGML();
  SGML_Data *start_data, *end_data, *new_data;
  char *new_buf;
  
  if (comp == NULL)
    return (NULL);
  
  component_data = get_component_record(component_id, comp);

  if (component_data == NULL)
    return (NULL);

  d = *doc;

  /* looks like we have a component item -- get the SGML */
  if (*doc != NULL && d->record_id == component_data->record_id) {
    /* same record as the last time -- leave it */
  }
  else {
    if (*doc) {
      free_doc(*doc);
    }
    *doc = GetSGML(comp->config_info->nickname, component_data->record_id);
    d = *doc;
  }
  /* We traverse the data looking for the SGML_Data entry where the */
  /* component starts                                               */
  start_data = find_component_start(component_data->start_offset, 
				    d, comp);
  /* duplicate the SGML_data */
  new_data = CALLOC(SGML_Data, 1);

  *new_data = *start_data;

  if (comp->end_tag == NULL) {
    new_data->next_data_element = NULL;
  }
  else {
    /* set the end pointers to the end offset */
    new_data->content_end_offset = component_data->end_offset;
    new_data->end_tag_offset = component_data->end_offset;
    new_data->data_end_offset = component_data->end_offset;
    /* we leave the correct pointers but change the offsets...
       new_data->content_end = (char *)(d->buffer 
       + component_data->end_offset);
       new_data->end_tag = (char *)(d->buffer
       + component_data->end_offset);
       new_data->data_end = (char *)(d->buffer 
       + component_data->end_offset);
    */

  }

  return (new_data);
}






