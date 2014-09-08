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
*       Header Name:    se_sortset.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        sort posting results in record number order
*                       
*
*       Usage:          
*                       weighted_result *se_sortwtset(weighted_result);
*
*       Variables:
*
*       Return Conditions and Return Codes:
*			Returns pointer to weighted_result structure.
*
*       Date:           11/7/93
*       Revised:        11/7/93
*       Version:        1.000001
*       Copyright (c) 1993.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/

#include <stdio.h>
#include <cheshire.h>

/* sort an array of entries by record number     *
 * for later comparison to other retrieved sets  */
int 
compare_wt_entries (weighted_entry *e1, weighted_entry *e2) {

  if (e1->record_num < e2->record_num)
    return (-1);
  else if (e1->record_num == e2->record_num) {
    /* if these are virtual results, we interleave from the different sources */
    if (e1->xtra < e2->xtra)
      return(-1);
    else if (e1->xtra > e2->xtra)
      return(1);
    else
      return (0);
  }
  else 
    return (1); 
}

weighted_result *
se_sortwtset (weighted_result *set) {

  qsort ((void *) set->entries, set->num_hits,
	 sizeof(weighted_entry), compare_wt_entries);

  return (set);
}




