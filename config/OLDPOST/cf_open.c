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
#include "cheshire.h"
#include "configfiles.h"

extern config_file_info *cf_info_base;

/* build a hash table of index names for a particular file */
void
build_index_hash(config_file_info *cf) 
{
  char *indexname;
  idx_list_entry *idx;
  int exists;

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
       

  }
}


/* search the global hash table of file names for the configfile info */
config_file_info *
find_file_config(char *filename)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch q_hash_search;
  char *name;

  if (filename == NULL)
    return (NULL);

  if (cf_file_names == NULL) {
    fprintf(LOGFILE,
	    "cf_file_names hash table not initialized in find_file_config\n");
    return (NULL);
  }
  
  entry = Tcl_FindHashEntry(cf_file_names,filename);
  if (entry == NULL) {
    fprintf(LOGFILE, 
	    "find_file_config: Couldn't locate file name in hash table\n");
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
    fprintf(LOGFILE, 
	    "find_index_name: Couldn't locate index name in hash table\n");
    return (NULL);
  }
  else 
    return ((idx_list_entry *)Tcl_GetHashValue(entry));

}


FILE *
cf_open(char *filename, int which)
{
  
  SGML_DTD *cf_getDTD();

  config_file_info *cf;


  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_ptr == NULL) { /* i.e.: first time files have opened */

      if (cf->filecont) {
	if ((cf->file_ptr = fopen(cf->filecont->name, cf_open_file_flags))
	    == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  cf->filecont->name);
	  exit(1);
	}
	cf->filecont->file = cf->file_ptr;
	cf->cont_id = 1;
	cf->current_cont = cf->filecont;
      }	
      else {
	if ((cf->file_ptr = fopen(cf->file_name, cf_open_file_flags))
	    == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  cf->file_name);
	  exit(1);
	}
      }
      if ((cf->DTD_parsed = cf_getDTD(cf->file_name)) == NULL) {
	fprintf(LOGFILE, "failed to get DTD in cf_open\n");
	exit(1);
      }
      if ((cf->assoc_ptr = fopen(cf->assoc_name, cf_open_file_flags))
	  == NULL) {
	fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		cf->assoc_name);
	exit(1);
      }
      if ((cf->history_ptr = fopen(cf->history_name, cf_open_file_flags)) 
	  == NULL) {
	fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		cf->history_name);
	exit(1);
      }
      /* also build the index name hash table for the file... */
      build_index_hash(cf);
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
	  exit(1);
	}
      }
      return(cf->docsize_ptr);
      break;

    otherwise:
      fprintf(LOGFILE,"invalid file type in cf_open\n");
      return(NULL);
    }
  }
  
  fprintf(LOGFILE,"file '%s' not found cf_open. Error in Config file?\n",
	  filename);
  return(NULL);
}

#define KEYBUFFERSIZE 200

void *cf_index_open(char *filename, char *indexname, int which)
{
  
  config_file_info *cf;
  idx_list_entry *idx, *idx2;
  DB *init_index_db();

  char keybuffer[KEYBUFFERSIZE];
  int exists;

  cf = find_file_config(filename);

  if (cf) {

    idx = find_index_name(cf, indexname);
    if (idx) {
      if (idx->db == NULL) {
	DBT keyval;
	DBT dataval;
	char srchbuffer[30];
	int returncode, num_open;
	
	idx->db = init_index_db(idx->name, idx->type, &num_open);
	
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
	  
	  returncode = idx->db->get(idx->db, &keyval, &dataval, 0); 
	  
	  if (returncode == 1) {
	    /* no matching special record in the index */
	    fprintf(LOGFILE, "Index not initialized correctly\n");
	    return (NULL);
	  }
	  else if (returncode == -1) {
	    fprintf(LOGFILE, "cf_index_open: db->get returns error %d.\n", 
		    errno);
	    return (NULL);
	  }
	  else {
	    memcpy((char *)idx->GlobalData, (char*)dataval.data, 
		   dataval.size);
	    if (idx->GlobalData->recptr == 0) 
	      idx->GlobalData->recptr = 1;
	    
#ifdef DEBUG
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
	
	if ((idx->assoc_file = fopen(idx->assoc_name, 
				     cf_open_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  idx->assoc_name);
	  exit(1);
	}
	if ((idx->postings_file = fopen(idx->postings_name, 
					cf_open_file_flags)) == NULL) {
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
		  idx->postings_name);
	  exit(1);
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
      }
      
      switch (which) {
      case INDEXFL:
	return( (void *)idx->db);
	break;
	
      case INDEXASSOC:
	return((void *)idx->assoc_file);
	break;
	
      case INDEXPOSTINGS:
	return((void *)idx->postings_file);
	break;
	
      case INDEXSTOPWORDS:
	return((void *)idx->stopwords_file);
	break;
	
      case PAGEASSOCFILE:
	return((void *)idx->page_rec_assoc_file);
	break;
	
      case PAGENAMEFILE:
	return((void *)idx->page_file_names);
	break;
	
      otherwise:
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

  if (idx->keys->subkey->attribute_flag 
      && (strcasecmp(idx->keys->subkey->key, "PAGED_DIRECTORY_REF") == 0)) {
    idx->type |= PAGEDINDEX; /* for later references */
    return 1;
  }
  else return 0;
}



/* open an opendb index (btree only for now) */
DB *init_index_db (char *idxname, int idxtype, int *number_open)
{
  Tcl_HashTable *hash_tab;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  static int hashtableinit = 1;
  int exists, flags;
  extern DB *dbopen();
  DB *db_handle;
  static BTREEINFO open_info;
  void *open_info_ptr;
  int accesstype;

  accesstype = idxtype & DB_MASK;
  
  *number_open = 0;
  
  if (hashtableinit) {
    /* initialize the global hash table */
    IndexNamesHash = CALLOC(Tcl_HashTable,1);
    Tcl_InitHashTable(IndexNamesHash,TCL_STRING_KEYS);
    hashtableinit = 0;
  }

  hash_tab = IndexNamesHash;

  /* check the hash table of names */
  entry = Tcl_FindHashEntry(hash_tab,idxname);

  /* set the flags for opening */
  if (strncmp(cf_open_file_flags,"r+",2) == 0)
    flags = O_RDWR;
  else
    flags = O_RDONLY;

  if (entry == NULL){

    db_handle = dbopen( idxname, flags, 0666, 
		       accesstype, NULL /*defaults*/);

    if (db_handle == NULL) {
      if (*number_open != -1) /* set during create, so a null result OK */
	fprintf(LOGFILE, "db_open failed in cf_open.c\n"); 
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

/* open a SGML DTD for this file, parse it, and return the DTD pointer */
SGML_DTD *cf_getDTD(char *filename)
{
  SGML_DTD *sgml_parse_dtd();
  config_file_info *cf;


  cf = find_file_config(filename);

  if (cf) {
      
    if (cf->DTD_parsed == NULL && cf->DTD_name != NULL) {
      cf->DTD_parsed = sgml_parse_dtd(cf->DTD_name, cf->SGML_Catalog_name);
      return (cf->DTD_parsed);
    }
    else if (cf->DTD_name == NULL) {
      fprintf(LOGFILE, "No DTD name in config file for '%s'\n",cf->nickname);
      exit(1);
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
	  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
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














