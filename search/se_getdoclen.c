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
*	Header Name:	se_getdoclen.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	reads SGML document length from Associator file.
*
*	Usage:		se_getdoclen(docid, filename?)
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
#include "configfiles.h"

extern page_assoc_rec *se_get_page_assoc(char *filename, char *indextag, 
					 int pagenum);
extern config_file_info *find_file_config(char *filename);

extern component_data_item *get_component_record(int component_id, 
						 component_list_entry *comp);
extern int VerifyDataStore(char *filename, int docid, assoc_rec *assocrec);

int se_getdoclen(int docid, char *filename, idx_list_entry *idx) 
{
  FILE *infile;
  assoc_rec address;
  int total_docsize;
  page_assoc_rec *page_assoc_rec_data = NULL;
  component_data_item *comp_data;
  config_file_info *cf;


  if (docid == 0 || filename == NULL || idx == NULL) 
    return 1;

  if (idx->type & PAGEDINDEX) { /* paged files have a doc associator file */
    page_assoc_rec_data = se_get_page_assoc(filename, idx->tag, docid);
    if (page_assoc_rec_data != NULL)
      return(page_assoc_rec_data->recsize);
    else
      return 1;
  } 
  else if (idx->type & EXTERNKEY) { 
    /* external files  and freq files have an associated  DOCSIZES file */
    infile = (FILE *) cf_open(filename, DOCSIZEFILE);
    if (infile) {
      fseek(infile, (docid * sizeof(int)), 0);
      fread(&total_docsize, sizeof(int), 1, infile);
      return(total_docsize);
    }
    else 
      return (0);
  }
  else if (idx->type & COMPONENT_INDEX ) {
    comp_data = get_component_record(docid, idx->comp_parent);
    if (comp_data == NULL)
      return (1);
    total_docsize = comp_data->end_offset - comp_data->start_offset;
    FREE (comp_data);
    return (total_docsize);
  }
  else {
    cf = find_file_config(filename);
    if (cf->file_type > 99) {
      VerifyDataStore(filename, docid, &address);
	return (address.recsize);
    }
    else {
      infile = cf_open(filename, ASSOCFILE);
      
      if (infile) {
	fseek(infile, (docid*sizeof(assoc_rec)), 0);
	fread(&address, sizeof(assoc_rec), 1, infile);
	return ((int)address.recsize);
      }
      else
	fprintf(LOGFILE,"Couldn't get assoc record in getdoclen\n");
      
      /* on errors return 1 */
    }
    return (1);
  }
}


double
se_get_avg_document_len(char *filename) {
  FILE *infile;
  assoc_rec address;
  double total_docsize, final_result;
  int nrecs, i;
  config_file_info *cf;


  cf = find_file_config(filename);
  
  if (cf->file_type > 99) {
    return(AvgDataStoreSize(filename, cf));
  }

  infile = cf_open(filename, ASSOCFILE);
  total_docsize = 0.0;

  if (infile) {
    fseek(infile, 0, 0);
    fread(&address, sizeof(assoc_rec), 1, infile);
    nrecs = address.offset;
    /* new version uses buildassoc created total in second long */
    if (address.recsize > 0)
      total_docsize = (double)address.recsize;
    else {
      /* OLD inefficient version... */
      for (i = 0; i < nrecs; i++) {
	fread(&address, sizeof(assoc_rec), 1, infile);
	total_docsize += (double)address.recsize;
      }
    } 

    final_result = total_docsize/(double)nrecs;
      
    return (final_result);

  }
  else
    return (total_docsize);
  
}

/* estimate average component length for components (approx) */
double 
se_get_avg_component_len(component_list_entry *comp)
{
  DBT key, data;
  DBC *dbcursor;
  component_data_item comp_data;
  int comp_id;
  int n, numrecs;
  int errno;
  double sum_size, avg_size;
  
  /* the component database should already be opened before this */
  /* function is called                                          */

  if (comp == NULL || comp->comp_db == NULL) 
    return (0.0); /* no data */

  
  /* Acquire a cursor for the database. */
  if ((errno = comp->comp_db->cursor(comp->comp_db, 
				     NULL, &dbcursor, 0)) != 0) {
    fprintf(stderr, "se_get_avg_component_len: cursor open error: %s\n", strerror(errno));
    return(0.0);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&key, 0, sizeof(key));
  memset(&data, 0, sizeof(data));
  
  /* Walk through the database and get the start and end info for comp */
  numrecs = 100;
  n = 0;

  while ((errno = dbcursor->c_get(dbcursor, &key, &data, DB_NEXT)) == 0
	 || numrecs > 0) {
    
    numrecs--;
    n++;

    if (key.size == sizeof(int)) {
      memcpy((char *)&comp_data,data.data,data.size);
      sum_size += (double)(comp_data.end_offset - comp_data.start_offset);
    }
    
  }

  (void)dbcursor->c_close(dbcursor);
  
  if (n > 0) {
    avg_size = sum_size/(double)n;
    
  }
  
  return(avg_size);

}









