#!/usr/bin/python

# Author: Rob Sanderson (azaroth@liv.ac.uk)
# Copyright:  Usable under the GPL
# Version: 0.5 (Beta)
# Description:  SRW/SRU Server, currently as gateway to Z39.50 server

# --------------------------------------------------------------------
# Imports and initialisation
# 

import os, sys, cStringIO as StringIO, cgi, traceback, time
from SimpleHTTPServer import SimpleHTTPRequestHandler
from BaseHTTPServer import HTTPServer
from ZSI import *
from xml.dom.minidom import parse, Node


import SRWTypes, SRWHandlers, SRWConfig, SRWDiagnostics, SRWState

# ------------------------------------------------------------
# SOAP level stuff, have already handled normal GET and SRU
#

def _Dispatch(ps, caller, **kw):
    '''Find a handler for the SOAP request
    Call SendResponse or SendFault to send the reply back
    via unknown transport on caller
    '''

    SendFault = caller.send_fault
    SendResponse = caller.send_xml
    url = caller.path

    if not SRWConfig.configURLHash.has_key(url):
        SendFault(Fault(Fault.Client, 'Unknown URL Path'))
        return
    else:
        config = SRWConfig.configURLHash[url]

    try:
        # OLD Authentication Token RBP
        headers = None
        token = None
        if ps.header_elements:
            for h in ps.header_elements:
                if h.localName == "authenticationToken":
                    tv = h.firstChild.data
                    # Find object
                    if SRWState.authenticationTokenValues.has_key(tv):
                        token = SRWState.authenticationTokenValues[tv]
                        token.time = time.time()
                        SRWState.authenticationTokens.sort(SRWState.sort_at)
                        headers = "<srw:authenticationToken>%s</srw:authenticationToken>" % tv
                    else:
                        # Unknown AT. May have expired.
                        SendFault(Fault(Fault.Client, 'Unknown AuthenticationToken'))
                        return
                    
        
        if ps.body_root.localName == "searchRetrieveRequest":
            result = SRWTypes.SearchRetrieveResponse('searchRetrieveResponse')
            try:
                req = ps.Parse(SRWTypes.SearchRetrieveRequest)
                req.calledAt = url
                if token:
                    req.authenticationToken = token
                result.processQuery(req)
            except SRWDiagnostics.SRWDiagnostic, diag:
                diag.code = diag.code
                diag.details = diag.details
                result.diagnostics = [diag]

        elif ps.body_root.localName =="explainRequest":
            # SOAP Explain request

            result = SRWTypes.ExplainResponse('explainResponse')
            try:
                req = ps.Parse(SRWTypes.ExplainRequest)
                req.calledAt = url
                if token:
                    req.authenticationToken = token
                result.processQuery(None, config)
                except SRWDiagnostics.SRWDiagnostic, diag:
                    diag.code = diag.code
                    diag.details = diag.details
                    result.diagnostics = [diag]

        elif ps.body_root.localName == "scanRequest":
            # Scan request.
            result = SRWTypes.ScanResponse('scanResponse')
            try:
                req = ps.Parse(SRWTypes.ScanRequest)
                req.calledAt = url
                if token:
                    req.authenticationToken = token
                result.processQuery(req)
            except SRWDiagnostics.SRWDiagnostic, diag:
                diag.code = diag.code
                diag.details = diag.details
                result.diagnostics = [diag]

        elif ps.body_root.localName == "authenticationTokenRequest":
            # Create and hand out an AT.
            result = SRWTypes.AuthTokenResponse('authenticationTokenResponse')
            result.processQuery(None, config)
            
        else:
            # Don't know what this is, fault with unknown method?
            SendFault(Fault(Fault.Client, 'Unknown Request')) 
            return

        reply = StringIO.StringIO()
        sw = SoapWriter(reply, nsdict=config.namespaces, header=headers)
        sw.serialize(result, inline=1)
        sw.close()
        SendResponse(reply.getvalue())

    except Fault, f:
        SendFault(f)
            
    except Exception, e:
        # Something went wrong, send a fault.
        print e
        SendFault(FaultFromException(e, 0, sys.exc_info()[2]))


# --------------- Overridden Handler ---------------------

