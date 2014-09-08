#!/usr/bin/python

# Create version 2 ZeeRex, using PyZ3950 for Explain

import os, sys, re, xml.dom.minidom, time
from PyZ3950.zoom import *

# Called with path to DBCONFIGFILE and databasename
if len(sys.argv) != 4:
    print "Usage:  ZeeRex.py <server init> <dbconfigfile> <database>"
    sys.exit()

try:
    host = os.environ['HOSTNAME']
except:
    # Called from admin.cgi, urgh.
    host = os.environ['SERVER_NAME']
    
zconf = sys.argv[1]
dbconfigfile = sys.argv[2]
database = sys.argv[3]

if (not os.path.exists(zconf)):
    print "Server Init file (%s) does not exist." % (zconf)
    sys.exit()
init = file(zconf).read()
portre = re.compile("PORT(\s)+(\d+)")
m = portre.search(init)
if (m):
    port = int(m.groups()[1])
else:
    port = 210

if (not os.path.exists(dbconfigfile)):
    print "Server Init file (%s) does not exist." % (dbconfigfile)
    sys.exit()
document = file(dbconfigfile).read()

databaseInfo = {}
targetInfo = {}
recsynarray = {"1.2.840.10003.5.105" : ['F'], "1.2.840.10003.5.101" : ['F'], "1.2.840.10003.5.109.9" : ['F'], "1.2.840.10003.5.109.10" : ['F']}

def attrname_to_num(name):
    bib1f = ['use', 'relation', 'position', 'structure', 'truncation', 'completeness']
    bib1s = ['use', 'relat', 'posit', 'struct', 'trunc', 'complet']
    aaf = ['access_point', 'semantic_qualifier', 'language', 'content_authority', 'expansion', 'normalized_weight', 'hit_count', 'comparison', 'format', 'occurence', 'indirection', 'functional_qualifier']
    aas = ['access', 'semantic', 'language', 'authority', '', 'weight', 'hits', '', '', '', '', 'functional']

    if name in bib1f:
        return bib1f.index(name) + 1
    elif name in bib1s:
        return bib1s.index(name) + 1
    elif name in aaf:
        return aaf.index(name) + 1
    elif name in aas:
        return aas.index(name) + 1
    else:
        return -1


def lowerElement(match):
    return match.groups()[0] + match.groups()[1].lower()

def attributeFix(match):
    fix = match.groups()[0].lower() + '="' + match.groups()[1] + '"'
    return fix

def getChildrenByName(node, name):
    nodes = []
    for c in node.childNodes:
        if c.nodeName == name:
            nodes.append(c)
    return nodes

def parse_index(idx):
    global database, indexidx
    
    indexidx += 1
    idxnames = [];
    maps = [];
    description = ""
    for c in idx.childNodes:
        if c.nodeName in ['indxtag', 'indextag']:
            idxnames.append(c.firstChild.data.strip())
        elif c.nodeName in ['indxmap', 'indexmap']:
            # Iter over each, look for attrset
            map = []
            globalattr = c.getAttribute('attributeset')
            globalattr = globalattr.lower()
            globalattr = globalattr.replace("-", "")

            for c2 in c.childNodes:
                if c2.nodeType == c2.ELEMENT_NODE:
                    value = c2.firstChild.data.strip()
                    if c2.nodeName == "description":
                        description = value;
                    else:
                        type = attrname_to_num(c2.nodeName)
                        attrset = c2.getAttribute('attributeset')

                        # Normalise
                        attrset = attrset.lower()
                        attrset = attrset.replace("-", "")

                        if str(attrset) != "":
                            attrset = " set=\"%s\"" % (attrset)
                        elif str(globalattr) != "":
                            attrset = " set=\"%s\"" % (globalattr)

                        map.append('<attr type="%s"%s>%s</attr>' % (type, attrset, value))
            maps.append(map)
    xmltxt = '    <index search="true" scan="true" sort="true" id="DISTRIB-%s-%s">\n      <title>%s</title>\n' % (database, indexidx, description)
    for map in maps:
        xmltxt += "      <map>\n"
        for m in map:
            xmltxt += "        " + m + "\n"
        xmltxt += "      </map>\n"
        
    for n in idxnames:
        xmltxt += "      <map>\n        <attr type=\"1\" set=\"bib1\">%s</attr>\n      </map>\n" % (n)

    xmltxt += "    </index>\n"

    return xmltxt
        

def parse_display(node):
    global recsynarray

    name = node.getAttribute('name')
    oid = node.getAttribute('oid')
    if recsynarray.has_key(str(oid)):
        esn = recsynarray[str(oid)]
        esn.append(name)
        recsynarray[str(oid)] = esn

    

