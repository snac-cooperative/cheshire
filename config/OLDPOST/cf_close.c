
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

/* forward declaration */
DB *close_index_db (char *idxname, idx_list_entry *indexes_head, 
		    idx_list_entry *idx_in, DB *dbptr);

config_file_info *find_file_config(char *filename);
idx_list_entry *find_index_name(config_file_info *cf, char *indexname);

extern config_file_info *cf_info_base;

cf_close(char *filename, int which)
{
  continuation *cont;
  config_file_info *cf;

  cf = find_file_config(filename);

  if (cf) {      
    if (cf->file_ptr) {
      fclose(cf->file_ptr);
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
	if (cont->file) 
	  fclose(cont->file);
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

  int exists;


  cf = find_file_config(filename);

  if (cf) {        
    idx = find_index_name(cf, indexname);
    if (idx) {
      if (idx->db) {
	idx->db = close_index_db(idx->name, cf->indexes, idx, idx->db);
      }
      if (idx->assoc_file) {
	fclose(idx->assoc_file);
	idx->assoc_file = NULL;
      }
      if (idx->postings_file) {
	fclose(idx->postings_file);
	idx->postings_file = NULL;
      }
      if (idx->page_rec_assoc_file) {
	fclose(idx->page_rec_assoc_file);
	idx->page_rec_assoc_file = NULL;
      }
      if (idx->page_file_names) {
	fclose(idx->page_file_names);
	idx->page_file_names = NULL;
      }
      /* close any index continuation files */
      for (cont = idx->indxcont; cont; cont = cont->next_cont) {
	if (cont->file) 
	  fclose(cont->file);
      }
      /* and for postings continuations */
      for (cont = idx->indxpcnt; cont; cont = cont->next_cont) {
	if (cont->file) 
	  fclose(cont->file);
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
  Tcl_HashSearch hash_search;
  int exists;
  idx_list_entry *idx;
  DB *db_handle;

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

    if (exists > 0) {
      cf_sync_global_data(idx_in);
    }

    if (exists == 1) {
      /* really close it */
      dbptr->close(dbptr);
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
cf_sync_global_data(idx_list_entry *idx) 
{
  char srchbuffer[30];
  DB *init_index_db();
  DBT keyval;
  DBT dataval;
  int returncode, nopen;

  /* update the Global information for the index */
  sprintf(srchbuffer, ">>>Global Data Values<<<");
  keyval.data = (void *) srchbuffer;
  keyval.size = strlen(srchbuffer) + 1;
        
  dataval.data = (void *) idx->GlobalData;
  dataval.size = sizeof(dict_info);
    
  returncode = idx->db->put(idx->db, &keyval, &dataval, 0); 
    
  /* just flush everything output so far to disk    */
  returncode = idx->db->sync(idx->db,0);    

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
}

/* synchronize the index file and flush the cached data for */
/* the index -- also frees all allocated index buffers ??   */
cf_sync_index(idx_list_entry *idx) 
{

  Tcl_HashTable *hash_tab;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  int exists;
  DB *db_handle;

  exists = 0;

  cf_sync_global_data(idx);

  /* THIS SHOULD HAVE BEEN ALL THAT WAS NEEDED --   */
  /* BUT DBOPEN DOESN'T MANAGE ITS BUFFERS PROPERLY */
  /* AND WILL RUN OUT OF MEMORY IF THE FILE IS NOT  */
  /* CLOSED AND RE-OPENED (DAMN PAIN IN THE ASS!!   */

  /* check the hash table of names */
  hash_tab = IndexNamesHash;
  entry = Tcl_FindHashEntry(hash_tab,idx->name);

  idx->db->close(idx->db);
  idx->db = NULL;
  if (entry) 
    Tcl_DeleteHashEntry(entry);

  /* now re-open the damn thing for the next io -- this is grossly */
  /* inefficient and wasteful but blasted dbopen is screwed up with*/
  /* a bad memory leak otherwise                                   */

  idx->db = init_index_db (idx->name, idx->type, &exists);  
}


cf_close_files(char *fname)
{
  config_file_info *cf;
  idx_list_entry *indexes, *idx, *cf_getidx();
  cluster_list_entry *cl, *cf_getcluster();
  continuation *cont;

  cf_close(fname, MAINFILE);

  for (indexes = idx = cf_getidx(fname); idx ; idx = idx->next_entry) {
    if (idx->db) { /* close the DB file and remove hash entries */
      idx->db = close_index_db(idx->name, indexes, idx, idx->db);
    }

    /* close any index continuation files */
    for (cont = idx->indxcont; cont; cont = cont->next_cont) {
      if (cont->file) 
	fclose(cont->file);
      cont->file = NULL;
    }
    /* and for postings continuations */
    for (cont = idx->indxpcnt; cont; cont = cont->next_cont) {
      if (cont->file) 
	fclose(cont->file);
      cont->file = NULL;
    }

    if (idx->assoc_file) {
      fclose(idx->assoc_file);
      idx->assoc_file = NULL;
    }
    if (idx->postings_file) {
      fclose(idx->postings_file);
      idx->postings_file = NULL;
    }
    if (idx->stopwords_file) {
      fclose(idx->stopwords_file);
      idx->stopwords_file = NULL; /* but we won't free the hash table here */
    }

  }

  for (cl = cf_getcluster(fname); cl; cl = cl->next_entry) {
    if (cl->temp_file)
      fclose(cl->temp_file);
      cl->temp_file = NULL;
  }
}


