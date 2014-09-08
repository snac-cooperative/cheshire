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
*	Header Name:	dumpdb.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility for examining an index inverted file
*
*	Usage:		dumpdb configfile mainfilename indexfilename
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		Aug 08, 1997
*	Revised:	
*	Version:	2.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#define MAIN
#include "cheshire.h"
#include "configfiles.h"
#include "bitmaps.h"
/* global config file structure */
config_file_info *cf_info_base;

/* the following is needed for searches of allied indexes */
char *LastResultSetID;
int current_user_id;
 
int last_rec_proc = 0;

extern idx_list_entry *cf_getidx_entry(char *filename, char *indexname);
extern char *GetRawSGML(char *filename, int recordnum);
extern char *cf_getdatafilename(char *fname, int docid);

main (int argc, char *argv[])
{
  assoc_rec assocrec;
  char *crflags, *opflags;
  int i, recnum = 0;
  int file_position;
  char *xmlrec;
  char *actual_file_path;

#ifdef WIN32
  crflags = "rb";
  opflags = "rb";
#else
  crflags = "r";
  opflags = "r";
#endif

  LOGFILE = stderr;

  if (argc < 4) {
    printf ("usage: %s configfilename mainfile_name docno1 docno2... docnoN\n", argv[0]);
    exit (0);
  }

  /* the following line reads the config info from its file */
  cf_info_base = cf_initialize(argv[1], crflags, opflags);

  for (i = 3; i < argc; i++) {

    recnum = atoi(argv[i]);
    
    actual_file_path = cf_getdatafilename(argv[2], recnum);

    file_position = fil_seek (argv[2],recnum,&assocrec);
    
    printf ("\n*************************************************\n");
    printf ("*  Record #%d: Offset %d: Length %d: File: %s\n",recnum,
	    assocrec.offset, assocrec.recsize, actual_file_path);
    printf ("-------------------------------------------------\n");

    xmlrec = GetRawSGML(argv[2],recnum);

    printf ("%s\n", xmlrec);
 

  }

  cf_closeall();
  
}








