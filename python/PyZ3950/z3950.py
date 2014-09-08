#!/usr/bin/env python
# XXX spiff up diagnostic reporting!!!

# This file should be available from
# http://www.pobox.com/~asl2/software/PyZ3950/
# and is licensed under the X Consortium license:
# Copyright (c) 2001, Aaron S. Lav, asl2@pobox.com
# All rights reserved. 

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, provided that the above
# copyright notice(s) and this permission notice appear in all copies of
# the Software and that both the above copyright notice(s) and this
# permission notice appear in supporting documentation. 

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
# OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
# INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
# FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

# Except as contained in this notice, the name of a copyright holder
# shall not be used in advertising or otherwise to promote the sale, use
# or other dealings in this Software without prior written authorization
# of the copyright holder. 

# Change history:
# 2002/05/23
# Fix for Python2 compatibility.  Thanks to Douglas Bates <bates@stat.wisc.edu>
# Fix to support SUTRS (requires asn1 updates, too)
# 2002/05/28
# Make SUTRS printing a little more useful
# Correctly close connection when done
# Handle receiving diagnostics instead of records a little better

"""<p>PyZ3950 currently is capable of sending and receiving v2 PDUs
Initialize, Search, Present, Scan, Close, and Delete.  For client
work, you probably want to use ZOOM, which should be in the same
distribution as this file, in zoom.py.  The Server class in this file
implements a server, but could use some work.  Both interoperate with
the <a href="http://www.indexdata.dk/yaz"> Yaz toolkit</a> and the
client interoperates with a variety of libraries.  <p>

Useful resources:
<ul>
<li><a href="http://lcweb.loc.gov/z3950/agency/">
Library of Congress Z39.50 Maintenance Agency Page</a></li>
<li><a href="http://lcweb.loc.gov/z3950/agency/document.html">
Official Specification</a></li>
<li><a href="http://www.loc.gov/z3950/agency/clarify/">Clarifications</a></li>
</ul>
"""

from __future__ import nested_scopes
from PyZ3950 import asn1

# This is a global switch: do we support V3 at all?  (The answer is no.)
# When we support V3, we also need per-connection state about whether
# v2 or v3 has been negotiated.
Z3950_VERS = 3

class Z3950Error(Exception):
    pass
class ConnectionError(Z3950Error): # TCP or other transport error
    pass
class ProtocolError(Z3950Error): # Unexpected message or badly formatted
    pass
class UnexpectedCloseError(ProtocolError):
    pass

# OID definitions from Z39.50
# NB: These are just values, not OID objects
# To encode:  asn1.OidVal(Z3950_FOO)

Z3950_OID = [1,2,840,10003]

Z3950_ATTRS = Z3950_OID + [3]
Z3950_DIAG = Z3950_OID + [4]
Z3950_RECSYN = Z3950_OID + [5]
Z3950_TRANSFER = Z3950_OID + [6]
Z3950_RRF = Z3950_OID + [7]
Z3950_ACCESS = Z3950_OID + [8]
Z3950_ES = Z3950_OID + [9]
Z3950_USR = Z3950_OID + [10]
Z3950_SPEC = Z3950_OID + [11]
Z3950_VAR = Z3950_OID + [12]
Z3950_SCHEMA = Z3950_OID + [13]
Z3950_TAGSET = Z3950_OID + [14]
Z3950_NEG = Z3950_OID + [15]
Z3950_QUERY = Z3950_OID + [16]
Z3950_IMPL = Z3950_OID + [1000]

Z3950_ATTRS_BIB1 = Z3950_ATTRS + [1]
Z3950_ATTRS_EXP1 =  Z3950_ATTRS + [2]
Z3950_ATTRS_EXT1 =  Z3950_ATTRS + [3]
Z3950_ATTRS_CCL1 =  Z3950_ATTRS + [4]
Z3950_ATTRS_GILS =  Z3950_ATTRS + [5]
Z3950_ATTRS_STAS =  Z3950_ATTRS + [6]
Z3950_ATTRS_COLLECTIONS1 =  Z3950_ATTRS + [7]
Z3950_ATTRS_CIMI1 = Z3950_ATTRS + [8]
Z3950_ATTRS_GEO =   Z3950_ATTRS + [9]
Z3950_ATTRS_ZBIG =  Z3950_ATTRS + [10]
Z3950_ATTRS_UTIL  = Z3950_ATTRS + [11]
Z3950_ATTRS_XD_1  = Z3950_ATTRS + [12]
Z3950_ATTRS_ZTHES = Z3950_ATTRS + [13]
Z3950_ATTRS_FIN1 =  Z3950_ATTRS + [14]
Z3950_ATTRS_DAN1 =  Z3950_ATTRS + [15]
Z3950_ATTRS_HOLD =  Z3950_ATTRS + [16]
Z3950_ATTRS_MARC =  Z3950_ATTRS + [17]
Z3950_ATTRS_BIB2 =  Z3950_ATTRS + [18]
Z3950_ATTRS_ZEEREX =  Z3950_ATTRS + [19]

Z3950_DIAG_BIB1 = Z3950_DIAG + [1]
Z3950_DIAG_DIAG1 = Z3950_DIAG + [2]
Z3950_DIAG_ES = Z3950_DIAG + [3]
Z3950_DIAG_GENERAL = Z3950_DIAG + [4]

Z3950_RECSYN_UNIMARC = Z3950_RECSYN + [1]
Z3950_RECSYN_INTERMARC = Z3950_RECSYN + [2]
Z3950_RECSYN_CCF = Z3950_RECSYN + [3]

