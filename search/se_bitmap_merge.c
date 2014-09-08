/*
 *  Copyright (c) 1990-2003 [see Other Notes, below]. The Regents of the
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
*       Header Name:    se_bitmap_merge.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Merges two sets of retrieved postings
*			from a database with bitmap Boolean indexes. 
*                       All Boolean operations are supported, but
*                       only needed bitmap blocks are fetched to
*                       perform the operations
*
*       Usage:          weighted_result *se_bitmap_merge 
*				(weighted_result *, weighted_result *, 
*                                bool_operation)
*
*       Variables:	newset is pointer to merged set
*			set1 & set2 are original sets
*
*       Return Conditions and Return Codes:
*			returns pointer to merged weighted_result set
*			or NULL on error
*
*       Date:           11/7/93
*       Revised:        11/10/93
*       Version:        1.000001
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "bitmaps.h"
/* external functions */
extern weighted_result *se_rank_docs(weighted_result *newset);
extern weighted_result *se_sortwtset (weighted_result *set);
extern weighted_result *se_bitmap_to_result(weighted_result *inres, 
					    int n_wanted, int n_start);
extern weighted_result  *se_ormerge (weighted_result *inset1, 
				     weighted_result *inset2);
extern weighted_result  *se_notmerge (weighted_result *inset1, 
				     weighted_result *inset2);

extern char *LastResultSetID; /* set in the command parser */


#define MIN(X, Y) ((X) < (Y)? (X) : (Y))

/* Fetch a bitmap block for a given record number or pass back the */
/* same block if the requested block is the same                   */
bitmap_t *
se_fetch_bitmap_block(weighted_result *inset, int recordno, 
		      int *block, bitmap_t *last_block)
{

  idx_list_entry *idx;
  DBT block_keyval;
  DBT block_dataval;
  int returncode;
  bitmap_t *accum_block;
  bitmap_t *result_block;
  bitblock_key *block_key;
  int docid;
  int current_blockno;
  int block_ok;
  int i, j;
  

  memset(&block_keyval, 0, sizeof(block_keyval));
  memset(&block_dataval, 0, sizeof(block_dataval));

  current_blockno = 0;
  current_blockno = bitblock_id(recordno);

  /* same block needed ? */
  if (*block == current_blockno && last_block != NULL) {
    return (last_block);
  }

  /* otherwise we fetch the block and put it in new storage */
  idx = inset->index_ptr[0];

  if (idx->db == NULL) {
    /* need to open or reopen database */
    cf_index_open(inset->filename, idx->tag, INDEXFL);
  }
  block_key = CALLOC(bitblock_key, 1);
  block_keyval.data = block_key;
  block_keyval.size = sizeof(bitblock_key);

  block_key->blockid = (short)current_blockno;
  block_key->termid = inset->termid[0];

  accum_block = (unsigned char *)CALLOC(char, bitmap_blocksize_bytes);  

  returncode = idx->db->get(idx->db, NULL, &block_keyval, &block_dataval, 0);
  if (returncode == 0) { /* found the block */
    if (block_dataval.size != bitmap_blocksize_bytes) {
      fprintf(LOGFILE, "Error: bitmap blocksize wrong on retrieval\n");
      return(NULL);
    }
    else {
      memcpy(accum_block,block_dataval.data, block_dataval.size);      
    }
  }
  else if (returncode == DB_NOTFOUND) { 
    /*  fprintf(LOGFILE, "Error: bitmap block %d/%d not found in retrieval\n",
     *    block_key->blockid, block_key->termid);
     */
    /* if not found return an empty block */
    *block = 0;
    return(accum_block); 
    
  }
  else {
    fprintf(LOGFILE, "Error: non-zero return code %d in bitmap block %d/%d retrieval - empty block returned\n",
	    returncode, block_key->blockid, block_key->termid );
    *block = 0;
    return(accum_block);
    
  }

  /* do boolean over multiple terms for the same block */
  if (inset->num_hits < 0) {
    int block_ok = 1;
      
    for(i = 1; inset->termid[i] > 0 && block_ok; i++) {
    
      /* get the next term's bit block */
      block_key->termid = inset->termid[i];
      /* these should all be the same index... but... */
      idx = inset->index_ptr[i];
      
      returncode = idx->db->get(idx->db, NULL, 
				&block_keyval, &block_dataval, 0);
      if (returncode == DB_NOTFOUND) {
	memset(accum_block, 0, bitmap_blocksize_bytes);
	block_ok = 0;
	continue;
      }
      if (returncode == 0) { /* found the block */
	if (block_dataval.size != bitmap_blocksize_bytes) {
	  fprintf(LOGFILE, "Error: bitmap blocksize wrong in ANDing retrieval\n");
	  return(NULL);
	}
	else {
	  /* AND the current accum_block and the results together */
	  result_block = (bitmap_t *)block_dataval.data;      
	  for (j=0; j < bitmap_blocksize_bytes; j++)
	    accum_block[j] &= result_block[j];      
	}
      }
      else {
	fprintf(LOGFILE, "Error: non-zero return code %d in bitmap block %d/%d ANDing retrieval\n",
		returncode, block_key->blockid, block_key->termid );
	*block = 0;
	return(accum_block);
	
      }      
    }
  }

  FREE(block_key);

  *block = current_blockno;
  
  return(accum_block);
}

