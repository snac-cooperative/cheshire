#!../bin/webcheshire
#

# Global Server Configuration
set CHESHIRE_DATABASE archives
set CHESHIRE_CONFIGFILE "CONFIG.ARCH_HUB"
set CHESHIRE_NUM_START 0
set CHESHIRE_NUMREQUESTED 1
set CHESHIRE_RECSYNTAX grs1

set controlaccess_tags {SUBJECT GEOGNAME PERSNAME}


#---------------------------------------------------------

proc flattext {list} {
    set list [string trim $list]
    if {[string index $list 0] == "\{" } {
        set contents {};
        while { [regexp {content:: \{([^\}]+)?\}} $list null content] } {
            if {$content != "elementEmpty"} {
                lappend contents $content
            }
            regsub -all {\[} $null {\\\[} null
            regsub -all {\]} $null {\\\]} null
            regsub -all {\\} $null {\\\\} null
            regsub -all {\(} $null {\\(} null
            regsub -all {\)} $null {\\)} null
            regsub -all {\$} $null {\\$} null
            regsub -all {\^} $null {\\^} null
            set oldlist $list
            regsub $null $list "" list
            if { $oldlist == $list } {
                # Ohoh! Craaaaaaaaaash out
                break
            }
        }
        return [join $contents]

    } else {
        return $list;
    }
}

proc sub_elem_contents {index list} {
    
    set list [lindex [lindex [lindex $list 1] 1] 1];
    set list [lindex $list $index]
    set list [lindex [lindex [lindex $list 1] 1] 1];

    if {$list == "elementEmpty"} {
	return "";
    } else {
 	return $list;
    }
}

proc sub_elem_index {name list} {
    set list [lindex [lindex [lindex $list 1] 1] 1];

    set idx 0;
    foreach item $list {
	if { [lindex $item 0] == "SUBELEMENT::" } {
	    set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1]
	    if { $elemname == $name } {
		return $idx;
	    }
 	}
 	incr idx;
    }
    return -1;
}
    
proc contents_elem_index {name list} { 
    set list [lindex $list 1]; 
 
    set idx 0;
    foreach item $list {
	if { [lindex $item 0] == "SUBELEMENT::" } {
	    set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1]
	    if { $elemname == $name } {
		return $idx;
	    } 
	} 
 	incr idx;
    } 
    return -1; 
} 
    

proc is_attr attritem {

    set data [lindex $attritem 1]
    if { [llength $data] == 3 } {
	return 1;
    } else {
	return 0;
    }
}

