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
*	Header Name:	in_components.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routine to add all component references, their
*                       keyword index and postings 
*                       as well as all exact key index and postings
*                       entries for the Cheshire II system. 
*
*	Usage:		in_components (cf, filename, recnum, sgmlrec, 
*                                      batchflag, tempdir)
*                       cf is the configfile entry for the file
*                       filename can be the full pathname or short "nickname"
*                       recnum is the logical record ID number for the record.
*                       sgmlrec is the parsed SGML Document.
*                       if batch_flag is not 0 batch processing is being used.
*                       if batch_processing is on, tempdir can be used to
*                        specify where temporary sort files are put.
*          
*                       
*	Variables:	
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or >0 (FAIL) on error;
*                       unable to find record = -1
*                       parsing error         = -2
*                       other error           = -3
*
*	Date:		10/11/2000
*                       
*	Version:	1.0
*	Copyright (c) 2000.  The Regents of the University of California
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
extern config_file_info *cf_info_base;
extern int errno;

extern batch_files_info *batch_files;

void PutPositionInfo(idx_list_entry *idx, int recnum, int termid, 
		     HT_DATA *ht_data);


extern DB *cf_component_open(char *filename, char *componentname);

extern config_file_info *find_file_config(char *filename);

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

SGML_Data *define_span(SGML_Data *dat, SGML_Tag_Data *end_data);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);

void free_span_data(SGML_Data *span_dat);

extern int in_extract(SGML_Document *sgml_rec, char **xkey_buffer, 
		      idx_key *in_key,
	       Tcl_HashTable *hash_tab, int recnum, idx_list_entry *idx,
	       SGML_Tag_Data *comptagdata, int batch_flag);



