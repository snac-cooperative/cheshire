
puts stderr "Loaded: zstrobe.tcl"

proc do_strobe {host port database} {

    set names {"AUTHOR" "ORGANIZATION" "TITLE" "ISBN" "ISSN" "LCCN" "SUBJECT" "LANG" "CODEN" "CONF" "SERIES" "UT" "BNB" "BGF" "LOCAL_NUMBER" "DEWEY_CLASSIFICATION" "UDC" "BC" "LC_CALL_NUMBER" "NLM_CALL_NUMBER" "NAL_CALL_NUMBER" "MOS_CALL_NUMBER" "LOCAL_CLASSIFICATION" "SUBJECT_RAMEAU" "BDI_INDEX_SUBJECT" "INSPEC_SUBJECT" "MESH_SUBJECT" "PA_SUBJECT" "LC_SUBJECT_HEADING" "RVM_SUBJECT_HEADING" "LOCAL_SUBJECT_INDEX" "DATE" "PUBDATE" "DATE_OF_ACQUISITION" "TITLE_KEY" "TITLE_COLLECTIVE" "TITLE_PARALLEL" "TITLE_COVER" "TITLE_ADDED_TITLE_PAGE" "TITLE_CAPTION" "TITLE_RUNNING" "TITLE_SPINE" "TITLE_OTHER_VARIANT" "TITLE_FORMER" "TITLE_ABBREVIATED" "TITLE_EXPANDED" "SUBJECT_PRECIS" "SUBJECT_RSWK" "SUBJECT_SUBDIVISION" "NUMBER_NATL_BIBLIO" "NUMBER_LEGAL_DEPOSIT" "NUMBER_GOVT_PUBLICATION" "NUMBER_PUBLISHER_FOR_MUSIC" "NUMBER_DB" "NUMBER_LOCAL_CALL" "CODE-GEOGRAPHIC_AREA" "CODE-INSTITUTION" "NAME_AND_TITLE" "NAME_GEOGRAPHIC" "PUBPLACE" "MICROFORM_GENERATION" "ABSTRACT" "NOTE" "AUTHOR-TITLE_KEY" "RECORD_TYPE_KEY" "NAME_KEY" "AUTHOR_KEY" "AUTHOR-NAME_PERSONAL_KEY" "AUTHOR-NAME_CORPORATION_KEY" "AUTHOR-NAME_CONFERENCE_KEY" "IDENTIFIER--STANDARD" "SUBJECT--LC_CHILDRENS" "SUBJECT_NAME--PERSONAL" "BODY_OF_TEXT" "DATE_TIME_ADDED_TO_DATABASE" "DATE_TIME_LAST_MODIFIED" "AUTHORITY_FORMAT_IDENTIFIER" "CONCEPT-TEXT" "CONCEPT-REFERENCE" "ANY" "DEFAULT" "PUBLISHER" "RECORD-SOURCE" "EDITOR" "BIB-LEVEL" "GEOGRAPHIC-CLASS" "INDEXED-BY" "MAP-SCALE" "MUSIC-KEY" "RELATED-PERIODICAL" "REPORT-NUMBER" "STOCK-NUMBER" "THEMATIC-NUMBER" "MATERIAL-TYPE" "DOC-ID" "HOST-ITEM" "CONTENT-TYPE" "ANYWHERE" "SPECIAL" "SIC" "Abstract-language" "Application-kind" "Classification" "Classification-basic" "Classification-local-record" "Enzyme" "Possessing-institution" "Record-linking" "Record-status" "Treatment" "Control-number-GKD" "Control-number-linking" "Control-number-PND" "Control-number-SWD" "Control-number-ZDB" "Country-publication" "Date-conference" "Date-record-status" "Dissertation-information" "Meeting-organizer" "Note-availability" "Number-CAS-registry" "Number-document" "Number-local-accounting" "Number-local-acquisition" "Number-local-call-copy-specific" "Number-of-reference" "Number-norm" "Number-volume" "Place-conference" "Reference" "Referenced-journal" "Section-code" "Section-heading" "Subject-GOO" "Subject-name-conference" "Subject-name-corporate" "Subject-name-form" "Subject-name-geographical" "Subject-name-chronological" "Subject-name-title" "Subject-name-topical" "Subject-uncontrolled" "Terminology-chemical" "Title-translated" "Year-of-beginning" "Year-of-ending" "Subject-AGROVOC" "Subject-COMPASS" "Subject-EPT" "Subject-NAL" "Classification-BCM" "Classification-DB" "Identifier-ISRC" "Identifier-ISMN" "Identifier-ISRN" "Identifier-DOI" "Code-language-original" "Title-later" "DC-TITLE" "DC-CREATOR" "DC-SUBJECT" "DC-DESCRIPTION" "DC-PUBLISHER" "DC-DATE" "DC-RESOURCETYPE" "DC-RESOURCEIDENTIFIER" "DC-LANGUAGE" "DC-OTHERCONTRIBUTOR" "DC-FORMAT" "DC-SOURCE" "DC-RELATION" "DC-COVERAGE" "DC-RIGHTSMANAGEMENT" "Controlled_Subject_Index" "Subject_Thesaurus" "Index_Terms--Controlled" "Controlled_Term" "Spatial_Domain" "Bounding_Coordinates" "West_Bounding_Coordinate" "East_Bounding_Coordinate" "North_Bounding_Coordinate" "South_Bounding_Coordinate" "Place" "Place_Keyword_Thesaurus" "Place_Keyword" "Time_Period" "Time_Period_Textual" "Time_Period_Structured" "Beginning_Date" "Ending_Date" "Availability" "Distributor" "Distributor_Name" "Distributor_Organization" "Distributor_Street_Address" "Distributor_City" "Distributor_State_or_Province" "Distributor_Zip_or_Postal_Code" "Distributor_Country" "Distributor_Network_Address" "Distributor_Hours_of_Service" "Distributor_Telephone" "Distributor_Fax" "Resource_Description" "Order_Process" "Order_Information" "Cost" "Cost_Information" "Technical_Prerequisites" "Available_Time_Period" "Available_Time_Textual" "Available_Time_Structured" "Available_Linkage" "Linkage_Type" "Linkage" "Sources_of_Data" "Methodology" "Access_Constraints" "General_Access_Constraints" "Originator_Dissemination_Control" "Security_Classification_Control" "Use_Constraints" "Point_of_Contact" "Contact_Name" "Contact_Organization" "Contact_Street_Address" "Contact_City" "Contact_State_or_Province" "Contact_Zip_or_Postal_Code" "Contact_Country" "Contact_Network_Address" "Contact_Hours_of_Service" "Contact_Telephone" "Contact_Fax" "Supplemental_Information" "Purpose" "Agency_Program" "Cross_Reference" "Cross_Reference_Title" "Cross_Reference_Relationship" "Cross_Reference_Linkage" "Schedule_Number" "Original_Control_Identifier" "Language_of_Record" "Record_Review_Date" "DISTRIB" "THEME_KEYWORD" "PURPOSE" "ACCESS_CONSTRAINTS" "USE_CONSTRAINTS" "HOURS_OF_SERVICE" "RESOURCE_DESCRIPTION" "ORDERING_INSTRUCTIONS" "TECHNICAL_PREREQUISITES" "ONLINE_LINKAGE" "CONTACT_PERSON" "CONTACT_ORGANIZATION" "ADDRESS" "CITY" "STATE_OR_PROVINCE" "POSTAL_CODE" "COUNTRY" "CONTACT_ELECTRONIC_MAIL_ADDRESS" "CONTACT_VOICE_TELEPHONE" "CONTACT_FACSIMILE_TELEPHONE" "SOURCE_CONTRIBUTION" "THEME_KEYWORD_THESAURUS" "WEST_BOUNDING_COORDINATE" "EAST_BOUNDING_COORDINATE" "NORTH_BOUNDING_COORDINATE" "SOUTH_BOUNDING_COORDINATE" "PLACE_KEYWORD" "PLACE_KEYWORD_THESAURUS" "TEMPORAL_KEYWORD" "SUPPLEMENTAL_INFORMATION" "FEES" "SPATIAL_DOMAIN" "BOUNDING_COORDINATES" "PLACE" "TIME_PERIOD_INFORMATION" "AVAILABLE_TIME_PERIOD" "POINT_OF_CONTACT" "CROSS_REFERENCE" "BEGINNING_DATE" "ENDING_DATE" "CONTACT_INFORMATION" "CONTACT_PERSON_PRIMARY" "CONTACT_POSITION" "CONTACT_ADDRESS" "ADDRESS_TYPE" "CONTACT_ORGANIZATION_PRIMARY" "CONTACT_TDD_TTY_TELEPHONE" "CONTACT_INSTRUCTIONS" "IDENTIFICATION_INFORMATION" "CITATION" "DESCRIPTION" "CURRENTNESS_REFERENCE" "STATUS" "PROGRESS" "MAINTENANCE_AND_UPDATE_FREQUENCY" "DATA_SET_G-POLYGON" "DATA_SET_G-POLYGON_OUTER_G-RING" "G-RING_LATITUDE" "G-RING_LONGITUDE" "DATA_SET_G-POLYGON_EXCLUSION_G-RING" "KEYWORDS" "THEME" "STRATUM" "STRATUM_KEYWORD_THESAURUS" "STRATUM_KEYWORD" "TEMPORAL" "TEMPORAL_KEYWORD_THESAURUS" "BROWSE_GRAPHIC" "BROWSE_GRAPHIC_FILE_NAME" "BROWSE_GRAPHIC_FILE_DESCRIPTION" "BROWSE_GRAPHIC_FILE_TYPE" "DATA_SET_CREDIT" "SECURITY_INFORMATION" "SECURITY_CLASSIFICATION_SYSTEM" "SECURITY_CLASSIFICATION" "SECURITY_HANDLING_DESCRIPTION" "NATIVE_DATA_SET_ENVIRONMENT" "EXTENT" "DATA_QUALITY_INFORMATION" "ATTRIBUTE_ACCURACY" "ATTRIBUTE_ACCURACY_REPORT" "QUANT_ATTRIBUTE_ACCURACY" "ATTRIBUTE_ACCURACY_VALUE" "ATTRIBUTE_ACCURACY_EXPLANATION" "LOGICAL_CONSISTENCY_REPORT" "COMPLETENESS_REPORT" "POSITIONAL_ACCURACY" "HORIZONTAL_POSITIONAL_ACCURACY" "HORIZONTAL_POSITIONAL_ACCURACY_REPORT" "QUANT_HORIZO_POSITIONAL_ACCURACY" "HORIZ_POSITIONAL_ACC_VAL" "HORIZ_POSIT_ACCURACY_EXPLAN" "VERTICAL_POSITIONAL_ACCURACY" "VERTICAL_POSITIONAL_ACCURACY_REPORT" "QUANT_VERTICAL_POSIT_ACCURACY_ASSESS" "VERTICAL_POSITIONAL_ACCURACY_VALUE" "VERTICAL_POSITIONAL_ACCURACY_EXPLANATION" "LINEAGE" "SOURCE_INFORMATION" "SOURCE_CITATION" "SOURCE_TIME_PERIOD_OF_CONTENT" "SOURCE_CURRENTNESS_REFERENCE" "SOURCE_CITATION_ABBREVIATION" "PROCESS_STEP" "PROCESS_DESCRIPTION" "SOURCE_USED_CITATION_ABBREVIATION" "PROCESS_DATE" "PROCESS_TIME" "SOURCE_PRODUCED_CITATION_ABBREVIATION" "PROCESS_CONTACT" "CLOUD_COVER" "SPATIAL_DATA_ORGANIZATION_INFORMATION" "INDIRECT_SPATIAL_REFERENCE" "DIRECT_SPATIAL_REFERENCE_METHOD" "SDTS_TERMS_DESCRIPTION" "SDTS_POINT_AND_VECTOR_OBJECT_TYPE" "POINT_AND_VECTOR_OBJECT_COUNT" "VPF_TERMS_DESCRIPTION" "VPF_TOPOLOGY_LEVEL" "VPF_POINT_AND_VECTOR_OBJECT_TYPE" "RASTER_OBJECT_INFORMATION" "RASTER_OBJECT_TYPE" "ROW_COUNT" "COLUMN_COUNT" "VERTICAL_COUNT" "POINT_AND_VECTOR_OBJECT_INFORMATION" "SPATIAL_REFERENCE_INFORMATION" "HORIZONTAL_COORDINATE_SYSTEM_DEFINITION" "GEOGRAPHIC" "LATITUDE_RESOLUTION" "LONGITUDE_RESOLUTION" "GEOGRAPHIC_COORDINATE_UNITS" "PLANAR" "MAP_PROJECTION" "MAP_PROJECTION_NAME" "STANDARD_PARALLEL" "LONGITUDE_OF_CENTRAL_MERIDIAN" "LATITUDE_OF_PROJECTION_ORIGIN" "FALSE_EASTING" "FALSE_NORTHING" "SCALE_FACTOR_AT_EQUATOR" "HEIGHT_OF_PERSP_POINT_ABOVE_SURFACE" "LONGITUDE_OF_PROJECTION_CENTER" "LATITUDE_OF_PROJECTION_CENTER" "SCALE_FACTOR_AT_CENTER_LINE" "OBLIQUE_LINE_AZIMUTH" "AZIMUTHAL_ANGLE" "AZIMUTH_MEASURE_POINT_LONGITUDE" "OBLIQUE_LINE_POINT" "OBLIQUE_LINE_LATITUDE" "OBLIQUE_LINE_LONGITUDE" "STRAIGHT_VERTICAL_LONGITUDE_FROM_POLE" "SCALE_FACTOR_AT_PROJECTION_ORIGIN" "LANDSAT_NUMBER" "PATH_NUMBER" "SCALE_FACTOR_AT_CENTRAL_MERIDIAN" "OTHER_PROJECTION'S_DEFINITION" "GRID_COORDINATE_SYSTEM" "GRID_COORDINATE_SYSTEM_NAME" "UNIVERSAL_TRANSVERSE_MERCATOR" "UTM_ZONE_NUMBER" "UNIVERSAL_POLAR_STEREOGRAPHIC" "UPS_ZONE_IDENTIFIER" "STATE_PLANE_COORDINATE_SYSTEM" "SPCS_ZONE_IDENTIFIER" "ARC_COORDINATE_SYSTEM" "ARC_SYSTEM_ZONE_IDENTIFIER" "OTHER_GRID_SYSTEM'S_DEFINITION" "LOCAL_PLANAR" "LOCAL_PLANAR_DESCRIPTION" "LOCAL_PLANAR_GEOREFERENCE_INFORMATION" "PLANAR_COORDINATE_INFORMATION" "PLANAR_COORDINATE_ENCODING_METHOD" "COORDINATE_REPRESENTATION" "ABSCISSA_RESOLUTION" "ORDINATE_RESOLUTION" "DISTANCE_AND_BEARING_REPRESENTATION" "DISTANCE_RESOLUTION" "BEARING_RESOLUTION" "BEARING_UNITS" "BEARING_REFERENCE_DIRECTION" "BEARING_REFERENCE_MERIDIAN" "PLANAR_DISTANCE_UNITS" "LOCAL" "LOCAL_DESCRIPTION" "LOCAL_GEOREFERENCE_INFORMATION" "GEODETIC_MODEL" "HORIZONTAL_DATUM_NAME" "ELLIPSOID_NAME" "SEMI-MAJOR_AXIS" "DENOMINATOR_OF_FLATTENING_RATIO" "VERTICAL_COORDINATE_SYSTEM_DEFINITION" "ALTITUDE_SYSTEM_DEFINITION" "ALTITUDE_DATUM_NAME" "ALTITUDE_RESOLUTION" "ALTITUDE_DISTANCE_UNITS" "ALTITUDE_ENCODING_METHOD" "DEPTH_SYSTEM_DEFINITION" "DEPTH_DATUM_NAME" "DEPTH_RESOLUTION" "DEPTH_DISTANCE_UNITS" "DEPTH_ENCODING_METHOD" "ALBERS_CONICAL_EQUAL_AREA" "AZIMUTHAL_EQUIDISTANT" "EQUIDISTANT_CONIC" "EQUIRECTANGULAR" "GENERAL_VERTICAL_NEAR-SIDED_PROJECTION" "GNOMONIC" "LAMBERT_AZIMUTHAL_EQUAL_AREA" "LAMBERT_CONFORMAL_CONIC" "MERCATOR" "MODIFIED_STEREOGRAPHIC_FOR_ALASKA" "MILLER_CYLINDRICAL" "OBLIQUE_MERCATOR" "ORTHOGRAPHIC" "POLAR_STEREOGRAPHIC" "POLYCONIC" "ROBINSON" "SINUSOIDAL" "SPACE_OBLIQUE_MERCATOR_(LANDSAT_)" "STEREOGRAPHIC" "TRANSVERSE_MERCATOR" "VAN_DER_GRINTEN" "ENTITY_AND_ATTRIBUTE_INFORMATION" "DETAILED_DESCRIPTION" "ENTITY_TYPE" "ENTITY_TYPE_LABEL" "ENTITY_TYPE_DEFINITION" "ENTITY_TYPE_DEFINITION_SOURCE" "ATTRIBUTE" "ATTRIBUTE_LABEL" "ATTRIBUTE_DEFINITION" "ATTRIBUTE_DEFINITION_SOURCE" "ATTRIBUTE_DOMAIN_VALUES" "ENUMERATED_DOMAIN" "ENUMERATED_DOMAIN_VALUE" "ENUMERATED_DOMAIN_VALUE_DEFINITION" "ENUMERATED_DOMAIN_VALUE_DEF_SOURCE" "RANGE_DOMAIN" "RANGE_DOMAIN_MINIMUM" "RANGE_DOMAIN_MAXIMUM" "CODESET_DOMAIN" "CODESET_NAME" "CODESET_SOURCE" "UNREPRESENTABLE_DOMAIN" "ATTRIBUTE_UNITS_OF_MEASUREMENT" "ATTRIBUTE_MEASUREMENT_RESOLUTION" "BEGINNING_DATE_OF_ATTRIBUTE_VALUES" "ENDING_DATE_OF_ATTRIBUTE_VALUES" "ATTRIBUTE_VALUE_ACCURACY_INFORMATION" "ATTRIBUTE_VALUE_ACCURACY" "ATTRIBUTE_VALUE_ACCURACY_EXPLANATION" "ATTRIBUTE_MEASUREMENT_FREQUENCY" "OVERVIEW_DESCRIPTION" "ENTITY_AND_ATTRIBUTE_OVERVIEW" "ENTITY_AND_ATTRIBUTE_DETAIL_CITATION" "DISTRIBUTION_INFORMATION" "DISTRIBUTION_LIABILITY" "STANDARD_ORDER_PROCESS" "NON-DIGITAL_FORM" "DIGITAL_FORM" "DIGITAL_TRANSFER_INFORMATION" "FORMAT_NAME" "FORMAT_VERSION_NUMBER" "FORMAT_VERSION_DATE" "FORMAT_SPECIFICATION" "FORMAT_INFORMATION_CONTENT" "FILE_DECOMPRESSION_TECHNIQUE" "TRANSFER_SIZE" "DIGITAL_TRANSFER_OPTION" "ONLINE_OPTION" "COMPUTER_CONTACT_INFORMATION" "NETWORK_ADDRESS" "NETWORK_RESOURCE_NAME" "DIALUP_INSTRUCTIONS" "LOWEST_BPS" "HIGHEST_BPS" "NUMBER_DATABITS" "NUMBER_STOPBITS" "PARITY" "COMPRESSION_SUPPORT" "DIALUP_TELEPHONE" "DIALUP_FILE_NAME" "ACCESS_INSTRUCTIONS" "ONLINE_COMPUTER_AND_OPERATING_SYSTEM" "OFFLINE_OPTION" "OFFLINE_MEDIA" "RECORDING_CAPACITY" "RECORDING_DENSITY" "RECORDING_DENSITY_UNITS" "RECORDING_FORMAT" "COMPATIBILITY_INFORMATION" "TURNAROUND" "CUSTOM_ORDER_PROCESS" "METADATA_REFERENCE_INFORMATION" "METADATA_REVIEW_DATE" "METADATA_FUTURE_REVIEW_DATE" "METADATA_CONTACT" "METADATA_STANDARD_NAME" "METADATA_STANDARD_VERSION" "METADATA_TIME_CONVENTION" "METADATA_ACCESS_CONSTRAINTS" "METADATA_USE_CONSTRAINTS" "METADATA_SECURITY_INFORMATION" "METADATA_SECURITY_CLASSIFICATION_SYSTEM" "METADATA_SECURITY_CLASSIFICATION" "METADATA_SECURITY_HANDLING_DESCRIPTION" "CITATION_INFORMATION" "PUBLICATION_TIME" "GEOSPATIAL_DATA_PRESENTATION_FORM" "SERIES_INFORMATION" "EDITION" "ISSUE_IDENTIFICATION" "PUBLICATION_INFORMATION" "OTHER_CITATION_DETAILS" "LARGER_WORK_CITATION" "TIME_PERIOD_OF_CONTENT" "SINGLE_DATE_TIME" "CALENDAR_DATE" "TIME_OF_DAY" "MULTIPLE_DATES_TIMES" "RANGE_OF_DATES_TIMES" "BEGINNING_TIME" "ENDING_TIME" "METADATA" "DistributorName" "Distributor_Organization" "Distributor_Street_Address" "Distributor_City" "Distributor_State" "Distributor_Zip_Code" "Distributor_Country" "Distributor_Network_Address" "Distributor Telephone" "Distributor_Fax" "Available_Time_Period--Structured" "Available_Time_Period--Textual" "Available_Linkage_Type" "Contact_Hours_of_Service" "Agency_Program" "Methodology" "Time_Period-Structured" "Cross_Reference_Title" "Cross_Reference_Linkage" "Cross_Reference_Type" "Original_Control_Identifier" "BROADER_TERM" "NARROWER_TERM" "RELATED_TERM" "TOP_TERM" "BLOBID" "IMAGEID" "BACKGROUND" "BLOB" "lctgm" "gmgpc" "3.1.1" "3.1.2" "3.1.3" "3.1.4" "3.1.5" "3.2.1" "3.2.2" "3.2.3" "3.2.4" "3.2.5" "3.3.1" "3.3.2" "3.3.3" "3.3.4" "3.4.1" "3.4.2" "3.4.3" "3.4.4" "3.4.5" "3.4.6" "3.5.1" "3.5.2" "3.5.3" "3.5.4" "3.5.5" "3.6.1" "COMPONENTID" };

    set descs {"Author_Personal_name" "Corporate_name" "Title" "Intl_Standard_Bibliographic_Number" "Intl_Standard_Series_Number" "LC_card_number" "Subject_heading" "Language" "CODEN" "Conference_name" "Series_Title" "Uniform_Title" "BNB_card_no." "BGF_number" "Local_number" "Dewey_classification" "UDC_classification" "Bliss_classification" "LC_call_number" "NLM_call_number" "NAL_call_number" "MOS_call_number" "Local_classification" "Subject_Rameau" "BDI_index_subject" "INSPEC_subject" "MESH_subject" "PA_subject" "LC_subject_heading" "RVM_subject_heading" "Local_subject_index" "Date" "Date_of_Publication" "Date_of_acquisition" "Title_key" "Title_collective" "Title_parallel" "Title_cover" "Title_added_title_page" "Title_caption" "Title_running" "Title_spine" "Title_other_variant" "Title_former" "Title_abbreviated" "Title_expanded" "Subject_precis" "Subject_rswk" "Subject_subdivision" "Number_natl_biblio" "Number_legal_deposit" "Number_govt_publication" "Number_publisher_for_music" "Number_db" "Number_local_call" "Code_--_geographic_area" "Code_--_institution" "Name_and_title" "Name_geographic" "Place_publication" "Microform_generation" "Abstract" "Note" "Author-title" "Record_type" "Name" "Author" "Author-name_personal" "Author-name_corporation" "Author-name_conference" "Identifier_--_standard" "Subject_--_LC_children's" "Subject_name_--_personal" "Body_of_text" "Date/time_added_to_database" "Date/time_last_modified" "Authority/format_identifier" "Concept-text" "Concept-reference" "Any" "Default" "Publisher" "Record-source" "Editor" "Bib-level" "Geographic-class" "Indexed-by" "Map-scale" "Music-key" "Related-periodical" "Report-number" "Stock-number" "Thematic-number" "Material-type" "Doc-Id" "Host-Item" "Content-type" "Anywhere" "UNUSED-1036" "SIC Code (MELVYL)" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "country of Publication" "(meeting date)" "" "" "" "" "CAS registry number" "document number" "" "" "" "reference count" "" "" "meeting location" "references and footnotes" "reference work" "" "" "" "" "" "" "" "" "" "" "" "chemical name" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "Dublin Core Title BIB-1 Mapping" "Dublin Core Creator BIB-1 Mapping" "Dublin Core Subject BIB-1 Mapping" "Dublin Core Description BIB-1 Mapping" "Dublin Core Publisher BIB-1 Mapping" "Dublin Core Date BIB-1 Mapping" "Dublin Core Resource Type BIB-1 Mapping" "Dublin Core Resource Identifier BIB-1 Mapping" "Dublin Core Language BIB-1 Mapping" "Dublin Core Other Contributor BIB-1 Mapping" "Dublin Core Format BIB-1 Mapping" "Dublin Core Source BIB-1 Mapping" "Dublin Core Relation BIB-1 Mapping" "Dublin Core Coverage BIB-1 Mapping" "Dublin Core Rights Management BIB-1 Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "New Bib-1 GILS Mapping" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "GEO Attributes" "DistributorName" "Distributor Organization" "Distributor Street Address" "Distributor City" "Distributor State" "Distributor Zip Code" "Distributor Country" "Distributor Network Address" "Distributor Telephone" "Distributor_Fax" "Available Time Period -- Structured" "Available Time Period -- Textual" "Available Linkage Type" "Contact Hours of Service" "Agency Program" "Methodology" "Time Period - Structured" "Cross Reference Title" "Cross Reference Linkage" "Cross Reference Type" "Original Control Identifier" "Thesaurus" "Thesaurus" "Thesaurus" "Thesaurus" "BLOB_ID" "Image_ID" "background" "blobs" "LC Thesaurus of Graphic Materials" "Graphic Material Genre Headings" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "ISAD(G)" "Local Components" };

    set nums {1 2 4 7 8 9 21 54 60 3 5 6 10 11 12 13 14 15 16 17 18 19 20 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 55 56 57 58 59 61 62 63 1000 1001 1002 1003 1004 1005 1006 1007 1008 1009 1010 1011 1012 1013 1014 1015 1016 1017 1018 1019 1020 1021 1022 1023 1024 1025 1026 1027 1028 1030 1031 1032 1033 1034 1035 1036 1037 1038 1039 1040 1041 1042 1043 1044 1045 1046 1047 1048 1049 1050 1051 1052 1053 1054 1055 1056 1057 1058 1059 1060 1061 1062 1063 1064 1065 1066 1067 1068 1069 1070 1071 1072 1073 1074 1075 1076 1077 1078 1079 1080 1081 1082 1083 1084 1085 1086 1087 1088 1089 1090 1091 1092 1093 1094 1095 1096 1097 1098 1099 1100 1101 1102 1103 1104 1105 1106 1107 1108 1109 1110 1111 1112 1113 1114 1115 1116 1117 1118 1119 1120 1121 1122 1123 1124 1125 1126 1127 1128 1129 1130 1131 1132 1133 1134 1135 1136 1137 1138 1139 1140 1141 1142 1143 1144 1145 1146 1147 1148 1149 1150 1151 1152 1153 1154 1155 1156 1157 1158 1159 1160 1161 1162 1163 1164 1165 1166 1167 1168 1169 1170 1171 1172 1173 1174 1175 1176 1177 1178 1179 1180 1181 1182 1183 1184 2000 2002 2003 2004 2005 2013 2016 2017 2018 2021 2023 2024 2025 2026 2027 2028 2029 2030 2032 2033 2035 2036 2038 2039 2040 2041 2042 2043 2045 2050 2055 2059 2060 2061 2062 2065 2067 2068 2072 2073 3000 3004 3005 3006 3007 3008 3014 3018 3100 3101 3102 3106 3107 3108 3109 3116 3117 3118 3119 3120 3121 3122 3128 3129 3130 3131 3132 3137 3138 3139 3140 3141 3142 3143 3144 3145 3146 3148 3200 3201 3202 3203 3204 3205 3206 3207 3208 3209 3210 3211 3212 3213 3214 3215 3216 3217 3218 3219 3220 3221 3223 3224 3225 3227 3228 3229 3230 3231 3232 3233 3234 3300 3301 3302 3303 3304 3305 3306 3307 3308 3309 3310 3311 3312 3313 3314 3400 3401 3402 3403 3404 3405 3406 3407 3408 3410 3411 3412 3413 3414 3415 3416 3417 3418 3419 3420 3421 3422 3423 3424 3425 3426 3427 3428 3429 3430 3431 3432 3433 3434 3435 3436 3437 3438 3439 3440 3441 3442 3443 3444 3445 3446 3447 3448 3449 3450 3451 3452 3453 3454 3455 3456 3457 3458 3459 3460 3461 3462 3463 3464 3465 3466 3467 3468 3469 3470 3471 3472 3473 3474 3475 3476 3477 3478 3479 3480 3481 3482 3483 3484 3485 3486 3487 3488 3491 3492 3493 3494 3495 3496 3497 3498 3499 3500 3501 3502 3503 3504 3505 3506 3507 3508 3509 3510 3511 3512 3513 3514 3515 3516 3517 3518 3519 3520 3521 3522 3523 3524 3525 3526 3527 3528 3529 3530 3531 3532 3600 3603 3604 3605 3606 3607 3608 3609 3610 3611 3612 3613 3614 3615 3616 3617 3618 3619 3620 3621 3622 3623 3624 3625 3626 3627 3628 3629 3630 3631 3632 3633 3634 3635 3636 3637 3640 3641 3700 3702 3703 3704 3705 3706 3707 3708 3709 3710 3711 3712 3713 3800 3803 3805 3806 3807 3808 3809 3812 3814 3901 3902 3903 3904 3905 3906 3908 3910 3999 2001 2006 2007 2008 2009 2010 2011 2012 2014 2015 2019 2020 2022 2031 2034 2037 2044 2046 2047 2048 2049 5001 5002 5003 5004 5011 5012 5013 5014 5201 5202 5311 5312 5313 5314 5315 5321 5322 5323 5324 5325 5331 5332 5333 5334 5341 5342 5343 5344 5345 5346 5351 5352 5353 5354 5355 5361 5400 };

    set main 1035
    set extended 1096
    set dublin 1111
    set gils 1184
    set geo 3999
    set thesaurus 5202
    set isadg 5361
    set component 5400
    
    set borders {0 1035 1096 1111 1184 3999 5202 5361 5400}
    set bordernames {null main extended dublin gils geo thesaurus isadg component}

    # Need to read in sets from a store.
    set searchsets {1 2 3 }

    set foundnums {}
    set foundidxs {}
    set length [llength $nums]

    zselect probe $host $database $port
    
    for {set idx 0} {$idx < $length} { incr idx} {
	set number [lindex $nums $idx]
	
	set dosearch 0
	foreach setidx $searchsets {
	    if { $number > [lindex $borders [expr $setidx -1]] && $number <= [lindex $borders $setidx] } {
		set dosearch 1
	    }
	}
	if { $dosearch == 1} {
	    if { [lsearch $foundnums $number] == -1 } {
		set err [catch {zfind $number nothing} qresults];
		set status [lindex [lindex [lindex $qresults 0] 1] 1]
		if {$status == 1} {
		    lappend foundnums $number;
		    lappend foundidxs $idx;
		}
	    }
	}
    }
    
    #Write HTML form in the mean time for searching.
    
    
    throw_html_type
    throw_file "index_strobe_template.html"
    
    set url "z3950://$host:$port/search/$database"
    
    #Build form
    set formtxt "<form action=\"$url\">\n<table>\n"
    append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
    
    foreach fidx $foundidxs {
	set name [lindex $names $fidx]
	set number [lindex $nums $fidx]
	regsub -all {[_-]} $name " " name
	append formtxt "<tr><td><b>$name</b>:</td><td><input type = \"text\" size = \"40\" name = \"$number\" value = \"\"></td></tr>\n"
    }
    
    append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
    append formtxt "<tr><td>Combine Fields With:</td><td><select name = \"bool\"><option value = \"AND\">And<option value = \"OR\">Or</select></td></tr>\n";
    append formtxt "\n</table>\n<input type =\"submit\" value = \"Search\"> &nbsp; <input type = \"reset\" value = \"Reset\">\n</form>\n"
    
    puts $formtxt
    puts "<hr></body></html>"
    
}
