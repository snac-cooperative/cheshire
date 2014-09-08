# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary 1.0 compabillity File
#              This modul contains all procedures and
#              naming conventions to use the library
#              with its old interface.
# -----------------------------------------------------------------------
package provide rtl_compat 1.0;
package require rtl_all;

# ---------------------------------------------------------------------------
# Function    : mkOptionPair - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Simplify to define a data oriented option, you must
#               call mkOptionPairs before calling mkOptionPair.
# ---------------------------------------------------------------------------
proc mkOptionPair {class args} {
    foreach typ $args {
	uplevel "#0"\
		"proc ::${class}::get-${typ} \{base\} \{\n\
		\nreturn \[get-action \$base ${typ}\];\
		\n\}\n\
		\nproc ::${class}::set-${typ} \{base value\} \{\n\
		\nreturn \[set-action \$base ${typ} \$value\];\
		\n\}\n";
    }
}

# ---------------------------------------------------------------------------
# Function    : mkOptionPairs - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Generates a special set/get procedure to allow
#               simplify the definition of data oriented options
#               The procedure can be used together with mkOptionPair
#               or standalone.
# ---------------------------------------------------------------------------
proc mkOptionPairs {class args} {
    ## create all attribute callback procedures -- for actions ...
    ## create set/get - action first. 
    ::eval "\
	    proc ::${class}::set-action \{base typ action\} \{\n\
	    \nvariable var;\
	    \nset var(\$base,\$typ) \$action;\
	    \nreturn \$action;\
	    \n\}\n\
	    \nproc ::${class}::get-action \{base typ\} \{\n\
	    \nvariable var;\
	    \nif \{!\[info exists var(\$base,\$typ)\]\} \{\n\
	    \nreturn \{\};\
	\n\}\n\
	\nreturn \[set var(\$base,\$typ)\];\
    \n\}\n";

#    ::eval {mkOptionPair $class} $args;
     foreach typ $args {
	 ::eval "proc ::${class}::get-${typ} \{base\} \{\n\
 		\nreturn \[get-action \$base ${typ}\];\
 		\n\}\n\
 		\nproc ::${class}::set-${typ} \{base value\} \{\n\
 		\nreturn \[set-action \$base ${typ} \$value\];\
 		\n\}\n";
     }
}

# ---------------------------------------------------------------------------
# Function    : mkTrash - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal used (currently deactivated) Don't use this
#               procedure
# ---------------------------------------------------------------------------
proc mkTrash {procedure} {
    if {[string compare "" [info commands ::trash::*]] != 0} {
	namespace delete ::trash
    }
    rename $procedure ::trash::current;
    set ::trash::procedure $procedure;
}

# ---------------------------------------------------------------------------
# Description : Wrapper functions for backward compability
#               for all RuntimeLibrary version 1.0 templates.
# ---------------------------------------------------------------------------
proc preferences {args} {
    return [uplevel 1 "rtl_preferences $args"];
}

proc scrollwin {args} {
    return [uplevel 1 "rtl_scrollwin $args"];
}

proc spinbox {args} {
    return [uplevel 1 "rtl_spinbox $args"];
}

proc statusbar {args} {
    return [uplevel 1 "rtl_statusbar $args"];
}

proc symbolbar {args} {
    return [uplevel 1 "rtl_symbolbar $args"];
}

proc symbolbarcustomize {args} {
    return [uplevel 1 "rtl_symbolbarcustomize $args"];
}

proc symbutton {args} {
    return [uplevel 1 "rtl_symbutton $args"];
}

proc tree {args} {
    return [uplevel 1 "rtl_tree $args"];
}

proc question {args} {
    return [uplevel 1 "rtl_question $args"];
}

proc gridwin {args} {
    return [uplevel 1 "rtl_gridwin $args"];
}

proc combobox {args} {
    return [uplevel 1 "rtl_combobox $args"];
}

proc mlistbox {args} {
    return [uplevel 1 "rtl_mlistbox $args"];
}

# ---------------------------------------------------------------------------
# Function    : vgshell - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Wrapper for rtl_shell =- vgshell
# ---------------------------------------------------------------------------
proc vgshell {args} {
    return [uplevel 1 "rtl_shell $args"]; 
}


foreach vgmodul [list optionInfo inspector optionItem project vgres] {
    namespace eval $vgmodul {};
    ::eval "\
	    proc ::${vgmodul}::set-action \{base typ action\} \{\n\
	    ::set-action ${vgmodul} \$base \$typ \$action;\n\
	    \n\}\n\
	    \nproc ::${vgmodul}::get-action \{base typ\} \{\n\
	    \nreturn \[::get-action ${vgmodul} \$base \$typ)\];\
	    \n\}\n";

}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Implementation    : -
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

