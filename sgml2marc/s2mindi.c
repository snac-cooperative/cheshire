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
*	Header Name:	s2mindi.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	To translate USMARC indicators from SGML form to
*                       single character values
*
*	Usage:		s2mindi(tag, attributes, indchars);
*
*	Variables:	char *tag -- MARC Field tag in question
*                       SGML_Attribute_Data *attributes -- pointer to
*                            SGML Attribute data structure containing info
*                            to translate
*                       char *indchars -- pointer to a 2 character array
*                            to hold translated values
*
*	Return Conditions and Return Codes:	SUCCESS (1) or FAILURE (0)
*
*	Date:		11/02/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "sgml.h"
#include "indicator.h"     /* Table of indicator values */
#include "dmalloc.h"
#ifdef WIN32
#define strcasecmp _stricmp
#endif
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
extern FILE *LOGFILE;

int s2mindi(tag, attributes, indchars)
char *tag;
SGML_Attribute_Data *attributes;
char *indchars;
{
     int i = 0, j, k;
     SGML_Attribute_Data *tempptr = attributes;
     
     /* Find matching tag in indicators array */
     while ((strncmp(tag, indicators[i].tag, 3)) > 0)
	  i++;
     
     if ((strncmp(tag, indicators[i].tag, 3)) != 0)
     {
	  fprintf(LOGFILE, "s2mindi: No indicators for tag %s.\n", tag);
	  return(FAILURE);
     }

     /* For each attribute (aka, indicator value)... */
     for (j = 0; j < 2; j++)
     {
	  /* hunt through attribute values for current position in */
	  /* indicator array for a match for attribute string value */
	  k = 0;
	  if(j == 0)
	  {
	       while ((strcasecmp(tempptr->string_val, indicators[i].att1vals[k])) != 0)
	       {
		    k++;
		    if (k > 10)
		    {
			 /* fprintf(LOGFILE, "s2mindi: couldn't find indicator match.  Substituting blank...\n"); */
			 indchars[0] = (char) 32;
			 break;
		    }
	       }
	       if (k == 10)
		    indchars[0] = (char) 32;
	       if (k < 10)
		    indchars[0] = k % 10 + '0';
	       tempptr = tempptr->next_attribute_data;
	  }
	  else
	  {
	       while ((strcasecmp(tempptr->string_val, indicators[i].att2vals[k])) != 0)
	       {
		    k++;
	           if (k > 10)
		    {
			/* fprintf(LOGFILE, "s2mindi: couldn't find indicator match.  Substituting blank...\n"); */
			 indchars[1] = (char) 32;
			 break;
		    }
	       }
	       if (k == 10)
		    indchars[1] = (char) 32;
	       if (k < 10)
		    indchars[1] = k % 10 + '0';
	  }
     }
     return(SUCCESS);
}     

