
import os, sys, SRWHandlers, SRWState, time
from xml.dom.minidom import parse, Node
from PyZ3950 import zoom

configs = []
configURLHash = {}

# (Shamelessly copied from CQLParser.XCQLParser)
def firstChildElement(elem):
    """ Find first child which is an Element """
    for c in elem.childNodes:
        if c.nodeType == Node.ELEMENT_NODE:
            return c
    return None

def firstChildData(elem):
    """ Find first child which is Data """
    for c in elem.childNodes:
        if c.nodeType == Node.TEXT_NODE:
            return c
    return None


class SRWConfig:
    """ Configuration holder per Z Database """

    # Default Namespaces etc if built by hand
    protocolNamespaces = {
        'srw' : 'http://www.loc.gov/zing/srw/',
        'xcql' : 'http://www.loc.gov/zing/srw/xcql/',
        'xsort' : 'http://www.loc.gov/zing/srw/xsortkeys/',
        'diag' : 'http://www.loc.gov/zing/srw/diagnostic/'
        }

    recordNamespaces = {
        'dc' : 'info:srw/schema/1/dc-v1.1',
        'diag' : 'info:srw/schema/1/diagnostic-v1.1',
        'mods' : 'info:srw/schema/1/mods-v3.0',
        'onix' : 'info:srw/schema/1/onix-v2.0',
        'marcxml' : 'info:srw/schema/1/marcxml-v1.1',
        'ead' : 'info:srw/schema/1/ead-2002',
        'ccg' : 'http://srw.o-r-g.org/schemas/ccg/1.0/',
        'marcsgml' : 'http://srw.o-r-g.org/schemas/marcsgml/12.0/',
        'metar' : 'http://srw.o-r-g.org/schemas/metar/1.0/',
        'unesco' : 'http://srw.o-r-g.org/schemas/unesco/1.0/',
        'zthes' : 'http://zthes.z3950.org/xml/zthes-05.dtd',
        'zeerex' : 'http://explain.z3950.org/dtd/2.0/',
        'rec' : 'info:srw/schema/2/rec-1.0',
        'xpath' : 'info:srw/schema/1/xpath-1.0'
        }

    contextSetNamespaces = {
        'cql' : 'info:srw/cql-context-set/1/cql-v1.1',
        'srw' : 'info:srw/cql-context-set/1/cql-v1.1',
        'dc' : 'info:srw/cql-context-set/1/dc-v1.1',
        'bath' : 'http://www.loc.gov/zing/cql/context-sets/bath/v1.1/',
        'zthes' : 'http://zthes.z3950.org/cql/1.0/',
        'ccg' : 'http://srw.cheshire3.org/contextSets/ccg/1.1/',
        'ccg_l5r' : 'http://srw.cheshire3.org/contextSets/ccg/l5r/1.0/',
        'rec' : 'info:srw/cql-context-set/2/rec-1.0',
        'net' : 'info:srw/cql-context-set/2/net-1.0'
        }

    profileNamespaces = {
        'bath' : 'http://zing.z3950.org/srw/bath/2.0/',
        'zthes' : ' http://zthes.z3950.org/srw/0.5',
        'ccg' : 'http://srw.cheshire3.org/profiles/ccg/1.0/',
        'srw' : 'info:srw/profiles/1/base-profile-v1.1'
        }

    extensionNamespaces = {
        'schemaNegotiation' : 'info:srw/extension/2/schemaNegotiation-1.0',
        'authenticationToken' : 'info:srw/extension/2/auth-1.0'
        }

    attrSetHash = {
        'BIB1' : '1.2.840.10003.3.1',
        'EXP1' : '1.2.840.10003.3.2',
        'UTIL' : "1.2.840.10003.3.11",
        'XD' : "1.2.840.10003.3.12",
        'zthes' : '1.2.840.10003.3.13',
        'BIB2' : '1.2.840.10003.3.18',
        'ZeeRex' : '1.2.840.10003.3.19'
        }

    host = "localhost"
    port = 210
    databaseName = "default"
    url = ""
    indexHash = {}
    defaultNumberOfRecords = 1
    defaultNumberOfTerms = 20
    defaultContextSet = "dc"
    defaultIndex = "title"
    defaultRelation = "any"
    defaultSortSchema = 'dc'
    defaultRetrieveSchema = 'dc'
    defaultSortCaseSensitive = 0
    defaultSortAscending = 1
    defaultResponsePosition = 1
    maximumRecords = 50
    SRWZeeRex = ""

    # Stuff to configure from server config
    conn = None
    logLevel = 3
    logfile = None
    convertFromSGML = 1
    databaseSchema = ""
    elementSetName = "F"
    preferredRecordSyntax = "XML"
    recordSchemaHash = {} 
    maxTTL = 600
    useWordIndexes = 1
    xslUrl = ""

    def __init__(self, zfile=None, srwfile=None):
        """ Create a config from the given ZeeRex files """

        # We don't actually want it to do anything, just create an object
        if srwfile == None and zfile == None:
            return

        # Check if files exist and are readable
        if not os.path.exists(srwfile):
            raise ValueError
        elif not os.path.exists(zfile):
            raise ValueError

        self.SRWZeeRex = srwfile

        indexHash = {'cql' : {}, 'srw' : {}}
        contextSetNamespaces = self.contextSetNamespaces

        dom1 = parse(zfile)
        # Extract host/port/database
        self.host = firstChildData(dom1.getElementsByTagName('host')[0]).data
        self.port = int(firstChildData(dom1.getElementsByTagName('port')[0]).data)
        self.databaseName = firstChildData(dom1.getElementsByTagName('database')[0]).data

        # Extract supported Z indexes
        sets = dom1.getElementsByTagName('set')
        for s in sets:
            self.attrSetHash[s.getAttribute('name')] = s.getAttribute('identifier')

        indexListZ = dom1.getElementsByTagName('index')
        indexHashZ = {}
        for idx in indexListZ:
            id = idx.getAttribute('id')
            # Extract attributes
            maps = idx.getElementsByTagName('map')
            if not maps:
                raise ValueError, "Z39.50 description does not contain any way to access index %s" % (id)
            else:
                attrList = []
                attrs = []
                mapIdx = 0
                while not attrs and mapIdx < len(maps):
                    attrs = maps[mapIdx].getElementsByTagName('attr')
                    mapIdx += 1
                    
                for a in attrs:
                    attrSet = a.getAttribute('set')
                    if (not attrSet):
                        attrSet = 'BIB1'
                    try:
                        attrSet = self.attrSetHash[attrSet]
                    except:
                        print "Cannot generate attrSet for %s" % (attrSet)
                        continue
                    type = a.getAttribute('type')
                    value = firstChildData(a).data
                    attrList.append([attrSet, type, value])
                if not attrList:
                    print "WARNING:  " + id + " has no attribute combination"
                indexHashZ[id] = attrList

        dom2 = parse(srwfile)
        # Extract url for this database
        self.url = firstChildData(dom2.getElementsByTagName('database')[0]).data
        # Extract indexSet list
        setList = dom2.getElementsByTagName('set')
        for idxSet in setList:
            setname = idxSet.getAttribute('name')
            # Case Insensitive
            setname = setname.lower()
            contextSetNamespaces[setname] = idxSet.getAttribute('identifier')
            indexHash[setname] = {}
        self.contextSetNamespaces = contextSetNamespaces

        # Extract indexes and map to Z indexes
        indexListSRW = dom2.getElementsByTagName('index')
        for idx in indexListSRW:
            id = idx.getAttribute('id')
            names = idx.getElementsByTagName('name')
            for n in names:
                idxSet = n.getAttribute('set')
                idxName = firstChildData(n).data
                # Case insensitive
                idxName = idxName.lower()
                try:
                    indexHash[idxSet][idxName] = indexHashZ[id]
                except:
                    # If there's no index to map to, just ignore?
                    print "WARNING:  No map for %s.%s ?" % (idxSet, idxName)
                    pass
        self.indexHash = indexHash

        # Extract supported record Schema Namespaces
        schemaList = dom2.getElementsByTagName('schema')
        # Must support DC, diag and ZeeRex and xpath
        schemaIds = {
            'dc' : 'http://www.loc.gov/zing/srw/dcschema/v1.0/',
            'diag' : 'http://www.loc.gov/zing/srw/v1.0/diagnostic/',
            'zeerex' : 'http://explain.z3950.org/dtd/2.0/',
            'xpath' : 'info:srw/schema/1/xpath-1.0'
            }
        for schema in schemaList:
            name = schema.getAttribute('name')
            name = name.lower()
            schemaIds[name] = schema.getAttribute('identifier')
        self.recordNamespaces = schemaIds

        # Extract Defaults and Settings
        cfgkids = []
        for e in dom2.childNodes[0].childNodes:
            if e.nodeType == Node.ELEMENT_NODE and e.localName == "configInfo":
                cfgkids = e.childNodes
                break
        for cfg in cfgkids:
            if cfg.nodeType == Node.ELEMENT_NODE:
                name = cfg.localName
                dtype = cfg.getAttribute('type')
                if name == "default":
                    pname = "default" + dtype[0].capitalize() + dtype[1:]
                    data = firstChildData(cfg).data
                    if (data.isdigit()):
                        data = int(data)
                    elif data == "false":
                        data = 0
                    elif data == "true":
                        data = 1
                    setattr(self, pname, data)
                elif name == "setting" and dtype == "maximumRecords":
                    self.maximumRecords = int(firstChildData(cfg).data)
    
    def log(self, msg):
        now = time.ctime()
        txt = "[%s] %s:  %s" % (self.databaseName, now, msg)
        if (self.logfile):
            self.logfile.write(txt)
        else:
            print txt

    def zconnect(self):
        """ Reconnect to the Z server """
        global resultSets
        c = zoom.Connection(self.host, self.port)
        c.databaseName = self.databaseName
        c.preferredRecordSyntax = self.preferredRecordSyntax
        c.elementSetName = self.elementSetName
        self.conn = c
        # Trash any resultsets
        # XXX SRWState.expire_rs_from_config(self)

        if self.logLevel > 1:
            self.log("Connecting to Z Server: %s:%d/%s" % (self.host, self.port, self.databaseName))

    def expireResultSet(self, rset):
        # Make sure connection is active
        try:
            rset.delete()
        except zoom.error, err:
            return 0
        if self.logLevel > 2:
            print "RESULTSET: Expiring " + name
        return 1

    def resolvePrefix(self, p):
        if (self.contextSetNamespaces.has_key(p)):
            return self.contextSetNamespaces[p]
        else:
            return None

    
