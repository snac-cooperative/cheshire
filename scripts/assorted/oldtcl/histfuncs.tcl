#***********************************************************************
#
#        Header Name:    Searchfuncs.tcl
#
#        Programmer:     Alicia Abrahmson
#
#        Purpose:   	Manage user's history, including keeping a
#	list of searches performed, allowing searches to be redone,
#	allowing add-on searches, loading of old history files, and
#	saving history files.               
#
#        Usage:
#
#        Variables:
#
#        Return Conditions and Return Codes:    none
#
#        Date:           11/20/93
#        Revised:
#        Version:        1.0
#        Copyright (c) 1993.  The Regents of the University of
#	California
#                All Rights Reserved
#
#
#***********************************************************************/



# user search history functions
# h_list puts each search into a 2 dimensional array    
# h_save creates a file for the user of search history
# put_h_in_win reads history from 2 dimensional arrays and puts them in
# the history window
# mkRedo makes the redo dialog box
# send_redo sends an already parsed query back to send_zfind and recalls the
# display window

 
proc h_list {tmp_res3 hits} {
	global S_NUM HOST CURHOST HIST H_HITS H_HOST
#	puts stdout $S_NUM
	set hitlist $tmp_res3


	set x $S_NUM

# put all variables relating to history into arrays in memory so they
# can be accessed as needed

	set HIST(1,$S_NUM) $hitlist
	set H_HITS(1,$S_NUM) $hits
	set H_HOST(1,$S_NUM) $CURHOST
	
# puts stdout $HIST
#  h_format $HIST
 }

proc h_add_term {{w .addterm}} {
      catch {destroy $w}
        toplevel $w
        global S_NUM  h_auth h_title h_subj h_cluster
	set index " "
	set term " "
	set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
	message $w.msg -font $bigfont -width 10c \
         -text "Type the number of the search you want to add on to and the term you wish to add. Then select the appropriate tag for your new term from the list below."
 set junk $w.i.entry 

        frame $w.i -bd 2m
        label $w.i.label
        entry $w.i.entry -relief sunken -width 30
        pack append $w.i $w.i.entry right $w.i.label left

        $w.i.label config -font $bigfont -text "Added term:"
        $w.i.entry config -font $medfont -textvariable term
	
	frame $w.j -bd 2m
        label $w.j.label
        entry $w.j.entry -relief sunken -width 5
        pack append $w.j $w.j.label left $w.j.entry left

        $w.j.label config -font $bigfont -text "Search Number:"
        $w.j.entry config -font $medfont -textvariable index

# frame containing checkbuttons to tag the search term

	frame $w.tags -borderwidth 2 -width 50
	pack append $w.tags \
	 [frame $w.tags.left] {left expand} \
         [frame $w.tags.right] {right expand}
	pack append $w.tags.left \
	[checkbutton $w.tags.left.1 -text Author \
	 -variable h_auth -relief flat] \
	{top pady 4 expand frame w} \
	[checkbutton $w.tags.left.2 -text Title -variable h_title \
	-relief flat] \
	{top pady 4 expand frame w}
	pack append $w.tags.right\
	  [checkbutton $w.tags.right.1 -text Subject -variable h_subj \
	-relief flat] \
        {top pady 4 expand frame w} \
	  [checkbutton $w.tags.right.2 -text Any/All -variable h_cluster \
	-relief flat] \
        {top pady 4 expand frame w}
	

 frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
      [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text OK \
        -command "h_send_add_term $w \$index \$term .addterm.tags \
	h_auth h_title h_subj h_cluster" -width 20 ] {top pady 4 expand}

        pack append $w $w.msg {top fill} $w.j {top fillx} \
	$w.i {fillx} $w.tags {fillx} $w.options {bottom fillx}

	
}

