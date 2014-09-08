proc get_preferred_text {tlist} {
    global preferred_language

    regsub "\n\}\}$" $tlist "\}\}" tlist
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
    global HTML_RESULT

    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]
    set simpletext [lrange $item 1 end]
    
    if {$title == "recordSyntax"} {
	append HTML_RESULT "<b>Record Syntax Identifier</b>: $simpletext<br>\n"
    } elseif {$title == "name"} {
	append HTML_RESULT  "<b>Record Syntax Name</b>: $simpletext<br>\n"
    }  elseif {$title == "description"} {
	set text [get_preferred_text $simpletext]
	append HTML_RESULT "<b>Description</b>: $text\n"
    } else {
	# Too funky to handle
    }
}

proc process_explain_attributedetails {item} {
    global HTML_RESULT bib1nums bib1names oidnames DATABASENAME
    
    set title [lindex $item 0]
    set title [string range $title 0 [expr [string length $title] -3]]

    if {$title == "databaseName"} {
	set info [lrange $item 1 end]

	set DATABASENAME $info
	set titlestr [get_explain_titleString]
	if {$titlestr == "" || $titlestr == $info} {
	    append HTML_RESULT "<center><h3>$info</h3></center>"
	} else {
	    append HTML_RESULT "<center><h3>$titlestr</h3></center><center><b>$info</b></center>"
	}

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
	set oid [lrange $item 1 end]
	if {[lsearch [array names oidnames] $oid] != -1 } {
	    set oid $oidnames($oid)
	}
	
	append HTML_RESULT "<b>Attribute Set</b>: $oid<br>"
    } elseif {$title == "attributesByType"} {

	set info [lindex $item 1]
	set type [lindex [lindex $info 0] 1]
	set defaultattr [lindex [lindex $info 1] 1]
	set values [lindex $info 2]

	append HTML_RESULT "<b>Type</b>: $type<br>\n"
	if {$defaultattr != ""} {
	    append HTML_RESULT "Default: $defaultattr<br>\n"
	}
	
	if {[llength $values] > 1} {
	    append HTML_RESULT "<b>Available Search Attributes</b>:\n"
	    append HTML_RESULT "<p><table border = 1 width = 50%>\n"
	    foreach attr [lrange $values 1 end] {
		set value [lindex [lindex $attr 0] 1]
		set desc [get_preferred_text [lindex $attr 1]]

		if {$desc == ""} {
		    if {[set uidx [lsearch $bib1nums $value]] != -1} {
			set desc [lindex $bib1names $uidx]
		    } else {
			set desc "Unknown Field"
		    }
		}

		set sub [lindex [lindex $attr 2] 1]
		set super [lindex [lindex $attr 3] 1]
		set partial [lindex [lindex $attr 4] 1]
		
		append HTML_RESULT "<tr><td>$value</td><td>$desc</td>\n"
		
	    }
	    append HTML_RESULT "</table>"
	}

    } else {
	append HTML_RESULT "<i>Unhandled $title</i>: [lrange $item 1 end]<br>\n"
    }
}

proc process_explain_targetinfo  {item welcome_message} {
    global URI_HOST URI_PORT HTML_RESULT

    array set targetinfo_text_array {Recent_news "News" namedResultSets "Named Result Sets" multipleDBsearch "Multiple Database Search" maxResultSets "Maximum Result Sets" maxResultSize "Maximum Result Size" maxTerms "Maximum Search Terms" Contact_Info "Contact Information" Nickname "Server Nickname" Usage_restrictions "Usage Restrictions" Payment_address "Payment Address" Description "Description" Hours "Hours Available" DB_combinations "Database Combinations"}
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

	    append HTML_RESULT "<tr><td><b>$targetinfo_text_array($title):</b></td><td> $info</td></tr>"
	}
    } elseif {$title == "TimeoutInterval"} {
	set info [get_unit_value [lrange $item 1 end]]
	append HTML_RESULT "<tr><td><b>Idle Timeout Interval:</b></td><td>$info</td></tr>"
    } elseif {$title == "Target_Name"} {
	append HTML_RESULT "<tr><td colspan = 2><center><h3>[lrange $item 1 end]</h3></center></td></tr>"
	append HTML_RESULT "<tr><td colspan = 2>$welcome_message</td></tr>"
    } elseif {$title == "NetworkAddress"} {
	#Handle Network address
	set host [lindex [lindex [lindex [lindex $item 1] 1] 1] 1]
	set port [lindex [lindex [lindex $item 1] 1] 2]
	set port [string range $port 6 end]
	set url "z3950://$host:$port/"
	set info "<a href = \"$url\">$url</a>"

	append HTML_RESULT "<tr><td><b>Address:</b></td><td>$info</td></tr>"
    } else {
	if {[lsearch $unsupported_titles $title] == -1} {
	    append HTML_RESULT "<i>Unsupported $title</i>: [lrange $item 1 end]<br>\n"
	}
    }

}


