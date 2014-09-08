# display recs parses the xml records coming back from the
# server and puts them into the text box.
#

# define some procs for handling callbacks from XML query parsing
set xmlnamespaces("") {}

# set/clear some globals for parsing
set response_flag 0
set prop_pending 0
set href_pending 0
set prop_list {}
set clientsid {}
set stateTimeOutReq {}
set last_data {}
set last_tag {}
set current_tag {} 
set current_ns {}
set first_pass 1
set total_pending 0
set total_hits 0
set nHits 0
set nRetr 0
set recnum 0

proc HandleStartTag {name attlist args} {
    global xmlnamespaces
    global response_flag 
    global last_data prop_pending
    global href_pending current_tag current_ns
    global prop_list total_pending recnum

    

    foreach {attname attval} $attlist {
	if {[string match "xmlns:*" $attname] == 1} {
	    set xmlnamespaces([lindex [split $attname :] 1]) $attval
	}
    }

    set nlist [split $name :]
    if {[llength $nlist] == 1} {
	set tagname $nlist
	set namesp ""
    } else {
	set tagname [lindex $nlist 1]
	set namesp [lindex $nlist 0]
    }

    set current_tag $tagname
    set current_ns  $namesp

    if {[llength $args] > 0} {
	# Empty tags reported here
	# puts "additional args = $args"
    }

    switch -regexp $tagname {
	"multistatus" {
#	    puts "multistatus"
	}

	"^response$" {
#	    puts "response"
	    if {$recnum > 0} {
		.retrieve.text insert end "\n==================================================================================\nResponse Record #$recnum\n" red
	    }
	    incr recnum

	}

	"^propstat$" {
	}

	"^prop$" {
	    set prop_pending 1
	}

	"^href$" {
	}

	"^status$" {
	}

	"^DID$" {
	}

	"exceptionCode" {
	}

	"expectedTotal" {
	    set total_pending 1
	}

	"responsedescription" {
	}

	"stateTimeout" {
	}

	"serverSID" {
	}

	default {
	  
	    if {$prop_pending == 1} {
		# puts "start: $current_tag"		
	    } else {
		.retrieve.text insert end "Unknown tag: $current_tag  Namespace: $current_ns" red

	    }

	}
	
    }
    
}

proc HandleEndTag {name args} { 
    global xmlnamespaces
    global response_flag 
    global last_data prop_pending
    global href_pending current_tag current_ns
    global clientsid stateTimeOutReq queryoptions
    
    set nlist [split $name :]
    if {[llength $nlist] == 1} {
	set tagname $nlist
	set namesp ""
    } else {
	set tagname [lindex $nlist 1]
	set namesp [lindex $nlist 0]
    }
    #    puts "End Tag </$tagname> (namespace $namesp = $xmlnamespaces($namesp))"
    
    if {[llength $args] > 0} {
	# Empty tags reported here
	#	puts "additional args = $args"
    }
    
    switch -regexp $tagname {
	"multistatus" {
	    #	    puts "multistatus"
	}
	
	"^response$" {
	    #	    puts "BASICSEARCH"
	}
	
	"^propstat$" {
	}
	
	"^prop$" {
	    set prop_pending 0
	}
	
	"^href$" {
	}

	"^status$" {
	}

	"exceptionCode" {
	}
	
	"expectedTotal" {
	    set total_pending 0
	}

	"responsedescription" {
	}
	
	default {
	    #puts "end: $current_tag"		
	    # puts "unknown end tag: $tagname"
	}
	
    }
    
}

