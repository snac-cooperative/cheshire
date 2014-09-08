
# This library contains the functions needed for maintaining the user database.
# The official name for a person is a 'user' not an editor
# (As it's conceivable that they won't be able to edit anything)

# Procedures in this library:

# function_useredit -> User interface to changing own data
# function_usersave -> User interface to saving own changed data
# display_user_info <who> -> Display info for 'who'
# user_has_flag <flag> -> Does current user have given flag
# check_user_password <password> -> Does password match current user
# user_can_edit -> Can current user edit current document
#           XXX (Should this be a document function ? )
# get_user_info -> return information about user
# set_user_info -> set current user's info (locally)
# generate_user_record -> generate sgml for current user's record
# write_user_record -> write user sgml to disk
# write_htaccess -> write htaccess file to disk from current user db
# rebuild_user_database -> rebuild user database (what can I say?)


# ---- User Functions ----

proc function_useredit {} {
    global REMOTE_USER current_user_info
    
    set realname [lindex $current_user_info 3]
    set email [lindex $current_user_info 4]
    set hostmask [lindex $current_user_info 5]
    
    throwfile "useredit.html" [list "\%USER\% $REMOTE_USER" "\%REALNAME\% $realname" "\%EMAIL\% $email" "\%HOSTMASK\% $hostmask" "\%ERR\% "]

}

proc function_usersave {} {
    global indata current_user_info REMOTE_USER
    set cei $current_user_info

    set newname $indata(realname)
    set newemail $indata(email)
    set newhm $indata(hostmask)
    set opass $indata(passwordo)
    set npassa $indata(passworda)
    set npassb $indata(passwordb)

    set changed [list]

    if {$opass != "" || $npassa != "" || $npassb != "" } {
	set err ""
	if { [check_user_password $opass] == 0 } {
 	    set err "The password supplied did not match your current one! Please try again."
	} elseif { $npassa != $npassb } {
	    set err "The new passwords supplied did not match! Please try again."
	} elseif { $npassa == $newname || $npassa == $REMOTE_USER || $npassa == $newemail } {
	    # Extremely basic sanity checking
	    set err "The new password supplied was one of your public values! Please try again with a more difficult to guess password."
	} elseif { [check_user_password $npassa] == 1 } {
	    set err "The new password supplied was the same as your current password! Please try again."
	} elseif {[string length $npassa] < 6 } {
	    set err "The new password supplied was less than the minimum 6 characters in length! Please try again."
	}
	if {$err != "" } {
	    throwfile "useredit.html" [list "\%USER\% $REMOTE_USER" "\%REALNAME\% $newname" "\%EMAIL\% $newemail" "\%HOSTMASK\% $newhm" "\%ERR\% $err"]
	    exit 
	} else {
	    set_user_info "password" $npassa
	    lappend changed "Password"
	}
    }
	    
    if { $newname != [lindex $cei 3] } {
	# No sanity checking
	set_user_info "realname" $newname
	lappend changed "Real Name"
    }
    if { $newemail != [lindex $cei 4] } {
	# No sanity checking (so you can put a message)
	set_user_info "email" $newemail
	lappend changed "Email Address"
    }
    if { $newhm != [lindex $cei 5] } {
	# No sanity checking
	set_user_info "hostmask" $newhm
	lappend changed "Hostmask"
    }

    if {[llength $changed] == 0} {
	set err "No fields were different from their current values. Please try again."
	throwfile "useredit.html" [list "\%USER\% $REMOTE_USER" "\%REALNAME\% $newname" "\%EMAIL\% $newemail" "\%HOSTMASK\% $newhm" "\%ERR\% $err"]
    } else {
	set record [generate_user_record]
	write_user_record $REMOTE_USER $record
	if { [set err [rebuild_user_database]] != 0 } {
	    #all is well. Throw success.
	    set changetxt ""
	    foreach c $changed {
		append changetxt "$c, "
	    }
	    set changetxt [string range $changetxt 0 [expr [string length $changetxt] - 3]]

	    if {[lsearch $changed "Password"] != -1 } {
		# Need to update access file
		write_htaccess
	    }

	    throwfile "usersave.html" [list "\%USER\% $REMOTE_USER" "\%CHANGED\% $changetxt"]
	}
    }
}

proc display_user_info {who} {
    global REMOTE_USER global_variables
    foreach var $global_variables {
	global $var
    }

    # NB: At this point REMOTE_USER is not necessarily valid.


    set results [get_user_info $who]
    if {$results == 0} {
	#Couldn't retreive info with that name
    } else {
	if {$REMOTE_USER != ""} {
	    # We can maybe display 'sensitive' information
	    set other "<tr><td colspan = 2>Here be sensitive information.</td></tr>"
	} else {
	    set other ""
	}

	throwfile "userdisplay.html" [list "\%USER\% [lindex $results 0]" "\%REALNAME\% [lindex $results 3]" "\%EMAIL\% [lindex $results 4]" "\%INFO\% [lindex $results 6]" "\%OTHER\% $other" "\%FLAGS\% [lindex $results 2]"]

    }
}




