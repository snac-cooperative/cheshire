#!../bin/webcheshire

set CHESHIRE_DATABASE ead_components
set CHESHIRE_CONFIGFILE DBCONFIGFILE
set CHESHIRE_RECSYNTAX grs1
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 20

set CHESHIRE_ELEMENTSET TPGRS

set query "search (id { cld-021000 } )"
set err [catch {eval $query} qresults]
set hits [lindex [lindex [lindex $qresults 0] 0] 1]

foreach rec [lrange $qresults 1 end] {

puts $rec

set relevance [lindex [lindex [lindex $rec 1] 1] 1]
set docid [lindex [lindex [lindex $rec 2] 1] 1]
set title [lindex [lindex [lindex $rec 3] 1] 1]
set dsc [lindex [lindex [lindex $rec 4] 1] 1]

puts "Relevance: $relevance\nDocid: $docid\nTitle: $title\nDSC: $dsc\n\n"
}

set CHESHIRE_ELEMENTSET CTPGRS

set query "search ( comp1subject @ medicine )"
set err [catch {eval $query} qresults]

set rec [lindex $qresults 1]
set relevance [lindex [lindex [lindex $rec 1] 1] 1]
set docid [lindex [lindex [lindex $rec 2] 1] 1]
set title [lindex [lindex [lindex $rec 3] 1] 1]
set unitid [lindex [lindex [lindex $rec 4] 1] 1]
set parenttitle [lindex [lindex [lindex $rec 5] 1] 1]

puts $rec

#puts "Relevance: $relevance\nDocid: $docid\nTitle: $title\nunitid: $unitid\nParentTitle: $parenttitle"

