/*
 *  Copyright (c) 1990-2005 [see Other Notes, below]. The Regents of the
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
*	Header Name:  detag_data_block
*
*	Programmer:   Ray R. Larson
*
*	Purpose:      Throwing tags out of a record
*                     This is PART of index/normalize_key.c
*	Usage:	      
*	Date:		6/18/2005
*	Revised:	
*	Version:	1.0
*	Copyright (c) 2005.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include "string.h"
#include "cheshire.h"
#include "configfiles.h"
#include "dmalloc.h"



/* the following function takes an SGML_data structure and turns everything  */
/* in it into a string allocated string after removing any imbedded SGML tags*/
/* It has the added effect of removing diacritics (of the ALA/LC character   */
/* set). We substitute romanized versions of selected non-roman non-ascii    */
/* characters.                                                               */

char *detag_data_block (SGML_Data *data, int index_type, idx_list_entry *idx)
{
  char *c, t, *databuf, *orig_databuf;
  int buffsize, i;
  unsigned char u;
  int contains_non_roman;

  int FIXONLY=0;

  if (data == NULL) return (NULL);

  if (idx == NULL && index_type == 0) {
    index_type = index_type | NORM_NOMAP;
  }

  buffsize = (long)data->content_end - (long)data->content_start;

  if (buffsize <= 0) return (NULL);

  /* calloc should initialize the databuf to nulls */
  /* we add extra space for char substitutions     */
  orig_databuf = CALLOC(char, buffsize + 10); 
  databuf = CALLOC(char, buffsize + 40); 
  strncpy(orig_databuf, data->content_start, buffsize);

  /* printf ("buffsize=%d\n",buffsize+40); */

  if ((index_type & NORM_DO_NOTHING) == NORM_DO_NOTHING) {
    return(orig_databuf);
  }

  /* if there are processing instructions associated with this data */
  /* then do them on the substituted buffer                         */
#ifdef NOTFORZSEARCHINGALONG
  if (data->processing_flags) {
    orig_databuf = in_proc_instr(data, orig_databuf, 
				 data->content_start_offset, idx);
  } 
#endif


  /* the following has potential problems when there is a literal */
  /* 'greater than' sign in the data -- this SHOULD be avoided by */
  /* using entity references for the > symbol                     */
  contains_non_roman = 0 ;

  for (i = 0, c = orig_databuf; *c ; c++, i++) { 
    t = u = *c;

    /* if processing instructions were handled check for  the following */
    if (data->processing_flags) {

      if (t == '\001' && *(c+1) == '\002' && *(c+2) == '\003') {
	/* special flag from processing instruction handling */
	/* to close up any gap of blanks in the data         */
	/* blanks, etc preceding these characters should have*/
	/* been removed already                              */
	c += 3;
	while (*c == ' ') c++;
	t = u = *c;
      }
      else if (t == '\001') {
	/* if there are extraneous 001's in the data, skip them */
	c+=1;
	t = u = *c;
      }
    }

    /* treat CDATA sections as data, and ignore any "tags" */
    if (strncmp(c,"<![CDATA[",9) == 0) {
      while (*c != '\0' && strncmp(c,"]]>",3) != 0)
	databuf[i++] = *c++;
      i--;
      continue;
    }

    /* in copying stuff skip over imbedded tags */
    if (t == '<') {
      /* squash out the remaining tags (not handled by proc instr) */
      while (t != '>' && *c ) {
	c++ ;
	t = *c;
	if (index_type & PROXIMITY) {
	  /* need to replace with spaces if this is a prox index */
	        databuf[i++] = ' ';
	}
      }
      t = ' ';
    }
#ifndef WIN32
    if (t == '\r') {
      /* squash out the carriage returns */
      while (t == '\r' && *c ) {
	c++ ;
	t = *c;
	if (index_type & PROXIMITY) {
	        databuf[i++] = ' ';
	}

      }
    }
#endif
    if ((u > 128) && ((index_type & (NORM_NOMAP | NORM_MIN)) == 0)) {
      if (u < 192) { /* Might be ALA */
	contains_non_roman = 1;
	switch (u) {
	case 161: t = 'L';
	  break;
	case 162: t = 'O';
	  break;
	case 163: t = 'D';
	  break;
	case 164: 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 165: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 166: 
	  databuf[i++] = 'O';
	  t = 'E';
	  break;
	case 167: t = '_';
	  break;
	case 168: t = '_';
	  break;
	case 169: t = '_';
	  break;
	case 170: t = '_';
	  break;
	case 171: t = '_';
	  break;
	case 172: t = 'O';
	  break;
	case 173: t = 'U';
	  break;
	case 174: t = '_';
	  break;
	case 175: t = '_';
	  break;
	case 176: t = '_';
	  break;
	case 177: t = 'l';
	  break;
	case 178: t = 'o';
	  break;
	case 179: t = 'd';
	  break;
	case 180: 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 181: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 182: 
	  databuf[i++] = 'o';
	  t = 'e';
	  break;
	case 183: t = '_';
	  break;
	case 184: t = 'i';
	  break;
	case 185: t = '_';
	  break;
	case 186: t = 'd'; /* this is eth  fixed now */
	  break;
	case 187: t = '_';
	  break;
	case 188: t = 'o';
	  break;
	case 189: t = 'u';
	  break;
	}
	databuf[i] = t;
      }
      else if (u > 191) {
	/* assume these are IS0 8859-1 (Latin-1) */
	switch (u) {
	case 192:
	case 193:
	case 194:
	case 195:
	case 196:
	case 197: t = 'A';
	  break;
	case 198: 
	  databuf[i++] = 'A';
	  t = 'E';
	  break;
	case 199: t = 'C';
	  break;
	case 200:
	case 201:
	case 202:
	case 203: t = 'E';
	  break;
	case 204:
	case 205:
	case 206:
	case 207: t = 'I';
	  break;
	case 208: /*Eth*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 209: t = 'N';
	  break;
	case 210:
	case 211:
	case 212:
	case 214:
	case 216: t = 'O';
	  break;
	case 215: t = ' ';
	  break;
	case 217:
	case 218:
	case 219:
	case 220: t = 'U';
	  break;
	case 221: t = 'Y';
	  break;
	case 222: /*Thorn*/ 
	  databuf[i++] = 'T';
	  t = 'h';
	  break;
	case 223: /*ss*/ 
	  databuf[i++] = 's';
	  t = 's';
	  break;
	case 224:
	case 225:
	case 226:
	case 227:
	case 228:
	case 229: t = 'a';
	  break;
	case 230: 
	  databuf[i++] = 'a';
	  t = 'e';
	  break;
	case 231: t = 'c';
	  break;
	case 232:
	case 233:
	case 234:
	case 235: t = 'e';
	  break;
	case 236:
	case 237:
	case 238:
	case 239: t = 'i';
	  break;
	case 240: /*Eth*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	case 241: t = 'n';
	  break;
	case 242:
	case 243:
	case 244:
	case 245:
	case 246:
	case 248: t = 'o';
	  break;
	case 247: t = ' ';
	  break;
	case 250:
	case 251:
	case 252: t = 'u';
	  break;
	case 253:
	case 255: t = 'Y';
	  break;
	case 254: /*thorn*/ 
	  databuf[i++] = 't';
	  t = 'h';
	  break;
	}
	databuf[i] = t;
      } 
      else { /* although these are non-roman -- they are already handled */
	/* if it isn't in the range -- ignor it and re-use the position */ 
	i--; 
      }
    }
    else {
      if (i > buffsize+40) {
	continue;
	/* printf ("ERROR IN DETAG_DATABLOCK: Try NOMAP options: i=%d\n",i); */
      }
      databuf[i] = t;
    }
  }

  /* The following is a special fix (that should no longer be needed) */
  /* for adding ONLY index items with non-roman substituted characters*/
  if (FIXONLY == 1) {
    if (contains_non_roman == 1) {
      FREE(orig_databuf);
      return (databuf);
    }
    else { /* ignor normal words */
      FREE(databuf);
      FREE(orig_databuf);
      return (NULL);
    }
  }

  /* if FIXONLY is NOT on return the buffer */
  FREE(orig_databuf);
  return (databuf);
}

