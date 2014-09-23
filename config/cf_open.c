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
*	Header Name:	cf_open.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to open files under the control of
*                       the configfile structure and return file 
*                       pointers
*	Usage:		FILE *cf_open(filename, which)
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
*	Revised:	11/5/93
*	Version:	1.1 -- hash table for btrees added - RRL
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#endif
#include "cheshire.h"
#include "configfiles.h"
#include <string.h>

/* global DB_ENV variable */
DB_ENV *gb_dbenv=NULL;

extern config_file_info *cf_info_base;
int db_init(char *home, DB_ENV **dbenvp);

extern SGML_DTD *sgml_parse_dtd(char *filename, char *sgml_catalog_name, 
				char *schema_file_name, filelist *fl, int dtd_type);

extern void *cf_open_DBMS(config_file_info *cf, char *login, char *pwd);

/* forward decl */
SGML_DTD *cf_getDTD(char *filename);

/* build a hash table of index names for a particular file */
void
build_index_hash(config_file_info *cf) 
{
  idx_list_entry *idx;
  component_list_entry *comp;
  char *tmp, *tmp2;
  int exists, i;

  if (cf == NULL) {
    fprintf(LOGFILE, 
	    "build_index_hash: Null config file pointer -- no hash built\n");
    return ;
  }
  cf->IndexNamesHash = CALLOC(Tcl_HashTable,1);
  Tcl_InitHashTable(cf->IndexNamesHash,TCL_STRING_KEYS);

  /* build hash entries for BOTH name and tag */	  
  for (idx = cf->indexes; idx ; idx = idx->next_entry) {
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(
					cf->IndexNamesHash,
					idx->tag,
					&exists),
		     (ClientData)idx);
       
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(
					cf->IndexNamesHash,
					idx->name,
					&exists),
		     (ClientData)idx);


    tmp = strdup(idx->tag);
    tmp2 = strdup(idx->name);
    for (i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }
    for (i = 0; i < strlen(tmp2); i++) {
      tmp2[i] = toupper(tmp2[i]);
    }

    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(
					 cf->IndexNamesHash,
					 tmp,
					 &exists),
		     (ClientData)idx);
       
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(
					cf->IndexNamesHash,
					tmp2,
					&exists),
		     (ClientData)idx);

    FREE(tmp);
    FREE(tmp2);

  }

  for (comp = cf->components; comp; comp = comp->next_entry) {
    /* add the component name to the hash table... */
    /* NOTE that this is storing a component pointer */
    char *tempstr;
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(
					 cf->IndexNamesHash,
					 comp->name,
					 &exists),
		     (ClientData)comp);
    /* include the final (short) name for the path if there is one */
#ifdef WIN32

    if ((tempstr = strrchr(comp->name, '\\')) != NULL) {
      tempstr++;
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   tempstr,
					   &exists),
		       (ClientData)comp);
    }
    
#else

    if ((tempstr = strrchr(comp->name, '/')) != NULL) {
      tempstr++;

      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   tempstr,
					   &exists),
		       (ClientData)comp);
    }
    
#endif

    /* and add all of the component indexes -- just like regular indexes */
    for (idx = comp->indexes; idx ; idx = idx->next_entry) {
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   idx->tag,
					   &exists),
		       (ClientData)idx);
      
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   idx->name,
					   &exists),
		       (ClientData)idx);



      tmp = strdup(idx->tag);
      tmp2 = strdup(idx->name);
      for (i = 0; i < strlen(tmp); i++) {
	tmp[i] = toupper(tmp[i]);
      }
      for (i = 0; i < strlen(tmp2); i++) {
	tmp2[i] = toupper(tmp2[i]);
      }
      
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   tmp,
					   &exists),
		       (ClientData)idx);
      
      Tcl_SetHashValue(
		       Tcl_CreateHashEntry(
					   cf->IndexNamesHash,
					   tmp2,
					   &exists),
		       (ClientData)idx);
      
      FREE(tmp);
      FREE(tmp2);
      
    }
  }
}


