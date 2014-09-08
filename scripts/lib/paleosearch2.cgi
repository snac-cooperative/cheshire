#!/home/cheshire/cheshire/bin/webcheshire
# This can be used as a generic MARC display script

source "lib/libcgi-1.0.0.tcl"
source "lib/libsearch-1.0.0.tcl"

set CHESHIRE_DATABASE paleo
set CHESHIRE_CONFIGFILE /home/cheshire/cheshire/paleo/DBCONFIGFILE
set CHESHIRE_RECSYNTAX SGML

set CHESHIRE_ELEMENTSET_TITLES TPGRS
set CHESHIRE_RECSYNTAX_TITLES GRS1
set CHESHIRE_ELEMENTSET_FULL F
set CHESHIRE_RECSYNTAX_FULL SGML

set OUTPUT_TO_FILE 0
set OUTPUT_TO_STDOUT 1

set BASE_URL "http://gondolin.hist.liv.ac.uk/~cheshire/paleo/"
set CHESHIRE_ROOT "/home/cheshire/cheshire/paleo"
set CGI_PATH "/home/cheshire/public_html/cgi-bin"
set CGI_URL "http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/"

puts "Content-type: text/HTML\n"
cd $CHESHIRE_ROOT

set DTD "/home/cheshire/cheshire/paleo/USMARC12.DTD"
zmakeformat myhtml $DTD { \
{{<tr><td><b>Author:</b></td><td>} {fld100} {a} {<A HREF=${QUOTES}$SCRIPTNAME?fieldidx1=author&fieldcont1=%+} { } {$QUOTES>%s</A></td></tr>} TRUE FALSE FALSE FALSE FALSE TRUE 0} \
{{<tr><td><b>Corp. Author:</b></td><td>} {fld110} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Conference:</b></td><td>} {fld111} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Title:</b></td><td>} {fld245} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Publisher:</b></td><td>} {fld260} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Periodical:</b></td><td>} {fld773} {t} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 15} \
{{<tr><td><b>Date:</b></td><td>} {fld773} {d 9} {} {, } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 15} \
{{<tr><td><b>Volume:</b></td><td>} {fld773} {g v n} {} {} {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 15} \
{{<tr><td><b>Pages:</b></td><td>} {fld300} {a b} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Series:</b></td><td>} {fld4..} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Notes:</b></td><td><br></td></tr>} {fld5[0-9][0-9]} {} {<tr><td><br></td><td>} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE TRUE 0} \
{{<tr><td><b>Subjects:</b></td><td><br></td></tr>} {fld(96|[6][59])0} {} {<tr><td><br></td><td><A HREF=${QUOTES}$SCRIPTNAME?fieldidx1=subject&fieldcont1=%+} { -- } {${QUOTES}>%s</A></td></tr>  } TRUE FALSE FALSE FALSE FALSE TRUE 0} \
{{<tr><td><b>Other Authors:</b></td><td><br></td></tr>} {fld700} {a} {<tr><td><br></td><td><A HREF=${QUOTES}$SCRIPTNAME?fieldidx1=author&fieldcont1=%+} { } {$QUOTES>%s</A></td></tr>  } TRUE FALSE FALSE FALSE FALSE TRUE 0} \
{{<tr><td><b>Conference:</b></td><td>} {fld711} {} {} { } {</td></tr> } TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>LC Call Number:</b></td><td>} {fld050} {} {} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<tr><td><b>Dewey Call Number:</b></td><td>} {fld082} {} {} { } {</td></tr> } TRUE FALSE FALSE FALSE FALSE FALSE 0} \
{{<p><tr><td><b>Local Call Numbers:</b></td><td><br></td></tr>} {fld950} {l a b} {<tr><td><br></td><td>} { } {</td></tr>} TRUE FALSE FALSE FALSE FALSE FALSE 0} \
}



# ---------------

proc handle_format_full { record } {
    global SCRIPT_NAME DTD

    set QUOTES "\""     
    set fmt "myhtml"
    set num 1
    set SCRIPTNAME $SCRIPT_NAME

    set rectype "sgml"
    puts "<tr><td colspan = 2><HR></td></tr>"
    set outrec [zformat $fmt $record $rectype $num 2000 $DTD]
    regsub -all {\+\+} $outrec {+} outrec
    regsub -all {\=\+} $outrec {=} outrec
    puts [subst -nocommands -nobackslashes $outrec]
    
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





#------------------------------------------------------------ 
#   Main Function
#

set format $indata(format)

set err [catch "handle_search_$format" fsres]
if {$err} {
    puts stderr "\[paleosearch error\]: $fsres"	
    throwfile "err.html" [list "\%ERR\% '$format' is not a known result format. Valid formats are: titles, scan, full, titleframes, scanframes " ]
}

#
#------------------------------------------------------------



exit

# ------------- Old code to port ----------------

# Retrieve records
puts "<tr><td colspan = 2><HR></td></tr>"
puts "</table>"


puts "<p><center><h3>Subject Browsing</h3></center>"
puts "<p><hr><p>"

set CHESHIRE_ELEMENTSET DOCIDONLY
set CHESHIRE_RECSYNTAX GRS1
set CHESHIRE_NUMREQUESTED 10
set sresults [lrange $qresults 1 end]

		    

set docid [lindex [lindex [lindex [lindex $srchresults 1] 1] 1] 1]
regsub "fieldcont1=replace" $cgia "fieldcont1=$docid" cgiaa
puts "<tr><td><a href = \"$cgiaa\">$termtxt</a></td><td align = right>(1 match)</td></tr>"

set cgiterm [cgisub $term]
regsub "fieldcont1=replace" $cgib "fieldcont1=$cgiterm" cgibb
puts "<tr><td><a href = \"$cgibb\">$termtxt</a></td><td align = right> ($recs matches)</td></tr>"
set searchquery "search $index {\"$term\"}"
