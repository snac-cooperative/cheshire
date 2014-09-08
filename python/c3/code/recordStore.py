
from xml.dom import Node
from baseObjects import RecordStore
from record import SaxRecord
from c3errors import *
from configParser import C3Object
from document import StringDocument
import os, types, md5, sha

try:
    # name when installed by hand
    import bsddb3 as bdb
except:
    # name that comes in python 2.3
    import bsddb as bdb


class SimpleRecordStore(RecordStore):
    # Instantiate some type of simple record store
    parent = None
    inTransformer = None
    outParser = None
    currentId = -1

    def _numericToString(self, session, id):
        if (self.idNormaliser):
            return self.idNormaliser.process_string(session, str(id))
        else:
            return "%010d" % (id) 

    def __init__(self, node, parent):

        C3Object.__init__(self, node, parent)
    
        # databasePath is some sort of on disk filename
        # Might be directory, but might not, so can't use defaultPath
        # Not a path to an xml config
        databasePath = self.get_path(None, 'databasePath')
        if (not databasePath):
            databasePath = "recordStore_" + self.id

        if (not os.path.isabs(databasePath)):
            # Prepend defaultPath from parents
            dfp = self.get_path(None, 'defaultPath')
            if (not dfp):
                raise(ConfigFileException("RecordStore has relative path, and no visible defaultPath."))
            databasePath = os.path.join(dfp, databasePath)

        # Record fully resolved path
        self.paths['databasePath'] = databasePath

        # If these are None, then implementation must store/return Record
        self.inTransformer = self.get_path(None, 'inTransformer')
        self.outParser = self.get_path(None, 'outParser')
        self.idNormaliser = self.get_path(None, 'idNormaliser')
        self._verifyDatabase()

    def _verifyDatabase(self):
        pass

    def fetch_record(self, session, id):
        pass
    def fetch_recordList(self, session, req):
        pass
    def create_record(self, session, record=None):
        pass
    def store_record(self, session, record):
        pass
    def delete_record(self, session, id):
        pass
    def fetch_recordSize(self, session, id):
        r = self.fetch_record(session, id)
        return r.size


class FileSystemRecordStore(SimpleRecordStore):
    """ Very simple file system record store ...
    We just save records to disk with id as filename"""

    def _verifyDatabase(self):
        dbp = self.get_path(None, 'databasePath')
        if (not os.path.exists(dbp)):
            # Try and make it? Otherwise error
            os.mkdir(dbp, 0755)

    def create_record(self, session, record=None):
        # Need to allow non numeric IDs (eg config/authStore)

        if self.currentId == -1:
            # Lets do this only once!
            files = os.listdir(self.paths['databasePath'])
            files = filter(lambda a: len(a) == 15 and a[:10].isdigit(), files)
            if len(files):
                files.sort()
                last = files[-1][:-5]
                id = int(last) + 1
            else:
                id = 1
            self.currentId = id
        else:
            self.currentId += 1
            id = self.currentId

        if (record == None):
            # Create a placeholder
            record = SaxRecord([], "", id)
        record.id = id
        filename = self._numericToString(session, id) + ".sgml"
            
        filename = os.path.join(self.get_path(None, 'databasePath'), filename)
        record.recordStore = self
        if (self.inTransformer):
            doc = self.inTransformer.process_record(session, record)
            data = doc.get_raw()
        else:
            # Just dump raw XML, which is currently empty
            data = ""
        handle = file(filename, "w")
        handle.write(data)
        handle.close()
        return id

    def store_record(self, session, record):
        record.recordStore = self
        if (type(record.id) == types.IntType):
            filename = self._numericToString(session, record.id) + ".sgml"
        else:
            filename = record.id + ".sgml"

        filename = os.path.join(self.paths['databasePath'], filename)
        if (self.inTransformer):
            doc = self.inTransformer.process_record(session, record)
            data = doc.get_raw()
        else:
            data = record.get_xml()
        handle = file(filename, "w")
        handle.write(data)
        handle.close()

    def fetch_record(self, session, id):
        if (type(id) == types.IntType):
            filename = self._numericToString(session, id) + ".sgml"
        else:
            filename = id + ".sgml"

        filename = os.path.join(self.paths['databasePath'], filename)
        handle = file(filename)
        data = handle.read()
        handle.close()
        doc = StringDocument(data)
        if (self.outParser):
            record = self.outParser.process_document(None, doc)
        else:
            #Use default parser
            parser = self.get_object('defaultParser')
            record = parser.process_document(None, doc)
        record.recordStore = self
        return record

    def delete_record(self, session, id):
        if (type(id) == types.IntType):
            filename = self._numericToString(session, id) + ".sgml"
        else:
            filename = id + ".sgml"

        filename = os.path.join(self.paths['databasePath'], filename)
        os.remove(filename)

    def fetch_recordList(self, session, req):
        files = os.listdir(self.paths['databasePath'])
        files.sort()
        # Now do something???
        

