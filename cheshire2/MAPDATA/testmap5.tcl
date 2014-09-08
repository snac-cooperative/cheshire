#!/usr/bin/env staffcheshire
# OR !/home/ray/Work/cheshire/bin/staffcheshire

set CHESHIRE_DATABASE geotest
set CHESHIRE_CONFIGFILE config.geotest
set CHESHIRE_RECSYNTAX XML
set CHESHIRE_ELEMENTSET XML_ELEMENT_cityname|latlongstr
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 200
set CHESHIRE_ATTRIBUTESET GEO

# --- Test phrase searching

source map4.tcl

set query "search cityllstr >#< {54o00'00''N 117o0'0''W 4o0'0''S 50o30'0''W}"

.map create rectangle yellow 54o00'00''N 117o00'00''W 4o00'0''S 50o30'0''W

set results [eval $query]

puts "TEST === ENCLOSED WITHIN - what cities (white points) are enclosed within the search area (yellow)?) - city component search ===== $query"

#puts $results 

foreach rec $results {
    puts $rec
    set newrecs [split $rec "\n"]
puts "newrecs = $newrecs"
    foreach line $newrecs {

	if {[regexp -nocase {(<latlongstr> )([^<]*)(</latlongstr>$)} $line all junk1 coord junk2] == 1} {
	    #puts $coord
	    .map create point white [lindex $coord 0] [lindex $coord 1]
	}
    } 
}


