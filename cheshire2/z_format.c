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
 * Copyright (c) 1994-6 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Information Management and Systems, UC Berkeley
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
/* TKZ_Format -- format records and concatenate them to the current  */
/*               tcl return value                                         */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#ifndef SOLARIS
#ifndef WIN32
#include <strings.h>
#endif
#endif
#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif
#include "tcl.h"
#include "marc.h"
#include "dispsgml.h"
#include "dispmarc.h"
#include "sgml.h"

Tcl_Interp *Current_interp;		/* Current interpreter. */

#define MAX(x,y) ((x) > (y)) ? (x) : (y) 


#define NUM_FORMAT_SLOTS 200

FORMAT_TAB FormatTable[NUM_FORMAT_SLOTS] = {
  {"FULL", "",defaultformat},
  {"LONG", "",defaultformat},
  {"TAGGED","", defaultformat},
  {"BRIEF","", shortformat},
  {"SHORT","", shortformat},
  {"MARC","", marcformat},
  {"FULLMARC","", marcformat},
  {"REVIEW","", evalformat},
  {"EVAL", "",evalformat},
  {"LIST", "",tcllistformat},
  {"TCLLIST","", tcllistformat},
  {"SGML", "",dummyformat},
  {"HTML", "",htmlformat},
  {"SHORTHTML","", shorthtmlformat},
  {"REVIEWHTML","", reviewhtmlformat},
  {"DC","", dcformat},
  {"SHORTDC","", shortdcformat},
  {"","", NULL} /* default to brief format */
};

int LAST_BUILTIN_FORMAT = 17;
int last_format_slot = 17 ;

/* external and forward prototypes */
MARC_REC *DispMARCbuff(char *recbuffer, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)());

MARC_REC *DispOPACbuff(Tcl_Interp *interp, char *recbuffer, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)());

SGML_Document *DispSGMLbuff(char *recbuffer, char *dtd_file, char *format,
		   int displaynum, int maxlen, void (*output)(), char *filename);


void 
TKZ_Format_Out(char *line)
{
     Tcl_AppendResult(Current_interp, line, (char *)NULL );
     
}

int
TKZ_FormatCmd(dummy, interp, argc, argv)
ClientData dummy;			/* Not used. */
Tcl_Interp *interp;			/* Current interpreter. */
int argc;				/* Number of arguments. */
char **argv;			/* Argument strings. */
{
     char *frmtname, *up_case_name();
     FORMAT_TAB *ft;
     DISP_FORMAT *format_chosen = NULL;
     int rectype;
     MARC_REC *record;
     SGML_Document *sgml_record;
     int line_len = 72;
     int recnumber = -1;
     char *filename = NULL;     
     int MARC = 1;
     int SGMLMARC = 2;
     int OPAC = 3;
     int SUTRS = 4;
     int SGML = 5;
     int XML = 5;
     int GENERIC = 6;
     int EXPLAIN = 7;

     void TKZ_Format_Out();
     
     if (argc < 4 || argc > 7) {
	  Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
			   " formatname {record} rectype <recnumber> <max_line_length> <DTD filename>\"", 
			   (char *) NULL);
	  return TCL_ERROR;
     }


     if (strlen(argv[2]) == 0) {
       Tcl_AppendResult(interp, "Empty record supplied for formatting\n", 
			  (char *) NULL);
       Tcl_AppendResult(interp, "Usage: \"", argv[0],
			" formatname {record} rectype <recnumber> <max_line_length> <DTD filename>\"", 
			(char *) NULL);
       
       return TCL_ERROR;
     }	 
	 
