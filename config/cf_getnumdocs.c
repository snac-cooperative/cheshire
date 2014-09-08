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
*	Header Name:	cf_getnumdocs.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility routine for retrieving the number of docs
*                        in a main file
*	Usage:		cf_getnumdocs( mainfilename)
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/14/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include <math.h>

extern FILE *LOGFILE;
extern config_file_info *find_file_config(char *filename);
int cf_getnumVdocs (char *filename);

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags, 
				    int casesensitive);


extern char *detag_data_block (SGML_Data *data, int index_type, 
			       idx_list_entry *idx);


int
cf_getnumDSdocs(char *filename) {
  DB *db_handle;
  DBTYPE dbtype;
  DBT keyval;
  DBT dataval;
  int high_recno;
  int returncode;
  config_file_info *cf;
  


  /* should have opened the database file elsewhere... */

  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_db != NULL) { 
      db_handle = cf->file_db;
    }
    else {
      cf_open_datastore(cf);
      if (cf->file_db == NULL) {
	fprintf(LOGFILE, "Datastore filename or filetag '%s' not initialized\n",
		filename);
	return (0);  
      }
      else
	db_handle = cf->file_db;
    }
  }
  else {
    fprintf(LOGFILE, "Couldn't get configfile info for filename or filetag '%s'\n", 
	    filename);
    return (0);  
  }
  
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
	  
  keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
  keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
	  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&high_recno,dataval.data, sizeof(int));
    return (high_recno);
  }
  else if (returncode == DB_NOTFOUND) { 
    fprintf(LOGFILE, "Datastore file '%s' not initialized (no data?)\n", 
	    filename);
    return (0);  
    
  }
}

int 
cf_getnumVdocs (char *filename) 
{
  config_file_info *virt_conf;
  filelist *fl;
  int total_recs;
  int dbcount, db, res_count;

  virt_conf = find_file_config(filename);
  total_recs = 0;
  dbcount = 0;
    
  for (fl = virt_conf->othernames; fl; fl = fl->next_filename) {
    total_recs += cf_getnumdocs(fl->filename);
  }
  return(total_recs);

}

int
cf_getnumdocs (char *filename)
{
  FILE *main_assoc;
  long maxtermid;
  int filetype;
  int virtualsum;

  if ((filetype = cf_getfiletype(filename)) == FILE_TYPE_DBMS) 
    return(0);

  if (filetype > 99) {
    return (cf_getnumDSdocs(filename));
  }
  
  if (filetype == FILE_TYPE_VIRTUALDB) {
    return (cf_getnumVdocs(filename));
  }
  else {
    main_assoc = cf_open(filename, ASSOCFILE);
  
    if (main_assoc == NULL) {
      fprintf(LOGFILE, 
	      "couldn't open %s assoc file in cf_getnumdocs\n",filename);
      /* cheshire_exit(1); */
      return(0);
    }
  
    /* to get the maxtermid set in the assoc  */
    rewind(main_assoc);
    fread(&maxtermid, sizeof(long), 1, main_assoc);
  
  
    return(maxtermid);  
  }
}


int 
cf_get_dist_docsize(char *filename, int recnum, 
		    SGML_Document *doc, config_file_info *cf, int doclen)
{
  int N_dist_docs = 0;
  FILE *docsizes = NULL;
  SGML_Tag_Data *datlist;
  int casesensitive = 0;
  char *distdata;

#ifdef REALLY_NEED_TO_HAVE_AN_EXACT_COUNT_OF_DISTDOCS
  /* this function either extracts the number of distributed docs in doc */
  /* using the key in cf, or reads it from the associated file           */
  if (doc != NULL && cf != NULL) {
    
    /* if it has already been extracted, just return it */
    if (doc->dist_docsize > 0)
      return (doc->dist_docsize);

    /* extract it from the record */
    if (cf->file_type == FILE_TYPE_XML
	|| cf->file_type == FILE_TYPE_XML_NODTD
	|| cf->file_type == FILE_TYPE_XML_DATASTORE)
      casesensitive = 1;
 
    datlist = 
      comp_tag_list(cf->distrib_docsize_key, doc->Tag_hash_tab,
		    NULL, casesensitive);

    if (datlist == NULL) {
      return(0);
    }

    distdata = detag_data_block(datlist->item, 0, NULL); 

    sscanf(distdata, "%d", &N_dist_docs);
    doc->dist_docsize = N_dist_docs;
    FREE(distdata);
    FREE(datlist);

  }
  else if (filename != NULL && recnum != 0) {
    /* get it from the DOCSIZES file */
    docsizes = (FILE *) cf_open(filename, DOCSIZEFILE);
    if (docsizes != NULL) {
      fseek(docsizes, (recnum * sizeof(int)), 0);
      fread(&N_dist_docs, sizeof(int), 1, docsizes);
      if (doc != NULL)
	doc->dist_docsize = N_dist_docs;
    }
  }
#else

  N_dist_docs = 1 + (int)(593.91065 + (0.01071 * (double)doclen) + (-18.95184 * sqrt((double)(doclen/10))));
  
  if (doc != NULL)
    doc->dist_docsize = N_dist_docs;
  
#endif
  
  return (N_dist_docs);
}





