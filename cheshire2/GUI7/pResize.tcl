# pResize -- Resize the user interface

proc pResize {} {
    global newwindowsize windowsize dialogButton unusedProbButtonName
    global buttonBG entryBG boolTypeFG index searchTerm unusedButtonName
    global probTypeList currProbType defaultBG defaultFont entryFont
    global permSaveList currHistory smallFont disabledFG textBG
    global defaultActiveBG dialogBG dialogActiveBG HTFG HTActiveFG
    global selectFG hostList currHost searchList formatList indexList
    global titleFont

    # if new size is same as old, just bail out
    if {"$newwindowsize" == "$windowsize"} {
	return
    }

    # double check with user before resizing (which will restart
    # the user interface, resetting it to probabilistic mode).
    pDialog .d "Resize Cheshire II Window" "Resizing the Cheshire II\
user interface will end the current connection and clear your current\
search results.  Are you sure you want to do this?" \
	{warning} 0 {OK} {Cancel}
    
    if {$dialogButton != 0} {
	if {"$windowsize" == "large"} {
	    set newwindowsize large
	    .mbar.special.menu.win activate "Large"
	} elseif {"$windowsize" == "medium"} {
	    set newwindowsize medium
	    .mbar.special.menu.win activate "Medium"
	} elseif {"$windowsize" == "small"} {
	    set newwindowsize small
	    .mbar.special.menu.win activate "Small"
	}
	return
    }
    
    # if we get this far, user has agreed to resize interface.  So
    # set some key variables

    set windowsize $newwindowsize

    if {$windowsize == "large"} {
	wm minsize . 650 650
	wm maxsize . 950 950
	set titleFont "-adobe-courier-bold-o-normal--17-120-100-100-m-100-iso8859-1"
	set defaultFont "-adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1"; # for Buttons
	set entryFont "-adobe-courier-bold-r-normal--17-120-100-100-m-100-iso8859-1";     # for entry widgets
	set smallFont "-adobe-times-bold-i-normal--14-100-100-100-p-77-iso8859-1";       # for small labels
	set statusFont "-adobe-helvetica-bold-r-normal--14-100-100-100-p-82-iso8859-1";  # for status bar
    } elseif {$windowsize == "medium"} {
	wm minsize . 525 525
	wm maxsize . 850 850
	set titleFont "-adobe-courier-bold-o-normal--14-100-100-100-m-90-iso8859-1"
	set defaultFont "-adobe-helvetica-bold-r-normal--14-100-100-100-p-82-iso8859-1"; # for Buttons
	set entryFont "-adobe-courier-bold-r-normal--14-100-100-100-m-90-iso8859-1";     # for entry widgets
	set smallFont "-adobe-times-bold-i-normal--11-80-100-100-p-57-iso8859-1";       # for small labels
	set statusFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1";  # for status bar
    } elseif {$windowsize == "small"} {
	wm minsize . 400 400
	wm maxsize . 750 750
	set titleFont "-adobe-courier-bold-o-normal--11-80-100-100-m-60-iso8859-1"
	set defaultFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1"; # for Buttons
	set entryFont "-adobe-courier-bold-r-normal--11-80-100-100-m-60-iso8859-1";     # for entry widgets
	set smallFont "-adobe-times-bold-i-normal--11-80-100-100-p-57-iso8859-1";       # for small labels
	set statusFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1";  # for status bar
    }

    # destroy the old interface (gracefully)
    wm withdraw .
    pRestart
    destroy .mbar
    destroy .search
    destroy .retrieve
    destroy .statusbar

    #
    # Basic structure of search interface
    # 
    
    wm title . "Cheshire II"
    wm iconname . "Cheshire II"
    
    frame .mbar -relief raised -bd 1;	# Background for menu bar
    frame .search -relief raised -bd 1;	# Background for entry windows
    frame .search.panel0 -relief flat;	#   Label
    frame .search.panel1 -relief flat;	#   space for prob. search panel
    frame .search.panel2 -relief flat;	#   Empty???
    frame .search.panel3 -relief flat;      #   boolean entry windows
    frame .search.bbar -relief flat;        # Background for button bar
    frame .retrieve -relief raised -bd 1;	# Background for text window
    frame .statusbar -relief raised -bd 1;	#   Control panel
    frame .statusbar.top -relief flat;
    frame .statusbar.bottom -relief flat;
    frame .statusbar.top.bbar2 -relief flat -bd 1;  #Background for lower button bar
    frame .statusbar.bottom.a -relief flat;	#     Retrieval counter
    frame .statusbar.bottom.b -relief flat;	#     Action label -- blinkenlight
    
    
    #
    #   Status Bar
    #
    
    scale .statusbar.bottom.a.scale -label Retrievals -length 10c -sliderlength 8 \
	-background $defaultBG -orient horizontal -from 0 \
	-foreground $disabledFG -state disabled -troughcolor $disabledFG \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $statusFont
    
    label .statusbar.bottom.a.hitsLabel -textvariable nHits \
	-foreground $defaultBG -font $statusFont
    
    button .statusbar.bottom.b.actionLabel -text "Inactive" -width 12 \
	-relief sunken -foreground $defaultBG \
	-disabledforeground $defaultBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $statusFont
    
    
    #
    #   Lower Button Bar
    #
    
    button .statusbar.top.bbar2.print -relief raised -bd 2 -text "Print" \
	-command "pPrintDlg" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .statusbar.top.bbar2.mail -relief raised -bd 2 -text "Mail" \
	-command "pMailButtonCmd .m main" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .statusbar.top.bbar2.feedback -relief raised -bd 2 \
	-text "More Like Selected" \
	-command "pFeedbkButtonCmd" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .statusbar.top.bbar2.save -relief raised -bd 2 -text "Save" \
	-command "pSaveButtonCmd" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .statusbar.top.bbar2.saved -relief raised -bd 2 -text "View Saved" \
	-command "pViewSaveButtonCmd" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    
    #
    #   Retrieval window
    #   
    
    text .retrieve.text -relief sunken -bd 1 -width 86 -height 22 \
	-background $textBG \
	-highlightbackground $defaultBG \
	-highlightcolor $defaultBG \
	-selectbackground $textBG \
	-selectborderwidth 0 \
	-font $entryFont \
	-cursor {} \
	-wrap word \
	-state disabled \
	-yscrollcommand ".retrieve.scroll set";	# Text window
    
    scrollbar .retrieve.scroll \
	-background $defaultBG \
	-activebackground $defaultActiveBG \
	-relief sunken \
	-troughcolor $buttonBG \
	-highlightcolor $buttonBG \
	-highlightbackground $defaultBG \
	-command ".retrieve.text yview";		# Scroll bar
    
    bind .retrieve.scroll <Any-ButtonRelease> {pMoreButtonCmd $currPacketSize}
    pTagCreate .retrieve.text
    
    
    #
    #   Entry window
    #   
    
    #     Create 4 menubuttons and entry windows.  The search terms
    #     entered are associated with the index currently listed on
    #     the button
    
    label .search.title -text "SEARCH TERMS" -font $defaultFont
    label .search.problabel -font $smallFont -text "Ranked Searching:"
    menubutton .search.probtype -relief raised -text $unusedProbButtonName  \
	-menu .search.probtype.menu -bg $buttonBG -state disabled -width 13 \
	-height 1 -font $defaultFont
    text .search.probentry -relief sunken -width 75 -height 2  \
	-font $entryFont -highlightbackground $defaultBG \
	-background $entryBG  -wrap word -state disabled -highlightcolor black
    bind .search.probentry <Return> pSearchButtonCmd
    
    label .search.boollabel -font $smallFont -text "Boolean Searching:"
    label .search.booltype0 -text "A" -foreground $boolTypeFG \
	-font $defaultFont
    button .search.bool1 -relief raised -bd 1 -textvariable boolSwitch1 \
	-bg $buttonBG -fg $boolTypeFG -command "pBooleanTypeDisplay 1" \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    label .search.booltype1 -text "B" -foreground $boolTypeFG \
	-font $defaultFont

    for {set i 0} {$i<2} {incr i} {
	set index($i) "unused"
	set searchTerm($i) ""
	menubutton .search.button$i -relief raised -width 11 -height 1 \
	    -text $unusedButtonName -menu .search.button$i.menu \
	    -bg $buttonBG -state disabled -font $defaultFont
	entry .search.entry$i -width 25 -relief sunken \
	    -font $entryFont -highlightbackground $defaultBG \
	    -background $entryBG -state disabled -highlightcolor black
	bind .search.entry$i <Return> pSearchButtonCmd
    }
    
    
    #
    #   Upper Button bar
    #   
    
    button .search.bbar.history -relief raised -bd 2 -text "View History" \
	-command "pHistoryButtonCmd" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont 
    button .search.bbar.search -relief raised -bd 2 -padx 20 -pady 5 \
	-text "SEARCH" -command pSearchButtonCmd -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .search.bbar.clear -relief raised -bd 2 -text "Clear Terms" \
	-bg $buttonBG -command "ClearTerms" -highlightbackground $defaultBG \
	-highlightcolor $defaultBG -font $defaultFont
    
    #
    #   Menubar
    #  
    
    menubutton .mbar.exit -text "Exit  " -menu .mbar.exit.menu -underline 0 \
	-font $defaultFont
    label .mbar.hostDisp -relief flat -text "Host:" -font $defaultFont
    menubutton .mbar.host -width 20 -relief groove -bd 3 -textvariable \
	currHostName -bg $buttonBG -fg red -menu .mbar.host.menu \
	-font $defaultFont
    label .mbar.searchDisp -relief flat -text "  Search Interface:" \
	-font $defaultFont
    menubutton .mbar.search -width 10 -relief groove -bd 3 \
	-textvariable currSearchName -bg $buttonBG -menu .mbar.search.menu \
	-font $defaultFont
    label .mbar.formatDisp -relief flat -text "  Display:" -font $defaultFont
    menubutton .mbar.format -width 7 -relief groove -bd 3 \
	-textvariable currFormatName -bg $buttonBG -menu .mbar.format.menu \
	-font $defaultFont
    menubutton .mbar.special -text "Options" -menu .mbar.special.menu \
	-underline 0 -font $defaultFont
    menubutton .mbar.help -text "Help" -menu .mbar.help.menu \
	-underline 0 -font $defaultFont
    
    
    #   Exit menu
    
    menu .mbar.exit.menu -tearoff 0
    .mbar.exit.menu add command -label "Yes" -underline 0 \
	-command "pRestart" -font $defaultFont
    .mbar.exit.menu add command -label "No" -underline 0 -font $defaultFont
    
    # the REAL way to quit
    
    bind . <Control-q> {zclose; destroy .; exit}
    
    #   Host menu
    
    menu .mbar.host.menu -selectcolor $selectFG -tearoff 0
    foreach l $hostList {
	.mbar.host.menu add radiobutton -indicatoron 1 -selectcolor $selectFG \
	    -label [lindex $l 1] \
	    -variable currHostName -value [lindex $l 1] \
	    -command "pZselect [lindex $l 0]" -font $defaultFont
    }
    .mbar.host.menu add radiobutton -label "None" -variable currHostName \
	-value "START" -command "set lastHost $currHost; set currHost \"\"; \
     set currSearch \"pr\"; pSearchChange 1; pResetButtonCmd; zclose" \
	-font $defaultFont
    
    
    #   Search interface menu
    
    menu .mbar.search.menu -selectcolor $selectFG -tearoff 0
    foreach l $searchList {
	.mbar.search.menu add radiobutton -label [lindex $l 1] \
	    -variable currSearch -value [lindex $l 0] \
	    -command "pSearchChange 0;" -font $defaultFont
    }
    
    #   record format menu
    
    menu .mbar.format.menu -selectcolor $selectFG -tearoff 0
    foreach l $formatList {
	.mbar.format.menu add radiobutton -label [lindex $l 1] \
	    -variable currFormat -value [lindex $l 0] \
	    -command pReformat -font $defaultFont
    }
    
    
    #   Options menu
    menu .mbar.special.menu -tearoff 0 -selectcolor $selectFG
    .mbar.special.menu add cascade -label "Packet Size" -underline 0 \
	-menu .mbar.special.menu.packet -font $defaultFont
    .mbar.special.menu add cascade -label "Window Size" -underline 0 \
	-menu .mbar.special.menu.win -font $defaultFont
    .mbar.special.menu add command -label "Restart" -underline 0 \
	-command pResetButtonCmd -font $defaultFont
    menu .mbar.special.menu.packet -tearoff 0 -selectcolor $selectFG
    foreach l {{5 "5"} {10 "10"} {25 "25"} \
		   {50 "50"} {75 "75"} {100 "100"}} {
	.mbar.special.menu.packet add radiobutton -label [lindex $l 1] \
	    -variable currPacketSize -value [lindex $l 0] \
	    -font $defaultFont
    }
    menu .mbar.special.menu.win -tearoff 0 -selectcolor $selectFG
    .mbar.special.menu.win add radiobutton -label "Small" \
	-variable newwindowsize -value "small" -command pResize \
	-font $defaultFont
    .mbar.special.menu.win add radiobutton -label "Medium" \
	-variable newwindowsize -value "medium" -command pResize \
	-font $defaultFont
    .mbar.special.menu.win add radiobutton -label "Large" \
	-variable newwindowsize -value "large" -command pResize \
	-font $defaultFont
    
    #   Help Menu
    menu .mbar.help.menu -tearoff 0
    .mbar.help.menu add command -label "Quick Start" -underline 0 \
	-command "createHelpListBox QuickStart doc/quickstart.help.txt" \
	-font $defaultFont
    .mbar.help.menu add command -label "On Cheshire II" -underline 3 \
	-command "tkiWinShow Top cheshire2.info" -font $defaultFont
    .mbar.help.menu add command -label "On Search Strategies" -underline 3 \
	-command "tkiWinShow Top search.info" -font $defaultFont
    .mbar.help.menu add command -label "Complaints Dept." -underline 11 \
	-command "pComplaints .help" -font $defaultFont
    .mbar.help.menu add command -label "Credits" \
	-command "chCredits .help main" -font $defaultFont
    
    #
    #   Pack
    #
    
    pack .mbar .search .retrieve .statusbar -side top -fill x
    pack .search.panel0 -side top -fill x
    pack .search.panel1 -side top -fill x -pady 1m
    pack .search.panel2 .search.panel3 .search.bbar -side top -fill x
    pack .mbar.exit -side left -pady 2
    pack .mbar.hostDisp .mbar.host -side left -pady 2
    pack .mbar.searchDisp .mbar.search -side left -pady 2
    pack .mbar.formatDisp .mbar.format -side left -pady 2
    pack .mbar.help -side right -ipadx 2m -pady 2
    pack .mbar.special -side right -ipadx 2m -pady 2
    pack .search.bbar.clear -side left -pady 12 -ipadx 3m -expand 1
    pack .search.bbar.search -side left -pady 12 -ipadx 3m -ipady 2 -expand 1
    pack .search.bbar.history -side left -pady 12 -ipadx 3m -expand 1
    pack .retrieve.scroll -in .retrieve -side right -fill y -padx 2 -pady 2
    pack .retrieve.text -in .retrieve -side left -padx 2 -pady 2 -fill x -expand 1
    pack .statusbar.top .statusbar.bottom -side top -fill x -in .statusbar
    pack .statusbar.top.bbar2 -side top -fill x -in .statusbar.top
    pack .statusbar.top.bbar2.print .statusbar.top.bbar2.mail \
	.statusbar.top.bbar2.feedback .statusbar.top.bbar2.save \
	.statusbar.top.bbar2.saved -side left -pady 4 -ipadx 3m -expand 1
    pack .statusbar.bottom.a -in .statusbar.bottom -side left -fill x -pady 1
    pack .statusbar.bottom.b -in .statusbar.bottom -side right -fill x -pady 1
    pack .search.title  -in .search.panel0 -side top
    pack .search.problabel -in .search.panel1 -side top -anchor w
    pack .search.probtype -in .search.panel1 \
	-side left -padx 1m
    pack .search.probentry -in .search.panel1 \
	-side left -padx 1m -pady 1m
    pack .search.boollabel -in .search.panel2 -side top -anchor w
    pack .search.button0 -in .search.panel2 \
	-side left -padx 1m
    pack .search.entry0 -in .search.panel2 \
	-side left -padx 1m -pady 1m
    pack .search.booltype0 .search.bool1 .search.booltype1 -in .search.panel2 \
	-side left -fill x -expand 1
    pack .search.entry1 -in .search.panel2 \
	-side right -padx 1m -pady 1m
    pack .search.button1 -in .search.panel2 \
	-side right -padx 1m
    pack .statusbar.bottom.a.scale -in .statusbar.bottom.a -side left -padx 3m \
	-ipadx 5 -ipady 3
    pack .statusbar.bottom.a.hitsLabel -in .statusbar.bottom.a -side left -ipady 3
    pack .statusbar.bottom.b.actionLabel -in .statusbar.bottom.b -side right \
	-padx 10m -ipadx 1m -ipady 3
    
    # set cursor keys to move record display window 
    bind . <KeyPress-Up> {.retrieve.text yview scroll -1 units}
    bind . <KeyPress-Down> {.retrieve.text yview scroll 1 units}
    bind . <KeyPress-Prior> {.retrieve.text yview scroll -1 pages}
    bind . <KeyPress-Next> {.retrieve.text yview scroll 1 pages}
    bind . <KeyRelease-Up> {pMoreButtonCmd $currPacketSize}
    bind . <KeyRelease-Down> {pMoreButtonCmd $currPacketSize}
    bind . <KeyRelease-Prior> {pMoreButtonCmd $currPacketSize}
    bind . <KeyRelease-Next> {pMoreButtonCmd $currPacketSize}
    
    # Set the client to restart after 5 minutes of nonuse
    catch {after 600000 {pLogCmd 33; pRestart}} idleID
    bind . <Button> {after cancel $idleID; catch {after 600000 {pLogCmd 33; pRestart}} idleID}
    bind . <Key> {after cancel $idleID; catch {after 600000 {pLogCmd 33;
	pRestart}} idleID}
    
    # foreground the client interface
    update
    wm deiconify .

}


