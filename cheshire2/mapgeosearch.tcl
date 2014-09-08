#!/home/ray/Work/cheshire/bin/cheshire2
#
# This is an example script showing how the map interface may be
# used for geographical searching of conventional data (not georeferenced)
#
# This version is for LARGE screens...
#
global CITIES
set current_full_display 0
set dtdname /projects/metadata/cheshire/TREC/cheshire_index/TREC.FT.DTD
set textresults {}
set date_start {}
set date_end {}
set search_date_start {}
set search_date_end {}
set start_changed {}
set end_changed {}

proc startdate {start_val} {
    global date_start date_end
    global start_changed

    set endval [.time.cont.end get]
    set date_start [clock format $start_val -format "%B %d, %Y"]
    #puts $date_start

    if {$start_val > $endval} {
	.time.cont.end set $start_val
    }

    if {$start_changed == ""} {
	set start_changed 1
    }

}

proc enddate {end_val} {
    global date_start date_end
    global end_changed

    set startval [.time.cont.start get]
    set date_end [clock format $end_val -format "%B %d, %Y"]
    #puts $date_end

    if {$end_val < $startval} {
	.time.cont.start set $end_val
    }

    if {$end_changed == ""} {
	set end_changed 1
    }

}

map .map -mapfile MAPDATA/map.data -bg black -width 16i  -height 7.5i -detail 3 -aspect 90
pack .map -side top
label .lab -textvar test
pack .lab -side top -expand 1  -fill both
frame .mf -relief raised -borderwidth 3
map .mf.map2 -mapfile MAPDATA/map.data -width 2.75i -height 1.3i -detail 4 -zoom 1.14 -aspect 101 -cursor crosshair
pack .mf.map2 -side top
pack .mf -side top
scale .zoom -label {Zoom Value} -orient horizontal -relief raised -showvalue true -from 1 -to 1000
pack .zoom -side top -expand true -fill x

frame .time
frame .time.labels
label .time.labels.start -textvariable date_start
label .time.labels.end -textvariable date_end
pack .time.labels.start -side left -expand true
pack .time.labels.end -side right -expand true
pack .time.labels -side top -expand true -fill x

frame .time.cont
scale .time.cont.start -orient horizontal -relief raised -showvalue false -command "startdate" -from 673081200 -to 788860800 

scale .time.cont.end -orient horizontal -relief raised -showvalue false -command "enddate"  -from 673081200 -to 788860800 
pack .time.cont.start -side left -expand true -fill x
pack .time.cont.end -side right -expand true -fill x
pack .time.cont -side top -expand true -fill x
pack .time -side top -expand true -fill x
.time.cont.end set 788860800

frame .t
label .t.label -text "Enter place name:"
entry .t.entry -width 50
frame .b
button .b.rdraw -text {Geo Search} -command startrect 
button .b.help -text {Help} -command show_help
button .b.quit -text {Quit} -command exit
pack .t.label -side left 
pack .t.entry -side right -expand true -fill x
pack .t -side top -expand true -fill x

pack .b.rdraw .b.help .b.quit -side left -expand true -fill x
pack .b -side top -expand true -fill x

set test "click below to center and show lat-lon in map"
update
.map display
.mf.map2 display

bind .zoom <B1-Motion> +"dozoom"
bind .zoom <B1-ButtonRelease> +"dozoom"
bind .t.entry <Key-Return> "findplace"


global polylist rectlist rectanchor polyanchor
set polylist {}
set rectlist {}

proc setbindings {} {
    bind .mf.map2 <1> "doloc %x %y"
    bind .map <Shift-Button-1> "makepoly %x %y"
    bind .map <Shift-Button-2> "finishpoly %x %y"
    bind .map <Control-Button-1> "doloc2 %x %y"
    bind .map <Control-Button-2> "showdistance start %x %y"
    bind .map <Control-Button-3> "showdistance end %x %y"
    bind .map <1> "catch {destroy .map.citylabel}; catch {destroy .map.citydot}; %W centerpixel %x %y" 
    bind .map <2> "%W zoom +1.0 ; zoomdetail %W " 
    bind .map <3> "%W zoom -1.0 ; zoomdetail %W " 


}

