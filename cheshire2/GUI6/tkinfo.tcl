#!/usr/local/bin/wish -f

# VERSION
set tki_version 0.7-beta
set tki_header {

Version:	tkInfo version $tki_version
Author:		Kennard White <kennard@ohm.eecs.berkeley.edu>
Copyright:	see below
RCS:		$Header: /users/kennard/src/tkinfo/RCS/tkinfo.tcl,v 1.19 1993/12/23 01:57:38 kennard Exp $

Tk script to read GNU "info" files and display them.  tkInfo can be
used stand alone (via WISH), embedded within an application to provide
integrated, on-line help, or used as a server tool for on-line application
help.

Please see the "About" and "Info" file sections below.  (search for
"README" to find these sections quickly).  These explain much more
about what tkInfo is and what info files are, and gives references to
other programs and sources of info.  For information on the internals
of tkInfo, see "tkicore.tcl".  For info on embedding tkInfo or using
it as a server tool, see "tkiwin.tcl".

The program provides on-line help about itself: node (builtin)QuickHelp,
and the texinfo distribution contains a tutorial: node (info)Top.

tkInfo consists of the files:

	tkinfo.tcl	(the primary file, and the one you are reading now)
	tkicore.tcl	(info file reading, parsing and storing)
	tkiwin.tcl	(windows to display info nodes)
	topgetopt.tcl	(argument parsing)
	searchbox.tcl	(text searching)
	tkinfo		(shell script for invoking tkinfo.tcl with wish)

All of the above files should be in the same directory.  See the
"tkinfo" shell script for installation ideas.  This release should work
with tcl7.0/tk3.3 or later.  tkInfo no longer works with tcl6.7/tk3.2
(sorry).  tkInfo has gone through several releases, but it is by no
means complete.  Feel free to make suggestions, or better yet, send me
patch files.

See below for copyright (search for "README").  Basically you can
re-distribute this any way you like, just don't sue me and don't pretend
you wrote tkInfo.

Contributions and/or good ideas (some minor, some major) by Larry
Virden <lvirden@cas.org>, Bob Bagwill <bagwill@swe.ncsl.nist.gov>,
??? <tlukka@snakemail.hut.fi>, Kurt Hornik <hornik@neuro.tuwien.ac.at>,
Hume Smith <850347s@dragon.acadiau.ca>, Stephen Gildea <gildea@x.org>,
Warren Jones <wjones@tc.fluke.COM>, Robert Wilensky <wilensky@CS.Berkeley.EDU>.
Tom Phelps <phelps@CS.Berkeley.EDU> contributed the searching code, as
well as many other good ideas.

} ; set tki_help_usage {

		Stand-alone usage, via WISH

When invoked with no arguments, tkInfo looks for an "info tree"
(a collection of info files installed on your system) and displays
the top level node.  On a well maintained system, you can get
to every info file starting from this top level node.  Alternatively,
you can specify the file you want to see on the command line as:
"tkinfo -infofile <filename>".  The complete syntax is below:

usage: tkinfo [-headers] [-nodelook looktype] [-dir dir1] [-dir dir2] ... \
		  [-infofile filePath] [node]
Options:
   -headers	Turns on display of the raw info node headers.
   -nodelook	Specifies how to highlight xrefs and menu entries.  Must
		be one of "color", "font", or "underline".
   -dir		Specifies directories to search for info files, like the
		INFOPATH variable.
   -infofile	Specifies an info file to initially display.
   node		Specifies the node you want to see.  The complete syntax
		is below, but it will generally be of the form
		"(filename)nodename"; remember to use quotes if nodename
		has spaces in it.
Note that the "-file" argument is taken by WISH, and thus can't be used
to specify the info file name.
Environment variables:
   INFOPATH	A colon (``:'') seperated list of directories to search
		for info files.  Equivalent to each -dir option above.
   INFOSUFFIX	A colon seperated list of file suffixes to try when searching
		for an info file.  tkinfo will automatically try the
		suffixes .Z, .z, and .gz and deal with the compressed files
		if required.

The default INFOPATH and INFOSUFFIX is defined in the procedure tkiInit(),
please edit as appropriate for your site.

} ; set tki_header {

This file is organized as:
  1)  This comment area.
  2)  Configuration, initialization, and error handling.
  3)  The built-in nodes.  This includes info on texinfo and how
      to use tkInfo.
  4)  The public interface and boot-strapping code.

} ; unset tki_header

