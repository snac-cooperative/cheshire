# pReformat -- Reformat current record display.

proc pReformat {} {
    global currFormat currFormatName formatList nRetr searchSet stdFmt tmpdir
    global currSelectList fname rectype MARC SGMLMARC OPAC SGML SUTRS currHost
    global currHostName ERROR

    # if the currFormat and currFormatName variable match, then there's
    # no need to update and we return.  Otherwise, set the currFormatName
    # to match the currFormat and proceed to redisplay for that format
    foreach l $formatList {
	if {"[lindex $l 0]" == "$currFormat"} {
	    if {"[lindex $l 1]" == "$currFormatName"} {
		return
	    } else {
		set currFormatName "[lindex $l 1]"
		break
	    }
	}
    }

    # if there's no records to reformat, return
    if {$nRetr <= 0} return

    # save our current relative location in the record display window
    set ylist [.retrieve.text yview]
    set y [lindex $ylist 0]

    # flash indicator light and get rid of existing text in display window
    pActionLabelOn "Formatting" 1 flash
    pClearROText .retrieve.text

    # for all the records we've gotten from server so far for this search,
    # output a record number/select button, and then the record.  If
    # we're dealing with a bibliographic record display that's *not* a
    # MARC format display, then tag the record with author/subject
    # hypertext links.
    for {set i 1} {$i <= $nRetr} {incr i} {
	pLabelRecord .retrieve.text $i "%d\."
	set txtlines [pAppendROText .retrieve.text "\n\n%s\n\n" \
			   [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]]
	#We should do if check to make sure we're displaying MARC or SGML/MARC
	#and if so, then do fancy display formatting
	if {("$rectype" == "$SGMLMARC") || ("$rectype" == "$MARC") || ("$rectype" == "$OPAC")} {
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

    # if any of currently displayed records are already selected, 
    # make sure their "Select" buttons reflect that.
    foreach i $currSelectList {
	.retrieve.text tag configure selectTag$i -relief sunken \
	    -background Snow4
    }

    # Move the display to its approximate former position
    .retrieve.text yview moveto $y

    # log the reformat
    pLogCmd 2

    pActionLabelOn "Inactive" 0 noflash
}



