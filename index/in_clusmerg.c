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
*	Header Name:	in_clusmerg.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routine to merge temporary cluster records
*                       extracted in in_cluster (q.v.) into a complete
*                       cluster file. The current version expects to
*                       have the complete cluster information in the
*                       temp file, and no pre-existing main cluster file
*
*	Usage:		in_clusmerg(filename)
*                       filename can be the full pathname or short "nickname"
*                          for the source file being clustered
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or 0 (FAIL) on error;
*
*	Date:		10/31/93
*	Revised:	11/7/93 (bug fixes)
*                       07/19/93 (Major Revision to Handle SGML records)
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#define strcasecmp _stricmp
#endif
#include "cheshire.h"
#include <sys/stat.h>

extern cluster_list_entry *cf_getcluster(char *filename);
extern SGML_DTD *cf_getDTD(char *filename);
extern int cf_getfiletype(char *filename);


extern config_file_info *cf_info_base;
extern int errno;

/* the following structures are used in accumulating the data */
/* for each cluster                                           */

struct clusnode {
  struct clusnode *next;
  char *data;
  int recnum;
};

struct clusfield {
  struct clusfield *next;
  char *tag;
  SGML_Element *element_ptr;
  Tcl_HashTable *summarize_hash;
  int sum_maxnum;
  struct clusfield *summarize_field;
  int has_summary;
  struct clusnode *fld;
  struct clusnode *last_fld;
};

struct Cluster {
  char *cluskey;
  int clusternum; /* cluster ID */
  struct clusfield *fields;
  struct clusnode *recnums;
  struct clusnode *last_recnum;
};

struct summary_list {
  int frequency;
  char *data;
};



/* First initialization of cluster structure */
void
initcluster(struct Cluster *cluster, SGML_DTD *dtd) 
{
  struct clusfield *cf = NULL, *last_cf = NULL, *first_cf;
  SGML_Element *el;
  SGML_Tag_List *tags;
  char *tag, *up_case_string();

  cluster->cluskey = NULL;
  cluster->clusternum = 0;

  for (el = dtd->Elements; el; el = el->next_element) {

    if (el->tag != NULL) {
      if (strcasecmp(el->tag,"CLUSTER") == 0
	  || strcasecmp(el->tag,"CLUSKEY") == 0
	  || strcasecmp(el->tag,"FREQUENCY") == 0
	  || strcasecmp(el->tag,"SUMDATA") == 0
	  || strcasecmp(el->tag,"N") == 0) 
	continue;
      else {
	last_cf = cf;
	if ((cf = CALLOC(struct clusfield, 1)) == NULL) {
	  fprintf(LOGFILE, "Unable to alloc clusfield in initcluster\n");
	  exit(1);
	}
	if (last_cf)
	  last_cf->next = cf;
	else
	  first_cf = cf;
	
	cf->tag = up_case_string(strdup(el->tag));
	cf->element_ptr = el;
	cf->fld = NULL;
	cf->last_fld = NULL;
	/* check to see if this is a summary element */
	if (el->attribute_list != NULL) {
	  if (strcasecmp(el->attribute_list->name, "SUMMARY") == 0) {
	    /* indicates an element to be summarized */
	    cf->has_summary = 1;
	  }
	  if (strcasecmp(el->attribute_list->name, "MAXNUM") == 0) {
	    /* indicates a summary element */
	    /* initialize a hashtable structure */
	    if ((cf->summarize_hash = CALLOC(Tcl_HashTable, 1)) == NULL) {
	      fprintf(LOGFILE, "Unable to alloc summarize_hash in initcluster\n");
	      exit(1);
	    }
	    Tcl_InitHashTable(cf->summarize_hash,TCL_STRING_KEYS);
	  }
	}
      }
    }
    else if (el->tag_list != NULL) { 
      for (tags = el->tag_list ; tags ; tags = tags->next_tag) {
	if (strcasecmp(tags->tag,"CLUSTER") == 0
	    || strcasecmp(tags->tag,"CLUSKEY") == 0
	    || strcasecmp(el->tag,"FREQUENCY") == 0
	    || strcasecmp(el->tag,"SUMDATA") == 0
	    || strcasecmp(tags->tag,"N") == 0) 
	  continue;
	else {
	  last_cf = cf;
	  if ((cf = CALLOC(struct clusfield, 1)) == NULL) {
	    fprintf(LOGFILE, "Unable to alloc clusfield in initcluster\n");
	    return;
	  }
	  if (last_cf)
	    last_cf->next = cf;
	  else
	    first_cf = cf;
	  
	  tag = up_case_string(strdup(tags->tag));
	  cf->element_ptr = el;
	  cf->fld = NULL;
	  cf->last_fld = NULL;
	  /* check to see if this is a summary element */
	  if (el->attribute_list != NULL) {
	    if (strcasecmp(el->attribute_list->name, "SUMMARY") == 0) {
	      /* indicates an element to be summarized */
	      cf->has_summary = 1;
	    }
	    if (strcasecmp(el->attribute_list->name, "MAXNUM") == 0) {
	      /* indicates a summary element */
	      /* initialize a hashtable structure */
	      if ((cf->summarize_hash = CALLOC(Tcl_HashTable, 1)) == NULL) {
		fprintf(LOGFILE, "Unable to alloc summarize_hash in initcluster\n");
		exit(1);
	      }
	      Tcl_InitHashTable(cf->summarize_hash,TCL_STRING_KEYS);
	    }
	  }
	}
      }
    }
  }
  cluster->fields = first_cf;
  cluster->recnums = NULL;
  return;
}


