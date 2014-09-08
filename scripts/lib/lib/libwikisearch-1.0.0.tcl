
# First of all, include all of the default functions
source "lib/libsearch.tcl"

# --- Search functions and subroutines ---

proc function_search {} {
    global indata global_variables maxfield SCRIPT_NAME
    foreach var $global_variables {
	global $var
    }


    if {$maxfield == 0 && [lsearch [array names indata] "user"] != -1} {
    set who $indata(user)
	if {[get_user_info $who] == 0} {
	    throwfile "err.html" [list "\%ERR\% [format $E_NOUSER $who]"]
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
	throwfile "err.html" [list "\%ERR\% [format $E_NODATABASE $db]"]
	exit
    }

    # Now we're configured for Wiki, we can call out to the default search routines.

    set format $indata(format)
    set err [catch "handle_search_$format" fsres]
    if {$err} {
	throwfile "err.html" [list "\%ERR\% [format $E_NOFORMAT $format]"]
    }
}


# --- Handlers called from library ---

proc handle_format_titles {result} {
    global indata
    set db $indata(searchdb)

    if {$db == "data"} {
	set info [flatten_doc_metagrs [list $result]]
	return "<tr><td><a href = \"[lindex $info 2]\">[lindex $info 0]</a></td></tr>"
    } else {
	set info [flatten_user_metagrs [list $result]]
	return "<tr><td><a href = \"?request=search&user=[lindex $info 0]\">[lindex $info 0]</a></td></tr>"
    }
}


proc handle_format_scan_missedterm {scanterm} {
    return "<tr><td colspan = 2><center><b>Your term '$scanterm' would be here.</b></center></td></tr>";
}

proc handle_format_scan_single {record term} {
    global indata
    set db $indata(searchdb)
    
    if {$db == "data"} {
	set info [flatten_doc_metagrs [list $record]]
	set href [lindex $info 2]
    } else {
	set info [flatten_user_metagrs [list $record]]
	set href "$SCRIPT_NAME/?request=search&user=[lindex $info 0]"
    }

    return "<tr><td><a href = \"$href\">$term</a></td><td>1</td></tr>"
    
}

proc handle_format_scan_multiple {term recs} {
    global indata SCRIPT_NAME
    
    set db $indata(searchdb)
    set scanindex $indata(scanindex)

    set href "request=search&searchdb=$db&fieldidx1=$scanindex&fieldcont1=$term"
    return "<tr><td><a href = \"$SCRIPT_NAME/?$href\">$term</a></td><td>$recs</td></tr>"
}

proc get_max_docid_wiki {db} {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    if {$db == "user"} {
	set ccf $CHESHIRE_CONFIGFILE_USER
	set cdb $CHESHIRE_DATABASE_USER
    } elseif {$db == "data"} {
	set ccf $CHESHIRE_CONFIGFILE_DATA
	set cdb $CHESHIRE_DATABASE_DATA
    } else {
	set ccf $CHESHIRE_CONFIGFILE
	set cdb $CHESHIRE_DATABASE
    }

    return [get_max_docid $ccf $cdb]


}
