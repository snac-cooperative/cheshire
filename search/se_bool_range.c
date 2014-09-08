/* #define DEBUG */
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
*	Header Name:	se_bool_range.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	boolean range search processing for a single index
*                       element of the cheshire catalog
*
*	Usage:		se_bool_range (index_struct, term, relation,
*                                 position, structure, truncation, 
*                                 completeness)
*
*	Variables:	index_struct is an idx_list_entry for the index
*                       term is the word, phrase or wordlist to be searched
*                       the relation, position, structure, truncation, 
*                        and completeness variables are the z39.50 attributes
*                        specified in the query.
*
*	Return Conditions and Return Codes:	
*                        returns a weighted list on success, and NULL on
*                        failure. Note that zero hits is NOT failure, and
*                        returns a list with num_hits == 0.
*
*	Date:		12/12/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "cheshire.h"
#include "configfiles.h"
#include "date_time.h"
#include "z_parameters.h"
#include <math.h>
#ifdef WIN32
/* Microsoft doesn't have strncasecmp, so here it is... blah... */
#define strcasecmp _stricmp
#define strncasecmp _strnicmp 
#endif

extern char *normalize_key(char *raw_word, idx_list_entry *idx, 
			   int *morphflag, int diacritics);


weighted_result 
*se_bool_range (idx_list_entry *idx,  char *searchstring, 
		int relation, int position, 
		int structure, int truncation, 
		int completeness, int attr_nums[],
		char *attr_str[], ObjectIdentifier attr_oid[])
{
  
  int i, exists;
  int  tot_docs;
  int record_num;
  int num_search_words = 0;
  int max_hits = 0;
  int direction;
  int morphed;
  int geo_ops_ok, geo_op;
  char *search_ptr = NULL;
  char *start_key = NULL, *end_key = NULL;
  char *c;
  float s_lat1, s_lat2, s_long1, s_long2;
  float t_lat1, t_lat2, t_long1, t_long2;
  float tempcoord;
  int scanresult;
  char tempgeokey[64];
  int ok_to_add;
  int direction_flag = 1;
  struct temp_results {
    int weight;
    int tot_freq;
  } *storage ;
  int search_stopword_count;
  
  Tcl_HashTable *hash_tab, hash_tab_data;
  Tcl_HashEntry *entry;
  Tcl_HashSearch hash_search;
  DB *index_db;
  DBT keyval;
  DBT dataval;
  DBC *dbcursor;
  int db_errno;
  idx_result *newinfo;
  int more;

  weighted_result *wt_res, *se_sortwtset();

  if (searchstring == NULL) return (NULL);

  search_stopword_count = 0;


  /* make sure everything is open */
  if (idx->db == NULL) {
      return(NULL);
  }
  else
    index_db = idx->db;

  search_ptr = 
    normalize_key(searchstring, idx, &morphed, 1);

  if (search_ptr == NULL && idx->stopwords_file != NULL) {
    /* this is REALLY ugly to do this way -- */
    /* stopword -- since this is a range search we continue... */
    Tcl_HashTable temp_stop_data,  stoplist_hash;
    
    stoplist_hash = idx->stopwords_hash;

    Tcl_InitHashTable(&temp_stop_data,TCL_STRING_KEYS);
  
    idx->stopwords_hash = temp_stop_data;
    search_stopword_count++;

    search_ptr = 
      normalize_key(searchstring, idx, &morphed, 1);

    idx->stopwords_hash = stoplist_hash;

  }

  /* check for known relation attributesets */
  if (attr_oid[2] == NULL 
      || strcmp(attr_oid[2]->data, OID_GEO) == 0
      || strcmp(attr_oid[2]->data, OID_BIB1) == 0) {
    /* we pretend that GEO is part of BIB1 too */
    geo_ops_ok = 1;
    geo_op = relation;
  }
  else {
    geo_ops_ok = 0;
    geo_op = 0;
  }

  ok_to_add = 1;

  if (relation == 104 || geo_op == 16) {
    if (idx->type & DATE_KEY) {
      start_key = search_ptr;
      end_key = search_ptr;
    }
    else if (idx->type & DATE_RANGE_KEY) {
      /* date ranges are stored as strings like the following
       * 0000001958 00 00 00 00 0000001994 00 00 00 00 00000
       * so the end date starts at position 23
        */
      start_key = search_ptr;
      end_key = search_ptr+23;
      search_ptr[22] = '\0';
    }
    else { /* Not dates -- look for hyphenation... */
      if ((end_key = strchr(search_ptr, (int)'-')) != NULL) {
	start_key = search_ptr;
	*end_key = '\0';
	end_key++;
      }
      else {
	start_key = search_ptr;
	end_key = search_ptr;
      }
    }
  } 
  else
    start_key = end_key = search_ptr;
    
  /* Acquire a cursor for the database. */
  if ((db_errno = index_db->cursor(index_db, NULL, &dbcursor, 0)) != 0) {
    fprintf(LOGFILE,"unable to get cursor in range query\n");
    return(NULL);
  }
    
  if (start_key == NULL) {
    fprintf(LOGFILE,"Invalid key in range search\n");
    return(NULL);
  }

  /* Initialize the key/data pair so the flags aren't set. */
  memset(&keyval, 0, sizeof(keyval));
  memset(&dataval, 0, sizeof(dataval));
  keyval.data = (void *) start_key;
  keyval.size = strlen(start_key);
    
  if (keyval.size < 1)  { /* don't permit searches on NULL strings */
    return(NULL);
  }
    
  /* jump to the appropriate place in the index */
  db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);

  more = 1;

  if (relation == 1 || geo_op == 14) { /* Less Than */
    if ((idx->type & INTEGER_KEY 
	 || idx->type & DECIMAL_KEY
	 || idx->type & DATE_KEY
	 || idx->type & FLOAT_KEY)
	&& start_key[0] == '-') {
      
      direction = DB_NEXT;
      if (strcmp(keyval.data,start_key) <= 0)
	db_errno =
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT);
    }
    else {
      direction = DB_PREV;
      db_errno =
	dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV);
    }
  }    
  else if (relation == 2 || geo_op == 15) { /* Less Than or Equal To */
    if ((idx->type & INTEGER_KEY 
	 || idx->type & DECIMAL_KEY
	 || idx->type & DATE_KEY
	 || idx->type & FLOAT_KEY)
	&& start_key[0] == '-') {
      
      direction = DB_NEXT;
    }
    else {
      direction = DB_PREV;
      
      if (strcmp(keyval.data,start_key) > 0)
	db_errno = 
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV);
    }
  }
  else if (relation == 4 || geo_op == 17) { /* Greater Than or Equal To */
    if ((idx->type & INTEGER_KEY 
	 || idx->type & DECIMAL_KEY
	 || idx->type & DATE_KEY
	 || idx->type & FLOAT_KEY)
	&& start_key[0] == '-') {
      
      direction = DB_PREV;
      if (strcmp(keyval.data,start_key) > 0) 
	db_errno =
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV);
      if (db_errno == DB_NOTFOUND) {
	keyval.data = (void *)"0000000";
	keyval.size = strlen(keyval.data);
	
	/* jump to the zero point in the index */
	db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	direction = DB_NEXT;
	more = 1;
      }
    }
    else {
      direction = DB_NEXT;
      if (db_errno == DB_NOTFOUND) 
	more = 0;
    }
  }
  else if (relation == 5 || geo_op == 18) { /* Greater Than */
    if ((idx->type & INTEGER_KEY 
	 || idx->type & DECIMAL_KEY
	 || idx->type & DATE_KEY
	 || idx->type & FLOAT_KEY)
	&& start_key[0] == '-') {
      
      direction = DB_PREV;
      if (strcmp(keyval.data,start_key) >= 0) 
	db_errno =
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_PREV);
      if (db_errno == DB_NOTFOUND) {
	keyval.data = (void *)"0000000";
	keyval.size = strlen(keyval.data);
	
	/* jump to the zero point in the index */
	db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	direction = DB_NEXT;
	more = 1;
      }
    }
    else {
      direction = DB_NEXT;
      /* step ahead if equal to */
      if (db_errno == DB_NOTFOUND) 
	more = 0;
      if (strcmp(keyval.data,start_key) == 0) 
	db_errno =
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_NEXT);
    }
  }    
  else if (relation == 104  || geo_op == 16) {
    /* within range -- start as beginning of range */  
    direction = DB_NEXT;
    if (idx->type & DATE_RANGE_KEY) { 
      
      keyval.data = (void *) "0000000000 00 00 00 00";
      keyval.size = strlen(start_key);
      
      /* jump to the beginning of the index for range on range overlap */
      db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
    }

  } 
  /* The following are spatial operation that only make sense for geo
     data */
  else if (geo_op >= 7 && geo_op <= 18) {
    if (idx->type & LAT_LONG_KEY) {
      start_key = search_ptr;
      end_key = search_ptr;
      /* because search strings might be either BBox or lat_long... */
      if (strncasecmp(idx->dateformat,"DECIMAL",7) == 0 
	  && (sscanf(searchstring, "%f %f %f %f", 
		     &t_lat1, &t_long1, &t_lat2, &t_long2) == 4)) {
	/* parse the end bits...*/
	/* create a new key and parse it */
	sprintf(tempgeokey, "%11.6f %11.6f", t_lat2, t_long2);
	end_key = normalize_key(tempgeokey, idx, &morphed, 0);
      } 
      else if (strlen(searchstring) > 30) {
	char *save;
	char *newsrchptr;

	save = idx->dateformat;
	idx->dateformat = 
	  strdup("DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW");
	newsrchptr =  normalize_key(searchstring, idx, &morphed, 0);
	if (newsrchptr != NULL) {
	  FREE(search_ptr);
	  search_ptr = newsrchptr;
	  start_key = search_ptr;
	  end_key = search_ptr+24;
	  search_ptr[23] = '\0';
	}
	FREE(idx->dateformat);
	idx->dateformat = save;
      }
    }
    else {
      /* Bounding box key -- I hope */
      /* bounding boxes are stored as strings like the following
       * "0065.683296 -023.816700 0063.416698 -017.933300"
       * so the "ending" latlong starts at position 24
       */
      start_key = search_ptr;
      end_key = search_ptr+24;
      search_ptr[23] = '\0';
    }

    if (sscanf(start_key,"%f %f", 
	       &s_lat1, &s_long1) != 2) {
      diagnostic_set(125,0,"Search term must be lat_long or Bounding_box");
      dbcursor->c_close(dbcursor);
      return NULL;
    }
    if (sscanf(end_key,"%f %f", 
	       &s_lat2, &s_long2) != 2) {
      diagnostic_set(125,0,"Search term must be lat_long or Bounding_box");
      dbcursor->c_close(dbcursor);
      return NULL;
    }

    switch (geo_op) {
      
    case 7:
      /* OVERLAPS 
       * The access point region has a geometric area in common 
       * with the search term region. Given a
       * search term region of S and access point region of T,
       * the following algebra expresses the
       * conditions required: 
       * {S(North) >= T(South)} and {S(South) <= T(North)} 
       * and {S(East) >= T(West)} and {S(West) <= T(East)}.
       * so reset the start point to the SOUTH coord
       */
      keyval.data = (void *)end_key;
      keyval.size = strlen(end_key)+1;
      
      db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
      if (s_lat2 < 0.0)
	direction = DB_PREV;
      else
	direction = DB_NEXT;
      break;
      

    case 8:
      /* Fully enclosed within 
       * The access point region is fully enclosed within the search term 
       * region.
       * {S(North) >= T(North)} and {S(South) <= T(South)} 
       * and {S(East) >= T(East)} and {S(West) <= T(West)}.
       */
      if (s_lat1 < 0.0)
	direction = DB_NEXT;
      else
	direction = DB_PREV;
      break;
      
    case 9:
      /* Encloses
       * The access point region fully encloses the search term region
       * {S(North) <= T(North)} and {S(South) >= T(South)} 
       * and {S(East) <= T(East)} and {S(West) >= T(West)}.
       */
      /* search these from low to high */
      if (s_lat1 < 0.0)
	direction = DB_PREV;
      else
	direction = DB_NEXT;
      break;

    case 10:
      /* Fully Outside Of
       * The access point region has no geometric area in common with 
       * the search term region.
      */      
      keyval.data = (void *) "-000.000000";
      keyval.size = 11;
      
      db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
      direction = DB_NEXT;
      /* have to scan everything ? */
      break;
      
    case 11:
      /* Near
       * The access point region falls within a default distance 
       * of the search term region. The default distance is defined 
       * by the server.
       */
      /* we use half a degree as "near" -- about 30 miles */

      if (s_lat1 < 0.0) {
	direction = DB_NEXT;
	tempcoord = s_lat1 + 0.5;
	
      }
      else {
	direction = DB_PREV;
	tempcoord = s_lat1 + 0.5;
      }

      sprintf(tempgeokey, "%011.6f", 
	      tempcoord);

      keyval.data = (void *) tempgeokey;
      keyval.size = strlen(tempgeokey);
    
      db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);

      break;
      
    case 12:
	/* Members Contain
	 * The access point element or one of its subordinate elements 
	 * is equal to the search term value (subject to possible qualification 
	 * by the Truncation and Structure Attributes).
	 */
      diagnostic_set(117,relation,"Member Contain not supported by server");
      dbcursor->c_close(dbcursor);
      return NULL;
      break;
      
    case 13:
      /* Members Not Contain 
       * The access point element and all of its subordinate elements 
       * are not equal to the search term value (subject to possible 
       * qualification by the Truncation and Structure Attributes).
       */
      diagnostic_set(117,relation,"Member Not Contain not supported by server");
      dbcursor->c_close(dbcursor);
      return NULL;
      break;
    }
    ok_to_add = 0; /* need to test before adding these */

  } else {
    diagnostic_set(117,relation,"Relation Operator not supported by server");
    dbcursor->c_close(dbcursor);
    return NULL;
    
  }


  /* Allocate the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);
    
  do {

    c = keyval.data;

    if (geo_op >= 7 && geo_op < 104) {

      ok_to_add = 0;
      t_lat1 = t_long1 = t_lat2 = t_long2 = 0.0;

      scanresult = sscanf(keyval.data, "%f %f %f %f",
			  &t_lat1, &t_long1, &t_lat2, &t_long2);

      if (scanresult == 2) {
	t_lat2 = t_lat1;
	t_long2 = t_long1;
      }
      
#ifdef DEBUG

      printf("\nSRCH s_lat1 %011.6f s_long1 %011.6f  s_lat2 %011.6f s_long2 %011.6f\n", 
	     s_lat1, s_long1, s_lat2, s_long2);

      printf("DATA t_lat1 %011.6f t_long1 %011.6f  t_lat2 %011.6f t_long2 %011.6f\n", 
	     t_lat1, t_long1, t_lat2, t_long2);
#endif

      switch (geo_op) {
	
      case 7:
	/* OVERLAPS 
	 * The access point region has a geometric area in common 
	 * with the search term region. Given a
	 * search term region of S and access point region of T,
	 * the following algebra expresses the
	 * conditions required: 
	 * {S(North) >= T(South)} and {S(South) <= T(North)} 
	 * and {S(East) >= T(West)} and {S(West) <= T(East)}.
	 */
