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
#ifndef Z3950_3_H
#define Z3950_3_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifndef WIN32
#include <sys/uio.h>
#include <unistd.h>
#endif
#include "zprimitive.h"
#include "list.h"

typedef GeneralString  InternationalString;
typedef struct PDU PDU;
typedef struct InitializeRequest InitializeRequest;
typedef struct InitializeResponse InitializeResponse;
typedef BitString ProtocolVersion;
typedef BitString Options;
typedef struct SearchRequest SearchRequest;
typedef struct Query Query;
typedef struct RPNQuery RPNQuery;
typedef struct RPNStructure RPNStructure;
typedef struct Operand Operand;
typedef struct AttributesPlusTerm AttributesPlusTerm;
typedef struct ResultSetPlusAttributes ResultSetPlusAttributes;
typedef struct AttributeList AttributeList;
typedef struct Term Term;
typedef struct Operator Operator;
typedef struct AttributeElement AttributeElement;
typedef struct ProximityOperator ProximityOperator;
typedef int KnownProximityUnit;
typedef struct SearchResponse SearchResponse;
typedef struct PresentRequest PresentRequest;
typedef struct Segment Segment;
typedef struct PresentResponse PresentResponse;
typedef struct Records Records;
typedef struct NamePlusRecord NamePlusRecord;
typedef struct FragmentSyntax FragmentSyntax;
typedef struct DiagRec DiagRec;
typedef struct DefaultDiagFormat DefaultDiagFormat;
typedef struct Range Range;
typedef struct ElementSetNames ElementSetNames;
typedef int PresentStatus;
typedef struct CompSpec CompSpec;
typedef struct Specification Specification;
typedef struct DeleteResultSetRequest DeleteResultSetRequest;
typedef struct DeleteResultSetResponse DeleteResultSetResponse;
typedef struct ListStatuses ListStatuses;
typedef int DeleteSetStatus;
typedef struct AccessControlRequest AccessControlRequest;
typedef struct AccessControlResponse AccessControlResponse;
typedef struct ResourceControlRequest ResourceControlRequest;
typedef struct ResourceControlResponse ResourceControlResponse;
typedef struct TriggerResourceControlRequest TriggerResourceControlRequest;
typedef struct ResourceReportRequest ResourceReportRequest;
typedef struct ResourceReportResponse ResourceReportResponse;
/*
typedef External ResourceReport;
*/
typedef ObjectIdentifier ResourceReportId;
typedef struct ScanRequest ScanRequest;
typedef struct ScanResponse ScanResponse;
typedef struct ListEntries ListEntries;
typedef struct Entry Entry;
typedef struct TermInfo TermInfo;
typedef struct OccurrenceByAttributes OccurrenceByAttributes;
typedef struct SortRequest SortRequest;
typedef struct SortResponse SortResponse;
typedef struct SortKeySpec SortKeySpec;
typedef struct SortElement SortElement;
typedef struct SortKey SortKey;
typedef struct ExtendedServicesRequest ExtendedServicesRequest;
typedef struct ExtendedServicesResponse ExtendedServicesResponse;
typedef struct Permissions Permissions;
typedef struct Close Close;
typedef int CloseReason;
typedef OctetString ReferenceId;
typedef InternationalString  ResultSetId;
typedef InternationalString  ElementSetName;
typedef InternationalString  DatabaseName;
typedef ObjectIdentifier AttributeSetId;
typedef struct OtherInformation OtherInformation;
typedef struct InfoCategory InfoCategory;
typedef struct IntUnit IntUnit;
typedef struct Unit Unit;
typedef struct StringOrNumeric StringOrNumeric;
typedef struct DiagnosticFormat DiagnosticFormat;
typedef struct DiagFormat DiagFormat;
typedef struct Explain_Record Explain_Record;
typedef struct TargetInfo TargetInfo;
typedef struct DatabaseInfo DatabaseInfo;
typedef struct SchemaInfo SchemaInfo;
typedef struct ElementInfo ElementInfo;
typedef struct Path Path;
typedef struct ElementDataType ElementDataType;
typedef int PrimitiveDataType;
typedef struct TagSetInfo TagSetInfo;
typedef struct RecordSyntaxInfo RecordSyntaxInfo;
typedef struct AttributeSetInfo AttributeSetInfo;
typedef struct AttributeType AttributeType;
typedef struct AttributeDescription AttributeDescription;
typedef struct TermListInfo TermListInfo;
typedef struct ExtendedServicesInfo ExtendedServicesInfo;
typedef struct AttributeDetails AttributeDetails;
typedef struct AttributeSetDetails AttributeSetDetails;
typedef struct AttributeTypeDetails AttributeTypeDetails;
typedef struct OmittedAttributeInterpretation OmittedAttributeInterpretation;
typedef struct AttributeValue AttributeValue;
typedef struct TermListDetails TermListDetails;
typedef struct ElementSetDetails ElementSetDetails;
typedef struct RetrievalRecordDetails RetrievalRecordDetails;
typedef struct PerElementDetails PerElementDetails;
typedef struct RecordTag RecordTag;
typedef struct SortDetails SortDetails;
typedef struct SortKeyDetails SortKeyDetails;
typedef struct ProcessingInformation ProcessingInformation;
typedef struct VariantSetInfo VariantSetInfo;
typedef struct VariantClass VariantClass;
typedef struct VariantType VariantType;
typedef struct VariantValue VariantValue;
typedef struct ValueSet ValueSet;
typedef struct ValueRange ValueRange;
typedef struct ValueDescription ValueDescription;
typedef struct UnitInfo UnitInfo;
typedef struct UnitType UnitType;
typedef struct Units Units;
typedef struct CategoryList CategoryList;
typedef struct CategoryInfo CategoryInfo;
typedef struct CommonInfo CommonInfo;
typedef struct HumanString HumanString;
typedef struct IconObject IconObject;
typedef InternationalString  LanguageCode;
typedef struct ContactInfo ContactInfo;
typedef struct NetworkAddress NetworkAddress;
typedef struct AccessInfo AccessInfo;
typedef struct QueryTypeDetails QueryTypeDetails;
typedef struct PrivateCapabilities PrivateCapabilities;
typedef struct RpnCapabilities RpnCapabilities;
typedef struct Iso8777Capabilities Iso8777Capabilities;
typedef struct ProximitySupport ProximitySupport;
typedef struct SearchKey SearchKey;
typedef struct AccessRestrictions AccessRestrictions;
typedef struct Costs Costs;
typedef struct Charge Charge;
typedef struct DatabaseList DatabaseList;
typedef struct AttributeCombinations AttributeCombinations;
typedef struct AttributeCombination AttributeCombination;
typedef struct AttributeOccurrence AttributeOccurrence;
typedef InternationalString  SutrsRecord;
typedef struct OPACRecord OPACRecord;
typedef struct HoldingsRecord HoldingsRecord;
typedef struct HoldingsAndCircData HoldingsAndCircData;
typedef struct Volume Volume;
typedef struct CircRecord CircRecord;
typedef struct BriefBib BriefBib;
typedef struct FormatSpec FormatSpec;
typedef struct GenericRecord GenericRecord;
typedef struct TaggedElement TaggedElement;
typedef struct ElementData ElementData;
typedef struct ElementMetaData ElementMetaData;
typedef struct TagPath TagPath;
typedef struct Order Order;
typedef struct Usage Usage;
typedef struct HitVector HitVector;
typedef struct Variant Variant;
typedef struct TaskPackage TaskPackage;
typedef struct ResourceReport ResourceReport;
typedef struct Estimate Estimate;
typedef int EstimateType;
typedef struct PromptObject PromptObject;
typedef struct Challenge Challenge;
typedef struct Response Response;
typedef struct PromptId PromptId;
typedef struct Encryption Encryption;
typedef struct DES_RN_Object DES_RN_Object;
typedef struct DRNType DRNType;
typedef struct KRBObject KRBObject;
typedef struct KRBRequest KRBRequest;
typedef struct KRBResponse KRBResponse;
typedef struct PersistentResultSet PersistentResultSet;
typedef struct OriginPartNotToKeep OriginPartNotToKeep;
typedef struct TargetPart TargetPart;
typedef struct PersistentQuery PersistentQuery;
typedef struct OriginPartToKeep OriginPartToKeep;
typedef struct PeriodicQuerySchedule PeriodicQuerySchedule;
typedef struct Period Period;
typedef struct ItemOrder ItemOrder;
typedef struct CreditCardInfo CreditCardInfo;
typedef struct Update Update;
typedef struct SuppliedRecords SuppliedRecords;
typedef struct CorrelationInfo CorrelationInfo;
typedef struct TaskPackageRecordStructure TaskPackageRecordStructure;
typedef struct ExportSpecification ExportSpecification;
typedef struct Destination Destination;
typedef struct ExportInvocation ExportInvocation;
typedef struct SearchInfoReport SearchInfoReport;
typedef struct ResultsByDB ResultsByDB;
typedef struct QueryExpression QueryExpression;
typedef struct Espec_1 Espec_1;
typedef struct ElementRequest ElementRequest;
typedef struct SimpleElement SimpleElement;
typedef struct Occurrences Occurrences;


struct PDU {
    enum {
        e1_initRequest, e1_initResponse, e1_searchRequest, e1_searchResponse, e1_presentRequest, e1_presentResponse, e1_deleteResultSetRequest, e1_deleteResultSetResponse, e1_accessControlRequest, e1_accessControlResponse, e1_resourceControlRequest, e1_resourceControlResponse, e1_triggerResourceControlRequest, e1_resourceReportRequest, e1_resourceReportResponse, e1_scanRequest, e1_scanResponse, e1_sortRequest, e1_sortResponse, e1_segmentRequest, e1_extendedServicesRequest, e1_extendedServicesResponse, e1_close
    } which;
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
        ScanRequest	*scanRequest;
        ScanResponse	*scanResponse;
        SortRequest	*sortRequest;
        SortResponse	*sortResponse;
        Segment	*segmentRequest;
        ExtendedServicesRequest	*extendedServicesRequest;
        ExtendedServicesResponse	*extendedServicesResponse;
        Close	*close;
    } u;
};

struct InitializeRequest {
    OctetString	referenceId;
    BitString	protocolVersion;
    BitString	options;
    int	preferredMessageSize;
    int	exceptionalRecordSize;
    any	idAuthentication;
    InternationalString	implementationId;
    InternationalString	implementationName;
    InternationalString	implementationVersion;
    External	*userInformationField;
    OtherInformation	*otherInfo;
};

struct InitializeResponse {
    OctetString	referenceId;
    BitString	protocolVersion;
    BitString	options;
    int	preferredMessageSize;
    int	exceptionalRecordSize;
    Boolean	result;
    InternationalString	implementationId;
    InternationalString	implementationName;
    InternationalString	implementationVersion;
    External	*userInformationField;
    OtherInformation	*otherInfo;
};



struct SearchRequest {
    OctetString	referenceId;
    int	smallSetUpperBound;
    int	largeSetLowerBound;
    int	mediumSetPresentNumber;
    Boolean	replaceIndicator;
    InternationalString	resultSetName;
    struct databaseNames_List1 {
        struct databaseNames_List1	*next;
        DatabaseName	item;
    } *databaseNames;
    ElementSetNames	*smallSetElementSetNames;
    ElementSetNames	*mediumSetElementSetNames;
    ObjectIdentifier	preferredRecordSyntax;
    Query	*query;
    OtherInformation	*additionalSearchInfo;
    OtherInformation	*otherInfo;
};

