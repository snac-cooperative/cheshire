
from configParser import C3Object
from baseObjects import Transformer
import os.path, time, utils, types
from document import StringDocument
from c3errors import ConfigFileException

from Ft.Xml.Xslt.Processor import Processor
from Ft.Xml import InputSource
from Ft.Xml.Domlette import ConvertDocument
from PyZ3950 import z3950

class XsltTransformer(Transformer):

    def __init__(self, config, parent):
        C3Object.__init__(self, config, parent)
        xfrPath = self.get_path(None, "xsltPath")
        dfp = self.get_path(None, "defaultPath")
        path = os.path.join(dfp, xfrPath)
        xfr = InputSource.DefaultFactory.fromStream(file(path), "file://" + dfp)
        processor = Processor()
        processor.appendStylesheet(xfr)
        self.processor = processor

    def process_record(self, session, rec):
        dom = rec.get_dom()
        dfp = self.get_path(None, "defaultPath")
        result = self.processor.runNode(dom, u'')
        return StringDocument(result)

class GRS1Transformer(Transformer):

    def initState(self):
        self.top = None
        self.nodeStack = []

    def startElement(self, name, attribs):
        node = z3950.TaggedElement()
        node.tagType = 3
        node.tagValue = ('string', name)
        node.content = ('subtree', [])

        for a in attribs:
            # Elements with Metadata
            anode = z3950.TaggedElement()
            md = z3950.ElementMetaData()
            anode.tagType = 3
            anode.tagValue = ('string', a)
            md.message = 'attribute'
            anode.metaData = md
            anode.content = ('octets', attribs[a])
            node.content[1].append(anode)

        if (self.nodeStack):
            self.nodeStack[-1].content[1].append(node)
        else:
            self.top = node
        self.nodeStack.append(node)

        
    def endElement(self, foo):
        if (self.nodeStack[-1].content[1] == []):
            self.nodeStack[-1].content = ('elementEmpty', None)
        self.nodeStack.pop()

    def characters(self, text, zero, length):
        if (self.nodeStack):
            if (text.isspace()):
                text = " "
            node = z3950.TaggedElement()
            node.tagType = 2
            node.tagValue = ('numeric', 19)
            node.content = ('octets', text)
            self.nodeStack[-1].content[1].append(node)


    def process_record(self, session, rec):
        self.initState()
        rec.saxify(self)
        return StringDocument(self.top)


class ChainTransformer(Transformer):
    # Transform a record by passing it sequentially to multiple transformers

    def __init__(self, config, parent):
        # XXX implement
        pass

    def process_record(self, session, rec):
        # XXX Implement
        pass

class ClusterExtractionTransformer(Transformer):


    def __init__(self, config, parent):
        self.keyMap = []
        self.maps = []
        Transformer.__init__(self, config, parent)

        for m in range(len(self.maps)):
            for t in range(len(self.maps[m][2])):
                o = self.get_object(None, self.maps[m][2][t][1])
                if (o <> None):
                    self.maps[m][2][t][1] = o
                else:
                    raise(ConfigFileException("Unknown object %s" % (self.maps[m][2][t][1])))
        for t in range(len(self.keyMap[2])):
            o = self.get_object(None, self.keyMap[2][t][1])
            if (o <> None):
                self.keyMap[2][t][1] = o
            else:
                raise(ConfigFileException("Unknown object %s" % (self.keyMap[2][t][1])))
            
    def _mergeHash(self, a, b):
        if not a:
            return b
        if not b:
            return a
        for k in b.keys():
            a[k] = a.get(k, 0) + b[k] 
        return a

    def _mergeData(self, a, b):
        if not a:
            return b
        return a + b


    def _handleConfigNode(self, node):
        if (node.localName == "cluster"):
            maps = []
            for child in node.childNodes:
                if (child.nodeType == utils.elementType and child.localName == "map"):
                    t = child.getAttribute('type')
                    map = []
                    for xpchild in child.childNodes:
                        if (xpchild.nodeType == utils.elementType and xpchild.localName == "xpath"):
                            map.append(utils.flattenTexts(xpchild))
                        elif (xpchild.nodeType == utils.elementType and xpchild.localName == "process"):
                            p = []
                            for child2 in xpchild.childNodes:
                                if child2.nodeType == utils.elementType and child2.localName == "object":
                                    p.append([child2.getAttribute('type'), child2.getAttribute('ref')])
                            map.append(p)
                    vxp = utils.verifyXPaths([map[0]])
                    if (len(map) < 3):
                        # default ExactExtracter
                        map.append([['extracter', 'ExactExtracter']])
                    if (t == u'key'):
                        self.keyMap = [vxp[0], map[1], map[2]]
                    else:
                        maps.append([vxp[0], map[1], map[2]])
            self.maps = maps

    def _processChain(self, session, data, process):
        (otype, obj) = process[0]
        new = {}
        if (otype == 'extracter'):
            # list input
            for d in data:
                if (type(d) == types.ListType):
                    # SAX event
                    new = self._mergeHash(new, obj.process_eventList(session, d))
                elif (type(d) in types.StringTypes):
                    # Attribute content
                    new = self._mergeHash(new, obj.process_string(session, d))
                else:
                    # DOM nodes
                    new = self._mergeHash(new, obj.process_node(session, d))
        elif (otype == 'normaliser'):
            fn = obj.process_string
            for d in data.keys():
                nk = fn(session, d)
                new[nk] = self._mergeData(data.get(nk, None), data[d])
        else:
            raise(ConfigFileException("Unknown object type: %s" % (otype)))

        if (len(process) == 1):
            return new
        else:
            return self._processChain(session, new, process[1:])

    def begin_indexing(self, session):
        path = self.get_path(session, "tempPath")
        if (not os.path.isabs(path)):
            dfp = self.get_path(session, "defaultPath")
            path = os.path.join(dfp, path)       
        self.fileHandle = file(path, "w")

    def commit_indexing(self, session):
        self.fileHandle.close()
                           
    def process_record(self, session, rec):
        # Extract cluster information, append to temp file
        # Step through .maps keys
        raw = rec.process_xpath(self.keyMap[0])
        keyData = self._processChain(session, raw, self.keyMap[2])
        fieldData = []
        for map in self.maps:
            raw = rec.process_xpath(map[0])
            fd = self._processChain(session, raw, map[2])
            for f in fd.keys():
                fieldData.append("%s\x00%s\x00" % (map[1], f))
        d = "".join(fieldData)
        for k in keyData.keys():
            self.fileHandle.write("%s\x00%s\n" % (k, d))
            self.fileHandle.flush()
                                             

