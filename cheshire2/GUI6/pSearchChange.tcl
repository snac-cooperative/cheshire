# pSearchChange.tcl -- invokes change from boolean to probabilistic or 
#                      vice versa

proc pSearchChange {newHost} {
    global currSearch currSearchName currHost searchTerm probSearchTerm
    global indexList index unusedButtonName searchList defaultFont
    global probTypeList unusedProbButtonName selectFG csStage

    # if user tries to change interface to what it's already set for,
    # bail out without doing anything
    foreach l $searchList {
	if {$currSearch == [lindex $l 0]} {
	    if {$currSearchName == [lindex $l 1]} {
		return
	    }
	}
    }

    #record the current placement of the user interface on the screen
    set wgeo [wm geometry .]
    set wgeo [split $wgeo x+-]
    set xdist [lindex $wgeo 2]
    set ydist [lindex $wgeo 3]

    # if we're changing to a boolean interface...
    if {$currSearch == "bl"} {
	#update the search interface name
	set currSearchName [lindex [lindex $searchList 0] 1]
	# do the actual change to a boolean interface
	pSearchFaceB $xdist $ydist
	# reset some Z39.50 settings
	set csStage 0
	set err [catch {zset sResultSetName Default} msg]
	if {$err != 0} {
	    pDialog .d {Z39.50 ResultSetName mis-set!} $msg {error} 0 OK
	}
	set err [catch {zset pResultSetId Default} msg]
	if {$err != 0} {
	    pDialog .d {Z39.50 ResultSetId mis-set!} $msg {error} 0 OK
	}
	if {$newHost == 0} {

	    # create search boolean index button menus

	    for {set i 0} {$i<4} {incr i} {
		menu .search.button$i.menu -selectcolor $selectFG -tearoff 0
		foreach l $indexList($currHost) {
		    .search.button$i.menu add radiobutton -label [lindex $l 1] \
			-font $defaultFont \
			-variable index($i) -value [lindex $l 0] \
			-command "if {\"[lindex $l 0]\" == \"unused\"} { \
                        .search.button$i configure -text $unusedButtonName \
                      } else { \
                        .search.button$i configure -text \"[lindex $l 1]\" \
                      }; \
                      .search.entry$i configure -textvariable searchTerm($i); \
                      set searchTerm($i) \"\"; \
                      if {\"[lindex $l 0]\" == \"unused\"} { \
                        .search.entry$i configure -state disabled \
                      } else { \
                        .search.entry$i configure -state normal \
                      }; \
                      pSearchButtonOn"
		}
		.search.button$i configure -state normal
	    }
	    # log the interface change
	    pLogCmd 1
	}
    } else {
	# else if we're switching to probabilistic interface,
	# update the search interface name
	set currSearchName [lindex [lindex $searchList 1] 1]
	# create the new search interface
	pSearchFaceP $xdist $ydist
	if {$newHost == 0} {


	    # create search index and prob search index button menus

	    menu .search.probtype.menu -selectcolor $selectFG -tearoff 0
	    foreach m $probTypeList {
		.search.probtype.menu add radiobutton -label [lindex $m 1] \
		    -font $defaultFont \
		    -variable currProbType -value [lindex $m 0] \
		    -command "set currProbTypeName \"[lindex $m 1]\"; \
                        .search.probtype configure -text \"[lindex $m 1]\"; \
                        if {\"[lindex $m 0]\" == \"unused\"} { \
                           .search.probtype configure -text $unusedProbButtonName; \
                           .search.probentry delete 1.0 end; \
                           .search.probentry configure -state disabled; \
                           .search.button0 configure -state normal; \
                           .search.button1 configure -state normal \
                        } else { \
                           .search.probentry configure -state normal; \
                           if {\"[lindex $m 0]\" == \"cs\"} { \
                                ClearTerms; \
                                .search.entry0 configure -state disabled; \
                                .search.entry1 configure -state disabled; \
                                .search.button0.menu invoke \"Unused\"; \
                                .search.button1.menu invoke \"Unused\"; \
                                .search.button0 configure -text $unusedButtonName;\
                                .search.button1 configure -text $unusedButtonName; \
                                .search.button0 configure -state disabled; \
                                .search.button1 configure -state disabled; \
                                focus .search.probentry \
                           } else { \
                                .search.button0 configure -state normal; \
                                .search.button1 configure -state normal; \
                           } \
                        }
                        pSearchButtonOn"
	    }
	    
	    .search.probtype configure -state normal
	    for {set i 0} {$i<2} {incr i} {
		menu .search.button$i.menu -selectcolor $selectFG -tearoff 0
		foreach l $indexList($currHost) {
		    .search.button$i.menu add radiobutton -label [lindex $l 1] \
			-font $defaultFont \
			-variable index($i) -value [lindex $l 0] \
			-command "if {\"[lindex $l 0]\" == \"unused\"} { \
                        .search.button$i configure -text $unusedButtonName \
                      } else { \
                        .search.button$i configure -text \"[lindex $l 1]\" \
                      }; \
                      .search.entry$i configure -textvariable searchTerm($i); \
                      set searchTerm($i) \"\"; \
                      if {\"[lindex $l 0]\" == \"unused\"} { \
                        .search.entry$i configure -state disabled \
                      } else { \
                        .search.entry$i configure -state normal \
                      }; \
                      pSearchButtonOn"
		}
		.search.button$i configure -state normal
	    }
	    # log the interface change
	    pLogCmd 1
	}
    }

}

