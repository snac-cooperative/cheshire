# Aaron Sami Abassi, 2001
# Public Domain Software
# Should this program fail you, and you feel I should be legally
# responsible, consult my complaints department at /dev/null
# You are solely responsible for it's use.

package provide cTCL 0.1.5

namespace eval ::cTCL {namespace export new delete}
namespace eval ::cTCL::classes {namespace export class}
namespace eval ::cTCL::heap {}
namespace eval ::cTCL::lib {}
namespace eval ::cTCL::objects {namespace export object copy}
namespace eval ::cTCL::procs {}
namespace eval ::cTCL::references {namespace export reference}
namespace eval ::cTCL::references::arrays {}

# new class ?constructor args ...?
#  Returns a new object_namespace
# new class* ?object_namespace?
#  Returns a new reference command
proc ::cTCL::new {class args} {
	set fromns [uplevel namespace current]
	switch -regexp -- $class {
		{[*]$} {
			set class [string range $class 0 end-1]
			set type reference
		}
		default {
			set type object
		}
	}
	switch -regexp -- $class {
		{^[:]{2}} {
			set classns $class
		}
		default {
			set classns [::cTCL::classes::locate $fromns $class]
		}
	}
	set target [::cTCL::lib::unique]
	switch -- $type {
		reference {
			set code [list ::cTCL::references::reference]
			lappend code $fromns\::$target
			lappend code $classns
			foreach arg $args {
				lappend code $arg
			}
		}
		object {
			set code [list ::cTCL::objects::instanciate]
			lappend code $classns
			lappend code ::cTCL::heap::$target
			foreach arg $args {
				lappend code $arg
			}
		}
	}
	if {[catch {
		set ret [eval $code]
	} errMSG]} {
		error $errMSG $errMSG
	}
	return $ret
}

# Deletes a reference or an object
# If it finds both, deletes the reference
proc ::cTCL::delete {symbol} {
	if {! [regexp {^[:]{2}} $symbol]} {
		set symbol [::cTCL::lib::nsattach [uplevel namespace current] $symbol]
	}
	if {[::cTCL::references::is $symbol]} {
		::cTCL::references::delete $symbol
		return
	}
	::cTCL::objects::delete $symbol
}

# Error table for class procedure
namespace eval ::cTCL::classes {
	variable perr
	array set perr {
		class_syntax
		{Syntax:  class namespace ?parent1 parent2..? definition}
		member_array_syntax
		{Syntax:  array name ?{default value}?}
		member_variable_syntax
		{Syntax:  variable name ?{default value}?}
		member_class_syntax
		{Syntax:  class name ?{list of parent classes}? {definition}}
		member_proc_syntax
		{Syntax:  proc name ?{arg1 .. args} {procedure body}?}
		member_object_syntax1
		{Syntax:  object class_name object_name}
		member_object_syntax2
		{Syntax:  class_name object_name}
		member_reference_syntax
		{Syntax:  reference name ?class_name?}
		member_reference_syntax2
		{Syntax:  class_name* name}
		member_namespace_qualifiers
		{Class members cannot include namespace qualifiers (::)}
		duplicate_members
		{Duplicate member names are not allowed}
		virtual_proc_only
		{Virtual may only be used with proc}
		constructor_name_reserved
		{Only the constructor proc may assume the class name}
		destructor_name_reserved
		{Only the destructor proc may assume the ~class name}
		constructor_static
		{The constructor function cannot be static}
		destructor_static
		{The destructor function cannot be static}
		constructor_syntax
		{Syntax:  constructor ?{args} {body}?}
		destructor_syntax
		{Syntax:  destructor ?{body}?}
		virtual_constructor
		{Constructor cannot be virtual}
		virtual_static
		{Virtual procedures cannot be static}
	}
}