/* AND or NOT of two bitmap results */

weighted_result  *
se_bitmaps_and (weighted_result *inset1, weighted_result *inset2, int oper)
{
  int returncode;
  bitmap_t *result_block1 = NULL, *result_block2 = NULL;
  bitmap_t byte;
  int i=0, j=0, bit;
  int max_docid1, max_docid2;
  int max_blockno1, max_blockno2;
  int max_blockno;
  int doc_count = 0;
  int all_finished = 0;
  int max_result = 0;
  weighted_result *wt_res; /* final result */
  int skip = 0;
  int dummy_recno = 1;
  int block;
  int current_blockno = 0;

  if (inset1 == NULL || inset2 == NULL)
    return (NULL);
  

  max_docid1 = inset1->entries[0].record_num;
  max_docid2 = inset2->entries[0].record_num;

  max_blockno = MIN(bitblock_id(max_docid1) + 1, bitblock_id(max_docid2) + 1);

  if (inset1->num_hits > 0) 
    max_result = inset1->num_hits;
  else
    max_result = max_docid1;

  if (inset2->num_hits > 0 && inset2->num_hits < max_result)
    max_result = inset2->num_hits;
    
  


  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + ((max_result+1) * sizeof(weighted_entry))));

  strcpy(wt_res->filename, inset1->filename);
  wt_res->num_hits = 0;
  wt_res->setid[0] = '\0';
  /* the results with be a real resultset and not bitmaps */
  wt_res->result_type &= ~BITMAP_RESULT;
  

  for (block = 0; block < max_blockno; block++) {

    dummy_recno = block * bitmap_blocksize_bits + 1;

    result_block1 = se_fetch_bitmap_block(inset1, dummy_recno, 
					 &max_blockno1, result_block1);

    result_block2 = se_fetch_bitmap_block(inset2, dummy_recno, 
					 &max_blockno2, result_block2);
    
    /* accum block should now contain any possible results */
    
    for (j=0; j < bitmap_blocksize_bytes; j++) {
      if (oper == BOOLEAN_AND)
	byte = result_block1[j] & result_block2[j];
      else
	byte = result_block1[j] & ~result_block2[j];
      if (byte) {
	for (bit = 0; bit < 8; bit++) {
	  if(byte & (1 << bit)) {
	    wt_res->entries[wt_res->num_hits].record_num =
	      (block * bitmap_blocksize_bits) +
	      (j * 8) + bit + 1;
	    wt_res->entries[wt_res->num_hits].weight = 1.0;
	    wt_res->num_hits++;
	  }
	}
      }
    }
  }

  if (result_block1 != NULL) 
    FREE(result_block1);
  if (result_block2 != NULL) 
    FREE(result_block2);
  

  return(wt_res);

}

