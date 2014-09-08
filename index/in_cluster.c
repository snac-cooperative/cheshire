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
*	Header Name:	in_cluster.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Routine to extract clustering information 
*                       from a record being indexed. This version does batch
*                       creation of elements, which are then sorted and
*                       merged to create the true cluster file.
*
*	Usage:		in_cluster(filename, recnum, SGML_Record)
*                       filename can be the full pathname or short "nickname"
*                       recnum is the logical record ID number for the record
*                       being indexed.
*                       SGML_Record is a pointer to the actual SGML_Data struct
*                       if it is available, otherwise (when NULL) the record
*                       with the specified recnum will be read from the file.
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCEED) on successful completion
*                       or 0 (FAIL) on error;
*
*	Date:		2/28/95
*
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"

extern config_file_info *cf_info_base;
extern int errno;

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);
extern void free_tag_data_list(SGML_Tag_Data *in_tags);

extern int get_key_data(SGML_Data *data, char **current_buffer);

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

#define MAXOUTPUTLINE 20000


int
splitbuffer(FILE *cluster_temp, int recnum, char *key, char *tag,
	    char *buffer, char *sum, int maxnum, int have_key)
{
  char *inbuff;
  char *newstart;
  inbuff = buffer;
  
  while (strlen(inbuff) > MAXOUTPUTLINE) {
    newstart = inbuff + (MAXOUTPUTLINE - (MAXOUTPUTLINE / 8));
    /* go back to the first blank space */
    while (*newstart != ' ') newstart--;
    *newstart++ = '\0';
    if (have_key == 0) 
      fprintf(cluster_temp, "%s%c", key,'\001');
    else 
      have_key = 0;

    fprintf(cluster_temp, "<%s", tag);
    if (maxnum != 0)
      fprintf(cluster_temp, " maxnum=%d>", maxnum);
    else if (sum != NULL)
      fprintf(cluster_temp, " summary=%s>", sum);
    else 
      fprintf(cluster_temp, ">");
    fprintf(cluster_temp, "%s", inbuff);
    fprintf(cluster_temp, "</%s>", tag);
    fprintf(cluster_temp,"<N>%d</N>\n",recnum);  
    inbuff = newstart;
  }
  
  /* starting a new record less than max size */
  fprintf(cluster_temp, "%s%c", key,'\001');   
  fprintf(cluster_temp, "<%s", tag);
  if (maxnum != 0)
    fprintf(cluster_temp, " maxnum=%d>", maxnum);
  else if (sum != NULL)
    fprintf(cluster_temp, " summary=%s>", sum);
  else 
    fprintf(cluster_temp, ">");
  fprintf(cluster_temp, "%s", inbuff);
  fprintf(cluster_temp, "</%s>", tag);
  
  return (0);
}


int
check_outlen(FILE *cluster_temp, int recnum, char *key, char *data_buffer, 
	     int *outputlength, int newlen,  cluster_map_entry *cmap) 
{
  if ((*outputlength + newlen) > MAXOUTPUTLINE) {
    
    if (*outputlength == (strlen(key) + 1)) {
      /* there is a single chunk of data that is TOO long */
      *outputlength = 
	splitbuffer(cluster_temp, recnum, key, 
		    cmap->to->key, data_buffer, 
		    (cmap->summarize ? cmap->summarize->key : NULL), 
		    cmap->sum_maxnum, 1);
      
      return (2);
    }
    else {
      *outputlength =
	splitbuffer(cluster_temp, recnum, key, 
		    cmap->to->key, data_buffer, 
		    (cmap->summarize ? cmap->summarize->key : NULL), 
		    cmap->sum_maxnum, 0);
      return (1);
    }
  }
  else {
    return (0);
  } 
}