# Parses and produces a class namespace
proc ::cTCL::classes::class {args} {
#if {[catch {
	variable perr
	if {[llength $args] < 2} {
		error $class_syntax $class_syntax
	}
	set i 0
	if {! [regexp {^[:]{2}} [lindex $args $i]]} {
		set classname [::cTCL::lib::nsattach [uplevel namespace current] [lindex $args $i]]
	} else {
		set classname [lindex $args $i]
	}
	incr i
	namespace eval $classname {}
	upvar 0 $classname\::_cTCL_members_table	members_table
	upvar 0 $classname\::_cTCL_members_access	members_access
	upvar 0 $classname\::_cTCL_members_type 	members_type
	upvar 0 $classname\::_cTCL_members_scope	members_scope
	upvar 0 $classname\::_cTCL_members_args 	members_args
	upvar 0 $classname\::_cTCL_members_local	members_local
	upvar 0 $classname\::_cTCL_members_virtual	members_virtual
	upvar 0 $classname\::_cTCL_parent_classes	parent_classes
	upvar 0 $classname\::_cTCL_call_classes 	call_classes
	upvar 0 $classname\::_cTCL_object_variables object_variables
	upvar 0 $classname\::_cTCL_static_variables static_variables
	array set members_table   {}
	array set members_access  {}
	array set members_type	  {}
	array set members_scope   {}
	array set members_args	  {}
	array set members_virtual {}
	set members_local		  {}
	set parent_classes		  {}
	set call_classes		  {}
	set object_variables	  {}
	set static_variables	  {}
	while {$i < [expr [llength $args] - 1]} {
		set pclass [::cTCL::classes::locate [namespace qualifiers $classname] [lindex $args $i]]
		lappend parent_classes $pclass
		incr i
	}
	::cTCL::classes::inherit $classname
	set predef [lindex $args $i]
	incr i
	set predef [split $predef \n]
	set items {}
	foreach line $predef {
		if {[string length [string trim $line]]} {
			append item $line
			if {[catch {
				llength $item
			} errmsg]} {
				if {! [info exists omsg]} {
					set omsg $errmsg
				}
				append item \n
			} else {
				lappend items $item
				if {[info exists omsg]} {
					unset omsg
				}
				unset item
			}
		}
	}
	if {[info exists item]} {
		error $omsg $omsg
	}
	set daccess private
	foreach item $items {
		if {[string first "#" [string trimleft $item]] != 0} {
			set iSize [llength $item]
			set i 0
			set member(access) $daccess
			set member(scope) object
			while {$i < $iSize} {
				switch -glob -- [lindex $item $i] {
					private: {
						set daccess private
						set member(access) private
						incr i
					}
					protected: {
						set daccess protected
						set member(access) protected
						incr i
					}
					public: {
						set daccess public
						set member(access) public
						incr i
					}
					private {
						set member(access) private
						incr i
					}
					protected {
						set member(access) protected
						incr i
					}
					public {
						set member(access) public
						incr i
					}
					static {
						set member(scope) static
						incr i
					}
					virtual {
						set member(virtual) 1
						incr i
					}
					class {
						set member(scope) static
						set member(type) class
						incr i
						if {$i == $iSize} {
							error $perr(member_class_syntax) $perr(member_class_syntax)
						}
						set member(name) [lindex $item $i]
						incr i
						if {$i == $iSize} {
							error $perr(member_class_syntax) $perr(member_class_syntax)
						}
						while {$i < $iSize} {
							lappend member(args) [lindex $item $i]
							incr i
						}
					}
					array {
						set member(type) array
						incr i
						if {$i == $iSize} {
							error $perr(member_array_syntax) $perr(member_array_syntax)
						}
						set member(name) [lindex $item $i]
						incr i
						if {$i < $iSize} {
							set member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(member_array_syntax) $perr(member_array_syntax)
						}
					}
					variable {
						set member(type) variable
						incr i
						if {$i == $iSize} {
							error $perr(member_variable_syntax) $perr(member_variable_syntax)
						}
						set member(name) [lindex $item $i]
						incr i
						if {$i < $iSize} {
							set member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(member_variable_syntax) $perr(member_variable_syntax)
						}
					}
					constructor {
						set member(type) proc
						set member(name) [namespace tail $classname]
						incr i
						if {$i < $iSize} {
							lappend member(args) [lindex $item $i]
							incr i
							if {$i == $iSize} {
								error $perr(constructor_syntax) $perr(constructor_syntax)
							}
							lappend member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(constructor_syntax) $perr(constructor_syntax)
						}
					}
					destructor {
						set member(type) proc
						set member(name) ~[namespace tail $classname]
						incr i
						if {$i < $iSize} {
							lappend member(args) {}
							lappend member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(destructor_syntax) $perr(destructor_syntax)
						}
					}
					proc {
						set member(type) proc
						incr i
						if {$i == $iSize} {
							error $perr(member_proc_syntax) $perr(member_proc_syntax)
						}
						set member(name) [lindex $item $i]
						incr i
						if {$i < $iSize} {
							lappend member(args) [lindex $item $i]
							incr i
							if {$i == $iSize} {
								error $perr(member_proc_syntax) $perr(member_proc_syntax)
							}
							lappend member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(member_proc_syntax) $perr(member_proc_syntax)
						}
					}
					object {
						set member(type) object
						incr i
						if {$i == $iSize} {
							error $perr(member_object_syntax1) $perr(member_object_syntax1)
						}
						set member(args) [lindex $item $i]
						incr i
						if {$i == $iSize} {
							error $perr(member_object_syntax1) $perr(member_object_syntax1)
						}
						set member(name) [lindex $item $i]
						incr i
					}
					reference {
						set member(type) reference
						incr i
						if {$i == $iSize} {
							error $perr(member_reference_syntax) $perr(member_reference_syntax)
						}
						set member(name) [lindex $item $i]
						incr i
						if {$i < $iSize} {
							set member(args) [lindex $item $i]
							incr i
						}
						if {$i < $iSize} {
							error $perr(member_reference_syntax) $perr(member_reference_syntax)
						}
					}
					default {
						set word [lindex $item $i]
						if {[regexp {[*]$} $word]} {
							set member(type) reference
							set member(args) [string range $word 0 end-1]
							incr i
							if {$i == $iSize} {
								error $perr(member_reference_syntax2) $perr(member_reference_syntax2)
							}
							set member(name) [lindex $item $i]
							incr i
							if {$i < $iSize} {
								error $perr(member_reference_syntax2) $perr(member_reference_syntax2)
							}
						} else {
							set member(type) object
							set member(args) $word
							incr i
							if {$i == $iSize} {
								error $perr(member_object_syntax2) $perr(member_object_syntax2)
							}
							set member(name) [lindex $item $i]
							incr i
							if {$i < $iSize} {
								error $perr(member_object_syntax2) $perr(member_object_syntax2)
							}
						}
						unset word
					}
				}
			}
			if {[info exists member(name)]} {
				if {[string first :: $member(name)] > -1} {
					error $perr(member_namespace_qualifiers) $perr(member_namespace_qualifiers)
				}
				set tablename [::cTCL::lib::ns2table $classname\::$member(name)]
				if {[info exists members_table($member(name))]} {
					if {[string match $members_table($member(name)) $tablename]} {
						error $perr(duplicate_members) $perr(duplicate_members)
					}
				}
				lappend members_local			  $member(name)
				set members_table($member(name))  $tablename
				set members_type($tablename)	  $member(type)
				set members_access($tablename)	  $member(access)
				set members_scope($tablename)	  $member(scope)
				if {[info exists member(args)]} {
					set members_args($tablename) $member(args)
				}
				if {[string match $member(type) proc]} {
					set isvirtual 0
					if {[info exists members_virtual($member(name))]} {
						if {[string compare $member(access) $members_access($members_virtual($member(name)))]} {
							error $perr(virtual_access) $perr(virtual_access)
						}
						set isvirtual 1
					} elseif {[info exists member(virtual)]} {
						set isvirtual 1
					}
					if {$isvirtual} {
						if {[string match $member(name) [namespace tail $classname]]} {
							error $perr(virtual_constructor) $perr(virtual_constructor)
						}
						if {[string compare $member(scope) object]} {
							error $perr(virtual_static) $perr(virtual_static)
						}
						set members_virtual($member(name)) $tablename
					}
					unset isvirtual
				} else {
					if {[info exists member(virtual)]} {
						error $perr(virtual_proc_only) $perr(virtual_proc_only)
					}
				}
			}
			unset member
		}
	}
	set tclassname [namespace tail $classname]
	set tablename [::cTCL::lib::ns2table $classname\::$tclassname]
	set defaultconstructor 0
	if {[info exists members_table($tclassname)]} {
		if {[string compare $members_table($tclassname) $tablename]} {
			set defaultconstructor 1
		}
	} else {
		set defaultconstructor 1
	}
	if {$defaultconstructor} {
		lappend members_local $tclassname
		set members_table($tclassname) $tablename
		set members_type($tablename) proc
		set members_access($tablename) private
		set members_scope($tablename) object
		set members_args($tablename) {{{obj {}}}}
		lappend members_args($tablename) {
			if {[regexp {^[:]{2}} $obj]} {
				if {[::cTCL::objects::is $obj]} {
					::cTCL::objects::copy [this] $obj
				} else {
					set errmsg {Unknown parameter to default constructor}
					error $errmsg $errmsg
				}
			}
		}
	} else {
		if {[string compare $members_type($tablename) proc]} {
			error $perr(constructor_name_reserved) $perr(constructor_name_reserved)
		}
		if {[string compare $members_scope($tablename) object]} {
			error $perr(constructor_static) $perr(constructor_static)
		}
	}
	set defaultdestructor 0
	set tablename [::cTCL::lib::ns2table $classname\::~$tclassname]
	if {[info exists members_table(~$tclassname)]} {
		if {[string compare $members_table(~$tclassname) $tablename]} {
			set defaultdestructor 1
		}
	} else {
		set defaultdestructor 1
	}
	if {$defaultdestructor} {
		lappend members_local ~$tclassname
		set members_table(~$tclassname) $tablename
		set members_type($tablename) proc
		set members_access($tablename) private
		set members_scope($tablename) object
		set members_args($tablename) {{} {}}
	} else {
		if {[string compare $members_type($tablename) proc]} {
			error $perr(destructor_name_reserved) $perr(destructor_name_reserved)
		}
		if {[string compare $members_scope($tablename) object]} {
			error $perr(destructor_static) $perr(destructor_static)
		}
	}
	foreach name $members_local {
			set tablename [::cTCL::lib::ns2table $classname\::$name]
			switch $members_type($tablename) {
				variable  {
					switch $members_scope($tablename) {
						object {
							if {[lsearch $object_variables $name] == -1} {
								lappend object_variables $name
							}
						}
						static {
							if {[lsearch $static_variables $name] == -1} {
								lappend static_variables $name
							}
							namespace inscope $classname variable $name
							if {[info exists members_args($tablename)]} {
								set $classname\::$name $members_args($tablename)
							}
						}
					}
				}
				array {
					switch $members_scope($tablename) {
						object {
							if {[lsearch $object_variables $name] == -1} {
								lappend object_variables $name
							}
						}
						static {
							if {[lsearch $static_variables $name] == -1} {
								lappend static_variables $name
							}
							namespace inscope $classname variable $name
							if {[info exists members_args($tablename)]} {
								array set $classname\::$name $members_args($tablename)
							}
						}
					}
				}
				proc {
					if {[info exists members_args($tablename)]} {
						set code [list proc $name]
						lappend code [lindex $members_args($tablename) 0]
						lappend code [lindex $members_args($tablename) 1]
						namespace inscope $classname $code
					}
				}
				class {
					set code [list class $name]
					foreach arg $members_args($tablename) {
						lappend code $arg
					}
					namespace inscope $classname $code
				}
				object {
					if {[string match $members_scope($tablename) static]} {
						namespace inscope $classname object $members_args($tablename) $name
					}
				}
				reference {
					if {[string match $members_scope($tablename) static]} {
						if {[info exists members_args($tablename)]} {
							namespace inscope $classname reference $name $members_args($tablename)
						}
					}
				}
			}
	}
	::cTCL::classes::interface $classname $classname
	lappend call_classes $classname
	interp alias {} $classname {} \
		::cTCL::objects::object $classname
	interp alias {} $classname\* {} \
		::cTCL::references::referencec $classname
#} errMSG]} {
#	 error $errMSG $errMSG
#}
	return $classname
}

