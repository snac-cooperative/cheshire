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
*	Header Name:	getsgml.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Routines to retrieve and parse sgml records
*
*	Usage:		GetSGMLrec(infile, assocfile, dtd, recordnum, *recsize)
*
*	Variables:	infile -- (opened) file of SGML MARC records to 
*                                 translate
*                       assocfile -- optional associator file for the docs
*                       dtd -- file name for the DTD used 
*                       recordnum -- the SGML record to get from the file.
*                       
*
*	Return Conditions and Return Codes:	
*
*	Date:		11/02/94
*	Revised:	9/08/95 Ray R. Larson --
*                       removed config file and other cheshire dependencies
*                       for standalone version.
*
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
/* some standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>

#include "sgml.h"

#include "dmalloc.h"

#include "alpha.h"

/* Structure for associator files */
typedef struct assoc_rec {
  long offset;
  long recsize;
} assoc_rec;

/* The parsed DTD used for this run */
SGML_DTD *main_dtd = NULL;
FILE *main_sgml_file = NULL;

extern FILE *LOGFILE;

char *GetRawSGMLRec(FILE *infile, FILE *assocf, long recnum, long *recsize)
{
  static long lastrecnum = 0;
  static long maxrecords = 1;
  static char *scanbuffer = NULL;
  char *record;
  assoc_rec assocrec;
  int seekreturn;

  *recsize = 0;

  if (infile != NULL && assocf != NULL) /* have an associator */
    {
      if (lastrecnum == 0)
	{ /* get the Maximum number of records in the file */
	  fseek(assocf, 0, 0);
	  fread(&maxrecords, sizeof(long), 1, assocf);
	}
      lastrecnum = recnum;

      if (recnum > maxrecords) {
	fprintf(LOGFILE,
		"Bad SGML record number #%d - past end of file\n",
		recnum);
	return (NULL);
      }

      if (recnum > 0) 
	{ 
	  seekreturn = fseek(assocf, (recnum*sizeof(assoc_rec)), 0);
	  if (seekreturn == -1) 
	    {
	      if (errno == EBADF)
		fprintf(LOGFILE,"unable to use associator file\n");
	      else if (errno == EINVAL)
		fprintf(LOGFILE,"invalid seek value in associator\n");
	      else
		fprintf(LOGFILE,"associator file error in GetRawSGMLRec\n");
	      return (NULL);
	    }
	  else /* Seek was OK, so read the record into the assocfile struct */
	    fread(&assocrec, sizeof(assoc_rec), 1, assocf);
	}
      else 
	{
	  fprintf(LOGFILE,"Bad SGML record number - %d\n",recnum);
	  return (NULL);
	}
  
      if (assocrec.offset < 0) 
	{
	  return (NULL); /* indicating deleted record number */
	}
  
      /* if we get to here the record number must be OK */
      seekreturn = fseek(infile, assocrec.offset, 0);
      if (seekreturn == -1) 
	{
	  if (errno == EBADF)
	    fprintf(LOGFILE,"unable to use SGML file\n");
	  else if (errno == EINVAL)
	    fprintf(LOGFILE,"invalid seek value in SeekSGML\n");
	  else
	    fprintf(LOGFILE,"SGML file error in SeekSGML\n");
	  return (NULL);
	}
      else 
	{ /* found the record so allocate a buffer... */
	  if ((record = CALLOC(char, (assocrec.recsize+1))) == NULL) 
	    {
	      fprintf(LOGFILE,"Could not allocate record buffer in GetRawSGML\n");
	      return(NULL);
	    }
	  
	  if (fread(record, assocrec.recsize, 1, infile) == 0)
	    {
	      fprintf(LOGFILE, "Couldn't read record at offset %d\n",
		      assocrec.offset);
	      return(NULL);
	    }
	  else 
	    { /* send back the record and set the size */
	      *recsize = assocrec.recsize;
	      record[assocrec.recsize] = 0;
	      return(record);
	    }    
	}     
    }
  else if (assocf == NULL)
    {
      fprintf(LOGFILE,"not handling files without assoc yet\n");
      exit(1);
    }
  
}



SGML_Document *GetSGMLRec(FILE *infile, FILE *assocf, char *DTDname, 
			       long recnum, long *recsize)
{
char *recordbuffer;
SGML_Document *doc, *sgml_parse_document();
SGML_DTD *sgml_parse_dtd();

recordbuffer = GetRawSGMLRec(infile, assocf, recnum, recsize);

if (recordbuffer != NULL) 
  { /*  have a record */

    if (main_dtd == NULL) 
      {
	if ((main_dtd = sgml_parse_dtd(DTDname, NULL, NULL, NULL, 0)) == NULL) {
	  fprintf(LOGFILE, "couldn't get main dtd in GetSGML\n");
	  exit(1);
	}
	/* also set the global filename */
	main_sgml_file = infile;
      }
  
    doc = sgml_parse_document(main_dtd, "dummyname", recordbuffer, recnum);

    return(doc);
  }
else /* no record */
  return (NULL);

}


/* The following are dummy routines to remove dependencies on      */
/* stuff created for the Cheshire II project that is useless here  */

SGML_DTD *cf_getDTD(char *filename) 
{
  return (main_dtd);
}

FILE *cf_open(char *filename, int flag)
{
  return (main_sgml_file);
}

cf_getdispopt (char *filename) {
  return (0);
}

char *
cf_getdatafilename(char *filename, int recordid)
{
  return(NULL);
}
