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
/* marc2sgml -- function to convert a USMARC record (using MARC data
                structures from libmarc.a) into an SGML form of MARC
		compliant with USMARC.DTD.

     Call:      marc2sgml(*MARC_REC, *char, int);
                where *MARC_REC is a pointer to the MARC record, 
		*char is a pointer to a string to hold the translated
		record, and the int is a logical record number.

     Return:    SUCCESS (1) or FAILURE (0)

     Features:  Function assumes you've already malloc'ed a space
                for your *char sufficient to hold converted MARC record.

     By:        Jerome McDonough
     Date:      May 19, 1994
     Rev:
*/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <dmalloc.h>
#include "alpha.h"
#include "marc.h"

#define RECBUFSIZE  	 10000
#define FIELDBUFSIZE     10000
#define SGMLRECSIZE      50000
#define TRUE   	    	 1
#define FALSE  	    	 0
#define SUCCESS          1
#define FAILURE          0

int marc2sgml(marcrec, sgmlrec, recno)
MARC_REC *marcrec;
char *sgmlrec;
int recno;
{

     char *dir;       /* pointer to start of directory in MARC */
     int pos = 0;     /* current position to write in sgmlrec */
     char ba[6];      /* array to hold base address from marcrec */
     int baseaddr;   /* base address as int */
     int dirlen = 0; /* length of directory */

     /* Clear out the variables cause we're good little programmers */
     memset (sgmlrec, 0, SGMLRECSIZE);
     memset (ba, 0, 6);
     
     /* Convert MARC Leader to SGML form */
     if (m2sldr(marcrec->leader, sgmlrec, recno, &pos) != SUCCESS)
     {
	  fprintf(stderr, "MARC2SGML: error processing record %08d.\n", recno);
	  return(FAILURE);
     }

     /* Convert MARC Directory to SGML form */
     strcpy(&sgmlrec[pos], "<Directry>");
     pos = pos + 10;
     dir = marcrec->record + sizeof(MARC_LEADER_OVER);
     strncpy (ba, &marcrec->record[12], 5);
     baseaddr = atoi(ba);
     dirlen = baseaddr - (sizeof(MARC_LEADER_OVER) + 1);
     memcpy(&sgmlrec[pos], dir, dirlen);
     pos = pos + dirlen;
     strcpy(&sgmlrec[pos], "</Directry>");
     pos = pos + 11;

     /* Convert MARC Variable Fields to SGML Form.  Hah! */
     if (m2sflds(marcrec, sgmlrec, &pos) != SUCCESS)
     {
	  fprintf(stderr, "MARC2SGML: error processing record %08d.\n", recno);
	  return(FAILURE);
     }
     
     return (SUCCESS);
     
}


