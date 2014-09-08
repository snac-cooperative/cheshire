

#--------------------------------------------------------------
# ead2html-tcl init
# Do NOT reorder these lists, as they are indexed from one to another.
# Needed here for both summary and full

set ead_subs { ADDRESS ADDRESSLINE BLOCKQUOTE CHRONLIST CHRONITEM CHANGE DEFITEM DIV HEAD01 HEAD02 ITEM INDEX INDEXENTRY LABEL LB LIST P TITLE  THEAD TBODY}
set html_subs {address br blockquote ul li li  li hr b b li ul li b br ul p i  THEAD TBODY}
set html_sub_entities {"" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" "" ""  "" ""}

set eadheadlist {ACCESSRESTRICT ACQINFO ACCRUALS ADD ADMININFO ALTFORMAVAIL APPRAISAL ARRANGEMENT BIBLIOGRAPHY BIOGHIST CUSTODHIST FILEPLAN INDEX NOTE ODD ORGANIZATION OTHERFINDAID PREFERCITE PROCESSINFO RELATEDMATERIAL SCOPECONTENT SEPARATEDMATERIAL USERESTRICT CONTROLACCESS}
set textheadlist {"Access Conditions" "Immediate Source of Acquisition" "Accruals" "Further Information" "Administrative Information" "Existence of Copies" "Appraisal, Destruction, Scheduling" "System of Arrangement" "Publication Note" "Administrative/Biographical History" "Custodial History" "File Plan" "Index" "Note" "" "Organization" "Finding Aids" "Preferred Citation" "Dates of Accumulation" "Related Units of Description" "Scope and Content" "Associated Material" "Copyright/Reproduction" "Access Points"}

set eadtextlist {AUTHOR CONTAINER CREATION EDITION LANGUSAGE ORIGINATION  PHYSFACET PHYSLOC PUBLISHER REPOSITORY SERIESSTMT SPONSOR UNITDATE UNITID UNITLOC UNITTITLE LANGMATERIAL ABSTRACT EXTENT PHYSDESC}
set texttextlist { "<br><b>Author</b>: " "<br><b>Container</b>: " "<br><b>Creation Information</b>: " "<br><b>Edition</b>: " "<br><b>Language</b>: " "<br><b>Name of Creator</b>: "   "<br><b>Physical Description</b>: " "<br><b>Physical Location</b>:" "<br><b>Published by</b>: " "<br><b>Held at</b>: " "<br><b>Archive Series</b>:" "<br><b>Sponsor</b>: " "<br><b>Dates of creation</b>: " "<br><b>Reference</b>:  " "<br><b>Location</b>: "  "<br><b>Title</b>:" "<br><b>Language</b>: " "<br><b>Abstract</b>:  " "<br><b>Extent:</b>" "<br><b>Physical Description</b>:"}

set catags {PERSNAME FAMNAME CORPNAME GEOGNAME SUBJECT GENREFORM FUNCTION OCCUPATION}
set catexts {"<br><b>Personal Name:</b> " "<br><b>Family Name:</b>" "<br><b>Corporate Name:</b>" "<br><b>Place Name:</b>" "<br><b>Subject:</b>" "<br><b>Genre or Form:</b>" "<br><b>Function:</b>" "<br><b>Occupation:</b>"}

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
set ignoretotal {CHESHIRETITLE CHESHIREID CHESHIRELOCATION CHESHIREACCESS CHESHIREHISTORY}
set ignorebelow {HEAD DROW}
set ignoretags {TITLEPAGE FRONTMATTER DID ARCHDESC PERSNAME GEOGNAME CORPNAME PLACENAME EVENT LANGUAGE LISTHEAD NOTESTMT TITLESTMT PHYSFACET DROW DENTRY TSPEC FAMNAME ABBR DIMENSIONS PUBLICATIONSTMT EDITIONSTMT SUBJECT OCCUPATION 19 DocumentContent DATE TGROUP COLSPEC EAD CHESHIREDOCUMENT} 
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

set include_flat_tags {}
set ignore_flat_tags {}
set empty_tags {LB PTR SPANSPEC PTRLOC EXTPTRLOC EXTPTR COLSPEC }
set cafldidx 0

# End of variable initialisation	
#-----------------------------------------------------



proc processitem item {
    global element_order eadheadlist textheadlist eadtextlist texttextlist ead_subs html_subs topheadings ignorebelow html_no_ends eademph emphstart emphend ignoretags debugtags hreftags EADID FILEDESC REVISIONDESC PROFILEDESC fm_fd_duplicates fdstart fdend fdtags titledone titletags titlestart titleend LEGALSTATUS LANGMATERIAL LEVEL ctaglist clevel ccount toclist toc_title max_filenum contains_dsc catags catexts curr_toc_title output_to_file ignoretotal  unittitlestart unittitleend indenthr start end fields subjectsubstarts subjectsubends namesubstarts persnamesubends famnamesubends namesubfields subjectsubfields catype bnterms cafldidx maxcafld CGI_URL scriptname html_sub_entities tabledimensions tableattrs tablecellidx tablecellattrs nolinkcafields;
    
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
		    if { [ regexp -nocase {GB ([0-9]+).+} $string null idnum ] } {
			regsub "^0*" $idnum "" idnum 
			set nstring "<a target = \"new\" href = \"http://www.hmc.gov.uk/archon/searches/locresult.asp?LR="
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
	    return

	} elseif {$elemname == "UNITTITLE" && [lsearch $element_order "DSC"] != -1} { 

	    print $unittitlestart
	    set appendtxt $unittitleend

	} elseif {$elemname == "UNITID" && [lsearch $element_order "DSC"] != -1} {
	    print "<br><b>Reference</b>: "

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
	    if {$htag != "br" && $htag != "hr"} {
	  	set appendtxt "</$htag>"
	    }
	    
	} elseif {$elemname == "DSC"} {
	    #Need to clear the eadheader information so that it's not displayed with each DID!
	    foreach clear {EADID FILEDESC REVISIONDESC PROFILEDESC LEGALSTATUS LANGMATERIAL LEVEL } {
		set $clear ""
	    }
	    set contains_dsc 1
	    set appendtxt "<hr>"
	    
	} elseif { [lsearch $ctaglist $elemname] != -1 } {
	    #Build ToC list as per ead2html
	    # Uhhh, no.

	    if {$indenthr != ""} {
		print $indenthr
	    } else {
		print "<hr>"
	    }
	    print "<a name = \"$ccount\">"


	} elseif { [set caidx [lsearch $catags $elemname]] != -1 && [lsearch $element_order "CONTROLACCESS"] != -1} {

	    # Print heading
	    print [lindex $catexts $caidx];
	    set cafldidx 0

	    if { [sub_elem_index EMPH $item] != -1 } {
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
		    set end $famnamesubends;
		    set fields $namesubfields
		} else {
		    
		    set start $subjectsubstarts
		    set end $subjectsubends
		    set fields $subjectsubfields
		}

	    }  else {
		set appendtxt "<br>"
	    }	    


	} elseif { $elemname == "EMPH" } {

	    if { [ lsearch $element_order CONTROLACCESS ] != -1  && [set aridx [ sub_elem_index ALTRENDER $item]] != -1} {
		incr cafldidx
		set fld  [sub_elem_contents $aridx $item];
		set fld [string tolower $fld]

		if { [set fldidx [ lsearch $fields $fld]] != -1 } {
		    print [lindex $start $fldidx]

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
	    #Map some to http://foo, others to an eadid search.

	    set href [sub_elem_contents $hridx $item]
	    set href [ textsub $href ]
	    
	    set showtxt ""
	    if { [set aidx [sub_elem_index "SHOW" $item]] != -1} {		
		set showtxt [sub_elem_contents $aidx $item]
		set showtxt [string tolower $showtxt]
	    }
	    
	    if { $showtxt == "embed" && [ regexp {\.(jpg|gif|png|jpeg)$} $href ] } {
		print "<img src =\"$href\">"
	    } else {
		set target " target = \"_top\""
		if {[string tolower $showtxt] == "new" } {
		    set target " target = \"new\""
		}
		
		
		if {[regexp {\[^ \]*\.\[^ \]\[^ \]\[^ \]*$} $href ] } {
		    print "<a href = \"$href\" $target>"
		} elseif { [regexp {^(http:|ftp:|mailto:|telnet:|news:|shttp:).*} $href] } {
		    print "<a href = \"$href\" $target>"
		} 
		set appendtxt "</a>"
	    }
	} elseif { $elemname == "LIST"} {
	    set tag "ul"

	    if { [set tidx [sub_elem_index "TYPE" $item]] != -1 } {
		set type [sub_elem_contents $tidx $item]
		set type [string tolower $type]

		if {$type == "ordered"} {
		    set tag "ol"
		} elseif { $type == "deflist" } {
		    set tag "dl"
		}
	    }

	    set attrs ""
	set htmltype ""
	    if { [set nidx [sub_elem_index "NUMERATION" $item]] != -1} {
		set numer [sub_elem_contents $nidx $item]
		set numer [string tolower $numer]

		# This uses Deprecated HTML! Should use CSS.
		set numertypes {arabic uppperalpha loweralpha upperroman lowerroman}
		set htmlntypes {1 A a I i}
		set htmltyle [lindex $htmlntypes [lsearch $numertypes $numer]]
		set attrs " type=\"$htmltype\""
 	    }		

	    print "<$tag $attrs>"
	    set appendtxt "</$tag>"

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
	    # We want to process Drow etc cleverly. 
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
	    puts stderr "Unrendered EAD Element: $elemname"
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
	    
	    if {$LEVEL != {}} {
		print "<br><b>Level of Description</b>: $LEVEL";
	    }
	    
	    # print "<hr width = 40%>"
	    
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
	    
	} elseif { $elemname == "FRONTMATTER" } {
	    print "<hr width = 40%>"
	} 
	
	if {$elemname != "19"} { 
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	}
	
    } else {
	#Debug unknown grs1 syntax to stderr
	# puts stderr "Unknown GRS:: $item<br>"
    }
}


proc print {text} {
    global eadhtmlrec
    append eadhtmlrec $text
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

proc cgisub {id} {
    # XXX Need to reverse process in decode_cgi
    # XXX Rename to cgi_encode

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
