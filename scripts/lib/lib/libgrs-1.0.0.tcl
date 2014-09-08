

set html_no_ends {BR HR}

# This is here as for non transformed files we shouldn't really need to throw to a file as well
proc print {text} {
    global OUTPUT_TO_FILE fileh OUTPUT_TO_STDOUT

    if {$OUTPUT_TO_FILE} {
	puts $fileh $text
    }
    
    if {$OUTPUT_TO_STDOUT } {
	puts stdout "$text"
    }
}

proc recurse_flatten {list} {
    global flattext flatelemname include_flat_tags ignore_flat_tags

    set grstype [lindex $list 0]
    if { $grstype == "content::"} {
	if {[llength [lindex [lindex $list 1] 0]] == 1} {
	    set string [lindex $list 1]
	    if {$string != "elementEmpty" } {
		append flattext $string
	    }
	} else {
	    foreach nitem [lindex $list 1] { 
		recurse_flatten $nitem 
	    } 
	}	    
    } elseif { $grstype == "SUBELEMENT::"} {
	set data [lindex $list 1]
	if {[llength $data] != 3} {
	    set flatelemname [lindex [ lindex [lindex [lindex  $list 1] 0] 0] 1]
	    set appendtxt "";

	    if { [ lsearch $include_flat_tags $flatelemname ] != -1 } {
		append flattext "<$flatelemname>"
		set appendtxt "</$flatelemname>"
	    } elseif { [ lsearch $ignore_flat_tags $flatelemname] != -1} {
		return
	    }

	    set cont [lindex [lindex $list 1] 1]
	    recurse_flatten $cont

	    append flattext $appendtxt

	} else {
	    return
	}
    }
}