Z3950_RECSYN_USMARC = Z3950_RECSYN + [10]

Z3950_RECSYN_MARC_BIBLIO = Z3950_RECSYN_USMARC + [1]
Z3950_RECSYN_MARC_AUTH = Z3950_RECSYN_USMARC + [2]
Z3950_RECSYN_MARC_HOLD = Z3950_RECSYN_USMARC + [3]
Z3950_RECSYN_MARC_COMMUNITY = Z3950_RECSYN_USMARC + [4]
Z3950_RECSYN_MARC_CLASS = Z3950_RECSYN_USMARC + [5]

Z3950_RECSYN_UKMARC = Z3950_RECSYN + [11]
Z3950_RECSYN_NORMARC = Z3950_RECSYN + [12]
Z3950_RECSYN_LIBRISMARC = Z3950_RECSYN + [13]
Z3950_RECSYN_DANMARC = Z3950_RECSYN + [14]
Z3950_RECSYN_FINMARC = Z3950_RECSYN + [15]
Z3950_RECSYN_MAB = Z3950_RECSYN + [16]
Z3950_RECSYN_CANMARC = Z3950_RECSYN + [17]
Z3950_RECSYN_SBNMARC = Z3950_RECSYN + [18]
Z3950_RECSYN_PICAMARC = Z3950_RECSYN + [19]
Z3950_RECSYN_AUSMARC = Z3950_RECSYN + [20]
Z3950_RECSYN_IBERMARC = Z3950_RECSYN + [21]
Z3950_RECSYN_CATMARC = Z3950_RECSYN + [22]
Z3950_RECSYN_MALMARC = Z3950_RECSYN + [23]
Z3950_RECSYN_JPMARC = Z3950_RECSYN + [24]
Z3950_RECSYN_SWEMARC = Z3950_RECSYN + [25]
Z3950_RECSYN_SIGLEMARC = Z3950_RECSYN + [26]
Z3950_RECSYN_ISDSMARC = Z3950_RECSYN + [27]
Z3950_RECSYN_RUSMARC = Z3950_RECSYN + [28]
Z3950_RECSYN_HUNMARC = Z3950_RECSYN + [29]

Z3950_RECSYN_NACSIS_CATP = Z3950_RECSYN + [30]
Z3950_RECSYN_FINMARC_2000 = Z3950_RECSYN + [31]
Z3950_RECSYN_MARC21_FIN = Z3950_RECSYN + [32]
Z3950_RECSYN_COMARC = Z3950_RECSYN + [33]

Z3950_RECSYN_EXPLAIN = Z3950_RECSYN + [100]
Z3950_RECSYN_SUTRS = Z3950_RECSYN + [101]
Z3950_RECSYN_OPAC  = Z3950_RECSYN + [102]
Z3950_RECSYN_SUMMARY = Z3950_RECSYN + [103]
Z3950_RECSYN_GRS0 = Z3950_RECSYN + [104]
Z3950_RECSYN_GRS1 = Z3950_RECSYN + [105]
Z3950_RECSYN_ES = Z3950_RECSYN + [106]
Z3950_RECSYN_FRAGMENT = Z3950_RECSYN + [107]

Z3950_RECSYN_PDF = Z3950_RECSYN + [109, 1]
Z3950_RECSYN_POSTSCRIPT = Z3950_RECSYN + [109, ]
Z3950_RECSYN_HTML = Z3950_RECSYN + [109, 3]
Z3950_RECSYN_TIFF = Z3950_RECSYN + [109, 4]
Z3950_RECSYN_GIF = Z3950_RECSYN + [109, 5]
Z3950_RECSYN_JPEG = Z3950_RECSYN + [109, 6]
Z3950_RECSYN_PNG = Z3950_RECSYN + [109, 7]
Z3950_RECSYN_MPEG = Z3950_RECSYN + [109, 8]
Z3950_RECSYN_SGML = Z3950_RECSYN + [109, 9]
Z3950_RECSYN_XML = Z3950_RECSYN + [109, 10]

Z3950_RECSYN_TIFF_B = Z3950_RECSYN + [110, 1]
Z3950_RECSYN_WAV = Z3950_RECSYN + [110, 2]

Z3950_RECSYN_SQL = Z3950_RECSYN + [111]

Z3950_RRF_RESOURCE1 = Z3950_RRF + [1]
Z3950_RRF_RESOURCE2 = Z3950_RRF + [2]

Z3950_ACCESS_PROMPT1 = Z3950_ACCESS + [1]
Z3950_ACCESS_DES1 = Z3950_ACCESS + [2]
Z3950_ACCESS_KRB1 = Z3950_ACCESS + [3]

Z3950_ES_PERSIST_RS = Z3950_ES + [1]
Z3950_ES_PERSIST_QRY = Z3950_ES + [2]
Z3950_ES_PERIOD_QRY = Z3950_ES + [3]
Z3950_ES_ITEM_ORDER = Z3950_ES + [4]
Z3950_ES_DB_UPDATE = Z3950_ES + [5]
Z3950_ES_DB_UPDATE_REV1 = Z3950_ES_DB_UPDATE + [1,1]
Z3950_ES_EXPORT_SPEC = Z3950_ES + [6]
Z3950_ES_EXPORT_INV = Z3950_ES + [7]


