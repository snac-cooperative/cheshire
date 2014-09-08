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
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:      Jerome McDonough, Ray R. Larson
 *		School of Information Management and System, UC Berkeley
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

/****************************************************************************/
/* dispmarc.h -- Data structure for holding MARC display format information */
/****************************************************************************/

#ifndef DISPSGML
#define DISPSGML
#endif

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#ifndef SUCCESS
#define SUCCESS 1
#define FAILURE 0
#endif

#define NONE 0
#define REMOVE_BLANKS 1

typedef struct {
     char *label;
     char *elements;
     char *subelmts;
     char *elsep;
     char *beginpunct;
     char *endpunct;
     int special_proc;
} SGML_FORMAT;


typedef struct {
     char dtd_name[200];
     char format_name[200];
     int format_id;
} SGML_FORMAT_LIST;

/* the following need to be changed together when changed */
#define NUM_FORMATS 19

#ifdef SGML_INITIALIZE
SGML_FORMAT_LIST known_formats[] = {
{"Fake", "FAKE", 0}, 
{"USMARC", "REVIEW", 1},
{"USMARC", "SHORT", 2},
{"USMARC", "LONG", 3},
{"LCCLUST", "LCCSHORT", 4},
{"SUBJCLUS", "LCCSHORT", 4},
{"USMARC", "MARC", 5},
{"USMARC", "HTMLREVIEW", 6},
{"USMARC", "HTMLSHORT", 7},
{"USMARC", "HTMLLONG", 8},
{"USMARC", "CSMP_HTMLREVIEW", 9},
{"USMARC", "CSMP_HTMLSHORT", 10},
{"USMARC", "CSMP_HTMLLONG", 11},
{"FT931", "REVIEW", 12},
{"FT931", "SHORT", 13},
{"FT931", "LONG", 14},
{"USMARC", "GLAS_HTMLREVIEW", 15},
{"USMARC", "GLAS_HTMLSHORT", 16},
{"USMARC", "GLAS_HTMLLONG", 17}
};

SGML_FORMAT *USER_DEFINED_FORMAT;