int in_cluster (char *filename, int recnum, SGML_Document *in_sgmlrec)
{
  FILE *cluster_temp;

  cluster_list_entry *cl, *cf_getcluster();
  cluster_map_entry *cmap;

  idx_key *from_key;
  SGML_Tag_Data *matchdata, *td, *contentdata, *ctd;
  SGML_Document *sgmlrec;
  SGML_Data *keys_data;
  idx_key *attr_subkey;
  idx_key *k;
  int check_result;
  int key_saved;
  int attribute_flag = 0;
  int read_sgml_record = 0;
  int filetype;
  char *get_attr_value();
  char *key_buffer;
  char *data_buffer;
  int casesensitive;
  int outputlength;
  int num_keys, num_targets;

  SGML_Document *GetSGML();    /* SGML record from mainfile */

  if (cf_info_base == NULL) {
    fprintf(LOGFILE,"cf_info_base is NULL in in_cluster -- need to initialize\n");
    return(FAIL);
  }
  
  filetype = cf_getfiletype(filename);
  if (filetype != FILE_TYPE_SGMLFILE 
      && filetype != FILE_TYPE_SGML_DATASTORE
      && filetype != FILE_TYPE_XML
      && filetype != FILE_TYPE_XML_DATASTORE
      && filetype != FILE_TYPE_MARC_DATASTORE
      && filetype != FILE_TYPE_CLUSTER_DATASTORE
      && filetype != FILE_TYPE_CLUSTER
      && filetype != FILE_TYPE_MARCFILE) {
    fprintf(LOGFILE,"file %s is not SGML or cluster in in_cluster\n", filename);
    return(FAIL);
  }

  /* Read the record from the sgml file and parse */
  if (in_sgmlrec == NULL) {
    sgmlrec = GetSGML(filename,recnum);

    if (sgmlrec == NULL) { 
      fprintf(LOGFILE, "in_cluster: SGML parse error for record %d\n",recnum);
      return(FAIL);
    }
    read_sgml_record = 1;
  }
  else sgmlrec = in_sgmlrec;

  key_buffer = NULL;
  data_buffer = NULL;

  if (sgmlrec->DTD->type > 0)  /* XML or XML_Schema */
    casesensitive = 1;
  else
    casesensitive = 0;

  for (cl = cf_getcluster(filename) ; cl ; cl = cl->next_entry) {
#ifdef DEBUGIND
    printf("cluster = %s\n", cl->name);
#endif
    cluster_temp = (FILE *) cf_cluster_open(filename, cl->name, CLUSTERTEMP);

    if (cluster_temp == NULL) {
      fprintf(LOGFILE, "Unable to open cluster temp file for record %d\n",recnum);
      return(FAIL);
    }
      
    fseek(cluster_temp, 0L, SEEK_END); /* goto the end of file */

    matchdata = comp_tag_list(cl->cluster_key,sgmlrec->Tag_hash_tab, 
			      NULL, casesensitive);

    attribute_flag = 0;
    attr_subkey = NULL;

    for (k = cl->cluster_key; k ; k = k->subkey) {
      if (k->attribute_flag == 1) {
	attribute_flag = k->attribute_flag;
	attr_subkey = k;
      }
    }

    num_keys = 0;
    num_targets = 0;

    for (td = matchdata; td; td = td->next_tag_data) {
      

      keys_data = td->item;
      
      if (key_buffer) {
	FREE(key_buffer);
	key_buffer = NULL;
      }
      if (data_buffer) {
	FREE(data_buffer);
	data_buffer = NULL;
      }
      

      num_keys++;

      if (num_keys > 100)
	continue;
      
      if (num_keys == 100) {
	fprintf(LOGFILE, "More than 100 cluster key items in cluster generation, skipping the rest of record #%d\n",recnum);
	fflush(LOGFILE);
      }

      
      if (attribute_flag != 0)
	key_buffer = get_attr_value(keys_data, attr_subkey->key);
      else
	get_key_data(keys_data, &key_buffer);
      
      if (key_buffer != NULL) {
	int morphflag;
	char *norm_key;
	Tcl_HashEntry *entry;
	idx_list_entry fakeidx;
	char *tmp;
	
	
	outputlength = 0;
	
	memset(&fakeidx,0,sizeof(idx_list_entry));
	fakeidx.type = cl->normalization;
	
	/* normalize the key  -- converting diacritics */
	norm_key = normalize_key(key_buffer, &fakeidx,
				 &morphflag, 1);
	
	/* write it out to the file */
	if (norm_key) {
	  /* check the NORMALIZED key against the cluster stoplist */
	  entry = Tcl_FindHashEntry(&cl->stopwords_hash, norm_key);
	  if (entry == NULL) { /* not a stopword */
	    
	    /* print a "super-blank" so that shorter things sort before longer */
	    outputlength += fprintf(cluster_temp, "%s%c", norm_key,'\001');   
	    if (norm_key != key_buffer) {
	      tmp = key_buffer;
	      key_buffer = norm_key;
	      FREE(tmp);
	    }
	    
	    
	    for (cmap = cl->field_map; cmap ; cmap = cmap->next_clusmap) {

	      for (from_key = cmap->from; from_key; 
		   from_key = from_key->next_key) {
		
		contentdata = comp_tag_list(from_key, sgmlrec->Tag_hash_tab, 
					    NULL, casesensitive);
		
		num_targets = 0;

		for (ctd = contentdata; ctd; ctd = ctd->next_tag_data) {

		  num_targets++;

		  if (num_targets > 100)
		    continue;

		  if (num_targets == 100) {
		    fprintf(LOGFILE, "More than 100 FROM items in a single document (%d) during cluster generation, skipping the rest...\n", recnum);
		    fflush(LOGFILE);
		  }

		  get_key_data(ctd->item, &data_buffer);
		  /* look ahead -- if this is all one heading merg things */
		  /* by calling get_key_data again...                     */
		  if (ctd->next_tag_data != NULL &&
		      ctd->next_tag_data->item->parent == ctd->item->parent)
		    continue;
		  
		  if (data_buffer != NULL) {
		    if (cmap->summarize && cmap->summarize->key != NULL) {
		      if ((check_result = 
			   check_outlen(cluster_temp, recnum, key_buffer,
					data_buffer, &outputlength,
					(2 * strlen(cmap->summarize->key)+ 20),
					cmap))
			  == 0)
			outputlength += 
			  fprintf(cluster_temp,"<%s maxnum=%d></%s>",
				  cmap->summarize->key, cmap->sum_maxnum,
				  cmap->summarize->key);
		      
		      if ((check_result = 
			   check_outlen(cluster_temp, recnum, key_buffer, 
					data_buffer, &outputlength,
					(2 * strlen(cmap->to->key)
					 + strlen(cmap->summarize->key)
					 + strlen(data_buffer) + 20), cmap))
			  == 0)
			outputlength += 
			  fprintf(cluster_temp,"<%s summary=%s>%s</%s>",
				  cmap->to->key, cmap->summarize->key,
				  data_buffer, cmap->to->key);  
		      
		    }
		    else {
		      if ((check_result = 
			   check_outlen(cluster_temp, recnum, key_buffer, 
					data_buffer, &outputlength,
					(2 * strlen(cmap->to->key)
					 + strlen(data_buffer) + 20), cmap))
			  == 0)
			outputlength += 
			  fprintf(cluster_temp,"<%s>%s</%s>",cmap->to->key,
				  data_buffer, cmap->to->key);  
		    }
		    FREE(data_buffer);
		    data_buffer = NULL;
		  }
		  else {
		    /* we put put empty fields for no data */
		    if (cmap->summarize && cmap->summarize->key != NULL) {
		      if ((check_result = 
			   check_outlen(cluster_temp, recnum, key_buffer,
					data_buffer, &outputlength,
					(2 * strlen(cmap->to->key)+ 
					 + strlen(cmap->summarize->key) + 20),
					cmap))
			  == 0)
			outputlength += 
			  fprintf(cluster_temp,"<%s summary=%s> </%s>",
				  cmap->to->key, cmap->summarize->key,
				  cmap->to->key);
		    }
		    else {
		      if ((check_result = 
			   check_outlen(cluster_temp, recnum, key_buffer, 
					data_buffer, &outputlength,
					(2 * strlen(cmap->to->key)+ 20),
					cmap)) 
			  == 0)
			outputlength += 
			  fprintf(cluster_temp,"<%s> </%s>",
				  cmap->to->key,cmap->to->key);  

		    }
		  }
		}
		free_tag_data_list(contentdata);
		contentdata = NULL;
	      }
	    }
	    fprintf(cluster_temp,"<N>%d</N>\n",recnum);  
	    outputlength = 0;
	  }
	  if (key_buffer) { /* these will not have been freed if the key */
	    if (norm_key != key_buffer)
	      FREE(norm_key);
	    FREE(key_buffer);  /* is on the cluster stoplist */
	    key_buffer = NULL;
	  }
	}
      }
    }
    free_tag_data_list(matchdata);
  }
  /* free up the document, should be finished with it */
  if (read_sgml_record) 
    free_doc(sgmlrec);
  
  return (SUCCEED);
}