/* printf ("zformat parms:\n1: %s \n2: %s \n3: %s \n4: %s \n5: %s \n6: %s\n",
		argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
*/
     if ((filename = Tcl_GetVar(interp, "CHESHIRE_DATABASE",
				TCL_GLOBAL_ONLY )) == NULL) {
       filename = "***dummy***";
     }

     /* find out record type MARC||SGML||OPAC||SUTRS */
     if (argv[3][0] >= '0' && argv[3][0] <= '9')
       rectype = atoi(argv[3]);
     else {
       /* interpret string form */
       if (strncasecmp(argv[3],"marc",4) == 0)
	 rectype = MARC;
       else if (strncasecmp(argv[3],"sgmlmarc",8) == 0)
	 rectype = SGMLMARC;
       else if (strncasecmp(argv[3],"sgml",4) == 0)
	 rectype = SGML;
       else if (strncasecmp(argv[3],"xml",4) == 0)
	 rectype = XML;
       else if (strncasecmp(argv[3],"opac",4) == 0)
	 rectype = OPAC;
       else if (strncasecmp(argv[3],"sutr",4) == 0)
	 rectype = SUTRS;
       else if (strncasecmp(argv[3],"gene",4) == 0)
	 rectype = GENERIC;
       else if (strncasecmp(argv[3],"expl",4) == 0)
	 rectype = EXPLAIN;
       else if (strncasecmp(argv[3],"text",4) == 0)
	 rectype = SUTRS;
       else if (strncasecmp(argv[3],"usma",4) == 0)
	 rectype = MARC;
     }
     
     frmtname = up_case_name(argv[1]);
     
     /* if MARC, scan the format table for known formats */
     if (rectype == MARC || rectype == OPAC) {
       for (ft = FormatTable; ft->formatname[0] != '\0'
	      && format_chosen == NULL; ft++) {
	 if (strcmp(ft->formatname, frmtname) == 0) {
	   format_chosen = ft->format_ptr;
	 }
       }
     }
     
     if (argc == 5 || argc == 6 || argc == 7)
	  recnumber = atoi(argv[4]);
     
     if (argc == 6 || argc == 7) 
	  line_len = atoi(argv[5]);     
     
     Current_interp = interp;
     
     if (rectype == MARC) 
     {
	  record = DispMARCbuff(argv[2], format_chosen, recnumber, 
				line_len, TKZ_Format_Out);
	  if (record == NULL)
	       return TCL_ERROR;
	  else
	       FreeMARC_REC(record);
     }
     else if ((rectype == SGMLMARC) || (rectype == SGML) || (rectype == XML))
     {
       if (argv[6] != NULL)
	 sgml_record = DispSGMLbuff(argv[2], argv[6], frmtname, recnumber, 
				    line_len, TKZ_Format_Out, filename);
       else {
	 Tcl_AppendResult(interp, "No DTD pathname supplied for SGMLMARC record\n", 
			  (char *) NULL);
	  Tcl_AppendResult(interp, "Usage: \"", argv[0],
			   " formatname {record} rectype <recnumber> <max_line_length> <DTD filename>\"", 
			   (char *) NULL);

	 return TCL_ERROR;
       }	 
     }
     else if (rectype == OPAC)
     {
	  record = DispOPACbuff(interp, argv[2], format_chosen, recnumber, 
				line_len, TKZ_Format_Out);
	  if (record == NULL)
	       return TCL_ERROR;
	  else
	       FreeMARC_REC(record);
     }
     else 
     { /* treat as plain text and output -- assume that the text is */
	  /* is SUTRS and therefore, preformatted or that the output   */
	  /* device will handle wrapping of lines and such             */
	  TKZ_Format_Out(argv[2]);
     }  
     
     return TCL_OK;
     
} 


