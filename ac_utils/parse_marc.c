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
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int mr_tobuf (char *, AC_LIST *);
void mr_subsfr (char *, int);

AC_LIST *
parseMarc(marc)
char *marc;
{
    AC_LIST *result = ac_list_alloc();
    if (marc == NULL)
	return NULL;
    mr_tobuf(marc, result);
    return result;
}

static  char    field_data[1000];
static  char	tbuf[2000];
int
mr_tobuf(marc_rec, buffer)
char    *marc_rec;
AC_LIST *buffer;
{
    char    *dirptr;
    char    *dataptr;
    int     base_addr;
    char    field_tag[4];
    int     field_length;
    int     field_offset;

    /* determine where data starts in the marc record.
     */
    sscanf(marc_rec, "%*12c%5d", &base_addr);
    dirptr  = marc_rec + 24;
    dataptr = marc_rec + base_addr;

    /* copy marc record header into buffer.
     * set the field tag for marc record header to "000".
     */
    ac_list_append(buffer, marc_rec, 24);

    /* loop through all directory entries, determine the length
     * and the offset of each field, then copy the field data in
     * to a static buffer.
     */
    while (*dirptr != '\036') {
	memset(field_tag, '\0', sizeof(field_tag));
	sscanf(dirptr,"%3c%4d%5d",  field_tag, &field_length, &field_offset);

	memset(field_data, '\0', sizeof(field_data));
      	strncpy(field_data, dataptr+field_offset, field_length);
	field_data[field_length] = '\0';

	/* replace SFT with $, FT and RT with NULL character.  */
	mr_subsfr(field_data, field_length);

	/* copy current field into buffer.
	 */
	/*
	ac_list_append2(buffer, field_tag, TAGLEN, field_data, field_length);
	*/
	sprintf(tbuf, "%s   %s", field_tag, field_data);
	ac_list_add(buffer, tbuf);
	dirptr += 12;
    }
    return 1;
}

/* 
 * replace SF with '$', FT with '\0', and RT with '\0'
 */
/*
void
mr_subsfr(marcbuf, marclen)
char	*marcbuf;
int	marclen; 
{
    int	i;

    for (i=0; i<marclen; i++) {
	switch(marcbuf[i]) {
	  case '\037' : marcbuf[i] = '$';
	    break;
	  case '\036' : marcbuf[i] = '\0';
	    break;
	  case '\035' : marcbuf[i] = '\0';
	    break;
	  default : 
	    break;
	}
    }
}	
*/