int 
in_components (config_file_info *cf, char *filename, int recnum, 
		   SGML_Document *sgmlrec, int batch_flag, 
		   char *temp_file_dir)
{

  DB *index_db, *component_db, *cf_component_open();
  FILE *index_stoplist;
  component_list_entry *comp;
  component_data_item out_comp_data;
  idx_list_entry *idx, *cf_getidx();
  int component_id_start;
  
  char *file_name;
  char temp[500];
  char *word_ptr;
  SGML_Tag_Data *tag_data, *end_data;
  SGML_Tag_Data *td, tagdata;
  SGML_Data *dat, *span_dat, *save_next_data;
  SGML_Data *save_sub_data, *save_data_parent;
  char *xkey_buffer = NULL;
  char *tempptr;
  batch_files_info *bf = NULL, *prev = NULL;

  extern int sgml_parse_error;

  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  
  int items_output = 0;
  int total_docsize = 0;

  HT_DATA *ht_data;
  POSDATA *posdata, *prevpos;

  idx_result *newinfo, tempinfo;
  DBT keyval;
  DBT dataval;
  int returncode;
  int output_flag;
  char *fmode;
  int casesensitive;
  
  if (sgmlrec == NULL) { 
    fprintf(stderr, "in_components: No SGML record %d\n", recnum);
    fprintf(LOGFILE, "in_components: No SGML record %d \n", recnum);
    fflush(LOGFILE);
    return(FAIL);
  }

  if (sgmlrec->DTD->type > 0) 
    casesensitive = 1;
  else
    casesensitive = 0;

  if (cf == NULL)
    cf = find_file_config(filename);
  
  /* Allocate the hash table for collecting the words for each index */
  hash_tab = &hash_tab_data;
  
  for (comp = cf->components; comp; comp = comp->next_entry) {

    /* we process each component in turn for this record */
    if (comp->comp_db == NULL) {
      if ((component_db = cf_component_open(filename, comp->name)) == NULL) {
	fprintf(stderr, "in_components: Could not open component %s\n",
		comp->name);
	fprintf(LOGFILE, "in_components: Could not open component %s\n",
		comp->name);
	fflush(LOGFILE);
	exit(0);
      }
    }
    else
      component_db = comp->comp_db;

    component_id_start = comp->max_component_id + 1;

    /* we now find and extract each component */
    /* call the "comp_tag_list()" function to get the entries  */
    /* this is only the starting points if there is an end tag */
    tag_data = comp_tag_list(comp->start_tag, 
			     sgmlrec->Tag_hash_tab, NULL, casesensitive);

    if (comp->end_tag) {
      end_data = comp_tag_list(comp->end_tag,
			 sgmlrec->Tag_hash_tab, NULL, casesensitive);
    }
    else 
      end_data = NULL;

    /* we now have a list of components for this record... */
    output_flag = 0;

    for (td = tag_data; td ; td = td->next_tag_data) {
      
      /* Get the component data and set the new component id */
      dat = td->item;
      output_flag = 1;
      /* for components we (temporarily) truncate any list of "next" elements */
      if (end_data == NULL) {
	save_data_parent = dat;
	save_next_data = dat->next_data_element;
	save_sub_data = dat->sub_data_element;
	dat->next_data_element = NULL;
	span_dat = NULL;
      }
      else {
	/* we are matching a span, so we build a composite data struct */
	/* with no "next" or sub items                                 */
	save_data_parent = NULL;
	save_next_data = NULL;
	save_sub_data = NULL;
	span_dat = dat = define_span(dat, end_data);
      }

      tagdata.item = dat;
      tagdata.next_tag_data = NULL;

      comp->max_component_id++;

      for (idx = comp->indexes ; idx ; idx = idx->next_entry) {
#ifdef DEBUGIND
	printf("index tag = %s\n", idx->tag);
#endif
	
	index_db = (DB *) cf_index_open(filename, idx->tag, INDEXFL);
	index_stoplist = (FILE *) cf_index_open(filename, idx->tag, INDEXSTOPWORDS);
	
	Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);
	
	if (batch_flag) { /* this is a batch run, set up the file for output */
	  
	  for (bf = batch_files; bf != NULL; bf = bf->next) {
	    if (idx == bf->idx)
	      break;
	    prev = bf;
	  }
	  if (bf == NULL || batch_files == NULL) {
	    /* first entry */
	    bf = CALLOC(batch_files_info, 1);
	    if (batch_files == NULL) 
	      batch_files = bf;
	    else
	      prev->next = bf;
	    bf->idx = idx;
	    bf-> index_name = idx->name;
	    bf->load_flag = 0; /* always zero except in batch_load fixes */
	    if (temp_file_dir) {
	      file_name = strrchr(bf->index_name, '/');
	      file_name++;
	      sprintf(temp,"%s/%s_BATCHTEMP", temp_file_dir, file_name);
	    }
	    else
	      sprintf(temp,"%s_BATCHTEMP", bf->index_name);
#ifdef WIN32
	    fmode = "wb";
#else 
	    fmode = "w";
#endif
	    
	    if ((bf->outfile = fopen(temp, fmode)) == NULL) {
	      fprintf(LOGFILE, "Couldn't open batch file %s\n", temp);
	      return (FAIL);
	    }
	  }
	}
	
	/* search the sgml data and match against index keys */
	/* for this index -- extract matching keys and put them into the   */
	/* hash table for addition to the index file.                      */
	
	in_extract(sgmlrec, &xkey_buffer, idx->keys, hash_tab, 
		   comp->max_component_id, idx, &tagdata, batch_flag);
	
	if (idx->type & PAGEDINDEX) {
	  /* do nothing -- already output at a lower level than in_extract */
	  /* except clear out out the current hash table for this index */
	  Tcl_DeleteHashTable(hash_tab);
	}
	else {
	  /* go through the hash table for this index and record */
	  /* the new terms and the postings information          */
	  items_output = 0;
	  
	  /* Initialize the key/data pair so the flags aren't set. */
	  memset(&keyval, 0, sizeof(keyval));
	  memset(&dataval, 0, sizeof(dataval));
	  
	  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search); 
	       entry != NULL; 
	       entry = Tcl_NextHashEntry(&hash_search)) {
	    /* get the word */
	    word_ptr = Tcl_GetHashKey(hash_tab,entry); 
	    /* and the frequency info */
	    ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
#ifdef DEBUGIND
	    printf("word: %s\n", word_ptr);
#endif
	    if (idx->type & EXTERNKEY) {
	      /* for external files accumulate the raw word counts for */
	      /* recording document size for retrieval calculations    */
	      total_docsize += ht_data->termfreq ;
	    }
	    
	    /* if this is NOT a batch load, insert the terms directly */
	    if (batch_flag == 0) {
	      keyval.data = (void *) word_ptr;
	      keyval.size = strlen(word_ptr) + 1;
	      
	      returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
	      
	      if (returncode == 0) { /* found the word in the index already */
		int temprecnum = 0;
		int tempfreq = 0;
		
		/* allocate space for the posting info */
		newinfo = (idx_result *) 
		  CALLOC(int, 
			 ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
		memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
		
#ifdef DEBUGIND
		printf("outputting hash table word: %s - termid: %d\n", 
		       word_ptr,newinfo->termid);
#endif
		/* now increment the counters */
		newinfo->tot_freq += ht_data->termfreq;
		newinfo->num_entries++;
		idx->GlobalData->tot_occur += ht_data->termfreq;
		newinfo->entries[newinfo->num_entries-1].record_no = comp->max_component_id;
		newinfo->entries[newinfo->num_entries-1].term_freq 
		  = ht_data->termfreq;
		dataval.data = newinfo;
		dataval.size += sizeof(idx_posting_entry);
		
		/* and write it to the index */
		returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
		items_output++;
		
		if (returncode != 0) { 
#ifdef DEBUGIND
		  printf("\nNon-zero return in db put of found term: %d\n", 
			 returncode);
#endif
		  if (returncode == DB_RUNRECOVERY) {
		    fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
		    exit(999);
		  } 
		  if (returncode == DB_LOCK_NOTGRANTED) {
		    fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
		    exit(999);
		  } 
		  if (returncode == DB_LOCK_DEADLOCK) {
		    fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
		    exit(999);
		  } 
		}
		if (idx->type & PROXIMITY)
		  PutPositionInfo(idx, comp->max_component_id, newinfo->termid, ht_data);
		
		FREE(newinfo);
	      }
	      else if (returncode == DB_NOTFOUND) { 
		/* a new word that has no freq data */
		tempinfo.termid = idx->GlobalData->recptr++;
		tempinfo.tot_freq = ht_data->termfreq;
		tempinfo.entries[0].record_no = comp->max_component_id;
		tempinfo.entries[0].term_freq = ht_data->termfreq;
		tempinfo.num_entries = 1;
		idx->GlobalData->tot_occur += ht_data->termfreq;
		dataval.size = sizeof(tempinfo);
		dataval.data = (void *)&tempinfo;
		
#ifdef DEBUGIND
		printf("output new hash table word: %s - termid: %d\n", 
		       word_ptr,tempinfo.termid);
#endif
		/* put the new word into the index */
		returncode = index_db->put(index_db, NULL, &keyval, &dataval, 
					   DB_NOOVERWRITE); 
		items_output++;
		
		if (returncode != 0) { 
		  if(returncode == 1)
		    fprintf(LOGFILE, "Duplicate term in db put : %s\n",word_ptr);
		  else {
		    fprintf(LOGFILE, "Non-zero return in db put of new term: %d\n", 
			    returncode);

		    if (returncode == DB_RUNRECOVERY) {
		      fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
		      exit(999);
		    } 
		    if (returncode == DB_LOCK_NOTGRANTED) {
		      fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
		      exit(999);
		    } 
		    if (returncode == DB_LOCK_DEADLOCK) {
		      fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
		      exit(999);
		    }
		  }
		}
		if (idx->type & PROXIMITY)
		  PutPositionInfo(idx, comp->max_component_id, tempinfo.termid, ht_data);
		
		/* no output to postings for single record items */
	      } 
	    }
	    else { /* BATCH -- output the temp entries for this index */
	      /* first remove any 001 chars from the word, just in case... */
	      for (tempptr = strchr(word_ptr, (int)'\001');
		   tempptr != NULL; tempptr = strchr(word_ptr, (int)'\001')) {
		*tempptr = '?';
	      }
	      
	      fprintf(bf->outfile, "%s\t%012d\t%d", word_ptr, comp->max_component_id, 
		      ht_data->termfreq);
	      
	      if (idx->type & PROXIMITY) {
		/* output the positional info for each occurrence */
		fprintf(bf->outfile,"\001");
		for (posdata = ht_data->pos; posdata; posdata = posdata->next) {
		  fprintf(bf->outfile," %d", posdata->position);
		}
	      }
#ifndef WIN32
	      fprintf(bf->outfile, "\n");
#else
	      /* to ensure size matching after sorting... */
	      fprintf(bf->outfile, "\r\n");
#endif
	      
#ifdef DEBUGIND
	      printf("PUTTING %s\t%d\t%d\n", word_ptr, 
		     comp->max_component_id, 
		     ht_data->termfreq);
#endif
	    }
	    
	    /* all done with this term data hash entry */
	    if (idx->type & PROXIMITY) {
	      prevpos = ht_data->pos;
	      for (posdata = ht_data->pos->next; posdata; posdata = posdata->next) {
		FREE(prevpos);
		prevpos = posdata;
	      }
	      FREE(ht_data->last);
	    }
	    FREE(ht_data);
	  }
	  
	  /* clear out out the current hash table for this index */
	  Tcl_DeleteHashTable(hash_tab);
	  memset(hash_tab,0,sizeof(Tcl_HashTable));
	  
	  if (items_output != 0) {
	    idx->GlobalData->tot_numrecs++;
	    /* cf_sync_global_data(idx); don't sync here */
	  }
	}
      } /* end of index loop */

      /* restore the data linkages */
      if (save_data_parent) {
	save_data_parent->next_data_element = save_next_data;
	save_data_parent->sub_data_element = save_sub_data;
      }
      /* now we add the component to the component index file */
      out_comp_data.record_id = recnum ;
      out_comp_data.start_offset = dat->start_tag_offset;
      out_comp_data.end_offset = dat->data_end_offset;
      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));
      keyval.data = (void *)&(comp->max_component_id);
      keyval.size = sizeof(int);
      dataval.data = (void *)&out_comp_data;
      dataval.size = sizeof(component_data_item);
      returncode = comp->comp_db->put(comp->comp_db, NULL, &keyval, &dataval, 0);
      if (returncode != 0) { 
	if(returncode == 1)
	  fprintf(LOGFILE, "Duplicate component id in db put : %d\n",comp->max_component_id);
	else {
	  fprintf(LOGFILE, "Non-zero return in db put of new term: %d\n", 
		  returncode);
	  if (returncode == DB_RUNRECOVERY) {
	    fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
	    exit(999);
	  } 
	  if (returncode == DB_LOCK_NOTGRANTED) {
	    fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
	    exit(999);
	  } 
	  if (returncode == DB_LOCK_DEADLOCK) {
	    fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
	    exit(999);
	  }
	}
      }
      /* if we have a defined span -- free the duplicated SGML_Data items */
      if (span_dat) {
	free_span_data(span_dat);
	span_dat = NULL;
      }
 
    } /* end of loop for tag_data component list */
    /* loop for each component -- post processing after indexing */
    /* output the DOC record for the components                  */
    if (output_flag) {
      out_comp_data.record_id = recnum ;
      out_comp_data.start_offset = component_id_start;
      out_comp_data.end_offset = comp->max_component_id;
      memset(&keyval, 0, sizeof(keyval));
      memset(&dataval, 0, sizeof(dataval));
      sprintf(temp,"DOC %012d", recnum);
      keyval.data = (void *)temp;
      keyval.size = strlen(temp) + 1; /* include null */
      dataval.data = (void *)&out_comp_data;
      dataval.size = sizeof(component_data_item);
      returncode = comp->comp_db->put(comp->comp_db, NULL, &keyval, &dataval, 0);
      if (returncode != 0) { 
	if(returncode == 1)
	  fprintf(LOGFILE, "Duplicate component id in db put : %d\n",comp->max_component_id);
	else {
	  fprintf(LOGFILE, "Non-zero return in db put of new term: %d\n", 
		  returncode);
	  if (returncode == DB_RUNRECOVERY) {
	    fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
	    exit(999);
	  } 
	  if (returncode == DB_LOCK_NOTGRANTED) {
	    fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
	    exit(999);
	  } 
	  if (returncode == DB_LOCK_DEADLOCK) {
	    fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
	    exit(999);
	  } 
	}
      }
    }
    free_tag_data_list(tag_data);
    free_tag_data_list(end_data);
  } /* end of loop for each component type for this record */

  /*  We no longer sync on each record, but only at closing or on error  
   *   for (idx = cf_getidx(filename) ; idx ; idx = idx->next_entry) 
   *   cf_sync_global_data(idx);
   */
  return (SUCCEED);
}


