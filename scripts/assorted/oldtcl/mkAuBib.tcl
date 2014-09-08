# ************************************************************************
#
#	Program Name:	mkAuBib.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	creating a toplevel window for staff to choose
#			processing option, or quit the prcessing 
#	
#
#	Date:		11/12/1993
#	Revised:        11/23/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkAuBib {{w .aubib}} {

	catch {destroy $w}
	toplevel $w
	
	wm title $w "MARC Record Processing Options"		
	wm geometry $w +100+100
 
	message $w.msg -font -Adobe-times-medium-r-normal--*180* \
		-aspect 300 -text "CLICK on one of the \
		options above to start Authority or Bibliographic \
		Record Processing, \
		, or quit to the login screen....." -width 500 -pady 0.5c 
	
	frame $w.frame1  
	frame $w.frame2

	pack $w.frame1 $w.frame2 -in $w -side top
	
	label $w.bitmap -bitmap @/usr3/ray/Work/z3950_3/scripts/oldtcl/marchhare.xbm \
		-pady 0.2c
	pack $w.frame1 $w.msg $w.bitmap -in $w -side top

	# pack $w.bitmap $w.msg -in $w.frame2 -side bottom 

	pack [menubutton  $w.au -menu $w.au.m -text \
		"Authority Control Records" -width 25 \
		-relief raised -bd 2] \
	     [menubutton $w.bib -menu $w.bib.m -text \
		"Bibliographic Records" -width 25 \
		-relief raised -bd 2] \
	     [button $w.quit -command "destroy $w; focus .nameentry; wm deiconify ." \
	        -text "Quit to Login Screen" -width 25 -relief raised -bd 2] \
	     -in $w.frame1 -side left 

	menu $w.au.m
	$w.au.m add command -label "Add a New Record" \
   		-command {mkMsgDialog .notfuncion \
			{-text "***** SORRY ***** \n
			This function is still under development. \
			Not available yet......\nClick on the <OK> button to \
			go back to option screen." -aspect 250 -justify left} \
			{OK {}}}

	$w.au.m add command -label "Delete an Old Record" \
		 -command {mkMsgDialog .notfuncion \
			{-text "***** SORRY ***** \n
			This function is still under development. \
			Not available yet......\nClick on the <OK> button to \
			go back to option screen." -aspect 250 -justify left} \
			{OK {}}
			}


	$w.au.m add command -label "Edit an Old Record" \
		 -command {mkMsgDialog .notfuncion \
			{-text "***** SORRY ***** \n
			This function is still under development. \
			Not available yet......\nClick on the <OK> button to \
			go back to option screen." -aspect 250 -justify left} \
			{OK {}}
			}


        $w.au.m add command -label "Load Records from Files" \
 		-command {mkMsgDialog .notfuncion \
			{-text "***** SORRY ***** \n
			This function is still under development. \
			Not available yet......\nClick on the <OK> button to \
			go back to option screen." -aspect 250 -justify left} \
			{OK {}}
			}


	menu $w.bib.m
        $w.bib.m add command -label "Add a New Record" \
		-command {mkMsgDialog .host {-text "You can \
			build the record from scratch, or, \
			edit an existing record from MELVYL. \
			\nClick on the following option \
			button to proceed....." -aspect 250 \
			-justify left} \
			{MELVYL {wm withdraw .aubib; \
				mkBibAdd}} \
			{HOME {wm withdraw .aubib; \
				mkBibLongDisplayAdd}}}
 

        $w.bib.m add command -label "Delete an Old Record" \
		-command "mkBibDel"
        $w.bib.m add command -label "Edit an Old Record" \
		-command "mkBibEdit"
        $w.bib.m add command -label "Load Records from Files" \
		-command "mkBibLoad"
	
}
