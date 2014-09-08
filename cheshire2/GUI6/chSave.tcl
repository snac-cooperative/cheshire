# chSave.tcl -- Record save routines

# pSaveButtonCmd -- Save records

proc pSaveButtonCmd {} {
    global entryBG defaultBG defaultActiveBG dialogButton
    global currSelectList selection buttonBG rectype 

# if no selected records, save all of current set
# otherwise, check if user wants selected or all
# If user gets dialog box and cancels request, log that transaction
    if {[llength $currSelectList] == 0} {
	set selection "allrecs"
	pSaveCmd
    } else {
	pDialog .d "Save Selection" "Would you like to save all \
of the current records, or only those you've selected?" \
	    {question} 0 {All Records} {Selected Records} {Cancel}

	if {$dialogButton == 0} {
	    set selection "allrecs"
	    pSaveCmd
	} elseif {$dialogButton == 1} {
	    set selection "selectrecs"
	    pSaveCmd
	} else {
	    #LOG
	    pLogCmd 31
	}
    }
}

# pSaveCmd -- Save all or selected records from current list and add
#             to permanent saved collection

proc pSaveCmd {} {
    global entryBG defaultBG defaultActiveBG dialogButton
    global currSelectList selection nRetr
    global currSaveList saveIndexList saveFormat searchSet permSaveList
    global fname rectype tmpdir SLi

    if {"$selection" == "allrecs"} {
	for {set i 1} {$i <= $nRetr} {incr i} {

	    # Find index in current save list
	    set listPos [lsearch $currSaveList $i]

	    # Add index to list, and update save window if necessary
	    if {$listPos < 0} {

		#LOGGING ASSISTANCE
		set SLi $i

		set currSaveList [lappend currSaveList $i]
		if {[winfo exists .s] == 1} {
		    pAppendROText .s.disp.text "\n%s\n" \
			[zformat $saveFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		    lappend saveIndexList [.s.disp.text index end]
		}	    

		# LOG
		pLogCmd 22
	    }
	}
    } else {
	# for selected records
	foreach i $currSelectList {
	    # Find index in current save list
	    set listPos [lsearch $currSaveList $i]

	    # Add index to list, and update save window if necessary
	    if {$listPos < 0} {

		#LOGGING ASSISTANCE
		set SLi $i

		set currSaveList [lappend currSaveList $i]
		if {[winfo exists .s] == 1} {
		    pAppendROText .s.disp.text "\n%s\n" \
			[zformat $saveFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		    lappend saveIndexList [.s.disp.text index end]
		}

		# LOG
		pLogCmd 22
	    }    
	}
    }

    # Reconfigure "Saved" button as appropriate
    pSavedButtonOn
}



# pMakeSaveToplevel -- Create display for saved records

proc pMakeSaveToplevel {} {
    global currFormatName saveFormatName entryBG formatList
    global currFormat saveFormat defaultBG defaultActiveBG selectFG
    global buttonBG entryFont defaultFont
 
    # Initialize variables

    set saveFormatName $currFormatName
    set saveFormat $currFormat

    # Geometry, etc.

    set location "center";		# Location of window
    set saveTextBG AntiqueWhite1;	# Text widget background

    toplevel .s -class Saved
    wm title .s "Saved Records"
    wm iconname .s "Saved"

    scan [wm geometry .] "%dx%d+%d+%d" width height x y
    if {"$location" == "center"} {
	wm geometry .s +[expr ($width-$x)/4]+[expr ($height-$y)/2]
    } elseif {"$location" == "nw"} {
	wm geometry .s +$x+$y
    }

    # Framework

    frame .s.bbar -relief raised -bd 1;	# Background for button bar
    frame .s.disp -relief raised -bd 1; # Background for display
    pack .s.bbar .s.disp -side top -fill both


    # Button bar

    button .s.bbar.iconify -relief raised -bd 2 -text "Iconify" \
	-command "wm iconify .s; pLogCmd 24" -highlightbackground $defaultBG \
	-font $defaultFont
    button .s.bbar.print -relief raised -bd 2 -text "Print" \
	-state disabled -highlightbackground $defaultBG \
	-font $defaultFont
    button .s.bbar.mail -relief raised -bd 2 -text "Mail" \
	-command "pMailButtonCmd .s.m save" -highlightbackground $defaultBG \
	-font $defaultFont
    button .s.bbar.clear -relief raised -bd 2 -text "Clear" \
	-command pClearSave -highlightbackground $defaultBG \
	-font $defaultFont
    button .s.bbar.exit -relief raised -bd 2 -text "Exit" \
	-command "destroy .s; pSavedButtonOn; pLogCmd 27" \
	-highlightbackground $defaultBG \
	-font $defaultFont

    
    pack .s.bbar.iconify .s.bbar.print .s.bbar.mail .s.bbar.clear \
	.s.bbar.exit \
	-side left -pady 1 -ipadx 1m -ipady 1m -fill x -expand 1

    # Display window

    text .s.disp.text -relief sunken -bd 1 \
	-background $saveTextBG \
	-highlightbackground $defaultBG \
	-selectbackground $saveTextBG \
	-selectborderwidth 0 \
	-font $entryFont \
	-cursor {} \
	-wrap word \
	-yscrollcommand ".s.disp.scroll set";	# Text window

    scrollbar .s.disp.scroll \
	-background $defaultBG \
	-highlightbackground $defaultBG \
	-activebackground $defaultActiveBG \
	-relief sunken \
	-troughcolor $buttonBG \
	-command ".s.disp.text yview";		# Scroll bar

    pack .s.disp.scroll -side right -fill y -padx 2 -pady 2
    pack .s.disp.text -side left -padx 2 -pady 2 -fill x -expand 1
}


# pViewSaveButtonCmd -- Display saved records, making display if necessary

proc pViewSaveButtonCmd {} {
    global currSaveList permSaveList nRetr searchSet saveFormat
    global saveIndexList fname rectype tmpdir

    # Return if nothing saved

    if {([llength $currSaveList] == 0 && \
		 [llength $permSaveList] == 0)} return

    # Make display if necessary

    if {[winfo exists .s] == 0} {
	pMakeSaveToplevel
    }
    pSavedButtonOn

    # Clear existing display

    pClearROText .s.disp.text
    set saveIndexList {}

    # Display saved records

    pActionLabelOn "Formatting" 1 flash

    foreach recSet $permSaveList {
	set temprec [lindex $recSet 0]
	set temprectype [lindex $recSet 1]
	pAppendROText .s.disp.text "\n%s\n" \
	    [zformat $saveFormat $temprec $temprectype -1 80 $tmpdir$fname]
    }

    lappend saveIndexList [.s.disp.text index end]
    foreach ndx $currSaveList {
	pAppendROText .s.disp.text "\n%s\n" \
	    [zformat $saveFormat $searchSet($ndx) $rectype -1 80 $tmpdir$fname]
	lappend saveIndexList [.s.disp.text index end]
    }

    pActionLabelOn "Inactive" 0 noflash

    #LOG
    pLogCmd 23
}
	    


# pClearSave

proc pClearSave {} {
    global currSaveList permSaveList saveIndexList dialogButton

    # Confirm intent

    pDialog .s.d "Clear Saved Records" "Saved records cannot be \
recovered once they have been cleared." \
	{warning} 0 {OK} {Cancel}

    if {$dialogButton != 0} return

    # Clear display of saved records and record indexes

    pClearROText .s.disp.text
    set saveIndexList {1.0}
    
    # Zero out save lists and destroy window

    set currSaveList {}
    set permSaveList {}
    destroy .s

    pSavedButtonOn

    #LOG
    pLogCmd 26
}





