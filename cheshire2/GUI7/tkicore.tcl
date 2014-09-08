#
# tkicore.tcl :: TkInfo package
#

set tki_header {

Author:		Kennard White <kennard@ohm.eecs.berkeley.edu>
Version:	See the file "tkinfo.tcl"
Copyright:	See the file "tkinfo.tcl"
RCS: $Header: /users/kennard/src/tkinfo/RCS/tkicore.tcl,v 1.2 1993/12/14 23:49:46 kennard Exp $

This is the core of the tkinfo package.  It handles reading, parsing,
and storing info files.  Everything in here should be tcl-only, no
tk stuff.  Note that this can't be used independently: it requires
initialization and error handling stuff found in tkinfo.tcl.

tkInfo requires the following global variables:
 tki		This is a huge array where essentially all the loaded info-files
		are stored.  It also contains some configuration state.
		The contents of this is described below.
 .tki##		Each toplevel info window has a global variable associated
		with it.  The name of the variable is the same as the
		toplevel window name, which is ".tki" followed by some number.
 tkiEmbed	tkInfo can operate stand-alone (like the "info" program) or
		embedded (part of your application).  Embedded mode is
		used iff this variable exists.  When this file is
		sourced in the stand-alone case, the argv options will be
		parsed (see tkiBoot() below) and a new toplevel window
	 	will be opened.

The core structure of an info file is a {node}.  Each info file consists
of a set of nodes seperated by a magic character.  Each nodes consists of
of a header, body, and menu.  There are also special nodes that contain
control information used to reference "split" files and speed up access.
The first half of the code below parses this file format.
A node may be specified in one of several ways (called a {nodeSpec}):
	(filename)nodename
	nodename		The given node within the current file.
	(filename)		The "Top" node of the file.
	(filename)*		The entire file.


In the implementation below, the info format consists of {nodes} stored 
in files.  A given info file has three identifiers associated with it:
 -	The {filename}, which is the name used either by the user to
	reference a file, or by one info file to reference another.
	Such a reference could be complete UNIX path name (either
	absolute or relative), or may be a partial specification (see below).
 -	The {filepath}, which is a valid UNIX path name to access the
	file.  The filepath is derived from the filename.  If the filename
	is already a valid path, no work needs be done.  Otherwise,
	the filepath is formed by prepending a path prefix and appending
	a file suffix.  These are defined by the INFOPATH and INFOSUFFIX
	variables.
 -	The {filekey}, which is an internal, auto-generated token associated
	with each file.


The global array "tki" contains the following elements:
  file-$fileName	The fileKey for $fileName.
  fileinfo-$fileKey	The fileinfo struct for $fileKey. Each fileinfo is
			{ fileKey fileName filePath pntKey wholeB }
  incore-$fileKey	Boolean 0/1; true if file has been loaded into core.
  nodesinfo-$fileKey	A list of nodeinfo for every node in $fileKey.
			Each nodeinfo is a list { idx node file up prev next }.
  nodesbody-$fileKey	A list of the textual body for every node in $fileKey.
  indirf-$fileKey	List of indirect-file-info for $fileKey.  Each
			info is a list { indirFileKey byteOfs }.
  indirn-$fileKey	List of indirect-node-info for $fileKey.  Each
			info is a list { nodeName byteOfs fileKey }.
  xrefinfo-$fileKey-$nodeIdx
			Contains information on all cross reference
			pointers within the node's body text.
  menuinfo-$fileKey-$nodeIdx
			Contains information on all menu entries
			within the node's menu text.  Constist of list of:
			 { linecnt menucnt toNode nBeg nEnd }

Notes (some important, some not).
1.	Because of the graphical system, there may be several parallel
	info windows active.  These windows must operate independently.
	Because of this, there can be no concept of the "current file"
	or "current node" within the tkinfo core.  Rather, this information
	must be maintained by the window.
2.	Because of #1, we must maintain multiple files in core.  Currently
	we never flush.
3.	The background color used in tkiInit() is BISQUE1, from tk/defaults.h
4.	The byte offsets in the indirect tables are not used as such;
	this is because we parse the file when loaded.  However, they are
	used to identify which indirect file the node is in.
5.	The function tkiLoadFile() attempts to deal with compressed files.
	Currently it uses "zcat" for .Z files and "gunzip -c" for .z and .gz
	files.
	If you have better suggestions, please let me know.


} ; unset tki_header

