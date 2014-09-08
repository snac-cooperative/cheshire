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
/**************************************************************************
* 
* Cheshire II Online Catalog System
* 
* Copyright (c) 1994-7 Regents of the University of California
* 
* All rights reserved.
*
* Authors:      Ray Larson, ray@sherlock.berkeley.edu
*               Jerome McDonough, jmcd@lucien.sims.berkeley.edu
*               Aitao Chen,  aitao@info.sims.berkeley.edu
*               Lucy Kuntz,  lkuntz@info.sims.berkeley.edu
*               Ralph Moon,  rmoon@library.berkeley.edu
*
*		School of Information Management and Systems, UC Berkeley
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*	This product includes software developed by the University of
*	California, Berkeley.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND THE AUTHOR ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifndef WIN32
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#ifdef WIN32
#define strcasecmp _stricmp
#endif
#include "z3950_3.h"

#include "session.h"
#include "z3950_intern.h"

/*
 *	The field names used in server initialization file must
 *	be declared here. When you add a field name to 
 *	SESSION_FIELD_NAMES, make sure you also add an entry
 *	to the static array 'names'.
 */	

enum SESSION_FIELD_NAMES 	{
	PREFERRED_MESSAGE_SIZE,
	MAXIMUM_RECORD_SIZE,
	IMPLEMENTATION_ID,
	IMPLEMENTATION_NAME,
	IMPLEMENTATION_VERSION,
	PROTOCOL_VERSION,
	OPTIONS,
	PORT,
	DATABASE_NAMES,
	DATABASE_DIRECTORIES,
	DATABASE_CONFIGFILES,
	SUPPORT_NAMED_RESULT_SET,
	SUPPORT_MULTIPLE_DATABASE_SEARCH,
	MAXIMUM_NUMBER_DATABASES,
	MAXIMUM_NUMBER_RESULT_SETS,
	NO_DISK_RESULT_SETS,
	TIMEOUT,
	LOG_FILE_NAME,
	PERM_LOG_FILE_NAME,
	RESULT_SET_DIRECTORY,
	ATTRIBUTE_SET_ID,
	SUPPORT_TYPE_0_QUERY,
	SUPPORT_TYPE_1_QUERY,
	SUPPORT_TYPE_2_QUERY,
	SUPPORT_TYPE_100_QUERY,
	SUPPORT_TYPE_101_QUERY,
	SUPPORT_TYPE_102_QUERY,
	SUPPORT_ELEMENT_SET_NAMES,
	SUPPORT_SINGLE_ELEMENT_SET_NAME,
	TARGETINFO_NAME,
	TARGETINFO_NEWS,
	TARGETINFO_WELCOME_MSG,
	TARGETINFO_CONTACT_NAME,
	TARGETINFO_CONTACT_DESCRIPTION,
	TARGETINFO_CONTACT_ADDRESS,
	TARGETINFO_CONTACT_EMAIL,
	TARGETINFO_CONTACT_PHONE,
	TARGETINFO_DESCRIPTION,
	TARGETINFO_USAGE_RESTRICTION,
	TARGETINFO_PAYMENT_ADDRESS,
	TARGETINFO_LANGUAGES,
	TARGETINFO_HOURS
};

