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
  
  search.c - WordNet library of search code
  
*/

#include "wnconsts.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
/* #include <malloc.h> */
#include "wn.h"
#include "wnglobals.h"

static char *Id = "$Id: search.c,v 1.74 93/07/29 13:46:08 wn Exp $";

/* For adjectives, indicates synset type */

#define DONT_KNOW	0
#define DIRECT_ANT	1	/* direct antonyms (cluster head) */
#define INDIRECT_ANT	2	/* indrect antonyms (similar) */
#define PERTAINYM	3	/* no antonyms or similars (pertainyms) */

#define ALLWORDS	0	/* print all words */
#define SKIP_ANTS	0	/* skip printing antonyms in printsynset() */
#define PRINT_ANTS	1	/* print antonyms in printsynset() */
#define SKIP_MARKER	0	/* skip printing adjective marker */
#define PRINT_MARKER	1	/* print adjective marker */

/* Trace types used by printspaces() to determine print sytle */

#define TRACEP		1	/* traceptrs() */
#define TRACEC		2	/* tracecoords() */
#define TRACEI		3	/* traceinherit() */

#define DEFON 1
#define DEFOFF 0

/* Forward declarations */

void printsynset(), printantsynset(), printbuffer(), printsense(), printword(),
    printspaces(), readfnames();
int HasPtr(), getsearchsense(), depthcheck();
char *printant();

static int prflag, sense, prsense;
static int overflag;		/* set when output buffer overflows */
static char *searchbuffer = NULL;
static unsigned long searchbufsize = SEARCHBUF; /* init to default size */
#ifdef MSDOS
static HGLOBAL hsearchbuffer;
#endif
static char tmpbuf[256];	/* general purpose printing buffer */
static char wdbuf[WORDBUF];	/* general purpose word buffer */
static int adj_marker;
static char *markers[] = {
    "",
    "(prenominal)",
    "(postnominal)",
    "(predicate)",
};

static char *freqcats[] = {
    "extremely rare","very rare","rare","uncommon","common",
    "familiar","very familiar","extremely familiar"
};


/*
 * Given a word and a part of speech,
 * returns an index entry structure IndexPtr.
 */
IndexPtr index_lookup(word,dbase)
char *word;
int  dbase;
{
    register char *ptrtok;
    register IndexPtr idx = NULL;
    register int j;
    FILE *fp;
    char *line;

    if ((fp = indexfps[dbase]) == NULL) {
	fprintf(stderr, "Error: %s indexfile not open\n", partnames[dbase]);
	return(NULL);
    }

    if ((line = bin_search(word, fp)) != NULL) {
        idx = (IndexPtr)malloc(sizeof(Index));
        assert(idx);
	
        idx->wd='\0';
        idx->pos='\0';
        idx->off_cnt=0;
        idx->sense_cnt=0;
        idx->offset='\0';
        idx->ptruse_cnt=0;
        idx->ptruse='\0';
	
	/* get the word */
	ptrtok=strtok(line," \n");
	
	idx->wd=(char *) malloc(strlen(ptrtok) + 1);
	assert(idx->wd);
	strcpy(idx->wd, ptrtok);
	
	/* get the part of speech */
	ptrtok=strtok(NULL," \n");
	idx->pos=(char *) malloc(strlen(ptrtok) + 1);
	assert(idx->pos);
	strcpy(idx->pos, ptrtok);
	
	/* get the collins count */
	ptrtok=strtok(NULL," \n");
	idx->sense_cnt = atoi(ptrtok);
	
	/* get the number of pointers types */
	ptrtok=strtok(NULL," \n");
	idx->ptruse_cnt = atoi(ptrtok);

	if (idx->ptruse_cnt) {
	    idx->ptruse = (int *) malloc(idx->ptruse_cnt * (sizeof(int)));
	    assert(idx->ptruse);
	
	    /* get the pointers types */
	    for(j=0;j < idx->ptruse_cnt; j++) {
		ptrtok=strtok(NULL," \n");
		idx->ptruse[j] = getptrtype(ptrtok);
	    }
	}
	
	/* get the number of offsets */
	ptrtok=strtok(NULL," \n");
	idx->off_cnt = atoi(ptrtok);
	
	/* make space for the offsets */
	idx->offset = (long *) malloc(idx->off_cnt * (sizeof(long)));
	assert(idx->offset);
	
	/* get the offsets */
	for(j=0;j<idx->off_cnt;j++) {
	    ptrtok=strtok(NULL," \n");
	    idx->offset[j] = atol(ptrtok);
	}
    }
    return(idx);
}

#define MAX_TRIES	5

IndexPtr getindex(searchstr, dbase)
char *searchstr;
int dbase;
{
    register int offset = 0, i, j, k;
    register char c;
    IndexPtr idx[MAX_TRIES];
    char strings[MAX_TRIES][WORDBUF]; /* vector of search strings */
    
    /* Prepare vector of search strinsg */
    
    for (i = 0; i < MAX_TRIES; i++) {
	strcpy(strings[i], searchstr);
	idx[i] = NULL;
    }
    strsubst(strings[1], '_', '-');
    strsubst(strings[2], '-', '_');

    /* remove all spaces and hyphens from last search string, then
       all periods */
    for (i = j = k = 0; (c = searchstr[i]) != '\0'; i++) {
	if (c != '_' && c != '-')
	    strings[3][j++] = c;
	if (c != '.')
	    strings[4][k++] = c;
    }
    strings[3][j] = c;		/* copy final NULL */
    strings[4][k] = c;		/* copy final NULL */
	
    /* Eliminate duplications */

    for (i = 1; i < MAX_TRIES; i++)
	if (!strcmp(strings[0], strings[i]))
	    strings[i][0] = '\0';	
    
    /* Get index entries for all versions of search string, and
       return lowest numbered offset (assume if more than one
       found, they're in the same syset as alternate spellings) */
    
    for (i = (MAX_TRIES - 1); i >= 0; i--)
	if (strings[i][0] != '\0')
	    if (idx[i] = index_lookup(strings[i], dbase))
		offset = i;

    for (i = 0; i < MAX_TRIES; i++) /* free up unused index structures */
	if (i != offset && idx[i])
	    free_index(idx[i]);
    return(idx[offset]);
}

