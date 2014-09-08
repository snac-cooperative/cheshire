# -*-tcl-*-
# Project     : StaffCheshire
# Module        : scifc.tcl
# start Date: Sunday 23 July 00:47
# last modified: Tuesday 5-21-2001
# Programmer: Yonghui Zhang 
# Organization: SIMS, University of California, Berkeley 
#
# ---------------------------------------------------------------------------
# Description: This is the dynamic part of the configurator code
# 
# ===========================================================================

set proDir [file dirname [info script]];
if {![info exists ::auto_path] || \
    [lsearch -exact $::auto_path $proDir] < 0} {
    lappend auto_path $proDir;
}

# the default directory for html doc files
set env(HTMLHOME) /home/yzhang/cheSum/cheshire/config-gui/html;

# import the runtime library of VisualGipsy
package require rtl_all;

# import the code that does the directory selection as opposed to the
# built-in file select 
package require DirSelect;

# a procedure useful for debugging
proc Call_Trace {{file stdout}} {
    puts $file "Tcl Call Trace";
    for {set x [expr [info level] -1]} {$x > 0} {incr x -1} {
	puts $file "$x: [info level $x]";
    }
}

# set or unset dbenv
proc DefineDBEnv {path} {
    global dbcfg;
    global appGlobal;
    if [info exists dbcfg(dbenv)] {
	unset dbcfg(dbenv);
	return;
    }
    fileselect $path SetEnv;
}

proc SetEnv {path dir} {
    global dbcfg;
    set dbcfg(dbenv) $dir;
}

#----------------------------------------------------------------------------
# these short procedures reset global variables (back to 1) when the user tries
# to proceed to the next or go back to the previous definition, including:
# filedef, indexdef, clusterdef, displaydef, compdef, and/or indexdef
# within a component definition 

# these variables are global because the whole program relies on
# the fact that at any given point of time, the user will only be
# working on one page of the whole interface with one set of the
# global variables (one state)
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# this is called when the user is moving between filedef's -- the top
# level subelement of <DBCONFIG>
#----------------------------------------------------------------------------
proc ResetFiledefControlVariables {} {
    global appGlobal;
    # for this filedef, reset everything back to 1
    set appGlobal(indexdefNum) 1;
    ResetIndexdefControlVariables;

    set appGlobal(clusterdefNum) 1;
    ResetClusterdefControlVariables;

    set appGlobal(displaydefNum) 1;
    ResetDisplaydefControlVariables;

    set appGlobal(componentdefNum) 1;
    ResetComponentdefControlVariables;
}

#----------------------------------------------------------------------------
# This is called when it's time to change to another indexdef
#----------------------------------------------------------------------------
proc ResetIndexdefControlVariables {} {
    global appGlobal;
    set appGlobal(indexmapNum) 1;
    set appGlobal(indexkeytagNum) 1;
    set appGlobal(indexexctagNum) 1;
}

#----------------------------------------------------------------------------
# This is called when it's time to change to another clusterdef
#----------------------------------------------------------------------------
proc ResetClusterdefControlVariables {} {
    global appGlobal;
    ResetClusmapControlVariables;
}

#----------------------------------------------------------------------------
# This is called when it's time to change to another displaydef
#----------------------------------------------------------------------------
proc ResetDisplaydefControlVariables {} {
    global appGlobal;
    set appGlobal(formatexcludetagNum) 1;
    ResetClusmapControlVariables;
}

#----------------------------------------------------------------------------
# This is called when it's time to change to another clusterdef, displaydef
#----------------------------------------------------------------------------
proc ResetClusmapControlVariables {} {
    global appGlobal;
    # starting with the first clusmap entry
    set appGlobal(clusmapentryNum) 1;
    # starting with the first entry of the following element construct 
    # <FROM><TAGSPEC>...</TAGSPEC></FROM>
    set appGlobal(clusmapfromtagNum) 1;
}

#----------------------------------------------------------------------------
# This is called when it's time to change to another componentdef
#----------------------------------------------------------------------------
proc ResetComponentdefControlVariables {} {
    global appGlobal;
    set appGlobal(compindexdefNum) 1;
    ResetIndexdefControlVariables;
}

#------------------------------------------------------------------------
# configure textvariables for the whole DBCONFIG file:
# this involves binding entries and comboboxes and other widgets of the 
# interface with variables
#------------------------------------------------------------------------
proc ConfigureDbVariables {{path .}} {
    ConfigureDbDataVariables $path;
    ConfigureDbIndexVariables $path;
    ConfigureDbClusterVariables $path;
    ConfigureDbExplainVariables $path;
    ConfigureDbDisplayVariables $path;
    ConfigureDbComponentVariables $path;
}

#-----------------------------------------------------------------------------
## this procedure lets the user define new filedef or
## navigate between the previous one and the next one
#-----------------------------------------------------------------------------
proc ChangeFileDef {{direction next}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
	if {$appGlobal(filedefNum) <= 1} {
	    return;
	}
        set incrval -1;
    }

    incr appGlobal(filedefNum) $incrval;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: Filedef\
        $appGlobal(filedefNum) -- DATA";

    # reset all global control variables to 1 for this filedef
    ResetFiledefControlVariables;

    # we always start with the data window when we change filedef
    set dbtabset [find-widget dbtabset .];
    $dbtabset setselection data;
    set appGlobal(dbcfgSaved) $oldsaved;
}    

proc CheckDefaultPath {path} {
    global dbcfg;
    global appGlobal;
    set num $appGlobal(filedefNum);
    if {[info exists dbcfg(FILEDEF${num}_0DEFAULTPATH)] &&
        $dbcfg(FILEDEF${num}_0DEFAULTPATH) != "" } {
	set dbcfg(FILEDEF${num}_0DEFAULTPATH) "";
	return;
    }
    fileselect $path SetDefaultPath;
}

proc SetDefaultPath {path dir} {
    global dbcfg;
    global appGlobal;
    set num $appGlobal(filedefNum);
    set dbcfg(FILEDEF${num}_0DEFAULTPATH) $dir;
}

#------------------------------------------------------------------------------
# hide the continuation file frame when data is in a single file 
#------------------------------------------------------------------------------
proc HideContFrame {} {
    set thiswidget [find-widget contframe .];
    [winfo parent $thiswidget] configure -relief flat -borderwidth 0;
    # hide the frame containing entry for the user to enter the
    # continuation file name
    pack forget $thiswidget;
    set bb [find-widget dataFilePathbb .];
    # now since a single file is used, the browse button should bring
    # up the TK-built-in fileselect dialog
    $bb configure -command "HandleBBSelect $bb";
}

#------------------------------------------------------------------------------
# show the continuation file frame when data is in multiple files 
#------------------------------------------------------------------------------
proc ShowContFrame {} {
    set thiswidget [find-widget contframe .];
    [winfo parent $thiswidget] configure -relief groove -borderwidth 2;
    pack $thiswidget -side top -pady 5 -anchor w;
    # the browse button should bring up the directory select dialog
    set bb [find-widget dataFilePathbb .];
    $bb configure -command "fileselect $bb HandleBBSelect";
}

#------------------------------------------------------------------------------
# this procedure configure textvariables of the entry fileds for the main
# file data definition, that is, the overall data part associated with a
# particular filedef
#------------------------------------------------------------------------------
proc ConfigureDbDataVariables {{path .}} {
    global appGlobal;
    global dbcfg;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- DATA";
    set num $appGlobal(filedefNum);    
    set parentfr [find-widget datafr $path]; 
    
    $parentfr.dpathfr.dpathentry configure -textvariable\
	    appGlobal(empty);
    set dbcfg(dpathchck_var) 0;

    $parentfr.dpathfr.dpathentry configure -textvariable\
	    dbcfg(FILEDEF${num}_0DEFAULTPATH);
    if {$dbcfg(FILEDEF${num}_0DEFAULTPATH) != ""} {
	set dbcfg(dpathchck_var) 1;
    }

    #---------------------------------------------------------------
    # control the file type combobox, default to "SGML" 
    # NOTE that this only works one way, to set the entry according
    # to the array variable or default value because the rtl_combobox
    # doesn't automatically bind variable to entry.
    # reseting the variable value will be accomplished using a
    # procedure called HandleComboSelect
    #---------------------------------------------------------------
    if {![info exists dbcfg(FILEDEF${num}_TYPE.a)]} {
        # default to "SGML"
	# to make the decision about whether to default a combobox
	# value, the followings are considered:
	# 1. If it's a attribute: generally ok to default
	# 2. if it really has default value
        $parentfr.filetypefr.dataFileTypeCombo entryset "SGML";
        set dbcfg(FILEDEF${num}_TYPE.a) "SGML";
    } else {
        $parentfr.filetypefr.dataFileTypeCombo entryset \
            $dbcfg(FILEDEF${num}_TYPE.a); 
    }

    #---------------------------------------------------------------
    # configure the variable of the radio button group:
    # single file or multiple files in a directory
    #---------------------------------------------------------------
    $parentfr.filepathfr.filepathfrup.singlebut configure \
            -variable dbcfg(FILEDEF${num}_singleormulti);
    $parentfr.filepathfr.filepathfrup.multibut configure \
            -variable dbcfg(FILEDEF${num}_singleormulti);

    if {$dbcfg(FILEDEF${num}_singleormulti) == ""} {
        # file default to a single monolithic file 
        set dbcfg(FILEDEF${num}_singleormulti) "single";
	HideContFrame;
    } elseif {$dbcfg(FILEDEF${num}_singleormulti) == "single"} {
	HideContFrame;
    } else {
        ShowContFrame;
    }

    # configure textvariable for the filetag entry:
    # NOTE: the variable: appGlobal(empty) is used to simply wipe 
    # out the value shown in an entry without affecting the
    # textvariable bound to the entry.
    # this scheme will be used for all entries
    $parentfr.nicknamefr.dataNickEntry configure \
            -textvariable appGlobal(empty);
    # the numbers used in names of variables, e.g. 2FILETAG are
    # there to enfore an order when printing out the
    # configuration file
    $parentfr.nicknamefr.dataNickEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_2FILETAG)";

    # configure textvariable for the filename entry
    $parentfr.filepathfr.filepathfrlow.dataFilePathEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.filepathfr.filepathfrlow.dataFilePathEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_1FILENAME)";

    # configure textvariable for the continuation file entry.
    # not necessary if a single file, do this anyway
    # if this entry is filled, the content of this continuation file
    # will be inserted into the final configuration file
    $parentfr.filepathfr.contframe.dataContPathEntry configure \
            -textvariable appGlobal(empty); 
    $parentfr.filepathfr.contframe.dataContPathEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_filecont)";

    # configure textvariable for the filedtd entry
    $parentfr.dtdfr.dataDtdEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.dtdfr.dataDtdEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_6FILEDTD)";

    # configure textvariable for the associator file entry
    $parentfr.assocpathfr.dataAssocPathEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.assocpathfr.dataAssocPathEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_7ASSOCFIL)";

    # configure textvariable for the history file entry
    $parentfr.historypathfr.dataHisPathEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.historypathfr.dataHisPathEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_8HISTORY)";

    # configure textvariable for the sgmlcat entry
    $parentfr.sgmlcatfr.dataSgmlCatEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.sgmlcatfr.dataSgmlCatEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_3SGMLCAT)";

    # configure textvariable for the sgmlsubdoc entry
    $parentfr.sgmlsubdocfr.dataSubDocEntry configure \
            -textvariable appGlobal(empty);
    $parentfr.sgmlsubdocfr.dataSubDocEntry configure \
            -textvariable "dbcfg(FILEDEF${num}_4SGMLSUBDOC)";

    # handle the listbox widget 
    set dispoplist [find-widget dispOptList .]; 
    # clear previous selection
    $dispoplist selection clear 0 end;
    # set current selection
    if [info exists dbcfg(FILEDEF${num}_12DISPOPTIONS)] {
        foreach indx $dbcfg(FILEDEF${num}_12DISPOPTIONS) { 
	    $dispoplist selection set $indx;
        }
    }
    # add a new binding to buttonRelease event on this list box
    # so when user make a selection or deselection, update associated variable
    bind $dispoplist <ButtonRelease-1> {
        tkCancelRepeat;
        %W activate @%x,%y;
	if {[%W nearest %y] == 1 || [%W nearest %y] == 2 || [%W \
		nearest %y] == 3} {
           %W selection clear 0;
	} elseif {[%W nearest %y] == 0} {
	    %W selection clear 1 3;
	}
        # NOTE: now we are actually in global scope 
        set outernum $appGlobal(filedefNum);
        set dbcfg(FILEDEF${outernum}_12DISPOPTIONS) [%W curselection];
    }
}

################################################################################
# the following procedures are used to configure textvariables for 
# entries in the indexdef page of the interface, create new indexkey
# entries, and pop up idxmap dialog, et al.
################################################################################

