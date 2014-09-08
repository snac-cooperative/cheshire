/*
 *  Copyright (c) 2005 [see Other Notes, below]. The Regents of the
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
 * Copyright (c) 2005 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sims.berkeley.edu
 *		School of Information Management and Systems, UC Berkeley
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
/* TKZ_Highlight -- Scan data and highlight specified words optionally    */
/* performing stemming conversion and matching stemmed versions of words  */
/* The located words are surrounded with the strings supplied (should be  */
/* html formatting tags for html output, etc.                             */
/**************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#ifndef SOLARIS
#ifndef WIN32
#include <strings.h>
#endif
#endif
#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif
#include "tcl.h"

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include "z_parse.h"

extern RPNQuery *queryparse(char *query, char *attributeSetId);

extern int ToLowerCase(char *data);

extern char *Stem(char *word);


char *extr_terms_terms(AttributesPlusTerm *attrPlusTerm)
{
  if (attrPlusTerm == NULL) 
    return(NULL);
  return(strdup(attrPlusTerm->term->u.general->data));
}

char *extr_terms_rpn_structure(RPNStructure *s) {
  char *left_res;
  char *right_res;
  char *result_res;
  int len;
  Operand *oper; 

  if (s == NULL) return (NULL); /* This SHOULD never happen */

  if (s->which == e3_rpnRpnOp) {
    /* recursively process the left hand side ... */
    left_res = extr_terms_rpn_structure(s->u.rpnRpnOp.rpn1); 
    if (left_res == NULL && (s->u.rpnRpnOp.op->which == e6_and
			     || s->u.rpnRpnOp.op->which == e6_and_not))
      return (NULL);
    /* recursively process the right hand side ... */
    right_res = extr_terms_rpn_structure(s->u.rpnRpnOp.rpn2);
    if (right_res == NULL && s->u.rpnRpnOp.op->which == e6_and)
      return (NULL);
    
    /* Have a left and right side, so process the operator */
    len = strlen(left_res) + strlen(right_res) + 2;
    result_res = CALLOC(char, len);

    strcpy(result_res, left_res);
    strcat(result_res, " ");
    strcat(result_res, right_res);
    FREE(left_res);
    FREE(right_res);
    
    return(result_res);

  }
  else if (s->which == e3_op) {

    oper = s->u.op;

    if (oper->which == e4_resultSet) {
      char *dummy;
      
      dummy = CALLOC(char, 1);
      return(dummy);
    }
    else { 
      /* Have to process a term or set of terms */
      return(extr_terms_terms(oper->u.attrTerm));
    }	   
  }
  else
    return(NULL); /* error: neither operand or binop?? */
}

char *stem_rep(char *instring)
{
  char *result;
  char *origword;
  char *stemptr;
  char *ptr;
  char *x;
  int i, wordlen, resultlen;

  result = strdup(instring);

  resultlen = strlen(result);
  
  origword = strtok_r (result, " .,;:'!?()_=|{}[]@$#%^&*+<>?|~`\"", &ptr);

  do {
    wordlen = strlen(origword);
    stemptr = Stem(origword);
    /*for (x = origword; x < ptr; x++)
     * *x = ' ';
     */
    /* strcpy(origword, stemptr); */
    if (wordlen > strlen(origword)) {
      for(x = origword + strlen(origword); x < ptr; x++) 
	*x = '\0';
    }
    origword = strtok_r(NULL, " .,;:'!?()_=|{}[]$@#%^&*+<>?|~`\"", &ptr);
  } while (origword != NULL);
  
  for (i = 0; i < resultlen; i++) {
    if (result[i] == '\0') 
      result[i] = ' ';
  } 

  return(result);

}

char *extr_terms_query(RPNQuery *RPN_Query)
{
  if (RPN_Query == NULL) 
    return (NULL);
  
  /* should check RPN_Query->attributeSetId to be sure it is one */
  /* we can process                                              */
  return (extr_terms_rpn_structure(RPN_Query->rpn));
  
}

