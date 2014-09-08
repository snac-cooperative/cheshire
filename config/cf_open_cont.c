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
*	Header Name:	cf_open_cont.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	routines to open files under the control of
*                       the configfile structure and return file 
*                       pointers -- opens the appropriate continuation
*                       file for multi-file databases.
*
*	Usage:		FILE *cf_open(filename, which, recordid)
*                       filename can be the full pathname or short "nickname"
*                       which can be MAINFILE, ASSOCFILE, POSTINGFILE;
*                       (defined in cheshire.h)
*                       recordid is the record number sought (or 0 if
*                       immaterial).
*
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		7/15/97
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"

extern config_file_info *cf_info_base;


config_file_info *find_file_config(char *name);
char *cf_getDBfilename(config_file_info *cf, continuation *cont, int recordid);
FILE *cf_open(char *filename, int which);


FILE *
cf_open_cont_DB(config_file_info *cf, continuation *cont, int recordid)
{
  continuation *newcont;
  char *actualfilename;
  
  actualfilename = cf_getDBfilename(cf, cont, recordid);
  
  if (actualfilename == NULL) {
    return(NULL);
  }
  
  
  if (cf->cont_id && cf->current_cont != NULL) {
    /* if it is the same recordid, just return it... */
    if (cf->current_cont->docid_min == recordid) {
      return (cf->current_cont->file);
    } 
    else { /* close the currently open file */
      fclose(cf->current_cont->file);
      cf->current_cont->file = NULL;
      if (cf->current_cont->name != NULL)
	FREE(cf->current_cont->name);
      FREE(cf->current_cont);
      cf->current_cont = NULL;
      cf->cont_id = 0;
    }
  }
  
  newcont = CALLOC(continuation,1);
  newcont->name = actualfilename; 
  newcont->id_number = 1;
  newcont->docid_min = recordid;
  newcont->docid_max = recordid;
  
  cf->cont_id = 1;
  if ((cf->file_ptr = fopen(newcont->name, cf_open_file_flags))
      == NULL) {
    if ((cf->file_ptr = fopen(newcont->name, "r")) == NULL) {
      fprintf(LOGFILE,"failed to open %s\n has create been run?", 
	      newcont->name);
      return(NULL);
    }
  }

  cf->current_cont = newcont;
  cf->current_cont->file = cf->file_ptr;
  return(cf->file_ptr);
  
}


FILE *
cf_open_cont(char *filename, int which, int recordid)
{
  
  SGML_DTD *cf_getDTD();
  FILE *firstfile;
  continuation *cont;
  config_file_info *cf;

  if (recordid == 0 || which != MAINFILE)
    return (cf_open(filename, which));

  cf = find_file_config(filename);

  if (cf != NULL) {
    if (cf->file_ptr == NULL) { /* i.e.: first time files have opened */
      firstfile = cf_open(filename, which);
    }
    if (cf->filecont != NULL && cf->filecont->dbflag == 1) {
      return(cf_open_cont_DB(cf, cf->filecont, recordid));
    }
    if (cf->filecont != NULL) {

      
      if (cf->current_cont != NULL 
	  && (recordid >= cf->current_cont->docid_min 
	  && recordid <= cf->current_cont->docid_max)) {
	/* already have the correct file open */
	return (cf->file_ptr);
      }
      else {
	/* find the continuation file matching the recordid */
	for (cont = cf->filecont; cont != NULL; cont = cont->next_cont) {
	  if (recordid >= cont->docid_min && recordid <= cont->docid_max) {
	    /* this is the correct continuation file */
	    if (cont->id_number == cf->cont_id) /* already open */
	      return (cont->file);
	    else {
	      /* close existing cont file */
	      if (cf->current_cont->file) {
		fclose(cf->current_cont->file);
		cf->current_cont->file = NULL;	      
	      }
	      cf->cont_id = cont->id_number;
	      if ((cf->file_ptr = fopen(cont->name, cf_open_file_flags))
		  == NULL) {
		if ((cf->file_ptr = fopen(cont->name, "r")) == NULL) {
		  fprintf(LOGFILE,"failed to open %s\n has create been run?", 
			  cont->name);
		  return(NULL);
		}
	      }
	      cf->current_cont = cont;
	      cf->current_cont->file = cf->file_ptr;
	      return(cf->file_ptr);
	    }
	  }
	}  
	if (cont == NULL) /* didn't find a cont file with recordid in range */
	  return (NULL);

      }
    }    
    return(cf->file_ptr); /* this is a single file */
  }
  return (NULL); /* no cf entry for this file */
}
