proc flattext {list elemname} {
    global flattext flatelemname
    set flattext ""
    set flatelemname $elemname
    set grstype [lindex $list 0]
    if {$grstype != "content::" && $grstype != "SUBELEMENT::"} {
	return $list
    } else {
	recurse_flatten $list
	return $flattext
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




# XXX:  Turn this into the meta script with callouts to other functions
    

proc processitem item {
    global element_order heading_tags heading_texts main_heading_tags text_tags text_texts alt_title_tag alt_title_attr start_small_heading end_small_heading start_large_heading end_large_heading substitute_tags substitute_html surround_tags surround_starts surround_ends internal_attr internal_val ignore_below ignore_total ignore_tags record_tags html_no_ends href_tags href_attrs  before_process_tags before_processed_tags after_process_tags after_processed_tags debug_tags OUTPUT_TO_FILE indata;
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
		set string [ textsub $string ]
		if { [regexp -nocase {(^| )(http://[^ ,!;?]+)(\. |[ ,!;?])} $string null null href ] } {
		    regsub -all $href $string "<a href = \"$href\">$href</a>" string 
		}
		print "$string"
	    }
	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::" } { 
	
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1] 
	if {[llength [lindex $item 1]] == 3} { 
	    #Is an attribute.   Try calling a handler or just fail silently
	    if {[catch "processitem_attr_$elemname $item" appendtxt] } {
		return
	    }
	}  else {
	    
	    #Ignore tag name 19 - it's a cheshire holder for plain text
	    if { $elemname != "19" } {
		lappend element_order $elemname 
	    }
	    
	    set appendtxt "" 
	    set appendeval "" 
	    
	    #Strip internal tags
	    if {[set aidx [sub_elem_index $internal_attr $item ]] != -1} {
		set atxt [string toupper [sub_elem_contents $aidx $item]]
		if {$atxt == $internal_val} {
		    return;
		}
	    }
	    
	    #ignore tag and subtags
	    if {[lsearch $ignore_total $elemname] != -1  } {
		return 
	    }
	    
	    #Process tags before
	    if { [ set pidx [lsearch $before_process_tags $elemname]] != -1 } {
		foreach subitem $before_processed_tags {
		    catch "return $$subitem" pitem
		    if { [llength $pitem] } {
			processitem $pitem
		    }
		}
	    }
	    
	    if { [set eidx [lsearch $heading_tags $elemname]] != -1 } { 
		#Process major headings 
		set htxt [lindex $heading_texts $eidx]; 
		if { [set hidx [sub_elem_index $alt_title_attr $item ]] != -1 } { 
		    set htxt [sub_elem_contents $hidx $item] 
		} 
		if { [set hidx [sub_elem_index $alt_title_tag $item ]] != -1 } { 
		    set htxt [sub_elem_contents $hidx $item] 
		} 
		if {$htxt != ""} { 
		    set htxt [ textsub $htxt ]
		    #Don't print heading large if we're nested.
		    if { [lindex $element_order [expr [llength $element_order] -2]] == $elemname} {
			print "$start_small_heading $htxt $end_small_heading"
		    } elseif { [set headidx [lsearch $main_heading_tags $elemname]] != -1 } { 
			print "$start_large_heading $htxt $end_large_heading"
		    } else { 
			print "$start_small_heading $htxt $end_small_heading" 
		    } 
		}
		
	    } elseif { [set eidx [lsearch $text_tags $elemname]] != -1} {
		#Process minor headings
		set htxt "";
		if { [set hidx [sub_elem_index $alt_title_attr $item ]] != -1 } { 
		    set htxt [sub_elem_contents $hidx $item] 
		} 
		if { [set hidx [sub_elem_index $alt_title_tag $item ]] != -1 } { 
		    set htxt [sub_elem_contents $hidx $item] 
		} 
		if {$htxt == ""} { 
		    print [lindex $text_texts $eidx];
		} else {
		    print "<br><b>$htxt:</b> "
		}
		
	    } elseif { [set eidx [lsearch $substitute_tags $elemname]] != -1} {
		#Process tags with direct equivalence
		set htag [lindex $substitute_html $eidx];
		if {$htag != " "} {
		    print "<$htag>"
		}
		if {[lsearch $html_no_ends $htag] == -1} {
		    set appendtxt "</$htag>"
		}
		
	    } elseif { [set rtidx [lsearch $record_tags $elemname]] != -1 } { 
		#Record the info for later processing
		set $elemname $item
		return
		
	    } elseif { [set suridx [lsearch $surround_tags $elemname]] != -1 } {
		print [lindex $surround_starts $suridx]
		set appendtxt [lindex $surround_ends  $suridx]
		
	    } elseif {[set htidx [lsearch $href_tags $elemname]] != -1 && [set hridx [sub_elem_index [lindex $href_attrs $htidx]  $item]] != -1} {
		#Only processing HREFs for WWW
		set href [sub_elem_contents $hridx $item]
		set href [ textsub $href ]
		print "<a href=\"$href\" target=\"new\">"
		set appendtxt "</a>"
		
	    } elseif { $elemname != "19" && [lsearch $ignore_below $elemname] == -1 && [lsearch $href_tags $elemname] == -1 && [lsearch $ignore_tags $elemname] == -1 && [lindex [lindex [lindex $item 1] 1] 1] != "elementEmpty"} {
		
		# Try calling a process_tagname with the item.
		if {[catch "processitem_$elemname $item" appendtxt]} {
		    #Last resort: Debug unknown elements to STDERR
		    puts stderr "\[$SCRIPT_NAME\]: Unrendered SGML Element: $elemname"
		}
	    }
	    
	}
	
	if {[lsearch $debug_tags $elemname] != -1 } {
	    # Debug elements that are misbehaving
	    regsub -all "\}" $item "\}\n" displayitem;
	    print "<pre>$displayitem</pre><p>"
	}
	
	
	# --- Process sub items of list  ---
	if {[lsearch $ignore_below $elemname] == -1} {
	    processitem [lindex [lindex $item 1] 1]
	}
	
	
	# Things to be done on close of element
	if {$appendtxt != "" } {
	    print $appendtxt
	}

	if { [ set pidx [lsearch $after_process_tags $elemname]] != -1 } {
	    foreach subitem $after_processed_tags {
		catch "return $$subitem" pitem
		if { [llength $pitem] } {
		    processitem $pitem
		}
	    }
	}
    
    } elseif { $item == "" } {
	#We've been stripped out, ignore
    } else {
	#Debug unknown grs1 syntax
	print stderr "\[$SCRIPT_NAME\]: UnknownGRS:: '$item'"
    }
}