class BdbRecordStore(SimpleRecordStore):
    cxn = None
    currentId = -1

    def _verifyDatabase(self):
        dbp = self.get_path(None, 'databasePath')

        if (not os.path.exists(dbp)):
            # We don't exist, try and instantiate new database
            self._initialise()
        else:
            cxn = bdb.db.DB()
            try:
                cxn.open(dbp)
                cxn.close()
            except:
                # Still don't exist
                self._initialise()

    def _initialise(self):
        cxn = bdb.db.DB()
        cxn.open(self.get_path(None, 'databasePath'), dbtype=bdb.db.DB_BTREE, flags = bdb.db.DB_CREATE, mode=0660)
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

    def create_record(self, session, record=None):
        self._openContainer()
        # NB: First record is 0
        if (self.currentId == -1):
            c = self.cxn.cursor()
            try:
                id = '999999999999'
                if (self.idNormaliser):
                    id = self.idNormaliser.process_string(session, id)
                item = c.set_range(id)
                if (item[0][0] == "_"):
                    item = c.prev()
                id = int(item[0])
            except:
                id = 0
        else:
            id = self.currentId +1
        self.currentId = id
            
        if (record == None):
            # Create a placeholder
            record = SaxRecord([], "", id)
        else:
            record.id = id

        record.recordStore = self
        self.store_record(session, record)
        return id

    def store_record(self, session, record):
        self._openContainer()
        record.recordStore = self
        # Check SHA/MD5 before transforming (and adding dates etc)
        data = repr(record.get_sax())
        digest = self.get_setting(session, "digest")
        if (digest):
            if (digest == 'md5'):
                dmod = md5
            elif (digest == 'sha'):
                dmod = sha
            else:
                raise ValueError
            m = dmod.new()
            m.update(data)
            digest = m.hexdigest()
            if (self.cxn.get("_" + digest)):
                # XXX Better Error
                raise ValueError
        if (self.inTransformer <> None):
            doc = self.inTransformer.process_record(session, record)
            data = doc.get_raw()
        id = str(record.id)
        if (self.idNormaliser <> None):
            id = self.idNormaliser.process_string(session, str(record.id))
        self.cxn.put(id, data)
        if digest:
            self.cxn.put("_" + digest, id)
        if (record.size):
            self.cxn.put("__size_%s" % (id), str(record.size))


    def fetch_record(self, session, id):
        self._openContainer()
        sid = str(id)
        if (self.idNormaliser <> None):
            sid = self.idNormaliser.process_string(session, sid)
        data = self.cxn.get(sid)
        if (data):
            if (self.outParser <> None):
                doc = StringDocument(data)
                record = self.outParser.process_document(session, doc)
            else:
                record = SaxRecord(eval(data))
                record.id = id
            record.recordStore = self
            return record
        else:
            return None

    def delete_record(self, session, id):
        self._openContainer()
        try:
            sid = str(id)
            if (self.idNormaliser <> None):
                sid = self.idNormaliser.process_string(session, str(record.id))
            self.cxn.delete(sid)
        except:
            raise(KeyError)

    def get_recordSize(self, session, id):
        self._openContainer()
        sid = str(id)
        if (self.idNormaliser <> None):
            sid = self.idNormaliser.process_string(session, sid)
        rsz = self.cxn.get("__size_%s" % (sid))
        if (rsz):
            return long(rsz)
        else:
            return -1

    def close(self, session):
        self._closeContainer()


