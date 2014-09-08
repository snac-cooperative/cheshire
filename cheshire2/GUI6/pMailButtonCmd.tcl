# pMailButtonCmd


proc pMailButtonCmd {w type} {
    global entryBG defaultBG defaultActiveBG buttonBG
    global mailAddress mailSubject response dialogButton
    global nRetr searchSet currFormat currSelectList
    global currSaveList permSaveList saveFormat selection
    global fname rectype selectFG tmpdir tmpfname ERROR defaultFont entryFont

    # Build the mailing window
    set location "center";		 # Location of window
    set mailTextBG AntiqueWhite2;# Text widget background

    set mailAddress "";		 # Initialize variables
    set mailSubject ""

    toplevel $w -class Mail
    wm title $w "Mail Search Results"
    wm iconname $w Mail

    set lastdot [string last . $w]

    if {$lastdot == 0} {
	set parent "."
    } else {
	set parent [string range $w 0 [expr $lastdot-1]]
    }

    scan [wm geometry $parent] "%dx%d+%d+%d" width height x y
    if {"$location" == "center"} {
	wm geometry $w +[expr ($width-$x)/4]+[expr ($height-$y)/2]
    } elseif {"$location" == "nw"} {
	wm geometry $w +$x+$y
    }

    # Framework

    frame $w.entr -relief raised -bd 1; # Background for adddress and
					#   subject entries
    frame $w.sel -relief raised -bd 1;  # Background for radio button choice
                                        #   of mailing all or selected records
    frame $w.comm -relief raised -bd 1; # Background for comments
    frame $w.comm.top -relief flat;	# Background for "Comments" label
    frame $w.comm.bottom -relief flat;	# Background for comments entry
    frame $w.bbar -relief raised -bd 1;	# Background for button bar

    pack $w.entr $w.sel $w.comm $w.bbar -side top -fill both
    pack $w.comm.top $w.comm.bottom -side top -fill both

    # Address and subject entry windows

    label $w.entr.addrLabel -relief flat -text "Address" -font $defaultFont
    entry $w.entr.addrEntry -relief sunken -background $entryBG \
	-width 25 -textvariable mailAddress -highlightbackground $defaultBG \
	-highlightcolor black -font $entryFont

    label $w.entr.subjLabel -relief flat -text "Subject" -font $defaultFont
    entry $w.entr.subjEntry -relief sunken -background $entryBG \
	-width 35 -textvariable mailSubject -highlightbackground $defaultBG \
	-highlightcolor black -font $entryFont

    pack $w.entr.addrLabel -side left -padx 1m -pady 1m
    pack $w.entr.addrEntry -side left -pady 1m
    pack $w.entr.subjEntry -side right -padx 1m -pady 1m
    pack $w.entr.subjLabel -side right -pady 1m

    # Radio buttons for choosing to mail all records or selected records

    radiobutton $w.sel.all -text "Mail All Current Records" -variable \
	selection -relief flat -value allrecs -selectcolor $selectFG \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    radiobutton $w.sel.select -text "Mail Selected Current Records" \
	-variable selection -relief flat -value selectrecs \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-selectcolor $selectFG -font $defaultFont
    $w.sel.all select
    pack $w.sel.all $w.sel.select -side top -pady 2 -padx 20 -anchor w

    # Comments window

    label $w.comm.label -text "Enter Any Attached Message Below" \
	-font $defaultFont

    text $w.comm.text -relief sunken -bd 1 \
	-height 10 \
	-width  75 \
	-background $mailTextBG \
	-highlightbackground $defaultBG \
	-selectbackground $mailTextBG \
	-highlightcolor black \
	-selectborderwidth 0 \
	-font $entryFont \
	-cursor {} \
	-yscrollcommand "$w.comm.scroll set";	# Text window

    scrollbar $w.comm.scroll \
	-background $defaultBG \
	-activebackground $defaultActiveBG \
	-highlightbackground $defaultBG \
	-highlightcolor $defaultBG \
	-relief sunken \
	-troughcolor $buttonBG \
	-command "$w.comm.text yview";		# Scroll bar

    # Button bar

    button $w.bbar.mail -relief raised -bd 2 -text "Mail" \
	-command "set response 0" -highlightbackground $defaultBG \
	-font $defaultFont
    button $w.bbar.exit -relief raised -bd 2 -text "Cancel" \
	-command "set response 1" -highlightbackground $defaultBG \
	-font $defaultFont
    
    pack $w.bbar.mail $w.bbar.exit \
	-side left -pady 1 -ipadx 1m -ipady 1m -fill x -expand 1


    pack $w.comm.label -in $w.comm.top -side top \
	-fill both
    pack $w.comm.scroll -in $w.comm.bottom \
	-side right -fill y -padx 1m -pady 1m
    pack $w.comm.text -in $w.comm.bottom \
	-side left -padx 1m -pady 1m

    # Take control of display

    set oldFocus [focus]
    grab set $w
    focus $w

    tkwait variable response

    while {$response == 0} {
	# Check for properly formatted address
	if {[regexp {.@.} $mailAddress] == 1} {
	    break
	} else {
	    # OK, the test for string@string is bogus.  Our test
	    # for a legitimate e-mail address should probably include
	    # a ping attempt or something to confirm at least the
	    # machine address, and preferably the user ID as well,
	    # and ask for a confirmation from the user if we can't confirm
	    # independently.  For the moment, we just tell user to
	    # fix things if we don't find the string@string combo, and
	    # log the error.
	    pDialog $w.d "Address Error" \
		"Please enter an e-mail address of the form user@host." \
		{error} 0 {OK}
	    set ERROR "User forgot to enter e-mail address when mailing records."
	    pLogCmd 37
	    unset response
	    tkwait variable response
	}
    }

    # if the user really asks us to mail something
    if {$response == 0} {

	# check to make sure user isn't trying to do selected
	# records with nothing selected
	if {("$selection" == "selectrecs") && (("$type" == "save") || ([llength $currSelectList] == 0))} {
	    pDialog $w.d "Selection Error" \
		"You asked to e-mail selected records when either: 1. No records were selected in the main window, or 2. you were mailing saved records.  It's not possible to e-mail selected records under either of those two conditions.  Would you like me to e-mail all of the records, or cancel the e-mail?" \
		{question} 0 {Mail All Records} {Cancel}
	    if {$dialogButton == 0} {
		set selection "allrecs"
	    } else {
		# log e-mail cancelled and return
		pLogCmd 29
		destroy $w
		update
		focus $oldFocus
		return
	    }
	}

	# flash user blinkenlight
	pActionLabelOn "Mailing" 1 flash

	# Build message in temporary file using call to tempnam in C

	set tmpfname [catch {pTmpNam $tmpdir} msg]
	set f [open $tmpfname w]

	# Message header

	if {[string length $mailSubject] > 0} {
	    puts $f "Subject: $mailSubject"
	    puts $f ""
	}
	puts $f [$w.comm.text get 1.0 end]
	puts $f ""; puts $f "-----<SEARCH RESULTS>-----"

	# Records

	switch $type {
	    main {
		if {("$selection" == "allrecs")} {
		    # print all records to file or...
		    for {set i 1} {$i <= $nRetr} {incr i} {
			puts $f [format "\n%d.\n" $i]
			puts $f [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		    }
		} else {
		    # print selected records to file or...
		    foreach i $currSelectList {
			puts $f [format "\n%d.\n" $i]
			puts $f [zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
		    }
		}
	    }
	    save {
		# print all records in permanent and current save list to file.
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

	# close the temp. file, and then use host mail facility to send
	# log the transaction and delete the temporary file.
	close $f

	exec <$tmpfname mail $mailAddress

	#LOG
	switch $type {
	    main {
		pLogCmd 21
	    }
	    save {
		pLogCmd 25
	    }
	}
	exec rm $tmpfname
	
	pActionLabelOn "Inactive" 0 noflash
    } elseif {$response == 1} {
	#LOG user cancellation of e-mail
	pLogCmd 29
    }

    # Destroy window and return control to parent

    destroy $w
    update
    focus $oldFocus

}









