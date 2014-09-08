#!/usr/local/bin/tclsh
#
#
puts "<html>"
puts "<head><title> Sample record </title></head>"
puts "<body>"
puts "<h3> Test EXTERNAL conversion script for record display</h3>"
puts "<HR></HR>"

while {[gets stdin line] >= 0} {
    
    #    if {[regexp -nocase "<eadid>" $line match]} {
    #	regsub -all -nocase {<titleproper>[^<]*</titleproper>} $line "" line
    #	puts "<p><b>EADID: $line </b></p>"
    #
    #    }
    #    if {[regexp -nocase "^<titleproper>" $line match]} {
    #	puts "<p><h2>TITLE: <i> $line </i></H2></p>"
    #    }
	    
    set start [string first "<Fld245" $line]
    set last [expr [string first "</Fld245>" $line] + 8]
    set match [string range $line $start $last]

    puts "<p><h2>TITLE: <i> $match </i></H2></p>"



}

puts "<HR></HR>"
puts "</body></html>"
exit
