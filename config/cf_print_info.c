/*
 *  Copyright (c) 1990-1999 [see Other Notes, below]. The Regents of the
 *   University of California (Regents). All Rights Reserved.
 *   
 *   Permission to use, copy, modify, and distribute this software and its
 *   documentation for educational, research, and not-for-profit purposes,
 *   without fee and without a signed licensing agreement, is hereby
 *   granted, provided that the above copyright notice, this paragraph and
 *   the following two paragraphs appear in all copies, modifications, and
 *   distributions. Contact The Office of Technology Licensing, UC
 *   Berkeley, 2150 Shattuck Avenue, Suite 510, Berkeley, CA 94720-1620,
 *   (510) 643-7201, for commercial licensing opportunities. 
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
*	Header Name:	cf_print_info.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine prints the contents of the config_
*                       info_file structure for the CHESHIRE II system
*	Usage:		cf_print_info(config_file_info *)
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
#include "configfiles.h"

void cf_print_subkeys();
void cf_print_format();

void
cf_print_info (config_file_info *cf_info)
{
  config_file_info *cf;
  idx_list_entry  *idx;
  component_list_entry *comp;
  cluster_list_entry *clus;
  cluster_map_entry *cmap;
  display_format *format;
  continuation *cont;
  int i;  
  ranking_parameters *rnk;
  idx_key *key;
  attr_map *map;
  filelist *f;

  for (cf = cf_info; cf; cf = cf->next_file_info) {
    if (cf->defaultpath) printf( "\n<DefaultPath> %s </DefaultPath>\n", cf->defaultpath);
    if (cf->file_name) printf( "\n<filename> %s </filename>\n", cf->file_name);
    if (cf->nickname) printf( "<filetag> %s </filetag> \n", cf->nickname);
    if (cf->othernames) {
      for (f = cf->othernames; f ; f = f->next_filename) 
	printf( "\n<filetag> %s </filetag> <!-- ALIAS -->\n", f->filename);
    }
    if (cf->file_type) printf( "<filetype> %d </filetype>\n", cf->file_type);
    for (cont = cf->filecont; cont; cont = cont->next_cont) 
      {
    	printf( "   <filecont id=%d min=%d max=%d> %s </filecont>\n",
	       cont->id_number, cont->docid_min, cont->docid_max, cont->name);
      }
    if (cf->assoc_name) printf( "<assocname> %s </assocname>\n", cf->assoc_name);
    if (cf->history_name) printf( "<historyname> %s </historyname>\n", cf->history_name);
    if (cf->DTD_name) { 
      printf( "<filedtd");
      if (cf->XML_Schema == 2)
	printf( " TYPE=\"XMLSchema\"");
      else if (cf->XML_Schema == 1)
	printf( " TYPE=\"XML\"");
      printf( "> %s \n", cf->DTD_name);
    }
    else printf( "<filedtd> NO DTD SPECIFIED \n");

    if (cf->XML_Schema_Name) { 
      printf( "<XMLSchema> %s </XMLSchema>");
    }

    if (cf->SGML_Catalog_name) printf( "<SGMLCatalog> %s </SGMLCatalog>\n", 
				       cf->SGML_Catalog_name);
    else printf( "<SGMLCAT> NO SGML CATALOG SPECIFIED </SGMLCAT> \n");

    if (cf->primary_key && cf->primary_key->tag) 
      printf( "PRIMARY KEY INDEX is %s\n", cf->primary_key->tag);

    if (cf->distrib_docsize_key && cf->distrib_docsize_key->key) 
      printf( "Distributed Doc Size tag is %s\n", cf->distrib_docsize_key->key);

    printf("USE attribute mappings:\n");   
    for (i = 1; i < 1050; i++) {
      if (USE_BIT_ON(cf,i))
	printf("%d ",i);
    }
    printf("\n");

    printf("FULL attribute mappings:\n");   
    printf ("\tUSE\tRELAT\tPOSIT\tSTRUCT\tTRUNC\tCOMPLET\n");
    printf ("\tor\tor\tor\tor\tor\tor\n");
    printf ("\tACCESS\tSEMANT\tLANG\tAUTHRTY\tEXPANS\tWEIGHT\tHITS\tCOMPAR\tFORMAT\tOCCUR\tINDIR\tFUNCTION\t(INDEX)\t(attributeset)\n");
    for (map = cf->bib1_attr_map; map; map = map->next_attr_map_file) {
      printf("\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d (%s)(%s)\n", map->attr_nums[1],
	     map->attr_nums[2], map->attr_nums[3],
	     map->attr_nums[4],map->attr_nums[5],
	     map->attr_nums[6],map->attr_nums[7],
	     map->attr_nums[8],map->attr_nums[9],
	     map->attr_nums[10],map->attr_nums[11],
	     map->attr_nums[12],
	     map->idx_entry->tag, map->attributeSetOID);
      if (map->altdescription)
	printf("\t\tLOCAL DESCRIPTION: %s\n", map->altdescription); 
      
    }

    printf( "Number of Indexes %d \n", cf->num_indexes);

    for (idx = cf->indexes; idx ; idx = idx->next_entry) {
      
      if (idx->type) printf( "\n\t<indextype> %d", idx->type);
      if (idx->type & DB_DBMS_TYPE) {
	printf( " - DBMS"); /* external DBMS */
	if ((idx->type & DB_MASK) == DB_MASK)
	  printf("+BTREE");
      }
      else {
	if ((idx->type & DB_MASK) == DB_BTREE_TYPE)  printf( " - BTREE");
	if ((idx->type & DB_MASK) == DB_HASH_TYPE)  printf( " - HASH");
	if ((idx->type & DB_MASK) == VECTOR_TYPE)  printf( " - VECTOR");
      }

      if (idx->type & BITMAPPED_TYPE)  printf( " - BITMAPPED");
      if (idx->type & KEYWORD)  printf( " KEYWORD");
      if (idx->type & EXACTKEY)  printf( " EXACTKEY");
      if (idx->type & GEOTEXT)  printf( " GEOTEXT");
      if (idx->type & FLD008_KEY)  printf( " FLD008_key");
      if (idx->type & URL_KEY)  printf( " URL_KEY");
      if (idx->type & FILENAME_KEY)  printf( " FILENAME_KEY");
      if (idx->type & EXTERNKEY)  printf( " EXTERNAL_KEY");
      if (idx->type & PAGEDINDEX)  printf( " PAGEDINDEX");
      if (idx->type & INTEGER_KEY)  printf( " DBMS_INTEGER");
      if (idx->type & DECIMAL_KEY)  printf( " DBMS_DECIMAL");
      if (idx->type & FLOAT_KEY)  printf( " DBMS_FLOAT");
      if (idx->type & DATE_KEY)  printf( " DATE_KEY");
      if (idx->type & LAT_LONG_KEY)  printf( " LAT_LONG_KEY");
      if (idx->type & BOUNDING_BOX_KEY)  printf( " BOUNDING_BOX_KEY");
      if (idx->type & DATE_RANGE_KEY)  printf( " DATE_RANGE_KEY");
      if (idx->type2 & NGRAMS) printf( " NGRAMS(%i)",idx->snowball_stem_type);
      if (idx->type2 & BITMAP_CODED_ELEMENT) printf( " CODED_ELEMENT");
      if (idx->type2 & BITMAP_CODED_ATTRIBUTE) printf( " CODED_ATTRIBUTE");
      
      if (idx->type & STEMS) printf(" NORM_STEM");
      if (idx->type & SSTEMS) printf(" NORM_PLURALSTEM");
      if (idx->type & SNOWBALL_STEMS) printf(" NORM_SNOWBALL_STEM");
      if (idx->snowball_stem_type > 0) printf(" (TYPE=%d)", idx->snowball_stem_type);
      if (idx->type & WORDNET) printf(" NORM_WORDNET");
      if (idx->type & CLASSCLUS) printf(" NORM_CLASSCLUS");
      if (idx->type & NORM_DO_NOTHING) printf(" DO_NOT_NORMALIZE");
      if (idx->type & PROXIMITY) printf("+PROXIMITY");
      if (idx->type & NORM_NOMAP) printf("+NORM_NOMAP");
      if (idx->type & NORM_WITH_FREQ) printf("+FREQ_MAPPING");
      if (idx->dateformat) printf(" %s", idx->dateformat);

      printf ("\n");

      if(idx->type & PRIMARYREPLACE) printf( "\t\t<<PRIMARY KEY Option = REPLACE>>\n");
      if(idx->type & PRIMARYIGNORE) printf( "\t\t<<PRIMARY KEY Option = IGNORE>>\n");
      if(idx->tag) printf( "\t<indextag> %s \n", idx->tag);
      if(idx->name) printf( "\t<indexname> %s \n", idx->name);
      for (cont = idx->indxcont; cont; cont = cont->next_cont) {
	printf( "   <indxcont id=%d> %s\n",cont->id_number, cont->file);
      }

      if(idx->stopwords_name) 
      	printf( "\t<stoplist> %s \n", idx->stopwords_name);

      if(idx->expansion_name) 
      	printf( "\t<expansion_terms> %s \n", idx->expansion_name);

      if (idx->exclude) {
	printf( "\tExcluded elements: \n");
      
	for (key = idx->exclude; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	  cf_print_subkeys(key->subkey);
	  printf("\n");
	}
      }
      if (idx->ranking_parameters) {
	printf( "\tIndex Ranking Parameters :\n");
	printf ("\t\tType\tID\tValue\n");
	for (rnk = idx->ranking_parameters; rnk; rnk = rnk->next_parm) {
	  printf("\t\t%d\t%d\t%f\n", rnk->type, rnk->id, rnk->val);
	}
	printf("\n");
      }
      else {
	printf( "\tIndex Ranking Parameters : DEFAULT\n");
      }
      printf( "\tIndex keys : \n");
      
      for (key = idx->keys; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      printf("\tZ39.50 Attribute Mapping:\n");

      printf ("\tUSE\tRELAT\tPOSIT\tSTRUCT\tTRUNC\tCOMPLET\n");
      printf ("\tor\tor\tor\tor\tor\tor\n");
      printf ("\tACCESS\tSEMANT\tLANG\tAUTHRTY\tEXPANS\tWEIGHT\tHITS\tCOMPAR\tFORMAT\tOCCUR\tINDIR\tFUNCTION\t(INDEX)\t(attributeset)\n");
      for (map = idx->bib1_attr_map; map; map = map->next_attr_map) {
	printf("\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d (%s)(%s)\n", map->attr_nums[1],
	       map->attr_nums[2], map->attr_nums[3],
	       map->attr_nums[4],map->attr_nums[5],
	       map->attr_nums[6],map->attr_nums[7],
	       map->attr_nums[8],map->attr_nums[9],
	       map->attr_nums[10],map->attr_nums[11],
	       map->attr_nums[12],
	       map->idx_entry->tag, map->attributeSetOID);
	if (map->altdescription)
	  printf("\t\tLOCAL DESCRIPTION: %s\n", map->altdescription); 
	
      }
    }

    if (cf->components) {
      printf( "\n\n<COMPONENTS> \n");
    }
    for (comp = cf->components; comp ; comp = comp->next_entry) {
      printf( "<COMPONENTDEF> \n");
      printf("<COMPONENTNAME> %s </COMPONENTNAME>\n", comp->name);
      if (comp->normalization & COMPONENT_STORE) {
	printf("<COMPONENTSTORE> YES </COMPONENTSTORE>\n");
      }
      else {
	printf("<COMPONENTSTORE> NO </COMPONENTSTORE>\n");
      }

      if (comp->normalization & COMPONENT_COMPRESS) {
	printf("<COMPONENTNORM> COMPRESS </COMPONENTNORM>\n");
      }
      else {
	printf("<COMPONENTNORM> NONE </COMPONENTNONE>\n");
      }

      printf( "<COMPSTARTTAG> \n");
      for (key = comp->start_tag; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      printf( "</COMPSTARTTAG> \n");
      
      printf( "<COMPENDTAG> \n");
      for (key = comp->end_tag; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      printf( "</COMPENDTAG> \n");
      
      printf( "<COMPONENTINDEXES> \n");
      
      for (idx = comp->indexes; idx ; idx = idx->next_entry) {
	
	if (idx->type) printf( "\n\t<indextype> %d", idx->type);
	if (idx->type & COMPONENT_INDEX) printf(" (COMPONENT_INDEX)");
	if (idx->type & DB_DBMS_TYPE) {
	  printf( " - DBMS"); /* external DBMS */
	  if ((idx->type & DB_MASK) == DB_MASK)
	    printf("+BTREE");
	}
	else {
	  if ((idx->type & DB_MASK) == DB_BTREE_TYPE)  printf( " - BTREE");
	  if ((idx->type & DB_MASK) == DB_HASH_TYPE)  printf( " - HASH");
	  if ((idx->type & DB_MASK) == VECTOR_TYPE)  printf( " - VECTOR");
	}
	if (idx->type & KEYWORD)  printf( " KEYWORD");
	if (idx->type & EXACTKEY)  printf( " EXACTKEY");
	if (idx->type & FLD008_KEY)  printf( " FLD008_key");
	if (idx->type & URL_KEY)  printf( " URL_KEY");
	if (idx->type & FILENAME_KEY)  printf( " FILENAME_KEY");
	if (idx->type & EXTERNKEY)  printf( " EXTERNAL_KEY");
	if (idx->type & PAGEDINDEX)  printf( " PAGEDINDEX");
	if (idx->type & INTEGER_KEY)  printf( " DBMS_INTEGER");
	if (idx->type & DECIMAL_KEY)  printf( " DBMS_DECIMAL");
	if (idx->type & FLOAT_KEY)  printf( " DBMS_FLOAT");
	if (idx->type & DATE_KEY)  printf( " DATE_KEY");
	if (idx->type & DATE_RANGE_KEY)  printf( " DATE_RANGE_KEY");
	if (idx->type & STEMS) printf(" NORM_STEM");
	if (idx->type & SSTEMS) printf(" NORM_PLURALSTEM");
	if (idx->type & SNOWBALL_STEMS) printf(" NORM_SNOWBALL_STEM");
	if (idx->snowball_stem_type > 0) printf(" (TYPE=%d)", 
						idx->snowball_stem_type);
	if (idx->type & WORDNET) printf(" NORM_WORDNET");
	if (idx->type & CLASSCLUS) printf(" NORM_CLASSCLUS");
	if (idx->type & NORM_DO_NOTHING) printf(" DO_NOT_NORMALIZE");
	if (idx->type & PROXIMITY) printf("+PROXIMITY");
	if (idx->type & NORM_NOMAP) printf("+NORM_NOMAP");
	if (idx->type & NORM_WITH_FREQ) printf("+FREQ_MAPPING");
	if (idx->dateformat) printf(" %s", idx->dateformat);
	
	printf ("\n");
	if(idx->type & PRIMARYKEY) printf( "\t\t<<PRIMARY KEY");
	if(idx->type & PRIMARYREPLACE) printf( " Option = REPLACE>>\n");
	if(idx->type & PRIMARYIGNORE) printf( " Option = IGNORE>>\n");
	if(idx->tag) printf( "\t<indextag> %s \n", idx->tag);
	if(idx->name) printf( "\t<indexname> %s \n", idx->name);
	for (cont = idx->indxcont; cont; cont = cont->next_cont) {
	  printf( "   <indxcont id=%d> %s\n",cont->id_number, cont->file);
	}
	
	if(idx->stopwords_name) 
	  printf( "\t<indexstopwordsname> %s \n", idx->stopwords_name);
	if (idx->exclude) {
	  printf( "\tExcluded elements: \n");
	  
	  for (key = idx->exclude; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }

	    cf_print_subkeys(key->subkey);
	    printf("\n");
	  }
	}
	if (idx->ranking_parameters) {
	  printf( "\tIndex Ranking Parameters :\n");
	  printf ("\t\tType\tID\tValue\n");
	  for (rnk = idx->ranking_parameters; rnk; rnk = rnk->next_parm) {
	    printf("\t\t%d\t%d\t%f\n", rnk->type, rnk->id, rnk->val);
	  }
	  printf("\n");
	}
	else {
	  printf( "\tIndex Ranking Parameters : DEFAULT\n");
	}
	printf( "\tIndex keys : \n");
	
	for (key = idx->keys; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	  cf_print_subkeys(key->subkey);
	  printf("\n");
	}
	printf("\tZ39.50 Attribute Mapping:\n");

	printf ("\tUSE\tRELAT\tPOSIT\tSTRUCT\tTRUNC\tCOMPLET\n");
	printf ("\tor\tor\tor\tor\tor\tor\n");
	printf ("\tACCESS\tSEMANT\tLANG\tAUTHRTY\tEXPANS\tWEIGHT\tHITS\tCOMPAR\tFORMAT\tOCCUR\tINDIR\tFUNCTION\t(INDEX)\t(attributeset)\n");
	for (map = cf->bib1_attr_map; map; map = map->next_attr_map) {
	  printf("\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d (%s)(%s)\n", map->attr_nums[1],
		 map->attr_nums[2], map->attr_nums[3],
		 map->attr_nums[4],map->attr_nums[5],
		 map->attr_nums[6],map->attr_nums[7],
		 map->attr_nums[8],map->attr_nums[9],
		 map->attr_nums[10],map->attr_nums[11],
		 map->attr_nums[12],
		 map->idx_entry->tag, map->attributeSetOID);
	  if (map->altdescription)
	    printf("\t\tLOCAL DESCRIPTION: %s\n", map->altdescription); 
	  
	}
      }
      printf( "</COMPONENTINDEXES> \n");
      
      printf( "</COMPONENTDEF> \n");
    }

    if (cf->components) {
      printf( "</COMPONENTS> \n\n");
    }

    if (cf->clustered_file) printf( "<clusterbase> %s \n", cf->clustered_file);

    if (cf->clusters) printf("CLUSTER DEFINITIONS:\n");

    for (clus = cf->clusters; clus ; clus = clus->next_entry) {
      
      if(clus->name) printf( "\t<clusname> %s \n", clus->name);
      printf( "\t\t<cluskey normal = ");
      if (clus->normalization == WORDNET) printf("WORDNET");
      else if (clus->normalization == STEMS) printf("STEMS");
      else if (clus->normalization == CLASSCLUS) printf("CLASSCLUS");
      else if (clus->normalization == EXACTKEY) printf("EXACTKEY");
      else printf("NONE");
      printf(">\n");
      for (key = clus->cluster_key; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t<ftag> %s </ftag> \n\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t<SQL> %s </SQL> \n\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      if (clus->cluster_stoplist_name) {
	printf( "\t\t\t<stoplist> %s </stoplist> \n", 
	       clus->cluster_stoplist_name);
      }
      printf("\t\t<clusmap>\n");
      for (cmap = clus->field_map; cmap ; cmap = cmap->next_clusmap) {
	printf("\t\t\tFROM:\n");
	for (key = cmap->from; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	  cf_print_subkeys(key->subkey);
	  printf("\n");
	}
	printf("\t\t\tTO:\n");
	for (key = cmap->to; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	  cf_print_subkeys(key->subkey);
	  printf("\n");
	}
	if (cmap->summarize) {
	  printf("\t\t\tSUMMARIZE:\n\t\t\t\t<maxnum> %d </maxnum>\n", 
		 cmap->sum_maxnum);
	  for (key = cmap->summarize; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t", key->key);
	  }
	    cf_print_subkeys(key->subkey);
	    printf("\n");
	  }
	}
      }
    }

    if (cf->display_options) {
      printf("DISPLAY OPTIONS:");
      if (cf->display_options & KEEP_AMP)
	printf(" KEEP_AMP");
      if (cf->display_options & KEEP_LT)
	printf(" KEEP_LT");
      if (cf->display_options & KEEP_GT)
	printf(" KEEP_GT");
      if (cf->display_options & KEEP_ALL)
	printf(" (i.e.: KEEP_ALL)");
      if (cf->display_options & KEEP_ENT)
	printf(" KEEP_ENTITIES");
	printf("\n");
    }
    if (cf->display) printf("DISPLAY DEFINITIONS:\n<DISPLAY>\n");

    for (format = cf->display; format ; format = format->next_format) {
      cf_print_format(format);      
    }
  }
}

