# This routine displays the trace info for the SDLIP client

#  -- create the full text window and controls...

proc createTraceWindow {} {

    set textFont "-adobe-courier-bold-r-normal--17-120-*-*-*-*-*-*"

    set w .twind
    catch {destroy $w}
    
    toplevel $w
    wm title $w "SDLIP Trace Window"
    wm iconname $w "Trace"
    wm geometry $w -10-10
    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Close -command "destroy $w"
    pack $w.buttons.dismiss -side left -expand 1
    
    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -font $textFont \
         -width 50 -height 30 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    $t tag configure center -justify center -spacing1 5m -spacing3 5m
    $t tag configure red -foreground red
    $t tag configure blue -foreground blue
    $t tag configure buttons -lmargin1 1c -lmargin2 1c -rmargin 1c \
	    -spacing1 3m -spacing2 0 -spacing3 0


}



