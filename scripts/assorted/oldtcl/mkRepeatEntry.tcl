#************************************************************************t//
#
#	Program Name:	mkRepeatEntry.tcl
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu	
#
#	Purpose:	create window for added entries and form those
#			information into a list which will merge with
#			marclist created in the mkBibMarcList.tcl
#			routine
#	
#
#	Date:		11/18/1993
#	Revised:        11/23/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkRepeatEntry {{w .repeat}} {
	global ttag tablist1 repeatentrylist

# if {[string compare [tk colormodel .] "monochrome"] != 0} {
#       option add *Button.activeBackground LightSkyBlue1
#       option add *Menubutton.activeBackground LightSkyBlue1
#       option add *Menu.activeBackground LightSkyBlue1
#       option add *ListBox.activeBackground LightSkyBlue1
# }


	catch {destroy $w}
	toplevel $w
	wm title $w "Repeat Entry Panel"
	wm geometry $w +300+500

	set tablist1 ""
	
	pack append $w [frame $w.frametop] {top pady 1c} [frame $w.frameupper] \
		{top pady 05.c} [frame $w.framelower -bd 2 -relief raised] \
		bottom

	pack [frame $w.frame1] \
		[frame $w.frame2] \
		[frame $w.frame3] [frame $w.frame4] \
		-in $w.frameupper -side left -padx 0.5c
	pack [button $w.ok -text "OK" -command {mkRepeatEntryList
						destroy .repeat} \
			 -relief raised -bd 2] \
		[button $w.cancel -text "Cancel" \
      		        -command {for {set i 1; set k 0} \
				{$k <= [lindex $ttag 1]} {incr i; incr k} {
					global repeatentryvalue$i
                			set repeatentryvalue$i ""
        			}
        			set ttag ""
				destroy .repeat} \
		   -relief raised -bd 2] \
		   -in $w.framelower -side left -padx 2.5c -pady 0.3c
			
	for {set i 0; set k 1} {$i < [lindex $ttag 1]} \
		{incr i; incr k} {
		if {$i == 0} {
			label $w.label -text [lindex $ttag 0] 
			pack $w.label -in $w.frame1 -anchor nw

			global repeatentryvalue$k
			entry $w.repeatentry$k -width 10 -relief sunken \
				-bd 2 -textvariable repeatentryvalue$k
			pack $w.repeatentry$k -in $w.frame2 -anchor nw 
			lappend tablist1 $w.repeatentry$k
			incr k
		}
		set j [expr $i+2]
		label $w.subfielddelimiter$i \
			-text [lindex $ttag $j]
		pack $w.subfielddelimiter$i -in $w.frame3 -side top -anchor nw
		global repeatentryvalue$k
		entry $w.repeatentry$k -width 50 -relief sunken -bd 2 \
			-textvariable repeatentryvalue$k
		pack $w.repeatentry$k -in $w.frame4 -anchor nw
		lappend tablist1 $w.repeatentry$k
	
	}

	foreach el $tablist1 {
		bind $el <Tab> "Tab1"
	}
}

proc mkRepeatEntryList {} {

	global repeatentrylist ttag

puts "I am here"
puts $ttag
	set templist ""
	for {set i 1; set j 2; set k 2} {$i <= [lindex $ttag 1]} \
		{incr i; incr j; incr k} {
		global repeatentryvalue$j
		eval set x \$repeatentryvalue$j
puts "x=$x"
		if {[string length $x] != 0} {
				set templist1 ""
				lappend templist1 \
				[lindex $ttag $k] $x
				lappend templist $templist1
		}
puts "templist=$templist"
	}

	if {[llength $templist] != 0} {
		global repeatentryvalue1
		set templist [linsert $templist 0 \
				[lindex $ttag 0]]

		set templist [linsert $templist 1 \
				$repeatentryvalue1]	
		lappend repeatentrylist $templist

	}

puts "templist =$templist"
puts "repeatentrylist =$repeatentrylist"

	for {set i 1; set k 0} {$k <= [lindex $ttag 1]} {incr i; incr k} {
		global repeatentryvalue$i
		set repeatentryvalue$i ""
	}
	set ttag ""
		
}

proc Tab1 {} {

	global tablist1
        set i [lsearch $tablist1 [focus]]

        if {$i < 0} {
                set i 0
        } else {
                incr i
                if {$i >= [llength $tablist1]} {
                        set i 0
                }
        }
        focus [lindex $tablist1 $i]
}


