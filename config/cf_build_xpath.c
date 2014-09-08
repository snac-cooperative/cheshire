/*
 *  Copyright (c) 1990-2003 [see Other Notes, below]. The Regents of the
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
*	Header Name:	cf_build_xpath_key.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Construct an FTAG structure from an XPATH
*                       
*	Usage:		
*                       
*	Variables:	
*
*	Return Conditions and Return Codes:	
*                       
*
*	Date:		03/10/2003
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2003.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
/* This is copied from displayrec.c and renamed for use in configfiles */
#include "cheshire.h"

idx_key *
cf_build_xpath_key(char *element_name) 
{
  idx_key *k, *firstkey, *lastkey, *valkey;
  char *work, *start;

  work = strdup(element_name);
  /* sometimes tcl is too helpful...*/
  if (*work == '{')
    work++;

  start = strtok(work, "/[");
  k = firstkey = lastkey = NULL;

  do {
    if (strchr(start,']') != NULL) {
      /* an occurrence indicator */
      if (lastkey == NULL) {
	/* error -- occurrence without a tag */
	fprintf (LOGFILE,"Error in XPATH specification\n");
	diagnostic_set(227,0, "Error in XPATH specification for display -- Occurrence indicator with no tag?");
	return NULL;
      }
      lastkey->occurrence = atoi(start);
    }
    else {
      /* another step in the path */
      k = CALLOC(idx_key, 1);
      k->key = strdup(start);
      if (firstkey == NULL) {
	firstkey = k;
	lastkey = k;
      }
      else {
	lastkey->subkey = k;
	lastkey = k;
      }
    }
  } while (start = strtok(NULL, "/["));


  /* now step through and handle attribute specs */
  for (k = firstkey; k ; k = k->subkey) {
    if ((start = strchr(k->key,'@')) != NULL) {
      start++;
      k->key = strdup(start);
      k->attribute_flag = 1;
      if ((start = strchr(k->key,'=')) != NULL) {
	*start++ = '\0';
	while (*start == '"' || *start == '\'') 
	  start++;
	valkey = CALLOC(idx_key, 1);
	valkey->attribute_flag = 2;
	valkey->key = strdup(start);
	if (k->subkey != NULL)
	  valkey->subkey = k->subkey;
	k->subkey = valkey;
	/* terminate it */
        if ((work = strchr(start,'"')) || (work = strchr(start,'\''))) {
	  *work = '\0';
	}
      }
    } 
  }

  if ((start = strchr(lastkey->key,'}')) != NULL) {
    *start = '\0';
  }

  return(firstkey);

}

