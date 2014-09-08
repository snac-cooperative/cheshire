/* Cursors.c  -- This stuff handles coordinate conversions for the */
/* map widget                                                      */
/*   by Ray R. Larson  - Mar 1995                                  */
/* Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 */

#include <stdio.h>
#include <math.h>
#include "tcl.h"
#include <signal.h>

/*****************************************************************
 * ParseMapCoord -- parse a latitude or longitude specification
 *
 * Results -- long value specifying lat or long in "seconds"
 *****************************************************************/
long ParseMapCoord(char *coordspec)
{
  long ldeg = 0, lmin = 0, lsec = 0;
  long value = 0;
  char dir, dummy;
  int sign;
  
  sign = 1;
  if (sscanf(coordspec,"%do%d'%d''%c", &ldeg, &lmin, &lsec, &dir) == 4) {
    if (ldeg < 0) {
      sign = -1;
      ldeg = ldeg * -1;
    }
    value = (ldeg * 3600) + (lmin * 60) + lsec;
    
    /* do some error checks and direction setting */
    
    if (dir == 'S' || dir == 's') {
      /* must be a latitude */
      sign = -1;
      if (value > (90 * 3600))
	value = 90*3600;
    }
    if (dir == 'N' || dir == 'n') {
      /* must be a latitude */
      if (value > (90 * 3600))
	value = 90*3600;
    }
    if (dir == 'W' || dir == 'w') {
      /* must be a longitude */
      sign = -1;
      if (value > (180 * 3600))
	value = 180*3600;
    }
    if (dir == 'E' || dir == 'e') {
      /* must be a longitude */
      if (value > (180 * 3600))
	value = 180*3600;
    }
    
    return (sign * value);
  }

  return(0);
}


int
CNVT_dfloat_latlon(ClientData inSession, 
		   Tcl_Interp *interp, 
		   float in_lat,
		   float in_lon)
  {
  int lat_sign, lat_deg, lat_min, lat_sec, lon_sign, lon_deg, lon_min, lon_sec;
  float lat_secf, lon_secf;
  char result_string[50];

  lon_deg = abs((int)in_lon);
  
  if (in_lon >= 0) 
    lon_sign = 1;
  else 
    lon_sign = -1;

  lon_secf = ((in_lon * (float)lon_sign) - (float)lon_deg)  * 3600.0 ;

  lon_min = (int) lon_secf/60;

  lon_sec = (int) (lon_secf - ((float)lon_min * 60.0));

  lon_secf = lon_secf - lon_sec;

  lat_deg = abs((int)in_lat);
  
  if (in_lat >= 0) 
    lat_sign = 1;
  else 
    lat_sign = -1;

  lat_secf = ((in_lat * (float)lat_sign) - (float)lat_deg)  * 3600.0 ;

  lat_min = (int) lat_secf/60;

  lat_sec = (int) (lat_secf - ((float)lat_min * 60.0));

  lat_secf = lat_secf - lat_sec;


  sprintf(result_string, "%do%d'%d''%s %do%d'%d''%s",
	  lat_deg, lat_min, lat_sec, ((lat_sign > 0) ? "N":"S"),
	  lon_deg, lon_min, lon_sec, ((lon_sign > 0) ? "E":"W"));

  /* append to the results */
  Tcl_AppendResult(interp, result_string, (char *) NULL);

  return TCL_OK;
}



