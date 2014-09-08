#!/usr/bin/env python
# XXX spiff up diagnostic reporting!!!

# This file should be available from
# http://www.pobox.com/~asl2/software/PyZ3950/
# and is licensed under the X Consortium license:
# Copyright (c) 2001, Aaron S. Lav, asl2@pobox.com
# All rights reserved. 

# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, and/or sell copies of the Software, and to permit persons
# to whom the Software is furnished to do so, provided that the above
# copyright notice(s) and this permission notice appear in all copies of
# the Software and that both the above copyright notice(s) and this
# permission notice appear in supporting documentation. 

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
# OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
# INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
# FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
# NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
# WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

# Except as contained in this notice, the name of a copyright holder
# shall not be used in advertising or otherwise to promote the sale, use
# or other dealings in this Software without prior written authorization
# of the copyright holder. 

# Change history:
# 2002/05/23
# Fix for Python2 compatibility.  Thanks to Douglas Bates <bates@stat.wisc.edu>
# Fix to support SUTRS (requires asn1 updates, too)
# 2002/05/28
# Make SUTRS printing a little more useful
# Correctly close connection when done
# Handle receiving diagnostics instead of records a little better

"""<p>PyZ3950 currently is capable of sending and receiving v2 PDUs
Initialize, Search, Present, Scan, Close, and Delete.  For client
work, you probably want to use ZOOM, which should be in the same
distribution as this file, in zoom.py.  The Server class in this file
implements a server, but could use some work.  Both interoperate with
the <a href="http://www.indexdata.dk/yaz"> Yaz toolkit</a> and the
client interoperates with a variety of libraries.  <p>

Useful resources:
<ul>
<li><a href="http://lcweb.loc.gov/z3950/agency/">
Library of Congress Z39.50 Maintenance Agency Page</a></li>
<li><a href="http://lcweb.loc.gov/z3950/agency/document.html">
Official Specification</a></li>
<li><a href="http://www.loc.gov/z3950/agency/clarify/">Clarifications</a></li>
</ul>
"""

from __future__ import nested_scopes
import getopt
import sys
import exceptions
import random
import socket
import string
import traceback

from PyZ3950 import asn1
from PyZ3950 import charneg
from PyZ3950.z3950 import *

trace_recv = 0
print_hex = 0
vers = '0.62'
default_resultSetName = 'default'
DEFAULT_PORT = 2101

def make_origin_prop ():
    iso10646 = charneg.Iso10646 ()
    iso10646.encodingLevel = asn1.OidVal ([1,0,10646,1,0,8])
    origin_prop = charneg.OriginProposal ()
    origin_prop.proposedCharSets = [
        ('iso10646', iso10646)]
    origin_prop.recordsInSelectedCharSets = 1
    return asn1.encode (charneg.OriginProposal, origin_prop).tostring ()

msg_size = 0x10000

def make_initreq (optionslist = None, authentication = None):
    negotiate_charset = 0
    # see http://lcweb.loc.gov/z3950/agency/wisdom/unicode.html
    InitReq = InitializeRequest ()
    InitReq.protocolVersion = Version (Z3950_VERS == 3)
    InitReq.options = OptionsVal ()
    if optionslist == None: optionslist = []
    optionslist.append (OptionsVal.delSet)
    optionslist.append (OptionsVal.scan)
    if negotiate_charset:
        optionslist.append (OptionsVal.negotiation)
    InitReq.options.set_bits (tuple (optionslist))
