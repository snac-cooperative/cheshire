# ************************************************************************
# 
#        Header Name:   Dispfuncs.tcl
# 
#        Programmer:    Joachim Martin 
# 
#        Purpose:       Set of functions to handle displaying marc
# 			records encapsulated in a tcl list.  Also 
# 			some funcs to handle saving/deleting recs.
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


# myprintz print out a z39 result in a variety of formats
# opt =  what kind of result is this?  For distinguishing \
    between saved recs and current result set, and for cleaning \
    up text widget 
# r = tcl variable that holds the z39 result (in string form \
    using ztclsh or zwish
# w = name of window to insert into
# 

proc myprintrec {opt r w} {
   if {[llength $r]==1} {
      return
   }
   global format RSTATS SRECS smfont medfont bigfont BIBTAGS RELRECS CURQUERY CURHOST

   # get result stats
   set tmp [lindex $r 0]

   if {"Hits"==[lindex [lindex $tmp 2] 0]} {
      # this is initial res, get number of recs in current set
      set tmp2 [lindex $tmp 3]
      set n [lindex $tmp2 1]
      # get hits
      set tmp3 [lindex $tmp 2]
      set h [lindex $tmp3 1]
      # set up tag generation
      set starttag 1
      set endtag $n
$w delete 0.0 end
   } else {
      # must be zdisplay, get position
      set tmp2 [lindex $tmp 3]
      set position [lindex $tmp2 1]
      # get number of recs in current set
      set tmp3 [lindex $tmp 2]
      set n [lindex $tmp3 1]
      # set up tag generation
      set starttag $position
      set endtag [expr $position + $n]
      # get hits var 
      set h [lindex [lindex $RSTATS 2] 1]
   }
    

    # Set up display styles

    $w tag configure italic -font -Adobe-Times-Medium-I-Normal-*-180-*
    $w tag configure verysmall -font -Adobe-Times-medium-R-Normal-*-100-*
    $w tag configure small -font -Adobe-Times-medium-R-Normal-*-120-*
    $w tag configure big -font -Adobe-Times-medium-R-Normal-*-140-*
    $w tag configure verybig -font -Adobe-Times-medium-R-Normal-*-240-*
    $w tag configure bigcourier -font -Adobe-Courier-medium-R-Normal-*-140-*
    $w tag configure bighell -font -Adobe-Helvetica-Bold-R-Normal-*-140-*

    if {[tk colormodel $w] == "color"} {
        $w tag configure color1 -background #eed5b7
        $w tag configure color2 -foreground salmon1
        $w tag configure raised -background #ffe4c4 -relief raised \
         -borderwidth 2
        $w tag configure sunken -background #eed5b7 -relief sunken \
                -borderwidth 1
        set bold "-foreground salmon1"
        set normal "-foreground {}"
    } else {
        $w tag configure color1 -background black -foreground white
        $w tag configure color2 -background black -foreground white
        $w tag configure raised -background white -relief raised \
                -borderwidth 1
        $w tag configure sunken -background white -relief sunken \
                -borderwidth 1
        set bold "-foreground white -background black"
        set normal "-foreground {} -background {}"
    }
    $w tag configure bgstipple -background black -borderwidth 0 \
            -bgstipple gray25
    $w tag configure fgstipple -fgstipple gray50
    $w tag configure underline -underline on

    $w insert end \n

    for {set i $starttag; set x 1} {$i <= $endtag} {incr i;incr x} {
        set tag r$i
        $w tag delete $tag
        set tag r$i
        if {$CURHOST=="Cheshire2"} {
          set num [expr [llength [lindex $r $x]] - 1]
          puts stdout "this is a rel search"
          set goodrec [lreplace [lindex $r $x] $num $num]
        } else {
           puts "not a RELSEQARCG"
          set goodrec [lindex $r $x]
        }
puts stdout $goodrec
        set exists [lsearch -exact $RELRECS $goodrec]
        if {$exists>-1} { $w tag config $tag -background salmon1 \
                      -font -adobe-times-medium-i-normal-*-120-*
        }
        $w tag bind $tag <1> "SaveRelRec {$goodrec}; $w tag config $tag \
        -font -adobe-times-medium-i-normal-*-120-* -background salmon1"
    }
     
    if {$opt=="SRECS"} {
       for {set i $starttag; set x 1} {$i <= $endtag} {incr i;incr x} {
           set tag d$i
           set badrec {[lindex $r $x]}
           $w tag bind $tag <1> "DelRec $i $w"
       }
    }

    for {set i $starttag; set x 1} {$i <= $endtag} {incr i;incr x} {
        set tag s$i
        $w tag delete $tag 
        set tag s$i
        if {$CURHOST=="Cheshire2"} {
          set num [expr [llength [lindex $r $x]] - 1]
          puts stdout "this is a rel search"
          set goodrec [lreplace [lindex $r $x] $num $num]
        } else {
           set goodrec [lindex $r $x]
        }
        set exists [lsearch -exact $SRECS $goodrec]
        if {$exists>-1} { $w tag config $tag -background salmon1 \
                      -font -adobe-times-medium-i-normal-*-120-* 
        }

        $w tag bind $tag <1> "SaveRec {$goodrec}; $w tag config $tag \
        -font -adobe-times-medium-i-normal-*-120-* -background salmon1"
        $w tag bind $tag <2> "puts stdout {$goodrec}"
    }

    for {set i $starttag} {$i <= $endtag} {incr i} {
       foreach l {r s d} {
          set tag $l$i
          $w tag bind $tag <Any-Enter> "$w configure -cursor hand2; \
           $w tag configure $tag $bold "
          $w tag bind $tag <Any-Leave> "$w configure -cursor \
           top_left_arrow; $w tag configure $tag $normal"
          $w tag configure $tag -relief raised
      }
    }

   $w yview end
   for {set i 0} {$i < $n} {incr i} {
# increment counter 1 to skip over status info
      set currtag $starttag ; #incr'd later
      set curr [expr $i + 1]
      set rec [lindex $r $curr]
      set reltag r$currtag
      set savetag s$currtag
      set deltag d$currtag

      if {$opt=="RES"} {
         set recno [lindex $rec 0]
         $w insert end "$currtag/$h      " 
         insertWithTags $w { }
         insertWithTags $w "  Relevant  " $reltag raised big
         insertWithTags $w {          }
         insertWithTags $w "  Save Record   " $savetag raised big 
         insertWithTags $w {          }
         if {[lindex [lindex $rec [expr [llength $rec] - 1]] 1]=="999" && \
            [regexp -- "TOPIC" $CURQUERY]} {
            insertWithTags $w " record weight = [lindex [lindex [lindex \
            [lindex $rec [expr [llength $rec] - 1]] 3] 1] 0]" raised
            insertWithTags $w { }
         }
         $w insert end \n
      } elseif {$opt=="SRECS"} {
         set recno $curr
         $w insert end "$recno/$h                    "
         insertWithTags $w "   Delete record from list   " \
          $deltag raised big 
         insertWithTags $w {                    }
         $w insert end \n

      }
      insertWithTags $w { } verysmall
      $w insert end \n
      insertWithTags $w {}

      set rec [lrange $rec 1 end]
         switch $format {
         SHORT {
            foreach field $rec {
               set fieldnum [lindex $field 0]
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
         }
         MEDIUM {
            foreach field $rec {
               set fieldnum [lindex $field 0]
               set tag [lindex $field 1]
               if {$tag==100 || $tag==245 || $tag==250 || $tag==260 || \
                   $tag==300} {
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
                  if {$tag >= 100 && $tag<=200} {
                     $w insert end "\n      " }
                  $w insert end " "
               }
            } 
         }
         LONG {
            foreach field $rec {
               set fieldnum [lindex $field 0]
               set tag [lindex $field 1]
               if {$tag==100 || $tag==245 || $tag==260 || $tag==300 || \
                   $tag==440 || $tag==500 || $tag==650} {
                  set tmpind [lindex $field 2]
                  set indic [lindex $tmpind 0]
                  set tmpnum [llength $field]
                  set numsubfields [expr $tmpnum + 1]
                  insertWithTags $w "[format "%-13s" $BIBTAGS($tag)]" \
                     bigcourier
                  for {set s 3} {$s < $numsubfields} {incr s} {
                     set subfield [lindex $field $s]
                     set subfielddelim [lindex $subfield 0]
                     set subfieldval [lindex $subfield 1]
                     if {(($tag=="245") && ($subfielddelim=="a" || \
                          $subfielddelim=="b"))} {
                        insertWithTags $w $subfieldval underline bigcourier
                        insertWithTags $w { }
                     } else {
                        insertWithTags $w { }
                        insertWithTags $w "$subfieldval  " bigcourier
                     }
#                     if {($tag>499) && ($tag<700)} 
#                        $w insert end "$subfield
                  }
               $w insert end \n 
               }
            } 
         }
         MTAG {
            $w insert end \n
            foreach field $rec {
               set fieldnum [lindex $field 0]
               set tag [lindex $field 1]
               set tmpind [lindex $field 2]
               set indic [lindex $tmpind 0]
               set tmpnum [llength $field]
               set numsubfields [expr $tmpnum + 1]
               if {$tag<=008} {
                  set tmp [lindex $field 2]
                  set fieldval [lindex $tmp 0]
                  insertWithTags $w "$tag | $fieldval" bigcourier
               } else {
                  insertWithTags $w "$tag $indic | " bigcourier
                  for {set s 3} {$s < $numsubfields} {incr s} {
                     set subfield [lindex $field $s]
                     set subfielddelim [lindex $subfield 0]
                     set subfieldval [lindex $subfield 1]
                     if {(($tag=="245") && ($subfielddelim=="a" || \
                          $subfielddelim=="b"))} {
                        insertWithTags $w $subfieldval bigcourier underline
                        insertWithTags $w { }
                     } else {
                        insertWithTags $w { }
                        insertWithTags $w "$subfieldval  " bigcourier
                     }
                  }
               }
            $w insert end \n
            } 
         }
         default {
            foreach field $rec {
               set fieldnum [lindex $field 0]
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
         }
      $w insert end \n\n
      }
   $w insert end \n\n\n\n
   incr starttag
   }
$w insert end \n\n\n
} 


# The procedure below inserts text into a given text widget and
# applies one or more tags to that text.  The arguments are:
#
# w             Window in which to insert
# text          Text to insert (it's inserted at the "insert" mark)
# args          One or more tags to apply to text.  If this is empty
#               then all tags are removed from the text.

proc insertWithTags {w text args} {
    set start [$w index end]
    $w insert end $text
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start end
    }
    foreach i $args {
        $w tag add $i $start end
    }
}


# getmore -- tcl function to get more z39.50 recs
# num = number of recs to get
# w insert window for new recs

proc getmore {num w} {
   global RES RSTATS DISPLAYCMD
   if {[lindex [lindex $RSTATS 2] 1]<=[lindex [lindex $RSTATS 3] 1]} {
      mkDialog .foo {-text " Sorry, I cant get any more records!  You
have retrieved them all!!!" -aspect 250} {OK {} Quit {}}
      return
   }

   set tmpRES [$DISPLAYCMD]
   
   # get result stats
   set tmp [lindex $tmpRES 0]

   # get number of recs in current present set
   set tmp2 [lindex $tmp 2]
   set recnum  [lindex $tmp2 1]
   # get position
   set tmp3 [lindex $tmp 3]
   set position [lindex $tmp3 1]
   for {set i 1} { $i <= $recnum} {incr i} {
      set rec [lindex $tmpRES $i] 
      lappend RES $rec
   }
   set currnum [lindex [lindex [lindex $RES 0] 3] 1]
   set replacement "Received [expr [expr $currnum  + $i] - 1]"
   set RSTATS [lreplace $RSTATS 3 3 $replacement]
   set RES [lreplace $RES 0 0 $RSTATS]
   myprintrec RES $tmpRES $w

}

proc SaveRec {r} {
   global SRECS

   set n [lsearch -exact $SRECS $r]

   if {$n==-1} {
      lappend SRECS $r
      set currnum [lindex [lindex [lindex $SRECS 0] 3] 1]
      incr currnum
      set replacement1 "Hits $currnum"
      set replacement2 "Received $currnum"
      set SRECSTATS [lreplace [lindex $SRECS 0] 2 2 $replacement1]
      set SRECSTATS [lreplace $SRECSTATS 3 3 $replacement2]
      set SRECS [lreplace $SRECS 0 0 $SRECSTATS]
      set saveinfo [format "%d records saved in your list" [expr [llength $SRECS] - 1]]
      set dispinfo [format " %30s " $saveinfo]
      .display.recno config -text "$dispinfo"
   } else {
     mkDialog .foo {-text { This record has already been saved in your list; notice the neat salmon1 background.} -aspect 250} {OK {}}
   }
}


proc DelRec {n w} {
   global SRECS 

   set r [lindex $SRECS $n]
   set title ""
   set currnum [lindex [lindex [lindex $SRECS 0] 2] 1]

   foreach field $r {
      if {[lindex $field 1]==245} {
         set numsubfields [expr [llength $field] + 1]
         for {set s 3} {$s < $numsubfields} {incr s} {
            set subfieldval [lindex [lindex $field $s] 1]
            set title [format "%s %s" $title $subfieldval]
         }
      }
   }
   set SRECS [lreplace $SRECS $n $n]
#   mkDialog .foo {-text " Sure?                       " -aspect 250} {OK {global SRECS n ; set SRECS [lreplace $SRECS $n $n]} Cancel {}}

   set currnum [expr $currnum - 1]
   set replacement1 "Hits $currnum"
   set replacement2 "Received $currnum"
   set SRECSTATS [lreplace [lindex $SRECS 0] 2 2 $replacement1]
   set SRECSTATS [lreplace $SRECSTATS 3 3 $replacement2]
   set SRECS [lreplace $SRECS 0 0 $SRECSTATS]
   set saveinfo [format "%d records saved in your list" \
      [expr [llength $SRECS] - 1]]
   set dispinfo [format " %30s " $saveinfo]
   .display.recno config -text "$dispinfo"

   if {[llength $SRECS]>1} {
      myprintrec SRECS $SRECS $w
   } else {
      $w delete 0.0 end
   }
}

