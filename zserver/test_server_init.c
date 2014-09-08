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
/*
 *	AN IMPLEMENTATION OF Z3950 SERVER
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include "connect.h"
#include "z3950_3.h"
#include "session.h"
#include "z_parameters.h"

int InitializeSession (ZSESSION *session, char *initFile);

int
Connection_Type () /* this is a dummy -- no real connections */
{
 return 1;
}

void
PrintServerUsage()
{
    	printf("\tUsage: zcserver -p port\n");
    	exit(1);
}


/*
 * Initialize and create n new_var temporary directory.
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
    	fprintf(stderr, "** making directory: %s\n", path);
    	mkdir (path, 00777);
}

void
ExitResultSetDir (session)
ZSESSION	*session;
{
    	char	*path;
    	char	command[120];

    	path = session->c_resultSetDir;
    	fprintf(stderr, "removing directory: %s\n", path);
    	sprintf(command, "rm -rf %s\n", path);
    	system (command);
}

int
ShowSession(session)
     ZSESSION *session;	      /* Current Session */
{
  int i, Result;
  char allbuffer[3000];
  int printall = 1;
  char *c;


  printf("SERVER Connection Variables:\n\
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
  printf("Database Names: ");
  for (c = list_first(session->s_databaseNames); c != NULL; 
       c = list_next(session->s_databaseNames))
    printf("%s ",c);

  
  printf("\nDatabase Configfiles: ");
  for (c = list_first(session->s_DBConfigFileNames); c != NULL; 
       c = list_next(session->s_DBConfigFileNames))
    printf("%s ",c);

  printf("\nDatabase PathNames: ");
  for (c = list_first(session->s_databasePathNames); c != NULL; 
       c = list_next(session->s_databasePathNames))
    printf("%s ",c);

  printf("\nDatabase AccessPoints: ");
  for (c = list_first(session->s_accessPoints); c != NULL; 
       c = list_next(session->s_accessPoints))
    printf("%s ",c);

    printf("\nCLIENT Connection Variables:\n\
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

    printf("SEARCH Variables: \n\
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

    printf("PRESENT Variables:\n\
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

}



#ifdef NEEDTHISREALLY

DOCTYPE
GetTypeExternal (ex)
External *ex;
{
  char *OIDstring, *GetOidStr();
  DOCTYPE result;

  if (ex == NULL)
    return ((DOCTYPE)0);

  if (ex->directReference) {
    OIDstring = GetOidStr(ex->directReference);
    if (OIDstring == NULL)
      return ((DOCTYPE)0);
      
    if (strcmp(OIDstring, MARCRECSYNTAX)==0)
      result = Z_US_MARC;		/* USMARC record	  */
    else if (strcmp(OIDstring,OPACRECSYNTAX)==0)
      result = Z_OPAC_REC;             /* OPAC record syntax     */
    else if (strcmp(OIDstring,SUTRECSYNTAX)==0)
      result = Z_SUTRS_REC;            /* SUTRS record syntax    */
    else if (strcmp(OIDstring,EXPLAINRECSYNTAX)==0)
      result = Z_EXPLAIN_REC;          /* EXPLAIN record syntax  */
    else if (strcmp(OIDstring,GRS0RECSYNTAX)==0)
      result = Z_GRS_0_REC;            /* Generic record syntax 0*/
    else if (strcmp(OIDstring,GRS1RECSYNTAX)==0)
      result = Z_GRS_1_REC;            /* Generic record syntax 1*/
    else if (strcmp(OIDstring,ESRECSYNTAX)==0)
      result = Z_ES_REC;               /* Extended Services      */
    else if (strcmp(OIDstring,SUMMARYRECSYNTAX)==0)
      result = Z_SUMMARY_REC;          /* Summary record syntax  */
    /* there may be other formats, but not in the standard ...    */
    else
      result = Z_OTHER_MARC;		/* Other MARC formats */

    free(OIDstring);
    return result;
  }
  else 
    return (Z_US_MARC); /* default format */

  
}

#endif

FILE *LOGFILE;

void
main(argc, argv)
int	argc;
char	**argv;
{
  
  ZSESSION	*session;	

  LOGFILE = stderr;

  session = calloc(1, sizeof(ZSESSION));

  InitializeSession(session, "server.init");

  InitResultSetDir(session, getpid());
  
  ShowSession(session);
  
  
  exit(0);	
}