# Ignore this, it is used by a custom auto-reload script.
proc tkicore.tcl {} {}

# Get a unique serial number.
#
proc tkiGetSN { } {
    global tki
    incr tki(sn)
    return $tki(sn)
}

#
# Add list tcl list of paths {newPaths} to the directory search list.
# The list is added in order at the *head* of the list.
# Duplicate paths are removed, leaving the first most path present.
#
proc tkiAddInfoPaths { newPaths } {
    global tki

    if { ! [info exist tki(infoPath) ] } {
	set tki(infoPath) ""
    }
    if { [llength $newPaths] > 0 } {
	set tki(infoPath) [eval linsert {$tki(infoPath)} 0 $newPaths]
    }
    # Kill off null paths
    while 1 {
	set idx [lsearch $tki(infoPath) ""]
	if { $idx < 0 }	break
	set tki(infoPath) [lreplace $tki(infoPath) $idx $idx]
    }
    # Kill off duplicate paths
    for {set idx 0} {$idx < [llength $tki(infoPath)]} {incr idx} {
	set path [lindex $tki(infoPath) $idx]
	while 1 {
	    set dup [lsearch [lrange $tki(infoPath) [expr {$idx+1} ] end] $path]
	    if { $dup < 0 } break
	    set tki(infoPath) [lreplace $tki(infoPath) $dup $dup]
	}
    }
}

proc _tkiFileFindSuf { fileName } {
    global tki

    foreach suf $tki(infoSuffix) {
	foreach extrasuf {"" .Z .z .gz} {
	    set filePath "$fileName$suf$extrasuf"
	    if { [file isfile $filePath] } {
		return $filePath
	    }
	}
    }
    return ""
}

#
# Given {fileName} (see intro section above), find the cooresponding
# filepath.  The filepath of {pntFileKey}, if specified, is
# used as a starting point for locating {fileName}.
# Returns the file path if found, else empty string.
#
proc tkiFileFind { fileName {pntFileKey ""} } {
    global tki

    catch {unset tki(temp-search-path)}
    case [string index $fileName 0] {
      "/ . ~" {
	lappend tki(temp-search-path) [file dirname $fileName]
	# Should be valid UN*X path modulo suffix
	set filePath [_tkiFileFindSuf $fileName]
	if { "$filePath"!="" } { return $filePath }
	set filePath [_tkiFileFindSuf [string tolower $fileName]]
	if { "$filePath"!="" } { return $filePath }
      }
      default {
	# Try all the infopaths, and all suffixs
	set pp ""
	if { "$pntFileKey"!="" } {
	    set pp [file dirname [lindex $tki(fileinfo-$pntFileKey) 2]]
	}
	foreach prepath "$pp $tki(infoPath)" {
#puts stdout "Searching dir ``$prepath''"
	    lappend tki(temp-search-path) $prepath
	    if { ! [file isdir $prepath] } continue
	    set filePath [_tkiFileFindSuf $prepath/$fileName]
	    if { "$filePath"!="" } { return $filePath }
	    set filePath [_tkiFileFindSuf $prepath/[string tolower $fileName]]
	    if { "$filePath"!="" } { return $filePath }
	}
      }
    }
    return ""
}

#
# Given {fileName}, find the coorsponding filepath via tkiFindFile().
# Create a {fileKey} for the file, and make the appropriate table entries.
# Note that {fileName} must be just that, and not a filekey.
#
proc tkiFileAdd { fileName {pntFileKey ""} {wholeB 0} } {
    global tki

    if { [info exist tki(file-$fileName)] } {
	return $tki(file-$fileName)
    }
    set filePath [tkiFileFind $fileName $pntFileKey]
    if { "$filePath"=="" } { return "" }
    set fileKey fk[tkiGetSN]
    set tki(file-$fileName) $fileKey
    set tki(fileinfo-$fileKey)  [list $fileKey $fileName $filePath $pntFileKey $wholeB]
    set tki(incore-$fileKey) 0
    return $fileKey
}

