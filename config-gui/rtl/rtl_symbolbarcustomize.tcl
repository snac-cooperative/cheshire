# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary VG Preferences Settings
# -----------------------------------------------------------------------
package provide rtl_symbolbarcustomize 1.2;
package require rtl;
package require rtl_customize;
package require rtl_images;

####VG-START####
set rtl_symbolbarcustomize(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_symbolbarcustomize; 
	set src [file join $rtl_symbolbarcustomize(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_symbolbarcustomize.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_symbolbarcustomize 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_symbolbarcustomize {
 
    variable var; 
    variable w; 
    array set w [list 4 .select.menubutton.menu 0 {} 1 .select 5 .select.apply 2 \
        .select.entry 6 .select.del 3 .select.menubutton 7 .customize]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  treebackground {7 treebackground} \
        highlightbackground [list [list 0 highlightbackground]\
        [list 1 highlightbackground] [list 7 highlightbackground]] \
        .rtl_symbolbarcustomize,creator frame height {0 height} \
        disabledforeground [list [list 3 disabledforeground]\
        [list 5 disabledforeground] [list 6 disabledforeground]] \
        highlightthickness [list [list 0 highlightthickness]\
        [list 1 highlightthickness] [list 7 highlightthickness]] \
        currentvar [list {2 textvariable} {3 textvariable}] nopesize [list \
        [list 7 nopesize]] original frame symbolbars symbolbars pattern \
        pattern symbolbarsvar symbolbarsvar attributes [list borderwidth\
        relief root currentvar symbolbars symbolbarsvar readonly\
        variable pattern background height width highlightbackground\
        highlightcolor highlightthickness selectforeground\
	selectbackground activebackground\
        activeforeground disabledforeground foreground\
        treebackground nopebackground nopesize] highlightcolor [list \
        [list 0 highlightcolor] [list 1 highlightcolor]\
        [list 7 highlightcolor]]\
        background [list [list 0 background] [list 1 background]\
        [list 5 background] [list 6 background] [list 7 background]\
        [list 3 background]] width {0 width} nopebackground [list \
        [list 7 nopebackground]] foreground [list [list 3 foreground]\
        [list 5 foreground] [list 6 foreground]] root {7 root} \
        activebackground [list [list 4 activebackground] \
	[list 5 activebackground] [list 6 activebackground] \
	[list 3 activebackground] [list 7 activebackground]]\
        selectbackground [list {2 selectbackground} [list 7\
	selectbackground]] readonly \
        readonly variable variable borderwidth {0 borderwidth} relief [list \
        [list 0 relief]] activeforeground [list [list 4\
	activeforeground] [list 7 activeforeground] [list 5\
	activeforeground] [list 6 activeforeground] [list 3 activeforeground]] \
	selectforeground [list [list 2 selectforeground]  \
	[list 7 selectforeground]]]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_symbolbarcustomize - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -borderwidth
#             : -relief
#             : -root
#             : -currentvar
#             : -symbolbars
#             : -symbolbarsvar
#             : -readonly
#             : -variable
#             : -pattern
#             : -background
#             : -height
#             : -width
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -activebackground
#             : -activeforeground
#             : -disabledforeground
#             : -foreground
#             : -font
#             : -treebackground
#             : -nopebackground
#             : -nopesize 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_symbolbarcustomize 
# --------------------------------------------------------------------------- 
proc rtl_symbolbarcustomize {{base .rtl_symbolbarcustomize} args} {
 
    variable rtl_symbolbarcustomize::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_symbolbarcustomize $base$w(0) $args] $base\
		-width 420 -height 240\
		-class Rtl_symbolbarcustomize -relief "raised";

        # Childs to $base$w(0), rtl_symbolbarcustomize of class:
 	# Rtl_Symbolbarcustomize
        frame $base$w(1);

        # Childs to $base$w(1), select of class: 	Frame
        entry $base$w(2);
        menubutton $base$w(3) -direction "flush" -indicatoron "1" -menu\
            $base$w(4) -relief "raised";

        # Childs to $base$w(3), menubutton of class: 	Menubutton
        menu $base$w(4) -tearoff "0";

        ### skip geometry for $base$w(4) (Menu)
        button $base$w(5) -command "rtl_symbolbarcustomize::apply $base$w(0)" \
		-width "8";
        button $base$w(6) -command "rtl_symbolbarcustomize::del $base$w(0)" \
		-width "8";

	pack propagate $base 0;
        pack $base$w(2) -anchor center -expand 0 -fill both -ipadx 0\
             -ipady 0 -padx 5 -pady 5 -side left;
        pack $base$w(3) -anchor center -expand 0 -fill y -ipadx 0\
             -ipady 0 -padx 5 -pady 5 -side left;
        pack $base$w(5) -anchor center -expand 0 -fill y -ipadx 0\
             -ipady 0 -padx 5 -pady 5 -side right;
        pack $base$w(6) -anchor center -expand 0 -fill y -ipadx 0\
             -ipady 0 -padx 5 -pady 5 -side right;
        rtl_customize $base$w(7) -borderwidth "1" -relief "raised";

        pack $base$w(1) -anchor center -expand 0 -fill x -ipadx 0\
             -ipady 0 -padx 5 -pady 0 -side top;
        pack $base$w(7) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 5 -pady 5 -side top; 
        set base [mkProc $base rtl_symbolbarcustomize $args];  
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------
option add *Rtl_symbolbarcustomize.select.entry.help {Enter new Symbolbar name and create it using Apply|Create new}
option add *Rtl_symbolbarcustomize.select.menubutton.help {Select Symbolbar to modify|Select}
option add *Rtl_symbolbarcustomize.select.menubutton.menu.title {Select Symbolbar}
option add *Rtl_symbolbarcustomize.select.apply.text Apply
option add *Rtl_symbolbarcustomize.select.apply.help {Save current settings|Save settings}
option add *Rtl_symbolbarcustomize.select.del.text Delete
option add *Rtl_symbolbarcustomize.select.del.help {Delete current selected Symbolbar|Delete Symbolbar}



# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set rat(WIDGETCLASS,rtl_symbolbarcustomize) rtl_symbolbarcustomize;
# ---------------------------------------------------------------------------

####VG-STOP####

option add *Rtl_symbolbarcustomize.select.entry.help\
	{Enter new Symbolbar name and create it using\
	Apply|Create new}
option add *Rtl_symbolbarcustomize.select.menubutton.help\
	{Select symbolbar to modify|Select}
option add *Rtl_symbolbarcustomize.select.apply.help\
	{Save current settings|Save settings}
option add *Rtl_symbolbarcustomize.select.del.help\
	{Delete current selected Symbolbar|Delete Symbolbar}


# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_symbolbarcustomize\
	variable pattern readonly symbolbarsvar symbolbars;


# ===========================================================================
# Overwritten Generic Attributes (set methods):
# ---------------------------------------------------------------------------
# Function    : rtl_symbolbarcustomize::set-symbolbars - get-symbolbars 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_symbolbarcustomize::set-symbolbars {base value} {
    variable w;

    ::set-action rtl_symbolbarcustomize $base symbolbars $value;
    set li [$base$w(4) index last];
    if {"none" != $li} {
	for {set i $li} {$i >= 0} {incr i -1} {
	    catch {$base$w(4) delete $i};
	}
    }

    foreach item $value {
	$base$w(4) add command\
		-label [mkFirstUp $item]\
		-command "rtl_symbolbarcustomize::select $base $item";
    }

    catch {
	set v [get-symbolbarsvar $base];
	set $v $value;
    }

    return $value;
}

# ===========================================================================
# Public Methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_symbolbarcustomize::select - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Select callback from menu
# ---------------------------------------------------------------------------
proc rtl_symbolbarcustomize::select {base bar} {
    variable w;

    set v [$base$w(2) cget -textvariable];
    variable $v;
    set $v $bar;
    
    # open options for that bar;
    set v [get-variable $base];
    variable $v;
    set pat [get-pattern $base];
    set name $pat,[mkFirstUp $bar],Options;
    $base$w(7) setitems [set ${v}($name)];
}



# ---------------------------------------------------------------------------
# Function    : rtl_symbolbarcustomize::apply - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Make current changes permanent (must be done before
#               closing the dialog)
# ---------------------------------------------------------------------------
proc rtl_symbolbarcustomize::apply {base} {
    variable w;

    set v [$base$w(2) cget -textvariable];
    variable $v;
    set bar [set $v];
    set bar [mkFirstUp $bar];

    set readonly [get-readonly $base];
    if {[lsearch $readonly $bar] >= 0} {
	return;
    }

    # open options for that bar;
    set v [get-variable $base];
    variable $v;
    set pat [get-pattern $base];
    
    set name $pat,$bar,Options;
    set options [$base$w(7) items];
    set ${v}($name) [list "-items" $options];
    set symbars [get-symbolbars $base];
    if {[lsearch -exact $symbars $bar] < 0} {
	lappend symbars $bar;
	set-symbolbars $base $symbars;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_symbolbarcustomize::del - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Delete current selected bar from the Symbolbar list
# ---------------------------------------------------------------------------
proc rtl_symbolbarcustomize::del {base} {
    variable w;

    set v [$base$w(2) cget -textvariable];
    variable $v;
    set bar [mkFirstUp [set $v]];
    set readonly [get-readonly $base];
    if {[lsearch $readonly $bar] >= 0} {
	return;
    }
   
    set symbars [get-symbolbars $base];
    set id [lsearch -exact $symbars $bar];
    if {$id >= 0} {
	set symbars [lreplace $symbars $id $id];
	set-symbolbars $base $symbars;

	set v [get-variable $base];
	variable $v;
	set pat [get-pattern $base];
	set name $pat,$bar,Options;
	catch {
	    unset ${v}($name);
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

