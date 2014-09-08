#!/usr/old_users/ray/Work/cheshire/bin/webcheshire
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

puts "<BODY VLINK =\"990000\" BACKGROUND=\"/GIFS/paper3.gif\">" 
puts "<H2><IMG SRC=\"/GIFS/smallcat.gif\" \
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

   if {$servername == "TESTSERVER"} {
	set CHESHIRE_DATABASE bibfile
        set CHESHIRE_NUMREQUESTED 10
	set CHESHIRE_NUM_START 0
        set CHESHIRE_CONFIGFILE "/usr/users/ray/Work/cheshire/index/testconfig.new"
	set CURRENT_TERMS [lrange $querystring 1 end]
	set DTD "/usr/users/ray/Work/cheshire/doc/USMARC09.DTD"
	set query "lfind $querystring"	
   } else {
	set query "zfind $querystring"	
   }
# might want to do something here about maxnum and fmt

   if {[info exists indata] != 0} {
       unset indata
   }

   set indata(server) $servername

   set searchmethod "zfind"

   set indata(field1idx) [lindex $querystring 0]

   set indata(field1cont) [lrange $querystring 1 end]

   set indata(field2idx) "author"

   set indata(field2cont) ""

   set indata(field3idx) "title"

   set indata(field3cont) ""

   set indata(boolean) "and"

   set rec1 1
   set indata(firstrec) $rec1

   set maxnum 10
   set indata(maxrecs) $maxnum

   set fmt "HTMLLONG"
   set indata(format) $fmt

   set INFOBUS 0

} else {
    # data arrived by POST  

    set searchmethod "zfind" 

    set i1 "field1idx"
    set c1 "field1cont"
    
    set i2 "field2idx"
    set c2 "field2cont"
    
    set i3 "field3idx"
    set c3 "field3cont"
    
    set bool "boolean"

    set servername $indata(server)
    set INFOBUS 0

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
    
# Determine whether an INNOPAC server is requested
#   and replace topic searches by subject searches
#   since INNOPAC does not handle the former

    switch $servername {
	"UMICHBUS" -
	"OHIOLINK" -
	"HONGKONG" -
	"HKAPA" -
	"BU" -
	"WEL" -
	"HKUST" {set innopac 1}
	default {set innopac 0}
    }

    if {$innopac == 1 && $indata($i1) == "topic"} {
	set indata($i1) "subject"
    }
    if {$innopac == 1 && $indata($i2) == "topic"} {
	set indata($i2) "subject"
    }
    if {$innopac == 1 && $indata($i3) == "topic"} {
	set indata($i3) "subject"
    }

# Map MELVYL A&I topic searches to the keyword index


    switch $servername {
	"MELVYL" -
	"TEN" -
	"PE" {set melvyldbs 1}
	default {set melvyldbs 0}
    }
	
    if {$melvyldbs == 1 && $indata($i1) == "topic"} {
	set indata($i1) "subject"
    }
    if {$melvyldbs == 1 && $indata($i2) == "topic"} {
	set indata($i2) "subject"
    }
    if {$melvyldbs == 1 && $indata($i3) == "topic"} {
	set indata($i3) "subject"
    }


# Map topic to kw on other hosts

    switch $servername {
	"DUKE" -
	"UNCC" -
	"CMU" {set OtherKw 1}
	default {set OtherKw 0}
    }
    if {$OtherKw == 1 && $indata($i1) == "topic"} {
	set indata($i1) "any"
    }
    if {$OtherKw == 1 && $indata($i2) == "topic"} {
	set indata($i2) "any"
    }
    if {$OtherKw == 1 && $indata($i3) == "topic"} {
	set indata($i3) "any"
    }

# Map topic to subject where necessary

    if {$servername == "LC_BOOKS" && $indata($i1) == "topic"} {
	set indata($i1) "subject"
    }
    if {$servername == "LC_BOOKS" && $indata($i2) == "topic"} {
	set indata($i2) "subject"
    }
    if {$servername == "LC_BOOKS" && $indata($i3) == "topic"} {
	set indata($i3) "subject"
    }

# Some stuff needed for backend searching
	
   if {$servername == "TESTSERVER"} {
	set searchmethod "search"
	set CHESHIRE_DATABASE bibfile
        set CHESHIRE_NUMREQUESTED $maxnum
	set CHESHIRE_NUM_START $rec1
        set CHESHIRE_CONFIGFILE "/usr/users/ray/Work/cheshire/index/testconfig.new"
	set DTD "/usr/users/ray/Work/cheshire/doc/USMARC09.DTD"
   }
	

# Determine the number of indexes
   set CURRENT_TERMS ""
   
   if {$indata($c1) != ""} {
       incr numindexes
       append CURRENT_TERMS " $indata($c1)"
       regsub -all -nocase { and } $indata($c1) " " indata($c1);
   }
   if {$indata($c2) != ""} {
       incr numindexes
       append CURRENT_TERMS " $indata($c2)"
       regsub -all -nocase { and } $indata($c2) " " indata($c2);
   }
   if {$indata($c3) != ""} {
       incr numindexes
       append CURRENT_TERMS " $indata($c3)"
       regsub -all -nocase { and } $indata($c3) " " indata($c3);
   }
   
   if {$numindexes == 0} {
       puts "Your search was <B>not submitted</B><HR>"
       puts "<P><B>Diagnosis</B>"
       puts "<BLOCKQUOTE>The requested search did not "
       puts "specify any search terms.  Return to the search screen, "
       puts "enter one or more terms, and submit another search."
       puts "</BLOCKQUOTE></BODY></HTML>"
       exit
   } elseif {$numindexes > 1 && $innopac != 0} {
       puts "Your search was <B>not submitted</B><HR>"
       puts "<P><B>Diagnosis</B>"
       puts "<BLOCKQUOTE>The host server you selected "
       puts "cannot process multi-index queries.  "
       puts "Choose a single index and resubmit \
	       your search.</BLOCKQUOTE>"
       puts "</BODY></HTML>"
       exit
   } else { 

       set query "$searchmethod "
       set firstterm 1
       
       if {$indata($i1) != "none" && $indata($c1) != ""} {
	   if {($servername == "cheshire" || $servername == "SUNSITE"
           || $servername == "TESTSERVER" || $servername == "TESTSERVERZ") 
	   && ($indata($i1) == "topic" || $indata($i1) == "any")} {
	       append query "($indata($i1) @ \{$indata($c1)\} )"
	   } else {
	       append query "($indata($i1)  \{$indata($c1)\} )"
	   }
	   set firstterm 0
       }
       if {$indata($i2) != "none" && $indata($c2) != ""} {
	   if {$firstterm == 0} {
	       append query " $indata($bool) "
	   }
	   if {($servername == "cheshire" || $servername == "SUNSITE"
           || $servername == "TESTSERVER" || $servername == "TESTSERVERZ") 
	   && ($indata($i2) == "topic" || $indata($i2) == "any")} {
	       append query "($indata($i2) @ \{$indata($c2)\} )"
	   } else {
	       append query "($indata($i2) \{$indata($c2)\} )"
	   }
	   set firstterm 0
       }
       if {$indata($i3) != "none" && $indata($c3) != ""} {
	   if {$firstterm == 0} {
	       append query " $indata($bool) "
	   }
	   if {($servername == "cheshire" || $servername == "SUNSITE"
           || $servername == "TESTSERVER" || $servername == "TESTSERVERZ") 
	   && ($indata($i2) == "topic" || $indata($i2) == "any")} {
	       append query "($indata($i3) @ \{$indata($c3)\} )"
	   } else {
	       append query "($indata($i3) \{$indata($c3)\} )"
	   }
	   set firstterm 0
       }
       
   }

   regsub -all { [0-9][0-9.,]*\ *} $query " " query
}

