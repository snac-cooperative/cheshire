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
/* test scanner code for dtds */
#include <stdio.h>

#define MAIN
#include "cheshire.h"

extern char *temp_parse_buffer;

extern SGML_DTD *DTD_list, *current_sgml_dtd;

extern FILE *sgml_in;

extern void dump_dtd(SGML_DTD *DTD);

extern void dump_completer(SGML_DTD *DTD_list, char *filename);

extern void free_all_dtds();
extern SGML_DTD *sgml_parse_dtd(char *filename, char *sgml_catalog_name, char *schema_file_name, filelist *fl, int dtd_type);

FILE *LOGFILE;

int last_rec_proc = 0;

extern char *sgml_in_name;

main (int argc, char **argv)
{
  SGML_DTD *dtd, *cf_getDTD();
  extern SGML_Doclist  *PRS_first_doc; /* set in document parsing */
  SGML_Document *doc;
  int i, morerecs = 1;
  char outname[200];
  char *docbuffer;
  struct stat filestatus, filestatus2;
  int statresult;
  int datasize;
  FILE *indocfile;
  filelist *fl=NULL, *newfl=NULL;

  LOGFILE = stdout;


  if (argc < 3) {
    fprintf(stderr, "usage: %s dtdfilename schemafilename <included_schema1 included_schema2 ...> \n", argv[0]);
    exit(0);
  }

  ++argv, --argc; /* skip program name */

  if (argc > 2) {

    for (i = 2; i < argc; i++) {

      newfl = CALLOC(filelist, 1);
      newfl->filename = argv[i];
      if (fl == NULL) {
	fl = newfl;
      }
      else {
	newfl->next_filename = fl;
	fl = newfl;
      }
    }
  }
  else
    fl = NULL;

  dtd = sgml_parse_dtd(argv[0], NULL, argv[1], fl, 2);

  printf ("Dumping dtd\n");
  dump_dtd(dtd);
     
  free_all_dtds();

  return (0);
}