Z3950_USR_SEARCHRES1 = Z3950_USR + [1]
Z3950_USR_CHARSET_NEG = Z3950_USR + [2]
Z3950_USR_INFO1 = Z3950_USR + [3]
Z3950_USR_SEARCH_TERMS1 = Z3950_USR + [4]
Z3950_USR_SEARCH_TERMS2 = Z3950_USR + [5]
Z3950_USR_DATE_TIME = Z3950_USR + [6]
# 7 and 8 are obsolete
Z3950_USR_INSERT_ACTION_QUAL = Z3950_USR + [9]
Z3950_USR_EDIT_ACTION_QUAL = Z3950_USR + [10]
Z3950_USR_AUTH_FILE = Z3950_USR + [11]

Z3950_SPEC_ESPEC1 = Z3950_SPEC + [1]
Z3950_SPEC_ESPEC2 = Z3950_SPEC + [2]
Z3950_SPEC_ESPECQ = Z3950_SPEC + [3]

Z3950_VAR_VARIANT1 = Z3950_VAR + [1]

Z3950_SCHEMA_WAIS = Z3950_SCHEMA + [1]
Z3950_SCHEMA_GILS = Z3950_SCHEMA + [2]
Z3950_SCHEMA_COLLECTIONS = Z3950_SCHEMA + [3]
Z3950_SCHEMA_GEO = Z3950_SCHEMA + [4]
Z3950_SCHEMA_CIMI = Z3950_SCHEMA + [5]
Z3950_SCHEMA_UPDATE = Z3950_SCHEMA + [6]
Z3950_SCHEMA_HOLDINGS = Z3950_SCHEMA + [7]
Z3950_SCHEMA_HOLDINGS11 = Z3950_SCHEMA + [7,1]
Z3950_SCHEMA_HOLDINGS12 = Z3950_SCHEMA + [7,2]
Z3950_SCHEMA_HOLDINGS14 = Z3950_SCHEMA + [7,4]
Z3950_SCHEMA_ZTHES = Z3950_SCHEMA + [1]
Z3950_SCHEMA_INSERT = Z3950_SCHEMA + [1]
Z3950_SCHEMA_EDIT = Z3950_SCHEMA + [1]

Z3950_TAGSET_M = Z3950_TAGSET + [1]
Z3950_TAGSET_G = Z3950_TAGSET + [2]
Z3950_TAGSET_STAS = Z3950_TAGSET + [3]
Z3950_TAGSET_GILS = Z3950_TAGSET + [4]
Z3950_TAGSET_COLLECTIONS = Z3950_TAGSET + [5]
Z3950_TAGSET_CIMI = Z3950_TAGSET + [6]
Z3950_TAGSET_UPDATE = Z3950_TAGSET + [7]
Z3950_TAGSET_ZTHES = Z3950_TAGSET + [8]

Z3950_NEG_CHARSET2 = Z3950_NEG + [1]
Z3950_NEG_ES = Z3950_NEG + [2]
Z3950_NEG_CHARSET3 = Z3950_NEG + [3]

Z3950_QUERY_SQL = Z3950_QUERY + [1]
Z3950_QUERY_CQL = Z3950_QUERY + [2]

Z3950_IMPL_CHESHIRE = Z3950_IMPL + [171]
Z3950_IMPL_CHESHIRE_CCG = Z3950_IMPL_CHESHIRE + [1]


# ASN1 definitions

ProtocolVersion = asn1.TYPE (3, asn1.BITSTRING)
Options = asn1.TYPE (4, asn1.BITSTRING)
ReferenceId = asn1.TYPE (2, asn1.OCTSTRING)

# XXX Negotiatable
if Z3950_VERS == 2:
    InternationalString = asn1.VisibleString
else:
    InternationalString = asn1.GeneralString

InfoCategory = asn1.SEQUENCE ([('categoryTypeId',1, asn1.OID, 1),
                               ('categoryValue', 2, asn1.INTEGER)])

OtherInfoInformation = asn1.CHOICE ([
    ('characterInfo', 2, InternationalString),
    ('binaryInfo', 3, asn1.OCTSTRING),
    ('externallyDefinedInfo', 4, asn1.EXTERNAL),
    ('oid', 5, asn1.OID)])


OtherInfoElt = asn1.SEQUENCE ([('category', 1, InfoCategory, 1),
                               ('information', None, OtherInfoInformation)])

OtherInformation = asn1.TYPE (201, asn1.SEQUENCE_OF (OtherInfoElt))

UserPass = asn1.SEQUENCE ([
    ('groupId', 0, InternationalString,1),
    ('userId', 1, InternationalString, 1),
    ('password', 2, InternationalString, 1)])

IdAuthentication = asn1.CHOICE ([
    ('open', None, asn1.VisibleString),
    ('idPass',   None, UserPass),
    ('anonymous', None, asn1.NULL),
    ('other', None, asn1.EXTERNAL)])
# if other, possibly see Appendix 7 for access control formats

InitializeRequest = asn1.SEQUENCE([
    ('referenceId', None, ReferenceId, 1),
    ('protocolVersion', None, ProtocolVersion),
    ('options', None, Options),
    ('preferredMessageSize', 5, asn1.INTEGER),
    ('exceptionalRecordSize', 6, asn1.INTEGER),
    ('idAuthentication', asn1.EXPLICIT(7), IdAuthentication, 1),
    # actually idAuthentication should be asn1.ANY.  Let me know
    # if you need the full generality here.
    ('implementationId', 110, InternationalString, 1),
    ('implementationName', 111, InternationalString, 1),
    ('implementationVersion', 112, InternationalString, 1),
    ('userInformationField', 11, asn1.EXTERNAL, 1),
    ('otherInfo', None, OtherInformation, 1)])

