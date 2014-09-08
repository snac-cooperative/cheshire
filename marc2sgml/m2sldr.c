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
/* m2sldr --    function to convert a USMARC record leader (using MARC data
                structures from libmarc.a) into an SGML form of MARC
		compliant with USMARC.DTD.

     Call:      m2sldr(*MARC_LEADER_OVER, *char, int, int);
                where *MARC_LEADER_OVER is a pointer to the MARC leader, 
		*char is a pointer to a string to hold the translated
		record, the first int is a logical record number,
		and the second int is used to mark the write position
		in the *char buffer.

     Return:    SUCCESS (1) or FAILURE (0)

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
#include "marc.h"

#define RECBUFSIZE  	 10000
#define FIELDBUFSIZE     10000
#define SGMLRECSIZE      50000
#define TRUE   	    	 1
#define FALSE  	    	 0
#define SUCCESS          1
#define FAILURE          0


int m2sldr(leader, sgmlrec, recno, pos)
MARC_LEADER_OVER *leader;
char *sgmlrec;
int recno;
int *pos;
{

     char format[3] = "BK\0";      /* record format */
     int slength = 0;              /* Length of string written to sgmlrec */
     int curpos = 0;               /* position in sgmlrec in function */
     
     /* set current position in sgmlrec */
     curpos = *pos;
     
     /* Determine record format */
     switch (leader->RecType)
     {
     case 'b':
	  strcpy(format,"AM\0");
	  break;
     case 'c': case 'd': case 'i': case 'j':
	  strcpy(format, "MU\0");
	  break;
     case 'e': case 'f':
	  strcpy(format, "MP\0");
	  break;
     case 'g': case 'k': case 'o': case 'r':
	  strcpy(format, "VM\0");
	  break;
     case 'm':
	  strcpy(format, "CF\0");
	  break;
     case 'z':
          strcpy(format, "AU\0"); /* new code for authority records */
	  break;
     case 'a':
	  if (leader->BibLevel == 'b' || leader->BibLevel == 's')
	       strcpy(format, "SE\0");
	  else
	       strcpy(format, "BK\0");
	  break;
	  
     default:
	  break;
     }

     /* print the leader fields out to the sgml record */
     /* The Following sucks -- There should be a function that you 
	throw the sgmlrecord, the fieldname, the current position, and
	a pointer to field to, and it does the rest for any given field.
	I was in a hurry, OK? */
     slength = sprintf(&sgmlrec[curpos], "<USMARC Material=\"%s\" ID=\"%08d\">", format, recno);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<Leader>");
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<LRL>%05ld</LRL>", GetNum(leader->LRECL,5));
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<RecStat>%c</RecStat>", leader->RecStatus);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<RecType>%c</RecType>", leader->RecType);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<BibLevel>%c</BibLevel>", leader->BibLevel);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<UCP></UCP>");
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<IndCount>%c</IndCount>", leader->IndCount);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<SFCount>%c</SFCount>", leader->SubFCount);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<BaseAddr>%05ld</BaseAddr>", GetNum(leader->BaseAddr,5));
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<EncLevel>%c</EncLevel>", leader->EncLevel);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<DscCatFm>%c</DscCatFm>", leader->DesCatForm);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<LinkRec>%c</LinkRec>", leader->blank2);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<EntryMap><FLength>%c</FLength>", leader->LenLenF);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<SCharPos>%c</SCharPos>", leader->LenStartF);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<IDLength>%c</IDLength>", leader->UnDef[0]);
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "<EMUCP></EMUCP></EntryMap>");
     curpos = curpos + slength;
     slength = sprintf(&sgmlrec[curpos], "</Leader>");
     curpos = curpos + slength;
     
     /* set the write position indicator for sgmlrec to the current position */
     *pos = curpos;
     
     if (*pos > SGMLRECSIZE)
	  return (FAILURE);
     else
	  return (SUCCESS);
     
}

