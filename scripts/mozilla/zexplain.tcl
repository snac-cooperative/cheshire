
puts stderr "Loaded: zexplain.tcl"

proc get_preferred_text {tlist} {
    global preferred_language

    regsub -all "\n" $tlist "<br>" tlist
    
    set text ""
    set defaulttext ""
    foreach item $tlist {
	set language [lindex [lindex $item 0] 1]
	if {$language == "($preferred_language)"} {
	    set text [lrange [lindex $item 1] 1 end]
	    break
	} elseif {$language == "(eng)"} {
	    set defaulttext [lrange [lindex $item 1] 1 end]
	}
    }

    if {$text != ""} {
	return [fix_text_list $text]
    } elseif { $defaulttext != "" } {
	return [fix_text_list $defaulttext]
    } else {
	return [fix_text_list [lrange [lindex [lindex $tlist 0] 1] 1 end]]
    }
}

proc fix_text_list {text} {
    #Get rid of {}s from around words (and anything else that comes up)
    # This won't be called on hrefs, as we generate them ourselves...
    # So we can add anchors for URLs

    regsub -all {\{} $text "" text
    regsub -all {\}} $text "" text
    regsub -all {(http|ftp|z3950|https|telnet|mailto|news):(//)?[^ <&]+} $text "<a href = \"&\">&</a>" text

    return $text

}

proc get_unit_value {valuelist} {
    #{{value:: 3} {unitUsed:: {{unitSystem:: } {unitType:: time} {unit:: Months} {scaleFactor:: 1}}}}

    set valuelist [lindex $valuelist 0]
    set text [lindex [lindex $valuelist 0] 1]
    append text " "
    set unitlist [lindex [lindex $valuelist 1] 1]
    append text [lindex [lindex $unitlist 2] 1]

    return $text

}


proc process_explain_recordSyntaxInfo {item} {

    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]
    set simpletext [lrange $item 1 end]
    
    if {$title == "recordSyntax"} {
	puts "<b>Record Syntax Identifier</b>: $simpletext<br>"
    } elseif {$title == "name"} {
	puts "<b>Record Syntax Name</b>: $simpletext<br>"
    }  elseif {$title == "description"} {
	set text [get_preferred_text $simpletext]
	puts "<b>Description</b>: $text"
    } else {
	# Too funky to handle
    }


}

proc process_explain_attributedetails {item} {
    
    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]
    

    if {$title == "databaseName"} {
	set info [lrange $item 1 end]
	puts "<center><h3>$info</h3></center>"
    } elseif { $title == "attributeCombinations" } {
	#What is this???
	# puts "<b>Attribute Combinations</b>: [lrange $item 1 end]<br>"
    } elseif { $title == "attributesBySet" } {
	# Forwards back into ourselves.
	set attrset [lindex [lindex $item 1] 0]
	set attrtype [lindex [lindex $item 1] 1]
	process_explain_attributedetails $attrset
	process_explain_attributedetails $attrtype
    } elseif {$title == "attributeSet"} {
	puts "<b>Attribute Set</b>: [lrange $item 1 end]<br>"
    } elseif {$title == "attributesByType"} {

	set info [lindex $item 1]
	set type [lindex [lindex $info 0] 1]
	set defaultattr [lindex [lindex $info 1] 1]
	set values [lindex $info 2]

	puts "<b>Type</b>: $type<br>"
	if {$defaultattr != ""} {
	    puts "Default: $defaultattr<br>"
	}
	
	if {[llength $values] > 1} {
	    puts "<b>Available Search Attributes</b>:"
	    puts "<p><table border = 1 width = 50%>"
	    foreach attr [lrange $values 1 end] {
		set value [lindex [lindex $attr 0] 1]
		set desc [get_preferred_text [lindex $attr 1]]
		set sub [lindex [lindex $attr 2] 1]
		set super [lindex [lindex $attr 3] 1]
		set partial [lindex [lindex $attr 4] 1]
		
		puts "<tr><td>$value</td><td>$desc</td>"
		
	    }
	    puts "</table>"
	}

    } else {
	puts "<i>Unhandled $title</i>: [lrange $item 1 end]<br>"
    }
}

