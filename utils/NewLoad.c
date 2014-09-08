/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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
 *  Created by Ray R. Larson 
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
/* This program loads data into a BerkeleyDB file in parsed form    
 * a hash of the record (using code from md5) is done to test
 * if the record is already stored in the database                  
 * if it is, then nothing is done, otherwise it is stored and the   
 * hash entry created.
 * The program is based on buildassoc and will do the same sorts of 
 * scans of files or directories to get the records to be stored.   
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef MACOSX
#include <sys/types.h>
#include <machine/types.h>
#endif
#ifdef WIN32
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#ifdef WIN32
#define strcasecmp _stricmp
#endif

#define MAIN

#include "cheshire.h"
#include <dirent.h>
#include "md5.h"
#include "dmalloc.h"
#include "flatrec.h"

int last_rec_proc = 0;

extern continuation *first_cont;
extern int cont_restart_scanner(FILE *newfile);
extern void CONTparse(); 

extern int get_next_sgmlrec_data(int *offset, char *filename, char *tag, 
				 int taglen, int skipheaders);

extern Output_Record *flatten_sgml_document(SGML_Document *doc, 
					    char digest[17],
					    int *length, int docid);

extern DB_ENV *gb_dbenv;


int
process_directory(DB *db_handle, SGML_DTD *main_dtd,
		  Tcl_HashTable *ht,
		  int *recno, int *cfilenum, int *total_size,
		  char *dirname, char *tag, int tag_length, 
		  int quiet, int skipheaders)
{
  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in marc file */
  int startnum, endnum; /* the starting and ending recnos for a given file */
  int result;
  Tcl_HashEntry *entry;
  FILE *datafile;
  DIR *dirptr;
  struct dirent *direntry;
  int dirsize, dirname_size;
  char *filenamebuff;
  char *workrecord;
  char *fmread, *fmwrite;
  int seekreturn;
  struct MD5Context mdcontext;
  char digestbuff[17];
  int db_errno;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  int outrec_length;
  int reclen;
  SGML_Document *doc, *sgml_parse_document();
  
  /* We need to scan the directory and grab each file -- getting its  */
  /* page number from the name and generating the appropriate mapping */
  /* info in the associated special index files                       */
  if ((dirptr = opendir(dirname)) == NULL)  {
    return(0); /* this indicates that this is probably a normal file */
  }
 

#ifdef WIN32
  /* have to define the file as binary */
  fmread = "rb";
  fmwrite = "wb+";
#else
  fmread = "r";
  fmwrite = "w+";
