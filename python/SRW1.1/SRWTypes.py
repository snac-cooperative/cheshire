
# Objects and TypeCodes for SRW/SRU operations
# Version: 1.1
# Copyright: Rob Sanderson (azaroth@liv.ac.uk)

from ZSI import *
from PyZ3950 import SRWDiagnostics
from PyZ3950.SRWDiagnostics import SRWDiagnostic
from xml.dom.minidom import Node, parseString
import types

# ----- New Types for ZSI Parsing and Serialization -----

# Fix for XML serialization to respect unsuppressedPrefixes
try:
    from xml.dom.ext import Canonicalize
    from xml.ns import SCHEMA, SOAP
except:
    from ZSI.compat import Canonicalize, SCHEMA, SOAP

def NewXMLSerialization(self, sw, pyobj, name=None, attrtext='', **kw):
        if not self.wrapped:
            Canonicalize(pyobj, sw, comments=self.comments, unsuppressedPrefixes=kw.get('unsuppressedPrefixes', []))
            return
        objid = '%x' % id(pyobj)
        n = name or self.oname or ('E' + objid)
        if type(pyobj) in _stringtypes:
            print >>sw, '<%s%s href="%s"/>' % (n, attrtext, pyobj)
        elif kw.get('inline', self.inline):
            self.cb(sw, pyobj)
        else:
            print >>sw, '<%s%s href="#%s"/>' % (n, attrtext, objid)
            sw.AddCallback(self.cb, pyobj)

TC.XML.serialize = NewXMLSerialization

# New Array types for (stupid) schema without typed arrays
# Records, Diagnostics, Sortkeys
# (Or Robert David Sanderson, if you prefer :) )
class RDSArray(TC.Array):
    "Array like type for Records, Diagnostics and SortKeys"

    def serialize(self, sw, pyobj, name=None, attrtext='', childnames=None, **kw):
        "Serialise without arrayType, etc."

        objid = '%x' % id(pyobj)
        n = name or self.oname or ('E' + objid)
        if self.unique:
            idtext = ''
        else:
            idtext = ' id="%s"' % objid

        print >>sw, '<%s%s%s>' % \
                (n, attrtext, idtext)
        d = {}
        kn = childnames or self.childnames
        if kn:
            d['name'] = kn
        elif not self.ofwhat.aname:
            d['name'] = 'element'
        position = 0
        for v in pyobj:
            self.ofwhat.serialize(sw, v, **d)
        print >>sw, '</%s>' % n

class XMLorString(TC.String):
    "XML/String 'choice' for recordData"
    def serialize(self, sw, pyobj, name=None, attrtext='', **kw):
        if pyobj.lstrip()[0] == '<':
            self.tag = "XMLFragment"
        else:
            self.tag = "string"
        return TC.String.serialize(self, sw, pyobj, name, attrtext, **kw)

    def parse(self, elt, ps):
        type = elt.getAttributeNS('http://www.w3.org/2001/XMLSchema-instance', 'type')

        if (len(elt.childNodes) == 1):
            if (elt.childNodes[0].nodeType == Node.ELEMENT_NODE):
                return elt.childNodes[0]
            else:
                return elt.childNodes[0].data
        else:
            # Strip out empty data sections
            nodes = []
            for c in elt.childNodes:
                if (c.nodeType == Node.TEXT_NODE and c.data.isspace()):
                    continue
                nodes.append(c)
            return nodes

# -----------


def generateExtraData(extra, config):
    # Parse each individually to prevent badly named params
    nodes  = []
    for k in (extra):
        xml = "<%s>%s</%s>" % (k, extra[k], k)
        try:
            dom = parseString(xml)
            nodes.append(dom.childNodes[0])
        except:
            # Uhh... ?
            pass
    return nodes

# ----- Sub Classes for Response -----

class Record:
    "Object to represent an SRW Record"
    name = ""
    recordSchema = ""
    recordData = ""
    recordPosition = 0
    inline=1

    def __init__(self,n, **kw):
        self.name = n
        self.inline =1