/* search the global hash table of file names for the configfile info */
config_file_info *
find_file_config(char *filename)
{
  Tcl_HashEntry *entry;
  char *tmp;
  int i, templen;


  if (filename == NULL 
      || (*filename == '*' && strcmp(filename, "***dummy***") == 0))
    return (NULL);
  
  if (cf_file_names == NULL && LOGFILE != NULL) {
    fprintf(LOGFILE,
	    "cf_file_names hash table not initialized in find_file_config\n");
    return (NULL);
  }
  else if (cf_file_names == NULL) {
    return (NULL);
  }
  
  tmp = strdup(filename);
  templen = strlen(tmp);

  for (i = 0; i < templen; i++) {
    tmp[i] = toupper(tmp[i]);
  }

  entry = Tcl_FindHashEntry(cf_file_names,tmp);

  FREE(tmp);

  if (entry == NULL) {
    fprintf(LOGFILE, 
	    "find_file_config: Couldn't locate file name in hash table\n");
    return (NULL);
  }
  else 
    return ((config_file_info *)Tcl_GetHashValue(entry));
}


int 
cf_clear_tables(config_file_info *cf) 
{
  Tcl_HashEntry *entry;
  char *tmp;
  filelist *others;
  int i;

  if (cf == NULL)
    return (0);

  if (cf_file_names == NULL)
    return (0);

  if (cf->file_name) {
    tmp = strdup(cf->file_name);
    for (i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }
  
    entry = Tcl_FindHashEntry(cf_file_names,tmp);
    if (entry)
      Tcl_DeleteHashEntry(entry);
    FREE(tmp);
  }
  
  if (cf->nickname) {
    tmp = strdup(cf->nickname);
    for (i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }
    
    entry = Tcl_FindHashEntry(cf_file_names,tmp);
    if (entry)
      Tcl_DeleteHashEntry(entry);
    FREE(tmp);
  }  

  for (others = cf->othernames; others != NULL; 
       others = others->next_filename) {
    if (others->filename) {
      tmp = strdup(others->filename);
      for (i = 0; i < strlen(tmp); i++) {
	tmp[i] = toupper(tmp[i]);
      }
  
      entry = Tcl_FindHashEntry(cf_file_names,tmp);
      if (entry)
	Tcl_DeleteHashEntry(entry);
      FREE(tmp);
    }  
  }
  return(0);
}

/* search the global hash table of file names for the configfile info but
   doesn't put out any messages if the file is not found */
config_file_info *
find_file_config_quiet(char *filename)
{
  Tcl_HashEntry *entry;
  char *tmp;
  int i, templen;


  if (filename == NULL)
    return (NULL);

  if (cf_file_names == NULL) {
    return (NULL);
  }
  
  tmp = strdup(filename);
  templen = strlen(tmp);

  for (i = 0; i < templen; i++) {
    tmp[i] = toupper(tmp[i]);
  }

  entry = Tcl_FindHashEntry(cf_file_names,tmp);

  FREE(tmp);

  if (entry == NULL) {
    return (NULL);
  }
  else 
    return ((config_file_info *)Tcl_GetHashValue(entry));
}


idx_list_entry *
find_index_name(config_file_info *cf, char *indexname)
{
  Tcl_HashEntry *entry;

  if (indexname == NULL)
    return(NULL);

  if (cf == NULL) {
    fprintf(LOGFILE, "cf is NULL in find_index_name");
    return (NULL);
  }

  if (cf->IndexNamesHash == NULL) {
    build_index_hash(cf);
  }

  entry = Tcl_FindHashEntry(cf->IndexNamesHash, indexname);
  if (entry == NULL) {
    /* try the uppercase version of the name */
    char *tmp;
    int i;

    tmp = strdup(indexname);
    for (i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }

    entry = Tcl_FindHashEntry(cf->IndexNamesHash, tmp);
    FREE(tmp);

    if (entry == NULL) {
      fprintf(LOGFILE, 
	      "find_index_name: Couldn't locate index name in hash table\n");
      return (NULL);
    }
  }
  return ((idx_list_entry *)Tcl_GetHashValue(entry));
    
}

idx_list_entry *
find_index_name_nowarn(config_file_info *cf, char *indexname)
{
  Tcl_HashEntry *entry;

  if (indexname == NULL)
    return(NULL);

  if (cf == NULL) {
    return (NULL);
  }

  if (cf->IndexNamesHash == NULL) {
    build_index_hash(cf);
  }

  entry = Tcl_FindHashEntry(cf->IndexNamesHash, indexname);
  if (entry == NULL) {
    /* try the uppercase version of the name */
    char *tmp;
    int i;

    tmp = strdup(indexname);
    for (i = 0; i < strlen(tmp); i++) {
      tmp[i] = toupper(tmp[i]);
    }

    entry = Tcl_FindHashEntry(cf->IndexNamesHash, tmp);
    FREE(tmp);

    if (entry == NULL) {
      return (NULL);
    }
  }
  return ((idx_list_entry *)Tcl_GetHashValue(entry));
    
}


