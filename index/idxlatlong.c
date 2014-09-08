/*
 *  Copyright (c) 1990-2002 [see Other Notes, below]. The Regents of the
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
*	Header Name:  idxlatlong.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Indexing formatted latitudes and longitudes in an SGML field.
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
#include "lat_long.h"
#include "ht_info.h"
#include "ctype.h"

extern char *detag_data_block(SGML_Data *data, int index_type, idx_list_entry *idx);

extern weighted_result *se_bool_element (idx_list_entry *idx,  
					 char *filename, char *insearchstring, 
					 int relation, int position, 
					 int structure, int truncation, 
					 int completeness, int attr_nums[],
					 char *attr_str[],  
					 ObjectIdentifier attr_oid[]);

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);

extern char *GetRawSGML(char *filename, int recordnum);

  /***************************************************
   * Defined formats for lat_LONG and Bounding_box
   * from cf_parse.flex
    "DECIMAL_LAT_LONG" 	return (LAT_LONG_NORM)
    "DECIMAL_LONG_LAT" return (LAT_LONG_NORM)
    "DDoMM'SS''NS DDDoMM'SS''EW"  (LAT_LONG_NORM)
    "DD-MM-SS NS DDD-MM-SS EW" (LAT_LONG_NORM)
    "DD-MM-SS-NS DDD-MM-SS-EW" (LAT_LONG_NORM)
    "\"DECIMAL_LAT_LONG\"" (LAT_LONG_NORM)
    "\"DECIMAL_LONG_LAT\"" (LAT_LONG_NORM)
    "\"DDoMM'SS''NS DDDoMM'SS''EW\"" (LAT_LONG_NORM)
    "\"DD-MM-SS NS DDD-MM-SS EW\"" (LAT_LONG_NORM)
    "\"DD-MM-SS-NS DDD-MM-SS-EW\"" (LAT_LONG_NORM)
    "DECIMAL_BOUNDING_BOX" (BOUNDING_BOX_NORM)
    "DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW" (BOUNDING_BOX_NORM)
    "DD-MM-SS NS DDD-MM-SS EW DD-MM-SS NS DDD-MM-SS EW" (BOUNDING_BOX_NORM)
    "DD-MM-SS-NS DDD-MM-SS-EW DD-MM-SS-NS DDD-MM-SS-EW" (BOUNDING_BOX_NORM)
    "\"DECIMAL_BOUNDING_BOX\"" (BOUNDING_BOX_NORM)
    "\"DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW\"" (BOUNDING_BOX_NORM)
    "\"DD-MM-SS NS DDD-MM-SS EW DD-MM-SS NS DDD-MM-SS EW\"" (BOUNDING_BOX_NORM)
    "\"DD-MM-SS-NS DDD-MM-SS-EW DD-MM-SS-NS DDD-MM-SS-EW\"" (BOUNDING_BOX_NORM)
   ********************************************/   


/* getdirection -- read past whitespace and check for N S E W */
int
getdirection(char *in) 
{
  char *ptr = in;
  char test;

  if (*in == '\0')
    return 1;

  test = '\0';

  while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n'
	 || *ptr == '"' || *ptr == '-' || *ptr == '\'' 
	 ) ptr++;

  test = toupper(*ptr);

  if (test == 'S' || test == 'W')
    return (-1);

  /* default to N or E */
  return (1);
}

