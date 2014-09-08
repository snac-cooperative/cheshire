#
# tkiwin.tcl :: TkInfo package
#

set tki_header {

Author:		Kennard White <kennard@ohm.eecs.berkeley.edu>
Version:	See the file "tkinfo.tcl"
Copyright:	See the file "tkinfo.tcl"
RCS:		$Header: /users/kennard/src/tkinfo/RCS/tkiwin.tcl,v 1.2 1993/12/23 01:57:38 kennard Exp $

This module provides the windowing (e.g., tk-interface) to tkinfo.
It creates the windows and widgets, sets the bindings, and provides
the binding callback functions.

This module is organized in 3 parts:
    1. callback procs that are called as a result of bindings,
       setting up bindings, and creeating the info windows.
    2. utility funcs for displaying nodes in the info windows
    3. Primary entry points such as tkiWinShow().

In the function names below, I use the abbreviations:
	Show	Display a node specified by a nodeSpec and optional fileSpec.
		This provides the external interface, and requires
		processing by the tkicore functions to retrieve the
		actual data for display.
	Dpy	Display a node specified by a fileKey and an internal
		representation of the node.  This is an internal interface.

} ; set tki_header {

tkInfo may be used in one of three modes: stand alone, embedded or
as a server tool.  These modes are described below:

Stand-alone
	In this mode, the user directly invokes tkinfo, and directly
	manipulates it to display the nodes of interest.  This mode
	requires that the shell script "tkinfo" be proper configured,
	and that the info path be properly configured, either 
	by editing the default info path in "tkinfo.tcl"/tkiInit(),
	or by the user's INFOPATH environment variable.  The file
	"tkinfo.tcl", as well as the built-in help contains additional
	information (command line arguments) for this mode.

Embedded
	In this mode, your application will include tkinfo as part of
	its distribution, and tkinfo will run within the same process
	and the the same TCL interpreter as your application.  tkInfo
	is written with this in mind, and avoids name space pollution.
	In this mode, tkInfo doesn't do anything until the application
	explicitly request an action by calling tkiWinShow(); normally
	the application will do this in response to the user selecting
	a "Help" button or pressing a "Help" key.

	To use this mode, your application must set the global variable
	``tkiEmbed'' to any value and then source "tkinfo.tcl" (the
	auto-load facility may replace explicitly source'ing
	"tkinfo.tcl", but ``tkiEmbed'' must be set before this
	happens).  tkInfo attempts to minimize start up time and will
	not explicitly source all of its files; for this reason, the
	tclIndex must be valid and the auto-load facility active.
	tkiAddInfoPaths() should be called by the application to let
	tkInfo know where the application's info files are installed.
	The application should call tkiWinShow() to display a window.
	Also, the application may find tkiWinContextHelp() useful
	for processing "Help" key bindings.

Server Tool
	From the user's perspective, this is very similar to the
	Embedded mode, but the implementation is different.  In this
	mode, tkInfo runs as in the stand-alone mode, but responds to
	requests from other applications via Tk's "send" mechanism.
	The application must rendezvous with tkInfo (locating the
	existing server or starting a new server running) and makes
	calls to tkiAddInfoPaths() and tkiWinShow() as in the embedded
	case (but via "send").  The application may which tkInfo to
	dedicate a single window to the application, the "window tag"
	feature of tkiWinShow() may be useful for this.

} ; unset tki_header

proc tkiwin.tcl {} {}

##############################################################################
#
#		Binding callbacks and binding creation
#
##############################################################################


#
# There are some rather bizzare interactions with the timing of certain
# events.  In particular, changing the text of a text widget while a
# scan or other mouse-driven functions are active causes problems.  These
# functions provide the ability to defer activity.
#
proc _tkiWinShowIdle_Do { nodeSpec fileSpec w } {
    global tki
#puts stdout "tkiWinShowIdle_Do: $nodeSpec"
    update
    set tw $w.main.text
    tkiWinShow $nodeSpec $fileSpec $w
    $tw conf -cursor $tki(normCursor)
}

proc _tkiWinShowIdle { nodeSpec {fileSpec ""} {w ""} } {
    global tki
    if { "$w"!="" } {
	set tw $w.main.text
	if { "[lindex [$tw conf -cursor] 4]"=="$tki(waitCursor)" } return
	$tw conf -cursor $tki(waitCursor)
    }
#puts stdout "_tkiWinShowIdle: $nodeSpec"
    after 1 [list _tkiWinShowIdle_Do $nodeSpec $fileSpec $w]
}

#
# Support our link to TkMan.  This is based on tkman/contrib/remote.tcl
#
proc _tkiGetInterps { bad_interps } {
    set ips [winfo interps]
    foreach badip $bad_interps {
	if {[set bidx [lsearch -exact $ips $badip]]>=0} {
	    set ips [lreplace $ips $bidx $bidx]
	}
    }
    return $ips
}

