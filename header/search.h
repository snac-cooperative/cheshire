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

#ifndef SEARCH_H
#define	SEARCH_H

#include <stdio.h>
#include "bptree.h"

#define	SE_OK		1
#define	SE_ERROR	-1

#define	SEARCH_OK	1
#define	SEARCH_FOUND	2
#define	SEARCH_NOTFOUND	3
#define	SEARCH_ERROR	-1

#define	MAX_FILENAME_LENGTH	120

/*
 *	search file system.
 */
typedef enum {FD_INT, FD_FILE, FD_BPT, FD_REC} FD_TYPE;
typedef struct searchFileSystem {
	char	fileName[MAX_FILENAME_LENGTH];
	FD_TYPE	fileType;
	union {
		int	fd;
		FILE	*fp;
		BPT	*bpt;
	} u;
	struct searchFileSystem	*next;
} SearchFileSystem;

typedef	struct searchEngine {
	SearchFileSystem	*fileSystem;
	/*
	 * A list of search engine procedures.
	 * To be implemented.
	 */
} SearchEngine;

#include "searchinter.h"
#endif	/*	SEARCH_H	*/


