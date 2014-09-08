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
*	Header Name:	cont_parse.flex
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	lexical parser for continuation files
*
*	Usage:		token = yylex(); -- after using flex
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
%{
#include <stdlib.h>
#ifndef WIN32
#ifndef SOLARIS
#include <strings.h> 
#endif
#endif

#include <string.h>
#include "CONTpars.h"

#ifdef DMALLOC_INCLUDE
#include <dmalloc.h>
#endif

#ifdef DMALLOC_INCLUDE
#define YY_MALLOC_DECL
#endif

int prev_state[500];

int lineno = 1;

extern FILE *LOGFILE;

extern struct cont_LVAL { 
        int  tok_id;
        char *string;
        int intval;
 } cont_lval, cont_lval_pushback;


%}

/* Definitions */

DIGIT  [0-9]
DIGITS [0-9]+	
DECIMALS {DIGITS}.{DIGITS}
FLOATS {DECIMALS}[Ee]{DIGITS}
LETTER [a-zA-Z]

nullmdo "<!>"
comments "<!--".*"-->"

ws  [ \t]+
nl  \n
cr  \r
eq [ \t]*"="[ \t]*
name_token [a-zA-Z0-9][a-zA-Z0-9\-_.]*
quoted_name_token [''""][a-zA-Z0-9][a-zA-Z0-9\-_.]*[''""]
filename "/"[a-zA-Z][a-zA-Z0-9\-\_\/\.\~]*
relfilename "."[./]*([^/ \n\t<>]*"/")*[^/ \n\t<>]*[ \n\t]
localfilename [^/ \n\t<>]*"/"([^/ \n\t<>]*"/")*[^/ \n\t<>]*[ \n\t]
quoted_filename [''""]"/"[a-zA-Z][a-zA-Z0-9\-\_\/\.\~]*[''""]
dosfilename [a-zA-Z]":"[a-zA-Z0-9/\-\_\.\~\\]*

/* Start conditions */
%x sgml_comment
%x filecont

%%

"<FILECONT" {BEGIN(filecont); return (FILECONT_START);}

"</FILECONT>" {return (FILECONT_END);}

<filecont>"ID=" {return(FILECONT_NUMBER);}
<filecont>"MIN=" {return(FILECONT_MIN);}
<filecont>"MAX=" {return(FILECONT_MAX);}

<filecont>{DIGITS} { cont_lval.intval = atoi(cont_text);
                     return NUMBER;
                   }
<filecont>">" {
          BEGIN(INITIAL);
          return(ATTRIBUTES_END);
          }

<*>{filename}  { cont_lval.string = strdup(cont_text); 
	     return FILENAME_LITERAL;
	}

<*>{localfilename}  { cont_lval.string = strdup(cont_text); 
	    /* printf("Matched %s\n", cont_lval.string);  */
	     return FILENAME_LITERAL;
	}

<*>{relfilename}  { cont_lval.string = strdup(cont_text); 
	    /* printf("Matched %s\n", cont_lval.string);  */
	     return FILENAME_LITERAL;
	}

<*>{quoted_filename}  { cont_lval.string = strdup(cont_text); 
	     cont_text[strlen(cont_text)-1] = '\0'; /* kill final quote */
	     cont_lval.string = strdup(cont_text+1); 
	     cont_lval.intval = FILENAME_LITERAL;
	     return FILENAME_LITERAL;
	}

<*>{dosfilename}  { cont_lval.string = strdup(cont_text); 
	     return FILENAME_LITERAL;
	}


<*>{nl}  { lineno++; /* count and ignore newlines */}

<*>{comments} /* ignore comments */
<*>{cr} /* ignore carriage returns (for NT/DOS) */

<*>{ws} /* ignore whitespace */;

.	{ printf("Unrecognized char ='%c' in cont_parser\n",
		cont_text[0]); 
	  if (strlen(cont_text) > 1)
	     printf("Unmatched string: %s\n", cont_text);
	  return cont_text[0];
	}


%%



int 
next_token()
{
   int token;
   /* check if there is a token pushed back or read ahead */
   if (cont_lval_pushback.tok_id != 0) {
	cont_lval = cont_lval_pushback;
        cont_lval_pushback.tok_id = 0;
	return (cont_lval.tok_id);
   }
   cont_lval.intval = 0;
   /* otherwise read the next token */
   token = cont_lex();

   cont_lval.tok_id = token;
   return (token);
} 

int push_back_token(int token)
{
    cont_lval_pushback = cont_lval;
    cont_lval_pushback.tok_id = token;
}


int cont_restart_scanner(FILE *newfile) 
{
	yyrestart(newfile);

	BEGIN(INITIAL);
}


/* this routine is called by the parser created by LLgen */
int CONTmessage(int token)
{
   extern int CONTsymb;	

    fprintf(stdout,"CONTmessage called: %d\n", token); 

   switch (token) {
	case -1: 
		break;
	
	case 0: 
		fprintf(LOGFILE,"Continuation file parsing problem at line %d\n",
			lineno);
		fprintf(LOGFILE,"EXITING\n");
		exit(1);
		break;
	default: 
		fprintf(LOGFILE,"Continuation file parsing problem at line %d\n",
			lineno);
		fprintf(LOGFILE,"pushing back token type %d\n",token);

		push_back_token(token);

		break;
   }

}

int cont_wrap () 
{
 /*  fprintf(LOGFILE,"cont_wrap -- End of input file\n"); */
 /*  yy_delete_buffer(YY_CURRENT_BUFFER); */
  return (1); 
}