proc h_send_add_term {w index term args} {
	catch {destroy $w}
        toplevel $w
        dpos $w
        global S_NUM h_auth h_title h_subj h_cluster HIST medfontb
        set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
        message $w.msg -font $bigfont -width 10c \
            -text "Results of your Added Term Search"
        frame $w.i -bd 2m
        label $w.i.label
        text $w.t -relief raised -width 40 -height 10 -wrap word -setgrid true \
                -font $medfontb
        pack append $w.i $w.t right $w.i.label left


 frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
       [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Display Results" \
        -command "mkDisplay; destroy $w " -width 20 ] {top pady 4 expand}
       pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}

	global  h_auth h_title h_subj h_cluster HIST

	set s_tag(1,0) probsearch
        set s_tag(1,1) author
        set s_tag(1,2) title
        set s_tag(1,3) subject

set x [expr $index-1]
set y 0

# puts stdout "Add a term to search number $index"
# puts stdout "Added term is $term"
# puts stdout "and it corresponds with HISTORY $HIST(1,$x)"

 	foreach i "$h_cluster $h_auth $h_title $h_subj" {
	
		if {$i == 1} {
		set addsearch [concat  $s_tag(1,$y)  = $term]
#		puts stdout "$addsearch"
	}
	incr y

   }
	set newsearch [concat $HIST(1,$x) and $addsearch]
	send_z_search $newsearch $w.t
}



proc h_load {{w .hload} putithere} {
	catch {destroy $w}
        toplevel $w
        dpos $w
        global S_NUM
        global HIST
        global H_HITS
        global HFILE
	set window $putithere
	set filetoload " "
	set tfile " "
        set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
        message $w.msg -font $bigfont -width 10c \
          -text "Load previous History File"
        frame $w.i -bd 2m
        label $w.i.label
        entry $w.i.entry -relief sunken -width 40
        pack append $w.i $w.i.entry right $w.i.label left

        $w.i.label config -font $bigfont -text "Filename:"
        $w.i.entry config -font $medfont -textvariable filetoload


frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
        [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Load File" \
        -command "TextLoadFile  $window \$filetoload; destroy $w" \
 	  -width 20] {top pady 4 expand}
        pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}
# puts stdout "File to load: $filetoload"

}

proc h_save {{w .hsave}} {
      catch {destroy $w}
        toplevel $w
        dpos $w
        global S_NUM
        global HIST
	global H_HITS
	global HFILE
        set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
	set directory [exec pwd]
	set file "/user.his"
	set HFILE [concat $directory$file]
	set temp_hfile $HFILE
#	set i 1

#        for {set x 0} {$x < $S_NUM} {incr x} {
#    		puts $SAVEFILE "$i. $HIST(1,$x)\t $H_HITS(1,$x)"
#		incr i
#	}

        message $w.msg -font $bigfont -width 10c \
            -text "File to save your History list in:"
        frame $w.i -bd 2m
        label $w.i.label
	entry $w.i.entry -relief sunken -width 40
        pack append $w.i $w.i.entry right $w.i.label left

        $w.i.label config -font $bigfont -text "Filename:"
	$w.i.entry config -font $medfont -textvariable HFILE


 frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
        [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Save in Specified File" \
        -command "diff_hfile $temp_hfile; destroy $w" \
		 -width 20] {top pady 4 expand}
        pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}

 #	puts stdout $HFILE 
#	puts stdout $temp_hfile

	if {$HFILE != $temp_hfile} {
#	puts stdout "YOU MUST USE SPECIFIED FILE NAME"
	}
}	

proc diff_hfile {temp_hfile} {

	global HFILE
	set changed_hfile $HFILE
	 if {$HFILE != $temp_hfile} {
#        puts stdout "YOUR FILE NAME IS $changed_hfile"
	put_h_in_file $changed_hfile
        } else {
	put_h_in_file $HFILE     
	}
}
proc put_h_in_file {filename {w .empty}} {
	catch {destroy $w}
	global S_NUM
        global HIST
        global H_HITS
	global HOST
#        puts stdout "S_NUM set to $S_NUM"
        set i 1
	set SAVEFILE [open $filename w+]

	   set i 1
        for {set x 0} {$x < $S_NUM} {incr x} {
                puts $SAVEFILE "$i. $HIST(1,$x)\t$HOST\t\t $H_HITS(1,$x)"
                incr i
        }
	close $SAVEFILE
}

