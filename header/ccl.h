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
#ifndef	CCL_H
#define	CCL_H

#include "stree.h"

#include "z3950_3.h"

typedef enum commands {
	CCL_COMM_STOP,
	CCL_COMM_SORT,
	CCL_COMM_HELP,
	CCL_COMM_FIND,
	CCL_COMM_EXPLAIN,
	CCL_COMM_DEFINE,
	CCL_COMM_SAVE,
	CCL_COMM_SCAN,
	CCL_COMM_SHOW,
	CCL_COMM_REVIEW,
	CCL_COMM_PRESENT,
	CCL_COMM_DELETE,
	CCL_COMM_BACK,
	CCL_COMM_PRINT,
	CCL_COMM_MORE,
	CCL_COMM_CHOOSE,
	CCL_COMM_RELATE,
	CCL_COMM_CONNECT
} CCL_COMMS;

typedef struct connectRequest {
	char	*hostName;
	int	portNumber;
} ConnectRequest;

typedef struct ccl {
 	int	setno;			/* Next set numbe	*/
	CCL_COMMS	command;
	union {
		Query	*find;
		PresentRequest	*presentRequest;
		ConnectRequest	*connectRequest;
		DeleteResultSetRequest *deleteResultSetRequest;
	} u;
} CCL;

#define MAX_TOKEN_LENGTH        200
typedef enum {AND_OP, OR_OP, NOT_OP, LEAF} NTYPE;
typedef enum {AND_TOKEN, OR_TOKEN, NOT_TOKEN, WORD_TOKEN, EOS_TOKEN} TTYPE;
typedef struct binary {
    NTYPE type;
    char *word;
    char *attributes;
    struct binary *left;
    struct binary *right;
} BINARY;

/* Indexes mapping.
 */
typedef enum {
	A_USE = 1,
	A_RELATION = 2,
	A_POSITION = 3,
	A_STRUCTURE = 4,
	A_TRUNCATION = 5,
	A_COMPLETENESS = 6
} ATYPE;

#define	V_IGNORE	-1

typedef struct index_mapping {
	char	indexName[20];
	int	useValue;
	int	relationValue;
	int	positionValue;
	int	structureValue;
	int	truncationValue;
	int	completenessValue;
} INDEX_MAPPING;

#include "cclinter.h"
#endif	/*	CCL_H	*/

