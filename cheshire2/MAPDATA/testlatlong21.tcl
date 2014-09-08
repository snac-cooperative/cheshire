#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX XML
set CHESHIRE_ELEMENTSET XML_ELEMENT_country
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 200
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

#source map4.tcl

set query "search bboxstr >#< {56o30'00''N 120o11'6''W 5o36'0''S 47o21'0''W}"

#.map create rectangle white 56o30'00''N 120o11'6''W 5o36'0''S 47o21'0''W

#.map create rectangle yellow  0o33'0''S 50o49'59''W 29o57'0''S 33o7'0''W


set results [eval $query]
puts "TEST ===FULLY ENCLOSED WITHIN ===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

