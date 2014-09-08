#!./webcheshire
# 
# 
#

set CHESHIRE_CONFIGFILE "/usr3/ray/Work/z3950_3/index/CONFIG.DL"
# set CHESHIRE_CONFIGFILE "/elib/sys/cheshire/DATA/CONFIG.DL"
set CHESHIRE_DATABASE bibfile


proc get_page_count {path} {
    return [llength [glob $path/*.txt]]
}

proc process_bar {bar1 bar2} {

    set sbar1 [lsort $bar1]
    set sbar2 [lsort $bar2]
    set maxwt 0.0
    set minwt 0.999

    foreach pair $sbar1 {
	set weight [lindex $pair 1]
	if {$weight > $maxwt} { set maxwt $weight}
	if {$weight < $minwt} { set minwt $weight}
    }

    foreach pair $sbar2 {
	set weight [lindex $pair 1]
	if {$weight > $maxwt} { set maxwt $weight}
	if {$weight < $minwt} { set minwt $weight}
    }

    set mid [expr ($minwt+$maxwt)/2.0]
    set high [expr ($maxwt+$mid)/2.0]
    set low [expr ($minwt+$mid)/2.0]

    set bars ""


    foreach pair $sbar1 {
	set pagestr [lindex $pair 0]
	regexp "^(0*)(\[1234567890\]*)" $pagestr all zeros pageid
	if {$all == 0} { set pageid 0}
	set weight [lindex $pair 1]
	if {$weight >= $high} {
	    set wtval 4
        } elseif {$weight < $high && $weight >= $mid} {
	    set wtval 3
        } elseif {$weight < $mid && $weight >= $low} {
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
	if {$weight >= $high} {
	    set wtval 3
        } elseif {$weight < $high && $weight >= $mid} {
	    set wtval 2
        } elseif {$weight < $mid && $weight >= $low} {
	    set wtval 1
        } else {
	    set wtval 0
	}
	append finalbar [format "%d/%d," $pageid $wtval]
    }

    set finalbar [string trimright $finalbar ","]
    append finalbar ";"

}


# set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL \
		 SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED \
		 SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR \
		 REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH \
		 HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}

#NOTE: all the following set env items are for testing only
set env(SERVER_SOFTWARE) test_server_software
set env(SERVER_NAME) test_server_name
set env(GATEWAY_INTERFACE) test_gateway
set env(SERVER_PROTOCOL) test_protocol
		 
set env(SERVER_PORT) test_port
set env(REQUEST_METHOD) "GET"
set env(PATH_INFO) test_path
set env(PATH_TRANSLATED) test_path_trans
		 
set env(SCRIPT_NAME) test_script_name
set env(QUERY_STRING) "terms1=sierra&terms2=stream+water&max=10"
set env(REMOTE_HOST) test_rem_host
set env(REMOTE_ADDR) test_rem_addr
		 
set env(REMOTE_USER) test_rem_user
set env(AUTH_TYPE) test_rem_auth
set env(CONTENT_TYPE) test_content_type
set env(CONTENT_LENGTH) test_content_length
set env(HTTP_ACCEPT) test_http_accept
set env(HTTP_REFERER) test_http_referer
set env(HTTP_USER_AGENT) test_http_user_agent


puts "Content-type: text/HTML\n"
puts "<HTML>"
puts "<HEAD>"
puts "<TITLE> Cheshire II TileBar Search Results </TITLE>"
puts "</HEAD>"

puts "<BODY VLINK =\"990000\" \
    BACKGROUND=\"http://elib.cs.Berkeley.EDU/cheshire/paper3.gif\">" 
puts "<H2><IMG SRC=\"http://elib.cs.Berkeley.EDU/cheshire/smallcat.gif\" \
    ALIGN=middle> Cheshire II TileBar Search Results </H2>"
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

set err [catch {set maxnum $indata(max)} junk]
set maxrecs "max"
if {$err} {
    set maxnum 20;
    set indata(max) $maxnum
}

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

set query "$searchmethod ANY \{$termlist1\} \{$termlist2\}"

# Submit search and parse results
set err [catch {eval $query} qresults]

set hits [lindex [lindex [lindex $qresults 0] 0] 1]
set errmsg ""

puts "Your search for:<p>Topic_1: <b>\"$termlist1\"</b><br>Topic_2: <b>\"$termlist2\"</b> <br>was submitted to the UC Berkeley Digital Library Database "

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
    puts "where <B>$hits</B> records were found."
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

# puts "\n<APPLET archive=\"mvd0.zip\" CODE=\"TileBars.class\" CODEBASE=\"http://$env(SERVER_NAME)/java\" WIDTH=700 HEIGHT=1100>"
puts "\n<APPLET archive=\"mvd0.zip\" CODE=\"TileBars.class\" CODEBASE=\"file:///usr3/httpd/htdocs/java\" WIDTH=500 HEIGHT=[expr ($numrecv*40)+10]>"
	
puts "<PARAM NAME=\"hits\" VALUE=\"$numrecv\">"
# termset count is currently limited to two
puts "<PARAM NAME=\"termsetcount\" VALUE=\"2\">"
# need to ascii armour the following two
puts "<PARAM NAME=\"termset1\" VALUE=\"$indata(terms1)\">"
puts "<PARAM NAME=\"termset2\" VALUE=\"$indata(terms2)\">"

set barnum 0

while {$numdisp < $maxnum && $numrecv > 0} {

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
	
	set pagedir "/elib/data/disk/disk5/documents/$CURRENT_ID/GIF-INLINE"
	set txtpagedir "/elib/data/disk/disk5/documents/$CURRENT_ID/OCR-ASCII-NOZONE"
	set numpages [get_page_count $txtpagedir]
	set url "http://$env(SERVER_NAME)/TR/ELIB:$CURRENT_ID"
	set urlpage "http://$env(SERVER_NAME)/TR/ELIB:$CURRENT_ID/Page/"
	set sortedvalues [process_bar $bar1 $bar2]
	
	
	puts "\n<PARAM NAME=\"bar$barnum\" VALUE=\"$CURRENT_TITLE|$CURRENT_TYPE|$url|$urlpage|$numpages|$sortedvalues\">";

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


if {$nextrec <= $hits} {
    puts "<HR>"
    puts "<FORM \
ACTION=\"http://elib.cs.Berkeley.EDU/cgi-bin/tilebars.cheshire\" \
METHOD=POST>"
    set indata(firstrec) $nextrec
    foreach item [array names indata] {
	puts "<INPUT TYPE=hidden NAME=\"$item\" VALUE=\"$indata($item)\">"
    }
    if {[expr $hits-$nextrec+1] == 1} {
	puts "<B>Display last item</B>:"
    } elseif {[expr $hits-$nextrec+1] < $indata($maxrecs)} {
	puts "<B>Display last [expr $hits-$nextrec+1] items</B>:"
    } else {
	puts "<B>Display next $indata($maxrecs) items</B>:"
    }
    puts "<BR><INPUT TYPE=submit VALUE=\"Next\">"
    puts "</FORM>"
}

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
