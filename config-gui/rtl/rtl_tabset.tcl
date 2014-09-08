#!/bin/sh 
# start with WISH8.0 \
	exec wish8.0 "$0" ${1+"$@"} 
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary tabset Implementation
# -----------------------------------------------------------------------
# VisualGIPSY 2.5
package provide rtl_tabset 1.2;
package require rtl;

####VG-START####
####VG-CODE:normal####

# ---------------------------------------------------------------------------
# Template    : rtl_tabset
# 
# Attributes  :

# ---------------------------------------------------------------------------
namespace eval rtl_tabset {

    # Define the built-in arrays
    variable w;
    variable var;

    # Widget array for template rtl_tabset
    array set w [list 0 {} 1 .c 3 .c.f];

    # -----------------------------------------------------------------------
    # The template interface of rtl_tabset
    array set var [list  original frame \
	    attributes \
	    {show type offset taboffset borderspace orient\
	    autovisible width height background borderwidth relief\
	    highlightthickness highlightbackground highlightcolor\
	    tabfont tabbackground tabforeground tabwidth\
	    xscrollcommand xscrollincrement yscrollcommand yscrollincrement\
	}];

    array set var [list \
	    orient        orient\
	    show          items\
	    type          itemType\
	    offset        offset\
	    taboffset     taboffset\
	    borderspace   startX\
	    autovisible   autovisible\
	    tabbackground tabbackground\
	    tabforeground tabforeground\
	    tabfont       tabfont\
	    tabwidth      tabwidth\
	    ];


    # Make canvas attributes accessable ...
    foreach attr [list xscrollcommand xscrollincrement\
	    yscrollcommand yscrollincrement] {
	array set var [list $attr [list 1 $attr]];
    }

    foreach attr [list width height borderwidth relief\
	    highlightthickness highlightbackground highlightcolor] {
	array set var [list $attr [list 0 $attr]];
    }

    array set var [list background [list {0 background} {1 background}\
	    {3 background} background]];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset - template constructor
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
#               rtl_tabset. The Constructor may be used as each other
#               widget class.
# ---------------------------------------------------------------------------
proc rtl_tabset {{base .rtl_tabset} args} {
    variable ::rtl_tabset::w;
    variable ::rtl_tabset::var;

    if {![winfo exists $base]} {

	# -------------------------------------------------------------------
	# The widget hierarchy
	# -------------------------------------------------------------------
	# Look for creation class of template rtl_tabset:
	[getCreator rtl_tabset $base$w(0) $args] $base$w(0)  -class Rtl_tabset\
		;


	# Children of $base$w(0) := rtl_tabset (level:2)
	canvas $base$w(1) -borderwidth "1" -highlightthickness "0"\
		-takefocus 1 -width 60 -height 60 -relief "flat";

	# Redefine navigation keys but don't overwrite
	# Other application related keys.
	foreach k [list\
		Up Down Right Left plus minus Return Home End] {
	    bind $base$w(1) <${k}> \
		    "rtl_tabset::moveKey $base %K;break";
	}
	bind $base$w(1) <1> "focus $base$w(1)"

	# Allow Keyboard navigation in this tabset widget ...
	foreach key [list a b c d e f g h i j k l m n o p q r s t u v w x y z] {
		bind $base$w(1) <Any-${key}> "rtl_tabset::moveKey $base %K;break"
		set ukey [string toupper $key]
		bind $base$w(1) <Any-${ukey}> "rtl_tabset::moveKey $base %K;break"		
	}

    
        # -------------------------------------------------------------------
	# Graphic items of $base$w(1) = c
	if {[catch {

	    
	    $base$w(1) create rectangle -10.0 -100.0 2000.0 120.0\
		    -fill "white" -outline "" -stipple "gray50" -tags bg;

	    $base$w(1) create rectangle -10.0 60.0 2000.0 120.0\
		    -fill "white" -tags bl;

	    $base$w(1) create rectangle -10.0 62.0 2000.0 120.0\
		    -fill "gray" -outline "" -tags bd;

            # ---------------------------------------------------------------
	    # Tags
	    

	} err]} {
	    puts stderr $err;
	}


	# Children of $base$w(1) := c (level:3)
	frame $base$w(3) -relief "solid";
	place $base$w(3) -x 1 -y 100 -width -2 -relwidth 1 -height -102 -relheight 1;
	pack $base$w(1) -expand 1 -fill both;

	pack propagate $base 0;

	# -------------------------------------------------------------------
	# Call the init method
	if {[catch {
	    ::rtl_tabset::init $base;
	} msg]} {
	    # init does not exist.
	}

	# Additional Code
	

	# -------------------------------------------------------------------
	# Use the template interface for this instance
	set base [mkProc $base rtl_tabset $args];
    }

    return $base;
}


# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet!
# ---------------------------------------------------------------------------


namespace eval rtl_tabset {
 