Record.typecode = TC.Struct(Record,
                            (TC.String('srw:recordSchema', inline=1, unique=1, typed=0),
                             TC.String('srw:recordPacking', inline=1, unique=1, typed=0),
                             XMLorString('srw:recordData', inline=1, unique=1, textprotect=0, typed=0),
                             TC.IpositiveInteger('srw:recordPosition', optional=1, unique=1, typed=0),
                             RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRecordData', optional=1, unique=1, inline=1, undeclared=1)
                             ),
                            'srw:record', inline=1, optional=1
                            )

class SortKey:
    "Object to represent a sort key"
    name = ""
    path = ""
    schema = ""
    ascending = 1
    caseSensitive = 0
    missingValue = ""

    def __init__(self, name, **kw):
        self.name = name
        

SRWDiagnostic.typecode = \
                       TC.Struct(SRWDiagnostic,
                                 (TC.String('diag:uri', unique=1, inline=1, typed=0),
                                  TC.String('diag:details', unique=1, inline=1, typed=0, optional=1),
                                  TC.String('diag:message', unique=1, inline=1, typed=0, optional=1)
                                  ),
                                 'diag:diagnostic', inline=1)

class ScanTerm:
    name ='term'
    value = ""
    numberOfRecords = 0
    displayTerm = ""
    whereInList = ""
    extraTermData = None

    def __init__(self, name, **kw):
        self.name = name
        if (kw.has_key('opts')):
            opts = kw['opts']
            self.value = opts['term'][1]
            if (opts.has_key('freq')):
                self.numberOfRecords = int(opts['freq'])
            if (opts.has_key('display')):
                self.displayTerm = opts['display']
            

ScanTerm.typecode = \
                  TC.Struct(ScanTerm,
                            (TC.String('srw:value', optional=1, unique=1, typed=0),
                             TC.InonNegativeInteger('srw:numberOfRecords', optional=1, unique=1, typed=0),
                             TC.String('srw:displayTerm', optional=1, unique=1, typed=0),
                             TC.String('srw:whereInList', optional=1, unique=1, typed=0),
                             RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraTermData', optional=1, unique=1, inline=1, undeclared=1)
                             ),
                            'srw:term', inline=1, unique=1, typed=0
                            )


# -----------------------------


class ExplainResponse:
    name = ""
    version = "1.1"
    record = None
    diagnostics = []
    extraResponseData = []
    echoedExplainRequest = None

    def __str__(self):
        return "ExplainResponse:\n    Version: %s\n    Record (presence of): %i\n    Diagnostics: %s\n    ExtraResponseData: %s" % (self.version, self.record <> None, repr(self.diagnostics), repr(self.extraResponseData))

    def __init__(self, n, **kw):
        self.name = n
        self.version = "1.1"

class ExplainRequest:
    name = ""
    responseName = "explainResponse"
    responseType = ExplainResponse
    defaultStylesheet = ""
    defaultRecordPacking = "string"

    version = ""
    recordPacking ="string"
    stylesheet= None
    extraRequestData = None

    def __str__(self):
        return "ExplainRequest:\n    Version: %s\n    RecordPacking: %s\n    Stylesheet: %s\n    ExtraRequestData: %s" % (self.version, self.recordPacking, self.stylesheet, repr(self.extraRequestData))

    def __init__(self, n, config=None, opts={}, protocol="SRW"):
        self.name = n
        if config <> None:
            self.configure(config)
        if (protocol == "SRU"):
            self.recordPacking = "xml"

        extra = {}
        for k in opts:
            if (type(opts[k]) != types.ListType):
                opts[k] = [opts[k]]
            if (k[:2] == "x-"):
                extra[k[2:]] = opts[k]
            else:
                setattr(self, k, opts[k][0])
        if extra:
            self.extraRequestData = generateExtraData(extra, config)

    def configure(self, config):
        self.config = config

    def get(self, name):
        g = getattr(self, name)
        if (g <> None):
            return g
        else:
            g = getattr(self, "default%s%s" % (name[0].upper(), name[1:]))
            return g
                            

ExplainRequest.typecode = \
                        TC.Struct(ExplainRequest,
                                  (TC.String('srw:version', unique=1, typed=0),
                                   TC.String('srw:recordPacking', optional=1, unique=1, typed=0),
                                   TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
                                   RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
                                   ), 'srw:explainRequest', inline=1
                                  )