#endif
  
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
      if (process_directory(db_handle, main_dtd, ht, recno, total_size, 
			    cfilenum, filenamebuff, tag, tag_length, 
			    quiet, skipheaders) == 1)
	continue; /* when the result is a 1 */
      
      offset = 0;
      
      startnum = *recno + 1;
      
      datafile = fopen(filenamebuff, fmread);
      
      
      while ((result = get_next_sgmlrec_data(&offset, 
					     filenamebuff, tag, 
					     tag_length, skipheaders)) != 0) {
	if (result < 0) {
	  /* this should not be the case any more  (right)... */
	  /* this is a special case where there is trailing information */
	  /* after the last record end tag in a file                    */
	  /* redo the last record...                                    */
	  printf("Got result < 0 in processing file %s", filenamebuff);
	}
	else {
	  *recno = *recno + 1;

	  if (!quiet)
	    printf ("File: %s Record #%08d: Length %08d\n",filenamebuff,
		    *recno, result);
	  
	  /* record offset and length of sgml record in assoc file */
	  assocrec.offset = offset ;
	  assocrec.recsize = result;
	  
	  /* fetch, parse and store the record... */
	  workrecord = CALLOC(char, result+10);
	  
	  /* if we get to here the record number must be OK */
	  seekreturn = fseek(datafile, assocrec.offset, SEEK_SET);
	  if (seekreturn != -1) {
	    reclen = fread(workrecord, sizeof(char), 
			   assocrec.recsize, datafile);
	    if (reclen == 0) {
	      printf("Read of data failed for %d\n", recno);
	      exit (0);
	    }
	    else {
	      /* got the record -- do the digest */
	      MD5Init(&mdcontext);
	      MD5Update(&mdcontext, workrecord, assocrec.recsize);
	      MD5Final(digestbuff, &mdcontext);
	      digestbuff[16] = '\0';
	      /* printf ("Digest for record %d is %s\n", recno, digestbuff); */
	      /* search for the digest record in the database */
	      /* Initialize the key/data pair so the flags aren't set. */
	      memset(&keyval, 0, sizeof(keyval));
	      memset(&dataval, 0, sizeof(dataval));
	      
	      keyval.data = (void *)digestbuff;
	      keyval.size = sizeof(digestbuff);
	      
	      returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	      
	      if (returncode == 0) { /* found the word in the index already */
		printf("DUPLICATE RECORD FOUND as record #%d\n", *recno);
		*recno -= 1; /* back off */
		continue;
	      }
	      else if (returncode == DB_NOTFOUND) { 
		/* a new record not in the database */	    
		/* set up the new key */
		keyval.data = (void *)recno;
		keyval.size = sizeof(int);
		
		doc = sgml_parse_document(main_dtd, cf_info_base->file_name, workrecord,
					  *recno, -1);
		if (doc == NULL) {
		  printf("Document Parsing failed for record #%d\n", recno);
		  exit(1);
		}
		
		/* OK, we have the parsed document, now flatten it and write */
		/* it to the database                                        */
		outrec_length = 0;
		outrec = flatten_sgml_document(doc, digestbuff,
					       &outrec_length, *recno);
		if (outrec == NULL) {
		  printf("Record conversion failed for record #%d\n", recno);
		  exit(1);
		}
		/* store the flattened document */
		dataval.size = outrec_length;
		*total_size += outrec_length;
		dataval.data = (void *)outrec;
		/* put the new record into the database */
		returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 
					    DB_NOOVERWRITE); 
		
	    
		if (returncode == DB_KEYEXIST) {
		  if (!quiet)
		    printf("Document ID #%d already exists in the database\n", *recno);
		}
		
		/* put the new digest into the database */
		keyval.data = (void *)digestbuff;
		keyval.size = sizeof(digestbuff);
		dataval.size = sizeof(int);
		dataval.data = (void *)recno;
		returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 
					    DB_NOOVERWRITE); 
		
		if (returncode == DB_KEYEXIST) {
		  if (!quiet)
		    printf("Document digest for record #%d already exists in the database\n", recno);
		}

		/* sync the database -- flushing all of the buffers to disk */
		db_handle->sync(db_handle,0);

		/* free up the parsed document and the output record */
		free_doc(doc);
		FREE(outrec);

		last_rec_proc = *recno;
	  
	      }
	    }
	  } 
	  else {
	    printf("Seek to start of data failed for %d\n", recno);
	    exit (0);
	  }
	  
	  FREE(workrecord);
	}
	

      }
      fclose(datafile); /* close the current file and open the next one */
    }
    else {
      /* match in the hash table */
      if (!quiet)
	printf ("Duplicate file name: %s\n", filenamebuff);
    }
    FREE(filenamebuff);
  }
  return (1);
}

