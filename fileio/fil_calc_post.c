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
*	Header Name:	fil_calc_post.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	utility routines to calculate variable size postings
*                       blocks.
*
*	Usage:		calc_post_blocksize (recnum) -- returns the size in
*                                                       bytes for the block
*                                                       containing recnum's
*                                                       entry
*                       calc_post_blocknum (recnum) -- returns the number of
*                                                      postings in a recnum's
*                                                      block
*                       calc_post_remaining (recnum)-- returns the number of
*                                                      unused slots in recnum's
*                                                      block.
*                                                      
*                       
*	Variables:	
*                       
*	Return Conditions and Return Codes:	
*
*	Date:		7/31/97
*	Revised:	7/31/97
*	Version:	1.0
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "cheshire.h"
#include "dmalloc.h"

int calc_post_blocksize(int recnum)
{
  int i;
  
  for (i=2;i<31; i++) {
    /* first blocks will have 4 entries */
    if ((recnum+3) < (1<<(i+1))) {
      /* return the block size in bytes */
      /* note that the post_result structure contains one entry already */
      return(sizeof(idx_result) + (((1<<i)-1) * sizeof(idx_posting_entry)));
    }
  }
}

int calc_post_blocknum(int recnum)
{
  int i;
  
  for (i=2;i<31; i++) {
    if ((recnum+3) < (1<<(i+1))) {
      return(1<<i);			
    }
  }
}

int calc_post_remaining (int recnum)
{
  int y;
  y = calc_post_blocknum(recnum);
  return ((((y * 2)-3)-recnum)-1);
}

#ifdef TESTCALCSTUFF
int
main (int argc, char **argv)
{
  int recnum;
  
  recnum = atoi(argv[1]);

  printf ("Blocksize   %d\n",  calc_post_blocksize(recnum));
  printf ("Blockslots  %d\n",  calc_post_blocknum(recnum));
  printf ("Blockremain %d\n",  calc_post_remaining(recnum));
}
#endif