class XmlRecordStoreTransformer(Transformer):
    # Transform a record, return 'string' to dump to database.
    # (String might be a struct in other implementations)

    def __init__(self, config, parent):
        pass

    def process_record(self, session, rec):

        vars = {'id' : rec.id, 'baseUri': rec.baseUri, 'schema' : rec.schema,
                'schemaType' : rec.schemaType, 'status' : rec.status,
                'size': rec.size}
        if session == None:
            vars['user'] = 'admin'
        else:
            vars['user'] = session.user.username


        vars['now'] = time.strftime("%Y-%m-%d %H:%M:%S")

        if (rec.recordStore <> None and rec.id <> None):
            history = rec.history
            histlist = []

            if (history):
                history.append((vars['user'], vars['now'], 'modified'))
                for h in history:
                    histlist.append('<c3:modification type="%s"><c3:date>%s</c3:date><c3:agent>%s</c3:agent></c3:modification>' % (h[2], h[1], h[0]))
                histlist.append('<c3:modification type="modify"><c3:date>%(now)s</c3:date><c3:agent>%(user)s</c3:agent></c3:modification>' % (vars))
                histtxt = "\n".join(histlist)
            else:
                histtxt = '<c3:modification type="create"><c3:date>%(now)s</c3:date><c3:agent>%(user)s</c3:agent></c3:modification>' % (vars)
            
            rightslist = []
            for r in rec.rights:
                rightslist.append('<c3:%(userType)s role="%(role)s">%(user)</c3:%(userType)s>' % ({'userType' : r[1], 'role' : r[2], 'user': r[0]}))
            rightstxt = '\n'.join(rightslist)

            saxlist = rec.get_sax()
            sax = '\n'.join(saxlist)

        else:
            histtxt = '<c3:modification type="create"><c3:date>%(now)s</c3:date><c3:agent>%(user)s</c3:agent></c3:modification>' % (vars)
            rightstxt = '<c3:agent role="editor">%(user)s</c3:agent>' % (vars)
            sax = ""


        vars['rights'] = rightstxt
        vars['history'] = histtxt
        vars['sax'] = sax
            
        xml = """<c3:record xmlns:c3="http://www.cheshire3.org/schemas/record/1.0/">
        <c3:id>%(id)s</c3:id>
        <c3:status>%(status)s</c3:status>
        <c3:baseUri>%(baseUri)s</c3:baseUri>
        <c3:schema>%(schema)s</c3:schema>
        <c3:schemaType>%(schemaType)s</c3:schemaType>
        <c3:size>%(size)d</c3:size>
        <c3:technicalRights>
          %(rights)s
        </c3:technicalRights>
        <c3:history>
          %(history)s
        </c3:history>
        <c3:saxEvents>%(sax)s</c3:saxEvents>
        </c3:record>
        """ % (vars)

        return StringDocument(xml)
              
              

