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
#include <sys/types.h>
#include <regex.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include "cheshire.h"
#include "dmalloc.h"

extern FILE *infile;
extern void CONTparse();
extern continuation *first_cont;
extern int cont_restart_scanner(FILE *newfile);
extern void CONTparse();



int    match(char *pattern, char *string, int casesensitive)
{
  char    message[200];
  int    error, msize;
  regex_t preg;
  int    flags;
  
  
  /* The REG_ICASE and REG_NEWLINE flags are different from the library */
  /* values apparently -- so we use ALL flags just in case              */
  if (casesensitive) 
    flags = REG_NOSUB|REG_EXTENDED|REG_NEWLINE;
  else
    flags = REG_NOSUB|REG_ICASE|REG_EXTENDED|REG_NEWLINE;
  
  error = regcomp(&preg, pattern, flags);
  
  if (error) { 
    msize = regerror(error, &preg, message, 200);
    regfree(&preg); 
    return(0);
  } 
  /* do the match */
  error = regexec(&preg, string, (size_t) 0, NULL, 0); 
  
  regfree(&preg); 
  if (error == REG_NOMATCH) {
    /* printf("No matches in string\n"); */
    return(0);
  }
  else {
    return(1);
  }
}

/* Yeah, this doesn't use the configfile info, so it is potentially */
/* dangerous */


int
main (int argc, char **argv)
{
  char afilename[200];
  int i, j, atoi();
  char c;
  FILE *LOGFILE = stderr;
  FILE *afile;
  FILE *cfile;
  char *fmwrite;  
  char *fmread;  
  char linebuffer[500];
  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
  /* added by this function and passed to index */
  /* function. Also the value of the 0th record */
  /* in assocfile to keep track of highest      */
  /* logical recno in use.                      */
  continuation *cont;
  int find_num = 1; /* if an argument is provided for record number */
  
  if (argc < 4 || argc > 5) {
    fprintf(LOGFILE, "usage: %s assoc_file_name cont_file_name \"filter pattern\"\n",argv[0]);
    exit(1);
  }
  
#ifdef WIN32
  fmread = "rb+";
  fmwrite = "wb+";
#else
  fmread = "r+";
  fmwrite = "w+";
#endif
  
  
  if ((afile = fopen((char *)argv[1],fmread)) == NULL) {
    fprintf(LOGFILE, "unable to open %s\n",(char *)argv[1]);
    exit(1);
  }
  
  if ((cfile = fopen((char *)argv[2],fmread)) == NULL) {
    fprintf(LOGFILE, "unable to open %s\n",(char *)argv[2]);
    exit(1);
  }
  
  
  /* parse the EXISTING continuation input file */
  fseek(cfile, 0, SEEK_SET);
  cont_restart_scanner(cfile);
  CONTparse();
  
  for (cont = first_cont; cont; cont = cont->next_cont) {
    /* filter based on the name */
    if (match(argv[3],cont->name,1)) {
      /* A match on the pattern for the file name */
      for (i = cont->docid_min; i <= cont->docid_max; i++) {
	
	fseek(afile,i*sizeof(assoc_rec), 0);
	fread(&assocrec, sizeof(assocrec), 1, afile);
	/* set deleted record's offset to negative, write it; return SUCCEED */
	if (assocrec.offset == 0)
	  assocrec.offset = -1;
	else if (assocrec.offset == -1) 
	  assocrec.offset = 0;
	else
	  assocrec.offset *= -1 ; /* if it is already deleted undelete otherwise delete*/

	fseek(afile, (i*sizeof(assoc_rec)), 0);
	fwrite(&assocrec, sizeof(assoc_rec), 1, afile);
	fflush(afile);
	/* write back the assoc record */
      }      
    }
    
    /* if it doesn't match the pattern do nothing */
    
  }  
  
  fclose(afile);
  fclose(cfile);
  
  return 0;
}