component_list_entry *
find_component_name(config_file_info *cf, char *compname)
{
  Tcl_HashEntry *entry;

  if (compname == NULL)
    return(NULL);

  if (cf == NULL) {
    fprintf(LOGFILE, "cf is NULL in find_component_name");
    return (NULL);
  }
  if (cf->IndexNamesHash == NULL) {
    build_index_hash(cf);
  }
  entry = Tcl_FindHashEntry(cf->IndexNamesHash, compname);
  if (entry == NULL) {
    fprintf(LOGFILE, 
	    "find_index_name: Couldn't locate component name in hash table\n");
    return (NULL);
  }
  else 
    return ((component_list_entry *)Tcl_GetHashValue(entry));

}


FILE *
cf_open(char *filename, int which)
{
  

  config_file_info *cf;


  cf = find_file_config(filename);


  if (cf != NULL) {
    if (cf->file_type == FILE_TYPE_DBMS) {
      /* cf_open should NOT be used anymore for external DBMS files */
      return ((FILE *)cf_open_DBMS(cf, NULL, NULL));
    }

    if (which == ASSOCFILE && cf->assoc_ptr != NULL 
	&& cf->filecont != NULL && cf->filecont->dbflag != NULL) {
      /* There was a problem with opening new copies of the assoc */
      /* whenever trying to get a file length when using a cont db */
      return(cf->assoc_ptr);

    }

    if (cf->file_ptr == NULL) { /* i.e.: first time files have opened */

      if (cf->filecont) {
	/* Have some kind of cont file */
	if (cf->filecont->dbflag) {
	  /* open later as needed */
	  cf->filecont->file = NULL;
	  cf->cont_id = 0;
	  cf->current_cont = NULL;
	}
      	else {
	  if ((cf->file_ptr = fopen(cf->filecont->name, cf_open_file_flags))
	      == NULL) {
	    fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		    cf->filecont->name);
	    return(NULL);
	  }
	  cf->filecont->file = cf->file_ptr;
	  cf->cont_id = 1;
	  cf->current_cont = cf->filecont;
	}
      }
      else {
	if ((cf->file_ptr = fopen(cf->file_name, cf_open_file_flags))
	    == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  cf->file_name);
	  return (NULL);
	}
      }
      if ((cf->DTD_parsed = cf_getDTD(cf->file_name)) == NULL) {
	if (cf->file_type != FILE_TYPE_VIRTUALDB) {
	  fprintf(LOGFILE, "failed to get DTD in cf_open\n");
	  return (NULL);
	} 
      }
      if (cf->file_type < 99 && cf->file_type != FILE_TYPE_VIRTUALDB) {
	if ((cf->assoc_ptr = fopen(cf->assoc_name, cf_open_file_flags))
	    == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  cf->assoc_name);
	  return (NULL);
	}
      }
      if (cf->history_name != NULL && cf->history_ptr == NULL 
	  && (cf->history_ptr = fopen(cf->history_name, cf_open_file_flags)) 
	  == NULL) {
	fprintf(LOGFILE,"failed to open history file -- file missing in config?\n");
	/* don't die for this one -- exit(1); */
      }
      /* also build the index name hash table for the file... */
      /* build_index_hash(cf); */
    }
    switch (which) {
    case MAINFILE:
      return(cf->file_ptr);
      break;
      
    case ASSOCFILE:
      return(cf->assoc_ptr);
      break;
      
    case HISTORYFILE:
      return(cf->history_ptr);
      break;
      
      
    case DOCSIZEFILE:
      if (cf->docsize_ptr == NULL) {
	char namebuff[500];
	sprintf(namebuff, "%s.DOCSIZES", cf->file_name);
	if ((cf->docsize_ptr = fopen(namebuff, cf_open_file_flags)) 
	    == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  namebuff);
	  return(NULL);
	}
      }
      return(cf->docsize_ptr);
      break;

    default:
      fprintf(LOGFILE,"invalid file type in cf_open\n");
      return(NULL);
    }
  }
  
  fprintf(LOGFILE,"file '%s' not found cf_open. Error in Config file?\n",
	  filename);
  return(NULL);
}

