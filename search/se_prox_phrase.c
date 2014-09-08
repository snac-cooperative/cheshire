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
*       Header Name:    se_prox_phrase.c
*
*       Programmer:     Ray Larson
*
*       Purpose:        Test for phrases using proximity data
*                   
*                       
*
*       Usage:          int se_prox_phrase(int num_prox_terms, 
*                                       int *proxtermidlist, int record_num, 
*                                       int phrase_prox_length);
*
*
*       Variables:	newset is pointer to merged set
*			set1 & set2 are original sets
*
*       Return Conditions and Return Codes:
*			returns match position offset on success
*			or 0 for match failure
*
*       Date:           9/11/1999
*     Revised:       
*       Version:        1.000001
*
************************************************************************/

/* the following includes the Z39.50 defs */
#include "z3950_3.h"
#include "z_parameters.h"
#include "z3950_intern.h"

#include <cheshire.h>


int 
se_prox_phrase(DB *dbprox, int num_prox_terms, int proxtermidlist[], 
	       int proxtermlength[], int record_num, int phrase_prox_length)
{
  
  DBT key, data;
  DBT proxkey, proxdata;
  int **proxlist;
  int *listsize;
  int *matchpos;
  int numints;
  int result = 0;
  int minpos;
  int fail;
  int returncode;
  int listnum, list, i, j;

  prox_idx_pos_key poskeyval;


  if (dbprox == NULL)
    return 0;

  /* allocate an array of pointers for the actual prox lists for each term */
  proxlist = CALLOC(int *, num_prox_terms + 1);
  listsize = CALLOC(int, num_prox_terms +1);
  matchpos = CALLOC(int, num_prox_terms +1);

  /* Initialize the prox key/data pair so the flags aren't set. */
  for (listnum = 0; listnum < num_prox_terms; listnum++) {
    
    memset(&proxkey, 0, sizeof(proxkey));
    memset(&proxdata, 0, sizeof(proxdata));
    
    poskeyval.recnum = record_num;
    poskeyval.termid = proxtermidlist[listnum];
    
    proxkey.data = (void *) &poskeyval;
    proxkey.size = sizeof( prox_idx_pos_key);;

    returncode = dbprox->get(dbprox, NULL, &proxkey, &proxdata, 0); 
	  
    if (returncode == 0) { /* found the word in the index */
      numints = proxdata.size / sizeof(int);
      proxlist[listnum] = CALLOC(int, numints + 1);
      listsize[listnum] = numints;
      /* get the existing data (Move to aligned space) */
      memcpy(proxlist[listnum], (int *)proxdata.data, proxdata.size);
    }
  }
  /* we now have each of the lists for the term proximity info */
  /* we process the lists in order so the match is in order    */
  
  for (i = 0; i < listsize[0]; i++) {
    /* set an anchor */
    minpos = proxlist[0][i];
    matchpos[0] = minpos;
    fail = 0;

    for(list = 1; list < num_prox_terms; list++) {
      for (j = 0; j < listsize[list]; j++) {
	if (proxlist[list][j] < minpos) {
	  fail = 1; /* set in case we run out of data */
	  continue;
	}
	else if (proxlist[list][j] 
		 > (minpos + phrase_prox_length)) {
	  /* the "following" term too far past the current anchor */
	  fail = 1;
	  break;
	} else if (proxlist[list][j] == minpos && 
		   proxtermidlist[j] == proxtermidlist[j-1]) {
	  /* looking for doubled terms -- try the next item */
	  fail = 1;
	  continue;
	} else { 
	  /* it is in the appropriate range, so set a match and */
	  /* go on                                              */
	  fail = 0;
	  matchpos[list] = proxlist[list][j];
	  minpos = proxlist[list][j];
	  break;
	}
      }
      if (fail)
	break;
    }
    /* if any one of the term lists doesn't match we go on and try */
    /* a new anchor */
    if (fail == 0) {
      /* do the fine test... */
      result = 1; /* we think it is ok... */
      for (j = 1; j < num_prox_terms; j++) {
	/* this is fuzzy and may fail if there is a lot of whitespace */
	if (((matchpos[j]-matchpos[j-1]) - proxtermlength[j-1]) < 4)
	  continue;
	result = 0;
      }
    }
    if (result == 1) {
      result = matchpos[0]; /* return the start position of the match */
      break;
    }
  }
  
  /* clean up */
  for (listnum = 0; listnum < num_prox_terms; listnum++) 
    FREE(proxlist[listnum]);
  FREE (proxlist);
  FREE (listsize);

  return (result);

}


