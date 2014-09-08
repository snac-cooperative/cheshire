# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary GridWin implementation
# -----------------------------------------------------------------------
package provide rtl_gridwin 1.2;
package require rtl;

####VG-START####
set rtl_gridwin(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_gridwin; 
	set src [file join $rtl_gridwin(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set rat(WIDGETCLASS,rtl_gridwin) frame;
# ---------------------------------------------------------------------------

# --------------------------------------------------------------------------- 
# class (namespace): rtl_gridwin 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_gridwin {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .vert 2 .hori]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  troughcolor\
        [list {1 troughcolor} {2 troughcolor}] repeatdelay [list \
        [list 1 repeatdelay] [list 2 repeatdelay]] elementborderwidth [list \
        [list 1 elementborderwidth] [list 2 elementborderwidth]] \
        highlightbackground [list [list 1 highlightbackground]\
        [list 2 highlightbackground] [list 0 highlightbackground]]\
        height {0 height} highlightthickness {0 highlightthickness} \
        original frame scrollrelief [list {1 relief} {2 relief}] \
        .rtl_gridwin,creator frame attributes [list widget auto fill sides\
	    xscrollstartcommand yscrollstartcommand\
	xscrollstopcommand yscrollstopcommand width height background\
        highlightbackground highlightcolor highlightthickness relief\
        borderwidth activebackground scrollborderwidth\
        elementborderwidth scrollhighlightthickness scrollrelief\
        repeatdelay repeatinterval troughcolor scrollwidth] scrollwidth [list \
        [list 1 width] [list 2 width]] highlightcolor [list \
        [list 1 highlightcolor] [list 2 highlightcolor]\
        [list 0 highlightcolor]] background\
	[list [list 0 background]] width {0 width} \
        scrollhighlightthickness [list [list 1 highlightthickness]\
        [list 2 highlightthickness]] activebackground [list \
        [list 1 activebackground] [list 2 activebackground]] \
        scrollborderwidth [list {1 borderwidth} {2 borderwidth}] \
        borderwidth {0 borderwidth} repeatinterval [list \
        [list 1 repeatinterval] [list 2 repeatinterval]] relief [list \
        [list 0 relief]] auto auto widget widget sides sides fill\
	fill xscrollstartcommand xscrollstartcommand yscrollstartcommand\
	yscrollstartcommand xscrollstopcommand xscrollstopcommand\
	yscrollstopcommand yscrollstopcommand];
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_gridwin - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -auto
#             : -widget
#             : -sides
#             : -fill
#             : -width
#             : -height
#             : -background
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -relief
#             : -borderwidth
#             : -activebackground
#             : -scrollborderwidth
#             : -elementborderwidth
#             : -scrollhighlightthickness
#             : -scrollrelief
#             : -repeatdelay
#             : -repeatinterval
#             : -troughcolor
#             : -scrollwidth 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_gridwin 
# --------------------------------------------------------------------------- 
proc rtl_gridwin {{base .rtl_gridwin} args} {
 
    variable rtl_gridwin::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_gridwin $base$w(0) $args] $base -class Rtl_gridwin;

        # Childs to $base$w(0), rtl_gridwin of class: 	Rtl_Gridwin
        scrollbar $base$w(1);
        scrollbar $base$w(2) -orient "horizontal";
	#grid propagate $base 0;

	foreach orient [list x y] id [list 2 1] {
#           bind $base$w($id) <ButtonPress> \
#                   "eval \{eval \"$base cget\
#                   -${orient}scrollstartcommand\"\};";
#           bind $base$w($id) <ButtonRelease> \
#                   "eval \{eval \"$base cget\
#                   -${orient}scrollstopcommand\"\};";
            bind $base$w($id) <ButtonPress> \
                    "eval \[$base cget\
                    -${orient}scrollstartcommand\];";
            bind $base$w($id) <ButtonRelease> \
                    "eval \[$base cget\
                    -${orient}scrollstopcommand\];";

	}

        # Geometry section:	grid
        #grid $base$w(1) -column 2 -row 1 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky ns;
        #grid $base$w(2) -column 1 -row 2 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky ns;
        # grid $base$w(0):3 3
        grid columnconfigure $base$w(0) 1 -weight 1;
        grid rowconfigure $base$w(0) 1 -weight 1; 
        
	set base [mkProc $base rtl_gridwin $args];

	grid propagate $base 0;
        $base$w(0) update; 
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------
####VG-STOP####

# ===========================================================================
# Public methods:
# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::remove - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Remove scrollactions from current widget and also
#               unmanage that widget.
# ---------------------------------------------------------------------------
proc rtl_gridwin::remove {path} {
    variable var;

    set widg [get-widget $path];
    if {[winfo exists $widg]} {

	if {[catch {
	    $widg configure -xscrollcommand "" -yscrollcommand "";
	} msg]} {
	    puts stderr "Error removing old widget: $msg";
	}

	grid forget $widg;
	::set-action rtl_gridwin $path widget {};
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::update - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Refresh and remanage current widget
# ---------------------------------------------------------------------------
proc rtl_gridwin::update {path args} {
    set-widget $path [get-widget $path];
}

# ===========================================================================
# Generic attributes:
# ---------------------------------------------------------------------------
create-action rtl_gridwin\
	widget {auto 1} {sides {bottom right}} {fill both}\
	xscrollstartcommand yscrollstartcommand\
	xscrollstopcommand yscrollstopcommand;

# ===========================================================================
# Redefined generic set-attributes:
# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::set-widget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Make given widget managed by the gridwin container
# ---------------------------------------------------------------------------
proc rtl_gridwin::set-widget {path arg} {
    variable var;
    remove $path;
    ::set-action rtl_gridwin $path widget $arg;
    if {[winfo exists $arg]} {
	grid $arg -in $path -column 1 -row 1 -sticky news;
	set-fill $path [get-fill $path];
	raise $arg;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::set-auto - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set auto scrollbar insertion for this instance
# ---------------------------------------------------------------------------
proc rtl_gridwin::set-auto {path arg} {
    switch $arg {
	"yes" {set arg 1}
	"no" {set arg 0}
	 1 -
	 0 -
	"y" -
	"x" {}
	default {
	    error "option must be 1, 0, or yes, no; x, y";
	}
    }
    ::set-action rtl_gridwin $path auto $arg;
}


# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::set-sides - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set where the scrollbars are inserted
# ---------------------------------------------------------------------------
proc rtl_gridwin::set-sides {path arg} {

    grid forget $path.vert $path.hori;
    
    set result {};
    foreach side $arg {
	switch $side {
	    "top" {
		append result "grid $path.hori -column 1 -row 0\
			-sticky we; grid remove $path.hori;";
	    }
	    "bottom" {
		append result "grid $path.hori -column 1 -row 2\
			-sticky we; grid remove $path.hori;";
	    }
	    "left" {
		append result "grid $path.vert -column 0\
			-row 1 -sticky ns; grid remove $path.vert;";
	    }
	    "right" {
		append result "grid $path.vert -column 2 -row 1\
			-sticky ns; grid remove $path.vert;";
	    }
	    default {
		error "option -side must be top, bottom, left, right";
	    }
	}
    }
    after idle $result;
    ::set-action rtl_gridwin $path sides $arg;
}


# ---------------------------------------------------------------------------
# Function    : rtl_gridwin::set-fill - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set scrollbar fill insertion mode
# ---------------------------------------------------------------------------
proc rtl_gridwin::set-fill {path arg} {

    foreach sbar [list vert hori] {
	$path.$sbar configure -command "";
    }

    set widg [get-widget $path];
    set auto [get-auto $path];

    if {[winfo exists $widg]} {

	foreach {sbar orie} [list hori x vert y] {
	    set $sbar [list $widg ${orie}view];
	    set ${orie}scroll "rslider $widg $path.$sbar $orie\
		    $auto; $path.$sbar set";
	}
	    
	switch $arg {
	    "x" {
		
		set vert {};
		set yscroll {};
	    }
	    "y" {
		set hori {};
		set xscroll {};
	    }
	    "both" {}
	    "none" {
		set vert {};
		set yscroll {};
		set hori {};
		set xscroll {};
	    }
	    default {
		error "option -fill must be x, y, both, none";
	    }
	}
	
	$path.hori configure -command $hori;
	$path.vert configure -command $vert;
	$widg configure\
		-yscrollcommand $yscroll -xscrollcommand $xscroll;
    }

    ::set-action rtl_gridwin $path fill $arg;
    set-sides $path [get-sides $path];
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 14.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

