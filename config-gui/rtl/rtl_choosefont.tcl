#!/bin/sh 
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Runtime Library Font Chooser Dialog.
# -----------------------------------------------------------------------
# start with WISH8.0 \
	exec wish8.0 "$0" ${1+"$@"} 

# VisualGIPSY 2.0

package provide rtl_choosefont 1.2;
package require rtl;
package require rtl_gridwin;

####VG-START####
set vgfont(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::vgfont; 
	set src [file join $vgfont(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# class (namespace): rtl_chooseFont 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_chooseFont {
 
    variable var; 
    variable w; 
    array set w [list 0 {} 17 .size_gridwin.hori 18 .size_gridwin.vert 1 \
        .name_label 20 .strike_cb 19 .effect_label 2 .style_label 21 \
        .uline_cb 3 .size_label 22 .wysiwyg_label 4 .name_entry 23 \
        .wysiwg 5 .style_entry 6 .size_entry 7 .name_gridwin 8 \
        .name_gridwin.name_listbox 9 .name_gridwin.hori 10 \
        .name_gridwin.vert 11 .style_gridwin 12 \
        .style_gridwin.style_listbox 13 .style_gridwin.hori 14 \
        .style_gridwin.vert 15 .size_gridwin 16 \
        .size_gridwin.size_listbox]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  highlightbackground [list \
        [list 0 highlightbackground] [list 1 highlightbackground]\
        [list 2 highlightbackground] [list 3 highlightbackground]\
        [list 7 highlightbackground] [list 11 highlightbackground]\
        [list 15 highlightbackground] [list 5 highlightbackground]\
        [list 6 highlightbackground] [list 8 highlightbackground]\
        [list 12 highlightbackground] [list 16 highlightbackground]\
        [list 20 highlightbackground] [list 21 highlightbackground]\
        [list 4 highlightbackground]] style style height {0 height} \
        msghighlightthickness {23 highlightthickness}\
        disabledforeground [list [list 20 disabledforeground]\
        [list 21 disabledforeground]] size size highlightthickness [list \
        [list 5 highlightthickness] [list 6 highlightthickness]\
        [list 8 highlightthickness] [list 12 highlightthickness]\
        [list 16 highlightthickness] [list 20 highlightthickness]\
        [list 21 highlightthickness] [list 4 highlightthickness]] \
        original toplevel msghighlightcolor {23 highlightcolor} font [list \
        [list 4 font] [list 5 font] [list 6 font] [list 8 font]\
        [list 12 font] [list 16 font] [list 20 font] [list 21 font]] \
        msghighlightbackground {23 highlightbackground} attributes [list name\
        style size overstrike underline variable background highlightbackground\
        highlightcolor borderwidth highlightthickness selectbackground\
        selectborderwidth selectforeground foreground font relief height\
        width activeforeground activebackground disabledforeground\
        indicatoron msghighlightbackground msghighlightcolor\
	entrybackground entryforeground\
        msghighlightthickness] highlightcolor [list [list 0 highlightcolor]\
        [list 1 highlightcolor] [list 2 highlightcolor]\
        [list 3 highlightcolor] [list 7 highlightcolor]\
        [list 11 highlightcolor] [list 15 highlightcolor]\
        [list 5 highlightcolor] [list 6 highlightcolor]\
        [list 8 highlightcolor] [list 12 highlightcolor]\
        [list 16 highlightcolor] [list 20 highlightcolor]\
        [list 21 highlightcolor] [list 4 highlightcolor]] \
        selectbackground [list [list 4 selectbackground]\
        [list 5 selectbackground] [list 6 selectbackground]\
        [list 8 selectbackground] [list 12 selectbackground]\
        [list 16 selectbackground] selectcolor]\
	background [list [list 0 background]\
        [list 1 background] [list 2 background] [list 3 background]\
        [list 7 background] [list 11 background] [list 15 background]\
        [list 20 background] [list 21 background] [list 19 background]\
        [list 22 background]] width {0 width} name name\
	indicatoron [list \
        [list 20 indicatoron] [list 21 indicatoron]] foreground [list \
        [list 5 foreground] [list 6 foreground] [list 8 foreground]\
        [list 12 foreground] [list 16 foreground]\
        [list 4 foreground]] selectforeground [list [list 4 selectforeground]\
        [list 5 selectforeground] [list 6 selectforeground]\
        [list 8 selectforeground] [list 12 selectforeground]\
        [list 16 selectforeground]] activebackground [list \
        [list 20 activebackground] [list 21 activebackground]] \
        selectborderwidth [list [list 4 selectborderwidth]\
        [list 5 selectborderwidth] [list 6 selectborderwidth]\
        [list 8 selectborderwidth] [list 12 selectborderwidth]\
        [list 16 selectborderwidth]] borderwidth {0 borderwidth} \
        overstrike overstrike underline underline relief {0 relief} \
        variable variable activeforeground\
        [list {20 activeforeground} {21 activeforeground}]\
	entrybackground [list {4 background} {5 background}\
	{6 background} {8 background} {12 background} {16 background}]\
	entryforeground [list {4 foreground} {5 foreground}\
	{6 foreground} {8 foreground} {12 foreground} {16 foreground}]\
	]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_chooseFont - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -name
#             : -style
#             : -size
#             : -overstrike
#             : -underline
#             : -background
#             : -highlightbackground
#             : -highlightcolor
#             : -borderwidth
#             : -highlightthickness
#             : -selectbackground
#             : -selectborderwidth
#             : -selectforeground
#             : -foreground
#             : -font
#             : -relief
#             : -height
#             : -width
#             : -activeforeground
#             : -activebackground
#             : -disabledforeground
#             : -indicatoron
#             : -msghighlightbackground
#             : -msghighlightcolor
#             : -msghighlightthickness 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_chooseFont 
# --------------------------------------------------------------------------- 
proc rtl_chooseFont {{base .toplevel1} args} {
 
    variable rtl_chooseFont::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_chooseFont $base$w(0) $args] $base -class Rtl_chooseFont;

	bindtags $base [list $base Rtl_chooseFont\
		[winfo toplevel $base] all];

        # Childs to $base$w(0), toplevel1 of class: 	Rtl_chooseFont
        label $base$w(1);
        label $base$w(2);
        label $base$w(3);
        entry $base$w(4)\
            -textvariable "::rtl_chooseFont::$base$w(0)(name)"\
            -width "30";
        entry $base$w(5)\
            -textvariable "::rtl_chooseFont::$base$w(0)(style)"\
            -width "15";
        entry $base$w(6)\
            -textvariable "::rtl_chooseFont::$base$w(0)(size)"\
            -width "6";
        rtl_gridwin $base$w(7) -widget "$base$w(8)" -auto "1" -fill "both"\
            -sides "bottom right" -height "160";
        # Managed children to $base$w(7):

        listbox $base$w(8) -exportselection "0" -width "0"\
            -xscrollcommand "rslider $base$w(8) $base$w(9) x 1; $base$w(9) set"\
            -yscrollcommand "rslider $base$w(8) $base$w(10) y 1; $base$w(10) set";
        bind $base$w(8) <ButtonRelease-1> \
             "$base$w(0) setItem name %W;";
        rtl_gridwin $base$w(11) -widget "$base$w(12)" -auto "1" -fill "both"\
            -sides "bottom right" -height "160";
        # Managed children to $base$w(11):

        listbox $base$w(12) -exportselection "0" -width "0"\
            -xscrollcommand "rslider $base$w(12) $base$w(13) x 1; $base$w(13) set"\
            -yscrollcommand "rslider $base$w(12) $base$w(14) y 1; $base$w(14) set";
        bind $base$w(12) <ButtonRelease-1> \
             "$base$w(0) setItem style %W;";
        rtl_gridwin $base$w(15) -widget "$base$w(16)" -auto "y" -fill "both"\
            -sides "right" -height "160";
        # Managed children to $base$w(15):

        listbox $base$w(16) -exportselection "0" -width "0"\
            -xscrollcommand "rslider $base$w(16) $base$w(17) x y; $base$w(17) set"\
            -yscrollcommand "rslider $base$w(16) $base$w(18) y y; $base$w(18) set";
        bind $base$w(16) <ButtonRelease-1> \
             "$base$w(0) setItem size %W;";
        label $base$w(19);
        checkbutton $base$w(20) -anchor "w" -command "$base$w(0) validate;"\
            -justify "left" -offvalue ""\
            -onvalue "overstrike"\
            -variable "::rtl_chooseFont::$base$w(0)(overstrike)";
        checkbutton $base$w(21) -anchor "w" -command "$base$w(0) validate;"\
            -justify "left" -offvalue ""\
            -onvalue "underline"\
            -variable "::rtl_chooseFont::$base$w(0)(underline)";
        label $base$w(22);
        message $base$w(23) -aspect "800" -borderwidth "0"\
            -highlightbackground "Black"\
            -highlightthickness "1" -padx "4" -pady "2"\
            -textvariable "::rtl_chooseFont::$base$w(0)(name)";

        grid $base$w(1) -column 0 -row 0 -padx 3 -sticky nsw;
        grid $base$w(2) -column 3 -row 0 -padx 3 -sticky nsw;
        grid $base$w(3) -column 5 -row 0 -padx 3 -sticky nsw;
        grid $base$w(4) -column 0 -row 1 -columnspan 2 -padx 3 -sticky nesw;
        grid $base$w(5) -column 3 -row 1 -padx 3 -sticky nesw;
        grid $base$w(6) -column 5 -row 1 -padx 3 -sticky nesw;
        grid $base$w(7) -column 0 -row 2 -columnspan 2 -padx 3 -sticky nesw;
        grid $base$w(11) -column 3 -row 2 -padx 3 -sticky nesw;
        grid $base$w(15) -column 5 -row 2 -padx 3 -sticky nesw;
        grid $base$w(19) -column 0 -row 4 -padx 3 -pady 3 -sticky nsw;
        grid $base$w(20) -column 0 -row 5 -padx 3 -pady 3 -sticky nesw;
        grid $base$w(21) -column 0 -row 6 -padx 3 -pady 3 -sticky nesw;
        grid $base$w(22) -column 1 -columnspan 3 -row 4 -padx 3 -pady 3 -sticky nsw;
        grid $base$w(23) -column 1 -row 5 -columnspan 5 -rowspan 3\
             -padx 3 -pady 3 -sticky nesw;
        grid propagate $base$w(23) 0;
        # grid $base$w(0):8 11
        foreach {id opt param} [list  1 weight 1 2 minsize 6 3 minsize 6 4\
            minsize 6] {
            grid columnconfigure $base$w(0) $id -$opt $param;
        }
        foreach {id opt param} [list  2 weight 2 3 minsize 6 7 weight 1 7\
            minsize 42] {
            grid rowconfigure $base$w(0) $id -$opt $param;
        } 
        catch {rtl_chooseFont::init $base;} 
        set base [mkProc $base rtl_chooseFont $args]; 
        $base$w(7) update;
        $base$w(11) update;
        $base$w(15) update; 
    }
 
    return $base; 
}

# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet! 
# ---------------------------------------------------------------------------
option add *Rtl_chooseFont.name_label.text {Font};
option add *Rtl_chooseFont.style_label.text {Style};
option add *Rtl_chooseFont.size_label.text {Size};
option add *Rtl_chooseFont.effect_label.text {Effects};
option add *Rtl_chooseFont.strike_cb.text {Strikeout};
option add *Rtl_chooseFont.uline_cb.text {Underline};
option add *Rtl_chooseFont.wysiwyg_label.text {Sample};
option add *Rtl_chooseFont.wysiwg.background white;
option add *Rtl_chooseFont.wysiwg.foreground black;

# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 2.0 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,rtl_chooseFont) rtl_choosefont;
# ---------------------------------------------------------------------------

####VG-STOP####

# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for widgets of class chooseFont
# ---------------------------------------------------------------------------
proc rtl_chooseFont::init {base} {
    variable var;

    foreach {type attrs} [list\
	    style [list normal italic bold {bold italic}]\
	    size [list 8 9 10 11 12 14 16 18 20 22 24 26 28 36 48 72 144]\
	    name [lsort [font families]]] {
	set lb [find-widget ${type}_listbox $base];
	foreach opt $attrs {
	    $lb insert end $opt;
	}
    }
    set ::rtl_chooseFont::${base}(name) helvetica;
    set ::rtl_chooseFont::${base}(size) 12;
    set ::rtl_chooseFont::${base}(style) normal;
}

# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::setItem - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_chooseFont::setItem {base type lb} {
    set id [$lb curselection];
    set item [$lb get $id];
    set-action rtl_chooseFont $base $type $item;
    set-modified $base 1;

    validate $base;
}

# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::validate - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_chooseFont::validate {base} {
    set v [get-variable $base];
    if {$v != {}} {
	set $v [getfont $base];
    }
    update $base;
}


# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::getfont - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_chooseFont::getfont {base} {
    return [string trim\
	"\{[get-name $base]\} [get-size $base]\
	[get-style $base] [get-overstrike $base]\
	[get-underline $base]" \ ];
}

# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::update - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_chooseFont::update {base} {
    set msg [find-widget wysiwg $base];
    $msg configure -font [getfont $base];
}

# ---------------------------------------------------------------------------
# Additional used options for the font chooser.
# ---------------------------------------------------------------------------
create-action rtl_chooseFont {name helvetica} {style normal} {size 12}\
	{overstrike 0} {underline 0}\
	modified variable selectcolor;




foreach opt [list name style size] {
    uplevel "#0" "proc ::rtl_chooseFont::set-$opt \{base value\} \{\n\
	    set lb \[find-widget ${opt}_listbox \$base\];\n\
	    #set sel \[\$lb selection get\]\n\
	    set sel \[\$lb curselection\];\n\
	    if \{\$sel != \"\"\} \{\n\
	    catch \{\$lb selection clear \[\$lb curselection\];\}\n\
	    \n\}\n\
	    catch \{set lb_l \[\$lb get 0 end\];\}\n\
	    catch \{\n\
	    set id \[lsearch \$lb_l \[string trim\
	    \$value \\\{\\\}\]\];\n\
	    \$lb selection set \$id;\n\
	    \n\}\n\
	    catch \{validate \$base;\}\n\
	    return \[::set-action rtl_chooseFont \$base $opt \$value\];\n\
	    \n\}\n";
 
}

foreach opt [list underline overstrike] {
    uplevel "#0" "proc ::rtl_chooseFont::set-$opt \{base value\} \{\n\
	    catch \{validate \$base;\}\n\
	    return \[::set-action rtl_chooseFont \$base $opt \$value\];\n\
	    \n\}\n";
 
}

# ---------------------------------------------------------------------------
# Function    : rtl_chooseFont::selectcolor - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Set selectbackground of checkbutton items under UNIX
#               and skip this under Windows
# ---------------------------------------------------------------------------
proc rtl_chooseFont::selectcolor {base value} {
    if {[string compare "Windows" $::tcl_platform(platform)] !=} {
	variable w;
	foreach id [list 20 21] {
	    $base$w($id) configure -selectcolor $value;
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

