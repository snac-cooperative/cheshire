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
#include <sys/types.h>
#include <regex.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
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

main()
{
  char    patt[SLENGTH], strng[SLENGTH], casestring[SLENGTH];
  char    *eol;
  int result;
  int casesensitive;

  /* printf("REG_EXTENDED = %d, REG_NEWLINE = %d, REG_ICASE = %d, REG_NOSUB = %d\n",
   *   REG_EXTENDED, REG_NEWLINE, REG_ICASE, REG_NOSUB);
   */

  printf("Enter a regular expression:"); 
  fgets(patt, SLENGTH, stdin);
  if ((eol = strchr(patt, '\n')) != NULL)
    *eol = '\0';  /* Replace newline with null */
  else
    return;  /* Line entered too long */
  printf("Enter string to compare\nString: ");
  fgets(strng, SLENGTH, stdin);
  if ((eol = strchr(strng, '\n')) != NULL)
    *eol = '\0';  /* Replace newline with null */ 
  else
    return;  /* Line entered too long */
  printf("Case Sensitive match ?[Y/N]: ");
  fgets(casestring, SLENGTH, stdin);

  if (casestring[0] == 'Y' || casestring[0] == 'y')
    casesensitive = 1;
  else
    casesensitive = 0;

  result = match(patt, strng, casesensitive);
  if (result == 0) 
    printf("Pattern does not match the string\n");
  else
    printf("Pattern matches the string\n");

}





