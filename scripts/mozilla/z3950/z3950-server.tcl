#!/home/cheshire/cheshire/bin/ztcl

# First create Z variables that need to be maintained across sessions

set zsessions(null) NULL
set resultsets(null) NULL


# ------------- High Level Routines --------------

proc try_zsearch {database fail_on_error} {
    global second_zsearch QUERY

    if {$QUERY == ""} {
	set query [generate_query]
    } else {
	set query $QUERY
    }

    puts stderr "Trying: $query"
    set err [catch {eval $query} qresults]
    if {$err != 0} {
	set errmsg [lindex $qresults 1]

	if {([regexp "Connection closed.$" $errmsg] || [regexp "Connection to .* is closed," $qresults]) && $second_zsearch == 0} {
	    # Try for a reconnect for timeouts	    
	    puts stderr "Connection closed, trying to reopen..."
	    if { [try_zconnect database] } {
		# Now search again
		set second_zsearch 1
		return [try_zsearch $database $fail_on_error]
	    }
	}

	if {$fail_on_error == 1} {
	    puts stderr "Error: $qresults"
	    exit_error "Search Error for '$query': $errmsg"
	}
	return 0
    }

    puts stderr "Got results (from $database ?): $qresults"
    return $qresults

}

proc try_zconnect {database} {
    global URI_HOST URI_PORT zsessions 

    if { $database == ""} {
	set database "IR-Explain-1"
    }
    set serverstr "$URI_HOST:$URI_PORT/$database"

    if {[lsearch [array names zsessions] $serverstr] == -1 || $zsessions($serverstr) == ""} {
	set connname "conn[llength [array names zsessions]]"
	set err [catch {zselect $connname $URI_HOST $database $URI_PORT} cresults]
	set sessionid [lindex $cresults 0]
	set message [lindex $cresults 1]

	if {$sessionid == -1} {
	    exit_error "Connection failed to $serverstr: $message"
	    return 0
	} else {
	    puts stderr "Connected to $serverstr okay."
	    set zsessions($serverstr) $sessionid
	    return 1
	}
    } else {
	# This will NOT catch time outs, which are only got at zfind time
	set err [catch {zset session $zsessions($serverstr)} setresults]
	puts stderr "Connecting to $serverstr: $setresults"
	if {$err != 0} {
	    # Try reconnecting...
	    set zsessions($serverstr) ""
	    z_connect $database
	}
	return 1
    }
}
    


proc throw_result {} {
    global HTML_RESULT OUT_SOCKET PROCESS_FINISHED

    regsub -all "\n" $HTML_RESULT "%%N%%" HTML_RESULT
    puts $OUT_SOCKET $HTML_RESULT

    set PROCESS_FINISHED 1
}

proc init_explain_query {database} {
    if { [try_zconnect $database] } {
	zset attributes explain
	zset recsyntax explain
	source "zexplain.tcl"
    }
}

proc strobe_database {} {
    global URI_HOST database URI_PORT
    source "zstrobe.tcl"
    do_strobe $URI_HOST $URI_PORT $database
} 


# ------------  Low Level Routines  -------------

proc throw_html_type {} {
    global HTML_RESULT

    # We don't actually want to do this apparently
    # append HTML_RESULT  "Content-type: text/html\n\n"
}

proc throw_file { filename } {
    global HTML_RESULT
    set fh [open $filename]
    while {[gets $fh line] >= 0} {
	append HTML_RESULT "$line\n"
    }
    close $fh
}

