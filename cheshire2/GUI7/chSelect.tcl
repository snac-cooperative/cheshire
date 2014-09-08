# chSelect.tcl -- Record select routines


# pSelectEnter -- Respond to cursor entry of select button i

proc pSelectEnter {w i} {
    global currSelectList buttonBG defaultBG

    if {[lsearch $currSelectList $i] < 0} {
	$w tag configure selectTag$i -background $buttonBG
    }
}



# pSelectLeave -- Respond to cursor leaving select button i

proc pSelectLeave {w i} {
    global currSelectList buttonBG defaultBG

    if {[lsearch $currSelectList $i] < 0} {
	$w tag configure selectTag$i -background $defaultBG
    }
}
# pSelect -- Respond to button press on select button i

proc pSelect {w i} {
    global currSelectList selectFormat selectFormatName
    global searchSet currFormat currFormatName SLi
    global buttonBG defaultBG

    # LOGGING ASSISTANCE
    set SLi $i


    # Find index in current save list

    set listPos [lsearch $currSelectList $i]
    set selectFormat $currFormat
    set selectFormatName $currFormatName

    # Add or delete index from list, and reconfigure button

    if {$listPos < 0} {
        $w tag configure selectTag$i -relief sunken \
            -background Snow4
        set currSelectList [lappend currSelectList $i]
	#LOG
	pLogCmd 19
    } else {
        $w tag configure selectTag$i -relief raised \
            -background $defaultBG
        set currSelectList [lreplace $currSelectList $listPos $listPos]
	#LOG
	pLogCmd 38
    }
}



# pHyperEnter -- Respond to cursor entry of hypertext

proc pHyperEnter {w hypert} {
    global HTFG HTActiveFG

	$w tag configure $hypert -foreground $HTActiveFG

}



# pHyperLeave -- Respond to cursor leaving hypertext

proc pHyperLeave {w hypert} {
    global HTFG

	$w tag configure $hypert -foreground $HTFG

}











