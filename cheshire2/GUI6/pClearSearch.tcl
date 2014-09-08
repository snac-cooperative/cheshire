# pClearSearch {} -- Clear search results from search window, unset
#   all entries in searchSet array, zero out nHits and nRetr, and
#   set buttons and displays appropriately

proc pClearSearch {} {
    global searchSet nHits nRetr disabledFG defaultBG currSearch
    global currSaveList permSaveList saveIndexList currSelectList
    global rectype csStage

    # Xfer saves from current search to permanent list,
    # and update the Saved window if it's around

    foreach i $currSaveList {
	set permSaveList [lappend permSaveList [list $searchSet($i) $rectype]]
    }
    set currSaveList {}
    set saveIndexList {}

    if {[winfo exists .s] == 1} {
	lappend saveIndexList [.s.disp.text index end]
    }

    # Zero out current search set
    if {[info exists searchSet]} {
	set sSetSize [array size searchSet]
	for {set i 1} {$i <= $sSetSize} {incr i} {
	    unset searchSet($i)
	}
    }


    # Delete tags and clear record display screen

    pClearROText .retrieve.text


    # Reset search globals

    set nHits 0
    set nRetr 0
    set currSelectList {}

    # Cluster search initializations

    set csStage 0
    .mbar.format configure -state normal

    # Disable mail, print and save

    .statusbar.top.bbar2.mail configure -state disabled
    .statusbar.top.bbar2.print configure -state disabled
    if {$currSearch == "pr"} {
	.statusbar.top.bbar2.feedback configure -state disabled
    }
    .statusbar.top.bbar2.save configure -state disabled

    # Adjust retrieval scale

    .statusbar.bottom.a.hitsLabel configure -foreground $defaultBG
    .statusbar.bottom.a.scale configure -foreground $disabledFG \
	-state normal
    .statusbar.bottom.a.scale set 0
    .statusbar.bottom.a.scale configure -state disabled
    update
}

