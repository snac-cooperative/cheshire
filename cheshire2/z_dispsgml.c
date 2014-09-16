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
 * Copyright (c) 1995-99 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Library and Information Studies, UC Berkeley
 *              Jerome McDonough
 *              School of Library and Information Studies, UC Berkeley
 *
 * Modified:
 *
 *  19-JAN-1996 Ralph Moon (class_var cluster formatting)
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
#include <stdlib.h>
#include <stdio.h>
#ifndef WIN32
#include "strings.h"
#endif
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include "cheshire.h"
#include "configfiles.h"
#define SGML_INITIALIZE
#include "dispsgml.h"
#include "dispmarc.h"

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define TAB "     "
#define TABLEN 5
#define LINEBUFSIZE 5000
#define MAXINDENT 10
#define INDENTSGML 15
#define INDENTMARC 5

 
config_file_info *cf_info_base;
FILE *LOGFILE;
extern SGML_DTD *main_dtd;

extern FORMAT_TAB FormatTable[];

extern SGML_Tag_Data *comp_tag_list(idx_key *start_key, 
				    Tcl_HashTable *tag_hash,
				    SGML_Tag_Data *parent_tags,
				    int casesensitive);

extern void outputline(void (*outfunc)(), char *in_line, int maxlen, 
		       int indent, int datatype);

extern char *detag_data_block(SGML_Data *data, int index_type, 
			      idx_list_entry *idx);

extern SGML_DTD *sgml_parse_dtd(char *filename, char *sgml_catalog_name, 
				char *schema_file_name, filelist *fl,  int dtd_type);

extern SGML_Document *sgml_parse_document(SGML_DTD *dtd, char *filename, 
					  char *buffer, 
					  int recordnum, int keep_buffer);

/* DispSGMLBuff -- Print out sgml records to screen display */


SGML_Document *DispSGMLbuff(char *recbuffer, char *dtd_file, char *format, 
			    long displaynum, long maxlen, void (*output)(), 
			    char *filename)
{

  SGML_Document *FormatSGMLrec();
  SGML_Document *IndentSGMLrec();
  SGML_Document *record;
  SGML_DTD *record_dtd;
  FORMAT_TAB *ft, *found_ft;
  DISP_FORMAT *user_format, *f;
  int num_format_entries;
  
  char *doc_type;
  char *doc_format;
  int use_format = 0;
  int i;
  
  /* We need to:   */
  /* 1. parse the dtd_file                                       */
  /*    NOTE: the second parameter to sgml_parse_dtd should be   */
  /*          the name of the SGML catalog mapping from PUBLIC   */
  /*          names, etc. to SYSTEM names. We are currently      */
  /*          assuming that there are NO PUBLIC names in the DTD */
  /*          The THIRD parameter is the of DTD:                 */
  /*              0 = SGML, 1 = XML, 2 = XMLSchema               */
  /* need to make the third parameter a variable...              */
  if ((record_dtd = sgml_parse_dtd(dtd_file,NULL, NULL, NULL, 0)) == NULL) {
    outputline(output, "DTD couldn't be parsed in zformat.\n\0", maxlen, 0,0);
    return (NULL);
  }
  main_dtd = record_dtd;
  
  /* 2. for each record in recbuffer */
  /* parse the record using parsed DTD */
  /* if DTD has matching style table, use that to format record */
  /* and print it to output */
  /* if not, format record using tab indenting for each level */
  /* of SGML hierarchy in document, and print to output */
  if (( record = sgml_parse_document(record_dtd, filename, recbuffer, 0, 1)) == NULL) {
    outputline(output, "Could not parse this record for display.\n\0", maxlen, 0,0);
    return (NULL);
  }else {
    
    for (i = 0; i < NUM_FORMATS; i++) {
      doc_type = known_formats[i].dtd_name;
      doc_format = known_formats[i].format_name;
      
      if (strcasecmp(record_dtd->DTD_name, doc_type) == 0) {
	if (strcasecmp(format, doc_format) == 0) {
	  use_format = 1;
	  break;
	}
      }
    }
    if (use_format == 1) {
      /* Need to develop formating section here */
      return(FormatSGMLrec(record, known_formats[i], displaynum, maxlen, output));
    } 
    else {  
      /* check for user_defined format and set it up */
      user_format = NULL;
      found_ft = NULL;
      USER_DEFINED_FORMAT = NULL;
      
      for (ft = FormatTable; ft->formatname[0] != '\0'; ft++) {
	if (strcasecmp(format, ft->formatname) == 0) {
	  if (strcasecmp(record_dtd->DTD_name, ft->DTD_name) == 0
	      || strcasecmp(record_dtd->DTD_file_name, ft->DTD_name) == 0) {
	    user_format = ft->format_ptr;
	    USER_DEFINED_FORMAT = ft->sgml_format_ptr;
	    found_ft = ft;
	  }
	}
      }
      
      if (user_format == NULL) {
	return(IndentSGMLrec(record, displaynum, maxlen, output));
      }
      else {
	/* set up the user-defined format */
	SGML_FORMAT_LIST newlist;
	strcpy(newlist.dtd_name, found_ft->DTD_name);
	strcpy(newlist.format_name, found_ft->formatname);
	newlist.format_id = 99;
	num_format_entries = 0;
	if (USER_DEFINED_FORMAT == NULL) {
	  /* build the format and install in the formattable */
	  for (f = user_format; f->newfield >=0 ; f++) {
	    num_format_entries++;
	  }
	  USER_DEFINED_FORMAT = CALLOC(SGML_FORMAT, num_format_entries+1);
	  i = 0;
	  for (f = user_format; f->newfield >=0 ; f++) {
	    USER_DEFINED_FORMAT[i].label = f->label;
	    USER_DEFINED_FORMAT[i].elements = f->tags;
	    USER_DEFINED_FORMAT[i].subelmts = f->subfields;
	    USER_DEFINED_FORMAT[i].elsep = f->subfsep;
	    USER_DEFINED_FORMAT[i].beginpunct = f->beginpunct;
	    USER_DEFINED_FORMAT[i].endpunct = f->endpunct;
	    if (f->multisubstitute)
	      USER_DEFINED_FORMAT[i].special_proc = 2;
	    else 
	      USER_DEFINED_FORMAT[i].special_proc = 0;
	    

	    i++;
	  }	
	  found_ft->sgml_format_ptr;
	}
	return(FormatSGMLrec(record, newlist, displaynum, maxlen, output));
	
      }
    }
  }
}