SynsetPtr read_synset(dbase,boffset,word)
int  dbase;
long boffset;
char *word;
{
    FILE *fp;
    static char line[LINEBUF];
    register char *ptrtok;
    char *tmpptr;
    char too[3],tfrm[3];
    register int i;
    register SynsetPtr synptr;
    
    if((fp = datafps[dbase]) == NULL) {
	fprintf(stderr, "Error: %s datafile not open\n", partnames[dbase]);
	return(NULL);
    }
    
    synptr = (SynsetPtr)malloc(sizeof(Synset));
    assert(synptr);
    
    synptr->hereiam=0;
    synptr->sstype=DONT_KNOW;
    synptr->fnum=0;
    synptr->pos='\0';
    synptr->wcount=0;
    synptr->words='\0';
    synptr->whichword=0;
    synptr->ptrcount=0;
    synptr->ptrtyp='\0';
    synptr->ptroff='\0';
    synptr->ppos = '\0';
    synptr->pto='\0';
    synptr->pfrm='\0';
    synptr->fcount=0;
    synptr->frmid='\0';
    synptr->frmto='\0';
    synptr->defn='\0';
    synptr->nextss = NULL;
    synptr->searchtype = -1;
    synptr->ptrlist = NULL;
    synptr->headword = NULL;
    synptr->headsense = 0;

    fseek(fp, boffset, 0);
    fgets(line, LINEBUF, fp);
    
    ptrtok=line;
    
    /* looking at offset */
    ptrtok=strtok(line," \n");
    synptr->hereiam = atol(ptrtok);
    
    /* looking at FNUM */
    ptrtok=strtok(NULL," \n");
    synptr->fnum=atoi(ptrtok);
    
    /* looking at POS */
    ptrtok=strtok(NULL," \n");
    synptr->pos = (char *)malloc(strlen(ptrtok) + 1);
    assert(synptr->pos);
    strcpy(synptr->pos, ptrtok);
    if (getsstype(synptr->pos) == SATELLITE)
	synptr->sstype = INDIRECT_ANT;
    
    /* looking at numwords */
    ptrtok=strtok(NULL," \n");
    synptr->wcount=strtol(ptrtok,NULL,16);
    
    synptr->words=(char **)malloc(synptr->wcount  * sizeof(char *));
    assert(synptr->words);
    synptr->wnsense = (int *)malloc(synptr->wcount * sizeof(int));
    assert(synptr->wnsense);
    
    for(i=0;i<synptr->wcount; i++) {
	ptrtok=strtok(NULL," \n");
	synptr->words[i]= (char *)malloc(strlen(ptrtok) + 1);
	assert(synptr->words[i]);
	strcpy(synptr->words[i], ptrtok);
	
	/* is this the word we're looking for? */
	
	if(!strcmp(word,strtolower(ptrtok))) /* strtolower does deadjify */
	    synptr->whichword = i+1;
	
	ptrtok = strtok(NULL, " \n");
	sscanf(ptrtok, "%x", &synptr->wnsense[i]);
    }
    
    /* get the pointer count */
    ptrtok=strtok(NULL," \n");
    synptr->ptrcount=atoi(ptrtok);

    if (synptr->ptrcount) {

	/* alloc storage for the pointers */
	synptr->ptrtyp=(int *)malloc(synptr->ptrcount * sizeof(int));
	assert(synptr->ptrtyp);
	synptr->ptroff=(long *)malloc(synptr->ptrcount * sizeof(long));
	assert(synptr->ptroff);
	synptr->ppos = (int *)malloc(synptr->ptrcount * sizeof(int));
	assert(synptr->ppos);
	synptr->pto=(int *)malloc(synptr->ptrcount * sizeof(int));
	assert(synptr->pto);
	synptr->pfrm=(int *)malloc(synptr->ptrcount * sizeof(int));
	assert(synptr->pfrm);
    
	for(i=0;i<synptr->ptrcount;i++) {
	    /* get the pointer type */
	    ptrtok=strtok(NULL," \n");
	    synptr->ptrtyp[i] = getptrtype(ptrtok);
	    if (dbase == ADJ && synptr->sstype == DONT_KNOW) {
		if (synptr->ptrtyp[i] == PERTPTR)
		    synptr->sstype = PERTAINYM;
		else if (synptr->ptrtyp[i] == ANTPTR)
		    synptr->sstype = DIRECT_ANT;
	    }

	    /* get the pointer offset */
	    ptrtok=strtok(NULL," \n");
	    synptr->ptroff[i] = atol(ptrtok);
	
	    /* get the pointer part of speech */
	    ptrtok = strtok(NULL, " \n");
	    synptr->ppos[i] = getpos(ptrtok);
	
	    /* get the lexp to/from restrictions */
	    ptrtok=strtok(NULL," \n");
	
	    tmpptr=ptrtok;
	    strncpy(tfrm,tmpptr,2);
	    tfrm[2]='\0';
	    synptr->pfrm[i]=atoi(tfrm);
	
	    tmpptr += 2;
	    strncpy(too,tmpptr,2);
	    too[2]='\0';
	    synptr->pto[i]=atoi(too);
	}
    }
    
    /* retireve optional information from verb synset */
    if(dbase == VERB) {
	ptrtok=strtok(NULL," \n");
	synptr->fcount=atoi(ptrtok);
	
	/* allocate frame storage */
	
	synptr->frmid=(int *)malloc(synptr->fcount * sizeof(int));  
	assert(synptr->frmid);
	synptr->frmto=(int *)malloc(synptr->fcount * sizeof(int));  
	assert(synptr->frmto);
	
	for(i=0;i<synptr->fcount;i++) {
	    /* skip the frame pointer (+) */
	    ptrtok=strtok(NULL," \n");
	    
	    ptrtok=strtok(NULL," \n");
	    synptr->frmid[i]=atoi(ptrtok);
	    
	    ptrtok=strtok(NULL," \n");
	    synptr->frmto[i]=atoi(ptrtok);
	}
    }
    
    /* get the optional definition */
    
    ptrtok=strtok(NULL," \n");
    if(ptrtok) {
	ptrtok=strtok(NULL," \n");
	sprintf(tmpbuf,"");
	while(ptrtok != NULL) {
	    strcat(tmpbuf,ptrtok);
	    ptrtok=strtok(NULL," \n");
	    if(ptrtok)
		strcat(tmpbuf," ");
	}
	synptr->defn=(char *)malloc(strlen(tmpbuf) + 4);
	assert(synptr->defn);
	sprintf(synptr->defn,"(%s)",tmpbuf);
    }

    return(synptr);
}