static	struct names {
  int	code;
  char	name[50];
  
} names[]= {
  PREFERRED_MESSAGE_SIZE,	"PREFERRED_MESSAGE_SIZE",
  MAXIMUM_RECORD_SIZE,	"MAXIMUM_RECORD_SIZE",
  IMPLEMENTATION_ID,	"IMPLEMENTATION_ID",
  IMPLEMENTATION_NAME,	"IMPLEMENTATION_NAME",
  IMPLEMENTATION_VERSION,	"IMPLEMENTATION_VERSION",
  PROTOCOL_VERSION,	"PROTOCOL_VERSION",
  OPTIONS,		"OPTIONS",
  PORT, "PORT",
  DATABASE_NAMES,		"DATABASE_NAMES",
  DATABASE_DIRECTORIES,	"DATABASE_DIRECTORIES",
  DATABASE_CONFIGFILES,	"DATABASE_CONFIGFILES",
  SUPPORT_NAMED_RESULT_SET,"SUPPORT_NAMED_RESULT_SET",
  SUPPORT_MULTIPLE_DATABASE_SEARCH,"SUPPORT_MULTIPLE_DATABASE_SEARCH",
  MAXIMUM_NUMBER_DATABASES,"MAXIMUM_NUMBER_DATABASES",
  MAXIMUM_NUMBER_RESULT_SETS,"MAXIMUM_NUMBER_RESULT_SETS",
  NO_DISK_RESULT_SETS, "NO_DISK_RESULT_SETS",
  TIMEOUT, "TIMEOUT",
  LOG_FILE_NAME, "LOG_FILE_NAME",
  PERM_LOG_FILE_NAME, "PERMANENT_LOG_FILE_NAME",
  RESULT_SET_DIRECTORY, "RESULT_SET_DIRECTORY",
  ATTRIBUTE_SET_ID, "ATTRIBUTE_SET_ID",
  SUPPORT_TYPE_0_QUERY, "SUPPORT_TYPE_0_QUERY",
  SUPPORT_TYPE_1_QUERY, "SUPPORT_TYPE_1_QUERY",
  SUPPORT_TYPE_2_QUERY, "SUPPORT_TYPE_2_QUERY",
  SUPPORT_TYPE_100_QUERY, "SUPPORT_TYPE_100_QUERY",
  SUPPORT_TYPE_101_QUERY, "SUPPORT_TYPE_101_QUERY",
  SUPPORT_TYPE_102_QUERY, "SUPPORT_TYPE_102_QUERY",
  SUPPORT_ELEMENT_SET_NAMES, "SUPPORT_ELEMENT_SET_NAMES",
  SUPPORT_SINGLE_ELEMENT_SET_NAME, "SUPPORT_SINGLE_ELEMENT_SET_NAME",
  TARGETINFO_NAME,  "TARGETINFO_NAME",
  TARGETINFO_NEWS,  "TARGETINFO_NEWS",
  TARGETINFO_WELCOME_MSG,  "TARGETINFO_WELCOME_MSG",
  TARGETINFO_CONTACT_NAME,  "TARGETINFO_CONTACT_NAME",
  TARGETINFO_CONTACT_DESCRIPTION,  "TARGETINFO_CONTACT_DESCRIPTION",
  TARGETINFO_CONTACT_ADDRESS,  "TARGETINFO_CONTACT_ADDRESS",
  TARGETINFO_CONTACT_EMAIL,  "TARGETINFO_CONTACT_EMAIL",
  TARGETINFO_CONTACT_PHONE,  "TARGETINFO_CONTACT_PHONE",
  TARGETINFO_DESCRIPTION,  "TARGETINFO_DESCRIPTION",
  TARGETINFO_USAGE_RESTRICTION,  "TARGETINFO_USAGE_RESTRICTION",
  TARGETINFO_PAYMENT_ADDRESS,  "TARGETINFO_PAYMENT_ADDRESS",
  TARGETINFO_LANGUAGES,  "TARGETINFO_LANGUAGES",
  TARGETINFO_HOURS,  "TARGETINFO_HOURS"
};

int Connection_Type();

/*
 *	Return -1 if not in the list, otherwise return the
 *	index in the list.
 */
int
NameLookup(name)
char	*name;
{
    int	i;
    int count = sizeof(names) / sizeof(struct names);

    for (i=0; i<count; i++)
	if (strcasecmp(name, names[i].name) == 0)
	    return i;
    return -1;
}

/*
 *	Extract the name and value from a buf.
 *	Return 0 if this is a comment line, otherwise return 1.
 */
int
ParseNameValue(buf, name, value)
char	*buf;
char	*name;
char	*value;
{
    char	*cptr, *eptr;
    char	*nptr, *vptr;

        cptr = buf;
	eptr = buf + strlen(buf);

        /* remove blank spaces.	
	 */
	while ((cptr < eptr) && (isspace(*cptr)))
	    cptr++; 

	if ((cptr >= eptr) || (*cptr == '#'))
	    return 0;

	/*
 	 * read the field name.
	 */
	nptr = name;
	while ((cptr < eptr) && 
	       (isalnum(*cptr) || (*cptr == '_') || (*cptr == '-'))) 
	    *nptr++ = *cptr++;
	*nptr = '\0';

	/*
	 * read the value.
	 */
	while ((cptr < eptr) && (isspace(*cptr)))
	    cptr++; 

	vptr = value;
	if (*cptr == '"') {
	    cptr++;
	    while ((cptr < eptr) && (*cptr != '"'))
		*vptr++ = *cptr++;
	} else {
	    while ((cptr < eptr) && 
                   (isalnum(*cptr) || (*cptr == '|')))
                *vptr++ = *cptr++;
	}	
	*vptr = '\0';
	
	return 1;
}


