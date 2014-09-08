# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary Symbutton combine text and image
# -----------------------------------------------------------------------
package provide rtl_symbutton 1.2;
package require rtl;

####VG-START####
set rtl_symbutton(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_symbutton; 
	set src [file join $rtl_symbutton(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_symbutton.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_symbutton 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_symbutton {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .image 2 .text]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  appearance appearance highlightbackground [list \
        [list 0 highlightbackground]] state [list {2 state} {1 state}] \
        height {2 height} .rtl_symbutton,invoke {} anchor {2 anchor} \
        imagewidth {1 height} textvariable {2 textvariable} image [list \
        [list 1 image]] highlightthickness {0 highlightthickness} text [list \
        [list 2 text]] original frame justify {2 justify} default,side \
        left attributes [list background foreground borderwidth\
        highlightthickness highlightcolor highlightbackground\
        activebackground activeforeground text textvariable image padx\
        pady width height imagewidth state relief side appearance\
        command font anchor takefocus justify] font {2 font} highlightcolor [list \
        [list 0 highlightcolor]] background [list [list 2 background]\
        [list 1 background] [list 0 background]] width {2 width} \
        default,appearance [list image text] foreground {2 foreground}\
	takefocus {1 takefocus}\
        activebackground\
        [list {2 activebackground} {1 activebackground}] padx [list \
        [list 2 padx] [list 1 padx]] side side default,list .rtl_symbutton \
        pady [list {2 pady} {1 pady}] borderwidth {0 borderwidth}\
        relief {0 relief} command command activeforeground [list \
        [list 2 activeforeground] [list 1 activeforeground]] \
        .rtl_symbutton,creator frame]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_symbutton - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -background
#             : -foreground
#             : -borderwidth
#             : -highlightthickness
#             : -highlightcolor
#             : -highlightbackground
#             : -activebackground
#             : -activeforeground
#             : -text
#             : -textvariable
#             : -image
#             : -padx
#             : -pady
#             : -width
#             : -height
#             : -imagewidth
#             : -state
#             : -relief
#             : -side
#             : -appearance
#             : -command
#             : -font
#             : -anchor
#             : -justify 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_symbutton 
# --------------------------------------------------------------------------- 
proc rtl_symbutton {{base .rtl_symbutton} args} {
 
    variable rtl_symbutton::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_symbutton $base$w(0) $args] $base\
		-borderwidth "2" -highlightthickness "1" -relief\
		raised -class Rtl_symbutton;

	if {[opget $base win32button 0] == 1} {
	    bindtags $base$w(0) [list $base$w(0) Win32Button\
		    Rtl_symbutton Invoke\
		    Button [winfo toplevel $base] all];
	} else {
	    bindtags $base$w(0) [list $base$w(0) Rtl_symbutton\
		    Invoke Button [winfo\
		    toplevel $base] all];
	}

        # Childs to $base$w(0), rtl_symbutton of class: 	Rtl_Symbutton
        button $base$w(1) -relief flat -highlightthickness "0";
        bindtags $base$w(1) [list $base$w(1) Breakbutton [winfo\
		toplevel $base] all];
        button $base$w(2) -takefocus 0 -relief flat -highlightthickness "0";
        bindtags $base$w(2) [list $base$w(1) Breakbutton\
		[winfo toplevel $base] all];

	pack propagate $base 0;
        pack $base$w(1) -anchor center -expand 0 -fill none -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side top;
        pack $base$w(2) -anchor center -expand 0 -fill none -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side top; 
        catch {rtl_symbutton::init $base;} 
        set base [mkProc $base rtl_symbutton $args];  
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
set rat(WIDGETCLASS,rtl_symbutton) rtl_symbutton;
# ---------------------------------------------------------------------------

####VG-STOP####


namespace eval rtl_symbutton {
    bind Breakbutton <1> \
	    "event generate \[winfo parent %W\] <1> -state %s";
    bind Breakbutton <ButtonRelease-1> \
	    "event generate \[winfo parent %W\]\
	    <ButtonRelease-1> -state %s";
    bind Breakbutton <Leave> \
	    "break";
    bind Invokebutton <1> "%W invoke";
    bind Rtl_symbutton <Configure> "rtl_symbutton::pending %W update";
    image create bitmap nil;
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for widgets of class symbutton
# ---------------------------------------------------------------------------
proc rtl_symbutton::init {path} {
    variable var;

    set id [lsearch -exact $var(default,list) $path];
    if {$id < 0} {
	lappend var(default,list) $path;
    }
}


# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Destructor for widgets of class symbutton
# ---------------------------------------------------------------------------
proc rtl_symbutton::destroy {base} {
    variable var;
    
    # Remove base from class list
    set id [lsearch -exact $var(default,list) $base];
    if {$id < 0} {
	lappend var(default,list) $base;
    }
}

proc rtl_symbutton::pending {base type} {
    variable $base;

    if {![info exists ${base}(pending,$type)]} {
	set  ${base}(pending,$type) 1;
	after idle "$base $type; unset rtl_symbutton::${base}(pending,$type)";
    }
}

# ===========================================================================
# Set/Get Wrapper Methods:
# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::set-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Private set method used as a wrapper around the
#               public set method.
# ---------------------------------------------------------------------------
proc rtl_symbutton::set-action {base typ value} {
    return [::set-action rtl_symbutton $base $typ $value];
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::get-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Private get method used as a wrapper around the
#               public get method.
# ---------------------------------------------------------------------------
proc rtl_symbutton::get-action {base typ} {
    variable $base;
    variable var;
    
    if {[info exists ${base}($typ)]} {
	set val [::get-action rtl_symbutton $base $typ];
    } elseif  {[info exists var(default,$typ)]} {
	set val $var(default,$typ);
    }

    return $val;
}

# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_symbutton invoke var {side left} {appearance image};

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::set-appearance - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set the appearance for this particular widget
# ---------------------------------------------------------------------------
proc rtl_symbutton::set-appearance {path arg} {
    switch $arg {
	"image" -
	"text" -
	"image text" -
	"text image" {}
	default {
	    error "option -side: must be image,text or image text";
	}
    }
    appear $path $arg;
    set-action $path appearance $arg;
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::set-side - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set side orientation for this particular widget
# ---------------------------------------------------------------------------
proc rtl_symbutton::set-side {path arg} {
    variable var;
    switch $arg {
	"top" -
	"left" -
	"right" -
	"bottom" {}
	default {
	    error "option -side: must be top left right or bottom";
	}
    }
    set-action $path side $arg;
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::get-command - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Wrapper for invoke attribute (outside -command)
# ---------------------------------------------------------------------------
proc rtl_symbutton::get-command {path} {
    return [get-invoke $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::set-command - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Wrapper for invoke attribute (outside -command)
# ---------------------------------------------------------------------------
proc rtl_symbutton::set-command {path arg} {
    set-invoke $path $arg;
}


# ===========================================================================
# Symbutton class related settings:
# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::class-default - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_symbutton::class-default {element args} {
    variable var;
    set id 1;
    if {![winfo exists [lindex $args 0]]} {
	set id 0;
    }
    set var(default,$element) [lindex $args $id];
    set w_list [list];
    if {[info exists var(default,list)]} {
	foreach child $var(default,list) {
	    if {[winfo exists $child]} {
		$child update;
		lappend w_list $child;
	    }
	}
	set var(default,list) $w_list;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::all - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_symbutton::all {args} {
    variable var;
    if {[info exists var(default,list)]} {
	foreach child $var(default,list) {
	    ::eval {$child configure} $args;
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::appearance - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Apply appearance to the complete class
# ---------------------------------------------------------------------------
proc rtl_symbutton::appearance {args} {
    ::eval "class-default appearance $args";
}
# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::side - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set class default value for option side
# ---------------------------------------------------------------------------
proc rtl_symbutton::side {args} {
    ::eval "class-default side $args";
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::appear - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_symbutton::appear {path appear} {
    variable var;
    variable $path;

    set side [get-action $path side];

    foreach child [pack slaves $path] {
	pack forget $child;
    }

    set xs 0;
    set ys 0;

    foreach name $appear {
	set wname $path.$name;
	if {$name == "image"} {
	    pack $wname -side $side -fill both;
	} else {
	    pack $wname -side $side -expand 1 -fill both;
	}
	switch $side {
	    "top" -
	    "bottom" {
		set ws [winfo reqheight $wname];
		incr ys $ws;
		set ms [winfo reqwidth $wname];
		if {$ms > $xs} {
		    set xs $ms;
		}
	    }
	    "left" -
	    "right" {
		set ms [winfo reqheight $wname];
		set ws [winfo reqwidth $wname];
		incr xs $ws;
		if {$ms > $ys} {
		    set ys $ms;
		}
	    }
	}
	${path}_# configure -width [incr xs 8] -height [incr ys 8];
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::invoke - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Start associated command
# ---------------------------------------------------------------------------
proc rtl_symbutton::invoke {path} {
    ::eval [get-invoke $path];
}
 
# ---------------------------------------------------------------------------
# Function    : rtl_symbutton::update - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_symbutton::update {path} {
    set appear [get-action $path appearance];
    appear $path $appear;
}


# ===========================================================================
# Tasks:
# 
# [1] Check Focus function for Symbutton.
# [2] Maybe also bind invoke on button release.
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

