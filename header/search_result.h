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
#ifndef	SEARCH_RESULT_H
#define	SEARCH_RESULT_H

typedef struct searchPartResult {
	char	databaseName[MAX_NAME_LENGTH];
	int	startPosition;		/* starting position in result set */
	int	numberRecords;		/* number of records from this db. */
} SEARCH_PART_RESULT;

typedef struct searchResult {
	LIST	*resultSet;		/* current search result.	*/
	int	numberRecords;		/* total number of records.	*/
	int	numberDatabases;	/* number of databases.		*/	
	SEARCH_PART_RESULT	partResults[MAX_NUM_DBNAMES];
} SEARCH_RESULT;

typedef struct recordLists RECORD_LISTS;
	
struct recordLists {
    char	databaseName[MAX_NAME_LENGTH];
    int		numRecords;
    LIST	*records;
    RECORD_LISTS *next;
};

typedef struct search_result_object {
    int	searchStatus;
    int resultCount;	/* # of records retrieved */
    int noRecordsReturned;	/* # of records returned 	*/
    int nextResultSetPosition;
    char *resultSetName;	/* the result set name at server side.	*/
    NamePlusRecord **records;
    DiagRec *diagRec;
} SEARCH_RESULT_OBJECT;

#endif	/* SEARCH_RESULT_H	*/

