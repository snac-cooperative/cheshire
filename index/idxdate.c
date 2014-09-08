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
*	Header Name:  idxdate.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing formatted dates in an SGML field.
*
*	Usage:	      idxdate(data, stoplist_hash, hash_tab,
*                             GlobalData, index_db, index_type);
*
*	Variables:    SGML_data data, Tcl_HashTable *stoplist_hash,
*                     Tcl_HashTable *hash_tab, dict_info *GlobalData,
*                     DB *index_db, int index_type
*
*	Return Conditions and Return Codes:	
*
*	Date:		09/21/98
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1998.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "configfiles.h"
#include "dmalloc.h"
#include "date_time.h"
#include "ht_info.h"
#include "ctype.h"

char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);

  /***************************************************
   * Defined formats for date_times and date ranges
   * from cf_parse.flex

   "YYMMDD" DATE_NORMALIZE
   "YYYYMMDD" DATE_NORMALIZE
   "YYYY-MM-DD" DATE_NORMALIZE
   "YYYY/MM/DD" DATE_NORMALIZE
   "YYYY.MM.DD" DATE_NORMALIZE
   "MM/DD/YY" DATE_NORMALIZE
   "MM/DD/YYYY" DATE_NORMALIZE
   "DD/MM/YY" DATE_NORMALIZE
   "DD/MM/YYYY" DATE_NORMALIZE
   "MM.DD.YY" DATE_NORMALIZE
   "MM.DD.YYYY" DATE_NORMALIZE
   "DD.MM.YY" DATE_NORMALIZE
   "DD.MM.YYYY" DATE_NORMALIZE
   "DD MMM YYYY" DATE_NORMALIZE
   "DD MMM YY" DATE_NORMALIZE
   "DD MONTH YY" DATE_NORMALIZE
   "DD MONTH YYYY" DATE_NORMALIZE
   "MONTH DD, YY" DATE_NORMALIZE
   "MONTH DD, YYYY" DATE_NORMALIZE
   "YYMMDD HH:MM" DATE_NORMALIZE
   "YYYYMMDD HH:MM" DATE_NORMALIZE
   "MM/DD/YY HH:MM" DATE_NORMALIZE
   "MM/DD/YYYY HH:MM" DATE_NORMALIZE
   "DD/MM/YY HH:MM" DATE_NORMALIZE
   "DD/MM/YYYY HH:MM" DATE_NORMALIZE
   "MM.DD.YY HH:MM" DATE_NORMALIZE
   "MM.DD.YYYY HH:MM" DATE_NORMALIZE
   "DD.MM.YY HH:MM" DATE_NORMALIZE
   "DD.MM.YYYY HH:MM" DATE_NORMALIZE
   "YYYY" DATE_NORMALIZE
   "YYDDD" DATE_NORMALIZE
   "YEAR" DATE_NORMALIZE
   "DECADE" DATE_NORMALIZE
   "CENTURY" DATE_NORMALIZE
   "MILLENNIUM" DATE_NORMALIZE
   "MIXED YEAR" DATE_NORMALIZE
   "MIXED_YEAR" DATE_NORMALIZE
   "UNIX_TIME" DATE_NORMALIZE
   "UNIX_CTIME" DATE_NORMALIZE
   "DAY MONTH DD HH:MM:SS YEAR" DATE_NORMALIZE
   "DAY MMM DD HH:MM:SS YYYY" DATE_NORMALIZE

   "YYYY-YYYY" DATE_RANGE_NORM
   "YYYY to YYYY" DATE_RANGE_NORM
   "YYYY through YYYY" DATE_RANGE_NORM
   "YEAR-YEAR" DATE_RANGE_NORM
   "YEAR to YEAR" DATE_RANGE_NORM
   "YEAR through YEAR" DATE_RANGE_NORM
   "DECADE-DECADE" DATE_RANGE_NORM
   "DECADE to DECADE" DATE_RANGE_NORM
   "DECADE through DECADE" DATE_RANGE_NORM
   "CENTURY-CENTURY" DATE_RANGE_NORM
   "CENTURY to CENTURY" DATE_RANGE_NORM
   "CENTURY through CENTURY" DATE_RANGE_NORM
   "MILLENNIUM-MILLENNIUM" DATE_RANGE_NORM
   "MILLENNIUM to MILLENNIUM" DATE_RANGE_NORM
   "MILLENNIUM through MILLENNIUM" DATE_RANGE_NORM
   "MIXED YEAR RANGE" DATE_RANGE_NORM
   "MIXED_YEAR_RANGE" DATE_RANGE_NORM

   "\"YYMMDD\"" DATE_NORMALIZE
   "\"YYYYMMDD\"" DATE_NORMALIZE
   "\"YYYY-MM-DD\"" DATE_NORMALIZE
   "\"YYYY/MM/DD\"" DATE_NORMALIZE
   "\"YYYY.MM.DD\"" DATE_NORMALIZE
   "\"MM/DD/YY\"" DATE_NORMALIZE
   "\"MM/DD/YYYY\"" DATE_NORMALIZE
   "\"DD/MM/YY\"" DATE_NORMALIZE
   "\"DD/MM/YYYY\"" DATE_NORMALIZE
   "\"MM.DD.YY\"" DATE_NORMALIZE
   "\"MM.DD.YYYY\"" DATE_NORMALIZE
   "\"DD.MM.YY\"" DATE_NORMALIZE
   "\"DD.MM.YYYY\"" DATE_NORMALIZE
   "\"DD MMM YYYY\"" DATE_NORMALIZE
   "\"DD MMM YY\"" DATE_NORMALIZE
   "\"DD MONTH YY\"" DATE_NORMALIZE
   "\"DD MONTH YYYY\"" DATE_NORMALIZE
   "\"MONTH DD, YY\"" DATE_NORMALIZE
   "\"MONTH DD, YYYY\"" DATE_NORMALIZE
   "\"YYMMDD HH:MM\"" DATE_NORMALIZE
   "\"YYYYMMDD HH:MM\"" DATE_NORMALIZE
   "\"MM/DD/YY HH:MM\"" DATE_NORMALIZE
   "\"MM/DD/YYYY HH:MM\"" DATE_NORMALIZE
   "\"DD/MM/YY HH:MM\"" DATE_NORMALIZE
   "\"DD/MM/YYYY HH:MM\"" DATE_NORMALIZE
   "\"MM.DD.YY HH:MM\"" DATE_NORMALIZE
   "\"MM.DD.YYYY HH:MM\"" DATE_NORMALIZE
   "\"DD.MM.YY HH:MM\"" DATE_NORMALIZE
   "\"DD.MM.YYYY HH:MM\"" DATE_NORMALIZE
   "\"YYYY\"" DATE_NORMALIZE
   "\"YEAR\"" DATE_NORMALIZE
   "\"YYDDD\"" DATE_NORMALIZE
   "\"DECADE\"" DATE_NORMALIZE
   "\"CENTURY\"" DATE_NORMALIZE
   "\"MILLENNIUM\"" DATE_NORMALIZE
   "\"MIXED YEAR\"" DATE_NORMALIZE
   "\"MIXED YEAR\"" DATE_NORMALIZE
   "\"UNIX_TIME\"" DATE_NORMALIZE
   "\"UNIX_CTIME\"" DATE_NORMALIZE
   "\"DAY MONTH DD HH:MM:SS YEAR\"" DATE_NORMALIZE
   "\"DAY MMM DD HH:MM:SS YYYY\"" DATE_NORMALIZE

   "\"YYYY-YYYY\"" DATE_RANGE_NORM
   "\"YYYY to YYYY\"" DATE_RANGE_NORM
   "\"YYYY through YYYY\"" DATE_RANGE_NORM
   "\"YEAR-YEAR\"" DATE_RANGE_NORM
   "\"YEAR to YEAR\"" DATE_RANGE_NORM
   "\"YEAR through YEAR\"" DATE_RANGE_NORM
   "\"DECADE-DECADE\"" DATE_RANGE_NORM
   "\"DECADE to DECADE\"" DATE_RANGE_NORM
   "\"DECADE through DECADE\"" DATE_RANGE_NORM
   "\"CENTURY-CENTURY\"" DATE_RANGE_NORM
   "\"CENTURY to CENTURY\"" DATE_RANGE_NORM
   "\"CENTURY through CENTURY\"" DATE_RANGE_NORM
   "\"MILLENNIUM-MILLENNIUM\"" DATE_RANGE_NORM
   "\"MILLENNIUM to MILLENNIUM\"" DATE_RANGE_NORM
   "\"MILLENNIUM through MILLENNIUM\"" DATE_RANGE_NORM
   "\"MIXED YEAR RANGE\"" DATE_RANGE_NORM
   "\"MIXED_YEAR_RANGE\"" DATE_RANGE_NORM
   There are a bunch more now too -- range version of the single date 
   formats...
   ********************************************/   

