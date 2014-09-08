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
/**************************************************************************/
/* DISPMARCTEST - print a marc record from a file by ID number            */
/*                this is a test shell to call DispMARC();                */
/**************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include "marc.h"


#define   MAXRECSOUT     1000000
#define   RECBUFSIZE     10000
#define   FIELDBUFSIZE   10000
#define   READONLY       O_RDONLY
#define   BADFILE        -1
#define   TRUE           1
#define   FALSE          0


/* EXTERNAL ROUTINES -- in marclib.c, memcntl.c and dispmarc.c */
extern int GetNum(/*start char, number of chars to convert */);
extern int ReadMARC(/* file id, buffer, buffer size*/);
extern MARC_FIELD *SetField(/*rec, dir*/);
extern MARC_REC *GetMARC(/*buffer,record length, copy flag*/);
extern MARC_FIELD *GetField(/*marc_rec pointer,buffer,field tag*/);
extern MARC_SUBFIELD *GetSubf(/*marc_field pointer,buffer,subfield code*/);
extern MARC_REC *DispMARC()/*marc file, assoc file, record ID, format #,
				display #, maxlen , output routine pointer */;

/* EXTERNAL VARIABLES */

static char usage[] = "\nUsage: dispmarc {-i000} {-f00} filename\n\
	    where -i is the record ID of the record to print\n\
            and   -f is the format type\n\n";

extern char recbuffer[RECBUFSIZE];
extern char fieldbuffer[FIELDBUFSIZE];


printline(line) /* a simple output routine called from dispmarc */
char *line;
{
  printf("%s",line);
}


/* MAIN */

main(argc,argv)
     int argc;
     char *argv[];
{
  FILE *specfile = NULL, *fopen();
  MARC_DIRENTRY_OVER *dir;
  MARC_FIELD *fld;
  MARC_SUBFIELD *subf;
  MARC_REC *rec;
  int lrecl, frec, nrecs, format, recordID;
  char *inputfilename, assocfilename[100];
  char *readptr, *c;
  int  marcin, i, assocfile;
  int  print_leader = FALSE;         /* special function flags */
  int  print_directory = FALSE;
  int  print_authority = FALSE;
  int printline(); /* call-back output routine */
  int seqnum = 0;
  
  nrecs = 1; /* 'all' of them, may change with args */
  recordID = 0;    /* start at beginning unless otherwise instructed */
  format = 0;      /* default format */
  
  /* Handle command line arguments */
  if (argc < 2)
    error(usage);
  
  inputfilename = argv[argc-1];
  
  for (i = 1; i < (argc-1); i++) {
    c = argv[i];
    if (*c == '-')
      switch (*++c) {
      case 'a': print_authority = TRUE;
	break;
      case 'i': if (isdigit(*++c))
	recordID = atol(c);
      else {
	fputs (usage, stderr);
	exit(1);
      }
	break;
      case 'n': if (isdigit(*++c))
	nrecs = atol(c);
      else {
	fputs (usage, stderr);
	exit(1);
      }
	break;
      case 'd': print_directory = TRUE;
	break;
      case 'l': print_leader = TRUE;
	break;
      case 'f': if (isdigit(*++c))
	format = atol(c);
      else {
	fputs (usage, stderr);
	exit(1);
      }
	break;
	/*
	   case 's': if (argv[i+1] != NULL &&
	   (specfile = fopen(argv[i+1], "r")) != NULL) {
	   getspecs(specfile);
	   fclose(specfile);
	   }
	   break;
	   */
      default : fputs (usage, stderr);
	exit(1);
      }
  }
  
  /* open the input file */
  
  marcin = open(inputfilename, READONLY);
  if (marcin == BADFILE) {
    fprintf(stderr, "\n%s: ");
    error("cannot open file");
  }
  
  sprintf(assocfilename, "%s.asso", inputfilename);
  assocfile = open(assocfilename, READONLY);
  if (assocfile == BADFILE) {
    fprintf(stderr,"no associator file %s\n", inputfilename);
    /* error("cannot open associator"); */
  }
  
  /* call DispMARC to show the record */
  
  if (marcin) {
    while (nrecs--) {
      rec = DispMARC(marcin,assocfile,recordID,
		     format, recordID, 75, printline);
      
      
      if (rec == NULL)
	error("Unable to display record"); 
      recordID++;
      /* free up the allocated space for the record */
      FreeMARC_REC(rec);
    }
  }
  
  close(marcin);
  close(assocfile);
}

/* GETSPECS -- read a specification from the supplied file */

getspecs(f)
     FILE *f;
{
  /* do nothing yet */
}


/*  ERROR -- print an error message and exit  */

error(msg)
     char *msg;
{
  fprintf(stderr, "\n%s\n\n", msg);
  exit(1);
}


tagcmp(pattag,comptag)
     char *pattag, *comptag;
{
  int i;
  for (i = 0; i < 3; i++) {
    if (pattag[i] == 'x' || pattag[i] == 'X')
      continue;
    if (isdigit(pattag[i]) && pattag[i] == comptag[i])
      continue;
    else
      return(0);
  }
  /* must match */
  return(1);
} 