/* The main MERGE part */
weighted_result  *
se_bitmap_merge (weighted_result *inset1, weighted_result *inset2, int oper)
{
  bitmap_t *result_block = NULL, *last_result = NULL;
  int i = 0, i1 = 0, i2 = 0; /* loop and array indices */
  int prob_flag = 0;
  int both_bitmap = 0;
  int bitmap_flag = 0;
  int newset_size;
  weighted_result *set1, *set2, 
    *work_set, *newset, *bitmap_set, *normal_set; /* pointer to merged set */
  char tempresultname[MAX_RSN_LENGTH];
  int num_sub_sets;
  int res_count;
  int current_blockno=0;


  if (inset1 == NULL || inset2 == NULL) 
    return(NULL);
  
  /* check to see if either of the sets is from a bitmap search    */
  if ((inset1->result_type & BITMAP_RESULT) == BITMAP_RESULT
      && (inset2->result_type & BITMAP_RESULT) == BITMAP_RESULT) {
    both_bitmap = 1;
    set1 = inset1;
    set2 = inset2;
  }
  else if ((inset1->result_type & BITMAP_RESULT) == BITMAP_RESULT) {
    bitmap_flag = 1;
    bitmap_set = inset1;
    normal_set = inset2;
    /* bad designer! no way to really use the bitmaps */
    if (oper == BOOLEAN_NOT) {
      return(
	     se_notmerge(
			 se_bitmap_to_result(inset1, inset1->entries[0].record_num, 1),
			 normal_set));
    }
  }
  else if ((inset2->result_type & BITMAP_RESULT) == BITMAP_RESULT) {
    bitmap_flag = 2;
    bitmap_set = inset2;
    normal_set = inset1;
  }
  
  /* check to see if the normal set is a probabilistic search */
  if (both_bitmap != 1 
      && (normal_set->result_type 
      & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT | CORI_RESULT | FUZZY_RESULT | BLOB_RESULT))) {
    prob_flag = (normal_set->result_type 
		 & (PROBABILISTIC_RESULT | OKAPI_RESULT | TFIDF_RESULT 
		    | CORI_RESULT |FUZZY_RESULT | BLOB_RESULT));
  }

  if (both_bitmap) {
    
    switch (oper) {
    case BOOLEAN_AND:
      return(se_bitmaps_and(set1, set2, BOOLEAN_AND));
      break;
    case BOOLEAN_OR:
      return(
	     se_ormerge(
			se_bitmap_to_result(set1, set1->entries[0].record_num, 1), 
			se_bitmap_to_result(set2, set2->entries[0].record_num, 1)));
      break;
    case BOOLEAN_NOT:
      return(se_bitmaps_and(set1, set2, BOOLEAN_NOT));
      break;
    }
    
  }
  else { /* not both bitmap */
    if (oper == BOOLEAN_OR) {
      int max_docid;
      /* have to materialize the WHOLE bitmap */
      max_docid = bitmap_set->entries[0].record_num;
      return(se_ormerge(normal_set, se_bitmap_to_result(bitmap_set, 
							max_docid, 1)));
    }
    else { /* all other operations */
      
      
      newset = (weighted_result *) 
	CALLOC (char, (sizeof(weighted_result) + 
		       ((normal_set->num_hits+1) * sizeof(weighted_entry))));
      
      strcpy(newset->filename, normal_set->filename);
      res_count = 0;
      /* if these are ranked, then sort them by record number */
      if (prob_flag)
	work_set = se_sortwtset(normal_set);
      else
	work_set = normal_set;
      
      for (i=0; i < normal_set->num_hits; i++) {
	/* to plug a weird memory leak we don't reuse blocks -- very wasteful */
	result_block = 
	  se_fetch_bitmap_block(bitmap_set, normal_set->entries[i].record_num,
				&current_blockno, NULL);
	
	switch (oper) {
	case BOOLEAN_AND:
	  if (bitblock_test(result_block, normal_set->entries[i].record_num)) {
	    newset->entries[res_count].record_num = normal_set->entries[i].record_num;
	    newset->entries[res_count].xtra = normal_set->entries[i].xtra;
	    newset->entries[res_count].weight = normal_set->entries[i].weight;
	    res_count++;
	  }
	  break;
	case BOOLEAN_NOT:

	  if (bitblock_test(result_block, normal_set->entries[i].record_num)) {
	    continue;
	  }
	  else {
	    newset->entries[res_count].record_num = normal_set->entries[i].record_num;
	    newset->entries[res_count].xtra = normal_set->entries[i].xtra;
	    newset->entries[res_count].weight = normal_set->entries[i].weight;
	    res_count++;
	  }
	  break;
	}
	
	if (result_block) {
	  FREE(result_block);
	}
      }
      
      newset->num_hits = res_count;
    }
  }
  
  if (last_result == result_block) {
    FREE(result_block);
  }
  
  
  if (prob_flag) {
    newset->result_type |= prob_flag;
    return (se_rank_docs(newset));
  }
  else
    return(newset);
}