/* Free a synset linked list allocated by findtheinfo_ds() */

void free_syns(synptr)
SynsetPtr synptr;
{
    SynsetPtr cursyn, nextsyn;

    if (synptr) {
	cursyn = synptr;
	nextsyn = synptr->nextss;
	do {
	    free_synset(cursyn);
	    cursyn = nextsyn;
	} while (cursyn && (nextsyn = cursyn->nextss));
    }
}

/* Free a synset */

void free_synset(synptr)
SynsetPtr synptr;
{
    int i;
    
    free(synptr->pos);
    for (i = 0; i < synptr->wcount; i++){
	free(synptr->words[i]);
    }
    free(synptr->words);
    free(synptr->wnsense);
    if (synptr->ptrcount) {
	free(synptr->ptrtyp);
	free(synptr->ptroff);
	free(synptr->ppos);
	free(synptr->pto);
	free(synptr->pfrm);
    }
    if (synptr->fcount) {
	free(synptr->frmid);
	free(synptr->frmto);
    }
    if (synptr->defn)
	free(synptr->defn);
    if (synptr->headword)
	free(synptr->headword);
    if (synptr->ptrlist)
	free_syns(synptr->ptrlist); /* changed from free_synset() */
    free(synptr);
}

/* Free an index structure */

void free_index(idx)
IndexPtr idx;
{
    free(idx->wd);
    free(idx->pos);
    if (idx->ptruse)
	free(idx->ptruse);
    free(idx->offset);
    free(idx);
}

/* Recursive search algorithm to trace a pointer tree */

static void traceptrs(synptr,ptrtyp,dbase,depth)
SynsetPtr synptr;
int ptrtyp;
int dbase;
int depth;
{
    register int i;
    int extraindent = 0;
    register SynsetPtr cursyn;
    char prefix[40], tbuf[20];

    if (ptrtyp < 0) {
	ptrtyp = -ptrtyp;
	extraindent = 2;
    }
    
    for(i=0;i<synptr->ptrcount;i++) {
	if((synptr->ptrtyp[i] == ptrtyp) &&
	   ((synptr->pfrm[i] == 0) ||
	    (synptr->pfrm[i] == synptr->whichword))) {
	    
	    if(!prflag) {
		printsense(synptr);
		printsynset("",synptr,"\n",DEFON, ALLWORDS, PRINT_ANTS);
		prflag = 1;
	    }

	    printspaces(TRACEP, depth + extraindent);

	    switch(ptrtyp) {
	    case PERTPTR:
		if (dbase == ADV) 
		    sprintf(prefix, "Derived from %s -> ",
			    partnames[synptr->ppos[i]]);
		else
		    sprintf(prefix, "Pertains to %s -> ",
			    partnames[synptr->ppos[i]]);
		break;
	    case HASMEMBERPTR:
		sprintf(prefix, "   HAS MEMBER: ");
		break;
	    case HASSTUFFPTR:
		sprintf(prefix, "   HAS SUBSTANCE: ");
		break;
	    case HASPARTPTR:
		sprintf(prefix, "   HAS PART: ");
		break;
	    case ISMEMBERPTR:
		sprintf(prefix, "   MEMBER OF: ");
		break;
	    case ISSTUFFPTR:
		sprintf(prefix, "   SUBSTANCE OF: ");
		break;
	    case ISPARTPTR:
		sprintf(prefix, "   PART OF: ");
		break;
	    default:
		sprintf(prefix, "=> ");
		break;
	    }
	    
	    cursyn=read_synset(synptr->ppos[i], synptr->ptroff[i], "");

	    if (ptrtyp == PERTPTR && synptr->pto[i] != 0) {
		sprintf(tbuf, " (Sense %d)\n",
			getsearchsense(cursyn, synptr->pto[i]));
		printsynset(prefix, cursyn, tbuf, DEFOFF, synptr->pto[i],
			    SKIP_ANTS);
	    } else 
		printsynset(prefix, cursyn, "\n", DEFON, ALLWORDS, PRINT_ANTS);

#ifdef FOOP
	    if (ptrtyp == PERTPTR && getpos(synptr->pos) == ADV) {
		printspaces(TRACEP, depth);
		printsynset("=> ", cursyn, "\n", DEFON, ALLWORDS, PRINT_ANTS);
		if (getsstype(cursyn->pos) == SATELLITE)
		    traceptrs(cursyn, -SIMPTR, dbase, 0);
	    }
#endif
	    
	    if(depth) {
		depth = depthcheck(depth, cursyn);
		traceptrs(cursyn,ptrtyp,dbase,(depth+1));

		free_synset(cursyn);
	    } else
		free_synset(cursyn);
	}
    }
}

SynsetPtr traceptrs_ds(synptr,ptrtyp,dbase,depth)
SynsetPtr synptr;
int ptrtyp;
int dbase;
int depth;
{
    register int i;
    register SynsetPtr cursyn, synlist = NULL, lastsyn = NULL;
    
    /* If synset is a satellite, find the head word of its
       head synset and the head word's sense number. */

    if (getsstype(synptr->pos) == SATELLITE) {
	for (i = 0; i < synptr->ptrcount; i++)
	    if (synptr->ptrtyp[i] == SIMPTR) {
		cursyn = read_synset(synptr->ppos[i],
				      synptr->ptroff[i],
				      "");
		synptr->headword = malloc(strlen(cursyn->words[0]) + 1);
		assert(synptr->headword);
		strcpy(synptr->headword, cursyn->words[0]);
		synptr->headsense = cursyn->wnsense[0];
		free_synset(cursyn);
		break;
	    }
    }
	    
    for(i=0;i<synptr->ptrcount;i++) {
	if((synptr->ptrtyp[i] == ptrtyp) &&
	   ((synptr->pfrm[i] == 0) ||
	    (synptr->pfrm[i] == synptr->whichword))) {
	    
	    cursyn=read_synset(synptr->ppos[i], synptr->ptroff[i], "");
	    cursyn->searchtype = ptrtyp;

	    if (lastsyn)
		lastsyn->nextss = cursyn;
	    if (!synlist)
		synlist = cursyn;
	    lastsyn = cursyn;

	    if(depth) {
		depth = depthcheck(depth, cursyn);
		cursyn->ptrlist = traceptrs_ds(cursyn,ptrtyp,dbase,(depth+1));
	    }	
	}
    }
    return(synlist);
}

