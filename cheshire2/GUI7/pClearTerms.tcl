#proc ClearTerms -- empty out entry widgets in display

proc ClearTerms {} {

    global searchTerm probSearchTerm currSearch currProbType

    # if boolean interface, clear out 4 boolean terms, otherwise
    # do 2 boolean terms and the probabilistic entry.  Log the clear out.
    if {$currSearch == "bl"} {
	for {set i 0} {$i<4} {incr i} {
	    set searchTerm($i) ""
	}
    } else {
	for {set i 0} {$i<2} {incr i} {
	    set searchTerm($i) ""
	}
	set probSearchTerm ""
	.search.probentry configure -state normal
	.search.probentry delete 1.0 end
	if {$currProbType == "unused"} {
	    .search.probentry configure -state disabled
	}
	update
    }
    pLogCmd 15
}

