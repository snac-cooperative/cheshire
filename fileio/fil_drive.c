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
*         Function Name: fil_drive.c
*
*         Programmer: Helene Jaillet, Ray Larson
*
*         Purpose: Driver to test various fil_* functions
*
*         Usage:  fil_drive (when compiled)
*
*         Variables: Requires 2 data files: marc.dat, a small marc file
*                    and a.dat, its associator file. marc.dat can have any
*                    number of records and both files can be renamed by
*                    changing the "define" statement.
*
*         Return values and return codes:
*
*         Date:    11/20/93
*         Revised: 9/2/94 SGML version
*         Version: 1.0
*         Copyright (c) 1993, 1994. The Regents of the University of California
*				All Rights Reserved
*****************************************************************************/
#define MAIN

#include "cheshire.h"
#include "dmalloc.h"

#define BIBFILE "bibfile"

void main (int argc, char **argv) {
  int address;
  int recno;
  FILE *sgml_file_ptr;      /* pointer to marc files */
  FILE *assoc_file_ptr;      /* pointer to assoc. files */
  char *save_rec;
  char *sgmlrecord, *get_next_sgmlrec();
  int i;
  char *SGMLFILE;
  extern SGML_Doclist  *PRS_first_doc; /* set in document parsing */
  SGML_Document *doc, *GetSGML();
  char *rawdoc, *GetRawSGML();
  char *crflags, *opflags;
#ifdef WIN32
    crflags = "r+b";
    opflags = "r+b";
#else
    crflags = "r+";
    opflags = "r+";
#endif

  
  LOGFILE = stderr;

  if (argc < 2) {
    fprintf(LOGFILE, "Usage: %s configfile rawsgmlfile\n",argv[0]);
    exit(1);
  }
  
  i = 0;
  /* initialize config file */

  cf_info_base = (config_file_info *) cf_initialize(argv[1],crflags,opflags);

  SGMLFILE = argv[2];
  /* initialize or create all files */
  if (cf_createall() != -1) {
    printf("All files created\n");
  }
  else 
    printf("file creation failure\n");


  /* load a file (SGMLFILE) to existing bibfile (BIBFILE) */

  while ((sgmlrecord = get_next_sgmlrec(SGMLFILE,"USMARC")) != NULL) {

    /* add the test record to existing bibfile (BIBFILE) */
    if (fil_add(sgmlrecord, BIBFILE)) {
      printf("\nAdded test record.\n");
    }
    else {
      puts("Failed to add record. Exiting...");
      exit(1);
    }

    FREE(sgmlrecord);
  }
  /* the following closes the SGMLFILE */
  get_next_sgmlrec(NULL,NULL);

  /* replace a record; arbitrarily pick recno = 5 for test */
  if (fil_replace(save_rec, 5, BIBFILE)) {
    printf("\nReplaced record #5 with #3.\n");
  }
  else {
    puts("Couldn't replace record 5. Exiting...");
    exit(0);
  }

  /* delete a record; arbitrarily delete recno = 7 */
  recno = 7;
  
  assoc_file_ptr = cf_open(BIBFILE, ASSOCFILE);
  if (assoc_file_ptr) {
    fseek(assoc_file_ptr, (recno*sizeof(assoc_rec)), 0);
    fread(&address, sizeof(int), 1, assoc_file_ptr);
    printf("\nAddress of record to be deleted is = %ld\n", address);
  }
  else 
    printf("couldn't get assoc record\n");
  
  if (fil_del(recno, BIBFILE)) {
    printf("\nRecord deleted.\n");
  }
  else {
    puts("Couldn't delete record. Exiting...");
    exit(0);
  }
  /* show that record is deleted by reading its offset in */
  /* the associator file (which will be 0 ) */

  if (assoc_file_ptr) {
    fseek(assoc_file_ptr, (recno*sizeof(assoc_rec)), 0);
    fread(&address, sizeof(int), 1, assoc_file_ptr);
    printf("\nAddress of deleted record is now = %ld\n", address);
  }
  else 
    printf("couldn't get assoc record\n");
  
  printf("Getting raw record #6\n");
  rawdoc = GetRawSGML(BIBFILE,6L);
  printf("Record #6: \n%s\n",rawdoc);
  
  FREE(rawdoc);
  
  printf("Getting SGMLdoc record #8\n");
  
  doc = GetSGML(BIBFILE,8L);

  dump_doc(doc);
	
	
  printf("Attempting to get SGMLdoc record #7 (deleted earlier)\n");

  doc = GetSGML(BIBFILE,7L);
  if (doc == NULL)
    printf("SGML_Document point returned is NULL\n");
  else {
    printf("SGML_Document point returned is NOT NULL!!!\n");
    exit(1);
  }
  
  printf("Looks like all tests were OK.\nFreeing all memory...\n");

  free_doc_list(PRS_first_doc); /* free all the memory */

  /* shut all the files */
  cf_closeall();

  /* free all the DTDs */
  free_all_dtds();

  printf("All done.\n");
}

