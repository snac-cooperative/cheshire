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
*	Header Name:	cf_free_info.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine frees the allocated memory of the config_
*                       info_file structure for the CHESHIRE II system
*	Usage:		cf_free_info(config_file_info *)
*                       
*	Variables:	see cheshire.h
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/20/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "dmalloc.h"

cf_free_info (config_file_info *cf)
{
  if (cf == NULL) return;

  cf_free_info(cf->next_file_info);
  cf_free_indexes(cf->indexes);
  cf_free_clusters(cf->clusters);
  cf_free_attrmap(cf->bib1_attr_map);
  cf_free_display(cf->display);
  if (cf->filecont) cf_free_cont(cf->filecont);    
  if (cf->file_name) FREE(cf->file_name);
  if (cf->nickname) FREE(cf->nickname);
  if (cf->assoc_name) FREE(cf->assoc_name);
  if (cf->history_name) FREE(cf->history_name);
  if (cf->DTD_name) FREE(cf->DTD_name);
  if (cf->SGML_Catalog_name) FREE(cf->SGML_Catalog_name);
  if (cf->clustered_file) FREE(cf->clustered_file);
  FREE(cf);
 
}

cf_free_indexes(idx_list_entry  *idx)
{
  if (idx == NULL) return;
 
  cf_free_indexes(idx->next_entry);

  cf_free_subkeys(idx->keys);
  cf_free_cont(idx->indxcont);

  if(idx->tag) FREE(idx->tag);
  if(idx->name) FREE(idx->name);

  if(idx->stopwords_name) FREE(idx->stopwords_name);

  if (idx->stopwords_hash.buckets != NULL)
    Tcl_DeleteHashTable(&(idx->stopwords_hash));
  
  FREE(idx);

  }


cf_free_clusters(cluster_list_entry  *clus)
{
  if (clus == NULL) return;
 
  cf_free_clusters(clus->next_entry);

  cf_free_subkeys(clus->cluster_key);
  cf_free_clus_map(clus->field_map);

  if(clus->name) FREE(clus->name);
  if(clus->temp_file_name)  FREE(clus->temp_file_name);
  if(clus->cluster_stoplist_name) FREE(clus->cluster_stoplist_name);
  
  if (clus->stopwords_hash.buckets != NULL)
    Tcl_DeleteHashTable(&(clus->stopwords_hash));

  FREE(clus);

  }

cf_free_clus_map(cluster_map_entry *map)
{
  if (map == NULL) return;

  cf_free_clus_map(map->next_clusmap);

  cf_free_subkeys(map->from);
  cf_free_subkeys(map->to);
  cf_free_subkeys(map->summarize);
  FREE(map);

}

cf_free_display(display_format *format)
{
  if (format == NULL) return;

  cf_free_display(format->next_format);

  cf_free_subkeys(format->include);
  cf_free_subkeys(format->exclude);
  cf_free_clus_map(format->convert);

  if (format->name)
    FREE(format->name);

  if (format->convert_name)
    FREE(format->convert_name);

  FREE(format);

}

cf_free_attrmap(attr_map *map)
{
  if (map == NULL) return;
 
  cf_free_attrmap(map->next_attr_map);

  if (map->altdescription)
    FREE(map->altdescription);

  FREE(map);
  
}

cf_free_subkeys (idx_key *subkey)
{
  if (subkey == NULL) return;

  /* free nested subkeys */
  cf_free_subkeys(subkey->next_key);
  cf_free_subkeys(subkey->subkey);


  if (subkey->key)
    FREE(subkey->key);
  FREE(subkey);
    
}

/* sequentially free file continuation structures */
cf_free_cont(continuation *cont)
{
  continuation *last_cont, *c;

  if (cont == NULL) return;
  last_cont = cont;
  for (c = cont->next_cont; c != NULL; c = c->next_cont) {
    FREE(last_cont->name);  
    FREE(last_cont);
    last_cont = c;
  }
}

