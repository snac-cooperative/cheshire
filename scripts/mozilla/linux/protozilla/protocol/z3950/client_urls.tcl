
puts stderr "Loaded clienturls.tcl"

proc delete_resultsets {} {
    global resultsets zsessions QUERY_STRING URI_HOST URI_PORT DATABASENAME 

    set sessionlist [array names zsessions]
    if {$QUERY_STRING != ""} {
	# Should be just sets=(sets)
	set setstr [string range $QUERY_STRING 5 end]
	set setlist [split $setstr ","]

	set resulthostlist [array names resultsets]
	
	foreach set $setlist {

	    set servsetlist [split $set "^"]
	    set serverstr [lindex $servsetlist 0]

	    if {[lsearch $resulthostlist $serverstr] != -1} {
		set setname [lindex $servsetlist 1]

		if {[set sidx [lsearch $resultsets($serverstr) $setname]] != -1 } {

		    regexp  "(.+):(.+)/(.+)" $serverstr null host port db

		    set URI_HOST $host
		    set URI_PORT $port
		    set DATABASENAME $db
		    try_zconnect
		    try_zdelete $setname

		    set newlist [lreplace $resultsets($serverstr) $sidx $sidx]
		    set resultsets($serverstr) $newlist

		}
	    }
	}
    }

}

proc url_sidebar {} {
    global resultsets sidebar_searches zsessions HTML_RESULT URI_HOST URI_PORT DATABASENAME

    set sessionlist [array names zsessions]
    set sessions ""
    foreach sesh $sessionlist {
	if { $sesh != "null" } {
	    regexp  "(.+):(.+)/(.+)" $sesh null host port db
	    if {($db != "IR-Explain-1") && ($zsessions($sesh) != "null")} {

		set URI_HOST $host
		set URI_PORT $port
		set DATABASENAME $db
		# Put in explain_titleString here.
		if { [set title [get_explain_titleString]] == "" } {
		    set title $db
		}

		if {[string length $title] > 30} {
		    set title "[string range $title 0 29]..."
		}

		append sessions "<vbox><checkbox id=\"$sesh\" checked=\"false\" label=\"$host\"/><hbox><spring flex=\"10\"/><html><html:a href=\"z3950://$host:$port/search/$db\" target=\"_content\"><text value=\"$title\"/></html:a></html><spring flex=\"10\"/><html:a href=\"z3950://$host:$port/close/$db?sidebar=true\" border=\"0\"><image src=\"chrome://z3950/content/redx.gif\"/></html:a><spring flex=\"1\"/></hbox></vbox>"
	    }
	}
    }
    
    if {$sessions == ""} {
	set sessions "<html><text value=\"None connected yet.\"/></html>"
    }

    set srchist ""
    foreach srch $sidebar_searches {
	append srchist "<menuitem label=\"$srch\" value=\"$srch\"/>"
    } 

    puts stderr "GOT srchist: $srchist from s_s: $sidebar_searches"

    set resultsetlist [array names resultsets]
    set resultstr ""
    foreach rhost $resultsetlist {
	if {$rhost != "null" && $resultsets($rhost) != [list]} {
	    regexp  "(.+):(.+)/(.+)" $rhost null host port db
	    append resultstr "<treeitem container=\"true\" open=\"false\"> <treerow><treecell class=\"treecell-indent\" label=\"$host ($db)\"/></treerow><treechildren>"

	    foreach set $resultsets($rhost) {
		append resultstr "<treeitem id=\"$host:$port/$db^$set\"><treerow><treecell class=\"treecell-indent\" label=\"$set\"/></treerow></treeitem>";
	    }
	    append resultstr "</treechildren></treeitem>"
	}
    }

    if { $resultstr == "" } {
	set resultstr  "<treeitem><treerow><treecell label=\"No result sets yet.\"/></treerow></treeitem>"
    }

    set HTML_RESULT "Content-type: application/vnd.mozilla.xul+xml\n\n"
    throw_file "html/sidebar.xul" [list "\%SESSIONS\% $sessions" "\%RESULTSETS\% $resultstr" "\%SEARCHISTORY\% $srchist"]
    throw_result
}


