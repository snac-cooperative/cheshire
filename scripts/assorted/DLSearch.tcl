#!/elib/sys/cheshire/bin/webcheshire
# 
# 
#
set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL \
		 SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED \
		 SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR \
		 REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH \
		 HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}


puts "Content-type: text/HTML\n"
puts "<HTML>"
puts "<HEAD>"
puts "<TITLE>Cheshire II Search Results </TITLE>"
puts "</HEAD>"

puts "<BODY VLINK =\"990000\" \
BACKGROUND=\"/cheshire/paper3.gif\">" 
puts "<H2><IMG SRC=\"/cheshire/smallcat.gif\" \
ALIGN=middle>Cheshire II Search Results</H2>"
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

set index_list "topic title author localnum record_type_key note"

set numindexes 0

if {$argc > 0} { # this means a hyperlink search from a previous display 
    
    set servername [lindex [split $argv @] 0]
    set querystring [lindex [split $argv @] 1]
    
    # Get rid of braces, parentheses, and double dashes
    # and set to lower case. Parentheses (actually "\(" sequences)
    # kill the search.  The rest muck up the search string display.
    # The other regsubs have been added as problems cropped up.
    
    regsub -all {\/sub\ +([0-9]+)[\+\-]*\/*} $querystring {\1} querystring
    regsub -all {\/sup\ +([0-9]+)[\+\-]*\/*} $querystring {\1} querystring
    regsub -all {\\\(|\\\)} $querystring "" querystring;	#"\( and \)
    regsub -all {\{|\}} $querystring "" querystring;	# { and }
    regsub -all {\-\-|\,} $querystring "" querystring;	# -- and ,
    regsub -all {\\\'|\.} $querystring "" querystring;	# \' and .
    regsub -all {\-|\/} $querystring " " querystring;	# - and /
    regsub -all {\\\*} $querystring "" querystring;	# \*
    regsub -all { and } $querystring " " querystring;	# " and "
    regsub -all { AND } $querystring " " querystring;	# " and "
    regsub -all { [0-9][0-9.,]*\ *} $querystring " " querystring
    set querystring [string tolower $querystring]
    
    if {$servername == "UCBDL"} {
	set CHESHIRE_DATABASE bibfile
	set CHESHIRE_NUMREQUESTED $indata(maxnum)
	set CHESHIRE_NUM_START  $indata(firstrec)
    


	set CHESHIRE_CONFIGFILE "/elib/sys/cheshire/DATA/CONFIG.DL"
	set CURRENT_TERMS [lrange $querystring 1 end]
	set query "lfind $querystring"	
	set searchmethod "lfind"
    } else {
	set query "zfind $querystring"	
	set searchmethod "zfind"
    }
    # might want to do something here about maxnum and fmt

    set maxnum $indata(maxnum)
    set fmt $indata(format)

    if {[info exists indata] != 0} {
	unset indata
    }
    
    set indata(server) $servername
    
    set indata(topic) ""
    set indata(title) ""
    set indata(author) ""
    set indata(localnum) ""
    set indata(record_type_key) ""
    set indata(note) ""
    
    set indata(boolean) "and"

    # set the real thing to be searched...
    set realindex [lindex $querystring 0]
    set realdata [lrange $querystring 1 end]
    set indata($realindex) "$realdata"
    
} else {
    # data arrived by POST  
    
    # Requests for 0 records are permitted on the grounds that
    #   a user may wish to see how many hits he or she gets 
    #   prior to setting a retrieval number.

    if {$indata(firstrec) == ""} {
	set rec1 1
    } else {
	set err [catch {expr $indata(firstrec)}]
	if {$err != 0 || $indata(firstrec) < 1} {
	    puts "Your search was <B>not submitted</B>."
	    puts "<HR><P><B>Diagnosis</B>"
	    puts "<BLOCKQUOTE>Inappropriate value entered for \
		    number of starting record.</BLOCKQUOTE>"
	    puts "</BODY></HTML>"
	    exit
	}
	set rec1 $indata(firstrec)
    }
    
    if {$indata(maxrecs) == ""} {
	set maxnum 0
    } else {
	set err [catch {expr $indata(maxrecs)}]
	if {$err != 0 || $indata(maxrecs) < 0} {
	    puts "Your search was <B>not submitted</B>."
	    puts "<HR><P><B>Diagnosis</B>"
	    puts "<BLOCKQUOTE>Inappropriate value entered for \
		    number of records to retrieve.</BLOCKQUOTE>"
	    puts "</BODY></HTML>"
	    exit
	}
	set maxnum $indata(maxrecs)
    }
    
    set fmt $indata(format)
    set servername $indata(server)

    # Some stuff needed for backend searching
    if {$servername == "UCBDL"} {
	set searchmethod "search"
	set CHESHIRE_DATABASE bibfile
        set CHESHIRE_NUMREQUESTED $maxnum
	set CHESHIRE_NUM_START $rec1
        set CHESHIRE_CONFIGFILE "/elib/sys/cheshire/DATA/CONFIG.DL"
    }
    
    # Determine the number of indexes
    set CURRENT_TERMS ""
    
   foreach xx $index_list {
       if {$indata($xx) != ""} {
	   incr numindexes
	   append CURRENT_TERMS " $indata($xx)"
	   regsub -all -nocase { and } $indata($xx) " " indata($xx);
       }
   }


   # do some mapping to approved forms of things
   switch $indata(record_type_key) {
       ""  {# do nothing}
       "article" {set indata(record_type_key) "article"}
       "basin plan" {set indata(record_type_key) "basinplan"}
       "basinplan" {set indata(record_type_key) "basinplan"}
       "bulletin" {set indata(record_type_key) "bulletin"}
       "city plan" {set indata(record_type_key) "cityplan"}
       "cityplan" {set indata(record_type_key) "cityplan"}
       "city zoning ordinance" {set indata(record_type_key) "cityzoning"}
       "cityzoning" {set indata(record_type_key) "cityzoning"}
       "coastal plan" {set indata(record_type_key) "coastalplan"}
       "coastalplan" {set indata(record_type_key) "coastalplan"}
       "county general plan" {set indata(record_type_key) "generalplan"}
       "generalplan" {set indata(record_type_key) "generalplan"}
       "county zoning ordinance" {set indata(record_type_key) "countyzoning"}
       "countyzoning" {set indata(record_type_key) "countyzoning"}
       "educational" {set indata(record_type_key) "educational"}
       "EIR" {set indata(record_type_key) "eir"}
       "eir" {set indata(record_type_key) "eir"}
       "guideline" {set indata(record_type_key) "guideline"}
       "habitat plan" {set indata(record_type_key) "habitatplan"}
       "habitatplan" {set indata(record_type_key) "habitatplan"}
       "journal" {set indata(record_type_key) "journal"}
       "manual" {set indata(record_type_key) "manual"}
       "misc. plan" {set indata(record_type_key) "plan"}
       "plan" {set indata(record_type_key) "plan"}
       "newsletter" {set indata(record_type_key) "newsletter"}
       "pamphlet" {set indata(record_type_key) "pamphlet"}
       "report" {set indata(record_type_key) "report"}
       "reference" {set indata(record_type_key) "reference"}
       "regional plan" {set indata(record_type_key) "regionalplan"}
       "regionalplan" {set indata(record_type_key) "regionalplan"}
       "tribal plan" {set indata(record_type_key) "tribalplan"}
       "tribalplan" {set indata(record_type_key) "tribalplan"}
       "watershed assessment" {set indata(record_type_key) "watershedassess"}
       "watershedassessment" {set indata(record_type_key) "watershedassess"}
       "watershedplan" {set indata(record_type_key) "watershedplan"}
       default {set indata(record_type_key) ""}
   }

   if {$numindexes == 0} {
       puts "Your search was <B>not submitted</B><HR>"
       puts "<P><B>Diagnosis</B>"
       puts "<BLOCKQUOTE>The requested search did not "
       puts "specify any search terms.  Return to the search screen, "
       puts "enter one or more terms, and submit another search."
       puts "</BLOCKQUOTE></BODY></HTML>"
       exit
   } else { 
       
       set query "$searchmethod "
       set firstterm 1
       set bool $indata(boolean)
       
       foreach iname $index_list {
	   if {$indata($iname) != ""} {
	       if {$firstterm == 0} {
		   append query " $bool "
	       }
	       if {$iname == "topic"} {
		   append query "($iname @ \{$indata($iname)\} )"
		   set firstterm 0
		   
		   
	       } else {
		   append query "($iname \{$indata($iname)\} )"
		   set firstterm 0
	       }
	   }
       }
       regsub -all { [0-9][0-9.,]*\ *} $query " " query
   }
}
### grab the full-text search terms (Ginger)
set ft [join $indata(topic) "+"]

set serverdisp "UC Berkeley Digital Library"

# Display search and server

puts "Your search, encoded as <I>$query</I>, was submitted to the \
<I>$serverdisp</I> server,"

# Submit search and parse results

set err [catch {eval $query} qresults]

set hittype [lindex [lindex [lindex $qresults 0] 0] 0]
set hits [lindex [lindex [lindex $qresults 0] 0] 1]
set errmsg ""

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
}
# otherwise just build a list of records...

set sresults [lrange $qresults 1 end]

# Deal with null retrievals

if {$err != 0} {
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
    puts "These records are shown below, listed in order of relevance to your query."
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

# Retrieve records

set numdisp 0
set numrecv [lindex [lindex [lindex $qresults 0] 1] 1]
set num [lindex [lindex [lindex $qresults 0] 2] 1]
set recs $sresults
set LEFTBRACKET "\["
set RIGHTBRACKET "\]"
set QUOTES "\""



puts "<p>Click on the title to go to the document."

puts "Click on the value in the <b>Type</b> or <b>Author</b> fields to do a new search of all documents using that value. "

# Format records

while {$numdisp < $maxnum && $numrecv > 0} {

    foreach record $recs {
	set y "$record"
	puts "<HR>"

	# Must format the html for the dl display here
	puts "<p>"
	if {$hittype == "PageDocHits"} {
	    set docrank [lindex $y 0]
	    set doc [lindex $y 1]
	    set docpages [lindex $y 2]
	    set y $doc
	}
	
	set yset [split $y "\n"]
	foreach line $yset {
	    if {[regexp "<ELIB-BIB>" $line match] == 1} {
		#ignore the main tags
	    } else { #it is a normal line??
		if {[regexp "(<)(\[^>\]*)(>)(\[^<\]*)(</.*>)" \
			$line match op tag cl contents endtag] == 0} {
		    puts "$line<p>"
		} else {
		    switch $tag {
			"BIB-VERSION" -
			"ENTRY" -
			"TEXTSOUP" -
			"DATE" {# do nothing}
			"ID" {
			    # puts "<b>$num. <a href = \"/cgi-bin/doc_home?elib_id=$contents\"> (Elib-$contents): </a></b>"
			    set CURRENT_ID $contents
			}
			"TITLE" {
			    set tl [split $contents]
			    set tlkey [join $tl "+"]
			    set sl [split $CURRENT_TERMS]
			    set slkey [join $sl "+"]
			    # puts "<p><b>Title</b>: $contents."
			    puts "<b>$num. <a href=/cgi-bin/doc_home?elib_id=$CURRENT_ID&search=$ft> " 
			    puts "$contents</b></a> (Elib-$CURRENT_ID)"
			    # puts "<a href =/cgi-bin/ibhtml.pl?baseurl=%2FTR%2FELIB:$CURRENT_ID&search=$ft&title=$tlkey><img border=0 align=top src = \"/graphics/ib.gif\"></a>"
			}
			"AUTHOR" -
			"AUTHOR-PERSONAL" -
			"AUTHOR-INSTITUTIONAL" - 
			"AUTHOR-CONTRIBUTING-PERSONAL" -
			"AUTHOR-CONTRIBUTING-INSTITUTIONAL" {
			    set al [split $contents]
			    set alkey [join $al "+"]
			    puts "<br><b>Author</b>: <a href=/cgi-bin/CheshireII.tcl?UCBDL@author+$alkey>$contents</a>"
			    # puts "<br><b>Author</b>: $contents"
			} 
			"PROJECT" {
			    set al [split $contents]
			    set alkey [join $al "+"]
			    # puts "<p><b>Project</b>: <a href=/cgi-bin/CheshireII.tcl?UCBDL@note+$alkey>$contents.</a>"
			}
			"PAGES" {
			    puts "<br><b>Pages</b>: $contents"
			}
			"ORGANIZATION" {
			    # puts "<p><b>Organization</b>: $contents."
			}
			"SERIES" {
			    # puts "<p><b>Series</b>: $contents."
			}
			"TYPE" {
			    set al [split $contents]
			    set alkey [join $al "+"]
			    puts "<br><b>Type</b>: <a href=/cgi-bin/CheshireII.tcl?UCBDL@record_type_key+$alkey>$contents</a>"
			    # puts "<br><b>Type</b>: $contents"
			}
			"REVISION" {
			    # puts "<p><b>Revision</b>: $contents."
			}
			"NOTES" {
			    # puts "<p><b>Notes</b>: $contents."
			}
			"BIOREGION" {
			    # puts "<p><b>BioRegion</b>: $contents."
			}
			"ABSTRACT" {
			    # puts "<p><b>Abstract</b>: $contents."
			}
			"PAGED-REF" -
			"TEXT-REF" {#do nothing
			}
			"/ELIB-BIB" { #do nothing
			}
			default {
			    # don't want keywords, textsoup, etc.
			    # puts "<p>$contents"
			}
		    }
		}
	    }
	    if {$hittype == "PageDocHits"} {
		puts "<p><b>Individual pages (ordered by estimated relevance):</b><br>"
		foreach d $docpages {
		    puts "\[<a href=\"/cgi-bin/display_page?format=gif;elib_id=$CURRENT_ID;page=$d\"> $d </a>\] "
		}
	    }
	}	    
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
zclose


if {$nextrec <= $hits} {
    puts "<HR>"
    puts "<FORM ACTION=\"/cgi-bin/DLSearch.tcl\" METHOD=POST>"
    set indata(firstrec) $nextrec
    foreach item [array names indata] {
	puts "<INPUT TYPE=hidden NAME=\"$item\" VALUE=\"$indata($item)\">"
    }
    if {[expr $hits-$nextrec+1] == 1} {
	puts "<B>Display last item</B>:"
    } elseif {[expr $hits-$nextrec+1] < $indata(maxrecs)} {
	puts "<B>Display last [expr $hits-$nextrec+1] items</B>:"
    } else {
	puts "<B>Display next $indata(maxrecs) items</B>:"
    }
    puts "<BR><INPUT TYPE=submit VALUE=\"Next\">"
    puts "</FORM>"
}

puts "<hr>"
puts "<a href=/docs/query.shtml>Search Documents</a> | "
puts "<a href=/>UC Berkeley Digital Library Project</a> | "
puts "<a href=mailto:www@elib.cs.berkeley.edu?subject=cheshire>"
puts "www@elib.cs.berkeley.edu</a>"
puts "</BODY>"
puts "</HTML>"

######################
# end of tcl-cgi.tcl
######################

