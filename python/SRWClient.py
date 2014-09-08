#!/usr/bin/python

from ZSI.client import Binding
import SRWTypes
import sys

#req = SRWTypes.searchRetrieveRequest('searchRetrieveRequest', init=1)
#req.query = "cardname any/relevant \"ninja naga token\""
#req.maximumRecords = 1

#b = Binding(host="www.o-r-g.org", port=8080, url="/l5r/", tracefile=sys.stdout)
#r = b.RPC("/l5r/", "searchRetrieveRequest", req, requestclass=SRWTypes.searchRetrieveRequest, replytype = SRWTypes.searchRetrieveResponse)

#req2 = SRWTypes.searchRetrieveRequest('searchRetrieveRequest', init=1)
#req2.query = "dc.title any computer"
#b2 = Binding(host="indexdata.dk", port=8001, url="/marc", tracefile=sys.stdout)
#r = b2.RPC("/marc", "searchRetrieveRequest", req2, requestclass=SRWTypes.searchRetrieveRequest, replytype = SRWTypes.searchRetrieveResponse)

req = SRWTypes.searchRetrieveRequest('searchRetrieveRequest', init=1)
req.query = "education"
req.maximumRecords = 1
delattr(req, "recordSchema")

b = Binding(host="alcme.oclc.org", port=80, tracefile=sys.stdout, ns="http://www.loc.gov/zing/srw/v1.0/", nsdict={'srw' : 'http://www.loc.gov/zing/srw/v1.0/'})
r = b.RPC("/axis/SOAR/services/SearchRetrieveService", "searchRetrieveRequest", req, requestclass=SRWTypes.searchRetrieveRequest, replytype = SRWTypes.searchRetrieveResponse)

print r.numberOfRecords
