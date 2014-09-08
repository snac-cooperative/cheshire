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

  util.c - utility functions used by WordNet code

*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
/* #include <malloc.h> --- This causes problems on DEC Alpha __malloc */
#include "wn.h"
#include "wnconsts.h"
#include "wnglobals.h"


/* Initialization functions */

void wninit()
{
    int i;
    static int done = 0;
    void do_init(), readfnames();


    if (done)
	return;
    do_init();
    for (i = 1; i < NUMPARTS + 1; i++)
	readfnames(i);
    morphinit();
    done = 1;
}

void re_wninit()
{
    int i;
    void readfnames();

    for (i = 1; i < NUMPARTS + 1; i++) {
	fclose(datafps[i]);
	fclose(indexfps[i]);
    }
    fclose(sensefp);
    do_init();
    for (i = 1; i < NUMPARTS + 1; i++)
	readfnames(i);

    re_morphinit();
}

static void do_init()
{
    int i;
    char searchdir[256], tmpbuf[256];
    char *env;
 
    strcpy(searchdir,((env = getenv("WNSEARCHDIR")) ? env : DEFAULTPATH));

    for (i = 1; i < NUMPARTS + 1; i++) {
	sprintf(tmpbuf, DATAFILE, searchdir, partnames[i]);
	if((datafps[i] = fopen(tmpbuf, "r")) == NULL)
	    fprintf(stderr, "Error: can't open datafile(%s)\n",tmpbuf);
	sprintf(tmpbuf, INDEXFILE, searchdir, partnames[i]);
	if((indexfps[i] = fopen(tmpbuf, "r")) == NULL)
	    fprintf(stderr, "Error: can't open indexfile(%s)\n",tmpbuf);
    }
    sprintf(tmpbuf, SENSEIDXFILE, searchdir);
    if((sensefp = fopen(tmpbuf, "r")) == NULL)
	fprintf(stderr, "Error: can't open sense index(%s)\n",tmpbuf);
}

static void readfnames(dbase)
int dbase;
{
    FILE *fp;
    register int i = 0, len;
    char line[SMLINEBUF];
    char *tmpptrs[100];		/* place to hold ptrs to fnames */
    char **p;

    fp = datafps[dbase];
    while (1) {			/* skip license and copyright notice */
	fgets(line, SMLINEBUF, fp);
	if (line[0] != ' ')
	    break;
    }
    
    while (line[0] != '0') {
	len = 0;
	while (line[len++] != ' ');
	tmpptrs[i] = malloc(len);
	assert(tmpptrs[i]);
	strncpy(tmpptrs[i], line, len - 1);
	tmpptrs[i][len - 1] = '\0';	/* append null */
	i++;
	fgets(line, SMLINEBUF, fp);
    }
    
    p = (char **)malloc(i * sizeof(char *));
    assert(p);
    wn_filenames[dbase] = p;
    assert(wn_filenames[dbase]);
    
    while (i--)
	p[i] = tmpptrs[i];
}

/* Count the number of underscore or space separated words in a string. */

int cntwords(s, separator)
char *s, separator;
{
    register int wdcnt = 0;

    while (*s) {
	if (*s == separator || *s == ' ' || *s == '_') {
	    wdcnt++;
	    while (*s && (*s == separator || *s == ' ' || *s == '_'))
		s++;
	} else
	    s++;
    }
    return(++wdcnt);
}


/* Downcase a word and deadjify at the same time */

char *strtolower(word)
char *word;
{
    register char *y;

    y=word;
    while(*y){
	if(isupper(*y))
	    *y=tolower(*y);
	if(*y == '(')
	    *y='\0';
	else
	    y++;
    }
    return(word);
}

char *ToLowerCase(str)
char *str;
{
    register char *s = str;

    while(*s!='\0') {
	if(*s>='A'&&*s<='Z')
	    *s += 32;
	s++;
    }
    return(str);
}

/* Replace all occurences of 'from' with 'to' in 'str' */

char *strsubst(str, from, to)
char *str;
char from;
char to;
{
    register char *p;

    for (p = str; *p != 0; ++p)
	if (*p == from)
	    *p = to;
    return str;
}

int getptrtype(ptrstr)
char *ptrstr;
{
    register int i;

    for(i = 1; i <= MAXSEARCH; i++)
	if(!strcmp(ptrstr, ptrtyp[i]))
	    return(i);
}

int getpos(s)
char *s;
{
    switch (*s) {
    case 'n':
	return(NOUN);
    case 'a':
    case 's':
	return(ADJ);
    case 'v':
	return(VERB);
    case 'r':
	return(ADV);
    default:
	fprintf(stderr, "Error: unknown part of speech %s\n", s);
	exit(-1);
    }
}

int getsstype(s)
char *s;
{
    switch (*s) {
    case 'n':
	return(NOUN);
    case 'a':
	return(ADJ);
    case 'v':
	return(VERB);
    case 's':
	return(SATELLITE);
    case 'r':
	return(ADV);
    default:
	fprintf(stderr, "Error: unknown synset type %s\n", s);
	exit(-1);
    }
}

