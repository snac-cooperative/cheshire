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
*	Header Name:	in_sync.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:        flush index buffers to disk and (hopefully) free
*                       memory for reuse.
*
*	Usage:		in_sync_index(configfile_info)
*
*	Variables:	
*
*	Return Conditions and Return Codes:
*
*	Date:		03/16/95
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1995.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "stem.h"
#include "bitmaps.h"

int
in_add_bitmap_entry(idx_list_entry *idx, int recnum, char *word_ptr,
		    bitmap_t *in_bitmap, int n_new_entries)
{
  idx_result *newinfo = NULL, tempinfo;
  DBT main_keyval;
  DBT main_dataval;
  DBT block_keyval;
  DBT block_dataval;
  int returncode;
  bitmap_t *bitmap_block = NULL;
  bitblock_key *block_key;

  /* Initialize the key/data pairs so the flags aren't set. */
  memset(&main_keyval, 0, sizeof(main_keyval));
  memset(&main_dataval, 0, sizeof(main_dataval));
    
  memset(&block_keyval, 0, sizeof(block_keyval));
  memset(&block_dataval, 0, sizeof(block_dataval));
    

  /* Build keys to locate the term entry and block...*/
  main_keyval.data = (void *) word_ptr;
  main_keyval.size = strlen(word_ptr) + 1;

  block_key = CALLOC(bitblock_key, 1);
  block_key->blockid = (short)bitblock_id(recnum);
  block_keyval.data = block_key;
  block_keyval.size = sizeof(bitblock_key);

  returncode = idx->db->get(idx->db, NULL, &main_keyval, &main_dataval, 0);

  if (returncode == 0) { /* found the term in the index already */
    /* get the global data for the entry */
    newinfo = (idx_result *) 
      CALLOC(int, (main_dataval.size / sizeof(int)) + 1);
    memcpy((char *)newinfo, (char *)main_dataval.data, main_dataval.size);
    
#ifdef DEBUGIND
    printf("outputting hash table word: %s - termid: %d\n", 
	   word_ptr,newinfo->termid);
#endif
    /* now increment the counters */
    newinfo->tot_freq += n_new_entries;
    newinfo->num_entries += n_new_entries;
    block_key->termid = newinfo->termid;
    newinfo->entries[0].record_no = recnum;
    newinfo->entries[0].term_freq++;
    idx->GlobalData->tot_occur += n_new_entries;
    idx->GlobalData->tot_numrecs += n_new_entries;

    main_dataval.data = newinfo;
    /* should be the same size as before */
  }
  else if (returncode == DB_NOTFOUND) { 
    /* a new word that has no freq data */
    tempinfo.termid = idx->GlobalData->recptr++;
    block_key->termid = tempinfo.termid;
    tempinfo.tot_freq = n_new_entries;
    tempinfo.entries[0].record_no = recnum;
    tempinfo.entries[0].term_freq = 1;
    tempinfo.num_entries = n_new_entries;
    idx->GlobalData->tot_occur += n_new_entries;
    idx->GlobalData->tot_numrecs += n_new_entries;
    main_dataval.size = sizeof(tempinfo);
    main_dataval.data = (void *)&tempinfo;
    
#ifdef DEBUGIND
    printf("output new hash table word: %s - termid: %d\n", 
	   word_ptr,tempinfo.termid);
#endif
  }	  

  /* and write it to the index */
  returncode = idx->db->put(idx->db, NULL, &main_keyval, &main_dataval, 0); 
  if (returncode != 0) { 
#ifdef DEBUGIND
    printf("\nNon-zero return in db put of found term: %d\n", 
	   returncode);
#endif
    if (returncode == DB_RUNRECOVERY) {
      fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
      exit(999);
    } 
    if (returncode == DB_LOCK_NOTGRANTED) {
      fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
      exit(999);
    } 
    if (returncode == DB_LOCK_DEADLOCK) {
      fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
      exit(999);
    } 
      
  }

  if (newinfo)
    FREE(newinfo);

  /* now output the actual bit block */
  returncode = idx->db->get(idx->db, NULL, &block_keyval, &block_dataval, 0);
  if (returncode == 0) { /* found the block */
    if (block_dataval.size != bitmap_blocksize_bytes) {
      fprintf(LOGFILE, "Error: bitmap blocksize wrong on retrieval\n");
      exit(999);
    }
    if (n_new_entries == 1 && in_bitmap == NULL) {
      /* no need to copy, because the data is byte aligned */
      bitblock_set((char *)(block_dataval.data), recnum);
    }
    else {
      int i;
      bitmap_t *b;
      
      b = (bitmap_t *)block_dataval.data;
      
      for (i=0; i < bitmap_blocksize_bytes; i++)
	b[i] |= in_bitmap[i];
      
    }
    
  }
  else {
    if (n_new_entries == 1 && in_bitmap == NULL) {
      /* Set a bit in a new block */
      bitmap_block = bitmap_block_alloc();
      bitblock_set(bitmap_block, recnum);
      block_dataval.data = bitmap_block;
      block_dataval.size = bitmap_blocksize_bytes;
    }
    else {
      block_dataval.data = in_bitmap;
      block_dataval.size = bitmap_blocksize_bytes;	
    }
    }
  /* now have the block & bits set correctly -- so put it back in storage */
  returncode = idx->db->put(idx->db, NULL, &block_keyval, &block_dataval, 0); 
  if (returncode != 0) { 
#ifdef DEBUGIND
    printf("\nNon-zero return in db put of found term: %d\n", 
	   returncode);
#endif
    if (returncode == DB_RUNRECOVERY) {
      fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
      exit(999);
    } 
    if (returncode == DB_LOCK_NOTGRANTED) {
      fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
      exit(999);
    } 
    if (returncode == DB_LOCK_DEADLOCK) {
      fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
      exit(999);
    } 
    
  }
  
  /* all done with the new bitmap block (for now) */
  if (bitmap_block)  
    FREE(bitmap_block);

  return(0);

}


