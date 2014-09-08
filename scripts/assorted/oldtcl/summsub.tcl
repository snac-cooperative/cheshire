# ************************************************************************
# 
#        Header Name: 	summsub.tcl   
# 
#        Programmer:     Joachim Martin 
# 
#        Purpose:       Summarrize subjects from a set of marc records
#			encoded as a tcl list and send them to a text
#			widget 
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

proc uniqsub {r w} {

   global RELSUBS sortedlist
   set bold "-foreground red"
   set normal "-foreground {}"
   set sortedlist ""

#   set fname tmp[pid]
   set fid [open SUMSUB.TXT w+]

   # get result stats
   set tmp [lindex $r 0]
   
   # get number of recs in current set
   set n [lindex [lindex $tmp 3] 1]

   # get hits
   set h [lindex [lindex $tmp 2] 1]

   set subject ""
   set sublist(Total_Headings) 0

   for {set i 0} {$i < $n} {incr i} {
      # increment counter 1 to skip over status info
      set rec [lindex $r [expr $i + 1]]

      foreach field $rec {
         set fieldnum [lindex $field 0]
         set tag [lindex $field 1]
         if {(($tag>=600) && ($tag<700))} {
            set numsubfields [llength $field]
            for {set s 3} {$s < $numsubfields} {incr s} {
               if {$subject=="" && [expr $s + 1] != $numsubfields} {
                  set subject "[lindex [lindex $field $s] 1]--"
               } elseif {$subject!="" && [expr $s + 1] < $numsubfields} {
                  set subject "$subject[lindex [lindex $field $s] 1]--"
               } else {
                  set subject "$subject[lindex [lindex $field $s] 1]"
               }
            }
            if {[info exists sublist($subject)]} {
               incr sublist($subject) 
            } else {
	       set sublist($subject) 1
            }
            incr sublist(Total_Headings)  
         }
         set subject ""
      }
   }
   set tmp [$w get 0.0 end]
   $w delete 0.0 end

   foreach sub [array names sublist] {
      if {$sublist($sub)>0} {
         puts $fid "$sublist($sub)      $sub"
      }
   }

   close $fid
   exec sort +0nr -1 +1d SUMSUB.TXT > SUMSUB.TXT2
   set fid [open SUMSUB.TXT2 r]

   set i 0
   while {[gets $fid line] >= 0} {
      set tag rs[incr i]
      $w tag delete $tag
      $w tag bind $tag <1> "RelSub {$line} $w $tag" 
      $w tag bind $tag <Any-Enter> "$w configure -cursor hand2; \
         $w tag configure $tag $bold "
      $w tag bind $tag <Any-Leave> "$w configure -cursor \
         top_left_arrow; $w tag configure $tag $normal"
      lappend sortedlist $line
   }

   $w delete 0.0 end

   set i 0
   foreach line $sortedlist {
      set tag rs[incr i]

      set subject "[lrange $line 1 end]"
     
      set index [lsearch -exact $RELSUBS "$subject"]
      if {$index==-1} {
         $w tag configure $tag -font -adobe-times-medium-r-normal-*-180-*
      } else {
         $w tag configure $tag -font -adobe-times-medium-i-normal-*-180-*
      } 
      insertWithTags $w "$line" $tag
      $w insert end \n
   }

   exec /bin/rm -f ~/TK3.2/SUMSUB.TXT ~/TK3.2/SUMSUB.TXT2

} 


proc RelSub {subject w tag} {
   global RELSUBS
   
   set subject "[lrange $subject 1 end]"
   
   set index [lsearch $RELSUBS "$subject"]
   if {$index >= 0} {
      $w tag configure $tag -font -adobe-times-medium-r-normal-*-180-*
      set RELSUBS [lreplace $RELSUBS $index $index ]
   } else {
      $w tag configure $tag -font -adobe-times-medium-i-normal-*-180-*
      lappend RELSUBS $subject
   }   
 
}

