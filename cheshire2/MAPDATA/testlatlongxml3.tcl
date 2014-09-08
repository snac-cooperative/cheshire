#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX GRS1
set CHESHIRE_ELEMENTSET XML_ELEMENT_cities
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 5

# --- Test phrase searching

set query "search lldec 6.33333 -9.23333"


set results [eval $query]
puts "TEST ======== $query"

foreach rec $results {
    puts $rec
    puts ""
}

