#!/usr/bin/python

import ZCheshire
import sys

class ZoomError (Exception):
    """Base error class"""
    pass

class ConnectionError (ZoomError):
    pass

class ClientNotImplError (ZoomError):
    pass

class ServerNotImplError (ZoomError):
    pass

class QuerySyntaxError (ZoomError):
    pass

class ProtocolError (ZoomError):
    pass

class UnknownRecSyn (ZoomError):
    pass

class Bib1Err (ZoomError):
    """Bib1 Error Exception"""
    def __init__ (self, condition, message, addInfo):
        self.condition = condition
        self.message = message
        self.addInfo = addInfo
        ZoomError.__init__ (self)

    def __str__ (self):
        return "Bib1Err: %d %s %s" % (self.condition, self.message, self.addInfo)



class baseZoomObject:
    """Base Zoom Object"""
    attrlist = []
    def __setattr__ (self, attr, val):
        if attr[0] == '_' or attr in self.attrlist:
            self.__dict__[attr] = val
        else:
            raise AttributeError (attr, val)


class Connection(baseZoomObject):
    """Connection Object"""
    attrlist = ['stepSize', 'numberOfEntries', 'responsePosition', 'databaseName', 'recordSyntax', 'elementSetName', '_session', '_rsetctr', 'namedResultSets', 'host', 'port']
    host = ''
    port = 0
    databaseName = 'default'
    elementSetName = 'F'
    recordSyntax = 'XML'
    stepSize = 0
    numberOfEntries = 20
    responsePosition = 1
    namedResultSets = 1
    _rsetctr = 0
    _session = 0

    def __init__(self, host, port, **kw):
        """Establish connection to z3950://host:port/"""

        if (not host):
            raise ValueError('Host may not be null')
        if (port < 1):
            raise ValueError('Port must be greater than zero')

        """ May raise an exception on unknown attribute """
        for (k,v) in kw.items():
            if k in self.attrlist:
                setattr(self, k, v)

        try:
            result = ZCheshire.connect(host, port)
            self._session = result[result.find("'")+1:result.rfind("'")]
        except ZCheshire.error, strerror:
            raise ConnectionError(strerror)
        self.host = host
        self.port = port

    def generate_resultSetName(self):
        self._rsetctr += 1
        return 'rset%s' % (self._rsetctr)

    def search(self, query):
        ZCheshire.set('session', self._session)
        ZCheshire.set('database', self.databaseName)
        try:
            if self.namedResultSets:
                rsetname = self.generate_resultSetName()
                results = ZCheshire.search(query.querystr, rsetname)
            else:
                results = ZCheshire.search(query.querystr)
        except ZCheshire.error, strerror:
            if strerror[:10] == "Diagnostic":
                colonidx = strerror.find(':')
                code = int(strerror[11:colonidx])
                message = strerror[colonidx+1:].strip()
                raise Bib1Err(code, message, '')
            elif strerror == "Syntax error in query":
                raise QuerySyntaxError
            else:
                print strerror
                raise ConnectionError

        return ResultSet(self, results)

    def scan(self, query):
        """Need to extract index and term from query... either quoted or last word
        If we have any boolean, we should fail, not send to server. (ToDo, requires Python Cheshire Parser)
        """
        ZCheshire.set('session', self._session)
        ZCheshire.set('database', self.databaseName)
        qstr = query.querystr
        if qstr[-1] == '"':
            idx = qstr[:-2].rfind('"')
            term = qstr[idx+1:-2].strip()
            index = qstr[:idx].strip()
        else:
            idx = qstr.rfind(' ')
            term = qstr[idx+1:].strip()
            index = qstr[:idx].strip()

        try:
            results = ZCheshire.scan(index, term, self.stepSize, self.numberOfEntries, self.responsePosition)
        except ZCheshire.error, strerror:
            if strerror[:10] == "Diagnostic":
                colonidx = strerror.find(':')
                code = int(strerror[11:colonidx])
                message = strerror[colonidx+1:].strip()
                raise Bib1Err(code, message, '')
            elif strerror == "Syntax error in query":
                raise QuerySyntaxError
            else:
                raise ConnectionError
            
        return ScanSet(results)

    def close(self):
        ZCheshire.set('session', self._session)
        ZCheshire.close()

