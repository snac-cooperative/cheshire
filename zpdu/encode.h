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

ASN_NODE *EncodeInternationalString(int class_var, int tagno, 
				    InternationalString obj);
ASN_NODE *EncodeInternationalString(int class_var, int tagno, 
				    InternationalString internationalString);
ASN_NODE *EncodeLanguageCode(int class_var, int tagno, 
			     InternationalString internationalString);
ASN_NODE *EncodeProtocolVersion(int class_var, int type, int tagno, 
				ProtocolVersion protocolVersion);
ASN_NODE *EncodeOptions(int class_var, int type, int tagno, Options options);
ASN_NODE *EncodeReferenceId(int class_var, int type, int tagno, 
			    ReferenceId referenceId);
ASN_NODE *EncodeResultSetId(int class_var, int type, int tagno, 
			    ResultSetId resultSetId);
ASN_NODE *EncodeElementSetName(int class_var, int type, int tagno, 
			       ElementSetName elementSetName);
ASN_NODE *EncodeDatabaseName(int class_var, int type, int tagno, 
			     DatabaseName databaseName);
ASN_NODE *EncodeAttributeSetId(int class_var, int type, int tagno, 
			       AttributeSetId attributeSetId);
ASN_NODE *EncodePDU(PDU *pDU);
ASN_NODE *EncodeInitializeRequest(int class_var, int type, int tagno, 
				  InitializeRequest *initializeRequest);
ASN_NODE *EncodeInitializeResponse(int class_var, int type, int tagno, 
				   InitializeResponse *initializeResponse);
ASN_NODE *EncodeSearchRequest(int class_var, int type, int tagno, 
			      SearchRequest *searchRequest);
ASN_NODE *EncodeQuery(int class_var, int type, int tagno, Query *query);
ASN_NODE *EncodeRPNQuery(int class_var, int type, int tagno, RPNQuery *rPNQuery);
ASN_NODE *EncodeRPNStructure(int class_var, int type, int tagno, 
			     RPNStructure *rPNStructure);
ASN_NODE *EncodeOperand(int class_var, int type, int tagno, Operand *operand);
ASN_NODE *EncodeAttributesPlusTerm(int class_var, int type, int tagno, 
				   AttributesPlusTerm *attributesPlusTerm);
ASN_NODE *EncodeResultSetPlusAttributes(int class_var, int type, int tagno, 
					ResultSetPlusAttributes 
					*resultSetPlusAttributes);
ASN_NODE *EncodeAttributeList(int class_var, int type, int tagno,
			      AttributeList *attributeList);
ASN_NODE *EncodeTerm(int class_var, int type, int tagno, Term *term);
ASN_NODE *EncodeOperator(int class_var, int type, int tagno, Operator *operator_var);
ASN_NODE *EncodeAttributeElement(int class_var, int type, int tagno, 
				 AttributeElement *attributeElement);
ASN_NODE *EncodeKnownProximityUnit(int class_var, int type, int tagno, 
				   KnownProximityUnit knownProximityUnit);
ASN_NODE *EncodeProximityOperator(int class_var, int type, int tagno, 
				  ProximityOperator *proximityOperator);
ASN_NODE *EncodePresentStatus(int class_var, int type, int tagno, 
			      PresentStatus presentStatus);
ASN_NODE *EncodeSearchResponse(int class_var, int type, int tagno, 
			       SearchResponse *searchResponse);
ASN_NODE *EncodePresentRequest(int class_var, int type, int tagno, 
			       PresentRequest *presentRequest);
ASN_NODE *EncodeSegment(int class_var, int type, int tagno, 
			Segment *segment);
ASN_NODE *EncodePresentResponse(int class_var, int type, int tagno, 
				PresentResponse *presentResponse);
ASN_NODE *EncodeRecords(int class_var, int type, int tagno, Records *records);
ASN_NODE *EncodeNamePlusRecord(int class_var, int type, int tagno, 
			       NamePlusRecord *namePlusRecord);
