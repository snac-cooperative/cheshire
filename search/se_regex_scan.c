/*
 *  Copyright (c) 1990-2004 [see Other Notes, below]. The Regents of the
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
*	Header Name:	se_regex_scan.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	feedback word searching of specified records
*
*	Usage:		se_regex_scan(weighted_result in, char *words[])
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		8/18/2004
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2004.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"


weighted_result *se_regex_scan(weighted_result *in_set, char *regex[], 
			      int nregex)
{

  char *word_ptr, *norm_word_ptr;
  int matchrecs=0;
  int matchflag = 0;
  weighted_result *wt_res;
  int w, i, docid;
  char *pattern;
  char *data;
  
  wt_res = (weighted_result *) 
    CALLOC (char, (sizeof(weighted_result) + 
		   (in_set->num_hits * sizeof(weighted_entry))));

  memcpy(wt_res, in_set, sizeof(weighted_result));
	 
  wt_res->num_hits = 0;
  
  for (i = 0 ; i < in_set->num_hits; i++) {
    docid = in_set->entries[i].record_num ;
    data = GetRawSGML(in_set->filename, docid);
    matchflag = 0;
    for (w = 0 ; w < nregex; w++) {
      /* match the word(s) to the doc */
      pattern = CALLOC(char, strlen(regex[w])+50);

      /* if the regex is complex take it directly...*/
      if (strpbrk(regex[w], "[]^$?*\\.(){}|") != NULL)
	sprintf(pattern, "%s", regex[w]);
      else /* otherwise treat like a word and search for it surrounded */
	sprintf(pattern, "(^|[^a-zA-Z])(%s)([^a-zA-Z]|$)", regex[w]);
      
      if (match(pattern, data, 0)) {
	wt_res->entries[matchrecs].record_num = docid;
	if (matchflag == 0) {
	  wt_res->entries[matchrecs].weight = 1.0;
	  matchflag = 1;
	}
	else
	  wt_res->entries[matchrecs].weight 
	    = wt_res->entries[matchrecs].weight + 1.0; 
      }
      FREE(pattern);
      pattern = NULL;
      
    }  
    if (matchflag) 
      matchrecs++;
    FREE(data);
    
  }

  wt_res->num_hits = matchrecs;

  FREE(in_set);

  return(wt_res);
}