proc HandleData {data} {
    global last_data current_attrval lit_pending href_pending limit_flag from
    global current_tag current_ns last_tag total_pending nHits nRetr
    global numRecsReq 

    if {[string length $data] > 0 
    && [regexp "^\[ 	\n\]*\$" $data] == 0 } {
	# puts "data: '$data'"
	# don't bother if it is all white space... otherwise
	if {$data == "&" || $data == "<" || $data == ">"
	|| $last_data == "&" || $last_data == "<" 
	|| $last_data == ">"} {
	    # This call is the result of an entity sub
	    .retrieve.text insert end "$data"
	} else {
	    if {$total_pending} {
		set nHits $data
		if {$nHits < $numRecsReq} {
		    set nRetr $nHits
		} else {
		    set nRetr $numRecsReq
		}
		set total_pending 0
	    } else {
		if {$last_tag == $current_tag} {
		    if {$data != "\n"} {
			.retrieve.text insert end "\n              $data"
		    }
		} else {
		    switch -regexp $current_tag {
			"^href$" { }
			"^expectedTotal$" {
			}
			"^stateTimeout$" {
			}
			"^prop$" {
			}
			"^serverSID$" {
			}
			"^status$" {
			    # this occurs at the end of records -- so
			    # add a newline
			}
			default {
			    .retrieve.text insert end "\n$current_tag" blue
			    if {[string match "*http://*" $data]} {
				.retrieve.text insert end ":    "
				.retrieve.text insert end "$data" hlink
			    } else {
				.retrieve.text insert end ":    $data"
			    }
			    set last_tag $current_tag
			}
		    }
		}
	    }
	}
	set last_data $data
    }
}


proc displayRecs {data} {
    global first_pass qparser nHits nRetr recnum

    if {$first_pass} {

	set qparser [xml::parser qparser]

	$qparser configure -elementstartcommand HandleStartTag
	$qparser configure -elementendcommand HandleEndTag
	$qparser configure -characterdatacommand HandleData
	$qparser configure -reportempty 1
	set first_pass 0
    } else {
	$qparser reset
    }
    set recnum 0

    .retrieve.text configure -state normal
    .retrieve.text tag configure blue -foreground blue
    .retrieve.text tag configure red -foreground red
    .retrieve.text tag configure hlink \
	-foreground blue -underline 1
    .retrieve.text tag configure visited \
	-foreground "#303080" -underline 1
    .retrieve.text tag configure hot -foreground red -underline 1

    .retrieve.text tag bind hlink <ButtonRelease-1> {
	invoke [.retrieve.text index {@%x,%y linestart}]
    }
    set lastLine ""
    .retrieve.text tag bind hlink <Enter> {
	set lastLine [.retrieve.text index {@%x,%y linestart}]
	
	.retrieve.text tag add hot "$lastLine +0 chars" "$lastLine lineend -0 chars"
	.retrieve.text config -cursor hand2
    }
    .retrieve.text tag bind hlink <Leave> {
	.retrieve.text tag remove hot 1.0 end
	.retrieve.text config -cursor xterm
    }
    .retrieve.text tag bind hlink <Motion> {
	set newLine [.retrieve.text index {@%x,%y linestart}]
	if {[string compare $newLine $lastLine] != 0} {
	    .retrieve.text tag remove hot 1.0 end
	    set lastLine $newLine

	    set tags [.retrieve.text tag names {@%x,%y}]
	    set i [lsearch -glob $tags demo-*]
	    if {$i >= 0} {
		.retrieve.text tag add hot "$lastLine +1 chars" "$lastLine lineend -1 chars"
	    }
	}
    }


    if {[catch {$qparser parse $data} err] != 0} {

	pDialog .d {XML Parsing Error} \
	    "XML Parsing Error. The data returned from the server contains an XML error: $err.\n" \
	    {error} 0 OK
	set ERROR "User submitted search with no server."
	return "";

    } 
    .statusbar.bottom.a.hitsLabel configure -fg black
    .statusbar.bottom.a.scale configure -fg black \
	    -to $nHits -state normal
    .statusbar.bottom.a.scale set $nRetr
    .statusbar.bottom.a.scale configure -state disabled

    .retrieve.text configure -state disabled
  
}


# invoke --
# This procedure is called when the user clicks on a demo description.
# It is responsible for invoking the demonstration.
#
# Arguments:
# index -	The index of the character that the user clicked on.

proc invoke index {

    set lineno [lindex [split $index .] 0]
    set line [.retrieve.text get hot.first hot.last]
    set url_start [string first "http" $line]
    set url [string range $line $url_start end]
    puts "invoking netscape on url $url"
    exec netscape $url &

}