proc clearbindings {} {
    bind .mf.map2 <1> {}
    bind .map <Shift-Button-1> {}
    bind .map <Shift-Button-2> {}
    bind .map <Control-Button-1> {}
    bind .map <Control-Button-2> {}
    bind .map <Control-Button-3> {}
    bind .map <1> {}
    bind .map <2> {}
    bind .map <3> {}
    bind .map  <B1-ButtonRelease> {}
    bind .map  <B1-Motion> {}

}


proc startrect {} {
    clearbindings
    clearrect
    bind .map <Button-1> "setrectanchor %x %y"
    bind .map  <B1-ButtonRelease> "finishrect %x %y"
    bind .map  <B1-Motion> "drawrubberrect %x %y"
}

proc setrectanchor {x y} {
    global rectlist rectanchor
    set rectlist {}
    set rectanchor [.map getlatlon $x $y]
    lappend rectlist $rectanchor
}

proc drawrubberrect {x y} {
    global rectanchor rectlast rubberID
    set rectlast [.map getlatlon $x $y]
    set rubberID [eval ".map rubberband rectangle white [join $rectanchor] [join $rectlast]"]    
}

proc finishrect {x y} {
    global rectanchor rectlast rubberID
    clearbindings
    .map rubberband end
    make_searchlist $rectanchor $rectlast
    setbindings
}

proc clearrect {} {
    global rubberID
    catch {.map destroy $rubberID}
}

proc doloc {x y} {
    global test
    #clear any labels...
    catch {destroy .map.citylabel}
    catch {destroy .map.citydot}

    set test [.mf.map2 getlatlon $x $y]
    set lati [lindex $test 0]
    set long [lindex $test 1]
    # puts "lat $lati lon $long"
    .map config -latitude $lati -longitude $long
    .map display
    .mf.map2 display
}

proc doloc2 {x y} {
    global test
    set test [.map getlatlon $x $y]
}

proc zoomdetail {w} {
     
     set d [lindex [$w config -detail] 4]

     set z [lindex [$w config -zoom] 4]
		
     .zoom set [expr $z * 10]
			
     if {$z > 8} {
	if {$z > 8 && $z < 15} {
	    if {$d == 2} {
	        return
	    } else {
	    	$w config -detail 2 	    
		#puts "Changing to level 2"
	    }
	}
	if {$z > 15 && $z < 20} {

	   if {$d == 1} {
	      return
	   } else {
	    	$w config -detail 1 	    
		#puts "Changing to level 1"
	   }
	}
        if {$z > 20} {
	   if {$d == 0} {
	      return
	   } else {
	    	$w config -detail 0 	    
		#puts "Changing to level 0"
	   }	
	}

     } else {
       if {$d >= 3} {
       	   return
       } else {	
	   $w config -detail 3 	    
	   #puts "Changing to level 3"
       }
    }
}


proc dozoom {} {

    set zval [.zoom get]
    set fz [expr $zval/10.0]

    if {$fz < 0.8} { .map zoom 0.8} else {
	.map zoom $fz 
	zoomdetail .map
    }
}

proc dodetail {} {
    .map config -detail [.detail get]
}

proc doaspect {} {
    .map config -aspect [.aspect get]
}


proc loadcities {} {
    global CITIES COUNTRIES NAMEDATA
    set f [open /home/ray/Work/cheshire/cheshire2/MAPDATA/cities1]
    
    while {[gets $f x] != -1} {
	lappend CITIES([string toupper [lindex $x 1]]) $x
	lappend COUNTRIES([string toupper [lindex $x 0]]) $x
	lappend NAMEDATA $x
	
    }
    close $f
}



