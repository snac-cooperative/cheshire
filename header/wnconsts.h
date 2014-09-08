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

  wnconsts.h - constants and functions used by all parts of WordNet system

*/

/* $Id: wnconsts.h,v 1.34 93/07/29 13:42:04 wn Exp $ */

#ifndef _WNCONSTS
#define _WNCONSTS

/* This is a hack to define MAC on the Macintosh and to make 'getenv' work*/
#if (THINK_C == 5)
#define MAC
#endif

#ifdef unix
#define HASGREP
#endif

/* OS specific path and filename specifications */

#ifdef MSDOS
#define DEFAULTPATH	"c:\\wordnet"
#define DATAFILE	"%s\\%s.dat"
#define INDEXFILE	"%s\\%s.idx"
#define SENSEIDXFILE	"%s\\index.sense"
#else
#ifdef MAC
#define DEFAULTPATH     "Hard Disk:Database"
#define DATAFILE	"%s:data.%s"
#define INDEXFILE	"%s:index.%s"
#define SENSEIDXFILE	"%s:index.sense"
#else
#define DEFAULTPATH	"/usr/local/lib/wordnet"
#define DATAFILE	"%s/data.%s"
#define INDEXFILE	"%s/index.%s"
#define SENSEIDXFILE	"%s/index.sense"
#endif
#endif

/* Buffer sizes */

#define SEARCHBUF	8*1024	/* initial size of search buffer returned
				   and size to increment it by */
#ifdef MSDOS
#define MAXSEARCHBUF	32768
#else
#ifdef MAC
#define MAXSEARCHBUF	32*1024
#else
#define MAXSEARCHBUF	200*1024
#endif
#endif
#define LINEBUF		15*1024	/* 15K buffer to read in index & data files */
#define SMLINEBUF	1*1024	/* small buffer for output lines */
#define WORDBUF		256	/* buffer for one word or collocation */

#define ALLSENSES	0	/* pass to findtheinfo() if want all senses */
#define MAXSENSES	16	/* maximum number of senses */
#define MAXDEPTH	20	/* maximum tree depth - used to find cycles */

/* Pointer type and search type counts */

#define MAXPTR		18
#ifdef HASGREP
#define MAXSEARCH       25
#else
#define MAXSEARCH	24
#endif

/* Pointers */

#define ANTPTR           1	/* ! */
#define HYPERPTR         2	/* @ */
#define HYPOPTR          3	/* ~ */
#define ENTAILPTR        4	/* * */
#define SIMPTR           5	/* & */

#define ISMEMBERPTR      6	/* %m */
#define ISSTUFFPTR       7	/* %s */
#define ISPARTPTR        8	/* %p */

#define HASMEMBERPTR     9	/* #m */
#define HASSTUFFPTR     10	/* #s */
#define HASPARTPTR      11	/* #p */

#define MERONYM         12	/* # (not valid in lexicographer file) */
#define HOLONYM         13	/* % (not valid in lexicographer file) */
#define CAUSETO         14	/* > */
#define CAUSEDBY        15	/* not used (<) */
#define SEEALSOPTR	16	/* ^ */
#define PERTPTR		17	/* \ */
#define ATTRIBUTE	18	/* = */

#define SYNS            (MAXPTR + 1)
#define FREQ            (MAXPTR + 2)
#define FRAMES          (MAXPTR + 3)
#define COORDS          (MAXPTR + 4)
#define HMERONYM        (MAXPTR + 5)
#define HHOLONYM	(MAXPTR + 6)
#ifdef HASGREP
#define WNGREP		(MAXPTR + 7)
#endif

/* WordNet part of speech stuff */

#define NUMPARTS	4	/* number of parts of speech */
#define NUMFRAMES	35	/* number of verb frames */

/* Generic names for part of speech */

#define NOUN		1
#define VERB		2
#define ADJ		3
#define ADV		4
#define SATELLITE	5	/* not really a part of speech */
#define ADJSAT	SATELLITE

