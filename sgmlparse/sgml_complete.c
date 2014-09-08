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
*	Header Name:	completer.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	This routine takes a filled in SGML_DTD structure
*                       and builds a "completer" pattern file for sgmlsasp
*	Usage:		
*
*	Variables:	
*
*	Return Conditions and Return Codes:	
*
*	Date:		7/31/94
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "sgml.h"
#ifndef SOLARIS
#ifndef WIN32
#include "strings.h"
#endif
#endif
#include <string.h>
#include "dmalloc.h"

extern FILE *LOGFILE;


char *up_case_string(char *s)
{
  char *c;
  extern int sgml_in_dtd_type;

  if (sgml_in_dtd_type != 0)
    return(s);

  if (s == NULL) return (NULL);

  for(c = s; *c; c++)
    *c = toupper(*c);

  return (s);
}


void
dump_completer(SGML_DTD *DTD_list, char *filename)
{

  FILE *outfile;

  SGML_Element *el; 
  SGML_Tag_List *tags;
  SGML_Attribute *at;
  
  char *tag;
  char *attrname;

  if (DTD_list == NULL) {
    fprintf(LOGFILE,"No DTDs to dump\n");
    return;
  }

  if (filename == NULL) {
    fprintf(LOGFILE,"No filename for completer dump file\n");
    return;
  }

  outfile = fopen(filename,"w");

  for ( el = DTD_list->Elements; el != NULL ; el = el->next_element) {
    if (el->tag != NULL && el->attribute_list == NULL) {
      tag = up_case_string(strdup(el->tag));
      fprintf(outfile,"<%s>\t\t\"<%s>\"\n",tag, tag);
      fprintf(outfile,"</%s>\t\t\"</%s>\"",tag, tag);
      FREE(tag);
    }
    else if (el->tag_list != NULL && el->attribute_list == NULL) { 
      for (tags = el->tag_list ; tags ; tags = tags->next_tag) {
	tag = up_case_string(strdup(tags->tag));
	fprintf(outfile,"<%s>\t\t\"<%s>\"\n",tag, tag);
	fprintf(outfile,"</%s>\t\t\"</%s>\"\n",tag, tag);
	FREE(tag);
      }
    } else if (el->tag != NULL && el->attribute_list != NULL) {
      tag = up_case_string(strdup(el->tag));
      fprintf(outfile,"<%s>\t\t\"<%s ",tag, tag);
      for (at = el->attribute_list; at ; at = at->next_attribute) {
	attrname = up_case_string(strdup(at->name));
	fprintf(outfile," %s = [%s]", attrname, attrname);
	FREE(attrname);
      }
      fprintf(outfile,">\"\n");
      fprintf(outfile,"</%s>\t\t\"</%s>\"",tag, tag);
      FREE(tag);
    }
    else if (el->tag_list != NULL && el->attribute_list != NULL) { 
      for (tags = el->tag_list ; tags ; tags = tags->next_tag) {
	tag = up_case_string(strdup(tags->tag));
	fprintf(outfile,"<%s>\t\t\"<%s ",tag, tag);

	for (at = el->attribute_list; at ; at = at->next_attribute) {
	  attrname = up_case_string(strdup(at->name));
	  fprintf(outfile," %s = [%s]", attrname, attrname);
	  FREE(attrname);
	}
	fprintf(outfile,">\"\n");
	fprintf(outfile,"</%s>\t\t\"</%s>\"\n",tag, tag);
	FREE(tag);
      }
    }
    if (el == DTD_list->Elements)
      fprintf(outfile,"\t+\n"); /* put returns after last element */
    else
      fprintf(outfile,"\n");
  }
  fclose(outfile);
}