proc placetext {map lat lon color} {
    
}

proc show_help {} {
    toplevel .help
    set w ".help"
    frame $w.buttons
    pack  $w.buttons -side bottom -expand y -fill x -pady 2m
    button $w.buttons.dismiss -text Dismiss -command "destroy $w"
    pack $w.buttons.dismiss  -side top -expand 1
    
    text $w.text -yscrollcommand "$w.scroll set" -setgrid true \
	-width 70 -height 32 -wrap word
    scrollbar $w.scroll -command "$w.text yview"
    pack $w.scroll -side right -fill y
    pack $w.text -expand yes -fill both

# Set up display styles

    $w.text tag configure bold -font -*-Courier-Bold-O-Normal--*-120-*-*-*-*-*-*
    $w.text tag configure big -font -*-Courier-Bold-R-Normal--*-140-*-*-*-*-*-*
    $w.text tag configure verybig -font \
	-*-Helvetica-Bold-R-Normal--*-240-*-*-*-*-*-*
    if {[winfo depth $w] > 1} {
	$w.text tag configure color1 -background #a0b7ce
	$w.text tag configure color2 -foreground red
	$w.text tag configure raised -relief raised -borderwidth 1
	$w.text tag configure sunken -relief sunken -borderwidth 1
    } else {
	$w.text tag configure color1 -background black -foreground white
	$w.text tag configure color2 -background black -foreground white
	$w.text tag configure raised -background white -relief raised \
	    -borderwidth 1
	$w.text tag configure sunken -background white -relief sunken \
	    -borderwidth 1
    }
    $w.text tag configure bgstipple -background black -borderwidth 0 \
	-bgstipple gray25
    $w.text tag configure fgstipple -fgstipple gray50
    $w.text tag configure underline -underline on
    $w.text tag configure overstrike -overstrike on
    $w.text tag configure right -justify right
    $w.text tag configure center -justify center
    $w.text tag configure super -offset 4p \
	-font -Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*
    $w.text tag configure sub -offset -2p \
	-font -Adobe-Courier-Medium-R-Normal--*-100-*-*-*-*-*-*
    $w.text tag configure margins -lmargin1 12m -lmargin2 6m -rmargin 10m
    $w.text tag configure spacing -spacing1 10p -spacing2 2p \
	-lmargin1 12m -lmargin2 6m -rmargin 10m
    

    $w.text insert end "Help Information for Map Demo\n" {verybig color2 color1}

    $w.text insert end "This demo shows some of the features of the "
    $w.text insert end "Map widget" bold
    $w.text insert end ". The Map Widget is used in Tcl programs just like "
    $w.text insert end "any other widget. This demo shows 2 map widgets used "
    $w.text insert end "in quite different ways\n\n"
    $w.text insert end "The Large map widget has a number of mouse bindings...\n\n"
    $w.text insert end "In the normal configuration:\n" big
    $w.text insert end "1. Left Button: " big
    $w.text insert end "Centers the map on the point clicked.\n"
    $w.text insert end "2. Middle Button: " big
    $w.text insert end "Zooms in.\n"
    $w.text insert end "3. Right Button: " big
    $w.text insert end "Zooms out.\n\n"
    $w.text insert end "After selecting one of the object drawing buttons:\n" big
    $w.text insert end "Draw Rectangle" raised 
    $w.text insert end " or "
    $w.text insert end "Draw Polygon" raised 
    $w.text insert end "\n1. Left Button with Drag: \n" big
    $w.text insert end "Draws the outline of the object. "
    $w.text insert end "(Either the entire rectangle, or for each segment "
    $w.text insert end "of a polygon). Note that when drawing a polygon "
    $w.text insert end "you should click first on a single start point "
    $w.text insert end "before dragging the first segment. "
    $w.text insert end "Clicking on the "
    $w.text insert end "Fill Polygon" raised 
    $w.text insert end " button will complete a polygon and fill the area "
    $w.text insert end "outlined with the default transparent stipple pattern \n\n"
    $w.text insert end "The Small Map Widget\n " big
    $w.text insert end "The small map widget is used to center the large "
    $w.text insert end "map widget. This is done by clicking on a point "
    $w.text insert end "with the left mouse button. "
    $w.text insert end "It also shows the latitude and longitude"
    $w.text insert end "of the point clicked above the small map."
    $w.text insert end "\n\n"
    $w.text insert end "The Sliders (scale controls):\n " big
    $w.text insert end "The "
    $w.text insert end "Zoom Value " bold
    $w.text insert end "slider can be used to zoom in and out on the map at "
    $w.text insert end "the current center point. The zoom value is set to "
    $w.text insert end "a fraction (1/10th) of the value shown on the slider. "
    $w.text insert end "Using the mouse button to zoom increments or decrements "
    $w.text insert end "the value of the slider by units of 10. "
    $w.text insert end "If higher zoom values than 400 (40) are needed you can "
    $w.text insert end "use the mouse buttons to move in further.\n\n"
    $w.text insert end "Detail Level " bold
    $w.text insert end "slider is used to control how detailed the drawing will "
    $w.text insert end "be. The default start value is 4, which is OK for global "
    $w.text insert end "views of the data or for zooming with low lag time from "
    $w.text insert end "re-drawing. Once zoomed-in on moving the detail slider "
    $w.text insert end "up to 1 provides the maximum detail.\n\n"
    $w.text insert end "The "
    $w.text insert end "Aspect Control " bold
    $w.text insert end "slider is for tweaking the projection of the map. "
    $w.text insert end "The map is shown using a cylindrical projection such "
    $w.text insert end "that the default value of 100 has things at the "
    $w.text insert end "equator about right, but as you move towards the poles "
    $w.text insert end "the map is stretched out for the projection. Moving this "
    $w.text insert end "slider allows you to change the projection somewhat for "
    $w.text insert end "a more accurate appearance when zoomed in on "
    $w.text insert end "northern or southern regions. "
    $w.text insert end "Note that this "
    $w.text insert end "tweaking (and the projection) is taken into account "
    $w.text insert end "when placing objects or reporting coordinates. \n\n"
    $w.text insert end "The "
    $w.text insert end "Enter Place Name " bold
    $w.text insert end "entry widget lets you enter the name of a country or "
    $w.text insert end "city and if that place is in the database, the map "
    $w.text insert end "will be centered on the location and zoomed to an "
    $w.text insert end "appropriate elevation."
    $w.text insert end ""
    $w.text insert end ""
    $w.text insert end ""
    $w.text insert end ""
    $w.text insert end ""
    $w.text insert end ""



    
}

