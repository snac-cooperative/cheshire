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
/*****************************************************************************
*
*	Function Name:	fil_hist.c
*
*	Programmers: Helene Jaillet & Lucy Kuntz & Ray Larson
*
*	Purpose:  This routine updates the history file that
*                 tracks addition, deletion and modification
*                 of records in the database. It can be used
*                 to recover to previous versions of the database.
*
*	Usage:   fil_hist(type, offset, recno, reclen, filename)
*
*	Variables: 
*                type is the type of transaction H_ADD, H_DELETE,
*                H_REPLACE or H_VACUUM.
*                offset is the address of the record in the marc
*                file; recno is the logical record number of the
*                marc record to be deleted or replaced; reclen is
*                the record length of the record affected. filename is
*                the full path or nickname of the file where this
*                record is being deleted or edited.
*
*                Makes call to cf_open function to get file pointer.
*
*
*	Return values and return codes:
*                returns 1 (SUCCEED) upon successful
*                completion or 0 (FAIL) on error.
*
*	Date:     11/19/93
*       Revised:  8/11/94  added second long to associator
*                 8/30/94  changed completely to sequential log. RRL
*       Version:  1.0
*
*
* Copyright (c) 1994 The Regents of the University of California.
* All rights reserved.
*
* Author:	Ray Larson, ray@sherlock.berkeley.edu
*               Jerome McDonough jmcd@lucien.berkeley.edu
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
*****************************************************************************/

#include "cheshire.h"
#include "dmalloc.h"

int fil_hist(int type, int offset, int recno, int reclen, char *filename) {
  FILE *histfile;
  history_file_info hist;

  /* get pointer to file histfile */
  histfile = cf_open(filename, HISTORYFILE);
  if (histfile == NULL) {
    return(FAIL);
  }
	
  /* fill in the history record structure */
  hist.recno = recno;
  hist.reclen = reclen;
  hist.offset = offset;
  hist.transaction = type;
  time(&hist.time);
  
  /* go to the end of the history file and write the record */
  fseek(histfile, 0, 2);
  fwrite(&hist, sizeof(history_file_info), 1, histfile); 

  fflush(histfile);

  return(SUCCEED);
}

