# ************************************************************************
#
#       Program Name:	mkBibLoad.tcl
#
#       Programmer:     Sandia Hsu, Sui-hwai Hwang
#
#       Purpose:	Lists the files in a directory to allow user to 
#			browse up and down others directories to choose 
#			a file to load.	
#
#       Date:		11/15/93
#       Revised:   	11/15/93
#       Version:	1.0
#       Copyright (c) 1992.  The Regents of the University of California
#               All Rights Reserved
#
# ***********************************************************************/

# display screen
proc mkBibLoad { {l .bibload} } {

  	global curdir prevdir home
        set home "[exec pwd]"
	set curdir $home

        catch {destry $l}
        toplevel $l
        wm title $l "Bibliographic Record Processing: Loading File"
        wm geometry $l +100+100
        pack append $l [frame $l.frame1] {pady 1c top} \
                    [frame $l.frame2] top \
                    [frame $l.frame3] {pady 1.5c top} \
		    [frame $l.frame20] {pady 1c top}
        pack [frame $l.frame5] \
             [frame $l.frame6] \
             -in $l.frame2 -side left -padx 2m -pady 2m
	pack [frame $l.frame21] \
	     [frame $l.frame22] \
	     -in $l.frame20 -side left -padx 2m -pady 2m
	pack [frame $l.frame7] \
	     [frame $l.frame8] \
	     -in $l.frame5 -side top -padx 2m -pady 2m
	pack [frame $l.frame9] \
	     [frame $l.frame10] \
	     -in $l.frame6 -side top -padx 2m -pady 2m 
	pack [frame $l.frame16] \
	     [frame $l.frame17] \
	     -in $l.frame3 -side top -padx 1m -pady 1m
        pack [message $l.msg -font -Adobe-times-medium-r-normal-*180* \
             -aspect 200 -text "Double click on one of the filenames to load \
             ... or ... enter another directory to browse ..." \
             -justify center] \
             -in $l.frame1 
        pack [label $l.home -width 40 -text "$home" -bd 2] \
             -in $l.frame9
        pack [entry $l.pathentry -width 40 -textvariable path \
             -relief sunken -bd 2] -in $l.frame10
        pack [button $l.defname -width 20 -text "Default Directory: " \
             -bd 2 -command {set curdir $home; loadList} -relief raised] \
	     -in $l.frame7
        pack [button $l.default -width 20 -text "Change to Directory: " \
             -relief raised -bd 2 -command {focus .bibload.pathentry}] \
	     -in $l.frame8 
        pack [scrollbar $l.scroll -relief raised -command "$l.list yview"] \
             -in $l.frame17 -side right -fill y
        pack [listbox $l.list -yscroll "$l.scroll set" -relief raised \
             -geometry 20x10 -setgrid yes] \
             -fill both -expand yes -in $l.frame17 -side left
	pack [button $l.tologin -text "Quit" \
	     -bd 2 -command {destroy .bibload; focus .nameentry; \
	     wm deiconify .} -relief raised] -in $l.frame21 -side left \
	     -padx 10m 
	pack [button $l.toproc -text "Process other records" -bd 2 \
	     -command {destroy .bibload; wm deiconify .aubib} \
	     -relief raised] -in $l.frame22 -side right \
	     -padx 2m

        wm minsize . 1 1

        pack [label $l.current -width 40 -textvar curdir \
 	     -bd 2] \
             -in $l.frame16 -anchor center
	set prevdir $curdir
        loadList
# Set up bindings for the browser.
        bind $l.list <Control-q> {destroy $l}
        bind $l.list <Control-c> {destroy $l}
        focus $l.list
        bind $l.pathentry <Return> {
                set prevdir $curdir; set curdir $path; check}
        bind $l.list <Double-Button-1> {foreach i [selection get] {
               set prevdir $curdir; set curdir $curdir/$i; check}}
}

proc loadList {} {
# Fill the listbox with a list of all the files in the directory (run
# the "ls" command to get that information).
        global curdir home

        .bibload.list delete 0 end
        foreach i [exec ls -a $curdir] {
        .bibload.list insert end $i}
}

# procedure to position the window passed
proc dpos w {
	wm geometry $w +150+150
}

# checks to see if the current selection, curdir, is a file or
# a directory ....

proc check {} {
    global curdir prevdir home
    if [file isdirectory $curdir] {
       	doublechk
       	loadList
    } else {
        if [file isfile $curdir] {
        mkDialog .oktoload {-text "You have selected $curdir.\n \
            Press \"OK\" to load this file or \n Press \"Cancel\" to \
            return to the list of files." -aspect 200 -justify \
	    center -padx 4m -pady 4m} {OK {destroy .bibload; \
  	    doBibLoad $curdir; \
 mkMsgDialog .sucessmsg {-text "MARC record processing \
                done sucessfully.\nClick one of option \
                buttons below to continue...." -aspect 250 \
                -justify left} {Quit {focus .nameentry; wm deiconify .}} \
                {{Processing Another Records} {wm deiconify \
                        .aubib}} }} \
	    {Cancel {set curdir $prevdir}}
	dpos .oktoload
	tkwait visibility .oktoload
	grab set .oktoload
	tkwait window .oktoload 
        } else {
	mkDialog .notokdir {-text "$curdir \n is not a directory or a \
		regular file.  Press \"OK\" to return to the list\
		of files." -aspect 250 -justify center -padx 4m -pady 4m} \
		{OK {set curdir $home}}
	dpos .notokdir
	tkwait visibility .notokdir
	grab set .notokdir
	tkwait window .notokdir
        	}
    	}
}

# in browsing upward from the current directory, make sure when the top is
# reached, tells user situation and return him/her to the working directory.
# reset directory path as needed.

proc doublechk {} {
	global curdir home
  	
	if {[string match "*/.." $curdir]} {
	   set listsplit [split $curdir /]
	   if {[llength $listsplit] >3} {
		set x [expr ([llength $listsplit]-3)]
		set shorter "[lrange $listsplit 0 $x]"
		set curdir [join $shorter /]
		} else {
			if {[llength $listsplit]==3} {
		if {[lindex $listsplit 1]==""} {
 		mkDialog .rootdir {-text "You have reached the root of the \
           	directory.  Press \"OK\" to return to the working \
           	directory." -aspect 250 -justify center -padx 4m -pady 4m} \
          	 {OK {set curdir $home}}
           	dpos .rootdir
           	tkwait visibility .rootdir
           	grab set .rootdir
           	tkwait window .rootdir
        } else { 
		set curdir "/"
				} } }
	} else {if [string match "*/." $curdir] {
			if {[string match "//." $curdir]} {
               mkDialog .rootdir {-text "You have reached the root of the \
                directory.  Press \"OK\" to return to the working \
                directory." -aspect 250 -justify center -padx 4m -pady 4m} \
                 {OK {set curdir $home}}
                dpos .rootdir
                tkwait visibility .rootdir
                grab set .rootdir
                tkwait window .rootdir
		} else {
			set curdir "[string trimright $curdir /.]"}
		}  }

}
