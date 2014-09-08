/*
 *  Copyright (c) 1990-2000 [see Other Notes, below]. The Regents of the
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
*	Header Name:	stagcmp.c
*
*	Programmer:	Ray R. Larson, Jerome McDonough
*
*	Purpose:	to compare a index key tag to an SGML_Data
*                       start tag, using wildcards
*
*	Usage:		stagcmp(tag, start_tag);
*
*	Variables:	char *tag (tag we're looking for -- may
*                                  contain wildcard character, 'X')
*                       char *start_tag (tag for element in SGML_Data struct)
*
*	Return Conditions and Return Codes:	 0 for not match
*                                               non 0 for match
*
*	Date:		07/18/94
*	Revised:	12/05/2000
*	Version:	2.0
*	Copyright (c) 1994-2000.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <regex.h>
int  match(char *pattern, char *string, int casesensitive);


int
stagcmp(char *pattag, char *comptag, int casesensitive)
{
  int i = 0;
  int length = 0;
  char ctag[1000];
  int eolflag = 0;
  
  if (pattag == NULL || comptag == NULL) 
    return(0); /* no possible match */

  memset(ctag, 0, 100);

  strncpy(ctag,comptag+1,strcspn(comptag+1," >"));

  return(match(pattag,ctag, casesensitive));
} 


#define SLENGTH 100

int    match(char *pattern, char *string, int casesensitive)
{
  char    message[SLENGTH];
  int    error, msize;
  regex_t preg;
  int    flags;
  

  /* The REG_ICASE and REG_NEWLINE flags are different from the library */
  /* values apparently -- so we use ALL flags just in case              */
  if (casesensitive) 
    flags = REG_NOSUB|REG_EXTENDED|REG_NEWLINE;
  else
    flags = REG_NOSUB|REG_ICASE|REG_EXTENDED|REG_NEWLINE;

  error = regcomp(&preg, pattern, flags);

  if (error) { 
    msize = regerror(error, &preg, message, SLENGTH);
    regfree(&preg); 
    return(0);
  } 
  /* do the match */
  error = regexec(&preg, string, (size_t) 0, NULL, 0); 

  regfree(&preg); 
  if (error == REG_NOMATCH) {
    /* printf("No matches in string\n"); */
    return(0);
  }
  else {
    return(1);
  }
}









