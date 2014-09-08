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
#include <string.h>
#include "zprimitive.h"

int
db_put(dbuf, data, size)
DBUF *dbuf;
UCHAR *data; 
int size;
{
    	if (dbuf == (DBUF *)NULL)
		return -1;
	if (size == 0)		/* it is true for the NULL type	*/
		return 0;
    	if ((data == (UCHAR *)NULL) || (size < 0))
		return -1;
    	if ((dbuf->max - dbuf->size) < size) {
		dbuf->max = (2 * dbuf->max + size);
		dbuf->data = (UCHAR *)realloc(dbuf->data, dbuf->max);
    	}
	if (dbuf->data == (UCHAR *)NULL)
		return -1;
    	memmove(dbuf->data+dbuf->size, data, size);
    	dbuf->size += size;
    	return size;
}

int
db_get(dbuf, data, size)
DBUF *dbuf; 
UCHAR *data; 
int size;
{
    	int amount = size;

    	if (dbuf == (DBUF *)NULL)
		return -1;
	if (size == 0)		/* it is true for the NULL type	*/
		return 0;	
    	if ((data == (UCHAR *)NULL) || (size < 0))
		return -1;
    	if ((dbuf->size - dbuf->cursor) < size) 
		amount = dbuf->size - dbuf->cursor;
    	memmove(data, dbuf->data+dbuf->cursor, amount);
    	dbuf->cursor += amount;
    	return amount;
}

/*
 * Create and initialize a new_var instance of DBUF structure. 
 * NewDbuf returns NULL if malloc fails, otherwise
 * returns a pointer to DBUF.
 */

DBUF *
NewDbuf() 
{
    	DBUF *dbuf;

    	dbuf = (DBUF *)malloc(sizeof(struct dbuf));
    	if (dbuf == NULL) 
		return (DBUF *)NULL;
    	dbuf->size = 0;
    	dbuf->max = 100;
    	dbuf->cursor = 0;
    	dbuf->data = (char *)malloc(dbuf->max);
    	if (!dbuf->data)
		return (DBUF *)NULL;
    	return dbuf;
}

void
FreeDbuf(dbuf)
DBUF	*dbuf;
{
    	if (dbuf == NULL)
		return;
    	if (dbuf->data)
		(void) free((char *)dbuf->data);
    	(void) free((char *)dbuf);
}


static char hex[] = 
	{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e', 'f'};
void
PrintDbuf(DBUF *dbuf)
{
    int i;
    unsigned char c;
    
    if (dbuf == NULL)
	return;
    for (i=0; i<dbuf->size; i++) {
        c = dbuf->data[i];
	printf("%c%c ", hex[(c&0xF0)>>4], hex[c&0x0F]);
	if (i && ((i % 18) == 0))
	    printf("\n");
    }
    printf("\n");
}

