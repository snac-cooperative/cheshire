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
/* DispMARC - print marc records from a file or buffer                    */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include "marc.h"
#include "dispsgml.h"
#define INIT_FORMATS
#include "dispmarc.h"
#include "tcl.h"

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

#define DATATYPE_MARC 0
#define DATATYPE_SGML 1


/* EXTERNAL VARIABLES */
char recbuffer[RECBUFSIZE];
char fieldbuffer[FIELDBUFSIZE];
char linebuffer[FIELDBUFSIZE];

void outputline(void (*outfunc)(), char *in_line, int maxlen, 
		int indent, int datatype);
char *xml_sub_chars(char *fieldbuffer);

extern int Tcl_StringMatch(const char *str, const char *pattern);

/* this is called for tag comparisons in marclib.c */
int tagcmp(char *pattern, char *string)
{
  char marctagbuffer[4];

  strncpy(marctagbuffer,string,3);
  marctagbuffer[3] = '\0'; 

  return(Tcl_StringMatch(marctagbuffer,pattern));
}


/* the following is called for formatting MARC data */

MARC_REC *FormatMARCrec(MARC_REC *rec, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)())
{

  MARC_FIELD *fld;
  int  print_leader = FALSE;         /* special function flags */
  int  print_directory = FALSE;
  int  print_authority = FALSE;
  int  repeat = FALSE;
  int  list_format = FALSE;
  int  line_length;
  int  field_num = 1;
  char *line, *format_field(), *tcl_escape();
  DISP_FORMAT *f;
        
  line_length = maxlen;
        

  if (format == tcllistformat) {
    list_format = 1;
    line_length = 10000;  /* prevent wrapping */
  }
  
  if (list_format == 1) { /* do some special initialization */
    sprintf(linebuffer,"{%d {0 000 {", displaynum);
    strncat(linebuffer, (char *)rec->leader, sizeof(MARC_LEADER_OVER));
    strcat(linebuffer,"}}");
    outputline (output, linebuffer, line_length, 0 /* no indenting */,0);
  }

  if (format == htmlformat ||
      format == shorthtmlformat ||
      format == reviewhtmlformat) {
    list_format = 2;
    line_length = 10000;  /* prevent wrapping */
  }

  if (format == dcformat ||
      format == shortdcformat) {
    list_format = 3;      /* duplicate tags */
    line_length = 10000;  /* prevent wrapping */
  }


  for (f = format; f->newfield >=0 ; f++) {
    /* get the first field in the format */
    fld = GetField(rec, NULL, fieldbuffer, f->tags);
    
    /* if no field found, check for number format */
    if (fld == NULL && *f->tags == '#') {
      /* a pound sign means output the supplied */
      /* record number -- unless it is negative */
      if (displaynum >= 0) {
	sprintf(linebuffer,"%s%s%d%s", 
		f->label, f->beginpunct,displaynum, f->endpunct);
	/* assume it won't be longer than maxlen*/
	outputline (output, linebuffer, line_length, f->indent,0);
      }
    }	
    /* if no field found, check for number format */
    if (fld == NULL && *f->tags == '+') {
      /* a plus sign means output the supplied */
      /* label only                            */
      if (displaynum >= 0) {
	sprintf(linebuffer,"%s", f->label);
	/* assume it won't be longer than maxlen*/
	outputline (output, linebuffer, line_length, f->indent,0);
      }
    }	
    repeat = FALSE;
    
    while (fld) {
      if (f->print_all) {
	codeconvert(fieldbuffer);
	if (*f->label == '\0') {
	  sprintf(linebuffer,"%s %s%s%s", 
		  fld->tag, f->beginpunct,fieldbuffer, f->endpunct);
	}
	else {
	  sprintf(linebuffer,"%s %s%s%s", 
		  f->label, f->beginpunct,fieldbuffer, f->endpunct);
	}
	outputline (output, linebuffer, line_length, f->indent,0);
      }
      else  {/* more selective printing */
	line = format_field(fld,f,linebuffer,repeat,list_format, 
			    field_num, fieldbuffer);
	if (line) {
	  if (list_format == 2) {
	    line  = tcl_escape(line);	    
	    outputline (output, line, line_length, f->indent,0);
	  }
	  else
	    outputline (output, line, line_length, f->indent,0);
	}
      }
      /* more of the same tag set? */
      fld = GetField(NULL,fld->next,fieldbuffer,f->tags);
      if (fld) repeat = TRUE;
      field_num++;
    }
  }
  if (list_format == 1) { /* do ending brace to terminate the list */
    sprintf(linebuffer,"}");
    outputline (output, linebuffer, line_length, 0 /* no indenting */,0);
  }

  return(rec);
}



