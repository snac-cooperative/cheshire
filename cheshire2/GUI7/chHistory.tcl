# pHistoryButtonCmd

proc pHistoryButtonCmd {} {
    global currHistory textBG defaultBG defaultActiveBG
    global currRedo response index searchTerm indexList currHost
    global unusedButtonName dialogButton boolSwitch1 boolSwitch2 boolSwitch3
    global currSearch buttonBG HistSrch ERROR defaultFont entryFont
    global response

    set location "center";		# Location of window
    set histTextBG AntiqueWhite1;	# Text widget background

    toplevel .h -class History
    wm title .h "History"
    wm iconname .h "History"

    scan [wm geometry .] "%dx%d+%d+%d" width height x y
    if {"$location" == "center"} {
	wm geometry .h +[expr ($width-$x)/4]+[expr ($height-$y)/2]
    } elseif {"$location" == "nw"} {
	wm geometry .h +$x+$y
    }

    # Framework

    frame .h.bbar -relief raised -bd 1;	# Background for button bar
    frame .h.disp -relief raised -bd 1; # Background for display

    pack .h.bbar .h.disp -side top -fill both

    # Button bar

    button .h.bbar.clear -relief raised -bd 2 -text "Clear" \
	-command "pClearHist" -highlightbackground $defaultBG \
	-font $defaultFont
    button .h.bbar.exit -relief raised -bd 2 -text "Exit" \
	-command "destroy .h" -highlightbackground $defaultBG \
	-font $defaultFont
    
    pack .h.bbar.clear .h.bbar.exit\
	-side left -pady 1 -ipadx 1m -ipady 1m -fill x -expand 1

    # Display window

    text .h.disp.text -relief sunken -bd 1 \
	-height 20 \
	-width  60 \
	-background $histTextBG \
	-highlightbackground $defaultBG \
	-selectbackground $histTextBG \
	-selectborderwidth 0 \
	-font $entryFont \
	-cursor {} \
	-wrap word \
	-yscrollcommand ".h.disp.scroll set";	# Text window

    scrollbar .h.disp.scroll \
	-background $defaultBG \
	-highlightbackground $defaultBG \
	-highlightcolor $defaultBG \
	-troughcolor $buttonBG \
	-activebackground $defaultActiveBG \
	-relief sunken \
	-command ".h.disp.text yview";		# Scroll bar

    pack .h.disp.scroll -side right -fill y -padx 1m -pady 1m
    pack .h.disp.text -side left -padx 1m -pady 1m

    # display all of the history commands
    set i 1; set currRedo 1
    foreach cmd $currHistory {
	pLabelHistory .h.disp.text $i "\n%d\."
	pAppendROText .h.disp.text "\n\n%s\n" \
	    [string range $cmd 5 end]
	incr i
    }

    # Log
    pLogCmd 16

}
	