proc process_explain_targetinfo  {item welcome_message} {
    global URI_HOST URI_PORT

    array set targetinfo_text_array {Recent_news "News" namedResultSets "Supports Named Result Sets" multipleDBsearch "Supports Multiple Database Searching" maxResultSets "Maximum Result Sets" maxResultSize "Maximum Result Size" maxTerms "Maximum Search Terms" Contact_Info "Contact Information" Nickname "Server Nickname" Usage_restrictions "Usage Restrictions" Payment_address "Payment Address" Description "Description" Hours "Hours Available" DB_combinations "Database Combinations"}
    set targetinfo_text_list [array names targetinfo_text_array]

    set unsupported_titles {Icon CommonAccessInfo CommonInfo Welcome_Message}

    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]

    if {[lsearch $targetinfo_text_list $title] != -1} {
	if {[llength $item] != 1} {
	    set info [lrange $item 1 end]

	    if {$info == "FALSE"} {
		set info "No"
	    } elseif {$info == "TRUE" } {
		set info "Yes"
	    } elseif {[regexp {\{text:: ([^\}]+)} $info null text]} {
		set info [get_preferred_text $info]
	    } elseif {[regexp {\{actualNumber:: ([^\}]+)} $info null text]} {
		set info $text
	    }

	    puts "<b>$targetinfo_text_array($title)</b>: $info<br>"
	}
    } elseif {$title == "TimeoutInterval"} {
	set info [get_unit_value [lrange $item 1 end]]
	puts "<b>Idle Timeout Interval</b>: $info<br>"
    } elseif {$title == "Target_Name"} {
	puts "<center><h3>[lrange $item 1 end]</h3></center>"
	puts $welcome_message
    } elseif {$title == "NetworkAddress"} {
	#Handle Network address
	set host [lindex [lindex [lindex [lindex $item 1] 1] 1] 1]
	set port [lindex [lindex [lindex $item 1] 1] 2]
	set port [string range $port 6 end]
	set url "z3950://$host:$port/"
	set info "<a href = \"$url\">$url</a>"

	puts "<b>Address</b>: $info<br>"
    } else {
	if {[lsearch $unsupported_titles $title] == -1} {
	    puts "<i>Unsupported $title</i>: [lrange $item 1 end]<br>"
	}
    }

}


proc process_explain_commoninfo {item} {

    array set commoninfo_text_arry {DateAdded "Date Added" DateChanged "Date Changed" Expiry "Expiry" Language "Language" OtherInfo "Other Information"}
    set commoninfo_text_list [array names explain_text_array]

    puts "<b>Common Information</b>:<p>"

    foreach entry [lrange $item 1 end] {
	set title [lindex $entry 0]
	set title [string range $title 0 [expr [string length $title] -3]]
	if {[lsearch $commoninfo_text_list $title] != -1} {
	    set title $commoninfo_text_array($title)
	}

	set info [lrange $entry 1 end]
	if {$info != ""} {
	    puts "<b>$title</b>: $info<br>"
	}

    }

}

