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
*	Header Name:	q_grammar.g
*                       this is an LLgen grammar file 
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	defines the grammar for a query language used
*                       for submitting z39.50 queries via Tcl/Tk. The
*                       language is a fairly simple one, and in the
*                       parsing process it is converted into a tree
*			structure, which is then traversed to generate
*                       a Z39.50 RPN query.
*
*	Usage:		LLgen the file then call z__parse
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		9/6/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
{
#include <stdlib.h>
#ifndef WIN32
#ifndef SOLARIS
#include <strings.h>
#endif
#endif
#include <string.h>
#include "tcl.h"

/* Z39.50 Library includes */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z_parse.h"

#include <dmalloc.h>

extern FILE *LOGFILE; /* should be opened or set to stderr in main */

/* #ifndef SOLARIS
 *#define strdup(s) (char *)(strcpy(((char *)malloc((strlen(s)+1))),(s)))
 *#endif
 */

#define TRUE 1
#define FALSE 0

/* uncomment the following for loads of tracing info to stdout */
/* #define DEBUGCMD 1 */

/* z__lval contains the current values from the lexical analysis */

struct Z__LVAL { 
        int  tok_id;
        char *string;
        int intval;
	int attr_set;
	int useval;
	int relval;
	int posval;
	int structval;
	int truncval;
	int compval;
	int other_attr_type;
	int other_attr_val;
 } z__lval, z__lval_pushback;


int parse_errors = 0; 
int Z3950_VERSION_3 = 1;
char *lastfreedvalue = NULL;

char *TKZquery_buffer; /* pointer to query buffer in calling program */


/* the following are defined in the libz3950 zdefs.h and ztypes.h files */
RPNQuery *rpn_query_ptr;
RPNStructure *top_rpn = NULL, *last_rpn = NULL;

#define STACKSIZE 200
struct stack {
	int top;
	RPNStructure *items[STACKSIZE];
} operstack;


extern StringOrNumeric *MakeStringOrNumeric(char *string, 
                                            int number, int which);


/* Forward  declarations for stack handling routines */
int empty(struct stack *ps);
RPNStructure *pop(struct stack *ps);
void popandtest(struct stack *ps, RPNStructure **px, int *pund);
void push (struct stack *ps, RPNStructure *x);
RPNStructure *stacktop(struct stack *ps);


/* where to put the postfix form of the query */
RPNStructure *postr[400];
int post_outpos = 0;

/* forward declaration of function to convert from postfix to a rpn tree */
RPNStructure *build_rpn_tree(RPNStructure *rpn[],int n_rpn);

RPNStructure *topsymb, *symb;
ProximityOperator *tempProx;

int infix_paren_nesting = 0;
int first_next_token_call = 1;

/* for building the "term" values */
char *TEMPvaluebuffer;
int TEMPvaluesize;
/* for result set id specifications */
char *LastResultSetID = NULL;

}
/* force the prefix of generated routines to z_ instead of LL */
%prefix z__;

/* Config file tag defs */
%token INDEXNAME;
%token VALUE;
%token RESULTSETID;
%token RESULTSETSPEC;
%token RESULTSETNAME;
%token AND;
%token OR;
%token NOT;
%token PROXOP;
%token PROXTYPE;
%token PROXDIST;
%token PROXEND;
%token LESS_THAN;
%token LESS_THAN_OR_EQUAL;
%token EQUAL;
%token GREATER_THAN_OR_EQUAL;
%token GREATER_THAN;
%token NOT_EQUAL;
%token PHONETIC;
%token STEM;
%token RELEVANCE;
%token OKAPI_RANK;
%token CORI_RANK;
%token TREC2_RANK;
%token TREC2_FBK_RANK;
%token LANGMOD_RANK;
%token TFIDF_RANK;
%token TFIDF_LUCENE_RANK;
%token NGRAM_FULL;
%token WITHIN;
%token GEO_OVERLAPS;
%token GEO_FULLY_ENCLOSED_WITHIN;
%token GEO_ENCLOSES;
%token GEO_FULLY_OUTSIDE_OF;
%token GEO_NEAR;
%token GEO_MEMBERS_CONTAIN;
%token GEO_MEMBERS_NOT_CONTAIN;
%token GEO_OVERLAPS_RANK;
%token GEO_FULLY_ENCLOSED_WITHIN_RANK;
%token GEO_ENCLOSES_RANK;
%token TIME_BEFORE;
%token TIME_BEFORE_OR_DURING;
%token TIME_DURING;
%token TIME_DURING_OR_AFTER;
%token TIME_AFTER;

%token TRUNC;
%token NOTRUNC;
%token START_ATTRIBUTES;
%token ATTRSETOID;
%token USE_ATTR;
%token REL_ATTR;
%token POS_ATTR;
%token STRUCT_ATTR;
%token TRUNC_ATTR;
%token COMP_ATTR;
%token OTHER_ATTR;
%token END_ATTRIBUTES;


%start z__parse, query ;

