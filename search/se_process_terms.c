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
*	Header Name:	se_process_terms.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Process a Z39.50 AttributesPlusTerm structure
*                       build a weighted result set. The weights will
*                       depend on the attribute and operators used in the query
*                       This routine is called by se_process_query, which
*                       handles the boolean operations between indexes.
*
*	Usage:		se_process_terms(databasename, attrPlusTerm)
*
*	Variables:	attrPlusTerm - the Z39.50 AttributePlusTerms struct.
*                       databasename - the filename or file tag of the
*                           desired database.
*
*	Return Conditions and Return Codes:
*
*	Date:		12/5/94
*	Revised:
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* the following includes the Z39.50 defs */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"
#include "z_parse.h"

/* external declarations */
extern weighted_result *se_bool_element (idx_list_entry *idx,
					 char *filename,
					 char *searchstring,
					 int relation, int position,
					 int structure, int truncation,
					 int completeness, int attr_nums[],
					 char *attr_str[],
					 ObjectIdentifier attr_oid[]);

extern weighted_result *se_georank(idx_list_entry *idx, char *searchstring,
                                int relation, int position,
                                int structure, int truncation,
                                int completeness, int attr_nums[],
                                char *attr_str[], ObjectIdentifier attr_oid[]);

extern weighted_result *se_prob(char *file, char *indexname, char *searchstring);
extern weighted_result *se_blobs (idx_list_entry *idx,  char *searchstring);

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
extern idx_list_entry *cf_getidx_entry_nowarn(char *filename, char *indexname);
extern config_file_info *find_file_config(char *filename);
extern component_list_entry *find_component_name(config_file_info *cf,
					  char *compname);
extern DB *cf_component_open(char *filename, char *componentname) ;

extern component_data_item *get_component_record(int component_id,
						 component_list_entry *comp);

extern idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[],
				  char *in_strattr[],
				  ObjectIdentifier in_oid[]);

extern weighted_result *se_okapi (char *filename, char *indexname, 
				  char *insearchstring);

extern weighted_result *se_cori (char *filename, char *indexname, 
				 char *insearchstring);
extern weighted_result *se_prob_trec2 (char *filename, char *indexname, 
				       char *insearchstring, int blindfeedback);
extern weighted_result *se_tfidf (char *filename, char *indexname, 
				  char *insearchstring);
extern weighted_result *se_tfidf_lucene (char *filename, char *indexname, 
					 char *insearchstring);