ExplainResponse.typecode = \
                         TC.Struct(ExplainResponse,
                                   (TC.String('srw:version', unique=1, inline=1, typed=0),
                                    Record.typecode,
                                    RDSArray('srw:diagnosticType',
                                             SRWDiagnostic.typecode,
                                             'srw:diagnostics', optional = 1, undeclared=1, typed=0, inline=1),
                                    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraResponseData', optional=1, unique=1, inline=1, undeclared=1),
                                    TC.Struct(ExplainRequest,
                                              (TC.String('srw:version', unique=1, typed=0),
                                               TC.String('srw:recordPacking', optional=1, unique=1, typed=0),
                                               TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
                                               RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)),
                                              'srw:echoedExplainRequest', inline=1, unique=1, optional=1)
                                    ), 'srw:explainResponse', inline=1
                                   )



class SearchRetrieveResponse:
    """ Object to represent a response """
    name = ""
    version = "1.1"
    numberOfRecords = 0

    resultSetId = ""
    resultSetIdleTime = 0
    records = []
    nextRecordPosition = 0
    extraResponseData = []
    echoedSearchRetrieveRequest = None

    def __str__(self):
        return "SearchRetrieveResponse:\n    Version: %s\n    NumberOfRecords: %i\n    ResultSetId: %s\n    ResultSetIdleTime: %s\n    Records (length): %i\n    NextRecordPosition: %i\n    ExtraResponseData: %s" % (self.version, self.numberOfRecords, self.resultSetId, self.resultSetIdleTime, len(self.records), self.nextRecordPosition, repr(self.extraResponseData))

    def __init__(self, n, **kw):
        self.name = n
        self.version="1.1"
        self.numberOfRecords=0
        self.records = []


class SearchRetrieveRequest:
    "Object to represent a request"
    name = ""
    responseName = "searchRetrieveResponse"
    responseType = SearchRetrieveResponse
    integerParams = ['resultSetTTL', 'startRecord', 'maximumRecords']

    version = ""
    stylesheet = ""
    extraRequestData = []
    query = ""
    startRecord = None
    maximumRecords = None
    recordPacking = None
    recordSchema = None
    recordXPath = None
    resultSetTTL = None
    sortKeys = None

    # Need these for processing and echo
    xSortKeys = []
    xQuery = None
    queryStructure = None
    defaultStylesheet = ""
    defaultRecordSchema = "dc"
    defaultMaximumRecords = 1
    defaultStartRecord = 1
    defaultRecordPacking = "string"
    defaultRecordXPath = ""
    defaultResultSetTTL = 0
    defaultSortKeys = ""
    diagnostics = []

    def __str__(self):
        return "SearchRetrieveRequest:\n    Version: %s\n    Query: %s\n    StartRecord: %s\n    MaximumRecords: %s\n    RecordPacking: %s\n    RecordSchema: %s\n    RecordXPath: %s\n    ResultSetTTL: %s\n    SortKeys: %s\n    Stylesheet: %s\n    ExtraRequestData: %s" % (self.version, self.query, self.startRecord, self.maximumRecords, self.recordPacking, self.recordSchema, self.recordXPath, self.resultSetTTL, self.sortKeys, self.stylesheet, repr(self.extraRequestData))


    def __init__(self, name, config=None, opts={}, protocol="SRW"):
        self.name = name

        if config <> None:
            self.configure(config)
        if (protocol == "SRU"):
            self.defaultRecordPacking = "xml"

        self.diagnostics = []
        extra = {}
        for k in opts:
            if (type(opts[k]) != types.ListType):
                opts[k] = [opts[k]]
            if (k in self.integerParams):
                try:
                    setattr(self, k, int(opts[k][0]))
                except ValueError:
                    # non integer value
                    diag = SRWDiagnostics.Diagnostic6()
                    diag.details = k
                    diag.message = "Invalid parameter value: Could not turn '%s' into an integer." % (opts[k][0])
                    self.diagnostics.append(diag)
            elif (len(k) > 2 and k[0:2] == "x-"):
                extra[k[2:]] = opts[k][0]
            else:
                setattr(self, k, opts[k][0])
        if (extra):
            # Process ExtraRequestData
            self.extraRequestData = generateExtraData(extra, config)

    def configure(self, config):
        self.config = config
        self.defaultRecordSchema = config.defaultRetrieveSchema
        self.defaultMaximumRecords = config.defaultNumberOfRecords

    def get(self, name):
        g = getattr(self, name)
        if (g <> None):
            return g
        else:
            g = getattr(self, "default%s%s" % (name[0].upper(), name[1:]))
            return g