int 
in_del_bitmap_entry(idx_list_entry *idx, int recnum, int termid)
{
  DBT block_keyval;
  DBT block_dataval;
  int returncode;
  bitmap_t *bitmap_block = NULL;
  bitblock_key *block_key;

  /* Initialize the key/data pairs so the flags aren't set. */
  memset(&block_keyval, 0, sizeof(block_keyval));
  memset(&block_dataval, 0, sizeof(block_dataval));
    

  /* Build keys to locate the term entry and block...*/
  block_key = CALLOC(bitblock_key, 1);
  block_key->blockid = (short)bitblock_id(recnum);
  block_key->termid = termid;
  block_keyval.data = block_key;
  block_keyval.size = sizeof(bitblock_key);
  
  returncode = idx->db->get(idx->db, NULL, &block_keyval, &block_dataval, 0);

  if (returncode == 0) { /* found the block */
    if (block_dataval.size != bitmap_blocksize_bytes) {
      fprintf(LOGFILE, "Error: bitmap blocksize wrong on retrieval\n");
      exit(999);
    }
    /* no need to copy, because the data is byte aligned */
    bitblock_clear((char *)(block_dataval.data), recnum);

    /* now have the block & bits set correctly -- so put it back in storage */
    returncode = idx->db->put(idx->db, NULL, &block_keyval, &block_dataval, 0); 
    if (returncode != 0) { 
#ifdef DEBUGIND
      printf("\nNon-zero return in db put of found term: %d\n", 
	     returncode);
#endif
      if (returncode == DB_RUNRECOVERY) {
	fprintf(LOGFILE, "DB_RUNRECOVERY ERROR returned from PUT\n");
	exit(999);
      } 
      if (returncode == DB_LOCK_NOTGRANTED) {
	fprintf(LOGFILE, "DB_LOCK_NOTGRANTED ERROR returned from PUT\n");
	exit(999);
      } 
      if (returncode == DB_LOCK_DEADLOCK) {
	fprintf(LOGFILE, "DB_LOCK_DEADLOCK ERROR returned from PUT\n");
	exit(999);
      } 
      
    }
  }
  /* all done with the new bitmap block (for now) */
  if (bitmap_block)  
    FREE(bitmap_block);
  
  return(0);

}




