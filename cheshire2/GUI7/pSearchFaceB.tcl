#
# 
#        File Name:      pSearchFaceB.tcl
# 
#        Programmer:     Ralph Moon, Jerome McDonough
#
#        Purpose:        Search portion of interface rewrite for boolean
# 
#        Usage:          pSearchFaceB
# 
#        Variables:      xdist, ydist: window manager x & y geometry coordinate
# 
#        Return Conditions and Return Codes:	none
# 
#        Date:           4/16/95
#        Revised:
#        Version:        1.0
#        Copyright (c) 1995.  The Regents of the University of California
#          All Rights Reserved
# 
# ***********************************************************************/

proc pSearchFaceB {xdist ydist} {

    global boolTypeFG buttonBG index searchTerm unusedButtonName
    global entryBG defaultBG defaultFont entryFont permSaveList currHistory

    # withdraw window for rearrangement
    wm withdraw .

    # destroy old search frame and contents and lower button bar

    destroy .search
    destroy .statusbar.top.bbar2

    #create new search frame and lower button bar

    frame .search -relief raised -bd 1
    frame .search.panel0 -relief flat
    frame .search.panel1 -relief flat
    frame .search.panel2 -relief flat
    frame .search.panel3 -relief flat
    frame .search.bbar -relief flat
    frame .statusbar.top.bbar2 -relief flat -bd 1

    #     Create 4 menubuttons and entry windows.  The search terms
    #     entered are associated with the index currently listed on
    #     the button
    
    label .search.title -text "SEARCH TERMS" -font $defaultFont
    label .search.booltype0 -text "A" -foreground $boolTypeFG \
	-font $defaultFont
    button .search.bool1 -relief raised -bd 1 -textvariable boolSwitch1 \
	-bg $buttonBG -fg $boolTypeFG -command "pBooleanTypeDisplay 1" \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    label .search.booltype1 -text "B" -foreground $boolTypeFG \
	-font $defaultFont
    button .search.bool2 -relief raised -bd 1 -textvariable boolSwitch2 \
	-bg $buttonBG -fg $boolTypeFG -command "pBooleanTypeDisplay 2" \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    label .search.booltype2 -text "C" -foreground $boolTypeFG \
	-font $defaultFont
    button .search.bool3 -relief raised -bd 1 -textvariable boolSwitch3 \
	-bg $buttonBG -fg $boolTypeFG -command "pBooleanTypeDisplay 3" \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    label .search.booltype3 -text "D" -foreground $boolTypeFG \
	-font $defaultFont
    
    for {set i 0} {$i<4} {incr i} {
	set index($i) "unused"
	set searchTerm($i) ""
	menubutton .search.button$i -relief raised -width 11 -height 1 \
	    -text $unusedButtonName -menu .search.button$i.menu \
	    -bg $buttonBG -state disabled -font $defaultFont
	entry .search.entry$i -width 25 -relief sunken \
	    -font $entryFont -highlightbackground $defaultBG \
	    -background $entryBG -state disabled -highlightcolor black \
	    -highlightbackground $defaultBG -highlightcolor black
	bind .search.entry$i <Return> pSearchButtonCmd
    }

    # fill in top button bar
button .search.bbar.history -relief raised -bd 2 -text "View History" \
    -command "pHistoryButtonCmd" -bg $buttonBG -state disabled \
     -highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
button .search.bbar.search -relief raised -bd 2 -padx 20 -pady 5 -text "SEARCH" \
    -command pSearchButtonCmd -bg $buttonBG -state disabled \
     -highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
button .search.bbar.clear -relief raised -bd 2 -text "Clear Terms" -bg $buttonBG \
    -command "ClearTerms" \
     -highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont


    # fill in lower button bar

    button .statusbar.top.bbar2.print -relief raised -bd 2 -text "Print" \
	-command "pPrintDlg" -bg $buttonBG -state disabled \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
    button .statusbar.top.bbar2.mail -relief raised -bd 2 -text "Mail" \
	-command "pMailButtonCmd .m main" -bg $buttonBG -state disabled \
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

    if {$permSaveList != {}} {
	.statusbar.top.bbar2.saved configure -state normal
    }

    if {$currHistory != {}} {
	.search.bbar.history configure -state normal
    }

    
    #
    #   Pack
    #
    
    pack .mbar .search .retrieve .statusbar -side top -fill x \
	-in .
    pack .search.panel0 .search.panel1 .search.panel2 .search.panel3 \
	.search.bbar -side top -fill x -in .search
    pack .search.title  -in .search.panel0 -side top -pady 1m
    pack .search.button0 .search.entry0 -in .search.panel1 \
	-side left -padx 2m -pady 1m
    pack .search.booltype0 .search.bool1 .search.booltype1 -in .search.panel1 \
	-side left -fill x -expand 1
    pack .search.entry1 .search.button1 -in .search.panel1 \
	-side right -padx 2m -pady 1m
    pack .search.bool2 -in .search.panel2
    pack .search.button2 .search.entry2 -in .search.panel3 \
	-side left -padx 2m -pady 2m
    pack .search.booltype2 .search.bool3 .search.booltype3 -in .search.panel3 \
	-side left -fill x -expand 1
    pack .search.entry3 .search.button3 -in .search.panel3 \
	-side right -padx 2m -pady 2m
    pack .search.bbar.clear -side left -pady 12 -ipadx 3m -expand 1
    pack .search.bbar.search -side left -pady 12 -ipadx 3m -ipady 2 -expand 1
    pack .search.bbar.history -side left -pady 12 -ipadx 3m -expand 1
    pack .statusbar.top .statusbar.bottom -side top -fill x -in .statusbar
    pack .statusbar.top.bbar2 -side top -fill x -in .statusbar.top
    pack .statusbar.top.bbar2.print .statusbar.top.bbar2.mail \
	.statusbar.top.bbar2.save \
	.statusbar.top.bbar2.saved -side left -pady 4 -ipadx 3m -expand 1
    
    
    # update display
    wm geometry . "+$xdist+$ydist"
    update
    wm deiconify .
}