struct Query {
    enum {
        e2_type_0, e2_type_1, e2_type_2, e2_type_100, e2_type_101, e2_type_102
    } which;
    union {
        any	type_0;
        RPNQuery	*type_1;
        OctetString	type_2;
        OctetString	type_100;
        RPNQuery	*type_101;
        OctetString	type_102;
    } u;
};

struct RPNQuery {
    ObjectIdentifier	attributeSet;
    RPNStructure	*rpn;
};

struct RPNStructure {
    enum {
        e3_op, e3_rpnRpnOp
    } which;
    union {
        Operand	*op;
        struct rpnRpnOp {
            RPNStructure	*rpn1;
            RPNStructure	*rpn2;
            Operator	*op;
        } rpnRpnOp;
    } u;
};

struct Operand {
    enum {
        e4_attrTerm, e4_resultSet, e4_resultAttr
    } which;
    union {
        AttributesPlusTerm	*attrTerm;
        ResultSetId	resultSet;
        ResultSetPlusAttributes	*resultAttr;
    } u;
};

struct AttributesPlusTerm {
    AttributeList	*attributes;
    Term	*term;
};

struct ResultSetPlusAttributes {
    ResultSetId	resultSet;
    AttributeList	*attributes;
};

struct AttributeList {
    struct AttributeList	*next;
    AttributeElement	*item;
};

struct Term {
    enum {
        e5_general, e5_numeric, e5_characterString, e5_oid, e5_dateTime, e5_external, e5_integerAndUnit, e5_nullVal
    } which;
    union {
        OctetString	general;
        int	numeric;
        InternationalString	characterString;
        ObjectIdentifier	oid;
        GeneralizedTime	dateTime;
        External	*external;
        IntUnit	*integerAndUnit;
        unsigned char	nullVal;
    } u;
};

struct Operator {
    enum {
        e6_and, e6_or, e6_and_not, e6_prox
    } which;
    union {
        unsigned char	and;
        unsigned char	or;
        unsigned char	and_not;
        ProximityOperator	*prox;
    } u;
};

struct AttributeElement {
    ObjectIdentifier	attributeSet;
    int	attributeType;
    struct {
        enum {
            e7_numeric, e7_complex
        } which;
        union {
            int	numeric;
            struct complex {
                struct list_List2 {
                    struct list_List2	*next;
                    StringOrNumeric	*item;
                } *list;
                struct semanticAction_List3 {
                    struct semanticAction_List3	*next;
                    int	item;
                } *semanticAction;
            } complex;
        } u;
    } attributeValue;
};

struct ProximityOperator {
    Boolean	exclusion;
    int	distance;
    Boolean	ordered;
    int	relationType;
    struct {
        enum {
            e8_known, e8_private
        } which;
        union {
            int	known;
            int	private_var;
        } u;
    } proximityUnitCode;
};


struct SearchResponse {
    OctetString	referenceId;
    int	resultCount;
    int	numberOfRecordsReturned;
    int	nextResultSetPosition;
    Boolean	searchStatus;
    int	resultSetStatus;
    int	presentStatus;
    Records	*records;
    OtherInformation	*additionalSearchInfo;
    OtherInformation	*otherInfo;
};

struct PresentRequest {
    OctetString	referenceId;
    ResultSetId	resultSetId;
    int	resultSetStartPoint;
    int	numberOfRecordsRequested;
    struct additionalRanges_List4 {
        struct additionalRanges_List4	*next;
        Range	*item;
    } *additionalRanges;
    struct RecordComposition {
        enum {
            e9_simple, e9_complex
        } which;
        union {
            ElementSetNames	*simple;
            CompSpec	*complex;
        } u;
    } *recordComposition;
    ObjectIdentifier	preferredRecordSyntax;
    int	maxSegmentCount;
    int	maxRecordSize;
    int	maxSegmentSize;
    OtherInformation	*otherInfo;
};

struct Segment {
    OctetString	referenceId;
    int	numberOfRecordsReturned;
    struct segmentRecords_List5 {
        struct segmentRecords_List5	*next;
        NamePlusRecord	*item;
    } *segmentRecords;
    OtherInformation	*otherInfo;
};

struct PresentResponse {
    OctetString	referenceId;
    int	numberOfRecordsReturned;
    int	nextResultSetPosition;
    int	presentStatus;
    Records	*records;
    OtherInformation	*otherInfo;
};

struct Records {
    enum {
        e10_responseRecords, e10_nonSurrogateDiagnostic, e10_multipleNonSurDiagnostics
    } which;
    union {
        struct responseRecords_List6 {
            struct responseRecords_List6	*next;
            NamePlusRecord	*item;
        } *responseRecords;
        DefaultDiagFormat	*nonSurrogateDiagnostic;
        struct multipleNonSurDiagnostics_List7 {
            struct multipleNonSurDiagnostics_List7	*next;
            DiagRec	*item;
        } *multipleNonSurDiagnostics;
    } u;
};

struct NamePlusRecord {
    DatabaseName	name;
    struct {
        enum {
            e11_retrievalRecord, e11_surrogateDiagnostic, e11_startingFragment, e11_intermediateFragment, e11_finalFragment
        } which;
        union {
            External	*retrievalRecord;
            DiagRec	*surrogateDiagnostic;
            FragmentSyntax	*startingFragment;
            FragmentSyntax	*intermediateFragment;
            FragmentSyntax	*finalFragment;
        } u;
    } record;
};

struct FragmentSyntax {
    enum {
        e12_externallyTagged, e12_notExternallyTagged
    } which;
    union {
        External	*externallyTagged;
        OctetString	notExternallyTagged;
    } u;
};

struct DiagRec {
    enum {
        e13_defaultFormat, e13_externallyDefined
    } which;
    union {
        DefaultDiagFormat	*defaultFormat;
        External	*externallyDefined;
    } u;
};

struct DefaultDiagFormat {
    ObjectIdentifier	diagnosticSetId;
    int	condition;
    struct {
        enum {
            e14_v2Addinfo, e14_v3Addinfo
        } which;
        union {
            OctetString	v2Addinfo;
            InternationalString	v3Addinfo;
        } u;
    } addinfo;
};

struct Range {
    int	startingPosition;
    int	numberOfRecords;
};

struct ElementSetNames {
    enum {
        e15_genericElementSetName, e15_databaseSpecific
    } which;
    union {
        InternationalString	genericElementSetName;
        struct databaseSpecific_List8 {
            struct databaseSpecific_List8	*next;
            struct {
                DatabaseName	dbName;
                ElementSetName	esn;
            } item;
        } *databaseSpecific;
    } u;
};


struct CompSpec {
    Boolean	selectAlternativeSyntax;
    Specification	*generic;
    struct dbSpecific_List9 {
        struct dbSpecific_List9	*next;
        struct {
            DatabaseName	db;
            Specification	*spec;
        } item;
    } *dbSpecific;
    struct recordSyntax_List10 {
        struct recordSyntax_List10	*next;
        ObjectIdentifier	item;
    } *recordSyntax;
};

struct Specification {
    ObjectIdentifier	schema;
    struct elementSpec {
        enum {
            e16_elementSetName, e16_externalEspec
        } which;
        union {
            InternationalString	elementSetName;
            External	*externalEspec;
        } u;
    } *elementSpec;
};

struct DeleteResultSetRequest {
    OctetString	referenceId;
    int	deleteFunction;
    struct resultSetList_List11 {
        struct resultSetList_List11	*next;
        ResultSetId	item;
    } *resultSetList;
    OtherInformation	*otherInfo;
};

struct DeleteResultSetResponse {
    OctetString	referenceId;
    int	deleteOperationStatus;
    ListStatuses	*deleteListStatuses;
    int	numberNotDeleted;
    ListStatuses	*bulkStatuses;
    InternationalString	deleteMessage;
    OtherInformation	*otherInfo;
};

struct ListStatuses {
    struct ListStatuses	*next;
    struct {
        ResultSetId	id;
        int	status;
    } item;
};


struct AccessControlRequest {
    OctetString	referenceId;
    struct SecurityChallenge {
        enum {
            e17_simpleForm, e17_externallyDefined
        } which;
        union {
            OctetString	simpleForm;
            External	*externallyDefined;
        } u;
    } securityChallenge;
    OtherInformation	*otherInfo;
};

struct AccessControlResponse {
    OctetString	referenceId;
    struct SecurityChallengeResponse {
        enum {
            e18_simpleForm, e18_externallyDefined
        } which;
        union {
            OctetString	simpleForm;
            External	*externallyDefined;
        } u;
    } *securityChallengeResponse;
    DiagRec	*diagnostic;
    OtherInformation	*otherInfo;
};

struct ResourceControlRequest {
    OctetString	referenceId;
    Boolean	suspendedFlag;
    ResourceReport	*resourceReport;
    int	partialResultsAvailable;
    Boolean	responseRequired;
    Boolean	triggeredRequestFlag;
    OtherInformation	*otherInfo;
};

struct ResourceControlResponse {
    OctetString	referenceId;
    Boolean	continueFlag;
    Boolean	resultSetWanted;
    OtherInformation	*otherInfo;
};

struct TriggerResourceControlRequest {
    OctetString	referenceId;
    int	requestedAction;
    ObjectIdentifier	prefResourceReportFormat;
    Boolean	resultSetWanted;
    OtherInformation	*otherInfo;
};

struct ResourceReportRequest {
    OctetString	referenceId;
    OctetString	opId;
    ObjectIdentifier	prefResourceReportFormat;
    OtherInformation	*otherInfo;
};

struct ResourceReportResponse {
    OctetString	referenceId;
    int	resourceReportStatus;
    ResourceReport	*resourceReport;
    OtherInformation	*otherInfo;
};



struct ScanRequest {
    OctetString	referenceId;
    struct databaseNames_List12 {
        struct databaseNames_List12	*next;
        DatabaseName	item;
    } *databaseNames;
    ObjectIdentifier	attributeSet;
    AttributesPlusTerm	*termListAndStartPoint;
    int	stepSize;
    int	numberOfTermsRequested;
    int	preferredPositionInResponse;
    OtherInformation	*otherInfo;
};

struct ScanResponse {
    OctetString	referenceId;
    int	stepSize;
    int	scanStatus;
    int	numberOfEntriesReturned;
    int	positionOfTerm;
    ListEntries	*entries;
    ObjectIdentifier	attributeSet;
    OtherInformation	*otherInfo;
};

struct ListEntries {
  struct entries_List13 {
    struct entries_List13	*next;
    Entry	*item;
  } *entries;
  struct nonsurrogateDiagnostics_List14 {
    struct nonsurrogateDiagnostics_List14	*next;
    DiagRec	*item;
  } *nonsurrogateDiagnostics;
};

struct Entry {
    enum {
        e19_termInfo, e19_surrogateDiagnostic
    } which;
    union {
        TermInfo	*termInfo;
        DiagRec	*surrogateDiagnostic;
    } u;
};

struct TermInfo {
    Term	*term;
    InternationalString	displayTerm;
    AttributeList	*suggestedAttributes;
    struct alternativeTerm_List15 {
        struct alternativeTerm_List15	*next;
        AttributesPlusTerm	*item;
    } *alternativeTerm;
    int	globalOccurrences;
    OccurrenceByAttributes	*byAttributes;
    OtherInformation	*otherTermInfo;
};

