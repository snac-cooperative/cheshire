# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary tooltip Implementation
# -----------------------------------------------------------------------
package provide rtl_tooltip 1.2;
package require rtl;

namespace eval rtl_tooltip {
    variable var;
    array set var {tip {} xoff 15 yoff 5};

    namespace export tooltip;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::get-tip - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Extract tooltip from help entry
# ---------------------------------------------------------------------------
proc rtl_tooltip::get-tip {txt {index 1} {ch |}} {
    return [get-help $txt $index $ch];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::get-help - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get Tooltip or help message from help entry 
# ---------------------------------------------------------------------------
proc rtl_tooltip::get-help {txt {index 0} {ch |}} {
    return [lindex [split $txt $ch] $index];
}


# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::status - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Give help message to external status procedure
# ---------------------------------------------------------------------------
proc rtl_tooltip::status {txt} {
    if {[string compare "status" [info proc status]] == 0} {
	::status [get-help $txt];
    }
}


# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::remove - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Unmap tooltip widget
# ---------------------------------------------------------------------------
proc rtl_tooltip::remove {} {
    variable var;
    if {![info exists var(ignore,leave)]} {
	if {[info exists var(active,tip)]} {
	    unset var(active,tip);
	}
	unmap .tooltip;

    } else {
	unset var(ignore,leave);
    }
    if {[info exists var(active,remove)]} {
	unset var(active,remove);
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::leave - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Popup tooltip widget after 200ms, when leaving
#               a widget with an associated tooltip.
# ---------------------------------------------------------------------------
proc rtl_tooltip::leave {w args} {
    variable var;
    if {[winfo exists .tooltip]} {
	if {![info exists var(active,remove)]} {
	    after [opget $w leaveTime 200] {
		    rtl_tooltip::remove;
	    }
	    set var(active,remove) 1;
	}
    }
}
    
# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::tooltip - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Creator of tooltip widget.
#               Note each Application does own one single tooltip
#               widget.
# ---------------------------------------------------------------------------
proc rtl_tooltip::tooltip {{screen {}}} {
    variable var;
    
    set var(sx) [expr {[winfo screenwidth .] - 5}];
    set var(sy) [expr {[winfo screenheight .] - 5}];
    
    destroy .tooltip;

    # Use screen to create a tooltip.
    toplevel .tooltip -class Tooltip -bd 1\
	    -takefocus 0\
	    -screen $screen\
	    -background black;

    wm withdraw .tooltip;
    wm overrideredirect .tooltip 1;
    pack [message .tooltip.txt -textvariable rtl_tooltip::var(tip)\
	    -aspect 3000 -bg cornsilk -fg black]\
		-expand 1 -fill both;
    bindtags .tooltip [list .tooltip rtl_tooltip Toplevel];
    bindtags .tooltip.txt [list .tooltip.txt Message .tooltip];
    bind .tooltip <Enter> {rtl_tooltip::unmap .tooltip}
    bind all <Enter> {rtl_tooltip::enter %W %X %Y}
    bind all <Leave> {rtl_tooltip::leave %W}
    bind all <FocusIn> {rtl_tooltip::enter %W %X %Y}
    bind all <FocusOut> {rtl_tooltip::leave %W}

}

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::unmap - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : hide tooltip widget
# ---------------------------------------------------------------------------
proc rtl_tooltip::unmap {w} {
    if {[winfo exists $w]} {
	wm withdraw $w;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::statusline - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : NOP
# ---------------------------------------------------------------------------
proc rtl_tooltip::statusline {path args} {
    uplevel 1 [list error "currently not implemented"];
}

namespace inscope :: namespace import -force rtl_tooltip::tooltip;

# ---------------------------------------------------------------------------
# Function    : rtl_tooltip::enter - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : A new widget was be entered; maybe a tooltip must
#               be shown.
# ---------------------------------------------------------------------------
proc rtl_tooltip::enter {w rx ry} {
    variable var;

    set screen [winfo screen $w];
    if {[winfo exists .tooltip]} {

	set tipTop [winfo toplevel $w];

	# Check screen for tooltip and
	# Recreate for the proper screen.

	if {[string compare [winfo screen .tooltip]\
		$screen] != 0} {
	    destroy .tooltip;
	    tooltip $screen;
	}

	set txt [opget $w help ""];	
	set var(tip) [get-tip $txt];
	if {$var(tip) != ""} {
	    if {![info exists var(active,tip)]} {
		set ::rtl_tooltip::_top $tipTop;
		after [opget $w enterTime 50] {
		    set rtl_tooltip::var(active,tip) 1;
		    if {[set rtl_tooltip::var(tip)] != ""} {
			wm transient .tooltip $::rtl_tooltip::_top;
			wm deiconify .tooltip;
			raise .tooltip $::rtl_tooltip::_top;
		    }
		}
	    }

	    set var(ignore,leave) 1;
	} else {
	    if {[info exists var(active,tip)]} {
		unset var(active,tip);
	    }
	    unmap .tooltip;
	}
	
	set ww [winfo width .tooltip];
	set wh [winfo height .tooltip];
	set wrx [winfo rootx $w];
	set wry [winfo rooty $w];
	set rx [expr {$wrx +  $var(xoff)}];
	set ry [expr {$wry + [winfo height $w] + $var(yoff)}]; 
	
	if {[expr {$ww + $rx}] >= $var(sx)} {
	    set rx [expr {[winfo rootx $w] - $ww + $var(xoff)}];
	}
	
	if {[expr {$wh + $ry}] >= $var(sy)} {
	    set ry [expr {[winfo rooty $w] - $wh - $var(yoff)}];
	}
	wm geometry .tooltip +$rx+$ry;
    }

    # This is a call to the previsious namespace:
    status [opget $w help ""];
}

# ===========================================================================
# Tasks:
#
# [1] Check multi screen support
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

