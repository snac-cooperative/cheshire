# This routine displays the full text version of a TREC record in
# a separate scollable window for reading.

# pFullTextEnter -- Respond to cursor entry of select button i

proc pFullTextEnter {w i} {
    global currFullTextList buttonBG defaultBG current_full_display

    if {$current_full_display != $i} {
	if {[lsearch $currFullTextList $i] < 0} {
	    $w tag configure fullTextTag$i -background $buttonBG
	}
    }
}

proc pFullTextOn {w i} {
    global currFullTextList buttonBG defaultBG current_full_display

    if {$current_full_display != $i} {
	$w tag configure fullTextTag$i -background blue
    }
}

proc pFullTextOff {w} {
    global currFullTextList buttonBG defaultBG current_full_display

    $w tag configure fullTextTag$current_full_display -background DarkGoldenrod1
    set current_full_display 0

}

# pFullTextSelect -- activate select in main window for this record
#                    and then save the record(s)

proc pFullTextSelect {w i button} {
    global currSelectList selection nRetr
    global currSaveList saveIndexList saveFormat searchSet permSaveList
    global buttonBG defaultBG 

    pSelect $w $i

    set listPos [lsearch $currSaveList $i]


    # Add index to list, and update button color if necessary
    if {$listPos >= 0} {
	# puts "removing color"
	$button.buttons.select configure -background $buttonBG
    } else {
	# puts "adding color?"
	$button.buttons.select configure -background red
	pSaveButtonCmd
	
    }
    

}

# pFullTextLeave -- Respond to cursor leaving select button i

proc pFullTextLeave {w i} {
    global currFullTextList buttonBG defaultBG current_full_display

    if {$current_full_display != $i} {
	if {[lsearch $currFullTextList $i] < 0} {
	    $w tag configure fullTextTag$i -background $defaultBG
	}
    }
}


# pFullText -- create the full text window and controls...

proc pFullText {mainwin i} {
    global currFormat currFormatName formatList nRetr 
    global fname searchSet stdFmt tmpdir SLi idleID
    global current_full_display currFullTextList currPacketSize

    set textFont "-adobe-courier-bold-r-normal--17-120-*-*-*-*-*-*"

    if {$current_full_display != 0} {
	# There is a full_text display active 
	pFullTextOff $mainwin 
    }

    if {$i == 0} return

    if {[array size searchSet] < $i} {
	# asking for a record not in the current display set
	scan [.retrieve.scroll get] "%f %f" first last
	.retrieve.scroll set $first 1.0
	pMoreButtonCmd $currPacketSize
    }

    # Find index in current save list

    set listPos [lsearch $currFullTextList $i]

    # Add or delete index from list, and reconfigure button

    if {$listPos < 0} {
        set currFullTextList [lappend currFullTextList $i]
    }

    set w .twind
    catch {destroy $w}
    
    toplevel $w
    wm title $w "Full Text for Article #$i"
    wm iconname $w "Full Text"
    wm geometry $w -10-10
    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Close -command "pFullTextOff $mainwin; set current_full_display 0; destroy $w"
    pack $w.buttons.dismiss -side left -expand 1
    button $w.buttons.select -text Select -command "pFullTextSelect $mainwin $i $w"
    pack $w.buttons.select -side left -expand 1

    button $w.buttons.prev -text "<< Previous Doc" -command "pFullText $mainwin [expr $i - 1]"
    pack $w.buttons.prev -side left -expand 1

    button $w.buttons.next -text "Next Doc >>" -command "pFullText $mainwin [expr $i + 1]"
    pack $w.buttons.next -side left -expand 1
    
    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -font $textFont \
         -width 82 -height 56 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    $t tag configure center -justify center -spacing1 5m -spacing3 5m
    $t tag configure buttons -lmargin1 1c -lmargin2 1c -rmargin 1c \
	    -spacing1 3m -spacing2 0 -spacing3 0


    set txtlines [pAppendROText $t "%s\n" \
	    [zformat long $searchSet($i) SGML -1 80 $tmpdir$fname]]

    pTRECtag $txtlines $i $t
    pFullTextOn $mainwin $i
    set current_full_display $i
    
    #reset timestuff
    bind .twind <Button> {after cancel $idleID; catch {after 600000 {pLogCmd 33; pRestart}} idleID}
    bind .twind <Key> {after cancel $idleID; catch {after 600000 {pLogCmd 33;
pRestart}} idleID}


    #LOG the command...
    set SLi $i
    pLogCmd 39

}



