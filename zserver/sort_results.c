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
*	Header Name:	sort_result.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Sort and merge result sets 
*                       
*	Usage:		SortResults(int *result_set_status, 
*                                   ZSESSION *session, 
*                                   SortRequest *sortRequest, 
*                                   int *result_set_size)
*       
*	Date:		6/9/2001
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2001.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"
#include "z3950_3.h"
#include "list.h"
#include <string.h>

#ifdef WIN32
/* Microsoft doesn't have strncasecmp */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);
extern char *GetInternationalString(InternationalString str);
extern weighted_result *se_get_result_set(char *setname);
extern weighted_result *se_ormerge(weighted_result *left, 
				   weighted_result *right, int addweights);

char *get_sort_key_from_attrlist(SGML_Document *sgmlrec, SGML_Data *comp_data,
				 int docid, 
				 char *filename, 
				 ObjectIdentifier oid,
				 AttributeList *attrlist); 

char *get_sort_key_from_tags(SGML_Document *sgmlrec, SGML_Data *comp_data,
			     int docid, 
			     char *filename, char *tagspec);


extern char *detag_data_block(SGML_Data *data, int index_type, 
			      idx_list_entry *idx);
extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
extern char *parse_date(char *databuf, char *patptr);
extern char *normalize_008(char *fielddata, char *field_name);

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);

extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);


extern component_data_item *get_component_record(int component_id, 
						 component_list_entry *comp);

extern SGML_Data *get_component_data(int component_id,  
				     component_list_entry *comp, 
				     SGML_Document **doc);

#define SORT_SUCCESS 0
#define SORT_PARTIAL 1
#define SORT_FAILURE 2
#define RESULT_EMPTY 1
#define RESULT_INTERIM 2
#define RESULT_UNCHANGED 3
#define RESULT_NONE 4

typedef struct sortkeyentry {
  char *key;
  SortKeySpec *spec;
} SortKeyEntry;

typedef struct ressortkey {
  int docid;
  float doc_weight;
  void *vdb_ptr;
  int orig_rank; 
  char *setname; 
  int numkeys;
  SortKeyEntry keys[1];
} ResultSortKey;


int compare_ResSrt_entries (ResultSortKey **e1, ResultSortKey **e2);

/* the main event... */

