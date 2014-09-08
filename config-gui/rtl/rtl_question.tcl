# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary Modal Dialog with right sided buttons
# -----------------------------------------------------------------------
package provide rtl_question 1.2;
package require rtl;

####VG-START####

namespace eval rtl_question {
    variable var;
    variable w;
    variable answer;

    set answer break;
    array set w [list 0 {} 3 .client 2 .command 1 .footer 4 .footer.line];
    array set var [list original frame attributes\
	    {footer default clientfcn closefcn type title help}\
	    default default\
	    help help\
	    footer [list footer]\
	    clientfcn [list clientfcn]\
	    type [list type]\
	    closefcn onclosefcn\
	    title [list title]];
}

proc rtl_question {base args} {
    variable rtl_question::w; 
    if {![winfo exists $base]} {
	toplevel  $base -class Rtl_question;

	wm withdraw $base;
	
	pack [frame $base$w(1) -bd 0 -highlightthickness 0 -height 10]\
		-side bottom -fill x;
	pack [frame $base$w(2) -bd 0 -highlightthickness 0]\
		-padx 5 -pady 5 -side right -fill y;
	pack [frame $base$w(3) -bd 0 -highlightthickness 0]\
		-side left -expand 1 -fill both;

	pack [frame $base$w(4) -bd 1 -relief sunken -height 2 -width 100]\
		-anchor w -padx 5 -pady 5 -side top;
	
	catch {rtl_question::init $base}
        set base [mkProc $base rtl_question $args];  
    }

    set oldFocus [focus];
    set oldGrab [grab current $base];
    if {$oldGrab != ""} {
	set grabStatus [grab status $oldGrab];
    }

    #update idletasks;
    catch {
	centerPos $base;
    }

    wm deiconify $base;
    grab $base;
    tkwait variable rtl_question::answer;
    global tkPriv;
    set tkPriv(postedMb) {};
    grab release $base;
    return [set rtl_question::answer];
}


####VG-STOP####

# ===========================================================================
# Default Resources for the question widget class:
# ---------------------------------------------------------------------------
option add *Rtl_question.command.ok.help {Accept action and close dialog|Accept action}
option add *Rtl_question.command.cancel.help {Deny action and close dialog|Deny action}
option add *Rtl_question.command.yes.help {Accept action and close dialog|Accept action}
option add *Rtl_question.command.no.help {Deny action and close dialog|Deny action}
option add *Rtl_question.command.ignore.help {Ingore given warning and proceed with\
	previsious command|Ignore warning}
option add *Rtl_question.command.retry.help\
	{Try again previsious action|Repeat previsious action}

# Apply the type value to the database to ensure that, type will
# be called, even when no -type attribute was added to the call. 
option add *Rtl_question.type ok;
option add *Rtl_question.command*Button.width 8;

# ===========================================================================
# Construction methods:
# ---------------------------------------------------------------------------
# Function    : rtl_question::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Secondary constructor for modal dialog of class question
# ---------------------------------------------------------------------------
proc rtl_question::init {base} {

    variable $base;

    wm transient $base [winfo parent $base];
    wm protocol $base WM_DELETE_WINDOW\
	    "set rtl_question::answer cancel;rtl_question::destroy $base";
    #after 1 $base set-type "\[$base get-type\]";
}

# ---------------------------------------------------------------------------
# Function    : rtl_question::close - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Close button, destroy current dialog
# ---------------------------------------------------------------------------
proc rtl_question::close {base value} {
    variable answer;

    ::eval [get-onclosefcn $base];
    update idletasks;
    destroy $base;
    set answer $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_question::destroy - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Destructor for widgets of class question
# ---------------------------------------------------------------------------
proc rtl_question::destroy {base} {
    variable var;
    variable $base;

    foreach elem [array names var $base,*] {
	unset var($elem);
    }
    
    if {[info exists $base]} {
	unset $base;
    }

    ::destroy $base;
}

# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_question {footer {}} {title Question} {type ok}\
	clientfcn onclosefcn {default ok} help;

# ===========================================================================
# Overwritten Generic Attributes (set methods):
# ---------------------------------------------------------------------------
proc rtl_question::set-help {base value} {
    variable w;

    mkStatic footer [get-help $base] {}; 
    set img _help;
    if {[lsearch -exact [image names] $img] < 0} {
	set img {};
    }

    pack [button $base$w(1).help\
	    -image $img\
	    -highlightthickness 0 -text "?"\
	    -command "showHref $value"]\
	    -side left -pady 5 -padx 5; 
    return [::set-action rtl_question $base help $value];
}

# ---------------------------------------------------------------------------
# Function    : rtl_question::set-footer - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_question::set-footer {base value} {
    variable w;

    mkStatic footer [get-footer $base] {}; 

    set i 0;
    foreach elem $value {
	pack [::eval {checkbutton $base$w(1).footer_$i\
		-highlightthickness 0} $elem]\
		-side left -pady 5 -padx 5; 
	incr i;
    }

    return [::set-action rtl_question $base footer $value];
}


