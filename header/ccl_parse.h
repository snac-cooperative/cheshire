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
#ifndef CCL_PARSE
#define CCL_PARSE

#define CCL_OK		1
#define CCL_ERROR	-1

typedef enum {
    ANDOP, 
    EQOP, 
    GEOP, 
    GTOP, 
    IDOP, 
    INTOP, 
    LABELOP, 
    LEOP, 
    LTOP, 
    NEOP, 
    NOTOP, 
    NULLOP, 
    OROP, 
    PROGOP, 
    STROP, 
    INDEX_OP, 
    FIND_OP, 
    ELEM_OP, 
    SETID_OP, 
    LIMIT_OP,
    WORD_OP, 
    PHRASE_OP, 
    NUMBER_OP, 
    WITHIN_OP, 
    NEXT_OP,
    REL_OP, 
} ASTTYPE;

typedef struct ast	AST;
struct ast {
	ASTTYPE	op;		/* node operator_var */
	char    *attr;		/* attribute */
	AST	*first;		/* first child */
	AST	*next;		/* next child  */
	AST	*last;		/* last child  */
	AST	*parent;	/* parent node */
	int	line;		/* line number */
	int	column;		/* column number */
};

void initparser();
void yyerror(char *s);
AST *NewAST(ASTTYPE nodeType);
AST *NewUnaryAST(ASTTYPE, AST *);
AST *NewBinaryAST(ASTTYPE nodeType, AST *left, AST *right);
AST *AttachSiblingAST(AST *left, AST *right);
AST *SetAttr(AST *, char *);
void AttachChildAST(AST *parent, AST *child);

#endif /* CCL_PARSE */

