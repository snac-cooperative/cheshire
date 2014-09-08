#!/bin/sh
# -*- tcl -*- \
exec wish8.0 "$0" "$@"

# Copyright (c) 1996,1997 Australian National University
# 
# The Australian National University (ANU) makes this software and all 
# associated data and documentation ('Software') available free of charge 
# for non-commercial purposes only. You may make copies of the Software but 
# you must include all of this notice on any copy.
# 
# The Software was developed for research purposes and ANU does not warrant
# that it is error free or fit for any purpose.  ANU disclaims any
# liability for all claims, expenses, losses, damages and costs any user may
# incur as a result of using, copying or modifying the Software.


set pkgname XML
set pkgdir XML
set libfiles(source,xml-parse.tcl) {# $Id: XML-1.0a1.tcl,v 1.1 2001/06/07 16:14:50 azaroth Exp $
#
# Copyright (c) 1997 Australian National University (ANU).
# 
# ANU makes this software and all associated data and documentation
# ('Software') available free of charge for non-commercial purposes only. You
# may make copies of the Software but you must include all of this notice on
# any copy.
# 
# The Software was developed for research purposes and ANU does not warrant
# that it is error free or fit for any purpose.  ANU disclaims any
# liability for all claims, expenses, losses, damages and costs any user may
# incur as a result of using, copying or modifying the Software.

##########################################
#
# XML Parser
#
# This package provides procedures for parsing 
# XML DTDs and document instances.
#
# It uses the XML namespace.
#
##########################################

package provide XML 1.0

namespace XML {
    namespace export parse parse:idx pretty_print pretty_print:idx
    namespace export parse:DTD

# XML:parse:idx --
#
# Arguments:
#	xml		Document instance
#	args	option/value pairs
#
# Recognised Options:
#	-textcommand	Default parse:text
#	-elementcommand	Default parse:elem
#	-picommand	Default parse:pi
#	-commentcommand	Default None
#	-dtd		DTD to use for document
#	-loaddtdcommand	Script to load and parse DTD
#	-errorcommand	Script to evaluate if error found
#	-warningcommand	Script to evaluate for a reportable warning
#	-unusedvariable	global variable for storing unused text
#	-statevariable	global parser variable
#	-start		Default true
#	-end		Default true
#
# Return value:
#	None.

proc parse:idx {xml args} {
    array set opts {
	-start 1 -end 1 
	-removewhitespace 0
	-textcommand		parse:text 
	-elementcommand		parse:elem 
	-picommand		parse:pi
	-loaddtdcommand		noop
	-errorcommand		noop
	-warningcommand		noop
    }
    array set opts [list \\
	-statevariable		[namespace current]::parse_state	\\
    ]
    array set opts $args	;# very rudimentary option processing
    regsub {^(1|yes|true|on)$} $opts(-start) 1 opts(-start)
    regsub {^(0|no|false|off)$} $opts(-start) 0 opts(-start)
    regsub {^(1|yes|true|on)$} $opts(-end) 1 opts(-end)
    regsub {^(0|no|false|off)$} $opts(-end) 0 opts(-end)

    # First, transform the XML into a (flat) Tcl list, ala Uhler

    # Protect Tcl special characters
    regsub -all {([{}\\\\])} $xml {\\\\\\1} xml

    # Do the translation
    set w " \\t\\r\\n"	;# white space
    set n "\\r\\n"	;# newline
    set no_ws [cl ^$w]
    set nmtoken [cl -a-zA-Z0-9._]+	;# NMTOKENs
    set name [cl a-zA-Z_][cl -a-zA-Z0-9._]*

    # steve: check whether XML swallows newlines before and/or after a tag
    # this version swallows
    # Answer: don't swallow here, but collapse ws for element-only content
    set exp <(/?)([cl ^$w>]+)[cl $w]*([cl ^>]*)(/?)>
    set sub "\\}\\n{\\\\2} {\\\\1} {\\\\4} {\\\\3} \\{"
    if {[info exists opts(-unusedvariable)]} {
	upvar #0 $opts(-unusedvariable) unused
	if {[info exists unused]} {
	    # Sanity check
	    if {$opts(-start)} {
		error {specified start of XML, but found unused XML text}
	    }
	    regsub -all $exp "$unused$xml" $sub xml
	    unset unused
	} else {
	    regsub -all $exp $xml $sub xml
	}
	set xml "{} {} {} {} \\{$xml\\}"
	if {[regexp {^([^<]*)(<[^>]*$)} [lindex $xml end] x text unused]} {
	    set xml [lreplace $xml end end $text]
	}
    } else {
	regsub -all $exp $xml $sub xml
	set xml "{} {} {} {} \\{$xml\\}"
    }

    dbgputs parse {Flat list:}
    foreach {tag close empty param text} $xml {
	dbgputs parse [list $tag $close $empty $param $text]
    }
    dbgputs parse {}

    # Now process this flat representation into a heirarchical list.
    # Start with no context.

    upvar #0 $opts(-statevariable) state
    if {[info exists opts(-dtd)]} {
	array set contentmodel [lindex $opts(-dtd) 0]
	array set attributes [lindex $opts(-dtd) 1]
	array set entities [lindex $opts(-dtd) 2]
    } else {
	# Parse the document without a DTD.  All content models assume
	# ANY, all attributes are allowed and the basic entities are
	# available.
	#array set contentmodel {}
	#array set attributes {}
	array set entities {amp & lt < gt > quot \\" apos '}
    }

    if {$opts(-start)} {
    	# The document prologue gives us the required DTD and 
    	# starting context, from which we can determine the start
    	# content model.
    	catch {unset state}
    	array set state [list mode normal RMD ALL cmodel {} context {} stack $opts(-statevariable)(container0) level 0 cid 0 preserve 0 pre_stack {}]
	upvar #0 [lindex $state(stack) end] currContainer
    	set currContainer {}
    } else {
    	upvar #0 [lindex $state(stack) end] currContainer
    }

    foreach {tag close empty param text} $xml {
	# If the current mode is cdata or comment then we must undo what the
	# regsub above has done to reconstitute the data
	switch $state(mode) {
	    comment {
		if {![string compare {} $param] && [regexp ([cl ^-]*)--[cl $w]*\\$ $tag x comm1]} {
		    # Found the end of the comment
		    dbgputs parse {end of comment (in tag)}
		    catch {append currContainer "\\n$opts(-commentcommand) [list "$state(comment_text)<$close$comm1"] {} {}"}
		    set tag {}
		    unset state(comment_text)
		    set state(mode) normal
		} elseif {[regexp ([cl ^-]*)--[cl $w]*\\$ $param x comm1]} {
		    # Found the end of the comment
		    dbgputs parse {end of comment (in attributes)}
		    catch {append currContainer "\\n$opts(-commentcommand) [list "$state(comment_text)<$close$tag $comm1"] {} {}"}
		    set tag {}
		    set param {}
		    unset state(comment_text)
		    set state(mode) normal
		} elseif {[regexp ([cl ^-]*)--[cl $w]*>(.*) $text x comm1 text]} {
		    # Found the end of the comment
		    dbgputs parse {end of comment (in text)}
		    catch {append currContainer "\\n$opts(-commentcommand) [list "$state(comment_text)<$close$tag $param>$comm1"] {} {}"}
		    set tag {}
		    set param {}
		    unset state(comment_text)
		    set state(mode) normal
		} else {
		    dbgputs parse {comment continues}
		    append state(comment_text) <$close$tag { } $param$empty>$text
		    continue
		}
	    }
	    cdata {
		if {![string compare {} $param] && [regexp ([cl ^\\]]*)\\]\\][cl $w]*\\$ $tag x cdata1]} {
		    # Found the end of the CDATA
		    dbgputs parse {end of CDATA (in tag)}
		    set text $state(cdata)<$close$cdata1$text
		    set tag {}
		    unset state(cdata)
		    set state(mode) normal
		} elseif {[regexp ([cl ^\\]]*)\\]\\][cl $w]*\\$ $param x cdata1]} {
		    # Found the end of the CDATA
		    dbgputs parse {end of CDATA (in attributes)}
		    set text $state(cdata)<$close$tag\\ $cdata1$text
		    set tag {}
		    set param {}
		    unset state(cdata)
		    set state(mode) normal
		} elseif {[regexp ([cl ^\\]]*)\\]\\][cl $w]*>(.*) $text x cdata1 text]} {
		    # Found the end of the CDATA
		    dbgputs parse {end of CDATA (in text)}
		    set text $state(cdata)<$close$tag\\ $param$empty>$cdata1$text
		    set tag {}
		    set param {}
		    unset state(cdata)
		    set state(mode) normal
		} else {
		    dbgputs parse {CDATA continues}
		    append state(cdata) <$close$tag { } $param$empty>$text
		    continue
		}
	    }
	    markupdecl {
		if {[regexp [cl $w]*([cl ^$w])[cl $w](.*) $param x id value]} {
		    catch {parse:dtd:[string toupper $tag] $id $value} err
		    continue
		} else {
		    eval $opts(-errorcommand) [list "bad syntax in internal DTD subset declaration \\"<$tag $param>\\""]
		}
		if {[regexp {]>} $text]} {
		    # Ignore other text, since we're in a PI
		    set tag {}
		    set param {}
		    set text {}
		    set state(mode) normal
		}
	    }
	}
	# default: normal mode

	# Fold tag to uppercase and process attributes
	set ftag [string toupper $tag]
	catch {unset params}
	array set params [parse:attrs $param]
	# A validator would now check the attributes against the attribute list
	# We won't bother, but (later) we will search the list for #FIXED attributes

	dbgputs parse [list processing $close$ftag$empty - context [lindex $state(context) end] container [lindex $state(stack) end] cmodel [lindex $state(cmodel) end] level $state(level) containerid $state(cid) pre $state(preserve)]

	switch -glob -- [regexp {^\\?|!.*} $ftag],$close,$empty {
	    0,, {
		# Start tag for a non-empty (container) element.

		parse:ifValid opts $ftag [list parse:element:open opts $ftag params]
	    }
	    0,/, {
		# End tag for an element.

		# This tag (the end tag) should match the current context (which was the open tag).
		if {[string compare $ftag [lindex $state(context) end]]} {
		    # Go looking for the matching start tag
		    dbgputs XML_parse [list end tag $ftag doesn't match open container [lindex $state(context) end]]
		    # First check whether the start tag is there at all
		    if {[lsearch $state(context) $ftag] >= 0} {
			dbgputs XML_parse [list open tag found - closing intervening containers]
			while {[string compare $ftag [lindex $state(context) end]]} {
			    dbgputs XML_parse [list closing intervening container [lindex $state(context) end]]
			    parse:element:close opts
			}
			# Now finally close the container
			parse:element:close opts
			eval $opts(-errorcommand) unbalanced [list "unbalanced open tags for close tag \\"$ftag\\""]
		    } else {
			# No start tag found - ignore this tag
			dbgputs XML_parse [list ignoring close tag $ftag - start tag not found]
			eval $opts(-errorcommand) unmatchedclose [list "ignoring close tag for element \\"$ftag\\": no matching start tag"]
		    }

		} else {
		    dbgputs XML_parse [list close container $ftag]
		    parse:element:close opts
		}
	    }
	    0,,/ {
		# Empty element

		parse:ifValid opts $ftag [list append currContainer "\\n$opts(-elemcommand) $ftag [list [array get params]] {}"]
	    }
	    1,, {
		# Processing instructions or XML declaration
		switch -glob -- $ftag {
		    \\\\?XML {
			# $params(RMD) indicates whether DTD is required or not
			catch {
			    if {![regexp {(NONE|INTERNAL|ALL)} [string toupper $params(RMD)] x state(RMD)]} {
				eval $opts(-errorcommand) invalidvalue [list "invalid value \\"$params(RMD)\\" for Required Markup Declaration"]
			    }
			}
			append currContainer "\\n$opts(-picommand) $ftag [list [array get params]] {}"
		    }
		    \\\\?* {
			append currContainer "\\n$opts(-picommand) $ftag [list [array get params]] {}"
		    }
		    !DOCTYPE {
			# Parse the params supplied.  Looking for Name, ExternalID and MarkupDecl
			regexp ^[cl $w]*($name)(.*) $param x state(doc_name) param
			set state(doc_name) [string toupper $state(doc_name)]
			set externalID {}
			if {[regexp -nocase ^[cl $w]*(SYSTEM|PUBLIC)(.*) $param x id param]} {
			    switch [string toupper $id] {
				SYSTEM {
				    if {[regexp ^[cl $w]+"([cl ^"]*)"(.*) $param x systemlit param] || [regexp ^[cl $w]+'([cl ^']*)'(.*) $param x systemlit param]} {
					set externalID [list SYSTEM $systemlit]
				    } else {
					eval $opts(-errorcommand) syntax [list "SYSTEM identifier not followed by literal"]
				    }
				}
				PUBLIC {
				    if {[regexp ^[cl $w]+"([cl ^"]*)"(.*) $param x pubidlit param] || [regexp ^[cl $w]+'([cl ^']*)'(.*) $param x pubidlit param]} {
					if {[regexp ^[cl $w]+"([cl ^"]*)"(.*) $param x systemlit param] || [regexp ^[cl $w]+'([cl ^']*)'(.*) $param x systemlit param]} {
					    set externalID [list PUBLIC $pubidlit $systemlit]
					} else {
					    eval $opts(-errorcommand) syntax [list "PUBLIC identifier not followed by system literal"]
					}
				    } else {
					eval $opts(-errorcommand) syntax [list "PUBLIC identifier not followed by literal"]
				    }
				}
			    }
			    if {[regexp -nocase ^[cl $w]+NDATA[cl $w]+($name)(.*) $param x notation param]} {
				lappend externalID $notation
			    }
			}

			append currContainer "\\n$opts(-picommand) $ftag [list $externalID] {}"

			catch {
			    # If a DTD is supplied on the command line, it overrides any DTD specified by the PI
			    if {![string compare {} $opts(-dtd)] && ![string compare ALL $state(RMD)]} {
				eval $opts(-loaddtdcommand) $externalID
			    }
			}

			# Now parse markupdecl
			if {[regexp [format {^[%s]*\\[[^<]*<([^%s]+)[%s]*([^%s]*)[%s]*(.*)} $w $w $w $w $w] $param x decl id value]} {
			    set state(mode) markupdecl
			    catch {parse:dtd:[string toupper $decl] $id $value} err
			}

			if {[info exists contentmodel]} {
			    set state(cmodel) [list [list $state(doc_name) {}]]
			    dbgputs XML_parse [list !doctype set cmodel to $state(cmodel)]
			}
		    }
		    !--* {
			# Start of a comment
			# See if it ends in the same tag, otherwise change the
			# parsing mode
			regexp {!--(.*)} $tag x comm1
			if {[regexp ([cl ^-]*)--[cl $w]*\\$ $param x comm2]} {
			    dbgputs parse {processed comment (end in attributes)}
			    catch {append currContainer "\\n$opts(-commentcommand) [list $comm1$comm2] {} {}"}
			} elseif {[regexp ([cl ^-]*)--[cl $w]*\\$ $text x comm2]} {
			    dbgputs parse {processed comment (end in text)}
			    catch {append currContainer "\\n$opts(-commentcommand) [list "$comm1 $param>$comm2"] {} {}"}
			} else {
			    dbgputs parse {start of comment}
			    set state(mode) comment
			    set state(comment_text) "$comm1 $param>$text"
			    continue
			}
		    }
		    ![CDATA[* {
			dbgputs parse {start CDATA section}
			regexp {!\\[CDATA\\[(.*)} $tag x cdata1
			if {[regexp {([^\\]]*)]]$} $param x cdata2]} {
			    dbgputs parse {processed CDATA (end in attribute)}
			    set text "$cdata1 $cdata2$text"
			} elseif {[regexp {([^\\]]*)]]>(.*)} $text x cdata2 text]} {
			    dbgputs parse {processed CDATA (end in text)}
			    set text "$cdata1 $param>$cdata2$text"
			} else {
			    dbgputs parse {start CDATA}
			    set state(cdata) "$cdata1 $param$text"
			    set state(mode) cdata
			    continue
			}
		    }


		    !ELEMENT {
			# Internal DTD declaration
		    }
		    !ATTLIST {
		    }
		    !ENTITY {
		    }
		    !NOTATION {
		    }


		    !* {
			append currContainer "\\n$opts(-picommand) $ftag [list $param] {}"
		    }
		    default {
			eval $opts(-errorcommand) unknown [list "unknown processing instruction \\"<$ftag>\\""]
		    }
		}
	    }
	    1,* -
	    0,/,/ {
		# Syntax error
	    	eval $opts(-errorcommand) syntax [list "syntax error: tag \\"</$ftag/>\\""]
	    }
	}

	# Add the text to the current container
	# Must check that #PCDATA is in the content model

	# XML states that white space is removed for element-only content,
	# but is forwarded to the application in mixed content.
	# The attribute XML-SPACE="PRESERVE" can be used to force white space to
	# be kept.  However, applications may want white space collapsed wherever
	# possible.  -removewhitespace may be used to indicate this behaviour.

	set text [expr {($state(preserve) || ![info exists contentmodel]) || !$opts(-removewhitespace) ? [entity $text] : [zap_white [entity $text]]}]
	switch -glob [info exists contentmodel([lindex $state(context) end])],[string compare 0 [regexp $no_ws $text]] {
	    1,-1 {
		dbgputs XML_parse [list processing #PCDATA - current content model [lindex $state(cmodel) end]]
		# Is #PCDATA valid for the current content model?
		parse:ifValid opts #PCDATA [list append currContainer "\\n$opts(-textcommand) [list $text] {} {}"]
	    }
	    0,-1 {
		# No content model for this element, assume ANY
		dbgputs XML_parse [list no content model for current element, assuming ANY]
		append currContainer "\\n$opts(-textcommand) [list $text] {} {}"
	    }
	}
    }

    # If this is the end of the document, close all open containers
    if {$opts(-end)} {
    	while {$state(level) > 0} {
	    dbgputs XML_parse [list at end - popping stack]
	    parse:element:close opts
	}
    }

    return {}
}

