
class Query {
    private:
    static variable options {"query" "type" "resultsetname"}
    static variable typeEnum {"ztcl"}
    variable query {""}
    variable type {"ztcl"}
    variable resultsetname {""}

    proc Query {typenum querystr} {
	if {[llength $typeEnum] > [expr $typenum + 1]} {
	    error "Undefined Query Type."
	}
	set type [lindex $typeEnum $typenum]
	set query $querystr
	set resultsetname Default
    }

    
    public:
    proc setOption {args} {
	source "setter.tcl"
    }

    proc getQuery {} {
	return $query
    }
}

