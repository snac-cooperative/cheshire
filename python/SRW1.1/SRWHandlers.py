
# Handlers for SRW Operations
# Version: 1.1
# Author:  Rob Sanderson (azaroth@liv.ac.uk)

import re, time, SRWTypes, cgi, traceback, string, SRWConfig, os, sys, SRWState, re, cStringIO as StringIO, types
from ZSI import *
from PyZ3950.SRWDiagnostics import *
from random import Random
from xml.sax.saxutils import escape
from PyZ3950 import zoom, CQLParser, pqf, zmarc

# For Authentication Tokens and Result Sets
randomGen = Random(time.time())
asciiChars = string.ascii_letters + string.digits + string.punctuation
asciiChars = asciiChars.replace("<","")
asciiChars = asciiChars.replace(">","")
asciiChars = asciiChars.replace("&","")
asciiChars = asciiChars.replace("\\","")
asciiChars = asciiChars.replace("^","")
asciiChars = asciiChars.replace("*","")
asciiChars = asciiChars.replace("?","")


# -------------------------------------------------------------------
# Data transformations
#

def cheshireXPathHandler(record, schema):
    # get rid of <result_data> and <item>

    recxml = ['<result>\n  <nodeSet schema="%s">\n' % (schema)]
    reclines = record.split('\n')
    xpath = ""
    for l in reclines:
        if (l[:5] == "<ITEM"):
            xpath = l[13:-2]
            recxml.append('    <node type="element" path="%s">\n' % (xpath))
        elif (l[:6] == "</ITEM"):
            xpath = ""
            recxml.append('\n    </node>\n')
        elif (xpath <> ""):
            recxml.append(l)

    recxml.append("  </nodeSet>\n</result>")
    record = ''.join(recxml)
    return record

def Unesco2ZthesHandler(record, schema):
    "Convert Unesco database into Zthes schema"
    record = record.replace('<name>', '<termName>')
    record = record.replace('</name>', '</termName>')
    record = record.replace('<info>', '<termId>')
    record = record.replace('</info>', '</termId>')
    record = record.replace('<SN>', '<termNote>')
    record = record.replace('</SN>', '</termNote>')

    exp = re.compile('<heading>[^<]+</heading>')
    record = re.sub(exp, '', record)

    exp = re.compile('<(USE|BT|UF|NT|RT)>([^<]+)</(USE|BT|UF|NT|RT)>')
    record = re.sub(exp, r'<relation><relationType>\1</relationType><termName>\2</termName></relation>', record)
    return ''.join(['<Zthes>', record, '</Zthes>'])


def DCGRS1Handler(record, schema):
    "Convert GRS1 record into Dublin Core"
    xml = ["<srw_dc:dc xmlns:srw_dc=\"%s\" xmlns=\"http://purl.org/dc/elements/1.1/\">\n" % schema]
    glist = record.children
    map = {1 : "title", 2 : "creator", 4 : "date",  5 : "identifier", 6 : "description", 8: "date",  17 : "description", 18 : "date", 20 : "language", 21 : "subject", 22 : "type", 27 : "format", 28 : "identifier", 29 : "rights", 30 : "relation", 31 : "publisher", 32 : "contributor", 33 : "source", 34 : "coverage"}
    for g in glist:
        if map.has_key(g.leaf.tagValue[1]):
            xml.append("  <%s>%s</%s>\n" % (map[g.leaf.tagValue[1]], g.leaf.content[1], map[g.leaf.tagValue[1]]))
    xml.append("</srw_dc:dc>\n")
    return ''.join(xml)

def MARCHandler(record, schema):
    "Convert MARC record into various schemas"
    mo = zmarc.MARC(record)
    if schema in["http://www.loc.gov/marcxml/", "http://www.loc.gov/MARC21/slim"]:
        return mo.toMARCXML()
    elif schema == "http://srw.o-r-g.org/schemas/marcsgml/12.0/":
        return mo.toSGML()
    elif schema == "http://www.openarchives.org/OAI/1.1/oai_marc/":
        return mo.toOAIMARC()
    elif schema == "http://www.loc.gov/zing/srw/dcschema/v1.0/":
        return mo.toSimpleDC()
    elif schema == "http://www.loc.gov/mods/":
        return mo.toMODS()
    else:
        diag = Diagnostic66()
        diag.details = "Cannot convert MARC to requested schema"
        raise diag

