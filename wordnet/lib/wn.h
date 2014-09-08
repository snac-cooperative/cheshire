/*

  wn.h - header file for use with WordNet searching, morphology 
         and binary search functions

*/

/* $Id: wn.h,v 1.32 93/07/30 13:09:57 wn Exp $ */

#ifndef _WN_
#define _WN_

/*
  Data structures used by search code and returned by
  index_lookup(), read_synset() and findtheinfo_ds() 
*/

/* Structure for index file - returned from index_lookup() */

typedef struct {
    char *wd;			/* word string */
    char *pos;			/* part of speech */
    int sense_cnt;		/* sense (collins) count */
    int off_cnt;		/* number of offsets */
    long *offset;		/* offsets of synsets containing word */
    int ptruse_cnt;		/* number of pointers used */
    int *ptruse;		/* pointers used */
} Index, *IndexPtr;

/* Structure for data file synset - returned from read_synset()
   and findtheinfo_ds() */

typedef struct ss {
    long hereiam;		/* current file position */
    int sstype;			/* type of ADJ synset */
    int fnum;			/* file number that synset comes from */
    char *pos;			/* part of speech */
    int wcount;			/* number of words in synset */
    char **words;		/* words in synset */
    int *wnsense;		/* sense number in wordnet */
    int whichword;		/* which word in synset we're looking for */
    int ptrcount;		/* number of pointers */
    int *ptrtyp;		/* pointer types */
    long *ptroff;		/* pointer offsets */
    int *ppos;			/* pointer part of speech */
    int *pto;			/* pointer 'to' fields */
    int *pfrm;			/* pointer 'from' fields */
    int fcount;			/* number of verb frames */
    int *frmid;			/* frame numbers */
    int *frmto;			/* frame 'to' fields */
    char *defn;			/* synset gloss (definition) */

    /* these fields are used if a data structure is returned
       instead of a text buffer */

    struct ss *nextss;		/* ptr to next synset containing searchword */
    int searchtype;		/* type of search performed */
    struct ss *ptrlist;		/* ptr to synset list result of search */
    char *headword;		/* if pos is "s", this is cluster head word */
    short headsense;		/* sense number of headword */
} Synset, *SynsetPtr;

/* Search and database functions */

extern char *findtheinfo();	/* search algorithm */
extern SynsetPtr findtheinfo_ds(); /* search algorithm, returns data struct */
extern unsigned long is_defined(); /* set bits for each valid search on word */
extern unsigned int in_wn();	/* set bits for each pos word is in */
extern IndexPtr index_lookup(); /* find word in index file */
extern IndexPtr getindex();	/* 'smart' search of index file */
extern SynsetPtr read_synset();	/* read synset from data file */
extern void free_syns();	/* free chain of synsets from findtheinfo_ds */
extern void free_synset();	/* free a synset */
extern void free_index();	/* free an index structure */

/* Morphology functions */

extern char *morphword();	/* morphology function for a word */
extern char *morphstr();	/* morphology function for a string */

/* Utility functions */

extern void wninit();		/* initialize search code and wnfilenames */
extern void re_wninit();	/* reopen database and exception files */
extern void morphinit();	/* open exception list files */
extern void re_morphinit();	/* reopen exception list files */
extern char *addtofile();	/* place line in file */
extern int cntwords();		/* number of words in a string */
extern char *strtolower();	/* convert string to lower case & deadjify */
extern char *ToLowerCase();	/* convert string passed to lower case */
extern char *strsubst();	/* replace all occurrences of
				   'from' with 'to' in 'str' */
extern int getptrtype();	/* return pointer type for string passed */
extern int getpos();		/* return part of speech for string passed */
extern int getsstype();		/* return synset type for string passed */
extern char *FmtSynset();	/* format synset structure into char string */

/* Utility functions for converting between encoded and non-encoded
   senses and accessing database */

extern char *StrToSense();	/* convert string to encoded sense */
extern char *SenseToStr();	/* convert encoded sense to string */
extern char *LexFileName();	/* get name of lexicographers' file */
extern int LexFileNum();	/* get offset corresponding to lex file */
extern int StrToPos();		/* convert POS as string to constant */
extern SynsetPtr GetSynsetForSense(); /* return synset for sense */
extern long GetDataOffset();	/* read sense index and return byte offset */
extern int GetPolyCount();	/* get polysemy count for word (pass sense) */
extern char *GetWORD();		/* get word from sense string */
extern int GetPOS();		/* get constant for POS from sense string */
extern char *WNSnsToStr();	/* convert WordNet sense to string */
extern IndexPtr GetValidIndexPointer(); /* returns a valid idx pointer */
#ifdef NEXT
extern char *strdup();		/* duplicates a string (needed for NeXT) */
#endif

/* Binary search functions */

extern char *bin_search();	/* binary search routine */
extern void copyfile();		/* copy contents from one file to another */
extern char *replace_line();	/* replace a line in sorted file */
extern char *insert_line();	/* insert a line in sorted file */

#endif /*_WN_*/

/*
  Revision log:

  $Log:	wn.h,v $
 * Revision 1.32  93/07/30  13:09:57  wn
 * added strdup for NeXT
 * 
 * Revision 1.31  93/07/29  13:45:59  wn
 * *** empty log message ***
 * 
 * Revision 1.30  93/07/29  13:42:02  wn
 * *** empty log message ***
 * 
 * Revision 1.29  93/07/28  16:32:52  wn
 * added re_*init
 * 
 * Revision 1.28  93/06/09  14:52:58  wn
 * added in_wn
 * 
 * Revision 1.27  93/05/05  13:26:50  wn
 * 
 * 
 * Revision 1.26  93/05/04  13:39:41  wn
 * added insert_line, replace_likne
 * 
 * Revision 1.25  93/04/13  13:48:22  wn
 * added free_synset and free_index
 * 
 * 
 * Revision 1.24  93/03/11  16:35:15  wn
 * added #ifndef to protect from multiple inclusions
 * 
 * Revision 1.23  93/01/26  13:56:14  wn
 * added extern for getindex
 * 
 * Revision 1.22  92/09/09  13:26:45  wn
 * removed search code globals variables - moved to wnglobals.h

*/