int
cf_init_gaz_configfile(idx_list_entry *idx) 
{
  char *filepath;
  char *filename;
  char *index_name; 
  char *data_tag;
  char *temp = NULL;
  int returncode = 0;


  temp = strdup(idx->extern_app_name);

  if ((filepath = strchr(temp,':'))
      && (filename = strchr(filepath+1,':'))
      && (index_name = strchr(filename+1,':'))
      && (data_tag = strchr(index_name+1,':'))) {
    filepath++;
    *filename = '\0';
    filename++;
    *index_name = '\0';
    index_name++;
    *data_tag = '\0';
    data_tag++;


    idx->gaz_config_file = find_file_config(filename);

    if (idx->gaz_config_file == NULL) {
      cf_initialize(filepath,NULL,NULL); /* open read-only*/
      idx->gaz_config_file = find_file_config(filename);
    }

    idx->gaz_config_file->file_type = EXTERNAL_LOOKUP_FILE;

    idx->gaz_idx = find_index_name(idx->gaz_config_file, index_name);

   

    if (idx->gaz_idx == NULL || idx->gaz_idx->db == NULL) {
      if (cf_index_open(filename, index_name, INDEXFL) == NULL) {
	fprintf(LOGFILE,"Gazetteer index %s (file %s) in %s could not be opened\n", 
		index_name, filename, filepath);
	returncode = -1;
      }
    }
    if (idx->gaz_data_tag == NULL) 
      idx->gaz_data_tag = strdup(data_tag);
  } 
  else {
    fprintf(LOGFILE,"Gazetteer info string in index %s is incorrect: %s\n", 
	    idx->tag, idx->gaz_config_file);
    returncode = -1;
  }
  
  if (temp)
    FREE (temp);
  return(returncode);
}


#define KEYBUFFERSIZE 200


/* open an opendb index (btree only for now) */
DB *init_index_db (char *idxname, int idxtype, int *number_open, config_file_info *cf)
{
  Tcl_HashTable *hash_tab;
  Tcl_HashEntry *entry;
  int exists;
  DB *db_handle;
  DBTYPE dbtype;
  int accesstype;
  int db_errno;

  accesstype = idxtype & DB_MASK;

  switch (accesstype) {
  case 0: dbtype = DB_BTREE;
    break;
  case 1: dbtype = DB_HASH;
    break;
  default:
    dbtype = DB_BTREE;
  }

  *number_open = 0;
  
  if (cf->IndexNamesDBHash == NULL) {
    /* initialize the global hash table */
    cf->IndexNamesDBHash = CALLOC(Tcl_HashTable,1);
    Tcl_InitHashTable(cf->IndexNamesDBHash,TCL_STRING_KEYS);
  }

  hash_tab = cf->IndexNamesDBHash;

  /* check the hash table of names */
  entry = Tcl_FindHashEntry(hash_tab,idxname);

  /* Initialize the database environment. */
  if (gb_dbenv == NULL) {
    if (db_errno = db_init(NULL, &gb_dbenv) != 0) {
      fprintf(LOGFILE, "BerkeleyDB:db_init failed in cf_open.c:init_index_db: %d %s\n",
	      db_errno, db_strerror(db_errno));
      exit(1);
    }
  }
 
  if (entry == NULL){

    if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
      if (*number_open != -1) /* set during create, so a null result OK */
	gb_dbenv->err(gb_dbenv, db_errno,
		      "db_create failed in cf_open.c:init_index_db");
      return(NULL);
    }
    
    db_handle->set_pagesize(db_handle, 8 * 1024);
    
    if (db_errno = db_handle->open(db_handle,
				   idxname, NULL, dbtype, 
				   ( cf_open_file_flags[0] == 'r' 
				     && cf_create_file_flags[0] == 'r'
				     && cf_open_file_flags[1] == '\0' 
				     && cf_create_file_flags[1] == '\0'
				     ? DB_RDONLY : dbtype), 
				   0664) != 0) {
      if (*number_open != -1) /* set during create, so a null result OK */
	db_handle->err(db_handle, db_errno, 
		       "db_handle->open failed to create %s in cf_open.c:init_index_db", idxname);
      return(NULL);
    }
    
    Tcl_SetHashValue(Tcl_CreateHashEntry(hash_tab,idxname,&exists),
		     (ClientData)db_handle);
    *number_open = 1;
  }
  else {
    db_handle = (DB *) Tcl_GetHashValue(entry);
    *number_open = 2;
  }

  return (db_handle);

}