InitializeResponse = asn1.SEQUENCE([
    ('referenceId', None, ReferenceId, 1),
    ('protocolVersion', None, ProtocolVersion),
    ('options', None, Options),
    ('preferredMessageSize', 5, asn1.INTEGER),
    ('exceptionalRecordSize', 6, asn1.INTEGER),
    ('result', 12, asn1.BOOLEAN),
    ('implementationId', 110, InternationalString, 1),
    ('implementationName', 111, InternationalString, 1),
    ('implementationVersion', 112, InternationalString, 1),
    ('userInformationField', asn1.EXPLICIT(11), asn1.EXTERNAL, 1),
    ('otherInfo', None, OtherInformation, 1)])


CloseReason = asn1.TYPE (211, asn1.INTEGER) # see std for enum'd vals

ResourceReportId = asn1.OID

StringOrNumeric = asn1.CHOICE ([('string', 1, InternationalString),
                                ('numeric', 2, asn1.INTEGER)])

Unit = asn1.SEQUENCE ([('unitSystem', 1, InternationalString,1),
                            ('unitType', 2, StringOrNumeric,1),
                            ('unit', 3, StringOrNumeric, 1),
                            ('scaleFactor', 4, asn1.INTEGER, 1)])

IntUnit = asn1.SEQUENCE ( [('value', 1, asn1.INTEGER),
                               ('unitUsed', 2, Unit)])

Estimate = asn1.SEQUENCE ([('type', 1, StringOrNumeric),
                                ('value', 2, IntUnit)])

ResourceReport = asn1.SEQUENCE ( [('estimates', 1, Estimate, 1),
                                      ('message', 2, InternationalString, 1)])

Close = asn1.SEQUENCE ([('referenceId', None, ReferenceId, 1),
                             ('closeReason', None, CloseReason),
                             ('diagnosticInformation', 3, InternationalString),
                             ('resourceReportFormat', 4, ResourceReportId, 1),
                             ('resourceReport', 5, ResourceReport, 1),
                             ('otherInfo', None, OtherInformation, 1)])
                              

DatabaseName = asn1.TYPE (105, InternationalString)
ElementSetName = asn1.TYPE (103, InternationalString)

dbspec = asn1.SEQUENCE ([('dbName', None, DatabaseName),
                         ('esn', None, ElementSetName)])

# XXX this seems to generate the right BER for now, but doesn't
# match the spec.  probably an asn1.py bug -- fix!!

ElementSetNames = asn1.SEQUENCE ([('foo', 0, asn1.CHOICE (
    [('genericElementSetName', 0, InternationalString),
     ('databaseSpecific', 1, asn1.SEQUENCE_OF (dbspec))]))])

PresentStatus = asn1.TYPE (27, asn1.INTEGER)
# 0 success, partial-1 to partial-4, 5 failure

DefaultDiagFormat = asn1.SEQUENCE ([
    ('diagnosticSetId', None, asn1.OID),
    ('condition', None, asn1.INTEGER),
    ('addinfo', None,
     asn1.CHOICE ([('v2Addinfo', None, asn1.VisibleString),
                   ('v3Addinfo', None, asn1.GeneralString)]))])

# DiagRec must be DefaultDiagFormat if v2 in effect
DiagRec = asn1.CHOICE ([('defaultFormat', None, DefaultDiagFormat),
                        ('externallyDefined', None, asn1.EXTERNAL)])

NamePlusRecord = asn1.SEQUENCE ([
    ('name', 0, DatabaseName, 1),
    ('record', asn1.EXPLICIT(1),
     asn1.CHOICE ([('retrievalRecord', asn1.EXPLICIT(1), asn1.EXTERNAL),
                   ('surrogateDiagnostic', asn1.EXPLICIT(2),
                    DiagRec)]))]) # and more for v3 l2 seg XXX

Records = asn1.CHOICE ([
    ('responseRecords', 28, asn1.SEQUENCE_OF (NamePlusRecord)),
    ('nonSurrogateDiagnostic', 130, DefaultDiagFormat),
    ('multipleNonSurDiagnostics', 205, asn1.SEQUENCE_OF (DiagRec))])

AttributeSetId = asn1.OID

AttributeElement = asn1.SEQUENCE ([
    ('attributeSet', 1, AttributeSetId, 1),
    ('attributeType', 120, asn1.INTEGER),
    ('attributeValue', None, asn1.CHOICE([('numeric', 121, asn1.INTEGER),
                                          ('complex', 224, asn1.SEQUENCE([
    ('list', 1, asn1.SEQUENCE_OF(StringOrNumeric)),
    ('semanticAction', 2, asn1.SEQUENCE_OF(asn1.INTEGER))
    ])
                                           )
                                          ]
                                         )
     )
    ])


Term = asn1.TYPE (45, asn1.OCTSTRING) # XXX v3 allows more generality
                                        
AttributeList = asn1.TYPE (44, asn1.SEQUENCE_OF (AttributeElement))

apt = asn1.SEQUENCE ([('attributes', None, AttributeList),
                      ('term', None, Term)])
AttributesPlusTerm = asn1.TYPE (102, apt)

ResultSetId = asn1.TYPE (31, InternationalString)

resSetPlusAttr = asn1.SEQUENCE ([('resultSet', None, ResultSetId),
                                 ('attributes', None, AttributeList)])

ResultSetPlusAttributes = asn1.TYPE (214, resSetPlusAttr)

Operand = asn1.CHOICE ([('attrTerm', None, AttributesPlusTerm),
                        ('resultSet', None, ResultSetId),
                        ('resultAttr', None, ResultSetPlusAttributes)])

KnownProximityUnit = asn1.INTEGER

