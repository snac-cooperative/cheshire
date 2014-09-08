# -*-tcl-*-
# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1999 Patzschke + Rasp Software AG
# 
# Description: Runtime Library sample file, this file was used
#              to create the example in the Runtime Library
#              User's Guide.
# -----------------------------------------------------------------------
# ---------------------------------------------------------------------------
# Function    : rtl_message - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Default message handler
# ---------------------------------------------------------------------------
proc rtl_message {argl} {
    set cmd {};
    set cmd_ag {};
    set id [lsearch -exact $argl "-format"];
    if {$id >= 0} {
	set str_ag [lrange $argl 0 $id];
	set cmd "::format";
	set cmd_ag [lrange $argl [incr id] end];
    } else {
	set id [lsearch -exact $argl "-command"];
	if {$id >= 0} {
	    set str_ag [lrange $argl 0 $id];
	    set cmd [lindex $argl [incr id]];
	    set cmd_ag [lrange $argl [incr id] end];
	} else {
	    set str_ag $argl;
	}
    }

    set str_ [join $str_ag \ ];
    return [list $str_ $cmd $cmd_ag];

}

# ---------------------------------------------------------------------------
# Function    : ! - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Prints messages to stdout using the default
#               rtl_message handler
# ---------------------------------------------------------------------------
proc ! {args} {
    
    foreach {str_ cmd cmd_ag} [rtl_message $args] {}

    if {[string compare {} $cmd] != 0} {
	set str_ [::eval {$cmd $str_} $cmd_ag];
    }
 
    puts stdout $str_;
}

# ---------------------------------------------------------------------------
# Function    : !! - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Prints messages to stdout/stderr using the default
#               rtl_message handler 
# ---------------------------------------------------------------------------
proc !! {args} {

    foreach {str_ cmd cmd_ag} [rtl_message $args] {}

    if {[string compare {} $cmd] != 0} {
	set str_ [::eval {$cmd $str_} $cmd_ag];
    }

    puts stdout "\[error\] $str_";
    puts stderr "\[error\] $str_";

}

# ---------------------------------------------------------------------------
# Function    : !!! - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Print messages to stdout and stderr, the specified
#               Message is treated as an debug message.
# ---------------------------------------------------------------------------
proc !!! {args} {

    foreach {str_ cmd cmd_ag} [rtl_message $args] {}

    if {[string compare {} $cmd] != 0} {
	set str_ [::eval [list $cmd $str_] $cmd_ag];
    }

    puts stdout "\[debug\] $str_";

}

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Extend the auto_path with the rtl base folder;
!

set auto_path [linsert $auto_path 0 [file dirname [info script]]];

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! *** Load Images, which are used by this sample file;
! *** source [file join [file dirname [info script]] sampleimages.tcl];
!

source [file join [file dirname [info script]] sampleimages.tcl];

! ===========================================================================
! Start Building Sample Suite
! ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! First load all basic packages rtl + rtl_compat;
!

package require rtl;
package require rtl_compat;

#option add *selectBackground white;

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Show unconfigured widgets;
!

foreach wg [list\
	makeFont shell chooseFont combobox spinbox] {

    set pkg [string tolower rtl_$wg];
    package require $pkg;


    ! ---------------------------------------------------------------------------
    ! Sample widget [mkFirstUp $wg];
    !
    ! --> rtl_${wg} .${wg} -creator toplevel;
    !     Test
    
    rtl_${wg} .${wg} -creator toplevel;
    wm title .${wg} [mkFirstUp $wg];
}

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Preferences Dialog;
!

package require rtl_preferences;

! --> Design Data for Preferences Dialog;
!

array set qq [list aa,1 1 aa,2 2 aa,3 3 aa,4 4 aa,1,1 1 aa,1,2 2\
aa,1,3 3  aa,1,4 4 aa,2,1 1 aa,2,2 2 aa,2,3 3 aa,2,4 4 aa,3,1 1 aa,3,2\
2 aa,3,3 3 aa,3,4 4 aa,4,1 1 aa,4,2 2 aa,4,3 3 aa,4,4 4 aa,4,1,1 1\
aa,4,1,2 2 aa,4,1,3 3 aa,4,1,4 4];

! --> Create Preference Dialog and fill em with data;
!

preferences .pd -variable ::qq -root aa -treewidth 256\
	    -creator toplevel;
.pd update;

wm title .pd "Preferences Dialog";


# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Gridwin;
!

package require rtl_gridwin;

rtl_gridwin .gw -widget .gw.child -creator toplevel;

! --> Use a large canvas widget as the child of that gridwin widget;
!

canvas .gw.child -bd 0 -background [.gw cget -background] -scrollregion {0 0 2000 2000}\
	-highlightthickness 0;

