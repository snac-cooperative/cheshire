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
*	Header Name:	tests2m.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	Driver to test sgml2marc conversion function
*
*	Usage:		tests2m inputfile outputfile
*
*	Variables:	inputfile -- file of SGML MARC records to translate
*                       outputfile -- output file for translated MARC records
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
#include <stdlib.h>
#include <string.h>
#include "sgml.h"
#include "dmalloc.h"
#include "alpha.h"
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
#define MARCSIZE 99999

/* config_file_info *cf_info_base; */
FILE *LOGFILE; 
int last_rec_proc;

main(argc,argv)
int argc;
char *argv[];
{
  FILE *outputf, *inputf, *assocf;
  char *marcrec;
  SGML_Document *sgmlrec, *GetSGMLRec();
  long recsize;
  long i;
  char *inputfilename;
  char *inputassocname;
  char *DTDname;
  char *outputfilename;
  
  LOGFILE = stderr;
  last_rec_proc = 0;

  if (argc < 4 || argc > 5)
    {
      fprintf(LOGFILE, "USAGE: %s inputfile {inputassociator} DTDfile outputfile\n\n", argv[0]);
      exit(1);
    }

  inputfilename = argv[1];
  if (argc == 5) {
    inputassocname = argv[2];
    DTDname = argv[3];
    outputfilename = argv[4];
  } 
  else { /* argc is 4, so input file has no associator */
    inputassocname = NULL;
    DTDname = argv[2];
    outputfilename = argv[3];
  } 


  if ((outputf = fopen(outputfilename, "w+")) == NULL)
    {
      fprintf(LOGFILE, "could not open output file %s\n", outputfilename);
      exit(1);
    }
     
  if ((inputf = fopen(inputfilename, "r")) == NULL)
    {
      fprintf(LOGFILE, "could not open input file %s\n", inputfilename);
      exit(1);
    }
  
  if (inputassocname != NULL) 
    {
      if ((assocf = fopen(inputassocname, "r")) == NULL)
	{
	  fprintf(LOGFILE, "could not open input associator file %s\n", 
		  inputassocname);
	  exit(1);
	}
    }
  else
    assocf = NULL;
  

  for (i = 1; 
       ((sgmlrec = GetSGMLRec(inputf,assocf, DTDname, i, &recsize)) != NULL)
       && (strncmp(sgmlrec->buffer, "bibfile", 7)); i++)
    {
      last_rec_proc = i;
      marcrec = (char *) calloc(recsize, sizeof(char));
	  
      if ((sgml2marc(sgmlrec, marcrec, recsize)) != SUCCESS)
	{
	  fprintf(LOGFILE, "Conversion Failed for Record %d\n", i);
	  exit(1);
	}
      fprintf(outputf, "%s", marcrec);
      free(marcrec);
    }
  fclose(outputf);
  exit(0);
}


