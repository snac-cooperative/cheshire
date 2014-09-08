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
*	Function Name:	fil_seek.c
*
*	Programmer: Ray Larson, Jerome McDonough
*
*	Purpose: retrieves an associator file record so that
*            the SGML file can be positioned to a given logical record number -
*            the associator file ONLY is opened needed.
*            returns -1 on seek errors and the SGML file position
*            when successful.                                    
*            assocrec must point to real memory or this will fail.
*
*	Usage:   reclength =  fil_seek(filename,record_num,assocrec)
*
*	Variables:
*              filename is the nickname or full path of the file
*                  from which this record is to be read.
*              record_num is the id number of the record to read.
*              assocrec is a pointer to an associator record structure
*                  that will be filled in with the read record's file
*                  offset and record length.
*
*                  Makes calls to cf_open to get file pointers.
*
*
*	Return values and return codes:
*                  returns the record position in the file on successful
*                  completion; or -1 on error.
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
****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "cheshire.h"
#include "dmalloc.h"
#include <errno.h>

extern int VerifyDataStore(char *filename, int docid, assoc_rec *assocrec);

int fil_seek (char *sgmlfilename, int recnumber, assoc_rec *assocrec)
{
  int seekreturn;

  FILE *afile;
  int MaxRecs;

  /* check for a NULL assoc_rec */
  if (assocrec == NULL) {
    fprintf(LOGFILE,"NULL associator record in fil_seek\n");
    return(-1);
  }

  if (cf_getfiletype(sgmlfilename) > 99) {
    return(VerifyDataStore(sgmlfilename, recnumber, assocrec));
  }

  /* call cf functions to get pointer to assocfile */
  afile = cf_open(sgmlfilename, ASSOCFILE);
  if (afile == NULL) {
    return(-1);
  }

  /* get the Maximum number of records in the file */
  fseek(afile, 0, 0);
  fread(&MaxRecs, sizeof(int), 1, afile);

  if (recnumber > MaxRecs) {
    fprintf(LOGFILE,"Bad SGML record number in SeekSGML - past end of file\n");
    return (-1);
  }

  if (recnumber > 0) { 
    seekreturn = fseek(afile, (recnumber*sizeof(assoc_rec)), 0);
    if (seekreturn == -1) {
#ifndef WIN32
      if (errno == EBADF)
	fprintf(LOGFILE,"unable to use associator file\n");
      else if (errno == EINVAL)
	fprintf(LOGFILE,"invalid seek value in SeekSGML\n");
      else
	fprintf(LOGFILE,"associator file error in SeekSGML\n");
#else
	fprintf(LOGFILE,"Associator file seek error in SeekSGML\n");
#endif
      return (-1);
    }
    else /* Seek was OK, so read the record into the assocfile struct */
      fread(assocrec, sizeof(assoc_rec), 1, afile);
  }
  else {
    fprintf(LOGFILE,"Bad SGML record number in fil_seek - %d\n",recnumber);
    return (-1);
  }
  /* return may be negative -- indicating deleted record number */
  /* or a seek error of some sort.                              */
  /* positive returns are the position in the data file of the  */
  /* start of the data record.                                  */
  return (assocrec->offset); 

}    