int
SortResults(int *result_set_status, ZSESSION *session, 
	    SortRequest *sortRequest, int *result_set_size)
{
  struct inputResultSetNames_List17 *in;
  struct sortSequence_List18 *seq;
  struct SortKeySpec *key_spec[100];
  struct SortKey *sk;
  int number_of_in_sets = 0;
  int number_of_keys = 0;
  ResultSortKey **key_list = NULL;
  char *keydata;
  int dummy;
  int same_name = 0;
  char *setname[100], *final_set_name;
  char *output_set_name;
  weighted_result *result_set[100], *final_set, *in_set, *out_set, *v_set;
  char errormsg[200];
  int i,k;
  SGML_Document *sgmlrec, *GetSGML();    /* SGML record from mainfile */
  SGML_Data *component_data;
  component_data_item *comp_data_item;
  extern int sgml_parse_error;
  int replaceflag = 1;
  char *infilename;


  *result_set_status = 0;
  *result_set_size = 0;

  component_data = NULL;
  comp_data_item = NULL;

  sgmlrec = NULL;

  diagnostic_clear(); /* clear any previous diagnostics */

  if (sortRequest->sortedResultSetName == NULL 
      || sortRequest->sortedResultSetName->length == 0) {
    *result_set_status = RESULT_NONE;
    diagnostic_set(208,0,"Output resultset name missing");
    return (SORT_FAILURE);
  }

  output_set_name = GetInternationalString(sortRequest->sortedResultSetName);

  if (sortRequest->inputResultSetNames == NULL 
      || sortRequest->inputResultSetNames->item == NULL
      || sortRequest->inputResultSetNames->item->length == 0) {
    *result_set_status = RESULT_NONE;
    diagnostic_set(208,0,"Input resultset names missing");
    return (SORT_FAILURE);
  }

  for (in = sortRequest->inputResultSetNames; in; in = in->next) {

    if (number_of_in_sets > 100) {
      diagnostic_set(230,100, "Maximum number (100) of input resultsets exceeded");
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
      return (SORT_FAILURE);
    }
    setname[number_of_in_sets] = GetInternationalString(in->item);
    
    if (strcmp(setname[number_of_in_sets], output_set_name) == 0)
      same_name = 1;
    
    result_set[number_of_in_sets] = 
      se_get_result_set(setname[number_of_in_sets]);
    
    if (result_set[number_of_in_sets] == NULL) {
      /* diagnostic will be set by se_get_result_set */
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
      return (SORT_FAILURE);
    }

    number_of_in_sets++;    
  }

  for (seq = sortRequest->sortSequence; seq; seq = seq->next) {
    
    if (number_of_keys > 100) {
      diagnostic_set(211,100, "Maximum number (100) exceeded");
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
      return (SORT_FAILURE);
    }

    if(seq->item->sortElement->which == e22_datbaseSpecific) {
      diagnostic_set(210,0, "");
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
      return (SORT_FAILURE);
    }

    if (seq->item->sortRelation > 1) {
      /* only handle ascending and descending */
      diagnostic_set(214,0, "Only ASCENDING and DESCENDING supported");
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
      return (SORT_FAILURE);
    }

    key_spec[number_of_keys++] = seq->item;
    
  }

  if (number_of_in_sets == 1) {
    final_set = result_set[0];
    final_set_name = setname[0];
  }
  else {
    /* we merge multiple result sets by doing a Boolean OR on the sets */
    /* This will need to be modified to handle results from multiple   */
    /* databases eventually                                            */
    in_set = NULL;
    out_set = NULL;

    for (i = 1; i < number_of_in_sets; i++) { 
      if (i == 1) {
	in_set = se_ormerge (result_set[0], result_set[i],1);
      }
      else {
	out_set = se_ormerge (in_set, result_set[i],1);

	if (in_set && in_set->setid[0] == '\0') /* not stored set */
	  FREE(in_set);
	in_set = out_set;
      }
    }
    if (out_set == NULL)
      final_set = in_set;
    else
      final_set = out_set;

    final_set_name = "TEMPMERGESETNAME";
  }


  /* build the actual keys and sort */
  key_list = CALLOC(ResultSortKey *, final_set->num_hits+1);
  if (key_list == NULL) goto allocationerror;
  for (i = 0; i < final_set->num_hits; i++) {
    int first_key;

    key_list[i] = (ResultSortKey *) 
      CALLOC (char, (sizeof(ResultSortKey) 
		     + (number_of_keys * sizeof(SortKeyEntry))));
    if (key_list[i] == NULL) goto allocationerror;
    key_list[i]->docid = final_set->entries[i].record_num;
    key_list[i]->doc_weight = final_set->entries[i].weight;
    /* For virtual database result sets there will be a pointer to the */
    /* original result set                                             */
    key_list[i]->vdb_ptr = final_set->entries[i].xtra; 
    key_list[i]->orig_rank = i; 
    key_list[i]->setname = final_set_name; /* pretty useless currently*/
    key_list[i]->numkeys = number_of_keys;
    
    first_key = 1;

    for (k = 0; k < number_of_keys; k++) {
      key_list[i]->keys[k].spec = key_spec[k];
      sk = key_spec[k]->sortElement->u.generic;
     
      switch (sk->which) {

      case e23_sortfield:

	/* Read the record from the sgml file and parse */
	if (first_key) {
	  if ((final_set->result_type & COMPONENT_RESULT) 
	      != COMPONENT_RESULT) {
	    if ((final_set->result_type & VIRTUAL_RESULT) 
		!= VIRTUAL_RESULT) {
	      /* this is a regular document resultset */
	      infilename = final_set->filename;
	      sgmlrec = GetSGML(infilename,key_list[i]->docid);
	    } 
	    else { /* This one is a virtual result */
	      /* which means that we have to get the record from the REAL */
	      /* resultsets                                               */
	      v_set = (weighted_result *) final_set->entries[key_list[i]->orig_rank].xtra; 
	      infilename = v_set->filename;
	      sgmlrec = GetSGML(infilename,key_list[i]->docid);       
	    }
	  }
	  else { /* This is a component result set */
	    /* get the component data */
	    component_data = get_component_data(key_list[i]->docid, 
						final_set->component,
						&sgmlrec);
	    
	  }
	  first_key = 0;
	}
	if (sgmlrec == NULL) { 
	  /* for sorting we just do a message and set the key to NULL */
	  if (sgml_parse_error != 0) {
	    fprintf(LOGFILE, "SORT: SGML parse error for record %d\n",key_list[i]->docid);
	    fflush(LOGFILE);
	  }
	  else {
	    fprintf(LOGFILE, "Sort-Sortfield: error reading record %d errno %d\n",
		    key_list[i]->docid, errno);
	    fflush(LOGFILE);
	  }
	} 
	
	keydata = get_sort_key_from_tags(sgmlrec, component_data,
					 key_list[i]->docid,
					 infilename,
					 sk->u.sortfield->data);

	if (diagnostic_get() != 0 || keydata == NULL) {
	  if (same_name)
	    *result_set_status = RESULT_UNCHANGED;
	  else
	    *result_set_status = RESULT_NONE;
	  return (SORT_FAILURE);
	} 
	else 
	  key_list[i]->keys[k].key = keydata;
	
	
	break;

      case e23_elementSpec:
	/* These element specs don't need to read the SGML records */
       	if (strcasecmp(sk->u.elementSpec->elementSpec->u.elementSetName->data, "RANK") == 0
	    || strcasecmp(sk->u.elementSpec->elementSpec->u.elementSetName->data, "SCORE") == 0 
	    || strcasecmp(sk->u.elementSpec->elementSpec->u.elementSetName->data, "RANKSUM") == 0 
	    || strcasecmp(sk->u.elementSpec->elementSpec->u.elementSetName->data, "SUMRANK") == 0) {
	  
	  sgmlrec = NULL;
	  keydata = CALLOC(char, 16);
	  if (keydata == NULL) goto allocationerror;
	  
	  if (final_set->result_type & PROBABILISTIC_RESULT
	      || final_set->result_type & OKAPI_RESULT
	      || final_set->result_type & CORI_RESULT 
	      || final_set->result_type & TFIDF_RESULT 
	      || final_set->result_type & BLOB_RESULT
	      || final_set->result_type & GEORANK_RESULT 
	      || final_set->result_type & FUZZY_RESULT) {
	    sprintf(keydata,"%03.10f",  key_list[i]->doc_weight);
	  }
	  else { /* boolean result -- no real ranking so use orig_rank */
	    sprintf(keydata,"%015d", key_list[i]->orig_rank);
	  }
	  key_list[i]->keys[k].key = keydata;
	}
	else {
	  /* no other keys defined now */
	  sprintf(errormsg,"Unknown sort element set name '%s'",
		  sk->u.elementSpec->elementSpec->u.elementSetName->data);
	  diagnostic_set(207,0, errormsg);
	  if (same_name)
	    *result_set_status = RESULT_UNCHANGED;
	  else
	    *result_set_status = RESULT_NONE;
	  return (SORT_FAILURE);
	}
	break;
	
      case e23_sortAttributes:
	/* Read the record from the sgml file and parse */
	if (first_key) {
	  if ((final_set->result_type & COMPONENT_RESULT) 
	      != COMPONENT_RESULT) {
	    if ((final_set->result_type & VIRTUAL_RESULT) 
		!= VIRTUAL_RESULT) {
	      /* this is a regular document resultset */
	      infilename = final_set->filename;
	      sgmlrec = GetSGML(infilename,key_list[i]->docid);
	    } 
	    else { /* This one is a virtual result */
	      /* which means that we have to get the record from the REAL */
	      /* resultsets                                               */
	      v_set = (weighted_result *) final_set->entries[key_list[i]->orig_rank].xtra; 
	      infilename = v_set->filename;
	      sgmlrec = GetSGML(infilename,v_set->entries[key_list[i]->docid].record_num);
	    }
	  }
	  else { /* This is a component result set */
	    /* get the component data */
	    component_data = get_component_data(key_list[i]->docid, 
						final_set->component,
						&sgmlrec);
	    
	  }
	  
	  first_key = 0;
	}
	if (sgmlrec == NULL) { 
	  /* for sorting we just do a message and set the key to NULL */
	  if (sgml_parse_error != 0) {
	    fprintf(LOGFILE, "SORT: SGML parse error for record %d\n",key_list[i]->docid);
	    fflush(LOGFILE);
	  }
	  else {
	    fprintf(LOGFILE, "Sort - Attributes: error reading record %d errno %d\n",
		    key_list[i]->docid, errno);
	    fflush(LOGFILE);
	  }
	} 
	keydata = get_sort_key_from_attrlist(sgmlrec, component_data,
					     key_list[i]->docid,
					     infilename,
					     sk->u.sortAttributes.id,
					       sk->u.sortAttributes.list);
	if (diagnostic_get() != 0) {
	  if (same_name)
	    *result_set_status = RESULT_UNCHANGED;
	  else
	    *result_set_status = RESULT_NONE;
	  return (SORT_FAILURE);
	} 
	else 
	  key_list[i]->keys[k].key = keydata;

	break;
	
      }
    }

    if (sgmlrec 
	&& (final_set->result_type & COMPONENT_RESULT) != COMPONENT_RESULT) {
      /* we don't free component records here */
      free_doc(sgmlrec);
      sgmlrec = NULL;
    }
  }

  /* actually perform the sort */
  qsort ((void *) key_list, final_set->num_hits,
	 sizeof(ResultSortKey *), compare_ResSrt_entries);


  if (diagnostic_get() != 0) {
    if (same_name)
      *result_set_status = RESULT_UNCHANGED;
    else
      *result_set_status = RESULT_NONE;
  }
  else {
    out_set = (weighted_result *) 
      CALLOC (char, (sizeof(weighted_result) 
		     + ((final_set->num_hits+1) * sizeof(weighted_entry))));

    
    *out_set = *final_set;
    /* reorder the result set according to the sort */
    for (i = 0; i < out_set->num_hits; i++) { 

      out_set->entries[i].record_num = key_list[i]->docid;
      out_set->entries[i].xtra = key_list[i]->vdb_ptr;
      out_set->entries[i].weight = key_list[i]->doc_weight;
    }
    /* now store the data */
    
    if (session->disable_saving_result_sets)
      replaceflag |= 4; /* no save flag */

    out_set->setid[0] = '\0';

    out_set->result_type |= SORTED_RESULT_SET;

    if (se_store_result_set(out_set, output_set_name, 
			    final_set->filename, 
			    session->c_resultSetDir, replaceflag) == SUCCEED) {
      diagnostic_clear(); /* set not found may be set for new set names */
    }
    else { /* Store failed */
      if (same_name)
	*result_set_status = RESULT_UNCHANGED;
      else
	*result_set_status = RESULT_NONE;
    }

  }

  /* At this point final_set MAY have been freed by se_store_result_set() */

  /* Free up the sort keys */
  if (key_list != NULL) {
    for (i = 0; i < out_set->num_hits && key_list[i] != NULL ; i++) { 
      for (k = 0; k < key_list[i]->numkeys; k++) {
	if (key_list[i]->keys[k].key != NULL)
	  FREE(key_list[i]->keys[k].key);
      }
      FREE(key_list[i]);
    }
    FREE(key_list);
  }

  if (diagnostic_get() != 0) {
    return (SORT_FAILURE);
  }

  /* this will be passed back to the client as "other_information" */
  *result_set_size = out_set->num_hits; 

  return (SORT_SUCCESS); 

 allocationerror: 

  /* first attemptto free up the data */
  if (key_list != NULL) {
    for (i = 0; i < final_set->num_hits && key_list[i] != NULL ; i++) { 
      for (k = 0; k < key_list[i]->numkeys; k++) {
	if (key_list[i]->keys[k].key != NULL)
	  FREE(key_list[i]->keys[k].key);
      }
      FREE(key_list[i]);
    }
    FREE(key_list);
  }
  diagnostic_set(230,0, "Memory for sort exhausted");
  if (same_name)
    *result_set_status = RESULT_UNCHANGED;
  else
    *result_set_status = RESULT_NONE;
  return (SORT_FAILURE);
}

