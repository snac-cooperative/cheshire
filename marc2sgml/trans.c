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
/* TRANS.c -- Program to perform batch conversion of USMARC records 
   to SGML/MARC format.

   BY:     Jerome McDonough, Ray Larson
   DATE:   May 17, 1994
   REV:    July 14, 1996 - RRL - changed so that running with an existing
           associator and output file name will append records to that
           file rather than truncate. This permits update additions to
           existing databases.
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <dmalloc.h>
#include <unistd.h>
#include "alpha.h"
#include "marc.h"

#define RECBUFSIZE  	 100000
#define FIELDBUFSIZE     50000
#define SGMLRECSIZE      500000
#define READONLY    	 O_RDONLY
#define READWRITE        O_RDWR
#define BADFILE	    	 -1
#define TRUE   	    	 1
#define FALSE  	    	 0
#define SUCCESS          1
#define FAILURE          0
#define FREEALLMEM       1


/* EXTERNAL ROUTINES from marclib.c */
extern int GetNum(/*start char, number of chars to convert */);
extern int ReadMARC2(/* file id, buffer, buffer size */);
extern MARC_REC *GetMARC (/*buffer, record length, copy flag */);

/* EXTERNAL VARIABLES */
char recbuffer[RECBUFSIZE];
char fieldbuffer[FIELDBUFSIZE];

main(argc,argv)
     int argc;
     char *argv[];
{
  int marcin;
  FILE *assocfile, *sgmlout;       /* file descriptors for MARC input,
				      Associator file, and SGML output */
  char *inputf, *assocf, *outputf; /* to hold filenames give for input,
				      associator, and output files */
  MARC_REC *marcrec;
  MARC_FIELD *marcfld;
  char sgmlrec[SGMLRECSIZE];
  int marcrecsin = 0;
  int nrecs = 1;
  int lrecl = 0;
  char lang[4];
  int offset;                     /* offset in sgmlout file */
  int srecsize;                   /* size of sgmlrec for assoc file */
  int assoc_exists = 1;
  int outfile_exists = 1;
  int sum_marclen = 0;
  char *wfmode, *rfmode;
#ifdef WIN32
  wfmode = "w+b";
  rfmode = "r+b";
#else 
  wfmode = "w+";
  rfmode = "r+";
#endif

  /* check to see if command line properly formed... */
  if (argc < 4)
    {
      fprintf(stderr, "USAGE: marc2sgml INPUTFILE ASSOCFILE OUTPUTFILE [startid_number]\n\n");
      exit(1);
    }
  
  /* open up the MARC data file, output data file & associator file */
  
  inputf = argv[1];
  assocf = argv[2];
  outputf = argv[3];
  if (argc == 5)
    marcrecsin = (atol(argv[4]) - 1);
 
  marcin = open(inputf, READONLY);
  if (marcin == BADFILE) 
    {
      fprintf(stderr, "Can't open input file.");
    }
  if ((assocfile = fopen(assocf, rfmode)) == NULL) /* try to open for update */
    {
      assoc_exists = 0;
      if ((assocfile = fopen(assocf, wfmode)) == NULL) /* nope, create it */
	fprintf(stderr, "Can't open associator file.");
    }
  if ((sgmlout = fopen(outputf, "r+")) == NULL)
    {
      outfile_exists = 0;
      if ((sgmlout = fopen(outputf, "w+")) == NULL)
	fprintf(stderr, "Can't open output file.");
    }
  
  if (assoc_exists && outfile_exists) 
    {
      printf("Appending to existing file!!\n");
      fseek(assocfile, 0, 0);
      fread(&nrecs, sizeof(int), 1, assocfile);
      nrecs++;
      /* append to the end of the data file */
      fseek(sgmlout, 0, 2);
      offset = ftell(sgmlout);
      if (argc < 5)
	marcrecsin = nrecs;
    }
  else 
    {
      offset = (ftell(sgmlout));
      fwrite("0", 2 * sizeof(int), 1, assocfile);
    }
  /* for each record until done... */
  /* get the record, */
  while ((lrecl = ReadMARC2(marcin, recbuffer, sizeof recbuffer)) != 0)
    {
      marcrecsin++;
      if (marcrecsin % 10000 == 0)
	printf("Processing Record %d\n", marcrecsin);

      if ((marcrec = GetMARC(recbuffer, lrecl, 0)) == NULL)
	{
	  fprintf(stderr, "no record retrieved by GetMARC for input record #%d\n\n", marcrecsin);
	  /* exit(0); */
	  continue;
	}

      
#ifdef DOCONVERSIONS
      
      /* This was commented out by RRL -- it exits when records don't have
	 008 fields (like older music format records)
	 */
      /* check if it's in English.  If yes, go straight to SGML output
	 stage.  If not, run through LC-Romanization to ISO Entity set
	 conversion */
      if ((marcfld = (MARC_FIELD *) GetField(marcrec, NULL, fieldbuffer, "008")) == NULL)
	{
	  fprintf(stderr, "Couldn't extract Field 008 from MARC.\n\n");
	  exit(0);
	}
      if (strncmp(&marcfld->data[35], "eng", 3) != NULL)
	{
	  strncpy(lang, &marcfld->data[35], 3);
	  fprintf(stdout, "Record %d is in %s.\n", nrecs, lang);
	  memset(lang, 0, 4);
	  
	  /* INSERT LC-ROMAN TO ISO ENTITY CONVERSION HERE XXX */
	}
      
#endif
      /* output MARC to output data file */
      
      if(marc2sgml(marcrec, sgmlrec, marcrecsin) != SUCCESS)
	{
	  fprintf(stderr, "MARC to SGML conversion failed for record %d.\n", nrecs);
	  fprintf(stderr, "FAILED RECORD: %s\n\n", sgmlrec);
	}
      else
	{
	  
	  fprintf(sgmlout, "%s\n", sgmlrec);
	  
	  /* Free up the marc record */
	  FreeMARC_REC(marcrec);

	  /* record position of SGML record in associator file */
	  fseek(assocfile, 0, 2);
	  fwrite(&offset, sizeof(int), 1, assocfile);
	  srecsize = strlen(sgmlrec);
	  sum_marclen += srecsize;
	  fwrite(&srecsize, sizeof(int), 1, assocfile);
	  fseek(assocfile, 0, 0);
	  fwrite(&nrecs, sizeof(int), 1, assocfile);
	  fwrite(&sum_marclen, sizeof(int), 1, assocfile);
	  offset = (ftell(sgmlout));
	  nrecs++;
	}
      
      /* ...and start all over again */
      
    }

  exit(0);
}