switch $servername {
    "TESTSERVER" {set serverdisp "Cheshire Sample Test Database"}
    "TESTSERVERZ" {set serverdisp "Cheshire Sample Test Database (via Z39.50)"}
    "cheshire" {set serverdisp "UCB Astr-Math-Stat"}
    "SUNSITE" {set serverdisp "UCB Physical Sciences"}
    "MELVYL" {set serverdisp "UC Books"}
    "TEN" {set serverdisp "UC Ten-year"}
    "PE" {set serverdisp "UC Periodicals"}
    "PENN" {set serverdisp "Penn State"}
    "ATT" {set serverdisp "AT&T Library"}
    "ATT_CIA" {set serverdisp "CIA World Factbook"}
    "OHIOLINK" {set serverdisp "Ohio Public Univ's"}
    "HONGKONG" {set serverdisp "Chinese U of Hong Kong"}
    "DUKE" {set serverdisp "Duke Univ"}
    "GILS" {set serverdisp "Govt Info Locator"}
    "UMICHBUS" {set serverdisp "Michigan Business"}
    "HKAPA" {set serverdisp "Hong Kong Acad Perf Art"}
    "HKUST" {set serverdisp "Hong Kong U Sci Tech"}
    "BU" {set serverdisp "Boston U"}
    "CMU" {set serverdisp "Carnegie Mellon U"}
    "UNCC" {set serverdisp "UNC Chapel Hill"}
    "WEL" {set serverdisp "Wellesley College"}
    "MAGS" {set serverdisp "Expanded Academic Index"}
    "ABI" {set serverdisp "ABI Inform"}
    "INS" {set serverdisp "Inspec"}
    "NEWS" {set serverdisp "National Newspaper Index"}
    "COMP" {set serverdisp "Computer Database"}
    "LC_BOOKS" {set serverdisp "Library of Congress"}
    default {set serverdisp $servername}
}

# Attempt connection with server
if {$servername != "TESTSERVER"} {
    if {$servername == "TESTSERVERZ"} {
	# do a zselect with address db and port  to set up the server correctly...
	set err [catch {zselect $servername sherlock.berkeley.edu bibfile 2100} sresults]
	    
    } else {
	set err [catch {zselect $servername} sresults]
    }

    if {$err != 0} {
	puts "Cheshire was <B>unable to connect</B> with $serverdisp."
	puts "<HR><P><B>Reason for Connection Failure</B>"
	if {$sresults == ""} {
	    puts "No diagnostics given"
	} else {
	    puts "<BLOCKQUOTE>$sresults</BLOCKQUOTE>"
	}
	puts "</BODY></HTML>"
	exit
    }
}
# Display search and server