char *
parse_decimal(char *databuf, int type) 
{
  char *result_string;
  float lat1, lat2;
  float long1, long2;
    
    if (type == 0) {
      if (sscanf(databuf,"%f %f", 
		 &lat1, &long1) == 2) {
	result_string = CALLOC(char, 30);
	
	sprintf(result_string, "%011.6f %011.6f", 
		lat1, long1);
      }
      else {
	if (sscanf(databuf,"%f, %f", 
		   &lat1, &long1) == 2) {
	  result_string = CALLOC(char, 30);
	
	  sprintf(result_string, "%011.6f %011.6f", 
		  lat1, long1);
	}
	else {

	  return NULL;
	}
      }
    }
    else if (type == 1) {
      if (sscanf(databuf,"%f %f", 
		 &long1, &lat1) == 2) {
	result_string = CALLOC(char, 30);
	
	sprintf(result_string, "%011.6f %011.6f", 
		lat1, long1);
      }
      else {
	if (sscanf(databuf,"%f, %f", 
		   &long1, &lat1) == 2) {
	  result_string = CALLOC(char, 30);
	  
	  sprintf(result_string, "%011.6f %011.6f", 
		  lat1, long1);
	}
	else {
	  return NULL;
	}
      } 
    }
    else if (type == 2) {
      if (sscanf(databuf,"%f %f %f %f", 
		 &lat1, &long1, &lat2, &long2) == 4) {

	result_string = CALLOC(char, 60);
	sprintf(result_string, "%011.6f %011.6f %011.6f %011.6f ", 
		lat1, long1, lat2, long2);
      }
      else {
	return NULL;
      }
    }
    else if (type == 3) {
      /* FGDC order for BB coords is westbc,eastbc,northbc,southbc */
      if (sscanf(databuf,"%f %f %f %f", 
		  &long1, &long2, &lat1, &lat2) == 4) {

	result_string = CALLOC(char, 60);
	sprintf(result_string, "%011.6f %011.6f %011.6f %011.6f ", 
		lat1, long1, lat2, long2);
      }
      else {
	return NULL;
      }
    }

  return(result_string);

}