ASN_NODE *EncodeFragmentSyntax(int class_var, int type, int tagno, 
			       FragmentSyntax *fragmentSyntax);
ASN_NODE *EncodeDiagRec(int class_var, int type, int tagno, DiagRec *diagRec);
ASN_NODE *EncodeDefaultDiagFormat(int class_var, int type, int tagno, 
				  DefaultDiagFormat *defaultDiagFormat);
ASN_NODE *EncodeRange(int class_var, int type, int tagno, Range *range);
ASN_NODE *EncodeElementSetNames(int class_var, int type, int tagno, 
				ElementSetNames *elementSetNames);
ASN_NODE *EncodeCompSpec(int class_var, int type, int tagno, CompSpec *compSpec);
ASN_NODE *EncodeSpecification(int class_var, int type, int tagno, 
			      Specification *specification);
ASN_NODE *EncodeDeleteResultSetRequest(int class_var, int type, int tagno, 
				       DeleteResultSetRequest 
				       *deleteResultSetRequest);
ASN_NODE *EncodeDeleteResultSetResponse(int class_var, int type, int tagno, 
					DeleteResultSetResponse 
					*deleteResultSetResponse);
ASN_NODE *EncodeListStatuses(int class_var, int type, int tagno, 
			     ListStatuses *listStatuses);
ASN_NODE *EncodeDeleteSetStatus(int class_var, int type, int tagno, 
				DeleteSetStatus deleteSetStatus);
ASN_NODE *EncodeAccessControlRequest(int class_var, int type, int tagno, 
				     AccessControlRequest *accessControlRequest);
ASN_NODE *EncodeAccessControlResponse(int class_var, int type, int tagno, 
				      AccessControlResponse 
				      *accessControlResponse);
ASN_NODE *EncodeResourceControlRequest(int class_var, int type, int tagno, 
				       ResourceControlRequest 
				       *resourceControlRequest);
ASN_NODE *EncodeResourceControlResponse(int class_var, int type, int tagno, 
					ResourceControlResponse 
					*resourceControlResponse);
ASN_NODE *EncodeTriggerResourceControlRequest(int class_var, int type, int tagno, 
					      TriggerResourceControlRequest 
					      *triggerResourceControlRequest);
ASN_NODE *EncodeResourceReportRequest(int class_var, int type, int tagno, 
				      ResourceReportRequest 
				      *resourceReportRequest);
ASN_NODE *EncodeResourceReportResponse(int class_var, int type, int tagno, 
				       ResourceReportResponse 
				       *resourceReportResponse);
ASN_NODE *EncodeOtherInformation(int class_var, int type, int tagno, 
				 OtherInformation *otherInformation);
ASN_NODE *EncodeInfoCategory(int class_var, int type, int tagno, 
			     InfoCategory *infoCategory);
ASN_NODE *EncodeStringOrNumeric(int class_var, int type, int tagno, 
				StringOrNumeric *stringOrNumeric);
ASN_NODE *EncodeResourceReport(int class_var, int type, int tagno, 
			       External *resourceReport);
ASN_NODE *EncodeResourceReportId(int class_var, int type, int tagno, 
				 ResourceReportId resourceReportId);
ASN_NODE *EncodeIntUnit(int class_var, int type, int tagno, IntUnit *intUnit);
ASN_NODE *EncodeUnit(int class_var, int type, int tagno, Unit *unit);
ASN_NODE *EncodeScanRequest(int class_var, int type, int tagno, 
			    ScanRequest *scanRequest);
ASN_NODE *EncodeScanResponse(int class_var, int type, int tagno, 
			     ScanResponse *scanResponse);
ASN_NODE *EncodeListEntries(int class_var, int type, int tagno, 
			    ListEntries *listEntries);
ASN_NODE *EncodeEntry(int class_var, int type, int tagno, Entry *entry);
ASN_NODE *EncodeTermInfo(int class_var, int type, int tagno, TermInfo *termInfo);
ASN_NODE *EncodeOccurrenceByAttributes(int class_var, int type, int tagno, 
				       OccurrenceByAttributes 
				       *occurrenceByAttributes);