# Ignore this, it is used by a custom auto-reload script.
proc tkinfo.tcl { } { }

#
# README: You might want to customize "defInfoPath" below for your site,
#	  just put your paths at the head of the list (after ".").
#	  If you feel there is a "standard" location not listed below,
#	  please send me mail.
#
proc tkiInit { } {
    global tki env auto_path tkiEmbed defaultPath

    set defInfoPath [list . \
      /usr/tools/gnu/info /usr/sww/info /usr/sww/lib/emacs/info \
      /usr/local/info /usr/local/gnu/info /usr/gnu/info \
      /usr/local/emacs/info /usr/local/lib/emacs/info \
      /usr/local/gnumacs/info /usr/local/lib/gnumacs/info \
      /usr3/cheshire2/new/doc $defaultPath/doc \
      /trec/cheshire/cheshire2/GUI4/doc]
    set defInfoSuffix [list "" .info -info]

    if { [info exist tki(sn)] } return

    set tki(sn)			0
    set tki(self)		[info script]
    set tki(detailstatusB)	0
    set tki(timestatusB)	0
    _tkiSourceFile topgetopt.tcl
    _tkiSourceFile tkicore.tcl
    _tkiSourceFile tkiwin.tcl
    _tkiSourceFile searchbox.tcl searchboxRegexpSearch

    set tki(compresscat-Z)	"zcat"
    set tki(compresscat-z)	"gunzip -c"
    set tki(compresscat-gz)	"gunzip -c"
    set tki(tkman)		tkman
    set tki(background)		"#ffe4c4"
    set tki(highlightbackground) "#ffe4c4"
    set tki(highlightcolor)     "#ffe4c4"
    set tki(rawHeadersB)	0
    set tki(showButtonsB)	1
    set tki(nodeSep)		"\037"
    set tki(nodeByteSep)	"\177"
    set tki(topLevelFile)	"dir"
    set tki(topLevelNode)	"Top"

    set tki(nodelook)		"color"
    if { "[info commands winfo]"!="" } {
	if { [winfo depth .] == 1 } {
	    set tki(nodelook) underline
	}
    }
    set tki(nodelookColor)	"red4"
    set tki(nodelookFont)	"-*-Courier-Bold-R-Normal-*-180-*"
    set tki(normCursor)		"left_ptr"
    set tki(waitCursor)		"watch"
    set tki(windows)		""
    set tki(curWindow)		""
    if [info exist env(INFOPATH)] {
      tkiAddInfoPaths [split $env(INFOPATH) ":"]
    } else {
      tkiAddInfoPaths $defInfoPath
    }
    if [info exist env(INFOSUFFIX)] {
	set tki(infoSuffix) [split $env(INFOSUFFIX) ":"]
    } else {
	set tki(infoSuffix) $defInfoSuffix
    }

    _tkiNodeParseInit
    rename _tkiNodeParseInit ""

    _tkiBuiltinFile
    rename _tkiBuiltinFile ""

    trace var tki(rawHeadersB) w "_tkiTraceOptionsCB"
    trace var tki(showButtonsB) w "_tkiTraceOptionsCB"
    trace var tki(nodelook) w "_tkiTraceOptionsCB"
}

proc _tkiTraceOptionsCB { n1 n2 op } {
    tkiWinRefreshAll
}