#ifdef DEBUG
	printf("OVERLAPS TESTS: s_lat1 >= t_lat2 == %d : s_lat2 <= t_lat1 == %d : s_long2 >= t_long1 == %d : s_long1 <= t_long2 == %d\n", (s_lat1 >= t_lat2),
	       (s_lat2 <= t_lat1), (s_long2 >= t_long1), (s_long1 <= t_long2));
#endif

	if (s_lat1 >= t_lat2 
	    && s_lat2 <= t_lat1
	    && s_long2 >= t_long1
	    && s_long1 <= t_long2) {
	  /* it overlaps ... */
	  ok_to_add = 1;	
#ifdef DEBUG
	  printf("**************** OVERLAP MATCH *****************\n\n");
#endif
	}


	if (db_errno == DB_NOTFOUND) 
	  more = 0;

	/* test stopping conditions ??? didn't work do full scan...
	 * if ((direction == DB_PREV && t_lat1 < s_lat2)
	 *	 || (direction == DB_NEXT && 
	 *	     ((t_lat1 < 0.0 && t_lat1 < s_lat2)
	 *            || (t_lat1 >= 0.0 && t_lat1  s_lat2))))
	 *  more = 0;
	 */
	
	break;
	
      case 11:
	/* Near
	 * The access point region falls within a default distance 
	 * of the search term region. The default distance is defined 
	 * by the server.
	 * Because of the initial values set, this is like Overlaps.
	 */
	
	{ 
	  /* for calculating great circle distance -- if necessary */
	  double pi = 3.14159265358979323846;
	  double rslat1, rslong1, rslat2, rslong2;
	  double rtlat1, rtlong1, rtlat2, rtlong2;
	  double dist1, dist2, dist3, dist4;
	  double c_lat, c_long, dist_cent1, dist_cent2;
	  
	  if (s_lat1 >= t_lat1 
	      && s_lat2 <= t_lat2
	      && s_long2 >= t_long2
	      && s_long1 <= t_long1) {
	    /* it is enclosed */
	    
#ifdef DEBUG
	    printf("ENCLOSED WITHIN match\n");
#endif
	    ok_to_add = 1;
	  }
	  else if (s_lat1 <= t_lat1 
		   && s_lat2 >= t_lat2
		   && s_long2 <= t_long2
		   && s_long1 >= t_long1) {
	    /* it encloses */
#ifdef DEBUG
	    printf("ENCLOSES match\n");
#endif
	    ok_to_add = 1;
	  }
	  else if (s_lat1 >= t_lat2 
		   && s_lat2 <= t_lat1
		   && s_long2 >= t_long1
		   && s_long1 <= t_long2) {
	    /* it overlaps ... */
#ifdef DEBUG
	    printf("OVERLAP Match\n");
#endif
	    ok_to_add = 1;
	  }
	  else {
	    
	    rslat1 = s_lat1 * ((2*pi)/360);
	    rslong1 = s_long1 * ((2*pi)/360);
	    rslat2 = s_lat2 * ((2*pi)/360);
	    rslong2 = s_long2 * ((2*pi)/360);
	    rtlat1 = t_lat1 * ((2*pi)/360);
	    rtlong1 = t_long1 * ((2*pi)/360);
	    rtlat2 = t_lat2 * ((2*pi)/360);
	    rtlong2 = t_long2 * ((2*pi)/360);
	    
	    /* calculate some distances */
	    /* should replace with Haversine formula - see se_georank.c */
	    dist1 = 3963.00 * acos(cos(rslat1) * cos(rtlat1)
				   * cos(rslong1 - rtlong1) 
				   + (sin(rslat1) * sin(rtlat1)));
	    
	    dist2 = 3963.00 * acos(cos(rslat1) * cos(rtlat2)
				   * cos(rslong1 - rtlong2) 
				   + (sin(rslat1) * sin(rtlat2)));
	    
	    dist3 = 3963.00 * acos(cos(rslat2) * cos(rtlat1)
				   * cos(rslong2 - rtlong1) 
				   + (sin(rslat2) * sin(rtlat2)));
	    
	    dist4 = 3963.00 * acos(cos(rslat2) * cos(rtlat2)
				   * cos(rslong2 - rtlong2) 
				   + (sin(rslat2) * sin(rtlat2)));
	    
	    c_lat = ((s_lat1 + s_lat2) / 2.0) * ((2*pi)/360);
	    c_long = ((s_long1 + s_long1) / 2.0) * ((2*pi)/360);
	    
	    dist_cent1 = 3963.00 * acos(cos(c_lat) * cos(rtlat1)
					* cos(c_long - rtlong1) 
					+ (sin(c_lat) * sin(rtlat1)));
	    
	    dist_cent2 = 3963.00 * acos(cos(c_lat) * cos(rtlat2)
					* cos(c_long - rtlong2) 
					+ (sin(c_lat) * sin(rtlat2)));
	    
#ifdef DEBUG
	    printf("  * dist1 %11.4f miles : dist2 %11.4f miles : dist3 %11.4f miles : dist4 %11.4f miles\n",
		   dist1, dist2, dist3, dist4);
	    printf("  * -- dist_cent1 %11.4f miles : dist_cent2 %11.4f miles\n",
		   dist_cent1, dist_cent2);
#endif
	    
	    if (dist1 < 50.00
		|| dist2 < 50.00
		|| dist3 < 50.00
		|| dist4 < 50.00
		|| dist_cent1 < 50.00
		|| dist_cent1 < 50.00) {
	      /* it is within 50 miles */
	      ok_to_add = 1;
#ifdef DEBUG
	      printf("DISTANCE MATCH\n", record_num);	    
#endif
	    }
	  }
	}

	if (db_errno == DB_NOTFOUND) 
	  more = 0;
	
	if (direction == DB_PREV && t_lat1 < 0) {
	  /* Change position and direction in the index */
#ifdef DEBUG
	  printf("+++++ REVERSING COURSE FOR getting South +++++\n");	    
#endif
	  keyval.data = (void *) "-000.000000";
	  keyval.size = 11;
	  db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	  direction = DB_NEXT;
	  direction_flag = 0;
	}
	else if ((direction == DB_PREV && t_lat1 < s_lat2)
		 || (direction == DB_NEXT && 
		     ((t_lat1 < 0.0 && t_lat1 < s_lat2)
		      || (t_lat1 >= 0.0 && t_lat1 > s_lat1))))
	  more = 0;
	
	break;
	
      case 8:
	/* Fully enclosed within 
	 * The access point region is fully enclosed within the search term 
	 * region.
	 * {S(North) >= T(North)} and {S(South) <= T(South)} 
	 * and {S(East) >= T(East)} and {S(West) <= T(West)}.
	 */