def search_dbconfigfile(dbconfigfile, database):
    global recsynarray, indexes

    if (not os.path.exists(dbconfigfile)):
        print "File does not exist: " + dbconfigfile
        sys.exit()
    else:
        inh = file(dbconfigfile);
        data = inh.read()
        inh.close()

        xmltext = ""

        # Fix SGML into XML. Yummy tcl portage. Quote all attrs, lowercase all elements, DEFAULT -> default="true".  No empty elems in configfile.dtd

        attr_re = re.compile('([a-zA-Z0-9_]+)[ ]*=[ ]*["]?([-:_.a-zA-Z0-9]+)["]?')
        data = attr_re.sub(attributeFix, data);
        attr_elem = re.compile('(<[/]?)([a-zA-Z0-9_]+)')
        data = attr_elem.sub(lowerElement, data)
        
        data = data.replace('DEFAULT>', 'default="true">')
        data = data.replace('default>', 'default="true">')

        dom = xml.dom.minidom.parseString(data)
        fdefs = dom.getElementsByTagName("filedef")
        fdtype = ""
        foundDB = 0
        for fd in fdefs:
            tempfdtype = fd.getAttribute('type')
            for child in fd.childNodes:
                
                if child.nodeName == "filetag" and child.firstChild.data.strip() == database:
                    foundDB = 1
                    fdtype = tempfdtype
                elif child.nodeName == "filetag":
                    break
                elif foundDB and child.nodeName == "indexes":
                    for idx in child.childNodes:
                        if idx.nodeName in ['index', 'indexdef']:
                            xmltext = parse_index(idx)
                            if xmltext != "":
                                indexes.append(xmltext)
                elif foundDB and child.nodeName in ['display', 'displays']:
                    for disp in child.childNodes:
                        if disp.nodeName in ['displaydef', 'format']:
                            parse_display(disp)

        if fdtype in ['MARCSGML', 'MARCSGML_DATASTORE']:
            recsynarray['1.2.840.10003.5.10'] = ['F']


def get_text(humanStr):
    hash = {}
    for t in humanStr:
        hash[t.language] = t.text

    keys = hash.keys()
    if keys == []:
        return ""
    elif 'eng' in keys:
        return hash['eng']
    else:
        return hash[keys[0]]

def search_explain(database):
    # Search databaseInfo for interesting bits.
    # In particular, recordCount and lastUpdate

    global port

    c = Connection('localhost', port)
    c.databaseName = "IR-Explain-1"
    c.preferredRecordSyntax = 'EXPLAIN'

    q = Query('PQF', '@attrset EXP1 @and @attr 1=1 databaseInfo @attr 1=3 %s' % (database))
    rs = c.search(q)

    if len(rs) == 1:
        rec = rs[0].data[1]
        databaseInfo['lastUpdate'] = rec.lastUpdate.strip()
        databaseInfo['recordCount'] = rec.recordCount[1]
	
        # Need to check presence
        try:
            databaseInfo['title'] = get_text(rec.titleString)
        except:
            pass
        try:
            databaseInfo['description'] = get_text(rec.description)
        except:
            pass
        try:
            databaseInfo['coverage'] = get_text(rec.coverage)
        except:
            pass
        try:
            databaseInfo['restrictions'] = get_text(rec.copyrightText)
        except:
            pass
	
    else:
        raise(ValueError)
    
    return databaseInfo


indexidx = 0
indexes = []
search_dbconfigfile(dbconfigfile, database)
databaseInfo = search_explain(database)

try:
    timelist = time.strptime(databaseInfo['lastUpdate'])
except:
    timelist = time.strptime(time.ctime())

isotime = "%i-%i-%iT%i:%i:%i" % timelist[0:6]

nowisotime = "%i-%i-%iT%i:%i:%i" % time.strptime(time.ctime())[0:6]

print "<explain authoritative=\"false\" id=\"DISTRIB-%s\">" % database
print '  <serverInfo protocol="Z39.50" version=\"3\">'
print '    <host>%s</host>' % host
print '    <port>%i</port>' % port
print '    <database numrecs="%i" lastUpdate="%s">%s</database>' % (databaseInfo['recordCount'], isotime, database)
print '  </serverInfo>'
print '  <databaseInfo>'
if databaseInfo.has_key('title'):
    print '    <title>%s</title>' % databaseInfo['title']
if databaseInfo.has_key('description'):
    print '    <description>%s</description>' % databaseInfo['description']
if databaseInfo.has_key('coverage'):
    print '    <coverage>%s</coverage>' % databaseInfo['coverage']
if databaseInfo.has_key('restrictions'):
    print '    <restrictions>%s</restrictions>' % databaseInfo['restrictions']
print '  </databaseInfo>'
print '  <metaInfo>'
print '    <dateModified>%s</dateModified>' % nowisotime
print '  </metaInfo>'
print '  <indexInfo>'
for i in indexes:
    print i
print '  </indexInfo>'
print '  <recordInfo>'
for oid in recsynarray.keys():
    print '    <recordSyntax oid="%s">' % oid
    for esn in recsynarray[oid]:
        print '      <elementSet name="%s"></elementSet>' % esn
    print '    </recordSyntax>'
print '  </recordInfo>'
print '</explain>'
