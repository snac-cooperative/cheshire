
puts stderr "Loaded Z Server..."

#Exported boring functions. 
#We might as well just import as we're bound to use them 
#over the running of the server
source "liblowlevel.tcl"
source "zstrobe.tcl" 
source "zexplain.tcl"
source "display.tcl"
source "use_info.tcl"
source "client_urls.tcl"

# First create Z variables that need to be maintained across sessions
set zsessions(null) NULL
set resultsets(null) NULL
set strobeResults(null) NULL
set databaseTitles(null) NULL
set explainDatabases(null) NULL
set sidebar_searches {}

# Set up some globals 
set preferred_language "eng"
set information_terms {"bool" "numrec" "firstrec" "recsyntax" "resultsetname"}
set supported_commands {"search" "scan" "sort" "browse" "explain" "close"}

# ------------- Z Connection Routines --------------

# Note:  try_.search returns qresults or {0 error}
#    but:   try_.connect returns errormsg or 0 for success 

proc try_zsearch {} {
    global second_zsearch QUERY URI_HOST URI_PORT DATABASENAME zsessions searchterms resultsets
    # Do QUERY on DATABASENAME using current connection

    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
    if {$QUERY == ""} {
	set query [generate_query]
    } else {
	set query $QUERY
    }


    set err [catch {eval $query} qresults]
    if {$err != 0} {
	set errmsg [lindex $qresults 1]
	
	if {([regexp "Connection .*closed" $errmsg]) && $second_zsearch == 0} {
	    # Try for a reconnect for timeouts	    
	    # Take serverstr out of zsessions as it's no longer valid
	    unset zsessions($serverstr)
	    if { [try_zconnect 1] == 0} {
		# Now search again
		set second_zsearch 1
		return [try_zsearch ]
	    }
	}
	return [list 0 $errmsg]
    }

    if {$searchterms(resultsetname) != "" } {
	set current $resultsets($serverstr);
	if { [lsearch $current $searchterms(resultsetname)] == -1 } {
	    lappend current $searchterms(resultsetname)
	    set resultsets($serverstr) $current
	}
    }

    return $qresults

}

proc try_esearch {} {
    global second_esearch QUERY  zsessions 
    # Do QUERY on DATABASENAME using current connection  // EXPLAIN

    if {$QUERY == ""} {
	set query [generate_query]
    } else {
	set query $QUERY
    }

    set err [catch {eval $query} qresults]
    if {$err != 0} {
	set errmsg [lindex $qresults 1]
	if {([regexp "Connection .*closed" $errmsg])  && $second_esearch == 0} {
	    # Try for a reconnect for timeouts	    
	    if { [try_econnect 1] == 0} {
		# Now search again
		set second_esearch 1
		return [try_esearch ]
	    }
	}
	return [list 0 $errmsg]
    }
    return $qresults
}

proc try_zconnect args {
    global URI_HOST URI_PORT zsessions DATABASENAME URI_USER URI_PASS resultsets
    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"

    if {$URI_USER != ""} {
	set extraargs "$URI_USER/$URI_PASS"
    } else {
	set extraargs  ""
    }

    if { [llength $args] > 0 } {
	unset zsessions($serverstr)
    }

    if {[lsearch [array names zsessions] $serverstr] == -1 || $zsessions($serverstr) == ""} {

	set connname "conn[llength [array names zsessions]]"
	set cmd "zselect $connname $URI_HOST $DATABASENAME $URI_PORT $extraargs"
	set err [catch {eval $cmd} cresults]
	set sessionid [lindex $cresults 0]
	set message [lindex $cresults 1]

	if {$sessionid == -1} {
	    return $message
	} else {
	    zset timeout 15
	    set zsessions($serverstr) $sessionid
	    set resultsets($serverstr) [list]
	    return 0
	}
    } else {
	# This will NOT catch time outs, which are only got at zfind time :/
	set err [catch {zset session $zsessions($serverstr)} setresults]
	if {$err != 0} {
	    unset zsessions($serverstr)
	    return [try_zconnect ]
	}
	return 0
    }
}
    
