
puts stderr "Loaded Z Server..."

#Exported boring functions. 
#We might as well just import as we're bound to use them 
#over the running of the server
source "liblowlevel.tcl"
source "zstrobe.tcl" 
source "zexplain.tcl"
source "display.tcl"
source "use_info.tcl"

# First create Z variables that need to be maintained across sessions
set zsessions(null) NULL
set resultsets(null) NULL
set strobeResults(null) NULL
set explainDatabases(null) NULL


# Set up some globals 
set preferred_language "eng"
set information_terms {"bool" "numrec" "firstrec" "recsyntax" "resultsetname"}

# Is command_map actually useful???
set command_map(search) "zfind"
set command_map(scan) "zscan"
set command_map(browse) "zscan"
set command_map(explain) "zfind"

# ------------- Z Connection Routines --------------

# Note:  try_.search returns qresults or {0 error}
#    but:   try_.connect returns errormsg or 0 for success 

proc try_zsearch {} {
    global second_zsearch QUERY URI_HOST URI_PORT DATABASENAME zsessions
    # Do QUERY on DATABASENAME using current connection

    if {$QUERY == ""} {
	set query [generate_query]
    } else {
	set query $QUERY
    }

    set err [catch {eval $query} qresults]
    if {$err != 0} {
	set errmsg [lindex $qresults 1]
	
	if {([regexp "Connection closed.$" $errmsg] || [regexp "Connection to .* is closed" $qresults]) && $second_zsearch == 0} {
	    # Try for a reconnect for timeouts	    
	    # Take serverstr out of zsessions as it's no longer valid
	    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
	    set zsessions($serverstr) ""
	    if { [try_zconnect] == 0} {
		# Now search again
		set second_zsearch 1
		return [try_zsearch ]
	    }
	}
	return [list 0 $errmsg]
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
	if {([regexp "Connection closed.$" $errmsg] || [regexp "Connection to .* is closed," $qresults]) && $second_esearch == 0} {
	    # Try for a reconnect for timeouts	    
	    if { [try_econnect] == 0} {
		# Now search again
		set second_esearch 1
		return [try_esearch ]
	    }
	}
	return [list 0 $errmsg]
    }
    return $qresults
}

proc try_zconnect {} {
    global URI_HOST URI_PORT zsessions DATABASENAME URI_USER URI_PASS
    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"

    if {$URI_USER != ""} {
	set extraargs "$URI_USER/$URI_PASS"
    } else {
	set extraargs  ""
    }


    if {[lsearch [array names zsessions] $serverstr] == -1 || $zsessions($serverstr) == ""} {

	set connname "conn[llength [array names zsessions]]"
	set cmd "zselect $connname $URI_HOST $DATABASENAME $URI_PORT $extraargs"
	set err [catch {eval $cmd} cresults]
	set sessionid [lindex $cresults 0]
	set message [lindex $cresults 1]

	puts stderr "Connect results: $cresults"

	if {$sessionid == -1} {
	    return $message
	} else {
	    zset timeout 15
	    set zsessions($serverstr) $sessionid
	    return 0
	}
    } else {
	# This will NOT catch time outs, which are only got at zfind time :/
	set err [catch {zset session $zsessions($serverstr)} setresults]
	if {$err != 0} {
	    set zsessions($serverstr) ""
	    return [try_zconnect ]
	}
	return 0
    }
}
    
proc try_econnect {} {
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
	    set zsessions($explainstr) ""
	    return [try_econnect]
	}
	return 0
    }
}


proc try_zdisplay {} {
    global NUM_RECORDS FIRST_RECORD

    set err [catch {eval "zdisplay $NUM_RECORDS $FIRST_RECORD"} results]
    if {!$err} {
	return $results
    } else {
	return [list 0 $results]
    }
}

proc try_edisplay {num} {
    set err [catch {eval "zdisplay $num"} results]
    if {!$err} {
	return $results
    } else {
	return [list 0 $results]
    }
}
    

proc throw_result {} {
    global HTML_RESULT thread_listener flipflop

    if {$flipflop == -1} {
	# We've been canceled
    } else {
	regsub -all "\n" $HTML_RESULT "%%N%%" HTML_RESULT
	thread::send $thread_listener "returnHTML {$HTML_RESULT}"
    }
}


# ---------- MAIN ROUTINE ----------

# Initialise and Set the environment variables

