# pDialog -- Ousterhout's dialog box routine, modified to allow a message
#   to be displayed without buttons.  In this case the dialog box still
#   grabs all input, and the caller is responsible for its destruction.
#
# BUGS: Position will be wrong if x y coordinates of main window are < 0


proc pDialog {w title text bitmap default args} {
    global dialogButton defaultBG defaultFont

    # establish separate dialog box window
    catch {destroy $w}
    toplevel $w -class Dialog
    wm title $w $title
    wm iconname $w Dialog

    wm withdraw $w
    frame $w.top -relief raised -bd 1
    pack $w.top -side top -fill both
    frame $w.bot -relief raised -bd 1
    pack $w.bot -side bottom -fill both

    # Put in the dialog box text and any bitmap to be used
    label $w.top.msg -wraplength 3i -text $text \
	-font $defaultFont
    pack $w.top.msg -side right -expand 1 -fill both \
	-padx 3m -pady 3m
    if {$bitmap != ""} {
	label $w.top.bitmap -bitmap $bitmap
	pack $w.top.bitmap -side left -padx 3m -pady 3m
    }

    # create all buttons for dialog box
    set i 0
    foreach but $args {
	button $w.bot.button$i -text $but -command \
	    "set dialogButton $i" -highlightbackground $defaultBG \
	    -font $defaultFont
	if {$i == $default} {
	    frame $w.bot.default -relief sunken -bd 1
	    raise $w.bot.button$i
	    pack $w.bot.default -side left -expand 1 \
		-padx 3m -pady 2m
	    pack $w.bot.button$i -in $w.bot.default \
		-side left -padx 2m -pady 2m \
		-ipadx 2m -ipady 1m
	} else {
	    pack $w.bot.button$i -side left -expand 1 \
		-padx 3m -pady 3m -ipadx 2m -ipady 1m
	}
	incr i
    }

    # display dialog box
    update idletasks
    set x [expr [winfo screenwidth $w]/2 - [winfo reqwidth $w]/2 \
	    - [winfo vrootx [winfo parent $w]]]
    set y [expr [winfo screenheight $w]/2 - [winfo reqheight $w]/2 \
	    - [winfo vrooty [winfo parent $w]]]
    wm geom $w +$x+$y
    wm deiconify $w


    set oldFocus [focus]
    grab $w
    tkwait visibility $w

    if {$default >= 0} {
	focus $w.bot.default
    } else {
	focus $w
    }

    # wait for user to hit a dialog box button and return its value
    # after destroying dialog box
    tkwait variable dialogButton
    destroy $w
    update
    catch {focus $oldFocus}
    return $dialogButton

}
		



