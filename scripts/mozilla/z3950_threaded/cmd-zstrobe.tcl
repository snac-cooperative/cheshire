
puts stderr "Loaded: zstrobe.tcl"

proc do_strobe {host port database} {

    source use_info.tcl
    
    # Need to read in sets from a store.
    set searchsets {1 2 3 }

    set serverstr "$host:$port/$database"

	set foundnums {}
	set foundidxs {}
	set length [llength $bib1nums]
	
	zselect probe $host $database $port
	
	for {set idx 0} {$idx < $length} { incr idx} {
	    set number [lindex $bib1nums $idx]
	    
	    set dosearch 0
	    foreach setidx $searchsets {
		if { $number > [lindex $borders [expr $setidx -1]] && $number <= [lindex $borders $setidx] } {
		    set dosearch 1
		}
	    }
	    if { $dosearch == 1} {
		if { [lsearch $foundnums $number] == -1 } {
		    
		    if {[lsearch $numeric_fields $number] == -1} {
			set term "anything"
		    } else {
			set term 100
		    }

		    set err [catch {zfind $number $term} qresults];
		    set status [lindex [lindex [lindex $qresults 0] 1] 1]
		    if {$status == 1 || $status == 255}  {
			puts "Found: $number"
			lappend foundnums $number;
			lappend foundidxs $idx;
		    }
		}
	    }
	}
	
}
