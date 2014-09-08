# pComplaints


proc pComplaints {w} {
    global entryBG defaultBG defaultActiveBG buttonBG
    global mailAddress mailSubject response dialogButton
    global nRetr searchSet currFormat currSelectList
    global currSaveList permSaveList saveFormat selection
    global fname rectype selectFG tmpdir tmpfname ERROR
    global C2_helpinfo1 C2_helpinfo2 C2_helpinfo3 defaultFont
    global entryFont

    # Build the mailing window
    set location "center";		 # Location of window
    set mailTextBG AntiqueWhite2;# Text widget background

    set mailAddress "cheshire@sunsite.berkeley.edu";	 # Initialize variables
    set mailSubject "Cheshire II Complaints Dept."

    toplevel $w -class Mail
    wm title $w "Cheshire II Complaints Dept."
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

    frame $w.comm -relief raised -bd 1; # Background for comments
    frame $w.comm.top -relief flat;	# Background for "Comments" label
    frame $w.comm.bottom -relief flat;	# Background for comments entry
    frame $w.bbar -relief raised -bd 1;	# Background for button bar

    pack $w.comm $w.bbar -side top -fill both
    pack $w.comm.top $w.comm.bottom -side top -fill both

    # Comments window

    label $w.comm.label -text "Cheshire II is still in development.  If 
there are any changes you'd like to see made to Cheshire II, or new features 
you'd like to see added, type your suggestions and/or complaints in the 
text area below, then press the \"Mail\" key.  Your comments will be sent to 
the Cheshire II development team.  While we can't respond to every e-mail 
we receive, if you include a return e-mail address in your message and 
a specific question or suggestion, we'll do our best to get back to you. 
Thanks!" -font $defaultFont

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

    # Log the transaction as a user help interaction
    set C2_helpinfo1 4
    set C2_helpinfo2 002
    set C2_helpinfo3 [$w.comm.text get 1.0 end]
    pLogCmd 4


    # if the user really asks us to mail something
    if {$response == 0} {

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

	# close the temp. file, and then use host mail facility to send
	# log the transaction and delete the temporary file.
	close $f

	exec <$tmpfname mail $mailAddress

	exec rm $tmpfname
	
	pActionLabelOn "Inactive" 0 noflash
    } elseif {$response == 1} {


    }

    # Destroy window and return control to parent
    pLogCmd 34
    destroy $w
    update
    focus $oldFocus

}









