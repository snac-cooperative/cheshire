#!/home/cheshire/cheshire/bin/webcheshire
#
# Author:  Rob Sanderson (azaroth@liv.ac.uk)
# Copyright:  &copy; (c) The University of Liverpool, All Rights Reserved
# Licence:  This script may be used by any non profit organisation or academic institution.
# Last Modified: 13/02/00
# Description:  Sgml email search, scan and display script via Cheshire 2.
# Version:  1.0.0
#
# TODO:  
#                   * Fix Threads! (Req Server fix)
#                   * Fix email address of people (Req Server fix) 
#                   * Sorting (Req Server fix) (New Major Version)

# --- Configuration Variables ---

set CHESHIRE_DATABASE mailarchive
set CHESHIRE_CONFIGFILE "/home/cheshire/cheshire/mailarchive/DBCONFIGFILE"
set HOMEURL "/~cheshire/cheshire-l/"
set HEADTITLE "Cheshire-L Email Search"
set BODYTITLE "Cheshire-L Search Results"

# --- End of Configuration Variables ---

set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED  SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR  REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH  HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}

set SCRIPTNAME $env(SCRIPT_NAME)
regsub -all {\%3A} $SCRIPTNAME {:} SCRIPTNAME
regsub -all {\%7E} $SCRIPTNAME {~} SCRIPTNAME

puts "Content-type:  text/html\n\n";

# --- Subroutines ---

