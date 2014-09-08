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
*	Header Name:	cf_createall.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to open files under the control of
*                       the configfile structure as a file creation
*                       process. This should only be run once per config file.
*
*	Usage:		FILE *cf_createall()
*
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns -1 on error 0 on normal completion;
*
*	Date:		10/21/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "cheshire.h"
#include "db.h"
#include "configfiles.h"

extern config_file_info *cf_info_base;

DB *create_index_db (char *idxname, int idxtype, int *opened);
DB *create_prox_db  (char *idxname, int idxtype, int *opened);
DB *create_vector_db  (char *idxname, int idxtype, int *opened);
extern DB *cf_component_open (char *filename, char *componentname); 

#define KEYBUFFERSIZE 200
int db_init(char *home, DB_ENV **dbenvp);

int create_indexes(idx_list_entry *idx, int *num_open, idx_list_entry *head); 

cf_createall()
{
  
  config_file_info *cf;
  idx_list_entry *idx;
  component_list_entry *comp;

  char keybuffer[KEYBUFFERSIZE];
  char namebuff[500];
  int exists;
  int empty=0, num_open;
  int index_return;

  for (cf = cf_info_base; cf; cf = cf->next_file_info) {

    if (cf->file_type == FILE_TYPE_LCCTREE) 
      continue; /* lcctree files shouldn't be built here */

    if (cf->file_type == FILE_TYPE_DBMS) 
      continue; /* DBMS files shouldn't be built here */

    if (cf->filecont) {
      if (cf->filecont->dbflag) {
	/* continuation data is in a database - it will be opened when needed*/ 
	cf->filecont->file = NULL;
	cf->cont_id = 0;
	cf->current_cont = NULL;
	continue;
      }
      /* when this is a file with continuations, the main file name */
      /* should be just a directory name and not the data file      */
      if ((cf->file_ptr = fopen(cf->filecont->name, cf_open_file_flags)) 
	  == NULL) {
	if ((cf->file_ptr = fopen(cf->filecont->name, cf_create_file_flags)) 
	    == NULL) {
	  if ((cf->file_ptr = fopen(cf->filecont->name, "r")) == NULL) {
	    fprintf(LOGFILE,
		    "failed to create or open continuation file %s\n", 
		    cf->filecont->name);
	    return (-1);
	  }
	}
      } 
      cf->filecont->file = cf->file_ptr;
      cf->cont_id = 1;
      cf->current_cont = cf->filecont;
    }
    else if (cf->file_type < 99) {
      if ((cf->file_ptr = fopen(cf->file_name, cf_open_file_flags)) == NULL) {
	if ((cf->file_ptr = fopen(cf->file_name, cf_create_file_flags)) 
	    == NULL) {
	  if ((cf->file_ptr = fopen(cf->file_name, "r")) == NULL) {
	    
	    fprintf(LOGFILE,"failed to create or open existing main file %s\n", 
		    cf->file_name);
	    return (-1);
	  }
	}
      } 
    }
    else {
      /* DATASTORE File -- Should have been created by cheshire_load */
      /* do nothing */
    }
    
    if (cf->file_type < 99 
	&& (cf->assoc_ptr = fopen(cf->assoc_name, cf_open_file_flags)) == NULL) {
      if (cf->assoc_name != NULL) { /* has an assoc file */
	if ((cf->assoc_ptr = fopen(cf->assoc_name, cf_create_file_flags)) 
	    == NULL) {
	  char *fmode;
#ifdef WIN32
	  fmode = "rb";
#else 
	  fmode = "r";
#endif

	  if ((cf->assoc_ptr = fopen(cf->assoc_name, fmode)) 
	      == NULL) {
	    fprintf(LOGFILE,"failed to create %s\n", 
		    cf->assoc_name);
	    return (-1);
	  }
	}
	else { /* have to initialize the new associator file */
	    assoc_rec head_record;
	    head_record.offset = 0;
	    head_record.recsize = 0;
	    fwrite(&head_record,sizeof(assoc_rec), 1, cf->assoc_ptr);
	    fflush(cf->assoc_ptr);
	}
      }
    }
    if ((cf->history_ptr = fopen(cf->history_name, cf_open_file_flags)) 
	== NULL) {
      if (cf->history_name != NULL) { /* has an assoc file */
	if ((cf->history_ptr = fopen(cf->history_name, cf_create_file_flags))
	    == NULL) {
	  fprintf(LOGFILE,"failed to create %s\n", 
		  cf->history_name);
	  return (-1);
	}
      }
    }
    for (idx = cf->indexes; idx ; idx = idx->next_entry) {
	index_return = create_indexes(idx, &num_open, cf->indexes);
	if (index_return != 0)
	  return(index_return);
	if (cf->file_type < 99 
	    && ((idx->type & EXTERNKEY) 
		|| (idx->type & NORM_WITH_FREQ))) {
	  /* we no longer even try to open the DOCSIZES file if there */
	  /* are no external keys or collection info                  */
	  sprintf(namebuff, "%s.DOCSIZES", cf->file_name);
	  if ((cf->docsize_ptr = fopen(namebuff, cf_open_file_flags)) 
	      == NULL) {
	    if ((cf->docsize_ptr = fopen(namebuff, cf_create_file_flags)) 
		== NULL) {
	      fprintf(LOGFILE,"WARNING: failed to open %s -- may not be needed\n?", 
		      namebuff);
	      cheshire_exit(1);
	    }
	  }
	}
    }

    for (comp = cf->components; comp ; comp = comp->next_entry) {
      int dummy;

      comp->comp_db = cf_component_open(cf->file_name,comp->name);

      if (comp->comp_db == NULL) {
	fprintf(LOGFILE,"failed to open component DB file %s\n?", comp->name);        cheshire_exit(1);
      }

      for (idx = comp->indexes; idx ; idx = idx->next_entry) {
	index_return = create_indexes(idx, &num_open, comp->indexes);
	if (index_return != 0)
	  return(index_return);
      }
    }    
  }
  return(0);
}


