#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>


static char usage[] =
"Usage: cpmarc2 -b000 -n000 from to\n\
        where -n gives number of MARC records to copy \n\
              -b gives the number of record to skip before starting\n\
              'from' is the input file and 'to' is the output file\n";

/* maximum number of records to output */
#define MAXRECSOUT 1000000
#define RECTERM   '\035'

/************************************************************************/
/* Main routine of printmarc - goes through a file of marc recs and     */
/* prints them out according to format provided (as spec file)          */
/************************************************************************/
main(argc,argv)
int argc;
char *argv[];
{
	FILE *from, *to, *fopen();
	int i, c, nrecs, frec;
	char *tofile, *fromfile, *ac;
  

	nrecs = MAXRECSOUT; /* 'all' of them, may change with args */

	/* Handle command line arguments */
	if (argc < 4) {
		fputs (usage, stderr);
		exit(1);
	}
	tofile = argv[argc-1];
	fromfile = argv[argc-2];
	for (i = 1; i < (argc-2); i++) {
		ac = argv[i];
		if (*ac == '-')
			switch (*++ac) {
				case 'b': if (isdigit(*++ac))
							frec = atol(ac);
						else {
							fputs (usage, stderr);
							exit(1);
						}
						break;
				case 'n': if (isdigit(*++ac))
							nrecs = atol(ac);
						else {
							fputs (usage, stderr);
							exit(1);
						}
						break;
				default : fputs (usage, stderr);
						exit(1);
			}
	}


  /* open the input file */
  if ((from = fopen(fromfile,"r")) == NULL)
    { printf("Cannot open MARC input file %s\n",fromfile);
     exit(1);
    }
  /* open the output file */
  if ((to = fopen(tofile,"w")) == NULL)
    { printf("Cannot open MARC output file %s\n",tofile);
     exit(1);
    }

/* skip frec records before starting the copy */
while(frec) 
   {
     c = getc(from);
     if (c == -1) frec = 0;
     else
         if (c == RECTERM) frec--;
    }

/* Loop through the file - print records until nrecs are printed or EOF */
while(nrecs) 
   {
     c = getc(from);
     if (c == -1) nrecs = 0;
     else
        {
         if (c == RECTERM) nrecs--;
         putc((char)c,to);
        }
    }
     
 fclose(to);
 fclose(from);

}