proc process_explain_commoninfo {item} {
    global HTML_RESULT

    array set commoninfo_text_arry {DateAdded "Date Added" DateChanged "Date Changed" Expiry "Expiry" Language "Language" OtherInfo "Other Information"}
    set commoninfo_text_list [array names explain_text_array]

    append HTML_RESULT "<b>Common Information</b>:<p>\n"

    foreach entry [lrange $item 1 end] {
	set title [lindex $entry 0]
	set title [string range $title 0 [expr [string length $title] -3]]
	if {[lsearch $commoninfo_text_list $title] != -1} {
	    set title $commoninfo_text_array($title)
	}

	set info [lrange $entry 1 end]
	if {$info != ""} {
	    append HTML_RESULT "<b>$title</b>: $info<br>\n"
	}

    }

}

proc process_explain_databaseInfo {item index has_title do_commoninfo} {
    global currdbname URI_HOST URI_PORT HTML_RESULT oidnames searchterms

    array set explain_text_array {user_fee "Requires Payment" available "Available" Description  "Description" Disclaimers "Disclaimers" News  "News" DefaultOrder "Default Record Order" avRecordSize "Average Record Size" maxRecordSize "Maximum Record Size" Hours "Hours of Operation" BestTime "Best Access Time" LastUpdate "Last Updated" coverage "Dates Covered" proprietary "Proprietary Information" CopyrightText "Copyright Issues" CopyrightNotice "Notice of Copyright" RecordCount "Number of Entries" Nickname "Database Nickname" name "Name" description "Description" address "Address" email "Email Address" phone "Telephone Number" diagnosticsSets "Diagnostic Sets" AttributeSetIds "Attribute Sets" Schemas "Schemas" RecordSyntaxes "Record Syntaxes" ResourceChallenges "Resource Challenges" AccessRestrictions "Access Restrictions" Costs "Costs" VariantSets "Variant Sets" ElementSetNames "Display Formats" UnitSystems "Unit Systems"}

    set unsupported {Icon}
    set summaryfields {available Description News LastUpdate CopyrightText RecordCount name address email}



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
	    } elseif {[regexp {\{text::([^\}]+)} $info null text]} {
		set info [get_preferred_text $info]
	    } elseif {[regexp {\{approxNumber:: ([^\}]+)} $info null text]} {
		set info "Approximately $text"
	    } elseif {[regexp {\{actualNumber:: ([^\}]+)} $info null text]} {
		set info $text
	    }
	    if {$searchterms(format) != "summary" || [lsearch $summaryfields $title ] != -1 && $info != ""}  {
		append HTML_RESULT "<tr><td valign = top><b>$explain_text_array($title):</b></td><td> $info</td></tr>"
	    }
	}
    } elseif {[regexp "ContactInfo$" $title] } {
	set type [string range $title 0 [expr [string length $title] - 12]]
	# set info [lrange $item 1 end]
	if { [llength $item] != 1} {
	    append HTML_RESULT "<tr><td colspan = 2><b>$type Contact Information:</b></td></tr>"
	    append HTML_RESULT "<tr><td colspan = 2><blockquote><table>"
	    foreach bit [lrange [lindex $item 1] 1 end] {
		process_explain_databaseInfo $bit 0 1 0
	    }
	    append HTML_RESULT "</table></blockquote></td></tr>"
	}
    } elseif {$title == "titleString"} {
	set info [get_preferred_text [lrange $item 1 end]]
	if {$info != ""} {
	    append HTML_RESULT "<tr><td colspan = 2><p><center><b>$info</b></center><p></td></tr>"
	}
    } elseif {$title == "DatabaseName" } {
	set info [lrange $item 1 end]
	set currdbname $info
	if {$has_title == 0} {
	    append HTML_RESULT "Database $index: <center><b>$info</b></center>\n"
	} else {
	    append HTML_RESULT "<tr><td><b>Database Name:</b></td><td>$info</td></tr>"
	}
    } elseif {$title == "UpdateInterval"} {
	set info [get_unit_value [lrange $item 1 end]]
	if {$info != "" && $info != " "} {
	    append HTML_RESULT "<tr><td><b>Updated Every:</b></td><td>$info</td></tr>"
	}
    } elseif { $title == "explainDatabase" } {
	if {$searchterms(format) != "summary"} {
	    set info [lindex $item 1]
	    set txt "<tr><td><b>Is an Explain Database:</b></td><td>"
	    
	    if {$info == "NO" } {
		append txt "No</td></tr>"
	    } else {
		append txt "<a href = \"z3950://$URI_HOST:$URI_PORT/explain/?db=$currdbname\">Yes</a></td></tr>"
	    }
	    append HTML_RESULT "$txt"
	}

    } elseif {$title == "AssociatedDBs" || $title == "SubDBs"} {
	# Assume this will be list of db names
	if {$searchterms(format) != "summary"} {
	    if { [llength $item] != 1} {
		set title [string range $title 0 [expr [string length $title] -4]]
		append title " Databases"
		set txt "<ul>"
		foreach db [lrange $item 1 end] {
		    append txt "<li><a href = \"z3930://$URI_HOST:$URI_PORT/explain/databaseinfo?db=$db\">$db</a>"
		}	    
		append txt "</ul>"
		append HTML_RESULT "<tr><td><b>$title:</b></td><td>$txt</td></tr>"
	    }
	}
    } elseif {$title == "CommonInfo"} {
	if {$do_commoninfo} {
	    #We're only displaying one db, so need to process this
	    process_explain_commoninfo $item
	}
    } elseif { $title == "AccessInfo"} {
	
	if {$searchterms(format) != "summary"} {
	    
	    set info [lindex $item 1]
	    append HTML_RESULT "<tr><td colspan =2><b>Access Information:</b></td></tr><tr><td colspan =2><blockquote><table>"
	    
	    foreach bit $info {
		set title [lindex $bit 0]
		set title [string range $title 0 [expr [string length $title] -3]]
		
		if {[string tolower $title] != "querytypessupported" } {
		    set htitle $explain_text_array($title)
		    # We don't care at the moment...
		    set cont [lrange $bit 1 end]
		    
		    if {[lsearch [list "diagnosticssets" "attributesetids" "recordsyntaxes" "schemas"]  [string tolower $title] ] != -1 } {
			set ncont ""
			foreach oid $cont {
			    if {[lsearch [array names oidnames] $oid] != -1 } {
				append ncont "$oidnames($oid) "
			    } else {
				append ncont $oid
			    }
			}
			set cont $ncont
		    }		
		    
		    if {$cont != ""} {
			append HTML_RESULT "<tr><td><b>$htitle:</b></td><td>$cont</td></tr>"
		    }
		}
	    }
	    append HTML_RESULT "</table></blockquote></td></tr>"
	}

    } else {

	if {[lsearch $unsupported $title] == -1} {
	     append HTML_RESULT "<i>Unable to handle '$title'</i><br>\n"
	    append HTML_RESULT "<pre>$item</pre>\n"

	}
    }
}

