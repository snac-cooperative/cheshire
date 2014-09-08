# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary scrollable widget
# -----------------------------------------------------------------------
package provide rtl_scrollwin 1.2;
package require rtl;

####VG-START####
set rtl_scrollwin(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_scrollwin; 
	set src [file join $rtl_scrollwin(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_scrollwin.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_scrollwin 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_scrollwin {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .canvas 2 .canvas.frame]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  highlightbackground {0 highlightbackground} \
        height {0 height} highlightthickness {0 highlightthickness} \
        widget widget original frame xscrollcommand {1 xscrollcommand} \
        attributes [list widget fill xscrollcommand yscrollcommand background\
        borderwidth relief highlightbackground highlightcolor\
        highlightthickness width height] fill fill highlightcolor [list \
        [list 0 highlightcolor]] width {0 width} background [list \
        [list 0 background] [list 1 background] [list 2 background]] \
        borderwidth {0 borderwidth} relief {0 relief}\
        .rtl_scrollwin,creator frame yscrollcommand {1 yscrollcommand}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_scrollwin - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -widget
#             : -fill
#             : -xscrollcommand
#             : -yscrollcommand
#             : -background
#             : -borderwidth
#             : -relief
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -width
#             : -height 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_scrollwin 
# --------------------------------------------------------------------------- 
proc rtl_scrollwin {{base .rtl_scrollwin} args} {
 
    variable rtl_scrollwin::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_scrollwin $base$w(0) $args] $base\
		-class Rtl_scrollwin;

        # Childs to $base$w(0), rtl_scrollwin of class: 	Rtl_Scrollwin
        canvas $base$w(1) -closeenough "1.0" -height "1" \
            -highlightthickness 0 -width "1";
 
       # graphic children of $base$w(1):

        # Childs to $base$w(1), canvas of class: 	Canvas
        frame $base$w(2) -bd 0 -highlightthickness 0\
		-height "1000" -width "1000";

	$base$w(1) create window 0.0 0.0\
		-anchor "nw" -tags "$base$w(2)" -window\
		$base$w(2)


        bind $base$w(2) <Configure> \
             "rtl_scrollwin::adapt $base$w(0)";
 

        pack $base$w(1) -anchor center -expand 1 -fill both -ipadx 0\
		-ipady 0 -padx 0 -pady 0 -side top;
 
        catch {rtl_scrollwin::init $base;} 
        set base [mkProc $base rtl_scrollwin $args];  
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set rat(WIDGETCLASS,rtl_scrollwin) rtl_scrollwin;
# ---------------------------------------------------------------------------

####VG-STOP####

# Ensure that fill is set during instanciation.
option add *Rtl_scrollwin.fill {none};

# ---------------------------------------------------------------------------
# Function    : rtl_scrollwin::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor
# ---------------------------------------------------------------------------
proc rtl_scrollwin::init {base} {

    set-bindtag $base [join [split $base .] _];
}

proc rtl_scrollwin::pending {base type} {
    variable $base;

    if {![info exists ${base}(pending,$type)]} {
	set  ${base}(pending,$type) 1;
	after idle "$base $type;\
		\nrtl_scrollwin::unsetVar ${base} pending,$type";
    }
}

proc rtl_scrollwin::unsetVar {base item} {
    variable $base;

    if {[info exists ${base}($item)]} {
	unset ${base}($item);
    }
}

bind Rtl_scrollwin <Configure> \
	{rtl_scrollwin::adapt %W;}


# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_scrollwin {fill none} bindtag;

# ===========================================================================
# Overwritten Generic Attributes (set methods):
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : set-fill - set current fill option
# 
# Returns     : 
# Parameters  : 
# 
# Description : modifies the child expading behavior,
#               the child can expand/shrink in x or y direction
#               Note no scrollbar is needed in this direction!
# ---------------------------------------------------------------------------
proc rtl_scrollwin::set-fill {path arg} {
    switch $arg {
	"x" -
	"y" -
	"none" {}
	default {
	    uplevel 2 [list error "option -fill must be x, y, or none"];
	}
    }

    ::set-action rtl_scrollwin $path fill $arg;
    pending $path adapt;
}

# ===========================================================================
# Additional Attributes (set and get methods):
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : get-widget - used to retrieve current child widget 
# 
# Returns     : child
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_scrollwin::get-widget {path} {
    variable $path;

    if {![info exists ${path}(widget)]} {
	set children [pack slaves $path.canvas.frame];
	if {[llength $children] == 1} {
	    set-widget $path [lindex $children 0];
	}
    }

    return [::get-action rtl_scrollwin $path widget];

}

# ---------------------------------------------------------------------------
# Function    : set-widget - set current child widget 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_scrollwin::set-widget {path arg} {
    variable $path;
    
    if {[string compare [get-widget $path] $arg] != 0} {
	remove $path;
    }

    set ${path}(widget) $arg;
    if {[winfo exists $arg]} {
	pack $arg -in $path.canvas.frame -expand 1 -fill both;
	raise $arg;
    }
    pending $path adapt;
}

# ===========================================================================
# Public Methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_scrollwin::remove - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Remove current managed widget (this method is called
#               by set-widget and should not recall it.
# ---------------------------------------------------------------------------
proc rtl_scrollwin::remove {path} {
    variable $path;

    set widg [get-widget $path]

    if {[winfo exists $widg]} {
	set bt [bindtags $widg];
	set id [lsearch $bt [get-bindtag $path]];
	if {$id >= 0} {
	    bindtags $widg [lreplace $bt $id $id];
	}
	
	pack forget $widg;
    }

    set ${path}(widget) {};
}

# ---------------------------------------------------------------------------
# Function    : rtl_scrollwin::xview - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Scrolling command for the contained canvas widget
# ---------------------------------------------------------------------------
proc rtl_scrollwin::xview {path args} {
    return [::eval "$path.canvas xview $args"];
}

# ---------------------------------------------------------------------------
# Function    : rtl_scrollwin::yview - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Scrolling command for the contained canvas widget
# ---------------------------------------------------------------------------
proc rtl_scrollwin::yview {path args} {
    return [::eval "$path.canvas yview $args"];
}

# ---------------------------------------------------------------------------
# Function    : update - method to invoke a recalc and redraw action 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_scrollwin::update {path} {
    set widg [get-widget $path];
    if {[winfo exists $widg]} {
	set bt [bindtags $widg];
	set id [lsearch $bt [get-bindtag $path]];
	
	if {$id < 0} {
	    bindtags $widg [linsert $bt 1 [get-bindtag $path]];
	}
	
	pending $path adapt;
	set-widget $path $widg;
    }
}

# ---------------------------------------------------------------------------
# Function    : adapt - the geometry managment methode
# 
# Returns     : 
# Parameters  : 
# 
# Description : adapt will modify the scrollregion of the contained
#               canvas widget to the disired behavior x/y or none.
#               Propagtes the geometry from the container to the
#               canvas scrollregion.
# ---------------------------------------------------------------------------
proc rtl_scrollwin::adapt {path} {
    variable var;
    variable $path;

    ::update idletasks;
    set ca $path.canvas;
    set fr $ca.frame;

    set xoff 0;
    set yoff 0;

    set he 1000;
    set wi 1000;

    set child [get-widget $path];
    if {[winfo exists $child]} {
	set he [winfo reqheight $child];
	set wi [winfo reqwidth $child];
	set childid [$ca find withtag $fr];

	if {[info exists ${path}(fill)]} {
	    
	    # should the cointained widget follow in one direction ?
	    
	    switch [set ${path}(fill)] {
		"x" {

		    set xoff [lindex [$ca coords $fr] 0];
		    set wi [expr {[winfo width $ca] - $xoff}];
		    $ca itemconfigure $childid -width $wi;
		}
		"y" {
		    set yoff [lindex [$ca coords $fr] 1];
		    set he [expr {[winfo height $ca] - $yoff}];
		    $ca itemconfigure $childid -height $he;
		}
		default {

		    # New introduced, propagation also for
		    # non extend mode!
		    #$ca itemconfigure $childid -width $wi -height $he;
		}
	    }
	} else {
	    #$ca itemconfigure $childid -width $wi -height $he;
	}
    }
    $ca configure -scrollregion "$xoff $yoff $wi $he";
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------