ProximityOperator = asn1.SEQUENCE([
    ('exclusion', 1, asn1.BOOLEAN, 1),
    ('distance', 2, asn1.INTEGER),
    ('ordered', 3, asn1.BOOLEAN),
    ('relationType', 4, asn1.INTEGER),
    ('proximityUnitCode', asn1.EXPLICIT(5), asn1.CHOICE([
        ('known', 1, asn1.INTEGER),
        ('private', 2, asn1.INTEGER)
        ])
     )
    ])

Operator = asn1.TYPE (asn1.EXPLICIT(46),
                      asn1.CHOICE ([('and', 0, asn1.NULL),
                                    ('or', 1, asn1.NULL), 
                                    ('and-not', 2, asn1.NULL),
                                    ('prox', 3, ProximityOperator)
                                    ]))

RPNStructure = asn1.CHOICE ([('op', asn1.EXPLICIT(0), Operand),
                             ('rpnRpnOp', 1, asn1.SEQUENCE ([]))])

# set_arm is hack to allow recursive data structure.

rpnRpnOp = asn1.SEQUENCE ([('rpn1', None, RPNStructure),
                           ('rpn2', None, RPNStructure),
                           ('op', None, Operator)])

RPNStructure.set_arm (1, ('rpnRpnOp', 1, rpnRpnOp))

RPNQuery = asn1.SEQUENCE ([('attributeSet', None, AttributeSetId),
                                ('rpn', None, RPNStructure)])

Query = asn1.CHOICE ([
    ('type-0', asn1.EXPLICIT(0), asn1.ANY),
    ('type-1', 1, RPNQuery),
    ('type-2', asn1.EXPLICIT(2), asn1.OCTSTRING),
    # but works w/ yaz server, LC?
    ('type-100', asn1.EXPLICIT(100), asn1.OCTSTRING),
    ('type-101', 101, RPNQuery),
    ('type-102', asn1.EXPLICIT(102), asn1.OCTSTRING),
    ('type-104', 104, asn1.EXTERNAL)
    ])

SearchRequest = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId, 1),
    ('smallSetUpperBound', 13, asn1.INTEGER),
    ('largeSetLowerBound', 14, asn1.INTEGER),
    ('mediumSetPresentNumber', 15, asn1.INTEGER),
    ('replaceIndicator', 16, asn1.BOOLEAN),
    ('resultSetName', 17, InternationalString),
    ('databaseNames', 18, asn1.SEQUENCE_OF (DatabaseName)),
    ('smallSetElementSetNames', 100, ElementSetNames, 1),
    ('mediumSetElementSetNames', 101, ElementSetNames, 1),
    ('preferredRecordSyntax', 104, asn1.OID, 1),
    ('query', asn1.EXPLICIT(21), Query),
    ('additionalSearchInfo', 203, OtherInformation, 1),
    ('otherInfo', None, OtherInformation, 1)])

SearchResponse = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId, 1),
    ('resultCount', 23, asn1.INTEGER),
    ('numberOfRecordsReturned', 24, asn1.INTEGER),
    ('nextResultSetPosition', 25, asn1.INTEGER),
    ('searchStatus', 22, asn1.BOOLEAN),
    ('resultSetStatus', 26, asn1.INTEGER,1), # 1 subset, 2 interim, 3 none
    ('presentStatus', None, PresentStatus, 1),
    ('records', None, Records, 1),
    ('additionalSearchInfo', 203, OtherInformation, 1),
    ('otherInfo', None, OtherInformation, 1)])

Range = asn1.SEQUENCE ([('startingPosition', 1, asn1.INTEGER),
                         ('numberOfRecords', 2, asn1.INTEGER)])

PresentRequest = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId, 1),
    ('resultSetId', None, ResultSetId),
    ('resultSetStartPoint', 30, asn1.INTEGER),
    ('numberOfRecordsRequested', 29, asn1.INTEGER),
    ('additionalRanges', 212, asn1.SEQUENCE_OF (Range), 1),
    ('recordComposition', 19, ElementSetNames,1), # XXX or CHOICE if v3
    ('preferredRecordSyntax', 104, asn1.OID, 1),
    ('maxSegmentCount', 204, asn1.INTEGER, 1),
    ('maxRecordSize', 206, asn1.INTEGER, 1),
    ('maxSegmentSize', 207, asn1.INTEGER, 1),
    ('otherInfo', None, OtherInformation, 1)])


PresentResponse = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId,1),
    ('numberOfRecordsReturned', 24, asn1.INTEGER),
    ('nextResultSetPosition', 25, asn1.INTEGER),
    ('presentStatus', None, PresentStatus),
    ('records', None, Records, 1),
    ('otherInfo', None, OtherInformation, 1)])

DeleteSetStatus = asn1.TYPE (33, asn1.INTEGER)
# DeleteSetStatus values: 
#                            success                             (0),
#                            resultSetDidNotExist                (1),
#                            previouslyDeletedByTarget           (2),
#                            systemProblemAtTarget               (3),
#                            accessNotAllowed                    (4),
#                            resourceControlAtOrigin             (5),
#                            resourceControlAtTarget             (6),
#                            bulkDeleteNotSupported              (7),
#                            notAllRsltSetsDeletedOnBulkDlte     (8),
#                            notAllRequestedResultSetsDeleted    (9),
#                            resultSetInUse                      (10)}

ListStatuses = asn1.SEQUENCE_OF (
    asn1.SEQUENCE ([
    ('id', None, ResultSetId),
    ('status', None, DeleteSetStatus)]))