char *parse_date(char *databuf, char *patptr);
char *parse_mixed(char *databuf, int range);

int
monthbyname(char *name)
{
  int month = 0;

  switch (name[0]) {
    
  case 'J':
    switch (name[1]) {
    case 'A': month = 1;
      break;
    case 'U': 
      switch (name[2]) {
      case 'L': month = 7;
	break;
      case 'N': month = 6;
	break;
      }
    }
    break;

  case 'F':
    if (name[1] == 'E' && name[2] == 'B') month = 2;
    break;
    
  case 'M':
    if (name[2] == 'R') month = 3;
    else if (name[2] == 'Y') month = 5;

  case 'A':
    if (name[1] == 'P' && name[2] == 'R') month = 4;
    else if (name[1] == 'U' && name[2] == 'G') month = 8;
    break;

  case 'S':
    if (name[1] == 'E' && name[2] == 'P') month = 9;
    break;

  case 'O':
    if (name[1] == 'C' && name[2] == 'T') month = 10;
    break;

  case 'N':
    if (name[1] == 'O' && name[2] == 'V') month = 11;
    break;

  case 'D':
    if (name[1] == 'E' && name[2] == 'C') month = 12;
    break;

  }

  return (month);

}

/* getEra -- read past whitespace and check for Era indication for years */
/* returns negative 1 for B.C.E., BC, BCE, B.C. and positive 1 otherwise */
/* updates the input pointer to point to the first significant character */
/* following the era.                                                    */
int 
getEra(char **in, int negyear) 
{
  char *ptr = *in;
  int result = 1;

  if (in == NULL || *in == NULL) {
    return (result);
  }

  if (*ptr == '\0')
    return (1 * negyear);

  while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n'
	     || *ptr == '"' || *ptr == ':' || *ptr == '/' 
	     || *ptr == '.') ptr++;

  if (toupper(*ptr) == 'B') {
    if ((toupper(*(ptr+1)) == '.' && toupper(*(ptr+2)) == 'C') 
	|| toupper(*(ptr+1)) == 'C')
      result = -1;
  }
  /* remove the era */
  while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n'
	 || *ptr == '"' || *ptr == ':' || *ptr == '/' 
	 || *ptr == '.' || toupper(*ptr) == 'A' || toupper(*ptr) == 'D'
	 || toupper(*ptr) == 'B' || toupper(*ptr) == 'C' 
	 || toupper(*ptr) == 'E') ptr++;

  *in = ptr;
  return (result);

}