#------------------------------------------------------------------------------
# this procedure reconfigure all the variables bound with the indexdef
# NOTE: it's possible that we are working on indexdef inside 
# componentdef -- we use the same set of procedures for the
# indexdef nested in componentdef
#------------------------------------------------------------------------------
proc ConfigureDbIndexVariables {{path .} {nested {}}} {
    global appGlobal;
    global dbcfg;

    # find out which filedef we are (or will be) in
    set num $appGlobal(filedefNum);    

    # start with all indexdef-related variables set to 1
    ResetIndexdefControlVariables;

    set fn [file tail $dbcfg(savefilename)];
    if {$nested == "InsideComponent"} {
	set componentdefNum $appGlobal(componentdefNum);
	set compindexdefNum $appGlobal(compindexdefNum);
	set arrPre "FILEDEF${num}_11COMPONENTS_COMPONENTDEF";
	append arrPre "${componentdefNum}_COMPONENTINDEXES_";
	append arrPre "INDEXDEF${compindexdefNum}";
	set appGlobal(compindexdefheader) "Component Indexdef $compindexdefNum";
	set compPre "comp";
    } else {
	wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- INDEXDEF $appGlobal(indexdefNum)";
	set indexdefNum $appGlobal(indexdefNum);
	set arrPre "FILEDEF${num}_9INDEXES_INDEXDEF${indexdefNum}"; 
	set appGlobal(indexdefheader) "Index Definition $indexdefNum";
	set compPre {};
    }

    set parentfr [find-widget ${compPre}indexbuttonfr $path];

    set tmpNum [set ${compPre}indexdefNum];
    # the following instructions handle the navigation buttons for
    # indexdef, all navigation buttons are handled this way
    set backbutExist [winfo exists $parentfr.${compPre}indexBackButton];
    if {$tmpNum > 1 && !$backbutExist} {
        # need to create a new back button
        set backbut [button $parentfr.${compPre}indexBackButton -text "Prev" \
		-command "ChangeIndexDef prev $path $nested" -background lightgray \
            -font -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-* \
            -padx 10 -pady 3 -foreground blue -borderwidth 2]; 
        grid $backbut -column 0 -row 0 -padx 10 -sticky e;
    } elseif {$tmpNum == 1 && $backbutExist && \
	    [winfo ismapped $parentfr.${compPre}indexBackButton]} {
        # we are back in the first indexdef, no backing up further 
        grid forget $parentfr.${compPre}indexBackButton;
    } elseif {$tmpNum == 2 && $backbutExist && \
	    ![winfo ismapped $parentfr.${compPre}indexBackButton]} {
        # we are changing direction again and  going from first
	# indexdef to the second, the button exists but is not mapped,
	# need to remap the button on the screen
        grid $parentfr.${compPre}indexBackButton -column 0 -row 0 -padx 10 -sticky e;
    }

    set parentfr [find-widget ${compPre}indexdeffr $path];
    #----------------------------------------------------------------------
    # access type default to BTREE
    #----------------------------------------------------------------------
    if {![info exists dbcfg(${arrPre}_ACCESS.a)]} {
        $parentfr.${compPre}indexACCESSCombo entryset "BTREE";
        set dbcfg(${arrPre}_ACCESS.a) "BTREE";
    } else {
        # read the current value of this array element
        $parentfr.${compPre}indexACCESSCombo entryset \
		$dbcfg(${arrPre}_ACCESS.a); 
    }

    #----------------------------------------------------------------------
    # extract key type default to KEYWORD 
    #----------------------------------------------------------------------
    if {![info exists dbcfg(${arrPre}_EXTRACT.a)]} {
        $parentfr.${compPre}indexEXTRACTCombo entryset "KEYWORD";
        set dbcfg(${arrPre}_EXTRACT.a) "KEYWORD";
    } else {
        $parentfr.${compPre}indexEXTRACTCombo \
		entryset $dbcfg(${arrPre}_EXTRACT.a); 
    }
    
    #----------------------------------------------------------------------
    # normalization type default to NONE 
    #----------------------------------------------------------------------
    if {![info exists dbcfg(${arrPre}_NORMAL.a)]} {
        $parentfr.${compPre}indexNORMALCombo entryset "NONE";
        set dbcfg(${arrPre}_NORMAL.a) "NONE";
    } else {
        $parentfr.${compPre}indexNORMALCombo entryset \
	    $dbcfg(${arrPre}_NORMAL.a); 
    }

    #----------------------------------------------------------------------
    # primary key option default to NO
    #----------------------------------------------------------------------
    if {![info exists dbcfg(${arrPre}_PRIMARYKEY.a)]} {
        $parentfr.${compPre}indexPRIMARYKEYCombo entryset "NO";
        set dbcfg(${arrPre}_PRIMARYKEY.a) "NO";
    } else {
        $parentfr.${compPre}indexPRIMARYKEYCombo entryset \
            $dbcfg(${arrPre}_PRIMARYKEY.a); 
    }

    # the other simple entry widgets
    # configure textvariable for the indexname entry 
    $parentfr.${compPre}indexPathEntry configure -textvariable appGlobal(empty);
    $parentfr.${compPre}indexPathEntry configure -textvariable \
        "dbcfg(${arrPre}_1INDXNAME)";

    # configure textvariable for the indextag entry (alias for this index)
    $parentfr.${compPre}indexNameEntry configure -textvariable appGlobal(empty);
    $parentfr.${compPre}indexNameEntry configure -textvariable \
        "dbcfg(${arrPre}_2INDXTAG)";

    # configure textvariable for the stoplist entry 
    $parentfr.${compPre}indexStoplistEntry configure -textvariable appGlobal(empty);
    $parentfr.${compPre}indexStoplistEntry configure -textvariable \
        "dbcfg(${arrPre}_4STOPLIST)";

    # the indexkey and indexexc tagspec entry1
    # The following instructions deal with the indexkey and indexexc tagspec
    # entries. Because there could be more than one indxkey or indexexc
    # specifications or just one, we keep the first entry widget and
    # destroy the other ones (if they exist) at first, then we
    # recreate the other entry widgets only when necessary
    $parentfr.${compPre}indexKeyspecFr.${compPre}indexKeyspecEntry1 \
	    configure -textvariable appGlobal(empty);
    $parentfr.${compPre}indexKeyspecFr.${compPre}indexKeyspecEntry1 \
	    configure -textvariable "dbcfg(${arrPre}_5INDXKEY_1TAGSPEC)";

    $parentfr.${compPre}indexExcludeFr.${compPre}indexExcspecEntry1 \
	    configure -textvariable appGlobal(empty);
    $parentfr.${compPre}indexExcludeFr.${compPre}indexExcspecEntry1 \
	    configure -textvariable "dbcfg(${arrPre}_6INDXEXC_1TAGSPEC)";


    # destroy all entries except for the first one, which will always be there
    foreach w [winfo children $parentfr.${compPre}indexKeyspecFr] {
        set domatch [regexp {\D(\d+)$} $w match digit];
        if {$domatch == 1 && $digit > 1} {
            destroy $w;
        }
    }

    foreach w [winfo children $parentfr.${compPre}indexExcludeFr] {
        set domatch [regexp {\D(\d+)$} $w match digit];
        if {$domatch == 1 && $digit > 1} {
            destroy $w;
        }
    }

    # now, only add entries if there are already textvariables 
    set enum 2;
    set arrPre1 "${arrPre}_5INDXKEY_";
    while {[info exists dbcfg(${arrPre1}${enum}TAGSPEC)] && \
	    $dbcfg(${arrPre1}${enum}TAGSPEC) != ""} {
        set ent [entry \
	$parentfr.${compPre}indexKeyspecFr.${compPre}indexKeyspecEntry$enum \
                -width 60 -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
        $ent configure -textvariable dbcfg(${arrPre1}${enum}TAGSPEC);
        grid $ent -row $enum -column 0 -padx 10 -pady 5 -sticky ew; 
	# now a delete button 
	set but [button \
	$parentfr.${compPre}indexKeyspecFr.${compPre}indexKeyspecEntryButton$enum \
	        -text X -padx 2 -pady 2];
	$but configure -command "DeleteAnIndxXXXEntry $but $arrPre1 key";
	grid $but -row $enum -column 1 -pady 5 -sticky w;
	set appGlobal(indexkeytagNum) $enum;
        incr enum;
    }

    set enum 2;
    set arrPre2 "${arrPre}_6INDXEXC_";
    while {[info exists dbcfg(${arrPre2}${enum}TAGSPEC)] && \
	    $dbcfg(${arrPre2}${enum}TAGSPEC) != ""} {
        set ent [entry \
	$parentfr.${compPre}indexExcludeFr.${compPre}indexExcspecEntry$enum \
                -width 60 -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
        $ent configure -textvariable dbcfg(${arrPre2}${enum}TAGSPEC);
        grid $ent -row $enum -column 0 -padx 10 -pady 5 -sticky ew; 
	# now a delete button 
	set but [button \
	$parentfr.${compPre}indexExcludeFr.${compPre}indexExcspecEntryButton$enum \
	        -text X -padx 2 -pady 2];
	$but configure -command "DeleteAnIndxXXXEntry $but $arrPre2 exc";
	grid $but -row $enum -column 1 -pady 5 -sticky w;
	set appGlobal(indexexctagNum) $enum;
        incr enum;
    }
}

#-------------------------------------------------------------------------------
# this procedure allows the user to create new indexdef as well as navigate 
# between previous and next index definition. 
# argument: the direction of the navigation
#-------------------------------------------------------------------------------
proc ChangeIndexDef {{direction next} {path .} {nested {}}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }
    if {$nested == ""} {
	incr appGlobal(indexdefNum) $incrval;
    } elseif {$nested == "InsideComponent"} {
	incr appGlobal(compindexdefNum) $incrval;
    }

    ConfigureDbIndexVariables $path $nested;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#----------------------------------------------------------------------------
## this procedure addes a indexkey spec entry for current index definition 
#----------------------------------------------------------------------------
proc MoreIndexKeySpec {{nested {}}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    #------------------------------------------------------------
    ## find out which filedef we are working on 
    #------------------------------------------------------------
    set filedefNum $appGlobal(filedefNum); 
    #------------------------------------------------------------
    ## increment the number of last indexkey tagspec entry 
    #------------------------------------------------------------
    set num [incr appGlobal(indexkeytagNum)];

    if {$nested == "InsideComponent"} {
	set componentdefNum $appGlobal(componentdefNum);
	set compindexdefNum $appGlobal(compindexdefNum);
	set arrPre "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
	append arrPre "${componentdefNum}_COMPONENTINDEXES_";
	append arrPre "INDEXDEF${compindexdefNum}";
	set compPre "comp";
    } else {
	set indexdefNum $appGlobal(indexdefNum);
	set arrPre "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	set compPre {};
    }

    append arrPre "_5INDXKEY_";

    set fr [find-widget ${compPre}indexKeyspecFr .];
    if {![winfo exists $fr.${compPre}indexKeyspecEntry$num]} {
        set ent [entry $fr.${compPre}indexKeyspecEntry$num -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
	# now a delete button 
	set but [button $fr.${compPre}indexKeyspecEntryButton$num -text X -padx \
	2 -pady 2];
	$but configure -command "DeleteAnIndxXXXEntry $but $arrPre key";
    } else {
        set ent $fr.${compPre}indexKeyspecEntry$num;
	set but $fr.${compPre}indexKeyspecEntryButton$num;
    }
    $ent configure -textvariable dbcfg(${arrPre}${num}TAGSPEC);
    grid $ent -row $num -column 0 -padx 10 -pady 5 -sticky ew;   
    grid $but -row $num -column 1 -pady 5 -sticky w;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#----------------------------------------------------------------------------
## this procedure addes a indxexc spec entry for current index definition 
#----------------------------------------------------------------------------
proc MoreIndexExcSpec {{nested {}}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    #------------------------------------------------------------
    ## find out which filedef we are working on 
    #------------------------------------------------------------
    set filedefNum $appGlobal(filedefNum); 
    #------------------------------------------------------------
    ## increment the number of last indexexc tagspec entry 
    #------------------------------------------------------------
    set num [incr appGlobal(indexexctagNum)];

    if {$nested == "InsideComponent"} {
	set componentdefNum $appGlobal(componentdefNum);
	set compindexdefNum $appGlobal(compindexdefNum);
	set arrPre "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
	append arrPre "${componentdefNum}_COMPONENTINDEXES_";
	append arrPre "INDEXDEF${compindexdefNum}";
	set compPre "comp";
    } else {
	set indexdefNum $appGlobal(indexdefNum);
	set arrPre "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	set compPre {};
    }

    append arrPre "_6INDXEXC_";

    set fr [find-widget ${compPre}indexExcludeFr .];
    if {![winfo exists $fr.${compPre}indexExcspecEntry$num]} {
        set ent [entry $fr.${compPre}indexExcspecEntry$num -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
	# now a delete button 
	set but [button $fr.${compPre}indexExcspecEntryButton$num -text X -padx \
	2 -pady 2];
	$but configure -command "DeleteAnIndxXXXEntry $but $arrPre exc";
    } else {
        set ent $fr.${compPre}indexExcspecEntry$num;
	set but $fr.${compPre}indexExcspecEntryButton$num;
    }
    $ent configure -textvariable dbcfg(${arrPre}${num}TAGSPEC);
    grid $ent -row $num -column 0 -padx 10 -pady 5 -sticky ew;   
    grid $but -row $num -column 1 -pady 5 -sticky w;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
# This procedure deletes an entry if the user click on the delete
# button beside this entry widget, 
#-------------------------------------------------------------------------------
proc DeleteAnIndxXXXEntry {path varPrefix what} {
    global dbcfg;
    global appGlobal;
    regexp {^(.+\D)(\d+)$} $path match head num;
    regsub {Button} $head "" entry;
   
    # now delete the entry, button, and associated variable
    set biggerNum [expr $num + 1];
    while {[info exists dbcfg(${varPrefix}${biggerNum}TAGSPEC)]} {
	set dbcfg(${varPrefix}${num}TAGSPEC) \
		$dbcfg(${varPrefix}${biggerNum}TAGSPEC);
	incr num;
	incr biggerNum;
    }
    # need to destroy the widget first to unset the associated textvariable
    destroy $head$num;
    destroy $entry$num;
    # to enable the save button
    set dbcfg(${varPrefix}${num}TAGSPEC) "";
    unset dbcfg(${varPrefix}${num}TAGSPEC);
    incr appGlobal(index${what}tagNum) -1;
}
 
#-------------------------------------------------------------------------------
## This procedure pops up a window for the user to define an indxmap
#-------------------------------------------------------------------------------
proc IdxMapCreateEntries {{nested {}}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    set idxmapNum $appGlobal(indexmapNum);
    set top [toplevel .idxmapEntries];

    set fr [frame $top.indexmapfr];
    label $fr.attributesetLbl -text "ATTRIBUTESET";      
    label $fr.useLbl -text "USE";      
    label $fr.relationLbl -text "RELATION";      
    label $fr.positionLbl -text "POSITION";      
    label $fr.structureLbl -text "STRUCTURE";      
    label $fr.truncationLbl -text "TRUNCATION";      
    label $fr.completenessLbl -text "COMPLETENESS";      

    # the attribute set combobox
    # NOTE: $nested is appended to the combobox widgets because it's
    # necessary when doing HandleComboSelect
    set comb0 [rtl_combobox $fr.attributesetCombo$nested -clientbackground gray95];
    $comb0 configure -selectcommand "HandleComboSelect $comb0";
    $comb0 configure -command "HandleComboSelect $comb0";
    $comb0 histinsert 1 "BIB-1";
    $comb0 histinsert 2 "EXP-1";
    $comb0 histinsert 3 "EXT-1";
    $comb0 histinsert 4 "GILS";

    # the USE entry
    entry $fr.useEntry -background gray95;

    # the RELATION combobox
    set comb1 [rtl_combobox $fr.relationCombo$nested -clientbackground gray95];
    $comb1 configure -selectcommand "HandleComboSelect $comb1";
    $comb1 configure -command "HandleComboSelect $comb1";
    $comb1 histinsert 1 "less than";
    $comb1 histinsert 2 "less or equal";
    $comb1 histinsert 3 "equal";
    $comb1 histinsert 4 "greater or equal";
    $comb1 histinsert 5 "greater than";
    $comb1 histinsert 6 "not equal";
    $comb1 histinsert 7 "phonetic";
    $comb1 histinsert 8 "stem";
    $comb1 histinsert 9 "relevance";
    $comb1 histinsert 10 "always matches";

    # the POSITION combobox
    set comb2 [rtl_combobox $fr.positionCombo$nested -clientbackground gray95];
    $comb2 configure -selectcommand "HandleComboSelect $comb2";
    $comb2 configure -command "HandleComboSelect $comb2";
    $comb2 histinsert 1 "first in field";
    $comb2 histinsert 2 "first in subfield";
    $comb2 histinsert 3 "any position in field";

    # the STRUCTURE combobox
    set comb3 [rtl_combobox $fr.structureCombo$nested -clientbackground gray95];
    $comb3 configure -selectcommand "HandleComboSelect $comb3";
    $comb3 configure -command "HandleComboSelect $comb3";
    $comb3 histinsert 1 "phrase";
    $comb3 histinsert 2 "word";
    $comb3 histinsert 3 "key";
    $comb3 histinsert 4 "year";
    $comb3 histinsert 5 "date (normalized)";
    $comb3 histinsert 6 "word list";
    $comb3 histinsert 7 "date (un-normalized)";
    $comb3 histinsert 8 "name (normalized)";
    $comb3 histinsert 9 "name (un-normalized)";
    $comb3 histinsert 10 "structure";
    $comb3 histinsert 11 "urx";
    $comb3 histinsert 12 "free-form-text";
    $comb3 histinsert 13 "document-text";
    $comb3 histinsert 14 "local number";
    $comb3 histinsert 15 "string";
    $comb3 histinsert 16 "numeric string";

    # the TRUNCATION combobox
    set comb4 [rtl_combobox $fr.truncationCombo$nested -clientbackground gray95];
    $comb4 configure -selectcommand "HandleComboSelect $comb4";
    $comb4 configure -command "HandleComboSelect $comb4";
    $comb4 histinsert 1 "right truncation";
    $comb4 histinsert 2 "left truncation";
    $comb4 histinsert 3 "left and right";
    $comb4 histinsert 4 "do not truncate";
    #$comb4 histinsert 5 "Process #";
    #$comb4 histinsert 6 "Glob(regExpr-1)";
    #$comb4 histinsert 7 "Regexp(regExpr-2)";

    # the COMPLETENESS combobox
    set comb5 [rtl_combobox $fr.completenessCombo$nested -clientbackground gray95];
    $comb5 configure -selectcommand "HandleComboSelect $comb5";
    $comb5 configure -command "HandleComboSelect $comb5";
    $comb5 histinsert 1 "incomplete subfield";
    $comb5 histinsert 2 "complete subfield";
    $comb5 histinsert 3 "complete field";

    grid $fr.attributesetLbl -column 0 -row 0 -sticky w -padx 20 -pady 5;
    grid $fr.attributesetCombo$nested -column 1 -row 0 -sticky e -padx 20 -pady 5;
    grid $fr.useLbl -column 0 -row 1 -sticky w -padx 20 -pady 5;
    grid $fr.useEntry -column 1 -row 1 -sticky we -padx 20 -pady 5;
    grid $fr.relationLbl -column 0 -row 2 -sticky w -padx 20 -pady 5;
    grid $fr.relationCombo$nested -column 1 -row 2 -sticky e -padx 20 -pady 5;
    grid $fr.positionLbl -column 0 -row 3 -sticky w -padx 20 -pady 5;
    grid $fr.positionCombo$nested -column 1 -row 3 -sticky e -padx 20 -pady 5;
    grid $fr.structureLbl -column 0 -row 4 -sticky w -padx 20 -pady 5;
    grid $fr.structureCombo$nested -column 1 -row 4 -sticky e -padx 20 -pady 5;
    grid $fr.truncationLbl -column 0 -row 5 -sticky w -padx 20 -pady 5;
    grid $fr.truncationCombo$nested -column 1 -row 5 -sticky e -padx 20 -pady 5;
    grid $fr.completenessLbl -column 0 -row 6 -sticky w -padx 20 -pady 5;
    grid $fr.completenessCombo$nested -column 1 -row 6 -sticky e -padx 20 -pady 5;

    button $fr.next -text Next -command "ChangeIdxMapEntry next $nested";
    button $fr.done -text Done -command DoneIdxMapEntry;
    grid $fr.done -column 1 -row 7 -sticky w -padx 10 -pady 5;
    grid $fr.next -column 1 -row 7 -sticky e -padx 10 -pady 5;
    pack $fr;  
    grab $fr;
    ConfigureIdxMapVariables $nested;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
# this procedure allows the user to create new indxmap as well as navigate 
# between previous and next idxmap definition 
# argument: the direction of the navigation
#-------------------------------------------------------------------------------
proc ChangeIdxMapEntry {{direction next} {nested {}}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }
    incr appGlobal(indexmapNum) $incrval;
    ConfigureIdxMapVariables $nested;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
# this procedure simply closes the idxmap dialog
#-------------------------------------------------------------------------------
proc DoneIdxMapEntry {} {
    destroy .idxmapEntries;
}

#-------------------------------------------------------------------------------
# this procedure configures textvariables for the popup indexmap dialog
#-------------------------------------------------------------------------------
proc ConfigureIdxMapVariables {{nested {}}} {
    global appGlobal;
    global dbcfg;
    set num $appGlobal(filedefNum);
    set idxmapNum $appGlobal(indexmapNum);

    if {$nested == "InsideComponent"} {
	set pre "Component ";
    } else {
	set pre {};
    }
    wm title .idxmapEntries "${pre}Indxmap Definition $appGlobal(indexmapNum)";

    if {$nested == "InsideComponent"} {
	set compindexdefNum $appGlobal(compindexdefNum);
	set componentdefNum $appGlobal(componentdefNum);
	set arrPre "FILEDEF${num}_11COMPONENTS_COMPONENTDEF";
	append arrPre "${componentdefNum}_COMPONENTINDEXES_";
	append arrPre "INDEXDEF${compindexdefNum}";
	set compPre "comp";
    } else {
	set indexdefNum $appGlobal(indexdefNum);
	set arrPre "FILEDEF${num}_9INDEXES_INDEXDEF${indexdefNum}"; 
	set compPre {};
    }

    set arrPre "${arrPre}_3INDXMAP${idxmapNum}"

    # deal with the navigation buttons
    set fr .idxmapEntries.indexmapfr;
    set backbutExist [winfo exists $fr.indexmapBackButton]; 
    if {$idxmapNum > 1 && !$backbutExist} {
        set backbut [button $fr.indexmapBackButton -text "Prev" \
            -command "ChangeIdxMapEntry prev $nested"]
        grid $backbut -column 0 -row 7 -sticky w -padx 10 -pady 5;
    } elseif {$idxmapNum == 1 && $backbutExist && \
            [winfo ismapped $fr.indexmapBackButton]} {
        grid forget $fr.indexmapBackButton;
    } elseif {$idxmapNum == 2 && $backbutExist && \
            ![winfo ismapped $fr.indexmapBackButton]} {
        grid $fr.indexmapBackButton -column 0 -row 7 -padx 10 -pady 5 -sticky w;
    }

    if {![info exists dbcfg(${arrPre}_ATTRIBUTESET.a)]} {
        # default to "BIB-1"
        $fr.attributesetCombo$nested entryset "BIB-1";
        set dbcfg(${arrPre}_ATTRIBUTESET.a) "BIB-1";
    } else {
        $fr.attributesetCombo$nested entryset $dbcfg(${arrPre}_ATTRIBUTESET.a); 
    }

    $fr.useEntry configure -textvariable appGlobal(empty); 
    $fr.useEntry configure -textvariable "dbcfg(${arrPre}_1USE)"; 

    if {[info exists dbcfg(${arrPre}_3POSIT)]} {
        $fr.positionCombo$nested entryset "$dbcfg(${arrPre}_3POSIT)"; 
    } else {
        $fr.positionCombo$nested entryset ""; 
    }

    if {[info exists dbcfg(${arrPre}_4STRUCT)]} {
        $fr.structureCombo$nested entryset "$dbcfg(${arrPre}_4STRUCT)"; 
    } else {
        $fr.structureCombo$nested entryset ""; 
    }

    if {[info exists dbcfg(${arrPre}_2RELAT)]} {
        $fr.relationCombo$nested entryset "$dbcfg(${arrPre}_2RELAT)"; 
    } else {
        $fr.relationCombo$nested entryset ""; 
    }

    if {[info exists dbcfg(${arrPre}_5TRUNC)]} {
        $fr.truncationCombo$nested entryset "$dbcfg(${arrPre}_5TRUNC)"; 
    } else {
        $fr.truncationCombo$nested entryset ""; 
    }

    if {[info exists dbcfg(${arrPre}_6COMPLET)]} {
        $fr.completenessCombo$nested entryset "$dbcfg(${arrPre}_6COMPLET)"; 
    } else {
        $fr.completenessCombo$nested entryset ""; 
    }
}

##############################################################################
## the following procedures are used to define component variables 
##############################################################################

#-----------------------------------------------------------------------------
## this procedure configures variables for a componentdef
#-----------------------------------------------------------------------------
proc ConfigureDbComponentVariables {{path .}} {
    global appGlobal;
    global dbcfg;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- COMPONENTDEF $appGlobal(componentdefNum)";

    set num $appGlobal(filedefNum);
    set componentdefNum $appGlobal(componentdefNum);
    set appGlobal(compdefheader) "Component Definition $componentdefNum";

    # start with all componentdef related control variables set to 1
    ResetComponentdefControlVariables;

    # takes care of navigation buttons
    set parentfr [find-widget componentbuttonfr $path];
    set backbutExist [winfo exists $parentfr.componentBackButton];
    if {$componentdefNum > 1 && !$backbutExist} {
        set backbut [button $parentfr.componentBackButton -text "Prev" \
            -command {ChangeComponentDef prev} -background lightgray \
            -font -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-* \
            -padx 10 -pady 3 -borderwidth 2 -foreground blue]; 
        grid $backbut -column 0 -row 0 -padx 10 -sticky e;
    } elseif {$componentdefNum == 1 && $backbutExist && \
                [winfo ismapped $parentfr.componentBackButton]} {
        grid forget $parentfr.componentBackButton;
    } elseif {$componentdefNum == 2 && $backbutExist && \
                ![winfo ismapped $parentfr.componentBackButton]} {
        grid $parentfr.componentBackButton -column 0 -row 0 -padx 10 -sticky e;
    }

    set parentfr [find-widget componentfr $path];
    set arrPre "FILEDEF${num}_11COMPONENTS_COMPONENTDEF${componentdefNum}_";

    # bind the compnameentry widget to a textvariable
    $parentfr.compnameentry configure -textvariable appGlobal(empty);
    $parentfr.compnameentry configure -textvariable \
	    dbcfg(${arrPre}1COMPONENTNAME);

    # set the entry value for the component normalization combobox
    if [info exists dbcfg(${arrPre}2COMPONENTNORM)] {
        $parentfr.compNormCombo entryset $dbcfg(${arrPre}2COMPONENTNORM); 
    } else {
        $parentfr.compNormCombo entryset "";
    }

    # bind the compstarttagEntry to textvariable
    $parentfr.compstarttagEntry configure -textvariable appGlobal(empty);
    $parentfr.compstarttagEntry configure -textvariable \
	    dbcfg(${arrPre}3COMPONENTSTARTTAG_TAGSPEC);
   
    # bind the compendttagEntry to textvariable
    $parentfr.compendtagEntry configure -textvariable appGlobal(empty);
    $parentfr.compendtagEntry configure -textvariable \
	    dbcfg(${arrPre}4COMPONENTENDTAG_TAGSPEC);

    # Now configure the indexes for this componetdef
    set appGlobal(compindexdefNum) 1;
    ConfigureDbIndexVariables $path InsideComponent;
}

#----------------------------------------------------------------------------
## this procedure starts the definition for a new component or
## navigates between previous and next definition
#----------------------------------------------------------------------------
proc ChangeComponentDef {{direction next}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }
    incr appGlobal(componentdefNum) $incrval;
    ConfigureDbComponentVariables;
    set appGlobal(dbcfgSaved) $oldsaved;
}



################################################################################
## the following procedures are used to define cluster variables 
################################################################################

#-------------------------------------------------------------------------------
# this procedure configures textvariables for the cluster definition
#-------------------------------------------------------------------------------
proc ConfigureDbClusterVariables {{path .}} {
    global appGlobal;
    global dbcfg;

    set num $appGlobal(filedefNum);
    set basefr [find-widget clusbasefr .];
    set clusternamefr [find-widget clusternamefr .];
    set fn [file tail $dbcfg(savefilename)];
    if {[info exists dbcfg(FILEDEF${num}_TYPE.a)] &&
    $dbcfg(FILEDEF${num}_TYPE.a) == "CLUSTER"} {
	wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- CLUSBASE";
	grid forget $clusternamefr;
	grid $basefr -column 0 -row 0 -pady 30 -padx 50 -sticky news;
	$basefr.clusbaseent configure -textvariable appGlobal(empty);
	$basefr.clusbaseent configure -textvariable \
		dbcfg(FILEDEF${num}_10CLUSTERS_CLUSBASE);
		#dbcfg(FILEDEF${num}_10CLUSTERS_CLUSTERDEF1_CLUSBASE);
	return;
    }

    grid forget $basefr;
    grid $clusternamefr -row 0 -column 0 -sticky news;

    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- CLUSTERDEF $appGlobal(clusterdefNum)";
        
    set clusterdefNum $appGlobal(clusterdefNum); 
    set arrayPrefix "FILEDEF${num}_10CLUSTERS_CLUSTERDEF${clusterdefNum}"; 
    set appGlobal(clusterdefheader) "Cluster Definition $clusterdefNum";
    
    # start with all clusterdef related control variables set to 1
    ResetClusterdefControlVariables;

    # handle the navigation buttons for clusterdef 
    set parentfr [find-widget clusterbuttonfr $path];
    set backbutExist [winfo exists $parentfr.clusterBackButton];
    if {$clusterdefNum > 1 && !$backbutExist} {
        set backbut [button $parentfr.clusterBackButton -text "Prev" \
            -command {ChangeClusterDef prev} -background lightgray \
            -font -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-* \
            -padx 10 -pady 3 -borderwidth 2 -foreground blue]; 
        grid $backbut -column 0 -row 0 -padx 10 -sticky e;
    } elseif {$clusterdefNum == 1 && $backbutExist && \
                [winfo ismapped $parentfr.clusterBackButton]} {
        grid forget $parentfr.clusterBackButton;
    } elseif {$clusterdefNum == 2 && $backbutExist && \
                ![winfo ismapped $parentfr.clusterBackButton]} {
        grid $parentfr.clusterBackButton -column 0 -row 0 -padx 10 -sticky e;
    }

    set parentfr [find-widget clusterdeffr $path];
    # configure clusname entry for this clusterdef
    $parentfr.clusterNameEntry configure -textvariable appGlobal(empty);
    $parentfr.clusterNameEntry configure -textvariable \
        "dbcfg(${arrayPrefix}_1CLUSNAME)";

    # the normalization attribute of cluskey
    if {![info exists dbcfg(${arrayPrefix}_2CLUSKEY_NORMAL.a)]} {
        # cluskey normalization type default to NONE
        $parentfr.clusterkeyfr.clusterKeyNormalCombo entryset "NONE";
	set dbcfg(${arrayPrefix}_2CLUSKEY_NORMAL.a) "NONE";
    } else {
        $parentfr.clusterkeyfr.clusterKeyNormalCombo entryset \
        $dbcfg(${arrayPrefix}_2CLUSKEY_NORMAL.a); 
    }

    # configure stoplist entry for this clusterdef
    $parentfr.clusterStoplistEntry configure -textvariable appGlobal(empty);
    $parentfr.clusterStoplistEntry configure -textvariable \
            "dbcfg(${arrayPrefix}_4STOPLIST)";

    # configure the first tagspec entry
    $parentfr.clusterkeyfr.cluskeyTagspecFr.cluskeyTagspecEntry1 configure \
        -textvariable appGlobal(empty);
    $parentfr.clusterkeyfr.cluskeyTagspecFr.cluskeyTagspecEntry1 configure \
        -textvariable dbcfg(${arrayPrefix}_2CLUSKEY_TAGSPEC);

    # destroy all entries except for the first one, which will always be there
    #set thisFr $parentfr.clusterkeyfr.cluskeyTagspecFr;
    #foreach w [winfo children $thisFr] {
    #    set domatch [regexp {\D(\d+)$} $w match digit];
    #    if {$domatch == 1 && $digit > 1} {
    #        destroy $w;
    #    }
    #}

    # now, add entries if there are already textvariables 
    #set enum 2;
    #while {[info exists dbcfg(${arrayPrefix}_2CLUSKEY_${enum}TAGSPEC)]} {
    #    set ent [entry $thisFr.cluskeyTagspecEntry$enum \
    #            -width 60 -background gray95 \
    #            -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
    #    $ent configure -textvariable dbcfg(${arrayPrefix}_2CLUSKEY_${enum}TAGSPEC);
    #    grid $ent -row $enum -column 0 -columnspan 2 -padx 20 -pady 5 \
    #        -sticky nsew; 
    #    incr enum;
    #}
}

#-----------------------------------------------------------------------------
## this procedure lets user define new cluster def and navigate back and forth 
#-----------------------------------------------------------------------------
proc ChangeClusterDef {{direction next}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }

    incr appGlobal(clusterdefNum) $incrval;
    ConfigureDbClusterVariables;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
##this procedure addes a cluskey spec entry for current cluster definition 
#### !!!! not needed since in each clusterdef, there could only be one
#cluskey tagspec ???
#-------------------------------------------------------------------------------
#proc MoreClusKeySpec {} {
#    global appGlobal;
#    global dbcfg;
#    #------------------------------------------------------------
#    ## find out which filedef we are in
#    #------------------------------------------------------------
#    set filedefNum $appGlobal(filedefNum); 
#
#    #------------------------------------------------------------
#    ## find out which clusterdef we are working on 
#    #------------------------------------------------------------
#    set clusterdefNum $appGlobal(clusterdefNum);
#
#    #------------------------------------------------------------
#    ## increment the number of last cluskey tagspec entry 
#    #------------------------------------------------------------
#    set num [incr appGlobal(clusterkeytagNum)];
#
#    set arrPrefix "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF${clusterdefNum}";
#    set arrPrefix "${arrPrefix}_2CLUSKEY_";
#    set fr [find-widget cluskeyTagspecFr .];
#    if {![winfo exists $fr.cluskeyTagspecEntry$num]} {
#        set ent [entry $fr.cluskeyTagspecEntry$num -width 60 -background \
#            gray95 -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
#    } else {
#        set ent $fr.cluskeyTagspecEntry$num;
#    }
#    $ent configure -textvariable dbcfg($arrPrefix${num}TAGSPEC);
#    grid $ent -row $num -column 0 -columnspan 2 -padx 20 -pady 5 -sticky nsew; 
#}


###############################################################################
## The following procedures deal with clusmap creation 
## It takes an argument indicating whether it's defining a clusmap for 
## a clusterdef (default)
## or a clusmap for the convert function of a displaydef
###############################################################################

#------------------------------------------------------------------------------
## this procedure pops up a window for the user to create clusmap entries
#------------------------------------------------------------------------------
proc ClusMapCreateEntries {{context cluster}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    set top [toplevel .clusmaptop];
    wm title $top "Clusmap entry $appGlobal(clusmapentryNum)"; 
    set fr [frame $top.clusmapmainfr];

    #---------------------------------------------------------------------------
    ## a sub frame that lets user enter "FROM" info 
    #---------------------------------------------------------------------------
    set fromFr [frame $fr.fromFr -relief groove -borderwidth 2];
    set fromLbl [label $fr.fromFr.fromLbl -text "FROM: <tagspec>"];
    set fromtagspecEntry1 [entry $fromFr.fromtagspecEntry1 \
                       -width 50 -background gray95 \
                       -font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*}];
    set frommorebut [button $fromFr.frommorebut -text More -command \
                            "MoreClusmapFromTagEntry $context"];
    grid $fromLbl -row 0 -column 0 -padx 10 -pady 2  -sticky w; 
    grid $frommorebut -row 0 -column 0 -padx 10 -pady 2 -sticky e; 
    grid $fromtagspecEntry1 -row 1 -column 0  -padx 10 -pady 2 -sticky ew; 

    #---------------------------------------------------------------------------
    ## a sub frame that lets user enter "TO" info 
    #---------------------------------------------------------------------------
    set tofr [frame $fr.tofr -relief groove -borderwidth 2];
    set toLbl [label $tofr.toLbl -text "TO: <tagspec>"];
    set toFtagEntry [entry $tofr.toFtagEntry -width 30 -background gray95];
    set todumfr [frame $tofr.dum -width 40 -height 10];
    grid $toLbl -column 0 -row 0 -padx 10 -pady 2 -sticky w;
    grid $toFtagEntry -column 1 -row 0 -padx 10 -pady 2 -sticky w;
    grid $todumfr -column 2 -row 0 -sticky ew;

    if {$context == "cluster"} {
    #---------------------------------------------------------------------------
    ## a sub frame that lets user enter optional summary information
    ## only needed for cluster clusmap entries
    #---------------------------------------------------------------------------
        set sumfr [frame $fr.sumfr -borderwidth 2 -relief groove];
        set sumLbl [label $sumfr.sumLbl -text "SUMMARIZE (optional)"]; 
        set maxnumLbl [label $sumfr.maxnumLbl -text "MAXNUM"];
        set sumFtagLbl [label $sumfr.sumFtagLbl -text "TAGSPEC"];
        set maxnumEntry [entry $sumfr.maxnumEntry -background gray95];
        set sumFtagEntry [entry $sumfr.sumFtagEntry -background gray95];
        grid $sumLbl -column 0 -row 0 -columnspan 2 -padx 10 -pady 5 -sticky w;
        grid $maxnumLbl -column 0 -row 1 -padx 10 -pady 2 -sticky w;
        grid $sumFtagLbl -column 0 -row 2 -padx 10 -pady 2 -sticky w;
        grid $maxnumEntry -column 1 -row 1 -padx 10 -pady 2 -sticky ew;
        grid $sumFtagEntry -column 1 -row 2 -padx 10 -pady 2 -sticky ew;
        grid $sumfr -column 0 -row 2 -padx 30 -pady 5 -sticky we;
    }

    #---------------------------------------------------------------------------
    ## a sub frame that holds navigational buttons 
    #---------------------------------------------------------------------------
    set butfr [frame $fr.butfr];
    set nextbut [button $butfr.clusmapNextButton -text "Next" -command \
                        "ChangeClusmapEntry $context next"]; 
    set donebut [button $butfr.clusmapDoneButton -text "Done" -command \
                        DoneClusmapEntry];
    grid $donebut -column 1 -row 0 -pady 10 -sticky w;
    grid $nextbut -column 2 -row 0 -pady 10 -sticky w;

    grid $fromFr -column 0 -row 0 -padx 30 -pady 5 -sticky we;
    grid $tofr -column 0 -row 1 -padx 30 -pady 5 -sticky we;
    grid $butfr -column 0 -row 3 -padx 10 -pady 5 -sticky ew;

    ## now configure all the text variables for this new popup window
    ConfigureClusmapVariables $context;
    pack $fr;
    grab $fr;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#------------------------------------------------------------------------------
## this procedure configure the textvariables in the clusmap popup dialog 
#------------------------------------------------------------------------------
proc ConfigureClusmapVariables {{context cluster}} {
    global appGlobal;
    global dbcfg;
    set num $appGlobal(filedefNum);    

    if {$context == "cluster"} {
        set clusterdefNum $appGlobal(clusterdefNum);
	set titlepre "Cluster";
    } elseif {$context == "display"} {
        set displaydefNum $appGlobal(displaydefNum);
	set titlepre "Display";
    }

    set clusmapentryNum $appGlobal(clusmapentryNum);
    wm title .clusmaptop "$titlepre Clusmap Entry $appGlobal(clusmapentryNum)";

    # start with the clusmapfromtagNum set to 1
    set appGlobal(clusmapfromtagNum) 1;

    set mainfr .clusmaptop.clusmapmainfr;

    # deal with the navigation button
    set butfr .clusmaptop.clusmapmainfr.butfr;
    set backbutExist [winfo exists $butfr.clusmapBackButton];

    if {$clusmapentryNum > 1 && !$backbutExist} {
        set backbut [button $butfr.clusmapBackButton -text "Back" \
                        -command "ChangeClusmapEntry $context prev"];
        grid $backbut -column 0 -row 0 -pady 10 -sticky w;
    } elseif {$clusmapentryNum == 1 && $backbutExist && \
                [winfo ismapped $butfr.clusmapBackButton]} {
        grid forget $butfr.clusmapBackButton;
    } elseif {$clusmapentryNum == 2 && $backbutExist && \
                ![winfo ismapped $butfr.clusmapBackButton]} {
        grid $butfr.clusmapBackButton -column 0 -pady 10 -row 0 -sticky w;
    }

    ## from ##
    ## call another procedure to deal with the "from" frame
    ConfigureClusmapFromEntryVariables $context;

    ## summarize ##
    if {$context == "cluster"} {
        set arrPre "FILEDEF${num}_10CLUSTERS_CLUSTERDEF${clusterdefNum}";
        set arrPre "${arrPre}_5CLUSMAP${clusmapentryNum}"; 
        $mainfr.sumfr.maxnumEntry configure -textvariable appGlobal(empty);
        $mainfr.sumfr.maxnumEntry configure -textvariable \
            dbcfg(${arrPre}_USUMMARIZE_MAXNUM);
        $mainfr.sumfr.sumFtagEntry configure -textvariable appGlobal(empty);
        $mainfr.sumfr.sumFtagEntry configure -textvariable \
            dbcfg(${arrPre}_USUMMARIZE_TAGSPEC);
    } elseif {$context == "display"} {
        set arrPre "FILEDEF${num}_13DISPLAYS_DISPLAYDEF${displaydefNum}";
        set arrPre "${arrPre}_2CONVERT_5CLUSMAP${clusmapentryNum}"; 
    }

    ## to ##
    $mainfr.tofr.toFtagEntry configure -textvariable appGlobal(empty);
    $mainfr.tofr.toFtagEntry configure -textvariable \
    dbcfg(${arrPre}_TO_TAGSPEC);
}

#------------------------------------------------------------------------------
## this procedure configure the textvariables for the <FROM> frame 
#------------------------------------------------------------------------------
proc ConfigureClusmapFromEntryVariables {{context cluster}} {
    global appGlobal;
    global dbcfg;
    set num $appGlobal(filedefNum);    
    set clusmapentryNum $appGlobal(clusmapentryNum);
    set fr .clusmaptop.clusmapmainfr.fromFr;

    if {$context == "cluster"} {
        set clusterdefNum $appGlobal(clusterdefNum);
        set arrPre "FILEDEF${num}_10CLUSTERS_CLUSTERDEF${clusterdefNum}";
        set arrPre "${arrPre}_5CLUSMAP${clusmapentryNum}_"; 
    } elseif {$context == "display"} {
        set displaydefNum $appGlobal(displaydefNum);
        set arrPre "FILEDEF${num}_13DISPLAYS_DISPLAYDEF${displaydefNum}";
        set arrPre "${arrPre}_2CONVERT_5CLUSMAP${clusmapentryNum}_"; 
    }

    ## configure textvariable for the first entry
    $fr.fromtagspecEntry1 configure -textvariable appGlobal(empty);
    $fr.fromtagspecEntry1 configure -textvariable dbcfg(${arrPre}1FROM_TAGSPEC);

    # destroy all other in this frame entries
    foreach w [winfo children $fr] {
        set domatch [regexp {\D(\d+)$} $w match digit];
        if {$domatch == 1 && $digit > 1} {
            destroy $w;
        }
    }

    # re-create them if necessary -- that is, there are textvariables assigned 
    set enum 2;
    while {[info exists dbcfg(${arrPre}${enum}FROM_TAGSPEC)] &&
            $dbcfg(${arrPre}${enum}FROM_TAGSPEC) != ""} {
        set ent [entry $fr.fromtagspecEntry$enum -width 50 -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*}];
        $ent configure -textvariable dbcfg($arrPre${enum}FROM_TAGSPEC);

	set but [button $fr.fromtagspecEntryButton$enum -text X -padx \
	2 -pady 2];
	$but configure -command "DeleteANFromEntry $but $arrPre";
        grid $ent -row $enum -column 0 -padx 10 -pady 2 -sticky ew;
	grid $but -row $enum -column 1 -pady 2 -sticky w;
	set appGlobal(clusmapfromtagNum) $enum;
	incr enum;
    }
}
  
#----------------------------------------------------------------------------
## this procedure starts a new clusmap entry, namely, from, to and optional
## summarize info
#----------------------------------------------------------------------------
proc ChangeClusmapEntry {{context cluster} {direction next}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    set filedefNum $appGlobal(filedefNum);

    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }

    incr appGlobal(clusmapentryNum) $incrval;
    ConfigureClusmapVariables $context;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#----------------------------------------------------------------------------
## this procedure simply closes the window for clusmap entry creation 
#----------------------------------------------------------------------------
proc DoneClusmapEntry {} {
    destroy .clusmaptop;
}

#------------------------------------------------------------------------------
## this procedure lets user enter more "<FROM>" tagspec 
#------------------------------------------------------------------------------
proc MoreClusmapFromTagEntry {{context cluster}} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    set filedefNum $appGlobal(filedefNum); 
    set clusmapentryNum $appGlobal(clusmapentryNum);
    set num [incr appGlobal(clusmapfromtagNum)];

    if {$context == "cluster"} {
        set clusterdefNum $appGlobal(clusterdefNum);
        set arrPre "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF${clusterdefNum}";
        set arrPre "${arrPre}_5CLUSMAP${clusmapentryNum}_";
    } elseif {$context == "display"} {    
        set displaydefNum $appGlobal(displaydefNum);
        set arrPre "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF${displaydefNum}";
        set arrPre "${arrPre}_2CONVERT_5CLUSMAP${clusmapentryNum}_";
    }    

    set fr .clusmaptop.clusmapmainfr.fromFr;
    if {![winfo exists $fr.fromtagspecEntry$num]} {
        set ent [entry $fr.fromtagspecEntry$num -width 50 -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-12-*-*-*-*-*-*-*}];
	set but [button $fr.fromtagspecEntryButton$num -text X -padx \
	2 -pady 2];
	$but configure -command "DeleteANFromEntry $but $arrPre";
    } else {
        set ent $fr.fromtagspecEntry$num;
	set but $fr.fromtagspecEntryButton$num;
    }

    $ent configure -textvariable dbcfg(${arrPre}${num}FROM_TAGSPEC);
    grid $ent -row $num -column 0 -padx 10 -pady 2 -sticky ew;
    grid $but -row $num -column 1 -pady 2 -sticky w;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
# This procedure deletes an "FROM" tagspec entry if the user click on the delete
# button beside this entry widget, 
#-------------------------------------------------------------------------------
proc DeleteANFromEntry {path varPrefix} {
    global dbcfg;
    global appGlobal;
    regexp {(.+\D)(\d+)$} $path match head num;
    regsub {Button} $head "" entry;

    # move all variables downward a slot
    set biggerNum [expr $num + 1];
    while {[info exists \
	    dbcfg(${varPrefix}${biggerNum}FROM_TAGSPEC)]} {
	set dbcfg(${varPrefix}${num}FROM_TAGSPEC) \
		$dbcfg(${varPrefix}${biggerNum}FROM_TAGSPEC);
	incr num;
	incr biggerNum;
    }
    destroy $entry$num;
    destroy $head$num;
    set dbcfg(${varPrefix}${num}FROM_TAGSPEC) "";
    unset dbcfg(${varPrefix}${num}FROM_TAGSPEC);
    incr appGlobal(clusmapfromtagNum) -1;
}
##############################################################################

#-----------------------------------------------------------------------------
# the following procedure configures textvariables for the explain definition
#-----------------------------------------------------------------------------

proc ConfigureDbExplainVariables {{path .}} {
    global appGlobal;
    global dbcfg;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- EXPLAIN";

    set filedefNum $appGlobal(filedefNum);
    set preExp "FILEDEF${filedefNum}_";
    set fr [find-widget explainfr .];
    set choicefr $fr.explainchoicefr;

    if {![info exists dbcfg(${preExp}EXPLAINdefined)]} {
	set dbcfg(${preExp}EXPLAINdefined) skip;
    }
    $choicefr.explaindefine configure -variable dbcfg(${preExp}EXPLAINdefined);
    $choicefr.explainskip configure -variable dbcfg(${preExp}EXPLAINdefined);

    # proprietary info
    set proprietaryfr $fr.explaindeffr.proprietaryfr;
    $proprietaryfr.proprietarybb1 configure \
	    -variable dbcfg(${preExp}5EXPLAIN_9PROPRIETARY_NULL);
    $proprietaryfr.proprietarybb2 configure \
	    -variable dbcfg(${preExp}5EXPLAIN_9PROPRIETARY_NULL);

    set fr $fr.explaindeffr;

    # unfortunately for the recWrite proc to work, need to append NULL tag as
    # a subelement of this tag to handle cases like:
    # <TITLESTRING LANGUAGE=FRA>Blahblah</TITLESTRING>
    # will produce output in this form:
    # <TITLESTRING LANGUAGE=FRA><NULL>Blahblah</NULL></TITLESTRING>
    # then the pair of NULL tags are stripped off
    $fr.tstringentry configure -textvariable appGlobal(empty);
    $fr.tstringentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_1TITLESTRING_NULL);
   
    $fr.descentry configure -textvariable appGlobal(empty)
    $fr.descentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_2DESCRIPTION_NULL);
    
    $fr.disclentry configure -textvariable appGlobal(empty)
    $fr.disclentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_3DISCLAIMERS_NULL);
   
    $fr.newsentry configure -textvariable appGlobal(empty)
    $fr.newsentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_4NEWS_NULL);
    
    $fr.hrsentry configure -textvariable appGlobal(empty)
    $fr.hrsentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_5HOURS_NULL);
   
    $fr.btentry configure -textvariable appGlobal(empty)
    $fr.btentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_6BESTTIME_NULL);

    $fr.coverentry configure -textvariable appGlobal(empty)
    $fr.coverentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_8COVERAGE_NULL);

    $fr.cprightentry configure -textvariable appGlobal(empty)
    $fr.cprightentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_10COPYRIGHTTEXT_NULL);

    $fr.cprightnoticeentry configure -textvariable appGlobal(empty)
    $fr.cprightnoticeentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_11COPYRIGHTNOTICE_NULL);
   
    ## language attributes, default to ENG
    if {![info exists dbcfg(${preExp}5EXPLAIN_1TITLESTRING_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_1TITLESTRING_LANGUAGE.a) ENG;
    }
    $fr.tslcentry configure -textvariable appGlobal(empty);
    $fr.tslcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_1TITLESTRING_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_2DESCRIPTION_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_2DESCRIPTION_LANGUAGE.a) ENG;
    }
    $fr.desclcentry configure -textvariable appGlobal(empty);
    $fr.desclcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_2DESCRIPTION_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_3DISCLAIMERS_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_3DISCLAIMERS_LANGUAGE.a) ENG;
    }
    $fr.disclcentry configure -textvariable appGlobal(empty);
    $fr.disclcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_3DISCLAIMERS_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_4NEWS_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_4NEWS_LANGUAGE.a) ENG;
    }
    $fr.newslcentry configure -textvariable appGlobal(empty);
    $fr.newslcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_4NEWS_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_5HOURS_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_5HOURS_LANGUAGE.a) ENG;
    }
    $fr.hrslcentry configure -textvariable appGlobal(empty);
    $fr.hrslcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_5HOURS_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_6BESTTIME_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_6BESTTIME_LANGUAGE.a) ENG;
    }
    $fr.btlcentry configure -textvariable appGlobal(empty);
    $fr.btlcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_6BESTTIME_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_8COVERAGE_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_8COVERAGE_LANGUAGE.a) ENG;
    }
    $fr.coverlcentry configure -textvariable appGlobal(empty);
    $fr.coverlcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_8COVERAGE_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_10COPYRIGHTTEXT_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_10COPYRIGHTTEXT_LANGUAGE.a) ENG;
    }
    $fr.cprightlcentry configure -textvariable appGlobal(empty);
    $fr.cprightlcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_10COPYRIGHTTEXT_LANGUAGE.a);

    if {![info exists dbcfg(${preExp}5EXPLAIN_11COPYRIGHTNOTICE_LANGUAGE.a)]} {
	set dbcfg(${preExp}5EXPLAIN_11COPYRIGHTNOTICE_LANGUAGE.a) ENG;
    }
    $fr.cprightnoticelcentry configure -textvariable appGlobal(empty);
    $fr.cprightnoticelcentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_11COPYRIGHTNOTICE_LANGUAGE.a);

    $fr.lastupdatentry configure -textvariable appGlobal(empty);
    $fr.lastupdatentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_7LASTUPDATE_NULL);

    $fr.updateintervalfr.updateintervalnuentry configure\
	    -textvariable appGlobal(empty);
    $fr.updateintervalfr.updateintervalnuentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_12UPDATEINTERVAL_1VALUE);

    # update interval units combobox 
    if {![info exists dbcfg(${preExp}5EXPLAIN_12UPDATEINTERVAL_2UNITS)]} {
	$fr.updateintervalfr.updateintervalunit entryset "";
    } else {
	$fr.updateintervalfr.updateintervalunit entryset\
	$dbcfg(${preExp}5EXPLAIN_12UPDATEINTERVAL_2UNITS);
    }

    # the producer contact info
    set fr1 ${fr}.producercontactfr;
    $fr1.pcontactnameentry configure -textvariable appGlobal(empty);
    $fr1.pcontactnameentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_13PRODUCERCONTACTINFO_1CONTACTNAME);
    
    $fr1.pcontactdescentry configure -textvariable appGlobal(empty);
    $fr1.pcontactdescentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_13PRODUCERCONTACTINFO_2CONTACTDESCRIPTION);

    $fr1.pcontactaddentry configure -textvariable appGlobal(empty);
    $fr1.pcontactaddentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_13PRODUCERCONTACTINFO_3CONTACTADDRESS);

    $fr1.pcontactemailentry configure -textvariable appGlobal(empty);
    $fr1.pcontactemailentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_13PRODUCERCONTACTINFO_4CONTACTEMAIL);

    $fr1.pcontactphoneentry configure -textvariable appGlobal(empty);
    $fr1.pcontactphoneentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_13PRODUCERCONTACTINFO_5CONTACTPHONE);

    # now the supplier contact info
    set fr2 ${fr}.suppliercontactfr;
    $fr2.scontactnameentry configure -textvariable appGlobal(empty);
    $fr2.scontactnameentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_14SUPPLIERCONTACTINFO_1CONTACTNAME);
    
    $fr2.scontactdescentry configure -textvariable appGlobal(empty);
    $fr2.scontactdescentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_14SUPPLIERCONTACTINFO_2CONTACTDESCRIPTION);

    $fr2.scontactaddentry configure -textvariable appGlobal(empty);
    $fr2.scontactaddentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_14SUPPLIERCONTACTINFO_3CONTACTADDRESS);

    $fr2.scontactemailentry configure -textvariable appGlobal(empty);
    $fr2.scontactemailentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_14SUPPLIERCONTACTINFO_4CONTACTEMAIL);

    $fr2.scontactphoneentry configure -textvariable appGlobal(empty);
    $fr2.scontactphoneentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_14SUPPLIERCONTACTINFO_5CONTACTPHONE);

    # now the submission contact info
    set fr3 ${fr}.submissioncontactfr;

    $fr3.subcontactnameentry configure -textvariable appGlobal(empty);
    $fr3.subcontactnameentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_15SUBMISSIONCONTACTINFO_1CONTACTNAME);
    
    $fr3.subcontactdescentry configure -textvariable appGlobal(empty);
    $fr3.subcontactdescentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_15SUBMISSIONCONTACTINFO_2CONTACTDESCRIPTION);

    $fr3.subcontactaddentry configure -textvariable appGlobal(empty);
    $fr3.subcontactaddentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_15SUBMISSIONCONTACTINFO_3CONTACTADDRESS);

    $fr3.subcontactemailentry configure -textvariable appGlobal(empty);
    $fr3.subcontactemailentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_15SUBMISSIONCONTACTINFO_4CONTACTEMAIL);

    $fr3.subcontactphoneentry configure -textvariable appGlobal(empty);
    $fr3.subcontactphoneentry configure -textvariable\
    dbcfg(${preExp}5EXPLAIN_15SUBMISSIONCONTACTINFO_5CONTACTPHONE);
}

