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
*	Header Name:	cf_getcluster.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	get one or a list of cluster_list_entry items from
*                       the configfile structure and return a pointer to it.
*                       or return other info about an index (like type)
*                       
*	Usage:		cluster_list_entry *cf_getcluster(filename)
*       
*			cluster_list_entry *cf_getcluster_entry(filename, clustername)
*                            get a specific cluster by name;
*
*                       filename can be the full pathname or short "nickname"
*                       clustername should be the short or "nickname" for
*                         the specific index
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		1/8/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"

extern config_file_info *cf_info_base;
config_file_info *find_file_config(char *filename);
idx_list_entry *find_index_name(config_file_info *cf, char *indexname);

cluster_list_entry *cf_getcluster(char *filename)
{
  
  config_file_info *cf;

  if (cf = find_file_config(filename)) {
    return (cf->clusters);
  }
  return(NULL);
}

cluster_list_entry *cf_getcluster_entry(char *filename, char *clustername)
{
  cluster_list_entry *cluster;

  for (cluster = cf_getcluster(filename); cluster; 
       cluster = cluster->next_entry) {
    
    if (strcmp(cluster->name,clustername) == 0)
      return (cluster);
   
  }
  return(NULL);
}

/* given a file name, return the name of the file it clusters */
char *cf_getclustered_name(char *filename)
{
  
  config_file_info *cf;

  if (cf = find_file_config(filename)) {
    return(cf->clustered_file);
  }
  return(NULL);
}


/* given a file name, return the name of the file it clusters, if any */
config_file_info *cf_getclustered(char *filename)
{
  char *clusters_name;

  /* get the name of the file clustered */
  clusters_name = cf_getclustered_name(filename);

  /* find it and return the cf pointer */
  return(find_file_config(clusters_name));

}








