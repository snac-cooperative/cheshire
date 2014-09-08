#
# topgetopt.tcl
#
# The function has "top" prefix b/c it is conceptually part of my "top" library.
#
# Authors: Kennard White (kennard@ohm.eecs.berkeley.edu)
#	   Phil Lapsley (phil@ohm.eecs.berkeley.edu)
#
# Based on "@(#)getopt.tcl 1.5 12/7/91" by Phil Lapsley
# RCS: $Header: /export/cooley/cooley1/kennard/src/tkinfo/RCS/topgetopt.tcl,v 1.3 1993/11/23 20:50:30 kennard Exp $
#

# Ignore this; its used by some custom auto-reload software
proc topgetopt.tcl { } { }

# Simple "getopt" for TCL.
#
# topgetopt ?-any? ?-all? opt_list arg_list
# The proc will process the arguments in {arg_list} according to the
# information in {opt_list}.  Processed arguments are passed back
# to the caller by setting variables in the caller's proc-environment
# (i.e., using upvar).
#
# option_list is a list of option specs.  Each spec is a 3-tuple:
#	{ optname varname mode }
# optname is the name of the option to be parsed (without the leading dash).
# varname is the name of a tcl variable in the caller's environment.
#	If ommitted, the varname defaults to the optname.
# mode describes the type of option. If ommitted, it defaults to "single".
#	The modes:
# 	  single:	sets the variable to the next argument.
# 	  append:	lappends the next argument to the variable.
#			this allows multiple instances of the same option.
#	  boolean:	sets the variable to 0 if the argument prefix is "+"
#			 and to 1 of the argument prefix is "-".
#
# "topgetopt" sets the variables named in the option_list that were
# specified in arg_list, and returns the remainder of arg_list after
# the first non "-" or "+" option.  If a bad option specifier is
# encountered, scanning stops and getopt aborts using error.
#
# If -all is specified, then everything in arg_list must match an
# option in opt_list; that is, there may be no "leftover" arguments.
#
# If -any is specified, then processing will stop at the first
# unmatched option.  That is, the returned list of unprocessed
# arguments may contain unregcognized options.
#
# For example, the option_list:
#
#	{ min max { file filename } { toplevel toplevel boolean } }
#
# means that the option "-min value" or "-max value" should set the
# variables "min" or "max" to the specified value, and "-file foo.txt"
# should set the variable "filename" to foo.txt.  "toplevel"
# sets the variable "toplevel", and is a boolean:  the option "-toplevel"
# would set the variable "toplevel" to 1, while the option "+toplevel"
# would set the variable "toplevel" to 0.
#
# In typical usage, the caller will first initialize all the option
# variables to default values, and then call topgetopt.
#

proc topgetopt { args } {
    set do_all 0
    set do_any 0
    if { "[lindex $args 0]"=="-all" } {
	set do_all 1
	set args [lreplace $args 0 0]
    }
    if { "[lindex $args 0]"=="-any" } {
	set do_any 1
	set args [lreplace $args 0 0]
    }
    if { [llength $args]!=2 } {
	error "topgetopt: programming error: wrong number arguments\n$args"
    }
    set opt_list [lindex $args 0]
    set arg_list [lindex $args 1]

    set n [llength $arg_list]
    for { set i 0 } { $i < $n } { incr i } {
	set arg [lindex $arg_list $i]
	set argkey [string index $arg 0]
	if { "$argkey"!="-" && "$argkey"!="+" } {
	    if { $do_all } {
		error "Extra arguments after options not allowed: ``$arg''"
	    }
	    break
	}
	set argname [string range $arg 1 end]
	set matched 0
	foreach opt $opt_list {
	    if { "[lindex $opt 0]"=="$argname" } {
		set optlen [llength $opt]
		set pntVar pntVar$i
		upvar 1 [lindex $opt [expr { ($optlen > 1) ? 1 : 0 }]] $pntVar
		# lindex returns empty string for out-of-range
		case [lindex $opt 2] {
		  b* {
		    set $pntVar [expr {"$argkey"=="-" ? 1 : 0}]
		  }
		  a* {
		    lappend $pntVar [lindex $arg_list [incr i 1] ]
		  }
	          default {
		    set $pntVar [lindex $arg_list [incr i 1] ]
		  }
		}
# puts stdout "got [lindex $opt 0] -- [lindex $opt 1] -- [set $pntVar]"
		set matched 1
		break
	    }
	}
	if { $matched == 0 } {
	    if { $do_any } {
		break
	    } else {
	        error "No match for argument ``$arg''"
	    }
	}
    }
    return [lrange $arg_list $i end]
}