proc process_explain_retrievalrecorddetails {item} {
    global HTML_RESULT
    
    set type [lindex $item 0]

    if {$type == "RetrievalRecordDetails::"} {
	foreach i [lrange $item 1 end] {
		process_explain_retrievalrecorddetails $i
	    }
	
    } elseif {$type == "DatabaseName::"} {
	append HTML_RESULT "<br><b>Database:</b> [lrange $item 1 end]"
    } elseif {$type == "RecordSyntax::"} {
	append HTML_RESULT "<br><b>Record Syntax Id:</b> [lindex $item 1]"
    } elseif {$type == "Schema::" } {
	append HTML_RESULT "<br><b>Record Schema Id:</b> [lindex $item 1]"
    } elseif {$type == "Description::"} {
	append HTML_RESULT "<br><b>Description:</b> [lrange $item 1 end]"
    } elseif { $type == "DetailsPerElement::"} {
	append HTML_RESULT "<br><b>Database Fields:</b>"
	foreach i [lrange $item 1 end] {
	    process_explain_retrievalrecorddetails [lindex $i 0]
	}
    } elseif { $type == "name::"} {
	append HTML_RESULT " [lindex $item 1]"
    }
}

proc do_explain_categoryInfo {title} {
    global URI_HOST URI_PORT HTML_RESULT QUERY 

    array set category_desc_array {categoryinfo "Description of available Explain categories." databaseinfo "Description of the available databases." recordsyntaxinfo "Description of the available record syntaxes." targetinfo "Description of the server." attributedetails "Description of the available Attribute sets." categorylist "Description of available Explain categories."}

    set QUERY "zfind explainCategory categoryList"
    set results [try_esearch]

    if {[lindex $results 0] != 0} {
	set sresults [try_edisplay 1]

	if {[lindex $sresults 0] != 0} {
	    
	    set clist [lindex [lindex $sresults 1] 2]
	    set clist [lrange $clist 1 end]
	    
	    append HTML_RESULT "<b>$title</b>:<blockquote><table width = 100%>\n"
	    #	if {$explain_database != "IR-Explain-1"} {
	    #	    set hrefquery "?db=$explain_database"
	    #	} else {
	    set hrefquery ""
	    #	}
	    
	    foreach category $clist {
		set type [lindex [lindex $category 1 ] 1];
		set desc [lindex [lindex $category 3] 1];
		set text [get_preferred_text [list $desc]]
		
		if {$text == ""} {
		    set text $category_desc_array([string tolower $type])
		}
		if {$type != "categoryInfo"} {
		    if {[string tolower $type] == "databaseinfo"} {
			append HTML_RESULT "<tr><td><a href = \"z3950://$URI_HOST:$URI_PORT/explain/$type$hrefquery\">$text</a></td><td>(<a href =\"z3950://$URI_HOST:$URI_PORT/explain/$type$hrefquery?format=summary\">Summary</a>)</td></tr>\n"
		    } else {
			append HTML_RESULT "<tr><td colspan=2><a href = \"z3950://$URI_HOST:$URI_PORT/explain/$type$hrefquery\">$text</a></td></tr>\n"

		    }
		} else {
		    append HTML_RESULT "<tr><td colspan =2>$text (above)</td></tr>\n"
		}
	    }
	    append HTML_RESULT "</table></blockquote>"
	    return 1
	}
    } else {
	append HTML_RESULT "This server does not appear to support the Explain standard."
	return 0
    } 
}