proc try_econnect args {
    global URI_HOST URI_PORT DATABASENAME explainDatabases zsessions

    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
    if {[lsearch [array names explainDatabases] $serverstr] == -1 } {
	# We don't know where to look... Try default
	set host $URI_HOST
	set port $URI_PORT
	set db "IR-Explain-1"
    } else {
	set explaindata $explainDatabases($serverstr);
	set host [lindex $explaindata 0]
	set port [lindex $explaindata 1]
	set db [lindex $explaindata 2]
    }

    set explainstr "$host:$port/$db"

    if { [llength $args] > 0 && [lsearch [array names zsessions] $explainstr] != -1} {
	set zsessions($explainstr) ""
    }

    if {[lsearch [array names zsessions] $explainstr] == -1 || $zsessions($explainstr) == ""} {
	set connname "conn[llength [array names zsessions]]"
	set err [catch {zselect $connname $host $db $port} cresults]
	set sessionid [lindex $cresults 0]
	set message [lindex $cresults 1]

	if {$sessionid == -1} {
	    return $message
	} else {
	    set zsessions($explainstr) $sessionid
	    zset attributes explain
	    zset recsyntax explain
	    zset timeout 15
	    return 0
	}
    } else {
	# This will NOT catch time outs, which are only got at zfind time :/
	set err [catch {zset session $zsessions($explainstr)} setresults]
	if {$err != 0} {
	    unset zsessions($explainstr)
	    return [try_econnect]
	}
	return 0
    }
}


proc try_zdisplay {} {
    global NUM_RECORDS FIRST_RECORD DISPLAY_RESULTSET

    set err [catch {eval "zdisplay $DISPLAY_RESULTSET $NUM_RECORDS $FIRST_RECORD"} results]
    if {!$err} {
	return $results
    } else {
	return [list 0 $results]
    }
}

proc try_edisplay {num} {
    global ERESULTSETNAME 

    set err [catch {eval "zdisplay $ERESULTSETNAME $num"} results]
    if {!$err} {
	return $results
    } else {
	return [list 0 $results]
    }
}

proc try_zdelete {setname} {
    set cmd "zdelete $setname"
    set err [catch {eval $cmd} results]
    return $err
}

proc try_zclose {} {
    global URI_HOST URI_PORT DATABASENAME zsessions resultsets

    set cmd "zclose"
    set err [catch {eval $cmd} results]
    if {$err == 0} {
	set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
	unset zsessions($serverstr)
	unset resultsets($serverstr)
    }
    return $err
}
    

proc throw_result {} {
    global HTML_RESULT 
    # global thread_listener flipflop

#    if {$flipflop == -1} {
	# We've been canceled
#    } else {
#	thread::send $thread_listener "returnHTML {$HTML_RESULT}"

    regsub -all "\n" $HTML_RESULT "%%N%%" HTML_RESULT
    returnHTML $HTML_RESULT

#    }
}


# ---------- MAIN ROUTINE ----------

# Initialise and Set the environment variables

