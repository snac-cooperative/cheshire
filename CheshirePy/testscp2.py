import sys
import CheshirePy as c

c.init('index/testconfig.new')
print ("Opened Cheshire config file %s" % c.showconfig())

c.setdb('bibfile')
print ("Set database to bibfile")

searchstr = 'topic @ mathematics programming'

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