##############################################################################
## the following procedures are used to define display variables 
##############################################################################

#-----------------------------------------------------------------------------
## this procedure configures variables for a displaydef
#-----------------------------------------------------------------------------
proc ConfigureDbDisplayVariables {{path .}} {
    global appGlobal;
    global dbcfg;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF\
        $appGlobal(filedefNum) -- DISPLAYDEF $appGlobal(displaydefNum)";

    set num $appGlobal(filedefNum);    
    set displaydefNum $appGlobal(displaydefNum); 

    # start with all displaydef related control variables set to 1
    ResetDisplaydefControlVariables;

    set arrPre "FILEDEF${num}_13DISPLAYS_DISPLAYDEF${displaydefNum}"; 
    set appGlobal(displaydefheader) "Display (Format) Definition $displaydefNum";

    # takes care of navigation button
    set parentfr [find-widget displaybuttonfr $path];
    set backbutExist [winfo exists $parentfr.formatBackButton];
    if {$displaydefNum > 1 && !$backbutExist} {
        set backbut [button $parentfr.formatBackButton -text "Prev" \
            -command {ChangeDisplayDef prev} -background lightgray \
            -font -*-helvetica-bold-r-normal-*-12-*-*-*-*-*-*-* \
            -padx 10 -pady 3 -borderwidth 2 -foreground blue]; 
        grid $backbut -column 0 -row 0 -padx 10 -sticky e;
    } elseif {$displaydefNum == 1 && $backbutExist && \
                [winfo ismapped $parentfr.formatBackButton]} {
        grid forget $parentfr.formatBackButton;
    } elseif {$displaydefNum == 2 && $backbutExist && \
                ![winfo ismapped $parentfr.formatBackButton]} {
        grid $parentfr.formatBackButton -column 0 -row 0 -padx 10 -sticky e;
    }
    
    set parentfr [find-widget displaydeffr $path];
    # default on or off 
    $parentfr.formatdefaultCheckbutton configure -variable \
                dbcfg(${arrPre}_DEFAULT.a);

    # configure textvariable for the format name entry
    $parentfr.formatnameEntry configure -textvariable appGlobal(empty);
    $parentfr.formatnameEntry configure -textvariable \
                dbcfg(${arrPre}_NAME.a);
    
    # set the entry value for the format OID combobox
    if [info exists dbcfg(${arrPre}_OID.a)] {
        $parentfr.formatOIDCombo entryset $dbcfg(${arrPre}_OID.a); 
    } else {
        $parentfr.formatOIDCombo entryset ""; 
    }
    
    # configure variables for exclude
    $parentfr.formatexcludeFr.formatexcludecompressCheckbutton configure \
        -variable dbcfg(${arrPre}_1EXCLUDE_COMPRESS.a);

    $parentfr.formatexcludeFr.formatexclTagspecEntry1 configure -textvariable \
        appGlobal(empty);
    $parentfr.formatexcludeFr.formatexclTagspecEntry1 configure -textvariable \
        dbcfg(${arrPre}_1EXCLUDE_1TAGSPEC);

    # destroy all other entries in this frame
    foreach w [winfo children $parentfr.formatexcludeFr] {
        set domatch [regexp {\D(\d+)$} $w match digit];
        if {$domatch == 1 && $digit > 1} {
            destroy $w;
        }
    }

    # re-create them if there are textvariables defined, i.e., more
    # exclude tagspec entries for this displaydef
    set enum 2;
    while {[info exists dbcfg(${arrPre}_1EXCLUDE_${enum}TAGSPEC)] &&
           $dbcfg(${arrPre}_1EXCLUDE_${enum}TAGSPEC) != ""} {
        set ent [entry $parentfr.formatexcludeFr.formatexclTagspecEntry$enum \
                -width 60 -background gray95 \
                -font {-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
        $ent configure -textvariable dbcfg(${arrPre}_1EXCLUDE_${enum}TAGSPEC);

	set but [button \
	$parentfr.formatexcludeFr.formatexclTagspecEntryButton$enum];
	$but configure -text X -padx 2 -pady 2 -command\
	"DeleteAnExcludeEntry $but $arrPre";

        grid $ent -row $enum -column 0 -padx 20 -pady 5 \
            -sticky nsew; 
	grid $but -row $enum -column 1 -pady 5 -sticky w;
	set appGlobal(formatexcludetagNum) $enum;
        incr enum;
    }

    # configure variable for the ALL attribute of <CONVERT>
    $parentfr.formatconvertfr.formatconvertallCheckbutton configure -variable \
                dbcfg(${arrPre}_2CONVERT_ALL.a);

    set converFr $parentfr.formatconvertfr.formatconvertfunctionfr;

    # Configure the (text)variables related to the convert function 
    # associate radio buttons with the variable
    # now hide the external function frame first
    grid forget $converFr.formatfuncextfr;
    grid $converFr.formatfuncextdum -column 0 -row 4 -columnspan 3 \
	 -sticky news;
    $converFr.formatfuncradio5 configure -value "path/to/external/function";

    if [info exists dbcfg(${arrPre}_2CONVERT_FUNCTION.a)] {
	set funcName $dbcfg(${arrPre}_2CONVERT_FUNCTION.a);
	if {$funcName != "PAGE_PATH" && $funcName != "RECMAP" &&
            $funcName != "TAGSET-M" && $funcName != "TAGSET-G" &&
            $funcName != ""} {
            # a value exists that was input by the user 
	    grid forget $converFr.formatfuncextdum;
	    grid $converFr.formatfuncextfr -column 0 -row 4 -columnspan 3 \
            -sticky news;
	} 
    }

    for {set j 1} {$j < 6} {incr j} {
        $converFr.formatfuncradio$j \
            configure -variable dbcfg(${arrPre}_2CONVERT_FUNCTION.a);
    }

    # associate the potentially hiden entry with the variable
    $converFr.formatfuncextfr.formatfuncextEntry configure \
        -textvariable appGlobal(empty);
    $converFr.formatfuncextfr.formatfuncextEntry configure \
        -textvariable dbcfg(${arrPre}_2CONVERT_FUNCTION.a);
}

#---------------------------------------------------------------------------------
##this procedure addes a exclude spec entry for current display definition 
#---------------------------------------------------------------------------------
proc MoreFormatExcludeTagSpec {} {
    global appGlobal;
    global dbcfg;
    set oldsaved $appGlobal(dbcfgSaved);
    #------------------------------------------------------------
    ## find out which filedef we are in
    #------------------------------------------------------------
    set filedefNum $appGlobal(filedefNum); 

    #------------------------------------------------------------
    ## find out which displaydef (format) we are working on 
    #------------------------------------------------------------
    set displaydefNum $appGlobal(displaydefNum);

    #------------------------------------------------------------
    ## increment the number of last exclude tagspec entry 
    #------------------------------------------------------------
    set num [incr appGlobal(formatexcludetagNum)];

    set arrPre "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF${displaydefNum}";
    set fr [find-widget formatexcludeFr .];
    if {![winfo exists $fr.formatexclTagspecEntry$num]} {
        set ent [entry $fr.formatexclTagspecEntry$num -width 60 \
                -background gray95 -font\
	{-*-helvetica-medium-r-normal-*-14-*-*-*-*-*-*-*}];
	set but [button $fr.formatexclTagspecEntryButton$num -text X];
	$but configure -padx 2 -pady 2 -command "DeleteAnExcludeEntry\
	$but $arrPre";
    } else {
        set ent $fr.formatexclTagspecEntry$num;
	set but $fr.formatexclTagspecEntryButton$num;
    }

    $ent configure -textvariable dbcfg(${arrPre}_1EXCLUDE_${num}TAGSPEC);
    grid $ent -row $num -column 0 -padx 20 -pady 5 -sticky nsew; 
    grid $but -row $num -column 1 -pady 5 -sticky w;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#-------------------------------------------------------------------------------
# This procedure deletes an "EXCLUDE" tagspec entry if the user click on the delete
# button beside this entry widget, 
#-------------------------------------------------------------------------------
proc DeleteAnExcludeEntry {path varPrefix} {
    global dbcfg;
    global appGlobal;
    regexp {(.+\D)(\d+)$} $path match head num;
    regsub {Button} $head "" entry;

    set biggerNum [expr $num + 1];
    while {[info exists \
	    dbcfg(${varPrefix}_1EXCLUDE_${biggerNum}TAGSPEC)]} {
	set dbcfg(${varPrefix}_1EXCLUDE_${num}TAGSPEC) \
	    $dbcfg(${varPrefix}_1EXCLUDE_${biggerNum}TAGSPEC);
	incr num;
	incr biggerNum;
    }
    destroy $head$num;
    destroy $entry$num;
    set dbcfg(${varPrefix}_1EXCLUDE_${num}TAGSPEC) "";
    unset dbcfg(${varPrefix}_1EXCLUDE_${num}TAGSPEC);
    incr appGlobal(formatexcludetagNum) -1;
}

#----------------------------------------------------------------------------
## this procedure starts the definition for a new display format 
#----------------------------------------------------------------------------
proc ChangeDisplayDef {{direction next}} {
    global appGlobal;
    set oldsaved $appGlobal(dbcfgSaved);
    global dbcfg;
    if {$direction == "next"} {
        set incrval 1;
    } elseif {$direction == "prev"} {
        set incrval -1;
    }

    incr appGlobal(displaydefNum) $incrval;
    ConfigureDbDisplayVariables;
    set appGlobal(dbcfgSaved) $oldsaved;
}

################################################################################
# The following procedures are used to output, save the DBCONFIG file
################################################################################

#-------------------------------------------------------------------------------
# this procedure collects all array elements of the dbcfg array, and
# writes out an one line representation of them appropriately enclosed 
# in tags; another formatting procedure is called to break this line up
# and output the DBCONFIG file
# args:
#    1. prefix: each recursion builds on a previously constructed
#       prefix string as part of the index name
#    2. inlist: a list resulting from partitioning all the array
#       elements of last recursion level
#-------------------------------------------------------------------------------

proc recWrite {{prefix {}} inlist} {
    global dbcfg;
    # the return string of this recursive call
    set rtStr "";
    # NOTE: numbers have been inserted deliberately
    # into index names to force an order of output
    
    # handle each element of the list
    foreach elm $inlist {  
	# only handle those variables that actually stored some values
        if {$dbcfg($prefix$elm) != ""} {    
            if [regexp {^(\d*[A-Z]+)\.a$} $elm match attrname] {
		# we have reached the deepest level of nesting,
		# the index name ends with a '.a', a notation used to
		# denote an attribute of a tag
                lappend tmparr(attributes) $attrname;
            } elseif [regexp {^(\d*[A-Z]+)$} $elm match elename] {
		# we have reached the deepest level of nesting,
		# the index name ends with a regular upper case word.
		# this should be a regular leaf element (an element
		# that has no further subelements)
                set tmparr($elename) "";
	    } elseif [regexp {^([^_]+)_(.*)} $elm match head tail] {
		# we haven't reached the deepest level of nesting yet
		# since there are still underscore(s) in the index
		# name. We partition the big list into sublist and
		# handle them in turn. The sublists are, e.g.
		# tmparr(FILEDEF1)
		# tmparr(FILEDEF2) ......
		lappend tmparr($head) $tail;
	    }
        }
    }
    
    # handle the attributs of an element, if any
    # for example, <FILEDEF TYPE=SGML>
    if [info exists tmparr(attributes)] {
	set tmparr(attributes) [lsort -dictionary $tmparr(attributes)];
        foreach attr $tmparr(attributes) {
	    append rtStr " $attr=\"$dbcfg($prefix${attr}.a)\"";      
        }
	unset tmparr(attributes);
    }
    # close the tag
    append rtStr ">";

    set keys [array names tmparr];
    set keys [lsort -dictionary $keys];
    foreach key $keys {
	if {$tmparr($key) == ""} {
	    # this is a terminal element
	    append rtStr "<$key>$dbcfg($prefix${key})</$key>";
	    if {$key == "1FILENAME" || $key == "2FILETAG"} {
	      regexp {^FILEDEF(\d)+} $prefix match fdNum;
	      append rtStr "PLACEHOLDER_FOR_FILECONT$fdNum";
	    }
	} elseif [regexp {^(\d*[A-Z]+)(\d*)$} $key match tag num] {
	    # this is an enclosing tag that has subelements
	    append rtStr "<$tag";
            append rtStr [recWrite "$prefix${key}_" $tmparr($key)];
            append rtStr "</$tag>";
	}
    }    
    return $rtStr;
}

#----------------------------------------------------------------------------
## this procedure saves the DBCONFIG file being edited "AS" a filename
## it calls SaveDbConfigFile after a filename is provided by the user
#----------------------------------------------------------------------------
proc SaveDbConfigFileAs {} {
    global appGlobal;
    global dbcfg;
    set filename [tk_getSaveFile -initialdir $appGlobal(initDir)]
    if {$filename == ""} return;
    set dbcfg(savefilename) $filename;
    SaveDbConfigFile $filename;
    # update the window title
    set wintitle [wm title .staffcheshire];
    regexp {Staffcheshire \(DBCONFIG\)  (.+)\:} $wintitle match oldname;
    set newname [file tail $dbcfg(savefilename)];
    regsub $oldname $wintitle $newname wintitle;
    wm title .staffcheshire $wintitle;
}


#----------------------------------------------------------------------------
## this procedure saves the DBCONFIG file being edited
## it does all the formatting 
#----------------------------------------------------------------------------
proc SaveDbConfigFile {{filename ""}} {
    global appGlobal;
    global dbcfg;
    # if called without argument
    # see if a file name has been provided
    if {$filename == ""} {
	if {$dbcfg(savefilename) == "Untitled"} {
	    SaveDbConfigFileAs;
	    return;
	} else {
	    set filename $dbcfg(savefilename);
	}
    }
	
    for {set cont_num 1}\
	{[info exists dbcfg(FILEDEF${cont_num}_singleormulti)]}\
	{incr cont_num} {
          if {$dbcfg(FILEDEF${cont_num}_singleormulti) == "multiple"} {
	     if {[info exists dbcfg(FILEDEF${cont_num}_filecont)] == 0\
	          || $dbcfg(FILEDEF${cont_num}_filecont) == ""} {
	         tk_messageBox -message "Please fill in the path of\
			 continuation file for FILEDEF${cont_num}";
	         return;
	     }
	     set fname $dbcfg(FILEDEF${cont_num}_filecont);
	     if [catch {open $fname r} fid] {
		 tk_messageBox -message $fid;
	         return;
	     }
	     set contfile$cont_num [read $fid];
	     close $fid;
	 }
    }

    if {[file exists $filename] && [file isfile $filename]} {
	file copy $filename "${filename}~";
    }

    set sbar .staffcheshire.fdbcfg.dbstatusbar;
    $sbar progress init 50;
    for {set i 1} {$i < 10} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    set arrindex [array names dbcfg];
    # output all the index names for checking
    # NEEDFURTHER remove for production
    global proDir;
    if [catch {open  $proDir/vars w} fileidd] {
        puts stderr "cann't open file: $fileid"; 
        return;
    }

    set index [lsort -dictionary $arrindex];
    #foreach ind $index { 
        #puts $fileidd $ind;
	#puts $ind;
    #}

    set flag 1;
    set num 1;
    while {$flag} {
	if [info exists dbcfg(FILEDEF${num}_TYPE.a)] {
	    if {$dbcfg(FILEDEF${num}_TYPE.a) != "CLUSTER"} {
		if [info exists dbcfg(FILEDEF${num}_10CLUSTERS_CLUSBASE)] {
		    set dbcfg(FILEDEF${num}_10CLUSTERS_CLUSBASE) "";
		}
	    } else {
		foreach var [array names dbcfg] {
		    if [regexp {^FILEDEF\d+_10CLUSTERS_CLUSTERDEF} $var] {
			set dbcfg($var) "";
		    }
		}
	    }
	    incr num;
	} else {
	    set flag 0;
	}
    }
    set finalStr "<DBCONFIG";
    append finalStr [recWrite {} $arrindex]; 
    append finalStr "</DBCONFIG>";
    # now we get a one line representation of all the tags and values

    regsub -all {</1FILENAME>PLACEHOLDER_FOR_FILECONT\d+<2FILETAG>} $finalStr\
	    {</1FILENAME><2FILETAG>} finalStr;

    for {set i 10} {$i < 20} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    if {[info exists dbcfg(dbenv)] && $dbcfg(dbenv) != ""} {
	regsub {<DBCONFIG>} $finalStr\
		"<DBCONFIG>\n<DBENV>$dbcfg(dbenv)</DBENV>" finalStr;
    }
    # remove the digits in front of every tag, these digits were added
    # to force an order of the tags
    regsub -all {<(/?)\d+(\D)} $finalStr {<\1\2} finalStr;

    regsub -all "MARC Record Syntax" $finalStr "1.2.840.10003.5.10"\
    finalStr;
    regsub -all "OPAC Record Syntax" $finalStr "1.2.840.10003.5.102"\
    finalStr;
    regsub -all "SUTRS Record Syntax" $finalStr "1.2.840.10003.5.101"\
    finalStr;
    regsub -all "SGML Record Syntax" $finalStr "1.2.840.10003.5.109.9"\
    finalStr;
    regsub -all "XML Record Syntax" $finalStr "1.2.840.10003.5.109.10"\
    finalStr;
    regsub -all "HTML Record Syntax" $finalStr "1.2.840.10003.5.109.3"\
    finalStr;
    regsub -all "EXPLAIN Record Syntax" $finalStr "1.2.840.10003.5.100"\
    finalStr;
    regsub -all "GSR-1 Record Syntax" $finalStr "1.2.840.10003.5.105"\
    finalStr;
    regsub -all "Extended Services Record Syntax" $finalStr \
	 "1.2.840.10003.5.106" finalStr;
    
    # translate from string to number
    # POSIT
    regsub -all {<POSIT>first in field</POSIT>} $finalStr \
	    {<POSIT>1</POSIT>} finalStr;
    regsub -all {<POSIT>first in subfield</POSIT>} $finalStr \
	    {<POSIT>2</POSIT>} finalStr;
    regsub -all {<POSIT>any position in field</POSIT>} $finalStr \
	    {<POSIT>3</POSIT>} finalStr;
    # RELAT
    regsub -all {<RELAT>less than</RELAT>} $finalStr \
	    {<RELAT>1</RELAT>} finalStr;
    regsub -all {<RELAT>less or equal</RELAT>} $finalStr \
	    {<RELAT>2</RELAT>} finalStr;
    regsub -all {<RELAT>equal</RELAT>} $finalStr \
	    {<RELAT>3</RELAT>} finalStr;
    regsub -all {<RELAT>greater or equal</RELAT>} $finalStr \
	    {<RELAT>4</RELAT>} finalStr;
    regsub -all {<RELAT>greater than</RELAT>} $finalStr \
	    {<RELAT>5</RELAT>} finalStr;
    regsub -all {<RELAT>not equal</RELAT>} $finalStr \
	    {<RELAT>6</RELAT>} finalStr;
    regsub -all {<RELAT>phonetic</RELAT>} $finalStr \
	    {<RELAT>100</RELAT>} finalStr;
    regsub -all {<RELAT>stem</RELAT>} $finalStr \
	    {<RELAT>101</RELAT>} finalStr;
    regsub -all {<RELAT>relevance</RELAT>} $finalStr \
	    {<RELAT>102</RELAT>} finalStr;
    regsub -all {<RELAT>always matches</RELAT>} $finalStr \
	    {<RELAT>103</RELAT>} finalStr;

    # STRUCT
    regsub -all {<STRUCT>phrase</STRUCT>} $finalStr \
	    {<STRUCT>1</STRUCT>} finalStr;
    regsub -all {<STRUCT>word</STRUCT>} $finalStr \
	    {<STRUCT>2</STRUCT>} finalStr;
    regsub -all {<STRUCT>key</STRUCT>} $finalStr \
	    {<STRUCT>3</STRUCT>} finalStr;
    regsub -all {<STRUCT>year</STRUCT>} $finalStr \
	    {<STRUCT>4</STRUCT>} finalStr;
    regsub -all {<STRUCT>date \(normalized\)</STRUCT>} $finalStr \
	    {<STRUCT>5</STRUCT>} finalStr;
    regsub -all {<STRUCT>word list</STRUCT>} $finalStr \
	    {<STRUCT>6</STRUCT>} finalStr;
    regsub -all {<STRUCT>date \(un-normalized\)</STRUCT>} $finalStr \
	    {<STRUCT>100</STRUCT>} finalStr;
    regsub -all {<STRUCT>name \(normalized\)</STRUCT>} $finalStr \
	    {<STRUCT>101</STRUCT>} finalStr;
    regsub -all {<STRUCT>name \(un-normalized\)</STRUCT>} $finalStr \
	    {<STRUCT>102</STRUCT>} finalStr;
    regsub -all {<STRUCT>structure</STRUCT>} $finalStr \
	    {<STRUCT>103</STRUCT>} finalStr;
    regsub -all {<STRUCT>urx</STRUCT>} $finalStr \
	    {<STRUCT>104</STRUCT>} finalStr;
    regsub -all {<STRUCT>free-form-text</STRUCT>} $finalStr \
	    {<STRUCT>105</STRUCT>} finalStr;
    regsub -all {<STRUCT>document-text</STRUCT>} $finalStr \
	    {<STRUCT>106</STRUCT>} finalStr;
    regsub -all {<STRUCT>local number</STRUCT>} $finalStr \
	    {<STRUCT>107</STRUCT>} finalStr;
    regsub -all {<STRUCT>string</STRUCT>} $finalStr \
	    {<STRUCT>108</STRUCT>} finalStr;
    regsub -all {<STRUCT>numeric string</STRUCT>} $finalStr \
	    {<STRUCT>109</STRUCT>} finalStr;

    # TRUNC
    regsub -all {<TRUNC>right truncation</TRUNC>} $finalStr \
	    {<TRUNC>1</TRUNC>} finalStr;
    regsub -all {<TRUNC>left truncation</TRUNC>} $finalStr \
	    {<TRUNC>2</TRUNC>} finalStr;
    regsub -all {<TRUNC>left and right</TRUNC>} $finalStr \
	    {<TRUNC>3</TRUNC>} finalStr;
    regsub -all {<TRUNC>do not truncate</TRUNC>} $finalStr \
	    {<TRUNC>100</TRUNC>} finalStr;

    # COMPLET
    regsub -all {<COMPLET>incomplete subfield</COMPLET>} $finalStr \
	    {<COMPLET>1</COMPLET>} finalStr;
    regsub -all {<COMPLET>complete subfield</COMPLET>} $finalStr \
	    {<COMPLET>2</COMPLET>} finalStr;
    regsub -all {<COMPLET>complete field</COMPLET>} $finalStr \
	    {<COMPLET>3</COMPLET>} finalStr;


    regsub -all {(<DISPOPTIONS>[^<]*)0([^<]*</DISPOPTIONS>)} $finalStr \
	    {\1KEEP_ALL\2} finalStr;
    regsub -all {(<DISPOPTIONS>[^<]*)1([^<]*</DISPOPTIONS>)} $finalStr \
	    {\1KEEP_AMP\2} finalStr;
    regsub -all {(<DISPOPTIONS>[^<]*)2([^<]*</DISPOPTIONS>)} $finalStr \
	    {\1KEEP_LT\2} finalStr;
    regsub -all {(<DISPOPTIONS>[^<]*)3([^<]*</DISPOPTIONS>)} $finalStr \
	    {\1KEEP_GT\2} finalStr;

    # change cases for ftag and s
    regsub -all -nocase -- {<(/?)ftag>} $finalStr {<\1FTAG>} finalStr;
    regsub -all -nocase -- {<(/?)s>} $finalStr {<\1S>} finalStr;

    regsub -all {</?NULL>} $finalStr {} finalStr;

    # add new lines after these tags
    regsub -all {(</?DBCONFIG>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?EXPLAIN>)} $finalStr "\\1\n" finalStr;
    regsub -all {(<EXPLAIN>)} $finalStr "\n\\1" finalStr;
    regsub -all {(</CLUSBASE>)} $finalStr "\\1\n" finalStr;

    regsub -all {(</?FILEDEF[^>]*>)} $finalStr "\\1\n\n" finalStr;
    regsub -all {(</?(INDEXE|CLUSTER|DISPLAY|COMPONENT)S>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?(INDEX|CLUSTER|DISPLAY|COMPONENT)DEF[^>]*>)} $finalStr\
        "\\1\n" finalStr;
    regsub -all {(<(INDEX|CLUSTER|DISPLAY|COMPONENT)DEF[^>]*>)} $finalStr\
        "\n\\1" finalStr;

    regsub -all {(</(TITLESTRING|DESCRIPTION|DISCLAIMERS)>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</(NEWS|HOURS|BESTTIME|LASTUPDATE|COVERAGE)>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</(PROPRIETARY|COPYRIGHTTEXT|COPYRIGHTNOTICE)>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</UPDATEINTERVAL>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</(CONTACTNAME|CONTACTDESCRIPTION|CONTACTADDRESS)>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</(CONTACTEMAIL|CONTACTPHONE)>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {(</?(PRODUCER|SUPPLIER|SUBMISSION)CONTACTINFO>)} $finalStr\
	    "\\1\n" finalStr;
    regsub -all {<CONTACT} $finalStr {    <CONTACT_} finalStr;
    regsub -all {</CONTACT} $finalStr {</CONTACT_} finalStr;

    for {set i 20} {$i < 30} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    set findtags\
    "(FILETAG|FILENAME|FILEDTD|ASSOCFIL|HISTORY|SGMLCAT|SGMLSUBDOC|DISPOPTIONS)"
    regsub -all "(</$findtags>)" $finalStr "\\1\n" finalStr;

    set findtags "(INDXTAG|INDXNAME|INDXMAP|STOPLIST|INDXKEY)";
    regsub -all "(</$findtags>)" $finalStr "\\1\n" finalStr;

    set findtags \
    "COMPONENT(NAME|NORM|STARTTAG|ENDTAG|INDEXES)";
    regsub -all "(</$findtags>)" $finalStr "\\1\n" finalStr;

    regsub -all {(</(CLUSNAME|CLUSKEY)>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?CLUSMAP>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?INDXEXC>)} $finalStr "\\1\n" finalStr;

    regsub -all {(</?(INDX|CLUS)KEY[^>]*>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?(CONVERT|EXCLUDE)[^>]*>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</?TAGSPEC>)} $finalStr "\\1\n" finalStr;

    # remove these repeating tags
    regsub -all {</TAGSPEC>\n</FROM><FROM><TAGSPEC>} $finalStr "" finalStr;
    regsub -all {</CLUSMAP>\n<CLUSMAP>} $finalStr "" finalStr;
    regsub -all {</TAGSPEC>\n<TAGSPEC>} $finalStr "" finalStr;

    # a "U" was added in front of SUMMARIZE to force order, now remove it
    regsub -all {USUMMARIZE} $finalStr {SUMMARIZE} finalStr;    
    regsub -all {(</?(FROM|TO|SUMMARIZE)>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</MAXNUM>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</TAGSPEC>)} $finalStr "\n\\1" finalStr;

    # indent some of the tags
    regsub -all {(</?TAGSPEC>)} $finalStr "    \\1" finalStr;
    regsub -all {(<MAXNUM>)} $finalStr "    \\1" finalStr;
    regsub -all {(<FTAG>)} $finalStr "        \\1" finalStr;

    regsub -all {(<COMPONENT(START|END)TAG>)} $finalStr "\\1\n" finalStr;
    regsub -all {(</COMPONENT(START|END)TAG>)} $finalStr "\n\\1" finalStr;

    # add new line before some tags to fine tune the formatting
    regsub -all {(<(INDX|CLUS)KEY)} $finalStr "\n\\1" finalStr;
    regsub -all {(</CLUSMAP>)} $finalStr "\\1\n" finalStr;
    regsub -all {(<INDXMAP)} $finalStr "\n\\1" finalStr;
    regsub -all {</INDXMAP>\n\n<INDXMAP} $finalStr \
        "</INDXMAP>\n<INDXMAP" finalStr;
    regsub -all {(</CLUSMAP>)\n\n(</CONVERT>)} $finalStr \
	    "</CLUSMAP>\n</CONVERT>\n" finalStr;
 
    # replace binary attributes "X=1" with "X"
    #                           "X=0" with ""
    regsub -all {( [A-Z]+)=\"1\"([^>]*)>} $finalStr {\2\1>} finalStr;
    regsub -all { [A-Z]+=\"0\"} $finalStr {} finalStr;

    # now recursively remove all empty tags except for <CONVERT> which
    # may be empty
    regsub -all -- {(<CONVERT[^>]+>)\s*</CONVERT>} $finalStr\
	    {\1TEMP</CONVERT>} finalStr;
    set flag 1;
    while {$flag} {
	if [regexp {<(\S+)[^>]*>\s*</\1>} $finalStr match tag] {
	    regsub -all {<(\S+)[^>]*>\s*</\1>\n} $finalStr "" \
		finalStr;
	} else {
	    set flag 0;
        }
    }
    for {set i 30} {$i < 40} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    regsub -all {(<CONVERT[^>]*>)TEMP</CONVERT>} $finalStr \
	    {\1</CONVERT>} finalStr;
  
    # add comments in the output file
    regsub -all {(<FILETAG>)} $finalStr \
        "<!-- FILETAG is the \"shorthand\" name of the file\
        -->\n\\1" finalStr;

    regsub -all {(<FILENAME>)} $finalStr \
        "\n<!-- FILENAME is the full path name of the file\
        -->\n\\1" finalStr;

    regsub -all {(<FILEDTD>)} $finalStr \
        "\n<!-- FILEDTD is the full path name of the file's DTD\
        -->\n\\1" finalStr;

    regsub -all {(<ASSOCFIL>)} $finalStr \
        "\n<!-- ASSOCFIL is the full path name of the file's Associator file\
        -->\n\\1" finalStr;

    regsub -all {(<HISTORY>)} $finalStr \
        "\n<!-- HISTORY is the full path name of the file's history file\
        -->\n\\1" finalStr;

    regsub -all {(<(INDEX|CLUSTER|DISPLAY)DEF[^>]*>)} $finalStr \
        "<!-- ************************************************************\
        -->\n\\1" finalStr;

    regsub -all {(<FILEDEF [^>]*>)} $finalStr \
        "\n<!-- ************************************************************\
        -->\n<!-- ************************************************************\
        -->\n<!-- ************************************************************\
        -->\n\\1" finalStr;

    regsub -all {(<(INDEX|CLUSTER|DISPLAY|COMPONENT)(E?)S>)} $finalStr \
        "\n<!-- ************************************************************\
        -->\n<!-- ** The following are the \\2\\3S definitions for the file **\
        -->\n<!-- ************************************************************\
        -->\n\\1" finalStr;

    for {set i 40} {$i < 50} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    # some other substitutions
    regsub -all { LANGUAGE=\"ENG\"} $finalStr {} finalStr;
    regsub -all {<CLUSTERS>(\n*<CLUSBASE>)} $finalStr {<CLUSTER>\1} finalStr;
    regsub -all {(</CLUSBASE>\n*)</CLUSTERS>} $finalStr {\1</CLUSTER>} finalStr;

    for {set cont_num 1}\
	{[info exists dbcfg(FILEDEF${cont_num}_singleormulti)]}\
	{incr cont_num} {
         if {$dbcfg(FILEDEF${cont_num}_singleormulti) == "multiple" &&\
		 [info exists contfile$cont_num]} {
	     set varname contfile$cont_num;
	     regsub "PLACEHOLDER_FOR_FILECONT$cont_num" $finalStr\
	            [set $varname] finalStr;
	 }
    }
    regsub -all {PLACEHOLDER_FOR_FILECONT\d+} $finalStr "" finalStr;

    if [catch {open $filename w} fileid] {
	tk_messageBox -message $fileid;
        return;
    }

    puts $fileid $finalStr;
    close $fileid;
     
    #close $fileidd;
    # flag that the file has been saved
    set appGlobal(dbcfgSaved) 1;
    set filetail [file tail $filename];
    tk_messageBox -message "Previous version of $filetail copied to ${filetail}~";
}


#--------------------------------------------------------------------
## this procedure inits some of the selections for configuration file
## e.g, combobox entries
#---------------------------------------------------------------------
proc InitDBconfig {{path .}} {
    global appGlobal;
    global dbcfg;

    # insert options for the file type combobox
    set filetypecombo [find-widget dataFileTypeCombo $path];
    $filetypecombo histinsert 1 "SGML";
    $filetypecombo histinsert 2 "MARC";
    $filetypecombo histinsert 3 "AUTH";
    $filetypecombo histinsert 4 "CLUSTER";
    $filetypecombo histinsert 5 "LCTREE";
    $filetypecombo histinsert 6 "MAPPED";
    $filetypecombo histinsert 7 "SQL | DBMS | RDBMS";
    
    ## initialize the display option listbox
    set dispoptlbox [find-widget dispOptList $path]; 
    $dispoptlbox insert end "KEEP_ALL";
    $dispoptlbox insert end "KEEP_AMP";
    $dispoptlbox insert end "KEEP_LT";
    $dispoptlbox insert end "KEEP_GT";

    # insert options for the index ACCESS type combobox
    foreach tmpVar {{} comp} {
	set indexaccCombo [find-widget ${tmpVar}indexACCESSCombo $path];
	$indexaccCombo histinsert 1 "BTREE";
	$indexaccCombo histinsert 2 "HASH";
	$indexaccCombo histinsert 3 "RECNO";
    	$indexaccCombo histinsert 4 "DBMS";

	# insert options for the index EXTRACT type combobox
	set indexextrCombo [find-widget ${tmpVar}indexEXTRACTCombo $path];
	$indexextrCombo histinsert 1 "KEYWORD";
	$indexextrCombo histinsert 2 "EXACTKEY";
	$indexextrCombo histinsert 3 "FLD008KEY";
	$indexextrCombo histinsert 4 "KEYWORD_SUBDOC";
	$indexextrCombo histinsert 5 "EXACTKEY_SUBDOC";
	$indexextrCombo histinsert 6 "URL";
	$indexextrCombo histinsert 7 "FILENAME";
	$indexextrCombo histinsert 8 "DATE";
	$indexextrCombo histinsert 9 "DATE_RANGE";
	$indexextrCombo histinsert 10 "DATE_TIME";
	$indexextrCombo histinsert 11 "DATE_TIME_RANGE";
	$indexextrCombo histinsert 12 "INTEGER_KEY";
	$indexextrCombo histinsert 13 "DECIMAL_KEY";
	$indexextrCombo histinsert 14 "FLOAT_KEY";
	$indexextrCombo histinsert 15 "KEYWORD_EXTERNAL";


	# insert options for the index NORMAL type combobox
	set indexnormCombo [find-widget ${tmpVar}indexNORMALCombo $path];
	$indexnormCombo histinsert 1 "NONE";
	$indexnormCombo histinsert 2 "STEM";
	$indexnormCombo histinsert 3 "WORDNET";
	$indexnormCombo histinsert 4 "CLASSCLUS";
    

	#insert options for the index PRIMARYKEY combobox
	set indexpkCombo [find-widget ${tmpVar}indexPRIMARYKEYCombo $path];
	$indexpkCombo histinsert 1 "NO";
	$indexpkCombo histinsert 2 "REJECT";
	$indexpkCombo histinsert 3 "IGNORE";
	$indexpkCombo histinsert 4 "REPLACE";
    }

    # insert options for the component norm combobox
    set compNormCombo [find-widget compNormCombo $path];
    $compNormCombo histinsert 1 "NONE";
    $compNormCombo histinsert 2 "STEM";
    $compNormCombo histinsert 3 "WORDNET";
    $compNormCombo histinsert 4 "CLASSCLUS";
    $compNormCombo histinsert 5 "EXACTKEY";

    #insert options for the cluskey NORMAL type combobox
    set cluskeynormCombo [find-widget clusterKeyNormalCombo $path];
    $cluskeynormCombo histinsert 1 "NONE";
    $cluskeynormCombo histinsert 2 "STEM";
    $cluskeynormCombo histinsert 3 "WORDNET";
    $cluskeynormCombo histinsert 4 "CLASSCLUS";
    $cluskeynormCombo histinsert 5 "EXACTKEY";

    set formatOIDCombo [find-widget formatOIDCombo $path];

    # 1.2.840.10003.5.10
    $formatOIDCombo histinsert 1 "MARC Record Syntax";
    # 1.2.840.10003.5.102
    $formatOIDCombo histinsert 2 "OPAC Record Syntax";
    # 1.2.840.10003.5.101
    $formatOIDCombo histinsert 3 "SUTRS Record Syntax";
    # 1.2.840.10003.5.109.9
    $formatOIDCombo histinsert 4 "SGML Record Syntax";
    # 1.2.840.10003.5.109.10
    $formatOIDCombo histinsert 5 "XML Record Syntax";
    # 1.2.840.10003.5.109.3
    $formatOIDCombo histinsert 6 "HTML Record Syntax";
    # 1.2.840.10003.5.100
    $formatOIDCombo histinsert 7 "EXPLAIN Record Syntax";
    # 1.2.840.10003.5.105
    $formatOIDCombo histinsert 8 "GRS-1 Record Syntax";
    # 1.2.840.10003.5.106
    $formatOIDCombo histinsert 9 "Extended Services Record Syntax";

    set updateintervalCombo [find-widget updateintervalunit $path];
    $updateintervalCombo histinsert 1 "DAY";
    $updateintervalCombo histinsert 2 "WEEK";
    $updateintervalCombo histinsert 3 "MONTH";
    $updateintervalCombo histinsert 4 "YEAR";

    # by default, hide the continuation frame, assume the data is in a
    # single file
    HideContFrame;
    set appGlobal(singleormulti) single;

    # we started with an empty configuration file, so nothing to save
    set appGlobal(dbcfgSaved) 1;
}

#--------------------------------------------------------------------
## this procedure hides or show the frame that holds an entry for the
# user to type in an external function name 
#---------------------------------------------------------------------
proc ShowHideExtFuncFr {path} {
    set parentfr [find-widget formatconvertfunctionfr .];
    # if the user clicks on the "External" radio button
    if [regexp {.+5$} $path match] {
        grid forget $parentfr.formatfuncextdum;
        grid $parentfr.formatfuncextfr -column 0 -row 4 -columnspan 3\
	-sticky news;
    } else {
	# user chose one of the named functions such as PAGE_PATH
        grid forget $parentfr.formatfuncextfr;
        grid $parentfr.formatfuncextdum -column 0 -row 4 -columnspan 3 -sticky news;
    }
}    

#--------------------------------------------------------------------
## the following procedures are called when the user changes the
# window by clicking on the tabs of the DBCONFIG interface  
#---------------------------------------------------------------------
proc ShowDataFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.data;
    # we keep the save status before doing the variable bindings, which
    # are traced. If the user is simply navigating without typing in
    # any value, then change back to this status
    #Call_Trace;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate we are now in display frame
    set appGlobal(workondata) 1; 
    set appGlobal(workonindex) 0; 
    set appGlobal(workoncluster) 0; 
    set appGlobal(workonexplain) 0; 
    set appGlobal(workondisplay) 0;
    set appGlobal(workoncomp) 0;
    set filedefNum $appGlobal(filedefNum);
    set dbcfg(FILEDEF${filedefNum}_DATAdefined) true;
    ConfigureDbDataVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

proc ShowIndexFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.indexes;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate that we are currently in the index frame
    set appGlobal(workondata) 0; 
    set appGlobal(workonindex) 1; 
    set appGlobal(workoncluster) 0; 
    set appGlobal(workonexplain) 0; 
    set appGlobal(workondisplay) 0;
    set appGlobal(workoncomp) 0;
    ConfigureDbIndexVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

proc ShowClusterFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.clusters;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate we are now in cluster frame
    set appGlobal(workondata) 0; 
    set appGlobal(workonindex) 0; 
    set appGlobal(workoncluster) 1; 
    set appGlobal(workonexplain) 0; 
    set appGlobal(workondisplay) 0;
    set appGlobal(workoncomp) 0;
    ConfigureDbClusterVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

proc ShowExplainFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.explain;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate we are now in explain frame
    set appGlobal(workondata) 0; 
    set appGlobal(workonindex) 0; 
    set appGlobal(workoncluster) 0; 
    set appGlobal(workonexplain) 1; 
    set appGlobal(workondisplay) 0; 
    set appGlobal(workoncomp) 0;
    ConfigureDbExplainVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

proc ShowDisplayFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.display;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate we are now in display frame
    set appGlobal(workondata) 0; 
    set appGlobal(workonindex) 0; 
    set appGlobal(workoncluster) 0; 
    set appGlobal(workonexplain) 0; 
    set appGlobal(workondisplay) 1;
    set appGlobal(workoncomp) 0;
    ConfigureDbDisplayVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

proc ShowComponentFrame {{path .}} {
    global appGlobal;
    global dbcfg;
    .staffcheshire.fdbcfg.dbcfgfr.gridwin configure -widget \
       .staffcheshire.fdbcfg.dbcfgfr.gridwin.component;
    set oldsaved $appGlobal(dbcfgSaved);
    # indicate we are now in component frame
    set appGlobal(workondata) 0; 
    set appGlobal(workonindex) 0; 
    set appGlobal(workoncluster) 0; 
    set appGlobal(workonexplain) 0; 
    set appGlobal(workondisplay) 0;
    set appGlobal(workoncomp) 1;
    ConfigureDbComponentVariables $path;
    set appGlobal(dbcfgSaved) $oldsaved;
}

#--------------------------------------------------------------------
## this procedure traces the dbcfg array, if it has been written to,
## then we need a save when user switches to another DBCONFIG or 
## open up a new one
#---------------------------------------------------------------------
proc TraceDBConfigArray {varname index op} {
    global appGlobal;
    global dbcfg;
    # if any write has been down to the array, we need to save it
    set appGlobal(dbcfgSaved) 0;
    $appGlobal(filemenu) entryconfigure 2 -state normal; # enable new

    # cannot safely open another dbconfig once the user starts working
    # on one config file. need to force the user to click on "new"
    # which does the clean up
    #$appGlobal(filemenu) entryconfigure 1 -state disabled; 
}

# trace appGlobal(dbcfgSaved), change the state of Save menu
proc TraceSaved {varname index op} {
    global appGlobal;
    if {$appGlobal(dbcfgSaved) == 1} {
	$appGlobal(filemenu) entryconfigure 4 -state disabled;
    } else {
	$appGlobal(filemenu) entryconfigure 4 -state normal;
    }
}

#----------------------------------------------------------------------------
# init procedure further constructs the user interface adding contents to it
#----------------------------------------------------------------------------
proc staffcheshire::init {path} {
    global appGlobal;
    global dbcfg;

    set pwd [pwd];
    set appGlobal(initDir) [pwd]
    set appGlobal(filedefNum) 1;
    set appGlobal(dbcfgSaved) 1;
    ResetFiledefControlVariables; 

    InitDBconfig $path;

    set appGlobal(filemenu) [find-widget filecascade .];
    set dbcfg(savefilename) "Untitled";
    $appGlobal(filemenu) entryconfigure 2 -state disabled; # disable new
    $appGlobal(filemenu) entryconfigure 4 -state disabled; # disable save

    # now start with the data window
    set dbtabset [find-widget dbtabset .];
    $dbtabset setselection data;
    # start tracing the variables
    trace variable appGlobal(dbcfgSaved) w TraceSaved;
    trace variable dbcfg w TraceDBConfigArray;
}

#----------------------------------------------------------------------------
# Now open the user interface
#----------------------------------------------------------------------------
proc OpenDbInterface {} {
    global dbcfg;
    #set click1 [clock clicks -milliseconds];
    staffcheshire;
    set fn [file tail $dbcfg(savefilename)];
    wm title .staffcheshire "Staffcheshire (DBCONFIG)  ${fn}: FILEDEF 1 -- DATA";
    wm geometry . 800x600+50+50;
    #set click2 [clock clicks -milliseconds];
    #if {[expr $click2 - $click1] < 2000} {
	#after 2000;
    #}
    wm withdraw .;
    wm protocol .staffcheshire WM_DELETE_WINDOW {exit};	
}

#############################################################################
## the command that opens the db interface
#############################################################################
proc FlashIcon {} {
    wm withdraw .;
    wm resizable . 0 0;
    wm overrideredirect . 1;
    set img [image create photo img  -file Staffcheshire.gif];
    set lab [label .flashlab -image $img];
    pack $lab;
    wm geometry . 413x580+50+50;
    wm deiconify .;
    update idletasks;
}

#FlashIcon;
OpenDbInterface;

proc CheckDbSave {} {
    global appGlobal;
    global dbcfg;
    if {$appGlobal(dbcfgSaved) == 0} {
	set filename $dbcfg(savefilename);
	set saveornot [tk_dialog .save "Save File?" \
		"Do you want to save the change you made to $filename?"\
		{} 0 "Yes" "No" "Cancel"];
	if {$saveornot == 0} {
	    SaveDbConfigFile;
	    return 0;
	} elseif {$saveornot == 2} {
	    return 2;
	}
	return 1;
    }
    return 3;
}

proc NewConfigFile {} {
    global appGlobal;
    global dbcfg;
    if {[CheckDbSave] == 2} {
	return 2;
    }
    destroy .staffcheshire;
    unset appGlobal;
    unset dbcfg;
    OpenDbInterface;
}

proc DbToServer {} {
    global dbcfg;
    global appGlobal;
    if {[CheckDbSave] == 2} {
	return;
    }
    destroy .staffcheshire;
    unset appGlobal;
    unset dbcfg;
    source server.tcl;
    source serverifc.tcl;
    OpenServerInterface;
}

proc DbToCmd {cmdname} {
    global dbcfg;
    global appGlobal;
    if {[CheckDbSave] == 2} {
	return;
    }
    destroy .staffcheshire;
    unset appGlobal;
    unset dbcfg;
    source cmd.tcl;
    source cmdifc.tcl;
    OpenCmdInterface $cmdname;
}

proc FinishDbcfg {} {
    if {[CheckDbSave] == 2} {
	return;
    }
    exit 0;
}

# ---------------------------------------------------------------------------
# Activate tool help (in statusbar and help windows).
# don't know why, extremely slow to use
# ---------------------------------------------------------------------------
#tooltip;
# ---------------------------------------------------------------------------
# Function    : status - # 
# Returns     : 
# Parameters  : 
# Description : Fill the statusbar information (from db help)
# ---------------------------------------------------------------------------
proc status {string} {
    set sb [find-widget statusbar .];
    $sb configure -text $string;
}

# ---------------------------------------------------------------------------
# Function    : debug - 
# Returns     : 
# Parameters  :  
# Description : Extended debug messages either to stderr and with
#               Additional Informations. 
# ---------------------------------------------------------------------------
proc debug {info {message {}}} {
    status $info;
    puts stderr $info;
    if {[info exists ::env(DEBUG)] && $::env(DEBUG)} {
        puts stderr "Message : $msg";
    }
}

#--------------------------------------------------------------------------------
# this procedure handles the actions to take after the user selects
# entries from all comboboxes
# this is needed because the combobox widget isn't like an entry
# widget and cannot automatically update the value of associated textvariable
#--------------------------------------------------------------------------------
proc HandleComboSelect {path} {
    global appGlobal;
    global dbcfg;
    set filedefNum $appGlobal(filedefNum);

    # construct the prefix string of the array index name
    # this is specifically used for those comboboxes inside component 
    # indexdef's
    if [regexp {(InsideComponent|compindex.+Combo)$} $path] {
	set componentdefNum $appGlobal(componentdefNum);
	set compindexdefNum $appGlobal(compindexdefNum);
	set idxPre \
	"FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF${componentdefNum}";
	append idxPre "_COMPONENTINDEXES_INDEXDEF${compindexdefNum}";
    }
    
    # get the current selected value
    set val [$path get];
    switch -regexp -- $path {
        {\.dataFileTypeCombo$} {
            set dbcfg(FILEDEF${filedefNum}_TYPE.a) $val; 
        }

        {\.indexACCESSCombo$} {
	#-----------------------------------------------------------------
	# start comboboxes related to toplevel indexdef (not embedded
	# in componentdef)
	#-----------------------------------------------------------------
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
            set dbcfg(${idxPre}_ACCESS.a) $val; 
        }
        {\.indexEXTRACTCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
            set dbcfg(${idxPre}_EXTRACT.a) $val; 
        }
        {\.indexNORMALCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
            set dbcfg(${idxPre}_NORMAL.a) $val; 
        }
        {\.indexPRIMARYKEYCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
            set dbcfg(${idxPre}_PRIMARYKEY.a) $val; 
        }
        {\.attributesetCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_ATTRIBUTESET.a) $val; 
        }
        {\.relationCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);            
	    set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_2RELAT) $val; 
        }
        {\.positionCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);            
	    set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_3POSIT) $val; 
        }
        {\.structureCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_4STRUCT) $val; 
        }
        {\.truncationCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_5TRUNC) $val; 
        }
        {\.completenessCombo$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set idxPre \
            "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}"; 
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_6COMPLET) $val; 
	#-----------------------------------------------------------------
	# end comboboxes related to indexdef
	#-----------------------------------------------------------------
        }

	{\.compNormCombo$} {
	#-----------------------------------------------------------------
	# start comboboxes related to componentdef
	#-----------------------------------------------------------------
	    set componentdefNum $appGlobal(componentdefNum);    
	    set compPre \
           "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF${componentdefNum}";
	    set dbcfg(${compPre}_2COMPONENTNORM) $val;
	}
        {\.compindexACCESSCombo$} {
            set dbcfg(${idxPre}_ACCESS.a) $val; 
        }
        {\.compindexEXTRACTCombo$} {
            set dbcfg(${idxPre}_EXTRACT.a) $val; 
        }
        {\.compindexNORMALCombo$} {
            set dbcfg(${idxPre}_NORMAL.a) $val; 
        }
        {\.compindexPRIMARYKEYCombo$} {
            set dbcfg(${idxPre}_PRIMARYKEY.a) $val; 
        }
        {\.attributesetComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_ATTRIBUTESET.a) $val; 
        }
        {\.relationComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_2RELAT) $val; 
        }
        {\.positionComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);	  
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_3POSIT) $val; 
        }
        {\.structureComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_4STRUCT) $val; 
        }
        {\.truncationComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_5TRUNC) $val; 
        }
        {\.completenessComboInsideComponent$} {
	    set indexmapNum $appGlobal(indexmapNum);
            set dbcfg(${idxPre}_3INDXMAP${indexmapNum}_6COMPLET) $val;
	#-----------------------------------------------------------------
	# end comboboxes related to componentdef
	#-----------------------------------------------------------------
	}

        {\.clusterKeyNormalCombo$} {
	    set clusterdefNum $appGlobal(clusterdefNum); 
	    set cluPre \
            "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF${clusterdefNum}"; 
	    set dbcfg(${cluPre}_2CLUSKEY_NORMAL.a) $val; 
        }

        {\.updateintervalunit$} {
	    set explPre "FILEDEF${filedefNum}_5EXPLAIN";
            set dbcfg(${explPre}_12UPDATEINTERVAL_2UNITS) $val; 
        }

        {\.formatOIDCombo$} {
	    set displaydefNum $appGlobal(displaydefNum);
	    set dispPre \
            "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF${displaydefNum}"; 
            set dbcfg(${dispPre}_OID.a) $val; 
        }

   }
}
#-------------------------------------------------------------------------------------
# handle all the file browse selection actions
# this procedure uses the built-in fileselect dialog to let the user
# select a file
# NOTE: use the fileselect package for directory selection 
#-------------------------------------------------------------------------------------
proc HandleBBSelect {path {input ""}} {
    global appGlobal;
    global dbcfg;
    global serverArray;
    global cmdArray;
   
    if [info exists appGlobal(filedefNum)] {
	set filedefNum $appGlobal(filedefNum);
    }

    set dpath $dbcfg(FILEDEF${filedefNum}_0DEFAULTPATH);

    if {$dpath == ""} {
	set initdir $appGlobal(initDir)
    } else {
	set initdir $dpath
    }

    if {$input == ""} {
        set fileselect [tk_getOpenFile -initialdir $initdir];
        if {$fileselect == ""} return;
    } else {
        set fileselect $input;
    }

    # strip off the default path prefix
    if {$dpath != ""} {
	regsub "${dpath}/" $fileselect "" fileselect;
    }

    switch -regexp -- $path { 
        {\.dataFilePathbb$} {
            set dbcfg(FILEDEF${filedefNum}_1FILENAME) $fileselect; 
        }
        {\.dataContPathbb$} {
            set dbcfg(FILEDEF${filedefNum}_filecont) $fileselect; 
        }
        {\.dataDtdbb$} {
            set dbcfg(FILEDEF${filedefNum}_6FILEDTD) $fileselect; 
        }
        {\.dataSgmlCatbb$} {
            set dbcfg(FILEDEF${filedefNum}_3SGMLCAT) $fileselect; 
        }
        {\.dataAssocPathbb$} {
            set dbcfg(FILEDEF${filedefNum}_7ASSOCFIL) $fileselect; 
        }
        {\.dataHisPathbb$} {
            set dbcfg(FILEDEF${filedefNum}_8HISTORY) $fileselect; 
        }
        {\.indexPathBrowseButton$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set pre1 "FILEDEF${filedefNum}_9INDEXES_INDEXDEF$indexdefNum";
            set dbcfg(${pre1}_1INDXNAME) $fileselect; 
        }
        {\.indexStoplistBrowseButton$} {
	    set indexdefNum $appGlobal(indexdefNum); 
	    set pre1 "FILEDEF${filedefNum}_9INDEXES_INDEXDEF$indexdefNum";
	    set dbcfg(${pre1}_4STOPLIST) $fileselect; 
        }
        {\.compindexPathBrowseButton$} {
	    set componentdefNum $appGlobal(componentdefNum);
	    set compindexdefNum $appGlobal(compindexdefNum);
	    set pre3 "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
	    append pre3 "${componentdefNum}_COMPONENTINDEXES";
	    append pre3 "_INDEXDEF${compindexdefNum}";
            set dbcfg(${pre3}_1INDXNAME) $fileselect; 
        }
        {\.compindexStoplistBrowseButton$} {
	    set componentdefNum $appGlobal(componentdefNum);
	    set compindexdefNum $appGlobal(compindexdefNum);
	    set pre3 "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
	    append pre3 "${componentdefNum}_COMPONENTINDEXES";
	    append pre3 "_INDEXDEF${compindexdefNum}";
            set dbcfg(${pre3}_4STOPLIST) $fileselect; 
        }
	{\.compNameBButton$} {
	    set componentdefNum $appGlobal(componentdefNum);
	    set pre4 "FILEDEF${filedefNum}_11COMPONENTS_";
	    append pre4 "COMPONENTDEF${componentdefNum}";
	    set dbcfg(${pre4}_1COMPONENTNAME) $fileselect;
	}

        {\.clusterStoplistBrowseButton$} {
	    set clusterdefNum $appGlobal(clusterdefNum); 
	    set pre2 \
            "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF$clusterdefNum";
            set dbcfg(${pre2}_4STOPLIST) $fileselect; 
        }

	{\.clusbasebbut$} {
	    set \
	    dbcfg(FILEDEF${filedefNum}_10CLUSTERS_CLUSBASE) \
		    $fileselect;
	}

        {\.dbdirbb$} {
            set serverArray(dbdir) $fileselect; 
        }
        {\.dbcffilesbb$} {
            set serverArray(dbcffiles) $fileselect; 
        }
        {\.rsdirbb$} {
            set serverArray(rsdir) $fileselect; 
        }
	{\.formatfuncextButton$} {
	    set displaydefNum $appGlobal(displaydefNum);
	    set dispPre \
            "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF$displaydefNum";
            set dbcfg(${dispPre}_2CONVERT_FUNCTION.a) $fileselect;
	}
	{\.buildassocfpBB$} {
	    set cmdArray(build,path) $fileselect;
	}
	{\.indexchedirBB$} {
	    set cmdArray(indexche,dir) $fileselect;
	}
	{\.indexchecfBB$} {
	    set cmdArray(indexche,cf) $fileselect;
	}
	{\.indexcludirBB$} {
	    set cmdArray(indexclu,dir) $fileselect;
	}
	{\.indexclucfBB$} {
	    set cmdArray(indexclu,cf) $fileselect;
	}
    }    
}

