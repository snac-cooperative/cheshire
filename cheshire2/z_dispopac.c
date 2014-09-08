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
/*
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**************************************************************************/
/* DispOPAC - print marc OPAC format records (with holdings from a file   */
/* or buffer                                                              */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <tcl.h>
#include <string.h>

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#include "z3950_3.h"
#include "zprimitive.h"
#include "z_parameters.h"
#include "marc.h"
#include "dispmarc.h"
 


#define MENUHT 3

#define   RECBUFSIZE     20000
#define   FIELDBUFSIZE   20000
#define   READONLY       O_RDONLY
#define   BADFILE        -1
#define   TRUE           1
#define   FALSE          0

#ifndef   SEEK_CUR
#define   SEEK_CUR       1
#endif

MARC_REC *FormatMARCrec(MARC_REC *rec, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)());

char *xml_sub_chars(char *fieldbuffer);

/* EXTERNAL VARIABLES */
extern char recbuffer[];
extern char fieldbuffer[];
extern char linebuffer[];

MARC_REC *DispOPACbuff(Tcl_Interp *interp, char *recbuffer, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)())
{
  MARC_REC *rec;
  int lrecl, GetNum();
  int i, first, reclistArgc, elementArgc;
  char **reclistArgv, **elementArgv;
  char *fmt_a, *fmt_b, *fmt_c, *fmt_d, *fmt_e, *fmtend;
  
  if (recbuffer == NULL || recbuffer[0] == '\0')
    return NULL;
  
  if (format == NULL)
    return NULL;
  
  if (Tcl_SplitList(interp, recbuffer, &reclistArgc, &reclistArgv) != TCL_OK)
    return NULL;
  
  if (reclistArgc > 1) {
    
    if (Tcl_SplitList(interp, reclistArgv[0], &elementArgc, &elementArgv) 
	== TCL_OK) {
      lrecl = GetNum(elementArgv[1],5);
      
      if((rec = GetMARC(elementArgv[1],lrecl,0)) == NULL) {
	/* fprintf(stderr,"no record retrieved by GetMARC\n"); */
	Tcl_AppendResult(interp, "No record retrieved by GetMARC ",
			 "while attempting to format OPAC record", 
			 (char *) NULL);
	return NULL;
      }
      /* Do the MARC formatting */
      FormatMARCrec(rec, format, displaynum, maxlen, output);
    }
    else {
      Tcl_AppendResult(interp, "Unable to parse elements of OPAC record", 
		       (char *) NULL);
      return NULL;
    }
    
    /* now handle the holdings information (call numbers only) */
    Tcl_Free((char *)elementArgv);
    
    if (format == htmlformat ||
	format == shorthtmlformat ||
	format == reviewhtmlformat) 
      {
	fmt_a = "<DL><DT><EM>Local Call Numbers:</EM><DD><STRONG>%s ";
	fmt_b = "<DD><STRONG>%s ";
	fmt_c = "<DD>%s ";
	fmt_d = "</STRONG>";
	fmt_e = "";
	fmtend = "</DL>";
      }
    else if (format == dcformat ||
	     format == shortdcformat) 
      {
	fmt_a = "<dc:Identifier>%s ";
	fmt_b = "</dc:Identifier><dc:Identifier>%s ";
	fmt_c = "%s ";
	fmt_d = " ";
	fmt_e = " ";
	fmtend = "</dc:Identifier>";
      }
      else
      {
	fmt_a = "Call Numbers:  %s ";
	fmt_b = "               %s ";
	fmt_c = "                    %s ";
	fmt_d = "\n";
	fmt_e = "\n";
	fmtend = "";
      }
    
    
    first = 1;
    
    for (i = 1; i < reclistArgc; i++) {
 
      /* process each piece of holdings data */
      if (Tcl_SplitList(interp, reclistArgv[i], &elementArgc, &elementArgv) 
	  == TCL_OK) {
	
	if (strcasecmp(elementArgv[0], "localLocation") == 0) {
	  if (first) {
	  if (format == dcformat ||
	      format == shortdcformat)
	    sprintf(linebuffer, fmt_a , xml_sub_chars(elementArgv[1]));
	  else
	  if (format == dcformat ||
	      format == shortdcformat)
	    sprintf(linebuffer, fmt_a , xml_sub_chars(elementArgv[1]));
	  else
	    sprintf(linebuffer, fmt_a , elementArgv[1]);
	    first = 0;
	  }
	  else 
	  if (format == dcformat ||
	      format == shortdcformat)
	    sprintf(linebuffer, fmt_b , xml_sub_chars(elementArgv[1]));
	  else
	    sprintf(linebuffer, fmt_b, elementArgv[1]);
	}
	
	if (strcasecmp(elementArgv[0], "shelvingLocation") == 0) {
	  if (format == dcformat ||
	      format == shortdcformat)
	    strcat (linebuffer, xml_sub_chars(elementArgv[1]));
	  else
	    strcat (linebuffer, elementArgv[1]);
	  strcat (linebuffer, " ");
	}
	
	if (strcasecmp(elementArgv[0], "callNumber") == 0) {
	  if (format == dcformat ||
	      format == shortdcformat)
	    strcat(linebuffer, xml_sub_chars(elementArgv[1]));
	  else
	    strcat (linebuffer, elementArgv[1]);
	  strcat (linebuffer, fmt_d);
	  outputline (output, linebuffer, maxlen, 0);  
	  
	}

	if (strcasecmp(elementArgv[0], "enumAndChron") == 0) {
	  if (format == dcformat ||
	      format == shortdcformat)
	    sprintf(linebuffer, fmt_c , xml_sub_chars(elementArgv[1]));
	  else
	    sprintf(linebuffer, fmt_c, elementArgv[1]);
	  strcat (linebuffer, fmt_e);
	  outputline (output, linebuffer, maxlen, 0);  
	}

	if (strcasecmp(elementArgv[0], "publicNote") == 0) {
	  if (format == dcformat ||
	      format == shortdcformat)
	    sprintf(linebuffer, fmt_c , xml_sub_chars(elementArgv[1]));
	  else
	    sprintf(linebuffer, fmt_c, elementArgv[1]);
	  strcat (linebuffer, fmt_e);
	  outputline (output, linebuffer, maxlen, 0);  
	}

	Tcl_Free ((char *)elementArgv);
      }
    }

    strcpy (linebuffer, fmtend);
    outputline (output, linebuffer, maxlen, 0);

    Tcl_Free((char *)reclistArgv);
  
    return (rec); /* send back the marc record pointer */
  }
  else {
    Tcl_AppendResult(interp, "No holdings info in OPAC record", 
		     (char *) NULL); 
    return (NULL);
  }
}


