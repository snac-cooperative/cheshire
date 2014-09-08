# chMisc.tcl -- Miscellaneous Utility Functions

# lcount -- function to count number of exact occurances of a term
#           in a list -- case IS significant.  lcount is at this
#           point only used in chHistory.tcl to determine the number
#           of "or," "OR," "and", and "AND" conjunctions in a search string

proc lcount {l pattern} {

    set i 0

    while [string length $l]>0 {
	set currPlace [string first $pattern $l]
	if {($currPlace==-1) && ($i==0)} {
	    return 0
	} elseif {$currPlace==-1} {
	    return $i
	} else {
	    set currPlace [expr ($currPlace+1)]
	    set l [string range $l $currPlace end]
	    incr i
	}
    }
}
