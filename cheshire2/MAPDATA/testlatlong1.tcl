#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5

# --- Test phrase searching

set query "search lldec 0065.683296 -017.933300"


set results [eval $query]
puts "TEST === Northernmost ===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

set query "search lldec -44.133301 170.500000"


set results [eval $query]
puts "TEST === Southernmost ===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

