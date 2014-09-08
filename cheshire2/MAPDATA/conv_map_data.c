#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* read a binary file of map data and swap the byte order on output */
/* this program is useful to go from a bigendian machine to a littleendian */
/* machine or vice-versa with the WDB map data included in this directory  */

typedef struct { 
  unsigned short Code, Lat, Lon; 
} Point ;


main (int argc, char **argv) 
{

  unsigned short y;
  unsigned short tmpcode;
  unsigned short tmplon;
  unsigned short tmplat;
  FILE *infile, *outfile;
  Point inpoint, outpoint;

  if (argc < 2) {
    printf("Usage: %s in_map_file out_map_file\n", argv[0]);
    exit(0);
  }
  
  infile = fopen(argv[1], "r");
  outfile = fopen(argv[2], "w");

  /* if we have a bigendian machine vs a littleendian machine */
  
  while (fread(&inpoint, sizeof(Point), 1, infile) > 0) {
	  
	  y = (unsigned short)inpoint.Code << 8;
	  tmpcode = (unsigned short)y | (inpoint.Code >> 8);
	  y = (unsigned short) inpoint.Lat << 8;
	  tmplat = (unsigned short) y | (inpoint.Lat >> 8);
	  y = (unsigned short) inpoint.Lon << 8;
	  tmplon = (unsigned short) y | (inpoint.Lon >> 8);
	  outpoint.Code = tmpcode;
	  outpoint.Lon = tmplon;
	  outpoint.Lat = tmplat;

	  fwrite(&outpoint, sizeof(Point), 1, outfile);
  }

  fclose(infile);
  fclose(outfile);

  exit(0);

}