SGML_Document *FormatSGMLrec(SGML_Document *record, 
			     SGML_FORMAT_LIST doc_format, 
		       long displaynum, long maxlen, void (*output)())
{
     SGML_FORMAT *fmt, *f;
     char *linebuffer;
     int repeat = FALSE;
     long real_max_len;
     
     if (record == NULL)
	  return NULL;

     if ((linebuffer = CALLOC(char, strlen(record->buffer))) == NULL) {
       fprintf(stderr, "FormatSGMLrec: couldn't calloc sufficient memory to guarantee successful handling of large lines.  Winging it...\n");
       linebuffer = CALLOC(char, LINEBUFSIZE);
     }
     
     switch (doc_format.format_id)
     {
     case 1: fmt = &USMARC_review[0];
             real_max_len = maxlen;
	     break;
     case 2: fmt = &USMARC_short[0];
             real_max_len = maxlen;
	     break;
     case 3: fmt = &USMARC_long[0];
             real_max_len = maxlen;
	     break;
     case 4: fmt = &LCCLUST_lccshort[0];
             real_max_len = maxlen;
	     break;
     case 5: fmt = &USMARC_marc[0];
             real_max_len = maxlen;
	     break;
     case 6: fmt = &USMARC_HTMLreview[0];
             real_max_len = 10000;
	     break;
     case 7: fmt = &USMARC_HTMLshort[0];
             real_max_len = 10000;
	     break;
     case 8: fmt = &USMARC_HTMLlong[0];
             real_max_len = 10000;
	     break;
     case 9: fmt = &USMARC_CSMP_HTMLreview[0];
             real_max_len = 10000;
	     break;
     case 10: fmt = &USMARC_CSMP_HTMLshort[0];
             real_max_len = 10000;
	     break;
     case 11: fmt = &USMARC_CSMP_HTMLlong[0];
             real_max_len = 10000;
	     break;
     case 12: fmt = &FT931_review[0];
             real_max_len = 10000;
	     break;
     case 13: fmt = &FT931_short[0];
             real_max_len = 10000;
	     break;
     case 14: fmt = &FT931_long[0];
             real_max_len = 10000;
	     break;
     case 15: fmt = &USMARC_GLAS_HTMLreview[0];
             real_max_len = 10000;
	     break;
     case 16: fmt = &USMARC_GLAS_HTMLshort[0];
             real_max_len = 10000;
	     break;
     case 17: fmt = &USMARC_GLAS_HTMLlong[0];
             real_max_len = 10000;
	     break;
	    
     case 99: fmt = USER_DEFINED_FORMAT;
             real_max_len = maxlen;
	     break;
     }  
	  
     for (f = fmt; f->label; f++) {
       FormatSGMLel(f, doc_format.format_id, record, linebuffer, 
		    displaynum, real_max_len, &repeat, output);
       repeat = FALSE;
     }

     free(linebuffer);
     
     return(record);
}

