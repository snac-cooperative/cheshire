#!/home/cheshire/cheshire/bin/webcheshire
#Author:  Rob Sanderson (azaroth@liv.ac.uk)
#Copyright:  &copy; (c) Rob Sanderson All Rights Reserved
#Last Modified: 12/03/01

set CHESHIRE_ROOT "/home/cheshire/cheshire/l5r"
set CGI_PATH "/home/cheshire/public_html/cgi-bin"
set BASE_URL "http://gondolin.hist.liv.ac.uk/~cheshire/l5r/"
set CGI_URL "http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/"

# Global Server Configuration
set CHESHIRE_DATABASE l5r
set CHESHIRE_CONFIGFILE "$CHESHIRE_ROOT/DBCONFIGFILE"
set CHESHIRE_ELEMENTSET_TITLES TPGRS
set CHESHIRE_RECSYNTAX_TITLES GRS1
set CHESHIRE_ELEMENTSET_FULL F
set CHESHIRE_RECSYNTAX_FULL GRS1

set OUTPUT_TO_FILE 0
set OUTPUT_TO_STDOUT 1

# Initialise for template files
puts "Content-type: text/HTML\n"
cd "$CHESHIRE_ROOT"

# Import our libraries
source "$CGI_PATH/lib/libgrs-1.0.0.tcl"
source "$CGI_PATH/lib/libsearch-1.0.0.tcl"
source "$CGI_PATH/lib/libcgi-1.0.0.tcl"


#---------- Subroutines ----------

proc handle_format_full { record } {

#    source "$SCRIPT_FILENAME.grsconfig"
    
    set sresults [ lindex $record 1]

    regsub -all "{tagType:: 3}" $sresults "" sresults
    regsub -all "{tagType:: 2}" $sresults "" sresults
    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
    regsub -all "\"" $sresults {\&quot;} sresults

    foreach item [lrange $sresults 0 end] {
	processitem $item
    }
    print "<p><hr>"
    
}

proc handle_format_titles {record} {
    global indata SCRIPT_NAME
	
    set record [lrange $record 1 end]
    set relevance [lindex [lindex [lindex $record 0] 1] 1]
    set docid [lindex [lindex [lindex $record 1] 1] 1]
    set title [lindex [lindex [lindex $record 2] 1] 1]
    set title [ textsub $title ]
	
    set relv [string range $relevance 0 [expr [string length $relevance] -2]]
    append relv "%"
	
    set href "$SCRIPT_NAME?format=full&fieldidx1=docid&fieldcont1=$docid&firstrec=1&numreq=1"
	
    set line "<tr><td><a href=\"$href\" target =\"dest\">$title</a></td><td align = right>$relv</td></tr>"
    
    return $line
}

proc handle_format_scan_missedterm {term} {
    return "<tr><td colspan=2><b>Your term '$term' would be here</b></td></tr>"
}

proc handle_format_scan_single {record term is_scanterm} {
    global SCRIPT_NAME
    set docid [lindex [lindex [lindex $record 1] 1] 1]
    set href "$SCRIPT_NAME?format=full&fieldidx1=docid&fieldcont1=$docid&firstrec=1&numreq=1"
	
    if {$is_scanterm} {
	set term "<b>$term</b>"
    }

    set line "<tr><td><a href=\"$href\" target =\"dest\">$term</a></td><td align=\"right\">1</td></tr>"
    return $line
}

proc handle_format_scan_multiple {term recs is_scanterm} {
    global indata SCRIPT_NAME

    set index $indata(scanindex)
    set numreq $indata(numreq)
    if {$is_scanterm} {
	set termtxt "<b>$term</b>"
    } else {
	set termtxt "$term"
    }

    set term [cgisub $term]
    set href "$SCRIPT_NAME?format=titles&fieldidx1=$index&fieldcont1=$term&numreq=$numreq"

    return "<tr><td><a href = \"$href\">$termtxt</a></td><td align=\"right\">$recs</td></tr>"

}


proc processitem item {
    global element_order format
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
		set string [ textsub $string ]
		print "$string"
	    }
	    
	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::"} { 
	
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1] 
	set elemname [string toupper $elemname]
	if {[llength [lindex $item 1]] == 3} { 
	    #Is an attribute Do we care? Not unless we're an isad(g) template. 
	    return 
	}  
	
	
	#Ignore tag name 19 - it's a cheshire holder for plain
	if { $elemname != "19" } {
	    lappend element_order $elemname 
	}
	set appendtxt "" 
	set appendeval "" 
	
	# Process SGML

	if {$elemname == "NAME"} {
	    print "<center><b>"
	    set appendtxt "</b></center>"
	} elseif {$elemname == "TEXT"} {

	    set type [sub_elem_contents 0 $item]
	    set text [sub_elem_contents 1 $item]

	    if {$text != "" && $text != "elementEmpty"} {
		print "<p>"
		set appendtxt "</p>"
		if {$type == "flavour"} {
		    print "<i>"
		    set appendtxt "</i></p>"
		} elseif {$type == "errata"} {
		    print "<b>Errata:</b> "
		}
	    }
	} elseif {$elemname == "COST"} {
	    # need to parse attrs
	    print "<br><b>[sub_elem_contents 1 $item]:</b>"
	} elseif {$elemname == "STAT"} {
	    # need to parse attrs
	    print "<br><b>[string tolower [sub_elem_contents 0 $item]]:</b>"

	} elseif {$elemname == "SET"} {
	    print "<p><b>Set:</b>"
	} elseif {$elemname == "GAME"} {
	    return
	} elseif {$elemname == "LEGAL"} {
	    print "<br><b>Legality:</b>"
	} elseif {$elemname == "TYPE"} {
	    print "<br><b>Card Type:</b>"
	} elseif {$elemname == "ARTIST"} {
	    print "<b>Artist:</b>"
	    set appendtxt "<br>"
	} elseif {$elemname == "RARITY"} {
	    print "<br><b>Rarity:</b>"
	} elseif {$elemname == "COLLECTION"} {
	    print "<p><b>Collection:</b><ul>"
	    set appendtxt "</ul>"
	} elseif {$elemname == "INSTANCE"} {
	    set num [lindex [lindex [lindex [lindex [lindex [lindex [ lindex $item 1] 1] 1] 0] 1] 1] 1]
	    print "<li>$num in "
	}
	
	
	# --- Process sub items of list  ---
	processitem [lindex [lindex $item 1] 1]
	
	# Things to be done on close of element
	if {$appendtxt != "" } {
	    print $appendtxt
	}
    }
}



#------------------------------------------------------------ 
#   Main Function
#

set format $indata(format)

handle_search_$format
exit
set err [catch "handle_search_$format" fsres]
if {$err} {
    puts stderr "\[l5rsearch error\]: $fsres"	
    throwfile "err.html" [list "\%ERR\% '$format' is not a known result format. Valid formats are: titles, scan, full, titleframes, scanframes " ]
}

#
#------------------------------------------------------------