! --> Call update to make the child widget visible;
!

.gw update;

wm title .gw "Gridwin";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Scrollwin;
!
! This Scrollwin widget is managed by a Gridwin widget;
!

package require rtl_scrollwin;
package require rtl_gridwin;

rtl_gridwin .gs -widget .gs.sw -creator toplevel;
rtl_scrollwin .gs.sw -widget .gs.sw.child;

! --> Create a sample child for this scrollwin widget.

frame .gs.sw.child -bg white -width 1000 -height 1000;
place [button .gs.sw.child.b1 -text "Exit"\
	-foreground white -background red -width 12\
	-command {!! Feature \"Exit\" is not supported}]\
	-x 100 -y 100;

! --> Call the update methods to make the Child appear on Screen;
!
#update;
.gs.sw update;
.gs update;

wm title .gs "Scrollwin";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Statusbar;
!

package require rtl_statusbar;

#option add *Rtl_statusbar*selectBackground: blue;
rtl_statusbar .sb -width 360 -creator toplevel;

wm title .sb "Statusbar";
wm geometry .sb 420x18;

update idletasks;

set i 0;

! --> Display messages in the statusbar and
!     let the progress bar flush across the entire statusbar
!
!     Make 7 progress segments

.sb progress init 7;
foreach msg [list one two three four five six seven] {

    .sb configure -text "New message has arrived : \"$msg\"";

    .sb progress set [incr i];
    update idletasks;

    ! *** Show Segment : $i in the statusbar .sb;

    after 1111;
}

! *** Wait up to 10 seconds to reset the statusbar .sb back
! *** to it's empty state
!

after 10000 {
    .sb progress hide;
    .sb configure -text "Progess sample has finished!";
}

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Tooltips;
!

! --> Create the main status procedure, which will maintain
!     long help messages extracted from the help items.
!

package require rtl_tooltip;

# ---------------------------------------------------------------------------
# Function    : status - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Userdefined status callback (used by the tooltip implementation).
# ---------------------------------------------------------------------------
proc status {message} {
    if {[string compare {} $message] != 0} {
	puts stdout "\[status message\] \"$message\"";
    }
}

#
# Create a sample widget hierarchy ...
#

toplevel .tts;

! toplevel --> .tts;
! 
! --> Add an help and href item to the toplevel widget;

set help "This is assigned to the toplevel widget .tts|Child of .tts";
set href "nirvana.htm#toplevel;
"
! --> help: $help;
! --> href: $href;
!

option add *tts.help $help;
option add *tts.href $href;
 
!
! --> Make a couple of \"tts\" childrens; 
!

foreach {bt help href} [list\
	Exit "This Button is used to close the entire\
	Application|Close Application" "nirvane.htm#404"\
	Apply "This Button will apply somthing|Apply something"\
	"/usr/local/stuff/nirvana.htm#404"\
	Cancel "I don't know what to cancel|what's this"\
	""] {

    grid [button .tts.[mkFirstLow $bt] -width 12 -text $bt]\
	    -sticky news;

    ! button --> .tts.[mkFirstLow $bt];
    !
    ! --> Add help and href items to the tk option database;
    ! --> help: $help;
    ! --> href: $href;
    !

    option add *tts.[mkFirstLow $bt].help $help;
    option add *tts.[mkFirstLow $bt].href $href;    
}

!
! --> Create the tooltip itself,
!     Note: you do not need to give any argument to the
!           tooltip call.
!

tooltip;

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Symbolbar;
!

! --> The Symbolbar widget let you easily compose toolbars,
!     the symbolbar is capable to maintain two different
!     action types: Free formed and menu assignments.
!

package require rtl_symbolbar;

! *** Create a Toplevel widget with an included menu

toplevel .ts -menu .ts.m

! *** Build some displayed menu widgets.

menu .ts.m
menu .ts.m.file
foreach item [list New Open Save {Save As} Close Exit] {
    .ts.m.file add command -label $item\
	    -command "! This is item $item of menu .ts.m.file";
}

! --> Create a Symbolbar which contains references to menu items
!

rtl_symbolbar .syb -width 100 -items {{.ts.m.file 1 {-image _new}}\
	{.ts.m.file 2 {-image _open}}\
	{.ts.m.file 3 {-image _save}}} -creator toplevel;

! --> Create a Symbolbar whith free defined items
!

rtl_symbolbar .syb2 -commands [list\
	{button %s -text "New Button" -bg green -image _new\
	-command "! Invoked by a green button"}\
	{button %s -text "Open Button" -bg yellow -image _new\
	-command "! Invoked by a yellow button"}\
	{button %s -text "Save Button" -bg red -fg white -image _save\
	-command "! Invoked by a red button"}] -creator toplevel;

wm geometry .syb2 100x34;
wm geometry .syb 100x34;
wm title .syb "Symbolbar";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Symbolbarcustomize;
!

! --> Create a symbolbarcustomize widget to reconfigure 
!     an itemset of a symbolbar, these example uses the
!     same menu as the first symbolbar example.
!

package require rtl_symbolbarcustomize;

! *** Setup a default symbolbar dataset
! *** These dataset will be used by the symbarcustomize example
!

variable ::qq;

set ::qq(sym,bars) {Default};
set ::qq(sym,Default,Options) {-items {{.ts.m.file 1 {-image _new}}
{.ts.m.file 2 {-image _open}} {.ts.m.file 3 {-image _save}}}};


! --> Create a symbolbarcustomize dialog with
!     .ts.m as the used root menu and array qq
!     as the container of all symbolbar data.
!

rtl_symbolbarcustomize .syc  -width 400 -height 400 -variable ::qq\
	    -root .ts.m\
	    -symbolbarsvar ::qq(sym,bars)\
	    -currentvar :::qq(sym,Current)\
	    -symbolbars [set ::qq(sym,bars)]\
	    -pattern "sym" -creator toplevel;

wm title .syc "Symbolbar Customize Dialog";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Symbutton
!

! --> Create a couple set of symbuttons and switch between
!     there appearance.
!

package require rtl_symbutton;

toplevel .sym;

wm title .sym "Symbuttons";

pack [symbutton .sym.s1 -image _new -text "Images"\
	-width 12 -imageheight 24\
	-command {rtl_symbutton::appearance image}];
pack [symbutton .sym.s2 -image _open -text "Text"\
	-width 12 -imageheight 24\
	-command {rtl_symbutton::appearance text}];
pack [symbutton .sym.s3 -image _save -text "Both"\
	-width 12 -imageheight 24 -appearance {image text}\
	-command {rtl_symbutton::appearance {text image}}];

! --> Set the default behavior to bot image and text
!

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Mlistbox;
!

! --> Create a multi column listbox, which is managed
!     by a gridwin widget.
!

package require rtl_gridwin;
package require rtl_mlistbox;

rtl_gridwin .gml -widget .gml.mlb -creator toplevel;
rtl_mlistbox .gml.mlb -columns 5 \
	-columnwidths [list 30 80 50 50 50]; 

!!! update;

! -->  Fill mlistbox with data

for {set i 0} {$i<30} {incr i} {

    # Built row
    set ent [list];
    for {set j 0} {$j<5} {incr j} {
	lappend ent "MLB Item ($j,$i)"
    }


    ! Insert --> .gml.mlb insert end $ent;
    
    .gml.mlb insert end $ent;
}

!
.gml update;

wm title .gml "Mlistbox";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Tree;
!

! --> Create the main status procedure, which will maintain
!     long help messages extracted from the help items.
!

package require rtl_tree;

rtl_tree .tree -width "200"\
	-height "200" -indentx "30" -indenty "25"\
	 -creator toplevel;

! --> Insert some items into the tree widget .tree
!     Note: the insert action make the item directly visible
!     

.tree insert / -label "global";
.tree insert /alpha -label "alpha";
.tree insert /beta -label "beta";
.tree insert /gamma -label "gamma";

foreach item [list one two three four five six seven] {
    .tree insert /alpha/$item -label "Alpha Item $item";
    .tree insert /beta/$item -label "Beta Item $item";
    .tree insert /gamma/$item -label "Gamma Item $item";
}

! --> Initiate a entire redraw
!

!!! Not required anymore when using insert: .tree build;

wm title .tree "Tree";

# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Sample Question Dialog;
!

package require rtl_question;

rtl_question .qu -title "Question Dialog"\
	-clientfcn "frame %s -bg white -width 200 -bd 1 -relief solid";


# ---------------------------------------------------------------------------
! ---------------------------------------------------------------------------
! Rearange Samples nicly on the screen; 
!

wm withdraw .ts
wm geometry .makeFont 480x360+526+601
wm geometry .shell 360x60+64+64
wm geometry .chooseFont 411x332+126+454
wm geometry .combobox 100x22+1000+130
wm geometry .spinbox 100x30+1000+254
wm geometry .pd 566x230+183+709
wm geometry .gw 200x200+590+130
wm geometry .gs 200x200+590+362
wm geometry .sb 420x18+0+0
wm geometry .tts 110x84+217+312
wm geometry .ts 200x200+401+120
wm geometry .syb 100x34+1000+316
wm geometry .syb2 100x34+1000+186
wm geometry .syc 400x400+1017+419
wm geometry .sym 154x138+215+142
wm geometry .gml 200x200+795+362
wm geometry .tree 200x200+795+130










