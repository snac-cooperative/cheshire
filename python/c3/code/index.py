
from baseObjects import Index, Document
from configParser import C3Object
from utils import elementType, getFirstData, verifyXPaths
from c3errors import ConfigFileException
import re, types, sys
from record import SaxRecord, DomRecord
from resultSet import SimpleResultSet
from PyZ3950 import CQLParser, SRWDiagnostics

class SimpleIndex(Index):
    sources = []
    xPathAllAbsolute = 1
    xPathAttributesRequired = []
    xPathsNormalized = {}
    currentFullPath = []
    currentPath = []
    storeOrig = {}

    def _handleConfigNode(self, node):
        # Source
        if (node.localName == "source"):
            xpaths = []
            process = []
            preprocess = []
            for child in node.childNodes:
                if child.nodeType == elementType:
                    if child.localName == "xpath":
                        # add XPath
                        xpaths.append(getFirstData(child).data)
                    elif child.localName == "preprocess":
                        # Record process list
                        for child2 in child.childNodes:
                            if child2.nodeType == elementType and child2.localName == "object":
                                preprocess.append([child2.getAttribute('type'), child2.getAttribute('ref')])
                    elif child.localName == "process":
                        # Record process list
                        for child2 in child.childNodes:
                            if child2.nodeType == elementType and child2.localName == "object":
                                process.append([child2.getAttribute('type'), child2.getAttribute('ref')])
            cxps = verifyXPaths(xpaths)
            self.sources.append([cxps, process, preprocess])

    def __init__(self, node, parent):
        self.sources = []
        self.xPathAttributesRequired = []
        self.xPathsNormalized = {}
        self.xPathAllAbsolute = 1
        self.storeOrig = {}
        self.origTerms = {}

        C3Object.__init__(self, node, parent)

        # We need a Store object
        iStore = self.get_path(None, 'indexStore')

        if (iStore == None):
            raise(ConfigFileException("Index config does not have an indexStore."))
        elif not iStore.contains_index(None, self):
            iStore.create_index(None, self)

        for s in range(len(self.sources)):
            if self.sources[s][1][0][0] <> 'extracter':
                raise(ConfigFileException("First link in process chain must be an Extracter."))
            for t in range(len(self.sources[s][1])):
                o = self.get_object(None, self.sources[s][1][t][1])
                if (o <> None):
                    self.sources[s][1][t][1] = o
                else:
                    raise(ConfigFileException("Unknown object %s" % (self.sources[s][1][t][1])))
            for t in range(len(self.sources[s][2])):
                o = self.get_object(None, self.sources[s][2][t][1])
                if (o <> None):
                    self.sources[s][2][t][1] = o
                else:
                    raise(ConfigFileException("Unknown object %s" % (self.sources[s][2][t][1])))
    def _mergeHash(self, a, b):
        if not a:
            return b
        if not b:
            return a
        for k in b.keys():
            a[k] = a.get(k, 0) + b[k] 
        return a

    def _mergeData(self, a, b):
        if not a:
            return b
        return a + b

    def _processChain(self, session, data, process):
        (otype, obj) = process[0]
        new = {}
        if (otype == 'extracter'):
            # list input
            for d in data:
                if (type(d) == types.ListType):
                    # SAX event
                    new = self._mergeHash(new, obj.process_eventList(session, d))
                elif (type(d) in types.StringTypes):
                    # Attribute content
                    new = self._mergeHash(new, obj.process_string(session, d))
                else:
                    # DOM nodes
                    new = self._mergeHash(new, obj.process_node(session, d))
        elif (otype == 'normaliser'):
            fn = obj.process_string
            try:
                storeOrig = self.storeOrig[obj.id]
            except:
                storeOrig = obj.get_setting(session, "storeRecordData")
                self.storeOrig[obj.id] = storeOrig
            
            for d in data.keys():
                nk = fn(session, d)
                # Might normalise to an existing term
                new[nk] = self._mergeData(new.get(nk, None), data[d])
                if (storeOrig and nk <> d):
                    self.origTerms[nk] = d
        elif (otype == 'preParser'):
            # Give current keys to the parser to do something with
            # Will return documents...
            fn = obj.process_document
            for d in data.keys():
                if (not isinstance(d[0], Document)):
                    doc = StringDocument(d)
                else:
                    doc = d
                new[fn(session, doc)] = data[d]

        elif (otype == 'parser'):
            # Give current keys (which had better be documents!) to parser
            fn = obj.process_document
            for d in data.keys():
                new[fn(session, d)] = data[d]

        elif (otype == 'index'):
            # Had better be a record!
            # Stomp infinite recursion:
            # Don't assign anything different to new ?
            if obj == self:
                raise(ConfigFileException("Infinitely recursive process chain!"))
            fn = obj.handle_indexRecord
            for d in data.keys():
                fn(session, d)

        elif (otype == 'transformer'):
            # Had better be a record!
            fn = obj.process_record
            for d in data.keys():
                new[fn(session, d)] = data[d]

        elif (type == 'indexStore'):
            # XXX Store terms
            pass
        elif (type == 'recordStore'):
            # Had better be a record!
            fn = obj.store_record
            for d in data.keys():
                if (not isinstance(d, Record)):
                    raise(ValueError)
                fn(session, d)
            new = data
        else:
            raise(ConfigFileException("Unknown object type: %s" % (otype)))

        if (len(process) == 1):
            return new
        else:
            return self._processChain(session, new, process[1:])

    # XXX API badness
    def extract_data(self, session, record):
        (paths, process, preprocess) = self.sources[0]
        xp = paths[0]
        raw = record.process_xpath(xp)
        processed = self._processChain(session, raw, process)
        if (processed):
            return processed.keys()[0]
        else:
            return None

    def handle_indexRecord(self, session, record):
        # First extract simple paths, the majority of cases
        for src in self.sources:
            (paths, process, preprocess) = src
            for xp in paths:
                # Allow preprocessing of record into XPathable form
                if (preprocess):
                    r2hash = self._processChain(session, {record : ''}, preprocess)
                    record2 = r2hash.keys()[0]
                else:
                    record2 = record

                raw = record2.process_xpath(xp)
                self.origTerms = {}
                processed = self._processChain(session, raw, process)
                if (process[-1][0] != 'indexStore'):
                    # Store in our indexStore
                    istore = self.get_path(session, 'indexStore')
                    if (istore <> None):
                        istore.store_terms(session, self, processed, record, self.origTerms)
        # Return final link's results in chain??
        return processed

    def begin_indexing(self, session):
        # Find all indexStores
        stores = []
        for src in self.sources:
            if src[1][-1][0] == 'indexStore':
                stores.append(src[1][-1][1])
        istore = self.get_path(session, 'indexStore')
        if (istore <> None and not istore in stores):
            stores.append(istore)
        for s in stores:
            s.begin_indexing(session, self)


    def commit_indexing(self, session):
        stores = []
        for src in self.sources:
            if src[1][-1][0] == 'indexStore':
                stores.append(src[1][-1][1])
        istore = self.get_path(session, 'indexStore')
        if (istore <> None and not istore in stores):
            stores.append(istore)
        for s in stores:
            s.commit_indexing(session, self)

    def handle_search(self, session, clause, db):
        # Final destination. Process Term.
        pn = self.get_setting(session, 'termProcess')
        if (pn == None):
            pn = 0
        else:
            pn = int(pn)
        process = self.sources[pn][1]
        res = self._processChain(session, [clause.term.value], process)

        store = self.get_path(session, 'indexStore')
        matches = []
        rel = clause.relation

        if (clause.relation.value in ['any', 'all', '=', 'exact']):
            for k in res:
                term = store.fetch_term(session, self, k)
                # May be empty result set
                s = SimpleResultSet(session, term[3:])
                s.index = self
                s.queryTerm = k
                s.queryFreq = res[k]
                if (term):
                    s.termid = term[0]
                    s.totalRecs = term[1]
                    s.totalOccs = term[2]
                else:
                    s.totalRecs = 0
                    s.totalOccs = 0
                matches.append(s)
        elif (clause.relation.value in ['>=', '>', '<', '<=']):
            if (len(res) <> 1):
                d = SRWDiagnostics.Diagnostic24()
                d.details = "%s %s" % (clause.relation.toCQL(), clause.term.value)
                raise d
            else:
                rel = CQLParser.Relation('any')
                termList = store.fetch_termList(session, self, res.keys()[0], 0, clause.relation.value)
                for t in termList:
                    matches.append(SimpleResultSet(session, t[1][3:]))
        elif (clause.relation.value == "within"):
            if (len(res) <> 2):
                d = SRWDiagnostics.Diagnostic24()
                d.details = "%s %s" % (clause.relation.toCQL(), clause.term.value)
                raise d
            else:
                rel = CQLParser.Relation('any')
                termList = store.fetch_termList(session, self, res.keys()[0], end=res.keys()[1])
                for t in termList:
                    matches.append(SimpleResultSet(session, t[1][3:]))

        if (len(matches) == 1):
            return matches[0].combine(session, [], clause, db)
        elif not matches:
            return SimpleResultSet(session)
        else:
            return matches[0].combine(session, matches[1:], clause, db)

    def handle_scan(self, session, clause, numReq, direction=">="):
        # Process term.
        pn = self.get_setting(session, 'termProcess')
        if (pn == None):
            pn = 0
        else:
            pn = int(pn)
        process = self.sources[pn][1]
        res = self._processChain(session, [clause.term.value], process)
        if (len(res) <> 1):
            d = SRWDiagnostics.Diagnostic24()
            d.details = "%s %s" % (clause.relation.toCQL(), clause.term.value)
            raise d
        store = self.get_path(session, 'indexStore')
        tList = store.fetch_termList(session, self, res.keys()[0], numReq=numReq, relation=direction)
        return tList


