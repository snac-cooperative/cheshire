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
*	Header Name:	cf_getdisplay.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	get one a display format by name for a given file
*                       
*	Usage:		cluster_list_entry *cf_getdisplay(filename, oid, 
*                                                         formatname)
*                       filename can be the full pathname or short "nickname"
*                       oid should be the z39.50 OID for a record syntax
*                       formatname should be the z39.50 element set name
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		1/8/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "z_parameters.h"
#include <string.h>
#ifdef WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#ifdef WIN32
#define strncasecmp _strnicmp 
#endif
#endif

extern config_file_info *cf_info_base;
config_file_info *find_file_config(char *filename);

display_format *cf_getdisplay(char *filename, char *oid, char *format)
{
  
  config_file_info *cf;
  display_format *df;

  if (cf = find_file_config(filename)) {  
    for(df = cf->display; df; df = df->next_format) {
      if (format == NULL && oid == NULL && df->default_format == 1)
	return(df);
      else if (format && (
			  strcasecmp(format,df->name) == 0
			  || (strncasecmp(format,"XML_ELEMENT_", 12) == 0)
			     && strncasecmp(df->name,"XML_ELEMENT_", 12) == 0) ) {
	/* The format name is a match -- now check OIDS for syntax */
	if (oid == NULL) {
	  return(df);
	}
	else if (df->oid != NULL && strcasecmp(oid, df->oid) == 0) {
	  /* REAL match */
	  return(df);
	}
	else if (df->oid == NULL && 
		 (strcasecmp(oid, SGML_RECSYNTAX) == 0 
		  || strcasecmp(oid, XML_RECSYNTAX) == 0
		  || strcasecmp(oid, GRS1RECSYNTAX) == 0
		  || strcasecmp(oid, SUTRECSYNTAX) == 0 )) {
	  return(df); /* probably a simple exclude or GRS-1 CONVERSION */
	}
	
      }
      else if (format == NULL && oid != NULL && df->oid != NULL
	       && (strcasecmp(oid, df->oid) == 0)) {
	return(df);
      }
    }
  }
  
  return(NULL);
}


display_format *cf_getdefaultdisplay(char *filename)
{
  
  config_file_info *cf;
  display_format *df;

  if (cf = find_file_config(filename)) {  
    for(df = cf->display; df; df = df->next_format) {
      if (df->default_format == 1)
	return(df);
    }
  }  
  return(NULL);
}





