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
*	Function Name:	fil_replace.c
*
*	Programmer: Helene Jaillet, Ray Larson
*
*	Purpose: replaces a record from an sgml file by
*                   first adding the record to the end of the file,
*                   recording its old offset in a history file, then
*                   updating its new offset in the associator file.
*
*	Usage:  fil_replace(input, recno, filename)
*
*	Variables: input is the sgml record as a string;
*                    recno is the logical record number of the sgml
*                    record to be deleted; filename is the full path
*                    or nickname of the file where the record is being
*                    replaced.
*
*                    Makes a call to fil_hist function to write the old
*                    offset of the record prior to updating it in the
*                    associator file.
*
*
*	Return values and return codes:
*                    returns 1 (SUCCEED) upon successful
*                    completion or 0 (FAIL) on error.
*
*	Date:     11/19/93
*       Revised:  8/30/94 -- changed to "replace" and sgml usage RRL
*       Version:  2.0
*
* Copyright (c) 1994 The Regents of the University of California.
* All rights reserved.
*
* Author:	Ray Larson, ray@sherlock.berkeley.edu
		Jerome McDonough jmcd@lucien.berkeley.edu
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

int fil_hist(int type, int offset, int recno, int reclen, char *filename);

int fil_replace(char *input, int recno, char *filename) {
  FILE *afile;
  FILE *sgmlfile;
  assoc_rec assocrec;

  if (input == NULL) 
    return (FAIL);

  /* call cf_open to get assocfile pointer */
  afile = cf_open(filename, ASSOCFILE);
  if (afile == NULL) {
    return(FAIL);
  }

  /* get record's current offset; write it to the history file */
  fseek(afile, (recno*sizeof(assoc_rec)), 0);
  fread(&assocrec, sizeof(assoc_rec), 1, afile);
  
  /* if history file can't be updated, stop and return FAIL */
  if (!(fil_hist(H_REPLACE, assocrec.offset, recno, 
		 assocrec.recsize, filename))) {
    return(FAIL);
  }

  /* call function to get sgmlfile pointer */
  sgmlfile = cf_open_cont(filename, MAINFILE, recno);
  if (sgmlfile == NULL) {
    return(FAIL);
  }

  /* get the new offset and write it to the associator file */
  fseek(sgmlfile, 0, 2);
  assocrec.offset = (ftell(sgmlfile));
  assocrec.recsize = strlen(input);
  fseek(afile, (recno*sizeof(assoc_rec)), 0);
  fwrite(&assocrec, sizeof(assoc_rec), 1, afile);
  fflush(afile);

  /* write the record to the marc file */

  fprintf(sgmlfile,"%s\n",input);
  fflush(sgmlfile);
  return(SUCCEED);
}


