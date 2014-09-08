/*
 *  Copyright (c) 1990-2001 [see Other Notes, below]. The Regents of the
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
*       Header Name:    se_comp_minmax.c
*
*       Programmer:     Ray R. Larson
*              
*
*       Purpose:        Convert a list of components to a possibly smaller 
*                       list containing only the MIN or MAX level for
*                       the components -- I.e., if component definitions
*                       permit nesting of components...

                         <c01>
			     <c02>
                                 <c03>
                                   blotz
                                 </c03>
                                 <c03>
                                   ...
				 </c03>
                             </c02>
                             <c02>
                               ...
                             </c02>
                         </c01>

                     Then MIN for a resultset matching "blotz" in a component
                     set defined as <ftag> c[0-9][0-9] </ftag> 
                     would return only the <c03> component...
		     MAX would return only the <c01> component...
*
*       Usage:          se_comp_max(resultset) or se_comp_min(resultset)
*
*       Variables:	
*
*       Return Conditions and Return Codes:
*			returns pointer to new weighted_result set
*			or NULL on error
*
*       Date:           7/5/2001
*       Version:        1.0
*       Copyright (c) 2001.  The Regents of the University of California
*               All Rights Reserved
*
************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <cheshire.h>

/* external functions */
weighted_result *se_rank_docs(weighted_result *newset);
weighted_result *se_sortwtset(weighted_result *newset);
component_data_item *get_component_record(int component_id, 
					  component_list_entry *comp);

weighted_result *se_comp_minmax(int minmax, weighted_result *inset)
{

  int i = 0; /* loop and array indices */
  int j = 0; /* loop and array indices */
  int doc_count = 0;
  int exists;
  int compnum, recnum;
  int num_comp;
  float sum_wts;
  int total_postings, newset_size;
  weighted_result *newset; /* result set*/
  component_data_item *comp_data;
  component_data_item **templist;

  if (inset == NULL)
    return(NULL);
  
  if (inset->result_type & COMPONENT_RESULT != COMPONENT_RESULT
      && inset->result_type & HYBRID_RESULT != HYBRID_RESULT)
    return(NULL);

  total_postings = inset->num_hits;

  newset_size = 
    sizeof(weighted_result) + (sizeof(weighted_entry) * total_postings);
  newset = (weighted_result *) CALLOC(char, newset_size);
  templist = CALLOC(component_data_item *, total_postings);

  *newset = *inset; /* copy over the basic information */

  /* get all of the component start and end data */
  for (i = 0; i < total_postings; i++) {
    templist[i] = get_component_record(inset->entries[i].record_num,
				   inset->component);
  }

  if (minmax == 1) {
    /* look for minimum inclusions */
    for (i = 0; i < total_postings; i++) {

      if (templist[i] == NULL)
	continue;

      for (j = 0; j < total_postings; j++) {
	/* skip cases of no possible match */
	if (j == i)
	  continue;
	if (templist[j] == NULL)
	  continue;

	if (templist[i]->end_offset <= templist[j]->start_offset
	    || templist[i]->start_offset >= templist[j]->end_offset) 
	  continue; /* can't possibly be contained */
	
	/* if the item is contained, then kill the container */
	if (templist[i]->start_offset >= templist[j]->start_offset
	    && templist[i]->end_offset <= templist[j]->end_offset) {
	  FREE(templist[j]);
	  templist[j] = NULL;
	}
      }
    }
  }
  else {
    /* look for maximum */
    for (i = 0; i < total_postings; i++) {

      if (templist[i] == NULL)
	continue;

      for (j = 0; j < total_postings; j++) {
	if (j == i)
	  continue;
	if (templist[j] == NULL)
	  continue;
	if (templist[i]->end_offset <= templist[j]->start_offset
	    || templist[i]->start_offset >= templist[j]->end_offset) 
	  continue; /* can't possibly be contained */
	
	/* if the item is contained, then kill it */
	if (templist[i]->start_offset <= templist[j]->start_offset
	    && templist[i]->end_offset >= templist[j]->end_offset) {
	  FREE(templist[j]);
	  templist[j] = NULL;
	}
      }
    }
  }

  /* transfer the data to the new result set... */
  newset->num_hits = 0;
  for (i = 0; i < total_postings; i++) {
    if (templist[i] != NULL) {
      newset->entries[newset->num_hits].weight = inset->entries[i].weight;
      newset->entries[newset->num_hits++].record_num = 
	inset->entries[i].record_num;
      FREE(templist[i]);
      templist[i] = NULL;
    }
  }
  return(newset);
}


