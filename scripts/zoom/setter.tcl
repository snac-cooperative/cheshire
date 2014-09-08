
# Global getter/setter code for all objects
set varname [lindex $args 0]

if { [string range $varname 0 1] == "X-" } {
    if { [lsearch [array names xoptions] $varname] == -1 && [llength $args] == 1 } {
	return -1
    } elseif  { [llength $args] > 1} {
	set err [catch {set old $xoptions($varname)} old]
	if {$err} {
	    set old -1
	}
	set new [lindex $args 1]
	set xoptions($varname) $new
	return $old
    } else {
	return $xoptions($varname)
    }

} elseif { [ lsearch $options $varname] != -1 } {
    if { [llength $args] > 1} {
	set old [eval "set $varname"]
	set new [lindex $args 1]
	set $varname $new
	return $old
    } else {
	return [eval "set $varname"]
    }
} else {
    return -1
}
