from baseObjects import ResultSet, ResultSetItem, Index
from PyZ3950 import CQLParser
import sets, math

class SimpleResultSet(ResultSet):
    _set = None
    _list = []
    termid = -1
    totalOccs = 0
    totalRecs = 0
    proxInfo = []
    id = ""
    expires = ""
    index = None
    queryTerm = ""
    queryFreq = 0
    queryFragment = None
    relevancy = 0

    def __init__(self, session, l=[], n=""):
        self._list = l
        self._set = sets.Set(l)
        self.id = n

    def __getitem__(self, k):
        return self._list[k]

    def __setitem__(self, k, val):
        if (val in self._set):
            # ResultSets have to have unique keys
            raise KeyError
        # May raise KeyError:
        self._set.remove(self._list[k])
        self._set.add(val)
        self._list[k] = val

    def __len__(self):
        return len(self._list)

    def append(self, item):
        l = len(self._set)
        self._set = self._set.union(sets.Set([item]))
        # Check we don't already exist
        if (l <> len(self._set)):
            self._list.append(item)

    def extend(self, itemList):
        for i in itemList:
            self.append(i)
        
    def combine(self, session, others, clause, db=None):
        s = self._set
        # XXX  All the other combinations
        # Set query on object and use that if cql not specified
        if (isinstance(clause, CQLParser.Triple)):
            cql = clause.boolean
        else:
            cql = clause.relation

        # To Configuration?
        relSetUri = "http://srw.cheshire3.org/contextSets/rel/1.0"
        cqlSet = "info:srw/cql-context-set/1/cql-v1.1"

        relevancy = 0
        algorithm = "lr"
        combine = "norm"
        for m in cql.modifiers:
            m.type.parent = clause
            m.type.resolvePrefix()
            if (m.type.prefixURI == relSetUri):
                # Relevancy info
                relevancy = 1
                if (m.type.value == "algorithm"):
                    algorithm = m.value
                elif (m.type.value == "combine"):
                    combine = m.value
            elif (m.type.prefixURI == cqlSet and m.value == "relevant"):
                # Generic 'relevancy please' request
                relevancy = 1
        rslist = [self]
        rslist.extend(others)
        if (not relevancy):
            for x in rslist:
                if (x.relevancy):
                    relevancy = 1
                    break

        if (relevancy):
            # D[ocument] here == Record
            if (db):
                totalDocuments = db.totalRecords
            else:
                # Uhoh. Can't do it.  (XXX Better Error)
                raise(ValueError)

            if (isinstance(cql, CQLParser.Relation)):
                if (algorithm == "lr"):
                    x2 = math.sqrt(len(rslist))
                    totalMatchedQueryTerms = 0
                    x1 = 0.00
                    totalSumIDF = 0.0
                    totalSumLogRecFreq = 0.0
                    idx = db.protocolMaps['CQL'].resolveIndex(session, query)
                    constants = [-3.7, 1.269, -0.31, 0.679, -1.012, 0.223, 4.01]
                    if (idx):
                        # Check for constants
                        for x in range(7):
                            temp = idx.get_setting(session, 'lr_constant%d' % x)
                            if (temp):
                                constants[x] = float(temp)

                    # XXX Handle Adjacency and All by ANDing against query
                    # This will just strip out non useful ones

                    dfhash = {}
                    docMatchHash = {}
                    for s in rslist:
                        x1 += math.log(float(s.queryFreq))
                        totalSumIDF += math.log(totalDocuments / float(len(s)))
                        for i in s:
                            v = dfhash.get(i,0)
                            o = float(i.occurences)
                            dfhash[i] == math.log(o) + v
                            v2 = docMatchHash.get(i, 0)
                            docMatchHash[i] = v2 + 1
                            
                    recStores = {}
                    tmplist = []
                    for k in dfhash.keys():
                        logdf = dfhash[k]
                        mqt = float(docMatchHash[k])
                        docLen = recStore.get_recordSize(session, docid)
                        x3 = logdf / float(mqt)
                        x4 = sqrt(docLen)
                        x5 = totalSumIDF / mqt
                        x6 = math.log(mqt)
                        logodds = constants[0] + (constants[1] * x1) + (constants[2] * x2) + (constants[3] * x3) + (constants[4] * x4) + (constants[5] * x5) + (constants[6] * x6)
                        k.weight = 0.75 * (math.exp(logodds) / (1 + math.exp(logodds)))
                        tmplist.append((k.weight, k))
                    tmplist.sort()
                    self.relevancy = 1
                    self._list = [x for (key,x) in tmplist]
                    self._set = sets.set(self._list)

                else:
                    raise NotImplementedError
            else:
                # boolean Merge
                
                if (combine == "sum"):
                    if (cql.value == "or"):
                        pass
                    elif (cql.value == "and"):
                        pass

                elif (combine == "mean"):
                    pass
                elif (combine == "norm"):
                    # Normalised Mean
                    pass
                elif (combine == "cmbz"):
                    #Normalize Mean, then double
                    pass
                elif (combine == "nsum"):
                    # Normalised Sum
                    pass
                else:
                    raise NotImplementedError

            # Don't fall through to default list/set setting
            return self

        if (cql.value in ['all', 'and']):
            for o in others:
                s = s.intersection(o._set)
        elif (cql.value in ['any', 'or']):
            for o in others:
                s = s.union(o._set)
        elif (cql.value in ['=', 'prox']):
            # Need self.proxInfo
            if (not self.proxInfo):
                # XXX Better error
                raise(NotImplementedError)
            else:
                # Check location of each term
                # Combine from left to right, so update proxInfo to o
                # proxInfo is hash of (docid, recStore) to list of locations in record
                leftProx = self.proxInfo
                for o in others:
                    rightProx = o.proxInfo
                    newProx = {}
                    for doc in leftProx:
                        matchLocs = []
                        if (rightProx.get(doc, None)):
                            leftLocs = leftProx[doc]
                            rightLocs = rightProx[doc]
                            for ll in leftLocs:
                                for rl in rightLocs:
                                    if ll[0] == rl[0] and ll[1]+1 == rl[1]:
                                        matchLocs.append(rl)
                        if (matchLocs):
                            newProx[doc] = matchLocs
                    leftProx = newProx
                self.proxInfo = leftProx
                # Now update ResultSetItems
                new = sets.Set()
                for o in leftProx.keys():
                    new.add(SimpleResultSetItem(session, o[0], o[1]))
                self._set = new
                s = new
                            
        elif (cql.value == 'not'):
            for o in others:
                s = s.difference(o)
        else:
            raise(NotImplementedError)

        self._list = list(s)
        self._set = s
        return self
            
    def order(self, session, spec):
        # sort according to some spec
        # spec can be index, "docid", xpath, XXX other?
        # XXX Need secondary sort specs

        l = self._list

        if (isinstance(spec, Index) and spec.get_setting(session, 'sortStore')):
            # check pre-processed db
            istore = spec.get_path(session, 'indexStore')
            tmplist = [(istore.get_sortValue(session, spec, x), x) for x in l]
            tmplist.sort()
            self._list = [x for (key,x) in tmplist]
        elif isinstance(spec, Index):
            # Extract data as per indexing, MUCH slower
            recs = []
            storeHash = {}
            for r in l:
                store = r.recordStore
                o = storeHash.get(store, spec.get_object(session, store))
                storeHash[store] = o
                recs.append(o.fetch_record(session, r.docid))
            tmplist = [(spec.extract_data(session, recs[x]), l[x]) for x in range(len(l))]
            tmplist.sort()
            self._list = [x for (key,x) in tmplist]
        elif spec == "docid":
            tmplist = [(x.docid, x) for x in l]
            tmplist.sort()
            self._list = [x for (key,x) in tmplist]
        elif isinstance(spec, str):
            # XXX correct check?
            # XPath
            pass
        else:
            raise NotImplementedError


class SimpleResultSetItem(ResultSetItem):
    docid = 0
    recordStore = ""
    occurences = 0
    weight = -1.0
    diagnostic = None

    def __init__(self, session, docid=0, recStore="", occs=0, diagnostic=None, weight=-1.0):
        self.docid = docid
        self.recordStore = recStore
        self.occurences = occs
        self.weight = weight


    def __eq__(self, other):
        return self.docid == other.docid and self.recordStore == other.recordStore

    def __str__(self):
        return "%s/%d" % (self.recordStore, self.docid)
    def __repr__(self):
        return "%s/%d" % (self.recordStore, self.docid)

    def __cmp__(self, other):
        # default sort by docid
        return cmp(self.docid, other.docid)

    def __hash__(self):
        # Hash of recordstore + id
        return hash(str(self))

    
