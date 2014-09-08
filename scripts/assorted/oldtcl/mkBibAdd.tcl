# ************************************************************************
#
#	Program Name:	mkBibAdd.tcl
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#	Purpose:	
#	
#
#	Date:		11/23/1993
#	Revised:        11/28/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkBibAdd {{w .bibadd}} {

	global fieldlist 
	catch {destroy $w}
	toplevel $w
	
	wm title $w "Bibliographic Record Processing: Adding"
	wm geometry $w +500+100


	pack [frame $w.frame3] [frame $w.frame4] -in $w -side top

	pack [label $w.emptylabeltop -text "          "] \
	     [label $w.panel -text "Search Request Panel" \
		-relief raised -bd 3 -padx 2c] \
	     [message $w.msg -font -Adobe-times-medium-r-normal--*180* \
                -aspect 300 \
		-text "Enter information in the following \
		entries to retrieve records from MELVYL..." \
		-pady 1c \
		-padx 1c -width 500] \
	     [frame $w.frame5] \
	     [frame $w.frame8] \
	     -in $w.frame3 -side top  

	pack [frame $w.frame6] \
	     [frame $w.frame7] \
	     -in $w.frame5 -side left
	
	set fieldlist {Author Title}
       
	foreach i $fieldlist {
                pack [label $w.t$i -text "$i:"] \
			-in $w.frame6 -side top -anchor w -padx 0.5c
		pack [entry $w.searchentry$i -textvar $i \
			-width 50 -relief sunken -bd 2] \
			-in $w.frame7 -side top -anchor w -padx 0.5c
		bind $w.searchentry$i <Return> {destroy .bibadd
						wm withdraw .aubib
						mkBibSearchProcessDel}
	}

	pack [button $w.ok -text "OK" -relief raised -bd 2 \
		-command {destroy .bibadd
			 wm withdraw .aubib
			 mkBibSearchProcessAdd}] \
             [button $w.cancel -text "Cancel" \
		-command {set Auther ""
			  set Title ""
			 destroy .bibadd} \
			 -bd 2 -relief raised] \
		-in $w.frame8 -side left -padx 1c -pady 0.5c

} 
