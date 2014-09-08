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
*	Function Name:	fil_add.c
*
*	Programmer: Helene Jaillet, Ray Larson
*
*	Purpose: Adds a record to a main sgml file; updates
*                  appropriate associator file; updates appropriate
*                  index file.
*
*	Usage:    new_record_num = fil_add(input, filename)
*
*	Variables: input is the sgml record as a string buffer;
*                  filename is the nickname or full path of the file
*                  to which this record is to be added.
*
*                  The history file tracks updates and changes to the
*                  main data file, so the history file is also updated.
*                  This routine does NOT make a call to in_kwadd function 
*                  to update index.
*                  Makes calls to cf_open to get file pointers.
*
*
*	Return values and return codes:
*                  returns the new record ID number on successful
*                  completion; or 0 (FAIL) on error.
*
*       Date:     11/19/93
*       Revised:  8/30/93 -- converted for sgml usage
*       Version:  1.0
*
* Copyright (c) 1994 The Regents of the University of California.
* All rights reserved.
*
* Author:	Ray Larson, ray@sherlock.berkeley.edu
*		Jerome McDonough jmcd@lucien.berkeley.edu
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


#include <cheshire.h>
#include <dmalloc.h>

int fil_hist();

int fil_add(char *indata, char *filename) 
{
  FILE *afile;        /* pointer to associator file */
  FILE *sgmlfile;     /* pointer to sgml file */

  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int slength;       /* length of sgml record */		
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
                      /* added by this function and passed to index */
		      /* function. Also the value of the 0th record */
		      /* in assocfile to keep track of highest      */
		      /* logical recno in use.                      */
  int i = 0;

  /* call function to get assocfile pointer */
  afile = cf_open(filename, ASSOCFILE);
  if (afile == NULL) {
    fprintf(LOGFILE, "Could not open associator file in fil_add\n");
    return(FAIL);
  }

  /* get new highest logical record number */
  fseek(afile, 0, 0);
  fread(&high_recno,sizeof(int),1, afile); 
  recno = high_recno + 1;

  /* call function to get sgmlfile pointer */
  sgmlfile = cf_open_cont(filename, MAINFILE, recno);
  if (sgmlfile == NULL) {
    fprintf(LOGFILE, "Could not open main file in fil_add\n");
      return(FAIL);
  }

  /* append to the end of the data file */
  fseek(sgmlfile, 0, 2);
  assocrec.offset = offset = ftell(sgmlfile);
  fprintf(sgmlfile, "%s\n", indata);

  /* record offset and length of sgml record in assoc file */
  assocrec.recsize = slength = strlen(indata);
  fseek(afile,recno*sizeof(assoc_rec), 0);
  fwrite(&assocrec, sizeof(assoc_rec), 1, afile);

  /* increment the zeroth record to record the */
  /* highest record number now in use          */
  fseek(afile, 0, 0);
  fwrite(&recno, sizeof(int), 1, afile);

  /* update history file */
  if (fil_hist(H_ADD, offset, recno, slength, filename) == FAIL) {
    fprintf(LOGFILE, "Could not update history file in fil_add\n");
    return(FAIL);
  }

  /* make sure the writes are done */
  fflush(sgmlfile);
  fflush(afile);

  return(recno);
}