ASN_NODE *EncodeSortRequest(int class_var, int type, int tagno, 
			    SortRequest *sortRequest);
ASN_NODE *EncodeSortResponse(int class_var, int type, int tagno, 
			     SortResponse *sortResponse);
ASN_NODE *EncodeSortKeySpec(int class_var, int type, int tagno, 
			    SortKeySpec *sortKeySpec);
ASN_NODE *EncodeSortElement(int class_var, int type, int tagno, 
			    SortElement *sortElement);
ASN_NODE *EncodeSortKey(int class_var, int type, int tagno, SortKey *sortKey);
ASN_NODE *EncodeExtendedServicesRequest(int class_var, int type, int tagno, 
					ExtendedServicesRequest 
					*extendedServicesRequest);
ASN_NODE *EncodeExtendedServicesResponse(int class_var, int type, int tagno, 
					 ExtendedServicesResponse 
					 *extendedServicesResponse);
ASN_NODE *EncodePermissions(int class_var, int type, int tagno, 
			    Permissions *permissions);
ASN_NODE *EncodeClose(int class_var, int type, int tagno, Close *close);
ASN_NODE *EncodeCloseReason(int class_var, int type, int tagno, 
			    CloseReason closeReason);
ASN_NODE *EncodeDiagnosticFormat(int class_var, int type, int tagno, 
				 DiagnosticFormat *diagnosticFormat);
ASN_NODE *EncodeDiagFormat(int class_var, int type, int tagno, 
			   DiagFormat *diagFormat);
ASN_NODE *EncodeExplain_Record(int class_var, int type, int tagno, 
			       Explain_Record *explain_Record);
ASN_NODE *EncodeTargetInfo(int class_var, int type, int tagno, 
			   TargetInfo *targetInfo);
ASN_NODE *EncodeDatabaseInfo(int class_var, int type, int tagno, 
			     DatabaseInfo *databaseInfo);
ASN_NODE *EncodeSchemaInfo(int class_var, int type, int tagno, 
			   SchemaInfo *schemaInfo);
ASN_NODE *EncodeElementInfo(int class_var, int type, int tagno, 
			    ElementInfo *elementInfo);
ASN_NODE *EncodePath(int class_var, int type, int tagno, Path *path);
ASN_NODE *EncodeElementDataType(int class_var, int type, int tagno, 
				ElementDataType *elementDataType);
ASN_NODE *EncodePrimitiveDataType(int class_var, int type, int tagno, 
				  PrimitiveDataType primitiveDataType);
ASN_NODE *EncodeTagSetInfo(int class_var, int type, int tagno, 
			   TagSetInfo *tagSetInfo);
