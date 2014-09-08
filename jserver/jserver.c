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
* Copyright (c) 1994-8 Regents of the University of California
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
/*
 *	JServer: Direct Access support for the Cheshire II search engine
 *               to handle Java client protocols via a TCP/IP connection.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(LINUX) || defined(MACOSX)
/* ignore the mode stuff */
#else
#include <sys/mode.h>
#endif
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include "connect.h"
/* note that we have to include the Z39.50 Structures, but not the library */
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"
#define STATMAIN
#include "search_stat.h"

#define BACKLOG	5
#define MAXPATHNAME 512

int     DIAG_CONDITION;      /* Diagnostic Set Conditon      */
char    *DIAG_MESSAGES[] = {
#include "diag_messages.data"
};

FILE *LOGFILE;   /* log file */

struct diagMessage {
        int     errorCode;
        char    *errorMessage;
} DIAG_MESSAGE[] = {
#include "diag_msg.data"
};

int InitializeSession (ZSESSION *session, char *initFile);

void
PrintServerUsage(FILE *file)
{
  fprintf(file, "\tUsage: jserver <-p port><-c server_init_file>\n");
  exit(1);
}

/*
 * Initialize and create new temporary directory.
 */

void
InitResultSetDir (session, uid)
ZSESSION	*session;
int	uid;	/* unique identifier. (e.g process number)	*/
{
    	char	*path;
    	sprintf(session->c_resultSetDir, "%s/%d", 
		session->s_resultSetDirectory, uid);
    	path = session->c_resultSetDir;
    	mkdir (path, 00777);
}

void
ExitResultSetDir (session)
ZSESSION	*session;
{
    	char	*path;
    	char	command[120];

    	path = session->c_resultSetDir;
    	fprintf(session->s_lfptr, "** removing directory: %s\n", path);
    	sprintf(command, "rm -rf %s\n", path);
    	system (command);
}


int 
Connection_Type() {
  struct sockaddr Name;
  int NameLen;
  struct hostent *Peer;
  
  NameLen = sizeof(struct sockaddr);

  if(!getpeername(fileno(stdout),(struct sockaddr *)&Name,&NameLen)) 
    return(CON_INETD); /* this is an inetd connection */
  else
    return(CON_COMMANDLINE); /* server started from command line */
  
}


