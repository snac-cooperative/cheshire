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
	    return
	}
    }

    # set hostKLUDGE to the currHostName so that logging can record it.
    # note that we need this because of inexplicable weirdness in
    # accessing currHostName in logging C code.
    set hostKLUDGE $currHostName

    # Destroy index menus
    
    for {set i 0} {$i < 2} {incr i} {
	catch {destroy .search.button$i.menu}
	set index($i) "unused"
	set searchTerm($i) ""
	.search.button$i configure -text $unusedButtonName
    }

    # Save current search type and remove search type display
    
    set savedSearch $currSearch
    set savedSearchName $currSearchName
    set currSearch ""
    set currSearchName ""
    
    # Clear search results, close current session
    
    ClearTerms
    
    set lastHost $currHost
    set currHost ""
    set currHostName ""
    set currHostType $hostType

   .mbar.host configure -fg black
    set currHost "$newhost"
    set currHostName "$hostName"

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

    .search.bool1 configure -state normal
    .search.probentry configure -state normal

    pSearchButtonOn;			# Activate or disable buttons
    .search.bbar.search configure -state normal; #force it...

}




