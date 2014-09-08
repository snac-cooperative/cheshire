# ************************************************************************
#
#	Program Name:	mkBibSearchProcessAdd.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	
#	
#
#	Date:		11/23/1993
#	Revised:        11/29/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkBibSearchProcessAdd {} { 

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

	zselect melvyl
	set result [zfind $indextermlist]
		
	# set result [local_zfind $indextermlist]

puts $result
	set indextermlist ""
	mkBibLongDisplayAddMelvyl

}
