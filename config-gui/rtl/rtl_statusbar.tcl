# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary Statusbar Implementation
# -----------------------------------------------------------------------
package provide rtl_statusbar 1.2;
package require rtl;

####VG-START####
set rtl_statusbar(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_statusbar; 
	set src [file join $rtl_statusbar(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_statusbar.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_statusbar 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_statusbar {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .label 2 .canvas]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list \
	    original frame \
	    attributes [list borderwidth relief text label labelwidth foreground\
	    background highlightbackground highlightcolor highlightthickness \
	    height width] \
	    highlightbackground {0 highlightbackground} \
	    height {0 height} \
	    highlightthickness [list [list 0 highlightthickness]] \
	    highlightcolor [list [list 0 highlightcolor]] \
	    width {0 width} \
	    background [list [list 0 background] [list 2 background] [list 1 background]] \
	    borderwidth {0 borderwidth} \
	    foreground [list {1 foreground} foreground]\
	    label {1 text}\
	    labelwidth {1 width}\
	    relief [list [list 0 relief]] \
	    text text]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_statusbar - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -borderwidth
#             : -relief
#             : -text
#             : -background
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -height
#             : -width 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_statusbar 
# --------------------------------------------------------------------------- 
proc rtl_statusbar {{base .rtl_statusbar} args} {
 
    variable rtl_statusbar::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_statusbar $base$w(0) $args] $base\
		-class Rtl_statusbar;

        # Childs to $base$w(0), rtl_statusbar of class: 	Rtl_Statusbar
        label $base$w(1) -width "1";
        canvas $base$w(2) -closeenough "1.0" -height "0"\
		-highlightthickness 0 -width "120";

        # graphic children of $base$w(2):
        if {[catch {

	    # NOTE: the opget selectBackground code is handcoded:

	    $base$w(2) create rectangle -10.0 -10.0 -10.0 -10.0\
		-fill [opget $base selectBackground green]\
		-outline {} -tags progress;
	    
	    $base$w(2) create text 5.0 3.0  -anchor {nw}\
		-fill [opget $base foreground black] \
		-tags status;

	    $base$w(2) create text 5.0 3.0  -anchor {nw}\
		    -fill {red} -tags {error}
 
        } err]} {
            puts stderr $err;
        }

        pack $base$w(1) -fill y -side left;
        pack $base$w(2) -expand 1 -fill both -side right; 
        set base [mkProc $base rtl_statusbar $args];  
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
set rat(WIDGETCLASS,rtl_statusbar) rtl_statusbar;
# ---------------------------------------------------------------------------

####VG-STOP####

option add *Rtl_statusbar.selectBackground blue2;

# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_statusbar {stepw 0} {width 0} {height 20}\
	text {max 0.0};

# ===========================================================================
# Overwritten Generic Attributes:
# ---------------------------------------------------------------------------
proc rtl_statusbar::set-text {base value} {
    variable w;
    $base$w(2) itemconfigure status -text $value;
    return [::set-action rtl_statusbar $base text $value];
}

proc rtl_statusbar::set-foreground {base value} {
    variable w;
    $base$w(2) itemconfigure status -fill $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_statusbar::progress - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Progress bar flash
# ---------------------------------------------------------------------------
proc rtl_statusbar::progress {base typ {value 0}} {
    variable w;
    
    set c $base$w(2);
    switch $typ {
	"init" {
	    if {$value > 0} {
		set-max $base ${value}.0;
		set-width $base [winfo width $c];
		set-height $base [winfo height $c];
		$c coords progress 0 0 1 [get-height $base];

		set-stepw $base\
			[expr {[get-width $base] / [get-max $base]}];

		after 50 "$c coords progress -10 -10 -10 -10";
	    }
	}
	"hide" {
	    $c coords progress -10 -10 -10 -10;
	}
	"set" {
	    set wc [expr {$value * [get-stepw $base]}];
	    $c coords progress 0 0 $wc [get-height $base];
	}
    }
} 

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