class ProximityIndex(SimpleIndex):
    """ Need to use prox extracter and override _mergeHash """
    origTerms = {}
    
    def _mergeHash(self, a, b):
        if not a:
            return b
        if not b:
            return a
        for k in b.keys():
            temp = a.get(k, [0])
            tempb = b[k]
            temp[0] += tempb[0]
            temp.extend(tempb[1:])
            a[k] = temp
        return a

    def _mergeData(self, a, b):
        if not a:
            return b
        a.extend(b)
        return a

    def handle_search(self, session, clause, db):
        pn = self.get_setting(session, 'termProcess')
        if (pn == None):
            pn = 0
        else:
            pn = int(pn)
        process = self.sources[pn][1]
        res = self._processChain(session, [clause.term.value], process)
        store = self.get_path(session, 'indexStore')
        matches = []
        rel = clause.relation
        if (clause.relation.value in ['any', 'all', '=', 'exact']):
            for k in res:
                val = store.fetch_term(session, self, k)
                if (val):                    
                    (term, prox) = val
                    s = SimpleResultSet(session, term[3:])
                    s.termid = term[0]
                    s.totalRecs = term[1]
                    s.totalOccs = term[2]
                    s.proxInfo = prox
                else:
                    # No match
                    term = None
                    s = SimpleResultSet(session, [])
                    s.totalRecs = 0
                    s.totalOccs = 0
                matches.append(s)
        elif (clause.relation.value in ['>=', '>', '<', '<=']):
            if (len(res) <> 1):
                d = SRWDiagnostics.Diagnostic24()
                d.details = "%s %s" % (clause.relation.toCQL(), clause.term.value)
                raise d
            else:
                rel = CQLParser.Relation('any')
                termList = store.fetch_termList(session, self, res.keys()[0], 0, clause.relation.value)
                for t in termList:
                    matches.append(SimpleResultSet(session, t[1][3:]))
        elif (clause.relation.value == "within"):
            if (len(res) <> 2):
                d = SRWDiagnostics.Diagnostic24()
                d.details = "%s %s" % (clause.relation.toCQL(), clause.term.value)
                raise d
            else:
                rel = CQLParser.Relation('any')
                termList = store.fetch_termList(session, self, res.keys()[0], end=res.keys()[1])
                for t in termList:
                    matches.append(SimpleResultSet(session, t[1][3:]))

        if (len(matches) == 1):
            return matches[0]
        elif (not matches):
            return SimpleResultSet(session)
        else:
            return matches[0].combine(session, matches[1:], clause)


        
