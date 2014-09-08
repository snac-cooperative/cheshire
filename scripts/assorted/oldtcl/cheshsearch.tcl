# cheshsearch.tcl
# Author   Ray R. Larson/Lucy Kuntz
# date     Nov. 24, 1993
# version  1.0
#
# The following procedures provide an interface to the probabilistic
# and boolean search mechanisms of the Cheshire II system.
#
#
global RECSEQUENCENUM
set RECSEQUENCENUM 0

proc local_zfind {query} {

global RECSEQUENCENUM
set RECSEQUENCENUM 0

    set tag [lindex $query 0]
#    puts stdout $tag
    set oper [lindex $query 1]
#    puts stdout $oper
    set search_string [lindex $query 2]
#    puts stdout $search_string

    set returncode [catch {cheshsearch $query} results]
    if {$returncode != 0} {
	set results "{OK {Status -1} {Hits 0} {Received 0} {Set default}}"
    }
    return $results
}


proc local_zdisplay {args} {

global RECSEQUENCENUM

    set recvd 0

    set nargs [llength $args]
#   puts stdout "nargs = $nargs  args = '$args'"

    set returncode [catch getnextprobresult temp]

if {$returncode == 0} {
    #	puts stdout $temp

    set recno [lindex $temp 0]

    set weight [lindex $temp 1]
 
    set returncode [catch {getmarcstring bibfile $recno} x]

    if {$returncode == 0} {
	incr RECSEQUENCENUM 
	set l [marc2list $x $recno $weight]
	set results [format "{OK {Status 2} {Received 1} {Position %d} {Set default}} %s" $RECSEQUENCENUM $l]

	return $results
    } else {
	set results "{OK {Status 5} {Received 0} {Position 0} {Set default}}"
    }	
  } else {
      set results "{OK {Status 5} {Received 0} {Position 0} {Set default}}"
  }

return $results
}