char *
parse_latlong(char *databuf, char *patptr)
{
  char *cptr;
  char tmppat[100] = "";
  char tmp[100];
  char *result_string;
  int i, j;
  int degrees=0, minutes=0, seconds=0, direction=0, latlong_type = 0;
  int bbox_flag = 0;
  float tempvalue;
  
  lat_long lldata;
  bounding_box bbdata;
  
  if (databuf == NULL) return(0);
  
  while (*databuf == ' ' || *databuf == '\t' 
	 || *databuf == '\n' || *databuf == '"') databuf++;
  
  if (patptr == NULL)
    return(NULL); /* maybe should have some default...*/
  
  for (i = 0; patptr[i] != '\0'; i++)
    patptr[i] = toupper(patptr[i]);
  
  
  if (strcmp(patptr, "DECIMAL_LAT_LONG") == 0
      || strcmp(patptr, "\"DECIMAL_LAT_LONG\"") == 0)
    return (parse_decimal(databuf,0));
  
  if (strcmp(patptr, "DECIMAL_LONG_LAT") == 0
      || strcmp(patptr, "\"DECIMAL_LONG_LAT\"") == 0)
    return (parse_decimal(databuf,1));
  
  if (strcmp(patptr, "DECIMAL_BOUNDING_BOX") == 0
      || strcmp(patptr, "\"DECIMAL_BOUNDING_BOX\"") == 0)
    return (parse_decimal(databuf,2));

  if (strcmp(patptr, "FGDC_BOUNDING_BOX") == 0
      || strcmp(patptr, "\"FGDC_BOUNDING_BOX\"") == 0)
    return (parse_decimal(databuf,3));
  
  if (strlen(patptr) > 30) /* bounding box patterns are larger */
    bbox_flag = 1;
  
  i = 0;
  
  memset(&lldata, 0, sizeof(lat_long));
  memset(&bbdata, 0, sizeof(bounding_box));
  
  cptr = databuf;
  
  degrees = minutes = seconds = -1;
  direction = 0;
  
  while (patptr[i] != '\0') {
    
    switch (patptr[i]) {
      
    case '"': /* ignore quotes and whitespace in the pattern */
    case ' ':
    case '-':
    case '\'':
    case 'O':
    case 'o':
    case ':':
    case ';':
    case '.':
    case '\t':
    case '\n':
      i++;
      while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	     || *cptr == '-' || *cptr == '\'' || *cptr == 'O' || *cptr == 'o'
	     || *cptr == ':' || *cptr == '.' || *cptr == ';'
	     || *cptr == '°'
	     || *cptr == '"') cptr++;
      
      break;
      
    case 'S':
      if (patptr[i+1] == 'S') {
	/* 2 digit seconds */
	i+=2;
	for (j=0; isdigit(*cptr) && j < 2; j++) tmp[j] = *cptr++;
	tmp[j] = '\0';
	seconds = atoi(tmp);
      }
      break;
      
    case 'M':
      if (patptr[i+1] == 'M') {
	/* 2 digit minutes */
	i+=2;
	for (j=0; isdigit(*cptr) && j < 2; j++) tmp[j] = *cptr++;
	tmp[j] = '\0';
	minutes = atoi(tmp);
      }
      break;
      
    case 'D':  /* DEGREES */
      if (patptr[i+1] == 'D') {
	i+=2;
	for (j=0; isdigit(*cptr) && j < 3; j++) tmp[j] = *cptr++;
	tmp[j] = '\0';
	if (patptr[i] == 'D')
	  i++;
	degrees = atoi(tmp);
      }
      break;
      
    case 'N': /* direction */
      if (patptr[i+1] == 'S') {
	i+=2;
	latlong_type = 1;
	direction = getdirection(cptr);
	while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	       || *cptr == '"' || *cptr == '-' || *cptr == '\'' 
	       || *cptr == 'N' || *cptr == 'S' || *cptr == 'E' 
	       || *cptr == 'W'
	       || *cptr == 'n' || *cptr == 's' || *cptr == 'e' 
	       || *cptr == 'w') cptr++;
      }
      break;
      
    case 'E': /* direction */
      if (patptr[i+1] == 'W') {
	i+=2;
	latlong_type = 2;
	direction = getdirection(cptr);
	while (*cptr == ' ' || *cptr == '\t' || *cptr == '\n'
	       || *cptr == '"' || *cptr == '-' || *cptr == '\'' 
	       || *cptr == 'N' || *cptr == 'S' || *cptr == 'E' 
	       || *cptr == 'W'
	       || *cptr == 'n' || *cptr == 's' || *cptr == 'e' 
	       || *cptr == 'w') cptr++;
      }
      break;
    }
    
    if (degrees != -1 && minutes != -1 && seconds != -1 && direction != 0) {
      if (latlong_type == 1 /* latitude */) {
	lldata.lat_degrees = degrees;
	lldata.lat_minutes = minutes;
	lldata.lat_seconds = seconds;
	lldata.lat_direction = direction;
      }
      else {
	lldata.long_degrees = degrees;
	lldata.long_minutes = minutes;
	lldata.long_seconds = seconds;
	lldata.long_direction = direction;
      }
      
      /* re-init the temp ones */
      degrees = minutes = seconds = -1;
      direction = 0;
      
      if (lldata.lat_direction != 0 && lldata.long_direction != 0) {
	/* convert to decimal form */
	lldata.latitude = 
	  ((((float)lldata.lat_degrees * 3600.00) 
	    + ((float)lldata.lat_minutes * 60.00) 
	    + lldata.lat_seconds) / 3600.00) * (float)lldata.lat_direction ;
	
	lldata.longitude = 
	  ((((float)lldata.long_degrees * 3600.00) 
	    + ((float)lldata.long_minutes * 60.00) 
	    + lldata.long_seconds) / 3600.00) * (float)lldata.long_direction ;
	
	if (bbox_flag /* add to bbox coords */ ) {
	  if (bbdata.NW.lat_direction == 0) {
	    bbdata.NW = lldata;
	    memset(&lldata, 0, sizeof(lat_long));
	  }
	  else {
	    bbdata.SE = lldata;
	  }
	}
      }
    }
  }    

  /* should have all of the data in place now, so create the key */

  if (bbox_flag == 0) {
    result_string = CALLOC(char, 30);
    
    sprintf(result_string, "%011.6f %011.6f", 
	    lldata.latitude, lldata.longitude);
    
  }
  else {
    result_string = CALLOC(char, 60);
    
    sprintf(result_string, "%011.6f %011.6f %011.6f %011.6f ", 
	    bbdata.NW.latitude, bbdata.NW.longitude,
	    bbdata.SE.latitude, bbdata.SE.longitude);
    
  }
  
  return(result_string);
}