# Transform SGML -> XML 
def lowerElement(match):
    """ SGML to XML regular expression function. Converts elements to lowercase. """
    return match.groups()[0] + match.groups()[1].lower()

def attributeFix(match):
    """ SGML to XML regular expression function. Quotes attributes """
    fix = match.groups()[0].lower() + '="' + match.groups()[1] + '"'
    return fix



# ----------------------------------------------------------------------
# Parsers for sections of the request
#


def parseSortKeys(self):
    " Parse sortKeys parameter to sortStructure "
    self.sortStructure = []
    if (self.sortKeys):
        # First try some simple parsing...
        self.sortKeys = self.sortKeys.strip()
        sks = self.sortKeys.split()
        if (len(sks) > 1):
            for s in sks:
                if not (s[0] in ['"', '/']):
                    # Paths should always start with / or " something is screwed up
                    # XXX Write better parser!!
                    pass

        skObjects = []
        for skstr in sks:
            sko = SRWTypes.SortKey('sortKey')
            sk = skstr.split(",")
            sko.path = sk[0]
            try:
                sko.schema = sk[1]
                sko.ascending = int(sk[2])
                sko.caseSensitive = int(sk[3])
                sko.missingValue = sk[4]
            except:
                # No problems if these fail from indexError
                pass
            skObjects.append(sko)
        self.sortStructure = skObjects

SRWTypes.SearchRetrieveRequest.parseSortKeys = parseSortKeys


# ---- Main query handler ----

