
puts stderr "Loaded: zstrobe.tcl"

proc strobe_indexes {host port database} {
    global bib1nums strobeResults borders numeric_fields zsessions
    #global flipflop

    set searchsets {1 2}
    set serverstr "$host:$port/$database"
    set foundnums {}
    set foundidxs {}
    set length [llength $bib1nums]
    
    try_zconnect
    
    for {set idx 0} {$idx < $length} { incr idx} {
	set number [lindex $bib1nums $idx]
	
	set dosearch 0
	foreach setidx $searchsets {
	    if { $number > [lindex $borders [expr $setidx -1]] && $number <= [lindex $borders $setidx] } {
		set dosearch 1
	    }
	}
	if { $dosearch == 1} {
	    if { [lsearch $foundnums $number] == -1 } {
		
		if {[lsearch $numeric_fields $number] == -1} {
		    set term "anything"
		} else {
		    set term 100
		}
		
		set err [catch {zfind $number $term} qresults];
		set status [lindex [lindex [lindex $qresults 0] 1] 1]
		puts stderr "Strobe for $number:  $status"
#		vwait flipflop
#		if {$flipflop == -1} {
#		    return 0
#		}
		if {$status == 1 || $status == 255}  {
		    lappend foundnums $number;
		    lappend foundidxs $idx;
		}
	    }
	}
    }
    

    if { [llength $foundnums] == 0} {
	zclose
	set zsessions($serverstr) "null"
    }
    set strobeResults($serverstr) $foundidxs
    return $foundnums
}

proc strobe_scan {host port database} {
    global HTML_RESULT strobeResults bib1names bib1nums 
    set serverstr "$host:$port/$database"


    if {[lsearch [array names strobeResults]  $serverstr] == -1} {

	set foundnums [strobe_indexes $host $port $database]
	set foundidxs $strobeResults($serverstr)
    } else {
	set foundidxs $strobeResults($serverstr)
    }


    if { [llength $foundidxs] == 0 } {
	throw_file "html/strobe_failed.html" [list "\%DB\% $database" "\%HOSTNAME\% $host:$port"]
    } else {


	throw_file "html/scan_strobe.html" [list]
	set url "z3950://$host:$port/scan/$database"
	#Build form
	set formtxt "<form action=\"$url\">\n<table>\n"
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	append formtxt "<tr><td><select name=\"scanfield\">"
	foreach fidx $foundidxs {
	    set name [lindex $bib1names $fidx]
	    set number [lindex $bib1nums $fidx]
	    regsub -all {[_-]} $name " " name
	    append formtxt "<option value = \"$number\">$name</option>"
	}
	append formtxt "</select></td><td><input type = text size = 40 value = \"\" name = \"scanterm\"> </td></tr>"
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	append formtxt "</table><input type =submit value=Scan></form></body></html>"
	append HTML_RESULT $formtxt
    }

    throw_result
    

}


proc strobe_search {host port database} {
    global HTML_RESULT strobeResults bib1names bib1nums 

    set serverstr "$host:$port/$database"
    if {[lsearch [array names strobeResults]  $serverstr] == -1} {

	set foundnums [strobe_indexes $host $port $database]

	if {$foundnums == 0} {
	    return
	}
	set foundidxs $strobeResults($serverstr)
    } else {
	set foundidxs $strobeResults($serverstr)
    }


    if { [llength $foundidxs] == 0 } {
	throw_file "html/strobe_failed.html" [list "\%DB\% $database" "\%HOSTNAME\% $host:$port"]
    } else {
    
	throw_file "html/index_strobe_template.html" [list]
	set url "z3950://$host:$port/search/$database"
	set formtxt "<form action=\"$url\">\n<table>\n"
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	
	foreach fidx $foundidxs {
	    set name [lindex $bib1names $fidx]
	    set number [lindex $bib1nums $fidx]
	    regsub -all {[_-]} $name " " name
	    append formtxt "<tr><td><b>$name</b>:</td><td><input type = \"text\" size = \"40\" name = \"$number\" value = \"\"></td></tr>\n"
	}

	    set formatlist [get_explain_elementsetnames]
	    
	    set formats "<select name =\"format\"><option value=\"LONG2\">Full</option><option value=\"SUMMARY\">Summary</option>"
	    if {$formatlist != [list]} {
		foreach f $formatlist {
		    append formats "<option value=\"$f\">$f</option>"
		}
	    }	    
	    append formats "</select>"
	
	
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	append formtxt "<tr><td>Combine Fields With:</td><td><select name = \"bool\"><option value = \"AND\">And<option value = \"OR\">Or</select></td></tr>\n";
	append formtxt "<tr><td>Number of Records Requested:</td><td><input type = \"text\" name = \"numreq\" size = 10 value = 10></td></tr>"
	append formtxt "<tr><td>First Record:</td><td><input type = \"text\" name = \"firstrec\" size = 10 value = 1></td></tr>"
	append formtxt "<tr><td>Requested Format:</td><td>$formats</td></tr>"
	append formtxt "<tr><td>Name of Result Set:</td><td><input type = \"text\" name = \"resultsetname\" size = 20 value = \"\"></td></tr>"


	append formtxt "\n</table>\n<input type =\"submit\" value = \"Search\"> &nbsp; <input type = \"reset\" value = \"Reset\">\n</form>\n"
	
	append HTML_RESULT $formtxt
	append HTML_RESULT "<hr></body></html>"
    }
    throw_result

}
