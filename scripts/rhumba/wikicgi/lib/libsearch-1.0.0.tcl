
set relevance_indexes {topic any anywhere}
set ctime_indexes {date before_date after_date}
set within_indexes {date_range}

# XXX Finish me! =)

# --- Search functions and subroutines ---

proc function_search {} {
    global indata global_variables maxfield SCRIPT_NAME
    foreach var $global_variables {
	global $var
    }


    # Shouldn't do this, but lets overload for easy user searches.
    if {$maxfield == 0 && [lsearch [array names indata] "user"] != -1} {
    set who $indata(user)
	if {[get_user_info $who] == 0} {
	    throwfile "err.html" [list "\%ERR\% '$who' is not a known user."]
	} else {
	    display_user_info $who
	}
	exit
    }

    set db $indata(searchdb)
    if {$db == "data"} {
	set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
	set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
    } elseif {$db == "user"} {
	set CHESHIRE_DATABASE $CHESHIRE_DATABASE_USERS
	set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
    } else {
	throwfile "err.html" [list "\%ERR\% '$db' is not a searchable database. Please use 'user' or 'data'."]
	exit
    }

    set format $indata(format)
    switch $format {
	"titles" {

	    set CHESHIRE_NUMREQUESTED $indata(numreq)
	    set CHESHIRE_NUM_START $indata(firstrec)

	    set query [generate_query]
	    set tstr [time {set err [catch {eval $query} qresults]} 1]
	    set tstr [lindex $tstr 0]
	    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	    set sresults [lrange $qresults 1 end]

	    # Check for errors etc
	    
	    if {$err != 0} {
		throwfile "err.html" [list "\%ERR\% A search error occured: $err"]
	    } elseif {$hits == 0} {
		throwfile "err.html" [list "\%ERR\% No records matched that search."]
	    } else {

		set tstr [string range $tstr 0 [expr [string length $tstr] -4]]
		set tstr [expr $tstr / 1000.0]
		set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
		set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]

		throwfile "titles.html" [list "\%TIME\% $tstr" "\%HITS\% $hits" "\%STARTREC\% $startrec" "\%RETURNED\% $returned"]
		
		puts "<table>"
		foreach result $sresults {
		    if {$db == "data"} {
			set info [flatten_doc_metagrs [list $result]]
			puts "<tr><td><a href = \"[lindex $info 2]\">[lindex $info 0]</a></td></tr>"
		    } else {
			set info [flatten_user_metagrs [list $result]]
			puts "<tr><td><a href = \"?request=search&user=[lindex $info 0]\">[lindex $info 0]</a></td></tr>"
		    }
		}
		puts "</table>"
	    }
	    puts "<P><hr></body></html>"

	}
	"scan" {

	    set innames [array names indata]
	    if {[lsearch $innames "scanindex"] == -1 || $indata(scanindex) == ""} {
		throwfile "err.html" [list "\%ERR\% No index was given to browse."]
		exit
	    } elseif { [lsearch $innames "scanterm"] == -1 || $indata(scanterm) == ""} {
		set indata(scanterm) "aaa"
	    }


	    set numreq $indata(numreq)
	    set firstrec $indata(firstrec)
	    set scanindex $indata(scanindex)
	    set scanterm $indata(scanterm)

	    # lscan $index $term 0 $numrequested $firstrec
	    set query "lscan $scanindex $scanterm 0 $numreq $firstrec"
	    set tstr [time {set err [catch {eval $query} qresults]} 1]
	    set tstr [lindex $tstr 0]

	    if {$err != 0} {
		throwfile "err.html" [list "\%ERR\% A browse error occured: $err"]
	    } else {
		set sresults [lrange $qresults 1 end]

		set firstterm ""
		set lastterm ""
		set prevterm ""
		set foundterm 0
		set termidx 0

		set tstr [string range $tstr 0 [expr [string length $tstr] -4]]
		set tstr [expr $tstr / 1000.0]
		throwfile "browse.html" [list "\%TIME\% $tstr"]
		puts "<table border = 1>"
		puts "<tr><td width = 80%><b>Term</b></td><td><b>Records</b></td></tr>"

		foreach result $sresults {
		    set term [lrange $result 0 [expr [llength $result] -2]]
		    
		    if { [string index $term 0] == "{" } {
			set term [string range $term 1 [expr [string length $term] -2]]
		    }

		    set recs [lindex $result [expr [llength $result] - 1]]

		    

		    if {$term == ">>>Global Data Values<<<" || $recs == 0} {
			set firstterm ""
		    } else {
			
			if {$termidx == 1} {
			    set firstterm $term
			} elseif { $termidx == $numreq } {
			    set lastterm $term
			}
			
			set termtxt $term
			if { [string tolower $term ] == [ string tolower $scanterm] } {
			    set termtxt "<b>$termtxt</b>"
			}

			if {$foundterm == 0 && $term > $scanterm && $prevterm < $scanterm && $scanterm != "aaa"} {
			    puts "<tr><td colspan = 2><center><b>Your term '$scanterm' would be here.</b></center></td></tr>";
			    set foundterm 1
			}
			
			if {$recs == 1} {

			    set CHESHIRE_NUMREQUESTED 1
			    set CHESHIRE_ELEMENTSET METAGRS

			    set searchquery "search $scanindex {\"$term\"}"
			    set err [ catch { eval $searchquery } srchresults ]
			    set hits [lindex [lindex [ lindex $srchresults 0] 0] 1]
			    set record [lindex $srchresults 1]
			    if {$err != 0 || $hits == 0 } {
				puts "<i>BROKEN INDEX: '$term' - hits: $hits</i><br>"
			    } else {

				if {$db == "data"} {
				    set info [flatten_doc_metagrs [list $record]]
				    set href [lindex $info 2]
				} else {
				    set info [flatten_user_metagrs [list $record]]
				    set href "$SCRIPT_NAME/?request=search&user=[lindex $info 0]"
				}
				    
				puts "<tr><td><a href = \"$href\">$term</a></td><td>$recs</td></tr>"
			    }
			    
			} else {
			    # create search href
			    set href "request=search&searchdb=$db&fieldidx1=$scanindex&fieldcont1=$term"
			    puts "<tr><td><a href = \"$SCRIPT_NAME/?$href\">$term</a></td><td>$recs</td></tr>"
			}
		    }
		    set prevterm $term
		}
		puts "</table>"
		puts "<p><hr></body></html>"

	    }
	}
	default {
	    # Unknown format
	    throwfile "err.html" [list "\%ERR\% '$format' is not a known result format. Valid formats are: titles, scan"]
	}
    }
}


