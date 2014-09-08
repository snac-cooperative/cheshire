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
#ifndef STREE_H
#define STREE_H

typedef enum {GT, LT, NE, GE, LE, EQ} LREL;
typedef enum {AND, OR, NOT}	BOOL;
typedef enum {TERMINAL, INTERNAL}	NODETYPE;
typedef enum {SETNAME, SEARCHKEY, LIMITT, RANGET, WITHINT, NEXTT} TNODETYPE;

typedef struct range {
	char	*from;
	char	*to;
} RANGE;

typedef struct proximity {
	int	order;
	char	*distance;
	char	*first;
	char	*second;
} PROXIMITY;

typedef struct limit {
	/*
	LREL	relation;
	*/
	char	*relation;
	char	*value;
} LIMIT;

typedef	struct node	NODE;

typedef struct internal {
    BOOL	op;		/* Boolean operator_var.	*/
    NODE	*left;		/* Left subtree.	*/
    NODE	*right;		/* Right subtree.	*/
} INTERNODE;   

typedef struct terminal {
    char	*index;
    TNODETYPE	ttype;		/* Terminal node type	*/ 
    union {
    	char	*key;		/* Key to be searched.	*/
    	char	*setname;	/* Result set name.	*/
	LIMIT	*limit;		
	RANGE	*range;
	PROXIMITY	*proximity;
    } u;
} TERMNODE;

struct node {
    NODETYPE	type;		/* Node type		*/
    union {
	INTERNODE *internal;
	TERMNODE  *terminal;
    } u;
    void	*result;	/* Search result.	*/
};

/*      Dynamic string structure definition.
 */
 typedef struct dstr {
	 int     size;    /* current size of the data.    */
	 int     max;     /* the amount of space allocated to the data    */
	 char    *data;
 } DSTR;
			  
NODE 	*alloc_node();
TERMNODE	*alloc_terminal();
INTERNODE	*alloc_internal();
RANGE	*alloc_range();
PROXIMITY	*alloc_proximity();

void	print_terminal(TERMNODE *);
void	print_internal(INTERNODE *);
void	print_node(NODE *);

#endif	/* STREE_H	*/