void *
cf_index_open(char *filename, char *indexname, int which) 
{
  
  config_file_info *cf;
  idx_list_entry *idx, *idx2;
  char keybuffer[KEYBUFFERSIZE];
  int exists;
  
  cf = find_file_config(filename);
  
  if (cf) {

    idx = find_index_name(cf, indexname);
    if (idx) {
      /* if it isn't a special dbms index... */
      if (cf->file_type == FILE_TYPE_DBMS
	  && ((idx->type & (DB_DBMS_TYPE | DB_MASK)) 
	      != (DB_DBMS_TYPE | DB_MASK))) {

	return (cf->file_ptr); /* this is really a handle for the DBMS */
      }
      /* otherwise it's a regular index or a special DBMS index */
      if (idx->db == NULL) {
	DBT keyval;
	DBT dataval;
	int db_errno;  
	char srchbuffer[30];
	int returncode, num_open;
	
	memset(&keyval,0,sizeof(DBT));
	memset(&dataval,0,sizeof(DBT));

	/* Initialize the database environment. */
	if (gb_dbenv == NULL && cf->dbenv_path != NULL) {
	  if (db_errno = db_init(cf->dbenv_path, &gb_dbenv) != 0) {
	    fprintf(LOGFILE, "BerkeleyDB:db_init failed in cf_open.c:init_index_db: %d %s\n",
		    db_errno, db_strerror(db_errno));
	    exit(1);
	  }
	}

 	
	idx->db = init_index_db(idx->name, idx->type, &num_open, cf);
	
	if (idx->db == NULL) {
	  /* no index file? must not have run indexing on the DB */
	  fprintf(LOGFILE, "Index %s not initialized correctly\n",
		  idx->name);
	  return (NULL);
	}
	
	
	if (num_open == 1) {
	  /* this is the first opening of this index so... */
	  /* get the global info from the "special record" */
	  idx->GlobalData = CALLOC(dict_info,1);
	  
	  sprintf(srchbuffer, ">>>Global Data Values<<<");
	  keyval.data = (void *) srchbuffer;
	  keyval.size = strlen(srchbuffer) + 1;
	  
	  returncode = idx->db->get(idx->db,NULL, &keyval, &dataval, 0); 
	  
	  if (returncode == 1) {
	    /* no matching special record in the index */
	    fprintf(LOGFILE, "Index not initialized correctly\n");
	    returncode = idx->db->close(idx->db, 0); 
	    return (NULL);
	  }
	  else if (returncode == -1) {
#ifndef WIN32
	    fprintf(LOGFILE, "cf_index_open: db->get returns error %d.\n", 
		    errno);
#else
	    fprintf(LOGFILE, "cf_index_open: db->get returns error.\n");
#endif	    
	    returncode = idx->db->close(idx->db, 0); 
	    return (NULL);
	  }
	  else {
	    memcpy((char *)idx->GlobalData, (char*)dataval.data, 
		   dataval.size);
	    if (idx->GlobalData->recptr == 0) 
	      idx->GlobalData->recptr = 1;
	    
#ifdef DEBUGCF
	    printf ("Index Max termid = %d  tot_occur %d  tot_numrecs %d\n", 
		    idx->GlobalData->recptr, idx->GlobalData->tot_occur, 
		    idx->GlobalData->tot_numrecs);
#endif
	  }
	  /* check if this is a paged index and set flag if so */
	  cf_pagedindex(idx);
	  
	  if ((idx2 = find_index_name(cf, idx->name)) != NULL
	      && idx != idx2 && idx->db == idx2->db)
	    idx->GlobalData = idx2->GlobalData;
	  
	}
	else { 
	  /* this index was opened previously for another index so */
	  /* set the idx->GlobalData pointer to the correct struct */
	  
	  if ((idx2 = find_index_name(cf, idx->name)) != NULL
	      && idx != idx2 && idx->db == idx2->db)
	    idx->GlobalData = idx2->GlobalData;
	  
	}
	
	if (idx->stopwords_name != NULL) {
	  if ((idx->stopwords_file = fopen(idx->stopwords_name, "r")) 
	      == NULL) {
	    fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		    idx->stopwords_name);
	    exit(1);
	  }
	  Tcl_InitHashTable(&(idx->stopwords_hash),TCL_STRING_KEYS);
	  while (fgets(keybuffer,KEYBUFFERSIZE,idx->stopwords_file)
		 != NULL) {
	    keybuffer[strlen(keybuffer)-1] = '\0'; /* eliminate newline */
	    Tcl_CreateHashEntry(&(idx->stopwords_hash),keybuffer,&exists);
	  }
	}
	else /* initialize it but put nothing in */
	  Tcl_InitHashTable(&(idx->stopwords_hash),TCL_STRING_KEYS);
	

       	if (idx->expansion_name != NULL) {
	  if ((idx->expansion_file = fopen(idx->expansion_name, "r")) 
	      == NULL) {
	    fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		    idx->expansion_name);
	    exit(1);
	  }
	  Tcl_InitHashTable(&(idx->expansion_hash),TCL_STRING_KEYS);
	  while (fgets(keybuffer,KEYBUFFERSIZE,idx->expansion_file)
		 != NULL) {
	    char *content, *tmp;
	    keybuffer[strlen(keybuffer)-1] = '\0'; /* eliminate newline */
	    tmp = strchr(keybuffer,'\t');
	    if (tmp == NULL)
	      continue;
	    *tmp = '\0';
	    content = tmp+1;
	    Tcl_SetHashValue(
		    Tcl_CreateHashEntry(&(idx->expansion_hash),
					keybuffer,&exists),
	            (ClientData)strdup(content));
	  }
	}
	else /* initialize it but put nothing in */
	  Tcl_InitHashTable(&(idx->expansion_hash),TCL_STRING_KEYS);
	

	/* check if this is a paged index and set flag if so */
	if (cf_pagedindex(idx)) {	  
	  char namebuff[500];
	
	  sprintf(namebuff,"%s.PAGEDMAP", idx->name);
	  if ((idx->page_rec_assoc_file = 
	       fopen(namebuff, cf_open_file_flags)) == NULL) {
	    fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		    namebuff);
	    exit(1);
	  } else { /* get the highest page number */
	    fread(&(idx->highest_page_number),sizeof(long),1, 
		  idx->page_rec_assoc_file);		
	  }
	  sprintf(namebuff,"%s.PAGEDFILES", idx->name);
	  if ((idx->page_file_names = 
	       fopen(namebuff, cf_open_file_flags)) == NULL) {
	    fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		    namebuff);
	    exit(1);
	  }
	}
	/* check if this is a proximity index  */
	if (idx->type & PROXIMITY) {
	  char namebuff[500];
	  
	  sprintf(namebuff,"%s.PROX", idx->name);
	  idx->prox_db = init_index_db(namebuff, idx->type, &num_open, cf);
	  
	  if (idx->prox_db == NULL) {
	    /* no index file? must not have run indexing on the DB */
	    fprintf(LOGFILE, "PROXIMITY Index %s not initialized correctly\n",
		    idx->name);
	    return (NULL);
	  }
	}

	/* check if this is a proximity index  */
	if (idx->type & VECTOR_TYPE) {
	  char namebuff[500];
	  
	  sprintf(namebuff,"%s.VECTOR", idx->name);
	  idx->vector_db = init_index_db(namebuff, idx->type, &num_open, cf);
	  
	  if (idx->vector_db == NULL) {
	    /* no index file? must not have run indexing on the DB */
	    fprintf(LOGFILE, "Vector Index %s not initialized correctly\n",
		    idx->name);
	    return (NULL);
	  }
	}


	if (idx->type & GEOTEXT && idx->gaz_config_file == NULL) {
	  if (idx->extern_app_name == NULL) {
	    fprintf(LOGFILE, "No gazetteer information in <extern_app> for index %s\n", idx->tag);
	    return (NULL);
	  }
	  if (cf_init_gaz_configfile(idx) == -1)
	    return(NULL);
	}
      }
      /* either it was already open, or was just opened */
      switch (which) {
      case INDEXFL:
	return( (void *)idx->db);
	break;
	
      case INDEXPROXFL:
	return( (void *)idx->prox_db);
	break;
	
      case INDEXVECTORFL:
	return( (void *)idx->vector_db);
	break;
	
      case INDEXSTOPWORDS:
	return((void *)idx->stopwords_file);
	break;
	
      case INDEXEXPANSION:
	return((void *)idx->expansion_file);
	break;
	
      case PAGEASSOCFILE:
	return((void *)idx->page_rec_assoc_file);
	break;
	
      case PAGENAMEFILE:
	return((void *)idx->page_file_names);
	break;
	
      default:
	fprintf(LOGFILE,"invalid file type in cf_index_open\n");
	exit (1);
      } 
    }
  }
  
  fprintf (LOGFILE, "File %s, index %s not found\n", filename, indexname);
  return (NULL);
}



