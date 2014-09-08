# ************************************************************************
#
#	Program Name:	mkMsgDialog.tcl	
#
#	Programmer:     Sandia Hsu, Sui-hwai Hwang	
#
#	Purpose:	Create a dialog box to display message
#	
#       usage: 	 	mkMsgDialog w msgArgs list list ...
#
#	Date:	        11/13/1993	
#	Revised:
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkMsgDialog {w msgArgs args} {
    catch {destroy $w}

# if {[string compare [tk colormodel .] "monochrome"] != 0} {
#       option add *Button.activeBackground LightSkyBlue1
#       option add *Menubutton.activeBackground LightSkyBlue1
#       option add *Menu.activeBackground LightSkyBlue1
#       option add *ListBox.activeBackground LightSkyBlue1
# }

    toplevel $w -class Dialog
    wm title $w "Message"
    wm geometry $w +200+300

    frame $w.top -relief raised -border 1
    frame $w.bot -relief raised -border 1
    pack append $w $w.top {top fill expand} $w.bot {top fill expand}
    
    eval message $w.top.msg -justify center \
	    -font -Adobe-times-medium-r-normal--*-180* $msgArgs
    pack append $w.top $w.top.msg {top expand padx 5 pady 5}
    
    # Create as many buttons as needed and arrange them from left to right
    # in the bottom frame.  Embed the left button in an additional sunken
    # frame to indicate that it is the default button, and arrange for that
    # button to be invoked as the default action for clicks and returns in
    # the dialog.

    if {[llength $args] > 0} {
	set arg [lindex $args 0]
	frame $w.bot.0 -relief sunken -border 1
	pack append $w.bot $w.bot.0 {left expand padx 20 pady 20}
	button $w.bot.0.button -text [lindex $arg 0] \
		-command "destroy $w; [lindex $arg 1]"
	pack append $w.bot.0 $w.bot.0.button {expand padx 12 pady 12}
	bind $w <Return> "[lindex $arg 1]; destroy $w"
	focus $w

	set i 1
	foreach arg [lrange $args 1 end] {
	    button $w.bot.$i -text [lindex $arg 0] \
		    -command "[lindex $arg 1]; destroy $w"
	    pack append $w.bot $w.bot.$i {left expand padx 20}
	    set i [expr $i+1]
	}
    }
    bind $w <Any-Enter> [list focus $w]
    focus $w
}