proc ::cTCL::classes::inherit {targetns {sourcens {}}} {
	upvar 0 $targetns\::_cTCL_call_classes tcall_classes
	foreach do {members_table members_access members_type
				members_scope members_args members_virtual} {
		upvar 0 $targetns\::_cTCL_$do t$do
	}
	upvar 0 $targetns\::_cTCL_object_variables tobject_variables
	upvar 0 $targetns\::_cTCL_static_variables tstatic_variables
	if {! [llength $sourcens]} {
		upvar 0 $targetns\::_cTCL_parent_classes parent_classes
	} else {
		upvar 0 $sourcens\::_cTCL_parent_classes parent_classes
	}
	foreach pclass $parent_classes {
		if {[lsearch $tcall_classes $pclass] == -1} {
			::cTCL::classes::inherit $targetns $pclass
			lappend tcall_classes $pclass
			::cTCL::classes::interface $targetns $pclass
		}
		foreach do {members_table members_access members_type
					members_scope members_args members_virtual} {
			upvar 0 $pclass\::_cTCL_$do $do
		}
		foreach {tablename access} [array get members_access] {
			set tmembers_access($tablename) $members_access($tablename)
			set tmembers_type($tablename)	$members_type($tablename)
			set tmembers_scope($tablename)	$members_scope($tablename)
			if {[info exists members_args($tablename)]} {
				set tmembers_args($tablename) $members_args($tablename)
			}
		}
		foreach {name tablename} [array get members_table] {
			if {[string compare $members_access($tablename) private]} {
				set tmembers_table($name) $members_table($name)
			}
		}
		foreach {name tablename} [array get members_virtual] {
			set tmembers_virtual($name) $members_virtual($name)
		}
		upvar 0 $pclass\::_cTCL_object_variables object_variables
		upvar 0 $pclass\::_cTCL_static_variables static_variables
		foreach var $object_variables {
			if {[lsearch $tobject_variables $var] == -1} {
				lappend tobject_variables $var
			}
		}
		foreach var $static_variables {
			if {[lsearch $tstatic_variables $var] == -1} {
				lappend tstatic_variables $var
			}
		}
	}
}

