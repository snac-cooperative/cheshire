# pFeedbkCmd -- create relevance feedback query and send to server

proc pFeedbkCmd {} {
    global index entryBG defaultBG defaultActiveBG
    global nHits nRetr searchSet stdFmt currFormat currSelectList
    global selection currPacketSize currSearch currHost currHostName
    global fname rectype MARC SGMLMARC OPAC SGML SUTRS tmpdir FBcmd 
    global ERROR csStage currClassClusDB

    # set feedback command to empty string
    set FBcmd ""

    # save some information about the current state of the Z39.50
    # connection in case we have to muck with it and then restore it
    set saveDB [lindex [zshow database] 1]
    set saveRecsyntax [lindex [zshow recsyntax] 1]
    set saveESN [lindex [zshow pElementSetNames] 1]

    # if we're doing feedback search after initial part of a class
    # cluster search, make sure we have SGML/MARC DTD available for
    # parsing records and set Z39.50 stuff to get SGML records
    if {$csStage >= 1} {
	set csFeedback 1
	set rectype $SGMLMARC
	set fname "$currHost$rectype.dtd"
	if {[file exists $tmpdir$fname] == 0} {
	    set fname "$currHost$rectype.dtd"
	}
	zset pElementSetNames F
	zset recsyntax sutrs
    }

    # create first part of feedback search as appropriate
    set saveCSstage $csStage
    if {$csStage == 1} {
	zset database $currClassClusDB
	append FBcmd "zfind clusters:"
    } elseif {$csStage == 2} {
	zset database bibfile
	append FBcmd "zfind Default:"
    } else {
	append FBcmd "zfind Default:"
    }

    # determine which record numbers we're using for feedback search
    # and append to feedback command.  NOTE: cheshire 2 server doesn't
    # take whole records back for feedback search.  The client just tells
    # the cheshire2 server the numbers of the records from the last
    # search to use, and the server takes things from there.
    if {("$selection" == "allrecs")} {
	for {set i 1} {$i <= $nRetr} {incr i} {
	    append FBcmd "$i"
	    if {$i < $nRetr} {
		append FBcmd ","
	    }
	}
	
    } else {
	set j [llength $currSelectList]
	set k 1
	foreach i $currSelectList {
	    append FBcmd "$i"
	    if {$k < $j} {
		append FBcmd ","
	    }
	    incr k
	}
    }
    
    # clear out the display before displaying new results
    pClearSearch

    # ifthis is a class cluster  search, append appropriate command end
    if {$saveCSstage == 1} {
	set csStage 2
	append FBcmd " resultsetid Default"
    } else {
	set csStage $saveCSstage
    }

    # flash blinkenlight for user
    pActionLabelOn "Searching" 1 flash

    # Log the feedback command
   if {($csStage == 2) && ($saveCSstage == 1)} {
       pLogCmd 13
   } else {
       pLogCmd 12
   }

    # Send search off to server
    set err [catch {eval $FBcmd} retrieval]
    pActionLabelOn "Inactive" 0 noflash
    set header [lindex $retrieval 0]
    
    # If there's an error during search, tell user, log it, and restore
    # Z39.50 connection to previous state, and bail out
    if {$err == 1} {
	pDialog .d {Search Error} \
	    [lindex $retrieval 1] {error} 0 OK
	set ERROR $retrieval
	pLogCmd 37
	zset database $saveDB
	zset pElementSetNames $saveESN
	zset recsyntax $saveRecsyntax
	return
    } else {
	#if no errors in search, bring lower buttons to life
	.search.bbar.history configure -state normal
	.statusbar.top.bbar2.mail configure -state normal
	.statusbar.top.bbar2.print configure -state disabled
# The following line ought to be used, but we've disabled
# printing for the client for use in AMS library
#	.statusbar.top.bbar2.print configure -state normal
	.statusbar.top.bbar2.feedback configure -state normal
	.statusbar.top.bbar2.save configure -state normal
	set nRcvd [lindex [lindex $header 3] 1]
    }    
    
    # find out how many hits and configure retrieval scale
    set nHits [lindex [lindex $header 2] 1]
    set nRetr $nRcvd
    
    .statusbar.bottom.a.hitsLabel configure -foreground black
    .statusbar.bottom.a.scale configure -foreground black \
	-to $nHits -state normal
    .statusbar.bottom.a.scale set $nRetr
    update
    
    # Get records from server and display them.  Note that we disable
    # the record format button while retrievivng records so that user
    # can't switch record format during retrieval (which screws up
    # the SGML parser).  Also note that if we're displaying bibliographic
    # records in formats other than the MARC display, we add hypertext
    # tags to author and subject fields via pOPACtag.
    pActionLabelOn "Retrieving" 1 noflash
    .mbar.format configure -state disabled

    for {set i 1} {$i <= $nRcvd} {incr i} {
	set searchSet($i) [lindex $retrieval $i]
	pLabelRecord .retrieve.text $i "%d\."
	set txtlines [pAppendROText .retrieve.text "\n\n%s\n\n" \
			  [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]]
	#We should do if check to make sure we're displaying MARC or SGML/MARC
	#and if so, then do fancy display formatting
# XXX NOTE -- the currFormat != marc test belows is nasty hack to allow
# for MARC format display and should be excised.
	if {("$rectype" == "$SGMLMARC") || ("$rectype" == "$MARC") || ("$rectype" == "$OPAC")} {
	    if {"$currFormat" != "marc"} {
		pOPACtag $txtlines $currFormat $i
	    }
	}
	if {("$rectype" == "$SGML")} {
	    if {("$currHostName" == "TREC")} {
		pTRECtag $txtlines $i .retrieve.text
	    }
	}
    }

    if {($nRetr < $nHits) && ($nRetr < $currPacketSize)} {
	pMoreButtonCmd [expr $currPacketSize-$nRetr]
    } else {
	.statusbar.bottom.a.scale configure -state disabled
	pActionLabelOn "Inactive" 0 noflash
    }

    # bring record format button back to life, and restore Z39.50 state
    .mbar.format configure -state normal

    zset database $saveDB
    zset pElementSetNames $saveESN
    zset recsyntax $saveRecsyntax

}










