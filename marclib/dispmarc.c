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
 * Copyright (c) 1992 The Regents of the University of California.
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
/* DispMARC - print marc records from a file                              */
/**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#ifndef SOLARIS
#include "strings.h"
#endif
#include <string.h>
#include "marc.h"


#define MENUHT 3

#define   RECBUFSIZE     10000
#define   FIELDBUFSIZE   10000
#define   READONLY       O_RDONLY
#define   BADFILE        -1
#define   TRUE           1
#define   FALSE          0

#ifndef   SEEK_CUR
#define   SEEK_CUR       1
#endif


/* EXTERNAL ROUTINES -- in marclib.c and memcntl.c */
extern int GetNum(/*start char, number of chars to convert */);
extern int ReadMARC(/* file id, buffer, buffer size*/);
extern MARC_FIELD *SetField(/*rec, dir*/);
extern MARC_REC *GetMARC(/*buffer,record length, copy flag*/);
extern MARC_FIELD *GetField(/*marc_rec pointer,buffer,field tag*/);
extern MARC_SUBFIELD *GetSubf(/*marc_field pointer,buffer,subfield code*/);


/* EXTERNAL VARIABLES */
char recbuffer[RECBUFSIZE];
char fieldbuffer[FIELDBUFSIZE];
char linebuffer[FIELDBUFSIZE];

typedef struct {
	char *label;
	char *tags;
	char *subfields;
        char *beginpunct;
	char *subfsep;
	char *endpunct;
        int  newfield;
	int  print_all;
	int  print_indicators;
	int  print_delimiters;
        int  repeatlabel;
        int  indent;
 } DISP_FORMAT;
	
