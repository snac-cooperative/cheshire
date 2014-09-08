# ************************************************************************
#
#	Program Name:	mkBibMarcList.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	to form a list of marc information entered in full
#			screen; the tcl list will be passed to C coded
#			routines to update/add marc records to bibliographic
#			file 	
#
#	Date:		11/07/1993
#	Revised:        11/26/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkBibMarcList {} {

        global taglist marclist leaderfixedfieldtextlist
	global subfielddelimiterlist repeatentrylist
	global optiontype tablist recno

	set num [llength $tablist]
puts $num
	set i 1
	set marclist ""
	set j 0
	set templist ""

	# loop until the last entry value from the full screen editor

        while {$i <= $num} {
                global entryvalue$i entryvalue[expr $i+1]
                set x entryvalue$i
		set y entryvalue[expr $i+1]
		eval set X \$$x
		if {$i != $num} {
			eval set Y \$$y
		} 
		# leader 

		if {$i >= 1 & $i <= 5} {
			set marclist [linsert $marclist $i "$X"]
			incr i		

		# fixed field 001 005

		} elseif {$i >= 6 & $i <= 7} {
			set templist ""
			lappend templist [lindex \
				$leaderfixedfieldtextlist [expr \
					$i -1]] $X

			if {[llength [lindex $templist 1]] != 0 } {
				lappend marclist $templist
			}
			incr i

		# fixed field 008
				
		} elseif {$i >= 8 & $i <= 26} {

			# attach only one tag 008 at the beginning of
			# this field

			if {$i == 8} {
				set templist 008
				set templist1 ""
			}
			eval set templist1 \${templist1}\$$x
		
			# check if the end of 008 field

			if {$i == 26 & [llength $templist1] != 0} {
				lappend templist $templist1
				lappend marclist $templist
			}

			incr i

		# variable fields

		} else {

			# check if the 1st entry associated with a
			# particular tag

			if {[lindex $taglist $j] != ""} {
				eval set t \$$y
				set templist1 ""
				set templist ""
				if {[llength $x] == 0} {
					set x {  }
				}
				eval lappend templist \
				   [lindex $taglist $j] \$$x

				# subfieldcode

				if {$t != ""} {
					eval lappend templist1 \
				  	 [lindex $subfielddelimiterlist $j] \
				  	   \$$y
					lappend templist $templist1
					incr j
					incr i 2
					set t ""
				} else {
					incr i 2
					incr j
				}
			} else {
				eval set t \$$x
				if {$t != ""} {
					set templist1 ""
					eval lappend templist1 \
				  	 [lindex $subfielddelimiterlist $j] \
				  		\$$x
			        	lappend templist $templist1
					incr j
					incr i
					set t ""
				} else {
					incr i
					incr j
				}
			}
			
			# check if reach the end of each field

			if {[llength $templist] > 2} {
				if {[lindex $taglist $j] \
			   		!= ""} {
					if {$j < [llength $taglist]} {
						lappend marclist \
				   		    $templist
					}
				}

				# check if the end of marc record, i.e.,
				# check if the last field of the record

				if {$j == [llength $taglist]} {
					lappend marclist \
						$templist
				}
			}
		}
        }

	# set the value in each entry back to empty string

puts "marclist befor merager = $marclist"

	for {set i 1} {$i <= $num} {incr i} {
		set entryvalue$i ""
	}

	if {[llength $repeatentrylist] != 0} { 	
		mkBibMergeMarcList
	}
puts "marclist after meger = $marclist"

	if {$optiontype == 1} {
		doBibAdd $recno $marclist
	}
	if {$optiontype == 3} {
		doBibEdit $recno $marclist
	}

	set marclist ""
	set optiontype ""
	set recno ""

	mkMsgDialog .sucessmsg {-text "****** Completion ****** \n\nMARC \
		record processing \
		done sucessfully.\nClick on one of option \
		buttons below to continue...." -aspect 250 \
		-justify left} {Quit {focus .nameentry; wm deiconify .}} \
		{{Processing Another Records} {wm deiconify \
			.aubib}}
	
}


proc mkBibMergeMarcList {} {

	global repeatentrylist marclist

	set oldmarclist ""
	set empty ""

	foreach r $repeatentrylist {
			set oldmarclist $marclist
			set x [lindex $r 0]
			set count 1 
			foreach m $oldmarclist {
				set y [lindex $m 0]
				if {[llength $m] > 2} {
					if {$x <= $y} {
						set marclist [linsert \
							$marclist \
							[expr $count-1] $r]
						break
					}
				}
				if {$count == [llength $oldmarclist] & \

					$x > $y } {
					set marclist [linsert $marclist \
						$count $r]
					break
				}
				incr count
			}
	}
	
puts $repeatentrylist
puts "oldmarclist = $oldmarclist"

	set repeatentrylist ""
}

