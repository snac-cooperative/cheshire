
# relback = extract title and subject words for relevance feedback

# r = tcl variable that holds the relevant records
# 



proc relback {r w} {
global RES
   if {$r==""} {
      return
   }
set relwords ""
   # get result stats
   set tmp [lindex $r 0]

   # this is initial res, get number of recs in current set
   set tmp2 [lindex $tmp 3]
   set n [lindex $tmp2 1]

   # get hits
   set tmp3 [lindex $tmp 2]
   set h [lindex $tmp3 1]
   
   for {set i 0} {$i < $n} {incr i} {
# increment counter 1 to skip over status info
      set curr [expr $i + 1]
      set rec [lindex $r $curr]

      set rec [lrange $rec 1 end]
      foreach field $rec {
         set fieldnum [lindex $field 0]
         set tag [lindex $field 1]
         if {($tag==245) || ($tag>=600 && $tag<700)} {
            set tmpind [lindex $field 2]
            set indic [lindex $tmpind 0]
            set tmpnum [llength $field]
            set numsubfields [expr $tmpnum + 1]
            for {set s 3} {$s < $numsubfields} {incr s} {
               set subfield [lindex $field $s]
               set subfielddelim [lindex $subfield 0]
               set subfieldval [lindex $subfield 1]
               if {(($tag=="245") && ($subfielddelim=="a" || \
                    $subfielddelim=="b"))} {
                   append relwords "$subfieldval "
               } elseif {$tag>=600 && $tag<700} {
                   append relwords "$subfieldval "
               } 
            }
         }
      } 
   }

   regsub -all (\\.|\\(|\\)|-|:|/) $relwords " " newrelwords
   set newrelwords [string tolower $newrelwords]


   foreach word $newrelwords {
      if {[info exists wordlist($word)]} {
         incr wordlist($word)
      } else {
         set wordlist($word) 1
      }
   }
   set topicwords "TOPIC = \{"
   foreach word [array names wordlist] {
      append topicwords "$word "
   }
   set topicwords [string trimright $topicwords]

   append topicwords "\}"
   send_z_search $topicwords .relinfo.t 
} 


proc RelInfo {{w .relinfo}} {

   global RES RELRECS smfont medfont bigfont

   catch {destroy $w}
   toplevel $w
#   dpos $w

   message $w.msg -font $bigfont -width 10c \
       -text "Please verify these records as relevant"

   frame $w.i -bd 2m
   text $w.t -bd 2 -relief raised -width 60 -height 10 -wrap word \
      -setgrid true -font $medfont
   pack append $w.i $w.t bottom

   frame $w.options -borderwidth 1
      pack append $w.options \
          [button $w.options.quit -text CANCEL -command "destroy $w "  \
           -width 20] {top pady 4 expand} \
          [button $w.options.send -text "Send Query"  -command \
             {.relinfo.t delete 0.0 end ; relback $RELRECS .junk; \
              .relinfo.options.send configure -command "mkDisplay" \
              -text "Display Records"; .relinfo.msg configure \
              -text "Results of your relevance feedback"} -width 20 ] \
             {top pady 4 expand}

   pack append $w $w.msg {top fill} $w.i {top fillx} $w.options {bottom fillx}

   PrintRelRecs $w.t

}

proc PrintRelRecs {w} {
    global RELRECS 

   set n "[lindex [lindex [lindex $RELRECS 0] 3] 1]"
   # get hits
   set h "[lindex [lindex $RELRECS 0] 2] 1]"
   # set up tag generation
   set starttag 1
   set endtag $n
     
   for {set i $starttag; set x 1} {$i <= $endtag} {incr i;incr x} {      
      set tag drr$i
      $w tag delete $tag
      set tag drr$i
      set badrec {[lindex $r $x]}
      $w tag config $tag -relief raised
      $w tag bind $tag <Any-Enter> "$w configure -cursor hand2; \
       $w tag configure $tag -foreground salmon1 "
      $w tag bind $tag <Any-Leave> "$w configure -cursor \
       top_left_arrow; $w tag configure $tag -foreground black"

      $w tag bind $tag <1> "DelRelRec $i ; .relinfo.t delete 0.0 end ; \
         PrintRelRec .relinfo.t"
   }

   for {set i 0} {$i < $n} {incr i} {
# increment counter 1 to skip over status info
      set currtag $starttag ; #incr'd later
      set curr [expr $i + 1]
      set rec [lindex $RELRECS $curr]
      set deltag drr$currtag
      insertWithTags $w " Remove " drr$i raised
      insertWithTags $w { }
      foreach field $rec {
         set tag [lindex $field 1]
         if {$tag==245} {
            set tmpind [lindex $field 2]
            set indic [lindex $tmpind 0]
            set tmpnum [llength $field]
            set numsubfields [expr $tmpnum + 1]
            for {set s 3} {$s < $numsubfields} {incr s} {
               set subfield [lindex $field $s]
               set subfielddelim [lindex $subfield 0]
               set subfieldval [lindex $subfield 1]
               if {(($tag=="245") && ($subfielddelim=="a" || \
                   $subfielddelim=="b"))} {
                   insertWithTags $w $subfieldval underline
                   insertWithTags $w { }
               } else {
                   insertWithTags $w { }
                   $w insert {end - 1 chars} "$subfieldval  "
               }
            }
            $w insert end " "
         }
      } 
   $w insert end \n
   }
   $w insert end \n
}



proc SaveRelRec {r} {
   global RELRECS CURQUERY

   if {[lindex [lindex $r [expr [llength $r] - 1]] 1]=="999" && \
            [regexp -- "TOPIC" $CURQUERY]} {
     set x [expr [llength $r] - 1]

     set r [lreplace $r $x $x]
   }

   set n [lsearch -exact $RELRECS $r]

   if {$n==-1} {
      lappend RELRECS $r
      set currnum [lindex [lindex [lindex $RELRECS 0] 3] 1]
      incr currnum
      set replacement1 "Hits $currnum"
      set replacement2 "Received $currnum"
      set RELRECSTATS [lreplace [lindex $RELRECS 0] 2 2 $replacement1]
      set RELRECSTATS [lreplace $RELRECSTATS 3 3 $replacement2]
      set RELRECS [lreplace $RELRECS 0 0 $RELRECSTATS]
puts stdout " $RELRECSTATS "
   } else {
     mkDialog .foo {-text { This record has already been marked as relevant; not
ice the neat salmon1 background.} -aspect 250} {OK {}}
   }
}

proc DelRelRec {n} {
   global RELRECS

   set r [lindex $RELRECS $n]
   set currnum [lindex [lindex [lindex $RELRECS 0] 2] 1]

   set RELRECS [lreplace $RELRECS $n $n]
#   mkDialog .foo {-text " Sure?                       " -aspect 250} {OK {global RELRECS n ; set RELRECS [lreplace $RELRECS $n $n]} Cancel {}}

   set currnum [expr $currnum - 1]
   set replacement1 "Hits $currnum"
   set replacement2 "Received $currnum"
   set RELRECSTATS [lreplace [lindex $RELRECS 0] 2 2 $replacement1]
   set RELRECSTATS [lreplace $RELRECSTATS 3 3 $replacement2]
   set RELRECS [lreplace $RELRECS 0 0 $RELRECSTATS]

}
