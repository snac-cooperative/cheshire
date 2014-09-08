#!/usr/local/cheshire/bin/webcheshire


#Author:  Rob Sanderson (azaroth@liv.ac.uk)
#Copyright (c) The University of Liverpool All Rights Reserved
#Last Modified: 6/10/99
#Description:  EAD searching with ead2html and cheshire2
#Version: 2.0.0 (Fully functional, no longer relies on ead2html-cgi, caching and DSC checking)
#Based initially on Ray Larson's SFSearch.tcl script.


#Server Config
set CHESHIRE_DATABASE archives
set CHESHIRE_CONFIGFILE "/home/ray/EADCONF/CONFIG.ARCH_HUB"


cd "/opt/APhttpd/htdocs/ead/html"
set output_to_file 1

#---------- Subroutines ----------



proc navbar {n} {
    global toc_eadid
    
    if {$n != 0} {
	set previous "\[ <a href = \"$toc_eadid"
	append previous "A"
	append previous [expr $n -1]
	append previous ".html\">previous</a> \]";
	set top "\[ <a href = \"$toc_eadid"
	append top "A0.html\">top</a> \]"
    }

    set next "\[ <a href = \"$toc_eadid"
    append next "A"
    append next [expr $n +1]
    append next ".html\">next</a> \]"

    if {$n == 0} {
	return "<p><center>$next</center></p>"
    } else {
	return "<table width = \"100%\"><tr><td>$previous</td><td>$top</td><td align = right>$next</td></tr></table>"
    }

}

proc init_newfile {n} {
    global fileh toc_title
    puts $fileh "<html>\n<head>\n<title>$toc_title</title></head><body bgcolor = white>"
    puts $fileh [navbar $n]

}

proc end_oldfile {n} {
    global fileh
    
    puts $fileh "<hr>"
    puts $fileh [navbar $n]
    puts $fileh "</body>\n</html>"

}

proc check_new_file {} {
    global  fileh filelength toc_eadid max_filenum

    if {$filelength($fileh) > 30000} {
	#more than 30k in file, split it

	set filebase $toc_eadid
	append filebase "A"
	set newfilename $filebase
	append newfilename [expr $max_filenum +1]
	append newfilename ".html"

	end_oldfile $max_filenum
	close $fileh

	puts stderr "  -- starting new file $newfilename"

	incr max_filenum
	set fileh [open $newfilename w]
	set filelength($fileh) 0
	init_newfile $max_filenum
    }

}

proc print {text} {
    global fileh filelength     
    puts $fileh $text
    if {$filelength($fileh) != -1} {
	incr filelength($fileh) [string length $text]
    }
}