/* OPAC Record utilities for MELVYL records */
/* these are based on the record utilities  */
/* in the New V3 Z39.50 library             */

int
calcOPACsize(OPACRecord *opacRecord)
{
  struct holdingsData_List87 *tmp;
  struct volumes_List88	*vol;
  Volume *v;
  struct circulationData_List89	*circ;
  CircRecord *c;
  HoldingsAndCircData *d;
  int totalsize = 0;
  int formatting = 5;
  int namesize = 18;

  if (opacRecord == NULL)
    return (0);

  totalsize = formatting + namesize;

  if (opacRecord->bibliographicRecord != NULL) {
    totalsize += opacRecord->bibliographicRecord->u.octetAligned->length;
  }

  for (tmp = opacRecord->holdingsData; tmp!=NULL; tmp = tmp->next) {
    if (tmp->item->which == e45_marcHoldingsRecord) {
      totalsize += namesize + formatting + 
	tmp->item->u.marcHoldingsRecord->u.octetAligned->length;
    }
    else { /* not marc holdings */
      d = tmp->item->u.holdingsAndCirc;
      if (d->typeOfRecord)
	totalsize += namesize + formatting + d->typeOfRecord->length;
      if (d->encodingLevel)
	totalsize += namesize + formatting + d->encodingLevel->length;
      if (d->format)
	totalsize += namesize + formatting + d->format->length;
      if (d->receiptAcqStatus)
	totalsize += namesize + formatting + d->receiptAcqStatus->length;
      if (d->generalRetention)
	totalsize += namesize + formatting + d->generalRetention->length;
      if (d->completeness)
	totalsize += namesize + formatting + d->completeness->length;
      if (d->dateOfReport)
	totalsize += namesize + formatting + d->dateOfReport->length;
      if (d->nucCode)
	totalsize += namesize + formatting + d->nucCode->length;
      if (d->localLocation)
	totalsize += namesize + formatting + d->localLocation->length;
      if (d->shelvingLocation)
	totalsize += namesize + formatting + d->shelvingLocation->length;
      if (d->callNumber)
	totalsize += namesize + formatting + d->callNumber->length;
      if (d->shelvingData)
	totalsize += namesize + formatting + d->shelvingData->length;
      if (d->copyNumber)
	totalsize += namesize + formatting + d->copyNumber->length;
      if (d->publicNote)
	totalsize += namesize + formatting + d->publicNote->length;
      if (d->reproductionNote)
	totalsize += namesize + formatting + d->reproductionNote->length;
      if (d->termsUseRepro)
	totalsize += namesize + formatting + d->termsUseRepro->length;
      if (d->enumAndChron)
	totalsize += namesize + formatting + d->enumAndChron->length;

      for (vol = d->volumes; vol ; vol = vol->next) {
	v = vol->item;
	if (v->enumeration)
	  totalsize += namesize + formatting + v->enumeration->length;
	if (v->chronology)
	  totalsize += namesize + formatting + v->chronology->length;
	if (v->enumAndChron)
	  totalsize += namesize + formatting + v->enumAndChron->length;
      }
      
      for (circ = d->circulationData; circ ; circ = circ->next) {
	c = circ->item;
	/* for availablenow, renewable, and onHold  booleans */
	totalsize += namesize + formatting;
	totalsize += namesize + formatting;
	totalsize += namesize + formatting;
	if (c->availablityDate)
	  totalsize += namesize + formatting + c->availablityDate->length;
	if (c->availableThru)
	  totalsize += namesize + formatting + c->availableThru->length;
	if (c->restrictions)
	  totalsize += namesize + formatting + c->restrictions->length;
	if (c->itemId)
	  totalsize += namesize + formatting + c->itemId->length;
	if (c->enumAndChron)
	  totalsize += namesize + formatting + c->enumAndChron->length;
	if (c->midspine)
	  totalsize += namesize + formatting + c->midspine->length;
	if (c->temporaryLocation)
	  totalsize += namesize + formatting + c->temporaryLocation->length;

      }
    }
  }
  
  return (totalsize);
}

