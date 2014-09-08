
# Requirements:
#  Provide a single interface to multiple connections and return a list of resultsets.
#  Connections may be SRW or Z39.50
#  Z Connections may be to MARC (bl) or XML (ArchivesHub)

from PyZ3950 import zoom, zmarc
from threading import Thread
import time, random, sys
from mod_python import apache, util


class Server:
    name = ""
    host = ""
    port = 0
    connection = None
    options = {}
    queryCache = {}

    def __init__(self, host, port, **kw):
        self.host = host
        self.port = port
        options = {'preferredRecordSyntax' : 'USMARC',
                   'elementSetName' : 'F'}
        for k in kw.keys():
            options[k] = kw[k]
        self.options = options
        self.connect()

    def connect(self):
        # Connect
        global trace
        if (trace):
            sys.stderr.write("(Re)Connecting to %s:%i/%s\n" % (self.host, self.port, self.options['databaseName']))
            sys.stderr.flush()
        self.queryCache = {}
        # XXX Need a Timeout
        self.connection =  zoom.Connection(self.host, self.port, **self.options)


class ZoomThread(Thread):
    config = None
    query = None
    result = None

    def run(self):
        global request, trace
        if (trace):
            request.write("%s: Searching %s\n" % (time.ctime(), self.config.connection.host))
        retry = 0
        rs = ""
        while retry < 3:
            try:
                rs = self.config.connection.search(self.query)
                retry = 3
            except zoom.ConnectionError, err:
                # Reconnect
                conn = self.config.connect()
                retry = retry + 1
            except zoom.Bib1Err, bib1:
                # Nothing we can do
                if (trace):
                    request.write(str(bib1))
                retry = 3
            except:
                # Something went really badly
                retry = 3
        if (trace):
            request.write("%s: Searched %s\n" % (time.ctime(), self.config.connection.host))
        # Now prefetch some records, if small
        if (self.config.connection.preferredRecordSyntax == 'USMARC' or self.config.connection.elementSetName in ['TPGRS', 'B']):
            end = min(10, len(rs))
            r = rs[0:end]
        if (trace):
            request.write("%s: Fetched %i records from %s.\n" % (time.ctime(), end, self.config.connection.host))
        self.result = rs

def search(qstr, conns=[]):
    # Take a zoom Query and throw to all of our connections.
    global connections, request, trace

    query = zoom.Query('CQL', qstr)
    results = {}
    threads = []
    if not conns:
        conns = connections

    # Ooooh. NB: Abuses 'for k in dict' vs 'for k in seq'
    for key in conns:
        config = connections[key]
        if (config.queryCache.has_key(qstr)):
            results[key] = config.queryCache[qstr]
            keys = config.queryCache.keys()
            if (len(keys) > 100):
                rnd = random.choice(keys)
                del config.queryCache[random]
        else:
            zt = ZoomThread()
            zt.config = connections[key]
            zt.query = query
            zt.name = key
            threads.append(zt)
            zt.start()

    while (threads):
        if (trace):
            request.write("Waiting for threads...\n")
        for t in threads:
            if (t.result <> None):
                results[t.name] = t.result
                threads.remove(t)
        if threads:
            time.sleep(1)

    return results


# Forge connections at startup and reconnect when necessary
#connections['ox'] = Server(host='library.ox.ac.uk', port=210, databaseName='advance')

request = None
trace = 0
connections = {}
connections['ah'] = Server(host='www.archiveshub.ac.uk', port=1212, databaseName='eadhub', preferredRecordSyntax ='GRS-1', elementSetName = 'TPGRS')
connections['swan'] = Server(host='voyager.swan.ac.uk', port=7090, databaseName='voyager')
connections['nott'] = Server(host='aleph.nottingham.ac.uk', port=9909, databaseName='NOT01')


def render(record):
    if (record.syntax == 'USMARC'):
        m = zmarc.MARC(record.data)
        return str(m)
    elif (record.syntax == 'GRS-1'):
        grs = record.data.children
        stuff = {}
        for node in grs:
            type, value = (node.leaf.tagValue[1], node.leaf.content[1])
            if (type == 1):
                stuff['title'] = value.replace('\n', '').strip()
            elif (type == 5):
                stuff['docid'] = value
            else:
                stuff['rank'] = value
        return repr(stuff)
    else:
        return str(set[0].data)


def handler(req):
    global request, trace
    req.content_type = "text/plain"
    if (trace):
        req.write("Searching...\n")
    request = req

    store = util.FieldStorage(req)
    qstr = store['query'].value

    rss = search(qstr)

    for rs in rss.keys():
        if (len(rss[rs]) > 0):
            req.write("\n     SERVER: %s\n\n" % (rs))
            try:
                set = rss[rs]
                req.write(render(set[0]))
            except zoom.ConnectionError, err:
                # record not cached, connection timed out
                connections[rs].queryCache = {}
                new = search(qstr, [rs])
                req.write(render(new[0]))
            req.write("\n")
    return apache.OK