puts "Your search, encoded as <I>$query</I>, was submitted to the \
<I>$serverdisp</I> server,"

# Submit search and parse results
set err [catch {eval $query} qresults]


if {$servername != "TESTSERVER"} {
   set hits [lindex [lindex [lindex $qresults 0] 2] 1]
   set errmsg "[lindex $qresults 1]"
   set hittype "Hits"
} else { #local searches are formatted differently...

   set hittype [lindex [lindex [lindex $qresults 0] 0] 0]
   set hits [lindex [lindex [lindex $qresults 0] 0] 1]
   set errmsg ""
}

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

# Translate cryptic error messages

if {[string first "Resource Control" $errmsg] >= 0} {
    set errmsg "Peak load restriction: Replace common terms or \
try again during off hours."
}

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
if {$servername != "TESTSERVER"} {
    
    zset pElementSetNames F
    
    switch $servername {
	"ATT_CIA" -
	"GILS" {set recsntx SUTRS; set rectype 4}
	"PE" -
	"TEN" -
	"MELVYL" {set recsntx OPAC; set rectype 3}
	default {set recsntx MARC; set rectype 1}
    }
    zset recsyntax $recsntx
    
    set err [catch {zdisplay 4 $rec1} x]
    
    if {$err !=0} {
	puts "<HR><P><B>Display error</B><BLOCKQUOTE>The first record \
	requested may not exist. Compare its number with the number of \
	records found."
	puts "</BODY>"
	puts "</HTML>"
	zclose
	exit
    }

    set numdisp 0
    set numrecv [lindex [lindex [lindex $x 0] 2] 1]
    set num [lindex [lindex [lindex $x 0] 3] 1]
    set recs [lrange $x 1 end]
    set LEFTBRACKET "\["
    set RIGHTBRACKET "\]"
    set QUOTES "\""
    
} else { # Extract the UCBDL stuff...

   set numdisp 0
    set numrecv [lindex [lindex [lindex $qresults 0] 1] 1]
    set num [lindex [lindex [lindex  $qresults 0] 2] 1]
    set recs $sresults
    set LEFTBRACKET "\["
    set RIGHTBRACKET "\]"
    set QUOTES "\""
}

# Format records


while {$numdisp < $maxnum && $numrecv > 0} {

    foreach record $recs {
	set y "$record"
	puts "<HR>"
	
	if {$servername == "ATT_CIA"} {
	    puts "<H3>CIA FACT BOOK ENTRY:</H3>"
	    puts "<p>"
	    set yset [split $y "\n"]
	    foreach line $yset {
		if {[regsub "(^\[^:\]*):(.*)" $line \
			{<p><strong>\1</strong>: \2} outline]} {
		    puts $outline 	
		} elseif {[regsub "^\[*\].*" $line \
			{<p><H2>\0</H2><p>} outline]} {
		    puts $outline
		} else {
		    puts $outline
		}
	    }
	    puts "<p>"
	} elseif {$servername == "GILS"} {
	    puts "<H3>Government Information Locator Entry:</H3>"
	    puts "<p>"
	    set yset [split $y "\n"]
	    foreach line $yset {
		regsub "(^\[^:\]*):(.*)" $line \
			{<strong>\1</strong>: \2} outline
		puts $outline 
		puts "<p>"
	    }
	    puts "<p>"	    
	} elseif {$servername == "TESTSERVER"} {
	    # Must format the html for the dl display here
	    puts "<p>" 
	    set outrec [zformat HTMLLONG $y 2 $num 80 $DTD]
	    #change the hard pointers... in the old format routines...
	    # regsub -all {http://cheshire.lib.berkeley.edu/cgi-bin/Search} $outrec0 {/cgi-bin/CheshireII.tcl} outrec1
	    #regsub -all {\[|\]|\$[0-9]} $outrec1 {\\&} outrec
	    # this rather messy eval setup substitutes things like LEFTBRACKET
	    puts "$outrec"

	} else { # this is for Z39.50 retrieved MARC or OPAC records...
	    #	    puts [subst {[zformat $fmt $y $rectype $num 80]}]
	    set outrec0 [zformat $fmt $y $rectype $num 80]
	    #since this is the elib server change the hard pointers...
	    regsub -all {http://cheshire.lib.berkeley.edu/cgi-bin/Search} $outrec0 {/cgi-bin/CheshireII_sample.tcl} outrec1
	    regsub -all {\[|\]|\$[0-9]} $outrec1 {\\&} outrec
	    puts "[eval set zzz "\"[eval set outrec]\""]"
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
    puts "<FORM \
ACTION=\"/cgi-bin/CheshireII_sample.tcl\" \
METHOD=POST>"
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

puts "</BODY>"
puts "</HTML>"
######################
# end of tcl-cgi.tcl
######################