# We need to be able to find varous files.  Assume its in
# the same directory as us.  Could just do:
#	lappend auto_path [file dirname $tki(self)]
# but this requires the tclIndex to be up-to-date, which is to
# much to ask of some users.  So just source it here.
#
proc _tkiSourceFile { filename {tagproc ""} } {
    global tki auto_path
    if { "$tagproc" == "" } { set tagproc $filename }
    if { "[info procs $tagproc]"=="" } {
	set selfdir [file dirname $tki(self)]
	set otherfile $selfdir/$filename
	if { [file isfile $otherfile] } {
	    if { [info exist tkiEmbed] } {
		if { [lsearch $auto_path $selfdir]==-1 } {
		    lappend auto_path $selfdir
		}
	    } else {
		uplevel #0 source $otherfile
	    }
	}
    }
}

proc tkiUninit { } {
    global tki
    catch {rename topgetopt.tcl ""}
    catch {rename tkicore.tcl ""}
    catch {rename searchboxRegexpSearch ""}
    # Must destroy all existing windows so that there is no trace
    # on anything in $tki.  Note that the "Options" menu does direct
    # traces on stuff in tki.
    catch {eval destroy $tki(windows)}
    catch {unset tki}
}

proc tkiReset { } {
    tkiUninit
    tkiInit
}

proc _tkiNull { args } {
}

# These are called from searchbox.tcl
proc winstdout {w msg} { tkiStatus $msg; after 1000 }
proc winerrout {w msg} { tkiError $msg }

proc tkiStatus { msg } {
    global tki
    if { "$tki(curWindow)"=="" } {
        puts stdout "tkInfo: $msg"
    } else {
	$tki(curWindow).s.status conf -text "$msg"
	# idletasks should be sufficient, but the geometry management
	# apparently needs some X-events to make the redisplay occur
	update
    }
}

proc tkiDetailStatus { msg } {
    global tki
    if { $tki(detailstatusB) } {
	tkiStatus $msg
    }
}

proc tkiWarning { msg } {
    # Warnings allways go to stderr
    puts stderr "tkInfo Warning: $msg"
}

proc tkiFileWarning { fileSpec msg } {
    global tki
    if [info exist tki(fileinfo-$fileSpec)] {
	set fileSpec [lindex $tki(fileinfo-$fileSpec) 2]
    }
    tkiWarning "$fileSpec: $msg"
}

proc tkiError { msg } {
    global tki
    if [info exist tki(error-active)] {
	puts stderr "tkInfo Warning: Re-entrant error message!"
    }
    set tki(error-active) 1
    if { "$tki(curWindow)"=="" } {
        puts stdout "tkInfo Error: $msg"
    } else {
	set infowin $tki(curWindow)
	upvar #0 $infowin wvars
	set w .tkierr[tkiGetSN]
	if ![winfo ismapped $infowin] {
#puts stdout "tkiError: vis $infowin"
	    tkwait vis $infowin
	}
#puts stdout "tkiError: $w"
	toplevel $w
	wm geom $w +[winfo rootx $infowin]+[winfo rooty $infowin]
	wm title $w "tkInfo Error"
	wm iconname $w "tkInfo Error"
	pack append $w [label $w.title -text "tkInfo Error"] \
	  {top fillx}
	pack append $w [message $w.msg -text $msg \
	  -width [winfo width $infowin]] {top fill expand}
	pack append $w [button $w.dismiss -text "Dismiss" \
	  -com "tkiErrorDone $w $infowin"] {top fillx}
        $infowin.s.status conf -text "Error!"
	grab $w
#	$tki(curWindow).s.status conf -text "Error: $msg"
	tkwait window $w
        $infowin.s.status conf -text $wvars(status)
    }
    unset tki(error-active)
}

proc tkiErrorDone { errwin infowin } {
    catch {destroy $errwin}
}

#
# This is complicated by the fact that "time" doesnt provide access to
# the return value.  Thus "cnt" is used as follows:
#	0 ==> Do once for timing, and repeat for return value (no side-affects)
#	1 ==> Do once for timing&side-affects, empty return value
#
proc tkiTimeStatus { msg cnt args } {
    global tki
    if { $tki(detailstatusB) } {
	tkiStatus $msg
    }
    if { $tki(timestatusB) } {
	puts stdout "tkInfo time: $msg [lindex [time $args] 0] microseconds"
	if { $cnt == 0 } {
	    return [eval $args]
	} else {
	    return ""
	}
    } else {
	return [eval $args]
    }
}