proc findplace {} {
    global CITIES COUNTRIES test
    
    catch {destroy .map.citylabel}
    catch {destroy .map.citydot}
    set place [string toupper [.t.entry get]]
    if {[catch {set country_data $COUNTRIES($place)} errormsg] == 0} {
	.t.entry delete 0 end
	set numcities 0
	set long 0
	set lati 0
	foreach city $country_data {
	    if {[lindex $city 2] != 0} { 
		incr numcities
		#puts "$city"
		set temp [lindex $city 3]
		set long [expr $long + [lindex $temp 0]]
		set lati [expr $lati + [lindex $temp 1]]
	    }
	}
	set long [expr $long / $numcities]
	set lati [expr $lati / $numcities]
	set test [convert_degrees $long $lati]
	set lati [lindex $test 0]
	set long [lindex $test 1]
	.map config -latitude $lati -longitude $long
	.map zoom 10
	.map display
	.t.entry delete 0 end
	set xy [.map getxy $lati $long]
	
	label .map.citylabel -text [lindex $city 0]
	set wid [string length [lindex $city 0]]

	set xval [expr [lindex $xy 0] - ($wid * 8)]	    	    
	set yval [expr [lindex $xy 1] - 5]	    	    
	
	place .map.citylabel -x $xval -y $yval
	    


    } elseif {[catch {set city_data [lindex $CITIES($place) 0]} errormsg] == 0} {
	set temp [lindex $city_data 3]
	set long [lindex $temp 0]
	set lati [lindex $temp 1]
	set test [convert_degrees $long $lati]
	set lati [lindex $test 0]
	set long [lindex $test 1]
	.map config -latitude $lati -longitude $long
	.map zoom 20
	.map display
	.t.entry delete 0 end
	set xy [.map getxy $lati $long]
	set xval [lindex $xy 0]
	set yval [lindex $xy 1]
	label .map.citylabel -text [lindex $city_data 1]
	label .map.citydot -foreground red -background black -bitmap @x_bitmap.xbm -borderwidth 1
	place .map.citylabel -x [expr $xval + 4] -y [expr $yval + 4]
	place .map.citydot -x $xval -y $yval
	    
    } else {
	if {[file exists cities.netmap]} {
	    if {[catch {set places [split [exec grep "$place" cities.netmap] \n ]} errormsg] == 0} {
		foreach line $places {
		    puts "$line"		
		    if { [regexp -nocase "(^\[0-9\]*\[A-Z\ \]*)(\[0-9-\]*\ *)(\[0-9\]*\ *)(\[0-9\]*\ *)(\[NS\]\ *)(\[0-9\]*\ *)(\[0-9\]*\ *)(\[0-9\]*\ *)(\[EW]\ *)" $line matchvar pname latdeg latmin latsec latns londeg lonmin lonsec lonwe] == 1} {
		    
			set compname [string trim \
					  [string range $pname 0 \
					       [string length $place]]]
			
			if {[string compare $place $compname] == 0} {
			    set lati [format "%do%d'%d''%s" $latdeg $latmin $latsec $latns]
			    set long [format "%do%d'%d''%s" $londeg $lonmin $lonsec $lonwe]
			    .map config -latitude $lati -longitude $long
			    .map zoom 20
			    .map display
			    .t.entry delete 0 end
			    set xy [.map getxy $lati $long]
			    set xval [lindex $xy 0]
			    set yval [lindex $xy 1]
			    label .map.citylabel -text $place
			    label .map.citydot -foreground red -background black \
				-bitmap @x_bitmap.xbm -borderwidth 1
			    place .map.citylabel -x [expr $xval + 4] -y \
				[expr $yval + 4]
			    place .map.citydot -x $xval -y $yval
			    break
			    
			}
		    }
		}
	    } else {
		puts "Location $place not found"
	    }
	}	    
    }
}
	

