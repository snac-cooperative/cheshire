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
*	Header Name:	sgml_to_marc.c
*
*	Programmer:	Ray Larson
*
*	Purpose:	simplified driver to for the sgml2marc conversion 
*                       function
*
*	Usage:		out = sgml_to_marc(sgml_data)
*
*	Variables:	
*                       
*
*	Return Conditions and Return Codes:	
*                       returns NULL on conversion errors, else returns
*                       a pointer to a marc record (as a character string);
*
*	Date:		1/11/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "configfiles.h"
#include "dmalloc.h"

#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'


char *sgml_to_marc(SGML_Document *sgmlrec)
{
     char *marcrec;
     int size;

     if (sgmlrec == NULL)
       return NULL;

     size = strlen(sgmlrec->buffer);

     marcrec = CALLOC(char, size);
	
     if (marcrec == NULL) {
       fprintf(LOGFILE, "Couldn't allocate marcrec in sgml_to_marc\n");
       return(NULL);
     }

     if ((sgml2marc(sgmlrec, marcrec, size)) != SUCCEED) {
       fprintf(LOGFILE, "Conversion Failed in sgml_to_marc\n");
       return(NULL);
     }
     
     return(marcrec);
}



