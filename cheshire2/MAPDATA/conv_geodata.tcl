global CITIES

proc loadcities {} {
    global CITIES COUNTRIES
    set f [open testgeo.data]
    
    while {[gets $f x] != -1} {
	lappend CITIES([string toupper [lindex $x 1]]) $x
	lappend COUNTRIES([string toupper [lindex $x 0]]) $x
    }
    close $f
}

proc convgeodata {} {
    global CITIES COUNTRIES test
    global XDOCS
    set outfile [open testxml.data w]
    
    foreach country [array names COUNTRIES] {
	set N -90
	set S 90
	set E -180
	set W 180
	set rec ""
	set citycount 0
	puts $outfile "<GeoData><country> $country </country>"
	puts $outfile "<cities>"
	foreach city_data $COUNTRIES($country) {
	    set temp [lindex $city_data 3]
	    set long [lindex $temp 0]
	    if {$long < $W} {set W $long}
	    if {$long > $E} {set E $long}
	    set lati [lindex $temp 1]
	    if {$lati < $S} {set S $lati}
	    if {$lati > $N} {set N $lati}
	    set test [degrees_to_latlon $lati $long]
	    set lati2 [lindex $test 0]
	    set long2 [lindex $test 1]
	    regsub -all "''" $lati2 "-" latix
	    regsub -all "'" $latix "-" latix
	    regsub -all "o" $latix "-" latix
	    regsub -all "''" $long2 "-" longx
	    regsub -all "'" $longx "-" longx
	    regsub -all "o" $longx "-" longx
	    set cityname [lindex $city_data 1]
	    set pop [lindex $city_data 2]
	    puts $outfile "<city><cityname> $cityname </cityname>"
	    puts $outfile "<pop> $pop </pop>"
	    puts $outfile "<latlongdeg> $lati $long </latlongdeg>" 
	    puts $outfile "<longlatdeg> $long $lati </longlatdeg>" 
	    puts $outfile "<latlongstr> $lati2 $long2 </latlongstr>" 
	    puts $outfile "<latlonghy> $latix $longx </latlonghy>" 
	    puts $outfile "</city>"

	    incr citycount
	}
	puts $outfile "</cities>"
	if {$citycount > 1} {
	    puts $outfile "<bboxdeg> $N $W $S $E </bboxdeg>"
	    puts $outfile "<bboxll> [degrees_to_latlon $N $W] [degrees_to_latlon $S $E] </bboxll>"
	} else {
	    puts $outfile "<bboxdeg> $lati $long $lati $long </bboxdeg>"
	    puts $outfile "<bboxll> [degrees_to_latlon $lati $long] [degrees_to_latlon $lati $long] </bboxll>"
	}
	puts $outfile "</GeoData>"
	
    }

    close $outfile
}

# attach cities and set up default bindings
loadcities
convgeodata
puts "all done..."
exit
