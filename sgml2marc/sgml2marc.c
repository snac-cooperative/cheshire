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
*	Header Name:	sgml2marc.c
*
*	Programmer:	Jerome McDonough
*
*	Purpose:	takes an SGML marc record data structure, and
*                       produces an actual USMARC record
*
*	Usage:		int sgml2marc(SGML_Document *sgmldoc, char *marcrec)
*                       
*	Variables:	config_file_info *cf_info_base -- external 
*                            structure holding the configfile info;
*                       SGML_Document *sgmldoc -- parameter pointing at rec. to
*                            convert
*                       char *marcrec -- parameter consisting of pointer to
*                            damned-big-string to hold converted record
*
*	Return Conditions and Return Codes:	
*                       returns 1 (SUCCESS) on successful completion
*                       or 0 (FAILURE) on error;
*                       WARNING: You better calloc your memory for marcrec
*                                in the calling function, cause it's sure
*                                not done here.
*
*	Date:		10/26/94
*	Version:	1.0
*	Copyright (c) 1994.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include <string.h>
#include "sgml.h"
#include "dmalloc.h"
#include "alpha.h"
#define SUCCESS 1
#define FAILURE 0
#define SFDELIM '\037'
#define FT '\036'
#define RT '\035'
extern FILE *LOGFILE;

int sgml2marc(sgmldoc, marcrec, recsize)
SGML_Document *sgmldoc;
char *marcrec;
long recsize;
{

     char leader[25];
     char *directory;
     char *varflds;
     long fldsize;
     SGML_Data *dataptr;
     int i = 0, newbaseaddr = 0, newLRL = 0;
     char BAstring[6];
     char LRLstring[6];
     
     /* confirm that what we're dealing with is really an SGML MARC record */

     if ((strcmp(sgmldoc->DTD->DTD_name, "USMARC")) != 0)
     {
	  fprintf(LOGFILE, "sgml2marc: SGML Document is Not USMARC.\n");
	  return(FAILURE);
     }

     /* allocate directory and varfields strings large enough to hold record */
     /* and clean them up */
     varflds = (char *) malloc(recsize);
     directory = (char *) malloc(recsize);
     memset (leader, 0, 25);
     memset (directory, 0, (recsize));
     memset (varflds, 0, recsize);
     memset (BAstring, 0, 6);
     memset (LRLstring, 0, 6);
     
     /* move existing SGML leader information into leader string */
     dataptr = sgmldoc->data->sub_data_element->sub_data_element;
     if ((GetLdr (leader, dataptr)) != SUCCESS)
     {
	  fprintf(LOGFILE, "sgml2marc: Leader conversion failed.\n");
	  return(FAILURE);
     }
     
     
     /* Get Variable Control Fields */
          /* set dataptr to point at first field in Variable control fields */
     dataptr = sgmldoc->data->sub_data_element;
     for (i = 0; i < 2; i++)
	  dataptr = dataptr->next_data_element;
     for (i = 0; i < 2; i++)
	  dataptr = dataptr->sub_data_element;
          /* Get the Variable Control Fields */
     if ((GetVCfld(directory, varflds, dataptr)) != SUCCESS)
     {
	  fprintf(LOGFILE, "sgml2marc: Variable Control Field conversion failed.\n");
	  return(FAILURE);
     }
     
     
     /* Get Variable Data Fields */
          /* set dataptr to point at first major subdivision of */
          /* variable data fields */
     dataptr = sgmldoc->data->sub_data_element;
     for (i = 0; i < 2; i++)
	  dataptr = dataptr->next_data_element;
     dataptr = dataptr->sub_data_element;
     dataptr = dataptr->next_data_element;
     dataptr = dataptr->sub_data_element;
     
          /* get the Variable Data Fields */
     if ((GetVDfld (directory, varflds, dataptr, recsize)) != SUCCESS)
     {
	  fprintf(LOGFILE, "sgml2marc: Variable Data Field conversion failed.\n");
	  return(FAILURE);
     }
     
     /* put appropriate terminators on directory and varfields string */
     fldsize = strlen(directory);
     directory[fldsize] = FT;
     fldsize = strlen(varflds);
     varflds[fldsize] = RT;
     
     /* update base address and logical record length */
     newbaseaddr = strlen(directory) + 24;
     newLRL = strlen(varflds) + newbaseaddr;
     sprintf(BAstring, "%05d", newbaseaddr);
     sprintf(LRLstring, "%05d", newLRL);
     for (i = 0; i < 5; i++)
     {
	  leader[i] = LRLstring[i];
	  leader[i + 12] = BAstring[i];
     }
     
     /* assemble final record */
     if ((sprintf(marcrec, "%s%s%s", leader, directory, varflds)) < 0)
     {
	  fprintf(LOGFILE, "sgml2marc: record assembly failed.\n");
	  return(FAILURE);
     }

     /* Free up malloc'ed memory, excepting marcrec */
     free(varflds);
     free(directory);
     
     return(SUCCESS);
}




