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
/************************************************************************
*
*       Header Name:    se_driver.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive search engine 
*                      
*
*       Usage:          se_driver key
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/7/93
*       Revised:        11/28/93
*       Version:        0.99999999
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
/* #define TESTFORMATS */

#include <stdlib.h>

/* Z39.50 Library includes */
#include <z3950_3.h>
#include <z_parameters.h>
#include <z3950_intern.h>

#include <cheshire.h>
#include <z_parse.h>

#include <dispsgml.h>
#include <dispmarc.h>

#include <search_stat.h>

#ifdef MAIN

#define NUM_FORMAT_SLOTS 100

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
  {"","", NULL} /* default to brief format */
};

#define LAST_BUILTIN_FORMAT 15
int last_format_slot = LAST_BUILTIN_FORMAT ;

SGML_DTD *main_dtd = NULL;

int last_rec_proc; /* used in indexing and parsing code */

#endif
#include <dmalloc.h>




/**** function prototypes ****/
extern RPNQuery *queryparse(char *query, char *attributeSetId);
extern weighted_result *se_process_query(char *, RPNQuery *);
extern char *se_process_meta(RPNQuery *RPN_Query);

int init(char *config_path);
char *showconfig();
char *getconfigpath();
char *getconfigindexnames();

int setdb(char *dbname);
char *showdbs();
char *showdb();
char *getdbname();

int setresultname(char *name);
char *showresultname();

int setnumwanted(int numwanted);
int shownumwanted();

int setsyntax(char *syntax);
char *showsyntax();

int setresultformat(char *format);
char *showformat();

weighted_result *Search(char *search);
int getnumfound(weighted_result *final_set);
char *getrecord(weighted_result *final_set, int recnum);
float getrelevance(weighted_result *final_set, int recnum);

void closeresult(weighted_result *final_set);
void CheshireClose();

/*****************************/

