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
*	Header Name:	se_get_result.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Store and Retrieve stored result sets by name
*                       and extract elements from them.
*
*	Usage:		se_get_result(setid)
*                           get the result set by id.
*
*                       weighted_result *se_get_result_subset(res_ptr,recnos)
*                           get only some of the items in a result set.
*
*                       se_store_result_set(weighted_result *set, 
                                            char *setid,
*                                           char *search_file, int flags)
*                           store a result set under a setid for a given file.
*
*                       se_delete_result_set(char *setid)
*                           get rid of the in-core and disk form of a RS.
*
*                       se_delete_all_result_sets()
*                           get rid of all result sets for this session.
*
*                       char *se_get_result_filetag(char *setid)
*                           given a setid, return the filename where the file
*                           is stored.
*
*	Variables:      setid: identifier for the particular set.
*                      res_ptr: a pointer to result_set_data struct (see below)
*                       recnos: a char represenation of a set of record numbers
*
*	Return Conditions and Return Codes:	
*
*	Date:		12/5/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* the following includes the Z39.50 defs */
#include <stdlib.h>
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"
#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

extern void diagnostic_set();
extern void se_free_result();

#define REPLACE_RESULT_SETS  1
#define MAKE_PERM_RESULT_SET 2
#define DONT_SAVE_RESULT_SETS 4

#define TMPFILEDIR "/usr2/tmp"
#define TMPFILEPREFIX "chesh"

Tcl_HashTable *result_set_hash_tab = NULL, result_set_hash_tab_data;

typedef struct result_set_data 
    {
      char filename[500];
      char search_file_name[500]; /* this may NOT be the real file with */
                                  /* records listed in result.          */
      int  userid;
      int  really_stored;
      weighted_result *result_ptr;
      weighted_result result;
    } result_set_data;



extern weighted_result *se_regex_scan(weighted_result *in_set, char *regex[], 
			      int nregex);

weighted_result *se_get_result_subset(result_set_data *res_ptr,char *recnos);

#ifndef LINUX
#ifndef MACOSX
char *strtok_r(char *s1, char *s2, char **savept);
#endif
#endif

Tcl_HashEntry *
se_check_result_set(char *setid, char **recnos) {
  Tcl_HashEntry *entry;
  char temp_setid[100];
  char *c, *setname;
  int userid, current_user_id();


  if (setid == NULL) {
    return (NULL);
  }

  if (result_set_hash_tab == NULL) {
    result_set_hash_tab = &result_set_hash_tab_data;
    Tcl_InitHashTable(result_set_hash_tab,TCL_STRING_KEYS);
  }

  /* build the hashed set name */
  /* first check to see if the set name is qualified with numbers. */
  /* and use only the name portion */
  if (setid[0] == '\0')
    return NULL;

  setname = strtok_r(setid,":,",recnos);


  if (*recnos == NULL || *recnos[0] == '\0') 
    *recnos = NULL;

  if (setname == NULL) {
    /* set diagnostic */
    diagnostic_set(128,0,"NULL"); /* doesn't exist */
    return (NULL);
  }


  userid = current_user_id(NULL);
  sprintf(temp_setid,"%s_%d",setname,userid);
  for(c = temp_setid; *c; c++)
    *c = toupper(*c);

  /* check the hash table */
  entry = Tcl_FindHashEntry(result_set_hash_tab,temp_setid);
      
  if (entry == NULL){ /* nope, not found */
    /* set diagnostic */
    diagnostic_set(30,0,NULL); /* doesn't exist */
    return (NULL);
  }

  return (entry);

}