proc tkiFileGet { fileSpec {pntFileKey ""} {wholeB 0} } {
    global tki

    if { [info exist tki(fileinfo-$fileSpec)] } {
	set fileKey $fileSpec
    } else {
	if { [info exist tki(file-$fileSpec)] } {
	    set fileKey $tki(file-$fileSpec)
	} else {
	    set fileKey [tkiFileAdd $fileSpec $pntFileKey $wholeB]
	    if { "$fileKey"=="" } {
		set msg "Can't locate info file ``$fileSpec''."
		if [info exist tki(temp-search-path)] {
		    set search $tki(temp-search-path)
		    regsub -all " " $search ", " search
		    append msg "\nSearch path is \{$search\}."
		}
		tkiError $msg
		return ""
	    }
	}
    }
    set fileinfo $tki(fileinfo-$fileKey)
    if { ! $tki(incore-$fileKey) } {
        tkiFileLoad $fileKey [lindex $fileinfo 1] [lindex $fileinfo 2] [lindex $fileinfo 4]
    }
    return $fileKey
}

proc _tkiFileLoadIndirectTbl { fileKey lines } {
    global tki

    set indirinfos ""
    foreach line $lines {
	if { "$line"!="" } {
	    set pair [split $line ":"]
	    if { [llength $pair] != 2 } {
		tkiFileWarning $fileKey "has bad file-indirect line ``$line''"
		continue
	    }
	    set indirKey [tkiFileAdd [lindex $pair 0] $fileKey]
	    if { "$indirKey"=="" } {
		tkiError "Can't locate indirect file ``[lindex $pair 0]''."
		continue
	    }
	    set byteOfs [string trim [lindex $pair 1]]
	    lappend indirinfos [list $indirKey $byteOfs]
	}
    }
    set tki(indirf-$fileKey) $indirinfos
#puts stdout "IndirectTbl $fileKey: $indirinfos"
}

proc _tkiFileLookupIndir { indirf byte } {
    set lastKey ""
    foreach fi $indirf {
	if { [lindex $fi 1] > $byte } break
	set lastKey [lindex $fi 0]
    }
    return $lastKey
}

proc _tkiFileLoadTagTbl { fileKey lines } {
    global tki

    set subkey [lindex $lines 0]
    if { "$subkey"!="(Indirect)" } return
    set indirf $tki(indirf-$fileKey)
    set indirinfos ""
    foreach line [lrange $lines 1 end] {
	if { "$line"=="" } continue
	set pair [split $line $tki(nodeByteSep)]
	if { [llength $pair] != 2 } {
	    tkiFileWarning $fileKey "has bad tag-indirect line ``$line''"
	    continue
	}
	set nodeName [string tolower [string trim [string range [lindex $pair 0] 5 end]]]
	set byteOfs [string trim [lindex $pair 1]]
	set indirFile [_tkiFileLookupIndir $indirf $byteOfs]
	lappend indirinfos [list $nodeName $byteOfs $indirFile]
# puts stdout "$fileKey: tag [list $nodeName $byteOfs $indirFile]"
    }
    set tki(indirn-$fileKey) $indirinfos
}

