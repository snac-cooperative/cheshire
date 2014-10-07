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
*	Header Name:	in_tag_data.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to handle extracting data from SGML documents
*
*	Usage:	
*                       
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		1/5/2001
*                       
*	Version:	1.0
*	Copyright (c) 2001.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <string.h>
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "ht_info.h"
#ifdef WIN32
#define strcasecmp _stricmp
#endif

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);


/* free up a tag list (but not the SGML data) */
void
free_tag_data_list(SGML_Tag_Data *in_tags) 
{
  /* we do NOT free the attached SGML, only the linking structures */
  if (in_tags == NULL)
    return ;

  free_tag_data_list(in_tags->next_tag_data);
  
  FREE(in_tags);

}

/* extract data and put it in buffer -- appending from multiple calls */
int get_key_data( SGML_Data *data, char **current_buffer)
{
  
  char *databuf;
  char *nlptr;
  int i;

  databuf = NULL;

  if (data == NULL) return(0);

  if (data != NULL) {
    databuf = detag_data_block(data, 0, NULL); /* part of indexing code */
    /* note that index_type is passed as zero -- characters will be mapped */
    if (databuf == NULL)
      return (0);

    for (i = 0; i < strlen(databuf); i++) {
      if (databuf[i] != ' '
	  && databuf[i] != '\n'
	  && databuf[i] != '\r'
	  && databuf[i] != '\t') break;
    }
    
    if (i == strlen(databuf)) {
      FREE(databuf);
      return (0);
    }
    
    if (*current_buffer == NULL) {
      *current_buffer = CALLOC(char,strlen(databuf)+1);
    }
    else {
      int newlen;
      char *tmp;
      /* append to the current buffer (add a space and the new data) */
      newlen = strlen(*current_buffer) + strlen(databuf) + 4;
      tmp = *current_buffer;
      *current_buffer = CALLOC(char, newlen);
      strcat(*current_buffer, tmp);
      FREE(tmp);
    }
    /* append the word to the current buffer */
    if (**current_buffer != '\0')
      strcat(*current_buffer, " ");
    strcat(*current_buffer, databuf);
  }
  
  /* remove newlines from the buffer */
  for (nlptr = strchr(*current_buffer, '\n'); nlptr ; nlptr = strchr(nlptr, '\n')) {
    *nlptr = ' ';
  }

  for (nlptr = strchr(*current_buffer, '\r'); nlptr ; nlptr = strchr(nlptr, '\r')) {
    *nlptr = ' ';
  }


  /* databuf was allocated in detagging the raw sgml */
  if (databuf != NULL) 
    FREE(databuf);

  return(0);
  
}

int
count_tag_data(SGML_Tag_Data *in_tags) 
{
  SGML_Tag_Data *td;
  int count;

  count = 0;

  if (in_tags == NULL)
    return (0);

  for (td = in_tags; td ; td = td->next_tag_data) {
    count++;
  }
  return (count);
}


/* this reverses the order of the tag list while duplicating */
SGML_Tag_Data *
dup_tag_data(SGML_Tag_Data *in_tags, SGML_Tag_Data **finaltd) 
{
  SGML_Tag_Data *newtagdata, *td, *lasttd;
  
  *finaltd = NULL;

  if (in_tags == NULL)
    return (NULL);

  lasttd = NULL;

  for (td = in_tags; td ; td = td->next_tag_data) {
    newtagdata = CALLOC(SGML_Tag_Data, 1);
    newtagdata->item = td->item;
    newtagdata->next_tag_data = lasttd;
    if (lasttd == NULL)
      *finaltd = newtagdata;
    lasttd = newtagdata;
  }
  return (newtagdata);
}