def processQuery(self, req):
    """ Send off search query to Z target and handle results """

    if (not req.version):
        diag = Diagnostic7()
        diag.message = "Mandatory 'version' parameter not supplied"
        diag.details = 'version'
        raise diag

    # Get our config based on URL
    config = SRWConfig.configURLHash[req.calledAt]

    # Setup for processing

    if (req.query != ""):
        req.queryStructure = CQLParser.parse(req.query)
    else:
        # No Query, Request is seriously Broken
        f = Diagnostic7()
        f.message = 'Request must include a query'
        f.details = 'query'
        raise f
    req.queryStructure.config = config

    req.xQuery = req.queryStructure.toXCQL()
    self.echoedSearchRetrieveRequest = req

    req.parseSortKeys()

    if (req.diagnostics):
        self.diagnostics = req.diagnostics
        return

    # Check if we recognise the record Schema
    schema = req.get('recordSchema')
    # Redirect to full value
    if (config.recordNamespaces.has_key(schema)):
        schema = config.recordNamespaces[schema]
    if (not schema in config.recordNamespaces.values()):
        diag = Diagnostic66()
        diag.details = schema
        raise diag

    recordPacking = req.get('recordPacking')
    if not recordPacking  in ["string", "xml"]:
        diag = Diagnostic71()
        diag.details = req.recordPacking;
        raise diag

    startRecord = req.get('startRecord')
    maximumRecords = req.get('maximumRecords')

    # Is the query a 'present' special case
    nsk = len(req.sortStructure)   # If sorted, we need a temp result set
    rsn =  req.queryStructure.getResultSetId()

    # Otherwise does the client want a resultSet?
    ttl = req.get('resultSetTTL')
    if ttl > config.maxTTL or ttl < 0:
        # TTL is non fatal
        diag = Diagnostic51()
        diag.uri = diag.uri
        diag.details = "TTL Requested (%s) is negative or greater than maximum of %d" % (req.resultSetTTL, config.maxTTL)
        diag.message = diag.details
        self.diagnostics = [diag]
        ttl = 0

    # Query for result set and no connection. Fail now.
    if rsn and not config.conn:
        diag = Diagnostic51()
        diag.details = rsn
        raise diag

    if (nsk or not (config.conn and rsn)):
        req.queryStructure.config = config
        try:
            qo = zoom.Query('CQL-TREE', req.queryStructure)
        except SRWDiagnostic, err:
            raise err

        if config.logLevel > 1:
            # Need convenient short form.
            config.log("Built Query: %s"  % ( pqf.rpn2pqf(qo.query) ))

        # Connect
        if config.conn == None:
            config.zconnect()
    else:
        # Present
        qo = None

    if rsn and not nsk:
        # Asked for a resultset, need to touch the time on it.
        foundRS = 0
        for set in SRWState.resultSets:
            if set['resultSetId'] == rsn:
                foundRS = 1
                set['time'] = time.time()
                if ttl:
                    set['ttl'] = ttl
                else:
                    ttl = set['ttl']
                SRWState.resultSets.sort(SRWState.sort_rset)
                rs = set['resultSet']
                break
        if not foundRS:
            # We don't know about it so throw unknown rset
            diag = Diagnostic51()
            diag.details = rsn
            raise diag
            

    if (qo):
        retry = 0
        rs = None
        while retry < 3:
            try:
                rs = config.conn.search(qo)
                # We never get to this if the search errors
                retry = 3
            except zoom.Bib1Err, bib1:
                # Map from bib1 to SRW Diagnostic
                if bib1.condition == 114:
                    diag = Diagnostic16()
                    diag.details = bib1.message
                    raise diag
                elif bib1.condition == 22:
                    # Try again with no result set
                    config.conn.namedResultSets = 0
                    retry = retry + 1
                elif bib1.condition == 30:
                    # Nonexistant Result Set
                    diag = Diagnostic51()
                    raise diag
                elif bib1.condition == 4:
                    # Term only stopwords
                    diag = Diagnostic35()
                    raise diag
                else:
                    diag = Diagnostic2()
                    diag.details = str(bib1)
                    raise diag
            except zoom.ConnectionError, econn:
                config.zconnect()
                retry = retry +1
            except Exception, err:
                # Uhoh...
                diag = Diagnostic2()
                diag.details = repr(err)
                raise diag

    if rs == None:
        # Couldn't [re]connect?!
        diag = Diagnostic2()
        diag.details="Couldn't generate a new result set or couldn't connect to Z server."
        raise diag

    # Check if we need to sort
    if (nsk > 0):
        # Turn local sortKeys into zoom.SortKeys.
        zsk = []
        for k in req.sortStructure:
            n = zoom.SortKey()
            n.caseInsensitive = not k.caseSensitive
            if (k.ascending == 1):
                n.relation = 'ascending'
            else:
                n.relation = 'descending'
            if (k.missingValue in ['abort', 'omit']):
                n.missingValueAction = k.missingValue
            elif (k.missingValue == 'highValue' or not k.missingValue):
                n.missingValueData = 'ZZZZZZZZ'
            elif (k.missingValue == 'lowValue'):
                n.missingValueData = '00000000'
            elif (k.missingValue):
                n.missingValueData = k.missingValue[1:-1]

            # Cheshire can sort by XPath in private
            ss = k.schema
            if (config.recordNamespaces.has_key(ss)):
                ss = config.recordNamespaces[ss]

            if (ss == config.databaseSchema):
                n.type="private"
                n.sequence = k.path
            elif (ss == config.recordNamespaces['dc']):
                # turn /dc/title into dc.title, querify it, chuck it into sequence
                p = k.path
                if (p[0] == "/"):
                    p = p[1:]
                if (p[:3] == "dc/"):
                    p = "dc.%s" % (p[3:])
                else:
                    p = "dc.%s" % (p)
                q = zoom.Query('CQL', '%s exact/cql.string fish' % p)
                print pqf.rpn2pqf(q.query)
                n.sequence = q
            else:
                # Uhhh.... Raise a diagnostic.
                diag = Diagnostic87()
                diag.details = ss
                raise diag
            zsk.append(n)

        try:
            rs = config.conn.sort([rs], zsk)
        except zoom.Bib1Err, bib1:
            if (bib1.condition == 123):
                diag = Diagnostic89()
                raise diag
            else:
                # Not sure what went wrong...
                diag = Diagnostic82()
                raise diag
        except:
            diag = Diagnostic2()
            diag.details = repr(err)
            raise diag
            

    # Has to be after sort incase mva=omit (when it works)
    self.numberOfRecords = len(rs)
    last = min([maximumRecords, self.numberOfRecords])

    if not rsn and ttl:
        # Add to resultSets list
        chars = []
        for x in range(10):
            chars.append(asciiChars[randomGen.randrange(len(asciiChars))])
        name = ''.join(chars)
        setattr(rs, "X-name", name)
        SRWState.resultSets.append({'time' : time.time(), 'config' : config, 'resultSetId' : name, 'ttl': ttl, 'resultSet' : rs})
        SRWState.resultSets.sort(SRWState.sort_rset)

    if config.logLevel > 1:
        config.log("Found:  " + str(len(rs))  +  "  Fetching:  " + str(last) + " From " + str(req.get('startRecord')))
    if (self.numberOfRecords > 0):
        self.records = []
        if (self.numberOfRecords < startRecord or startRecord < 1):
            diag = Diagnostic61()
            diag.details = str(req.startRecord)
            raise diag
        recs = []
        externalHandler = resetDefaults = 0

        if config.recordSchemaHash.has_key(schema):
            resetDefaults = 1
            for k in config.recordSchemaHash[schema].keys():
                if k != "handler":
                    setattr(config.conn, k, config.recordSchemaHash[schema][k])
                else:
                    externalHandler = 1

        recordXPath = req.get('recordXPath')
        if (recordXPath):
            if (schema != config.databaseSchema):
                # We can only handle XML_ELEMENT_ in our base schema
                # XXX: Use 4Suite to extract XPaths.
                diag = Diagnostic74()
                diag.details = config.databaseSchema
                raise diag
            else:
                config.conn.elementSetName = "XML_ELEMENT_" + recordXPath
                externalHandler = cheshireXPathHandler
                resetDefaults = 1

        # REs for checking/substing namespace
        hasxmlns = re.compile("^[ ]*<[^>]+ xmlns[ ]*=[ ]*\"")
        addns = re.compile("^[ ]*<([^>]+)>")
        xmlver = re.compile("^[ ]*<\?xml[^>]+>")
        doctype = re.compile("^[ ]*<\?DOCTYPE[^>]+>")
        attr_re = re.compile('([a-zA-Z0-9_]+)[ ]*=[ ]*([-:_.a-zA-Z0-9]+)')
        attr_elem = re.compile('(<[/]?)([a-zA-Z0-9_]+)')

        m = min(len(rs), startRecord + last - 1)

        for r in range(startRecord-1, m):
            ro = SRWTypes.Record('record')
            ro.recordPacking = recordPacking
            ro.recordPosition = r +1

            try:
                rec = rs[r].data
                # If a 'well behaved' server says that rec is XML we need to strip
                if (type(rec) == types.StringType):
                    rec = xmlver.sub("", rec);
                    rec = doctype.sub("", rec);

                if (recordXPath):
                    ro.recordSchema = config.recordNamespaces['xpath']
                elif (config.recordNamespaces.has_key(schema)):
                    ro.recordSchema = config.recordNamespaces[schema]
                else:
                    ro.recordSchema = schema

                if externalHandler == 1:
                    rec = config.recordSchemaHash[schema]["handler"](rec, schema)
                elif externalHandler != 0:
                    rec = externalHandler(rec, schema)
                else:
                    if config.convertFromSGML:
                        rec = attr_re.sub(attributeFix, rec);
                        rec = attr_elem.sub(lowerElement, rec)
                        rec = rec.replace('& ', '&amp; ')

            except zoom.Bib1Err, bib1:
                # Map from bib1 to SRW Diagnostic, surrogate for the record
                # All non surrogate errors should already have been caught.
                if bib1.condition == 12:
                    diag = Diagnostic60()
                elif bib1.condition == 13:
                    diag = Diagnostic61()
                elif bib1.condition == 14:
                    diag = Diagnostic1()
                elif bib1.condition == 238:
                    diag = Diagnostic67()
                elif bib1.condition == 1026:
                    diag = Diagnostic64()
                elif bib1.condition == 1028:
                    diag = Diagnostic65()
                elif bib1.condition == 1070:
                    diag = Diagnostic67()
                else:
                    diag = Diagnostic2()
                diag.details = bib1.message

                # Now we need to serialise it and dump it into the record
                reply = StringIO.StringIO()
                sw = SoapWriter(reply, envelope=0)
                # Required Insanity
                diag.uri = diag.uri
                diag.message = ""
                sw.serialize(diag)
                rec = reply.getvalue()
                ro.recordSchema = config.recordNamespaces['diag']
                
            try:
                if recordPacking == "string":
                    ro.recordData = escape(rec)
                else:
                    if (not recordXPath and not re.match(hasxmlns, rec)):
                        # Check if we have a namespace, otherwise assign recordSchema
                        rec = re.sub(addns, (r'<\1 xmlns="%s">' % (ro.recordSchema)), rec)
                    ro.recordData = rec
            except Exception, err:
                ro.recordData = "SYSTEM FAILURE: NO DATA AVAILABLE"
            recs.append(ro)

        self.records = recs
        if resetDefaults:
            for k in config.recordSchemaHash[schema].keys():
                if k != "handler":
                    setattr(config.conn, k, getattr(config, k))

        if ( (startRecord + last) <= self.numberOfRecords):
            self.nextRecordPosition = startRecord + last
        if (ttl or rsn):
            self.resultSetId = getattr(rs, 'X-name')
            self.resultSetIdleTime = ttl
        else:
            # Trash the result set
            try:
                rs.delete()
            except:
                # XXX Should we try anything here?
                pass

