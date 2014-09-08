# chDisplayRec.tcl -- record Format and display routines


# pLabelRecord -- Place a label and select button for record i in
#   text widget w

proc pLabelRecord {w i format} {
    global buttonBG defaultBG defaultFont

    # output the label to the text window
    $w configure -state normal
    scan [$w index end] %f lineNr
    $w insert $lineNr [format $format $i]

    # record the current line number
    set pntLoc [string first . $lineNr]
    set lineLoc [string range $lineNr 0 [expr $pntLoc - 1] ]
    set lineLoc [expr $lineLoc - 1]

    # output button text
    $w insert end "  Select  "

    # figure out where on current line to put button tagging
    set tagstart [$w search " Select" $lineLoc.0]
    set pntLoc [string first . $tagstart]
    set startLoc [string range $tagstart 0 [expr $pntLoc -1] ]
    set endLoc [string range $tagstart [expr $pntLoc + 1] end]
    set endLoc [expr $endLoc + 9]
    set tagend [format %s\.%s $startLoc $endLoc]

    # add the button tag and configure appropriately
    $w tag add selectTag$i $tagstart $tagend

    $w tag configure selectTag$i -background $defaultBG \
	-relief raised -borderwidth 2 \
	-font $defaultFont

    $w tag bind selectTag$i <Enter> \
	"pSelectEnter $w $i"

    $w tag bind selectTag$i <Leave> \
	"pSelectLeave $w $i"

    $w tag bind selectTag$i <ButtonPress> \
	"pSelect $w $i"

    # disable text window so stupid user can't enter text in it
    $w configure -state disabled
}


#
# pClearROText w -- Clear read-only text window w
#

proc pClearROText {w} {

    $w configure -state normal
    $w delete 1.0 end
    foreach tag [$w tag name] {
	if {"$tag" != "sel"} {
	    $w tag delete $tag
	}
    }
    $w configure -state disabled

    update

}


#
# pAppendROText w s -- Add formatted string s at end of text window w
#   and return the numbers of the beginning and ending lines of new text
#   in the window

proc pAppendROText {w f s} {
    $w configure -state normal
    set start [$w index end]
    $w insert end [format $f $s]
    set end [$w index end]
    $w configure -state disabled
    update

    set pntLoc [string first . $start]
    set startLine [string range $start 0 [expr $pntLoc - 1]]
    set pntLoc [string first . $end]
    set endLine [string range $end 0 [expr $pntLoc - 1]]
    set endLine [expr $endLine - 1]
    return "$startLine $endLine"
}


#
#pOPACtag -- tag lines in .retrieve.text (yes, hardcoded) between
#            the lines in txtlines to conform with OPAC display
#            style for particular format (review, short, long) for
#            record #i.  Yes, this is extremely nasty.
#