int
TKZ_MakeFormatCmd(dummy, interp, argc, argv)
     ClientData dummy;		/* Not used. */
     Tcl_Interp *interp;	/* Current interpreter. */
     int argc;			/* Number of arguments. */
     char **argv;		/* Argument strings. */
{
  char *tempname, *dtd_name, *up_case_name();
  FORMAT_TAB *ft;
  int n, i;
  DISP_FORMAT *newformat, *pf;
  char **elementvPtr;
  int  elementcPtr;
  char **components;
  int  compcount;
  int  list_pos;
  
  if (argc != 3 && argc != 4) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " formatname [DTDNAME] {{list_of_format_elements}}\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }
  
  tempname = up_case_name(argv[1]);
  if (argc == 4) {
    dtd_name = argv[2];
    list_pos = 3;
  }
  else {
    dtd_name = NULL;
    list_pos = 2;
  }
 
  /* scan the format table for known formats */
  for (ft = FormatTable, n = 0; ft->formatname[0] != '\0'; ft++, n++) {
    if (strcmp(ft->formatname, tempname) == 0) {
      Tcl_AppendResult(interp, "Display format name ", tempname,
		       " already defined -- use removeformat to delete it.", 
		       (char *) NULL);
      return TCL_ERROR;
    }
  }
     
  last_format_slot = n;
  if (n < NUM_FORMAT_SLOTS) {
    if (Tcl_SplitList(interp, argv[list_pos], &elementcPtr, &elementvPtr) 
	== TCL_ERROR) {
      Tcl_AppendResult(interp, 
		       "Badly formed format element list ", 
		       argv[list_pos], (char *) NULL);
      return TCL_ERROR;
    }
    
       
    pf = newformat = CALLOC(DISP_FORMAT, (elementcPtr+1));
    
    for (i = 0; i < elementcPtr; i++, pf++) {
      if (Tcl_SplitList(interp, elementvPtr[i], &compcount, &components) 
	  == TCL_ERROR || compcount < 13) {
	Tcl_AppendResult(interp, 
			 "Badly formed format components list \"", 
			 elementvPtr[i], "\"\n", "components should be\n {", 
			 "{label string} ",
			 "{tags string} ",
			 "{subfields string} ",
			 "{beginpunct string} ",
			 "{subfsep string} ",
			 "{endpunct string} ",
			 "{newfield TRUE | FALSE} ",
			 "{print_all TRUE | FALSE} ",
			 "{print_indicators TRUE | FALSE} ",
			 "{print_delimiters TRUE | FALSE} ",
			 "{repeatlabel TRUE | FALSE} ",
			 "{multisubstitute TRUE | FALSE} ",
			 "{indent number}} \n\n", "For example:\n",
			 "{{Title: } {245} {ab} { } { } {.} TRUE FALSE FALSE FALSE FALSE FALSE 10}\n",
			 "Where: \n","label is a leading label to print\n ",
			 "tags is a regexp indicating the field tags to print\n",
			 "subfields are the codes for subfields to print\n",
			 "beginpunct string to print before field data\n",
			 "subfsep string to print between subfields\n",
			 "{endpunct string to print at end of field\n",
			 "newfield TRUE | FALSE if field starts something\n",
			 "print_all TRUE | FALSE if all marc elements print\n",
			 "print_indicators TRUE | FALSE print marc indicators\n",
			 "print_delimiters TRUE | FALSE print subfield delimiters\n",
			 "repeatlabel TRUE | FALSE if label repeats for ",
			 "each instance of the field\n",
			 "multisubstitute TRUE | FALSE substitute the field ",
			 "for '%' in the beginpunct string\n"
			 "indent is the number of spaces to indent field data",
			 " from label start\n", 
			 (char *) NULL);
	return TCL_ERROR;
      }
      strncpy(pf->label, components[0], 499);
      strncpy(pf->tags, components[1], 199);
      strncpy(pf->subfields, components[2], 199);
      strncpy(pf->beginpunct, components[3], 199);
      strncpy(pf->subfsep, components[4], 199);
      strncpy(pf->endpunct, components[5], 199);
      pf->newfield = truthflag(components[6]);
      pf->print_all = truthflag(components[7]);
      pf->print_indicators = truthflag(components[8]);
      pf->print_delimiters = truthflag(components[9]);
      pf->repeatlabel = truthflag(components[10]);
      pf->multisubstitute = truthflag(components[11]);
      pf->indent = atoi(components[12]);
	       
      /* free up the components */
      Tcl_Free(components);
      
    }
    pf->newfield = -1; /* terminate the list */
    Tcl_Free(elementvPtr); 
    
  }
  else  { /* ran out of slots */
    Tcl_AppendResult(interp, 
		     "Ran out of space in formats name table", (char *) NULL);
    return TCL_ERROR;
  }
     
  /* install the new_var format in the format table */
  strncpy(ft->formatname, tempname, FORMAT_NAME_SIZE-1);
  if (argc == 4) {
    strncpy(ft->DTD_name, dtd_name, DTD_NAME_SIZE-1);
  }
  else {
    ft->DTD_name[0] = '\0';
  }
  ft->format_ptr = newformat;
  ft->sgml_format_ptr = NULL;
  last_format_slot++;
  ft++;
  *ft->formatname = '\0';
  ft->format_ptr = NULL;
  
  free (tempname);
  
  return TCL_OK;
  
}


