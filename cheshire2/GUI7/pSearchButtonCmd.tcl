# pSearchButtonCmd -- Command associated with "SEARCH" button in button
#   bar.

proc pSearchButtonCmd {} {
    global index searchTerm currFormat nHits nRetr searchSet stdFmt
    global currHostName currPacketSize dialogButton currHistory
    global boolSwitch1 boolSwitch2 boolSwitch3 currSearch
    global currProbType probSearchTerm currHost csStage
    global recsyntax rectype fname MARC SGMLMARC OPAC SGML SUTRS dtd_files
    global tmpdir cmd HTAuthor HTSubject HistSrch ERROR
    global unusedButtonName currHostType currClassClusDB ccDTDname

    # set the 'zfind' command to empty
    set cmd ""
    set first 1;                       # 1 = first time thru command loop
    
    # Needed to handle hyperlink hits during cluster searches
    if {$currSearch == "pr"} {
	set prob_string [.search.probentry get 0.0 end]
	set probSearchTerm [string trim $prob_string]
    }
    set saveCSstage $csStage
    
    # if this is a boolean search...build a boolean zfind command string
    if {$currSearch == "bl"} {
	# Build search command
	foreach i [array names index] {
	    if {("$index($i)" != "unused") && ("$searchTerm($i)" != "")} {
		if {$first == 1} {
		    append cmd "zfind \"("
		    set first 0
		}
		switch $i {
		    0 {
			append cmd [string tolower \
					"$index($i) \{$searchTerm($i)\}"]
			if {"$searchTerm(1)" == ""} {
			    append cmd ")"
			    if {("$searchTerm(2)" == "") &&
				("$searchTerm(3)" == "")} {
				break
			    } else {
				if {("$boolSwitch2" == "AND")} {
				    append cmd " and ("
				} elseif {("$boolSwitch2" == "NOT")} {
				    append cmd " not ("
				} else {
				    append cmd " or ("
				}
			    }
			} else {
			    if {("$boolSwitch1" == "and")} {
				append cmd " and ("
			    } elseif {("$boolSwitch1" == "not")} {
				append cmd " not ("
			    } else {
				append cmd " or ("
			    }
			}
		    }
		    
		    1 {
			append cmd [string tolower \
					"$index($i) \{$searchTerm($i)\})"]
			if {("$searchTerm(2)" == "") &&
			    ("$searchTerm(3)" == "")} {
			    break
			} else {
			    if {("$boolSwitch2" == "AND")} {
				append cmd " and ("
			    } elseif {("$boolSwitch2" == "NOT")} {
				append cmd " not ("
			    } else {
				append cmd " or ("
			    }
			}
		    }
		    
		    2 {
			append cmd [string tolower \
					"$index($i) \{$searchTerm($i)\}"]
			if {"$searchTerm(3)" == ""} {
			    append cmd ")"
			    break
			} else {
			    if {("$boolSwitch3" == "and")} {
				append cmd " and "
			    } elseif {("$boolSwitch3" == "not")} {
				append cmd " not "
			    } else {
				append cmd " or "
			    }
			}
		    }
		    
		    3 { 
			append cmd [string tolower \
					"$index($i) \{$searchTerm($i)\})"]
		    }
		}
	    }
	}
	append cmd "\""
	
    } elseif {$currProbType == "cs" && \
		  $probSearchTerm != ""} {

      #  This is a class cluster search.  Note that type cs searches with empty
      #   strings in the prob window are treated as direct rank searches.  This
      #   will only happen when all windows are empty, or when a hyperlink is
      #   activated in the aftermath of a cluster search.

	# Get rid of anything in boolean indexes (they won't be used anyway)
	for {set i 0} {$i < 2} {incr i} {
	    set index($i) "unused"
	    set searchTerm($i) ""
	    .search.button$i configure -text $unusedButtonName \
		-state disabled
	    .search.entry$i configure -state disabled
	}
	
	# build the classification cluster search (pretty easy)
	append cmd "zfind \"topic @ "
	append cmd [string tolower "\{$probSearchTerm\}\" resultsetid \
Clusters"]
	
	
    } else {
	
	# This is a probabilistic search
	# current direct rank search hard-coded for @topic index
	# note that pure boolean searches are also handled here when
	# the search interface is set to probabilistic form
	
	# Get the query terms for the probabilistic search (if any)
	set prob_string [.search.probentry get 0.0 end]
	set probSearchTerm [string trim $prob_string]
	
	# start building the probabilistic query
	if {$currProbType == "dr"} {
	    if {$probSearchTerm != ""} {
		if {$first == 1} {
		    append cmd "zfind \"("
		    set first 0
		}
		append cmd [string tolower "topic @ \{$probSearchTerm\})"]
		foreach i [array names index] {
		    if {("$index($i)" != "unused") && ("$searchTerm($i)" != "")} {
			if {("$boolSwitch1" == "and")} {
			    append cmd " and ("
			} elseif {("$boolSwitch1" == "not")} {
			    append cmd " not ("
			} else {
			    append cmd " or ("
			}

			break
		    }
		}
	    }
	}
	# after putting probabilistic terms (if any) in search command,
	# add boolean component (if any)
	foreach i [array names index] {
	    if {("$index($i)" != "unused") && ("$searchTerm($i)" != "")} {
		if {$first == 1} {
		    append cmd "zfind \"("
		    set first 0
		}
		switch $i {
		    0 {
			append cmd [string tolower \
					"$index($i) \{$searchTerm($i)\}"]
			if {"$searchTerm(1)" == ""} {
			    append cmd ")"
			} else {
			    if {("$boolSwitch1" == "and")} {
				append cmd ") and ("
			    } elseif {("$boolSwitch1" == "not")} {
				append cmd ") not ("
			    } else {
				append cmd ") or ("
			    }
			}
		    }
		    1 {
			    append cmd [string tolower \
				    "$index($i) \{$searchTerm($i)\})"]
		    }
		}
	    }
	}
	append cmd "\""
    }

    # puts "cmd is $cmd"

    # if there's no terms in search, display error and bail out
    # Cmd will consist of terminal quote only if no search terms
    
    if {[string length $cmd] <= 1} {
	
	pDialog .d {Empty Search} \
	    "Cannot submit search. At least one search term is required." \
	    {error} 0 OK
	set ERROR "User submitted empty search."
	pLogCmd 37

    # else if it's a cluster search
    } elseif {$currSearch == "pr" && \
		  $currProbType == "cs" && \
		  $probSearchTerm != ""} {
	
	# Again, type cs searches with no probabilitic terms are handled as
	#  direct rank searches
	
	# we're going to switch the database we're talking to at the
	# cheshire server, so save some information about our current
	# connection so we can restore it later
	set saveDB [lindex [zshow database] 1]
	set saveRecsyntax [lindex [zshow recsyntax] 1]
	set saveESN [lindex [zshow pElementSetNames] 1]
	set saveRSN [lindex [zshow sResultSetName] 1]
	set saveRSId [lindex [zshow pResultSetId] 1]
	
	# set setname and record syntax for getting metadata
	zset pElementSetNames B
	zset recsyntax marc
	
	pClearSearch
	set csStage 1
	
	# see if client machine has previously grabbed the class. cluster DTD
	set ccDTDname "classcluster.dtd"
	if {[file exists $tmpdir$ccDTDname] == 0} {
	    set ccDTDname "classcluster.dtd"
	}
	
	# if we *don't* have the DTD, switch over to the metadata database,
	# and get it from cheshire 2 server
	if {[file exists $tmpdir$ccDTDname] == 0} {
	    # Locate and retrieve the cluster DTD
	    zset database metadata;	 	
            zfind DTD "$currClassClusDB"
	    set temp_dtd [lindex [zdisplay 1 1] 1]
	    set clus_dtd_file [open $tmpdir$ccDTDname w+ 0666]
	    puts $clus_dtd_file $temp_dtd
	    close $clus_dtd_file
	}
	
	# now see if we have the SGML/MARC DTD for Cheshire 2
	set bibDTDname "$currHost$SGMLMARC.dtd"
	if {[file exists $tmpdir$bibDTDname] == 0} {
	    set bibDTDname "$currHost$SGMLMARC.dtd"
	}
	
	# if we *don't* have the SGML/MARC DTD, switch over to the metadata
	# database, and grab it
	if {[file exists $tmpdir$bibDTDname] == 0} {
	    zset database metadata
	    zfind CLUSTERS "$currClassClusDB"
	    set dbName [lindex [zdisplay] 1]
	    zfind DTD $dbName
	    set temp_dtd [lindex [zdisplay] 1]
	    set bib_dtd_file [open $tmpdir$bibDTDname w+ 0666]
	    puts $bib_dtd_file $temp_dtd
	    close $bib_dtd_file
	}
	
	
	# Get cluster records

	# first, switch to classcluster database and set recsyntax to sgml
	
	zset database "$currClassClusDB"
	zset recsyntax sutrs

	# Log transaction
	set mixed 0

	if {$HistSrch == "1"} {
	    pLogCmd 17
	} elseif {$currSearch == "pr"} {
	    if {$currProbType == "cs"} {
		foreach i [array names index] {
		    if {"$searchTerm($i)" != ""} {
			if {"$probSearchTerm" != ""} {
			    set mixed 1
			}
		    }
		}
		if {$mixed == "1"} {
		    pLogCmd 9
		} else {
		    pLogCmd 7
		}
	    }
	    if {$currProbType == "unused"} {
		foreach i [array names index] {
		    if {"$searchTerm($i)" != ""} {
			pLogCmd 5
		    }
		}
	    }
	}
	
	set HTAuthor 0
	set HTSubject 0
	set HistSrch 0
	
	# send query to cheshire server

	pActionLabelOn "Searching" 1 flash

	set err [catch {eval $cmd} retrieval]
	pActionLabelOn "Inactive" 0 noflash

	# if there was an error, tell user, and restore the Z39.50 connection
	# earlier state, then get out of here

	if {$err !=0} {
	    # If/else statement is new, used to just be else statement
            # new error message put in for clearer handling of Resource
	    # Control
	    if {[string first "Resource Control" $retrieval] == -1} {
		pDialog .d {Search Error} [lindex $retrieval 1] {error} 0 OK
	    } else {
		pDialog .d {Search Error} "The server you're searching is currently unable to complete this search.  You might try a more specific query, or trying at another time." {error} 0 OK
	    }

	    zset pElementSetNames $saveESN
	    zset database $saveDB
	    zset recsyntax $saveRecsyntax
            return
	} else {
	    # if there wasn't an error, add the query to the search history
	    set currHistory [linsert $currHistory 0 $cmd]
	    .search.bbar.history configure -state normal
	}

	
	# get the number of matching records/number to receive,
	# then update the status bar display
	set header [lindex $retrieval 0]
	set nHits [lindex [lindex $header 2] 1]
	set nRcvd [lindex [lindex $header 3] 1]
	set nRetr $nRcvd
	
	.statusbar.top.bbar2.feedback configure -state normal
	.statusbar.bottom.a.hitsLabel configure -fg black
	.statusbar.bottom.a.scale configure -fg black \
	    -to $nHits -state normal
	.statusbar.bottom.a.scale set $nRetr
	update
	

	# Format and display cluster records
	
	pActionLabelOn "Retrieving" 1 noflash
	pClearROText .retrieve.text
	.mbar.format configure -state disabled

	for {set i 1} {$i <= $nRcvd} {incr i} {
	    set searchSet($i) [lindex $retrieval $i]
	    pLabelRecord .retrieve.text $i "Topic Area %d\."
	    pAppendROText .retrieve.text "\n\n%s\n\n" \
		[zformat lccshort $searchSet($i) 2 -1 500 $tmpdir$ccDTDname]
	}
	
	if {($nRetr < $nHits) && ($nRetr < $currPacketSize)} {
	    pMoreButtonCmd [expr $currPacketSize-$nRetr]
	} else {
	    .statusbar.bottom.a.scale configure -state disabled
	    pActionLabelOn "Inactive" 0 noflash
	}
	
	# restore the previous state of the Z39.50 connection
	zset pElementSetNames $saveESN
	zset database $saveDB
	zset recsyntax $saveRecsyntax
	zset sResultSetName $saveRSN
	zset pResultSetId $saveRSId
	
    } else {
	# ...else you're dealing with a ranked-by-record or boolean
	# search

	zset pResultSetId Default

	pClearSearch
	
	# Hyperlink searces performed in the aftermath of cluster
	#  searches are handled here, and csStage will be 2
	
	if {$currProbType == "cs"} {
	    set csStage $saveCSstage
	}
	
	# find out if we're using MARC or SGML before searching
	set err [catch {eval zshow recsyntax} showstr]
	if {$err == 0} {
	    #set record syntax to current PreferredRecSyntax
	    set recsyntax [string range $showstr 10 28 ]
	} else {
	    #set record syntax to MARC
	    set recsyntax 1.2.840.10003.5.10
	}
	# if the rectype is SGML, get the DTD from the server
	if {$recsyntax == "1.2.840.10003.5.101"} {
	    if {($currHostType == "Ranked")} {
		set rectype $SGMLMARC
		if {$currHost == "trec"} {
		    set rectype $SGML
		}
	    
		set file_exists 0
		foreach l $dtd_files {
		    if {$l == "$tmpdir$currHost$rectype.dtd"} {
			set file_exists 1
			break
		    }
		}
		if {$file_exists == 0} {
		    # find out current database name
		    set err [catch {zshow sDBNames} zdbnames]
		    set dbname [lindex $zdbnames 1]
		    # Query metadata database for current DTD
		    # and get DTD
		    zset database metadata
		    zfind name $dbname
		    set temp_dtd [lindex [zdisplay 1 1] 1]
		    set fname "$currHost$rectype.dtd"
		    set dtd_file [open $tmpdir$fname w+ 0666]
		    puts $dtd_file $temp_dtd
		    close $dtd_file
		    lappend dtd_files $tmpdir$fname
		    zset database $dbname
		} else {
		    set fname "$currHost$rectype.dtd"
		}
	    } else {
		set rectype $SUTRS
	    }
	} else {
	    # if we're not using SGML, check to see if we're using
	    # a primary melvyl database, and if so, use Z39.50 OPAC format.
	    # otherwise, just use USMARC
	    if {$currHost == "melvyl" || \
		$currHost == "ten" || \
		$currHost == "pe"} {
		zset recsyntax opac
		set rectype $OPAC
	    } else {
		set rectype $MARC
	    }
	}
	
	# log the search

	pActionLabelOn "Searching" 1 flash
	
	set mixed 0
	set bools 0

	if {$HTAuthor == "1"} {
	    pLogCmd 10
	} elseif {$HTSubject == "1"} {
	    pLogCmd 11
	} elseif {$HistSrch == "1"} {
	    pLogCmd 17
	} elseif {$currSearch == "bl"} {
	    pLogCmd 5
	} elseif {$currSearch == "pr"} {
	    if {$currProbType == "dr"} {
		foreach i [array names index] {
		    if {("$searchTerm($i)" != "")} {
			if {"$probSearchTerm" != ""} {
			    set mixed 1
			} 
		    }
		}
		if {$mixed == "1"} {
		    pLogCmd 8
		} else {
		    pLogCmd 6
		}
	    }
	    if {$currProbType == "cs"} {
		foreach i [array names index] {
		    if {("$searchTerm($i)" != "")} {
			if {"$probSearchTerm" != ""} {
			    set mixed 1
			}
		    }
		}
		if {$mixed == "1"} {
		    pLogCmd 9
		} else {
		    pLogCmd 7
		}
	    }
	    if {$currProbType == "unused"} {
		foreach i [array names index] {
		    if {("$searchTerm($i)" != "")} {
			    pLogCmd 5
		    }
		}
	    }
	}
	
	set HTAuthor 0
	set HTSubject 0
	set HistSrch 0

	# send the query off to the server
	set err [catch {eval $cmd} retrieval]
	pActionLabelOn "Inactive" 0 noflash
	
	set header [lindex $retrieval 0]
	
	# if we've got an error, tell the user, log it, and bail on out
	if {$err == 1} {
	    # If/else statement is new, used to just be else statement.
            # new error message put in for clearer handling of Resource Control
	    if {[string first "Resource Control" $retrieval] == -1} {
		pDialog .d {Search Error} [lindex $retrieval 1] {error} 0 OK
	    } else {
		pDialog .d {Search Error} "The server you're searching \
    is currently unable to complete this search.  You might \
    try a more specific query, or trying at another time." {error} 0 OK
	    }

	    set ERROR $retrieval
	    pLogCmd 37
	    return
	} else {
	    # if successful search, add query to history, reconfigure
	    # lower button bar
	    set currHistory [linsert $currHistory 0 $cmd]
	    .search.bbar.history configure -state normal
	    .statusbar.top.bbar2.mail configure -state normal
	    .statusbar.top.bbar2.print configure -state disabled
# the following line ought to be used, but we've disabled printing
# in the client for use in teh AMS library
#	    .statusbar.top.bbar2.print configure -state normal
	    if {$currSearch == "pr"} {
		.statusbar.top.bbar2.feedback configure -state normal
	    }
	    .statusbar.top.bbar2.save configure -state normal
	    set nRcvd [lindex [lindex $header 3] 1]
	}    
	
	# find out number of hits and reconfigure status bar
	set nHits [lindex [lindex $header 2] 1]
	set nRetr $nRcvd
	
	.statusbar.bottom.a.hitsLabel configure -foreground black
	.statusbar.bottom.a.scale configure -foreground black \
	    -to $nHits -state normal
	.statusbar.bottom.a.scale set $nRetr
	update
	
	# grab the records and display them
	# note that we don't allow user to change record display format
	# when grabbing new records from server (it makes the SGML
	# formatting code explode).
	pActionLabelOn "Retrieving" 1 noflash
	pClearROText .retrieve.text
	.mbar.format configure -state disabled

	for {set i 1} {$i <= $nRcvd} {incr i} {
	    set searchSet($i) [lindex $retrieval $i]
	    pLabelRecord .retrieve.text $i "%d\."
	    pAppendROText .retrieve.text "\n\n%s\n\n" \
		[zformat $currFormat $searchSet($i) $rectype -1 80 $tmpdir$fname]
	}
	
	if {($nRetr < $nHits) && ($nRetr < $currPacketSize)} {
	    pMoreButtonCmd [expr $currPacketSize-$nRetr]
	} else {
	    .statusbar.bottom.a.scale configure -state disabled
	    pActionLabelOn "Inactive" 0 noflash
	}

	.mbar.format configure -state normal
    }
}