void free_span_data(SGML_Data *span_dat)
{
  if (span_dat == NULL)
    return;
  
  free_span_data(span_dat->next_data_element);
  free_span_data(span_dat->sub_data_element);
  
  FREE(span_dat);
}


SGML_Data *
dup_span_data(SGML_Data *dat, SGML_Data *end_data) 
{
  SGML_Data *subdata, *nextdata, *new_dat;

  if (dat == NULL)
    return (NULL);

  if (dat->start_tag_offset >= end_data->data_end_offset)
    return (NULL);
  
  subdata = dup_span_data(dat->sub_data_element, end_data);
  nextdata = dup_span_data(dat->next_data_element, end_data);

  new_dat = CALLOC(SGML_Data, 1);
  /* structure assignment */
  *new_dat = *dat;
  if (new_dat->data_end_offset > end_data->data_end_offset) {
    new_dat->end_tag = end_data->end_tag;
    new_dat->data_end = end_data->data_end;
    new_dat->content_end = end_data->content_end;
    new_dat->end_tag_offset = end_data->end_tag_offset;
    new_dat->data_end_offset = end_data->data_end_offset;
    new_dat->content_end_offset = end_data->content_end_offset;
    new_dat->next_data_element = NULL;
    new_dat->sub_data_element = subdata;
  }
  else {
    new_dat->next_data_element = nextdata;
    new_dat->sub_data_element = subdata;
  }
  return (new_dat);
  
}