class Query:
    type = "C"
    querystr = ""
    def __init__ (self, query, type='C'):
        """Creates query object"""
        if type != 'C' and type != 'c':
            raise ClientNotImplError ('Only Cheshire Query supported (type C).')
        elif not query:
            raise ValueError("Must give a query")
        else:
            self.querystr = query


class ResultSet(baseZoomObject):
    attrlist = ['recordSyntax', 'elementSetName', 'connection', 'records', 'results', 'name']
    records = {}
    results = []
    name = ""
    _connection = 0
    
    def __init__ (self, conn, results):
        self._connection = conn;
        self.results = results;
        self.name = results['resultSet']
        self.records = {}

    def _do_present(self, start, num):
        ZCheshire.set('session', self._connection._session)
        ZCheshire.set('elementSet', self.elementSetName)
        try:
            ZCheshire.set('recordSyntax', self.recordSyntax)
        except ZCheshire.error:
            raise UnknownRecSyn

        try: 
            results = ZCheshire.present(start+1, num, self.results['resultSet'])
        except ZCheshire.error, strerror:
            if strerror[:10] == "Diagnostic":
                colonidx = strerror.find(':')
                code = int(strerror[11:colonidx])
                message = strerror[colonidx+1:].strip()
                raise Bib1Err(code, message, '')
            else:
                raise strerror

        ridx = 0
        for r in range(start, start + num):
            record = Record(self, results['records'][ridx])
            self.records[r] = record
            ridx += 1

    def __setattr__ (self, key, value):
        """If we change recsyntax or esn, then clear record cache"""
        if (key == "elementSetName" or key == "recordSyntax"):
            self.records = {}
        baseZoomObject.__setattr__ (self, key, value)

    def __getattr__ (self, key):
        if (key == "elementSetName"):
            return self._connection.elementSetName
        elif (key == "recordSyntax"):
            return self._connection.recordSyntax
        else:
            raise AttributeError(key)
        
    def __len__ (self):
        return self.results['hits']

    def _map_int (self, i):
        """Negative indices count back from end(?)"""
        if i < 0:
            return len(self) - i
        return i

    def __getitem__(self, i):
        i = self._map_int(i);
        if i >= len(self):
            raise IndexError
        if self.records.has_key(i):
            return self.records[i]
        else:
            self._do_present(i, 1)
            return self.records[i]

    def __getslice__ (self, i, j):
        i = self._map_int(i)
        j = self._map_int(j)
        if (i < 0 or j < 0 or i >= len(self) or j >= len(self)):
            raise IndexError

        if (len(self.records) == 0):
            """ We don't have any records yet, so just grab them all at once"""
            self._do_present(i, j-i)

        else:
            for k in range(i,j):
                """ Check we have each record """
                if (not self.records.has_key(k)):
                    self._do_present(k, 1)
        """ Create list and return """
        reclist = []
        for k in range(i,j):
            reclist.append(self.records[k])
        return reclist
        
            
    def delete(self):
        """Delete resultset on server too"""
        ZCheshire.set('session', self._connection._session)
        ZCheshire.delete(self.results['resultSet'])


class Record(baseZoomObject):
    attrlist = ['data', 'recordSyntax', 'elementSetName']
    data = ''
    recordSyntax = ''
    elementSetName = ''
    
    def __init__ (self, rset, data):
        self.elementSetName = rset.elementSetName
        self.recordSyntax = data['recordSyntax']
        self.data = data['record']
    def __str__ (self):
        """render minimally"""
        return self.data
    def render (self, **kw):
        """Clever Render -- accept keywords and process"""
        
    def get_raw (self):
        """Return raw data"""
        return self.data

class ScanSet(baseZoomObject):
    """Should treat as a list of dictionaries"""
    attrlist = ['results']
    def __init__ (self, results):
        self.results = results

    def __len__ (self):
        return self.results['numTerms']

    def __getitem__ (self, key):
        """ Scansets need to look like a list of dictionaries """
        if key < 0:
            raise IndexError
        elif key >= len(self.results['terms']):
            raise IndexError
        else:
            t = self.results['terms'][key]
            idx = t.find('(')
            occurences = t[idx+1:-1]
            term = t[:idx]
            return {'term' : term, 'freq' : occurences}
        

