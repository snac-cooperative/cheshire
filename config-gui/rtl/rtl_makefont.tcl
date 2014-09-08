#!/bin/sh 
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1999-2000 Patzschke + Rasp Software AG
# 
# Description: The Runtime Library Extended Font Chooser
# -----------------------------------------------------------------------
# start with WISH8.0 \
	exec wish8.0 "$0" ${1+"$@"} 

# VisualGIPSY 2.0

package provide rtl_makefont 1.2;
package require rtl;
package require rtl_choosefont;
package require rtl_combobox;
 
####VG-START####
set vgmkfont(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::vgmkfont; 
	set src [file join $vgmkfont(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# class (namespace): rtl_makeFont 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_makeFont {
 
    variable var; 
    variable w; 
    array set w [list 4 .chosser_label 0 {} 5 .update 1 .font_label 2 \
        .font_combobox 3 .font_chooser]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  highlightbackground [list \
        [list 0 highlightbackground] [list 3 highlightbackground]] style [list \
        [list 3 style]] height {0 height} size {3 size} \
        highlightthickness\
        [list {0 highlightthickness} {3 highlightthickness}] original \
        toplevel attributes [list interp variable background borderwidth height\
        highlightbackground highlightcolor highlightthickness relief\
        width foreground selectbackground selectforeground font\
        selectborderwidth name style size overstrike underline] font [list \
        [list 3 font] [list 2 font]] highlightcolor [list \
        [list 0 highlightcolor] [list 3 highlightcolor]] background [list \
        [list 0 background] [list 1 background] [list 3 background]\
        [list 4 background] [list 5 background]] width {0 width} \
        selectbackground\
        [list {3 selectbackground} {2 selectbackground}] name {3 name} \
        interp interp foreground [list {2 foreground} {3 foreground}] \
        selectforeground\
        [list {3 selectforeground} {2 selectforeground}] borderwidth [list \
        [list 0 borderwidth]] selectborderwidth {3 selectborderwidth} \
        overstrike {3 overstrike} underline {3 underline} relief [list \
        [list 0 relief]] variable {3 variable}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_makeFont - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -interp
#             : -background
#             : -borderwidth
#             : -height
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -relief
#             : -width
#             : -foreground
#             : -selectbackground
#             : -selectforeground
#             : -font
#             : -selectborderwidth
#             : -name
#             : -style
#             : -size
#             : -overstrike
#             : -underline 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_makeFont 
# --------------------------------------------------------------------------- 
proc rtl_makeFont {{base .toplevel1} args} {
 
    variable rtl_makeFont::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_makeFont $base$w(0) $args] $base\
		-class Rtl_makeFont -width 480 -height 360;

        # Childs to $base$w(0), toplevel1 of class: 	Rtl_makeFont
        label $base$w(1);
        rtl_combobox $base$w(2) -command "$base$w(0) showfonts"\
            -histposition "below" -histheight "5"\
            -histlength "-1" -histwidth "28"\
            -relief "sunken";

        rtl_chooseFont $base$w(3) -relief "raised" -height "240" -width "300"\
            -msghighlightbackground "Black"\
            -msghighlightthickness "1" -creator "frame";
        label $base$w(4);
        button $base$w(5) -command "$base$w(0) make" -padx "11" -pady "4"\
            -width "12";

	grid propagate $base 0;
        grid $base$w(1) -column 0 -row 0 -padx 3 -pady 3 -sticky nes;
        grid $base$w(2) -column 2 -row 0 -padx 3 -pady 3 -sticky nesw;
        grid $base$w(3) -column 2 -row 2 -columnspan 4 -rowspan 2\
             -padx 3 -pady 3 -sticky nesw;
        grid propagate $base$w(3) 0;
        grid $base$w(4) -column 0 -row 2 -padx 3 -pady 3 -sticky nes;
        grid $base$w(5) -column 4 -row 0 -padx 3 -pady 3 -sticky nesw;
        # grid $base$w(0):6 4
        foreach {id opt param} [list  1 minsize 6 3 minsize 6 5 weight 1] {
            grid columnconfigure $base$w(0) $id -$opt $param;
        }
        foreach {id opt param} [list  1 minsize 6 3 weight 1] {
            grid rowconfigure $base$w(0) $id -$opt $param;
        } 
        catch {rtl_makeFont::init $base;} 
        set base [mkProc $base rtl_makeFont $args];  
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------
option add *Rtl_makeFont.font_label.text {Name};
option add *Rtl_makeFont.chosser_label.text {Select};
option add *Rtl_makeFont.update.text {Create};

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,rtl_makeFont) rtl_makefont;
# ---------------------------------------------------------------------------

####VG-STOP####

# ---------------------------------------------------------------------------
# Function    : rtl_makeFont - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Constructor
# ---------------------------------------------------------------------------
proc rtl_makeFont::init {base} {
    after idle [list $base showfonts];
}

# ---------------------------------------------------------------------------
# Function    : rtl_makeFont::showfonts - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Callback used to fill the font object list
# ---------------------------------------------------------------------------
proc rtl_makeFont::showfonts {base} {
    set cb [find-widget font_combobox $base];
    $cb histclear;
    foreach font [teval $base "font names"] {
	$cb histinsert end $font;
    }
   
}

# ---------------------------------------------------------------------------
# Function    : rtl_makeFont::font - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : return/update current font. Font can be a real
#               fontname following the tk convention or a font object.
# ---------------------------------------------------------------------------
proc rtl_makeFont::getfont {base} {
    set name [get-selection $base];
    set fc [find-widget font_chooser $base];
    if {[string compare {} $name] != 0} {
	# font name was choosed !
	if {[string compare [$fc get-modified] "1"] == 0} {
	    make $base;
	}
    } else {
	set name [$fc getfont];
    }
    return $name;
}

# ---------------------------------------------------------------------------
# Function    : rtl_makeFont::make - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Create an font object, if no name was supplied
#               an automatic name is created.
# ---------------------------------------------------------------------------
proc rtl_makeFont::make {base} {
    set cb [find-widget font_combobox $base];
    set fc [find-widget font_chooser $base];
 
    set name [$cb get];
    set script {};
    foreach attr [list name style size overstrike underline] {
	set value [$fc cget -$attr];
	switch $attr {
	    "style" {
		switch $value {
		    "bold" {
			append script " -weight bold";
		    }
		    "italic" {
			append script " -slant italic";
		    }
		    "bold italic" {
			append script " -slant italic -weight bold";
		    }
		}
	    }
	    "size" {
		append script " -size $value";
	    }
	    "name" {
		append script " -family \{$value\}";
	    }
	    default {
		if {[string compare {} $value] != 0} {
		    append script " -$attr 1";
		}
	    }
	}
    }
    if {[catch {
	teval $base "font create \{$name\}$script";
	$cb histinsert end $name;
    }]} {
	teval $base "font configure \{$name\}$script";	
    }

    set v [$fc cget -variable];
    if {$v != {}} {
	set $v $name;
    }

    $fc set-modified 0;
    set-selection $base $name;
}
 
# ---------------------------------------------------------------------------
# Function    : rtl_makeFont::teval - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : central evaluation routine, is used to create
#               font objects within various tcl interpreters
# ---------------------------------------------------------------------------
proc rtl_makeFont::teval {base script} {
    set ip [get-interp $base];
    if {[string compare {} $ip] == 0} {
	::eval $script;
    } else {
	$ip eval $script;
    }
}

# # Place bindgroups here: 
# # 
# # Instanciation the User Interface: 
# # automatic generated from VisualGIPSY 2.0
# foreach file [glob /prod/vg20/vg2.0/rtl/*.tcl] {
#     catch {uplevel #0 source $file}
# }
 
# source /timoda/roger/tcl/vgfont.tcl;
# rtl_makeFont;
#         wm title .toplevel1 "toplevel1";
# wm protocol .toplevel1\
#  	WM_DELETE_WINDOW {exit};
 
create-action rtl_makeFont interp selection;

# ===========================================================================
# Tasks:
#
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 16.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

