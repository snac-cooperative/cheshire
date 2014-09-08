# Version 0.4
# Refactoring for C3 design, rather than experimental
# Should allow SQL/BDB and DOM/SAX all interchangably
# No implementation, this is just a skeleton file to define API

from configParser import C3Object

class Server(C3Object):

    databases = {}
    authStore = None
    resultSetStore = None
    queryStore = None

    def __init__(self, configFile="serverConfig.xml"):
        "Instantiate ourselves from configuration file"
        raise(NotImplementedError)

    def handle_connect(self, req):
        "Accept a connection to the server and perform any per session initialisation"
        "Return a Session object"
        raise(NotImplementedError)
    def handle_disconnect(self, session, req):
        "Called when a connection is dropped to perform any end of session tidying."
        raise(NotImplementedError)

    def handle_search(self, session, req):
        "Handle a search style request and return response"
        raise(NotImplementedError)
    def handle_scan(self, session, req):
        "Handle a browse/scan style request and return response"
        raise(NotImplementedError)
    def handle_sort(self, session, req):
        "Handle a sort style request and return response"
        raise(NotImplementedError)
    def handle_retrieve(self, session, req):
        "Handle a get/retrieve/present style request and return response"
        raise(NotImplementedError)
    def handle_explain(self, session, req):
        "Handle a get/retrieve/present style request for server metadata"
        "Not sure if this is at all useful."
        raise(NotImplementedError)
    def handle_deleteResultSet(self, session, req):
        "Handle a request to delete an existing result set"
        raise(NotImplementedError)

    def handle_addRecord(self, session, req):
        "Handle a request to add a record to a database."
        raise(NotImplementedError)
    def handle_replaceRecord(self, session, req):
        "Handle a request to replace a record in a database."
        raise(NotImplementedError)
    def handle_deleteRecord(self, session, req):
        "Handle a request to delete a record from a database."
        raise(NotImplementedError)
    def handle_indexRecord(self, session, req):
        "Handle a request to index a specific record."
        raise(NotImplementedError)

    def handle_addDatabase(self, session, req):
        "Handle a request to create a new database from a configuration."
        raise(NotImplementedError)
    def handle_replaceDatabase(self, session, req):
        "Handle a request to replace a database configuration."
        raise(NotImplementedError)
    def handle_deleteDatabase(self, session, req):
        "Handle a request to delete an existing database."
        raise(NotImplementedError)
    def handle_indexDatabase(self, session, req):
        "Handle a request to re-index from scratch an existing database."
        raise(NotImplementedError)

    def handle_authenticate(self, session, req):
        "Handle authentication of session/request and establish user profile."
        raise(NotImplementedError)


class Database(C3Object):
    
    indexes = {}
    protocolMaps = {}
    recordStore = None

    def handle_addDatabase(self, session, req):
        # Different from __init__ how??
        raise(NotImplementedError)
    def handle_addRecord(self, session, record):
        raise(NotImplementedError)
    def handle_authenticate(self, session, req):
        raise(NotImplementedError)
    def handle_connect(self, session, req):
        raise(NotImplementedError)
    def handle_deleteDatabase(self, session, req):
        raise(NotImplementedError)
    def handle_deleteRecord(self, session, req):
        raise(NotImplementedError)
    def handle_deleteResultSet(self, session, req):
        raise(NotImplementedError)
    def handle_disconnect(self, session, req):
        raise(NotImplementedError)
    def handle_editDatabase(self, session, req):
        raise(NotImplementedError)
    def handle_editRecord(self, session, req):
        raise(NotImplementedError)
    def handle_explain(self, session, req):
        raise(NotImplementedError)
    def handle_indexDatabase(self, session, req):
        raise(NotImplementedError)
    def handle_indexRecord(self, session, req):
        raise(NotImplementedError)
    def handle_retrieve(self, session, req):
        raise(NotImplementedError)
    def handle_scan(self, session, req):
        raise(NotImplementedError)
    def handle_search(self, session, req):
        raise(NotImplementedError)
    def handle_sort(self, session, req):
        raise(NotImplementedError)


class Index(C3Object):
    indexStore = None

    def handle_deleteRecord(self, session, record):
        raise(NotImplementedError)
    def handle_updateRecord(self, session, record):
        raise(NotImplementedError)
    def handle_indexRecord(self, session, record):
        raise(NotImplementedError)
    def handle_scan(self, session, req):
        raise(NotImplementedError)
    def handle_search(self, session, req):
        raise(NotImplementedError)
    def handle_sort(self, session, req):
        raise(NotImplementedError)

# Takes some data, returns a list of extracted values
class Extracter(C3Object):
    def process_string(self, session, data):
        raise(NotImplementedError)
    def process_node(self, session, data):
        raise(NotImplementedError)
    def process_eventList(self, session, data):
        raise(NotImplementedError)

