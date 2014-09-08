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
*	Header Name:	cf_initialize.c
*
*	Programmer:	Ray R. Larson
*
*	Purpose:	The routines in this module load the configfile
*                       for the CHESHIRE II system and provide file 
*                       opening and closing by name or in a batch.
*	Usage:		Used whenever a database file needs to be accessed
*                       in place of direct system fopen or open
*	Variables:	see cheshire.h
*
*	Return Conditions and Return Codes:	
*
*	Date:		10/10/93
*	Revised:	
*	Version:	1.0
*	Copyright (c) 1993.  The Regents of the University of California
*		All Rights Reserved
*
************************************************************************/
#include <stdlib.h>
#include "cheshire.h"
/* #include "configfiles.h" not needed and includes the extern def */
/* for the following flags -- these are set here                   */
char cf_create_file_flags[4];
char cf_open_file_flags[4];

int db_init(char *home, DB_ENV **dbenvp);

config_file_info *cf_initialize(char *cf_name, char *create_f, char *open_f) 
{
	extern FILE *cf_in;
	extern config_file_info *cf_info_base, *base_cf_info;

	cf_in = fopen(cf_name,"r");

  	if (cf_in == NULL) {

	  fprintf(LOGFILE, "Configuration file");

	  if (cf_name)
	    fprintf(LOGFILE, " %s", cf_name);

          fprintf(LOGFILE, " not found.\n");
	  return (NULL);
	}

	if (cf_info_base != NULL) 
	  cf_restart_scanner(cf_in);

  	/* parse the configuration input file */
  	CFparse();
   
	fclose (cf_in);

	if (create_f) 
	  cf_set_create_flags(create_f);
	else
	  cf_set_create_flags("r");

	if (open_f) 
	  cf_set_open_flags(open_f);
	else
	  cf_set_open_flags("r");

	/* initialize the DB environment */
	cf_init_db_env(base_cf_info);

	return (base_cf_info);
}

cf_init_db_env(config_file_info *config) {
  config_file_info *cf;
  int first = 1;
  char *home;

  /* if the environment variable is set, it overides anything in the
   * config file, so just return -- the environment will be opened
   * during index file initialization using the environment var.
   */
  if ((home = getenv("CHESHIRE_DB_HOME")) != NULL)
      return;

  if (config == NULL) 
    return;

  /* otherwise, check for DBENV tags in the config files(s)
   */
  if (gb_dbenv == NULL) {
	
    for (cf = config; cf; cf = cf->next_file_info) {
      if (cf->dbenv_path) {
	db_init(cf->dbenv_path, &gb_dbenv);
	break;
      }
    }
  }
  
}

cf_set_create_flags(char *flags)
{
  if (flags != NULL)
    strncpy(cf_create_file_flags, flags, 3);
}

cf_set_open_flags(char *flags)
{
  if (flags != NULL)
    strncpy(cf_open_file_flags, flags, 3);
}



