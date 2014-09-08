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
  
  morph.c - WordNet search code morphology functions
  
*/

#include "wnconsts.h"
#include <stdio.h>
#ifdef MACH
#include <ctype.h>
#endif
#ifdef WIN32
#include <ctype.h>
#endif
#ifdef MAC
#include "macfns.h"
#include <string.h>
#include <ctype.h>
#else
#include <string.h>
#include <stdlib.h>
#endif
#include "wn.h"

#ifdef MSDOS
#define EXCFILE	"%s\\%s"
#else
#ifdef MAC
#define EXCFILE	"%s:%s"
#else
#define EXCFILE	"%s/%s"
#endif
#endif

static char *Id = "$Id: morph.c,v 1.39 93/07/28 16:02:26 wn Exp $";

void do_init();

static char *sufx[] ={ 
    /* Noun suffixes */
    "s","ses","xes","zes","ches","shes",
    /* Verb suffixes */
    "s","ies","es","es","ed","ed","ing","ing",
    /* Adjective suffixes */
    "er","est","er", "est"
};

static char *addr[] ={ 
    /* Noun endings */
    "", "s", "x", "z", "ch", "sh", 
    /* Verb endings */
    "", "y", "e", "", "e", "", "e", "",
    /* Adjective endings */
    "", "", "e", "e"
};

static int offsets[NUMPARTS] = { 0, 0, 6, 14 };
static int cnts[NUMPARTS] = { 0, 6, 8, 4 };

#define NUMPREPS	15

static struct {
    char *str;
    int strlen;
} prepositions[NUMPREPS] = {
    "to", 2,
    "at", 2,
    "of", 2,
    "on", 2,
    "off", 3,
    "in", 2,
    "out", 3,
    "up", 2,
    "down", 4,
    "from", 4,
    "with", 4,
    "into", 4,
    "for", 3,
    "about", 5,
    "between", 7,
};

static char *exc_fnames[NUMPARTS + 1] =
{ NULL, "noun.exc", "verb.exc", "adj.exc", NULL };
static FILE *exc_fps[NUMPARTS + 1];

void morphinit()
{
    static int done = 0;

    if (done)
	return;
    do_init();
    done = 1;
}

void re_morphinit()
{
    int i;

    for (i = 1; i <= NUMPARTS; i++)
	fclose(exc_fps[i]);
    do_init();
}

static void do_init()
{
    int i;
    char *env;
    char searchdir[256], fname[256];

    strcpy(searchdir,((env = getenv("WNSEARCHDIR")) ? env : DEFAULTPATH));
    for (i = 1; i <= NUMPARTS; i++) {
	if (exc_fnames[i] != NULL) {
	    sprintf(fname, EXCFILE, searchdir, exc_fnames[i]);
	    if ((exc_fps[i] = fopen(fname, "r")) == NULL)
		fprintf(stderr, "Error in opening exception file(%s)\n\n",
			fname);
	}
    }
}

static int strend(str1,str2)
char *str1,*str2;
{
    char *pt1;
    
    if(strlen(str2) >= strlen(str1))
	return(0);
    else {
	pt1=str1;
	pt1=strchr(str1,0);
	pt1=pt1-strlen(str2);
	return(!strcmp(pt1,str2));
    }
}

static char *wordbase(word,ender)
char *word;
int ender;
{
    char *pt1;
    static char copy[WORDBUF];
    
    strcpy(copy, word);
    if(strend(copy,sufx[ender])) {
	pt1=strchr(copy,'\0');
	pt1 -= strlen(sufx[ender]);
	*pt1='\0';
	strcat(copy,addr[ender]);
    }
    return(copy);
}

static int hasprep(s, wdcnt)
char *s;
int wdcnt;
{
    /* Find a preposition in the verb string and return its
       corresponding word number. */

    int i, wdnum;

    for (wdnum = 2; wdnum <= wdcnt; wdnum++) {
	s = strchr(s, '_');
	for (s++, i = 0; i < NUMPREPS; i++)
	    if (!strncmp(s, prepositions[i].str, prepositions[i].strlen) &&
		(s[prepositions[i].strlen] == '_' ||
		 s[prepositions[i].strlen] == '\0'))
		return(wdnum);
    }
    return(0);
}
 
