#!/usr/bin/python

from ChZoom import *
from zmarc import *
import sys

c = Connection('lib.liv.ac.uk', 210, databaseName='innopac', namedResultSets=0)
c.recordSyntax = "MARC"
q = Query('author smith')


rs = c.search(q)
for x in range(len(rs)):
    marc =  rs[x].get_raw()
    mr = MARC(marc)
    print mr.toSGML()
    if not x % 100:
        sys.stderr.write("... ")
