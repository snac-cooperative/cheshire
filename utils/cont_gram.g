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
*	Header Name:	cont_grammar.g
*                       this is a test LLgen grammar file 
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	defines the grammar for config files
*
*	Usage:		LLgen the file then call LLparse
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7-15-94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
{
#include <stdlib.h>
#include <stdio.h>
#include "cheshire.h"
#include "configfiles.h"

/* Solaris doesn't have bzero, so the following maps it */
#ifdef SOLARIS
#ifndef DMALLOC_INCLUDE
#define bzero(x,y)  memset(x, 0, y)
#endif
#endif
#ifdef ALPHA
#ifndef DMALLOC_INCLUDE
#define bzero(x,y)  memset(x, 0, y)
#endif
#endif


/* cont_lval contains the current values from the lexical analysis */

struct cont_LVAL { 
        int  tok_id;
        char *string;
        int intval;
 } cont_lval, cont_lval_pushback;
 
  continuation *first_cont, *new_cont, *last_cont;

}
/* force the prefix of generated routines to CF instead of LL */
%prefix CONT;

/* Config file tag defs */

%token FILECONT_START;
%token FILECONT_END;

%token FILECONT_NUMBER;
%token FILECONT_MIN;
%token FILECONT_MAX;

%token ATTRIBUTES_END;

/* string values */
%token FILENAME_LITERAL;

/* number values */
%token NUMBER;

%start CONTparse, contfile ;

contfile: filecont* ;

filecont: FILECONT_START  { 
			  	if ((new_cont = CALLOC(continuation, 1))
					 == NULL) {
    				fprintf(LOGFILE, "No malloc space for file continuation\n");
    				exit (1);
  				}
  			
  				if (first_cont == NULL) {
  					first_cont = new_cont;
  					last_cont = new_cont;
  				}
	  			else {
  				    last_cont->next_cont = new_cont;
  				    last_cont = new_cont;
  				}
			} 
          FILECONT_NUMBER 
		  NUMBER { last_cont->id_number = cont_lval.intval; }
          FILECONT_MIN 
		  NUMBER { last_cont->docid_min = cont_lval.intval; }
          FILECONT_MAX 
		  NUMBER { last_cont->docid_max = cont_lval.intval; }
          ATTRIBUTES_END
          FILENAME_LITERAL {last_cont->name = cont_lval.string;}
          FILECONT_END ;


%lexical next_token ;

