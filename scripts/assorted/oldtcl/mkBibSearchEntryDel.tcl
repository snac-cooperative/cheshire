# ************************************************************************
#
#	Program Name:	mkBibSearchEntryDel.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	
#	
#
#	Date:		11/22/1993
#	Revised:
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/

proc mkBibSearchEntryDel {{c .bibdel2}} {

# if {[string compare [tk colormodel .] "monochrome"] != 0} {
#       option add *Button.activeBackground LightSkyBlue1
#       option add *Menubutton.activeBackground LightSkyBlue1
#       option add *Menu.activeBackground LightSkyBlue1
#       option add *ListBox.activeBackground LightSkyBlue1
# }

	global searchstringlist searchtypelist
	catch {destroy $c}
	toplevel $c
	
	wm title $c "Bibliographic Record Processing: Editing"
	wm geometry $c +500+200
	
        pack [label $c.emptylabeltop -text "          "] \
             [label $c.panel -text "Entry Panel for \
		Search Request" \
                -relief raised -bd 3 -padx 2c] \
             [label $c.msg -text "Enter Information on the following \
                entries  to start searching \
                records for edting......"] \
		[frame $c.frame5] \
             [label $c.emptylabelbottom -text "     "] \
	     -in $c -side top -padx 2c
	pack [frame $c.frame5left] [frame $c.frame5right] \
		-in $c.frame5 -side left

	set i 0
	set k 0
	set term ""
	set searchstringlist {}
	set entrywindowlist {} 
	while {$i < [llength $searchtypelist]} {
			
		set x [lindex $searchtypelist $i]
		
		
		pack [entry $c.frame5right.$i -width 35 \
                          -relief raised -textvariable $x] \
                       	-in $c.frame5right \
			     -side top

	       	pack [label $c.frame5left.$k -text "$x:  " \
			-width [string length "$x: "]] \
			-in $c.frame5left -side top -anchor nw 

	 	lappend entrywindowlist $c.frame5right.$i
		
		if {$i == [expr [llength $searchtypelist]-1]} {
			bind $c.frame5right.$i <Return> \
				"eval lappend searchstringlist {\$$x}; \
				destroy $c; \
				mkBibSearchProcessDel"
		} else {
			bind $c.frame5right.$i <Return> \
			"eval lappend searchstringlist {\$$x}; \
			focus $c.frame5right.[expr $i+1]"
		}
		incr i 
		incr k

	}
	focus [lindex $entrywindowlist 0]
	tkwait variable [lindex $searchtypelist 0] 
}