/* Format ID 1 */	
SGML_FORMAT USMARC_review[]= {
{"Title:        ", "FLD245", "*", " ","", ".", NONE},
{"Call Number:  ", "FLD050", "L A B V", " ","", "", NONE},
{"Call Numbers: ", "FLD950", "L A B V", " ","", "", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 2 */
SGML_FORMAT USMARC_short[] = {
{"Author:        ", "FLD1..", "*", " ","", "", NONE},
{"Title:         ", "FLD245", "*", " ","", ".", NONE},
{"Publisher:     ", "FLD260", "A B", " ","", "", NONE},
{"Date:          ", "FLD260", "C", " ","", ".", NONE},
{"Pages:         ", "FLD300","A B"," ","", "", NONE},
{"Periodical:    ", "FLD773", "*", " ","", "", NONE},
{"ISSN:          ", "FLD022", "*", " ","","", NONE},
{"Subjects:      ", "FLD6..", "*", " -- ","", "", NONE},
{"Call Number:   ", "FLD050", "L A B V", " ","", "", NONE},
{"Call Numbers:  ", "FLD950", "L A B V", " ","", "", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 3 */
SGML_FORMAT USMARC_long[] = {
{"Author:        ", "FLD1..", "*", " ","", "", NONE},
{"Title:         ", "FLD245", "*", " ","", ".", NONE},
{"Publisher:     ", "FLD260", "A B", " ","", "", NONE},
{"Date:          ", "FLD260", "C", " ","", ".", NONE},
{"Pages:         ", "FLD300", "*", " ","", ".", NONE},
{"Series:        ", "FLD4..", "*", " ","", "", NONE},
{"Periodical:    ", "FLD773", "*", " ","", "", NONE},
{"Notes:         ", "FLD5..", "*", " ","", ".", NONE},
{"Subjects:      ", "FLD6..", "*", " -- ","", "", NONE},
{"Other Authors: ", "FLD7.0", "*", " ","", "", NONE},
{"ISSN:          ", "FLD022", "*", " ","","", NONE},
{"Call Number:   ", "FLD050", "L A B V", " ","", "", NONE},
{"Call Numbers:  ", "FLD950", "L A B V", " ","", "", NONE},
{NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 4 */
SGML_FORMAT LCCLUST_lccshort[] = {
{"Classification: ", "CLUSTER", "CLUSKEY", " ","", "", NONE},
{"Subjects:       ", "SUBJSUM", "SUMDATA", "\n                 ","", "", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 5 */
SGML_FORMAT USMARC_marc[] = {
{"001 ", "FLD001", "", " ","", "", NONE},
{"003 ", "FLD003", "", " ","", "", NONE},
{"005 ", "FLD005", "", " ","", "", NONE},
{"006 ", "FLD006", "", " ","", "", NONE},
{"007 ", "FLD007", "", " ","", "", NONE},
{"008 ", "FLD008", "", " ","", "", NONE},
{"010 ", "FLD010", "*", " ","", "", NONE},
{"011 ", "FLD011", "*", " ","", "", NONE},
{"015 ", "FLD015", "*", " ","", "", NONE},
{"017 ", "FLD017", "*", " ","", "", NONE},
{"018 ", "FLD018", "*", " ","", "", NONE},
{"019 ", "FLD019", "*", " ","", "", NONE},
{"020 ", "FLD020", "*", " ","", "", NONE},
{"022 ", "FLD022", "*", " ","", "", NONE},
{"023 ", "FLD023", "*", " ","", "", NONE},
{"024 ", "FLD024", "*", " ","", "", NONE},
{"025 ", "FLD025", "*", " ","", "", NONE},
{"027 ", "FLD027", "*", " ","", "", NONE},
{"028 ", "FLD028", "*", " ","", "", NONE},
{"029 ", "FLD029", "*", " ","", "", NONE},
{"030 ", "FLD030", "*", " ","", "", NONE},
{"031 ", "FLD031", "*", " ","", "", NONE},
{"032 ", "FLD032", "*", " ","", "", NONE},
{"033 ", "FLD033", "*", " ","", "", NONE},
{"034 ", "FLD034", "*", " ","", "", NONE},
{"035 ", "FLD035", "*", " ","", "", NONE},
{"036 ", "FLD036", "*", " ","", "", NONE},
{"037 ", "FLD037", "*", " ","", "", NONE},
{"039 ", "FLD039", "*", " ","", "", NONE},
{"040 ", "FLD040", "*", " ","", "", NONE},
{"041 ", "FLD041", "*", " ","", "", NONE},
{"042 ", "FLD042", "*", " ","", "", NONE},
{"043 ", "FLD043", "*", " ","", "", NONE},
{"044 ", "FLD044", "*", " ","", "", NONE},
{"045 ", "FLD045", "*", " ","", "", NONE},
{"046 ", "FLD046", "*", " ","", "", NONE},
{"047 ", "FLD048", "*", " ","", "", NONE},
{"050 ", "FLD050", "*", " ","", "", NONE},
{"051 ", "FLD051", "*", " ","", "", NONE},
{"052 ", "FLD052", "*", " ","", "", NONE},
{"055 ", "FLD055", "*", " ","", "", NONE},
{"060 ", "FLD060", "*", " ","", "", NONE},
{"061 ", "FLD061", "*", " ","", "", NONE},
{"066 ", "FLD066", "*", " ","", "", NONE},
{"069 ", "FLD069", "*", " ","", "", NONE},
{"070 ", "FLD070", "*", " ","", "", NONE},
{"071 ", "FLD071", "*", " ","", "", NONE},
{"072 ", "FLD072", "*", " ","", "", NONE},
{"074 ", "FLD074", "*", " ","", "", NONE},
{"080 ", "FLD080", "*", " ","", "", NONE},
{"082 ", "FLD082", "*", " ","", "", NONE},
{"084 ", "FLD084", "*", " ","", "", NONE},
{"086 ", "FLD086", "*", " ","", "", NONE},
{"088 ", "FLD088", "*", " ","", "", NONE},
{"096 ", "FLD096", "*", " ","", "", NONE},
{"100 ", "FLD100", "*", " ","", "", NONE},
{"110 ", "FLD110", "*", " ","", "", NONE},
{"111 ", "FLD111", "*", " ","", "", NONE},
{"130 ", "FLD130", "*", " ","", "", NONE},
{"210 ", "FLD210", "*", " ","", "", NONE},
{"211 ", "FLD211", "*", " ","", "", NONE},
{"212 ", "FLD212", "*", " ","", "", NONE},
{"214 ", "FLD214", "*", " ","", "", NONE},
{"222 ", "FLD222", "*", " ","", "", NONE},
{"240 ", "FLD240", "*", " ","", "", NONE},
{"242 ", "FLD242", "*", " ","", "", NONE},
{"243 ", "FLD243", "*", " ","", "", NONE},
{"245 ", "FLD245", "*", " ","", "", NONE},
{"246 ", "FLD246", "*", " ","", "", NONE},
{"247 ", "FLD247", "*", " ","", "", NONE},
{"250 ", "FLD250", "*", " ","", "", NONE},
{"254 ", "FLD254", "*", " ","", "", NONE},
{"255 ", "FLD255", "*", " ","", "", NONE},
{"256 ", "FLD256", "*", " ","", "", NONE},
{"257 ", "FLD257", "*", " ","", "", NONE},
{"260 ", "FLD260", "*", " ","", "", NONE},
{"261 ", "FLD261", "*", " ","", "", NONE},
{"262 ", "FLD262", "*", " ","", "", NONE},
{"263 ", "FLD263", "*", " ","", "", NONE},
{"265 ", "FLD265", "*", " ","", "", NONE},
{"300 ", "FLD300", "*", " ","", "", NONE},
{"305 ", "FLD305", "*", " ","", "", NONE},
{"306 ", "FLD306", "*", " ","", "", NONE},
{"310 ", "FLD310", "*", " ","", "", NONE},
{"315 ", "FLD315", "*", " ","", "", NONE},
{"321 ", "FLD321", "*", " ","", "", NONE},
{"340 ", "FLD340", "*", " ","", "", NONE},
{"350 ", "FLD350", "*", " ","", "", NONE},
{"351 ", "FLD351", "*", " ","", "", NONE},
{"355 ", "FLD355", "*", " ","", "", NONE},
{"357 ", "FLD357", "*", " ","", "", NONE},
{"362 ", "FLD362", "*", " ","", "", NONE},
{"400 ", "FLD400", "*", " ","", "", NONE},
{"410 ", "FLD410", "*", " ","", "", NONE},
{"411 ", "FLD411", "*", " ","", "", NONE},
{"440 ", "FLD440", "*", " ","", "", NONE},
{"490 ", "FLD490", "*", " ","", "", NONE},
{"500 ", "FLD500", "*", " ","", "", NONE},
{"501 ", "FLD501", "*", " ","", "", NONE},
{"502 ", "FLD502", "*", " ","", "", NONE},
{"503 ", "FLD503", "*", " ","", "", NONE},
{"504 ", "FLD504", "*", " ","", "", NONE},
{"505 ", "FLD505", "*", " ","", "", NONE},
{"506 ", "FLD506", "*", " ","", "", NONE},
{"507 ", "FLD507", "*", " ","", "", NONE},
{"508 ", "FLD508", "*", " ","", "", NONE},
{"510 ", "FLD510", "*", " ","", "", NONE},
{"511 ", "FLD511", "*", " ","", "", NONE},
{"513 ", "FLD513", "*", " ","", "", NONE},
{"515 ", "FLD515", "*", " ","", "", NONE},
{"516 ", "FLD516", "*", " ","", "", NONE},
{"518 ", "FLD518", "*", " ","", "", NONE},
{"520 ", "FLD520", "*", " ","", "", NONE},
{"521 ", "FLD521", "*", " ","", "", NONE},
{"522 ", "FLD522", "*", " ","", "", NONE},
{"523 ", "FLD523", "*", " ","", "", NONE},
{"524 ", "FLD524", "*", " ","", "", NONE},
{"525 ", "FLD525", "*", " ","", "", NONE},
{"530 ", "FLD530", "*", " ","", "", NONE},
{"533 ", "FLD533", "*", " ","", "", NONE},
{"534 ", "FLD534", "*", " ","", "", NONE},
{"535 ", "FLD535", "*", " ","", "", NONE},
{"536 ", "FLD536", "*", " ","", "", NONE},
{"537 ", "FLD537", "*", " ","", "", NONE},
{"538 ", "FLD538", "*", " ","", "", NONE},
{"540 ", "FLD540", "*", " ","", "", NONE},
{"541 ", "FLD541", "*", " ","", "", NONE},
{"544 ", "FLD544", "*", " ","", "", NONE},
{"545 ", "FLD545", "*", " ","", "", NONE},
{"546 ", "FLD546", "*", " ","", "", NONE},
{"547 ", "FLD547", "*", " ","", "", NONE},
{"550 ", "FLD550", "*", " ","", "", NONE},
{"555 ", "FLD555", "*", " ","", "", NONE},
{"556 ", "FLD556", "*", " ","", "", NONE},
{"561 ", "FLD561", "*", " ","", "", NONE},
{"562 ", "FLD562", "*", " ","", "", NONE},
{"565 ", "FLD565", "*", " ","", "", NONE},
{"567 ", "FLD567", "*", " ","", "", NONE},
{"570 ", "FLD570", "*", " ","", "", NONE},
{"580 ", "FLD580", "*", " ","", "", NONE},
{"581 ", "FLD581", "*", " ","", "", NONE},
{"582 ", "FLD582", "*", " ", "","", NONE},
{"583 ", "FLD583", "*", " ","", "", NONE},
{"584 ", "FLD584", "*", " ","", "", NONE},
{"585 ", "FLD585", "*", " ","", "", NONE},
{"586 ", "FLD586", "*", " ", "","", NONE},
{"590 ", "FLD590", "*", " ", "","", NONE},
{"596 ", "FLD596", "*", " ", "","", NONE},
{"600 ", "FLD600", "*", " ", "","", NONE},
{"610 ", "FLD610", "*", " ", "","", NONE},
{"611 ", "FLD611", "*", " ", "","", NONE},
{"630 ", "FLD630", "*", " ", "","", NONE},
{"650 ", "FLD650", "*", " ", "","", NONE},
{"651 ", "FLD651", "*", " ", "","", NONE},
{"653 ", "FLD653", "*", " ", "","", NONE},
{"654 ", "FLD654", "*", " ", "","", NONE},
{"655 ", "FLD655", "*", " ", "","", NONE},
{"656 ", "FLD656", "*", " ", "","", NONE},
{"657 ", "FLD657", "*", " ", "","", NONE},
{"658 ", "FLD658", "*", " ", "","", NONE},
{"690 ", "FLD690", "*", " ", "","", NONE},
{"697 ", "FLD697", "*", " ", "","", NONE},
{"700 ", "FLD700", "*", " ", "","", NONE},
{"710 ", "FLD710", "*", " ", "","", NONE},
{"711 ", "FLD711", "*", " ", "","", NONE},
{"730 ", "FLD730", "*", " ", "","", NONE},
{"740 ", "FLD740", "*", " ", "","", NONE},
{"752 ", "FLD752", "*", " ", "","", NONE},
{"753 ", "FLD753", "*", " ", "","", NONE},
{"754 ", "FLD754", "*", " ", "","", NONE},
{"755 ", "FLD755", "*", " ", "","", NONE},
{"760 ", "FLD760", "*", " ", "","", NONE},
{"762 ", "FLD762", "*", " ", "","", NONE},
{"765 ", "FLD765", "*", " ", "","", NONE},
{"767 ", "FLD767", "*", " ", "","", NONE},
{"770 ", "FLD770", "*", " ", "","", NONE},
{"772 ", "FLD772", "*", " ", "","", NONE},
{"773 ", "FLD773", "*", " ", "","", NONE},
{"775 ", "FLD775", "*", " ", "","", NONE},
{"776 ", "FLD776", "*", " ", "","", NONE},
{"777 ", "FLD777", "*", " ", "","", NONE},
{"780 ", "FLD780", "*", " ", "","", NONE},
{"785 ", "FLD785", "*", " ", "","", NONE},
{"787 ", "FLD787", "*", " ", "","", NONE},
{"797 ", "FLD797", "*", " ", "","", NONE},
{"800 ", "FLD800", "*", " ", "","", NONE},
{"810 ", "FLD810", "*", " ", "","", NONE},
{"811 ", "FLD811", "*", " ", "","", NONE},
{"830 ", "FLD830", "*", " ", "","", NONE},
{"840 ", "FLD840", "*", " ", "","", NONE},
{"841 ", "FLD841", "*", " ", "","", NONE},
{"843 ", "FLD843", "*", " ", "","", NONE},
{"845 ", "FLD845", "*", " ", "","", NONE},
{"850 ", "FLD850", "*", " ", "","", NONE},
{"852 ", "FLD852", "*", " ", "","", NONE},
{"853 ", "FLD853", "*", " ", "","", NONE},
{"854 ", "FLD854", "*", " ", "","", NONE},
{"855 ", "FLD855", "*", " ", "","", NONE},
{"856 ", "FLD856", "*", " ", "","", NONE},
{"863 ", "FLD863", "*", " ", "","", NONE},
{"864 ", "FLD864", "*", " ", "","", NONE},
{"865 ", "FLD865", "*", " ", "","", NONE},
{"866 ", "FLD866", "*", " ", "","", NONE},
{"867 ", "FLD867", "*", " ", "","", NONE},
{"868 ", "FLD868", "*", " ", "","", NONE},
{"876 ", "FLD876", "*", " ", "","", NONE},
{"877 ", "FLD877", "*", " ", "","", NONE},
{"878 ", "FLD878", "*", " ", "","", NONE},
{"880 ", "FLD880", "*", " ", "","", NONE},
{"886 ", "FLD886", "*", " ", "","", NONE},
{"901 ", "FLD901", "*", " ", "","", NONE},
{"902 ", "FLD902", "*", " ", "","", NONE},
{"903 ", "FLD903", "*", " ", "","", NONE},
{"905 ", "FLD905", "*", " ", "","", NONE},
{"935 ", "FLD935", "*", " ", "","", NONE},
{"950 ", "FLD950", "*", " ", "","", NONE},
{"955 ", "FLD955", "*", " ", "","", NONE},
{"998 ", "FLD998", "*", " ", "","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};     

/* Format ID 6 */	
SGML_FORMAT USMARC_HTMLreview[]= {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Author:</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Call Number:</EM><DD><strong>", "FLD050", "L A B V", " ","", "", NONE},
{"</strong><DT><EM>Call Numbers:</EM><DD><strong>", "FLD950", "L A B V", " ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};


/* Format ID 7 */
SGML_FORMAT USMARC_HTMLshort[] = {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Author:</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "A B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Periodical:</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
{"</strong><DT><EM>ISSN:</EM><DD><strong>", "FLD020", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Call Numbers:</EM><DD><strong>", "FLD950", "L A B V", " ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 8 */
SGML_FORMAT USMARC_HTMLlong[] = {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Author:</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "A B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Periodical:</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong><DT><EM>Pages:</EM><DD><strong>", "FLD3..", "*", " ","", "", NONE},
{"</strong><DT><EM>Series:</EM><DD><strong>", "FLD4..", "*", " ","", "", NONE},
{"</strong><DT><EM>Notes:</EM><DD><strong>", "FLD5..", "*", " ","", "", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Other Authors:</EM><DD><strong>", "FLD7.0", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Call Numbers:</EM><DD><strong>", "FLD950", "L A B V", " ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 9 */	
SGML_FORMAT USMARC_CSMP_HTMLreview[]= {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Composer:</EM><DD><strong>", "FLD1..", "*", " ", "","", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};


/* Format ID 10 */
SGML_FORMAT USMARC_CSMP_HTMLshort[] = {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Composer:</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "A B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Periodical:</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
  /* {"</strong><DT><EM>Cover Illus:</EM><DD><strong>", "FLD655", "A B Z Y", " -- ","", "", NONE}, */

{"</strong><DT><EM>ISSN:</EM><DD><strong>", "FLD020", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 11 */
SGML_FORMAT USMARC_CSMP_HTMLlong[] = {
{"<strong><A HREF=","FLD856","A U","","", ">SUB856MESSAGE</A></strong><br>", NONE},
{"<p><DL><DT><EM>Composer:</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "A B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Periodical:</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong><DT><EM>Pages:</EM><DD><strong>", "FLD300", "*", " ","", "", NONE},
{"</strong><DT><EM>Series:</EM><DD><strong>", "FLD4..", "*", " ","", "", NONE},
{"</strong><DT><EM>Plate No.:</EM><DD><strong>", "FLD028", "*", " ","", "", NONE},
{"</strong><DT><EM>Notes:</EM><DD><strong>", "FLD5..", "*", " ","", "", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
  /* {"</strong><DT><EM>Cover Illus:</EM><DD><strong>", "FLD655", "A B Z Y", " -- ","", "", NONE}, */
{"</strong><DT><EM>Other Authors:</EM><DD><strong>", "FLD7.0", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 12 */	
SGML_FORMAT FT931_review[]= {
{"DOCUMENT NO.: ", "DOCNO", "", " ","", ".", NONE},
{"HEADLINE:     ", "HEADLINE", "", " ","", ".", NONE},
{"BYLINE:       ", "BYLINE", "", " ","", ".", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 13 */	
SGML_FORMAT FT931_short[]= {
{"DOCUMENT NO.: ", "DOCNO", "", " ","", ".", NONE},
{"HEADLINE:     ", "HEADLINE", "", " ","", ".", NONE},
{"BYLINE:       ", "BYLINE", "", " ","", ".", NONE},
{"DATELINE:     ", "DATELINE", "", " ","", ".", NONE},
{"PUBLICATION:  ", "PUB", "", " ","", ".", NONE},
{"PAGE:         ", "PAGE", "", " ","", ".", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 14 */	
SGML_FORMAT FT931_long[]= {
{"DOCUMENT NO.: ", "DOCNO", "", " ","", ".", NONE},
{"HEADLINE:     ", "HEADLINE", "", " ","", ".", NONE},
{"BYLINE:       ", "BYLINE", "", " ","", ".", NONE},
{"DATELINE:     ", "DATELINE", "", " ","", ".", NONE},
{"PUBLICATION:  ", "PUB", "", " ","", ".", NONE},
{"PAGE:         ", "PAGE", "", " ","", ".", NONE},
{"TEXT:         ", "TEXT", "", " ","", ".", NONE},
{NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 15 */	
SGML_FORMAT USMARC_GLAS_HTMLreview[]= {
{"<strong><A HREF=\"GLASLINK.tcl.ITEMID=","FLD080","A","","", "", REMOVE_BLANKS},
{"\">Special Collections: ", "FLD080", "A", "","", "</A></strong><br>", NONE},
{"<p><DL><DT><EM>Main creator (person):</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Holdings:</EM><DD><strong>", "FLD852", "*", " -- ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};


/* Format ID 16 */
SGML_FORMAT USMARC_GLAS_HTMLshort[] = {
{"<p><DL><DT><EM>Shelf Mark: </EM><strong><A HREF=\"GLASLINK.tcl.ITEMID=","FLD080","A","","", "", REMOVE_BLANKS},
{"\">Special Collections: ", "FLD080", "A", "","", "</A></strong><br>", NONE},
{"<DT><EM>Main creator (person):</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Collective Title:</EM><DD><strong>", "FLD240", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ","", "", NONE},
{"</strong><DT><EM>Place of Origin:</EM><DD><strong>", "FLD260", "A", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Physical Description::</EM><DD><strong>", "FLD300", "*", " ","", "", NONE},
{"</strong><DT><EM>General Notes:</EM><DD><strong>", "FLD520", "*", " ","", "", NONE},
{"</strong><DT><EM>Related Entries:</EM><DD><strong>", "FLD580", "*", " ","", "", NONE},
{"</strong><DT><EM>Added Names: Pers::</EM><DD><strong>", "FLD700", "*", " ","", "", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Host Entry</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

/* Format ID 17 */
SGML_FORMAT USMARC_GLAS_HTMLlong[] = {
{"<p><DL><DT><EM>Shelf Mark: </EM><strong><A HREF=\"GLASLINK.tcl.ITEMID=","FLD080","A","","", "", REMOVE_BLANKS},
{"\">Special Collections: ", "FLD080", "A", "","", "</A></strong><br>", NONE},
{"<DT><EM>Main creator (person):</EM><DD><strong>", "FLD1..", "*", " ","", "", NONE},
{"</strong><DT><EM>Collective Title:</EM><DD><strong>", "FLD240", "*", " ","", "", NONE},
{"</strong><DT><EM>Title:</EM><DD><strong>", "FLD245", "*", " ", "","", NONE},
{"</strong><DT><EM>Place of Origin:</EM><DD><strong>", "FLD260", "A", " ","", "", NONE},
{"</strong><DT><EM>Publisher:</EM><DD><strong>", "FLD260", "B", " ","", "", NONE},
{"</strong><DT><EM>Date:</EM><DD><strong>", "FLD260", "C", " ","", "", NONE},
{"</strong><DT><EM>Physical Description::</EM><DD><strong>", "FLD300", "*", " ","", "", NONE},
{"</strong><DT><EM>Arrangement:</EM><DD><strong>", "FLD351", "A", " ","", "", NONE},
{"</strong><DT><EM>General Notes:</EM><DD><strong>", "FLD520", "*", " ","", "", NONE},
{"</strong><DT><EM>Summary Notes:</EM><DD><strong>", "FLD500", "*", " ","", "", NONE},
{"</strong><DT><EM>Terms of Use Notes:</EM><DD><strong>", "FLD506", "*", " ","", "", NONE},
{"</strong><DT><EM>Collection Name:</EM><DD><strong>", "FLD524", "*", " ","", "", NONE},
{"</strong><DT><EM>Provenance:</EM><DD><strong>", "FLD541", "*", " ","", "", NONE},
{"</strong><DT><EM>Related Entries:</EM><DD><strong>", "FLD580", "*", " ","", "", NONE},
{"</strong><DT><EM>Added Names: Pers::</EM><DD><strong>", "FLD700", "*", " ", "","", NONE},
{"</strong><DT><EM>Subjects:</EM><DD><strong>", "FLD6..", "*", " -- ","", "", NONE},
{"</strong><DT><EM>Host Entry</EM><DD><strong>", "FLD773", "*", " ","", "", NONE},
{"</strong><DT><EM>Location:</EM><DD><strong>", "FLD852", "*", " ","", "", NONE},
{"</strong></DL>","+","","","","", NONE},
{NULL,NULL,NULL,NULL,NULL,NULL, NONE}
};

#else

extern SGML_FORMAT_LIST known_formats[];

extern SGML_FORMAT *USER_DEFINED_FORMAT;
/* Format ID 1 */	
extern SGML_FORMAT USMARC_review[];

/* Format ID 2 */
extern SGML_FORMAT USMARC_short[] ;

/* Format ID 3 */
extern SGML_FORMAT USMARC_long[] ;

/* Format ID 4 */
extern SGML_FORMAT LCCLUST_lccshort[] ;

/* Format ID 5 */
extern SGML_FORMAT USMARC_marc[] ;     

/* Format ID 6 */	
extern SGML_FORMAT USMARC_HTMLreview[];

/* Format ID 7 */
extern SGML_FORMAT USMARC_HTMLshort[] ;

/* Format ID 8 */
extern SGML_FORMAT USMARC_HTMLlong[] ;

/* Format ID 9 */	
extern SGML_FORMAT USMARC_CSMP_HTMLreview[];


/* Format ID 10 */
extern SGML_FORMAT USMARC_CSMP_HTMLshort[];

/* Format ID 11 */
extern SGML_FORMAT USMARC_CSMP_HTMLlong[];

/* Format ID 12 */	
extern SGML_FORMAT FT931_review[];

/* Format ID 13 */	
extern SGML_FORMAT FT931_short[];

/* Format ID 14 */	
extern SGML_FORMAT FT931_long[];

/* Format ID 15 */	
extern SGML_FORMAT USMARC_GLAS_HTMLreview[];


/* Format ID 16 */
extern SGML_FORMAT USMARC_GLAS_HTMLshort[] ;

/* Format ID 17 */
extern SGML_FORMAT USMARC_GLAS_HTMLlong[] ;

#endif