proc search_explain_attributedetails {} {
    global URI_HOST URI_PORT HTML_RESULT QUERY nums names ERESULTSETNAME

    throw_html_type
    append HTML_RESULT "<html><head><title>Attribute Details</title></head><body bgcolor = white><center><h3>Attribute Details</h3></center><p>\n"

    set QUERY "zfind explainCategory attributeDetails resultsetid EAttrDetSet"
    set results [try_esearch]

    if {[lindex $results 0] != 0} {
	set ERESULTSETNAME "EAttrDetSet"

	set hits [lindex [lindex [lindex $results 0 ] 2] 1]

	#Need to do commoninfo trick again
	set item [try_edisplay 1]
	set do_commoninfo 0

	if { [regexp {\{CommonInfo:: ([^\}]+)\}} $item] } {
	    set cinfo [lindex [lindex $item 1] 1]
	    process_explain_commoninfo $cinfo
	}
	append HTML_RESULT "<hr>"
	
	for {set idx 1} {$idx <= $hits} {incr idx} {
	    if {[lindex $item 0] != 0} {
		set item [lindex $item 1]
		
		foreach entry [lrange $item 2 end] {
		    process_explain_attributedetails $entry
		}
		append HTML_RESULT "<hr>"
	    }
	    if {$idx != $hits} {
		set ERESULTSETNAME "EAttrDetSet"
		set item [try_edisplay 1]
	    }
	}
	append HTML_RESULT "<p><hr></body></html>\n"
    }
}