SGML_Data *find_last_data(SGML_Data *dat)
{
  if (dat->next_data_element == NULL)
    return (dat);
  else
    return(find_last_data(dat->next_data_element));
}

SGML_Data *
define_span(SGML_Data *dat, SGML_Tag_Data *end_data) 
{
  SGML_Tag_Data *ed;
  SGML_Data *end_dat, *new_dat, *last_dat;

  end_dat = NULL;
  new_dat = CALLOC(SGML_Data, 1);
  /* structure assignment */
  *new_dat = *dat;

  new_dat->next_data_element = NULL;
  new_dat->sub_data_element = NULL;

  if (new_dat->parent->processing_flags == PROC_FLAG_IN_CHILD) {
    /* possible items that need processing instruction support */
    new_dat->processing_flags = PROC_FLAG_IN_CHILD;
  }

  /* assumes that the end_data info is in sequence from first to last */
  /* which is how it comes from the hash table.                       */
  for (ed = end_data; ed && end_dat == NULL; ed = ed->next_tag_data) {
    if (dat->start_tag_offset >= ed->item->start_tag_offset) {
      continue;
    }
    end_dat = ed->item;

  }
  if (end_dat != NULL) {
    /* patch the end points of the new_dat element */
    new_dat->content_end = end_dat->start_tag;
    new_dat->end_tag = end_dat->start_tag;
    new_dat->data_end = end_dat->start_tag;
    new_dat->content_end_offset = end_dat->start_tag_offset;
    new_dat->end_tag_offset = end_dat->start_tag_offset;
    new_dat->data_end_offset = end_dat->start_tag_offset;
  }

  /* build a copy of the partial sub and next structure within the bounds */
  /* of the span                                                          */

  new_dat->sub_data_element = dup_span_data(dat->sub_data_element, new_dat);

  /* also may need to treat SOME next elements as sub elements */
  if (new_dat->sub_data_element == NULL &&
      dat->next_data_element != NULL)
    new_dat->sub_data_element = dup_span_data(dat->next_data_element, new_dat);
  else {
    if (dat->next_data_element != NULL) {
      last_dat = find_last_data(new_dat->sub_data_element);
      last_dat->next_data_element = dup_span_data(dat->next_data_element, new_dat);
    }
  }

  return (new_dat);
}