DISP_FORMAT defaultformat[] = {
     {"Record #", "", "",""," ","\n",     TRUE,FALSE,FALSE,FALSE, FALSE, 0},
     {"Author:" , "1xx", "", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Title:"  , "245", "", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Publisher:", "260", "", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Pages:"  , "300", "", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Series:" , "4xx", "", ""," ", "\n", TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Notes:"  , "5xx", "", "", " ","\n",   TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Subjects:","6xx", "", "", " -- ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Other authors:","7xx", "", "", " ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Call Numbers:","950", "", "", " ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,15},
     {NULL,NULL,NULL,NULL,NULL,NULL,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT shortformat[] =  {
     {"", "", "",""," ",". ", TRUE,FALSE,FALSE,FALSE,FALSE, 0},
     {"", "1xx", "ab", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 0},
     {"", "245", "a", ""," ", ".",  TRUE,FALSE,FALSE,FALSE,FALSE, 4},
     {"", "260", "c", "", " ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 4},
     {"", "950", "","", " ", "\n",TRUE,FALSE,FALSE,FALSE,FALSE,4},
     {NULL,NULL,NULL,NULL,NULL,NULL,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT marcformat[] =  {
     {"Record ID: ", "", "",""," ","\n", TRUE,FALSE,FALSE,FALSE,FALSE, 0},
     {"" , "xxx", "", "","", "\n",  TRUE,TRUE,TRUE,TRUE,FALSE,0},
     {NULL,NULL,NULL,NULL,NULL,NULL,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT evaluationformat[] = {
     {"Record #", "", "",""," ","\n",     TRUE,FALSE,FALSE,FALSE, FALSE, 0},
     {"Title:"  , "245", "", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Subjects:","6xx", "", "", " -- ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE, 15},
     {"Call Numbers:","950", "", "", " ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,15},
     {NULL,NULL,NULL,NULL,NULL,NULL,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

/*************************************************************************/
/* DispMARC - display a marc record given files and record ID            */
/*            requires an associator address file for the MARC file      */
/*            Parameters:						 */
/*			marcin - MARC input file			 */
/*			assocfile - Associator file for marcin		 */
/*			recordID - MARC record # to retrieve		 */
/*			format - one of the formats defined above	 */
/*			displaynum - number to use in display (if 	 */
/*				-1 use the recordID.			 */
/*			maxlen - maximum length of line to output        */
/*			output - pointer to the output routine		 */
/*************************************************************************/
MARC_REC *DispMARC(marcin,assocfile,recordID,format,displaynum, maxlen, output)
int marcin, assocfile; /*files*/
int recordID, format, displaynum, maxlen ;
void *(output)();
{
	MARC_DIRENTRY_OVER *dir;
	MARC_FIELD *fld;
	MARC_SUBFIELD *subf;
	MARC_REC *rec;
	int lrecl;
	char *readptr, *c;
	int  i;
	int  print_leader = FALSE;         /* special function flags */
	int  print_directory = FALSE;
	int  print_authority = FALSE;
	int  repeat = FALSE;
        char *line, *format_field();
        DISP_FORMAT *formatcontrol, *f;
        
        
        switch (format) {
		case 1: formatcontrol = &shortformat[0];
			break;
		case 2: formatcontrol = &marcformat[0];
        		break;
		case 3: formatcontrol = &evaluationformat[0];
			break;
        	default: formatcontrol = &defaultformat[0];
        		break;
        }

        /* find the address of the record and position the marc file pointer */
	if (assocfile && recordID > 0) {
		if (SeekMARC(marcin, assocfile, recordID) == -1)
			return(NULL);
	}

	/* Read the record from the marcin file */
	if ((lrecl = ReadMARC(marcin,recbuffer,sizeof recbuffer)) == 0)
		return(NULL);
	else {  /* record was read - so load it into the marc structure */
		if((rec = GetMARC(recbuffer,lrecl,1)) == NULL) {
			fprintf(stderr,"no record retrieved by GetMARC\n");
			return(NULL);
		}

		if (displaynum == -1) displaynum = recordID;

                for (f = formatcontrol; f->label; f++) {
                	/* get the first field in the format */
			fld = GetField(rec, NULL, fieldbuffer, f->tags);

			/* if no field found, check for number format */
			if (fld == NULL && *f->tags == '\0') {
				/* a null tag means output the supplied */
				/* record number			*/
				sprintf(linebuffer,"%s%s%d%s", 
					f->label, f->beginpunct,displaynum, f->endpunct);
				/* assume it won't be longer than maxlen*/
				outputline (output, linebuffer, maxlen, f->indent);
			}	
			repeat = FALSE;
			
			while (fld) {
				if (f->print_all) {
					codeconvert(fieldbuffer);
					if (*f->label == '\0')
						sprintf(linebuffer,"%s %s%s%s", 
							fld->tag, f->beginpunct,fieldbuffer, f->endpunct);
					else
						sprintf(linebuffer,"%s %s%s%s", 
							f->label, f->beginpunct,fieldbuffer, f->endpunct);
					outputline (output, linebuffer, maxlen, f->indent);
				}
				else  {/* more selective printing */
					line = format_field(fld,f,linebuffer,repeat);
					if (line) outputline (output, line, maxlen, f->indent);
				}
				/* more of the same tag set? */
				fld = GetField(NULL,fld->next,fieldbuffer,f->tags);
				if (fld) repeat = TRUE;
			}
		}
	        return(rec);
	}
}


/***********************************************************************/
/* format_field - given a marc field struct and a format item, build   */
/*                a line in a buffer according to the format.          */
/***********************************************************************/
char *format_field(mf,format,buff,repeat)
MARC_FIELD *mf;
DISP_FORMAT *format;
char *buff;
int repeat;
{
	MARC_SUBFIELD *subf;
	register char *linend, *c;

        int pos, count, ok;
	
	linend = buff;
	*linend = '\0';
	pos = 0;

	if (repeat && (format->repeatlabel == FALSE)) ;  /* skip it */
	else /* add the label */
		for(c = format->label; *c ; *linend++ = *c++) pos++;

        /* indentation */
        for (; pos < format->indent; pos++) *linend++ = ' ';
        /* initial 'punctuation' */		
	for(c = format->beginpunct; *c ; *linend++ = *c++);
	/* subfields */
	for (subf = mf->subfield; subf; subf = subf->next) {
		if ((*format->subfields == '\0') || 
			(strchr(format->subfields, subf->code))) {	
			/* this one should be copied */
			for(c = format->beginpunct; *c ; *linend++ = *c++);
			count = subfcopy(linend,subf->data,1) - 1;
			linend += count;
			for(c = format->subfsep; *c ; *linend++ = *c++);
			ok = TRUE;
		}
	}
	
	if (ok) {
		/* backtrack over the last subfield separator */
		linend -= strlen(format->subfsep);
		/* add end of field punctuation */
		if (*format->endpunct) {
			/* kill the existing punctuation and trailing blanks */
			if (ispunct(*(linend-1)) && ispunct(*format->endpunct) &&
			   *(linend-1) != ')' && *(linend-1) != ']')
				linend--;
			while(*(linend-1) == ' ') linend--;
	 		for(c = format->endpunct; *c ; *linend++ = *c++) pos++;
 		}
		*linend = '\0'; /* Null terminate the line */
		return(buff);	
	}
	else return(NULL); /* no subfields copied */
}



/* break an output line into segments to fit on the screen and call the */
/* output function                                                      */
outputline(outfunc,line, maxlen, indent)
void *(outfunc)();
char *line;
int maxlen, indent;
{
  int linelen, i;
  char indentstr[80];
  char *nextpart, *c;

  /* if the line will fit output it now */
  if ((linelen = strlen(line)) <= maxlen) {
  	(*outfunc)(line);
	return;
  }
  else { /* put out first part, no indentation */
	for (c = &line[maxlen - 1]; *c != ' '; c--); /* find word break */
	*c = '\0';
	nextpart = c+1;
	(*outfunc)(line);
	(*outfunc)("\n");
  }
  /* set up indent string - add 3 spaces to regular indent for wrapped lines */
  indent += 3;
  for (i=0;i<indent; i++) indentstr[i] = ' ';
  indentstr[i] = '\0';

  /* loop to output rest of line */
  while ((linelen = strlen(nextpart)) > (maxlen - indent)) {
        for (c = &nextpart[maxlen - indent]; *c != ' '; c--); 
             /* find word break */ 
        *c = '\0'; 
        (*outfunc)(indentstr); 
        (*outfunc)(nextpart); 
        (*outfunc)("\n");
        nextpart = c+1;  
  }
  (*outfunc)(indentstr); 
  (*outfunc)(nextpart); 
  return;
}



