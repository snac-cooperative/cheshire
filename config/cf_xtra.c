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
*	Header Name:	cf_xtra.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	Routines for creation of z39.50 elements needed
*                       for config file parsing (used only for test_config)
*	Usage:		
*
*	Date:		4/30/98
*	Revised:
*	Version:
*	Copyright (c) 1998.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include "z_parameters.h"
#include "z3950_3.h"

/* #include "configfiles.h" not needed and includes the extern def */
/* for the following flags -- these are set here                   */
/* these are referenced someplace and the NORMAL def is in cf_init.c */
char cf_create_file_flags[4];
char cf_open_file_flags[4];


#define STRINGVAL 0
#define INTVAL 1

GSTR * 
NewGSTR(s) 
char	*s;	/* MUST be NULL-terminated string.	*/
{
    	GSTR *gstr;
    	if (s == NULL)
		return (GSTR *)NULL;
    	if ((gstr = CALLOC(GSTR,1)) == NULL)
		return (GSTR *)NULL;
    	gstr->length = strlen(s);
    	gstr->data = (UCHAR *)calloc(1, strlen(s)+1);
	if (gstr->data == NULL)
		return (GSTR *)NULL;
    	strcpy(gstr->data, s);
    	gstr->data[gstr->length] = '\0';
    	return gstr;
}


GeneralizedTime 
NewGeneralizedTime(char* str) 
{
	return (GeneralizedTime)NewGSTR(str);
}

InternationalString 
NewInternationalString(char* str) 
{
	return (InternationalString)NewGSTR(str);
}


StringOrNumeric *
BuildStringOrNumeric(char *string, int numeric, int which)
{
  StringOrNumeric *tmp;

  tmp = CALLOC(StringOrNumeric, 1);

  if (which == STRINGVAL) {
    tmp->which = e25_string;
    tmp->u.string = NewInternationalString(string);
  }
  else {
    tmp->which = e25_numeric;
    tmp->u.numeric = numeric;
  }
  
  return (tmp);
}

HumanString *
BuildHumanString(char *text, char *language)
{
  HumanString *tmp;
  
  if (text == NULL)
    return NULL;

  tmp = CALLOC(HumanString, 1);
  tmp->item.language = (LanguageCode *)NewInternationalString(language);
  tmp->item.text = NewInternationalString(text);

  return(tmp);

}