struct OccurrenceByAttributes {
    struct OccurrenceByAttributes	*next;
    struct {
        AttributeList	*attributes;
        struct {
            enum {
                e20_global, e20_byDatabase
            } which;
            union {
                int	global;
                struct byDatabase_List16 {
                    struct byDatabase_List16	*next;
                    struct {
                        DatabaseName	db;
                        int	num;
                        OtherInformation	*otherDbInfo;
                    } item;
                } *byDatabase;
            } u;
        } *occurrences;
        OtherInformation	*otherOccurInfo;
    } item;
};

struct SortRequest {
    OctetString	referenceId;
    struct inputResultSetNames_List17 {
        struct inputResultSetNames_List17	*next;
        InternationalString	item;
    } *inputResultSetNames;
    InternationalString	sortedResultSetName;
    struct sortSequence_List18 {
        struct sortSequence_List18	*next;
        SortKeySpec	*item;
    } *sortSequence;
    OtherInformation	*otherInfo;
};

struct SortResponse {
  OctetString	referenceId;
  int	sortStatus;
  int	resultSetStatus;
  struct diagnostics_List19 {
    struct diagnostics_List19	*next;
    DiagRec	*item;
  } *diagnostics;
#ifdef Z3950_2001
  int resultCount;
#endif
  OtherInformation	*otherInfo;
};

struct SortKeySpec {
    SortElement	*sortElement;
    int	sortRelation;
    int	caseSensitivity;
    struct {
        enum {
            e21_abort, e21_nullVal, e21_missingValueData
        } which;
        union {
            unsigned char	abort;
            unsigned char	nullVal;
            OctetString	missingValueData;
        } u;
    } missingValueAction;
};

struct SortElement {
    enum {
        e22_generic, e22_datbaseSpecific
    } which;
    union {
        SortKey	*generic;
        struct datbaseSpecific_List20 {
            struct datbaseSpecific_List20	*next;
            struct {
                DatabaseName	databaseName;
                SortKey	*dbSort;
            } item;
        } *datbaseSpecific;
    } u;
};

struct SortKey {
    enum {
        e23_sortfield, e23_elementSpec, e23_sortAttributes
    } which;
    union {
        InternationalString	sortfield;
        Specification	*elementSpec;
        struct sortAttributes {
            ObjectIdentifier	id;
            AttributeList	*list;
        } sortAttributes;
    } u;
};

struct ExtendedServicesRequest {
    OctetString	referenceId;
    int	function;
    ObjectIdentifier	packageType;
    InternationalString	packageName;
    InternationalString	userId;
    IntUnit	*retentionTime;
    Permissions	*permissions;
    InternationalString	description;
    External	*taskSpecificParameters;
    int	waitAction;
    ElementSetName	elements;
    OtherInformation	*otherInfo;
};

struct ExtendedServicesResponse {
    OctetString	referenceId;
    int	operationStatus;
    struct diagnostics_List21 {
        struct diagnostics_List21	*next;
        DiagRec	*item;
    } *diagnostics;
    External	*taskPackage;
    OtherInformation	*otherInfo;
};

struct Permissions {
    struct Permissions	*next;
    struct {
        InternationalString	userId;
        struct allowableFunctions_List22 {
            struct allowableFunctions_List22	*next;
            int	item;
        } *allowableFunctions;
    } item;
};

struct Close {
    OctetString	referenceId;
    int	closeReason;
    InternationalString	diagnosticInformation;
    ObjectIdentifier	resourceReportFormat;
    ResourceReport	*resourceReport;
    OtherInformation	*otherInfo;
};







struct OtherInformation {
    struct OtherInformation	*next;
    struct {
        InfoCategory	*category;
        struct {
            enum {
                e24_characterInfo, e24_binaryInfo, e24_externallyDefinedInfo, e24_oid
            } which;
            union {
                InternationalString	characterInfo;
                OctetString	binaryInfo;
                External	*externallyDefinedInfo;
                ObjectIdentifier	oid;
            } u;
        } information;
    } item;
};

struct InfoCategory {
    ObjectIdentifier	categoryTypeId;
    int	categoryValue;
};

struct IntUnit {
    int	value;
    Unit	*unitUsed;
};

struct Unit {
    InternationalString	unitSystem;
    StringOrNumeric	*unitType;
    StringOrNumeric	*unit;
    int	scaleFactor;
};

struct StringOrNumeric {
    enum {
        e25_string, e25_numeric
    } which;
    union {
        InternationalString	string;
        int	numeric;
    } u;
};

struct DiagnosticFormat {
    struct DiagnosticFormat	*next;
    struct {
        struct {
            enum {
                e26_defaultDiagRec, e26_explicitDiagnostic
            } which;
            union {
                DefaultDiagFormat	*defaultDiagRec;
                DiagFormat	*explicitDiagnostic;
            } u;
        } *diagnostic;
        InternationalString	message;
    } item;
};

struct DiagFormat {
    enum {
        e27_tooMany, e27_badSpec, e27_dbUnavail, e27_unSupOp, e27_attribute, e27_attCombo, e27_term, e27_proximity, e27_scan, e27_sort, e27_segmentation, e27_extServices, e27_accessCtrl, e27_recordSyntax
    } which;
    union {
        struct tooMany {
            int	tooManyWhat;
            int	max;
        } tooMany;
        struct badSpec {
            Specification	*spec;
            DatabaseName	db;
            struct goodOnes_List23 {
                struct goodOnes_List23	*next;
                Specification	*item;
            } *goodOnes;
        } badSpec;
        struct dbUnavail {
            DatabaseName	db;
            struct why {
                int	reasonCode;
                InternationalString	message;
            } why;
        } dbUnavail;
        int	unSupOp;
        struct attribute {
            ObjectIdentifier	id;
            int	type;
            int	value;
            Term	*term;
        } attribute;
        struct attCombo {
            AttributeList	*unsupportedCombination;
            struct recommendedAlternatives_List24 {
                struct recommendedAlternatives_List24	*next;
                AttributeList	*item;
            } *recommendedAlternatives;
        } attCombo;
        struct term {
            int	problem;
            Term	*term;
        } term;
        struct {
            enum {
                e28_resultSets, e28_badSet, e28_relation, e28_unit, e28_distance, e28_attributes, e28_ordered, e28_exclusion
            } which;
            union {
                unsigned char	resultSets;
                InternationalString	badSet;
                int	relation;
                int	unit;
                int	distance;
                AttributeList	*attributes;
                unsigned char	ordered;
                unsigned char	exclusion;
            } u;
        } proximity;
        struct {
            enum {
                e29_nonZeroStepSize, e29_specifiedStepSize, e29_termList1, e29_termList2, e29_posInResponse, e29_resources, e29_endOfList
            } which;
            union {
                unsigned char	nonZeroStepSize;
                unsigned char	specifiedStepSize;
                unsigned char	termList1;
                struct termList2_List25 {
                    struct termList2_List25	*next;
                    AttributeList	*item;
                } *termList2;
                int	posInResponse;
                unsigned char	resources;
                unsigned char	endOfList;
            } u;
        } scan;
        struct {
            enum {
                e30_sequence, e30_noRsName, e30_tooMany, e30_incompatible, e30_generic, e30_dbSpecific, e30_sortElement, e30_key, e30_action, e30_illegal, e30_inputTooLarge, e30_aggregateTooLarge
            } which;
            union {
                unsigned char	sequence;
                unsigned char	noRsName;
                int	tooMany;
                unsigned char	incompatible;
                unsigned char	generic;
                unsigned char	dbSpecific;
                SortElement	*sortElement;
                int	key;
                unsigned char	action;
                int	illegal;
                struct inputTooLarge_List26 {
                    struct inputTooLarge_List26	*next;
                    InternationalString	item;
                } *inputTooLarge;
                unsigned char	aggregateTooLarge;
            } u;
        } sort;
        struct {
            enum {
                e31_segmentCount, e31_segmentSize
            } which;
            union {
                unsigned char	segmentCount;
                int	segmentSize;
            } u;
        } segmentation;
        struct {
            enum {
                e32_req, e32_permission, e32_immediate
            } which;
            union {
                int	req;
                int	permission;
                int	immediate;
            } u;
        } extServices;
        struct {
            enum {
                e33_noUser, e33_refused, e33_simple, e33_oid, e33_alternative, e33_pwdInv, e33_pwdExp
            } which;
            union {
                unsigned char	noUser;
                unsigned char	refused;
                unsigned char	simple;
                struct oid_List27 {
                    struct oid_List27	*next;
                    ObjectIdentifier	item;
                } *oid;
                struct alternative_List28 {
                    struct alternative_List28	*next;
                    ObjectIdentifier	item;
                } *alternative;
                unsigned char	pwdInv;
                unsigned char	pwdExp;
            } u;
        } accessCtrl;
        struct recordSyntax {
            ObjectIdentifier	unsupportedSyntax;
            struct suggestedAlternatives_List29 {
                struct suggestedAlternatives_List29	*next;
                ObjectIdentifier	item;
            } *suggestedAlternatives;
        } recordSyntax;
    } u;
};

struct Explain_Record {
    enum {
        e34_targetInfo, e34_databaseInfo, e34_schemaInfo, e34_tagSetInfo, e34_recordSyntaxInfo, e34_attributeSetInfo, e34_termListInfo, e34_extendedServicesInfo, e34_attributeDetails, e34_termListDetails, e34_elementSetDetails, e34_retrievalRecordDetails, e34_sortDetails, e34_processing, e34_variants, e34_units, e34_categoryList
    } which;
    union {
        TargetInfo	*targetInfo;
        DatabaseInfo	*databaseInfo;
        SchemaInfo	*schemaInfo;
        TagSetInfo	*tagSetInfo;
        RecordSyntaxInfo	*recordSyntaxInfo;
        AttributeSetInfo	*attributeSetInfo;
        TermListInfo	*termListInfo;
        ExtendedServicesInfo	*extendedServicesInfo;
        AttributeDetails	*attributeDetails;
        TermListDetails	*termListDetails;
        ElementSetDetails	*elementSetDetails;
        RetrievalRecordDetails	*retrievalRecordDetails;
        SortDetails	*sortDetails;
        ProcessingInformation	*processing;
        VariantSetInfo	*variants;
        UnitInfo	*units;
        CategoryList	*categoryList;
    } u;
};

struct TargetInfo {
    CommonInfo	*commonInfo;
    InternationalString	name;
    HumanString	*recent_news;
    IconObject	*icon;
    Boolean	namedResultSets;
    Boolean	multipleDBsearch;
    int	maxResultSets;
    int	maxResultSize;
    int	maxTerms;
    IntUnit	*timeoutInterval;
    HumanString	*welcomeMessage;
    ContactInfo	*contactInfo;
    HumanString	*description;
    struct nicknames_List30 {
        struct nicknames_List30	*next;
        InternationalString	item;
    } *nicknames;
    HumanString	*usage_restrictions;
    HumanString	*paymentAddr;
    HumanString	*hours;
    struct dbCombinations_List31 {
        struct dbCombinations_List31	*next;
        DatabaseList	*item;
    } *dbCombinations;
    struct addresses_List32 {
        struct addresses_List32	*next;
        NetworkAddress	*item;
    } *addresses;
    struct languages_List33 {
        struct languages_List33	*next;
        InternationalString	item;
    } *languages;
    AccessInfo	*commonAccessInfo;
};

