#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

set query "search bboxdeg +-+ 42.3833 -122.41 29.4167 -74"


set results [eval $query]
puts "TEST ======== $query"

foreach rec $results {
    puts $rec
    puts ""
}

