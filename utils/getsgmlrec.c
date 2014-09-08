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
 * Copyright (c) 1994 The Regents of the University of California.
 * All rights reserved.
 *
 * Author:	Ray Larson, ray@sherlock.berkeley.edu
		Jerome McDonough jmcd@lucien.berkeley.edu
 *		School of Library and Information Studies, UC Berkeley
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef WIN32
#include <stdlib.h>
#define strncasecmp strnicmp
#endif
#include <cheshire.h>
#include <ctype.h>
#include <dmalloc.h>

/************************************************************************
 * GET_NEXT_SGMLREC reads sequentially from the named SGML file and
 * matches SGML records bracketed by the given tag.
 * It then allocates a buffer for that record and returns a pointer to
 * the buffer (which should be freed when no longer needed). 
 * It keeps track of the file currently being read, and keeps reading
 * sequentially as long as each call has the same filename. A new filename
 * starts a new read in that file.
 ************************************************************************/
FILE *infile = NULL;

int
get_next_sgmlrec_data(int *offset, char *filename, char *tag, int taglen, int skipheaders)
{
  static char currentfile[500];
  int go_on;
  int c, c2, c3, c4;
  int trailing_data;
  char i;
  int length;
  int tagstartpos;
  char tagbuff[500];
  int bytesread;
  int recstart;
  FILE *LOGFILE = stderr;
  int in_comment = 0;
  char *fmread;
#ifdef WIN32
  /* have to define the file as binary */
  /* otherwise, if there are only newlines in the file, FTELL will */
  /* give incorrect locations -- (stupid MSDOG bug)                */
  fmread = "rb";
#else
  fmread = "r";
#endif

  if (filename == NULL && tag == NULL) {
    /* signal to close the file */
    if (infile)
		fclose(infile);
    infile = NULL;
    return 0;
  }

  /* Is this the same or a new file? */
  if (filename != NULL && strcmp(filename, currentfile) != 0) {
    /* its a new file */
    if (infile != NULL) 
      fclose(infile);
    strcpy (currentfile, filename);
    infile = fopen(currentfile, fmread);
    
    if (infile == NULL) {
      fprintf (LOGFILE, "Couldn't open sgml input file %s \n",
	       filename);
      return 0;
    }
    /* if we are appending "offset" will contain the previous EOF */
    /* if a new file offset should be set to 0 and we start at the*/
    /* beginning                                                  */
    fseek(infile,*offset, 0);
  }
  go_on = 1;
  recstart = -1;
  trailing_data = 0;
  length = 0;

 retryskiph:

  while (1) {
    /* first skip any junk before the first tag */
    if ((c = getc(infile)) != '<' && c != EOF) {
      length++;
      continue;
    }
    length++;
    /* if it is a comment or dtd stuff */
    /* keep looking                    */
    if ((c2 = getc(infile)) == '!' && c != EOF) {
      length++;
      if ((c3 = getc(infile)) == '-' && c != EOF) {
	length++;
	if ((c4 = getc(infile)) == '-' && c != EOF) {
	  length++;
	  /* in a comment */
	  in_comment = 1;
	  while (in_comment && (c2 = getc(infile)) != EOF) {
	    length++;
	    if (c2 == '-' && c2 != EOF) {
	      if ((c3 = getc(infile)) == '-' && c2 != EOF) {
		length++;
		if ((c4 = getc(infile)) == '>' && c2 != EOF) {
		  length++;
		  /* end of the comment */
		  in_comment = 0;
		}
	      }
	    }
	  }
	}
      } else if (recstart == -1 && skipheaders == 0) {
	recstart = ftell(infile) - 3;
	length += 3;
	trailing_data = 1;
      }
      continue;                      
    }
    else if (c2 == '?' && c != EOF) {
      if (recstart == -1 && skipheaders == 0) {
	recstart = ftell(infile) - 2;
	length += 2;
	trailing_data = 1;
      }
      continue;
    }
    else {
      length++;
      break;
    }
  }


  
  if (c == EOF || c2 == EOF) {
    if (trailing_data)
      return (length * -1);
    else
      return 0;
  }

  /* found a < followed by something other than a ! or ? */

  /* pass back where the new record will be starting */
  if (recstart == -1) {
    tagstartpos = (ftell(infile) - 2); /* -2 for the < and first char */
    length = 2;
  }
  else {
    tagstartpos = recstart;
    length = ftell(infile) - recstart;
  }
  
  tagbuff[0] = c2;

  /* this SHOULD be the main tag but check it out */
  if ((bytesread = fread((tagbuff+1),sizeof(char),taglen,infile)) != taglen) {
    length += bytesread;
    /*fclose(infile);*/
    return 0;
  }

  length += bytesread;

  i = tagbuff[taglen];
  if (strncasecmp(tagbuff,tag,taglen) == 0
   && (i == ' ' || i == '\t' || i == '\n' || i == '>')) {
    /* matched the tag */
  }
  else if (skipheaders) {
    recstart = -1;
    goto retryskiph;
  }
  else {
    fprintf(LOGFILE, "No tag match in get_next_sgmlrec: File %s\n", filename);
    return 0;
  }
  
  while (go_on) {

    while ((c = getc(infile)) != '<' && c != EOF) {
      length++; /* just keep adding */
    }

    if (c == EOF) return 0;

    length++; /* add the < */
    
    if ((c= getc(infile)) == '/') {
      length++; /* add the / */
      /* this SHOULD be the main end tag but check it out */
      for (i = 0; i < taglen; i++) {
	c2 = getc(infile);
	length++;
	if (c2 == tag[i] || (c2+32) == tag[i] || ((c2 > 32) && (c2-32 == tag[i])))
	  continue;
	else
	  break;
      }
      if (i == taglen) { /* match */
	if ((c = getc(infile)) == '>') {
	  length++;
	  go_on = 0;
	}
	else
	  length++;
      }
    }
    else
      length++;
  }

  *offset = tagstartpos;
  return (length);
}