# Preferred and Exceptional msg sizes are pretty arbitrary --
# we dynamically allocate no matter what
    InitReq.preferredMessageSize = msg_size
    InitReq.exceptionalRecordSize = msg_size
    InitReq.implementationId = 'PyZ3950 - contact asl2@pobox.com'
    # haven't been assigned an official id # XXX
    InitReq.implementationName = 'PyZ3950'
    InitReq.implementationVersion = "%s (asn.1 %s)" % (vers, asn1.vers)
    if authentication <> None:
        up = UserPass ()
        upAttrList = ['userId', 'password', 'groupId']
        for val, attr in zip (authentication, upAttrList): # silently truncate
            if val <> None:
                setattr (up, attr, val)
        InitReq.idAuthentication = ('idPass', up)
    if negotiate_charset:
        origin_prop_data = make_origin_prop ()
        negot = asn1.EXTERNAL ()
        negot.direct_reference = asn1.OidVal (Z3950_NEG_CHARSET3)
        negot.encoding= ('single-ASN1-type', origin_prop_data)
        oi_elt = OtherInfoElt ()
        oi_elt.information = ('externallyDefinedInfo', negot)
        other_info = [oi_elt]
        if 1:
            InitReq.otherInfo = other_info
        else:
            ui = asn1.EXTERNAL ()
            ui.direct_reference = asn1.OidVal (Z3950_USR_INFO1)
            ui.encoding = ('single-ASN1-type', asn1.encode (OtherInformation,
                                                         other_info).tostring ())
            # see http://lcweb.loc.gov/z3950/agency/defns/user-1.html
            # for use of OtherInformation in userInformationField
            InitReq.userInformationField = ui
    return InitReq

def make_sreq (query, dbnames, rsn, **kw):
    sreq = SearchRequest ()
    sreq.smallSetUpperBound = 0
    sreq.largeSetLowerBound = 1
    sreq.mediumSetPresentNumber = 0
# as per http://lcweb.loc.gov/z3950/lcserver.html, Jun 07 2001,
# to work around Endeavor bugs in 1.13
    sreq.replaceIndicator = 1
    sreq.resultSetName = rsn
    sreq.databaseNames = dbnames
    sreq.query = query
    for (key, val) in kw.items ():
        setattr (sreq, key, val)
    return sreq


class parse_marc:
    def __init__ (self, s):
        self.marc = s
        print "Rec len", self.extract_int (0,4)
        print "Status:", self.marc[5]
        print "Type:", self.marc[6]
        print "Bibliographic level:", self.marc[7]
        print "Type of control:", self.marc[8]
        assert (self.marc[9] == ' ') # 'a' would be UCS/Unicode
        assert (self.marc[10] == '2' and self.marc[11] == '2')
        baseaddr = self.extract_int (12, 16)
        print "Encoding level:", self.marc[17]
        print "Descriptive cataloging form:", self.marc[18]
        print "Linked record requirement:", self.marc[19]
        assert (self.marc[20:22] == '45')
        pos = 24
        while pos < baseaddr:
            tag = self.marc[pos:pos+3]
            if tag [0] == '\035' or tag [0] == '\036':
                break
            fieldlen = self.extract_int (pos + 3, pos + 6)
            startpos = self.extract_int (pos + 7, pos + 11)
            pos = pos + 12
            line = self.marc[baseaddr + startpos:
                             baseaddr + startpos + fieldlen]
            printline = string.replace (line, '\037', '$')
# Warning: this replacement is for display purposes only!  '$' is perfectly
# valid within a subfield!
            if printline == '':
                print "Empty tag", tag
                continue
            while printline[-1] == '\036' or printline [-1] == '\035':
                printline = printline [:-1]
            printline = string.replace (printline, '\035', '~')
            printline = string.replace (printline, '\036', '^')
            print tag, printline


    def extract_int (self, start, end):
        return string.atoi (self.marc[start:end+1])

def extract_recs (resp):
    (typ, recs) = resp.records
    if (typ <> 'responseRecords'):
        raise ProtocolError ("Bad records typ " + str (typ) + str (recs))
    if len (recs) == 0:
        raise ProtocolError ("No records")
    fmtoid = None
    extract = []
    for r in recs:
        (typ, data) = r.record
        if (typ <> 'retrievalRecord'):
            raise ProtocolError ("Bad typ %s data %s" % (str (typ), str(data)))
        oid = data.direct_reference
        if fmtoid == None:
            fmtoid = oid
        elif fmtoid <> oid:
            raise ProtocolError (
                "Differing OIDs %s %s" % (str (fmtoid), str (oid)))
        # Not, strictly speaking, an error.
        dat = data.encoding
        (typ, dat) = dat
        if (oid == asn1.OidVal (Z3950_RECSYN_USMARC)):
            if typ <> 'octet-aligned':
                raise ProtocolError ("Weird record EXTERNAL MARC type: " + typ)
        extract.append (dat)
    return (fmtoid, extract)

