#!/usr/bin/python2.2

# Standalone SRW server
# Version: 1.1
# Author: Rob Sanderson (azaroth@liv.ac.uk)

# Updated to SRW 1.1
# Refactored to use inspection tricks

import os, sys, cStringIO as StringIO, cgi, traceback, time
from SimpleHTTPServer import SimpleHTTPRequestHandler
from BaseHTTPServer import HTTPServer
from ZSI import *
from xml.dom.minidom import parse, Node
import SRWTypes, SRWHandlers, SRWConfig, SRWDiagnostics, SRWState

class SRWRequestHandler(SimpleHTTPRequestHandler):
    server_version='ZSI/1.2 ' + SimpleHTTPRequestHandler.server_version
    
    def send_xml(self, text, code=200, prexml=""):
        '''Send SOAP XML.'''
        self.send_response(code)
        self.send_header('Content-type', 'text/xml; charset="utf-8"')
        self.send_header('Content-Length', str(len(prexml + text)))
        self.end_headers()
        if prexml:
            self.wfile.write(prexml)
        self.wfile.write(text)
        self.wfile.flush()

    def send_fault(self, f):
        '''Send a fault.'''
        self.send_xml(f.AsSOAP(), code=500)

    def processUnknownOperation(self, operation, error, config):
        # ExplainResponse + Diagnostic

        result = SRWTypes.ExplainResponse('explainResponse')
        req = SRWTypes.ExplainRequest('explainRequest', opts={'recordPacking' : 'xml', 'version' : "1.1"}, config=config)
        result.processQuery(req)
        diag = SRWDiagnostics.Diagnostic4()
        diag.code = diag.code
        diag.details = operation + ": " + str(error)
        diag.message = diag.message
        result.diagnostics = [diag]
        return result

    def dispatch(self, req):
        '''Take request object and generate response object.'''

        global resultSets, authenticationTokens, authenticationTokenValues
        # Try to expire resultSets and authTokens
        SRWState.expire_rsat()

        req.calledAt = self.path
        result = req.responseType(req.responseName)

        try:
            result.processQuery(req)
        except SRWDiagnostics.SRWDiagnostic, diag:
            diag.code = diag.code
            diag.details = diag.details
            diag.message = diag.message
            result.diagnostics = [diag]

        return result


    def do_GET(self):

        qm = self.path.find("?")
        query = ""
        if qm > -1:
            query = self.path[qm+1:]
            self.path = self.path[:qm]

        if not SRWConfig.configURLHash.has_key(self.path):
            # Will do 404s etc.
            return SimpleHTTPRequestHandler.do_GET(self)
        else:

            config = SRWConfig.configURLHash[self.path]
            # Parse out params, if exist
            if query:
                opts = cgi.parse_qs(query)
            else:
                # No params == server defined explain
                opts = {
                    'operation' : ['explain'],
                    'version' : ['1.1'],
                    'recordPacking' : ['xml']
                    }

            # Switch on 'operation' field of query
            if not opts.has_key('operation'):
                operation = 'searchRetrieve'
            else:
                operation = opts['operation'][0]
                
            classname = operation[0].upper() + operation[1:] + "Request"
            req = None
            try:
                objType = getattr(globals()['SRWTypes'], classname)
                req = objType(operation + "Request", opts=opts, config=config)
                result = self.dispatch(req)
                style = ""
            except AttributeError, err:
                result = self.processUnknownOperation(operation, err, config)
                style = "/sru.xsl"
            # Otherwise it's just a crash!
                
            reply = StringIO.StringIO()
            sw = SoapWriter(reply, envelope=0)
            sw.serialize(result, result.typecode)
            text = reply.getvalue()
            
            # Tack on namespaces and stylesheets

            text = text.replace('Response>',  'Response xmlns="%s" xmlns:diag="%s" xmlns:xsi="%s" xmlns:xsd="%s">' % (config.protocolNamespaces['srw'], config.protocolNamespaces['diag'], 'http://www.w3.org/2001/XMLSchema', 'http://www.w3.org/2001/XMLSchema-instance'), 1)

            if req and req.stylesheet:
                headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n' % (req.stylesheet) 
            elif style:
                headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n' % (style) 
            elif config.xslUrl:
                headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n' % (config.xslUrl) 
            else:
                headerText = ""
            
            self.send_xml(text, prexml=headerText)


    def do_POST(self):

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

        if not SRWConfig.configURLHash.has_key(self.path):
            self.send_fault(Fault(Fault.Client, 'Unknown URL Path'))
        else:
            config = SRWConfig.configURLHash[self.path]
            callname = ps.body_root.localName
            classname = callname[0].upper() + callname[1:]
            req = None

            try:
                objType = getattr(globals()['SRWTypes'], classname)
                req = ps.Parse(objType)
                req.config=config
                result = self.dispatch(req)
            except AttributeError, err:
                # result = self.processUnknownOperation(classname, err, config)
                self.send_fault(Fault(Fault.Client, 'Unknown Operation'))
                return

            reply = StringIO.StringIO()

            sw = SoapWriter(reply, nsdict=config.protocolNamespaces)
            sw.serialize(result, inline=1)
            sw.close()

            if req and req.stylesheet:
                headerText = '<?xml version="1.0"?>\n<?xml-stylesheet type="text/xsl" href="%s"?>\n' % (req.stylesheet) 
            else:
                headerText = ""

            self.send_xml(reply.getvalue(), prexml=headerText)


# ----------
# Main Routine

if (__name__ == "__main__"):
    # Start server up

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
    for cfg in SRWConfig.configs:
        SRWConfig.configURLHash[cfg.url] = cfg

    httpd = HTTPServer(('', 8080), SRWRequestHandler)
    httpd.modules = None
    httpd.docstyle = 1
    httpd.serve_forever()
