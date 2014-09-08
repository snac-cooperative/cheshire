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
*	Header Name:	GetLdr.c
*
*	Programmer:     Jerome McDonough
*
*	Purpose:	To convert the Leader information from an SGML
*                       MARC to a standard MARC Leader string
*
*	Usage:		GetLdr (leader, dataptr);
*
*	Variables:	char *leader -- Pointer to array of 25 chars
*                       SGML_Data *dataptr -- pointer to first SGML_Data
*                       Structure for an element of the Leader (i.e., a
*                       pointer to the Logical Record Length SGML_Data struct
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
#include <string.h>
#include "sgml.h"
#include "dmalloc.h"
 
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
extern FILE *LOGFILE; 

int GetLdr (leader, dataptr)
char *leader;
SGML_Data *dataptr;
{

     char *ldrptr;
     int fldlen = 0;
     
     ldrptr = leader;
     
     /* for all fields in leader, calculate length of field.  If zero, */
     /* put a space in leader at current position and advance position in */
     /* leader by one.  If not, copy data */
     /* from sgml field to leader, and move current position to that */
     /* immediately after data just added to leader */
     for (dataptr; dataptr != NULL; dataptr = dataptr->next_data_element)
     {
	  /* descend one level if this go 'round is for the Entry Map */
	  if ((strcmp(dataptr->element->tag, "ENTRYMAP")) == 0)
	  {
	       dataptr = dataptr->sub_data_element;
	  }
	  
	  fldlen = dataptr->content_end - dataptr->content_start;
	  if ((fldlen == 0) && ((strcmp(dataptr->element->tag, "UCP")) == 0))
	  {
	       sprintf (ldrptr, "  ");
	       ldrptr = ldrptr + 2;
	  }
	  else if ((fldlen == 0) && (strcmp(dataptr->element->tag, "EMUCP") == 0))
	  {
	       sprintf (ldrptr, " ");
	       ldrptr++;
	  }
	  else
	  {
	       strncpy(ldrptr, dataptr->content_start, fldlen);
	       ldrptr = ldrptr + fldlen;
	  }
     }
     
     /* Terminate Leader */
     *ldrptr = '\0';
     
     /* double check length of leader and bail out if wrong */
     if ((strlen(leader)) != 24)
     {
	  fprintf(LOGFILE, "GetLdr: leader translation failed.\n");
	  return(FAILURE);
     }

     return(SUCCESS);
}