/* check to see if an index file is a paged file */
int cf_pagedindex(idx_list_entry *idx)
{
  if (idx == NULL)
    return 0;

  if ((idx->type & PAGEDINDEX) == PAGEDINDEX ) 
    return 1; /* already seen this one */

  if ((idx->type & EXTERNKEY) != EXTERNKEY) 
    return 0; /* if not declared "KEYWORD_EXTERNAL" it can't be paged */

  if (idx->keys == NULL || idx->keys->subkey == NULL)
    return 0; /* no keys? */

  if (idx->keys->subkey->attribute_flag == 1 
      && (strcasecmp(idx->keys->subkey->key, "PAGED_DIRECTORY_REF") == 0)) {
    idx->type |= PAGEDINDEX; /* for later references */
    return 1;
  }
  else return 0;
}


/* open a SGML DTD for this file, parse it, and return the DTD pointer */
SGML_DTD *cf_getDTD(char *filename)
{
  config_file_info *cf;


  cf = find_file_config(filename);

  if (cf) {
      
    if (cf->DTD_parsed == NULL && cf->DTD_name != NULL) {
      cf->DTD_parsed = sgml_parse_dtd(cf->DTD_name, cf->SGML_Catalog_name,
				      cf->XML_Schema_Name, cf->included_schemas,
				      cf->XML_Schema);
      return (cf->DTD_parsed);
    }
    else if (cf->DTD_name == NULL) {
      if (cf->file_type != FILE_TYPE_VIRTUALDB
	  && cf->file_type != FILE_TYPE_XML_NODTD) {
	fprintf(LOGFILE, "Warning: No DTD name in config file for '%s'\n",cf->nickname);
	exit;
      }
      else {
	/* virtual DB -- has no DTD */
	return NULL;
      }
    }
    /* otherwise must have both a name and a pointer, so return it */
    return (cf->DTD_parsed);
    
  } 

  /* no matching filename found */
  fprintf (LOGFILE, "File %s not found in cf_getDTD\n", filename);
  return (NULL);
}


