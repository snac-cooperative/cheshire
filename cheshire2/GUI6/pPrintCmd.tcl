# pPrintCmd


proc pPrintCmd {type} {
    global entryBG defaultBG defaultActiveBG
    global nRetr searchSet currFormat currSelectList
    global currSaveList permSaveList saveFormat selection
    global fname rectype tmpdir tmpfname

    # flash indicator light

    pActionLabelOn "Printing" 1 flash
    
    # Build message in temporary file using call to tempnam in C
    
    set tmpfname [catch {pTmpNam $tmpdir} msg]
    set f [open $tmpfname w]
    
    # Message header
    
    puts $f ""; puts $f "-----<SEARCH RESULTS>-----"
    
    # Records
    
    switch $type {
	main {
	    if {("$selection" == "allrecs")} {
		# Print all of currently displayed records to file or...
		for {set i 1} {$i <= $nRetr} {incr i} {
		    puts $f [format "\n%d.\n" $i]
		    puts $f [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		}
	    } else {
		# Print all currently selected records to file or ...
		foreach i $currSelectList {
		    puts $f [format "\n%d.\n" $i]
		    puts $f [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		}
	    }
	}
	save {
	    # print all records in permanent and current save list to file
	    set i 1
	    foreach recSet $permSaveList {
		set temprec [lindex $recSet 0]
		set temprectype [lindex $recSet 1]
		puts $f [format "\n%d.\n" $i]
		puts $f [zformat $saveFormat $temprec $temprectype -1 80 $tmpdir$fname]
		incr i
	    }
	    foreach ndx $currSaveList {
		puts $f [format "\n%d.\n" $i]
		    puts $f [zformat $saveFormat $searchSet($ndx) $rectype -1 80 $tmpdir$fname]
		incr i
	    }
	}
    }
    
    # close the file pointer and then and then print the file using lpr
    # remove the temporary file when done

    close $f
    
    exec <$tmpfname lpr
    pLogCmd 20
    exec rm $tmpfname
    
    pActionLabelOn "Inactive" 0 noflash
}