query { RPNStructure *s; int underflow; } : 
	{/* initialize */
	operstack.top = -1; /* initialize the stack */
	post_outpos = 0;
	parse_errors = 0;
	rpn_query_ptr = CALLOC(RPNQuery, 1);
	TEMPvaluebuffer = CALLOC(char, 1000);
	TEMPvaluesize = 999;
	lastfreedvalue = NULL;
        LastResultSetID = NULL;
        }

	boolean_query {	
		if (parse_errors == 0) {
		   /* finish up the postfix array of the query */
		   while (!empty(&operstack)) {
			 postr[post_outpos++] = pop(&operstack);
		   }
		   /* build tree of rpn structures */
		   top_rpn = build_rpn_tree(postr,post_outpos);

		   /* reset the root to the top rpn structure */
		   rpn_query_ptr->rpn = top_rpn;

		   /* print the thing in rpn */
		   /* dump_rpn_query(rpn_query_ptr); */
		   /* printf("\n");	             */
		   
		   /* set up for NEXT query */
		
		   FREE(TEMPvaluebuffer);
		   TEMPvaluesize = 0;
		   first_next_token_call = 1;
		   post_outpos = 0;
		   reset_scanner();

#ifdef DEBUGCMD
		   printf("All done....\n");
#endif
		}
		else { /* Error in parsing -- destroy the rpn created */
		   /* finish up the postfix array of the query */
		   while (!empty(&operstack))
		  	 postr[post_outpos++] = pop(&operstack);
	
		   /* build tree of rpn structures */
		   top_rpn = build_rpn_tree(postr,post_outpos);

		   /* and then free it */
		   FreeRPNStructure(top_rpn);
		   FREE(rpn_query_ptr);

		   /* set up for NEXT query */
		   first_next_token_call = 1;
		   post_outpos = 0;
		   reset_scanner();
		}
    	};

boolean_query 
     { RPNStructure *newrpn, lastrpn; int level; int pri; int booltype;} :
     { level = 1;}

	factor resultsetspec?
	[  %while (priority(z__symb) >= level) 
	 boolean_operator(&pri, &booltype) {
		/* what we do here is create a new structure   */
		/* then migrate the last_rpn structure into it */
#ifdef DEBUGCMD
		printf("IDed as boolean op\n");
#endif

		newrpn = CALLOC(RPNStructure, 1);
		newrpn->which = e3_rpnRpnOp;
		newrpn->u.rpnRpnOp.rpn1 = NULL;
		newrpn->u.rpnRpnOp.rpn2 = NULL;
		newrpn->u.rpnRpnOp.op = CALLOC(Operator, 1);
		switch (booltype) {
		    case 0:
			newrpn->u.rpnRpnOp.op->which = e6_and;
			newrpn->u.rpnRpnOp.op->u.and = 0;
			break;

		    case 1:
			newrpn->u.rpnRpnOp.op->which = e6_or;
			newrpn->u.rpnRpnOp.op->u.or = 1;
			break;

		    case 2:
			newrpn->u.rpnRpnOp.op->which = e6_and_not;
			newrpn->u.rpnRpnOp.op->u.and_not = 2;
			break;

		    case 3:
#ifdef DEBUGCMD
	printf("bool type is proximity\n");
#endif
			newrpn->u.rpnRpnOp.op->which = e6_prox;
			newrpn->u.rpnRpnOp.op->u.prox = tempProx;
			break;
		}
		push(&operstack, newrpn);
	      }
	  boolean_query {
		}

	]* ;

boolean_operator (int *pri; int *type) :
	AND {
#ifdef DEBUGCMD
	printf("BOOL AND\n");
#endif
	      *pri = priority(AND);
	      *type = 0;
	    }

	| OR { 
#ifdef DEBUGCMD
	printf("BOOL OR\n");
#endif
	      *pri = priority(OR);
	      *type = 1;
	    }

	| NOT {
#ifdef DEBUGCMD
	printf("BOOL ANDNOT\n");
#endif
	      *pri = priority(NOT);
	      *type = 2;
	    } 

        | proxop { 
#ifdef DEBUGCMD
	printf("proximity Operator"); 
#endif
	      *pri = priority(PROXOP);
	      *type = 3;
	} ;



factor :	

        simplequery
	| 
	'(' { 
#ifdef DEBUGCMD
	printf("parens start %d\n",++infix_paren_nesting);
#endif

		push(&operstack,NULL); /* parens are marked by a null item*/
		
            }
	boolean_query  {  }

	')' { int und;	
#ifdef DEBUGCMD
	printf("parens end %d\n",infix_paren_nesting--);
#endif

		popandtest(&operstack,&topsymb,&und);
		while (!und && topsymb != NULL) {
		    postr[post_outpos++] = topsymb;
		    popandtest(&operstack,&topsymb,&und);
		}
		if (!und)
		   push(&operstack,topsymb);
		
		 topsymb = pop(&operstack);
	    } ;