proc processitem item {
    global element_order fileh firstsubject lastsubject controlaccess_tags
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
		print "$string"
	    }

	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem $nitem 
	    } 
	} 

    } elseif { [lindex $item 0] == "SUBELEMENT::" } { 
	set elemname [lindex [ lindex [ lindex [ lindex  $item 1] 0] 0] 1] 
	if {[llength [lindex $item 1]] == 3} { 
	    set data [ lindex [ lindex [ lindex $item 1] 1] 1]
	    set data [string trim $data]
	    puts -nonewline $fileh " $elemname = \"$data\""
	    return 
	}  
	
	#Ignore tag name 19 - it's a cheshire holder for plain
	if { $elemname != "19" } {
	    lappend element_order $elemname 
	    set llen [llength $element_order]
	    for {set s 0} {$s < $llen} {incr s} {
		puts -nonewline $fileh " "
	    }
	    puts -nonewline $fileh "<$elemname"
	}



	#Check for and remove attrs off front
	set contents [lindex [lindex [lindex $item 1] 1] 1]

	while { $contents != {} && [is_attr [set attritem [lindex $contents 0]]] } {

	    processitem $attritem
	    if { [llength $contents ] > 1 } {
		set contents [lrange $contents 1 end]
	    } else {
		set contents {}
	    }
	}

	#Close tag unless content wrapper
	if { $elemname != "19" } {
	    print ">"
	}

#--------------	    
	if {[lsearch $controlaccess_tags $elemname] != -1 && [lsearch $element_order CONTROLACCESS ] != -1} {
	    #Some have 'title' in CA. Bad. Flattext them.
	    set string [ flattext $contents ]
	    #Subtagerise
	    set splitstr " NT1 | NT2 |UF | NT3 |RT | x | xx |--|---| - "

	    if {$elemname == "PERSNAME" && [string first "|" $string] == -1} {
		append splitstr "|, "
	    }
		

	    regsub -all $splitstr $string "\|" string;
	    set stringarr {}
	    while { [ regexp {^([^|/]+)?([|/])(.+)$} $string null first delim second] } {
		lappend stringarr $first
		set string $second
	    }
	    lappend stringarr $string
	    set strlen [llength $stringarr]

	    if {$strlen == 1 && $elemname == "PERSNAME" } {
		# just write out, we're not split at all
		print $string
	    } else {
	    

		set idxmod 0
		for {set idx 0} {$idx < $strlen} {incr idx} {
		    set bit [lindex $stringarr $idx]
		    set bit [string trim $bit]

		    if {$bit == "" } {
			#strip & play with $idx
			incr idxmod
			continue
		    }

		    #Here's more mess
		    #We should check repositories somehow?  Maybe by docid?
		    if {$elemname == "PERSNAME"} {
			if {$idx < [expr 5 - $idxmod] } {
			    set type [lindex {surname forename dates epithet title} [expr $idx - $idxmod]]
			} else {
			    set type "other"
			}

			#Sanity check for dates both ways
			if {$type == "dates" && [regexp {^((fl|b|d)(.*))?[0-9][0-9]} $bit] == 0 } {
			    set type "epithet"
			}
			if {$type != "dates" && [regexp {^((fl|b|d)(.*))?[0-9][0-9]} $bit] } {
			    set type "dates"
			}
			    
			#Hack in 'sir' etc automagically
			set teststr [string tolower $bit]
			if {$teststr == "sir" || $teststr == "lady" || $teststr == "lord" } {
			    set type "title"
			    incr idxmod
			}
			
		    } else {
			if {[expr $idx - $idxmod ] == 0} {
			    set type $firstsubject
			} elseif {$idx == [expr $strlen -1]} {
			    set type $lastsubject
			} else {
			    set type "x"}
		    }
		    
		    print "<EMPH ALTRENDER = \"$type\">$bit</EMPH> "
		}
	    }

		set llen [llength $element_order]
	    for {set s 0} {$s < $llen} {incr s} {
		puts -nonewline $fileh " "
	    }
	    print "</$elemname>"
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	    return
	}
#--------------
	set contents [ list "content::" $contents]

	# --- Process sub items of list  ---
	processitem $contents


	# Things to be done on close of element
	if {$elemname != "19"} { 
	    set llen [llength $element_order]
	    for {set s 0} {$s < $llen} {incr s} {
		puts -nonewline $fileh " "
	    }
	    print "</$elemname>"
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	}

    } else {
	#Debug unknown grs1 syntax
	puts "Unknown:: '$item'"
    }
}

proc print text {
    global fileh
    puts $fileh $text
}


set MAX_ID 2758

for {set docid 1} {$docid <= $MAX_ID} {incr docid} {

    set query "search docid $docid"
    set err [catch {eval $query} qresults]
    set sresults [lrange $qresults 1 end]

    #Strip headers to ead tag, and play with results a bit
    set sresults [lindex [ lindex $sresults 0] 1]
    regsub -all "{tagType:: 3}" $sresults "" sresults
    regsub -all "{tagType:: 2}" $sresults "" sresults
    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
    regsub -all "\"" $sresults {\&quot;} sresults
    
    #Write SGML file
    set filename "newdata/$docid.sgml"
    set fileh [open $filename w]
    global fileh

    set firstsubject "a"
    set lastsubject "a-"

    puts "Writing $filename..."
    print "<EAD>"
    foreach item [lrange $sresults 1 end] {
	processitem $item
    }
    print "</EAD>"
    
    close $fileh
}