int 
idxlatlongattr(SGML_Data *data, char *attr_name, 
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  char *end_buffer, *patptr, *latlong_key, *get_attr_value();
  int buflen;
  int exists;

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

  latlong_key = parse_latlong(databuf, patptr);

  /* put it into the hash table */
  if (latlong_key != NULL)
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 latlong_key, &exists), 
		     (ClientData)ht_data);
  
  
  if (latlong_key)
    FREE(latlong_key);

  FREE(databuf);
  return(0);
}


char *
gaz_get_data(char *namestring,int item, char *gazrecord, int *match) 
{
  
  char *tagstart;
  int taglen;
  int taglen2;

  char *searchtag;
  char *valtag=NULL;
  char *datastart;
  int datalen;
  int count;
  char *start[8];
  char *resultdata = NULL;

  start[0] = namestring;
  
  for (count = 1; count < 8 && count <= item; count++) {
    start[count] = (strchr(start[count-1], ':'))+1;
    
  }

  taglen2 = strcspn(start[item],"="); 
  taglen = strcspn(start[item],":"); 
  if (taglen2 < taglen) {
    valtag = CALLOC(char,taglen+1);
    strncpy(valtag,start[item]+taglen2+1,taglen-taglen2-1);
    taglen = taglen2;
  }
  searchtag = CALLOC(char, taglen+2);
  searchtag[0] = '<';
  strncat(searchtag, start[item], taglen);

  datastart = strstr(gazrecord, searchtag);

  if (datastart != NULL) {
    /* matched the start */
    datastart = strpbrk(datastart, ">");
    datastart++;
    datalen =  strcspn(datastart,"<"); 
    resultdata = CALLOC(char, datalen+1);
    strncpy(resultdata, datastart, datalen);
  }
  *match = 1;

  if (valtag) {
    if (strcasecmp(resultdata,valtag)==0) {
      *match = 1;
    } else {
      *match = 0;
    }
    FREE(valtag);
  }
  FREE(searchtag);

  return(resultdata);

}