static void tracecoords(synptr,ptrtyp,dbase,depth)
SynsetPtr synptr;
int ptrtyp;
int dbase;
int depth;
{
    register int i;
    register SynsetPtr cursyn;
    
    for(i=0;i<synptr->ptrcount;i++) {
	if((synptr->ptrtyp[i] == HYPERPTR) &&
	   ((synptr->pfrm[i] == 0) ||
	    (synptr->pfrm[i] == synptr->whichword))) {
	    
	    if(!prflag) {
		printsense(synptr);
		printsynset("",synptr,"\n",DEFON, ALLWORDS, PRINT_ANTS);
		prflag = 1;
	    }
	    printspaces(TRACEC, depth);

	    cursyn=read_synset(dbase,synptr->ptroff[i],"");

	    printsynset("-> ",cursyn,"\n",DEFON, ALLWORDS, SKIP_ANTS);

	    traceptrs(cursyn,ptrtyp,NOUN,depth);
	    
	    if(depth) {
		depth = depthcheck(depth, cursyn);
		tracecoords(cursyn,ptrtyp,dbase,(depth+1));
		free_synset(cursyn);
	    } else
		free_synset(cursyn);
	}
    }
}

/* Trace through the hypernym tree and print all MEMBER, STUFF
   and PART info. */

static void traceinherit(synptr,ptrbase,dbase,depth)
SynsetPtr synptr;
int ptrbase;
int dbase;
int depth;
{
    register int i;
    register SynsetPtr cursyn;
    
    for(i=0;i<synptr->ptrcount;i++) {
	if((synptr->ptrtyp[i] == HYPERPTR) &&
	   ((synptr->pfrm[i] == 0) ||
	    (synptr->pfrm[i] == synptr->whichword))) {
	    
	    if(!prflag) {
		printsense(synptr);
		printsynset("",synptr,"\n",DEFON, ALLWORDS, PRINT_ANTS);
		prflag = 1;
	    }
	    printspaces(TRACEI, depth);
	    
	    cursyn=read_synset(dbase,synptr->ptroff[i],"");

	    printsynset("=> ",cursyn,"\n",DEFON, ALLWORDS, SKIP_ANTS);
	    
	    traceptrs(cursyn,ptrbase,NOUN,depth);
	    traceptrs(cursyn,ptrbase + 1,NOUN,depth);
	    traceptrs(cursyn,ptrbase + 2,NOUN,depth);
	    
	    if(depth) {
		depth = depthcheck(depth, cursyn);
		traceinherit(cursyn,ptrbase,dbase,(depth+1));
		free_synset(cursyn);
	    } else
		free_synset(cursyn);
	}
    }
}

static void traceadjant(synptr)
SynsetPtr synptr;
{
    SynsetPtr newsynptr;
    register int i, j;
    int anttype = DIRECT_ANT;
    register SynsetPtr simptr, antptr;
    static char similar[] = "        => ";
    
    /* This search is only applicable for ADJ synsets which have
       either direct or indirect antonyms (not valid for pertainyms). */
    
    if (synptr->sstype == DIRECT_ANT || synptr->sstype == INDIRECT_ANT) {
	printsense(synptr);
	printsynset("", synptr, "\n\n", DEFON, ALLWORDS, PRINT_ANTS);
	
	/* if indirect, get cluster head */
	
	if(synptr->sstype == INDIRECT_ANT) {
	    anttype = INDIRECT_ANT;
	    i = 0;
	    while (synptr->ptrtyp[i] != SIMPTR) i++;
	    newsynptr = read_synset(ADJ, synptr->ptroff[i], "");
	} else
	    newsynptr = synptr;
	
	/* find antonyms - if direct, make sure that the antonym
	   ptr we're looking at is from this word */
	
	for (i = 0; i < newsynptr->ptrcount; i++) {
	    if (newsynptr->ptrtyp[i] == ANTPTR &&
		((anttype == DIRECT_ANT &&
		  newsynptr->pfrm[i] == newsynptr->whichword) ||
		 (anttype == INDIRECT_ANT))) {
		
		/* read the antonym's synset and print it.  if a
		   direct antonym, print it's satellites. */
		
		antptr = read_synset(ADJ, newsynptr->ptroff[i], "");
    
		if (anttype == DIRECT_ANT) {
		    printsynset("", antptr, "\n", DEFON, ALLWORDS, PRINT_ANTS);
		    for(j = 0; j < antptr->ptrcount; j++) {
			if(antptr->ptrtyp[j] == SIMPTR) {
			    simptr = read_synset(ADJ, antptr->ptroff[j], "");
			    printsynset(similar, simptr, "\n",
					DEFON, ALLWORDS, SKIP_ANTS);
			    free_synset(simptr);
			}
		    }
		} else
		    printantsynset(antptr, "\n", anttype, DEFON);

		free_synset(antptr);
	    }
	}
	if (newsynptr != synptr)
	    free_synset(newsynptr);
    }
}

static void partsall(synptr, ptrtyp)
SynsetPtr synptr;
int ptrtyp;
{
    register int ptrbase;
    int hasptr = 0;
    
    ptrbase = (ptrtyp == HMERONYM) ? HASMEMBERPTR : ISMEMBERPTR;
    
    /* First, print out the MEMBER, STUFF, PART info for this synset */
    
    if (HasPtr(synptr, ptrbase)) {
	traceptrs(synptr, ptrbase, NOUN, 1);
	hasptr++;
    }
    if (HasPtr(synptr, ptrbase + 1)) {
	traceptrs(synptr, ptrbase + 1, NOUN, 1);
	hasptr++;
    }
    if (HasPtr(synptr, ptrbase + 2)) {
	traceptrs(synptr, ptrbase + 2, NOUN, 1);
	hasptr++;
    }
    
    /* Print out MEMBER, STUFF, PART info for hypernyms on
       HMERONYM search only */
	
    if (hasptr && ptrtyp == HMERONYM) 
	traceinherit(synptr, ptrbase, NOUN, 1);
}

