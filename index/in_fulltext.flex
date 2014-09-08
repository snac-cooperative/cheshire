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
*	Header Name:	in_fulltext.flex
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	lexical parser for raw text and html files
*
*	Usage:		token = yylex(); -- after using flex
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/30/1996
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1996.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
%{
#include <stdlib.h>
#ifndef SOLARIS
#ifndef WIN32
#include <strings.h> 
#endif
#endif

#include <string.h>

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#define LOGFILE stderr

#ifdef DMALLOC_INCLUDE
#define YY_MALLOC_DECL
#endif

int prev_state[500];

char *FULL_TEXT_TOKEN;
int FULL_TEXT_POSITION;

#define YY_USER_ACTION FULL_TEXT_POSITION += yyleng;
 
#define TOKEN 1
#define TAG 2
#define NUMBER 3
#define YEAR 4
#define JUNK  0

%}

/* Definitions -- note that starttag MAY get too much data it is intended */
/* to strip sequences of tags as well as single tags                      */
/* if the goal is to index everything surrounded by tags this will need to*/
/* be fixed                                                               */
ws  [ \t\n]+
nl  \n
token [a-zA-Z][a-zA-Z\-_]*
year  19[0-9][0-9]
year2 20[0-9[0-9]
number [0-9]*
punct [^<>a-zA-Z0-9]*
starttag "<"[a-zA-Z]+[^>]*">"  
endtag "</"[a-zA-Z][a-zA-Z0-9_\-]*">"


/* Start conditions */

%%

<*>{token}  { FULL_TEXT_TOKEN = strdup(in_ft_text); 
	     return TOKEN;
	}

<*>{starttag} { FULL_TEXT_TOKEN = strdup(in_ft_text); 
		return TAG;
	}

<*>{endtag} { FULL_TEXT_TOKEN = strdup(in_ft_text);
		return TAG;
	}

<*>{year} {  FULL_TEXT_TOKEN = strdup(in_ft_text);
		return YEAR;
	}

<*>{year2} {  FULL_TEXT_TOKEN = strdup(in_ft_text);
		return YEAR;
	}

<*>{number} /* ignore numbers */;

<*>{ws} /* ignore whitespace */;
<*>{punct} /* ignore punctuation */;

.	{ /* printf("Unrecognized char ='%c' in full text parser\n",
		in_ft_text[0]); 
	  return in_ft_text[0];
	 */
	}

<<EOF>> return(JUNK);

%%


int in_ft_wrap () 
{
 /*  fprintf(LOGFILE,"cf_wrap -- End of input file\n"); */
 /* yy_delete_buffer(YY_CURRENT_BUFFER); */
  return (-1); 
}