int 
idxdateattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  char *end_buffer, *patptr, *date_key, *get_attr_value();
  int buflen;
  int *exists;

  HT_DATA *ht_data;

  char *breakletters = " \t\n\r\a\b\v\f/:.-";

  databuf = get_attr_value(data, attr_name);

  if (databuf == NULL) return(0);

  buflen = strlen(databuf);

  if (buflen == 0) return(0);

  end_buffer = databuf + buflen;

  patptr = idx->dateformat;

  if (patptr == NULL)
    return(0); /* maybe should have some default...*/


  /* create the hash table structure */
  ht_data = CALLOC(struct hash_term_data, 1) ;
  ht_data->termid = -1;
  ht_data->termfreq = 1;

  date_key = parse_date(databuf, patptr);

  /* put it into the hash table */
  if (date_key != NULL)
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 date_key, &exists), 
		     ht_data);
  
  
  if (date_key)
    FREE(date_key);

  FREE(databuf);
  return(0);
}


int idxdatestring (char *databuf, 
		   Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  char *end_buffer, *patptr, *date_key;
  int buflen;
  int *exists;
  HT_DATA *ht_data;

  if (databuf == NULL) return(0);

  buflen = strlen(databuf);

  if (buflen == 0) return (0);
  
  end_buffer = databuf + buflen;
  
  patptr = idx->dateformat;
  
  if (patptr == NULL)
    return(0); /* maybe should have some default...*/
  

  /* create the hash table structure */
  ht_data = CALLOC(struct hash_term_data, 1) ;
  ht_data->termid = -1;
  ht_data->termfreq = 1;
  
  date_key = parse_date(databuf, patptr);
  
  /* put it into the hash table */
  if (date_key != NULL)
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 date_key, &exists), 
		     ht_data);
  
  
  if (date_key)
    FREE(date_key);
  
  return(0);

}