SearchRetrieveRequest.typecode =  \
TC.Struct(SearchRetrieveRequest,    
          (TC.String('srw:version', unique=1, typed=0),
           TC.String('srw:query', unique=1, typed=0),  
           TC.IpositiveInteger('srw:startRecord', optional=1, unique =1, typed=0),
           TC.InonNegativeInteger('srw:maximumRecords', optional=1, unique=1, typed=0),
           TC.String('srw:recordPacking', optional=1, unique=1, typed=0),
           TC.String('srw:recordSchema', optional=1, unique=1, typed=0),
           TC.String('srw:recordXPath', optional=1, unique=1, typed=0),
           TC.InonNegativeInteger('srw:resultSetTTL', optional=1, unique=1, typed=0),
           TC.String('srw:sortKeys', optional=1, unique=1, typed=0),
           TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
           RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
            ), 'srw:searchRetrieveRequest', inline=1
          )

SearchRetrieveResponse.typecode = \
                                TC.Struct(SearchRetrieveResponse, (
    TC.String('srw:version', unique=1, inline=1, typed=0),
    TC.InonNegativeInteger('srw:numberOfRecords', unique=1, inline=1, typed=0),
    TC.String('srw:resultSetId', optional=1, unique=1, typed=0),
    TC.IpositiveInteger('srw:resultSetIdleTime', optional=1, unique=1, typed=0),
    RDSArray('srw:recordType',
             Record.typecode,
             'srw:records', optional=1, undeclared=1, inline=1, typed=0, unique=1),
    TC.IpositiveInteger('srw:nextRecordPosition', optional=1, unique=1, typed=0),
    RDSArray('srw:diagnosticType',
             SRWDiagnostic.typecode,
             'srw:diagnostics', optional = 1, undeclared=1, typed=0, inline=1, unique=1),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraResponseData', optional=1, unique=1, inline=1, undeclared=1),
    TC.Struct(SearchRetrieveRequest,
              (TC.String('srw:version', unique=1, inline=1, typed=0),
               TC.String('srw:query', unique=1, inline=1, typed=0),  
               XMLorString('srw:xQuery', optional =1, textprotect=0, unique=1, typed=0),
               TC.IpositiveInteger('srw:startRecord', optional=1, unique=1, typed=0),
               TC.InonNegativeInteger('srw:maximumRecords', optional=1, unique=1, typed=0),
               TC.String('srw:recordPacking', optional=1, unique=1, typed=0),
               TC.String('srw:recordSchema', optional=1, unique=1, typed=0),
               TC.String('srw:recordXPath', optional=1, unique=1, typed=0),
               TC.InonNegativeInteger('srw:resultSetTTL', optional=1, unique=1, typed=0),
               TC.String('srw:sortKeys', optional=1, unique=1, typed=0),
               TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
               RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1, unsuppressedPrefixes=[]), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
               ), 'srw:echoedSearchRetrieveRequest', inline=1, optional=1, unique=1),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraResponseData', optional=1, unique=1, inline=1, undeclared=1)
    ), 'srw:searchRetrieveResponse', inline=1)


class ScanResponse:
    name = 'scanResponse'

    version = "1.1"
    terms = []
    diagnostics = []
    extraResponseData = []
    echoedScanRequest = None

    def __str__(self):
        return "ScanResponse:\n    Version: %s\n    Terms (length): %d\n    Diagnostics: %s\n    ExtraResponseData: %s" % (self.version, len(self.terms), repr(self.diagnostics), repr(self.extraResponseData))
    
    def __init__(self, name, **kw):
        self.name = name
        self.version = "1.1"