void
freeclusnode(struct clusnode *node)
{
  struct clusnode *n, *prev;
    
  if (node == NULL)
    return;
  
  prev = node;

  for (n = node->next; n != NULL; n = n->next) {
    if (prev->data)
      FREE(prev->data);
  
    FREE(prev);
    prev = n;
  }

}

void
reinitcluster(struct Cluster *cluster) 
{
  struct clusfield *cf;

  /* if cluster is not empty, free the data in it */
  if (cluster->cluskey != NULL)
    FREE(cluster->cluskey);
  cluster->cluskey = NULL;

  /* free the field data, but leave the tags */
  for (cf = cluster->fields; cf; cf = cf->next) {
    freeclusnode(cf->fld);
    /* clear out the hash table for this summary */
    if (cf->summarize_hash) {
      Tcl_DeleteHashTable(cf->summarize_hash);
      Tcl_InitHashTable(cf->summarize_hash,TCL_STRING_KEYS);
    }
    cf->fld = NULL;
    cf->last_fld = NULL;
  }
  
  freeclusnode(cluster->recnums);
  cluster->recnums = NULL;

}

void
freeclusfield(struct clusfield *fld)
{
  if (fld == NULL)
    return;

  freeclusfield(fld->next);
  freeclusnode(fld->fld);
  FREE(fld->tag);
  FREE(fld);

}

void
freecluster(struct Cluster *cluster) 
{
  if (cluster == NULL)
    return;

  if (cluster->cluskey)
    FREE(cluster->cluskey);
  
  freeclusfield(cluster->fields);
  freeclusnode(cluster->recnums);

  cluster->cluskey = NULL;
  cluster->fields = NULL;
  cluster->recnums = NULL;
  cluster->last_recnum = NULL;
  cluster->clusternum = 0;


}

void
freesumhash(Tcl_HashTable *tab) 
{


}

struct clusfield *get_summarize_fld(struct clusfield *start_fld,
				     char *attr_name, char *attr_value)
{
  struct clusfield *fld;

  if (attr_name == NULL || attr_value == NULL)
    return NULL;

  if (strcasecmp(attr_name, "SUMMARY") != 0) {
    fprintf(LOGFILE, "Attr name not SUMMARY in get_summarize_fld: %s\n", 
	    attr_name);
    return NULL;
  }
  for (fld = start_fld; fld; fld = fld->next) {
    if (strcasecmp(fld->tag,attr_value) == 0)
      return (fld);
  }
  return(NULL); /* field not found */
}