DeleteResultSetRequest = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId, 1),
    ('deleteFunction', 32, asn1.INTEGER), # 0 for list, 1 for all
    ('resultSetList', None, asn1.SEQUENCE_OF (ResultSetId), 1),
    ('otherInfo', None, OtherInformation, 1)])
    
DeleteResultSetResponse = asn1.SEQUENCE ([
    ('referenceId', None, ReferenceId, 1),
    ('deleteOperationStatus', 0, DeleteSetStatus),
    ('deleteListStatuses', 1, ListStatuses, 1),
    ('numberNotDeleted', 34, asn1.INTEGER, 1),
    ('bulkStatuses', 35, ListStatuses, 1),
    ('deleteMessage', 36, InternationalString),
    ('otherInfo', None, OtherInformation, 1)])

byDb = asn1.SEQUENCE_OF (asn1.SEQUENCE ([
    ('db', None, DatabaseName),
    ('num', 1, asn1.INTEGER, 1),
    ('otherDbInfo', None, OtherInformation, 1)]))

occur = asn1.CHOICE ([
    ('global', asn1.EXPLICIT (2), asn1.INTEGER),
    ('byDatabase', 3, byDb)])

OccurrenceByAttributes = asn1.SEQUENCE_OF (
    asn1.SEQUENCE ([
    ('attributes', asn1.EXPLICIT (1),AttributeList),
    ('occurrences', None, occur, 1),
    ('otherOccurInfo', None,  OtherInformation, 1)]))

TermInfo = asn1.SEQUENCE  ([
   ('term', None, Term),
   ('displayTerm', 0,  InternationalString, 1),
   ('suggestedAttributes', None, AttributeList, 1),
   ('alternativeTerm', 4, asn1.SEQUENCE_OF (AttributesPlusTerm), 1),
   ('globalOccurrences', 2, asn1.INTEGER, 1),
   ('byAttributes', 3, OccurrenceByAttributes,1),
   ('otherTermInfo', None, OtherInformation, 1)])

Entry = asn1.CHOICE ([
   ('termInfo', 1, TermInfo),
   ('surrogateDiagnostic', asn1.EXPLICIT (2),   DiagRec)])

ListEntries = asn1.SEQUENCE ([
    ('entries', 1, asn1.SEQUENCE_OF (Entry), 1),
    ('nonsurrogateDiagnostics', 2, asn1.SEQUENCE_OF (DiagRec), 1)])


ScanRequest = asn1.SEQUENCE ([
   ('referenceId', None, ReferenceId, 1),
   ('databaseNames', 3, asn1.SEQUENCE_OF (DatabaseName)),
   ('attributeSet', None, AttributeSetId, 1),
   ('termListAndStartPoint', None, AttributesPlusTerm),
   ('stepSize', 5, asn1.INTEGER, 1),
   ('numberOfTermsRequested', 6, asn1.INTEGER),
   ('preferredPositionInResponse', 7, asn1.INTEGER, 1),
   ('otherInfo', None, OtherInformation, 1)])

ScanResponse = asn1.SEQUENCE ([
   ('referenceId', None, ReferenceId, 1),
   ('stepSize', 3, asn1.INTEGER, 1),
   ('scanStatus', 4, asn1.INTEGER),
   ('numberOfEntriesReturned', 5, asn1.INTEGER),
   ('positionOfTerm', 6, asn1.INTEGER,1),
   ('entries', 7, ListEntries, 1),
   ('attributeSet', 8, AttributeSetId, 1),
   ('otherInfo', None, OtherInformation, 1)])

# Specification page 232 of v3-2002
Specification = asn1.SEQUENCE ([
    ('schema', None,
     asn1.CHOICE([
       ('oid', 1, asn1.OID),
       ('uri', 300, InternationalString)
       ]), 1),
    ('elementSpec', asn1.EXPLICIT(2),
     asn1.CHOICE([
       ('elementSetName', 1, InternationalString),
       ('externalEspec', 2, asn1.EXTERNAL)
       ]), 1)
    ])

#page 226 v3-2002 (235 in PDF)
#page 60 v3-1995 (part2.pdf, page 12)

SortKey = asn1.CHOICE ([
    ('privateSortKey', 0, InternationalString),
    ('elementSpec', 1, Specification),
    ('sortAttributes', 2,
     asn1.SEQUENCE([
      ('id', None, AttributeSetId),
      ('list', None, AttributeList)
      ])
     )
    ])

SortElement = asn1.CHOICE ([
    ('generic', asn1.EXPLICIT(1), SortKey),
    ('databaseSpecific', 2, asn1.SEQUENCE_OF(
      asn1.SEQUENCE(
        ([
          ('databaseName', None, DatabaseName),
          ('dbSort', None, SortKey)
          ])
        )
      )
     )
    ])

SortKeySpec = asn1.SEQUENCE ([
    ('sortElement', None, SortElement),
    ('sortRelation', 1, asn1.INTEGER),
    ('caseSensitivity', 2, asn1.INTEGER),
    ('missingValueAction', asn1.EXPLICIT(3), asn1.CHOICE([
      ('abort', 1, asn1.NULL),
      ('null', 2, asn1.NULL),
      ('missingValueAction', 3, asn1.OCTSTRING)
      ]), 1)
    ])

SortRequest = asn1.SEQUENCE ([
    ('referenceId', 2, ReferenceId, 1),
    ('inputResultSetNames', 3, asn1.SEQUENCE_OF (InternationalString)),
    ('sortedResultSetName', 4, InternationalString),
    ('sortSequence', 5, asn1.SEQUENCE_OF(SortKeySpec)),
    ('otherInfo', None, OtherInformation, 1)])

