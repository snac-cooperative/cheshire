
# Note: By this time, the permissions of REMOTE_USER have already been checked.
# No need to verify if the user can do the function.
# On /both/ calls to a function, if relevant.

# ---- Admin Functions ----

proc function_adminselect {} {
    global REMOTE_USER
    throwfile "adminselect.html" [list "\%USER\% $REMOTE_USER"]
}

proc function_rebuild {} {
    global REMOTE_USER indata
    if { ! [maybe_throw_reqform "" "System"] } {
	set database $indata(arg1)

	if {$database == "user"} {
	    if { [rebuild_user_database] } {
		write_htaccess
		throw_success_file
	    }
	    
	} elseif {$database == "data"} {
	    if { [rebuild_record_database] } {
		throw_success_file
	    }

	} else {
	    throw_error_reqform "" "System" "You need to give either 'user' or 'data' as the database to rebuild."
	}
    }
}

proc function_adduser {} {
    global indata current_user_info

    if { [lsearch [array names indata] "name"] == -1} {
	throwfile "newuser.html" [list "\%ERR\% " "\%NAME\% " "\%PASSWORD\% " "\%REALNAME\% " "\%EMAIL\% " "\%INFO\% " "\%FLAGS\% " "\%HOSTMASK\% "]
    } else {
	
	set name $indata(name)
	set password $indata(password)
	set info $indata(info)
	set flags $indata(flags)
	set realname $indata(realname)
	set email $indata(email)
	set hostmask $indata(hostmask)

	# Do some sanity checking.

	if {[get_user_info $name] != 0} {
	    throwfile "newuser.html" [list "\%ERR\% A user with that user name already exists." "\%NAME\% " "\%PASSWORD\% " "\%REALNAME\% " "\%EMAIL\% " "\%INFO\% " "\%FLAGS\% " "\%HOSTMASK\% "]
	} else {
	    set_user_info "name" $name
	    set_user_info "password" $password
	    set_user_info "info" $info
	    set_user_info "realname" $realname
	    set_user_info "email" $email
	    set_user_info "hostmask" $hostmask
	    set current_user_info [lreplace $current_user_info 2 2 [list ]]
	    foreach f $flags {
		set_user_info "addflag" $f
	    }
	    
	    set record [generate_user_record]
	    write_user_record $name $record
	    if { [set err [rebuild_user_database]] != 0 } {
		write_htaccess
		throw_success_file
	    }
	}
    }
}

proc function_deluser {} {
    global indata CHESHIRE_PATH CHESHIRE_ROOT CHESHIRE_CONFIGFILE_USERS CHESHIRE_DATABASE_USERS CGI_ROOT CHESHIRE_ELEMENTSET REMOTE_USER

    if {! [maybe_throw_reqform "" "System"] } {
	# Do delete for user as per record
	set who $indata(arg1)
	set docid [get_user_docid $who]

	if {$docid == 0} {
	    throw_error_reqform "" "System" "$who is not a known user."
	} else {

	    # Move to database dir to avoid leaving log files in weird places.
	    cd $CHESHIRE_ROOT

	    exec "$CHESHIRE_PATH/delete_recs" -L logs/userDELETERECS_LOGFILE $CHESHIRE_CONFIGFILE_USERS $CHESHIRE_DATABASE_USERS $docid

	    move_deleted_user $who $docid

	    #Record deleted ID, cheshireID, time, user
	    set outh [open "logs/deleted_userdocids.log" a]
	    puts $outh "$docid:$who:[clock seconds]:$REMOTE_USER"
	    close $outh

	    write_htaccess

	    cd $CGI_ROOT
	    throw_success_file

	}
    }
}

proc function_setpassword {} {
    global indata current_user_info

    if {! [ maybe_throw_reqform "" "System"] } {
	set who $indata(arg1)
	set pass $indata(arg2)

	set old_info $current_user_info
	set current_user_info [get_user_info $who]

	if {$current_user_info == 0} {
	    throw_error_reqform "" "System" "$who is not a known user."
	} else {
	    set_user_info "password" $pass

	    set record [generate_user_record]
	    write_user_record $who $record
	    if { [set err [rebuild_user_database]] != 0 } {
		write_htaccess
		throw_success_file
	    }
	}
    }
}

proc function_setinfo {} {
    global indata current_user_info

    if {![maybe_throw_reqform "" "System"] } {
	set who $indata(arg1)
	set info $indata(arg2)
	
	set old_info $current_user_info
	set current_user_info [get_user_info $who]
	
	if {$current_user_info == 0} {
	    throw_error_reqform "" "System" "$who is not a known user."
	} else {
	    set_user_info "info" $info

	    set record [generate_user_record]
	    write_user_record $who $record
	    if { [set err [rebuild_user_database]] != 0 } {
		throw_success_file
	    }
	}
    }
}

proc function_setflags {} {
    global indata current_user_info

    if {![maybe_throw_reqform "" "System"] } {
	set who $indata(arg1)
	set flags $indata(arg2)

	set old_info $current_user_info
	set current_user_info [get_user_info $who]

	if {$current_user_info == 0} {
	    throw_error_reqform "" "System" "$who is not a known user."
	} else {
	    set_user_info "flags" $info

	    set record [generate_user_record]
	    write_user_record $who $record
	    if { [ set err [rebuild_user_database]] != 0 } {
		throw_success_file
	    }
	}
    }
}