int
create_indexes(idx_list_entry *idx, int *num_open, idx_list_entry *head) 
{
  
  char keybuffer[KEYBUFFERSIZE];
  char namebuff[500];
  int exists;

  idx->db = create_index_db(idx->name, idx->type, num_open);
  if (*num_open == 0) {
    /* this is the first opening of this index so... */
    /* get the global info from the "special record" */
    idx->GlobalData = CALLOC(dict_info,1);
    idx->GlobalData->recptr = 1;
    if (idx->type & PROXIMITY) {
      idx->prox_db = create_prox_db(idx->name, idx->type, num_open);
    }
    if (idx->type & VECTOR_TYPE) {
      idx->vector_db = create_vector_db(idx->name, idx->type, num_open);
    }    
    
  }
  else { 
    /* this index may have been opened previously for another index so */
    /* check and set the idx->GlobalData pointer to the correct struct */
    idx_list_entry *idx2;
    
    for (idx2 = head; idx2 ; idx2 = idx2->next_entry) {
      if (idx2 != idx && idx2->db == idx->db && 
	  strcmp(idx2->name, idx->name) == 0 ) 
	idx->GlobalData = idx2->GlobalData;
    }
    if (idx->GlobalData == NULL) {
      /* this is the first opening of this one for this run */
      /* get the global info from the "special record" */
      DBT keyval;
      DBT dataval;
      char srchbuffer[30];
      int returncode, num_open;
      
      idx->GlobalData = CALLOC(dict_info,1);
      
      memset(&keyval,0,sizeof(DBT));
      memset(&dataval,0,sizeof(DBT));
      
      sprintf(srchbuffer, ">>>Global Data Values<<<");
      keyval.data = (void *) srchbuffer;
      keyval.size = strlen(srchbuffer) + 1;
      
      returncode = idx->db->get(idx->db, NULL, &keyval, &dataval, 0); 
      
      if (returncode == 1) {
	/* no matching special record in the index */
	fprintf(LOGFILE, "Index not initialized correctly\n");
	return (-2);
      }
      else if (returncode == -1) {
#ifndef WIN32
	fprintf(LOGFILE, "cf_createall: db->get returns error %d.\n", 
		errno);
#else
	fprintf(LOGFILE, "cf_createall: db->get returns error.\n");
#endif
	return (-3);
      }
      else {
	memcpy((char *)idx->GlobalData, (char*)dataval.data, 
	       dataval.size);
	if (idx->GlobalData->recptr == 0) 
	  idx->GlobalData->recptr = 1;
      }
      
      if (idx->type & PROXIMITY) {
	idx->prox_db = create_prox_db(idx->name, idx->type, &num_open);
      }
      if (idx->type & VECTOR_TYPE) {
	idx->vector_db = create_vector_db(idx->name, idx->type, &num_open);
      }

      
    }
    
  }

  if (idx->stopwords_name != NULL) {
    if ((idx->stopwords_file = fopen(idx->stopwords_name, "r")) 
	== NULL) {
      if ((idx->stopwords_file = fopen(idx->stopwords_name, 
				       cf_create_file_flags)) == NULL) {
	fprintf(LOGFILE,"failed to create %s\n", 
		idx->stopwords_name);
	return(-1);
      }
    }
    Tcl_InitHashTable(&(idx->stopwords_hash),TCL_STRING_KEYS);
    while (fgets(keybuffer,KEYBUFFERSIZE,idx->stopwords_file)
	   != NULL) {
      keybuffer[strlen(keybuffer)-1] = '\0'; /* eliminate newline */
      Tcl_CreateHashEntry(&(idx->stopwords_hash),keybuffer,&exists);
    }
  }
  else /* create an empty table */
    Tcl_InitHashTable(&(idx->stopwords_hash),TCL_STRING_KEYS);
  
  if (idx->expansion_name != NULL) {
    if ((idx->expansion_file = fopen(idx->expansion_name, "r")) 
	== NULL) {
      if ((idx->expansion_file = fopen(idx->expansion_name, 
				       cf_create_file_flags)) == NULL) {
	fprintf(LOGFILE,"failed to create %s\n", 
		idx->expansion_name);
	return(-1);
      }
    }
    Tcl_InitHashTable(&(idx->expansion_hash),TCL_STRING_KEYS);
    while (fgets(keybuffer,KEYBUFFERSIZE,idx->expansion_file)
	   != NULL) {
      keybuffer[strlen(keybuffer)-1] = '\0'; /* eliminate newline */
      Tcl_CreateHashEntry(&(idx->expansion_hash),keybuffer,&exists);
    }
  }
  else /* create an empty table */
    Tcl_InitHashTable(&(idx->expansion_hash),TCL_STRING_KEYS);
  

  /* check if this is a paged index and set flag if so */
  if (cf_pagedindex(idx)) {
    sprintf(namebuff,"%s.PAGEDMAP", idx->name);
    if ((idx->page_rec_assoc_file = 
	 fopen(namebuff, cf_open_file_flags)) == NULL) {
      if ((idx->page_rec_assoc_file = 
	   fopen(namebuff, cf_create_file_flags)) == NULL) {
	fprintf(LOGFILE,"failed to create %s\n", 
		namebuff);
	cheshire_exit(1);
      } else { /* create the highest number info record */
	page_assoc_rec page_data;
	page_data.parentid = 0;
	page_data.pagenum = 0;
	page_data.recsize = 0;
	page_data.nameoffset = 0;
	fwrite(&page_data,sizeof(page_assoc_rec),1, 
	       idx->page_rec_assoc_file);		
      }
      
    } else { /* get the highest page number */
      fread(&(idx->highest_page_number),sizeof(long),1, 
	    idx->page_rec_assoc_file);		
    }
    
    sprintf(namebuff,"%s.PAGEDFILES", idx->name);
    if ((idx->page_file_names = 
	 fopen(namebuff, cf_open_file_flags)) == NULL) {
      if ((idx->page_file_names = 
	   fopen(namebuff, cf_create_file_flags)) == NULL) {
	fprintf(LOGFILE,"failed to create %s\n", 
		namebuff);
	cheshire_exit(1);
      }
    }
  }


  if (idx->type & GEOTEXT && idx->gaz_config_file == NULL) {
    if (idx->extern_app_name == NULL) {
      fprintf(LOGFILE, "No gazetteer information in <extern_app> for index %s\n", idx->tag);
      return (1);
    }
    if (cf_init_gaz_configfile(idx) == -1)
      return(1);
  }
  

  return (0);
}



