proc pMoreButtonCmd {min} {
    global nHits nRetr totRcvd stdFmt currFormat searchSet currHostName tmpdir
    global currPacketSize recsyntax rectype fname MARC SGMLMARC OPAC SGML SUTRS
    global ERROR csStage currHost currClassClusDB ccDTDname

    # if we've already retrieved all records, return
    if {$nRetr >= $nHits} return

    # find out beginning and ending location of text in results window
    scan [.retrieve.scroll get] "%f %f" first last

    if {($min >= $currPacketSize) && \
	    ($last < 1)} return

    # if there are some records to retrieve..

    #find out the line number of the last line on screen
    scan [.retrieve.text index @0,0] "%f" lineNr
    set pntLoc [string first . $lineNr]
    set oldLastLine [string range $lineNr 0 [expr $pntLoc - 1] ]
    set oldLastLine [expr $oldLastLine - 1]

    #detach scrollbar from text window
    .retrieve.scroll configure -command ""

    # flash the status light
    if {$min >= $currPacketSize} {
	pActionLabelOn  "Retrieving" 1 flash
        .statusbar.bottom.a.scale configure -state normal
    }

    # disable the user's ability to change record format while we're
    # getting records from server
    .mbar.format configure -state disabled

    # set the appropriate DTD names for parsing retrieved records
    if {$csStage == 1} {
	if {[file exists $tmpdir$ccDTDname] == 0} {
	    set ccDTDname "$currClassClusDB"
	}
    }

    if {$csStage >= 2} {
	set rectype $SGMLMARC
	set fname "$currHost$rectype.dtd"
	if {[file exists $tmpdir$fname] == 0} {
	    set fname "$currHost$rectype.dtd"
	
	}
    }
	
    set totRcvd 0

    # while there are records to get
    while {($nRetr < $nHits) && ($totRcvd < $min)} {

	set nextRec [expr $nRetr+1]
	if {$csStage == 1} {
	    # if we're in class cluster retrieval (stage 1)
	    set saveESN [lindex [zshow pElementSetNames] 1]
	    zset pElementSetNames B
	    set err [catch {zdisplay Clusters 4 $nextRec} retrieval]
	    zset pElementSetNames $saveESN
	} elseif {$csStage == 2} {
	    # if we're getting bib records from cluster search
	    set saveESN [lindex [zshow pElementSetNames] 1]
	    zset pElementSetNames F
	    set err [catch {zdisplay Default 4 $nextRec} retrieval]
	    zset pElementSetNames $saveESN

	} else {
	    # otherwise, normal boolean or direct ranking retrieval
	    set err [catch {zdisplay 4 $nextRec} retrieval]
	}

	# let the user know if something goes wrong getting recordss from
	# server and log the error
	if {$err == 1} {
	    pDialog .d {Display Error} \
		[lindex $retrieval 1] {error} 0 OK
	    set ERROR $retrieval
	    pLogCmd 37
	    break
	}
	
	# update the status bar
	set header [lindex $retrieval 0]
	set nRcvd [lindex [lindex $header 2] 1]
	incr totRcvd $nRcvd
	incr nRetr $nRcvd

	.statusbar.bottom.a.scale set $nRetr
	update

	# for all the records we've got, output a record label to the
	# retrieval window, and then appropriately format the record
	# and display it in the retrieval window
	for {set i $nextRec; set j 1} {$i <= $nRetr} {incr i; incr j} {
	    set searchSet($i) [lindex $retrieval $j]
	    if {$csStage == 1} {
		pLabelRecord .retrieve.text $i "Topic Area %d\."
		pAppendROText .retrieve.text "\n\n%s\n\n" \
		    [zformat lccshort $searchSet($i) 2 -1 500 $tmpdir$ccDTDname]
	    } else {
		pLabelRecord .retrieve.text $i "%d\."
		set txtlines [pAppendROText .retrieve.text "\n\n%s\n\n" \
			     [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]]
#We should do if check to make sure we're displaying MARC or SGML/MARC
#and if so, then do fancy display formatting
# XXX NOTE -- if currFormat != marc below is nasty hack to allow for
# MARC format display and should be blown away when we remove MARC display
		if {("$rectype" == "$SGMLMARC") || \
			("$rectype" == "$MARC") || \
			("$rectype" == "$OPAC")} {
		    if {"$currFormat" != "marc"} {
			pOPACtag $txtlines $currFormat $i
		    }
		}
                if {("$rectype" == "$SGML")} {
                    if {("$currHostName" == "TREC")} {
                        pTRECtag $txtlines $i
                    }
                }
	    }
	}
    }

    #find out the line number of the last line on screen now
    scan [.retrieve.text index end] "%f" lineNr
    set newLastLine $lineNr

    #move window to right spot
    if {($newLastLine != 0) && ($oldLastLine != 0)} {
	.retrieve.text yview moveto [expr $oldLastLine / $newLastLine]
	.retrieve.text yview scroll 1 unit
    }

    #reattach scroll bar to text window
    .retrieve.scroll configure -command ".retrieve.text yview"

    .statusbar.bottom.a.scale configure -state disabled
    pActionLabelOn "Inactive" 0 noflash

    # restore user's ability to change record display format
    .mbar.format configure -state normal

    # log the transaction
    pLogCmd 14

}





