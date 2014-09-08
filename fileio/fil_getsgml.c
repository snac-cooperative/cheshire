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
/*****************************************************************************
*
*   Function Name:	getsgml.c
*
*   Programmer:  Ray Larson
*
*   Purpose:
*
*   Usage:    char *GetRawSGML(filename,record_number);
*             SGML_Data *GetSGML(filename,record_number);
*
*   Variables: filename is the nickname or full path of the file
*              containing the record (must be in the configfile)
*              record_number is the logical record number for the
*              record.
*
*
*   Return values and return codes:
*           GetRawSGML
*              returns a pointer to the raw sgml record in
*              an allocated buffer if successful or NULL on error.
*
*           GetSGML
*              returns a pointer to the SGML_Document structure for the 
*              record if successful or NULL on error.
*
*         Date:     9/3/94
*         Revised:  
*         Version:  1.0
*
* Copyright (c) 1994 The Regents of the University of California.
* All rights reserved.
*
* Author:	Ray Larson, ray@sherlock.berkeley.edu
*		Jerome McDonough jmcd@lucien.berkeley.edu
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
*****************************************************************************/

#include <stdlib.h>
#include <cheshire.h>
#include <dmalloc.h>

#ifndef WIN32
extern int errno;
#endif

extern SGML_Document *FetchSGMLDataStore(char *filename, int recordnum);
extern char *FetchSGMLRawDataStore(char *filename, int recordnum);
extern SGML_Document *sgml_parse_document(SGML_DTD *dtd, char *filename, 
					  char *buffer,			
					  int recordnum, int keep_buffer); 
extern SGML_Document *load_xml2_buffer(char *filename, char *record_buffer, 
				       int recordid);



#ifdef DEBUGFIL
char *docs[1500];
int docind = 0;

int add_doc(char *ptr) 
{
  docs[docind++] = ptr;
}

int dumpdoc() 
{
  int i;
  struct pageitem {
    int pagenum;
    int setindex;
    int pageid;
    struct pageitem *next;
  } *lastpage;

  struct docsum{
    int num_pages;
    float sum_wts;
    struct pageitem *page_ptr;
    struct pageitem *lastpage;
  } *doc_wts;


  printf("DUMPDOC info:\n");

  for (i = 0; i < docind; i++) {
    doc_wts = (struct docsum *) docs[i];
    printf ("num_pages %d weight %f pageaddr %x ->pagenum %d\n",
	   doc_wts->num_pages, doc_wts->sum_wts, doc_wts->page_ptr,
	   doc_wts->page_ptr->pagenum);
    
  }
}

#endif 


char *GetRawSGML(char *filename, int recordnum) {
  assoc_rec assocrec;
  int file_position, fil_seek();
  char *rec_buffer;
  int fil_read();
  int filetype;

  filetype = cf_getfiletype(filename);

  if (filetype > 99) {
    return(FetchSGMLRawDataStore(filename, recordnum));
  }

  file_position = fil_seek (filename,recordnum,&assocrec);

  if (assocrec.offset < 0) { /* deleted record */
#ifndef WIN32
    errno = 1000;
#endif
    return (NULL);
  }

  if (file_position >= 0) { /* should be OK */
    if ((rec_buffer = CALLOC(char, (assocrec.recsize+10))) == NULL) {
      fprintf(LOGFILE,"Could not allocate record buffer in GetRawSGML\n");
      return(NULL);
    }
    
    /* have what we need to get the record */
    fil_read(filename,rec_buffer,&assocrec, recordnum);
    
    return(rec_buffer);
  }
  else 
    return (NULL);
}

SGML_Document *GetSGML(char *filename, int recordnum) {
  char *recordbuffer, *GetRawSGML();
  SGML_Document *doc, *sgml_parse_document();
  SGML_DTD *main_dtd, *cf_getDTD();
  int filetype;

  filetype = cf_getfiletype(filename);

  if (filetype > 99) {
    doc = FetchSGMLDataStore(filename, recordnum);
    return doc;
  }
  else {
    recordbuffer = GetRawSGML(filename, recordnum);

    if (recordbuffer != NULL) {/*  have a record */
      
      main_dtd = (SGML_DTD *) cf_getDTD(filename);
      if (main_dtd == NULL && filetype != FILE_TYPE_XML_NODTD) 
	fprintf(LOGFILE, "couldn't get main dtd in GetSGML\n");
      
      if (filetype == FILE_TYPE_XML_NODTD) {
	doc = load_xml2_buffer(filename, recordbuffer, recordnum); 
      }
      else 
	doc = sgml_parse_document(main_dtd, filename, recordbuffer, recordnum, 0);
      
      return(doc);
    }
    else
      return (NULL);
  }
}