int 
idxdate(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  char *end_buffer, *patptr, *date_key;
  int buflen;
  int *exists;
  HT_DATA *ht_data;

  char *breakletters = " \t\n/:.-";

  databuf = detag_data_block(data, idx->type, idx);

  if (databuf == NULL) return(0);

  buflen = strlen(databuf);

  if (buflen == 0) {
    FREE(databuf);
    return (0);
  }

  end_buffer = databuf + buflen;

  patptr = idx->dateformat;

  if (patptr == NULL) {
    FREE(databuf);
    return(0); /* maybe should have some default...*/
  }

  /* create the hash table structure */
  ht_data = CALLOC(struct hash_term_data, 1) ;
  ht_data->termid = -1;
  ht_data->termfreq = 1;

  date_key = parse_date(databuf, patptr);

  /* put it into the hash table */
  if (date_key != NULL)
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 date_key, &exists), 
		     ht_data);
  
  
  if (date_key)
    FREE(date_key);


  FREE(databuf);
  return(0);
}

char *parse_mixed(char *databuf, int range)
{
  char *cptr, *endptr;
  char tmp[100];

  char *result_string;
  int numlen;
  date_time_range daterangedata;
  int year, year2;
  
  if (databuf == NULL)
    return NULL;

  memset(&daterangedata, 0, sizeof(date_time_range));

  tmp[0] = '\0';
  numlen = 0;
  cptr = databuf;
  endptr = databuf + strlen(databuf);
  
  /* we scan for 3 and 4 digit sequences as the year */

  while (cptr != NULL && numlen < 3) {
    cptr = strpbrk(cptr,"0123456789");
    if (cptr) {
      numlen = strspn(cptr,"0123456789");
      if (numlen == 4) {
	strncpy(tmp,cptr,4);
	tmp[4] = '\0';
      }
      if (numlen == 3) {
	strncpy(tmp,cptr,3);
	tmp[3] = '\0';
      }
      cptr += numlen;
      if (cptr >= endptr)
	cptr = NULL;
    }
  }
  
  year = atoi(tmp) * getEra(&cptr, 1);

  if (year == 0) 
    return (NULL);

  if (range) {
    daterangedata.start.dt_year = year;

    tmp[0] = '\0';
    numlen = 0;

    while (cptr != NULL && numlen < 3) {
      cptr = strpbrk(cptr,"0123456789");
      if (cptr) {
	numlen = strspn(cptr,"0123456789");
	if (numlen == 4) {
	  strncpy(tmp,cptr,4);
	  tmp[4] = '\0';
	}
	if (numlen == 3) {
	  strncpy(tmp,cptr,3);
	  tmp[3] = '\0';
	}
	cptr += numlen;
	if (cptr >= endptr)
	  cptr = NULL;
      }
    }
  
    year2 = atoi(tmp) * getEra(&cptr, 1);

    if (year2 == 0) {
      daterangedata.end.dt_year = year;      
    }
    else
      daterangedata.end.dt_year = year2;

    daterangedata.end.dt_month = 12;
    daterangedata.end.dt_day = 31;
    daterangedata.end.dt_hour = 23;
    daterangedata.end.dt_minute = 59;

    result_string = CALLOC(char, 60);

    sprintf(result_string,
	    "%010d %02d %02d %02d %02d %010d %02d %02d %02d %02d %05d",
	    daterangedata.start.dt_year, daterangedata.start.dt_month, 
	    daterangedata.start.dt_day, daterangedata.start.dt_hour, 
	    daterangedata.start.dt_minute, 
	    daterangedata.end.dt_year, daterangedata.end.dt_month, 
	    daterangedata.end.dt_day,
	    daterangedata.end.dt_hour, daterangedata.end.dt_minute,  
	    daterangedata.scale);
    
  }
  else {
    /* not range */
    result_string = CALLOC(char, 60);
    sprintf(result_string, "%010d %02d %02d %02d %02d", 
	    year, 0, 0, 0, 0);
    
  }
  
  return(result_string);

}

char do_wild(char in, int end_range) 
{
  if (isdigit(in) || in == '-')
    return (in);
  else {
    if (end_range && (in == 'X' || in == 'x'))
      return ('9');
    else 
      return ('0');
  }
}

