# ************************************************************************
#
#	Program Name:	mkBibLongDisplayAdd.tcl	
#
#	Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu	
#
#	Purpose:	create a full screen MARC record format for
#			staff to edit the content of a new marc record        
#	
#
#	Date:		11/07/1993		
#	Revised:        11/27/1993
#	Version:	
#	Copyright (c) 1992.  The Regents of the University of California
#		All Rights Reserved
#
# ***********************************************************************/
proc mkBibLongDisplayAdd {{w .dis2}} {

    global tablist leaderfixedfieldtextlist taglist
    global subfielddelimiterlist
    global optiontype ttag repeatentrylist recno

    catch {destroy $w}
    toplevel $w 
    

    wm title $w "MARC Record Adding Panel"
    wm geometry $w +0+0
    
	# optiontype: adding a new marc record 

    set optiontype 1 
    set c $w.frame2.c
    set recno 0
    frame $w.frame1 -relief raised -bd 2
    frame $w.frame2 -relief raised -bd 2
    frame $w.frame3 -relief sunken -bd 2

	
    button $w.frame3.ok -text "Add Me" -command "destroy .dis2; mkBibMarcList"
    button $w.frame3.quit -text "Quit to Login Screen" \
			 -command {destroy .dis2
			 	focus .nameentry
			 	wm deiconify .}
 
    button $w.frame3.change -text "Change Processing" -command \
			 {destroy .dis2
			 wm deiconify .aubib}
			 
    pack $w.frame3.ok $w.frame3.change $w.frame3.quit \
	-in $w.frame3 \
	 -side left -padx 3c -pady 0.5c 
 
    pack append $w $w.frame1 {top fill} $w.frame2 {top fill \
            expand} $w.frame3 {bottom fill}
    
    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* \
	-aspect 1200 \
        -text "In this panel, you can edit/add MARC record using the \
		following methods: \n \
		\t1. move your <mouse cursor> to the field to be \
edited and press <Button 1> to start editing/adding, \n \
		\t2. press <Button 1> on scrollbar, or, press <Button 2> \
to scroll screens, \n \
		\t3. <tab> through different field entries, \n \
		\t4. click on the <tag> (ex. 600, 650...) \
		 for repeated entries \n \
		\t5. to complete the process, \

click on the <Add Me> button on the bottom of screen......"

    pack append $w.frame1 $w.frame1.m {frame center}

    canvas $c -scrollregion {0c 0c 30c 100c} -width 30c -height 20c
    scrollbar $w.frame2.vscroll  -relief sunken -command "$c yview"
    pack append $w.frame2 \
            $w.frame2.vscroll {right filly} $c {expand fill}

    $c config -yscrollcommand "$w.frame2.vscroll set"

    # Display a rectangular grid with four areas divided.
    
    $c create rect 0c 0c 30c 100c -width 2 
    $c create line 0c 5.7c 30c 5.7c -width 2
    $c create line 7c 5.7c 7c 100c -width 2
    $c create line 9c 5.7c 9c 100c -width 2
    $c create line 11c 5.7c 11c 100c -width 2
    $c create line 13c 5.7c 13c 100c -width 2

    set font1 -Adobe-Helvetica-Medium-R-Normal-*-180-*
    set font2 -Adobe-Helvetica-Bold-R-Normal-*-240-*

    set blue DeepSkyBlue3
    set red red
    set bisque bisque3
    set green SeaGreen3
	
    set leaderfixedfieldtextlist "Record_Status Record_Type \
	Bib_Level Encode_Level DesCatFm 001 005 008/00-05 008/06 \
	008/07-10 008/11-14 008/15-17 008/18-21 008/22 008/23 \
	008/24-27 008/28 008/29 008/30 008/31 008/32 008/33 \
	008/34 008/35-37 008/38 008/39"

    # set up each fields of MARC for editing or adding

    $c create text 2c 0.5c -text "Record Status" -anchor n
    $c create text 8c 0.5c -text "Record Type" -anchor n
    $c create text 14c 0.5c -text "Bib. Level" -anchor n
    $c create text 20c 0.5c -text "Enc. Level" -anchor n
    $c create text 26c 0.5c -text "Des. Cat. Fm." -anchor n

    $c create text 2c 1.2c -text "001" -anchor n
    $c create text 14c 1.2c -text "005" -anchor n


    $c create text 2c 1.9c -text "ENTRD 008/00-05" -anchor n
    $c create text 8c 1.9c -text "DAT TP 008/06" -anchor n
    $c create text 14c 1.9c -text "DATE1 008/07-10" -anchor n
    $c create text 20c 1.9c -text "DATE2 008/11-14" -anchor n
    $c create text 26c 1.9c -text "CNTRY 008/15-17" -anchor n

    $c create text 2c 2.6c -text "ILLUS 008/18-21" -anchor n
    $c create text 8c 2.6c -text "INT LVL 008/22" -anchor n
    $c create text 14c 2.6c -text "REPR 008/23" -anchor n
    $c create text 20c 2.6c -text "CONT 008/24-27" -anchor n
    $c create text 26c 2.6c -text "GOVT PUB 008/28" -anchor n

    $c create text 2c 3.3c -text "CONF PUB 008/29" -anchor n
    $c create text 8c 3.3c -text "FESTSCHR 008/30" -anchor n
    $c create text 14c 3.3c -text "INDEX 008/31" -anchor n
    $c create text 20c 3.3c -text "MEI 008/32" -anchor n
    $c create text 26c 3.3c -text "FIC 008/33" -anchor n

    $c create text 2c 4c -text "BIO 008/34" -anchor n
    $c create text 8c 4c -text "LANG 008/35-37" -anchor n
    $c create text 14c 4c -text "MOD REC 008/38" -anchor n
    $c create text 20c 4c -text "SOURCE 008/39" -anchor n


 
    set tablist ""

    for {set m 1} {$m <= 26} {incr m} {
	global entryvalue$m
	if {$m == 6 || $m == 7} {
        	entry $c.entry$m -width 30 -relief sunken \
			-textvariable entryvalue$m
	} else {
        	entry $c.entry$m -width 8 -relief sunken \
			-textvariable entryvalue$m
	}

	set entryvalue$m ""
	lappend tablist $c.entry$m
    }
    set entryvalue1 n
    set entryvalue2 a
    set entryvalue3 m
    set entryvalue4 { }
    set entryvalue5 a

    $c create window 4.8c 0.5c -window $c.entry1 -anchor n \
	-tags field
    $c create window 11c 0.5c -window $c.entry2 -anchor n \
	-tags field
    $c create window 17c 0.5c -window $c.entry3 -anchor n \
	-tags field
    $c create window 23c 0.5c -window $c.entry4 -anchor n \
	-tags field
    $c create window 28.8c 0.5c -window $c.entry5 -anchor n \
	-tags field

    $c create window 7.0c 1.2c -window $c.entry6 -anchor n \
	-tags field
    $c create window 19.2c 1.2c -window $c.entry7 -anchor n \
	-tags field

    $c create window 4.8c 1.9c -window $c.entry8 -anchor n \
	-tags field
    $c create window 11c 1.9c -window $c.entry9 -anchor n \
	-tags field
    $c create window 17c 1.9c -window $c.entry10 -anchor n \
	-tags field
    $c create window 23c 1.9c -window $c.entry11 -anchor n \
	-tags field
    $c create window 28.8c 1.9c -window $c.entry12 -anchor n \
	-tags field

    $c create window 4.8c 2.6c -window $c.entry13 -anchor n \
	-tags field
    $c create window 11c 2.6c -window $c.entry14 -anchor n \
	-tags field
    $c create window 17c 2.6c -window $c.entry15 -anchor n \
	-tags field
    $c create window 23c 2.6c -window $c.entry16 \
        -anchor n -tags field  
    $c create window 28.8c  2.6c -window $c.entry17 -anchor n \
	-tags field

    $c create window 4.8c 3.3c -window $c.entry18 -anchor n \
	-tags field
    $c create window 11c 3.3c -window $c.entry19 -anchor n \
	-tags field
    $c create window 17c 3.3c -window $c.entry20 -anchor n \
	-tags field
    $c create window 23c 3.3c -window $c.entry21 -anchor n \
	-tags field
    $c create window 28.8c 3.3c -window $c.entry22 -anchor n \
	-tags field

    $c create window 4.8c 4c -window $c.entry23 -anchor n \
	-tags field
    $c create window 11c 4c -window $c.entry24 -anchor n \
	-tags field
    $c create window 17c 4c -window $c.entry25 -anchor n \
	-tags field
    $c create window 23c 4c -window $c.entry26 -anchor n \
	-tags field
    
    set textlist "LC_Control_Number {} ISBN {} {} {} \
	System_Control_Number {} {} {} {} {} {} \
	Geographic_Area_Code LC_Call_Number {} \
	Classification_Number {} Personal_Name {} \
	Corporate_Name {} {} Conference_Name {} Uniform_Title {} \
 	Uniform_Title {} {} Title {} {} Edition_Statement {} {} \
	Imprint {} {} projected_pub_date Physical_Description {} {} \
	Personal_Name Corporate_Name Meeting_Name Title Series_Statement \
	{} General_Notes Bibliograhpy_Notes Personal_Name {} Corporate_Name \
	Conference_Name Uniform_Title Topical_Subject {} {} \
	Geographic_Name {} Local_Subject_Heading {} \
	Personal_Name {} Corporate_Name {} Conference/Meeting_Name \
	Uniform_Title Title_Traced_Differently Personal_Name-Title \
	Corporate_Name-Title {} {} {} Conference/Meeting-Title Personal_Name-Title {} \
	{} {} {} {} {} {}"

    set subfielddelimiterlist "a z a c z 6 \
        a a b c d e f \
        a a b \
        a 2 a d \
        a d c a d a d \
        a b c a b c a b c \
        a b c a a b c a a a a a v \
        a a a d a \
        a a a x z \
        a x a x \
        a d a b a\
        a a a \
        a b v t a a v \
        l a e b l q"

    set taglist "010 {} 020 {} {} {} \
        035 039 {} {} {} {} {} \
        043 050 {} \
        082 {} 100 {} \
        110 {} {} 111 {} 130 {} \
        240 {} {} 245 {} {} 250 {} {} \
        260 {} {} 263 300 {} {} 400 410 411 440 490 {} \
        500 504 600 {} 610 \
        611 630 650 {} {} \
        651 {} 690 {} \
        700 {} 710 {} 711 \
        730 740 800 \
        810 {} {} {} 811 830 {} \
        950 {} {} {} 955 {}"


    for {set n 6.2; set m 27; set r 0} \
	{$m <= [expr [llength $taglist]+27+41]} \
		{set n [expr $n+0.7]; incr m; incr r} {

	$c create text 2.5c ${n}c -text [lindex $textlist $r] -anchor n
	$c create text 8c ${n}c -text [lindex $taglist $r] -anchor n \
		-tags t[lindex $taglist $r]


        if {[lindex $taglist $r] != ""} {
		global entryvalue$m
                entry $c.entry$m -width 5 -relief \
                          sunken -textvariable entryvalue$m
                $c create window 9.3c ${n}c -window \
                           $c.entry$m -anchor nw
                           
		lappend tablist $c.entry$m
		incr m
	}

	$c create text 12c ${n}c -text [lindex $subfielddelimiterlist \
		$r] -anchor n 
	

	entry $c.entry$m -width 81 -relief sunken \
		-textvariable entryvalue$m
    	$c create window 13.5c ${n}c -window $c.entry$m -anchor nw
	


	lappend tablist $c.entry$m
    }
    foreach el $tablist {
	bind $el <Tab> "Tab"

    }

    set ttag "" 
    set repeatentrylist ""


    $c bind t100 <1> {lappend ttag 100 2 a d
                      mkRepeatEntry}
    $c bind t110 <1> {lappend ttag 110 3 a d c
			mkRepeatEntry}
    $c bind t111 <1> {lappend ttag 111 2 a d
                      mkRepeatEntry}
    $c bind t130 <1> {lappend ttag 130 2 a d
                      mkRepeatEntry}

    $c bind t240 <1> {lappend ttag 240 1 a
                     mkRepeatEntry}
    $c bind t245 <1> {lappend ttag 245 3 a b c
                      mkRepeatEntry}

    $c bind t250 <1> {lappned ttag 250 3 a b c
                      mkRepeatEntry}

    $c bind t260 <1> {lappend ttag 260 3 a b c
                      mkRepeatEntry}

    $c bind t263 <1> {lappend ttag 263 1 a
                      mkRepeatEntry}

    $c bind t300 <1> {lappend ttag 300 3 a b c
                     mkRepeatEntry}


    $c bind t400 <1> {lappend ttag 400 1 a
                         mkRepeatEntry}

    $c bind t410 <1> {lappend ttag 410 1 a
                     mkRepeatEntry}

    $c bind t411 <1> {lappend ttag 411 1 a
                        mkRepeatEntry}

    $c bind t440 <1> {lappend ttag 440 1 a
                        mkRepeatEntry}

    $c bind t490 <1> {lappend ttag 490 2 a v
                       mkRepeatEntry}



   $c bind t600 <1> {lappend ttag 600 2 a d
                        mkRepeatEntry}

    $c bind t610 <1> {lappend ttag 610 1 a
                        mkRepeatEntry}

    $c bind t611 <1> {lappend ttag 611 1 a
                        mkRepeatEntry}

    $c bind t630 <1> {lappend ttag 630  1 a
                        mkRepeatEntry}

    $c bind t650 <1> {lappend ttag 650 3 a x z
                        mkRepeatEntry}

    $c bind t651 <1> {lappend ttag 651 2 a x
                        mkRepeatEntry}

   $c bind t690 <1> {lappend ttag 690 2 a x
                        mkRepeatEntry}

    $c bind t700 <1> {lappend ttag 700 2 a d
                        mkRepeatEntry}

    $c bind t710 <1> {lappend ttag 710 2 a b
                        mkRepeatEntry}

    $c bind t711 <1> {lappend ttag 711 1 a
                        mkRepeatEntry}

    $c bind t730 <1> {lappend ttag 730 1 a
                        mkRepeatEntry}

    $c bind t740 <1> {lappend ttag 740 1 a
                        mkRepeatEntry}

    $c bind t800 <1> {lappend ttag 800 1 a
                        mkRepeatEntry}

    $c bind t810 <1> {lappend ttag 810 4 a b v t
                        mkRepeatEntry}

    $c bind t811 <1> {lappend ttag 811 1 a
                        mkRepeatEntry}

    $c bind t830 <1> {lappend ttag 830 2 a v
                        mkRepeatEntry}

    $c bind t950 <1> {lappend ttag 950 4 l a e b
                        mkRepeatEntry}

    $c bind t955 <1> {lappend ttag 955 2 l q
                        mkRepeatEntry}


}

proc Tab {} {
	global tablist
	set i [lsearch $tablist [focus]]

	if {$i < 0} {
		set i 0
        } else {
		incr i
		if {$i >= [llength $tablist]} {
			set i 0
		}
	}
	focus [lindex $tablist $i]
}