/***********************************************************************/
/* format_field - given a marc field struct and a format item, build   */
/*                a line in a buffer according to the format.          */
/***********************************************************************/
char *format_field(mf,format,buff,repeat,list_format, field_num, fieldbuff)
MARC_FIELD *mf;
DISP_FORMAT *format;
char *buff;
int repeat;
int list_format;
int field_num;
char *fieldbuff;
{
  MARC_SUBFIELD *subf;
  register char *linend, *c;
  char *format_008();
  char *substitute;
  char *substart;
  char *tempsubf;
  char *tempconv;
  char blankfillchar = ' ';

  int pos, count, ok = FALSE;
  
  int multi;
  char *hold_copy;
  int hold_len;

  multi = format->multisubstitute;

  hold_copy = NULL;
  linend = buff;
  *linend = '\0';
  pos = 0;

  if (list_format==1) {/* each field in the list has to include the tag, etc */
    if (mf->tag[0] == '0' && mf->tag[1] == '0') {
      sprintf(buff," {%d %s {", field_num, mf->tag);
      linend += strlen(buff) ;
      fieldcopy(linend,mf->data);
      strcat(buff, "}}");
    }
    else
      sprintf(buff," {%d %s {%c%c}", field_num, mf->tag, mf->indicator1,
	      mf->indicator2);
    linend += strlen(buff) ;
  }
  else { /* otherwise it is a "regular" format */
    if (repeat && (format->repeatlabel == FALSE)) ;  /* skip it */
    else /* add the label */
      for(c = format->label; *c ; *linend++ = *c++) pos++;
  
    /* indentation */
    for (; pos < format->indent; pos++) *linend++ = ' ';

    /* initial 'punctuation' */		
    substitute = strchr(format->beginpunct, '%');
    if (multi && substitute != NULL) {
      for(c = format->beginpunct; c < substitute ; *linend++ = *c++);
      if (*(substitute+1) == 's')
	blankfillchar = ' ';
      else
	blankfillchar = *(substitute+1);
      substart = linend; /* so we know where to start rescanning */
    }
    else 
      for(c = format->beginpunct; *c ; *linend++ = *c++);
    
  }

  /* process the subfields */
  for (subf = mf->subfield; subf; subf = subf->next) {

    if ((*format->subfields == '\0') || 
	(strchr(format->subfields, subf->code))) {	
      /* this one should be copied */
      if (list_format != 2) /* avoid multiple insertion of markup */
	for(c = format->beginpunct; *c ; *linend++ = *c++);
      if (list_format==1) {
	*linend++ = '{';
	*linend++ = subf->code;
	*linend++ = '}';
	*linend++ = ' ';
	*linend++ = '{';
      }
      /* subfcopy will remove non-ascii chars if the last param is 1 */
      /* we want to keep them in for formatting most of the time now */
      if (list_format == 3) { 
	/* have to substitute things in XML */
	tempsubf = CALLOC(char, FIELDBUFSIZE);
	count = subfcopy(tempsubf,subf->data,0) - 1;
	tempconv = xml_sub_chars(tempsubf);
	strcpy(linend, tempconv);
	count = strlen(tempconv);
	if (tempsubf == tempconv) /* no substitutions */
	  FREE (tempsubf);
	else {
	  FREE (tempsubf);
	  FREE (tempconv);
	}
      } 
      else
	count = subfcopy(linend,subf->data,0) - 1;

      linend += count;
      for(c = format->subfsep; *c ; *linend++ = *c++);
      if (list_format == 1)
	*linend++ = '}'; /* terminate the subfield */
      else
	ok = TRUE;
    }
  }

  if (mf->subfield == NULL) {/* must be a fixed field */

    if (tagcmp("008",mf->tag)) {
      c = format_008(fieldbuff);
      for(; *c ; *linend++ = *c++) pos++;
      *linend = '\0';
      ok = TRUE;
    }

  }

  if (ok) {
    /* backtrack over the last subfield separator */
    linend -= strlen(format->subfsep);

    /* kill the existing punctuation and trailing blanks */
    if (ispunct(*(linend-1)) && ispunct(*format->endpunct) && 
	list_format != 3 &&
	*(linend-1) != ')' && *(linend-1) != ']' &&
	*(linend-1) != '>' && *(linend-1) != '<' )
      linend--;

    while(*(linend-1) == ' ') linend--;

    if (multi) {
      hold_len = (linend - substart);
      hold_copy = CALLOC(char,hold_len+1);
      memcpy(hold_copy,substart,hold_len);
    }

    if (blankfillchar != ' ') {
      for(c = substart; c < linend; c++)
	if (*c == ' ') *c = blankfillchar;
    }

    /* add end of field punctuation */
    if (*format->endpunct) {
      if (list_format == 2) { /* HTML format */
	  c = format->endpunct;
	  substitute = strchr(c, '%');
	  if (substitute) {
	    for(; c < substitute ; *linend++ = *c++) pos++;
	    strncpy(linend,hold_copy,hold_len);
	    linend += hold_len;
	    for (c = substitute+2; *c ; *linend++ = *c++) pos++;
	  }
	  else 
	    for(c = format->endpunct; *c ; *linend++ = *c++) pos++;

      }
      else {
	for(c = format->endpunct; *c ; *linend++ = *c++) pos++;
      }
    }
    *linend = '\0'; /* Null terminate the line */
    if (hold_copy) FREE(hold_copy);
    return(buff);	
  }
  else if (list_format==1) {
    for(c = format->endpunct; *c ; *linend++ = *c++) pos++;
    *linend = '\0'; /* Null terminate the line */
    return(buff);	
  }
  else return(NULL); /* no subfields copied */
}