proc pOPACtag {txtlines format i} {
    global reviewTags shortTags longTags searchTerm buttonBG HTFG HTActiveFG
    global titleFont

    # initial setup
    set tmpLEnd -1
    set tmpCEnd -1

    # find the starting and ending line of record
    set pntLoc [string first " " $txtlines]
    set startLine [string range $txtlines 0 [expr $pntLoc - 1]]
    set endLine [string range $txtlines [expr $pntLoc + 1] end]

    # find out the formatting tagset we're going to use
    switch $format {
	review {
	    set tagset $reviewTags
	}
	short {
	    set tagset $shortTags
	}
	long {
	    set tagset $longTags
	}
    }

    # for every line in the record
    #   for every tag in the tagset
    #      see if this line is the beginning of something that
    #      ought to be tagged with the current tag
    #      if yes, tag as appropriate, if no, go on
    for {set i $startLine} {$i < $endLine} {incr i} {
	for {set j 0} {$j < [llength $tagset]} {incr j} {
	    set matchstring [lindex [lindex $tagset $j] 0]
	    set matchlength [string length $matchstring]
	    set tmpstring [.retrieve.text get "$i.0" "$i.$matchlength"]

	    if {[string compare $tmpstring $matchstring] == 0} {
		# following gets the start and end points for considering
		# where to assign tag
		set tagstart "$i.15"
		for {set k [expr $i + 1]} {$k < $endLine} {incr k} {
		    set tmpstring [.retrieve.text get "$k.0" "$k.14"]
		    if {[string first ":" $tmpstring] != -1} {
			set tmpLEnd [expr $k - 1]
			set tmpstring [.retrieve.text get "$tmpLEnd.0" $tmpLEnd.80]
			set tmpCEnd [expr [string length $tmpstring]]
			break
		    }
		}
		if {$tmpLEnd == -1} {
		    set tmpLEnd [expr $endLine - 1]
		}
		if {$tmpCEnd == 0} {
		    set tmpCEnd 80
		}
		set tagend "$tmpLEnd.$tmpCEnd"

		set tagCfgOpt [lindex [lindex $tagset $j] 1]
		set tmpstring [string range $matchstring 0 [expr $matchlength - 2]]

		# if there's spaces in tagName, get rid of them. 
		set hasSpace [string first " " $tmpstring]
		while {$hasSpace != -1} {
		    set tmpFstring [string range $tmpstring 0 [expr $hasSpace - 1]]
		    set tmpEstring [string range $tmpstring [expr $hasSpace + 1] end]
		    set tmpstring "$tmpFstring$tmpEstring"
		    set hasSpace [string first " " $tmpstring]
		}
		set tagName "$tmpstring$i"

		if {[lindex [lindex $tagset $j] 3] == "ALL"} {
		    .retrieve.text tag add $tagName $tagstart $tagend
		    # NOTE -- without the eval, the tagCfgOpt isn't recognized
		    eval .retrieve.text tag configure $tagName $tagCfgOpt

		    set tmpCmd [lindex [lindex $tagset $j] 2]
		    if {$tmpCmd != ""} {
			
			set tmpstring [.retrieve.text get $tagstart $tagend] 
			set hyperStr [tagClean $tagName $tmpstring]
			set hyperCmd [format $tmpCmd $hyperStr]
			
			.retrieve.text tag bind $tagName <Enter> \
			    "pHyperEnter .retrieve.text $tagName"
			
			.retrieve.text tag bind $tagName <Leave> \
			    "pHyperLeave .retrieve.text $tagName"
			
			.retrieve.text tag bind $tagName <ButtonPress> \
			    "eval $hyperCmd"
		    }
		    
		    
		} elseif {[lindex [lindex $tagset $j] 3] == "LINES"} {
		    for {set k $i} {$k <= $tmpLEnd} {incr k} {
			if {[.retrieve.text get "$k.15" "$k.18"] != "   "} {
			    set tmpTagStart $k.15
			    set tmpTLEnd -1

			    #find end of this tagline.
			    for {set x [expr $k + 1]} {$x <= $tmpLEnd} {incr x} {
				if {[.retrieve.text get "$x.15" "$x.18"] != "   "} {
				    set tmpTLEnd [expr $x - 1]
				    break
				}
			    }

			    if {$tmpTLEnd == -1} {
				set tmpTLEnd $tmpLEnd
			    }
			    
			    set tmpstring [.retrieve.text get "$tmpTLEnd.0" "$tmpTLEnd.80"]
			    set tmpTCEnd [string length $tmpstring]
			    
			    set tmpTagEnd "$tmpTLEnd.$tmpTCEnd"
			    
			    #get text for hypertag
			    .retrieve.text tag add $tagName$k $tmpTagStart $tmpTagEnd
			    eval .retrieve.text tag configure $tagName$k $tagCfgOpt
			    
			    set tmpCmd [lindex [lindex $tagset $j] 2]
			    if {$tmpCmd != ""} {

				set tmpstring [.retrieve.text get $tmpTagStart $tmpTagEnd]
				set hyperStr [tagClean $tagName $tmpstring]
				set hyperCmd [format $tmpCmd $hyperStr]
				
				.retrieve.text tag bind $tagName$k <Enter> \
				    "pHyperEnter .retrieve.text $tagName$k"
				
				.retrieve.text tag bind $tagName$k <Leave> \
				    "pHyperLeave .retrieve.text $tagName$k"
				
				.retrieve.text tag bind $tagName$k <ButtonPress> \
				    "eval $hyperCmd"
				
			    }
			}
		    }
		} else {
		    puts stderr "Something's VERY wrong in pOPACtag\n"
		    set tagHunt $tagstart
		    set searchString [lindex [lindex $tagset $j] 3]
		    for {set k 0} {$k < [string length $searchString]} {incr k} {
			set searchChar [string index $searchString $k]
			set charLocation [.retrieve.text search $searchChar $tagHunt $tagend]
			if {$charLocation == ""} {
			    set tmpTagEnd $tagend
			    break
			} else {
			    set pntLoc [string first . $charLocation]
			    set startLoc [string range $charLocation 0 [expr $pntLoc -1] ]
			    set endLoc [string range $charLocation [expr $pntLoc + 1] end]
			    set endLoc [expr $endLoc + 1]
			    set tagHunt [format %s\.%s $startLoc $endLoc]
			}
			if {[expr $k + 1] == [string length $searchString]} {
			    set tmpTagEnd $tagHunt
			}
		    }
		    .retrieve.text tag add $tagName $tagstart $tmpTagEnd
		    eval .retrieve.text tag configure $tagName $tagCfgOpt
		    set tmpCmd [lindex [lindex $tagset $j] 2]
		    if {$tmpCmd != ""} {
			
			set hyperCmd [format $tmpCmd [.retrieve.text get $tagstart $tmpTagEnd]]
			
			.retrieve.text tag bind $tagName <Enter> \
			    "pHyperEnter .retrieve.text $tagName"
			
			.retrieve.text tag bind $tagName <Leave> \
			    "pHyperLeave .retrieve.text $tagName"
			
			.retrieve.text tag bind $tagName <ButtonPress> \
			    "eval $hyperCmd"
		    }
		    
		}
	    }
	    set tmpLEnd -1
	    set tmpCEnd -1
	}
    }
}