SGML_Tag_Data *
exclude_tag_data( SGML_Tag_Data *exclude_tags, 
		  SGML_Tag_Data *data_tags) 
{
  SGML_Tag_Data *ed, *td, *lasttd, *firsttd, *freetd;
  
  if (exclude_tags == NULL)
    return (data_tags);

  td = lasttd = freetd = NULL;

  firsttd = data_tags;

  for (td = data_tags; td ; td = td->next_tag_data) {
    
    /* if we changed a link, remove the old linking data */
    if (freetd) {
      FREE(freetd);
      freetd = NULL;
    }

    for (ed = exclude_tags; ed; ed = ed->next_tag_data) {
      if (td->item->start_tag_offset > ed->item->start_tag_offset
	  && td->item->end_tag_offset < ed->item->end_tag_offset) {
	/* the tag is within the bounds of the excluded tag */
	/* so change the next link... */
	if (lasttd == NULL)
	  firsttd = td->next_tag_data;
	else 
	  lasttd->next_tag_data = td->next_tag_data;
	/* and remove the old link data */
	freetd = td;
	break; /* once we have one match we can stop -- it is excluded */
      }
    }
    /* if we didn't remove a link, set the lasttd to the current */
    /* otherwise it remains the last.                            */
    if (freetd == NULL)
      lasttd = td;
  }

  return (firsttd);

}

SGML_Tag_Data *
restrict_occurrence(SGML_Tag_Data *in_data, int occurrence_qual, 
		    int *free_flag) 
{
  SGML_Tag_Data *td, *lasttd, *firsttd, *newtagdata;

  if (occurrence_qual == 0) 
    return(in_data);

  /* we DO free some tag data here -- anything not matching */
  td = lasttd = firsttd = NULL;
    
  for (td = in_data; td ; td = td->next_tag_data) {
      
    if (td->item->occurrence_no == occurrence_qual) {
      /* add it to the new data */
      newtagdata = CALLOC(SGML_Tag_Data, 1);
      newtagdata->item = td->item;
      newtagdata->next_tag_data = NULL;
      if (lasttd == NULL)
	firsttd = lasttd = newtagdata;
      else {
	lasttd->next_tag_data = newtagdata;
	lasttd = newtagdata;
      }
    }
  }

  if (*free_flag == 1) {
    /* free the input data */
    free_tag_data_list(in_data);
  }

  /* set up to free the new data when finished */
  *free_flag = 1;
  
  return (firsttd);
}




SGML_Tag_Data *
included_tag_data( SGML_Tag_Data *parent_tags, 
		  SGML_Tag_Data *data_tags) 
{
  SGML_Tag_Data *ed, *td, *lasttd, *firsttd, *newtagdata;
  /* we don't free any tag data here, that has to be done elsewhere */
  
  if (parent_tags == NULL)
    return (data_tags);
  
  td = lasttd = firsttd = NULL;
    
  for (ed = parent_tags; ed; ed = ed->next_tag_data) {
    
    for (td = data_tags; td ; td = td->next_tag_data) {
      
      if (td->item->start_tag_offset >= ed->item->start_tag_offset
	  && td->item->end_tag_offset <= ed->item->end_tag_offset) {
	/* the tag is within the bounds of the parent tag */    
	/* if we changed a link, remove the old linking data */
	/* so change the next link... */
	newtagdata = CALLOC(SGML_Tag_Data, 1);
	newtagdata->item = td->item;
	newtagdata->next_tag_data = NULL;
	if (lasttd == NULL)
	  firsttd = lasttd = newtagdata;
	else {
	  lasttd->next_tag_data = newtagdata;
	  lasttd = newtagdata;
	}
      }
    }
  }
  return (firsttd);
}



