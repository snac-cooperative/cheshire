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
*	Header Name:	sgml_free_dcl.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_Declaration
*                       and frees all the memory and data associated with
*                       it.
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:	        10/29/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "sgml.h"
#include "dmalloc.h"
#ifdef WIN32
#define strcasecmp _stricmp
#endif

void
free_sgml_dcl(SGML_Declaration *dcl)
{
  extern FILE *orig_sgml_in, *new_sgml_in;

  if (dcl == NULL) return;
  if (dcl->filename != NULL) {
    if (strcasecmp(dcl->filename,"DEFAULT") == 0) /* don't free static struct */
      return;

    if (strcasecmp(dcl->filename,"XML") == 0) /* don't free static struct */
      return;
  }
  if (dcl->filename) 
    FREE(dcl->filename);
  if (dcl->version_id)
    FREE(dcl->version_id);

  FREE(dcl);
 
  if (orig_sgml_in) fclose(orig_sgml_in);
  if (new_sgml_in) fclose(new_sgml_in);

}




