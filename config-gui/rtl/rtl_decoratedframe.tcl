#!/bin/sh 
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 2000 Patzschke + Rasp Software AG
# 
# Runtime Library Decorated Frame.
# -----------------------------------------------------------------------
# start with wish8.2 \
	exec wish8.2 "$0" ${1+"$@"}

package provide rtl_decoratedFrame 1.2;
package require rtl;

# VisualGIPSY 2.5
####VG-START####
####VG-CODE:normal####

# ---------------------------------------------------------------------------
# Template    : rtl_decoratedFrame
# 
# Attributes  :

#             -lwidth
#             -text
#             -relief
#             -underline
#             -textvariable
#             -anchor
#             -font
#             -image
#             -borderrelief
#             -width
#             -height
# ---------------------------------------------------------------------------
namespace eval rtl_decoratedFrame {

    # Define the built-in arrays
    variable w;
    variable var;

    # Widget array for template rtl_decoratedFrame
    array set w [list 0 {} 1 .framedecoration 2 .framedecoration.inframe\
	3 .labeldecoration];

    # -----------------------------------------------------------------------
    # The template interface of rtl_decoratedFrame
    array set var [list  height {0 height} anchor {3 anchor} \
	    image {3 image} textvariable {3 textvariable} lwidth [list \
	    [list 3 width]] borderrelief {1 borderwidth} original \
	    frame text {3 text} attributes [list lwidth text relief\
	    underline textvariable anchor font image borderrelief\
	    width height] font {3 font} width {0 width} underline [list \
	    [list 3 underline]] relief {1 relief}];

}

# ---------------------------------------------------------------------------
# Function    : rtl_decoratedFrame - template constructor
# 
# Returns     : 
# Parameters  : base - widget pathName
#               args - optional creation arguments, the option value
#                      pairs, stored in args, must corrospond to the
#                      attributes listed above the template
#                      definition.  In addition, the option -creator 
#                      is allways allowed. 
# 
# Description : This is the primary Constructor for widgets of class
#               rtl_decoratedFrame. The Constructor may be used as each other
#               widget class.
# ---------------------------------------------------------------------------
proc rtl_decoratedFrame {{base .rtl_decoratedFrame} args} {
    variable ::rtl_decoratedFrame::w;
    variable ::rtl_decoratedFrame::var;

    if {![winfo exists $base]} {

	# -------------------------------------------------------------------
	# The widget hierarchy
	# -------------------------------------------------------------------
	# Look for creation class of template rtl_decoratedFrame:
	[getCreator rtl_decoratedFrame $base$w(0) $args] $base$w(0) -class Rtl_decoratedFrame\
		 -height "100" -width "100";;


	# Children of $base$w(0) := rtl_decoratedFrame (level:2)
	frame $base$w(1) -borderwidth "2" -height "40"\
		-relief "groove" -width "40";

	# Children of $base$w(1) := framedecoration (level:3)
	frame $base$w(2) -height "40" -width "40";

	place $base$w(2) -x 5 -y 15 -width -10 -relwidth 1 -height -20 -relheight 1;
	label $base$w(3) -anchor "w" -borderwidth "0" -padx "3";

	place $base$w(1) -x 5 -y 5 -width -10 -relwidth 1 -height -10 -relheight 1;
	place $base$w(3) -x 25 -y 0;

	# -------------------------------------------------------------------
	# Call the init method
	if {[catch {
	    ::rtl_decoratedFrame::init $base;
	} msg]} {
	    # init does not exist.
	}

	# Additional Code
	

	# -------------------------------------------------------------------
	# Use the template interface for this instance
	set base [mkProc $base rtl_decoratedFrame $args];
    }

    return $base;
}


# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet!
# ---------------------------------------------------------------------------


namespace eval rtl_decoratedFrame {
 
    # option database items to template rtl_decoratedFrame

    option add *Rtl_decoratedFrame$w(3).text {};
}


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,rtl_decoratedFrame) rtl_decoratedFrame;
# Local used Packages:
set ::rat(PACKAGES,LOADED) [list ];
# ---------------------------------------------------------------------------

####VG-STOP####
