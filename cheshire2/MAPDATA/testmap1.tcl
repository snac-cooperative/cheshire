#!/usr/bin/env staffcheshire
# or !/User/ray/Work/cheshire/bin/staffcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX XML
set CHESHIRE_ELEMENTSET XML_ELEMENT_country
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 200
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

source map4.tcl

set query "search bboxstr >=< {54o00'00''N 117o0'0''W 4o0'0''S 50o30'0''W}"

.map create rectangle white 54o00'00''N 117o00'00''W 4o00'0''S 50o30'0''W

.map create rectangle yellow  0o33'0''S 50o49'59''W 29o57'0''S 33o7'0''W

.map create rectangle red  41o58'12''N 124o15'36''W 34o28'12''N 114o16'11''W

set results [eval $query]
puts "TEST ===OVERLAPS===== $query"

foreach rec $results {
    puts $rec
    puts ""
}