struct DatabaseInfo {
    CommonInfo	*commonInfo;
    DatabaseName	name;
    unsigned char	explainDatabase;
    struct nicknames_List34 {
        struct nicknames_List34	*next;
        DatabaseName	item;
    } *nicknames;
    IconObject	*icon;
    Boolean	user_fee;
    Boolean	available;
    HumanString	*titleString;
    struct keywords_List35 {
        struct keywords_List35	*next;
        HumanString	*item;
    } *keywords;
    HumanString	*description;
    DatabaseList	*associatedDbs;
    DatabaseList	*subDbs;
    HumanString	*disclaimers;
    HumanString	*news;
    struct recordCount {
        enum {
            e35_actualNumber, e35_approxNumber
        } which;
        union {
            int	actualNumber;
            int	approxNumber;
        } u;
    } *recordCount;
    HumanString	*defaultOrder;
    int	avRecordSize;
    int	maxRecordSize;
    HumanString	*hours;
    HumanString	*bestTime;
    GeneralizedTime	lastUpdate;
    IntUnit	*updateInterval;
    HumanString	*coverage;
    Boolean	proprietary;
    HumanString	*copyrightText;
    HumanString	*copyrightNotice;
    ContactInfo	*producerContactInfo;
    ContactInfo	*supplierContactInfo;
    ContactInfo	*submissionContactInfo;
    AccessInfo	*accessInfo;
};

struct SchemaInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	schema;
    InternationalString	name;
    HumanString	*description;
    struct tagTypeMapping_List36 {
        struct tagTypeMapping_List36	*next;
        struct {
            int	tagType;
            ObjectIdentifier	tagSet;
            unsigned char	defaultTagType;
        } item;
    } *tagTypeMapping;
    struct recordStructure_List37 {
        struct recordStructure_List37	*next;
        ElementInfo	*item;
    } *recordStructure;
};

struct ElementInfo {
    InternationalString	elementName;
    Path	*elementTagPath;
    ElementDataType	*dataType;
    Boolean	required;
    Boolean	repeatable;
    HumanString	*description;
};

struct Path {
    struct Path	*next;
    struct {
        int	tagType;
        StringOrNumeric	*tagValue;
    } item;
};

struct ElementDataType {
    enum {
        e36_primitive, e36_structured
    } which;
    union {
        int	primitive;
        struct structured_List38 {
            struct structured_List38	*next;
            ElementInfo	*item;
        } *structured;
    } u;
};


struct TagSetInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	tagSet;
    InternationalString	name;
    HumanString	*description;
    struct elements_List39 {
        struct elements_List39	*next;
        struct {
            InternationalString	elementname;
            struct nicknames_List40 {
                struct nicknames_List40	*next;
                InternationalString	item;
            } *nicknames;
            StringOrNumeric	*elementTag;
            HumanString	*description;
            int	dataType;
            OtherInformation	*otherTagInfo;
        } item;
    } *elements;
};

struct RecordSyntaxInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	recordSyntax;
    InternationalString	name;
    struct transferSyntaxes_List41 {
        struct transferSyntaxes_List41	*next;
        ObjectIdentifier	item;
    } *transferSyntaxes;
    HumanString	*description;
    InternationalString	asn1Module;
    struct abstractStructure_List42 {
        struct abstractStructure_List42	*next;
        ElementInfo	*item;
    } *abstractStructure;
};

struct AttributeSetInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	attributeSet;
    InternationalString	name;
    struct attributes_List43 {
        struct attributes_List43	*next;
        AttributeType	*item;
    } *attributes;
    HumanString	*description;
};

struct AttributeType {
    InternationalString	name;
    HumanString	*description;
    int	attributeType;
    struct attributeValues_List44 {
        struct attributeValues_List44	*next;
        AttributeDescription	*item;
    } *attributeValues;
};

struct AttributeDescription {
    InternationalString	name;
    HumanString	*description;
    StringOrNumeric	*attributeValue;
    struct equivalentAttributes_List45 {
        struct equivalentAttributes_List45	*next;
        StringOrNumeric	*item;
    } *equivalentAttributes;
};

struct TermListInfo {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    struct termLists_List46 {
        struct termLists_List46	*next;
        struct {
            InternationalString	name;
            HumanString	*title;
            int	searchCost;
            Boolean	scanable;
            struct broader_List47 {
                struct broader_List47	*next;
                InternationalString	item;
            } *broader;
            struct narrower_List48 {
                struct narrower_List48	*next;
                InternationalString	item;
            } *narrower;
        } item;
    } *termLists;
};

struct ExtendedServicesInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	type;
    InternationalString	name;
    Boolean	privateType;
    Boolean	restrictionsApply;
    Boolean	feeApply;
    Boolean	available;
    Boolean	retentionSupported;
    int	waitAction;
    HumanString	*description;
    External	*specificExplain;
    InternationalString	esASN;
};

struct AttributeDetails {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    struct attributesBySet_List49 {
        struct attributesBySet_List49	*next;
        AttributeSetDetails	*item;
    } *attributesBySet;
    AttributeCombinations	*attributeCombinations;
};

struct AttributeSetDetails {
    ObjectIdentifier	attributeSet;
    struct attributesByType_List50 {
        struct attributesByType_List50	*next;
        AttributeTypeDetails	*item;
    } *attributesByType;
};

struct AttributeTypeDetails {
    int	attributeType;
    OmittedAttributeInterpretation	*defaultIfOmitted;
    struct attributeValues_List51 {
        struct attributeValues_List51	*next;
        AttributeValue	*item;
    } *attributeValues;
};

struct OmittedAttributeInterpretation {
    StringOrNumeric	*defaultValue;
    HumanString	*defaultDescription;
};

struct AttributeValue {
    StringOrNumeric	*value;
    HumanString	*description;
    struct subAttributes_List52 {
        struct subAttributes_List52	*next;
        StringOrNumeric	*item;
    } *subAttributes;
    struct superAttributes_List53 {
        struct superAttributes_List53	*next;
        StringOrNumeric	*item;
    } *superAttributes;
    unsigned char	partialSupport;
};

struct TermListDetails {
    CommonInfo	*commonInfo;
    InternationalString	termListName;
    HumanString	*description;
    AttributeCombinations	*attributes;
    struct scanInfo {
        int	maxStepSize;
        HumanString	*collatingSequence;
        Boolean	increasing;
    } *scanInfo;
    int	estNumberTerms;
    struct sampleTerms_List54 {
        struct sampleTerms_List54	*next;
        Term	*item;
    } *sampleTerms;
};

struct ElementSetDetails {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    ElementSetName	elementSetName;
    ObjectIdentifier	recordSyntax;
    ObjectIdentifier	schema;
    HumanString	*description;
    struct detailsPerElement_List55 {
        struct detailsPerElement_List55	*next;
        PerElementDetails	*item;
    } *detailsPerElement;
};

struct RetrievalRecordDetails {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    ObjectIdentifier	schema;
    ObjectIdentifier	recordSyntax;
    HumanString	*description;
    struct detailsPerElement_List56 {
        struct detailsPerElement_List56	*next;
        PerElementDetails	*item;
    } *detailsPerElement;
};

struct PerElementDetails {
    InternationalString	name;
    RecordTag	*recordTag;
    struct schemaTags_List57 {
        struct schemaTags_List57	*next;
        Path	*item;
    } *schemaTags;
    int	maxSize;
    int	minSize;
    int	avgSize;
    int	fixedSize;
    Boolean	repeatable;
    Boolean	required;
    HumanString	*description;
    HumanString	*contents;
    HumanString	*billingInfo;
    HumanString	*restrictions;
    struct alternateNames_List58 {
        struct alternateNames_List58	*next;
        InternationalString	item;
    } *alternateNames;
    struct genericNames_List59 {
        struct genericNames_List59	*next;
        InternationalString	item;
    } *genericNames;
    AttributeCombinations	*searchAccess;
};

struct RecordTag {
    StringOrNumeric	*qualifier;
    StringOrNumeric	*tagValue;
};

struct SortDetails {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    struct sortKeys_List60 {
        struct sortKeys_List60	*next;
        SortKeyDetails	*item;
    } *sortKeys;
};

struct SortKeyDetails {
    HumanString	*description;
    struct elementSpecifications_List61 {
        struct elementSpecifications_List61	*next;
        Specification	*item;
    } *elementSpecifications;
    AttributeCombinations	*attributeSpecifications;
    struct sortType {
        enum {
            e37_character, e37_numeric, e37_structured
        } which;
        union {
            unsigned char	character;
            unsigned char	numeric;
            HumanString	*structured;
        } u;
    } *sortType;
    int	caseSensitivity;
};

struct ProcessingInformation {
    CommonInfo	*commonInfo;
    DatabaseName	databaseName;
    int	processingContext;
    InternationalString	name;
    ObjectIdentifier	oid;
    HumanString	*description;
    External	*instructions;
};

struct VariantSetInfo {
    CommonInfo	*commonInfo;
    ObjectIdentifier	variantSet;
    InternationalString	name;
    struct variants_List62 {
        struct variants_List62	*next;
        VariantClass	*item;
    } *variants;
};

struct VariantClass {
    InternationalString	name;
    HumanString	*description;
    int	variantClass;
    struct variantTypes_List63 {
        struct variantTypes_List63	*next;
        VariantType	*item;
    } *variantTypes;
};

struct VariantType {
    InternationalString	name;
    HumanString	*description;
    int	variantType;
    VariantValue	*variantValue;
};

struct VariantValue {
    int	dataType;
    ValueSet	*values;
};

struct ValueSet {
    enum {
        e38_range, e38_enumerated
    } which;
    union {
        ValueRange	*range;
        struct enumerated_List64 {
            struct enumerated_List64	*next;
            ValueDescription	*item;
        } *enumerated;
    } u;
};

struct ValueRange {
    ValueDescription	*lower;
    ValueDescription	*upper;
};

struct ValueDescription {
    enum {
        e39_integer, e39_string, e39_octets, e39_oid, e39_unit, e39_valueAndUnit
    } which;
    union {
        int	integer;
        InternationalString	string;
        OctetString	octets;
        ObjectIdentifier	oid;
        Unit	*unit;
        IntUnit	*valueAndUnit;
    } u;
};

struct UnitInfo {
    CommonInfo	*commonInfo;
    InternationalString	unitSystem;
    HumanString	*description;
    struct units_List65 {
        struct units_List65	*next;
        UnitType	*item;
    } *units;
};

struct UnitType {
    InternationalString	name;
    HumanString	*description;
    StringOrNumeric	*unitType;
    struct units_List66 {
        struct units_List66	*next;
        Units	*item;
    } *units;
};

struct Units {
    InternationalString	name;
    HumanString	*description;
    StringOrNumeric	*unit;
};


struct CategoryInfo {
    InternationalString	category;
    InternationalString	originalCategory;
    HumanString	*description;
    InternationalString	asn1Module;
};

struct CategoryList {
    CommonInfo	*commonInfo;
    struct categories_List67 {
        struct categories_List67	*next;
        CategoryInfo	*item;
    } *categories;
};