int
InitializeSession(session, initFileName)
ZSESSION	*session;
char	*initFileName;
{
  FILE	*fptr;
  char	name[80];
  char	value[100000];
  char	buf[100000];
  char        tmpbuffer[100000];
  char	**databaseNames;
  char	**databaseConfigNames;
  char	**databasePathNames;
  char	**attribute_set_ids;
  char  **languages;
  int i;
  int dbflag = 0;
  int dirflag = 0;
  extern FILE *LOGFILE;
  struct hostent *hostinfo;
  char hostname[200];
  struct in_addr in_ad;

  if ((fptr = fopen(initFileName, "r")) == NULL) {
    if (LOGFILE)
      fprintf(LOGFILE, "can't open server initialization file: %s\n",initFileName);
    return Z_ERROR;
  }

  /* structure to hold explain string information */
  session->s_explainstrings = CALLOC(ExplainStrings, 1);

  session->connection_type = Connection_Type();

  while (!feof(fptr)) {
    memset(buf, '\0', sizeof(buf));
    fgets(buf, sizeof(buf), fptr);
    memset(name, '\0', sizeof(name));
    memset(value, '\0', sizeof(value));
    if (ParseNameValue(buf, name, value) == 0)
      continue;
    
    switch (NameLookup(name)) {
    case PREFERRED_MESSAGE_SIZE:
      session->s_preferredMessageSize = atoi(value);
      break;
    case MAXIMUM_RECORD_SIZE:
      session->s_exceptionalRecordSize = atoi(value);
      break;
    case IMPLEMENTATION_ID:
      session->s_implementationId = (char *)strdup(value);
      break;
    case IMPLEMENTATION_NAME:
      session->s_implementationName = (char *)strdup(value);
      break;
    case IMPLEMENTATION_VERSION:
      session->s_implementationVersion = (char *)strdup(value);
      break;
    case PROTOCOL_VERSION:
      session->s_protocolVersion = (char *)strdup(value);
      break;
    case OPTIONS:
      session->s_options = (char *)strdup(value);
      break;
    case PORT:
      session->s_portNumber = atoi(value);
      break;
    case DATABASE_NAMES:
      /* Note -- the database names must be unique -- and must match */
      /* the filetag names in the config files                       */
      /* This is no longer used in normal server init files          */
      session->s_databaseNames = ac_list_alloc();
      databaseNames = StringToArgv(value);
      for (i=0; databaseNames[i] != NULL; i++)
	ac_list_add(session->s_databaseNames, databaseNames[i]);
      /*
       * FreeSpace (databaseNames);
       */
      break;
    case DATABASE_DIRECTORIES:
      /* This is no longer used in normal server init files          */
      session->s_databasePathNames = ac_list_alloc();
      databasePathNames = StringToArgv(value);
      dirflag = 1;
      for (i=0; databasePathNames[i] != NULL; i++)
	ac_list_add(session->s_databasePathNames, 
		 databasePathNames[i]);
      break;
    case DATABASE_CONFIGFILES:
      /* NOTE: At initialization all of the configfiles are read into */
      /* the server -- this means that names must be unique or odd    */
      /* problems (like thinking you are searching one db when really */
      /* searching another) will occur                                */
      session->s_DBConfigFileNames = ac_list_alloc();
      databaseConfigNames = StringToArgv(value);
      dbflag = 1;
      /* the configfiles should have full path names, or be in the   */
      /* matching (order-wise) database directory                    */
      for (i=0; databaseConfigNames[i] != NULL; i++) {
	if (databaseConfigNames[i][0] != '/')
	  /* USE OF THIS FORM IS DEPRECATED */
	  sprintf(tmpbuffer, "%s/%s", 
		  ac_list_nth(session->s_databasePathNames, i),
		  databaseConfigNames[i]);
	else
	  /* Config file names should be complete path name! */
	  sprintf(tmpbuffer, "%s", databaseConfigNames[i]);
	
	ac_list_add(session->s_DBConfigFileNames, tmpbuffer);
      }
      break;
    case SUPPORT_NAMED_RESULT_SET:
      session->namedResultSet	= atoi(value);
      break;
    case SUPPORT_MULTIPLE_DATABASE_SEARCH:
      session->multipleDatabasesSearch = atoi(value);
      break;
      
    case MAXIMUM_NUMBER_DATABASES:
      session->maximumNumberDatabases = atoi(value);
      break;
    case MAXIMUM_NUMBER_RESULT_SETS:
      session->maximumNumberResultSets = atoi(value);
      break;
    case NO_DISK_RESULT_SETS:
      if (atoi(value) || strcasecmp(value,"TRUE") == 0)
	session->disable_saving_result_sets = 4;
      break;
    case TIMEOUT:
      session->s_timeOut	= atoi(value);
      break;
    case LOG_FILE_NAME:
      session->s_logFileName	= (char *)strdup(value);
      break;
    case PERM_LOG_FILE_NAME:
      session->s_permLogFileName = (char *)strdup(value);
      break;
    case RESULT_SET_DIRECTORY:
      session->s_resultSetDirectory = (char *)strdup(value);
      break;
    case ATTRIBUTE_SET_ID:
      session->s_attributeSetIds = ac_list_alloc();
      
      attribute_set_ids = StringToArgv(value);
      for (i=0; attribute_set_ids[i] != NULL; i++)
	ac_list_add(session->s_attributeSetIds, 
		 attribute_set_ids[i]);
      break;
    case SUPPORT_TYPE_0_QUERY:
      session->s_supportType0Query = atoi(value);
      break;
    case SUPPORT_TYPE_1_QUERY:
      session->s_supportType1Query = atoi(value);
      break;
    case SUPPORT_TYPE_2_QUERY:
      session->s_supportType2Query = atoi(value);
      break;
    case SUPPORT_TYPE_100_QUERY:
      session->s_supportType100Query = atoi(value);
      break;
    case SUPPORT_TYPE_101_QUERY:
      session->s_supportType101Query = atoi(value);
      break;
    case SUPPORT_TYPE_102_QUERY:
      session->s_supportType102Query = atoi(value);
      break;
    case SUPPORT_ELEMENT_SET_NAMES:
      session->s_supportElementSetNames = atoi(value);
      break;
    case SUPPORT_SINGLE_ELEMENT_SET_NAME:
      session->s_supportSingleElementSetName = atoi(value);
      break;

    case TARGETINFO_NAME:
      session->s_explainstrings->targetinfo_name = strdup(value);
      break;

    case TARGETINFO_NEWS:
      session->s_explainstrings->targetinfo_news = strdup(value);
      break;

    case TARGETINFO_WELCOME_MSG:
      session->s_explainstrings->targetinfo_welcome_msg = strdup(value);
      break;

    case TARGETINFO_CONTACT_NAME:
      session->s_explainstrings->targetinfo_contact_name = strdup(value);
      break;

    case TARGETINFO_CONTACT_DESCRIPTION:
      session->s_explainstrings->targetinfo_contact_description = strdup(value);
      break;

    case TARGETINFO_CONTACT_ADDRESS:
      session->s_explainstrings->targetinfo_contact_address = strdup(value);
      break;

    case TARGETINFO_CONTACT_EMAIL:
      session->s_explainstrings->targetinfo_contact_email = strdup(value);
      break;

    case TARGETINFO_CONTACT_PHONE:
      session->s_explainstrings->targetinfo_contact_phone = strdup(value);
      break;

    case TARGETINFO_DESCRIPTION:
      session->s_explainstrings->targetinfo_description = strdup(value);
      break;

    case TARGETINFO_USAGE_RESTRICTION:
      session->s_explainstrings->targetinfo_usage_restriction = strdup(value);
      break;

    case TARGETINFO_PAYMENT_ADDRESS:
      session->s_explainstrings->targetinfo_payment_address = strdup(value);
      break;

    case TARGETINFO_HOURS:
      session->s_explainstrings->targetinfo_hours = strdup(value);
      break;

    case TARGETINFO_LANGUAGES:
      session->s_explainstrings->targetinfo_languages = ac_list_alloc();
      languages = StringToArgv(value);
      for (i=0; languages[i] != NULL; i++)
	ac_list_add(session->s_explainstrings->targetinfo_languages,languages[i]);
      break;

    default:
      if (LOGFILE)
	fprintf(LOGFILE, "invalid name: %s\n", name);
      else
	fprintf(stderr, "invalid name: %s\n", name);
      break;
    }
  }
  fclose(fptr);

  if (dirflag == 0 && dbflag == 0) {
    if (LOGFILE)
      fprintf(LOGFILE, "Invalid server initialization file: %s\n",initFileName);
    else 
      fprintf(stderr, "Invalid server initialization file: %s\n",initFileName);
    return Z_ERROR;
  }


  /*
   *	A few more fields need to be initialized.
   */

  gethostname(hostname, 200);
  hostinfo = gethostbyname(hostname);


  /* server machine name.	*/
  session->s_serverMachineName = strdup(hostinfo->h_name); 
  /* ip address in dot format	*/
  in_ad.s_addr = *(u_int *)hostinfo->h_addr_list[0];
  session->s_serverIP = strdup(inet_ntoa(in_ad));		

  session->searchResult = (SEARCH_RESULT *)NULL;
  session->s_pid	= (int)getpid();
  session->c_resultSetNames = ac_list_alloc();

  return Z_OK;
}