int
main(argc, argv)
int	argc;
char	**argv;
{
  int	sfd;		/* server connection	*/
  int	cfd;		/* client connection	*/
  int	cpid;		/* child process id	*/
  int	port;		/* port number		*/
  int   portarg;
  ZSESSION	session;	
  char	pathname[MAXPATHNAME];
  char  *server_init_file_name = "server.init";
  char  *configfile_name = NULL;
  FILE *tmplogfile;
  char *current_dir, *getcwd();
  int      retval;               /* General return value. */
  int      sbufsize;
  int      len = sizeof(sbufsize);


  /* templog is only open to catch errors printed to LOGFILE before */
  /* the session is properly initialized                            */
  /* It should be world readable and writeable                      */
  tmplogfile = fopen("/tmp/templog","w+");
  chmod("/tmp/templog", S_IRWXO | S_IRWXG | S_IRWXU);
  
  portarg = 0;

  if (argc > 5)
    PrintServerUsage(tmplogfile);
  
  while (argc > 1) {
    if (strcmp("-p", argv[0]) == 0) {
      port = atoi(argv[1]);
      portarg = 1;
      argc -= 2;
      argv += 2;
    }
    else if (strcmp("-c", argv[0]) == 0) {
      server_init_file_name = argv[1];
      argc -= 2;
      argv += 2;
    }
    else if (argv[0][0] == '-') {
      PrintServerUsage(tmplogfile);
    }
    else {
      argc -= 1;
      argv += 1;
    }
  }

  memset((char *)&session, '\0', sizeof(ZSESSION));

  LOGFILE = tmplogfile;

  if (InitializeSession(&session, server_init_file_name) == Z_ERROR) { 
    fprintf(stderr, "Couldn't initialize session with init file  %s\n",
	    server_init_file_name);
    fprintf(LOGFILE, "Couldn't initialize session with init file  %s\n",
	    server_init_file_name);
    exit(1);
  }
  /*
   * Initialize the directory for result sets
   */
  
  InitResultSetDir(&session, getpid());
  
  /*
   * Open the session log file
   */      

  if (session.s_logFileName == NULL ||
      (strlen(session.s_logFileName) + 
       strlen(session.c_resultSetDir) + 2) > MAXPATHNAME)
    exit(1);
  
  if (session.s_logFileName[0] == '/') 
    sprintf(pathname,"%s", session.s_logFileName);
  else
    sprintf(pathname,"%s/%s", session.c_resultSetDir, 
	    session.s_logFileName);

  
  if ((session.s_lfptr = fopen(pathname,"w+")) == NULL) {
    fprintf(stderr, "Couldn't open log file %s/%s",
	    (session.c_resultSetDir == NULL) ? "NULL":
	    session.c_resultSetDir,
	    (session.s_logFileName == NULL)? "NULL":
	    session.s_logFileName);
    exit(1);
  }

  LOGFILE = session.s_lfptr;

  /* This version is intended for INETD or SINGLE SESSION use */

  if (session.connection_type == CON_COMMANDLINE) {

    printf("COMMANDLINE SESSION--- see %s/%s for messages\n",
	   session.c_resultSetDir, session.s_logFileName);
    if (portarg == 0)
      port = session.s_portNumber;
    fprintf(session.s_lfptr,"Server started: job= %d -- see %s/%s for messages\n",
	   getpid(), session.c_resultSetDir, session.s_logFileName);

    /* Start server at a well-know port on the local machine. 
     * When the server is up, it listens to incoming requests 
     * for connection at the well-know port.
     */
    if ((sfd = StartServer(port, BACKLOG)) < 0) {
      fprintf(session.s_lfptr, "can't start the server on port %d\n",port);
      exit(1);
    }
  

    /* Waiting for connection requests from clients.
     * No authentication is supported now.
     */ 
    if ((cfd = ConnectToClient(sfd)) < 0)
      exit(cfd);

    fprintf(session.s_lfptr,"SERVER: Client socket number: %d\n", cfd);
  
  }
  else { /* set things up for an INETD connection */
    char PeerName[500];
    int flags;

    fprintf(session.s_lfptr,"Starting INETD Session:\n");
    fflush(session.s_lfptr);

    /* When INETD starts a connection FDs 0, 1, and 2 are duped */
    /* to to FD for the incoming socket ---                     */
    cfd = 1; /* stdout */
    ClientStatus(cfd, PeerName);
    fprintf(session.s_lfptr,"Peer Name: %s\n", PeerName);

    fprintf(session.s_lfptr,"Server started job= %d -- see %s/%s for messages\n",
	   getpid(), session.c_resultSetDir, session.s_logFileName);
    fflush(session.s_lfptr);

    /* DumpSession(&session); */
  }


  /*
   * Initialize the session variable with socket info.
   */
  session.c_fd = cfd;
  
  
  /*
   * Initialize the Search Engine...
   */
  if (InitializeSearchEngine(&session) == -1) {
    fprintf(session.s_lfptr,
	    "cf_initialize failed in searchapi\n");
    fflush(session.s_lfptr);
    exit(2); 
  }
  
  /* process the client connection for this session */
  JHandleClientConnection(&session, cfd);
  
  /* When the child process is done with this client,
   * the server process goes away silently.
   */
  close(cfd);
  fclose(session.s_lfptr);
  ExitResultSetDir(&session);
  CloseSearchEngine(&session);
  exit(0);

  return(0);
}




