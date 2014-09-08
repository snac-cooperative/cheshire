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
*	Header Name:	stem_test.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	driver for Porter Stemmer
*
*	Usage:		stem_test 
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/31/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdio.h>
#include <string.h>
#include <strings.h>


main (int argc, char *argv[])
{

  char databuf[300];	/* string to hold query */
  char *word_ptr, *next_tok;
  int result;
  char *breakletters = " \t\n`~!@#$%^&*()_=|\\{[]};:'\",<.>?/";

  printf("Query-> ");
  fflush(stdout);
  gets(databuf);

  /* find first token */
  word_ptr = strtok_r (databuf, breakletters, &next_tok);
  /* a buffer full of blanks or punct? */
  if (word_ptr == NULL)
    return(0);

  do {
    char *tempwordptr;
    /* normalize the word according to the specs for this index */
    /* (as defined in indextype)                                */
    tempwordptr = word_ptr;

    printf ("word = %s : databuf %d, word_ptr %d, next_tok %d\n",
	    word_ptr, (int)databuf, (int)word_ptr, (int)next_tok);

    printf ("offset for %s : %d\n", word_ptr, (int)(word_ptr - databuf));
    /* get the next word from the current buffer     */
    word_ptr = strtok_r (NULL, breakletters, &next_tok);
  } while (word_ptr != NULL);  
  
}




