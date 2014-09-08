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
*	Header Name:	EntSub.c
*
*	Programmer:     Jerome P. McDonough
*
*	Purpose:	Translate some characters into SGML entity references
*                       in order to avoid interpreting data as markup
*
*	Usage:		EntSub(fieldbuffer);
*
*	Variables:	char *fieldbuffer -- point to char array of
*                                            FIELDBUFSIZE containing
*                                            MARC field information to
*                                            translate
*
*	Return Conditions and Return Codes:	SUCCESS
*
*	Date:		10/11/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/

#include <stdio.h>
#include "unicode_tables.h"

void outfunc(char *in) {
  char *c;
  for (c = in; *c; c++) {
    putchar(*c);
  }
}

int
main(int argc, char **argv)
{
  int c, c2, final, firstchar, nextchar, temp;
  int i, *setptr;
  int set0, set1;

  set0 = 1;
  set1 = 2;

  if (argc > 1) {
    printf("usage: %s < infile > outfile\n", argv[0]);
    exit(0);
  }

  
  setptr == NULL;
  
  while ((c = getchar()) != EOF) {
    i = (int) c;

    if (c == 0x1B) {
      /* escape sequence for alternate character sets */
      switch (getchar()) {
      case 0x67: /* Greek Symbol set, Table 3 */
	set0 = 3;
	i = (int)getchar();
	outfunc(&char_table_3[i].UTF_8[0]);
	break;
      case 0x62: /* Subscripts set, Table 4 */
	set0 = 4;
	i = (int)getchar();
	outfunc(&char_table_4[i].UTF_8[0]);
	break;
      case 0x70: /* Superscript set, Table 5 */
	set0 = 5;
	i = (int)getchar();
	outfunc(&char_table_5[i].UTF_8[0]);
	break;
      case 0x73: /* Outfuncto ASCII & ANSEL Table 1 & 2 */
	set0 = 1;
	set1 = 2;
	i = (int)getchar();
	outfunc(&char_table_1[i].UTF_8[0]);
	break;
      case 0x28: /* Beginning G0 set with single char representation */
      case 0x2C:
	if ((temp = getchar()) == ',') {
	  final = getchar();
	  firstchar = getchar();
	}
	else {
	  final = temp;
	  firstchar = getchar();
	}
	switch (final) {
	case 0x33: /* Basic Arabic - table 9 */
	  set0 = 9;
	  i = (int)firstchar;
	  outfunc(&char_table_9[i].UTF_8[0]);
	  break;
	case 0x34: /* Extended Arabic - table 10 */
	  set0 = 10;
	  i = (int)firstchar;
	  outfunc(&char_table_10[i].UTF_8[0]);
	  break;
	case 0x42: /* Basic Latin and Extended Latin table 1 */
	case 0x21:
	  set0 = 1;
	  set1 = 2;
	  if (final == 0x21 && firstchar == 0x45) {
	    firstchar = getchar();
	  }  
	  outfunc(&char_table_1[(int)firstchar].UTF_8[0]);	  
	  break;
	case 0x31: /* Chinese Japanese and Korean table 12 */
	  /* Really need to do this SOMETIME */
	  printf ("CJK conversion not working yet, sorry... \n");
	  break;
	case 0x4E: /* Basic Cyrillic - Table 7 */
	  set0 = 7;
	  i = (int)firstchar;
	  outfunc(&char_table_8[i].UTF_8[0]);
	  break;
	case 0x51: /* Extended Cyrillic - Table 8 */
	  set0 = 8;
	  i = (int)firstchar;
	  outfunc(&char_table_1[i].UTF_8[0]);
	  break;
	case 0x53: /* Basic Greek - Table 11 */
	  set0 = 11;
	  i = (int)firstchar;
	  outfunc(&char_table_11[i].UTF_8[0]);
	  break;
	case 0x32: /* Basic Hebrew - Table 6 */
	  set0 = 6;
	  i = (int)firstchar;
	  outfunc(&char_table_6[i].UTF_8[0]);
	  break;
	  
      }
	break;
      case 0x24: /* Beginning G0 or G1 set with multiple char representation */
	/* A bunch of this is duplicated, because it AINT CLEAR what   */
	/* combinations are OK                                         */
	if ((temp = getchar()) == ',') {
	  final = getchar();
	  firstchar = getchar();
	  setptr = &set0;
	} else if (temp == ')' || temp == '-') {
	  final = getchar();
	  firstchar = getchar();
	  setptr = &set1;
	}
	else {
	  final = temp;
	  firstchar = getchar();
	  setptr = &set0;	
	}
	switch (final) {
	case 0x33: /* Basic Arabic - table 9 */
	  *setptr = 9;
	  i = (int)firstchar;
	  outfunc(&char_table_9[i].UTF_8[0]);
	  break;
	case 0x42: /* Basic Latin and Extended Latin table 1 */
	case 0x21:
	  set0 = 1;
	  set1 = 2;
	  if (final == 0x21 && firstchar == 0x45) {
	    firstchar = getchar();
	  }  
	  outfunc(&char_table_1[(int)firstchar].UTF_8[0]);
	  
	  break;
	case 0x31: /* Chinese Japanese and Korean table 12 */
	  /* Really need to do this SOMETIME */
	  printf ("CJK conversion not working yet, sorry... \n");
	  break;
	case 0x4E: /* Basic Cyrillic - Table 7 */
	  *setptr = 7;
	  i = (int)firstchar;
	  outfunc(&char_table_8[i].UTF_8[0]);
	  break;
	case 0x53: /* Basic Greek - Table 11 */
	  *setptr = 11;
	  i = (int)firstchar;
	  outfunc(&char_table_11[i].UTF_8[0]);
	  break;
	case 0x32: /* Basic Hebrew - Table 6 */
	  *setptr = 6;
	  i = (int)firstchar;
	  outfunc(&char_table_6[i].UTF_8[0]);
	  break;
	  
	}
	break;
      case 0x29: /* Switching the G1 set for single chars */
      case 0x2D:
	final = getchar();
	firstchar = getchar();
	
	switch (final) {
	case 0x34: /* Extended Arabic - table 10 */
	  set1 = 10;
	  i = (int)firstchar;
	  outfunc(&char_table_10[i].UTF_8[0]);
	  break;
	case 0x42: 
	case 0x21: /* Basic Latin and Extended Latin table 1 */
	  set0 = 1;
	  set1 = 2;
	  if (final == 0x21 && firstchar == 0x45) {
	    firstchar = getchar();
	  }  
	  outfunc(&char_table_1[(int)firstchar].UTF_8[0]);
	  
	  break;
	case 0x31: /* Chinese Japanese and Korean table 12 */
	  /* Really need to do this SOMETIME */
	  printf ("CJK conversion not working yet, sorry... \n");
	  break;
	case 0x51: /* Extended Cyrillic - Table 8 */
	  set1 = 8;
	  i = (int)firstchar;
	  outfunc(&char_table_8[i].UTF_8[0]);
	  break;
	}
	
	break;
	
      }
      
    }
    else { /* not an escape, so put out the data in the current set */
      if (c <= 127) {
	/* set G0 */
	switch (set0) {
	case 1:
	  outfunc(&char_table_1[i].UTF_8[0]);
	  break;

	case 2:
	  outfunc(&char_table_1[i].UTF_8[0]);
	  break;

	case 3:
	  outfunc(&char_table_3[i].UTF_8[0]);
	  break;

	case 4:
	  outfunc(&char_table_4[i].UTF_8[0]);
	  break;

	case 5:
	  outfunc(&char_table_5[i].UTF_8[0]);
	  break;

	case 6:
	  outfunc(&char_table_6[i].UTF_8[0]);
	  break;

	case 7:
	  outfunc(&char_table_7[i].UTF_8[0]);
	  break;

	case 8:
	  outfunc(&char_table_8[i].UTF_8[0]);
	  break;

	case 9:
	  outfunc(&char_table_9[i].UTF_8[0]);
	  break;

	case 10:
	  outfunc(&char_table_10[i].UTF_8[0]);
	  break;

	case 11:
	  outfunc(&char_table_11[i].UTF_8[0]);
	  break;

	}
      } else {
	/* set G1 */
	switch (set1) {
	case 1:
	  outfunc(&char_table_1[i].UTF_8[0]);
	  break;

	case 2:
	  outfunc(&char_table_1[i].UTF_8[0]);
	  break;

	case 3:
	  outfunc(&char_table_3[i].UTF_8[0]);
	  break;

	case 4:
	  outfunc(&char_table_4[i].UTF_8[0]);
	  break;

	case 5:
	  outfunc(&char_table_5[i].UTF_8[0]);
	  break;

	case 6:
	  outfunc(&char_table_6[i].UTF_8[0]);
	  break;

	case 7:
	  outfunc(&char_table_7[i].UTF_8[0]);
	  break;

	case 8:
	  outfunc(&char_table_8[i].UTF_8[0]);
	  break;

	case 9:
	  outfunc(&char_table_9[i].UTF_8[0]);
	  break;

	case 10:
	  outfunc(&char_table_10[i].UTF_8[0]);
	  break;

	case 11:
	  outfunc(&char_table_11[i].UTF_8[0]);
	  break;

	}

      }
    }
  }

  exit(0);
}


