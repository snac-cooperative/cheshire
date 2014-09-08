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
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define MAIN

#include "cheshire.h"
#include <dirent.h>

#include "dmalloc.h"

extern continuation *first_cont;
extern int cont_restart_scanner(FILE *newfile);
extern void CONTparse(); 

int get_next_sgmlrec_data(int *offset, char *filename, char *tag, 
			  int taglen, int skipheaders);


int
process_directory(FILE *afile, FILE *cfile, Tcl_HashTable *ht,
		  int *recno, int *tot_length, int *cfilenum, 
		  char *dirname, char *tag, int tag_length, int quiet,
		  int skipheaders)
{
  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int startnum, endnum; /* the starting and ending recnos for a given file */
  int result;
  Tcl_HashEntry *entry;

  DIR *dirptr;
  struct dirent *direntry;
  int dirsize, dirname_size;
  char *filenamebuff;

  /* We need to scan the directory and grab each file -- getting its  */
  /* page number from the name and generating the appropriate mapping */
  /* info in the associated special index files                       */
  if ((dirptr = opendir(dirname)) == NULL)  {
    /* printf("Normal file? (errno = %d) - returning -- \n", errno); */
    return(0); /* this indicates that this is probably a normal file  */
  }
 

  while ((direntry = readdir(dirptr)) != NULL) {
    
    /* get each file in the directory... */
#if defined SOLARIS || defined LINUX || defined WIN32
    dirsize = direntry->d_reclen + strlen(dirname) + 3;
    dirname_size = direntry->d_reclen - sizeof(struct dirent) + 2;
#else
    dirsize = strlen(dirname) + direntry->d_namlen + 3;
    dirname_size = direntry->d_namlen + 1;
#endif
    filenamebuff = CALLOC(char, dirsize); 
    if (filenamebuff == NULL) {
      fprintf(LOGFILE, "Unable to allocate file name buffer in idxpages\n");
      exit(1);
    }
    
    /* skip dot and dotdot */
    if (strcmp(direntry->d_name,".") == 0 
	|| (strcmp(direntry->d_name, "..") == 0)) {
      FREE(filenamebuff);
      continue;
    }
    
    strcpy(filenamebuff, dirname);
#ifdef WIN32
    strcat(filenamebuff, "\\");
#else
    strcat(filenamebuff, "/");
#endif
    strcat(filenamebuff, direntry->d_name);

    entry = Tcl_FindHashEntry(ht,filenamebuff);

    if (entry == NULL) {

      /* This filename may be a directory so we will recursively call */
      /* this routine                                                 */
      /* printf("(recursive) Calling process directory for %s\n", filenamebuff);*/

      if (process_directory(afile, cfile, ht, recno, tot_length, 
			    cfilenum, filenamebuff, tag, tag_length, 
			    quiet, skipheaders) == 1)
	continue; /* when the result is a 1 */
      
      offset = 0;
      
      startnum = *recno + 1;

      while ((result = get_next_sgmlrec_data(&offset, 
					     filenamebuff, tag, tag_length, 
					     skipheaders)) != 0) {
	if (result < 0) {
	  /* this is a special case where there is trailing information */
	  /* after the last record end tag in a file                    */
	  /* redo the last record...                                    */
	  fseek(afile,*recno*sizeof(assoc_rec), 0);
	  fread(&assocrec, sizeof(assoc_rec), 1, afile);
	  assocrec.recsize = assocrec.recsize + (result * -1);
	  *tot_length += assocrec.recsize ;
	  
	  if (!quiet)
	    printf ("Additional Data at EOF: ReWriting Record #%08d: Length %08d\n",*recno,
		    (int)assocrec.recsize);
	  
	  fseek(afile,*recno*sizeof(assoc_rec), 0);
	  fwrite(&assocrec, sizeof(assoc_rec), 1, afile);
	  fflush(afile);
	  
	}
	else {
	  *recno = *recno + 1;
	  
	  if (!quiet)
	    printf ("Record #%08d: Length %08d\n",*recno, result);
	  
	  /* record offset and length of sgml record in assoc file */
	  assocrec.offset = offset ;
	  assocrec.recsize = result;
	  *tot_length += result;
	  fseek(afile,*recno*sizeof(assoc_rec), 0);
	  fwrite(&assocrec, sizeof(assoc_rec), 1, afile);
	  fflush(afile);
	}
      }

      get_next_sgmlrec_data(NULL,NULL,NULL, 0, 0); /* closes file */

      if (*recno >= startnum) { /* we actually processed some records */
	endnum = *recno;
	/* write out the continuation record */
	*cfilenum = *cfilenum + 1;
	fprintf(cfile, "<FILECONT ID=%d MIN=%d MAX=%d> %s </FILECONT>\n",
		*cfilenum, startnum, endnum, filenamebuff);
      }
    }
    else {
      /* match in the hash table */
      if (!quiet)
	printf ("Duplicate file name: %s\n", filenamebuff);
    }
    FREE(filenamebuff);
  }
  closedir(dirptr);
  return (1);
}

