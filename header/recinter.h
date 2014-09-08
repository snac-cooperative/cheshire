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
#include "alpha.h"
int  rec_create(char *file);
BPT *rec_open(char *file);
void rec_close(BPT *bp);
 
int rec_insert(BPT *bp, int tree, char *key, DATUM *data);
int rec_delete(BPT *bp, int tree, char *key);
int rec_change(BPT *bp, int tree, char *key, DATUM *data);
  
int rec_search(BPT *bp, int tree, char *key);

int rec_first(BPT *bp, int tree, KD *kd);
int rec_last(BPT *bp, int tree, KD *kd);
int rec_next(BPT *bp, int tree, KD *kd);
int rec_prev(BPT *bp, int tree, KD *kd);

DATUM *rec_get_data(BPT *bp, int tree, char *accno);

/*
*      B+ tree internal routines. The following B+ tree
*      internal routines should NOT be called directly
*      in application programs.
*/

int rec_slot(IPAGEINFO *page, char *key, int *slot);
int rec_page_init(IPAGEINFO *page);

/*
PAGENO rec_alloc_page(BPT *bp);
void   rec_free_page(BPT *bp, PAGENO pageno);
IPAGEINFO *rec_new_page(BPT *bp, PAGENO *pageno);
IPAGEINFO *rec_get_page(BPT *bp, PAGENO pageno);
*/

int rec_read_header(int fd, FILEHDR *hdr);
int rec_write_header(int fd, FILEHDR *hdr);
int rec_write_page(int fd, int pageno, void *page);
int rec_read_page(int fd, int pageno, void *page);
int rec_flush(BPT *bp);

void rec_init_header(FILEHDR *hdr);
void rec_bpt_stat();

int rec_page_num_keys(void *page);
int rec_page_key_offset(IPAGEINFO *page, int slot);
int rec_page_key_leh(IPAGEINFO *page, int slot);
char *rec_page_key(void *page, int slot);
int rec_page_child(IPAGEINFO *page, int slot);
int rec_page_update_child(IPAGEINFO *page, int slot, int data);
int rec_page_free_space(IPAGEINFO *page);
int rec_space_needed(char *key);
int rec_space_needed(char *key);
void rec_page_dump(IPAGEINFO *page);
int rec_slot(IPAGEINFO *page, char *key, int *slot);
int rec_page_init(IPAGEINFO *page);
int rec_key_size(char *key);
int rec_page_move_keys(IPAGEINFO *curr_page, IPAGEINFO *sibling_page, int slot);
int rec_page_insert_key(IPAGEINFO *page, int slot, char *key, int child);
int rec_page_append_key(IPAGEINFO *page, char *key, int child);
int rec_page_delete_key(IPAGEINFO *page, int slot);
int rec_page_delete_key_child(IPAGEINFO *page, int kslot, int cslot);
int rec_insert_key(BPT *, int, char *, int, PAGENO, int); 
int rec_insert_record(BPT *bp, int tree, char *key,DATUM *child,int pageno,int slot);
int rec_delete_key(BPT *bp, int tree, PAGENO child_pageno, int child_slot);
int rec_delete_page(BPT *bp, int tree, PAGENO curr_pageno);

int rec_push(BPT *bp, PAGENO pageno, int slot);
int rec_pop(BPT *bp, PAGENO *pageno, int *slot);
int rec_scan_page(void *page, char *key, CURSOR *cs);

void rec_init_kd(KD *kd, char *key, int data, int flag);

void print_kd(KD *kd);

void rec_print_header(FILEHDR *hdr);
void rec_page_print(BPT *bp, PAGENO pageno, int level);

int rec_update_root(BPT *, int, PAGENO);
PAGENO rec_get_root(BPT *, int);
  
int rec_slot(IPAGEINFO *page, char *key, int *slot);

int rec_slot(IPAGEINFO *page, char *key, int *slot);
int leaf_page_init(LEAFPAGE *page);
int leaf_page_insert_key(LEAFPAGE *page, int slot, char *key, DATUM *data);
int key_data_size (char *key, DATUM *data);
int data_size (DATUM *data);
int leaf_page_append_key (LEAFPAGE *page, char *key, DATUM *data);
int page_delete_record (LEAFPAGE *page, int slot);
int leaf_page_move_keys (LEAFPAGE *curr_page, LEAFPAGE *sibling_page, int slot);
void leaf_page_dump (LEAFPAGE *page);
int leaf_space_needed (char *key, DATUM *data);
int rec_space_needed (char *key);
int leaf_page_free_space (LEAFPAGE *page);
int leaf_page_key_len (LEAFPAGE *page, int slot);
int leaf_page_key_offset (LEAFPAGE *page, int slot);

int rec_delete_record (BPT *, int, PAGENO, int);
int page_data_size (LEAFPAGE *page, int slot);
char *page_data_addr (LEAFPAGE *page, int slot);