proc pHistSearch { } {	
    global currRedo currHistory currSearch HistSrch

    # See if search type allowable using current screen setup (bool or prob),
    # if no, helpful message telling user to fuck off.  If yes, do
    # appropriate resets based on whether boolean search or prob. search.
    # bool redo is listed below as pHistoryBoolRedo, prob as
    # pHistoryProbRedo
    # then update and search
    
    # Grab selected command to redo
    # Find out how many boolean terms in redoCmd
    # if you're interested, the effect of the regsubs here is to
    # remove the search terms so that any search terms containing
    # a boolean term (e.g., subject {polymers and polymerization}) do not
    # get counted in the number of boolean terms for the query.
    # XXX -- this solution is imperfect at the moment, as it won't
    # handle upper 128 (e.g. non-ASCII) characters at all.
    # any probabilistic component to the search is also removed so
    # that the "and" between the "topic @" and the boolean portion
    # of the search is removed.

    set redoCmd [lindex $currHistory [expr $currRedo-1]]
    regsub -all {\{[a-zA-Z0-9 ]+\}} $redoCmd \{\} redoCmd
    regsub {\(topic @ \{\}\) and} $redoCmd PROBPART redoCmd
    set boolCount [expr ([lcount $redoCmd " and "]+[lcount $redoCmd " AND "]+[lcount $redoCmd " or "]+[lcount $redoCmd " OR "])]
    set redoCmd [lindex $currHistory [expr $currRedo-1]]

    # if interface set for boolean and no prob. stuff in redoCmd
    # then do pHistoryBoolRedo.  If interface set for prob. and
    # there's no more than one boolean conjunction, do
    # pHistoryProbRedo.  Otherwise, tell user to reset interface
    # to boolean style to redo a search with more than one
    # boolean term.  XXX -- Eventually, this should change
    # so that interface is automatically reconfigured for
    # boolean if user wants to redo boolean with more than
    # one boolean conjunction when in prob. interface mode.
    
    if {($currSearch == "bl") && ([string first @ $redoCmd] == -1)} {
	pHistoryBoolRedo
    } elseif {($currSearch == "pr") && ($boolCount<2)} {
	pHistoryProbRedo
    } elseif {$currSearch == "pr"} {
	pDialog .d {Search Error} "Cheshire II doesn't \
 currently support redoing boolean searches with more than one \
 boolean conjunction (and/or) while the Search Interface is set to \
 \"Ranked\".  You might try manually reseting the Search Interface, \
 then redoing the search." {error} 0 OK
	set ERROR "User attempted to redo a boolean search with more than two terms using probabilistic interface."
	pLogCmd 37
	if {[winfo exists .h]} {
	    destroy .h
	}
	update
	return
    } elseif {$currSearch == "bl"} {
	pDialog .d {Search Error} "Cheshire II doesn't \
 currently support redoing probabilistic searches \(those in the History \
 list that contain an \'\@\' symbol\) while the Search Interface is set to \
 \"Boolean\".  If you're connected to the Cheshire II server for the U.C. \
 Berkeley Physical Science libraries, you might try manually reseting the \
 Search Interface, then \
 redoing the search.  If connected to another server such as Melvyl, you cannot perform probabilistic searches." {error} 0 OK
	set ERROR "User attempted to redo a probabilistic search while connected to server other than Cheshire II."
	pLogCmd 37
	if {[winfo exists .h]} {
	    destroy .h
	}
	update
	return
    }
    
    # get rid of history window
    if {[winfo exists .h]} {
	destroy .h
    }
    update
    
    # Perform search
    set HistSrch 1
    pSearchButtonCmd
    
}


# pLabelHistory -- Place a label and redo button for command i in
# widget w using specified format

proc pLabelHistory {w i format} {
    global currRedo defaultFont

    # configure text window for writing and insert record label
    $w configure -state normal
    scan [$w index end] %f lineNr
    $w insert $lineNr [format $format $i]

    # figure out where we're going to be "Redo" button
    set pntLoc [string first . $lineNr]
    set lineLoc [string range $lineNr 0 [expr $pntLoc - 1] ]

    # insert text of Redo button
    $w insert end "  Redo  "

    # Tag the Redo button
    set tagstart [$w search " Redo" $lineLoc.0]
    set pntLoc [string first . $tagstart]
    set startLoc [string range $tagstart 0 [expr $pntLoc -1] ]
    set endLoc [string range $tagstart [expr $pntLoc + 1] end]
    set endLoc [expr $endLoc + 9]
    set tagend [format %s\.%s $startLoc $endLoc]

    $w tag add redoTag$i $tagstart $tagend

    $w tag configure redoTag$i -background Snow2 \
	-relief raised -borderwidth 2 \
	-font $defaultFont
    
    # bind the tagged redo button for color changes on entry/exit and
    # redoing the particular search for this button
    $w tag bind redoTag$i <Enter> \
	"pRedoEnter $w $i"

    $w tag bind redoTag$i <Leave> \
	"pRedoLeave $w $i"

    $w tag bind redoTag$i <ButtonPress> \
	"pRedoSelect $w $i"

    # disable writing to history window again 
    $w configure -state disabled
}