proc tkiFileParseNode { fileKey node } {
    global tki

    set lines [split $node "\n"]
    set keyline [string trim [lindex $lines 1]]
    case $keyline {
      { {[Ii]ndirect:} } {
	_tkiFileLoadIndirectTbl $fileKey [lrange $lines 2 end]
	return "IndirectTable"
      }
      { {[Tt]ag [Tt]able:} } {
	_tkiFileLoadTagTbl $fileKey [lrange $lines 2 end]
	return "TagTable"
      }
      { {[Ee]nd [Tt]ag [Tt]able} } {
	return "EndTagTable"
      }
    }
    # Some screwed up files omit the ``,'' for the file key.
    regsub "(File:\[^,\]*)Node:" $keyline "\\1,Node:" keyline
    set nodekey ""; set filekey ""
    set nextkey ""; set prevkey ""; set upkey ""
    foreach key [split $keyline ",\t"] {
	set key [string trim $key]
	case $key {
	  "File:*" { set filekey [string trim [string range $key 5 end]] }
	  "Node:*" { set nodekey [string trim [string range $key 5 end]] }
	  "Up:*"   { set upkey   [string trim [string range $key 3 end]] }
	  "Prev:*" { set prevkey [string trim [string range $key 5 end]] }
	  "Next:*" { set nextkey [string trim [string range $key 5 end]] }
	}
    }
    if { "$nodekey" == "" } { return "" }
    lappend tki(nodesinfo-$fileKey) [list [llength $tki(nodesinfo-$fileKey)] $nodekey $filekey $upkey $prevkey $nextkey]
    lappend tki(nodesbody-$fileKey) $node
    return $nodekey
}

proc tkiFileLoad { fileKey fileName filePath wholeB } {
    global tki

    case $filePath in {
      *.Z	{ set fp "|$tki(compresscat-Z) $filePath" }
      *.z	{ set fp "|$tki(compresscat-z) $filePath" }
      *.gz	{ set fp "|$tki(compresscat-gz) $filePath" }
      default	{ set fp $filePath }
    }
    if [catch {open $fp "r"} fid] {
	tkiError "Can't open ``$fp''."
	return ""
    }
    if { $wholeB } {
	set node_Top [list 0 Top $fileName "" "" ""]
    	set tki(nodesinfo-$fileKey) [list $node_Top]
    	set tki(nodesbody-$fileKey) [list [read $fid]]
        close $fid
        set tki(incore-$fileKey) 1
	return $fileKey
    }
    set nodelist [split [read $fid] $tki(nodeSep)]
    close $fid
    tkiStatus "Loading $fileName"
    set nodecnt 0
    set tki(nodesinfo-$fileKey) ""
    set tki(nodesbody-$fileKey) ""
    foreach node $nodelist {
	incr nodecnt
	if { $nodecnt==1 || [string length $node] < 10 } continue
	set nodeName [tkiFileParseNode $fileKey $node]
	if { "$nodeName" == "" } {
	    puts stdout "Warning: node #$nodecnt of file $filePath is bogus"
	    continue
	}
    }
    set tki(incore-$fileKey) 1
    return $fileKey
}

#
# Parse nodeSpec and fileSpec.  {nodeSpecVar} and {fileSpecVar} must
# refer to variables within the caller's context.  They will be substituted
# and replaced with canonical forms.
#
proc tkiParseNodeSpec { nodeSpecVar fileSpecVar } {
    global tki
    upvar $nodeSpecVar nodeSpec $fileSpecVar fileSpec

    if { "[string index $nodeSpec 0]"=="(" } {
	set ridx [string first ")" $nodeSpec]
	if { $ridx <= 0 } {
	    tkiError "Malformed nodespec ``$nodeSpec''."
	    return 0
	}
	set fileSpec [string range $nodeSpec 1 [expr $ridx-1]]
	set nodeSpec [string trim [string range $nodeSpec [expr $ridx+1] end]]
    }
    if { "$fileSpec"=="" } {
	set fileSpec $tki(topLevelFile)
    }
    if { "$nodeSpec"=="" } {
	set nodeSpec $tki(topLevelNode)
    }
    set nodeSpec [string trim $nodeSpec]
    return 1
}

proc tkiFmtFileSpec { fileSpec } {
    global tki
    if [info exist tki(fileinfo-$fileSpec)] {
	return [lindex $tki(fileinfo-$fileSpec) 1]
    }
    return $fileSpec
}

proc tkiFmtNodeSpec { nodeSpec {fileSpec ""} } {
    global tki
    if ![tkiParseNodeSpec nodeSpec fileSpec] {
	return "Bad file/node spec ``$nodeSpec''"
    }
    set fileSpec [tkiFmtFileSpec $fileSpec]
    return "($fileSpec)$nodeSpec"
}