static void printframe(synptr, prsynset)
SynsetPtr synptr;
int prsynset;
{
    register int i;

    if (prsynset) {
	printsense(synptr);
	printsynset("",synptr,"\n",DEFON, ALLWORDS, PRINT_ANTS);
    }
    
    for(i = 0; i < synptr->fcount; i++) {
	if ((synptr->frmto[i] == synptr->whichword) ||
	    (synptr->frmto[i] == 0)) {
	    if (synptr->frmto[i] == synptr->whichword)
		printbuffer("          => ");
	    else
		printbuffer("          *> ");
	    printbuffer(frametext[synptr->frmid[i]]);
	    printbuffer("\n");
	}
    }
}

static void printseealso(synptr, dbase)
SynsetPtr synptr;
int dbase;
{
    register SynsetPtr cursyn;
    register int i, first = 1;
    static char firstline[] = "          Also See-> ";
    static char otherlines[] = ", ";
    char *prefix = firstline;


    /* Find all SEEALSO pointers from the searchword and print the
       word or synset pointed to. */

    for(i = 0; i < synptr->ptrcount; i++) {
	if ((synptr->ptrtyp[i] == SEEALSOPTR) &&
	    ((synptr->pfrm[i] == 0) ||
	     (synptr->pfrm[i] == synptr->whichword))) {

	    cursyn = read_synset(dbase, synptr->ptroff[i], "");

	    if (synptr->pto[i] == 0)
		printsynset(prefix, cursyn, "", DEFOFF,
			    ALLWORDS, SKIP_ANTS);
	    else
		printsynset(prefix, cursyn, "", DEFOFF,
			    synptr->pto[i], SKIP_ANTS);
	    free_synset(cursyn);

	    if (first) {
		prefix = otherlines;
		first = 0;
	    }
	}
    }
    if (!first)
	printbuffer("\n");
}

static void freq_word(index)
register IndexPtr index;
{
    register int familiar=0;
    register int cnt;
    static char *a_an[] = {
	"", "a noun", "a verb", "an adjective", "an adverb" };
    
    if(index) {
	cnt = index->sense_cnt;
	if (cnt == 0) familiar = 0;
	if (cnt == 1) familiar = 1;
	if (cnt == 2) familiar = 2;
	if (cnt >= 3 && cnt <= 4) familiar = 3;
	if (cnt >= 5 && cnt <= 8) familiar = 4;
	if (cnt >= 9 && cnt <= 16) familiar = 5;
	if (cnt >= 17 && cnt <= 32) familiar = 6;
	if (cnt > 32 ) familiar = 7;
	
	sprintf(tmpbuf,
		"%s used as %s is %s (polysemy count = %d)\n",
		index->wd, a_an[getpos(index->pos)], freqcats[familiar], cnt);
	printbuffer(tmpbuf);
    }
}

#ifdef HASGREP
static void wngrep(string, dbase)
char *string;
int dbase;
{
    register int c;
    register char *tp;
    char *tmpfile = "/tmp/wngrep";
    FILE *fp;

    strsubst(strcpy(wdbuf, string), '_', ' ');
    
    unlink(tmpfile);
    sprintf(tmpbuf, "wngrep \"%s\" %s >%s", wdbuf, partnames[dbase], tmpfile);
    system(tmpbuf);

    if ((fp = fopen(tmpfile, "r")) != NULL) {
	sprintf(tmpbuf, "Grep of %s %s\n", partnames[dbase], wdbuf);
	printbuffer(tmpbuf);
	tp = tmpbuf;
	*tp++ = '\t';
	while ((c = getc(fp)) != EOF) {
	    *tp++ = c;
	    if (c == '\n') {
		*tp = '\0';
		printbuffer(tmpbuf);
		tp = tmpbuf;
		*tp++ = '\t';
	    }
	}
	fclose(fp);
	unlink(tmpfile);
    } else {
	fprintf(stderr, "Error: can't open temp file \"%s\"\n", tmpbuf);
	return;
    }
}
#endif

/* Determine if any of the synsets that this word is in have inherited
   meronyms or holonyms. */

static int HasHoloMero(index, ptrtyp)
IndexPtr index;
int ptrtyp;
{
    register int i;
    register SynsetPtr synset;
    register int found=0;
    int ptrbase;
    
    ptrbase = (ptrtyp == HMERONYM) ? HASMEMBERPTR : ISMEMBERPTR;
    
    for(i = 0; i < index->off_cnt; i++) {
	synset = read_synset(NOUN, index->offset[i], "");
	found += HasPtr(synset, ptrbase);
	found += HasPtr(synset, ptrbase + 1);
	found += HasPtr(synset, ptrbase + 2);
	free_synset(synset);
    }
    return(found);
}

static int HasPtr(synptr,ptrtyp)
SynsetPtr synptr;
int ptrtyp;
{
    register int i;
    
    for(i = 0; i < synptr->ptrcount; i++) {
        if(synptr->ptrtyp[i] == ptrtyp) {
	    return(1);
	}
    }
    return(0);
}

static int depthcheck(depth, synptr)
int depth;
SynsetPtr synptr;
{
    if(depth >= MAXDEPTH) {
/*	if(depth == MAXDEPTH) { */
	    fprintf(stderr, "Error Cycle detected\n");
	    fprintf(stderr,"   %s\n", synptr->words[0]);
/*	} */
/*	if(depth == MAXDEPTH + 5) */
	    depth = -1;		/* reset to get one more trace then quit */
    }
    return(depth);
}

/*
  Search code interfaces to WordNet database

  findtheinfo() - print search results and return ptr to output buffer
  findtheinfo_ds() - return search results in linked list data structrure
*/