int FormatSGMLel(SGML_FORMAT *format, int format_id, SGML_Document *doc,
			    char *linebuffer, long displaynum, long maxlen,
			    int *repeat, void (*output)())
{
     char *linebufptr;
     char *tokensep = " ";
     char *elements;
     char *s_elem;
     char *saveptr;
     int casesensitive;

     SGML_Tag_Data *td, *tag_data;
     idx_key start_key;

     if (format->elements[0] == '+') {  
       /* If the elements is a plus then this prints only the label */
       /* at the point encountered -- regardless of any data        */
       sprintf(linebuffer, "%s ", format->label);
       outputline(output, linebuffer, maxlen, INDENTSGML,1);
     }
     else if (format->elements[0] == '#') {  
       /* If the elements is a pound sign then this prints only the label */
       /* and the "displaynum" at the point encountered                   */
       sprintf(linebuffer, "%s%s%d%s", format->label, format->beginpunct,
	       displaynum, format->endpunct);
       outputline(output, linebuffer, maxlen, INDENTSGML,1);
     }

     else {
       
       
       /* for each element listed in format.elements, get all of the matching
	*  elements and if any are found call print function to
	*  dump out appropriate items for this element. 
	*/
       
       linebufptr = linebuffer;
       memset (linebuffer, 0, sizeof(linebuffer));
       elements = CALLOC(char, LINEBUFSIZE);
       
       strcpy(elements, format->elements);
       
       s_elem = strtok_r(elements, tokensep, &saveptr);
       start_key.subkey = NULL;
       start_key.next_key = NULL;
       start_key.occurrence = 0;
       start_key.attribute_flag = 0;

       if (doc->DTD->type > 0) 
	 casesensitive = 1;
       else
	 casesensitive = 0;

       while (s_elem != NULL) {
	 start_key.key = s_elem;
	 
	 tag_data = comp_tag_list(&start_key, doc->Tag_hash_tab, NULL,
				  casesensitive);

	 for (td = tag_data; td ; td = td->next_tag_data) {

	   PrintSGMLel(format, format_id, td->item, linebuffer, 
		       displaynum, maxlen, repeat, output);
	   
	   if (format_id != 5) {
	     *repeat = TRUE;
	   }
	   /* clear the buffer for the next item */
	   memset (linebuffer, 0, sizeof(linebuffer));
	 }
	 s_elem = strtok_r(NULL, tokensep, &saveptr);
       }
       free(elements);
       /* free the tag data */
       free_tag_data_list(tag_data);
     }
     return(SUCCESS);
}

