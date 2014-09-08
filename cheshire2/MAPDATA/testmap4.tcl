#!/usr/bin/env staffcheshire
# OR !/home/ray/Work/cheshire/bin/staffcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX XML
set CHESHIRE_ELEMENTSET XML_ELEMENT_country|bboxll
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 200
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

source map4.tcl

set query "search bboxstr <#> {26o00'00''N 105o0'0''W 22o0'0''N 99o0'0''W}"

.map create rectangle white 26o00'00''N 105o0'0''W 22o0'0''N 99o0'0''W


set results [eval $query]
puts "TEST === ENCLOSES - what countries (yellow) enclose the search area (white)?)===== $query"

#puts $results 

foreach rec $results {
    puts $rec
    if {[regexp -nocase {(<bboxll> )([^<]*)(</bboxll>)} $rec all junk1 coord junk2] == 1} {
	puts $coord
	.map create rectangle yellow [lindex $coord 0] [lindex $coord 1] [lindex $coord 2] [lindex $coord 3]
    } 
}