proc exit_error {reason} {
    global PROCESS_FINISHED HTML_RESULT
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

proc process_query_string {} {
    global QUERY_STRING searchterms
    
    set arglist [split $QUERY_STRING &]
    set searchterms(bool) "AND"

    foreach pair $arglist {
	set name [lindex [set pairlist [split $pair =]] 0]
	set val [lindex $pairlist 1]
	set name [decode_cgistring $name]
	set val [decode_cgistring $val]
	set searchterms($name) "$val"
    }

}


# ---------- MAIN ROUTINE ----------

# Initialise and Set the environment variables

proc handle_request { envlist } {
    global  URI_HOST QUERY_STRING URI_PORT PATH_INFO HTML_RESULT PROCESS_FINISHED OUT_SOCKET QUERY
    global command_map explain_database information_terms preferred_language DEBUG database second_zsearch searchterms

    set preferred_language "eng"
    set information_terms {"bool" "numrec" "firstrec" "recsyntax" "resultsetname"}
    set explain_database "IR-Explain-1"
    set command_map(search) "zfind"
    set command_map(scan) "zscan"
    set command_map(explain) "zfind"
    set DEBUG 1
    set database ""


    if {$PROCESS_FINISHED == 0} {
	# Previous search did not finish?
	# We should do something about this potentially.
    }

    set PROCESS_FINISHED 0
    set HTML_RESULT ""    
    set QUERY ""
    set second_zsearch 0

    set QUERY_STRING ""
    set URI_HOST ""
    set URI_PORT 210
    set PATH_INFO ""
    
    foreach pair $envlist {
	set name [lindex $pair 0]
	global $name
	set value [lindex $pair 1]
	set $name $value
    }
    
    # Set up what the URL actually means.
    # z3950://host:port/command/database?arguments
    # arguments --  index=term & index=term ...
    
    set pathlist [split $PATH_INFO /]
    
    set command ""
    if {[llength $pathlist] > 1} {
	set command [lindex $pathlist 1]
    }
    if {[llength $pathlist] > 2} {
	set database [lindex $pathlist 2]
    }
    
    
    if {$URI_HOST == ""} {
	exit_error "Malformed URL; No host given."
	
    } elseif {$command == "" || ($command == "explain" && $database == "" && $QUERY_STRING == "")} {
	init_explain_query ""
	search_explain_host
	throw_result

    } elseif {[lsearch [array names command_map] $command] == -1} {
	exit_error "Malformed URL; Unknown Z39.50 command '$command'."
    } elseif {$database == "" && $command == "search" } {
	# No database, but search, query explain for dbs to search
	init_explain_query ""
	search_explain_type "databaseInfo"
	throw_result
    } elseif {$command == "explain" && $QUERY_STRING == ""} {
	init_explain_query ""
	search_explain_type $database
	throw_result
    } elseif {$command == "explain" } {
	# Process query string. NB: database can be "" here
	set searchterms(db) "IR-Explain-1"

	process_query_string
	
	# Search for db=(name) and if so, initialise with it.
	# Otherwise we need to generate a proper query

	puts stderr "Found alternate explain db: $searchterms(db)"
	
	if {$searchterms(db) != "IR-Explain-1"} {
	    set explain_database $searchterms(db)
	    init_explain_query $explain_database
	    if {$database != ""} {
		search_explain_type $database
	    } else {
		search_explain_host
	    }
	} else {
	    # puts "Not finding db"
	}

	throw_result
    } elseif {$QUERY_STRING == "" } {
	init_explain_query ""
	set success [search_explain_indexes]
	if {!$success} {
	    strobe_database
	}
	throw_result
    }
    
    if {$PROCESS_FINISHED == 0 && [lsearch [array names command_map] $command] == -1} {
	#Malformed URL, error
	exit_error "Malformed URL; unknown command '$command'"
    } elseif {$PROCESS_FINISHED == 0}  {

	set command $command_map($command)
	process_query_string

	zset timeout 15
	set connected [try_zconnect $database]
	
	if { $connected == 0} {
	    exit_error "Connection Error; Could not connect to $URI_HOST"
	} else {
	    
	    set qresults [try_zsearch $database 1]

	    puts stderr "Got:  $qresults"
	    
	    if {$PROCESS_FINISHED == 0} {
		# --- Display ---
		
		set hits [lindex [lindex [lindex $qresults 0] 2] 1]
		
		set HTML_RESULT "<html><head><title>Z39.50 Search Results</title></head><body bgcolor = white><p><pre>"
		
		append HTML_RESULT "\nhits: $hits\n\n"
		
		if {$hits == 0 } {
		    append HTML_RESULT "No Matches for <code>$query</code>."
		    append HTML_RESULT "$qresults"
		} else {
		    zset RECSYNTAX SGML
		    set err [catch {zdisplay} presults]
		    append HTML_RESULT "$presults"
		}
		
		append HTML_RESULT "</pre></body></html>"
		
		throw_result
	    }
	}
	
	close $OUT_SOCKET

    }
}
