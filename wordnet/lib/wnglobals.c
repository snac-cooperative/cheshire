/*
  
  wnglobals.c 

*/

/* $Id: wnglobals.c,v 1.26 93/08/04 12:24:37 wn Exp $ */

#include <stdio.h>
#include "wnconsts.h"

#ifndef NULL
#define NULL	0
#endif

char *wnrelease = "1.4";

char *ptrtyp[]={ "", "!", "@", "~", "*", "&", "#m", "#s", "#p", "%m", "%s",
		     "%p", "%", "#", ">", "", "^", "\\", "=",
/* Additional searches, but not pointers.  */
		     "", "", "+", "", "", "", 
#ifndef HASGREP
		     "", 
#endif
		     NULL};

char *partnames[]={ "", "noun", "verb", "adj", "adv", NULL};
char partchars[] = " nvara"; /* add char for satellites to end */
char *adjclass[] = { "", "(p)", "(a)", "(ip)" };

char *frametext[] = {
    "",
    "Something ----s",
    "Somebody ----s",
    "It is ----ing",
    "Something is ----ing PP",
    "Something ----s something Adjective/Noun",
    "Something ----s Adjective/Noun",
    "Somebody ----s Adjective",
    "Somebody ----s something",
    "Somebody ----s somebody",
    "Something ----s somebody",
    "Something ----s something",
    "Something ----s to somebody",
    "Somebody ----s on something",
    "Somebody ----s somebody something",
    "Somebody ----s something to somebody",
    "Somebody ----s something from somebody",
    "Somebody ----s somebody with something",
    "Somebody ----s somebody of something",
    "Somebody ----s something on somebody",
    "Somebody ----s somebody PP",
    "Somebody ----s something PP",
    "Somebody ----s PP",
    "Somebody's (body part) ----s",
    "Somebody ----s somebody to INFINITIVE",
    "Somebody ----s somebody INFINITIVE",
    "Somebody ----s that CLAUSE",
    "Somebody ----s to somebody",
    "Somebody ----s to INFINITIVE",
    "Somebody ----s whether INFINITIVE",
    "Somebody ----s somebody into V-ing something",
    "Somebody ----s something with something",
    "Somebody ----s INFINITIVE",
    "Somebody ----s VERB-ing",
    "It ----s that CLAUSE",
    "Something ----s INFINITIVE",
    ""
};

/* Search code variables */

int SenseCount = 0;		/* number of senses word has */
int OutSenseCount = 0;		/* number of senses search found */
int fnflag = 0;			/* if set, print lex filename after sense */
int dflag = 1;			/* if set, print definitional glosses */
int saflag = 1;			/* if set, print SEE ALSO pointers */
int frflag = 0;			/* if set, print verb frames */

/* array of database file names */
char **wn_filenames[NUMPARTS + 1] = { NULL, NULL, NULL, NULL, NULL };

/* File pointers for database files */
FILE *datafps[NUMPARTS + 1], *indexfps[NUMPARTS + 1], *sensefp;

/*
  Revision log:

  $Log:	wnglobals.c,v $
 * Revision 1.26  93/08/04  12:24:37  wn
 * changed text of frame 35
 * 
 * Revision 1.25  93/07/01  16:35:27  wn
 * updated wnrelease to 1.4
 * 
 * Revision 1.24  93/06/09  14:52:11  wn
 * added file pointers
 * 
 * Revision 1.23  93/04/21  11:19:22  wn
 * added new ptr ATTRIBUTES (=)
 * 
 * Revision 1.22  93/04/13  13:48:41  wn
 * added verb frame
 * 
 * Revision 1.21  92/11/17  14:51:50  wn
 * removed help strings.
 * 
 * Revision 1.20  92/11/12  16:19:08  wn
 * cleanups for release 1.3 - merged change required for mac and next ports
 * 
 * Revision 1.19  92/11/06  13:39:02  wn
 * added one new frame.  updated old frames
 * 
 * Revision 1.18  92/10/15  13:16:39  wn
 * added helptext
 * 
 * Revision 1.17  92/09/09  13:27:03  wn
 * added search code globals
 * 
 * Revision 1.16  92/09/08  09:46:48  wn
 * added wnrelease
 * 
 * Revision 1.15  92/08/19  15:23:00  wn
 * added adverbs to partnames and partchars
 * 
 * Revision 1.14  92/08/11  11:12:03  wn
 * changed #if(n)def PC 's to MSDOS
 * 
 * Revision 1.13  92/06/17  16:05:07  wn
 * added placeholder for WNGREP
 * 
 * Revision 1.12  92/05/28  14:13:51  wn
 * *** empty log message ***
 * 
 * Revision 1.11  92/03/26  14:46:01  wn
 * removed search_cmd
 * 
 * Revision 1.10  92/03/26  12:59:51  wn
 * removed unused pointers
 * 
 * Revision 1.9  92/03/24  14:56:03  wn
 * removed -brac(v|n)
 * 
 * Revision 1.8  92/03/19  11:47:14  wn
 * added search_cmd strings
 * 
 * Revision 1.7  91/12/18  11:52:31  wn
 * no changes
 * 
 * Revision 1.6  91/12/13  15:23:41  wn
 * changed name from enums.c
 * removed is_def and freqcats.  added adjclass
 * 
 * Revision 1.5  91/12/10  16:23:56  wn
 * removed nfnames, vfnames, afnames static structures
 * 
 * Revision 1.4  91/12/04  10:38:17  wn
 * added partchars
 * 
 * Revision 1.3  91/11/27  10:56:39  wn
 * added pertainym pointer
 * 
 * Revision 1.2  91/09/25  15:42:31  wn
 * defined NULL
 * 
 * Revision 1.1  91/09/25  15:38:08  wn
 * Initial revision
 * 
*/
