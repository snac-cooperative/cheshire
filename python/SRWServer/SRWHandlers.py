
# Handlers for SRW Operations
# Version: 1.1
# Author:  Rob Sanderson (azaroth@liv.ac.uk)

import CQLParser, ChZoom, re, time, ZCheshire, SRWTypes, zmarc, cgi, traceback, string, SRWConfig, os, sys, SRWState, re, cStringIO as StringIO, types 
from ZSI import *
from SRWDiagnostics import *
from random import Random
from xml.sax.saxutils import escape

oldpackagedir = "/usr/lib/python1.5/site-packages"
if os.path.exists(oldpackagedir):
    sys.path.insert(0, oldpackagedir)
import pg

# For Authentication Tokens
randomGen = Random(time.time())
asciiChars = string.ascii_letters + string.digits + string.punctuation
asciiChars = asciiChars.replace("<","")
asciiChars = asciiChars.replace(">","")
asciiChars = asciiChars.replace("&","")

# -------------------------------------------------------------------
# Data transformations
#

def cheshireXPathHandler(record, schema):
    # get rid of <result_data> and <item>

    re1 = re.compile('<RESULT_DATA DOCID="[^>]+">\n<ITEM XPATH="[^>]+">\n')
    re2 = re.compile('</ITEM>\n<ITEM XPATH="[^>]+">\n')
    re3 = re.compile('</ITEM>\n</RESULT_DATA>\n')
    record = re1.sub('', record)
    record = re2.sub('', record)
    record = re3.sub('', record)
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
    xml = "<dc>\n"
    glist = record[1:]
    map = {1 : "title", 2 : "creator", 4 : "date",  5 : "identifier", 6 : "description", 8: "date",  17 : "description", 18 : "date", 20 : "language", 21 : "subject", 22 : "type", 27 : "format", 28 : "identifier", 29 : "rights", 30 : "relation", 31 : "publisher", 32 : "contributor", 33 : "source", 34 : "coverage"}
    for g in glist:
        if map.has_key(g['name']):
            xml += "  <%s>%s</%s>\n" % (map[g['name']], g['contents'], map[g['name']])
    xml += "</dc>"
    return xml

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


def toCheshire(self, config):
    """ Create cheshire sort spec [type, term, case, descending, MVA] """
    # Currently if we have a MVA and it's not a constant, it's messy.
    # XXX Allow sorting via indexes somehow
    #     -- Need mapping of index to XPath somewhere... How? Where?
    # XXX Fix ZCheshire for multiple sort keys already!!
    if self.missingValue:
        if self.missingValue[0] == '"':
            self.missingValue = self.missingValue[1:-1]
        elif self.missingValue == "highValue":
            self.missingValue = "ZZZZZZZZZZZ"
        elif self.missingValue == "lowValue":
            self.missingValue = "0000000000"
        elif self.missingValue in ['omit', 'abort']:
            diag = Diagnostic92()
            diag.details = "Omit and Abort not currently supported under Python"
            raise diag
    elif (self.schema != config.recordNamespaces[config.databaseSchema]):
        diag = Diagnostic87()
        diag.details = "Only " + config.recordNamespaces[config.databaseSchema] + " supported."
        raise diag

    return ["private", self.path, self.caseSensitive, not self.ascending, self.missingValue]
       

def parseSortKeys(self):
    " Parse sortKeys parameter to sortStructure "
    self.sortStructure = []
    if (self.xSortKeys != []):
        self.sortStructure = self.xSortKeys
    elif (self.sortKeys != ""):
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
            sko = SortKey('sortKey')
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


def parseCQL(self, conf):
    """ Turn CQL String/XCQL XML into Objects """
    if (self.query != ""):
        self.queryStructure = CQLParser.parse(self.query)
    else:
        # No Query, Request is seriously Broken
        f = Diagnostic10()
        f.message = 'Request must include a query'
        raise(f)
    # Set config on the top level of the query
    self.queryStructure.config = conf

SRWTypes.SearchRetrieveRequest.parseCQL = parseCQL