void
add_summary_data(struct clusfield *sumfield, char *data) 
{

  Tcl_HashEntry *entry;

  int exists; 
  long freq_data;

  if (sumfield == NULL || data == NULL)
    return;

  entry = Tcl_FindHashEntry(sumfield->summarize_hash,data);
  
  if (entry == NULL){ /* nope, it is new word */
    /* put it into the hash table */
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(sumfield->summarize_hash,
					 data, &exists), 
			   (ClientData)1);
  }
  else { /* word already in hash table, so just increment it */
    freq_data = (long) Tcl_GetHashValue(entry);
    freq_data++;
    /* put it into the hash table */
    Tcl_SetHashValue(entry,(ClientData)freq_data);
  }

}

void
add_clus_keys(struct Cluster *cluster, char *data) 
{
  struct clusfield *field;
  struct clusnode *n;
  char *tag, *tagend, *nexttag, *datastart, *dataend;
  char *attr_start, *attr_end, *attr_value;
  int setend, foundtag;

  int err_count = 0;

  attr_start = attr_end = attr_value = NULL;
  tagend = nexttag = datastart = NULL;

  tag = data;

  while (tag) {

    if (*tag == '<') 
      tag++;

    tagend = strchr(tag,'>');
    *tagend = '\0';
    datastart = tagend+1;

    if (attr_end = strchr(tag,'=')) {
      *attr_end = '\0';
      attr_value = attr_end + 1;
      tagend = strchr(tag, ' ');
      *tagend = '\0';
      attr_start = tagend + 1;
    }

    setend = 0;
    nexttag = datastart;

    do {
      dataend = strstr(nexttag,tag);
      if (dataend) {
	if (*(dataend-1) == '/' && *(dataend-2) == '<') {
	  /* matched end */
	  *(dataend-2) = '\0';
	  setend = 1;
	}
	else {
	  nexttag = dataend +1; 
	  fprintf(LOGFILE, "skipping tag in data %s", dataend);
	}
      }
      else setend = -1;
    } while (setend == 0);
	
    if (setend == -1) {
      fprintf(LOGFILE,"mismatched tags in in_clusmerg\n");
      return;
    }
      
    foundtag = 0;

    for (field = cluster->fields; field; field = field->next) {
      if (strcasecmp(field->tag,tag) == 0) { /* matching tag */
	if (field->summarize_hash != NULL) {
	  if (attr_value)
	    field->sum_maxnum = atoi(attr_value);
	  foundtag = 1;
	}
	else {
	  if (field->has_summary) {
	    if(field->summarize_field == NULL) {
	      field->summarize_field = get_summarize_fld(cluster->fields,
							 attr_start, attr_value);
	    }
	    /* put the data into the hash table ... */
	    add_summary_data(field->summarize_field, datastart);
	    
	  } 
	  n = CALLOC(struct clusnode, 1);
	  if (field->fld == NULL) {
	    field->fld = n;
	    field->last_fld = n;
	  }
	  else { 
	    field->last_fld->next = n;
	    field->last_fld = n;
	  }
	  n->data = strdup(datastart);
	  foundtag = 1;
	}
      }
    }
    
    if (foundtag == 0) {
      if (strcasecmp(tag,"N") == 0) { /* the source record number */
	n = CALLOC(struct clusnode, 1);
	if (cluster->recnums == NULL) {
	  cluster->recnums = n;
	  cluster->last_recnum = n;
	}
	else { 
	  cluster->last_recnum->next = n;
	  cluster->last_recnum = n;
	}
	n->data = strdup(datastart);
      }
      /* if it isn't a known tag, we'll just ignore it */
    }
    /* point to the begining of the next tag */
    tag = strchr(dataend+1,'<');
  }
}

