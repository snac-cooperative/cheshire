#!/usr/local/bin/python2.3 -i

import time, sys, os
from server import SimpleServer
from documentGroup import FileDocumentGroup, ClusterDocumentGroup
from PyZ3950.CQLParser import parse
from document import StringDocument

# Build environment...
session = None
serv = SimpleServer("../configs/serverConfig.xml")

# Discover objects...
l5r = serv.get_object(None, 'db_l5r')
recStore = l5r.get_object(None, 'l5rRecordStore')
pp = l5r.get_object(None, 'SgmlPreParser')
sax = l5r.get_object(None, 'SaxParser')
rss = serv.get_object(session, 'defaultResultSetStore')
ast = serv.get_object(session, 'defaultAuthStore')

id = -1

# Create a user
if ('-user' in sys.argv):
    f = file("/home/cheshire/c3/new/configs/azarothUser.xml")
    data = f.read()
    f.close()
    sd = StringDocument(data)
    rec = sax.process_document(session, sd)
    rec.id = "azaroth"
    ast.store_record(session, rec)
    ast.close(session)

# Load recordStore
if ('-load' in sys.argv):
    dg = FileDocumentGroup('/home/cheshire/c3/new/l5r/cardlist.sgml')
    total = dg.get_length(session)
    print "Loading %d..." % (total)
    for d in range(total):
        doc = dg.get_document(session, d)
        doc = pp.process_document(session, doc)
        try:
            rec = sax.process_document(session, doc)
        except:
            print doc.get_raw()
            sys.exit()
        id = recStore.create_record(session, rec)
        l5r.add_record(session, rec)
        if not id % 100:
            print "... %d" % (id)
        del rec
    dg2 = FileDocumentGroup('/home/cheshire/c3/new/l5r/cardlist2.sgml')
    total = dg2.get_length(session)
    print "Loading %d..." % (total)
    for d in range(total):
        doc = dg2.get_document(session, d)
        doc = pp.process_document(session, doc)
        try:
            rec = sax.process_document(session, doc)
        except:
            print doc.get_raw()
            sys.exit()
        id = recStore.create_record(session, rec)
        l5r.add_record(session, rec)
        if not id % 100:
            print "... %d" % (id)
        del rec
    recStore.close(session)
    l5r.commit_metadata(session)

# Load indexes
if ('-index' in sys.argv):
    print "Indexing..."
    l5r.begin_indexing(session)
    if (id == -1):
        id = l5r.totalRecords -1

    for d in range(id+1):
        rec = recStore.fetch_record(session, d)
        l5r.handle_indexRecord(session, rec)
        if not d % 100:
            print "... %d" % (d)
        del rec
    print "Committing..."
    l5r.commit_indexing(session)

# Create and execute search ...
if ('-search' in sys.argv):
    cl = parse('dc.title all oath')
    print time.time()
    result = l5r.handle_search(None, cl)
    print time.time()
    print "Search found %i matches..." % (len(result))

    if (len(result)):
        idx = l5r.indexes['l5r-idx-1']
        result.order(None, idx)
        rPtr = result[0].docid
        rec = recStore.fetch_record(session, rPtr)
        txr = l5r.get_object(session, 'l5rDublinCoreTxr')
        xml = txr.process_record(session, rec)
        rsid = rss.create_resultSet(session, result)

if ('-clusterLoad' in sys.argv or '-clusterIndex' in sys.argv or '-cluster' in sys.argv):
    ctxr = l5r.get_path(session, "onIndexRecord")
    cdb = ctxr.get_path(session, "database")
    crs = cdb.get_object(session, "l5rClusterRecordStore")

    if ('-clusterLoad' in sys.argv):
        if (ctxr):
            path = ctxr.get_path(session, "tempPath")
            if (not os.path.isabs(path)):
                dfp = cdb.get_path(session, "defaultPath")
                path = os.path.join(dfp, path)       
            cdg = ClusterDocumentGroup(path)
            # Now build database
            total = cdg.get_length(session)
            print "Loading %d..." % (total)
            for d in range(total):
                doc = cdg.get_document(session, d)
                try:
                    rec = sax.process_document(session, doc)
                except:
                    print doc.get_raw()
                    sys.exit()
                id = crs.create_record(session, rec)
                cdb.add_record(session, rec)
                if not id % 100:
                    print "... %d" % (id)
                del rec
            crs.close(session)
            cdb.commit_metadata(session)

            # Load indexes
    if ('-clusterIndex' in sys.argv):
        print "Indexing..."
        cdb.begin_indexing(session)
        if (id == -1):
            id = cdb.totalRecords -1
        for d in range(id+1):
            rec = crs.fetch_record(session, d)
            cdb.handle_indexRecord(session, rec)
            if not d % 100:
                print "... %d" % (d)
            del rec
        print "Committing..."
        cdb.commit_indexing(session)

        