int
main (int argc, char **argv)
{
  int result;
  char afilename[500];
  char cfilename[500];
  FILE *afile, *cfile, *fopen();
  char *tag;
  int tag_length;
  int quiet;
  int recursive;
  int skipheaders;
  int exists;
  int cfilenum = 0;
  continuation *cont;
  char *fmread, *fmwrite;
  Tcl_HashTable *ContNamesHash;

  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
                      /* added by this function and passed to index */
		      /* function. Also the value of the 0th record */
		      /* in assocfile to keep track of highest      */
		      /* logical recno in use.                      */
  int total_doc_length; /* sum of document lengths */
  LOGFILE = stderr;

  

  if ((argc > 2 && (argv[1][0] == '-' && argv[1][1] == 'q')) 
      || (argc > 3 && (argv[2][0] == '-' && argv[2][1] == 'q'))
      || (argc > 4 && (argv[3][0] == '-' && argv[3][1] == 'q')))
    quiet = 1;
  else
    quiet = 0;

  if ((argc > 2 && (argv[1][0] == '-' && argv[1][1] == 'r'))
      || (argc > 3 && (argv[2][0] == '-' && argv[2][1] == 'r'))
      || (argc > 4 && (argv[3][0] == '-' && argv[3][1] == 'r')))
    recursive = 1;
  else
    recursive = 0;

  if ((argc > 2 && (argv[1][0] == '-' && argv[1][1] == 's'))
      || (argc > 3 && (argv[2][0] == '-' && argv[2][1] == 's'))
      || (argc > 4 && (argv[3][0] == '-' && argv[3][1] == 's')))
    skipheaders = 1;
  else
    skipheaders = 0;

  if (argc-quiet-recursive-skipheaders < 3 
      || argc-quiet-recursive-skipheaders > 7) {
    fprintf(LOGFILE, "usage: %s {-q}{-s} sgmlfile <assocfile> sgmltag{/sgml_subtag/.../sgml_subtag}\n   or: %s {-q}{-s} -r directory assocfile sgmltag{/sgml_subtag/.../sgml_subtag}\n",argv[0], argv[0]);
    exit(1);
  }

  /* open up the associator (only one file even if recursive */
  if (argc-quiet-recursive-skipheaders == 3)
    sprintf(afilename,"%s.assoc",argv[1+quiet+recursive+skipheaders]);
  else
    strcpy(afilename, argv[2+quiet+recursive+skipheaders]);

  /* printf (" afilename = %s", afilename); */
#ifdef WIN32
  /* have to define the file as binary */
  fmread = "rb+";
  fmwrite = "wb+";
#else
  fmread = "r+";
  fmwrite = "w+";
#endif

  
  if ((afile = fopen(afilename,fmread)) == NULL) { /* try to open for update */
    if ((afile = fopen(afilename,fmwrite)) == NULL) {
      fprintf(LOGFILE, "unable to open %s\n",afilename);
      exit(1);
    }
  }

  if (recursive) {
    /* open a file for generating continuation entries */
    sprintf(cfilename,"%s.cont",argv[1+quiet+recursive+skipheaders]);
    if ((cfile = fopen(cfilename,fmread)) == NULL) { /* try to open for update */
      if ((cfile = fopen(cfilename,fmwrite)) == NULL) {
	fprintf(LOGFILE, "unable to open %s\n",cfilename);
	exit(1);
      } else {
	/* new file -- create empty hash table */
	ContNamesHash = CALLOC(Tcl_HashTable,1);
	Tcl_InitHashTable(ContNamesHash,TCL_STRING_KEYS);
      }
    } else {
      /* parse the EXISTING continuation input file */
      fseek(cfile, 0, SEEK_SET);
      cont_restart_scanner(cfile);
      CONTparse();
      ContNamesHash = CALLOC(Tcl_HashTable,1);
      Tcl_InitHashTable(ContNamesHash,TCL_STRING_KEYS);

      for (cont = first_cont; cont; cont = cont->next_cont) {
	/* printf("CONT FILE NAME IS: %s \n", cont->name); */
	/* build hash entries for name */	  
	Tcl_SetHashValue(
			 Tcl_CreateHashEntry(
					     ContNamesHash,
					     cont->name,
					     &exists),
			 (ClientData)cont);
       if (cfilenum < cont->id_number)
	 cfilenum = cont->id_number;
      }

      /* there is an existing file -- go to the end */
      fseek(cfile, 0, SEEK_END);
    }
  }

  high_recno = 0;
  total_doc_length = 0;
  /* get new highest logical record number */
  fseek(afile, 0, 0);
  fread(&high_recno,sizeof(int),1, afile); 
  fread(&total_doc_length,sizeof(int),1, afile); /* Should be doclent */ 
  recno = high_recno;

  if (high_recno > 0) {
   printf ("Appending to existing associator file %s... start record #%d\n", 
	   afilename, high_recno+1); 
   fseek(afile,recno*sizeof(assoc_rec), 0);
   fread(&assocrec, sizeof(assoc_rec), 1, afile);
   offset = assocrec.offset + assocrec.recsize;
  } 
  else
    offset = 0;        /* address in marc file */



  tag = argv[argc-1];
  tag_length = strlen(tag);

  if (!recursive) {
    int seekresult, writeresult, flushresult;

    while ((result = get_next_sgmlrec_data(&offset, 
					   argv[1+quiet+recursive+skipheaders],
					   tag, tag_length, skipheaders)) != 0) {
      recno++;
      if (!quiet)
	printf ("Record #%08d: Length %08d\n",recno, result);
      
      /* record offset and length of sgml record in assoc file */
      assocrec.offset = offset ;
      assocrec.recsize = result;
      total_doc_length += result;
      seekresult = fseek(afile,recno*sizeof(assoc_rec), SEEK_SET);
      writeresult = fwrite(&assocrec, sizeof(assoc_rec), 1, afile);
      flushresult = fflush(afile);
    }

    printf("End of File encountered after record %d\n", recno);

    get_next_sgmlrec_data(NULL,NULL,NULL, 0, 0); /* closes file */
    
    /* increment the zeroth record to record the */
    /* highest record number now in use          */
    fseek(afile, 0, 0);
    fwrite(&recno, sizeof(int), 1, afile);
    fwrite(&total_doc_length, sizeof(int), 1, afile);
    fclose(afile);
  }
  else {
    /* Process a directory tree recursively looking for sgml tags */
    /* that match the one provided...                             */
    /* the filename provided should be a directory                */
    process_directory(afile, cfile, ContNamesHash, &recno, &total_doc_length, 
		      &cfilenum, argv[1+quiet+recursive+skipheaders], 
		      tag, tag_length, quiet, skipheaders);
    

    get_next_sgmlrec_data(NULL,NULL,NULL, 0, 0); /* closes file */
    
    /* increment the zeroth record to record the */
    /* highest record number now in use          */
    fseek(afile, 0, 0);
    fwrite(&recno, sizeof(int), 1, afile);
    fwrite(&total_doc_length, sizeof(int), 1, afile);
    fclose(afile);
  }
  return 0;

}





