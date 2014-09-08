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

  binsearch.c - general binary search functions

*/

#include "wnconsts.h"
#include "wnglobals.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Binary search - looks for the word passed at the start of a line
   in the file associated with open file descriptor fp, and returns
   a buffer containing the line in the file. */

#define KEY_LEN		1024
#define LINE_LEN	1024*8
#define BIG_LINE_LEN	1024*25

char *bin_search(word, fp)
char *word;
FILE *fp;
{
    register int c;
    register long top, mid, bot, diff;
    char *linep, key[KEY_LEN];
    static char line[LINE_LEN];
    int length;

    diff=666;
    linep = line;
    line[0] = '\0';

    fseek(fp, 0L, 2);
    top = 0;
    bot = ftell(fp);
    mid = (bot - top) / 2;

    do {
	fseek(fp, mid - 1, 0);
	if(mid != 1)
	    while((c = getc(fp)) != '\n' && c != EOF);
	fgets(linep, LINE_LEN, fp);
	length = (int)(strchr(linep, ' ') - linep);
	strncpy(key, linep, length);
	key[length] = '\0';
	if(strcmp(key, word) < 0) {
	    top = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
	if(strcmp(key, word) > 0) {
	    bot = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
    } while((strcmp(key, word)) && (diff != 0));
    
    if(!strcmp(key, word))
	return(line);
    else
	return(NULL);
}

static long offset;

static int bin_search_key(word, fp)
char *word;
FILE *fp;
{
    register int c;
    register long top, mid, bot, diff;
    char *linep, key[KEY_LEN];
    static char line[BIG_LINE_LEN];
    int length, offset1, offset2;

    /* do binary search to find correct place in file to insert line */

    diff=666;
    linep = line;
    line[0] = '\0';

    fseek(fp, 0L, 2);
    top = 0;
    bot = ftell(fp);
    if (bot == 0) {
	offset = 0;
	return(0);		/* empty file */
    }
    mid = (bot - top) / 2;

    /* If only one line in file, don't work through loop */

    length = 0;
    rewind(fp);
    while((c = getc(fp)) != '\n' && c != EOF)
	line[length++] =  c;
    if (getc(fp) == EOF) {	/* only 1 line in file */
	length = (int)(strchr(linep, ' ') - linep);
	strncpy(key, linep, length);
	key[length] = '\0';
	if(strcmp(key, word) > 0) {
	    offset = 0;
	    return(0);		/* line with key is not found */
	} else if (strcmp(key,word) < 0) {
	    offset = ftell(fp);
	    return(0);		/* line with key is not found */
	} else {
	    offset = 0;
	    return(1);		/* line with key is found */
	}
    }

    do {
	fseek(fp, mid - 1, 0);
	if(mid != 1)
	    while((c = getc(fp)) != '\n' && c != EOF);
	offset1 = ftell(fp);	/* offset at start of line */
	if (fgets(linep, BIG_LINE_LEN, fp) != NULL) {
  	    offset2 = ftell(fp); /* offset at start of next line */
	    length = (int)(strchr(linep, ' ') - linep);
	    strncpy(key, linep, length);
	    key[length] = '\0';
	    if(strcmp(key, word) < 0) {	/* further in file */
		top = mid;
		diff = (bot - top) / 2;
		mid = top + diff;
		offset = offset2;
	    }
	    if(strcmp(key, word) > 0) {	/* earlier in file */
		bot = mid;
		diff = (bot - top) / 2;
		mid = top + diff;
		offset = offset1;
	    }
	} else {
	    bot = mid;
	    diff = (bot - top) / 2;
	    mid = top + diff;
	}
    } while((strcmp(key, word)) && (diff != 0));

    if(!strcmp(key, word)) {
	offset = offset1;	/* get to start of current line */
	return(1);		/* line with key is found */
    } else
	return(0);		/* line with key is not found */
}

void copyfile(fromfp, tofp)
FILE *fromfp, *tofp;
{
    register int c;

    while ((c = getc(fromfp)) != EOF)
	putc(c, tofp);
}

/* Function to replace a line in a file.  Returns the original line,
   or NULL in case of error. */

char *replace_line(newline, word, fp)
char *newline, *word;
FILE *fp;
{
    FILE *tfp;			/* temporary file pointer */
    static char line[BIG_LINE_LEN];

    if (!bin_search_key(word, fp))
	return(NULL);		/* line with key not found */

    if ((tfp = tmpfile()) == NULL)
	return(NULL);		/* could not create temp file */
    fseek(fp, offset, 0);
    fgets(line, BIG_LINE_LEN, fp);	/* read original */
    copyfile(fp, tfp);
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    fprintf(fp, newline);	/* write line */
    rewind(tfp);
    copyfile(tfp, fp);

    fclose(tfp);
    fflush(fp);

    return(line);
}

char *insert_line(newline, word, fp)
char *newline, *word;
FILE *fp;
{
    FILE *tfp;

    /* Find location to insert line at in file.  If line with this
       key is already in file, return NULL. */

    if (bin_search_key(word, fp))
	return(NULL);
    
    if ((tfp = tmpfile()) == NULL)
	return(NULL);		/* could not create temp file */
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    copyfile(fp, tfp);
    if (fseek(fp, offset, 0) == -1)
	return(NULL);		/* could not seek to offset */
    fprintf(fp, newline);	/* write line */
    rewind(tfp);
    copyfile(tfp, fp);

    fclose(tfp);
    fflush(fp);

    return(newline);
}

