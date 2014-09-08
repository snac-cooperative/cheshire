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
#ifndef	SESSION_H
#define	SESSION_H

#include <stdio.h>
#ifdef LINUX
#include <sys/select.h>
#endif
#include "zprimitive.h"
#include "list.h"

#ifdef WIN32
#include <winsock.h>
#endif

#define	SESSION_OK	1
#define	SESSION_ERROR	-1

#define	MAX_RSN_LENGTH	200	/* Maximum result set name length.	*/
#define MAX_EM_LENGTH	500	/* Maximum error message length.	*/
#define MAX_DIR_LENGTH	500	/* Maximum directory path name.		*/
#define MAX_NAME_LENGTH 200
#define MAX_NUM_DBNAMES	10
#define CON_INETD 0
#define CON_COMMANDLINE 1

typedef struct searchPartResult {
	char	databaseName[MAX_NAME_LENGTH];
	int	startPosition;		/* starting position in result set */
	int	numberRecords;		/* number of records from this db. */
} SEARCH_PART_RESULT;

typedef struct searchResult {
	AC_LIST	*resultSet;		/* current search result.	*/
	int	numberRecords;		/* total number of records.	*/
	int	numberDatabases;	/* number of databases.		*/	
	SEARCH_PART_RESULT	partResults[MAX_NUM_DBNAMES];
} SEARCH_RESULT;

typedef struct recordLists RECORD_LISTS;
typedef struct RECS RECS;

typedef struct explainstrings {
  char *targetinfo_name;
  char *targetinfo_news;
  char *targetinfo_welcome_msg;
  char *targetinfo_contact_name;
  char *targetinfo_contact_description;
  char *targetinfo_contact_address;
  char *targetinfo_contact_email;
  char *targetinfo_contact_phone;
  char *targetinfo_description;
  char *targetinfo_usage_restriction;
  char *targetinfo_payment_address;
  char *targetinfo_hours;
  AC_LIST *targetinfo_languages;
} ExplainStrings;

struct RECS {
  RECS *next;
  enum {rec_type_list, rec_type_asn1
  } which;
    union {
      AC_LIST *list;
      any  ASN1;
    } u;
};

struct recordLists {
    RECORD_LISTS *next;
    char	databaseName[MAX_NAME_LENGTH];
    int		numRecords;
    RECS	*records;
};

typedef struct search_result_object {
    int	searchStatus;
    int resultSetStatus;
    int presentStatus;
    int resultCount;		/* # of records retrieved */
    int noRecordsReturned;	/* # of records returned 	*/
    int nextResultSetPosition;
    char *resultSetName;	/* the result set name at server side.	*/
    NamePlusRecord **records;	/* list of records returned.		*/
    int	diagRecPresent; 	/* 1 if a diag record returned, 0 otherwise. */
    DiagRec *diagRec;
} SEARCH_RESULT_OBJECT;

typedef struct element_sets {
	char	databaseName[MAX_NAME_LENGTH];
	char	defaultElementSetName[MAX_NAME_LENGTH];
	char	**elementSetNames;
	struct element_sets	*next;
} ELEMENT_SETS;

typedef struct result_set {
	struct result_set *next;
	int	counts;		/* no of records in this result set	*/
	int	status;
	char	resultSetName[MAX_NAME_LENGTH];
} RESULT_SET;	

typedef	enum {
	Z_DIAG_REC,		/* diagnostic record	  */
	Z_US_MARC,		/* USMARC record	  */
	Z_OPAC_REC,             /* OPAC record syntax     */
	Z_SUTRS_REC,            /* SUTRS record syntax    */
	Z_SGML_REC,             /* SGML record syntax     */
	Z_XML_REC,              /* XML record syntax      */
	Z_EXPLAIN_REC,          /* EXPLAIN record syntax  */
	Z_GRS_0_REC,            /* Generic record syntax 0*/
	Z_GRS_1_REC,            /* Generic record syntax 1*/
	Z_ES_REC,               /* Extended Services      */
	Z_SUMMARY_REC,          /* Summary record syntax  */
	Z_TEXT,			/* text document	  */
	Z_IMAGE, 		/* image document	  */
	Z_OTHER_MARC		/* other MARC types 	  */	
} DOCTYPE;

