
class Connection Zoom  {
    private:

    static variable options {"database" "smallSetUpperBound" "largeSetLowerBound" "mediumSetPresentNumber" "smallSetElementSetNames" "mediumSetElementSetNames" "preferredRecordSyntax" "elementSetNames"}
    variable sessionHandle {}
    variable currentResultSetNumber {0}

# local copies of zset variables
    variable database "default"
    variable smallSetUpperBound {0}
    variable largeSetLowerBound {2}
    variable mediumSetPresentNumber {0}
    variable smallSetElementSetNames {}
    variable mediumSetElementSetNames {}
    variable preferredRecordSyntax {MARC}
    variable elementSetNames {F}

    proc Connection {{host localhost} {port 210} {db default}} {
	set cmd "zselect w $host $db $port"
	set err [catch {eval $cmd} cresults]
	set sessionHandle [lindex $cresults 0]
    }
    proc ~Connection {} {
	# This will automatically delete resultsets etc as well, via subnamespaces
	# puts "Destroying [namespace parent]"
	selectSession
	zclose
    }

    proc selectSession {} {
	zset session $sessionHandle
    }


    public:

    # Public API

    proc setOption {args} {
	# Override to zset variables
	
	set varname [lindex $args 0]
	
	if { [string range $varname 0 1] == "X-" } {
	    if { [lsearch [array names xoptions]] == -1 && [llength $args] == 1 } {
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
		zset $varname $new
		return $old
	    } else {
		return [eval "set $varname"]
	    }
	} else {
	    return -1
	}
    }

    proc search {queryObj} {
	selectSession
	set query "[$queryObj.getQuery]"
	if { [set rsn [$queryObj.setOption "resultsetname"]] != -1 } {
	    append query " resultsetid $rsn"
	}

	set err [catch {eval $query} qresults]

	set qresults [lindex $qresults 0]
	incr currentResultSetNumber
	object ResultSet res$currentResultSetNumber [namespace parent] [lindex [lindex $qresults 2] 1] [lindex [lindex $qresults 4] 1]
	return [res$currentResultSetNumber]
    }

    # Not Public ZOOM, but required for selectSession and option inheritance
    proc present {rsetObj first} {
	selectSession
	if {[set esn [$rsetObj.setOption "elementSet"]] == -1} {
	    set esn $elementSetNames
	}
	zset elementsetnames $esn

	if { [set rsn [$rsetObj.setOption "preferredRecordSyntax"]] == -1} {
	    set rsn $preferredRecordSyntax
	}
	zset preferredRecordSyntax $rsn

	set pq "zdisplay [$rsetObj.getName] 1 [expr $first + 1]"
	set err [catch {eval $pq} recs]

	return $recs
    }

}

