#!/bin/sh 
# start with WISH8.3 \
	exec wish8.3 "$0" ${1+"$@"} 
# VisualGIPSY 2.5
####VG-START####
set ::cmd(dir) "[file dirname [info script]]"; 
proc srcStartUp {file type script} {
    variable ::cmd;
    set src [file join $::cmd(dir) $file];
    if {[file exists $src]} {
	if {[catch {uplevel "#0" [list $script $src]} result]} {
	    puts stderr  "Error loading file $src:\n$result"; 
	}
    }
}
####VG-CODE:normal####

# ---------------------------------------------------------------------------
# Template    : cmd
# 
# Attributes  :

# ---------------------------------------------------------------------------
namespace eval cmd {

    # Define the built-in arrays
    variable w;
    variable var;

    # Widget array for template cmd
    array set w [list 0 {} 35 .cmdtabset.indexche.indexchefr.indexcheshireButton \
	17 .cmdtabset.build.buildfpfr.filepath 36 \
	.cmdtabset.indexclu.indexclufr 18 .cmdtabset.build.buildfpfr.buildpath \
	1 .menufr 37 .cmdtabset.indexclu.indexclufr.indexcludirlbl 20 \
	.cmdtabset.build.buildtagfr 19 \
	.cmdtabset.build.buildfpfr.buildassocfpBB 2 .cmdtabset 21 \
	.cmdtabset.build.buildtagfr.entertag 3 .menufr.switchmenu 38 \
	.cmdtabset.indexclu.indexclufr.indexcludirentry 40 \
	.cmdtabset.indexclu.indexclufr.indexclucflbl 39 \
	.cmdtabset.indexclu.indexclufr.indexcludirBB 22 \
	.cmdtabset.build.buildtagfr.buildtag 4\
	.menufr.switchmenu.switchcascade 41 \
	.cmdtabset.indexclu.indexclufr.indexclucfentry 23 \
	.cmdtabset.build.buildassocButton 5 .cmdtabset.build 42 \
	.cmdtabset.indexclu.indexclufr.indexclucfBB 24 \
	.cmdtabset.indexche.indexchefr 6 .cmdtabset.indexche 43 \
	.cmdtabset.indexclu.indexclufr.indexonlyckbut 25 \
	.cmdtabset.indexche.indexchefr.indexchedirlbl 7 .cmdtabset.indexclu 44 \
	.cmdtabset.indexclu.indexclufr.indexclustersButton 26 \
	.cmdtabset.indexche.indexchefr.indexchedirentry 8 \
	.cmdtabset.build.buildradiofr 45 .cmdtabset.c.f 27 \
	.cmdtabset.indexche.indexchefr.indexchedirBB 9 \
	.cmdtabset.build.buildradiofr.file 10 \
	.cmdtabset.build.buildradiofr.singlefile 46 .cmdstatusbar 28 \
	.cmdtabset.indexche.indexchefr.indexchecflbl 11 \
	.cmdtabset.build.buildradiofr.directorytree 30 \
	.cmdtabset.indexche.indexchefr.indexchecfBB 29 \
	.cmdtabset.indexche.indexchefr.indexchecfentry 12 \
	.cmdtabset.build.buildnamefr 31 \
	.cmdtabset.indexche.indexchefr.indexchestartreclbl 13 \
	.cmdtabset.build.buildnamefr.assocfilename 32 \
	.cmdtabset.indexche.indexchefr.indexchestartrecentry 14 \
	.cmdtabset.build.buildnamefr.buildname 33 \
	.cmdtabset.indexche.indexchefr.indexchemaxreclbl 15 \
	.cmdtabset.build.buildnamefr.optionallbl 34 \
	.cmdtabset.indexche.indexchefr.indexchemaxrecentry 16 \
	.cmdtabset.build.buildfpfr];

    # -----------------------------------------------------------------------
    # The template interface of cmd
    array set var [list  original toplevel attributes {}];

}