def get_formatter (oid):
    def printer (x): print oid, x
    if oid == asn1.OidVal (Z3950_RECSYN_USMARC):
        formatter = parse_marc
    else: # SUTRS and who knows
        formatter = printer
    return formatter

def disp_resp (resp):
    try:
        (fmtoid, recs) = extract_recs (resp)
    except ProtocolError, val:
        print "Bad records", str (val)
    formatter = get_formatter (fmtoid)
    for rec in recs:
        formatter (rec)

class Conn:
    rdsz = 65536
    def __init__ (self, sock = None):
        if sock == None:
            self.sock = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock
        self.ctx = asn1.Ctx (asn1.Ctx.dir_read)
        def readproc (s = self.sock, rdsz = self.rdsz):
            try:
                b = s.recv (rdsz)
            except socket.error, val:
                raise ConnectionError ('socket', str (val))
            if len (b) == 0: # graceful close
                raise ConnectionError ('graceful close')
            if trace_recv:
                print map (lambda x: hex(ord(x)), b)
            return b
        self.ctx.set_data ("")
        self.ctx.set_readproc (readproc)
    def read_PDU (self):
        try:
            recv = PDU.decode (self.ctx)
        except asn1.BERError, val:
            raise ProtocolError ('ASN1 BER', str(val))
        self.ctx.reset_read ()
        return recv
    
class Server (Conn):
    test = 0
    def __init__ (self, sock):
        Conn.__init__ (self, sock)
        self.expecting_init = 1
        self.done = 0
        self.result_sets = {}
        while not self.done:
            (typ, val) = self.read_PDU ()
            fn = self.fn_dict.get (typ, None)
            if fn == None:
                raise ProtocolError ("Bad typ", typ + " " + str (val))
            # XXX should send close if OK, then close socket
            if typ <> 'initRequest' and self.expecting_init:
                raise ProtocolError ("Init expected", typ)
            fn (self, val)
    def send (self, val):
        b = asn1.encode (PDU, val)
        if self.test:
            print "Internal Testing"
            # a reminder not to leave this switched on by accident
            decoded = asn1.decode (PDU, b)
            assert (val== decoded)
        self.sock.send (b)

    def do_close (self, reason, info):
        close = Close ()
        close.closeReason = reason
        close.diagnosticInformation = info
        self.send (('close', close))

    def close (self, parm):
        self.done = 1
        self.do_close (0, 'Normal close')
        
    def search_child (self, query):
        return range (random.randint (2,10))
    def search (self, sreq):
        if sreq.replaceIndicator == 0 and self.result_sets.has_key (
            sreq.resultSetName):
            raise ProtocolError ("replaceIndicator 0")
        # XXX spec says to reject politely
        # Should map search across sreq.databaseNames
        result = self.search_child (sreq.query)
        self.result_sets[sreq.resultSetName] = result
        sresp = SearchResponse ()
        sresp.resultCount = len (result)
        sresp.numberOfRecordsReturned = 0
        sresp.nextResultSetPosition = 1
        sresp.searchStatus = 1
        sresp.resultSetStatus = 0
        sresp.presentStatus = 0
        sresp.records = ('responseRecords', [])
        self.send (('searchResponse', sresp))
    def format_records (self, start, count, res_set, prefsyn):
        l = []
        sutrs = asn1.OidVal (Z3950_RECSYN_SUTRS)
        for i in range (start - 1, start + count - 1):
            elt = res_set[i]
            elt_external = asn1.EXTERNAL ()
            elt_external.direct_reference = sutrs
