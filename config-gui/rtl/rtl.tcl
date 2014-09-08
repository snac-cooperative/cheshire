# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary template creation module
# -----------------------------------------------------------------------
package provide rtl 1.2;

# ---------------------------------------------------------------------------
# Current patchlevel for the entire Runtime Library

set rtl_patchlevel 0;


# ===========================================================================
# Utilities section:
# ---------------------------------------------------------------------------
# Function    : rslider - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : trigger procedure used to insert/hide scrollbars when needed
# ---------------------------------------------------------------------------
proc rslider {list slider d {auto 1}} {
    if {$auto != 0} {
	if {$auto == $d || $auto == "1"} {
	    set ys [$list ${d}view];
	    set pos1 [lindex $ys 0];
	    set pos2 [lindex $ys 1];
	    if {0 != $pos1 || $pos2 < 1.0} {
		grid $slider;
	    } elseif {0 == $pos1 && $pos2 == 1} {
		grid remove $slider;
	    }
	}
    } elseif {![winfo ismapped $slider]} {
	grid $slider;
    }
}

# ---------------------------------------------------------------------------
# Function    : mkFirstLow - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : uncapitalize the first char of the given string
# ---------------------------------------------------------------------------
proc mkFirstLow {str} {
    return [string tolower [string range $str 0 0]][string range $str 1 end]; 
}

# ---------------------------------------------------------------------------
# Function    : mkFirstUp - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Capitalize the first char of the given string
# ---------------------------------------------------------------------------
proc mkFirstUp {str} {
    return [string toupper [string range $str 0 0]][string range $str 1 end]; 
}

# ---------------------------------------------------------------------------
# Function    : opget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : read the value for the given database name out of the
#               Tk resource database, if there is no entry defined,
#               deliever the given defalt (param) back to the caller.
#               THIS PROCEDURE MUST BE INCLUDED IN EVERY GENERATED
#               VISUALGIPSY 2.0 PROJECT, AND THEREFOR IS DUPLICATED IN
#               EACH IFC-FILE.When you include rtl.tcl in your
#               running application remove the duplicated entries from
#               your ifc-files.
# ---------------------------------------------------------------------------
proc opget {w argum param} {
    if {![winfo exists $w]} {
	return $param
    }
    set opt [option get $w $argum {}];
    if {![string compare $opt {}]} {
	return $param;
    }
    return $opt;
}

# ---------------------------------------------------------------------------
# Function    : mkStatic - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal used to declare options as static.
#               Use this procedure when defineing static options in
#               your template.
# ---------------------------------------------------------------------------
proc mkStatic {option value static} {

    set value [string trim $value \{\}];
    set static [string trim $static \{\}];

    if {[string compare $value $static] != 0} {
	uplevel 3 "error \"Error: can't modify -$option after widget is created\"";
   }
}

