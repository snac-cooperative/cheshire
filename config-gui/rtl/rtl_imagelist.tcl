# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: Package Container area
# -----------------------------------------------------------------------
package provide rtl_imagelist 1.2;
package require rtl;

####VG-START####
set rtl_imagelist(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_imagelist; 
	set src [file join $rtl_imagelist(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

# --------------------------------------------------------------------------- 
# class (namespace): rtl_imagelist 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_imagelist {
 
    variable var; 
    variable w; 
    array set w [list 4 .gridwin.vert 0 {} 1 .gridwin 2 .gridwin.text 3 \
        .gridwin.hori]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list  highlightbackground {0 highlightbackground} \
        select select height {0 height} \
        imagewidth imagewidth \
        highlightthickness {0 highlightthickness} original frame \
        attributes [list images select width height imagewidth\
	selectbackground activebackground activeforeground\
        background borderwidth relief highlightthickness highlightcolor\
        highlightbackground cursor] highlightcolor {0 highlightcolor} \
        width {0 width}\
	activebackground activebackground activeforeground activeforeground\
	background {2 background} \
        images images borderwidth {0 borderwidth} cursor {2 cursor} \
        selectbackground selectcolor relief {0 relief}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_imagelist - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -images
#             : -select
#             : -width
#             : -height
#             : -imagewidth
#             : -selectcolor
#             : -background
#             : -borderwidth
#             : -relief
#             : -highlightthickness
#             : -highlightcolor
#             : -highlightbackground
#             : -cursor 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_imagelist 
# --------------------------------------------------------------------------- 
proc rtl_imagelist {{base .rtl_imagelist} args} {
 
    variable rtl_imagelist::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_imagelist $base$w(0) $args] $base -class Rtl_imagelist;

        # Childs to $base$w(0), rtl_imagelist of class: 	Rtl_Imagelist
        rtl_gridwin $base$w(1) -widget "$base$w(2)" -auto "y" -fill "both"\
            -sides "right" -width "10" -height "10";
        # Managed children to $base$w(1):

        text $base$w(2) -cursor "arrow" -state disabled\
            -xscrollcommand "rslider $base$w(2) $base$w(3) x y; $base$w(3) set"\
            -yscrollcommand "rslider $base$w(2) $base$w(4) y y; $base$w(4) set";

        pack $base$w(1) -anchor center -expand 1 -fill both -ipadx 0\
             -ipady 0 -padx 0 -pady 0 -side top; 
        set base [mkProc $base rtl_imagelist $args]; 
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
set rat(WIDGETCLASS,rtl_imagelist) rtl_imagelist;
# ---------------------------------------------------------------------------

####VG-STOP####

# ===========================================================================
# Generic Attributes:
# ---------------------------------------------------------------------------
create-action rtl_imagelist images select {selectcolor blue2}\
	{imagewidth 32} {activebackground gray90}\
	{activeforeground black};

# ===========================================================================
# Overwritten Generic Attributes (set Methods)
# ---------------------------------------------------------------------------
# Function    : rtl_imagelist::set-selectcolor - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : 
# ---------------------------------------------------------------------------
proc rtl_imagelist::set-selectcolor {base color} {
    variable w;

    ::set-action rtl_imagelist $base selectcolor $color;

    foreach child [winfo children $base$w(2)] {
	$child configure -selectcolor $color;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_imagelist::set-imagewidth - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Reconfigure display image width
# ---------------------------------------------------------------------------
proc rtl_imagelist::set-imagewidth {base iw} {
    variable w;

    ::set-action rtl_imagelist $base imagewidth $iw;

    foreach child [winfo children $base$w(2)] {
	$child configure -width $iw -height $iw;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_imagelist::set-images - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Reassign all images to the imagelist
# ---------------------------------------------------------------------------
proc rtl_imagelist::set-images {base images} {
    variable w;
    ::set-action rtl_imagelist $base images $images;

    foreach child [winfo children $base$w(2)] {
	destroy $child;
    }

    $base$w(2) configure -state normal;
    set sc [get-selectcolor $base];
    set iw [get-imagewidth $base];
    set co [$base$w(2) cget -background];
    set ab [get-activebackground $base];
    set af [get-activeforeground $base];

    set image_l [llength $images];
    for {set i 0} {$i < $image_l} {incr i} {
	set img [lindex $images $i];
	if {[catch {
	    set win [checkbutton $base$w(2).image_$i -bd 0 -highlightthickness 1\
		    -variable ::rtl_imagelist::${base}(select) -onvalue $img\
		    -selectcolor $sc -width $iw -height $iw\
		    -activebackground $ab -activeforeground $af\
		    -background $co\
		    -highlightbackground $co\
		    -image $img -indicatoron 0];
	    set imgname [string trim $img _];
	    set imgtext [::format [opget $base imagehelp\
		    "Image: %s|%s"] $imgname $imgname];
	    option add *[string trim $win .].help $imgtext;
	    $base$w(2) window create end -window $win -stretch 1;
	} msg]} {
	    puts stderr "image failed $img: reson is <$msg>";
	}
    }
    $base$w(2) configure -state disabled;
}


# ===========================================================================
# Tasks:
# ---------------------------------------------------------------------------
# Port RTL 1.1      : 15.06.199,Roger
# Test              :
# Integration VG 2.1:
# System            :
# ---------------------------------------------------------------------------

