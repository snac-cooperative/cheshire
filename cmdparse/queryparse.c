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
*       Header Name:    queryparse.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        parse query and build z39.50 RPN query tree
*                       uses LLgen and Flex code developed for
*                       the client side. This module provides a simple
*                       interface to the parser for client apps and
*                       for testing server-side functions.
*
*       Usage:          RPN = queryparse (char *query, char *attributeSetId)
*
*       Variables:
*
*       Return Conditions and Return Codes:
*			Returns pointer to RPN structure or NULL on error
*
*       Date:           12/1/94
*       Version:        1.0
*       Copyright (c) 1994.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/


/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"

/* the following define activates the table of index info in z_parse.h */
#define Z_PARSE_INTERFACE
#include "z_parse.h"


RPNQuery *queryparse (char *querystring, char *attr_oid) {

  extern int parse_errors;
  extern int parse_attribute_set;
  extern RPNQuery *rpn_query_ptr;
  extern char *TKZquery_buffer; /* query buffer interface  for z__parse */
  OctetString *attributeSetId;
  extern FILE *LOGFILE;
  FILE *logfile;

  if (LOGFILE != NULL)
    logfile = LOGFILE;
  else
    logfile = stderr;

  if(strlen(querystring)==0) {
    fprintf(logfile,"You must supply a valid query term(s) for queryparse");
    return(NULL);
  }

  TKZquery_buffer = querystring;

  /* set the attribute table */
  if (attr_oid == NULL)
    parse_attribute_set = BIB_1_ATTR_SET;
  else {
    if (strcmp(attr_oid, OID_BIB1) == 0)
      parse_attribute_set = BIB_1_ATTR_SET;
    else if (strcmp(attr_oid, OID_EXP1) == 0)
      parse_attribute_set = EXP_1_ATTR_SET;
    else if (strcmp(attr_oid, OID_GILS) == 0)
      parse_attribute_set = GILS_ATTR_SET;
    else if (strcmp(attr_oid, OID_GEO) == 0)
      parse_attribute_set = GEO_ATTR_SET;
    else 
      parse_attribute_set = BIB_1_ATTR_SET;
  }

  /* call the query parser */
  z__parse();

  if (parse_errors != 0) {
    fprintf(logfile,"Syntax error in query from queryparse\n");
    return(NULL);
  }
  
  /* Put the attribute set OID into the query */
  if (attr_oid == NULL || strcmp(attr_oid, "default") == 0) 
    rpn_query_ptr->attributeSet = NewOID(OID_DEFAULT);
  else
    rpn_query_ptr->attributeSet = NewOID(attr_oid);

  return (rpn_query_ptr); /* set in the parser */

}


