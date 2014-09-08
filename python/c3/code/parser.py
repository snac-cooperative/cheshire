
from configParser import C3Object
from baseObjects import Parser
from xml.dom.minidom import parseString as domParseString
from record import DomRecord, SaxRecord
from xml.sax import ContentHandler, make_parser, parseString as saxParseString, ErrorHandler, InputSource as SaxInput
from utils import flattenTexts, elementType
import re
import cStringIO as StringIO

class MinidomParser(Parser):

    def process_document(self, session, doc):
        xml = doc.get_raw()
        dom = domParseString(xml)
        rec = DomRecord(dom, xml)
        return rec


# #elem <name> <attrHash> parent predicate end
# #elemNS
# #end <name> <startLine>
# #endNS
# #text <text>
# #hash <element hash>


class SaxParser(Parser, ContentHandler):
    locked = 0
    currentText = []
    currentPath = []
    pathLines = []
    currentLine = -1
    recordSize = 0
    elementHash = {}

    def __init__(self, parent, config):
        C3Object.__init__(self, parent, config)
        self.parser = make_parser()
        self.inputSource = SaxInput()
        self.errorHandler = ErrorHandler()
        self.parser.setErrorHandler(self.errorHandler)
        self.parser.setContentHandler(self)

    def process_document(self, session, doc):

        if (self.locked):
            # Shouldn't be reusing across threads anyway!
            # XXX: Can we instantiate a new self ??
            raise(ValueError)
        self.locked = 1

        xml = doc.get_raw()
        self.inputSource.setByteStream(StringIO.StringIO(xml))
        self.currentText = []
        self.pathLines = []
        self.currentLine = -1
        self.elementHash = {}
        self.elementIndexes = []
        self.recordSize = 0
        
        try:
            self.parser.parse(self.inputSource)
        except:
            # Try again... sometimes odd things happen
            self.currentText = []
            self.pathLines = []
            self.currentLine = -1
            self.elementHash = {}
            self.elementIndexes = []
            self.recordSize = 0
            self.inputSource.setByteStream(StringIO.StringIO(xml))
            self.parser.parse(self.inputSource)
            
        self.currentText.append("#hash " + repr(self.elementHash))
        self.locked = 0
        rec = SaxRecord(self.currentText, xml, recordSize=self.recordSize)
        return rec

    # We want to fwd elems to NS elem handlers with default NS
    def startElement(self, name, attrs):
        self.currentLine += 1
        attrHash = {}
        for k in attrs.keys():
            attrHash[k] = attrs[k]
        self.pathLines.append(self.currentLine)
        if (len(self.pathLines) > 1):
            parent = self.pathLines[-2]
        else:
            parent = -1

        if (self.currentLine == 0):
            npred = 1
            self.elementIndexes = [{name: npred}]
        elif self.elementIndexes[-1].has_key(name):
            npred = self.elementIndexes[-1][name] + 1
            self.elementIndexes[-1][name] = npred
        else:
            npred = 1
            self.elementIndexes[-1][name] = 1
        self.elementIndexes.append({})
        ptxt = "#elem %s %s %d %d" % (name, repr(attrHash), parent, npred)
        self.currentText.append(ptxt)

    def endElement(self, name):
        self.currentLine += 1
        start = self.pathLines.pop()
        self.currentText.append("#end %s %d" % (name, start))
        self.currentText[start] = "%s %d" % (self.currentText[start], self.currentLine)
        self.elementIndexes.pop()
        if (self.elementHash.has_key(name)):
            self.elementHash[name].append([start, self.currentLine])
        else:
            self.elementHash[name] = [[start, self.currentLine]]

    def startElementNS(self, name, qname, attrs):
        self.currentLine += 1
        attrHash = {}
        for k in attrs.keys():
            attrHash[k] = attrs[k]
        ptxt = "#elemNS %s %s %s" % (name, qname, repr(attrHash))
        self.currentText.append(ptxt)

    def endElementNS(self, name, qname):
        self.currentLine += 1
        self.currentText.append("#endNS %s %s" % (name, qname))

    def characters(self, text, start=0, length=-1):
        if text.isspace():
            text = " "            
        self.currentLine += 1
        self.currentText.append("#text %s" % (text))
        self.recordSize += len(text.split())
                    
    def processingInstruction(self, target, data):
        pass
    def skippedEntity(self, name):
        pass


class XmlRecordStoreParser(Parser):
    # We take in stuff and return a Record, that makes us a Parser.
    # Retrieve metadata and sax list from XML structure

    def process_document(self, session, doc):
        # Take xml wrapper and convert onto object
        # Strip out SAX events first

        data = doc.get_raw()

        # Strip out sax to list
        saxre = re.compile("<c3:saxEvents>(.+)</c3:saxEvents>", re.S)
        match = saxre.search(data)
        data = saxre.sub("", data)
        elemHash = {}
        if match:
            sax = match.groups(1)[0]
            sax = sax.split("\n")
            # Now check if last is an element hash
            if sax[-1][:5] == "#hash":
                elemHash = eval(sax[-1][6:])
                sax = sax[:-1]
        else:
            sax = []
        
        # Now parse the rest of it and build Record
        rec = SaxRecord(sax)
        rec.elementHash = elemHash
        dom = domParseString(data)
        for c in dom.childNodes[0].childNodes:
            if c.nodeType == elementType:
                if (c.localName == "id"):
                    rec.id = flattenTexts(c)
                    if (rec.id.isdigit()):
                        rec.id = int(rec.id)
                elif (c.localName == "baseUri"):
                    rec.baseUri = flattenTexts(c)
                elif (c.localName == "schema"):
                    rec.schema = flattenTexts(c)
                elif (c.localName == "schemaType"):
                    rec.schemaType = flattenTexts(c)
                elif (c.localName == "size"):
                    rec.size = int(flattenTexts(c))
                elif (c.localName == "technicalRights"):
                    for c2 in c.childNodes:
                        if (c2.nodeType == elementType):
                            entry = (flattenTexts(c2), c2.localName, c2.getAttribute('role'))
                            rec.rights.append(entry)
                elif (c.localName == "history"):
                    for c2 in c.childNodes:
                        if (c2.nodeType == elementType):
                            # A modification
                            entry = ['', '', c2.getAttribute('type')]
                            for c3 in c2.childNodes:
                                if (c3.nodeType == elementType):
                                    if (c3.localName == "agent"):
                                        entry[0] = flattenTexts(c3)
                                    elif (c3.localName == "date"):
                                        entry[1] = flattenTexts(c3)
                            rec.history.append(entry)
        
        return rec

