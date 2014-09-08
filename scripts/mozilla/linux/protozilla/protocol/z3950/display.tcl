
source "eadgrs.tcl"

proc find_supported_format {} {
    global FIRST_RECORD NUM_RECORDS RECORD_FORMAT GRS_FORMATS SUMMARY_FORMATS GRS_TRANSFORMS
    # HTML, MARC, XML, GRS1, SUTRS


    # --- Inital set up   (This is messy...)
    
    if {$RECORD_FORMAT != "LONG2" && [string toupper $RECORD_FORMAT] != "SUMMARY"} {
	zset elementsetnames $RECORD_FORMAT
    } elseif {[string toupper $RECORD_FORMAT] == "SUMMARY"} {
	zset elementsetnames B
    } else {
	zset elementsetnames F
    }
    set realfr $FIRST_RECORD
    set realnr $NUM_RECORDS
    set FIRST_RECORD 1
    set NUM_RECORDS 1

    # --- HTML
    zset recsyntax HTML
    set zdisp [try_zdisplay]
    if {[lindex $zdisp 0] != 0} {
	# Check it, as most servers just return marc.
	if { [regexp "<html>" $zdisp] } {
	    # WOW it actually is!
	    set FIRST_RECORD $realfr
	    set NUM_RECORDS $realnr
	    return "HTML"
	}
    }
	
    # --- MARC
    zset recsyntax MARC
    set zdisp [try_zdisplay]
    if {[lindex $zdisp 0] != 0} {
	# Assume that it is.
	set FIRST_RECORD $realfr
	set NUM_RECORDS $realnr
	return "MARC"
    }

    # --- XML
    zset recsyntax XML
    set zdisp [try_zdisplay]
    set xml_disp $zdisp
    if {[lindex $zdisp 0] != 0} {
	# Look for Style Sheet
	if { [regexp "<\?xml-stylesheet " $zdisp] } {
	    # We have a winner :)
	    set FIRST_RECORD $realfr
	    set NUM_RECORDS $realnr
	    return "XML"
	} else {
	    # Try to find the top level tag and check against personal
	    # list of style sheets.

	    # XXX: Finish
	    set record [lindex $zdisp 1]
	    regexp {^(.+)<([^?!>]+)>} $record null morenull tag
	}
    }


    # --- GRS1
    zset recsyntax GRS1
    set zdisp [try_zdisplay]
    if {[lindex $zdisp 0] != 0} {

	set oldformat [lindex [zshow elementsetnames] 1]
	# Resolve 'Summary'  Will fall through if not supported here
	if {[string toupper $RECORD_FORMAT] == "SUMMARY"} {
	    foreach fmt $SUMMARY_FORMATS {
		zset elementsetnames $fmt
		set testdisp [try_zdisplay]
		if {[lindex $testdisp 0] != 0} {
		    set RECORD_FORMAT $fmt
		    set FIRST_RECORD $realfr
		    set NUM_RECORDS $realnr
		    return "GRS1"
		}
	    }
	}

	if {[lsearch $GRS_FORMATS $RECORD_FORMAT] != -1} {
	    zset elementsetnames $RECORD_FORMAT
	    set testdisp [try_zdisplay]
	    if {[lindex $testdisp 0] != 0} {
		set FIRST_RECORD $realfr
		set NUM_RECORDS $realnr
		return "GRS1"
	    }
	}

	# Get top level tag, check against list.
	set tlt [lindex [lindex [lindex [lindex [lindex $zdisp 1] 1] 0] 1] 1]

	if { [lsearch $GRS_TRANSFORMS $tlt] != -1 } {
	    set FIRST_RECORD $realfr
	    set NUM_RECORDS $realnr
	    set RECORD_FORMAT [string tolower $tlt]
	    return "GRS1"
	}
	
	zset elementsetnames $oldformat
    }

    # --- SUTRS
    zset recsyntax SUTRS
    set ishtml 0
    if {[string first "html" [string tolower $RECORD_FORMAT] ] != -1} {
	set ishtml 1
	zset elementsetnames  $RECORD_FORMAT
    }
    set zdisp [try_zdisplay]

    if {$xml_disp == $zdisp} {
	# Cheshire server. Fall through to GRS, which does better parsing. Probably.
    } elseif {[lindex $zdisp 0] != 0} {
	
	if {$ishtml == 1 && ![regexp -nocase {<(html|tr|td|b|i|p|center|hr|br)>} $zdisp] } {
	    zset elementsetnames F
	}	

	# If it returns okay, assume it is.
	set FIRST_RECORD $realfr
	set NUM_RECORDS $realnr
	return "SUTRS"
    }

    # Try last ditch GRS1 processor
    zset recsyntax GRS1
    set zdisp [try_zdisplay]
    if {[lindex $zdisp 0] != 0} {
	    set FIRST_RECORD $realfr
	    set NUM_RECORDS $realnr
	    return "GRS1"
    }

    # --- No Match :(
    set FIRST_RECORD $realfr
    set NUM_RECORDS $realnr
    return "NONE"
}


