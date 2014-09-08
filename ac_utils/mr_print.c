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
#include <string.h>
#include <ctype.h>

static char one_field[2000];

void mr_subsfr(char *marcbuf, int marclen);

/*
 * Print field 245, subfied a.
 */
int
PrintMarcTitle(fptr, marc)
FILE	*fptr;
char    *marc;
{
    	char    *dirptr;
    	char    *dataptr;
    	int     recbase;
    	char    tag[4];
    	char    ind[3];
    	int     len, i;
    	int	    offset = 0;

	if ((!fptr) || (!marc))
		return 0;

    	sscanf(marc, "%*12c%5d", &recbase);
    	dirptr  = marc + 24;
    	dataptr = marc + recbase;
    	while (*dirptr != '\036') {
		memset(tag, '\0', sizeof(tag));
		memset(ind, '\0', sizeof(ind));
		sscanf(dirptr,"%3c%4d%5d",  tag, &len, &offset);
        	if (strcmp(tag, "245") == 0) {
	    		strncpy(one_field, dataptr+offset+4, len);
	    		one_field[len] = '\0';
			dataptr = one_field;	/* skip ind + sub a */
			while (*dataptr && *dataptr != '\037')
			   	dataptr++;
			*dataptr = '\0';
		
			for (i=0; i<strlen(one_field); i++) {
				if (((i % 65) == 0) && (i != 0)) 
					fprintf(fptr,"\n%10c", ' ');
				fprintf(fptr,"%c", one_field[i]);
			}
			fprintf(fptr,"\n");
		}
		dirptr += 12;
    	}
    	return 1;
}


int
PrintMarcLong(fptr, marc)
FILE	*fptr;
char    *marc;
{
    	char    *dirptr;
    	char    *dataptr;
    	int     recbase;
    	char    tag[4];
    	char    ind[3];
    	char    leader[25];
    	int     len, i;
    	int	    offset = 0;

	if ((!fptr) || (!marc))
		return 0;

    	strncpy(leader, marc, 24);
    	leader[24] = '\0';
    	fprintf(fptr,"%s\n", leader);

    	sscanf(marc, "%*12c%5d", &recbase);
    	dirptr  = marc + 24;
    	dataptr = marc + recbase;
    	while (*dirptr != '\036') {
		memset(tag, '\0', sizeof(tag));
		memset(ind, '\0', sizeof(ind));
		sscanf(dirptr,"%3c%4d%5d",  tag, &len, &offset);
        	if (strcmp(tag, "008") > 0) {
	    		strncpy(ind, dataptr+offset, 2);
      	    		strncpy(one_field, dataptr+offset+2, len-2);
	    		one_field[len-2] = '\0';
		} else {
	    		strncpy(one_field, dataptr+offset, len);
	    		one_field[len] = '\0';
		}
		mr_subsfr(one_field, strlen(one_field));
		fprintf(fptr,"%-5s%-5s", tag, ind);
		for (i=0; i<strlen(one_field); i++) {
			if (((i % 65) == 0) && (i != 0)) 
				fprintf(fptr,"\n%10c", ' ');
			fprintf(fptr,"%c", one_field[i]);
		}
		fprintf(fptr,"\n");
		dirptr += 12;
    	}
    	return 1;
}

int
PrintMarc(fptr, marc)
FILE	*fptr;
char    *marc;
{
	if ((!fptr) || (!marc))
		return 0;
	fprintf(fptr, "%s\n", marc);
    	return 1;
}
/* 
 * replace SF with '$', FT with '\0', and RT with '\0'
 */

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