# Produces interfaces to a class namespace
proc ::cTCL::classes::interface {eclassns aclassns} {
	foreach do {members_table members_access members_type
				members_scope members_args} {
		upvar 0 $aclassns\::_cTCL_$do $do
	}
	set targetns $eclassns\::[::cTCL::lib::ns2table $aclassns]
	namespace eval $targetns [list set _cTCL_call_class $aclassns]
	::cTCL::procs::copy $targetns\::_cTCL_procs static
	foreach {name tablename} [array get members_table] {
		set nsname [::cTCL::lib::table2ns $tablename]
		set nsfrom [namespace qualifiers $nsname]
		set do_member 0
		set usename $tablename
		if {[string match $eclassns $nsfrom]} {
			set do_member 1
			set usename $name
		} elseif {[string compare $members_access($tablename) private]} {
			set do_member 1
		}
		if {[string match $members_scope($tablename) object]} {
			set do_member 0
		}
		if {$do_member} {
			switch $members_type($tablename) {
				variable - array {
					namespace inscope $targetns upvar 0 $nsname $name
				}
				proc {
					set pcallns $nsfrom\::[::cTCL::lib::ns2table $nsfrom]
					interp alias {} $targetns\::$name {} $pcallns\::_cTCL_procs $name
				}
				class {
					interp alias {} $targetns\::$name {} $nsname
				}
				object - reference {
					::cTCL::references::alias $targetns\::$name $nsname
				}
			}
		}
	}
}

# Verifies that namespace is a cTCL class
proc ::cTCL::classes::is {namespace} {
	if {! [catch {
		set e [array exists $namespace\::_cTCL_members_table]
	}]} {
		if {$e} {
			return 1
		}
	}
	return 0
}

# Locate a class within a given the callns scope
proc ::cTCL::classes::locate {fromns class} {
	if {[string first :: $class] == 0} {
		return $class
	}
	if {[string match $fromns ::]} {
		set fromns {}
	}
	if {[::cTCL::objects::is $fromns]} {
		set oclassns [::cTCL::objects::typeof $fromns]
		upvar 0 $oclassns\::_cTCL_members_table members_table
		upvar 0 $oclassns\::_cTCL_members_type	members_type
		if {[info exists members_table($class)]} {
			if {[string match $members_type($members_table($class)) class]} {
				return [::cTCL::lib::table2ns $members_table($class)]
			}
		}
		lappend tries [namespace qualifiers $oclassns]::$class
	} else {
		set tries $fromns\::$class
		lappend tries [namespace qualifiers $fromns]::$class
	}
	lappend tries ::$class
	foreach try $tries {
		if {[::cTCL::classes::is $try]} {
			return $try
		}
	}
	set err "Could not locate class $class"
	error $err $err
}

# Return a namespace in table format
proc ::cTCL::lib::ns2table {ns} {
	return [string map [list :: !!] $ns]
}

# Join two words with ::
proc ::cTCL::lib::nsattach {parentns ns} {
	if {[string match :: $parentns]} {
		return [join [list $parentns $ns] {}]
	}
	return [join [list $parentns $ns] ::]
}

# Is the namespace an object, class or just namespace?
proc ::cTCL::lib::nstype {namespace} {
	if {[::cTCL::objects::is $namespace]} {
		return object
	}
	if {[::cTCL::classes::is $namespace]} {
		return class
	}
	return namespace
}

# Like namespace qualifiers but for table format
proc ::cTCL::lib::tablequal {tablens} {
	set i [string last !! $tablens]
	return [string range $tablens 0 [expr $i - 1]]
}

# Like namespace tail but for table format
proc ::cTCL::lib::tabletail {tablens} {
	set i [string last !! $tablens]
	return [string range $tablens [expr $i + 2] end]
}

