# -----------------------------------------------------------------------
#   $Source: /projects/cheshire/CVSREPOS/cheshire/config-gui/rtl/rtl_combobox.tcl,v $
# $Revision: 1.1 $
#     $Date: 2000/10/31 21:44:52 $
#   $Author: yzhang $
#    $State: Exp $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: 
#              rtl_combobox megawidget
# 
# -----------------------------------------------------------------------
# RuntimeLibrary Release 1.2 (roger)
# Reduced time consumption and complexity
# Changed color behavior hl/sel and normal colors.
# -----------------------------------------------------------------------
# RuntimeLibrary Release 1.1 (roger)
# Removed var usage, changed naming convention.
# -----------------------------------------------------------------------
# 
# $Log: rtl_combobox.tcl,v $
# Revision 1.1  2000/10/31 21:44:52  yzhang
# *** empty log message ***
#
# Revision 1.3  1999/09/03 09:27:00  roger
# BETA II Release
#
# Revision 1.2  1999/07/21 07:41:19  roger
# Added solid border to the toplevel widget
#
# Revision 1.1.1.1  1999/07/13 15:13:13  roger
# VisualGIPSY 2.1
#
# Revision 1.1  1999/06/25 13:12:59  heiko
# Added (and Replaced) widgets. New image-widgets inserted.
#
# Revision 1.13  1999/01/09 19:47:56  heiko
# Did bugfixes on some templates.
#
# Revision 1.12  1998/10/30 14:23:58  heiko
# Variaous changes for new vg2.0 component concept.
#
# Revision 1.11  1998/09/17 15:42:45  thomas
# Close history window when new text is inserted via method 'insert'.
#
# Revision 1.10  1998/08/26 16:04:50  heiko
# Fixed rtl_combobox to work with VG.
#
# Revision 1.9  1998/08/26 07:43:49  thomas
# Added option -selectcommand.
#
# Revision 1.8  1998/08/20 10:28:06  heiko
# GUI modifications to get Windows look and feel.
#
# Revision 1.7  1998/07/31 15:20:02  heiko
# Changed default histwidth (use the entries width!).
#
# Revision 1.6  1998/07/23 13:01:44  heiko
# Changed takefocus options for some widgets.
#
# Revision 1.5  1998/07/20 16:14:12  thomas
# restored repository
#
# Revision 1.3  1998/07/10 08:09:50  heiko
# Renamed gridWin to gridwin.
#
# Revision 1.2  1998/07/10 08:07:11  heiko
# Fixed the demos.
#
# Revision 1.1  1998/07/06 14:34:50  thomas
# new
#
# -----------------------------------------------------------------------
# ---------------------------------------------------------------------------
#
# Information about rtl_combobox widget
# --------------------------------
#
# Name
#  rtl_combobox - Rtl_Combobox widget.
#
# Synopsis
#  rtl_combobox pathName ?options?
#
# Options
#  -background color
#     Color used as background for the rtl_combobox widget.
#  -foreground color
#     Color used as foreground for the rtl_combobox widget.
#  -command command
#     Command invoked when pressing the return-key inside the widgets entry and
#     when pushing the widgets button.
#  -selectcommand selectcommand
#     Command invoked when chosing an item from the history.
#  -borderwidth width
#     The width of the borders of the rtl_combobox widget.  
#  -histposition [above | below]
#     Direction where the rtl_combobox history is opened - above or below the widget.
#  -histheight
#     Height of the rtl_combobox history window.
#  -histlength
#     Maximum number of items stored in the rtl_combobox history list.
#  -histwidth
#     Width of the rtl_combobox history window. If histwidth is 0, the width of the 
#     rtl_combobox widget will be used.
#  -selectbackground
#     Selection background color used in the rtl_combobox history list.
#  -selectforeground
#     Selection foreground color used in the rtl_combobox history list.
#  -width
#     Width of the rtl_combobox entry.
#
# Widget commands
#  get
#     Query the value shown in the rtl_combobox entry.
#  insert index string
#     Insert a string in the rtl_combobox entry starting at the position specified by
#     index.
#  delete first ?last?
#     Delete one or more elements of the rtl_combobox entry
#  histget ?index?
#     Query the values shownin the rtl_combobox history.
#  histinsert index ?element element ...?
#     Insert one or more elements to the rtl_combobox history list.
#  histclear
#     Clear the rtl_combobox history list.
#
#
# RTL_COMBOBOX NEEDS LIBRARIES vgnetxt.tcl AND vgclass.tcl PROVIDED BY ROGER.
# PATH IS:  /timoda/roger/vgxtnd/
#
# ToDo:
# =====
#  * ...
#
# ---------------------------------------------------------------------------