proc get_explain_elementsetnames {} {
    global URI_HOST URI_PORT DATABASENAME QUERY
    set esn [list]

    if {[try_econnect] == 0 } {
	set QUERY "zfind explainCategory DatabaseInfo AND databaseName $DATABASENAME"
	set results [try_esearch]
	if {[lindex $results 0] != 0 } {
	    set disp [try_edisplay 1]
	    if {[lindex $disp 0] != 0} {
		regexp -nocase {ElementSetNames:: ([^\}]+)} $disp null esn
		return $esn
	    }
	}
    }
    return $esn
}

proc get_explain_titleString {} {
    global URI_HOST URI_PORT DATABASENAME QUERY ERESULTSETNAME databaseTitles

    set serverstr "$URI_HOST:$URI_PORT/$DATABASENAME"
    set names [array names databaseTitles]
    if {[lsearch $names $serverstr] != -1} {
	return $databaseTitles($serverstr)
    } else {

	if { [try_econnect] == 0 } {
	    set ERESULTSETNAME "eTitleStrSet"
	    set QUERY "zfind explainCategory DatabaseInfo AND databaseName $DATABASENAME resultsetid eTitleStrSet"
	    set results [try_esearch]
	    if {[lindex $results 0] != 0 } {
		set disp [try_edisplay 1]
		if {[lindex $disp 0] != 0} {
		    set record [lindex $disp 1]
		    foreach bit [lrange $record 1 end] {
			if {[lindex $bit 0] == "titleString::" } {
			    set title [get_preferred_text [lrange $bit 1 end]]
			    set databaseTitles($serverstr) $title
			    return $title
			}
		    }
		}
	    }
	}
	return ""
    }
}


proc get_explain_indexes {} {
    global attr_values attr_descs DATABASENAME QUERY
    set attr_values {}
    set attr_descs {}
    
    if { [try_econnect] == 0} {
	set QUERY "zfind explainCategory AttributeDetails AND databasename $DATABASENAME"
	set results [try_esearch]
	if {[lindex $results 0] != 0} {
	    set disp [try_edisplay 1]
	    if {[lindex $disp 0 ] != 0} {
		set disp [lindex $disp 1]
		foreach bit $disp {
		    check_explain_attribute_item $bit
		}
	    }
	}
    }

    if {$attr_values != {} } {
	return $attr_values
    } else {
	return 0
    }
}

proc explain_scan {} {
    global HTML_RESULT bib1names bib1nums  URI_HOST URI_PORT DATABASENAME attr_descs
    set attr_values [get_explain_indexes]

    if {$attr_values == 0} {
	return 0
    } else {
	throw_file "html/scan_explain.html" [list]
	set url "z3950://$URI_HOST:$URI_PORT/scan/$DATABASENAME"
	#Build form
	set formtxt "<form action=\"$url\">\n<table>\n"
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	append formtxt "<tr><td><select name=\"scanfield\">"

	for {set idx 0} {$idx < [llength $attr_values]} {incr idx} {
	    set number [lindex $attr_values $idx]
	    set name [get_preferred_text [list [lindex $attr_descs $idx]]]
	    regsub -all {[_-]} $name " " name
	    
	    if { $name == "" } {
		set nidx [lsearch $bib1nums $number]
		if {$nidx != -1} {
		    set name [lindex $bib1names $idx]
		} else {
		    set name "Unknown Index"
		}
	    }
	    append formtxt "<option value = \"$number\">$name</option>"
	}

	append formtxt "</select></td><td><input type = text size = 40 value = \"\" name = \"scanterm\"> </td></tr>"
	append formtxt "<tr><td colspan = 2><hr></td></tr>\n"
	append formtxt "</table><input type =submit value=Scan></form></body></html>"
	append HTML_RESULT $formtxt
	
	throw_result
	return 1
    }
}