int
truthflag(char *flag)
{
  if (flag == NULL) return 0;
  
  if (strncasecmp("TRUTH", flag, MAX(strlen(flag),5)) == 0) return 1;
  if (strncasecmp("TRUE", flag, MAX(strlen(flag),4)) == 0) return 1;
  return 0;
}

int
TKZ_RemoveFormatCmd(dummy, interp, argc, argv)
     ClientData dummy;		/* Not used. */
     Tcl_Interp *interp;	/* Current interpreter. */
     int argc;			/* Number of arguments. */
     char **argv;		/* Argument strings. */
{
  char *tempname, *up_case_name();
  FORMAT_TAB *ft;
  int n;
  
  
     
  if (argc != 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " formatname\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  tempname = up_case_name(argv[1]);
  
  /* scan the format table for known formats */
  for (ft = FormatTable, n = 0; ft->formatname[0] != '\0'; ft++, n++) {
    if (strcmp(ft->formatname, tempname) == 0) {
      ft->formatname[0] = '\0';
      if (n > LAST_BUILTIN_FORMAT) {
	free(ft->format_ptr);
      }
    }
  }
  free(tempname);
  return TCL_OK;
}

char *do_escapes(char *in, char *out) 
{
  char *c, *b;
  b = out;
  
  for (c = in; *c != '\0' ;) {
    if (*c == '{' || *c == '}' || *c == '"')
      *b++ = '\\';
    *b++ = *c++;
  }
  *b = '\0';
  return(out);
}


int
TKZ_ShowFormatCmd(dummy, interp, argc, argv)
     ClientData dummy;		/* Not used. */
     Tcl_Interp *interp;	/* Current interpreter. */
     int argc;			/* Number of arguments. */
     char **argv;		/* Argument strings. */
{
  char *tempname, *up_case_name();
  FORMAT_TAB *ft;
  char elementbuffer[500];
  char workbuff[6][100];
  int n;
  DISP_FORMAT *newformat;
  Tcl_DString dstring;
  
  if (argc != 2) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " formatname\"", (char *) NULL);
    return TCL_ERROR;
  }
  
  
  tempname = up_case_name(argv[1]);
  
  Tcl_DStringInit(&dstring);
     
  /* scan the format table formats */
  for (ft = FormatTable, n = 0; n < last_format_slot; ft++, n++) {
    if (ft->formatname[0] != '\0' 
	&& (strcmp(tempname,ft->formatname) == 0 
	    || strcmp(tempname, "ALL") == 0)) {
      Tcl_DStringAppendElement(&dstring, ft->formatname);
      
      for (newformat = ft->format_ptr; newformat->newfield >= 0; newformat++) {
	sprintf(elementbuffer,
		"{%s} {%s} {%s} {%s} {%s} {%s} %s %s %s %s %s %s %d",
		(do_escapes(newformat->label,workbuff[0])),
		(do_escapes(newformat->tags,workbuff[1])),
		(do_escapes(newformat->subfields,workbuff[2])),
		(do_escapes(newformat->beginpunct,workbuff[3])),
		(do_escapes(newformat->subfsep,workbuff[4])),
		(do_escapes(newformat->endpunct,workbuff[5])),
		(newformat->newfield == 1) ? "TRUE" : "FALSE" ,
		(newformat->print_all == 1) ? "TRUE" : "FALSE" ,
		(newformat->print_indicators == 1) ? "TRUE" : "FALSE" ,
		(newformat->print_delimiters == 1) ? "TRUE" : "FALSE" ,
		(newformat->repeatlabel == 1) ? "TRUE" : "FALSE" ,
		(newformat->multisubstitute == 1) ? "TRUE" : "FALSE" ,
		newformat->indent );
	Tcl_DStringAppendElement(&dstring, elementbuffer);
      }
      Tcl_AppendElement(interp, Tcl_DStringValue(&dstring));
      Tcl_DStringFree(&dstring);
      Tcl_DStringInit(&dstring);
    }
  }
  free(tempname);
  
  return TCL_OK;
}