weighted_result *
se_get_result_set(char *setid)
{
  FILE *res_file;
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  weighted_result *wt_res;
  char *recnos = NULL;

  if (setid[0] == '\0') 
    setid = "default";

  if ((entry = se_check_result_set(setid, &recnos)) == NULL)
    return (NULL);
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
    

  if (res_ptr->result_ptr != NULL) {
    /* still in core */
    if (recnos == NULL)
      return (res_ptr->result_ptr);
    else
      return (se_get_result_subset(res_ptr,recnos));
  }
  else {
    char *fmode;
#ifdef WIN32
    fmode = "rb";
#else
    fmode = "r";
#endif

    /* read it in from a file */
    res_file = fopen(res_ptr->filename,fmode);
    
    if (res_file == NULL) {
      /* set diagnostic that result set deleted */
      diagnostic_set(27,0,NULL); 
      return (NULL);
    }
  
    fread(res_ptr, sizeof(result_set_data), 1, res_file);

    wt_res = (weighted_result *) 
      CALLOC (char, (sizeof(weighted_result) + 
	       (res_ptr->result.num_hits * sizeof(weighted_entry))));

    *wt_res = res_ptr->result;
    res_ptr->result_ptr = wt_res;

    fread(&wt_res->entries[1], sizeof(weighted_entry), 
	  res_ptr->result.num_hits, res_file);

    fclose(res_file);
    
    if (recnos == NULL)
      return (wt_res);
    else
      return (se_get_result_subset(res_ptr,recnos));
  }
}

weighted_result *
se_get_result_subset(result_set_data *res_ptr,char *recnos)
{
  weighted_result *wt_res, *in_res;
  char *tempnos;
  char *recnumber;
  char *range;
  int *recno_list;
  int new_recs = 0;
  int search_keys = 0;
  char *lastptr;
  char *regex[30];
  int regexcount = 0;
  int i;
  
  /* space for building list of records to extract */
  recno_list = CALLOC(int,res_ptr->result_ptr->num_hits);

  in_res = res_ptr->result_ptr;

  tempnos = strdup(recnos);
  
  recnumber = strtok_r(tempnos,",", &lastptr);

  while (recnumber != NULL) {
    if (search_keys == 1) {
      /* a search of things in the recno_list for this word has been */
      /* requested */
      if (regexcount > 30)
	break;
      else
	regex[regexcount++] = recnumber;
    }
    else {
      if ((range = strchr(recnumber,'-')) == NULL) {
	/* sets start with record #1, not zero , so change for indexes */
	if (strcasecmp(recnumber,"FIND") == 0) {
	  /* we will assume that this is a request for search in doc */
	  search_keys = 1 ;
	}
	else
	  recno_list[new_recs++] = atoi(recnumber) - 1;
	
      }
      else {/* there is a range*/
	int start;
	int end;
	
	if (sscanf(recnumber,"%d-%d",&start,&end) == 2) {

	  if (start > in_res->num_hits) {
	    /* set diagnostic */
	    diagnostic_set(100,0,
			   "Starting record number out of range in feedback set\n");
	    return (NULL);
	  }
	  if (end > in_res->num_hits)
	    end = in_res->num_hits;
	  
	  if (end < start) {
	    end = start;
	  }

	  for (i = start-1; i < end; i++)
	    recno_list[new_recs++] = i;
	  
	}
	else {
	  /* set diagnostic */
	  diagnostic_set(100,0,
			 "negative number or ill-formed range in feedback set\n");
	  return (NULL);
	}
      }
    }
    recnumber = strtok_r(NULL,",", &lastptr);

  }
  
  
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + 
		   ( new_recs * sizeof(weighted_entry))));
  
  memcpy(wt_res, in_res, sizeof(weighted_result));
	 
  wt_res->num_hits = new_recs;
  
  /* create a new setid or 'erase' the old one */
  if (search_keys == 1)
    strcat(wt_res->setid, "_kwscan");
  else 
    wt_res->setid[0] = '\0';

  /* strcpy(in_res->filename,wt_res->filename);
     wt_res->result_type = in_res->result_type;
     wt_res->component = in_res->component
     wt_res->result_weight = in_res->result_weight;
     wt_res->result_type = in_res->result_type;
  */
  
  for (i = 0; i < new_recs; i++) {
    int indx;
    indx = recno_list[i];
    if (indx >= 0 && indx < in_res->num_hits) {
      wt_res->entries[i].record_num = in_res->entries[indx].record_num;
      wt_res->entries[i].weight = in_res->entries[indx].weight;
    }
    else {
      /* set diagnostic */
      diagnostic_set(100,0,
		     "Invalid number or range in feedback set\n");
      FREE(wt_res);
      return (NULL);
    }

  }

  if (search_keys == 1 && regexcount >= 1) 
    return(se_regex_scan(wt_res, regex, regexcount));
  else
    return(wt_res);

}