proc search_explain_indexes {} {
    global URI_HOST URI_PORT DATABASENAME attr_values attr_descs HTML_RESULT QUERY bib1nums bib1names

    # Need to catch all this and check for non exp1 servers
    # Then check our pseudo-database
    # Then fall back to strobe

    set attr_values [get_explain_indexes]
    if {$attr_values == 0} {
	return 0
    } else {

	try_zconnect
	throw_file "html/index_explain_template.html" [list]
	set url "z3950://$URI_HOST:$URI_PORT/search/$DATABASENAME"
	
	#Build form
	set formtxt "<form action=\"$url\">\n<table>\n"
	append formtxt "<tr><td colspan = 4><hr></td></tr>\n"
	
	set booloptions "<option value =\"AND\">And</option> \
<option value = \"OR\">Or</option> \
<option value = \"NOT\">Not</option>"
	
	set operoptions "<option value = \"=\"> for </option> \
    <option value = \"@\"> for *</option> \
    <option value = \"LT\"> less than</option> \
    <option value = \"GT\"> greater than</option> \
<option value = \"WITHIN\">is within</option>"
	
	for {set idx 0} {$idx < [llength $attr_values]} {incr idx} {
	    set number [lindex $attr_values $idx]
	    set name [get_preferred_text [list [lindex $attr_descs $idx]]]
	    regsub -all {[_-]} $name " " name
	    
	    if { $name == "" } {
		set nidx [lsearch $bib1nums $number]
		if {$nidx != -1} {
		    set name [lindex $bib1names $idx]
		} else {
		    set name "Unknown Index"
		}
	    }
	    
	    set formatlist [get_explain_elementsetnames]
	    
	    set formats "<select name =\"format\"><option value=\"LONG2\">Full</option><option value=\"SUMMARY\">Summary</option>"
	    if {$formatlist != [list]} {
		foreach f $formatlist {
		    append formats "<option value=\"$f\">$f</option>"
		}
	    }	    
	    append formats "</select>"
	    
	    append formtxt "<tr><td><select name=\"bool$number\">$booloptions</select></td><td><b>$name</b>:</td><td><select name =\"oper$number\">$operoptions</select></td><td><input type = \"text\" size = \"40\" name = \"$number\" value = \"\"></td></tr>\n"
	}
	
	append formtxt "<tr><td colspan = 4><hr></td></tr>\n"
	
	append formtxt "<tr><td colspan = 4><table width =\"100%\">"
	append formtxt "<tr><td>Combine Fields With:</td><td><select name = \"bool\"><option value = \"AND\">And<option value = \"OR\">Or</select></td></tr>\n";
	append formtxt "<tr><td>Number of Records Requested:</td><td><input type = \"text\" name = \"numreq\" size = 10 value = 10></td></tr>"
	append formtxt "<tr><td>First Record:</td><td><input type = \"text\" name = \"firstrec\" size = 10 value = 1></td></tr>"
	append formtxt "<tr><td>Requested Format:</td><td>$formats</td></tr>"
	append formtxt "<tr><td>Name of Result Set:</td><td><input type = \"text\" name = \"resultsetname\" size = 20 value = \"\"></td></tr>"
	append formtxt "</table></td></tr>"
	
	append formtxt "\n</table>\n<input type =\"submit\" value = \"Search\"> &nbsp; <input type = \"reset\" value = \"Reset\">\n</form>\n"
	
	append HTML_RESULT "$formtxt\n"
	append HTML_RESULT "<hr></body></html>\n"
	
	return 1
    }
}

proc check_explain_attribute_item {item} {
    global attr_values attr_descs

    if {[lindex $item 0] == "attributesBySet::"} {
	foreach bit [lindex $item 1] {
	    check_explain_attribute_item $bit
	}
    }

    if {[lindex $item 0] == "attributesByType::"} {
	foreach bit [lrange $item 1 end] {
	    check_explain_attribute_item $bit
	}
    }

    if { [lindex [lindex $item 0] 0] == "attributeType::" && [lindex [lindex $item 0] 1] == "1"} {
	set values [lrange [lindex $item 2] 1 end]
	foreach val $values {
	    lappend attr_values [lindex [lindex $val 0] 1]
	    lappend attr_descs [lindex [lindex $val 1] 1]
	}
    }
}


proc search_explain_targetinfo {} {
    global HTML_RESULT QUERY DATABASENANE
    # Isn't working on Valinor :(
    
    throw_html_type
    append HTML_RESULT "<html><body bgcolor = white>"

    set QUERY "zfind explainCategory targetInfo"
    set sresults [try_esearch]

    if {[lindex $sresults 0] != 0} {

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
	
	append HTML_RESULT "<table>"
	foreach item [lrange $results 1 end] {
	    process_explain_targetinfo $item $welcome_message
	}
	append HTML_RESULT "</table><p><hr></body></html>"
    }
}

