# -----------------------------------------------------------------------
#   $Source: /projects/cheshire/CVSREPOS/cheshire/config-gui/rtl/rtl_mlistbox.tcl,v $
# $Revision: 1.1 $
#     $Date: 2000/10/31 21:46:00 $
#   $Author: yzhang $
#    $State: Exp $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: 
#  Multi column listbox (mega)widget.
# 
# $Log: rtl_mlistbox.tcl,v $
# Revision 1.1  2000/10/31 21:46:00  yzhang
# *** empty log message ***
#
# Revision 1.3  1999/09/03 09:27:01  roger
# BETA II Release
#
# Revision 1.2  1999/07/14 15:15:54  roger
# Redesign header color is taken from background and not longer from dummy!
# Showcol asks if the widget does exist before using grid.
#
# Revision 1.1.1.1  1999/07/13 15:13:13  roger
# VisualGIPSY 2.1
#
# Revision 1.18  1999/01/19 11:04:59  heiko
# Commented resource database options for mlistbox.
#
# Revision 1.17  1999/01/11 11:55:03  thomas
# Added some option to make it work with vg 2.0.
#
# Revision 1.16  1999/01/04 09:38:11  thomas
# Added shutdown procedure for widget instance.
#
# Revision 1.15  1998/12/22 18:01:04  thomas
# Removed some bugs before distribution.
#
# Revision 1.14  1998/10/30 14:23:59  heiko
# Variaous changes for new vg2.0 component concept.
#
# Revision 1.13  1998/10/12 16:21:20  heiko
# Initial setting of vertical slider now works under Windows.
#
# Revision 1.12  1998/09/08 12:42:21  thomas
# Added option -executecommand.
#
# Revision 1.11  1998/08/20 10:28:06  heiko
# GUI modifications to get Windows look and feel.
#
# Revision 1.10  1998/08/20 08:18:24  heiko
# Fixed wrong resize handle position if widget is scrolled.
#
# Revision 1.9  1998/07/23 16:08:25  heiko
# Fixed bug in set-resizable.
#
# Revision 1.8  1998/07/22 15:15:53  heiko
# Fixed bug in widgets yview command.
#
# Revision 1.7  1998/07/21 16:24:19  heiko
# Fixed borderwidth and selectcommand to work with VG.
#
# Revision 1.6  1998/07/21 15:53:35  heiko
# Fixed tags and columnwidths options.
#
# Revision 1.5  1998/07/21 14:48:27  heiko
# Fixed widgets to work with VG.
#
# Revision 1.4  1998/07/20 16:14:16  thomas
# restored repository
#
# Revision 1.2  1998/07/07 10:21:37  thomas
# Modified select method to provide deselection.
#
# Revision 1.1  1998/07/06 14:09:51  thomas
# renamed
#
# Revision 1.10  1998/07/06 14:08:21  thomas
# New procs: showcol hidecol. Configuration on -columnwidths nor works withs sizes in units of pixels (instead of letters).
#
# Revision 1.9  1998/07/03 15:02:00  thomas
# Inserted some comments, resorted procs in source. Slightly changed selection mechanism of rows in listboxes.
#
# Revision 1.8  1998/07/03 10:54:40  thomas
# Added option -borderwith to rtl_mlistbox.
#
# Revision 1.7  1998/07/03 10:26:46  thomas
# New methods 'getcol' and 'setcol'. Added some comments.
#
# Revision 1.6  1998/07/01 15:53:51  thomas
# Added proc curselection.
#
# Revision 1.5  1998/07/01 08:54:25  thomas
# Added proc makelist.
#
# Revision 1.4  1998/07/01 08:28:21  thomas
# Added proc 'search'.
#
# Revision 1.3  1998/06/25 16:54:52  thomas
# Added new procedure makeassoc. Corrected procedure get to return result like a
# simple listbox does.
#
# -----------------------------------------------------------------------
# ---------------------------------------------------------------------------
#
# Information about rtl_mlistbox widget
# --------------------------------
#
# Name
#  rtl_mlistbox -Multicolumn listbox. Each for each column there is  an heading at
#  the top of the listbox.
#
# Synopsis
#  rtl_mlistbox pathName ?options?
#
# Options
#  -columns cols
#     Number of columns. If cols is a positive integer, the rtl_mlistbox will have
#     cols columns. If cols is a Tcl-List, for each item there will be an column 
#  -yscrollcommand
#     Command for vertical scrolling of rtl_mlistbox. See yscrollcommand of tk listbox.
#  -xscrollcommand
#     Command horizontal scrolling of rtl_mlistbox. See xscrollcommand of tk listbox.  
#  -borderwidth
#     borderwidths of listboxes
#  -columnwidths
#     Widths of columns. Tcl-List of column width is expected. Each Item is
#     expected to have the format accepted as width by a tk listbox.
#  -headings
#     Headings for columns. Tcl-List of column headings. Each heading is a string.
#  -headingsanchor
#     Justification of the heading strings. Value is set for each heading.
#  -selectcommand
#     Procedure called each time a selection in the rtl_mlistbox is done. Calling
#     rtl_mlistbox widget and selected line index ar appended to call so your
#     procedure should look like
# 	proc handleselection {widget index} { # body }
#  -command
#     Procedure called each time a doubleclick in the rtl_mlistbox is performed.
#  -tags
#     Each column has a tag serving as handle for configuration. The tags of all
#     columns may be set via this option. Default tags ar the column numbers.
#     NOTE: duplicate tags are allowed but only first colunm with a duplicate
#       tag is reachable via columnconfigure.
#
# Widget commands
#  Most commands have same syntax as tk listbox except listbox entries are lists of
#  strings instead of strings. Each list referes to a line in rtl_mlistbox.
#
#   activate
#     See listbox.
#   columnconfigure tag ?opton value? ?opton value? ...
#     Configure column specifyed by tag. Allowed options are 
# 	heading   - text of column heading 
#       resizable - if 1 column is resizable via mouse (drag&drop)
# 	tag	  - substitute tag for column
#       width	  - column width in pixels
#   delete pos data1 data2 data3 ...
#     See listbox.
#   get first last
#     See listbox.  
#   insert pos data1 data2 data3 ...
#     See listbox.  
#   select index
#    Select single line in rtl_mlistbox. Line number is index. selcetcommand is
#     executed. 
#   xscrollcommand  
#   xview
#     xview of canvas is executed.
#   yscrollcommand
#   yview
#     See listbox. Return value ist yview of last column.
#   curselection
#     Get index of current selection (See listbox).
#   size
#     Get number of lines in listbox (See listbox).
#   makeassoc data
#     Build associative list using column tags.
#   makelist data
#     Simplyfy associative list to sorted list according to tags.
#   setcol tag
#     Set values of specific column.
#   getcol tag
#     Get values in column as sorted list.
#   showcol tag
#     Make specified column visible on screen.
#   hidecolumn tag
#     Make specified column invisible.
#     
#
# RTL_MLISTBOX NEEDS LIBRARIES vgnetxt.tcl AND vgclass.tcl PROVIDED BY ROGER.
# PATH IS:  /timoda/roger/vgxtnd/
#
# ToDo:
# =====
#  * invariance of option order!
#  * drag & drop inside list
#
#  * implement some other options provided by tk listbox 
#   (colors, borderwidths, reliefs ...)
# 
#  * implement some other commands provided by tk listbox 
#
# ---------------------------------------------------------------------------
package provide rtl_mlistbox 1.2;
package require rtl;