# Takes a string, returns a list of normalised values
class Normaliser(C3Object):
    def process_string(self, session, data):
        raise(NotImplementedError)

# Takes a Document, returns a Record
class Parser(C3Object):
    def process_document(self, session, doc):
        raise(NotImplementedError)

# Takes a Document, returns a Document
class PreParser(C3Object):
    def process_document(self, session, doc):
        raise(NotImplementedError)

# Takes a Record, returns a string (Record?) 
class Transformer(C3Object):
    def process_record(self, session, rec):
        raise(NotImplementedError)

# Users instantiated out of AuthStore like any other configured object
class User(C3Object):
    username = ''
    password = ''
    rights = []

    groups = []
    history = []
    email = ''
    address = ''
    tel = ''
    realName = ''
    description = ''
    hostmask = []

    def get_right(self, flag):
        raise(NotImplementedError)


class ProtocolMap(C3Object):
    protocol = ""

    def map_index(self, session, data):
        raise(NotImplementedError)
    

# --- Store APIs ---

# Not an object store, we just look after terms and indexes
class IndexStore(C3Object):

    def begin_indexing(self, session, req):
        raise(NotImplementedError)
    def commit_indexing(self, session, req):
        raise(NotImplementedError)

    def contains_index(self, session, index):
        raise(NotImplementedError)
    def create_index(self, session, index):
        raise(NotImplementedError)
    def delete_index(self, session, index):
        raise(NotImplementedError)
    def fetch_indexList(self, session, req):
        raise(NotImplementedError)
    def fetch_indexStats(self, session, index):
        raise(NotImplementedError)

    def delete_term(self, session, req):
        raise(NotImplementedError)
    def store_term(self, session, req):
        raise(NotImplementedError)

    def fetch_term(self, session, req):
        raise(NotImplementedError)
    def fetch_termList(self, session, req):
        raise(NotImplementedError)


class ObjectStore(C3Object):
    def create_object(self, session, object=None):
        raise(NotImplementedError)
    def delete_object(self, session, id):
        raise(NotImplementedError)
    def fetch_object(self, session, id):
        raise(NotImplementedError)
    def fetch_objectList(self, session, req):
        raise(NotImplementedError)
    def store_object(self, session, object):
        raise(NotImplementedError)


# Are these needed? Can they just be ObjectStore + Config?
class QueryStore(ObjectStore):
    def create_query(self, session, query=None):
        raise(NotImplementedError)
    def delete_query(self, session, id):
        raise(NotImplementedError)
    def fetch_query(self, session, id):
        raise(NotImplementedError)
    def fetch_queryList(self, session, req):
        raise(NotImplementedError)
    def store_query(self, session, object):
        raise(NotImplementedError)


class RecordStore(ObjectStore):
    def create_record(self, session, record=None):
        raise(NotImplementedError)
    def delete_record(self, session, id):
        raise(NotImplementedError)
    def fetch_record(self, session, id):
        raise(NotImplementedError)
    def fetch_recordList(self, session, req):
        raise(NotImplementedError)
    def store_record(self, session, record):
            raise(NotImplementedError)

class ResultSetStore(ObjectStore):
    def create_resultSet(self, session, rset=None):
        raise(NotImplementedError)
    def delete_resultSet(self, session, req):
        raise(NotImplementedError)
    def fetch_resultSet(self, session, req):
        raise(NotImplementedError)
    def fetch_resultSetList(self, session, req):
        raise(NotImplementedError)
    def store_resultSet(self, session, req):
        raise(NotImplementedError)

# --- Code Instantiated Objects ---

class Document:
    text = ''
    def __init__(self, data):
        raise(NotImplementedError)
    def get_raw(self):
        raise(NotImplementedError)

class DocumentGroup:
    def __init__(self, data):
        raise(NotImplementedError)
    def get_document(self, session, idx):
        raise(NotImplementedError)


class ResultSet:
    sortKeys = None
    source = None
    name = ""

    def handle_deleteResultSet(self, session, req):
        raise(NotImplementedError)
    def handle_replaceResultSet(self, session, req):
        raise(NotImplementedError)
    def handle_retrieve(self, session, req):
        raise(NotImplementedError)
    def handle_sort(self, session, req):
        raise(NotImplementedError)

class ResultSetItem(object):
    pass


class Record:
    schema = ''
    schemaType = ''
    status = ''
    baseUri = ''
    history = []
    rights = []
    recordStore = None
    elementHash = {}

    dom = None
    xml = ""
    sax = []

    def __init__(self, data, xml, docid=None):
        raise(NotImplementedError)

    def get_dom(self):
            raise(NotImplementedError)
    def get_sax(self):
            raise(NotImplementedError)
    def get_xml(self):
            raise(NotImplementedError)


class Session:
    user = None
    inHandler = None
    outHandler = None