proc showdistance {type x y} {
    global distx1 disty1 test
    
    if {$type == "start"} {
	set distx1 $x
	set disty1 $y
    } else {
	set distance [.map distance $distx1 $disty1 $x $y]
	set test [format "distance is %s" $distance]
    }
}


# This routine displays the full text version of a TREC record in
# a separate scollable window for reading.

# pFullTextEnter -- Respond to cursor entry of select button i

proc pFullTextEnter {w i} {
    global currFullTextList buttonBG defaultBG current_full_display

    if {$current_full_display != $i} {
	if {[lsearch $currFullTextList $i] < 0} {
	    $w tag configure fullTextTag$i -background $buttonBG
	}
    }
}

# pFullTextLeave -- Respond to cursor leaving select button i

proc pFullTextLeave {w i} {
    global currFullTextList buttonBG defaultBG current_full_display

    if {$current_full_display != $i} {
	if {[lsearch $currFullTextList $i] < 0} {
	    $w tag configure fullTextTag$i -background $defaultBG
	}
    }
}


# pFullText -- create the full text window and controls...

proc pFullText {mainwin i} {
    global dtdname
    global current_full_display
    global textresults 

    set textFont "-adobe-courier-bold-r-normal--17-120-*-*-*-*-*-*"

    if {$i == 0} return

    set record [lindex $textresults $i]

    if {$record == ""} return

    #puts "record is $record"
    set w .twind
    catch {destroy $w}
    
    toplevel $w
    wm title $w "Full Text for Article #$i"
    wm iconname $w "Full Text"
    wm geometry $w +10+10
    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Close -command "set current_full_display 0; destroy $w"
    pack $w.buttons.dismiss -side left -expand 1
    button $w.buttons.select -text Select -command "pFullTextSelect $mainwin $i $w"
    pack $w.buttons.select -side left -expand 1

    button $w.buttons.prev -text "<< Previous Doc" -command "pFullText $mainwin [expr $i - 1]"
    pack $w.buttons.prev -side left -expand 1

    button $w.buttons.next -text "Next Doc >>" -command "pFullText $mainwin [expr $i + 1]"
    pack $w.buttons.next -side left -expand 1
    
    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -font $textFont \
         -width 82 -height 56 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    $t tag configure center -justify center -spacing1 5m -spacing3 5m
    $t tag configure buttons -lmargin1 1c -lmargin2 1c -rmargin 1c \
	    -spacing1 3m -spacing2 0 -spacing3 0


    set txtlines [pAppendROText $t "%s\n" \
	    [zformat long $record SGML -1 80 $dtdname]]

}


