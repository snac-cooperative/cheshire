# pSearchButtonCmd -- Command associated with "SEARCH" button in button
#   bar.

proc pSearchButtonCmd {} {
    global index searchTerm currServer nHits nRetr searchSet stdFmt
    global currHostName currPacketSize dialogButton currHistory
    global boolSwitch1 boolSwitch2 boolSwitch3 currSearch
    global currProbType probSearchTerm currHost csStage
    global recsyntax rectype fname MARC SGMLMARC OPAC SGML SUTRS dtd_files
    global tmpdir cmd HTAuthor HTSubject HistSrch ERROR numRecsReq
    global unusedButtonName currHostType currClassClusDB ccDTDname
    global sdlip_trace

    # make sure a server is selected
    if {[string length $currServer] == 0} {
	
	pDialog .d {No Server Selected} \
	    "Cannot submit search. You must select a server from the server menu before searching." \
	    {error} 0 OK
	set ERROR "User submitted search with no server."
	return "";
    }

    if {$sdlip_trace} {
	.twind.f.text insert end "\nNew Search:\n"
    }

    # set the 'zfind' command to empty
    set cmd ""
    set first 1;                       # 1 = first time thru command loop
    
    # This is a probabilistic search
    # current direct rank search hard-coded for @topic index
    # note that pure boolean searches are also handled here when
    # the search interface is set to probabilistic form
    
    # Get the query terms for the probabilistic search (if any)
    set prob_string [.search.probentry get 0.0 end]
    set probSearchTerm [string trim $prob_string]
	
    set contains ""
    set hasbool 0
    set boolpart ""

    # start building the query

    if {$probSearchTerm != ""} {
	set contains "<a:contains>$probSearchTerm</a:contains>"
    }
    
    foreach i [array names index] {
	if {("$index($i)" != "unused") && ("$searchTerm($i)" != "")} {
	    incr hasbool;
	    set boolelement "<a:eq><a:prop><$index($i)/></a:prop><a:literal>$searchTerm($i)</a:literal></a:eq>"
	    append boolpart $boolelement
	}
    }
    
    if {$hasbool == 2} {
	if {$contains == ""} {
	    set cmd "<a:$boolSwitch1>$boolpart</a:$boolSwitch1>"
	} else {
	    set cmd "<a:$boolSwitch1>$contains <a:$boolSwitch1>$boolpart</a:$boolSwitch1></a:$boolSwitch1>"
	}
    } elseif {$hasbool == 1 && $contains != ""} {
	set cmd "<a:$boolSwitch1>$contains $boolelement</a:$boolSwitch1>"
    } elseif {$hasbool == 1} {
	set cmd $boolpart
    } elseif {$contains != ""} {
	set cmd $contains
    }

    
    if {[string length $cmd] <= 1} {
	
	pDialog .d {Empty Search} \
	    "Cannot submit search. At least one search term is required." \
	    {error} 0 OK
	set ERROR "User submitted empty search."
    }

    #puts "COMMAND is $cmd"
    pActionLabelOn "Searching" 1 flash
    
    #perform the search
    # cmd has the basicsearch boolean or contains query parts -- We
    # need to build the rest of the basicsearch part
    #
    set clientSID [pid]

    set search "<a:searchrequest xmlns:dc=\"DublinCore:\" xmlns:a=\"DAV:\" xmlns:b=\"http://interlib.org/SDLIP/1.0#\"><a:basicsearch><a:select><a:allprop/></a:select><a:from><a:scope><a:href>sdlip://galaxy.cs.berkeley.edu:8888/$currHost</a:href></a:scope></a:from><a:where>$cmd</a:where><a:limit><a:nresults>$numRecsReq</a:nresults></a:limit><b:clientSID>$clientSID</b:clientSID><b:stateTimeoutReq>0</b:stateTimeoutReq></a:basicsearch></a:searchrequest>"
    
    set header2 "Content-Encoding: utf-8"
    set header3 "Content-Length: [string length $search]"
    set header4 "Content-Type: text/xml"


    
    pActionLabelOn "Inactive" 0 noflash

    # make the connection to the server...

    set serverinfo [split $currServer :]
    set serverAddress [lindex $serverinfo 0]
    set serverPort [lindex $serverinfo 1]
    set serverPath [lindex $serverinfo 2]

    if {$sdlip_trace} {
	.twind.f.text insert end "Search serverAddress: $serverAddress\n"
	.twind.f.text insert end "Search serverPort: $serverPort\n"
	.twind.f.text insert end "Search serverPath: $serverPath\n"
    }

    set header1 "SEARCH /$serverPath HTTP/1.1"
    
    set sock [socket $serverAddress $serverPort]

    puts $sock "$header1"
    puts $sock "$header2"
    puts $sock "$header3"
    puts $sock "$header4"
    puts $sock ""
    puts $sock "$search"
    flush $sock

    # output the query
    .retrieve.text configure -state normal
    catch {.retrieve.text delete 1.0 end}


    if {$sdlip_trace} {
	.twind.f.text insert end "\n******************************************* \n"
	.twind.f.text insert end "QUERY:\n"
	.twind.f.text insert end "$header1 \n"
	.twind.f.text insert end "$header2 \n"
	.twind.f.text insert end "$header3 \n"
	.twind.f.text insert end "$header4 \n"
	.twind.f.text insert end "\n"
	.twind.f.text insert end "$search \n"
	.twind.f.text insert end "\n******************************************* \n"
    }

    .retrieve.text configure -state disabled

    # now turn around and wait for results...
    # grab the http headers...
    set line 1
    set err 0
    set nRetr 0

    if {$sdlip_trace} {
	.twind.f.text insert end "Results from Query:\n"
    }
    while {[gets $sock instuff] > 0} {
	incr line	
	if {$sdlip_trace} {
	    .twind.f.text insert end "$instuff\n"
	}
	if {$line == 1} {
	    if {[string match "HTTP/1.1 207 Multi-Status" $instuff] == 1} {
		#    puts $outfile "Matched Search header: $instuff"
	    } else {
		set err 1
		# error handling....
		.retrieve.text configure -state normal
		.retrieve.text insert end  "Search not successful:\n"
		.retrieve.text insert end "$instuff \n"
		while {[gets $sock instuff] >= 0}  {
		    .retrieve.text insert end "$instuff \n"
		}
		.retrieve.text configure -state disabled
	    }
	} else {
	    set type [string trim [lindex [split $instuff :] 0] " "]
	    set value [string trim [lindex [split $instuff :] 1] " "]
	    set header($type) $value
	}
    }

    set maxheaders $line
    # we get the length of the message from the header...
    set verify [catch {set content_length $header(Content-Length)}]

    if {$verify == 0} {
	# Use content-length to read the data
	set message [read $sock $content_length]
    } else {
	set message {}
	while {[gets $sock instuff] >= 0} {
	    incr line	
	    if {$sdlip_trace} {
		.twind.f.text insert end "$instuff\n"
	    }
	    append message $instuff
	}
    }
    if {$sdlip_trace} {
	.twind.f.text insert end "\n$message\n"
    }

    if {$err == 0} {
	displayRecs $message
    } else {
	.retrieve.text configure -state normal
	.retrieve.text insert end "Search not successful:"
	.retrieve.text insert end $message
	.retrieve.text configure -state disabled
	if {$sdlip_trace} {
	    .twind.f.text insert end "\nSearch not successful\n"
	}
    }
    
}

