weighted_result *
se_dup_result_set(weighted_result *set) 
{
  int i, newset_size;
  weighted_result *newset;

  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * set->num_hits);
  newset = (weighted_result *) CALLOC(char, newset_size);
  
  /* structure copy */
  *newset = *set;
  

  for (i = 0; i < set->num_hits; i++) {
      newset->entries[i].record_num = set->entries[i].record_num; 
      newset->entries[i].weight = set->entries[i].weight;
      newset->entries[i].xtra = set->entries[i].xtra;

  }
  
  return (newset);

}

int
se_store_result_set(weighted_result *set, char *setid, char *search_file,
		    char *save_directory, int flags)
{
  Tcl_HashEntry *entry;
  int exists;
  result_set_data *res_ptr;
  char temp_setid[100];
  char tempfilename[500];
  FILE *res_file;
  int userid, current_user_id();
  char *recnos;
  char *c;
  int dup_flag;

  if (set == NULL) 
    return(FAIL);

  dup_flag = 0;

  userid = current_user_id(NULL);
  
  if (set->setid[0] != '\0') {
    /* already stored if it has a name... */
    if (strcasecmp(set->setid, setid) == 0)
      return(SUCCEED);
    else
      dup_flag = 1;
  }
  
  if ((entry = se_check_result_set(setid, &recnos)) == NULL) {
    /* nope, it is new entry */
    /* create the hash table structure */

    
    if (dup_flag) /* assume the set is modified or changed somehow */
      set = se_dup_result_set(set);


    res_ptr = CALLOC(struct result_set_data, 1) ;

    if (setid[0] == '\0') 
      setid = "default";

    sprintf(temp_setid,"%s_%d",setid,userid);
    for(c = temp_setid; *c; c++)
      *c = toupper(*c);

    sprintf(tempfilename, "%s/%sXXXXXX", save_directory, TMPFILEPREFIX);
#ifdef WIN32
    mktemp(tempfilename);
#else
    mkstemp(tempfilename);
#endif
    strncpy(res_ptr->filename, tempfilename, 499);

    strncpy(res_ptr->search_file_name, search_file, 499);
    res_ptr->userid = userid;
    res_ptr->result_ptr = set;
    strncpy(set->setid,setid,199);
    res_ptr->result = *set; 

    /* put it into the hash table */
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(result_set_hash_tab,
					 temp_setid, &exists), 
		     (ClientData)res_ptr);


    if ((flags & DONT_SAVE_RESULT_SETS) != DONT_SAVE_RESULT_SETS) {
      char *fmode;
#ifdef WIN32
      fmode = "wb";
#else
      fmode = "w";
#endif
      /* and store it in a file */
      res_file = fopen(res_ptr->filename,fmode);
    
      if (res_file == NULL) {
	/* set some diagnostic... */
	diagnostic_set(2,0,"Unable to open result set file");
	return (FAIL);
      }
      
      fwrite(res_ptr, sizeof(result_set_data), 1, res_file);
      fwrite(&set->entries[1], sizeof(weighted_entry), 
	     set->num_hits, res_file);
      res_ptr->really_stored = 1;
      fclose(res_file);
    }
    else {
      unlink(tempfilename);
    }

    return (SUCCEED);
  }
  else { /* setid already in hash table, so just replace it */
    if (flags & REPLACE_RESULT_SETS == REPLACE_RESULT_SETS) {
      res_ptr =  
	(struct result_set_data *) Tcl_GetHashValue(entry);

      /* this comes first because with duplicated RS the name may vanish */
      /* when the old incore result is freed                             */
      strncpy(res_ptr->search_file_name, search_file, 199);

      /* free the old in-core result set */
      se_free_result(res_ptr);

      res_ptr->result_ptr = set;
      strncpy(set->setid,setid,199);
      res_ptr->result = *set; 
      

      if (flags & DONT_SAVE_RESULT_SETS != DONT_SAVE_RESULT_SETS) {
	char *fmode;
#ifdef WIN32
	fmode = "wb";
#else
	fmode = "w";
#endif

	/* and store it in a file */
	res_file = fopen(res_ptr->filename,fmode);
    
	if (res_file == NULL) {
	  /* set some diagnostic... */
	  diagnostic_set(2,0,"Unable to open result set file");      
	  return (FAIL);
	}
      
	fwrite(res_ptr, sizeof(result_set_data), 1, res_file);
	fwrite(&res_ptr->result.entries[1], sizeof(weighted_entry), 
	       res_ptr->result.num_hits, res_file);
	res_ptr->really_stored = 1;
	fclose(res_file);
      }
      return(SUCCEED);

    }
    else { /* set diagnostic for no replace flag, etc. */
      diagnostic_set(21,0,NULL);
      return(FAIL);
    }
  }
}