# See Z39.50 clarification XXX - we really want GeneralString even when v2
# is in force, although sending characters in the GeneralString but not
# VisibleString repertoire might be bad.

            elt_external.encoding = ('octet-aligned',
                                     asn1.encode (asn1.GeneralString,
                                                  'a silly placeholder %d' % (
                elt)).tostring ())
            n = NamePlusRecord ()
            n.name = 'foo'
            n.record = ('retrievalRecord', elt_external)
            l.append (n)
        return l
        
    def present (self, preq):
        presp = PresentResponse ()
        res_set = self.result_sets [preq.resultSetId]
        presp.numberOfRecordsReturned = preq.numberOfRecordsRequested
        presp.nextResultSetPosition = preq.resultSetStartPoint + \
                                      preq.numberOfRecordsRequested
        presp.presentStatus = 0
        presp.records = ('responseRecords',
                         self.format_records (preq.resultSetStartPoint,
                                              preq.numberOfRecordsRequested,
                                              res_set,
                                              preq.preferredRecordSyntax))
        self.send (('presentResponse', presp))
        
    def init (self, ireq):
        ir = InitializeResponse () # Should negotiate
        ir.protocolVersion = Version (Z3950_VERS == 3)
        ir.options = OptionsVal ()
        ir.preferredMessageSize = 0x10000 
        ir.exceptionalRecordSize = 0x10000
        ir.implementationId = 'PyZ39.50 - contact asl2@pobox.com'
        # haven't been assigned an official id #
        ir.implementationName = 'Test server'
        ir.implementationVersion = "%s (asn.1 %s)" % (vers, asn1.vers)
        ir.result = 1
        self.expecting_init = 0
        self.send (('initResponse', ir))
    def sort (self, sreq):
        print sreq
        sresp = SortResponse ()
        sresp.sortStatus = 0
        self.send (('sortResponse', sresp))
        
    fn_dict = {'searchRequest': search,
               'presentRequest': present,
               'initRequest' : init,
               'close' : close,
               'sortRequest' : sort}
        


