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
/* test code for simple sgml reading */
#define MAIN
#include <string.h>
#include "cheshire.h"
#include "dmalloc.h"

extern FILE *infile;

int
main (int argc, char **argv)
{
  char afilename[200];
  int i, j, atoi();
  char c;
  FILE *LOGFILE = stderr;
  FILE *afile, *fopen();
  FILE *sfile;
  char *fmode;  
  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
                      /* added by this function and passed to index */
		      /* function. Also the value of the 0th record */
		      /* in assocfile to keep track of highest      */
		      /* logical recno in use.                      */

  int find_num = 1; /* if an argument is provided for record number */

  if (argc < 2 || argc > 3) {
    fprintf(LOGFILE, "usage: %s sgmlfile <recordnum>\n",argv[0]);
    exit(1);
  }

  if ((sfile = fopen(argv[1],"r")) == NULL) {
    fprintf(LOGFILE, "unable to open %s\n",argv[1]);
    exit(1);
  }

  sprintf(afilename,"%s.assoc",argv[1]);

#ifdef WIN32
  fmode = "rb";
#else
  fmode = "r";
#endif

  if ((afile = fopen(afilename,fmode)) == NULL) {
    fprintf(LOGFILE, "unable to open %s\n",afilename);
    exit(1);
  }
  
  high_recno = 0;
  /* get new highest logical record number */
  fseek(afile, 0, 0);
  fread(&high_recno,sizeof(int),1, afile); 
  recno = high_recno;
  
  offset = 0;        /* address in marc file */

  if (argc == 3) {
    find_num = atoi(argv[2]);
    high_recno = find_num;
  }

  for (i = find_num; i <= high_recno; i++) {

    fseek(afile,i*sizeof(assoc_rec), 0);
    fread(&assocrec, sizeof(assocrec), 1, afile);

    printf ("\n*************************************************\n");
    printf ("*  Record #%d: Offset %d: Length %d: \n",i,
	    assocrec.offset, assocrec.recsize);
    printf (  "-------------------------------------------------\n");
    
    fseek(sfile,assocrec.offset, 0);
    for (j = 0; j < assocrec.recsize; j++) {
      fread(&c,1,1,sfile);
      putchar((int)c);
    }
  }

  fclose(afile);
  fclose(sfile);

  return 0;
}