/* open an opendb index (btree only for now) */
DB *create_index_db (char *idxname, int idxtype, int *opened)
{
  dict_info newinfo ;
  DBT keyval;
  DBT dataval;
  int returncode;
  char termbuffer[30] ;
  DB *db_handle;
  int accesstype, num_open;
  int db_errno;

  accesstype = idxtype & DB_MASK;

  num_open = -1;

  db_handle = init_index_db(idxname,idxtype, &num_open);
  *opened = num_open;

  if (db_handle == NULL) {
    
    /* Create the database. */
    if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
      gb_dbenv->err(gb_dbenv, db_errno, "db_create failed in cf_createall.c:create_index_db");
      cheshire_exit(1);
    }
    /* Initialize the database. */
    db_handle->set_pagesize(db_handle, 8 * 1024);
    if ((db_errno = db_handle->open(db_handle, idxname, NULL, 
				    DB_BTREE, DB_CREATE, 0664)) != 0) {
      db_handle->err(db_handle, db_errno, "db_handle->open failed to create %s in cf_createall.c:create_index_db", idxname);
      cheshire_exit (1);
    }


    /* this is a new database file, so we install a standard item */
    /* for tracking some index-wide frequency information         */
    memset(&keyval,0,sizeof(DBT));
    memset(&dataval,0,sizeof(DBT));

    sprintf(termbuffer, ">>>Global Data Values<<<");
    keyval.data = (void *) termbuffer;
    keyval.size = strlen(termbuffer) + 1;

    newinfo.recptr = 0L; /*i.e. the last termid */
    newinfo.tot_numrecs = 0L;
    newinfo.tot_occur = 0L;

    dataval.data = (void *) &newinfo;
    dataval.size = sizeof(newinfo);

    /* write this data, close to flush it, then reopen via init_index_db */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 
				DB_NOOVERWRITE); 
    
    db_handle->close(db_handle,0);
    db_handle = init_index_db(idxname,idxtype,&num_open);

    *opened = 0;
  }
    
  return (db_handle);
}

