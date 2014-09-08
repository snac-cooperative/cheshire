# pZselect -- Connect with Z39.50 host specified by newhost

proc pZselect {newhost} {
    global hostList currHost currHostName dialogButton searchSet nRetr
    global indexList index searchTerm unusedButtonName lastHost
    global currSearch currSearchName searchList
    global probTypeList unusedProbButtonName probSearchTerm
    global currProbType currProbTypeName selectFG currFormat hostKLUDGE
    global ERROR defaultFont currHostType currClassClusDB

    # set probabilistic search type and term to null state
    set currProbType "unused"
    set probType ""
    set probSearchTerm ""

    # get the button label for the new host
    foreach l $hostList {
	if {"[lindex $l 0]" == "$newhost"} {
	    set hostName "[lindex $l 1]"
	    set hostType "[lindex $l 2]"
	    set currClassClusDB "[lindex $l 3]"
	    break
	}
    }
    
    # Get OK from user to disconnect from current host and clear results
    if {$nRetr > 0} {
	pDialog .d $hostName "Accessing a new server will\
end the current connection and clear the current search results." \
	    {warning} 0 {OK} {Cancel}
	
	if {$dialogButton != 0} {
	    foreach l $hostList {
		if {"[lindex $l 0]" == "$currHost"} {
		    set currHostName "[lindex $l 1]"
		    set currHostType "[lindex $l 2]"
		    set currClassClusDB "[lindex $l 3]"
		    break
		}
	    }
	    pLogCmd 32
	    return
	}
    }

    # set hostKLUDGE to the currHostName so that logging can record it.
    # note that we need this because of inexplicable weirdness in
    # accessing currHostName in logging C code.
    set hostKLUDGE $currHostName

    pLogCmd 0

    # Destroy index menus
    
    if {$currSearch == "bl"} {
	for {set i 0} {$i < 4} {incr i} {
	    catch {destroy .search.button$i.menu}
	    set index($i) "unused"
	    set searchTerm($i) ""
	    .search.button$i configure -text $unusedButtonName \
		-state disabled
	    .search.entry$i configure -state disabled
	}
    } elseif {$currSearch == "pr"} {
	catch {destroy .search.probtype.menu}
	set probType "unused"
	set probTypeName "Unused"
	set probSearchTerm ""
	.search.probtype configure -text $unusedProbButtonName \
	    -state disabled
	.search.probentry configure -state disabled
	
	for {set i 0} {$i < 2} {incr i} {
	    catch {destroy .search.button$i.menu}
	    set index($i) "unused"
	    set searchTerm($i) ""
	    .search.button$i configure -text $unusedButtonName \
		-state disabled
	    .search.entry$i configure -state disabled
	}
    }

    # Save current search type and remove search type display
    
    set savedSearch $currSearch
    set savedSearchName $currSearchName
    set currSearch ""
    set currSearchName ""
    .mbar.search configure -state disabled
    
    # Clear search results, close current session
    
    pClearSearch
    
    zclose
    set lastHost $currHost
    set currHost ""
    set currHostName ""
    set currHostType $hostType

    # reset Maximum record size and preferred message size for 
    # dealing with Chshire server, so we can get DTD's without problems.
    if {$currHostType == "Ranked"} {
	zset iMaxRecSize 70000
	zset iPreferredMessageSize 70000
    }

    # Attempt new connection
    pActionLabelOn "Connecting" 1 flash

    if {($newhost == "RLIN") || ($newhost == "AUT") || ($newhost == "FLP")} {
	# These are RLG databases that need a password
	set authentication "Y6.E93/C72JACIN"
	set err [catch {zselect $newhost $authentication} msg]
    } else {
	set err [catch {zselect $newhost} msg]
    }
    pActionLabelOn "Inactive" 0 noflash
    
    # if there's no problem connecting, configure interface and
    # set client to request SGML if we're dealing with cheshire server
    # or with Univ. of Liverpool
    if {$err == 0} {
	.mbar.host configure -fg black
	set currHost "$newhost"
	set currHostName "$hostName"

	set err [catch {zset NumberOfRecordsRequested 0} msg]
	if {$err != 0} {
	    pDialog .d {Number of records settings failed.!} $msg {error} 0 OK
	}


	if {$currHostType == "Ranked"} {
	    set err [catch {zset recsyntax sutrs} msg]
	    if {$err != 0} {
		pDialog .d {Record Syntax mis-set!} $msg {error} 0 OK
	    }
	} else {
	    set err [catch {zset recsyntax MARC} msg]
	    if {$err != 0} {
		pDialog .d {Record Syntax mis-set!} $msg {error} 0 OK
	    }
	    set err [catch {zset sResultSetName Default} msg]
	    if {$err != 0} {
		pDialog .d {Z39.50 ResultSetName mis-set!} $msg {error} 0 OK
	    }
	    set err [catch {zset pResultSetId Default} msg]
	    if {$err != 0} {
		pDialog .d {Z39.50 ResultSetId mis-set!} $msg {error} 0 OK
	    }
	}

	if {($savedSearch == "pr")  && ($currHostType != "Ranked")} { 
	    set currSearch "bl"
	    pSearchChange 1;
	} elseif {($savedSearch == "bl") && ($currHostType == "Ranked")} {
	    set currSearch "pr"
	    pSearchChange 1;
	} else {
	    set currSearch $savedSearch
	    set currSearchName $savedSearchName
	}

	# Create menus for index selection buttons.
	
	if {$currSearch == "bl"} {
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

	} else {
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
	}
	# Configure interface so people can't fuck up using TREC
	# database
	if {$currHost == "trec"} {
	    .mbar.format.menu invoke 1
	    .mbar.format.menu entryconfigure end -state disabled
	    .search.probtype.menu entryconfigure 1 -state disabled
	} elseif {$currSearch == "pr"} {
	    .mbar.format.menu entryconfigure end -state normal
	    .search.probtype.menu entryconfigure 1 -state normal
	}
	pLogCmd 35

    } else {
	# if we *don't* connect, tell user, log problem, and reset
	# client interface to initial configuration
	pDialog .d {Connect Failure!} $msg {error} 0 OK
	set ERROR $msg
	pLogCmd 37
	set currHost ""
	set currHostName "START"
	.mbar.host configure -fg red
	set currSearch "pr"
	set currSearchName "Ranked"
	set currProbType "unused"
	set currProbTypeName "Unused"
	set probSearchTerm ""
	pSearchChange 1
	pResetButtonCmd
	return
    }
    
    pSearchButtonOn;			# Activate or disable buttons
    pSearchMenuOn
    
}