static char *exc_lookup(word, pos)
char *word;
int pos;
{
    static char line[WORDBUF], *beglp, *endlp;
    char *excline;
    int found = 0;

    if (exc_fps[pos] == NULL)
	return(NULL);

    /* first time through load line from exception file */
    if(word != NULL){
	if ((excline = bin_search(word, exc_fps[pos])) != NULL) {
	    strcpy(line, excline);
	    endlp = strchr(line,' ');
	} else
	    endlp = NULL;
    }
    if(endlp && *(endlp + 1)){
	beglp = endlp + 1;
	while(*beglp && *beglp == ' ') beglp++;
	endlp = beglp;
	while(*endlp && *endlp != ' ' && *endlp != '\n') endlp++;
	if(endlp != beglp){
	    *endlp='\0';
	    return(beglp);
	}
    }
    beglp = NULL;
    endlp = NULL;
    return(NULL);
}

static char *morphprep(s)
char *s;
{
    char *rest, *exc_word, *lastwd = NULL, *last;
    int i, offset, cnt;
    char word[WORDBUF], end[WORDBUF];
    static char retval[WORDBUF];

    /* Assume that the verb is the first word in the phrase.  Strip it
       off, check for validity, then try various morphs with the
       rest of the phrase tacked on, trying to find a match. */

    rest = strchr(s, '_');
    last = strrchr(s, '_');
    if (rest != last) {		/* more than 2 words */
	if (lastwd = morphword(last + 1, NOUN)) {
	    strncpy(end, rest, last - rest + 1);
	    end[last-rest+1] = '\0';
	    strcat(end, lastwd);
	}
    }
    
    strncpy(word, s, rest - s);
    word[rest - s] = '\0';
    for (i = 0, cnt = strlen(word); i < cnt; i++)
	if (!isalnum(word[i])) return(NULL);

    offset = offsets[VERB];
    cnt = cnts[VERB];

    /* First try to find the verb in the exception list */

    if ((exc_word = exc_lookup(word, VERB)) &&
	strcmp(exc_word, word)) {

	sprintf(retval, "%s%s", exc_word, rest);
	if(is_defined(retval, VERB))
	    return(retval);
	else if (lastwd) {
	    sprintf(retval, "%s%s", exc_word, end);
	    if(is_defined(retval, VERB))
		return(retval);
	}
    }
    
    for (i = 0; i < cnt; i++) {
	if ((exc_word = wordbase(word, (i + offset))) &&
	    strcmp(word, exc_word)) { /* ending is different */

	    sprintf(retval, "%s%s", exc_word, rest);
	    if(is_defined(retval, VERB))
		return(retval);
	    else if (lastwd) {
		sprintf(retval, "%s%s", exc_word, end);
		if(is_defined(retval, VERB))
		    return(retval);
	    }
	}
    }
    sprintf(retval, "%s%s", word, rest);
    if (strcmp(s, retval))
	return(retval);
    if (lastwd) {
	sprintf(retval, "%s%s", word, end);
	if (strcmp(s, retval))
	    return(retval);
    }
    return(NULL);
}

char *morphword(word,pos)
char *word;
int pos;
{
    int offset, cnt;
    int i;
    static char retval[WORDBUF];
    char *tmp;
    
    if (pos == ADV)
	return(NULL);
    
    sprintf(retval,"");
    
    if(word == NULL) 
	return(NULL);
    offset = offsets[pos];
    cnt = cnts[pos];
    
    /* first look for word on exception list */
    
    if((tmp = exc_lookup(word, pos)) != NULL)
	return(tmp);			/* found it in exception list */
    
    
    if (pos == NOUN) {
	/* check for noun ending with 'ss' or short words */
	if(strend(word, "ss") || (strlen(word) <= 2)) return(NULL);
    }
    
    for(i = 0; i < cnt; i++){
	strcpy(retval, wordbase(word, (i + offset)));
	if(strcmp(retval, word) && is_defined(retval, pos))
	    return(retval);
    }
    return(NULL);
}

