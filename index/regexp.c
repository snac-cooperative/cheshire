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
 *  regexp.c -- regular expression handling code.
 *
 */
#ifndef WIN32
#include <stdlib.h>
#include <stdio.h>

/*
 *  macros to support the regexp(3) library calls
 */
#define INIT		register char *p = instring;
#define GETC()		(*p++)
#define PEEKC()		(*p)
#define	UNGETC(c)	(*--p = (c))
#define	RETURN(v)	return(v)
#define	ERROR(val)	fprintf(stderr, "regexp library reports error %d", (val));

#define	EXPBUFSZ	500
#define	PCHARLEN	16

#ifndef MACOSX
#include <regexp.h>
#else
#include <regex.h>
#endif

#include <dmalloc.h>

#define FALSE 0
#define TRUE  1
#define bool  int

/*
 *  routines that use the regexp stuff
 */
bool
regexp_eq(matchstring, pattern)
	char *matchstring;
	char *pattern;
{
	char *expbuf, *endbuf;
	int result;

	if (!matchstring || !pattern)
		return FALSE;

	expbuf = (char *)MALLOC(EXPBUFSZ);
	endbuf = expbuf + (EXPBUFSZ - 1);

	/* compile the regular expression */
	(void) compile(pattern, expbuf, endbuf, 0);

	/* do the regexp matching */
	result = step(matchstring, expbuf);

	FREE(expbuf);

	return ((bool) result);
}

bool
regexp_ne(matchstring, pattern)
	char *matchstring;
	char *pattern;
{
	return (!regexp_eq(matchstring, pattern));
}

#endif