FILE *cf_cluster_open(char *filename, char *clustername, int type) 
{
  cluster_list_entry *cf_getcluster_entry();
  cluster_list_entry *cluster;
  char *fname, *cf_getfilename();  
  char keybuffer[KEYBUFFERSIZE];
  int exists;

  cluster = cf_getcluster_entry(filename, clustername);

  if (cluster == NULL)
    return NULL;

  if (type == CLUSTERTEMP) {
    if (cluster->temp_file == NULL) {
      if (cluster->temp_file_name == NULL) {
	fname = cf_getfilename(clustername, MAINFILE);
	if (fname) {
	  cluster->temp_file_name = CALLOC(char, (strlen(fname) + 5));
	  sprintf(cluster->temp_file_name,"%s.tmp",fname);
	}
	else {
	  fprintf(LOGFILE,"failed to find cluster filedef for %s\n", 
		  clustername);
	  exit(1);
	}
      }
      if ((cluster->temp_file = fopen(cluster->temp_file_name,
				      cf_open_file_flags)) == NULL) {
	/* probably no temp file yet, so create it */
	if ((cluster->temp_file = fopen(cluster->temp_file_name,
				      cf_create_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to open cluster temp %s\n", 
		  cluster->temp_file_name);
	  exit(1);
	}
      }
      
      if (cluster->cluster_stoplist_name != NULL) {
	if ((cluster->cluster_stoplist = 
	     fopen(cluster->cluster_stoplist_name, "r")) == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n cluster stoplist?", 
		  cluster->cluster_stoplist_name);
	  exit(1);
	}
	Tcl_InitHashTable(&(cluster->stopwords_hash),TCL_STRING_KEYS);
	while (fgets(keybuffer,KEYBUFFERSIZE,cluster->cluster_stoplist)
	       != NULL) {
	  keybuffer[strlen(keybuffer)-1] = '\0'; /* eliminate newline */
	  Tcl_CreateHashEntry(&(cluster->stopwords_hash),keybuffer,&exists);
	}
      }
      else /* initialize it but put nothing in */
	Tcl_InitHashTable(&(cluster->stopwords_hash),TCL_STRING_KEYS);
    }

    return (cluster->temp_file);
  }
  else { /* for any other type (MAINFILE, ASSOCFILE, etc) */ 
         /* open the underlying cluster file              */
    return (cf_open(clustername,type));
	  
  }
  
  return NULL; /* no other file types allowed */
}