struct CommonInfo {
    GeneralizedTime	dateAdded;
    GeneralizedTime	dateChanged;
    GeneralizedTime	expiry;
    LanguageCode	*humanString_Language;
    OtherInformation	*otherInfo;
};

struct HumanString {
    struct HumanString	*next;
    struct {
        LanguageCode	*language;
        InternationalString	text;
    } item;
};

struct IconObject {
    struct IconObject	*next;
    struct {
        struct {
            enum {
                e40_ianaType, e40_z3950type, e40_otherType
            } which;
            union {
                InternationalString	ianaType;
                InternationalString	z3950type;
                InternationalString	otherType;
            } u;
        } bodyType;
        OctetString	content;
    } item;
};


struct ContactInfo {
    InternationalString	name;
    HumanString	*description;
    HumanString	*address;
    InternationalString	email;
    InternationalString	phone;
};

struct NetworkAddress {
    enum {
        e41_internetAddress, e41_osiPresentationAddress, e41_other
    } which;
    union {
        struct internetAddress {
            InternationalString	hostAddress;
            int	port;
        } internetAddress;
        struct osiPresentationAddress {
            InternationalString	pSel;
            InternationalString	sSel;
            InternationalString	tSel;
            InternationalString	nSap;
        } osiPresentationAddress;
        struct other {
            InternationalString	type;
            InternationalString	address;
        } other;
    } u;
};

struct AccessInfo {
    struct queryTypesSupported_List68 {
        struct queryTypesSupported_List68	*next;
        QueryTypeDetails	*item;
    } *queryTypesSupported;
    struct diagnosticsSets_List69 {
        struct diagnosticsSets_List69	*next;
        ObjectIdentifier	item;
    } *diagnosticsSets;
    struct attributeSetIds_List70 {
        struct attributeSetIds_List70	*next;
        AttributeSetId	item;
    } *attributeSetIds;
    struct schemas_List71 {
        struct schemas_List71	*next;
        ObjectIdentifier	item;
    } *schemas;
    struct recordSyntaxes_List72 {
        struct recordSyntaxes_List72	*next;
        ObjectIdentifier	item;
    } *recordSyntaxes;
    struct resourceChallenges_List73 {
        struct resourceChallenges_List73	*next;
        ObjectIdentifier	item;
    } *resourceChallenges;
    AccessRestrictions	*restrictedAccess;
    Costs	*costInfo;
    struct variantSets_List74 {
        struct variantSets_List74	*next;
        ObjectIdentifier	item;
    } *variantSets;
    struct elementSetNames_List75 {
        struct elementSetNames_List75	*next;
        ElementSetName	item;
    } *elementSetNames;
    struct unitSystems_List76 {
        struct unitSystems_List76	*next;
        InternationalString	item;
    } *unitSystems;
};

struct QueryTypeDetails {
    enum {
        e42_private, e42_rpn, e42_iso8777, e42_z39_58, e42_erpn, e42_rankedList
    } which;
    union {
        PrivateCapabilities	*private_var;
        RpnCapabilities	*rpn;
        Iso8777Capabilities	*iso8777;
        HumanString	*z39_58;
        RpnCapabilities	*erpn;
        HumanString	*rankedList;
    } u;
};

struct PrivateCapabilities {
    struct operators_List77 {
        struct operators_List77	*next;
        struct {
            InternationalString	operator_var;
            HumanString	*description;
        } item;
    } *operators;
    struct searchKeys_List78 {
        struct searchKeys_List78	*next;
        SearchKey	*item;
    } *searchKeys;
    struct description_List79 {
        struct description_List79	*next;
        HumanString	*item;
    } *description;
};

struct RpnCapabilities {
    struct operators_List80 {
        struct operators_List80	*next;
        int	item;
    } *operators;
    Boolean	resultSetAsOperandSupported;
    Boolean	restrictionOperandSupported;
    ProximitySupport	*proximity;
};

struct Iso8777Capabilities {
    struct searchKeys_List81 {
        struct searchKeys_List81	*next;
        SearchKey	*item;
    } *searchKeys;
    HumanString	*restrictions;
};

struct ProximitySupport {
    Boolean	anySupport;
    struct unitsSupported_List82 {
        struct unitsSupported_List82	*next;
        struct unitstypes {
            enum {
                e43_known, e43_private
            } which;
            union {
                int	known;
                struct private_var {
                    int	unit;
                    HumanString	*description;
                } private_var;
            } u;
        } *item;
    } *unitsSupported;
};

struct SearchKey {
    InternationalString	searchKey;
    HumanString	*description;
};

struct AccessRestrictions {
    struct AccessRestrictions	*next;
    struct {
        int	accessType;
        HumanString	*accessText;
        struct accessChallenges_List83 {
            struct accessChallenges_List83	*next;
            ObjectIdentifier	item;
        } *accessChallenges;
    } item;
};

struct Costs {
    Charge	*connectCharge;
    Charge	*connectTime;
    Charge	*displayCharge;
    Charge	*searchCharge;
    Charge	*subscriptCharge;
    struct otherCharges_List84 {
        struct otherCharges_List84	*next;
        struct {
            HumanString	*forWhat;
            Charge	*charge;
        } item;
    } *otherCharges;
};

struct Charge {
    IntUnit	*cost;
    Unit	*perWhat;
    HumanString	*text;
};

struct DatabaseList {
    struct DatabaseList	*next;
    DatabaseName	item;
};

struct AttributeCombinations {
    ObjectIdentifier	defaultAttributeSet;
    struct legalCombinations_List85 {
        struct legalCombinations_List85	*next;
        AttributeCombination	*item;
    } *legalCombinations;
};

struct AttributeCombination {
    struct AttributeCombination	*next;
    AttributeOccurrence	*item;
};

struct AttributeOccurrence {
    ObjectIdentifier	attributeSet;
    int	attributeType;
    unsigned char	mustBeSupplied;
    struct {
        enum {
            e44_any_or_none, e44_specific
        } which;
        union {
            unsigned char	any_or_none;
            struct specific_List86 {
                struct specific_List86	*next;
                StringOrNumeric	*item;
            } *specific;
        } u;
    } attributeValues;
};


struct OPACRecord {
    External	*bibliographicRecord;
    struct holdingsData_List87 {
        struct holdingsData_List87	*next;
        HoldingsRecord	*item;
    } *holdingsData;
};

struct HoldingsRecord {
    enum {
        e45_marcHoldingsRecord, e45_holdingsAndCirc
    } which;
    union {
        External	*marcHoldingsRecord;
        HoldingsAndCircData	*holdingsAndCirc;
    } u;
};

struct HoldingsAndCircData {
    InternationalString	typeOfRecord;
    InternationalString	encodingLevel;
    InternationalString	format;
    InternationalString	receiptAcqStatus;
    InternationalString	generalRetention;
    InternationalString	completeness;
    InternationalString	dateOfReport;
    InternationalString	nucCode;
    InternationalString	localLocation;
    InternationalString	shelvingLocation;
    InternationalString	callNumber;
    InternationalString	shelvingData;
    InternationalString	copyNumber;
    InternationalString	publicNote;
    InternationalString	reproductionNote;
    InternationalString	termsUseRepro;
    InternationalString	enumAndChron;
    struct volumes_List88 {
        struct volumes_List88	*next;
        Volume	*item;
    } *volumes;
    struct circulationData_List89 {
        struct circulationData_List89	*next;
        CircRecord	*item;
    } *circulationData;
};

struct Volume {
    InternationalString	enumeration;
    InternationalString	chronology;
    InternationalString	enumAndChron;
};

struct CircRecord {
    Boolean	availableNow;
    InternationalString	availablityDate;
    InternationalString	availableThru;
    InternationalString	restrictions;
    InternationalString	itemId;
    Boolean	renewable;
    Boolean	onHold;
    InternationalString	enumAndChron;
    InternationalString	midspine;
    InternationalString	temporaryLocation;
};

struct BriefBib {
    InternationalString	title;
    InternationalString	author;
    InternationalString	callNumber;
    InternationalString	recordType;
    InternationalString	bibliographicLevel;
    struct format_List90 {
        struct format_List90	*next;
        FormatSpec	*item;
    } *format;
    InternationalString	publicationPlace;
    InternationalString	publicationDate;
    InternationalString	targetSystemKey;
    InternationalString	satisfyingElement;
    int	rank;
    InternationalString	documentId;
    InternationalString	abstract;
    OtherInformation	*otherInfo;
};

struct FormatSpec {
    InternationalString	type;
    int	size;
    int	bestPosn;
};

struct GenericRecord {
    struct GenericRecord	*next;
    TaggedElement	*item;
};

struct TaggedElement {
    int	tagType;
    StringOrNumeric	*tagValue;
    int	tagOccurrence;
    ElementData	*content;
    ElementMetaData	*metaData;
    Variant	*appliedVariant;
};

struct ElementData {
    enum {
        e46_octets, e46_numeric, e46_date, e46_ext, e46_string, e46_trueOrFalse, e46_oid, e46_intUnit, e46_elementNotThere, e46_elementEmpty, e46_noDataRequested, e46_diagnostic, e46_subtree
    } which;
    union {
        OctetString	octets;
        int	numeric;
        GeneralizedTime	date;
        External	*ext;
        InternationalString	string;
        Boolean	trueOrFalse;
        ObjectIdentifier	oid;
        IntUnit	*intUnit;
        unsigned char	elementNotThere;
        unsigned char	elementEmpty;
        unsigned char	noDataRequested;
        External	*diagnostic;
        struct subtree_List91 {
            struct subtree_List91	*next;
            TaggedElement	*item;
        } *subtree;
    } u;
};

struct ElementMetaData {
    Order	*seriesOrder;
    Usage	*usageRight;
    struct hits_List92 {
        struct hits_List92	*next;
        HitVector	*item;
    } *hits;
    InternationalString	displayName;
    struct supportedVariants_List93 {
        struct supportedVariants_List93	*next;
        Variant	*item;
    } *supportedVariants;
    InternationalString	message;
    OctetString	elementDescriptor;
    TagPath	*surrogateFor;
    TagPath	*surrogateElement;
    External	*other;
};

struct TagPath {
    struct TagPath	*next;
    struct {
        int	tagType;
        StringOrNumeric	*tagValue;
        int	tagOccurrence;
    } item;
};

struct Order {
    Boolean	ascending;
    int	order;
};

struct Usage {
    int	type;
    InternationalString	restriction;
};

struct HitVector {
    Term	*satisfier;
    IntUnit	*offsetIntoElement;
    IntUnit	*length;
    int	hitRank;
    OctetString	targetToken;
};

struct Variant {
    ObjectIdentifier	globalVariantSetId;
    struct triples_List94 {
        struct triples_List94	*next;
        struct {
            ObjectIdentifier	variantSetId;
            int	class_var;
            int	type;
            struct {
                enum {
                    e47_intVal, e47_interStringVal, e47_octStringVal, e47_oidVal, e47_boolVal, e47_nullVal, e47_unit, e47_valueAndUnit
                } which;
                union {
                    int	intVal;
                    InternationalString	interStringVal;
                    OctetString	octStringVal;
                    ObjectIdentifier	oidVal;
                    Boolean	boolVal;
                    unsigned char	nullVal;
                    Unit	*unit;
                    IntUnit	*valueAndUnit;
                } u;
            } value;
        } item;
    } *triples;
};