/* break an output line into segments to fit on the screen and call the */
/* output function                                                      */
void
outputline(outfunc,in_line, maxlen, indent, datatype)
     void (*outfunc)();
     char *in_line;
     int maxlen, indent, datatype;
{
  int linelen, i;
  char indentstr[80];
  char *nextpart, *c;
  char *char_sub_line;
  char *line;


  normalize_char(in_line, &char_sub_line);

  if (datatype == 0) {
    line = char_sub_line;
  } 
  else {
    line = in_line;
  }


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

  /* free the workspace */
  free (char_sub_line);

  return;
}

/* the following puts out a tagged version of the 008 fixed fields */
char *format_008(fielddata)
char *fielddata;
{
  MARC_008_OVER *m;
  static char line[500];

  m = (MARC_008_OVER *)fielddata;
  

  sprintf(line,
"entry_date:'%c%c%c%c%c%c' date_type:'%c' dates:'%c%c%c%c%c%c%c%c' \
country_code:'%c%c%c' illus_code:'%c%c%c%c' intellectual_level:'%c' \
form_of_reproduction:'%c' nature_of_contents:'%c%c%c%c' \
government_pub_code:'%c' conference_indicator:'%c' \
festschrift_indicator:'%c' index_indicator:'%c' main_entry_in_body:'%c' \
fiction_indicator:'%c' biography_indicator:'%c' language_code:'%c%c%c' \
modified_record_code:'%c' cataloging_source:'%c' " ,
	  m->entry_date[0]/* [6] */,
	  m->entry_date[1]/* [6] */,
	  m->entry_date[2]/* [6] */,
	  m->entry_date[3]/* [6] */,
	  m->entry_date[4]/* [6] */,
	  m->entry_date[5]/* [6] */,
	  m->date_type,
	  m->dates[0]/* [8] */,
	  m->dates[1]/* [8] */,
	  m->dates[2]/* [8] */,
	  m->dates[3]/* [8] */,
	  m->dates[4]/* [8] */,
	  m->dates[5]/* [8] */,
	  m->dates[6]/* [8] */,
	  m->dates[7]/* [8] */,
	  m->country_code[0]/* [3] */,
	  m->country_code[1]/* [3] */,
	  m->country_code[2]/* [3] */,
	  m->illus_code[0]/* [4] */,
	  m->illus_code[1]/* [4] */,
	  m->illus_code[2]/* [4] */,
	  m->illus_code[3]/* [4] */,
	  m->intellectual_level,
	  m->form_of_reproduction,
	  m->nature_of_contents[0]/* [4] */,
	  m->nature_of_contents[1]/* [4] */,
	  m->nature_of_contents[2]/* [4] */,
	  m->nature_of_contents[3]/* [4] */,
	  m->government_pub_code,
	  m->conference_indicator,
	  m->festschrift_indicator,
	  m->index_indicator,
	  m->main_entry_in_body,
	  m->fiction_indicator,
	  m->biography_indicator,
	  m->language_code[0]/* [3] */,
	  m->language_code[1]/* [3] */,
	  m->language_code[2]/* [3] */,
	  m->modified_record_code,
	  m->cataloging_source );

  return(line);

}

