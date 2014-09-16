/*
 *  Copyright (c) 1990-2014 [see Other Notes, below]. The Regents of the
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
*       Header Name:    cp_driver.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive search engine in test mode for CheshirePy 
*                      
*
*       Usage:          testCheshirePy configfile db query
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           05/05/2014
*       Revised:        
*       Version:        0.1
*       Copyright (c) 2014.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include "CheshirePy.h"


void output_routine(char *line) {
  printf("%s",line);
}

main (argc, argv)
int argc;
char *argv[];
{
  int i;		  /* loop counter */
  char *cf_name;	  /* name of configuration file */
  char querystring[300];	/* string to hold query */
  char *filename;
  int num_request=0;
  int filetype;
  int argp = 1;
  char *result = NULL;
  char *diagnostic_string(), *diagnostic_addinfo();
  int resultlen;
  int num_hits;
  int rank;
  float relevance_score;

  weighted_result *final_set;	/* result from query processing */
  
  LOGFILE = stderr;

  num_request = 0;

  if (argc < 2) { 
    fprintf(LOGFILE, 
	    "Wrong number of args should be: %s [-n num_recs_wanted] configfile filename [query_string]\n", 
	    argv[0]);
    exit (1);
  }

  if ((argc > 2 && (argv[1][0] == '-' && argv[1][1] == 'n'))) {
    argp = 3;
    num_request = setnumwanted(atoi(argv[2]));
  }

  if (num_request == 0)
    num_request = setnumwanted(10);

  cf_name = argv[argp];

  init(cf_name);
  
  filename = argv[argp+1];

  setdb(filename);

  if (cf_info_base == NULL) {
    fprintf(LOGFILE, 
	    "bad config file ? should be: %s configfile filename query_string\n", 
	    argv[0]);
    exit (2);
  }
  
  filetype = cf_getfiletype(filename);
  querystring[0] = '\0';

  if (argc > 3) {/* this could fail */
    for (i=(argp+2); i<argc; i++) {
      strcat(querystring,argv[i]);
      strcat(querystring," ");
    }
  }
  else {
    fprintf(LOGFILE, 
	    "Wrong number of args should be: %s [-n num_recs_wanted] configfile filename query_string\n", 
	    argv[0]);
    exit (1);
  }

  final_set = Search(querystring);


  if (final_set != NULL) {

    num_request = shownumwanted();
    num_hits = getnumfound(final_set);

    for (i=0;i < num_request && i < num_hits; i++) {

      result = getrecord(final_set,i);
      rank = i+1;
      relevance_score = getrelevance(final_set,i);

      printf("***************************\nRank: %d Score: %f\nRecord: %s",
	     rank, relevance_score, result);
      
    }

    closeresult(final_set);

  }
  CheshireClose();
  
}