proc url_search {} {
    global sidebar_searches HTML_RESULT QUERY_STRING searchterms URI_HOST URI_PORT DATABASENAME RECORD_FORMAT NUM_RECORDS RECV_RECORDS zsessions HITS QUERY

    if { $QUERY_STRING != "" } { 
	# Do Search
	set searchterms(stype) "Search"
	process_query_string

	set hosts [list]
	foreach q [array names searchterms] {
	    if { $searchterms($q) == "srchhost" } {
		lappend hosts $q
	    } elseif {$q == "searchfield" } {
		# HACK!
		set term $searchterms(searchterm)
		set searchterms($searchterms(searchfield)) $term
		if { [lsearch $sidebar_searches $term] == -1 } {
		    puts stderr "APPENDING: $term"
		    lappend sidebar_searches $term
		}
	    }
	}

	if {[llength $hosts] == 0} {
	    exit_error "No hosts supplied to search"
	} elseif { $searchterms(stype) == "Search" && $searchterms(searchterm) == ""} {
	    exit_error "No search term given."
	} elseif { $searchterms(stype) == "Scan" && [llength $hosts] > 1} {
	    exit_error "You can only scan one host at a time."
	} elseif { $searchterms(stype) == "Search" } {
	    # Do searches...  

	    set HTML_RESULT "<html><body bgcolor = white><center><b>MultiHost Search Results</b></center><p>Here are the short results for the servers you selected to search.<p>"
	    set NUM_RECORDS 10

	    foreach hoststr $hosts {
		regexp  "(.+):(.+)/(.+)" $hoststr null host port db	

		set URI_HOST $host
		set URI_PORT $port
		set DATABASENAME $db

		set titlestr [get_explain_titleString]
		set QUERY ""
		if {$titlestr == ""} {
		    set titlestr "$host ($db)"
		}
		
		set connected [try_zconnect]
		if {$connected != 0} {
		    # Skip host
		append HTML_RESULT "<p><b>$host ($db):</b> Could not connect to host."
		} else {

		    set qresults [try_zsearch]
		    if {[lindex $qresults 0] == 0 } {
			# No results, or error
			append HTML_RESULT "<p><b>$titlestr:</b> No matching records."
		    } else {
			set HITS [lindex [lindex [lindex $qresults 0] 2] 1]
			set RECORD_FORMAT "SUMMARY"


			zremoveformat LONG2
			source "display.tcl"

			set htmlres [try_display_records]

			if {$HITS > $RECV_RECORDS} {
			    set more "<a href=\"z3950://$host:$port/search/$db?$searchterms(searchfield)=$searchterms(searchterm)\&format=SUMMARY\">more</a>"
			} else {
			    set more ""
			}
			append HTML_RESULT "<p><table width = 100%><tr><td><b>$titlestr:</b> (Matched  $HITS Records)</td><td align = right>$more</td></tr></table>"
			append HTML_RESULT $htmlres
		    }
		}
	    }
	    append HTML_RESULT "</body></html>"
	    throw_result

	} else {
	    # Do Scan.

	    set hoststr [lindex $hosts 0]
	    regexp  "(.+):(.+)/(.+)" $hoststr null host port db	
	    set URI_HOST $host
	    set URI_PORT $port
	    set DATABASENAME $db
	    


	    process_query_string

	    if { [set err [try_zconnect]] == 0} {

		set searchterms(scanfield) $searchterms(searchfield)
		set searchterms(scanterm) $searchterms(searchterm)
		
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
		
		    throw_file "html/browse.html" [list ]
		
		    
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
	    } else {
		exit_error "Connection Error: $err"
	    }
	} 
	
    } else {

	set sessionlist [array names zsessions]
	set sessions ""
	foreach sesh $sessionlist {
	    if { $sesh != "null" } {
		# host:port/db
		regexp  "(.+):(.+)/(.+)" $sesh null host port db
		if {$db != "IR-Explain-1"} {
		    append sessions "<tr><td align = top width = 5%><input type = \"checkbox\" name =\"$sesh\" value =\"srchhost\"></td><td><a href= \"z3950://$host:$port/\" target=\"_CONTENT\">$host</a> (<a href =\"z3950://$host:$port/search/$db\" target=\"_content\">$db</a>)</td><tr>"
		}
	    }
	}
	
	# Generate this from values? or Just put in common ones?
	set fields "<option value = 1003>Author</option><option value = 4>Title</option><option value=30>Date</option>"
	if {$sessions == ""} {
	    set sessions "<tr><td colspan =2><i>None connected yet</i></td></tr>"
	}
	
	throw_file "html/multisearch.html" [list "\%HOSTS\% $sessions" "\%OPTIONS\% $fields"]
	throw_result
    }
}
