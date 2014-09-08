# orig: /home/yew/yew5/users/phelps/spine/RCS/searchbox.tcl,v 1.2 1993/08/19 00:11:30 phelps Exp phelps $
# RCS: $Header: /users/kennard/src/tkinfo/RCS/searchbox.tcl,v 1.2 1993/12/23 01:57:38 kennard Exp $

#
# SearchBox mega widget
#   incremental and regular expression searching in a text widget
#
#   by Tom Phelps (phelps@cs.Berkeley.EDU)
#
# extracted from and then used by TkMan and NBT 6-Aug-93
#
# 19-Aug  made more robust (Kennard White)
#

# requires: proc regexpTextSearch
# name space use: prefixes searchbox, sb, sbx



### default key bindings
# ""=unmodified, S=shift, A=alt, C=control, M=meta

set sb(key,*) "add modifiers in this order: M, C, A, S"
set sb(key,C-x) exchangepointandmark
set sb(key,C-space) setmark
set sb(key,-Delete) pageup
set sb(key,M-v) pageup
set sb(key,-space) pagedown
set sb(key,C-v) pagedown
set sb(key,MS-less) pagestart
set sb(key,MS-greater) pageend
set sb(key,-Escape) searchkill
set sb(key,C-g) searchkill
set sb(key,C-n) nextline
set sb(key,C-p) prevline
set sb(key,C-s) incrsearch
set sb(key,C-r) revincrsearch
set sb(key,MS-question) help
set sb(key,-Help) help



#--------------------------------------------------
#
# searchboxRegexpSearch -- initiate a regular expression search
#
# params
#    str = string to search for
#    regexp = boolean - regular expression search?
#    casesen = case sensitive?
#    tag = tag to associate with matches
#       (do a `tag bind' in the text widget for this tag)
#    w = text widget
#    wv = associated vertical scrollbar
#    wmsg = (optional) window to show status messages
#    wcnt = (optional) widget to show number of matches
#
#
#--------------------------------------------------

proc searchboxSearch {str regexp casesen tag w wv {wmsg ""} {wcnt ""}} {
   global sbx
#puts stdout "searchboxRegexpSearch $str $casesen w=$w $wv wmsg=$wmsg $wcnt"
   if {$str==""} {
      winerrout $wmsg "Nothing to search for!  Type a regexp and click `Search'."
      return -1
   }

   if {$regexp} {set type regexp} {set type ""}
   set cnt [${type}TextSearch $w $str $tag $casesen]
   if {$cnt==-1} {winerrout $wmsg "Malformed regular expression."; return -1}
   if {$cnt==0} {winstdout $wcnt "No matches"; return 0}
   if {$cnt==1} {set txt "$cnt match"} {set txt "$cnt matches"}
   winstdout $wcnt $txt

   # show the first one
#   set sbx(search) 1.0
#   $w yview 0; update idletasks
   # show the next match
   searchboxNext $tag $w $wv $wmsg [expr [lindex [$wv get] 2]+1].0
   return $cnt
}



#--------------------------------------------------
#
# searchboxNext -- show the next match
#
# params
#    tag = tag to search for (see searchboxRegexpSearch)
#    w = text widget
#    wv = associated vertical scrollbar
#    wmsg = window to show status messages
#
#--------------------------------------------------

proc searchboxNext {tag w wv {wmsg ""} {next ""}} {
   global sbx

#   set tmp [$w tag nextrange search [max [expr [lindex [$wv get] 2]+1].0 $sbx(search)]]
   if {$next==""} {set next [expr [lindex [$wv get] 3]+1+1].0}
   set tmp [$w tag nextrange $tag $next]
   if {$tmp==""} {
      winstdout $wmsg "No more matches; restarting at top."
      $w yview 0
#      set sbx(search) 1.0
   } else {
      $w yview -pickplace [lindex $tmp 0]
      update
#      winstdout $wmsg "Viewing lines [lindex [$wv get] 2] to [lindex [$wv get] 3]."
#      set sbx(search) [expr [lindex [$wv get] 3]+1+1].0
   }
}



