# ------------  Low Level Routines  -------------
# More accurately, boring routines

proc throw_html_type {} {
    global HTML_RESULT
    # We don't actually want to do this apparently
    # append HTML_RESULT  "Content-type: text/html\n\n"
}

proc throw_file { filename substs } {
    global HTML_RESULT
    
    set fh [open $filename]
    
    while {[gets $fh line] >= 0} {
	foreach sub $substs {
	    set subout [lindex $sub 0]
	    if [regexp $subout $line] {
		set subin [join [lrange $sub 1 [llength $sub]]]
		regsub -all [lindex $sub 0] $line $subin line
	    }
	}
	    append HTML_RESULT "$line\n"
    }
    close $fh
}

proc exit_error {reason} {
    global  HTML_RESULT
    set HTML_RESULT "<html><head><script>alert(\"$reason\");</script></head><body></body></html>"
    #set HTML_RESULT "Content-type: application/x-javascript\n\nalert(\"$reason\");\nexit;\n";
    throw_result
}

proc decode_cgistring {value} {
    set ascii "                                "
    append ascii " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\[\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    append ascii "                                                                                                                                                                                                                                                                                "
    set eol "<EOL>"
    set hexa "0123456789ABCDEF"

    # Do some that we can't let fall to the script
    regsub -all {\+} $value " " value;
    regsub -all "%0D%0A" $value $eol value;
    regsub -all "%0A" $value $eol value;
    regsub -all "%0D" $value $eol value;
    regsub -all "%26" $value {\&} value;

    set done 0
    while {[regexp {%([0-9A-F][0-9A-F])} [string range $value $done end] full code ]} {
        set coda [string index $code 0]
        set codb [string index $code 1]
        set idx [expr 16 * [string first $coda $hexa] + [string first $codb $hexa]]
        set char [string index $ascii $idx]
        set done [expr [string first $full $value] + 1]
        regsub -all "%$code" $value $char value
    }

    regsub -all "<EOL>" $value "\n" value
    return $value
}