proc OpenConfigFile {} {
    global appGlobal;
    if {[info exists appGlobal(dbcfgSaved)] &&\
	    $appGlobal(dbcfgSaved) == 0} {
	if {[NewConfigFile] == 2} {
	    return;
        }
    }
    if {[ReadOldConfigFile] == 2} {
	return;
    }
    $appGlobal(filemenu) entryconfigure 1 -state disabled; 
}

#------------------------------------------------------------------------------
# this procedure reads a configuration file and parses it before handing it
# over to ParseOld procedure
#------------------------------------------------------------------------------
proc ReadOldConfigFile {} {
    #set click1 [clock clicks];
    global appGlobal;
    global dbcfg;

    # a variable to use to keep context when parsing an old DBCONFIG file. 
    # each tag or attribute value in the DBCONFIG file is transformed
    # into the value of an element of the dbcfg array using this
    # constantly updated variable.
    set appGlobal(varPrefix) "";
 
    set fileselect [tk_getOpenFile -initialdir $appGlobal(initDir)];
    if {$fileselect == ""} {
	return 2;
    }
    if [catch {open $fileselect r} fileid] {
        tk_messageBox -message $fileid;
	return;
    }
    
    set dbcfg(savefilename) $fileselect;

    # read in the whole file at once
    set sgmlfile [read $fileid];
    close $fileid;

    set sbar .staffcheshire.fdbcfg.dbstatusbar;
    $sbar progress init 50;
    for {set i 1} {$i < 10} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }
    
    # strip off comment lines
    # non-greedy match '*?'
    regsub -all -- {<!--.*?-->} $sgmlfile {} sgmlfile;

    # change double quotes into single quote
    # so they won't get in the way of the intepreter doing substitution.
    # NOTE: "|" is exactly the same as ", did it this way for the
    # correct syntax coloring in Vi or Emacs (balanced double quotes)
    regsub -all -- {("|")} $sgmlfile {'} sgmlfile; 
    # strip off newlines
    regsub -all -- {\n} $sgmlfile {} sgmlfile;

    # strip off the filecont entry lines. For each filedef, only leave
    # one pair of <FILECONT ...> ... </FILECONT> to indicate this
    # filedef uses a directory instead of a single file
    regsub -all -nocase -- {<FILECONT[^<]*?</FILECONT>(?=<FILECONT)} \
	    $sgmlfile {} sgmlfile;

    # strip off spaces between > <
    regsub -all -- {>\s+<} $sgmlfile {><} sgmlfile;

    # strip off spaces before >
    regsub -all -- {\s+>} $sgmlfile {>} sgmlfile;

    # strip off spaces after < 
    regsub -all -- {<\s+} $sgmlfile {<} sgmlfile;

    # strip off white spaces in attribute=value pairs
    regsub -all -- {(\S+)\s*=\s*(\S+)} $sgmlfile {\1=\2} sgmlfile;

    # strip off spaces before or after data
    regsub -all -- {>\s*([^<\s]+)\s*<} $sgmlfile {>\1<} sgmlfile;

    # the following steps are taken to change the parts of the file 
    # that are in this form:

    #<TAGSPEC>
    #<FTAG>FLD130</FTAG><S>^[apnfl]</S>
    #<FTAG>FLD245</FTAG><S>^[ab]</S>
    #<FTAG>FLD490</FTAG> 
    #<FTAG>FLD830</FTAG><S>^[apnfl]</S>
    #</TAGSPEC> 

    # into this form: 

    #<TAGSPEC>
    #<ENTRY>&LT;FTAG&GT;FLD130&LT;/FTAG&GT;&LT;S&GT;^[apnfl]&LT;/S&GT;</ENTRY>
    #<ENTRY>&LT;FTAG&GT;FLD245&LT;/FTAG&GT;&LT;S&GT;^[ab]&LT;/S&GT;</ENTRY>
    #<ENTRY>&LT;FTAG&GT;FLD490&LT;/FTAG&GT;</ENTRY> 
    #<ENTRY>&LT;FTAG&GT;FLD830&LT;/FTAG&GT;&LT;S&GT;^[apnfl]&LT;/S&GT;</ENTRY>
    #</TAGSPEC> 

    # COMPSTARTTAG and COMPONENTSTARTTAG are aliases
    # COMPENDTAG and COMPONENTENDTAG are aliases
    regsub -all -nocase -- {COMPSTARTTAG} $sgmlfile {COMPONENTSTARTTAG} sgmlfile;
    regsub -all -nocase -- {COMPONENTSTART>} $sgmlfile {COMPONENTSTARTTAG>} sgmlfile;

    regsub -all -nocase -- {COMPENDTAG} $sgmlfile {COMPONENTENDTAG} sgmlfile;
    regsub -all -nocase -- {COMPONENTEND>} $sgmlfile {COMPONENTENDTAG>} sgmlfile;

    regsub -all -nocase -- {<FTAG>} $sgmlfile "%</ENTRY><ENTRY><FTAG>" sgmlfile;
    
    #regsub -all -nocase -- {(</(TAGSPEC|COMPONENTSTARTTAG|COMPONENTENDTAG)>)} \
	    # $sgmlfile "%</ENTRY>\\1" sgmlfile;
   
    regsub -all -nocase -- {(</TAGSPEC>)} $sgmlfile "%</ENTRY>\\1" sgmlfile;

    # now remove the first extra %</ENTRY> right after <TAGSPEC> or <COM...>
    #regsub -all -nocase --\
	#    {(<(TAGSPEC|COMPONENTSTARTTAG|COMPONENTENDTAG)>[^<]*)%</ENTRY>} \
	 #   $sgmlfile {\1} sgmlfile;
    
    regsub -all -nocase -- {(<TAGSPEC>[^<]*)%</ENTRY>} $sgmlfile {\1} sgmlfile;

    # now we should get something like this:
    #<TAGSPEC><ENTRY><FTAG>FLD130 </FTAG><S>^[apnfl]</S>%</ENTRY>
    #<ENTRY><FTAG>FLD245</FTAG><S>^[ab]</S>%</ENTRY>
    #<ENTRY><FTAG>FLD490</FTAG>%</ENTRY>
    #<ENTRY><FTAG>FLD830</FTAG><S>^[apnfl]</S>%</ENTRY></TAGSPEC>
    # without the newlines

    # I use the '%' as a flag to help me convert '<' into &LT; and '>' into &GT;
    # inside <ENTRY>.....%</ENTRY> temprarily so the parser will
    # not try to touch these tags: <FTAG> <S> <ATTR>; so the stuff between
    # <ENTRY> and </ENTRY> will be treated as pure data but not tag-enclosed
    # data. Without '%', the reg exp engine won't stop converting right before
    # </ENTRY> 

    # NOTE: if '%' has to be used inside <TAGSPEC> in the CONFIG file, 
    # must choose another character
    # also: <ENTRY> must not appear as a regular tag
    # now do the regional convertion from '<' to &LT; '>' to &GT;
    # on tags inside <ENTRY> ... %</ENTRY>
    set done 0;
    while {!$done} {
        if [regexp {<ENTRY>[^%]*?<[^%]*?%</ENTRY>} $sgmlfile] {
            regsub -all {(<ENTRY>[^%]*?)<([^%]*?%</ENTRY>)} \
                    $sgmlfile {\1\&LT;\2} sgmlfile;
        } elseif [regexp {<ENTRY>[^%]*?>[^%]*?%</ENTRY>} $sgmlfile] {
            regsub -all {(<ENTRY>[^%]*?)>([^%]*?%</ENTRY>)} \
                    $sgmlfile {\1\&GT;\2} sgmlfile;
        } else {
            set done 1; # done
        }
    } 

    for {set i 10} {$i < 20} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }
    # now get rid of the '%' before </ENTRY>
    regsub -all -- {%</ENTRY>} $sgmlfile {</ENTRY>} sgmlfile;
    # now get rid of the <ENTRY> tags between COMPONENTSTARTTAG or ENDTAG
    #regsub -all -- {(<COMPONENT(START|END)TAG>)<ENTRY>} $sgmlfile \
	#    {\1} sgmlfile;
    #regsub -all -- {</ENTRY>(</COMPONENT(START|END)TAG>)} $sgmlfile \
	#    {\1} sgmlfile;

    # construct the new code to be evaluated
    set exp {<(/?)([^\s>]+)\s*([^>]*)>};
    # \1 indicates whether this is a starting or closing tag
    # \2 is the name of this tag
    # \3 is all attribute name value pairs of this tag
    # everything between the starting and ending tags is enclosed in {
    # }
    set sub "\}\nParseOld {\\2} {\\1} {\\3} \{";
    regsub -all -- $exp $sgmlfile $sub sgmlfile;
    # get rid of the first extra right brace and new line 
    regsub {\A\}\n} $sgmlfile {} sgmlfile;
    # get rid of the last extra left brace 
    regsub {\{\Z} $sgmlfile {} sgmlfile;
    # now add a pair of braces at last
    set sgmlfile [append sgmlfile "\{\}"];

    # when parsing each tag using the ParseOld procedure (see below), we
    # need to know where we are currently for some of the sub tags shared by
    # a few outer tags, e.g. the <ENTRY> tag, which is generated by this 
    # procedure to enclose individual <TAGSPEC> entries, can be within <INDXKEY>
    # or <CLUSKEY>, or <FROM>, or <To>. Furthermore, <FROM> and <TO>, which
    # are subtags of <CLUSMAP>, can appear in a <CLUSTERDEF> as well as a 
    # <DISPLAYDEF>.
    # keep track of the number of filedef's
    set appGlobal(inIndxkey) 0;
    set appGlobal(inIndxexc) 0;

    set appGlobal(inCluster) 0;
    set appGlobal(inCluskey) 0;

    set appGlobal(inDisplay) 0;
    set appGlobal(inFormatexc) 0;

    set appGlobal(inComponent) 0;
    set appGlobal(inCompStart) 0;
    set appGlobal(inCompEnd) 0;
    set appGlobal(inFrom) 0;
    set appGlobal(inTo) 0;
    set appGlobal(inSummarize) 0;

    # set control variables to 1 before starting parsing
    set appGlobal(filedefNum) 1;
    ResetFiledefControlVariables;

    for {set i 20} {$i < 30} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }

    #puts $sgmlfile;
    # do the real parsing, construct in memory an associative array
    # representing the DBCONFIG file
    if [catch {eval $sgmlfile} errormsg] {
	tk_messageBox -message "Having trouble parsing the file:\n $errormsg";
	$appGlobal(filemenu) entryconfigure 1 -state normal;
	$appGlobal(filemenu) entryconfigure 4 -state disabled;
	return;
    }

    # goes back to the first page of everything
    set appGlobal(filedefNum) 1;
    ResetFiledefControlVariables;
    
    for {set i 30} {$i < 50} {incr i} {
	$sbar progress set $i;
	update idletasks;
	after 10;
    }
    # activate file type combo and dispoption list
    set dbtabset [find-widget dbtabset .];
    $dbtabset setselection data;

    # since we just load the file and hasn't changed anything
    set appGlobal(dbcfgSaved) 1;
    #$sbar configure -text "Done";
    #set click2 [clock clicks];
    #puts [expr $click2 - $click1];
}


