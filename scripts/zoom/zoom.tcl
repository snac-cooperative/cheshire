#!/home/cheshire/cheshire/bin/webcheshire

source cTCL.tcl
source baseZoomObject.tcl
source record.tcl
source connection.tcl
source resultset.tcl
source query.tcl

# ------ Test Script -------

# Connect and set Options
object Connection mail gondolin.hist.liv.ac.uk 210
mail.setOption "database" "mailarchive"
mail.setOption "preferredRecordSyntax" "SGML"

# Create 2 queries, one with explicit RSN
object Query robq 0 "zfind 1016 rob"

object Query cheshq 0 "zfind 1016 cheshire"
cheshq.setOption "resultsetname" "foo"

# Perform Searches
set rset1 [mail.search [robq]]
set rset2 [mail.search [cheshq]]

# Retrieve GRS1/TPGRS record
$rset1.setOption "preferredRecordSyntax" "GRS1"
$rset1.setOption "elementSet" "TPGRS"
set rec [$rset1.getRecord 0]

# Dump to User
puts "Got Record:\n[$rec.getRaw]"

# Clean up objects
delete [mail]
delete [robq]
delete [cheshq]
