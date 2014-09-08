##!/bin/sh 
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: Runtime Library Spinbox widget
# -----------------------------------------------------------------------
# start with wish \
	exec wish8.0 "$0" $@ 

# VisualGIPSY 2.0

package provide rtl_spinbox 1.2;
package require rtl;


####VG-START####
set rtl_spinbox(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_spinbox; 
	set src [file join $rtl_spinbox(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set rat(WIDGETCLASS,rtl_spinbox) frame;
# ---------------------------------------------------------------------------

# --------------------------------------------------------------------------- 
# class (namespace): rtl_spinbox 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_spinbox {
 
        variable var; 
        variable w; 
        array set w [list \
		0 {} \
		1 .f \
		2 .f.l \
		3 .f.e \
		4 .f.up \
		5 .f.down\
		]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list \
	    original frame \
	    .rtl_spinbox,creator frame \
	    frame .f \
	    label .f.l \
	    entry .f.e \
	    up .f.up\
	    down .f.down \
	    \
	    attributes [list \
	    background foreground state text borderwidth \
	    relief font width upcommand downcommand returncommand min max \
	    variable selectbackground selectforeground\
	    ]\
	    background [list {0 background} {1 background} {2 background} \
	    {4 background} {5 background} background] \
	    borderwidth [list {1 borderwidth} {4 borderwidth} {5 borderwidth}] \
	    downcommand downcommand \
	    font [list {3 font} {2 font} {5 font}] \
	    foreground [list {3 foreground} {2 foreground} \
	    {4 foreground} {5 foreground} foreground] \
	    selectbackground {3 selectbackground}\
	    selectforeground {3 selectforeground}\
	    max max \
	    min min \
	    relief {1 relief} \
	    returncommand returncommand \
	    state [list {3 state} {4 state} {5 state}] \
	    text {2 text} \
	    upcommand upcommand \
	    variable [list {3 textvariable} variable] \
	    width {3 width} \
	    ]; 
    namespace export rtl_spinbox; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_spinbox - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -background
#             : -foreground
#             : -state
#             : -text
#             : -borderwidth
#             : -relief
#             : -font
#             : -width 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_spinbox 
# --------------------------------------------------------------------------- 
proc rtl_spinbox {{base .rtl_spinbox} args} {
 
    variable rtl_spinbox::w; 
    if {![winfo exists $base]} {
 
        #create base container 
        [getCreator rtl_spinbox $base$w(0) $args] $base\
		-class Rtl_spinbox;

        # Childs to $base$w(0), rtl_spinbox of class: 	Rtl_Spinbox
        frame $base$w(1) -borderwidth "2" -relief "raised";

        # Childs to $base$w(1), f of class: 	Frame
        label $base$w(2) -borderwidth "0" -padx "0"\
		-underline [opget $base$w(2) underline {-1}];
        entry $base$w(3) -width "5";

        foreach pair [list  [list <Key-Down>\
		"rtl_spinbox::button-down $base$w(0)"]\
             [list <Key-Return> "rtl_spinbox::return-pressed $base$w(0)"]\
             [list <Key-Up> "rtl_spinbox::button-up $base$w(0)"]] {
             bind $base$w(3) [lindex $pair 0] [lindex $pair 1]; 
        }

        button $base$w(4) -command "rtl_spinbox::button-up\
		$base$w(0)" -padx "11" -pady "4"\
		-takefocus "0" -underline\
		[opget $base$w(4) underline {-1}];
        button $base$w(5) -command "rtl_spinbox::button-down\
		$base$w(0)" -padx "11"\
		-pady "4" -takefocus "0" -underline\
		[opget $base$w(5) underline {-1}];

        grid $base$w(2) -column 0 -row 0 -columnspan 1 -rowspan 2\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;
        grid $base$w(3) -column 1 -row 0 -columnspan 1 -rowspan 2\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;
        grid $base$w(4) -column 2 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;
        grid $base$w(5) -column 2 -row 1 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;

        # grid $base$w(1): # 3 2
        grid columnconfigure $base$w(1) 1 -weight 1;
        grid rowconfigure $base$w(1) 0 -weight 1;
        grid rowconfigure $base$w(1) 1 -weight 1;

        pack $base$w(1) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side top; 
        catch {rtl_spinbox::init $base;} 
        set base [mkProc $base rtl_spinbox $args];  
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
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_spinbox {value 0} {state normal} {min 0} {max 255}\
	{borderwidth 0} {width 5}\
	upcommand downcommand returncommand;

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::set-background - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set background & foreground for bitmaps
# ---------------------------------------------------------------------------
proc rtl_spinbox::set-background {path value} {
    variable var;

    if {"windows" != $::tcl_platform(platform)} {
	$path$var(entry) configure -background $value;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::set-foreground - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set background & foreground for bitmaps
# ---------------------------------------------------------------------------
proc rtl_spinbox::set-foreground {path value} {
    up($path)_xbm configure -foreground $value;
    down($path)_xbm configure -foreground $value;
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::set-variable - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::set-variable {path value} {
    upvar ::$value variable;
    catch {rtl_spinbox::insert $path $variable};
    return;
}



# ===========================================================================
# Public methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::get - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get value from entryfield
# ---------------------------------------------------------------------------
proc rtl_spinbox::get {path} {
    check-entry $path;
    return [get-value $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::insert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set the entryfield
# ---------------------------------------------------------------------------
proc rtl_spinbox::insert {path string} {
#      variable var;

#      $path$var(entry) delete 0 end;
#      $path$var(entry) insert 0 $string;
#      set var($path,value) $string;
    return [set-value $path $string];
}

# ===========================================================================
# Private methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::button-up - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Pop up a listbox if the button is pressed
# ---------------------------------------------------------------------------
proc rtl_spinbox::button-up {path} {

    # Nothing to do if box is disabled.
    if {[string compare [get-state $path] "disabled"] != 0} {
	set val [check-entry $path];
	insert $path [succ $path $val];
	check-entry $path;

	::eval [get-upcommand $path];
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::button-down - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Close the history-window
# ---------------------------------------------------------------------------
proc rtl_spinbox::button-down {path} {

    # Nothing to do if box is disabled.
    if {[string compare [get-state $path] "disabled"] != 0} {
	set val [check-entry $path];
	insert $path [pred $path $val];
	check-entry $path;
	::eval [get-downcommand $path];
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::return-pressed - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Invoked when return is pressed in the entryfield
# ---------------------------------------------------------------------------
proc rtl_spinbox::return-pressed {path} {
    check-entry $path;
    ::eval [get-returncommand $path];
    return;
}

# ===========================================================================
# Check content of spinbox:
# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::check-entry - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal used to verify the given entry field.
#               This function isn't needed anymore?
# ---------------------------------------------------------------------------
proc rtl_spinbox::check-entry {path} {
    variable var;

    set entr $path$var(entry);
    set val [$entr get];

    if {![valid-p $path $val]} {
	# Invalid input!
	if {""==$val} {
	    # ... use min as new value
	    set val [first $path];
	} else {
	    # ... keep old value;
	    set val [get-value $path];
	}

	if {![valid-p $path $val]} {

	    # Old value is invalid too, use first.
	    if {$val > [get-max $path]} {
		set val [last $path];
	    } else {
		set val [first $path];
	    }
	}
    }

    insert $path [format "%d" $val];
    return [get-value $path]; 
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::valid-p - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::valid-p {path val} {
    return [expr ![catch {format "%d" $val}] \
	    && {$val <= [get-max $path]} \
	    && {$val >= [get-min $path]}];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::succ - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::succ {path val} {
    return [expr $val + 1];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::pred - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::pred {path val} {
    return [expr $val -1];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::first - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::first {path} {
    return [get-min $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::last - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::last {path} {
    return [get-max $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::key-command - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_spinbox::key-command {path} {
    after 1 "rtl_spinbox::check-entry $path";
    return;
}

# ===========================================================================
# Construction Methods:
# ---------------------------------------------------------------------------
# Function    : rtl_spinbox::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Initialize instance specific variable
# ---------------------------------------------------------------------------
proc rtl_spinbox::init {path} {

    variable var;

    # Create images for the widget
    image create bitmap up($path)_xbm -data { \
	    #define up(path)_width 5 \
	    #define up(path)_height 5 \
	    static char up(path)_bits[] = { \
	    0x00, 0x04, 0x0e, 0x1f, 0x00}
    } -maskdata { \
	    #define up(path)_width 5 \
	    #define up(path)_height 5 \
	    static char up(path)_bits[] = { \
	    0x00, 0x04, 0x0e, 0x1f, 0x00}
    }

    image create bitmap down($path)_xbm -data { \
	    #define down(path)_width 5 \
	    #define down(path)_height 5 \
	    static char down(path)_bits[] = { \
	    0x00, 0x1f, 0x0e, 0x04, 0x00}
    } -maskdata { \
	    #define down(path)_width 5 \
	    #define down(path)_height 5 \
	    static char down(path)_bits[] = { \
	    0x00, 0x1f, 0x0e, 0x04, 0x00}
    }

    foreach item [list up down entry] {
	set $item $path$var($item);
    }

    # Configure some widgets
    $up configure -image up($path)_xbm;
    $down configure -image down($path)_xbm;
    
    $entry configure -textvariable ::rtl_spinbox::${path}(value);

    # Set the entry to minimum value
    insert $path [get-min $path];


    # Add bindings to check values during input
    bind $entry <KeyPress> "rtl_spinbox::key-command $path";
    bind $entry <ButtonRelease-2> "break";
    
    return;
}
# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