#define MINSENSELEN	7

char *StrToSense(str)
char *str;
{
    static char sense[200];
    char word[100], posname[5], file[50], headword[100];
    int sensenum, headsense, pos, filenum, i = 0;

    /* Pass in string of the form: 
             word%pos.filename%sensenum 
	     or
             word%pos.filename%sensenum%headword%headsensenum
       Returns string of the form:
             word%sense
	     or
	     word%sense%headword%headsense
       where sense and headsense are 5 bytes integers of the form:
             Byte 1:	POS
	     Bytes 2-3:	File number
	     Bytes 4-5:	Sense number
    */

    sscanf(str, "%[^%%]%%%[^.].%[^%%]%%%d", word, posname, file, &sensenum);
    ToLowerCase(word);
    ToLowerCase(posname);
    ToLowerCase(file);
    if(!strcmp(file,"tops")) file[0]='T';
    pos = StrToPos(posname);
    filenum = LexFileNum(posname, file);

    while (i < 3) {		/* determine if satellite */
	if (*str == '%')
	    i++;
	else if (*str == '\0')
	    break;
	str++;
    }
    if (i == 3) {
	sscanf(str, "%[^%%]%%%d", headword, &headsense);
	ToLowerCase(headword);
	pos = SATELLITE;
    }
    if (pos != SATELLITE)
	sprintf(sense, "%s%%%-1.1d%-2.2d%-2.2d",
		word, pos, filenum, sensenum);
    else
	sprintf(sense, "%s%%%-1.1d%-2.2d%-2.2d%%%s%%%-2.2d",
		word, pos, filenum, sensenum, headword, headsense);
    return(sense);
}

char *SenseToStr(sense)
char *sense;
{
    static char str[200];
    char word[100], posname[5], headword[100], *file;
    int pos, filenum, sensenum, headsense;

    /* Opposite of StrToSense described above. */

    sscanf(sense, "%[^%%]%%%1d%2d%2d", word, &pos, &filenum, &sensenum);
    file = LexFileName(pos, filenum);
    if (pos == SATELLITE) {
	sense += (strlen(word) + MINSENSELEN);
	sscanf(sense, "%[^%%]%%%2d", headword, &headsense);
	sprintf(str, "%s%%%s%%%d%%%s%%%d",
		word, file, sensenum, headword, headsense);
    } else
	sprintf(str, "%s%%%s%%%d",
		word, file, sensenum);
    return(str);
}

char *LexFileName(pos, filenum)
int pos, filenum;
{
    /* Pass in POS and file number and return filename */
    if (pos == SATELLITE) pos = ADJ;
    return(wn_filenames[pos][filenum]);
}

int LexFileNum(posname, filename)
char *posname, *filename;
{
    int i = 0;
    int pos;
    char buf[100];

    /* Pass in strings for POS and filename and return file number */

    pos = StrToPos(posname);
    sprintf(buf, "%s.%s", posname, filename);
    while (strcmp(wn_filenames[pos][i], buf)) i++;
    return(i);
    
}

int StrToPos(str)
char *str;
{
    /* Pass in string for POS, return corresponding integer value */

    if (!strcmp(str, "noun"))
	return(NOUN);
    else if (!strcmp(str, "verb"))
	return(VERB);
    else if (!strcmp(str, "adj"))
	return(ADJ);
    else if (!strcmp(str, "adv"))
	return(ADV);
    else {
	return(-1);
    }
}

SynsetPtr GetSynsetForSense(sense)
char *sense;
{
    long offset;

    /* Pass in encoded sense string and return parsed sysnet structure */

    if ((offset = GetDataOffset(sense)))
	return(read_synset(GetPOS(sense),
			   offset,
			   GetWORD(sense)));
    else
	return(NULL);
}
    
long GetDataOffset(sense)
char *sense;
{
    char *line;
    char senseidx[256], *env;

    /* Pass in encoded sense string, return byte offset of corresponding
       synset in data file. */

    if (sensefp == NULL) {
	fprintf(stderr, "Error: Sense index file not open\n");
	return(0L);
    }
    line = bin_search(sense, sensefp);
    if (line) {
	while (*line++ != ' ');
	return(atol(line));
    } else
	return(0L);
}

int GetPolyCount(sense)
char *sense;
{
    IndexPtr idx;
    int sense_cnt;

    /* Pass in encoded sense string and return polysemy count
       for word in corresponding POS */

    idx = index_lookup(GetWORD(sense), GetPOS(sense));
    sense_cnt = idx->sense_cnt;
    free_index(idx);
    return(sense_cnt);
}

char *GetWORD(s)
char *s;
{
    static char word[100];
    int i=0;

    /* Pass in encoded sense string and return WORD */

    while ((word[i++] = *s++) != '%');
    word[i - 1] = '\0';
    return(word);
}