#
# pClearROText w -- Clear read-only text window w
#

proc pClearROText {w} {

    $w configure -state normal
    $w delete 1.0 end
    foreach tag [$w tag name] {
	if {"$tag" != "sel"} {
	    $w tag delete $tag
	}
    }
    $w configure -state disabled

    update

}


#
# pAppendROText w s -- Add formatted string s at end of text window w
#   and return the numbers of the beginning and ending lines of new text
#   in the window

proc pAppendROText {w f s} {
    $w configure -state normal
    set start [$w index end]
    $w insert end [format $f $s]
    set end [$w index end]
    $w configure -state disabled
    update

    set pntLoc [string first . $start]
    set startLine [string range $start 0 [expr $pntLoc - 1]]
    set pntLoc [string first . $end]
    set endLine [string range $end 0 [expr $pntLoc - 1]]
    set endLine [expr $endLine - 1]
    return "$startLine $endLine"
}

#
# pTRECtag
#
# tag all non-stopword search terms in TREC records with a special color
# scheme to highlight them.  Similar to pOPACtag, but not nearly as
# nasty.  NOTE -- this function removes stopwords, but does not stem,
# so only exact matches for user's query terms get highlighted.

proc pTRECtag {txtlines i window} {
    global searchTerm probSearchTerm currSearch stoplist defaultPath

    # BUILD TERM LIST
    # 1. get all search terms
    for {set i 0} {$i < [array size searchTerm]} {incr i 1} {
	append OriginalTerms "$searchTerm($i) "
    }
    if {$currSearch == "pr"} {
	append OriginalTerms "$probSearchTerm "
    }

    # 2. Remove stopwords
    # create stopword list if necessary

    if {[llength $stoplist] == 0} {
	set stopfile [open $defaultPath/stoplist r]
	while {[gets $stopfile line] >= 0} {
	    append stoplist "$line "
	}
	close $stopfile
    }
    foreach term $OriginalTerms {
	if {[lsearch -exact $stoplist $term] == -1} {
	    append TermList "$term "
	    # if the term ends in a plural -- trim it...
	    set termlen [string length $term]
	    if {[string last "es" $term] == $termlen - 2} {
		append TermList "[string range $term 0 [expr $termlen - 3]] "
	    } elseif {[string last "s" $term] == $termlen - 1} {
		append TermList "[string range $term 0 [expr $termlen - 2]] "
	    }
	}
    }

    # USE TERM LIST TO HIGHLIGHT OCCURENCE OF TERMS IN RECORD
    # find the starting and ending line of record
    set pntLoc [string first " " $txtlines]
    set startLine [string range $txtlines 0 [expr $pntLoc - 1]]
    set endLine [string range $txtlines [expr $pntLoc + 1] end]

    # for each line in the record,
    #      for each term in the TermList
    #           if the current term occurs on this line, tag
    #           and highlight it
    for {set i $startLine} {$i < $endLine} {incr i} {
	foreach term $TermList {
	    set tmpstring [$window get "$i.0" "$i.0 lineend"]
	    set lineLength [string length $tmpstring]
	    set j 0
	    # Handle case problems
	    set tmpstring [string tolower $tmpstring]
	    set term [string tolower $term]

	    while {$j < $lineLength} {
		if {[set start [string first $term $tmpstring]] != -1} {
		    set tstart [expr {$start + $j}]
		    set tagstart "$i.$tstart"
		    set tend [expr {$tstart + [string length $term]}]
		    set tagend "$i.$tend"
		    $window tag add "TREC.HIGHLIGHT" $tagstart $tagend
		    set j [expr {$tend + 1}]
		    set tmpstring [string range $tmpstring [expr {$start + [string length $term]}] end]
		} else {
		    #stop this nonsense
		    set j $lineLength
		}
	    }
	}
	$window tag configure "TREC.HIGHLIGHT" -background \
	    snow3
    }

}