DB *
cf_component_open(char *filename, char *componentname) 
{
  
  config_file_info *cf;
  component_list_entry *comp;
  DBT keyval;
  DBT dataval;
  int count_data;
  int returncode;
  char termbuffer[30] ;
  DB *db_handle;
  
  char keybuffer[KEYBUFFERSIZE];
  int exists;
  
  cf = find_file_config(filename);
  
  if (cf) {
    comp = find_component_name(cf, componentname);
    if (comp) {
      if (comp->comp_db == NULL) {
	int num_open;
	
	comp->comp_db = init_index_db(comp->name, 0, &num_open, cf);
	
	if (comp->comp_db == NULL) {
	  /* no index file? must not have run indexing on the DB */
	  fprintf(LOGFILE, "Not able to open component file %s \n",
		  comp->name);
	  return (NULL);
	}
	
      }
      /* get the tracking variable from the comp_db file */
      memset(&keyval,0,sizeof(DBT));
      memset(&dataval,0,sizeof(DBT));
      
      sprintf(termbuffer, ">>>COMPONENT COUNT<<<");
      keyval.data = (void *) termbuffer;
      keyval.size = strlen(termbuffer) + 1;
      
      returncode = comp->comp_db->get(comp->comp_db, NULL, &keyval, &dataval, 0); 
      
      if (returncode == 1) {
	/* no matching special record in the index */
	fprintf(LOGFILE, "Initializing Component db %s\n", comp->name);

	/* initialize the tracking variable */
	/* this is a new database file, so we install a standard item */
	/* for tracking the maximum component numbers                 */
	count_data = 0;
	dataval.data = (void *) &count_data;
	dataval.size = sizeof(int);

	/* write this data, close to flush it, then reopen via init_index_db */
	returncode = comp->comp_db->put(comp->comp_db, NULL, &keyval, &dataval, 
					DB_NOOVERWRITE); 
	  
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

	comp->comp_db->sync(comp->comp_db,0);

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

	return (comp->comp_db);

      }
      else if (returncode == -1) {
#ifndef WIN32
	fprintf(LOGFILE, "cf_component_open: db->get returns error %d.\n", 
		errno);
#else
	fprintf(LOGFILE, "cf_component_open: db->get returns error.\n");
#endif

	return (NULL);
      }
      else {
	memcpy((char *) &(comp->max_component_id), (char*)dataval.data, 
	       dataval.size);
	return (comp->comp_db);
      }
    }
  }
  fprintf (LOGFILE, "File %s, Component %s not found\n", filename, 
	   componentname);
  return (NULL);
}

int
cf_open_datastore(config_file_info *cf) {

  DB *db_handle;
  int db_errno;
  

  if (cf->file_db != NULL) {
    return(0);
  }

  if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
    gb_dbenv->err(gb_dbenv, db_errno,
		  "db_create failed to create handle\n");
    return(db_errno);
  }
  
  db_handle->set_pagesize(db_handle, 8 * 1024);
  
  if (db_errno = db_handle->open(db_handle,
				 cf->file_name, NULL, DB_BTREE, 
				 ( cf_open_file_flags[0] == 'r' 
				   && cf_create_file_flags[0] == 'r'
				   && cf_open_file_flags[1] == '\0' 
				   && cf_create_file_flags[1] == '\0'
				   ? DB_RDONLY : 0), 
				 0664) != 0) {
    db_handle->err(db_handle, db_errno, 
		   "db_handle->open failed to open %s \n", 
		   cf->file_name);
    return (db_errno);
  }
  
  /* should have opened the database file */
  
  cf->file_db = db_handle;

  return(0);  
}