    # option database items to template rtl_tabset
}

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,rtl_tabset) toplevel;
# Local used Packages:
set ::rat(PACKAGES,LOADED) [list ];
# ---------------------------------------------------------------------------

####VG-STOP####

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for template rtl_tabset.
# ---------------------------------------------------------------------------
proc rtl_tabset::init {path} {
    set c [find-widget c $path];
    $c bind tabItem <1> "$path selectat %x %y";  
}


# ---------------------------------------------------------------------------
# Function    : rtl_tabset::destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Remove all used items ...
# ---------------------------------------------------------------------------
proc rtl_tabset::destroy {path} {

    foreach item [get-names $path] {
	set id $path.<${item}>;
	$id close;
    }
}



# ---------------------------------------------------------------------------
# Function    : rtl_tabset::selectat - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Locate the item underneath the mouse cursor.
# ---------------------------------------------------------------------------
proc rtl_tabset::selectat {path x y} {
    set c [find-widget c $path];
    set x [$c canvasx $x];
    set y [$c canvasy $y];

    set id   [$c find closest $x $y];
    set item [string range [lindex [$c itemcget $id -tags] 0] 1 end];
    setselection $path $item;
}

# ---------------------------------------------------------------------------
# Generic Attributes for the tabset template.
# ---------------------------------------------------------------------------
create-action rtl_tabset {itemType  rtl_tabItem} {bbox {0 0 1 1}}\
	items selected names format {offset -5} {startX 10} {taboffset 50}\
	{orient horizontal} {distant 1} {autovisible 1} background disabled\
	{tabfont {Helvetica 11 bold}} {tabwidth 100} {tabyposition 0}\
	{tabbackground lightgray} {tabforeground black}\
	{tabheight 0} ;


