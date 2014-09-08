#!/home/cheshire/cheshire/bin/webcheshire
# --- Setup Global Variables ---

source "lib/libcgi-1.0.0.tcl"
source "lib/libsearch-1.0.0.tcl"
source "lib/libgrs-1.0.0.tcl"

set DATABASE_ROOT "/home/cheshire/cheshire/unesco"
set CHESHIRE_DATABASE unesco
set CHESHIRE_CONFIGFILE "$DATABASE_ROOT/DBCONFIGFILE"

set CHESHIRE_ELEMENTSET_TITLES TPGRS
set CHESHIRE_RECSYNTAX_TITLES GRS1
set CHESHIRE_ELEMENTSET_FULL F
set CHESHIRE_RECSYNTAX_FULL GRS1

set CGI_PATH "/home/cheshire/public_html/cgi-bin"
set HTML_ROOT "/home/cheshire/public_html/unesco"
set TEMPLATE_ROOT "/home/cheshire/cheshire/unesco/templates"
set BASE_URL "http://gondolin.hist.liv.ac.uk/~cheshire/unesco/"
set CGI_URL "http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/"

set OUTPUT_TO_STDOUT 1
set OUTPUT_TO_FILE 0

set linklist {UF USE BT NT RT HEADING}
set startlist {"Use For" "Use Term" "Broader Term" "Narrower Term" "Related Term"  "Top Level"}

set bt_term ""
set heading_term ""
set name_term ""
set tree_text ""

puts "Content-type: text/HTML\n"
cd "~cheshire/cheshire/paleo"


#
# ----- SUBROUTINES -----
#

proc handle_format_full {sresults} {
    global indata bt_term heading_term name_term tree_text

    regsub -all "{tagType:: 3}" $sresults "" sresults
    regsub -all "{tagType:: 2}" $sresults "" sresults
    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
    regsub -all "\"" $sresults {\&quot;} sresults

    set item $sresults
    
    puts "<p><hr><p>"

    set heading_term ""
    set bt_term ""
    set name_term ""

#    puts "<pre>$item</pre>"
    processitem $item
    
    set asarray "new Array(\"$name_term\", \"\",\"\",\"\", \"unesco\")"
    
    puts "<form><input type = \"button\" value = \"Add to Form\"onClick=\'addSubject($asarray)\'></form>"
    
    if {$heading_term != "" || $bt_term != ""} {
	puts "<P><b>Subject Tree</b>: "
	set tree_list { }
	
	#aberrant case of being the top level and wanting a tree. Don't try reprocessing
	if {$heading_term != ""} {
	    if {$heading_term == $name_term} {
		set tree_list [list $heading_term]
	    } else {
		set tree_list [list $heading_term $name_term]
	    }
	} else {
	    set  tree_list [linsert $tree_list 0 $name_term]
	    
	    while {$heading_term == "" } {
		# Search first using current record parsed when displaying.
		
		set CHESHIRE_NUM_START 1
		set CHESHIRE_NUMREQUESTED 1
		set CHESHIRE_RECSYNTAX "GRS1"
		
		set query "search xtitle \{\"$bt_term\"\}"
		set err [catch {eval $query} qresults]
		set sresults [lrange $qresults 1 end]
		set hits [lindex [lindex [lindex $qresults 0] 0] 1]
		
		if {$err != 0 || $hits == 0} {
		    puts "<br><b>Could not generate tree!</b>"
		    puts "<br><b>Query</b>: $query"
		    puts "<br><b>Error</b>: $qresults"
		    set heading_term "error"
		    set tree_list {}
		} else {
		    regsub -all "{tagType:: 3}" $sresults "" sresults
		    regsub -all "{tagType:: 2}" $sresults "" sresults
		    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
		    regsub -all "\"" $sresults {\&quot;} sresults
		    
		    foreach item $sresults {
			processtree $item
		    }
		    
		    if {$heading_term == "" || $heading_term == $name_term} {
			set tree_list [linsert $tree_list 0 $name_term]
		    } else {
			set tree_list [ linsert $tree_list 0 $heading_term $name_term]
		    }
		}
	    }
	}	    
	
	foreach level [lrange $tree_list 0 [expr [llength $tree_list] -2]] {
	    puts -nonewline " $level - "
	}
	puts [lindex $tree_list [expr [llength $tree_list] - 1]]
	puts "<p>\[<b><a href = \"/~cheshire/unesco/\">home</a></b>\]"
    }
}

proc handle_format_titles {record} {
    global indata SCRIPT_NAME
	
    set record [lrange $record 1 end]
    set relevance [lindex [lindex [lindex $record 1] 1] 1]
    set docid [lindex [lindex [lindex $record 0] 1] 1]
    set title [lindex [lindex [lindex $record 2] 1] 1]
    set title [ textsub $title ]
	
    set relv [string range $relevance 0 [expr [string length $relevance] -2]]
    append relv "%"
	
    set href "$SCRIPT_NAME?format=full&fieldidx1=docid&fieldcont1=$docid&firstrec=1&numreq=1"
	
    set line "<tr><td><a href=\"$href\" target =\"dest\">$title</a></td><td align = right>$relv</td></tr>"
    
    return $line
}