int
se_delete_result_set(char *setid)
{
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  char *recnos;

  if (result_set_hash_tab == NULL) /* nothing there yet */
    return (SUCCEED);


  if ((entry = se_check_result_set(setid, &recnos)) == NULL) {
    /* set diagnostic */
    diagnostic_set(30,0,NULL);
    return (FAIL);
  }
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
    
  if (strcmp(setid,"***TMP*BITMAP*RESULTS***") != 0
      && res_ptr->really_stored == 1)
    unlink(res_ptr->filename);
  
  se_free_result(res_ptr);

  FREE(res_ptr);
  
  Tcl_DeleteHashEntry(entry);

  return (SUCCEED);
}


int
se_delete_all_result_sets()
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  result_set_data *res_ptr;


  if (result_set_hash_tab == NULL) /* nothing there yet */
    return (SUCCEED);

  /* remove everything from the hash table */

  for (entry = Tcl_FirstHashEntry(result_set_hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);

    if (res_ptr->really_stored == 1)
      unlink(res_ptr->filename);

    se_free_result(res_ptr);

    FREE(res_ptr);
  
  }

  Tcl_DeleteHashTable(result_set_hash_tab); /* clean up */
  result_set_hash_tab = NULL;

  return (SUCCEED);
}



char *
se_get_all_result_set_names()
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  result_set_data *res_ptr;
  int size=0;
  char *name_list;


  if (result_set_hash_tab == NULL) /* nothing there yet */
    return ("");

  /* first get the size for the result... */
  for (entry = Tcl_FirstHashEntry(result_set_hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);

    size += strlen(res_ptr->result.setid) + 1;
  }

  name_list = CALLOC(char,size);
  
  /* now create the result ... */
  for (entry = Tcl_FirstHashEntry(result_set_hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);

    strcat(name_list, res_ptr->result.setid);
    strcat(name_list, " ");
  }
  

  return (name_list);
}



/* free the in-core result set associated with the result_set_data */
void
se_free_result(  result_set_data *res_ptr)
{
  if (res_ptr->result_ptr != NULL) {
    FREE(res_ptr->result_ptr);
    res_ptr->result_ptr = NULL;
  }
}

void
se_free_result_name(char *setid)
{
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  char *recnos;

  if (result_set_hash_tab == NULL) return;

  if ((entry = se_check_result_set(setid, &recnos)) == NULL) {
    return;
  }
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
    
  se_free_result(res_ptr);
  return;
}



char *
se_get_result_filetag(char *setid)
{
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  char *recnos;;

  if (setid[0] == '\0') 
    setid = "default";


      
  if ((entry = se_check_result_set(setid, &recnos)) == NULL) {
    /* set diagnostic */
    diagnostic_set(30,0,NULL); /* doesn't exist */
    return (NULL);
  }
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
  
  return(res_ptr->result.filename); /* this SHOULD be the REAL file */
  
}

int
se_get_result_type(char *setid)
{
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  char *recnos;;

  if (setid[0] == '\0') 
    setid = "default";


      
  if ((entry = se_check_result_set(setid, &recnos)) == NULL) {
    /* set diagnostic */
    diagnostic_set(30,0,NULL); /* doesn't exist */
    return (0);
  }
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
  
  return(res_ptr->result.result_type); /* return the type only */
  
}

int
se_get_result_hits(char *resultname)
{
  Tcl_HashEntry *entry;
  result_set_data *res_ptr;
  char *recnos;

  if (resultname[0] == '\0') 
    resultname = "default";


  if ((entry = se_check_result_set(resultname, &recnos)) == NULL) {
    /* set diagnostic */
    diagnostic_set(30,0,resultname); /* doesn't exist */
    return (-1);
  }
  else /* found the entry */
    res_ptr = (struct result_set_data *) Tcl_GetHashValue(entry);
  
  return(res_ptr->result.num_hits); 

}





