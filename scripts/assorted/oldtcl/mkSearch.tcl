# ************************************************************************
# 
#        Header Name:    
# 
#        Programmer:     Joachim Martin and Alicia Abrahmson
# 
#        Purpose:        
# 
#        Usage:          
# 
#        Variables:
# 
#        Return Conditions and Return Codes:	none
# 
#        Date:           11/20/93
#        Revised:
#        Version:        1.0
#        Copyright (c) 1993.  The Regents of the University of California
#                All Rights Reserved
# 
# ***********************************************************************/

#
# Search screen
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkSearch {{w .search}} {
    global tabList RES RSTATS SRECS HOST CURHOST S_NUM smfont medfont bigfont \
       smfontb medfontb bigfontb TOPIC sep
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Search"
    wm iconname $w "Search"
    set null " "
    set auth " "
    set query {NULL}
    set zhosts {Cheshire2 Melvyl Penn OCLC RLG ERIC MathStat }
    set CURHOST "Cheshire2"
    set TOPIC ""
    for {set i 0} {$i<3} {incr i} {
       set sep($i) "And"
    }
 
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* \
       -width 5i  -text "Welcome to the search screen.\
         Enter Author, title or subject. Click the \"SEARCH\" \
         button when you're done." -pady 20
	 
    frame $w.zhostf
    pack append $w.zhostf \
       [menubutton $w.zhostf.zhostb -text "Hosts" -relief raised \
         -menu $w.zhostf.zhostb.zhostm -width 10 -font $medfont] \
         {top pady 20 padx 20 left}\
       [label $w.zhostf.currhost -textvariable CURHOST -width 10 \
         -font $medfontb -relief raised -anchor center] \
         {top pady 20 padx 20 right}\
       [label $w.zhostf.currhosttext -text "Current Host =" \
         -font $medfontb] {top pady 20 right} 


    menu $w.zhostf.zhostb.zhostm
    foreach i $zhosts {
    $w.zhostf.zhostb.zhostm add radio -label $i -variable $HOST -value $i \
    -command "set CURHOST $i; checkhost $w.prob; focus .search.f1.entry " -font $medfont
    } 

    frame $w.problabel 
    pack append $w.problabel \
       [label $w.problabel.label -text "Probabilistic search" \
       -font $medfont -width 30] {top expand} 

    frame $w.prob -bd 2m
    message $w.prob.label -width 1i -font $bigfont -text "Topic Words"
    text $w.prob.t -bd 2 -relief sunken -width 66 -font $bigfont \
       -height 3 -wrap word
    bind $w.prob.t <Tab> "Tab \$tabList"
#    bind $w.prob.t <esc> "Tab \$tabList"
    bind $w.prob.t <Return> "makeclock $w; update ; \
       parse_z_search \$auth \$title \$subj $w.help.t $w.results.t \
       ; makenormal $w ; update "
    pack append $w.prob $w.prob.t {padx 40 right}  $w.prob.label right

    frame $w.boollabel -bd 2m
    pack append $w.boollabel \
       [label $w.boollabel.padding -width 6] {top padx 20 pady 20 left} \
       [label $w.boollabel.label -text "Boolean keyword search" \
       -font $medfont -width 30] {top left expand} \
       [menubutton $w.boollabel.sep0 -menu $w.boollabel.sep0.menu \
        -width 6 -relief raised -text "And" -font $medfont ] {top padx 20 right}

    menu $w.boollabel.sep0.menu
       foreach s {And Or Not} {
          $w.boollabel.sep0.menu add radiobutton -command \
           "$w.boollabel.sep0  config -text $s; set sep(0) $s" -label "$s" \
           -value $s -variable sep(0)
       }


    foreach i {f1 f2 f3} {
	frame $w.$i -bd 2m
	label $w.$i.label 
	entry $w.$i.entry -relief sunken -width 53 -font $bigfont
	bind $w.$i.entry <Tab> "Tab \$tabList"
	bind $w.$i.entry <Return> "makeclock $w ; update ;  \
        parse_z_search \$auth \$title \$subj $w.help.t $w.results.t  \
        ; makenormal $w ; update "

	pack append $w.$i $w.$i.entry {padx 40 right} $w.$i.label right
    }

    $w.f1.label config -font $bigfont -text Author
    $w.f2.label config -font $bigfont -text Title 
    $w.f3.label config -font $bigfont -text Subject 

    $w.f1.entry config -textvariable auth 
    $w.f2.entry config -textvariable title
    $w.f3.entry config -textvariable subj

    foreach i {1 2} {
        frame $w.sep$i -bd 2m
        menubutton $w.sep$i.button -menu $w.sep$i.button.menu -width 6 \
        -relief raised -text "And" -font $medfont
        pack append $w.sep$i $w.sep$i.button {top padx 20 right}
    
       menu $w.sep$i.button.menu
       foreach s {And Or Not} {
          $w.sep$i.button.menu add radiobutton -command "$w.sep$i.button config \
           -text $s; set sep($i) $s" -label "$s" -value $s -variable sep($i)
       }
    }


    frame $w.results 
    pack append $w.results \
        [text $w.results.t -relief raised -yscrollcommand "$w.results.s set" \
        -setgrid true -wrap word -bd 2 -width 50 -height 8 \
        -font $bigfont] {left} \
        [scrollbar $w.results.s -relief raised -command "$w.results.t yview" \
        ] {right filly expand}

    frame $w.help
    pack append $w.help \
        [text $w.help.t -relief raised -bd 2 -yscrollcommand "$w.help.s set"\
        -setgrid true -width 30 -height 6 -font $smfont -wrap word] \
        {bottom left} \
        [scrollbar $w.help.s -relief raised -command "$w.help.t yview"] \
         {right filly expand}

    frame $w.options -borderwidth 10
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
        [button $w.options.left.quit -text Quit -command "exit"  -width 20 -font $medfont ] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Display Results" \
        -command { if {[lindex [lindex $RSTATS 2] 1] > 0} {
                    wm iconify .search ; mkDisplay
                 } else {
                  mkDialog .foo {-text " You do not have a result set to \
                   display" -aspect 250} {OK {} Quit {}} } \
                 } \
         -width 40 -font $medfont ]  {top pady 4 expand}

    frame $w.search
    pack append $w.search \
        [button $w.search.button -text SEARCH \
        -command "makeclock $w ; update ;  \
          parse_z_search \$auth \$title \$subj $w.help.t $w.results.t
          ; makenormal $w ; update " \
           -width 20 -font $bigfont] {top pady 30 expand}


# final pack
    pack append $w $w.msg {top fill} $w.zhostf {top fill padx 40} \
     $w.problabel {top fill} $w.prob {top fillx} \
     $w.boollabel {top fill}  $w.f1 {top fillx} $w.sep1 {top fill} \
     $w.f2 {top fillx} $w.sep2 {top fill} $w.f3 {top fillx} \
     $w.search {top fill} $w.options {bottom fillx} \
     $w.help {top pady 20 fill left} $w.results {top fill right} 

    set tabList "$w.prob.t $w.f1.entry $w.f2.entry $w.f3.entry"

    $w.results.t mark set insert 0.0

    bind $w <Control-Return> "mkDisplay" 

    focus $w.prob.t

}

