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
    puts "buildassoc called";
}

proc CallIndexcheshire {} {
    global cmdArray;
    puts "indexcheshire called";
}

proc CallIndexclusters {} {   
    global cmdArray;
    puts "indexclusters called";
}



