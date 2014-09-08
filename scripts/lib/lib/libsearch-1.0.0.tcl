
set relevance_indexes {topic any anywhere}
set ctime_indexes {date before_date after_date}
set within_indexes {date_range}
set cheshire_vars {CHESHIRE_DATABASE CHESHIRE_CONFIGFILE CHESHIRE_ELEMENTSET CHESHIRE_NUMREQUESTED CHESHIRE_NUM_START CHESHIRE_RECSYNTAX}

# --- Search functions and subroutines ---

proc generate_sort {} {
    global indata 

    set resultsetname "SearchResults"
    set sortcmd "cheshire_sort -IN $resultsetname -OUT $resultsetname"
    set sortargs ""
    set names [array names indata]

    set num 1

    while { [set idx [ lsearch $names "sorttype$num"]] != -1 } {
	
	set type $indata(sorttype$num);
	set field $indata(sortfield$num);

	if {$field != "" } {
	
	    set bit " -[string toupper $type] $field"
	    
	    if { [lsearch $names "sortdescend$num"] != -1 && $indata(sortdescend$num) == 1} {
		append bit " -DESCENDING"
	    }
	    if { [lsearch $names "sortcase$num"] != -1 && $indata(sortcase$num) ==  1} {
		append bit " -CASE_SENSITIVE"
	    }
	    if { [lsearch $names "sortmissing$num"] != -1 } {
		set missing $indata(sortmissing$num)
		append bit " -MISSING_VALUE $missing"
	    }
	    
	    append sortargs $bit
	}
	incr num
    }

    if {$sortargs != ""} {
        return "$sortcmd $sortargs"
    } else {
        return 0
    }

}


