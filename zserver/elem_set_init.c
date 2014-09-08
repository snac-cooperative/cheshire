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
#include <stdlib.h>
#include <stdio.h>
#include "z3950_3.h"
#include "session.h"
#include "z3950_intern.h"
#include "cheshire.h"

#ifdef WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

extern FILE *LOGFILE;

display_format *cf_getdisplay(char *filename, char *oid, char *format);
display_format *cf_getdefaultdisplay(char *filename);


char *
DefaultElementSetName(char *databaseName) 
{
  display_format *df;

  df = cf_getdefaultdisplay(databaseName);
  if (df != NULL)
    return df->name;
  else
    return "F";

}

int
SearchElementSetNames (char *databaseName, char *elemSetName, int type)
{
  /* type = 0 for smallelementsetnames and 1 for medium */

  display_format *df;

  df = cf_getdisplay(databaseName, NULL, elemSetName);

  if (df != NULL)
    return 1;
  else if (strcasecmp(elemSetName, "F") == 0 
	   || strcasecmp(elemSetName, "B") == 0
	   || strncasecmp(elemSetName,"STRING_SEGMENT_", 15) == 0)
    return 1;
  else
    return 0;
}


