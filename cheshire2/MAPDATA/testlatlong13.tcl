#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

set query "search bboxstr  +-+ {42o22'59''N 122o24'36''W 29o25'0''N 74o0'0''W}"


set results [eval $query]
puts "TEST ======== $query"

foreach rec $results {
    puts $rec
    puts ""
}

