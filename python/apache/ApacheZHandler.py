
# Use with mod_python (requires post 3.0.3 for improved conn.read())
# Plus CQL and PyZ3950

# In Apache config, for example:
# Listen 127.0.0.1:2100
# <VirtualHost 127.0.0.1:2100>
#      PythonPath "sys.path+['/path/to/code/']"
#      PythonConnectionHandler filenameOfCode
#      PythonDebug On  
# </VirtualHost>

from PyZ3950.asn1 import Ctx, IncrementalDecodeCtx, GeneralString 
from PyZ3950 import asn1
from PyZ3950.z3950_2001 import *
from z3950_utils import *
from PyZ3950.zdefs import *
from PyZ3950.oids import *

from mod_python import apache
import traceback, codecs, sys
import CQLParser as cql

LOGFILE = "/home/cheshire/log.txt"

def zAttrsToCQL(attrCombinations):
    # XXX Mmmmm. What to do exactly?
    # If we have ZeeRex configs to check, look there.
    # Otherwise we should just do some generic transformations?

    return (str(attrCombinations), "=")


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

        queryType = data.query[0]
        query = None
        queryString = ""
        if (queryType in ['type_1', 'type_101']):
            zQuery = data.query[1]
            attrset = zQuery.attributeSet
            rpn = zQuery.rpn

            # Lets try and turn RPN into CQL.
            if rpn[0] == 'op':
                # single search clause
                op = rpn[1]
                query = self.build_searchClause(op, data)
            elif rpn[0] == 'rpnRpnOp':
                triple = rpn[1]
                query = self.build_triple(triple, data)
        elif (queryType == 'type_0'):
            # A Priori external
            queryString = data.query[1]
        elif (queryType == 'type_2'):
            # ISO8777  (CCL)
            queryString = data.query[1]
        elif (queryType == 'type_104'):
            # Look for CQL or SQL
            type104 = data.query[1].direct_reference
            queryString = data.query[1].encoding[1]

            if (type104 == Z3950_QUERY_CQL_ov):
                # Native CQL query
                query = cql.parse(queryString)
            elif (type104 == Z3950_QUERY_SQL_ov):
                # Hopefully just pass off to Postgres
                pass
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
        resp.resultCount = 1
        resp.numberOfRecordsReturned = 0
        resp.nextResultSetPosition = 1
        resp.searchStatus = 1
        resp.resultSetStatus = 1
        resp.presentStatus = PresentStatus.get_num_from_name('failure')
        # resp.records = ('responseRecords', [])
        return self.encode(('searchResponse', resp))

    def handleScan(self, data):

        resp = ScanResponse()
        resp.stepSize = 1
        resp.scanStatus = 1
        resp.numberOfEntriesReturned = 0
        resp.positionOfTerm = 0
        resp.entries = ('entries', [])
        return self.encode(('scanResponse', resp))

    def handlePresent(self, data):
        resp = PresentResponse()
        resp.numberOfRecordsReturned = 1
        resp.nextResultSetPosition = 2
        resp.presentStatus = 1
        resp.records = ('responseRecords', [])
        return self.encode(('presentResponse', resp))

    def handleClose(self, data):
        resp = Close()
        resp.closeReason = 0
        resp.diagnosticInformation = "Normal Close"
        return self.encode(('close', resp))

    def handleSort(self, data):
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

    def build_searchClause(self, op, data):
        type = op[0]
        
        if type == 'attrTerm':
            attrs = op[1].attributes
            term = op[1].term
            combs = []
            for acomb in attrs:
                if (hasattr(acomb, 'attributeSet')):
                    aset = acomb.attributeSet
                elif(hasattr(data, 'attributeSet')):
                    aset = data.attributeSet
                else:
                    # Oh Man... just assume BIB1
                    aset = Z3950_ATTRS_BIB1_ov

                if (hasattr(acomb, 'attributeType')):
                    atype = acomb.attributeType
                else:
                    # URGH!?
                    atype = 1

                astruct = acomb.attributeValue
                if astruct[0] == 'numeric':
                    avalue = astruct[1]
                else:
                    # complex
                    astruct = astruct[1]
                    if (hasattr(astruct, 'list')):
                        avalue = astruct.list[0][1]
                    else:
                        #semanticAction
                        # Uhh... sequence of int ??
                        avalue = astruct.semanticAction[0][1]
                combs.append([aset, atype, avalue])

            # Need to do real mapping
            sc = cql.SearchClause()
            (index, relation) = zAttrsToCQL(combs)
            sc.index = index
            sc.relation = cql.Relation(relation)
            # XXX term is tuple ('general', 'term')
            # What other than general can we be?
            sc.term = term[1]
            return sc

        else:
            self.log("Not attrTerm: " + repr(op))
            return None


    def build_triple(self, triple, data):
        bool = triple.op
        lhs = triple.rpn1
        rhs = triple.rpn2

        triple = cql.Triple()

        if (lhs[0] == 'op'):
            lhs = self.build_searchClause(lhs[1], data)
        else:
            lhs = self.build_triple(lhs[1], data)
        triple.leftOperand = lhs

        if (rhs[0] == 'op'):
            rhs = self.build_searchClause(rhs[1], data)
        else:
            rhs = self.build_triple(rhs[1], data)
        triple.rightOperand = rhs

        triple.boolean = cql.Boolean(bool[0])
        if bool[0] == 'prox':
            distance = bool[1].distance
            order = bool[1].ordered
            if order:
                order = "ordered"
            else:
                order = "unordered"

            relation = bool[1].relationType
            rels = ["", "<", "<=", "=", ">=", ">", "<>"]
            relation = rels[relation]

            unit = bool[1].proximityUnitCode
            units = ["", "character", "word", "sentence", "paragraph", "section", "chapter", "document", "element", "subelement", "elementType", "byte"]
            if unit[0] == "known":
                unit = units[unit[1]]

            mods = [relation, str(distance), unit, order]
            triple.boolean.modifiers = mods
            
        return triple
                              

def connectionhandler(conn):
    # Apache level stuff
    if (conn.local_addr[1] != 2100):
        return apache.DECLINED
    try:
        logfile = file(LOGFILE, "w")
        handler = ZHandler(conn, logfile)
        handler.read()

    except Exception, err:
        logfile.write("Major Failure:\n")
        traceback.print_exc(100, logfile)
        logfile.flush()

    return apache.OK
