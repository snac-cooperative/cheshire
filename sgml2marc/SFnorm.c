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
/* SFNORM.c -- Program to normalize SGML-MARC subfield codes into USMARC form.

   BY:     Jerome McDonough
   DATE:   Jan. 23, 1997
   REV:
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

char SFnorm(sfcode)
char *sfcode;
{
     char sfcnorm = 0;
     
     if (strcmp(sfcode, "ZERO") == 0)
     {
	  return('0');
     }
     else if (strcmp(sfcode, "ONE") == 0)
     {
	  return('1');
     }
     else if (strcmp(sfcode, "TWO") == 0)
     {
	  return('2');
     }
     else if (strcmp(sfcode, "THREE") == 0)
     {
	  return('3');
     }
     else if (strcmp(sfcode, "FOUR") == 0)
     {
	  return('4');
     }
     else if (strcmp(sfcode, "FIVE") == 0)
     {
	  return('5');
     }
     else if (strcmp(sfcode, "SIX") == 0)
     {
	  return('6');
     }
     else if (strcmp(sfcode, "SEVEN") == 0)
     {
	  return('7');
     }
     else if (strcmp(sfcode, "EIGHT") == 0)
     {
	  return('8');
     }
     else if (strcmp(sfcode, "NINE")  == 0)
     {
	  return('9');
     }
     else
     {
	  sfcnorm = sfcode[0];
	  return(sfcnorm);
     }
}



