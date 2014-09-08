/*

  wnglobals.h

*/

/* $Id: wnglobals.h,v 1.15 93/06/09 14:57:27 wn Exp $ */

#ifndef _WNGLOBALS_
#define _WNGLOBALS_

#include <stdio.h>

extern char *wnrelease;
extern char *ptrtyp[];
extern char *partnames[];
extern char partchars[];
extern char *adjclass[];
extern char *frametext[];

/* Global variables */

extern int SenseCount;		/* number of senses word has */
extern int OutSenseCount;	/* number of senses search found */
extern int fnflag;		/* if set, print lex filename after sense */
extern int dflag;		/* if set, print definitional glosses */
extern int saflag;		/* if set, print SEE ALSO pointers */
extern int frflag;		/* if set, print verb frames after synset */
extern char **wn_filenames[NUMPARTS + 1];/* lex files ground in database */
extern FILE *datafps[NUMPARTS + 1], *indexfps[NUMPARTS + 1], *sensefp;

#endif /* _WNGLOBALS_ */

/*
  Revision log:

  $Log:	wnglobals.h,v $
 * Revision 1.15  93/06/09  14:57:27  wn
 * *** empty log message ***
 * 
 * Revision 1.14  93/06/09  14:52:24  wn
 * added file pointers
 * 
 * Revision 1.13  93/04/21  11:18:56  wn
 * no changes
 * 
 * Revision 1.12  93/03/11  16:37:07  wn
 * add #ifdefs to protect against multiple inclusions
 * 
 * Revision 1.11  92/11/17  14:52:09  wn
 * removed helptext
 * 
 * Revision 1.10  92/11/12  16:19:11  wn
 * cleanups for release 1.3 - merged change required for mac and next ports
 * 
 * Revision 1.9  92/10/15  13:16:35  wn
 * added helptext
 * 
 * Revision 1.8  92/09/09  13:27:16  wn
 * *** empty log message ***
 * 
 * Revision 1.7  92/03/26  14:46:06  wn
 * removed search_cmd
 * 
 * Revision 1.6  92/03/19  11:47:29  wn
 * added search_cmd
 * 
 * Revision 1.5  91/12/18  11:52:41  wn
 * no changes
 * 
 * Revision 1.4  91/12/13  15:21:45  wn
 * changed name from enums.h
 * removed isdef and freqcats.  added adjclass
 * 
 * Revision 1.3  91/12/10  16:24:09  wn
 * removed nfnames, vfnames, afnames static structures
 * 
 * Revision 1.2  91/12/04  10:38:27  wn
 * added partchars
 * 
 * Revision 1.1  91/09/25  15:39:14  wn
 * Initial revision
 * 
*/

