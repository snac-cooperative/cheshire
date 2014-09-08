class ResultSet Zoom  {
    private:
    variable connectionHandle {}
    variable currentRecordNumber {0}
    variable size {0}
    variable name {Default}
    variable options {elementSet preferredRecordSyntax }
    variable elementSet {-1}
    variable preferredRecordSyntax {-1}

    proc ResultSet {conn hits rsname} {
	set connectionHandle $conn
	set size $hits
	set name $rsname
    }
    proc ~ResultSet {} {
	# puts "Destroying [namespace parent]"
    }

    public:

    # Public API

    proc setOption {args} {
	source "setter.tcl"
    }
    proc getSize {} {
	return $size
    }
    proc getName {} {
	return $name
    }
    proc getRecord { first } {
	# Need to retrieve the record from the server
	set reclist [$connectionHandle.present [namespace parent] $first]
	set status [lindex $reclist 0]
	set rectext [lindex $reclist 1]

	# Need to return a record object with record
	incr currentRecordNumber
	object Record rec$currentRecordNumber [namespace parent] $status $rectext
	return [rec$currentRecordNumber]
    }

    proc destroy {} {
	delete [namespace parent]
    }
}