int GetPOS(s)
char *s;
{
    int pos;

    /* Pass in encoded sense string and return POS */

    while (*s++ != '%');	/* skip over WORD */
    sscanf(s, "%1d", &pos);
    return(pos == SATELLITE ? ADJ : pos);
}

char *FmtSynset(synptr, defn)
SynsetPtr synptr;
int defn;
{
    int i;
    static char synset[512];

    /* Reconstruct synset from synset pointer and return ptr to buffer */

    strcpy(synset, "{ ");
    for (i = 0; i < (synptr->wcount - 1); i++) {
	strcat(synset, synptr->words[i]);
	strcat(synset, ", ");
    }
    strcat(synset, synptr->words[i]);
    if (defn && synptr->defn) {
	strcat(synset, " (");
	strcat(synset, synptr->defn);
	strcat(synset, ") ");
    }
    strcat(synset, " }");
    return(synset);
}

char *WNSnsToStr(wd, sense, pos)
char *wd;
int sense; 
int pos;
{
	IndexPtr idx;
	SynsetPtr sptr, adjss;
	char nwd[128], temp[10], key[10], *line, *fname, snsstring[512];
	char *ret;
	int i, j, cnt, fnum, wnsns, sstype;
	long offset;

	strcpy(nwd, wd);

	idx = GetValidIndexPointer(nwd, pos);
	if(!idx) return(NULL);

	if(strcmp(idx->wd, nwd) != 0) 
		strcpy(nwd, idx->wd);

	cnt = 0;

	if(sense <= idx->off_cnt) offset = *(idx->offset + sense - 1);
	else return(NULL);

	sprintf(temp,"%ld",offset);
	for(i = 0; i < 8 - strlen(temp); i++)
		*(key+i) = '0';
	for(i = 8 - strlen(temp); i < 8; i++)
		*(key+i) = *(temp+(cnt++));
	*(key + i) = '\0';
	
	line = bin_search(key, datafps[pos]);
	if(!line) return(NULL);

	fnum = GetFileNum(line);
	fname = LexFileName(pos, fnum);
	if(!fname) return(NULL);

	sptr = read_synset(pos, offset, nwd);

	if ((sstype = getsstype(sptr->pos)) == SATELLITE) {
		for (j = 0; j < sptr->ptrcount; j++) {
			if (sptr->ptrtyp[j] == SIMPTR) {
				adjss = read_synset(sptr->ppos[j],sptr->ptroff[j],"");
				sptr->headword = malloc (strlen(adjss->words[0]) + 1);
				if(!sptr->headword) return(NULL);
				strcpy(sptr->headword, adjss->words[0]);
				sptr->headsense = adjss->wnsense[0];
				free_synset(adjss);
				break;
			}
		}
	}

	for (j=0; j < sptr->wcount; j++) {
		wnsns = *(sptr->wnsense+j);
		if(!strcmp(sptr->words[j], idx->wd) ||
		   (!strncmp(sptr->words[j],idx->wd,strlen(idx->wd)) &&
			*(sptr->words[j]+strlen(idx->wd)) == '('))
			break;
	}

	if(j == sptr->wcount) return(NULL);

	if (sstype == SATELLITE) 
		sprintf(snsstring,"%s%%%s%%%2.2d%%%s%%%2.2d",idx->wd,
				fname, wnsns, sptr->headword,sptr->headsense);
	else 
		sprintf(snsstring,"%s%%%s%%%2.2d",idx->wd,fname,wnsns);

	free_synset(sptr);
	ret = (char *) malloc (strlen(StrToSense(snsstring))+1);
	strcpy(ret, StrToSense(snsstring));
	return(ret);
}

IndexPtr GetValidIndexPointer(key, pos)
char *key;
int pos;
{
	IndexPtr idx = NULL;
	char *morphword;

	idx = getindex(key, pos);

	if(!idx) {
		if((morphword = morphstr(key, pos)) != NULL)
			while (morphword) {
				idx = getindex(morphword, pos);
				if(idx) break;
				morphword = morphstr(NULL, pos);
			}
	}

	return (idx);
}

int GetFileNum(string)
char *string;
{
	char *head = string;

	while (*string != ' ') string++;
	head = string++;
	while (*string != ' ') string++;
	*string = '\0';
	return (atoi(head));
}

int GetNextSense(string)
char *string;
{
	char *copy , *head = (char *) malloc (strlen(string)+1);
	int answer;

	if(!head) return(-1);
	copy = head;
	while(*string != '\0' && !isdigit(*string)) string++;
	while(*string != '\0' && isdigit(*string)) *copy++ = *string++;
	*copy = '\0';
	answer = atoi(head);
	free(head);
	return(answer);
}	

#ifdef NEXT
char *strdup(char *s)
{
	char *t;

	t = (char *) malloc (strlen(s) +1);
	if(t) strcpy(t, s);
	return(t);
}
#endif

