
from baseObjects import IndexStore
from configParser import C3Object
from c3errors import ConfigFileException, FileDoesNotExistException
from resultSet import SimpleResultSetItem
from index import ProximityIndex
import os, types, struct, sys
try:
    # Python 2.3 vs 2.2
    import bsddb as bdb
except:
    import bsddb3 as bdb

class BDBIndexStore(IndexStore):

    indexing = 0
    outFiles = {}
    outOrigFiles = {}
    storeHash = {}
    storeHashReverse = {}
    sortStoreHash = {}
    sortStoreCxn = {}
    recordHash = {}

    def __init__(self, parent, config):
        C3Object.__init__(self, parent, config)
        self.outFiles = {}
        self.outOrigFiles = {}
        self.storeHash = {}
        self.recordHash = {}
        self.sortStoreHash = {}
        self.sortStoreCxn = {}
        rsh = self.get_path(None, 'recordStoreHash')
        if rsh:
            wds = rsh.split()
            for w in range(len(wds)):
                self.storeHash[long(w)] = wds[w]
                self.storeHashReverse[wds[w]] = long(w)

    def _fileFilter(self, x):
        if x[-6:] <> ".index":
            return 0
        elif x[:len(self.id)] <> self.id:
            return 0
        else:
            return 1

    def _generateFilename(self, index):
        stuff = [self.id, "--", index.id, ".index"]
        return ''.join(stuff)

    def begin_indexing(self, session, index):
        temp = self.get_path(session, 'tempPath')
        if not os.path.isabs(temp):
            temp = os.path.join(self.get_path(session, 'defaultPath'), temp)
        if (not os.path.exists(temp)):
            try:
                os.mkdir(temp)
            except:
                raise(ConfigFileException('TempPath does not exist and is not creatable.'))
        elif (not os.path.isdir(temp)):
            raise(ConfigFileException('TempPath is not a directory.'))
        name = os.path.join(temp, self._generateFilename(index) + "_TEMP")
        self.outFiles[index] = file(name, 'w')
        self.outOrigFiles[index] = file(name + "2", 'w')


    def commit_indexing(self, session, index):
        # Need to do this per index so one store can be doing multiple things at once

        if (not self.outFiles.has_key(index)):
            raise FileDoesNotExistException(index.id)

        sort = self.get_path(session, 'sortPath')
        temp = self.get_path(session, 'tempPath')
        dfp = self.get_path(session, 'defaultPath')
        if not os.path.isabs(temp):
            temp = os.path.join(dfp, temp)

        fh = self.outFiles[index]
        fh.flush()
        fh.close()
        del self.outFiles[index]
        oh = self.outOrigFiles[index]
        oh.flush()
        oh.close()
        del self.outOrigFiles[index]
        

        fn = self._generateFilename(index) + "_TEMP"
        tempfile = os.path.join(temp, fn)
        sorted = tempfile + "_SORT"
        os.spawnl(os.P_WAIT, sort, sort, tempfile, '-o', sorted)
        dbname = os.path.join(dfp, fn[:-5])
        cxn = bdb.db.DB()
        cxn.open(dbname)
        nonEmpty = cxn.stat()['nkeys']

        # Original terms from data
        origname = os.path.join(dfp, fn[:-5] + "_ORIG")
        if (os.path.exists(origname)):
            tempfile2 = os.path.join(temp, fn + "2")
            inh2 = file(tempfile2, 'r')
            origTerms = {}
            for il in inh2.xreadlines():
                (term, data) = il.split('\x00')
                origTerms[term] = data[:-1]
            # Might be empty...
            if origTerms:
                orign = bdb.db.DB()
                orign.open(origname)
        else:
            tempfile2 = ""
            origTerms = {}

        proxname = os.path.join(dfp, fn[:-5] + "_PROX")
        if (os.path.exists(proxname)):
            useProx = 1
            proxn = bdb.db.DB()
            proxn.open(proxname)
        else:
            useProx = 0
                
        f = file(sorted)
        l = f.readline()
        termid = long(0)
        # term docid recstore occs (line, posn)*
        while(l):
            termid += 1
            data = l.split('\x00')
            term = data[0]
            fulloccs = map(long, data[1:])
            occs = fulloccs[:3]
            proccs = fulloccs[3:]
            if (useProx):
                prox = []
                for x in range(len(proccs)/2):
                    prox.extend([occs[0], occs[1], proccs[x*2], proccs[x*2+1]])

            l = f.readline()
            # chomp
            l = l[:-1]
            data2 = l.split('\x00')
            term2 = data2[0]
            while term == term2:
                fulloccs2 = map(long, data2[1:])
                occs.extend(fulloccs2[:3])
                proccs2 = fulloccs2[3:]
                if (useProx):
                    for x in range(len(proccs2)/2):
                        prox.extend([fulloccs2[0], fulloccs2[1], proccs2[x*2], proccs2[x*2+1]])
                l = f.readline()
                l = l[:-1]
                if l:
                    data2 = l.split('\x00')
                else:
                    # Obviously it's not going to be the same
                    break
                term2 = data2[0]

            # Check if we're adding or not
            if (nonEmpty):
                val = cxn.get(term)
                if (val <> None):
                    unpacked = list(self._parseIndexStruct(val))
                    unpacked.extend(occs)
                    termid = unpacked[0]
                    unpacked = unpacked[3:]
                else:
                    unpacked = occs
                packed = self._buildIndexStruct(termid, unpacked)
            else:
                packed = self._buildIndexStruct(termid, occs)
            k = origTerms.get(term, None)
            if (k):
                orign.put(str(termid), k)
            cxn.put(term, packed)
            if (useProx):
                packed = self._buildProxStruct(prox)
                proxn.put(str(termid), packed)

        cxn.close()
        if (useProx):
            proxn.close()
        if origTerms:
            orign.close()
        elif os.path.exists(origname):
            os.remove(origname)
        if (tempfile2):
            os.remove(tempfile2)
        os.remove(sorted)
        os.remove(tempfile)

    def fetch_indexList(self, session, req):
        # Return IDs not object pointers
        # XXX: store metadata in a bdb...?
        dfp = self.get_path(session, "defaultPath")
        files = os.listdir(dfp)
        files = filter(self._fileFilter, files)
        ids = []
        start = len(self.id) + 1
        for f in files:
            ids.append(f[start:-6])
        return ids

    def fetch_indexStats(self, session, index):
        raise(NotImplementedError)

    def contains_index(self, session, index):
        # Send Index object, check exists, return boolean
        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        return os.path.exists(os.path.join(dfp, name))

    def create_index(self, session, index):
        # Send Index object to create, null return
        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        fullname = os.path.join(dfp, name)
        if os.path.exists(fullname):
            raise FileDoesNotExistException(fullname)
        cxn = bdb.db.DB()
        try:
            cxn.open(fullname, dbtype=bdb.db.DB_BTREE, flags=bdb.db.DB_CREATE, mode=0660)
            cxn.close()
        except:
            raise ConfigFileException(fullname)
        if (isinstance(index, ProximityIndex)):
            try:
                pxn = bdb.db.DB()
                pxn.open(fullname + "_PROX", dbtype=bdb.db.DB_BTREE, flags=bdb.db.DB_CREATE, mode=0660)
                pxn.close()
            except:
                raise(ValueError)
        # XXX: FTM just create for all
        try:
            oxn = bdb.db.DB()
            oxn.open(fullname + "_ORIG", dbtype=bdb.db.DB_BTREE, flags=bdb.db.DB_CREATE, mode=0660)
            oxn.close()
        except:
            raise(ValueError)

        if (index.get_setting(session, "sortStore")):
            try:
                oxn = bdb.db.DB()
                oxn.open(fullname + "_VALUES", dbtype=bdb.db.DB_BTREE, flags=bdb.db.DB_CREATE, mode=0660)
                oxn.close()
            except:
                raise(ValueError)
        return 1

    def delete_index(self, session, index):
        # Send Index object to delete, null return
        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        try:
            os.remove(os.path.join(dfp, name))
        except:
            raise FileDoesNotExistException(dfp)
        return 1

    def _buildProxStruct(self, prox):
        # docid, storeid, line, posn
        fmt = 'LLLL' * (len(prox) / 4)
        params = [fmt]
        params.extend(prox)
        return apply(struct.pack, params)

    def _parseProxStruct(self, value):
        fmtstr = 'LLLL' * (len(value) / 16)
        pl = struct.unpack(fmtstr, value)
        hash = {}
        for p in range(len(pl) /4):
            doc = (pl[p*4], pl[p*4+1])
            k = hash.get(doc, [])
            k.append(pl[p*4+2:p*4+4])
            hash[doc] = k
        return hash
                
    def _buildIndexStruct(self, id, occList):
        # occList:  [docid, storeid, occs, docid ...] 
        # L: unsigned long  (0 to 4.3 billion)
        # If need to up this (?!!!), go to Q which is unsigned 64 bit

        totalOccs = 0
        totalRecs = len(occList) / 3
        # Funky slices
        for t in occList[2::3]:
            totalOccs += t

        fmt = 'LLL' * (totalRecs + 1)
        params = [fmt, id, totalRecs, totalOccs]
        params.extend(occList)
        return apply(struct.pack, params)

    def _parseIndexStruct(self, value):
        fmtstr = 'LLL' * (len(value) / 12)
        return struct.unpack(fmtstr, value)

    def _unpackIndexStruct(self, session, data):
        unpacked = [data[0], data[1], data[2]]
        for x in range(1,len(data)/3):
            unpacked.append(self.create_item(session, data[x*3], data[x*3+1], data[x*3+2]))
        return unpacked

    def _packIndexStruct(self, a, b, c, data):
        p = [a, b, c]
        for item in data:
            p.extend([data.docid, data.storeid, data.occs])
        return p

    def get_sortValue(self, session, index, item):
        if (self.sortStoreCxn.has_key(index)):
            cxn = self.sortStoreCxn[index]
        else:
            if (not index.get_setting(session, 'sortStore')):
                raise FileDoesNotExistException()
            dfp = self.get_path(session, "defaultPath")
            name = self._generateFilename(index) + "_VALUES"
            fullname = os.path.join(dfp, name)
            cxn = bdb.db.DB()
            cxn.open(fullname)
            self.sortStoreCxn[index] = cxn

        return cxn.get(repr(item))

    def store_terms(self, session, index, hash, record, original=None):
        # Store terms from hash
        # Need to store:  term, totalOccs, totalRecs, (record id, recordStore id, number of occs in record)
        docid = record.id
        if (type(docid) <> types.IntType):
            if (self.recordHash.has_key(docid)):
                docid = self.recordHash[docid]
            else:
                self.recordHash[storeid] = len(self.recordhash.keys())
                docid = self.recordHash[docid]

        storeid = record.recordStore.id
        if (type(storeid) <> types.IntType):
            # Map
            if (self.storeHashReverse.has_key(storeid)):
                storeid = self.storeHashReverse[storeid]
            else:
                self.storeHashReverse[storeid] = len(self.storeHash.keys())
                self.storeHash[self.storeHashReverse[storeid]] = storeid
                storeid = self.storeHashReverse[storeid]
                # XXX: Need to make this persistent!!!

        if (index.get_setting(session, "sortStore")):
            # Store in db for faster sorting
            if self.sortStoreHash.has_key(index):
                fullname = self.sortStoreHash[index]
            else:
                dfp = self.get_path(session, "defaultPath")
                name = self._generateFilename(index) + "_VALUES"
                fullname = os.path.join(dfp, name)
                if (not os.path.exists(fullname)):
                    raise FileDoesNotExistException(fullname)
                self.sortStoreHash[index] = fullname
            cxn = bdb.db.DB()
            cxn.open(fullname)
            # Sort on first occurence
            value = hash.keys()[0]
            cxn.put("%s/%s" % (str(record.recordStore.id), docid), value)
            cxn.close()

        if self.outFiles.has_key(index):
            # save to temporary files for batch loading
            if (self.outOrigFiles.has_key(index)):
                for k in original:
                    self.outOrigFiles[index].write("%s\x00%s\n" % (k, original[k]))

            for k in hash.keys():
                val = hash[k]
                if (type(val) == types.ListType):
                    # Prox
                    self.outFiles[index].write("%s\x00%i\x00%i\x00%i" % (k, docid, storeid, val[0]))
                    for x in val[1:]:
                        self.outFiles[index].write("\x00%i" % x)
                    self.outFiles[index].write("\n")
                else:
                    # Regular
                    self.outFiles[index].write("%s\x00%i\x00%i\x00%i\n" % (k, docid, storeid, val))
        else:
            # Directly insert into index
            dfp = self.get_path(session, "defaultPath")
            name = self._generateFilename(index)
            fullname = os.path.join(dfp, name)
            if (not os.path.exists(fullname)):
                raise FileDoesNotExistException(fullname)
            cxn = bdb.db.DB()
            cxn.open(fullname)

            # This is going to be ... slow ... with lots of i/o
            # Use commit method unless only doing very small amounts of work.
            for k in hash.keys():
                val = cxn.get(k)
                if (val <> None):
                    unpacked = self._parseIndexStruct(val)
                    replace = 0
                    for x in range(1,len(unpacked)/3):
                        if unpacked[x*3] == docid and unpacked[x*3+1] == storeid:
                            unpacked[x*3+2] = hash[k]
                            replace = 1
                            break
                    if (not replace):
                        unpacked.extend([docid, storeid, hash[k]])
                    termid = unpacked[0]
                    unpacked = unpacked[3:]
                else:
                    unpacked = [docid, storeid, hash[k]]
                    termid = cxn.stat()['nkeys']
                packed = self._buildIndexStruct(termid, unpacked)
                cxn.put(k, packed)
            cxn.close()

    def delete_term(self, session, index, term):
        raise(NotImplementedError)

    def fetch_termList(self, session, index, term, numReq=0, relation="", end=""):
        if (not (numReq or relation or end)):
            # XXX Default from config
            numReq = 20
        if (not relation and not end):
            relation = ">="
        if (not relation):
            if (term > end):
                relation = "<="
            else:
                relation = ">"

        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        fullname = os.path.join(dfp, name)
        cxn = bdb.db.DB()
        cxn.open(fullname)
        c = cxn.cursor()
        try:
            (key, data) = c.set_range(term)
        except:
            (key, data) = c.last()
            if (relation in [">", ">="] and term > key):
                # Asked for > than maximum key
                cxn.close()
                return []

        tlist = []

        if (os.path.exists(fullname + "_ORIG")):
            oxn = bdb.db.DB()
            oxn.open(fullname +"_ORIG")
        else:
            oxn = None
            
        if (not (key == term and relation in ['>', '<'])):
            # We want this one
            parsed = self._parseIndexStruct(data)
            tid = parsed[0]
            if (oxn <> None):
                nk = oxn.get(str(tid))
                if (nk):
                    key = nk
            unpacked = self._unpackIndexStruct(session, parsed)
            tlist.append([key, unpacked])

        fetching = 1
        while fetching:
            dir = relation[0]
            if (dir == ">"):
                tup = c.next()
            else:
                tup = c.prev()
            if tup:
                (key, rec) = tup
                if (end and dir == '>' and key >= end):
                    fetching = 0
                elif (end and dir  == "<" and key <= end):
                    fetching = 0
                else:
                    parsed = self._parseIndexStruct(rec)
                    unpacked = self._unpackIndexStruct(session, parsed)
                    if (oxn <> None):
                        nk = oxn.get(str(parsed[0]))
                        if (nk):
                            key = nk
                    tlist.append([key, unpacked])
                    if (numReq and len(tlist) == numReq):
                        fetching = 0
            else:
                key = None
                fetching = 0

        cxn.close()
        if (oxn <> None):
            oxn.close()
        return tlist

    def create_item(self, session, tid, rst, occs):
        recStore = self.storeHash[rst]
        rsi = SimpleResultSetItem(session, tid, recStore, occs)
        return rsi

    def fetch_term(self, session, index, term):
        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        fullname = os.path.join(dfp, name)
        cxn = bdb.db.DB()
        cxn.open(fullname)
        val = cxn.get(term)
        unpacked = []
        if (val <> None):
            data = self._parseIndexStruct(val)
            unpacked.extend([data[0], data[1], data[2]])
            for x in range(1,len(data)/3):
                unpacked.append(self.create_item(session, data[x*3], data[x*3+1], data[x*3+2]))
            if (isinstance(index, ProximityIndex)):
                pxn = bdb.db.DB()
                pxn.open(fullname + "_PROX")
                pval = pxn.get(str(data[0]))
                pdata = self._parseProxStruct(pval)
                pxn.close()
                unpacked = (unpacked, pdata)
        cxn.close()
        return unpacked

    def handle_search(self, session, index, clause, request=None):
        # clause is just this index, request is full request
        # eg for SQL order by etc.
        dfp = self.get_path(session, "defaultPath")
        name = self._generateFilename(index)
        fullname = os.path.join(dfp, name)
        cxn = bdb.db.DB()
        cxn.open(fullname)
        # XXX ????
        # Do we need this?

        cxn.close()

    def close(self):
        pass


class C2IndexStore(BDBIndexStore):
    """Use C2 style indexes, only one recordStore"""
    pass