proc put_h_in_win {w} {

	global S_NUM HIST H_HITS H_HOST CURHOST HOST

    if {[tk colormodel $w] == "color"} {
        set bold "-foreground red"
        set normal "-foreground {}"
    } else {
        set bold "-foreground white -background black"
        set normal "-foreground {} -background {}"
    }

	set i 1
	  for {set x 0} {$x < $S_NUM} {incr x} {
	  set currtag $i
          lappend TAGLIST $currtag : # unused?
     insert_H_WithTags $w "$i. $HIST(1,$x)\t $H_HOST(1,$x)\t\t \
	$H_HITS(1,$x)\n" $i 
	incr i
	}

    foreach tag $TAGLIST {
# puts stdout "This is your tag $tag"
    $w tag bind $tag <Any-Enter> "$w tag configure $tag $bold"
    $w tag bind $tag <Any-Leave> "$w tag configure $tag $normal"
    $w tag bind $tag <1> "mkRedo $tag"
    $w tag bind $tag <3> h_add_term
#   bind $w <Any-Enter> "focus $w"
	}
}

proc insert_H_WithTags {w text args} {
	global HIST
	global H_HITS
	global S_NUM

    set start [$w index end]
	   set i 1
    $w insert end $text
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start end
    }
    foreach i $args {
        $w tag add $i $start end
    }
}
proc mkRedo {argc {w .redo}} {
	catch {destroy $w}
	toplevel $w
#	dpos $w
	global NIL $w.i.entry
    	global S_NUM HOST
	global HIST redo_el
	 set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
	  set zhosts {Cheshire2 Melvyl Penn OCLC RLG ERIC MathStat }
	set n [expr $argc-1]
	set redo_el $HIST(1,$n)
	message $w.msg -font $bigfont -width 10c \
	-text "REDO"
	frame $w.i -bd 2m
	label $w.i.label 
        entry $w.i.entry -relief sunken -width 40 
	pack append $w.i $w.i.entry right $w.i.label left

	$w.i.label config -font $bigfont -text "Redo this search?"
	$w.i.entry config -font $medfont -textvariable redo_el
#	$w.i.entry insert 0 $redo_el

     frame $w.zhostf
   message $w.zhostf.msg -font -Adobe-times-medium-r-normal--*-180* \
	-text "Select a Database"
    pack append $w.zhostf \
       [menubutton $w.zhostf.zhostb -text "Hosts" -relief raised \
         -menu $w.zhostf.zhostb.zhostm -width 10 -font $medfont] \
         {top pady 20 padx 20 left}\
       [label $w.zhostf.currhost -textvariable CURHOST -width 10 \
         -font $medfont -relief raised -anchor center] \
         {top pady 20 padx 20 right}\
       [label $w.zhostf.currhosttext -text "Current Host =" \
         -font $medfont] {top pady 20 right}

    menu $w.zhostf.zhostb.zhostm
    foreach i $zhosts {
  $w.zhostf.zhostb.zhostm add radio -label $i -variable $HOST -value $i \
    -command "set CURHOST $i" -font $medfont
    }


 frame $w.options -borderwidth 1

    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
      [button $w.options.left.quit -text CANCEL -command \
	"set redo_el {} ; destroy $w " -width 20] \
	{top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text OK \
      -command "send_redo $n; destroy $w"  -width 20 ] \
	{top pady 4 expand}
	pack append $w $w.msg {top fill} $w.i {top fillx} $w.zhostf \
		{frame center} $w.options {bottom fillx}
	
}




proc send_redo {n {w .redores}} {

        catch {destroy $w}
        toplevel $w
        dpos $w
        global S_NUM
        global HIST
        set medfont "-Adobe-times-medium-r-normal--*-140*"
        set bigfont -Adobe-times-medium-r-normal--*-180*
	set index $n
        message $w.msg -font $bigfont -width 10c \
            -text "Results of your History list search"	
        frame $w.i -bd 2m
        label $w.i.label
	text $w.t -relief raised -width 40 -height 10 -wrap word -setgrid true \
		-font $medfont
        pack append $w.i $w.t right $w.i.label left



 frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
        [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Display Results" \
 	-command "mkDisplay; destroy $w" -width 20 ] {top pady 4 expand}
        pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}

	# set redo [expr $n-1]
	set newzfind $HIST(1,$index)
	send_z_search $newzfind $w.t

}

proc TextLoadFile {w file} {
    set f [open $file]
#	puts stdout "Name of textfile $file"
    $w delete 1.0 end
    while {![eof $f]} {
        $w insert end [read $f 10000]
    }
    close $f
}

