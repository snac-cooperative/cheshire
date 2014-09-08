#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5

# --- Test phrase searching

set query "search llstr 6o19'59''N 9o13'59''W"


set results [eval $query]
puts "TEST ======== $query"

foreach rec $results {
    puts $rec
    puts ""
}

