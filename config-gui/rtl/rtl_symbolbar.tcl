# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH, Wiesbaden
#               1999-2000 Patzschke + Rasp Software AG 
# 
# Description: RunTimeLibrary Symbolbar Implementation.
# -----------------------------------------------------------------------
package provide rtl_symbolbar 1.2;
package require rtl;

####VG-START####
set rtl_symbolbar(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_symbolbar; 
	set src [file join $rtl_symbolbar(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_symbolbar.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_symbolbar 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_symbolbar {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .gridwin 2 .gridwin.text 3 .gridwin.vert]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  troughcolor {1 troughcolor} highlightbackground [list \
        [list 0 highlightbackground]] \
        highlightthickness {0 highlightthickness} original frame \
        attributes [list background relief borderwidth wrap items commands\
        height highlightbackground highlightcolor highlightthickness width\
        troughcolor] highlightcolor {0 highlightcolor} background [list \
        [list 0 background] [list 2 background] [list 1 background]] \
        width {0 width} height {0 height} wrap {2 wrap} items items commands commands \
        borderwidth {0 borderwidth} relief {0 relief}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_symbolbar - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -background
#             : -relief
#             : -borderwidth
#             : -wrap
#             : -items
#             : -commands
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -width
#             : -height
#             : -troughcolor 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_symbolbar 
# --------------------------------------------------------------------------- 
proc rtl_symbolbar {{base .rtl_symbolbar} args} {
 
    variable rtl_symbolbar::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_symbolbar $base$w(0) $args] $base\
		-class Rtl_symbolbar -relief "raised" -height 34\
		-width 200;

        # Childs to $base$w(0), rtl_symbolbar of class: 	Rtl_Symbolbar
        rtl_gridwin $base$w(1) -widget "$base$w(2)" -auto "1" -fill "y" -sides\
            right -width 34 -height 34 -borderwidth 0 -highlightthickness 0;
        # Managed children to $base$w(1):

        text $base$w(2) -borderwidth "0" -cursor "top_left_arrow" \
            -exportselection 0 -height "1" \
            -highlightthickness 0 -state "disabled" -width\
            1 -wrap "none" -yscrollcommand\
            "rslider $base$w(2) $base$w(3) y 1; $base$w(3) set";
        # textentries of $base$w(2):

        grid $base$w(1) -column 0 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;

        grid propagate $base 0;
        # grid $base$w(0):1 1
        grid columnconfigure $base$w(0) 0 -weight 1;
        grid rowconfigure $base$w(0) 0 -weight 1;

        catch {rtl_symbolbar::init $base;} 
        set base [mkProc $base rtl_symbolbar $args]; 
        $base$w(1) update; 
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
set rat(WIDGETCLASS,rtl_symbolbar) rtl_symbolbar;
# ---------------------------------------------------------------------------

####VG-STOP####

option add *Rtl_symbolbar.borderWidth 0;
option add *Rtl_symbolbar.highlightThickness 0;

# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Constructor for widgets of class symbolbar, created
#               with the rtl_symbolbar template.
# ---------------------------------------------------------------------------
proc rtl_symbolbar::init {base} {
    variable $base;

    if {[info exists $base]} {
	unset $base;
    }
}


# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_symbolbar commands items {flag 0}\


# ===========================================================================
# Overwritten Generic Attributes (set-methods):
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::set-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Local set-action with automatic update function
# ---------------------------------------------------------------------------
proc rtl_symbolbar::set-action {base cmd value} {

    ::set-action rtl_symbolbar $base $cmd $value;
    if {![get-flag $base]} {
	set-flag $base 1;
	after idle $base update;
    }

}

# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::set-items - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Create button toolbar from menu items
# ---------------------------------------------------------------------------
proc rtl_symbolbar::set-items {base values} {
    set-action $base items $values;
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::set-commands - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Create button toolbar from owner defined actions
# ---------------------------------------------------------------------------
proc rtl_symbolbar::set-commands {base values} {
    set-action $base commands $values;
}

# ===========================================================================
# Public Methods:
# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::popup - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Dipslay symbolbar context menu
# ---------------------------------------------------------------------------
proc rtl_symbolbar::popup {base widget m} {
    set x [winfo rootx $widget];
    set y [winfo rooty $widget];
    tk_popup $m [incr x 10] [incr y 10];
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbolbar::update - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Refresh widget on Screen
# ---------------------------------------------------------------------------
proc rtl_symbolbar::update {base} {
    variable w;

    $base$w(2) configure -state normal;

    $base$w(2) delete 0.0 end;

#    foreach child [winfo children $base$w(2)] {
#	destroy $child;
#    }

    set nb 0;
    set resname [string range $base$w(2) 1 end];
    set values [get-items $base];

    set itemtext [opget $base itemtext\
	    "Action created from menuitem %s of type %s|%s (%s)"];

    # Create menu items ...
    foreach item $values {
	set m [lindex $item 0];

	if {![winfo exists $m]} {
	    continue;
	}

	set index [lindex $item 1];
	set lg [$m index last];
	if {$index > $lg} {
	    continue;
	}

	set args [lindex $item 2];
	set typ [$m type $index];
	set widget $base$w(2).child_$nb;
	switch $typ {
	    "cascade" {
		set label [$m entrycget $index -label];
		set menu [$m entrycget $index -menu];
		set cmd "rtl_symbolbar::popup $base $widget $menu";
		::eval {button $widget -text $label -command $cmd} $args;
	    }
	    "separator" {
		$base$w(2) insert end "  ";
		continue;
	    }
	    "radio" -
	    "radiobutton" {
		set label [$m entrycget $index -label];
		set variable [$m entrycget $index -variable];
		set value [$m entrycget $index -value];
		set cmd [$m entrycget $index -command];
		::eval {radiobutton $widget -text $label -variable $variable\
			-value $value -command $cmd} $args;
	    }
	    "check" -
	    "checkbutton" {
		set label [$m entrycget $index -label];
		set variable [$m entrycget $index -variable];
		set on [$m entrycget $index -onvalue];
		set off [$m entrycget $index -offvalue];
		set cmd [$m entrycget $index -command];
		::eval {checkbutton $widget -text $label -variable $variable\
			-onvalue $on -offvalue $off -command $cmd} $args;
	    }
	    "command" {
		set label [$m entrycget $index -label];
		set cmd "$m invoke \[$m index \"$label\"\]";
		::eval {button $widget -text $label -command $cmd} $args;
	    }
	}
	
	set label [$widget cget -text];
	option add *$resname.child_$nb.help\
		[::format $itemtext $label $typ $label $typ];
	incr nb;
	if {[opget $widget win32button 0] == 1} {
	    bindtags $widget [linsert [bindtags $widget] 1\
		    Win32Button];
	}
	$base$w(2) window create end -window $widget -stretch 1;
    }


    # Create commands ...
    #set nb 0;
    set values [get-commands $base];
    set cmdtext [opget $base cmdtext "Action created with %s|%s"];

    foreach item $values {
	if {[catch {

	    set widget [::eval [format $item $base$w(2).command_$nb]];
	    set label [$widget cget -text];
	    set help [::format $cmdtext $item $label $label]
	    option add *$resname.command_$nb.help [opget $widget help $help];
	    incr nb;
	    $base$w(2) window create end -window $widget -stretch 1;
	} msg]} {
	    puts stderr "ERROR : $msg";
	}
    }

    $base$w(2) configure -state disabled;
    set-flag $base 0;
}


bind Win32Button <Enter> [list %W configure -relief raised];
bind Win32Button <Leave> [list %W configure -relief flat];
bind Win32Button <FocusIn> [list %W configure -relief raised];
bind Win32Button <FocusOut> [list %W configure -relief flat];

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