/* The following functions extract sort keys from the SGML record */

char *
get_sort_key_from_keylist(SGML_Document *sgmlrec, SGML_Data *component, 
			  int docid, char *filename, idx_key *keys, 
			  idx_list_entry *idx)
{
  idx_key *k, *top_key;
  idx_key *attrval_subkey;
  idx_key *attr_subkey;
  Tcl_HashTable *hash_tab;
  int attribute_flag = 0;
  SGML_Tag_Data *tag_data;
  char *result_sortkey = NULL;
  int casesensitive;

  SGML_Tag_Data *comp_td, comp_tag_data;
  

  /* if, for some reason, there is no record, then the key is NULL */
  if (sgmlrec == NULL)
    return (NULL);

  if (sgmlrec->DTD->type > 0)
    casesensitive = 1;
  else
    casesensitive = 0;

  /* check for display directive to include only the first match */
  
  if (component != NULL) {
    comp_tag_data.item = component;
    comp_tag_data.next_tag_data = NULL;
    comp_td = &comp_tag_data;
  }
  else {
    comp_td = NULL;
  }

  /* go through the list of keys until the first match */
  for (top_key = keys; top_key != NULL 
	 && result_sortkey == NULL ; top_key = top_key->next_key) {

    /* OK, we have the record parsed and ready */
    /* so we extract the matching key data */
    attribute_flag = 0;
    attr_subkey = NULL;
    attrval_subkey = NULL;
    
    /* we look only at the first key */
    for (k = top_key; k ; k = k->subkey) {
      if (k->attribute_flag == 1 || k->attribute_flag == 5
	  || k->attribute_flag == 6 || k->attribute_flag == 7) {
	attribute_flag = k->attribute_flag;
	attr_subkey = k;
	/* the subkey is the beginning of the value list */
	/* to match within a specific tag                */
	attrval_subkey = k->subkey;
      }
      
    }
  
    tag_data = comp_tag_list(keys, sgmlrec->Tag_hash_tab, comp_td, casesensitive);
  
    /* we take only the first occurrence from the tag data */
    if (tag_data == NULL)
      continue;


    if (idx->type & FLD008_KEY) {
      result_sortkey = normalize_008(tag_data->item->content_start, 
				     attrval_subkey->key);
    }
    else if ((idx->type & DATE_KEY) 
	     || (idx->type & DATE_RANGE_KEY)) {
      char *temp;
      temp = detag_data_block(tag_data->item, idx->type, idx);
      if (temp == NULL)
	continue;
      result_sortkey = parse_date(temp,idx->dateformat);
      FREE (temp);
    }
    else if ((idx->type & INTEGER_KEY) 
	     || (idx->type & DECIMAL_KEY)
	     || (idx->type & FLOAT_KEY)) {
      char *temp;
      int integer_key;
      int decimal_part;
      int scanresult;
      double double_key;

      temp = detag_data_block(tag_data->item, idx->type, idx);

      if (temp == NULL)
	continue;

      result_sortkey = CALLOC(char, 50);

      switch (idx->type & (INTEGER_KEY | DECIMAL_KEY | FLOAT_KEY)) {
      case INTEGER_KEY:
	scanresult = sscanf(temp,"%d", &integer_key);
	if (scanresult == 0)
	  return (0);
	else
	  sprintf(result_sortkey,"%010d", integer_key);
	break;

      case DECIMAL_KEY:
	integer_key = 0;
	decimal_part = 0;
	scanresult = sscanf(temp,"%d.%d", &integer_key, &decimal_part);
    
	if (scanresult == 0)
	  return (0);
	else
	  sprintf(result_sortkey,"%010d.%d", integer_key, decimal_part);
	break;

      case FLOAT_KEY:
	scanresult = sscanf(temp,"%g", &double_key);
	if (scanresult == 0)
	  return (0);
	else
	  sprintf(result_sortkey,"%016.6f", double_key);
	break;
      }

    }
    else {
      char *orig, *temp = NULL;
      orig = temp = detag_data_block(tag_data->item, idx->type, idx);
      temp += strspn(temp, " \t\r\v\n");
      result_sortkey = strdup(temp);
      FREE(orig);
    }
    free_tag_data_list(tag_data);
  }
  
  return(result_sortkey);
}