simplequery
	{ Operand *oper; AttributesPlusTerm *t; AttributeList *l = NULL;
	  AttributeElement *elem; OctetString r; Term *term; 
          RPNStructure *newrpn;
	} :

	 [ INDEXNAME { 
	       if (z__lval.string != NULL) {
#ifdef DEBUGCMD
		    printf("index: %s\n",z__lval.string);
#endif
		    newrpn = CALLOC(RPNStructure, 1); 
		    newrpn->which = e3_op;
		    newrpn->u.op = oper = CALLOC(Operand, 1);
		    oper->which = e4_attrTerm;
		    t = oper->u.attrTerm = 
				CALLOC(AttributesPlusTerm, 1);
		    /* the value returned in the z__lval.intval variable */
		    /* should be the appropriate "Use" number for the index */
		    l = t->attributes = CALLOC(AttributeList, 1);
		    l->next = NULL;
		    /* start the attribute elements list */
		    elem = l->item = CALLOC(AttributeElement, 1);
		    elem->attributeType = 1; /* Use attribute */

		    if (z__lval.intval == -1) {

#ifdef DEBUGCMD
       printf ("Creating complex...\n");
#endif
		         elem->attributeValue.which = e7_complex;
			 elem->attributeValue.u.complex.list =
				    CALLOC(struct list_List2, 1);
		         elem->attributeValue.u.complex.list->item =
				MakeStringOrNumeric(z__lval.string, 0, 0);

			 free(z__lval.string); /* don't need the name any more */
		    }
		    else {
#ifdef DEBUGCMD
       printf ("Creating numeric...\n");
#endif
		         elem->attributeValue.which = e7_numeric;
			 elem->attributeValue.u.numeric = z__lval.useval;	
			 free(z__lval.string); /* don't need the name any more */
		    }
		    /* should probably be doing something with all the */
		    /* attribute values -- sometime                    */
		    if (z__lval.structval != 6) {
   			/* has the phrase bit set in table value */
			AttributeList *new;
			AttributeElement *elem2; 
			new = l->next = CALLOC(AttributeList, 1);
			new->next = NULL;
			elem2 = new->item = CALLOC(AttributeElement, 1);
			elem2->attributeType = 4; /* structure attribute */
			elem2->attributeType = e7_numeric;
			elem2->attributeValue.u.numeric = z__lval.structval;
			elem2->attributeType = 4; /* structure attribute */
		    }
		
		    /* add it to the postfix output */
		    postr[post_outpos++] = newrpn;

		    /* start the term value space */

		    r = (OctetString)CALLOC(GSTR, 1);
		    term = CALLOC(Term,1);
		    t->term = term;
		    term->which = e5_general;
		    term->u.general = r;
		    r->length = 0;
		    r->data = NULL;

		    TEMPvaluebuffer[0] = (char)0;	    

		   }
		   else {
                      l = NULL;
		      TEMPvaluebuffer[0] = (char)0;	    
                   }
	      }
	  attributelist(l, &r)? 
	  relop(l)
	  [VALUE  { 
#ifdef DEBUGCMD
	printf("value: %s\n",z__lval.string);
#endif
		if (parse_errors == 0) {
		    if (r->length == 0) { /* first value */
		    	r->length = strlen(z__lval.string);
			if (r->length < TEMPvaluesize)
			   strcpy(TEMPvaluebuffer, z__lval.string);
		        else {
			    char *tmp;
			    TEMPvaluesize = (r->length + 1) * 2;
			    tmp = CALLOC(char, TEMPvaluesize);
			    strcpy(tmp, z__lval.string);
			    FREE(TEMPvaluebuffer);
			    TEMPvaluebuffer = tmp;
			}
		    }
		    else { /* append values */ int len;
			if (((len = strlen(z__lval.string) 
			      + 1) + r->length) > TEMPvaluesize) {
			    char *tmp;

			    TEMPvaluesize = (len + r->length + 1) * 2;
			    tmp = CALLOC(char, TEMPvaluesize);
			    strcpy(tmp, TEMPvaluebuffer);
			    FREE(TEMPvaluebuffer);
			    TEMPvaluebuffer = tmp;
			} 
		        TEMPvaluebuffer[r->length] = ' ';
		        strcpy(&TEMPvaluebuffer[r->length+1], 
			       z__lval.string);
			r->length += len;
		    
		    }
		    lastfreedvalue = z__lval.string; /* for error checks */
		    FREE(z__lval.string); /* no longer needed */
		  }
		}

	   [TRUNC { AttributeList *new;  AttributeElement *elem;
	        
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}

		if (l->next != NULL) 
		   for(new = l->next; new != NULL; l = new, new = l->next);
		  
		new = l->next = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 5; /* truncation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 101; /* process # in term */
		/* insert the truncation into the buffer */
	        TEMPvaluebuffer[r->length] = '#';
		TEMPvaluebuffer[r->length+1] = (char)0;
		r->length += 1;
		
		}

	   ]?

	   [NOTRUNC { AttributeList *new;  AttributeElement *elem;
	        
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}

		if (l->next != NULL) 
		   for(new = l->next; new != NULL; l = new, new = l->next);
		  
		new = l->next = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 5; /* truncation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 100; /* DO NOT TRUNCATE */
		
		}

	   ]?

	  ]+
	]  {/* add the term to the attributesplusterm structure */
	     if (r->length > 0) {
		r->data = CALLOC(char, r->length + 1);
		strcpy(r->data,TEMPvaluebuffer);
		
	     }
	   } 
	| RESULTSETID   { 
#ifdef DEBUGCMD
   printf("resultsetID: %s parse_errs %d\n",z__lval.string, parse_errors);
#endif
		    newrpn = CALLOC(RPNStructure, 1); 
		    newrpn->which = e3_op;
		    newrpn->u.op = oper = CALLOC(Operand, 1);
		    oper->which = e4_resultSet;
		    r = oper->u.resultSet = (ResultSetId) CALLOC(GSTR, 1);
		    if (z__lval.string != NULL && 
			z__lval.string != lastfreedvalue) {
		       r->length = strlen(z__lval.string);
		       r->data = z__lval.string;
		    }
		    else {
		       parse_errors++;
		       r->length = 0;
		       r->data = NULL;
		    }

		    /* add it to the postfix output */
		    postr[post_outpos++] = newrpn;

		    /* string is NOT freed here */   
		   } ;

