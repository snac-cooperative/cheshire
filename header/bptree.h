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
 *	B+ Tree Header File.
 *	Aitao Chen
 */
#ifndef BPT_H
#define BPT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "alpha.h"
#include "db.h"
#include "mpool.h"

#define BPT_DUPLICATE	5
#define BPT_CONTINUE	4
#define BPT_FOUND	3
#define BPT_NOT_FOUND	2
#define BPT_OK	1
#define BPT_ERROR	-1
#define BPT_UNDEF	-2

#define MIN_NUM_KEYS	2		/* Minimum number of keys in one page */
#define PAGE_SIZE	4096

#define MAX_TREE_HEIGHT	50

#define	PAGE_LEAF	0x01
#define PAGE_INTERNAL	0x02
#define PAGE_ROOT	0x04
#define PAGE_METADATA	0x08

typedef int PAGENO;

typedef enum {LESS_THAN, EQUAL_OR_LESS_THAN, EQUAL,
	      EQUAL_OR_GREATER_THAN, GREATER_THAN} REL;

typedef struct item {
	int	key;		/* Where this key starts in this page */
	PAGENO	child;		/* Pointer to the data record or child page */
} ITEM;

#define ITEM_SIZE	sizeof(ITEM)

typedef struct pageinfo {
	PAGENO  self;			/* This page's page number.	*/
	PAGENO  prevpg;			/* Left sibling's page number.	*/
	PAGENO  nextpg;			/* Right sibling's page number.	*/
	int   	page_type;		/* Page type: LEAF or INTERNAL	*/
	int	lower;
	int   	higher;			/* The largest key starts.	*/
	PAGENO	child0;			/* The first child page number. */
	char    space[PAGE_SIZE - 4*sizeof(PAGENO) - 3*sizeof(int)];
					/* The free space.              */
} PAGEINFO;	

#define MAX_KEY_LENGTH  ((PAGE_SIZE - 4*sizeof(PAGENO) - 3*sizeof(int))/2)
typedef char MKEY[MAX_KEY_LENGTH];
 
#define B_PLUS_TREE     0x01234567


#define MAX_TREE_FILE	20	/* Maximum number of BTREE in one file */

/* There is one tree header record for every tree defined 
 * in a file. The tree header records the root page number
 * of the tree and the number of keys in that tree. More
 * fields can be easily added if needed.
 */
typedef struct treehdr {
	int tree;		/* The tree index.	*/
	int key_count;		/* How many keys in this tree. */
	PAGENO root;		/* Where is the root page of this tree */
	char tree_name[20];	/* The name of this tree. Note that the
				 * tree names must be UNIQUE within one file */
} TREEHDR;

typedef struct filehdr {
	int magic;			/* Magic number.	*/
	int page_size;			/* Number of bytes per page	*/
    	PAGENO next_page;
    	PAGENO free_page;
	int num_trees;			/* Number of BTREE in this file. */
	TREEHDR treehdrs[MAX_TREE_FILE];
} FILEHDR;


typedef struct cursor {
	PAGENO	pageno;		/* Page number.	*/
	int	slot;		/* Key or Child index	*/
	PAGENO  child;
} CURSOR;


typedef struct kd {
	int     flag;           /* BPT_FOUND or BPT_NOT_FOUND   */
	int    data;           /* Data associated with a key   */
	MKEY    key;            /* The key requested.           */
	} KD;

/* The tree path is organized as a stack. It is created by the
 * search routine. The two operations on tree path are push
 * and pop. The stack structure is implemented as a linked list.
 */
typedef struct tree_path {
    	int top;
	CURSOR path[MAX_TREE_HEIGHT];
} TREE_PATH;

typedef struct bpt_stat {
	int	io_reads;	/* Number of disk reads		*/
	int	io_writes;	/* Number of disk writes	*/
	int	parent_reads;	
	int	parent_writes;
} BPT_STAT;

BPT_STAT bpt_stat;

typedef struct bpt {
	int	fd;			/* file index.		*/
	FILE	*fptr;			/* File pointer		*/

	int	magic;			
	int	page_size;		/* Number of bytes per page 	*/
	PAGENO  next_page;
	PAGENO  free_page;
	int	num_trees;		/* Number of BTREE in this file */
	TREEHDR treehdrs[MAX_TREE_FILE];

	PAGENO	pageno;
	int	slot;
	PAGEINFO *page;

	/* Hash tree name to tree index.   */

	/* search path.	*/
	int	top;
	CURSOR	path[MAX_TREE_HEIGHT];

	/* BTREE buffer management fields. */
	MPOOL	*mpool;
} BPT, *BPTP;

#include "bpinter.h"
#endif	/* BPT_H	*/

