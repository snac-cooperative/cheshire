
from baseObjects import Record
from c3errors import C3Exception
import types, utils, os
from Ft.Xml.Domlette import implementation

class SaxToDomHandler:
    nodeStack = []
    document = None
    currText = ""

    def initState(self):
        self.nodeStack = []
        self.document = implementation.createDocument(None, 'foo', None)
        self.top = None
        
    def startElement(self, name, attribs):
        elem = self.document.createElementNS(None,name)
        for a in attribs:
            elem.setAttributeNS(None,a,attribs[a])
        if (self.nodeStack):
            self.nodeStack[-1].appendChild(elem)
        else:
            self.document.appendChild(elem)
        self.nodeStack.append(elem)
        
    def endElement(self, foo):
        self.nodeStack.pop()

    def characters(self, text, zero, length):
        if (self.nodeStack):
            if (text.isspace()):
                text = " "
            d = self.document.createTextNode(text)
            self.nodeStack[-1].appendChild(d)

    def getRootNode(self):
        return self.document

s2dhandler = SaxToDomHandler()

class SaxToXmlHandler:
    xml = []

    def initState(self):
        self.xml = []
        
    def startElement(self, name, attribs):
        attrs = []
        for a in attribs:
            attrs.append('%s="%s"' % (a, attribs[a]))
        attribtxt = ' '.join(attrs)
        if (attribtxt):
            attribtxt = " " + attribtxt
        
        self.xml.append("<%s%s>" % (name, attribtxt))
        
    def endElement(self, name):
        self.xml.append("</%s>" % (name))

    def characters(self, text, zero, length):
        self.xml.append(text)

    def getDocument(self):
        return ''.join(self.xml)


s2xhandler = SaxToXmlHandler()


class NumericPredicateException(C3Exception):
    pass

class DomRecord(Record):

    def __init__(self, domNode, xml="", docid=None):
        self.dom = domNode
        self.xml = xml
        self.id = docid

    def get_dom(self):
        return self.dom

    def get_xml(self):
        if (self.xml):
            return self.xml
        else:
            # Form XML string from DOM and cache
            pass
        
    def get_sax(self):
        if (self.sax):
            return self.sax
        else:
            # Turn DOM into SAX and cache
            pass