char *
get_sort_key_from_attrlist(SGML_Document *sgmlrec, SGML_Data *comp_data,
			   int docid, char *filename, 
			   ObjectIdentifier oid, AttributeList *attrlist)
{
  AttributeList *l;
  AttributeElement *elem;
  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  idx_list_entry *idx;
  char *local_index_name;
  char errormsg[500];
  struct list_List2 *l1;
  char *result_sortkey;
  int i;
  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];
  
  
  if (sgmlrec == NULL)
    return NULL;
  
  
  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }
  
  for (l = attrlist; l ; l = l->next) {
    elem = l->item;
    
    if (elem->attributeValue.which == e7_numeric) {
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = elem->attributeValue.u.numeric;
	attr_oid[elem->attributeType] = elem->attributeSet;
      }  else {
	/* diagnostic unsupported attribute type... */
	diagnostic_set(113,elem->attributeType,NULL);
	return NULL;
      }
    }
    else {
      /* complex attribute -- Must be local index name or string valued attribute type */
      
      if (elem->attributeType < MAXATTRTYPE) {

	attr_nums[elem->attributeType] = -1;
	for (l1 = elem->attributeValue.u.complex.list;
	     l1; l1 = l1->next) {
	  if (l1->item->which == e25_numeric) {
	    /* we don't handle any numeric types */
	    diagnostic_set(123,0,NULL);
	    return NULL;
	  }
	  else { /* string type, so set the local name... */
	    /* in principle there can be multiple items, take only last */
	    if (l1->item == NULL || l1->item->u.string == NULL) {
	      diagnostic_set(126,0,"NULL String as attribute name");
	      return NULL;
	    }
	    attr_str[elem->attributeType] =
	      local_index_name = l1->item->u.string->data;
	    attr_oid[elem->attributeType] = elem->attributeSet;
	  }
	}
      } else {
	/* diagnostic unsupported attribute type... */
	diagnostic_set(113,elem->attributeType,NULL);
	return NULL;
      }      
    }
  }  
      
  use = attr_nums[USE_ATTR];
  
  /* special cases where docid is specified as the sort key */
  if (use == 1032 || use == 5401 || use == 5400) {
    
    result_sortkey = CALLOC(char, 20);
    sprintf(result_sortkey,"%018d", docid);
    return(result_sortkey);
  }

  
  /* convert the specified elements to an index list entry, if possible */
  if (use == -1) {
    idx = cf_getidx_entry(filename, local_index_name);
  } 
  else {
    idx = cf_getidx_mapattr(filename,
			    attr_nums, 
			    attr_str,
			    attr_oid);
    
    
    if (idx == NULL) { /* try without relation operator */
      attr_nums[RELATION_ATTR] = 0;
      idx = cf_getidx_mapattr(filename,
			      attr_nums, 
			      attr_str,
			      attr_oid);
      
      if (idx == NULL) { /* try without relation or truncation operator */
	attr_nums[RELATION_ATTR] = 0;
	attr_nums[TRUNC_ATTR] = 0;
	idx = cf_getidx_mapattr(filename,
				attr_nums, 
				attr_str,
				attr_oid);
	
	if (idx == NULL) { /* try with only USE */
	  for (i = 2; i < MAXATTRTYPE; i++)
	    attr_nums[i] = 0;
	  idx = cf_getidx_mapattr(filename,
				  attr_nums, 
				  attr_str,
				  attr_oid);
	}
	
      }
      
    }
    
  }
  if (idx == NULL) { /* check the names for this USE attribute */
    /* unable to match attributes */
    if (use == -1) 
      sprintf(errormsg,"Complex Use Attribute '%s' not available in this database",
	      local_index_name);
    else
      sprintf(errormsg,"Use Attribute '%d' not available in this database",
	      use);
    
    diagnostic_set(207,0, errormsg);
    return (NULL);
  }
  
  result_sortkey = get_sort_key_from_keylist(sgmlrec, comp_data, docid, 
					     filename, idx->keys, idx);
  return (result_sortkey);
  
}


