
#Procedures in this library:

# function_docselect -> user interface to document functions
# function_edit -> UI to editing a document
# function_save -> UI to saving documents
# function_move -> UI to moving documents
# function_copy -> UI to copying documents
# function_delete -> UI to deleting documents
# function_metadata -> UI to retreiving metadata of document
# function_addeditor -> UI to adding a user as editor of document
# function_deleditor -> UI to deleting user from editor list of document
# function_addflag -> UI to adding a flag to a document
# function_delflag -> UI to deleting a flag from a document
# function_static -> UI to making a document static
# function_revert -> UI to reverting document to previous version
# function_create -> UI to creating a new document

# get_document_title -> return title of current document
#        XXX Should be procs for all parts of CDI
# get_linked_document <info> -> Get linked document from info
#         XXX Unsupported
# get_document <key> - > return document with key
# get_document_by_path <path> -> return document with path
# get_document_info <key> -> return document info of key
# get_document_docid <key> -> return DOCID of key
# get_docid_by_path <path> -> return DOCID of path

# set_metadata <type> <value> -> Set current document's TYPE metadata to VALUE
# generate_record <key> [edit type] -> Generate SGML of full record with new modification of type
# generate_metadata [type] -> Generate metadata SGML
# write_record <key> <SGML> -> Write record to disk
# validate_and_save <SGML> <key> <title> -> validate and save record
# validate_record <SGML> -> check validity of record
# move_deleted_files <key> <docid> -> move deleted files out of data dir
# rebuild_record_database -> rebuild the database


# ---- Document Functions ----

proc function_docselect {key title} {
    global REMOTE_USER 
    throwfile "docselect.html" [list "\%KEY\% $key" "\%TITLE\% $title" "\%USER\% $REMOTE_USER"]
}
    
proc function_edit {key title} {
    throwfile "edit-top.html" [list "\%KEY\% $key" "\%TITLE\% $title" "\%INFO\% <p>"]
    set doc [get_document $key]
    puts $doc
    throwfile "edit-bottom.html" {}
}

proc function_save {key title} {
    global indata current_document_info
    
    # This implies that either the system will rebuild straight away, 
    # or that there will be some forking of multiply edited files.
    # Always rebuilding may make the system unusable as it constantly rebuilds...

    set doc $indata(doctext)
    set record [generate_metadata "edit"]
    append record $doc
    append record "</cheshireDocument>\n</cheshireRecord>\n"

    set myerror [validate_and_save $record $key $title]

    if {$myerror != 0} {
	throwfile "edit-top.html" [list "\%KEY\% $key" "\%TITLE\% $title" "\%INFO\% There was an error detected in your SGML:<blockquote>$myerror</blockquote><p>Please correct this and resubmit."]
	puts $doc
	throwfile "edit-bottom.html" {}
    }
}

