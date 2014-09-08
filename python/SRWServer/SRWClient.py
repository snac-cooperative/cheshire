#!/usr/bin/python

from ZSI.client import Binding
import SRWTypes, SRWDiagnostics
import sys

req = SRWTypes.SearchRetrieveRequest('searchRetrieveRequest', opts={'x-foo': 'bar', 'x-baz' : 'frob'})
req.query = "cardname any \"ninja naga token\""
req.recordSchema = "http://srw.o-r-g.org/schemas/ccg/1.0/"
req.maximumRecords = 1
req.recordPacking = "xml"
req.version = "1.1"

b = Binding(host="localhost",
            port=8080,
            url="/l5r",
            tracefile = sys.stdout)

def send(req, b):
    return b.RPC("/l5r",                       # Path
                 "searchRetrieveRequest",       # Request name
                 req,                           # Request object
                 requestclass=SRWTypes.SearchRetrieveRequest, # Serialization for Req.
                 replytype = SRWTypes.SearchRetrieveResponse.typecode) # Serialization for Resp
r = send(req, b)
print r.numberOfRecords
#print r.records[0].recordData

#req.recordPacking = "string"
#r = send(req, b)
#print r.records[0].recordData

#xpreq = SRWTypes.ExplainRequest('explainRequest', opts={'version': '1.1'})
#explain = b.RPC("/l5r", "explainRequest", xpreq, requestclass=SRWTypes.ExplainRequest, replytype=SRWTypes.ExplainResponse.typecode)
#print explain.record.recordData




# --- Interoperability Testing ---

# - IndexData -
#req2 = SRWTypes.searchRetrieveRequest('searchRetrieveRequest', init=1)
#req2.query = "dc.title any computer"
#b2 = Binding(host="indexdata.dk", port=8001, url="/marc", tracefile=sys.stdout)
#r = b2.RPC("/marc", "searchRetrieveRequest", req2, requestclass=SRWTypes.searchRetrieveRequest, replytype = SRWTypes.searchRetrieveResponse)


# - OCLC -
#req = SRWTypes.searchRetrieveRequest('searchRetrieveRequest', init=1)
#req.query = "education"
#req.maximumRecords = 1
#delattr(req, "recordSchema")
#b = Binding(host="alcme.oclc.org", port=80, tracefile=sys.stdout, ns="http://www.loc.gov/zing/srw/v1.0/", nsdict={'srw' : 'http://www.loc.gov/zing/srw/v1.0/'})
#r = b.RPC("/axis/SOAR/services/SearchRetrieveService", "searchRetrieveRequest", req, requestclass=SRWTypes.searchRetrieveRequest, replytype = SRWTypes.searchRetrieveResponse)


