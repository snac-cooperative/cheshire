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
*	Header Name:	getdata.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	To obtain data from a tree of SGML_Data structs
*                       for a data struct matching a particular key or
*                       key and subkey
*
*	Usage:		GetData(sgmlrec, curdata, buffer, tag, top_key);
*
*	Variables:	SGML_Document *sgmlrec; SGML Document being indexed
*                       SGML_Data *curdata; Pointer to current position in
*                                           in tree of SGML_Data structs
*                       char *buffer; buffer to handle extracted data to index
*                       char *tag; Key we're currently searching for. If it
                                   matchines top_key->key, we're indexing all
*                                  of a given element.  If not, we're only
*                                  subelements <tag> of element <top_key->key>.
*                       idx_key *top_Key; top-most key that we're looking for.
*                       
*
*	Return Conditions and Return Codes:	FAIL if not provided
*                                                    appropriate parameters,
*                                               NULL if no more data elements
*                                                    matching key to be found,
*                                               SGML_Data * (pointer to
*                                                           struct containing
*                                                           info to be indexed
*                                                           if successful
*
*
*	Date:		09/01/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/


#include <stdio.h>
#include <ctype.h>

#include "cheshire.h"

extern int stagcmp(char *tag1, char *tag2);

SGML_Data *GetData(curdata,tag, matchflag)
SGML_Data *curdata;
char *tag;
int *matchflag;
{
  SGML_Data *data;
  int i = 0;
  int match = 0;
  
  *matchflag = 0;

  if (curdata == NULL)
    return (NULL);
  
  data = curdata;
  
  
  if ((match = stagcmp(tag, data->start_tag))== 1) {
#ifdef DEBUGIND
    printf("matching start_tag '%s'\n", tag);
#endif
    *matchflag = 1;
    return(data);
  }
  else {
    if (*matchflag == 0 && (data->sub_data_element))
      curdata = GetData(data->sub_data_element, tag, matchflag);

    if (*matchflag == 0 && (data->next_data_element))
      curdata = GetData(data->next_data_element, tag, matchflag);
  }
  return(curdata);
}