proc handle_request { envlist } {
    global  URI_HOST QUERY_STRING URI_PORT PATH_INFO HTML_RESULT QUERY DATABASENAME FIRST_RECORD NUM_RECORDS HITS RECORD_FORMAT RECV_RECORDS URI_USER URI_PASS
    global command_map information_terms preferred_language  second_zsearch searchterms second_esearch flipflop searchterms


    set HTML_RESULT ""    
    set QUERY ""
    set QUERY_STRING ""
    set URI_HOST ""
    set URI_PORT 210
    set PATH_INFO ""
    set DATABASENAME ""

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
    # arguments --  index=term & index=term ...
    
    set pathlist [split $PATH_INFO /]
    
    set command ""
    if {[llength $pathlist] > 1} {
	set command [lindex $pathlist 1]
    }
    if {[llength $pathlist] > 2} {
	set DATABASENAME [lindex $pathlist 2]
    }
    

    # Each block should end in throw_result or exit_error
    
    if {$URI_HOST == ""} {
	exit_error "Badly formed URL: No host given."
    } elseif {$command == "" || ($command == "explain" && $DATABASENAME == "" && $QUERY_STRING == "")} {
	search_explain_host
	throw_result
    } elseif {[lsearch [array names command_map] $command] == -1} {
	exit_error "Malformed URL; Unknown Z39.50 command '$command'."
    } elseif {$DATABASENAME == "" && $command == "search" } {
	# No database, but search, query explain for dbs to search
	search_explain_type "databaseInfo"
	throw_result
    } elseif {$command == "explain" && $QUERY_STRING == ""} {
	search_explain_type $DATABASENAME
	throw_result
    } elseif {$command == "explain" } {

	# Doing a weirdo search on explain??
	# We need to work on this.
	
	exit_error "Can't handle that at the moment..."
	return
	

	set searchterms(db) "IR-Explain-1"

	process_query_string
	
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
    } elseif { [lsearch [array names command_map] $command] == -1} {
	#Malformed URL, error
	exit_error "Badly formed URL; unknown command '$command'"
    } elseif {$command == "search" && $QUERY_STRING == "" } {
	set success [search_explain_indexes]
	if {!$success} {
	    strobe_search $URI_HOST $URI_PORT $DATABASENAME
	}
	throw_result
    } elseif {$command == "search"} {
	set command $command_map($command)
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
		
		# XSLT in Mozilla works, can throw XML docs at a Stylesheet
		# Need to format SUTRS and MARC etc
		# NB:  Cheshire SUTRS == SGML
		# Or import GRS handler where we know how
		
		# Need to allow for Summary results linked to full results (eg title format)
		
		set HITS [lindex [lindex [lindex $qresults 0] 2] 1]
		
		set HTML_RESULT "<html><head><title>Z39.50 Search Results</title></head><body bgcolor = white><p>"


		# Need a good way to find out the available elementsets ...
		# Can just try them in some sensible order...

		# Bleh, there must be something better than this :/

		zset recsyntax MARC
		zset elementsetnames F
		while { [lindex [set zdisp [try_zdisplay]] 0] == 0 } {
		    # Try next combo
		    
		    # If no more, break out
		    break
		}
		

		set ridx $FIRST_RECORD
		set RECV_RECORDS [lindex [lindex [lindex $zdisp 0 ] 2] 1]

		append HTML_RESULT "<table>"
		append HTML_RESULT "<p>Your search was submitted and matched $HITS records, $RECV_RECORDS starting at $FIRST_RECORD are shown below:</p>"


		foreach rec [lrange $zdisp 1 end]  { 

		    vwait flipflop
		    if {$flipflop < 0} {
			break
		    }

		    puts stderr "Processing $ridx / $HITS ..."

		    if {$RECORD_FORMAT == "TPGRS" } {
			# Try and do link from title/author to LONG2

			set fmtcmd "zformat SHORT \{$rec\} marc $ridx 2000"
			set err [catch {eval $fmtcmd} outrec]

			if {!$err} {
			    # Get author/title out
			    regexp {Author:([^\n]+)\n} $outrec null author
			    regexp {Title:([^\n]+)\n} $outrec null title

			    append HTML_RESULT "<tr><td>$title</td><td>$author</td></tr>"

			} else {
			    append HTML_RESULT "<tr><td colspan = 2>Broken Record</td></tr>"
			}

		    } else {
			append HTML_RESULT "<tr><td colspan = 2><HR></td></tr>"
		    
			set fmtcmd "zformat $RECORD_FORMAT \{$rec\} marc $ridx 2000"
			set err [catch {eval $fmtcmd} outrec]
			
			if {$err == 0 } {
			    if {[string first  "<br>" $outrec] != -1 } {
				while { [set idx [string first "<br>" $outrec]] != -1 } {
				    append HTML_RESULT "<tr><td valign = top width = 20%><i>[string range $outrec 0 19]</i></td><td>[string range $outrec 20 [expr $idx -1]]</td></tr>"
				    set outrec [string range $outrec [expr $idx +4] end]
				}
			    } else {
				# plain text format
				set lines [split $outrec "\n"]
				foreach l $lines {
				    if {[string range $l 0 6] == "Record"} {
					append HTML_RESULT "<tr><td colspan = 2><b>$l</b></td></tr>"
				    } else {
					append HTML_RESULT "<tr><td valign = top width = 20%><i>[string range $l 0 14]</i></td><td>[string range $l 15 end]</td></tr>"
				    }
				}
			    }
			} else {
			    append HTML_RESULT "<tr><td colspan = 2><center>Record $ridx does not appear to be valid.</center></td></tr>"
			}
			incr ridx
		    }
		}
		append HTML_RESULT "<tr><td colspan = 2><HR></td></tr>"
		append HTML_RESULT "</table>"

		# Generate prev/next forms
		append HTML_RESULT [build_navbar]
		append HTML_RESULT "</body></html>"
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
	set queryargs [generate_scan_query]
	set scanfield [lindex $queryargs 0]
	set scanterm [lindex $queryargs 1]
	set scanpos [lindex $queryargs 2]
	
	set query "zscan $scanfield \{$scanterm\} 0 $NUM_RECORDS $scanpos"

	if { [try_zconnect] == 0} {
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
		
		throw_file "browse.html" [list ]
		
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
		throw_file "browse-bottom.html" [list "\%NAVBAR\% $navbar"]

		throw_result
	    }
	}
    } elseif {$command == "sort"} {
	
    }
}
