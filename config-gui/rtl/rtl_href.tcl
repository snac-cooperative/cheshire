# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: Href support
# -----------------------------------------------------------------------
package provide rtl_href 1.2;

# ---------------------------------------------------------------------------
# Function    : findWidget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : find widget which is currently under the cursor
# ---------------------------------------------------------------------------
proc findWidget {w rx ry} {
    set rw [winfo containing -displayof $w $rx $ry];
    popupHref $rw;
}

# ---------------------------------------------------------------------------
# Function    : showHref - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Call a new internet browser with the given page.
#               startup point is the html directory under GIPSY
# ---------------------------------------------------------------------------
proc showHref {args} {
    variable ::env;
    variable ::tcl_platform;

    if {[info exists env(HTMLHOME)]} {
	set htmdir $env(HTMLHOME);
    } elseif {[info exists env(GHOME)]} {
	set htmdir [file join $env(GHOME) html];
    } else {
	set htmdir [file join $env(VGHOME) html];
    }

    set href [::eval {file join} [linsert $args 0 $htmdir]];

    if {![info exists env(GHTMLVIEWER)]} {
	if {![info exists env(HTMLBROWSER)]} {
	    askForProg "Html Viewer"\
		    netscape "exec %s $href &"\
		    "set env(GHTMLVIEWER) %s";
	} else {
	    set env(GHTMLVIEWER) $env(HTMLBROWSER);
	}
    } else {
	# under unix launch netscape with -remot openURL
        # don't do the same under any other platform or for an
        # other browser ...
	if {[string first "netscape" $env(GHTMLVIEWER)] > -1 \
		&& [string compare "windows" $tcl_platform(platform)] != 0} {
	    if {[catch {
		exec [string trim $env(GHTMLVIEWER) \"]\
			-remote openURL(file:$href);
	    }]} {
		exec [string trim $env(GHTMLVIEWER) \"] file:$href &
	    }
	} else {
	    exec [string trim $env(GHTMLVIEWER) \"] $href &
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : showVGHref - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : VisualGIPSY help procedure used to find the
#               VisualGIPSY html pages. It will be assumed that
#               the vghome directory contains an subdirectory html!
# ---------------------------------------------------------------------------
proc showVGHref {args} {
    variable ::env;
    variable ::tcl_platform;

    set htmdir [file join $env(VGHOME) html];
    set href [::eval {file join} [linsert $args 0 $htmdir]];

    # Use the GHOME directory before VGHOME directory
    if {[info exists env(GHOME)]} {
	set ghtmdir [file join $env(GHOME) html];
	if {[file exists $ghtmdir]} {
	    set href [::eval {file join} [linsert $args 0 $ghtmdir]];
	}
    }

    if {![info exists env(GHTMLVIEWER)]} {
	if {![info exists env(HTMLBROWSER)]} {
	    askForProg "Html Viewer"\
		    netscape "exec %s $href &"\
		    "set env(GHTMLVIEWER) %s";
	} else {
	    set env(GHTMLVIEWER) $env(HTMLBROWSER);
	}
    } else {
	# under unix launch netscape with -remot openURL
        # don't do the same under any other platform or for an
        # other browser ...
	if {[string first "netscape" $env(GHTMLVIEWER)] > -1 \
		&& [string compare "windows" $tcl_platform(platform)] != 0} {
	    if {[catch {
		exec [string trim $env(GHTMLVIEWER) \"]\
			-remote openURL(file:$href);
	    }]} {
		exec [string trim $env(GHTMLVIEWER) \"] file:$href &
	    }
	} else {
	    exec [string trim $env(GHTMLVIEWER) \"] $href &
	}
    }
}


# ---------------------------------------------------------------------------
# Function    : popupHref - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : popup page as callback for F1.
# ---------------------------------------------------------------------------
proc popupHref {w} {
   set tmp [opget $w href ""];
   if {[string length $tmp] > 4} {
       if {[string compare "macintosh"\
	       $::tcl_platform(platform)] == 0} {
	   ::eval showHref [join [split $tmp] :];
       } else {
	   ::eval showHref [join [split $tmp] /];
       }
   }
}


# ---------------------------------------------------------------------------
# Function    : askForProg - 
# 
# Returns     : -
# Parameters  : label      - caption of dialog
#               cmdline    - default argument (interacitve modifyable)
#               script     - command to perform
#               pastscript - on success
# 
# Description : New modified version must be usable inside of the rtl_href
# ---------------------------------------------------------------------------
proc askForProg {label cmdline script pastscript} {

    catch {unset ::rat(cmdline,ok)}
    set ::rat(cmdline) $cmdline;
    
    # GUI
    set ask .ask;
    toplevel $ask;

    grid [entry $ask.e \
	    -textvariable rat(cmdline)]\
	    -column 0 -row 0 -columnspan 3 -sticky ews;
    grid [button $ask.ok -text "ok" -command "set ::rat(cmdline,ok) 1;destroy $ask"]\
	    -column 1 -row 1 -sticky news;
    grid [button $ask.cancel -text "cancel" -command "destroy $ask"]\
	    -column 2 -row 1 -sticky news;
    wm title $ask $label;
    bind $ask.e <Return> "set ::rat(cmdline,ok) 1;destroy $ask";

    grid rowconfigure $ask 0 -weight 1;
    grid columnconfigure $ask 0 -weight 1;
    grid columnconfigure $ask 1 -minsize 40;
    grid columnconfigure $ask 2 -minsize 40;

    # process
    tkwait window $ask;

    if {[info exists ::rat(cmdline,ok)]} {
	if { [catch {
	    eval [format $script $::rat(cmdline)];
	} result]} {
	    error $result
	} else {
	    # posteval
	    eval [format $pastscript $::rat(cmdline)];
	}
	catch {
	    unset ::rat(cmdline);
	    unset ::rat(cmdline,ok);
	}
    }
}

