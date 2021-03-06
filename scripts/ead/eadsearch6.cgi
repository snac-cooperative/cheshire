#!/home/cheshire/cheshire/bin/webcheshire
#Author:  Rob Sanderson (azaroth@liv.ac.uk)
#Copyright:  &copy; (c) The University of Liverpool, All Rights Reserved
#Last Modified: 12/03/01
#Description:  EAD searching with ead2html-tcl and cheshire2
#Version: 6.0.0

#Version History:
#     * 1.0: Fully functional, no longer relies on ead2html-cgi,
#     * 2.0: /Lots/ of added bonuses
#     * 3.0: Easier configuration, intelligent flattexting, fixed ToC generation
#     * 4.0: Splitting records by numreq and startec, user formatting, table, [D]ROW
#             handling 
#     * 5.0  scan format handling, abstracted fieldcont/fieldidx, 
#             frameless display, handles new EMPH tricks  
#     * 5.5   Latest eadform.html generation from records. 
#     * 6.0 Supports Component retrieval, relevance display

set CHESHIRE_ROOT "/home/cheshire/cheshire/ead_components"
set CGI_ROOT "/home/cheshire/public_html/cgi-bin"
set HTML_ROOT "/home/cheshire/public_html/eadcomp"
set EAD_ROOT "/home/cheshire/cheshire/eadhub/ead-dir"
set BASE_URL "http://gondolin.hist.liv.ac.uk/~cheshire/eadhub/"
set CGI_URL "http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/"

# Global Server Configuration
set CHESHIRE_DATABASE ead_components
set CHESHIRE_CONFIGFILE "$CHESHIRE_ROOT/DBCONFIGFILE"

set LOGGING 0
set LOGFILE "$EAD_ROOT/eadsearch.log"
set CACHING 0

# CGI Script Initialisation
set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED  SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR  REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH  HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}

set scriptname $env(SCRIPT_NAME);

puts "Content-type: text/HTML\n"
cd "$EAD_ROOT"

set output_to_file 1
set output_to_stdout 0
set last_index_time [file mtime "$CHESHIRE_ROOT/indexes/title.index"]


#--------------------------------------------------------------
# ead2html-tcl init
# Do NOT reorder these lists, as they are indexed from one to another.
# Needed here for both summary and full

set ead_subs { ADDRESS ADDRESSLINE BLOCKQUOTE CHRONLIST CHRONITEM CHANGE DEFITEM DIV HEAD01 HEAD02 ITEM INDEX INDEXENTRY LABEL LB LIST P TITLE  THEAD TBODY}
set html_subs {address br blockquote ul li li  li hr b b li ul li b br ul p i  THEAD TBODY}
set html_sub_entities {"" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" ""  "" ""}
set html_no_ends {br hr}

set eadheadlist {ACCESSRESTRICT ACQINFO ACCRUALS ADD ADMININFO ALTFORMAVAIL APPRAISAL ARRANGEMENT BIBLIOGRAPHY BIOGHIST CUSTODHIST FILEPLAN INDEX NOTE ODD ORGANIZATION OTHERFINDAID PREFERCITE PROCESSINFO RELATEDMATERIAL SCOPECONTENT SEPARATEDMATERIAL USERESTRICT CONTROLACCESS}
set textheadlist {"Access Conditions" "Immediate Source of Acquisition" "Accruals" "Further Information" "Administrative Information" "Existence of Copies" "Appraisal, Destruction, Scheduling" "System of Arrangement" "Publication Note" "Administrative/Biographical History" "Custodial History" "File Plan" "Index" "Note" "" "Organization" "Finding Aids" "Preferred Citation" "Dates of Accumulation" "Related Units of Description" "Scope and Content" "Associated Material" "Copyright/Reproduction" "Access Points"}

set eadtextlist {AUTHOR CONTAINER CREATION EDITION LANGUSAGE ORIGINATION  PHYSDESC PHYSLOC PUBLISHER REPOSITORY SERIESSTMT SPONSOR UNITDATE UNITID UNITLOC UNITTITLE LANGMATERIAL ABSTRACT GENREFORM OCCUPATION SUBJECT}
set texttextlist { "<br><b>Author</b>: " "<br><b>Container</b>: " "<br><b>Creation Information</b>: " "<br><b>Edition</b>: " "<br><b>Language</b>: " "<br><b>Name of Creator</b>: "   "<br><b>Extent</b>: " "<br><b>Physical Location</b>:" "<br><b>Published by</b>: " "<br><b>Held at</b>: " "<br><b>Archive Series</b>:" "<br><b>Sponsor</b>: " "<br><b>Dates of creation</b>: " "<br><b>Reference</b>:  " "<br><b>Location</b>: "  "<br><b>Title</b>:" "<br><b>Language</b>: " "<br><b>Abstract</b>:  " "" "" ""}

set catags {PERSNAME FAMNAME CORPNAME GEOGNAME SUBJECT GENREFORM FUNCTION OCCUPATION}
set catexts {"<br>" "<br>" "<br>" "<br>" "<br>" "<br>" "<br>"}

set eademph { BOLD ITALIC BOLDITALIC BOLDUNDERLINE BOLDSMCAPS BOLDQUOTED NONPROPORT ALTRENDER QUOTED SMCAPS SUB SUPER UNDERLINE }
set emphstart {<B> <I> <B><I> <B><U> <B> &QUOT;<I>B> <PRE> <B> &QUOT;<I> " " <SUB> <SUP> <U> }
set emphend {"</B>" "</I>" </I></B> "</U></B>" "</B>" "</b></i>&QUOT;" "</PRE>" "</B>" "</i>&quot;" " " "</SUB>" "</SUP>" "</U>" }

set fdtags {TITLESTMT EDITIONSTMT PUBLICATIONSTMT}
set fdstart {"<center><h3>" "<br><b>Edition</b>:" "<br><b>Publisher</b>:"}
set fdend {"</h3></center>" "" ""}

set titletags {TITLEPROPER SUBTITLE}
set titlestart {"<center><h3>" "<center><b>"}
set titleend {"</h3></center>" "</b></center>"}

set unittitlestart "<center><h3>"
set unittitleend "</h3></center>"

set topheadings {ADMININFO BIOGHIST SCOPECONTENT ORGANIZATION ODD ADD NOTE CONTROLACCESS} 
set ignoretotal {}
set ignorebelow {HEAD DROW}
set ignoretags {TITLEPAGE FRONTMATTER DID ARCHDESC EXTENT PERSNAME GEOGNAME CORPNAME PLACENAME EVENT LANGUAGE LISTHEAD NOTESTMT TITLESTMT PHYSFACET DROW DENTRY TSPEC FAMNAME ABBR DIMENSIONS PUBLICATIONSTMT EDITIONSTMT SUBJECT OCCUPATION 19 DocumentContent DATE TGROUP COLSPEC EAD}
set hreftags {ARCHREF BIBREF DAO DAOLOC EXTPTR EXTPTRLOC EXTREF EXTREFLOC PTR PTRLOC REF REFLOC NUM}
set element_order {EAD}
set debugtags { }

set fm_fd_duplicates {TITLEPROPER EDITION PUBLISHER}

set namesubfields { "surname" "forename" "dates" "epithet" "title" "other" } 
set namesubstarts { "" "" "(" "" "" "" } 
set persnamesubends {"," "." ")" "" "" ""}
set famnamesubends  { "" "" ")" "" "" "."}
set subjectsubfields {"a" "x" "a-" "z" "zx" "z-" "y" "o"}
set subjectsubstarts {"" "" "" "" "" "" "(" "\["}
set subjectsubends {"--" "--" "--" "|" "--" "|" ")" "\]"}

set bnterms {"" "" "Corporate Entity</i>: " "Location</i>: " "Subject</i>: " "Genre or Form</i>: "}
set start {}
set end {}
set fields {}

set indenthr ""
set titledone 0

set ctaglist {C01 C02 C03 C04 C05 C06 C07 C08 C09 C10 C11 C12 C};
set toclist {}
set ccount 0
set clevel 0
set contains_dsc 0
set toc_title "title"
set curr_toc_title ""

foreach tag {EADID FILEDESC PROFILEDESC REVISIONDESC LEGALSTATUS LANGMATERIAL LEVEL} {
    eval "set $tag {}"
}

set tabledimensions {}
set tableattrs ""
set tablecellattrs ""
set tablecellidx -1

set record_tags {eadid titleproper creation unitid unittitle unitdate repository extent physfacet origination bioghist scopecontent arrangement custodhist acqinfo appraisal accruals accessrestrict userestrict altformavail note otherfindaid relatedmaterial bibliography controlaccess odd}

set other_rec_tags {threesevenone threefourthree langmaterial revisions}

foreach rtag $record_tags {
    set $rtag {}
}
foreach rtag $other_rec_tags {
    set $rtag {}
}
set include_flat_tags {}
set ignore_flat_tags {}
set empty_tags {LB PTR SPANSPEC PTRLOC EXTPTRLOC EXTPTR COLSPEC }


# End of variable initialisation	
#-----------------------------------------------------




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

	incr max_filenum
	set fileh [open $newfilename w]
	set filelength($fileh) 0
	init_newfile $max_filenum
    }

}

