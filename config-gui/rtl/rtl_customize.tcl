# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary VG Preferences Settings
# -----------------------------------------------------------------------
package provide rtl_customize 1.1;

foreach pkg [list rtl rtl_imagelist rtl_tree rtl_gridwin] {
    package require $pkg
}

####VG-START####
set rtl_customize(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_customize; 
	set src [file join $rtl_customize(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_customize.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_customize 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_customize {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 1 .menutree 2 .imagetree 3 .work 4 .work.addcmd 5 \
        .work.delcmd 6 .work.text 7 .work.gridwin 8 .work.gridwin.hori\
        9 .work.gridwin.vert 10 .xhandle_ 11 .xnope_ 12 .work.entry]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  treebackground\
        [list {1 background}] highlightbackground [list \
        [list 0 highlightbackground]] height {0 height} \
        highlightthickness {0 highlightthickness} nopesize [list \
        [list 11 height] [list 11 width]] original frame \
        .rtl_customize,select 0 highlightcolor {0 highlightcolor}\
        attributes [list borderwidth relief root background width height\
        highlightbackground highlightcolor highlightthickness \
	selectforeground selectbackground activeforeground activebackground\
        treebackground nopebackground nopesize] \
	selectforeground [list {12 selectforeground} selectforeground]\
	selectbackground [list {12 selectbackground}\
	{2 selectbackground} selectbackground]\
	activeforeground [list {2 activeforeground} activeforeground]\
	activebackground [list {2 activebackground} activebackground]\
	background [list \
        [list 0 background] [list 4 background] [list 5 background]\
        [list 7 background] [list 6 background]] width {0 width} items \
        options nopebackground {11 background} root root \
        .rtl_customize,index 0 .rtl_customize,creator frame borderwidth [list \
        [list 0 borderwidth]] relief {0 relief}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_customize - widget constructor 
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
#             : -background
#             : -width
#             : -height
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -treebackground
#             : -nopebackground
#             : -nopesize 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_customize 
# --------------------------------------------------------------------------- 
proc rtl_customize {{base .rtl_customize} args} {
 
    variable rtl_customize::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_customize $base$w(0) $args] $base\
		-class Rtl_customize -relief "raised";

        # Childs to $base$w(0), rtl_customize of class: 	Rtl_Customize
        rtl_tree $base$w(1) -char "." -indentx "32"\
		-indenty "24" -root "0" \
		-connect 1 -width "120" -height "200" \
		-fiximagewidth 0 -format "%s" -itemselect "0" \
		-visible "label text title class geometry manager"\
		-drag 0 -sides "bottom right";

        rtl_imagelist $base$w(2) -height 100 -width 48 -imagewidth 24;

        frame $base$w(10) -cursor "sb_h_double_arrow" -relief "raised";
        foreach pair [list \
            [list <B1-Motion> "resize-drag $base$w(0) %W %x %y"]\
            [list <Button-1> "resize-start $base$w(0) %W %x %y"]\
            [list <ButtonRelease-1> "resize-end $base$w(0) %W %x %y"]] {
             bind $base$w(10) [lindex $pair 0] [lindex $pair 1]; 
        }

        frame $base$w(11) -borderwidth "1" -cursor "sb_h_double_arrow" \
            -height 6 -relief "raised" -width "6";
        foreach pair [list \
            [list <B1-Motion> "resize-drag $base$w(0) $base$w(10) %x %y"]\
            [list <Button-1> "resize-start $base$w(0) $base$w(10) %x %y"]\
            [list <ButtonRelease-1> "resize-end $base$w(0) $base$w(10) %x %y"]] {
             bind $base$w(11) [lindex $pair 0] [lindex $pair 1]; 
        }

        frame $base$w(3);

        # Childs to $base$w(3), work of class: 	Frame
        button $base$w(4) -command "rtl_customize::add $base$w(0)" -width "8";
        button $base$w(5) -command "rtl_customize::del $base$w(0)" -width "8";
        entry $base$w(12);
        rtl_gridwin $base$w(7) -widget "$base$w(6)" -auto "1" -fill "both" \
            -sides "bottom right" -width "120" -height "78";
        text $base$w(6) -borderwidth "0" -cursor "arrow" -exportselection\
            0 -height "1" -highlightthickness "0" -state\
            disabled -xscrollcommand\
            "rslider $base$w(6) $base$w(8) x 1; $base$w(8) set" \
            -yscrollcommand\
            "rslider $base$w(6) $base$w(9) y 1; $base$w(9) set";
        # textentries of $base$w(6):
        grid $base$w(4) -column 0 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(5) -column 0 -row 1 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(12) -column 1 -row 0 -columnspan 2 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(7) -column 1 -row 1 -columnspan 2 -rowspan 2\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
	
        #grid propagate $base$w(7) 0;
        grid $base$w(6) -in $base$w(7) -column 1 -row 1 -columnspan 1\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;
        # grid $base$w(3):3 3
        grid columnconfigure $base$w(3) 2 -weight 1;
        grid rowconfigure $base$w(3) 2 -weight 1;

        grid $base$w(1) -column 1 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(2) -column 0 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(10) -column 0 -row 0 -columnspan 1 -rowspan 100\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nes;
        place $base$w(11) -in $base$w(10) -x -3 -relx 0 -y 24 -rely 0\
             -anchor nw;
        grid $base$w(3) -column 0 -row 1 -columnspan 2 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nesw;
        # grid $base$w(0):2 100
        grid columnconfigure $base$w(0) 1 -weight 1;
        grid columnconfigure $base$w(0) 0 -minsize 72;
        grid rowconfigure $base$w(0) 0 -weight 1; 
        catch {rtl_customize::init $base;} 
        set base [mkProc $base rtl_customize $args]; 
        $base$w(7) update; 
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
set rat(WIDGETCLASS,rtl_customize) rtl_customize;
# ---------------------------------------------------------------------------

####VG-STOP####

option add *Rtl_customize.menutree.font {Helvetica -12 bold}
option add *Rtl_customize.menutree.help {Select a menuentry, to integrate in the current Symbolbar|Select Action}
option add *Rtl_customize.imagetree.font {Helvetica -12 bold}
option add *Rtl_customize.imagetree.help {Select an Image for displaying|Select an Image}
option add *Rtl_customize.xhandle_.background black
option add *Rtl_customize.work.addcmd.text Add
option add *Rtl_customize.work.addcmd.help {Click here to integrate the selected items into the Symbolbar|Append to Symbolbar}
option add *Rtl_customize.work.delcmd.text Delete
option add *Rtl_customize.work.delcmd.help {Click here to delete the selected Symbolbar item|Delete from Symbolbar}
option add *Rtl_customize.work.entry.help {Modify the selected Symbolbar item|Modify Item}

# ---------------------------------------------------------------------------
# Function    : rtl_customize::getSettings - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get default values from application
# ---------------------------------------------------------------------------
proc rtl_customize::getSettings {key def} {
    if {[string compare "::getSettings" [info commands "::getSettings"]] == 0} {
	set def [::getSettings $key $def];
    }
    return $def;
}
# ---------------------------------------------------------------------------
# Function    : rtl_customize::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : second constructor to rtl_customize
# ---------------------------------------------------------------------------
proc rtl_customize::init {base} {
    variable w;
    variable $base;

    update;
    set bb [grid bbox $base.work 0 0];
    set wc [lindex $bb 2];
    grid columnconfigure $base 0 -minsize $wc;

    set imagel [lsort [image names]];
    set skiplist [list\
	    tree:closedbm idir ifile _pr _networking _lucent tree:openbm];

    set skipimages [getSettings "Appearance,Images,Skip"\
		$skiplist];

    set newl [list];
    foreach img $imagel {
	if {[lsearch -exact $skipimages $img] < 0} {
	    lappend newl $img;
	}
    }

    set ${base}(index) 0;
    $base$w(2) configure -images $newl;
}

# ===========================================================================
# Public methods:
# ---------------------------------------------------------------------------
# ===========================================================================
# Generic Attributes:

create-action rtl_customize root {index 0} {select 0} \
	{activeforeground black} {activebackground lightgray}\
	{selectbackground blue3} {selectforeground white};

# ---------------------------------------------------------------------------
# Function    : rtl_customize::set-root - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : menu entrypoint (toplevel menu)
# ---------------------------------------------------------------------------
proc rtl_customize::set-root {base value} {
    variable var;
    variable w;
    
    set childlist [traverseWidgets $value];
    set newl [list [list . -image _menu]];
    foreach child $childlist {
	if {[regexp "^${value}(.*)\$" $child match pn]} {
	    lappend newl [list $pn "-image" _menu];
	    foreach it [menuitems $child $pn] {
		lappend newl $it;
	    }
	}
    }

    ::eval {$base$w(1) replace} $newl;
    return [::set-action rtl_customize $base root $value];
}

# ---------------------------------------------------------------------------
# Description : Actions
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_customize::add - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Add an Action to the given Symbolbar
# ---------------------------------------------------------------------------
proc rtl_customize::add {base} {
    variable var;
    variable $base;
    variable w;
    
    if [catch {
    set cmd [$base$w(1) get-select];
    } msg] {
	puts stderr "tree faild for get-select $cmd"
    }

    set img [$base$w(2) cget -select];

    set treename [get-root $base]$cmd;
    if {![regexp {^(.*).<(.*[^>])>$} $treename tmp m id]} {
	error "wrong pattern $treename, must be widgetname.<index>";
    }
    if {$img != {}} {
	set itementry "$m $id \{-image [string trim $img /]\}";
    } else {
	set itementry "$m $id";
    }
    set idx [incr ${base}(index)];

    set ${base}(symbolbar:$idx) $itementry;

    set label "-";
    catch {
	set label [$m entrycget $id -label]; 
    };

    $base$w(6) window create end\
	    -window [checkbutton $base$w(6).b$idx\
	    -command "$base$w(12) configure\
	    -textvariable rtl_customize::${base}(symbolbar:$idx)"\
	    -image [string trim $img /]\
	    -variable rtl_customize::${base}(select)\
	    -selectcolor [get-selectbackground $base]\
	    -activeforeground [get-activeforeground $base]\
	    -activebackground [get-activebackground $base]\
	    -padx 7 -pady 5\
	    -text $label -indicatoron 0\
	    -offvalue 0\
	    -onvalue $idx]\
	    -stretch 1;

    option add *[string trim $base$w(6).b$idx .].help\
	    "$itementry|$itementry";

    $base$w(12) configure -textvariable\
	    rtl_customize::${base}(symbolbar:$idx);

    set-select $base $idx;
}

# ---------------------------------------------------------------------------
# Function    : rtl_customize::del - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Delete the selected item from the Symbolbar
# ---------------------------------------------------------------------------
proc rtl_customize::del {base} {
    variable $base;
    variable w;

    $base$w(12) configure -textvariable\
	    rtl_customize::${base}(symbolbar:0);

    set sel [get-select $base];
    if {[catch {
	unset ${base}(symbolbar:$sel);
    } msg]} {
	puts stderr "Error : $msg";
    }

    destroy $base$w(6).b$sel;
    set-select $base 0;
}

# ---------------------------------------------------------------------------
# Function    : rtl_customize::items - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : retrieve all setted items
# ---------------------------------------------------------------------------
proc rtl_customize::items {base} {
    variable $base;
    set newl [list];
    set idx [get-index $base];
    for {set i 1} {$i <= $idx} {incr i} {
	set item symbolbar:$i;
	if {[info exists ${base}($item)]} {
	    set val [set ${base}($item)];
	    if {$val == {}} {
		continue;
	    }
	    lappend newl $val;
	}
    }
    return $newl;
}

# ---------------------------------------------------------------------------
# Function    : rtl_customize::setitems - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : initialize the Symbolbar
# ---------------------------------------------------------------------------
proc rtl_customize::setitems {base itemlist} {
    variable $base;
    variable var;
    variable w;

    foreach item [array names $base symbolbar:*] {
	unset ${base}($item);
    }

    foreach child [winfo children $base$w(6)] {
	destroy $child;
    }

    set-index $base 0;

    foreach item [lindex $itemlist 1] {
	set label "-";
	set img {};

	catch {
	    set options [lindex $item 2];
	    set id [lsearch -exact $options "-image"];

	    if {$id >= 0} {
		set img [lindex $options [incr id]];
	    }
	}
	if {$img == {}} {
	    catch {
		set label [[lindex $item 0] entrycget\
			[lindex $item 1] -label];
	    }
	}

	set idx [incr ${base}(index)];
	$base$w(6) window create end\
		-window [checkbutton $base$w(6).b$idx\
		-image $img\
		-command "$base$w(12) configure\
		-textvariable rtl_customize::${base}(symbolbar:$idx)"\
		-variable rtl_customize::${base}(select)\
		-activeforeground [get-activeforeground $base]\
		-activebackground [get-activebackground $base]\
		-selectcolor [get-selectbackground $base]\
		-padx 7 -pady 5\
		-text $label -indicatoron 0\
		-offvalue 0\
		-onvalue $idx]\
		-stretch 1;   
	    
	option add *[string trim $base$w(6).b$idx .].help\
	    "$item|$item";
	set ${base}(symbolbar:$idx) $item;
    }
}

# ---------------------------------------------------------------------------
# Function    : menuitems - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : integrate non widget object into the given widgetlist
# ---------------------------------------------------------------------------
proc menuitems {m pn} {
    set items [$m index last];
    set w_list [list];
    if {$items == {}} {
	error "empty menuentry in $m"
    }

    for {set i [$m cget -tearoff]}\
	    {$i <= $items} {incr i} {
	set typ [$m typ $i];
	if {[catch {
	    set label "";
	    switch $typ {
		"cascade" -
		"radiobutton" -
		"checkbutton" -
		"command" {
		    set label [$m entrycget $i -label];
		    set img [getSettings "Appearance,Images,Menu,$typ" _m$typ];
		}
		default {
		    set label "---";
		    set img [getSettings "Appearance,Images,Menu,$typ" _m$typ];
		}
	    }
	}]} {
	    set img _dummy;
	}
	set label [join [split $label \;] {}];
	
	# setup new item
	set entry [list $pn.<$i>\
		-typ menuitem -class $typ\
		-text \"$label\" -image $img];
	lappend w_list $entry;
    }
    return $w_list;
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 14.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------
