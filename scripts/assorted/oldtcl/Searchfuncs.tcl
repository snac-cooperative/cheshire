# ************************************************************************
# 
#        Header Name:    Searchfuncs.tcl
# 
#        Programmer:     Joachim Martin and Alicia Abrahmson
# 
#        Purpose:        Set of tcl/tk procedures that handle query 
#  			formulation and sending a z39.50 or cheshire2 search.
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
# ***********************************************************************/



proc parse_z_search {auth title subj w1 w2} {

# s_tag is an array of "tags" assigned to  search terms
# s_var is a list of keys input by user
# boole is an array of boolean search conjunctions
# tmp_res and it's variants are storage areas for the bits of parsed search 
# counter is used to determine number of boolean connectors
# Search is a multidimensional array where the search is strung together

   global RES sep TOPIC
   set TOPIC [.search.prob.t get 0.0 end]
   set search ""

# HARDCODED LIKE JO MAMA ONLY KNOWS HOW

   if {$TOPIC!=""} {
      append search "TOPIC = {$TOPIC} "
   }

   if {(($auth!="") && ($TOPIC!=""))} {
      append search "$sep(0) AUTHOR = {$auth} "
   } elseif {$auth!=""} {
      append search "AUTHOR = {$auth} "
   }

   if {(($title!="") && ($auth!=""))} {
      append search "$sep(1) TITLE = {$title} "
   } elseif {(($title!="") && ($auth=="") && ($TOPIC!=""))} {
      append search "$sep(0) TITLE = {$title} "
   } elseif {$title!=""} {
      append search "TITLE = {$title} "
   }

   if {(($subj!="") && ($title!=""))} {
      append search "$sep(2) SUBJECT = {$subj} "
   } elseif {(($subj!="") && ($title=="") && ($auth!=""))} {
      append search "$sep(1) SUBJECT = {$subj} "
   } elseif {(($subj!="") && ($title=="") && ($auth=="") && ($TOPIC!=""))} {
      append search "$sep(0) SUBJECT = {$subj} "
   } elseif {(($subj!="") && ($title=="") && ($auth=="") && ($TOPIC==""))} {
      append search "SUBJECT = {$subj} "
   }
      
   if {$search!=""} {
      send_z_search $search $w2
   } else {
      $w2 yview end
      $w2 insert end "You have not entered any terms to search for!!!"
      checkhost .search.f1
      return
   }

}


########
# this func actually sends the query
########

proc send_z_search {temp_res w} {

   global RES RSTATS HIST HOST CURHOST S_NUM FINDCMD DISPLAYCMD CURQUERY


      if {$HOST=="" && $CURHOST=="Not set"} {
         set HOST Cheshire2
         set CURHOST Cheshire2
         set FINDCMD local_zfind
         set DISPLAYCMD local_zdisplay
#         zselect $HOST
puts stdout "Cheshire2 selected"
      } elseif {$CURHOST=="Cheshire2"} {
         set HOST $CURHOST
         set FINDCMD local_zfind
         set DISPLAYCMD local_zdisplay
puts stdout "Cheshire2 selected"
      } elseif {$HOST=="" && $CURHOST!=""} {
         if {$CURHOST=="Cheshire2"} {
            set HOST $CURHOST
            set FINDCMD local_zfind
            set DISPLAYCMD local_zdisplay
puts stdout "Cheshire2 selected"
         } else {
         set HOST $CURHOST
         set DISPLAYCMD zdisplay
         set FINDCMD zfind
         zselect $HOST
    puts stdout $HOST
        }
      } elseif {$HOST!=$CURHOST} {
         set DISPLAYCMD zdisplay
         set FINDCMD zfind
         zselect $CURHOST
puts stdout $CURHOST
      } else {
puts stdout "$CURHOST is current host"
      }
   set CURQUERY "$temp_res"

   set RES [$FINDCMD $temp_res]
   if {[lindex [lindex [lindex $RES 0] 1] 1]<0 && $CURHOST!="Cheshire2"} {
       mkDialog .z3950error {-text "There was a Z39.50 error.  This is probably due to a search that is too broad or too complex." -aspect 250} {OK {}}
       focus .search.f1.entry
       return 
   }   

 # get result stats
   set tmp [lindex $RES 0]
puts stdout "$tmp"
   # get number of recs in current present set
   set tmp2 [lindex $tmp 3]
   set recnum [lindex $tmp2 1]
   # get hits
   set tmp3 [lindex $tmp 2]
   set hits [lindex $tmp3 1]

   for {set i 1} { $i <= $recnum} {incr i} {
      set rec [lreplace [lindex $RES $i] 0 0 "0"]
      set RES [lreplace $RES $i $i $rec]
   }

   h_list $temp_res $hits

   while {$recnum < 10 && (!($recnum >= $hits))} {
      set tmpRES [$DISPLAYCMD]
      set numr [llength $tmpRES]

      # get result stats
      set tmpstats [lindex $tmpRES 0]
      # get number of recs in current present set
      set tmp2 [lindex $tmpstats 2]
      set presrecnum  [lindex $tmp2 1]
      # get position
      set tmp3 [lindex $tmpstats 3]
      set position [lindex $tmp3 1]

# replace recno with 0 - for checking saved records
      for {set i 1} { $i <= $presrecnum} {incr i} {
         set rec [lreplace [lindex $tmpRES $i] 0 0 "0"]
        lappend RES $rec
      }
      set recnum [expr $recnum + $presrecnum]
   }
    set tmp [lreplace $tmp 3 3 "Received $recnum"]
    set RES [lreplace $RES 0 0 $tmp]
    set RSTATS [lindex $RES 0]



   h_list $temp_res $hits
   incr S_NUM

 if {[$w get 0.0 end] != ""} {
   $w yview end
   }

   if {$w==".display.t"} {
      myprintrec RES $RES .display.t
   } else {
   $w insert end  "\
Your query resulted in $hits matches.

   Press the Display Results button below to see the results or enter a new search and press the SEARCH button to start over.
 
$tmp
"
   }
}