extern char *
MakeOPACListElement(OPACRecord *opacRecord)
{
  struct holdingsData_List87 *tmp;
  struct volumes_List88	*vol;
  Volume *v;
  struct circulationData_List89	*circ;
  CircRecord *c;
  HoldingsAndCircData *d;
  char *clipMARCdata();
  char *workbuffer;
  char *buf;
  int buffersize;

  if (opacRecord == NULL)
    return NULL;

  buffersize = calcOPACsize(opacRecord);
  
  workbuffer = (char *)calloc(1, buffersize);

  if (opacRecord->bibliographicRecord != NULL) {
    buf = clipMARCdata(opacRecord->bibliographicRecord->u.octetAligned->data);
    sprintf (workbuffer, "{BIBRECORD {%s}}", buf);
  }
  

  for (tmp = opacRecord->holdingsData; tmp!=NULL; tmp = tmp->next) {
    if (tmp->item->which == e45_marcHoldingsRecord) {
      strcat (workbuffer, " {MARCHOLDINGS {");
      strcat (workbuffer, 
	      tmp->item->u.marcHoldingsRecord->u.octetAligned->data);
      strcat (workbuffer, "}}");
  
    
    }
    else { /* not marc holdings */
      d = tmp->item->u.holdingsAndCirc;
      if (d->typeOfRecord) {
	strcat (workbuffer, " {typeOfRecord {");
	strcat (workbuffer, 
		d->typeOfRecord->data);
	strcat (workbuffer, "}}");
      }
      if (d->encodingLevel) {
	strcat (workbuffer, " {encodingLevel {");
	strcat (workbuffer, 
		d->encodingLevel->data);
	strcat (workbuffer, "}}");
      }
      if (d->format) {
	strcat (workbuffer, " {format {");
	strcat (workbuffer, 
		d->format->data);
	strcat (workbuffer, "}}");
      }
      if (d->receiptAcqStatus) {
	strcat (workbuffer, " {receiptAcqStatus {");
	strcat (workbuffer, 
		d->receiptAcqStatus->data);
	strcat (workbuffer, "}}");
      }
      if (d->generalRetention) {
	strcat (workbuffer, " {generalRetention {");
	strcat (workbuffer, 
		d->generalRetention->data);
	strcat (workbuffer, "}}");
      }
      if (d->completeness) {
	strcat (workbuffer, " {completeness {");
	strcat (workbuffer, 
		d->completeness->data);
	strcat (workbuffer, "}}");
      }
      if (d->dateOfReport) {
	strcat (workbuffer, " {dateOfReport {");
	strcat (workbuffer, 
		d->dateOfReport->data);
	strcat (workbuffer, "}}");
      }
      if (d->nucCode) {
	strcat (workbuffer, " {nucCode {");
	strcat (workbuffer, 
		d->nucCode->data);
	strcat (workbuffer, "}}");
      }
      if (d->localLocation) {
	strcat (workbuffer, " {localLocation {");
	strcat (workbuffer, 
		d->localLocation->data);
	strcat (workbuffer, "}}");
      }
      if (d->shelvingLocation) {
	strcat (workbuffer, " {shelvingLocation {");
	strcat (workbuffer, 
		d->shelvingLocation->data);
	strcat (workbuffer, "}}");
      }
      if (d->callNumber) {
	strcat (workbuffer, " {callNumber {");
	strcat (workbuffer, 
		d->callNumber->data);
	strcat (workbuffer, "}}");
      }
      if (d->shelvingData) {
	strcat (workbuffer, " {shelvingData {");
	strcat (workbuffer, 
		d->shelvingData->data);
	strcat (workbuffer, "}}");
      }
      if (d->copyNumber) {
	strcat (workbuffer, " {copyNumber {");
	strcat (workbuffer, 
		d->copyNumber->data);
	strcat (workbuffer, "}}");
      }
      if (d->publicNote) {
	strcat (workbuffer, " {publicNote {");
	strcat (workbuffer, 
	d->publicNote->data);
	strcat (workbuffer, "}}");
      }
      if (d->reproductionNote) {
	strcat (workbuffer, " {reproductionNote {");
	strcat (workbuffer, 
		d->reproductionNote->data);
	strcat (workbuffer, "}}");
      }
      if (d->termsUseRepro) {
	strcat (workbuffer, " {termsUseRepro {");
	strcat (workbuffer, 
		d->termsUseRepro->data);
	strcat (workbuffer, "}}");
      }
      if (d->enumAndChron) {
	strcat (workbuffer, " {enumAndChron {");
	strcat (workbuffer, 
	d->enumAndChron->data);
	strcat (workbuffer, "}}");
      }
      
      if (d->volumes)
	strcat (workbuffer, " {VOLUMEINFO");

      for (vol = d->volumes; vol ; vol = vol->next) {
	v = vol->item;

	if (v->enumeration) {
	  strcat (workbuffer, " {enumeration {");
	  strcat (workbuffer, 
		  v->enumeration->data);
	  strcat (workbuffer, "}}");
	}
	if (v->chronology) {
	  strcat (workbuffer, " {chronology {");
	  strcat (workbuffer, 
		  v->chronology->data);
	  strcat (workbuffer, "}}");
	}
	if (v->enumAndChron) {
	  strcat (workbuffer, " {enumAndChron {");
	  strcat (workbuffer, 
		  v->enumAndChron->data);
	  strcat (workbuffer, "}}");
	}
      }
      if (d->volumes)
	strcat (workbuffer, "}");
      
  
      if (d->circulationData)
	strcat (workbuffer, " {CIRCDATA");

      for (circ = d->circulationData; circ ; circ = circ->next) {
	c = circ->item;
	/* for availablenow and renewable booleans */
	if (c->availableNow)
	  strcat (workbuffer, " {availableNow {YES}}");
	else
	  strcat (workbuffer, " {availableNow {NO}}");
	
	if (c->availablityDate) {
	  strcat (workbuffer, " {availablityDate {");
	  strcat (workbuffer, 
		  c->availablityDate->data);
	  strcat (workbuffer, "}}");
	}
	if (c->availableThru) {
	  strcat (workbuffer, " {availableThru {");
	  strcat (workbuffer, 
		  c->availableThru->data);
	  strcat (workbuffer, "}}");
	}
	if (c->restrictions) {
	  strcat (workbuffer, " {restrictions {");
	  strcat (workbuffer, 
		  c->restrictions->data);
	  strcat (workbuffer, "}}");
	}
	if (c->itemId) {
	  strcat (workbuffer, " {itemId {");
	  strcat (workbuffer, 
		  c->itemId->data);
	  strcat (workbuffer, "}}");
	}

	if (c->renewable)
	  strcat (workbuffer, " {renewable {YES}}");
	else
	  strcat (workbuffer, " {renewable {NO}}");
	
	if (c->onHold)
	  strcat (workbuffer, " {onHold {YES}}");
	else
	  strcat (workbuffer, " {onHold {NO}}");
	
    
	if (c->enumAndChron) {
	  strcat (workbuffer, " {enumAndChron {");
	  strcat (workbuffer, 
		  c->enumAndChron->data);
	  strcat (workbuffer, "}}");
	}
	if (c->midspine) {
	  strcat (workbuffer, " {midspine {");
	  strcat (workbuffer, 
		  c->midspine->data);
	  strcat (workbuffer, "}}");
	}
	if (c->temporaryLocation) {
	  strcat (workbuffer, " {temporaryLocation {");
	  strcat (workbuffer, 
		  c->temporaryLocation->data);
	  strcat (workbuffer, "}}");
	}
	}
      if (d->circulationData)
	strcat (workbuffer, "}");
      
    }
  }
  
  return (workbuffer);

}


