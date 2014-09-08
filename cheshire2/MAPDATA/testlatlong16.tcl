#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

set query "search bboxstr <#> {54o00'00''N 120o0'0''W 4o0'0''S 50o0'0''W}"


set results [eval $query]
puts "TEST ===ENCLOSES===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