char *
get_sort_key_from_tags(SGML_Document *sgmlrec, SGML_Data *comp_data, 
		       int docid, char *filename, 
		       char *tagspec)
{
  char *tagpat, *lastptr, *tags;
  idx_key *idx_keys, *prev_idx_keys, *head_idx_keys;
  char *result_sortkey;
  idx_list_entry idx;
  char *attr;
  char *equal;
  char *quote;

  if (sgmlrec == NULL)
    return NULL;
  
  if (tagspec == NULL) 
    return NULL;
  
  tags = strdup(tagspec);

  /* this dummy index entry is needed for extracting the key from the data */
  memset(&idx,0,sizeof(idx_list_entry));

  idx_keys = prev_idx_keys = head_idx_keys = NULL;
  
  /* tagspec should be a XPath-like string (with the addition of regular */
  /* expressions)                                                        */
  tagpat = strtok_r(tagspec, "/{}", &lastptr);
  
  do {
    prev_idx_keys = idx_keys;
    if ((idx_keys = CALLOC(idx_key,1)) == NULL) {
      diagnostic_set(230,0, "Memory for sort exhausted");
      return(NULL);
    }
    if (prev_idx_keys == NULL) {
      /* must be the first index entry in the list */
      head_idx_keys = idx_keys;
    }
    else
      prev_idx_keys->subkey = idx_keys;
    
    /* check the key for attribute specs, etc..*/

    if ((attr = strpbrk(tagpat,"@")) == NULL) {
      idx_keys->key =  strdup(tagpat);         
    }
    else {
      /* Have an attribute specification */
      attr++;      
      if ((equal = strpbrk(tagpat,"=")) == NULL) {
	idx_keys->key = strdup(attr);
	idx_keys->attribute_flag = 1;
      }
      else {
	*equal = '\0';
	idx_keys->key = strdup(attr);
	idx_keys->attribute_flag = 5; /* subkey flag value */
	equal++;
	equal++; /* past quotes */
	equal[strlen(equal)-1] = '\0';

	if ((idx_keys->subkey = CALLOC(idx_key,1)) == NULL) {
	  diagnostic_set(230,0, "Memory for sort exhausted");
	  return(NULL);
	}
	idx_keys->subkey->key = strdup(equal);
	idx_keys->subkey->attribute_flag = 2;
	idx_keys = idx_keys->subkey;

      }
    }
    
       
       
    tagpat = strtok_r(NULL, "/{}", &lastptr);

  } while (tagpat != NULL);
  
  
  result_sortkey = get_sort_key_from_keylist(sgmlrec, comp_data, docid, 
	 				     filename, head_idx_keys, &idx);

  /* remove the idx_keys */
  prev_idx_keys = NULL;
  FREE(tags);

  for (idx_keys = head_idx_keys; idx_keys; idx_keys = prev_idx_keys) {
    prev_idx_keys = idx_keys->subkey;
    if (idx_keys->key)
      FREE(idx_keys->key);
    FREE(idx_keys);
  }
  
  return (result_sortkey);
  
  
}


