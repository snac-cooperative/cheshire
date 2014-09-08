#!/bin/sh 
# start with WISH8.3 \
	exec wish8.3 "$0" ${1+"$@"} 
# VisualGIPSY 2.5
####VG-START####
set ::server(dir) "[file dirname [info script]]"; 
proc srcStartUp {file type script} {
    variable ::server;
    set src [file join $::server(dir) $file];
    if {[file exists $src]} {
	if {[catch {uplevel "#0" [list $script $src]} result]} {
	    puts stderr  "Error loading file $src:\n$result"; 
	}
    }
}
####VG-CODE:normal####

# ---------------------------------------------------------------------------
# Template    : servercfg
# 
# Attributes  :

# ---------------------------------------------------------------------------
namespace eval servercfg {

    # Define the built-in arrays
    variable w;
    variable var;

    # Widget array for template servercfg
    array set w [list 35 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbdirbb \
	36 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbcffilesentry \
	37 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbcffilesbb \
	38 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.timeoutlbl \
	39 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.timeoutentry \
	40 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.logfilenamelbl \
	41 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.logfileentry \
	42 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.rsdirlbl \
	43 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.rsdirentry \
	44 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.asidentry \
	45 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.rsdirbb \
	46 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.asidlbl \
	47 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe \
	48 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.snrsckbut \
	50 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st0qckbut \
	49 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.smdsckbut \
	51 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st1qckbut \
	52 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st2qckbut \
	53 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st100qckbut \
	54 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st101qckbut \
	55 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.st102qckbut \
	56 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.sesnckbut \
	57 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportframe.ssesnckbut \
	58 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfolbl \
	60 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.newslbl \
	59 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe \
	61 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.welcomelbl \
	62 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contactlbl \
	63 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contdeslbl \
	64 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contaddrlbl \
	65 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contemaillbl \
	66 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contphonelbl \
	67 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.serdesclbl \
	68 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.userestlbl \
	70 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.langlbl \
	69 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.hourslbl \
	71 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.newsentry \
	72 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.welcomentry \
	73 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contactentry \
	74 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contdesentry \
	75 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contaddrentry \
	76 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contemailentry \
	77 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.contphoneentry \
	78 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.serdesentry \
	80 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.payaddrentry \
	79 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.useresentry \
	81 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.payaddrlbl \
	10 .fsercfg.servercfgparamframe.gridwin.scrollwin.frame3 82 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.hourentry \
	11 .fsercfg.servercfgparamframe.gridwin.hori 83 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.explaininfoframe.langentry \
	12 .fsercfg.servercfgparamframe.gridwin.vert 84 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.supportlbl \
	13 .fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr\
	85 .fsercfg.servercfgparamframe.gridwin.scrollwin.canvas.frame 14 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.pmslbl \
	86 .fsercfg.servercfgmenuframe.servswitch 15 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.pmsentry \
	87 .fsercfg.servercfgmenuframe.servswitch.switchcascade 16 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.mrslbl \
	17 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.mrsentry \
	0 {} 1 .fsercfg 18 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.iidlbl \
	2 .fsercfg.servercfgmenuframe 19 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.iidentry \
	20 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.inamelbl \
	3 .fsercfg.servercfgmenuframe.servfile 21 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.inameentry \
	4 .fsercfg.servercfgmenuframe.servfile.servfilecascade 22 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.iverlbl \
	5 .fsercfg.servercfgmenuframe.servhelp 23 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.iverentry \
	6 .fsercfg.servercfgmenuframe.servhelp.servhelpcascade 24 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.pverlbl \
	7 .fsercfg.servercfgparamframe 25 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.pverentry \
	8 .fsercfg.servercfgparamframe.gridwin 26 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.optlbl \
	9 .fsercfg.servercfgparamframe.gridwin.scrollwin 27 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.optentry \
	28 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.portlbl \
	29 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.portentry \
	30 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbnameslbl \
	31 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbnamesentry \
	32 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbdirlbl \
	33 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbcfgfileslbl \
	34 \
	.fsercfg.servercfgparamframe.gridwin.scrollwin.frame3.servercfgfr.dbdirentry];

    # -----------------------------------------------------------------------
    # The template interface of servercfg
    array set var [list  original toplevel attributes {}];

}

