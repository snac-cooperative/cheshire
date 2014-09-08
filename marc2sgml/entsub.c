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
*	Header Name:	EntSub.c
*
*	Programmer:     Jerome P. McDonough
*
*	Purpose:	Translate some characters into SGML entity references
*                       in order to avoid interpreting data as markup
*
*	Usage:		EntSub(fieldbuffer);
*
*	Variables:	char *fieldbuffer -- point to char array of
*                                            FIELDBUFSIZE containing
*                                            MARC field information to
*                                            translate
*
*	Return Conditions and Return Codes:	SUCCESS
*
*	Date:		10/11/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdio.h>
#include <string.h>
#include <dmalloc.h>

#define FIELDBUFSIZE 50000
#define SUCCESS 1


/* External Variables */
char fieldbuffer[FIELDBUFSIZE];

int EntSub(fieldbuffer)
char *fieldbuffer;
{
     char tempbuf[FIELDBUFSIZE];
     char *tempbufptr;
     char *fldbufptr;
     
     fldbufptr = fieldbuffer;
     tempbufptr = &tempbuf[0];
     memset(tempbufptr, 0, FIELDBUFSIZE);
     
     for (;*fldbufptr != 0; fldbufptr++)
     {
	  if (*fldbufptr == '<')
	  {
	       sprintf(tempbufptr, "&lt;");
	       tempbufptr = tempbufptr + 4;
	  }
	  else
	  {
	       *tempbufptr = *fldbufptr;
	       tempbufptr++;
	  }
     }
     strcpy(fieldbuffer, tempbuf);
     return(SUCCESS);
}






