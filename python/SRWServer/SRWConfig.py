
import os, sys, SRWHandlers, ChZoom, SRWState, ZCheshire
from xml.dom.minidom import parse, Node

oldpackagedir = "/usr/lib/python1.5/site-packages"
if os.path.exists(oldpackagedir):
    sys.path.insert(0, oldpackagedir)
import pg

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
        'srw' : 'http://www.loc.gov/zing/srw/v1.0/',
        'xcql' : 'http://www.loc.gov/zing/srw/v1.0/xcql/',
        'xSort' : 'http://www.loc.gov/zing/srw/v1.0/xsortkeys/',
        'diag' : 'http://www.loc.gov/zing/srw/v1.0/diagnostic/'
        }

    recordNamespaces = {
        'dc' : 'http://www.loc.gov/zing/srw/dcschema/v1.0/',
        'diag' : 'http://www.loc.gov/zing/srw/v1.0/diagnostic/',
        'mods' : 'http://www.loc.gov/mods/',
        'onix' : 'http://www.editeur.org/onix/ReferenceNames/',
        'marcxml' : 'http://www.loc.gov/marcxml/',
        'ead' : 'http://www.loc.gov/ead/',
        'ccg' : 'http://srw.o-r-g.org/schemas/ccg/1.0/',
        'marcsgml' : 'http://srw.o-r-g.org/schemas/marcsgml/12.0/',
        'metar' : 'http://srw.o-r-g.org/schemas/metar/1.0/',
        'unesco' : 'http://srw.o-r-g.org/schemas/unesco/1.0/',
        'oaimarc' : 'http://www.openarchives.org/OAI/1.1/oai_marc',
        'zthes' : 'http://zthes.z3950.org/xml/zthes-05.dtd',
        'zeerex' : 'http://explain.z3950.org/dtd/2.0/'
        }

    contextSetNamespaces = {
        'cql' : 'http://www.lov.gov/zing/cql/contextSets/cql/v1.1/',
        'srw' : 'http://www.lov.gov/zing/cql/contextSets/cql/v1.1/',
        'dc' : 'http://www.loc.gov/zing/cql/contextSets/dc/v1.1/',
        'bath' : 'http://www.loc.gov/zing/cql/bath-indexes/v1.0/',
        'zthes' : 'http://zthes.z3950.org/cql/1.0/',
        'ccg' : 'http://srw.cheshire3.org/contextSets/ccg/1.1/',
        'ccg_l5r' : 'http://srw.cheshire3.org/contextSets/ccg/l5r/1.1/',
        'rec' : 'http://srw.cheshire3.org/contextSets/rec/1.1/',
        'net' : 'http://srw.cheshire3.org/contextSets/net/1.1/'
        }

    host = "localhost"
    port = 210
    databaseName = "default"
    url = ""
    indexHash = {}
    defaultNumberOfRecords = 1
    defaultContextSet = "dc"
    defaultIndex = "title"
    defaultRelation = "any"
    defaultSortSchema = 'dc'
    defaultRetrieveSchema = 'dc'
    defaultSortCaseSensitive = 0
    defaultSortAscending = 1
    defaultResponsePosition = 1
    defaultNumberOfTerms = 20
    maximumRecords = 50
    SRWZeeRex = ""

    # Stuff to configure from server config
    conn = None
    logLevel = 3
    convertFromSGML = 1
    databaseSchema = ""
    elementSetName = "F"
    recordSyntax = "XML"
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

        indexHash = {}
        contextSetNamespaces = {
            'cql' : 'http://www.lov.gov/zing/cql/contextSets/cql/v1.1/',
            'srw' : 'http://www.lov.gov/zing/cql/contextSets/cql/v1.1/'
            }

        dom1 = parse(zfile)
        # Extract host/port/database
        self.host = firstChildData(dom1.getElementsByTagName('host')[0]).data
        self.port = int(firstChildData(dom1.getElementsByTagName('port')[0]).data)
        self.databaseName = firstChildData(dom1.getElementsByTagName('database')[0]).data

        # Extract supported Z indexes
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
                    attrSet = "BIB1"
                    try:
                        attrSet = a.getAttribute('set')
                    except:
                        pass
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
                    pass
        self.indexHash = indexHash

        # Extract supported record Schema Namespaces
        schemaList = dom2.getElementsByTagName('schema')
        # Must support DC, diag and ZeeRex
        schemaIds = {
            'dc' : 'http://www.loc.gov/zing/srw/dcschema/v1.0/',
            'diag' : 'http://www.loc.gov/zing/srw/v1.0/diagnostic/',
            'zeerex' : 'http://explain.z3950.org/dtd/2.0/'
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
    
    def zconnect(self):
        """ Reconnect to the Z server """
        global resultSets
        c = ChZoom.Connection(self.host, self.port)
        c.databaseName = self.databaseName
        c.recordSyntax = self.recordSyntax
        c.elementSetName = self.elementSetName
        self.conn = c
        # Trash any resultsets
        SRWState.expire_rs_from_config(self)

        if self.logLevel > 1:
            print "Connecting to Z Server: %s:%d/%s" % (self.host, self.port, self.databaseName)

    def expireResultSet(self, name):
        # Make sure connection is active
        self.conn.select()
        try:
            # Try a throw away search first
            ZCheshire.search('docid 1')
            ZCheshire.delete(name)
        except ZCheshire.error, err:
            return 0
        if self.logLevel > 2:
            print "RESULTSET: Expiring " + name
        return 1

    def pgConnect(self):
        """ Connect to PostGres database """
        c = pg.connect(self.database)
        self.conn = c

    
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
                    tags = h.getElementsByTagName('recordSyntax')
                    if len(tags) > 0:
                        hash['recordSyntax'] = firstChildData(tags[0]).data
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