#
# This proc is called once during initialization, and then destroyed.
# (It is destroyed to save memory).
# Currently we fake all the appropriate table entires to create a "builtin"
# file.  It might be easier, however, to just pass one large text string
# into the parser and have it be dealt with like any other file.
#
proc _tkiBuiltinFile { } {
    global tki tki_help_usage tki_version

    set fileKey			builtin
    set tki(file-$fileKey)	$fileKey
    set tki(fileinfo-$fileKey)	[list $fileKey $fileKey $fileKey "" 0]
    set tki(incore-$fileKey)	1
    set tki(nodesinfo-$fileKey) ""
    set tki(nodesbody-$fileKey) ""

    set tki(dirnodes) ""
    set diritems "\nDirectory nodes (from INFOPATH):\n"
    foreach pp $tki(infoPath) {
	set dirpath [tkiFileFind $pp/$tki(topLevelFile)]
	if { "$dirpath" != "" } {
	    lappend tki(dirnodes) $dirpath
	    append diritems "* ($dirpath)::\n"
	}
    }
    if { [llength $tki(dirnodes)] == 0 } {
	append diritems "(No directories found!)\n"
    }

    tkiFileParseNode $fileKey "
File: builtin, Node: Top, Up: (dir)Top
tkInfo v$tki_version
-----------
This is the builtin info on tkInfo itself.  If you need help on using
tkInfo, try selecting \"QuickHelp\" or \"PlainInfo\" below.  Select an item
by moving the mouse over it and pressing the left mouse button.

* Menu:
* Usage::			tkInfo's command line usage.
* Copyright::			tkInfo's copyright.
* About::			About tkInfo.
* Info::			Info files.
* QuickHelp::			Quick summary of how to use tkInfo.
* PlainInfo::			Tutorial on using the plain info.
* ToDo::			My to-do list.
* Source Code: ($tki(self))*	Source code to tkInfo.
$diritems"

    tkiFileParseNode $fileKey "
File: builtin, Node: Usage, Up: Top, Next: Copyright
tkInfo Usage
------------
$tki_help_usage"

#README
    tkiFileParseNode $fileKey {
File: builtin, Node: Copyright, Up: Top, Prev: Usage, Next: About
Copyright for tkInfo
--------------------
This copyright applies to the tkInfo system only.  If tkInfo is
embedded within a larger system, that system will most likely have
a different copyright.

Sorry this is so long.  Basically, do whatever you want with this
software, just don't sue me and don't pretend you wrote it -- kennard.

Copyright (c) 1993 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
    }
#VERSION README
    tkiFileParseNode $fileKey "
File: builtin, Node: About, Up: Top, Prev: Copyright, Next: Info
About tkInfo
------------
tkInfo version $tki_version (based on tcl-7.2 and tk-3.5) by Kennard White

tkInfo displays ``info'' files.  Info files provide a hyper-text
capability that is ideal for on-line help.  The format is suitable for
both tty-based systems and graphical systems.  In addition, the same
document source can produce both a nice hardcopy manual and Info
files.  For more information, *note (builtin)Info::.

tkInfo is the work of Kennard White.  For information, comments,
code-contributions or bug reports please contact me at:
	phone:	510/643-6686
	e-mail: kennard@ohm.eecs.Berkeley.EDU
    	s-mail:	Kennard White, Box #115
		207 Cory Hall
		UC Berkeley
		Berkeley, CA 94720, USA

You can obtain the tkInfo distribution by anonymous ftp from:
	ftp@ptolemy.eecs.berkeley.edu:/pub/misc
	ftp@harbor.ecn.purdue.edu:/pub/tcl/code
The first site will always have the most recent version.

RCS: \$Revision: 1.19 $, \$Date: 1993/12/23 01:57:38 $
    "

#README
    tkiFileParseNode $fileKey {
File: builtin, Node: Info, Up: Top, Prev: About, Next: QuickHelp
Info Files
----------

tkInfo displays "info" files, a file format that supports a robust
hypercard system which is ideal for on-line help.  Each file contains
many nodes organized into a tree, and typically all the files on your
system are organized into a (flat) tree.  The top-level info file is
called "dir" or "dir.info", and lists all the other files on your
system (typically GNU software, since most GNU software is documented
using texinfo).

Info files can be created manually with any text editor (vi,emacs),
with the support of the emacs "info" package, with the GNU "makeinfo"
program, or with the emacs scripts for latexinfo.  The makeinfo program
produces a set of info files from TeX source that use the GNU texinfo
format (the one that uses "@" everywhere).  Similarly, the latexinfo
package (like texinfo, but with latex commands and syntax) provides
emacs scripts for producing info files.

Portions of the GNU "makeinfo" is GNU copylefted, but only the author
of the document needs "makeinfo": the author can freely redistribute
the info files produced by "makeinfo" (the ones read by this script).
I think latexinfo is similar.

There are several info file browsers or viewers:
  tkinfo	what you're using now, *note (builtin)About::.
  info		an ascii curses-based C program.
  emacs info mode: ??? (ask an emacs wizard, not me).
  xinfo		an X11 athena-based program, (old and unsupported).

In addition, there are a couple different Tk-based graphical "help" systems.
These solve different problems than tkinfo, but might be what you're
looking for:
  jstools/help	Uses troff-like formating macros with multiple fonts.
  HLP		Uses custom input format, supports hierarchy.

See the texinfo package for the info viewer, makeinfo program,
emacs info mode and general information about info files.
All these info packages may be obtained by anonymous ftp from:
texinfo:	prep.ai.mit.edu		pub/gnu/texinfo-3.1.tar.z
latexinfo:	csc-sun.math.utah.edu	pub/tex/pub/latexinfo
xinfo:		gatekeeper.dec.com	pub/X11/contrib/xinfo.tar.Z
jstools:	harbor.ecn.purdue.edu	pub/tcl/code/tk3.2v1.0.tar.Z
HLP:		harbor.ecn.purdue.edu	pub/tcl/code/HLP.tar.Z
    }

    tkiFileParseNode $fileKey {
File: builtin, Node: QuickHelp, Up: Top, Prev: Info, Next: PlainInfo
		tkInfo Commands

Use scroll bar on right side to scroll through the node text.  Cross references
and menu entires are shown highlighted (blue foreground).  Press the left mouse
button over highlighted text to see that node.  Most of the functions below
are accessible from the menu bar.

Accelerator keys:
?	Show this quick help message.  Equivalent to `g(builtin)QuickHelp'.
n       Move to the "next" node of this node.
p       Move to the "previous" node of this node.
u       Move "up" from this node.
t	"Toggle" to last node you were at, toggle based.
l       Move to the "last" node you were at, stack based.
d       Move to the "directory" node of the first tree.  Equivalent to `gDIR'.
D       Move to the builtin "directory" node.  Equivalent to `g(builtin)DIR'.
	If you have multiple info directories installed, use this key.
q	Quit info window.

Advanced commands:
1-9	Pick first, second, etc, item in node's menu.
g,(	Goto given node (Ctrl-g to abort,Ctrl-u to erase line).
	Syntax: NODENAME or (FILENAME)NODENAME.
s,/	Search for text literally or by regular expression.
Ctrl-s	Continue previous search.
m,f	menu, follow cross-reference (not implemented).
M	Show manual page using tkman.  Uses selection or prompts.
p	Print (not implemented).
!	Issue tcl command, results printed on stdout.

Scrolling commands:
b			Scroll to the beginning of the node.
e			Scroll to the end of the node.
SPACE, Ctrl-F,
Ctrl-V, PgDn		Scroll forwards one page.
BACKSPACE, DELETE,
Ctrl-B, Alt-V, PgUp	Scroll backwards one page.

Go "up" from this node to obtain more information on tkInfo.
    }


    tkiFileParseNode $fileKey {
File: builtin, Node: PlainInfo, Up: Top, Prev: QuickHelp, Next: ToDo
Plain Info
----------
There is a GNU program called "info" that is similar to tkInfo, but
is ascii tty based.  There is a tutorial info file written for this 
program that is available on most systems.  This tutorial might
be useful for some people learning how to use tkInfo.  However,
some of the commands and functionality is different, so don't assume
everything it says applies to tkInfo.  To see this tutorial,
select the menu entry below.

* Menu:
* Plain Info Tutorial: (info)Help.
    }

    tkiFileParseNode $fileKey {
File: builtin, Node: ToDo, Up: Top, Prev: PlainInfo
tkInfo is still incomplete.  The following is my todo list.

-  Better text widget bindings.  These are getting better with each
   release, but more are needed.  If you have suggestions, let me know.
-  Better menu bars, and more user-configurability (colors, fonts, etc).
-  Add option to allow all the "*note:" to not be drawn on the screen,
   or change them to "see also".
-  Implement all the currently unimplemented bindings.
-  Implement a tcl-only, tty-based interface?
-  Implement stat'ing of the source files with auto-reload.
-  Profile the whole mess: speed up file loading and node formating.
-  Figure out some huiristic for un-loading files to save memory.
-  Add in links to TkMan.
-  Extend searching to whole-file search, and make literal case-insen
   searching work right.  The coloring of search'd and hyper'd
   text when combined doesnt work right.
    }
}

#
# Do stand-alone help window
# The -node option is for compatibility to the info program only.
#
proc tkiInfoWindow { args } {
    global tki

    if { ! [info exist tki] } { tkiInit }

    set w ""
    set nodeSpec ""
    set fileSpec ""
    set fileSpec2 ""
    set dirList ""
    set nodelook ""
    set headersB -1
    set buttonsB -1
    set opt_list { 
      { "window" w } 
      { "dir" dirList append } 
      { "file" fileSpec }
      { "headers" headersB bool }
      { "buttons" buttonsB bool }
      { "nodelook" nodelook }
      { "infofile" fileSpec2 }
      { "node" nodeSpec append }
    }

    set args [topgetopt $opt_list $args]

    if { "$dirList"!="" } {
	set infoPaths ""
	foreach dir $dirList {
	    eval lappend infoPaths [split $dir ":"]
	}
	tkiAddInfoPaths $infoPaths
    }
    if { "$nodelook" != "" }	{ set tki(nodelook) $nodelook }
    if { $headersB != -1 }	{ set tki(rawHeadersB) $headersB }
    if { $buttonsB != -1 }	{ set tki(showButtonsB) $buttonsB }
    if { "$fileSpec"=="" }	{ set fileSpec $fileSpec2 }
    if { "$fileSpec"!="" }	{ tkiAddInfoPaths [file dirname $fileSpec] }

    if { "$args"!="" } {
	eval lappend nodeSpec $args
    }
    if { [llength $nodeSpec] > 1 } {
	error "tkiInfoWindow: Only one node may be specified"
    }
    tkiWinShow [lindex $nodeSpec 0] $fileSpec $w
    return ""
}

#
# We are operating in one of two modes:
#   1)  Stand-alone.  Popup an initial window, filling it according to argv.
#	Kill the stupid "." window.
#   2)	Embedded within a larger application.  Don't do anything automatically;
#	instead, let that application's startup script handle things.
#
# We are operating in embedded mode iff the global tkiEmbed exists.
#
proc tkiBoot { } {
    global argv tkiEmbed

    if { [info exist tkiEmbed] } return
    wm withdraw .
    if { "[lindex $argv 0]"!="" && [file isfile [lindex $argv 0]] } {
	# Some wishs pass the filename as argv[0].  Kill it off.
	set argv [lreplace $argv 0 0]
    }
    eval tkiInfoWindow $argv
}

tkiReset
tkiBoot
