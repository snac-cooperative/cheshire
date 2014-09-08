
class Record {
    private:
    
    static variable options {"elementSet"}
    array xoptions {}
    variable resultSetHandle {}
    variable text {}
    variable syntax {}

    proc Record {handle recsyntax rtext} {
	set resultSetHandle $handle
	set text $rtext
	set syntax $recsyntax
	set xoptions(X-ElementSetName) [$handle.setOption "elementSet"]
    }
    proc ~Record {} {
	# puts "Destroying [namespace parent]"
    }


    public:

    # Public API
    proc setOption {args} {
	source "setter.tcl"
    }

    proc getSyntax {} {
	return $syntax
    }
    proc getRaw {} {
	return $text
    }
    proc getRendered {} {
	return ""
    }
    proc getNumberOfFields {} {
	return 0
    }
    proc getField { field } {
	return -1
    }
    
}