char *
parse_date(char *databuf, char *patptr)
{
  
  char *cptr;
  char tmppat[100] = "DAY MMM DD HH:MM:SS YYYY";
  char tmp[100];
  char *result_string;
  int i, j;
  int temp_era;
  int year=0, month=0, day=0, hours=0, minutes=0;
  int date_time_flag = 0, range_flag = 0;
  int millennium_flag = 0, century_flag = 0, decade_flag = 0;
  int through_flag = 0;
  int ISO = 0; /* flag for ISO style dates */
  int unix_time = 0; /* flag for unix time style dates */
  int negyear = 1;
  date_time datedata;
  date_time_range daterangedata;
  
  if (databuf == NULL) return(0);

  
  while (*databuf == ' ' || *databuf == '\t' || *databuf == '\n') databuf++;
  
  if (patptr == NULL)
    return(NULL); /* maybe should have some default...*/
  
  for (i = 0; patptr[i] != '\0'; i++)
    patptr[i] = toupper(patptr[i]);


  if (strcmp(patptr, "MIXED YEAR") == 0 
      || strcmp(patptr, "MIXED_YEAR") == 0 
      || strcmp(patptr, "\"MIXED YEAR\"") == 0 
      || strcmp(patptr, "\"MIXED_YEAR\"") == 0)
    return (parse_mixed(databuf,0));

  if (strcmp(patptr, "MIXED YEAR RANGE") == 0 
      || strcmp(patptr, "MIXED_YEAR_RANGE") == 0 
      || strcmp(patptr, "\"MIXED YEAR RANGE\"") == 0 
      || strcmp(patptr, "\"MIXED_YEAR_RANGE\"") == 0)
    return (parse_mixed(databuf,1));

  if (strcmp(patptr, "UNIX_TIME") == 0 
      || strcmp(patptr, "UNIX_CTIME") == 0 
      || strcmp(patptr, "UNIX TIME") == 0 
      || strcmp(patptr, "UNIX CTIME") == 0 
      || strcmp(patptr, "\"UNIX_TIME\"") == 0 
      || strcmp(patptr, "\"UNIX_CTIME\"") == 0 
      || strcmp(patptr, "\"UNIX TIME\"") == 0 
      || strcmp(patptr, "\"UNIX CTIME\"") == 0) {
    patptr = tmppat;
    unix_time = 1;
  }


  if (strcmp(patptr, "YYYY-MM-DD") == 0 
      || strcmp(patptr, "YY-MM-DD") == 0 
      || strcmp(patptr, "\"YYYY-MM-DD\"") == 0 
      || strcmp(patptr, "\"YY-MM-DD\"") == 0 
      || strcmp(patptr, "YYYY-MM-DDTHH:MM:SS") == 0
      || strcmp(patptr, "YYYY-MM-DD HH:MM:SS") == 0
      || strcmp(patptr, "\"YYYY-MM-DD HH:MM:SS\"") == 0
      || strcmp(patptr, "\"YYYY-MM-DDTHH:MM:SS\"") == 0 )
      ISO = 1;
  /* We should now have the data buffer and the pattern buffer */
  /* ready to go, so initialize the dates structures */

  i = 0;

  memset(&datedata, 0, sizeof(date_time));
  memset(&daterangedata, 0, sizeof(date_time_range));


  if ((patptr[0] == 'Y' && patptr[1] == 'Y') 
      || (patptr[0] == 'M' && patptr[1] == 'M')
      || (patptr[0] == 'D' && patptr[1] == 'D')) {

    /* first thing to match is a number so skip everything else*/
    while (isdigit(*databuf)==0) databuf++;

  }


  cptr = databuf;


  while (patptr[i] != '\0') {

    switch (patptr[i]) {

    case '"': /* ignore quotes and whitespace in the pattern */
    case ' ':
    case ':':
    case '/':
    case '.':
    case ',':
    case '\t':
    case '\n':
      i++;
      while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || *cptr == '"' || *cptr == ':' || *cptr == '/' 
	     || *cptr == '.' || *cptr == ',') cptr++;
      
      break;
     
    case 'S':
      if (patptr[i+1] == 'S') {
	/* 2 digit seconds */
	i+=2;
	for (j=0; (isdigit(*cptr) || *cptr == 'x' || *cptr == 'X') && j < 2; j++) tmp[j] = do_wild(*cptr++,range_flag);
	/* we ignore seconds for now */
	/* if there is a zone indication, get it */
	for (; *cptr == ' ' || isalpha(*cptr) ; j++) tmp[j] = toupper(*cptr++);
	tmp[j] = '\0';
      }
      break;

    case 'Y':  /* Years */
      if (patptr[i+1] == 'Y' && patptr[i+2] == 'Y' && patptr[i+3] == 'Y') {
	/* 4 digit year */
	i+=4;
	tmp[0] = do_wild(*cptr++,range_flag);
        tmp[1] = do_wild(*cptr++,range_flag);
	tmp[2] = do_wild(*cptr++,range_flag);
	tmp[3] = do_wild(*cptr++,range_flag);
	tmp[4] = '\0';
	year = atoi(tmp) * getEra(&cptr,1);
      }
      else if (patptr[i+1] == 'Y') {
	/* 2 digit year */
	i+=2;
	tmp[0] = '1';
	tmp[1] = '9';
	tmp[2] = do_wild(*cptr++,range_flag);
        tmp[3] = do_wild(*cptr++,range_flag);
	tmp[4] = '\0';
	year = atoi(tmp) * getEra(&cptr,1);
      }
      else if (patptr[i+1] == 'E' && patptr[i+2] == 'A' 
	       && patptr[i+3] == 'R') {
	/* multi digit year */
	i+=4;
	if (*cptr == '-') {
	  negyear = -1;
	  cptr++;
	}
	else {
	  negyear = 1;
	}
	for (j=0; (isdigit(*cptr) || *cptr == 'x' || *cptr == 'X' || *cptr == '-'); j++) 
	  tmp[j] = do_wild(*cptr++,range_flag);
	tmp[j] = '\0';
	year = atoi(tmp) * getEra(&cptr, negyear);
      }
      else i++; /* ignore other Y's */
      break;

    case 'M':  /* Months or Minutes */
      if (patptr[i+1] == 'M' && patptr[i+2] == 'M') {
	/* 3 letter month */
	i+=3;
	tmp[0] = toupper(*cptr++);
        tmp[1] = toupper(*cptr++);
	tmp[2] = toupper(*cptr++);
	tmp[3] = '\0';
	/* compare month names */
	month = monthbyname(tmp);
      }
      else if (patptr[i+1] == 'M') {
	/* 2 digit month or minutes*/
	i+=2;
	tmp[0] = do_wild(*cptr++,range_flag);
        tmp[1] = do_wild(*cptr++,range_flag);
	tmp[2] = '\0';
	if (date_time_flag) minutes = atoi(tmp);
	else
	  month = atoi(tmp);
	date_time_flag = 0;
      }
      else if (patptr[i+1] == 'O' 
	       && patptr[i+2] == 'N'
	       && patptr[i+3] == 'T'
	       && patptr[i+4] == 'H') {
	/* spelled out or abbreviated month */
	i+=5;
	for (j=0; isalpha(*cptr) ; j++) tmp[j] = toupper(*cptr++);
	tmp[j] = '\0';
	month = monthbyname(tmp);
      }
      else if (patptr[i+1] == 'I' 
	       && patptr[i+2] == 'L'
	       && patptr[i+3] == 'L'
	       && patptr[i+4] == 'E'
	       && patptr[i+5] == 'N'
	       && patptr[i+6] == 'N'
	       && patptr[i+7] == 'I'
	       && patptr[i+8] == 'U'
	       && patptr[i+9] == 'M') {
	/* Millennium parsing                          */
        /* for values like "3rd Millennium BC"         */
	i+=10;
	millennium_flag = 1;
	for (j=0; isdigit(*cptr); j++) tmp[j] = *cptr++;
	tmp[j] = '\0';
	
	while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || *cptr == '"' || *cptr == ':' || *cptr == '/' 
	     || toupper(*cptr) == 'S' || toupper(*cptr) == 'T'
	     || toupper(*cptr) == 'N' || toupper(*cptr) == 'D'
	     || toupper(*cptr) == 'R' || toupper(*cptr) == 'H'
	     || toupper(*cptr) == 'M' || toupper(*cptr) == 'I'
	     || toupper(*cptr) == 'L' || toupper(*cptr) == 'E'
	     || toupper(*cptr) == 'U'
	     || *cptr == '.' || *cptr == '\'' ) cptr++;


	year = 1000 * (atoi(tmp) * getEra(&cptr,1));

      }
      else i++; /*ignore other M's */
      
      break;
    case 'D':  /* Days */
      if (patptr[i+1] == 'D') {
	/* 2 digit days*/
	i+=2;
	tmp[0] = do_wild(*cptr++,range_flag);
        tmp[1] = do_wild(*cptr++,range_flag);
	tmp[2] = '\0';
	day = atoi(tmp);
      }
      else if (strncmp("DECADE", &patptr[i], 6) == 0) {
	/* DECADE numbers */
	i+=6;
	decade_flag = 1;
	for (j=0; 
	     (isdigit(*cptr) || *cptr == 'x' || *cptr == 'X');
	     j++) tmp[j] = *cptr++;

	tmp[j] = '\0';
	year = atoi(tmp) * getEra(&cptr,1);
	/* assume decades will be in the 20th century */
	if (year <= 90) year += 1900;

	while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || *cptr == '"' || *cptr == ':' || *cptr == '/' 
	     || *cptr == '.' || toupper(*cptr) == 'S' || *cptr == '\'' ) cptr++;
	
      }
      else if (patptr[i+1] == 'A' && patptr[i+2] == 'Y') {
	/* spelled out day of the week -- ignore for now */
	for (j=0; isalpha(*cptr) ; j++) tmp[j] = toupper(*cptr++);
	tmp[j] = '\0';
	i += 3;
      }
      else i++; /* ignore other D's */
	
      break;
      
    case 'C':  /* Century */

      if (strncmp("CENTURY", &patptr[i], 7) == 0) {
	/* Century numbers                           */
	/* get values like "21st" "18th" "12th" "3rd Century B.C." */
	i+=7;
	century_flag = 1;
	for (j=0; isdigit(*cptr) ; j++) tmp[j] = *cptr++;

	tmp[j] = '\0';
	
	while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || *cptr == '"' || *cptr == ':' || *cptr == '/' 
	     || toupper(*cptr) == 'S' || toupper(*cptr) == 'T'
	     || toupper(*cptr) == 'N' || toupper(*cptr) == 'D'
	     || toupper(*cptr) == 'R' || toupper(*cptr) == 'H'
	     || toupper(*cptr) == 'C' || toupper(*cptr) == 'E'
	     || toupper(*cptr) == 'U' || toupper(*cptr) == 'Y'
	     || *cptr == '.' || *cptr == '\'' ) cptr++;

	temp_era = getEra(&cptr,1);
	if (temp_era < 0)
	  year = (100 * (atoi(tmp)*temp_era)); 
	else
	  year = (100 * atoi(tmp)) - 100; 
	  
      }
      else i++; /* ignore other C's */
	
      break;
      
    case 'T':  /* possible range separator */
      if (strncmp("TO ", &patptr[i], 3) == 0) {
	/* range separator               */
	i+=2;
      }
      else if (strncmp("THROUGH ", &patptr[i], 8) == 0) {
	i+=7;
	through_flag = 1;
      }
      else if (strncmp("THRU ", &patptr[i], 5) == 0) {
	i+=4;
      }
      else {
	i++; /* ignore other t's */
	break;
      }
    case '-':  /* range separator */
      i++; /* add one to other range seps */

      /* if an ISO format date, ignore dashes */
      if (ISO) {
	cptr++; /* step past hyphen */
	break;
      }
      through_flag = 1;
      /* gobble up the separator */
      while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || toupper(*cptr) == 'T' || toupper(*cptr) == 'H'
	     || toupper(*cptr) == 'R' || toupper(*cptr) == 'O'
	     || toupper(*cptr) == 'G' || toupper(*cptr) == 'U' 
	     || toupper(*cptr) == '-') cptr++;
      
      if (month > 12) 
	month = 0;
      if (day > 31)
	day = 0;
      if (hours > 24)
	hours = 0;
      if (minutes > 59)
	minutes = 0;
      daterangedata.start.dt_year = year;
      daterangedata.start.dt_month = month;
      daterangedata.start.dt_day = day;
      daterangedata.start.dt_hour = hours;
      daterangedata.start.dt_minute = minutes;
      if (millennium_flag) daterangedata.scale = 1000;
      if (century_flag) daterangedata.scale = 100;
      if (decade_flag) daterangedata.scale = 10;
      range_flag = 1;

      break;

    case 'H':  /* Hours */
      if (patptr[i+1] == 'H') {
	/* 2 digit hours */
	i+=2;
	tmp[0] = do_wild(*cptr++,range_flag);
	tmp[1] = do_wild(*cptr++,range_flag);
	tmp[2] = '\0';
	date_time_flag = 1;
	hours = atoi(tmp);
      }
      else i++; /* ignore other H's */
      break;

    default: /* not a recognized initial letter for any format codes: ignore */
      i++;
    }
  }

  if (month > 12) 
    month = 0;
  if (day > 31)
    day = 0;
  if (hours > 24)
    hours = 0;
  if (minutes > 59)
    minutes = 0;
  
  result_string = CALLOC(char, 60);
  
  
  if (range_flag) {
    if (through_flag && decade_flag)
      year = year + 9;
    if (through_flag && century_flag) {
      if (year < 0)
	year = year - 1;
      else
	year = year + 99;
    }
    if (through_flag && millennium_flag) {
	if (year < 0)
	  year = year - 1;
	else 
	  year = year + 999;
    }
    
    daterangedata.end.dt_year = year;
    daterangedata.end.dt_month = month;
    daterangedata.end.dt_day = day;
    daterangedata.end.dt_hour = hours;
    daterangedata.end.dt_minute = minutes;

    if (daterangedata.end.dt_year == 0 &&
	daterangedata.end.dt_month == 0 &&
	daterangedata.end.dt_day == 0 &&
	daterangedata.end.dt_hour == 0 &&
	daterangedata.end.dt_minute == 0 &&
	daterangedata.start.dt_year == 0 &&
	daterangedata.start.dt_month == 0 &&
	daterangedata.start.dt_day == 0 &&
	daterangedata.start.dt_hour == 0 &&
	daterangedata.start.dt_minute == 0 )
      return (NULL);

    /* single year specified for range??? */

    if ((daterangedata.end.dt_year == 0 || 
	 daterangedata.end.dt_year == daterangedata.start.dt_year) &&
	daterangedata.start.dt_year != 0 &&
	daterangedata.end.dt_month == 0 &&
	daterangedata.end.dt_day == 0 &&
	daterangedata.end.dt_hour == 0 &&
	daterangedata.end.dt_minute == 0) {
      daterangedata.end.dt_year = daterangedata.start.dt_year;
      if (daterangedata.start.dt_month == 0)
	daterangedata.end.dt_month = 12;
      else 
	daterangedata.end.dt_month = daterangedata.start.dt_month;
      if (daterangedata.start.dt_day == 0)
	daterangedata.end.dt_day = 31;
      else 
	daterangedata.end.dt_day = daterangedata.start.dt_day;
      daterangedata.end.dt_hour = 23;
      daterangedata.end.dt_minute = 59;
    }

    
    sprintf(result_string,
	    "%010d %02d %02d %02d %02d %010d %02d %02d %02d %02d %05d",
	    daterangedata.start.dt_year, daterangedata.start.dt_month, 
	    daterangedata.start.dt_day, daterangedata.start.dt_hour, 
	    daterangedata.start.dt_minute, 
	    daterangedata.end.dt_year, daterangedata.end.dt_month, 
	    daterangedata.end.dt_day,
	    daterangedata.end.dt_hour, daterangedata.end.dt_minute,  
	    daterangedata.scale);
    
  }
  else { /* not an explicit range -- may be an implicit range like decade */

    if (decade_flag || century_flag || millennium_flag) {
    
      if (decade_flag) {
	daterangedata.start.dt_year = year;
	daterangedata.end.dt_year = year + 9;
	daterangedata.scale = 10;
	range_flag = 1;
      }
      else if (century_flag) {
	daterangedata.start.dt_year = year;
	daterangedata.end.dt_year = year + 99;
	daterangedata.scale = 100;
	range_flag = 1;
      }
      else if (millennium_flag) {
	if (year > 0) {
	  daterangedata.start.dt_year = year - 1000 ;
	  daterangedata.end.dt_year = year - 1;
	}
	else { /* millennia BC */
	  daterangedata.start.dt_year = year;
	  daterangedata.end.dt_year = year + 999;;
	}	  
	
	daterangedata.scale = 1000;
	range_flag = 1;
      }
      
    if (daterangedata.end.dt_year == 0 &&
	daterangedata.end.dt_month == 0 &&
	daterangedata.end.dt_day == 0 &&
	daterangedata.end.dt_hour == 0 &&
	daterangedata.end.dt_minute == 0 &&
	daterangedata.start.dt_year == 0 &&
	daterangedata.start.dt_month == 0 &&
	daterangedata.start.dt_day == 0 &&
	daterangedata.start.dt_hour == 0 &&
	daterangedata.start.dt_minute == 0 )
      return (NULL);
    
      sprintf(result_string,
	      "%010d %02d %02d %02d %02d %010d %02d %02d %02d %02d %05d",
	      daterangedata.start.dt_year, daterangedata.start.dt_month, 
	      daterangedata.start.dt_day, daterangedata.start.dt_hour, 
	      daterangedata.start.dt_minute, 
	      daterangedata.end.dt_year, daterangedata.end.dt_month, 
	      daterangedata.end.dt_day,
	      daterangedata.end.dt_hour, daterangedata.end.dt_minute,  
	      daterangedata.scale);
    }
    else {
      datedata.dt_year = year;
      datedata.dt_month = month;
      datedata.dt_day = day;
      datedata.dt_hour = hours;
      datedata.dt_minute = minutes;

      if (datedata.dt_year == 0 &&
	  datedata.dt_month == 0 &&
	  datedata.dt_day == 0 &&
	  datedata.dt_hour == 0 &&
	  datedata.dt_minute == 0)
	return (NULL);
    
      
      sprintf(result_string, "%010d %02d %02d %02d %02d", 
	      datedata.dt_year, datedata.dt_month, datedata.dt_day,
	      datedata.dt_hour, datedata.dt_minute);
    }
  }
    

  return(result_string);

}








