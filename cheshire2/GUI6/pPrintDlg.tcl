# pPrintDlg


proc pPrintDlg {} {
    global entryBG defaultBG defaultActiveBG dialogButton
    global currSelectList selection

# if no selected records, just print current set using pPrintCmd
# otherwise, check if user wants selected or all, then invoke pPrintCmd

    if {[llength $currSelectList] == 0} {
	set selection "allrecs"
	pPrintCmd main
    } else {
	pDialog .d "Print Selection" "Would you like to print all \
of the current records, or only those you've selected?" \
	    {question} 0 {All Records} {Selected Records} {Cancel}

	if {$dialogButton == 0} {
	    set selection "allrecs"
	    pPrintCmd main
	} elseif {$dialogButton == 1} {
	    set selection "selectrecs"
	    pPrintCmd main
	} else {
	    #LOG
	    pLogCmd 28
	}
    }
}
















