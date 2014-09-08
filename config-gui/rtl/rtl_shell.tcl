# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: VisualGIPSY Tclshell
# -----------------------------------------------------------------------
package provide rtl_shell 1.2;
package require rtl;
package require rtl_gridwin;

####VG-START####
set rtl_shell(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_shell; 
	set src [file join $rtl_shell(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_shell.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_shell 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_shell {
 
    variable var; 
    variable w; 
    array set w [list 4 .cmdline.label 0 {} 1 .gridwin 5 .cmdline.cmd 6 \
        .gridwin.hori 2 .gridwin.text 7 .gridwin.vert 3 .cmdline]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  highlightbackground {0 highlightbackground} \
        .shell,start 1.6 maxbuffer maxbuffer state {2 state} \
        insertontime {2 insertontime} height {0 height} highlightthickness [list \
        [list 0 highlightthickness]] insertwidth [list \
        [list 2 insertwidth]] original frame switchcmd switchcmd\
        spacing1 {2 spacing1} font {2 font} attributes [list interp prompt\
        switchcmd maxbuffer font background borderwidth height width\
        relief highlightthickness highlightcolor highlightbackground\
        sides exportselection foreground insertofftime insertontime\
        insertwidth padx pady selectbackground selectborderwidth\
        selectforeground spacing1 spacing2 spacing3 tabs state wrap\
        insertbackground insertborderwidth localprompt] highlightcolor [list \
        [list 0 highlightcolor]] spacing2 {2 spacing2} \
        .shell,history,index 0 background [list [list 2 background]]\
	width {0 width} \
	localprompt localprompt\
	\
	sides [list \
        [list 1 sides]] exportselection {2 exportselection} \
        selectbackground {2 selectbackground} spacing3 {2 spacing3} \
        .shell,hist_index 0 prompt prompt interp interp tabs {2 tabs} \
        wrap {2 wrap} foreground {2 foreground} selectforeground [list \
        [list 2 selectforeground]] insertborderwidth [list \
        [list 2 insertborderwidth]] padx {2 padx} \
	insertbackground {2 insertbackground}\
        borderwidth {0 borderwidth} pady {2 pady} \
        selectborderwidth {2 selectborderwidth} relief {0 relief} \
        insertofftime {2 insertofftime}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_shell - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -interp
#             : -prompt
#             : -switchcmd
#             : -maxbuffer
#             : -font
#             : -background
#             : -borderwidth
#             : -height
#             : -width
#             : -relief
#             : -highlightthickness
#             : -highlightcolor
#             : -highlightbackground
#             : -sides
#             : -exportselection
#             : -foreground
#             : -insertofftime
#             : -insertontime
#             : -insertwidth
#             : -padx
#             : -pady
#             : -selectbackground
#             : -selectborderwidth
#             : -selectforeground
#             : -spacing1
#             : -spacing2
#             : -spacing3
#             : -tabs
#             : -state
#             : -wrap
#             : -insertbackground
#             : -insertborderwidth 
#             : -localprompt
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_shell 
# --------------------------------------------------------------------------- 
proc rtl_shell {{base .shell} args} {
 
    variable rtl_shell::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_shell $base$w(0) $args] $base\
		-class Rtl_shell -width 360 -height 60;

        # Childs to $base$w(0), shell of class: 	Rtl_Shell
        rtl_gridwin $base$w(1) -widget "$base$w(2)" -auto "1" -fill "both" \
            -sides "bottom right" -width "42" -height "42";
        # Managed children to $base$w(1):

        text $base$w(2) -height "5" -width "1" -xscrollcommand\
            "rslider $base$w(2) $base$w(6) x 1; $base$w(6) set" \
            -yscrollcommand\
            "rslider $base$w(2) $base$w(7) y 1; $base$w(7) set";

        foreach pair [list \
            [list <<Clear>> "catch \{%W delete sel.first sel.last\};break;"]\
            [list <<Copy>> "tk_textCopy %W;break"]\
            [list <<Paste>> "catch \{rtl_shell::paste %W;\}; break;"]\
            [list <Button-2> "break"]\
            [list <ButtonRelease-2> "catch \{rtl_shell::paste %W;\}; break;"]\
            [list <Key-BackSpace> "rtl_shell::backspace $base$w(0) -backspace;break"]\
            [list <Key-Delete> "rtl_shell::backspace $base$w(0) -delete;break"]\
            [list <Key-Down> "rtl_shell::down $base; break"]\
            [list <Key-Left> "tkTextSetCursor %W insert-1c;break;"]\
            [list <Key-Return> "rtl_shell::returnCmd $base; break"]\
            [list <Key-Right> "tkTextSetCursor %W insert+1c;break;"]\
            [list <Key-Up> "rtl_shell::up $base; break"]\
            [list <Key> "rtl_shell::insertKey $base$w(0) %A;break"]] {
             bind $base$w(2) [lindex $pair 0] [lindex $pair 1]; 
        }

        frame $base$w(3);

        # Childs to $base$w(3), cmdline of class: 	Frame
        label $base$w(4);
        entry $base$w(5) -width "1";

	# Disable geometry propagation for the shell root
	# widget ...

	pack propagate $base 0;

        pack $base$w(4) -anchor center -expand 0 -fill y -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side left;
        pack $base$w(5) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side right;

        pack $base$w(1) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side top; 
        catch {rtl_shell::init $base;} 
        set base [mkProc $base rtl_shell $args]; 
        $base$w(1) update; 
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
set rat(WIDGETCLASS,rtl_shell) rtl_shell;
# ---------------------------------------------------------------------------
#option add *Rtl_Shell.background pink;
####VG-STOP####

# ---------------------------------------------------------------------------
# Function    : rtl_shell::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for widgets of class shell
# ---------------------------------------------------------------------------
proc rtl_shell::init {base} {
    variable var;
    variable $base;
    variable w;

    set ${base}(buffer) [list];
    set ${base}(hist_index) 0;

    set vg20handle .[string trim [winfo parent $base].handle_ .];
    if {[winfo exists $vg20handle]} {
	raise $vg20handle;
    }

    set ${base}(history) {};
    set ${base}(history,index) 0;

    after idle [list rtl_shell::initialPrompt $base];

}

proc rtl_shell::initialPrompt {base} {
    variable $base;
    variable w;

    $base$w(2) insert end "[get-prompt $base]: ";
    set ${base}(start) [$base$w(2) index end-1c];
}

# shell routines ...
# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_shell interp {switchcmd {+-}} {prompt "VG2.0"}\
	{maxbuffer 20} {localprompt "Current"} localargs;

# ---------------------------------------------------------------------------
# Function    : rtl_shell::insertKey - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Insert a new keystroke to the shell
# ---------------------------------------------------------------------------
proc rtl_shell::insertKey {base key} {
    variable var;
    variable $base;
    variable w;

    if {![info exist ${base}(start)]} {
	$base$w(2) mark set insert end-1c;
	set ${base}(start) [$base$w(2) index insert];
	set ${base}(cmd) "";
    }
    
    set start [set ${base}(start)];
    set start [set ${base}(start)];
    set start [split $start .];
    set sl [lindex $start 0];
    set sc [lindex $start 1];
    set insert [split [$base$w(2) index insert] .];
    set il [lindex $insert 0];
    set ic [lindex $insert 1];
    if {$sc <= $ic && $sl == $il || $sl < $il} {
	set ins $insert;
    } else {
	$base$w(2) mark set insert end-1c;
    }
    tkTextInsert $base$w(2) $key;
    $base$w(2) yview end;
    append ${base}(cmd) $key;
    #$base$w(2) insert $ins $key;
}


# ---------------------------------------------------------------------------
# Function    : rtl_shell::openCmd - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Start a new command
# ---------------------------------------------------------------------------
proc rtl_shell::openCmd {base r {tag display}} {
    variable var;
    variable $base;
    variable w;

    $base$w(2) insert end \n;
    set start [$base$w(2) index insert];
    $base$w(2) insert end $r;
    set stop [$base$w(2) index insert];

    set prompt [get-prompt $base];

    if {[info exists ${base}(useinterp)]} {
	set prompt [get-localprompt $base];
    }
    $base$w(2) insert end "\n${prompt}: ";
    $base$w(2) tag add valid [set ${base}(start)] [set ${base}(stop)];
    $base$w(2) tag add $tag $start $stop;

    set ${base}(start) [$base$w(2) index "end-1c"];
    $base$w(2) mark set insert end-1c;

    $base$w(2) tag configure error -foreground red -background white;
    $base$w(2) tag configure display -foreground "maroon";
    $base$w(2) tag configure valid -foreground "green4";
    unset ${base}(stop);
    $base$w(2) yview end;
}


# ---------------------------------------------------------------------------
# Function    : rtl_shell::toggle - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Allow to switch to the local prompt.
# ---------------------------------------------------------------------------
proc rtl_shell::toggle {base args} {
    variable $base;
    variable w;

    # If args is given, ensure that the Local prompt is used instead
    # of the global prompt. Args will be delievered to the registered
    # procedure and allows to fill this procedure with more details.

    if {[llength $args] > 0} {
	set-localargs $base $args;
	set ${base}(useinterp) 1;
	$base$w(2) insert end \n[get-localprompt $base]:;
    } else {
	if {[info exists ${base}(useinterp)]} {
	    unset ${base}(useinterp);
	    $base$w(2) insert end \n[get-prompt ${base}]:;
	} else {
	    set-localargs $base {};
	    set ${base}(useinterp) 1;
	    $base$w(2) insert end \n[get-localprompt $base]:;
	}
    }

    set ${base}(start) [$base$w(2) index insert];
    set ${base}(cmd) "";
    $base$w(2) yview end;

    return;
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::returnCmd - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Finish command and try to evalute
# ---------------------------------------------------------------------------
proc rtl_shell::returnCmd {base} {
    variable var;
    variable $base;
    variable w;
    
    set stop [$base$w(2) index "end-1c"];
    set ${base}(cmd) [$base$w(2) get [set ${base}(start)] $stop];

    set swCmd [get-switchcmd $base];

    if {[string first $swCmd [set ${base}(cmd)]] == 0} {
	set swag [lreplace [split [set ${base}(cmd)] \ ] 0 0];
	if {[llength $swag] == 0} {
	    toggle $base;
	} else {
	    set cmd  [set ${base}(cmd)];
	    add $base $cmd;
	    set qq [::eval [linsert $swag 0 toggle $base]];
	}
	return;
    }

    if {[regexp {^!([0-9]*)} [set ${base}(cmd)] match nb]} {
	set ${base}(cmd) [history event $nb];
    }

    if {[info complete [set ${base}(cmd)]\n]} {
	set ${base}(stop) $stop;

	if {[catch {
	    if {[info exists ${base}(useinterp)]} {
		set evl "[set ${base}(cmd)]";
		set largs [get-localargs $base];
		if {[llength $largs] > 0} {

		    # Use local arguments for the local prompt.
		    set r [::eval [linsert $largs 0 [get-interp $base] $evl]];

		} else {
		    set r [[get-interp $base] $evl];
		}
	    } else {
		set r [uplevel #0 [set ${base}(cmd)]];
	    }
	} result]} {
	    set r $result;
	    set excp 1;
	}
 
	set tag error;

	if {[string compare "" [set ${base}(cmd)]] == 0} return;

	add $base [set ${base}(cmd)];

	if {![info exists excp]} {
	    set tag display;
	}

	set ${base}(cmd) "";
	openCmd $base $r $tag;
	set ${base}(hist_index) 0;
    } else {
	$base$w(2) insert end \n;
    }
    $base$w(2) yview [set ${base}(start)];
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::paste - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Use the clipboard
# ---------------------------------------------------------------------------
proc rtl_shell::paste {w} {
    variable ::tcl_platform;

    if {[string compare "Text" [winfo class $w]] == 0} {
	$w mark set insert end-1c;
	if {[catch {
	    set sel [selection get];
	    $w insert end-1c [selection get -displayof $w -selection PRIMARY];
	}]} {
	    catch {$w insert end-1c\
		    [selection get -displayof $w -selection CLIPBOARD]}
	}
	if {[$w cget -state] == "normal"} {focus $w}
    } else {
	uplevel 1 [list error "$w isn't a text widget"];
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::add - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Insert new evaluated command to the public and private
#               history list.
# ---------------------------------------------------------------------------
proc rtl_shell::add {base cmd} {
    variable var;
    variable $base;

    if {[catch {
	history add [set ${base}(cmd)];
    } msg]} {
	puts stderr "Error : history $msg";
    }

    set ${base}(history,index) -1;
    set ${base}(history)\
	    [linsert [set ${base}(history)] 0 [set ${base}(cmd)]];
    set ml [getSettings Maximum,History 20];
    if {[llength [set ${base}(history)]] > $ml} {
	    set ${base}(history)\
		    [lreplace  [set ${base}(history)] $ml end];
    }

    set ${base}(buffer) [linsert [set ${base}(buffer)] 0 $cmd];
    set mb [get-maxbuffer $base];
    if {[llength [set ${base}(buffer)]] > $mb} {
	set ${base}(buffer) [lreplace [set ${base}(buffer)] $mb end];
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::insertScript - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_shell::insertScript {base} {
    variable var;
    variable $base;
    variable w;

    $base$w(2) delete [set ${base}(start)] end;
    $base$w(2) insert [set ${base}(start)]\
	    [lindex [set ${base}(history)]\
	    [set ${base}(history,index)]];

}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::backspace - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : The given keystroke is a Backspace kye and must
#               be treated separatly.
# ---------------------------------------------------------------------------
proc rtl_shell::backspace {base typ} {
    variable var;
    variable $base;
    variable w;

    set start [set ${base}(start)];
    set start [split $start .];
    set sl [lindex $start 0];
    set sc [lindex $start 1];

    if {$typ == "-backspace"} {
	incr sc;
    }

    set insert [split [$base$w(2) index insert] .];
    set il [lindex $insert 0];
    set ic [lindex $insert 1];


    if {$sc <= $ic && $sl == $il || $sl < $il} {

	if {[$base$w(2) tag nextrange sel $sl.$sc end] != ""} {
	    $base$w(2) delete sel.first sel.last
	} else {
	    switch -- $typ {
		"-backspace" {
		    $base$w(2) delete insert-1c;
		}
		"-delete" {
		    $base$w(2) delete insert;
		}
	    }
	    $base$w(2) see insert;
	}
    }
    
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::up - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Browse the local history using the up key
# ---------------------------------------------------------------------------
proc rtl_shell::up {base} {
    variable var;
    variable $base;

    set ml [llength [set ${base}(history)]];
    incr ${base}(history,index);
    if {[set ${base}(history,index)] >= $ml} {
	set ${base}(history,index) 0;
    }
    insertScript $base;
}

# ---------------------------------------------------------------------------
# Function    : rtl_shell::down - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : See above up-key
# ---------------------------------------------------------------------------
proc rtl_shell::down {base} {
    variable var;
    variable w;
    variable $base;

    set ml [llength [set ${base}(history)]];
    incr ${base}(history,index) -1;
    if {[set ${base}(history,index)] < -1} {
	set ${base}(history,index) $ml;
    } elseif {[set ${base}(history,index)] == -1} {
	$base$w(2) delete [set ${base}(start)] end;
	return;
    }
    insertScript $base;
}


# ---------------------------------------------------------------------------
# Function    : rtl_shell::getSettings - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Get default values from application
# ---------------------------------------------------------------------------
proc rtl_shell::getSettings {key def} {
    if {[string compare "::getSettings" [info commands "::getSettings"]] == 0} {
	set def [::getSettings $key $def];
    }
    return $def;
}

# ===========================================================================
# Tasks:
#
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 17.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------




