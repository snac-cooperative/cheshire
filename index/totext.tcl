#!/usr/local/bin/tclsh
#
#
puts "Sample record:"
puts "Test EXTERNAL conversion script for record display"
puts ""

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

    puts "TITLE: $match "



}

puts "End of Record"
puts "+++++++++++++"
exit
