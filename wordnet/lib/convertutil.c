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