# Return a tablename in namespace format
proc ::cTCL::lib::table2ns {tablens} {
	return [string map [list !! ::] $tablens]
}

# Unsets a variable and returns it's contents
proc ::cTCL::lib::unset_return {name} {
	upvar 1 $name var
	set data $var
	unset var
	return $data
}

# Returns a unique value.  Non sequential, but always unique
proc ::cTCL::lib::unique {} {
	return [clock seconds][expr abs([clock clicks])]
}

# Copy Object procedure
proc ::cTCL::objects::copy {targetns sourcens} {
	upvar 0 $targetns\::_cTCL_class tclassns
	upvar 0 $sourcens\::_cTCL_class sclassns
	upvar 0 $tclassns\::_cTCL_call_classes tcall_classes
	upvar 0 $sclassns\::_cTCL_members_type smembers_type

	foreach {tablename type} [array get smembers_type] {
		set nsname [::cTCL::lib::table2ns $tablename]
		set fromns [namespace qualifiers $nsname]
		if {[lsearch $tcall_classes $fromns] > -1} {
			set name [namespace tail $nsname]
			set tname $tablename
			set sname $tablename
			if {[string match $fromns $tclassns]} {
				set tname $name
			}
			if {[string match $fromns $sclassns]} {
				set sname $name
			}
			switch $type {
				variable {
					set $targetns\::$tname [set $sourcens\::$sname]
				}
				array {
					array set $targetns\::$tname [array get $sourcens\::$sname]
				}
				object {
					::cTCL::objects::copy $targetns\::$tname $sourcens\::$sname
				}
				reference {
					if {[::cTCL::references::hastype $sourcens\::$sname]} {
						::cTCL::references::type $targetns\::$tname [::cTCL::references::typeof $sourcens\::$sname]
					}
					if {[::cTCL::references::hasobject $sourcens\::$sname]} {
						::cTCL::references::object $targetns\::$tname [::cTCL::references::objectof $sourcens\::$sname]
						::cTCL::references::construct $targetns\::$tname
					}
				}
			}
		}
	}
}

# Object specific delete procedure
proc ::cTCL::objects::delete {objectns} {
	set classns [::cTCL::objects::typeof $objectns]
	upvar 0 $classns\::_cTCL_call_classes call_classes
	::cTCL::objects::deltree $objectns
	for {set c [expr [llength $call_classes] - 1]} {$c > -1} {incr c -1} {
		set call_class [lindex $call_classes $c]
		$objectns\::[::cTCL::lib::ns2table $call_class]::~[namespace tail $call_class]
	}
	foreach ref [::cTCL::objects::unreference $objectns] {
		::cTCL::references::demolish $ref
	}
	namespace delete $objectns
	return
}

# Checks child namespaces for objects and deletes them recursively
proc ::cTCL::objects::deltree {namespace} {
	set potentials [namespace children $namespace]
	foreach potential $potentials {
		deltree $potential
		if {[::cTCL::objects::is $potential]} {
			delete $potential
		}
	}
}

# object procedure - produce an object in a give namespace
proc ::cTCL::objects::object {classns objectns args} {
	set fromns [uplevel namespace current]
	if {! [regexp {^[:]{2}} $classns]} {
		set classns [::cTCL::classes::locate $fromns $classns]
	}
	if {! [regexp {^[:]{2}} $objectns]} {
		set objectns [::cTCL::lib::nsattach $fromns $objectns]
	}
	set code [list ::cTCL::objects::instanciate $classns $objectns]
	foreach arg $args {
		lappend code $arg
	}
#	 if {[catch {
		eval $code
#	 } errMSG]} {
#		 error $errMSG $errMSG
#	 }
	return [::cTCL::references::reference $objectns $classns $objectns]
}

# instanciate an object
proc ::cTCL::objects::instanciate {classns objectns args} {
	namespace eval $objectns [list set _cTCL_class $classns]
	upvar 0 $classns\::_cTCL_call_classes call_classes
	foreach class [lrange $call_classes 0 end-1] {
		::cTCL::objects::interface $class $objectns
		set targetns $objectns\::[::cTCL::lib::ns2table $class]
		::cTCL::references::object $targetns\::this $objectns
		::cTCL::references::type $targetns\::this $class
		::cTCL::references::construct $targetns\::this 1
		$targetns\::[namespace tail $class]
	}
	set class [lindex $call_classes end]
	::cTCL::objects::interface $class $objectns
	set targetns $objectns\::[::cTCL::lib::ns2table $class]
	::cTCL::references::object $targetns\::this $objectns
	::cTCL::references::type $targetns\::this $class
	::cTCL::references::construct $targetns\::this 1
	set const $targetns\::[namespace tail $class]
	foreach arg $args {
		lappend const $arg
	}
	uplevel 2 $const
	return $objectns
}