####VG-START####
set rtl_mlistbox(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_mlistbox; 
	set src [file join $rtl_mlistbox(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# class (namespace): rtl_mlistbox 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_mlistbox {
 
    variable var; 
    variable w; 
    array set w [list 4 .slider 0 {} 1 .entry 2 .canvas 3 .canvas.inner]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  list .listbox original frame xscrollcommand \
	    xscrollcommand selectbackground [list {1 selectbackground}\
	    selectbackground] columnwidths \
        columnwidths headings headings foreground [list {1 foreground}\
	foreground] container \
        .canvas.inner canvas .canvas highlightthickness [list \
        [list 0 highlightthickness]] attributes [list background borderwidth\
        columns columnwidths command dragstartcommand dragmotioncommand\
        dragendcommand font foreground headings headingsanchor resizable\
        selectcommand selectbackground tags xscrollcommand\
        yscrollcommand highlightbackground highlightcolor\
        highlightthickness relief height width] tags tags dummy .entry \
        command command height {0 height} columns columns \
        dragstartcommand dragstartcommand dragendcommand dragendcommand \
        highlightcolor {0 highlightcolor} font [list {1 font} font] \
        background [list [list 3 background]\
        [list 0 background] background] resizable resizable head .head \
        dragmotioncommand dragmotioncommand borderwidth [list\
        [list 0 borderwidth] borderwidth] yscrollcommand yscrollcommand \
        highlightbackground {0 highlightbackground} widthhandle \
        .widthhandle width {0 width} slider .slider headingsanchor \
        headingsanchor selectcommand selectcommand relief {0 relief}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_mlistbox - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -background
#             : -borderwidth
#             : -columns
#             : -columnwidths
#             : -command
#             : -dragstartcommand
#             : -dragmotioncommand
#             : -dragendcommand
#             : -font
#             : -foreground
#             : -headings
#             : -headingsanchor
#             : -resizable
#             : -selectcommand
#             : -selectbackground
#             : -tags
#             : -xscrollcommand
#             : -yscrollcommand
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -relief
#             : -height
#             : -width 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_mlistbox 
# --------------------------------------------------------------------------- 
proc rtl_mlistbox {{base .rtl_mlistbox} args} {
 
    variable rtl_mlistbox::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_mlistbox $base$w(0) $args] $base\
		-class Rtl_mlistbox;

        # Childs to $base$w(0), rtl_mlistbox of class: 	Rtl_Mlistbox
        entry $base$w(1) -borderwidth "0";
        canvas $base$w(2) -height "150" -highlightthickness "0" -width "1";
        bind $base$w(2) <Configure> \
             "$base$w(0) adapt";

        # Childs to $base$w(2), canvas of class: 	Canvas
        frame $base$w(3);
        bind $base$w(3) <Configure> \
             "$base$w(0) adapt";

	$base$w(2) create window 0.0 0.0\
		-anchor "nw" -tags "$base$w(3)"\
		-window "$base$w(3)";

        frame $base$w(4) -borderwidth "2" -relief "sunken" -width "2";

        pack $base$w(2) -expand 1 -fill both;
        place $base$w(4) -x -2 -y 0 -relheight 1; 
        catch {rtl_mlistbox::init $base;} 
        set base [mkProc $base rtl_mlistbox $args];  
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------

#option add *Rtl_Mlistbox.background {pink};
##option add *Rtl_Mlistbox.entry.background {#D300D300D300};
#option add *Rtl_Mlistbox.entry.font {-dt-interface user-medium-r-normal-m*-*-*-*-*-*-*-*-*};
#option add *Rtl_Mlistbox.entry.foreground {#000000000000};
#option add *Rtl_Mlistbox.canvas.background {#D300D300D300};


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,rtl_mlistbox) rtl_mlistbox;
# ---------------------------------------------------------------------------

####VG-STOP####

# ===========================================================================
# Rtl_mlistbox Class bindings:
# ---------------------------------------------------------------------------
# Create bind group for wiget. Note: Ancestor's widget methods are used.
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::ancestor - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Find the great grandfather widget of 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::ancestor {w} {
    return [winfo parent [winfo parent [winfo parent $w]]];
}

bind Rtl_mlistbox_listbox <1> \
	    "\[rtl_mlistbox::ancestor %W\] select \[%W index @%x,%y\];";
# enable/disable drag & drop here !
bind Rtl_mlistbox_listbox <1> \
	"\[rtl_mlistbox::ancestor %W\] select \[%W index @%x,%y\]; \
	rtl_mlistbox::button-press-cb %W %x %y %t";
bind Rtl_mlistbox_listbox <Key-Select> \
	"\[rtl_mlistbox::ancestor %W\] select \[%W index @%x,%y\];";
bind Rtl_mlistbox_listbox <Double-1> \
	"\[rtl_mlistbox::ancestor %W\] execute;";
bind Rtl_mlistbox_listbox <Key-space> \
	"\[rtl_mlistbox::ancestor %W\] select \[%W index active];";
bind Rtl_mlistbox_listbox <Key-Down> \
	"\[rtl_mlistbox::ancestor %W\] tkCommand list tkListboxUpDown 1;";
bind Rtl_mlistbox_listbox <Key-Up> \
	"\[rtl_mlistbox::ancestor %W\] tkCommand list tkListboxUpDown -1;";
bind Rtl_mlistbox_handle <Button-1> \
	"\[rtl_mlistbox::ancestor %W\] resize-start %W %x %y";
bind Rtl_mlistbox_handle <ButtonRelease> \
	"\[rtl_mlistbox::ancestor %W\] resize-end %W %x %y";
bind Rtl_mlistbox_handle <B1-Motion> \
	"\[rtl_mlistbox::ancestor %W\] resize-drag %W %x %y";


# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_mlistbox\
	position background foreground selectbackground\
	dragendcommand dragstartcommand dragmotioncommand dragstart\
	xscrollcommand yscrollcommand borderwidth command\
	selectcommand tags resizable columns font;
	
# ===========================================================================
# Overwritten Generic Attributes (set methods):
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-background - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set background of child widgets (in respect to platform)
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-background {path value} {
    ::set-action rtl_mlistbox $path background $value;
    set contents [list head widthhandle];

    if {"windows" != $::tcl_platform(platform)} {
	lappend contents "list";
    }

    foreach type $contents {
	foreachcol $path $type w i {
	    $w configure -background $value;
	}
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-foreground - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set foreground attribute for all child widgets
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-foreground {path value} {
    ::set-action rtl_mlistbox $path foreground $value;
    foreach type [list head list] {
	foreachcol $path $type w i {
	    $w configure -foreground $value
	}
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-font - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Apply font to all child widgets
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-font {path value} {
    ::set-action rtl_mlistbox $path font $value;
    foreach type {head list} {
	foreachcol $path $type w i {
	    $w configure -font $value
	}
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-selectbackground - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set selectbackground for all child widgets
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-selectbackground {path value} {
    ::set-action rtl_mlistbox $path selectbackground $value;
    foreach type {list} {
	foreachcol $path $type w i {
	    $w configure -selectbackground $value
	}
    }
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-xscrollcommand - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Apply xscrollcommand to the
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-xscrollcommand {path value} {
    variable var;

    ::set-action rtl_mlistbox $path xscrollcommand $value;
    $path$var(canvas) configure -xscrollcommand $value;
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-yscrollcommand - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Appy yscrollcommand to the child widgets
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-yscrollcommand {path value} {
    variable var;

    ::set-action rtl_mlistbox  $path yscrollcommand $value;

    foreachcol $path list widget i {
	$widget configure -yscrollcommand\
		"rtl_mlistbox::do-yscroll $path $widget";
    }

    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-borderwidth - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Apply borderwidth to all children
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-borderwidth {path value} {
    variable var;

    ::set-action rtl_mlistbox $path borderwidth $value;

    # Set borderwidth for all listboxes
    foreach item {head list} {
	command $path $item configure -borderwidth $value;
    }
    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-tags - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set column tags
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-tags {path taglist} {
    variable var;
    variable $path;

    checklength $path $taglist "configure -tags";

    # Delet old tags 
    foreach tag [get-tags $path] {
	catch {unset ${path}(colhandle,$tag)};
    }

    set i 0;
    foreach tag $taglist {
	set ${path}(colhandle,$i) $tag;
	set ${path}(colnumber,$tag) $i;
	incr i;
    }

    ::set-action rtl_mlistbox $path tags $taglist;
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-resizable - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set if columns are resizable or ot
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-resizable {path on} {

    # Check if parameter is of type boolean
    if ![expr ![catch {if $on {}}]] {
	uplevel 2 [list error "Bad boolean value \"$on\""];
    }

    if {[get-resizable $path] != $on} {

	# Estimate new level of handles
	if $on {
	    set level raise;
	} else {
	    set level lower;
	}

	# Set level of handles
	foreachcol $path widthhandle widget i {
	    $level $widget;
	}

	::set-action rtl_mlistbox $path resizable $on;
    }

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-columns - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Column count as positive integer
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-columns {path value} {
    variable var;
    variable $path;

    set oldCols [get-columns $path];
    set container ${path}$var(container);

    # Install handles
    # Test for positive integer

    # ? Implicit set tags ? -->
    set-tags $path [list];

    if {[expr ![catch {format "%i" $value}]] && ($value>-1)} {
	
	# Default handles: column numbers.
	::set-action rtl_mlistbox $path columns $value;

	for {set i 0} {$i<$value} {incr i} {
	    set ${path}(colhandle,$i) "$i";
	    set ${path}(colnumber,$i) "$i";
	    lappend ${path}(tags) "$i";
	}
    } else {

	::set-action rtl_mlistbox $path columns [llength $value];

	set i 0;
	foreach item $value {
	    set $path(colhandle,$i) "$item";
	    set $path(colnumber,$item) "$i";
	    lappend ${path}(tags) "$item";
	    incr i;
	}
    }

    # Create and manage headings
    foreachcol $path head widget i {
	if {![winfo exists $widget]} {
	    label $widget -bd 1 -relief raised;
	}

	set handle [set ${path}(colhandle,$i)];
	$widget configure \
		-width 1 \
		-text [opget $path _$handle $handle] \
		-background [get-background $path] \
		-foreground [get-foreground $path] \
		-font [get-font $path];
	grid $widget -row 0 -column $i -sticky nswe;
    }

    foreachcol $path widthhandle widget i {
	set borderwidth [opget $path borderwidth 1];
	if {![winfo exists $widget]} {
	    frame $widget\
		    -highlightthickness 0\
		    -cursor [opget $path handlecursor sb_h_double_arrow] \
		    -width [opget $path resizesenswidth 5] \
		    -background [get-background $path];

	    frame $widget.black\
		    -highlightthickness 0\
		    -background black\
		    -width $borderwidth;

	    pack propagate $widget 0;
	    pack $widget.black -side right -fill y;

	    # Attach bindings
	    bindtags $widget [linsert [bindtags $widget] 1 Rtl_mlistbox_handle];
	}
	grid $widget -row 0 -column $i -sticky nse -pady $borderwidth;
    }

    # Get the default background for rtl_mlistboxes to get look and feel of NT
    set bg [$path$var(dummy) cget -bg];

    # Create and manage listboxes
    foreachcol $path list widget i {
	if {[catch {

	if {![winfo exists $widget]} {
	    listbox $widget\
		    -bd 0\
		    -borderwidth [get-borderwidth $path] \
		    -highlightthickness 0\
		    -selectborderwidth 0\
		    -selectmode single \
		    -exportselection no \
		    -foreground [get-foreground $path] \
		    -selectbackground [get-selectbackground $path] \
		    -font [get-font $path] \
		    -background $bg;

	    # Manage listbox
	    grid $widget -row 1 -column $i -sticky nswe;
	    
	    # Attach bindings
	    bindtags $widget [linsert [bindtags $widget] 1 Rtl_mlistbox_listbox];
	}

	$widget configure -width [opget $path width 1];

	# Clear listbox
	$widget delete 0 end;

	# ? Implicit set of columnwidth ? -->
	set colw 10;
	set coll [list];
	for {set j 0} {$j < [get-columns $path]} {incr j} {
	    lappend coll $colw;
	    set ${path}($j,columnwidth) $colw;
	}

	set-columnwidths $path $coll;

	grid columnconfigure $container $i \
		-weight 0 \
		-minsize $colw;
    } msg]} {
	puts stderr "error message caused by creation listboxes\n <$msg>";
    }
    }

    # Destroy suplementary widget sets
    for {} {$i<$oldCols} {incr i} {
	foreach item {head widthhandle list} {
	    set type $var($item);
	    destroy $container${type}_$i;
	}

	unset ${path}(colnumber,[set ${path}(colhandle,$i)]);
	unset ${path}(colhandle,$i);
	grid columnconfigure $container $i -minsize 0;
    }

    grid rowconfigure $container 1 -weight 1;

    # Hide slider
    lower $path$var(slider);
    
    # Activate scrolling update for all listboxes
    set-yscrollcommand $path [get-yscrollcommand $path];

    # Set default size
    set width 50;

    # <-- ? Note implicit set of tags above ? --<
    ## MAke sure that columnconfigure is using good code
    set tags [get-tags $path];
    foreach tag $tags {
	columnconfigure $path $tag -width $width;
    }

    ${path}.canvas configure -height 100 \
	    -width [expr [llength $tags]*$width];

    return $value;
}




# ===========================================================================
# New Generic Attributes:
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-headings - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Column headings as list of headings (strings)
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-headings {path headinglist} {
    checklength $path $headinglist "configure -headings";
    return [setoption $path head text $headinglist];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::get-headings - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get column headings as a list
# ---------------------------------------------------------------------------
proc rtl_mlistbox::get-headings {path} {
    return [getoption $path head text];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-headingsanchor - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get Justify headings
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-headingsanchor {path vallist} {
    checklength $path $vallist "configure -headingsanchor";
    return [setoption $path head anchor $vallist];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::get-headingsanchor - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set Justify headings
# ---------------------------------------------------------------------------
proc rtl_mlistbox::get-headingsanchor {path} {
    return [getoption $path head anchor];
}


# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::set-columnwidths - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set list of column width values
#               column widts as list of widths 
#              (number of chars, positive integers)
# ---------------------------------------------------------------------------
proc rtl_mlistbox::set-columnwidths {path widthlist} {
    variable $path;
    checklength $path $widthlist "configure -columnwidths";
    foreachcol $path head wid i {
	set ww [lindex $widthlist $i];
	columnconfigure $path $i -width $ww;
	set ${path}($i,columnwidth) $ww;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::get-columnwidths - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::get-columnwidths {path} {
    variable var;

    set colu [get-columns $path];
    set cont $path$var(container);
    set result {};

    for {set i 0} {$i < $colu} {incr i 1} {
	lappend result [grid columnconfigure $cont $i -minsize];
    }

    return $result;
}


# ===========================================================================
# Public Methods:
# ---------------------------------------------------------------------------

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::get - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get specified lines from listboxes
# ---------------------------------------------------------------------------
proc rtl_mlistbox::get {path first {last ""}} {
    variable var;

    set columns [get-columns $path];
    if {""==$last} {
	set last $first;
    }

    # Do nothing if no columns exist.
    if {$columns < 1} {
	return [list];
    }

    # Get entries of lists.
    foreachcol $path list widget i {
	set col($i) [$widget get $first $last];
    }
    
    # Group rows
    set rows [llength $col(0)]
    set result [list];
    for {set i 0} {$i<$rows} {incr i} {
	set row [list];
	for {set j 0} {$j<$columns} {incr j} {
	    lappend row [lindex $col($j) $i];
	}   
	lappend result $row;
    }

    # If list contains only on row, don't return it as a list but only return
    # element (= listbox behaviour).
    if {1==[llength $result]} {
	set result [lindex $result 0];
    }

    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::insert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Insert lines to listbox
#
# COMMENT: Collecting the items for the columns instead of inserting them
# row whise may slow down the process because 'eval' is nedded to perform
# the insertion operation. On Windows systems, for which this procedure is
# optimized, this version is faster.
#
# ---------------------------------------------------------------------------
proc rtl_mlistbox::insert {path index args} {

    foreach valuelist $args {
	checklength $path $valuelist insert;

	# Build up insertion list for columns.
	# Collect all column items first, insert columns as whole later.
	set i 0;
	foreach item $valuelist {
	    lappend col($i) $item;
	    incr i;
	}
    }

    if [info exists col] {
	foreachcol $path list widget i {
	    eval $widget insert $index $col($i);
	}
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::delete - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Delete lines from listbox
# ---------------------------------------------------------------------------
proc rtl_mlistbox::delete {path args} {
    return [command $path list delete  $args];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::activate - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Activate line
# ---------------------------------------------------------------------------
proc rtl_mlistbox::activate {path index} {
    return [command $path list activate $index];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::select - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Select line (not existing for single listbox)
# ---------------------------------------------------------------------------
proc rtl_mlistbox::select {path {index {}}} {

    command $path list selection clear 0 end;
    if {""!=$index} {
	command $path list selection set $index;

	# The following command does the same as the preceding two do. 
	#    tkCommand $path list tkListboxBeginSelect $index;
	# I Think we don't need the following command.
	#    command $path list activate $index;
	
	# Note: eval {} take's less time to compute then a string
	# compare if {} == {} ...

	set selc [get-selectcommand $path];
	::eval $selc;
    }

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::execute - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Callback when a doubleclick on a listbox happen
# ---------------------------------------------------------------------------
proc rtl_mlistbox::execute {path} {
    set cmd [get-command $path];
    ::eval $cmd;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::curselection - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get line indices of current selection
# ---------------------------------------------------------------------------
proc rtl_mlistbox::curselection {path} {
    return [command0 $path list curselection]; 
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::dragstart - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See set-dragstart
# ---------------------------------------------------------------------------
proc rtl_mlistbox::dragstart {path} {
    return [get-dragstartcommand $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::dragmotion - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See set-dragmotion
# ---------------------------------------------------------------------------
proc rtl_mlistbox::dragmotion {path} {
    return [get-dragmotioncommand $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::dragend - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See set-dragend
# ---------------------------------------------------------------------------
proc rtl_mlistbox::dragend {path} {
    return [get-dragendcommand $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::yview - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set visible interval in y-direction
# ---------------------------------------------------------------------------
proc rtl_mlistbox::yview {path args} {

    # Update viewport for each listbox
    foreachcol $path list widget i {
	if {[winfo manager $widget]!=""} {
	    eval $widget yview $args;
	}
    }

    # Return scroll value of selected column listbox
    set yv {};
    foreachcol $path list widget i {
	if {[winfo manager $widget]!=""} {
	    set yv [$widget yview];
	    break;
	}
    }
    return $yv;

    # we should use the following command for consistancy with 'size' and
    # 'curselection' instead of the preceding if-statement. Don't have the
    # time to tsts this section so i leave it like it is.
    # return [command0 $path list yview];
}

# set visible interval in x-direction
proc rtl_mlistbox::xview {path args} {
    variable var;
    set canv $path$var(canvas);

    # ? Two scroll commands ? -->
    ::eval $canv xview $args;
    return [$canv xview];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::columnconfigure - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::columnconfigure {path tag args} {
    variable var;
    variable $path;
 
   # Check if specified tag exists
    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }

    set column [set ${path}(colnumber,$tag)];

    # Save conversion of option list to array
    if [catch {array set options $args}] {
	uplevel 1 [list error "Wrong number of arguments"];
    }

    set cont $path$var(container);
    set head $cont$var(head);
    set hand $cont$var(widthhandle);

    foreach opt [array names options] {
	switch -- $opt {
	    -width { 
		# resize specified column
		if {0 < $options($opt)} {
		    if {[catch {
			showcol $path $column;
		    } msg]} {
			puts stderr "showcol is causing this\
				exception!\n $msg";
			
		    }
		    if [catch {colresize $path $column $options($opt)} msg] {
			uplevel 1 [list error $msg];
		    }
		} else {
		    hidecol $path $column;
		}
	    }
	    -heading {
		${head}_$column configure -text $options($opt);
	    }
	    -resizable {

		# Check if parameter is of type boolean
		if ![expr ![catch {if $options($opt) {}}]] {
		    uplevel 1 [list error "Bad boolean value \"$on\""];
		}
		# Estimate new level of handles
		if $options($opt) {
		    set level raise;
		} else {
		    set level lower;
		}

		# Set level of handles
		$level ${hand}_$column;
	    }
	    -tag {
		unset ${path}(colnumber,$tag);
		set ${path}(colnumber,$options($opt)) $column;
		set ${path}(colhandle,$column) $options($opt);
	    }
	    default {
		uplevel 1 [list error "Unknown option: $opt. Use heading,\
			resizable, tag or width"];
	    }
	}
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::search - 
# 
# Returns     : 
#  Index  of first row that matches pattern.
# Parameters  : 
#  path       - path of parent of listboxes
#  tag        - key specifying column
#  pattern    - pattern to glob for
#  
# Description : 
#  Search column specified by $tag for pattern $pattern.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::search {path tag pattern} {
    variable var;
    variable $path;

    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }

    set listbox $path$var(container)$var(list)_[set ${path}(colnumber,$tag)];
    
    set colvals [$listbox get 0 end];
    set row [lsearch -glob [$listbox get 0 end] $pattern];
    return $row;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::getcol - 
# 
# Returns     : 
#  Content of column in format provided by listbox.
# Parameters  : 
#  path       - path of parent of listboxes
#  tag        - key specifying column
# 
# Description : 
#  Get content of column specified by tag.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::getcol {path tag} {
    variable var;
    variable $path;

    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }
    set listbox $path$var(container)$var(list)_[set ${path}(colnumber,$tag)];
    return [$listbox get 0 end];
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::setcol - 
# 
# Returns     : 
#  Result of listbox insert command.
# Parameters  : 
#  path       - path of parent of listboxes
#  tag        - key specifying column
#  data       - list of lines for listbox.
# 
# Description : 
#  Set content of column specified by tag.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::setcol {path tag data} {
    variable var;
    variable $path;

    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }

    # Test for propper list length for rtl_mlistbox
    if {[size $path]!=[llength $data]} {
	uplevel 1 [list error "Invalid length of data list. Must be same as list size."];
    }

    # Get selection to retreive it after setting new values
    set index [curselection $path];

    # Set values
    set listbox $path$var(container)$var(list)_[set ${path}(colnumber,$tag)];
    $listbox delete 0 end;
    set result [::eval [concat $listbox insert end $data]];

    # Retreive selection selction
    if {""!=$index} {
	$listbox selection clear 0 end;
	$listbox selection set $index;
    }

    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::hidecol - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::hidecol {path tag} {
    variable var;
    variable $path;

    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }

    foreachrow $path [set ${path}(colnumber,$tag)] wid {
	grid remove $wid;
    }

    grid columnconfigure $path$var(container)\
	    [set ${path}(colnumber,$tag)] -minsize 0;
    return;
}
    

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::showcol - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::showcol {path tag} {
    variable var;
    variable $path;

    if ![info exists ${path}(colnumber,$tag)] {
	uplevel 1 [list error "Unknown tag: $tag"];
    }

    set column [set ${path}(colnumber,$tag)];
    foreachrow $path $column wid {
	if {[winfo exists $wid]} {
	    grid $wid;
	}
    }

    # ? where is it set columnwidth related to column ? -->
    grid columnconfigure $path$var(container) $column\
	    -minsize [set ${path}($column,columnwidth)];

    return;
}
    

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::makeassoc - 
# 
# Returns     : 
#  Associative list containing one entry for each column in rtl_mlistbox
# Parameters  : 
#  path       - path of parent of listboxes
#  vallist    - list of values for resultlist.
# 
# Description : 
#  Buil associative list from $vallist. $vallist usually looks like result
#  from method 'get'. 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::makeassoc {path vallist} {
    
    checklength $path $vallist "makeassoc";
    
    set result [list];
    foreach key [get-tags $path] val $vallist {
	lappend result [list $key $val];
    }
    
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::makelist - 
# 
# Returns     : 
#  sorted list
# Parameters  : 
#  path       - path of parent of listboxes
#  alist      - associative list
# 
# Description : 
#  Reduce associative list $alist to sorted list according to column tags of
#  rtl_mlistbox. For Column tags not contained as key within $alist, empty list
#  elements are inserted.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::makelist {path alist} {

    # No comments in here for speedup reasons!!
    # The inner foreach loop emulates 'assoc'.
    foreach tag [get-tags $path] {
	set val {};
	foreach elem $alist {
	    if {$tag==[lindex $elem 0]} {
		set val [lindex $elem 1];
		break;
	    }
	}
	lappend result $val;
    }
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::size - 
# 
# Returns     : 
#  Size of rtl_mlistbox (number of lines).
# Parameters  : 
#  path       - path of parent of listboxes
# 
# Description : 
#  Get line count of rtl_mlistbox, that is the 'size' of the first listbox.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::size {path} {
    return [command0 $path list size]; 
}    

#############################################################################
#
#                         PRIVATE PROCEDURES
#
#############################################################################

# 
# procedures for execution of widget command existing for column item 
# for all columns
#
# ---------------------------------------------------------------------------
# Function    : foreachcol - loop through all listboxes
# 
# Returns     : NOTHING
# Parameters  : 
#  path       - path of parent of listboxes
#  type       - widget type in column: slider | head | list | widthhandle
#  w          - widget name of listbox (variable name in upper level).
#  idx        - index of listbox (column) (variable name in upper level).
#  body       - body to be executed.
# 
# Description : 
#  Loops over all listboxes contained in instance of rtl_mlistbox. After some
#  initial settings 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::foreachcol {path type w idx body} {
    upvar $w wid;
    upvar $idx i;
    variable var;
    variable $path;

    set columns [get-columns $path];
    set container $path$var(container);
    set type $var($type);
    for {set i 0} {$i < $columns} {incr i} {
	set wid "${container}${type}_$i";
	uplevel $body;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::foreachrow - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::foreachrow {path col w body} {
    upvar $w wid;
    variable var;
    variable $path;

    set container $path$var(container);
    foreach item {head list widthhandle} {
	set wid "${container}$var($item)_$col";
	uplevel $body;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::command - 
# 
# Returns     :
#  Sorted list of result of each widget command-execution.
# Parameters  : 
#  path       - path of parent of listboxes
#  type       - widget type in column: slider | head | list | widthhandle
#  command    - widget command to be executed
#  args       - arguments for command.
# 
# Description : 
#  Execute same command on specified component in each column of mlb.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::command {path type command args} {
    set args [join $args];
    set result [list];
    foreachcol $path $type widget i {
	lappend result [eval [concat $widget $command $args]];
    }
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::command0 - 
# 
# Returns     : 
#  result of command execution;
# Parameters  : 
#  path       - path of parent of listboxes
#  type  - widget type in column: slider | head | list | widthhandle
#  command    - widget command to be executed
#  args       - arguments for command.
# 
# Description : 
#  Execute command on first column if specified widget exists.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::command0 {path type command args} {
    variable var;
    set args [join $args];
    set widget $path$var(container)$var($type)_0;
    if [winfo exists $widget] {
	return [eval [concat $widget $command $args]];
    } else {
	return;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::tkCommand - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::tkCommand {path type command args} {
    set args [join $args];
    foreachcol $path $type widget i {
	eval lappend result [$command $widget $args];
    }
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::setoption - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::setoption {path type option vallist} {
    foreachcol $path $type widget i {
	set label [lindex $vallist $i];
	$widget configure -$option $label;
    }
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::getoption - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::getoption {path type option} {
    set result [list];
    foreachcol $path $type widget i {
	lappend result [$widget cget -$option];
    }
    return $result;
}	

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::adapt - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Resize canvas when frame was resized.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::adapt {path} {
    variable var;

    set ca $path$var(canvas);
    set fr $path$var(container);

    set offset [lindex [$ca coords $fr] 1]
    set he [expr {[winfo height $ca] - $offset}]
    set wi [winfo width $fr]
    set childid [$ca find withtag $fr]
    $ca itemconfigure $childid -height $he
    $ca configure -scrollregion "0 0 $wi $he"
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::checklength - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Check length of value list
# ---------------------------------------------------------------------------
proc rtl_mlistbox::checklength {path vallist action} {
    variable var;
    set colu [get-columns $path];
    set len [llength $vallist];
    if {$len != $colu} {
	error "Invalid list length to perform \"$action\".\
		Expected $colu items, got $len.";

    } else {
	return;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
#   Clear up data storage for widget instance and destroy widget.
# ---------------------------------------------------------------------------
proc rtl_mlistbox::destroy {base} {
    variable var;
    foreach elem [array names var $base,*] {
	unset var($elem);
    }

    default-destroy rtl_mlistbox $base;
}


# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::do-yscroll - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::do-yscroll {path widget args} {
    variable var;

    set pos [get-position $path];
    # If positon is not changed, don't do nothing.
    if {$args == $pos} {
	return;
    }

    # Store new position
    set-position $path $args;

    # Update listboxes
    ::eval yview $path "moveto [lindex $args 0]";
    activate $path [$widget index active];

    # Update scroll bar if command exists.
    set yscmd [get-yscrollcommand $path];
    if {"" != $yscmd} {

	#Ccommented update - causes strange scrollbar behaviour
	#update;
	::eval "$yscmd $args";
    }
    return;
}


# ===========================================================================
# Drag'n'Drop methods:
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::resize-start - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : procedures for column width resize per drag&drop
# ---------------------------------------------------------------------------
proc rtl_mlistbox::resize-start {path widget x y} {
    variable $path;
    variable var;
    
    # ? exactly ! ? -->

    ### must be marked for the current widget ?!
    ### you didn't use xresize anywhere:
    #set var(drag) 1;

    set ${path}(xresize,start) $x;

    # show slider at cursor position with size of window
    # set pos [winfo x $widget];

    # place $path$var(slider) -x $pos;
    resize-drag $path $widget $x $y;
    raise $path$var(slider);
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::resize-end - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See above
# ---------------------------------------------------------------------------
proc rtl_mlistbox::resize-end {path widget x y} {
    variable var;

    lower $path$var(slider);

    ### ask the grid which column is affected:
    set pare $path$var(container);
    set column [lindex [grid location $pare [winfo x $widget] 10] 0];
    set cursorcol \
	    [lindex [grid location $pare [expr {[winfo x $widget] + $x}] 10] 0];

    if {$cursorcol >= $column} {
	set old_size [lindex [grid bbox $pare $column 0] 2];
	set new_size [expr {$old_size + $x}];
    } else {
	set new_size 1;
    }

    colresize $path $column $new_size;

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::resize-drag - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See above
# ---------------------------------------------------------------------------
proc rtl_mlistbox::resize-drag {path widget x y} {
    variable var;

    set pos [expr [winfo x $widget] + $x];
    # ...the parent widget might be scrolled
    set pos [expr $pos + [winfo x [winfo parent $widget]]];

    # To be prevent user from moving size-cursor to a position left of
    # column he's actualy resizing, the procedure 'getsister' is needed!
    # set minpos [winfo x [getsister $widget list]];
    # set pos [expr ($minpos<$pos)?$pos:$minpos];

    place configure $path$var(slider) -x $pos;
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::colresize - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Comfortable resize of width of specific column
# ---------------------------------------------------------------------------
proc rtl_mlistbox::colresize {path column width} {
    variable var;

    set widget $path$var(container)$var(head)_$column;

    # Estimat new width
    switch -- $width {
	optimal { 
	    
	    # ? This is partly true since under windows
	    #   highlightthickness is also part of a widget
	    # ? -->

	    # make column as width as text. remind border of label
	    set text [$widget cget -text];
	    set textwidth [font measure [$widget cget -font] $text];
	    set borderwidth [expr 2 * [$widget cget -borderwidth]];
	    set newwidth [expr $borderwidth + $textwidth];
	}
	default {
	    # test for positive integer
	    if {[expr ![catch {format "%i" $width}]] && ($width>0) } { 
		set newwidth $width;
	    } else {
		uplevel 1 [list error "Bad column width \"$width\": must be optimal or a \
			positive integer"];
	    }
	}
    }

    # <-- ? There is a question sentence above for this set ... ? -->
    # Resize column
    set ${path}($column,columnwidth) $newwidth;
    grid columnconfigure $path$var(container) $column \
	    -minsize [set ${path}($column,columnwidth)];
    return;
}

# ------------#
# Drag & Drop #
# ------------#
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::drag-start - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Drag'n'Drop
# ---------------------------------------------------------------------------
proc rtl_mlistbox::drag-start {path widget x y} {
    variable var;

    set var($path,cursor) [$widget cget -cursor];
    set var($path,dragstart) [$widget curselection];

    set dstart [get-dragstart $path];

    if {-1 == $dstart || "" == $dstart} {

	# No drag and drop with zero entries
	return;
    }
    foreachcol $path list w i {
	$w configure -cursor icon;
    }

    # ? Why removing the bindings for all widgets ? -->
    bind Rtl_mlistbox_listbox <ButtonRelease> \
	    "rtl_mlistbox::drag-end $path %W %x %y";
    bind Rtl_mlistbox_listbox <B1-Motion> \
	    "rtl_mlistbox::drag-motion $path %W %x %y";

    # call the drag-start-callback
    #puts "START $var($path,dragstart)";
    ::eval [get-dragstartcommand $path];
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::drag-motion - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::drag-motion {path widget x y} {
    variable var;

    # call the drag-motion-callback
    if {$y < 0} {
	set motion -1;
    } else {
	set motion [$widget nearest $y];
    }

    set-dragmotion $path $motion;

    # Use listbox selection as feedback
    # ? Why multiple selection set ? -->
    foreachcol $path list w i {
	$w selection clear 0 end;
	$w selection set $motion;
	$w selection set [get-dragstart $path];
    }

    ::eval [get-dragmotioncommand $path];
    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::drag-end - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::drag-end {path widget x y} {
    variable var;

    # See comment drag-start ...
    bind Rtl_mlistbox_listbox <ButtonRelease> "";
    bind Rtl_mlistbox_listbox <B1-Motion> "";

    foreachcol $path list w i {
	$w configure -cursor $var($path,cursor);
    }

    set var($path,cursor) "";

    # call the drag-stop-callback
    if {$y < 0} {
	set stop -1;
    } else {
	set stop [$widget nearest $y];
    }

    set-dragend $path $stop;

    # Reselect the dragstart
    foreachcol $path list w i {
	$w selection clear 0 end;
	$w selection set [get-dragstart $path];
    }

    ::eval [get-dragendcommand  $path];

    # Reset drag values
    foreach acti [list start motion end] {
	set-drag$acti $path -1;
    }

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::button-press-cb - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::button-press-cb {widget x y time} {

    set path [winfo parent [winfo parent [winfo parent $widget]]];
    variable $path;

    set ${path}(clicktime) $time;

    bind Rtl_mlistbox_listbox <B1-Motion> \
	    "rtl_mlistbox::button-motion-cb $path %W %x %y %t";
    return;
}
# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::button-motion-cb - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_mlistbox::button-motion-cb {path widget x y time} {
    variable $path;

    if {[expr $time - [set ${path}(clicktime)]] > 400} {
	bind Rtl_mlistbox_listbox <B1-Motion> "";
	drag-start $path $widget $x $y;
    }
    return;
}




# ---------------------------------------------------------------------------
# Function    : rtl_mlistbox::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for widgets of type mlistbox.
#               Initialize instance specific variable
#
# ---------------------------------------------------------------------------
proc rtl_mlistbox::init {path} {
    variable var;
    variable $path;

   # set default columns
    set ${path}(columns) 0;
    set ${path}(yscrollcommand) {};	
    set ${path}(xscrollcommand) {};	
    set ${path}(selectcommand) {};	
    set ${path}(command) {};
    set ${path}(dragstartcommand) {};
    set ${path}(dragmotioncommand) {};
    set ${path}(dragendcommand) {};
    set ${path}(dragstart) -1;
    set ${path}(dragmotion) -1;
    set ${path}(dragend) -1;
    set ${path}(position) "";
    set ${path}(resizable) 1;
    set ${path}(tags) {};

    #set ${path}(background) [$path$var(canvas) cget -background];
    set ${path}(foreground) [$path$var(dummy) cget -foreground];
    set ${path}(selectbackground) [$path$var(dummy) cget -selectbackground];
    set ${path}(font) [$path$var(dummy) cget -font];
    set ${path}(borderwidth) 0;

    # configure default bg (to get windows look and feel)
    $path$var(canvas) configure -bg [$path$var(dummy) cget -bg];
    return;
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Todo :
# ------
# [1] check misc ?? inside of entire modul.
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 15.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

