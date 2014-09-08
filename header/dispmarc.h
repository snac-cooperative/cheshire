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

/****************************************************************************/
/* dispmarc.h -- Data structure for holding MARC display format information */
/****************************************************************************/

#ifndef DISPMARC
#define DISPMARC

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

typedef struct {
	char label[500];
	char tags[200];
	char subfields[200];
        char beginpunct[200];
	char subfsep[200];
	char endpunct[200];
        int  newfield;
	int  print_all;
	int  print_indicators;
	int  print_delimiters;
        int  repeatlabel;
	int  multisubstitute;
        int  indent;
 } DISP_FORMAT;


#ifndef DISPSGML
typedef struct {
     char *label;
     char *elements;
     char *subelmts;
     char *elsep;
     char *endpunct;
     int special_proc;
} SGML_FORMAT;
#endif

#define FORMAT_NAME_SIZE 200
#define DTD_NAME_SIZE 200

typedef struct {
  char formatname[FORMAT_NAME_SIZE];
  char DTD_name[DTD_NAME_SIZE];
  DISP_FORMAT *format_ptr;
  SGML_FORMAT *sgml_format_ptr;

} FORMAT_TAB ;




#ifdef INIT_FORMATS
	
DISP_FORMAT defaultformat[] = {
  {"Record #", "#", "",""," ","\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"Author:", "1??", "", ""," ", ".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Title:", "245", "", ""," ", ".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Publisher:","260","ab",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Date:","260","c",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Pages:","300","ab",""," ",".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Series:" , "4??", "", ""," ", "\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Notes:"  , "5??", "", "", " ","\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Periodical:","773","",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Subjects:","6[59]0","",""," -- ",".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Other Authors:","700","",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,15},
  {"LC Call No.:","050","",""," ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT shortformat[] = {
  {"Record #", "#", "",""," ","\n", TRUE,FALSE,FALSE,FALSE, FALSE,FALSE, 0},
  {"Author:", "1??", "", ""," ", ".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Title:", "245", "", ""," ", ".\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Publisher:","260","ab",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Date:","260","c",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Periodical:","773","",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Subjects:","6[59]0","",""," -- ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"LC Call No.:","050","",""," ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE, 0}
/*  
  {"", "#", "",""," ",". ", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"", "1??", "ab", ""," ", ".\n",  TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"", "245", "a", ""," ", ".",  TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 4},
  {"", "260", "c", "", " ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 4},
  {"", "950", "","", " ", "\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,4},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
*/
};

DISP_FORMAT marcformat[] = {
  {"Record ID: ","#","",""," ","\n", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"","???","","","", "\n",  TRUE,TRUE,TRUE,TRUE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0},
};

DISP_FORMAT tcllistformat[] = {
  {"" , "???", "", " {","}", "}",  TRUE,FALSE,TRUE,TRUE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0} 
};

DISP_FORMAT evalformat[] = {
  {"Record #","#","",""," ","\n", TRUE,FALSE,FALSE,FALSE, FALSE,FALSE, 0},
  {"Title:","245","",""," ",".\n",  TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"Periodical:","773","",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"LC Call Number:","050","",""," ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
/*
  {"008 tags:","008","",""," ","\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,15},
*/
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT dummyformat[] = {
  {"","","","","","|",FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0} 
};

DISP_FORMAT htmlformat[] = {
  {"<p><h3>Record #", "#", "",""," ","</H3>", TRUE,FALSE,FALSE,FALSE, FALSE,FALSE, 0},
  {"<DL><DT><EM>Author:</EM><DD><strong>", "100", "a", "<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@author+%+"," ", "${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"<DL><DT><EM>Corp. Author:</EM><DD><strong>", "110", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"<DL><DT><EM>Conference:</EM><DD><strong>", "111", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Title:</EM><DD><strong>", "245", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Publisher:</EM><DD><strong>","260","",""," ",".",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Periodical:</EM><DD><strong>","773","t",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"</strong><DT><EM>Date:</EM><DD><strong>","773","d9","",", ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"</strong><DT><EM>Volume:</EM><DD><strong>","773","gvn","","",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"</strong><DT><EM>Pages:</EM><DD><strong>","300","ab",""," ",".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Series:</EM><DD><strong>" , "4??", "", ""," ", "", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Notes:</EM><DD><strong>"  , "5??", "", "", " ","", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Subjects:</EM><DD><strong>","60?","","<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@pa_subject+%+"," -- ","${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"</strong><DT><EM>Subjects:</EM><DD><strong>","61?","","<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@ca_subject+%+"," -- ","${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"</strong><DT><EM>Subjects:</EM><DD><strong>","65?","","<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@subject+%+"," -- ","${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
    {"</strong><DT><EM>Other Authors:</EM><DD><strong>","700","a","<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@author+%+"," ","${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,0},
  {"</strong><DT><EM>Conference:</EM><DD><strong>","711","",""," ",". ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong><DT><EM>LC Call Number:</EM><DD><strong>","050","",""," ", " ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong><DT><EM>Dewey Call Number:</EM><DD><strong>","082","",""," ", " ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"<p></strong><DT><EM>Local Call Numbers:</EM><DD><strong>","950","lab",""," ", "<br>",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"<p></strong><DT><EM>URL:</EM><DD><strong>","856","u","<A HREF=\"","", "\"><i>Click here for document</i></a><br>",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong></DL>","+","","","","",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT shorthtmlformat[] = {
  {"<p><h3>Record #", "#", "",""," ","</H3>", TRUE,FALSE,FALSE,FALSE, FALSE,FALSE, 0},
  {"<DL><DT><EM>Author:</EM><DD><strong>", "100", "a", "<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@author+%+"," ", "${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"<DL><DT><EM>Corp. Author:</EM><DD><strong>", "110", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"<DL><DT><EM>Conference:</EM><DD><strong>", "111", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Title:</EM><DD><strong>", "245", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Publisher:</EM><DD><strong>","260","",""," ",".",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Periodical:</EM><DD><strong>","773","",""," ",".\n",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"</strong><DT><EM>Subjects:</EM><DD><strong>","6[59]0","","<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@subject+%+"," -- ","${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"</strong><DT><EM>LC Call Number:</EM><DD><strong>","050","",""," ", " ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong><DT><EM>Dewey Call Number:</EM><DD><strong>","082","",""," ", " ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"<p></strong><DT><EM>Local Call Numbers:</EM><DD><strong>","950","lab",""," ", "<br>",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"<p></strong><DT><EM>URL:</EM><DD><strong>","856","u","<A HREF=\"","", "\"><i>Click here for document</i></a><br>",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong></DL>","+","","","","",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT reviewhtmlformat[] = {
  {"<p><h3>Record #", "#", "",""," ","</H3>", TRUE,FALSE,FALSE,FALSE, FALSE,FALSE, 0},
  {"<DL><DT><EM>Author:</EM><DD><strong>", "100", "a", "<A HREF=${QUOTES}http://cheshire.lib.berkeley.edu/cgi-bin/Search?$servername@author+%+"," ", "${QUOTES}>%s</A>.  ", TRUE,FALSE,FALSE,FALSE,FALSE,TRUE, 0},
  {"<DL><DT><EM>Corp. Author:</EM><DD><strong>", "110", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"<DL><DT><EM>Conference:</EM><DD><strong>", "111", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Title:</EM><DD><strong>", "245", "", ""," ", ".", TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 0},
  {"</strong><DT><EM>Source:</EM><DD><strong>","773","","","","",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE, 15},
  {"</strong><DT><EM>LC Call Number:</EM><DD><strong>","050","",""," ", " ",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"<p></strong><DT><EM>URL:</EM><DD><strong>","856","u","<A HREF=\"","", "\"><i>Click here for document</i></a><br>",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"</strong></DL>","+","","","","",TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};



DISP_FORMAT dcformat[] = {
  {"<dc:Creator>", "100", "a", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Creator>", "110", "", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Creator>", "111", "", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Title>", "245", "", ""," ", "</dc:Title>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Publisher>","260","b",""," ","</dc:Publisher>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Date>","260","c","",", ","</dc:Date>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Date>","773","d9","",", ","</dc:Date>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Language>","041","","", " + ", "</dc:Language>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Relation>" , "4??", "", ""," ", "</dc:Relation>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Relation>" , "773", "", ""," ", "</dc:Relation>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Description>"  , "5??", "", "", " ","</dc:Description>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Description>"  , "999", "", "", " ","</dc:Description>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Subject>","6[59]0","","", " -- ", "</dc:Subject>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
    {"<dc:Contributor>","700","a"," "," ","</dc:Contributor>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Contributor>","711","",""," ","</dc:Contributor>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Identifier>","010","",""," ", "</dc:Identifier> ",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Identifier>","950","lab",""," ", "</dc:Identifier>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Identifier>","856","knu",""," : ", "</dc:Identifier>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};

DISP_FORMAT shortdcformat[] = {
  {"<dc:Creator>", "100", "a", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Creator>", "110", "", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Creator>", "111", "", ""," ", "</dc:Creator>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Title>", "245", "", ""," ", "</dc:Title>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Date>","260","c","",", ","</dc:Date>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Date>","773","d9","",", ","</dc:Date>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"<dc:Identifier>","010","",""," ", "</dc:Identifier> ",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Identifier>","856","knu","","", "</dc:Identifier>",TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,0},
  {"<dc:Relation>" , "773", "", ""," ", "</dc:Relation>", TRUE,FALSE,FALSE,FALSE,TRUE,FALSE, 0},
  {"","","","","","",-1,FALSE,FALSE,FALSE,FALSE,FALSE,0}
};


#else
	
extern DISP_FORMAT defaultformat[];
extern DISP_FORMAT shortformat[];
extern DISP_FORMAT marcformat[];
extern DISP_FORMAT tcllistformat[];
extern DISP_FORMAT evalformat[];
extern DISP_FORMAT dummyformat[];
extern DISP_FORMAT htmlformat[];
extern DISP_FORMAT shorthtmlformat[];
extern DISP_FORMAT reviewhtmlformat[];
extern DISP_FORMAT dcformat[]; /* Dublin Core XML formats */
extern DISP_FORMAT shortdcformat[];

#endif

#endif

/* EXTERNAL ROUTINES -- in marclib.c */
#include "marclib.h"