class SOAPRequestHandler(SimpleHTTPRequestHandler):
    '''SOAP handler.
    Handle do_GET for Explain, SRU and normal files
    '''
    server_version = 'ZSI/1.2 ' + SimpleHTTPRequestHandler.server_version

    def send_xml(self, text, code=200):
        '''Send SOAP XML.'''
        self.send_response(code)
        self.send_header('Content-type', 'text/xml; charset="utf-8"')
        self.send_header('Content-Length', str(len(text)))
        self.end_headers()
        self.wfile.write(text)
        self.wfile.flush()

    def send_fault(self, f):
        '''Send a fault.'''
        self.send_xml(f.AsSOAP(), 500)

    def do_GET(self):
        """ Retrieve Explain record, handle SRU, act as normal server """
        global resultSets, authenticationTokens, authenticationTokenValues

        # Try to expire resultSets and authTokens
        SRWState.expire_rsat()

        text = ""
        headerText = ""
            
        qm = self.path.find("?")
        query = ""
        if qm > -1:
            query = self.path[qm+1:]
            self.path = self.path[:qm]

        # Old Authentication Token RBP
        token = ''
        for l in str(self.headers).splitlines():
            if len(l) > 27 and l[:27] == 'X-SRU-Authentication-Token':
                token = l[28:].strip()
            
        if not SRWConfig.configURLHash.has_key(self.path):
            # Will do 404s etc.
            return SimpleHTTPRequestHandler.do_GET(self)
        else:

            try:
                # Do SRU
                config = SRWConfig.configURLHash[self.path]

                if query:
                    opts = cgi.parse_qs(query)
                else:
                    opts['operation'] = ['explain']
                    opts['version'] = ['1.1']
                    opts['recordPacking'] = ['xml']

                # Switch on 'operation' field of query
                if not opts.has_key('operation'):
                    operation = 'searchRetrieve'
                else:
                    operation = opts['operation'][0]

                if operation == 'searchRetrieve':
                    req = SRWTypes.SearchRetrieveRequest('searchRetrieveRequest', init=1, config=config)
                    result = SRWTypes.SearchRetrieveResponse('searchRetrieveResponse')
                    result.echoedRequest = req
                    req.calledAt = self.path
                    if token:
                        req.authenticationToken = token

                    intOpts = ['maximumRecords', 'startRecord', 'resultSetTTL']
                    for o in opts.keys():
                        if o in intOpts:
                            setattr(req, o, int(opts[o][0]))
                        else:
                            setattr(req, o, opts[o][0])

                    try:
                        result.processQuery(req)            
                    except SRWDiagnostics.SRWDiagnostic, diag:
                        diag.code = diag.code
                        diag.details = diag.details
                        result.diagnostics = [diag]

                elif operation== 'scan':
                    req = SRWTypes.ScanRequest('scanRequest', init=1)
                    result = SRWTypes.ScanResponse('scanResponse')
                    result.echoedRequest = req
                    req.calledAt = self.path
                    if token:
                        req.authenticationToken = token

                    intOpts = ['numberOfTerms', 'responsePosition']
                    for o in opts.keys():
                        if o in intOpts:
                            setattr(req, o, int(opts[o][0]))
                        else:
                            setattr(req, o, opts[o][0])

                    try:
                        result.processQuery(req)            
                    except SRWDiagnostics.SRWDiagnostic, diag:
                        diag.code = diag.code
                        diag.details = diag.details
                        result.diagnostics = [diag]

                elif (operation == 'explain'):
                    config = SRWConfig.configURLHash[self.path]
                    req = SRWTypes.ExplainRequest('explainRequest')
                    result = SRWTypes.ExplainResponse('explainResponse')

                    try:
                        result.processQuery(req)
                    except Exception, err:
                        # XXX All explain diagnostics are surrogate?
                        pass
                        
                else:
                    # Unknown
                    result = SRWDiagnostics.Diagnostic4()
                    result.details = opts['operation'][0]


                reply = StringIO.StringIO()
                sw = SoapWriter(reply, envelope=0)
                sw.serialize(result, result.typecode)
                text = reply.getvalue()

                text = text.replace('Response>',  'Response xmlns="%s">' % (config.namespaces['srw']), 1)
                if config.xslUrl:
                    headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n' % (config.xslUrl) 
                else:
                    headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="/sru.xsl"?>\n'

            except:
                traceback.print_exc()
                text = "<html><body><h3>Something went badly wrong! You probably didn't send the right parameters</h3></body></html>"
                
            self.send_response(200)
            self.send_header("Content-type", "text/xml")
            self.end_headers()

            try:
                if headerText:
                    self.wfile.write(headerText)
                self.wfile.write(text)
            except IOError, err:
                print err
            self.wfile.flush()
            


    def do_POST(self):
        '''The POST command for handling SOAP'''

        # Expire RS and AT
        SRWState.expire_rsat()

        try:
            ct = self.headers['content-type']
            if ct.startswith('multipart/'):
                cid = resolvers.MIMEResolver(ct, self.rfile)
                xml = cid.GetSOAPPart()
                ps = ParsedSoap(xml, resolver=cid.Resolve)
            else:
                length = int(self.headers['content-length'])
                ps = ParsedSoap(self.rfile.read(length))
        except ParseException, e:
            self.send_fault(FaultFromZSIException(e))
            return
        except Exception, e:
            # Faulted while processing; assume it's in the header.
            self.send_fault(FaultFromException(e, 1, sys.exc_info()[2]))
            return

        _Dispatch(ps, self)


def AsServer(port=80, modules=None, docstyle=1, **kw):
    """ Run as a server """
    address = ('', port)
    httpd = HTTPServer(address, SOAPRequestHandler)
    httpd.modules = modules
    httpd.docstyle = docstyle
    httpd.serve_forever()

if (__name__ == "__main__"):
    # Start server

    # Parse Config File
    cfgFile = "serverConfig.xml"
    if len(sys.argv) == 2:
        cfgFile = sys.argv[1]
    elif len(sys.argv) > 2:
        print "Usage: " + sys.argv[0] + " [configFile]"
        sys.exit()
        
    if not os.path.exists(cfgFile):
        print "Could not load config file '%s', aborting." % cfgFile
        sys.exit()
    try:
        dom = parse(cfgFile)
    except Exception, err:
        print err
        print "Could not parse config file '%s', aborting." % cfgFile
        sys.exit()


    SRWConfig.buildConfig(SRWConfig, dom.childNodes[0])

    if 0:
        # EXAMPLE:  Build a trick postgres config
        pgcfg = SRWConfig.SRWConfig()
        pgcfg.database = "user_db"
        pgcfg.databaseSchema = "connectionlog"
        pgcfg.port = "postgres"
        pgcfg.url = "/postgres/"
        pgcfg.recordSchemaHash = {'http://www.loc.gov/zing/srw/dcschema/v1.0/' : []}
        SRWConfig.configs.append(pgcfg)

    for cfg in SRWConfig.configs:
        SRWConfig.configURLHash[cfg.url] = cfg


    AsServer(port=8080)