# ---------------------------------------------------------------------------
# Function    : rtl_question::set-title - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_question::set-title {base value} {
    catch {wm title $base $value}
    return [::set-action rtl_question $base title $value];
}

# ---------------------------------------------------------------------------
# Function    : rtl_question::set-type - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_question::set-type {base value} {
    variable w;

    foreach child [winfo children $base$w(2)] {
	destroy $child;
    }

    switch $value {
	"abortretryignore" {
	    foreach t [list abort retry ignore] {
		pack [button $base$w(2).$t\
			-text [opget $base $t [mkFirstUp $t]]\
			-command "rtl_question::close $base $t"]\
			-padx 5 -pady 5\
			-fill x -side top;
	    }
	}
	"ok" {
	    pack [button $base$w(2).ok -text [opget $base ok Ok]\
		    -command "rtl_question::close $base ok"] -padx 5 -pady 5\
		    -fill x -side top;

	    after idle focus $base$w(2).ok;
	    bind $base <Return> "$base$w(2).ok invoke";
	}
	"okcancel" {
	    foreach t [list ok cancel] {
		pack [button $base$w(2).$t\
			-text [opget $base $t [mkFirstUp $t]]\
			-command "rtl_question::close $base $t"]\
			-padx 5 -pady 5\
			-fill x -side top;
	    }

	    bind $base <Return> "$base$w(2).ok invoke";
	    bind $base <Escape> "$base$w(2).cancel invoke";

	}
	"retrycancel" {
	    foreach t [list retry cancel] {
		pack [button $base$w(2).$t\
			-text [opget $base $t [mkFirstUp $t]]\
			-command "rtl_question::close $base $t"]\
			-padx 5 -pady 5\
			-fill x -side top;
	    }
	    bind $base <Escape> "$base$w(2).cancel invoke";
	}
	"yesno" {
	    foreach t [list yes no] {
		pack [button $base$w(2).$t\
			-text [opget $base $t [mkFirstUp $t]]\
			-command "rtl_question::close $base $t"]\
			-padx 5 -pady 5\
			-fill x -side top;
	    }
	    bind $base <Return> "$base$w(2).yes invoke";
	    bind $base <Escape> "$base$w(2).no invoke";
	}
	"yesnocancel" {
	    foreach t [list yes no cancel] {
		pack [button $base$w(2).$t\
			-text [opget $base $t [mkFirstUp $t]]\
			-command "rtl_question::close $base $t"]\
			-padx 5 -pady 5\
			-fill x -side top;
	    }

	    bind $base <Return> "$base$w(2).yes invoke";
	    bind $base <Escape> "$base$w(2).cancel invoke";
	}
    }

    set wf  $base$w(2).[get-default $base];
    if {[winfo exists $wf]} {
	after idle focus $wf;
    }

    return [::set-action rtl_question $base type $value];
}


# ---------------------------------------------------------------------------
# Function    : rtl_question::set-clientfcn - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : New extended with %s substitution for clientfcn.
#
# ---------------------------------------------------------------------------
proc rtl_question::set-clientfcn {base value} {
    variable w;
    mkStatic clientfcn [get-clientfcn $base] {};

    if {[string first "%s" $value] > 0} {
	pack [::eval [format $value $base$w(3).client]]\
		-expand 1 -fill both -padx 5 -pady 5;
    } else {
	pack [$value $base$w(3).client]\
		-expand 1 -fill both -padx 5 -pady 5;
    }
    return [::set-action rtl_question $base clientfcn $value];
}

# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 14.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