#define NOUNSS          NOUN
#define VERBSS          VERB
#define ADJSS           ADJ
#define ADVSS		ADV
#define FANSS		SATELLITE
#define SATELLITESS	SATELLITE

#define ALL_POS		0	/* passed to in_wn() to check all POS */

#define bit(n) ((unsigned int)((unsigned int)1<<((unsigned int)n)))

/* Adjective markers */

#define PADJ		1	/* (p) */
#define NPADJ		2	/* (n) */
#define IPADJ		3	/* (ip) */

#define UNKNOWN_MARKER		0
#define ATTRIBUTIVE		PADJ
#define IMMED_POSTNOMINAL	NPADJ
#define PREDICATIVE		IPADJ

#endif				/* _WNCONSTS */

/*
  Revision log:

  $Log:	wnconsts.h,v $
 * Revision 1.34  93/07/29  13:42:04  wn
 * *** empty log message ***
 * 
 * Revision 1.33  93/07/06  15:32:33  wn
 * increased linebuf, added maxdepth
 * 
 * Revision 1.32  93/06/09  14:53:17  wn
 * added ALL_POS
 * 
 * Revision 1.31  93/04/21  12:36:25  wn
 * changed ATTRIBUTES to ATTRIBUTE
 * 
 * Revision 1.30  93/04/21  11:19:11  wn
 * added new_var ptr ATTRIBUTES (=)
 * 
 * Revision 1.29  93/04/21  11:08:36  wn
 * added MAXSENSES
 * 
 * Revision 1.28  93/04/06  11:26:21  wn
 * added another verb frame (change NUMFRAMES)
 * 
 * Revision 1.27  93/02/11  15:59:24  wn
 * added ADJSAT as another name for SATELLITE
 * 
 * Revision 1.26  92/11/06  13:39:16  wn
 * added one new_var frame.  updated old frames
 * 
 * Revision 1.25  92/09/09  12:18:53  wn
 * change MAXSEARCHBUF on sun
 * 
 * Revision 1.24  92/09/08  14:58:00  wn
 * *** empty log message ***
 * 
 * Revision 1.23  92/09/01  16:22:27  wn
 * *** empty log message ***
 * 
 * Revision 1.22  92/09/01  16:17:55  wn
 * changed SS defines
 * 
 * Revision 1.21  92/09/01  10:08:59  wn
 * added ALLSENSES
 * 
 * Revision 1.20  92/08/27  09:07:47  wn
 * added MAXSEARCHBUF
 * 
 * Revision 1.19  92/08/19  16:10:32  wn
 * added adverbs
 * 
 * Revision 1.17  92/08/11  11:12:02  wn
 * changed #if(n)def PC 's to MSDOS
 * 
 * Revision 1.16  92/06/17  16:02:33  wn
 * added WNGREP
 * 
 * Revision 1.15  92/06/16  16:37:40  wn
 * *** empty log message ***
 * 
 * Revision 1.14  92/05/28  14:14:02  wn
 * addd casts to bit and added Mac DEFAULTPATH
 * 
 * Revision 1.13  92/03/26  14:45:43  wn
 * removed search_cmd
 * 
 * Revision 1.12  92/03/26  12:59:17  wn
 * removed unused pointers - V2NSPTR, N2VSPTR, ASPECT*PTR
 * 
 * Revision 1.11  92/03/24  14:56:33  wn
 * removed BRACKET
 * 
 * Revision 1.10  92/03/04  15:00:59  wn
 * added HHOLONYM search
 * 
 * Revision 1.9  92/02/25  14:39:58  wn
 * added DEFAULTPATH
 * 
 * Revision 1.8  92/02/05  11:06:21  wn
 * added defines for NOUN VERB ADJ
 * 
 * Revision 1.7  91/12/18  11:55:40  wn
 * removed function definitions (moved to search.h)
 * 
 * Revision 1.6  91/12/13  15:24:34  wn
 * renamed from find.h
 * removed things that were specific to search code and added some
 * general things used by all parts of wn software`
 * 

*/