#
# tagClean -- procedure to remove extraneous characters from hypertext tag
#             so they don't fuck up Z39.50 search
#

proc tagClean {tagname tagstring} {
    
    set tmpstring1 $tagstring
    set tmpstring2 ""


    #if an author tag, remove final date ranges
    if {[string first "Author" $tagname] != -1} {
	regsub {([0-9][0-9][0-9][0-9]-)} $tmpstring1 "" tmpstring2
	regsub {[0-9][0-9][0-9][0-9]} $tmpstring2 "" tmpstring1
    }

    #remove any parentheses
    while {[string first "\(" $tmpstring1] != -1} {
	regsub {\(} $tmpstring1 "" tmpstring2
	set tmpstring1 $tmpstring2
    }
    while {[string first "\)" $tmpstring1] != -1} {
	regsub {\)} $tmpstring1 "" tmpstring2
	set tmpstring1 $tmpstring2
    }

    #remove any periods or commas
    while {[string first . $tmpstring1] != -1} {
	regsub {\.} $tmpstring1 " " tmpstring2
	set tmpstring1 $tmpstring2
    }
    while {[string first , $tmpstring1] != -1} {
	regsub {,} $tmpstring1 "" tmpstring2
	set tmpstring1 $tmpstring2
    }

    #replace/fix upper 128 characters from USMARC character set
    # as appropriate (e.g., remove diacritics) so that we match
    # the server indexes.

    regsub -all \xD0 $tmpstring1 D tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xDE $tmpstring1 Th tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xC6 $tmpstring1 AE tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xA3|\xAE|\xB1 $tmpstring1 _ tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xF0 $tmpstring1 d tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xFE $tmpstring1 th tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xE6 $tmpstring1 ae tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xC0|\xC1|\xC2|\xC3|\xC4|\xC5 $tmpstring1 A tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xC8|\xC9|\xCA|\xCB $tmpstring1 E tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xCC|\xCD|\xCE|\xCF  $tmpstring1 I tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xD2|\xD3|\xD4|\xD5|\xD6|\xD8  $tmpstring1 O tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xD9|\xDA|\xDB|\xDC $tmpstring1 U tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xE0|\xE1|\xE2|\xE3|\xE4|\xE5 $tmpstring1 a tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xE8|\xE9|\xEA|\xEB $tmpstring1 e tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xB9|\xEC|\xED|\xEE|\xEF $tmpstring1 i tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xF2|\xF3|\xF4|\xF5|\xF6|\xF8 $tmpstring1 o tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xF9|\xFA|\xFB|\xFC $tmpstring1 u tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xD1 $tmpstring1 N tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xF1 $tmpstring1 n tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xFF $tmpstring1 y tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xC7 $tmpstring1 C tmpstring2
    set tmpstring1 $tmpstring2

    regsub -all \xE7 $tmpstring1 c tmpstring2
    set tmpstring1 $tmpstring2

    return $tmpstring1
}

