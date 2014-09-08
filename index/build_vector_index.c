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
*	Header Name:	build_vector_index.c (main for index_vectors)
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining an index inverted file
*                       and building a corresponding vector file from it.
*
*	Usage:		index_vectors configfile mainfilename indexfilename
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		Aug 08, 1997
*	Revised:	
*	Version:	2.0
*	Copyright (c) 1993-7.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
/* #define DEBUGIND 1 */

#define MAIN
#include "cheshire.h"
#include "configfiles.h"
#include "bitmaps.h"
#include <sys/stat.h>

/* global config file structure */
config_file_info *cf_info_base;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;
int last_rec_proc = 0;

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);


int BuildVectors(idx_list_entry *idx, char *filename, int numrecs, 
		 char *temp_file_dir, char *sort_args)
{

  FILE *tempfile, *sorttempfile;
  DB *dbtest;
  DB *dbvect;
  DBT key, data;
  DBT veckey, vecdata;
  DBC *dbcursor;
  int bitblock_count = 0;
  bitblock_key block_key;
  vec_term_id termid;
  vec_term_overlay *term_overlay;
  vector_data_info *vector_data;
  vec_term_info *temp_term_info;
  dict_info tempdict;
  idx_result *newinfo=NULL, tempinfo;
  int i, startrec, nargs;
  int bufferlen = 0;
  char *tempfilename;
  char *sortfilename;
  char *tmp_buffer;
  struct stat filestatus, filestatus2;
  int statresult, statresult2;
  int recnum, termnum, termfreq, lastrecnum;
  char *fmode;
  double sumtfidf, idf;
  float tfidf;
  float sumtf;
  int maxtf;
  int count_terms, entry;
  int temp_term_index=0;
  int recsoutput = 0;
  char *local_sort_args; 

  


  if (idx == NULL)
    return (0);
    
#ifdef DEBUGIND
  printf("index tag = %s\n", idx->tag);
#endif
  fprintf(LOGFILE, "Building Vectors for index tag = %s\n", idx->tag);
  fflush(LOGFILE);
    
  dbtest = (DB *) cf_index_open(filename, idx->tag, INDEXFL);

  dbvect = (DB *) cf_index_open(filename, idx->tag , INDEXVECTORFL);

  if (dbtest == NULL) {
    fprintf(LOGFILE, "Unable to open index: %s\n", idx->tag);
    exit(1);
  }
  
  tempfilename = CALLOC(char,strlen(idx->name)+20);
  sortfilename = CALLOC(char,strlen(idx->name)+20);
  bufferlen = (2*strlen(idx->name))+200;
  tmp_buffer = CALLOC(char, bufferlen);

  sprintf(tempfilename,"%s_VECTOR_TEMP",idx->name);
  sprintf(sortfilename,"%s_VECTOR_SORT",idx->name);
  
    tempfile = fopen(tempfilename,"w"); /* open the tempfile to write */
  
    /* Acquire a cursor for the index. */
    if ((errno = dbtest->cursor(dbtest, NULL, &dbcursor, 0)) != 0) {
      fprintf(stderr, "dumpdb: cursor: %s\n", strerror(errno));
      exit (1);
    }

    /* Initialize the key/data pair so the flags aren't set. */
    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    /* Walk through the database and put out the key/data pairs. */
    while ((errno = dbcursor->c_get(dbcursor, &key, &data, DB_NEXT)) == 0) {

      if (idx->type & BITMAPPED_TYPE && data.size == bitmap_blocksize_bytes) {
	continue; /* ignore bitmapped indexes for now... */
	bitblock_count++;
	memcpy((char *)&block_key,key.data,key.size);
	printf("Bitblock key: Block %d, Termid %d\n", block_key.blockid, block_key.termid);
	continue;
      }
      else if (idx->type & BITMAPPED_TYPE) {
	continue; /* ignore bitmapped indexes for now... */
	if (data.size < sizeof(idx_result)) {
	  memcpy((char *)&tempdict,data.data,data.size);
	}
	else {
	  printf("key: %s ", (char *)key.data);
	  
	  /* allocate space for the posting info */
	  newinfo = (idx_result *) 
	    CALLOC(int, (data.size + sizeof(idx_posting_entry))/sizeof(int));
	  memcpy((char *)newinfo, (char *)data.data, data.size);
	  
	  printf("termid %d, tot_freq %d, num_entries %d\n",
		 newinfo->termid, newinfo->tot_freq, 
		 newinfo->num_entries);
	  
	  printf("\tPOSTINGS DATA FOR BITMAPS NOT YET AVAILABLE\n");
	}
      }
      else {
	/* Here is where the normal indexes are read */
	if (data.size < sizeof(idx_result)) {
	  /* this is the global data record */
	  memcpy((char *)&tempdict,data.data,data.size);
	}
	else {
	  /*printf("key: %s ", (char *)key.data); */
	  
	  /* allocate space for the posting info */
	  newinfo = (idx_result *) 
	    CALLOC(int, (data.size + sizeof(idx_posting_entry))/sizeof(int));
	  memcpy((char *)newinfo, (char *)data.data, data.size);
	  
	  /*printf("termid %d, tot_freq %d, tot_records %d\n",
	   *	 newinfo->termid, newinfo->tot_freq, 
	   *	 newinfo->num_entries);
	   */
	  /* output the temp information for each document/term pair */
	  
	  idf = log((double)numrecs/(double)newinfo->num_entries);

	  for (i = 0; i < newinfo->num_entries; i++) {
	    fprintf(tempfile, "%d %d %d %f\n",
		    newinfo->entries[i].record_no, 
		    newinfo->termid,
		    newinfo->entries[i].term_freq,
		    (float)((double)newinfo->entries[i].term_freq * idf));
	  }
	  
	  /* output new termid/term mapping entry for this termid */
	  memset(&veckey, 0, sizeof(veckey));
	  memset(&vecdata, 0, sizeof(vecdata));
	  memset(&termid, 0, sizeof(vec_term_id));
	  termid.id = newinfo->termid;
	  termid.code = 'T';
	  veckey.size = sizeof(termid);
	  veckey.data = &termid;

	  term_overlay = (vec_term_overlay *)CALLOC(char, sizeof(int)+key.size+1);
	  term_overlay->coll_freq = newinfo->num_entries;
	  strncpy(term_overlay->term, key.data, key.size); 
	  vecdata.size = sizeof(int)+key.size+1;
	  vecdata.data = term_overlay;
	  dbvect->put(dbvect,NULL, &veckey, &vecdata,DB_NOOVERWRITE);

	}
      }
     
      /* Clean up -- this was a mem leak */
      if (newinfo)
	FREE(newinfo);
      newinfo = NULL;

    }
    
    /* close the temp file  and have the OS sort it ...*/
    fclose(tempfile);

    /* close the DB cursor */
    (void)dbcursor->c_close(dbcursor);

    if (sort_args == NULL)
      local_sort_args = "";
    else
      local_sort_args = sort_args;
    
    if (temp_file_dir) {
      
#ifndef WIN32
      sprintf(tmp_buffer,"sort -T %s -n %s %s > %s",
	      temp_file_dir, local_sort_args, tempfilename, sortfilename);
#else
      sprintf(tmp_buffer,"sort -n %s < \"%s\" > \"%s\"",
	      local_sort_args, tempfilename, sortfilename);
#endif
    }
    else
#ifndef WIN32 
      sprintf(tmp_buffer,"sort -n %s %s > %s",
	      local_sort_args, tempfilename, sortfilename);
#else
      sprintf(tmp_buffer,"sort -n %s < \"%s\" > \"%s\"",
	      local_sort_args, tempfilename, sortfilename);

#endif
    fprintf(LOGFILE,"SORT COMMAND: %s\n", tmp_buffer);
    fflush(LOGFILE);

    system(tmp_buffer);

    /* check to see if the sorted file and source file are the same size...*/
    statresult = stat(sortfilename, &filestatus);
    statresult2 = stat(tempfilename, &filestatus2);
    if (statresult == 0 && statresult2 == 0 
	&& filestatus.st_size == filestatus2.st_size) {
      /* files are the same size -- OK to delete the temp file and proceed */
      unlink(tempfilename);
    }
    else {
      if (statresult == 0 && statresult2 == 0 
	  && filestatus.st_size > filestatus2.st_size) {
	fprintf(LOGFILE,"ERROR:Sorted file size and temp size do not match\n");
	exit(1);
      }  
    }
    
      
    /* now we read in each record and accumulate those with the same */
    /* index key.                                                    */
#ifdef WIN32
    fmode = "rb";
#else 
    fmode = "r";
#endif

    sorttempfile = fopen(sortfilename, fmode);
  
    if (sorttempfile == NULL) {
      fprintf(LOGFILE,"sort file is null in in_batch_load \n");
      fflush(LOGFILE);
      cf_closeall();
      exit(1);
    }

    /* now we read in each record and put out a vector entry */
    lastrecnum = 0;
    sumtfidf = 0.0;

    temp_term_info = CALLOC(vec_term_info, idx->GlobalData->recptr + 1 );
    memset(temp_term_info, 0, (idx->GlobalData->recptr * sizeof(vec_term_info)));
    count_terms = 0;

    sumtf = 0.0;
    maxtf = 0;

    


    while (fgets(tmp_buffer,bufferlen,sorttempfile)) {

      if (strlen(tmp_buffer) > bufferlen) {
	fprintf(LOGFILE,
		"Extracted term input line exceeds buffer size (%d) in index_vectores: \n %s \n\n",
		bufferlen, tmp_buffer);
	fflush(LOGFILE);
	continue;
      }
      
      sscanf(tmp_buffer,"%d %d %d %f", 
	     &recnum, &termnum, &termfreq, &tfidf);
      
      if (recnum != lastrecnum) {
	/* output the data */

	if (lastrecnum == 0) {
	  
	  lastrecnum = recnum;
	  temp_term_info[0].termid = termnum;
	  temp_term_info[0].termfreq = termfreq;
	  sumtfidf = (double)tfidf * (double)tfidf;
	  count_terms = 1;
	  sumtf = (float)termfreq;
	  maxtf = termfreq;
	}
	else {
	  /* really output the data */
	  vector_data = NULL;
	  vector_data = (vector_data_info *) 
	    CALLOC(int, (sizeof(vector_data_info) 
			 / sizeof(int))+100+(2*count_terms));
	  if (vector_data == NULL) {
	    fprintf(LOGFILE,
		    "\n Unable to allocate vector for %d terms in build_vector_index\n\n",
		    count_terms);
	    fflush(LOGFILE);
	    exit(1);
	  }
	  entry = 0;
	  vector_data->num_terms = count_terms;
	  vector_data->sqrt_sum_tfidf_sq = (float) sqrt(sumtfidf);
	  vector_data->avg_term_freq = sumtf/(float)count_terms ;
	  vector_data->max_term_freq = maxtf;

	  for (i=0; i < count_terms; i++) {
	    if (temp_term_info[i].termid != 0) {
	      vector_data->entries[entry].termid = temp_term_info[i].termid;
	      vector_data->entries[entry].termfreq = temp_term_info[i].termfreq;
	      entry++;
	    }
	  }
	  /* now output the data */
	  memset(&veckey, 0, sizeof(veckey));
	  memset(&vecdata, 0, sizeof(vecdata));
	  veckey.size = sizeof(int);
	  veckey.data = &lastrecnum;
	  vecdata.size = (sizeof(vector_data_info)+((1+(2*count_terms))*sizeof(int)));
	  vecdata.data = vector_data;
	  dbvect->put(dbvect,NULL, &veckey, &vecdata,DB_NOOVERWRITE);

	  recsoutput++;
	  if (recsoutput % 1000 == 0) {
	    fprintf(LOGFILE,"%d vectors written\n", recsoutput);
	    fflush(LOGFILE);
	  }
	  if (vector_data != NULL)
	    FREE(vector_data);
	  
	  
	  /* clear the temp_term_info array for the next*/
	  memset(temp_term_info, 0, ((count_terms+1) * sizeof(vec_term_info)));

	  lastrecnum = recnum;	  
	  sumtfidf = (double)tfidf * (double)tfidf;
	  count_terms = 1;
	  temp_term_info[0].termid = termnum;
	  temp_term_info[0].termfreq = termfreq;
	  sumtf = (float)termfreq;
	  maxtf = termfreq;
	}
      }
      else {
	/* same record, so keep adding things */
	temp_term_info[count_terms].termid = termnum;
	temp_term_info[count_terms].termfreq = termfreq;
	sumtfidf += (double)tfidf * (double)tfidf;
	sumtf += (float)termfreq;
	count_terms++;
	if (maxtf < (float)termfreq) 
	  maxtf = (float)termfreq;
      }
    }

    /* output the LAST vector */
    vector_data = (vector_data_info *) 
      CALLOC(int, (sizeof(vector_data_info) 
		   / sizeof(int))+1+(2*count_terms));
    entry = 0;
    vector_data->num_terms = count_terms;
    vector_data->sqrt_sum_tfidf_sq = (float) sqrt(sumtfidf);
    vector_data->avg_term_freq = sumtf/(float)count_terms ;
    vector_data->max_term_freq = maxtf;


    for (i=0; i < count_terms; i++) {
      if (temp_term_info[i].termid != 0) {
	vector_data->entries[entry].termid = temp_term_info[i].termid;
	vector_data->entries[entry].termfreq = temp_term_info[i].termfreq;
	entry++;
      }
    }

    /* now output the data */
    memset(&veckey, 0, sizeof(veckey));
    memset(&vecdata, 0, sizeof(vecdata));
    veckey.size = sizeof(int);
    veckey.data = &lastrecnum;
    vecdata.size = (sizeof(vector_data_info)+((1+(2*count_terms))*sizeof(int)));
    vecdata.data = vector_data;
    dbvect->put(dbvect,NULL, &veckey, &vecdata,DB_NOOVERWRITE);
    
    FREE(vector_data);
    recsoutput++;
	  
    /* close this index and vector file */
    cf_index_close(filename, idx->tag, INDEXFL);

    fclose(sorttempfile);
    unlink(sortfilename);

    FREE(temp_term_info);
    FREE(tempfilename);
    FREE(sortfilename);
    FREE(tmp_buffer);
 
    return(recsoutput);

}




