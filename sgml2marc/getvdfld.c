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
*	Header Name:	getvdfld.c
*
*	Programmer:	Jerome P. McDonough
*
*	Purpose:	To extract variable data fields from an SGML
*                       representation of a USMARC record
*
*	Usage:		int GetVDfld (directory, varflds, dataptr);
*
*	Variables:	char *directory -- pointer to beginning of USMARC
*                                          directory info being compiled
*                       char *varflds -- pointer to beginning of variable
*                                        field information being compiled
*                       SGML_Data *dataptr -- pointer to first of the
*                                             major subdivisions of variable
*                                             data fields in SGML MARC record,
*                                             as per USMARC DTD.
*
*	Return Conditions and Return Codes:	SUCCESS (1) or FAILURE (0)
*
*	Date:		10/31/94.  Boo.
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
#include "alpha.h"
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
extern FILE *LOGFILE;
extern int last_rec_proc;

int GetVDfld(directory, varflds, dataptr, recsize)
char *directory;
char *varflds;
SGML_Data *dataptr;
long recsize;
{
     int datalen = 0, offset = 0;
     char tag[4];
     char sbftag[2];
     char indicators[3];
     char *dirptr;
     char *vfptr;
     SGML_Data *fldptr;
     SGML_Data *sbfldptr;
     char *tempstr;
     long fldsize = 0;
     
     tempstr = (char *) malloc(recsize);
     memset(tag, 0, 4);
     memset(sbftag, 0, 2);
     
     /* set dirptr and vfptr pointing at end of existing data in each
        string */
     dirptr = directory + (strlen(directory));
     vfptr = varflds + (strlen(varflds));

     /* for each major subdivision of Variable Data fields portion of  */
     /* SGML MARC record */
     for(dataptr; dataptr != NULL; dataptr = dataptr->next_data_element)
     {
	  /* set a pointer to first field data element in subdivision */
	  fldptr = dataptr->sub_data_element;

	  /* for each field in subdivision */
	  for(fldptr; fldptr != NULL; fldptr = fldptr->next_data_element)
	  {
	       /* cleare out temporary string */
	       memset(tempstr, 0, recsize);
	       
	       /* get field tag */
	       strncpy(tag, ((*fldptr).element->tag + 3), 3);
	       
	       /* translate attributes into MARC indicator values */
	       if ( fldptr->attributes ) {
		 if ((s2mindi(tag, fldptr->attributes, indicators)) != SUCCESS)
		   {
		     fprintf(LOGFILE, "GetVDfld: MARC indicator translation failed.\n");
		     return(FAILURE);
		   }
	       }
	       /* set first characters of temporary string to indicators */
	       /* set offset in temporary string ahead 2 */
	       strncpy(tempstr, indicators, 2);
	       offset = 2;
	       
	       /* set a pointer to first subfield data element for field */
	       sbfldptr = fldptr->sub_data_element;

	       /* for each subfield in field */
	       for(sbfldptr; sbfldptr != NULL; sbfldptr = sbfldptr->next_data_element)
	       {
		    /* print subfield delimiter and subfield code */
		    sbftag[0] = SFnorm((*(*sbfldptr).element).tag);
		    sbftag[0] = tolower(sbftag[0]);
		    sprintf(&tempstr[offset], "%c%s", SFDELIM, sbftag);
		    offset = offset + 2;
		    
		    /* print subfield data */
		    datalen = sbfldptr->content_end - sbfldptr->content_start;
		    if (datalen < 0) {
		      fprintf(stderr, "Negative subfield length -- Incorrect tagging? In record #%d\n", last_rec_proc);
		      exit(1);
		    }

		    strncpy(&tempstr[offset], sbfldptr->content_start, datalen);
		    offset = offset + datalen;
		    
		    
	       }
	       /* terminate data in tempstr */
	       tempstr[offset] = FT;
	       
	       /* move tempstr into current location in varflds string,
		  add new entry to directory,
		  then advance pointers into varflds and directory to prepare
		  for next field to be written */
	       datalen = strlen(tempstr);
	       strncpy(vfptr, tempstr, datalen);
	       sprintf(dirptr, "%s%04d%05d", tag, datalen, (vfptr - varflds));
	       dirptr = dirptr + 12;
	       vfptr = vfptr + datalen;
	  }
     }
     free(tempstr);
     return(SUCCESS);
}