proc print_jstoc_file {} {
    global toc_title toc_eadid toclist

    set title $toc_title
    set toclength [llength $toclist]
    # Should be -1, but we've hacked 0 to be the title
    set tocHeading "Table of Contents"
    set toctabpad 1
    set toctabspace 1
    set toctabborder 0
    set toctabbg "lightblue"
    set graphics 1
    set itemfile "blank.gif"
    set contractfile "opened.gif"
    set expandfile "closed.gif"
    set htmlpath "http://sca.lib.liv.ac.uk/ead/html/"
    set colours {"#ff0000" "#cc3333" "#aa5555" "#997777" "#888888" "#555555" "#333333" "#000000" "#000000" "#000000"}
    set expanddir "Expand Directory"
    set tocframename "tocf.html"
    set firsthtmlname $toc_eadid 
    append firsthtmlname "A0"
    set htmlcode ".html"

print "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">"
print "<html>\n<head>\n<title>$title</title>\n<script>\n<!--"
print "MAX_LENGTH = $toclength;"
print "level = -1;\nexpanding = new Array();"

print [generate_toc]

set osb "\\\["
set csb "\\\]"
set ocb "\\\{"
set ccb "\\\}"
set bs "\\\\"
set qt "\\\""

set eoframe "\n\
function redraw() \{\n\
  writeto= this.frames\[0\];\n\
  writeto.document.open();\n\
  writeto.document.writeln('<html><head><title>$tocHeading</title>', \n\
      '<base href = \"$htmlpath\">', \n\
      '</head><body bgcolor = \"#ffffff\">');\n\
      writeto.document.writeln('<center><h3>$tocHeading</h3></center>');\n\
\n\
      writeto.document.writeln('<table cellpadding = $toctabpad cellspacing = $toctabspace width = 100% border = $toctabborder bgcolor = $toctabbg>');\n\
\n\
   for (i = 0; i <= MAX_LENGTH; i++) \{\n\
     identifier = \"toc\" + i;\n\
    levelnums = new Array(0,0,0,0,0,0,0,0,0,0,0,0);\n\
    process_item(this\[identifier\], -1, identifier, levelnums);\n\
  \}\n\
  writeto.document.write('</table>');\n\
  writeto.document.writeln('</body></html>');\n\
  writeto.document.close();\n\
\}\n\
\n\
function Toggle(id, expandall) \{\n\
  idarray = id.split(\"-\");\n\
  /*  arrayname = idarray.shift(); Not under IE4.0 */\n\
\n\
  arrayname= idarray\[0\];\n\
      if ((len = idarray.length) > 1) \{\n\
    idarray\[len\] = \"\";\n\
    /*    idarray = idarray.slice(1); Not under NS3.0  */\n\
    for (i = 0; i < len; i++) \{\n\
      idarray\[i\] = idarray\[i+1\];\n\
    \}\n\
 \} else \{\n\
    idarray = new Array();\n\
  \}\n\
\n\
  array = this\[arrayname\];\n\
  for (i in idarray) \{\n\
    if (idarray\[i\] != \"\") \{\n\
      array = array\[3\]\[idarray\[i\]\];\n\
    \}\n\
  \}\n\
  array\[2\] = (array\[2\] == 1) ? 2 : 1;\n\
\n\
  if (expandall) \{\n\
    expanding\[expanding.length\] = id;\n\
    array\[2\] = 2;\n\
\}\n\
  redraw();\n\
\}\n\
\n\
function process_item(item, level, identifier, levelnums) \{\n\
  GRAPHICS = $graphics;\n\
  SPACES = \"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\";\n\
  symbols = \"*+-\";\n\
  graphics = new Array('<img src = \"$itemfile\" border = \"0\" height = \"15\" width = \"15\">', '<img src = \"$expandfile\" border = \"0\" width = 14 height = 14>', '<img src = \"$contractfile\" border = \"0\" width = 14 height = 14>');\n\
  writeto = this.frames\[0\];\n\
  text = item\[0\];\n\
  href = item\[1\];\n\
  colours = new Array("

for {set jsn 1} {$jsn < 10} {incr jsn} {
    set js_colour [lindex $colours $jsn]
    append eoframe "'<font color = \"$js_colour\">'";
    if {$jsn != 9} {
	append eoframe ", ";
    }
}

  append eoframe ");";

append eoframe "\n\
level++;\n\
  levelnums\[level\]++;\n\
  spaces = \"\";\n\
  onespace = \"<td>&nbsp;</td>\";\n\
  for (x=0; x < level; x++) \{\n\
    spaces = spaces + onespace;\n\
  \}\n\
  cols = 25 - level;\n\
  text = text.small();\n\
  text = colours\[level\] + text + \"</font>\";\n\
\n\
  joined = \"#\" + expanding.join('**') + '**';\n\
  inexpanding = joined.indexOf(identifier.substring(0, identifier.lastIndexOf(\"-\")) + '**');\n\
\n\
  if (levelnums\[level\] >= 10 && inexpanding < 1) \{ \n\
\      
    togglesym = '$expanddir'.link('javascript:parent\[\\'Toggle\\'\](\\'' + identifier.substring(0, identifier.lastIndexOf(\"-\"))    + '\\', 1)');\n\
    writeto.document.writeln('<tr>' + spaces + '<td colspan = \"' + cols + '\"> &nbsp; &nbsp;' + togglesym + '</td></tr>', \"\\n\");\n\
    return 1;\n\
  \}\n\
\n\
  if (href) \{\n\
    href = '<a href = \"' + href + '\" target = \"dest\">' + text + \"</a>\";
   \} else \{\n\
     href = text;\n\
   \}\n\
\n\
  if (!GRAPHICS) \{\n\
    symbol = symbols.charAt(item\[2\]);\n\
  \} else \{\n\
    symbol = graphics\[item\[2\]\];\n\
  \}\n\
\n\
  togglesym = item\[2\] == 0 ? symbol  :  symbol.link(\"javascript:parent\['Toggle'\]('\" + identifier + \"')\");\n\
\n\
  writeto.document.writeln('<tr>' +spaces + '<td colspan = \"' + cols + '\">' + togglesym + href +'</td></tr>', \"\\n\");\n\
  \n\
  if (item\[2\] == 2) \{\n\
    for (link in item\[3\]) \{\n\
      identifier = identifier + \"-\" + link;\n\
end = process_item(item\[3\]\[link\], level, identifier, levelnums);\n\
      identifier = identifier.substring(0, identifier.lastIndexOf(\"-\"));\n\
      if (end) \{\n\
         break;\n\
      \}\n\
    \}\n\
  \} \n\
\n\
levelnums\[level+1\] = 0;\n\
  level--;\n\
return 0; \n\
\} \n\
\n\
//-->\n\
</script>\n\
\n\
<base href =\"http://sca.lib.liv.ac.uk/ead/html/\"> \n\
  </head>\n\
 <frameset rows= 90%,*>\n\
  <FRAMESET COLS=\"45%,*\">\n\
    <FRAME SRC=\"$tocframename\" NAME = \"toc\">\n\
    <FRAME SRC=\"$firsthtmlname$htmlcode\" NAME = \"dest\">\n\
  </FRAMESET>\n\
  <frame src = \"http://sca.lib.liv.ac.uk/ead/search.html\" NAME=\"search\">\n\
</frameset>\n\
"

print $eoframe
unset eoframe

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
    

proc processitem item {
    global element_order eadheadlist textheadlist eadtextlist texttextlist ead_subs html_subs topheadings ignorebelow html_no_ends eademph emphstart emphend ignoretags debugtags hreftags EADID FILEDESC REVISIONDESC PROFILEDESC fm_fd_duplicates fdstart fdend fdtags titledone titletags titlestart titleend LEGALSTATUS LANGMATERIAL LEVEL ctaglist clevel ccount toclist toc_title max_filenum contains_dsc;
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
	 	regsub -all "\&apos;" $string {'} string
	 	regsub -all "\&lsquo;" $string {'} string
	 	regsub -all "\&rsquo;" $string {'} string
	 	regsub -all "\&sol;" $string {-} string

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
	    #Is an attribute Do we care? Not unless we're an isad(g) template. 

	    if { [lindex $element_order [expr [llength $element_order] -1]] == "ARCHDESC" && [lsearch {"LEGALSTATUS" "LANGMATERIAL" "LEVEL"} $elemname] != -1} { 
		set  $elemname [lindex [ lindex [lindex $item 1] 1] 1] 
	    } 

	    return 
	}  
	
	lappend element_order $elemname 
	set appendtxt "" 
	set appendeval "" 

	if { [set eidx [lsearch $eadheadlist $elemname]] != -1 } { 
	    #Process major headings 
	    set htxt [lindex $textheadlist $eidx]; 
	    if { [set hidx [sub_elem_index HEAD $item ]] != -1 } { 
		set htxt [sub_elem_contents $hidx $item] 
	    } 
	    if {$htxt != ""} { 
		regsub -all "\&apos;" $htxt {'} htxt 
		if { [set headidx [lsearch $topheadings $elemname]] != -1 } { 
		    print "<center><h4>$htxt</h4></center>"
		} else { 
		    print "<h4>$htxt</h4>" 
		} 
	    }

	} elseif {$elemname == "UNITTITLE" && [lsearch $element_order "DSC"] != -1} { 

	    print "<center><b>"
	    set appendtxt "</b></center>"
	    #need to put into toclist
	    set len [string length $toclist]
	    set tcend [expr $len - 8]

	    if {[string range $toclist  $tcend end] == " item \{ "} {
		set title [lindex [ lindex [lindex $item 1] 1] 1]
		regsub -all "\\&sol;" $title "-" title
		regsub -all "\\&apos;" $title "'" title
		regsub -all "\\&" $title "" title
		regsub -all "\"" $title "'" title
		
		set toclist [string range $toclist 0 [expr $tcend -1]]
		append toclist " \"$title\" \{ "
	    }

	} elseif {$elemname == "UNITID" && [lsearch $element_order "DSC"] != -1} {
	    print "<br><b>Reference</b>: "
	    #need to put into toclist
	    set len [string length $toclist]
	    set tcend [expr $len - 8]

	    if {[string range $toclist  $tcend end] == " item \{ "} {
		set title [lindex [ lindex [lindex $item 1] 1] 1]
		regsub -all "\\&sol;" $title "-" title
		regsub -all "\\&apos;" $title "'" title
		regsub -all "\\&" $title "" title
		regsub -all "\"" $title "'" title
		
		set toclist [string range $toclist 0 [expr $tcend -1]]
		append toclist " \"$title\" \{ "
	    }

	} elseif { [set eidx [lsearch $eadtextlist $elemname]] != -1} {
	    #Process minor headings
	    print [lindex $texttextlist $eidx];
	} elseif { [set eidx [lsearch $ead_subs $elemname]] != -1} {
	    #Process tags with direct equivalence
	    set htag [lindex $html_subs $eidx];
	    if {$htag != " "} {
		print "<$htag>"
	    }
	    if {[lsearch $html_no_ends $htag] == -1} {
		set appendtxt "</$htag>"
	    }
	    
	} elseif {$elemname == "DSC"} {
	    #Need to clear the eadheader information so that it's not displayed with each DID!
	    foreach clear {EADID FILEDESC REVISIONDESC PROFILEDESC LEGALSTATUS LANGMATERIAL LEVEL } {
		set $clear ""
	    }
	    set ccount 0
	    set clevel 0
	    set contains_dsc 1
	    set toclist {}
	    set appendtxt "<hr>"
	    
	} elseif { [lsearch $ctaglist $elemname] != -1 } {
	    #Build ToC list as per ead2html
	    #Use simple incr.ing # for anchor. Need to find unittitle.
 	    #Need to build title as per top level with head/unittitle
	    #File splitting next version.

	    check_new_file

	    incr ccount;
	    incr clevel;

	    set htxt "item";
	    if { [set hidx [sub_elem_index HEAD $item ]] != -1 } {
		set htxt [ sub_elem_contents $hidx $item ]
	    }

	    print "<hr><a name = \"$ccount\">"

	    set href $max_filenum
	    append href ".html\#$ccount"

	    append toclist "\{ $href $htxt \{ "

	} elseif { $elemname == "EMPH" } {
	    #Process EMPH rendering
	    #Need to check this with emph rendered sgml ...

	    set renderstart "<b>"
	    set renderend "</b>"
	    if { [ set renderidx [ sub_elem_index RENDER $item ]] != -1 } {
		set renderattr [sub_elem_contents $renderidx $item ]
		if {[set ridx [lsearch $eademph $renderattr]] != -1 } {
		    set renderstart [lindex $emphstart $ridx ]
		    set renderend [lindex $emphend $ridx ]
		}
	    }
	    print $renderstart
	    set appendtxt $renderend

	} elseif {[lsearch $hreftags $elemname] != -1 && [set hridx [sub_elem_index HREF $item]] != -1} {
	    #Only processing HREFs for WWW
	    set href [sub_elem_contents $hridx $item]
	    print "<a href=\"$href\">"
	    set appendtxt "</a>"

	    
	} elseif {$elemname == "EADHEADER"} {
	    #need to get information out of header, and use at beginning and end of DID.

	    foreach tag {EADID FILEDESC PROFILEDESC REVISIONDESC} {
		if { [ set idx [ sub_elem_index $tag $item ]] != -1} {
		    set contents [sub_elem_contents $idx $item ]; 
		    eval "set $tag {content:: {$contents}}"
		}

	    }

	} elseif { [set fmidx [lsearch $fm_fd_duplicates $elemname]] != -1 && [lsearch $element_order "FRONTMATTER"] != -1 } {
	    
	    set fdtag [lindex $fdtags $fmidx]

	    if { $FILEDESC != {} && [set idx [contents_elem_index $fdtag $FILEDESC]] != -1 } {
		set contlist [lindex $FILEDESC 1]
		set contlist [lreplace $contlist $idx $idx]
		set FILEDESC [lreplace $FILEDESC 1 1 $contlist]
		
		print [lindex $fdstart $fmidx];
		set appendtxt [lindex $fdend $fmidx];
		if { $elemname == "TITLEPROPER" } {
		    set titledone 1;
		    set toc_title [lindex [lindex [lindex $item 1] 1] 1]
		}
	    }

	} elseif {$elemname == "DID"} {
	    #If no UNITID, use EADID. 
	    #Display: TITLEPAGE or TITLEPROPER or UNITTITLE or UNITID or EADID
	    
	    if {$titledone == 0} {
		if { $FILEDESC != {} && [set idx [ contents_elem_index TITLESTMT $FILEDESC]] != -1} {
		    set contlist [lindex $FILEDESC 1]
		    processitem [lindex $contlist $idx]

		    set contlist [lreplace $contlist $idx $idx]
		    set FILEDESC [lreplace $FILEDESC 1 1 $contlist]
		}
	    }
	} elseif { [string compare $elemname "BIBREF"] == 0  && [lsearch {"BIBLIOGRAPHY" "RELATEDMATERIAL"}  [lindex $element_order [expr [llength $element_order]- 2 ]]] != -1} {
	    #Glasgow hack :P
	    #Fix bibrefs as lists in bibliography
	    set appendtxt "<br>"

	} elseif {[set tidx [lsearch $titletags $elemname]] != -1} {
	    print [lindex $titlestart $tidx]
	    set appendtxt [lindex $titleend $tidx]

	    if {$tidx == 0} {
		set toc_title [lindex [ lindex [ lindex $item 1] 1] 1]
	    }

	} elseif {[lsearch $ignorebelow $elemname] == -1 && [lsearch $hreftags $elemname] == -1 && [lsearch $ignoretags $elemname] == -1 && [lindex [lindex [lindex $item 1] 1] 1] != "elementEmpty"} {
	    #Last resort: Debug unknown elements
	    print "<br> Unrendered Element: $elemname <br>"
	}
	
	if {[lsearch $debugtags $elemname] != -1 } {
	    # Debug elements that are misbehaving
	    regsub -all "\}" $item "\}\n" displayitem;
	    print "<pre>$displayitem</pre><p>"
	}
	

	# --- Process sub items of list  ---
	if {[lsearch $ignorebelow $elemname] == -1} {
	    processitem [lindex [lindex $item 1] 1]
	}


	# Things to be done on close of element
	if {$appendtxt != "" } {
	    print $appendtxt
	}

	if  {$elemname == "DID"} {
	    #Add in extra elements from EADHEADER
	    if {[llength $PROFILEDESC]} {
		processitem $PROFILEDESC
	    }
	    if {[llength $REVISIONDESC]} {
		print "<br><b>Revisions</b>: "
		processitem $REVISIONDESC
	    }
	    if {[llength $FILEDESC]} {
		processitem $FILEDESC
	    }
	    #Add in extra information from ARCHDESC attributes
	    if {$LANGMATERIAL != {} && [contents_elem_index LANGUSAGE $PROFILEDESC] == -1} {
		print "<br><b>Language of Material</b>: $LANGMATERIAL";
	    }
	    if {$LEGALSTATUS != {}} {
		print "<br><b>Legal Status</b>: $LEGALSTATUS";
	    }
	    if {$LEVEL != {}} {
		print "<br><b>Level of Description</b>: $LEVEL";
	    }

	} elseif { [lsearch $ctaglist $elemname] != -1 } {
	    append toclist "\} \} "
	    incr clevel -1
	}
	
	set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	
    } else {
	#Debug unknown grs1 syntax
	print "<br>Unknown:: $item<br>"
    }
}


proc process_tocline {line} {
    global toc_eadid


    if {[string index $line 0] != "\{" } {
	set line [list $line]
    }

    set myline ""
    set linelen [llength $line]
    incr linelen -1
    set lastitem [lindex $line $linelen] 

    foreach bit $line {

	set hrefidx [lindex $bit 0]

	set href $toc_eadid
	append href "A$hrefidx"
 	set title [lindex $bit 1]

	regsub -all "\n" $title "" title
	
	set jsline "new Array(\"$title\", \"$href\""
	if {[lindex $bit 2] != {} } {
	    append jsline ", 1, new Array("
	    append jsline [process_tocline [lindex $bit 2]]
	    append jsline "))"
	} else {
	    append jsline ", 0)"
 	}
	append myline $jsline
	if {$bit != $lastitem} {
	    append myline ", "
	}
    }
    return $myline
}

proc generate_toc {} {
    global toclist toc_title firstfilename

    set tocstr "\nthis.toc0 = new Array(\"$toc_title\", \"$firstfilename\", 0)"
    set N 1

    regsub -all "\{ " $toclist "\{" toclist


    set len [llength $toclist]

    if {$len} {
	foreach tocline $toclist {
	    append tocstr "\nthis.toc$N = "
	    append tocstr [process_tocline $tocline]
	    incr N
	}
    }
    unset toclist
    return $tocstr

}

#---------- End of Subroutines ----------

	set ead_subs { ADDRESS ADDRESSLINE BLOCKQUOTE CHRONLIST CHRONITEM CHANGE DATE DEFITEM DIV HEAD01 HEAD02 ITEM INDEX INDEXENTRY LABEL LB LIST P TITLE}
	set html_subs {address br blockquote ul li li " " li hr b b li ul li b br ul p i}
	set html_no_ends {br hr}
	
	set eadheadlist {ACCESSRESTRICT ACQINFO ACCRUALS ADD ADMININFO ALTFORMAVAIL APPRAISAL ARRANGEMENT BIBLIOGRAPHY BIOGHIST CUSTODHIST FILEPLAN INDEX NOTE ODD ORGANIZATION OTHERFINDAID PREFERCITE PROCESSINFO RELATEDMATERIAL SCOPECONTENT SEPARATEDMATERIAL USERESTRICT}
	set textheadlist { "Restrictions on Access" "Acquisition Information" "Anticipated Additions" "Further Information" "Administrative Information" "Alternative Form Available" "Appraisal" "Arrangement" "Bibliography" "Biography or History" "Custodial History" "File Plan" "Index" "Notes" "Other Descriptive Data" "Organization" "Other Finding Aids" "Preferred Citation" "Processing Information" "Related Material" "Scope and Content" "Separated Material" "Restrictions on Use"}
	
	set eadtextlist {AUTHOR CONTAINER CREATION GENREFORM LANGUSAGE ORIGINATION PHYSDESC PHYSLOC REPOSITORY SERIESSTMT SPONSOR UNITDATE UNITID UNITLOC UNITTITLE}
	set texttextlist { "<br><b>Author</b>: " "<br><b>Container</b>: " "<br><b>Creation Information</b>: " "<br><b>Genre or Form</b>:" "<br><b>Language</b>: " "<br><b>Origination</b>: " "<br><b>Physical Description</b>: " "<br><b>Physical Location</b>:" "<br><b>Held at</b>: " "<br><b>Archive Series</b>:" "<br><b>Sponsor</b>: " "<br><b>Date</b>: " "<br><b>Reference</b>:  " "<br><b>Location</b>: "  "<br><b>Title</b>:"}
	
	set eademph { BOLD ITALIC BOLDITALIC BOLDUNDERLINE BOLDSMCAPS BOLDQUOTED NONPROPORT ALTRENDER QUOTED SMCAPS SUB SUPER UNDERLINE }
	set emphstart {<B> <I> <B><I> <B><U> <B> &QUOT;<I>B> <PRE> <B> &QUOT;<I> " " <SUB> <SUP> <U> }
	set emphend {"</B>" "</I>" </I></B> "</U></B>" "</B>" "</b></i>&QUOT;" "</PRE>" "</B>" "</i>&quot;" " " "</SUB>" "</SUP>" "</U>" }
	
	set topheadings {ADMININFO BIOGHIST SCOPECONTENT ORGANIZATION ODD ADD NOTE } 
	set ignorebelow {CONTROLACCESS HEAD EADHEADER}
	set ignoretags {TITLEPAGE FRONTMATTER DID ARCHDESC EXTENT PERSNAME GEOGNAME CORPNAME PLACENAME EVENT LANGUAGE LISTHEAD NOTESTMT TITLESTMT PHYSFACET DROW DENTRY TSPEC FAMNAME ABBR DIMENSIONS}
	set hreftags {ARCHREF BIBREF DAO DAOLOC EXTPTR EXTPTRLOC EXTREF EXTREFLOC PTR PTRLOC REF REFLOC }
	set element_order {EAD}
	set debugtags { }
	
	set fm_fd_duplicates {TITLEPROPER EDITION PUBLISHER}
	set fdtags {TITLESTMT EDITIONSTMT PUBLICATIONSTMT}
	set fdstart {"<center><h3>" "<br><b>Edition</b>:" "<br><b>Publisher</b>:"}
	set fdend {"</h3></center>" "" ""}
	
	set titletags {TITLEPROPER SUBTITLE}
	set titlestart {"<center><h3>" "<center><b>"}
	set titleend {"</h3></center>" "</b></center>"}
	set ctaglist {C01 C02 C03 C04 C05 C06 C07 C08 C09 C10 C11 C12 C};


# grab the cont from arguments

set cont [lindex $argv 0]


#first grab the summary tpgrs data for contains_dsc and eadid.
set CHESHIRE_ELEMENTSET TPGRS
set CHESHIRE_RECSYNTAX GRS1
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 1

set query "search docid $cont"

set err [catch {eval $query} qresults]
set sresults [lrange $qresults 1 end]
set hits [lindex [lindex [lindex $qresults 0] 0] 1]

if {!$hits} {
    puts stderr "Finished."
    exit 1
}

set qresults [lindex $qresults 1]
set toc_eadid [lindex [lindex [lindex $qresults 1] 1] 1]
set llen [llength $qresults]

puts stderr "Starting to process $toc_eadid id: $cont"

if {$llen == 4} {
    
    #retrieve full record
    
    set CHESHIRE_ELEMENTSET full
    set err [catch {eval $query} qresults]
    set sresults [lrange $qresults 1 end]
    set sresults [lindex [ lindex $sresults 0] 1]
    
    
    set titledone 0
    
    set toclist {}
    set ccount 0
    set clevel 0
    set contains_dsc 1
    set toc_title "title"
    foreach tag {EADID FILEDESC PROFILEDESC REVISIONDESC LEGALSTATUS LANGMATERIAL LEVEL} {
	eval "set $tag {}"
    }
    
    #Strip headers to ead tag, and play with results a bit
    regsub -all "{tagType:: 3}" $sresults "" sresults
    regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
    regsub -all "\"" $sresults {\&quot;} sresults
    
    #Build HTML file(s)
    set filename $toc_eadid
    append filename "A0.html"
    set fileh [open $filename w]
    set filelength($fileh) 0
    set max_filenum 0
    set firstfilename $filename
    
    print "<html><body bgcolor = white>"
    foreach item [lrange $sresults 1 end] {
	processitem $item
    }
    print "<p><hr><p></body></html>"
    close $fileh
    
    unset sresults
    unset item
    
    
    #Write ToC file
    set filename $toc_eadid
    append filename ".html"
    set fileh [open $filename w]
    set filelength($fileh) -1
    
    print_jstoc_file
    close $fileh
    
    unset filelength
    unset filename
    
}
return 1