typedef	struct z_document {
	DOCTYPE	dtype;				/* document type.	*/
	char	databaseName[MAX_NAME_LENGTH];	/* dbname this doc belongs to */
	void	*data;				/* document itself	*/
	int	dsize;				/* document size in byte*/
	int     docid;				/* other info		*/
} DOCUMENT;

typedef enum {
	Z_CLOSE = 0, 
      	Z_OPEN
} ZA_STATUS;		/* z-association status	*/

typedef	enum {
  S_CLOSE=0,
  S_OPEN,
  S_INIT_SENT,
  S_INIT_RECEIVED,
  S_SEARCH_SENT,
  S_SEARCH_RECEIVED,
  S_PRESENT_SENT,
  S_PRESENT_RECEIVED,
  S_DELETE_SENT,
  S_DELETE_RECEIVED,
  S_RESOURCE_CONTROL_SENT,
  S_RESOURCE_CONTROL_RECEIVED,
  S_RESOURCE_REPORT_SENT,
  S_RESOURCE_REPORT_RECEIVED,
  S_ACCESS_CONTROL_SENT,
  S_ACCESS_CONTROL_RECEIVED,
  S_TRIGGER_RESOURCE_CONTROL_SENT,
  S_TRIGGER_RESOURCE_CONTROL_RECEIVED,
  S_SORT_RECEIVED,
  S_SORT_SENT,
  S_SCAN_RECEIVED,
  S_SCAN_SENT,
  S_CLOSE_RECEIVED,
  S_CLOSE_SENT,
  S_EXTENDED_SERVICES_RECEIVED,
  S_EXTENDED_SERVICES_SENT
} Z_STATE;

/*
 * There is one zsession record for each association and the
 * zsession records are linked together.
 */