def buildConfig(object, node):
    "Build the configurations from global config file"
    global configs
    for c in node.childNodes:
        if c.nodeType == Node.ELEMENT_NODE:
            if c.localName == 'logLevel':
                object.logLevel = int(firstChildData(c).data)
            elif c.localName == 'recordSyntax':
                object.recordSyntax = firstChildData(c).data
            elif c.localName == 'elementSetName':
                object.elementSetName = firstChildData(c).data
            elif c.localName == 'schema':
                object.databaseSchema = firstChildData(c).data
                if (object.recordNamespaces.has_key(object.databaseSchema)):
                    object.databaseSchema = object.recordNamespaces[object.databaseSchema]

            elif c.localName == 'convertFromSGML':
                object.convertFromSGML = int(firstChildData(c).data)
            elif c.localName == 'xslUrl':
                object.xslUrl = firstChildData(c).data
            elif c.localName == 'database':
                # Create a new config and recurse
                files = c.getElementsByTagName('file')
                for f in files:
                    pcl = f.getAttribute('protocol')
                    if pcl == 'srw':
                        srwfile = str(firstChildData(f).data)
                    elif pcl == 'z39.50':
                        zfile = str(firstChildData(f).data)
                    else:
                        print "Unknown Protocol: " + str(firstChildData(f).data)
                        sys.exit()
                cfg = SRWConfig(zfile, srwfile)
                configs.append(cfg)
                buildConfig(cfg, c)
            elif c.localName == 'schemaHandlers':
                handlers = c.getElementsByTagName('handler')
                schemaHash = {}
                for h in handlers:
                    schema = h.getAttribute('schema')
                    if (object.recordNamespaces.has_key(schema)):
                        schema = object.recordNamespaces[schema]

                    hash = {}
                    # Accept elementSetName, recordSyntax, function
                    tags = h.getElementsByTagName('elementSetName')
                    if len(tags) > 0:
                        hash['elementSetName'] = firstChildData(tags[0]).data
                    tags = h.getElementsByTagName('preferredRecordSyntax')
                    if len(tags) > 0:
                        hash['preferredRecordSyntax'] = firstChildData(tags[0]).data
                    tags = h.getElementsByTagName('function')
                    if len(tags) > 0:
                        data = firstChildData(tags[0]).data
                        if data == 'MARCHandler':
                            hash['handler'] = SRWHandlers.MARCHandler
                        elif data == 'DCGRS1Handler':
                            hash['handler'] = SRWHandlers.DCGRS1Handler
                        elif data == 'Unesco2ZthesHandler':
                            hash['handler'] = SRWHandlers.Unesco2ZthesHandler
                        else:
                            print "Unknown Handler: " + data
                            sys.exit()
                    schemaHash[schema] = hash
                object.recordSchemaHash = schemaHash
            else:
                # Dunno what this is, just ignore
                pass