char *findtheinfo(searchstr,dbase,ptrtyp,whichsense)
char *searchstr;
int dbase, ptrtyp, whichsense;
{
    register SynsetPtr cursyn;
    register IndexPtr idx = NULL;
    register int depth = 0;
    
    if (!searchbuffer) {	/* initialize search buffer */
#ifdef MSDOS
	hsearchbuffer = GlobalAlloc(GMEM_MOVEABLE, SEARCHBUF * sizeof(char));
	searchbuffer = (char *) GlobalLock(hsearchbuffer);
#else
	searchbuffer = (char *) malloc(SEARCHBUF * sizeof(char));
#endif
	assert(searchbuffer);
    }
    
    sprintf(searchbuffer,"");
    
    if(ptrtyp < 0) {		/* negative type passed for recursive search */
	ptrtyp = -ptrtyp;
	depth=1;
    }
    
    if(ptrtyp == FREQ) {
	if ((idx = getindex(searchstr, dbase)) == NULL)
	    return(searchbuffer);
	SenseCount = idx->off_cnt; /* number of senses word has */
	freq_word(idx);
#ifdef HASGREP
    } else if (ptrtyp == WNGREP) {
	wngrep(searchstr, dbase);
#endif
    } else {
	if ((idx = getindex(searchstr, dbase)) == NULL)
	    return(searchbuffer);

	OutSenseCount = 0;	/* keep track of # senses printed */
	SenseCount = idx->off_cnt; /* number of senses word has */

	/* Only bother with extra sense msgs if looking at all senses */
	if (whichsense == ALLSENSES) {
	    sprintf(tmpbuf,
"                                                                         \n");
	    printbuffer(tmpbuf);
	}
	
	/* Go through all of the searchword's senses in the
	   database and perform the search requested. */

	for(sense = 0; sense < idx->off_cnt; sense++) {

	    if (whichsense == ALLSENSES || whichsense == sense + 1) {

		prflag=0;
		cursyn=read_synset(dbase,idx->offset[sense],idx->wd);
	    
		switch(ptrtyp) {
		case ANTPTR:
		    if(dbase == ADJ)
			traceadjant(cursyn);
		    else
			traceptrs(cursyn,ANTPTR,dbase,depth);
		    break;
		    
		case COORDS:
		    tracecoords(cursyn,HYPOPTR,dbase,depth);
		    break;
		    
		case FRAMES:
		    printframe(cursyn, 1);
		    break;
		    
		case MERONYM:
		    traceptrs(cursyn,HASMEMBERPTR,dbase,depth);
		    traceptrs(cursyn,HASSTUFFPTR,dbase,depth);
		    traceptrs(cursyn,HASPARTPTR,dbase,depth);
		    break;
		    
		case HOLONYM:
		    traceptrs(cursyn,ISMEMBERPTR,dbase,depth);
		    traceptrs(cursyn,ISSTUFFPTR,dbase,depth);
		    traceptrs(cursyn,ISPARTPTR,dbase,depth);
		    break;
		    
		case HMERONYM:
		    partsall(cursyn, HMERONYM);
		    break;
		    
		case HHOLONYM:
		    partsall(cursyn, HHOLONYM);
		    break;
		    
		case SEEALSOPTR:
		    printseealso(cursyn, dbase);
		    break;
		    
		case SIMPTR:
		case SYNS:
		case HYPERPTR:
		    printsense(cursyn);
		    
		    /* Don't print antonyms on a satellite synset */
		    printsynset("", cursyn, "\n", DEFON, ALLWORDS,
				((getsstype(cursyn->pos) == SATELLITE)
				 ? SKIP_ANTS : PRINT_ANTS));
		    prflag = 1;
		    
		    traceptrs(cursyn,ptrtyp,dbase,depth);
		    
		    if (dbase == ADJ || dbase == ADV)
			traceptrs(cursyn, PERTPTR, dbase, depth);
		    
		    if (saflag)	/* print SEE ALSO pointers if requested */
			printseealso(cursyn, dbase);
		    
		    if (frflag)
			printframe(cursyn, 0);
		    break;
		    
		default:
		    traceptrs(cursyn,ptrtyp,dbase,depth);
		    break;
		    
		}
		free_synset(cursyn);
	    }
	    if (whichsense == sense + 1) 
		break;
	}
	if (whichsense == ALLSENSES)
	    printbuffer("\n");

	/* patch in the number of senses output if looked at all senses */

	if (overflag) {
	    sprintf(searchbuffer,
		    "Search too large.  Narrow search and try again\n");
	    overflag = 0;
	} else if (whichsense == ALLSENSES) {
	    if (OutSenseCount == idx->off_cnt) {
		if (OutSenseCount == 1)
		    sprintf(tmpbuf, "1 sense of %s", idx->wd);
		else
		    sprintf(tmpbuf, "%d senses of %s", OutSenseCount, idx->wd);
	    } else
		sprintf(tmpbuf, "%d of %d senses of %s",
			OutSenseCount, idx->off_cnt, idx->wd);
	    strncpy(searchbuffer, tmpbuf, strlen(tmpbuf));
	}
    }
    if (idx)
	free_index(idx);
    return(strsubst(searchbuffer,'_',' '));
}

SynsetPtr findtheinfo_ds(searchstr,dbase,ptrtyp, whichsense)
char *searchstr;
int dbase, ptrtyp, whichsense;
{
    IndexPtr idx;
    SynsetPtr cursyn;
    SynsetPtr synlist = NULL, lastsyn = NULL;
    int depth = 0;
    
    if ((idx = getindex(searchstr, dbase)) != NULL) {
	
	if(ptrtyp < 0) {
	    ptrtyp = -ptrtyp;
	    depth = 1;
	}

	SenseCount = idx->off_cnt; /* number of senses word has */
	
	/* Go through all of the searchword's senses in the
	   database and perform the search requested. */
	
	for(sense = 0; sense < idx->off_cnt; sense++) {
	    if (whichsense == ALLSENSES || whichsense == sense + 1) {
		cursyn=read_synset(dbase,idx->offset[sense],idx->wd);
		if (lastsyn)
		    lastsyn->nextss = cursyn;
		if (!synlist)
		    synlist = cursyn;
	    
		cursyn->searchtype = ptrtyp;
		cursyn->ptrlist = traceptrs_ds(cursyn,ptrtyp,dbase,depth);
	    
		lastsyn = cursyn;

		if (whichsense == sense + 1)
		    break;
	    }
	}      
	free_index(idx);
    }
    return(synlist);
}