#
# This is the core search function.  It attempts to locate {nodeSpec}
# where ever it is.  {fileSpec} is a default file name that is used
# only if {nodeSpec} doesn't contain a reference.
# Returns a list {nodeIdx fileKey}, where {nodeIdx} is the index of the
# node within {fileKey}.
#
# As discussed in the intro above, at this level we cannot allow any concept of
# "current file" or "current node": it is up to the caller to maintain
# that information and pass up the appropriate arguments.
#
proc tkiGetNodeRef { nodeSpec {fileSpec ""} {pntFileKey ""} } {
    global tki

    if ![tkiParseNodeSpec nodeSpec fileSpec] {
	return ""
    }
    set wholeB 0
    if { "$nodeSpec"=="*" } {
	set wholeB 1
	set nodeSpec Top
    }
    set fileKey [tkiFileGet $fileSpec $pntFileKey $wholeB]
    if { "$fileKey"=="" } { return "" }
    set fileName [lindex $tki(fileinfo-$fileKey) 1]
    set realPntKey [lindex $tki(fileinfo-$fileKey) 3]
    tkiStatus "Searching for ``$nodeSpec'' in $fileName"
    set nodeSpec [string tolower $nodeSpec]

    # Popup to our indirect-parent, if it has a tag table
    if { "$pntFileKey"=="" && "$realPntKey"!="" 
      && [info exist tki(indirn-$realPntKey)] } {
	return [tkiGetNodeRef $nodeSpec $realPntKey]
    }

    #  Use index on this file, pushdown to our children
    if { [info exist tki(indirn-$fileKey)] } {
	# Use node index (indirect)
	foreach indir $tki(indirn-$fileKey) {
	    if { [string match $nodeSpec [lindex $indir 0]] } {
		set nodeRef [tkiGetNodeRef $nodeSpec [lindex $indir 2] $fileKey]
	        if { "$nodeRef"!="" } { return $nodeRef }
		tkiFileWarning $fileKey "Incorrect tag table"; break
	    }
	}
    }

    # Brute force on this file
    if { [info exist tki(nodesinfo-$fileKey)] } {
	foreach nodeinfo $tki(nodesinfo-$fileKey) {
	    if { [string match $nodeSpec [string tolower [lindex $nodeinfo 1]]] } {
		return [list [lindex $nodeinfo 0] $fileKey]
	    }
	}
    }

    # Look for node in all indirect files (brute force)
    if { [info exist tki(indirf-$fileKey)] } {
	foreach indir $tki(indirf-$fileKey) {
# puts stdout "Searching $indir"
	    set nodeRef [tkiGetNodeRef $nodeSpec [lindex $indir 0] $fileKey]
	    if { "$nodeRef"!="" } { return $nodeRef }
	}
    }

    # Look for node in my parent, but only if not called from my pnt
    if { "$pntFileKey"=="" } {
	if { "$realPntKey"!="" } {
	    return [tkiGetNodeRef $nodeSpec $realPntKey]
	}
    }
    return ""
}