# ---------------------------------------------------------------------------
# Function    : getRexpAttr - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : get atributes using a regular expression
# ---------------------------------------------------------------------------
proc getRexpAttr {path option rexp} {
    foreach w [traverseWidgets $path] {
	if {[regexp $rexp $w match]} {
	    if {$w == $path} {
		set val [${w}_# cget -$option];
	    } else {
		set val [$w cget -$option];
	    }
	    return $val;
	}
    }
    return {};
}

# ---------------------------------------------------------------------------
# Function    : setRexpAttr - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : set attributes using a regular expression
# ---------------------------------------------------------------------------
proc setRexpAttr {path option rexp val} {

    foreach w [traverseWidgets $path] {
	if {[regexp $rexp $w match]} {
	    if {$w == $path} {
		${w}_# configure -$option $val;
	    } else {
		$w configure -$option $val;
	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : mkRexpPair - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Create set/get pairs for regular expressions
#               only set uses the regexp.
# ---------------------------------------------------------------------------
proc mkRexpPair {class args} {
    foreach {typ rexp option} $args {
	uplevel "#0"\
		"proc ::${class}::get-${typ} \{base\} \{\n\
		\nreturn \[::get-action ${class} \$base ${typ}\];\
		\n\}\n\
		\nproc ::${class}::set-${typ} \{base value\} \{\n\
		\nsetRexpAttr \$base ${option} \{$rexp\} \$value;\
		\nreturn \[::set-action ${class} \$base ${typ} \$value\];\
		\n\}\n";
    }
}


# ---------------------------------------------------------------------------
# Function    : check-instance - look for an classdefinition
#               from within the unknown procedure.
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal used! Do not include calls of this procedure
#               into your application
# ---------------------------------------------------------------------------
proc check-instance {flag name args} {
    upvar $flag mark;
    set result "";
    if {![string compare ${name}_# [info commands ${name}_#]]} {
	set mark 1;
	if {[winfo exists $name]} {
	    set wc [mkFirstLow [winfo class $name]];
	    set result [uplevel 2  "instance ${wc} $args"];
	}
    }
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : getCreator - locate creator for widget
# 
# Returns     : widget creator
# Parameters  : args - options which may contain -creator
#               widget_class - new class for widget
# 
# Description : Locate the true class for the template
#               this class can be of type toplevel, frame
#               or a special class created with getExtCreator
#               -> set vgnext.htm
# ---------------------------------------------------------------------------
proc getCreator {widget_class widget args} {
    variable ${widget_class}::var;
    set args [lindex $args 0];
    set id [lsearch -exact $args "-creator"];
    if {$id >= 0} {
	incr id;
	set class [lindex $args $id];
    } else {
	set origin [lindex [array get ${widget_class}::var original] 1];
	if {[string compare "" $origin] != 0} {
	    set class $origin;
	} else {
	    set class "frame";
	}
    }

    array set ${widget_class}::var [list $widget,creator $class];
    return $class;
}
# ---------------------------------------------------------------------------
# Function    : getExtCreator - Extended constructor for templates
# 
# Returns     : new widget class 
# Parameters  : argl   - original config options (custom)
#               args   - config options (predefined)
#               widget - widget to create (in getExtCreator)
#               widget_class - assumed widget class, in normal case
#                              this class is delivered by getExtCreator
# 
# Description : Create widget from the option -class, and deliviers
#               current widget class (which is also the result).
#               This procedure is usfull for templates used as
#               bases for new more specific templates.
#               getExtCreator is not supported from VisualGIPSY.
#               ---
#               The functionality of getExtCreater is still under
#               decision and may be changed in the future. When using
#               it be very careful and also secure this version before
#               upgrading to a higher version.
#
#               See the release notes concerning this issue.
# ---------------------------------------------------------------------------
proc getExtCreator {widget_class widget argl args} {

    # Maybe someone redefines this class
    # using the hidden option -class

    set id [lsearch -exact $argl "-class"];
    if {$id >= 0} {
	incr id;
	set widget_class [mkFirstLow [lindex $argl $id]];
    }

    variable ${widget_class}::var;
    set id [lsearch -exact $argl "-creator"];

    if {$id >= 0} {
	incr id;
	set class [mkFirstLow [lindex $argl $id]];

	# Take care that widget_class != class
	# Both -creator and -class are equal if it was
	# called from another template ...

	if {$class == $widget_class} {
	    set origin [lindex [array get ${widget_class}::var original] 1];
	    if {[string compare "" $origin] != 0} {
		set class $origin;
	    } else {
		set class "frame";
	    }
	}

    } else {
	set origin [lindex [array get  ${widget_class}::var original] 1];
	if {[string compare "" $origin] != 0} {
	    set class $origin;
	} else {
	    set class "frame";
	}
    }
    
    ## -creator can be a template or a real class
    ## If it is a template it is required  to determine which 
    array set ${widget_class}::var [list $widget,creator $class];

    uplevel "#0" [linsert $args 0 $class $widget\
	    "-class" [mkFirstUp $widget_class]];
    return $widget_class;
}

# ---------------------------------------------------------------------------
# Function    : setvar - set a array element in the namespace $class
# 
# Returns     : -
# Sideeffect  : set $class::var
# Parameters  : class - namespace in wich var is defined
#               args  - list of name values to var
# 
# Description : Internal used, but can also be public used.
# ---------------------------------------------------------------------------
proc setvar {class args} {
    namespace inscope $class array set var $args;
}

# ---------------------------------------------------------------------------
# Function    : getvar - get the value from var in namespace $class
# 
# Returns     : value for $name
# Parameters  : class - namespace with array var 
#               name  - element from array var
# 
# Description : Counterpart to setvar internal/public
# ---------------------------------------------------------------------------
proc getvar {class name} {
    return  [lindex [namespace inscope $class array get var $name] 1];
}
 
# ---------------------------------------------------------------------------
# Function    : namevar - list all matching names in array var
# 
# Returns     : list of array elements in $var
# Parameters  : class - namespace wich contains a array var
#               pat   - pattern to look for element names
# 
# Description : third procedure  to setvar/getvar internal/public
# ---------------------------------------------------------------------------
proc namevar {class pat} {
    return [namespace inscope $class array names var $pat];
}

# ---------------------------------------------------------------------------
# As mkTrash: do not use!
# ---------------------------------------------------------------------------
bind ClassDef <Destroy> {mkTrash %W}

# ---------------------------------------------------------------------------
# Function    : mkCGet - WIDGET (internal used)
#               %.sample cget -mumpitz
#               mamon
# 
# Returns     : value for the given option
# Parameters  : path  - full name of widget
#               class - if path is a container (must be) the
#                       lowerformed Classname, and also the
#                       namespace in wich this class was defined.
#               attr  - option (WIDGET attribute) without leading - sign
# 
# Description : Internal used
# ---------------------------------------------------------------------------
proc mkCGet {path class attr} {
    set l [mkGetConfigureAttr $path $class $attr];
    set lg [expr {[llength $l] - 1}];
    return [lindex $l $lg];
}   

# ---------------------------------------------------------------------------
# Function    : mkGetConfigureAttr - WIDGET (internal used)
#               %.sample configure -mumpitz
#               -mumpitz mumpitz Mumpitz {} mamon
# 
# Returns     : optionlist for the given option,
#               this list contains five elements. The first three
#               elements are the attribute names. fourth the
#               default value and the last is the current value.
# Parameters  : path  - full name of widget
#               class - associated namespace.
#               attr  - option
#               -- See above --
# 
# Description : Internal used
# ---------------------------------------------------------------------------
proc mkGetConfigureAttr {path class attr} {
    set name [string trim $attr -];
    set match 0;
    set attr_default [mkFirstUp $name];

    if {$name == "creator"} {
	set para [getvar $class "$path,creator"];
	set para_default [getvar $class original];
	if {![string compare $para {}]} {
	    set para $para_default;
	}
    } else {
	if {![info exists ${class}::var($name)]} {
	    uplevel 2 "error \{unknown option $attr\}";
	}
	set argList [set ${class}::var($name)];
	set para {};
	set para_default {};
	set elem [lindex $argList 0];

	if {[regexp {^([0-9]*)$} $elem match]} {
	    set elem $argList;
	}
	if {[llength $elem] == 1} {
	    # methode
	    set para [${class}::get-$elem $path];
	    set para_default {};
	} else {
	    set which [lindex $elem 0];
	    set option [lindex $elem 1];
	    
	    if {[regexp {^([0-9]*)$} $which match]} {
		# widget number

		if {$which == 0} {
		    set w "${path}_#";
		} else {
		    set w ${path}[set ${class}::w($which)];
		}
	    } else {
		# regular expressions
		foreach id [array names ${class}::w] {
		    if {[regexp $which [set ${class}::w($id)] match]} {
			if {$id == 0} {
			    set w "${path}_#";
			} else {
			    set w ${path}[set ${class}::w($id)];
			}
			break;
		    }
		}
	    }

	    if {![info exists w]} {
		uplevel 2 "error \{wrong regular expression in $elem\}";
	    }

	    set optL [$w configure -$option];
	    set para [lindex $optL 4];
	    set para_default [lindex $optL 3];
	}
    }

    return [list $attr $name $attr_default\
	    [opget $path $attr_default $para_default] $para];
}


# ---------------------------------------------------------------------------
# Function    : mkGetConfigureAll - get complete list of options
#               % .sample configure
#               {-mum mum Mum {} {}} {-mumpitz mumpitz Mumpitz {} mamon} 
# 
# Returns     : optionlists
# Parameters  : path  - full name of widget
#               class - namespace
#               -- See above --
# 
# Description : Internal used.
# ---------------------------------------------------------------------------
proc mkGetConfigureAll {path class} {
    set attrs [list];
    set attr_list [getvar $class attributes];
    lappend attr_list "creator"; 
    foreach attr [lsort $attr_list] {

	 set attr_list [mkGetConfigureAttr $path $class -$attr];
	if {$attr_list == {}} {
	     continue;
	 }
	 lappend attrs $attr_list;
     }
     return $attrs;
}
 
# ---------------------------------------------------------------------------
# Function    : mkConfigure - {WIDGET interna used} configure the
#               options for $path
#               % test .sample -mumpitz mamon
# 
# Returns     : -
# Parameters  : path  - full name for widget
#               class - namespace
#               args  - list of option/value pairs
#               -- See above --
# 
# Description : Internal used
# Note        : Using the option database instead of command line
#               arguments may improve the creation by 1 percent
#               this is not enought to justify the additional code,
#               which consums up to 5 percent more ...
# ---------------------------------------------------------------------------
proc mkConfigure {path class vals} {
    foreach {name para} $vals {
	 set name [string trim $name -];
	 if {![string compare $name creator] ||\
		 ![string compare $name class]} {
	     continue;
	 }

	 set argList [getvar $class $name];
	 set fbreak 0;

	 foreach elem $argList {
	     if {[regexp {^([0-9]*)$} $elem match]} {
		 set elem $argList;
		 set fbreak 1;
	     }

	     if {[llength $elem] == 1} {
		 # methode call
		 ${class}::set-${elem} $path $para;
	     } else {
		 # option (regexp/widget index)
		 foreach {which option} $elem break;
		 if {[regexp {^([0-9]*)$} $which match]} {
		     # widget number
		     lappend cfg_list($which) -$option $para;
		 } else {
		     # regular expression
		     foreach id [array names ${class}::w] {
			 if {[regexp $which [set ${class}::w($id)] w]} {
			     lappend cfg_list($id) -$option $para;
			 }
		     }
		 }
	     }
	     if {$fbreak} break;
	 }
     }

     foreach id [array names cfg_list *] {
	 if {$id == 0} {
	     set w "${path}_#";
	 } else {
	     set w ${path}[set ${class}::w($id)];
	 }
	 
	 eval {$w configure} $cfg_list($id);
     }
     
}

# ---------------------------------------------------------------------------
# Function    : mkDefaultConfigure - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal used to set the default attributes during
#               instanciation an widget with a template.
# ---------------------------------------------------------------------------
proc mkDefaultConfigure {path class args} {
    if {[llength $args] == 1} {
       set args [lindex $args 0]
    }

    foreach da [set ${class}::var(attributes)] {
	set id [lsearch -exact $args -$da];
	if {$id < 0} {
	    set argL [set ${class}::var($da)];
	    foreach elem $argL {
		if {[llength $elem] == 1} {
		    
		    # Don't set default values, if this
		    # is a function call. Use the database
		    # to redefine  settings

		    #set defValue [mkCGet $path $class -$da];
		    set defValue [opget $path $da {}];
		    if {$defValue == {}} {
			continue;
		    } else {
			set args [linsert $args 0 -$da $defValue];
			break;
		    }
		} else {
		    if {[catch {
			set defValue [mkCGet $path $class -$da];
		    } msg]} {
			continue;
		    }
		    set defValue [opget $path $da $defValue];
		    if {$defValue == {}} {
			continue;
		    }
		    set args [linsert $args 0 -$da $defValue];
		    break;
		}
	    }
	}
    }

    return $args;
}



# ---------------------------------------------------------------------------
# Function    : instance - Alternaitive way to use 
#               classdefinitions, in contrast to
#               the procedure based approach it is not needed
#               to delete or create alternatives procedures
#               
# 
# Returns     : result
# Parameters  : any option
# 
# Description : Central procedure for widget handling. Internal used
# ---------------------------------------------------------------------------
proc instance {class path acti args} {
    set result "";
    switch -- $acti {
	cget {
	    if {[llength $args] != 1} {
		uplevel 1 "error \{wrong # args: should be \"$path cget option\"\}";
	    }
	    set result [mkCGet $path $class [lindex $args 0]];
	}
	config -
	configure {
	    set lg [llength $args];
	    if {0 == $lg} {
		set result [mkGetConfigureAll $path $class];
		} elseif {1 == $lg} { 
		    set result\
			    [mkGetConfigureAttr $path $class [lindex $args 0]];
               } else {
                  set result [mkConfigure $path $class $args];
               }
           }
	   default {
	       #set result [uplevel 1 "rtlparser $class $acti $path $args"];
  	       set proc_action ${class}::$acti;
  	       if {![string compare \
  		       [info commands $proc_action] ::$proc_action]} {
  		   set result [uplevel 1 [linsert $args 0 $proc_action $path]];
	       } elseif  {[catch {
  		   set result [uplevel 1 [linsert $args 0 ${path}_# $acti]];
  	       } msg]} {
  		   set res [rtlparser $class commands];
		   foreach n [list names commands] {
		       set id [lsearch $res $n];
		       if {$id >= 0} {
			   set res [lreplace $res $id $id];
		       }
		   }
		   
  		   uplevel 1 "error \{bad option \"$acti\": must be\
  			   [join $res ,\ ]\}";
  	       }
	   }
       }
       return $result;
}


# ---------------------------------------------------------------------------
# Function    : mkProc - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Central template constructor, used in combination with
#               getCreator above.
# ---------------------------------------------------------------------------
proc mkProc {path class argl} {
    # Visual GIPSY 2.0 relevant stuff:
    # register this component as the edit target.
    set id 1;
    if {![string compare [info commands ::mkRegisterVG]\
	    "::mkRegisterVG"]} {

	set id [mkRegisterVG $path $class 0];
    }

    if {$id == 1} {
	set upclass [winfo class $path];
	set wcl [mkFirstLow $upclass];
    
	# initialize all options ...
	if {$class != $wcl || [info exists ::env(GPMODE)]} {
	    # different class / namespace
	    set result [eval [linsert $argl 0 mkProc-procedure $path $class]];
	} else {
	    # same namespace and class - it is possible to use
	    # unknown ...
	    set result [mkProc-unknown $path $class $argl];
	}

	# Add a proper destructor to this class ...
	bind $upclass <Destroy> [format {
	    if {![string compare "::%1$s::destroy"\
		    [info commands "::%1$s::destroy"]]\
		    } {
		::%1$s::destroy %%W;
	    }
	    default-destroy %1$s %%W;
	} $wcl];
    }
    return $path;
}

# ---------------------------------------------------------------------------
# Function    : mkProc - WIDGET (internal used), makes a procedure
#                        with the name of an existing widget.
#               %test .sample
# 
# Returns     : -
# Parameters  : path  - full name of widget
#               class - namespace
#               -- See above --
# 
# Description : Called from mkProc. Never use this procedure
#               directly! Internal used.
# ---------------------------------------------------------------------------
proc mkProc-procedure {path class argl} {
    # take care to destroy the hide the instance procedure
    # when the widget is destroyed.

    set btags [bindtags $path];
    set id [lsearch $btags ClassDef];
    if {$id < 0} {
	set lg [llength $btags];
	if {$lg > 1} {
	    incr lg -2;
	}
	bindtags $path [linsert $btags $lg ClassDef];
    }

    ###################################################
    #
    # take a hidden feature from tk!
    # rename the widgetname, this has no effect onto the
    # widgethierachy or the "visible" name.
    
    # allow inherit mechanism (multiple times call to mkProc).
    if {![string compare "${path}_#" [info commands ${path}_#]]} {
	return $path;
    }

    rename $path ${path}_#;
    set script "\
    proc $path \{acti args\} \{\n\
         \treturn \[::eval \"instance $class $path \$acti \$args\"\];\n\
         \}\n\
    ";

    namespace inscope :: eval $script;
    namespace inscope $class namespace export $path;
    namespace inscope :: namespace import ${class}::$path;

    set args [lindex $args 0];
    if {[catch {
	if {[llength $args] > 0} {
	    set args [eval [linsert $args 0 mkDefaultConfigure $path $class]];
	} else {
	    set args [eval [list mkDefaultConfigure $path $class]];
	}
    } msg]} {
	puts stderr "Exception : $msg";
    }

     if {[llength $args] > 0} {
	 mkConfigure $path $class $args;
     }

     return $path;
}



# ---------------------------------------------------------------------------
# Function    : mkProc - move the original widget proc away
# 
# Returns     : widget
# Parameters  : 
# 
# Description : Called from mkProc. Never use this procedure directly!
#               Internal used
# ---------------------------------------------------------------------------
proc mkProc-unknown {path class argl} {

    rename $path ${path}_#;
    set argl [mkDefaultConfigure $path $class $argl];
    if {[llength $argl] > 0} {
	mkConfigure $path $class $argl;
    }
    return $path;
}

# ---------------------------------------------------------------------------
# Function    : default-destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Default destructor, assigned within mkProc to this
#               specifc class.
# ---------------------------------------------------------------------------
proc default-destroy {class path} {
    variable ::${class}::$path;

    if {[info exists ::${class}::$path]} {
	unset ::${class}::$path;
    }
}

# ---------------------------------------------------------------------------
# Install a new unknown procedure to allow the mkProc-unknown usage.
# If you do not want to use the unknown facility, set the environment
# variable GPMODE before starting the project.
# ---------------------------------------------------------------------------
if {![info exists env(GPMODE)] &&\
	![string compare "" [info commands "hidden::unknown"]]} {

    rename unknown hidden::unknown;

# ---------------------------------------------------------------------------
# Function    : unknown args -  new version to search for
#               instanciated widgets
# 
# Returns     : command result
# Parameters  : 
# 
# Description : move the original unknown procedure to
#               a hidden namespace.
# ---------------------------------------------------------------------------
    proc unknown args {
	set name [lindex $args 0];

	set flag 0;
	set result [eval "check-instance flag $name $args"];
	if {$flag} {
	    return $result;
	}	
	# make sure that unknown will be evaluated on the
	# same level as the new version.
	return [uplevel 1 "hidden::unknown $args"];
    }

}

# ===========================================================================
# Generic Attribute handler:
# ---------------------------------------------------------------------------
# Function    : set-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : general set method, this method is used to define
#               a value in this instance
# ---------------------------------------------------------------------------
proc set-action {class base type action} {
    set var ::${class}::${base};
    set ${var}($type) $action;
    return $action;
}

# ---------------------------------------------------------------------------
# Function    : get-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : general get method, this method delievers the
#               value as defined in this instance
# ---------------------------------------------------------------------------
proc get-action {class base type {def {}}} {
    set var ::${class}::${base};
    if {![info exists ${var}($type)]} {
	return [string trim [opget $base $type $def] \{\}];
    }
    return [set ${var}($type)];
}


# ---------------------------------------------------------------------------
# Function    : create-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : substitutes mkOptionPairs
# ---------------------------------------------------------------------------
proc create-action {class args} {
    foreach optpair $args {
	if {[llength $optpair] == 2} {
	    set opt [lindex $optpair 0];
	    set def [lindex $optpair 1];
	} else {
	    set opt $optpair;
	    set def "\{\}";
	}

	uplevel "#0" "proc ::${class}::set-$opt \{base value\} \{\n\
		return \[::set-action $class \$base $opt \$value\];\n\
		\n\}\n\
		proc ::${class}::get-$opt \{base\} \{\n\
		return \[::get-action $class \$base $opt \{$def\}\];\n\
		\n\}\n";
    }
}

# ===========================================================================
# Widget Finder:
# ---------------------------------------------------------------------------
# Function    : getChildren - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get all children, which are not "aliases" or of class
#               "Invisible"
# ---------------------------------------------------------------------------
proc getChildren {target} {
    set children [winfo children $target];
    set newchildren [list];
    foreach i $children {
	if {[winfo class $i] == "Invisible"} {
	    continue;
	}

	if {[string first "#" $i] > -1} {
	    continue;
	}
	lappend newchildren $i;
    }
    return $newchildren;
}

# ---------------------------------------------------------------------------
# Function    : traverseWidgets - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Create a hierarchy list of widgets, starting with the
#               target widget.
# ---------------------------------------------------------------------------
proc traverseWidgets {target} {
    set w_tree "$target ";
    foreach i [getChildren $target] {
	foreach x [traverseWidgets $i] {
	    lappend w_tree $x;
	}
    }
    return $w_tree;
}

# ---------------------------------------------------------------------------
# Function    : find-widget - 
# 
# Returns     : widget name/ empty
# Parameters  : 
# 
# Description : fixed in patchlevel 2
# ---------------------------------------------------------------------------
proc find-widget {name target} {

    set result {};
    if {$name == [winfo name $target]} {
	set result $target;
    } else {
	foreach i [getChildren $target] {
	    set result [find-widget $name $i];
	    if {[string compare "" $result] != 0} {
		# do not return $i
		return $result;
	    }
	}
    }
    return $result;
}

# ===========================================================================
# Resize Handler:
# ---------------------------------------------------------------------------
# Function    : resize-start - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Start drawing line ond button press
# ---------------------------------------------------------------------------
proc resize-start {path widget x y {ori x} {inv 0}} {

    if {![winfo exists $path.slider]} {
	frame $path.slider -class Invisible -width 3 -height 3\
		-background blue3;
    }

    set pos [expr [winfo $ori $widget] + [set $ori]];
    switch $ori {
	"x" {
	    place $path.slider -$ori $pos -relheight 1 -y 0;
	}
	"y" {
	    place $path.slider -$ori $pos -relwidth 1 -x 0;
	}
    }

    raise $path.slider;
    return;
}
    
# ---------------------------------------------------------------------------
# Function    : resize-end - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Drag release event; destroy resize line
# ---------------------------------------------------------------------------
proc resize-end {path widget x y {ori x} {typ column}\
	{inv 0} {attract {}} } {
    
    if {$inv} {
	set $ori [expr {0 - [set $ori]}];
    }

    if {[winfo exists $widget]} {

	switch $typ {
	    "column" {
		switch $attract {
		    "right" {
			set xpos [expr {[winfo $ori $widget]\
				+ [winfo width $widget]}];
		    }
		    default {
			set xpos [winfo $ori $widget];
		    }
		}

		set loc [grid location $path $xpos 10];
		set column [lindex $loc 0];
		set pos \
			[lindex [grid location $path\
			[expr {[winfo $ori $widget] + [set $ori]}] 10] 0];
		set bbox [grid bbox $path $column 0];
	    }
	    "row" {

		switch $attract {
		    "below" {
			set ypos [expr {[winfo $ori $widget]\
				+ [winfo height $widget]}];
		    }
		    default {
			set ypos [winfo $ori $widget];
		    }
		}

		set loc [grid location $path 10 $ypos];
		set row [lindex $loc 1];
		set pos \
			[lindex [grid location $path\
			10 [expr {[winfo $ori $widget] + [set $ori]}]] 1];
		set bbox [grid bbox $path 0 $row];
	    }
	}
	
	if {$inv} {
	    set column_width     [lindex $bbox 2];
	    set row_width        [lindex $bbox 3];
	    set size             [set ${typ}_width];
	    incr size            [set $ori];
	} else {
	    if {$pos >= [set $typ]} {
		set column_width [lindex $bbox 2];
		set row_width    [lindex $bbox 3];
		set size         [set ${typ}_width];
		incr size        [set $ori];
	    } else {
		set size 0;
	    }
	}
	grid ${typ}configure $path [set $typ] -minsize $size;
    }

    if {[winfo exists $path.slider]} {
	destroy $path.slider;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : resize-drag - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Drag line to new position
# ---------------------------------------------------------------------------
proc resize-drag {path widget x y {ori x} {inv 0}} {

    if {[winfo exists $path.slider]} {
	if {[winfo exists $widget]} {
	    set pos [expr [winfo $ori $widget] + [set $ori]];
	    place configure $path.slider -$ori $pos;
	    
	}
    }

    return;
}

# ===========================================================================
# Line drag utilities in x/y direction:
# ---------------------------------------------------------------------------
# Function    : xsizer - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Draw an x-oriented line splitter
# ---------------------------------------------------------------------------
proc xsizer {nope handle base column anchor {steps 10}} {

    frame $handle\
	    -bd 0 -relief raised -width 0\
	    -bg black -cursor sb_h_double_arrow;

    frame $nope \
	    -bd 1 -relief raised\
	    -width 6 -height 6 -cursor sb_h_double_arrow;


    place $nope -in $handle -x -3 -y 24;
    grid  $handle -row 0 -column $column -sticky $anchor\
	    -rowspan $steps;
	
    foreach evpair [list {B1-Motion resize-drag}\
	    {1 resize-start} {ButtonRelease-1 resize-end}] {
	set ev [lindex $evpair 0];
	set script [lindex $evpair 1];
	bind  $handle <$ev> "$script $base %W %x %y";
	bind $nope <$ev> "$script $base $handle %x %y";
    }


    # Ommit event generate due BUG in Tk 8.3 ...

    #      foreach ev [list B1-Motion 1 ButtonRelease-1] {
    #  	bind $nope <$ev> "event generate $handle <$ev> -x %x -y %y";
    #      }
}

# ---------------------------------------------------------------------------
# Function    : ysizer - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Draw a y-oriented line splitter
# ---------------------------------------------------------------------------
proc ysizer {nope handle base row anchor {steps 10}} {
    frame $handle\
	    -bd 0 -relief raised -width 0\
	    -bg black -cursor sb_v_double_arrow;

    frame $nope \
	    -bd 1 -relief raised\
	    -width 6 -height 6 -cursor sb_v_double_arrow;

    place $nope -in $handle -y -3 -x 24;
    grid  $handle -row $row -column 0 -sticky $anchor\
	    -columnspan $steps;
	
    foreach evpair [list {B1-Motion resize-drag} {1 resize-start}] {
	set ev [lindex $evpair 0];
	set script [lindex $evpair 1];
	bind  $handle <$ev> "$script $base %W %x %y y";
	bind $nope <$ev> "$script $base $handle %x %y";

    }

    bind  $handle <ButtonRelease-1> "resize-end $base %W %x %y y row";
    bind  $nope <ButtonRelease-1> "resize-end $base $handle %x %y y row";

    # Ommite event generate due BUG in Tk8.3 ...

#      foreach ev [list B1-Motion 1 ButtonRelease-1] {
#  	bind $nope <$ev> "event generate $handle <$ev> -x %x -y %y";
#      }

}

# ===========================================================================
# Utilities for creating templates:
# ---------------------------------------------------------------------------
# Function    : rtlmerge - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Merges all attributes of path, corrosponding to index
#               to the template interface.
#               As a sideeffect a flag ismerged will be added to
#               array var. rtlmerge will check this flag before
#               processing any merge operations. 
# ---------------------------------------------------------------------------
proc rtlmerge {path class {index 0}} {
    if {![info exists ::${class}::var(ismerged)]} {
        # get the subwidget
        eval set w $${class}::w($index);
        set attr_l [$path$w configure];
	foreach attr $attr_l {
	    if {[llength $attr] < 5} {
		continue;
	    }
	    set name [string trim [lindex $attr 0] -];
	    if {[lsearch -exact\
		    [set ::${class}::var(attributes)] $name] < 0} {
		lappend ::${class}::var(attributes) $name;
	    }

	    set pair "$index $name";
	    if {[info exists ::${class}::var($name)]} {
		if {[lsearch -exact\
			::${class}::var($name) $pair] < 0} {
		    lappend ::${class}::var($name) $pair;
		}
	    } else {
		set ::${class}::var($name) $pair;
	    }
	}
	set ::${class}::var(ismerged) 1;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtlparser - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Used for non widget templates to reconfigure objects
# ---------------------------------------------------------------------------
proc rtlparser {class {action ""} {ext {}}  args} {
    set result [list];

    switch $action {
	"open" {

	    # Check whether or not to create a procedure for
	    # this particular entry.
	    if {![info exists ::env(RTLUNPROC)] || !$::env(RTLUNPROC)} {
		uplevel "#0" "proc $ext \{action args\} \{\n\
			return \[eval \"${class} \$action $ext \$args\"\];
		\}\n";
	    }

	    array set ::${class}::$ext {};
	    if {[llength $args] > 0} {
		eval [linsert $args 0 $class configure $ext];
	    }
	    set idl    [expr {[string last "::" $ext] + 2}];
	    set result [string range $ext $idl end];
	}
	"cget" {
	    set result [::${class}::get[lindex $args 0] $ext];
	}
	"configure" {
	    set la [llength $args];
	    if {$la == 0} {
		set idl [string length ::${class}::get];
		foreach cmd [info commands ::${class}::get-*] {
		    set val [$cmd $ext];
		    set attr [string range $cmd $idl end];
		    lappend result [list $attr $val];
		}
	    } elseif {$la == 1} {
		set result [::${class}::get[lindex $args 0] $ext];
	    } else {
		foreach {attr val} $args {
		    ::${class}::set$attr $ext $val;
		}
	    }
	}
	"typ" -
	"type" {
	    return $class;
	}
	"close" {
	    if {[catch {

		# Remove the data of that item ...
		if {[info exists ::${class}::${ext}]} {
		    unset ::${class}::${ext};
		}

		# Remove an existing procedure ...
		if {![string compare $ext [info commands $ext]]} {
		    rename $ext {};
		}
	    } msg]} {
		puts stderr "Close failed: $msg"
	    }
	}

	"names" {

	    set qname  ::${class}::${ext};
	    set idl     [expr {[string last "::" $qname] + 2}];
	    set pattern [string range $qname 0 $idl];
	    set vars    [info vars ${pattern}*];

	    foreach var $vars {
		lappend result [string range $var $idl end];
	    }

	}
	"commands" {
	    set idl [string length ::${class}::];
	    foreach cmd [info commands ::${class}::*] {
		if {[string first "::${class}::get-" $cmd] == 0 ||\
			[string first "::${class}::set-" $cmd] == 0} {
		    continue;
		}
		lappend result [string range $cmd $idl end]; 
	    }
	    foreach act [list cget configure names commands type close] {
		lappend result $act;
	    }
	    set result [lsort $result];
	}
	default {
	    set cmds [rtlparser $class commands];
	    if {[lsearch $cmds $action] >= 0} {
		return [uplevel 1\
			[linsert $args 0 ::${class}::${action} $ext]];
	    } else {
		uplevel 1 "error \{bad option \"$action\": must be\
  			   [join $cmds ,\ ]\}";
	    }
	}
    }
    return $result;
}

if {[string compare "::centerPos" [info commands "::centerPos"]] != 0} {
    # ---------------------------------------------------------------------------
    # Function    : centerPos - 
    # 
    # Returns     : 
    # Parameters  : 
    # 
    # Description : position of toplevel widget.
    # ---------------------------------------------------------------------------
    proc centerPos {top} {
	
	update;
	set x [winfo rootx $top];
	set y [winfo rooty $top];
	set h [winfo height $top];
	set w [winfo width $top];
	set geom [wm geometry $top];
	
	set sw [winfo screenwidth .];
	set sh [winfo screenheight .];
	set dw [expr {($sw - $w) / 2}];
	set dh [expr {($sh - $h) / 2}];
	set geom [split $geom x+];
	set gx [lindex $geom 2];
	set gy [lindex $geom 3];
	set dgx [expr {$gx - $x}];
	set dgy [expr {$gy - $y}];
	incr dw $dgx;
	incr dh $dgy;
	
	set geo +$dw+$dh;
	wm geometry $top $geo;
	
    }
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Todo :
#                     [1] Added lsort to configureAll
# ---------------------------------------------------------------------------
# Port RTL 1.2      : 4.04.2000,Roger
# Test              :
# Integration VG 2.6:
# System            :
# ---------------------------------------------------------------------------


