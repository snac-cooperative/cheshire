#!/usr/bin/python

from ZSI.client import Binding
import SRWTypes, SRWConfig, sys
from xml.dom.minidom import Node, parseString

cf = SRWConfig.SRWConfig()
nd = cf.protocolNamespaces

def search(req, b):
    return b.RPC(b.url,                       # Path
                 "searchRetrieveRequest",       # Request name
                 req,                           # Request object
                 requestclass=SRWTypes.SearchRetrieveRequest, # Serialization for Req.
                 replytype = SRWTypes.SearchRetrieveResponse.typecode) # Serialization for Resp

def explain(req, b):
    return b.RPC(b.url,
                 "explainRequest",
                 req,
                 requestclass=SRWTypes.ExplainRequest,
                 replytype=SRWTypes.ExplainResponse.typecode)

def scan(req, b):
    return b.RPC(b.url,
                 "scanRequest",
                 req,
                 requestclass=SRWTypes.ScanRequest,
                 replytype=SRWTypes.ScanResponse.typecode)


#b = Binding(host="indexdata.dk",  port = 9000,  nsdict=nd, url = "/voyager",  tracefile = sys.stdout)
#b = Binding(host="localhost", port=8080, url="/l5r", nsdict=nd,  tracefile = sys.stdout)
b = Binding(host="alcme.oclc.org", port=80, url="/srw/search/SOAR", nsdict=nd, tracefile = sys.stdout)

# SRU only
#b = Binding(host ="greta.pica.nl", port=1080, url="/sru/", nsdict=nd, tracefile=sys.stdout)
#b = Binding(host = "krait.kb.nl", port=80, url="/cgi-zoek/sru.pl", nsdict=nd, tracefile=sys.stdout)


req = SRWTypes.SearchRetrieveRequest('searchRetrieveRequest')
req.query = "dc.title any fossil"
req.version = "1.1"
req.maximumRecords = 1
req.recordSchema = "info:srw/schema/1/dc-v1.1"
req.recordPacking = "string"
req.recordXPath = "/dc/title"
req.stylesheet = "http://www.o-r-g.org/foo/"

#req.extraRequestData = [parseString('<foo xmlns="http://foo/">bar</foo>')]

exp = SRWTypes.ExplainRequest('explainRequest')
exp.version = "1.1"
#exp.extraRequestData = [parseString('<foo xmlns="http://foo/">bar</foo>')]

scanreq = SRWTypes.ScanRequest('scanRequest')
scanreq.scanClause = 'dc.title = "duke"'
scanreq.version = "1.1"
#scanreq.extraRequestData = [parseString('<foo xmlns="http://foo/">bar</foo>')]

r = search(req, b)
#print r.numberOfRecords
#for rec in r.records:
#    print rec.recordData

#r = explain(exp, b)
#print r.record
    
# r = scan(scanreq, b)
#for t in r.terms:
#    print "%s %i" % (t.value, t.numberOfRecords)





