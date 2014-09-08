# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: Misc Registry functions used to find IB and Editors
#              under NT, this is used inside off VisualGIPSY

# -----------------------------------------------------------------------

if {$tcl_platform(platform) == "windows"} {

    # ---------------------------------------------------------------------------
    # Function    : registry-find-exe - 
    # 
    # Returns     : associated program for extension
    # Parameters  : file   - documentfile with extension
    #               action - shell command (see context menu in explorer).
    # 
    # Description : 
    # ---------------------------------------------------------------------------
    proc registry-find-exe {file {action open}} {
	variable ::env;

	set executable "";
	set ext [file extension $file];
	if {[string length $ext] < 2} {
	    error "No valid extension $ext contained in $file";
	} else {
	    set reg1 [registry get HKEY_CLASSES_ROOT\\$ext ""];
	    if {[string length $reg1] > 0} {
		set reg2 [registry get\
			HKEY_CLASSES_ROOT\\$reg1\\shell\\$action\\command ""];
		set l_exe [split $reg2 \"];
		foreach item $l_exe {
		    
		    if {[string length $item] > 1} {
			set executable $item;
			break;
		    }
		}
	    }
	}
	
	# single substitution for environment variables:
	if {[regexp {^(.*) %} $executable match tmp]} {
	    set executable $tmp;
	}

	if {[regexp {%(.*)%} $executable match name]} {
	    if {![regsub "%$name%" $executable $env($name) executable]} {
		break;
	    }
	}

	return $executable;
    }

    # ---------------------------------------------------------------------------
    # Function    : registry-find-path - 
    # 
    # Returns     : full path to program file
    # Parameters  : file program name
    # 
    # Description : 
    # ---------------------------------------------------------------------------
    proc registry-find-path {file} {
	return [registry get HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App\ Paths\\$file ""];
    }

    # ---------------------------------------------------------------------------
    # Function    : registry-init-utils - 
    # 
    # Returns     : - 
    # Parameters  : -
    # 
    # Description : load registry extension and store these default utils 
    #               if they are not already stored (no asking dialog will apear). 
    # ---------------------------------------------------------------------------
    proc registry-init-utils {} {
	global env;
	set ver [info tclversion];
	set ver_l [split $ver .];
	set ver_ [join $ver_l {}];

	if {![catch {
	    load "tclreg${ver_}.dll" registry;
	} msg]} {
	    # Don't use the registry if the registry dll couldn't be loaded.
	    # check if there is an editor registered
	    if {![info exists env(EDITOR)]} {
		set editor [registry-find-exe "glymna.txt"];

		# Do we run in a gipsy interpreter ?
		if {[string compare "gipsy" [file root [file tail\
			[info nameofexecutable]]]] == 0} {

		    # Ignore the above Editor and try to find
		    # the Edit item from the context menu.
		    set editor [registry-find-exe "glymna.tcl" Edit];
		    
		}
		if {[string length $editor] > 1} {
		    set env(EDITOR) $editor;
		}


	    }
	    
	    if {![info exists env(GHTMLVIEWER)]} {
		set viewer [registry-find-exe "viewer.htm"];
		if {[string length $viewer] > 1} {
		    set env(GHTMLVIEWER) $viewer;
		}
	    }
	}
    }
    # init utilities ...
    registry-init-utils;
}

