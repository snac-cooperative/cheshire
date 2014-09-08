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
/* 
 *	Record Manager Header File.
 *	Aitao Chen
 */
#ifndef REC_H
#define REC_H

#include "bptree.h"

#define REC_DUPLICATE	5
#define REC_CONTINUE	4
#define REC_FOUND	3
#define REC_NOT_FOUND	2
#define REC_OK	1
#define REC_ERROR	-1
#define REC_UNDEF	-2

#define REC_PAGE_SIZE	8*1024

/*
#define MIN_NUM_KEYS	3		
#define MAX_TREE_HEIGHT	100
#define	PAGE_LEAF	0x01
#define PAGE_INTERNAL	0x02
#define PAGE_ROOT	0x04
#define PAGE_METADATA	0x08
typedef char MKEY[255];
typedef int PAGENO;
typedef struct item {
	int	key;		
	PAGENO	child;	
} ITEM;
#define ITEM_SIZE	sizeof(ITEM)
*/


/* Internal page/node layout.  */
typedef struct ipageinfo {
	int   	page_type;		
	PAGENO  self;		
	int	lower;
	int   	higher;	
	PAGENO  prevpg;
	PAGENO  nextpg;
	PAGENO	child0;
	char	space[REC_PAGE_SIZE - 3*sizeof(int) - 4*sizeof(int)];
} IPAGEINFO;	


/* Leaf page/node layout.
 */
typedef struct leafpage {
	int   	page_type;		/* Page type: LEAF or INTERNAL	*/
	PAGENO  self;			/* This page's page number.	*/
	int	lower;
	int   	higher;			/* The largest key starts.	*/
	PAGENO  prevpg;			/* Left sibling's page number.	*/
	PAGENO  nextpg;			/* Right sibling's page number.	*/
	PAGENO	child0;			/* The first child page number. */
	char	space[REC_PAGE_SIZE - 3*sizeof(int) - 4*sizeof(int)];	
					/* Array of items followed by an
					 * array of keys.	*/
} LEAFPAGE;	

/* Note: The key in B+ tree must be null-terminated string. The conent
 * of the record can be any binary data. This record manager doesn't
 * know how to interpret the content of the records. 
 */
typedef struct rec_item {
	int	key;		/* Where this key starts in this page */
	int	data;		/* Where the data starts in this page */
	int	dsize;		/* data record size.	*/
} REC_ITEM;
#define REC_ITEM_SIZE	sizeof(REC_ITEM)

/*
typedef struct kd {
	int     flag;         
	int    data;       
	MKEY    key;
} KD;
*/
				 
typedef struct datum {
	char	*dptr;
	int	dsize;
} DATUM;

#ifndef MAX_TREE_FILE
#define MAX_TREE_FILE	20	/* Maximum number of BTREE in one file */
#endif

/* There is one tree header record for every tree defined 
 * in a file. The tree header records the root page number
 * of the tree and the number of keys in that tree. More
 * fields can be easily added if needed.
 */

/*
typedef struct treehdr {
	int tree;	
	int key_count;
	PAGENO root;
	char tree_name[10];
} TREEHDR;
typedef struct filehdr {
	int magic;			
	int page_size;			
    	PAGENO firstavail;
    	PAGENO firsthole;
	int num_trees;			
	TREEHDR treehdrs[MAX_TREE_FILE];
} FILEHDR;
typedef struct cursor {
	PAGENO	pageno;		
	int	slot;
	PAGENO  child;
} CURSOR;
*/

/* The tree path is organized as a stack. It is created by the
 * search routine. The two operations on tree path are push
 * and pop. The stack structure is implemented as a linked list.
 */
/*
typedef struct tree_path {
    	int top;
	CURSOR path[MAX_TREE_HEIGHT];
} TREE_PATH;
typedef struct bpt_stat {
	int	io_reads;	
	int	io_writes;
} BPT_STAT;
BPT_STAT bpt_stat;
typedef struct bpt {
	int	fd;	
	FILE	*fptr;
	int	magic;
	int	page_size;
	PAGENO  firstavail;
	PAGENO  firsthole;
	int	num_trees;
	TREEHDR treehdrs[MAX_TREE_FILE];
	int	top;
	CURSOR	path[MAX_TREE_HEIGHT];
	MPOOL	*mpool;
} BPT, *BPTP;
*/

#include "recinter.h"
#endif	/* REC_H	*/