#
# Get the Interp for remote tool.  We use the to get TkMan.
#
proc _tkiGetToolInterp { toolName toolProg {initCmd "set manx(init)"} } {
    global errorInfo
    global tki
    set ips_bad ""
    for {set try 0} {$try<5} {incr try} {
	set ips [_tkiGetInterps $ips_bad]
	if {[set found [lsearch -glob $ips $toolProg*]]>=0} {
	    # Its already running...
	    set tool_ip [lindex $ips $found]
	    tkiStatus "$toolName probe($try)..."
	    if [catch {send $tool_ip $initCmd} initB] {
		# Assume its bogus and get another one
		lappend ips_bad $tool_ip; continue
	    }
	} else {
	    # if tool doesn't already exist, start one up
	    tkiStatus "$toolName exec($try)..."
	    if [catch {exec $tki($toolProg) &} error] {
		error "Error during exec: $error" $errorInfo
	    }
	    # Wait at least 5 secs (take long time to init, so no hurry)
	    after 5000

	    # wait for it to be registered.  The new tool might
	    # get registered under the name of a previously bogus name
	    set ips_bad ""
	    tkiStatus "$toolName register($try)..."
	    for {set found -1; set cnt 0} {$found==-1} {incr cnt; after 1000} {
		if {$cnt > 10} { error "Timeout during interp registration." }
		set ips [_tkiGetInterps $ips_bad]
		set found [lsearch -glob $ips $toolProg*]
	    }
	    set tool_ip [lindex $ips $found]
	}
	    
	# wait for it to initialize.  Give it 5sec to get its main window
	# up, but 30sec to init
	tkiStatus "$toolName initialize($try)..."
	for {set cnt 0} {1} {incr cnt; after 1000} {
	    if {$cnt > 30} { error "Timeout during initialization." }
	    if [catch {send $tool_ip $initCmd} initB] {
		# The send can fail for two reasons: either tool_ip
		# is no longer alive, or it is still starting up
	        if {$cnt > 5} { break }
		continue
	    }
	    if {"$initB"=="1"} {
		# yeah, we've got a working tool
		return $tool_ip
	    }
	}
	lappend ips_bad $tool_ip; continue
    }
    error "Too many dead interps."
}

proc _tkiWinManPage { w manpage } {
    upvar #0 $w wvars
    global tki

    set tki(curWindow) $w
    if [catch {_tkiGetToolInterp TkMan tkman} man_ip] {
	tkiError "TkMan-tool rendezvous error: $man_ip\n(Try again after tkman has initialized.)"
	return
    }
    # .man is the main window, guaranteed to exist
    tkiStatus "TkMan raise..."
    if [catch {send $man_ip raise .man} error] {
	tkiError "TkMan-tool raise error: $error"
	return
    }
    tkiStatus "TkMan show..."
    if [catch {send $man_ip manShowMan $manpage} error] {
	tkiError "TkMan-tool show error: $error"
	return
    }
    tkiStatus $wvars(status)
    set tki(curWindow) ""
}

#
# Various functions for manipulating the "prompting" window.  This
# is the entry widget at the bottom of the info window used for entering
# node names and search text.
#

proc _tkiWinPromptMap { w mode extra } {
    upvar #0 $w wvars
    set wvars(promptmode) $mode
    set dd $w.s
    $dd.goto delete 0 end
    $dd.goto insert end $extra
    pack after $dd.status $dd.goto { left expand fill }
    switch $mode {
      search {
	pack after $dd.goto $dd.regexp { left filly }
	pack after $dd.regexp $dd.case { left filly }
      }
    }
    focus $dd.goto
}

proc _tkiWinPromptUnmap { w } {
    upvar #0 $w wvars
    set wvars(promptmode) ""
    set dd $w.s
    focus $w.main.text
    pack unpack $dd.goto
    pack unpack $dd.regexp
    pack unpack $dd.case
    $dd.status conf -text $wvars(status)
}

#
# This is called when <Return> is pressed in the "goto" text window.
# We could either be in a goto-node command, or a exec-tcl command.
# We take the appropriate action and cleanup.
#
proc _tkiWinPromptOk { w } {
    global tki
    upvar #0 $w wvars
    set dd $w.s
    set tw $w.main.text
    set node [string trim [$dd.goto get]]
    switch $wvars(promptmode) {
      search {
	set wvars(searchStr) $node
	set tki(curWindow) $w
        $tw conf -cursor $tki(waitCursor)
	set cnt [searchboxSearch $wvars(searchStr) $wvars(searchRegexpB) \
	  $wvars(searchCaseB) searchkey $w.main.text $w.main.vsb]
	if {$cnt==0} {after 1000}
	set tki(curWindow) ""
        _tkiWinPromptUnmap $w
        $tw conf -cursor $tki(normCursor)
      }
      goto {
	set wvars(gotoStr) $node
        _tkiWinPromptUnmap $w
	_tkiWinShowIdle $node $wvars(fileKey) $w
      }
      tclcmd {
	set wvars(tclcmdStr) $node
	if { "$node"!="" } {
	    if [catch "uplevel #0 $node" error] {
		puts stdout "Error: $error"
	    } else {
		puts stdout [expr { "$error"=="" ? "Ok" : "$error" }]
	    }
	    set node ""
	}
        _tkiWinPromptUnmap $w
      }
      manual {
	_tkiWinPromptUnmap $w
	if { "$node"!="" } {
	    set wvars(manualStr) $node
            _tkiWinManPage $w $node
	}
      }
    }
}

proc _tkiWinPromptAbort { w } {
    upvar #0 $w wvars
    _tkiWinPromptUnmap $w
}