/* tcl_escape -- substitute for square brackets for possible tcl evaluation */
char *tcl_escape (in_line)
char *in_line;
{
 char *in, *out;

 out = fieldbuffer;

 for(in = in_line; *in; in++) {
   if (*in == '[') {
     *out++ = '$';
     *out++ = '{';
     *out++ = 'L';
     *out++ = 'E';
     *out++ = 'F';
     *out++ = 'T';
     *out++ = 'B';
     *out++ = 'R';
     *out++ = 'A';
     *out++ = 'C';
     *out++ = 'K';
     *out++ = 'E';
     *out++ = 'T';
     *out++ = '}';
   }
   else if (*in == ']') {
     *out++ = '$';
     *out++ = '{';
     *out++ = 'R';
     *out++ = 'I';
     *out++ = 'G';
     *out++ = 'H';
     *out++ = 'T';
     *out++ = 'B';
     *out++ = 'R';
     *out++ = 'A';
     *out++ = 'C';
     *out++ = 'K';
     *out++ = 'E';
     *out++ = 'T';
     *out++ = '}';
   }
   else if (*in == '"') {
     *out++ = '$';
     *out++ = '{';
     *out++ = 'Q';
     *out++ = 'U';
     *out++ = 'O';
     *out++ = 'T';
     *out++ = 'E';
     *out++ = 'S';
     *out++ = '}';
   }
   else if (*in == '\n') {
     *out++ = '\\';
     *out++ = '\n';
   }
   else
     *out++ = *in;
 }
 *out = '\0';
 return (fieldbuffer);
}


int EntSub(fieldbuffer)
char *fieldbuffer;
{
     char tempbuf[FIELDBUFSIZE];
     char *tempbufptr;
     char *fldbufptr;
     
     fldbufptr = fieldbuffer;
     tempbufptr = &tempbuf[0];
     memset(tempbufptr, 0, FIELDBUFSIZE);
     
     for (;*fldbufptr != 0; fldbufptr++)
     {
	  if (*fldbufptr == '<')
	  {
	       sprintf(tempbufptr, "&lt;");
	       tempbufptr = tempbufptr + 4;
	  }
	  else
	  {
	       *tempbufptr = *fldbufptr;
	       tempbufptr++;
	  }
     }
     strcpy(fieldbuffer, tempbuf);
     return(SUCCESS);
}

char * 
xml_sub_chars(char *fieldbuffer)
{
     char *tempbuf;
     char *tempbufptr;
     char *fldbufptr;
     int buflen;
     

     if ((buflen = strlen(fieldbuffer)) == strcspn(fieldbuffer,"&<>"))
       return (fieldbuffer);

     if (buflen < 4)
       buflen = 4;

     tempbuf = CALLOC(char,(buflen * 2));

     fldbufptr = fieldbuffer;
     tempbufptr = tempbuf;
     
     for (;*fldbufptr != 0; fldbufptr++)
     {
	  if (*fldbufptr == '<')
	  {
	       sprintf(tempbufptr, "&lt;");
	       tempbufptr = tempbufptr + 4;
	  }
	  else if (*fldbufptr == '>')
	  {
	       sprintf(tempbufptr, "&gt;");
	       tempbufptr = tempbufptr + 4;
	  }
	  else if (*fldbufptr == '&')
	  {
	       sprintf(tempbufptr, "&amp;");
	       tempbufptr = tempbufptr + 5;
	  }
	  else
	  {
	       *tempbufptr = *fldbufptr;
	       tempbufptr++;
	  }
     }

     return tempbuf;

}


/*************************************************************************/
/* DispMARCfile - display a marc record given files and record ID        */
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
MARC_REC *DispMARCfile(int marcin /*file*/, int assocfile /*file*/, 
		       int recordID, DISP_FORMAT *format, int displaynum, 
		       int maxlen, void (*output)())
{
  MARC_REC *rec;
  int lrecl;

  /* find the address of the record and position the marc file pointer */
  if (assocfile && recordID > 0) {
    if (SeekMARC(marcin, assocfile, recordID) == -1)
      return(NULL);
  }
  
  /* Read the record from the marcin file into recbuffer */
  if ((lrecl = ReadMARC(marcin,recbuffer,sizeof recbuffer)) == 0)
    return(NULL);
  else {  /* record was read - so load it into the marc structure */
    if((rec = GetMARC(recbuffer,lrecl,0)) == NULL) {
      fprintf(stderr,"no record retrieved by GetMARC\n");
      return(NULL);
    }

    if (displaynum == -1) displaynum = recordID;
		
    return(FormatMARCrec(rec, format,
			displaynum, maxlen, output));
  }
}

MARC_REC *DispMARCbuff(char *recbuffer, DISP_FORMAT *format, 
		       int displaynum, int maxlen, void (*output)())
{
  MARC_REC *rec;
  int lrecl, GetNum();

  if (recbuffer == NULL || recbuffer[0] == '\0')
    return NULL;
  
  lrecl = GetNum(recbuffer,5);

  if((rec = GetMARC(recbuffer,lrecl,0)) == NULL) {
    fprintf(stderr,"no record retrieved by GetMARC\n");
    return(NULL);
  }
  /* if format is NULL, it means to convert only */
  if (format == NULL)
    return(rec);

  /* otherwise do the formatting */
  return(FormatMARCrec(rec, format,
			displaynum, maxlen, output));
}
