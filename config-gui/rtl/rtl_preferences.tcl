# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary VG Preferences Settings
# -----------------------------------------------------------------------
package provide rtl_preferences 1.2;
package require rtl;
package require rtl_tree;
package require rtl_gridwin;
package require rtl_scrollwin;

####VG-START####
set rtl_preferences(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_preferences; 
	set src [file join $rtl_preferences(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_preferences.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_preferences 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_preferences {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 17 .gridwin.vert 1 .tree 2 .gridwin 3 \
        .gridwin.scrollwin 4 .gridwin.scrollwin.select 5 \
        .gridwin.scrollwin.select.pref 6 .gridwin.scrollwin.select.l1 7 \
        .gridwin.scrollwin.select.l2 8 .gridwin.scrollwin.select.e1 10 \
        .gridwin.scrollwin.select.apply 9 .gridwin.scrollwin.select.e2 \
        11 .nope 12 .handle 13 .title 14 .title.label 15 .title.msg 16 \
        .gridwin.hori]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  nopeborderwidth {11 borderwidth} troughcolor [list \
        [list 2 troughcolor]] treewidth {1 width} highlightbackground [list \
        [list 0 highlightbackground]] height {0 height} treeconnect [list \
        [list 1 connect]] titlebackground [list [list 13 background]\
        [list 14 background] [list 15 background]] .rtl_preferences,newvalue\
        {} .rtl_preferences,msg [list Settings profile dialog] \
        highlightthickness {0 highlightthickness} .rtl_preferences,creator \
        frame titleforeground [list {15 foreground} {14 foreground}] \
        original frame nopesize [list {11 height} {11 width}] \
        treeborderwidth {1 borderwidth} attributes [list variable root\
        titlebackground titleforeground titlerelief titleborderwidth\
        background borderwidth relief highlightbackground highlightcolor\
        highlightthickness width height treefont treerelief\
        treeborderwidth treewidth treeconnect activebackground\
        troughcolor nopebackground nopeborderwidth nopesize fill] \
        highlightcolor {0 highlightcolor} treerelief {1 relief} fill [list \
        [list 3 fill]] titleborderwidth {13 borderwidth} background [list \
        [list 0 background] [list 2 background]] width {0 width} \
        titlerelief {13 relief} nopebackground {11 background} root\
        root activebackground {2 activebackground} .rtl_preferences,head \
        Rtl_Preferences variable variable borderwidth {0 borderwidth} \
        relief {0 relief} treefont {1 font}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_preferences - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -variable
#             : -root
#             : -titlebackground
#             : -titleforeground
#             : -titlerelief
#             : -titleborderwidth
#             : -background
#             : -borderwidth
#             : -relief
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -width
#             : -height
#             : -treefont
#             : -treerelief
#             : -treeborderwidth
#             : -treewidth
#             : -treeconnect
#             : -activebackground
#             : -troughcolor
#             : -nopebackground
#             : -nopeborderwidth
#             : -nopesize
#             : -fill 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_preferences 
# --------------------------------------------------------------------------- 
proc rtl_preferences {{base .rtl_preferences} args} {
 
    variable rtl_preferences::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_preferences $base$w(0) $args] $base\
		-class Rtl_preferences -relief "raised";

        # Childs to $base$w(0), rtl_preferences of class: 	Rtl_Preferences
        rtl_tree $base$w(1) -char "," -indentx "30"\
		-indenty "20" -root "0" -connect 1\
		-width "150" -height "200" \
		-borderwidth 1 -relief "sunken"\
		-fiximagewidth 0 -format "%s" -itemselect "1"\
		-visible "label text title class geometry manager"\
		-selectfcn rtl_preferences::select -drag "0";

        rtl_gridwin $base$w(2) -widget "$base$w(3)" -auto "1"\
		-fill "both" \
		-sides "bottom right" -width "160"\
		-height "160";
        # Managed children to $base$w(2):

        rtl_scrollwin $base$w(3) -widget "$base$w(4)" -fill "x"\
		-xscrollcommand\
		"rslider $base$w(3) $base$w(16) x 1; $base$w(16) set" \
		-yscrollcommand\
		"rslider $base$w(3) $base$w(17) y 1; $base$w(17) set" \
		-creator frame;
        # Managed children to $base$w(3):

        frame $base$w(4);

        # Childs to $base$w(4), select of class: 	Frame
        frame $base$w(5) -borderwidth "1" -relief "raised";
        label $base$w(6) -anchor "e";
        label $base$w(7) -anchor "w";
        entry $base$w(8) -textvariable\
		::rtl_preferences::${base}(newname)\
		-width "5";
        entry $base$w(9) -textvariable\
		::rtl_preferences::${base}(newvalue);

        button $base$w(10) -command "rtl_preferences::apply $base$w(0)";

        pack $base$w(5) -anchor center -expand 1 -fill both -ipadx 12\
             -ipady 12 -padx 0 -pady 0 -side bottom;
        grid $base$w(6) -in $base$w(5) -column 0 -row 0 -columnspan 1\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 3 -pady 1 -sticky nesw;
        grid $base$w(7) -in $base$w(5) -column 1 -row 0 -columnspan 1\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 3 -pady 1 -sticky nesw;
        grid $base$w(8) -in $base$w(5) -column 0 -row 1 -columnspan 1\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 5 -pady 1 -sticky nesw;
        grid $base$w(9) -in $base$w(5) -column 1 -row 1 -columnspan 1\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 3 -pady 1 -sticky nesw;
        grid $base$w(10) -in $base$w(5) -column 2 -row 1 -columnspan 2\
             -rowspan 1 -ipadx 0 -ipady 0 -padx 3 -pady 1 -sticky nesw;
        frame $base$w(12) -cursor "sb_h_double_arrow" -relief "raised";
        foreach pair [list \
            [list <B1-Motion> "resize-drag $base$w(0) %W %x %y"]\
            [list <Button-1> "resize-start $base$w(0) %W %x %y"]\
            [list <ButtonRelease-1> "resize-end $base$w(0) %W %x %y"]] {
             bind $base$w(12) [lindex $pair 0] [lindex $pair 1]; 
        }

        frame $base$w(11) -borderwidth "1" -cursor "sb_h_double_arrow" \
            -height 6 -relief "raised" -width "6";
        foreach pair [list \
            [list <B1-Motion> "resize-drag $base$w(0) $base$w(12) %x %y"]\
            [list <Button-1> "resize-start $base$w(0) $base$w(12) %x %y"]\
            [list <ButtonRelease-1> "resize-end $base$w(0) $base$w(12) %x %y"]] {
             bind $base$w(11) [lindex $pair 0] [lindex $pair 1]; 
        }

        frame $base$w(13) -borderwidth "1" -height 50 -relief "raised";

        # Childs to $base$w(13), title of class: 	Frame
        label $base$w(14) -anchor "e";
        message $base$w(15) -anchor "w" -aspect "600" -padx "4" -pady "2";


        pack $base$w(14) -anchor center -expand 0 -fill y -ipadx 0\
             -ipady 0 -padx 3 -pady 0 -side left;
        pack $base$w(15) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 3 -pady 0 -side right;

        grid $base$w(1) -column 0 -row 0 -columnspan 1 -rowspan 2\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(2) -column 1 -row 1 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;
        grid $base$w(12) -column 0 -row 0 -columnspan 1 -rowspan 100\
             -ipadx 0 -ipady 0 -padx 0 -pady 0 -sticky nes;
        place $base$w(11) -in $base$w(12) -x -3 -relx 0 -y 24 -rely 0\
             -anchor nw;
        grid $base$w(13) -column 1 -row 0 -columnspan 1 -rowspan 1\
             -ipadx 0 -ipady 0 -padx 5 -pady 5 -sticky nesw;

	pack propagate $base$w(13) 0;

        # grid $base$w(0):2 100
        grid columnconfigure $base$w(0) 1 -weight 1;
        grid columnconfigure $base$w(0) 1 -minsize 300;
        grid rowconfigure $base$w(0) 1 -weight 1; 
        catch {rtl_preferences::init $base;} 
        set base [mkProc $base rtl_preferences $args]; 
        $base$w(2) update; 
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------

option add *Rtl_preferences.handle.background black
option add *Rtl_preferences.title.background black
option add *Rtl_preferences.title.label.background black
option add *Rtl_preferences.title.label.font {Helvetica -24 bold}
option add *Rtl_preferences.title.label.foreground white
option add *Rtl_preferences.title.label.text Preferences
option add *Rtl_preferences.title.label.underline -1
option add *Rtl_preferences.title.msg.background black
option add *Rtl_preferences.title.msg.font {Helvetica -10 italic}
option add *Rtl_preferences.title.msg.foreground white
option add *Rtl_preferences.title.msg.text {Settings profile dialog}
option add *Rtl_preferences.gridwin.scrollwin.select.l2.text Value
option add *Rtl_preferences.gridwin.scrollwin.select.apply.text Apply
option add *Rtl_preferences.gridwin.scrollwin.select.l1.text Name


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set rat(WIDGETCLASS,rtl_preferences) rtl_preferences;
# ---------------------------------------------------------------------------

####VG-STOP####


# ===========================================================================
# Default construction methods:
# ---------------------------------------------------------------------------
# Function    : rtl_preferences::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor
# ---------------------------------------------------------------------------
proc rtl_preferences::init {base} {
    variable w;
    variable var;
    variable $base;

    $base$w(2) configure -widget $base$w(3);
    $base$w(3) configure -widget $base$w(4) -fill x;
    $base$w(14) configure -textvariable ::rtl_preferences::${base}(head);
    $base$w(15) configure -textvariable ::rtl_preferences::${base}(msg);

    grid columnconfigure $base$w(5) 1 -weight 1;
    grid columnconfigure $base$w(5) 0 -minsize 100;
}

# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_preferences {head "Preferences"}\
	{msg "Preferences Profile Dialog"} variable root\
	selectlast newname newvalue;

# ===========================================================================
# Public Methods:
# ---------------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_preferences::extend - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Show or Hide the profile editor entries
# ---------------------------------------------------------------------------
proc rtl_preferences::extend {base value} {
    variable w;

    set value [string tolower $value];
    switch $value {
	"true" -
	"1" {
	    foreach id [list 8 9 10] {
		grid $base$w($id);
	    }
	}
	"false" -
	"0" {
	    foreach id [list 8 9 10] {
		grid remove $base$w($id);
	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_preferences::update - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Refresh current preferences dialog
# ---------------------------------------------------------------------------
proc rtl_preferences::update {base} {
    variable var;
    variable w;
    
    set root [get-root $base];
    set variable [get-variable $base];

    variable $variable;

    set v_list {};
    set reg "^${root},(.*),(.*)\$"
    foreach elem [array names $variable $root,*] {
	if {[regexp $reg $elem tmp val rest]} {
	    set pval {};
 	    foreach pel [split $val ,] {
 		set pval ${pval},$pel;
		if {[lsearch -exact $v_list $pval] < 0} {
		    lappend v_list $pval;
		}
 	    }
	}
    }
    ::eval "$base$w(1) replace [lsort $v_list]";
}

# ---------------------------------------------------------------------------
# Function    : rtl_preferences::mkItem - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Construct the edit item with information from the 
#               tk database; there are some new widget types (not
#               classes) know by this method.
# ---------------------------------------------------------------------------
proc rtl_preferences::mkItem {base select current name variable id} {
    variable $variable;

    set descr [string trim [string range $name\
	    [string length $current] end] ,];
    grid [label $select.label$id -anchor e -width 5 -text $descr]\
	    -column 0 -row $id -sticky news -padx 3 -pady 1;

    ## now try to look at the database for a
    ## entry which will match for that item.

    set res [string tolower ${name}];
    set rlist [split $res ,];

    foreach elem [list type options help grid] {
	set $elem {};
	set res2 $res;
	set rlist2 $rlist;

	while {[string compare [set $elem [option get $base ${res2}:${elem} ""]] {}] == 0} {
	    set lg [llength $rlist2];
	    incr lg -1;
	    if {$lg <= 0} break;
	    set rlist2 [lreplace $rlist2 $lg $lg];
	    set res2 [join $rlist2 ,];
	}
    }

    if {[string compare $type {}] == 0} {
	entry $select.entry$id -textvariable ${variable}($name);
    } else {
	switch $type {
	    "tk_optionMenu" {
		uplevel #0 "$type $select.entry$id $options";
	    }
	    "vg_list" {
		if {$grid == {}} {
		    set grid {-padx 3 -pady 3};
		}

		set ent $select.entry$id;
		frame $ent;
		set i 0;
		grid columnconfigure $ent 0 -weight 1;
		#grid columnconfigure $ent 1 -weight 1;

		foreach item [set ${variable}($name)] {
		    grid [entry $ent.e$i -bd 1 -relief sunken \
			    -highlightthickness 0\
			    -textvariable\
			    ::rtl_preferences::tmp($id,$i)] -column 0 -row $i\
			    -columnspan 1 -sticky news -pady 0;
		    bind $ent.e$i <Return> "$ent.apply invoke;break";
		    set ::rtl_preferences::tmp($id,$i) $item;
		    incr i;
		}

		grid [button $ent.apply -text [opget .string apply\
			"Apply"]\
			-command "set  ${variable}($name) {};\
			\nfor \{set i 0\} \{\$i < $i\} \{incr\
			i\} \{\n\
			set val \[set ::rtl_preferences::tmp($id,\$i)\];\n\
			if \{\$val != {}\} \{\n\
			lappend ${variable}($name) \$val\n\}\n\};\n\
			eval \[$base get-selectlast\];"]\
			-padx 5 -pady 1 -column 1 -row 0 -rowspan 2\
			-sticky new;

		incr i 2;
		set ::rtl_preferences::tmp($id,new) {};
		grid [entry $ent.enew -textvariable\
			::rtl_preferences::tmp($id,new)] -column 0 -row $i\
			-columnspan 1 -pady 1 -sticky sew;
		bind $ent.enew <Return> "$ent.new invoke;break";
		grid [button $ent.new\
			-text [opget .string new "New"]\
			-command "foreach item\
			\[set ::rtl_preferences::tmp($id,new)\] \{\n\
			lappend ${variable}($name) \$item;\n\};\n\
			eval \[$base get-selectlast\];"]\
			-padx 5 -pady 1\
			-column 1 -row $i -rowspan 2 -sticky new;
	    }
	    "label" {
		::eval "$type $select.entry$id\
			-textvariable ${variable}($name) -width 1 $options";
	    }
	    default {
		::eval "$type $select.entry$id\
			-variable ${variable}($name) $options";
	    }
	}
    }

    ::eval "grid $select.entry$id -column 1 -row $id -sticky news\
	    -padx 3 -pady 1 $grid";

    set ressel [string range $select.entry$id 1 end];
    option add *$ressel.help $help;
}

# ---------------------------------------------------------------------------
# Function    : rtl_preferences::select - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_preferences::select {tree node typ} {
    variable w;
    
    set base [winfo parent $tree];
    set root [get-root $base];
    set variable [get-variable $base];
    variable $variable;

    set i 2;
    
    set-selectlast $base [list rtl_preferences::select $tree $node $typ];

    foreach child [winfo children $base$w(5)] {
	destroy $child;
    }
    
    set current ${root}${node};
    set dscr [string trim $node];
    regexp {(.*),(.*)$} $dscr match tmp dscr;


#    puts stderr "write to head : $dscr";

    set-head $base $dscr;
    set-msg $base [option get $base [string tolower ${current}:info] ""];

    foreach elem [array names $variable ${current}*] {

	if {[regexp "${current}(.*\[^,\]),(.*)" $elem match tmp tmp2]} {
	   continue;
	}

	mkItem $base $base$w(5) $current $elem $variable [incr i];
    }
    
    xsizer $base$w(5).nope $base$w(5).handle $base$w(5) 0 nse 100;
    $base$w(3) yview moveto 0;
}

# ---------------------------------------------------------------------------
# Function    : rtl_preferences::apply - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Internal apply changes to current dataset
# ---------------------------------------------------------------------------
proc rtl_preferences::apply {base} {
    variable w;

    set root [get-root $base];
    set variable [get-variable $base];
    variable $variable;

    set selected [$base$w(1) cget -select];

    set ${variable}([get-root $base]$selected,[get-newname $base])\
	    [get-newvalue $base];

    ::eval [$base get-selectlast];
    
    update $base;
    $base$w(1) configure -select $selected;
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 15.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------
