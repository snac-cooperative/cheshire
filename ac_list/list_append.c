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
#include "list.h"
#include "dmalloc.h"

/*
 * Append new data to the end of a list as a separate item.
 */
int
ac_list_append(list, data, size)
AC_LIST *list;
char *data;
int size;
{
    /* 
     * Allocate more data space if necessary.
     */
    if (list->data == NULL) {
	list->end = 2 * size;
	list->data = CALLOC(char, list->end);
    } else if ((list->end - list->size) <= size) {
	list->end = 2 * (size + list->end);
	list->data = REALLOC(list->data, char, list->end);
    }

    if (list->data == NULL) {
	fprintf(stderr, "malloc/realloc failed in ac_list_append.\n");
	return LIST_ERROR;
    }  
    /* 
     * Allocate more index entries if necessary.
     */
    if (list->idxsize >= list->idxend) {
	list->idxend = 2 * list->idxend;
	list->index = REALLOC(list->index, int, list->idxend);
    }	
    
    if (list->index == NULL) {
	fprintf(stderr, "malloc/realloc failed in ac_list_append.\n");
	return LIST_ERROR;
    } 

    /* 
     * record new data offset in the data buffer.
     */
    list->index[list->idxsize] = list->size;
    list->idxsize += 1;

    memmove(list->data+list->size, data, size);
    list->size += size;

    list->data[list->size] = '\0';
    list->size += 1;
    
    return LIST_OK;
}

/*
 * Add a null-terminated string to the end of list. This routine 
 * differs from ac_list_append only in the number of arguments it takes.
 */
int
ac_list_add(list, data)
AC_LIST *list;
char *data;
{
    return ac_list_append(list, data, strlen(data));
}

int
ac_list_addchar(list, ch)
AC_LIST *list;
char ch;
{
    char data[2];

    data[0] = ch;
    data[1] = '\0';
    return ac_list_append(list, data, strlen(data));
}

int
ac_list_addint(list, int_value)
AC_LIST *list;
int int_value;
{
    char int_str[100];

    sprintf(int_str, "%d", int_value);
    return ac_list_append(list, int_str, strlen(int_str));
}


int
ac_list_addlong(list, long_value)
AC_LIST *list;
int long_value;
{
    char long_str[100];

    sprintf(long_str, "%d", long_value);
    return ac_list_append(list, long_str, strlen(long_str));
}

int
ac_list_delete(list, data)
AC_LIST *list;
char *data;
{
    int	pos;
    int i;
    /* 
     * record new data offset in the data buffer.
     */

    if ((pos = ac_list_find(list, data)) < 0)
	return -1;

    for (i=pos; i<list->idxsize-1; i++)
	list->index[i] = list->index[i+1];
    list->index[list->idxsize - 1] = -1;
    list->idxsize -= 1;

    return LIST_OK;
}