# ---------------------------------------------------------------------------
# Function    : servercfg - template constructor
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
#               servercfg. The Constructor may be used as each other
#               widget class.
# ---------------------------------------------------------------------------
proc servercfg {{base .servercfg} args} {
    variable ::servercfg::w;
    variable ::servercfg::var;

    if {![winfo exists $base]} {

	# -------------------------------------------------------------------
	# The widget hierarchy
	# -------------------------------------------------------------------
	# Look for creation class of template servercfg:
	[getCreator servercfg $base$w(0) $args] $base$w(0)  -class Servercfg\
		;


	# Children of $base$w(0) := servercfg (level:2)
	frame $base$w(1) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(1) := fsercfg (level:3)
	frame $base$w(2) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "raised" -width "700";

	# Children of $base$w(2) := servercfgmenuframe (level:4)
	menubutton $base$w(3) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(4)" -padx "5" -pady "4";

	# Children of $base$w(3) := servfile (level:5)
	menu $base$w(4) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(4) := servfilecascade 
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu1accelerator {Alt-n}]"\
		-command "NewServerInit"\
		-font "[opget $base$w(4) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu1label {New}]"\
		-underline "[opget $base$w(4) menu1underline {0}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu2accelerator {Alt-o}]"\
		-command "OpenServerInit"\
		-font "[opget $base$w(4) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu2label {Open}]"\
		-underline "[opget $base$w(4) menu2underline {0}]";
	$base$w(4) add separator;
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu4accelerator {Alt-s}]"\
		-command "SaveServerInitFile"\
		-font "[opget $base$w(4) menu4font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu4label {Save}]"\
		-underline "[opget $base$w(4) menu4underline {0}]";
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu5accelerator {Alt-a}]"\
		-command "SaveServerInitFileAs"\
		-font "[opget $base$w(4) menu5font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu5label {Save as}]"\
		-underline "[opget $base$w(4) menu5underline {5}]";
	$base$w(4) add separator;
	$base$w(4) add command\
		-accelerator "[opget $base$w(4) menu7accelerator {Alt-x}]"\
		-command "CloseServerInit"\
		-font "[opget $base$w(4) menu7font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(4) menu7label {Exit}]"\
		-underline "[opget $base$w(4) menu7underline {1}]";

	menubutton $base$w(86) -activebackground "gray86"\
		-menu "$base$w(87)" -padx "5" -pady "3";

	# Children of $base$w(86) := servswitch (level:5)
	menu $base$w(87);

	# Items of $base$w(87) := switchcascade 
	$base$w(87) add command\
		-accelerator "[opget $base$w(87) menu1accelerator {}]"\
		-command "ServerToDb"\
		-font "[opget $base$w(87) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(87) menu1label {DB Config}]"\
		-underline "[opget $base$w(87) menu1underline {-1}]";
	$base$w(87) add command\
		-accelerator "[opget $base$w(87) menu2accelerator {}]"\
		-command "ServerToCmd build"\
		-font "[opget $base$w(87) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(87) menu2label {buildassoc}]"\
		-underline "[opget $base$w(87) menu2underline {-1}]";
	$base$w(87) add command\
		-accelerator "[opget $base$w(87) menu3accelerator {}]"\
		-command "ServerToCmd indexche"\
		-font "[opget $base$w(87) menu3font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(87) menu3label {index_cheshire}]"\
		-underline "[opget $base$w(87) menu3underline {-1}]";
	$base$w(87) add command\
		-accelerator "[opget $base$w(87) menu4accelerator {}]"\
		-command "ServerToCmd indexclu"\
		-font "[opget $base$w(87) menu4font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(87) menu4label {index_clusters}]"\
		-underline "[opget $base$w(87) menu4underline {-1}]";

	menubutton $base$w(5) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-menu "$base$w(6)" -padx "5" -pady "4";

	# Children of $base$w(5) := servhelp (level:5)
	menu $base$w(6) -activebackground "gray86"\
		-activeforeground "black" -selectcolor "yellow";

	# Items of $base$w(6) := servhelpcascade 
	$base$w(6) add command\
		-accelerator "[opget $base$w(6) menu1accelerator {}]"\
		-font "[opget $base$w(6) menu1font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(6) menu1label {About this tool}]"\
		-underline "[opget $base$w(6) menu1underline {-1}]";
	$base$w(6) add command\
		-accelerator "[opget $base$w(6) menu2accelerator {}]"\
		-font "[opget $base$w(6) menu2font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}]"\
		-label "[opget $base$w(6) menu2label {Readme}]"\
		-underline "[opget $base$w(6) menu2underline {-1}]";


	pack $base$w(3) -ipadx 5 -side left;
	pack $base$w(86) -ipadx 5 -side left;
	pack $base$w(5) -ipadx 5 -side right;
	frame $base$w(7) -borderwidth "1" -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(7) := servercfgparamframe (level:4)
	rtl_gridwin $base$w(8) -activebackground "gray86" -auto "1"\
		-borderwidth "1" -fill "both" -height "600"\
		-highlightbackground "lightgray" -relief "sunken"\
		-scrollborderwidth "1" -sides "bottom right"\
		-troughcolor "gray92" -widget "$base$w(9)" -width "800";
	rtl_scrollwin $base$w(9) -fill "none" -height "550"\
		-highlightbackground "lightgray" -widget "$base$w(10)"\
		-width "800"\
		-xscrollcommand "rslider $base$w(9) $base$w(11) x 1; $base$w(11) set"\
		-yscrollcommand "rslider $base$w(9) $base$w(12) y 1; $base$w(12) set";
	frame $base$w(10) -borderwidth "3" -height "1200"\
		-highlightbackground "lightGray" -width "800";

	# Events for $base$w(10) := frame3
	bindtags $base$w(10) [list $base$w(10) _servercfg_fsercfg_servercfgparamframe_gridwin_scrollwin _staffcheshire_tabset_fsercfg_servercfgparamframe_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_dbcfntbk_nbframe_expln_gridwin_scrollwin _staffcheshire_tabset_fdbcfg_mainframe_gridwin_scrollwin _toplevel1_mainframe_gridwin_scrollwin _toplevel1_fdemo_gridwin_scrollwin _rtlDemo_tabtour_fscrollwin_fdemo_gridwin_scrollwin Frame all];


	# Children of $base$w(10) := frame3 (level:7)
	frame $base$w(13) -height "40"\
		-highlightbackground "lightgray" -width "40";

	# Children of $base$w(13) := servercfgfr (level:8)
	label $base$w(14) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(15) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(pms)" -width "10";
	label $base$w(16) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(17) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(mrs)" -width "10";
	label $base$w(18) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(19) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(iid)" -width "10";
	label $base$w(20) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(21) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(iname)" -width "30";
	label $base$w(22) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(23) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(iver)" -width "10";
	label $base$w(24) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(25) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(pver)" -width "10";
	label $base$w(26) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(27) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(opt)";
	label $base$w(28) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(29) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(port)" -width "6";
	label $base$w(30) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(31) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(dbnames)" -width "40";
	label $base$w(32) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(33) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(34) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(dbdir)" -width "40";
	button $base$w(35) -activebackground "gray86"\
		-activeforeground "black"\
		-command "fileselect $base$w(35) HandleBBSelect"\
		-highlightbackground "lightgray" -padx "1";
	entry $base$w(36) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(dbcffiles)" -width "40";
	button $base$w(37) -activebackground "gray86"\
		-activeforeground "black" -command "HandleBBSelect $base$w(37)"\
		-highlightbackground "lightgray" -padx "1";
	label $base$w(38) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(39) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(timeout)" -width "10";
	label $base$w(40) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(41) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(logfile)";
	label $base$w(42) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(43) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(rsdir)" -width "40";
	entry $base$w(44) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(asid)" -width "40";
	button $base$w(45) -activebackground "gray86"\
		-activeforeground "black"\
		-command "fileselect $base$w(45) HandleBBSelect"\
		-highlightbackground "lightgray" -padx "1";
	label $base$w(46) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	frame $base$w(47) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "groove" -width "40";

	# Children of $base$w(47) := supportframe (level:9)
	checkbutton $base$w(48) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,snrs)";
	checkbutton $base$w(49) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,smds)";
	checkbutton $base$w(50) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st0q)";
	checkbutton $base$w(51) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st1q)";
	checkbutton $base$w(52) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st2q)";
	checkbutton $base$w(53) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st100q)";
	checkbutton $base$w(54) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st101q)";
	checkbutton $base$w(55) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,st102q)";
	checkbutton $base$w(56) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,sesn)";
	checkbutton $base$w(57) -activebackground "gray86"\
		-activeforeground "black" -highlightbackground "lightgray"\
		-selectcolor "blue" -variable "serverArray(support,ssesn)";

	grid $base$w(48) -column 0 -row 0 -pady 5 -sticky w;
	grid $base$w(49) -column 1 -row 0 -padx 30 -pady 5 -sticky w;
	grid $base$w(50) -column 0 -row 1 -pady 5 -sticky w;
	grid $base$w(51) -column 1 -row 1 -padx 30 -pady 5 -sticky w;
	grid $base$w(52) -column 0 -row 2 -pady 5 -sticky w;
	grid $base$w(53) -column 1 -row 2 -padx 30 -pady 5 -sticky w;
	grid $base$w(54) -column 0 -row 3 -pady 5 -sticky w;
	grid $base$w(55) -column 1 -row 3 -padx 30 -pady 5 -sticky w;
	grid $base$w(56) -column 0 -row 4 -pady 5 -sticky w;
	grid $base$w(57) -column 1 -row 4 -padx 30 -pady 5 -sticky w;

	# Grid (size:2 5) $base$w(47) = supportframe
	foreach {id opt param} [list  0 weight 1] {
	    grid columnconfigure $base$w(47) $id -$opt $param;
	}
	label $base$w(58) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	frame $base$w(59) -borderwidth "2" -height "40"\
		-highlightbackground "lightgray" -relief "groove" -width "40";

	# Children of $base$w(59) := explaininfoframe (level:9)
	label $base$w(60) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(61) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(62) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(63) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(64) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(65) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(66) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(67) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(68) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(69) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	label $base$w(70) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(71) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,news)";
	entry $base$w(72) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,welcome)";
	entry $base$w(73) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,contact)";
	entry $base$w(74) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,contdes)";
	entry $base$w(75) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,contaddr)";
	entry $base$w(76) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,contemail)";
	entry $base$w(77) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,contphone)";
	entry $base$w(78) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,serdes)";
	entry $base$w(79) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,useres)";
	entry $base$w(80) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,payaddr)";
	label $base$w(81) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";
	entry $base$w(82) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,hour)";
	entry $base$w(83) -highlightbackground "lightgray"\
		-insertbackground "black" -selectbackground "lightblue"\
		-selectborderwidth "0" -selectforeground "black"\
		-textvariable "serverArray(explain,lang)";

	grid $base$w(60) -column 0 -row 0 -padx 30 -pady 5 -sticky w;
	grid $base$w(61) -column 0 -row 1 -padx 30 -pady 5 -sticky w;
	grid $base$w(62) -column 0 -row 2 -padx 30 -pady 5 -sticky w;
	grid $base$w(63) -column 0 -row 3 -padx 30 -pady 5 -sticky w;
	grid $base$w(64) -column 0 -row 4 -padx 30 -pady 5 -sticky w;
	grid $base$w(65) -column 0 -row 5 -padx 30 -pady 5 -sticky w;
	grid $base$w(66) -column 0 -row 6 -padx 30 -pady 5 -sticky w;
	grid $base$w(67) -column 0 -row 7 -padx 30 -pady 5 -sticky w;
	grid $base$w(68) -column 0 -row 8 -padx 30 -pady 5 -sticky w;
	grid $base$w(69) -column 0 -row 10 -padx 30 -pady 5 -sticky w;
	grid $base$w(70) -column 0 -row 11 -padx 30 -pady 5 -sticky w;
	grid $base$w(71) -column 1 -row 0 -sticky nesw;
	grid $base$w(72) -column 1 -row 1 -sticky nesw;
	grid $base$w(73) -column 1 -row 2 -sticky nesw;
	grid $base$w(74) -column 1 -row 3 -sticky nesw;
	grid $base$w(75) -column 1 -row 4 -sticky nesw;
	grid $base$w(76) -column 1 -row 5 -sticky nesw;
	grid $base$w(77) -column 1 -row 6 -sticky nesw;
	grid $base$w(78) -column 1 -row 7 -sticky nesw;
	grid $base$w(79) -column 1 -row 8 -sticky nesw;
	grid $base$w(80) -column 1 -row 9 -sticky nesw;
	grid $base$w(81) -column 0 -row 9 -padx 30 -pady 5 -sticky w;
	grid $base$w(82) -column 1 -row 10 -sticky nesw;
	grid $base$w(83) -column 1 -row 11 -sticky nesw;

	# Grid (size:2 12) $base$w(59) = explaininfoframe
	foreach {id opt param} [list  1 weight 1] {
	    grid columnconfigure $base$w(59) $id -$opt $param;
	}
	label $base$w(84) -anchor "w" -borderwidth "0"\
		-highlightbackground "lightgray";

	grid $base$w(14) -column 0 -row 0 -padx 30 -pady 5 -sticky w;
	grid $base$w(15) -column 1 -row 0 -padx 30 -pady 5 -sticky w;
	grid $base$w(16) -column 0 -row 1 -padx 30 -pady 5 -sticky w;
	grid $base$w(17) -column 1 -row 1 -padx 30 -pady 5 -sticky w;
	grid $base$w(18) -column 0 -row 2 -padx 30 -pady 5 -sticky w;
	grid $base$w(19) -column 1 -row 2 -padx 30 -pady 5 -sticky w;
	grid $base$w(20) -column 0 -row 3 -padx 30 -pady 5 -sticky w;
	grid $base$w(21) -column 1 -row 3 -padx 30 -pady 5 -sticky w;
	grid $base$w(22) -column 0 -row 4 -padx 30 -pady 5 -sticky w;
	grid $base$w(23) -column 1 -row 4 -padx 30 -pady 5 -sticky w;
	grid $base$w(24) -column 0 -row 5 -padx 30 -pady 5 -sticky w;
	grid $base$w(25) -column 1 -row 5 -padx 30 -pady 5 -sticky w;
	grid $base$w(26) -column 0 -row 6 -padx 30 -pady 5 -sticky w;
	grid $base$w(27) -column 1 -row 6 -padx 30 -pady 5 -sticky w;
	grid $base$w(28) -column 0 -row 7 -padx 30 -pady 5 -sticky w;
	grid $base$w(29) -column 1 -row 7 -padx 30 -pady 5 -sticky w;
	grid $base$w(30) -column 0 -row 8 -padx 30 -pady 5 -sticky w;
	grid $base$w(31) -column 1 -row 8 -padx 30 -pady 5 -sticky w;
	grid $base$w(32) -column 0 -row 9 -padx 30 -pady 5 -sticky w;
	grid $base$w(33) -column 0 -row 10 -padx 30 -pady 5 -sticky w;
	grid $base$w(34) -column 1 -row 9 -padx 30 -pady 5 -sticky w;
	grid $base$w(35) -column 2 -row 9 -pady 5 -sticky nesw;
	grid $base$w(36) -column 1 -row 10 -padx 30 -pady 5 -sticky w;
	grid $base$w(37) -column 2 -row 10 -pady 5 -sticky nesw;
	grid $base$w(38) -column 0 -row 11 -padx 30 -pady 5 -sticky w;
	grid $base$w(39) -column 1 -row 11 -padx 30 -pady 5 -sticky w;
	grid $base$w(40) -column 0 -row 12 -padx 30 -pady 5 -sticky w;
	grid $base$w(41) -column 1 -row 12 -padx 30 -pady 5 -sticky w;
	grid $base$w(42) -column 0 -row 13 -padx 30 -pady 5 -sticky w;
	grid $base$w(43) -column 1 -row 13 -padx 30 -pady 5 -sticky ew;
	grid $base$w(44) -column 1 -row 14 -padx 30 -pady 5 -sticky w;
	grid $base$w(45) -column 2 -row 13 -pady 5 -sticky nesw;
	grid $base$w(46) -column 0 -row 14 -padx 30 -pady 5 -sticky w;
	grid $base$w(47) -column 0 -row 16 -columnspan 3 -padx 30 -sticky nesw;
	grid $base$w(58) -column 0 -row 17 -columnspan 3 -padx 30 -pady 10\
		 -sticky w;
	grid $base$w(59) -column 0 -row 18 -columnspan 3 -padx 30 -pady 5\
		 -sticky nesw;
	grid $base$w(84) -column 0 -row 15 -padx 30 -pady 5 -sticky w;

	# Grid (size:3 19) $base$w(13) = servercfgfr

	pack $base$w(13) -fill x -pady 30;
	pack $base$w(10) -in $base$w(85) -expand 1 -fill both;
	pack propagate $base$w(10) 0;
	grid $base$w(9) -column 1 -row 1 -sticky nesw;

	pack $base$w(8) -expand 1 -fill both;

	pack $base$w(2) -anchor n -expand 1 -fill x;
	pack $base$w(7) -expand 1 -fill both;

	pack $base$w(1) -expand 1 -fill x;

	# -------------------------------------------------------------------
	# Call the init method
	if {[catch {
	    ::servercfg::init $base;
	} msg]} {
	    # init does not exist.
	}

	# Additional Code
	
	$base$w(9) update;
	$base$w(8) update;

	# -------------------------------------------------------------------
	# Use the template interface for this instance
	set base [mkProc $base servercfg $args];
    }

    return $base;
}


