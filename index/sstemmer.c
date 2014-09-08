/*
 *  Copyright (c) 1990-2005 [see Other Notes, below]. The Regents of the
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
*	Header Name:	sstemmer.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Simple plural stemmer (based on the SMART version)
*
*	Usage:		sstem(word)
*
*	Variables:	char *word  -- word to be stemmed
*
*	Return Conditions and Return Codes:	Same word for no stemming
*                                               or stemmed word
*
*	Date:		04/20/2005
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2005.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <stdio.h>


char *sstem (char *word)
{
  int len = strlen (word);
    
  if (len <= 2 || word[len-1] != 's')
    /* not a plural word; return */
    return (word);
  
  /* otherwise, a candidate plural word */
  switch(word[len-2]) {
  case 'e':    /* still 2 possibilites - 'es', 'ies' */
    if (len > 3 && word[len-3] == 'i' &&
	word[len-4] != 'e' && word[len-4] != 'a' ) {
      /* 'ies' -> 'y' */
      word[len-3] = 'y';
      word[len-2] = '\0';
    }
    else if (len>3 && word[len-3] != 'a' && word[len-3] != 'e'
	     && word[len-3] != 'i' && word[len-3] != 'o')
      /* 'es' -> 'e' */
      word[len-1] = '\0';
    break;
  case 'u':    /* do not remove 's' */
  case 's':
  case '\'':
    break;
  default:     /* remove final 's' */
    word[len-1] = '\0';
  }
  return(word);         
}









