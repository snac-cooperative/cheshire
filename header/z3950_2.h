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
#ifndef	Z3950_2_H
#define	Z3950_2_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/uio.h>
#include <unistd.h>
#include "zprimitive.h"
#include "list.h"

typedef struct ListStatuses {
	struct ListStatuses	*next;
	struct {
		OctetString	resultSetId;
		int	deleteSetStatus;
	}item;
} ListStatuses;

typedef struct ElementSetNames {
	enum {
		t_generic, t_databaseSpecific
	}	which;
	union {
		OctetString	generic;
		struct databaseSpecific_list {
			struct databaseSpecific_list		*next;
			struct {
				OctetString	databaseName;
				OctetString	elementSetName;
			}item;
		} *databaseSpecific;
	} u;
} ElementSetNames;

typedef struct DiagRec {
	ObjectIdentifier	diagnosticSetId;
	int	condition;
	OctetString	addinfo;
} DiagRec;


typedef struct NamePlusRecord {
	OctetString	databaseName;
	struct {
		enum {
			t_databaseRecord, t_surrogateDiagnostic
		}	which;
		union {
			External	*databaseRecord;
			DiagRec	*surrogateDiagnostic;
		} u;
	}	record;
} NamePlusRecord;

typedef struct Records {
	enum {
		t_dataBaseOrSurDiagnostics, t_nonSurrogateDiagnostic
	}	which;
	union {
		struct dataBaseOrSurDiagnostics_list {
			struct dataBaseOrSurDiagnostics_list		*next;
			NamePlusRecord	*item;
		} *dataBaseOrSurDiagnostics;
		DiagRec	*nonSurrogateDiagnostic;
	} u;
} Records;

typedef struct InitializeRequest {
	OctetString	referenceId;
	BitString	protocolVersion;
	BitString	options;
	int	preferredMessageSize;
	int	maximumRecordSize;
	any	idAuthentication;
	OctetString	implementationId;
	OctetString	implementationName;
	OctetString	implementationVersion;
	External	*userInformationField;
} InitializeRequest;


typedef struct InitializeResponse {
	OctetString	referenceId;
	BitString	protocolVersion;
	BitString	options;
	int	preferredMessageSize;
	int	maximumRecordSize;
	Boolean	result;
	OctetString	implementationId;
	OctetString	implementationName;
	OctetString	implementationVersion;
	External	*userInformationField;
} InitializeResponse;


typedef BitString ProtocolVersion;
typedef BitString Options;
typedef int PreferredMessageSize;
typedef int MaximumRecordSize;
typedef OctetString ImplementationId;
typedef OctetString ImplementationName;
typedef OctetString ImplementationVersion;
typedef External UserInformationField;



typedef OctetString Term;


typedef struct Operator {
	enum {
		t_and, t_or, t_and_not
	}	which;
	union {
		unsigned char	and;
		unsigned char	or;
		unsigned char	and_not;
	} u;
} Operator;


typedef struct AttributeElement {
	int	attributeType;
	int	attributeValue;
} AttributeElement;


typedef struct AttributeList {
	struct AttributeList	*next;
		AttributeElement	*item;
	} AttributeList;

typedef struct AttributesPlusTerm {
	AttributeList	*attributeList;
	OctetString	term;
} AttributesPlusTerm;

typedef struct Operand {
	enum {
		t_attributesPlusTerm, t_resultSetId
	}	which;
	union {
		AttributesPlusTerm	*attributesPlusTerm;
		OctetString	resultSetId;
	} u;
} Operand;

typedef struct RPNStructure {
	enum {
		t_operand, t_binop
	}	which;
	union {
		Operand	*operand;
		struct binop {
			struct RPNStructure	*rpn1;
			struct RPNStructure	*rpn2;
			Operator	*op;
		} binop;
	} u;
} RPNStructure;



typedef struct RPNQuery {
	ObjectIdentifier	attributeSetId;
	RPNStructure	*rpnStructure;
} RPNQuery;

typedef struct Query {
	enum {
		t_type_0, t_type_1, t_type_2, t_type_100, t_type_101
	}	which;
	union {
		any	type_0;
		RPNQuery	*type_1;
		OctetString	type_2;
		OctetString	type_100;
		OctetString	type_101;
	} u;
} Query;



typedef int AttributeType;
typedef int AttributeValue;

typedef struct SearchRequest {
	OctetString	referenceId;
	int	smallSetUpperBound;
	int	largeSetLowerBound;
	int	mediumSetPresentNumber;
	Boolean	replaceIndicator;
	OctetString	resultSetName;
	struct databaseNames_list {
		struct databaseNames_list		*next;
		OctetString	item;
	} *databaseNames;
	ElementSetNames	*smallSetElementSetNames;
	ElementSetNames	*mediumSetElementSetNames;
	ObjectIdentifier	preferredRecordSyntax;
	Query	*query;
} SearchRequest;