SortResponse = asn1.SEQUENCE ([
    ('referenceId', 2, ReferenceId, 1),
    ('sortStatus', 3, asn1.INTEGER),
    ('resultSetStatus', 4, asn1.INTEGER, 1),
    ('diagnostics', 5, asn1.SEQUENCE_OF(DiagRec), 1),
    ('resultCount', 6, asn1.INTEGER, 1),
    ('otherInfo', None, OtherInformation, 1)])




PDU = asn1.CHOICE ([('initRequest', 20, InitializeRequest),
                    ('initResponse', 21, InitializeResponse),
                    ('searchRequest', 22, SearchRequest),
                    ('searchResponse', 23, SearchResponse),
                    ('presentRequest', 24, PresentRequest),
                    ('presentResponse', 25, PresentResponse),
                    ('deleteResultSetRequest', 26, DeleteResultSetRequest),
                    ('deleteResultSetResponse', 27, DeleteResultSetResponse),
                    ('scanRequest', 35, ScanRequest),
                    ('scanResponse', 36, ScanResponse),
                    ('sortRequest', 43, SortRequest),
                    ('sortResponse', 44, SortResponse),
                    ('close', 48, Close)])


# start GRS-1 defs

Order = asn1.SEQUENCE ([
    ('ascending', 1, asn1.BOOLEAN),
    ('order', 2, asn1.INTEGER)])

Usage = asn1.SEQUENCE ([
    ('type', 1, asn1.INTEGER), # 1 is redistributable, 2 restricted, 3 license
    ('restriction', 2, InternationalString, 1)])

HitVector = asn1.SEQUENCE ([
    ('satisfier', None, Term, 1),
    ('offsetIntoElement', 1, IntUnit),
    ('length', 2, IntUnit, 1),
    ('hitRank', 3, asn1.INTEGER, 1),
    ('targetToken', 4, asn1.OCTSTRING)])

variantval = asn1.CHOICE ([
    # hey! these don't have choice arm names in orig XXX
    ('int', None, asn1.INTEGER),
    ('str', None, InternationalString),
    ('oct', None, asn1.OID),
    ('bool', None, asn1.BOOLEAN),
    ('nul', None, asn1.NULL) # ignore unit, valueAndUnit for now
    ])

Variant = asn1.SEQUENCE ([
    ('globalVariantSetId', 1, asn1.OID, 1),
    ('triples', 2, asn1.SEQUENCE_OF (asn1.SEQUENCE ([
    ('variantSetId', 0, asn1.OID, 1),
    ('class', 1, asn1.INTEGER),
    ('type', 2, asn1.INTEGER),
    ('value', 3, variantval)])))])
    

TagPath = asn1.SEQUENCE_OF (asn1.SEQUENCE ([
    ('tagType', 1, asn1.INTEGER, 1),
    ('tagValue', 2, StringOrNumeric),
    ('tagOccurrence', 3, asn1.INTEGER, 1)]))


ElementData = asn1.CHOICE ([
    ('octets', None, asn1.OCTSTRING),
    ('numeric', None, asn1.INTEGER),
    ('date', None, asn1.GeneralizedTime),
    ('ext', None, asn1.EXTERNAL),
    ('string', None, asn1.GeneralString),
    # 'string' is defined as InternationalString, but b/c GRS-1 is only
    # defined in Z39.50 1995, this means GeneralString.
    # see http://lcweb.loc.gov/z3950/agency/clarify/sutrs.html for a
    # similar issue.
    ('trueOrFalse', None, asn1.BOOLEAN),
    ('oid', None, asn1.OID),
    ('intUnit', 1, IntUnit),
    ('elementNotThere', 2, asn1.NULL),
    ('elementEmpty', 3, asn1.NULL),
    ('noDataRequested', 4, asn1.NULL),
    ('diagnostic', 5, asn1.EXTERNAL),
    ('subtree', 6, asn1.SEQUENCE_OF (None))]) # set None to TaggedElement below

ElementMetaData = asn1.SEQUENCE ([
    ('seriesOrder', 1, Order, 1),
    ('usageRight', 2, Usage, 1),
    ('hits', 3, asn1.SEQUENCE_OF(HitVector), 1),
    ('displayName', 4, InternationalString, 1),
    ('supportedVariants', 5, asn1.SEQUENCE_OF(Variant),1),
    ('message', 6, InternationalString, 1),
    ('elementDescriptor', 7, asn1.OCTSTRING, 1),
    ('surrogateFor', 8, TagPath, 1),
    ('surrogateElement', 9, TagPath, 1),
    ('other', 99, asn1.EXTERNAL, 1)])

TaggedElement = asn1.SEQUENCE ([
    ('tagType', 1, asn1.INTEGER, 1),
    ('tagValue', asn1.EXPLICIT(2), StringOrNumeric),
    ('tagOccurrence', 3, asn1.INTEGER, 1),
    ('content', asn1.EXPLICIT(4),  ElementData),
    ('metaData', 5, ElementMetaData, 1), # set to ElementMetaData below
    ('appliedVariant', 6, Variant, 1)
    ])

ElementData.set_arm (12, ('subtree', asn1.EXPLICIT(6),
                          asn1.SEQUENCE_OF (TaggedElement)))

GenericRecord = asn1.SEQUENCE_OF(TaggedElement)

asn1.register_oid (asn1.OidVal (Z3950_RECSYN_GRS1), GenericRecord)

# OPAC stuff


