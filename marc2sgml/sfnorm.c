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
/* SFNORM.c -- Program to normalize subfield codes into SGML form.

   BY:     Jerome McDonough
   DATE:   June 6, 1994
   REV:
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <dmalloc.h>
#include "marc.h"

#define RECBUFSIZE  	 10000
#define FIELDBUFSIZE     10000
#define SGMLRECSIZE      50000
#define READONLY    	 O_RDONLY
#define READWRITE        O_RDWR
#define BADFILE	    	 -1
#define TRUE   	    	 1
#define FALSE  	    	 0
#define SUCCESS          1
#define FAILURE          0

#ifndef SEEK_CUR
#define SEEK_CUR    	 1
#endif

/* EXTERNAL VARIABLES */
extern char recbuffer[];
extern char fieldbuffer[];

char *sfnorm(char sfcode, char *sfcnorm)
{
     memset(sfcnorm, 0, 10);
     
     if ((sfcode >= 'A' && sfcode <= 'Z') || (sfcode >= 'a' && sfcode <= 'z'))
     {
	  sfcnorm[0] = sfcode;
	  return(sfcnorm);
     }
     
     else if (sfcode == '0')
     {
	  strcpy(sfcnorm, "Zero\0");
	  return(sfcnorm);
     }
     else if (sfcode == '1')
     {
	  strcpy(sfcnorm, "One\0");
	  return(sfcnorm);
     }
     else if (sfcode == '2')
     {
	  strcpy(sfcnorm, "Two\0");
	  return(sfcnorm);
     }
     else if (sfcode == '3')
     {
	  strcpy(sfcnorm, "Three\0");
	  return(sfcnorm);
     }
     else if (sfcode == '4')
     {
	  strcpy(sfcnorm, "Four\0");
	  return(sfcnorm);
     }
     else if (sfcode == '5')
     {
	  strcpy(sfcnorm, "Five\0");
	  return(sfcnorm);
     }
     else if (sfcode == '6')
     {
	  strcpy(sfcnorm, "Six\0");
	  return(sfcnorm);
     }
     else if (sfcode == '7')
     {
	  strcpy(sfcnorm, "Seven\0");
	  return(sfcnorm);
     }
     else if (sfcode == '8')
     {
	  strcpy(sfcnorm, "Eight\0");
	  return(sfcnorm);
     }
     else if (sfcode == '9')
     {
	  strcpy(sfcnorm, "Nine\0");
	  return(sfcnorm);
     }
     else
     {
	  sfcnorm[0] = sfcode;
	  return(sfcnorm);
     }
}



