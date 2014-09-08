# ************************************************************************
#
#	Program Name:	mkSecurityCheck.tcl	
#
#	Programmer:     Sandia Hsu, Sui-hwai Hwang	
#
#	Purpose:	check if the staff has the right to login the
#			marc processing system
#	
#
#	Date:		11/15/1993
#	Revised:
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkSecurityCheck {string} {

# if {[string compare [tk colormodel .] "monochrome"] != 0} {
#       option add *Button.activeBackground LightSkyBlue1
#       option add *Menubutton.activeBackground LightSkyBlue1
#       option add *Menu.activeBackground LightSkyBlue1
#       option add *ListBox.activeBackground LightSkyBlue1
# }


	global pwd name 
	set name ""
	set pwd ""
puts $string
# this is dummied 
	focus .nameentry
	wm withdraw .
	mkAuBib
return
#end of dummy stuff
	set eofstring "JJJJJJJJJJ"
	set emptystring ""
	set f [open /usr/users/l237/sue/tk_math_proc/checkfile r]
	gets $f namepwd
	while {[string compare $namepwd $eofstring] !=0 \
		& [string compare $namepwd $string] != 0} {

		gets $f namepwd
	}

	if {[string compare $namepwd $string] == 0 & \
		[string compare $namepwd $emptystring] !=0} {
		close $f
		focus .nameentry
		wm withdraw .
		mkAuBib
	} else {
		close $f
		focus .nameentry
		mkMsgDialog  .invalidmsg {-text "***** SORRY *****\n\n
		You are not allowed to process MARC records through \
                this system because of \
		INVALID LOGIN NAME/PASSWORD....\nClick the button \
                below to go back to login screen." -aspect 250 \
                -justify left} {{Login Screen} {focus .nameentry}} 
	}
}