SGML_Tag_Data *
comp_tag_list(idx_key *start_key, Tcl_HashTable *tag_hash,
	      SGML_Tag_Data *parent_tags, int casesensitive)
{
  Tcl_HashEntry *comp_entry, *tag_entry;
  Tcl_HashSearch tag_hash_search;
  SGML_Tag_Data *tag_data=NULL, *tag_data2=NULL,*new_tag_data=NULL, 
    *pat_tag_data=NULL;
  SGML_Tag_Data *newtd, *td, *td2, *lasttd, *firsttd, *prevtd;
  SGML_Data *parent, *child;
  char *attribute_key, *attribute_value;
  char *tag_ptr, *tag_name;
  int datasize;
  char *tempdata;
  int free_flag = 0;
  int free_tag_data2 = 0;
  int i;
  int count;
  char *tmp;
  int occurrence_qual;
  
  new_tag_data = tag_data = pat_tag_data = NULL;
  
  if (start_key == NULL) {
    return (NULL);
  }

  if (tag_hash == NULL) {
    fprintf(LOGFILE, "NULL Document Tag Hash Table in comp_tag_list (in_components.c)\n"); 
    return (NULL);
  }

  
  tmp = strchr(start_key->key, ':'); /* Namespace no longer in hash table */
  if (tmp != NULL) {
    tmp++;
    if (strpbrk(start_key->key, "[]^$?*\\.(){}|") != NULL) {
      /* have to assume it is part of a pattern */
      tag_ptr = strdup(start_key->key);
    }
    else 
      tag_ptr = strdup(tmp);
  }
  else
    tag_ptr = strdup(start_key->key);

  occurrence_qual = start_key->occurrence;

  if (strpbrk(start_key->key, "[]^$?*\\.(){}|") != NULL) {

    firsttd = lasttd = prevtd = NULL;
    
    for (tag_entry = Tcl_FirstHashEntry(tag_hash,&tag_hash_search); 
	 tag_entry != NULL; 
	 tag_entry = Tcl_NextHashEntry(&tag_hash_search)) {
      /* get the tag from the hash table indexes */
      tag_name = Tcl_GetHashKey(tag_hash,tag_entry); 
      
      /* compare the tag with the key pattern */
      if (match(tag_ptr, tag_name, casesensitive)) {
	/* tag_data is the original hash table structure and should NOT  */
	/* be returned directly -- all or part of the list is duplicated */
	new_tag_data =  dup_tag_data((struct SGML_Tag_Data *) Tcl_GetHashValue(tag_entry), &lasttd);
	/* if this is the first data ... */
	if (firsttd == NULL)
	  firsttd = new_tag_data;
	/* or attach to the end of a previous list */
	if (prevtd)
	  prevtd->next_tag_data = new_tag_data;
	/* point prevtd to the end of the new list */
	prevtd = lasttd;
	/* we will need to free this eventually */
	free_flag = 1;
      }
    }
    tag_data = firsttd;
  }
  else {  /* the easy case -- no regexp for the tag */

    if (casesensitive == 0) {
      /* all tags in the hash table are upper case, so... */
      for (i = 0; i < (int)strlen(tag_ptr); i++) {
	tag_ptr[i] = toupper(tag_ptr[i]);
      }
    }

    comp_entry = Tcl_FindHashEntry(tag_hash,tag_ptr);
    
    if (comp_entry == NULL) { /* Tag not found in table */
      FREE(tag_ptr);
      return (NULL);
    }
    else {
      /* tag_data is the original hash table structure and should NOT be */
      /* returned directly -- all or part of the list is duplicated      */
      tag_data =  (struct SGML_Tag_Data *) Tcl_GetHashValue(comp_entry);
      free_flag = 0;
      /* printf("exact tag match for %s count is %d\n", 
       *     tag_ptr, count_tag_data(tag_data));
       */
    }
  }
  
  FREE(tag_ptr);

  /* restrict the list to those that match the occurrence -- if necessary */
  if (occurrence_qual) {
    tag_data = restrict_occurrence(tag_data,occurrence_qual, &free_flag);
  }

  if (tag_data == NULL)
    return (NULL);

  /* We have a matching tag? */
    
  /* go through sub-elements... Attribute flag 0 means not attribute*/
  /* attribute_flag 5 means the subkey is for value testing */
  if (start_key->subkey != NULL 
      && (start_key->subkey->attribute_flag == 0
	  || start_key->subkey->attribute_flag == 5)) {
    /* pass in only the tags included in the parent data as "parent" */
    if (parent_tags) {
      tag_data2= included_tag_data(parent_tags,tag_data);

      if (tag_data2 != NULL)
	new_tag_data = comp_tag_list(start_key->subkey, tag_hash, 
				     tag_data2, casesensitive);
      free_tag_data_list(tag_data2);
    }
    else {
      new_tag_data = comp_tag_list(start_key->subkey, tag_hash, tag_data,
				   casesensitive);
    }
    /*
     * printf("parent limited count is %d\n", count_tag_data(new_tag_data));
     */
    /* no matching tags? */
    if (new_tag_data == NULL)
      return (NULL);
  }
  else {
    /* no more subkeys, only attributes */
    if (start_key->subkey != NULL 
	&& start_key->subkey->attribute_flag !=0
	&& start_key->subkey->attribute_flag != 7 
	  && start_key->subkey->attribute_flag != 6) {
      /* attribute specifications: we only deal with 1,2 and 6,7 */
      /* check the attribute specifications */
      attribute_key = attribute_value = NULL;
      
      if (start_key->subkey->attribute_flag == 2) {
	attribute_value = start_key->subkey->key;
	/* check the data for matching contents */
	td = td2 = lasttd = firsttd = newtd = NULL;
	
	/* loop over the parents and check */
	for (td2 = parent_tags; td2 ; td2 = td2->next_tag_data) {
	  parent = td2->item;

	  for (td = tag_data; td ; td = td->next_tag_data) {
	    child = td->item;
	    if (child->start_tag_offset >= parent->start_tag_offset
		&& child->end_tag_offset <= parent->end_tag_offset) {
	      /* the child tag is within the bounds of the parent */
	      /* so now check if there is a match with the value  */
	      
	      /* copy the data into a temp space */
	      tempdata = NULL;
	      datasize = child->content_end_offset 
		- child->content_start_offset;
	      tempdata = CALLOC(char, datasize + 1);
	      strncpy(tempdata, child->content_start, 
		      child->content_end_offset - 
		      child->content_start_offset);

	      if (match(attribute_value, tempdata, casesensitive)) {
		/* OK, we have a matching data value */
		/* so extract the field....          */
		newtd = CALLOC(SGML_Tag_Data, 1);
		
		newtd->item = td->item;
		
		if (lasttd == NULL) {
		  /* first item */
		  lasttd = firsttd = newtd;
		}
		else {
		  lasttd->next_tag_data = newtd;
		  lasttd = newtd;
		}
	      }
	      if (tempdata != NULL)
		FREE(tempdata);
	    }
	  }
	}
	/* we should now have a new list with only the things that have */
	/* matching attributes (and values)                             */
	new_tag_data = firsttd;
	  
      } 
      else {
	/* need to match for attributes or tags */
	if (start_key->subkey->attribute_flag == 1)
	  attribute_key = start_key->subkey->key;
	
	if (start_key->subkey->subkey == NULL &&
	    (strcasecmp("TEXT_FILE_REF", start_key->subkey->key) == 0
	     || strcasecmp("EXTERNAL_URL_REF", start_key->subkey->key) == 0
	     || strcasecmp("TEXT_DIRECTORY_REF",start_key->subkey->key) == 0
	     || strcasecmp("PAGED_DIRECTORY_REF", start_key->subkey->key) == 0)) {
	  /* we have an external reference in the data, so pass back the */
	  /* current tag list                                            */
	  new_tag_data = dup_tag_data(tag_data, &lasttd);
	  return (new_tag_data);
	}

	
	if (start_key->subkey->subkey == NULL
	    && start_key->subkey->attribute_flag == 1) {
	  /* extract and index the attribute itself */

	  td = td2 = lasttd = firsttd = newtd = NULL;

	  if (parent_tags) 
	    td2 = included_tag_data(parent_tags,tag_data);
	  else
	    td2 = tag_data;
	  
	  for (td = td2; td ; td = td->next_tag_data) {
	    SGML_Attribute_Data *ad;
	    
	    for (ad = td->item->attributes; ad; ad = ad->next_attribute_data) {
	      
	      if ((casesensitive == 0 && 
		   (strcasecmp(ad->name, attribute_key) == 0))
		  || (casesensitive == 1 && 
		      (strcmp(ad->name, attribute_key) == 0))
		  || match(attribute_key, ad->name, casesensitive)) {
		
		newtd = CALLOC(SGML_Tag_Data, 1);
		
		newtd->item = td->item;
		
		if (lasttd == NULL) {
		  /* first item */
		  lasttd = firsttd = newtd;
		}
		else {
		  lasttd->next_tag_data = newtd;
		  lasttd = newtd;
		}
	      }
	    }
	  }
	  /* we should now have a new list with only the things that have */
	  /* matching attributes (and values)                             */
	  new_tag_data = firsttd;
	  if (td2 != tag_data)
	    free_tag_data_list(td2); 
	}
	else if (start_key->subkey->subkey 
	    && start_key->subkey->subkey->attribute_flag == 2) {
	  idx_key *tempkey;
	  
	  td = lasttd = firsttd = newtd = NULL;
	  
	  if (parent_tags) {
	    td2 = included_tag_data(parent_tags,tag_data);
	    if (td2 == NULL)
	      return (NULL);
	  } 
	  else
	    td2 = tag_data;

	  for (tempkey = start_key->subkey->subkey; 
	       tempkey != NULL && tempkey->attribute_flag == 2 ;
	       tempkey = tempkey->subkey) {
	    attribute_value = tempkey->key;
	    
	    for (td = td2; td ; td = td->next_tag_data) {
	      SGML_Attribute_Data *ad;
	      /* check to see if the attribute and value match */
	      
	      if (strcasecmp("EXTERNAL_URL_REF", attribute_value) == 0
		  || strcasecmp("TEXT_FILE_REF", attribute_value) == 0) {
		newtd = CALLOC(SGML_Tag_Data, 1);
		
		newtd->item = td->item;
		
		if (lasttd == NULL) {
		  /* first item */
		  lasttd = firsttd = newtd;
		}
		else {
		  lasttd->next_tag_data = newtd;
		  lasttd = newtd;
		}

	      }
	      else {

		for (ad = td->item->attributes; ad; ad = ad->next_attribute_data) {
		  if ((casesensitive == 0 && 
		       (strcasecmp(ad->name, attribute_key) == 0))
		      || (casesensitive == 1 && 
			  (strcmp(ad->name, attribute_key) == 0))
		      || match(attribute_key, ad->name, casesensitive)) {
		    if ((attribute_value != NULL && 
			 (strcasecmp(ad->string_val, attribute_key) == 0 ||
			  match(attribute_value, ad->string_val, 0))) 
			|| attribute_value == NULL) {
		      newtd = CALLOC(SGML_Tag_Data, 1);
		      
		      newtd->item = td->item;
		      
		      if (lasttd == NULL) {
			/* first item */
			lasttd = firsttd = newtd;
		      }
		      else {
			lasttd->next_tag_data = newtd;
			lasttd = newtd;
		      }
		    }
		  }
		}
	      }
	    }
	  }
	  /* we should now have a new list with only the things that have */
	  /* matching attributes (and values)                             */
	  new_tag_data = firsttd;
	}
      }
    }
    else {
      /* no more subtags -- verify that the new list lies within a parent*/
        
      if (parent_tags == NULL && tag_data != NULL) {
	/* we have direct tag data for the tag (no subkeys extracted */
	new_tag_data = dup_tag_data(tag_data, &lasttd);
      }
      else {
	/* We have parents -- so check containment */
	/* 
	 * printf("*parent(end) count is %d\n", count_tag_data(parent_tags));
	 * printf("*tagdata(end) count is %d\n", count_tag_data(tag_data));
	 */
	new_tag_data = included_tag_data(parent_tags, tag_data);
	/*
	 * printf("parent(end) limited count is %d\n", count_tag_data(new_tag_data));
	 */

      }
    }
  }

  /* if the tag_data is the result of building a new list -- free it */
  if (tag_data != new_tag_data && free_flag == 1)
    free_tag_data_list(tag_data);

  return (new_tag_data);
}








