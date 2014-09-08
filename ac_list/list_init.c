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
 * Initialize a list variable. 
 */
#include "list.h"
#include "dmalloc.h"

int
ac_list_init(list)
AC_LIST *list;
{
    list->size = 0;
    list->end = 0;
    list->data = (char *)NULL;

    list->idxsize = 0;
    list->idxend = 1000;
    list->index = CALLOC(int, list->idxend);

    list->curpos = 0;
    return LIST_OK;
}

AC_LIST *
ac_list_alloc()
{
    AC_LIST *list;

    list = CALLOC(AC_LIST,1);
    if (list == (AC_LIST *)NULL)
	return (AC_LIST *)NULL;

    ac_list_init(list);
    return list;
}

void
ac_list_free(list)
AC_LIST *list;
{
    if (list == (AC_LIST *)NULL)
	return;

    if (list->index)
	FREE ((char *)list->index);

    if (list->data)
	FREE ((char *)list->data);

    FREE ((char *)list);
}

int
ac_list_load(file, list)
char *file;
AC_LIST *list;
{
    FILE *file_ptr;
    char item[200];
    int line_number;

    if ((file_ptr = fopen(file, "r")) == NULL) {
	printf("can't open file %s\n", file);
	return LIST_ERROR;
    }
    
    line_number = 0;
    while (!feof(file_ptr)) {
	if (1 != fscanf(file_ptr, "%s\n", item)) {
	    printf("can't read line %d in file %s\n", line_number, file);
	    return LIST_ERROR;
	}
	line_number++;

	/* 
	 * add the new item to the list.
	 */
    	if (ac_list_add(list, item) != LIST_OK) {
	    printf("can't add item %s\n", item);
	    return LIST_ERROR;
	}
    }
    fclose(file_ptr);
    return LIST_OK;
}

/*
 * Save the content in the list to a file.
 */
int
ac_list_save(file, list)
AC_LIST *list;
char *file;
{
    FILE *file_ptr;
    char *item;
    int line_number;

    if ((file_ptr = fopen(file, "w+")) == NULL) {
	printf("can't open file %s\n", file);
	return LIST_ERROR;
    }
    
    line_number = 0;
    for (item=ac_list_first(list); item!=NULL; item=ac_list_next(list)) {
	fprintf(file_ptr, "%s\n", item);
	line_number++;
    }
    fclose(file_ptr);
    /*
     * consistency check: the number of items written to the file
     * must be the same as the number of items in the list.
     */
    if (line_number != list->idxsize) {
	printf("can't write list to a file.\n");
	return LIST_ERROR;
    }
    return line_number;
}
