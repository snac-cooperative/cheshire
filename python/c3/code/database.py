
from configParser import C3Object
from baseObjects import Database, Index, ProtocolMap, Record
from c3errors import ConfigFileException
from bootstrap import BSParser, BootstrapDocument
import PyZ3950.CQLParser as cql,  PyZ3950.SRWDiagnostics as SRWDiagnostics
import os, sys


try:
    # name when installed by hand
    import bsddb3 as bdb
except:
    # name that comes in python 2.3
    import bsddb as bdb

class SimpleDatabase(Database):

    def __init__(self, node, parent):

        self.indexes = {}
        self.protocolMaps = {}
        self.records = {}

        self.totalRecords = 0
        self.totalRecordSize = 0
        self.minRecordSize = 10000000
        self.maxRecordSize = 0
        self.meanRecordSize = 0

        C3Object.__init__(self, node, parent)

        if self.get_path(None, 'recordStore') == None:
            raise (ConfigFileException('No recordStore set in database configFile'))
        mp = self.get_path(None, 'metadataPath')
        if (not mp):
            raise(ConfigFileException('No metadata db path set in database configFile'))
        if (not os.path.isabs(mp)):
            dfp = self.get_path(None, 'defaultPath')
            mp = os.path.join(dfp, mp)
        if (not os.path.exists(mp)):
            # We don't exist, try and instantiate new database
            self._initialise(mp)
        else:
            cxn = bdb.db.DB()
            try:
                cxn.open(mp)
                # Now load values.
                recs = cxn.get("records")
                totalRecs = cxn.get("totalRecords")
                totalSize = cxn.get("totalRecordSize")
                minSize = cxn.get("minRecordSize")
                maxSize = cxn.get("maxRecordSize")
                self.minRecordSize = long(minSize)
                self.maxRecordSize = long(maxSize)
                self.totalRecords = long(totalRecs)
                self.totalRecordSize = long(totalSize)
                self.records = eval(recs)
                cxn.close()
            except:
                # Doesn't exist in usable form
                self._initialise(mp)

        for o in self.objects.keys():
            if isinstance(self.objects[o], Index):
                self.indexes[o] = self.objects[o]
            elif isinstance(self.objects[o], ProtocolMap):
                self.protocolMaps[self.objects[o].protocol] = self.objects[o]

    def _initialise(self, mp):
        cxn = bdb.db.DB()
        cxn.open(mp, dbtype=bdb.db.DB_BTREE, flags = bdb.db.DB_CREATE, mode=0660)
        cxn.close()

    def commit_metadata(self, session):
        cxn = bdb.db.DB()
        mp = self.get_path(None, 'metadataPath')
        if (not os.path.isabs(mp)):
            dfp = self.get_path(None, 'defaultPath')
            mp = os.path.join(dfp, mp)
        try:
            cxn.open(mp)
            recs = cxn.get("records")
            cxn.put("totalRecords", str(self.totalRecords))
            cxn.put("totalRecordSize", str(self.totalRecordSize))
            cxn.put("minRecordSize", str(self.minRecordSize))
            cxn.put("maxRecordSize", str(self.maxRecordSize))
            cxn.put("records", repr(self.records))
            cxn.close()
        except:
            raise ValueError

    def add_record(self, session, rec):
        (store, id) = (rec.recordStore, rec.id)
        full = self.records.get(store.id, [[]])
        k = full[-1]
        if (len(k) > 1 and k[1] == id -1):
            k[1] = id
        elif ((len(k) == 1 and k[0] == id -1) or not k):
            k.append(id)
        else:
            full.append([id])
        self.records[store.id] = full
        # Increment
        self.totalRecords += 1
        # And record size
        self.totalRecordSize += rec.size
        if (rec.size > self.maxRecordSize):
            self.maxRecordSize = rec.size
        if (rec.size < self.minRecordSize):
            self.minRecordSize = rec.size

    def handle_indexRecord(self, session, record):
        txr = self.get_path(session, 'onIndexRecord')
        if (txr):
            txr.process_record(session, record)
        for idx in self.indexes.values():
            idx.handle_indexRecord(session, record)

    def begin_indexing(self, session):
        txr = self.get_path(session, 'onIndexRecord')
        if (txr):
            txr.begin_indexing(session)
        for idx in self.indexes.values():
            idx.begin_indexing(session)

    def commit_indexing(self, session):
        txr = self.get_path(session, 'onIndexRecord')
        if (txr):
            txr.commit_indexing(session)
        for idx in self.indexes.values():
            idx.commit_indexing(session)

    def handle_search(self, session, query):
        if (isinstance(query, cql.SearchClause)):
            # Check resultset
            rsid = query.getResultSetId()
            if (rsid):
                # Get existing result set
                rss = self.get_object(session, "defaultResultSetStore")
                return rss.fetch_resultSet(session, rsid)
            else:
                # Find index:
                idx = self.protocolMaps['CQL'].resolveIndex(session, query)
                if (idx <> None):
                    return idx.handle_search(session, query, self)
                else:
                    d = SRWDiagnostics.Diagnostic16()
                    d.details = query.index.toCQL()
                    raise d
        else:
            left = self.handle_search(session, query.leftOperand)
            right = self.handle_search(session, query.rightOperand)
            return left.combine(session, [right], query)

    def handle_retrieve(self, session, rset, start, numRecs):
        end = min(start+numRecs, len(rset))
        recs = []
        for x in range(start, end):
            if (not isinstance(rset[x], Record)):
                hit = rset[x]
                st = self.get_object(session, hit.recordStore)
                rec = st.fetch_record(session, hit.docid)                
                rset[x] = rec
            else:
                rec = rset[x]
            recs.append(rec)
        return recs

    def handle_scan(self, session, query, numReq, direction=">="):
        if (not isinstance(query, cql.SearchClause)):
            d = SRWDiagnostics.Diagnostic38()
            d.details = "Cannot use boolean in scan"
            raise d
        idx = self.protocolMaps['CQL'].resolveIndex(query)
        if (idx <> None):
            # Need to pass to index so as to extract/normalise term
            return idx.handle_scan(session, query, numReq, direction)
        else:
            d = SRWDiagnostics.Diagnostic16()
            d.details = query.index.toCQL()
            raise d
        
    def handle_sort(self, session, sets, keys):
        # Needed for Z sorts by index
        pass