int PrintSGMLel(SGML_FORMAT *format, int format_id, SGML_Data *dataptr, 
		char *linebuffer, long displaynum, long maxlen, int *repeat, 
		void (*output)())
{
  /* Function to print an SGML data element to Tk text widget via
     output function */
  
  char *linebufptr;
  int datalen = 0;
  char *databuf = NULL;
  char *databufptr = NULL;
  char *datatag;
  static char *tokensep = " ";
  char *subelements;
  char *cur_subel;
  SGML_Data *subdataptr;
  SGML_Attribute_Data *attribptr;
  int i;
  char attribs[3];
  char tagname[4];
  char *tmpstr;
  char *saveptr;
  char *conv_data = NULL;
  char *substitute;
  char blankfillchar;

  SGML_Tag_Data *td, *tag_data;
  
  subdataptr = dataptr->sub_data_element;
  linebufptr = linebuffer;
  subelements = CALLOC(char,LINEBUFSIZE);
  strcpy (subelements, format->subelmts);
  
  if (*repeat == FALSE) {
    sprintf(linebufptr, "%s", format->label);
    linebufptr += (strlen(format->label));
  } else {
    if (format_id < 6 || format_id > 11) {
      sprintf(linebufptr, "                ");
      linebufptr += INDENTSGML;
    }
    else { /* formats 6,7,8,9,10 & 11 are for HTML */
      /* -- indentation is done as a <DL>*/
      sprintf(linebufptr,"<DD> ");
      linebufptr += 5;
    }
  }
  /* set up for multiple substitutions... */
  substitute = strchr(format->beginpunct, '%');
  if ((format->special_proc & 2) && substitute != NULL) {
    if (*(substitute+1) == 's')
      blankfillchar = ' ';
    else {
      blankfillchar = *(substitute+1);
      *(substitute+1) = 's';
    }
  }

  
  /* if dealing with a MARC display, print the indicators */
  if (format_id == 5) {
    memset(attribs, 0, 3);
    memset(tagname, 0, 4);
    tmpstr = dataptr->element->tag;
    tmpstr += 3;
    strncpy(tagname, tmpstr, 3);
    attribptr = dataptr->attributes;
    if (attribptr != NULL) {
      s2mindi(tagname, attribptr, attribs);
      sprintf(linebufptr, "%s", attribs);
      linebufptr += (strlen(attribs));
    }
  }
  
  /* if dealing with a TREC Financial Times display, rip out the */
  /* hard returns of all fields EXCEPT THE FULL TEXT */
  if ((format_id > 11) && (format_id < 16) 
      && (strncmp(format->label, "TEXT:",5) != 0)) {
    for (tmpstr = dataptr->content_start, i = 0; i <  datalen; i++) {
      if (tmpstr[i] == '\n') {
	tmpstr[i] = ' ';
      }
    }
  }

  /* if there's no specific subelements to print, just print entirety
     of data for element; otherwise, copy data for all listed subelements
     to linebuffer for printing */
  if (*format->subelmts == '\0') {
    conv_data = detag_data_block(dataptr, (NORM_NOMAP | NORM_MIN), NULL);
    datalen = strlen(conv_data);
    if (format->special_proc == 1) {
      for (i = 0; i < datalen; i++) {
	if (conv_data[i] == ' ')
	  conv_data[i] = '0';
	if (conv_data[i] == '/')
	  conv_data[i] = '-';
      }
    }
    /* print the beginning punctuation for the field */
    if ((format->special_proc == 2) && substitute != NULL) {
      char *tmp;
      if (blankfillchar != ' ') {
	char *c;
	tmp = strdup(conv_data);
	for(c = tmp; *c ; c++)
	  if (*c == ' ') *c = blankfillchar;
      }
      else
	tmp = conv_data;
      
      sprintf(linebufptr, format->beginpunct, tmp);
      /* restore the format for the next item */
      if (blankfillchar != ' ')
	*(substitute+1) = blankfillchar;
      linebufptr += strlen(linebufptr);
      if (tmp != conv_data)
	FREE(tmp);
    }
    else {
      sprintf(linebufptr, "%s", format->beginpunct);
      linebufptr += strlen(format->beginpunct);
      sprintf(linebufptr, "%s", conv_data);
      linebufptr += (strlen(conv_data));
    }
  } 
  else {
    datalen = dataptr->end_tag_offset - dataptr->content_start_offset;
    databuf = databufptr = CALLOC(char,(datalen * 2));

    for (subdataptr; subdataptr != NULL; subdataptr = subdataptr->next_data_element) {
      datatag = subdataptr->element->tag;
      cur_subel = strtok_r(subelements, tokensep, &saveptr);
      while (cur_subel != NULL)	{
	if (strlen(cur_subel) == 1 && cur_subel[0] == '*')
	  cur_subel = ".*";

	if (match(cur_subel, datatag,0)) { /* case insensitive match */
	  /*copy over the data to linebuffer*/
	  conv_data = detag_data_block(subdataptr, (NORM_NOMAP | NORM_MIN), NULL);
	  if (conv_data != NULL) {
	    datalen = strlen(conv_data);
	    if (datalen > 0) {
	      if (format->special_proc == 1) {
		for (i = 0; i < datalen; i++) {
		  if (conv_data[i] == ' ')
		    conv_data[i] = '0';
		  if (conv_data[i] == '/')
		    conv_data[i] = '-';
		}
	      }
	    }
	 
	    if (format_id == 5) {
	      sprintf(databufptr, "$%c ", (char) tolower(pSFnorm(datatag)));
	      databufptr += 3;
	    }

	    sprintf(databufptr, "%s", conv_data);
	    databufptr += (strlen(conv_data));

	    if (subdataptr->next_data_element != NULL) {
	      sprintf(databufptr, "%s", format->elsep);
	      databufptr += strlen(format->elsep);
	    }
	  }
	}
	cur_subel = strtok_r(NULL, tokensep, &saveptr);
      }
      if (conv_data)
	FREE(conv_data);
      conv_data = NULL;
      memset (subelements, 0, LINEBUFSIZE);
      strcpy (subelements, format->subelmts);
    }
    conv_data = databuf;
    /* print the beginning punctuation for the field */
    if ((format->special_proc == 2) && substitute != NULL) {
      char *tmp;
      if (blankfillchar != ' ') {
	char *c;
	tmp = strdup(conv_data);
	for(c = tmp; *c ; c++)
	  if (*c == ' ') *c = blankfillchar;
      }
      else
	tmp = conv_data;
      
      sprintf(linebufptr, format->beginpunct, tmp);
      /* restore the format for the next item */
      if (blankfillchar != ' ')
	*(substitute+1) = blankfillchar;
      linebufptr += strlen(linebufptr);
      if (tmp != conv_data)
	FREE(tmp);
    }
    else {
      sprintf(linebufptr, "%s", format->beginpunct);
      linebufptr += strlen(format->beginpunct);
      sprintf(linebufptr, "%s", conv_data);
      linebufptr += (strlen(conv_data));
    }
    
  }

  /* set up for multiple substitutions... */
  substitute = strchr(format->endpunct, '%');
  if ((format->special_proc & 2) && substitute != NULL) {
    char *tmp;
    if (*(substitute+1) == 's')
      blankfillchar = ' ';
    else {
      blankfillchar = *(substitute+1);
      *(substitute+1) = 's';
    }

    if (blankfillchar != ' ') {
      char *c;
      tmp = strdup(conv_data);
      for(c = tmp; *c ; c++)
	if (*c == ' ') *c = blankfillchar;
    }
    else
      tmp = conv_data;

    sprintf(linebufptr, format->endpunct, tmp);
    linebufptr += strlen(linebufptr);
    /* restore the format for the next item */
    if (blankfillchar != ' ')
      *(substitute+1) = blankfillchar;
    if (tmp != conv_data)
      FREE(tmp);
  } 
  else {
    sprintf(linebufptr, "%s\n",format->endpunct);
  }

  if (format_id == 5) {
    outputline(output, linebuffer, maxlen, INDENTMARC,1);
  } else if  ((format_id > 11) && (format_id < 16) 
	      && ((strncmp(format->label, "TEXT:", 5)) == 0)) {
    outputline(output, linebuffer, 50000, 0,1);
  } else {
    outputline(output, linebuffer, maxlen, INDENTSGML,1);
  }
  
  if (conv_data)
    FREE(conv_data);

  if (subelements)
    FREE(subelements);

  if (databuf && databuf != conv_data)
    FREE(databuf);

  return(SUCCESS);
  
}