proc try_display_records {} {
    global FIRST_RECORD RECORD_FORMAT RECV_RECORDS HITS RECORD_SYNTAX URI_HOST URI_PORT DATABASENAME
    # global flipflop

    # Make sure we're in the right connection.

    try_zconnect
    set syntax [find_supported_format]
    if {$syntax == "NONE"} {
	if {$HITS != ""} {
	    exit_error "$HITS records were retrieved but could not be displayed"
	} else {
	    exit_error "The server could not process your request."
	}
	return
    }
    
    zset recsyntax $syntax
    set RECORD_SYNTAX $syntax
    set cmd [string tolower "display_multiple display_single_${syntax}_$RECORD_FORMAT" ]
    set err [catch {eval $cmd} displayresults]

    if {$err != 0} {
	# Fail. Try default recsyntax
	puts stderr "Failed: $displayresults"
	display_multiple [string tolower "display_single_$syntax"]
    }   else {
	return $displayresults
    }

}

proc display_multiple {singlecmd} {
    global HITS FIRST_RECORD RECV_RECORDS RECORD_SYNTAX RECORD_FORMAT
    # global flipflop

    set cmd [string tolower "init_display_${RECORD_SYNTAX}_$RECORD_FORMAT"]
    set err [catch {eval $cmd} display_html]
    if {$err != 0} {
	set display_html [[string tolower "init_display_${RECORD_SYNTAX}"]]
    }
    

    set zdisp [try_zdisplay]
    if {[lindex zdisp 0] == 0} {
	exit_error "Could not display records"
	return 0
    } else {
	set RECV_RECORDS [lindex [lindex [lindex $zdisp 0 ] 2] 1]
	set ridx $FIRST_RECORD

	foreach record [lrange $zdisp 1 end]  { 
#	    vwait flipflop
#	    if {$flipflop < 0} {
#		return -1
#	    }
	    append display_html [$singlecmd $record $ridx]
	    incr ridx
	}
    }
    append display_html "</table><hr><p>"
    return $display_html

}


proc init_display_html {} {
    return ""
}

proc display_single_html {record index} {
    return $record
}


proc init_display_marc {} {
    return "<table width = 100%>"
}

proc init_display_marc_tpgrs {} {
    return "<table width = 100% cellpadding =2 cellborder =0>"
}

proc init_display_grs1_ead {} {
    return "<hr>"
}

proc display_single_grs1_ead {sresults index} {
    global eadhtmlrec RECORD_FORMAT

    set RECORD_FORMAT "F"
    set eadhtmlrec ""

    set sresults [ lindex $sresults 1]
    regsub -all "{tagType:: 3}" $sresults "" sresults
    regsub -all "{tagType:: 2}" $sresults "" sresults
    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
    regsub -all "\"" $sresults {\&quot;} sresults

    foreach item [lrange $sresults 0 end] {
	processitem $item
    }

    return $eadhtmlrec

}