# Support routines

proc cl x {return "\\[$x\\]"}
set w " \\t\\r\\n"	;# white space

proc parse:ifValid {optvar content validcmd} {
    upvar $optvar opts
    upvar #0 $opts(-statevariable) state
    upvar contentmodel cm

    # Is this element valid for the current content model?
    if {![info exists cm] && [string compare {} $content]} {
	# There is no content model.
	uplevel 1 $validcmd
    } elseif {![string compare * [lindex [lindex $state(cmodel) end] 0]] || [lsearch [lindex [lindex $state(cmodel) end] 0] $content] >= 0} {
	dbgputs XML_parse [list content $content is OK]
	# Modify the content model according to the current rep
	switch [lindex [lindex $state(cmodel) end] 1] {
	    {} -
	    ? {
		# remove this model from the current content model
		set state(cmodel) [lreplace $state(cmodel) end end [lreplace [lindex $state(cmodel) end] 0 1]]
	    }
	    * -
	    + {# no change necessary}
	}

	# Go ahead and run script
	uplevel 1 $validcmd

    } else {
	dbgputs XML_parse [list element $content not in content model]
	# There are four options:
	#   - as reps of content models allow, search ahead for a context where this element is valid, and modify content model appropriately
	#   - search ahead through the content model and imply intervening elements
	#   - close elements until an element lower in the stack allows this element
	#   - ignore this element (a "hard" error)
    }
}

proc parse:element:open {optvar tag param} {
    upvar $optvar opts
    upvar #0 $opts(-statevariable) state
    upvar contentmodel cm
    upvar currContainer current
    upvar $param params

    dbgputs XML_parse [list parse:element:open current: level $state(level) cid $state(cid) cmodel $state(cmodel)]
    incr state(level)	;# invariant: $state(level) == [llength $state(stack)]
    set container $opts(-statevariable)(container[incr state(cid)])
    append current "\\n$opts(-elementcommand) $tag [list [array get params]] $container"
    lappend state(stack) $container
    uplevel "upvar #0 $container currContainer; set currContainer {}"
    lappend state(context) $tag
    catch {lappend state(cmodel) $cm($tag)}
    dbgputs XML_parse [list content model is now $state(cmodel)]

    # Need to keep a reference count for handling nested preformatted elements
    if {[catch {
	if {![string compare PRESERVE $params(XML-SPACE)]} {
	    dbgputs XML_parse {starting preformatted text}
	    incr state(preserve)
	    lappend state(pre_stack) 1
	} else {
	    lappend state(pre_stack) 0
	}
    }]} {
    	lappend state(pre_stack) 0
    }

    return $container
}

proc parse:element:close optvar {
    upvar $optvar opts
    upvar #0 $opts(-statevariable) state

    # Decrement preformatted reference count appropriately
    incr state(preserve) -[lindex $state(pre_stack) end]
    set state(pre_stack) [lreplace $state(pre_stack) end end]	;# pop stack

    incr state(level) -1
    set state(stack) [lreplace $state(stack) end end]
    set state(context) [lreplace $state(context) end end]
    catch {set state(cmodel) [lreplace $state(cmodel) end end]}
    uplevel "upvar #0 [lindex $state(stack) end] currContainer"
    dbgputs XML_parse [list parse:element:close $optvar - new: level $state(level) stack $state(stack) pre $state(preserve)]
}

# parse:attrs --
#
# Parse attributes.  There are two forms for name-value pairs:
#
#	name="value"
#	name='value'
#
# Arguments:
#	attrs	attribute string given in a tag
#
# Return Value:
#	A Tcl list representing the name-value pairs in the attribute string

proc parse:attrs {attrs} {
    # First check whether there's any work to do
    if {![string compare {} [string trim $attrs]]} {
	return {}
    }

    # Protect Tcl special characters
    regsub -all {([[\\$\\\\])} $attrs {\\\\\\1} attrs

    set ws "\\n\\t "
    regsub [format {([a-zA-Z0-9]+)[%s]*=[%s]*"([^"]*)"} $ws $ws] $attrs {[set parsed([string toupper {\\1}]) {\\2}] } attrs ;# "
    regsub [format {([a-zA-Z0-9]+)[%s]*=[%s]*'([^']*)'} $ws $ws] $attrs {[set parsed([string toupper {\\1}]) {\\2}] } attrs
    subst $attrs

    return [array get parsed]
}

# parse --
#
# Parse XML text into a Tcl heirarchical list format.
# First parses into indexed list format, and then expands the
# indexed structures to arrive at final format.

proc parse {xml args} {
    array set opts [list \\
	-statevariable	[namespace current]::parse_state \\
	-textcommand	parse:text \\
	-elemcommand	parse:elem \\
	-picommand	parse:pi \\
	-commentcommand	parse:comment \\
    ]
    array set opts $args

    eval parse:idx [list $xml] $args

    return [parse:expand $opts(-statevariable) $opts(-elemcommand) $opts(-textcommand) $opts(-picommand) $opts(-commentcommand)]
}

proc parse:expand {state elemcmd txtcmd picmd commentcmd} {
    upvar #0 $state data
    return [parse:expand_int ${state}(container0) $elemcmd $txtcmd $picmd $commentcmd]
}

proc parse:expand_int {container elemcmd txtcmd picmd commentcmd} {
    upvar #0 $container data
    if {![info exists data]} return;
    set ret {}
    foreach {type arg1 arg2 arg3} $data {
	switch $type \\
	    $elemcmd {
		lappend ret $type $arg1 $arg2 [parse:expand_int $arg3 $elemcmd $txtcmd $picmd $commentcmd]
	    } \\
	    $txtcmd - \\
	    $picmd - \\
	    $commentcmd {
		lappend ret $type $arg1 $arg2 $arg3
	    } \\
	    default {error [list $type doesn't match $elemcmd or $txtcmd]}
    }
    return $ret
}

# Print out a nice representation of a parsed XML structure

proc pretty_print {xml {indent {}} {elemcmd parse:elem} {txtcmd parse:text} {picmd parse:pi} {commentcmd parse:comment}} {
    set ret {}
    foreach {type arg1 arg2 arg3} $xml {
	switch $type \\
	    $picmd - \\
	    $commentcmd - \\
	    $txtcmd {
		if {[string compare {} $indent]} {
		    append ret [format %${indent}s { }]
		}
		append ret [list $arg1]\\n
	    } \\
	    $elemcmd {
		if {[string compare {} $indent]} {
		    append ret [format %${indent}s { }]
		} else {
		    append indent 0
		}
		append ret "[list $arg1] [list $arg2] \\{\\n"
		append ret [pretty_print $arg3 [expr $indent + 4] $elemcmd $txtcmd $picmd $commentcmd]
		if {$indent > 0} {
		    append ret [format %${indent}s { }]
		}
		append ret \\}\\n
	    }
    }
    return $ret
}

# Similar to above, but work from state variable with container pointers

proc pretty_print:idx {{container parse_state(container0)} {indent {}} {elemcmd parse:elem} {txtcmd parse:text} {picmd parse:pi} {commentcmd parse:comment}} {
    upvar #0 $container xml

    if {![info exists xml]} {
	if {[string compare {} $indent]} {
	    append ret [format %${indent}s { }]
	}
    	return {}
    }

    set ret {}
    foreach {type arg1 arg2 arg3} $xml {
	switch $type \\
	    $picmd - \\
	    $commentcmd - \\
	    $txtcmd {
		if {[string compare {} $indent]} {
		    append ret [format %${indent}s { }]
		}
		append ret [list $arg1]\\n
	    } \\
	    $elemcmd {
		if {[string compare {} $indent]} {
		    append ret [format %${indent}s { }]
		} else {
		    append indent 0
		}
		append ret "[list $arg1] [list $arg2] $arg3 \\{\\n"
		append ret [pretty_print:idx $arg3 [expr $indent + 4] $elemcmd $txtcmd $picmd $commentcmd]
		if {$indent > 0} {
		    append ret [format %${indent}s { }]
		}
		append ret \\}\\n
	    }
    }
    return $ret
}

# Debugging.  See Utilities package for how to enable.

proc dbgputs args {}

# Do-nothing proc

proc noop args {}

### Following procedures are based on html_library

# Convert multiple white space into a single space

proc zap_white data {
    regsub -all "\\[ \\t\\r\\n\\]+" $data { } data
    return $data
}

# find XML entity references (syntax: &xxx;)

proc entity [list text [list entities [namespace current]::entity_predef]] {
    if {![regexp & $text]} {return $text}
    regsub -all {([][$\\\\])} $text {\\\\\\1} new
    regsub -all {&#(x?)([0-9]+);} \\
	    $new {[format %c [scan \\2 %[expr {\\1 == {} ? d : x}] tmp; set tmp]]} new
    regsub -all {&([a-zA-Z]+);} $new [format {[entity:deref %s \\1]} $entities] new
    return [subst $new]
}

# convert an XML escape sequence into character

proc entity:deref [list text [list entities [namespace current]::entity_predef] {unknown ?}] {
    upvar #0 $entities map
    set result $unknown
    catch {set result $map($text)}
    return $result
}

# table of predefined entities

array set entity_predef {
   lt <   gt >   amp &   quot \\"   apos '
}

####################################
#
# DTD parser for XML
#
# A DTD is represented as a three element Tcl list.
# The first element contains the content models for elements,
# the second contains the attribute lists for elements and
# the last element contains the entities for the document.
#
####################################

proc parse:DTD {dtd args} {
    array set opts {
	-errorcommand		noop
    }
    array set opts $args

    set w " \\t\\r\\n"	;# white space
    set exp <!([cl ^$w>]+)[cl $w]*([cl ^$w]+)[cl $w]*([cl ^>]*)>
    set sub {{\\1} {\\2} {\\3}}
    regsub -all $exp $dtd $sub dtd

    foreach {decl id value} $dtd {
	catch {parse:dtd:[string toupper $decl] $id $value} err
    }

    return [list [array get contentmodel] [array get attributes] [array get entities]]
}

# Procedures for handling the various declarative elements in a DTD
# New elements may be added by creating a procedure of the form
# parse:dtd:_element_

# For each of these procedures, the various regular expressions they use
# are created outside of the proc to avoid overhead at runtime

proc parse:dtd:element {id value} {
    dbgputs XML_DTD_parse [list parse:dtd:element $id $value]
    upvar opts state
    upvar contentmodel cm
    upvar otherModels om

    if {[info exists cm($id)] || [info exists im($id)] || [info exists om($id)]} {
	eval $state(-errorcommand) element [list "element \\"$id\\" already declared"]
    } else {
	switch -- $value {
	    EMPTY {
	    	set om($id) {}
	    }
	    ANY {
	    	set cm($id) *
	    }
	    default {
		# Translate the content model into Tcl list format
		regsub -all {\\(} $value " \\{ " value
		regsub -all {\\)} $value " \\} " value
		regsub -all {([*,+|?])} $value { \\1 } value
		if {[catch {parse:dtd:element:cmodel $value 2} result]} {
		    eval $state(-errorcommand) element [list $result]
		} else {
		    set cm($id) $result
		}
	    }
	}
    }
}

# parse:dtd:element:cmodel --
#
# Parse an element content model.
# Content models are sequences of choices, possibly nested.
# If the model starts with a choice, then it is a single sequence
# at the top-level.
#
# This is going to need alot of work!
#
# Return Result:
#	A Tcl list representing the content model:
#	{elements1} rep {elements2} rep ...
#	Eg: (head, (p|list|note)*, div*) becomes:
#	head {} {p list note} * div *

proc parse:dtd:element:cmodel {value depth {nested 0}} {
    upvar $depth entities ents
    dbgputs XML_DTD_parse [list parse:dtd:element:cmodel $value $depth]
    set model {}
    set result {}
    set rep 0

    foreach part $value {
	if {[llength $part] > 1} {
	    set result [parse:dtd:element:cmodel $part [expr $depth + 1] 1]
	    if {[llength $result] == 1} {
	    	lappend model [lindex $result 0]
	    } else {
		dbgputs XML_DTD_parse [list appending $result]
	    	eval lappend model $result
	    }
	    set result {}
	    set rep 2	;# 2 indicates that next rep should replace rep returned
	} else {
	    switch -regexp -- $part {
		%[^;]*; {
		    # Substitute parameter entity
		    regexp {%([^;]*);} $part x pentity
		    if {[catch {lappend result $ents($pentity)}]} {
		    	error "unknown parameter entity \\"$pentity\\""
		    }
		}
	    	, {
		    if {[llength $result]} {
		    	lappend model $result
		    }
		    set result {}
		    if {!$rep} {
		    	lappend model {}
		    }
		    set rep 0
		}
		\\\\| {
		}
		\\\\+ -
		\\\\* -
		\\\\? {
		    if {[llength $result]} {
		    	lappend model $result
		    	set result {}
		    }
		    if {$rep == 2} {
			set model [lreplace $model end end $part]
		    } else {
			lappend model $part
		    }
		    set rep 1
		}
		default {
		    lappend result [lindex $part 0]
		}
	    }
	}
    }

    if {[llength $result]} {
    	lappend model $result
    }
    if {!$rep} {
    	lappend model {}
    }

    return $model
}

# Watch out for case-sensitivity

set attlist_exp [cl $w]*([cl ^$w]+)[cl $w]*([cl ^$w]+)[cl $w]*(#REQUIRED|#IMPLIED)
set attlist_enum_exp [cl $w]*([cl ^$w]+)[cl $w]*\\\\(([cl ^)]*)\\\\)[cl $w]*("([cl ^")])")? ;# "
set attlist_fixed_exp [cl $w]*([cl ^$w]+)[cl $w]*([cl ^$w]+)[cl $w]*(#FIXED)[cl $w]*([cl ^$w]+)

proc parse:dtd:attlist {id value} {
    variable attlist_exp attlist_enum_exp attlist_fixed_exp
    dbgputs XML_DTD_parse [list parse:dtd:attlist $id $value]
    upvar opts state
    upvar attributes am

    if {[info exists am($id)]} {
	eval $state(-errorcommand) attlist [list "attribute list for element \\"$id\\" already declared"]
    } else {
	# Parse the attribute list.  If it were regular, could just use foreach,
	# but some attributes may have values.
	regsub -all {([][$\\\\])} $value {\\\\\\1} value
	regsub -all $attlist_exp $value {[parse:dtd:attlist:att {\\1} {\\2} {\\3}]} value
	regsub -all $attlist_enum_exp $value {[parse:dtd:attlist:att {\\1} {\\2} {\\3}]} value
	regsub -all $attlist_fixed_exp $value {[parse:dtd:attlist:att {\\1} {\\2} {\\3} {\\4}]} value
	subst $value
	set am($id) [array get attlist]
    }
}

proc parse:dtd:attlist:att {name type default {value {}}} {
    upvar attlist al
    # This needs further work
    set al($name) [list $default $value]
}

set param_entity_exp [cl $w]*([cl ^$w]+)[cl $w]*([cl ^"$w]*)[cl $w]*"([cl ^"]*)"

proc parse:dtd:entity {id value} {
    variable param_entity_exp
    dbgputs XML_DTD_parse [list parse:dtd:entity $id $value]
    upvar opts state
    upvar entities ents

    if {[string compare % $id]} {
	# Entity declaration
	if {[info exists ents($id)]} {
	    eval $state(-errorcommand) entity [list "entity \\"$id\\" already declared"]
	} else {
	    if {![regexp {"([^"]*)"} $value x entvalue] && ![regexp {'([^']*)'} $value x entvalue]} {
		eval $state(-errorcommand) entityvalue [list "entity value \\"$value\\" not correctly specified"]
	    } ;# "
	    set ents($id) $entvalue
	}
    } else {
	# Parameter entity declaration
	switch -glob [regexp $param_entity_exp $value x name scheme data],[string compare {} $scheme] {
	    0,* {
		eval $state(-errorcommand) entityvalue [list "parameter entity \\"$value\\" not correctly specified"]
	    }
	    *,0 {
	    	# SYSTEM or PUBLIC declaration
	    }
	    default {
	    	set ents($id) $data
	    }
	}
    }
}

set notation_exp [cl $w]*([cl ^$w]+)[cl $w]*(.*)

proc parse:dtd:notation {id value} {
    variable notation_exp
    upvar opts state

    if {[regexp $notation_exp $value x scheme data] == 2} {
    } else {
	eval $state(-errorcommand) notationvalue [list "notation value \\"$value\\" incorrectly specified"]
    }
}

# End XML namespace
}

}


# -- installer.tcl --

# Given entries in arrays libfile and exefile, install the
# files into the appropriate directories on a system.

# Find where the library files (package) should be installed.

set install_libdir [file dirname $tcl_library]
set install_exedir [file dirname [info script]]
if {![string compare {} $install_exedir]} {
    set install_exedir [pwd]
}

wm title . "$pkgname Installation"
label .title -text "$pkgname Package Installation" -font {Times 18}
grid .title - - -row 0 -pady 10

. configure -menu .menu
menu .menu -tearoff 0
.menu add cascade -label File -menu .menu.file
.menu add cascade -label Edit -menu .menu.edit
.menu add cascade -label Help -menu .menu.help
menu .menu.file -tearoff 0
menu .menu.edit -tearoff 0
menu .menu.help -tearoff 0

.menu.file add command -label Install -command pkg:install
.menu.file add separator
.menu.file add command -label Quit -command inst:quit

.menu.edit add command -label Cut   -state disabled
.menu.edit add command -label Copy  -state disabled
.menu.edit add command -label Paste -state disabled

.menu.help add command -label Install -command tkAboutDialog
.menu.help add command -label $pkgname -command [list help:package $pkgname]

label .instlab -text "Install package $pkgname into directory:"
label .instdir -textvariable install_libdir -relief solid -bd 1
button .instchange -text Choose... -command [list change install_libdir {Package Installation Directory}]
grid .instlab .instdir -row 1 -sticky new -padx 5
grid .instchange -row 1 -column 2 -sticky ne

label .exelab -text "Install programs into directory:"
label .exedir -textvariable install_exedir -relief solid -bd 1
button .exechange -text Choose... -command [list change install_exedir {Program Installation Directory}]
grid .exelab .exedir -row 2 -sticky new -padx 5
grid .exechange -row 2 -column 2 -sticky ne

label .feedback -textvariable feedback
grid .feedback -row 3

set f [frame .log]
text $f.log -yscrollcommand [list $f.sy set] -height 10
$f.log insert end "Installation log:\n\n"
$f.log configure -state disabled
scrollbar $f.sy -orient vertical -command [list $f.log yview]
grid $f.log -row 0 -column 0 -sticky news
grid $f.sy -row 0 -column 1 -sticky ns
grid columnconfigure $f 0 -weight 1
grid rowconfigure $f 0 -weight 1
grid $f - - -row 4 -sticky ew

button .install -text Install -command pkg:install
button .quit -text Quit -command inst:quit
grid .install -row 5 -sticky w
grid .quit -row 5 -column 2 -sticky e

grid columnconfigure . 1 -weight 1

proc pkg:install {} {
    global pkgname pkgdir install_libdir install_exedir feedback libfiles exefiles

    set feedback {}
    update
    foreach {dir descr} [list [file join $install_libdir $pkgdir] Package $install_exedir Program] {
	if {![file isdirectory $dir]} {
	    if {[file exists $dir]} {
		tk_messageBox -title {Installation Error} -message "$descr installation directory \"$dir\" already exists and is not a directory.\n\nPlease remove the file and then restart installation" -type ok
		set feedback {Installation aborted}
		return
	    } else {
		inst:log [list Creating package directory $dir]
		file mkdir $dir
	    }
	}
    }

    set feedback {Installing Library Files...}
    update
    set source [set normal {}]
    foreach {libfile script} [array get libfiles] {
	foreach {type filename} [split $libfile ,] break
	inst:log [list Creating library file [file join [file join $install_libdir $pkgdir] $filename]]
	set fd [open [file join [file join $install_libdir $pkgdir] $filename] w]
	puts $fd [subst -novariables -nocommands $script]
	close $fd
	lappend $type $filename
    }

    set feedback {Creating Package Index...}
    update
    if {[llength $normal]} {
	inst:log [list Creating package index file for directory $install_libdir]
	eval pkg_mkIndex [list [file join $install_libdir $pkgdir]] $normal
    }
    if {[llength $source]} {
	set fd [open [file join [file join $install_libdir $pkgdir] pkgIndex.tcl] a+]
	puts $fd {}
	foreach filename $source {
	    inst:log [list Adding package information for $filename  to index]
	    # Find `package provide ...' command from package
	    regexp "package provide (\[^\n;\]*)" $libfiles(source,$filename) all info
	    regexp {([0-9]+\.[0-9]+[^ ]*)} $info all version
	    puts $fd "package ifneeded [lindex $info 0] $version \[list source \[file join \$dir $filename\]\]"
	}
	close $fd
    }

    set feedback {Installing Programs...}
    update
    foreach {filename script} [array get exefiles] {
	inst:log [list Creating program [file join $install_exedir $filename]]
	set fd [open [file join $install_exedir $filename] w]
	puts $fd [subst -nocommands -novariables $exefiles($filename)]
	close $fd
	file attributes [file join $install_exedir $filename] -permissions 0755
    }

    inst:log {Installation complete}
    set feedback {Installation Complete!}
}

proc inst:log log {
    .log.log configure -state normal
    .log.log insert end $log {} \n
    .log.log see end
    .log.log configure -state disabled
}

proc change {varname descr} {
    upvar #0 $varname var
    set filename [tk_getSaveFile -title $descr]
    if {[string compare {} $filename]} {
	set var $filename
    }
}

proc tkAboutDialog {} {
    catch {destroy .help}
    toplevel .help
    message .help.msg -text "Tcl/Tk Installer\n\nSteve Ball\n\nhttp://tcltk.anu.edu.au/" -aspect 1500
    button .help.dismiss -text Dismiss -command {destroy .help}
    grid .help.msg
    grid .help.dismiss
}

proc help:package pkg {
    upvar #0 help_text help

    catch {destroy .help}
    toplevel .help
    if {[catch {message .help.msg -text $help($pkg) -aspect 1500}]} {
	message .help.msg -text "No help available for package $pkg"
    }
    button .help.dismiss -text Dismiss -command {destroy .help}
    grid .help.msg
    grid .help.dismiss
}

proc inst:quit {} {
    global pkgname

    if {![string compare yes [tk_messageBox -icon question -message "Are you sure you want to quit installing $pkgname?" -default no -parent . -title {Really Quit?} -type yesno]]} {
	destroy .
    }
}

# -- end installer.tcl --