/*
  Set bit for each search type that is valid for the
  searchword/dbase passed and return bit mask.
*/
  
unsigned long is_defined(searchstr, dbase)
char *searchstr;
int  dbase;
{
    register IndexPtr index;
    register int i;
    unsigned long retval=0;
    
    if((index = getindex(searchstr, dbase)) != NULL) {

	SenseCount = index->off_cnt; /* number of senses word has */
	
	/* set bits that must be true for all words */
	
	retval = bit(SIMPTR) | bit(FREQ) | bit(SYNS);
#ifdef HASGREP
	retval |= bit(WNGREP);
#endif
	for(i = 0; i < index->ptruse_cnt; i++) {
	    
	    retval |= bit(index->ptruse[i]); /* set bit for this ptr */

	    if (dbase == NOUN) {

		/* set generic HOLONYM and/or MERONYM bit if necessary */
	    
		if(index->ptruse[i] >= ISMEMBERPTR &&
		   index->ptruse[i] <= ISPARTPTR)
		    retval |= bit(HOLONYM);
		else if(index->ptruse[i] >= HASMEMBERPTR &&
			index->ptruse[i] <= HASPARTPTR)
		    retval |= bit(MERONYM);
	    } else if (dbase == ADJ && index->ptruse[i] == SIMPTR)
		retval |= bit(ANTPTR);
	}

	if (dbase == NOUN) {
	    if (HasHoloMero(index, HMERONYM))
		retval |= bit(HMERONYM);
	    if (HasHoloMero(index, HHOLONYM))
		retval |= bit(HHOLONYM);
	    if (retval & bit(HYPERPTR))
		retval |= bit(COORDS);
	}	
	free_index(index);
    }
    return(retval);
}

unsigned int in_wn(word, pos)
{
    int i;
    unsigned int retval = 0;

    if (pos == ALL_POS) {
	for (i = 1; i < NUMPARTS + 1; i++)
	    if (indexfps[i] != NULL && bin_search(word, indexfps[i]) != NULL)
		retval |= bit(i);
    } else if (indexfps[pos] != NULL && bin_search(word,indexfps[pos]) != NULL)
	    retval |= bit(pos);
    return(retval);
}

/* Strip off () enclosed comments from a word */

static char *deadjify(word)
char *word;
{
    register char *y;
    
    adj_marker = UNKNOWN_MARKER; /* default if not adj or unknown */
    
    y=word;
    while(*y) {
	if(*y == '(') {
	    if (!strncmp(y, "(a)", 3))
		adj_marker = ATTRIBUTIVE;
	    else if (!strncmp(y, "(ip)", 4))
		adj_marker = IMMED_POSTNOMINAL;
	    else if (!strncmp(y, "(p)", 3))
		adj_marker = PREDICATIVE;
	    *y='\0';
	} else 
	    y++;
    }
    return(word);
}

static int getsearchsense(synptr, whichword)
SynsetPtr synptr;
int whichword;
{
    register IndexPtr idx;
    register int i;

    strsubst(strcpy(wdbuf, synptr->words[whichword - 1]), '_', ' ');
    strtolower(wdbuf);
		       
    if (idx = index_lookup(wdbuf, getpos(synptr->pos))) {
	for (i = 0; i < idx->off_cnt; i++)
	    if (idx->offset[i] == synptr->hereiam) {
		free_index(idx);
		return(i + 1);
	    }
	free_index(idx);
    }
    return(0);
}

static void printsynset(head, synptr, tail, definition, wdnum, antflag)
char *head, *tail;
SynsetPtr synptr;
int definition, wdnum, antflag;
{
    register int i, wdcnt;
    char *str;
    char tbuf[SMLINEBUF];
    
    sprintf(tbuf,"");
    
    if((str = getenv("WNDEBUG")) && *str != '\0') {
	sprintf(tbuf,"<%s>",
		(wn_filenames[getpos(synptr->pos)])[synptr->fnum]);
	prsense = 1;
    } else
	prsense = 0;
    
    strcat(tbuf,head);

    if (wdnum)			/* print only specific word asked for */
	printword(tbuf, synptr, wdnum - 1, PRINT_MARKER, antflag);
    else			/* print all words in synset */
	for(i=0, wdcnt = synptr->wcount; i < wdcnt; i++) {
	    printword(tbuf, synptr, i, PRINT_MARKER, antflag);
	    if (i < wdcnt - 1)
		strcat(tbuf, ", ");
	}
    
    if(definition && dflag && synptr->defn) {
	strcat(tbuf," -- ");
	strcat(tbuf,synptr->defn);
    }
    
    strcat(tbuf,tail);
    printbuffer(tbuf);
}

static void printantsynset(synptr, tail, anttype, definition)
char *tail;
SynsetPtr synptr;
int anttype, definition;
{
    register int i, wdcnt;
    char *str;
    char tbuf[SMLINEBUF];
    int first = 1;
    
    sprintf(tbuf,"");
    
    if((str = getenv("WNDEBUG")) && *str != '\0') {
	sprintf(tbuf,"<%s>",
		(wn_filenames[getpos(synptr->pos)])[synptr->fnum]);
	prsense = 1;
    } else
	prsense = 0;
    
    /* print anotnyms from cluster head (of indirect ant) */
    
    strcat(tbuf, "INDIRECT (VIA ");
    for(i=0, wdcnt = synptr->wcount; i < wdcnt; i++) {
	if (first) {
	    str = printant(ADJ, synptr, i + 1, "%s", ", ");
	    first = 0;
	} else
	    str = printant(ADJ, synptr, i + 1, ", %s", ", ");
	if (*str)
	    strcat(tbuf, str);
    }
    strcat(tbuf, ") -> ");
    
    /* now print synonyms from cluster head (of indirect ant) */
    
    for (i = 0, wdcnt = synptr->wcount; i < wdcnt; i++) {
	printword(tbuf, synptr, i, SKIP_MARKER, SKIP_ANTS);
	if (i < wdcnt - 1)
	    strcat(tbuf, ", ");
    }
    
    if(dflag && synptr->defn && definition) {
	strcat(tbuf," -- ");
	strcat(tbuf,synptr->defn);
    }
    
    strcat(tbuf,tail);
    printbuffer(tbuf);
}

