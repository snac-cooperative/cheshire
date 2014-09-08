/***************** swap bytes for World Data Bank data ********************/
/* takes the big-endian version of the wdb data and makes it little-endian*/
/* by Ray R. Larson (mar 92)                                              */
/**************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>

typedef struct { unsigned short Code, Lat, Lon; } PNT ;
#define PNTSiz	sizeof(PNT)

PNT	p_in, p_out;
char	*infile, *outfile;

/***************************** main routine ********************************/
main(argc, argv)
int argc;
char *argv[];
{
        int in, out, n; /* file handles */
        short lo, hi;

	if (argv[1][1] == '?')  {
		fprintf(stdout, "Usage: wdbconvert in_file out_file\n");
		exit(0);
	}
	if (argc == 3) {
		infile = argv[1];
		outfile = argv[2];
        }
        else  {
		fprintf(stdout, "Usage: wdbconvert in_file out_file\n");
		exit(0);
	}

        in = open(infile, O_RDONLY);
        out = open(outfile, O_CREAT | O_TRUNC | O_WRONLY);

	if (in >= 0 && out >= 0) {
		while (n = read(in, &p_in, PNTSiz)) {
#ifdef DEBUG
                       printf("IN code: %d  Lat: %d  Lon: %d \n",
				p_in.Code,p_in.Lat,p_in.Lon);
#endif
                        /* swap bytes in the data */
			lo = p_in.Code << 8;
			hi = p_in.Code >> 8;
			p_out.Code = lo | hi;
			lo = p_in.Lat << 8;
			hi = p_in.Lat >> 8;
			p_out.Lat = lo | hi;
			lo = p_in.Lon << 8;
			hi = p_in.Lon >> 8;
			p_out.Lon = lo | hi;
#ifdef DEBUG
                       printf("OUT code: %d  Lat: %d  Lon: %d \n",
				p_out.Code,p_out.Lat,p_out.Lon);
#endif
			write(out, &p_out, PNTSiz);

		}
		close(in);
                close(out);
	}
	else {
		fprintf(stdout, "Couldn't open '%s' and '%s'\n", infile, outfile);
        }
        exit (0);
}