/* The following actually compares the sort key entries for the sort */

int 
compare_ResSrt_entries (ResultSortKey **e1, ResultSortKey **e2) {

  struct SortKeySpec *spec;
  int relation;
  int casesensitive;
  int compresult;
  int k;
  ResultSortKey *t1, *t2;

  t1 = *e1;
  t2 = *e2;

  for (k = 0; k < t1->numkeys; k++) {
    relation = t1->keys[k].spec->sortRelation;
    casesensitive = 
      t1->keys[k].spec->caseSensitivity;
    if (t1->keys[k].key != NULL
	&& t2->keys[k].key != NULL) {
      /* do an actual comparison */
      if (casesensitive) {
	if ((compresult = strcmp(t1->keys[k].key, t2->keys[k].key)) > 1)
	  compresult = 1;
	else if (compresult < 0)
	  compresult = -1;
      }
      else { 
	if ((compresult = strcasecmp(t1->keys[k].key, t2->keys[k].key)) > 1)
	  compresult = 1;
	else if (compresult < 0)
	  compresult = -1;
      }
      if (relation == 0 & compresult != 0) /*ascending*/
	return (compresult);
      else
	return (compresult * -1);
    }
    else {
      /* Handle missing keys */
      if (t1->keys[k].spec->missingValueAction.which == e21_abort) {
	diagnostic_set(207,0, "Sort cancelled due to missing values");
      }
      else if (t1->keys[k].spec->missingValueAction.which == e21_nullVal) {
	if (t1->keys[k].key == NULL
	    && t2->keys[k].key == NULL) {
	  compresult = 0;
	}
	else if (t1->keys[k].key == NULL) {
	  /* we put the null values at the end if ascending */
	  if (relation == 0) /*ascending*/
	    return 1;
	  else
	    return -1;
	}
	else {
	  if (relation == 0) /*ascending*/
	    return -1;
	  else
	    return 1;
	}
      }
      else if (t1->keys[k].spec->missingValueAction.which 
	       == e21_missingValueData) {
	if (t1->keys[k].key == NULL
	    && t2->keys[k].key == NULL) {
	  compresult = 0 ; /* matching NULLs */
	}
	else {
	  if (t1->keys[k].key == NULL) {
	    
	    if (casesensitive) {
	      if ((compresult = strcmp(t1->keys[k].spec->missingValueAction.u.missingValueData->data, 
				       t2->keys[k].key)) > 0)
		compresult = 1;
	      else if (compresult < 0)
		compresult = -1;
	    }
	    else {
	      if ((compresult = strcasecmp(t1->keys[k].spec->missingValueAction.u.missingValueData->data,
					   t2->keys[k].key)) > 0)
		compresult = 1;
	      else if (compresult < 0)
		compresult = -1;
	    }
	  }
	  else { /* e2 key is null */
	    
	    if (casesensitive) {
	      if ((compresult = strcmp(t1->keys[k].key,
				       t2->keys[k].spec->missingValueAction.u.missingValueData->data)) > 0)
		compresult = 1;
	      else if (compresult < 0)
		compresult = -1;
	    }
	    else {
	      if ((compresult = strcasecmp(t1->keys[k].key,
					   t2->keys[k].spec->missingValueAction.u.missingValueData->data)) > 0)
		compresult = 1;
	      else if (compresult < 0)
		compresult = -1;
	    }
	    if (relation == 0) {/*ascending*/
	      if (compresult)
		return (compresult);
	    }
	    else {
	      if (compresult)
		return (compresult * -1);
	    }
	  }
	}
      }
    }
  }
  
  return (compresult); /* should be 0 or would have returned sooner */

}








