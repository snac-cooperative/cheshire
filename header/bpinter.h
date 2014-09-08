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
/*
 * Internal B+ Tree routines.
 * Aitao Chen
 */
#include "alpha.h"
int  bp_create(char *file);
BPT *bp_open(char *file);
void bp_close(BPT *bp);
 
int bp_insert(BPT *bp, int tree, char *key, int data);
int bp_delete(BPT *bp, int tree, char *key);
int bp_change(BPT *bp, int tree, char *key, int data);
  
int bp_search(BPT *bp, int tree, char *key, KD *kd);

int bp_search_extra(BPT *bp, int tree, char *key, KD *kd, REL relation);
 
int bp_first(BPT *bp, int tree, KD *kd);
int bp_last(BPT *bp, int tree, KD *kd);
int bp_next(BPT *bp, int tree, KD *kd);
int bp_prev(BPT *bp, int tree, KD *kd);


/*
*      B+ tree internal routines. The following B+ tree
*      internal routines should NOT be called directly
*      in application programs.
*/

int bp_slot(PAGEINFO *page, char *key, int *slot);
int page_init(PAGEINFO *page);

/*
PAGENO bp_alloc_page(BPT *bp);
void   bp_free_page(BPT *bp, PAGENO pageno);
PAGEINFO *bp_new_page(BPT *bp, PAGENO *pageno);
PAGEINFO *bp_get_page(BPT *bp, PAGENO pageno);
*/

int bp_read_header(int fd, FILEHDR *hdr);
int bp_write_header(int fd, FILEHDR *hdr);
int bp_write_page(int fd, int pageno, PAGEINFO *page);
int bp_read_page(int fd, int pageno, PAGEINFO *page);
int bp_flush(BPT *bp);

void bp_init_header(FILEHDR *hdr);
void bp_bpt_stat();

int page_num_keys(PAGEINFO *page);
int page_key_offset(PAGEINFO *page, int slot);
int page_key_leh(PAGEINFO *page, int slot);
char *page_key(PAGEINFO *page, int slot);
int page_child(PAGEINFO *page, int slot);
int page_update_child(PAGEINFO *page, int slot, int data);
int page_free_space(PAGEINFO *page);
int bp_space_needed(char *key);

int bp_space_needed(char *key);
void page_dump(PAGEINFO *page);
int bp_slot(PAGEINFO *page, char *key, int *slot);
int page_init(PAGEINFO *page);
int key_size(char *key);
int page_move_keys(PAGEINFO *curr_page, PAGEINFO *sibling_page, int slot);
int page_insert_key(PAGEINFO *page, int slot, char *key, int child);
int page_append_key(PAGEINFO *page, char *key, int child);
int page_delete_key(PAGEINFO *page, int slot);
int page_delete_key_child(PAGEINFO *page, int kslot, int cslot);
 
int bp_insert_key(BPT *bp, int tree, char *key,int child,int pageno,int slot);
int bp_delete_key(BPT *bp, int tree, PAGENO child_pageno, int child_slot);
int bp_delete_page(BPT *bp, int tree, PAGENO curr_pageno);

int bp_push(BPT *bp, PAGENO pageno, int slot);
int bp_pop(BPT *bp, PAGENO *pageno, int *slot);
int bp_scan_page(PAGEINFO *page, char *key, KD *dk, CURSOR *cs);

void bp_init_kd(KD *kd, char *key, int data, int flag);

void print_kd(KD *kd);

void bp_print_header(FILEHDR *hdr);
void page_print(BPT *bp, PAGENO pageno, int level);

int bp_update_root(BPT *, int, PAGENO);
PAGENO bp_get_root(BPT *, int);
  
int page_movekeys(PAGEINFO *curr_page, PAGEINFO *sibling_page, int slot);
int bp_slot(PAGEINFO *page, char *key, int *slot);