#ifdef DEBUG
	printf("FULLY ENCLOSED TESTS: s_lat1 >= t_lat1 == %d : s_lat2 <= t_lat2 == %d : s_long2 >= t_long2 == %d : s_long1 <= t_long1 == %d\n\n", (s_lat1 >= t_lat1),
	       (s_lat2 <= t_lat2), (s_long2 >= t_long2), (s_long1 <= t_long1));
#endif

	if (s_lat1 >= t_lat1 
	    && s_lat2 <= t_lat2
	    && s_long2 >= t_long2
	    && s_long1 <= t_long1) {
	  /* it is enclosed */
#ifdef DEBUG
	  printf("**************** FULLY ENCLOSED WITHIN MATCH *****************\n\n");
#endif
	  ok_to_add = 1;
	}
	/* this is wrong, but I have to look at stopping conditions */

	if (db_errno == DB_NOTFOUND) 
	  more = 0;

	if (direction == DB_PREV && t_lat1 < 0) {
	  /* Change position and direction in the index */
#ifdef DEBUG
	  printf("+++++ REVERSING COURSE FOR getting South +++++\n");	    
#endif
	  keyval.data = (void *) "-000.000000";
	  keyval.size = 11;
	  dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	  direction = DB_NEXT;
	  direction_flag = 0;
	}
	else if ((direction == DB_PREV && t_lat1 < s_lat2)
		 || (direction == DB_NEXT && 
		     ((t_lat1 < 0.0 && t_lat1 < s_lat2)
		      || (t_lat1 >= 0.0 && t_lat1 > s_lat1))))
	  more = 0;
	
	break;
	
      case 9:
	/* Encloses
	 * The access point region fully encloses the search term region
	 * {S(North) <= T(North)} and {S(South) >= T(South)} 
	 * and {S(East) <= T(East)} and {S(West) >= T(West)}.
	 */