# Create object interface namespaces
proc ::cTCL::objects::interface {aclassns eobjectns} {
	foreach do {members_table members_access members_type
				members_scope members_args members_virtual} {
		upvar 0 $aclassns\::_cTCL_$do $do
	}
	upvar 0 $eobjectns\::_cTCL_class eobjclass
	set targetns $eobjectns\::[::cTCL::lib::ns2table $aclassns]
	namespace eval $targetns [list set _cTCL_call_class $aclassns]
	::cTCL::procs::copy $targetns\::_cTCL_procs object
	foreach {name tablename} [array get members_table] {
		set nsname [::cTCL::lib::table2ns $tablename]
		set nsfrom [namespace qualifiers $nsname]
		set do_member 0
		set usename $tablename
		set do_default 0
		if {[string match $aclassns $nsfrom]} {
			set do_member 1
			set do_default 1
		} elseif {[string compare $members_access($tablename) private]} {
			set do_member 1
		}
		if {[string match $eobjclass $nsfrom]} {
			set usename $name
		}
		if {$do_member} {
			switch $members_type($tablename) {
				variable {
					switch $members_scope($tablename) {
						static {
							namespace inscope $targetns upvar 0 $nsname $name
						}
						object {
							namespace inscope $targetns upvar 0 $eobjectns\::$usename $name
							if {$do_default && [info exists members_args($tablename)]} {
								set $eobjectns\::$usename $members_args($tablename)
							}
						}
					}
				}
				array {
					switch $members_scope($tablename) {
						static {
							namespace inscope $targetns upvar 0 $nsname $name
						}
						object {
							namespace inscope $targetns upvar 0 $eobjectns\::$usename $name
							if {$do_default && [info exists members_args($tablename)]} {
								array set $eobjectns\::$usename $members_args($tablename)
							}
						}
					}
				}
				proc {
					switch $members_scope($tablename) {
						static {
							set pcallns $nsfrom\::[::cTCL::lib::ns2table $nsfrom]
							interp alias {} $targetns\::$name {} $pcallns\::_cTCL_procs $name
						}
						object {
							if {[info exists members_virtual($name)]} {
								set vtablename [set $eobjclass\::_cTCL_members_virtual($name)]
								set vnsfrom [::cTCL::lib::tablequal $vtablename]
								set pcallns $eobjectns\::[::cTCL::lib::ns2table $vnsfrom]
							} else {
								set pcallns $eobjectns\::[::cTCL::lib::ns2table $nsfrom]
							}
							interp alias {} $targetns\::$name {} $pcallns\::_cTCL_procs $name
						}
					}
				}
				object {
					switch $members_scope($tablename) {
						static {
							::cTCL::references::alias $targetns\::$name $eobjclass\::$usename
							::cTCL::references::construct $targetns\::$name
						}
						object {
							if {$do_default} {
								namespace inscope $eobjectns object $members_args($tablename) $usename
							}
							::cTCL::references::alias $targetns\::$name $eobjectns\::$usename
							::cTCL::references::construct $targetns\::$name
						}
					}
				}
				reference {
					switch $members_scope($tablename) {
						static {
							::cTCL::references::alias $targetns\::$name $eobjclass\::$usename
						}
						object {
							if {$do_default && [info exists members_args($tablename)]} {
								namespace inscope $eobjectns reference $usename $members_args($tablename)
							}
							::cTCL::references::alias $targetns\::$name $eobjectns\::$usename
						}
					}
				}
				class {
					if {$do_default} {
						interp alias {} $eobjectns\::$usename {} $nsname
					}
					interp alias {} $targetns\::$name {} $nsname
				}
			}
		}
	}
}

# Is namespace a cTCL object?
proc ::cTCL::objects::is {namespace} {
	return [info exists $namespace\::_cTCL_class]
}

# Mark an object as referenced
proc ::cTCL::objects::reference {object root} {
	variable object_references

	if {! [info exists object_references($object)]} {
		return [set object_references($object) $root]
	}
	if {[lsearch $object_references($object) $root] == -1} {
		return [lappend object_references($object) $root]
	}
	return $root
}

# Return all references to a given object
proc ::cTCL::objects::references {object} {
	variable object_references

	if {[info exists object_references($object)]} {
		return [llength $object_references($object)]
	}
	return 0
}

# Unmark a reference from object or
# Unmark all references from object and return them
proc ::cTCL::objects::unreference {object {root {}}} {
	variable object_references

	if {[llength $root]} {
		set i [lsearch $object_references($object) $root]
		if {$i > -1} {
			set object_references($object) [lreplace $object_references($object) $i $i]
		}
		return $root
	}
	return [::cTCL::lib::unset_return object_references($object)]
}

# What class is the object?
proc ::cTCL::objects::typeof {objectns} {
	return [set $objectns\::_cTCL_class]
}

# copy a procedure - generic
proc ::cTCL::procs::copy {target source} {
	set code [list proc $target]
	set arguments {}
	foreach arg [info args $source] {
		if {[info default $source $arg default]} {
			lappend arg $default
		}
		lappend arguments $arg
	}
	lappend code $arguments
	lappend code [info body $source]
	eval $code
	return $target
}

# Member static procedure code
proc ::cTCL::procs::static {_cTCL_proc args} {
	set _cTCL_classns [set [namespace current]::_cTCL_call_class]
	foreach _cTCL_name [set $_cTCL_classns\::_cTCL_static_variables] {
		variable $_cTCL_name
	}
	if {[info exists _cTCL_name]} {unset _cTCL_name}
	set _cTCL_argument_counter 0
	set argument {}
	foreach argument [info args [namespace parent]::$_cTCL_proc] {
		if {[string match $argument args]} {
			set nargs [lrange $args $_cTCL_argument_counter end]
			set _cTCL_argument_counter [llength $args]
		} elseif {$_cTCL_argument_counter < [llength $args]} {
			set $argument [lindex $args $_cTCL_argument_counter]
			incr _cTCL_argument_counter
		} else {
			if {! [info default [namespace parent]::$_cTCL_proc $argument $argument]} {
				unset $argument
				set err "Not enough arguments"
				error $err $err
			}
		}
	}
	if {$_cTCL_argument_counter < [llength $args]} {
		if {[string match $argument args]} {
			set nargs [lrange $args $_cTCL_argument_counter end]
		} else {
			set err "Too many arguments"
			error $err $err
		}
	}
	if {[string match $argument args]} {
		set args $nargs
		unset nargs
	} else {
		unset args
	}
	unset _cTCL_argument_counter argument
	return [eval [info body [::cTCL::lib::unset_return _cTCL_classns]::[::cTCL::lib::unset_return _cTCL_proc]]]
}

