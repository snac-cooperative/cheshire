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
*	Function Name:	fil_read.c
*
*	Programmer: Ray Larson, Jerome McDonough
*
*	Purpose: Reads a record from a main sgml file 
*            into a buffer and returns
*            the length of the record - also appends a NULL to the
*            end the buffer.
*            Returns 0 at end-of-file
*
*	Usage:   reclength =  fil_read(filename,record,assocrec)
*
*	Variables:
*              filename is the nickname or full path of the file
*                  from which this record is to be read.
*              record is the buffer to read the record into.
*                  Be warned: expects record to be already malloc'ed.
*              assocrec is a pointer to an associator record structure
*                  that will be filled in with the read record's file
*                  offset and record length.
*
*                  Makes calls to cf_open to get file pointers.
*
*
*	Return values and return codes:
*                  returns the record length on successful
*                  completion; or 0 (FAIL) on error.
*
*       Date:     11/19/93
*       Revised:  8/30/93 -- converted for sgml usage
*       Version:  1.0
*
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
***************************************************************************/

#include "cheshire.h"
#include "dmalloc.h"


int fil_read(filename, record_buffer, assocrec, recordid)
char *filename;
char *record_buffer; /* input record buffer */
assoc_rec *assocrec; /* offset and record size*/
int recordid;
{
  int reclen = 0;
  int seekreturn;

  FILE *file;

  if (record_buffer == NULL || assocrec == NULL) {
    fprintf(LOGFILE,"NULL record or assocrec pointers in fil_read\n");
    return (FAIL);
  }

  /* call cf functions to get pointer to main file */
  file = cf_open_cont(filename, MAINFILE, recordid);
  if (file == NULL) {
    return(FAIL);
  }
     
  /* if we get to here the record number must be OK */
  seekreturn = fseek(file, assocrec->offset, SEEK_SET);
  if (seekreturn == -1) {
#ifndef WIN32
    if (errno == EBADF)
      fprintf(LOGFILE,"unable to use SGML file %s\n",filename);
    else if (errno == EINVAL)
      fprintf(LOGFILE,"invalid seek value in fil_read\n");
    else
      fprintf(LOGFILE,"file error in fil_read\n");
#else
      fprintf(LOGFILE,"File error in fil_read- Invalid seek?\n");
#endif
    return (FAIL);
  }

  if ((reclen = fread(record_buffer, sizeof(char), 
		      assocrec->recsize, file)) == 0)
    {
      fprintf(LOGFILE, "fil_read: couldn't read record at offset %d\n",
	      assocrec->offset);
      return(FAIL);
    }
  else {

    /* record_buffer[assocrec->recsize] = 0; */
    return(assocrec->recsize);
  }    
}