# ------      Information functions       ------

# Check if current_editor has given flag
proc user_has_flag { flag } {
    global current_user_info
    set flags [lindex $current_user_info 2]
    # If we're an ADMIN then we get all flags by default
    if {  [lsearch $flags $flag] != -1 || [lsearch $flags "admin"] != -1 } {
	return 1
    } else {
	return 0
    }
}

# Check password matches
proc check_user_password { password } {
    global current_user_info
    set crypted [lindex $current_user_info 1]

    if { [crypt $password [string range $crypted 0 1]] == $crypted } {
	return 1
    } else {
	return 0
    }
}

# Check if user can edit file, based on editors and flags
proc user_can_edit {} {
    global current_user_info current_document_info
    set user [lindex $current_user_info 0]
    
    if { [user_has_flag globaleditor] } {
	return 1
    } elseif {[lsearch [lindex [lindex $current_document_info 2 ] 0] $user ] != -1} {
	return 1
    } else {
	# Step over user flags vs document flags
	foreach flag [lindex $current_user_info 2] {
	    if { [lsearch [lindex [lindex $current_document_info 2] 1] $flag ] != -1} {
		return 1
	    }
	}
    }
    return 0
}

# Check hostmask
proc check_hostmask {} {
    global current_user_info REMOTE_HOST REMOTE_ADDR

    set hostmask [lindex $current_user_info 5]
    if {[llength $hostmask] == 1 && $hostmask == "elementEmpty"} {
	return 1
    } else {
	set ralen [string length $REMOTE_ADDR]
	set rhlen [string length $REMOTE_HOST]

	foreach host $hostmask {
	    # Get rid of ; , *
	    regsub -all {[*;,]} $host "" host
	    set hlen [string length $host]
	    
	    if {$REMOTE_ADDR == $host} {
		return 1
	    } elseif {[string index $host [expr $hlen -1]] == "." && $hlen < $ralen && [string range $REMOTE_ADDR 0 [expr $hlen -1]] == $host } {
		return 1
	    } elseif {[string index $host [expr $hlen -1]] == "." && $hlen < $rhlen && [string range $REMOTE_HOST 0 [expr $hlen -1]] == $host } {
		return 1
	    } elseif {[string index $host 0] == "." && $hlen < $ralen && [string range $REMOTE_ADDR [expr $ralen - $hlen]   end] == $host } {
		return 1
	    } elseif {[string index $host 0] == "." && $hlen < $rhlen && [string range $REMOTE_HOST [expr $rhlen  - $hlen]   end] == $host } {
		return 1
	    }    
	}
	return 0;
    }
}


# ------       Retrieval functions      ------

# Return formatted data given name of user
proc get_user_info { name } {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_USERS
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
    
    set query "search ( cheshireUser {$name} ) "

    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]

    if {$err != 0 || $hits != 1} {
	return 0
    }

    set sresults [lrange $qresults 1 end]
    set fresults [flatten_user_metagrs $sresults]
    return $fresults
}

proc flatten_user_metagrs {sresults} {
    set fresults {}
    foreach res [lindex $sresults 0] {
	if {$res != "GenericRecord::"} {
	    set gtype [lindex [lindex [lindex $res 0 ] 1 ] 2]
	    set contents [lindex [lindex $res 1] 1]
	    if {$gtype == "publicationPlace"} {
		set flags {}
		foreach ed $contents {
		    set edtype [lindex [lindex [ lindex [lindex $ed 1] 0] 1] 1]
		    set edtype [string range $edtype 8 [expr [string length $edtype] -1]]
		    set edtype [string tolower $edtype]
		    set edcont [lindex [lindex [lindex $ed 1] 1 ] 1]
		    lappend flags $edcont
		}
		lappend fresults $flags
	    } else {
		lappend fresults $contents
	    }
	}
    }
    return $fresults
}


proc get_user_info_by_id { name } {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_USERS
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
    
    set query "search ( docid {$name} ) "

    set err [catch {eval $query} qresults]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]

    if {$err != 0 || $hits != 1} {
	return 0
    }

    set sresults [lrange $qresults 1 end]
    set fresults [flatten_user_metagrs $sresults]
    return $fresults
}