/* this routine removes braces from MARC data by reversable substitution */
char *clipMARCdata(char *record)
{
  char *badopen, *badclose, *subchar;
  int code = 0;
  
  if (badopen = strchr(record,'{')) {
    code = 1;
    if (subchar = strchr(record,'\001'))
      fprintf(stderr, "unable to convert open brace in marc record\n");
    else {
      *badopen = '\001';
      while (badopen = strchr(badopen,'{'))
	*badopen = '\001';
    }
    
  } 
  if (badclose = strchr(record,'}')) {
    code = code + 2;
    if (subchar = strchr(record,'\002')) {
      fprintf(stderr, "unable to convert close brace in marc record\n");
    }
    else {
      *badclose = '\002';
      while (badclose = strchr(badclose,'}'))
	*badclose = '\002';
    } 
  }
  if (code) {
    /* now we use an unassigned character in the leader to indicate */
    /* that substutions have happened                               */
    record[8] = (char) code + '0';
  }
  
  return (record);

}

/* this routine replaces braces in MARC data reversing clipMARCdata */
void
unclipMARCdata(char *record)
{

  char *c;  

  if (record[8] != ' ') {

    c = record;
    while (c = strchr(c,'\001'))
      *c = '{';

    c = record;
    while (c = strchr(c,'\002'))
	*c = '}';
  }
}

