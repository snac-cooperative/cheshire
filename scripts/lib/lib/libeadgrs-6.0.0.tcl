
#     ----- THIS IS THE WIKI  VERSION OF EADSEARCH6.CGI -----
#Author:  Rob Sanderson (azaroth@liv.ac.uk)
#Copyright:  &copy; (c) The University of Liverpool, All Rights Reserved
#Last Modified: 18/06/01
#Description:  EAD searching with ead2html-tcl and cheshire2
#Version: 6.0.0
# All we're supposed to do is write EAD to HTML and generate the form from given data.

source "lib/libgrs-1.0.0.tcl"

set HTML_ROOT "/home/cheshire/public_html/eadcomp"
set EAD_ROOT "/home/cheshire/cheshire/eadhub/ead-dir"
set BASE_URL "http://gondolin.hist.liv.ac.uk/~cheshire/eadhub/"
set CGI_URL "http://gondolin.hist.liv.ac.uk/~cheshire/cgi-bin/"


set LOGGING 0
set LOGFILE "$EAD_ROOT/eadsearch.log"
set CACHING 0

set output_to_file 0
set output_to_stdout 1


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

set eadtextlist {AUTHOR CONTAINER CREATION EDITION LANGUSAGE ORIGINATION  PHYSFACET PHYSLOC PUBLISHER REPOSITORY SERIESSTMT SPONSOR UNITDATE UNITID UNITLOC UNITTITLE LANGMATERIAL ABSTRACT EXTENT}
set texttextlist { "<br><b>Author</b>: " "<br><b>Container</b>: " "<br><b>Creation Information</b>: " "<br><b>Edition</b>: " "<br><b>Language</b>: " "<br><b>Name of Creator</b>: "   "<br><b>Physical Description</b>: " "<br><b>Physical Location</b>:" "<br><b>Published by</b>: " "<br><b>Held at</b>: " "<br><b>Archive Series</b>:" "<br><b>Sponsor</b>: " "<br><b>Dates of creation</b>: " "<br><b>Reference</b>:  " "<br><b>Location</b>: "  "<br><b>Title</b>:" "<br><b>Language</b>: " "<br><b>Abstract</b>:  " "<br><b>Extent:</b>"}

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

set record_tags {eadid titleproper creation unitid unittitle unitdate repository extent physfacet origination bioghist scopecontent arrangement custodhist acqinfo appraisal accruals accessrestrict userestrict altformavail note otherfindaid relatedmaterial bibliography controlaccess odd}

set other_rec_tags {threesevenone langmaterial revisions}

foreach rtag $record_tags {
    set $rtag {}
}
foreach rtag $other_rec_tags {
    set $rtag {}
}
set include_flat_tags {}
set ignore_flat_tags {}
set empty_tags {LB PTR SPANSPEC PTRLOC EXTPTRLOC EXTPTR COLSPEC }

set cafldidx 0

# End of variable initialisation	
#-----------------------------------------------------




#---------- Subroutines ----------


# Override the default processitem

proc processitem item {
    global element_order eadheadlist textheadlist eadtextlist texttextlist ead_subs html_subs topheadings ignorebelow html_no_ends eademph emphstart emphend ignoretags debugtags hreftags EADID FILEDESC REVISIONDESC PROFILEDESC fm_fd_duplicates fdstart fdend fdtags titledone titletags titlestart titleend LEGALSTATUS LANGMATERIAL LEVEL ctaglist clevel ccount toclist toc_title max_filenum contains_dsc catags catexts curr_toc_title output_to_file ignoretotal  unittitlestart unittitleend indenthr start end fields subjectsubstarts subjectsubends namesubstarts persnamesubends famnamesubends namesubfields subjectsubfields catype bnterms cafldidx maxcafld CGI_URL scriptname html_sub_entities tabledimensions tableattrs tablecellidx tablecellattrs;
    
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
	set elemname [string toupper $elemname]
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
		    set end $famnamesubends;
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

#		    set href "$scriptname?format=sgmlscan&bool=AND&maxrecs=20&firstrec=1&fieldidx1=$scanindex&fieldcont1=$cgitext"
		    set href  ""
		    print "<a target = \"_top\" href =\"$href\">"
		    set appendtxt "</a>"

		    if { $cafldidx != $maxcafld || $fld == "dates" ||  $fld == "other" ||  $fld == "y" || $fld == "o"} {
			append appendtxt [lindex $end $fldidx]
		    }
		}

	    } elseif { [set aridx [ sub_elem_index ALTRENDER $item]] != -1 } {
		# We should be JISC-HUBXX
		puts "<br><b>Language of Material:</b>"

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
	    
	} elseif { [lsearch $ctaglist $elemname] != -1 } {
	    append toclist "\} \} "
	    incr clevel -1
	} elseif { $elemname == "FRONTMATTER" } {
	    print "<hr width = 40%>"
	} elseif {$elemname != "19"} { 
	    set element_order [lrange $element_order 0 [expr [llength $element_order] -2]]
	}

	} else {
	    #Debug unknown grs1 syntax to stderr?
	}
}


# XXX Need to update this!!!! XXX

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

# XXX Need to update this too somehow XXX

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

# XXX Need to update this too .... XXX

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

# XXX Encorporate this in an implementation neutral form in libgrs

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

proc writeeadform {full}  {
    global record_tags other_rec_tags include_flat_tags ignore_flat_tags castring optname last_attr current_document_info
    foreach rtag $record_tags {
	global $rtag
    }
    foreach rtag $other_rec_tags {
	global $rtag
    }

    set include_flat_tags {TITLE LB P ITEM}
    set ignore_flat_tags {HEAD}

    if {$full} {
	returnfile edithead.html {}
	set key [lindex $current_document_info 1]
	puts "<input type = hidden name = cheshireID value = \"$key\">"
	puts "<input type = hidden name = request value = \"save\">"
    }


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

    if {$full} {
	puts "<p><hr></body></html>"
    }


}

# ----- Component procedures ----------

# XXX Update this to cheshire_sort
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