#------------------------------------------------------------------------------
# this procedure parses an existing DBCONFIG file 
#------------------------------------------------------------------------------
proc ParseOld {tag end attribute value} {
    global appGlobal;
    global dbcfg;
    # find all the control variables
    set filedefNum $appGlobal(filedefNum); 
    set indexdefNum $appGlobal(indexdefNum);
    set clusterdefNum $appGlobal(clusterdefNum);
    set displaydefNum $appGlobal(displaydefNum);
    set componentdefNum $appGlobal(componentdefNum);
    set compindexdefNum $appGlobal(compindexdefNum);
    set indexmapNum $appGlobal(indexmapNum);
    set indexkeytagNum $appGlobal(indexkeytagNum);
    set indexexctagNum $appGlobal(indexexctagNum);
    #set clusterkeytagNum $appGlobal(clusterkeytagNum);
    set formatexcludetagNum $appGlobal(formatexcludetagNum);
    set clusmapentryNum $appGlobal(clusmapentryNum);
    set clusmapfromtagNum $appGlobal(clusmapfromtagNum);
    #set varPrefix $appGlobal(varPrefix);

    # if attribute is like: normalize='Mixed Year Range', i.e., there
    # are white spaces inside the attribute value
    if [regexp {='([^\s']+\s+[^']+)'} $attribute match content] {
        # strip off the white spaces in the attribute string, change them 
        # to ~ if necessary so we can split on space later
        regsub -all {\s+} $content {~} content;
	regsub -all {='[^\s']+\s+[^']+'} $attribute "='$content'" attribute;
    }
    # toupper all tags
    set tag [string toupper $tag];

    if {$tag == "DBCONFIG"} {
        return;
    }

    # handle some of the closing tags
    # only need to be concerned with those closing tags that are going
    # to affect the decisions when handling starting tags and those
    # tags that have nested tags
    if {$end == "/"} {
    	switch -exact -- $tag {
    	    FILEDEF
    	    {   # leaving a filedef    	
		set appGlobal(varPrefix) "";
		incr appGlobal(filedefNum);
		return;
    	    }

    	    INDEXDEF
    	    {   # leaving an indexdef
		# change varPrefix back to its previous value before
		# we entered this INDEXDEF. 
    	        set appGlobal(varPrefix) "FILEDEF${filedefNum}_";
		if {$appGlobal(inComponent)} {
		    append appGlobal(varPrefix) \
	            "11COMPONENTS_COMPONENTDEF${componentdefNum}_";
		    incr appGlobal(compindexdefNum);
		} else {
		    incr appGlobal(indexdefNum);
		}
		return;
    	    }

    	    INDXMAP
    	    {   # leaving an indxmap
		# change varPrefix back to its previous value
		# before we entered this INDXMAP
		if {$appGlobal(inComponent)} {
		    set appGlobal(varPrefix) \
		    "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
		    append appGlobal(varPrefix) "${componentdefNum}_";
		    append appGlobal(varPrefix) "COMPONENTINDEXES_";
		    append appGlobal(varPrefix) "INDEXDEF${compindexdefNum}_";
		} else {
		    set appGlobal(varPrefix) \
                    "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}_";
		}
		incr appGlobal(indexmapNum);
		return;
    	    }

            INDXEXC
            {   # leaving an INDXEXC
		set appGlobal(inIndxexc) 0;
		if {$appGlobal(inComponent)} {
		    set appGlobal(varPrefix) \
		    "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
		    append appGlobal(varPrefix) "${componentdefNum}_";
		    append appGlobal(varPrefix) "COMPONENTINDEXES_";
		    append appGlobal(varPrefix) "INDEXDEF${compindexdefNum}_";
		} else {
		    set appGlobal(varPrefix) \
                    "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}_";
		}
		return;
            }

            INDXKEY
            {   # leaving an INDXKEY
                set appGlobal(inIndxkey) 0;
		if {$appGlobal(inComponent)} {
		    set appGlobal(varPrefix) \
		    "FILEDEF${filedefNum}_11COMPONENTS_COMPONENTDEF";
		    append appGlobal(varPrefix) "${componentdefNum}_";
		    append appGlobal(varPrefix) "COMPONENTINDEXES_";
		    append appGlobal(varPrefix) "INDEXDEF${compindexdefNum}_";
		} else {
		    set appGlobal(varPrefix) \
                    "FILEDEF${filedefNum}_9INDEXES_INDEXDEF${indexdefNum}_";
		}
		return;
	    }

    	    CLUSTER -
    	    CLUSTERDEF
    	    {   # cluster and clusterdef are aliases
    	        set appGlobal(inCluster) 0;
    	        set appGlobal(varPrefix) "FILEDEF${filedefNum}_";
    	        incr appGlobal(clusterdefNum);
		return;
    	    }

    	    CLUSKEY
    	    {
    	        set appGlobal(inCluskey) 0;
                set appGlobal(varPrefix) \
                  "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF${clusterdefNum}_";
		return;
            }

    	    FORMAT -
    	    DISPLAYDEF
    	    {   # FORMAT and DISPLAYDEF are aliases
    	        set appGlobal(inDisplay) 0;
    	        set appGlobal(varPrefix) "FILEDEF${filedefNum}_";
    	        incr appGlobal(displaydefNum);
		return;
    	    }

	    EXCLUDE
	    {   # leaving format exclude
		set appGlobal(inFormatexc) 0;
		set appGlobal(varPrefix)\
	          "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF${displaydefNum}_";
		return;
	    }

	    CONVERT
	    {   # leaving format exclude
		# set appGlobal(inConvert) 0;
		set appGlobal(varPrefix)\
	          "FILEDEF${filedefNum}_13DISPLAYS_DISPLAYDEF${displaydefNum}_";
		return;
	    }

	    EXPLAIN
	    {
		set appGlobal(varPrefix) "FILEDEF${filedefNum}_";
		return;
	    }

	    UPDATEINTERVAL -
	    PRODUCERCONTACTINFO -
	    SUPPLIERCONTACTINFO -
	    SUBMISSIONCONTACTINFO
	    {
		set appGlobal(varPrefix) "FILEDEF${filedefNum}_5EXPLAIN_";
		return;
	    }

	    COMPONENTDEF
	    {   # leaving componentdef
		set appGlobal(inComponent) 0;
		set appGlobal(varPrefix) "FILEDEF${filedefNum}_";
		incr appGlobal(componentdefNum);
		return;
	    }

	    COMPONENTSTARTTAG
	    {   
		set appGlobal(inCompStart) 0;
		return;
	    }
	    
	    COMPONENTENDTAG
	    {   
		set appGlobal(inCompEnd) 0;
		return;
	    }

    	    CLUSMAP
    	    {   # leaving CLUSMAP
		if {$appGlobal(inCluster)} {
		    set appGlobal(varPrefix)\
	           "FILEDEF${filedefNum}_10CLUSTERS_CLUSTERDEF${clusterdefNum}_";
		}
		# if inDisplay, </CLUSMAP> will be followed by
		# </CONVERT>, that will set the varPrefix back
		return;
    	    }

    	    FROM
    	    {   # leaving </FROM>
		# if we enter another set of <FROM>...<TO>, will need
		# to set appGlobal(clusmapfromtagNum) to 1
    	        set appGlobal(inFrom) 0;
    	        set appGlobal(clusmapfromtagNum) 1;
		return;
    	    }

    	    TO
    	    {   # leaving </TO>
    	        set appGlobal(inTo) 0;
		# some clusmap entry doesn't have summarize
		# so we need to increment clusmapentryNum here
    	        incr appGlobal(clusmapentryNum);
		return;
    	    }

	    SUMMARIZE
	    {    # leaving </SUMMARIZE>
		set appGlobal(inSummarize) 0;
		return;
	    }
    	}
	# this is a closing tag, so return anyway if no match 
	return;
    }

    switch -exact -- $tag {
	DBENV
	{
	    set dbcfg(Defineenv) 1;
	    set dbcfg(dbenv) $value;
	}

        FILEDEF
        {   # entering a new FILEDEF
            set pre [append appGlobal(varPrefix) "FILEDEF${filedefNum}_"];
            if {$attribute != ""} {
		# split the attribute name value pair
		# FILEDEF only has the 'TYPE' attribute
                regexp {^([^=]+)=([^=]+)$} $attribute match name val;
		# remove ' '
                regexp {'(.+)'} $val match val;
                set name [string toupper $name];
                set dbcfg($pre${name}.a) $val;
            }
            # now that we started a new filedef, let's reset everything
            ResetFiledefControlVariables;
            return;
        }

	DEFAULTPATH
	{
	    # handles the default path
	    set dbcfg(FILEDEF${filedefNum}_0DEFAULTPATH) $value;
	}

	FILECONT
	{
	    set dbcfg(FILEDEF${filedefNum}_singleormulti) multiple;
	    return;
	}
        FILENAME
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}1$tag) $value;
	    return;
        }
        FILETAG
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}2$tag) $value; 
	    return;
	}
        SGMLCAT
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}3$tag) $value; 
	    return;
        }
        SGMLSUBDOC
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}4$tag) $value; 
	    return;
        }
	EXPLAIN
	{   #entering explain
	    # now indicate this explain defined
	    set dbcfg(FILEDEF${filedefNum}_EXPLAINdefined) define;
	    append appGlobal(varPrefix) "5EXPLAIN_";
	    return;
	}
	TITLESTRING
	{
	    ReadinHelper 1 $tag $attribute $value;
	    return;
	}	
	DESCRIPTION
	{
	    ReadinHelper 2 $tag $attribute $value;
	    return;
	}
	DISCLAIMERS
	{
	    ReadinHelper 3 $tag $attribute $value;
	    return;
	}
	NEWS 
	{
	    ReadinHelper 4 $tag $attribute $value;
	    return;
	}
	HOURS 	
	{
	    ReadinHelper 5 $tag $attribute $value;
	    return;
	}
	BESTTIME
	{
	    ReadinHelper 6 $tag $attribute $value;
	    return;
	}
	LASTUPDATE
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}7LASTUPDATE_NULL) $value;
	    return;
	}
	COVERAGE
	{
	    ReadinHelper 8 $tag $attribute $value;
	    return;
	}
	PROPRIETARY
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}9PROPRIETARY_NULL) $value;
	    return;
	}
	COPYRIGHTTEXT
	{
	    ReadinHelper 10 $tag $attribute $value;
	    return;
	}
	COPYRIGHTNOTICE
	{
	    ReadinHelper 11 $tag $attribute $value;
	    return;
	}
	UPDATEINTERVAL
	{
	    append appGlobal(varPrefix) "12UPDATEINTERVAL_";
	    return;
	}
	VALUE
	{	   
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}1VALUE) $value;
	    return;
	}
	UNITS -
	UNIT
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}2UNITS) $value;
	    return;
	}
	PRODUCERCONTACTINFO
	{
	    append appGlobal(varPrefix) "13PRODUCERCONTACTINFO_";
	    return;
	}
	SUPPLIERCONTACTINFO
	{
	    append appGlobal(varPrefix) "14SUPPLIERCONTACTINFO_";
	    return;
	}
	SUBMISSIONCONTACTINFO
	{
	    append appGlobal(varPrefix) "15SUBMISSIONCONTACTINFO_";
	    return;
	}
	CONTACT_NAME
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}1CONTACTNAME) $value;
	    return;
	}
	CONTACT_DESCRIPTION
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}2CONTACTDESCRIPTION) $value;
	    return;
	}
	CONTACT_ADDRESS	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}3CONTACTADDRESS) $value;
	    return;
	}
	CONTACT_EMAIL
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}4CONTACTEMAIL) $value;
	    return;
	}
	CONTACT_PHONE
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}5CONTACTPHONE) $value;
	    return;
	}

        FILEDTD
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}6$tag) $value; 
	    return;
        }
        ASSOCFIL
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}7$tag) $value; 
	    return;
        }
        HISTORY
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}8$tag) $value; 
	    return;
        }

        INDEXDEF
        {
        #----------------------------------------------------------------------
	# indexdef and its sub tags
        #----------------------------------------------------------------------
            # entering a new INDXDEF
	    if {$appGlobal(inComponent)} {
		set pre [append appGlobal(varPrefix)\
			"COMPONENTINDEXES_INDEXDEF${compindexdefNum}_"];
	    } else {
		set pre [append appGlobal(varPrefix)\
		    "9INDEXES_INDEXDEF${indexdefNum}_"];
	    }
	    # take care of attributes
            if {$attribute != ""} {
                set pairs [split $attribute " "];
                foreach pair $pairs {
                    if [regexp {^([^=]+)=([^=]+)$} $pair match name val] {
                        regexp {'(.+)'} $val match val;
                        regsub -all {~} $val { } val;
                        set name [string toupper $name];
                        set dbcfg($pre${name}.a) $val;
                    } else { 
			# this is a boolean attribute
                        set pair [string toupper $pair];
                        set dbcfg($pre${pair}.a) 1;
                    }
                }
            }
            # now that we started a new indexdef, lets reset everything
            ResetIndexdefControlVariables;
            return;
        }
        INDXNAME
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}1$tag) $value; 
	    return;
        }
        INDXTAG 
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}2$tag) $value; 
	    return;
        }
        INDXMAP
        {
            # entering INDXMAP
	    set pre [append appGlobal(varPrefix) "3INDXMAP${indexmapNum}_"];
	    # the attributeset attribute is specified
            if {$attribute != ""} {
                if [regexp {^([^=]+)=([^=]+)$} $attribute match name val] {
                    regexp {'(.+)'} $val match val;
                    set name [string toupper $name];
                    set dbcfg($pre${name}.a) $val;
                }
            }
	    return;
        }
        USE 
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}1$tag) $value; 
	    return;
        }
        RELAT
        {                
            set pre $appGlobal(varPrefix);
	    switch -exact -- $value {
		1 {set value "less than"}
		2 {set value "less or equal"}
		3 {set value "equal"}
		4 {set value "greater or equal"}
		5 {set value "greater than"}
		6 {set value "not equal"}
		100 {set value "phonetic"}
		101 {set value "stem"}
		102 {set value "relevance"}
		103 {set value "always matches"}
	    }
            set dbcfg(${pre}2$tag) $value; 
	    return;
        }
        POSIT
        {                
            set pre $appGlobal(varPrefix);
	    switch -exact -- $value {
		1 {set value "first in field"}
		2 {set value "first in subfield"}
		3 {set value "any position in field"}
	    }
            set dbcfg(${pre}3$tag) $value; 
	    return;
        }
        STRUCT
        {                
            set pre $appGlobal(varPrefix);
	    switch -exact -- $value {
		1 {set value "phrase"}
		2 {set value "word"}
		3 {set value "key"}
		4 {set value "year"}
		5 {set value "date (normalized)"}
		6 {set value "word list"}
		100 {set value "date (un-normalized)"}
		101 {set value "name (normalized)"}
		102 {set value "name (un-normalized)"}
		103 {set value "structure"}
		104 {set value "urx"}
		105 {set value "free-from-text"}
		106 {set value "document-text"}
		107 {set value "local number"}
		108 {set value "string"}
		109 {set value "numeric string"}
	    }
            set dbcfg(${pre}4$tag) $value; 
	    return;
        }
        TRUNC
        {                
            set pre $appGlobal(varPrefix);
	    switch -exact -- $value {
		1 {set value "right truncation"}
		2 {set value "left truncation"}
		3 {set value "left and right"}
		100 {set value "do not truncate"}
	    }
            set dbcfg(${pre}5$tag) $value;
 	    return;
        }
        COMPLET
        {                
            set pre $appGlobal(varPrefix);
	    switch -exact -- $value {
		1 {set value "incomplete subfield"}
		2 {set value "complete subfield"}
		3 {set value "complete field"}
	    }
            set dbcfg(${pre}6$tag) $value; 
	    return;
        }
        STOPLIST 
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}4$tag) $value; 
	    return;
        }
        INDXKEY
        {   # entering INDXKEY
	    # need this state for the tagspec entries
            set appGlobal(inIndxkey) 1;
            append appGlobal(varPrefix) "5INDXKEY_";
	    return;
        }
        INDXEXC
        {   # entering INDXEXC
	    # need the following state for the tagspec entry
	    set appGlobal(inIndxexc) 1;
            append appGlobal(varPrefix) "6INDXEXC_";
	    return;
        }

        CLUSTER -
        CLUSTERDEF 
        {   
        #----------------------------------------------------------------------
	# clusterdef and its sub tags
        #----------------------------------------------------------------------
	    # entering a clusterdef
	    # need this state info for clusmap entry
            set appGlobal(inCluster) 1;
            set pre [append appGlobal(varPrefix) \
                "10CLUSTERS_CLUSTERDEF${clusterdefNum}_"];
            # take care of attributes
	    # CLUSTERDEF actually doesn't have any attributes
            if {$attribute != ""} {
                set pairs [split $attribute " "];
                foreach pair $pairs {
                    if [regexp {^([^=]+)=([^=]+)$} $pair match name val] {
                        regexp {'(.+)'} $val match val;
                        regsub -all {~} $val { } val;
                        set name [string toupper $name];
                        set dbcfg($pre${name}.a) $val;
                    } else {
			# this is a boolean attribute
                        set pair [string toupper $pair];
                        set dbcfg($pre${pair}.a) 1;
                    }
                }
            }
            # now that we started a new indexdef, lets reset everything
            ResetClusterdefControlVariables;
            return;
        }
	CLUSBASE
	{
	    set dbcfg(FILEDEF${filedefNum}_10CLUSTERS_CLUSBASE) $value;
	    return;
	}

        CLUSNAME
        {                
            set pre $appGlobal(varPrefix);
            set dbcfg(${pre}1$tag) $value; 
	    return;
        }
        CLUSKEY
        {   # entering cluskey
	    # need this state for the tagspec entry
            set appGlobal(inCluskey) 1;
            set pre [append appGlobal(varPrefix) "2CLUSKEY_"];
	    # the NORMAL attribute
            if {$attribute != ""} {
                if [regexp {^([^=]+)=([^=]+)$} $attribute match name val] {
                    regexp {'(.+)'} $val match val;
                    set name [string toupper $name];
                    set dbcfg($pre${name}.a) $val;
                }
            }
	    return;
        }
        CLUSMAP
        {   
        #---------------------------------------------------------------
	# NOTE: clusmap can appear inside a "CLUSTERDEF" as well as
	# inside the "CONVERT" subtag of "DISPLAYDEF". So we only
	# reset the control variables here and wait until FROM to 
	# change appGlobal(varPrefix)
	    
        #---------------------------------------------------------------
	    # entering clusmap
            ResetClusmapControlVariables;
	    return;
        }

        FROM
        {   # entering FROM
            set appGlobal(inFrom) 1;
	    if {$appGlobal(inCluster)} {
                set pre "FILEDEF${filedefNum}_10CLUSTERS_";
                set pre "${pre}CLUSTERDEF${clusterdefNum}_5CLUSMAP";
                set appGlobal(varPrefix) "${pre}${clusmapentryNum}_";
            } elseif {$appGlobal(inDisplay)} {
                set pre "FILEDEF${filedefNum}_13DISPLAYS_";
                set pre "${pre}DISPLAYDEF${displaydefNum}_2CONVERT_5CLUSMAP";
                set appGlobal(varPrefix) "${pre}${clusmapentryNum}_";
            }
	    return;
        }
        TO 
        {   # entering TO
	    # varPrefix stays the same as in FROM
            set appGlobal(inTo) 1;
	    return;
        }
	SUMMARIZE
	{   # entering SUMMARIZE
	    # varPrefix stays the same as in FROM
	    set appGlobal(inSummarize) 1;
	    return;
	}
        MAXNUM
        {
            set pre $appGlobal(varPrefix);
            append pre USUMMARIZE_MAXNUM;
            set dbcfg($pre) $value; 
	    return;

        }

	COMPONENTDEF
	{
	#---------------------------------------------------------------------
	# componentdef and its sub tags
	#---------------------------------------------------------------------
	    # entering a componentdef
	    set appGlobal(inComponent) 1;
	    append appGlobal(varPrefix) \
		"11COMPONENTS_COMPONENTDEF${componentdefNum}_";
	    # now that we started a new componentdef, let's reset
	    # every thing for this componentdef
	    # NOTE: this resets compindexdefNum to 1
	    #       then calls ResetIndexdefControlVariables
	    ResetComponentdefControlVariables;
	    return;
	}
	COMPONENTNAME
	{
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}1$tag) $value;
	    return;
	}
	COMPONENTNORM
	{
	    set pre $appGlobal(varPrefix);
	    #puts $pre;
	    set dbcfg(${pre}2$tag) $value;
	    return;
	}
	COMPONENTSTARTTAG
	{   
	    set appGlobal(inCompStart) 1;
	    return;
	}
	COMPONENTENDTAG
	{   
	    set appGlobal(inCompEnd) 1;
	    return;
	}

	DISPOPTIONS
	{
	    regsub -nocase {KEEP_ALL} $value 0 value;
	    regsub -nocase {KEEP_AMP} $value 1 value;
	    regsub -nocase {KEEP_LT} $value 2 value;	   
	    regsub -nocase {KEEP_GT} $value 3 value;
	    set pre $appGlobal(varPrefix);
	    set dbcfg(${pre}12$tag) $value;
	    return;
	}

        FORMAT -
        DISPLAYDEF 
        {
	#----------------------------------------------------------------------
	# displaydef and its sub tags
        #----------------------------------------------------------------------
	    set appGlobal(inDisplay) 1;
            set pre [append appGlobal(varPrefix) \
                "13DISPLAYS_DISPLAYDEF${displaydefNum}_"];
            # take care of attributes
            if {$attribute != ""} {
                set pairs [split $attribute " "];
                foreach pair $pairs {
                    if [regexp {^([^=]+)=([^=]+)$} $pair match name val] {
			regexp {'(.+)'} $val match val;
                        regsub -all {~} $val { } val;
                        set name [string toupper $name];
			if {$name == "OID"} {
			    regsub {1.2.840.10003.5.10$} $val \
				    {MARC Record Syntax} val;			
			    regsub {1.2.840.10003.5.102$} $val \
				    {OPAC Record Syntax} val;
			    regsub {1.2.840.10003.5.101$} $val \
				    {SUTRS Record Syntax} val;
			    regsub {1.2.840.10003.5.109.9$} $val \
				    {SGML Record Syntax} val;
			    regsub {1.2.840.10003.5.109.10$} $val \
				    {XML Record Syntax} val;
			    regsub {1.2.840.10003.5.109.3$} $val \
				    {HTML Record Syntax} val;
			    regsub {1.2.840.10003.5.100$} $val \
				    {EXPLAIN Record Syntax} val;
			    regsub {1.2.840.10003.5.105$} $val \
				    {GSR-1 Record Syntax} val;
			    regsub {1.2.840.10003.5.106$} $val \
				    {Extended Services Record Syntax} val;
			}
                        set dbcfg($pre${name}.a) $val;
                    } else {
			# this is a boolean attribute
                        set pair [string toupper $pair];
                        set dbcfg($pre${pair}.a) 1;
                    }
                }
            }
            # now that we started a new indexdef, lets reset everything
            ResetDisplaydefControlVariables;
            return;
        }
        EXCLUDE
        {   # entering exclude
	    # need this state for the tagspec entry
            set appGlobal(inFormatexc) 1;
            set pre [append appGlobal(varPrefix) "1EXCLUDE_"];
            # the compress attribute
	    if {$attribute != ""} {
                if [regexp {^([^=]+)=([^=]+)$} $attribute match name val] {
                    regexp {'(.+)'} $val match val;
                    set name [string toupper $name];
                    set dbcfg($pre${name}.a) $val;
                } else {
		    # boolean value: compress is true
		    set attribute [string toupper $attribute];
		    set dbcfg($pre${attribute}.a) 1;
		}
            }
	    return;
        }
	CONVERT
	{   # entering convert
	    set pre [append appGlobal(varPrefix) "2CONVERT_"];
            # take care of attributes
	    # ALL and FUNCTION
            if {$attribute != ""} {
                set pairs [split $attribute " "];
                foreach pair $pairs {
                    if [regexp {^([^=]+)=([^=]+)$} $pair match name val] {
                        regexp {'(.+)'} $val match val;
                        regsub -all {~} $val { } val;
                        set name [string toupper $name];
                        set dbcfg($pre${name}.a) $val;
                    } else { 
			# this is a boolean attribute
                        set pair [string toupper $pair];
                        set dbcfg($pre${pair}.a) 1;
                    }
                }
            }
            return;
	}

        ENTRY
        {   # convert &LT; &GT; back to <>
            regsub -all -- {\&LT;} $value {<} value;
            regsub -all -- {\&GT;} $value {>} value;
            set pre $appGlobal(varPrefix);
            if {$appGlobal(inIndxkey)} {
                append pre ${indexkeytagNum}TAGSPEC;
                set dbcfg($pre) $value;
                incr appGlobal(indexkeytagNum);
	    } elseif {$appGlobal(inIndxexc)} {
		append pre ${indexexctagNum}TAGSPEC;
		set dbcfg($pre) $value;
		incr appGlobal(indexexctagNum);
	    } elseif {$appGlobal(inFormatexc)} {
		append pre ${formatexcludetagNum}TAGSPEC;
		set dbcfg($pre) $value;
		incr appGlobal(formatexcludetagNum);
            } elseif {$appGlobal(inCluskey)} {
                append pre TAGSPEC;
                set dbcfg($pre) $value;
                #incr appGlobal(clusterkeytagNum);
            } elseif {$appGlobal(inFrom)} {
                append pre ${clusmapfromtagNum}FROM_TAGSPEC;
                set dbcfg($pre) $value;
                incr appGlobal(clusmapfromtagNum);
            } elseif {$appGlobal(inTo)} {
                append pre TO_TAGSPEC;
                set dbcfg($pre) $value;
            } elseif {$appGlobal(inSummarize)} {
		# in SUMMARIZE
                append pre USUMMARIZE_TAGSPEC;
                set dbcfg($pre) $value; 
            } elseif {$appGlobal(inCompStart)} {
		append pre 3COMPONENTSTARTTAG_TAGSPEC;
		set dbcfg($pre) $value;
	    } elseif {$appGlobal(inCompEnd)} {
		append pre 4COMPONENTENDTAG_TAGSPEC;
		set dbcfg($pre) $value;
	    }
	    return;
        }
    }
}

