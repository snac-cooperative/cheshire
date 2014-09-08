# ************************************************************************
# 
#        Header Name:    mkDisplay.tcl
# 
#        Programmer:     Joachim Martin and Alicia Abrahmson
# 
#        Purpose:        Set of functions that handle the creation of the
# 			display screen.
# 
#        Usage:          
# 
#        Variables:
# 
#        Return Conditions and Return Codes: 	none
# 
#        Date:           11/20/93
#        Revised:
#        Version:        1.0
#        Copyright (c) 1993.  The Regents of the University of California
#                All Rights Reserved
# 
# ************************************************************************/


#
# Create a top-level window containing a text widget that allows you
# to load a file and highlight all instances of a given string.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkDisplay {{w .display}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    
    wm title $w "Display"
    wm iconname $w "Display"
    
    set mtags {001 005 008 100 245 260 300}
    global RES SRECS RSTATS RELSUBS format smfont medfont bigfont
    set format SHORT
    set more "NULL"

    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -aspect 300 \
            -text "Use these buttons to control your display."

    frame $w.frame -borderwidth 3 -width 20 -relief raised
    pack append $w.frame \
        [frame $w.frame.res_oneclick ] {top padx 10 pady 20} \
        [button $w.frame.b2 -text "  Show History  " -command "mkHistory" \
         -font $medfont] {top pady 20 padx 10 } \
        [button $w.frame.b3 -text "  Getmore  " -command "getmore 5 $w.t " \
         -font $medfont] {top pady 20 } \
        [button $w.frame.b4 -text "  New Search  " -command " destroy $w; 
          wm deiconify .search" -font $medfont] {top pady 20 } \
        [button $w.frame.b7 -text "Summarize Subjects" -command \
         {uniqsub $RES .display.t} -font $medfont] {top pady 20 } \
        [button $w.frame.b9 -text "SumRelBack" -command \
         {VerifyRelSubs} -font $medfont] {top pady 20 } \
        [button $w.frame.b5 -text "  RelBack  " -font $medfont \
        -command {RelInfo} ]  {top pady 20 } \
        [button $w.frame.b6 -text "  Show RES var  " -font $medfont  \
        -command ".display.t insert end {$RES}"]  {top pady 20 } \
        [frame $w.frame.save_oneclick ] {top padx 10 pady 20} 

    pack append $w.frame.res_oneclick \
       [menubutton $w.frame.res_oneclick.format -text " Display Format " \
        -relief raised -menu $w.frame.res_oneclick.format.menu -width 19 \
        -borderwidth 4] {top expand} \
       [frame $w.frame.res_oneclick.top] {top expand} \
       [frame $w.frame.res_oneclick.bottom] {bottom expand} 
    pack append $w.frame.res_oneclick.top \
       [button $w.frame.res_oneclick.short -width 5 -text Short -command \
        {set format SHORT ; myprintrec RES $RES .display.t}] {top left} \
       [button $w.frame.res_oneclick.medium -width 7 -text Medium -command \
        {set format MEDIUM ; myprintrec RES $RES .display.t}] {top left} \
       [button $w.frame.res_oneclick.long -width 4 -text Long -command \
        {set format LONG ; myprintrec RES $RES .display.t}] {top left} 
    pack append $w.frame.res_oneclick.bottom \
       [button $w.frame.res_oneclick.marc -text "Tagged Marc" -command \
       {set format MTAG; myprintrec RES $RES .display.t} -width 18] {top expand}


    menu $w.frame.res_oneclick.format.menu -font $medfont
    $w.frame.res_oneclick.format.menu add radio -label "Short citation" \
     -variable format -value SHORT -command {myprintrec RES $RES .display.t} 
    $w.frame.res_oneclick.format.menu add radio -label "Medium citation" \
     -variable format -value MEDIUM -command {myprintrec RES $RES .display.t}
    $w.frame.res_oneclick.format.menu add radio -label "Long citation" \
     -variable format -value LONG -command {myprintrec RES $RES .display.t}
    $w.frame.res_oneclick.format.menu add radio -label "Long Tagged Marc" \
     -variable format -value MTAG -command {myprintrec RES $RES .display.t}

    pack append $w.frame.save_oneclick \
       [menubutton $w.frame.save_oneclick.format -text "Display Saved recs " \
        -relief raised -menu $w.frame.save_oneclick.format.menu -width 19 \
         -borderwidth 4] {top expand} \
       [frame $w.frame.save_oneclick.top] {top expand} \
       [frame $w.frame.save_oneclick.bottom] {bottom expand}
    pack append $w.frame.save_oneclick.top \
       [button $w.frame.save_oneclick.short -width 5 -text Short -command \
        {set format SHORT ; myprintrec SRECS $SRECS .display.t}] {top left} \
       [button $w.frame.save_oneclick.medium -width 7 -text Medium -command \
        {set format MEDIUM ; myprintrec SRECS $SRECS .display.t}] {top left} \
       [button $w.frame.save_oneclick.long -width 4 -text Long -command \
        {set format LONG ; myprintrec SRECS $SRECS .display.t}] {top left}
    pack append $w.frame.save_oneclick.bottom \
       [button $w.frame.save_oneclick.marc -text "Tagged Marc" -command \
       {set format MTAG; myprintrec SRECS $SRECS .display.t} -width 18] \
       {top expand}


   menu $w.frame.save_oneclick.format.menu -font $medfont
   $w.frame.save_oneclick.format.menu add radio -label "Short citation" \
    -variable format -value SHORT -command {myprintrec SRECS $SRECS .display.t}
   $w.frame.save_oneclick.format.menu add radio -label "Medium citation" \
    -variable format -value MEDIUM -command {myprintrec SRECS $SRECS .display.t}
   $w.frame.save_oneclick.format.menu add radio -label "Long citation" \
    -variable format -value LONG -command {myprintrec SRECS $SRECS .display.t}
   $w.frame.save_oneclick.format.menu add radio -label "Long Tagged Marc" \
    -variable format -value MTAG -command {myprintrec SRECS $SRECS .display.t}


    text $w.t -bd 2 -yscrollcommand "$w.s set" -relief raised\
    -wrap word -setgrid true -height 32 -width 66 -font $bigfont

    scrollbar $w.s -relief raised -command "$w.t yview" 
    $w.s configure -command "$w.t yview"

    frame $w.options -relief raised 
    pack append $w.options \
      [button $w.return -text "Return to Searching" -width 22 \
      -command "destroy $w; wm deiconify .search" -font $bigfont \
       -background salmon1] {top padx 20 pady 20 left } \
      [label $w.recno -width 56 -relief raised -font $medfont] \
      {top left expand} \
      [button $w.quit -text "QUIT Cheshire2" -width 18 -font $bigfont \
       -command exit -background salmon1] {top padx 20 right}

    pack append $w $w.options {top fillx pady 10} \
       $w.frame {left filly} $w.s {right filly} $w.t {expand fill}


    $w.t mark set insert 0.0

    set format SHORT
    set saveinfo [format "%d records saved in your list" [expr [llength $SRECS] - 1]]
    set dispinfo [format " %30s " $saveinfo]
    $w.recno config -text "$dispinfo"
    myprintrec RES $RES $w.t 
}

proc AttTypesScroll index {
   .display.t yview $index; .display.bibinfo yview $index
}



# showVars w var var var ...
#
# Create a top-level window that displays a bunch of global variable values
# and keeps the display up-to-date even when the variables change value
#
# Arguments:
#    w -        Name to use for new top-level window.
#    var -      Name of variable to monitor.

proc showVars {w args} {
    catch {destroy $w}
    toplevel $w
    wm title $w "Variable values"
    label $w.title -text "Variable values:" -width 20 -anchor center \
            -font -Adobe-helvetica-medium-r-normal--*-180*
    pack append $w $w.title {top fillx}
    foreach i $args {
        frame $w.$i
        label $w.$i.name -text "$i: "
        label $w.$i.value -textvar $i
        pack append $w.$i $w.$i.name left $w.$i.value left
        pack append $w $w.$i {top frame w}
    }
    button $w.ok -text OK -command "destroy $w"
    pack append $w $w.ok {bottom pady 2}
}