void
init_display_formats()
{
  FORMAT_TAB *ft;
  SGML_FORMAT *sft;
  DISP_FORMAT *dft;
  int numitems;
  int i, j;
  /* this adds the SGML formats to the table */
  /* scan the format table formats */
  
  for (i = 1; i < NUM_FORMATS; i++) {

    strcpy(FormatTable[LAST_BUILTIN_FORMAT].formatname, 
	   known_formats[i].format_name);

    strcpy(FormatTable[LAST_BUILTIN_FORMAT].DTD_name, 
	   known_formats[i].dtd_name);

    
     switch (known_formats[i].format_id)
     {
     case 1: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_review[0];
	     break;
     case 2: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_short[0];
	     break;
     case 3: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_long[0];
	     break;
     case 4: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &LCCLUST_lccshort[0];
	     break;
     case 5: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_marc[0];
	     break;
     case 6: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_HTMLreview[0];
	     break;
     case 7: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_HTMLshort[0];
	     break;
     case 8: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_HTMLlong[0];
	     break;
     case 9: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
	       &USMARC_CSMP_HTMLreview[0];
	     break;
     case 10: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&USMARC_CSMP_HTMLshort[0];
	     break;
     case 11: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&USMARC_CSMP_HTMLlong[0];
	     break;
     case 12: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&FT931_review[0];
	     break;
     case 13: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&FT931_short[0];
	     break;
     case 14: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&FT931_long[0];
	     break;
     case 15: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&USMARC_GLAS_HTMLreview[0];
	     break;
     case 16: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&USMARC_GLAS_HTMLshort[0];
	     break;
     case 17: FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr = 
		&USMARC_GLAS_HTMLlong[0];
	     break;
     }
     
     numitems = 0;

     for (sft = FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr; sft->label;
	  sft++) numitems++;

     FormatTable[LAST_BUILTIN_FORMAT].format_ptr = dft =
       CALLOC(DISP_FORMAT, numitems+1);

     for (sft = FormatTable[LAST_BUILTIN_FORMAT].sgml_format_ptr; sft->label;
	  sft++) {
       strcpy(dft->label, sft->label);
       strcpy(dft->tags, sft->elements);
       strcpy(dft->subfields, sft->subelmts);
       strcpy(dft->subfsep, sft->elsep);
       strcpy(dft->beginpunct, sft->beginpunct);
       dft->newfield = TRUE;
       dft->print_all = FALSE;
       dft->print_indicators = FALSE;
       dft->print_delimiters = FALSE;
       dft->repeatlabel = FALSE;
       dft->multisubstitute = sft->special_proc;
       dft->indent = 0;
       dft++;
     }
    
     dft->newfield = -1;

    LAST_BUILTIN_FORMAT++;
  }

  last_format_slot = LAST_BUILTIN_FORMAT;


}