SGML_Document *IndentSGMLrec(SGML_Document *record, long displaynum, 
			     long maxlen, void (*output)())
{

     SGML_Data *treetop;
     SGML_Data *sgmltree_disp();
     char *linebuffer;
     int indentlevel = 0;
     
     treetop = record->data;
     
     if (record == NULL)
	  return NULL;

     if ((linebuffer = CALLOC(char, strlen(record->buffer))) == NULL) {
       fprintf(stderr, "IndentSGMLrec: couldn't calloc sufficient memory to guarantee successful line handling for large lines.  Winging it...\n");
       linebuffer = CALLOC(char,LINEBUFSIZE);
     }

     sgmltree_disp(treetop, linebuffer, maxlen, &indentlevel, output);
     free(linebuffer);
     
     return(record);
}

SGML_Data *sgmltree_disp(SGML_Data *dataptr, char *linebuffer, long maxlen,
		    int *indentlevel, void (*output)())
{
  /* Big Fun!  Watch the Wild Recursive Descent Seg Fault again & again! */
  /* General idea is the walk down through tree, printing start tags */
  /* as we go, until we hit bottom.  Then print data.  Then move */
  /* horizontally if possible (and down if necessary) until there */
  /* ain't no more.  Back out of recursion to point where we hit */
  /* bottom, go up a level, and see if we need to move sideways, etc., */
  /* until done.  Between theory and practice lies the shadow. */
  SGML_Data *tmpdataptr;
  int staglen = 0;
  int etaglen = 0;
  int datalen = 0;
  int tmplen = 0;
  char *stagbuf = NULL;
  char *etagbuf = NULL;
  char *databuf = NULL;
  char *contentptr = NULL;
  int indent = MAXINDENT;
  char *linebufptr;
  int i;
  
  LOGFILE = stderr;
     
  if (dataptr == NULL) {
    fprintf(stderr, "sgmltree_disp: There's trouble in river city. NULL dataptr\n");
    return NULL;
  }

     
  if (*indentlevel <= MAXINDENT) {
    indent = *indentlevel;
  }

  linebufptr = linebuffer;
  memset(linebuffer, 0, sizeof(linebuffer));
  
  /* get lengths of start tag, end tag, and data for current element */
  staglen = dataptr->content_start_offset - dataptr->start_tag_offset;
  etaglen = dataptr->data_end_offset - dataptr->content_end_offset;
  datalen = dataptr->end_tag_offset - dataptr->content_start_offset;
     
  /* malloc buffer for start tag, and copy tag into buffer from SGML tree */
  if (staglen > 0) {
    stagbuf = CALLOC(char, (staglen + 1));
    strncpy (stagbuf, dataptr->start_tag, staglen);
  }
  else {
    /* we have an empty tag with XML notation */
    if (dataptr->next_data_element != NULL) {
      staglen = dataptr->next_data_element->start_tag_offset 
	- dataptr->start_tag_offset;
      etaglen = 0;
      datalen = 0;
      stagbuf = CALLOC(char, (staglen + 1));
      strncpy (stagbuf, dataptr->start_tag, staglen);
    } else {
      /* no next element -- use parent info */
      staglen = dataptr->parent->end_tag_offset 
	- dataptr->start_tag_offset;
      etaglen = 0;
      datalen = 0;
      stagbuf = CALLOC(char, (staglen + 1));
      strncpy (stagbuf, dataptr->start_tag, staglen);
    }
       
  }
  /* print appropriate number of tabs to indent, then print start tag, */
  /* and increment indentlevel */
  for (i = 0; i < indent; i++)   {
    sprintf(linebufptr, "%s", TAB);
    linebufptr += TABLEN;
  }
  sprintf(linebufptr, "%s\n", stagbuf);
  outputline(output, linebuffer, maxlen, (indent * TABLEN),1);

  ++*indentlevel;

  /* clean up */
  if (*indentlevel <= MAXINDENT) {
    indent = *indentlevel;
  }
  memset(linebuffer, 0, sizeof(linebuffer));
  linebufptr = linebuffer; 
     
  /*if this element has a sub-element, do recursive call for sub_element */
  if (dataptr->sub_data_element != NULL) {
    /* print any pcdata up the subelement */
    tmplen = dataptr->sub_data_element->start_tag_offset 
      - dataptr->content_start_offset;
    if (tmplen > 0) {
      databuf = CALLOC(char,(tmplen + 1));
      strncpy(databuf, dataptr->content_start, tmplen);
      for (i = 0; i < indent; i++) {
	sprintf(linebufptr, "%s", TAB);
	linebufptr += TABLEN;
      }
      sprintf(linebufptr, "%s\n", databuf);
      outputline(output, linebuffer, maxlen, (indent * TABLEN),1);
    }
    if (databuf) FREE(databuf);
    databuf = NULL;
    
    sgmltree_disp(dataptr->sub_data_element, linebuffer, maxlen, indentlevel, output);


    /*  print data after the element... */
    for (tmpdataptr = dataptr->sub_data_element; 
	 tmpdataptr->next_data_element != NULL;
	 tmpdataptr = tmpdataptr->next_data_element) ;
    tmplen = dataptr->content_end_offset 
      - tmpdataptr->data_end_offset; 
    if (tmplen > 0 && tmpdataptr->content_start_offset > 0) {
      linebufptr = linebuffer; 
      databuf = CALLOC(char,(tmplen + 1));
      strncpy(databuf, tmpdataptr->data_end, tmplen);
      for (i = 0; i < indent; i++) {
     	sprintf(linebufptr, "%s", TAB);
     	linebufptr += TABLEN;
      }
      sprintf(linebufptr, "%s\n", databuf);
      outputline(output, linebuffer, maxlen, (indent * TABLEN),1);
    }
    if (databuf) FREE(databuf);
    databuf = NULL;
    
  }
  else {
    /*if no sub-elements here, there should be real data (ie. PCDATA) */
    /* so print it (relying on outputline to handle linebreaks, etc.), */
    /* then decrement indentlevel, and print end tag */
    databuf = CALLOC(char,(datalen + 1));
    strncpy(databuf, dataptr->content_start, datalen);
    contentptr = dataptr->content_start;
    for (i = 0; i < indent; i++) {
      sprintf(linebufptr, "%s", TAB);
      linebufptr += TABLEN;
    }
    sprintf(linebufptr, "%s\n", databuf);
    outputline(output, linebuffer, maxlen, (indent * TABLEN),1);
    
  }
  --*indentlevel;
  
  /* clean up */
  if (*indentlevel <= MAXINDENT) {
    indent = *indentlevel;
  }
  memset(linebuffer, 0, sizeof(linebuffer));
  linebufptr = linebuffer; 
  
  /* malloc buffer for end tag; copy tag into buffer from SGML tree */
  if (etaglen > 0) {
    etagbuf = CALLOC(char,(etaglen + 1));
    strncpy(etagbuf, dataptr->end_tag, etaglen);
  }
  else {    
    etagbuf = CALLOC(char,(staglen + 2));
    strcpy(etagbuf, "/");
    strncat(etagbuf,dataptr->start_tag,staglen); 
  }
  /* print appropriate number of tabs to indent, then print end tag */
  for (i = 0; i < indent; i++) {
    sprintf(linebufptr, "%s", TAB);
    linebufptr += TABLEN;
  }
  sprintf(linebufptr, "%s\n", etagbuf);
  outputline(output, linebuffer, maxlen, (indent * TABLEN),1);
  
  /* clean up */
  if (*indentlevel <= MAXINDENT) {
    indent = *indentlevel;
  }
  memset(linebuffer, 0, sizeof(linebuffer));
  linebufptr = linebuffer; 
  
  /* if this element points to another element at the same level, */
  /* do recursive call for next element */
  if ((dataptr->next_data_element != NULL) && (dataptr->parent != NULL)) {

    /* first put out anything from the parent that is after the end tag */
    /* and before the beginning of the next element                     */
    tmplen = dataptr->next_data_element->start_tag_offset 
      - dataptr->data_end_offset;
    if (tmplen > 0 && dataptr->content_start_offset > 0) {
      if (databuf) FREE(databuf);
      databuf = NULL;
      databuf = CALLOC(char,(tmplen + 1));
      strncpy(databuf, dataptr->data_end, tmplen);
      for (i = 0; i < indent; i++) {
	sprintf(linebufptr, "%s", TAB);
	linebufptr += TABLEN;
      }
      sprintf(linebufptr, "%s\n", databuf);
      outputline(output, linebuffer, maxlen, (indent * TABLEN),1);
    }

    sgmltree_disp(dataptr->next_data_element, linebuffer, maxlen, indentlevel, output);
  }
     
  /* free remaining buffers */
  if (stagbuf) free(stagbuf);
  if (databuf) free(databuf);
  if (etagbuf) free(etagbuf);
  
  return(dataptr);

}




