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
/************************************************************************
*
*	Header Name:	configfiles.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	header for configfile handling -- includes
*                       symbolic definitions used in the configfiles
*	Usage:		#include configfiles.h
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/10/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#ifndef _CONFIGFILE_H
#define _CONFIGFILE_H

/* Index type flags are defined below */
/* bits 0 1 2 are reserved for DB_BTREE, DB_HASH, and DB_RECNO  */
/* when both bits are on (=3) the "index" is an external DBMS.  */
/* Value 2 is for vector indexes that will include imbedded     */
/* vector summary information                                   */
#define DB_MASK   3
#define DB_BTREE_TYPE 0
#define DB_HASH_TYPE 1
#define VECTOR_TYPE (1<<1)

#define EXACTKEY  (1<<2)

#define KEYWORD   (1<<3)
#define WORDNET   (1<<4)
#define STEMS     (1<<5)
#define CLASSCLUS  (1<<6)

#define PROXIMITY (1<<7)

#define FLD008_KEY (1<<8)

#define EXTERNKEY (1<<9)

#define PAGEDINDEX (1<<10)

#define PRIMARYIGNORE  (1<<12)
#define PRIMARYREPLACE (1<<13)
#define PRIMARYKEY (PRIMARYIGNORE | PRIMARYREPLACE)


#define DB_DBMS_TYPE (1<<14)

/* the following are used for DBMS key mapping */
#define INTEGER_KEY (1<<15)
#define DECIMAL_KEY (1<<16)
#define FLOAT_KEY  (1<<17)

/* the following are special key extraction and indexing types */
#define URL_KEY    (1<<18)
#define FILENAME_KEY  (1<<19)

#define SNOWBALL_STEMS  (1<<20)
#define DATE_KEY     (1<<21)
#define DATE_RANGE_KEY (1<<22)

#define NORM_NOMAP  (1<<23)

#define COMPONENT_INDEX (1<<24)

#define NORM_DO_NOTHING (1<<25)

#define NORM_WITH_FREQ  (1<<26) 

/* recycling old primary key flag for GEOTEXT*/
#define GEOTEXT    (1<<11)
#define LAT_LONG_KEY (1<<27)
#define BOUNDING_BOX_KEY (1<<28)

#define BITMAPPED_TYPE (1<<29)

#define NORM_MIN (1<<30)

#define SSTEMS (1<<31)

/* The following bit patterns are used in idx->type2 - the expansion */

#define BITMAP_CODED_ELEMENT 1
#define BITMAP_CODED_ATTRIBUTE (1<<1)
#define NGRAMS (1<<2)


#define CONFIGBUFFERSIZE 10001

/* the following are for display options (maintaining entity refs */
#define KEEP_AMP 1
#define KEEP_LT  2
#define KEEP_GT  4
#define KEEP_ENT 16
#define KEEP_ALL (KEEP_AMP | KEEP_LT | KEEP_GT | KEEP_ENT)

#define COMPONENT_COMPRESS 1
#define COMPONENT_STORE    2

extern char cf_create_file_flags[4];
extern char cf_open_file_flags[4];

#endif