# --------------------------------------------------------------------------- 
# non widget area, used to create 
# images     : composed or referenced bitmap objects 
# fonts      : composed font objects 
# bindgroups : not supported yet!
# ---------------------------------------------------------------------------


namespace eval servercfg {
 
    # option database items to template servercfg

    option add *Servercfg$w(1).background {lightgray};
    option add *Servercfg$w(2).background {lightgray};
    option add *Servercfg$w(3).background {lightgray};
    option add *Servercfg$w(3).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(3).foreground {black};
    option add *Servercfg$w(3).text {File};
    option add *Servercfg$w(3).underline {0};
    option add *Servercfg$w(4).background {lightgray};
    option add *Servercfg$w(4).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(4).foreground {black};
    foreach {opt param} [list  menu1accelerator {Alt-n} menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {New} menu1underline {0} menu2accelerator {Alt-o} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Open} menu2underline {0} menu4accelerator {Alt-s} menu4font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu4label\
	    {Save} menu4underline {0} menu5accelerator {Alt-a} menu5font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu5label\
	    {Save as} menu5underline {5} menu7accelerator {Alt-x}\
	    menu7font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}\
	    menu7label {Exit} menu7underline {1}] {
 
	option add *Servercfg$w(4).$opt $param; 
    }

    option add *Servercfg$w(86).background {lightgray};
    option add *Servercfg$w(86).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(86).text {SwitchTask};
    option add *Servercfg$w(86).underline {0};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {DB Config} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {buildassoc} menu3font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu3label\
	    {index_cheshire} menu4font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu4label\
	    {index_clusters}] {
 
	option add *Servercfg$w(87).$opt $param; 
    }

    option add *Servercfg$w(5).background {lightgray};
    option add *Servercfg$w(5).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(5).foreground {black};
    option add *Servercfg$w(5).text {Help};
    option add *Servercfg$w(5).underline {0};
    option add *Servercfg$w(6).background {lightgray};
    option add *Servercfg$w(6).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(6).foreground {black};
    foreach {opt param} [list  menu1font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu1label\
	    {About this tool} menu2font\
	    {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*} menu2label\
	    {Readme}] {
 
	option add *Servercfg$w(6).$opt $param; 
    }

    option add *Servercfg$w(7).background {lightgray};
    option add *Servercfg$w(8).background {lightgray};
    option add *Servercfg$w(9).background {lightgray};
    option add *Servercfg$w(10).background {lightgray};
    option add *Servercfg$w(13).background {lightgray};
    option add *Servercfg$w(14).background {lightgray};
    option add *Servercfg$w(14).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(14).foreground {black};
    option add *Servercfg$w(14).text {Preferred Message Size:};
    option add *Servercfg$w(15).background {gray95};
    option add *Servercfg$w(15).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(15).foreground {black};
    option add *Servercfg$w(16).background {lightgray};
    option add *Servercfg$w(16).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(16).foreground {black};
    option add *Servercfg$w(16).text {Maximum Record Size:};
    option add *Servercfg$w(17).background {gray95};
    option add *Servercfg$w(17).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(17).foreground {black};
    option add *Servercfg$w(18).background {lightgray};
    option add *Servercfg$w(18).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(18).foreground {black};
    option add *Servercfg$w(18).text {Implementation ID:};
    option add *Servercfg$w(19).background {gray95};
    option add *Servercfg$w(19).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(19).foreground {black};
    option add *Servercfg$w(20).background {lightgray};
    option add *Servercfg$w(20).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(20).foreground {black};
    option add *Servercfg$w(20).text {Implementation Name:};
    option add *Servercfg$w(21).background {gray95};
    option add *Servercfg$w(21).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(21).foreground {black};
    option add *Servercfg$w(22).background {lightgray};
    option add *Servercfg$w(22).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(22).foreground {black};
    option add *Servercfg$w(22).text {Implementation Version:};
    option add *Servercfg$w(23).background {gray95};
    option add *Servercfg$w(23).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(23).foreground {black};
    option add *Servercfg$w(24).background {lightgray};
    option add *Servercfg$w(24).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(24).foreground {black};
    option add *Servercfg$w(24).text {Protocol Version:};
    option add *Servercfg$w(25).background {gray95};
    option add *Servercfg$w(25).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(25).foreground {black};
    option add *Servercfg$w(26).background {lightgray};
    option add *Servercfg$w(26).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(26).foreground {black};
    option add *Servercfg$w(26).text {Options:};
    option add *Servercfg$w(27).background {gray95};
    option add *Servercfg$w(27).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(27).foreground {black};
    option add *Servercfg$w(28).background {lightgray};
    option add *Servercfg$w(28).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(28).foreground {black};
    option add *Servercfg$w(28).text {Port};
    option add *Servercfg$w(29).background {gray95};
    option add *Servercfg$w(29).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(29).foreground {black};
    option add *Servercfg$w(30).background {lightgray};
    option add *Servercfg$w(30).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(30).foreground {black};
    option add *Servercfg$w(30).text {Database Names};
    option add *Servercfg$w(31).background {gray95};
    option add *Servercfg$w(31).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(31).foreground {black};
    option add *Servercfg$w(32).background {lightgray};
    option add *Servercfg$w(32).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(32).foreground {black};
    option add *Servercfg$w(32).text {Database Directories};
    option add *Servercfg$w(33).background {lightgray};
    option add *Servercfg$w(33).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(33).foreground {black};
    option add *Servercfg$w(33).text {Database Config Files};
    option add *Servercfg$w(34).background {gray95};
    option add *Servercfg$w(34).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(34).foreground {black};
    option add *Servercfg$w(35).background {lightgray};
    option add *Servercfg$w(35).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Servercfg$w(35).foreground {black};
    option add *Servercfg$w(35).text {Browse};
    option add *Servercfg$w(36).background {gray95};
    option add *Servercfg$w(36).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(36).foreground {black};
    option add *Servercfg$w(37).background {lightgray};
    option add *Servercfg$w(37).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Servercfg$w(37).foreground {black};
    option add *Servercfg$w(37).text {Browse};
    option add *Servercfg$w(38).background {lightgray};
    option add *Servercfg$w(38).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(38).foreground {black};
    option add *Servercfg$w(38).text {Timeout};
    option add *Servercfg$w(39).background {gray95};
    option add *Servercfg$w(39).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(39).foreground {black};
    option add *Servercfg$w(40).background {lightgray};
    option add *Servercfg$w(40).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(40).foreground {black};
    option add *Servercfg$w(40).text {Log File Name};
    option add *Servercfg$w(41).background {gray95};
    option add *Servercfg$w(41).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(41).foreground {black};
    option add *Servercfg$w(42).background {lightgray};
    option add *Servercfg$w(42).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(42).foreground {black};
    option add *Servercfg$w(42).text {Result Set Directory};
    option add *Servercfg$w(43).background {gray95};
    option add *Servercfg$w(43).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(43).foreground {black};
    option add *Servercfg$w(44).background {gray95};
    option add *Servercfg$w(44).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(44).foreground {black};
    option add *Servercfg$w(45).background {lightgray};
    option add *Servercfg$w(45).font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*};
    option add *Servercfg$w(45).foreground {black};
    option add *Servercfg$w(45).text {Browse};
    option add *Servercfg$w(46).background {lightgray};
    option add *Servercfg$w(46).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(46).foreground {black};
    option add *Servercfg$w(46).text {Attribute Set ID};
    option add *Servercfg$w(47).background {lightgray};
    option add *Servercfg$w(48).background {lightgray};
    option add *Servercfg$w(48).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(48).foreground {black};
    option add *Servercfg$w(48).text {Support Named Result Set};
    option add *Servercfg$w(49).background {lightgray};
    option add *Servercfg$w(49).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(49).foreground {black};
    option add *Servercfg$w(49).text {Support Multiple Database Search};
    option add *Servercfg$w(50).background {lightgray};
    option add *Servercfg$w(50).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(50).foreground {black};
    option add *Servercfg$w(50).text {Support Type 0 Query};
    option add *Servercfg$w(51).background {lightgray};
    option add *Servercfg$w(51).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(51).foreground {black};
    option add *Servercfg$w(51).text {Support Type 1 Query};
    option add *Servercfg$w(52).background {lightgray};
    option add *Servercfg$w(52).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(52).foreground {black};
    option add *Servercfg$w(52).text {Support Type 2 Query};
    option add *Servercfg$w(53).background {lightgray};
    option add *Servercfg$w(53).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(53).foreground {black};
    option add *Servercfg$w(53).text {Support Type 100 Query};
    option add *Servercfg$w(54).background {lightgray};
    option add *Servercfg$w(54).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(54).foreground {black};
    option add *Servercfg$w(54).text {Support Type 101 Query};
    option add *Servercfg$w(55).background {lightgray};
    option add *Servercfg$w(55).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(55).foreground {black};
    option add *Servercfg$w(55).text {Support Type 102 Query};
    option add *Servercfg$w(56).background {lightgray};
    option add *Servercfg$w(56).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(56).foreground {black};
    option add *Servercfg$w(56).text {Support Element Set Names};
    option add *Servercfg$w(57).background {lightgray};
    option add *Servercfg$w(57).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(57).foreground {black};
    option add *Servercfg$w(57).text {Support Single Element Set Name};
    option add *Servercfg$w(58).background {lightgray};
    option add *Servercfg$w(58).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(58).foreground {black};
    option add *Servercfg$w(58).text {Explain Informaition About the Server};
    option add *Servercfg$w(59).background {lightgray};
    option add *Servercfg$w(60).background {lightgray};
    option add *Servercfg$w(60).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(60).foreground {black};
    option add *Servercfg$w(60).text {News};
    option add *Servercfg$w(61).background {lightgray};
    option add *Servercfg$w(61).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(61).foreground {black};
    option add *Servercfg$w(61).text {Welcome Message};
    option add *Servercfg$w(62).background {lightgray};
    option add *Servercfg$w(62).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(62).foreground {black};
    option add *Servercfg$w(62).text {Person to Contact};
    option add *Servercfg$w(63).background {lightgray};
    option add *Servercfg$w(63).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(63).foreground {black};
    option add *Servercfg$w(63).text {Contact Person Description/Title};
    option add *Servercfg$w(64).background {lightgray};
    option add *Servercfg$w(64).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(64).foreground {black};
    option add *Servercfg$w(64).text {Contact Person Address};
    option add *Servercfg$w(65).background {lightgray};
    option add *Servercfg$w(65).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(65).foreground {black};
    option add *Servercfg$w(65).text {Contact Person Email};
    option add *Servercfg$w(66).background {lightgray};
    option add *Servercfg$w(66).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(66).foreground {black};
    option add *Servercfg$w(66).text {Contact Person Phone};
    option add *Servercfg$w(67).background {lightgray};
    option add *Servercfg$w(67).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(67).foreground {black};
    option add *Servercfg$w(67).text {Server Description};
    option add *Servercfg$w(68).background {lightgray};
    option add *Servercfg$w(68).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(68).foreground {black};
    option add *Servercfg$w(68).text {Usage Restrictions};
    option add *Servercfg$w(69).background {lightgray};
    option add *Servercfg$w(69).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(69).foreground {black};
    option add *Servercfg$w(69).text {Hours Server is Available};
    option add *Servercfg$w(70).background {lightgray};
    option add *Servercfg$w(70).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(70).foreground {black};
    option add *Servercfg$w(70).text {Language used in the Server};
    option add *Servercfg$w(71).background {gray95};
    option add *Servercfg$w(71).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(71).foreground {black};
    option add *Servercfg$w(72).background {gray95};
    option add *Servercfg$w(72).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(72).foreground {black};
    option add *Servercfg$w(73).background {gray95};
    option add *Servercfg$w(73).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(73).foreground {black};
    option add *Servercfg$w(74).background {gray95};
    option add *Servercfg$w(74).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(74).foreground {black};
    option add *Servercfg$w(75).background {gray95};
    option add *Servercfg$w(75).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(75).foreground {black};
    option add *Servercfg$w(76).background {gray95};
    option add *Servercfg$w(76).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(76).foreground {black};
    option add *Servercfg$w(77).background {gray95};
    option add *Servercfg$w(77).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(77).foreground {black};
    option add *Servercfg$w(78).background {gray95};
    option add *Servercfg$w(78).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(78).foreground {black};
    option add *Servercfg$w(79).background {gray95};
    option add *Servercfg$w(79).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(79).foreground {black};
    option add *Servercfg$w(80).background {gray95};
    option add *Servercfg$w(80).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(80).foreground {black};
    option add *Servercfg$w(81).background {lightgray};
    option add *Servercfg$w(81).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(81).foreground {black};
    option add *Servercfg$w(81).text {Usage Payment Address};
    option add *Servercfg$w(82).background {gray95};
    option add *Servercfg$w(82).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(82).foreground {black};
    option add *Servercfg$w(83).background {gray95};
    option add *Servercfg$w(83).font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(83).foreground {black};
    option add *Servercfg$w(84).background {lightgray};
    option add *Servercfg$w(84).font {-*-helvetica-bold-r-normal-*-14-*-*-*-*-*-*-*};
    option add *Servercfg$w(84).foreground {black};
    option add *Servercfg$w(84).text {Support Information};
}


# --------------------------------------------------------------------------- 
# Internal used by VisualGIPSY 
# --------------------------------------------------------------------------- 
# store Environment settings:
set ::rat(WIDGETCLASS,servercfg) servercfg;
# Local used Packages:
set ::rat(PACKAGES,LOADED) [list ];
# ---------------------------------------------------------------------------

srcStartUp serverifc.tcl CODE source;
####VG-STOP####