int tagmatch(char *tag1, char *tag2)
{
     /* function to compare with tag2 matches with tag1; tag1 can
	include ?-style wild cards; a tag1 of '*' matches anything */

     int i, taglen;
     
     taglen = strlen(tag1);

     if (tag1[0] == '*')
	  return(TRUE);
     
     for (i = 0; i < taglen; i++)
     {
	  if (tag1[i] == '?')
	       continue;
	  if (toupper(tag1[i]) == toupper(tag2[i]))
	    continue;
	  else
	       return(FALSE);
     }
     return(TRUE);
     
}

int normalize_char(char *buffer, char **buffer2)
{
  
     /* function which crawls through a buffer looking for USMARC */
     /* ANSEL (upper 128) characters, converting any diacritic-   */
     /* letter pair to the appropriate single character within    */
     /* ISO8859-1 (latin 1) where possible, and converting others */
     /* as appropriate.  8-bit char sets delenda est!             */
     /* Note that this is for DISPLAY PURPOSES ONLY!!!            */
     /* Indexing on server side gets rid of diacritics completely */

     char *c, t, *normedbuf;
     int bufsize, i;
     unsigned char u, v;
     int badchar = FALSE;
     
     /* allocate buffer to hold normalized string */
     /* note that we're being extra cautious with space here */
     if (buffer == NULL) return 0;

     bufsize = strlen(buffer);
     normedbuf = CALLOC(char, ((2 * bufsize) + 1));
     
     /* troll through the buffer, copying normal ASCII to the */
     /* normalized buffer, and handling the MARC weird chars  */
     /* in the upper 128 */
     for (i = 0, c = buffer; *c && c < (buffer + bufsize); c++, i++)
     {
	  badchar = FALSE;
	  t = u = *c;
	  if (u > 128) 
	  {
	       if (u < 254) 
	       {
		    switch (u) 
		    {
		    /* Polish L, Upper Case, changed to L */
		    case 161:
			 t = 'L';
			 break;
		    /* Scandinavian O, Upper Case */
		    case 162:
			 t = 216;
			 break;
		    /* D with crossbar, Upper Case */
		    case 163:
			 t = 208;
			 break;
		    /* Icelandic Thorn, Upper Case */ 
		    case 164:
			 t = 222;
			 break;
		    /* Digraph AE, Upper Case */
		    case 165:
			 t = 198;
			 break;
		    /* Digraph OE, Upper Case, changed to OE */
		    case 166:
			 normedbuf[i++] = 'O';
			 t = 'E';
			 break;
		    /* Soft sign, changed to underscore */ 
		    case 167:
			 t = '_';
			 break;
		    /* Dot in middle of line, changed to underscore */
		    case 168:
			 t = '_';
			 break;
		    /* Musical flat, changed to underscore */
		    case 169:
			 t = '_';
			 break;
		    /* Registered trademark */
		    case 170:
			 t = 174;
			 break;
		    /* Plus or Minus */
		    case 171:
			 t = 177;
			 break;
		    /* O-hook, changed to plain O */
		    case 172:
			 t = 'O';
			 break;
		    /* U-hook, changed to plain U */
		    case 173:
			 t = 'U';
			 break;
		    /* Alif, changed to underscore */
		    case 174:
			 t = '_';
			 break;
		    /* Undefined, changed to underscore */
		    case 175:
			 t = '_';
			 break;
		    /* Ayn, changed to underscore */
		    case 176:
			 t = '_';
			 break;
		    /* Polish l, lower case, changed to l */
		    case 177:
			 t = 'l';
			 break;
		    /* Scandinavia o, lower case */
		    case 178:
			 t = 248;
			 break;
		    /* d with crossbar, lower case, changed to d */
		    case 179:
			 t = 'd';
			 break;
		    /* Icelandic thorn, lower case */
		    case 180:
			 t = 254;
			 break;
		    /* Digraph ae, lower case */
		    case 181:
			 t = 230;
			 break;
		    /* Digraph oe, lower case, changed to oe */
		    case 182:
			 normedbuf[i++] = 'o';
			 t = 'e';
			 break;
		    /* Hard sign, changed to underscore */
		    case 183:
			 t = '_';
			 break;
		    /* Turkish i, lower case */
		    case 184:
			 t = 'i';
			 break;
		    /* British pound */
		    case 185:
			 t = 163;
			 break;
		    /* Eth (used as Icelandic D) */
		    case 186:
			 t = 240;
			 break;
		    /* undefined, changed to underscore */
		    case 187:
			 t = '_';
			 break;
		    /* o-hook, lower case */
		    case 188:
			 t = 'o';
			 break;
		    /* u-hook, lower case */
		    case 189:
			 t = 'u';
			 break;
		    /* Grave accent... */
		    case 225:
			 c++;
			 v = *c;
			 switch (v)
			 {
			 /* with A */
			 case 65:
			      t = 192;
			      break;
			 /* with E */
			 case 69:
			      t = 200;
			      break;
			 /* with I */
			 case 73:
			      t = 204;
			      break;
			 /* with O */
			 case 79:
			      t = 210;
			      break;
			 /* with U */
			 case 85:
			      t = 217;
			      break;
			 /* with a */
			 case 97:
			      t = 224;
			      break;
			 /* with e */
			 case 101:
			      t = 232;
			      break;
			 /* with i */
			 case 105:
			      t = 236;
			      break;
			 /* with o */
			 case 111:
			      t = 242;
			      break;
			 /* with u */
			 case 117:
			      t = 249;
			      break;
			 /* with anything else, get rid of Grave */
			 default:
			      c--;
			      badchar = TRUE;
			 }
			 break;
		    /* Acute accent... */
		    case 226:
			 c++;
			 v = *c;
			 switch (v) 
			 {
			 /* with A */
			 case 65:
			      t = 193;
			      break;
			 /* with E */
			 case 69:
			      t = 201;
			      break;
			 /* with I */
			 case 73:
			      t = 205;
			      break;
			 /* with O */
			 case 79:
			      t = 211;
			      break;
			 /* with U */
			 case 85:
			      t = 218;
			      break;
			 /* with a */
			 case 97:
			      t = 225;
			      break;
			 /* with e */
			 case 101:
			      t = 233;
			      break;
			 /* with i */
			 case 105:
			      t = 237;
			      break;
			 /* with o */
			 case 111:
			      t = 243;
			      break;
			 /* with u */
			 case 117:
			      t = 250;
			      break;
			 /* with anything else, get rid of Acute */
			 default:
			      c--;
			      badchar = TRUE;
			 }
			 break;
		    /* Circumflex */
		    case 227:
			 c++;
			 v = *c;
			 switch (v) 
			 {
			 /* with A */
			 case 65:
			      t = 194;
			      break;
			 /* with E */
			 case 69:
			      t = 202;
			      break;
			 /* with I */
			 case 73:
			      t = 206;
			      break;
			 /* with O */
			 case 79:
			      t = 212;
			      break;
			 /* with U */
			 case 85:
			      t = 219;
			      break;
			 /* with a */
			 case 97:
			      t = 226;
			      break;
			 /* with e */
			 case 101:
			      t = 234;
			      break;
			 /* with i */
			 case 105:
			      t = 238;
			      break;
			 /* with o */
			 case 111:
			      t = 244;
			      break;
			 /* with u */
			 case 117:
			      t = 251;
			      break;
			 /* with anything else, get rid of circumflex */
			 default:
			      c--;
			      badchar = TRUE;
			 }
			 break;
		    /* Tilde */
		    case 228:
			 c++;
			 v = *c;
			 switch (v) 
			 {
			 /* with A */
			 case 65:
			      t = 195;
			      break;
			 /* with N */
			 case 78:
			      t = 209;
			      break;
			 /* with O */
			 case 79:
			      t = 213;
			      break;
			 /* with a */
			 case 97:
			      t = 227;
			      break;
			 /* with n */
			 case 110:
			      t = 241;
			      break;
			 /* with o */
			 case 111:
			      t = 245;
			      break;
			 /* with anything else, get rid of tilde */
			 default:
			      c--;
			      badchar = TRUE;
			      break;
			 }
			 break;
		    /* Umlaut (diaeresis) */
		    case 232:
			 c++;
			 v = *c;
			 switch (v) 
			 {
			 /* with A */
			 case 65:
			      t = 196;
			      break;
			 /* with E */
			 case 69:
			      t = 203;
			      break;
			 /* with I */
			 case 73:
			      t = 207;
			      break;
			 /* with O */
			 case 79:
			      t = 214;
			      break;
			 /* with U */
			 case 85:
			      t = 220;
			      break;
			 /* with a */
			 case 97:
			      t = 228;
			      break;
			 /* with e */
			 case 101:
			      t = 235;
			      break;
			 /* with i */
			 case 105:
			      t = 239;
			      break;
			 /* with o */
			 case 111:
			      t = 246;
			      break;
			 /* with u */
			 case 117:
			      t = 252;
			      break;
			 /* with y */
			 case 121:
			      t = 255;
			      break;
			 /* with anything else, get rid of umlaut */
			 default:
			      c--;
			      badchar = TRUE;
			      break;
			 }
			 break;
		    /* Hacek */
		    case 233:
			 c++;
			 v = *c;
			 switch (v) 
			 {
			 /* with A */
			 case 65:
			      t = 197;
			      break;
			 /* with anything else, get rid of hacek */
			 default:
			      c--;
			      badchar = TRUE;
			      break;
			 }
			 break;
		    /* Circle above (Angstrom) */
		    case 234:
			 c++;
			 v = *c;
			 switch (v)
			 {
			 /* with a */
			 case 97:
			      t = 229;
			      break;
			 /* with anything else, get rid of Angstrom */
			 default:
			      c--;
			      badchar = TRUE;
			      break;
			      
			 }
			 break;
		    /* Cedilla */
		    case 240:
			 c++;
			 v = *c;
			 switch (v)
			 {
			 /* with C */
			 case 67:
			      t = 199;
			      break;
			 /* with c */
			 case 99:
			      t = 231;
			      break;
			 /* with anything else, get rid of cedilla */
			 default:
			      c--;
			      badchar = TRUE;
			      break;
			 }
			 break;
			 
		    default:
			 badchar = TRUE;
		    }
		    normedbuf[i] = t;
		    if (badchar == TRUE)
			 i--;
	       }
	       
	  } else
	       normedbuf[i] = t;
     }
     
     /* send back the newly allocated buffer */
     *buffer2 = normedbuf;
     
}

