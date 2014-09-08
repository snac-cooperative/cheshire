# Map incoming request stuff into local objects
# Eg dc.title -> l5r-idx-cardname object

from baseObjects import ProtocolMap, Extracter
from configParser import C3Object
from c3errors import ConfigFileException
from utils import elementType, textType, flattenTexts
import sys, os

class CQLProtocolMap(ProtocolMap):
    protocol = "CQL"
    version = "1.1"

    protocolNamespaces = {
        'srw' : 'http://www.loc.gov/zing/srw/',
        'xcql' : 'http://www.loc.gov/zing/srw/xcql/',
        'xsort' : 'http://www.loc.gov/zing/srw/xsortkeys/',
        'diag' : 'http://www.loc.gov/zing/srw/diagnostic/'
        }
 
    recordNamespacesFull = {
        'xpath' : 'info:srw/schema/1/xpath-1.0',
        'diag' : 'info:srw/schema/1/diagnostic-v1.1',
        'zeerex' : 'http://explain.z3950.org/dtd/2.0/',
        'dc' : 'info:srw/schema/1/dc-v1.1',
        'mods' : 'info:srw/schema/1/mods-v3.0',
        'onix' : 'info:srw/schema/1/onix-v2.0',
        'marcxml' : 'info:srw/schema/1/marcxml-v1.1',
        'ead' : 'info:srw/schema/1/ead-2002',
        'ccg' : 'http://srw.o-r-g.org/schemas/ccg/1.0/',
        'marcsgml' : 'http://srw.o-r-g.org/schemas/marcsgml/12.0/',
        'metar' : 'http://srw.o-r-g.org/schemas/metar/1.0/',
        'unesco' : 'http://srw.o-r-g.org/schemas/unesco/1.0/',
        'zthes' : 'http://zthes.z3950.org/xml/zthes-05.dtd',
        'rec' : 'info:srw/schema/2/rec-1.0'
        }

    recordNamespaces = {
        'xpath' : 'info:srw/schema/1/xpath-1.0',
        'diag' : 'info:srw/schema/1/diagnostic-v1.1',
        'zeerex' : 'http://explain.z3950.org/dtd/2.0/'
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

    c3Namespace = "http://www.cheshire3.org/schemas/explain/"

    prefixes = {}
    indexHash = {}
    transformerHash = {}
    
    def __init__(self, node, parent):
        C3Object.__init__(self, node, parent)

        p = self.get_path(None, 'zeerexPath')
        if (p == None):
            raise(ConfigFileException('ZeerexPath not specified for CQLConfig.'))
        else:
            if ( not os.path.isabs(p)):
                dfp = self.get_path(None, 'defaultPath')
                p = os.path.join(dfp, p)

            dom = self.getDomFromFile(p)

        indexHash = {'cql' : {}, 'srw' : {}}
        contextSetNamespaces = self.contextSetNamespaces
        for c in dom.childNodes:
            if c.nodeType == elementType:
                self._walkZeeRex(c)

    def resolvePrefix(self, name):
        if (self.prefixes.has_key(name)):
            return self.prefixes[name]
        else:
            # XXX: Should step up to other config objects?
            raise(ConfigFileException("Unknown prefix: %s" % (name)))

    def resolveIndex(self, session, query):

        target = query
        while (target.parent):
            target = target.parent
        target.config = self
        query.index.resolvePrefix()
        uri = query.index.prefixURI
        name = query.index.value
        rel = query.relation.value
        relMods = query.relation.modifiers

        # XXX:  Awful!
        # Check relevance, check stem, check str/word, check relation,
        # Check index

        relv = stem = 0
        rms = []
        for r in relMods:
            # XXX Check context set
            if (r.type.value == 'relevant'):
                relv = 1
            elif (r.type.value == 'stem'):
                stem = 1
            else:
                rms.append(r.type.value)

        idx = None
        if (relv):
            idx = self.indexHash.get((uri, name, ('relationModifier', 'relevant')), None)
        if (not idx and stem):
            idx = self.indexHash.get((uri, name, ('relationModifier', 'stem')), None)
        if (not idx and rms):
            for rm in rms:
                idx = self.indexHash.get((uri, name, ('relationModifier', rm)), None)
                if (idx):
                    break
        if (not idx):
            idx = self.indexHash.get((uri, name, ('relation', rel)), None)
        if (not idx):
            idx = self.indexHash.get((uri,name), None)
        return idx


    def _walkZeeRex(self, node):

        if node.localName in ['databaseInfo', 'metaInfo']:
            # Ignore
            return
        elif node.localName == 'serverInfo':
            self.version = node.getAttribute('version')
            for c in node.childNodes:
                self._walkZeeRex(c)
        elif node.localName == 'database':
            self.databaseUrl = str(flattenTexts(node))
        elif node.localName == 'host':
            self.host = str(flattenTexts(node))
        elif node.localName == 'port':
            self.port = int(flattenTexts(node))
        elif node.localName == 'schema':
            id = node.getAttribute('identifier')
            name = node.getAttribute('name')
            xsl = node.getAttributeNS(self.c3Namespace, 'transformer')
            if (xsl):
                txr = self.get_object(None, xsl)
                if (txr == None):
                    raise ConfigFileException("No transformer to map to for %s" % (xsl))
                self.transformerHash[id] = txr
            self.recordNamespaces[name] = id
        elif node.localName == 'set':
            name = node.getAttribute('name')
            uri = node.getAttribute('identifier')
            if (self.prefixes.has_key(name) and uri <> self.prefixes[name]):
                raise(ConfigFileException('Multiple URIs bound to same short name: %s -> %s' % (name, uri)))
            self.prefixes[str(name)] = str(uri)
        elif node.localName == 'index':
            # Process indexes
            idxName = node.getAttributeNS(self.c3Namespace, 'index')
            indexObject = self.get_object(None, idxName)
            if indexObject == None:
                raise(ConfigFileException("No Index to map to for %s" % (idxName)))
            maps = []

            for c in node.childNodes:
                if (c.nodeType == elementType and c.localName == 'map'):
                    maps.append(self._walkZeeRex(c))
            for m in maps:
                self.indexHash[m] = indexObject
            # Need to generate all relations and modifiers
            for c in node.childNodes:
                if (c.nodeType == elementType and c.localName == 'configInfo'):
                    for c2 in c.childNodes:
                        if (c2.nodeType == elementType and c2.localName == 'supports'):
                            idxName2 = c2.getAttributeNS(self.c3Namespace, 'index')
                            if (not idxName2):
                                indexObject2 = indexObject
                            else:
                                indexObject2 = self.get_object(None, idxName2)
                                if indexObject2 == None:
                                    raise(ConfigFileException("No Index to map to for %s" % (idxName2)))
                            st = str(c2.getAttribute('type'))
                            val = str(flattenTexts(c2))
                            for m in maps:
                                self.indexHash[(m[0], m[1], (st, val))] = indexObject2

        elif (node.localName == 'map'):
            for c in node.childNodes:
                if (c.nodeType == elementType and c.localName == 'name'):
                    short = c.getAttribute('set')
                    index = flattenTexts(c)
                    uri = self.resolvePrefix(short)
                    if (not uri):
                        raise(ConfigFileException("No mapping for %s in Zeerex" % (short)))
                    return (str(uri), str(index))
        elif (node.localName == 'default'):
            dtype = node.getAttribute('type')
            pname = "default" + dtype[0].capitalize() + dtype[1:]
            data = flattenTexts(node)
            if (data.isdigit()):
                data = int(data)
            elif data == 'false':
                data = 0
            elif data == 'true':
                data = 1
            setattr(self, pname, data)
        elif (node.localName =='setting'):
            dtype = node.getAttribute('type')
            data = flattenTexts(node)
            if (data.isdigit()):
                data = int(data)
            elif data == 'false':
                data = 0
            elif data == 'true':
                data = 1
            setattr(self, dtype, data)
        else:
            for c in node.childNodes:
                if c.nodeType == elementType:
                    self._walkZeeRex(c)
                    
