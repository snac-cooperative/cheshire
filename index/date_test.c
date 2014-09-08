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
*       Header Name:    date_test.c
*
*       Programmer:     Ray Larson 
*
*       Purpose:        drive date parser
*                      
*
*       Usage:          
*
*       Variables:
*
*       Return Conditions and Return Codes:
*
*       Date:           11/7/93
*       Revised:        11/28/93
*       Version:        0.99999999
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#define MAIN
/* #define TESTFORMATS */
#include "cheshire.h"
#include "configfiles.h"
#include "date_time.h"
#include <dmalloc.h>

int last_rec_proc = 0;


main (argc, argv)
int argc;
char *argv[];
{
  SGML_Data data;
  idx_list_entry idx;
  
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;

  date_time dbuf, *date_data;
  date_time_range drbuf, *date_range_data;

  char datestring[1000]; /* XXXX for simulated output from driver */
  
  int i;		  /* loop counter */
  char formatstring[100];	/* string to hold query */
  char *filename;
  char databuffer[1000];
  char typedata[20];
  char *word_ptr;
  int null_return = 1;

  if (argc > 3) {
    printf("usage %s (interactive prompting)\n", argv[0]);
    exit(0);
  }
  else {
    printf("FORMAT-> ");
    fflush(stdout);
    gets(formatstring);
    printf("date-> ");
    fflush(stdout);
    gets(datestring);
    printf("Date (D) or Range (R)");
    fflush(stdout);
    gets(typedata);
 
  }

  /* Allocate the hash table for collecting the words for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_STRING_KEYS);


  while (formatstring[0] != 0) {
    
    null_return = 1;
    /*   idxdate(SGML_Data data,
	    Tcl_HashTable *hash_tab, DB *index_db, idx_list_entry *idx) */
    data.processing_flags = 0;
    data.local_entities = 0;
    data.parent = NULL;
    data.sub_data_element = NULL;
    data.next_data_element = NULL;
    data.content_end = datestring + sizeof(datestring);
    data.content_start = datestring;
    idx.dateformat = formatstring;

    /* this is a cheap kludge for testing --               */
    if (typedata[0] == 'r' || typedata[0] == 'R') 
      idx.type = DATE_RANGE_KEY;
    else
      idx.type = DATE_KEY;

    idxdate(&data, hash_tab, &idx); 
    
    
    for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search); 
	 entry != NULL; 
	 entry = Tcl_NextHashEntry(&hash_search)) {
      null_return = 0;
      /* get the date key... */
      word_ptr = Tcl_GetHashKey(hash_tab,entry); 
      /* ignore the frequency info */
      /* ht_data =  (struct temp_dates *) Tcl_GetHashValue(entry); */
      memset(&dbuf, 0, sizeof(date_time));
      memset(&drbuf, 0, sizeof(date_time_range));

      
      if (idx.type == DATE_RANGE_KEY || strlen(word_ptr) > 24) {
	date_range_data = &drbuf;
	printf ("Hash key : %s\n", word_ptr);

	sscanf (word_ptr, "%d %d %d %d %d %d %d %d %d %d %d",
		&drbuf.start.dt_year, &drbuf.start.dt_month, 
		&drbuf.start.dt_day, 
		&drbuf.start.dt_hour, &drbuf.start.dt_minute, 
		&drbuf.end.dt_year, &drbuf.end.dt_month, 
		&drbuf.end.dt_day, 
		&drbuf.end.dt_hour, &drbuf.end.dt_minute, 
		&drbuf.scale );

	printf ("Scale %d\nSTART Year: %d, Month: %d, Day: %d, Hours: %d, Minutes: %d\n",
		date_range_data->scale, date_range_data->start.dt_year, 
		date_range_data->start.dt_month, date_range_data->start.dt_day,
		date_range_data->start.dt_hour, 
		date_range_data->start.dt_minute);
	printf ("END Year: %d, Month: %d, Day: %d, Hours: %d, Minutes: %d\n",
		date_range_data->end.dt_year, date_range_data->end.dt_month,
		date_range_data->end.dt_day,
		date_range_data->end.dt_hour, date_range_data->end.dt_minute);
      }
      else {
	date_data =  &dbuf;

	sscanf (word_ptr, "%d %d %d %d %d %d %d %d %d %d %d",
		&dbuf.dt_year, &dbuf.dt_month, 
		&dbuf.dt_day, 
		&dbuf.dt_hour, &dbuf.dt_minute);
 
	printf ("Hash key : %s\n", word_ptr);
	printf ("Year: %d, Month: %d, Day: %d, Hours: %d, Minutes: %d\n",
		date_data->dt_year, date_data->dt_month, date_data->dt_day,
		date_data->dt_hour, date_data->dt_minute);
      }
      if (null_return == 1)
	printf ("Date is invalid for the specified format\n");

      Tcl_DeleteHashEntry(entry);
    
    }   
    printf("FORMAT-> ");
    fflush(stdout);
    gets(formatstring);
    printf("date-> ");
    fflush(stdout);
    gets(datestring);
    
    /* clean up everything */
    
  }

}

/* dummy routines needed to link */
int ToLowerCase(char *c) {
  return(0);
}

int wninit()
{
  return 0;
}

int morphinit()
{
  return 0;
}

char *morphstr(char *x)
{
  return ("JUNK");
}

int in_wn(char *x)
{
  return 1;
}

