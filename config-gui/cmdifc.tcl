#--------------------------------------------------------------------------
# the following commands deal with the switching between interfaces
# from the cmd window to either dbconfig or serverconfig window as
# well as the actual building up of the commands and excuting them.
#--------------------------------------------------------------------------
proc OpenCmdInterface {cmdname} {
    global cmdArray;
    cmd;
    if {$cmdname == "build"} {
	.cmd.cmdtabset setselection build;
    } elseif {$cmdname == "indexche"} {
	.cmd.cmdtabset setselection indexche;
    } elseif {$cmdname == "indexclu"} {
	.cmd.cmdtabset setselection indexclu;
    }

    wm title .cmd "Staffcheshire -- buildassoc -- index_cheshire --\
	    index_clusters";
    wm geometry . 700x600+50+50;
    wm withdraw .;
    set cmdArray(singleormulti) single;
    set cmdArray(nameoptional) "(Optional)";
    wm protocol .cmd WM_DELETE_WINDOW {exit};
}

proc CmdToDb {} {
    global cmdArray;
    destroy .cmd;
    unset cmdArray;
    OpenDbInterface;
}

proc CmdToServer {} {
    global cmdArray;
    destroy .cmd;
    unset cmdArray;
    # just in case these two files haven't been sourced
    source server.tcl;
    source serverifc.tcl;
    OpenServerInterface;
}

proc CmdClose {} {
    exit 0;
}


proc BuildSingle {} {
    global cmdArray;
    set cmdArray(nameoptional) "(Optional)";
    set bb [find-widget buildassocfpBB .];
    $bb configure -command "HandleBBSelect $bb";
}

proc BuildMultiple {} {
    global cmdArray;
    set cmdArray(nameoptional) "(Required)";
    set bb [find-widget buildassocfpBB .];
    $bb configure -command "fileselect $bb HandleBBSelect";
}

proc CallBuildassoc {} {
    global cmdArray;
    if {$cmdArray(singleormulti) == "multiple"} {
	if {$cmdArray(build,name) == ""} {
	    tk_messageBox -type ok -message "Please enter a name\
		    for the associator file" -icon error;
	    return;
	} elseif {$cmdArray(build,path) == ""} {
	    tk_messageBox -type ok -message "Please enter the path\
		    to the top level directory" -icon error;
	    return;
	} elseif {$cmdArray(build,tag) == ""} {
	    tk_messageBox -type ok -message "Please enter the top\
		    level tag defining a record" -icon error;
	    return;
        }
	set cmd [list buildassoc -q -r $cmdArray(build,path)\
		$cmdArray(build,name) $cmdArray(build,tag)];
	#puts $cmd;
    }

    if {$cmdArray(singleormulti) == "single"} {
	if {$cmdArray(build,path) == ""} {
	    tk_messageBox -type ok -message "Please enter the path\
		    to the sgml file" -icon error;
	    return;
	} elseif {$cmdArray(build,tag) == ""} {
	    tk_messageBox -type ok -message "Please enter the top\
		    level tag defining a record" -icon error;
	    return;
        }
	if {$cmdArray(build,name) == ""} {
	    set cmd [list buildassoc -q $cmdArray(build,path)\
		    $cmdArray(build,tag)];
	} else {
	    set cmd [list buildassoc -q $cmdArray(build,path)\
		    $cmdArray(build,name) $cmdArray(build,tag)];
	}
	#puts $cmd;
    }
    if [catch {exec $cmd} result] {
	tk_messageBox -type ok -message $result -icon error;
    }    
}

proc CallIndexcheshire {} {
    global cmdArray;
    if {$cmdArray(indexche,cf) == ""} {
	tk_messageBox -type ok -message "Please enter the path\
		to the configuration file" -icon error;
	return;
    }
    if {$cmdArray(indexche,start) == "" &&\
	    $cmdArray(indexche,last) != ""} {
	tk_messageBox -type ok -message "Please enter the logical\
		number of the first record as well" -icon error;
	return;
    }
    if {$cmdArray(indexche,dir) != ""} {
	set cmd [list index_cheshire -b -T $cmdArray(indexche,dir)\
		$cmdArray(indexche,cf)];
    } else {
	set cmd [list index_cheshire -b $cmdArray(indexche,cf)];
    }

    if {$cmdArray(indexche,start) != ""} {
	lappend cmd $cmdArray(indexche,start);
    } 
    if {$cmdArray(indexche,last) != ""} {
	lappend cmd $cmdArray(indexche,last);
    }
    #puts $cmd;
    if [catch {exec $cmd} result] {
	tk_messageBox -type ok -message $result -icon error;
    }
}

proc CallIndexclusters {} {   
    global cmdArray;
    if {$cmdArray(indexclu,cf) == ""} {
	tk_messageBox -type ok -message "Please enter the path\
		to the configuration file" -icon error;
	return;
    }
    
    if {$cmdArray(indexclu,dir) != ""} {
	set cmd [list index_clusters -b -T $cmdArray(indexclu,dir)\
		$cmdArray(indexclu,cf)];
    } else {
	set cmd [list index_clusters -b $cmdArray(indexclu,cf)];
    }
    
    if {$cmdArray(indexonly) == 1} {
	lappend cmd INDEXONLY;
    }
    #puts $cmd;
    if [catch {exec $cmd} result] {
	tk_messageBox -type ok -message $result -icon error;
    }
}