# --------------------------------------------------------------------
# Handle the query for pgql and z39.50
#

def handlePostgres(self, req, config):
    """ Handle postgres layer as opposed to Z39.50 layer """

    # Generate postgres query from CQL. Urgh.
    # Assumptions:  tables are similar to cheshire indexes... eg single entries.
    # Currently map directly from pg.foo to table foo

    where = req.queryStructure.toSQL()
    
    query =  "select count(*) from " + config.databaseSchema + " where " + where 
    if config.conn == None:
        config.pgConnect()
    res = config.conn.query(query)
    self.numberOfRecords = res.dictresult()[0]['count']

    if self.numberOfRecords > 0:
        last = min([req.maximumRecords, config.maximumRecords, self.numberOfRecords])
        limit = " LIMIT " + str(req.maximumRecords)
        offset = ""
        if (req.startRecord) != 1:
            sr = req.startRecord -1
            offset = " OFFSET " + str(sr)
        nsk = len(req.sortStructure)
        # Create ORDER BY
        order = ""
        if (nsk > 0):
            order = " ORDER BY "
            for sk in req.sortStructure:
                # ASC DESC
                order = order + sk.path + " "
                if sk.ascending == 0:
                    order = order + " DESC "
                    
        query =  "select * from " + config.databaseSchema + " where " + where + order + limit + offset
        print "Query: " + query
        res = config.conn.query(query)

        dicts = res.dictresult()
        recs = []
        for d in dicts:
            rec = SRWTypes.Record('record')
            rec.recordSchema = "POSTGRES"

            xml = "<record>\n"
            for k in d.keys():
                # Strip GET and HTTP/1.X
                if k == "uri":
                    d[k] = d[k][4:-9]
                xml += "  <%s>%s</%s>\n" % (k, d[k], k)
            xml += "</record>"
            
            rec.recordData = escape(xml)
            recs.append(rec)
        self.records = recs


SRWTypes.SearchRetrieveResponse.handlePostgres = handlePostgres

# ---- Main query handler ----

