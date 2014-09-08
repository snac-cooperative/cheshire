# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999 Patzschke + Rasp Software AG
# 
# Description: RunTimeLibrary Tree Implementation
# -----------------------------------------------------------------------
package provide rtl_tree 1.2;
package require rtl;
package require rtl_gridwin;

####VG-START####
set rtl_tree(dir) [file dirname [info script]]; 
proc srcStartUp {file type script} {
 	variable ::rtl_tree; 
	set src [file join $rtl_tree(dir)  $file]; 
	if [catch {uplevel "#0" $script $src} result] {
 		puts stderr  "Warning file $src of type:$type ($result)"; 
	}
}

#srcStartUp rtl_tree.res tkRESOURCE {option readfile};
# --------------------------------------------------------------------------- 
# class (namespace): rtl_tree 
# mapping and constructor (published) 
# --------------------------------------------------------------------------- 
namespace eval rtl_tree {
 
    variable var; 
    variable w; 
    array set w [list 4 .gridwin.vert 0 {} 1 .gridwin 2 .gridwin.canvas 3 \
        .gridwin.hori]; 
    # The Information about a new class is stored in a 
    # local namespace from VisualGIPSY 
    array set  var [list\
        contextfcn contextfcn original frame\
	connect connect selectfcn selectfcn indentx indentx\
	indenty indenty highlightthickness [list\
	{0 highlightthickness} {2 highlightthickness}]\
	\
	attributes [list\
	selectforeground selectbackground font select char type\
        indentx indenty root connect contextfcn width height borderwidth\
        fiximagewidth format columnselect itemselect visible selectfcn\
	openfcn drag dragfcn dragbitmap relief background\
	highlightbackground highlightcolor highlightthickness\
	hasChildrenfcn requestChildrenfcn typedSel\
	sides scrollwidth show]\
	\
	hasChildrenfcn hasChildrenfcn requestChildrenfcn requestChildrenfcn\
	\
	typedSel typedSel\
	root root itemselect itemselect columnselect columnselect\
	select select height {0 height} format format\
	show show openfcn openfcn selbbox selbbox globalbbox globalbbox\
	typ typ type typ dragfcn dragfcn drag drag font font\
	highlightcolor [list {0 highlightcolor} {2 highlightcolor}]\
        background {2 background} dragbitmap dragbitmap\
	borderwidth {0 borderwidth} selectforeground selectforeground\
	selectbackground selectbackground relief {0 relief}\
	char char visible visible highlightbackground [list\
	{0 highlightbackground} {2 highlightbackground}]\
        fiximagewidth fix scrollwidth {1 scrollwidth} \
        expandfcn expandfcn width {0 width} sides {1 sides}]; 
}
 