def run_server ():
    listen = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
    listen.setsockopt (socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listen.bind (('', DEFAULT_PORT))
    listen.listen (1)
    while 1:
        (sock,addr) = listen.accept ()
        try:
            serv = Server (sock)
        except:
            (typ, val, tb) = sys.exc_info ()
            if typ == exceptions.KeyboardInterrupt:
                print "kbd interrupt, leaving"
                raise
            print "error %s %s from %s" % (typ, val, addr)
            traceback.print_exc(40)
        sock.close ()
        
class ConnectErr (Exception):
    pass

def extract_apt (rpnQuery):
    """Takes RPNQuery to AttributePlusTerm"""
    RPNStruct = rpnQuery.rpn
    assert (RPNStruct [0] == 'op')
    operand = RPNStruct [1]
    assert (operand [0] == 'attrTerm')
    return operand [1]


class Client (Conn):
    test = 0
    def __init__ (self, addr, port = DEFAULT_PORT, optionslist = None,
                  authentication = None):
        Conn.__init__ (self)
        try:
            self.sock.connect ((addr, port))
        except socket.error, val:
            raise ConnectionError ('socket', str(val))
        InitReq = make_initreq (optionslist, authentication = authentication)
        self.initresp = self.transact (
            ('initRequest', InitReq), 'initResponse')
        self.search_results = {}
        self.max_to_request = 20
        self.default_recordSyntax = asn1.OidVal (Z3950_RECSYN_USMARC)
    def get_option (self,option):
        return self.initresp.options.is_set (option)
    def transact (self, to_send, expected):
        b = asn1.encode (PDU, to_send)
        if print_hex:
            print map (hex, b)
        if self.test:
            print "Internal Testing"
            # a reminder not to leave this switched on by accident
            decoded = asn1.decode (PDU, b)
            assert (to_send == decoded)
        self.sock.send (b)
        if expected == None:
            return
        (arm, val) = self.read_PDU ()
        if self.test:
            print "Internal Testing 2"
            b = asn1.encode (PDU, (arm, val))
            redecoded = asn1.decode (PDU, b)
            assert (redecoded == (arm, val))
        if arm == expected: # may be 'close'
            return val
        elif arm == 'close':
            raise UnexpectedCloseError (
                "Server closed connection reason %d diag info %s" % \
                (getattr (val, 'closeReason', -1),
                 getattr (val, 'diagnosticInformation', 'None given')))
        else:
            raise ProtocolError (
                "Unexpected response from server %s %s " % (expected,
                                                            repr ((arm, val))))
    def set_dbnames (self, dbnames):
        self.dbnames = dbnames
    def search_2 (self, query, rsn = default_resultSetName, **kw):
        if 0 == self.initresp.options.is_set (OptionsVal.search):
            print "search not supported" # XXX way bad!
            # Carry on anyway: maybe the init response was lying.
        sreq = make_sreq (query, self.dbnames, rsn, **kw)
        recv = self.transact (('searchRequest', sreq), 'searchResponse')
        self.search_results [rsn] = recv
        return recv
    def search (self, query, rsn = default_resultSetName, **kw):
        # for backwards compat
        recv = self.search_2 (('type-1', query), rsn, **kw)
        return recv.searchStatus and (recv.resultCount > 0)
    # If searchStatus is failure, check result-set-status - 
    # -subset - partial, valid results available
    # -interim - partial, not necessarily valid
    # -none - no result set
    # If searchStatus is success, check present-status:
    # - success - OK
    # - partial-1 - not all, access control
    # - partial-2 - not all, won't fit in msg size (but we currently don't ask for
    #               any records in search, shouldn't happen)
    # - partial-3 - not all, resource control (origin)
    # - partial-4 - not all, resource control (target)
    # - failure - no records, nonsurrogate diagnostic.
    def get_count (self, rsn = default_resultSetName):
        return self.search_results[rsn].resultCount
    def delete (self, rsn):
        if 0 == self.initresp.options.is_set (OptionsVal.delSet):
            return None
        delreq = DeleteResultSetRequest ()
        delreq.deleteFunction = 0 # list
        delreq.resultSetList = [rsn]
        return self.transact (('deleteResultSetRequest', delreq),
                              'deleteResultSetResponse')
    def present (self, rsn= default_resultSetName, start = None,
                 count = None, recsyn = None, esn = None):
        if 0 == self.initresp.options.is_set (OptionsVal.present):
            print "present not supported" # XXX way bad!
            # but carry on anyway.
        sresp = self.search_results [rsn]
        if start == None:
            start = sresp.nextResultSetPosition
        if count == None:
            count = sresp.resultCount
            if self.max_to_request > 0:
                count = min (self.max_to_request, count)
        if recsyn == None:
            recsyn = self.default_recordSyntax
        preq = PresentRequest ()
        preq.resultSetId = rsn
        preq.resultSetStartPoint = start
        preq.numberOfRecordsRequested = count
        preq.preferredRecordSyntax = recsyn
        if esn <> None:
            esn2 = ElementSetNames ()
            esn2.foo = esn
            preq.recordComposition = esn2
        return self.transact (('presentRequest', preq), 'presentResponse')
    def scan (self, query, **kw):
        sreq = ScanRequest ()
        sreq.databaseNames = self.dbnames
        assert (query[0] == 'type-1' or query [0] == 'type-101')
        sreq.attributeSet = query[1].attributeSet
        sreq.termListAndStartPoint = extract_apt (query[1])
        sreq.numberOfTermsRequested = 20 # default
        for (key, val) in kw.items ():
            setattr (sreq, key, val)

        return self.transact (('scanRequest', sreq), 'scanResponse')
    def close (self):
        close = Close ()
        close.closeReason = 0
        close.diagnosticInformation = 'Normal close'
        try:
            rv =  self.transact (('close', close), 'close')
        except ConnectionError:
            rv = None
            pass
        self.sock.close ()
        return rv

bib1 = asn1.OidVal (Z3950_ATTRS_BIB1)

def mk_compound_query ():
    aelt1 = AttributeElement (attributeType = 1, attributeValueInteger = 4)
    apt1 = apt ()
    apt1.attributes = [aelt1]
    apt1.term = '1066' 
    aelt2 = AttributeElement (attributeType = 1, attributeValueInteger = 1)
    apt2 = apt ()
    apt2.attributes = [aelt2]
    apt2.term = 'Sellar'
    myrpnRpnOp = rpnRpnOp ()
    myrpnRpnOp.rpn1 = ('op', ('attrTerm', apt1))
    myrpnRpnOp.rpn2 = ('op', ('attrTerm', apt2))
    myrpnRpnOp.op = ('and', None)
    rpnq = RPNQuery (attributeSet = bib1)
    rpnq.rpn = ('rpnRpnOp', myrpnRpnOp)
    return rpnq

def mk_simple_query (title):
    aelt1 = AttributeElement (attributeType = 1, attributeValueInteger = 1003)
    apt1 = apt ()
    apt1.attributes = [aelt1]
    apt1.term = title
    rpnq = RPNQuery (attributeSet = bib1)
    rpnq.rpn = ('op', ('attrTerm', apt1))
    return rpnq

def_host = 'LC'

host_dict = {'BIBSYS': ('z3950.bibsys.no', 2100, 'BIBSYS'),
             'YAZ': ('127.0.0.1', 9999, 'foo'),
             'LCTEST' :  ('ilssun2.loc.gov', 7090, 'Voyager'),
             'LC' : ('z3950.loc.gov', 7090, 'Voyager'),
             'NLC' : ('amicus.nlc-bnc.ca', 210, 'NL'),
             'BNC' : ('amicus.nlc-bnc.ca', 210, 'NL'),
             # On parle franc,ais aussi.
             'LOCAL': ('127.0.0.1', 9999, 'Default'),
             'LOCAL2': ('127.0.0.1', 2100, 'foo'),
             'BL' :('blpcz.bl.uk', 21021, 'BLPC-ALL'),
             'BELLLABS' : ('z3950.bell-labs.com', 210, 'books'),
             'BIBHIT' : ('www.bibhit.dk', 210, 'Default'),
             'YALE': ('webpac.library.yale.edu', 210, 'YALEOPAC'),
             'OXFORD': ('library.ox.ac.uk', 210, 'ADVANCE'),
             'OVID': ('z3950.ovid.com', 2213, 'pmed'), # scan only
             'UC':   ('ipac.lib.uchicago.edu', 210, 'uofc'),
             'KUB' : ('dbiref.kub.nl', 1800, 'jel'),
             'INDEXDATA' : ('muffin.indexdata.dk', 9004, 'thatt')}
# last two are Zthes servers.

if __name__ == '__main__':
    optlist, args = getopt.getopt (sys.argv[1:], 'sh:')
    server = 0
    host = def_host
    for (opt, val) in optlist:
        if opt == '-s':
            server = 1
        elif opt == '-h':
            host = val
    if server:
        run_server ()

    host = host.upper ()
    (name, port, dbname) = host_dict.get (host, host_dict[def_host])
    cli = Client (name, port)
    cli.set_dbnames ([dbname])
    print "Starting search"
#    rpnq = mk_simple_query ('Perec, Georges')
    rpnq = mk_simple_query ('Johnson, Kim')
#    rpnq = mk_compound_query ()
    if cli.search (rpnq, smallSetUpperBound = 0, mediumSetPresentNumber = 0,
                   largeSetLowerBound = 1):
        disp_resp (cli.present (recsyn = asn1.OidVal(Z3950_RECSYN_USMARC)))
    else:
        print "Not found"
    print "Deleting"
    print cli.delete (default_resultSetName)
    print cli.delete ('bogus')
    print "Closing"
    try:
        cli.close ()
    except ConnectionError:
        # looks like LC, at least, sends a FIN on receipt of Close PDU
        # guess we should check for gracefullness of close, and complain
        # if not.
        pass