/* the following comparison for summary entries orders them from high to low */
int compare_sum_items (struct summary_list *e1, struct summary_list *e2) {
	if (e1->frequency < e2->frequency)
		return (1);
	else if (e1->frequency == e2->frequency)
		return (0);
	else 
		return (-1);
}


/* sort an array of entries by frequency     */
void
in_sortsummary (struct summary_list *set, int num_sum) {
  qsort ((void *) set, num_sum,
	 sizeof(struct summary_list), compare_sum_items);

}



void
output_cluster_rec (FILE *sgmlfile, FILE *afile, struct Cluster *cluster)
{
  struct clusfield *field;
  struct clusnode *n;

  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */


  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  if (cluster->cluskey == NULL)
    return; /* no cluster information */

  /* output the merged record in the proper format */
  /* and build the associator entry                */
  
  /* append to the end of the data file */
  fseek(sgmlfile, 0, 2);
  assocrec.offset = offset = ftell(sgmlfile);
  

  fprintf(sgmlfile,"<CLUSTER ID=\"%d\">\n<CLUSKEY>%s</CLUSKEY>\n", 
	  cluster->clusternum, cluster->cluskey);

  for (field = cluster->fields; field; field = field->next) {

    /* If this is a summary field, output the summarized info */
    if (field->summarize_hash != NULL) {
      struct summary_list *sums;
      int i, n_ent;

      fprintf(sgmlfile,"<%s MAXNUM = %d>\n", field->tag, field->sum_maxnum);

      n_ent = field->summarize_hash->numEntries;
      if (n_ent > 0) {
	/* make an array of summary_list for the contents of the hash table */
	sums = CALLOC(struct summary_list,n_ent);
	i = 0;
      
	for (entry = Tcl_FirstHashEntry(field->summarize_hash,&hash_search); 
	     entry != NULL; 
	     entry = Tcl_NextHashEntry(&hash_search)) {
	  /* get the value */
	  sums[i].data = Tcl_GetHashKey(field->summarize_hash,entry); 
	  /* and the frequency info */
	  sums[i].frequency =  (int) Tcl_GetHashValue(entry);
	  i++;
	}
	/* have the items, now sort them */
	in_sortsummary(sums, n_ent);
      
	for (i = 0; i < n_ent && i < field->sum_maxnum; i++) {
	  fprintf(sgmlfile,
		  "<frequency> %d </frequency><sumdata>%s</sumdata>\n",
		  sums[i].frequency, sums[i].data);
	}
	/* if we allocated them, better free them... */
	if (sums)
	  FREE(sums);
      }
      fprintf(sgmlfile,"</%s>\n", field->tag);
    }
    else { /* not a summary field -- output the regular data */
      /* put out the start tag for the field */
      if (field->has_summary && field->summarize_field != NULL) 
	/* if it is being summarized... */
	fprintf(sgmlfile,"<%s summary = \"%s\">\n", 
		field->tag, field->summarize_field->tag);
      else /* otherwise just the tag is output */
	fprintf(sgmlfile,"<%s>\n", field->tag);

      /* output the contents for the field */
      for (n = field->fld; n ; n = n->next) {
	fprintf(sgmlfile,"%s\n",n->data);
      }
      /* and the end tag */
      fprintf(sgmlfile,"</%s>\n", field->tag);
    }
  }
  fprintf(sgmlfile,"<N>\n");
  for (n = cluster->recnums; n ; n = n->next) {
    fprintf(sgmlfile,"%s\n",n->data);
  }
  fprintf(sgmlfile,"</N>\n</CLUSTER>\n");

  /* record offset and length of sgml record in assoc file */
  offset = ftell(sgmlfile);
  assocrec.recsize = offset - assocrec.offset;
  fseek(afile,cluster->clusternum*sizeof(assoc_rec), 0);
  fwrite(&assocrec, sizeof(assoc_rec), 1, afile);

}



