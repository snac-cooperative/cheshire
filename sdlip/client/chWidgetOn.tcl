#   chWidgetOn.tcl -- Routines that determine whether a particular
#     widget should be enable or disabled and act on that information


#   pSavedButtonOn {} -- Activates or deactivates "View Saved" button

proc pSavedButtonOn {} {
    global currSaveList permSaveList

    # Disallow multiple save windows

    if {[winfo exists .s] == 1} {
	.statusbar.top.bbar2.saved configure -state disabled
	return
    }

    # Disallow save window if nothing has been saved

    if {[llength $currSaveList] > 0 || [llength $permSaveList] > 0} {
	.statusbar.top.bbar2.saved configure -state normal
    } else {
	.statusbar.top.bbar2.saved configure -state disabled
    }
}

#   pSaveButtonOn {} -- Activates or deactivates "Save" Button

proc pSaveButtonOn {} {
    global nRetr

    if {$nRetr != 0} {
	.statusbar.top.bbar2.print configure -state normal
    } else {
	.statusbar.top.bbar2.print configure -state disabled
    }

}

#   pPrintButtonOn {} -- Activates or deactivates print button

proc pPrintButtonOn {} {
    global nRetr

    .statusbar.top.bbar2.print configure -state disabled

# The following is how this function really OUGHT to work.  However,
# for public use in AMS, printing is disabled.  Hence the above line
# of code.
#    if {$nRetr != 0} {
#	.statusbar.top.bbar2.print configure -state normal
#    } else {
#	.statusbar.top.bbar2.print configure -state disabled
#    }
}

#   pFeedbkButtonOn {} -- Activates or deactivates feedback button
#        which is labelled "More Like Selected"

proc pFeedbkButtonOn {} {
    global nRetr

    if {$nRetr != 0} {
	.statusbar.top.bbar2.feedback configure -state normal
    } else {
	.statusbar.top.bbar2.feedback configure -state disabled
    }
}

#   pMailButtonOn {} -- Activates or deactivates mail button

proc pMailButtonOn {} {
    global nRetr

    if {$nRetr != 0} {
	.statusbar.top.bbar2.mail configure -state normal
    } else {
	.statusbar.top.bbar2.mail configure -state disabled
    }
}

#   pSearchButtonOn {} -- Activates or deactivates search button

proc pSearchButtonOn {} {
    global currHost lastHost index currProbType currHistory currSearch
    global searchTerm

    set nIndices 0
    set probTerm ""
    set boolTerm ""
    set TERMS 0
    set numBTerm 2

    # check to see if any indexes are in use
    foreach i [array names index] {
	if {"$index($i)" != "unused"} {incr nIndices}
    }
    if {$currProbType != "unused"} {incr nIndices}

    # check to see if any entry fields are in use
    set probTerm [.search.probentry get 0.0 end]
    set probTerm [string trim $probTerm]
    if {$probTerm != ""} {
	set TERMS 1
    }

    for {set i 0} {$i < 2} {incr i} {
	set boolTerm "$searchTerm($i)"
	if {$boolTerm != ""} {
	    set TERMS 1
	}
    } 

    # set SEARCH button appropriately

    if {("$currHost" == "") ||
	(($nIndices == 0) && ($TERMS == 0))} {
	.search.bbar.search configure -state disabled
    } else {
	.search.bbar.search configure -state normal
    }
}



#   pSearchMenuOn {} -- Activates or deactivates search menu,
#     initializing search type to keyword

proc pSearchMenuOn {} {
    global currHost currSearch currSearchName searchList currHostType

    # Each host defaults to boolean keyword search

    if {"$currHost" == ""} {
	set currSearch "pr"
	set currSearchName "Ranked"
    } elseif {"$currSearch" == ""} {
	set currSearch [lindex [lindex $searchList 0] 0]
	set currSearchName [lindex [lindex $searchList 0] 1]
    }


    # Disable ranked retrieval for non-local servers

    if {($currHostType == "Ranked")} {
	.mbar.search.menu entryconfigure 1 -state normal
    } else {
	.mbar.search.menu entryconfigure 1 -state disabled
    }

    # Disable search type selection where no choice

    if {"$currHost" == ""} {
	.mbar.search configure -state disabled
    } else {
	.mbar.search configure -state normal
    }
}


# pActionLabelOn {s switch} -- Announce action s if switch is 1,
#   remove announcement otherwise
#   This is for the Indictor blinkenlight on the statusbar.

proc pActionLabelOn {s switch flash} {
    global defaultBG oldFocus

    if {$switch == 0} {
	.statusbar.bottom.b.actionLabel configure -bg $defaultBG \
	    -foreground $defaultBG \
	    -disabledforeground $defaultBG \
	    -text "Inactive" -state disabled
	grab release .statusbar.bottom.b.actionLabel
	bind . <KeyPress-Up> {.retrieve.text yview scroll -1 units}
	bind . <KeyPress-Down> {.retrieve.text yview scroll 1 units}
	bind . <KeyPress-Prior> {.retrieve.text yview scroll -1 pages}
	bind . <KeyPress-Next> {.retrieve.text yview scroll 1 pages}
	bind . <KeyRelease-Up> {pMoreButtonCmd $currPacketSize}
	bind . <KeyRelease-Down> {pMoreButtonCmd $currPacketSize}
	bind . <KeyRelease-Prior> {pMoreButtonCmd $currPacketSize}
	bind . <KeyRelease-Next> {pMoreButtonCmd $currPacketSize}
	. config -cursor top_left_arrow
	update
    } else {
	bind . <KeyPress-Up> {}
	bind . <KeyPress-Down> {}
	bind . <KeyPress-Prior> {}
	bind . <KeyPress-Next> {}
	bind . <KeyRelease-Up> {}
	bind . <KeyRelease-Down> {}
	bind . <KeyRelease-Prior> {}
	bind . <KeyRelease-Next> {}
	grab set .statusbar.bottom.b.actionLabel
	. config -cursor watch
	.statusbar.bottom.b.actionLabel configure -foreground black \
	    -disabledforeground black \
	    -bg LightSalmon1 -text $s -state normal
	update
	if {"$flash" == "flash"} {
	    .statusbar.bottom.b.actionLabel flash
	    .statusbar.bottom.b.actionLabel flash
	}
	.statusbar.bottom.b.actionLabel configure -state disabled
    }
}