int
TKZ_HighlightCmd(session, interp, argc, argv)
     ClientData session;			/* Current Session */
     Tcl_Interp *interp;			/* Current interpreter. */
     int argc;				/* Number of arguments. */
     char **argv;			/* Argument strings. */
{
  char resultswork[1000];
  char *query;
  char *data;
  char *start_string;
  char *end_string;
  int stem = 0;
  int datalen;
  char *stem_source, *searchterms, *next_term, *word_ptr;
  char *startword, *endword;
  char *result_buffer;
  char *temp1, *temp2;
  char *work_buffer;
  char *startdata, *enddata, *startreal, *endreal, *orig_buffer;
  int cplen, startoffset, endoffset, wordoffset, buffoffset;
  

  RPNQuery *sRPNQuery;

  if (argc < 5 || argc > 6) {
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		     " <-stem> \"query\" \"data\" \"insert_before\" \"insert_after\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }
  
  if (strcasecmp(argv[1],"-STEM") == 0) {
    if (argc == 6) {
      stem = 1;
      query = argv[2];
      data = argv[3];
      start_string = argv[4];
      end_string = argv[5];

    }
    else {
      Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
		       " <-stem> \"query\" \"data\" \"insert_before\" \"insert_after\"", 
		       (char *) NULL);
      return TCL_ERROR;
    }
  } 
  else {
      stem = 0;
      query = argv[1];
      data = argv[2];
      start_string = argv[3];
      end_string = argv[4];
  }
  /* first we reparse the query... */

  /* remove the 'verb' */
  if (strncasecmp(query,"zfind",5) == 0
      || strncasecmp(query,"search",6) == 0
      || strncasecmp(query,"lfind",5) == 0
      || strncasecmp(query,"cheshire_search",14) == 0) {
    query = strchr(query,' ');
    query++;
  }

  /* call the command language parser to build an RPN query */
  sRPNQuery = queryparse(query, NULL);

  /* any problems are signaled by the external value parse_errors */
  if (sRPNQuery == NULL) {
    Tcl_AppendResult(interp, "Unable to parse query: ", query, (char *) NULL);
    return TCL_ERROR;
  }
  
  searchterms = extr_terms_query(sRPNQuery);

  /* first we have to create the stemmed version of words if they are */
  /* wanted                                                           */

  /*printf("Searchterms are '%s'\n", searchterms); */

  /* initialize all of the buffers, etc... */
  datalen = strlen(data);

  if (stem) {
    temp1 = stem_rep(searchterms);
    work_buffer = stem_rep(data);
    FREE(searchterms);
    searchterms = temp1;
  }
  else {
    work_buffer = strdup(data);
  }

  ToLowerCase(work_buffer);

  orig_buffer = strdup(data);
  
  result_buffer = CALLOC(char, (strlen(work_buffer) * 2));
  temp1 = CALLOC(char, (strlen(work_buffer) * 2));

  startdata = work_buffer;
  startreal = orig_buffer;
  buffoffset = 0;

  ToLowerCase(searchterms);
  /* find first token */
  word_ptr = strtok_r (searchterms, " .,;:'!?()_=|{}[]$@#%^&*+<>?|~`\"", &next_term);
  /* a buffer full of only blanks return the data...*/
  if (word_ptr == NULL) {
    Tcl_AppendResult(interp, data, (char *) NULL);
    return TCL_OK;
  }

  do {

    /* for this word, go through the entire string and make substitutions */
    while ((startword = strstr(startdata, word_ptr)) != NULL) {
      
      cplen = wordoffset = startword - startdata;

      enddata = startword + strlen(word_ptr);
      
      if ((wordoffset == 0 || isspace(*(startword-1)) || ispunct(*(startword-1))|| *(startword-1) == '>')
	  && (isspace(*(enddata)) || ispunct(*enddata) || *(enddata) == '<' || *(enddata) == '\0')) {
	/* copy the real data to the result buffer (temp) */
	if (wordoffset > 0)
	  strncat(result_buffer, startreal, wordoffset);
	strcat(result_buffer, start_string);
	strncat(result_buffer, startreal+wordoffset, strlen(word_ptr));
	strcat(result_buffer, end_string);
	startreal = startreal + wordoffset + strlen(word_ptr);

	/* copy the work data to the temp1 buffer */
	if (wordoffset > 0)
	  strncat(temp1, startdata, wordoffset);
	strcat(temp1, start_string);
	strncat(temp1, startdata+wordoffset, strlen(word_ptr));
	strcat(temp1, end_string);

      }
      else {
	/* it isn't actually a separate word, but part of one... */
	/* so copy over that part of the data... */
	strncat(result_buffer, startreal, wordoffset + strlen(word_ptr));
	startreal = startreal + wordoffset + strlen(word_ptr);
	strncat(temp1, startdata, wordoffset + strlen(word_ptr));
	enddata = startdata + wordoffset + strlen(word_ptr);
      }

      startdata = enddata;

    }

    /* copy any remaining parts of the strings over... */
    strcat(result_buffer, startreal);
    strcat(temp1,startdata);

    FREE(work_buffer);
    work_buffer = startdata = temp1;
    temp1 = CALLOC(char, (strlen(work_buffer) * 2));

    FREE(orig_buffer);
    startreal = orig_buffer = result_buffer;
    result_buffer = CALLOC(char, (strlen(work_buffer) * 2));

    /* set for the next round -- results are in startreal */

    /* get the next word from the current query buffer     */
    word_ptr = strtok_r (NULL, " .,;:'!?()_=|{}[]$@#%^&*+<>?|~`\"", &next_term);

  } while (word_ptr != NULL);  

  /* printf("RESULTS are '%s'\n", startreal); */

  FREE(temp1);
  FREE(result_buffer);
  

  Tcl_AppendResult(interp, startreal, (char *) NULL);
  return TCL_OK;
  
} 