int
main (int argc, char **argv)
{
  int result;
#ifndef WIN32
  struct rlimit limits;
#endif
  config_file_info *cf;
  char *crflags, *opflags;
  char afilename[500];
  char cfilename[500];
  FILE *afile, *cfile, *datafile, *fopen();
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
  char *workrecord;
  SGML_Document *doc, *sgml_parse_document();
  SGML_DTD *main_dtd, *cf_getDTD();
  int seekreturn, reclen;

  assoc_rec assocrec; /* associator output record */
  int offset;        /* address in data file */
  int high_recno;    /* current highest logical record number */
  int recno;         /* logical record number of the new record    */
                      /* added by this function and passed to index */
		      /* function. Also the value of the 0th record */
		      /* in assocfile to keep track of highest      */
		      /* logical recno in use.                      */
  int total_size;     /* SUM of all stored record sizes             */
  struct MD5Context mdcontext;
  char digestbuff[17];
  DB *db_handle;
  DBTYPE dbtype;
  int db_errno;
  DBT keyval;
  DBT dataval;
  int returncode;
  Output_Record *outrec;
  int outrec_length;

  LOGFILE = stderr;

  recno = 0;
  high_recno = 0;
  total_size = 0;
  offset = 0;

#ifndef WIN32
  /* Crank up the number of permitted open files --- just in case */
  getrlimit(RLIMIT_NOFILE, &limits);

  if (limits.rlim_cur < limits.rlim_max) {
    limits.rlim_cur = limits.rlim_max - 1;
    setrlimit(RLIMIT_NOFILE, &limits);
  }

#ifdef DEBUGIND
  getrlimit(RLIMIT_NOFILE, &limits);
  printf("curr fileno %d  max fileno %d\n",  limits.rlim_cur, limits.rlim_max);
#endif
#endif

#ifdef WIN32
  crflags = "wb+";
  opflags = "rb+";
#else
  crflags = "w+";
  opflags = "r+";
#endif

  

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

  if (argc-quiet-recursive-skipheaders < 3 || argc-quiet-recursive-skipheaders > 7) {
    fprintf(LOGFILE, "usage: %s {-q}{-s} configfile sgmlfile sgmltag\n   or: %s {-q}{-s} -r configfile directory sgmltag\n Note that the configfile must contain the filename where the data will be actually stored\n",argv[0], argv[0]);
    exit(1);
  }

  argc -= quiet;
  argc -= recursive;
  argc -= skipheaders;
  argv += quiet;
  argv += recursive;
  argv += skipheaders;

  /* open up the associator (only one file even if recursive */
  sprintf(afilename,"%s.assoc_temp",argv[2]);

  printf ("Temp Assoc file = %s \n", afilename);
#ifdef WIN32
  /* have to define the file as binary */
  fmread = "rb";
  fmwrite = "wb+";
#else
  fmread = "r";
  fmwrite = "w+";
#endif


  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }

  cf_init_db_env(cf_info_base); 



  if (recursive) {
    /* still use the hash table -- but not for the same reason */
    ContNamesHash = CALLOC(Tcl_HashTable,1);
    Tcl_InitHashTable(ContNamesHash,TCL_STRING_KEYS);
  }

  printf("File name to be created is %s\n", cf_info_base->file_name);

  /* prints the interesting bits of the config info structure */
  /* cf_print_info (cf_info_base); */
  
  if ((db_errno = db_create(&db_handle, gb_dbenv, 0)) != 0) {
    gb_dbenv->err(gb_dbenv, db_errno,
		  "db_create failed to create handle\n");
    return(0);
  }
  
  db_handle->set_pagesize(db_handle, 8 * 1024);
  
  if (db_errno = db_handle->open(db_handle,
				 cf_info_base->file_name, NULL, DB_BTREE, 
				 DB_CREATE, 0664) != 0) {
    db_handle->err(db_handle, db_errno, 
		   "db_handle->open failed to create %s \n", 
		   cf_info_base->file_name);
  }

  /* should have opened the database file */

  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
	  
  keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
  keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
	  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&high_recno,dataval.data, sizeof(int));
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    dataval.size = sizeof(int);
    dataval.data = (void *)&high_recno;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 
    
  }

  keyval.data = (void *)"TOTAL_RECORD_SIZE";
  keyval.size = strlen("TOTAL_RECORD_SIZE");
	  
  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
  if (returncode == 0) { /* found the key in the index already */
    /* The database is set up */
    memcpy(&total_size,dataval.data, sizeof(int));
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new record not in the database */
    dataval.size = sizeof(int);
    dataval.data = (void *)&total_size;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 

  }

  if (high_recno > 0) {
    printf ("Appending to existing database file %s... start record #%d\n", 
	    cf_info_base->file_name, high_recno+1); 
    recno = high_recno;
  }
  
  tag = argv[argc-1];
  tag_length = strlen(tag);
  
  main_dtd = (SGML_DTD *) cf_getDTD(cf_info_base->file_name);
  if (main_dtd == NULL) {
    printf("Couldn't get main dtd in GetSGML\n");
    exit(0);
  }
  
  if (!recursive) {
    int seekresult, writeresult, flushresult;
    
    datafile = fopen(argv[2], fmread);

    while ((result = get_next_sgmlrec_data(&offset, argv[2], tag, 
					   tag_length, skipheaders)) != 0) {
      recno++;
      if (!quiet)
	printf ("Record #%08d: Length %08d\n",recno, result);
      
      /* record offset and length of sgml record in assoc file */
      assocrec.offset = offset ;
      assocrec.recsize = result;
      
      /* fetch, parse and store the record... */
      workrecord = CALLOC(char, result+10);
      
      /* if we get to here the record number must be OK */
      seekreturn = fseek(datafile, assocrec.offset, SEEK_SET);
      if (seekreturn != -1) {
	reclen = fread(workrecord, sizeof(char), 
		       assocrec.recsize, datafile);
	if (reclen == 0) {
	  printf("Read of data failed for %d\n", recno);
	  exit (0);
	}
	else {
	  /* got the record -- do the digest */
	  MD5Init(&mdcontext);
	  MD5Update(&mdcontext, workrecord, assocrec.recsize);
	  MD5Final(digestbuff, &mdcontext);
	  digestbuff[16] = '\0';
	  /* printf ("Digest for record %d is %s\n", recno, digestbuff); */
	  /* search for the digest record in the database */
	  /* Initialize the key/data pair so the flags aren't set. */
	  memset(&keyval, 0, sizeof(keyval));
	  memset(&dataval, 0, sizeof(dataval));
	  
	  keyval.data = (void *)digestbuff;
	  keyval.size = sizeof(digestbuff);
	  
	  returncode = db_handle->get(db_handle, NULL, &keyval, &dataval, 0); 
	  
	  if (returncode == 0) { /* found the word in the index already */
	    printf("DUPLICATE RECORD FOUND as record #%d\n", recno);
	    recno--;
	    continue;
	  }
	  else if (returncode == DB_NOTFOUND) { 
	    /* a new record not in the database */	    
	    /* set up the new key */
	    keyval.data = (void *)&recno;
	    keyval.size = sizeof(int);
	  
	    doc = sgml_parse_document(main_dtd, cf_info_base->file_name, workrecord,
				      recno, -1);
	    if (doc == NULL) {
	      printf("Document Parsing failed for record #%d\n", recno);
	      exit(1);
	    }

	    /* OK, we have the parsed document, now flatten it and write */
	    /* it to the database                                        */
	    outrec_length = 0;
	    outrec = flatten_sgml_document(doc, digestbuff,
					   &outrec_length, recno);
	    if (outrec == NULL) {
	      printf("Record conversion failed for record #%d\n", recno);
	      exit(1);
	    }
	    /* store the flattened document */
	    dataval.size = outrec_length;
	    total_size += outrec_length;
	    dataval.data = (void *)outrec;
	    /* put the new digest into the database */
	    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 
					DB_NOOVERWRITE); 
	    
	    
	    if (returncode == DB_KEYEXIST) {
	      printf("Document ID #%d already exists in the database\n", recno);
	    }

	    /* put the new digest into the database */
	    keyval.data = (void *)digestbuff;
	    keyval.size = sizeof(digestbuff);
	    dataval.size = sizeof(int);
	    dataval.data = (void *)&recno;
	    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 
				       DB_NOOVERWRITE); 

	    if (returncode == DB_KEYEXIST) {
	      printf("Document digest for record #%d already exists in the database\n", recno);
	    }

	    /* sync the database -- flushing all of the buffers to disk */
	    db_handle->sync(db_handle,0);

	    /* free the parsed document and the output record */
	    free_doc(doc);
	    FREE(outrec);
	  }
	}
      } 
      else {
	  printf("Seek to start of data failed for %d\n", recno);
	  exit (0);
      }

      FREE(workrecord);
    }
	  
    printf("End of File encountered after record %d\n", recno);

    get_next_sgmlrec_data(NULL,NULL,NULL, 0, 0); /* closes file */
    
    /* increment the zeroth record to record the */
    /* highest record number now in use          */
    high_recno = recno;
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
	  
    keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
    keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
    dataval.size = sizeof(int);
    dataval.data = (void *)&high_recno;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 

    keyval.data = (void *)"TOTAL_RECORD_SIZE";
    keyval.size = strlen("TOTAL_RECORD_SIZE");
    dataval.size = sizeof(int);
    dataval.data = (void *)&total_size;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 

    /* close the database -- flushing all of the buffers to disk */
    db_handle->close(db_handle,0);

    
  }
  else {
    /* Process a directory tree recursively looking for sgml tags */
    /* that match the one provided...                             */
    /* the filename provided should be a directory                */
    process_directory(db_handle, main_dtd, ContNamesHash, 
		      &recno, &cfilenum, &total_size,
		      argv[2], tag, tag_length, quiet, skipheaders);
    

    get_next_sgmlrec_data(NULL,NULL,NULL, 0, 0); /* closes file */
    
    /* increment the zeroth record to record the */
    /* highest record number now in use          */

    /* increment the zeroth record to record the */
    /* highest record number now in use          */
    high_recno = recno;
    memset(&keyval, 0, sizeof(keyval));
    memset(&dataval, 0, sizeof(dataval));
	  
    keyval.data = (void *)"MAXIMUM_RECORD_NUMBER";
    keyval.size = strlen("MAXIMUM_RECORD_NUMBER");
    dataval.size = sizeof(int);
    dataval.data = (void *)&high_recno;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 

    keyval.data = (void *)"TOTAL_RECORD_SIZE";
    keyval.size = strlen("TOTAL_RECORD_SIZE");
    dataval.size = sizeof(int);
    dataval.data = (void *)&total_size;
    /* put the high recno (0) into the database */
    returncode = db_handle->put(db_handle, NULL, &keyval, &dataval, 0); 

    /* close the database -- flushing all of the buffers to disk */
    db_handle->close(db_handle,0);


  }
  return 0;

}