proc display_single_marc {record index} {
    return [display_single_marc_long2 $record $index]
}

proc display_single_marc_long  {record index} {

    zset elementsetnames F
    set fmtcmd "zformat LONG \{$record\} marc $index 2000"

    set html  "<tr><td colspan = 2><HR></td></tr>"
    set err [catch {eval $fmtcmd} outrec]
	    
    if {$err == 0 } {
	set lines [split $outrec "\n"]
	foreach l $lines {
	    if {[string range $l 0 6] == "Record"} {
		append html "<tr><td colspan = 2><b>$l</b></td></tr>"
	    } else {
		append html "<tr><td valign = top width = 20%><i>[string range $l 0 14]</i></td><td>[string range $l 15 end]</td></tr>"
	    }
	}	    
	return $html
    } else {
	return "<tr><td colspan = 2>Record number $index appears to be invalid.</td></tr>"
    }
}


proc display_single_marc_long2  {record index} {

    zset elementsetnames F
    set fmtcmd "zformat LONG2 \{$record\} marc $index 2000"

    set html  "<tr><td colspan = 2><HR></td></tr>"
    set err [catch {eval $fmtcmd} outrec]
	    
    if {$err == 0 } {
	while { [set idx [string first "<br>" $outrec]] != -1 } {
	    append html "<tr><td valign = top width = 20%><i>[string range $outrec 0 19]</i></td><td>[string range $outrec 20 [expr $idx -1]]</td></tr>"
	    set outrec [string range $outrec [expr $idx +4] end]
	}
	return $html
    } else {
	return "<tr><td colspan = 2>Record number $index appears to be invalid.</td></tr>"
    }
}

proc init_display_marc_summary {} {
    return "<table width = 100% bgcolor = \"\#ccccff\"><tr bgcolor=\"\#eeeeff\"><td><b>Full</b></td><td><b>Title</b></td><td><b>Author</b></td></tr>"
}

proc display_single_marc_summary {record index} {

    set fmtcmd "zformat SHORT \{$record\} marc $index 2000"
    set err [catch {eval $fmtcmd} outrec]

    if { [expr $index % 2] == 0} {
	set bgcol "\#e7e7e7"
    } else {
	set bgcol "white"
    }

    if {!$err} {
	# Get author/title out

	set fhref [build_cgihref $index]
	append fhref "&format=LONG2"


	set author "&nbsp;"
	set title "&nbsp;"
	regexp {Author:([^\n]+)\n} $outrec null author
	regexp {Title:([^\n]+)\n} $outrec null title
	return "<tr bgcolor=\"$bgcol\"><td><a href=\"$fhref\">$index</a></td><td>$title</td><td valign = top>$author</td></tr>"
    } else {
	return "<tr bgcolor=\"$bgcol\"><td colspan = 2>Record $index does not appear to be valid</td></tr>"
    }
}

proc init_display_sutrs {} {
    return "<table width = 100%>"
}

proc display_single_sutrs {record index} {
    global RECORD_FORMAT

    # Lame attempt to check for HTML over SUTRS stream :(
    if {[string first "html" [string tolower $RECORD_FORMAT] ] != -1} {
	if { [regexp -nocase {<(html|tr|td|b|i|p|center|hr|br)>} $record] } {
	    # It probably is... as we've already checked for this
	    return "<tr><td colspan=\"2\"><hr></td><tr><td colspan =2>$record</td></tr>"
	}
    }
	
    regsub -all "\&" $record {\&amp;} record
    regsub -all "<" $record {\&lt;} record
    regsub -all ">" $record {\&gt;} record

    regsub -all "\n\n" $record {<p>} record
    regsub -all "\n" $record {<br>} record

    return "<tr><td colspan = 2><hr></td><tr><tr><td colspan = 2><pre>$record</pre></td></tr>"

}

proc init_display_grs1 {} {
    return "<table width = 100%>"
}

