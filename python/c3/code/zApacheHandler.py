
# Use with mod_python (requires post 3.0.3 for improved conn.read())
# Plus CQL and PyZ3950

# In Apache config, for example:
# Listen 127.0.0.1:2100
# <VirtualHost 127.0.0.1:2100>
#      PythonPath "sys.path+['/path/to/code/']"
#      PythonConnectionHandler filenameOfCode
#      PythonDebug On  
# </VirtualHost>


# XXX To do:
# * Check database
# * Respect recordSyntax
# * Respect elementsetname

from PyZ3950.asn1 import Ctx, IncrementalDecodeCtx, GeneralString 
from PyZ3950 import asn1, CQLUtils
from PyZ3950.z3950_2001 import *
from z3950_utils import *
from PyZ3950.zdefs import *
from PyZ3950.oids import *

from mod_python import apache
import traceback, codecs, sys, os
import PyZ3950.CQLParser as cql

asn1.register_oid(Z3950_QUERY_SQL, SQLQuery)
asn1.register_oid(Z3950_QUERY_CQL, asn1.GeneralString)

from server import SimpleServer
os.chdir("/home/cheshire/c3/code")
serv = SimpleServer('/home/cheshire/c3/code/configs/serverConfig.xml')
l5r = serv.get_object(None, 'db_l5r')
recStore = l5r.get_object(None, 'l5rRecordStore')
rss = serv.get_object(None, 'defaultResultSetStore')
rsidmap = {}

LOGFILE = "/home/cheshire/c3/code/logs/log.txt"