# --- All The Usual Suspects ---

proc generate_query {} {
    global indata maxfield relevance_indexes ctime_indexes within_indexes

    set query "search "
    set firstterm 1
    set bool $indata(bool)
    
    for {set n 1} {$n <= $maxfield} {incr n} {
	
	set i "fieldidx$n"
	set index $indata($i)

	set t "fieldcont$n"
	if {[lsearch [array names indata] $t] == -1} {
	    continue
	}
	set term $indata($t)
	
	if {$index != "" && $index != "none" && $term != ""} {
	    if {$firstterm == 0} {
		append query " $bool "
	    }
	    if {[lsearch $relevance_indexes $index] != -1 } {
		append query " ($index @ \{$term\} )"
	    } elseif { [lsearch $ctime_indexes $index] != -1 } {
		# Set some aliases
		set now [clock seconds]
		set day [ expr 24 * 60 * 60 ];
		set week [ expr 7 * $day ]
		set month [ expr 31 * $day ]
		
		if {[string range $index 0 6] == "before_"} {
		    append query " ([string range $index 7 end] < \{$term\} )"
		} elseif {[string range $index 0 5] == "after_"} {
		    append query " ([string range $index 6 end] > \{$term\} )"
		} else {
		    if {$term == "day"}  {
			set stime [clock format  [expr $now - $day]]
		    } elseif {$term == "week"} {
			set stime [clock format [expr $now - $week]]
		    } elseif {$term == "month"} {
			set stime [clock format [expr $now - $month]]
		    } elseif {$term == "hour"} {
			set stime [clock format [expr $now - 3600]]
		    }
		    append query " ($index > \{$stime\} )"
		}
	    } elseif { [lsearch $within_indexes $index] != -1 } {
		append query " $index <=> \{$term\} )"
	    } else {
		append query " ($index \{$term\} )"
	    }
	    set firstterm 0
	}
    }

    if {$query == "search "} {
	throwfile "err.html" [list "\%ERR\% No search terms were given."]
	exit
    }
    return $query
}


proc from_ctime {time} {

    set day [string range $time 8 9]
    set month [string range $time 4 6]
    set year [string range $time [expr [string length $time] - 5] end]
    set hour [ string range $time 11 18]
    set monthn [expr [lsearch {Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec} $month] +1] 
    if {$monthn < 10} {
        set monthn "0$monthn"
    }
    regsub -all { } $day "0" day
    regsub -all ":" $hour "" hour
    
    return  "$year$monthn$day$hour"
}


