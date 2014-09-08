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

#include "cheshire.h"
#include "db.h"
#include "configfiles.h"

extern config_file_info *cf_info_base;

DB *create_index_db (char *idxname, int idxtype, int *opened);

#define KEYBUFFERSIZE 200


cf_createall()
{
  
  config_file_info *cf;
  idx_list_entry *idx;

  char keybuffer[KEYBUFFERSIZE];
  char namebuff[500];
  int exists, num_open;

  for (cf = cf_info_base; cf; cf = cf->next_file_info) {

    if (cf->file_type == FILE_TYPE_LCCTREE) 
      continue; /* lcctree files shouldn't be built here */

    if (cf->filecont) {
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
    else {
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
    if ((cf->assoc_ptr = fopen(cf->assoc_name, cf_open_file_flags)) == NULL) {
      if (cf->assoc_name != NULL) { /* has an assoc file */
	if ((cf->assoc_ptr = fopen(cf->assoc_name, cf_create_file_flags)) 
	    == NULL) {
	  if ((cf->assoc_ptr = fopen(cf->assoc_name, "r")) 
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

    sprintf(namebuff, "%s.DOCSIZES", cf->file_name);
    if ((cf->docsize_ptr = fopen(namebuff, cf_open_file_flags)) 
	== NULL) {
      if ((cf->docsize_ptr = fopen(namebuff, cf_create_file_flags)) 
	  == NULL) {
	fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		namebuff);
	exit(1);
      }
    }
    
    for (idx = cf->indexes; idx ; idx = idx->next_entry) {
      
      idx->db = create_index_db(idx->name, idx->type, &num_open);
      if (num_open == 0) {
	/* this is the first opening of this index so... */
	/* get the global info from the "special record" */
	idx->GlobalData = CALLOC(dict_info,1);
	idx->GlobalData->recptr = 1;
      }
      else { 
	/* this index may have been opened previously for another index so */
	/* check and set the idx->GlobalData pointer to the correct struct */
	idx_list_entry *idx2;
	      
	for (idx2 = cf->indexes; idx2 ; idx2 = idx2->next_entry) {
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

	  sprintf(srchbuffer, ">>>Global Data Values<<<");
	  keyval.data = (void *) srchbuffer;
	  keyval.size = strlen(srchbuffer) + 1;
	      
	  returncode = idx->db->get(idx->db, &keyval, &dataval, 0); 
    
	  if (returncode == 1) {
	    /* no matching special record in the index */
	    fprintf(LOGFILE, "Index not initialized correctly\n");
	    return (-2);
	  }
	  else if (returncode == -1) {
	    fprintf(LOGFILE, "cf_createall: db->get returns error %d.\n", 
		    errno);
	    return (-3);
	  }
	  else {
	    memcpy((char *)idx->GlobalData, (char*)dataval.data, 
		   dataval.size);
	    if (idx->GlobalData->recptr == 0) 
	      idx->GlobalData->recptr = 1;
	  }
	}
      }
      
      if ((idx->assoc_file = fopen(idx->assoc_name, cf_open_file_flags)) 
	  == NULL) {
	if ((idx->assoc_file = fopen(idx->assoc_name, cf_create_file_flags)) 
	    == NULL) {
	  fprintf(LOGFILE,"failed to create %s\n", 
		  idx->assoc_name);
	  return (-1);
	}
      }
      if ((idx->postings_file = fopen(idx->postings_name, cf_open_file_flags))
	  == NULL) {
	if ((idx->postings_file = fopen(idx->postings_name, 
					cf_create_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to create %s\n", 
		  idx->postings_name);
	  return(-1);
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

      /* check if this is a paged index and set flag if so */
      if (cf_pagedindex(idx)) {
	sprintf(namebuff,"%s.PAGEDMAP", idx->name);
	if ((idx->page_rec_assoc_file = 
	     fopen(namebuff, cf_open_file_flags)) == NULL) {
	  if ((idx->page_rec_assoc_file = 
	       fopen(namebuff, cf_create_file_flags)) == NULL) {
	    fprintf(LOGFILE,"failed to create %s\n", 
		    namebuff);
	    exit(1);
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
	    exit(1);
	  }
	}
      }
    }
  }
  return(0);
}



/* open an opendb index (btree only for now) */
DB *create_index_db (char *idxname, int idxtype, int *opened)
{
  extern DB *dbopen();
  DB *db_handle;
  dict_info newinfo ;
  DBT keyval;
  DBT dataval;
  int returncode;
  char termbuffer[30] ;
  static BTREEINFO open_info;
  void *open_info_ptr;
  int accesstype, num_open;

  accesstype = idxtype & DB_MASK;

  num_open = -1;

  db_handle = init_index_db(idxname,idxtype, &num_open);
  *opened = num_open;

  if (db_handle == NULL) {

    db_handle = dbopen( idxname, O_CREAT | O_RDWR, 
		       0666 , accesstype, NULL /*defaults*/);

    if (db_handle == NULL) {
      fprintf(LOGFILE, "db_open failed on create in cf_createall\n");
      exit(1);
    }
    /* this is a new database file, so we install a standard item */
    /* for tracking some index-wide frequency information         */
    sprintf(termbuffer, ">>>Global Data Values<<<");
    keyval.data = (void *) termbuffer;
    keyval.size = strlen(termbuffer) + 1;

    newinfo.recptr = 0L; /*i.e. the last termid */
    newinfo.tot_numrecs = 0L;
    newinfo.tot_occur = 0L;

    dataval.data = (void *) &newinfo;
    dataval.size = sizeof(newinfo);

    /* write this data, close to flush it, then reopen via init_index_db */
    returncode = db_handle->put(db_handle, &keyval, &dataval, R_NOOVERWRITE); 
    db_handle->close(db_handle);
    db_handle = init_index_db(idxname,idxtype,&num_open);
    *opened = 0;
  }
    
  return (db_handle);
}









