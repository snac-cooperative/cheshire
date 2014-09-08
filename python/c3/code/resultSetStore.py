
from configParser import C3Object
from baseObjects import ResultSetStore
from resultSet import SimpleResultSet, SimpleResultSetItem
import os, types, struct, sys, commands, string, time
from random import Random
try:
    # Python 2.3 vs 2.2
    import bsddb as bdb
except:
    import bsddb3 as bdb

try:
    from Ft.Lib.Uuid import GenerateUuid, UuidAsString
    use4Suite = 1
except:
    use4Suite = 0

randomGen = Random(time.time())
asciiChars = string.ascii_letters + string.digits + "@%#!-_=."

# name of result set needs to be unique within RSS
# But users may name result sets non uniquely in Z
# This map needs to happen at the user/session end

# Hence only the RSS can name a result set.

class BDBResultSetStore(ResultSetStore):
    storeHash = {}
    storeHashReverse = {}
    cxn = None
    txn = None

    def __init__(self, parent, config):
        C3Object.__init__(self, parent, config)
        rsh = self.get_path(None, 'recordStoreHash')
        self.cxn = None
        if rsh:
            wds = rsh.split()
            for w in range(len(wds)):
                self.storeHash[long(w)] = wds[w]
                self.storeHashReverse[wds[w]] = long(w)
        dbp = self.get_path(None, 'databasePath')
        dfp = self.get_path(None, 'defaultPath')

        if (not dbp):
            dbp = "resultSetStore_" + self.id

        if (not os.path.isabs(dbp)):
            if (not dfp):
                raise(ConfigFileException("RecordStore has relative path, and no visible defaultPath."))
            dbp = os.path.join(dfp, dbp)
        # Record fully resolved path
        self.paths['databasePath'] = dbp
        self._verifyDatabase(dbp)

    def _verifyDefault(self, type, value):
        if (type == "resultSetTimeout"):
            return int(value)
        else:
            return value

    def _verifyDatabase(self, path):
        if (not os.path.exists(path)):
            # We don't exist, try and instantiate new database
            self._initialise(path)
        else:
            cxn = bdb.db.DB()
            try:
                cxn.open(path)
                cxn.close()
            except:
                # Still don't exist
                self._initialise(path)

    def _initialise(self, path):
        cxn = bdb.db.DB()
        cxn.open(path, dbtype=bdb.db.DB_BTREE, flags = bdb.db.DB_CREATE, mode=0664)
        cxn.close()

    def _openContainer(self):
        if self.cxn == None:
            cxn = bdb.db.DB()
            cxn.open(self.get_path(None, 'databasePath'))
            self.cxn = cxn

    def _closeContainer(self):
        if self.cxn <> None:
            self.cxn.close()
            self.cxn = None

    def _createId(self):
        if (use4Suite):
            id = UuidAsString(GenerateUuid())
        else:
            id = commands.getoutput('uuidgen')
            if (len(id) <> 36 or id[8] <> '-'):
                c = []
                for x in range(16):
                    c.append(asciiChars[randomGen.randrange(len(asciiChars))])
                id = ''.join(c)
        return id

    def create_resultSet(self, session, rset=None):
        self._openContainer()
        id = self._createId()
        # Just to be sure, if random ascii is generated

        if (not rset):
            # Create a place holder with no information
            now = time.time()
            expires = now + self.get_default(session, 'resultSetTimeout')
            self.cxn.put("_%s__%f" % (id, expires), "")
            self.cxn.put("%f" % (expires), id)
            self.close(session)
        else:
            rset.id = id
            self.store_resultSet(session, rset)
        return id

    def delete_resultSet(self, session, rsid):
        self._openContainer()
        c = self.cxn.cursor()
        (key, data) = c.set_range("_%s__" % (rsid))
        if (key[1:len(rsid)+1] <> rsid):
            # Not result set
            raise ValueError
        cexp = key[len(rsid)+3:]
        self.cxn.delete(cexp)
        self.cxn.delete(key)
        self.close(session)

    def fetch_resultSet(self, session, rsid):
        self._openContainer()
        c = self.cxn.cursor()
        try:
            (key, data) = c.set_range("_%s__" % (rsid))
        except:
            return None
        if (key[1:len(rsid)+1] <> rsid):
            # Not result set
            return None

        # XXX:  Need to know requested timeout??
        now = time.time()
        expires = now + self.get_default(session, 'resultSetTimeout')

        # Update expires
        self.delete_resultSet(session, rsid)
        self._openContainer()
        self.cxn.put("_%s__%f" % (rsid, expires), data)
        self.cxn.put("%f" % (expires), rsid)

        if (data):
            unpacked = struct.unpack("L" * (len(data) / 4), data)
            items = []
            for o in range(len(unpacked) /3):
                items.append(SimpleResultSetItem(session, unpacked[o*3], self.storeHash[unpacked[o*3+1]], unpacked[o*3+2])) 
            self.close(session)
            return SimpleResultSet(session, items, rsid)
        else:
            self.close(session)
            return SimpleResultSet(session, [], rsid)

    def fetch_resultSetList(self, session):
        self._openContainer()
        c = self.cxn.cursor()
        (key, data) = c.set_range("_")
        keys = [key[1:37]]
        n = c.next()
        while(n):
            keys.append(n[0][1:37])
            n = c.next()
        return keys

    def store_resultSet(self, session, rset):
        # Store document links
        # (long, long) -> (store, docid)
        # load store map ala IndexStore

        self._openContainer()
        idlist = []
        for k in range(len(rset)):
            storeid = rset[k].recordStore
            id = rset[k].docid
            if (type(storeid) <> types.IntType):
                # Map
                if (self.storeHashReverse.has_key(storeid)):
                    storeid = self.storeHashReverse[storeid]
                else:
                    self.storeHashReverse[storeid] = len(self.storeHash.keys())
                    self.storeHash[self.storeHashReverse[storeid]] = storeid
                    storeid = self.storeHashReverse[storeid]
                    # XXX: Need to make this persistent!!!
            idlist.extend([id, storeid, rset[k].occurences])
        params = ['L' * len(idlist)]
        params.extend(idlist)
        data = apply(struct.pack, params)
        now = time.time()
        if (rset.expires):
            expires = now + rset.expires
        else:
            expires = now + self.get_default(session, 'resultSetTimeout')
        try:
            self.delete_resultSet(session, rset.id)
        except:
            pass
        self._openContainer()
        self.cxn.put("_%s__%f" % (rset.id, expires), data)
        self.cxn.put("%f" % (expires), rset.id)
        self.close(session)

    def clean(self, session):
        now = time.time()
        self._openContainer()
        c = self.cxn.cursor()
        try:
            (key, data) = c.set_range(str(now))
        except:
            # No database
            return 0
        deleted = 0
        while key:
            if (key[0] <> '_'):
                if (float(key) <= now):
                    self.delete_resultSet(session, data)
                    deleted += 1
            try:
                (key, data) = c.prev()
            except:
                return deleted

    def close(self, session):
        self._closeContainer()