proc function_move {key title} {
    global indata current_document_info

    if { ! [maybe_throw_reqform $key $title ] } {
	set location $indata(arg1)
	# Check location for stupidity
	# Don't copy over another record!
	if {[get_document_by_path $location] != 0 } {
	    throw_error_reqform $key $title "Another document already exists at '$location'!"
	} elseif { [regexp {[^A-Za-z0-9._/-]} $location ] } {
	    throw_error_reqform $key $title "The location must not contain non alphanumeric characters, plus / _ and -"
	} else {

	    # Here we need to change location, and regenerate record. Save and reindex.
	    set_metadata "location" $location
	    set doc [generate_record $key "move"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_copy {key title} {
    global indata current_document_info
    
    if { ! [maybe_throw_reqform $key $title ] } {
	set location $indata(arg1)
	set newid $indata(arg2)
	if {[get_document_by_path $location] != 0 } {
	    throw_error_reqform $key $title "Another document already exists at '$location'!"
	} elseif { [regexp {[^A-Za-z0-9._/-]} $location ] } {
	    throw_error_reqform $key $title "The location must not contain non alphanumeric characters, plus / .  _ and -"
	} elseif {$newid == "" } {
	    throw_error_reqform $key $title "You need to give an ID for the new record."
	} elseif { [regexp {[^A-Za-z0-9._-]} $newid ] } {
	    throw_error_reqform $key $title "The ID for the new record must not contain non alphanumeric characters, plus . _ - and = "
	} else {
	    # Should we record the copy in the original and start the new from scratch?
	    # (For the moment, we're going to ignore this idea because it's a pain)

	    set_metadata "id" $newid
	    set_metadata "location" $location
	    set doc [generate_record $key "copy"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }

	}
    }
}

proc function_delete {key title} {
    global indata REMOTE_USER CHESHIRE_CONFIGFILE_DATA CHESHIRE_DATABASE_DATA CHESHIRE_PATH CHESHIRE_ROOT CGI_ROOT
    
    # Require a confirmation.
    if { ! [maybe_throw_reqform $key $title ] } {
	set name $indata(arg1)

	if { $name != $key } {
	    throw_error_reqform $key $title "That key '$name' did not match the ID of the record you are trying to delete."
	} else {
	    # delete_recs [-L logfilename] configfilename filename record_id
	    # Don't delete associator when rebuilding!

	    set docid [get_document_docid $key]

	    # Move to database dir to avoid leaving log files in weird places.
	    cd $CHESHIRE_ROOT

	    exec "$CHESHIRE_PATH/delete_recs" -L logs/DELETERECS_LOGFILE $CHESHIRE_CONFIGFILE_DATA $CHESHIRE_DATABASE_DATA $docid

	    move_deleted_files $key $docid

	    #Record deleted ID, cheshireID, time, user
	    set outh [open "logs/deleted_docids.log" a]
	    puts $outh "$docid:$key:[clock seconds]:$REMOTE_USER"
	    close $outh

	    cd $CGI_ROOT
	    throw_success_file

	}
    }
}

proc function_rename {key title} {
    global indata

    if { ! [maybe_throw_reqform $key $title ] } {
	# Title can be non unique, but can't contain < or > for obvious reasons
	set name $indata(arg1)

	if { [regsub {[<>]} $name null null] } {
	    throw_error_reqform $key $title "The name must not contain &lt; or &gt;."
	} elseif { $name == $title } {
	    throw_error_reqform $key $title "That is the current name for the document."
	} else {
	    set_metadata "title" $name
	    set doc [generate_record $key "rename"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_metadata {key title} {
    global current_document_info
    set cdi $current_document_info

    # Link Editors to a record about them
    # Link Powers to a list of people who have them, in turn to the record about the person

    throwfile "metadata-top.html" [list "\%KEY\% $key" "\%TITLE\% $title"]

    puts "<tr><td colspan = 2><center><b>Basic Information</b></center></td></tr>"
    puts "<tr><td><b>Title:</b></td><td>$title</td></tr>"
    puts "<tr><td><b>ID:</b></td><td>$key</td></tr>"
    puts "<tr><td><b>Location:</b></td><td>[lindex $cdi 2]</td></tr>"
    puts "<tr><td colspan = 2><center><b>Editor Information</b></center></td></tr>"

    set editors [lindex [lindex $cdi 3] 0]
    puts "<tr><td valign = top><b>Editors:</b></td><td>"
    foreach ed $editors {
	puts "$ed<br>"
    }
    puts "</td></tr>"

    set flags [lindex [lindex $cdi 3] 1]
    puts "<tr><td valign = top><b>Editor Flags:</b></td><td>"
    foreach f $flags {
	puts "$f<br>"
    }
    puts "</td></tr>"


    puts "<tr><td colspan = 2><center><b>Record History</b></center></td></tr>"

    set mod [lindex [lindex $cdi 4] 0]
    puts "<tr><td><b>Created:</b></td><td>By [lindex $mod 1] at [lindex $mod 2]</td></tr>"
    foreach mod [lrange [lindex $cdi 4] 1 end] {
	puts "<tr><td><b>Modified:</b></td><td>[lindex $mod 1] by [lindex $mod 2] at [lindex $mod 3]</td></tr>"
    }

    throwfile "metadata-bottom.html" {}
}

proc function_addeditor {key title} {
    global indata current_document_info

    if { ! [maybe_throw_reqform $key $title ] } {
	set editor $indata(arg1)
	# Check editor exists
	set edinfo [get_user_info $editor]
	if {$edinfo == 0} {
	    throw_error_file $key $title "'$editor' is not the name of an editor in the system."
	} elseif {[lsearch [lindex [lindex $current_document_info 3] 0] $editor] != -1} {
	    throw_error_file $key $title "$editor is already an editor for this document."
	} else {
	    set_metadata "addeditor" $editor
	    set doc [generate_record $key "addeditor"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_deleditor {key title} {
    global indata current_document_info

    if { ! [maybe_throw_reqform $key $title ] } {
	set editor $indata(arg1)
	if { [lsearch [lindex [lindex $current_document_info 3] 0] $editor] == -1 } {
	    throw_error_file $key $title "'$editor' is not an editor for this document."
	} else {
	    set_metadata "deleditor" $editor
	    set doc [generate_record $key "deleditor"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_addflag {key title} {
    global indata editor_flag_list

    if { ! [maybe_throw_reqform $key $title ] } {
	set flag $indata(arg1)
	# Check for Power flags. Only allow admins to add them
	if {[lsearch $editor_flag_list $flag] != -1 && [user_has_flag admin] == 0} {
	    throw_error_file $key $title "You must be an administrator to add the '$flag' flag to a record."
	} else {
	    set_metadata "addflag" $flag
	    set doc [generate_record $key "addflag"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_delflag {key title} {
    global indata editor_flag_list current_document_info

    if { ! [maybe_throw_reqform $key $title ] } {
	set flag $indata(arg1)
	# Check for Power flags. Only allow admins to add them
	if {[lsearch [lindex [lindex $current_document_info 3] 1] $flag] == -1 } {
	    throw_error_file $key $title "This record does not have the '$flag' flag."
	} elseif {[lsearch $editor_flag_list $flag] != -1 && [user_has_flag admin] == 0} {
	    throw_error_file $key $title "You must be an administrator to remove the '$flag' flag to a record."
	} else {
	    set_metadata "delflag" $flag
	    set doc [generate_record $key "delflag"]
	    write_record $key $doc
	    if { [rebuild_record_database] == 1} {
		throw_success_file
	    }
	}
    }
}

proc function_static {key title} {
    # XXX Retreive document, write to proper location in the file system.
    # Leave the document in the database - only difference will be that you 
    # can retrieve the info out of it in a search.


}

proc function_revert {key title} {
    # XXX Delete the most recent file and reindex the entire database.
    # This means that we need to trash the assoc file, which buggers with delete.


}

proc function_create {path } {
    global indata REMOTE_USER current_document_info

    set innames [array names indata]
    if { [lsearch $innames "doctext"] != -1 } {
	# We're from the createform, not a create button
	foreach var $innames {
	    set $var $indata($var)
	}
	
 	set errormsg ""
	if { $cheshireID == "" } {
	    set errormsg "You must give an identifier for the document"
	} elseif { [regsub {[^a-zA-Z0-9_.-]} $cheshireID null null ] } {
	    set errormsg "The identifier must contain only alphanumeric characters, plus _. and -"
	} elseif { $title == "" } {
	    set errormsg "You must give a title for the document"
	} elseif {$location == ""} {
	    set errormsg "You must give a location for the document"
	} elseif { [regsub {[^a-zA-Z0-9_./-]} $location null null] } {
	    set errormsg "The location must contain only alphanumeric characters, plus _ . / and -"
	} elseif { $editors == "" && $flags == "" } {
	    set errormsg "You must give at least one editor or editor flag for the document"
	} elseif { $doctext == "" } {
	    set errormsg "You must give at least a minimal document."
	} elseif { [get_document_by_path $location] != 0 } {
	    set errormsg "There is already a document at that location."
	} elseif { [get_document $cheshireID] != 0 } {
	    set errormsg "There is already a document with that identifier."
	} else {

	    # Validate editors
	    foreach ed $editors {
		if { [get_user_info $ed] == 0} {
		    set errormsg "$ed is not a valid editor."
		    break
		}
	    }

	    # XXX Only allow admins to create with predefined flags?

	    if {$errormsg == ""} {
		# Generate metadata into current_document_info
		# Verify and Write record as per edit
		set_metadata "title" $title
		set_metadata "location" $location
		set_metadata "id" $cheshireID
		foreach id $editors {
		    set_metadata "addeditor" $ed
		}
		foreach flag $flags {
		    set_metadata "addflag" $flag
		}

		set doc $indata(doctext)
		set record [generate_metadata ""]
		append record $doc
		append record "</cheshireDocument>\n</cheshireRecord>\n"
		
		set myerror [validate_and_save $record $cheshireID $title]
		
		if {$myerror != 0 } {
		    # Throw back to re-edit
		    set errormsg $myerror
		}
	    }
	}
	
	if {$errormsg != "" } {
	    throwfile "createform.html" [list "\%USERS\% $editors" "\%LOCATION\% $location" "\%DOCTEXT\% $doctext" "\%KEY\% $cheshireID" "\%TITLE\% $title" "\%FLAGS\% $flags" "\%ERR\% $errormsg"]
	}
    } else {
	throwfile "createform.html" [list "\%USERS\% $REMOTE_USER" "\%LOCATION\% [string range $path 1 end]" "\%DOCTEXT\% " "\%KEY\% " "\%TITLE\% " "\%FLAGS\% " "\%ERR\% "]
    }

}


# ------       Retrieval functions      ------

proc get_document_title { } {
    global current_document_info
    return [lindex $current_document_info 0]
}

proc get_linked_document { info } {
    global LINKED_DOCUMENT_INDEX global_variables
    foreach var $global_variables {
	global $var
    }


    # XXX This really isn't supported at the moment.
    # This needs to be done for each configuration
    # Assume default to be: CONFIG:DBNAME:term

    set infolist [split info :]

    if {[llength $infolist] != 3} {
	return {}
    } else {
	set CHESHIRE_DATABASE [lindex $infolist 0]
	set CHESHIRE_CONFIGFILE [lindex $infolist 1]
	set CHESHIRE_ELEMENTSET F
	set CHESHIRE_RECSYNTAX SGML

	set query "search $LINKED_DOCUMENT_INDEX { [lindex $infolist 2] }"

	set err [catch {eval $query} qresults]
	set hits [lindex [lindex [lindex $qresults 0] 0] 1]
	
	if { $err != 0 || $hits != 1 } {
	    return $qresults
	} 
	
	set sresults [lrange $qresults 1 end]
	return [lindex $sresults 0]
    }
}


# Given the primary key for the file, retreive the encapsulated document in SGML
proc get_document { key } {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_ELEMENTSET F
    set CHESHIRE_RECSYNTAX SGML
    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
    set CHESHIRE_CONFIGFILE  $CHESHIRE_CONFIGFILE_DATA

    set query "search ( cheshireID \\\[5=100\\\] {$key} )"

    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]

    if {$err != 0 || $hits != 1} {
	# Could be a deleted record...
	return 0
    } 

    set sresults [lrange $qresults 1 end]
    
    # Strip out our encapsulating metadata
    regsub "<cheshireRecord>(.*)<cheshireDocument>" $sresults "" sresults
    regsub "</cheshireRecord>" $sresults "" sresults
    regsub "</cheshireDocument>" $sresults "" sresults

    #Now we have /either/ the document or a link to it.
    set document [lindex $sresults 0]

    if {$IS_ENCAPSULATED_DOCUMENT == 1} {
	return $document
    } else {
	# This depends on the type of linking
	return [get_linked_document $document]
    }
}

proc get_document_by_path { path } {
    global  global_variables
    foreach var $global_variables {
	global $var
    }

    if {[string index $path 0] == "/"} {
	set path [string range $path 1 end]
    }

    set CHESHIRE_ELEMENTSET "CIDGRS"
    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
    set query "search ( cLocation \\\[5=100\\\] {$path} )"
    
    #Do query, if a single hit, return file from get_document
    #If multiple files, present user with a list?

    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
    set sresults [lrange $qresults 1 end]

    if {$hits == 0 || $err != 0 } {
	puts stderr "Failed: $query -> $qresults"
 	return 0
    } elseif { $hits == 1 } {
	set key [lindex [lindex [lindex [lindex $sresults 0] 1] 1] 1]
	set doc [get_document $key]

	if {$doc == 0} {
	    # We're deleted or something.
	    return $doc
	}

	set form "<form method =\"POST\" action = \"/~cheshire/wiki/\"><input type = hidden name = \"cheshireID\" value =\"$key\"><input name = \"request\" value = \"docselect\" type = hidden><input type = submit value = \"Edit\"> </form>"
	regsub "</body>" $doc "$form</body>" doc
	return $doc
    } else {
	# Bad. Bad bad bad. Can't have multiple primary keys, so just have to check.
	return "Extreme Bad Juju. Multiple files with the same path. Fix me!"
    }
}

# Given the cheshireID, return a reformatted list of metadata
# {title id location { {accesstype accessers}} { {historytype changes}}}

proc get_document_info {key} {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
    
    set query "search cheshireID \\\[5=100\\\]  {$key} "
    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
    
    if {$err != 0 || $hits != 1} {
	return $qresults
    }
    
    set sresults [lrange $qresults 1 end]

    set formatted [ flatten_doc_metagrs $sresults ]

    return $formatted

}

proc flatten_doc_metagrs {sresults} {

    set fresults {}
    foreach res [lindex $sresults 0] {
	if {$res != "GenericRecord::"} {
	    set gtype [lindex [lindex [lindex $res 0 ] 1 ] 2]
	    set contents [lindex [lindex $res 1] 1]
	    
	    if {$gtype == "author"} {
		# cheshireEditors / cheshireEditorFlags

		set aresults {}
		foreach ed $contents {
		    set edtype [lindex [lindex [ lindex [lindex $ed 1] 0] 1] 1]
		    set edtype [string range $edtype 8 [expr [string length $edtype] -2]]
		    set edtype [string tolower $edtype]
		    set edcont [lrange [lindex $ed 1] 1 end]
		    
		    set a2results {$edtype}
		    foreach editor [lindex [lindex $edcont 0] 1] {
			set editor [lindex [lindex [lindex $editor 1] 1] 1]
			lappend a2results $editor
		    }
		    lappend aresults $a2results
		}
		lappend fresults $aresults
	    } elseif {$gtype == "publicationDate"} {
		# cheshireCreated / cheshireModified
		# cheshireModified now has a type attribute
		
		set pdresults {}
		foreach date $contents {
		    set datetype [lindex [lindex [ lindex [lindex $date 1] 0] 1] 1]
		    set datetype [string range $datetype 8 [expr [string length $datetype] -1]]
		    set datetype [string tolower $datetype]
		    set datecont [lrange [lindex $date 1] 1 end]

		    set pd2results [list $datetype]
		    
		    foreach change [lindex [lindex $datecont 0] 1] {
			set change [lindex [lindex [lindex $change 1] 1] 1]
			lappend pd2results $change
		    }
		    lappend pdresults $pd2results
		}
		lappend fresults $pdresults
	    } else {
		# Relies on DBCONFIG being ordered.
		lappend fresults $contents
	    }
	}
    }

    # Mung with editors to {{editors} {flags}}
    set editors [lindex $fresults 3]
    if { [llength $editors] == 1 } {
	if { [lindex [lindex $editors 0 ] 0] == "editor" } {
	    set editors [list [lrange [lindex $editors 0] 1 end] {} ]
	} else {
	    set editors [list  {} [lrange [lindex $editors 0] 1 end] ]
	}
    } else {
	set editors [list [lrange [lindex $editors 0] 1 end] [lrange [lindex $editors 1] 1 end] ]
    }
    set fresults [list [lindex $fresults 0] [lindex $fresults 1] [lindex $fresults 2] $editors [lindex $fresults 4]]

    return $fresults

}

proc get_document_docid { key } {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_ELEMENTSET "DOCIDGRS"
    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
    set query "search ( cheshireID \\\[5=100\\\]  {$key} )"
    
    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
    set sresults [lrange $qresults 1 end]

    if {$hits == 0 || $err != 0 } {
	puts stderr "Failed: $query -> $qresults"
 	return 0
    } elseif { $hits == 1 } {
	set id [lindex [lindex [lindex [lindex $sresults 0] 1] 1] 1]
	return $id
    } else {
	# EXTREME borkage
	return 0
    }
}

proc get_docid_by_path { path } {
    global global_variables 
    foreach var $global_variables {
        global $var
    }

    if {[string index $path 0] == "/"} {
        set path [string range $path 1 end]
    }

    set CHESHIRE_ELEMENTSET "DOCIDGRS"
    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_DATA
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
    set query "search ( cLocation \\\[5=100\\\]  {$path} )"
    
    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]
    set sresults [lrange $qresults 1 end]

    if {$hits == 0 || $err != 0 } {
        return 0
    } elseif { $hits == 1 } {
        set key [lindex [lindex [lindex [lindex $sresults 0] 1] 1] 1]
        return $key
    } else {
        return -2
    }
}



# -----------    File generation functions (etc) --------------

proc set_metadata {type args} {
    global current_document_info
    set cdi $current_document_info

    if { [llength $cdi] == 0 } {
	set current_document_info [list "" "" "" "" ""]
	set cdi $current_document_info
    }

    set editors [lindex [lindex $cdi 3] 0]
    set flags [lindex [lindex $cdi 3] 1]

    switch $type {
	"title" {
	    set current_document_info [lreplace $cdi 0 0 $args]
	}
	"id" {
	    set current_document_info [lreplace $cdi 1 1 $args]
	}
	"location" {
	    set current_document_info [lreplace $cdi 2 2 $args]
	}
	"addeditor" {
	    lappend editors $args
	    set replace [list $editors $flags]
	    set current_document_info [lreplace $cdi 3 3 $replace]
	}
	"deleditor" {
	    set idx [lsearch $editors $args]
	    set eds [lreplace $editors $idx $idx ]
	    set replace [list $eds $flags]
	    set current_document_info [lreplace $cdi 3 3 $replace]

	}
	"addflag" {
	    lappend flags $args
	    set replace [list $editors $flags]
	    set current_document_info [lreplace $cdi 3 3 $replace]

	}
	"delflag" {
	    set idx [lsearch $flags $args]
	    set fs [lreplace $flags $idx $idx ]
	    set replace [list $editors $fs]
	    set current_document_info [lreplace $cdi 3 3 $replace]
	}
    }
}

proc generate_record {key edittype} {
    global current_document_info REMOTE_USER

    set record [generate_metadata $edittype]
    append record [get_document $key]
    append record "</cheshireDocument>\n</cheshireRecord>\n"

    return $record
}

proc generate_metadata {type} {
    global current_document_info REMOTE_USER
    # Run over document_info and add a type modification
    
    set cdi $current_document_info
    
set document "<cheshireRecord>\n \
  <cheshireTitle>[lindex $cdi 0]</cheshireTitle>\n \
  <cheshireId>[lindex $cdi 1]</cheshireId>\n \
  <cheshireLocation>[lindex $cdi 2]</cheshireLocation>\n \
  <cheshireAccess>\n \
"

    if {[llength [set eds [lindex [lindex $cdi 3] 0]]] != 0} {
	append document "<cheshireEditors>\n"
	foreach ed $eds {
	    append document "<cheshireUser>$ed</cheshireUser>\n"
	}
	append document "</cheshireEditors>\n"
    }
    if {[llength [set eds [lindex [lindex $cdi 3] 1]]] != 0} {
	append document "<cheshireEditorFlags>\n"
	foreach flag $eds {
	    append document "<cheshireFlag>$flag</cheshireFlag>\n"
	}
	append document "</cheshireEditorFlags>\n"
    }

append document "\
</cheshireAccess>\n \
<cheshireHistory>\n\
"

    set creation [lindex [lindex $cdi 4] 0]
    if {[llength $creation] == 0} {
	set creation [list "" $REMOTE_USER [clock format [clock seconds]]]
    }

    append document "<cheshireCreated>\
<cheshireUser>[lindex $creation 1]</cheshireUser>\
<cheshireDate>[lindex $creation 2]</cheshireDate>\
</cheshireCreated>\n\
"
    foreach mod [lrange [lindex $cdi 4] 1 end] {
	append document "<cheshireModified type=\"[lindex $mod 1]\">\
<cheshireUser>[lindex $mod 2]</cheshireUser>\
<cheshireDate>[lindex $mod 3]</cheshireDate>\
</cheshireModified>\n\
"
    }

    # Add current modification, if we have one

    if {$type != ""} {

	append document "<cheshireModified type=\"$type\">\
<cheshireUser>$REMOTE_USER</cheshireUser>\
 <cheshireDate>[clock format [clock seconds]]</cheshireDate>\
     </cheshireModified>\n\
    " 
    }

    append document "\
    </cheshireHistory>\n\
    \n \ 
    <cheshireDocument>\n\
    "
    
    return $document
    
}

proc write_record { key doc } {
    global CHESHIRE_ROOT_DATA CGI_ROOT current_document_info

    cd "$CHESHIRE_ROOT_DATA"
    set fname "[lindex $current_document_info 1]-[clock seconds].sgml"
    set outh [open $fname w]
    puts $outh $doc
    close $outh

    # Always end in root directory
    cd "$CGI_ROOT"

}

proc validate_and_save {record key title} {
    global indata CHESHIRE_ROOT_DATA CHESHIRE_ROOT CHESHIRE_PATH CGI_ROOT REBUILD_SCRIPT_DATA REBUILD_LEVEL_DATA current_document_info
    
    set is_valid [validate_record $record]
    
    if { $is_valid == 1 } {
	# Add record to main system here
	# New name is:  $key-(time).sgml
	
	set fname "$key-[clock seconds].sgml"
	set newdir "$CHESHIRE_ROOT/verification/[pid]"
	file copy "$newdir/testrecord.sgml" "$CHESHIRE_ROOT_DATA/$fname"
	file delete -force $newdir

	if { $REBUILD_LEVEL_DATA == 2 } {
	    set err [catch {exec $REBUILD_SCRIPT_DATA} results]
	} elseif { $REBUILD_LEVEL_DATA == 1} {
	    set md [get_max_docid "data"]
	    set err [catch {exec $REBUILD_SCRIPT_DATA [expr $md +1] } results]
	} else {
	    # We don't want to rebuild at all (apparently)
	    set err 0
	}

	
	if {$err} {
	    
	    regsub -all "\n" $results "<br>" results
	    
	    throwfile "err.html" [list "\%ERR\% Could not rebuild the database! The system needs manual rebuild immediately, contact an administrator.<p>Errors given:<p>$results</p>"]
	} else {
	    set location [lindex $current_document_info 2]
	    throwfile "save.html" [list "\%KEY\% $key" "\%TITLE\% $title" "\%LOCATION\% $location"]
	}
	
    } elseif { $is_valid == -1 } {
	throwfile "err.html" [list "\%ERR\% Could not create a directory to verify your record in. Please try again."]
    } else {
	# Need to send back up to edit/create page
	return $is_valid
    }
    return 0

}


proc validate_record {record } {
    global indata CHESHIRE_ROOT CHESHIRE_PATH CGI_ROOT  current_document_info

    set newdir "$CHESHIRE_ROOT/verification/[pid]"
    set err [catch "file mkdir $newdir" results]
    if {$err} {
	return -1
    } else {
	cd $newdir
	set outh [open "testrecord.sgml" w]
	puts $outh $record
	close $outh

	# Create dbconfigfile... just set base and append the rest.

	set outh [open "DBCONFIGFILE" w]
	puts $outh "<DBCONFIG>\n \
 <DBENV>/home/cheshire/cheshire/default</DBENV>\n \
 <FILEDEF TYPE=SGML>\n \
 <DEFAULTPATH> $newdir </DEFAULTPATH>\n \
"	
	set inh [open "../DBCONFIGFILE.tmpl"]
	puts $outh [read $inh [file size "../DBCONFIGFILE.tmpl"]]
	close $inh
	close $outh

	exec "$CHESHIRE_PATH/buildassoc" testrecord.sgml  verify.assoc CheshireRecord
	set err [catch {exec "$CHESHIRE_PATH/index_cheshire" -b DBCONFIGFILE} errs]
	
	# Now in theory we've built for just this record.

	set loglines {}

	set logh [open "INDEX_LOGFILE"]
	while {[gets $logh line] >= 0} {
	    lappend loglines $line
	}

	set lastline [lindex $loglines [expr [llength $loglines] -1]]
	cd $CGI_ROOT
	
	if {[string range $lastline 0 9] == " -- Memory"} {
	    return 1
	} else {
	    set reason [lindex $loglines [expr [llength $loglines] -2]]
	    file delete -force $newdir
	    return $reason
	}
    }
}

proc move_deleted_files {key docid} {
    global CHESHIRE_ROOT DELETED_FILES_PATH_DATA global_variables
    foreach var $global_variables {
	global $var
    }

    # Find and move all files associated with $key
    # We need a current full list of files, so build just the pkeys.
    exec "$CHESHIRE_ROOT/buildnew.sh-deletehtml"
    set INH [open "$CHESHIRE_ROOT/logs/deleteINDEX_LOGFILE"]
    while {[gets $INH line] >= 0} {
	if {[string range $line 0 3] != "SORT"} {
	    lappend loglines $line
	}
    }
    close $INH
    
    set found [list -1]
    set llen [llength $loglines]
    for { set idx 0 } {$idx < $llen} {incr idx} {
	set line [lindex $loglines $idx]
	if {$line == "Duplicate primary key: $key"} {
	    incr idx
	    set line [lindex $loglines $idx]
	    regexp {^Record number ([0-9]+) will [A-Z]* ([0-9]+)} $line null first second
	    lappend found $first
	    lappend found $second
	}
    }
    
    set found [lsort -integer $found]
    
    set INH [open "$CHESHIRE_ROOT/DATA.cont"]
    while {[gets $INH line] >= 0} {
	lappend contlines $line
    }
    close $INH
    
    # Data.cont only contains the files, one per line. We can abuse this fact.
    set files [list]
    set flen [llength $found]
    for { set idx 1} {$idx < $flen} {incr idx} {
	set ln [lindex $found $idx]
	if {  [lindex $found [expr $idx -1 ]] == $ln } {
	    # deduplicated
	} else {
	    set  fileline [lindex $contlines [expr $ln -1 ]]
	    regexp {^<FILECONT .* MAX=[0-9]+> (.+) </FILECONT>$} $fileline null filename
	    lappend files $filename
	}
    }

    # Now we have a list of files. If 0 then just the current one. (need a metadata query)

    if {[llength $files] == 0 } {
	set CHESHIRE_DATABASE METADATA
	set CHESHIRE_NUMREQUESTED 1
	set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_DATA
	set results [search filename $CHESHIRE_DATABASE_DATA $docid]
	set files [lindex $results 1]
    }

    # Now move them to the deleted file repository.
    foreach fn $files {
	exec mv $fn $DELETED_FILES_PATH_DATA
    }

    return 1
}


proc rebuild_record_database {} {
    global REBUILD_SCRIPT_DATA REBUILD_LEVEL_DATA

    if { $REBUILD_LEVEL_DATA  == 2 } {
	set err [catch {exec $REBUILD_SCRIPT_DATA} results]
    } elseif { $REBUILD_LEVEL_DATA == 1 } {
	set md [get_max_docid "data"]
	set err [catch {exec $REBUILD_SCRIPT_DATA [expr $md +1] } results]
    } else {
	# No rebuilding at all
	set err 0
    }


    if {$err} {
	regsub -all "\n" $results "<br>" results
	throwfile "err.html" [list "\%ERR\% Could not rebuild the database! The system needs manual rebuild immediately, contact an administrator.<p>Errors given:<p>$results</p>"]
	return 0
    } else {
	return 1
    }

}
