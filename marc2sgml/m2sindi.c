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
/* m2sindi --   function to produce a string containing SGML form of
                USMARC data field indicators.

     Call:      m2sindi(*char, *MARC_FIELD);
                where *char is a string of sufficient length to hold
		resulting SGML form of indicators (no checks here --
		haaah-hah-hah-hah!!!!) and MARC_FIELD is the data
		field in question.

     Return:    SUCCESS if fld found in table below, otherwise FAILURE

     By:        Jerome McDonough
     Date:      June 28, 1994
     Rev:       May 12, 1995 for format integration update
*/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <dmalloc.h>
#include "marc.h"
#include "indicator.h"

#define RECBUFSIZE  	 10000
#define FIELDBUFSIZE     10000
#define SGMLRECSIZE      50000
#define TRUE   	    	 1
#define FALSE  	    	 0
#define SUCCESS          1
#define FAILURE          0

/* External Variables */
extern char recbuffer[];
extern char fieldbuffer[];




m2sindi(indstring, fld)
char *indstring;
MARC_FIELD *fld;
{
     int i = 0, j = 0, k = 0;
     char i1str[2];
     char i2str[2];

     memset (indstring, 0, 125);
     memset (i1str, 0, 2);
     memset (i2str, 0, 2);

     /* Get right structure in array */
     while ((strcmp(fld->tag, indicators[i].tag)) > 0)
	  i++;

     /* see if we've really got field, or have a problem */
     /* NULL in indicators[i].tag is end of table        */
     if (indicators[i].tag && (strcmp(fld->tag, indicators[i].tag)) == 0)
     {
     
          /* set appropriate array value for indicator value */
          if (fld->indicator1 == 32)
	       j = 10;
          else
          {
               i1str[0] = fld->indicator1;
	       j = atoi(i1str);
          }
          if (fld->indicator2 == 32)
	       k = 10;
          else
          {
               i2str[0] = fld->indicator2;
	       k = atoi(i2str);
          }     
          /* print the indicator names and values to indstring */
          sprintf(indstring, " %s=\"%s\" %s=\"%s\"", indicators[i].attrib1, indicators[i].att1vals[j], indicators[i].attrib2, indicators[i].att2vals[k]);
     
          return(SUCCESS);
     }
     else
     {
      fprintf(stderr, "M2SINDI: Field %s not in SGML conversion table.\n", fld->tag);
          return(FAILURE);
     }
}




