#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

set query "search bboxstr >#< {56o30'00''N 120o11'6''W 5o36'0''S 47o21'0''W}"


set results [eval $query]
puts "TEST ===FULLY ENCLOSED WITHIN ===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

