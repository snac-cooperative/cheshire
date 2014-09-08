
# Objects and TypeCodes for SRW/SRU operations
# Version: 1.1
# Copyright: Rob Sanderson (azaroth@liv.ac.uk)

from ZSI import *
from SRWDiagnostics import SRWDiagnostic
from xml.dom.minidom import Node, parseString
import types


def generateExtraData(extra, config):
    xml = ['<foo>']
    for k in (extra):
        xml.append("<%s>%s</%s>" % (k, extra[k], k))
    xml.append("</foo>")
    xml = ''.join(xml)
    dom = parseString(xml)
    return list(dom.childNodes[0].childNodes)


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
        if (type == "xsd:XMLFragment"):
            if (len(elt.childNodes) == 1):
                return elt.childNodes[0]
            else:
                return elt.childNodes
        else:
            return elt.childNodes[0].data

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
                            (TC.String('recordSchema', inline=1, unique=1),
                             TC.String('recordPacking', inline=1, unique=1),
                             XMLorString('recordData', inline=1, unique=1, textprotect=0),
                             TC.Integer('recordPosition', optional=1),
                             RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRecordData', optional=1, unique=1, inline=1, undeclared=1)
                             ),
                            'record', inline=1
                            )

SRWDiagnostic.typecode = \
                       TC.Struct(SRWDiagnostic,
                                 (TC.Integer('diag:code', unique=1, inline=1),
                                  TC.String('diag:details', unique=1, inline=1),
                                  TC.String('diag:message', unique=1, inline=1)
                                  ),
                                 'diag:diagnostic', inline=1)

class ExplainResponse:
    name = ""
    version = "1.1"
    record = None
    diagnostics = []
    extraResponseData = None

    def __init__(self, n, **kw):
        self.name = n
        self.version = "1.1"
    
ExplainResponse.typecode = \
                         TC.Struct(ExplainResponse,
                                   (TC.String('version', unique=1, inline=1),
                                    Record.typecode,
                                    RDSArray('srw:diagnosticType',
                                             SRWDiagnostic.typecode,
                                             'diagnostics', optional = 1, undeclared=1, typed=0, inline=1),
                                    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraResponseData', optional=1, unique=1, inline=1)
                                    ), 'explainResponse', inline=1, inorder=1
                                   )

class ExplainRequest:
    name = ""
    responseName = "explainResponse"
    responseType = ExplainResponse

    version = "1.1"
    recordPacking ="xml"
    stylesheet= None
    extraRequestData = None

    def __init__(self, n, config=None, opts={}):
        self.name = n
        self.version = "1.1"
        self.config = config
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