#--------------------------------------------------
#
# searchboxKeyNav -- keyboard-based navigation and searching
#
#   maybe separate out some commands so incrsearch doesn't have to wade through
#
# params:
#   m = modifier key
#   k = key
#   casesen = case sensitive?
#   w = text widget
#   wv = associated vertical scrollbar
#   wmsg = label in which to show incremental search string
#   mode = 1=>match on first character of line, 0=>match anywhere
#
#--------------------------------------------------

proc searchboxKeyNav {m k casesen w wv {wmsg ""} {firstmode 0}} {
   global sb sbx

   if {[regexp {(Shift|Control|Meta)_.} $k]} return
   if {![info exists sbx(try$w)]} {
      set sbx(try$w) 0
      set sbx(vect$w) 1
      set sbx(lastkeys$w) [set sbx(lastkeys-old$w) ""]
   }


   # get initial values
   set minele 1
   if {[winfo class $w]=="Text"} {set off 1; scan [$w index end] %d numLines} \
   elseif {[winfo class $w]=="Listbox"} {set off 0; set numLines [$w size]; set minele 0}
   scan [$wv get] "%d %d %d %d" total window first last


   # some translations
   if {$sbx(try$w) || $sbx(lastkeys$w)!=""} {
      switch -exact -- $k {
         space {set k " "}
         Delete {
            set k ""
            set last [expr [string length $sbx(lastkeys$w)]-2]
            set sbx(lastkeys$w) [string range $sbx(lastkeys$w) 0 $last]
            set sbx(try$w) 1
         }
         default { if {$m==""||$m=="S"} {set k [name2char $k]} }
      }
   }


   # commands
   set mk $m-$k
   if {[info exists sb(key,$mk)]} {set op $sb(key,$mk)} {set op default}
#puts stdout "trying for a match on $mk"
#puts stdout "*$sb(setmark)* *$sb(pageup)* *$sb(pagedown)*"
   switch -exact -- $op {
      help {$w.occ.m invoke Help; return}
      exchangepointandmark {
         # yview w/o parameter should return current value
         set tmp [expr [lindex [$wv get] 2]+1].0
         $w yview xmark
         update
         $w mark set xmark $tmp
      }
      setmark {$w mark set xmark [expr [lindex [$wv get] 2]+1].0}
      pageup {$w yview [max [expr $first-$window+1] 0]}
      pagedown {$w yview [min [expr $first+$window-1] [expr $numLines-$window]]}
      pagestart {$w yview 0}
      pageend {$w yview [max [expr $numLines-$window] 0]}
      searchkill {
         if {$sbx(lastkeys$w)!=""} {set sbx(lastkeys-old$w) $sbx(lastkeys$w)}
         set sbx(lastkeys$w) ""; set sbx(try$w) 0; winstdout $wmsg ""
      }
      C-l {$w yview [max [expr $first-$window/2] 0]}
      nextline {$w yview [min [expr $first+1] [expr $numLines-$window]]}
      prevline {$w yview [max [expr $first-1] 0]}
      default {
         # incremental search
         if {$op=="incrsearch"} {
            # C-s C-s retrieves last search pattern
            if {$sbx(try$w)&&$sbx(lastkeys$w)==""} {set sbx(lastkeys$w) $sbx(lastkeys-old$w)}
            incr off; set sbx(vect$w) 1; set sbx(try$w) 1
         } elseif {$op=="revincrsearch"} {
            if {$sbx(try$w)&&$sbx(lastkeys$w)==""} {set sbx(lastkeys$w) $sbx(lastkeys-old$w)}
            incr off -1; set sbx(vect$w) -1; set sbx(try$w) 1
         } elseif {$firstmode} {
            set sbx(lastkeys$w) $k
         } elseif {$sbx(try$w)} {
	    append sbx(lastkeys$w) $k
         } else return

         if {$firstmode} {
            set curline 0
         } else {
            set curline [lindex [$wv get] 2]
            winstdout $wmsg "Searching for \"$sbx(lastkeys$w)\" ..."; update idletasks
         }
         if {[set keys $sbx(lastkeys$w)]==""} return

         set klen [string length $keys]
         set found -1
         for {set i [expr $curline+$off]} {$minele<=$i && $i<=$numLines} {incr i $sbx(vect$w)} {
            if {$firstmode} {
               if {"$keys"=="[$w get $i.0 $i.$klen]"} {set found 0; break}
            } elseif {!$casesen} {
               if {[set found [string first [string tolower $keys] \
                  [string tolower [$w get $i.0 "$i.0 lineend"]]]]!=-1} \
                  break;
            } elseif {[set found [string first $keys [$w get $i.0 "$i.0 lineend"]]]!=-1} {
               break
            }
         }

         # show results
         if {$found!=-1} {
            $w yview [expr $i-1]; update idletasks
            if {!$firstmode} {winstdout $wmsg "\"$keys\" found on line $i"}
         } elseif {$op=="incrsearch"} {
            $w yview 0
            winstdout $wmsg "No more matches found; restarting search at top."
         } elseif {$op=="revincrsearch"} {
             $w yview $total
	    winstdout $wmsg "No more match found; restarting search at bottom."
         } else {
            winstdout $wmsg "\"$keys\" not found"
            # turn off searching once can't match what you have
            set sbx(try$w) 0
         }
      }
   }
}



