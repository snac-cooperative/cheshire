# pResetButtonCmd -- Close and clean up current session, returning
#   system to initial configuration


proc pResetButtonCmd {} {
    global formatList currHost currHostName index searchTerm
    global currFormat currFormatName currPacketSize selectFG
    global defaultPacketSize unusedButtonName currHistory
    global permSaveList currSearch currSearchName probSearchTerm
    global currProbType currProbTypeName unusedProbButtonName userid


    # Clear last search results, close current session

    pClearSearch

    zclose
    set currHost ""
    set currHostName "START"
    .mbar.host configure -fg red
    if {$currSearch == "bl"} {
	set currSearch "pr"
	pSearchChange 1
    }
    set currSearch "pr"
    set currSearchName "Ranked"
    set currProbType "unused"
    set currProbTypeName "Unused"
    set probSearchTerm ""

    for {set i 0} {$i < 2} {incr i} {
	catch {destroy .search.button$i.menu}
	set index($i) "unused"
	set searchTerm($i) ""
	.search.button$i configure -text $unusedButtonName \
	    -state disabled
	.search.entry$i configure -state disabled
    }
    .search.probentry delete 1.0 end
    .search.probentry configure -state disabled
    .search.probtype configure -text $unusedProbButtonName
    .search.probtype configure -state disabled


    # Reset format to default

    set currFormat [lindex [lindex $formatList 1] 0]
    set currFormatName [lindex [lindex $formatList 1] 1]

    # Reset packet size to default

    set currPacketSize $defaultPacketSize

    # Disable search menu and button

    pSearchMenuOn
    pSearchButtonOn

    # Clear history

    set currHistory {}
    .search.bbar.history configure -state disabled

    # Clear permanent saves (current cleared in pClearSearch),
    # destroy save window, and disable save, print & Mail buttons

    set permSaveList {}
    catch {destroy .s}
    pSaveButtonOn
    pSavedButtonOn
    pPrintButtonOn
    pMailButtonOn
    pFeedbkButtonOn

    # Log
    pLogCmd 3

    # XXX problem with incrementing userid here is what if user
    # resets session and keeps working?  Oh well.  
    set userid [expr $userid + 1]

}














