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
#ifndef	AC_LISTINTER_H
#define	AC_LISTINTER_H

int ac_list_append(AC_LIST *list, char *data, int size);
int ac_list_add(AC_LIST *list, char *data);
int ac_list_addchar(AC_LIST *list, char ch);
int ac_list_addint(AC_LIST *list, int intval);
int ac_list_addlong(AC_LIST *list, int longval);
void ac_list_free(AC_LIST *list);
int ac_list_init(AC_LIST *list);
AC_LIST *ac_list_alloc();
int ac_list_load(char *file, AC_LIST *list);
int ac_list_save(char *file, AC_LIST *list);
void ac_list_show(AC_LIST *);
int ac_list_size(AC_LIST *list);
int ac_list_search(AC_LIST *list, char *tag);
char *ac_list_first(AC_LIST *list);
char *ac_list_next(AC_LIST *list);
char *ac_list_nth (AC_LIST *, int);
AC_LIST *ac_list_dup(AC_LIST *list);
AC_LIST *ac_list_sub_list(AC_LIST *list, int start, int number);
AC_LIST *ac_list_add_list(AC_LIST *old, AC_LIST *new_var);
int ac_list_delete(AC_LIST *, char *);
int ac_list_find (AC_LIST *, char *);
#endif 	/* AC_LISTINTER_H	*/

