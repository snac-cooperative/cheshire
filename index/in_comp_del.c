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
*	Header Name:	in_comp_del.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routine to delete all component references, their
*                       keyword index and postings for a given record, 
*                       as well as all exact key index and postings
*                       entries for the Cheshire II system. 
*
*	Usage:		in_comp_del (cf, filename, recnum)
*                       cf is the configfile entry for the file
*                       filename can be the full pathname or short "nickname"
*                       recnum is the logical record ID number for the record.
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

extern SGML_Data *get_component_data(int component_id,  component_list_entry *comp, 
			      SGML_Document **doc);

extern config_file_info *find_file_config(char *filename);

SGML_Data *define_span(SGML_Data *dat, SGML_Tag_Data *end_data);

extern void free_tag_data_list(SGML_Tag_Data *in_tags);

void free_span_data(SGML_Data *span_dat);

extern int in_extract(SGML_Document *sgml_rec, char **xkey_buffer, 
		      idx_key *in_key,
	       Tcl_HashTable *hash_tab, int recnum, idx_list_entry *idx,
	       SGML_Tag_Data *comptagdata, int batch_flag);



int 
in_comp_del (config_file_info *cf, char *filename, int recnum)
{
  DB *index_db, *component_db, *cf_component_open();
  FILE *index_stoplist;
  component_list_entry *comp;
  component_data_item out_comp_data;
  idx_list_entry *idx, *cf_getidx();
  int component_id_start;
  int component_id_end;
  int compid;
  int reduce_rec_count;
  char *file_name;
  char temp[500];
  char *word_ptr;
  char *xkey_buffer = NULL;
  char *tempptr;
  extern int sgml_parse_error;
  SGML_Document *sgmlrec;   /* SGML record from mainfile */
  extern SGML_Document *current_sgml_document;
  SGML_Document *doc;
  SGML_Tag_Data tagdata;
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  
  HT_DATA *ht_data;
  POSDATA *posdata, *prevpos;

  idx_result *newinfo;
  DBT keyval;
  DBT dataval;
  int returncode;

  sgmlrec = current_sgml_document;
  
  if (sgmlrec == NULL) { 
    fprintf(stderr, "in_comp_del: No SGML record %d\n", recnum);
    fprintf(LOGFILE, "in_comp_del: No SGML record %d \n", recnum);
    fflush(LOGFILE);
    return(FAIL);
  }

  if (sgmlrec->record_id != recnum) {
    fprintf(stderr, "in_comp_del: SGML record %d doesn't match current record\n", recnum);
    fprintf(LOGFILE, "in_comp_del: SGML record %d doesn't match current record\n", recnum);
    fflush(LOGFILE);
    return(FAIL);
  }

  if (cf == NULL)
    cf = find_file_config(filename);
  
  /* Allocate the hash table for collecting the words for each index */
  hash_tab = &hash_tab_data;
  
  for (comp = cf->components; comp; comp = comp->next_entry) {
    
    /* we process each component in turn for this record */
    if (comp->comp_db == NULL) {
      if ((component_db = cf_component_open(filename, comp->name)) == NULL) {
	fprintf(stderr, "in_comp_del: Could not open component %s\n",
		comp->name);
	fprintf(LOGFILE, "in_comp_del: Could not open component %s\n",
		comp->name);
	fflush(LOGFILE);
	exit(0);
      }
    }
    else
      component_db = comp->comp_db;
    
    /* we now find and extract each component for this record */
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
    sprintf(temp,"DOC %012d", recnum);
    keyval.data = (void *)temp;
    keyval.size = strlen(temp) + 1; /* include null */
    returncode = comp->comp_db->get(comp->comp_db, NULL, &keyval, &dataval, 0);
    if (returncode == 0) { /* the component exists in the index */
      /* DB doesn't fetch things into aligned storage, so we need to copy */
      memcpy((char *)&out_comp_data, (char *)dataval.data, dataval.size);
    }
    else {
      /* no need to complain -- not all items will have all components */
      return FAIL;
    }

    component_id_start = out_comp_data.start_offset;
    component_id_end = out_comp_data.end_offset;

    /* delete the component record */
    returncode = comp->comp_db->del(comp->comp_db, NULL, &keyval, 0);

    if (returncode != 0) {
      fprintf(stderr, "in_comp_del: Could not delete component for rec #%d\n",
	      recnum);
      fprintf(LOGFILE, "in_comp_del: Could not delete component for rec #%d\n",
	      recnum);
      fflush(LOGFILE);
      return FAIL;
    }
    
    /* we now loop through each of the components, fetching the data */
    /* and removing index entries.                                   */
    doc = sgmlrec;

    for (compid = component_id_start; compid <= component_id_end; compid++) {
    
      tagdata.item = get_component_data(compid, comp, &doc);
      tagdata.next_tag_data = NULL;
     
     /* now we have data for this component */
       
      
     for (idx = comp->indexes ; idx ; idx = idx->next_entry) {
	
       reduce_rec_count = 0;
       index_db = (DB *) cf_index_open(filename, idx->tag, INDEXFL);
       index_stoplist = (FILE *) cf_index_open(filename, idx->tag, INDEXSTOPWORDS);
	
       Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);
	
	
       /* search the sgml data and match against index keys */
       /* for this index -- extract matching keys and put them into the   */
       /* hash table for addition to the index file.                      */
       
       in_extract(sgmlrec, &xkey_buffer, idx->keys, hash_tab, 
		  compid, idx, &tagdata, 0);
       
       /* Initialize the key/data pair so the flags aren't set. */
       memset(&keyval, 0, sizeof(keyval));
       memset(&dataval, 0, sizeof(dataval));
       
       /* go through the hash table for this index and remove */
       /* the new terms and the postings information          */
       
       for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
	    entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {
	 
	  /* get the word */
	  word_ptr = Tcl_GetHashKey(hash_tab,entry); 
	  /* and the frequency info */
	  ht_data =  (struct hash_term_data *) Tcl_GetHashValue(entry);
	  
	  keyval.data = (void *) word_ptr;
	  keyval.size = strlen(word_ptr) + 1;
	  
	  returncode = index_db->get(index_db, NULL, &keyval, &dataval, 0); 
      
	  if (returncode == 0) { /* the word is in the index */
	    
	    /* allocate space for the posting info */
	    newinfo = (idx_result *) 
	      CALLOC(int, 
		     ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
	    memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
	    
	    if (newinfo->num_entries == 1) {
	      /* delete it from the index */
	      returncode = index_db->del(index_db, NULL, &keyval, 0);
	      if (returncode != 0)
		fprintf(LOGFILE,"error on index delete errno is %d: %s\n", 
			errno, strerror(errno)); 
	      returncode = index_db->sync(index_db, 0); 
	      if (returncode != 0)
		fprintf(LOGFILE,"error on index sync errno is %d\n", errno); 
	    }
	    else { /* there will be postings records */
	      int i;
	      int foundflag = 0;
	      /* change the postings information */
	      for (i = 0; i < newinfo->num_entries; i++) {
		if (foundflag) { /* migrate all the entries */
		  newinfo->entries[i-1] = newinfo->entries[i];
		}
		if (newinfo->entries[i].record_no == recnum) 
		  foundflag = 1;
	      }
	      /* OK to delete, there was a matching record number */
	      if (foundflag) {
		newinfo->tot_freq -= ht_data->termfreq;
		newinfo->num_entries--;
		idx->GlobalData->tot_occur -= ht_data->termfreq;      
		reduce_rec_count = 1;
		
		dataval.data = newinfo;
		dataval.size -= sizeof(idx_posting_entry);
		/* and re-write it to the index */
		returncode = index_db->put(index_db, NULL, &keyval, &dataval, 0); 
	    
		if (returncode != 0) { 
#ifdef DEBUGIND
		  printf(LOGFILE, "\nNon-zero return in db put of found term: %d\n",
			 returncode);
#endif
		  if (returncode != 0) {
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
	    }
	  } 
	  
	  /* all done with this term data hash entry */ 	
	  FREE(ht_data);
       
       }
       
       /* clear out out the current hash table for this index */
       Tcl_DeleteHashTable(hash_tab);
	
       /* update the Global information for the index */
       if (reduce_rec_count == 1) {
	   idx->GlobalData->tot_numrecs--;
	   cf_sync_global_data(idx);
       }
     }
     /* only the top-level sgml_data is created by get_component_data */
     /* all of the lower-level parts are shared                       */
     FREE (tagdata.item); 
     keyval.data = (void *)&compid;
     keyval.size = sizeof(int);
     returncode = comp->comp_db->del(comp->comp_db, NULL, &keyval, 0);

     if (returncode != 0) {
       fprintf(stderr, "in_comp_del: Could not delete component id #%d\n",
	      compid);
      fprintf(LOGFILE, "in_comp_del: Could not delete component id #%d\n",
	      compid);
      fflush(LOGFILE);
    }

    } /* end of loop for component list */

  } /* end of loop for each component type for this record */

  return (SUCCEED);
}