# pRedoEnter -- Respond to cursor entry of redo button i

proc pRedoEnter {w i} {
    global currRedo

#    if {$i == $currRedo} return
    
    $w tag configure redoTag$i -background Snow3
}


# pRedoLeave -- Respond to cursor exit from redo button i

proc pRedoLeave {w i} {
    global currRedo

#    if {$i == $currRedo} return

    $w tag configure redoTag$i -background Snow2
}


# pRedoSelect -- Respond to click on redo button i

proc pRedoSelect {w i} {
    global currRedo

#    if {$i == $currRedo} return

    $w tag configure redoTag$currRedo -background Snow2 \
	-relief raised
    $w tag configure redoTag$i -background Snow4 -relief sunken
    
    set currRedo $i
    pHistSearch
}


# pHistoryBoolRedo

proc pHistoryBoolRedo {} {
    global currHistory textBG defaultBG defaultActiveBG
    global currRedo response index searchTerm indexList currHost
    global unusedButtonName dialogButton boolSwitch1 boolSwitch2 boolSwitch3
    # Grab selected command to redo
	    
    set redoCmd [lindex $currHistory [expr $currRedo-1]]

    # Reset index buttons
	    
    for {set i 0} {$i < 4} {incr i} {
	set index($i) "unused"
	set searchTerm($i) ""
	.search.button$i configure -text $unusedButtonName
    }
    
    # Clear search results
    
    pClearSearch
    
    # Set up search
    # Yes, this is incredibly nasty

    set i 0; set j 0; set x 0; set y 0; set z 0
    set boolMatrix(1,1) 0
    set boolMatrix(1,2) 0
    set boolMatrix(2,1) 0
    set boolMatrix(2,2) 0
    set matrixInd1 0
    set matrixInd2 0
    set boolterm1 ""
    set boolterm2 ""
    set boolterm3 ''
    set booltermNum 1
    set tempCmd2 ""
    set indexUsedList {}
    set curIndx 0
    
    # remove "zfind" and other extraneous elements from search history string
    # and in doing so, build up a representation of which index numbers and
    # boolSwitches to use in search
    set tempCmd1 "[lindex $redoCmd 1]"
    set tempCmd1 "[string trim $tempCmd1 \"]"
    set tempCmdLen "[string length $tempCmd1]"

    for {set k 0} {$k < $tempCmdLen} {incr k} {
	if {[string range "$tempCmd1" $k $k] == "("} {
	    incr matrixInd1
	} elseif {[string range "$tempCmd1" $k $k] == ")"} {
	    set matrixInd2 0
	} elseif {[string range "$tempCmd1" $k $k] == "\{"} {
	    incr matrixInd2
	    set boolMatrix($matrixInd1,$matrixInd2) 1
	    set tempCmd2 $tempCmd2[string range $tempCmd1 $k $k]
	} else {
	    set tempCmd2 $tempCmd2[string range $tempCmd1 $k $k]
	}
    }

    # set list of used indexes for this search
    for {set x 1} {$x < 3} {incr x} {
	for {set y 1} {$y < 3} {incr y} {
	    if {$boolMatrix($x,$y) == 1} {
		set indexUsedList [lappend indexUsedList $z]
	    }
	    incr z
	}
    }
    
    # set appropriate indexes and searchTerms to proper values, and record
    # boolean values for setting boolSwitches
    while {$i < [llength $tempCmd2]} {
	set ndx "[lindex $tempCmd2 $i]"; incr i
	set term "[lindex $tempCmd2 $i]"; incr i

	if {[llength $tempCmd2] > $i} {
	    if {[lindex $tempCmd2 $i] == "or"} {
		set boolterm$booltermNum "or"
	    } elseif {[lindex $tempCmd2 $i] == "and"} {
		set boolterm$booltermNum "and"
	    } 
	    incr booltermNum
	}
	foreach l $indexList($currHost) {
	    if {[string compare $ndx "[lindex $l 0]"] == 0} {
		set curIndx [lindex $indexUsedList $j]
		set index($curIndx) $ndx
		set searchTerm($curIndx) $term
		.search.button$curIndx configure -text [lindex $l 1]
		.search.entry$curIndx configure -textvariable searchTerm($curIndx) -state normal
		incr j
		break
	    }
	}
	incr i
    }
    
    # set boolSwitches properly for query
    # if only 1 term, don't touch boolSwitches
    # if indexUsedList is 0,2 or 0,3 or 1,2 or 1,3, only set middle boolSwitch
    # if indexUsedList is 0,1 set first boolSwitch
    # if indexUsedList is 2,3 set last boolSwitch
    # if indexUsedList is 0,1,2 or 0,1,3 set first boolSwitch, then second
    # if indexUsedList is 0,2,3 or 1,2,3, set second boolSwitch, then third
    # if indexUSedList is 0,1,2,3, set boolSwitch 1, then 2, then 3
    # ...and yes, there's probably a better way
    
    if {("$indexUsedList" == "0") || ("$indexUsedList" == "1") || ("$indexUsedList" == "2") || ("$indexUsedList" == "3")} {
	
    } elseif {("$indexUsedList" == "0 2") || ("$indexUsedList" == "0 3") || ("$indexUsedList" == "1 2") || ("$indexUsedList" == "1 3")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch2 "AND"
	} else {
	    set boolSwitch2 "OR"
	}
    } elseif {("$indexUsedList" == "0 1")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch1 "and"
	} else {
	    set boolSwitch1 "or"
	}
    } elseif {("$indexUsedList" == "2 3")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch3 "and"
	} else {
	    set boolSwitch3 "or"
	}
    } elseif {("$indexUsedList" == "0 1 2") || ("$indexUsedList" == "0 1 3")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch1 "and"
	} else {
	    set boolSwitch1 "or"
	}
	if {$boolterm2 == "and"} {
	    set boolSwitch2 "AND"
	} else {
	    set boolSwitch2 "OR"
	}
    } elseif {("$indexUsedList" == "0 2 3") || ("$indexUsedList" == "1 2 3")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch2 "AND"
	} else {
	    set boolSwitch2 "OR"
	}
	if {$boolterm2 == "and"} {
	    set boolSwitch3 "and"
	} else {
	    set boolSwitch3 "or"
	}
    } elseif {("$indexUsedList" == "0 1 2 3")} {
	if {$boolterm1 == "and"} {
	    set boolSwitch1 "and"
	} else {
	    set boolSwitch1 "or"
	}
	if {$boolterm2 == "and"} {
	    set boolSwitch2 "AND"
	} else {
	    set boolSwitch2 "OR"
	}
	if {$boolterm3 == "and"} {
	    set boolSwitch3 "and"
	} else {
	    set boolSwitch3 "or"
	}
    }
    # reactivate search button
    .search.bbar.search configure -state normal
    
}


