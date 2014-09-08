# pFeedbkButtonCmd -- user interface stuff to make sure user
#     knows what they're doing with relevance feedback search


proc pFeedbkButtonCmd {} {
    global entryBG defaultBG defaultActiveBG dialogButton
    global currSelectList selection ERROR currProbType

# if user hasn't selected records for a feedback search, tell them to go
# do that and bail out

    if {[llength $currSelectList] == 0} {
	pDialog .d "SEARCH Using Selected" "You must select records (using the \"Select\" button next to the record numbers in the main display) before attempting this type of search." \
	    {error} 0 {OK}
	set ERROR "User attempted to perform relevance feedback search without
selecting records first."
	pLogCmd 37
	return

    } elseif {$currProbType != "cs"} {
	# if the current search isn't a class cluster feedback, warn the
	# user that this could take a little bit so they don't get fidgety
	# during search.  We don't bother telling them this for a class
	# cluster feedback search as 1. class cluster feedbacks are 
	# quicker than record feedbacks, and 2. there's not much else
	# to do after a first part of a class cluster search than a
	# feedback search
	pDialog .d "SEARCH Using Selected" "Searches which attempt to find records similar to those you've selected can provide very useful results, but can take up to 30 seconds to perform.  Are you sure you want to go ahead with the search?" \
	    {error} 0 {Yes} {Cancel}
	if {$dialogButton == 0} {
	    set selection "selectrecs"
	    pFeedbkCmd
	} else {
	    pLogCmd 30
	    return
	}
    } else {
	# this section is invoked when a user does feedback from
	# class cluster record display
	set selection "selectrecs"
	pFeedbkCmd
    }
}
