class ZHandler:

    connection = None
    logfile = None
    handlers = {}
    debug = 1
    decode_ctx = None
    encode_ctx = None

    def __init__(self, conn, logf):
        self.connection = conn
        self.logfile = logf
        self.decode_ctx = asn1.IncrementalDecodeCtx(APDU)
        self.encode_ctx = asn1.Ctx()
        self.handlers = {"initRequest" : self.handleInit,
                         "searchRequest" : self.handleSearch,
                         "scanRequest" : self.handleScan,
                         "close" : self.handleClose,
                         "presentRequest" : self.handlePresent,
                         "sortRequest" : self.handleSort,
                         "deleteResultSetRequest" : self.handleDeleteResultSet,
                         "extendedServicesRequest" : self.handleExtendedServices
                         }

    def log(self, text):
        self.logfile.write(text + "\n")
        self.logfile.flush()
        
    def read(self):
        c = self.connection.read()
        while (c):
            try:
                self.decode_ctx.feed(map(ord, c))
                while self.decode_ctx.val_count() > 0:
                    # We have a PDU
                    (type, data) = self.decode_ctx.get_first_decoded()
                    
                    # Successfully decoded.
                    self.log("Received " + type);
                    
                    if (self.handlers.has_key(type)):
                        resp = self.handlers[type](data)
                        self.connection.write(resp.tostring())
                        self.log("Sent response");
                    else:
                        # Uhoh, unknown request
                        self.log("Ohoh, don't know how to handle " + type)

            except:
                if (self.debug):
                    self.logfile.write("\n")
                    traceback.print_exc(100, self.logfile)
                    self.logfile.flush()
            c = self.connection.read()

    def encode(self, resp):
        r = self.encode_ctx.encode(APDU, resp)
        return r
    def set_codec(self, name):
        self.encode_ctx.set_codec(asn1.GeneralString, codecs.lookup(name))
        self.decode_ctx.set_codec(asn1.GeneralString, codecs.lookup(name))

    def handleInit(self, req):
        resp = InitializeResponse()
        rsidmap = {}
        (name, resp) = negotiateCharSet(req, resp)

        resp.protocolVersion = ProtocolVersion()
        resp.protocolVersion['version_1'] = 1
        resp.protocolVersion['version_2'] = 1
        resp.protocolVersion['version_3'] = 1
        
        resp.options = Options()
        for o in ['search', 'present', 'delSet', 'scan', 'negotiation', 'sort']:
            resp.options[o] = 1

        resp.preferredMessageSize = 0x10000
        resp.exceptionalRecordSize = 0x10000
        resp.implementationId = 'Cheshire/PyZ39.50'
        resp.implementationName = 'Cheshire/PyZ39.50 Server'
        resp.implementationVersion = '0.1'
    
        resp.result = 1

        pdu = self.encode(('initResponse', resp))
        if (name <> None):
            self.set_codec(name)
        return pdu

    def handleSearch(self, data):

        self.log(repr(data))

        queryType = data.query[0]
        query = None
        queryString = ""
        if (queryType in ['type_1', 'type_101']):
            zQuery = data.query[1]
            attrset = zQuery.attributeSet
            rpn = zQuery.rpn
            query = CQLUtils.rpn2cql(rpn)
        elif (queryType == 'type_0'):
            # A Priori external
            queryString = data.query[1]
        elif (queryType == 'type_2'):
            # ISO8777  (CCL)
            queryString = data.query[1]
        elif (queryType == 'type_104'):
            # Look for CQL or SQL
            type104 = data.query[1].direct_reference

            if (type104 == Z3950_QUERY_CQL_ov):
                queryString = data.query[1].encoding[1]
                # Native CQL query
                # Looks like:  ((0,27) 'query', 0)
                query = cql.parse(queryString)
            elif (type104 == Z3950_QUERY_SQL_ov):
                # Hopefully just pass off to Postgres
                # Grab directly out of structure??
                sqlQ = data.query[1].encoding[1]
                queryString = sqlQ.queryExpression
            else:
                # Undefined query type
                raise NotImplementedError

        elif (queryType in ['type_102', 'type_100']):
            # 102: Ranked List, not yet /defined/ let alone implemented
            # 100: Z39.58 query (Standard was withdrawn)
            raise NotImplementedError

        if query:
            self.log(query.toXCQL())
        elif queryString:
            self.log("type:  " + queryType)
            self.log("query: " + queryString)

        rsetname = data.resultSetName
        dbs = data.databaseNames

        if len(dbs) == 1 and dbs[0].lower() == "ir-explain-1":
            self.search_explain(query)

        resp = SearchResponse()
        resp.resultCount = 0

        if len(dbs) == 1 and dbs[0].lower() == 'l5r':
            rs = l5r.handle_search(None, query)
            if (rs):
                resp.resultCount = len(rs)
                # Put it into our DB
                rsid = rss.create_resultSet(None, rs)
                rsidmap[data.resultSetName] = rsid
            
        resp.numberOfRecordsReturned = 0
        resp.nextResultSetPosition = 1
        resp.searchStatus = 1
        resp.resultSetStatus = 1
        resp.presentStatus = PresentStatus.get_num_from_name('failure')
        return self.encode(('searchResponse', resp))

    def handleScan(self, data):
        self.log(repr(data))
        db = data.databaseNames
        nt = data.numberOfTermsRequested
        rp = data.preferredPositionInResponse
        where = data.termListAndStartPoint

        resp = ScanResponse()
        resp.stepSize = 1
        resp.scanStatus = 1
        resp.numberOfEntriesReturned = 0
        resp.positionOfTerm = 0
        resp.entries = ('entries', [])
        return self.encode(('scanResponse', resp))

    def handlePresent(self, data):
        self.log(repr(data))

        rs = rsidmap[data.resultSetId]
        resultset = rss.fetch_resultSet(None, rs)

        f = data.resultSetStartPoint
        n = data.numberOfRecordsRequested
        recSyntax = data.preferredRecordSyntax
        recSchema = data.recordComposition

        records = []
        for x in range(f, f+n):
            did = resultset[x-1].docid
            data = recStore.fetch_record(None, did)
            xml = data.get_xml()
            r = NamePlusRecord()
            r.name = 'l5r'
            xr = asn1.EXTERNAL()
            xr.direct_reference= Z3950_RECSYN_MIME_XML_ov
            xr.encoding = ('octet-aligned', xml)
            r.record = ('retrievalRecord', xr)
            records.append(r)
            
        resp = PresentResponse()
        resp.numberOfRecordsReturned = len(records)
        resp.numberOfRecordsReturned = 0
        resp.nextResultSetPosition = n + len(records)
        resp.presentStatus = 1
        
        resp.records = ('responseRecords', records)
        return self.encode(('presentResponse', resp))

    def handleClose(self, data):
        resp = Close()
        resp.closeReason = 0
        resp.diagnosticInformation = "Normal Close"
        return self.encode(('close', resp))

    def handleSort(self, data):
        self.log(repr(data))
        resp = SortResponse()
        resp.sortStatus = 1
        resp.resultSetStatus = 1
        resp.resultCount = 1
        return self.encode(('sortResponse', resp))

    def handleDeleteResultSet(self, data):
        resp = DeleteResultSetResponse()
        resp.deleteOperationStatus = 0
        resp.numberNotDeleted = 0
        resp.deleteMessage = "No Resultset"
        return self.encode(('deleteResultSetResponse', resp))

    def handleExtendedServices(self, data):
        # Can't generate Z's packages anywhere?!
        # Just ILL and YAZ's adm ones. Urgh.
        self.log(str(data))

        # Spit back a Dunno What You Mean response
        resp = ExtendedServicesResponse()
        resp.operationStatus = 3
        return self.encode(('extendedServicesResponse', resp))

    def search_explain(self, query):
        # Just messing around
        ti = TargetInfo()
        ti.name = "Fish"
        self.log(str(ti))




def connectionhandler(conn):
    # Apache level stuff
    if (conn.local_addr[1] != 2100):
        return apache.DECLINED
    try:
        logfile = file(LOGFILE, "w")
        handler = ZHandler(conn, logfile)
        handler.read()
        # Shut down databases
        rss.close(None)
        recStore.close(None)

    except Exception, err:
        logfile.write("Major Failure:\n")
        traceback.print_exc(100, logfile)
        logfile.flush()

    return apache.OK