proc search_explain_recordsyntaxinfo {} {
    global HTML_RESULT QUERY DATABASENAME
    throw_html_type
    append HTML_RESULT "<html><body bgcolor = white><center><h3>Record Syntaxes</h3></center><p>"

    set QUERY "zfind explainCategory recordSyntaxInfo"

    set results [try_esearch]
    
    if {[lindex $results 0] != 0} {
	set hits [lindex [lindex [lindex $results 0] 2] 1]

	append HTML_RESULT "<hr>"
	for {set idx 1} {$idx <= $hits} {incr idx} {
	    set recitem [lindex [zdisplay 1] 1]
	    foreach item [lrange $recitem 1 end] {
		process_explain_recordSyntaxInfo $item
	    }
	    append HTML_RESULT "<p><hr><p>"
	}
	append HTML_RESULT "</body></html>"
    }
}


proc search_explain_categorylist {} {
    #These two are the same
    search_explain_categoryinfo
}

proc search_explain_categoryinfo {} {
    global URI_HOST URI_PORT HTML_RESULT
    #This shouldn't be called, but we'll support it incase someone types it in as a location
    throw_html_type
    append HTML_RESULT "<html><head></head><body bgcolor = white><center><h3>Available Explain Categories</h3></center><p>"
    do_explain_categoryInfo "Supported Explain Categories"
}

proc search_explain_databaseinfo {} {
    global URI_HOST URI_PORT currdbname searchterms HTML_RESULT QUERY DATABASENAME

    set QUERY "zfind explainCategory DatabaseInfo"
    set results [try_esearch]

    if {[lindex $results 0] != 0 } {

	set hits [lindex [lindex [lindex $results 0 ] 2] 1]

	throw_html_type
	append HTML_RESULT "<html>\n<head>\n<title>Databases on z39.50 server: $URI_HOST</title>\n</head>\n<body bgcolor = white>\n<center><h3>Databases on z39.50 server: $URI_HOST</h3></center>\n<p>"
	append HTML_RESULT "<hr>"
	append HTML_RESULT "<b>Total number of Databases</b>: $hits<p>"
	
	# Want to process commoninfo once, strangely enough.
	# Get first record and check for CommonInfo::
	set disp [try_edisplay 1]

	    
	set do_commoninfo 0
	    
	if { [regexp {\{CommonInfo:: ([^\}]+)\}} $disp] } {
	    set cinfo [lindex [lindex $disp 1] 1]
	    process_explain_commoninfo $cinfo
	}
	append HTML_RESULT "<hr>"
	    for {set idx 1} {$idx <= $hits} {incr idx} {
		if {[lindex $disp 0] != 0} {
		    set db [lindex $disp 1]
		    set has_title [regexp {\{titleString:: ([^\}]+)\}} $db]
		    append HTML_RESULT "<table>"
		    foreach section [lrange $db 1 end] { 
			process_explain_databaseInfo $section $idx $has_title $do_commoninfo
		    }
		    append HTML_RESULT "</table>"
		    append HTML_RESULT "<p><a href = \"z3950://$URI_HOST:$URI_PORT/search/$currdbname\">Search this Database</a><p>"
		    # Assume that if explain is supported, we also support Scan.
		    append HTML_RESULT "<p><a href = \"z3950://$URI_HOST:$URI_PORT/scan/$currdbname\">Scan this Database</a><p>"

		    append HTML_RESULT "<hr>"
		}
		# This is at the end as we pop one off the stack to look for commoninfo
		# Should handle errors in individual zdisplays
		if {$idx != $hits} {
		    set disp [try_edisplay 1]
		}
	    }
	append HTML_RESULT "</body></html>"
	return 1
    }
}

