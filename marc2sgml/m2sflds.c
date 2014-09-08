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
/* m2sflds --   function to convert a USMARC variable fields (using MARC data
   structures from libmarc.a) into an SGML form of MARC
   compliant with USMARC.DTD.
   
   Call:      m2sflds(*MARC_REC, *char, int);
   where *MARC_REC is a pointer to the MARC record, 
   *char is a pointer to a string to hold the translated
   record, and the int is used to mark the write position
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

/* External Variables */
extern char recbuffer[];
extern char fieldbuffer[];

/* Function Prototypes */
MARC_FIELD *GetField();
char *sfnorm(char, char *);


int m2sflds(marcrec, sgmlrec, pos)
MARC_REC *marcrec;
char *sgmlrec;
int *pos;
{
  int slength = 0;            /* Length of last write to sgmlrec */
  int curpos = 0;             /* current position in sgmlrec */
  MARC_FIELD *fld;            /* MARC field */
  MARC_FIELD *save_fld;       /* MARC field */
  MARC_SUBFIELD *subf;        /* MARC Subfield */
  char *subfcode;             /* pointer into subfield codes list */
  char subfcstr[10];          /* string for normalized subfield */
  char indstring[125] = "\0"; /* string containing complete SGML form
				 of indicator data for dumping to SGML
				 field tag */
  int rec_end = 0;            /* Have we hit end of record yet? */
  int j = 0;
  int authority_rec = 0;      /* flag for authority records */
  int fcount;
  char temp008buff[100];
  
  /* set current position */
  curpos = *pos;
  
  if (marcrec->leader->RecType == 'z')
    authority_rec = 1;

  
  /* Write out Variable Flds tag, followed by Variable Control Fields */
  curpos += sprintf(&sgmlrec[curpos], "<VarFlds><VarCFlds>");

  fcount = marcrec->nfields;

  for (fld = (MARC_FIELD *)GetField(marcrec, NULL, fieldbuffer, "xxx"); fcount > 0 && (j = GetNum(fld->tag, 3)) <= 9 || j == 39 ; fld = (MARC_FIELD *)GetField(NULL, fld->next, fieldbuffer, "xxx"), fcount--)
    {
      if ((EntSub(fieldbuffer)) != SUCCESS)
	{
	  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	}
      curpos += sprintf(&sgmlrec[curpos], "<Fld%s>%s</Fld%s>", fld->tag, fieldbuffer, fld->tag);
      
    }
  curpos += sprintf(&sgmlrec[curpos], "</VarCFlds>");
  
  if (fcount <= 0) {
    fprintf (stderr, "M2SFLDS: Only fixed fields in record NO DATA.\n");
    return(FAILURE);
  }
  
  /* Write out variable data fields' and the numbers & codes
     tag and first data field from the previous loop */
  curpos += sprintf(&sgmlrec[curpos], "<VarDFlds><NumbCode>");
  
  
  /* Transform indicator values into SGML attributes */
  if ((j = GetNum(fld->tag, 3)) < 99) {
    if ((m2sindi(indstring, fld)) == FAILURE) {
      fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
      fprintf(stderr, "Not translating Field: %s\n", fld->tag);
      fprintf(stderr, "for MARC Record %s\n", sgmlrec);
    }
    else 
      {
	curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
      
      
	for (subf = fld->subfield; subf != NULL; subf = subf->next)
	  {
	    /* XXX -- next line new.  Delete? */
	    subfcopy(fieldbuffer, subf->data, 0);
	    if ((EntSub(fieldbuffer)) != SUCCESS)
	      {
		fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	      }
	    curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	    
	  }
	curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	
      }
  
    /* we should still be in Numbers & Codes section, so loop
       to finish it off */
    if (fld->next != NULL)
      {
	if (m2sdflds(&fld, sgmlrec, &curpos, 99) != SUCCESS)
	  {
	    fprintf (stderr, "M2SFLDS: failure in NumbCodes translation.\n");
	    return(FAILURE);
	  }
      } else 
	{
	  rec_end = 1;
	}
  }
  
  curpos += sprintf(&sgmlrec[curpos], "</NumbCode>");
  
  
  /* ...So much for Numbers & Codes.  Now write Main Entries */
  curpos += sprintf(&sgmlrec[curpos], "<MainEnty>");
  
  
  if (((j = GetNum(fld->tag, 3)) <= 199) && (rec_end != 1)) {
      
    /* Transform indicators to SGML attributes */
    if ((m2sindi(indstring, fld)) == FAILURE) {
      fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
      fprintf(stderr, "M2SFLDS: not translating Field: %s\n", fld->tag);
      fprintf(stderr, "for MARC Record %s\n", sgmlrec);
      
    }
    else {
      curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
      /* damn misplaced 008 fields who did this crap!!! */
      if (j == 8) {
	strncpy(temp008buff, fld->data, fld->length);
	curpos += sprintf(&sgmlrec[curpos], "%s", temp008buff);
      }
      else {
	
	subfcode = &fld->subfcodes[1];
	subf = fld->subfield;
	
	if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)  {
	  fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	  return(FAILURE);
	}
	if ((EntSub(fieldbuffer)) != SUCCESS) {
	  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	}
	  
	curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	for (subf = subf->next; subf != NULL; subf = subf->next) {
	  subfcopy(fieldbuffer, subf->data, 0);
	  if ((EntSub(fieldbuffer)) != SUCCESS) {
	    fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	  }
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	}
	  
	  
      }

      curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	  
      if (fld->next != NULL) {
	if (m2sdflds(&fld, sgmlrec, &curpos, 199) != SUCCESS) {
	  fprintf (stderr, "M2SFLDS: failure in MainEnty translation.\n");
	  return(FAILURE);
	}
      } 
      else {
	rec_end = 1;
      }
    }
  }  
  curpos += sprintf(&sgmlrec[curpos], "</MainEnty>");
  
  
  /* ...So much for Main Entry.  Now write Titles */
  
  if (authority_rec == 0)
    curpos += sprintf(&sgmlrec[curpos], "<Titles>");
  
  
  if (((j = GetNum(fld->tag, 3)) <= 249) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	  
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS:subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 249) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in Titles translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
  if (authority_rec == 0)
    curpos += sprintf(&sgmlrec[curpos], "</Titles>");
  
  
  /* ...So much for Titles.  Now write Edition Imprint info */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<EdImprnt>");
   else
     curpos += sprintf(&sgmlrec[curpos], "<AuthCRef>");
  
  
  if (((j = GetNum(fld->tag, 3)) <= 275) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 275) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in EdImprnt translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</EdImprnt>");
  
  
  /* ...So much for Edition Imprint.  Now write Physical Description */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<PhysDesc>");
  
  if (((j = GetNum(fld->tag, 3)) <= 384) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	  
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 362) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in PhysDesc translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
  if (authority_rec == 0)
    curpos += sprintf(&sgmlrec[curpos], "</PhysDesc>");
  else
    curpos += sprintf(&sgmlrec[curpos], "</AuthCRef>");

  
  /* ...So much for Physical Description.  Now write Series statements */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<Series>");
   else 
     curpos += sprintf(&sgmlrec[curpos], "<AuthRef>");
  
  if (((j = GetNum(fld->tag, 3)) <= 490) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 490) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in Series translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</Series>");
   else 
     curpos += sprintf(&sgmlrec[curpos], "</AuthRef>");

  
  
   /* ...So much for Series.  Now write Main Notes */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<Notes>");
   else
     curpos += sprintf(&sgmlrec[curpos], "<AuthAlsoRef>");
  
  if (((j = GetNum(fld->tag, 3)) <= 599) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 599) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in Notes translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</Notes>");
   else
     curpos += sprintf(&sgmlrec[curpos], "</AuthAlsoRef>");
  
  
  /* ...So much for Notes.  Now write Subject Access */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<SubjAccs>");
   else
     curpos += sprintf(&sgmlrec[curpos], "<AuthNotes>");
  
  if (((j = GetNum(fld->tag, 3)) <= 699) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 699) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in SubjAccs translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</SubjAccs>");
   else
     curpos += sprintf(&sgmlrec[curpos], "</AuthNotes>");
  
  
  /* ...So much for Subject Access.  Now write Added Entries */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<AddEnty>");
  
  
  if (((j = GetNum(fld->tag, 3)) <= 755) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{ 
	  if (m2sdflds(&fld, sgmlrec, &curpos, 755) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in AddEnty translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</AddEnty>");
    
  /* ...So much for Added Entries.  Now write Link Entries */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<LinkEnty>");
  
  
  if ((rec_end != 1) && ((j = GetNum(fld->tag, 3)) <= 787))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 787) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in LinkEnty translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</LinkEnty>");
  
  
  /* ...So much for Link Entries.  Now write Series Added Entries */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<SAddEnty>");
  
  
  if ((rec_end != 1) && ((j = GetNum(fld->tag, 3)) <= 840))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if ((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 840) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in SAddEnty translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</SAddEnty>");
  
  
  /* ...So much for Series Added Entry.  Now write Holdings, Alt. Graph. */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<HoldAltG>");
  
  
  if ((rec_end != 1) && ((j = GetNum(fld->tag, 3)) <= 886))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)
	{
	  fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	  fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	  fprintf(stderr, "for MARC Record %s\n", sgmlrec);
	}
      else
	{
	  curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	  subfcode = &fld->subfcodes[1];
	  subf = fld->subfield;
	  
	  if((subfcopy(fieldbuffer, subf->data, 0)) == 0)
	    {
	      fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	      return(FAILURE);
	    }
	  if ((EntSub(fieldbuffer)) != SUCCESS)
	    {
	      fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 886) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in HoldAltG translation.\n");
	      return(FAILURE);
	    }
      } else 
	{
	  rec_end = 1;
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</HoldAltG>");
  
  
  /* ...So much for Holdings/Alt. Graphics.  Now write Local Holdings */
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "<Fld9XX>");
  
  
  if (((j = GetNum(fld->tag, 3)) <= 999) && (rec_end != 1))
    {
      
      /* Transform indicators to SGML attributes */
      if ((m2sindi(indstring, fld)) == FAILURE)	{
	fprintf(stderr, "M2SFLDS: Error in processing indicator.\n");
	fprintf(stderr, "M2SFLDS: Not Translating Field: %s\n", fld->tag);
	fprintf(stderr, "for MARC Record %s\n", sgmlrec);
      }
      else {
	curpos += sprintf(&sgmlrec[curpos], "<Fld%s%s>", fld->tag, indstring);
	  
	subfcode = &fld->subfcodes[1];
	subf = fld->subfield;
	
	if((subfcopy(fieldbuffer, subf->data,0)) == 0)
	  {
	    fprintf(stderr, "M2SFLDS: subfcopy failed.\n");
	    return(FAILURE);
	  }
	if ((EntSub(fieldbuffer)) != SUCCESS)
	  {
	    fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
	  }
	
	curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	  
	  
	  for (subf = subf->next; subf != NULL; subf = subf->next)
	    {
	      subfcopy(fieldbuffer, subf->data, 0);
	      if ((EntSub(fieldbuffer)) != SUCCESS)
		{
		  fprintf(stderr, "M2SFLDS: Entity Substitution Failed.\n");
		}
	      
	      curpos += sprintf(&sgmlrec[curpos], "<%s>%s</%s>", sfnorm(subf->code, subfcstr), fieldbuffer, sfnorm(subf->code, subfcstr));
	      
	    }
	  
	  curpos += sprintf(&sgmlrec[curpos], "</Fld%s>", fld->tag);
	  
	}
      
      
      if (fld->next != NULL)
	{
	  if (m2sdflds(&fld, sgmlrec, &curpos, 999) != SUCCESS)
	    {
	      fprintf (stderr, "M2SFLDS: failure in Fld9XX translation.\n");
	      return(FAILURE);
	    }
	}
    }
  
   if (authority_rec == 0)
     curpos += sprintf(&sgmlrec[curpos], "</Fld9XX>");
  
  
  
  /* INSERT DATA FIELD WRITE SECTION HERE */
  
  curpos += sprintf(&sgmlrec[curpos], "</VarDFlds></VarFlds></USMARC>");
  
  
  /* set the write position indicator for sgmlrec to current position */
  *pos = curpos;
  
  return (SUCCESS);
  
}