relop(AttributeList *l;) 
	{ AttributeList *new, *n;  AttributeElement *elem;
          RPNStructure *newrpn;
	} :

	  LESS_THAN {
#ifdef DEBUGCMD
	 printf("relop: LT\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 1; 
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| LESS_THAN_OR_EQUAL {
#ifdef DEBUGCMD
	 printf("relop: LE\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 2; 
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| EQUAL {
#ifdef DEBUGCMD
	 printf("relop: EQ\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 3;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }


	| GREATER_THAN_OR_EQUAL {
#ifdef DEBUGCMD
	 printf("relop: GE\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 4;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GREATER_THAN {
#ifdef DEBUGCMD
	 printf("relop: GT\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 5;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| NOT_EQUAL {
#ifdef DEBUGCMD
	printf("relop: NEQ\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 6;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }


	| PHONETIC {
#ifdef DEBUGCMD
	 printf("relop: PHONETIC\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 100;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }


	| STEM {
#ifdef DEBUGCMD
	 printf("relop: STEM\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 101;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }


	| RELEVANCE { 
#ifdef DEBUGCMD
	printf("relop: RELEVANCE\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 102;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| OKAPI_RANK { 
#ifdef DEBUGCMD
	printf("relop: OKAPI_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 500;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| CORI_RANK { 
#ifdef DEBUGCMD
	printf("relop: CORI_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 501;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| TREC2_RANK { 
#ifdef DEBUGCMD
	printf("relop: TREC2_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 510;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| TREC2_FBK_RANK { 
#ifdef DEBUGCMD
	printf("relop: TREC2_FBK_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 511;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| TFIDF_RANK { 
#ifdef DEBUGCMD
	printf("relop: TFIDF_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 530;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| TFIDF_LUCENE_RANK { 
#ifdef DEBUGCMD
	printf("relop: TFIDF_LUCENE_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 540;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| LANGMOD_RANK { 
#ifdef DEBUGCMD
	printf("relop: LANGMOD_RANK\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 520;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| NGRAM_FULL { 
#ifdef DEBUGCMD
	printf("relop: NGRAM_FULL\n");
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 580;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| WITHIN {
#ifdef DEBUGCMD
	 printf("relop: WITHIN\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 104;
		elem->attributeSet = NewOID(OID_BIB1);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_OVERLAPS {
#ifdef DEBUGCMD
	 printf("relop: GEO_OVERLAPS\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 7;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_FULLY_ENCLOSED_WITHIN {
#ifdef DEBUGCMD
	 printf("relop: GEO_FULLY_ENCLOSED_WITHIN\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 8;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| GEO_ENCLOSES  {
#ifdef DEBUGCMD
	 printf("relop: GEO_ENCLOSES\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 9;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_FULLY_OUTSIDE_OF {
#ifdef DEBUGCMD
	 printf("relop: GEO_FULLY_OUTSIDE_OF\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 10;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_NEAR {
#ifdef DEBUGCMD
	 printf("relop: GEO_NEAR\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 11;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_MEMBERS_CONTAIN {
#ifdef DEBUGCMD
	 printf("relop: GEO_MEMBERS_CONTAIN\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 12;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| GEO_MEMBERS_NOT_CONTAIN {
#ifdef DEBUGCMD
	 printf("relop:  GEO_MEMBERS_NOT_CONTAIN\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 13;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
	| GEO_OVERLAPS_RANK {
#ifdef DEBUGCMD
	 printf("relop: GEO_OVERLAPS_RANK\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 707;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
| GEO_FULLY_ENCLOSED_WITHIN_RANK {
#ifdef DEBUGCMD
	 printf("relop: GEO_FULLY_ENCLOSED_WITHIN_RANK\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 708;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }
		
	| GEO_ENCLOSES_RANK  {
#ifdef DEBUGCMD
	 printf("relop: GEO_ENCLOSES_RANK\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 709;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }


	| TIME_BEFORE  {
#ifdef DEBUGCMD
	 printf("relop: TIME_BEFORE\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 14;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| TIME_BEFORE_OR_DURING {
#ifdef DEBUGCMD
	 printf("relop: TIME_BEFORE_OR_DURING\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 15;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| TIME_DURING {
#ifdef DEBUGCMD
	 printf("relop: TIME_DURING\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 16;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| TIME_DURING_OR_AFTER {
#ifdef DEBUGCMD
	 printf("relop: TIME_DURING_OR_AFTER\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 17;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	| TIME_AFTER {
#ifdef DEBUGCMD
	 printf("relop: TIME_AFTER\n"); 
#endif
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		elem->attributeType = 2; /* Relation attribute */
		elem->attributeValue.which = e7_numeric;
		elem->attributeValue.u.numeric = 18;
		elem->attributeSet = NewOID(OID_GEO);
		if (l == NULL) {
		    /* l was passed as NULL - I.E. previous attribute defs */
		    newrpn = postr[post_outpos - 1];
		    if (newrpn->which == e3_op) {
		    l = newrpn->u.op->u.attrTerm->attributes ;
		}}
		for (n = l; n->next != NULL; n = n->next);
		n->next = new; }

	|   /* empty */ {
#ifdef DEBUGCMD
	 printf("relop: NONE\n "); 
#endif
		}  ;



attributelist(AttributeList *l; OctetString *in_r) 
	{ Operand *oper; AttributesPlusTerm *t; AttributeList *new, *n;
	  AttributeElement *elem; OctetString r; Term *term; 
          RPNStructure *newrpn; char *currattrset = NULL;
	} :

	START_ATTRIBUTES { 
	      currattrset = NULL;
#ifdef DEBUGCMD
	 printf("parsing attribute list\n "); 
#endif

            if (l == NULL) {
                newrpn = CALLOC(RPNStructure, 1); 
                newrpn->which = e3_op;
                newrpn->u.op = oper = CALLOC(Operand, 1);
                oper->which = e4_attrTerm;
		t = oper->u.attrTerm = 
		  CALLOC(AttributesPlusTerm, 1);
		/* add it to the postfix output */
		postr[post_outpos++] = newrpn;

		/* start the term value space */

		r = (OctetString)CALLOC(GSTR, 1);
		term = CALLOC(Term,1);
		t->term = term;
		term->which = e5_general;
		term->u.general = r;
		r->length = 0;
		r->data = NULL;
		*in_r = r;

	    }

        }
		
	[USE_ATTR { /* use attribute */

        /* printf ("Got the USE_ATTR\n"); */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 1; /* USE attribute */
		if (z__lval.useval > 0) {
		   elem->attributeValue.which = e7_numeric;
		   elem->attributeValue.u.numeric = z__lval.useval;
           if (z__lval.useval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);
		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 
		}
	| REL_ATTR { /* relation attr */
		new = CALLOC(AttributeList, 1);
		if (l == NULL) 
		    t->attributes = new;
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 2; /* Relation attribute */
		if (z__lval.relval > 0) {
		    elem->attributeValue.which = e7_numeric;
		    elem->attributeValue.u.numeric = z__lval.relval;; 
           if (z__lval.relval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);

		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 
		}
	| POS_ATTR {/* position attr */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 3; /* position attribute */
		if (z__lval.posval > 0) {
		    elem->attributeValue.which = e7_numeric;
		    elem->attributeValue.u.numeric = z__lval.posval; 
            if (z__lval.posval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);

		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 
		}
	| STRUCT_ATTR { /* Structure attr */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 4; /* structure attribute */
		if (z__lval.structval > 0) {
		    elem->attributeValue.which = e7_numeric;
		    elem->attributeValue.u.numeric = z__lval.structval; 
            if (z__lval.structval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);
		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 
		}
	
	| TRUNC_ATTR { /* truncation attr */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 5; /* trunc attribute */
		if (z__lval.truncval > 0) {
		    elem->attributeValue.which = e7_numeric;
		    elem->attributeValue.u.numeric = z__lval.truncval; 
           if (z__lval.truncval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n != NULL && n->next != NULL; n = n->next);

		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 
		}
	| COMP_ATTR { /* completeness attr */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = 6; /* completeness attribute */
		if (z__lval.compval > 0) {
		    elem->attributeValue.which = e7_numeric;
		    elem->attributeValue.u.numeric = z__lval.compval; 
            if (z__lval.compval == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);

		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 

		}

	| OTHER_ATTR { /* new attr architecture */
		new = CALLOC(AttributeList, 1);
		new->next = NULL;
		elem = new->item = CALLOC(AttributeElement, 1);
		if (currattrset != NULL) {
		   elem->attributeSet = NewOID(currattrset);
		   FREE(currattrset);
		   currattrset = NULL;
		}
		elem->attributeType = z__lval.other_attr_type;
		if (z__lval.relval > 0) {
		   elem->attributeValue.which = e7_numeric;
		   elem->attributeValue.u.numeric = z__lval.other_attr_val; 
           if (z__lval.other_attr_val == 999999)
              parse_errors++;
		}
		else {
		   elem->attributeValue.which = e7_complex;
		   elem->attributeValue.u.complex.list =
			CALLOC(struct list_List2, 1);
		   elem->attributeValue.u.complex.list->item =
		   MakeStringOrNumeric(z__lval.string, 0, 0);
		   free(z__lval.string); /* don't need the name any more */
		}
		for (n = l; n != NULL && n->next != NULL; n = n->next);

		if (n == NULL) {
		    t->attributes = new;
		    l = new;
		}
		else
		    n->next = new; 

		}

	| ATTRSETOID {
	        currattrset = z__lval.string;
	}
	]+

	END_ATTRIBUTES ;


proxop: 
	PROXOP  { /* ADJ, OADJ, NEAR, ONEAR, FAR, OFAR */
#ifdef DEBUGCMD
	printf("Prox operator Matched\n");
#endif
		tempProx = CALLOC(ProximityOperator, 1);
		tempProx->exclusion = z__lval.useval;
		tempProx->distance = z__lval.compval;
		tempProx->ordered = z__lval.intval;
                tempProx->relationType = 2; /* less than or  equal */
		if (z__lval.truncval == 1) {
		  tempProx->proximityUnitCode.which = e8_private;
		  z__lval.truncval = 0;
		}
		else 
		  tempProx->proximityUnitCode.which = e8_known;
		tempProx->proximityUnitCode.u.known = 2; /* default to word */
	}
	[PROXTYPE {
#ifdef DEBUGCMD
	printf("Prox type Matched\n");
#endif
	if (z__lval.intval < 90) {
	  tempProx->proximityUnitCode.u.known = z__lval.intval;
	  z__lval.intval = 0;
	}
	else {
	  tempProx->proximityUnitCode.u.private_var = z__lval.intval - 90;
	  z__lval.intval = 0;
	}
	}]?
	[PROXDIST {
#ifdef DEBUGCMD
	printf("Prox distance Matched\n");
#endif
		tempProx->distance = z__lval.intval;
		z__lval.intval = 0;
	}]?
	PROXEND {
#ifdef DEBUGCMD
	printf("Prox operator all Matched\n");
#endif
	} ;



resultsetspec:
	RESULTSETSPEC
	RESULTSETNAME { if (z__lval.string) {
		           LastResultSetID = strdup(z__lval.string);
			   FREE (z__lval.string);
		        }
		      } ;

%lexical next_Z_token ;

{/* additional parsing routines -- lexical analyzer, etc. */

int 
next_Z_token()
{
   extern char *z__inputdata; /* query buffer for scanning */
   extern char *z__inputptr; /* current position in z__inputdata */
   extern char *z__inputlim;  /* end of data */
   int token;
   char *tok_ptr;

   /* check if there is a token pushed back or read ahead */
   if (z__lval_pushback.tok_id != 0) {
	z__lval = z__lval_pushback;
        z__lval_pushback.tok_id = 0;
	return (z__lval.tok_id);
   }

   if (first_next_token_call) {
   	z__inputptr = z__inputdata = TKZquery_buffer;
	z__inputlim = z__inputptr + strlen(z__inputdata);
	first_next_token_call = 0;
	z__restart(NULL);
   }
   /* otherwise read the next token */
   token = z__lex();

   z__lval.tok_id = token;
   return (token);
} 

int z_push_back_token(int token)
{
    z__lval_pushback = z__lval;
    z__lval_pushback.tok_id = token;
}



/* this routine is called by the parser created by LLgen */
int z__message(int token)
{
   extern int z__symb;

   parse_errors++;

   switch (token) {
	case -1: 
#ifdef DEBUGCMD
		fprintf(stdout,"Supposed to be end of file??\n");
#endif
		z_push_back_token(0);	
		break;
	
	case 0: 
#ifdef DEBUGCMD
		fprintf(stdout,"token deleted? z__symb = %d z__lval = id %d value %s \n",
		z__symb, z__lval.tok_id, z__lval.string);

		fprintf(stdout,"used to EXIT here...\n");
		/* exit(1); */
#endif
		break;
	default: 
#ifdef DEBUGCMD
		fprintf(stdout,"token pushed back: CFsymb = %d z__lval = id %d value %s \n",
		z__symb, z__lval.tok_id, z__lval.string);
#endif		
		z_push_back_token(token);

		break;
   }

}

int priority (int tok) {
	switch (tok) {
	case AND:
	     return (3);
	     break;
	case OR:
	     return (3);
	     break;
	case NOT:
	     return (3);
	     break;
	case PROXOP:
	     return (3);
	     break;
	default:
	     return(0);
	}
}


dump_rpn_query(RPNQuery *query) {

  int parse_attribute_set;

  if (query == NULL) {
     printf("No RPN Query to print -- pointer is NULL in dump_rpn_query\n");
     return ;
  }
  /* otherwise dump the query structures by recursively printing them */
  /* set the attribute table */
  if (query->attributeSet != NULL) {
	  if (strcmp(query->attributeSet->data, OID_BIB1) == 0)	
		parse_attribute_set = BIB_1_ATTR_SET;
	  else if (strcmp(query->attributeSet->data, OID_EXP1) == 0)
		parse_attribute_set = EXP_1_ATTR_SET;
	  else if (strcmp(query->attributeSet->data, OID_GILS) == 0)
		parse_attribute_set = GILS_ATTR_SET;
	  else if (strcmp(query->attributeSet->data, OID_GEO) == 0)
		parse_attribute_set = GEO_ATTR_SET;

  }
  else 
	parse_attribute_set = BIB_1_ATTR_SET;

  dump_rpn_structure(query->rpn, parse_attribute_set);
}

dumpprox(ProximityOperator *p) {
	printf(") !");
	if (p->ordered)
		printf("O");
	printf("PROX");
	switch (p->proximityUnitCode.u.known) {
	case 1: printf("/CHAR");
		break;
	case 2: printf("/WORD");
		break;
	case 3: printf("/SENTENCE");
		break;
	case 4: printf("/PARAGRAPH");
		break;
	case 5: printf("/SECTION");
		break;
	case 6: printf("/CHAPTER");
		break;
	case 7: printf("/DOCUMENT");
		break;
	case 8: printf("/ELEMENT");
		break;
	case 9: printf("/SUBELEMENT");
		break;
	case 10: printf("/ELEMENTTYPE");
		break;
	case 11: printf("/BYTE");
		break;

	}
	printf("/%d", p->distance);
	printf(" (");
	
}

dump_rpn_structure(RPNStructure *s, int attrset) {
  
  Operand *oper; 
  AttributesPlusTerm *t; 
  AttributeList *l;
  AttributeElement *elem; 
  Term *term;
  char *stringdata;
  char attrdata[500], tmp[200];
  OctetString r; 
  int i;
  extern INDEX all_indexes[];
  

  if (s == NULL) return;
  
  if (s->which == e3_rpnRpnOp) {
    printf("(");
    dump_rpn_structure(s->u.rpnRpnOp.rpn1, attrset); /* recursively print them */
    if (s->u.rpnRpnOp.op->which == e6_and)
      printf(") AND (");
    if (s->u.rpnRpnOp.op->which == e6_or)
      printf(") OR (");
    if (s->u.rpnRpnOp.op->which == e6_and_not)
      printf(") NOT (");
    if (s->u.rpnRpnOp.op->which == e6_prox)
      dumpprox(s->u.rpnRpnOp.op->u.prox);
    dump_rpn_structure(s->u.rpnRpnOp.rpn2, attrset); /* recursively print them */
    printf(")");
  }
  else if (s->which == e3_op) {
    oper = s->u.op;
    if (oper->which == e4_resultSet) 
      printf("RESULTSETID: %s",oper->u.resultSet->data);
    else {
      char *attname = "LOCALNAME";
      char *relat[] = { " ", " < ", " <= ", " = ", " >= ",
			" > ", " != ", " ? ", " % ", " @ ", " ",
			" <=> ",  NULL};
      char *relopr = relat[0];
      int nameindex = 0, useval = 0;
      
      t = oper->u.attrTerm;
      
      attrdata[0] = '\0';
      
      for (l = t->attributes; l ; l = l->next) {
	elem = l->item;
	if (elem->attributeType == 1) {
	  if (elem->attributeValue.which == e7_numeric) {	        
	    useval = elem->attributeValue.u.numeric;
	    
	    for (i = 0; all_indexes[i].attributes[1] != 0; i++) {
	      if (useval == all_indexes[i].attributes[1]
		  && (all_indexes[i].attributes[0] & attrset) 
		  == attrset) {
		attname = &all_indexes[i].name[0];
		nameindex = i;
		break;
	      }
	    }
	    if (elem->attributeSet) {
	      sprintf(tmp," %s %d=%d", elem->attributeSet->data, 
		      elem->attributeType, useval);
	    }
	    else 
	      sprintf(tmp," %d=%d", elem->attributeType, useval);
	    strcat (attrdata, tmp);
	  }
	  else { /* complex attribute -- we handle only string */
	    struct list_List2 *l1;
	    struct semanticAction_List3 *l2;
	    for (l1 = elem->attributeValue.u.complex.list;
		 l1; l1 = l1->next) {
	      if (elem->attributeSet) {
		sprintf(tmp," %s %d=%d", elem->attributeSet->data, 
			elem->attributeType, useval);
		if (l1->item->which == e25_numeric) {
		  sprintf(tmp," %d=%d", elem->attributeType,
			  l1->item->u.numeric);
		  
		} 
		else {
		  sprintf(tmp," %s %d='%s'", elem->attributeSet->data, 
			  elem->attributeType,
			  l1->item->u.string->data);
		}
	      }
	      else {
		if (l1->item->which == e25_numeric) {
		  sprintf(tmp," %d=%d", elem->attributeType,
			  l1->item->u.numeric);
		  
		} 
		else {
		  sprintf(tmp," %d='%s'", elem->attributeType,
			  l1->item->u.string->data);
		  
		}
	      }
	      strcat (attrdata, tmp);
	      
	    }
	  }
	}
	else if (elem->attributeType == 2) {
	  int offset;
	  
	  if (elem->attributeValue.which == e7_numeric) {
	    if (elem->attributeValue.u.numeric < 12) 
	      offset = elem->attributeValue.u.numeric;
	    else
	      offset = elem->attributeValue.u.numeric - 93;
	    
	    if (offset > 11) /* version 3 z39.50 */
	      offset = 0;

	    relopr = relat[offset];

	    if (elem->attributeSet) {
	      sprintf(tmp," %s %d=%d", elem->attributeSet->data, 
		      elem->attributeType, 
		      elem->attributeValue.u.numeric);
	      if (strcmp(elem->attributeSet->data, OID_BIB1) != 0)
	         relopr = " ";
	    }
	    else 
	      sprintf(tmp," %d=%d", elem->attributeType, 
		      elem->attributeValue.u.numeric);
	  }
	  else { /* complex attribute -- we handle only string */
	    struct list_List2 *l1;
	    struct semanticAction_List3 *l2;
	    for (l1 = elem->attributeValue.u.complex.list;
		 l1; l1 = l1->next) {
	      if (l1->item->which == e25_numeric) {
		sprintf(tmp," %d=%d", elem->attributeType,
			l1->item->u.numeric);
		
	      } 
	      else {
		sprintf(tmp," %d='%s'", elem->attributeType,
			l1->item->u.string->data);
		
	      }
	      strcat (attrdata, tmp);
	      
	    }
	  }
	  strcat (attrdata, tmp);	
	  
	}
	else if (elem->attributeType == 4) {
	  for (i = nameindex; 
	       all_indexes[i].attributes[1] != 0; 
	       i++) {
	    if (useval == all_indexes[i].attributes[1]
		&& elem->attributeValue.u.numeric ==
		all_indexes[i].attributes[4]) {
	      attname = &all_indexes[i].name[0];
	      nameindex = i;
	      break;
	    }
	  }
	  
	  if (elem->attributeSet) {
	    sprintf(tmp," %s %d=%d", elem->attributeSet->data, 
		    elem->attributeType, 
		    elem->attributeValue.u.numeric);
	  }
	  else 
	    sprintf(tmp," %d=%d", elem->attributeType, 
		    elem->attributeValue.u.numeric);
	  
	  strcat (attrdata, tmp);	
	  
	}
	else {
	  
	  if (elem->attributeValue.which == e7_numeric) {
	    if (elem->attributeSet) {
	      sprintf(tmp," %s %d=%d", elem->attributeSet->data, 
		      elem->attributeType, 
		      elem->attributeValue.u.numeric);
	    }
	    else 
	      sprintf(tmp," %d=%d", elem->attributeType, 
		      elem->attributeValue.u.numeric);
	  }
	  else { /* complex attribute -- we handle only string */
	    struct list_List2 *l1;
	    struct semanticAction_List3 *l2;
	    for (l1 = elem->attributeValue.u.complex.list;
		 l1; l1 = l1->next) {
	      if (l1->item->which == e25_numeric) {
		sprintf(tmp," %d=%d", elem->attributeType,
			l1->item->u.numeric);
		
	      } 
	      else {
		sprintf(tmp," %d='%s'", elem->attributeType,
			l1->item->u.string->data);
		
	      }
	    }
	  }
	  
	  strcat (attrdata, tmp);	
	  
	}
	
      }
      term = t->term;
      switch (term->which) {
      case e5_general:
      case e5_characterString:
      case e5_oid:
      case e5_dateTime:
	printf("%s[%s]%s{%s}",attname,attrdata, relopr, 
	       term->u.general->data);
	break;
      case e5_numeric:
	printf("%s[%s]%s{%d}",attname, attrdata, relopr, 
	       term->u.numeric);
	break;
      case e5_nullVal:
	printf("%s[%s]%s{NULL_VAL}",attname, attrdata,
	       relopr);
	break;
      case e5_external:				
	printf("%s[%s]%s{EXTERNAL??}",attname, attrdata,
	       relopr);
	break;
      case e5_integerAndUnit:
	printf("%s[%s]%s{%d %s}",attname, attrdata, 
	       relopr, 
	       term->u.integerAndUnit->value,
	       term->u.integerAndUnit->unitUsed->unitSystem->data);
	break;
      }
    }	   
  }
  return ;
  
}

/* the following handle the stack manipulations */

int
empty(struct stack *ps)
{
   if(ps->top == -1)
	return(TRUE);
   else
	return(FALSE);
}

RPNStructure *pop(struct stack *ps)
{
   if (empty(ps)) {
        if (LOGFILE != NULL)
		fprintf(LOGFILE, "stack underflow in pop\n"); 
	else
		fprintf(stderr, "stack underflow in pop\n"); 
	parse_errors++;
     	return(NULL);	
   }
   return (ps->items[ps->top--]);
}

void popandtest(struct stack *ps, RPNStructure **px, int *pund) 
{
   if (empty(ps)) {
	*pund = TRUE;
   	return;
   }
   *pund = FALSE;
   *px = ps->items[ps->top--];
   return;
}

void push (struct stack *ps, RPNStructure *x)
{
   if (ps->top == STACKSIZE - 1) {
        if (LOGFILE != NULL)
	   fprintf(LOGFILE, "stack overflow in push\n");
	else
	   fprintf(stderr, "stack overflow in push\n");
	exit(1);
   }
   else
	ps->items[++(ps->top)] = x;
   return;
}

RPNStructure *stacktop(struct stack *ps)
{
   if (empty(ps))
	return (NULL);
   else
	return (ps->items[ps->top]);
}


dump_stack()
{
  int i;

  for (i = 0; i < post_outpos; i++)
	dump_rpn_structure(postr[i],1);	

}

/* forward declaration of function to convert from postfix to a rpn tree */
RPNStructure *build_rpn_tree(RPNStructure *rpn[],int n_rpn)
{
  int i;
  RPNStructure *arg1, *arg2, *opr;
 
  operstack.top = -1; /* initialize the stack */

  for (i = 0; i < n_rpn; i++) {
	/* push any operands (index/relop/value triples) onto stack */
	if (rpn[i]->which == e3_op)
	   push(&operstack,rpn[i]);
	else { /* its an operator so put the pointers right */
		rpn[i]->u.rpnRpnOp.rpn2  = arg2 = pop(&operstack);
		rpn[i]->u.rpnRpnOp.rpn1 = arg1 = pop(&operstack);
		if (arg1 == NULL || arg2 == NULL) {
			return(NULL);
		}
		push(&operstack,rpn[i]);
	}
  }
  /* the top-most operator in the tree should be the only thing left */
  /* on the top of the stack, so return it...                        */
  return (pop(&operstack));	   
}

int parser_cleanup () 
{
  operstack.top = -1; /* initialize the stack */
  post_outpos = 0;
  parse_errors = 0;
  first_next_token_call = 1;
  lastfreedvalue = NULL;
}

/* end of additional parsing routines */ }