/************** Convert from float degrees to lat/lon  *******************/
int
MapCNVT_dfloat_lonlat(ClientData inSession /* Current Session */,
		      Tcl_Interp *interp /* Current interpreter. */,
		      int argc /* Number of arguments. */,
		      char **argv /* Argument strings. */)
{

  float in_lat, in_lon;

  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " longitude latitude\" (in degrees)", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[1],"%f",&in_lon) != 1) {
    Tcl_AppendResult(interp, argv[0], ": longitude arg should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[2],"%f",&in_lat) != 1) {
    Tcl_AppendResult(interp, argv[0], ": latitude arg should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }
  
  return(CNVT_dfloat_latlon(inSession,interp,(float)in_lat,(float)in_lon));

}

int
MapCNVT_dfloat_latlon(ClientData inSession /* Current Session */,
		      Tcl_Interp *interp /* Current interpreter. */,
		      int argc /* Number of arguments. */,
		      char **argv /* Argument strings. */)
{

  float in_lat, in_lon;

  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " latitude longitude\" (in degrees)", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[1],"%f",&in_lat) != 1) {
    Tcl_AppendResult(interp, argv[0], ": latitude arg should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[2],"%f",&in_lon) != 1) {
    Tcl_AppendResult(interp, argv[0], ": longitude arg should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }
  
  return(CNVT_dfloat_latlon(inSession,interp,(float)in_lat,(float)in_lon));

}


int
MapCNVT_lonlat_dfloat(ClientData inSession /* Current Session */,
		      Tcl_Interp *interp /* Current interpreter. */,
		      int argc /* Number of arguments. */,
		      char **argv /* Argument strings. */)
{

  float lat_degf, lon_degf;
  long lat_sec, lon_sec;
  char result_string[50];

  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " longitude latitude\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  lon_sec = ParseMapCoord(argv[1]);
  lat_sec = ParseMapCoord(argv[2]);

  /* these return somewhat strange values using in mapwidget */

  lon_degf = (float)lon_sec /3600.0;
  lat_degf = (float)lat_sec /3600.0;
  
  sprintf(result_string, "%f %f", lat_degf, lon_degf);

  /* append to the results */
  Tcl_AppendResult(interp, result_string, (char *) NULL);

  return TCL_OK;
}

int
MapCNVT_latlon_dfloat( ClientData inSession /* Current Session */,
		       Tcl_Interp *interp /* Current interpreter. */,
		       int argc /* Number of arguments. */,
		       char **argv /* Argument strings. */)
{

  float lat_degf, lon_degf;
  long lat_sec, lon_sec;
  char result_string[50];

  if (argc < 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " longitude latitude\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  lat_sec = ParseMapCoord(argv[1]);
  lon_sec = ParseMapCoord(argv[2]);

  /* these return somewhat strange values using in mapwidget */

  lon_degf = (float)lon_sec /3600.0;
  lat_degf = (float)lat_sec /3600.0;
  
  sprintf(result_string, "%f %f", lat_degf, lon_degf);

  /* append to the results */
  Tcl_AppendResult(interp, result_string, (char *) NULL);

  return TCL_OK;
}


int
MapDIST_dfloat( ClientData inSession /* Current Session */,
		       Tcl_Interp *interp /* Current interpreter. */,
		       int argc /* Number of arguments. */,
		       char **argv /* Argument strings. */)
{

  float lat_degf1=0.0, lon_degf1=0.0, lat_degf2=0.0, lon_degf2=0.0;
  /* for calculating great circle distance -- if necessary */
  double pi = 3.14159265358979323846;
  double rslat, rslong;
  double rtlat, rtlong;
  double dist1, dist2;

  char result_string[50];
  int resulttype = 0;


  if (argc < 5) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
		     argv[0], " latitude1 longitude1 latitude2 longitude2 (coordinates in decimal degrees)\"", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (strcmp(argv[0], "geo_distance_km") == 0) {
    resulttype = 1;
  }
  else {
    resulttype = 0;
  }


  if (sscanf(argv[1],"%f",&lat_degf1) != 1) {
    Tcl_AppendResult(interp, argv[0], ": latitude args should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[2],"%f",&lon_degf1) != 1) {
    Tcl_AppendResult(interp, argv[0], ": longitude args should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }


  if (sscanf(argv[3],"%f",&lat_degf2) != 1) {
    Tcl_AppendResult(interp, argv[0], ": latitude args should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }

  if (sscanf(argv[4],"%f",&lon_degf2) != 1) {
    Tcl_AppendResult(interp, argv[0], ": longitude args should be in degrees", 
		     (char *) NULL);
    return TCL_ERROR;
  }



  rslat = (double)(lat_degf1 * ((2*pi)/360));
  rslong = (double)(lon_degf1 * ((2*pi)/360));
  rtlat = (double)(lat_degf2 * ((2*pi)/360));
  rtlong = (double)(lon_degf2 * ((2*pi)/360));

  /* calculate the distance */

  if (resulttype == 1) {
    /* convert to klicks */
    dist1 = 6378.7 * acos(cos(rslat) * cos(rtlat)
			   * cos(rslong - rtlong) 
			   + (sin(rslat) * sin(rtlat)));
  }
  else {
    dist1 = 3963.00 * acos(cos(rslat) * cos(rtlat)
			   * cos(rslong - rtlong) 
			   + (sin(rslat) * sin(rtlat)));

  }

  
  sprintf(result_string, "%f", dist1);

  /* append to the results */
  Tcl_AppendResult(interp, result_string, (char *) NULL);

  return TCL_OK;
}