#--------------------------------------------------
#
# searchboxSaveConfig -- dump persistent variables into passed file id
#
#--------------------------------------------------

proc searchboxSaveConfig {fid} {
   global sb sbx

   puts $fid "#\n# SearchBox\n#\n"
   foreach i [lsort [array names sb]] {
      puts $fid "set sb($i) [list $sb($i)]"
   }
   puts $fid "\n"
}



# swiped from mkTextSearch w
#
# The utility procedure below searches for all instances of a
# given string in a text widget and applies a given tag to each
# instance found.
# Arguments:
#
# w -		The window in which to search.  Must be a text widget.
# string -	The string to search for.  The search is done using
#		exact matching only;  no special characters.
# tag -		Tag to apply to each instance of a matching string.
# case -        (optional) case sensitive?

proc TextSearch {w string tag {case 1}} {
   set cnt 0

    $w tag remove $tag 0.0 end
    scan [$w index end] %d numLines
    set l [string length $string]
    if {!$case} {set string [string tolower $string]}
    for {set i 1} {$i <= $numLines} {incr i} {
	set match [$w get $i.0 $i.1000]
	if {!$case} {set match [string tolower $match]}
	if {[string first $string $match] == -1} {
	    continue
	}
	set line [$w get $i.0 $i.1000]
	set offset 0
	while 1 {
	    set index [string first $string $line]
	    if {$index < 0} {
		break
	    }
	    incr offset $index
	    $w tag add $tag $i.[expr $offset] $i.[expr $offset+$l]
            incr cnt
	    incr offset $l
	    # below bug fix from mkSearch.tcl
	    set line [string range $line [expr $index+$l] 1000]
	}
    }
   return $cnt
}


# modified to handle regexp's and return # of matches -TAP

proc regexpTextSearch {w string tag {case 1}} {
   set cnt 0
   if {$case} {set case ""} {set case "-nocase"}
   if {[catch {regexp $string bozomaniac}]} {return -1}

    $w tag remove $tag 0.0 end
    scan [$w index end] %d numLines

    for {set i 1} {$i <= $numLines} {incr i} {
      set line [$w get $i.0 $i.1000]
      set offset 0
      while 1 {
         if {![eval regexp $case -indices {"$string"} {"$line"} match]} break
         scan $match "%d %d" index iend
         $w tag add $tag $i.[expr $offset+$index] $i.[expr $offset+$iend+1]
         set line [string range $line [expr $iend+1] end]
         incr offset [expr $iend+1]
         incr cnt
      }
   }
   return $cnt
}



# instantiate standard mechanisms if don't already exist
if {[info procs winstdout]==""} {

proc winerrout {w msg} {
   if {![winfo exists $w]} return

   set fg [lindex [$w configure -foreground] 4]
   set bg [lindex [$w configure -background] 4]

   winstdout $w $msg
   $w configure -foreground $bg -background $fg
   update idletasks; after 500
   $w configure -foreground $fg -background $bg
}

proc winstdout {w msg} {
   global winout

   if {![winfo exists $w]} return
   $w configure -text $msg
   set winout(lastMessage$w) $msg
}

}