int
idxgeotext (char *databuf, int len, Tcl_HashTable *hash_tab, 
	    idx_list_entry *idx, int resultflag)
{
  SGML_Data *gaz_data;
  char *start;
  char *phrasestart;
  char *c;
  char *tempbuff;
  int exists;
  int tlen, i, morphed=0;
  char *latlong_key=NULL;
  char *extract_data=NULL;
  char *name_data=NULL;
  int   match=0;
  char *type_data=NULL;
  char *country_data=NULL;
  char *adminregion_data=NULL;
  char *region_data=NULL;
  char *latlong_data=NULL;
  char *bounding_box_data=NULL;
  Tcl_HashEntry *entry;
  HT_DATA *ht_data;
  weighted_result *gaz_result;
  char *gazrec=NULL;
  int gaz_recnum;
  extern int sgml_parse_error;
  /* The naive method is to search for capitalized words, but we need to */
  /* check for Unicode capitals too -- but I don't want to deal with wc  */
  char *capitals="ABCDEFGHIJKLMNOPQRSTUVWXYZ\303\304\305\316\320";
  

  start = databuf;
  
  while ((phrasestart = strpbrk(start, capitals))) {

    if (isalpha(*(phrasestart+1)) && 
	(strncasecmp(phrasestart, "The ", 4) == 0
	|| strncasecmp(phrasestart, "An ", 3) == 0
	|| strncasecmp(phrasestart, "But ", 4) == 0
	|| strncasecmp(phrasestart, "Der ", 4) == 0)) {
      
      /* step over first word of a sentence/segment */
      start += strcspn(phrasestart," \t\n\r,.;:/?!@$%^&*()+={}[]|\\<>-");
      continue;
    }
    
    /* include everything up to whitespace or punctuation */
    tlen = strcspn(phrasestart," \t\n\r,.;:/?!@$%^&*()+={}[]|\\<>-");
    
    if (tlen > 1) {
      while (*(phrasestart+tlen) != '\0' 
	     && strchr(capitals,*(phrasestart+tlen+1)) > 0) {
	tlen += strcspn(phrasestart+tlen+1,
			" \t\n\r,.;:/?!@$%^&*()+={}[]|\\<>-") + 1;
      }
    }
	   

    if (tlen == 1) {
      start = phrasestart+tlen;
      continue;
    }

    tempbuff = CALLOC(char, tlen+1);
    c = tempbuff;

    for (i = 0; i < tlen; i++) {
      if (isalpha(phrasestart[i]))
	*c++ = phrasestart[i];
      if (phrasestart[i] == ' ' || phrasestart[i] < 0) 
	*c++ = phrasestart[i];
    }

    /*need to check for the Unicode stuff here... */

    /* set up to continue the loop */
    start = phrasestart+tlen;


    /* Now that we have a Capitalized term or phrase in tempbuff */
    /* We need to search the Gazetteer for it...                 */
    /* We set the DO NOT TRUNCATE flag                           */
    gaz_result = se_bool_element(idx->gaz_idx,
				 idx->gaz_config_file->file_name,tempbuff,
				 0,0,0,100,0,NULL,NULL,NULL);
    
    
    if (gaz_result != NULL 
	&& gaz_result->num_hits > 0) {

      /* there were hits in the gazetteer. Examine the records and */
      /* get the appropriate elements for adding to the index      */

      for (i=0; i< gaz_result->num_hits; i++) {
	/* so we avoid memory leaks, this needs to be both here and the end */
	if (extract_data) {
	  FREE(extract_data);
	  extract_data = NULL;
	}
	if (name_data) {
	  FREE(name_data);
	  name_data = NULL;
	}
	if (type_data) {
	  FREE(type_data);
	  type_data = NULL;
	}
	if (country_data) {
	  FREE(country_data);
	  country_data = NULL;
	}
	if (adminregion_data) {
	  FREE(adminregion_data);
	  adminregion_data = NULL;
	}
	if (region_data) {
	  FREE(region_data);
	  region_data = NULL;
	}
	if (latlong_data) {
	  FREE(latlong_data);
	  latlong_data = NULL;
	}
	if (bounding_box_data) {
	  FREE(bounding_box_data);
	  bounding_box_data = NULL;
	}
	if (gazrec) {
	  FREE(gazrec);
	  gazrec = NULL;
	}

	gazrec = GetRawSGML(idx->gaz_config_file->file_name,
			    gaz_result->entries[i].record_num);
	
	
	if (gazrec) {

	  /* get the data from the gaz entry */

	  extract_data = gaz_get_data(idx->gaz_data_tag,0, gazrec, &match); 
	  if (match == 0) continue;
	  name_data  = gaz_get_data(idx->gaz_data_tag,1, gazrec, &match); 
	  if (match == 0) continue;
	  type_data = gaz_get_data(idx->gaz_data_tag,2, gazrec, &match); 
	  if (match == 0) continue;
	  country_data = gaz_get_data(idx->gaz_data_tag,3, gazrec, &match); 
	  if (match == 0) continue;
	  adminregion_data = gaz_get_data(idx->gaz_data_tag,4, gazrec, &match); 
	  if (match == 0) continue;
	  region_data = gaz_get_data(idx->gaz_data_tag,5, gazrec, &match); 
	  if (match == 0) continue;
	  latlong_data = gaz_get_data(idx->gaz_data_tag,6, gazrec, &match); 
	  if (match == 0) continue;
	  bounding_box_data = gaz_get_data(idx->gaz_data_tag,7, gazrec, &match); 
	  if (match == 0) continue;
  

	  /* If we get to here we must have the keys extracted from the gaz */
	  /* and any static requirements from the config file have been     */
	  /* matched (I hope)  */
	  /* Create the appropriate hash structures for terms or latlongs   */
	  if (resultflag) {
	    
	  if (idx->dateformat == NULL)
	    latlong_key = parse_latlong(extract_data, "DECIMAL_LAT_LONG");
	  else
	    latlong_key = parse_latlong(extract_data, idx->dateformat);
	  }
	  else {
	    latlong_key = normalize_key(extract_data, idx, &morphed, 0);
	    
	  }
	  
	  
	  /* NULL means a stop word or other problem, so skip it */
	  if (latlong_key != NULL && latlong_key[0] != '\0') {
	    int result_len;
	    
	    if ((result_len = strlen(latlong_key)) > 0) {
	      /* We have a normalized word, is it in the hash table? */
	      entry = Tcl_FindHashEntry(hash_tab,latlong_key);
	      
	      if (entry == NULL) { /* nope, it is new word */
		/* create the hash table structure */
		ht_data = CALLOC(struct hash_term_data, 1) ;
		ht_data->termid = -1;
		ht_data->termfreq = 1;

		/* put it into the hash table */
		Tcl_SetHashValue(
				 Tcl_CreateHashEntry(hash_tab,
						     latlong_key, &exists), 
				 (ClientData)ht_data);
	      }
	      else { /* word already in hash table, so just increment it */
		ht_data =  
		  (struct hash_term_data *) Tcl_GetHashValue(entry);
		ht_data->termfreq++;
	      }
	    }
	  
	    FREE(latlong_key);
	  }

	}
      }	    

    }

    /* free the last extract data */
    
    if (extract_data) {
      FREE(extract_data);
      extract_data = NULL;
    }
    if (name_data) {
      FREE(name_data);
      name_data = NULL;
    }
    if (type_data) {
      FREE(type_data);
      type_data = NULL;
    }
    if (country_data) {
      FREE(country_data);
      country_data = NULL;
    }
    if (adminregion_data) {
      FREE(adminregion_data);
      adminregion_data = NULL;
    }
    if (region_data) {
      FREE(region_data);
      region_data = NULL;
    }
    if (latlong_data) {
      FREE(latlong_data);
      latlong_data = NULL;
    }
    if (bounding_box_data) {
      FREE(bounding_box_data);
      bounding_box_data = NULL;
    }
    if (gazrec) {
      FREE(gazrec);
      gazrec = NULL;
    }
    
    
    if (gaz_result != NULL) 
      FREE(gaz_result);
    FREE(tempbuff);

  } /* end of loop through data */
  
  FREE(databuf);
  
  return(0);
  
}