/* the actual routine */
weighted_result *
se_process_terms(char *file, AttributesPlusTerm *attrPlusTerm)
{
  AttributeList *l;
  AttributeElement *elem;
  OctetString *r;
  ObjectIdentifier useoid = NULL, reloid = NULL, posoid = NULL,
    structoid = NULL, truncoid = NULL, compoid = NULL;
  int use = 0, relation = 0, position = 0, structure = 0;
  int truncation = 0, completeness = 0;
  idx_list_entry *idx;
  DB *index_db;
  weighted_result *wt_result;
  int termids[100];
  char *local_index_name;
  component_data_item *component_data;
  struct list_List2 *l1;

  int i;

  int attr_nums[MAXATTRTYPE];
  char *attr_str[MAXATTRTYPE];
  ObjectIdentifier attr_oid[MAXATTRTYPE];

  if (attrPlusTerm == NULL) /* no Query ?? */
    return NULL;

  for (i = 0; i < MAXATTRTYPE; i++) {
    attr_nums[i] = 0;
    attr_str[i] = NULL;
    attr_oid[i] = NULL;
  }


  for (l = attrPlusTerm->attributes; l ; l = l->next) {
    elem = l->item;

    if (elem->attributeValue.which == e7_numeric) {
      if (elem->attributeType < MAXATTRTYPE) {
	attr_nums[elem->attributeType] = elem->attributeValue.u.numeric;
	attr_oid[elem->attributeType] = elem->attributeSet;

      } else {
	/* diagnostic unsupported attribute type... */
	diagnostic_set(113,elem->attributeType,NULL);
	return NULL;
      }
    }
    else {  /* complex attribute -- Must be local index name or string valued attribute type */
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

  /* If this was a search with no attributes specified - use Default
   * values (which are the 0 values for use in the config file.
   */

  /* we handle the special case of a DocID by just creating */
  /* a result set with the appropriate record               */
  if (use == 1032) { /* it is a "docid" search */
    int idnum;
    int numrecs;
    int allocrecs;
    int start, end, recnum, i;

    if (attr_nums[RELATION_ATTR] != 0 && attr_nums[RELATION_ATTR] != 3)
      relation = attr_nums[RELATION_ATTR];
    else
      relation = 0;

    idnum = atoi(attrPlusTerm->term->u.general->data);

    if (idnum == 0) {
      /* No conversion could be performed */
      diagnostic_set(10,0,"Could not convert doc-id to number (or was 0)");
      return(NULL);
    }

    /* number converted OK -- see if it is with DB limits */
    numrecs = cf_getnumdocs(file);
    if (idnum > 0 && ((idnum <= numrecs)
	|| (relation == 1 || relation == 2
	    || relation == 4 || relation == 5))) {
      assoc_rec assocrec;

      assocrec.offset = 0;
      assocrec.recsize = 0;

      /* verify that the record is not deleted */
      if (idnum < numrecs)
	fil_seek (file,idnum,&assocrec);

      if (assocrec.offset < 0) { /* deleted record */
	return(NULL);
      }
      else { /* we have a match */
	if (relation == 0) {
	  allocrecs = 0;
	  start = idnum;
	  end = idnum;
	}
	else {
	  if (relation == 1) {
	    start = 1;
	    end = idnum ;
	    if (end > numrecs) {
	      end = numrecs;
	      allocrecs = numrecs;
	    }
	    else
	      allocrecs = idnum - 1;
	    if (allocrecs == 0) {
	      diagnostic_set(10,0,"Document number less than one");
	      return(NULL);
	    }
	  }
	  else if (relation == 2) {
	    start = 1;
	    end = idnum;
	    if (end > numrecs) {
	      end = numrecs;
	      allocrecs = numrecs;
	    }
	    else
	      allocrecs = idnum;
	    if (allocrecs == 0) {
	      diagnostic_set(10,0,"Document number less than one");
	      return(NULL);
	    }
	  }
	  else if (relation == 4) {
	    allocrecs = numrecs - (idnum - 1);
	    start = idnum;
	    end = numrecs;
	    if (start > numrecs) {
	      diagnostic_set(10,0,"Document start number larger than database size");
	      return(NULL);
	    }
	  }
	  else if (relation == 5) {
	    allocrecs = numrecs - (idnum);
	    start = idnum + 1;
	    end = numrecs;
	    if (start > numrecs) {
	      diagnostic_set(10,0,"Document start number larger than database size");
	      return(NULL);
	    }
	  }
	  if (allocrecs < 0)
	    allocrecs = 0;
	}


	wt_result = (weighted_result *)
	  CALLOC (char, sizeof(weighted_result)
		  + (allocrecs * sizeof(weighted_entry)));

	if (wt_result != NULL) {
	  wt_result->setid[0] = '\0';
	  wt_result->result_type = 0;
	  strcpy(wt_result->filename, file);
	  if (allocrecs == 0) {
	    wt_result->num_hits = 1;
	    wt_result->entries[0].record_num = idnum;
	    wt_result->entries[0].weight = 1;
	  }
	  else {
	    wt_result->num_hits = allocrecs;
	    i = 0;
	    for (recnum = start; recnum <= end; recnum++) {
	      wt_result->entries[i].record_num = recnum;
	    wt_result->entries[i++].weight = 1;
	    }
	  }
	}
	return(wt_result);
      }

    }
    else { /* Number out of range */
      diagnostic_set(10,0,"Document number larger than database size");
      return(NULL);
    }
  }

  /* we handle the special case of a COMPONENT_RECORD by just creating */
  /* a result set with the appropriate record               */
  if (use == 5401) { /* it is a "docid" search */
    char *component_name, *compnum;
    long idnum;
    DB *db;
    config_file_info *cf;
    component_list_entry *comp;

    if (attr_nums[RELATION_ATTR] != 0 && attr_nums[RELATION_ATTR] != 3)
      relation = attr_nums[RELATION_ATTR];
    else
      relation = 0;


    component_name = strdup(attrPlusTerm->term->u.general->data);

    compnum = strchr(component_name, ':');
    if (compnum == NULL) {
      /* No conversion could be performed */
      diagnostic_set(10,0,"Could not convert componentid to number (missing :?)");
      return(NULL);
    }
    *compnum = '\0';
    compnum++;

    idnum = atol(compnum);

    if (idnum == 0) {
      /* No conversion could be performed */
      diagnostic_set(10,0,"Could not convert componentid to number (or was 0)");
      return(NULL);
    }
    /* number converted OK -- see if it is with DB limits */
    cf = find_file_config(file);
    db = cf_component_open(file, component_name);
    comp = find_component_name(cf, component_name);
    if (comp == NULL || comp->comp_db == NULL) {
      diagnostic_set(10,0,"Could not find component name (exact key match required)");
      return(NULL);

    }
    if (idnum > 0 && idnum <= comp->max_component_id) {
      component_data = get_component_record(idnum, comp);

      wt_result = (weighted_result *)
	CALLOC (char, sizeof(weighted_result));

      if (wt_result != NULL) {
	wt_result->num_hits = 1;

	wt_result->entries[0].record_num = component_data->record_id;
	wt_result->entries[0].weight = 1;
	wt_result->setid[0] = '\0';
	wt_result->result_type = 0;
	strcpy(wt_result->filename, file);
      }
      return(wt_result);
    }
    else { /* Number out of range */
      diagnostic_set(10,0,"Component number out of range");
      return(NULL);
    }
  }

  /* we handle the special case of a COMPONENT_id by just creating */
  /* a result set with the appropriate record               */
  if (use == 5400) { /* it is a "docid" search */
    char *component_name, *compnum;
    long idnum;
    DB *db;
    config_file_info *cf;
    component_list_entry *comp;

    if (attr_nums[RELATION_ATTR] != 0 && attr_nums[RELATION_ATTR] != 3)
      relation = attr_nums[RELATION_ATTR];
    else
      relation = 0;

    component_name = strdup(attrPlusTerm->term->u.general->data);

    compnum = strchr(component_name, ':');
    if (compnum == NULL) {
      /* No conversion could be performed */
      diagnostic_set(10,0,"Could not convert componentid to number (missing :?)");
      return(NULL);
    }
    *compnum = '\0';
    compnum++;

    idnum = atol(compnum);

    if (idnum == 0) {
      /* No conversion could be performed */
      diagnostic_set(10,0,"Could not convert componentid to number (or was 0)");
      return(NULL);
    }
    /* number converted OK -- see if it is with DB limits */
    cf = find_file_config(file);
    db = cf_component_open(file, component_name);
    comp = find_component_name(cf, component_name);
    if (comp == NULL || comp->comp_db == NULL) {
      diagnostic_set(10,0,"Could not find component name (exact key match required)");
      return(NULL);

    }
    if (idnum > 0 && idnum <= comp->max_component_id) {

      wt_result = (weighted_result *)
	CALLOC (char, sizeof(weighted_result));

      if (wt_result != NULL) {
	wt_result->num_hits = 1;

	wt_result->entries[0].record_num = idnum;
	wt_result->entries[0].weight = 1;
	wt_result->setid[0] = '\0';
	wt_result->result_type = COMPONENT_RESULT;
	wt_result->component = comp;
	strcpy(wt_result->filename, file);
      }
      return(wt_result);
    }
    else { /* Number out of range */
      diagnostic_set(10,0,"Component number out of range");
      return(NULL);
    }
  }

  /* convert the specified elements to an index list entry, if possible */


  if (attr_nums[USE_ATTR] == -1) {
    relation = attr_nums[RELATION_ATTR] ;
    truncation = attr_nums[TRUNC_ATTR];
    idx = cf_getidx_entry(file, local_index_name);
  }
  else {
    idx = cf_getidx_mapattr(file,
			    attr_nums,
			    attr_str,
			    attr_oid);

    if (idx == NULL) { /* try without relation operator */
      relation = attr_nums[RELATION_ATTR];
      attr_nums[RELATION_ATTR] = 0;
      idx = cf_getidx_mapattr(file,
			      attr_nums,
			      attr_str,
			      attr_oid);

      if (idx == NULL) { /* try without relation or truncation operator */
	truncation = attr_nums[TRUNC_ATTR];
	attr_nums[TRUNC_ATTR] = 0;
	idx = cf_getidx_mapattr(file,
				attr_nums,
				attr_str,
				attr_oid);

	if (idx == NULL) { /* try with only USE */
	  for (i = 2; i < MAXATTRTYPE; i++)
	    attr_nums[i] = 0;
	  idx = cf_getidx_mapattr(file,
				  attr_nums,
				  attr_str,
				  attr_oid);
	}
      }
    }
  }

  if (idx == NULL && useoid == NULL && attr_nums[USE_ATTR] != -1) {
    /* check the names for this USE attribute */
    int i;
    char *temp;

    for (i = 0; all_indexes[i].name[0] != 0; i++) {
      if (all_indexes[i].attributes[1] == use) {
	temp = strdup(all_indexes[i].name);
	ToLowerCase(temp);
	idx = cf_getidx_entry_nowarn(file, temp);
	free(temp);
	if (idx != NULL)
	  break;
      }
    }
  }


  if (idx) {
#ifdef DEBUGSTUFF
    printf("index mapping = %s\n",idx->tag);
#endif

    /* may have had diagnostic set in testing, so clear it now */
    diagnostic_clear();

    /* restore things that may have been blanked for matching */
    if (relation)
      attr_nums[RELATION_ATTR] = relation;
    if (truncation)
      attr_nums[TRUNC_ATTR] = truncation;
    /* If the relation specified is "relevance" (102) */
    /* or this is a "free form text" or "document text*/
    /* structured query, (such as relevance feedback) */
    /* then use the probabilistic search              */
    if (attr_nums[RELATION_ATTR] == RELEVANCE
	|| attr_nums[STRUCTURE_ATTR] == FREE_TEXT
	|| attr_nums[STRUCTURE_ATTR] == DOCUMENT_TEXT) {
#ifdef DEBUGSTUFF
      printf("using probabilistic search\n");
#endif
      wt_result = se_prob(file,idx->tag,attrPlusTerm->term->u.general->data);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);

    }
    else if (attr_nums[RELATION_ATTR] == 500) {
      /* we are using 500 as the designation for basic OKAPI BM25 searching */
      wt_result = se_okapi(file,idx->tag,attrPlusTerm->term->u.general->data);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == 501) {
      /* we are using 500 as the designation for CORI searching */
      wt_result = se_cori(file,idx->tag,attrPlusTerm->term->u.general->data);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == 510) {
      /* we are using 510 as the designation for TREC-2 Prob searching */
      wt_result = se_prob_trec2(file,idx->tag,attrPlusTerm->term->u.general->data, 0);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == 511) {
      /* we are using 510 as the designation for TREC-2 Prob searching */
      wt_result = se_prob_trec2(file,idx->tag,attrPlusTerm->term->u.general->data, 1);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == 530) {
      /* we are using 530 as the designation for TFIDF searching */
      wt_result = se_tfidf(file,idx->tag,attrPlusTerm->term->u.general->data);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == 540) {
      /* we are using 540 as the designation for the Lucene TFIDF searching */
      wt_result = se_tfidf_lucene(file, idx->tag,
				  attrPlusTerm->term->u.general->data);
      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
	if (wt_result->result_type & PAGED_RESULT) {
	  strcat(wt_result->filename, "::");
	  strcat(wt_result->filename, idx->tag);
	}
      }
      return(wt_result);
    }
    else if (attr_nums[RELATION_ATTR] == STEM) {
      /* we are using the STEM designation for blob image searching */
      /* this will work for text too - not sure how good the results would be*/
      if (idx->db == NULL) { /* open the index file */
	index_db =
	  (DB *) cf_index_open(file, idx->tag, INDEXFL);

      }
      wt_result = se_blobs(idx,attrPlusTerm->term->u.general->data);

      if (wt_result != NULL) {
	strcpy(wt_result->filename, file);
      }
      return(wt_result);
    }
/* ******************************************************************************* */
   else if (attr_nums[RELATION_ATTR] == 707 || attr_nums[RELATION_ATTR] == 708 || attr_nums[RELATION_ATTR] == 709) {

#ifdef DEBUGSTUFF
      printf("using georank search\n");
#endif
	/* Using 707 as the designation for basic GEO_OVERLAPS_RANK searching
	 * 708 for GEO_FULLY_ENCLOSED_WITHIN_RANK  and 709 for GEO_ENCLOSES_RANK
	*/

    if (idx->db == NULL) { /* open the index file */
      index_db =
	(DB *) cf_index_open(file, idx->tag, INDEXFL);
      
    }
    wt_result = se_georank(idx,
			   attrPlusTerm->term->u.general->data,
			   attr_nums[RELATION_ATTR],
			   attr_nums[POSITION_ATTR],
			   attr_nums[STRUCTURE_ATTR],
			   attr_nums[TRUNC_ATTR],
			   attr_nums[COMPLETENESS_ATTR],
			   attr_nums, attr_str, attr_oid);
    
    if (wt_result != NULL) {
      strcpy(wt_result->filename, file);
      
      if (wt_result->result_type & PAGED_RESULT) {
	strcat(wt_result->filename, "::");
	strcat(wt_result->filename, idx->tag);
      }
    }
    return(wt_result);
   }
 /* ******************************************************************************* */
   else { /* assume it is NOT a probabilistic search */
#ifdef DEBUGSTUFF
     printf("using boolean search\n");
#endif
     if (idx->db == NULL) { /* open the index file */
       index_db =
	 (DB *) cf_index_open(file, idx->tag, INDEXFL);
       
     }
     wt_result = se_bool_element(idx, file,
				 attrPlusTerm->term->u.general->data,
				 attr_nums[RELATION_ATTR],
				 attr_nums[POSITION_ATTR],
				 attr_nums[STRUCTURE_ATTR],
				 attr_nums[TRUNC_ATTR],
				 attr_nums[COMPLETENESS_ATTR],
				 attr_nums, attr_str, attr_oid);
     
     if (wt_result != NULL) {
       strcpy(wt_result->filename, file);
       if (wt_result->result_type & PAGED_RESULT) {
	 strcat(wt_result->filename, "::");
	 strcat(wt_result->filename, idx->tag);
       }
     }
     return(wt_result);
     
   }
    
  }
  else { /* no matching index with all the criteria */
#ifdef DEBUGSTUFF
    printf("no index mapping found for the term '%s'\n",
	   attrPlusTerm->term->data);
#endif
    if (use == -1)
      diagnostic_set(114,0,local_index_name);
    else
      diagnostic_set(114,use,NULL);
    
  }
  
  return(NULL);
}

