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
#include <stdio.h>
#include <string.h>


/*
 * The items in data must be seperated by a blank space.
 * (e.g. "cat ten pen")
 */
char **
StringToArgv (data)
     char	*data;		/* Must be NULL-terminated string.	*/
{
  int argc;
  char **argv;
  char *cp;
  char *ep;
  char *wp;
  int start;
  int i;
  void *calloc();

  if (data == NULL)
    return (char **)NULL;
  argc = 0;
  cp = data;
  ep = data + strlen(data);
  while (cp < ep) {
    while ((cp < ep) && isspace(*cp))
      cp++;
    start = 0;
    while ((cp < ep) && !isspace(*cp)) {
      cp++;
      start = 1;
    }
    if (start)
      argc++;
  }
  
  /*
   * Extract all the items in string data.
   */
  argv = (char **)calloc(sizeof(char *), argc+1);
  argv[argc] = (char *)NULL;
  if (argc == 0)
    return argv;
  
  i = 0;
  cp = data;
  ep = data + strlen(data);
  while (cp < ep) {
    while ((cp < ep) && isspace(*cp))
      cp++;
    wp = cp;
    while ((cp < ep) && !isspace(*cp)) 
      cp++;
    if (cp > wp) {
      argv[i] = (char *)calloc(cp-wp+1, 1);
      strncpy(argv[i], wp, cp-wp);
      i++;
    }
  }
  argv[argc] = (char *)NULL;
  return argv;
}


main (int argc, char **argv)
{

  char **results;
  int i = 0;

  if (argc < 2) {
    printf ("test teststring\n");
    exit(-1);
  }
  results = StringToArgv(argv[1]);

  while (results[i] != NULL) {
    printf ("item %d: '%s'\n", i, results[i]);
    i++;
  }
}