proc print {text} {
    global output_to_file fileh output_to_stdout filelength     

    if {$output_to_file} {
	puts $fileh $text
	if {$filelength($fileh) != -1} {
	    incr filelength($fileh) [string length $text]
	}
	#Can write same data to multiple places.
	if {$output_to_stdout } {
	    puts stdout "$text"
	}
    } else {
	puts stdout "$text"
    }
}

proc returntocfile {filename} {
    set f [open $filename r]
    while {[gets $f line] >= 0} {
	puts stdout $line
    }
    close $f
}

proc print_jstoc_file {} {
    global toc_title toc_eadid toclist BASE_URL

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
    set htmlpath "$BASE_URL/html/"
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
<base href =\"$BASE_URL/html/\"> \n\
  </head>\n\
 <frameset rows= *,100>\n\
  <FRAMESET COLS=\"45%,*\">\n\
    <FRAME SRC=\"$tocframename\" NAME = \"toc\">\n\
    <FRAME SRC=\"$firsthtmlname$htmlcode\" NAME = \"dest\">\n\
  </FRAMESET>\n\
  <frame src = \"$BASE_URL/search.html\" NAME=\"search\">\n\
</frameset>\n\
"

print $eoframe

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
    

proc processitem item {
    global element_order eadheadlist textheadlist eadtextlist texttextlist ead_subs html_subs topheadings ignorebelow html_no_ends eademph emphstart emphend ignoretags debugtags hreftags EADID FILEDESC REVISIONDESC PROFILEDESC fm_fd_duplicates fdstart fdend fdtags titledone titletags titlestart titleend LEGALSTATUS LANGMATERIAL LEVEL ctaglist clevel ccount toclist toc_title max_filenum contains_dsc catags catexts curr_toc_title output_to_file ignoretotal indata unittitlestart unittitleend indenthr start end fields subjectsubstarts subjectsubends namesubstarts persnamesubends famnamesubends namesubfields subjectsubfields catype bnterms cafldidx maxcafld CGI_URL scriptname html_sub_entities tabledimensions tableattrs tablecellidx tablecellattrs;
    
    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    if { $string != "elementEmpty" } {
		set string [ textsub $string ]
		
		if { [regexp -nocase {(^| )(http://[^ ,!;?]+)(\. |[ ,!;?])} $string null null href ] } {
		    regsub -all $href $string "<a href = \"$href\">$href</a>" string 
		}

		#Hack in links to ARCHON

		if { [ lsearch $element_order "UNITID"] != -1 } {
		    if { [ regexp -nocase {^GB ([0-9]+).+} $string null idnum ] } {
			regsub "^0*" $idnum "" idnum 
			set nstring "<a target = \"new\" href = \"http://www.hmc.gov.uk/nra/locresult.asp?LR="
			append nstring $idnum
			append nstring "\">$string</a>"
			set string $nstring
		    }
		}
		print "$string"
	    }

	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::"} { 
	 
	set elemname [lindex [ lindex [lindex [lindex  $item 1] 0] 0] 1] 
	if {[llength [lindex $item 1]] == 3} { 
	    #Is an attribute Do we care? Not unless we're an isad(g) template. 

	    if { [lindex $element_order [expr [llength $element_order] -1]] == "ARCHDESC" && [lsearch { "LANGMATERIAL" "LEVEL"} $elemname] != -1} { 
		set  $elemname [lindex [ lindex [lindex $item 1] 1] 1] 
	    }
	    return 
	}  


	#Ignore tag name 19 - it's a cheshire holder for plain
	if { $elemname != "19" } {
	    lappend element_order $elemname 
	}
	set appendtxt "" 
	set appendeval "" 

	#Strip audience = internal tags
	if {[set aidx [sub_elem_index AUDIENCE $item ]] != -1} {
	    set atxt [string toupper [sub_elem_contents $aidx $item]]
	    if {$atxt == "INTERNAL"} {
		return;
	    }
	}

#	print "<I>$elemname</I><br>"

	if {[lsearch $ignoretotal $elemname] != -1  } {
	    return 
	}

	if { [set eidx [lsearch $eadheadlist $elemname]] != -1 } { 

	    set list [lindex [lindex [lindex $item 1] 1] 1]
	    set printheading 0
	    foreach bit $list {
		if { [ llength $bit] != 3  } {
		    set printheading 1
		}
	    }
	    
	    if {$printheading} {
		#Process major headings 
		set htxt [lindex $textheadlist $eidx]; 
		if { [set hidx [sub_elem_index HEAD $item ]] != -1 } { 
		    set htxt [sub_elem_contents $hidx $item] 
		} 
		if {$htxt != ""} { 

		    set htxt [ textsub $htxt ]

		    if { ($elemname == "NOTE" && ([ lsearch $topheadings [lindex $element_order [expr [llength $element_order] -2]]] != -1 || [lindex $element_order [expr [llength $element_order] -2]] == "DID")) || [lindex $element_order [expr [llength $element_order] -2]] == $elemname} {
			print "<h4>$htxt</h4>"
		    } elseif { [set headidx [lsearch $topheadings $elemname]] != -1 } { 
			print "<center><h3><font color= green>$htxt</font></h3></center>"
		    } else { 
			print "<h4>$htxt</h4>" 
		    } 
		}
	    }
	    
	} elseif {$elemname == "EADHEADER"} {
	    #need to get information out of header, and use at beginning and end of DID.

	    foreach tag {EADID FILEDESC PROFILEDESC REVISIONDESC} {
		if { [ set idx [ sub_elem_index $tag $item ]] != -1} {
		    set contents [sub_elem_contents $idx $item ]; 
		    eval "set $tag {content:: {$contents}}"
		}

	    }

	} elseif {$elemname == "UNITTITLE" && [lsearch $element_order "DSC"] != -1} { 

	    print $unittitlestart
	    set appendtxt $unittitleend

	    set len [string length $toclist]
	    set tcend [expr $len - [expr [string length $curr_toc_title] + 5]]

	    if {[string range $toclist  $tcend end] == "\"$curr_toc_title\" \{ "} {
		set title [flattext $item $elemname]
		set title [ textsub $title ]

		regsub -all "\"" $title "'" title
		regsub -all "\\&" $title "" title

		set toclist [string range $toclist 0 [expr $tcend -1]]
		append toclist " \"$title\" \{ "
	    }

	} elseif {$elemname == "UNITID" && [lsearch $element_order "DSC"] != -1} {
	    print "<br><b>Reference</b>: "

	    set len [string length $toclist]
	    set tcend [expr $len - 10]

	    if {[string range $toclist  $tcend end] == " \"item\" \{ "} {
		set title [flattext $item $elemname]
		
		set title [ textsub $title ]
		regsub -all "\\&" $title "" title
		regsub -all {"} $title "'" title
#" Damned emacs
		set toclist [string range $toclist 0 [expr $tcend -1]]
		append toclist " \"$title\" \{ "
		set curr_toc_title $title;
	     }

	  } elseif { [set eidx [lsearch $eadtextlist $elemname]] != -1} {
	      #Process minor headings
	      #Check for LABEL attributes ala HEAD tags

	      set htxt "";
	      if { [set hidx [sub_elem_index LABEL $item ]] != -1 } { 
		  set htxt [sub_elem_contents $hidx $item] 
	      } 

	      if {$htxt == ""} { 

		  # Check that we're not a CA tag outside of CA
		  if {[lsearch $catags $elemname] != -1 && [lsearch $element_order "CONTROLACCESS"] == -1} {
		      # Do nothing
		  } else {
		      print [lindex $texttextlist $eidx];
		  }
	      } else {
		  set htxt [string trim $htxt]
		  if { [string index $htxt [expr [string length $htxt] -1]] == ":" } {
		      print "<br><b>$htxt</b>"
		  } else {
		      print "<br><b>$htxt</b>: "
		  }
	      }

	} elseif { [set eidx [lsearch $ead_subs $elemname]] != -1} {
	    #Process tags with direct equivalence

	    set htag [lindex $html_subs $eidx];
	    set hattrs [lindex $html_sub_entities $eidx];
	    if {$htag != " "} {
		    print "<$htag $hattrs>"
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

	            if {$output_to_file} { 
		check_new_file
	    }
		
	    incr ccount;
	    incr clevel;

	    set htxt "item";

	    # Is this ever true??
	    if { [set hidx [sub_elem_index HEAD $item ]] != -1 } {
		set htxt [ sub_elem_contents $hidx $item ]
	    }

	    if {$indenthr != ""} {
		print $indenthr
	    } else {
		print "<hr>"
	    }
	    print "<a name = \"$ccount\">"

	    if { $output_to_file} {
	    set href $max_filenum
	    append href ".html\#$ccount"
	    
	    set curr_toc_title $htxt;
	    append toclist "\{ $href \"$htxt\" \{ "
	    }

	} elseif { [set caidx [lsearch $catags $elemname]] != -1 && [lsearch $element_order "CONTROLACCESS"] != -1} {

	    # Print heading
	    print [lindex $catexts $caidx];
	    set cafldidx 0

	    if { [sub_elem_index EMPH $item] } {
		# Standardise formatting for delimited CA fields.
		# Set up the fields here, and fall through for any additional processing in the actual tags
		set catype $elemname
		set conts [lindex [lindex [ lindex $item 1] 1] 1]
		
		# Need to step over to strip out attributes
		set maxcafld 0
		foreach emph $conts {
		    if { [llength [lindex $emph 1]] != 3 } {
			incr maxcafld
		    }
		}
		
		if { $elemname == "PERSNAME" } {
		    set start $namesubstarts;
		    set end $persnamesubends;
		    set fields $namesubfields
		} elseif { $elemname == "FAMNAME" } {
		    set start $namesubstarts;
		    set end $famnamesubstarts;
		    set fields $namesubfields
		} else {
		    set start $subjectsubstarts
		    set end $subjectsubends
		    set fields $subjectsubfields
		}

	    }	    
	} elseif { $elemname == "EMPH" } {

	    if { [ lsearch $element_order CONTROLACCESS ] != -1  && [set aridx [ sub_elem_index ALTRENDER $item]] != -1} {
		incr cafldidx
		set fld  [sub_elem_contents $aridx $item];
		set fld [string tolower $fld]

		if { [set fldidx [ lsearch $fields $fld]] != -1 } {
		    print [lindex $start $fldidx]

    		    # Create scan search from text
		    set text [flattext  $item $elemname]
		    set cgitext [cgisub $text]
		    
		    #Need to generate index from catype and fld.
		    set scanindex "x"

		    if { $fld == "z-" } {
			append scanindex "geogname-a-"
		    } elseif {$fld == "z" } {
			append scanindex "geogname-a" 
		    } else {
			append scanindex [string tolower $catype]
			append scanindex "-$fld"
		    }

		    set href "$scriptname?format=sgmlscan&bool=AND&maxrecs=20&firstrec=1&fieldidx1=$scanindex&fieldcont1=$cgitext"
		    print "<a target = \"_top\" href =\"$href\">"
		    set appendtxt "</a>"

		    if { $cafldidx != $maxcafld || $fld == "dates" ||  $fld == "other" ||  $fld == "y" || $fld == "o"} {
			append appendtxt [lindex $end $fldidx]
		    }
		}


	    } else {
	    set renderstart "<b>"
	    set renderend "</b>"
	    if { [ set renderidx [ sub_elem_index RENDER $item ]] != -1 } {
		set renderattr [sub_elem_contents $renderidx $item ]
		set renderattr [string toupper $renderattr]
		if {[set ridx [lsearch $eademph $renderattr]] != -1 } {
		    set renderstart [lindex $emphstart $ridx ]
		    set renderend [lindex $emphend $ridx ]
		}
	    }
	    print $renderstart
	    set appendtxt $renderend
	}

	} elseif {[lsearch $hreftags $elemname] != -1 && [set hridx [sub_elem_index HREF $item]] != -1} {
	    #Map some to http://foo, others to an eadid search

	    set href [sub_elem_contents $hridx $item]
	    set href [ textsub $href ]

	    if {$elemname != "ARCHREF" } {
		print "<a href=\"$href\" target=\"new\">"
	    } else {
		# Process show for new/replace 
		target = " target = \"new\""
		if { [set aidx [sub_elem_index "SHOW" $item]] != -1} {		
		    set showtxt [sub_elem_contents $aidx $item]
		    if {[string tolower $showtxt] != "new" } {
			target = ""
		    }
		}

		print "<a href= \"$scriptname?format=full&bool=AND&fieldidx1=eadid&fieldcont1=$href&firstrec=1&maxrecs=100\" $target>"
	    }
	    set appendtxt "</a>"
	    
	} elseif { [set fmidx [lsearch $fm_fd_duplicates $elemname]] != -1 && [lsearch $element_order "FRONTMATTER"] != -1 } {
	    
	    set fdtag [lindex $fdtags $fmidx]

	    if { $FILEDESC != {} && [set idx [contents_elem_index $fdtag $FILEDESC]] != -1 } {
		set contlist [lindex $FILEDESC 1]
		set contlist [lreplace $contlist $idx $idx]
		set FILEDESC [lreplace $FILEDESC 1 1 $contlist]
	    }		
	    print [lindex $fdstart $fmidx];
	    set appendtxt [lindex $fdend $fmidx];

	    if { $elemname == "TITLEPROPER" } {
		set titledone 1;
		set toc_title [flattext $item $elemname]
	    }

	} elseif { $elemname == "TABLE" } {
	    #For more than one table per document we need to clear previous.
	    set tabledimensions {}
	    set ta $tableattrs

	    if { [set aidx [sub_elem_index "FRAME" $item]] != -1} {
		set frametxt [sub_elem_contents $aidx $item]
		if {$frametxt =="ALL"} {
		    append ta " border = 1"
		}
	    }
	    if { [set aidx [sub_elem_index "VALIGN" $item]] != -1} {
		append ta " valign = \"[sub_elem_contents $aidx $item]\""
	    }
	    print "<table $ta>"
	    set appendtxt "</table>"

	} elseif { $elemname == "ROW" && [lsearch $element_order "TABLE"] != -1} {
	    set tablecellidx -1
	    print "<tr>"
	    set appendtxt "</tr>"

	} elseif { $elemname == "ENTRY" && [lsearch $element_order "TABLE"] != -1} {
	    incr tablecellidx
	    set tca ""
	    if {[llength $tabledimensions] > $tablecellidx} {
		set tca [lindex $tabledimensions $tablecellidx]
	    }
	    print "<td $tca>"
	    set appendtxt "</td>"
	    if {[lsearch $element_order "THEAD"] != -1} {
		print "<b>"
		set appendtxt "</b></td>"
	    }

	} elseif {$elemname == "COLSPEC" && [lsearch $element_order "TABLE"] != -1} {
	    # handle align, valign and colwidth for the moment... others are a bit tricky
	    set txt $tablecellattrs
	    
	    if { [set aidx [sub_elem_index "ALIGN" $item]] != -1} {
		append txt " align = \"[sub_elem_contents $aidx $item]\""
	    }
	    if { [set aidx [sub_elem_index "COLWIDTH" $item]] != -1} {
		append txt " width = \"[sub_elem_contents $aidx $item]\""
	    }
	    if { [set aidx [sub_elem_index "VALIGN" $item]] != -1} {
		append txt " valign = \"[sub_elem_contents $aidx $item]\""
	    }

	    lappend tabledimensions $txt

	} elseif {  $elemname == "DROW" || ($elemname == "ROW"  && [lsearch $element_order "TABLE"] == -1 )} {
@finger	    # We want to process Drow etc cleverly. 
	    # Process entire drow here and reset some handling.

	    #ROW is used in TABLE. Need to check for THEAD only


	    print "<table width = 100%>\n<tr>"
	    #We need to set up a max columns by looking ahead. This is not fun.
	    # If the first is not span c1-N then make one. or there will be .... trouble

	    set max_row_col 20
	    set contentlist [lindex [lindex [lindex $item 1] 1] 1]
	    set num_entries [llength $contentlist]
	    set donefirst 0;
	    set unittitlestart "<br><b>"
	    set unittitleend "</b>"
	    
	    
	    foreach subitem $contentlist {
		#Only [d]entry is valid here. And must contain a spanname
		set spanidx [sub_elem_index SPANNAME $subitem]
		if { $spanidx == -1 } {
		    #No spanname! Ohoh. Just leave width blank.
		    set width [expr (1 / $num_entries) * 100]
		    append width "%"
		} else {
		    set spancont [sub_elem_contents 0 $subitem]
		    regexp {([0-9]*)(-([0-9]+))} $spancont full first null second
		    set width [expr (($second - $first) + 1) * 5 ] 
		    append width "%"
		    
		    if {!$donefirst} {
			set donefirst 1
			if { $first  != 1 } {
			    # put in a blank  entry up to the current one
			    # These look silly with the HRs. Fix?
			    set fwidth [expr ($first -1) * 5]
			    append fwidth "%"
			    set indenthr "<table width = 100%><tr><td width = $fwidth>&nbsp;</td><td><hr></td></tr></table>"
			    print "<td width = $fwidth>&nbsp;</td>"
			} else {
			    set indenthr ""
			}
		    }
		    
		    print "<td width = $width>"
		    #process item the contents of the entry
		    set sappend ""
		    if {$elemname == "ROW"} {
			# Put in title style
			print "<font color = green><b>"
			set sappend "</b></font>"
		    }

		    set subcontents [lindex [ lindex [lindex $subitem 1] 1] 1]
		    foreach contitem $subcontents {
			#Check if attr. If not, processitem it.
			if { [llength $contitem] != 3} {
			    processitem $contitem
			}
		    }

		    print $sappend
		    print "</td>"
		}
	    }

	    print "</tr>\n</table>"
	    set unittitlestart "<center><h3>"
	    set unittitleend "</h3></center>"


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
	    #Glasgow/Warwick hack :P
	    #Fix bibrefs as lists in bibliography
	    set appendtxt "<br>"

	} elseif {[set tidx [lsearch $titletags $elemname]] != -1} {
	    print [lindex $titlestart $tidx]
	    set appendtxt [lindex $titleend $tidx]

	    if {$tidx == 0} {
		set toc_title [flattext $item $elemname]
	    }

	} elseif {[lsearch $ignorebelow $elemname] == -1 && [lsearch $hreftags $elemname] == -1 && [lsearch $ignoretags $elemname] == -1 && [lindex [lindex [lindex $item 1] 1] 1] != "elementEmpty"} {
	    #Last resort: Debug unknown elements to STDERR
	    puts stderr "\[eadsearch2\]: Unrendered EAD Element: $elemname"
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
	    if { $indata("format") == "full" } {

		if {$LEVEL != {}} {
		    print "<br><b>Level of Description</b>: $LEVEL";
		}

		print "<hr width = 40%>"
		
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
		print "<hr width = 40%>";
	    }
	    
	} elseif { [lsearch $ctaglist $elemname] != -1 } {
	    append toclist "\} \} "
	    incr clevel -1
	} elseif { $elemname == "FRONTMATTER" } {
	    print "<hr width = 40%>"
	} elseif {$elemname != "19"} { 
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	}

	} else {
	    #Debug unknown grs1 syntax to stderr
	    puts stderr "Unknown GRS:: $item<br>"
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
	regsub -all "\r" $title "" title;
	set title [ textsub $title ]
	

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

    regsub -all "\n" $toc_title "" toc_title;
    regsub -all "\r" $toc_title "" toc_title;
    set toc_title [ textsub $toc_title ]
    
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
    return $tocstr
}

proc check_cache {query} {
    #Search cache for title response
    #Cache line will look like:  query -=- <filename>

    #store in external files as it's easier to load from file to variable
    #And makes for less fuckups than with one megalithic cache file

    regsub -all "\n" $query "n" myquery
    regsub -all "\t" $myquery "t" myquery

    set q 0

    set cacheh [open "cache/titles.cache" r]
    while {[set line [gets $cacheh]] >= 0} {
	if {[string toupper "$myquery -=- $q"] == [string toupper  $line]} {
	    close $cacheh
	    set cachefile $q

	    set cachedresults ""
	    set cfileh [open "cache/$cachefile" r]
	    while {[set cline [gets $cfileh]] >= 0} {
		append cachedresults $cline
	    }
	    close $cfileh
	    regsub -all "\n" $cachedresults "" cachedresults
	    return $cachedresults
	}
	incr q
    }
    close $cacheh
    return 0
}

proc update_cache {} {
    global query qresults

    set hits [lindex [ lindex [ lindex $qresults 0] 0] 1]
    set returned [lindex [ lindex [lindex $qresults 0 ] 1] 1]
    if { $hits == $returned } {
	regsub -all "\n" $query "n" myquery
	regsub -all "\t" $myquery "t" myquery
	regsub -all "\n" $qresults "" myqresults
	set qstr "$myquery -=- "
	
	set cachelist {}
	set cacheh [open "cache/titles.cache" r]
	while {[set line [gets $cacheh]] >= 0} {
	    lappend cachelist $line
	}
	close $cacheh
	
	set rand [expr [clock seconds] % 100]
	
	if {[llength $cachelist] < $rand} {
	    append qstr [llength $cachelist]
	    set rand [llength $cachelist]
	    lappend cachelist $qstr
	} else {
	    append qstr $rand
	    set cachelist [lreplace $cachelist $rand $rand $qstr]
	}
	
	set cfileh [open "cache/$rand" w]
	puts $cfileh $myqresults
	close $cfileh
	set cacheh [open "cache/titles.cache" w]
	foreach l $cachelist {
	    puts $cacheh $l
	}
	close $cacheh
    }
}

proc generate_cgihref numslashes {
    global inlist indata cgiquery env;
    set cgiquery $env(SCRIPT_NAME);
    set firstterm 1;
    set slashes ""

    for {set n 1} {$n <= $numslashes} {incr n} {
	append slashes "\\"
    }
    foreach key $inlist {
	if {!$firstterm} {
	    append cgiquery "$slashes&"
	} else {
	    append cgiquery "?"
	    set firstterm 0
	}
	set val $indata("$key");
	set val [cgisub $val]
	append cgiquery "$key=$val"
    } 
} 

proc cgisub {id} {
    # Need to reverse process in decode_cgi

	regsub -all { } $id {+} id
	regsub -all "\n\t" $id {\%0A} id
	regsub -all {,} $id {\%2C} id
	regsub -all {'} $id {\%27} id
	regsub -all "\"" $id {\%22} id
	regsub -all {@} $id {\%40} id
	regsub -all {#} $id {\%23} id
	regsub -all {\}} $id {\%7D} id
	regsub -all {\{} $id  {\%7B} id
	regsub -all { } $id  {\%20} id
	return $id;
}

proc textsub {content} {
    regsub -all "\&apos;" $content {'} content
    regsub -all "\&lsqb;" $content {[} content
    regsub -all "\&rsqb;" $content {]} content
    regsub -all "\&commat;" $content {,} content
    regsub -all "\&lsquo;" $content {'} content
    regsub -all "\&rsquo;" $content {'} content
    regsub -all "\&sol;" $content {/} content
    regsub -all "\&rdquo;" $content {"} content
    regsub -all "\&mdash;" $content {-} content
    regsub -all "\&ndash;" $content {-} content
    regsub -all "\&ldquo;" $content {"} content

    return $content;
}


proc generate_query {i} {
    global firstterm indata query ;
    set idi $indata("fieldidx$i");
    set idc $indata("fieldcont$i");
    set bool $indata("bool");

    #Replace trailing *s with #s for truncated searches
    regsub -all {\*} $idc {#} idc;
    regsub -all ";" $idc "" idc;

    if {$idi != "none" && $idc != ""} {
	if {$firstterm == 0} {
	    append query " $bool "
	}
	if {$idi == "topic" || $idi == "any"} {
	    append query " ($idi @ \{$idc\} )"
	} elseif { $idi == "date" } {
	    append query " ($idi <=> $idc )"
	} else {
	    append query " ($idi \{$idc\} )"
	}
	set firstterm 0
    }
}

proc generate_compquery {i} {
    global firstterm indata compquery ;
    set idi $indata("fieldidx$i");
    set idc $indata("fieldcont$i");
    set bool $indata("bool");

    set idi "comp1$idi"

    #Replace trailing *s with #s for truncated searches
    regsub -all {\*} $idc {#} idc;
    regsub -all ";" $idc "" idc;

    if {$idi != "comp1none" && $idc != ""} {
	if {$firstterm == 0} {
	    append compquery " $bool "
	}
	if {$idi == "comp1topic" || $idi == "comp1any"} {
	    append compquery " ($idi @ \{$idc\} )"
	} elseif { $idi == "comp1date" } {
	    append compquery " ($idi <=> $idc )"
	} else {
	    append compquery " ($idi \{$idc\} )"
	}
	set firstterm 0
    }
}


proc returnfile {fn substs} {
    set fnH [open $fn]
    while {[gets $fnH line] >= 0} {
	foreach sub $substs {
	    if [regexp [lindex $sub 0] $line] {
		regsub -all [lindex $sub 0] $line [join [lrange $sub 1 [llength $sub]]] line
	    }
	}
	puts $line
    }
    close $fnH;
}

proc addtolog {type arga argb} {
    global env LOGGING EAD_ROOT
    if { $LOGGING } {
	set fileh [open "/$EAD_ROOT/eadsearch.log" a]
	set line [clock format [clock seconds ]]
	set envval "REMOTE_ADDR"
	set ip "$env($envval)"
	append line ":$type:$arga:$argb:$ip"
	puts $fileh $line
	puts stderr "\[eadsearch/addtolog\]:  Logging $type"
	close $fileh
    }
}

# --- eadform procedures ---

proc processitem_edit {item} {

    global element_order record_tags
    foreach rtag $record_tags {
	global $rtag
    }
    global threesevenone langmaterial datenormal threefourthree revisions


    if {[lindex $item 0] == "content::"} {
	if {[llength [lindex [lindex $item 1] 0]] == 1} {
	    set string [lindex $item 1]
	    # Don't need to worry about them here
	} else { 
	    foreach nitem [lindex $item 1] { 
		processitem_edit $nitem 
	    } 
	} 
    } elseif { [lindex $item 0] == "SUBELEMENT::" || [lindex $item 0] == "GenericRecord::"} { 

	set elemname [lindex [ lindex [ lindex [ lindex  $item 1] 0] 0] 1] 
	set elemname [string tolower $elemname]

	if {[llength [lindex $item 1]] == 3} { 
	    # $elemname == attrname $data == value
	    # We need to store: archdesc/langmaterial (unitdate/normal is eaten by unitdate)
	    set data [ lindex [ lindex [ lindex $item 1] 1] 1]
	    set data [string trim $data]
	    
	    if {$elemname == "langmaterial" && [lsearch $element_order "archdesc"] != -1} {
		set langmaterial $data
	    }
	    return
	}  
	
        if { $elemname != "19" } {
            lappend element_order $elemname 
        }

	set data [lindex [lindex $item 1] 1]
	set done 0
	if {$elemname == "list" && [lsearch $element_order "revisiondesc"] != -1} {
	    set revisions $data
	    set done 1
	} elseif {$elemname == "note" && [lsearch $element_order "notestmt"] != -1} {
	    set threesevenone $data
	    set done 1
	} elseif {$elemname == "emph" && [lsearch $element_order "physdesc"] != -1} {
	    set threefourthree $data
	    set done 1
	} elseif {[lsearch $record_tags $elemname] != -1} {
	    set $elemname $data
	    set done 1
	}
	
	if { $done == 0 } {
	    processitem_edit  [lindex [lindex $item 1] 1]
	}
	
	if {$elemname != "19"} {
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	}	
	
    } else {
	puts stderr "GRS1 Problem Formatting form."
    }
}

proc striptag {text tagname} {

    set start  "<$tagname>"
    set end  "</$tagname>"
    
    if { [string range $text 0 [expr [string length $start] -1]] == $start } {
	set text [string range $text [string length $start] end]
    }
    if { [string range $text  [set slen [expr [string length $text] - [string length $start] -1]]  end] == $end } {
	set text [string range $text 0 [expr $slen -1]]
    }
    return $text

}

proc is_attr attritem {
    set data [lindex $attritem 1]
    if { [llength $data] == 3 } {
        return 1;
    } else {
        return 0;
    }
}



proc process_rewrite { item } {
    # Bleh. Modified from my rewrite2.tcl What a mess.
    global castring empty_tags optname last_attr type

    
    if {[lindex $item 0] == "content::"} {
        if {[llength [lindex [lindex $item 1] 0]] == 1} {
            set string [lindex $item 1]
            if { $string != "elementEmpty" } {
		if {$last_attr == "a" || $last_attr == "surname"} {
		    set optname "$string"
		    set last_attr "null"
		}

                append castring "$string"
            }
	    
        } else { 
            foreach nitem [lindex $item 1] { 
                process_rewrite  $nitem 
            } 
        } 
	
    } elseif { [lindex $item 0] == "SUBELEMENT::" } { 
	
        set elemname [lindex [ lindex [ lindex [ lindex  $item 1] 0] 0] 1] 
        if {[llength [lindex $item 1]] == 3} { 
            set data [ lindex [ lindex [ lindex $item 1] 1] 1]
            set data [string trim $data]
            append castring " $elemname = \"$data\""
	    if {$elemname == "ALTRENDER"} {
		set last_attr $data
	    }

            return 
        }  
        
        #Ignore tag name 19 - it's a cheshire holder for plain
        if { $elemname != "19" } {
            append castring "&lt;$elemname"
        }
	
        #Check for and remove attrs off front
        set contents [lindex [lindex [lindex $item 1] 1] 1]
	
        while { $contents != {} && [is_attr [set attritem [lindex $contents 0]]] } {
            process_rewrite $attritem
            if { [llength $contents ] > 1 } {
                set contents [lrange $contents 1 end]
            } else {
                set contents {}
            }
        }
	
        #Close tag unless content wrapper
        if { $elemname != "19" } {
            append castring "&gt;"
        }

        # Reconstruct good GRS syntax from smashed data
        set contents [ list "content::" $contents]
	
        # --- Process sub items of list  ---
        process_rewrite $contents
	
        # Things to be done on close of element
        if {$elemname != "19"} { 
            if {[lsearch $empty_tags $elemname] == -1} {
                append castring "&lt;/$elemname&gt;"
            }
        }
    }
}

proc writeeadform {}  {
    global record_tags other_rec_tags include_flat_tags ignore_flat_tags castring optname last_attr
    foreach rtag $record_tags {
	global $rtag
    }
    foreach rtag $other_rec_tags {
	global $rtag
    }

    set include_flat_tags {TITLE LB P ITEM}
    set ignore_flat_tags {HEAD}

    returnfile edithead.html {}

    puts "<p>\n<b>3.1.1:  Reference Code</b> (including NCA Repository Code)\n<br><input type = \"text\" name = \"caa\" size = \"80\" value = \"[flattext $eadid eadid]\">"
    puts "<p>\n<b>3.1.2:  Title</b>\n<br><input type = \"text\" name = \"cab\" size = \"80\" value = \"[flattext $titleproper tp]\">"

    #get normal attr from unitdate
    set datenormal ""
    set udcont [lindex $unitdate 1]
    foreach  udc $udcont {
 	if {[string tolower [lindex [lindex [lindex [lindex $udc 1] 0] 0] 1]] == "normal"} {
	    set datenormal [lindex [lindex [lindex $udc 1] 1] 1]
	    break
	}
    }

    puts "<p>\n<table><tr><td>"
    puts "<b>3.1.3: Dates of Creation</b><br>\n<input type = \"text\" name= \"cac\" size = \"38\" value = \"[flattext $unitdate ud]\"></td><td>"
    puts "<b>Normalised Date</b><br>\n<input type = \"text\" name= \"can\" size = \"38\" value = \"$datenormal\"></td></tr></table>"
    puts "<p><b>3.1.5: Extent of Unit of Description</b>\n<br><input type = \"text\" name = \"cae\" size = \"80\" value = \"[flattext $extent extent]\">"    

    puts "<p>\n<hr width = \"50%\">"
    puts "<p><center><h3>3.2: Context Area</h3></center>"

    puts "<p><b>3.2.1: Name of Creator</b> (Also add as Access Point manually)<br><input type = \"text\" name = \"cba\" size = \"80\" value = \"[flattext $origination origination]\">"
    puts "<p><b>3.2.2: Administrative/Biographical History</b><br><textarea name = \"cbb\" rows = \"5\" cols= \"80\">[striptag [flattext $bioghist bioghist] P]</textarea>"
    puts "<p><b>3.2.3: Archival History</b><br><textarea name = \"cbc\" rows = \"5\" cols= \"80\">[striptag [flattext $custodhist custodhist] P]</textarea>"
    puts "<p><b>3.2.4: Immediate Source of Acquisition</b><br><textarea name = \"cbd\" rows = \"5\" cols= \"80\">[striptag [flattext $acqinfo acqinfo] P]</textarea>"

    puts "<p><hr width = \"50%\"><p><center><h3>3.3: Content and Structure Area</h3></center>"
    
    puts "<p><b>3.3.1: Scope and Content</b><br><textarea name = \"cca\" rows = \"5\" cols= \"80\">[striptag [flattext $scopecontent sc] P]</textarea>"
    puts "<p><b>3.3.2: Appraisal</b><br><textarea name = \"ccb\" rows = \"5\" cols= \"80\">[striptag [flattext $appraisal sc] P]</textarea>"
    puts "<p><b>3.3.3: Accruals</b><br><textarea name = \"ccc\" rows = \"5\" cols= \"80\">[striptag [flattext $accruals sc] P]</textarea>"
    puts "<p><b>3.3.4: System of Arrangement</b><br><textarea name = \"ccd\" rows = \"5\" cols= \"80\">[striptag [flattext $arrangement sc] P]</textarea>"

    puts "<p><hr width = \"50%\"><p><center><h3>3.4: Conditions of Access and Use Area</h3></center>"

    puts "<p><b>3.4.1: Conditions Governing Access</b><br><input type = \"text\" name = \"cda\" size = \"80\" value = \"[striptag [flattext $accessrestrict custodhist] P]\">"
    puts "<p><b>3.4.2: Conditions Governing Reproduction</b><br><textarea name = \"cdb\" rows = \"5\" cols= \"80\">[striptag [flattext $userestrict sc] P]</textarea>"
    puts "<p><b>3.4.3: Language of Material</b><br><table>"
    puts "<tr><td><b>Codes</b>:</td><td><input type = \"text\" name = \"cdc\" size=\"80\" value = \"$langmaterial\"></td></tr>"
    puts "<tr><td><b>Prose</b>:</td><td><input type = \"text\" name = \"cdp\" size=\"80\" value = \"[flattext $threefourthree tft]\"></td></tr>"
    puts "</table>"
    puts "<p><b>3.4.4: Physical Characteristics</b><br><textarea name = \"cdd\" rows = \"5\" cols= \"80\">[flattext $physfacet sc]</textarea>"
    puts "<p><b>3.4.5: Finding Aids</b><br><textarea name = \"cde\" rows = \"5\" cols= \"80\">[striptag [flattext $otherfindaid sc] P]</textarea>"

    puts "<p><hr width = \"50%\"><p>"
    puts "<center><h3>3.5: Aliied Materials Area</h3></center>"

    puts "<p><b>3.5.1: Existence/Location of Originals</b><br><textarea name = \"cea\" rows = \"5\" cols= \"80\">[striptag [flattext $odd odd] P]</textarea>"
    puts "<p><b>3.5.2: Existence/Location of Copies</b><br><textarea name = \"ceb\" rows = \"5\" cols= \"80\">[striptag [flattext $altformavail avf] P]</textarea>"
    puts "<p><b>3.5.3: Related Units of Description</b><br><textarea name = \"cec\" rows = \"5\" cols= \"80\">[striptag [flattext $relatedmaterial rm] P]</textarea>"
    puts "<p><b>3.5.4: Publication Note</b><br><textarea name = \"ced\" rows = \"5\" cols= \"80\">[striptag [flattext $bibliography bibliography] P]</textarea>"

    puts "<p><hr width = \"50%\"><p>"
    puts "<center><h3>3.6: Note Area</h3></center>"
    puts "<p><b>3.6.1: Note</b><br><textarea name = \"cfa\" rows = \"5\" cols= \"80\">[striptag [flattext $note note] P]</textarea>"

    puts "<p><hr width = \"50%\"><p>"
    puts "<center><h3>3.7: Description Area</h3></center>"
    puts "<p><b>3.7.1: Archivist's Note</b><br><textarea name = \"cga\" rows = \"5\" cols= \"80\">[striptag [flattext $threesevenone cr] P]</textarea>"

    # Need to store original  revisions (creation is static (currently))
    puts "<p> <i>3.7.2 Rules and Conventions</i> and <i>3.7.3 Dates of Description</i>  will be generated automatically for this record."
    
    puts "<input type = hidden name = revisions value = \"[flattext $revisions rev]\">"

    puts "<p><hr width = \"50%\"><p>"
    puts "<center><h3>Access Points</h3></center>"
    puts "<p><b>Note Well: Only the most appropriate level of the access point should be given, not the full broader/narrower term sturcture.</b>"

    returnfile capts.html {}
    
    # Write <script>s to generate new Options

    set types {"SUBJECT" "PERSNAME" "FAMNAME" "CORPNAME" "GEOGNAME"}
    set jslists {"su_list" "pn_list" "fn_list" "cn_list" "gn_list"}

    foreach ap [lindex $controlaccess 1] {
	# Get type, text for option name, full sgml

	set type [lindex [lindex [lindex [lindex $ap 1] 0] 0] 1]

	set tidx [lsearch $types $type]
	
	if { $tidx == -1} {
	    # Erk? Ignore...
	} else {
	    set whichlist [lindex $jslists $tidx]

	    set last_attr ""
	    set castring ""
	    set optname ""

	    # This tinkers with optname and castring
	    process_rewrite $ap
	    regsub -all "\n" $castring "\\n" castring
	    regsub -all "\n" $optname "" optname
	    append castring "\\n"

	    if {$optname == ""} {
		# This should never happen...
		set optname "Access Point"
	    }

	    puts stderr "name: $optname"


	    puts "<script>\
      olength = document.eadform.$whichlist.options.length;\
      new_item = new Option('$optname', '${castring}');\
      document.eadform.$whichlist.options\[olength\] = new_item;\
</script>"
	    
	}
    }

    puts "<p><b>Repository Name</b> (e.g. University of Nottingham)<br><input type = \"text\" name = \"rep\" size = \"80\" value = \"[flattext $repository rep]\">"

    returnfile endedit.html {}

}

# ----- Component procedures ----------

proc cmp_relv {a b} {

    set rela [lindex [lindex [lindex $a 1] 1] 1]
    set relb [lindex [lindex [lindex $b 1] 1] 1]
    return [expr $relb -$rela]

}

proc merge_results {resa resb} {
    #Currently merge on Relevance (first position)

    set full [concat $resa $resb]
    set full [lsort -command cmp_relv $full]
    return $full

}




#---------- End of Subroutines ----------



#------------------------------------------------------------ 
# Main Function


if {[string compare $env(REQUEST_METHOD) "POST"]==0} {
    set message [split [read stdin $env(CONTENT_LENGTH)] &]
} else {
    set message [split $env(QUERY_STRING) &]
}

set maxfield 0;
set indata("fakeentry") "null"
set inlist {}; #This should have the same contents as namelist.

foreach pair $message {

    set name [lindex [split $pair =] 0]
    set val [lindex [split $pair =] 1]
    regsub -all {\+} $val { } val
    regsub -all {\%0A} $val \n\t val
    regsub -all {\%2C} $val {,} val
    regsub -all {\%27} $val {'} val
    regsub -all {\%22} $val "\"" val
    regsub -all {\%40} $val {@} val
    regsub -all {\%7D} $val {\}} val
    regsub -all {\%7B} $val {\{} val
    regsub -all {\%20} $val { } val
    regsub -all {\%23} $val {#} val


    if {[regexp "^ +$" $val]} {
	set val "";
    }
    set indata("$name") "$val"
    lappend inlist $name

    if { [ string range $name 0 7 ] == "fieldidx" && [ set newidx [ string range $name 8 end]] > $maxfield } {
	set maxfield $newidx;
    }

}

    set numstart $indata("firstrec");
    set numstarttext $numstart
    set numreq $indata("maxrecs");
    set numreqtext $numreq

    if {[lsearch $inlist "noframes"] != -1} {
	set use_frames 0
    } else {
	set use_frames 1
    }

    if {[lsearch $inlist "components"] != -1} {
	set use_components 1
    } else {
	set use_components 0
    }
    

    # Check number sanity

    set errf [catch {expr $numstart}]
    set errm [catch {expr $numreq}]

    if { $errf != 0 || $numstart < 1} {
	returnfile err.html [list "\%err\% The start record, '$numstarttext', is not an appropriate value. It should be a number greater than 0"]
	exit
    }
    if { $errm != 0 || $numreq < 1} {
	returnfile err.html [list "\%err\% The number of records requested, '$numreqtext', is not an appropriate value. It should be a number greater than 0"]
	exit
    }

# Begin processing.

set firstterm 1

set query "search "

for {set n 1} {$n <=  $maxfield} {incr n} {
    generate_query $n;
}

set compquery "search "
set firstterm 1
# For component integration this is no longer so easy (sic)
if {($indata("format") == "sgml" || $indata("format") == "titles") && $use_components == 1} {
    for {set n 1} {$n <= $maxfield} {incr n} {
	generate_compquery $n;
    }
}
    

if {$query == "search "} {
    returnfile no_terms.html {}
    exit
}

#need to do different things for different formats.
# format == sgml   -> Do initial scan
# format == titles   -> Generate titles with links
# format == summary -> Generate summary info
# format == full -> Generate ead2html
# format == scan -> Scan index for terms, generate links
# format == sgmlscan -> Frameset for scan
# format == edit  -> Generate eadform to edit record

set redirect 1

while {$redirect} {

    set redirect 0

switch $indata("format") {

    "scan" {
	# Scan terms. clicking term will feed back to SGML as a search.
	# If just one, then show in RHS. Show number of matches with term
	
	set index $indata("fieldidx1")
	set scanterm $indata("fieldcont1")
	set scanterm [string tolower $scanterm]

	set scanq "lscan $index {$scanterm} 0 $numreq [ expr $numreq / 2 +1]"
	set err [ catch {eval $scanq } qresults ]
	set hits [lindex [ lindex [ lindex $qresults 0] 2] 1]

	if {$hits == 0 || $err != 0 } {
	    #handle error in scanning
	} else {

	    set CHESHIRE_ELEMENTSET TPGRS
	    set CHESHIRE_RECSYNTAX GRS1
	    set CHESHIRE_NUMREQUESTED 10
	    set sresults [lrange $qresults 1 end]

	    returnfile scantop.html {}

	    set indata("fieldcont1") "replace"

	    generate_cgihref 0
	    set navcgi $cgiquery

	    set indata("format") "full"
	    set indata("maxrecs") 1
	    set indata("firstrec") 1
	    set indata("fieldidx1") "docid"
	    
	    for {set f 2  } { $f <= $maxfield } { incr f } {
		set iname "fieldidx$f"
		set cname "fieldcont$f"
		set indata($iname) ""
		set indata($cname) ""
	    }

	    set nodscs 0
	    foreach val $inlist {
		if { [string first render_ $val ] == 0 } {
		    set nodscs 1
		    break
		}
	    }

	    generate_cgihref 0
	    set cgia $cgiquery
	    regsub "format=full" $cgiquery "format=sgml" cgib
	    regsub "fieldidx1=docid" $cgib "fieldidx1=$index" cgib
	    regsub "maxrecs=1" $cgib "maxrecs=25" cgib

	    set termidx 0

	    puts "<table width = 90%>"

	    set firstterm ""
	    set lastterm ""
	    set prevterm ""
	    set foundterm 0

	    foreach result $sresults {
		
		incr termidx
		set term [lrange $result 0 [expr [llength $result] - 2]]
		set recs [lindex $result [expr [llength $result] - 1]]

		if {$term == ">>>Global Data Values<<<" } {
		    #skip this 
		    set firstterm ""
		} else {
		    
		    if {$termidx == 1} {
			set firstterm $term
		    } elseif { $termidx == $numreq } {
			set lastterm $term
		    }
		    
		    set termtxt $term
		    if { [string tolower $term ] == [ string tolower $scanterm] } {
			set termtxt "<b>$termtxt</b>"
		    }
		    
		    if {$foundterm == 0 && $term > $scanterm && $prevterm < $scanterm} {
			puts "<tr><td colspan = 2><center><b>Your term '$scanterm' would be here.</b></center></td></tr>";
			set foundterm 1
		    }
		    
		    if {$recs == 1} {
			set searchquery "search $index {\"$term\"}"
			set err [ catch { eval $searchquery } srchresults ]
			set hits [lindex [lindex [ lindex $srchresults 0] 0] 1]

			set record [lindex $srchresults 1]
			
			if { $nodscs } {
			    set targframe "dest"
			} elseif {[llength $record] == 4} {
			    set targframe "_top"
			} else {
			    set targframe "dest"
			}
		
			if {$err != 0 || $hits == 0 } {
			    # Ohoh!
			    puts "<i>BROKEN INDEX: '$term' - hits: $hits</i><br>"
			} else {
			    set docid [lindex [lindex [lindex [lindex $srchresults 1] 1] 1] 1]
			    
			    # Build cgi link...
			    regsub "fieldcont1=replace" $cgia "fieldcont1=$docid" cgiaa
			    puts "<tr><td><a href = \"$cgiaa\" target = \"$targframe\" onClick='parent.frames\[2\].document.eadform.field1idx.selectedIndex = 8'>$termtxt</a></td><td align = right>(1 match)</td></tr>"
			}
			
		    } else {
			set cgiterm [cgisub $term]
			regsub "fieldcont1=replace" $cgib "fieldcont1=$cgiterm" cgibb
			puts "<tr><td><a href = \"$cgibb\" target =\"_top\">$termtxt</a></td><td align = right> ($recs matches)</td></tr>"
			set searchquery "search $index {\"$term\"}"
		    }
		    
		}
		
		set prevterm $term
	    }
	    
	    # Generate navigation forms

	    regsub "replace" $navcgi [cgisub $firstterm] firstnav
	    regsub "replace" $navcgi [cgisub $lastterm] lastnav

	    set first "<- <a href=\"$firstnav\">Previous</a>"
	    set last "<a href=\"$lastnav\">Next</a> ->"

	    set navstr ""
	    if {$firstterm != "" } {
		set navstr $first
	    }
	    if {$lastterm != ""} {
		append navstr " $last"
	    }

	    puts "</table><p>$navstr<hr><p></body></html>"

	}
	
    }

    "edit" {
	# step over structure. Record terms. substitue into form values.
	puts "<html><head><title>Cheshire EAD Editing</title></head><body bgcolor = white><p>"
	
	set CHESHIRE_NUM_START $indata("firstrec")
	set CHESHIRE_NUMREQUESTED 1
	set CHESHIRE_ELEMENTSET F
	set CHESHIRE_RECSYNTAX grs1

	set err [catch {eval $query} qresults]
	set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	set sresults [lrange $qresults 1 end]

	set sresults [lindex [ lindex $sresults 0] 1]
	regsub -all "{tagType:: 3}" $sresults "" sresults
	regsub -all "{tagType:: 2}" $sresults "" sresults
	regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
	regsub -all "\"" $sresults {\&quot;} sresults
	
	foreach item [lrange $sresults 1 end] {
	    processitem_edit $item
	}

	writeeadform
	exit

    }

    "sgmlscan" {

	set indata("format") "scan";
	generate_cgihref 3
	returnfile frameset.html [list "\%query\% $cgiquery" "\%search\% search1"]

    }

    "sgml" {

	set CHESHIRE_ELEMENTSET F
	set CHESHIRE_NUM_START 1
	set CHESHIRE_NUMREQUESTED 0
	set err [catch {eval $query} qresults]
	set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	set fullhits $hits

	set err2 0
	if {$use_components} {
	    set CHESHIRE_ELEMENTSET F
	    set CHESHIRE_NUM_START 1
	    set CHESHIRE_NUMREQUESTED 0
	    set err2 [catch {eval $query} qresults]
	    set hits2 [lindex [lindex [lindex $qresults 0] 0] 1]
	    set fullhits [expr $hits + $hits2]
	}

	addtolog "sgml" "$query" $fullhits

	if {$hits == 0 || $err != 0 || $err2 != 0} {
	    set sublist "\%query\% $query";
	    returnfile no_matches.html [list $sublist]
	    exit
	} elseif {$use_frames == 0} {
	    set redirect 1
	    set indata("format") "titles";
	} else {
	    set indata("format") "titles";
	    generate_cgihref 3

	    returnfile frameset.html [list "\%query\% $cgiquery" "\%search\% search1"]
	}
    }

    "titles" {
	#Generate html doc with links to summary in frame 'dest'
	# display $maxrecs starting at $firstrec

	set CHESHIRE_ELEMENTSET TPGRS
	set CHESHIRE_RECSYNTAX GRS1
	set CHESHIRE_NUM_START $numstart
	set CHESHIRE_NUMREQUESTED $numreq

	if {$CACHING && [set cached [check_cache $query]] != 0} {
	    set qresults $cached
	    set err 0
	    set tstr "0000000"
	} else {
	    set tstr [time {set err [catch {eval $query} qresults]} 1]
	    set tstr [lindex $tstr 0]
	    if {$err == 0 && $CACHING} {
		update_cache 
	    }
	}


	set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	set sresults [lrange $qresults 1 end]
	set returned [ lindex [ lindex [ lindex $qresults 0] 1] 1]
	set startrec [ lindex [ lindex [ lindex $qresults 0] 2] 1]
	set fullhits $hits
	set fullresults $sresults

	# Above handles plain records, need to check components and merge.

	if {$use_components == 1} {
	    set CHESHIRE_ELEMENTSET CTPGRS
	    set tstr2 [time {set err [catch {eval $compquery} qresults]} 1]
	    set tstr2 [lindex $tstr2 0]
	    
	    set tstr [expr $tstr + $tstr2]

	    set hits [lindex [lindex [lindex $qresults 0 ] 0] 1]
	    set sresults [lrange $qresults 1 end]
	    set fullhits [expr $fullhits + $hits]

	    set fullresults [merge_results $fullresults $sresults]

	}

	set tstr [string range $tstr 0 [expr [string length $tstr] -4]]
	if {[string length $tstr] == 3} {
	    set tstr "0.$tstr"
	} else {
	    set tstr [string range $tstr 0 [ expr [string length $tstr] -4]]
	}

	
	returnfile titles.html [list "\%hits\% $hits" "\%time\% $tstr" "\%returned\% $returned" "\%start\% $startrec"]

	#Generate form for next records now before we screw up indata()

	set tablestart 0
	set navstr ""
	
	if {$numstart > 1} {
	    set tablestart 1
	    append navstr "<table width = 100%><tr><td>"
	    append navstr "<form action = \"$scriptname\" method=post>"
	    
	    set fr [expr $numstart - $numreq]
	    if {$fr < 1 } {
		set fr 1
	    }
	    
	    set indata("firstrec") $fr
	    foreach item [array names indata ]  {
		append navstr "<input type = hidden name = $item value = \"$indata($item)\">"
	    }
	    append navstr "<input type = \"submit\" value = \"Previous $numreq Records\">"
	    append navstr "</form>"
	    append navstr "</td>"
	    
	}
	
	if {[expr $numstart + $numreq] < $hits } {
	    if {$tablestart} {
		append navstr "<td>"
	    }
	    
	    append navstr "<form action = \"$scriptname\" method=post>"
	    set indata("firstrec") [expr $numstart + $numreq]
	    foreach item [array names indata ]  {
		append navstr "<input type = hidden name = $item value = \"$indata($item)\">"
	    }
	    append navstr "<input type = \"submit\" value = \"Next $numreq Records\">"
	    append navstr "</form>"
	    
	    if {$tablestart} {
		append navstr "</td>"
	    }
	}    
	
	if {$tablestart } {
	    append navstr "</tr></table>"
	}
	

	#   Generate numreq records
	set indata("format") "replace"
	set indata("maxrecs") 1
	set indata("firstrec") 1
	set indata("fieldidx1") "REPLACEid"
	set indata("fieldcont1") "replace"
	
	for {set f 2  } { $f <= $maxfield } { incr f } {
	    set iname "fieldidx$f"
	    set cname "fieldcont$f"
	    set indata($iname) ""
	    set indata($cname) ""
	}

	set nodscs 0
	foreach val $inlist {
	    if { [string first render_ $val ] == 0 } {
		set nodscs 1
		break
	    }
	}

	generate_cgihref 0
	regsub -all "format=replace" $cgiquery "format=full" cgia
	regsub -all "format=replace" $cgiquery "format=summary" cgib
	
	puts "<table width = 100% cellpadding = 3>"
	
	foreach record $fullresults {

	    set record [lrange $record 1 end]

	    set relevance [lindex [lindex [lindex $record 0] 1] 1]
	    set docid [lindex [lindex [lindex $record 1] 1] 1]
	    regsub {^( )*} $docid "" docid
	    set docid [cgisub $docid]
	    set title [lindex [lindex [lindex $record 2] 1] 1]
	    set title [ textsub $title ]

	    set relv [string range $relevance 0 [expr [string length $relevance] -2]]
	    append relv "%"


	    if { [llength $record] == 5 } {
		# Component, no summary, link to parent? (need #parent#/#docid for this)
		
		set id [lindex [lindex [lindex $record 3 ] 1] 1]
		set id [textsub $id]
		set parent [lindex [lindex [lindex $record 4 ] 1] 1]
		set parent [textsub $parent]
		regsub "fieldidx1=REPLACE" $cgia "fieldidx1=component" cgiaa
		regsub "fieldcont1=replace" $cgiaa "fieldcont1=$docid" cgiaa

		puts "<tr><td colspan = 2>In <i>$parent</i><br><img src = \"http://gondolin.hist.liv.ac.uk/~cheshire/eadcomp/closed.gif\"><a href = \"$cgiaa\" target=\"dest\">$id</a></td><td>$relv</td></tr>"
		
	    } else {
		
		regsub "fieldidx1=REPLACE" $cgia "fieldidx1=doc" cgiaa
		regsub "fieldidx1=REPLACE" $cgib "fieldidx1=doc" cgibb
		regsub "fieldcont1=replace" $cgiaa "fieldcont1=$docid" cgiaa
		regsub "fieldcont1=replace" $cgibb "fieldcont1=$docid" cgibb

		if {$nodscs} {
		    set targframe "dest"
		} elseif {[llength $record] == 4} {
		    set targframe "_top"
		} else {
		    set targframe "dest"
		}

		
		if {$use_frames == 1} {
		    set line "<tr><td width = 10%><a href =\"$cgibb\" target = \"dest\"><img src=\"$BASE_URL/summary.jpg\" border =0></a></td><td><a href=\"$cgiaa\" target =\"$targframe\" onClick='parent.frames\[2\].document.eadform.field1idx.selectedIndex = 8'>$title</a></td><td>$relv</td></tr>"
		} else {
		    set line "<tr><td width = 10%><a href =\"$cgibb\"><img src=\"$BASE_URL/summary.jpg\" border =0></a></td><td><a href=\"$cgiaa\">$title</a></td><td>$relv</td></tr>"
		}
		puts $line
	    }
	}		    

	puts "</table>"
	
        puts "<hr width = 50%>" 
        puts $navstr
	
	if {$use_frames == 0} {
	    puts "<hr><center>\[ <a href = \"$BASE_URL\">Home</a> \] | \[ <a href = \"${BASE_URL}search.html\">QuickSearch</a> \] | \[ <a href = \"${BASE_URL}searchdeep.html\">In Depth Search</a> \]</center><p>"
	}
	
	puts "</body>"
	puts "</html>"
    }


    "summary" {
	#Generate summary information from elementset BRIEF


	set CHESHIRE_NUM_START $indata("firstrec")
	set CHESHIRE_NUMREQUESTED 1
	set CHESHIRE_ELEMENTSET C
	set CHESHIRE_RECSYNTAX grs1

	set err [catch {eval $query} qresults]
	set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	set sresults [lrange $qresults 1 end]


	set sresults [lindex [ lindex $sresults 0] 1]
	regsub -all "{tagType:: 3}" $sresults "" sresults
	regsub -all "{tagType:: 2}" $sresults "" sresults
	regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
	regsub -all "\"" $sresults {\&quot;} sresults

	set ignorebelow {HEAD EADHEADER DSC}
	set ignoretotal {BIOGHIST NOTE ADD ADMININFO ODD LEGALSTATUS DSC}

 	print "<html><body bgcolor = white>"

	    foreach item [lrange $sresults 1 end] {
		processitem $item
	    }
	print "<p><hr>"

	if {$use_frames == 0} {
	    puts "<hr>\[ <a href = \"$BASE_URL\">Home</a> \] | \[ <a href = \"${BASE_URL}search.html\">QuickSearch</a> \] | \[ <a href = \"${BASE_URL}searchdeep.html\">In Depth Search</a> \]<p>"
	}
	
	print "<p></body></html>"

	set id $indata("fieldcont1")
	addtolog "summary" "$id"

    }
    "full" {


	#Check if file is already written and shouldn't be updated.
	set toc_eadid $indata("fieldcont1");

	set ignore_cache 0
	# Process user defined output if given.
	# If we want generated, then we can't write the files.
	# Which means we never ever want to generate DSC or it would be SILLY.
	
	foreach val $inlist {
	    if { [string first render_ $val ] == 0 } {
		set tagname [string range $val 7 end]
		lappend ignoretotal $tagname
		set ignore_cache 1
	    }
	}
	if {$use_frames == 0} {
	    set ignore_cache 1
	}

	if {$ignore_cache} {
	    lappend ignoretotal C02
	    set output_to_stdout 1
	    set output_to_file 0
	}
	
	set ignore_cache 1

	#Top file to return is $docidA0 for singles, or $docid for multiples
	#Need to check which we should throw
	#toc_eadid == docid from version 2.8
	# for use_frames == 0, always return $docidA0
	#Don't check cache if we've got a render_ request

	if {$ignore_cache == 0} {
	
	    set filename $toc_eadid
	    set filename2 $toc_eadid
	    append filename "A0.html"
	    append filename2 ".html"
	    set filepath "$HTML_ROOT/html/"
	    set filepath2 "$HTML_ROOT/html/"
	    append filepath $filename
	    append filepath2 $filename2
	    
	    if {[file exists $filepath] && $last_index_time < [file mtime $filepath]} {
		addtolog "full" "cached" "$filepath"
		
		if { $use_frames == 1 && [ file exists $filepath2] } {
		    puts stderr "Returned $filepath2"
		    returntocfile $filepath2
		} else {
		    returntocfile $filepath
		}
		exit
	    }
	}


	set CHESHIRE_NUM_START $indata("firstrec")
	set CHESHIRE_NUMREQUESTED 1
	set CHESHIRE_RECSYNTAX grs1
	set err [catch {eval $query} qresults]
	set sresults [lrange $qresults 1 end]

	#Strip headers to ead tag, and play with results a bit
	set sresults [lindex [ lindex $sresults 0] 1]
	regsub -all "{tagType:: 3}" $sresults "" sresults
	regsub -all "{tagType:: 2}" $sresults "" sresults
	regsub -all "{tagOccurrence:: 0}" $sresults "" sresults
	regsub -all "\"" $sresults {\&quot;} sresults


	#Move to the html directory
	cd "$HTML_ROOT/html"
	
	#Build HTML file
	set filename $toc_eadid
	append filename "A0.html"
	set output_to_file 1
	set fileh [open $filename w]
	set filelength($fileh) 0
	set max_filenum 0
	set firstfilename $filename
	
 	print "<html><body bgcolor = white>"
	foreach item [lrange $sresults 0 end] {
	    processitem $item
	}
 	print "<p><hr>"

	# Note puts, not print, so we should just write to stdout but not the file
	if {$use_frames == 0} {
	    puts "<p><center>\[ <a href = \"$BASE_URL\">Home</a> \] | \[ <a href = \"${BASE_URL}search.html\">QuickSearch</a> \] | \[ <a href = \"${BASE_URL}searchdeep.html\">In Depth Search</a> \]</center><p>"
	}
	
	print "<p></body></html>"
	close $fileh

#	if {$ignore_cache} {
#	    exit;
#	}


	if {!$contains_dsc} {
	    addtolog "full" "generated" "$filename"
	    returntocfile $firstfilename
	    exit 
	}

	unset sresults
	unset item
	

	#Return and store ToC file
	set filename $toc_eadid
	append filename ".html"
	set fileh [open $filename w]
	set output_to_stdout 1
	set filelength($fileh) -1

	print_jstoc_file
	close $fileh

	addtolog "full" "generated" "$filename"
    }
    default {
	#Unrecognised format. Die gracefully.
	set format $indata("format");
	returnfile err.html [list "\%err\% An unrecognised record format, $format, was requested. Valid formats are: sgml, titles, summary, full, edit , scan"]
    }
}

}