# ---------------------------------------------------------------------------
# Function    : cmd - template constructor
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
#               cmd. The Constructor may be used as each other
#               widget class.
# ---------------------------------------------------------------------------
proc cmd {{base .cmd} args} {
    variable ::cmd::w;
    variable ::cmd::var;

    if {![winfo exists $base]} {

	# -------------------------------------------------------------------
	# The widget hierarchy
	# -------------------------------------------------------------------
	# Look for creation class of template cmd:
	[getCreator cmd $base$w(0) $args] $base$w(0)  -class Cmd\
		;


	# Children of $base$w(0) := cmd (level:2)
	frame $base$w(1) -height "40" -relief "raised"\
		-width "40";

	# Children of $base$w(1) := menufr (level:3)
	menubutton $base$w(3) -menu "$base$w(4)" -padx "5" -pady "3"\
		-relief "raised";

	# Children of $base$w(3) := switchmenu (level:4)
	menu $base$w(4) -activebackground "lightgray";

	# Items of $base$w(4) := switchcascade 
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu1accelerator {}]"\
		-command "CmdToDb"\
		-font "[opget $base$w(4) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu1label {Db Config}]"\
		-underline "[opget $base$w(4) menu1underline {-1}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu2accelerator {}]"\
		-command "CmdToServer"\
		-font "[opget $base$w(4) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu2label {Server Config}]"\
		-underline "[opget $base$w(4) menu2underline {-1}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu3accelerator {}]"\
		-command "CmdClose"\
		-font "[opget $base$w(4) menu3font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu3label {Exit}]"\
		-underline "[opget $base$w(4) menu3underline {-1}]";


	pack $base$w(3) -ipady 2 -side left;
	rtl_tabset $base$w(2) -autovisible "1" -borderspace "10"\
		-height "500" -offset "-10" -orient "horizontal"\
		-show "build indexche indexclu" -tabbackground "lightgray"\
		-tabfont "Helvetica 11 bold" -tabforeground "black"\
		-taboffset "50" -tabwidth "100" -type "rtl_tabItem"\
		-width "700";

	# Items of $base$w(2) := cmdtabset 
	$base$w(2) add build \
		-foreground "[opget $base$w(2) buildforeground {cornsilk}]"\
		-width "120" -onvalue "1" -widget "$base$w(5)"\
		-text "[opget $base$w(2) buildtext {buildassoc    }]"\
		-background "[opget $base$w(2) buildbackground {lightblue}]"\
		-state "normal";
	$base$w(2) add indexche \
		-foreground "[opget $base$w(2) indexcheforeground {cornsilk}]"\
		-width "120" -onvalue "1" -widget "$base$w(6)"\
		-text "[opget $base$w(2) indexchetext {index_cheshire}]"\
		-background "[opget $base$w(2) indexchebackground {lightblue}]"\
		-state "normal";
	$base$w(2) add indexclu \
		-foreground "[opget $base$w(2) indexcluforeground {cornsilk}]"\
		-width "120" -onvalue "1" -widget "$base$w(7)"\
		-text "[opget $base$w(2) indexclutext {index_clusters}]"\
		-background "[opget $base$w(2) indexclubackground {lightblue}]"\
		-state "normal";
	$base$w(2) setselection indexclu;
	frame $base$w(5) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "40";

	# Children of $base$w(5) := build (level:4)
	frame $base$w(8) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(8) := buildradiofr (level:5)
	label $base$w(9) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray" -relief "groove";
	radiobutton $base$w(10) -activebackground "gray86"\
		-activeforeground "black" -command "BuildSingle"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "single" -variable "cmdArray(singleormulti)";
	radiobutton $base$w(11) -activebackground "gray86"\
		-activeforeground "black" -anchor "nw" -command "BuildMultiple"\
		-highlightbackground "lightgray" -selectcolor "blue"\
		-value "multiple" -variable "cmdArray(singleormulti)";

	pack $base$w(9) -anchor w -padx 15 -pady 5;
	pack $base$w(10) -anchor nw -padx 30 -pady 5 -side left;
	pack $base$w(11) -anchor nw -padx 15 -pady 5 -side left;
	frame $base$w(12) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(12) := buildnamefr (level:5)
	label $base$w(13) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(14) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(build,name)";
	label $base$w(15) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray"\
		-textvariable "cmdArray(nameoptional)";

	pack $base$w(13) -anchor w -padx 15 -pady 5;
	pack $base$w(14) -anchor w -padx 30 -pady 5 -side left;
	pack $base$w(15) -expand 1 -fill x;
	frame $base$w(16) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(16) := buildfpfr (level:5)
	label $base$w(17) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(18) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(build,path)" -width "50";
	button $base$w(19) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(19)"\
		-highlightbackground "lightgray" -padx "1";

	pack $base$w(17) -anchor w -padx 15 -pady 5;
	pack $base$w(18) -anchor w -padx 30 -pady 5 -side left;
	pack $base$w(19) -side left;
	frame $base$w(20) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(20) := buildtagfr (level:5)
	label $base$w(21) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(22) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(build,tag)" -width "10";

	pack $base$w(21) -anchor w -padx 15 -pady 5 -side left;
	pack $base$w(22) -anchor w -padx 15 -pady 5 -side left;
	button $base$w(23) -activebackground "gray86"\
		-activeforeground "black" -command "CallBuildassoc"\
		-highlightbackground "lightgray";

	pack $base$w(8) -anchor n -fill x -pady 10;
	pack $base$w(12) -fill x -pady 10;
	pack $base$w(16) -fill x -pady 10;
	pack $base$w(20) -fill x -pady 10;
	pack $base$w(23) -anchor w -padx 30 -pady 20;
	frame $base$w(6) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "30";

	# Children of $base$w(6) := indexche (level:4)
	frame $base$w(24) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(24) := indexchefr (level:5)
	label $base$w(25) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(26) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(indexche,dir)" -width "50";
	button $base$w(27) -activebackground "gray86"\
		-activeforeground "black"\
		-command "fileselect $base$w(27) HandleBBSelect"\
		-highlightbackground "lightgray" -padx "1";
	label $base$w(28) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(29) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(indexche,cf)" -width "50";
	button $base$w(30) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(30)"\
		-highlightbackground "lightgray" -padx "1";
	label $base$w(31) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(32) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "10";
	label $base$w(33) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(34) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black" -width "10";
	button $base$w(35) -activebackground "gray86"\
		-activeforeground "black" -command "CallIndexcheshire"\
		-highlightbackground "lightgray" -justify "left";

	grid $base$w(25) -column 0 -row 0 -padx 25 -pady 10 -sticky w;
	grid $base$w(26) -column 0 -row 1 -padx 30 -pady 10 -sticky ew;
	grid $base$w(27) -column 5 -row 1 -padx 20 -pady 10 -sticky w;
	grid $base$w(28) -column 0 -row 2 -padx 25 -pady 10 -sticky w;
	grid $base$w(29) -column 0 -row 3 -padx 30 -pady 10 -sticky ew;
	grid $base$w(30) -column 5 -row 3 -padx 20 -pady 10 -sticky w;
	grid $base$w(31) -column 0 -row 4 -padx 25 -pady 10 -sticky w;
	grid $base$w(32) -column 5 -row 4 -pady 10 -sticky w;
	grid $base$w(33) -column 0 -row 5 -padx 25 -pady 10 -sticky w;
	grid $base$w(34) -column 5 -row 5 -pady 10 -sticky w;
	grid $base$w(35) -column 0 -row 6 -padx 30 -pady 30 -sticky w;

	# Grid (size:6 7) $base$w(24) = indexchefr
	foreach {id opt param} [list  0 weight 2 5 weight 4] {
	    grid columnconfigure $base$w(24) $id -$opt $param;
	}
	foreach {id opt param} [list  0 minsize 38] {
	    grid rowconfigure $base$w(24) $id -$opt $param;
	}

	pack $base$w(24) -anchor n -fill x -pady 20;
	frame $base$w(7) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "40";

	# Children of $base$w(7) := indexclu (level:4)
	frame $base$w(36) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(36) := indexclufr (level:5)
	label $base$w(37) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(38) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(indexclu,dir)" -width "60";
	button $base$w(39) -activebackground "gray86"\
		-activeforeground "black"\
		-command "fileselect $base$w(39) HandleBBSelect"\
		-highlightbackground "lightgray" -padx "1";
	label $base$w(40) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(41) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "cmdArray(indexclu,cf)" -width "60";
	button $base$w(42) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(42)"\
		-highlightbackground "lightgray" -padx "1";
	checkbutton $base$w(43) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "cmdArray(indexonly)";
	button $base$w(44) -activebackground "gray86"\
		-activeforeground "black" -command "CallIndexclusters"\
		-highlightbackground "lightgray";

	grid $base$w(37) -column 0 -row 0 -padx 25 -pady 10 -sticky w;
	grid $base$w(38) -column 0 -row 1 -padx 30 -pady 10 -sticky ew;
	grid $base$w(39) -column 5 -row 1 -pady 10 -sticky w;
	grid $base$w(40) -column 0 -row 2 -padx 25 -pady 10 -sticky w;
	grid $base$w(41) -column 0 -row 3 -padx 30 -pady 10 -sticky ew;
	grid $base$w(42) -column 5 -row 3 -pady 10 -sticky w;
	grid $base$w(43) -column 0 -row 4 -padx 25 -pady 10 -sticky w;
	grid $base$w(44) -column 0 -row 5 -padx 30 -pady 30 -sticky w;

	# Grid (size:6 6) $base$w(36) = indexclufr
	foreach {id opt param} [list  0 weight 2 5 weight 4] {
	    grid columnconfigure $base$w(36) $id -$opt $param;
	}
	foreach {id opt param} [list  0 minsize 38] {
	    grid rowconfigure $base$w(36) $id -$opt $param;
	}

	pack $base$w(36) -expand 1 -fill both;
	pack $base$w(7) -in $base$w(45) -expand 1 -fill both -padx 3 -pady 3;
	rtl_statusbar $base$w(46) -labelwidth "1";

	pack $base$w(1) -expand 1 -fill x;
	pack $base$w(2) -expand 1 -fill x;
	pack propagate $base$w(2) 0;
	pack $base$w(46) -expand 1 -fill x;

	# -------------------------------------------------------------------
	# Call the init method
	if {[catch {
	    ::cmd::init $base;
	} msg]} {
	    # init does not exist.
	}

	# Additional Code
	

	# -------------------------------------------------------------------
	# Use the template interface for this instance
	set base [mkProc $base cmd $args];
    }

    return $base;
}


# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet!
# ---------------------------------------------------------------------------


namespace eval cmd {
 
    # option database items to template cmd

    option add *Cmd$w(0).background {lightgray};
    option add *Cmd$w(1).background {lightgray};
    option add *Cmd$w(3).background {lightgray};
    option add *Cmd$w(3).text {SwitchTask};
    option add *Cmd$w(3).underline {-1};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {Db Config} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Server Config} menu3font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu3label\
	    {Exit}] {
 
	option add *Cmd$w(4).$opt $param; 
    }

    option add *Cmd$w(2).background {lightgray};
    option add *Cmd$w(5).background {lightgray};
    option add *Cmd$w(8).background {lightgray};
    option add *Cmd$w(9).background {lightgray};
    option add *Cmd$w(9).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(9).foreground {black};
    option add *Cmd$w(9).text {1. The data is:};
    option add *Cmd$w(10).background {lightgray};
    option add *Cmd$w(10).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(10).foreground {black};
    option add *Cmd$w(10).text {A single SGML file};
    option add *Cmd$w(11).background {lightgray};
    option add *Cmd$w(11).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(11).foreground {black};
    option add *Cmd$w(11).text {Multiple files within a directory subtree};
    option add *Cmd$w(12).background {lightgray};
    option add *Cmd$w(13).background {lightgray};
    option add *Cmd$w(13).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(13).foreground {black};
    option add *Cmd$w(13).text {2. Enter a name for the associator file};
    option add *Cmd$w(14).background {gray95};
    option add *Cmd$w(14).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(14).foreground {black};
    option add *Cmd$w(15).background {lightgray};
    option add *Cmd$w(15).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(15).foreground {blue};
    option add *Cmd$w(16).background {lightgray};
    option add *Cmd$w(17).background {lightgray};
    option add *Cmd$w(17).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(17).foreground {black};
    option add *Cmd$w(17).text {3. Enter the full path of the file or the top-level dirctory:};
    option add *Cmd$w(18).background {gray95};
    option add *Cmd$w(18).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(18).foreground {black};
    option add *Cmd$w(19).background {lightgray};
    option add *Cmd$w(19).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Cmd$w(19).foreground {black};
    option add *Cmd$w(19).text {Browse};
    option add *Cmd$w(20).background {lightgray};
    option add *Cmd$w(21).background {lightgray};
    option add *Cmd$w(21).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(21).foreground {black};
    option add *Cmd$w(21).text {4. Enter the top level SGML tag defining a record in the data file:};
    option add *Cmd$w(22).background {gray95};
    option add *Cmd$w(22).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(22).foreground {black};
    option add *Cmd$w(23).background {lightgray};
    option add *Cmd$w(23).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(23).foreground {black};
    option add *Cmd$w(23).text {Build Associator File};
    option add *Cmd$w(6).background {lightgray};
    option add *Cmd$w(24).background {lightgray};
    option add *Cmd$w(25).background {lightgray};
    option add *Cmd$w(25).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(25).foreground {black};
    option add *Cmd$w(25).text {1. Path of the directory to put temporary files};
    option add *Cmd$w(26).background {gray95};
    option add *Cmd$w(26).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(26).foreground {black};
    option add *Cmd$w(27).background {lightgray};
    option add *Cmd$w(27).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Cmd$w(27).foreground {black};
    option add *Cmd$w(27).text {Browse};
    option add *Cmd$w(28).background {lightgray};
    option add *Cmd$w(28).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(28).foreground {black};
    option add *Cmd$w(28).text {2. Path of the database configuration file};
    option add *Cmd$w(29).background {gray95};
    option add *Cmd$w(29).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(29).foreground {black};
    option add *Cmd$w(30).background {lightgray};
    option add *Cmd$w(30).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Cmd$w(30).foreground {black};
    option add *Cmd$w(30).text {Browse};
    option add *Cmd$w(31).background {lightgray};
    option add *Cmd$w(31).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(31).foreground {black};
    option add *Cmd$w(31).text {3. Logical record number of the record to start indexing};
    option add *Cmd$w(32).background {gray95};
    option add *Cmd$w(32).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(32).foreground {black};
    option add *Cmd$w(33).background {lightgray};
    option add *Cmd$w(33).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(33).foreground {black};
    option add *Cmd$w(33).text {4. Logical record number of the last record to index};
    option add *Cmd$w(34).background {gray95};
    option add *Cmd$w(34).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(34).foreground {black};
    option add *Cmd$w(35).background {lightgray};
    option add *Cmd$w(35).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(35).foreground {black};
    option add *Cmd$w(35).text {index_cheshire};
    option add *Cmd$w(7).background {lightgray};
    option add *Cmd$w(36).background {lightgray};
    option add *Cmd$w(37).background {lightgray};
    option add *Cmd$w(37).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(37).foreground {black};
    option add *Cmd$w(37).text {1. Path of the directory to put temporary files};
    option add *Cmd$w(38).background {gray95};
    option add *Cmd$w(38).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(38).foreground {black};
    option add *Cmd$w(39).background {lightgray};
    option add *Cmd$w(39).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Cmd$w(39).foreground {black};
    option add *Cmd$w(39).text {Browse};
    option add *Cmd$w(40).background {lightgray};
    option add *Cmd$w(40).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(40).foreground {black};
    option add *Cmd$w(40).text {2. Path of the database configuration file};
    option add *Cmd$w(41).background {gray95};
    option add *Cmd$w(41).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(41).foreground {black};
    option add *Cmd$w(42).background {lightgray};
    option add *Cmd$w(42).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Cmd$w(42).foreground {black};
    option add *Cmd$w(42).text {Browse};
    option add *Cmd$w(43).background {lightgray};
    option add *Cmd$w(43).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(43).foreground {black};
    option add *Cmd$w(43).text {Index Only};
    option add *Cmd$w(44).background {lightgray};
    option add *Cmd$w(44).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Cmd$w(44).foreground {black};
    option add *Cmd$w(44).text {index_clusters};
    option add *Cmd$w(46).background {lightgray};
    option add *Cmd$w(46).label {};
}


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,cmd) cmd;
# Local used Packages:
set ::rat(PACKAGES,LOADED) [list ];
# ---------------------------------------------------------------------------

srcStartUp cmdifc.tcl CODE source;
####VG-STOP####

