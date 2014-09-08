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
*	Header Name:	cf_getfilename.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	get the full path name for a file from the 
*                       the configfile structure and return a pointer to it.
*                       
*	Usage:		idx_list_entry *cf_getidx(filename,which)
*                       filename can be the full pathname or short "nickname"
*                       "which" should be the type of file name wanted,
*                       MAINFILE, ASSOCFILE, HISTORYFILE, etc.
*                        
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file name (pointer) or NULL on error;
*
*	Date:		11/12/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"

/* global DB_ENV variable */
extern DB_ENV *gb_dbenv;

extern config_file_info *cf_info_base;
config_file_info *find_file_config(char *filename);
idx_list_entry *find_index_name(config_file_info *cf, char *indexname);

char *cf_getfilename(char *filename, int which)
{
  
  config_file_info *cf;

  if (cf = find_file_config(filename)) {
      
    switch (which) {
    case MAINFILE:
      return(cf->file_name);
      break;
      
    case ASSOCFILE:
      return(cf->assoc_name);
      break;
      
    case HISTORYFILE:
      return(cf->history_name);
      break;
      
    case DTDFILE:
      return(cf->DTD_name);
      break;
      

    default:
      fprintf(LOGFILE,"invalid file type in cf_getfilename\n");
      exit (1);
    }
  }
}



char *
cf_getDBfilename(config_file_info *cf, continuation *cont, int recordid)
{
  int exists;
  DB *db_handle;
  int db_errno;
  DBT keyval;
  DBT dataval;
  char *filename;
  int returncode;
  int filenamesize;
  int filenamelen;
  
  /* This will just fetch the name, not open the file */
  
  if (cont->db == NULL) {
    
    /* Initialize the database environment. */
    
    if (gb_dbenv == NULL) {
      if (db_errno = db_init(NULL, &gb_dbenv) != 0) {
	fprintf(LOGFILE, "BerkeleyDB:db_init failed in cf_getfname:cf_getDBfilename: %d %s\n",
		db_errno, db_strerror(db_errno));
	exit(1);
      }
    }
    
    if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
      gb_dbenv->err(gb_dbenv, db_errno,
		    "db_create failed in cf_getfname:cf_getDBfilename");
      return(NULL);
    }
    
    db_handle->set_pagesize(db_handle, 8 * 1024);
    
    if (db_errno = db_handle->open(db_handle,
				   cont->name, NULL, DB_BTREE, 
				   DB_RDONLY, 0)) {
      
      db_handle->err(db_handle, db_errno, 
		     "db_handle->open failed to create %s in cf_getfname:cf_getDBfilename", cont->name);
      return(NULL);
    }
    
    cont->db = db_handle;
  }
  
  db_handle = cont->db;
  
  /* search for the recordid */
  memset(&keyval,0,sizeof(DBT));
  memset(&dataval,0,sizeof(DBT));
  
  keyval.data = (void *)&recordid;
  keyval.size = sizeof(int);
  
  returncode = db_handle->get(db_handle,NULL, &keyval, &dataval, 0); 
  
  if (returncode == 1) {
    /* no matching special record in the index */
    fprintf(LOGFILE, "Record id %d not found in CONT_DB file\n");
    returncode = db_handle->close(db_handle, 0); 
    return (NULL);
  }
  else if (returncode <= -1) {
#ifndef WIN32
    fprintf(LOGFILE, "cf_getfname:cf_getDBfilename: db->get returns error %d.\n", 
	    returncode);
#else
    fprintf(LOGFILE, "cf_getfname:cf_getDBfilename db->get returns error.\n");
#endif	    
    returncode = db_handle->close(db_handle, 0); 
    return (NULL);
  }
  else {
    if (*((char *)dataval.data) == '/') {
      filenamesize = dataval.size +1;
      filename = CALLOC(char, filenamesize);
      strcpy(filename, (char *)dataval.data);
    } 
    else {
      filenamesize = dataval.size + strlen(cf->defaultpath) + 2;
      filename = CALLOC(char, filenamesize);
      strcpy(filename, cf->defaultpath); 
      strcat(filename, "/");
      strcat(filename, (char *)dataval.data);
    }

    filenamelen = strlen(filename);

    if (filename[filenamelen-1] == ' ') {
      filename[filenamelen-1] = '\0';
    }

    return(filename); 
  }
}



char *
cf_getdatafilename(char *filename, int recordid)
{

  continuation *cont;
  config_file_info *cf;
  FILE *temp;
  
  cf = find_file_config(filename);
  
  if (cf != NULL) {
    
    if (cf->file_type == FILE_TYPE_VIRTUALDB) {
      return (filename); 
    }
    
    if (cf->file_ptr == NULL) { /* i.e.: first time files have opened */
      temp = cf_open(filename, MAINFILE);
    }
    if (cf->filecont != NULL) {
      if (cf->current_cont != NULL 
	  && (recordid >= cf->current_cont->docid_min 
	      && recordid <= cf->current_cont->docid_max)) {
	/* already have the correct file open */
	return (cf->current_cont->name);
      }
      else {
	/* find the continuation file matching the recordid */
	for (cont = cf->filecont; cont != NULL; cont = cont->next_cont) {

	  if (cont->dbflag) {
	    return (cf_getDBfilename(cf,cont,recordid));
	  }

	  if (recordid >= cont->docid_min && recordid <= cont->docid_max) {
	    /* this is the correct continuation file */
	    return(cont->name);
	  }
	}
      }  
      if (cont == NULL) /* didn't find a cont file with recordid in range */
	return (NULL);
      
    }
    /* no continuation files for this file, return the main cf name */
    return(cf->file_name); /* this is a single file */    
  }    
  return (NULL); /* no cf entry for this file */
}