# pHistoryProbRedo
proc pHistoryProbRedo {} {
    global currHistory textBG defaultBG defaultActiveBG
    global currRedo response index searchTerm indexList currHost
    global unusedButtonName dialogButton boolSwitch1 boolSwitch2 boolSwitch3
    global currSearch unusedProbButtonName probTypeList probSearchTerm
    global currProbType

    # Grab selected command to redo

    set redoCmd [lindex $currHistory [expr $currRedo-1]]

    # do some setup

    set currSearch "pr"
    set probTerm ""

    .search.probentry delete 1.0 end
    .search.probtype configure -text $unusedProbButtonName
    .search.probtype.menu invoke "Unused"

    # Clear Search Results

    pClearSearch

    # Figure out what type of probabilistic search if any
    if {([string first clusters $redoCmd] != -1) && ([string first resultsetid $redoCmd] != -1)} {
	set currProbType "cs"
    } elseif {[string first @ $redoCmd] != -1} {
	set currProbType "dr"
    } else {
	set currProbType "unused"
    }


    # Grab query portion of redoCmd

    set tempCmd "[lindex $redoCmd 1]"
    set tempCmd "[string trim $tempCmd \"]"
    set tempCmdLen "[string length $tempCmd]"

    # Find out if probabilistic component to search.  If so,
    # set Ranking Type button to direct and move search string
    # into .search.probentry.

    if {([string first @ $redoCmd] != -1) && ([string first topic $redoCmd] != -1)} {
	set first "[string first \{ $tempCmd]"
	set last "[string first \} $tempCmd]"
	set probTerm "[string range $tempCmd $first $last]"
	set probTerm "[string trim $probTerm \{\}]"
	if {$currProbType == "dr"} {
	    .search.probtype configure -text "By Record" -state normal
	    .search.probtype.menu activate "By Record"
	} elseif {$currProbType == "cs"} {
	    .search.probtype configure -text "By Topic Area" -state normal
	    .search.probtype.menu activate "By Topic Area"
	}
	.search.probentry configure -state normal
	.search.probentry delete 1.0 end
	.search.probentry insert end $probTerm
	set probSearchTerm $probTerm
    } else {
	set last [expr 0-2]
    }

    # disable boolean fields if this is a cluster search being redone
    for {set i 0} {$i < 2} {incr i} {
	if {$currProbType == "cs"} {
	    .search.button$i configure -state disabled
	} else {
	    .search.button$i configure -state normal
	}
	set index($i) "unused"
	set searchTerm($i) ""
	.search.button$i configure -text $unusedButtonName
    }
    
    # find out if boolean component to search.  If so, set indexes,
    # search entry fields, and search terms, as well as setting
    # boolean button

    set last "[expr $last+2]"
    if {[string index $tempCmd $last] == "\""} {
	return
    } else {

	if {$probTerm != ""} {
	    set last "[expr $last+5]"
	}
	set boolCmd "[string range $tempCmd $last end]"

	set boolCmd2 "[string trim $boolCmd \(\)]"
	set termCount "[llength $boolCmd2]"
	
	set i 0
	set j 0
	while {$i < $termCount} {
	    set ndx "[lindex $boolCmd2 $i]"; incr i
	    set term "[lindex $boolCmd2 $i]"; incr i

	    if {[llength $boolCmd2] > $i} {
		if {[lindex $boolCmd2 $i] == "or"} {
		    set boolSwitch1 "or"
		} elseif {[lindex $boolCmd2 $i] == "and"} {
		    set boolSwitch1 "and"
		}
	    }
	    foreach l $indexList($currHost) {
		if {[string compare $ndx "[lindex $l 0]"] == 0} {
		    set curIndx $j
		    set index($curIndx) $ndx
		    set searchTerm($curIndx) $term
		    .search.button$curIndx configure -text [lindex $l 1]
		    .search.entry$curIndx configure -textvariable searchTerm($curIndx) -state normal
		    incr j
		    break
		}
	    }
	    incr i
	}
    }
    # reactivate search button
    .search.bbar.search configure -state normal
}





# pClearHist

proc pClearHist {} {
    global currHistory currRedo HistSrch dialogButton

    # Confirm intent

    pDialog .h.d "Clear History" "Search History cannot be \
recovered once it has been cleared." \
	{warning} 0 {OK} {Cancel}

    if {$dialogButton != 0} return

    # Clear history

    set currHistory {}
    .search.bbar.history configure -state disabled

    #destroy display 
    destroy .h

    # Log
    pLogCmd 18
}