proc SumRelBack {w} {
   global RELSUBS

   if {[llength $RELSUBS]==0} {
      mkDialog .foo {-text " You have not marked ny subjects as \
         being relevant!" -aspect 250} {OK {} Quit {}}
      return
   }

   set search "SUBJECT = "

   foreach sub $RELSUBS {
      regsub -all (\\(|\\)|-|:|,|/) $sub  " " tmpsub
      regsub -all "  " $tmpsub  " " newsub
      append search "{$newsub}"
      set succ [lsearch $RELSUBS "$sub"]

# check to see if current subject is NOT the last sub
      if {[expr 1 + [lsearch $RELSUBS "$sub"]] < [llength $RELSUBS]} {
      append search " AND SUBJECT = "
      }
   }
   makeclock $w
   update
   SendRelSubs $search 
   makenormal $w
   update
}


proc SendRelSubs {query {w .sendrelsubsresult}} {
        global RES

        catch {destroy $w}
        toplevel $w
#        dpos $w
        global smfont medfont bigfont

        message $w.msg -font $bigfont -width 10c \
            -text "Results of your Subject Relevance Feedback"
        frame $w.i -bd 2m
        text $w.t -bd 2 -relief raised -width 40 -height 10 -wrap word \
           -setgrid true -font $medfont 
        pack append $w.i $w.t bottom 

 frame $w.options -borderwidth 1
    pack append $w.options \
       [frame $w.options.left] {left expand} \
       [frame $w.options.right] {right expand}
    pack append $w.options.left \
        [button $w.options.left.quit -text CANCEL -command "destroy $w" \
        -width 20] {top pady 4 expand}
    pack append $w.options.right \
        [button $w.options.right.display -text "Display Results" \
         -command {destroy .sendrelsubsresult; destroy .verify ; \
          myprintrec RES {$RES} .display.t } -width 20 ] {top pady 4 expand}

        pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}
        send_z_search $query $w.t

}

proc VerifyRelSubs {{w .verify}} {
   global smfont medfont bigfont RES RELSUBS

   catch {destroy $w}
   toplevel $w


   message $w.msg -font $bigfont -width 10c \
       -text "Please verify the these subjects"
   frame $w.i -bd 2m
   text $w.t -bd 2 -relief raised -width 60 -height 10 -wrap word \
      -setgrid true -font $bigfont
   pack append $w.i $w.t bottom

   frame $w.options -borderwidth 1
      pack append $w.options \
         [frame $w.options.left] {left expand} \
         [frame $w.options.right] {right expand}
      pack append $w.options.left \
          [button $w.options.left.quit -text CANCEL -command "destroy $w ; \
            uniqsub {$RES} .display.t"  -width 20] {top pady 4 expand}
      pack append $w.options.right \
          [button $w.options.right.send -text "Send Query" \
           -command "SumRelBack .verify" \
            -width 20 ] {top pady 4 expand}

        pack append $w $w.msg {top fill} $w.i {top fillx} \
                $w.options {bottom fillx}



   for {set i 0} {$i<[llength $RELSUBS]} {incr i} {
      set tag drs$i
      $w.t tag bind $tag <1> "DelRelSub $i $w"
#      $w.t tag bind $tag <Any-Enter> "$w.t configure -cursor hand2; \
#         $w.t tag configure $tag -background salmon1 "
#      $w.t tag bind $tag <Any-Leave> "$w.t configure -cursor \
#         top_left_arrow; $w.t tag configure $tag -background { }"
      $w.t tag configure $tag -relief raised 
   }

   set i -1
   foreach subject $RELSUBS {
      set tag drs[incr i]

      insertWithTags $w.t " Remove " drs$i
      insertWithTags $w.t { }
      $w.t insert end "     $subject"
      $w.t insert end \n
      

   }

}


proc DelRelSub {n w} {
   global RELSUBS

   set RELSUBS [lreplace $RELSUBS $n $n]

   $w.t delete 0.0 end

set i -1
   foreach subject $RELSUBS {
      set tag drs[incr i]

      insertWithTags $w.t " Remove " raised drs$i
      insertWithTags $w.t ""
      $w.t insert end "     $subject"

   }
}

 