class ScanRequest:
    name = "scanRequest"
    responseName = "scanResponse"
    responseType = ScanResponse
    integerParams = ['maximumTerms', 'responsePosition']
    
    version = None
    scanClause = None
    responsePosition = None
    maximumTerms = None
    stylesheet = None
    extraRequestData = None

    xScanClause = None
    defaultStylesheet = ""
    defaultMaximumTerms = 20
    defaultResponsePosition = 1
    diagnostics = []

    def __str__(self):
        return "ScanRequest:\n    Version: %s\n    ScanClause: %s\n    ResponsePosition: %s\n    MaximumTerms: %s\n    Stylesheet: %s\n    ExtraRequestData: %s" % (self.version, self.scanClause, self.responsePosition, self.maximumTerms, self.stylesheet, repr(self.extraRequestData))

    def __init__(self, name, config=None, opts={}, protocol="SRW"):
        self.name = name
        if (config <> None):
            self.configure(config)

        self.diagnostics = []
        extra = {}
        for k in opts:
            if (type(opts[k]) != types.ListType):
                opts[k] = [opts[k]]
            if (k in self.integerParams):
                try:
                    setattr(self, k, int(opts[k][0]))
                except ValueError:
                    # non integer value
                    diag = SRWDiagnostics.Diagnostic6()
                    diag.details = k
                    diag.message = "Invalid parameter value: Could not turn '%s' into an integer." % (opts[k][0])
                    self.diagnostics.append(diag)
            elif (k[:2] == "x-"):
                extra[k[2:]] = opts[k]
            else:
                setattr(self, k, opts[k][0])
        if (extra):
            self.extraRequestData = generateExtraData(extra, config)

    def configure(self, config):
        self.config = config
        try:
            self.defaultMaximumTerms=config.defaultNumberOfTerms
        except:
            self.defaultMaximumTerms = 20

    def get(self, name):
        g = getattr(self, name)
        if (g <> None):
            return g
        else:
            g = getattr(self, "default%s%s" % (name[0].upper(), name[1:]))
            return g
                            
                

ScanRequest.typecode = \
                     TC.Struct(ScanRequest,
                               (
    TC.String('srw:version', unique=1, inline=1, typed=0),
    TC.String('srw:scanClause', unique=1, typed=0),
    TC.InonNegativeInteger('srw:responsePosition', optional=1, unique=1, typed=0),
    TC.IpositiveInteger('srw:maximumTerms', optional=1, unique=1, typed=0),
    TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
    ),
                               'srw:scanRequest', inline=1
                               )


ScanResponse.typecode = \
TC.Struct(ScanResponse,
          (
    TC.String('srw:version', unique=1, inline=1, typed=0),
    RDSArray('srw:scanTermType', ScanTerm.typecode, 'srw:terms', unique=1, typed=0, undeclared=1, inline=1, optional=1),
    RDSArray('srw:diagnosticType', SRWDiagnostic.typecode, 'srw:diagnostics', optional = 1, undeclared=1, typed=0, inline=1),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraResponseData', optional=1, unique=1, inline=1, undeclared=1),
    TC.Struct(ScanRequest, (TC.String('srw:version', unique=1, inline=1, typed=0),
                            TC.String('srw:scanClause', unique=1, typed=0),
                            XMLorString('srw:xScanClause', optional =1, textprotect=0, unique=1, typed=0),
                            TC.InonNegativeInteger('srw:responsePosition', optional=1, unique=1, typed=0),
                            TC.IpositiveInteger('srw:maximumTerms', optional=1, unique=1, typed=0),
                            TC.String('srw:stylesheet', optional=1, unique=1, typed=0),
                            RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
                            ),
              'srw:echoedScanRequest', inline=1, optional=1
              ),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'srw:extraResponseData', optional=1, unique=1, inline=1, undeclared=1)
    ),
          'srw:scanResponse', inline=1
          )



# ------ Additional non standard stuff ------

class ResultSet:
    "Object to represent a resultSet"
    name = ""
    resultSetId = ""
    ttl = 0
    time = 0

    def __init__(self, name, **kw):
        self.name = name

class AuthToken:
    "Object to represent an Authentication Token"
    name = ""
    value = ""
    ttl = 0
    time = 0

    def __init__(self, name, **kw):
        self.name = name
        