proc encode_cgistring {string} {
    regsub -all { } $string {+} string
    regsub -all "\n\t" $string {\%0A} string
    regsub -all {,} $string {\%2C} string
    regsub -all {'} $string {\%27} string
    regsub -all "\"" $string {\%22} string
    regsub -all {@} $string {\%40} string
    regsub -all {\#} $string {\%23} string
    regsub -all {\}} $string {\%7D} string
    regsub -all {\{} $string  {\%7B} string
    regsub -all { } $string  {\%20} string
    return $string;
}

proc generate_query {} {
    global searchterms information_terms;
    # We should probably use attributeCombinations data from explain
    # But it's not worth it

    set bool $searchterms(bool)
    set firstterm 1;
    set searchterms(bool) ""
    set query "zfind"

    set inlist [array names searchterms]

    set fields(null) ""
    set bools(null) ""
    set opers(null) ""

    # First sort out our query some.
    foreach name $inlist {
	set value $searchterms($name)

	if { [regexp {^[0-9]+$} $name] } {
	    if {$value != ""} {
		set fields($name) $value
	    }
	} elseif { [ regexp {^bool([0-9]+)$} $name full field] } {
	    set bools($field) $value
	} elseif { [regexp {^oper([0-9]+)$} $name full field] } {
	    set opers($field) $value
	}
    }

    foreach name [array names fields] {
	
	set value $fields($name)
	if { $value != "" } {
	    if {$firstterm } {
		set firstterm 0
		# if bools(field) == NOT use it. Otherwise nothing
		if {[lsearch [array names bools] $name] != -1 && $bools($name) == "NOT" } {
		    append query " NOT"
		} 
	    } elseif {[lsearch [array names bools] $name] != -1} {
		append query " $bools($name)"
	    } else { 
		append query " $bool"
	    }
	    
	    append query " $name"
	    
	    if { [lsearch [array names opers] $name] != -1} {
		append query " $opers($name)"
	    }

	    append query " $value"
	}
    }

    if { [lsearch [array names searchterms] resultsetname] != -1 && $searchterms(resultsetname) != ""}  {
	append query " resultsetid $searchterms(resultsetname)"
    }

    return $query
}

proc generate_scan_query {} {
    global searchterms NUM_RECORDS 
    set scanfield ""
    set scanterm "000"
    set where [expr $NUM_RECORDS /2]

    foreach name [array names searchterms] {
	if {[regexp {^[0-9]+$} $name]} {
	    set scanfield $name
	    if {$searchterms($name) != ""} {
		set scanterm $searchterms($name)
	    }
	} elseif {$name == "firstrec"} {
	    set where $searchterms(firstrec)
	} elseif {$name == "scanfield"} {
	    set scanfield $searchterms($name)
	} elseif {$name == "scanterm"} {
	    set scanterm $searchterms($name)
	}
    }

    if {$scanfield == ""} {
	return 0
    } else {
	return [list $scanfield "$scanterm" $where]
    }
}


proc generate_scan_navbar {scanfield firstterm lastterm} {
    global  URI_HOST URI_PORT DATABASENAME NUM_RECORDS
    
    set numreq $NUM_RECORDS

    set url "z3950://$URI_HOST:$URI_PORT/scan/$DATABASENAME?numreq=$numreq\\\\&$scanfield="
    
    set text "<center>"
    
    set href $url
    append href "[encode_cgistring $firstterm]\\\\&firstrec=$numreq"
    append text "\[ <a href=\"$href\">Previous $numreq Records</a> \] | "
    
#    append text "\[ <a href = \"\" target =\"_top\">Home</a> \]"
    
    set href $url
    append href "[encode_cgistring $lastterm]\\\\&firstrec=1"
    append text " | \[ <a href=\"$href\">Next $numreq Records</a> \]"

    append text "</center>"

    return $text
}

proc process_query_string {} {
    global QUERY_STRING searchterms FIRST_RECORD NUM_RECORDS RECORD_FORMAT

    unset searchterms
    
    set arglist [split $QUERY_STRING &]
    set searchterms(bool) "AND"
    set searchterms(numreq) 20
    set searchterms(firstrec) 1
    set searchterms(format) "LONG2"

    foreach pair $arglist {
	set name [lindex [set pairlist [split $pair =]] 0]
	set val [lindex $pairlist 1]
	set name [decode_cgistring $name]
	set val [decode_cgistring $val]
	set searchterms($name) "$val"
    }

    set FIRST_RECORD $searchterms(firstrec)
    set NUM_RECORDS $searchterms(numreq)
    set RECORD_FORMAT $searchterms(format)
}


proc build_navbar {} {
    global searchterms FIRST_RECORD NUM_RECORDS URI_HOST URI_PORT DATABASENAME HITS RECORD_FORMAT RECV_RECORDS
    # Get relevant parts of searchterms array

    set fields(null) ""
    set bools(null) ""
    set opers(null) ""
    set inlist [array names searchterms]

    set prev "<form action = \"z3950://$URI_HOST:$URI_PORT/search/$DATABASENAME\" METHOD=\"GET\">"
    set next "<form action = \"z3950://$URI_HOST:$URI_PORT/search/$DATABASENAME\" METHOD=\"GET\">"

    foreach name $inlist {
	set value $searchterms($name)
	if { [regexp {^[0-9]+$} $name] } {
	    if {$value != ""} {
		set fields($name) $value
		
		if { [lsearch [array names bools] "bool$name"] == -1 } {
		    set bools($name) "AND"
		}
		if { [lsearch [array names opers] "oper$name"] == -1 } {
		    set opers($name) ""
		}
	    }
	} elseif { [ regexp {^bool([0-9]+)$} $name full field] } {
	    set bools($field) $value
	} elseif { [regexp {^oper([0-9]+)$} $name full field] } {
	    set opers($field) $value
	}
    }

    foreach name [array names fields] {
	set value $fields($name)
	if { $value != "" } {
	    append prev "<input type = hidden name = \"$name\" value = \"$value\">"
	    append prev "<input type = hidden name = \"bool$name\" value = \"$bools($name)\">"
	    append prev "<input type = hidden name = \"oper$name\" value = \"$opers($name)\">"

	    append next "<input type = hidden name = \"$name\" value = \"$value\">"
	    append next "<input type = hidden name = \"bool$name\" value = \"$bools($name)\">"
	    append next "<input type = hidden name = \"oper$name\" value = \"$opers($name)\">"
	} 
    }
    
    append prev "<input type = hidden name = \"numreq\" value=\"$NUM_RECORDS\">"
    append next "<input type = hidden name = \"numreq\" value=\"$NUM_RECORDS\">"

    append prev "<input type = hidden name = \"format\" value=\"$RECORD_FORMAT\">"
    append next "<input type = hidden name = \"format\" value=\"$RECORD_FORMAT\">"
    
    set prevstart [expr $FIRST_RECORD - $NUM_RECORDS]
    if {$prevstart < 0 && $FIRST_RECORD != 1} {
	    set prevstart 1
    }

    append prev "<input type = hidden name = \"firstrec\" value = \"$prevstart\">"
    append next "<input type = hidden name = \"firstrec\" value = \"[expr $FIRST_RECORD + $RECV_RECORDS]\">"
    
    append prev "<input type = submit value = \"Previous\"></form>"
    append next "<input type = submit value = \"Next\"></form>"

    if {$prevstart < 0  && [expr $FIRST_RECORD + $NUM_RECORDS] > $HITS } {
	return ""
    } elseif {$prevstart < 0 } {
	return $next 
    } elseif { [expr $FIRST_RECORD + $NUM_RECORDS] > $HITS } {
	return $prev
    } else {
	return "<table><tr><td>$prev</td><td>$next</td></tr></table>"
    }

}

proc build_cgihref {recno} {
    global URI_HOST URI_PORT DATABASENAME searchterms

    set href "z3950://$URI_HOST:$URI_PORT/search/$DATABASENAME?bool=AND"

    set inlist [array names searchterms]

    foreach name $inlist {
	set value $searchterms($name)
	if { [regexp {^[0-9]+$} $name] } {
	    if {$value != ""} {
		set fields($name) $value
		
		if { [lsearch [array names bools] "bool$name"] == -1 } {
		    set bools($name) "AND"
		}
		if { [lsearch [array names opers] "oper$name"] == -1 } {
		    set opers($name) ""
		}
	    }
	} elseif { [ regexp {^bool([0-9]+)$} $name full field] } {
	    set bools($field) $value
	} elseif { [regexp {^oper([0-9]+)$} $name full field] } {
	    set opers($field) $value
	}
    }

    foreach name [array names fields] {
	set value $fields($name)
	if { $value != "" } {
	    append href "&$name=$value"
	    append href "&bool$name=$bools($name)"
	    append href "&oper$name=$opers($name)"
	}
    }    
    append href "&numreq=1&firstrec=$recno"

    return $href
    
}



puts stderr "Loaded lowlevelibs..."