proc ReadinHelper {num tag attribute value} {
    global appGlobal;
    global dbcfg;
    set pre $appGlobal(varPrefix);
    set dbcfg(${pre}${num}${tag}_NULL) $value; 
    if {$attribute != ""} {
	if [regexp {^([^=]+)=([^=]+)$} $attribute match name val] {
	    regexp {'(.+)'} $val match val;
	    set name [string toupper $name];
	    set dbcfg(${pre}${num}${tag}_${name}.a) $val;
	}
    }
}

#------------------------------------------------------------------------------
# this procedure will pop up a window showing a tree that indicates the current
# place the user is at
#------------------------------------------------------------------------------
proc ShowContext {} {
    global appGlobal;
    global dbcfg;

    toplevel .context;
    set fr [frame .context.fram -background gray95];
    set tree [rtl_tree $fr.tree -width "400" -height "400" -indentx "30" \
        -indenty "25" -background gray95 -creator frame];
    # a button to close the window 
    button $fr.okButton -text OK -command "destroy .context";
    pack $tree;
    pack $fr.okButton;
    pack $fr;

    # root node is DBCONFIG
    $tree insert / -label "DBCONFIG";

    # look at all the elements of the dbcfg array, determine what has been
    # already defined
    set indexes [array names dbcfg];
    set indexes [lsort -dictionary $indexes];

    # starting at filedef 1; no index, cluster, display, component defined yet
    set idnum_1 0; set clunum_1 0; set dispnum_1 0; set compnum_1 0;

    # determine how many filedef's has been entered
    for {set fnum 1}\
	{[info exists dbcfg(FILEDEF${fnum}_DATAdefined)]}\
        {incr fnum} {
        set idnum_$fnum 0;
        set clunum_$fnum 0;
        set dispnum_$fnum 0;
        set compnum_$fnum 0;
    }
    incr fnum -1;

    foreach index $indexes {
        # try to match indexes that look like this:
        # FILEDEF1_9INDEXES_INDEXDEF1 or
        # FILEDEF3_9INDEXES_CLUSTERDEF4
        # extract the filedef num and indexdef(or clusterdef or displaydef) num
        set mat [regexp {^\D+(\d+)_\w+_(\D+)(\d+)} $index match fdefnum whatdef \
                  num];
	if {!$mat} continue;

	#puts $match;
        switch -exact -- $whatdef {
            INDEXDEF
            {
                set var idnum_$fdefnum;
                if {$num > [set $var]} {
                    set $var $num;
                }
            }
 
            CLUSTERDEF
            {
                set var clunum_$fdefnum;
                if {$num > [set $var]} {
                    set $var $num;
                }
            }
 
            DISPLAYDEF
            {
                set var dispnum_$fdefnum;
                if {$num > [set $var]} {
                    set $var $num;
                }
            }

            COMPONENTDEF
            {
                set var compnum_$fdefnum;
                if {$num > [set $var]} {
                    set $var $num;
                }
            }
        }
    }

    # now add other nodes in the tree
    # for each filedef
    for {set i 1} {$i <= $fnum} {incr i} {
        set var1 idnum_$i;
        set var2 clunum_$i;
        set var3 dispnum_$i;
	set var4 compnum_$i;
        $tree insert /FILEDEF$i;
	# there must be a data node for this filedef
        $tree insert /FILEDEF$i/DATA;

	if {$i == $fnum} {
	    set m [expr $i + 1];
	    if {[info exists dbcfg(FILEDEF${m}_DATAdefined)] &&\
		    $dbcfg(FILEDEF${m}_DATAdefined) == "true"} {
		$tree insert /FILEDEF$m;
		$tree insert /FILEDEF$m/DATA;
	    }
	}

	if {[info exists dbcfg(FILEDEF${i}_EXPLAINdefined)]} {         
	    if {$dbcfg(FILEDEF${i}_EXPLAINdefined) == "define"} {
		$tree insert /FILEDEF${i}/EXPLAIN;
	    }
	}

        # are there any indexdef's?
        if {[set $var1] > 0} {
            $tree insert /FILEDEF$i/INDEXES;
            for {set j 1} {$j <= [set $var1]} {incr j} {
                $tree insert /FILEDEF$i/INDEXES/INDEXDEF$j;
            }
        }

        # are there any clusterdef's?
        if {[set $var2] > 0} {
            $tree insert /FILEDEF$i/CLUSTERS;
            for {set l 1} {$l <= [set $var2]} {incr l} {
                $tree insert /FILEDEF$i/CLUSTERS/CLUSTERDEF$l;
            }
        }

        # are there any displaydef's?
        if {[set $var3] > 0} {
            $tree insert /FILEDEF$i/DISPLAYS;
            for {set k 1} {$k <= [set $var3]} {incr k} {
                $tree insert /FILEDEF$i/DISPLAYS/DISPLAYDEF$k;
            }
        }

        # are there any componentdef's?
        if {[set $var4] > 0} {
            $tree insert /FILEDEF$i/COMPONENTS;
            for {set k 1} {$k <= [set $var4]} {incr k} {
                $tree insert /FILEDEF$i/COMPONENTS/COMPONENTDEF$k;
            }
        }
    }

    # specify the callback function 
    $tree configure -selectfcn TreeSelFunc;
    # first determine which filedef we are in, close the other branches 
    set  currFiledefNum $appGlobal(filedefNum);
    for {set m 1} {$m <= $fnum} {incr m} {
        if {$m != $currFiledefNum} {
            $tree close /FILEDEF$m;
        }
    }

    if {[info exists appGlobal(workondata)] &&\
	    $appGlobal(workondata) == 1} {
        $tree configure -select /FILEDEF${currFiledefNum}/DATA;
        $tree close /FILEDEF${currFiledefNum}/INDEXES;
        $tree close /FILEDEF${currFiledefNum}/CLUSTERS;
        $tree close /FILEDEF${currFiledefNum}/DISPLAYS;
        $tree close /FILEDEF${currFiledefNum}/COMPONENTS;
    } elseif {[info exists appGlobal(workonindex)] &&\
	    $appGlobal(workonindex) == 1} {
        set currindexNum $appGlobal(indexdefNum);
        $tree close /FILEDEF${currFiledefNum}/CLUSTERS;
        $tree close /FILEDEF${currFiledefNum}/DISPLAYS;
        $tree close /FILEDEF${currFiledefNum}/COMPONENTS;
        $tree configure -select \
           /FILEDEF${currFiledefNum}/INDEXES/INDEXDEF$currindexNum;
    } elseif {[info exists appGlobal(workoncluster)] &&\
	    $appGlobal(workoncluster) == 1} {
        set currclusterNum $appGlobal(clusterdefNum);
        $tree close /FILEDEF${currFiledefNum}/INDEXES;
        $tree close /FILEDEF${currFiledefNum}/DISPLAYS;
        $tree close /FILEDEF${currFiledefNum}/COMPONENTS;
        $tree configure -select \
           /FILEDEF${currFiledefNum}/CLUSTERS/CLUSTERDEF$currclusterNum;
    } elseif {[info exists appGlobal(workonexplain)] &&\
	    $appGlobal(workonexplain) == 1} {
        $tree configure -select /FILEDEF${currFiledefNum}/EXPLAIN;
        $tree close /FILEDEF${currFiledefNum}/INDEXES;
        $tree close /FILEDEF${currFiledefNum}/CLUSTERS;
        $tree close /FILEDEF${currFiledefNum}/DISPLAYS;
        $tree close /FILEDEF${currFiledefNum}/COMPONENTS;
    } elseif {[info exists appGlobal(workoncomp)] &&\
	    $appGlobal(workoncomp) == 1} {
	set currcomponentNum $appGlobal(componentdefNum);
	$tree close /FILEDEF${currFiledefNum}/INDEXES;
        $tree close /FILEDEF${currFiledefNum}/CLUSTERS;
        $tree close /FILEDEF${currFiledefNum}/DISPLAYS;
        $tree configure -select \
           /FILEDEF${currFiledefNum}/COMPONENTS/COMPONENTDEF$currcomponentNum;
    } elseif {[info exists appGlobal(workondisplay)] &&\
	    $appGlobal(workondisplay) == 1} {
        set currdisplayNum $appGlobal(displaydefNum);
        $tree close /FILEDEF${currFiledefNum}/INDEXES;
        $tree close /FILEDEF${currFiledefNum}/CLUSTERS;
        $tree close /FILEDEF${currFiledefNum}/COMPONENTS;
        $tree configure -select \
           /FILEDEF${currFiledefNum}/DISPLAYS/DISPLAYDEF$currdisplayNum;
    }
    grab .context; 
}