proc handle_request { envlist } {
    global  URI_SPEC URI_HOST QUERY_STRING URI_PORT PATH_INFO HTML_RESULT QUERY DATABASENAME FIRST_RECORD NUM_RECORDS HITS RECORD_FORMAT RECV_RECORDS URI_USER URI_PASS DISPLAY_RESULTSET ERESULTSETNAME
    global supported_commands information_terms preferred_language  second_zsearch searchterms second_esearch searchterms zsessions resultsets newsetname
    #global flipflop


    set HTML_RESULT ""    
    set URI_SPEC ""
    set QUERY ""
    set QUERY_STRING ""
    set URI_HOST ""
    set URI_PORT 210
    set PATH_INFO ""
    set DATABASENAME ""
    set DISPLAY_RESULTSET ""
    set ERESULTSETNAME ""
    set RECV_RECORDS 0

    set second_zsearch 0
    set second_esearch 0
    set searchterms(null) NULL
    
    foreach pair $envlist {
	set name [lindex $pair 0]
	global $name
	set value [lindex $pair 1]
	set $name $value
    }

    # Extract user/pass if given
    if { [regexp -nocase "^z3950://(\[^/\]+):(\[^/\]+)@$URI_HOST" $URI_SPEC null URI_USER URI_PASS] } {
    } else {
	set URI_USER ""
	set URI_PASS ""
    }
    
    # Set up what the URL actually means.
    # z3950://user:pass@host:port/command/database?arguments
    # z3950://sidebar   -- build current state sidebar
    # z3950://search/ (?args)    -- multi server search
    
    set pathlist [split $PATH_INFO /]
    
    set command ""
    if {[llength $pathlist] > 1} {
	set command [lindex $pathlist 1]
    }
    if {[llength $pathlist] > 2} {
	set DATABASENAME [lindex $pathlist 2]
    }


    if {$URI_HOST == "sidebar" && $command == "deleteResultSets"} {
	delete_resultsets
	url_sidebar
    } elseif {$URI_HOST == "sidebar"} {
	url_sidebar
    } elseif {$URI_HOST == "search"} {
	url_search
    } elseif {$URI_HOST == ""} {
	exit_error "Badly formed URL: No host given."
    } elseif {$command == "" || ($command == "explain" && $DATABASENAME == "" && $QUERY_STRING == "")} {
	search_explain_host
	throw_result
    } elseif {[lsearch $supported_commands $command] == -1} {
	exit_error "Malformed URL; Unknown Z39.50 command '$command'."
    } elseif {$DATABASENAME == "" && $command == "search" } {
	# No database, but search, query explain for dbs to search
	set searchterms(format) "LONG2"
	search_explain_type "databaseInfo"
	throw_result
    } elseif {$command == "explain" && $QUERY_STRING == ""} {
	set searchterms(format) "LONG2"
	search_explain_type $DATABASENAME
	throw_result
    } elseif {$command == "explain" } {

	# Doing a weirdo search on explain??
	# We need to work on this.
	process_query_string

	if {$searchterms(format) == "summary"} {
	    search_explain_type $DATABASENAME
	    throw_result
	} else {
	    exit_error "Can't handle that at the moment..."
	}
	return
	
	set searchterms(db) "IR-Explain-1"
	
	# Search for db=(name) and if so, initialise with it.
	# Otherwise we need to generate a proper query

	if {$searchterms(db) != "IR-Explain-1"} {
	    set explain_database $searchterms(db)
	    init_explain_query $explain_database 0
	    if {$database != ""} {
		search_explain_type $database
	    } else {
		search_explain_host
	    }
	}

	throw_result
    } elseif { [lsearch $supported_commands $command] == -1} {
	#Malformed URL, error
	exit_error "Badly formed URL; unknown command '$command'"
    } elseif {$command == "search" && $QUERY_STRING == "" } {
	set success [search_explain_indexes]
	if {!$success} {
	    strobe_search $URI_HOST $URI_PORT $DATABASENAME
	}
	throw_result
    } elseif {$command == "search"} {

	process_query_string
	set connected [try_zconnect]	

	if { $connected != 0} {
	    exit_error "Connection Error; Could not connect to $URI_HOST: $connected"
	} else {
	    
	    set qresults [try_zsearch ]
	    
	    if {[lindex $qresults 0] == 0 } {
		exit_error [lindex $qresults 1]
	    } else {
		# --- Display ---
		
		set titlestr [get_explain_titleString]
		if { $titlestr == ""} {
		    set titlestr "$URI_HOST - $DATABASENAME"
		}

		set HITS [lindex [lindex [lindex $qresults 0] 2] 1]
		set HTML_RESULT "<html><head><title>$titlestr Search Results</title></head><body bgcolor = white><p><center><b>$titlestr Search Results</b></center>"

 		set recordhtml [try_display_records]
		
		append HTML_RESULT "<p>Your search matched $HITS records.  Displaying $RECV_RECORDS, starting at $FIRST_RECORD, below:<p>"
		append HTML_RESULT $recordhtml
		append HTML_RESULT "[build_navbar]</body></html>"
		
		throw_result
	    }
	}
	
    } elseif {[lsearch [list "scan" "browse"] $command] != -1 && $QUERY_STRING == ""} {
	# Generate scan form

	if { [explain_scan] == 0} {
	    strobe_scan $URI_HOST $URI_PORT $DATABASENAME
	}


    } elseif {[lsearch [list "scan" "browse"] $command] != -1 } {
	#.../scan/database?USE=TERM&numreq=N
	# Scan always returns the same format! =)

	process_query_string

	if { [try_zconnect] == 0} {

	    set queryargs [generate_scan_query]
	    set scanfield [lindex $queryargs 0]
	    set scanterm [lindex $queryargs 1]
	    set scanpos [lindex $queryargs 2]
	    set query "zscan $scanfield \{$scanterm\} 0 $NUM_RECORDS $scanpos"

	    set err [catch {eval $query} qresults]

	    if {$err != 0 } {
		exit_error "Scan error: $qresults"
	    } elseif {$qresults == "" || [llength $qresults] == 1} {
		exit_error "Scan error: No results"
	    } else {

		set sresults [lrange $qresults 1 end]
		set firstterm ""
		set lastterm ""
		set prevterm ""
		set foundterm 0
		set termidx 0
		set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
		set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]

		set titlestr [get_explain_titleString]
		if { $titlestr == ""} {
		    set titlestr "$URI_HOST - $DATABASENAME"
		}

		throw_file "html/browse.html" [list "\\%TITLE\\% $titlestr"]
		

		foreach result $sresults {
		    set recs [lindex $result [expr [llength $result] - 1]]

		    # Innopac servers don't return the number of records
		    if {! [regexp {^[0-9]+$} $recs]} {
			set term $result
			set recs "unknown"
		    } else {
			set term [lrange $result 0 [expr [llength $result] -2]]
		    }

		    if {$recs == 0} {
			set recs "unknown"
		    }

		    if { [set idx [string first " -->" $term]] != -1} {
			set term [string range $term 0 [expr $idx -1]]
			regsub -all {\^032} $term " " term
		    }

		    incr termidx
		    
		    # Cheshire Versions < 2.32 might return this as the first term
		    if {$term == ">>>Global Data Values<<<"} {
			set firstterm ""
		    } else {
			
			if {$termidx == 1} {
			    set firstterm $term
			} elseif { $termidx == $NUM_RECORDS } {
			    set lastterm $term
			}
			
			set is_scanterm 0
			if { [string tolower $term ] == [ string tolower $scanterm] } {
			    set is_scanterm 1
			    set foundterm 1
			}
			
			set termtxt "<a href=\"z3950://$URI_HOST:$URI_PORT/search/$DATABASENAME?numreq=$NUM_RECORDS&firstrec=1&$scanfield=[encode_cgistring $term]\">$term</a>"

			if {$foundterm == 0 && $term > $scanterm && $prevterm < $scanterm && $scanterm != "000"} {
			    append HTML_RESULT "<tr><td colspan =2><center>Your term '$scanterm' would have been here</center></td></tr>"
			    set foundterm 1
			} elseif {$is_scanterm == 1} {
			    append HTML_RESULT "<tr><td><b>$termtxt</b></td><td>$recs</td></tr>"
			} else {
			    append HTML_RESULT "<tr><td>$termtxt</td><td>$recs</td></tr>"
			}
		    }
		    set prevterm $term
		}
		set navbar [generate_scan_navbar $scanfield $firstterm $lastterm]
		throw_file "html/browse-bottom.html" [list "\%NAVBAR\% $navbar"]

		throw_result
	    }
	}
    } elseif {$command == "close"} {
	#Close connection to host/port/db if possible.

	set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
	set sessions [array names zsessions]
	
	if {[lsearch $sessions $serverstr] == -1 } {
	    exit_error "There is no connection to $DATABASENAME at $URI_HOST to close."
	} else {
	    
	    # XXX Wrap this intelligently  :P
	    try_zconnect
	    try_zclose
	    
	    if {$QUERY_STRING != "" } {
		url_sidebar
	    } else {
		throw_file "html/closed.html" [list]
		throw_result
	    }
	}
    } elseif {$command == "sort"} {

	process_query_string
	set names [array names searchterms]
	if {[lsearch $names "sets"] == -1 || $searchterms(sets) == ""} {
	    exit_error "You must select at least one result set to sort."
	} else {
	    
	    # If no sortfield, just return resultset?

	    set cmd [generate_sort_query]
	    if { [set conn [try_zconnect]] == 0} {
		set err [catch {eval $cmd} qresults]
		if {$err == 0} {

		    if {[lindex [lindex [lindex $qresults 0 ] 1] 1] == 2 } {
			exit_error "The server could not handle your sort request."
		    } else {


			set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
			set current $resultsets($serverstr);
			if { [lsearch $current $newsetname] == -1 } {
			    lappend current $newsetname
			    set resultsets($serverstr) $current
			}

			set HITS [lindex [lindex [lindex $qresults 0] 4] 1]

			set titlestr [get_explain_titleString]
			if { $titlestr == ""} {
			    set titlestr "$URI_HOST - $DATABASENAME"
			}

			set HTML_RESULT "<html><head><title>$titlestr Search Results</title></head><body bgcolor = white><p><center><b>$titlestr Search Results</b></center>"
			
			if { $searchterms(numreq) != 1} {
			    set RECORD_FORMAT "summary"
			}
			set DISPLAY_RESULTSET $newsetname
			set recordhtml [try_display_records]
			
			append HTML_RESULT "<p>Your search matched $HITS results.  Displaying $RECV_RECORDS starting at $FIRST_RECORD below:<p>"
			append HTML_RESULT $recordhtml
			append HTML_RESULT "[build_sort_navbar]</body></html>"
			throw_result
		}
		} else {
		    exit_error $qresults
		}
	    } else {
		exit_error $conn
	    }
	}

	
    }
}