ASN_NODE *EncodeRecordSyntaxInfo(int class_var, int type, int tagno,
RecordSyntaxInfo *recordSyntaxInfo);
ASN_NODE *EncodeAttributeSetInfo(int class_var, int type, int tagno,
AttributeSetInfo *attributeSetInfo);
ASN_NODE *EncodeAttributeType(int class_var, int type, int tagno,
AttributeType *attributeType);
ASN_NODE *EncodeAttributeDescription(int class_var, int type, int tagno,
AttributeDescription *attributeDescription);
ASN_NODE *EncodeTermListInfo(int class_var, int type, int tagno,
TermListInfo *termListInfo);
ASN_NODE *EncodeExtendedServicesInfo(int class_var, int type, int tagno,
ExtendedServicesInfo *extendedServicesInfo);
ASN_NODE *EncodeAttributeDetails(int class_var, int type, int tagno,
AttributeDetails *attributeDetails);
ASN_NODE *EncodeAttributeSetDetails(int class_var, int type, int tagno,
AttributeSetDetails *attributeSetDetails);
ASN_NODE *EncodeAttributeTypeDetails(int class_var, int type, int tagno,
AttributeTypeDetails *attributeTypeDetails);
ASN_NODE *EncodeOmittedAttributeInterpretation(int class_var, int type, int tagno,
OmittedAttributeInterpretation *omittedAttributeInterpretation);
ASN_NODE *EncodeAttributeValue(int class_var, int type, int tagno,
AttributeValue *attributeValue);
ASN_NODE *EncodeTermListDetails(int class_var, int type, int tagno,
TermListDetails *termListDetails);
ASN_NODE *EncodeElementSetDetails(int class_var, int type, int tagno,
ElementSetDetails *elementSetDetails);
ASN_NODE *EncodeRetrievalRecordDetails(int class_var, int type, int tagno,
RetrievalRecordDetails *retrievalRecordDetails);
ASN_NODE *EncodePerElementDetails(int class_var, int type, int tagno,
PerElementDetails *perElementDetails);
ASN_NODE *EncodeRecordTag(int class_var, int type, int tagno,
RecordTag *recordTag);
ASN_NODE *EncodeSortDetails(int class_var, int type, int tagno,
SortDetails *sortDetails);
ASN_NODE *EncodeSortKeyDetails(int class_var, int type, int tagno,
SortKeyDetails *sortKeyDetails);
ASN_NODE *EncodeProcessingInformation(int class_var, int type, int tagno,
ProcessingInformation *processingInformation);
ASN_NODE *EncodeVariantSetInfo(int class_var, int type, int tagno,
VariantSetInfo *variantSetInfo);
ASN_NODE *EncodeVariantClass(int class_var, int type, int tagno,
VariantClass *variantClass);
ASN_NODE *EncodeVariantType(int class_var, int type, int tagno,
VariantType *variantType);
ASN_NODE *EncodeVariantValue(int class_var, int type, int tagno,
VariantValue *variantValue);
ASN_NODE *EncodeValueSet(int class_var, int type, int tagno,
ValueSet *valueSet);
ASN_NODE *EncodeValueRange(int class_var, int type, int tagno,
ValueRange *valueRange);
ASN_NODE *EncodeValueDescription(int class_var, int type, int tagno,
ValueDescription *valueDescription);
ASN_NODE *EncodeUnitInfo(int class_var, int type, int tagno,
UnitInfo *unitInfo);
ASN_NODE *EncodeUnitType(int class_var, int type, int tagno,
UnitType *unitType);
ASN_NODE *EncodeUnits(int class_var, int type, int tagno,
Units *units);
ASN_NODE *EncodeCategoryList(int class_var, int type, int tagno,
CategoryList *categoryList);
ASN_NODE *EncodeCategoryInfo(int class_var, int type, int tagno,
CategoryInfo *categoryInfo);
ASN_NODE *EncodeCommonInfo(int class_var, int type, int tagno,
CommonInfo *commonInfo);
ASN_NODE *EncodeHumanString(int class_var, int type, int tagno,
HumanString *humanString);
ASN_NODE *EncodeIconObject(int class_var, int type, int tagno,
IconObject *iconObject);
ASN_NODE *EncodeContactInfo(int class_var, int type, int tagno,
ContactInfo *contactInfo);
ASN_NODE *EncodeNetworkAddress(int class_var, int type, int tagno,
NetworkAddress *networkAddress);
ASN_NODE *EncodeAccessInfo(int class_var, int type, int tagno,
AccessInfo *accessInfo);
ASN_NODE *EncodeQueryTypeDetails(int class_var, int type, int tagno,
QueryTypeDetails *queryTypeDetails);
ASN_NODE *EncodePrivateCapabilities(int class_var, int type, int tagno,
PrivateCapabilities *privateCapabilities);
ASN_NODE *EncodeRpnCapabilities(int class_var, int type, int tagno,
RpnCapabilities *rpnCapabilities);
ASN_NODE *EncodeIso8777Capabilities(int class_var, int type, int tagno,
Iso8777Capabilities *iso8777Capabilities);
ASN_NODE *EncodeProximitySupport(int class_var, int type, int tagno,
ProximitySupport *proximitySupport);
ASN_NODE *EncodeSearchKey(int class_var, int type, int tagno,
SearchKey *searchKey);
ASN_NODE *EncodeAccessRestrictions(int class_var, int type, int tagno,
AccessRestrictions *accessRestrictions);
ASN_NODE *EncodeCosts(int class_var, int type, int tagno,
Costs *costs);
ASN_NODE *EncodeCharge(int class_var, int type, int tagno,
Charge *charge);
ASN_NODE *EncodeDatabaseList(int class_var, int type, int tagno,
DatabaseList *databaseList);
ASN_NODE *EncodeAttributeCombinations(int class_var, int type, int tagno,
AttributeCombinations *attributeCombinations);
ASN_NODE *EncodeAttributeCombination(int class_var, int type, int tagno,
AttributeCombination *attributeCombination);
ASN_NODE *EncodeAttributeOccurrence(int class_var, int type, int tagno,
AttributeOccurrence *attributeOccurrence);
ASN_NODE *EncodeOPACRecord(int class_var, int type, int tagno,
OPACRecord *oPACRecord);
ASN_NODE *EncodeHoldingsRecord(int class_var, int type, int tagno,
HoldingsRecord *holdingsRecord);
ASN_NODE *EncodeHoldingsAndCircData(int class_var, int type, int tagno,
HoldingsAndCircData *holdingsAndCircData);
ASN_NODE *EncodeVolume(int class_var, int type, int tagno,
Volume *volume);
ASN_NODE *EncodeCircRecord(int class_var, int type, int tagno,
CircRecord *circRecord);
ASN_NODE *EncodeBriefBib(int class_var, int type, int tagno,
BriefBib *briefBib);
ASN_NODE *EncodeFormatSpec(int class_var, int type, int tagno,
FormatSpec *formatSpec);
ASN_NODE *EncodeGenericRecord(int class_var, int type, int tagno,
GenericRecord *genericRecord);
ASN_NODE *EncodeTaggedElement(int class_var, int type, int tagno,
TaggedElement *taggedElement);
ASN_NODE *EncodeElementData(int class_var, int type, int tagno,
ElementData *elementData);
ASN_NODE *EncodeElementMetaData(int class_var, int type, int tagno,
ElementMetaData *elementMetaData);
ASN_NODE *EncodeTagPath(int class_var, int type, int tagno,
TagPath *tagPath);
ASN_NODE *EncodeOrder(int class_var, int type, int tagno,
Order *order);
ASN_NODE *EncodeUsage(int class_var, int type, int tagno,
Usage *usage);
ASN_NODE *EncodeHitVector(int class_var, int type, int tagno,
HitVector *hitVector);
ASN_NODE *EncodeVariant(int class_var, int type, int tagno,
Variant *variant);
ASN_NODE *EncodeTaskPackage(int class_var, int type, int tagno,
TaskPackage *taskPackage);
/* -----
ASN_NODE *EncodeResourceReport(int class_var, int type, int tagno,
ResourceReport *resourceReport);
ASN_NODE *EncodeEstimate(int class_var, int type, int tagno,
Estimate *estimate);int type, int tagno,
EstimateType estimateType);
ASN_NODE *EncodeResourceReport(int class_var, int type, int tagno,
ResourceReport *resourceReport);
ASN_NODE *EncodeEstimate(int class_var, int type, int tagno,
Estimate *estimate);
ASN_NODE *EncodePromptObject(int class_var, int type, int tagno,
PromptObject *promptObject);
ASN_NODE *EncodeChallenge(int class_var, int type, int tagno,
Challenge *challenge);
ASN_NODE *EncodeResponse(int class_var, int type, int tagno,
Response *response);
ASN_NODE *EncodePromptId(int class_var, int type, int tagno,
PromptId *promptId);
ASN_NODE *EncodeEncryption(int class_var, int type, int tagno,
Encryption *encryption);
ASN_NODE *EncodeDES_RN_Object(int class_var, int type, int tagno,
DES_RN_Object *dES_RN_Object);
ASN_NODE *EncodeDRNType(int class_var, int type, int tagno,
DRNType *dRNType);
ASN_NODE *EncodeKRBObject(int class_var, int type, int tagno,
KRBObject *kRBObject);
ASN_NODE *EncodeKRBRequest(int class_var, int type, int tagno,
KRBRequest *kRBRequest);
ASN_NODE *EncodeKRBResponse(int class_var, int type, int tagno,
KRBResponse *kRBResponse);
ASN_NODE *EncodePersistentResultSet(int class_var, int type, int tagno,
PersistentResultSet *persistentResultSet);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno,
OriginPartNotToKeep *originPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno,
TargetPart *targetPart);
ASN_NODE *EncodePersistentQuery(int class_var, int type, int tagno,
PersistentQuery *persistentQuery);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno,
OriginPartNotToKeep *originPartNotToKeep);
ASN_NODE *EncodePeriodicQuerySchedule(int class_var, int type, int tagno,
PeriodicQuerySchedule *periodicQuerySchedule);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno,
OriginPartNotToKeep *originPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno,
TargetPart *targetPart);
ASN_NODE *EncodePeriod(int class_var, int type, int tagno,
Period *period);
ASN_NODE *EncodeItemOrder(int class_var, int type, int tagno,
ItemOrder *itemOrder);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeCreditCardInfo(int class_var, int type, int tagno,
CreditCardInfo *creditCardInfo);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno,
OriginPartNotToKeep *originPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno,
TargetPart *targetPart);
ASN_NODE *EncodeUpdate(int class_var, int type, int tagno,
Update *update);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno,
TargetPart *targetPart);
ASN_NODE *EncodeSuppliedRecords(int class_var, int type, int tagno,
SuppliedRecords *suppliedRecords);
ASN_NODE *EncodeCorrelationInfo(int class_var, int type, int tagno,
CorrelationInfo *correlationInfo);
ASN_NODE *EncodeTaskPackageRecordStructure(int class_var, int type, int tagno,
TaskPackageRecordStructure *taskPackageRecordStructure);
ASN_NODE *EncodeExportSpecification(int class_var, int type, int tagno,
ExportSpecification *exportSpecification);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeDestination(int class_var, int type, int tagno,
Destination *destination);
ASN_NODE *EncodeExportInvocation(int class_var, int type, int tagno,
ExportInvocation *exportInvocation);
ASN_NODE *EncodeOriginPartToKeep(int class_var, int type, int tagno,
OriginPartToKeep *originPartToKeep);
ASN_NODE *EncodeOriginPartNotToKeep(int class_var, int type, int tagno,
OriginPartNotToKeep *originPartNotToKeep);
ASN_NODE *EncodeTargetPart(int class_var, int type, int tagno,
TargetPart *targetPart);
ASN_NODE *EncodeSearchInfoReport(int class_var, int type, int tagno,
SearchInfoReport *searchInfoReport);
ASN_NODE *EncodeResultsByDB(int class_var, int type, int tagno,
ResultsByDB *resultsByDB);
ASN_NODE *EncodeQueryExpression(int class_var, int type, int tagno,
QueryExpression *queryExpression);
ASN_NODE *EncodeEspec_1(int class_var, int type, int tagno,
Espec_1 *espec_1);
ASN_NODE *EncodeElementRequest(int class_var, int type, int tagno,
ElementRequest *elementRequest);
ASN_NODE *EncodeSimpleElement(int class_var, int type, int tagno,
SimpleElement *simpleElement);
ASN_NODE *EncodeTagPath(int class_var, int type, int tagno,
TagPath *tagPath);
ASN_NODE *EncodeOccurrences(int class_var, int type, int tagno,
Occurrences *occurrences);
---*/