int in_clusmerg (char *filename, char *temp_file_directory)
{
  struct Cluster cluster;
  int line_num = 0 ;
  int count = 0;
  char tmp_buffer[250000];
  char sort_file_name[2056];
  char *cluster_line, *temp_key, *cluskey_end, *last_key;
  FILE *sort_temp_file;
  cluster_list_entry *cl;
  struct stat filestatus;
  int statresult;
  time_t source_mod_time;
  SGML_DTD *dtd;
  char *tag;
  char *fmode;
  FILE *afile;        /* pointer to associator file */
  FILE *sgmlfile;     /* pointer to sgml file */
  int filetype;
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
                      /* added by this function and passed to index */
		      /* function. Also the value of the 0th record */
		      /* in assocfile to keep track of highest      */
		      /* logical recno in use.                      */
  

  if (cf_info_base == NULL) {
    fprintf(LOGFILE,"cf_info_base is NULL in in_clusmerg -- need to initialize\n");
    return(FAIL);
  }
  
  if ((filetype = cf_getfiletype(filename)) != FILE_TYPE_SGMLFILE 
      && filetype != FILE_TYPE_XML
      && filetype != FILE_TYPE_XML_NODTD
      && filetype != FILE_TYPE_XML_DATASTORE
      && filetype != FILE_TYPE_SGML_DATASTORE
      && filetype != FILE_TYPE_MARC_DATASTORE
      && filetype != FILE_TYPE_MARCFILE) { 
    fprintf(LOGFILE,"file %s is not SGML in in_clusmerg\n", filename);
    return(FAIL);
  }


  /* process each of the cluster files for this MAIN file */
  for (cl = cf_getcluster(filename); cl ; cl = cl->next_entry) {

    /* Read and parse the DTD for the cluster file */
    dtd = (SGML_DTD *) cf_getDTD(cl->name);

    if (dtd == NULL) { 
      fprintf(LOGFILE, "in_clusmerg: SGML parse error for cluster dtd\n");
      return(FAIL);
    }

    /* call function to get sgmlfile pointer */
    sgmlfile = cf_cluster_open(filename, cl->name, MAINFILE);
    if (sgmlfile == NULL) {
      fprintf(LOGFILE, "Could not open main file in in_clusmerg\n");
      return(FAIL);
    }

    /* call function to get assocfile pointer */
    afile = cf_cluster_open(filename, cl->name, ASSOCFILE);
    if (afile == NULL) {
      fprintf(LOGFILE, "Could not open associator file in in_clusmerg\n");
      return(FAIL);
    }

    cl->temp_file = cf_cluster_open(filename, cl->name, CLUSTERTEMP);

    /* get new highest logical record number */
    fseek(afile, 0, 0);
    fread(&high_recno,sizeof(int),1, afile); 
    recno = high_recno + 1;

    /* initialize the cluster structure */
    initcluster(&cluster,dtd);

    cluster.clusternum = recno;
    
    /* the first step is to run a sort on the temp cluster file */
    /* unless the sort file already exists.                     */
    fclose(cl->temp_file);
    
    if (stat(cl->temp_file_name, &filestatus) != 0) {
      fprintf(LOGFILE,
	      "stat for cluster data file %d failed in in_clusmerg \n",
	      cl->temp_file_name);
      exit(1);
    }
    source_mod_time = filestatus.st_mtime;
    
    sprintf(sort_file_name,"%s.sort", cl->temp_file_name);

    statresult = stat(sort_file_name, &filestatus);
    
    if ((statresult == 0 && source_mod_time > filestatus.st_mtime)
	|| (statresult == -1 && errno == ENOENT)) { 
      /* either there is no such file, or the tmp file is newer than */
      /* an existing sort file, so we will create it by a batch sort */
      if (temp_file_directory == NULL) {
	sprintf(tmp_buffer,"sort -o %s %s",  sort_file_name,
		cl->temp_file_name);
      }
      else {
	sprintf(tmp_buffer,"sort -T %s -o %s %s",
		temp_file_directory,  sort_file_name, cl->temp_file_name);
      }

      
      fprintf(LOGFILE,"Sorting cluster .tmp data file %s\n", 
	      cl->temp_file_name);
      fflush(LOGFILE);
      system(tmp_buffer);
    }
    /* otherwise use the existing .sort file */
      
    /* now we read in each record and accumulate those with the same */
    /* cluster key.                                                  */
#ifdef WIN32
    fmode = "rb";
#else 
    fmode = "r";
#endif

    sort_temp_file = fopen(sort_file_name, fmode);
    
    if (sort_temp_file == NULL) {
      fprintf(LOGFILE,"sort file is null in in_clusmerg \n");
      fflush(LOGFILE);
      exit(1);
    }

    last_key = NULL;

    line_num = 0;

    while ((cluster_line = fgets(tmp_buffer, 249999, sort_temp_file)) != NULL) {
      
      line_num++;

      if (strrchr(tmp_buffer,(int)'\n') == NULL) {
	fprintf(LOGFILE,
		"Cluster line longer than temp buffer in in_clusmerg \n");
	fflush(LOGFILE);
	exit(1);
      }

      if (last_key == NULL) 
	last_key = strdup(cluster_line);
      else {
	/* no need to process truly identical items from same record */
	if (strcmp(last_key,cluster_line) == 0) {
	  continue;
	}
	else { 
	  /* not identical -- make the new line the last key */
	  /* and free up the old last_key space */
	  FREE(last_key);
	  last_key = strdup(cluster_line);
	}
      }
                                                       
      if (line_num % 1000 == 0) {
	fprintf(LOGFILE,
	      "%d cluster lines processed in in_clusmerg \n",
	      line_num);
      }
      /* parse the line and accumulate the elements */
      temp_key = cluster_line;
      cluskey_end = strchr(cluster_line,'\001');
      if (cluskey_end == NULL) {
	fprintf(LOGFILE,"No key terminator in cluster key\n");
	exit(1);
      }
      *cluskey_end = '\0';
      if (cluster.cluskey == NULL) 
	cluster.cluskey = strdup(temp_key);

      tag = cluskey_end+1;

      if (strcmp(cluster.cluskey, temp_key) == 0) { 
	/* same key, so accumulate the data */
	add_clus_keys(&cluster,tag);
      }
      else {
	/* output the merged record in the proper format */
	/* and build the associator entry                */

	output_cluster_rec(sgmlfile,afile,&cluster);

	/* re-init the cluster entry for the next set */
	reinitcluster(&cluster);
if (temp_key == NULL) {
  printf("temp_key is NULL... exiting\n");
  exit (1);
}
	cluster.cluskey = strdup(temp_key);
	recno++;
	cluster.clusternum = recno;
	add_clus_keys(&cluster,tag);

      }
    }

    /* output the final merged record  */
    output_cluster_rec(sgmlfile,afile,&cluster);
   
    /* increment the zeroth record to record the */
    /* highest record number now in use          */
    fseek(afile, 0, 0);
    fwrite(&recno, sizeof(int), 1, afile);

    /* make sure the writes are done */
    fflush(sgmlfile);
    fflush(afile);

    /* clear out the cluster data -- including the old tags */
    freecluster(&cluster);
    if (last_key) 
      FREE(last_key);

    /* remove the temp and sort files (since the real file now exists) */
    fclose(sort_temp_file);
    /* unlink(cl->temp_file_name); - destroys data if a sorting error occurs */
    cl->temp_file = NULL;
    /* unlink(sort_file_name); leave it for now... */

  } /* end of loop to handle each cluster file */

  return (SUCCEED);
}

