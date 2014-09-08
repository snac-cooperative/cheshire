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
*	Header Name:	cf_close.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to close files under the control of
*                       the configfile structure and return file 
*                       pointers
*	Usage:		FILE *cf_close(filename, which)
*                       filename can be the full pathname or short "nickname"
*                       which can be MAINFILE, ASSOCFILE, POSTINGFILE;
*                       (defined in cheshire.h)
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		10/21/93
*	Revised:	11/7/93 (added hash handling and single close)
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"

/* forward declaration */
DB *close_index_db (char *idxname, idx_list_entry *indexes_head, 
		    idx_list_entry *idx_in, DB *dbptr);

config_file_info *find_file_config(char *filename);
idx_list_entry *find_index_name(config_file_info *cf, char *indexname);

extern config_file_info *cf_info_base;
extern char cf_open_file_flags[];

cf_close(char *filename, int which)
{
  continuation *cont;
  config_file_info *cf;
  FILE *temp;
  int returncode;

  cf = find_file_config(filename);
  temp = NULL;

  if (cf) {      

    if (cf->file_type == FILE_TYPE_DBMS) {
      return (cf_close_DBMS(cf));
    }

    if (cf->file_type > 99) {
      if (cf->file_db) {
	returncode = cf->file_db->close(cf->file_db,0);
	cf->file_db = NULL;
      }
    }

    if (cf->file_ptr) {
      fclose(cf->file_ptr);
      temp = cf->file_ptr;
      cf->file_ptr = NULL;
    }
    if (cf->assoc_ptr) {
      fclose(cf->assoc_ptr);
      cf->assoc_ptr = NULL;
    }
    if (cf->history_ptr) {
      fclose(cf->history_ptr);
      cf->history_ptr = NULL;
    }
    if (cf->docsize_ptr) {
      fclose(cf->docsize_ptr);
      cf->docsize_ptr = NULL;
    }
    /* close any continuation files */
    for (cont = cf->filecont; cont; cont = cont->next_cont) {
	if (cont->file != NULL && (temp != NULL && temp != cont->file)) 
	  fclose(cont->file);
	cont->file = NULL;
    }
    return(0);
  }

  return (0);
}


DB *cf_index_close(char *filename, char *indexname, int which)
{
  
  continuation *cont;
  config_file_info *cf;
  idx_list_entry *idx;
  DB *init_index_db();

  cf = find_file_config(filename);

  if (cf) {        
    idx = find_index_name(cf, indexname);
    if (idx) {
      if (idx->db) {
	idx->db = close_index_db(idx->name, cf->indexes, idx, idx->db);
      }
      if (idx->type & PROXIMITY) { 
	idx->prox_db->close(idx->prox_db,0);
	idx->prox_db = NULL;
      }

      if (idx->type & VECTOR_TYPE) { 
	idx->vector_db->close(idx->vector_db,0);
	idx->vector_db = NULL;
      }

      if (idx->page_rec_assoc_file) {
	fclose(idx->page_rec_assoc_file);
	idx->page_rec_assoc_file = NULL;
      }
      if (idx->page_file_names) {
	fclose(idx->page_file_names);
	idx->page_file_names = NULL;
      }
    }
  }
  
  return (0);
}


/* close the database index file if it is open only one place */
/* otherwise sync the index and send back a null for the current close */
DB *close_index_db (char *idxname, idx_list_entry *indexes_head, 
		    idx_list_entry *idx_in, DB *dbptr)
{
  Tcl_HashTable *hash_tab;
  Tcl_HashEntry *entry;
  int exists;
  idx_list_entry *idx;

  exists = 0;


  /* check the global hash table of names */
  hash_tab = IndexNamesHash;
  entry = Tcl_FindHashEntry(hash_tab,idxname);

  if (entry == NULL){
    return (NULL); /* never opened? or already closed */
  }
  else {

    for (idx = indexes_head; idx ; idx = idx->next_entry) {
      if (strcmp(idx->name, idxname) == 0 && idx->db == dbptr) 
	exists++;
    }

    if (exists > 0 && !(cf_open_file_flags[0] == 'r' 
			&& strlen(cf_open_file_flags) == 1)) {
      cf_sync_global_data(idx_in);
    }

    if (exists == 1) {
      /* really close it */
      dbptr->close(dbptr,0);
      idx_in->db = NULL;
      if (idx_in->GlobalData)
	FREE(idx_in->GlobalData);
      idx_in->GlobalData = NULL;
      Tcl_DeleteHashEntry(entry);
    }
    else if (exists > 1) {
      /* it's open for another index too ... so we will */
      /* just make sure the data is all on disk         */
      dbptr->sync(dbptr,0);
    }
    else if (exists == 0) {
      /* already closed up */
    }
  }
  /* always return NULL */
  return (NULL);
}