SRWTypes.SearchRetrieveResponse.processQuery = processQuery

def scanProcessQuery(self, req):
    # Process a scan query

    self.terms = []
    config = SRWConfig.configURLHash[req.calledAt]

    if (not req.version):
        diag = Diagnostic7()
        diag.message = "Mandatory 'version' parameter not supplied"
        diag.details = 'version'
        raise diag

    if req.scanClause:
        #convert clause into SearchClause object
        clause = CQLParser.parse(req.scanClause)
        # Stupid schema.
        xsc = []
        xsc.append(clause.index.toXCQL())
        xsc.append(clause.relation.toXCQL())
        xsc.append(clause.term.toXCQL())
        req.xScanClause = "".join(xsc)
    else:
        # Seriously broken request.
        f = Diagnostic7()
        f.message = 'Request must include a query'
        f.details = 'scanClause'
        raise f

    self.echoedScanRequest = req
    if (req.diagnostics):
        self.diagnostics = req.diagnostics
        return

    if (not clause.term.value):
	clause.term.value = 'a'
    clause.config = config
    qo = zoom.Query('CQL-TREE', clause)

    if config.logLevel > 1:
        # Need convenient short form.
        config.log("Built Query: %s"  % ( pqf.rpn2pqf(qo.query) ))    

    if config.conn == None:
        config.zconnect()

    # Set up Z connection for scan
    config.conn.stepSize = 0

    mt = req.get('maximumTerms')
    rp = req.get('responsePosition')
    config.conn.numberOfEntries = mt
    config.conn.responsePosition = rp
    ss = []

    retries = 0
    while retries < 3:
        try:
            ss = config.conn.scan(qo)
            retries = 3
        except zoom.ConnectionError:
            retries +=1
            config.zconnect()
            config.conn.stepSize = 0
            config.conn.numberOfEntries = mt
            config.conn.responsePosition = rp
        except zoom.Bib1Err, bib1:
            if bib1.condition == 9:
                # No term found
                diag = Diagnostic2()
                diag.details = "No Terms Found."
                raise diag
            else:
                diag = Diagnostic2()
                diag.details = str(bib1)
                raise diag

    for t in ss:
        term = SRWTypes.ScanTerm('term', opts=t)
        self.terms.append(term)


SRWTypes.ScanResponse.processQuery = scanProcessQuery


def processQueryExplain(self, req):

    if (not req.version):
        diag = Diagnostic7()
        diag.message = "Mandatory 'version' parameter not supplied"
        diag.details = 'version'
        raise diag

    config = req.config

    self.echoedExplainRequest = req

    f = open(config.SRWZeeRex, "r")
    # add in default stylesheet
    if (not req.stylesheet):
        req.defaultStylesheet = SRWConfig.xslUrl
    if f:
        filestr = f.read()
        # Create a record object and populate
        rec = SRWTypes.Record('record')
        rec.recordPacking = req.recordPacking
        if (req.recordPacking == 'string'):
            filestr = escape(filestr)
        rec.recordSchema = config.recordNamespaces['zeerex']
        rec.recordData = filestr
        self.record = rec

SRWTypes.ExplainResponse.processQuery = processQueryExplain


          
                