Volume = asn1.SEQUENCE ([
    ('enumeration', 1, InternationalString, 1),
    ('chronology', 2, InternationalString, 1),
    ('enumAndChron', 3,  InternationalString, 1)])


CircRecord = asn1.SEQUENCE ([
   ('availableNow', 1, asn1.BOOLEAN),
   ('availablityDate', 2,  InternationalString, 1), 
   ('availableThru', 3, InternationalString, 1),
   ('restrictions', 4, InternationalString, 1),
   ('itemId', 5, InternationalString, 1),
   ('renewable', 6, asn1.BOOLEAN),
   ('onHold', 7, asn1.BOOLEAN),
   ('enumAndChron', 8, InternationalString, 1),
   ('midspine', 9, InternationalString, 1),
   ('temporaryLocation', 10, InternationalString, 1)])

HoldingsAndCircData = asn1.SEQUENCE ([
    ('typeOfRecord', 1, InternationalString, 1),
    ('encodingLevel', 2, InternationalString, 1),
    ('format', 3, InternationalString, 1),
    ('receiptAcqStatus', 4, InternationalString, 1),
    ('generalRetention', 5, InternationalString, 1),
    ('completeness', 6, InternationalString, 1),
    ('dateOfReport', 7, InternationalString, 1),
    ('nucCode', 8, InternationalString, 1),
    ('localLocation', 9, InternationalString, 1),
    ('shelvingLocation', 10, InternationalString, 1),
    ('callNumber', 11, InternationalString, 1),
    ('shelvingData', 12, InternationalString, 1),
    ('copyNumber', 13, InternationalString, 1),
    ('publicNote', 14, InternationalString, 1),
    ('reproductionNote', 15, InternationalString, 1), 
    ('termsUseRepro', 16, InternationalString, 1),
    ('enumAndChron', 17, InternationalString, 1),
    ('volumes', 18, asn1.SEQUENCE_OF (Volume), 1),
    ('circulationData', 19, asn1.SEQUENCE_OF (CircRecord), 1)])

HoldingsRecord = asn1.CHOICE ([
    ('marcHoldingsRecord', 1, asn1.EXTERNAL),
    ('holdingsAndCirc', 2, HoldingsAndCircData)])


OPACRecord = asn1.SEQUENCE ([
    ('bibliographicRecord', 1, asn1.EXTERNAL, 1),
    ('holdingsData', 2, asn1.SEQUENCE_OF (HoldingsRecord), 1)])

asn1.register_oid (asn1.OidVal (Z3950_RECSYN_OPAC), OPACRecord)

# User Info Format Search Result stuff

ResultsByDB = asn1.SEQUENCE_OF (asn1.SEQUENCE ([
    ('databases', asn1.EXPLICIT(1),
     asn1.CHOICE ([
    ('all', 1, asn1.NULL),
    ('list', 2, asn1.SEQUENCE_OF (DatabaseName))])),
    ('count', 2, asn1.INTEGER, 1),
    ('resultSetName', 3, InternationalString, 1)]))
     
QueryExpression = asn1.CHOICE ([
    ('term', 1, asn1.SEQUENCE ([
    ('queryTerm', asn1.EXPLICIT (1), Term),
    ('termComment', 2, InternationalString, 1)
    ])),
    ('query', asn1.EXPLICIT(2), Query)])

SearchInfoReport = asn1.SEQUENCE_OF (asn1.SEQUENCE ([
    ('subqueryId', 1, InternationalString, 1),
    ('fullQuery', 2, asn1.BOOLEAN),
    ('subqueryExpression',  asn1.EXPLICIT(3), QueryExpression,  1),
    ('subqueryInterpretation', asn1.EXPLICIT(4), QueryExpression, 1),
    ('subqueryRecommendation', asn1.EXPLICIT(5),  QueryExpression, 1),
    ('subqueryCount', 6, asn1.INTEGER, 1),
    ('subQueryWeight', 7, IntUnit, 1),
    ('resultsByDB', 8, ResultsByDB, 1)]))

asn1.register_oid (asn1.OidVal (Z3950_USR_SEARCHRES1),
                   SearchInfoReport)

class Version (asn1.BitStringVal):
    def __init__ (self, usev3 = 1):
        v1 = 0
        v2 = 1
        v3 = 2
        asn1.BitStringVal.__init__ (self, v3)
        self.set_bits ((v1, v2))
        if usev3: self.set_bits ((v3,))
    def __repr__ (self):
        return "Version bits %d" % self.bits

class OptionsVal (asn1.BitStringVal):
    search = 0
    present = 1
    delSet = 2
    resourceReport = 3
    triggerResourceCtrl = 4
    resourceCtrl = 5
    accessCtrl = 6
    scan = 7
    sort = 8
    extendedServices = 10
    l1Seg = 11
    l2Seg = 12
    concurrentOperations = 13
    namedResultSets = 14
    encapsulation = 15
    resultCountInSort = 16
    negotiation = 17 # http://lcweb.loc.gov/z3950/agency/nego.html
    dedup = 18 # Pg 218, V3-200X
    query_104 = 19
    pQESCorrection = 20
    stringSchema = 21
    top_ind = 21

    def __init__ (self):
        asn1.BitStringVal.__init__ (self, self.top_ind)
        # named result sets is max
        self.set_bits ((self.search, self.present))
    def __repr__ (self):
        rev = {}
        for k,v in self.__class__.__dict__.items ():  # XXX really awful hack
            if k[0] <> '_':
                rev [v] = k
        l = []
        for i in range (self.search, self.top_ind):
            if self.is_set (i):
                l.append (rev [i])
        return "OptionsVal: " + ",".join (l)