char *morphstr(origstr, pos)
char *origstr;
int pos;
{
    static char searchstr[WORDBUF], str[WORDBUF];
    static int svcnt, svprep;
    char word[WORDBUF], *tmp;
    int cnt, st_idx = 0, end_idx;
    int prep;
    char *end_idx1, *end_idx2;
    char *append;
    
    if (pos == ADV)
	return(NULL);
    else if (pos == SATELLITE)
	pos = ADJ;

    /* First time through for this string */

    if (origstr != NULL) {
	/* Assume string hasn't had spaces substitued with '_' */
	strtolower(strsubst(strcpy(str, origstr), ' ', '_'));
	searchstr[0] = '\0';
	cnt = cntwords(str, '_');
	svprep = 0;

	/* first try exception list */

	if ((tmp = exc_lookup(str, pos)) && strcmp(tmp, str)) {
	    svcnt = 1;		/* force next time to pass NULL */
	    return(tmp);
	}

	/* Then try simply morph on original string */

	if (pos != VERB && (tmp = morphword(str, pos)) && strcmp(tmp, str))
	    return(tmp);

	if (pos == VERB && cnt > 1 && (prep = hasprep(str, cnt))) {
	    /* assume we have a verb followed by a preposition */
	    svprep = prep;
	    return(morphprep(str));
	} else {
	    svcnt = cnt = cntwords(str, '-');
	    while (origstr && --cnt) {
		end_idx1 = strchr(str + st_idx, '_');
		end_idx2 = strchr(str + st_idx, '-');
		if (end_idx1 && end_idx2) {
		    if (end_idx1 < end_idx2) {
			end_idx = (int)(end_idx1 - str);
			append = "_";
		    } else {
			end_idx = (int)(end_idx2 - str);
			append = "-";
		    }
		} else {
		    if (end_idx1) {
			end_idx = (int)(end_idx1 - str);
			append = "_";
		    } else {
			end_idx = (int)(end_idx2 - str);
			append = "-";
		    }
		}	
		if (end_idx < 0) return(NULL);		/* shouldn't do this */
		strncpy(word, str + st_idx, end_idx - st_idx);
		word[end_idx - st_idx] = '\0';
		if(tmp = morphword(word, pos))
		    strcat(searchstr,tmp);
		else
		    strcat(searchstr,word);
		strcat(searchstr, append);
		st_idx = end_idx + 1;
	    }
	    
	    if(tmp = morphword(strcpy(word, str + st_idx), pos)) 
		strcat(searchstr,tmp);
	    else
		strcat(searchstr,word);
	    if(strcmp(searchstr, str) && is_defined(searchstr,pos))
		return(searchstr);
	    else
		return(NULL);
	}
    } else {
	if (svprep) {		/* if verb has preposition, no more morphs */
	    svprep = 0;
	    return(NULL);
	} else if (svcnt == 1)
	    return(exc_lookup(NULL, pos));
	else {
	    svcnt = 1;
	    if ((tmp = exc_lookup(str, pos)) && strcmp(tmp, str))
		return(tmp);
	    else
		return(NULL);
	}
    }
}

/* 
 * Revision 1.6  92/02/21  16:29:00  wn
 * moved cntwords to util.c
 * 
 * Revision 1.5  92/02/12  10:35:15  wn
 * added static to internal funtions
 * 
 * Revision 1.4  92/02/05  11:08:25  wn
 * added morphstr() and cntwords()
 * changed morphit() to morphword() and changed 2nd arg to int
 * 
 * Revision 1.3  91/12/13  15:21:08  wn
 * changed 2nd arg to morphit to be single char instead of string
 * 
 * Revision 1.2  91/12/04  10:49:08  wn
 * don't check searchcode to see if word is already known and then
 * skip morph.  always morph word and return word, even if the same
 * 
 * Revision 1.1  91/09/25  15:39:47  wn
 * Initial revision
 * 
 */