# ---------------------------------------------------------------------------
# Function    : rtl_tabset::get-format - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get tab formatter.
# ---------------------------------------------------------------------------
proc rtl_tabset::get-format {path} {
    variable private::_[get-orient $path];

    set value [::get-action rtl_tabset $path format];
    if {![string compare {} $value]} {
	set value [set _[get-orient $path]];
    }

    return $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::set-orient - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : The tabset outline may be vertical or horizontal orient.
# ---------------------------------------------------------------------------
proc rtl_tabset::set-orient {path orient} {
    variable w;

    set c  [find-widget c $path];
    set bg [get-background $path];

    switch $orient {
	"vertical" {
	    $c delete all;
	    $c create rectangle -300.0 -10.0 120.0 3000.0\
		    -fill "white" -outline "" -stipple "gray50" -tags bg;

	    $c create rectangle 60 -10.0  120.0 3000.0\
		    -fill "white" -tags bl;

	    $c create rectangle 62 -10.0  150.0 3000.0\
		    -fill $bg -outline "" -tags bd;

	}
	"horizontal" {
	    $c delete all;
	    $c create rectangle -10.0 -100.0 3000.0 120.0\
		    -fill "white" -outline "" -stipple "gray50" -tags bg;

	    $c create rectangle -10.0 60.0 3000.0 120.0\
		    -fill "white" -tags bl;

	    $c create rectangle -10.0 62.0 3000.0 150.0\
		    -fill $bg -outline "" -tags bd;

	}
	default {
	    error "option orient must be vertical or horizontal not \"${orient}\"";
	}
    }

    ::set-action rtl_tabset $path orient $orient;
 
    private::pending $path redraw;
    return $orient;
}

proc rtl_tabset::set-background {path bg} {
    variable w;

    set c [find-widget c $path];

    $c itemconfigure bd -fill $bg;
    ::set-action rtl_tabset $path background $bg;
}


# ===========================================================================
# Private actions:
# ---------------------------------------------------------------------------

namespace eval rtl_tabset {
    namespace eval private {

	variable _horizontal;
	variable vertical;

	# -------------------------------------------------------------------
	# The visible item. 
	# -------------------------------------------------------------------
	#  1 := Canvas widget
	#  2 := background
	#  3 := foreground
	#  4 := item name
	#
	#  5 := x position
	#  6 := y position (not used, fix position)
	#  7 := width
	#  8 := height (not used, fix position)
	
	#  9 := text
	# 10 := icon
	# 11 := font (for displayed text)
	#
	# -------------------------------------------------------------------
	# Note: Use the setformat action to redefine the items format.
	#       and/or the format attribute of an item.
	# -------------------------------------------------------------------
	

	set _horizontal {
	    private::horizontal {%1$s} %2$s %3$s {%4$s} %5$s %6$s %7$s\
		    %8$s {%9$s} {%10$s} {%11$s};
	}

	set _vertical {
	    private::vertical {%1$s} %2$s %3$s {%4$s} %5$s %6$s %7$s\
		    %8$s {%9$s} {%10$s} {%11$s};
	}



	set _oldHorizontal {
	    set x1 %5$s;
	    set x2 [expr {%7$s/2 + $x1}]
	    set x3 [expr {%7$s + $x1}];
	    set x4 [expr {$x3 - 5}];
	    set x5 [expr {$x1 + 5}];
	    set xt [expr {$x4 - 5}];

	    # The Polygon ...
	    %1$s create polygon  $x1 60.0 $x2 90.0 $x3 60.0 $x4 30.0 $x5 30.0\
		    -fill "%2$s" -outline "%3$s"\
		    -tags [list %4$s back tabItem];

	    # The Text ...
	    %1$s create text  $xt 45.0 -anchor e -justify right\
		    -text "%9$s" -fill "%3$s" -font "%11$s"\
		    -tags [list %4$s text tabItem];

	    # The Icon ...
	    %1$s create image  $x5 45.0 -anchor w\
		    -image "%10$s"\
		    -tags [list %4$s image tabItem];
	}

	set _oldVertical {
	    set y1 %5$s;
	    set y2 [expr {%7$s/2 + $y1}]
	    set y3 [expr {%7$s + $y1}];
	    set y4 [expr {$y3 - 5}];
	    set y5 [expr {$y1 + 5}];
	    set yt [expr {$y4 - 5}];

	    # The Polygon ...
	    %1$s create polygon  60 $y1 90 $y2 60 $y3 30 $y4 30 $y5\
		    -fill "%2$s" -outline "%3$s"\
		    -tags [list %4$s back tabItem];

	    # The Text (not usable in vertical mode).
	    # Text should be placed in the associated image
	    # to get vertical oriented text ...

# 	    %1$s create text  45 $yt -anchor se -justify right\
# 		    -text "%9$s" -fill "%3$s" -font "%11$s"\
# 		    -tags [list %4$s text tabItem];

	    # The Icon (including text)...
	    %1$s create image  45 $y5 -anchor n\
		    -image "%10$s"\
		    -tags [list %4$s image tabItem];
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::private::horizontal - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : procedure to render the registers in horizontal
#               fashion.
# ---------------------------------------------------------------------------
proc rtl_tabset::private::horizontal {c bg fg item xp yp wd hg text image ft} {

	    set x1 $xp;
	    set x2 [expr {$wd/2 + $x1}]
	    set x3 [expr {$wd + $x1}];
	    set x4 [expr {$x3 - 5}];
	    set x5 [expr {$x1 + 5}];
	    set xt [expr {$x4 - 5}];

	    # The Polygon ...
	    $c create polygon  $x1 60.0 $x2 90.0 $x3 60.0 $x4 30.0 $x5 30.0\
		    -fill $bg -outline $fg\
		    -tags [list $item back tabItem];

	    # The Text ...
	    $c create text  $xt 45.0 -anchor e -justify right\
		    -text $text -fill $fg -font $ft\
		    -tags [list $item text tabItem];

	    # The Icon ...
	    $c create image  $x5 45.0 -anchor w\
		    -image $image\
		    -tags [list $item image tabItem];

}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::private::vertical - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : procedure to place registers vertical, a procedure was
#               chosen to get the byte compile bonus ... 
# ---------------------------------------------------------------------------
proc rtl_tabset::private::vertical {c bg fg item xp yp wd hg text image ft} {

    set y1 $xp;
    set y2 [expr {$wd/2 + $y1}]
    set y3 [expr {$wd + $y1}];
    set y4 [expr {$y3 - 5}];
    set y5 [expr {$y1 + 5}];
    set yt [expr {$y4 - 5}];
    
    # The Polygon ...
    $c create polygon  60 $y1 90 $y2 60 $y3 30 $y4 30 $y5\
	    -fill $bg -outline $fg\
	    -tags [list $item back tabItem];
    
    # The Text (not usable in vertical mode).
    # Text should be placed in the associated image
    # to get vertical oriented text ...
    
# 	    $c create text  45 $yt -anchor se -justify right\
# 		    -text $text -fill $fg -font $ft\
# 		    -tags [list $item text tabItem];
    
    # The Icon (including text)...
    $c create image  45 $y5 -anchor n\
	    -image $image\
	    -tags [list $item image tabItem];

}


# ---------------------------------------------------------------------------
# Function    : rtl_tabset::private::propagate - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Propagate specific actions from subwidgets to the
#               general interface of rtl_tabset.
#               NOTE: propagate can be reused in other templates.
# ---------------------------------------------------------------------------
proc rtl_tabset::private::propagate {path widget action args} {
    set w [find-widget $widget $path];
    return [::eval [linsert $args 0 $w $action]];
}

proc rtl_tabset::private::orient {path} {
    switch [$path cget -orient] {
	"horizontal" {
	    set id1 0;
	    set cmd xview;
	}
	"vertical" {
	    set id1 1;
	    set cmd yview;
	}
    }

    return [list $id1 $cmd];
}

proc rtl_tabset::private::pending {base type} {

    set ncu [namespace current]
    set nsp [namespace parent $ncu];
    variable ${nsp}::$base;

    if {![info exists ${base}(pending,$type)]} {
	set  ${base}(pending,$type) 1;
	after idle "$base $type;\
		\n${ncu}::remove ${nsp}::${base} pending,$type";
    }
}

proc rtl_tabset::private::isPending {base type} {
      set ncu [namespace current]
      set nsp [namespace parent $ncu];
     variable ${nsp}::$base;

      set result 0;
      if {[info exists ${base}(pending,$type)]} {
  	set result [set ${base}(pending,$type)];
      }

      return $result;
}

#  proc rtl_tabset::private::removePending {base type} {
#      set ncu [namespace current]
#      set nsp [namespace parent $ncu];
#      remove ${nsp}::$base pending,type;
#  }

proc rtl_tabset::private::remove {base item} {
    variable $base;

    if {[info exists ${base}($item)]} {
	unset ${base}($item);
    }
}



# ===========================================================================
# Public Actions of rtl_tabset.
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_tabset::yview - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Send yview actions to the canvas widget
# ---------------------------------------------------------------------------
proc rtl_tabset::yview {path args} {
    return [::eval [linsert $args 0 private::propagate $path c yview]];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::xview - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Send xview actions to the canvas widget
# ---------------------------------------------------------------------------
proc rtl_tabset::xview {path args} {
    return [::eval [linsert $args 0 private::propagate $path c xview]];
}


# ---------------------------------------------------------------------------
# Function    : rtl_tabset::setformat - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Setformat is intended to reformat the current tab
#               outline. If no format is specified the hrizontal or
#               verical definition from the private namespace will be
#               used. Setformat is similar to other attributes but
#               due it's extend not defineable on the command line.
#               NOTE: the given format must corrospond with the
#               orientation of the tabset widget. 
# ---------------------------------------------------------------------------
proc rtl_tabset::setformat {path value} {
    set-format $path $value;
    private::pending $path redraw;
    return [get-format $path];
}

###
# FIXME
###
# ---------------------------------------------------------------------------
# Function    : rtl_tabset::size - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Gives the current size of associated items.
# ---------------------------------------------------------------------------
proc rtl_tabset::size {path} {
    return [llength [get-items $path]];
}


# ===========================================================================
# Tab interface to rtl_tabItem or whatever item you are about to use
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_tabset::tab - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Wrapper, access methode for the included items.
#               Note the used items need not to be of type rtl_tabItem.
#               The items inside of a rtl_tabset have to be composed
#               of the same template type and cannot being changed
#               after an single item was added to the tabset.
# ---------------------------------------------------------------------------
proc rtl_tabset::tab {path action item args} {

    set result {};
    set names [get-names $path];
    set id $path.<$item>;

    switch $action {
	"cget" -
	"configure" {

	    if {[lsearch -exact $names $item] < 0} {
		error "Tabset tab $action : \"${item}\" does not exist!";
	    }
	    
	    # Call the template table for this item ...
	    set result [::eval [linsert $args 0 $id $action]];

	}
	"add" {
	    if {[lsearch -exact $names $item] >= 0} {
		error "Tabset tab $action : \"${item}\" must not exist!";
	    }	    
	    lappend names $item;
	    set result  [::eval [linsert $args 0 [get-itemType $path]\
		    open $id]];

	}
	"delete" {

	    set i [lsearch -exact $names $item];
	    if {$i >= 0} {

		set visual [get-items $path];
		set names  [lreplace $names $i $i];

		# Is the item visible ?
		set j [lsearch -exact $visual $item];
		if {$j >= 0} {
		    set visual  [lreplace $visual $j $j];
		    set-items $path $visual;
		}

		$id close;
		
	    }
	}
    }

    # Use a better approeach to redraw "modified items -- 
    # add $item to the redraw call" ...

    set-names $path $names;

    if {[get-autovisible $path]} {
	
	# keep visible and non visible items in sync ...
	set-items $path $names;
    }

    private::pending $path redraw;

    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::entryconfigure - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Allow a menu item equally behavior.
# ---------------------------------------------------------------------------
proc rtl_tabset::entryconfigure {path item args} {
    set result {};
    set nl [::eval [linsert $args 0 $path tab configure $item]];
    if {[llength $args] == 0} {
	foreach item $nl {
	    lappend result [linsert $item 1 {} {} {}];
	}
    } elseif {[llength $args] == 1} {
	set result [linsert $nl 1 {} {} {}];
    }
    return $result;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::entrycget - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See above ...
# ---------------------------------------------------------------------------
proc rtl_tabset::entrycget {path item attr} {
    return [tab $path cget $item $attr];
}

proc rtl_tabset::add {path item args} {
    return [::eval [linsert $args 0 tab $path add $item]];
}

proc rtl_tabset::delete {path item args} {
    return [::eval [linsert $args 0 tab $path delete $item]];
}



proc rtl_tabset::type {path args} {
    return [get-itemType $path];
}


proc rtl_tabset::tabnames {path} {
    return [get-names $path];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::curselection - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get current selected item
# ---------------------------------------------------------------------------
proc rtl_tabset::curselection {path} {
    return [get-selected $path]
}


# ---------------------------------------------------------------------------
# Function    : rtl_tabset::setselection - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Activate the given item. Flag is used to invoke the
#               item. Since setselection is also used by other
#               methodes flag will ensure not to notify the item about
#               this internal stuff.  
# ---------------------------------------------------------------------------
proc rtl_tabset::setselection {path item {flag 1}} {
    set c     [find-widget c $path];
    set items [get-items $path];

    if {[lsearch $items $item] > -1} {

	if {[private::isPending $path redraw] && $flag} {
	    #redraw $path;
	    #return;
	}

	# Send all Items to the background.
	$c raise tabItem bg;
	foreach disabledItem [get-disabled $path] {
	    $c lower _$disabledItem bg;
	}

	set id $path.<${item}>;

	# prevent from multiple reinvokations ...
	# Is it allowed to select this item ?
	if {!$flag || ![$id invoke]} {
	    set item [get-selected $path];
	} else {
    
	    # Remove previsious widgets ...
	    set f [find-widget f $c];
	    foreach sl [pack slaves $f] {
		pack forget $sl;
	    }

	    set wg [$id cget -widget];
	    if {[winfo exists $wg]} {
		pack $wg -in $f -expand 1 -fill both -padx 3 -pady 3;
		raise $wg;
	    }
	}

	set-selected $path $item;

	# Bring item to the top.
	$c raise _$item;
	set sx   [get-taboffset $path];
	set bb   [$c bbox _$item];
	set bbox [get-bbox $path];

	if {[llength $bb]} {

	    foreach {id1 cmd} [private::orient $path] break;

	    set id2    [expr {$id1 + 2}];
	    set xyp1   [expr {[lindex $bb $id1] - $sx}];
	    set xyp2   [expr {$sx + [lindex $bb $id2]}];
	    set last   [lindex $bbox $id2];
	    set first  [lindex $bbox $id1];
		
	    set view   [$c $cmd];
	    set region [expr {double($last - $first)}];
	    set p1     [expr {$xyp1 / $region}];
	    set p2     [expr {$xyp2 / $region}];
	    
	    if {$p2 > [lindex $view 1]} {
		# Outer right, scroll ...
		if {$p2 > 1} {
		    set p2 1.0;
		}
		$c $cmd moveto [expr {$p2 - [lindex $view 1] +\
			[lindex $view 0]}];	    
		
	    } elseif {$p1 < [lindex $view 0]} {
		
		if {$p1 < 0} {
		    set p1 0.0;
		}
		
		# Outer left, scroll ...
		$c $cmd moveto $p1;
		
	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::moveKey - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Keyboard navigation through tabset pages ...
# ---------------------------------------------------------------------------
proc rtl_tabset::moveKey {base {key up}} {
    variable $base;

    set items   [get-items $base];
    set current [curselection $base];
    set id      [lsearch $items $current];
    set lg      [llength $items];
    set ot 1;

    if {$id > -1} {
	switch $key {
	    "Left" -
	    "Up" {
		set ot -1;
		incr id -1;
	    }
	    "Right" -
	    "Down" {
		incr id 1;
	    }
	    "Home" {
		set id 0;
	    }
	    "End" {
		set ot -1;
		set id [expr { $lg - 1}];
	    }
	    default {
		foreach item $items {
		    set txt [$base.<${item}> cget -text];
		    if {![string compare $key \
			    [mkFirstLow [string range $txt 0 0]]]} {
			set id [lsearch $items $item];
			break;
		    }
		}
	    }
	}
    }

    if {$id > -1 && $id < $lg} {
	set tab   [lindex $items $id];
	set item  $base.<${tab}>;
	set state [$item cget -state];

	# item is disabled, use ot to find the 
	# suited one.

	while {![string compare "disabled" $state]} {

	    incr id $ot;

	    if {$id >= $lg} return;
	    if {$id < 0}    return;

	    set tab   [lindex $items $id];
	    set item  $base.<${tab}>;
	    set state [$item cget -state];
	}

	setselection $base $tab;
	see $base $tab;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::redraw - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Used for the entire display stuff, also the contained
#               child widget managment widget will be replaced.
# ---------------------------------------------------------------------------
proc rtl_tabset::redraw {path} {
    variable w;
    
    set c   [find-widget c $path];

    set off [get-offset $path];
    set xp  [get-startX $path];
    set fmt [get-format $path];

    $c delete tabItem;

    set disabled [list];
    foreach item [get-items $path] {

	set id $path.<${item}>;
	set myFmt $fmt;

	if {[catch {
	    set cfg [$id configure];

	    # Improve go back to the configure stuff,
	    # multiple cgets are time wasting at all.

	    set tx [$id cget -text];
	    set ig [$id cget -image];
	    set st [$id cget -state];

	    set hg 0;
	    set yp 0;

	    foreach {var attr} [list \
		    yp yposition\
		    hg height\
		    ft font \
		    bg background \
		    fg foreground\
		    wd width] {

		# Ensure that this attribute is availabe ...
		set nb  [lsearch -regexp $cfg "-$attr "];
		set $var {};
		if {$nb >= 0} {
		    set $var [$id cget -$attr];
		}

		# Is there an individual value ...
		if {![string compare {} [set $var]]} {
		    # Use the default value from the tabset ...
		    set $var [get-tab$attr $path];
		}

	    }

	    # New Feature : allow to specify the format for an
	    # individual page ...

	    set nb  [lsearch -regexp $cfg "-format "];
	    if {$nb >= 0} {
		set myFmt [$id cget -format];
		if {![string compare {} $myFmt]} {
		    set myFmt $fmt;
		}
	    }
	    

	} msg]} {

	    # Don't do anything here
	    # Simple assume initialization stuff
	    # isn't ready yet.
	    
	    # FIXME
	    # See also the tab and set-insert procedures.

	    puts stderr "Redraw is called with insufficient\
		    informations ... (${item})"
	    return;
	}

	# Create them all ...
	::eval [format $myFmt\
		$c $bg $fg _$item $xp $yp $wd $hg $tx $ig $ft];

	if {![string compare "disabled" $st]} {
	    lappend disabled $item;
	}

	# Recalculate next position ...
	set xp [expr {$xp + $off + $wd}];
    }

    # Send all Items to the background
    $c raise tabItem bg;

    # Calculate Bounding box ...
    set sx   [get-startX $path];
    set bb   [$c bbox tabItem];
    set dist [get-distant $path];

    set cmd yview;

    if {[catch {

	switch [get-orient $path] {
	    "horizontal" {
		set lbb [list 0 "-$sx" 2 $sx 1 "-$sx" 3 $dist];
	    }
	    "vertical" {
		set lbb [list 0 "-$sx" 3 $sx 1 "-$sx" 2 $dist];
		set cmd xview;
	    }
	}

	foreach {id val} $lbb {
	    set bb [lreplace $bb $id $id\
		    [expr {[lindex $bb $id] + $val}]];
	}
	    
    } msg]} {

	# Does happen if no tabItems are part of the tabset ...
	return;
    }

    # Store actual bbox for reuse (moveto) ...
    set-bbox $path $bb;
    #if {[catch {
	switch [get-orient $path] {
	    "horizontal" {
		set x  1;
		set y  [expr {[lindex $bb 3] - [lindex $bb 1] + $dist}];
	    }
	    "vertical" {
		set x  [expr {[lindex $bb 2] - [lindex $bb 0] + $dist}];
		set y  1;	    
	    }
	}

	set hg [expr {-1 * ($y + 1)}];
	set wd [expr {-1 * ($x + 1)}];
	
	place configure $path$w(3) -x $x -y $y -width $wd -height $hg;
    
#      } msg]} {
#  	return;
#      }

    # Assign the 
    $c configure -scrollregion $bb;
    $c $cmd moveto 0;

    set-disabled $path $disabled;
    setselection $path [get-selected $path] 0;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tabset::see - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Bring the given item into the visible section. If the
#               given item is already visible no action will be performed.
# ---------------------------------------------------------------------------
proc rtl_tabset::see {path item} {

    # In contrast to the previsious selection
    # try to scroll this item to the center of
    # the tabset.

    set c       [find-widget c $path];
    set bb      [$c bbox _$item];
    set bbox    [get-bbox $path];

    if {[llength $bb]} {

	foreach {id1 cmd} [private::orient $path] break;

	set id2    [expr {$id1 + 2}];
	set xyp1   [lindex $bb $id1];
	set xyp2   [lindex $bb $id2];
	set last   [lindex $bbox $id2];
	set first  [lindex $bbox $id1];
	set view   [$c $cmd];
	
	set region [expr {double($last - $first)}];
	set p1     [expr {$xyp1 / $region}];
	set p2     [expr {$xyp2 / $region}];
	
	if {$p2 > [lindex $view 1] || $p1 < [lindex $view 0]} {
	    
	    set pdiff [expr {$p2 - $p1}];
	    set vdiff [expr {[lindex $view 1] - [lindex $view 0]}];
	    set p     [expr {($vdiff - $pdiff) / 2.0}];
	    
	    $c $cmd moveto [expr {$p1 - $p}];
	}
    }
}


# ===========================================================================
# A tabset is useless without any tabs in it.
# So we have to provide the tabset with a distinct type of "Items"
# The RTL Template is good suited for this purpose.
# Following is the implementation of an item template.
# ---------------------------------------------------------------------------
## The Item implementation ...
# ---------------------------------------------------------------------------
namespace eval rtl_tabItem {}

# ---------------------------------------------------------------------------
# Function    : rtl_tabItem - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Constructor of rtl_tabItem.
# ---------------------------------------------------------------------------
proc rtl_tabItem {{action {names}} {ext {}} args} {
    return [eval "rtlparser rtl_tabItem \"$action\" \"$ext\" $args"];
}


create-action rtl_tabItem background foreground\
	{text {Tab Item}} {image {}} variable {onvalue 1}\
	font format {state normal} widget width command;


# ---------------------------------------------------------------------------
# Function    : rtl_tabItem::invoke - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Callback handler, used to set variable and executes an
#               associated command. The Item state will be asked
#               before any action is performed. 
# ---------------------------------------------------------------------------
proc rtl_tabItem::invoke {item args} {

    set result 0;

    switch [get-state $item] {
	"normal" {
	    set v [get-variable $item];

	    # Is there a variable associated with ...
	    if {[string compare {} $v]} {
		set on [get-onvalue $item];
		set $v $on;
	    }

	    # Handle the associated command ...
	    ::eval [get-command $item];
	    set result 1;
	}
    }
    return $result;
}

##
## TODO :
##        [1] Convert tabItem usage to tabItem List usage
##        [2] Define the entire interface for the template
##        [3] Integration in VG.
##            <a> Define a Subeditor in VG to create lists and items.
##            <b> Extend code generation for tabItems and lists.
 





