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
*       Header Name:    testpars.c
*
*       Programmer:     Ray Larson 
*
*       Date:           11/7/93
*       Revised:        11/28/93
*       Version:        0.99999999
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#define MAIN

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "session.h"

FILE *LOGFILE;

RPNQuery *queryparse(char *query, char *attributeSetId);

ZSESSION *TKZ_User_Session = NULL;

extern char *LastResultSetID;

main (argc, argv)
int argc;
char *argv[];
{
  RPNQuery *RPN_Query_ptr;
  char *attrsetid;
  
  char querystring[5000]; /* XXXX for simulated output from driver */
  char *newline;

  int i;
  
  LOGFILE = stderr;

  querystring[0] = '\0';

  if (argc > 1) {

    attrsetid = argv[1];

    for (i=2; i<argc; i++) {
      strcat(querystring,argv[i]);
      strcat(querystring," ");
    }
  
    printf("Attribute set: %s, Query: %s\n", attrsetid, querystring);
  }
  else {
    attrsetid = "1.2.840.10003.3.1"; /* bib-1 oid */
    /* attrsetid = "1.2.840.10003.3.2";*/  /* explain oid */
    printf("Query-> ");
    fgets(querystring, 4999, stdin); /* changed from gets */
    /* which means we have to manually remove the f**king newline! */
    newline = strchr(querystring,'\n');
    if (newline) 
      *newline = '\0';

  }
  
  while (querystring[0] != 0) {
    
    printf("The Query is : '%s'\n", querystring);
    
    
    RPN_Query_ptr = queryparse(querystring, attrsetid);
    
    clean_up_query_parse();
    
    
    if (RPN_Query_ptr != NULL) {
      printf("\n");
      dump_rpn_query(RPN_Query_ptr);
      printf("\n");
      if (LastResultSetID)
	printf("ResultSetID = '%s'\n", LastResultSetID);
    }
    
    FreeRPNQuery(RPN_Query_ptr);
    if (argc == 1) {
      printf("Query-> ");
      fgets(querystring, 4999, stdin);
      /* which means we have to manually remove the f**king newline! */
      newline = strchr(querystring,'\n');
      if (newline) 
	*newline = '\0';     
      /*gets(querystring);*/
    }
    else querystring[0] = '\0';
  } 
}



