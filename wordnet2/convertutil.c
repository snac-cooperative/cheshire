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
#include <stdio.h>
#include "wn.h"
#include "wnconsts.h"

main(argc, argv)
int argc;
char *argv[];
{
    char *s, posname[6], fname[30];
    int c;
    int fnum;

    extern char *optarg;
    extern int optind, opterr;

    wninit();

    while ((c = getopt(argc, argv, "a:b:c:d:")) != -1)
	switch(c) {
	case 'a':
	    s = StrToSense(optarg);
	    printf("%s\n", s);
	    break;
	case 'b':
	    s = SenseToStr(optarg);
	    printf("%s\n", s);
	    break;
	case 'c':
	    sscanf(optarg, "%d", &fnum);
	    s = LexFileName(NOUN, fnum); /* always use NOUN table */
	    printf("%s\n", s);
	    break;
	case 'd':
	    sscanf(optarg, "%[^.].%s", posname, fname);
	    fnum = LexFileNum(posname, fname);
	    printf("%d\n", fnum);
	    break;
	}
    
}