proc get_user_docid {key} {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    set CHESHIRE_ELEMENTSET DOCIDGRS
    set CHESHIRE_DATABASE $CHESHIRE_DATABASE_USERS
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
    set query "search ( cheshireUser {$key} )"
    
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


# -----------    File generation functions (etc) --------------

proc set_user_info {type value} {
    global current_user_info
    set cei $current_user_info

    if {[llength $cei] == 0} {
	set current_user_info [list "" "" "" "" "" "" ""]
	set cei $current_user_info
    }

    switch $type {
	"name" {
	    set current_user_info [lreplace $cei 0 0 $value]
	}
	"password" {
	    set crypted [crypt $value]
	    set current_user_info [lreplace $cei 1 1 $crypted]
	}
	"realname" {
	    set current_user_info [lreplace $cei 3 3 $value]
	}
	"email" {
	    set current_user_info [lreplace $cei 4 4 $value]
	}	    
	"hostmask" {
	    set current_user_info [lreplace $cei 5 5 $value]
	}
	"info" {
	    set current_user_info [lreplace $cei 6 6 $value]
	}
	"addflag" {
	    set newflags [lappend [lindex $cei 2] $value]
	    set current_user_info [lreplace $cei 2 2 $newflags]
	}
	"delflag" {
	    set idx [lsearch [lindex $cei 2] $value]
	    set newflags [lreplace [lindex $cei 2] $idx $idx]
	    set current_user_info [lreplace $cei 2 2 $newflags]
	}
    }
}

proc generate_user_record {} {
    global current_user_info
    set cei $current_user_info

    # To set someone else's data, need to put it into cei. 
    # This is suboptimal, but not critical so long as we
    # restore old values afterwards.

set record "<cheshireEditor>\n \
  <cheshireUser>[lindex $cei 0]</cheshireUser>\n \
  <cheshirePassword>[lindex $cei 1]</cheshirePassword>\n \
  <cheshireFlags>\n"

    foreach flag [lindex $cei 2] {
	append record "    <cheshireFlag>$flag</cheshireFlag>\n"
    }

    append record "  </cheshireFlags>\n \
  <cheshireRealName>[lindex $cei 3]</cheshireRealName>\n \
  <cheshireEmail>[lindex $cei 4]</cheshireEmail>\n \
  <cheshireHostMask>[lindex $cei 5]</cheshireHostMask>\n \
  <cheshireInfo>[lindex $cei 6]</cheshireInfo>\n</cheshireEditor>\n"

    return $record

}

proc write_user_record {who doc} {
    global CHESHIRE_ROOT_USERS CGI_ROOT

    cd "$CHESHIRE_ROOT_USERS"
    set fname "$who-[clock seconds].sgml"
    set outh [open $fname w]
    puts $outh $doc
    close $outh

    # Always end in root directory
    cd "$CGI_ROOT"

}

proc write_htaccess {} {
    global HTACCESS_FILENAME current_user_info global_variables
    foreach var $global_variables {
	global $var
    }
    # We need to call this every time that someone changes their password
    # Or when we add a new editor to the system.

    set CHESHIRE_DATABASE METADATA
    set CHESHIRE_NUMREQUESTED 1
    set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
    set results [search maxdocid $CHESHIRE_DATABASE_USERS]
    set max_docid [lindex $results 1]

    set htfilehandle [open $HTACCESS_FILENAME w]

    for { set curr 1 } {$curr <= $max_docid} { incr curr } {
	set current_user_info [get_user_info_by_id $curr]
	set name [lindex $current_user_info 0]
	set password [lindex $current_user_info 1]

	# Don't write non existent docids
	if {$name != 0} {
	    puts $htfilehandle "$name:$password"
	}
    }

    close $htfilehandle
    return 1
}

proc rebuild_user_database {} {
    global REBUILD_SCRIPT_USERS REBUILD_LEVEL_USERS

  if { $REBUILD_LEVEL_USERS  == 2 } {
	set err [catch {exec $REBUILD_SCRIPT_USERS} results]
    } elseif { $REBUILD_LEVEL_USERS == 1 } {
	set md [get_max_docid "user"]
	set err [catch {exec $REBUILD_SCRIPT_USERS [expr $md +1] } results]
    } else {
	# No rebuilding at all
	set err 0
    }

    if {$err} {
	regsub -all "\n" $results "<br>" results
	puts "$results"
	throwfile "err.html" [list "\%ERR\% [format $E_REBUILD $results]"]
	return 0
    } else {
	return 1
    }
}

proc move_deleted_user {key docid} {
    global CHESHIRE_ROOT DELETED_FILES_PATH_USERS global_variables
    foreach var $global_variables {
	global $var
    }

    # Find and move all files associated with $key
    # We need a current full list of files, so build just the pkeys.
    exec "$CHESHIRE_ROOT/buildnew.sh-deleteuser"
    set INH [open "$CHESHIRE_ROOT/logs/deleteuserINDEX_LOGFILE"]
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
    
    set INH [open "$CHESHIRE_ROOT/users.cont"]
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
	set CHESHIRE_CONFIGFILE $CHESHIRE_CONFIGFILE_USERS
	set results [search filename $CHESHIRE_DATABASE_USERS $docid]
	set files [lindex $results 1]
    }

    # Now move them to the deleted file repository.
    foreach fn $files {
	exec mv $fn $DELETED_FILES_PATH_USERS
    }

    return 1
}