#
# Perform various actions on the info window.  
# Note that if the action requires prompting (searching or goto-node)
# then we have to play with the focus.  This can badly interact with
# the focus games played when unmapping popup menus, so the "idle"
# option should be used when called from a menu.
#
proc _tkiWinAction { w args } {
    upvar #0 $w wvars
    global tki

    set toNode ""
    set toFile $wvars(fileKey) 

    set arg1 [lindex $args 1]
    case [lindex $args 0] {
      idle {
	after 1 _tkiWinAction $w [lrange $args 1 end]
      }
      quit {
	catch {unset wvars}
	catch {destroy $w}
	# XXX: !!This is a major hack!!
	global tkiEmbed
	if { ![info exist tkiEmbed] && "[winfo children .]"=="" } {
	    destroy .
	}
	return
      }
      goto {
	set dd $w.s
	$dd.status conf -text "Enter node: "
	if { "$arg1"=="" } { set arg1 $wvars(gotoStr) }
	_tkiWinPromptMap $w goto $arg1
      }
      tclcmd {
	set dd $w.s
	$dd.status conf -text "Enter cmd: "
	if { "$arg1"=="" } { set arg1 $wvars(tclcmdStr) }
	_tkiWinPromptMap $w tclcmd $arg1
      }
      search {
	if { "$arg1"=="incr" } {
	    set tki(curWindow) $w
	    searchboxNext searchkey $w.main.text $w.main.vsb
	    set tki(curWindow) ""
    	    $w.s.status conf -text $wvars(status)
	} else {
	    set wvars(searchRegexpB) [expr {"$arg1"=="regexp"}]
	    #set wvars(searchCaseB) 0
	    $w.s.status conf -text "Search for: "
	    _tkiWinPromptMap $w search $wvars(searchStr)
	}
      }
      manual {
    	if {[catch {selection get} manpage] || "$manpage"=="" } {
	    $w.s.status conf -text "Enter man page: "
	    _tkiWinPromptMap $w manual $wvars(manualStr)
	} else {
	    _tkiWinManPage $w $manpage
	}
      }
      last {
	set idx [expr { [llength $wvars(lastNodes)] - 2 } ]
	if { $idx >= 0 } {
	    set lastinfo [lindex $wvars(lastNodes) $idx]
	    set wvars(lastNodes) [lreplace $wvars(lastNodes) $idx end]
	    set toFile [lindex $lastinfo 0]
	    set toNode [lindex $lastinfo 1]
	}
      }
      toggle {
	if { [info exist wvars(toggle)] } {
	    set toFile [lindex $wvars(toggle) 0]
	    set toNode [lindex $wvars(toggle) 1]
	}
      }

      up   { set toNode [lindex $wvars(nodeinfo) 3] }
      prev { set toNode [lindex $wvars(nodeinfo) 4] }
      next { set toNode [lindex $wvars(nodeinfo) 5] }
      menu {
	if { ![info exist wvars(menuinfo)] } return
	if { "$arg1"=="" } {
            set tki(curWindow) $w
	    tkiError "The ``menu'' command is not (yet) implemented."
	} else {
	    set menuitem [lindex $wvars(menuinfo) $arg1]
	    set toNode [lindex $menuitem 2]
	}
      }
      xref {
        set tki(curWindow) $w
        tkiError "The ``follow xref'' command is not (yet) implemented."
      }
      scroll {
	set sbinfo [$w.main.vsb get]
	set windelta [expr {[lindex $sbinfo 1]-1}]
	set lastpage [expr {[lindex $sbinfo 0]-[lindex $sbinfo 1]}]
	case $arg1 {
	  forw { set gotoline [expr {[lindex $sbinfo 2]+$windelta}] }
	  back { set gotoline [expr {[lindex $sbinfo 2]-$windelta}] }
	  top { set gotoline 0 }
	  bottom { set gotoline $lastpage }
	  menu {
	    if ![info exist wvars(menuinfo)] return
	    set gotoline [$w.main.text index "menu.first"]
	  }
	}
	if { $gotoline > $lastpage } { set gotoline $lastpage }
	if { $gotoline < 0 } { set gotoline 0 }
	$w.main.text yview $gotoline
      }
    }
    if { "$toNode"!="" } {
        _tkiWinShowIdle $toNode $toFile $w
    }
}


#
# Utility function for turning the "-accelerator" options from
# menus into actual bindings.
# Traverse {menu}, and install accelerators onto {winSpec}.
# {winSpec} may be a list of windows.  {menu} may be a menu, a
# menu button, or a frame containing menu buttons.
# Accelerator sequences may be any sequence of "normal" characters,
# or a normal char prefixed by "^" for Control.
# This code is cut&pasted from "tkgraph/lib/topwin.tcl topWin.BindAccels()".
#
proc _tkiBindAccels { winSpec menu } {
    switch [winfo class $menu] {
      Frame {
	foreach submenu [winfo children $menu] {
	    _tkiBindAccels $winSpec $submenu
	}
      }
      Menubutton {
	_tkiBindAccels $winSpec [lindex [$menu conf -menu] 4]
      }
      Menu {
	set lastIdx [$menu index last]
	for {set idx 0} {$idx <= $lastIdx} {incr idx} {
	    if [catch {$menu entryconf $idx -acc} acc] continue
	    set acc [lindex $acc 4]
	    if { "$acc"!="" && "$acc"!="==>" } {
		regsub -all "\\^(.)" $acc "<Control-\\1>" acc
		regsub -all "<(.)>" $acc "<Key-\\1>" acc
		foreach win $winSpec {
		    bind $win $acc "$menu invoke $idx"
		}
	    }
	    if { ! [catch {$menu entryconf $idx -menu} submenu] } {
		set submenu [lindex $submenu 4]
		if { "$submenu"!="" } {
		    _tkiBindAccels $winSpec $submenu
		}
	    }
	}
      }
    }
}

