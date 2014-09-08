#!/elib/sys/cheshire/bin/webcheshire
# 
# 
#

set CHESHIRE_CONFIGFILE "/elib/sys/cheshire/DATA/CONFIG.DL"
set CHESHIRE_DATABASE bibfile
set DOCDIR "/elib/data/docs"


global maxwt1 maxwt2 minwt1 minwt2
set maxwt1 0.0
set minwt1 0.999
set maxwt2 0.0
set minwt2 0.999

proc get_page_count {path} {
    return [llength [glob $path/*.txt]]
}

proc getminmax {rec} {

    global maxwt1 maxwt2 minwt1 minwt2

    set bar1 [lindex $rec 2]
    set bar2 [lindex $rec 3]

    foreach pair $bar1 {
	set weight [lindex $pair 1]
	if {$weight > $maxwt1} { set maxwt1 $weight}
	if {$weight < $minwt1} { set minwt1 $weight}
    }

    foreach pair $bar2 {
	set weight [lindex $pair 1]
	if {$weight > $maxwt2} { set maxwt2 $weight}
	if {$weight < $minwt2} { set minwt2 $weight}
    }
}

proc process_bar {bar1 bar2} {

    global maxwt1 maxwt2 minwt1 minwt2
    set sbar1 [lsort $bar1]
    set sbar2 [lsort $bar2]

    set mid1 [expr ($minwt1+$maxwt1)/2.0]
    set high1 [expr ($maxwt1+$mid1)/2.0]
    set low1 [expr ($minwt1+$mid1)/2.0]

    set mid2 [expr ($minwt2+$maxwt2)/2.0]
    set high2 [expr ($maxwt2+$mid2)/2.0]
    set low2 [expr ($minwt2+$mid2)/2.0]

    set bars ""


    foreach pair $sbar1 {
	set pagestr [lindex $pair 0]
	regexp "^(0*)(\[1234567890\]*)" $pagestr all zeros pageid
	if {$all == 0} { set pageid 0}
	set weight [lindex $pair 1]
	if {$weight >= $high1} {
	    set wtval 4
        } elseif {$weight < $high1 && $weight >= $mid1} {
	    set wtval 3
        } elseif {$weight < $mid1 && $weight >= $low1} {
	    set wtval 2
        } else {
	    set wtval 1
	}
	append bars [format "%d/%d," $pageid $wtval]
	
    }

    set finalbar [string trimright $bars ","]
    append finalbar ";"

    foreach pair $sbar2 {
	set pagestr [lindex $pair 0]
	regexp "^(0*)(\[1234567890\]*)" $pagestr all zeros pageid
	if {$all == 0} { set pageid 0}
	set weight [lindex $pair 1]
	if {$weight >= $high2} {
	    set wtval 4
        } elseif {$weight < $high2 && $weight >= $mid2} {
	    set wtval 3
        } elseif {$weight < $mid2 && $weight >= $low2} {
	    set wtval 2
        } else {
	    set wtval 1
	}
	append finalbar [format "%d/%d," $pageid $wtval]
    }

    set finalbar [string trimright $finalbar ","]
    append finalbar ";"

}

set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL \
		 SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED \
		 SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR \
		 REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH \
		 HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}


puts "Content-type: text/HTML\n"
puts "<HTML>"
puts "<HEAD>"
puts "<TITLE> Cheshire II TileBar Search Results </TITLE>"
puts "</HEAD>"

puts "<BODY VLINK =\"990000\" \
    BACKGROUND=\"/cheshire/paper3.gif\">" 
puts "<H2><IMG SRC=\"/cheshire/smallcat.gif\" \
    ALIGN=middle> Cheshire II Single Document TileBar Search Results </H2>"
puts "<hr>"

if {[string compare $env(REQUEST_METHOD) "POST"]==0} {
    set message [split [read stdin $env(CONTENT_LENGTH)] &]
} else {
    set message [split $env(QUERY_STRING) &]
}

foreach pair $message {
    set name [lindex [split $pair =] 0]
    set val [lindex [split $pair =] 1]

    regsub -all {\+} $val { } val
    # kludge to unescape some chars
    regsub -all {\%0A} $val \n\t val
    regsub -all {\%2C} $val {,} val
    regsub -all {\%27} $val {'} val
    
    set indata($name) "$val"
}

set numindexes 0

set rec1 1
set indata(firstrec) $rec1

# will only retrieve a single doc...
set maxnum 1
set indata(max) $maxnum


set fmt "html"
set format "format"
set indata($format) $fmt

set servername "UCBDL"
set searchmethod "tbsearch"

# the other two required variables for searching
# CHESHIRE_CONFIGFILE "/elib/sys/cheshire/DATA/CONFIG.DL"
# CHESHIRE_DATABASE bibfile
set CHESHIRE_NUMREQUESTED $maxnum
set CHESHIRE_NUM_START $rec1

# Determine the number of indexes
set CURRENT_TERMS ""

if {$indata(terms1) != ""} {
    incr numindexes
    set termlist1 $indata(terms1)
}
if {$indata(terms2) != ""} {
    incr numindexes
    set termlist2 $indata(terms2)
}

set elib_id $indata(elib_id)


if {$elib_id == ""} {
    puts "Your search was <B>not submitted</B><HR>"
    puts "<P><B>Diagnosis</B>"
    puts "<BLOCKQUOTE>The requested search did not "
    puts "specify an ELIB ID number. An ELIB ID number is required for"
    puts "single document searching."
    puts "Return to the search screen, "
    puts "enter an ELIB ID number, and submit another search."
    puts "</BLOCKQUOTE></BODY></HTML>"
    exit
}

if {$numindexes == 0} {
    puts "Your search was <B>not submitted</B><HR>"
    puts "<P><B>Diagnosis</B>"
    puts "<BLOCKQUOTE>The requested search did not "
    puts "specify any search terms.  Return to the search screen, "
    puts "enter one or more terms, and submit another search."
    puts "</BLOCKQUOTE></BODY></HTML>"
    exit
} elseif {$numindexes == 1} {
    puts "Your search was <B>not submitted</B><HR>"
    puts "<P><B>Diagnosis</B>"
    puts "<BLOCKQUOTE>The requested search did not "
    puts "specify any search terms in one of the concept fields."
    puts "Return to the search screen, "
    puts "enter one or more terms, and submit another search."
    puts "</BLOCKQUOTE></BODY></HTML>"
    exit
}

set query "$searchmethod ANY \{$termlist1\} \{$termlist2\} {$elib_id}"

# Submit search and parse results
set err [catch {eval $query} qresults]

set hits [lindex [lindex [lindex $qresults 0] 0] 1]
set errmsg ""

puts "Your search for:<p>Topic_1: <b>\"$termlist1\"</b><br>Topic_2: <b>\"$termlist2\"</b><br>Elib ID: <b>$elib_id <br>was submitted to the UC Berkeley Digital Library Database "

if {$hits == 0 || $err != 0} {
    set errmsg "$qresults"
    puts "but <B>no</B> items were retrieved.<HR>"
    if {$errmsg == ""} {
	set errmsg "No diagnostics given"
    }
    puts "<P><B>Reason for Null Retrieval</B><BLOCKQUOTE>$errmsg :: $qresults </BLOCKQUOTE>"
    puts "</BODY>"
    puts "</HTML>"
    zclose
    exit
} elseif {$hits == 1} {
    puts "where <B>1</B> record was found."
} else {
    puts "where <B>$hits</B> matching records records were found."
    puts "As requested, <b>$maxnum</b> of the top-ranked records will be shown"
    puts "in the tilebars display below."
}

# Handle maxnum < 1

if {$maxnum < 1} {
    puts "<HR><P><B>No records will be displayed</B>"
    puts "<BLOCKQUOTE>The number of records requested "
    puts "was zero (or blank).  If you would like to see "
    puts "any of the records that were found, set "
    puts "the maximum number to retrieve appropriately "
    puts "and resubmit your search."
    puts "</BLOCKQUOTE></BODY></HTML>"
    zclose
    exit
}
#add some space and a rule
puts "<p><hr><p>"

set numdisp 0
set numrecv [lindex [lindex [lindex $qresults 0] 1] 1]
set num [lindex [lindex [lindex $qresults 0] 2] 1]
set recs [lrange $qresults 1 end]
set LEFTBRACKET "\["
set RIGHTBRACKET "\]"
set QUOTES "\""
	
# Format records
#puts "<p><APPLET archive=\"mvd0.zip\" CODEBASE=$codebase CODE=\"STileBarsEn\
try.class\"  WIDTH=500 HEIGHT=60>";
#puts "<PARAM NAME=\"termsetcnt\" VALUE=\"2\">";
#puts "<PARAM NAME=\"termset1\" VALUE=\"$indata(terms1)\">";
#puts "<PARAM NAME=\"termset2\" VALUE=\"$indata(terms2)\">";
#puts "<PARAM NAME=\"moreURL\" VALUE=\"http://galaxy.cs.berkeley.edu:8080/cgi-bin/stb2?elib_id=$elib_id\">";
#puts "<B>You need Java to run this applet.</B>";
#puts "</APPLET><p>\n";


puts "\n<APPLET archive=\"mvd0.zip\" CODE=\"STileBars.class\" CODEBASE=\"/java\" NAME=\"tb\" WIDTH=40 HEIGHT=500 ALIGN=left HSPACE=0>"
# termset count is currently limited to two
puts "<PARAM NAME=\"termsetcount\" VALUE=\"2\">"
# need to ascii armour the following two
puts "<PARAM NAME=\"termset1\" VALUE=\"$indata(terms1)\">"
puts "<PARAM NAME=\"termset2\" VALUE=\"$indata(terms2)\">"

set barnum 0

while {$numdisp < $maxnum && $numrecv > 0} {

    foreach record $recs {
	getminmax $record
    }

    foreach record $recs {
	incr barnum
	set rank [lindex $record 0]
	set y  [lindex $record 1]
	set bar1 [lindex $record 2]
	set bar2 [lindex $record 3]
	set yset [split $y "\n"]
	foreach line $yset {
	    if {[regexp "<ELIB-BIB>" $line match] == 1} {
		#ignore the main tags
	    } else { #it is a normal line??
		if {[regexp "(<)(\[^>\]*)(>)(\[^<\]*)(</.*>)" \
			 $line match op tag cl contents endtag] == 0} {
		    # do nothing
		} else {
		    switch $tag {
			"ID" {
			    set CURRENT_ID $contents
			}
			"TITLE" {
			    set CURRENT_TITLE $contents
			}
			"TYPE" {
			    set CURRENT_TYPE $contents
			}
			default {#do nothing
			}
		    }
		}
	    }
	}
	# should now have the ID and TITLE
	set dirnum [format "%04d" [expr ($CURRENT_ID/100)*100]]
	set pagedir "$DOCDIR/$dirnum/$CURRENT_ID/GIF-INLINE"
	set txtpagedir "$DOCDIR/$dirnum/$CURRENT_ID/OCR-ASCII-NOZONE"
	set numpages [get_page_count $txtpagedir]
	# set url "http://$env(SERVER_NAME):8080/DOCS/$dirnum/$CURRENT_ID"
	# set urlpage "http://$env(SERVER_NAME):8080/DOCS/$dirnum/$CURRENT_ID/GIF-INLINE/"
	# set url "http://elib.cs.berkeley.edu/TR/ELIB:$CURRENT_ID"
	# set urlpage "http://elib.cs.berkeley.edu/TR/ELIB:$CURRENT_ID/Page/"
	# prev 2 lines changed to following 2 lines by Ginger 3/19/97
	set url "/TR/ELIB:$CURRENT_ID"
	set urlpage "/TR/ELIB:$CURRENT_ID/Page/"
	set sortedvalues [process_bar $bar1 $bar2]
	
	
	puts "\n<PARAM NAME=\"bar\" VALUE=\"$CURRENT_TITLE|$CURRENT_TYPE|$url|$url|$numpages|$sortedvalues\">";

	incr num
	incr numdisp
	if {$numdisp >= $maxnum} {
	    break
	}
    }
    set nextrec $num
    set err [catch {zdisplay 4} x]
    set numrecv [lindex [lindex [lindex $x 0] 2] 1]
    set num [lindex [lindex [lindex $x 0] 3] 1]
    set recs [lrange $x 1 end]
}

puts "</APPLET>\n";

# puts "<APPLET archive=\"mvd0.zip\" CODEBASE=\"http://galaxy.cs.berkeley.edu:8080/java\"  CODE=\"ImageBrowser.class\" NAME=\"ib\" WIDTH=750 HEIGHT=1100>"
# prev line changed to following line by Ginger 3/19/97
puts "<APPLET archive=\"mvd0.zip\" CODEBASE=\"/java\"  CODE=\"ImageBrowser.class\" NAME=\"ib\" WIDTH=750 HEIGHT=1100>"

puts "<PARAM NAME=\"title\" VALUE=\"$CURRENT_TITLE\">"
puts "<PARAM NAME=\"imageurl\" VALUE=\"$url/ShowPage?format=raw-gif&page=\">"
puts "<PARAM NAME=\"ocrurl\" VALUE=\"$url/ShowPage?format=xdoc&page=\">"
puts "<PARAM NAME=\"search\" VALUE=\"$indata(terms1)|$indata(terms2)\">\n";
puts "</APPLET>\n";

puts "<hr><table width=100%><tr align = center>\n";
puts "<td><a href = \"/\">";
puts "<img src = \"/graphics/home.gif\" border=0><br>";
puts "<small>Berkeley DL</small></a>";

puts "<td><a href = \"/data_access.html\">";
puts "<img src = \"/graphics/matrix.gif\"border =0><br>";
puts "<small>AccessMatrix</small></a>";

puts "<td><a href = \"/tilebars/about.html\">";
puts "<img src = \"/graphics/info.gif\" border =0><br>";
puts "<small>About TileBars</small></a>";

puts "<td><a href = \"/docs/index.html\">";
puts "<img src = \"/graphics/doc.gif\" border =0><br>";
puts "<small>Documents</small></a>";

puts "<td><a href = \"/cgi-bin/mail\">";
puts "<img src = \"/graphics/mail.gif\" border =0><br>";
puts "<small>Comments</small></a>";
puts "</table><hr>";
puts "</BODY>"
puts "</HTML>"