/* synchronize the global data and the cached global data for */
/* global index frequency information                         */
int
cf_sync_global_data(idx_list_entry *idx) 
{
  char srchbuffer[30];
  DB *init_index_db();
  DBT keyval;
  DBT dataval;
  int returncode;

  /* update the Global information for the index */
  sprintf(srchbuffer, ">>>Global Data Values<<<");

  memset(&keyval,0,sizeof(DBT));
  memset(&dataval,0,sizeof(DBT));

  keyval.data = (void *) srchbuffer;
  keyval.size = strlen(srchbuffer) + 1;
        
  dataval.data = (void *) idx->GlobalData;
  dataval.size = sizeof(dict_info);
    
  returncode = idx->db->put(idx->db, NULL, &keyval, &dataval, 0); 

  if (returncode != 0) { 
#ifdef DEBUG
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
    
  /* just flush everything output so far to disk    */
  returncode = idx->db->sync(idx->db,0);    

  if (returncode != 0) { 
#ifdef DEBUG
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

  /* if this is a page index, also put out the page count info */
  if (cf_pagedindex(idx)) {
    page_assoc_rec data;
    data.parentid = idx->highest_page_number;
    data.recsize = 0;
    data.nameoffset = 0;
    fseek(idx->page_rec_assoc_file, 0, 0); 
    fwrite(&data,sizeof(page_assoc_rec),1, 
	   idx->page_rec_assoc_file);		
  }  

  /* if this is a proximity index, sync the prox data too */
  if (idx->type & PROXIMITY) {
    returncode = idx->prox_db->sync(idx->prox_db,0);    
  }

  /* if this is a proximity index, sync the prox data too */
  if (idx->type & VECTOR_TYPE) {
    returncode = idx->vector_db->sync(idx->vector_db,0);    
  }

  return (returncode);
}

/* synchronize the index file and flush the cached data for */
/* the index -- also frees all allocated index buffers ??   */
cf_sync_index(idx_list_entry *idx) 
{

  cf_sync_global_data(idx);

}


/* synchronize the global data and the cached global data for */
/* global index frequency information                         */
close_comp_db(component_list_entry *comp) 
{
  char srchbuffer[30];
  DB *init_index_db();
  DBT keyval;
  DBT dataval;
  int returncode;

#ifdef WIN32
  if (!(cf_open_file_flags[0] == 'r' && cf_open_file_flags[1] == 'b'
	&& strlen(cf_open_file_flags) == 2)) {
#else
  if (!(cf_open_file_flags[0] == 'r' 
	&& strlen(cf_open_file_flags) == 1)) {
#endif
    
    if (comp->max_component_id != 0) {
      

      /* update the Global information for the components */
      sprintf(srchbuffer, ">>>COMPONENT COUNT<<<");

      memset(&keyval,0,sizeof(DBT));
      memset(&dataval,0,sizeof(DBT));
      
      keyval.data = (void *) srchbuffer;
      keyval.size = strlen(srchbuffer) + 1;
      
      dataval.data = (void *) &(comp->max_component_id);
      dataval.size = sizeof(int);
      
      returncode = comp->comp_db->put(comp->comp_db, NULL, &keyval, &dataval, 0); 
      
      if (returncode != 0) { 
#ifdef DEBUG
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
    }
  }

  /* just close it up (flushing output so far to disk)  */
  comp->comp_db->close(comp->comp_db,0);
  comp->comp_db = NULL;
 
}



void
cf_close_files(char *fname)
{
  idx_list_entry *indexes, *idx, *cf_getidx();
  cluster_list_entry *cl, *cf_getcluster();
  config_file_info *cf; 
  component_list_entry *comp;

  cf = find_file_config(fname);

  cf_close(fname, MAINFILE);

  for (indexes = idx = cf_getidx(fname); idx ; idx = idx->next_entry) {
    if (idx->db) { /* close the DB file and remove hash entries */
      idx->db = close_index_db(idx->name, indexes, idx, idx->db);
    }

    if (idx->stopwords_file) {
      fclose(idx->stopwords_file);
      idx->stopwords_file = NULL; /* but we won't free the hash table here */
    }
    if (idx->expansion_file) {
      fclose(idx->expansion_file);
      idx->expansion_file = NULL; /* but we won't free the hash table here */
    }

  }

  for (comp = cf->components; comp ; comp = comp->next_entry) {
    if (comp->comp_db) {
      close_comp_db(comp);
    }
    for (idx = comp->indexes; idx ; idx = idx->next_entry) {
      if (idx->db) { /* close the DB file and remove hash entries */
	idx->db = close_index_db(idx->name, comp->indexes, idx, idx->db);
      }
      if (idx->stopwords_file) {
	fclose(idx->stopwords_file);
	idx->stopwords_file = NULL;
      }
      if (idx->expansion_file) {
	fclose(idx->expansion_file);
	idx->expansion_file = NULL;
      }
    }
  }

  for (cl = cf_getcluster(fname); cl; cl = cl->next_entry) {
    if (cl->temp_file)
      fclose(cl->temp_file);
      cl->temp_file = NULL;
  }
}