proc handle_format_scan_missedterm {term} {
    return "<tr><td colspan=2><b><center>Your term '$term' would be here</center></b></td></tr>"
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

    if {$is_scanterm} {
	set termtxt "<b>$term</b>"
    } else {
	set termtxt $term
    }

    set index $indata(scanindex)
    set numreq $indata(numreq)
    set term [cgisub $term]

    set href "$SCRIPT_NAME?format=titles&fieldidx1=$index&fieldcont1=$term&numreq=$numreq"
    return "<tr><td><a href = \"$href\">$termtxt</a></td><td align=\"right\">$recs</td></tr>"

}




# ----  Main GRS1 Processing Routine ----

proc processitem item {
    global element_order OUTPUT_TO_FILE indata linklist startlist endlist bt_term heading_term name_term;
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
		set string [ textsub $string ]
		if { [regexp -nocase {(^| )(http://[^ ,!;?]+)(\. |[ ,!;?])} $string null null href ] } {
		    regsub -all $href $string "<a href = \"$href\">$href</a>" string 
		}
		puts "$string"
	    }
	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::" || [lindex $item 0] == "GenericRecord::"} { 
	
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1] 
	set elemname [string toupper $elemname]
	set done 0

	#Ignore tag name 19 - it's a cheshire holder for plain text
	if { $elemname != "19" } {
	    lappend element_order $elemname 
	}
	
	set appendtxt "" 
	set appendeval "" 
	
	#Processing here

	if {$elemname == "NAME" } {
	    puts "<center><h3>"

	    set text [flattext [lindex [lindex [lindex $item 1] 1] 1] "none"]
	    set name_term $text

	    set appendtxt "</h3></center>"
	} elseif {$elemname == "INFO" } {
	    set done 1
	} elseif { [set llidx [lsearch $linklist $elemname]] != -1 } {
	    set start [lindex $startlist $llidx]
	    set text [flattext [lindex [lindex [lindex $item 1] 1] 1] "none"]

	    # Strip out the number in <heading> for searching etc.
	    
	    if { [regexp {^( )*([0-9.])+ (.+)$} $text all null nulla new] } {
		set text $new
	    }

	    if {$elemname == "BT"} {
		set bt_term $text
	    }
	    if {$elemname == "HEADING"} {
		set heading_term $text
	    }


	    set appendtxt "<br>"

	    puts "<b>$start</b>: "
	    #Get content, generate CGI link to xtitle.

	    set subtext [cgisub $text]

	    set link "<a href = \"http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/unescosearch.cgi?fieldidx1=title&numstart=1&numreq=1&bool=AND&fieldcont1=$subtext\">$text</a>"

	    puts $link
	    set done 1;
	    
	    
	} elseif {$elemname == "SN"} {
	    puts "<br><b>SN</b>: "
	    set appendtxt "<br>"
	} elseif {$elemname == "FRENCH"} {
	    puts "<br><b>French</b>: "
	    set appendtxt "<br>"

	} elseif {$elemname == "SPANISH"} {
	    puts "<b>Spanish</b>: "
	    set appendtxt "<br>"
	}


	if {$done == 0 } {
	    processitem [lindex [lindex $item 1] 1]
	}
	
	# Things to be done on close of element
	if {$appendtxt != "" } {
	    puts $appendtxt
	}

    
    } elseif { $item == "" } {
	#We've been stripped out, ignore
    } else {
	#Debug unknown grs1 syntax
	puts stderr "\[metasearch\]: UnknownGRS:: '$item'"
    }
}


proc processtree  item  {
    global bt_term heading_term name_term;
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    # We're a string... Woop.
	} else { 
	    foreach nitem [lindex $item 1] { 
		processtree $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::" || [lindex $item 0] == "GenericRecord::"} { 
	
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1] 
	set elemname [string toupper $elemname]
	set done 0

	if {[lsearch {"NAME" "BT" "HEADING" "TERM"} $elemname] == -1} {
	    return
	}

	if {$elemname != "TERM" } {
	    set text [flattext [lindex [lindex [lindex $item 1] 1] 1] "none"]
	    if { [regexp {^( )*([0-9.])+ (.+)$} $text all null nulla new] } {
		set text $new
	    }

	    set done 1
	    append elemname "_term"
	    set varname [string tolower $elemname]
	    set $varname $text
	}
	
	if {$done == 0 } {
	    processtree [lindex [lindex $item 1] 1]
	}
	
    } else {
	#Debug unknown grs1 syntax
	puts "<pre>\[unesco\]: UnknownGRS:: '[lindex $item 0]'</pre>"
    }

}



#
#---- Main ---------------------------------------
#


set format $indata(format)

handle_search_$format
exit

set err [catch "handle_search_$format" fsres]
if {$err} {
    puts stderr "\[paleosearch error\]: $fsres"	
    throwfile "err.html" [list "\%ERR\% '$format' is not a known result format. Valid formats are: titles, scan, full, titleframes, scanframes " ]
}