# If URL is just explain/ or just server
#Note that we check availability of /explain/type so this won't get called from /explain/host
proc search_explain_host {} {
    global URI_HOST URI_PORT HTML_RESULT

    set econn_okay [try_econnect]
    
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

    append HTML_RESULT "<html>\n<head>\n<title>Z3950 Server: $serverinfo(hostname)</title>\n</head>\n<body bgcolor = white>\n<center><h3>Z39.50 Server: $serverinfo(hostname)</h3></center>\n"

    append HTML_RESULT "<table cellspacing = 5>"
    append HTML_RESULT "<tr><td><i>Z39.50 Server:</i></td><td> $serverinfo(ImplementationName), version $serverinfo(ImplementationVersion)</td></tr>"
    append HTML_RESULT "<tr><td><i>Supported Protocol Version:</i></td><td>"

    set maxv 0
    for {set v 1} {$v < 4} {incr v} {
	if { $serverinfo(SupportVersion$v) == 1 } {
	    set maxv $v
	}
    }
    append HTML_RESULT "$maxv</td></tr></table><p>"

    if {$econn_okay == 0} {
	set okay [do_explain_categoryInfo "Further Information"]
	if {$okay == 0} {
	    # Try to guess DB from implementation
	    set implname $serverinfo(ImplementationName)
	    
	    set dnames {"innopac" "dynix" "advance" "sirsi" "voyager"}
	    set dbname ""
	    foreach n $dnames {
		if {[regexp -nocase $n $implname]} {
		    set dbname $n
		    break
		}
	    }

	    if {$dbname == "" && [regexp -nocase "marquis" $implname] } {
		set dbname "horizon"
	    } elseif { $dbname == "" && [regexp -nocase "data research" $implname] } {
		set dbname "marion"
	    }


	    if {$dbname != ""} {
		append HTML_RESULT "<p><b><a href=\"z3950://$URI_HOST:$URI_PORT/search/$dbname/\">Search this Server</a></b><blockquote><i>This may take a while as the search fields must be found by probing each in turn. Also, it is possible that the database may not be as expected.</i></blockquote>"
	    } else {
		append HTML_RESULT "<p><b><a href=\"z3950://$URI_HOST:$URI_PORT/search/default/\">Try to Search this Server</a></b><blockquote><i>This relies on the server accepting a 'default' database, and cannot be relied on. It may take a while as the search fields must be found by probing each in turn.</i></blockquote>"
	    }

	    # Innopac doesn't announce that it supports Scan, but does minimally
	    if {$dbname == "innopac" || ($dbname != "" && [lsearch [array names supportinfo] "Scan"] != -1 && $supportinfo(Scan) == 1)} {
		append HTML_RESULT "<p><b><a href =\"z3950://$URI_HOST:$URI_PORT/scan/$dbname/\">Browse this Server</a></b><blockquote><i>This may also take a while as the fields must be found with a similar probe as Search above.</i></blockquote>"
	    }
	}
    }

    append HTML_RESULT "<p><i>Supported Requests:</i><ul>"

    foreach support [array names supportinfo] {
	if {$supportinfo($support) == 1} {
	    append HTML_RESULT "<li>$support"
	}
    }
    append HTML_RESULT "</ul>"
    



    append HTML_RESULT "<p><hr></body></html>"
}

proc search_explain_retrievalrecorddetails {} {
    global HTML_RESULT QUERY

    set HTML_RESULT "<html><body bgcolor = white>"

    set QUERY "zfind explainCategory retrievalRecordDetails"
    set results [try_esearch]

    if {[lindex $results 0] != 0} {
	set hits [lindex [lindex [lindex $results 0] 2] 1]
	for {set idx 1} {$idx <= $hits} {incr idx} {
	    set disp [try_edisplay 1]
	    if {[lindex $disp 0] != 0} {
		append HTML_RESULT "<b>Result $idx:</b>"

		set item [lrange $disp 1 end ]
		foreach i $item {
		    process_explain_retrievalrecorddetails $i
		}
		append HTML_RESULT "<p><hr><p>"

	    }
	}
    }

    append HTML_RESULT "</body></html>"
    throw_result

}


proc search_explain_type {type} {
    global URI_HOST URI_PORT DATABASENAME QUERY HTML_RESULT

    #XXX:  Query categoryTypes to make sure we support it!

    if {[try_econnect] == 0 } {

    set QUERY "zfind explainCategory categoryList"
    set results [try_esearch]

    if {[lindex $results 0] != 0} {
	set sresults [try_edisplay 1]

	if {[lindex $sresults 0] != 0 } {
	    
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
    }
    
    set type [string tolower $type]

    set err [catch "search_explain_$type" results]

    if {$err != 0} {
	puts stderr "Caught error: $results"
	set HTML_RESULT "<html><body bgcolor = white><center><b>Error!</b></center><p>That type of explain search, $type, is not supported in the client.</p><p>Error given: $results</p></body></html>"
    }
    
}
    
}