proc generate_navbar {} {
    global returned startrec numreq hits firstrec indata HOMEURL

    set prevstart [expr $startrec - $numreq]
    set nextstart [expr $startrec + $returned]
    set text "<center>"

    if {$prevstart > 0} {
	# Generate previous button
	set indata("firstrec") $prevstart
	set href [generate_cgihref 0]
	append text "\[ <a href = \"$href\">Previous $numreq Messages</a> \] | "
    }
    
    append text "\[ <a href = \"$HOMEURL\">Home</a> \]"
    
    if {[expr $nextstart +1]  < $hits } {
	# Generate next button
	set indata("firstrec") $nextstart
	set href [generate_cgihref 0]
	append text " | \[ <a href = \"$href\">Next $numreq Messages</a> \]"
    }
    append text "</center>"
    return $text
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

proc generate_query {i} {
    global firstterm indata query;
    set idi $indata("fieldidx$i");
    set idc $indata("fieldcont$i");

    regsub -all ";" $idc "" idc;

    if {$idi != "none" && $idc != ""} {
	if {$firstterm == 0} {
	    set bool $indata("boolean");
	    append query " $bool "
	}
	if {$idi == "topic" || $idi == "any"} {
	    append query " ($idi @ \{$idc\} )"
	} elseif { $idi == "date" } {
	    set now [clock seconds]
	    set day [ expr 24 * 60 * 60 ];
	    set week [ expr 7 * $day ]
	    set month [ expr 31 * $day ]
	    if {$idc == "day"}  {
		set stime [clock format  [expr $now - $day]]
	    } elseif {$idc == "week"} {
		set stime [clock format [expr $now - $week]]
	    } elseif {$idc == "month"} {
		set stime [clock format [expr $now - $month]]
	    } elseif {$idc == "hour"} {
  	 	set stime [clock format [expr $now - 3600]]
	    }
	    append query " (date > \{$stime\} )"
	} else {
	    append query " ($idi \{$idc\} )"
	}
	set firstterm 0
    }
}

proc generate_cgihref numslashes {
    global indata SCRIPTNAME;
    set cgiquery $SCRIPTNAME
    set firstterm 1;
    set slashes ""
    set inlist [array names indata]

    for {set n 1} {$n <= $numslashes} {incr n} {
	append slashes "\\"
    }
    foreach key $inlist {
	if {!$firstterm} {
	    append cgiquery "$slashes&"
	} else {
	    append cgiquery "?"
	    set firstterm 0
	}
	set val $indata($key);
	set val [cgisub $val]
	append cgiquery "$key=$val"
    } 
    regsub -all "\"" $cgiquery "" cgiquery
    return $cgiquery
} 

proc cgisub {text} {

	regsub -all { } $text {+} text
	regsub -all "\n\t" $text {\%0A} text
	regsub -all {,} $text {\%2C} text
	regsub -all {'} $text {\%27} text
	regsub -all "\"" $text {\%22} text
	regsub -all {@} $text {\%40} text
	regsub -all {#} $text {\%23} text
	regsub -all {\}} $text {\%7D} text
	regsub -all {\{} $text  {\%7B} text
	regsub -all { } $text  {\%20} text
	return $text;
}

proc exit_error {message} {
    global HOMEURL

    puts "<html>\n<head>\n<title>Cheshire Quickstep: Error</title>\n</head><body bgcolor = white>\n<p><center><b>Cheshire Quickstep: Error</b></center>"
    puts "<p>A problem was encountered processing your request:<p><blockquote>$message</blockquote>"
    puts "<p>Please try another search at the <a href = \"$HOMEURL\">search page</a><p><hr></body></html>"
    exit
}	    


# --- Main Routine ---

# Process CGI Request

if {[string compare $env(REQUEST_METHOD) "POST"]==0} {
    set message [split [read stdin $env(CONTENT_LENGTH)] &]
} else {
    set message [split $env(QUERY_STRING) &]
}

set maxfield 0;

foreach pair $message {
    set name [lindex [split $pair =] 0]
    set val [lindex [split $pair =] 1]

    regsub -all {\+} $val { } val
    regsub -all {\%0A} $val \n\t val
    regsub -all {\%2C} $val {,} val
    regsub -all {\%27} $val {'} val
    regsub -all {\%22} $val "\"" val
    regsub -all {\%40} $val {@} val
    regsub -all {\%7D} $val {\}} val
    regsub -all {\%7B} $val {\{} val
    regsub -all {\%20} $val { } val
    regsub -all {\%3A} $val {:} val
    regsub -all {\%7E} $val {~} val

    if {[regexp "^ +$" $val]} {
	set val "";
    }
    
    set indata("$name") "$val"
    if { [ string range $name 0 7 ] == "fieldidx" && [set newidx [string range $name 8 end]] > $maxfield } {
	set maxfield $newidx;
    }
}

# Process indata

if { [lsearch [array names indata] "\"format\""] != -1} {
    set format $indata("format")
} else {
    set format "short"
}

if {$format == "long"} {
    set CHESHIRE_RECSYNTAX SGML
    set CHESHIRE_ELEMENTSET F
} else {
    set CHESHIRE_RECSYNTAX GRS1
    set CHESHIRE_ELEMENTSET TPGRS
}
    
if { [lsearch [array names indata] "\"firstrec\""] != -1} {
    set CHESHIRE_NUM_START $indata("firstrec")
} else {
    set CHESHIRE_NUM_START 1
}
if { [lsearch [array names indata] "\"numreq\""] != -1} {
    set CHESHIRE_NUMREQUESTED $indata("numreq")
} else {
    set CHESHIRE_NUMREQUESTED 20
}
set numreq $CHESHIRE_NUMREQUESTED
set firstrec $CHESHIRE_NUM_START


#Sanity check numbers
set errf [catch {expr $firstrec}]
set errm [catch {expr $numreq}]
if { $errf != 0 || $firstrec < 1} {
    exit_error "The start record, '$firstrec', is not an appropriate value. It should be a number greater than 0"
}
if { $errm != 0 || $numreq < 1} {
    exit_error "The number of records requested, '$numreq', is not an appropriate value. It should be a number greater than 0"
}


# Generate Query

if {$format == "scan"} {
    #We want to scan the index listed in fieldidx1 at term fieldcont1
    set scanindex $indata("fieldidx1")
    set scanterm $indata("fieldcont1")
    if {$scanterm == ""} {
	set scanterm "0"
    }
    set scanterm [string tolower $scanterm]
    set query "lscan $scanindex {$scanterm} 0 $numreq [expr $numreq / 2 + 1]"
} else {
    
    if {$format == "threads"} {
	set query "search subject {cheshire-l} NOT subject re "
	set firstterm 0
    } else {	
	set firstterm 1
	set query "search "
    }

    for {set n 1} {$n <= $maxfield} {incr n} {
	generate_query $n;
    }
    if { [lsearch [array names indata] "\"datestart\""] != -1 && ($indata("datestart") != "" || $indata("dateend") != "")} {
	set datestart $indata("datestart");
	set dateend $indata("dateend");
	if {$datestart == "" } {
	    set datestart "Mon Jan 01 00:00:00 2001"
	}
	if {$dateend == ""} {
	    set dateend "Tue Jan 01 00:00:00 2002"
	}
	if {$query != "search "} {
	    append query " "
	    append query $indata("boolean")
	}
	append query " date >= $datestart AND date <= $dateend"
    }
}


# --- Processing ---

set err [catch {eval $query} qresults]
set hits [lindex [lindex [lindex $qresults 0] 0] 1]
set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]
if {$returned == 1} { set records "record" } else { set records "records"}

# --- Display ---

if { $err == 0 && ($hits > 0 || $format == "scan")} {
    set sresults [lrange $qresults 1 end]
    set tagnames { from fromaddress fromdate sender date to subject returnpath}

    puts "<html>\n<head>\n<title>Cheshire Quickstep: $HEADTITLE</title>\n"
    puts "</head><body bgcolor = white>\n<center><b>Cheshire Quickstep: $BODYTITLE</b></center>\n<p>\n";


    if {$format == "long"} {
	# --- Handle entire message ---
	puts "[generate_navbar]<p>"
	foreach message $sresults {
	    foreach tagname $tagnames {
		set $tagname "";
	    }
	    set lines [split $message "\n"]
	    set text ""
	    set recording 0
	    foreach line $lines {
		set match ""
		set tag ""
		set value ""
		if { [regexp {^[ ]*<([^/>]+)>(.*)</(.+)>[ ]*$} $line match tag value] } {
#		    regsub -all {"} $value {\"} value; 
		    set $tag $value;
		} elseif { [regexp "<body>" $line match] } {
		    set recording 1
		} elseif { [regexp "</body>" $line match] } {
		    set recording 0
		} elseif {$recording == 1} {
		    append text "$line\n"
		}
	    }
	    puts "<hr>\n<table>"
	    puts "<tr><td><b>Date</b>:</td><td>$date</td></tr>"
	    puts "<tr><td><b>From</b>:</td><td>$from</td></tr>"
	    puts "<tr><td><b>Subject</b>:</td><td>$subject</td></tr>"
	    puts "<tr><td colspan = 2><br><pre>$text</pre></td></tr>"
	    puts "</table>"
	}

    } elseif {$format == "short" || $format == "threads"} {
	# --- Handle summary format ---

	puts "[generate_navbar]<p>"
	puts "<hr><p>"
	puts "<table border = 1 width = 100%>"
	puts "<tr><td bgcolor = \"lightblue\"><b>Subject</b></td><td bgcolor = lightgrey><b>Author</b></td><td><b>Date</b></td></tr>"
	set idx 1
	foreach message $sresults {
	    foreach tag [lrange $message 1 end] {
		set type [lindex [lindex [lindex $tag 0] 1] 2]
		set content [lindex [lindex $tag 1] 1]
		set $type $content
	    }
	    puts "<tr><td><a href = \"$SCRIPTNAME?format=long&fieldidx1=DOCID&fieldcont1=$documentId\">$title</a></td><td>$author</td><td>$dateTime</td></tr>"
	    incr idx
	}
	puts "</table>"

    } elseif {$format == "scan"} {
	# --- Handle Scan ---

	set termidx 0
	set firstterm ""
	set prevterm ""
	set lastterm ""
	set termtxt ""
	set foundterm 0
	set typetitle "[string toupper [string index $scanindex 1]][string range $scanindex 2 end]"
	puts "<table border = 1 width = 100%>"
	puts "<tr><td bgcolor = lightblue><b>$typetitle</b></td><td bgcolor = lightgrey><b>Messages</b></td></tr>"
	foreach result $sresults {
	    incr termidx
	    set term [lrange $result 0 [expr [llength $result] - 2]]
	    set recs [lindex $result [expr [llength $result] - 1]]
	    if {$term == ">>>Global Data Values<<<" } {
		#skip this 
		set firstterm ""
	    } else {
		if {$termidx == 1} {
		    set firstterm $term
		} elseif { $termidx == $numreq } {
		    set lastterm $term
		}
		set prevterm $termtxt
		set termtxt $term
		if { [string tolower $term ] == [ string tolower $scanterm] } {
		    set termtxt "<b>$termtxt</b>"
		    set foundterm 1
		}
		if {$scanterm != "0" && $foundterm == 0 && $term > $scanterm && $prevterm < $scanterm } {
		    puts "<tr><td><center>Your term '$scanterm' would be here.</center></td><td>&nbsp;</td></tr>";
		    set foundterm 1
		}
		if {$recs == 1} {
		    set searchquery "search $scanindex {\"$term\"}"
		    set err [ catch { eval $searchquery } srchresults ]
		    set hits [lindex [lindex [ lindex $srchresults 0] 0] 1]

		    set record [lindex $srchresults 1]
		    if {$err != 0 || $hits == 0 } {
			puts "<i>BROKEN INDEX: '$term' - hits: $hits</i><br>"
		    } else {
			set docid [lindex [lindex [lindex [lindex $srchresults 1] 1] 1] 1]
			set href "$SCRIPTNAME?format=long&fieldidx1=DOCID&fieldcont1=$docid"
			puts "<tr><td><a href = \"$href\">$termtxt</a></td><td align = right>(1 message)</td></tr>"
		    }
		} else {
		    set cgiterm [cgisub $term]
		    set href "$SCRIPTNAME?format=short&fieldidx1=$scanindex&fieldcont1=$termtxt"
		    puts "<tr><td><a href = \"$href\" target =\"_top\">$termtxt</a></td><td align = right> ($recs messages)</td></tr>"
		}
	    }
	}
	puts "</table>"
	
	set navstr ""
	if {$firstterm != ""} {
	    set nterm [cgisub $firstterm]
	    set href "$SCRIPTNAME?format=scan&fieldidx1=$scanindex&fieldcont1=$nterm"
	    append navstr "\[ <a href = \"$href\">previous</a> \] |"
	}
	append navstr " \[ <a href = \"$HOMEURL\">home</a> \] "
	if {$lastterm != ""} {
	    set nterm [cgisub $lastterm]
	    set href "$SCRIPTNAME?format=scan&fieldidx1=$scanindex&fieldcont1=$nterm"
	    append navstr "| \[ <a href = \"$href\">next</a> \]"
	}
	puts "<center>$navstr</center>"
    }

    puts "<p><hr>"
    puts "<br><center><font size = -1>Powered by <a href = \"http://gondolin.hist.liv.ac.uk/~cheshire/quickstep/\">Cheshire Quickstep</a> --  &copy; All messages are  copyright by their respective authors</font></center>"
    puts "</body>";
    puts "</html>";

    exit
}


exit_error "There were no matches for your search, which was encoded as:<br><code>$query</code>"














