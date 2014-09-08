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
*	Header Name:	cf_getidx.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	get one or a list of idx_list_entry items from
*                       the configfile structure and return a pointer to it.
*                       or return other info about an index (like type)
*                       
*	Usage:		idx_list_entry *cf_getidx(filename)
*       
*			idx_list_entry *cf_getidx_entry(filename, indexname)
*                            get a specific index by tag.
*
*			int *cf_getidx_type(filename, indexname)
*                            get the index_type variable for a given index.
*
*                       idx_list_entry *cf_getidx_mapattr(char *filename,
*			         int attr_nums[], char *attr_str[], 
*                                ObjectIdentifier attr_oid[]);
*
*                            This function scans the config file info 
*                            to find an index that maps to a given set of
*                            Z39.50 attributes.
*
*                       filename can be the full pathname or short "nickname"
*                       indexname should be the short tag or "nickname" for
*                         the specific index
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                       structure holding the configfile info;
*
*	Return Conditions and Return Codes:	
*                       returns the file pointer or NULL on error;
*
*	Date:		10/31/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "zprimitive.h"
#ifdef WIN32
#define strcasecmp _stricmp
#endif
extern config_file_info *cf_info_base;
config_file_info *find_file_config(char *filename);
idx_list_entry *find_index_name(config_file_info *cf, char *indexname);
idx_list_entry *find_index_name_nowarn(config_file_info *cf, char *indexname);
extern void diagnostic_set();

idx_list_entry *cf_getidx(char *filename)
{
  
  config_file_info *cf;

  if (cf = find_file_config(filename)) {
      return (cf->indexes);

  }
  return(NULL);
}

idx_list_entry *cf_getidx_entry(char *filename, char *indexname)
{

  config_file_info *cf;

  if (cf = find_file_config(filename)) {
    return(find_index_name(cf, indexname));
  }
  return (NULL);
}

idx_list_entry *cf_getidx_entry_nowarn(char *filename, char *indexname)
{

  config_file_info *cf;

  if (cf = find_file_config(filename)) {
    return(find_index_name_nowarn(cf, indexname));
  }
  return (NULL);
}


/*********************************************************************
* cf_getidx_mapattr -- This function scans the config file info 
*                   to find an index that maps to a given set of
*                   Z39.50 attributes. It returns a pointer to the
*                   idx entry on success and a NULL otherwise.
*                   attributes specified as 0 will be ignored in matching
*                   and the FIRST matching index in the configfile will
*                   be the one returned.
* 
*********************************************************************/
#define USEMASK (1<<0)
#define RELMASK (1<<1)
#define POSMASK (1<<2)
#define STRMASK (1<<3)
#define TRUMASK (1<<4)
#define COMMASK (1<<5)
#define ACCESS_POINT_MASK (1<<0)
#define SEMANTIC_QUAL_MASK (1<<1)
#define LANGUAGE_ATTR_MASK (1<<2)
#define CONTENT_AUTH_MASK (1<<3)
#define EXPANSION_MASK (1<<4)
#define NORMALIZED_WEIGHT_MASK (1<<5)
#define HIT_COUNT_MASK (1<<6)
#define COMPARISON_MASK (1<<7)
#define FORMAT_MASK (1<<8)
#define OCCURRENCE_MASK (1<<9)
#define INDIRECTION_MASK (1<<10)
#define FUNC_QUAL_MASK (1<<11)

idx_list_entry *cf_getidx_mapattr(char *filename,
				  int in_attr[], 
				  char *in_strattr[],
				  ObjectIdentifier in_oid[])

{
  attr_map *map;
  config_file_info *cf;
  int testmask, match;  
  int use;
  int rel_feedback;
  int i;

  use = in_attr[USE_ATTR];

  if (use == -1) {
    /* should never get here -- but if it does... */
    return(cf_getidx_entry(filename, in_strattr[USE_ATTR]));

  }
  if (use == -2) {
    /* except in special cases, like relevance feedback */
    /* this code is probably no longer reachable, but just in case... */
    /* used to be use = -1, now preempted by local index names        */
    use = 0;
    in_attr[USE_ATTR] = 0;
    rel_feedback = 1;
  }
  else
    rel_feedback = 0;

  /* set the bit mask for matching */
  testmask = 0;
  match = 0;
  for (i = 0; i < MAXATTRTYPE; i++) {
    if (i !=(MAXATTRTYPE-1) && in_attr[i+1] != 0)
      testmask |= 1<<i;
  }

  
  if (cf = find_file_config(filename)) {
    for (map = cf->bib1_attr_map; map; map = map->next_attr_map_file) {
      if (map->attr_nums[USE_ATTR] == use || use == 0) {
	/* candidate entry -- at least the index has the contents wanted */
	if (testmask == 0 && use == 0 && map->attr_nums[USE_ATTR] == 0) /* default index */
	  return (map->idx_entry);
	
	/* Otherwise test each of the attribute and values. */
	
	
	for (i = 0; i < MAXATTRTYPE; i++) {
	  if (testmask & 1<<i) {
	    /* need to match this type */
	    if ((in_oid[i+1] == NULL
		|| (map->attr_set_oid[i+1] == NULL
		    && strcmp(in_oid[i+1]->data, map->attributeSetOID) == 0) 
		|| (map->attr_set_oid[i+1] != NULL 
		    && strcmp(in_oid[i+1]->data, map->attr_set_oid[i+1]) == 0))
		&& ((in_attr[i+1] != 0 &&
		     map->attr_nums[i+1] != 0 &&
		     in_attr[i+1] == map->attr_nums[i+1])
		    || (in_strattr[i+1] != NULL && 
			map->attr_string[i+1] != NULL &&
			strcasecmp(in_strattr[i+1],map->attr_string[i+1])))) {
	      /* Match here */
	      match |= 1<<i;
	    }
	    
	  }
	}
	
	if ((match & testmask) == testmask) 
	  return (map->idx_entry);
      }
    }
  }
  diagnostic_set(123,0,NULL);
  return(NULL);
}


int cf_getidx_type(char *filename, char *indexname)
{
  idx_list_entry *idx;

  idx = cf_getidx_entry(filename, indexname);

  if (idx)
    return (idx->type);
  else
    return (0);
}