proc init_display_grs1_titleauthor {} {
    return "<table width = 100% bgcolor = \"\#ccccff\"><tr bgcolor=\"\#e5e5ff\"><td><b>Full</b></td><td><b>Title</b></td><td><b>Author</b></td></tr>"
}

proc init_display_grs1_titledate {} {
    return "<table width = 100% bgcolor = \"\#ccccff\"><tr bgcolor=\"\#e5e5ff\"><td><b>Full</b></td><td><b>Title</b></td><td><b>Date</b></td></tr>"
}

proc init_display_grs1_tpgrs {} {
    return "<table width = 100% bgcolor = \"\#ccccff\"><tr bgcolor=\"\#e5e5ff\"><td><b>Full</b></td><td width = 98%><b>Title</b></td><td><b>Relevance</b></td></tr>"
}

proc init_display_grs1_titleauthordate {} {
    return "<table width = 100% bgcolor = \"\#ccccff\"><tr bgcolor=\"\#e5e5ff\"><td><b>Full</b></td><td><b>Title</b></td><td><b>Author</b></td><td><b>Date</b></td></tr>"
}

proc textsub {content} {
    regsub -all "\&apos;" $content {'} content
    regsub -all "\&lsqb;" $content {[} content
    regsub -all "\&rsqb;" $content {]} content
    regsub -all "\&commat;" $content {,} content
    regsub -all "\&lsquo;" $content {'} content
    regsub -all "\&rsquo;" $content {'} content
    regsub -all "\&sol;" $content {/} content
    regsub -all "\&rdquo;" $content {"} content
    regsub -all "\&ldquo;" $content {"} content
    regsub -all "\&mdash;" $content {-} content
    regsub -all "\&ndash;" $content {-} content

    return $content;
}

proc slindex {item idx} {

    set cmd "lindex {$item} $idx"
    set err [catch {eval $cmd} foo]
    if {$err != 0} {
	return "!BROKEN!"
    } else {
	return $foo
    }
}


proc grs_process_contents {item} {
    global grs_contents_text

    set type [lindex $item 0]

    if {$type == "SUBELEMENT::"} {
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 1] 1] 
	set elemname [string tolower $elemname]
	
	if { [llength [lindex $item 1]] == 3} {
	    # Probably a Cheshire Attribute
	    append grs_contents_text "($elemname: "
	    set apptxt ") "
	} else {
 	    if {$elemname != "19" } {
	 	append grs_contents_text "<table><tr><td valign = top><b>$elemname:</b></td><td>"
		set apptxt "</td></tr></table>"
	    } else {
		set apptxt ""
	    }
	}

	grs_process_contents [slindex [slindex $item 1] 1]
	append grs_contents_text $apptxt

    } else {
	if {[llength [slindex [slindex $item 1] 0]] == 1} {
	    set string [slindex $item 1]
	    if { $string != "elementEmpty" } {
		set string [ textsub $string ]
		# XXX This is not very clever.
		while { [regexp  -nocase -linestop -lineanchor {(^| )((http|z3950|mailto|ftp|https)://[^ ,!;)("]+)(\. |[ ,!;?])*} $string null null href ] } {
# "
		    regsub -all $href $string "<a href = \"$href\">$href</a>" string 
		}
		if {[regexp "\n" $string]} {
		    set string "<pre>$string</pre>"
		}
		append grs_contents_text "$string"
	    }
	} elseif { [slindex [slindex $item 1] 0] == "!BROKEN!" } {
	    if {[regexp "\n" $item]} {
		set item "<pre>$item</pre>"
	    }
	    append grs_contents_text $item
	} else { 
	    foreach nitem [slindex $item 1] { 
		grs_process_contents $nitem 
	    } 
	}
    } 
}


proc display_single_grs1 {record index} {
    global grs_contents_text
    
    set result "<b>Record: $index</b><table>"
    set grs_contents_text ""
    set type [lindex [lindex [lindex [lindex $record  1] 0] 0] 1]

    foreach tag [lrange $record 1 end] {
	if {$type == "2"} {
	    set title [lindex [lindex [lindex $tag 0] 1] 2]
	    set contents [lindex [lindex $tag 1] 1]
	} else {
	    set title [lindex [lindex [lindex $tag 0] 1] 1]
	    foreach bit [lrange $tag 1 end] {
		grs_process_contents $bit
	    }
	    set contents $grs_contents_text
	}

	append result "<tr><td valign = top><b>$title:</b></td><td>$contents</td></tr>"
    }
    append result "</table>"
    return $result

}
    

proc display_single_grs1_tpgrs {record index} {
    set record [lrange $record 1 end] 

    set relevance [lindex [lindex [lindex $record 0] 1] 1]
    set docid [lindex [lindex [lindex $record 1] 1] 1]
    regsub {^( )*} $docid "" docid
    set title [lindex [lindex [lindex $record 2] 1] 1]
    regsub -all "\n" $title " " title

    if { [expr $index % 2] == 0} {
	set bgcol "\#e7e7e7"
    } else {
	set bgcol "white"
    }

    set fhref [build_cgihref $index]
    append fhref "&format=F"

    return "<tr bgcolor=\"$bgcol\"><td><a href=\"$fhref\">$index</a></td><td>$title</td><td>$relevance</td></tr>"

}

proc display_single_grs1_titleauthordate {record index} {
    set record [lrange $record 1 end] 

    set title ""
    set author ""
    set datetime ""
    
    foreach tag $record {
	set type [lindex [lindex [lindex $tag 0] 1] 2]
	set type [string tolower $type]
	set content [lindex [lindex $tag 1] 1]
	set $type $content
    }

    regsub -all " " $author "\\&nbsp;" author

    if { [expr $index % 2] == 0} {
	set bgcol "\#e7e7e7"
    } else {
	set bgcol "white"
    }

    set fhref [build_cgihref $index]
    append fhref "&format=F"

    return "<tr bgcolor=\"$bgcol\"><td><a href=\"$fhref\">$index</a></td><td>$title</td><td>$author</td><td>$datetime</td></tr>"

}

proc display_single_grs1_titleauthor {record index} {
    set record [lrange $record 1 end] 

    set title ""
    set author ""
    
    foreach tag $record {
	set type [lindex [lindex [lindex $tag 0] 1] 2]
	set type [string tolower $type]
	set content [lindex [lindex $tag 1] 1]
	set $type $content
    }

    regsub -all " " $author "\\&nbsp;" author

    if { [expr $index % 2] == 0} {
	set bgcol "\#e7e7e7"
    } else {
	set bgcol "white"
    }

    set fhref [build_cgihref $index]
    append fhref "&format=F"

    return "<tr bgcolor=\"$bgcol\"><td><a href=\"$fhref\">$index</a></td><td>$title</td><td>$author</td></tr>"

}



set GRS_TRANSFORMS [list "EAD"]

set SUMMARY_FORMATS [list "TITLEAUTHORDATE" "TITLEAUTHOR" "TITLEDATE" "TPGRS" "TITLE"]

set GRS_FORMATS [list "TPGRS" "DC" "TITLEAUTHORDATE" "TITLEAUTHOR" "TITLEDATE" "TITLE"]


zmakeformat LONG2 { \
			{{Record&nbsp;#} {#} {} {} { } {               <br>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
			{{Title:} {245} {} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Author:} {1??} {} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Publisher:} {260} {ab} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Date:} {260} {c} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Periodical:} {773} {d9} {} {} {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Volume:} {773} {gvn} {} {} {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Pages:} {300} {ab} {} { -- } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Subjects:} {6[59]0} {} {} { -- } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Notes:} {5[0-9][0-9]} {} {} {} {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Other&nbsp;Auth:} {700} {a} {} { -- } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{ISBN:} {020} {} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{LC&nbsp;Call:} {050} {} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Dewey&nbsp;Call:} {082} {} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
			{{Local&nbsp;Call:} {950} {lab} {} { } {<br>} TRUE FALSE FALSE FALSE FALSE FALSE 20} \
		}