class SaxRecord(Record):

    def __init__(self, saxList, xml="", docid=None, recordSize=0):
        self.sax = saxList
        self.id = docid
        self.xml = xml
        self.history = []
        self.rights = []
        self.elementHash = {}
        self.size = recordSize


    def extract_xpath(self, session, path):
        # On the fly extraction
        c = utils.verifyXPaths([path])
        if (c and c[0][1]):
            return self.process_xpath(c[0])
        elif c[0][1] == None:
            # use DOM? 
            return []
        else:
            return []

    def process_xpath(self, xpTuple):

        xp = xpTuple[1]
        try:
            data = []
            # raise(NotImplementedError)

            if (xp[-1][0] == 'attribute'):
                attrName = xp[-1][1]
                if (len(xp) == 1):
                    # Extracting all occs of attribute anywhere!?
                    # Check predicates... (only support one numeric predicate)
                    if (len(xp[0][2]) == 1 and type(xp[0][2][0]) == types.FloatType):
                        nth = int(xp[0][2][0])
                    elif (len(xp[0][2])):
                        # Non index or multiple predicates??
                        raise(NotImplementedError)
                    else:
                        nth = 0

                    currn = 0
                    for l in self.sax:
                        if (l[:5] == "#elem"):
                            (name, attrs) = self._convert_elem(l)
                            if (attrs.has_key(attrName)):
                                currn += 1
                                content = attrs[attrName]
                                if (currn == nth):
                                    data.append(content)
                                    break
                                elif (not nth):
                                    data.append(content)
                                
                else:
                    elemName = xp[-2][1]
                    if (elemName == "*"):
                        # Let DOM code handle this monstrosity :P
                        raise(NotImplementedError)
                        
                    if (self.elementHash.has_key(elemName)):
                        elemLines = self.elementHash[elemName]
                        for e in elemLines:
                            line = self.sax[e[0]]
                            (name, attrs) = self._convert_elem(line)
                            if (attrName <> '*' and attrs.has_key(attrName)):
                                content = attrs[attrName]
                                # Now check rest of path
                                match = self._checkSaxXPathLine(xp[:-1], e[0])
                                if (match):
                                    data.append(content)
                            elif (attrName == '*'):
                                # All attributes' values
                                match = self._checkSaxXPathLine(xp[:-1], e[0])
                                if (match):
                                    for k in attrs.keys():
                                        data.append(attrs[k])
                                            
            elif(xp[-1][0] == 'child'):
                # Extracting element
                elemName = xp[-1][1]
                if (not self.elementHash.has_key(elemName)):
                    return []
                elemLines = self.elementHash[elemName]
                for e in elemLines:
                    match = self._checkSaxXPathLine(xp, e[0])
                    if (match):
                        # Return event chunk
                        l = self.sax[e[0]]
                        end = int(l[l.rfind(' ')+1:])
                        data.append(self.sax[e[0]:end+1])
                        
            else:
                # Unsupported final axis
                raise(NotImplementedError)

            return data
        except NotImplementedError:
            # Convert to DOM (slow) and reapply (slower)
            dom = self.get_dom()
            xp = xpTuple[0]
            try:
                return utils.evaluateXPath(xp, dom)
            except:
                print "Buggy Xpath!..."
                return []
        # Otherwise just fall over as we've hit a real bug


    def _checkSaxXPathLine(self, xp, line):
        # Check that event at line in record matches xpath up tree
        # Pass by reference, need a copy to pop! Looks like a hack...
        xpath = xp[:]

        while (xpath):
            posn = len(xpath)
            node = xpath.pop()
            if (line == -1):
                if (node <> "/"):
                    return 0
            else:
                elem = self.sax[line]
                (name, attrs) = self._convert_elem(elem)
                match = self._checkSaxXPathNode(node, name, attrs, line, posn)
                if not match:
                    return 0
                start = elem.rfind("}") + 2
                end = elem.find(" ", start)
                line = int(elem[start:end])
        return 1


    def _checkSaxXPathNode(self, step, name, attrs, line, posn):
        if (step[1] <> name and step[1] <> '*'):
            return 0
        elif (step[0] <> 'child'):
            # Unsupported axis
            raise(NotImplementedError)
        elif (step[2]):
            # Check predicates
            predPosn = 0
            for pred in (step[2]):
                predPosn += 1
                m = self._checkSaxXPathPredicate(pred, name, attrs, line, posn, predPosn)
                if (not m):
                    return 0
        return 1

    def _checkSaxXPathPredicate(self, pred, name, attrs, line, posn, predPosn):

        if (type(pred) <> types.ListType):
            # Numeric Predicate. (eg /foo/bar[1])
            if (predPosn <> 1):
                # Can't do numeric predicate on already predicated nodeset
                # eg:  text[@type='main'][2]
                raise(NotImplementedError)

            if (posn == 1):
                # First position in relative path.
                # Check against position in elementHash
                if (self.elementHash.has_key(name)):
                    all = self.elementHash[name]
                    p = int(pred)
                    if (len(all) < p):
                        return 0
                    return all[int(pred)-1][0] == line
                return 0
            else:
                # Not first position, so it applies to parent elem
                # Which we record during parsing
                elem = self.sax[line]
                end = elem.rfind("}") + 2
                start = elem.find(' ', end) + 1
                end = elem.find(' ', start)
                npred = float(elem[start:end])
                return npred == pred

        elif (pred[1] in ['and', 'or']):
            # Attribute combinations
            left = self._checkSaxXPathPredicate(pred[0], name, attrs, line, posn, predPosn)
            right = self._checkSaxXPathPredicate(pred[2], name, attrs, line, posn, predPosn)
            if (pred[1] == 'and' and left and right):
                return 1
            elif (pred[1] == 'or' and (left or right)):
                return 1
            return 0
        elif (pred[0] == 'attribute'):
            # Attribute exists test
            if (not attrs.has_key(pred[1])):
                return 0
        elif (pred[1] in ['=', '!=', '<', '>', '<=', '>=']):
            # Single attribute
            f = self._checkSaxXPathAttr(pred, attrs)
            if (not f):
                return 0
        else:
            # No idea!!
            raise(NotImplementedError)
        return 1
        
    def _checkSaxXPathAttr(self, pred, attrs):
        if (not attrs.has_key(pred[0])):
            return 0
        rel = pred[1]

        # XPath -> Python mapping
        if (rel == "="):
            rel = "=="
        elif (rel == "!="):
            rel = "<>"
            
        if (type(pred[2]) == types.FloatType):
            attrValue = float(attrs[pred[0]])
            check = "%f %s %f" % (attrValue, rel, pred[2])
        elif (type(pred[2] == types.StringType)):
            check = "%s %s %r" % (repr(attrs[pred[0]]), rel, pred[2])
        else:
            # AFAIK check will always be string or float...
            # So this should never be reached
            raise(NotImplementedError)
        return eval(check)


    def _convert_elem(self, line):
        # Currently: name {attrs} parent npred end
        end = line.rfind("}")
        start = line.find("{")
        name = line[6:start-1]
        attrs = eval(line[start:end+1])
        return [name, attrs]
        

    def saxify(self, handler=None):
        if handler == None:
            handler = self

        for l in self.get_sax():
            line = l.strip()
            if line[1:5] == "elem":
                # String manipulation method
                (name, attrs) = self._convert_elem(line)
                handler.startElement(name, attrs)
            elif line[1:5] == "text":
                handler.characters(line[6:], 0, len(line)-6)
            elif line[1:4] == "end":
                end = line.rfind(' ')
                handler.endElement(line[5:end])
            elif line[1:5] == "hash":
                pass
            else:
                raise ValueError


    def get_dom(self):
        if (self.dom):
            return self.dom
        else:
            # Turn SAX into DOM and cache
            s2dhandler.initState()
            self.saxify(s2dhandler);
            dom = s2dhandler.getRootNode()
            self.document = dom
            return dom

    def get_xml(self):
        if (self.xml):
            return self.xml
        else:
            # Turn SAX into XML and cache
            s2xhandler.initState()
            self.saxify(s2xhandler)
            self.xml = s2xhandler.getDocument()
            return self.xml
            

    def get_sax(self):
        return self.sax