typedef struct zsession {
  struct zsession	*next;
  struct zsession *prev;
  
  /* Information about an association.
   */
  int	status;				/* Either Z_OK or Z_ERROR.*/
  int   connection_type;                /* CON_INETD or CON_COMMANDLINE */
  int	fd;				/* socket file descriptor. */
  Z_STATE	state;
  ZA_STATUS connectionStatus;		/* Z_CLOSE, Z_OPEN	*/
  int	protocolVersion;		/* 1, 2 or 3 		*/
  int	preferredMessageSize;		/* choose the one of server */
  char    *preferredRecordSyntax;         /* set to constants in
					   *  z_parameters.h for
					   *  search or present
					   */
  int	exceptionalRecordSize;		/* choose the smaller one */
  int	errorCode;		
  char	errorMessage[MAX_EM_LENGTH];	/* Error message.	*/
  char	errorAddInfo[MAX_EM_LENGTH];	/* Additional information.*/
  char	databaseName[MAX_NAME_LENGTH];
  char	*queryString;		/* the last search query */
  int	queryType;
  ZBOOL	replaceIndicator;
  ZBOOL	parallelSearch;		/* involved in a parallel search? */
  int	startPosition;
  int	numberOfRecordsRequested;
  ZBOOL	deleteAllResultSets;
  char	**resultSetList;	/* list of result set names to be 
				 * delete from the server. */
  int	numberResultSets;	/* no of result sets in the list */
  
  RESULT_SET *s_resultSets;	/* list of result sets created during
				 * previous searches. 	*/
  int	s_numberResultSets;
  
  AC_LIST	*c_resultSetNames;	/* list of result set names that the
				 * server maintains for this session.
				 */
  void	*lastInPDU;		/* the last incoming pdu	*/
  int	lastInPDUSize;		/* size in bytes		*/
  void	*lastOutPDU;		/* the last outgoing pdu	*/
  int	lastOutPDUSize;		/* size in bytes		*/
  
  /* The latest search result.
   */
  int	searchStatus;		/* TRUE = success, FALSE = failure.
				 * a search must be done successfully
				 * before a present can be sent to 
				 * a server.
				 */
  char	SearchAddInfo[MAX_EM_LENGTH];	/* Additional information.*/
  int	SearchAddInfoType;	/* Additional information type (OID or CHAR).*/
  int	resultSetStatus;	/* 1=subset, 2=interim, 3=none 	*/
  int	presentStatus;		/* TRUE=records incl.,FALSE=otherwise */
  char	presentElementSet[MAX_NAME_LENGTH];
  
  int	noRecordsReturned;	/* # of records returned.	*/
  int	nextResultSetPosition;	/* next result set position.	*/
  int	resultCount;		/* Number of records retrieved	*/
  int	totalNumberRecordsReturned;
  /* no of records that have been 
     returned so far.*/
  DOCUMENT **documentList;	/* list of document pointers	*/
  int	listSize;		/* how many entries allocated.	*/
  int	listUse;		/* no of entries filled out.	*/
  ZBOOL	diagRecPresent;		/* TRUE = yes, FALSE = no.	*/
  DefaultDiagFormat *diagRec;	/* diagnostic record.		*/
  char	resultSetName[MAX_RSN_LENGTH];  /* current result set name */
  
  AC_LIST	*DIDList;		/* List of document ids.*/
  SEARCH_RESULT	*searchResult;
  
  /* Information about the server.
   */
  int	s_state;
  int	s_portNumber;		/* server port number.	*/
  char	*s_serverMachineName;	/* server machine name.	*/
  char	*s_serverIP;		/* ip address in dot format	*/

  int	s_preferredMessageSize;	
  int	s_exceptionalRecordSize;
  char	*s_protocolVersion;
  char	*s_options;
  char	*s_implementationId;
  char	*s_implementationName;
  char	*s_implementationVersion;
  
  /* use 1 to indicate yes, 0 to indicate no.	*/
  ZBOOL	s_supportVersion1;
  ZBOOL	s_supportVersion2;
  ZBOOL	s_supportVersion3;
  
  /* the following are flags from the Options bits set in initialize */
  ZBOOL	s_supportSearch;
  ZBOOL	s_supportPresent;
  ZBOOL	s_supportDeleteResultSet;
  ZBOOL	s_supportResourceReport;
  ZBOOL	s_supportTriggerResourceControl;
  ZBOOL	s_supportResourceControl;
  ZBOOL	s_supportAccessControl;
  ZBOOL	s_supportScan;
  ZBOOL	s_supportSort;
  ZBOOL	s_supportExtendedServices;
  ZBOOL	s_supportLevel_1_Segmentation;
  ZBOOL	s_supportLevel_2_Segmentation;
  ZBOOL	s_supportConcurrentOperations;
  ZBOOL	s_supportNamedResultSets;
  
  /* query types supported -- set ?? */
  ZBOOL	s_supportType0Query;
  ZBOOL	s_supportType1Query;
  ZBOOL	s_supportType2Query;
  ZBOOL	s_supportType100Query;
  ZBOOL	s_supportType101Query;
  ZBOOL	s_supportType102Query;
  
  ZBOOL	s_supportElementSetNames;
  ZBOOL	s_supportSingleElementSetName;
  
  AC_LIST	*s_databaseNames;	/* list of database names supported.*/	
  AC_LIST	*s_accessPoints;	/* list of access points supported. */
                                /* There should be one per database */
  AC_LIST *s_DBConfigFileNames;    /* ditto for configfilenames        */
  AC_LIST *s_databasePathNames;    /* ditto for dbpathnames */

  
  
  
  char *s_smallElementSetNames;  /* currently only one generic name allowed */
  char *s_mediumElementSetNames;
  
  ZBOOL	multipleDatabasesSearch;/* support multiple database search ? */
  int	maximumNumberDatabases;
  /* maximum number of databases
   * allowed in one search request  */
  ZBOOL	namedResultSet;		/* support result set name ?	*/
  int	maximumNumberResultSets;/* maximum number of result sets
				 * one client can have.
				 */

  char	*s_logFileName;		/* where to save the log messages. */	
  char  *s_permLogFileName;
  FILE	*s_lfptr;		/* log file pointer.	*/
  
  int	s_timeOut;		/* how long the server should wait
				 * before terminating the connection.
				 */		
  int	s_pid;			/* server (forked) process id.	*/
  void	*searchEngine;		/* pointer to search engine structure */
  char	*c_databaseNames[10];	/* list of database names for the
				 * current search. */ 
  char	*s_resultSetDirectory;	/* where to store the result sets */
  AC_LIST	*s_attributeSetIds;	/* attribute sets supported by server */

  ExplainStrings *s_explainstrings; /* explain information loaded from */
                                    /* server initialization file */
  char	c_resultSetDir[250];
  
  /* Information about the client.
   */
  int	c_fd;
  char	*c_clientMachineName;
  char	*c_clientIP;
  int	c_preferredMessageSize;	
  int	c_exceptionalRecordSize;
  OctetString c_referenceId;
  char	*c_protocolVersion;
  char	*c_options;
  char	*c_implementationId;
  char	*c_implementationName;
  char	*c_implementationVersion;
  char  *c_DBMS_query;
  int   c_DBMS_recsyntax;
  int	c_smallSetUpperBound;
  int	c_largeSetLowerBound;
  int	c_mediumSetPresentNumber;
  char  *c_login_id;  /* these are used on the server for authentication */
  char  *c_login_pwd;

  ZBOOL	c_supportVersion1;
  ZBOOL	c_supportVersion2;
  ZBOOL	c_supportVersion3;
  
  /* Options to request in initialization */
  ZBOOL	c_requestSearch;
  ZBOOL	c_requestPresent;
  ZBOOL	c_requestDeleteResultSet;
  ZBOOL	c_requestResourceReport;
  ZBOOL	c_requestTriggerResourceControl;
  ZBOOL	c_requestResourceControl;
  ZBOOL	c_requestAccessControl;
  ZBOOL	c_requestScan;
  ZBOOL	c_requestSort;
  ZBOOL	c_requestExtendedServices;
  ZBOOL	c_requestLevel_1_Segmentation;
  ZBOOL	c_requestLevel_2_Segmentation;
  ZBOOL	c_requestConcurrentOperations;
  ZBOOL	c_requestNamedResultSets;

  char *c_attributeSetId;	/* attribute set requested	*/

  int	timePDUSent;	/* time the last pdu was sent off	*/
  int	timePDUReceived;/* time the latest incoming pdu was received */
  int	c_timeOut;
  
  /* statistics
   */
  int	sessionStart;	/* time this session started	*/
  int	sessionEnd;	/* time this session terminated	*/
  int	numberSearches;	/* no of searches during this session	*/
  int	numberPresents;
  int	numberDeletes;
  int	numberResourceReports;
  int	numberOutPDUs;	/* no of outgoing pdus		*/
  int	numberInPDUs;	/* no of incoming pdus		*/
  int	accumulatedResponseTime;	
  /* useful for adjusting the timeout time
   * automatically.
   */
  int   disable_saving_result_sets;
  
 } ZSESSION;

typedef struct z_association {
  struct zsession	*head;		/* pointer to the first zsession */
  struct zsession	*tail;		/* pointer to the last zsession	*/
  int	counts;			/* number of active sessions	*/
  int	no_fd_set;
  int	maximum_fd;		/* maximum fd	*/
  fd_set	read_fd_set; 
  fd_set	write_fd_set; 
  fd_set	active_fd_set;
 } ZASSOCIATION;

#endif