/* open an opendb index (btree only for now) */
DB *create_prox_db  (char *idxname, int idxtype, int *opened)
{
  int returncode;
  DB *db_handle;
  int accesstype, num_open;
  int db_errno;
  int namelen;
  char *proxname;


  namelen = strlen(idxname);

  proxname = CALLOC(char, namelen+6);

  sprintf(proxname,"%s.PROX", idxname);

  db_handle = init_index_db(proxname,idxtype, &num_open);
  *opened = num_open;

  if (db_handle == NULL) {
    
    /* Create the database. */

    if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
      gb_dbenv->err(gb_dbenv, db_errno, "db_create failed in cf_createall.c:create_prox_db");
      cheshire_exit(1);
    }
    /* Initialize the database. */
    db_handle->set_pagesize(db_handle, 8 * 1024);

    if ((db_errno = db_handle->open(db_handle, proxname, NULL, 
				    DB_BTREE, DB_CREATE, 0664)) != 0) {
      db_handle->err(db_handle, db_errno, "db_handle->open failed to create %s in cf_createall.c:create_prox_db", proxname);
      cheshire_exit (1);
    }
  }
    
  return (db_handle);
}


/* open an opendb index (btree only) */
DB *create_vector_db  (char *idxname, int idxtype, int *opened)
{
  int returncode;
  DB *db_handle;
  int accesstype, num_open;
  int db_errno;
  int namelen;
  char *vectorname;


  namelen = strlen(idxname);

  vectorname = CALLOC(char, namelen+10);

  sprintf(vectorname,"%s.VECTOR", idxname);

  db_handle = init_index_db(vectorname, idxtype, &num_open);
  *opened = num_open;

  if (db_handle == NULL) {
    
    /* Create the database. */

    if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
      gb_dbenv->err(gb_dbenv, db_errno, "db_create failed in cf_createall.c:create_vector_db");
      cheshire_exit(1);
    }
    /* Initialize the database. */
    db_handle->set_pagesize(db_handle, 8 * 1024);

    if ((db_errno = db_handle->open(db_handle, vectorname, NULL, 
				    DB_BTREE, DB_CREATE, 0664)) != 0) {
      db_handle->err(db_handle, db_errno, "db_handle->open failed to create %s in cf_createall.c:create_vector_db", vectorname);
      cheshire_exit (1);
    }
  }
    
  return (db_handle);
}


/*
 * db_init --
 *      Initialize the environment.
 */
int
db_init(char *home, DB_ENV **dbenvp)
{
  int ret;
  DB_ENV *dbenv;

  if ((ret = db_env_create(&dbenv, 0)) != 0) 
    return (ret);
  
  /* dbenv->set_lk_max(dbenv, 10000); */
  dbenv->set_cachesize(dbenv, 0, 16 * 1024 * 1024, 0);
  /* new version only ... dbenv->set_flags(dbenv, DB_CDB_ALLDB, 1); */
  
  if (home == NULL) {
    home = getenv("CHESHIRE_DB_HOME");
    if (home == NULL) {
      fprintf(stderr, "CHESHIRE_DB_HOME must be set OR config file <DBENV> set\n");
      fprintf(LOGFILE, "CHESHIRE_DB_HOME must be set OR config file <DBENV> set\n");
      cheshire_exit(1);
    }
  }
  if ((ret = dbenv->open(dbenv, home,
			 DB_INIT_MPOOL | DB_INIT_CDB
			 | DB_CREATE | DB_USE_ENVIRON, 
			 0)) == 0) {
    *dbenvp = dbenv;
    dbenv->set_errfile(dbenv, LOGFILE);
    dbenv->set_errpfx(dbenv, "BerkeleyDB");
    return (0);
  }
  /* this goes to stdout and screws up z39.50 connections -- hangs them */
  /* dbenv->err(dbenv, ret, "Could not open DB environment: %s", home); */
  fprintf(LOGFILE,"Could not open DB environment: %s\n",home);
  (void)dbenv->close(dbenv, 0);
  return (ret);
}