typedef struct SearchResponse {
	OctetString	referenceId;
	int	resultCount;
	int	numberOfRecordsReturned;
	int	nextResultSetPosition;
	Boolean	searchStatus;
	int	resultSetStatus;
	int	presentStatus;
	Records	*databaseOrDiagnosticRecords;
} SearchResponse;


typedef struct PresentRequest {
	OctetString	referenceId;
	OctetString	resultSetId;
	int	resultSetStartPoint;
	int	numberOfRecordsRequested;
	ElementSetNames	*elementSetNames;
	ObjectIdentifier	preferredRecordSyntax;
} PresentRequest;


typedef struct PresentResponse {
	OctetString	referenceId;
	int	numberOfRecordsReturned;
	int	nextResultSetPosition;
	int	presentStatus;
	Records	*databaseOrDiagnosticRecords;
} PresentResponse;

typedef External DatabaseRecord;



typedef OctetString ElementSetName;
typedef int NumberOfRecordsReturned;
typedef int NextResultSetPosition;
typedef int PresentStatus;
typedef ObjectIdentifier PreferredRecordSyntax;


typedef struct DeleteResultSetRequest {
	OctetString	referenceId;
	int	deleteOperation;
	struct resultSetList_list {
		struct resultSetList_list		*next;
		OctetString	item;
	} *resultSetList;
} DeleteResultSetRequest;


typedef struct DeleteResultSetResponse {
	OctetString	referenceId;
	int	deleteOperationStatus;
	ListStatuses	*deleteListStatuses;
	int	numberNotDeleted;
	ListStatuses	*bulkStatuses;
	OctetString	deleteMessage;
} DeleteResultSetResponse;


typedef int DeleteSetStatus;


typedef struct Estimate {
	int	estimateType;
	int	estimateValue;
	int	currency_code;
} Estimate;

typedef struct ResourceReport {
	struct estimates_list {
		struct estimates_list		*next;
		Estimate	*item;
	} *estimates;
	OctetString	message;
} ResourceReport;

typedef struct AccessControlRequest {
	OctetString	referenceId;
	OctetString	securityChallenge;
} AccessControlRequest;


typedef struct AccessControlResponse {
	OctetString	referenceId;
	OctetString	securityChallengeResponse;
} AccessControlResponse;


typedef struct ResourceControlRequest {
	OctetString	referenceId;
	Boolean	suspendedFlag;
	ResourceReport	*resourceReport;
	int	partialResultsAvailable;
	Boolean	responseRequired;
	Boolean	triggeredRequestFlag;
} ResourceControlRequest;


typedef struct ResourceControlResponse {
	OctetString	referenceId;
	Boolean	continueFlag;
	Boolean	resultSetWanted;
} ResourceControlResponse;


typedef struct TriggerResourceControlRequest {
	OctetString	referenceId;
	int	requestedOperation;
	ObjectIdentifier	preferredResourceReportFormat;
	Boolean	resultSetWanted;
} TriggerResourceControlRequest;


typedef struct ResourceReportRequest {
	OctetString	referenceId;
	ObjectIdentifier	preferredResourceReportFormat;
} ResourceReportRequest;


typedef struct ResourceReportResponse {
	OctetString	referenceId;
	int	resourceReportStatus;
	ResourceReport	*resourceReport;
} ResourceReportResponse;


typedef ObjectIdentifier ResourceReportId;
typedef OctetString ReferenceId;
typedef OctetString DatabaseName;
typedef OctetString ResultSetId;

typedef struct PDU {
	enum {
t_initRequest, t_initResponse, t_searchRequest, t_searchResponse, t_presentRequest, t_presentResponse, t_deleteResultSetRequest, t_deleteResultSetResponse, t_accessControlRequest, t_accessControlResponse, t_resourceControlRequest, t_resourceControlResponse, t_triggerResourceControlRequest, t_resourceReportRequest, t_resourceReportResponse
	}	which;
	union {
	InitializeRequest	*initRequest;
	InitializeResponse	*initResponse;
	SearchRequest	*searchRequest;
	SearchResponse	*searchResponse;
	PresentRequest	*presentRequest;
	PresentResponse	*presentResponse;
	DeleteResultSetRequest	*deleteResultSetRequest;
	DeleteResultSetResponse	*deleteResultSetResponse;
	AccessControlRequest	*accessControlRequest;
	AccessControlResponse	*accessControlResponse;
	ResourceControlRequest	*resourceControlRequest;
	ResourceControlResponse	*resourceControlResponse;
	TriggerResourceControlRequest	*triggerResourceControlRequest;
	ResourceReportRequest	*resourceReportRequest;
	ResourceReportResponse	*resourceReportResponse;
	} u;
} PDU;

#ifndef ZERRNO
int	z_errno;
#define ZERRNO
#endif

#include "oointern.h"
#include "berintern.h"
#include "asn_decode.h"
#include "session.h"
#include "zinter.h"

#endif	/* Z3950_2_H	*/

