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
*	Header Name:	se_process_meta.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Traverse a Z39.50 RPN query structure and
*                       do a metadata search -- the primary purpose
*                       in v. 1 is to find and return the complete
*                       DTD by name or by the name of files using the
*                       DTD.
*
*	Usage:		se_process_meta(RPNquery)
*
*	Variables:	RPNQuery -the query in Z39.50 RPN form.
*
*	Return Conditions and Return Codes:	
*                        returns a buffer containing the metadata (DTD, etc.)
*                        or NULL with a diagnostic set on error.
*
*	Date:		5/31/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* the following includes the Z39.50 defs */
#ifdef SOLARIS
#include <sys/stat.h>
#endif

#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "cheshire.h"

/* forward declarations */
extern char *cf_getdatafilename(char *filename, int recordid);
extern int cf_getnumdocs (char *filename);
extern SGML_DTD *cf_getDTD(char *filename);


/* process_meta_rpn_structure: resolves the query posed in the */
/* RPN structure.  NULL is returned on errors -- a string with */
/* the metadata (usually DTD) is returned otherwise.           */


char *se_process_meta_rpn_structure(RPNStructure *s) 
{

  Operand *oper;
  char *out_buffer = NULL;
  char *temp;
  int docid;
  char *public_id;
  char *system_id;
  int maxrecs;
  SGML_DTD *dtd;

  if (s == NULL) return (NULL); /* This SHOULD never happen */
  
  if (s->which == e3_rpnRpnOp) {
    /* set a diagnostic for this -- no binary op allowed in metadata queries */
    diagnostic_set(6,0,"Boolean operators not permitted in metadata search");
    return (NULL);
  }
  else if (s->which == e3_op) {
    oper = s->u.op;
    if (oper->which == e4_resultSet) {
      diagnostic_set(18,0,NULL);
      return (NULL);
    }
    else {
      /* Have to process a name or reference */
      
      AttributesPlusTerm *attrplus;
      AttributeList *l;
      AttributeElement *elem; 
      OctetString *r; 
      int use = 0, relation = 0, position = 0, structure = 0;
      int truncation = 0, completeness = 0;
      idx_list_entry *idx;
      char *search_name, *clustered_name, *cf_getclustered_name();
      char *DTD_filename, *cf_getfilename();

      attrplus = oper->u.attrTerm;

      for (l = attrplus->attributes; l ; l = l->next) {
	elem = l->item;
	if (elem->attributeType == 1) {
	  use = elem->attributeValue.u.numeric;
	  break;
	}
      }
      
      /* Should have the use attribute figured out -- now to match */
      search_name = attrplus->term->u.general->data;

      switch (use) {
      case 1023: /* "indexed-by" used to query clustered file name */
      case 5500: /* CLUSTERS */
	clustered_name = cf_getclustered_name(search_name);
	if (clustered_name == NULL) {
	  /* not a cluster file? */
	  return NULL;
	}
	return (strdup(clustered_name));
	
	break;
      case 1002: /* name of dtd or file */
      case 1017: /* default */
      case 5501: /* DTD or DTD_NAME */
	/* first try to find file name */
	DTD_filename = cf_getfilename(search_name, DTDFILE);
	if (DTD_filename != NULL) { /* got it */
	  FILE *dtdfile;
	  struct stat stat_info;
	  int bytes_read;
	  
	  if (stat(DTD_filename, &stat_info) == 0) {
	    if ((out_buffer = CALLOC(char, stat_info.st_size)) == NULL) { 
	      diagnostic_set(2,0,NULL);
	      return(NULL);
	    }
	    else { /* should be OK to read it in */
	      dtdfile = fopen(DTD_filename,"r");
	      bytes_read = fread(out_buffer, 1, stat_info.st_size, dtdfile);
	      
	      if (bytes_read != stat_info.st_size) {
		fprintf(LOGFILE, 
			"Mismatch in sizes when reading DTD in process_meta\n");
		return NULL;
	      }
	      /* force null termination */
	      out_buffer[bytes_read] = '\0';
	      fclose(dtdfile);
	      return (out_buffer);
	    }
	  }
	}
	return (NULL); 
	break;
	
      case 5502: /* Public identifier -- get the file via catalog */
	/* split the search key */
	system_id = NULL;
	temp = strchr(search_name,' ');
	if (temp) {
	  public_id = temp;
	  while (*public_id == ' ') 
	    public_id++;
	  *temp = '\0';
	  dtd = cf_getDTD(search_name);
	  if (dtd->SGML_Catalog_hash_tab) {
	    Tcl_HashEntry *entry; 
	    SGML_Catalog  *temp_SGML_Catalog_Entry;

	    entry = Tcl_FindHashEntry(
				      dtd->SGML_Catalog_hash_tab,
				      public_id);
	    if (entry != NULL){
	      temp_SGML_Catalog_Entry =
		(SGML_Catalog *) Tcl_GetHashValue(entry);
	      system_id = temp_SGML_Catalog_Entry->system_name;
	    }
	    else { /* no such item?? */
	      fprintf(LOGFILE, 
		      "No match for Public ID %s\n", public_id);
	      return NULL;
	    }
	  }
	  else { /* no catalog?? */
	    fprintf(LOGFILE, 
		    "No catalog for database %s\n", search_name);
	    return NULL;
	  }
	}
	else {
	  /* no space in key */
	    fprintf(LOGFILE, 
		    "Incorrect search key for Public ID search\n");
	  return NULL;
	}
	
	if (system_id != NULL) { /* got it */
	  FILE *pubfile;
	  struct stat stat_info;
	  int bytes_read;
	  
	  if (stat(system_id, &stat_info) == 0) {
	    if ((out_buffer = CALLOC(char, stat_info.st_size)) == NULL) { 
	      diagnostic_set(2,0,NULL);
	      return(NULL);
	    }
	    else { /* should be OK to read it in */
	      pubfile = fopen(system_id,"r");
	      bytes_read = fread(out_buffer, 1, stat_info.st_size, pubfile);
	      
	      if (bytes_read != stat_info.st_size) {
		fprintf(LOGFILE, 
			"Mismatch in sizes when reading public id data file\n");
		return(NULL);
	      }
	      /* force null termination */
	      out_buffer[bytes_read] = '\0';
	      fclose(pubfile);
	      return (out_buffer);
	    }
	  }
	}
	fprintf(LOGFILE, 
		"Error in searching public id\n");
	return (NULL); 

	break;

      case 5503: /* Number of records in database */
	maxrecs = cf_getnumdocs (search_name);
	if ((out_buffer = CALLOC(char, 10)) == NULL) { 
	  diagnostic_set(2,0,NULL);
	  return(NULL);
	}
	sprintf(out_buffer,"%d",maxrecs);
	return (out_buffer);
   
	break;

      case 5504: /* Filename -- physical file name for a document */
	temp = strrchr(search_name,' ');
	if (temp) {
	  docid = atoi(temp);
	  if (docid >= 0) {
	    *temp = '\0';
	    out_buffer = cf_getdatafilename(search_name, docid);
	    return (strdup(out_buffer));
	  }
	  else {
	    /* invalid DOCID */
	    return NULL;
	  }
	}
	else 
	  return NULL;

	break;
	

      default:
	diagnostic_set(114,use,NULL); /* invalid use attribute */
	return(NULL);
      }

    }	   
  }
  else {
    diagnostic_set(18,0,NULL);
    return(NULL); /* error: neither operand or binop?? */
  }
}


char *se_process_meta(RPNQuery *RPN_Query)
{
  ObjectIdentifier *attributeSetId;

  if (RPN_Query == NULL) 
    return (NULL);

  /* should check RPN_Query->attributeSetId to be sure it is one */
  /* we can process -- eventually --                             */
  return (se_process_meta_rpn_structure(RPN_Query->rpn));

}