set auth ""
set title ""
set subj ""




# The procedure below is invoked in response to tabs in the entry # windows.  It moves the focus to the next window in the tab list.   TAKEN FROM DEMO
# Arguments:
#
# list -	Ordered list of windows to receive focus

proc Tab {list} {
    set i [lsearch $list [focus]]
    if {$i < 0} {
	set i 0
    } else {
	incr i
	if {$i >= [llength $list]} {
	    set i 0 }
    }
    focus [lindex $list $i]
}

proc makeclock {w} {
   foreach wchild [winfo children $w] {
      $wchild configure -cursor watch 
      foreach wchildchild [winfo children $wchild] {
         $wchildchild configure -cursor watch 
      }
   }
}

proc makenormal {w} {
   foreach wchild [winfo children $w] {
      $wchild configure -cursor top_left_arrow 
      foreach wchildchild [winfo children $wchild] {
         $wchildchild configure -cursor top_left_arrow 
      }
   }
}

proc checkhost {w} {
   global HOST CURHOST

   if {$CURHOST!="Cheshire2"} {
      $w.t delete 0.0 end 
      $w.t config -state disabled 
      $w.label config -foreground grey25
      set tabList "$w.f1.entry $w.f2.entry $w.f3.entry"
      focus .search.f1.entry 
   } else {
      $w.t config -state normal
      $w.label config -foreground black
      set tabList "$w.prob.t $w.f1.entry $w.f2.entry $w.f3.entry"
      focus $w.t 
   }
} 
