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
*	Header Name:  in_pagedel.c
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Removing all index keywords in a set of "pages", that is
*                     individual components of a document stored in a
*                     directory with names containing a number that
*                     indicates the sequence of the pages.
*
*	Usage:	      page_file_delete(int recnum, idx_list_entry *idx)
*
*	Variables:    recnum -- the record id to delete 
*                     idx -- the paged index
*
*	Return Conditions and Return Codes:	
*
*	Date:		03/01/97
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1997.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "cheshire.h"
#include "stem.h"
#include "configfiles.h"
#include "dmalloc.h"
#ifdef SOLARIS
#include <sys/dirent.h>
#endif
#include <dirent.h>

extern batch_files_info *batch_files;

int 
page_file_delete(int recnum, idx_list_entry *idx)
{
  int contflag = 1;
  int found_flag = 0;
  int neg_one = -1;
  page_assoc_rec page_assoc_rec_data;

  /* This is a brute force scan of the page mapping file and */
  /* marking of the items with the matching record number    */
  

  if (idx->page_rec_assoc_file == NULL) {
    fprintf(LOGFILE, 
	    "NULL index file pointer for pagefile associator in idxpages\n");
    return(0);
  }

  fseek(idx->page_rec_assoc_file, sizeof(page_assoc_rec), 0);
  
  while (fread(&page_assoc_rec_data, sizeof(page_assoc_rec), 1,
	       idx->page_rec_assoc_file) && contflag) {

    if (page_assoc_rec_data.parentid == recnum) {
      found_flag = 1;
      page_assoc_rec_data.parentid *= -1; /* mark it deleted */
      /* seek back in the file */
      fseek(idx->page_rec_assoc_file, (sizeof(page_assoc_rec) * neg_one), 1);
      fwrite(&page_assoc_rec_data, sizeof(page_assoc_rec), 
	     1, idx->page_rec_assoc_file);
    
    }
    else {
      /* since these are written sequentially, we can stop when the */
      /* id number changes                                          */
      if (found_flag == 1) contflag = 0;
    }

  }

  return(0);
}