package provide rtl_combobox 1.2;
package require rtl;
package require rtl_images 1.2;
package require rtl_gridwin;

####VG-START####
set rtl_combobox(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_combobox; 
	set src [file join $rtl_combobox(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
set rat(WIDGETPROC_USE) 1; 
# store Environment settings:
set rat(WIDGETCLASS,rtl_combobox) frame;
# ---------------------------------------------------------------------------

# --------------------------------------------------------------------------- 
# class (namespace): rtl_combobox 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_combobox {
 
    variable var; 
    variable w; 
    namespace eval private {};

    array set w [list \
	    0 {} \
	    1 .frame\
	    2 .frame.entry \
	    3 .frame.button \
	    4 .top \
	    5 .top.gridwin \
	    6 .top.list \
	    7 .top.gridwin.hori \
	    8 .top.gridwin.vert \
	    ]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list \
	    original frame \
	    \
	    attributes [list \
	    command clientbackground selectcommand\
	    histposition histheight histlength histwidth\
	    background foreground width selectbackground listtype\
	    selectforeground borderwidth font relief state buttonstate\
	    highlightbackground highlightthickness highlightcolor\
	    ]\
	    background [list {0 background} \
	    {3 background}]\
	    clientbackground [list {2 background} background]\
	    borderwidth [list {1 borderwidth} {3 borderwidth}]\
	    command command\
	    font [list {2 font} font]\
	    foreground [list {2 foreground}\
	    {3 foreground} foreground]\
	    histposition histposition \
	    listtype listtype\
	    histheight histheight \
	    histlength histlength \
	    histwidth histwidth \
	    highlightbackground [list {0 highlightbackground} highlightbackground]\
	    highlightcolor [list {0 highlightcolor} highlightcolor]\
	    highlightthickness {0 highlightthickness} \
	    relief [list {1 relief}]\
	    state [list {2 state} editable] buttonstate [list {2 state}]\
	    selectbackground [list {2 selectbackground} selectbackground]\
	    selectcommand selectcommand \
	    selectforeground [list {2 selectforeground} selectforeground]\
	    width {2 width} \
	    ]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_combobox - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -command
#             : -selectcommand
#             : -histposition
#             : -histheight
#             : -histlength
#             : -histwidth
#             : -background
#             : -clientbackground
#             : -foreground
#             : -width
#             : -selectBackground
#             : -selectForeground
#             : -borderWidth
#             : -font 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_combobox 
# --------------------------------------------------------------------------- 
proc rtl_combobox {{base .rtl_combobox} args} {
 
    variable rtl_combobox::w; 
    if {![winfo exists $base]} {
 
        #create base container 
        [getCreator rtl_combobox $base$w(0) $args] $base \
		-highlightthickness 1 -class Rtl_combobox;

	frame $base$w(1) -bd 1 -relief sunken;
        # Childs to $base$w(1), frame of class: 	Frame
        entry $base$w(2) -highlightthickness 0 -takefocus 1 -borderwidth 0\
		-textvariable rtl_combobox::${base}(entry);


	# Activate Cursor Keys, independent from editable ...
	foreach k [list Up Down] {
	    bind $base$w(2) <${k}> "$base findInCb %K;break";
	}

	bind $base$w(2) <Key-Return>\
		"rtl_combobox::return-pressed $base$w(0) ";

        # Childs to $base$w(2), entry of class: 	Entry

        button $base$w(3) -command "rtl_combobox::button-cmd $base$w(0)"\
		-width 15 -bd 1 -image rtl_down -takefocus "0";

        # Childs to $base$w(3), button of class: 	Button

        pack $base$w(2) -side left -fill both -expand 1;
        pack $base$w(3) -side right -fill y;
	pack $base$w(1) -fill both -expand 1;

        set base [mkProc $base rtl_combobox $args];  
    }

    after idle focus -force $base$w(2);
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------
####VG-STOP####

option add *Rtl_combobox.frame.entry.borderWidth 0;
option add *Rtl_combobox.top.list.borderWidth 0;
option add *Rtl_combobox.borderwidth 1;
option add *Rtl_combobox.frame.button.highlightThickness 0;

# ===========================================================================
# Private methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_combobox::destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Destructor
# ---------------------------------------------------------------------------
proc rtl_combobox::destroy {path} {
    if {[get-image $path]} {
	image delete up($path)_xbm down($path)_xbm;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::private::existsList - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : private methode, check wether listbox
#               exists or not.
# ---------------------------------------------------------------------------
proc rtl_combobox::private::existsList {path} {
    set nspace [namespace parent];

    variable ::${nspace}::w;
    return [winfo exists $path$w(4)];
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::private::demandList - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : private methode, create the listbox
#               and fill em with the history list.
# ---------------------------------------------------------------------------
proc rtl_combobox::private::demandList {path} {
    
    if {![existsList $path]} {
	set nspace [namespace parent];
	variable ::${nspace}::w;
	
	set wd      [$path get-histwidth];
	set ht      [$path get-histheight];
	set history [$path get-history];
	set bg      [$path get-background];
	set fg      [$path get-foreground];
	set hlb     [$path get-highlightbackground];
	set hlc     [$path get-highlightcolor];
	set slf     [$path get-selectforeground];
	set slb     [$path get-selectbackground];
	set fn      [$path get-font];

	# Create the toplevel widget and its children:
	toplevel $path$w(4) -cursor "arrow"\
		-highlightcolor $hlc\
		-highlightbackground $hlb\
		-borderwidth 1 -relief solid;

	wm withdraw $path$w(4);

	bind $path$w(4) <ButtonRelease>\
		"rtl_combobox::button-cmd-2 $path$w(0) %W ";

        rtl_gridwin $path$w(5) -auto "1"\
		-widget "$path$w(6)"\
		-sides "bottom right" -fill "both"\
		-creator "frame";

	set lb $path$w(6);


	# Uset the option database to specifiy given
	# Attributes.

	set rlb *[string trim $lb .];
	foreach {attr valu} [list\
		exportselection     0\
		height              $ht\
		width               $wd\
		background          $bg\
		foreground          $fg\
		highlightbackground $hlb\
		highlightcolor      $hlc\
		font                $fn\
		selectforeground    $slf\
		selectbackground    $slb\
		] {
	    option add ${rlb}.$attr $valu;
	}
	
	# Construct the listbox acoording the given
	# listtype (default is listbox).

        [$path get-listtype] $lb;

        foreach pair [list  [list <ButtonRelease-1>\
		"rtl_combobox::select-item %W $path$w(0)"]\
		[list <Key-Return>\
		"rtl_combobox::select-item %W $path$w(0)"]] {
	    bind $path$w(6) [lindex $pair 0] [lindex $pair 1]; 
        }

	# Allow Keyboard navigation in this listbox widget ...
	foreach key [list a b c d e f g h i j k l m n o p q r s t u v w x y z] {
		bind $lb <Any-${key}> "$path findInLb $lb %K;break"
		set ukey [string toupper $key]
		bind $lb <Any-${ukey}> "$path findInLb $lb %K;break"		
	}

        pack $path$w(5) -expand 1 -fill both;
        grid $path$w(6) -in $path$w(5) -column 1 -row 1 -sticky nesw;

        pack $path$w(1) -expand 1 -fill both;
        wm title $path$w(4) "Combobox";
        wm transient $path$w(4) [winfo toplevel $path];
	wm group $path$w(4) [winfo toplevel $path];
	wm overrideredirect $path$w(4) 1;

	# Fill the listbox
	set lbox [find-widget list $path];
	::eval [linsert $history 0 $lbox insert end];

	$path$w(5) update;

    }

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::button-cmd - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Popup a listbox, if the button is pressed
# ---------------------------------------------------------------------------
proc rtl_combobox::button-cmd {path} {
    private::demandList $path;
    update;


    set cont [find-widget top   $path];
    set lbox [find-widget list  $cont];
    set entr [find-widget entry $path];
    set hpos [get-histposition  $path];
    set hwid [get-histwidth     $path];

    # Check if the window is already displayed ...
    if {[wm state $cont]=="withdrawn"} {

	# Get dimension and position of the history-window 
	set h [winfo height $path];

	if {$hpos == "below"} {

	    # Histwin below
	    set y [expr [winfo rooty $path] + $h];
	    if {[expr $y+[winfo reqheight $lbox]] > [winfo\
		    screenheight $path]} {

		# Force window to apear above
		set y [expr [winfo rooty $path] - [winfo reqheight $lbox]];
	    }
	} else {

	    # Histwin above
	    set y [expr [winfo rooty $path] - [winfo reqheight $lbox]];
	    if {$y < 0} {

		# Force window to apear below
		set y [expr [winfo rooty $path] + $h];
	    }
	}

	if {$hwid == "0"} {
	    set w [expr {[winfo width $entr] + 4}];
	} else {
	    set w [winfo reqwidth $lbox];
	}

	set h [winfo reqheight $lbox];
	set x [winfo rootx $path];

	# Be sure to place the histwin inside the display
	if {[expr $x+$w]>[winfo screenwidth $path]} {
	    if {0 < [expr $x + [winfo width $path] - $w]} {
		set x [expr $x + [winfo width $path] - $w];
	    } else {
		set x [expr [winfo screenwidth $path] -$w];
	    }
	}


	# Set the histwin's position
	wm geometry $cont ${w}x${h}+${x}+${y};
	update;

	# Show the history-window
	wm deiconify $cont;
	raise $cont;

	set grab [grab current $cont];

	# Save current grab
	::set-action rtl_combobox $path grabsave $grab;
	if {"" != $grab} {
	    ::set-action rtl_combobox $path grabsavestate [grab status $grab];
	}

	# Set a global grab to the window (look & feel of menus)
	grab set -global $cont;

	# Configure the history-list
	$lbox selection clear 0 end;

	# Select the entries value in the history, if availiable
	set index [lsearch [$lbox get 0 end] [$entr get]];
	catch {
	    $lbox selection set $index;
	    $lbox activate $index;
	}
	focus $lbox;
    } else {

	# This code should not be reached...
	# ...used to get rid of the window when an error occured
	button-cmd-2 $path;
    }
    # execute the widgets command
    ::eval [get-command $path];
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::button-cmd-2 - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Pop down the history window
# ---------------------------------------------------------------------------
proc rtl_combobox::button-cmd-2 {path {w .}} {

    # Do not close the history-window while using the scrollbars
    if {[winfo class $w] != "Scrollbar"} {

	set cont [find-widget top   $path];
	set entr [find-widget entry $path];
	set grab [::get-action rtl_combobox $path grabsave];

	# Hide the history-window
	grab release $cont;
	if {"" != $grab} {
	    set state [::get-action rtl_combobox $path grabsavestate];
	    if {"global" == $state} {
		grab -global $grab;
	    } else {
		grab $grab;
	    }
	}

	wm withdraw $cont;

	# Set the focus to the entry. We need -force option for NT platforms
	focus -force $entr;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::return-pressed - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Invoked when return is pressed in the entryfield
# ---------------------------------------------------------------------------
proc rtl_combobox::return-pressed {path} {
    button-cmd $path;
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::select-item - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Invoked when an item is selected in the history-window
# ---------------------------------------------------------------------------
proc rtl_combobox::select-item {widget path} {

    # Insert a selected listitem into the entry
    set idx [$widget curselection];
    if {$idx!=""} {
	delete $path 0;
	insert $path 0 [$widget get $idx];
    }
    $widget selection clear 0 end;
    button-cmd-2 $path;

    set selcmd [get-selectcommand $path];
    if {"" != $selcmd} {
	::eval $selcmd;
    }

    return;
}

# ===========================================================================
# Public methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_combobox::entryget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : get value from entryfield
# ---------------------------------------------------------------------------
proc rtl_combobox::entryget {path} {
    return [::get-action rtl_combobox $path entry];
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::entryset - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set the entryfield completely.
# ---------------------------------------------------------------------------
proc rtl_combobox::entryset {path string} {
    return [::set-action rtl_combobox $path entry $string];
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::get - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get the entryfield's content.
# ---------------------------------------------------------------------------
proc rtl_combobox::get {path} {
    return [$path entryget];
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::insert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Insert an string at the given position int the entryfield
# ---------------------------------------------------------------------------
proc rtl_combobox::insert {path index string} {
    set entr [find-widget entry $path];

    if {[private::existsList $path]} {
	button-cmd-2 $path;
    }

    set state [$entr cget -state];

    $entr configure -state normal;
    $entr insert $index $string;
    $entr configure -state $state;

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::delete - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Delete string range from the entryfield
# ---------------------------------------------------------------------------
proc rtl_combobox::delete {path first {last end}} {
    set entr  [find-widget entry $path];
    set state [$entr cget -state];

    $entr configure -state normal;
    $entr delete $first $last;
    $entr configure -state $state;

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::histget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get history item at given position
# ---------------------------------------------------------------------------
proc rtl_combobox::histget {path {index 0}} {
    return [lindex [get-history $path] $index];
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::histinsert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Insert an item to the history list
# ---------------------------------------------------------------------------
proc rtl_combobox::histinsert {path index args} {
    set hlen [get-histlength $path];

    set history [get-history $path];
    set history [::eval [linsert $args 0 linsert $history $index]];

    # What do you mean with NULL, an entire
    # empty history list is useless!

    if {$hlen >= 0} {
	if {[llength $history] > $hlen} {
	    set history [lreplace $history $hlen end];
	}
    }

    set-history $path $history;

    if {[private::existsList $path]} {

	# update the existing listbox
	set lbox [find-widget list $path];
	$lbox delete 0 end;
	::eval [linsert $history 0 $lbox insert end];
    }

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::histclear - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Clear the history list
# ---------------------------------------------------------------------------
proc rtl_combobox::histclear {path} {
    set-history $path {};

    if {[private::existsList $path]} {

	# update the existing listbox
	set lbox [find-widget list $path];
	$lbox delete 0 end;
    }
    return;
}

# ===========================================================================
# Define custom attribute:
# ===========================================================================
# Define generic attribute:
# ---------------------------------------------------------------------------
create-action rtl_combobox command {histlength -1}\
	{histwidth 0} {histheight 5} {image 0} {histposition below}\
	background selectcommand highlightcolor highlightbackground\
	selectbackground selectforeground foreground history font\
	{editable 1} {listtype listbox};

# ===========================================================================
# Redefined generic set-attributes:
# ---------------------------------------------------------------------------
# Function    : rtl_combobox::set-foreground - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Reconfigure image forground color
# ---------------------------------------------------------------------------
proc rtl_combobox::set-foreground {path value} {
    ::set-action rtl_combobox $path foreground $value;

    if {[string compare $::tcl_platform(platform) "macintosh"] == 0} {
	# Don't do anything on MacOS because Tk 8.0 ...
	# does not support bitmaps on buttons on that platform.
	return;
    }

    if {![regexp {^(black|Black|#[0]*)$} $value match qq]} {
	if {![get-image $path]} {
	    # There is a new image to be created for that instance
	    # use the given coler to reconfigure it.
	       # Create images for the widget

	    image create bitmap up($path)_xbm -data { \
		    #define up(path)_width 9 \
		    #define up(path)_height 9 \
		    static char up(path)_bits[] = { \
		    0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x38, 0x00, 0x7c, \
		    0x00, 0xfe, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00}
	     }  -maskdata { \
		#define up(path)_width 9 \
		#define up(path)_height 9 \
		static char up(path)_bits[] = { \
		0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x38, 0x00, 0x7c, \
		0x00, 0xfe, 0x00, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00}
             } -foreground $value;
    
	     image create bitmap down($path)_xbm -data { \
		     #define down(path)_width 9 \
		     #define down(path)_height 9 \
		     static char down(path)_bits[] = { \
		     0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe, 0x00, 0x7c, \
		     0x00, 0x38, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}
	     } -maskdata { \
		     #define down(path)_width 9 \
		     #define down(path)_height 9 \
		     static char down(path)_bits[] = { \
		     0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe, 0x00, 0x7c, \
		     0x00, 0x38, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}
	     } -foreground $value;

	    switch {[get-histposition $path]} {
		"above" {
		    set image down($path)_xbm;
		}
		default {
		    set image up($path)_xbm;
		}
	    }

	    set btn [find-widget button $path]
	    $btn configure -image $image;


             set-image $path 1;
	} else {
	    down($path)_xbm configure -foreground $value;
	    up($path)_xbm configure -foreground $value;
	}
    } elseif {[get-image $path]} {
	down($path)_xbm configure -foreground $value;
	up($path)_xbm configure -foreground $value;
    }
}

foreach attr [list font background selectforeground selectbackground\
	highlightcolor highlightbackground] {
    ::eval [format {proc rtl_combobox::set-%1$s {path value} {
	::set-action rtl_combobox $path %1$s $value;
	if {[private::existsList $path]} {
	    [find-widget list $path] configure -%1$s $value;
	}
	return $value;
    }
    } $attr];
}


# ---------------------------------------------------------------------------
# Function    : rtl_combobox::set-histwidth - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Overwrite predefined configure option for histwidth
#               this method will reconfigure the list widget.
# ---------------------------------------------------------------------------
proc rtl_combobox::set-histwidth {path value} {
    ::set-action rtl_combobox $path histwidth $value;
    if {[private::existsList $path]} {
	[find-widget list $path] configure -width $value;
    }
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::set-histheight - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : height of the history window
# ---------------------------------------------------------------------------
proc rtl_combobox::set-histheight {path value} {
    ::set-action rtl_combobox $path histheight $value;
    if {[private::existsList $path]} {
	[find-widget list $path] configure -height $value;
    }
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::set-histposition - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : histposition where to pop up the history window
#              (valid values are above and below)
# ---------------------------------------------------------------------------
proc rtl_combobox::set-histposition {path value} {
    set image rtl_down;
    switch $value {
	above {
	    if {[get-image $path]} {
		set image up($path)_xbm;
	    } else {
		set image rtl_up;
	    }
	}
	below {
	    if {[get-image $path]} {
		set image down($path)_xbm;
	    }
	}
	default {
	    # Raise an exception!
	    error [format\
		    {Bad histposition "%s". Should be "above" or "below".}\
		    $value];
	    
	}
    }

    [find-widget button $path] configure -image $image;
    ::set-action rtl_combobox $path histposition $value;
    return $value;
}

# ===========================================================================
# New Code 1.2 : sophisticated search in entry and listbox.
# ---------------------------------------------------------------------------
# Function    : rtl_combobox::set-editable - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Change the binding on the combobox's entry widget
#               regarding to the given value.
# ---------------------------------------------------------------------------
proc rtl_combobox::set-editable {path value} {

    set entry  [find-widget entry $path];
    set button [find-widget button $path];

    switch $value {
	"disabled" {
	    bind $entry <ButtonRelease-1> "focus $entry;after idle $button invoke"
	    foreach type [list Tab Shift-Tab] {
		bind $entry <${type}> [bind Entry <${type}>]
	    }
	
	    bind $entry <Any-Key> "$path findInCb %K;break";
	}
	"normal" {
	    bind $entry <ButtonRelease-1> "";
	    bind $entry <Any-Key> "";
	}
	default {
	    # Ignore that, is handled by the
	    # state attribute of the entry widget.
	}
    }

    ::set-action rtl_combobox $path editable $value;
}

proc rtl_combobox::set-listtype {path value} {
    ::set-action rtl_combobox $path listtype $value;
    if {[private::existsList $path]} {
	
	# Make sure to destroy the given listbox, because
	# there might be a different one to be created.
	::destroy [find-widget top $path];
    }
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_combobox::findInCb - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Search for the given token inside the history list
#               and set the associated entry to that item.
# ---------------------------------------------------------------------------
proc rtl_combobox::findInCb {path token} {
    set data [$path get-history];
    set id   [lsearch -regexp $data ^${token}];

    if {$id > -1} {
	set item     [lindex $data $id];
	$cb entryset [lindex $data $id];
    } else {
	set item [$path entryget];
	set lg   [expr {[llength $data] - 1}];
	set id   [lsearch -exact $data $item];
	if {$id >= 0} {

	    switch $token {
		"Down" {
		    if {$id < $lg} {
			incr id;
		    }
		}
		"Up" {
		    if {$id > 0} {
			incr id -1;
		    }
		}
		default {
		    # Ignore any other token a-z  to 0-9 are handled above
		    return;
		}
	    }
	} else {
	    set id 0;
	}

	entryset $path [lindex $data $id];
    }
}


# ---------------------------------------------------------------------------
# Function    : rtl_combobox::findInLb - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Search for token in a open listbox
# ---------------------------------------------------------------------------
proc rtl_combobox::findInLb {cb lb token} {
	set data    [get-history $cb];
	set cid     [$lb curselection];
	set id      [lsearch -regexp $data ^${token}];
	if {$id > -1} {
		
		# Use the Built-in Listbox commands from tk to
		# properly navigate in the listbox ...
		
		#set item [lindex $data $id]
		#$lb activate $id
		#$lb yview $id
		
		set steps    [expr {$id - $cid}];
	    catch {
		tkListboxUpDown $lb $steps;
	    }
	}
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 14.06.1999, Roger
# Port RTL 1.2      : 04.03.2000, Roger
# Test RTL 1.2      : 04.03.2000
# Integration VG 2.5: 04.03.2000, Roger
# Integration G-Draw:
# System            :
# ---------------------------------------------------------------------------
# TODO              :
#                     [1] Modify set-foreground to allow a more
#                         sophisticated behavior.(done)
#                     [2] New Feature -editable(not needed because
#                         -state ...).(done)
#                     [3] New Feature smooth navigation
#                         through history list either from entry
#                         or the listbox. (LHT - code).(done) <test>
#                     [4] Fixme : don't reopen the listbox
#                         multiple times.
#                     [5] Review rtl.tcl mkDefault ...(seems not to
#                         work).(done)
#                     [6] Change borderwidth,relief to client...
#                     [7] New Feature allow to specify the class
#                         for the listbox widget.
#
# ---------------------------------------------------------------------------