int 
idxlatlong(SGML_Data *data,
	    Tcl_HashTable *hash_tab, idx_list_entry *idx)
{
  
  char *databuf;
  char *end_buffer, *patptr, *latlong_key;
  int buflen;
  int exists;
  HT_DATA *ht_data;
  char *breakletters = " \t\n\r/:.-";


  /* don't clobber UNICODE characters! use NOMAP */
  databuf = detag_data_block(data, (idx->type | NORM_NOMAP), idx);

  if (databuf == NULL) return(0);

  buflen = strlen(databuf);

  if (buflen == 0) {
    FREE(databuf);
    return (0);
  }

  end_buffer = databuf + buflen;

  /* GEOTEXT means we don't parse things directly, but get them from */
  /* the associated Gazetteer in idx->gaz...                         */
  if (idx->type & GEOTEXT) {
    if ((idx->type & (LAT_LONG_KEY | BOUNDING_BOX_KEY)) > 0) {
      /* NOTE idxgeotext will FREE databuf */
      return(idxgeotext(databuf, buflen, hash_tab, idx, 1));
    }
    else 
      return(idxgeotext(databuf, buflen, hash_tab, idx, 0));
  }



  patptr = idx->dateformat;

  if (patptr == NULL)
    return(0); /* maybe should have some default...*/


  /* create the hash table structure */
  ht_data = CALLOC(struct hash_term_data, 1) ;
  ht_data->termid = -1;
  ht_data->termfreq = 1;

  latlong_key = parse_latlong(databuf, patptr);

  /* put it into the hash table */
  if (latlong_key != NULL)
    Tcl_SetHashValue(
		     Tcl_CreateHashEntry(hash_tab,
					 latlong_key, &exists), 
		     (ClientData)ht_data);  
  if (latlong_key)
    FREE(latlong_key);

  FREE(databuf);
  return(0);
}