#ifdef DEBUG
	printf("ENCLOSES TESTS: s_lat1 <= t_lat1 == %d : s_lat2 >= t_lat2 == %d : s_long2 <= t_long2 == %d : s_long1 >= t_long1 == %d\n\n", (s_lat1 <= t_lat1),
	       (s_lat2 >= t_lat2), (s_long2 <= t_long2), (s_long1 >= t_long1));
#endif

	if (s_lat1 <= t_lat1 
	    && s_lat2 >= t_lat2
	    && s_long2 <= t_long2
	    && s_long1 >= t_long1) {
	  /* it encloses */
#ifdef DEBUG
	  printf("**************** ENCLOSES MATCH *****************\n\n");
#endif
	  ok_to_add = 1;
	}


	if (direction == DB_PREV && s_lat1 < 0.0) {
	  /* Change position and direction in the index */
#ifdef DEBUG
	  printf("+++++ REVERSING COURSE FOR getting North +++++\n");	    
#endif
	  keyval.data = (void *) "0000.000000";
	  keyval.size = 11;
	  db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	  direction = DB_NEXT;
	  direction_flag = 0;
	}
	else if ((direction == DB_NEXT && db_errno == DB_NOTFOUND))
	  more = 0;
	
	break;

      case 10:
	/* Fully Outside Of
	 * The access point region has no geometric area in common with 
	 * the search term region.
	 */
	
