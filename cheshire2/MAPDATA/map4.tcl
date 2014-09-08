global CITIES
map .map -mapfile map.data -bg black -width 8i  -height 5i -detail 4 -aspect 110
pack .map -side top
label .lab -textvar test
pack .lab -side top -expand 1  -fill both
frame .mf -relief raised -borderwidth 3
map .mf.map2 -mapfile map.data -bg gray -width 2.75i -height 1.3i -detail 4 -zoom 1.14 -aspect 101 -cursor crosshair
pack .mf.map2 -side top
pack .mf -side top
scale .zoom -label {Zoom Value} -orient horizontal -relief raised -showvalue true -from 1 -to 400
pack .zoom -side top -expand true -fill x
scale .detail -label {Detail Level} -orient horizontal -relief raised -showvalue true -from 1 -to 5
pack .detail -side top -expand true -fill x
scale .aspect -label {Aspect control} -orient horizontal -relief raised -showvalue true -from 50 -to 150
pack .aspect -side top -expand true -fill x
frame .t
label .t.label -text "Enter place name:"
entry .t.entry -width 50
frame .b
button .b.rdraw -text {Draw Rectangle} -command startrect 
button .b.mdraw -text {Draw Polygon} -command startpoly
button .b.finish -text {Fill Polygon} -command finishpoly
button .b.help -text {Help} -command show_help
button .b.quit -text {Quit} -command exit
pack .t.label -side left 
pack .t.entry -side right -expand true -fill x
pack .t -side top -expand true -fill x

pack .b.rdraw .b.mdraw .b.finish .b.help .b.quit -side left -expand true -fill x
pack .b -side top -expand true -fill x

set test "click below to center and show lat-lon in map"
update
.map display
.mf.map2 display

bind .zoom <B1-Motion> +"dozoom"
bind .zoom <B1-ButtonRelease> +"dozoom"
bind .detail  <B1-ButtonRelease> +"dodetail"
.detail set 4
bind .aspect  <B1-ButtonRelease> +"doaspect"
bind .aspect  <B1-Motion> +"doaspect"
.aspect set 110
bind .t.entry <Key-Return> "findplace"


global polylist rectlist rectanchor polyanchor
set polylist {}
set rectlist {}


proc setbindings {} {
    bind .mf.map2 <1> "doloc %x %y"
    bind .map <Control-Button-1> "doloc2 %x %y"
    bind .map <Control-Button-2> "showdistance start %x %y"
    bind .map <Control-Button-3> "showdistance end %x %y"
    bind .map <1> "catch {destroy .map.citylabel}; catch {destroy .map.citydot}; %W centerpixel %x %y" 
    bind .map <2> "%W zoom +1.0 ; inczoom" 
    bind .map <3> "%W zoom -1.0 ; deczoom" 


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
    do_city_search  $rectanchor $rectlast
    setbindings
}

proc do_city_search {bbtop bbend} {

  puts "$bbtop $bbend"

  set query "search cityllstr >#< $bbtop $bbend"
  set results [eval $query]

  foreach rec $results {

puts "RESULT IS $rec"
    foreach line $rec {

        if {[regexp -nocase {(<latlongstr> )([^<]*)(</latlongstr>$)} $line all junk1 coord junk2] == 1} {
            #puts $coord
            .map create point violet [lindex $coord 0] [lindex $coord 1]
        }
    } 
  }

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
}

proc doloc2 {x y} {
    global test
    set test [.map getlatlon $x $y]
}


proc startpoly {} {
    clearbindings
    bind .map <Button-1> "setpolyanchor %x %y"
    bind .map  <B1-ButtonRelease> "finishpolyseg %x %y"
    bind .map  <B1-Motion> "drawrubberpoly %x %y"
    set polylist {}
}


proc setpolyanchor {x y} {
    global polylist polylast polyanchor rubberID
    if {[llength $polylist] == 0} {
	set polyanchor [.map getlatlon $x $y]
	makepoly $x $y
	set rubberID [eval ".map rubberband line white [join $polyanchor] [join $polyanchor]"]
    } else {

	set polylast [.map getlatlon $x $y]
	set rubberID [eval ".map rubberband line white [join $polyanchor] [join $polylast]"]
    }
}

proc finishpolyseg {x y} {
    global polyanchor
    .map rubberband end
    set polyanchor [.map getlatlon $x $y]
    makepoly $x $y
}
proc drawrubberpoly {x y} {
    global polyanchor polylast rubberID
    set polylast [.map getlatlon $x $y]
    set rubberID [eval ".map rubberband line white [join $polyanchor] [join $polylast]"]

}

proc makepoly {x y} {
    global polylist test
    set test [.map getlatlon $x $y]
    lappend polylist $test

}


proc finishpoly {} {
    global polylist test

    clearbindings

    # close the polygon
    lappend polylist [lindex $polylist 0]

    set size [llength $polylist]

    if {$size >= 4} {
	eval ".map create polygon white [join $polylist]"
	set polylist {}
    } else {
	puts "not enough points for polygon"
	set polylist{}
    }

    set polylist {}
    setbindings    
}

proc inczoom {} {
    set x [.zoom get]
    incr x 10
    .zoom set $x
}

proc deczoom {} {
    set x [.zoom get]
    incr x -10
    .zoom set $x
}

proc dozoom {} {

    set zval [.zoom get]
    set fz [expr $zval/10.0]

    if {$fz < 0.8} { .map zoom 0.8} else {
	.map zoom $fz 
    }
}

proc dodetail {} {
    .map config -detail [.detail get]
}

proc doaspect {} {
    .map config -aspect [.aspect get]
}


proc loadcities {} {
    global CITIES COUNTRIES
    set f [open cities1]
    
    while {[gets $f x] != -1} {
	lappend CITIES([string toupper [lindex $x 1]]) $x
	lappend COUNTRIES([string toupper [lindex $x 0]]) $x
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
    $w.text insert end "Zooms out.\n"

    $w.text insert end "4. CNTL-Left Button: " big
    $w.text insert end "Shows the Latitude and Longitude of the \n"
    $w.text insert end "      point clicked. Shown above small map widget.\n"
    $w.text insert end "5. CNTL-Middle Button: " big
    $w.text insert end "Start of distance measurement.\n"
    $w.text insert end "6. CNTL-Right Button: " big
    $w.text insert end "End and Calculate distance measurement \n"
    $w.text insert end "      Result shown above small map widget.\n\n"

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
		puts "$city"
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
	set test [format "distance is %s miles" $distance]
    }
}

# attach cities and set up default bindings
loadcities
setbindings