# Member procedure code
proc ::cTCL::procs::object {_cTCL_proc args} {
	set _cTCL_classns [set [namespace current]::_cTCL_call_class]
	foreach _cTCL_name [set $_cTCL_classns\::_cTCL_static_variables] {
		variable $_cTCL_name
	}
	foreach _cTCL_name [set $_cTCL_classns\::_cTCL_object_variables] {
		variable $_cTCL_name
	}
	if {[info exists _cTCL_name]} {unset _cTCL_name}
	set _cTCL_argument_counter 0
	set argument {}
	foreach argument [info args $_cTCL_classns\::$_cTCL_proc] {
		if {[string match $argument args]} {
			set nargs [lrange $args $_cTCL_argument_counter end]
			set _cTCL_argument_counter [llength $args]
		} elseif {$_cTCL_argument_counter < [llength $args]} {
			set $argument [lindex $args $_cTCL_argument_counter]
			incr _cTCL_argument_counter
		} else {
			if {! [info default $_cTCL_classns\::$_cTCL_proc $argument $argument]} {
				unset $argument
				set err "Not enough arguments"
				error $err $err
			}
		}
	}
	if {$_cTCL_argument_counter < [llength $args]} {
		if {[string match $argument args]} {
			set nargs [lrange $args $_cTCL_argument_counter end]
		} else {
			set err "Too many arguments"
			error $err $err
		}
	}
	if {[string match $argument args]} {
		set args $nargs
		unset nargs
	} else {
		unset args
	}
	unset _cTCL_argument_counter argument
	return [eval [info body [::cTCL::lib::unset_return _cTCL_classns]\::[::cTCL::lib::unset_return _cTCL_proc]]]
}

# Reference alias to a reference
proc ::cTCL::references::alias {alias {root {}}} {
	variable alias_reference
	variable reference_aliases

	if {[llength $root]} {
		if {[hastype $root]} {
			type $alias [typeof $root]
		}
		if {[hasobject $root]} {
			object $alias [objectof $root]
		}
		lappend reference_aliases($root) $alias
		return [set alias_reference($alias) $root]
	}
	set root $alias_reference($alias)
	set i [lsearch $reference_aliases($root) $alias]
	if {$i > -1} {
		set reference_aliases($root) [lreplace $reference_aliases($root) $i $i]
	}
	unset alias_reference($alias)
}

# Return all aliases of reference
proc ::cTCL::references::aliasesof {root} {
	variable reference_aliases

	return $reference_aliases($root)
}

# Construct a reference's tree
# Do not run this against an alias
proc ::cTCL::references::construct {root {allowprivate 0}} {
	variable build_info
	set build_info($root) $allowprivate

	set classns [typeof $root]
	set objectns [objectof $root]
	set targetns $objectns\::[::cTCL::lib::ns2table $classns]

	upvar 0 $classns\::_cTCL_members_table	members_table
	upvar 0 $classns\::_cTCL_members_type	members_type
	upvar 0 $classns\::_cTCL_members_scope	members_scope
	upvar 0 $classns\::_cTCL_members_access members_access

	interp alias {} $root {} list $objectns

	foreach {name tablename} [array get members_table] {
		if {[string match $members_access($tablename) public] || $allowprivate} {
			set refname $root.$name
			set memname $targetns\::$name

			switch $members_type($tablename) {
				variable {
					interp alias {} $refname {} set $memname
				}
				array {
					interp alias {} $refname {} ::cTCL::references::arrays::handler $memname
				}
				proc {
					interp alias {} $refname {} $targetns\::$name
				}
				object -
				reference {
					if {[::cTCL::references::isalias $memname]} {
						::cTCL::references::alias $refname [::cTCL::references::dealias $memname]
					} else {
						::cTCL::references::alias $refname $memname
					}
					if {[::cTCL::references::isbuilt $memname]} {
						::cTCL::references::construct $refname
					}
				}
				class {
					interp alias {} $refname {} [::cTCL::lib::table2ns $tablename]
				}
			}
		}
	}
	if {[hasaliases $root]} {
		foreach alias [aliasesof $root] {
			construct $alias
		}
	}
	return
}

# Return the prime reference
proc ::cTCL::references::dealias {alias} {
	variable alias_reference

	return $alias_reference($alias)
}

# Reference speicific delete
proc ::cTCL::references::delete {root} {
	if {[::cTCL::references::isalias $root]} {
		set realref [::cTCL::references::dealias $root]
	} else {
		set realref $root
	}
	::cTCL::references::demolish $realref
	set objectns [::cTCL::references::objectof $realref]
	::cTCL::references::object $realref
	::cTCL::objects::unreference $objectns $realref
	if {! [::cTCL::objects::references $objectns]} {
		::cTCL::objects::delete $objectns
	}
	return
}