int
DumpSession(session)
     ZSESSION *session;	      /* Current Session */
{
  int i, Result;
  char allbuffer[3000];
  int printall = 1;
  char *c;


  fprintf(session->s_lfptr,"SERVER Connection Variables:\n\
Host Name %s\n\
Host IP Address %s\n\
Port %d\n\
Socket %d\n\
Server ImplementationId %s\n\
Server ImplementationName %s\n\
Server ImplementationVersion %s\n\
Server ProtocolVersion %s\n\
Server SupportVersion1 %d\n\
Server SupportVersion2 %d\n\
Server SupportVersion3 %d\n\
Server Options %s\n\
Support Search %d\n\
Support Present %d\n\
Support DeleteResultSet %d\n\
Support ResourceReport %d\n\
Support TriggerResourceControl %d\n\
Support ResourceControl %d\n\
Support AccessControl %d\n\
Support Scan %d\n\
Support Sort %d\n\
Support Extended Services %d\n\
Support Level-1 Segmentation %d\n\
Support Level-2 Segmentation %d\n\
Support Concurrent Operations %d\n\
Support Named Result Sets %d\n\
Support Type0Query %d\n\
Support Type1Query %d\n\
Support Type2Query %d\n\
Support Type100Query %d\n\
Support Type101Query %d\n\
Support Type102Query %d\n\
Support ElementSetNames %d\n\
Support SingleElementSetName %d\n\
PreferredMessageSize %d\n\
ExceptionalRecordSize %d\n\n", 
	    (session->s_serverMachineName != NULL ?
	    session->s_serverMachineName : "None Set") , 
 	    (session->s_serverIP != NULL ? 
	    session->s_serverIP : "None Set"),
 	    session->s_portNumber,
	    session->fd, 
 	    (session->s_implementationId != NULL ? 
	    session->s_implementationId : "None Set"),
 	    (session->s_implementationName != NULL ? 
	    session->s_implementationName : "None Set"),
 	    (session->s_implementationVersion != NULL ? 
	    session->s_implementationVersion : "None Set"),
 	    (session->s_protocolVersion != NULL ? 
	    session->s_protocolVersion : "None Set"),
 	    session->s_supportVersion1,
 	    session->s_supportVersion2,
 	    session->s_supportVersion3,
 	    (session->s_options != NULL ? 
	    session->s_options : "None Set"),
	    session->s_supportSearch,
	    session->s_supportPresent,
	    session->s_supportDeleteResultSet,
	    session->s_supportResourceReport,
	    session->s_supportTriggerResourceControl,
	    session->s_supportResourceControl,
	    session->s_supportAccessControl,
	    session->s_supportScan,
	    session->s_supportSort,
	    session->s_supportExtendedServices,
	    session->s_supportLevel_1_Segmentation,
	    session->s_supportLevel_2_Segmentation,
	    session->s_supportConcurrentOperations,
	    session->s_supportNamedResultSets,
	    session->s_supportType0Query,
	    session->s_supportType1Query,
	    session->s_supportType2Query,
	    session->s_supportType100Query,
	    session->s_supportType101Query,
	    session->s_supportType102Query,
	    session->s_supportElementSetNames,
	    session->s_supportSingleElementSetName,
	    session->s_preferredMessageSize,
	    session->s_exceptionalRecordSize); 

  fflush(session->s_lfptr);

  fprintf(session->s_lfptr,"Database Names: ");
  for (c = ac_list_first(session->s_databaseNames); c != NULL; 
       c = ac_list_next(session->s_databaseNames))
    fprintf(session->s_lfptr,"%s ",c);

  fflush(session->s_lfptr);
  
  fprintf(session->s_lfptr,"\nDatabase Configfiles: ");
  for (c = ac_list_first(session->s_DBConfigFileNames); c != NULL; 
       c = ac_list_next(session->s_DBConfigFileNames))
    fprintf(session->s_lfptr,"%s ",c);

  fflush(session->s_lfptr);

  fprintf(session->s_lfptr,"\nDatabase PathNames: ");
  for (c = ac_list_first(session->s_databasePathNames); c != NULL; 
       c = ac_list_next(session->s_databasePathNames))
    fprintf(session->s_lfptr,"%s ",c);

  fflush(session->s_lfptr);

  fprintf(session->s_lfptr,"\nDatabase AccessPoints: ");
  for (c = ac_list_first(session->s_accessPoints); c != NULL; 
       c = ac_list_next(session->s_accessPoints))
    fprintf(session->s_lfptr,"%s ",c);

  fflush(session->s_lfptr);

    fprintf(session->s_lfptr,"\nCLIENT Connection Variables:\n\
Host %s\n\
Client IP %s\n\
Client Socket %d\n\
Client ImplementationId %s\n\
Client ImplementationName %s\n\
Client ImplementationVersion %s\n\
Client ProtocolVersion %s\n\
Client SupportVersion1 %d\n\
Client SupportVersion2 %d\n\
Client SupportVersion3 %d\n\
Client Options %s\n\
Support Search %d\n\
Support Present %d\n\
Support DeleteResultSet %d\n\
Support ResourceReport %d\n\
Support TriggerResourceControl %d\n\
Support ResourceControl %d\n\
Support AccessControl %d\n\
Timeout %d\n\
PreferredMessageSize %d\n\
ExceptionalRecordSize %d\n\n",
 	    (session->c_clientMachineName != NULL ?
	     session->c_clientMachineName : "None Set") , 
            (session->c_clientIP != NULL ? 
	     session->c_clientIP : "None Set"),
            session->c_fd, 
            (session->c_implementationId != NULL ? 
	     session->c_implementationId : "None Set"),
            (session->c_implementationName != NULL ? 
	     session->c_implementationName : "None Set"),
            (session->c_implementationVersion != NULL ? 
	     session->c_implementationVersion : "None Set"),
            (session->c_protocolVersion != NULL ?
	     session->c_protocolVersion : "None Set"),
            session->c_supportVersion1,
            session->c_supportVersion2,
            session->c_supportVersion3,
            (session->c_options != NULL ? 
	     session->c_options : "None Set"),
	    session->c_requestSearch,
	    session->c_requestPresent,
	    session->c_requestDeleteResultSet,
	    session->c_requestResourceReport,
	    session->c_requestTriggerResourceControl,
	    session->c_requestResourceControl,
	    session->c_requestAccessControl,
            session->c_timeOut,
            session->c_preferredMessageSize,
            session->c_exceptionalRecordSize); 

  fflush(session->s_lfptr);

    fprintf(session->s_lfptr,"SEARCH Variables: \n\
SmallSetUpperBound %d\n\
LargeSetLowerBound %d\n\
MediumSetPresentNumber %d\n\
ReplaceIndicator %d\n\
ResultSetName %s\n\
Database %s\n\
SmallSetElementSetNames %s\n\
MediumSetElementSetNames %s\n\
PreferredRecordSyntax %s\n\
Last Query %s\n\
Last AttributeSet %s\n\
Last Search Status %d (0=failure 1=success)\n\
Last Search Result Set Status %d (0=Not appl., 1=subset 2=interim 3=none) \n\
Last Search Present Status %d \n\
Last Search ResultCount %d\n\
Last Search RecordsReturned %d\n\
Last Search Next ResultSet Position %d\n\n",

	    session->c_smallSetUpperBound,
	    session->c_largeSetLowerBound,
	    session->c_mediumSetPresentNumber,
	    session->replaceIndicator, 
            session->resultSetName,
            session->databaseName,
            session->presentElementSet, /* this isn't correct, but..*/
            session->presentElementSet, 
            session->preferredRecordSyntax ? 
            session->preferredRecordSyntax : "None Set",
	    session->queryString ?
	    session->queryString : "No Query",
	    session->c_attributeSetId,
	    session->searchStatus,
	    session->resultSetStatus,
	    session->presentStatus,
	    session->resultCount,
	    session->noRecordsReturned,
	    session->nextResultSetPosition);

  fflush(session->s_lfptr);

    fprintf(session->s_lfptr,"PRESENT Variables:\n\
ResultSet Name %s\n\
ResultSetStartPoint %d\n\
NumberOfRecordsRequested %d\n\
ElementSetNames %s\n\
preferredRecordSyntax(recsyntax) %s\n\
NumberOfRecordsReturned (records) %d\n\
NextResultSetPosition (nextrec) %d\n\
Total Records Returned (totrecords) %d\n\n",
	    session->resultSetName, 
	    session->nextResultSetPosition,
	    session->numberOfRecordsRequested, 
	    session->presentElementSet,
            session->preferredRecordSyntax ? 
            session->preferredRecordSyntax : "None Set",
	    session->noRecordsReturned, 
	    session->nextResultSetPosition,
	    session->totalNumberRecordsReturned);

  fflush(session->s_lfptr);
}