# --------------------------------------------------------------------------- 
# Procedure   : rtl_tree - widget constructor 
# 
# Returns     : base - valid widget 
# Parameters  : base - widget to create 
#             : args - widget attributes 
#                      defined in mapping area 
# 
# Description : published options
#             : -font
#             : -select
#             : -char
#             : -typ
#             : -indentx
#             : -indenty
#             : -root
#             : -connect
#             : -contextfcn
#             : -width
#             : -height
#             : -borderwidth
#             : -fiximagewidth
#             : -format
#             : -itemselect
#             : -visible
#             : -selectfcn
#             : -openfcn
#             : -drag
#             : -dragfcn
#             : -dragbitmap
#             : -background
#             : -highlightbackground
#             : -highlightcolor
#             : -highlightthickness
#             : -sides
#             : -scrollwidth 
# --------------------------------------------------------------------------- 
# The constructor must be part of the global namespace 
# proc rtl_tree 
# --------------------------------------------------------------------------- 
proc rtl_tree {{base .rtl_tree} args} {
 
    variable rtl_tree::w; 
    if {![winfo exists $base]} {
 
        # Create component root 
        [getCreator rtl_tree $base$w(0) $args] $base -class Rtl_tree\
		-borderwidth 1 -relief sunken -height "200" -width "200";
        # Childs to $base$w(0), rtl_tree of class: 	Rtl_Tree
        rtl_gridwin $base$w(1) -widget "$base$w(2)" -auto "1" -fill "both" \
		-sides "bottom right";
        canvas $base$w(2);


	bind $base$w(2) <1> "rtl_tree::setfocus $base;\
		#rtl_tree::startSelection $base %x %y";

#  	bind $base$w(2) <B1-Motion> "\
#  		rtl_tree::motionSelection $base %x %y";

#  	bind $base$w(2) <ButtonRelease-1> "\
#  		rtl_tree::releaseSelection $base %x %y";



	pack propagate $base 0;
        pack $base$w(1) -expand 1 -fill both -side left; 
        catch {rtl_tree::init $base;} 
        set base [mkProc $base rtl_tree $args]; 
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
set rat(WIDGETCLASS,rtl_tree) rtl_tree;
# ---------------------------------------------------------------------------

####VG-STOP####

namespace eval rtl_tree {
    variable prefs;

    if {[catch {
	set fg               black;
	set bg               white;
	entry .__m#rtl_tree;

	set prefs(color,fg)  [.__m#rtl_tree cget -foreground];
	set prefs(color,bg)  [.__m#rtl_tree cget -background];
	set prefs(color,abg) [.__m#rtl_tree cget -selectbackground];
	set prefs(color,afg) [.__m#rtl_tree cget -selectforeground];
	set prefs(font)      [.__m#rtl_tree cget -font];

	# Set the default background to that of an entry.
	::option add *Rtl_tree.background $prefs(color,bg);

	destroy .__m#rtl_tree;
    } msg]} {
	puts stderr "Error appeard in conf: $msg";
    }

    set maskdata "#define solid_width 9\n#define solid_height 9";
    append maskdata {
	static unsigned char solid_bits[] = {
	    0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01, 0xff, 0x01,
	    0xff, 0x01, 0xff, 0x01, 0xff, 0x01
	};
     }

     set data "#define open_width 9\n#define open_height 9";
     append data {
	 static unsigned char open_bits[] = {
	     0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x7d, 0x01, 0x01, 0x01,
	     0x01, 0x01, 0x01, 0x01, 0xff, 0x01
	 };
      }

      image create bitmap rtl_tree:openbm -data $data -maskdata $maskdata \
	      -foreground $fg -background $bg;

      set data "#define closed_width 9\n#define closed_height 9";
      append data {
	  static unsigned char closed_bits[] = {
	      0xff, 0x01, 0x01, 0x01, 0x11, 0x01, 0x11, 0x01, 0x7d, 0x01, 0x11, 0x01,
	      0x11, 0x01, 0x01, 0x01, 0xff, 0x01
	  };
      }

      image create bitmap rtl_tree:closedbm -data $data -maskdata $maskdata \
	      -foreground $fg -background $bg;

      image create bitmap _dummy -background cornsilk -foreground $fg\
	      -data {#define sccircle_width 14
#define sccircle_height 14
#define sccircle_x_hot 0
#define sccircle_y_hot 0
static unsigned char sccircle_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf0, 0x01, 0xfc, 0x07, 0xfc, 0x07, 0xfe, 0x0f,
   0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x0f, 0xfe, 0x0f, 0xfc, 0x07, 0xfc, 0x07,
   0xf0, 0x01, 0x00, 0x00};
}\
	-maskdata {#define screctangle_width 14
#define screctangle_height 14
#define screctangle_x_hot 0
#define screctangle_y_hot 0
static unsigned char screctangle_bits[] = {
   0x00, 0x00, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfe, 0x1f, 0x00, 0x00};
};

}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::init - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : make initial settings for each componenet instance
# ---------------------------------------------------------------------------
proc rtl_tree::init {base} {
    variable w;
    variable var; variable $base;

    set ${base}(drag) 1;
    set ${base}(selection) {};
    set ${base}(selidx) {};
    set ${base}(char) "/";
    set ${base}(root) 1;
    set ${base}(connect) 1;
    set ${base}(indentx) 5;
    set ${base}(indenty) 20;
    set ${base}(stream) [list];
    set ${base}(number) 1;
    set ${base}(format) %s;
    set ${base}(fix) 0;
    set ${base}(columnselect) 0;
    set ${base}(itemselect) 0;
    set ${base}(visible) {label text title class geometry manager}
    set ${base}(font) {Helvetica -12 bold};
    set ${base}(show) {};

    # events bound to canvas ...
    $base$w(2) bind tag <Control-1>\
	    "rtl_tree::browse $base %x %y 1";
 
    $base$w(2) bind tag <1>\
	    "rtl_tree::browse $base %x %y;break";    
    #$base$w(2) bind tag <ButtonRelease-1> rtl_tree::unset ${base}(browse);

     $base$w(2) bind node_select <Control-1>\
 	    "rtl_tree::browse $base %x %y 1;break";
    
      $base$w(2) bind node_select <ButtonRelease-1>\
  	    "rtl_tree::browse $base %x %y";    
    
    bind $base$w(2) <Double-1>\
	    "rtl_tree::opencallback $base %x %y";
    
    bind $base$w(2) <<Context>>\
	    "rtl_tree::opencontext $base %x %y %X %Y";

    # Redefine navigation keys but don't overwrite
    # Other application related keys.
    foreach k [list\
	    Up Down Right Left plus minus Return Home End] {
	bind $base$w(2) <${k}> \
		"rtl_tree::moveKey $base %K;break";
    }

    $base$w(2) bind Rtl_TreeDrag <1>\
	    {rtl_tree::drag-select %W %x %y %X %Y}
    $base$w(2) bind Rtl_TreeDrag <B1-Motion>\
	    {rtl_tree::drag-motion %W %x %y %X %Y}
    $base$w(2) bind Rtl_TreeDrag <ButtonRelease-1>\
	    {rtl_tree::drag-release %W %x %y %X %Y} 

    rtl_tree::inititem $base /;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::set-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Generic local set-action
# ---------------------------------------------------------------------------
proc rtl_tree::set-action {base typ value} {
    ::set-action rtl_tree $base $typ $value;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::get-action - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Generic local get-action
# ---------------------------------------------------------------------------
proc rtl_tree::get-action {base typ} {
    ::get-action rtl_tree $base $typ;
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::dump - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Make a backup of "all" instance data 
# ---------------------------------------------------------------------------
proc rtl_tree::dump {base} {
    variable $base;
    return [array get $base];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::restore - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Restore and redraw an instance of class tree
# ---------------------------------------------------------------------------
proc rtl_tree::restore {base argl} {
    variable $base;

    if {[info exists $base]} {
	unset $base;
    }

    array set $base $argl;
    build $base;
}

# ===========================================================================
# internal used methodes:
# ---------------------------------------------------------------------------
##
## substitute file dirname and file tail
## -------------------------------------
## 
## Needed for the macIntosh port of this template!
##
## 7.6.99, Roger
# ---------------------------------------------------------------------------
# Function    : rtl_tree::filter - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : new feature filter some attributes!
#               The filter mechanism is currently not used.
# ---------------------------------------------------------------------------
proc rtl_tree::filter {base node} {
    return 1;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::filedirname - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : The filedirname method is used to substitute the unix file
#               notation under MacOS. Rtl_Tree nodes are internal represented
#               in unix file system notation.
# ---------------------------------------------------------------------------
proc rtl_tree::filedirname {file} {
    variable ::tcl_platform;

    if {[string compare  "macintosh" $tcl_platform(platform)] == 0} { 
	set file_l [split $file /];
	set last   [expr {[llength $file_l] - 1}];

	if {$last <= 1} {
	    return "/";
	}

	return [join [lreplace $file_l $last $last] /];
    }

    return [file dirname $file];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::filetail - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : The filetail method is used to substitute the unix file
#               notation under MacOS. Rtl_Tree nodes are internal represented
#               in unix file system notation.
# ---------------------------------------------------------------------------
proc rtl_tree::filetail {file} {
    variable ::tcl_platform;

    if {[string compare  "macintosh" $tcl_platform(platform)] == 0} { 
	set file_l [split $file /];
	set last   [expr {[llength $file_l] - 1}];
	return     [lindex $file_l $last];
    }

    return [file tail $file];
}

# ---------------------------------------------------------------------------
# handle build:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::buildwhenidle - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : after ilde new build the view
# ---------------------------------------------------------------------------
proc rtl_tree::buildwhenidle {base} {
    dowhenidle $base build;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::rebuildwhenidle - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : after idle rebuild the view starting with lid (in stream)
# ---------------------------------------------------------------------------
proc rtl_tree::rebuildwhenidle {base lid} {
    dowhenidle $base rebuild $lid;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::rebuild - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : rebuild is used to setup the treview past a change
#               rebuild trys to minimize the needed redraw actions.
#
# Todo        : Extend move operation, use a invers move operation
#               if the tree is currently manipulated in the top
#               section.
# ---------------------------------------------------------------------------
proc rtl_tree::rebuild {base vp  {movef 1}} {
    variable var;
    variable $base;
    variable w;

    # Use the build process when drawing the root-item,
    # required to ensure that root is allready drawn.

    # Note : vp need not to be visible.
    #        which mean stream does not contain vp

    set id1 [lsearch -exact [set ${base}(stream)] $vp];

    if {[string compare "/" $vp] == 0 || $id1 <= 0} {
	return [build $base];
    }

    set id2 [expr {$id1 + 1}];
    set iny [get-indenty $base];
    set inx [get-indentx $base];

    set c $base$w(2);
    
    $c delete node_select;
    if {![info exists ${base}($vp:tag)]} {
	unset ${base}(insert);
	return;
    }

    set bbox1 [$c bbox item__[set ${base}($vp:nb)]];

    # make a bbox across complete entries of this item
    # and thereafter calcualte 

    set ystart1 [expr int([lindex $bbox1 3] - $iny / 2)];
    set ystart  $ystart1;

    # make a backup of stream beginning at id2
    # draw will manipulate the stream !

    set local_stream [lreplace [set ${base}(stream)] 0 $id1];

    ## remove the local stream to those elements with
    ## a lower or equal level to the selected $vp.

    ## Note: The move level will decrease,
    ##       don`t store children of aunt and ancle nodes
    ##       inside the move_stream! 


    if {![info exists ${base}(insert)]} {
	set local_insert {};
    } else {
	set local_insert [set ${base}(insert)];
    }

    unset ${base}(insert);

    set skip_move 0;
    set ll        [llength [set ${base}(stream)]];

    if {$id2 < $ll} {
	set ${base}(stream) [lreplace [set ${base}(stream)] $id2 end];
	set local_part1     [set ${base}(stream)];
    } else {
	set skip_move 1;
    }

    # Analyze which items are of interest ... 
    set level [expr {1 + [llength [split $vp /]]}];
    
    if {[llength $local_insert] > 0} {

	# Insert Operationen ...
	# insert new nodes
	set  flag   0; 
	incr ystart 2;

	foreach newitem $local_insert {
	    
	    # skip to insert double entries
	    if {[info exists ${base}($newitem:tag)]} continue;

	    set vparent [filedirname $newitem];
	    incr ystart $iny;
	    
	    # Calculate x,y position for new node
	    set xstart [expr {[set ${base}($vparent:x)] + $inx}];
	    set ystart [draw $base $newitem $xstart $ystart $vparent];
	}

    }

    set  move_level      $level;
    set  moveitem_stream [list];

    foreach item $local_stream {

	set new_level [llength [split $item /]];
	if {$new_level <= $move_level} {
	    lappend moveitem_stream $item;
	    set move_level $new_level;
	}
    }

    if {$skip_move} {
	drawselection $base $movef;
	set abox [$c bbox all];

	foreach {i val} [get-globalbbox $base] {
	    set abox [lreplace $abox $i $i [expr {[lindex $abox $i] + $val}]];
	}

	$c configure -scrollregion $abox;
	return;
    }


    # Two Scenarios
    # 1. A new item was inserted ystart/ystart1 differs
    # 2. The previsous call was initiated by a delete call.
    #    which means bbox2 must be anylysed.
    #
    # There are two opportunities here
    # move the lower or the higher part ...
    # Moving the higher part may introduce a
    # additional problem since this means moveChildren has to
    # be deactivated .


    set vp2 [lindex $local_stream 0];

    
    if {[string compare {} $vp2] != 0} {
	if {![info exists ${base}($vp2:tag)]} return;

        set bbox2   [$c bbox [set ${base}($vp2:tag)]];
	set ystart2 [expr {([lindex $bbox2 1] + [lindex $bbox2 3]) / 2}];

	# Check difference between ystart2 and ystart1
	set yplace  [expr {$ystart + $iny}];
	set ymove   [expr {$yplace - $ystart2}];

	set ltop [llength [set ${base}(stream)]];
	set lbot [llength $local_stream];

	if {$ltop < $lbot} {
	
	    set ymove [expr {$ymove * -1}];
	    set reverse_stream [set ${base}(stream)];
	    # Invert move direction
	    set new_stream [list];

	    set rlevel [llength [split $vp2 /]];

	    # yplace is the place to move to.
	    set invert_stream [list];
	    foreach item [set ${base}(stream)] {
		set invert_stream [linsert $invert_stream 0 $item];
	    }

	    foreach moveitem $invert_stream {
		set mp [split $moveitem /];
		set mlevel [llength $mp];
		
		if {$mlevel <= $rlevel} {
		    set rlevel $mlevel;
		    lappend new_stream $moveitem;
		}

		incr ${base}($moveitem:y) $ymove;
	    }

	    # look if this item is independent from vp and its parents
	    foreach item $new_stream {
		if {[string first $item $vp2] != 0} {
		    # entire branch
		    if {[info exists ${base}($item:children)]} {
			moveChildren $base $item $ymove;
		    }
		}

		$c move __[set ${base}($item:nb)] 0 $ymove;
	    }

	    foreach item  $local_stream {
		lappend ${base}(stream) $item;
	    }

	    foreach v $moveitem_stream {
		set nb      [set ${base}($v:nb)];
		$c delete   [$c find withtag con__$nb];
	    }

	} else {

	    # yplace is the place to move to.
	    foreach moveitem $local_stream {
		incr        ${base}($moveitem:y) $ymove;
		lappend     ${base}(stream) $moveitem;
	    }
	    
	    # Don't parse the entire hierarchy,
	    # instead move complete branches ...
	    foreach moveitem $moveitem_stream {
		moveItem $base /[string trim $moveitem /] $level $ymove;
	    }
	}
	
	if {[get-connect $base]} {
	    reconnect-stream $base $moveitem_stream;
	}
    
    }

    # Redraw selecetion and calculate scrollregion ...

    set abox [$c bbox all];

    foreach {i val} [get-globalbbox $base] {
	set abox [lreplace $abox $i $i [expr {[lindex $abox $i] + $val}]];
    }

    $c configure -scrollregion $abox;
    if {$ltop < $lbot} {
	set yv [$c yview];
	if {[lindex $yv 0] == 0 && [lindex $yv 1] == 1} {
	    $c yview moveto 0;
	}
    }
    #$c configure -scrollregion [$c bbox all];
    drawselection $base $movef;
}

proc rtl_tree::moveItem {base v level ymove {chflag 0}} {
    variable var;
    variable $base;
    variable w;

    set c $base$w(2);

    set nb      [set ${base}($v:nb)];
    $c delete   [$c find withtag con__$nb];
    $c move     __$nb 0 $ymove;
    
    # Move all children of this particular item ...
    if {[info exists ${base}($v:open)]\
	    && [set ${base}($v:open)]} {
	if {[info exists ${base}($v:children)]} {
	    moveChildren $base $v $ymove;
	}
    }
}

proc rtl_tree::moveChildren {base v ymove} {
    variable $base;
    variable w;

    set c $base$w(2);

    set iTag item_[getCurrentTag $base $v];

    if {[string compare item_glymna $iTag] == 0} {
	return;
    }

    # look for further subtrees ...
    foreach child [set ${base}($v:children)] {

	set vc [v $v $child];

	if {[info exists ${base}($vc:open)]\
		&& [set ${base}($vc:open)]} {
	    if {[info exists ${base}($vc:children)]} {
		moveChildren $base $vc $ymove;
	    }
	}
    }

    # Move complete subtree ...
    $c move $iTag 0 $ymove;
    
}

proc rtl_tree::unset {vari} {
    upvar $vari upv;
    if {[info exists upv]} {
	::unset upv;
    }
}
# ---------------------------------------------------------------------------
# Function    : rtl_tree::build - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : build entire view
# ---------------------------------------------------------------------------
proc rtl_tree::build {base} {
    variable var;
    variable $base;
    variable w;

    set c $base$w(2);

    $c delete all;

    foreach arv [array names $base tag*] {
	unset ${base}($arv);
    }

    # Remove entire stream and also the insert stream
    unset ${base}(stream);
    unset ${base}(insert);
    foreach j [$c find all] {
	$c delete $j;
    }

    # Note : / must exists!
    if {[get-root $base] != 1} {

	set iny   [get-indenty $base];
	set x     [get-indentx $base];
	set ystart 0;

	if {[info exists ${base}(/:children)]} {
	    foreach child [set ${base}(/:children)] {
		incr ystart $iny;
		set ystart [draw $base /$child $x $ystart {}];
	    }
	}
    } else {
	draw $base / [get-indentx $base] [get-indenty $base] {};
    }


    set abox [$c bbox all];

    if {[llength $abox] < 4 } {
	set abox [list 0 0 0 0];
    }
    foreach {i val} [get-globalbbox $base] {
	set abox [lreplace $abox $i $i [expr {[lindex $abox $i] + $val}]];
    }

    $c configure -scrollregion $abox;


    #set bbox [$c bbox all];
    #$c config -scrollregion $bbox;
    drawselection $base;
}


# ---------------------------------------------------------------------------
# handle various stuff:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::c2base - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : figure out what component is parent of that canvas widget
# ---------------------------------------------------------------------------
proc rtl_tree::c2base {c} {
    return [winfo parent [winfo parent $c]];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::v - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : concatinate childname with parentname
# ---------------------------------------------------------------------------
proc rtl_tree::v {vp vname} {
    if {$vp == "/"} {
	return /$vname;
    }
    return $vp/$vname;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::dowhenidle - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : after ilde handling call fcn with args. Note base is
#               insert anywhere.
# ---------------------------------------------------------------------------
proc rtl_tree::dowhenidle {base fcn args} {
    variable var; variable $base;
    if {![info exists ${base}(pending,$fcn)]} {
	set ${base}(pending,$fcn) 1;
	after idle [linsert $args 0 rtl_tree::$fcn $base];
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::inititem - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : make initial settings foreach node displayed in that rtl_tree
# ---------------------------------------------------------------------------
proc rtl_tree::inititem {base v} {
    variable var;
    variable $base;
    variable w;

    if {[info exists ${base}($v:children)]} {
	if {[llength [set ${base}($v:children)]] == 1\
		&& [string compare {}\
		[lindex [set ${base}($v:children)] 0]] == 0} {
	    unset ${base}($v:children);
	}
    }

    if {![info exists ${base}($v:open)]} {
	set ${base}($v:open) 0;
    }

    set ${base}($v:image) {};
    set ${base}($v:typ) [get-typ $base];
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::createitem - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : create a new item and reconfigure its parent node
# ---------------------------------------------------------------------------
proc rtl_tree::createitem {base v} {
    variable var; variable $base;
    variable w;

    set c    $base$w(2);
    set vp   [filedirname $v];
    set vn   [filetail $v];
 
    if {$vp != {}} {
	if {![info exists ${base}($vp:children)]} {

	    set vgf   [filedirname $vp];
	    set gfTag item_[getCurrentTag $base $vgf];

	    if {![info exists ${base}($vp:open)]} {
		set ${base}($vp:open) 1;
	    }

	    if {[set ${base}($vp:open)]} {
		set img rtl_tree:openbm;
	    } else {
		set img rtl_tree:closedbm;
	    }

	    # Search for open/close node and remove em
	    if {[info exists ${base}($vp:nb)]} {
		set nb [set ${base}($vp:nb)];
		set j [$c create image [set ${base}($vp:x)]\
			[set ${base}($vp:y)]\
			-anchor center\
			-image $img\
			-tags [list node__$nb __$nb $gfTag]];
		$c bind $j <1> [list rtl_tree::tswitch $base $vp];
	    }

	    set ${base}($vp:children) [list $vn];	    

	} elseif {[lsearch [set ${base}($vp:children)] $vn] < 0} {
	    # previsious not registered, initial vars.
	    lappend ${base}($vp:children) $vn;
	}

	inititem $base $v;
    }
    return $vn;
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::unmap - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : hide the given node v (this methode is called via delete).
# ---------------------------------------------------------------------------
proc rtl_tree::unmap {base v} {
    variable var;
    variable $base;
    variable w;

    if {[info exists ${base}($v:open)] && [set ${base}($v:open)]} {
	unmapChildren $base $v;
    }

    if {[info exists ${base}($v:nb)]} {
	set c $base$w(2);

	if {[catch {
	    set nb [set ${base}($v:nb)];
	    $c delete __$nb;
	} msg]} {
	    puts stderr "rtl_tree::unmap $msg";
	}
    }

    if {[info exists ${base}($v:tag)]} {
	if {[catch {
	    set j [set ${base}($v:tag)];
	    unset ${base}(tag:$j);
	} msg]} {
	    puts stderr "rtl_tree::unmap $msg";
	}
    }

    foreach en [list $v:nb $v:tag] {
	unset ${base}($en);
    }
	
    set lid [removefrom $base stream $v];

}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::connect-stream - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : setup whole connection, which are affected past a rebuild
# ---------------------------------------------------------------------------
proc rtl_tree::connect-stream {base stream} {
    variable var;
    variable $base;
    variable w;
    variable prefs;

    set c $base$w(2);
    # Display connection to parent 
    # Note vp and v need not to be a parent child relation.

    foreach {vp v} $stream {
	if {$vp != {}} {

	    set nb       [set ${base}($v:nb)];
	    set x1       [set ${base}($v:x)];
	    set y1       [set ${base}($v:y)];
	    set parentid [set ${base}($vp:tag)];
	    set iTag     item_[getCurrentTag $base [filedirname $v]];
	    set posp     [$c bbox $parentid];
	    set yp       [lindex $posp 3];

	    if {[string compare {} $yp] == 0} {
		error "parent isn't visible but should be connected $vp $v"
	    }

	    incr yp 2;

	    set con [$c create line $x1 $yp $x1 $y1\
		    -fill $prefs(color,fg)\
		    -tags [list __$nb con__$nb $iTag]];
	    set con2 [$c create line $x1 $y1 [expr {$x1 + 5}] $y1\
		    -fill $prefs(color,fg)\
		    -tags [list __$nb $iTag]];

	    $c lower $con;
	    $c lower $con2;
	}
    }
}

proc rtl_tree::reconnect-stream {base stream} {
    variable var;
    variable $base;
    variable w;
    variable prefs;

    set c $base$w(2);

    # Display connection to parent 
    
    foreach v $stream {
	set vp [filedirname $v];
	if {$vp != {}} {

	    # Example : hierachy without root node.
	    if {![info exists ${base}($vp:tag)]} break;

	    set nb       [set ${base}($v:nb)];
	    set x1       [set ${base}($v:x)];
	    set y1       [set ${base}($v:y)];
	    set parentid [set ${base}($vp:tag)];
	    set iTag     item_[getCurrentTag $base $vp];
	    set posp     [$c bbox $parentid];
	    set yp       [lindex $posp 3];

	    if {[string compare {} $yp] != 0} {
		incr yp 2;
		
		set con [$c create line $x1 $yp $x1 $y1\
			-fill $prefs(color,fg)\
			-tags [list __$nb con__$nb $iTag]];
		set con2 [$c create line $x1 $y1 [expr {$x1 + 5}] $y1\
			-fill $prefs(color,fg)\
			-tags [list __$nb $iTag]];

		$c lower $con;
		$c lower $con2;

	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::nextpos - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : append informative entries to the given node v
# ---------------------------------------------------------------------------
proc rtl_tree::nextpos {base n {direction y}} {
    variable var; variable $base;
    variable w;

    set c $base$w(2);

    set bbox [$c bbox $n];

    ::switch $direction {
	"x" {
	    set x [lindex $bbox 2];
	    set y [lindex $bbox 1];
	    incr x 5;
	}
	"y" {
	    set x [lindex $bbox 0];
	    set y [lindex $bbox 3];
	    incr y 5;
	}
    }
    return [list $x $y];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::connect - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : connect both nodes vp and, this method is not used
#               anywhere, instead see connect-stream
# ---------------------------------------------------------------------------
proc rtl_tree::connect {base vp v x1 y1} {
    variable var;
    variable $base;
    variable w;
    variable prefs;

    set c $base$w(2);
    # display connection to parent 
    set nb [set ${base}($v:nb)];
    if {$vp != {}} {
	set parentid [set ${base}($vp:tag)];
	set iTag     item_[getCurrentTag $base $vp];
	set posp     [$c bbox $parentid];
	set yp       [lindex $posp 3];

	incr yp 2;

	set con [$c create line $x1 $yp $x1 $y1\
		-fill $prefs(color,fg)\
		-tags [list __$nb con__$nb $iTag]];

	set con2 [$c create line $x1 $y1 [expr {$x1 + 5}] $y1\
		-fill $prefs(color,fg)\
		-tags [list __$nb $iTag]];

	$c lower $con;
	$c lower $con2;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::labelat - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : figure out which node was selected (for select and opn)
# ---------------------------------------------------------------------------
proc rtl_tree::labelat {c x y} {
    variable var;

    set base [c2base $c];
    variable $base;

    set x [$c canvasx $x];
    set y [$c canvasy $y];

    foreach m [$c find overlapping $x $y $x $y] {

	set tags [$c itemcget $m -tags];
	if {[lsearch -exact $tags node_select] > 0} continue;

	if {[info exists ${base}(tag:$m)]} {
#	    puts stderr "labelat found tag $m"
	    return [set ${base}(tag:$m)];
	}
    }

    return "";
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::removefrom - 
# 
# Returns     : lreplace [set ${base}(stream)] $id2 end
# Parameters  : 
# 
# Description : utility used to maintain various lists. Removes en
#               from list elem, which is part of var.
# ---------------------------------------------------------------------------
proc rtl_tree::removefrom {base elem en} {
    variable $base;

    set l   [set ${base}($elem)];
    set lid [lsearch -exact $l $en];
    if {$lid > -1} {
	set ${base}($elem) [lreplace $l $lid $lid];
    }
    return $lid;
}


# ---------------------------------------------------------------------------
# handle selection:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::setselection - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : set selection to item v 
# ---------------------------------------------------------------------------
proc rtl_tree::setselection {c v {multi 0} {movef 1}} {
    variable var;
    variable prefs;

    set base [c2base $c];
    variable $base;

    if {[llength [set ${base}(selection)]] == 0} {
	set ${base}(selection) [list $v];
	rtl_tree::drawselection $base $movef;
    } else {
	if {!$multi &&\
		[string compare $v [lindex [set ${base}(selection)] 0]] != 0} {
	    set ${base}(selection) [list $v];

	} elseif {$multi} {
	    set id [lsearch [set ${base}(selection)] $v];
	    if {$id < 0} {

		if {[get-typedSel $base]} {
		    # ! Ensure to select items of the same type
		    set newType [getItemTyp $base $v];
		    set oriType [getItemTyp $base [lindex [set\
			    ${base}(selection)] 0]];
		    
		    if {[string compare $oriType $newType] != 0} {
			# Denie this item selection ...
			return;
		    }
		}

		lappend ${base}(selection) $v;
	    } elseif {[info exists ${base}($v:tag)]} {
		set ${base}(selection)\
			[lreplace [set ${base}(selection)] $id $id];
	    }
	}
	rtl_tree::drawselection $base $movef;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::getselection c - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : get selection of canvas c
# ---------------------------------------------------------------------------
proc rtl_tree::getselection c {
    variable var;

    set base [c2base $c];
    variable $base;

    return [set ${base}(selection)];
}

# ---------------------------------------------------------------------------
# handle drawing:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::drawselection - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : display complete selected items with highlight colors.
# ---------------------------------------------------------------------------
proc rtl_tree::drawselection {base {movef 1}} {
    variable var; variable $base;
    variable w;
    variable prefs;

    set c $base$w(2);

    $c delete node_select;

    set sf [get-selectforeground $base];
    set sb [get-selectbackground $base];
    
    foreach v [set ${base}(selection)] {

	if {[string length $v]==0} continue;
	if {![info exists ${base}($v:tag)]} continue;

	if {[set ${base}(itemselect)]} {
	    set abox [$c bbox all];
	    set bbox [$c bbox item__[set ${base}($v:nb)]];
	    set sbox [list [lindex $abox 0]\
		    [lindex $bbox 1] [lindex $abox 2] [lindex $bbox 3]];
	} else {
	    set bbox [$c bbox [set ${base}($v:tag)]];
	    set sbox $bbox;
	}

	# Extend selection ...
	foreach {id val} [get-selbbox $base] {
	    set sbox [lreplace $sbox $id $id\
		    [expr {[lindex $sbox $id] + $val}]];
	}

	set item   [set ${base}($v:tag)];
	set coords [$c coords $item];
	set text   [$c itemcget $item -text];
	set tags   [linsert [$c itemcget $item -tags] 0 node_select];

	# let tag being part 0f tags

	foreach type [list current] {
	    set id     [lsearch $tags $type];
	    set tags   [lreplace $tags $id $id];
	}

	if {[llength $bbox] == 4} {
	    set i [$c create rectangle\
		    [lindex $sbox 0] [lindex $sbox 1]\
		    [lindex $sbox 2] [lindex $sbox 3]\
		    -fill $sb\
		    -outline $sb\
		    -tags $tags];

	    $c lower $i;


#	    puts stderr "drawselection tags are : $tags"

	    $c create text [lindex $coords 0] [lindex $coords 1]\
		    -fill $sf\
		    -anchor w\
		    -text $text\
		    -font [get-font $base]\
		    -tags $tags];

	    if {$movef} {

		# Don't scroll if the entire item is visible
		# Use bbox for this ..

		set spos   [$c yview];
		set abox   [$c bbox all];		

		foreach {i val} [get-globalbbox $base] {
		    set abox [lreplace $abox $i $i\
			    [expr {[lindex $abox $i] + $val}]];
		}

		set origin [lindex $abox 1];
		set norm   [expr {[lindex $abox 3] - $origin}];

		set minp   [expr {[lindex $bbox 1] - 1.0 - $origin}];
		set maxp   [expr {[lindex $bbox 3] + 1.0 - $origin}];
		
		set relmin [expr {$minp / $norm}];
		set relmax [expr {$maxp / $norm}];

		if {$relmin < 0.0} {set relmin 0.0}
		if {$relmax > 1.0} {set relmax 1.0}

		set height [expr {([$c canvasy [winfo height $c]]\
			-  [$c canvasy 0]) * 0.5}];
		
		set pos    [expr {[lindex $coords 1] - $origin}];
		set relp   [expr {($pos - $height) / $norm}];

		if {$relp < 0.0} {set relp 0.0}

		if {$relmin < [lindex $spos 0] || $relmax > [lindex $spos 1]} {
		    $c yview moveto $relp;
		} elseif {[lindex $spos 0] == 0 && [lindex $spos 1] == 1} {
		    $c yview moveto 0;
		}

	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::drawfellow - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : newly draw the informations following the selected
#               node v, v is never redrawn 
# ---------------------------------------------------------------------------
proc rtl_tree::drawfellow {base v n nb {iTag {}}} {
    variable var;
    variable $base;
    variable prefs;
    variable w;
    

    set c $base$w(2);

    set height  0;
    set visible [get-visible $base];
    
    foreach i [$c find withtag tag_$nb] {
	$c delete $i;
    }
    
    set cs    [get-columnselect $base];
    set tag_l [list __$nb item__$nb tag tag_$nb $iTag];
    
    foreach item $visible {
	if {[info exists ${base}($v:$item)]} {

	    # Try to display its entry ...
	    set posl [nextpos $base $n x];
	    set xx   [lindex $posl 0];
	    set yy   [lindex $posl 1];
	    
	    if {$cs == 1} {
		set n [$c create text $xx $yy\
			-text [set ${base}($v:$item)]\
			-fill $prefs(color,fg)\
			-font [get-font $base]\
			-anchor nw -tags $tag_l];
		set ${base}(tag:$n) $v;
	    } else {
		set n [$c create text $xx $yy\
			-text [set ${base}($v:$item)]\
			-fill $prefs(color,fg)\
			-font [get-font $base]\
			-anchor nw -tags $tag_l];
	    }

	    # Get itemheight of new entry
	    set bb [$c bbox $n];
	    set bo [lindex $bb 3];
	    set to [lindex $bb 1];
	    set hg [expr {$bo - $to}];

	    if {$hg > $height} {
		set height $hg;
	    }
	}
    }
    return $height;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::draw - 
# 
# Returns     : last y - position
# Parameters  : 
# 
# Description : draw entry v at position x,y if vparent isn't empty
#               the new item will be connected to its parent node. The
#               new node will create all its childnodes using draw.
# ---------------------------------------------------------------------------
proc rtl_tree::draw {base v x y vparent} {
    variable var;
    variable $base;
    variable prefs;
    variable w;

    # All Children should share a item_tag refering to their
    # Parent Node, to improve closeup!
    set iTag item_[getCurrentTag $base $vparent];
    set nb        [get-number $base];

    set-number $base [expr {$nb + 1}];

    set c $base$w(2);

    set inx [get-indentx $base];
    set iny [get-indenty $base];
    
    # store origin for every node (rebuild)
    foreach t [list x y] {
	set ${t}1          [set $t];
	set ${base}($v:$t) [set ${t}];
    }

    # display open/close indicator above connection line
    if {[info exists ${base}($v:children)]} {

	if {[set ${base}($v:open)]} {
	    set img rtl_tree:openbm;
	} else {
	    set img rtl_tree:closedbm;
	}

	set j [$c create image $x $y -anchor center\
		-image $img\
		-tags [list node__$nb __$nb $iTag]];

	$c bind $j <1> [list rtl_tree::tswitch $base $v];
    } else {

	# NF - Allow to insert items ondemand.
	set fcn [get-hasChildrenfcn $base];
	if {[string compare $fcn {}] != 0} {
	    if {[$fcn [unmakename $base $v] [set ${base}($v:typ)]]} {
		# The current node has children but currently nothing
		# gathered inside it, draw a + symbol for it.
		set ${base}($v:open) 0;
		set img rtl_tree:closedbm;
		set j [$c create image $x $y -anchor center\
			-image $img\
			-tags [list node__$nb __$nb $iTag]];

		$c bind $j <1> [list rtl_tree::tswitch $base $v];
	    }
	}
    }


    # Check if there is a filter defined for this node ...
    # Note: filtering isn't activated quite yet!
    set filter [filter $base $v];
    if {![info exists op] && !$filter} {
	return [expr {$y - $iny}];
    }

    # distance from x1 10 point minimum
    incr x 12;
        
    # display icon, aside of icon -conectionline
    if {[info exists ${base}($v:image)] && [set ${base}($v:image)] != {}} {
	set j [$c create image $x $y -image [set ${base}($v:image)]\
		-anchor w -tags [list __$nb Rtl_TreeDrag image__$nb item__$nb tag $iTag]];

	set ${base}($v:tag) $j;
	set ${base}(tag:$j) $v;
	set bb              [$c bbox $j]
	set x               [lindex $bb 2];

	if {[set ${base}(fix)] > 0} {
	    set x1 [lindex $bb 0];
	    set x [expr {$x1 + [set ${base}(fix)]}];
	}
	incr x 2;
    }

    # Display real node, use a defineable label for it
    set vname [filetail $v];
    set show  [get-show $base];

    if [info exists ${base}($v:$show)] {
	set text [::format [get-format $base]\
		[set ${base}($v:$show)]];
    } else {
	set text [::format [get-format $base] $vname];
    }

    # Use the filter version to display this node ...
    # Note: see above!
    if $filter {
	set j [$c create text $x $y\
		-text $text\
		-font [get-font $base]\
		-fill  $prefs(color,fg)\
		-anchor w -tags [list __$nb Rtl_TreeDrag item__$nb $iTag tag]];
    } else {
	set j [$c create text $x $y\
		-text $text\
		-font [get-font $base]\
		-fill  $prefs(color,fg)\
		-anchor w -tags [list __$nb item__$nb tag $iTag]];
    }

    # Stream used to find current item in view.
    lappend ${base}(stream) $v;
 

    set ${base}(tag:$j) $v;
    set ${base}($v:tag) $j;
    set ${base}($v:nb) $nb;
    set  x             [expr {[lindex [$c bbox $j] 2] + 2}];

    # Display node informations
    # Use filter version to calculate itemheight ...
    # Note: see above!
    if $filter {
	# display node informations
	set itemheight [drawfellow $base $v $j $nb $iTag];
    } else {
	set itemheight $iny;
    }

    # Display connection to parent 
    if {$vparent != {} && [get-connect $base]} {

	set parentid [set ${base}($vparent:tag)];
	set posp     [$c bbox $parentid];
	set yp       [lindex $posp 3];
	set xp       [lindex $posp 0];

	incr yp 2;
	incr xp [get-indentx $base];

	set con [$c create line $x1 $yp $x1 $y1\
		-fill $prefs(color,fg)\
		-tags [list __$nb con__$nb $iTag]];

	set con2 [$c create line $x1 $y1 [expr {$x1 + 10}] $y1\
		-fill $prefs(color,fg)\
		-tags [list __$nb $iTag]];

	$c lower $con;
	$c lower $con2;
    }

    # recalculate x indent for next entries
    incr x1 $inx;

    if {$itemheight > $iny} {
	set steps [expr {int($itemheight/$iny)}];
	if {[expr {$itemheight%$iny}] == 0} {
	    incr steps -1;
	}
	incr y1 [expr {$steps * $iny}];
    }

    # display children
    if {[info exists ${base}($v:children)] \
	    && [set ${base}($v:open)]} {

	if {[info exists ${base}($v:openimage)]} {
	    $c itemconfigure image__$nb -image [set ${base}($v:openimage)];
	}

	foreach child [set ${base}($v:children)] {

	    if {[string compare {} $child] == 0} continue;

	    incr y1 $iny;

	    set vc  [v $v $child];
	    if {$vc == $v} continue;
	    
	    set y1 [draw $base $vc $x1 $y1 $v];
	}
    }

    # Return y1-size for last drawm entry
    return $y1;
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::drag-select - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : initialize drag action
# ---------------------------------------------------------------------------
proc rtl_tree::drag-select {c x y rx ry} {
    variable var;

    set base [c2base $c];
    variable $base;

    if {[info exists ${base}(autoscroll)]} {
	unset ${base}(autoscroll);
    }
    set  ${base}(autoscrollable) 1;

    if {[set ${base}(drag)]} {
	set ${base}(motion) 0;
	set vs [labelat $c $x $y];
	if {[lsearch -exact [set ${base}(selection)] $vs] >= 0} {
	    set ${base}(dragselection) [set ${base}(selection)];
	} else {
	    set ${base}(dragselection) $vs;
	}
	set ${base}(point) [list $rx $ry];
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::drag-motion - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : real drag action is going on, determine if drag is started
# ---------------------------------------------------------------------------
proc rtl_tree::drag-motion {c x y rx ry} {
    variable var;
    variable prefs;

    set base [c2base $c];
    variable $base;
    
    if {![info exists ${base}(autoscroll)]} {
	drag-scroll $c;
    }
    if {[set ${base}(drag)]} {
	set lx 5; set ly 5;
	set px [lindex [set ${base}(point)] 0];
	set py [lindex [set ${base}(point)] 1];
	set dx [expr {$rx - $px}];
	set dy [expr {$ry - $py}];
	
	if {![set ${base}(motion)]} {
	    if {$dx > $lx || $lx < -$dx} {
		set ${base}(motion) 1;
	    } elseif {$dy > $ly || $ly < -$dy} {
		set ${base}(motion) 1;
	    } else {
		return;
	    }

	    set sf [get-selectforeground $base];
	    set sb [get-selectbackground $base];

	    set ${base}(dragoldcursor) [$c cget -cursor];
	    #$c configure -cursor hand2;
	    if {[set ${base}(selection)] !=\
		    [set ${base}(dragselection)]} {
		set ${base}(selection) [set ${base}(dragselection)];
		drawselection $base;
	    }
	    set bbox [$c bbox node_select];
	
	    if {[catch {
		set wb [expr {([lindex $bbox 2] - [lindex $bbox 0]) / 2}];
		set hb [expr {([lindex $bbox 3] - [lindex $bbox 1]) / 2}];
	    } msg]} {
		puts stderr $msg;
		return;
	    }
	    set ${base}(dragnode,w) $wb;
	    set ${base}(dragnode,h) $hb;

	    set j [$c create rectangle\
		    [lindex $bbox 0] [lindex $bbox 1]\
		    [lindex $bbox 2] [lindex $bbox 3]\
		    -fill $sb\
		    -outline {}\
		    -stipple [get-dragbitmap $base]\
		    -tags "node__drag"];
	}

	set wb [set ${base}(dragnode,w)];
	set hb [set ${base}(dragnode,h)];
	set xb [$c canvasx $x];
	set yb [$c canvasy $y];

	set l [expr {$xb - $wb}];
	set t [expr {$yb - $hb}];
	set r [expr {$xb + $wb}];
	set u [expr {$yb + $hb}];

	# checkout current selected target ...
	set select [labelat $c $x $y];

	$c delete select__drag;
	if {$select != {}} {
	    set sid [set ${base}($select:tag)];
	    set bbox [$c bbox $sid];
	    set j [$c create rectangle\
		    [lindex $bbox 0] [lindex $bbox 1]\
		    [lindex $bbox 2] [lindex $bbox 3]\
		    -fill maroon\
		    -tags "select__drag"];
	    $c lower $j;
	}

	$c coords node__drag $l $t $r $u;

    }
}

proc rtl_tree::drag-scroll {c {flag 0}} {

    set yr   [winfo rooty $c];
    set hg   [winfo height $c];
    set pos  [winfo pointery $c];
    set posx [winfo pointerx $c];

    set xr   [winfo rootx $c];
    set wd   [winfo width $c];

    set yh   [expr {$yr + $hg}];
    set xw   [expr {$xr + $wd}];

    set base [c2base $c];
    variable $base;


    # scroll near to the corner, but not outside of it!

    if {![info exists ${base}(autoscroll)] || $flag} {

	if {[set ${base}(autoscrollable)]} {
	    set units 0;
	    set yv [$c yview];

	    if {$posx <= $xw && $posx >= $xr} {
		if {$pos >= $yr && [expr {$pos - $yr}] < 10} {

		    # scroll up
		    if {[lindex $yv 0] > 0.0} {
			set units -1;
			set ${base}(autoscroll) 1;
		    }

		} elseif {$pos <= $yh && [expr {$yh - $pos}] < 10} {

		    #scroll down
		    if {[lindex $yv 1] < 1.0} {
			set units 1;
		    }

		    set ${base}(autoscroll) 1;
		} else {
		    unset ${base}(autoscroll);
		}
	    } else {
		unset ${base}(autoscroll);
	    }

	    if {[info exists ${base}(autoscroll)]} {
		$c yview scroll $units units;
		after 150 rtl_tree::drag-scroll $c 1;
	    }
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::drag-release - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : close drag action. Deinitialize drag environment.
#               Call dragfcn when defined.
# ---------------------------------------------------------------------------
proc rtl_tree::drag-release {c x y rx ry} {
    variable var;

    set base [c2base $c];
    variable $base;

    if {[set ${base}(drag)] && [set ${base}(motion)]} {

	unset ${base}(autoscroll);
	set  ${base}(autoscrollable) 0;

	# figure out wich entry may become that dragselection
	set dragv [labelat $c $x $y];
	if {$dragv != {} && [lsearch\
		[set ${base}(dragselection)] $dragv] < 0} {
	
	    # convert internal names to external names
	    # and apply foreach element its typ.
	    set drag [list [unmakename $base $dragv]\
		    [set ${base}($dragv:typ)]];
	    
	    set dragged [list];
	    foreach elem [set ${base}(dragselection)] {
		lappend dragged [list [unmakename $base $elem]\
			    [set ${base}($elem:typ)]];		
	    }

	    # reconstruct cursor
	    set fcn [get-dragfcn $base];
	    if {$fcn != {}} {
		if {[catch {
		    $fcn $base $c $drag $dragged $x $y $rx $rx;
		} msg]} {
		    puts stderr "Error while Drop : $msg";
		} 
	    }
	}
	
	$c delete node__drag;
	$c delete select__drag;
	$c configure -cursor [set ${base}(dragoldcursor)];
	foreach elem [list dragoldcursor dragselection point\
		dragnode,w ,dragnode,h] {
	    catch {unset ${base}($elem);}
	}
    }
}


# ===========================================================================
# callback methodes:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::tswitch - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : used to open/close node v, supress conversion at open/close 
# ---------------------------------------------------------------------------
proc rtl_tree::tswitch {base v} {
    variable var; variable $base;
    if {[set ${base}($v:open)]} {
	close $base $v 0;
    } else {
	open $base $v 0;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::opencallback - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : callback on doubleclick, used to reopen node at x,y. A
#               defined openfcn will be called. 
# ---------------------------------------------------------------------------
proc rtl_tree::opencallback {base x y} {
    variable var; variable $base;
    variable w;
    set c $base$w(2);
    set lbl [rtl_tree::labelat $c $x $y];
    set fcn [get-openfcn $base];
    if {[string compare {} $lbl] != 0} {
	if {[string compare {} [string trim $fcn \{\}]] != 0} {
	    close $base $lbl;
	    $fcn $base [rtl_tree::unmakename $base $lbl]\
		    [set ${base}($lbl:typ)]];
	} 
	open $base $lbl 0;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::browse - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : select callback at x,y. A defined selectfcn will be called.
# ---------------------------------------------------------------------------
proc rtl_tree::browse {base x y {multi 0}} {
    variable var; variable $base;
    variable w;

    set c $base$w(2);
    set lbl [rtl_tree::labelat $c $x $y];

    set ${base}(browse) 1;

    if {[info exists ${base}(motion)] && [set ${base}(motion)]} {

	# Drag state discard any further action is activated.
	# Finish that and discard evaluation browse!
	# Invoked by a node_select item.

	return;
    }

    if {[catch {
	setselection $c $lbl $multi 0;
	update idletasks;
    } msg]} {
	puts stderr "Failed to setselection for $lbl --- $msg"
    }

    if {$lbl != {}} {
	set fcn [get-selectfcn $base];
	if {$fcn != {}} {
	    $fcn $base [rtl_tree::unmakename $base\
		    [set ${base}(selection)]]\
		    [getItemTyp $base [lindex [set ${base}(selection)] end]];
	}
    }
}

proc rtl_tree::opencontext {base x y rx ry} {
    variable var; variable $base;
    variable w;

    set c $base$w(2);
    set lbl [rtl_tree::labelat $c $x $y];

    if {$lbl != {}} {
	set fcn [get-contextfcn $base];
	update idletasks;
	if {$fcn != {}} {
	    $fcn $base [rtl_tree::unmakename $base $lbl] $rx $ry;
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::open - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : open the given node v (if exists) and initiate a
#               redraw call to the given part of the rtl_treeview.
#               open/close can also be called from outside using the
#               conversion facility.
# ---------------------------------------------------------------------------
proc rtl_tree::open {base v {conv 1}} {
    variable var;
    variable $base;
    variable w;

    set c $base$w(2);

    if {$conv} {
	set v [makename $base $v];
    }

    set originalName [unmakename $base $v];

    # Allow to dynamically fill a branch.
    set fcn [get-expandfcn $base];
    if {[string compare $fcn {}] != 0} {
	$fcn $base $originalName [set ${base}($v:typ)];
    } elseif {![info exists ${base}($v:children)]} {

	# Currently there are no children available for that node
	# go call try to load them ondemand.
	set fcn [get-requestChildrenfcn $base];
	if {[string compare $fcn {}] != 0} {
	    $fcn $base $originalName [set ${base}($v:typ)];
	}
    }


    if {[info exists ${base}($v:children)]} {

	# Allow to open invisible items without
	# raising an exception ...

	if {[info exists ${base}($v:nb)]} {
	    set nb [set ${base}($v:nb)];
	    if {[info exists ${base}($v:openimage)]} {
		$c itemconfigure image__$nb -image [set ${base}($v:openimage)];
	    }

	    # Use new Current Tag ...
	    set vp [filedirname $v];
	    set iTag item_[getCurrentTag $base $vp];
	    set item_node [getCurrentTag $base $v];

	    # Item is visible ...
	    set node [$c find withtag node__$item_node];

	    if {$node == {}} {
		set j [$c create image [set ${base}($v:x)]\
			[set ${base}($v:y)]\
			-image rtl_tree:openbm -anchor center\
			-tags [list node__$item_node\
			__$item_node $iTag]];
		$c bind $j <1> [list rtl_tree::tswitch $base $v];
	    } else {
		$c itemconfigure $node -image rtl_tree:openbm;
	    }
	}

	if {![info exists ${base}(${v}:open)] ||\
		![set ${base}(${v}:open)]} {

	    set ${base}(insert) {};
	    foreach child [set ${base}($v:children)] {
		if {[string compare {} $child] != 0} {
		    lappend ${base}(insert) [v $v $child];
		}
	    }
	    set ${base}(${v}:open) 1;

	    # Don`t scroll after rebuild ...
	    rebuild $base $v 0;

	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::close - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : close the given node v. This is the opposit methode to
#               the open methode described above.
# ---------------------------------------------------------------------------
proc rtl_tree::close {base v {conv 1}} {
    variable var; 
    variable $base;
    variable w;

    set c $base$w(2);

    if {$conv} {
	set v [makename $base $v];
    } else {
	set v $v;
    }

    if {[info exists ${base}($v:children)]} {
	if {![info exists  ${base}($v:nb)]} {
	    #set ${base}(${v}:open) 0;
	    return;
	}
	set nb [set ${base}($v:nb)];
	set node [$c find withtag node__$nb];

	if {$node == {}} {

 	    set vp   [filedirname $v];
 	    set iTag item_[getCurrentTag $base $vp];

 	    $c create image [set ${base}($v:x)] [set ${base}($v:y)]\
 		    -image rtl_tree:closedbm -anchor center\
 		    -tags [list node__[set ${base}($v:nb)]\
 		    __[set ${base}($v:nb) $iTag]];
	} else {
	    $c itemconfigure $node -image rtl_tree:closedbm;
	}

	# Check if there are any children underneath that node
	if {[info exists ${base}(${v}:open)] &&\
		[set ${base}(${v}:open)]} {

	    # Restore an openimage
	    if {[info exists ${base}($v:openimage)]} {
		$c itemconfigure image__$nb -image [set ${base}($v:image)];
	    }

	    unmapChildren $base $v;
	    set ${base}(${v}:open) 0;
	    
	    # Don`t scroll after rebuild ...
	    rebuild $base $v 0;
	}
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::stillInsert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Same as insert but without a rebuild/build call
#               the sense is to initialize a complete tree and after
#               this call build explicit.
# ---------------------------------------------------------------------------
proc rtl_tree::stillInsert {base v args} {
    variable var; variable $base;

    set v [makename $base $v];
    set flag [info exists ${base}($v:typ)];

    createitem $base $v;
    foreach {op arg} $args {
	set ${base}($v:[string trim $op -]) $arg;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::insert - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : insert a new node int the rtl_tree view and initiate a
#               rebuild action
#               New item attributes -after -before and -index
#               Introduced in Patchlevel 1
# ---------------------------------------------------------------------------
proc rtl_tree::insert {base v args} {
    variable var; 
    variable $base;

    set v     [makename $base $v];
    set flag  [info exists ${base}($v:typ)];
    set vp    [filedirname $v];
    set vname [filetail $v];

    # search in args for special attributes like -after, -before
    #-index
    foreach speAttr [list after before index] {
	set id [lsearch -exact $args -$speAttr];
	if {$id > -1} {
	    if {!$flag} {

		# found specialAttribute and this is a new item.
		
		[set ${base}($vp:children)]
		set insert [lindex $args [expr {$id + 1}]];
		switch $speAttr {
		    "index" {
			set insPos $insert;
		    }
		    "after" {
			set insPos [lsearch [set\
				${base}($vp:children)] $insert];
		    }
		    "before" {
			set insPos [lsearch [set\
				${base}($vp:children)] $insert];
			if {$insPos > -1} {
			    incr insPos -1;
			}
		    }
		}
	    }
	    set args   [lreplace $args $id [expr {$id + 1}]];
	    break;
	}
    }

    createitem $base $v;
    foreach {op arg} $args {
	set ${base}($v:[string trim $op -]) $arg;
    }

    
    # open grandfather node
    set pa_list [lreplace [split [filedirname $vp] /] 0 0];
  
    set name {};
    if {[get-root $base]} {
	if {![set ${base}(/:open)]} {open $base /}
    }

    foreach pa $pa_list {
	set name $name/$pa;
	if {![set ${base}($name:open)]} {
	    open $base $name;
	}
    }
    if {!$flag} {

	set ${base}(insert) [list $v];
	if {[info exists insPos]} {
	    set id $insPos;
	} else {
	    set id [lsearch -exact [set ${base}($vp:children)] $vname];
	    incr id -1;
	}


	if {$id >= 0} {
	    
	    set vprev [v $vp [lindex [set ${base}($vp:children)] $id]];
	    while [set ${base}($vprev:open)] {
		if {![info exists ${base}($vprev:children)]} break;
		set id [llength [set ${base}($vprev:children)]];
		incr id -1;
		if {$id < 0} {break}
		set vprev [v $vprev\
			[lindex [set ${base}($vprev:children)] $id]];
	    }

	    # Invisible or visible rebuild ...
	    rebuild $base $vprev;

 	}
    }

    # Previsious parent node was closed, open it ...
    if {![set ${base}($vp:open)]} {
	open $base $vp;
    }
}

proc rtl_tree::unmapChildren {base v} {
    variable $base;
    variable w;
    set c $base$w(2);

    set iTag item_[getCurrentTag $base $v];

    if {[string compare item_glymna $iTag] == 0} {
	return;
    }

    # Call delete Children for each children and
    # remove this item from the streams.
    if {[info exists ${base}($v:children)]} {
	foreach child [set ${base}($v:children)] {
	    set vc [v $v $child];
	    if {[string compare $vc $v] != 0\
		    && [info exists ${base}($vc:open)]\
		    && [set ${base}($vc:open)]} {
		unmapChildren $base $vc;
	    }

	    if {[info exists ${base}($vc:tag)]} {
		# Unset tags for particular child ...
		set j [set ${base}($vc:tag)];
		unset ${base}(tag:$j);
	    }

	    foreach en [list $vc:nb $vc:tag] {
		unset ${base}($en);
	    }

	    set lid [removefrom $base stream $vc];

	}
    }

    # Delete All Items ..
    $c delete $iTag;
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::delete - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : remove a given node complete with its childs from the
#               view. Initialize a rebuild when delete was called from
#               outside.
# ---------------------------------------------------------------------------
proc rtl_tree::delete {base v {rebuild 1} {conv 1}} {
    variable var;
    variable $base;
    variable w;

    set c    $base$w(2);

    if {$conv} {
	set v [makename $base $v];
    }

    if {$v == "/"} {
	$c delete all;
	foreach elem [array names $base /*] {
	    unset ${base}($elem);
	}
    } else {

	# Search the element directly before this item.
	set lid  [lsearch [set ${base}(stream)] $v];
	if {$lid > 0} {incr lid -1}

	set vpre    [lindex [set ${base}(stream)] $lid];
	set vpa     [filedirname $v];
	set vname   [filetail $v];

	# Remove deleted item from the parent list
	# Note the parent must have a children list.
	set id [lsearch [set ${base}($vpa:children)] $vname];
	if {$id >= 0} {
	    set ${base}($vpa:children)\
		    [lreplace [set ${base}($vpa:children)]\
		    $id $id];
	    
	    if {[llength [set ${base}($vpa:children)]] == 0} {

		# search for open/close node and remove em
		if {[info exists ${base}($vpa:nb)]} {
		    set nb [set ${base}($vpa:nb)];
		    $c     delete node__$nb;
		}

		unset  ${base}($vpa:children);
	    }
	}

	# Destroy data and visible items
	if {[info exists ${base}($v:nb)]} {
	    set last_id [deleteItem $base $v];
	    rebuild $base $vpre;
	}
    }

}

# ===========================================================================
# public methodes:
#                 open         - internal and public
#                 close        - internal and public
#                 delete       - internal and public
#                 insert       - public
#                 makename     - internal and public
#                 umakename    - internal and public
#                 redrawfellow - public
# ---------------------------------------------------------------------------
# Function    : rtl_tree::redrawfellow - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : This methode is used to reconfigure the given
#               information wich are stored as subpart to node v.
# ---------------------------------------------------------------------------
proc rtl_tree::redrawfellow {base v args} {
    variable var; variable $base;
    variable w;

    set c    $base$w(2);
    set v    [makename $base $v];
    set vp   [filedirname $v];
    set iTag [getCurrentTag $base $vp];

    foreach {op arg} $args {
	set ${base}($v:[string trim $op -]) $arg;
    }

    if {[info exists ${base}($v:tag)]} {
	set j [set ${base}($v:tag)];
	set nb [set ${base}($v:nb)];

	# display in rtl_treeview
	drawfellow $base $v $j $nb $iTag;
    }
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::makename - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : convert value into internal representation
# ---------------------------------------------------------------------------
proc rtl_tree::makename {base value} {
    variable var; variable $base;
    set nl [split $value [set ${base}(char)]];
    set root {};
    return [join $nl /];
}

# ---------------------------------------------------------------------------
# Function    : rtl_tree::unmakename - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : reconveret internal name into real name
# ---------------------------------------------------------------------------
proc rtl_tree::unmakename {base value} {
    variable var; variable $base;
    set nl [split $value /];
    set root {};
    return [join $nl [set ${base}(char)]];
}


# ===========================================================================
# option methodes:
# ---------------------------------------------------------------------------
# Function    : rtl_tree::set-select + get-select 
# 
# Returns     : 
# Parameters  : 
# 
# Description : -select option. Set get current selections.
# ---------------------------------------------------------------------------
proc rtl_tree::set-select {base value} {
    variable w;
    variable $base;
    set ${base}(selection) [list];
    foreach item $value {
	lappend ${base}(selection) [makename $base $item]
    }
    rtl_tree::drawselection $base;
}

proc rtl_tree::get-select {base} {
    variable $base;

    set sel [list];
    foreach item [set ${base}(selection)] {
	lappend sel [unmakename $base $item];
    }
    return $sel;
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::set-number - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : internal used tag index for stream elements
# ---------------------------------------------------------------------------
proc rtl_tree::set-number {base value} {
    return [set-action $base number $value];
}

proc rtl_tree::get-number {base} {
    set number [get-action $base number];
    if {$number == {}} {
	set number 0;
    }
    return $number;
}


# ===========================================================================
# --------------------------------------------------------------------------
# new member methodes:
# --------------------------------------------------------------------------
# Description : Replace the complete content of this tree.
#
# --------------------------------------------------------------------------


proc rtl_tree::getCurrentTag {base v} {
    variable $base;
    set tag {glymna};
    if {[info exists ${base}($v:nb)]} {
	set tag [set ${base}($v:nb)];
    }
    return $tag;

}


proc rtl_tree::deleteItem {base v} {
    variable var;
    variable $base;
    variable w;

    set c       $base$w(2);

    if {[info exists ${base}($v:nb)]} {
	set nb      [set ${base}($v:nb)];
	$c delete __$nb;
	$c delete item_[getCurrentTag $base $v];

    }

    set last_id [removefrom $base stream $v];
    set sel_id  [removefrom $base selection $v];

    if {[info exists ${base}($v:tag)]} {
	set j [set ${base}($v:tag)];
	unset ${base}(tag:$j);
    }

    # Also move all children of this particular item ...
    if {[info exists ${base}($v:children)]} {
	foreach child [set ${base}($v:children)] {
	    deleteItem $base /[string trim $v/$child /];
	}
    }

    foreach elem [array names $base $v:*] {
	unset ${base}($elem);
    }

    return $last_id;
}


proc rtl_tree::replace {base args} {
    return [replaceUseVar $base args];
}

proc rtl_tree::replaceUseVar {base fromVar} {
    upvar $fromVar args;
    variable var; 
    variable $base;
    variable w;

    set c $base$w(2);

    # try to allow partial replace ...
    set id_stream 0;
    set rebuild   0;
    
    if {[lindex $args 0] == "-at"} {
	set rebuild   1;
	set itemr     [lindex $args 1];
	set vr        [makename $base [lindex $itemr 0]];
	set args      [lreplace $args 0 1];
	set id_stream [lsearch -exact [set ${base}(stream)] $vr];

	if {$id_stream > 0} {
	    incr id_stream -1;
	} else {
	    
	    # Disable rebuild and use build instead.
	    # The root will be reinsterd into to args
	    # list.

	    set args [linsert $args 0 $itemr];
	    set rebuild 0;
	}

    } else {
	# Remove entire tree and rebuild from scratch ...
	set vr /;
    }

    # tag should be deleted for selected nodes and
    # not for each note (this should also happen
    # from within the delete call ...
    foreach name [list pending] {
	foreach elem [array names $base $name*] {
	    unset ${base}($elem);
	}
    }

    #set ${base}(selection) {};
    set ${base}(selidx) {};

    # Make a Backup for all open items ...
    array set bkp_open [array get ${base} *:open];
 
    if {$rebuild} {


	# locate vr in stream ...
	$c delete node_select;
	
	set level [llength [split $vr /]];
	
	foreach item $args {
	    set name [lindex $item 0];
	    set v    [makename $base $name];
	    if {[llength [split $v /]] == $level} {
		deleteItem $base $v;
		#puts stderr "Killed item $v"

	    }
	}

	deleteItem $base $vr
	
	# Restore backup (the open flags) ...
	array set ${base} [array get bkp_open *:open];

	createitem $base $vr;

	unset      attrl;
	array set attrl [lreplace $itemr 0 0];
	foreach op [array names attrl] {
	    set ${base}($vr:[string trim $op -]) $attrl($op);
	}

	# Don add anything to the
	# insert stream if the parent isn't visible and open!
	set vp   [filedirname $vr];
	set ${base}(insert) [list];
	set flag 0;

	if {[info exists ${base}($vp:nb)] && [set ${base}($vp:open)]} {
	    set ${base}(insert) [list $vr];
	    set ${base}($vr:open) 1;
	    set flag 1;
	} elseif {![get-root $base] && [string compare / $vp] == 0} {
	    # vr is inserted below the root and
	    # visible ...
	    set ${base}(insert) [list $vr];
	    set ${base}($vr:open) 1;
	    set flag 1;
	}
   

	# It is possible that vr is a new parent to v and
	# not open!
	# Recover data ...
	foreach item $args {
	    set name [lindex $item 0];
	    set v    [makename $base $name];
	    
	    createitem $base $v;

	    set vpar [filedirname $v];
	    if {[set ${base}($vpar:open)] && $flag} {
		lappend ${base}(insert) $v;
	    }
	    
	    
	    set argl [lreplace $item 0 0];
	    foreach {op arg} $argl {
		set ${base}($v:[string trim $op -]) $arg;
	    }
	}
	
	# locate vr in stream ...
	set vprev [lindex [set ${base}(stream)] $id_stream];
	if {[string compare / $vprev] == 0} {
	    $c delete all;
	    build $base;
	} else {
	    if {[llength [set ${base}(insert)]] > 0} {
		rebuild $base $vr;
    }
	}
    } else {

	foreach exp [list /* tag:* stream insert] {
	    foreach arv [array names ${base} $exp] {
		unset ${base}($arv);
	    }
	}

	# Gather complete new data ...
	foreach item $args {
	    set name [lindex $item 0];
	    set v    [makename $base $name];
	    createitem $base $v;

	    # Setup node data
	    set argl [lreplace $item 0 0];
	    foreach {op arg} $argl {
		set ${base}($v:[string trim $op -]) $arg;
	    }
	}

	# Restore backup (the open flags) ...
	array set ${base} [array get bkp_open *:open];

	# display current data ...
	$c delete all;
	build $base;
    }
    
    # This is already done in rebuild (redundant):
    unset ${base}(insert);
}

proc rtl_tree::moveKey {base {key up}} {
    variable $base;

    set current [lindex [set ${base}(selection)] end];
    set last_id [lsearch -exact [set ${base}(stream)] $current];

    if {$last_id > -1} {
	switch $key {
	    "Up" {
		incr last_id -1;
	    }
	    "Down" {
		incr last_id 1;
	    }

	    "Right" {
		set name [filetail $current];
		set vp [filedirname $current];
		set id [lsearch [set ${base}($vp:children)] $name];
		if {[incr id] < [llength [set ${base}($vp:children)]]} {
		    set vs /[string trim $vp/[lindex [set ${base}($vp:children)] $id] /];
		    set last_id [lsearch -exact [set ${base}(stream)] $vs];
		}
	    }
	    "Left" {
		set vp [filedirname $current];
		set last_id [lsearch  -exact [set ${base}(stream)] $vp];
	    }
	    "plus" {
		open $base $current;
	    }
	    "minus" {
		close $base $current;
	    }
	    "Return" {
		if {[set ${base}($current:open)]} {
		    close $base $current;
		} else {
		    open $base $current;
		}
	    }
	    "Home" {
		set last_id 0;
	    }
	    "End" {
		set last_id [expr {[llength [set ${base}(stream)]] - 1}];
	    }
	    default {
		#puts stderr $key
		return;
	    }
	}
    }
    if {$last_id > -1} {

	if {$last_id < [llength [set ${base}(stream)]]} {
	    set v [lindex [set ${base}(stream)] $last_id];
	    set-select $base [unmakename $base $v];

	    set fcn [get-selectfcn $base];
	    if {$fcn != {}} {
		$fcn $base [rtl_tree::unmakename $base\
			[set ${base}(selection)]]\
			[set ${base}($v:typ)];
	    }
	}
    }
}

proc rtl_tree::setfocus {base} {
    set foc [focus];
    if {[string first $base $foc] < 0} {
	focus $base.gridwin.canvas;
    }
}


# ---------------------------------------------------------------------------
# Function    : rtl_tree::getItemTyp - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Deliever information what itemtype was used with this
#               node;
# ---------------------------------------------------------------------------
proc rtl_tree::getItemTyp {base v} {
    variable $base;
    set v [makename $base $v];

    return [set ${base}($v:typ)];
}
# --------------------------------------------------------------------------
# make option pairs:

create-action rtl_tree {visible {label text title class geometry manager}}\
	typ {font {Helvetica 11 bold}} {indentx 20} {indenty 20} \
	{char /} contextfcn {fix 0}\
	{itemselect 0} {columnselect 0} show {selectforeground white}\
	{selectbackground blue2} format {root 1} {connect 1} selectfcn\
	expandfcn openfcn {drag 1} dragfcn dragbitmap\
	\
	hasChildrenfcn requestChildrenfcn\
	{startX 0} {startY 0} {typedSel 0}\
	\
	[list globalbbox [list 0 -4 1 -4 2 4 3 4]]\
	[list selbbox [list 0 -2 1 -2 2 2 3 2]];




#  # ===========================================================================
#  # Bounding Box selection.
#  # The BB Sel is currently deactivated, since
#  # will conflict with the normal select methods.
#  # ---------------------------------------------------------------------------
#  # Function    : rtl_tree::startSelection - 
#  # 
#  # Returns     : 
#  # Parameters  : 
#  # 
#  # Description : Begin a bounding box selection
#  # ---------------------------------------------------------------------------
#  proc rtl_tree::startSelection {base x y {fl 0}} {
#      variable w;
#      variable var;

#      set c $base$w(2);

#      set cx [$c canvasx $x];
#      set cy [$c canvasy $y];

#      set-startX $base $cx;
#      set-startY $base $cy;
#  }

#  # ---------------------------------------------------------------------------
#  # Function    : rtl_tree::motionSelection - 
#  # 
#  # Returns     : 
#  # Parameters  : 
#  # 
#  # Description : draw the selection bounding box
#  # ---------------------------------------------------------------------------
#  proc rtl_tree::motionSelection {base x y} {
#      variable w;
#      variable var;
#      variable $base;

#      if {[info exists ${base}(motion)] && [set ${base}(motion)]} {
#  	return;
#      }

#      if {[info exists ${base}(browse)] && [set ${base}(browse)]} {
#  	unset ${base}(browse);
#  	return;
#      }


#      set c $base$w(2);
    
#      set cx [$c canvasx $x];
#      set cy [$c canvasy $y];
    
#      set startX [get-startX $base];
#      set startY [get-startY $base];
#      set sbg    [get-selectbackground $base];
    
#      $c delete Invisible;
#      $c create rectangle $startX $startY $cx $cy -outline $sbg\
#  	    -width 2 -tags Invisible;

#  }
#  # ---------------------------------------------------------------------------
#  # Function    : rtl_tree::releaseSelection - 
#  # 
#  # Returns     : 
#  # Parameters  : 
#  # 
#  # Description : Finish selection via 
#  # ---------------------------------------------------------------------------
#  proc rtl_tree::releaseSelection {base x y} {
#      variable w;
#      variable var;
#      variable $base;

#      if {[info exists ${base}(motion)] && [set ${base}(motion)]} {
#  	return;
#      }
    
#      if {[info exists ${base}(browse)] && [set ${base}(browse)]} {
#  	return;
#      }

#      set c $base$w(2);
#      $c delete Invisible;
#      set cx [$c canvasx $x];
#      set cy [$c canvasy $y];
    
#      set startX [get-startX $base];
#      set startY [get-startY $base];
    
#      set selection [list];
    
#      foreach item [$c find enclosed $startX $startY $cx $cy] {
#  	set tags [$c itemcget $item -tags];
	
	
#  	if {[lsearch -exact $tags tag] > -1} {
#  	    if {[info exists ${base}(tag:${item})]} {
#  		set v [set ${base}(tag:${item})];
#  		if {[lsearch -exact $selection $v] < 0} {
#  		    lappend selection $v;
#  		}
#  	    }
#  	}
#      }
    
    
    
#      # Handle the nodes inside the new selection ...
#      set ${base}(selection) $selection;
    
#      drawselection $base;
#      set fcn [get-selectfcn $base];
#      if {$fcn != {}} {
#  	set seltype [get-typ $base];
#  	if {[info exists ${base}([lindex $selection end]:typ)]} {
#  	    set seltype [set ${base}([lindex $selection end]:typ)];
#  	}
	
#  	$fcn $base [rtl_tree::unmakename $base\
#  		[set ${base}(selection)]]\
#  		$seltype;
#      }
#  }

# --------------------------------------------------------------------------
# NOTE:
# --------------------------------------------------------------------------
#
# The rebuild process
# -------------------
#
# The rtl_tree component provides two building processes, build and
# rebuild. Rebuild was developed as an respond to the fact that
# redrawing the graphical items increases exponential with there
# number. Rebuild is splitted into three parts. 1: there are entries,
# which are not affected by the last drawing. 2: There are many items
# following the point which has been changed. 3. There are points delete or
# newly created which must initial drawn.
# 
# How to handle these 3 parts?
#
# The non affected graphical items are not touched (no redraw is
# necessary).
# The second part is more difficult to handle, you must determine this
# part in two contradict direction; when you want to delete items,
# each item and its ascendant items are directly destroyed befor
# rebuild is called. Whereas when you display new items rebuild is
# required to display them.
# The third part is also splitted into two needed action first 
# the complete graphical items must be moved to the location which is
# produced by action 2. Each connection with the first part must be
# deleted and redrawn.
#
# How to determin between this parts ?
#
# Each instance stores the information about its items into a list
# named stream. Provide the last non affected item, out of stream,
# when calling rebuild. The list stream must be valid when calling
# rebuild, this means you must have removed each delete graphic from
# stream before invoking rebuild. Rebuild itself must calculate the
# new space from part 2 to determine how much part 3 must be
# moved. After this, the needed connection are reestablished.
#
# The space from part 2 and 3 are calculated in one single direction
# (the growth direction). Exampel: when you have delete 1 item with 10
# subitems (11 items together) the space between vp1 and vp3 mesures
# 11 x indenty, rebuild must reduce this space to 1 x indent. So this
# space is 10 x indenty and 0 x indentx.
#  
# --------------------------------------------------------------------------
# Performance:
#
# With minor release 1.1 new performance optimizations are applied to
# the tree widget. The formal rebuild process isn't changed but the
# determintation for 1,2 and 3 has been optimized. In addition also
# the delete move actions are now controlled by the deleted or moved
# parent node (child nodes do have a children tag which refers to the
# parent node and move and delete operations are working wit this
# tag).
# 
# --------------------------------------------------------------------------
# Roger, 21.08.98 - finished
# New Feature: 10.5.99 - extension required for gdraw (item
# atribute:show)
# 
# --------------------------------------------------------------------------
# Callbacks:
# ---------------------------------------------------------------------------
# The Runtime Library Tree widget has a growing list of callback
# attributes, most callbacks are used to inform/ gather data for a
# tree widget, the remaining callbacks are used for interaction
# purposes.
#
# Gathering Data:
# ---------------
# expandfcn <tree> <node> <typeOf node>
#
# This callback will be called each time a given node is openend.
# expandfcn is a unconditional call. The call to expandfcn will be
# invoked wether there are children to node or not.
#
# requestChildrenfcn <tree> <node> <typeOf node>
#
# Similar to expandfcn. In contrast to expandfcn a requestChildrenfcn
# call is surpressed if there are any children available for the tree.
# 
# the callback is responsable to insert the requested children into
# the given tree widget. Since the current call is invoked from a
# started open callback, use the stillInsert action to bring the data
# to the tree widget (don't use replace / rebuild or similar
# actions!).
#
# The expandfcn callback is prior to the requestChildrenfcn callback.
#
# hasChildrenfcn <node> <typeOf node>
#
# The hasChildrenfn will be called foreach node without actual
# childrens during drawing this item. The caller must return 1 (true)
# to tell that this node has children which will be inserted ondemand
# if the node is opend.
# Typicallly hasChildrenfcn is used in combination with expandfcn
# and/or requestChildrenfcn.
#
# hasChildrenfcn is the only callback which has to deliever something
# back to tree widget (0 or 1 for false and true).
#
# Interaction Callbacks:
# ----------------------
#
# openfcn <tree> <node> <typeOf node>
#
# The openfcn callback is called to inform the application before
# invoking the open action. Openfcn is called imediatly before
# expandfcn and requestChildrenfcn. Instead of the later two only user
# interactions are reported due the openfcn call.
#
# dragfcn <tree> <canvas widget of tree>\
#	<<target node> <typeOf target>>\
#       <<<dragged node1> <typeOf dragged node1>> <...>>\
#       <relative x position> <relative y position>\
#       <absolute x position> <absolute y position>
#
# Called during dropping multiple nodes onto other nodes in the same
# tree. The signature receives the single target node and a list of
# dragged nodes (together with their types).
# 
# selectfcn <tree> <selected nodes> <typeOf last selected node>
#
# Called during the selection of a node, the selected nodes are
# retrieveable using the select property of the tree widget and the
# delieverd type reflects the type of the last selected node. Type may
# not equal for all selected items.
#
# contextfcn <tree> <node below pointer> <absolute x> <absolute y>
#
# The contextfcn is called for a <<Context>> event. You have to add a
# proper event for the virtual <<Context>> event, to take benefit from
# it. The node for a context event doesn't need to be part of the
# current selection; use the select property, if you want to display a
# context menu for the selected nodes.
  
# ===========================================================================
# Tasks:
#
# ---------------------------------------------------------------------------






