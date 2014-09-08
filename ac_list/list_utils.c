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

#include "list.h"

/*
 * This routine returns the number of items in a list.
 * in a list.
 */
int
ac_list_size(list)
AC_LIST *list;
{
    if (list == NULL)
      return 0;

    return list->idxsize;
}

/*
 * Search for an item in the list, if found, return 1, otherwise
 * return 0.
 */
int 
ac_list_search(list, item)
AC_LIST *list;
char *item;
{
    int item_offset;
    int i;

    if (list == NULL)
	return 0;

    for (i=0; i<list->idxsize; i++) {
        item_offset = list->index[i];
        if (strcmp(item, list->data + item_offset) == 0)
	    return 1;
    }
    return 0;
}

/*
 * ac_list_find returns -1 if not found, otherwise returns
 * the position of the item found in the list
 */
int 
ac_list_find(list, item)
AC_LIST *list;
char *item;
{
    int item_offset;
    int i;

    if (list == NULL)
	return -1;

    for (i=0; i<list->idxsize; i++) {
        item_offset = list->index[i];
        if (strcmp(item, list->data + item_offset) == 0)
	    return i;
    }
    return -1;
}


/* Return the next item in the list. Return NULL at the end of the list.
 */
char *
ac_list_next(list)
AC_LIST *list;
{
    int offset;

    if (list == NULL)
      return (char *)NULL;

    list->curpos++;
    if (list->curpos >= list->idxsize)
	return (char *)NULL;
    offset = list->index[list->curpos];

    return ((char *)(list->data + offset));
}

/*
 * Return the first item.
 */
char *
ac_list_first(list)
AC_LIST *list;
{
    int offset;
 
    if (list == NULL)
      return (char *)NULL;
    
    if (list->idxsize == 0)
      return (char *)NULL;
    
    list->curpos = 0;
    offset = list->index[list->curpos];
    
    return ((char *)(list->data + offset));
}

/*
 *	Return the nth item on the list.
 */
char *
ac_list_nth(list, which)
AC_LIST *list;
int  which;
{
    int offset;

    if (list == NULL)
      return (char *)NULL;

    if ((which < 0) || (which > list->idxsize))
	return (char *)NULL;
    list->curpos = which;
    offset = list->index[list->curpos];

    return ((char *)(list->data + offset));
}

AC_LIST	* 
ac_list_dup(slist)
AC_LIST *slist;		/* source list	*/
{
    char	*item;
    AC_LIST	*dlist;

    if (slist == NULL)
      return (AC_LIST *)NULL;

    if (slist->idxsize == 0)
	return (AC_LIST *)NULL;

    dlist = ac_list_alloc();
    for (item=ac_list_first(slist); item!=NULL; item=ac_list_next(slist))
	ac_list_add(dlist, item);
    return dlist;
}

AC_LIST *
ac_list_sub_list (slist, start, number)
AC_LIST *slist;
int start;
int number;
{
    char	*item;
    AC_LIST	*dlist;
    int i;

    if (slist == NULL)
      return (AC_LIST *)NULL;

    if (ac_list_size(slist) == 0)
	return (AC_LIST *)NULL;
    dlist = ac_list_alloc();
    for (i=0,item = ac_list_nth(slist,start); i<number; i++,item=ac_list_next(slist))
	ac_list_add(dlist, item);
    return dlist;
}

AC_LIST *
ac_list_add_list (olist, nlist)
AC_LIST	*olist;
AC_LIST	*nlist;
{
    char        *item;
 
    if (olist == NULL || nlist == NULL)
      return (AC_LIST *)NULL;


    for (item=ac_list_first(nlist); item!=NULL; item=ac_list_next(nlist))
        ac_list_add(olist, item);
    return olist;
}

void
ac_list_show(list)
AC_LIST *list;
{
    int i = 0;
    int data_offset = 0;

    if (list == NULL)
      return;

    for (i=0; i<list->idxsize; i++) {
        data_offset = list->index[i];
        printf("%s\n", list->data + data_offset);
    }
}
