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
*	Header Name:	lat_long.h
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	header for latlong indexing and  handling -- includes
*                       symbolic definitions used in the configfiles
*	Usage:		#include lat_long.h
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		07/3/02
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2002.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

/* years BC are indicated by negative numberss */

typedef struct lat_long {
  int lat_degrees;
  int lat_minutes;
  int lat_seconds;
  int lat_direction;
  int long_degrees;
  int long_minutes;
  int long_seconds;
  int long_direction;
  float latitude;
  float longitude;
} lat_long;

typedef struct bounding_box {
  lat_long NW;
  lat_long SE;
} bounding_box;

/* Same structure is used for bounding boxes and lat_long points */
typedef struct lat_long_storage {
  float latitude1;
  float longitude1;
  float latitude2;
  float longitude2;
} lat_long_storage;


