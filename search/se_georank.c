/* #define DEBUG 1  */
/* #define DEBUGGEO 1 */

/*
 *  Copyright (c) 1990-2007 [see Other Notes, below]. The Regents of the
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
 *             School of Information 
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
*	Header Name:	se_georank.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Ranked Geographic Search
*
*	Usage:		se_georank (index_struct, term, relation,
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
			   int *morphflag);


weighted_result
*se_georank (idx_list_entry *idx,  char *searchstring,
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
  
  /* ADDED FOR GEO_SCORE - begin */
  float o_lat1, o_lat2, o_long1, o_long2;
  float search_area;
  float target_area;
  float olap_area;
  float s_olap;
  float t_olap;
  float geo_score;
  float sctr_lat;
  float sctr_long;
  float tctr_lat;
  float tctr_long;
  float p1_lat;
  float p1_long;
  float p2_lat;
  float p2_long;
  float dLat;
  float dLong;
  float geo_a;
  float geo_c;
  float geo_dist;
  float fraction_maxdist;
  int maxdist_km;
  int earth_radius_km;
  float pi;
  float geo_score2;

  double log_odds;
  float geo_score_lr;
  float geo_score_lradj;
  float geo_constant;
  float solap_coef;
  float tolap_coef;
  float dist_coef;
  
  /* ADDED FOR GEO - end */
  
  int scanresult;
  char tempgeokey[64];
  int ok_to_add;
  int direction_flag = 1;
  
  /* CHANGED FOR GEO_SCORE  */
  /* changing storage->weight to a float to accommodate fraction of overlap */
  struct temp_results {
    /* int weight; */
    float weight;
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
    normalize_key(searchstring, idx, &morphed);
  
  if (search_ptr == NULL && idx->stopwords_file != NULL) {
    /* this is REALLY ugly to do this way -- */
    /* stopword -- since this is a range search we continue... */
    Tcl_HashTable temp_stop_data,  stoplist_hash;
    
    stoplist_hash = idx->stopwords_hash;
    
    Tcl_InitHashTable(&temp_stop_data,TCL_STRING_KEYS);
    
    idx->stopwords_hash = temp_stop_data;
    search_stopword_count++;
    
    search_ptr =
      normalize_key(searchstring, idx, &morphed);
    
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
  
  /* PATTY: deleted a bunch of if/else statements that are not currently applicable. */
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

	/* PATTY: deleted a bunch of if/else statements that are not currently applicable.
	*  BUT probably deleted stuff that could be used for potentially georanked searches
	*  involving a single coordinate, like find all docs GT/LT/GTE/LTE a single coordinate
	*  The cases below only deal with a coordinate pair or bounding box
	*/

 if (geo_op >= 707) {
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
	end_key = normalize_key(tempgeokey, idx, &morphed);
      }
      else if (strlen(searchstring) > 30) {
	char *save;
	char *newsrchptr;

	save = idx->dateformat;
	idx->dateformat =
	  strdup("DDoMM'SS''NS DDDoMM'SS''EW DDoMM'SS''NS DDDoMM'SS''EW");
	newsrchptr =  normalize_key(searchstring, idx, &morphed);
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

    case 707:
      /* OVERLAPS_RANK
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


    case 708:
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

    case 709:
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

      }
    ok_to_add = 0; /* need to test before adding these */

  }


  /* Allocate the hash table for collecting weights for each document */
  hash_tab = &hash_tab_data;
  Tcl_InitHashTable(hash_tab,TCL_ONE_WORD_KEYS);

  do {

    c = keyval.data;

    if (geo_op == 707 || geo_op == 708 || geo_op == 709) {

      ok_to_add = 0;
      t_lat1 = t_long1 = t_lat2 = t_long2 = 0.0;

     /* Added for geo score calculations*/
      o_lat1 = o_long1 = o_lat2 = o_long2 = 0.0;
      search_area = target_area = olap_area = s_olap = t_olap = geo_score = 0.0;
      
      
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
	
      case 707:
	/* OVERLAPS_RANK
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
	
      case 708:
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

      case 709:
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
	    
	    /* CALCULATING GEO_SCORE - begin ************************************ */
	    if (geo_op == 707 || geo_op == 708 || geo_op == 709) {
	      
	      /* NEED TO CALCULATE GEO_SCORE TO USE AS WEIGHT
	       * geo_score = .8(s_olap) + .2(t_olap)
	       * where:
	       s_loap = fraction of overlap relative to search area
	       t_olap = fraction of overlap relative to TARGET area
	       Note: even the the calculated areas are only used in
	       a relative sense, area calculations based on decimal degrees
	       will introduce errors, esp. w/very large diffs in lat.
	       May be better in future to set some limits or define by zone.
	       
	       case 707: overlaps
	       cases 708 & 709 are special cases of 707
	       case 708: encloses, S contains T, 	t_olap is always 1
	       case 709: enclosed by, T contains S, 	s_olap is always 1
	       * ************************************************************** */
	      
	      /* olap_area - compute area of overlap between search and target regions  */
	      /* Note that
	       *   North = lat1               South = lat2
	       *   West = long1               East = long2
	       */
	      
	      /* check & correct target bounding boxes that are really points or lines
	       * by creating a little box around them. Should avoid divide by zero errors
	       */
	      if (t_lat1 == t_lat2) {
		t_lat1 = t_lat1 + .0001;
		t_lat2 = t_lat2 - .0001;
	      }
	      if (t_long1 == t_long2) {
		t_long1 = t_long1 - .0001 ;
		t_long2 = t_long2 + .0001 ;
	      }
	      
	      o_lat1 = s_lat1;
	      o_lat2 = s_lat2;
	      o_long2 = s_long2;
	      o_long1 = s_long1;
	      
	      if (s_long1 < t_long1) { o_long1 = t_long1; }
	      if (s_long2 > t_long2) { o_long2 = t_long2;}
	      if (s_lat2 < t_lat2) { o_lat2 = t_lat2; }
	      if (s_lat1 > t_lat1) { o_lat1 = t_lat1; }
	      
	      olap_area = fabs(o_long2 - o_long1) * fabs(o_lat1 - o_lat2);
	      
	      /* s_olap - compute fraction of overlap relative to search area  */
	      if (geo_op == 709) {
		s_olap = 1;
	      } else {
		search_area = fabs(s_long2 - s_long1) * fabs(s_lat1 - s_lat2);
		s_olap = olap_area / search_area;
	      }
	      
	      /* t_olap - compute fraction of overlap relative to TARGET area  */
	      if (geo_op == 708) {
		t_olap =1;
	      } else {
		target_area = fabs(t_long2 - t_long1) * fabs(t_lat1 - t_lat2);
		t_olap = olap_area / target_area;
	      }
	      
	      /* Compute geo_score
	       * geo_score = .8(s_olap) + .2(t_olap)
	       * PATTY: these weights are currently guestimates
	       */
	      
	      /* geo_score = (.8 * s_olap) + (.2 * t_olap); */
	      
	      
	      /* ***********************************************************************
	       * Add distance b/w BBOX centers as a factor to the geo score calculation
	       * Note: distance is calculated in KM
	       * ***********************************************************************
	       */
	      
	      sctr_lat = sctr_long = tctr_lat = tctr_long = 0;
	      p1_lat = p1_long= p2_lat = p2_long = 0;
	      dLat = dLong = 0;
	      geo_a = geo_c = geo_dist = fraction_maxdist = 0;
	      /* Note re maxdist_km: this is application specific.
	       * It is the max distance b/w any two bboxes in CA.
	       */
	      maxdist_km = 1420;
	      earth_radius_km = 6378; /* equatorial radius, approx 3963 miles */
	      pi = 3.14159265;
	      
	      /* geo_score2 = 0; */
	      
	      
	      /* compute center points for search and target bboxes */
	      /* calculate search bbox center point */
	      sctr_lat = s_lat2 + (abs(s_lat1 - s_lat2)/2);
	      sctr_long= s_long1 + (abs(s_long2 - s_long1)/2);
	      /* calculate target bbox center point */
	      tctr_lat = t_lat2 + (abs(t_lat1 - t_lat2)/2);
	      tctr_long= t_long1 + (abs(t_long2 - t_long1)/2);
	      
	      /* convert decimal degress to radians where rad = (dd/180) * PI */
	      p1_lat = (sctr_lat / 180) * pi;
	      p1_long = (sctr_long / 180) * pi;
	      p2_lat = (tctr_lat  / 180) * pi;
	      p2_long = (tctr_long / 180) * pi;
	      
	      /* This code from http://www.movable-type.co.uk/scripts/LatLong.html
	       * Calculate distance (in km) between two points specified by latitude/longitude.
	       *
	       * from: Haversine formula - R. W. Sinnott, "Virtues of the Haversine",
	       *       Sky and Telescope, vol 68, no 2, 1984
	       *       http://www.census.gov/cgi-bin/geo/gisfaq?Q5.1
	       */
	      
	      dLat  = p2_lat - p1_lat;
	      dLong = p2_long - p1_long;
	      
	      geo_a = sin(dLat/2) * sin(dLat/2) + cos(p1_lat) * cos(p2_lat) * sin(dLong/2) * sin(dLong/2);
	      geo_c = 2 * atan2(sqrt(geo_a), sqrt(1-geo_a));
	      geo_dist = earth_radius_km * geo_c;
	      
	      /* some minor adjustments to compute FRACTION MAX DISTANCE */
	      if ( geo_dist < 1) {
		fraction_maxdist = 0;
	      } else if (geo_dist >= maxdist_km) {
		fraction_maxdist = 1;
	      } else {
		fraction_maxdist = geo_dist / maxdist_km;
	      }
	      
	      /* ADJUST GEO SCORE */
	      /* sometimes the geoscore adjusted by dist will be negative, not sure if ok */
	      /* geo_score2 = geo_score - (.15 * fraction_maxdist); */
	      
	      /* PATTY NEW LOG REG STUFF */
	      
	      log_odds = 0;
	      geo_score_lr = 0;
	      
	      /* geo_score_lradj = 0; */
	      
	      /* TRY ONE VALUES ****** pre paper */
	      /* ********************
		 geo_constant = -1.6995;
		 solap_coef = 3.5534; 
		 tolap_coef = 2.8506;
		 dist_coef = -1.6682;
	      ***********************/
	      
	      /* GIR paper values updated 032404 */
	      /* CEIC All	MBR - LR 1	-1.6747	1.9871	3.2976 */
	      geo_constant = -1.6747;
	      solap_coef = 1.9871; 
	      tolap_coef = 3.2976;
	      
	      /* log_odds = (float) geo_constant + (solap_coef * s_olap) + (tolap_coef * t_olap) + (dist_coef * fraction_maxdist); */
	      log_odds = (float) geo_constant + (solap_coef * s_olap) + (tolap_coef * t_olap);
	      geo_score_lr = (exp(log_odds) / (1 + exp(log_odds)));
	      
	      /* geo_score_lradj = 0.75 * (exp(log_odds) / (1 + exp(log_odds))); */
	      
	      /* ASSIGN GE0_SCORE to the Storage weight */
	      /* storage->weight = geo_score; */
	      storage->weight = geo_score_lr;
	      
#ifdef DEBUGGEO
	      printf("\n+++++ GEO SCORE +++++\n");
	      printf("S_olap Score is [%2.6f]\n", s_olap);
	      printf("T_olap Score is [%2.6f]\n", t_olap);
	      printf("Distance b/w search and target bbox ctr pts is: [%6.4f] km \n", geo_dist);
	      printf("Fraction max distance possible w/in CA: [%6.4f] \n", fraction_maxdist);
	      printf("GEO Score  is [%2.6f]\n", geo_score);
	      printf("GEO Score2 (dist) is [%2.6f]\n", geo_score2);
	      printf("LogReg Score is [%2.6f]\n", geo_score_lr);
	      printf("LogReg ADJ Score is [%2.6f]\n", geo_score_lradj);
	      printf("Storage weight is [%2.6f]\n", storage->weight);
#endif
	    } else {
	      storage->weight = 1;
	    }
	    
	    
	    
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
      if (geo_op == 707 /* Overlap */ && direction == DB_PREV) {
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
    
#ifdef DEBUGGEO
    printf("Storage weight is [%2.6f]\n", storage->weight);
    printf("Num Search Words is [%d]\n", num_search_words);
    printf("totfreq is [%d]\n", storage->tot_freq);
    printf("I_val is [%d], \t recnum is [%d]\n", i, record_num);
#endif
    
    if (storage->weight >= (num_search_words - search_stopword_count)) {
      
      wt_res->entries[i].record_num = record_num;
      wt_res->num_hits++;
      
      if (geo_op == 707 || geo_op == 708 || geo_op == 709) {
	wt_res->entries[i].weight = storage->weight;
	wt_res->result_type = GEORANK_RESULT;
      } else {
	wt_res->entries[i].weight = (float)storage->tot_freq;
      }
      
      
#ifdef DEBUGGEO
      printf("WEIGHT Score right before exit is [%8.5f]\n", wt_res->entries[i].weight);
#endif
      
      i++;
    }
    free(storage); /* not needed any more */
  }
  
  if (search_ptr) FREE(search_ptr);
  
  Tcl_DeleteHashTable(hash_tab); /* clean up */
  
  return(se_rank_docs(wt_res));
  
}

