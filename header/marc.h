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
 * Copyright (c) 1992 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
 *		School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef MARC_H
#define MARC_H

#include <dmalloc.h>

/*********************************************************************/
/* special char definitions  for MARC records                        */
/*********************************************************************/
#define SUBFDELIM '\037'
#define FIELDTERM '\036'
#define RECTERM   '\035'

/*********************************************************************/
/* Structures for internal processing format of MARC records         */
/*********************************************************************/
typedef struct marc_leader_over /* overlay for marc leader */
 { char LRECL[5];    /* Logical Record Length            */
   char RecStatus;   /* Record Status                    */
   char RecType;     /* Legend - type of record          */
   char BibLevel;    /* Legend - Bibliographic Level     */
   char blanks1[2];
   char IndCount;    /* Indicator count                  */
   char SubFCount;   /* Subfield code count              */
   char BaseAddr[5]; /* Base Address of data             */
   char EncLevel;    /* encoding level                   */
   char DesCatForm;  /* Descriptive Cataloging Form      */
   char blank2;      
   /* Entry Map - description of directory fields */
   char LenLenF;     /* length of length of field        */
   char LenStartF;   /* length of Starting char position */
   char UnDef[2];    /* undefined chars                  */
  } MARC_LEADER_OVER;

typedef struct marc_direntry_over  /* overlay for directory entries */
  { char tag[3];     /* field tag           */
    char flen[4];    /* field length        */
    char fstart[5];  /* field start position*/
  } MARC_DIRENTRY_OVER;

typedef struct marc_008_over /* overlay for 008 fixed field */
  { char entry_date[6];
    char date_type;
    char dates[8];
    char country_code[3];
    char illus_code[4];
    char intellectual_level;
    char form_of_reproduction;
    char nature_of_contents[4];
    char government_pub_code;
    char conference_indicator;
    char festschrift_indicator;
    char index_indicator;
    char main_entry_in_body;
    char fiction_indicator;
    char biography_indicator;
    char language_code[3];
    char modified_record_code;
    char cataloging_source;

  } MARC_008_OVER;

typedef struct marc_subfield  /* processing structure for subfields */
  { char *data;                  /* pointer to data */
    struct marc_subfield  *next; /* pointer to next subfield */
    char code;                   /* subfield code char */
  } MARC_SUBFIELD;
   
typedef struct marc_field  /* field linked list node */
  { char *data;                /* pointer to start of field data */
    MARC_SUBFIELD *subfield;   /* head of linked list of subfields */
    MARC_SUBFIELD *lastsub;    /* tail of linked list of subfields */
    struct marc_field *next;   /* next field in list */
    char tag[4];               /* 3 char tag + null */
    char indicator1;           /* first indicator   */
    char indicator2;           /* second indicator  */
    char subfcodes[256];        /* direct hash table of subfield codes */
                               /* one char subfield code is index to   */
                               /* element [0] is a count of subfields  */
    int  length; /* full field length */
  } MARC_FIELD;
    
typedef struct marc_rec  /* marc record processing format */
  { int length;
    char *record,
         *BaseAddr;
    MARC_LEADER_OVER *leader;
    char *fixed_fields;
    MARC_FIELD *fields;    /* head of linked list */
    MARC_FIELD *lastfield; /* tail of linked list */
    int nfields;
    int copy; /* is record allocated space ? */
   } MARC_REC;


#endif