#
# Initialize the regexp strings that are used later in 
# tkiNodeParseBody() (for xrefs) and tkiNodeParseMenu() (for menus).
# This func is called once from tkiInit() and then destroyed.
#
proc _tkiNodeParseInit { } {
    global tki

    # For xrefs, there are two forms:
    #	*note nodeSpec::terminator			(form 1)
    #   *note label: nodeSpec terminator		(form 2)
    # Terminator is ``.'' or ``,'', forms may wrap accross lines.
#old      "\*(note\[ \t\n\]*)(\[^:\]+)::"
    set tki(re_xref1_p) "\\*(note\[ \t\n\]*)(\[^:\]+)::"
    set tki(re_xref1_s) "x\\1\037c\\2\037dxx"
#old      "\*(note\[ \t\n\]*)(\[^:\]+)(:\[ \t\n\]*)(\(\[^ \t\n)\]+\))?(\[^.,\]*)\[.,\]"
    set tki(re_xref2_p) "\\*(note\[ \t\n\]*)(\[^:\]+)(:\[ \t\n\]*)(\\(\[^ \t\n)\]+\\))?(\[^.,\]*)\[.,\]"
    set tki(re_xref2_s) "x\\1\037a\\2\037b\\3\037c\\4\\5\037dx"


    # For menus, there are two forms:
    #	* nodeSpec::	comments...			(form 1)
    #   * label: nodeSpec[ \t.,] comments...		(form 2)
#old      "(\*\[ \t\]*)(\[^:\]+)::"
    set tki(re_menu1_p) "(\\*\[ \t\]*)(\[^:\]+)::"
    set tki(re_menu1_s) "\\1\037A\\2\037B"
    # rp2 = "* ws label: ws", rp2a="rp2 nodename ws", rp2b="rp2 (file)node ws"
#old      "(\*\[ \t\]*)(\[^:\]+)(:\[ \t\]*)(\(\[^ \t)\]+\))?(\[^\t.,\]*)"
    set tki(re_menu2_p) "(\\*\[ \t\]*)(\[^:\]+)(:\[ \t\]*)(\\(\[^ \t)\]+\\))?(\[^\t.,\]*)"
    set tki(re_menu2_s) "\\1\037A\\2\037B\\3\037C\\4\\5\037D"
}

#
# Parse a nody-body and identify the cross references.
#
proc tkiNodeParseBody { nodeName fileKey bodytext } {
    global tki

    set nl "node ``($fileKey)$nodeName''"
    regsub -all -nocase $tki(re_xref1_p) $bodytext $tki(re_xref1_s) bodytext
    regsub -all -nocase $tki(re_xref2_p) $bodytext $tki(re_xref2_s) bodytext
    set xrefinfo ""
    set curIdx 1
    foreach seg [split $bodytext "\037"] {
	set stIdx $curIdx
	set curIdx [expr { $curIdx + [string length $seg] - 1 }]
	if { "[string index $seg 0]"!="c" } continue
	set toNode [string range $seg 1 end]
#puts stdout "tkiNodeParseBody:1 ``$toNode''"
	regsub -all "\[ \t\n\]+" $toNode " " toNode
#puts stdout "tkiNodeParseBody:2 ``$toNode''"
	lappend xrefinfo [list [llength $xrefinfo] $toNode $stIdx $curIdx]
    }
    return $xrefinfo
}

#
# Parse the menu and extract the keywords
#
proc tkiNodeParseMenu { nodeName fileKey menutext } {
    global tki

    # There are two forms:
    #	* nodeSpec::	comments...			(form 1)
    #   * label: nodeSpec[ \t.,] comments...		(form 2)
    set rp1 $tki(re_menu1_p)
    set sp1 $tki(re_menu1_s)
    set rp2 $tki(re_menu2_p)
    set sp2 $tki(re_menu2_s)

    set menuinfo ""
    set linecnt 0; set menucnt 0
    set nl "node ``($fileKey)$nodeName''"
    foreach line [split $menutext "\n"] {
	incr linecnt
	if { "[string index $line 0]"!="*" 
	  || "[string range $line 0 6]"=="* Menu:" } continue
	incr menucnt
#puts stdout "Try rp $line"
	if { [regsub $rp1 $line $sp1 prsline] } {
	    set nBeg [expr { [string first "\037A" $prsline] + 0 } ]
	    set nEnd [expr { [string first "\037B" $prsline] - 3 } ]
	} else {
	    if { [regsub $rp2 $line $sp2 prsline] } {
	        set nBeg [expr { [string first "\037C" $prsline] - 4 } ]
	        set nEnd [expr { [string first "\037D" $prsline] - 7 } ]
	    } else {
		tkiFileWarning $fileKey "$nl has bad menu (line $linecnt)"
		continue
	    }
	}
	set toNode [string range $line $nBeg $nEnd]
	lappend menuinfo [list $linecnt $menucnt $toNode $nBeg $nEnd]
    }
    return $menuinfo
}
