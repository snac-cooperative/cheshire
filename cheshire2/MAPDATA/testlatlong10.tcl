#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

set query "search citylldec +-+ 6.78333 -9.96667 4.41667 -6.28333"


set results [eval $query]
puts "TEST ======== $query"

foreach rec $results {
    puts $rec
    puts ""
}