proc process_explain_databaseInfo {item index has_title do_commoninfo} {
    global currdbname URI_HOST URI_PORT

    array set explain_text_array {user_fee "Requires Payment" available "Available" Description  "Description" Disclaimers "Disclaimers" News  "News" DefaultOrder "Default Record Order" avRecordSize "Average Record Size" maxRecordSize "Maximum Record Size" Hours "Hours of Operation" BestTime "Best Access Time" LastUpdate "Last Updated" coverage "Dates Covered" proprietary "Contains Proprietary Information" CopyrightText "Copyright Issues" CopyrightNotice "Notice of Copyright" ProducerContactInfo "Database Producer" SupplierContactInfo "Database Supplier" SubmissionContactInfo "Database Entry Submissions" RecordCount "Number of Entries" Nickname "Database Nickname"}

    set unsupported {Icon AccessInfo}


    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]
    set explain_text_list [array names explain_text_array]

    if {[lsearch $explain_text_list $title] != -1} {
	if {[llength $item] != 1} {
	    set info [lrange $item 1 end]

	    if {$info == "FALSE"} {
		set info "No"
	    } elseif {$info == "TRUE" } {
		set info "Yes"
	    } elseif {[regexp {\{text:: ([^\}]+)} $info null text]} {
		set info [get_preferred_text $info]
	    } elseif {[regexp {\{actualNumber:: ([^\}]+)} $info null text]} {
		set info $text
	    }

	    puts "<b>$explain_text_array($title)</b>: $info<br>"
	}
    } elseif {$title == "titleString"} {
	set info [get_preferred_text [lrange $item 1 end]]
	if {$info != ""} {
	    puts "<center><b>$info</b></center><p>"
	}
    } elseif {$title == "DatabaseName" } {
	set info [lrange $item 1 end]
	set currdbname $info
	if {!$has_title} {
	    puts "Database $index: <center><b>$info</b></center>"
	} else {
	    puts "<b>Database Name</b>: $info<br>"
	}
    } elseif {$title == "UpdateInterval"} {
	set info [get_unit_value [lrange $item 1 end]]
	puts "<b>Updated Every</b>: $info<br>"
    } elseif { $title == "explainDatabase" } {
	set info [lindex $item 1]
	set txt "<b>Is an Explain Database</b>: "

	if {$info == "NO" } {
	    append txt "No<br>"
	} else {
	    append txt "<a href = \"z3950://$URI_HOST:$URI_PORT/explain/?db=$currdbname\">Yes</a><br>"
	}
	puts $txt

    } elseif {$title == "AssociatedDBs" || $title == "SubDBs"} {
	# Assume this will be list of db names

	if { [llength $item] != 1} {
	    set title [string range $title 0 [expr [string length $title] -4]]
	    append title " Databases"
	    set txt "<ul>"
	    foreach db [lrange $item 1 end] {
		append txt "<li><a href = \"z3930://$URI_HOST:$URI_PORT/explain/databaseinfo?db=$db\">$db</a>"
	    }	    
	    append txt "</ul>"
	    puts "<b>$title</b>: $txt"
	}
    } elseif {$title == "CommonInfo"} {
	if {$do_commoninfo} {
	    #We're only displaying one db, so need to process this
	    process_explain_commoninfo $item
	}
    } else {

	if {[lsearch $unsupported $title] == -1} {
	     puts "<i>Unable to handle '$title'</i><br>"
	    puts "<pre>$item</pre>"

	}
    }
}

proc do_explain_categoryInfo {title} {
    global URI_HOST URI_PORT explain_database

    array set category_desc_array {categoryinfo "Description of available Explain categories." databaseinfo "Description of the available databases." recordsyntaxinfo "Description of the available record syntaxes." targetinfo "Description of the server." attributedetails "Description of the available Attribute sets." categorylist "Description of available Explain categories."}
    set results [zfind explainCategory categoryList]
    set sresults [zdisplay]
    set clist [lindex [lindex $sresults 1] 2]
    set clist [lrange $clist 1 end]

    puts "<b>$title</b>:<ul>"
    if {$explain_database != "IR-Explain-1"} {
	set hrefquery "?db=$explain_database"
    } else {
	set hrefquery ""
    }

    foreach category $clist {
	set type [lindex [lindex $category 1 ] 1];
	set desc [lindex [lindex $category 3] 1];
	set text [get_preferred_text [list $desc]]

	if {$text == ""} {
	    set text $category_desc_array([string tolower $type])
	}
	if {$type != "categoryInfo"} {
	    puts "<li><a href = \"z3950://$URI_HOST:$URI_PORT/explain/$type$hrefquery\">$text</a>"
	} else {
	    puts "<li>$text (above)"
	}
    }
    puts "</ul>"
}