proc generate_query {} {
    global indata maxfield relevance_indexes ctime_indexes within_indexes E_NOTERM

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

	set fbool $bool
	if {[lsearch [array names indata] "fieldbool$n"] != -1} {
	    set fbool $indata(fieldbool$n)
	}

	set foper ""
	if {[lsearch [array names indata] "fieldoper$n"] != -1} {
	    set foper $indata(fieldoper$n)
	}
	
	if {$index != "" && $index != "none" && $term != ""} {
	    if {$firstterm == 0 || $fbool == "NOT"} {
		append query " $fbool "
	    }

	    if {$foper != "" } {
		append query " ($index $foper \{$term\} )"
	    } elseif {[lsearch $relevance_indexes $index] != -1 } {
		append query " ($index @ \{$term\} )"
	    } elseif { [lsearch $ctime_indexes $index] != -1 } {
		# Set some aliases
		set now [clock seconds]
		set day [ expr 24 * 60 * 60 ];
		set week [ expr 7 * $day ]
		set month [ expr 31 * $day ]
		set fortnight [expr $week * 2]
		set sixmonth [expr $month * 6]
		
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
		    } elseif {$term == "fortnight"} {
			set stime [clock format [expr $now - $fortnight]]
		    } elseif { $term == "sixmonth"} {
			set stime [clock format [expr $now - $sixmonth]]
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
	throwfile "err.html" [list "\%ERR\% $E_NOTERM"]
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


proc handle_search_titleframes {} {
    global indata
    set indata(format) "titles";
    set cgiquery [generate_cgihref 3]
    throwfile frameset.html [list "\%QUERY\% $cgiquery"]
}

proc handle_search_scanframes {} {
    global indata
    set indata(format) "scan";
    set cgiquery [generate_cgihref 3]
    throwfile frameset.html [list "\%QUERY\% $cgiquery"]
}

proc handle_search_titles {} {
    global CHESHIRE_ELEMENTSET CHESHIRE_RECSYNTAX CHESHIRE_ELEMENTSET_TITLES CHESHIRE_RECSYNTAX_TITLES

    set CHESHIRE_ELEMENTSET $CHESHIRE_ELEMENTSET_TITLES
    set CHESHIRE_RECSYNTAX $CHESHIRE_RECSYNTAX_TITLES
    return [do_search "titles"]
}

proc handle_search_full {} {
    global CHESHIRE_ELEMENTSET CHESHIRE_RECSYNTAX CHESHIRE_ELEMENTSET_FULL CHESHIRE_RECSYNTAX_FULL

    set CHESHIRE_ELEMENTSET $CHESHIRE_ELEMENTSET_FULL
    set CHESHIRE_RECSYNTAX $CHESHIRE_RECSYNTAX_FULL
    return [do_search "full"]
}


proc do_search { format } {
    global indata cheshire_vars E_SEARCHERR E_NOMATCH
    foreach var $cheshire_vars {
	global $var
    }

    set CHESHIRE_NUMREQUESTED $indata(numreq)
    set CHESHIRE_NUM_START $indata(firstrec)

    set query [generate_query]
    if {[set srtcmd [generate_sort]] != 0} {
	append query " resultsetid SearchResults"
    }

    set tstr [time {set err [catch {eval $query} qresults]} 1]

    if {$srtcmd != 0 } {
	set sortres [eval $srtcmd]
	set qresults [fetch SearchResults]
    }	    

    set tstr [lindex $tstr 0]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
    set sresults [lrange $qresults 1 end]

    # Check for errors etc
	    
    if {$err != 0} {
	throwfile "err.html" [list "\%ERR\% [format $E_SEARCHERR $err]"]
    } elseif {$hits == 0} {
	throwfile "err.html" [list "\%ERR\% $E_NOMATCH"]
    } else {

	set tstr [string range $tstr 0 [expr [string length $tstr] -4]]
	set tstr [expr $tstr / 1000.0]
	set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
	set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]

	# Need to generate a navigation bar within the resultset	
	set navbar [generate_navbar $returned $startrec $hits]

	throwfile "$format.html" [list "\%TIME\% $tstr" "\%HITS\% $hits" "\%STARTREC\% $startrec" "\%RETURNED\% $returned" "\%NAVBAR\% $navbar"]
	foreach result $sresults {
	    puts [handle_format_$format $result]
	}
	throwfile "$format-bottom.html" [list "\%TIME\% $tstr" "\%HITS\% $hits" "\%STARTREC\% $startrec" "\%RETURNED\% $returned" "\%NAVBAR\% $navbar"]
    }
}


proc handle_search_scan {}  {
    global indata cheshire_vars CHESHIRE_ELEMENTSET_TITLES CHESHIRE_RECSYNTAX_TITLES E_NOINDEX E_SCANERR
    foreach var $cheshire_vars {
	global $var
    }

    set innames [array names indata]
    if {[lsearch $innames "scanindex"] == -1 || $indata(scanindex) == ""} {
	throwfile "err.html" [list "\%ERR\% $E_NOINDEX"]
	exit
    } elseif { [lsearch $innames "scanterm"] == -1 || $indata(scanterm) == ""} {
	set indata(scanterm) "000"
    }
    

    set numreq $indata(numreq)
    set firstrec $indata(firstrec)
    set scanindex $indata(scanindex)
    set scanterm $indata(scanterm)
    # This defaults to middle
    set position $indata(scanposition)

    if { $position == "first" } {
	set scanpos 1
    } elseif { $position == "last" } {
	set scanpos $numreq
    } else {
	set scanpos [expr $numreq / 2]
    }


    set query "lscan $scanindex {$scanterm} 0 $numreq $scanpos"

    set tstr [time {set err [catch {eval $query} qresults]} 1]

    if {$err != 0} {
	throwfile "err.html" [list "\%ERR\% [format $E_SCANERR $err]"]
    } else {
	set sresults [lrange $qresults 1 end]

	set firstterm ""
	set lastterm ""
	set prevterm ""
	set foundterm 0
	set termidx 0
	set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
	set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]

	set tstr [lindex $tstr 0]
	set tstr [string range $tstr 0 [expr [string length $tstr] -4]]
	set tstr [expr $tstr / 1000.0]
	throwfile "browse.html" [list "\%TIME\% $tstr" "\%RETURNED\% $returned" "\%STARTREC\% $startrec"]

	foreach result $sresults {
	    set term [lrange $result 0 [expr [llength $result] -2]]
	    set recs [lindex $result [expr [llength $result] - 1]]
	    incr termidx

	    # This is only for versions < 2.32
	    if {$term == ">>>Global Data Values<<<" || $recs == 0} {
		set firstterm ""
	    } else {
			
		if {$termidx == 1} {
		    set firstterm $term
		} elseif { $termidx == $numreq } {
		    set lastterm $term
		}
		
		set is_scanterm 0
		if { [string tolower $term ] == [ string tolower $scanterm] } {
		    set is_scanterm 1
		}
		if {$foundterm == 0 && $term > $scanterm && $prevterm < $scanterm && $scanterm != "000"} {
		    puts "[handle_format_scan_missedterm $scanterm]"
		    set foundterm 1
		}
			
		if {$recs == 1} {
		    
		    set CHESHIRE_NUMREQUESTED 1
		    set CHESHIRE_ELEMENTSET $CHESHIRE_ELEMENTSET_TITLES
		    set CHESHIRE_RECSYNTAX $CHESHIRE_RECSYNTAX_TITLES
		    
		    set searchquery "search $scanindex {\"$term\"}"
		    set err [ catch { eval $searchquery } srchresults ]
		    set hits [lindex [lindex [ lindex $srchresults 0] 0] 1]
		    set record [lindex $srchresults 1]
		    if {$err != 0 || $hits == 0 } {
			puts "<i>BROKEN INDEX: '$term' - hits: $hits</i>"
		    } else {
			puts "[handle_format_scan_single $record $term $is_scanterm]"
		    }
		    
		} else {
		    # create search href
		    puts "[handle_format_scan_multiple $term $recs $is_scanterm]"
		}
	    }
	    set prevterm $term
	}
	set navbar [generate_scan_navbar $firstterm $lastterm]
	throwfile "browse-bottom.html" [list "\%NAVBAR\% $navbar"]
    }
}