proc TreeSelFunc {base nodes type} {
    global dbcfg;
    global appGlobal;
    set dbtabset [find-widget dbtabset .];
    # click on explain node
    set mat [regexp {^/FILEDEF(\d+)/EXPLAIN$} $nodes match filedefNum];
    if {$mat} {
	set appGlobal(filedefNum) $filedefNum;
	$dbtabset setselection explain;
	return;
    }
    # click on data node
    set mat [regexp {^/FILEDEF(\d+)/DATA$} $nodes match filedefNum];
    if {$mat} {
	set appGlobal(filedefNum) $filedefNum;
	$dbtabset setselection data;
	return;
    }
    # click on other nodes
    set mat [regexp {^/FILEDEF(\d+)/\w+/(\D+)(\d+)} $nodes match filedefNum\
	    whatdef defNum];
    if {$mat} {
	set appGlobal(filedefNum) $filedefNum;
	switch -exact -- $whatdef {
	    INDEXDEF
	    {
		set appGlobal(indexdefNum) $defNum;
		$dbtabset setselection indexes;
	    }
	    CLUSTERDEF
	    {
		set appGlobal(clusterdefNum) $defNum;
		$dbtabset setselection clusters;
	    }
	    DISPLAYDEF
	    {
		set appGlobal(displaydefNum) $defNum;
		$dbtabset setselection display;
	    }
	    COMPONENTDEF
	    {
		set appGlobal(componentdefNum) $defNum;
		$dbtabset setselection component;
	    }
	}
    }	
}
