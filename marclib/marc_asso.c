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
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#define   LENGBUFFER     10
#define   READONLY       O_RDONLY
#define   BADFILE        -1
#define   OUTPUT         O_CREAT+O_WRONLY+O_EXCL
#define   RWFLAGS        00400 | 00200 | 00040 | 00004
#define   RECTERM        '\035'



#ifndef   SEEK_CUR
#define   SEEK_CUR       1
#endif


/* EXTERNAL VARIABLES */

static char usage[] = "Usage: buildasso MARCfilename\n\
	   where MARCfilename is the file of MARC records to be processed\n";

char recbuffer[LENGBUFFER];

/* copied from marclib - don't need the rest of the routines */
/*********************************************************************/
/* getnum converts a specifed number of characters to a number       */
/*********************************************************************/
int GetNum(s,n)
char *s;
int n;
{  
  char nbuf[10]; /* no more than 9 digits */

  strncpy(&nbuf[0],s,n);
  nbuf[n] = '\0';
  return(atol(nbuf));
}


/* MAIN */

main(argc,argv)
int argc;
char *argv[];
{
  int lrecl, nrecs, recordID, recoffset;
  char *inputfilename, assocfilename[80];
  int  marcin, assoc, i;

  /* Handle command line arguments */
  if (argc < 2) {
    fputs (usage, stderr);
    exit(1);
  }
  inputfilename = argv[argc-1];
  
  /* open the input file */

  marcin = open(inputfilename, READONLY);
  if (marcin == BADFILE) {
    printf("cannot open marc input file %s\n", inputfilename);
    exit(0);
  }
  /* open the 'associator' index file */
  sprintf(assocfilename,"%s.asso",inputfilename);
  assoc = open(assocfilename,OUTPUT,RWFLAGS);
  if(assoc == BADFILE) {
    if (errno == EEXIST)
      printf("Index file %s already exists \n",assocfilename);
    else if (errno == ENOENT)
      printf("cannot open associator index file \n");
    else 
      printf("open error %d on associator index file \n", errno);

    exit(0);
  }

  /* skip through the input file, accumulating record lengths     */
  /* and write a 4 byte offsets to the associator file for each   */
  /* record read                                                  */

  /* write a null zero record for the associator */
  recordID = 0;
  recoffset = 0;
  write(assoc,&recoffset,sizeof(int));
  recordID++;

  while (read(marcin, recbuffer, 5) == 5) {
    /* verify that the buffer does contain digits */
    for(i=0; i<5; i++) {
      if (isdigit(recbuffer[i])) ; /* keep going */
      else {
	if (recoffset = rescan(marcin,recbuffer)) ;
	else {
	  fprintf(stderr,"Bad record - unable to rescan #%d\n",recordID+1);
					exit(1);
	}
      }
    }

    if((lrecl = GetNum(recbuffer, 5)) > 0)
      {
	write(assoc,&recoffset,sizeof(int));
	recordID++;
	recoffset = lseek(marcin, lrecl-5, SEEK_CUR);
      }
  }
  recordID--;
  recoffset -= lrecl;
  lseek(assoc, 0, 0);
  write(assoc,&recordID,sizeof(int));
  close(marcin);
  close(assoc);
  printf("Number of records processed: %d  Last record offset %d\n",
	 recordID,recoffset);
}


/*********************************************************************/
/* rescan - pick through garbage preceding a Marc record             */
/*          returns the new record offset in the file when it is     */
/*          successful, returns zero when it fails to fix things     */
/*********************************************************************/
rescan(file,buffer,badpos)
int file;
char *buffer;
int badpos;
{
  char c;
  int badcount, i;
  int currentpos;

  badcount = 0;

  /* seek back in the file to the record terminator */
  currentpos = lseek(file,-6,1);
  read(file,&c,1); /*get one byte*/
  if (c == RECTERM)
    fprintf(stderr,"Rescanning - previous record OK\n");
  
  /* skip non-digits */
  while (isdigit(c) == 0) {
    badcount++;
    read(file,&c,1);
    fprintf(stderr,"%c",c);
  }
  fprintf(stderr,"%d bytes skipped at file position %d\n",
		badcount, currentpos+1);
  
  /* start putting digits into the buffer */
  buffer[0] = c;
  fprintf(stderr,"new record length = %c",c);
  for(i=1;i<5;i++) {
    if (read(file,&buffer[i],1) == 0)
      fprintf(stderr,"end of file in rescan\n");
    fprintf(stderr,"%c",buffer[i]);
    if (isdigit(buffer[i]) == 0) {
      fprintf(stderr,"Non-digit in suspected record length\n");
			return(0);
    }
  }
  fprintf(stderr,"\n\n");
  
  /* if we get to here should be a valid record (we hope) */
  return(lseek(file,0,1) - 5);
}


