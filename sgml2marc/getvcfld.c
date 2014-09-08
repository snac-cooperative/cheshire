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
*	Header Name:	GetVCfld.c
*
*	Programmer:     Jerome McDonough
*
*	Purpose:	To copy variable control fields from an SGML_Data
*                       structure linked list into a large string,
*                       simultaneously creating a series of MARC directory
*                       entries in a second string.
*
*	Usage:		GetVCfld(directory, varflds, dataptr);
*
*	Variables:	char *directory -- pointer to string to hold
*                            MARC directory information
*                       char *varflds -- pointer to string to hold 
*                            variable control field information
*                       SGML_Data *dataptr -- pointer to first variable
*                            control field SGML_Data structure
*
*	Return Conditions and Return Codes:	SUCCESS (1) or FAILURE (0)
*
*	Date:		10/28/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#ifdef WIN32
#include <stdlib.h>

#endif
#include <string.h>
#include "sgml.h"
#include "dmalloc.h"
 
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
 
 
int GetVCfld(directory, varflds, dataptr)
char *directory;
char *varflds;
SGML_Data *dataptr;
{

     int datalen = 0;
     char tag[4];
     char *dirptr;
     char *vfptr;
     
     memset(tag, 0, 4);
     dirptr = directory;
     vfptr = varflds;
     
     /* for each variable control field, copy numeric tag sequence from */
     /* complete tag field and calculate field length, print directory */
     /* entry, print field info into varfields string, then increment*/
     /* pointers into directory and varfields string for next loop */
     for (dataptr; dataptr != NULL; dataptr = dataptr->next_data_element)
     {
	  strncpy(tag, ((*dataptr).element->tag + 3), 3);
	  datalen = dataptr->content_end - dataptr->content_start;
	  
	  sprintf(dirptr, "%s%04d%05d", tag, (datalen + 1), (vfptr - varflds));
 
	  strncpy(vfptr, dataptr->content_start, datalen);

	  dirptr = dirptr + 12;
	  vfptr = vfptr + datalen;
	  sprintf(vfptr, "%c", FT);
	  vfptr++;
     }
     
     return(SUCCESS);
}