proc search_explain_attributedetails {} {
    global URI_HOST URI_PORT

    throw_html_type
    puts "<html><head><title>Attribute Details</title></head><body bgcolor = white><center><h3>Attribute Details</h3></center><p>"

    set results [zfind explainCategory attributeDetails]
    set hits [lindex [lindex [lindex $results 0 ] 2] 1]

    #Need to do commoninfo trick again
    set item [zdisplay 1]
    set do_commoninfo 0

    if { [regexp {\{CommonInfo:: ([^\}]+)\}} $item] } {
	set cinfo [lindex [lindex $item 1] 1]
	process_explain_commoninfo $cinfo
    }
    puts "<hr>"

    for {set idx 1} {$idx <= $hits} {incr idx} {
	set item [lindex $item 1]

	foreach entry [lrange $item 2 end] {
	    process_explain_attributedetails $entry
	}
	puts "<hr>"
	if {$idx != $hits} {
	    set item [zdisplay 1]
	}
    }
    puts "<p><hr></body></html>"

}


proc search_explain_indexes {} {
    return 0
    global URI_HOST URI_PORT database
    
    zselect explain $URI_HOST IR-Explain-1 $URI_PORT
    zset attributes explain
    zset recsyntax explain
    set results [zfind databasename $database]
    set disp [zdisplay]
    throw_html_type

    # When we can find the indexes available from this explain query, we'll write it
    #...
}


proc search_explain_targetinfo {} {
    # Isn't working in cheshire! Bad Cheshire!
    
    throw_html_type
    puts "<html><body bgcolor = white>"
    set sresults [zfind explainCategory targetInfo]
    set results [zdisplay]
    set results [lindex $results 1]
    
    #Find Welcome_Message
    set welcome_message ""
    foreach item [lrange $results 1 end] {
	if {[lindex $item 0] == "Welcome_Message::"} {
	    set wm [lrange $item 1 end]
	    set welcome_message [get_preferred_text $wm]
	    append welcome_message "<p>"
	    break
	}
    }

    foreach item [lrange $results 1 end] {
	process_explain_targetinfo $item $welcome_message
    }
    puts "<p><hr></body></html>"
}

proc search_explain_recordsyntaxinfo {} {
    throw_html_type
    puts "<html><body bgcolor = white><center><h3>Record Syntaxes</h3></center><p>"
    set results [zfind explainCategory recordSyntaxInfo]
    set hits [lindex [lindex [lindex $results 0] 2] 1]

    puts "<hr>"
    for {set idx 1} {$idx <= $hits} {incr idx} {
	set recitem [lindex [zdisplay 1] 1]
	foreach item [lrange $recitem 1 end] {
	    process_explain_recordSyntaxInfo $item
	}
	puts "<p><hr><p>"
    }
    puts "</body></html>"
}

proc search_explain_categorylist {} {
    #These two are the same
    search_explain_categoryinfo
}

proc search_explain_categoryinfo {} {
    global URI_HOST URI_PORT
    #This shouldn't be called, but we'll support it incase someone types it in as a location
    throw_html_type
    puts "<html><head></head><body bgcolor = white><center><h3>Available Explain Categories</h3></center><p>"
    do_explain_categoryInfo "Supported Explain Categories"
}

