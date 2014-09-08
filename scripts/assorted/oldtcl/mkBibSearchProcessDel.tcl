# ************************************************************************
#
#	Program Name:	mkBibSearchProcessDel.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	
#	
#
#	Date:		11/23/1993
#	Revised:
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkBibSearchProcessDel {} { 

# if {[string compare [tk colormodel .] "monochrome"] != 0} {
#       option add *Button.activeBackground LightSkyBlue1
#       option add *Menubutton.activeBackground LightSkyBlue1
#       option add *Menu.activeBackground LightSkyBlue1
#       option add *ListBox.activeBackground LightSkyBlue1
# }


	global fieldlist result

	set searchtypelist ""
	set searchtermlist ""
	set indextermlist ""
	set i 0
	set index ""
	set term ""


	foreach el $fieldlist {
		global $el
		eval set x \$$el
		if {[string length $x] != 0} {
			lappend searchtypelist $el
			lappend searchtermlist $x
		}
		set $el ""
	}

puts $searchtypelist

puts $searchtermlist
	while {$i < [llength $searchtypelist]} {
		set index "[string tolower [lindex $searchtypelist $i]]"
		set term "[string tolower [lindex $searchtermlist $i]]"

		if {$i == [expr [llength $searchtypelist]-1]} {
			lappend indextermlist $index = $term        

		} else {
			lappend indextermlist $index = $term and

		}
		incr i
		set index ""
		set term ""

	}
puts $indextermlist

	set searchtypelist ""
	set searchtermlist ""

	zselect test
	set result [zfind $indextermlist]
		
	# set result [local_zfind $indextermlist]

puts $result
	set indextermlist ""
	mkBibLongDisplayDel

}