def processQuery(self, req):
    """ Send off query to Z target and handle results """

    # Get our config based on URL
    config = SRWConfig.configURLHash[req.calledAt]

    # Setup for processing
    req.parseCQL(config)
    req.xQuery = req.queryStructure.toXCQL()
    self.echoedRequest = req
    req.parseSortKeys()


    # Check if we recognise the record Schema
    if not req.recordSchema:
        schema = config.defaultRetrieveSchema
        req.recordSchema = schema
    else:
        schema = req.recordSchema
        # Redirect to full value
        if (config.recordNamespaces.has_key(schema)):
            schema = config.recordNamespaces[schema]
            req.recordSchema = schema
        if (not schema in config.recordNamespaces.values()):
            diag = Diagnostic66()
            diag.details = req.recordSchema
            raise diag

    if not req.recordPacking:
        req.recordPacking = "string"
    elif not req.recordPacking  in ["string", "xml"]:
        diag = Diagnostic71()
        diag.details = req.recordPacking;
        raise diag

    # Split off here for postgres layer
    if config.port == "postgres":
        self.handlePostgres(req, config)
        return

    # Is the query a 'present' special case
    nsk = len(req.sortStructure)   # If sorted, we need a temp result set
    rsn =  req.queryStructure.getResultSetName()

    # Otherwise does the client want a resultSet?
    ttl = req.resultSetTTL
    if ttl > config.maxTTL or ttl < 0:
        # TTL is non fatal
        diag = Diagnostic51()
        diag.code = diag.code
        diag.details = "TTL Requested (%s) is negative or greater than maximum of %d" % (req.resultSetTTL, config.maxTTL)
        diag.message = diag.details
        self.diagnostics = [diag]
        ttl = 0

    # Query for result set and no connection. Fail.
    if rsn and not config.conn:
        diag = Diagnostic51()
        diag.details = rsn
        raise diag

    if config.conn and rsn and not nsk:
        # Present.
        qo = ChZoom.Query(rsn + ":")
        config.conn.namedResultSets = 0
    else:
        q = req.queryStructure.toCheshire()
        qo = ChZoom.Query(q)

    if config.logLevel > 1:
        print "Built Query: " + qo.querystr

    # Connect
    if config.conn == None:
        config.zconnect()

    if rsn and not nsk:
        # Asked for a resultset, need to touch the time on it.
        foundRS = 0
        for set in SRWState.resultSets:
            if set['resultSetName'] == rsn:
                foundRS = 1
                set['time'] = time.time()
                if ttl:
                    set['ttl'] = ttl
                else:
                    ttl = set['ttl']
                SRWState.resultSets.sort(SRWState.sort_rset)
                break
        if not foundRS:
            # We don't know about it so throw unknown rset
            diag = Diagnostic51()
            diag.details = rsn
            raise diag
            
    retry = 0
    rs = None
    while retry < 2:
        try:
            rs = config.conn.search(qo)
            # We never get to this if the search errors
            retry = 3
        except ChZoom.Bib1Err, bib1:
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
        except ChZoom.ConnectionError, econn:
            config.zconnect()
            retry = retry +1
        except Exception, err:
            # Uhoh...
            diag = Diagnostic2()
            diag.details = str(err)
            raise diag

    if rs == None:
        # Couldn't [re]connect?!
        diag = Diagnostic2()
        diag.details="Couldn't generate a new result set or couldn't connect to Z server."
        raise diag

    # Check if we need to sort
    if (nsk > 1):
        diag = Diagnostic84()
        diag.details = "Currently only one sort key is supported."
        raise diag
    elif (nsk == 1):
        sQL = req.sortStructure[0].toCheshire(config)
        # Need to fall back to ZCheshire level as Zoom doesn't do sort yet
        try:
            if sQL[4] != "":
                # We have an MVA
                sortResults = ZCheshire.sort(rs.name, rs.name, sQL[0], sQL[1], sQL[2], sQL[3], sQL[4])
            else:
                sortResults = ZCheshire.sort(rs.name, rs.name, sQL[0], sQL[1], sQL[2], sQL[3])
            if sortResults['status'] != 0:
                # Why did it fail?
                diag = Diagnostic82()
                diag.details = "Sort failed with status %i" %( sortResults['status'] )
                raise diag
        except ZCheshire.error, bib1:
            # Why did it fail?
            diag = Diagnostic82()
            diag.details = str(bib1)
            raise diag
        except Exception, err:
            # Unexpected.
            diag = Diagnostic2()
            diag.details = str(err)
            raise diag                

    # Has to be after sort incase mva=omit (when it works)
    self.numberOfRecords = len(rs)
    last = min([req.maximumRecords, config.maximumRecords, self.numberOfRecords])

    if rs.name and ttl:
        # Add to resultSets list
        SRWState.resultSets.append({'time' : time.time(), 'config' : config, 'resultSetName' : rs.name, 'ttl': ttl})
        SRWState.resultSets.sort(SRWState.sort_rset)

    if config.logLevel > 1:
        print "Found:  " + str(len(rs))  +  "  Fetching:  " + str(last) + " From " + str(req.startRecord)
    if (self.numberOfRecords > 0):
        self.records = []
        if (self.numberOfRecords < req.startRecord or req.startRecord < 1):
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

        if (req.recordXPath):
            if (req.recordSchema != config.databaseSchema):
                diag = Diagnostic74()
                diag.details = config.databaseSchema
                raise diag
            else:
                config.conn.elementSetName = "XML_ELEMENT_" + req.recordXPath
                externalHandler = cheshireXPathHandler
                resetDefaults = 1

        # REs for checking/substing namespace
        hasxmlns = re.compile("^[ ]*<[^>]+ xmlns[ ]*=[ ]*\"")
        addns = re.compile("^[ ]*<([^>]+)>")
        xmlver = re.compile("^[ ]*<\?xml[^>]+>")
        attr_re = re.compile('([a-zA-Z0-9_]+)[ ]*=[ ]*([-:_.a-zA-Z0-9]+)')
        attr_elem = re.compile('(<[/]?)([a-zA-Z0-9_]+)')

        m = min(len(rs), req.startRecord + last - 1)

        for r in range(req.startRecord-1, m):

            ro = SRWTypes.Record('record')
            ro.recordPacking = req.recordPacking
            ro.recordPosition = r +1

            try:
                rec = rs[r].get_raw()
                # If a well behaved server says that rec is XML we need to strip
                if (type(rec) == types.StringType):
                    rec = xmlver.sub("", rec);

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

            except ChZoom.Bib1Err, bib1:
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
                diag.code = diag.code
                sw.serialize(diag)
                rec = reply.getvalue()
                ro.recordSchema = "http://www.loc.gov/zing/srw/diagnostic/v1.0/"
                
            try:
                if req.recordPacking == "string":
                    ro.recordData = escape(rec)
                else:
                    if (not req.recordXPath and not re.match(hasxmlns, rec)):
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

        if ( (req.startRecord + last) <= self.numberOfRecords):
            self.nextRecordPosition = req.startRecord + last
        if (ttl or rsn):
            self.resultSetId = rs.name
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
    config = SRWConfig.configURLHash[req.calledAt]
    self.terms = []
    self.echoedRequest = req

    if config.port == "postgres":
        diag = Diagnostic4()
        diag.details = "scan"
        raise diag

    if req.scanClause:
        #convert clause into SearchClause object
        req.xScanClause = CQLParser.parse(req.scanClause)
    else:
        # Seriously broken request.
        f = Diagnostic10()
        f.message = 'Request must include a query'
        raise(f)
    
    req.xScanClause.config = config
    idx = CQLParser.convertIndex(req.xScanClause, req.xScanClause)
    term = req.xScanClause.term.strip()
    if term == '':
        term = 'a'
    q = "%s %s" % (idx, term)
    print "Built Query:  " + q

    req.xScanClause = req.xScanClause.toXCQL()

    qo = ChZoom.Query(q)
    if config.conn == None:
        config.zconnect()

    # Set up Z connection for scan
    config.conn.stepSize = 0
    if not req.numberOfTerms:
        req.numberOfTerms = config.defaultNumberOfTerms
    if not req.responsePosition:
        req.responsePosition = config.defaultResponsePosition
    config.conn.numberOfEntries = req.numberOfTerms
    config.conn.responsePosition = req.responsePosition

    ss = []

    retries = 0
    while retries < 3:
        try:
            ss = config.conn.scan(qo)
            retries = 3
        except ChZoom.ConnectionError:
            retries +=1
            config.zconnect()
            config.conn.stepSize = 0
            config.conn.numberOfEntries = req.numberOfTerms
            config.conn.responsePosition = req.responsePosition +1
        except ChZoom.Bib1Err, bib1:
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
    config = req.config
    f = open(config.SRWZeeRex, "r")
    # add in default stylesheet
    if (not req.stylesheet):
        req.stylesheet = "/sru.xsl"
    if f:
        filestr = f.read()
        # Create a record object and populate
        rec = SRWTypes.Record('record')
        rec.recordPacking = req.recordPacking
        if (rec.recordPacking == 'string'):
            filestr = escape(filestr)
        rec.recordSchema = config.recordNamespaces['zeerex']
        rec.recordData = filestr
        self.record = rec
        

SRWTypes.ExplainResponse.processQuery = processQueryExplain




def processQueryAuthToken(self, req, config):
    token = SRWTypes.AuthToken('authenticationToken')
    # Create a char[24] code (NB Strings immutable)
    chars = []
    for x in range(24):
        chars.append(asciiChars[randomGen.randrange(91)])
    token.value = ''.join(chars)
    token.ttl = 600
    token.time = time.time()
    token.config = config
    self.token = token
    SRWState.authenticationTokenValues[token.value] = token
    SRWState.authenticationTokens.append(token)

SRWTypes.AuthTokenResponse.processQuery = processQueryAuthToken
          
                
