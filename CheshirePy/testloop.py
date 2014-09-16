import sys
import CheshirePy as c

if c.init('../index/testconfig.new') == 0:
    print ("Opened Cheshire config file %s" % c.showconfig())
else:
    print("failed to open configuration file - exiting")
    exit()
    
if c.setdb('bibfile') == 0:
    print ("Set database to bibfile")
else:
    print("failed to set database")
    exit()

searchstrs = ['topic @ mathematics programming', 'su programming', 'title mathematics', 'topic @ geometry calculus', 'topic @ calculus', 'title probability']

for searchstr in searchstrs:

    print ("searching for '%s'" % searchstr)

    r = c.Search(searchstr)

    n = c.getnumfound(r)
    print ("%d records found" % n)

    i = 0

    while i < n :
        rec = c.getrecord(r,i)
        rel = c.getrelevance(r,i)
        print("*********************************************")
        print("Rank: %d Relevance: %f" % (i, rel))
        print(rec)
        print("*********************************************")
        i += 1

    c.closeresult(r)

print("End of record set")

exit()
