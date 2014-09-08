/************************************************************************
*
*	Header Name:	cf_closeall.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to close or reopen all files under the 
*                       control of the configfile structure. 
*                       cf_closeall should only be run once at the end of 
*                       a program run, to safely close all files and indexes.
*                       cf_reopen can be called any time to force "syncing"
*                       and flushing of all files to disk.
*
*	Usage:		FILE *cf_closeall()
*                             cf_reopen_all() 
*
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns -1 on error 0 on normal completion;
*
*	Date:		10/21/93
*	Revised:	11/7/93 (added hash handling and single close)
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

extern config_file_info *cf_info_base;

cf_closeall()
{
  continuation *cont;
  config_file_info *cf;
  idx_list_entry *idx;
  cluster_list_entry *clus;

  for (cf = cf_info_base; cf; cf = cf->next_file_info) {

    if (cf->file_ptr) 
      fclose(cf->file_ptr);
    if (cf->assoc_ptr)
      fclose(cf->assoc_ptr);
    if (cf->history_ptr) 
      fclose(cf->history_ptr);
    if (cf->docsize_ptr) 
      fclose(cf->docsize_ptr);

    /* close any continuation files */
    for (cont = cf->filecont; cont; cont = cont->next_cont) {
      if (cont->file) 
	fclose(cont->file);
    }


    for (idx = cf->indexes; idx ; idx = idx->next_entry) {
      if (idx->db) { /* close the DB file and remove hash entries */
	idx->db = close_index_db(idx->name, cf->indexes, idx, idx->db);
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

      if (idx->assoc_file)  
	fclose(idx->assoc_file);
      if (idx->postings_file) 
	fclose(idx->postings_file);
      if (idx->stopwords_file)
	fclose(idx->stopwords_file);
      if (idx->page_rec_assoc_file)
	fclose(idx->page_rec_assoc_file);
      if (idx->page_file_names) 
	fclose(idx->page_file_names);

    }
    /* close all cluster files */
    for (clus = cf->clusters; clus ; clus = clus->next_entry) {
      if (clus->temp_file)
	fclose(clus->temp_file);
      if (clus->cluster_stoplist)
	fclose(clus->cluster_stoplist);
      
    }
  }

  if (cf_info_base) {
    /* since everything is shut down, free all the structures and zero */
    /* out cf_info_base                                                */
    if (IndexNamesHash)
      FREE(IndexNamesHash);
    cf_free_info(cf_info_base);
    cf_info_base = NULL;
  }

}

/* the following routine close all the main files, closes and then re-opens */
/* all of the index files */
/* this is an attempt to work-around what seems to be a solaris bug */

cf_reopen_all()
{
  continuation *cont;
  config_file_info *cf;
  idx_list_entry *idx;

  for (cf = cf_info_base; cf; cf = cf->next_file_info) {

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

    /* close any continuation files */
    for (cont = cf->filecont; cont; cont = cont->next_cont) {
      if (cont->file) {
	fclose(cont->file);
	cont->file = NULL;
      }
    }


    for (idx = cf->indexes; idx ; idx = idx->next_entry) {

      /* close and re-open the index */
      cf_sync_index(idx);

      /* close any index continuation files */
      for (cont = idx->indxcont; cont; cont = cont->next_cont) {
	if (cont->file) {
	  fclose(cont->file);
	  cont->file = NULL;
	}
      }
      /* and for postings continuations */
      for (cont = idx->indxpcnt; cont; cont = cont->next_cont) {
	if (cont->file) {
	  fclose(cont->file);
	  cont->file = NULL;
	}
      }

      if (idx->assoc_file) {
	fclose(idx->assoc_file);
	if ((idx->assoc_file = fopen(idx->assoc_name, 
				     cf_open_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to reopen %s in cf_reopen_all\n", 
		      idx->assoc_name);
	  fflush(LOGFILE);
	  exit(1);
	}
      }
      if (idx->postings_file) {
	fclose(idx->postings_file);
	if ((idx->postings_file = fopen(idx->postings_name, 
					cf_open_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to reopen %s in cf_reopen_all\n", 
		      idx->postings_name);
	  fflush(LOGFILE);
	  exit(1);
	}
      }
      if (idx->stopwords_file) {
	fclose(idx->stopwords_file);
	if (idx->stopwords_name != NULL) {
	  if ((idx->stopwords_file = fopen(idx->stopwords_name, "r")) 
	      == NULL) {
	    fprintf(LOGFILE,"failed to reopen %s\n", 
		    idx->stopwords_name);
	    fflush(LOGFILE);
	    exit(1);
	  }
	}
      }
    }
  }
}

/* this should force all pending output to be written ... */
cf_sync_all_files() {
  
  sync();

}