struct TaskPackage {
    ObjectIdentifier	packageType;
    InternationalString	packageName;
    InternationalString	userId;
    IntUnit	*retentionTime;
    Permissions	*permissions;
    InternationalString	description;
    OctetString	targetReference;
    GeneralizedTime	creationDateTime;
    int	taskStatus;
    struct packageDiagnostics_List95 {
        struct packageDiagnostics_List95	*next;
        DiagRec	*item;
    } *packageDiagnostics;
    External	*taskSpecificParameters;
};

struct ResourceReport {
    struct estimates_List96 {
        struct estimates_List96	*next;
        Estimate	*item;
    } *estimates;
    InternationalString	message;
};

struct Estimate {
    int	type;
    int	value;
    int	currency_code;
};

/*
struct ResourceReport {
    struct estimates_List97 {
        struct estimates_List97	*next;
        Estimate	*item;
    } *estimates;
    InternationalString	message;
};

struct Estimate {
    StringOrNumeric	*type;
    IntUnit	*value;
};

struct PromptObject {
    enum {
        e48_challenge, e48_response
    } which;
    union {
        Challenge	*challenge;
        Response	*response;
    } u;
};

struct Challenge {
    struct Challenge	*next;
    struct {
        PromptId	*promptId;
        InternationalString	defaultResponse;
        struct {
            enum {
                e49_character, e49_encrypted
            } which;
            union {
                InternationalString	character;
                Encryption	*encrypted;
            } u;
        } *promptInfo;
        InternationalString	regExpr;
        unsigned char	responseRequired;
        struct allowedValues_List98 {
            struct allowedValues_List98	*next;
            InternationalString	item;
        } *allowedValues;
        unsigned char	shouldSave;
        int	dataType;
        External	*diagnostic;
    } item;
};

struct Response {
    struct Response	*next;
    struct {
        PromptId	*promptId;
        struct {
            enum {
                e50_string, e50_accept, e50_acknowledge, e50_diagnostic, e50_encrypted
            } which;
            union {
                InternationalString	string;
                Boolean	accept;
                unsigned char	acknowledge;
                DiagRec	*diagnostic;
                Encryption	*encrypted;
            } u;
        } promptResponse;
    } item;
};

struct PromptId {
    enum {
        e51_enummeratedPrompt, e51_nonEnumeratedPrompt
    } which;
    union {
        struct enummeratedPrompt {
            int	type;
            InternationalString	suggestedString;
        } enummeratedPrompt;
        InternationalString	nonEnumeratedPrompt;
    } u;
};

struct Encryption {
    OctetString	cryptType;
    OctetString	credential;
    OctetString	data;
};

struct DES_RN_Object {
    enum {
        e52_challenge, e52_response
    } which;
    union {
        DRNType	*challenge;
        DRNType	*response;
    } u;
};

struct DRNType {
    OctetString	userId;
    OctetString	salt;
    OctetString	randomNumber;
};

struct KRBObject {
    enum {
        e53_challenge, e53_response
    } which;
    union {
        KRBRequest	*challenge;
        KRBResponse	*response;
    } u;
};

struct KRBRequest {
    InternationalString	service;
    InternationalString	instance;
    InternationalString	realm;
};

struct KRBResponse {
    InternationalString	userid;
    OctetString	ticket;
};

struct PersistentResultSet {
    enum {
        e54_esRequest, e54_taskPackage
    } which;
    union {
        struct esRequest {
            unsigned char	toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            unsigned char	originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartNotToKeep {
    InternationalString	originSuppliedResultSet;
    int	replaceOrAppend;
};

struct TargetPart {
    InternationalString	targetSuppliedResultSet;
    int	numberOfRecords;
};

struct PersistentQuery {
    enum {
        e55_esRequest, e55_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    struct dbNames_List99 {
        struct dbNames_List99	*next;
        InternationalString	item;
    } *dbNames;
    OtherInformation	*additionalSearchInfo;
};

struct OriginPartNotToKeep {
    enum {
        e56_package, e56_query
    } which;
    union {
        InternationalString	package;
        Query	*query;
    } u;
};


struct PeriodicQuerySchedule {
    enum {
        e57_esRequest, e57_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    Boolean	activeFlag;
    struct databaseNames_List100 {
        struct databaseNames_List100	*next;
        InternationalString	item;
    } *databaseNames;
    int	resultSetDisposition;
    Destination	*alertDestination;
    struct {
        enum {
            e58_packageName, e58_exportPackage
        } which;
        union {
            InternationalString	packageName;
            ExportSpecification	*exportPackage;
        } u;
    } *exportParameters;
};

struct OriginPartNotToKeep {
    struct {
        enum {
            e59_actualQuery, e59_packageName
        } which;
        union {
            Query	*actualQuery;
            InternationalString	packageName;
        } u;
    } *querySpec;
    Period	*originSuggestedPeriod;
    GeneralizedTime	expiration;
    InternationalString	resultSetPackage;
};

struct TargetPart {
    Query	*actualQuery;
    Period	*targetStatedPeriod;
    GeneralizedTime	expiration;
    InternationalString	resultSetPackage;
    GeneralizedTime	lastQueryTime;
    int	lastResultNumber;
    int	numberSinceModify;
};

struct Period {
    enum {
        e60_unit, e60_businessDaily, e60_continuous, e60_other
    } which;
    union {
        IntUnit	*unit;
        unsigned char	businessDaily;
        unsigned char	continuous;
        InternationalString	other;
    } u;
};

struct ItemOrder {
    enum {
        e61_esRequest, e61_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    External	*supplDescription;
    struct contact {
        InternationalString	name;
        InternationalString	phone;
        InternationalString	email;
    } *contact;
    struct addlBilling {
        struct {
            enum {
                e62_billInvoice, e62_prepay, e62_depositAccount, e62_creditCard, e62_cardInfoPreviouslySupplied, e62_privateKnown, e62_privateNotKnown
            } which;
            union {
                unsigned char	billInvoice;
                unsigned char	prepay;
                unsigned char	depositAccount;
                CreditCardInfo	*creditCard;
                unsigned char	cardInfoPreviouslySupplied;
                unsigned char	privateKnown;
                External	*privateNotKnown;
            } u;
        } paymentMethod;
        InternationalString	customerReference;
        InternationalString	customerPONumber;
    } *addlBilling;
};

struct CreditCardInfo {
    InternationalString	nameOnCard;
    InternationalString	expirationDate;
    InternationalString	cardNumber;
};

struct OriginPartNotToKeep {
    struct resultSetItem {
        InternationalString	resultSetId;
        int	item;
    } *resultSetItem;
    External	*itemRequest;
};

struct TargetPart {
    External	*itemRequest;
    External	*statusOrErrorReport;
    int	auxiliaryStatus;
};

struct Update {
    enum {
        e63_esRequest, e63_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    int	action;
    InternationalString	databaseName;
    ObjectIdentifier	schema;
    InternationalString	elementSetName;
};


struct TargetPart {
    int	updateStatus;
    struct globalDiagnostics_List101 {
        struct globalDiagnostics_List101	*next;
        DiagRec	*item;
    } *globalDiagnostics;
    struct taskPackageRecords_List102 {
        struct taskPackageRecords_List102	*next;
        TaskPackageRecordStructure	*item;
    } *taskPackageRecords;
};

struct SuppliedRecords {
    struct SuppliedRecords	*next;
    struct {
        struct {
            enum {
                e64_number, e64_string, e64_opaque
            } which;
            union {
                int	number;
                InternationalString	string;
                OctetString	opaque;
            } u;
        } *recordId;
        struct {
            enum {
                e65_timeStamp, e65_versionNumber, e65_previousVersion
            } which;
            union {
                GeneralizedTime	timeStamp;
                InternationalString	versionNumber;
                External	*previousVersion;
            } u;
        } *supplementalId;
        CorrelationInfo	*correlationInfo;
        External	*record;
    } item;
};

struct CorrelationInfo {
    InternationalString	note;
    int	id;
};

struct TaskPackageRecordStructure {
    struct {
        enum {
            e66_record, e66_diagnostic
        } which;
        union {
            External	*record;
            DiagRec	*diagnostic;
        } u;
    } *recordOrSurDiag;
    CorrelationInfo	*correlationInfo;
    int	recordStatus;
};

struct ExportSpecification {
    enum {
        e67_esRequest, e67_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            unsigned char	notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            unsigned char	targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    CompSpec	*composition;
    Destination	*exportDestination;
};

struct Destination {
    enum {
        e68_phoneNumber, e68_faxNumber, e68_x400address, e68_emailAddress, e68_pagerNumber, e68_ftpAddress, e68_ftamAddress, e68_printerAddress, e68_other
    } which;
    union {
        InternationalString	phoneNumber;
        InternationalString	faxNumber;
        InternationalString	x400address;
        InternationalString	emailAddress;
        InternationalString	pagerNumber;
        InternationalString	ftpAddress;
        InternationalString	ftamAddress;
        InternationalString	printerAddress;
        struct other {
            InternationalString	vehicle;
            InternationalString	destination;
        } other;
    } u;
};

struct ExportInvocation {
    enum {
        e69_esRequest, e69_taskPackage
    } which;
    union {
        struct esRequest {
            OriginPartToKeep	*toKeep;
            OriginPartNotToKeep	*notToKeep;
        } esRequest;
        struct taskPackage {
            OriginPartToKeep	*originPart;
            TargetPart	*targetPart;
        } taskPackage;
    } u;
};

struct OriginPartToKeep {
    struct {
        enum {
            e70_packageName, e70_packageSpec
        } which;
        union {
            InternationalString	packageName;
            ExportSpecification	*packageSpec;
        } u;
    } exportSpec;
    int	numberOfCopies;
};

struct OriginPartNotToKeep {
    InternationalString	resultSetId;
    struct {
        enum {
            e71_all, e71_ranges
        } which;
        union {
            unsigned char	all;
            struct ranges_List103 {
                struct ranges_List103	*next;
                struct {
                    int	start;
                    int	count;
                } item;
            } *ranges;
        } u;
    } records;
};

struct TargetPart {
    IntUnit	*estimatedQuantity;
    IntUnit	*quantitySoFar;
    IntUnit	*estimatedCost;
    IntUnit	*costSoFar;
};

struct SearchInfoReport {
    struct SearchInfoReport	*next;
    struct {
        InternationalString	subqueryId;
        Boolean	fullQuery;
        QueryExpression	*subqueryExpression;
        QueryExpression	*subqueryInterpretation;
        QueryExpression	*subqueryRecommendation;
        int	subqueryCount;
        IntUnit	*subqueryWeight;
        ResultsByDB	*resultsByDB;
    } item;
};

struct ResultsByDB {
    struct ResultsByDB	*next;
    struct {
        struct {
            enum {
                e72_all, e72_list
            } which;
            union {
                unsigned char	all;
                struct list_List104 {
                    struct list_List104	*next;
                    DatabaseName	item;
                } *list;
            } u;
        } databases;
        int	count;
        InternationalString	resultSetName;
    } item;
};

struct QueryExpression {
    enum {
        e73_term, e73_query
    } which;
    union {
        struct term {
            Term	*queryTerm;
            InternationalString	termComment;
        } term;
        Query	*query;
    } u;
};

struct Espec_1 {
    struct elementSetNames_List105 {
        struct elementSetNames_List105	*next;
        InternationalString	item;
    } *elementSetNames;
    ObjectIdentifier	defaultVariantSetId;
    Variant	*defaultVariantRequest;
    int	defaultTagType;
    struct elements_List106 {
        struct elements_List106	*next;
        ElementRequest	*item;
    } *elements;
};

struct ElementRequest {
    enum {
        e74_simpleElement, e74_compositeElement
    } which;
    union {
        SimpleElement	*simpleElement;
        struct compositeElement {
            struct {
                enum {
                    e75_primitives, e75_specs
                } which;
                union {
                    struct primitives_List107 {
                        struct primitives_List107	*next;
                        InternationalString	item;
                    } *primitives;
                    struct specs_List108 {
                        struct specs_List108	*next;
                        SimpleElement	*item;
                    } *specs;
                } u;
            } elementList;
            TagPath	*deliveryTag;
            Variant	*variantRequest;
        } compositeElement;
    } u;
};

struct SimpleElement {
    TagPath	*path;
    Variant	*variantRequest;
};

struct TagPath {
    struct TagPath	*next;
    struct {
        enum {
            e76_specificTag, e76_wildThing, e76_wildPath
        } which;
        union {
            struct specificTag {
                int	tagType;
                StringOrNumeric	*tagValue;
                Occurrences	*occurrence;
            } specificTag;
            Occurrences	*wildThing;
            unsigned char	wildPath;
        } u;
    } *item;
};
*/
struct Occurrences {
    enum {
        e77_all, e77_last, e77_values
    } which;
    union {
        unsigned char	all;
        unsigned char	last;
        struct values {
            int	start;
            int	howMany;
        } values;
    } u;
};
ASN_NODE *EncodePDU(PDU *the_PDU);
ASN_NODE *EncodeInitializeRequest(int class_var, int type, int tagno, InitializeRequest *the_InitializeRequest);
ASN_NODE *EncodeInitializeResponse(int class_var, int type, int tagno, InitializeResponse *the_InitializeResponse);
ASN_NODE *EncodeSearchRequest(int class_var, int type, int tagno, SearchRequest *the_SearchRequest);
ASN_NODE *EncodeQuery(int class_var, int type, int tagno, Query *the_Query);
ASN_NODE *EncodeRPNQuery(int class_var, int type, int tagno, RPNQuery *the_RPNQuery);
ASN_NODE *EncodeRPNStructure(int class_var, int type, int tagno, RPNStructure *the_RPNStructure);
ASN_NODE *EncodeOperand(int class_var, int type, int tagno, Operand *the_Operand);
ASN_NODE *EncodeAttributesPlusTerm(int class_var, int type, int tagno, AttributesPlusTerm *the_AttributesPlusTerm);
ASN_NODE *EncodeResultSetPlusAttributes(int class_var, int type, int tagno, ResultSetPlusAttributes *the_ResultSetPlusAttributes);
ASN_NODE *EncodeAttributeList(int class_var, int type, int tagno, AttributeList *the_AttributeList);
ASN_NODE *EncodeTerm(int class_var, int type, int tagno, Term *the_Term);
ASN_NODE *EncodeOperator(int class_var, int type, int tagno, Operator *the_Operator);
ASN_NODE *EncodeAttributeElement(int class_var, int type, int tagno, AttributeElement *the_AttributeElement);
ASN_NODE *EncodeProximityOperator(int class_var, int type, int tagno, ProximityOperator *the_ProximityOperator);
ASN_NODE *EncodeSearchResponse(int class_var, int type, int tagno, SearchResponse *the_SearchResponse);
ASN_NODE *EncodePresentRequest(int class_var, int type, int tagno, PresentRequest *the_PresentRequest);
ASN_NODE *EncodeSegment(int class_var, int type, int tagno, Segment *the_Segment);
ASN_NODE *EncodePresentResponse(int class_var, int type, int tagno, PresentResponse *the_PresentResponse);
ASN_NODE *EncodeRecords(int class_var, int type, int tagno, Records *the_Records);
ASN_NODE *EncodeNamePlusRecord(int class_var, int type, int tagno, NamePlusRecord *the_NamePlusRecord);
ASN_NODE *EncodeFragmentSyntax(int class_var, int type, int tagno, FragmentSyntax *the_FragmentSyntax);
ASN_NODE *EncodeDiagRec(int class_var, int type, int tagno, DiagRec *the_DiagRec);
ASN_NODE *EncodeDefaultDiagFormat(int class_var, int type, int tagno, DefaultDiagFormat *the_DefaultDiagFormat);
ASN_NODE *EncodeRange(int class_var, int type, int tagno, Range *the_Range);
ASN_NODE *EncodeElementSetNames(int class_var, int type, int tagno, ElementSetNames *the_ElementSetNames);
ASN_NODE *EncodeCompSpec(int class_var, int type, int tagno, CompSpec *the_CompSpec);
ASN_NODE *EncodeSpecification(int class_var, int type, int tagno, Specification *the_Specification);
ASN_NODE *EncodeDeleteResultSetRequest(int class_var, int type, int tagno, DeleteResultSetRequest *the_DeleteResultSetRequest);
ASN_NODE *EncodeDeleteResultSetResponse(int class_var, int type, int tagno, DeleteResultSetResponse *the_DeleteResultSetResponse);
ASN_NODE *EncodeListStatuses(int class_var, int type, int tagno, ListStatuses *the_ListStatuses);
ASN_NODE *EncodeAccessControlRequest(int class_var, int type, int tagno, AccessControlRequest *the_AccessControlRequest);
ASN_NODE *EncodeAccessControlResponse(int class_var, int type, int tagno, AccessControlResponse *the_AccessControlResponse);
ASN_NODE *EncodeResourceControlRequest(int class_var, int type, int tagno, ResourceControlRequest *the_ResourceControlRequest);
ASN_NODE *EncodeResourceControlResponse(int class_var, int type, int tagno, ResourceControlResponse *the_ResourceControlResponse);
ASN_NODE *EncodeTriggerResourceControlRequest(int class_var, int type, int tagno, TriggerResourceControlRequest *the_TriggerResourceControlRequest);
ASN_NODE *EncodeResourceReportRequest(int class_var, int type, int tagno, ResourceReportRequest *the_ResourceReportRequest);
ASN_NODE *EncodeResourceReportResponse(int class_var, int type, int tagno, ResourceReportResponse *the_ResourceReportResponse);
ASN_NODE *EncodeScanRequest(int class_var, int type, int tagno, ScanRequest *the_ScanRequest);
ASN_NODE *EncodeScanResponse(int class_var, int type, int tagno, ScanResponse *the_ScanResponse);
ASN_NODE *EncodeListEntries(int class_var, int type, int tagno, ListEntries *the_ListEntries);
ASN_NODE *EncodeEntry(int class_var, int type, int tagno, Entry *the_Entry);
ASN_NODE *EncodeTermInfo(int class_var, int type, int tagno, TermInfo *the_TermInfo);
ASN_NODE *EncodeOccurrenceByAttributes(int class_var, int type, int tagno, OccurrenceByAttributes *the_OccurrenceByAttributes);
ASN_NODE *EncodeSortRequest(int class_var, int type, int tagno, SortRequest *the_SortRequest);
ASN_NODE *EncodeSortResponse(int class_var, int type, int tagno, SortResponse *the_SortResponse);
ASN_NODE *EncodeSortKeySpec(int class_var, int type, int tagno, SortKeySpec *the_SortKeySpec);
ASN_NODE *EncodeSortElement(int class_var, int type, int tagno, SortElement *the_SortElement);
ASN_NODE *EncodeSortKey(int class_var, int type, int tagno, SortKey *the_SortKey);
ASN_NODE *EncodeExtendedServicesRequest(int class_var, int type, int tagno, ExtendedServicesRequest *the_ExtendedServicesRequest);
ASN_NODE *EncodeExtendedServicesResponse(int class_var, int type, int tagno, ExtendedServicesResponse *the_ExtendedServicesResponse);
ASN_NODE *EncodePermissions(int class_var, int type, int tagno, Permissions *the_Permissions);
ASN_NODE *EncodeClose(int class_var, int type, int tagno, Close *the_Close);
ASN_NODE *EncodeOtherInformation(int class_var, int type, int tagno, OtherInformation *the_OtherInformation);
ASN_NODE *EncodeInfoCategory(int class_var, int type, int tagno, InfoCategory *the_InfoCategory);
ASN_NODE *EncodeIntUnit(int class_var, int type, int tagno, IntUnit *the_IntUnit);
ASN_NODE *EncodeUnit(int class_var, int type, int tagno, Unit *the_Unit);
ASN_NODE *EncodeStringOrNumeric(int class_var, int type, int tagno, StringOrNumeric *the_StringOrNumeric);
ASN_NODE *EncodeDiagnosticFormat(int class_var, int type, int tagno, DiagnosticFormat *the_DiagnosticFormat);
ASN_NODE *EncodeDiagFormat(int class_var, int type, int tagno, DiagFormat *the_DiagFormat);
ASN_NODE *EncodeExplain_Record(int class_var, int type, int tagno, Explain_Record *the_Explain_Record);
ASN_NODE *EncodeTargetInfo(int class_var, int type, int tagno, TargetInfo *the_TargetInfo);
ASN_NODE *EncodeDatabaseInfo(int class_var, int type, int tagno, DatabaseInfo *the_DatabaseInfo);
ASN_NODE *EncodeSchemaInfo(int class_var, int type, int tagno, SchemaInfo *the_SchemaInfo);
ASN_NODE *EncodeElementInfo(int class_var, int type, int tagno, ElementInfo *the_ElementInfo);
ASN_NODE *EncodePath(int class_var, int type, int tagno, Path *the_Path);
ASN_NODE *EncodeElementDataType(int class_var, int type, int tagno, ElementDataType *the_ElementDataType);
ASN_NODE *EncodeTagSetInfo(int class_var, int type, int tagno, TagSetInfo *the_TagSetInfo);
ASN_NODE *EncodeRecordSyntaxInfo(int class_var, int type, int tagno, RecordSyntaxInfo *the_RecordSyntaxInfo);
ASN_NODE *EncodeAttributeSetInfo(int class_var, int type, int tagno, AttributeSetInfo *the_AttributeSetInfo);
ASN_NODE *EncodeAttributeType(int class_var, int type, int tagno, AttributeType *the_AttributeType);
ASN_NODE *EncodeAttributeDescription(int class_var, int type, int tagno, AttributeDescription *the_AttributeDescription);
ASN_NODE *EncodeTermListInfo(int class_var, int type, int tagno, TermListInfo *the_TermListInfo);
ASN_NODE *EncodeExtendedServicesInfo(int class_var, int type, int tagno, ExtendedServicesInfo *the_ExtendedServicesInfo);
ASN_NODE *EncodeAttributeDetails(int class_var, int type, int tagno, AttributeDetails *the_AttributeDetails);
ASN_NODE *EncodeAttributeSetDetails(int class_var, int type, int tagno, AttributeSetDetails *the_AttributeSetDetails);
ASN_NODE *EncodeAttributeTypeDetails(int class_var, int type, int tagno, AttributeTypeDetails *the_AttributeTypeDetails);
ASN_NODE *EncodeOmittedAttributeInterpretation(int class_var, int type, int tagno, OmittedAttributeInterpretation *the_OmittedAttributeInterpretation);
ASN_NODE *EncodeAttributeValue(int class_var, int type, int tagno, AttributeValue *the_AttributeValue);
ASN_NODE *EncodeTermListDetails(int class_var, int type, int tagno, TermListDetails *the_TermListDetails);
ASN_NODE *EncodeElementSetDetails(int class_var, int type, int tagno, ElementSetDetails *the_ElementSetDetails);
ASN_NODE *EncodeRetrievalRecordDetails(int class_var, int type, int tagno, RetrievalRecordDetails *the_RetrievalRecordDetails);
ASN_NODE *EncodePerElementDetails(int class_var, int type, int tagno, PerElementDetails *the_PerElementDetails);
ASN_NODE *EncodeRecordTag(int class_var, int type, int tagno, RecordTag *the_RecordTag);
ASN_NODE *EncodeSortDetails(int class_var, int type, int tagno, SortDetails *the_SortDetails);
ASN_NODE *EncodeSortKeyDetails(int class_var, int type, int tagno, SortKeyDetails *the_SortKeyDetails);
ASN_NODE *EncodeProcessingInformation(int class_var, int type, int tagno, ProcessingInformation *the_ProcessingInformation);
ASN_NODE *EncodeVariantSetInfo(int class_var, int type, int tagno, VariantSetInfo *the_VariantSetInfo);
ASN_NODE *EncodeVariantClass(int class_var, int type, int tagno, VariantClass *the_VariantClass);
ASN_NODE *EncodeVariantType(int class_var, int type, int tagno, VariantType *the_VariantType);
ASN_NODE *EncodeVariantValue(int class_var, int type, int tagno, VariantValue *the_VariantValue);
ASN_NODE *EncodeValueSet(int class_var, int type, int tagno, ValueSet *the_ValueSet);
ASN_NODE *EncodeValueRange(int class_var, int type, int tagno, ValueRange *the_ValueRange);
ASN_NODE *EncodeValueDescription(int class_var, int type, int tagno, ValueDescription *the_ValueDescription);
ASN_NODE *EncodeUnitInfo(int class_var, int type, int tagno, UnitInfo *the_UnitInfo);
ASN_NODE *EncodeUnitType(int class_var, int type, int tagno, UnitType *the_UnitType);
ASN_NODE *EncodeUnits(int class_var, int type, int tagno, Units *the_Units);
ASN_NODE *EncodeCategoryList(int class_var, int type, int tagno, CategoryList *the_CategoryList);
ASN_NODE *EncodeCategoryInfo(int class_var, int type, int tagno, CategoryInfo *the_CategoryInfo);
ASN_NODE *EncodeCommonInfo(int class_var, int type, int tagno, CommonInfo *the_CommonInfo);
ASN_NODE *EncodeHumanString(int class_var, int type, int tagno, HumanString *the_HumanString);
ASN_NODE *EncodeIconObject(int class_var, int type, int tagno, IconObject *the_IconObject);
ASN_NODE *EncodeContactInfo(int class_var, int type, int tagno, ContactInfo *the_ContactInfo);
ASN_NODE *EncodeNetworkAddress(int class_var, int type, int tagno, NetworkAddress *the_NetworkAddress);
ASN_NODE *EncodeAccessInfo(int class_var, int type, int tagno, AccessInfo *the_AccessInfo);
ASN_NODE *EncodeQueryTypeDetails(int class_var, int type, int tagno, QueryTypeDetails *the_QueryTypeDetails);
ASN_NODE *EncodePrivateCapabilities(int class_var, int type, int tagno, PrivateCapabilities *the_PrivateCapabilities);
ASN_NODE *EncodeRpnCapabilities(int class_var, int type, int tagno, RpnCapabilities *the_RpnCapabilities);
ASN_NODE *EncodeIso8777Capabilities(int class_var, int type, int tagno, Iso8777Capabilities *the_Iso8777Capabilities);
ASN_NODE *EncodeProximitySupport(int class_var, int type, int tagno, ProximitySupport *the_ProximitySupport);
ASN_NODE *EncodeSearchKey(int class_var, int type, int tagno, SearchKey *the_SearchKey);
ASN_NODE *EncodeAccessRestrictions(int class_var, int type, int tagno, AccessRestrictions *the_AccessRestrictions);
ASN_NODE *EncodeCosts(int class_var, int type, int tagno, Costs *the_Costs);
ASN_NODE *EncodeCharge(int class_var, int type, int tagno, Charge *the_Charge);
ASN_NODE *EncodeDatabaseList(int class_var, int type, int tagno, DatabaseList *the_DatabaseList);
ASN_NODE *EncodeAttributeCombinations(int class_var, int type, int tagno, AttributeCombinations *the_AttributeCombinations);
ASN_NODE *EncodeAttributeCombination(int class_var, int type, int tagno, AttributeCombination *the_AttributeCombination);
ASN_NODE *EncodeAttributeOccurrence(int class_var, int type, int tagno, AttributeOccurrence *the_AttributeOccurrence);
ASN_NODE *EncodeOPACRecord(int class_var, int type, int tagno, OPACRecord *the_OPACRecord);
ASN_NODE *EncodeHoldingsRecord(int class_var, int type, int tagno, HoldingsRecord *the_HoldingsRecord);
ASN_NODE *EncodeHoldingsAndCircData(int class_var, int type, int tagno, HoldingsAndCircData *the_HoldingsAndCircData);
ASN_NODE *EncodeVolume(int class_var, int type, int tagno, Volume *the_Volume);
ASN_NODE *EncodeCircRecord(int class_var, int type, int tagno, CircRecord *the_CircRecord);
ASN_NODE *EncodeBriefBib(int class_var, int type, int tagno, BriefBib *the_BriefBib);
ASN_NODE *EncodeFormatSpec(int class_var, int type, int tagno, FormatSpec *the_FormatSpec);
ASN_NODE *EncodeGenericRecord(int class_var, int type, int tagno, GenericRecord *the_GenericRecord);
ASN_NODE *EncodeTaggedElement(int class_var, int type, int tagno, TaggedElement *the_TaggedElement);
ASN_NODE *EncodeElementData(int class_var, int type, int tagno, ElementData *the_ElementData);
ASN_NODE *EncodeElementMetaData(int class_var, int type, int tagno, ElementMetaData *the_ElementMetaData);
ASN_NODE *EncodeTagPath(int class_var, int type, int tagno, TagPath *the_TagPath);
ASN_NODE *EncodeOrder(int class_var, int type, int tagno, Order *the_Order);
ASN_NODE *EncodeUsage(int class_var, int type, int tagno, Usage *the_Usage);
ASN_NODE *EncodeHitVector(int class_var, int type, int tagno, HitVector *the_HitVector);
ASN_NODE *EncodeVariant(int class_var, int type, int tagno, Variant *the_Variant);
ASN_NODE *EncodeTaskPackage(int class_var, int type, int tagno, TaskPackage *the_TaskPackage);
ASN_NODE *EncodeEstimate(int class_var, int type, int tagno, Estimate *the_Estimate);
ASN_NODE *EncodeEstimate(int class_var, int type, int tagno, Estimate *the_Estimate);
ASN_NODE *EncodePromptObject(int class_var, int type, int tagno, PromptObject *the_PromptObject);
ASN_NODE *EncodeChallenge(int class_var, int type, int tagno, Challenge *the_Challenge);
ASN_NODE *EncodeResponse(int class_var, int type, int tagno, Response *the_Response);
ASN_NODE *EncodePromptId(int class_var, int type, int tagno, PromptId *the_PromptId);
ASN_NODE *EncodeEncryption(int class_var, int type, int tagno, Encryption *the_Encryption);
ASN_NODE *EncodeDES_RN_Object(int class_var, int type, int tagno, DES_RN_Object *the_DES_RN_Object);
ASN_NODE *EncodeDRNType(int class_var, int type, int tagno, DRNType *the_DRNType);
ASN_NODE *EncodeKRBObject(int class_var, int type, int tagno, KRBObject *the_KRBObject);
ASN_NODE *EncodeKRBRequest(int class_var, int type, int tagno, KRBRequest *the_KRBRequest);
ASN_NODE *EncodeKRBResponse(int class_var, int type, int tagno, KRBResponse *the_KRBResponse);
ASN_NODE *EncodePersistentResultSet(int class_var, int type, int tagno, PersistentResultSet *the_PersistentResultSet);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno, OriginPartNotToKeep *the_OriginPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno, TargetPart *the_TargetPart);
ASN_NODE *EncodePersistentQuery(int class_var, int type, int tagno, PersistentQuery *the_PersistentQuery);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno, OriginPartNotToKeep *the_OriginPartNotToKeep);
ASN_NODE *EncodePeriodicQuerySchedule(int class_var, int type, int tagno, PeriodicQuerySchedule *the_PeriodicQuerySchedule);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno, OriginPartNotToKeep *the_OriginPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno, TargetPart *the_TargetPart);
ASN_NODE *EncodePeriod(int class_var, int type, int tagno, Period *the_Period);
ASN_NODE *EncodeItemOrder(int class_var, int type, int tagno, ItemOrder *the_ItemOrder);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeCreditCardInfo(int class_var, int type, int tagno, CreditCardInfo *the_CreditCardInfo);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno, OriginPartNotToKeep *the_OriginPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno, TargetPart *the_TargetPart);
ASN_NODE *EncodeUpdate(int class_var, int type, int tagno, Update *the_Update);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno, TargetPart *the_TargetPart);
ASN_NODE *EncodeSuppliedRecords(int class_var, int type, int tagno, SuppliedRecords *the_SuppliedRecords);
ASN_NODE *EncodeCorrelationInfo(int class_var, int type, int tagno, CorrelationInfo *the_CorrelationInfo);
ASN_NODE *EncodeTaskPackageRecordStructure(int class_var, int type, int tagno, TaskPackageRecordStructure *the_TaskPackageRecordStructure);
ASN_NODE *EncodeExportSpecification(int class_var, int type, int tagno, ExportSpecification *the_ExportSpecification);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeDestination(int class_var, int type, int tagno, Destination *the_Destination);
ASN_NODE *EncodeExportInvocation(int class_var, int type, int tagno, ExportInvocation *the_ExportInvocation);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno, OriginPartToKeep *the_OriginPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno, OriginPartNotToKeep *the_OriginPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno, TargetPart *the_TargetPart);
ASN_NODE *EncodeSearchInfoReport(int class_var, int type, int tagno, SearchInfoReport *the_SearchInfoReport);
ASN_NODE *EncodeResultsByDB(int class_var, int type, int tagno, ResultsByDB *the_ResultsByDB);
ASN_NODE *EncodeQueryExpression(int class_var, int type, int tagno, QueryExpression *the_QueryExpression);
ASN_NODE *EncodeEspec_1(int class_var, int type, int tagno, Espec_1 *the_Espec_1);
ASN_NODE *EncodeElementRequest(int class_var, int type, int tagno, ElementRequest *the_ElementRequest);
ASN_NODE *EncodeSimpleElement(int class_var, int type, int tagno, SimpleElement *the_SimpleElement);
ASN_NODE *EncodeTagPath(int class_var, int type, int tagno, TagPath *the_TagPath);
ASN_NODE *EncodeOccurrences(int class_var, int type, int tagno, Occurrences *the_Occurrences);

#include "berintern.h"
#include "asn_decode.h"
#include "session.h"
#include "zinter.h"

#endif  /* Z3950_2_H    */


