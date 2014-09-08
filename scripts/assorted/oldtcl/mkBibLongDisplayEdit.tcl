# ************************************************************************
#
#       Program Name:   mkBibLongDisplayEdit.tcl
#
#       Programmer:     Sui-hwai Hwang (Sue), Sandia Hsu
#
#       Purpose:        create a full screen MARC record format for
#                       staff to edit the content of a new marc record
#
#
#       Date:           11/07/1993
#       Revised:        11/28/1993
#       Version:
#       Copyright (c) 1992.  The Regents of the University of California
#               All Rights Reserved
#
# ***********************************************************************/
proc mkBibLongDisplayEdit {{w .dis2}} {

	global datalist subfielddelimiterlist leaderfixedfieldtextlist result
	global tablist taglist
	global optiontype recno repeatentrylist ttag
 
	catch {destroy $w}
	toplevel $w

	wm title $w "MARC Record Editing Panel"
	wm geometry $w +0+0

	set optiontype 3 
	set c $w.frame2.c
	set recno 111
	set repeatentrylist ""
	
    frame $w.frame1 -relief raised -bd 2
    frame $w.frame2 -relief raised -bd 2
    frame $w.frame3 -relief sunken -bd 2

   button $w.frame3.ok -text "Edit Me" -command {destroy .dis2
				set result ""
			        mkBibMarcList}
   button $w.frame3.quit -text "Quit to Login Screen" \
                         -command {set result ""
				destroy .dis2
                                focus .nameentry
                                wm deiconify .}

    button $w.frame3.change -text "Change Processing" -command \
                         {set result ""
			 destroy .dis2
                         wm deiconify .aubib}

    pack $w.frame3.ok $w.frame3.change $w.frame3.quit -in $w.frame3 \
         -side left -padx 3c -pady 0.5c

    pack append $w $w.frame1 {top fill} $w.frame2 {top fill \
            expand} $w.frame3 {bottom fill}

    message $w.frame1.m -font -Adobe-Times-Medium-R-Normal-*-180-* \
        -aspect 1200 \
        -text "In this panel, you can edit/add MARC record using the \
                following methods: \n \
                \t1. move your <mouse cursor> to the field to be \
		edited and press <Button 1> to start editing/adding, \n \
                \t2. press <Button 1> on scrollbar, or, \
			press <Button 2> to scroll screens, \n \
                \t3. <tab> through different field entries, \n \
                \t4. click on the <tag> (ex. 600, 650...) \
                 for repeated entries \n \
                \t5. to complete the process, \
		click on the <Edit Me> button on the bottom of screen......"


    pack append $w.frame1 $w.frame1.m {frame center}

    canvas $c -scrollregion {0c 0c 30c 100c} -width 30c -height 20c
    scrollbar $w.frame2.vscroll  -relief sunken -command "$c yview"
    pack append $w.frame2 \
            $w.frame2.vscroll {right filly} $c {expand fill}
    $c config -yscroll "$w.frame2.vscroll set"

    # Display a rectangular grid with four areas divided.

    $c create rect 0c 0c 30c 100c -width 2
    $c create line 0c 5.7c 30c 5.7c -width 2
    $c create line 7c 5.7c 7c 100c -width 2
    $c create line 9c 5.7c 9c 100c -width 2
    $c create line 11c 5.7c 11c 100c -width 2
    $c create line 13c 5.7c 13c 100c -width 2

    set font1 -Adobe-Helvetica-Medium-R-Normal-*-180-*
    set font2 -Adobe-Helvetica-Bold-R-Normal-*-240-*
    if {[tk colormodel $c] == "color"} {
        set blue DeepSkyBlue3
        set red red
        set bisque bisque3
        set green SeaGreen3
    } else {
        set blue black
        set red black
        set bisque black
        set green black
    }
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

    set startrec [lreplace [lindex $result 1] 0 0]
    set fldstart [lreplace $startrec 0 3]
    set leader [lindex [lindex $startrec 0] 2]
    set leadsplit [lrange [split $leader {}] 1 24]

    set leadchk {5 6 7 17 18}
    set tablist ""

    for {set m 1} {$m<=5} {incr m} {
   	global entryvalue$m
        set entryvalue$m [lindex $leadsplit [lindex $leadchk \
		[expr $m-1]]]
        entry $c.entry$m -width 8 -relief sunken -textvar entryvalue$m
        lappend tablist $c.entry$m
    }

    for {set t 1} {$t < 4} {incr t} {
        set d [lindex [lindex $startrec $t] 1]
        if {$d == 001} {
           set field001 [string trim [lindex [lindex $startrec 1] \
		2] "{}"]
           global entryvalue$m
           set entryvalue$m $field001
           entry $c.entry$m -width 30 -relief sunken -textvar \
		entryvalue$m
           lappend tablist $c.entry$m
           incr m
        } elseif {$d == 005} {
           set field005 [string trim [lindex [lindex $startrec 2] 2] \
		"{}"]
           global entryvalue$m
           set entryvalue$m $field005
           entry $c.entry$m -width 30 -relief sunken -textvar \
		entryvalue$m
           lappend tablist $c.entry$m
           incr m
        } elseif {$d == 008} {
           set field008 [string trim [lindex [lindex $startrec 3] 2] \
		"{}"]
           set split008 [split $field008 {}]
           set xelement [llength $split008]
           set i 0

           set fours {7 11 18 24}
           set threes {15 35}
           set others {6 22 23 28 29 30 31 32 33 34 38 39}

           while {$i < $xelement} {
                global entryvalue$m
                if {$i == 0} {
                        set temp1 [string trim [lrange $split008 0 5] \
				"{}"]
                        set entryvalue$m [join $temp1 {}]
                        incr i +5
                }
                if {[lsearch $fours $i] >= 0} {
                        set temp2 [lrange $split008 $i [expr $i+3]]
                        set entryvalue$m [join $temp2 {}]
                        incr i +3
                }
                if {[lsearch $threes $i] >= 0} {
                        set temp3 [lrange $split008 $i [expr $i+2]]
                        set entryvalue$m [join $temp3 {}]
                        incr i +2
                }
                if {[lsearch $others $i] >= 0} {
                set entryvalue$m [lindex $split008 $i]
                }
                set x entryvalue$m
                eval set x \$$x


                entry $c.entry$m -width 8 -relief sunken -textvar \
			entryvalue$m
                lappend tablist $c.entry$m
                incr i
                incr m
          }

      }
    }

    # check if all fixed field entries have been created, if not creat
    #them

    if {$m != 27} {
                while {$m < 27} {
                        if {$m == 6} {
                                global entryvalue$m
                                entry $c.entry$m -width 30 -relief \
                                        sunken -textvar entryvalue$m
                                lappend tablist $c.entry$m
                                incr m
                        } elseif {$m == 7} {
                                global entryvalue$m
                                entry $c.entry$m -width 30 -relief sunken \
                                        -textvar entryvalue$m
                                lappend tablist $c.entry$m
                                incr m
                        } else {
                                while {$m < 27} {
                                        global entryvalue$m
                                        entry $c.entry$m -width 8 \
						-relief sunken \
                                                -textvar entryvalue$m
                                        lappend tablist $c.entry$m
                                        incr m
                                }
                        }
                }
    }


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
    $c create window 23c 2.6c -window $c.entry16 -anchor n \
        -tags field
    $c create window 28.8c 2.6c -window $c.entry17 -anchor n \
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

   set startflds ""
   # get rid of each of the elment number that comes with the fields in
   # the result list from z39.50 or other similar places.

   foreach field $fldstart {
	set eachone [lreplace $field 0 0]
        lappend startflds $eachone
        }

   set editflds ""
   set editflds {{010 {{  }} {{a} {}} {{z} {}}} \
             	{020 {{  }} {{a} {}} {{c} {}} {{z} {}} {{6} {}}} \
             	{035 {{  }} {{a} {}}} \
                {039 {{  }} {{a} {}} {{b} {}} {{c} {}} {{d} {}} {{e} {}} \
			{{f} {}}} \
             	{043 {{  }} {{a} {}}} \
                {050 {{  }} {{a} {}} {{b} {}}} \
             	{082 {{  }} {{a} {}} {{2} {}}} \
             	{100 {{  }} {{a} {}} {{d} {}}} \
             	{110 {{  }} {{a} {}} {{d} {}} {{c} {}}} \
             	{111 {{  }} {{a} {}} {{d} {}}} \
             	{130 {{  }} {{a} {}} {{d} {}}} \
             	{240 {{  }} {{a} {}} {{b} {}} {{c} {}}} \
	     	{245 {{  }} {{a} {}} {{b} {}} {{c} {}}} \
             	{250 {{  }} {{a} {}} {{b} {}} {{c} {}}} \
             	{260 {{  }} {{a} {}} {{b} {}} {{c} {}}} \
             	{263 {{  }} {{a} {}}} \
             	{300 {{  }} {{a} {}} {{b} {}} {{c} {}}} \
             	{400 {{  }} {{a} {}}} \
                {410 {{  }} {{a} {}}} \
                {411 {{  }} {{a} {}}} \
                {440 {{  }} {{a} {}}} \
                {490 {{  }} {{a} {}} {{v} {}}} \
                {500 {{  }} {{a} {}}} \
                {504 {{  }} {{a} {}}} \
                {600 {{  }} {{a} {}} {{d} {}}} \
                {610 {{  }} {{a} {}}} \
                {611 {{  }} {{a} {}}} \
                {630 {{  }} {{a} {}}} \
                {650 {{  }} {{a} {}} {{x} {}} {{z} {}}} \
                {651 {{  }} {{a} {}} {{x} {}}} \
                {690 {{  }} {{a} {}} {{x} {}}} \
                {700 {{  }} {{a} {}} {{d} {}}} \
		{710 {{  }} {{a} {}} {{b} {}}} \
                {711 {{  }} {{a} {}}} \
                {730 {{  }} {{a} {}}} \
                {740 {{  }} {{a} {}}} \
                {800 {{  }} {{a} {}}} \
                {810 {{  }} {{a} {}} {{b} {}} {{v} {}} {{t} {}}} \
                {811 {{  }} {{a} {}}} \
                {830 {{  }} {{a} {}}} \
                {950 {{  }} {{l} {}} {{a} {}} {{e} {}} {{b} {}}} \
                {955 {{  }} {{l} {}}} \

# the last field 999 is a false one to test whether or not the field is added if
# not found in the list.

# Gives the text label associated with each tag....
# Please feel free to add to this list....
   set infoflds ""
   set infoflds {{010 LC_Control_Number} \
                {020 ISBN} \
                {035 System_Control_Number} \
                {043 Geographic_Area_Code} \
                {050 LC_Call_Number} \
                {082 Dewey_Classification_No} \
                {100 Personal_Name} \
                {110 Corporate_Name} \
                {111 Conference_Name} \
                {130 Uniform_Title} \
                {240 Uniform_Title} \
                {245 Title} \
                {250 Edition_Statement} \
                {260 Imprint} \
                {263 Projected_Pub_Date} \
                {300 Physical_Description} \
                {400 Personal_Name} \
                {410 Corporate_Name} \
                {411 Meeting_Name} \
                {440 Title} \
                {490 Series_Statement} \
                {500 General_Notes} \
                {504 Bibliography/Discography_Note} \
                {600 Personal_Name} \
                {610 Corporate_Name} \
                {611 Conference_Name} \
                {630 Uniform_Title} \
                {650 Topical_Subject} \
                {651 Geographic_Name} \
                {690 Local_Subject_Heading} \
                {700 Personal_Name} \
		{710 Corporate_Name} \
                {711 Conference/Meeting_Name} \
                {730 Uniform_Title} \
                {740 Title_Traced_Differently} \
                {800 Personal_Name-Title} \
                {810 Corporate_Name-Title} \
                {811 Conference/Meeting_Title} \
                {830 Personal_Name-title}}

   set newlist ""
   set where 0
   catch {unset fldarray}

   # logical record num

#   set lastfield [lindex $startflds [expr [llength $startflds]-1]]
#puts $lastfield

#   set recnofield [lindex $lastfield 3]
#puts $recnofield

#   set recno [lindex $recnofield 1]
#puts $recno

puts "startflds = $startflds"

   set previoustag ""

   foreach fld $startflds {
      set tag [lindex $fld 0]
      if {$previoustag == $tag} {
		lappend fldarray($tag) $fld
      } else {
      		set fldarray($tag) [list $fld]
      }
      set previoustag $tag 
   }
parray fldarray

   foreach infld $editflds {
      set tag [lindex $infld 0]
      set match [lsearch -exact [array names fldarray] $tag]
      if {$match == -1} {
	      # it is an "empty" field, so replace the field
	      lappend fldarray($tag) $infld
	  }
  }
parray fldarray
   
   set newlist ""

   foreach i [lsort [array names fldarray]] {
       foreach fld $fldarray($i) {
	   lappend newlist $fld
       }
   }


   set taglist ""
   set datalist ""
   set subfielddelimiterlist ""

# for each field in the new list created: put the tag (zeroeth element of the
# field) into the taglist, put the indicator into the datalist, 
# put the subfield label (a,b,c etc.) into subfielddelimiterlist,
# put the data of subfield a into the datalist.

puts "newlist = $newlist"
foreach field $newlist {
        lappend taglist [lindex $field 0]
  	lappend datalist [lindex $field 1]
	lappend subfielddelimiterlist [lindex [lindex $field 2] 0]
	lappend datalist [lindex [lindex $field 2] 1]

# the field has more than 3 elements (the tag, the indicator, and subfield a with
# data), meaning more than just one subfield.
# Add the zeroeth element of each additional subfields 
# (the subfield label a, b, c etc.) to the subfield-
# delimiter list, add the first element of each additional subfields
# into the datalist.   

        if {[llength $field] > 3} {
                for {set x 3} {$x<[llength $field]} {incr x} {
			lappend subfielddelimiterlist [lindex [lindex \
				$field $x] 0]
			lappend datalist [lindex [lindex $field $x] 1]
                        lappend taglist {}
			}
                }
        }

   set textlist ""
   set previoustag ""

   foreach tag $taglist {
	set found 0
	if {$tag != ""} {
# if the tag is not empty, search each piece (infopiece) of information fields
# (infoflds) for the same tag. 
		if {$previoustag == $tag} {
			lappend textlist [lindex $infopiece 1]
			set previoustag $tag
			set found 1
		} else {
			foreach infopiece $infoflds {
				if {[lindex $infopiece 0] == $tag} {
# if the tag is equal to the zeroeth element of the information field,
# set flag to found and get out of checking loop. 
				   set found 1
				   lappend textlist [lindex $infopiece 1]
				   set previoustag $tag
				   break
				}
	 		 }
		}
	}
# if the tag is empty, append an empty element to textlist.
# set found (a flag) to 1 so that the next if would not add another empty element.  
	if {$tag == ""} {
		lappend textlist {}
		set found 1 
	}

	if {$found == 0} {
		lappend textlist {}
	}

    }			
	
    for {set n 6.2; set m 27; set r 0; set countdata 0} {$m <= \
	[expr [llength $newlist]+[llength $taglist]+[llength \
		$leaderfixedfieldtextlist]+1]} \
	{set n [expr $n+0.7]; incr m; incr r; incr countdata} {

# always puts out the text and the tag first .......
	$c create text 2.5c ${n}c -text [lindex $textlist $r] -anchor n
	$c create text 8c ${n}c -text [lindex $taglist $r] -anchor n \
		-tags t[lindex $taglist $r]

# however, if tag is not zero, put in the indicator fields.......
        if {[lindex $taglist $r] != ""} {
		global entryvalue$m
		set entryvalue$m [string trim [lindex $datalist $countdata] "{}"]
                entry $c.entry$m -width 5 -relief sunken -textvar entryvalue$m
                $c create window 9.3c ${n}c -window \
                      $c.entry$m -anchor nw -tags field
		lappend tablist $c.entry$m
		incr m
		incr countdata
	}

# always puts out the subfielddelimiter and the entry ......
	$c create text 12c ${n}c -text [lindex $subfielddelimiterlist \
		$r] -anchor n
	global entryvalue$m
	set entryvalue$m [lindex $datalist $countdata]
	entry $c.entry$m -width 81 -relief sunken -textvar entryvalue$m 
# set x entryvalue$m
# eval set x \$$x
    	$c create window 13.5c ${n}c -window $c.entry$m -anchor nw \
		-tags field
	lappend tablist $c.entry$m
    }
 
    foreach el $tablist {
	bind $el <Tab> "Tab"

    }

    set repeatentrylist ""
    set ttag ""

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

puts "taglist = $taglist"
puts "subfielddelimiter = $subfielddelimiterlist"
puts "textlist = $textlist"
puts "tablist = $tablist"

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

