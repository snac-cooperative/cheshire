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
#include <stdio.h>
#include <stdlib.h>
#include "z3950_3.h"

int PutLEN(DBUF *, int);
int PutTAG(DBUF *, TAG_CLASS, TAG_FORM, int);


ASN_NODE *
NewAsnNode(class_var, type, tagno)
int	class_var;
int	type;
int	tagno;
{
    	ASN_NODE *asnNode = (ASN_NODE *)malloc(sizeof(ASN_NODE));
	if (asnNode == NULL)
		return NULL;
    	asnNode->length = 0;
    	asnNode->class_var = class_var;
    	asnNode->type = type;
    	asnNode->tagno = tagno;
    	asnNode->dbuf = NewDbuf();
	if (asnNode->dbuf == NULL) 
		return NULL;
    	asnNode->parent = NULL;
    	asnNode->head = NULL;
    	asnNode->next = NULL;
    	asnNode->tail = NULL;
    	return asnNode;
}


/*
 * Free the space of the parent node and all of its descendants.
 */
void
FreeAsnNode(node)
ASN_NODE 	*node;
{

    	if (node == NULL)
		return;

	FreeAsnNode(node->head);
	FreeAsnNode(node->next);
	
	if (node->dbuf)
    		(void) FreeDbuf(node->dbuf);

    	(void) free((char *)node);
}

/*
 * Append a new_var node to the child node list of the parent
 * node. Both nodes must not be NULL.
 */
ASN_NODE *
Attach(parent, child)
ASN_NODE *parent; 
ASN_NODE *child;
{
    	ASN_NODE *astp;

	if (parent == NULL)
		return (ASN_NODE *)NULL;
	if (child == NULL)
		return (ASN_NODE *)NULL;
    	for (astp = child; astp != NULL; astp = astp->next) 
		astp->parent = parent;
    	if (parent->head == NULL) {
		parent->head = child;
		parent->tail = child;
    	} else {
		parent->tail->next = child;
		parent->tail = child;
    	}
	return parent;
}

/*
 *	LengthAsnTree and LengthAsnTree2 should be combined.
 */

/*
 * Calculate the length at each node, then encode the tag and
 * length. Write the result into the dynamic buffer allocated
 * to the node. Return -1 if an error has occurred.
 */
int
LengthAsnTree(p)
ASN_NODE *p;
{
    	ASN_NODE *child;
	int len=0;

    	if (p == NULL)
		return 0;
    	if (p->type == 1) {	/* CONSTRUCTED	*/
		p->length = 0;
       		for (child=p->head; child != NULL; child=child->next) {
			if ((len = LengthAsnTree(child)) < 0)
				return -1;
            		p->length += len;
		}
        	if (PutTAG(p->dbuf, p->class_var, p->type, p->tagno) != Z_OK)
			return -1;
 		if (PutLEN(p->dbuf, p->length) != Z_OK)
			return -1;
        	p->length += p->dbuf->size;
    	}
    	return p->length;
}

int
LengthAsnTree2(p)
ASN_NODE *p;
{
    	ASN_NODE *child;
    	DBUF *inBuf;
	int len=0;

    	if (p == NULL)
		return 0;

	/* Note that for the NULL type, p->dbuf should be NULL.
	 * if (p->dbuf == NULL)
	 *	return -1;
	 */
    	if (p->type == 1) {		/* Constructed type	*/
		p->length = 0;
       		for (child=p->head; child != NULL; child=child->next) {
			if ((len = LengthAsnTree2(child)) < 0)
				return -1;
            		p->length += len;
		}
        	if (PutTAG(p->dbuf, p->class_var, p->type, p->tagno) != Z_OK) {
			fprintf(stderr, "Error put tag.\n");
			return -1;
		}
 		if (PutLEN(p->dbuf, p->length) != Z_OK) {
			fprintf(stderr, "Error put length.\n");
			return -1;
		}
        	p->length += p->dbuf->size;
    	} else {			/* Primitive type	*/
		if ((inBuf = NewDbuf()) == NULL)
			return -1;
        	if (PutTAG(inBuf, p->class_var, p->type, p->tagno) != Z_OK) {
			fprintf(stderr, "Error put tag.\n");
			return -1;
		}
		if (PutLEN(inBuf, p->length) != Z_OK) {
			fprintf(stderr, "Error put length.\n");
			return -1;
		}
        	if (db_put(inBuf, p->dbuf->data,p->dbuf->size) != p->dbuf->size){
			fprintf(stderr, "[%d %d %d] len=%d size=%d\n",
				p->class_var, p->type, p->tagno, p->length, 
				p->dbuf->size);
			fprintf(stderr, "Error put data.\n");
			return -1;
		}
        	p->length = inBuf->size;
		(void) FreeDbuf(p->dbuf);
        	p->dbuf = inBuf;
    	}
    	return p->length;
}

/*
 * Write the tree representation of a pdu into a buffer.
 */
int 
BerAsnTree(p, dbuf)
ASN_NODE *p;
DBUF *dbuf;
{
    	ASN_NODE *child;

    	if ((p == NULL) || (dbuf == NULL))
		return Z_ERROR;
    	if (db_put(dbuf, p->dbuf->data, p->dbuf->size) != p->dbuf->size)
		return Z_ERROR;
    	if (p->type == 1)	/* CONSTRUCTED	*/ 
       		for (child=p->head; child != NULL; child=child->next)
	    		if (BerAsnTree(child, dbuf) != Z_OK)
				return Z_ERROR;
    	return Z_OK;
}

DBUF * 
SerializeAsnTree(p)
ASN_NODE *p;
{
	DBUF *buf;

    	if (p == NULL)
		return (DBUF *)NULL;
	if ((buf = NewDbuf()) == NULL)
		return (DBUF *)NULL;
	if (BerAsnTree (p, buf) != Z_OK)
		return (DBUF *)NULL;
	return buf;
}

static char *class_var[] = {
	"UNIVERSAL",
	"APPLICATION",
	"CONTEXT",
	"PRIVATE"
};
#define INDENT 4	/* tree print indent */

void
PrintAsnTree(p, n)
ASN_NODE *p;
int n;	/* tree level */
{
    	char format[200];
    	ASN_NODE *child;

    	if (p == NULL)
		return;
    	sprintf(format, "%%%ds", n*INDENT);
    	printf(format, "");
	if ((p->class_var < 0) || (p->class_var > 3)) {
		printf("Invalid tag class_var: %d\n", p->class_var);
		return;
	}
    	printf("[%-12s %d %3d]  Len = %d\n", 
		class_var[p->class_var],
		p->type,
		p->tagno,
		p->length);
    	for (child=p->head; child != NULL; child=child->next)
        	PrintAsnTree(child, n+1);
}