main (int argc, char *argv[])
{
  int numrecs, nargs, templen;
  char *temp_file_dir = NULL;
  char *sort_args = NULL;
  char *local_sort_args = NULL;
  char *log_file_name = NULL;
  char *progname;
  time_t startime;
  char *crflags, *opflags;
  char *filename=NULL;
  int recsoutput=0;
  idx_list_entry *idx;
  component_list_entry *comp;


#ifdef WIN32
  crflags = "w+b";
  opflags = "r+b";
#else
  crflags = "w+";
  opflags = "r+";
#endif

  progname = argv[0];

  if (argc < 2) {
    printf ("usage: %s <-T tmpdir -S \"sort flags\" -L logfilename> configfilename\n", progname);
    exit (0);
  }


  /* set the starting time */
  time(&startime);

  nargs = argc;

  while (nargs > 0) {
      /* the -T option is only technically available within -b and 
	 gives the temp directory */
    if (argv[1][0] == '-' && argv[1][1] == 'T') {
      temp_file_dir = argv[2];
      templen += strlen(temp_file_dir) + 1;
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'S') {
      sort_args = argv[2];
      templen += strlen(sort_args) + 1;
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-' && argv[1][1] == 'L') {
      log_file_name = argv[2];
      nargs -= 2;
      argc -= 2;
      argv += 2;
    }
    else if (argv[1][0] == '-') {
      printf ("usage: %s <-T tmpdir -S \"sort flags\" -L logfilename configfilename \n", progname);
      exit (0);
    }
    else
      nargs-- ;
  }

  /* set some GLOBAL stuff... */
  if (log_file_name)
    LOGFILE = fopen(log_file_name,"a+"); /* open the logfile to append */
  else
    LOGFILE = fopen("INDEX_LOGFILE","a+"); /* open the logfile to append */

  /* the following reads and parses the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  if (cf_info_base == NULL) {
    printf("could not initialize configfile %s\n",argv[1]);
    exit(1);
  }

      
  fprintf(LOGFILE,
	  "\nVector Index Run Date: %s \nfile: %s (%s)\n",
	  ctime(&startime), 
	  cf_info_base->file_name, cf_info_base->nickname);
  fflush(LOGFILE);

  filename = cf_info_base->file_name;

  numrecs = cf_getnumdocs(filename);

  
  for (idx = cf_info_base->indexes ; idx != NULL ; idx = idx->next_entry) {

    if ((idx->type & VECTOR_TYPE) == 0) 
      continue;    

    recsoutput += BuildVectors(idx, filename, numrecs, 
			       temp_file_dir, sort_args);

    
  }

  /* Handle the component indexes too... */

  for (comp = cf_info_base->components; comp; comp = comp->next_entry) {

    for (idx = comp->indexes; idx != NULL ; idx = idx->next_entry) {

      if ((idx->type & VECTOR_TYPE) == 0) 
	continue;    

      recsoutput += BuildVectors(idx, filename, numrecs, 
			       temp_file_dir, sort_args);

     
    } 
    
  }


  time(&startime);

  fprintf(LOGFILE,"Last vector processed %d at time: %s -- ", 
	  recsoutput, ctime(&startime)); 

  cf_closeall();
  
}