static void printword(buf, synptr, wdnum, adjmarker, antflag)
char *buf;
SynsetPtr synptr;
int wdnum, adjmarker, antflag;
{
    int pos;
    static char vs[] = " (vs. %s)";

    /* copy the word (since deadjify() changes original string),
       deadjify() the copy and append to buffer.  then append
       adjective marker string if there is one. */
    
    strcpy(wdbuf, synptr->words[wdnum]);
    strcat(buf, deadjify(wdbuf));
	
    /* if debug is set, add WordNet sense number if non-0 */
	
    if (prsense && (synptr->wnsense[wdnum] != 0))
	sprintf(buf + strlen(buf), "%d", synptr->wnsense[wdnum]);
	
    pos = getpos(synptr->pos);

    /* append adjective marker if present */

    if (pos == ADJ) {
	if (adjmarker)
	    if (adj_marker != UNKNOWN_MARKER) /* set in deadjify() */
		strcat(buf, markers[adj_marker]);
	if (antflag == PRINT_ANTS)
	    strcat(buf,
		   printant(getpos(synptr->pos), synptr, wdnum + 1, vs, ""));
    }
}

static char *printant(dbase, synptr, wdnum, template, tail)
int dbase;
SynsetPtr synptr;
int wdnum;
char *template, *tail;
{
    register int i, j, wdoff;
    register SynsetPtr psynptr;
    char tbuf[WORDBUF];
    static char retbuf[SMLINEBUF];
    int first = 1;
    
    retbuf[0] = '\0';
    
    /* Go through all the pointers looking for anotnyms from the word
       indicated by wdnum.  When found, print all the antonym's
       antonym pointers which point back to wdnum. */
    
    for (i = 0; i < synptr->ptrcount; i++) {
	if (synptr->ptrtyp[i] == ANTPTR && synptr->pfrm[i] == wdnum) {

	    psynptr = read_synset(dbase, synptr->ptroff[i], "");

	    for (j = 0; j < psynptr->ptrcount; j++) {
		if (psynptr->ptrtyp[j] == ANTPTR &&
		    psynptr->pto[j] == wdnum &&
		    psynptr->ptroff[j] == synptr->hereiam) {

		    wdoff = (psynptr->pfrm[j]) ? psynptr->pfrm[j] - 1 : 0;
		    strcpy(wdbuf, psynptr->words[wdoff]);
		    if (prsense && (psynptr->wnsense[wdoff] != 0))
			sprintf(tbuf, "%s%d", deadjify(wdbuf),
				psynptr->wnsense[wdoff]);
		    else
			sprintf(tbuf, "%s", deadjify(wdbuf));

		    if (!first)
			strcat(retbuf, tail);
		    else
			first = 0;
		    sprintf(retbuf + strlen(retbuf), template, tbuf);
		}
	    }
	    free_synset(psynptr);
	}
    }
    return(retbuf);
}

static void printbuffer(string)
char *string;
{
    register long delta;

    if (overflag)
	return;
    delta = (searchbufsize - strlen(searchbuffer)) - strlen(string);
    if(delta <= 0) {
	delta = -delta;
	if ((searchbufsize + (delta + SEARCHBUF)) > MAXSEARCHBUF) {
	    overflag = 1;
	    return;
	}
	searchbufsize += (delta + SEARCHBUF);
#ifdef MSDOS
	GlobalUnlock(hsearchbuffer);
	hsearchbuffer = GlobalReAlloc(hsearchbuffer, searchbufsize, GMEM_MOVEABLE);
	searchbuffer = (char *) GlobalLock(hsearchbuffer);
#else
	searchbuffer = realloc(searchbuffer, searchbufsize);
#endif
	assert(searchbuffer);
    }
    strcat(searchbuffer,string);
}

static void printsense(synptr)
SynsetPtr synptr;
{
    sprintf(tmpbuf,"\nSense %d", sense + 1);

    /* Append lexicographer filename after Sense # if flag is set. */

    if (fnflag)
	sprintf(tmpbuf + strlen(tmpbuf)," in file \"%s\"",
		(wn_filenames[getpos(synptr->pos)])[synptr->fnum]);
    strcat(tmpbuf, "\n");
    printbuffer(tmpbuf);
    OutSenseCount++;
}

static void printspaces(trace, depth)
int trace, depth;
{
    int j;

    for (j = 0; j < depth; j++)
	printbuffer("    ");

    switch(trace) {
    case TRACEP:		/* traceptrs() */
	if (depth)
	    printbuffer("   ");
	else
	    printbuffer("       ");
	break;

    case TRACEC:		/* tracecoords() */
	if (!depth)
	    printbuffer("    ");
	break;

    case TRACEI:			/* traceinherit() */
	if (!depth)
	    printbuffer("\n    ");
	break;
    }
}

/*
  Revision log:
  
  $Log:	search.c,v $
 * Revision 1.74  93/07/29  13:46:08  wn
 * *** empty log message ***
 * 
 * Revision 1.73  93/07/29  13:41:56  wn
 * *** empty log message ***
 * 
 * Revision 1.72  93/07/28  16:32:32  wn
 * added re_wninit
 * 
 * Revision 1.71  93/07/08  15:44:03  wn
 * cleanups for 1.4
 * 
 * Revision 1.70  93/07/01  17:10:27  wn
 * removed all exit() statements - converted to return(NULL)
 * 
 * Revision 1.69  93/06/09  14:53:51  wn
 * changed wninit to open fps, wrote in_wn()
 * 
 * Revision 1.68  93/06/08  15:23:06  wn
 * added code to getindex to strip '.' from strings before lookup
 * 
 * Revision 1.67  93/06/08  14:31:36  wn
 * no changes
 * 
 * Revision 1.66  93/04/22  11:58:51  wn
 * no changes
 * 
 * Revision 1.65  93/04/13  13:47:57  wn
 * made free code global
 * 
 * Revision 1.64  93/04/01  10:52:56  wn
 * added MACH #ifdefs and changed free_synset() call to free_syns()
 * in free_synset() ala marti hearst
 * 
 * Revision 1.63  93/03/02  16:22:02  wn
 * fixed bug in read_synset
 * 
 * Revision 1.62  93/01/26  13:55:52  wn
 * make getindex external instead of static
 * 
 * 
 */