proc search_explain_databaseinfo {} {
    global URI_HOST URI_PORT currdbname searchterms
    set results [zfind explainCategory DatabaseInfo]
    set hits [lindex [lindex [lindex $results 0 ] 2] 1]

    throw_html_type
   puts "<html>\n<head>\n<title>Databases on z39.50 server: $URI_HOST</title>\n</head>\n<body bgcolor = white>\n<center><h3>Databases on z39.50 server: $URI_HOST</h3></center>\n<p>"
    puts "<hr>"
    puts "<b>Total number of Databases</b>: $hits<p>"

    # Want to process commoninfo once, strangely enough.
    # Get first record and check for CommonInfo::
    set disp [zdisplay 1]
    set do_commoninfo 0

    if { [regexp {\{CommonInfo:: ([^\}]+)\}} $disp] } {
	set cinfo [lindex [lindex $disp 1] 1]
	process_explain_commoninfo $cinfo
    }
    puts "<hr>"
    for {set idx 1} {$idx <= $hits} {incr idx} {
	set db [lindex $disp 1]
	set has_title [regexp {\{titleString:: ([^\}]+)\}} $db]
	foreach section [lrange $db 1 end] { 
	    process_explain_databaseInfo $section $idx $has_title $do_commoninfo
	}
	puts "<p><a href = \"z3950://$URI_HOST:$URI_PORT/search/$currdbname\">Search this Database</a><p>"
	puts "<hr>"
	# This is at the end as we pop one off the stack to look for commoninfo
	if {$idx != $hits} {
	    set disp [zdisplay 1]
	}
    }
    puts "</body></html>"
    return 1
}

# If URL is just explain/ or just server
#Note that we check availability of /explain/type so this won't get called from /explain/host
proc search_explain_host {} {
    global URI_HOST URI_PORT
    set err [catch {zshow} zsresults]
    set lines [split $zsresults "\n"]

    foreach line $lines {
	if {[set tidx [lsearch {Host Server Support Port} [lindex $line 0]]] != -1} {
	    if {$tidx == 0} {
		if {[lindex $line 1] == "Name" } {
		    set serverinfo(hostname) [lindex $line 2]
		} elseif {[lrange $line 1 2] == "IP Address"} {
		    set serverinfo(ip) [lrange $line 3 end]
		}
	    } elseif {$tidx == 1 } {
		set serverinfo([lindex $line 1]) [lrange $line 2 end]
	    } elseif {$tidx == 2 } {
		set supportinfo([lrange $line 1 [expr [llength $line] -2]]) [lindex $line [expr [llength $line] -1]]
	    } else {
		set serverinfo(port) [lindex $line 1]
	    }
	} elseif { [ lindex $line 0] == "CLIENT" } {
	    break
	}
    }

    throw_html_type
    puts "<html>\n<head>\n<title>Z3950 Server: $serverinfo(hostname)</title>\n</head>\n<body bgcolor = white>\n<center><h3>Z3950 Server: $serverinfo(hostname)</h3></center>\n<p>"
    puts "<b>Z39.50 Server</b>: $serverinfo(ImplementationName), version $serverinfo(ImplementationVersion)<p>"
    puts "<b>Supported Protocol Versions</b>:"

    for {set v 1} {$v < 4} {incr v} {
	if { $serverinfo(SupportVersion$v) == 1 } {
	    puts "version $v "
	}
    }
    puts "<p>"
    puts "<b>Supported Requests:</b><ul>"

    foreach support [array names supportinfo] {
	if {$supportinfo($support) == 1} {
	    puts "<li>$support"
	}
    }
    puts "</ul>"
    puts "<p><b>Supported Query Types:</b><ul>"

    foreach support {Type0Query Type1Query Type2Query Type100Query Type101Query Type102Query} {
	if {$supportinfo($support) == 2} {
	    puts "<li>$support"
	}
    }
    puts "</ul>"
    do_explain_categoryInfo "Further Information"
    puts "<p><hr></body></html>"
}


proc search_explain_type {type} {
    global URI_HOST URI_PORT
    #Error checking to do:  Query categoryTypes to make sure we support it!


    set err [ catch { zfind explainCategory categoryList } results ]

    if {$err == 0} {
	set sresults [zdisplay]
	set clist [lindex [lindex $sresults 1] 2]
	set clist [lrange $clist 1 end]
	set supported 0
	set checktype [string tolower $type]
	
	foreach category $clist {
	    set cattype [lindex [lindex $category 1 ] 1]
	    set catchecktype [string tolower $cattype]
	    if {$checktype == $catchecktype} {
		set supported 1
		set type $cattype
		break
	    }
	}
	if {$supported == 0} {
	    exit_error "Unsupported Explain type: $type"
	}
    }

    set type [string tolower $type]
    search_explain_$type
    exit
}