void
cf_print_format(display_format *format) 
{
  idx_key *key;
  cluster_map_entry *cmap;

  if (format == NULL)
    return;

  if (format->name) 
    printf( "\t<format NAME=\"%s\"", format->name);

  if (format->oid)
    printf( " OID=\"%s\"", format->oid);

  if (format->marc_dtd_file_name)
    printf( " MARC_DTD=\"%s\"", format->marc_dtd_file_name);


  if (format->default_format) 
    printf(" DEFAULT>\n");
  else
    printf(">\n");
  
  if (format->include != NULL) {
    printf("\t\t<include>\n");
    for (key = format->include; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t<ftag> %s </ftag> \n\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t<SQL> %s </SQL> \n\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t", key->key);
	  }
      cf_print_subkeys(key->subkey);
      printf("\n");
    }
    printf("\t\t</include>\n");
  }
  
  if (format->exclude != NULL) {
    printf("\t\t<exclude");
    if (format->exclude_compress)
      printf(" COMPRESS=YES");
    else
      printf(" COMPRESS=NO");
	
    printf(">\n");
    for (key = format->exclude; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t<ftag> %s </ftag> \n\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t<SQL> %s </SQL> \n\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t", key->key);
	  }
      cf_print_subkeys(key->subkey);
      printf("\n");
    }
    printf("\t\t</exclude>\n");
  }
  
  if (format->convert_name != NULL) {
    printf("\t\t<convert FUNCTION = \"%s\"",format->convert_name);
    if (format->convert == NULL)
      printf(" ALL>\n");
    else
      printf(">\n");
    
    printf("\t\t<clusmap>\n");
    for (cmap = format->convert; cmap ; cmap = cmap->next_clusmap) {
      printf("\t\t\tFROM:\n");
      for (key = cmap->from; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      printf("\t\t\tTO:\n");
      for (key = cmap->to; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t\t", key->key);
	  }
	cf_print_subkeys(key->subkey);
	printf("\n");
      }
      if (cmap->summarize) {
	printf("\t\t\tSUMMARIZE:\n\t\t\t\t<maxnum> %d </maxnum>\n", 
	       cmap->sum_maxnum);
	for (key = cmap->summarize; key; key = key->next_key) {
	  if (key->attribute_flag < 10 )
	    printf( "\t\t\t\t<ftag> %s </ftag> \n\t\t\t\t\t", key->key);
	  else {
	    if (key->attribute_flag == 11) 
	      printf( "\t\t\t\t<SQL> %s </SQL> \n\t\t\t\t\t", key->key);
	    else
	      printf( "\t\t\t\t<XPATH> %s </XPATH> \n\t\t\t\t\t", key->key);
	  }
	  cf_print_subkeys(key->subkey);
	  printf("\n");
	}
      }
    }
    printf("\t\t</convert>\n");
  }
}

void
cf_print_subkeys (idx_key *subkey)
{
  if (subkey == NULL) return;

  if (subkey->attribute_flag == 0 || subkey->attribute_flag == 5)
    printf("<s> %s ", subkey->key);
  else if (subkey->attribute_flag == 1) /* it is an attribute key */
    printf("</s> <attr> %s ", subkey->key);
  else if (subkey->attribute_flag == 2) /* it is an attribute value key */
    printf("<value> %s </value>", subkey->key);
  else if (subkey->attribute_flag == 3) /* it is a replace pattern key */
    printf("<replace (pattern)> %s </replace (pattern)>", subkey->key);
  else if (subkey->attribute_flag == 4) /* it is a replace value key */
    printf("<replace (value)> %s </replace (value)>", subkey->key);

  /* print nested subkeys */
  cf_print_subkeys(subkey->subkey);

  if (subkey->attribute_flag == 0)
    printf(" </s> ");
  else if (subkey->attribute_flag == 1)
    printf(" </attr> ");

  if (subkey->next_key) {
    printf("\n\t\t\t");
    cf_print_subkeys(subkey->next_key);
  }
    
}