proc make_searchlist {top bot} {
    global NAMEDATA textresults
    global start_changed end_changed

    set names ""
    set topdeg [latlon_to_degrees [lindex $top 0] [lindex $top 1]]
    set botdeg [latlon_to_degrees [lindex $bot 0] [lindex $bot 1]]
    set toplat [lindex $topdeg 0]
    set toplon [lindex $topdeg 1]
    set botlat [lindex $botdeg 0]
    set botlon [lindex $botdeg 1]

    foreach place $NAMEDATA {
	set plon [lindex [lindex $place 3] 0]
	set plat [lindex [lindex $place 3] 1]
	if {$plat <= $toplat && $plat >= $botlat} {
	    # in the right latitude range 
	    if {$plon >= $toplon && $plon <= $botlon} {
		#got one
		#puts -nonewline "Country: [lindex $place 0] :: "
		#puts "City: [lindex $place 1]"
		append names [lindex $place 0] " " [lindex $place 1] " "
	    }
	}
	
    }

    #puts "Search Names are: $names"
    if {$names == ""} {
	puts "No names found in that area"
	    tk_messageBox -icon info -message "No placenames in the gazetteer for that area." -type ok -parent .map

    } else {
	set searchkey [format "{%s}" $names]
	set datepart {}
	if {$start_changed == 1 || $end_changed == 1} {
	    set search_date_start [clock format [.time.cont.start get] -format "%y%m%d"]
	    set search_date_end [clock format [.time.cont.end get] -format "%y%m%d"]
	    if {$search_date_start == $search_date_end} {
		set datepart [format "and date %s" $search_date_start]
	    } else {
		set datepart [format "and date >= %s and date <= %s" $search_date_start $search_date_end]
	    }
	
	    # puts "datepart is '$datepart'"
	    
	}
	
	puts "NAMES are: '$searchkey'"
	set results [eval zfind country @ $searchkey $datepart]
	#puts $results
	set hits [lindex [lindex [lindex $results 0] 2] 1]
	#puts "Hits $hits"
	if {$hits > 0} {
	    set textresults [zdisplay 50 1]

	    pFullText . 1
	} else {
	    tk_messageBox -icon info -message "No Matches for \"$names\" found in the database." -type ok -parent .map
	}

    }
    
    
}



# attach cities and set up default bindings
loadcities
setbindings

# set up the connection to the TREC DB
zselect xxx otlet.sims.berkeley.edu trec 2100
zset recsyntax sgml

