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
extern char cf_open_file_flags[];
extern FILE *LOGFILE;

void
cf_closeall()
{
  
  continuation *cont;
  config_file_info *cf;
  idx_list_entry *idx;
  cluster_list_entry *clus;
  component_list_entry *comp;
  int env_close;
  int returncode;

  for (cf = cf_info_base; cf; cf = cf->next_file_info) {

    if (cf->file_type == FILE_TYPE_DBMS) {
      cf_close_DBMS(cf);
      continue;
    }

    if (cf->file_type > 99) {
      if (cf->file_db) {
	returncode = cf->file_db->close(cf->file_db,0);
	cf->file_db = NULL;
      }
    }

    if (cf->file_ptr && cf->filecont == NULL) 
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
      if (idx->stopwords_file)
	fclose(idx->stopwords_file);
      if (idx->page_rec_assoc_file)
	fclose(idx->page_rec_assoc_file);
      if (idx->page_file_names) 
	fclose(idx->page_file_names);

    }
    /* close components */
    for (comp = cf->components; comp ; comp = comp->next_entry) {
      if (comp->comp_db) {
	close_comp_db(comp);
	comp->comp_db = NULL;
      }
      for (idx = comp->indexes; idx ; idx = idx->next_entry) {
	if (idx->db) { /* close the DB file and remove hash entries */
	  idx->db = close_index_db(idx->name, comp->indexes, idx, idx->db);
	}
	if (idx->stopwords_file) {
	  fclose(idx->stopwords_file);
	  idx->stopwords_file = NULL;
	}
      }
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
    /* close the DB environment  */
    if (gb_dbenv != NULL) {
      env_close = gb_dbenv->close(gb_dbenv, 0);
      if (env_close != 0) 
	fprintf(LOGFILE,"Error %d in closing DBenv\n");
    }
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
  component_list_entry *comp;

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

    for (comp = cf->components; comp ; comp = comp->next_entry) {

      if (comp->comp_db) {
	comp->comp_db->close(comp->comp_db,0);
	comp->comp_db = NULL;
      }

      for (idx = comp->indexes; idx ; idx = idx->next_entry) {

	cf_sync_index(idx);
	
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
}

/* this should force all pending output to be written ... */
cf_sync_all_files() {
  
#ifdef WIN32
  _flushall();
#else
  sync();
#endif

}