ExplainRequest.typecode = \
                        TC.Struct(ExplainRequest,
                                  (TC.String('version'),
                                   TC.String('recordPacking', optional=1),
                                   TC.URI('stylesheet', optional=1),
                                   RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRequestData', optional=1, unique=1, inline=1)
                                   ), 'explainRequest', inline=1
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
    echoedRequest = None

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

    version = "1.1"
    stylesheet = ""
    extraRequestData = []
    query = ""
    startRecord = 1
    maximumRecords = 0
    recordPacking = "xml"
    recordSchema = ""
    recordXPath = ""
    resultSetTTL = 0
    sortKeys = ""

    # Need these for processing and echo
    xSortKeys = []
    xQuery = None
    queryStructure = None

    def __init__(self, name, config=None, opts={}):
        self.name = name
        self.version = "1.1"
        self.recordPacking = "xml"
        self.config = config

        if config:
            self.recordSchema = config.defaultRetrieveSchema
            self.maximumRecords = config.defaultNumberOfRecords

        extra = {}
        for k in opts:
            if (type(opts[k]) != types.ListType):
                opts[k] = [opts[k]]
            if (k in self.integerParams):
                setattr(self, k, int(opts[k][0]))
            elif (len(k) > 2 and k[0:2] == "x-"):
                extra[k[2:]] = opts[k][0]
            else:
                setattr(self, k, opts[k][0])
        if (extra):
            # Process ExtraRequestData
            self.extraRequestData = generateExtraData(extra, config)



SearchRetrieveRequest.typecode =  \
TC.Struct(SearchRetrieveRequest,    
          (TC.String('version'),
           TC.String('query'),  
           TC.Integer('startRecord', optional=1),
           TC.Integer('maximumRecords', optional=1),
           TC.String('recordPacking', optional=1),
           TC.String('recordSchema', optional=1),
           TC.String('recordXPath', optional=1),
           TC.Integer('resultSetTTL', optional=1),
           TC.String('sortKeys', optional=1),
           TC.URI('stylesheet', optional=1),
           RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
            ), 'searchRetrieveRequest', inline=1
          )

SearchRetrieveResponse.typecode = \
                                TC.Struct(SearchRetrieveResponse, (
    TC.String('version', unique=1, inline=1),
    TC.Integer('numberOfRecords', unique=1, inline=1),
    TC.String('resultSetId', optional=1, unique=1),
    TC.Integer('resultSetIdleTime', optional=1),
    RDSArray('srw:recordType',
             Record.typecode,
             'records', optional=1, undeclared=1, inline=1, typed=0, unique=1),
    TC.Integer('nextRecordPosition', optional=1),
    RDSArray('srw:diagnosticType',
             SRWDiagnostic.typecode,
             'diagnostics', optional = 1, undeclared=1, typed=0, inline=1, unique=1),
    TC.Struct(SearchRetrieveRequest,
              (TC.String('version', unique=1, inline=1),
               TC.String('query', unique=1, inline=1),  
               XMLorString('xQuery', optional =1, textprotect=0, unique=1),
               TC.Integer('startRecord', optional=1, unique=1),
               TC.Integer('maximumRecords', optional=1, unique=1),
               TC.String('recordPacking', optional=1, unique=1),
               TC.String('recordSchema', optional=1, unique=1),
               TC.String('recordXPath', optional=1, unique=1),
               TC.Integer('resultSetTTL', optional=1, unique=1),
               TC.String('sortKeys', optional=1, unique=1),
               TC.URI('stylesheet', optional=1, unique=1),
               RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
               ), 'echoedRequest', inline=1, optional=1, unique=1),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraResponseData', optional=1, unique=1, inline=1, undeclared=1)
    ), 'searchRetrieveResponse', inline=1)


class ScanTerm:
    name ='term'
    value = ""
    numberOfRecords = 0
    displayTerm = ""
    extraTermData = None

    def __init__(self, name, **kw):
        self.name = name
        if (kw.has_key('opts')):
            opts = kw['opts']
            self.value = opts['term']
            if (opts.has_key('freq')):
                self.numberOfRecords = int(opts['freq'])
            if (opts.has_key('display')):
                self.displayTerm = opts['display']
            

ScanTerm.typecode = \
                  TC.Struct(ScanTerm,
                            (TC.String('value', optional=1, unique=1),
                             TC.Integer('numberOfRecords', optional=1),
                             TC.String('displayTerm', optional=1, unique=1),
                             RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraTermData', optional=1, unique=1, inline=1, undeclared=1)
                             ),
                            'term', inline=1, inorder=1
                            )


class ScanResponse:
    name = 'scanResponse'

    version = "1.1"
    terms = []
    diagnostics = []
    extraResponseData = None
    
    def __init__(self, name, **kw):
        self.name = name
        self.version = "1.1"

class ScanRequest:
    name = "scanRequest"
    responseName = "scanResponse"
    responseType = ScanResponse
    integerParams = ['numberOfTerms', 'responsePosition']
    
    version = "1.1"
    scanClause = ""
    responsePosition = 0
    numberOfTerms = 0
    stylesheet = ""
    extraRequestData = None

    xScanClause = None

    def __init__(self, name, config=None, opts={}):
        self.name = name
        self.version = "1.1"
        self.config = config
        extra = {}
        for k in opts:
            if (type(opts[k]) != types.ListType):
                opts[k] = [opts[k]]
            if (k in self.integerParams):
                setattr(self, k, int(opts[k][0]))
            elif (k[:2] == "x-"):
                extra[k[2:]] = opts[k]
            else:
                setattr(self, k, opts[k][0])
        if (extra):
            self.extraRequestData = generateExtraData(extra, config)
                

ScanRequest.typecode = \
                     TC.Struct(ScanRequest,
                               (
    TC.String('version', unique=1, inline=1),
    TC.String('scanClause'),
    TC.Integer('responsePosition', optional=1),
    TC.Integer('numberOfTerms', optional=1),
    TC.URI('stylesheet', optional=1),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
    ),
                               'scanRequest', inline=1, inorder=1
                               )

ScanResponse.typecode = \
TC.Struct(ScanResponse,
          (
    TC.String('version', unique=1, inline=1),
    RDSArray('srw:scanTermType', ScanTerm.typecode, 'terms', unique=1),
    RDSArray('srw:diagnosticType', SRWDiagnostic.typecode, 'diagnostics', optional = 1, undeclared=1, typed=0, inline=1),
    TC.Struct(ScanRequest, (TC.String('version', unique=1, inline=1),
                            TC.String('scanClause'),
                            XMLorString('xScanClause', optional =1, textprotect=0, unique=1),
                            TC.Integer('responsePosition', optional=1),
                            TC.Integer('numberOfTerms', optional=1),
                            TC.URI('stylesheet', optional=1),
                            RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraRequestData', optional=1, unique=1, inline=1, undeclared=1)
                            ),
              'echoedRequest', inline=1, inorder=1
              ),
    RDSArray('xsd:XMLFragment', TC.XML(wrapped=0, unique=1, inline=1), 'extraResponseData', optional=1, unique=1, inline=1, undeclared=1)
    ),
          'scanResponse', inline=1, inorder=1
          )

          



# --- Additional non standard requests ---


class ResultSet:
    "Object to represent a resultSet"
    name = ""
    resultSetName = ""
    ttl = 0
    time = 0

    def __init__(self, name, **kw):
        self.name = name
        if kw.has_key('initMap') and kw['initMap']:
            self.resultSetName = kw['initMap']['resultSetName']
            self.ttl = kw['initMap']['ttl']
            self.time = kw['initMap']['time']

class AuthToken:
    "Object to represent an Authentication Token"
    name = ""
    value = ""
    ttl = 0
    time = 0

    def __init__(self, name, **kw):
        self.name = name
        
class AuthTokenResponse:
    "Object to represent an AuthToken Reponse"
    name = ""
    token = None
    def __init__(self, name, **kw):
        self.name = name
        if kw.has_key('token'):
            self.token = kw['token']

AuthTokenResponse.typecode = \
TC.Struct(AuthTokenResponse,
          ( [TC.Struct(AuthToken,
                       ( TC.String('value'),
                         TC.Integer('ttl')
                         ),
                       'token'
                       )]
            ),
          'authenticationTokenReponse'
          )