proc _tkiWinBind { w } {
    set tw $w.main.text
    _tkiBindAccels "$w.main.text $w.s.status" $w.bar
    foreach win "$w.main.text $w.s.status" {
	bind $win (		"_tkiWinAction $w goto ("
	bind $win <Key-Help>	"tkiContextHelp %W"
	bind $win <Key-1>	"_tkiWinAction $w menu 0"
	bind $win <Key-2>	"_tkiWinAction $w menu 1"
	bind $win <Key-3>	"_tkiWinAction $w menu 2"
	bind $win <Key-4>	"_tkiWinAction $w menu 3"
	bind $win <Key-5>	"_tkiWinAction $w menu 4"
	bind $win <Key-6>	"_tkiWinAction $w menu 5"
	bind $win <Key-7>	"_tkiWinAction $w menu 6"
	bind $win <Key-8>	"_tkiWinAction $w menu 7"
	bind $win <Key-9>	"_tkiWinAction $w menu 8"
	bind $win <Key-space>		"_tkiWinAction $w scroll forw"
	bind $win <Control-Key-f>	"_tkiWinAction $w scroll forw"
	bind $win <Control-Key-v>	"_tkiWinAction $w scroll forw"
	bind $win <Key-F35>		"_tkiWinAction $w scroll forw"
	bind $win <Key-Delete>		"_tkiWinAction $w scroll back"
	bind $win <Key-BackSpace>	"_tkiWinAction $w scroll back"
	bind $win <Control-Key-b>	"_tkiWinAction $w scroll back"
	bind $win <Alt-Key-v>		"_tkiWinAction $w scroll back"
	bind $win <Key-F29>		"_tkiWinAction $w scroll back"
    }
    # XXX: this focus junk should really be ripped out and replaced by
    # a system like tkaux.
#    bind $w <Any-Enter> "focus $w.main.text"
#    bind $w <Any-Leave> "focus none"
    focus $w.main.text
}


#
# Make a new toplevel info window (with class ``TkInfo''),
# filled with buttons and bindings.
#
# If the argument {w} is non-empty, it specifies either the path name
# of the info window to create (if {w} doesn't already exist),
# or the parent of the info window to create (if {w} does already exist).
# It is an error for both {w} and {w}'s parent to not exist.
# If {w} is empty, the info window will be created as a child of the
# root window.
#
# If given, {tag} is some text that will appear in the window title and
# icon title.
#
# The path name of the new info window will be returned.
#
proc tkiWinCreate { {w ""} {tag ""} } {
    global tki defaultBG defaultActiveBG buttonBG textBG selectFG entryFont
    global defaultFont

    if { "$w"=="" || [winfo exist $w] } {
	if { "$w"!="" && "[winfo class $w]"=="TkInfo" } {
	    # This check isn't strictly required, but it helps catch
	    # problems with Tk's multi-phase window destruction process.
	    error "Can't nest TkInfo windows."
	}
	set parent $w
	while 1 {
	    # I think (but I dont really remember) that I use [winfo parent]
	    # here instead of [winfo exist] b/c multi-phase destroy.
	    set w $parent.tki[tkiGetSN]
	    if { [catch {winfo parent $w}] } break
	}
    }
    lappend tki(windows) $w
    upvar #0 $w wvars
    set wvars(nodeinfo) ""
    set wvars(fileKey) ""
    set wvars(infonodename) "(builtin)Top"
    set wvars(status) "Press ? for help."
    set wvars(gotoStr) ""
    set wvars(tclcmdStr) ""
    set wvars(promptmode) ""
    set wvars(searchStr) ""
    set wvars(manualStr) ""
    set wvars(title) [expr {("$tag"=="") ? "tkInfo" : "tkInfo:$tag"}]

    toplevel $w -class TkInfo
    wm title $w $wvars(title)
    wm iconname $w $wvars(title)
    wm minsize $w 20 20
    wm protocol $w WM_DELETE_WINDOW "_tkiWinAction $w quit"
    set dd $w.bar; pack [frame $dd -borderwidth 2 -relief raised] \
      -side top -fill x

    set ddm $dd.file.m
    pack [menubutton $dd.file -text "File" -und 0 -menu $ddm -font $defaultFont] -side left
    menu $ddm -tearoff 0 -font $defaultFont
#    $ddm add com -lab "Directory" -und 0 -acc d -com [list _tkiWinShowIdle "(dir)Top" "" $w]
#    $ddm add com -lab "Master Dir." -und 0 -acc D -com [list _tkiWinShowIdle "(builtin)Top" "" $w]
#    $ddm add com -lab "New Browser" -und 0 -com "tkiInfoWindow"
# Yeah, right, I'm going to let the user execute any Tcl Command they want.
# *Sure* I am.
#    $ddm add com -lab "Tcl Cmd..." -und 0 -acc ! -com "_tkiWinAction $w idle tclcmd"
    $ddm add com -lab "Quit" -und 0 -acc q -com "pLogCmd 34; _tkiWinAction $w quit"

    set ddm $dd.node.m
    pack [menubutton $dd.node -text "Node" -und 0 -menu $ddm -font $defaultFont] -side left
    menu $ddm -tearoff 0 -font $defaultFont
    $ddm add com -lab "Up" -und 0 -acc u -com "_tkiWinAction $w up"
    $ddm add com -lab "Next" -und 0 -acc n -com "_tkiWinAction $w next"
    $ddm add com -lab "Prev" -und 0 -acc p -com "_tkiWinAction $w prev"
    $ddm add com -lab "Goto..." -und 0 -acc g -com "_tkiWinAction $w idle goto"
    $ddm add com -lab "Menu..." -und 0 -acc m -com "_tkiWinAction $w idle menu"
    $ddm add com -lab "Xref..." -und 0 -acc f -com "_tkiWinAction $w idle xref"
    $ddm add com -lab "Toggle" -und 0 -acc t -com "_tkiWinAction $w toggle"
    $ddm add com -lab "Last" -und 0 -acc l -com "_tkiWinAction $w last"
    $ddm add sep
    $ddm add com -lab "Manual" -und 0 -acc M -com "_tkiWinAction $w manual"

    set ddm $dd.view.m
    pack [menubutton $dd.view -text "View" -und 0 -menu $ddm -font $defaultFont] -side left
    menu $ddm -tearoff 0 -font $defaultFont
    $ddm add com -lab "Top" -und 0 -acc b -com "_tkiWinAction $w scroll top"
    $ddm add com -lab "Bottom" -und 0 -acc e -com "_tkiWinAction $w scroll bottom"
    $ddm add com -lab "Menu" -und 0 -acc ^m -com "_tkiWinAction $w scroll menu"
    $ddm add com -lab "Search (exact)..." -und 0 -acc s \
      -com "_tkiWinAction $w idle search exact"
    $ddm add com -lab "Search (regexp)..." -und 8 -acc / \
      -com "_tkiWinAction $w idle search regexp"
    $ddm add com -lab "Search next" -und 7 -acc ^s \
      -com "_tkiWinAction $w search incr"

#    set ddm $dd.options.m
#    pack [menubutton $dd.options -text "Options" -und 0 -menu $ddm -font $defaultFont] -side left
#    menu $ddm -tearoff 0 -font $defaultFont
#    $ddm add check -lab "Show Headers" -und 5 -var tki(rawHeadersB) \
#	-selectcolor $selectFG
#    $ddm add check -lab "Show Buttons" -und 5 -var tki(showButtonsB) \
	-selectcolor $selectFG
#    $ddm add check -lab "Detailed Status" -und 0 -var tki(detailstatusB) \
	-selectcolor $selectFG
#    $ddm add check -lab "Time Status" -und 0 -var tki(timestatusB) \
	-selectcolor $selectFG
#    $ddm add sep
#    $ddm add com -lab "Node Look" -state disabled
#    $ddm add radio -lab "Color" -und 0     -var tki(nodelook) -val color \
	-selectcolor $selectFG
#    $ddm add radio -lab "Font" -und 0      -var tki(nodelook) -val font \
	-selectcolor $selectFG
#    $ddm add radio -lab "Underline" -und 0 -var tki(nodelook) -val underline \
	-selectcolor $selectFG

    set ddm $dd.help.m
    pack [menubutton $dd.help -text "Help" -und 0 -menu $ddm -font $defaultFont] -side left
    menu $ddm -tearoff 0 -font $defaultFont
    $ddm add com -lab "tkInfo Info" -und 0 -acc h \
      -com [list _tkiWinShowIdle "(builtin)Top" "" $w]
    $ddm add com -lab "tkInfo Summary" -und 7 -acc ? \
      -com [list _tkiWinShowIdle "(builtin)QuickHelp" "" $w]
    $ddm add sep
    $ddm add com -lab "About" -und 0 \
      -com [list _tkiWinShowIdle "(builtin)About" "" $w]

    tk_menuBar $w.bar $w.bar.file $w.bar.view $w.bar.options $w.var.help

    set dd $w.main; pack [frame $dd] -expand 1 -fill both
    pack append $dd [text $dd.text -state disabled -width 80 -wrap word \
      -background $textBG -highlightbackground $defaultBG -font $entryFont \
      -highlightcolor $defaultBG] { left expand fill }
    pack append $dd [scrollbar $dd.vsb -orient vert -com "$dd.text yview" \
      -background $defaultBG -activebackground $defaultActiveBG \
      -troughcolor $buttonBG -highlightcolor $buttonBG \
      -highlightbackground $defaultBG] { left fill }
    $dd.text config -yscroll "$dd.vsb set"

    set dd $w.div1; pack append $w [frame $dd -bd 1 -rel sunken \
      -height 3 -width 10] { top fillx }
    set dd $w.s; pack append $w [frame $dd] { top fillx }
    pack append $dd [label $dd.status -anc w -font $defaultFont] { left fillx }
#    place [label $dd.status -anc w] -in $dd -x 0 -y 0 -anc nw
#    pack append $dd [label $dd.forhelp -anc e -text "Press ? for help."] \
#      { right filly }
    place [label $dd.forhelp -anc e -text "Press ? for help." -font $defaultFont] \
      -in $dd -relx 1 -y 0 -anc ne
    entry $dd.goto -rel sunken
    checkbutton $dd.regexp -text "Regexp" -var ${w}(searchRegexpB) \
	-selectcolor $selectFG -font $defaultFont
    checkbutton $dd.case -text "Case Sen" -var ${w}(searchCaseB) \
	-selectcolor $selectFG -font $defaultFont
    bind $dd.goto <Return> "_tkiWinPromptOk $w"
    bind $dd.goto <Escape> "_tkiWinPromptAbort $w"
    bind $dd.goto <Any-Control-g> "_tkiWinPromptAbort $w"

    set dd $w.buts; frame $dd
    if { $tki(showButtonsB) } { pack conf $dd -fill x }
    pack append $dd [button $dd.prev -text "Prev Node" \
      -com "_tkiWinAction $w prev" -bg $buttonBG \
      -highlightbackground $defaultBG -font $defaultFont] { left exp fill }
    pack append $dd [button $dd.up   -text "Up Node" \
      -com "_tkiWinAction $w up" -bg $buttonBG \
      -highlightbackground $defaultBG -font $defaultFont] { left exp fill }
    pack append $dd [button $dd.next -text "Next Node" \
      -com "_tkiWinAction $w next" -bg $buttonBG \
      -highlightbackground $defaultBG -font $defaultFont] { left exp fill }

#    pack append $dd [button $dd.toggle -text "Toggle Node" \
#      -com "_tkiWinAction $w back" -state disabled] { left exp fill }

    _tkiWinBind $w

    set tki(curWindow) $w
    return $w
}

##############################################################################
#
#		Utility functions for updating info windows
#
##############################################################################

#
# This is more subtle than one might think.  Note that the text index
# "+1line" wont work on the last line of text, because the newline is
# considered part of the previous line.  Thus we use "lineend" instead.
#
proc _tkiTextTrim { w idx } {
    while 1 {
	set nidx [$w index "$idx lineend"]
	if { "[string trim [$w get $idx $nidx]]"!="" || "[$w index end]"=="1.0" } break
	$w delete $idx "$nidx +1char"
    }
}

# Modified version of ouster's version
proc _tkiTextInsertWithTags { w index text args } {
    set start [$w index $index]
    $w insert $start $text
    foreach tag $args {
    	$w tag add $tag $start insert
    }
}

proc _tkiConfChainButton { w which toNode } {
    set mode [expr { "$toNode"=="" ? "disabled" : "normal" } ]
    $w.buts.$which conf -state $mode
    $w.bar.node.m entryconf ?[string range $which 1 end] -state $mode
}

proc _tkiNodeLookTag { tw tag } {
    global tki
    case $tki(nodelook) {
      color { $tw tag conf $tag -fore $tki(nodelookColor) }
      underline { $tw tag conf $tag -underline 1 }
      font { $tw tag conf $tag -font $tki(nodelookFont) }
    }
}

proc _tkiNodeLookChange { tw tag state} {
    global tki HTFG HTActiveFG
    if {$tki(nodelook) == "color"} {
	if {$state == "active"} {
	    $tw tag configure $tag -foreground $HTActiveFG
	}
	if {$state == "inactive"} {
	    $tw tag configure $tag -foreground $HTFG
	}
    }
}

proc _tkiWinDpyBody { w nodeName nodeIdx fileKey bodytext } {
    global tki

    set tw $w.main.text
    $tw insert end $bodytext

    if { [info exist tki(xrefinfo-$fileKey-$nodeIdx)] } {
	set xrefinfo $tki(xrefinfo-$fileKey-$nodeIdx)
    } else {
	tkiDetailStatus "Parsing $nodeIdx body"
        set xrefinfo [tkiTimeStatus "Parsing $nodeIdx body" 0 \
	  tkiNodeParseBody $nodeName $fileKey $bodytext]
	set tki(xrefinfo-$fileKey-$nodeIdx) $xrefinfo
    }
    tkiDetailStatus "Formating $nodeIdx body"
    set ms "1.0"
    $tw tag delete xrefkey
    foreach xi $xrefinfo {
	# xi = { xrefidx toNode startIdx endIdx }
	set xrefidx [lindex $xi 0]
	set toNode [lindex $xi 1]
        $tw tag add xrefkey "$ms +[lindex $xi 2] c" "$ms +[lindex $xi 3] c"
        $tw tag add xref$xrefidx "$ms +[lindex $xi 2] c" "$ms +[lindex $xi 3] c"
	$tw tag bind xref$xrefidx <ButtonRelease-1> [list _tkiWinShowIdle $toNode $fileKey $w]
    }
    _tkiNodeLookTag $tw xrefkey

    _tkiTextTrim $tw 1.0
    if { ! $tki(rawHeadersB) } {
	$tw delete 1.0 "1.0 +1line"
        _tkiTextTrim $tw 1.0
    }
}

proc _tkiWinDpyMenu { w nodeName nodeIdx fileKey menutext } {
    global tki
#puts stdout "_tkiWinDpyMenu: $nodeName"
    if { [info exist tki(menuinfo-$fileKey-$nodeIdx)] } {
	set menuinfo $tki(menuinfo-$fileKey-$nodeIdx)
    } else {
        set menuinfo [tkiTimeStatus "Parsing $nodeIdx menu" 0 \
	  tkiNodeParseMenu $nodeName $fileKey $menutext]
	set tki(menuinfo-$fileKey-$nodeIdx) $menuinfo
    }
    tkiDetailStatus "Formating $nodeIdx menu"
    set tw $w.main.text
    _tkiTextInsertWithTags $tw end $menutext menu
    $tw tag delete menukey
    foreach mi $menuinfo {
#puts stdout "$mi"
	# mi = { lineidx menuidx toNode nBeg nEnd }
	set lineidx [lindex $mi 0]
	set menuidx [lindex $mi 1]
	set toNode [lindex $mi 2]
	set ms "menu.first +$lineidx lines -2 lines"
	$tw tag add menukey "$ms +[lindex $mi 3] c" "$ms +[lindex $mi 4] c +1 c"
	$tw tag add menu$menuidx "$ms linestart" "$ms lineend"
	$tw tag bind menu$menuidx <ButtonRelease-1> [list _tkiWinShowIdle $toNode $fileKey $w]
	$tw tag bind menu$menuidx <Enter> "_tkiNodeLookChange $tw \
           menu$menuidx \"active\""
	$tw tag bind menu$menuidx <Leave> "_tkiNodeLookChange $tw \
           menu$menuidx \"inactive\""

#	$tw tag bind menu$menuidx <Key> {puts stdout "key %A press"}
    }
    _tkiNodeLookTag $tw menukey
}

proc _tkiWinDpyNode { w fileKey info body } {
    global tki; upvar #0 $w wvars
    global C2_helpinfo1 C2_helpinfo2 C2_helpinfo3 C2_nodenames

    set wvars(fileKey) $fileKey
    set wvars(nodeinfo) $info
    set nodeIdx [lindex $info 0]
    set nodeSpec "([lindex $info 2])[lindex $info 1]"
    tkiStatus "Formating $nodeSpec"

    # additional hacks for cheshire 2 logging
    if {"[lindex $info 2]" == "cheshire2.info"} {
	set C2_helpinfo1 2
	foreach C2l $C2_nodenames(cheshire2.info) {
	    if {"[lindex $C2l 0]" == "[lindex $info 1]"} {
		set C2_helpinfo2 "[lindex $C2l 1]"
	    }
	}
    } elseif {[lindex $info 2] == "search.info"} {
	set C2_helpinfo1 3
	foreach C2l $C2_nodenames(search.info) {
	    if {"[lindex $C2l 0]" == "[lindex $info 1]"} {
		set C2_helpinfo2 "[lindex $C2l 1]"
	    }
	}

    } else {
	set C2_helpinfo1 999
	set C2_helpinfo2 999
    }
    set C2_helpinfo3 $nodeSpec
    pLogCmd 4

    set tw $w.main.text
    $tw conf -cursor $tki(waitCursor)
    set menuidx -1
    if { ! [lindex $tki(fileinfo-$fileKey) 4] } {
	set menuidx [string first "\n* Menu:" $body]
	if { $menuidx > 0 } {
	    set menutext [string range $body [expr {$menuidx+1}] end]
	    set body [string range $body 0 $menuidx]
	}
    }

    #
    # Config the up,prev,next headers
    #
    set nodeName [lindex $info 1]
    _tkiConfChainButton $w up   [lindex $info 3]
    _tkiConfChainButton $w prev [lindex $info 4]
    _tkiConfChainButton $w next [lindex $info 5]

    #
    # Config the main section
    #
    $tw conf -state normal
    $tw delete 1.0 end
    tkiTimeStatus "DpyNode body" 1 \
      _tkiWinDpyBody $w $nodeName $nodeIdx $fileKey $body
    if { [info exist menutext] } {
	tkiTimeStatus "DpyNode menu" 1 \
	  _tkiWinDpyMenu $w $nodeName $nodeIdx $fileKey $menutext
	set wvars(menuinfo) $tki(menuinfo-$fileKey-$nodeIdx)
	$w.bar.view.m entryconf Menu -state normal
    } else {
	catch {unset wvars(menuinfo)}
	$w.bar.view.m entryconf Menu -state disabled
    }
    tkiDetailStatus "Formating $nodeSpec (part 2)"
    $tw mark set insert 1.0
    $tw mark set anchor insert
    $tw tag remove sel 1.0 end
    $tw conf -state disabled -cursor $tki(normCursor)
#   set wvars(status) "$nodeSpec                           Press ? for help."
    set wvars(status) "$nodeSpec"
    wm title $w "$wvars(title)      $nodeSpec"
    tkiStatus $wvars(status)

    # Do search colors
    $tw tag conf searchkey -foreground [lindex [$tw conf -background] 4] \
      -background [lindex [$tw conf -foreground] 4]

    # This is really gross
    focus $tw
    after 1 [list $tw tag remove sel 1.0 end]
}


proc tkiWinDpy { w fileKey info body } {
    global tki; upvar #0 $w wvars

    if { [info exist wvars(nodeinfo)] } {
	set wvars(toggle) [list $wvars(fileKey) [lindex $wvars(nodeinfo) 1]]
#        $w.buts.toggle conf -state normal
    }
    lappend wvars(lastNodes) [list $fileKey [lindex $info 1]]

    _tkiWinDpyNode $w $fileKey $info $body
}

##############################################################################
#
#	The public interface
#
##############################################################################


#
# The argument {w} specified an info window in one of three ways:
#   - if empty, a new top-level window will be created and returned.
#   - if a window (starts will a ``.''), the window must exist and must have
#     been previously obtained using tkiWinCreate() or some variant
#     of tkiWinShow().
#   - otherwise it is a "window tag", which is arbitrary text that
#     must not begin with a ``.''.  Each tag has a unique window associated
#     with it that will be created (and re-created) upon demand.
#     The tag will also appear in the window title&icon.
#
proc _tkiWinResolveWinName { w } {
    global tki
    if { ! [info exist tki] } { tkiInit }
    if { "$w"=="" }			{ return [tkiWinCreate] }
    if { "[string index $w 0]"=="." }	{ return $w }

    # It must be a tag: retrieve (or make) the window assoicated with the tag
    set tag $w
    if { ![info exist tki(wintag-$tag)] } { 
puts stdout "creating tag $tag"
	set tki(wintag-$tag) [tkiWinCreate "" $tag]
    }
    set w $tki(wintag-$tag)

    # Now see if it still exists: the user might have killed it.  If
    # gone, recreate it.
    if ![winfo exist $w] {
	tkiWinCreate $w $tag
    }
    return $w
}

# 
# This is the primary entry point of this module.  The argument {nodeSpec}
# give the node to show, and may contains a filespec as in (filename)nodename.
# If no filename is contained in {nodeSpec}, it will be augmented by
# the argument {fileSpec} (if non-empty).  The argument
# {w} specifies which info window the node should be displayed in,
# as described by _tkiWinResolveWinName() above.
#
# The return value is a list "nodeRef window" where {nodeRef} is
# the internal "handle" to the node given by {nodeSpec} and {fileSpec},
# and {window} is the full path of the info window.
# If the node couldn't be loaded, the {nodeRef} will be empty.
#
proc tkiWinShow { nodeSpec {fileSpec ""} {w ""} } {
    global tki
    global C2_helpinfo1 C2_helpinfo2 C2_helpinfo3

    set w [_tkiWinResolveWinName $w]
    set tki(curWindow) $w
#puts stdout "tkiWinShow: spec $nodeSpec"
    set nodeRef [tkiGetNodeRef $nodeSpec $fileSpec]
    if { "$nodeRef"=="" } {
	set fmtSpec [tkiFmtNodeSpec $nodeSpec $fileSpec]
	tkiError "Can't locate info node ``$fmtSpec''"
    } else {
#puts stdout "tkiWinShow: ref $nodeRef"
	set nodeIdx [lindex $nodeRef 0]
	set fileKey [lindex $nodeRef 1]
	tkiWinDpy $w $fileKey [lindex $tki(nodesinfo-$fileKey) $nodeIdx] \
	  [lindex $tki(nodesbody-$fileKey) $nodeIdx]
    }
    set tki(curWindow) ""
    return [list $nodeRef $w]
}

#
# Get the current info node for {w}, and redisplay it in the window.
# This is used whenever the display mode (nodelook,etc) is changed.
#
proc tkiWinRefresh { w } {
    global tki; upvar #0 $w wvars
    if { $tki(showButtonsB) }	{ pack conf $w.buts -fill x } \
    else			{ pack forget $w.buts }
    if ![info exist wvars(nodeinfo)] return
    set nodeinfo $wvars(nodeinfo)
    return [tkiWinShow [lindex $nodeinfo 1] $wvars(fileKey) $w]
}

proc tkiWinRefreshAll { } {
    global tki

    foreach w $tki(windows) {
	if { ![winfo exist $w] } continue
	if [catch {tkiWinRefresh $w} error] {
	    global errorInfo
	    puts stderr "tkInfo: refresh $w: $error\n$errorInfo"
	}
    }
}

#
# A helper function to provide "context" help.  The idea is that the
# application, when it creates each window/widget, creates a global array
# variable corresponding to each "key" window in the application.  The
# array element "infonodename" contains the node name to display for
# context help for that window and its children.
#
# Start at window {w}, and traverse up the window tree looking for a variable
# of the form "$w(infonodename)".  If found, a window displaying that node
# will be generated.  {fileSpec} may be used to augment the infonode,
# and {infowin} may specific a pre-existing info window returned by
# tkiWinShow().
# 
proc tkiWinContextHelp { w {fileSpec ""} {infowin ""} } {
    for {} {"$w"!=""} {set w [winfo parent $w]} {
	# Line below is kludgy, b/c I can't see any other way to do it.
	if [uplevel #0 [list info exist ${w}(infonodename)]] {
	    upvar #0 $w wvars 
    	    return [tkiWinShow $wvars(infonodename) $fileSpec $infowin]
	}
    }
    if { "$fileSpec"!="" } {
    	return [tkiWinShow Top $fileSpec $infowin]
    }
    return [tkiWinShow (builtin)QuickHelp "" $infowin]
}








