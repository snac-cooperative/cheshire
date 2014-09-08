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
/* m2sdflds --  Function to write out all of the variable data fields
                and subfields in SGML format for a specified portion
		of the MARC record.

     Call: m2sdflds(**MARC_FIELD, *char, *int, int); 

     where *MARC_FIELD is a pointer to a pointer to the current field,
     *char is a
     point to a string to hold SGML MARC record, *int is used to mark
     the write position in the *char buffer, and int is field number
     for the last field to write

     Return:    SUCCESS (1) or FAILURE (0)

     By:        Jerome McDonough
     Date:      June 8, 1994
     Rev: */

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

MARC_FIELD *GetField();
char *sfnorm(char, char *);


/* External Variables */
extern char recbuffer[];
extern char fieldbuffer[];

int m2sdflds(fld, sgmlrec, pos, lastfld)
MARC_FIELD **fld;
char *sgmlrec;
int *pos;
int lastfld;
{
  int slength = 0;            /* Length of last write to sgmlrec */
  int curpos = 0;             /* current position in sgmlrec */
  MARC_SUBFIELD *subf;        /* MARC subfield */
  MARC_FIELD *save_fld;       /* MARC field previous to the current */
                              /* -- used to avoid NULLing out fld   */
  char *subfcode;             /* pointer into subfield codes list */
  char subfcstr[11];          /* string for normalized subfield */
  char indstring[125] = "\0"; /* string containing complete SGML form
				 of indicator data for dumping to SGML
				    field tag */
  int j = 0;
  
  curpos = *pos;
 
  save_fld = *fld;  

  for (*fld = (MARC_FIELD *)GetField(NULL, (*fld)->next, fieldbuffer, "xxx"); 
       (*fld != NULL) && ((j = GetNum((*fld)->tag, 3)) <= lastfld); 
       *fld = (MARC_FIELD *)GetField(NULL, (*fld)->next, fieldbuffer, "xxx"))
    {
      
      save_fld = *fld;  
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, *fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SDFLDS: Error in processing indicator.\n");
	  fprintf(stderr, 
		  "M2SDFLDS: Not Translating Field: %s\n", (*fld)->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  
	  slength = sprintf(&sgmlrec[curpos], "<Fld%s%s>", 
			    (*fld)->tag, indstring);
	  curpos = curpos + slength;
	  
	  subfcode = &((*fld)->subfcodes[1]);
	  subf = (*fld)->subfield;


	  
	  if (subf == NULL || (subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    { 
	      if (   (*fld)->tag[0] == '0' 
		  && (*fld)->tag[1] == '0' 
		  && (*fld)->tag[2] == '8') { 
		/* special handling for missplaced 008 tags */
		strncpy(fieldbuffer, (*fld)->data, (*fld)->length);
		slength = sprintf(&sgmlrec[curpos], "%s", 
				  fieldbuffer);
		curpos = curpos + slength;
	  
		slength = sprintf(&sgmlrec[curpos], "</Fld%s>", (*fld)->tag);
		curpos = curpos + slength;
		*pos = curpos;
		return (SUCCESS);

	      }
	      else {
		fprintf(stderr, "M2SDFLDS: subfcopy failed or NULL subf pointer.\n");
		fprintf(stderr, 
			"M2SDFLDS: Not Translating Field: %s\n", (*fld)->tag);
		fprintf(stderr, "for MARC Record %s\n", sgmlrec);
		return(FAILURE);
	      }
	    }
	      
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }

	  sfnorm(subf->code, subfcstr);

	  slength = sprintf(&sgmlrec[curpos], "<%s>%s</%s>", 
			    subfcstr, fieldbuffer, subfcstr);
	  curpos = curpos + slength;
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      sfnorm(subf->code, subfcstr);
	      slength = sprintf(&sgmlrec[curpos], "<%s>%s</%s>", 
				subfcstr, fieldbuffer, subfcstr);
	      curpos = curpos + slength;
	    }
	  
	  slength = sprintf(&sgmlrec[curpos], "</Fld%s>", (*fld)->tag);
	  curpos = curpos + slength;
	}
      
    }
  if (*fld == NULL) /* set point to the saved fld */
    *fld = save_fld;
  *pos = curpos;
  return (SUCCESS);
}



