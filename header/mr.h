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
#include <assert.h>

#ifndef MR_HEADER
#define MR_HEADER

#ifndef TRUE
#define	TRUE	1
#endif
#ifndef	FALSE
#define	FALSE	0
#endif

#define	MARC_OK	1
#define MARC_ERROR	0

#define	TAGLEN		3	/* tag length	= 3		*/
#define	INDLEN		2	/* ind length 	= 2		*/ 

#define DIRLEN		12	/* directory entry length	*/
#define HEADERLEN	24	/* marc record header length	*/

#define	RT		'\035' 	/* record terminator		*/
#define FT		'\036'	/* field terminator		*/
#define SF		'\037'	/* subfield beginner		*/

#ifndef BOOK
#define	BOOK		0	/* book marc format		*/
#endif
#define SERIAL		2

#ifndef AUTHORITY
#define AUTHORITY	1
#endif

#define COMPUTER	5
#define ARCHIVE		6
#define MAP		7
#define	MUSIC		8
#define VISUAL		9 

/* 
	FIELD 008 definition	
 */
struct	field_008
{
	char	catdate[6];	/* catalog date			*/
	char	datetype;	/* date type			*/
	char	begindate[4];	/* beginning date of publicat	*/
	char	enddate[4];	/* end date of publication	*/
	char	pubplace[3];	/* place of publication		*/
	char	illustration[4];/* illustrations		*/
	char	targetaud;	/* target audience		*/
	char	formitem;	/* form of item			*/
	char	contnature[4];	/* content nature		*/
	char	govtpub;	/* government publication	*/
	char	conference;	/* conference			*/
	char	festschrift;	/* festschrift			*/
	char	index;		/* index			*/
	char	mainentry;	/* main entry			*/
	char	fiction;	/* fiction			*/
	char	biography;	/* biography			*/
	char	language[3];	/* language			*/
	char	modifiedrec;	/* modified record		*/
	char 	catsource;	/* catalog source		*/
}; 

struct	marc_leader
{
	char	reclen[5];	/* record length		*/
	char	recstatus;	/* record status		*/
	char	rectype;	/* record type			*/
	char	biblevel;	/* biblio. level		*/
	char	res1[4];	/* 				*/
	char	baseaddr[5];	/* base address			*/
	char	encodinglevel;	/* encoding level		*/
	char	desccatform;	/* descriptive cat form		*/
	char	res2[5];	/*				*/
}; 

char	MARCBUF[MARCBUFLEN];

#include "mrinter.h"

#endif	