#ifdef DEBUG
	printf("FULLY OUTSIDE TESTS (will not all be 1): s_lat1 >= t_lat2 == %d : s_lat2 <= t_lat1 == %d : s_long2 >= t_long1 == %d : s_long1 <= t_long2 == %d\n\n", (s_lat1 >= t_lat2),
	       (s_lat2 <= t_lat1), (s_long2 >= t_long1), (s_long1 <= t_long2));
#endif

	if (!(s_lat1 >= t_lat2 
	    && s_lat2 <= t_lat1
	    && s_long2 >= t_long1
	    && s_long1 <= t_long2)) {
	  /* it overlaps ... */
#ifdef DEBUG
	  printf("**************** FULLY OUTSIDE OF MATCH *****************\n\n");
#endif
	  ok_to_add = 1;
	}


	/* we started at the top, and can only stop at the end */
	if (db_errno == DB_NOTFOUND) 
	  more = 0;

	break;
	
      }
    }
    /* need to check here for date range overlap processing... */
    if (relation == 104) {
      if (idx->type & DATE_RANGE_KEY) { 
	/* test for all conditions */
	if (strcmp(end_key,c) < 0 
	    || strcmp(start_key,c+23) > 0) 
	  ok_to_add = 0;
	else
	  ok_to_add = 1;
      }
    }

    /* otherwise assume that the get was OK */
    if (ok_to_add && dataval.data != NULL 
	&& db_errno != DB_NOTFOUND
	&& c[0] != '>') {       /* skip the Global Data */
      
      /* allocate space for the posting info */
      newinfo = (idx_result *) 
	CALLOC(int, 
	       ((dataval.size + sizeof(idx_posting_entry)) / sizeof(int)));
      memcpy((char *)newinfo, (char *)dataval.data, dataval.size);
      
      for (i = 0; i < newinfo->num_entries; i++) {
	    
	record_num = newinfo->entries[i].record_no;

	if (record_num > 0) { /* negative is a deleted entry */
	  entry = Tcl_FindHashEntry(hash_tab, (void *)record_num);
		
	  if (entry == NULL){ /* new record number */
	    storage = CALLOC(struct temp_results,1);
	    storage->weight = 1;
	    storage->tot_freq = newinfo->entries[i].term_freq;
	    Tcl_SetHashValue(
			     Tcl_CreateHashEntry(
						 hash_tab,
						 (void *)record_num,
						 &exists),
			     (ClientData)storage);
	  }
	  else {
	    storage = (struct temp_results *) Tcl_GetHashValue(entry);
	    if(storage->weight < num_search_words)
	      storage->weight++;
	    storage->tot_freq += newinfo->entries[i].term_freq;
	  }
	}
      }
      FREE(newinfo);
    }

    /* get the next entry in the index (backwards or forwards */
    if(dbcursor->c_get(dbcursor, &keyval, &dataval, direction) != 0
       && direction_flag) {
      if (geo_op == 7 /* Overlap */ && direction == DB_PREV) {
	/* overlap scanning now runs from south to North, so if the 
	 * southmost search point was in the southern hemisphere, and
	 * we have reached the equator, we have to switch direction
	 * and position to get the northern hemisphere
	 */
#ifdef DEBUG
	printf("+++++ REVERSING COURSE FOR getting North +++++\n");	    
#endif
	keyval.data = (void *) "0000.000000";
	keyval.size = 11;
	db_errno = dbcursor->c_get(dbcursor, &keyval, &dataval, DB_SET_RANGE);
	direction = DB_NEXT;
	direction_flag = 0;
      } 
      else
	more = 0; 
      
    }
    if (direction_flag == 0) 
      direction_flag = 1;
    if (relation == 104  || (geo_op == 16)) {
      /* test for stopping condition */
      if (strcmp(keyval.data, end_key) > 0)
	more = 0;
    }

    tot_docs = max_hits = hash_tab->numEntries; 

  } while (more); 
    

  dbcursor->c_close(dbcursor);

  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + (max_hits * sizeof(weighted_entry))));
    
  wt_res->num_hits = 0;
  wt_res->setid[0] = '\0';

  if (idx->type & PAGEDINDEX) {
    wt_res->result_type = PAGED_RESULT;
  }

  if (idx->type & COMPONENT_INDEX) {
    wt_res->result_type = COMPONENT_RESULT;
    wt_res->component = idx->comp_parent;
  }
  
  i = 0;

  for (entry = Tcl_FirstHashEntry(hash_tab,&hash_search);
       entry != NULL; entry = Tcl_NextHashEntry(&hash_search)) {

    record_num = (int)Tcl_GetHashKey(hash_tab,entry); 
    storage = (struct temp_results *) Tcl_GetHashValue(entry);
      


    if (storage->weight >= (num_search_words - search_stopword_count)) {
      wt_res->entries[i].record_num = record_num;
      wt_res->num_hits++;
      wt_res->entries[i].weight = (float)storage->tot_freq;
      if (wt_res->max_weight == 0.0) 
	wt_res->max_weight = wt_res->entries[i].weight;
      else if (	wt_res->max_weight < wt_res->entries[i].weight)
	wt_res->max_weight = wt_res->entries[i].weight;
      if (wt_res->min_weight == 0.0) 
	wt_res->min_weight = wt_res->entries[i].weight;
      else if (wt_res->min_weight > wt_res->entries[i].weight)
	wt_res->min_weight = wt_res->entries[i].weight;

      i++;
    }
    free(storage); /* not needed any more */
  }

  if (search_ptr) FREE(search_ptr);

  Tcl_DeleteHashTable(hash_tab); /* clean up */

  return(se_sortwtset(wt_res));
}