# Demolish a reference's tree
# Do not run this against an alias
proc ::cTCL::references::demolish {root} {
	variable build_info

	if {[llength [interp alias {} $root]]} {
		interp alias {} $root {}
	}

	set classns [typeof $root]
	set objectns [typeof $root]
	set targetns $objectns\::[::cTCL::lib::ns2table $classns]

	upvar 0 $classns\::_cTCL_members_table members_table
	upvar 0 $classns\::_cTCL_members_type members_type
	upvar 0 $classns\::_cTCL_members_access members_access

	foreach {name tablename} [array get members_table] {
		if {[string match $members_access($tablename) public] || [lindex $build_info($root) 0]} {
			set refname $root.$name
			switch $members_type($tablename) {
				variable -
				array -
				class -
				proc {
					if {[llength [interp alias {} $refname]]} {
						interp alias {} $refname {}
					}
				}
				reference -
				object {
					if {[llength [interp alias {} $refname]]} {
						::cTCL::references::demolish $refname
					}
				}
			}
		}
	}
	unset build_info($root)
	if {[hasaliases $root]} {
		foreach alias [aliasesof $root] {
			demolish $alias
		}
	}
}

# Do we have a reference registered by this name?
proc ::cTCL::references::is {refname} {
	 return [expr [::cTCL::references::hastype $refname] || \
				  [::cTCL::references::isalias $refname]]
}

# reference name ?class? ?object?
proc ::cTCL::references::reference {refname args} {
	set nsfrom [uplevel namespace current]
	if {[regexp {^[:]{2}} $refname]} {
		set refnamefull $refname
	} else {
		set refnamefull [::cTCL::lib::nsattach $nsfrom $refname]
	}
	if {[::cTCL::references::isalias $refnamefull]} {
		set refnamefull [::cTCL::references::dealias $refnamefull]
	}
	switch [llength $args] {
		1 {
			set checkns [lindex $args 0]
			if {! [regexp {^[:]{2}} $checkns]} {
				set checkns [::cTCL::lib::nsattach $nsfrom [lindex $args 0]]
			}
			if {[::cTCL::objects::is $checkns]} {
				set object $checkns
			} else {
				set class [::cTCL::classes::locate $nsfrom [lindex $args 0]]
			}
		}
		2 {
			set class [::cTCL::classes::locate $nsfrom [lindex $args 0]]
			set object [lindex $args 1]
			if {! [regexp {^[:]{2}} $object]} {
				set object [::cTCL::lib::nsattach $nsfrom $object]
			}
		}
	}
	if {[info exists class]} {
		switch -regexp -- $class {
			{^[:]{2}} {
				set classns $class
			}
			default {
				set classns [::cTCL::classes::locate $nsfrom $class]
			}
		}
	}
	if {[info exists object]} {
		switch -regexp -- $object {
			{^[:]{2}} {
				set objectns $object
			}
			default {
				set objectns [::cTCL::lib::nsattach $nsfrom $object]
			}
		}
		if {[::cTCL::references::isbuilt $refnamefull]} {
			::cTCL::objects::unreference [::cTCL::references::objectof $refnamefull] $refnamefull
			::cTCL::references::demolish $refnamefull
		}
	}
	if {[info exists classns]} {
		::cTCL::references::type $refnamefull $classns
	}
	if {[info exists objectns]} {
		if {! [::cTCL::references::hastype $refnamefull]} {
			::cTCL::references::type $refnamefull [::cTCL::objects::typeof $objectns]
		}
		::cTCL::references::object $refnamefull $objectns
		::cTCL::references::construct $refnamefull
		::cTCL::objects::reference $objectns $refnamefull
	}
	return $refname
}

# Called by classname* syntax
proc ::cTCL::references::referencec {class refname {object {}}} {
	set cmd [list reference $refname $class]
	if {[llength $object]} {
		lappend cmd $object
	}
	uplevel $cmd
}

# Is reference aliased?
proc ::cTCL::references::hasaliases {root} {
	variable reference_aliases

	if {[info exists reference_aliases($root)]} {
		if {[llength $reference_aliases($root)]} {
			return 1
		}
	}
	return 0
}

# Does reference point to an object?
proc ::cTCL::references::hasobject {root} {
	variable reference_object
	return [info exists reference_object($root)]
}

# Does reference have a class type?
proc ::cTCL::references::hastype {root} {
	variable reference_type
	return [info exists reference_type($root)]
}

# Is reference an alias?
proc ::cTCL::references::isalias {root} {
	variable alias_reference
	return [info exists alias_reference($root)]
}

# Has reference been constructed?
proc ::cTCL::references::isbuilt {root} {
	variable build_info
	return [info exists build_info($root)]
}

# Reference an object
proc ::cTCL::references::object {root {object {}}} {
	variable reference_object
	if {[hasaliases $root]} {
		foreach alias [aliasesof $root] {
			object $alias $object
		}
	}
	if {[llength $object]} {
		return [set reference_object($root) $object]
	}
	unset reference_object($root)
}

# What object does it reference?
proc ::cTCL::references::objectof {root} {
	variable reference_object
	return $reference_object($root)
}

# Set/unset class type for reference
proc ::cTCL::references::type {root {type {}}} {
	variable reference_type
	if {[hasaliases $root]} {
		foreach alias [aliasesof $root] {
			type $alias $type
		}
	}
	if {[llength $type]} {
		return [set reference_type($root) $type]
	}
	unset reference_type($root)
}

# Class type of reference
proc ::cTCL::references::typeof {root} {
	variable reference_type
	return $reference_type($root)
}

# Array handler for referenced arrays
proc ::cTCL::references::arrays::handler {name action args} {
	set code [list array $action $name]
	foreach arg $args {
		lappend code $arg
	}
	return [uplevel $code]
}

namespace eval :: {
	namespace import \
		::cTCL::new \
		::cTCL::delete \
		::cTCL::classes::class \
		::cTCL::objects::copy \
		::cTCL::objects::object \
		::cTCL::references::reference
}

