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
*       Header Name:    se_mapweights.c
*
*       Programmer:     Kuntz & O'Leary 
*
*       Purpose:        calculate term weights and map
*			postings and weights to new structure
*                       
*
*       Usage:          weighted_result *se_mapweights (post_result)
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

weighted_result *se_mapweights(idx_result *set) {


/* calculate term weights and map post_result onto weighted result */

	weighted_result *weighted_set;	/* result after weighting */
	int i;		/* loop counter */
	int resultsize;	/* amount of memory to allocate */

	resultsize = (sizeof(weighted_result) + 
		(set->num_entries * sizeof(weighted_entry)));
	weighted_set = (weighted_result *) malloc(resultsize); 

	strcpy (weighted_set->setid,"Set ID");   /* XXXX Change This */
	weighted_set->num_hits = set->num_entries;

	for (i=0; i < set->num_entries; i++) {
	  weighted_set->entries[i].record_num = set->entries[i].record_no;
	  weighted_set->entries[i].weight = 
		(float) set->entries[i].term_freq / (float) set->tot_freq;
	}

	return(weighted_set);
}