#
# pTRECtag
#
# tag all non-stopword search terms in TREC records with a special color
# scheme to highlight them.  Similar to pOPACtag, but not nearly as
# nasty.  NOTE -- this function removes stopwords, but does not stem,
# so only exact matches for user's query terms get highlighted.

proc pTRECtag {txtlines i} {
    global searchTerm probSearchTerm currSearch stoplist defaultPath

    # BUILD TERM LIST
    # 1. get all search terms
    for {set i 0} {$i < [array size searchTerm]} {incr i 1} {
	append OriginalTerms "$searchTerm($i) "
    }
    if {$currSearch == "pr"} {
	append OriginalTerms "$probSearchTerm "
    }

    # 2. Remove stopwords
    # create stopword list if necessary

    if {[llength $stoplist] == 0} {
	set stopfile [open $defaultPath/stoplist r]
	while {[gets $stopfile line] >= 0} {
	    append stoplist "$line "
	}
	close $stopfile
    }
    foreach term $OriginalTerms {
	if {[lsearch -exact $stoplist $term] == -1} {
	    append TermList "$term "
	}
    }


    # USE TERM LIST TO HIGHLIGHT OCCURENCE OF TERMS IN RECORD
    # find the starting and ending line of record
    set pntLoc [string first " " $txtlines]
    set startLine [string range $txtlines 0 [expr $pntLoc - 1]]
    set endLine [string range $txtlines [expr $pntLoc + 1] end]

    # for each line in the record,
    #      for each term in the TermList
    #           if the current term occurs on this line, tag
    #           and highlight it
    for {set i $startLine} {$i < $endLine} {incr i} {
	foreach term $TermList {
	    set tmpstring [.retrieve.text get "$i.0" "$i.0 lineend"]
	    set lineLength [string length $tmpstring]
	    set j 0
	    # Handle case problems
	    set tmpstring [string tolower $tmpstring]
	    set term [string tolower $term]

	    while {$j < $lineLength} {
		if {[set start [string first $term $tmpstring]] != -1} {
		    set tstart [expr {$start + $j}]
		    set tagstart "$i.$tstart"
		    set tend [expr {$tstart + [string length $term]}]
		    set tagend "$i.$tend"
		    .retrieve.text tag add "TREC.HIGHLIGHT" $tagstart $tagend
		    set j [expr {$tend + 1}]
		    set tmpstring [string range $tmpstring [expr {$start + [string length $term]}] end]
		} else {
		    #stop this nonsense
		    set j $lineLength
		}
	    }
	}
	.retrieve.text tag configure "TREC.HIGHLIGHT" -background \
	    snow3
    }

}
