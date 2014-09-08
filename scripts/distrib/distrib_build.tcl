#!/home/ray/Work/cheshire/bin/ztcl
# This script builds a database of distributed data by
# using the Explain database to find the databases on a server
# and then using scan to read the contents of the indexes for each
# database and store the results as "virtual collections"
#
set singleDB {}

if {$argc < 4} {
    puts "Usage: distrib_build HOST PORT OUTPUT_DIRECTORY LOGFILE <DATABASENAME>"
    exit
}

if {$argc == 5} {
    set singleDB [lindex $argv end]
    set LOGFILEname [lindex $argv [expr $argc - 2]]
} else {
    set LOGFILEname [lindex $argv end]
}

puts "LOGFILE is $LOGFILEname"
set LOGFILE [open $LOGFILEname w]

source zexplain.tcl

proc build_virtual_db {host port outdir} {
    global DATABASES
    global INDEXES
    global COMBOS
    global URI_HOST
    global URI_PORT
    global LOGFILE
    global singleDB
    global preferred_language


    set URI_HOST $host
    set URI_PORT $port

    puts "Host $URI_HOST Port $URI_PORT"
    puts $LOGFILE "Host $URI_HOST Port $URI_PORT"

    puts "CONNECTING"
    
    set connect_result [zselect xhost $URI_HOST IR-Explain-1 $URI_PORT]
    
    if {[lindex $connect_result 0] < 0} {
	puts "Connection failed: $connect_result"
    }
    
    set preferred_language eng
    set DATABASES {}
    
    zset recsyntax explain
    zset attributes explain

    puts "SEARCHING FOR DATABASEINFO"
    search_explain_type databaseinfo

    puts "SEARCHING FOR attributedetail"
    search_explain_type attributedetails
    # puts "DUMPING COMBINATIONS::::::::"
    # parray COMBOS

    set dbcount 0
    set TESTDATA(0) 1
    #puts "Starting each database..."
    zclose 

    foreach db $DATABASES {
	incr dbcount

	set dbname [lindex $db 0]

	# if looking for a single database... 
	if {$singleDB != {} && $singleDB != $dbname} {
	    continue
	}
	set recs [lindex $db 1]
	set outname "$outdir/$dbname.xml"
	set outfile [open $outname w]
	set db_start_time [clock seconds]
	set timestring [clock format $db_start_time -format "%H %M %S"]
	puts "DB $dbname: Start time $timestring"
	puts $LOGFILE "DB $dbname: Start time $timestring"

	zclose 

	set connect_result [zselect xhost $URI_HOST $dbname $URI_PORT]

	if {[lindex $connect_result 0] < 0} {
	    puts "Connection failed: $connect_result"
	    continue
	}

	puts -nonewline "$dbcount: $dbname has $recs records"

	puts -nonewline $LOGFILE "$dbcount: $dbname has $recs records"

	set attributeset [lindex $INDEXES($dbname) 0]
	puts " AttributeSet $attributeset"
	puts $LOGFILE " AttributeSet $attributeset"

	puts $outfile "<CDB>"
	puts $outfile "<DATABASE>$dbname</DATABASE>"
	puts $outfile "<HOSTNAME>$host</HOSTNAME>"
	puts $outfile "<PORT>$port</PORT>"
	puts $outfile "<RECORDCOUNT>$recs</RECORDCOUNT>"
	puts $outfile "<INDEXES>"

	# puts "INDEXES ARE $INDEXES($dbname)"

	set ind [lrange $INDEXES($dbname) 1 end]
	unset TESTDATA
	set indexnum 0
	set testindex 1

	zset database $dbname
	zset attributes $attributeset
	zset recsyntax SGML

	foreach pair $ind {
	    incr indexnum
	    puts "     Scanning $pair"
	    puts $LOGFILE "     Scanning $pair"
	    set indexname [lindex $pair 0]
	    set usevalue [lindex $pair 1]

	    if {$indexname == "" && $usevalue != ""} {
		set indexname $usevalue
	    }
	    
	    set err [catch {set testscan [zscan $indexname 0 1 200 0]} errmsg]
	    if {$err} {
		# Check attribute combinations...
		puts "Error in scanning: Different attribute combo???"
		puts $LOGFILE "Error in scanning: Different attribute combo???"
		foreach combo $COMBOS($dbname) {
		    if {[lindex $combo 0] == "1=$usevalue"} {
			regsub -all { 5=[0-9]*} $combo "" combo
			regsub -all { } $combo "," combo
			puts "COMBO $combo matches"
			puts $LOGFILE "COMBO $combo matches"
			set indexname "$indexname\[$combo\]"
			set err [catch {set testscan [zscan $indexname 0 1 200 0]} errmsg]
			if {$err} {
			    continue
			} else {
			    #puts "TESTSCAN FOR RETRY IS $testscan"
			    set err 0
			    break
			}
			
		    }
		}  
		# if err is not 0, there was no match...
		if {$err} {
		    puts "Error in scanning: $errmsg -- NO ENTRIES??"
		    puts $LOGFILE "Error in scanning: $errmsg -- NO ENTRIES??"
		    puts $outfile "<INDEX NAME=\"$indexname\" ATTRIBUTESET=\"$attributeset\" USE=\"$usevalue\">"
		    puts $outfile "</INDEX>"
		    continue
		}
	    }

	    if {$indexnum == 1} {
		set TESTDATA($testindex) [lrange $testscan 1 end]
		set dupnames($testindex) "{$indexname $usevalue}"
		incr testindex
	    } else {
		set dup 0
		for {set i 1} {$i < $testindex && $dup == 0} {incr i} {
		    if {[string compare $TESTDATA($i) [lrange $testscan 1 end]] == 0} {
			set dup $i
		    }
		}
		if {$dup} {
		    puts "Duplicate index, skipping..."
		    puts $LOGFILE "Duplicate index, skipping..."
		    lappend dupnames($dup) "$indexname $usevalue"
		    
		    continue
		} else {
		    set TESTDATA($testindex) [lrange $testscan 1 end]
		    set dupnames($testindex) "{$indexname $usevalue}"
		    incr testindex
		}    
	    }

	    puts $outfile "<INDEX NAME=\"$indexname\" ATTRIBUTESET=\"$attributeset\" USE=\"$usevalue\">"

	    set moredata 1
	    set lastterm 0
	    set firstpass 1

	    while {$moredata} {
		set err [catch {set scandata [zscan $indexname $lastterm 0 5000 0]} errmsg]
		if {$err} {
		    puts "Error in scanning: $errmsg"
		    exit
		}
		if {[lindex [lindex [lindex $scandata 0] 1] 1] == 5} {
		    set moredata 0
		}
		set lastterm [lindex [lindex $scandata end] 0]
		if {$lastterm == ""} {
		    puts "Error in scanning or parsing"
		    puts "scandata is $scandata"
		    set moredata 0
		} else {
		    puts "Lastterm is $lastterm"
		    puts $LOGFILE "Lastterm is $lastterm"
		    if {$firstpass} {
			puts $outfile [lrange $scandata 1 end]
			set firstpass 0
		    } else {
			# skip the term searched
			puts $outfile [lrange $scandata 2 end]
		    }
		}
	    }


	    unset scandata
	    unset errmsg
	    puts $outfile "</INDEX>"
	}
	
	puts $outfile "</INDEXES>"
	puts $outfile "<ALIASES>"
	for {set i 1} {$i < $testindex} {incr i} {
	    
	    if {[llength $dupnames($i)] > 1} {
		puts -nonewline $outfile "<ALIAS INDEX=\""
		if {[llength [lindex $dupnames($i) 0]] == 2} {
		    puts -nonewline $outfile [lindex [lindex $dupnames($i) 0] 0]
		} else {
		    puts -nonewline $outfile [lrange [lindex $dupnames($i) 0] 0 [expr [llength [lindex $dupnames($i) 0]] - 1]]
		}
		puts -nonewline $outfile "\" USE=\""
		puts -nonewline $outfile [lindex [lindex $dupnames($i) 0] end]
		puts $outfile "\">"

		foreach dn [lrange $dupnames($i) 1 end] {
		    if {[llength $dn] == 2} {
			puts $outfile "<ALTNAME USE=\"[lindex $dn end]\">[lindex $dn 0]</ALTNAME>"
		    } else {
			puts $outfile "<ALTNAME USE=\"[lindex $dn end]\">[lrange $dn 0 [expr [llength $dn]-1]]</ALTNAME>"
		    }
		}
		puts $outfile "</ALIAS>"
	    }
	}
	puts $outfile "</ALIASES>"
	puts $outfile "<ATTRIBCOMBO DEFAULTATTRSET=\"$COMBOS($dbname..DEFAULTATTR)\">"
	
	foreach combo $COMBOS($dbname) {
	    regsub -all { } $combo "," combo
	    puts $outfile "<COMBO>$combo</COMBO>"
	} 
	puts $outfile "</ATTRIBCOMBO>"
	puts $outfile "</CDB>"
	unset dupnames
	close $outfile
	set db_end_time [clock seconds]
	puts "Elapsed time for $dbname [expr $db_end_time - $db_start_time]"
	puts $LOGFILE "Elapsed time for $dbname [expr $db_end_time - $db_start_time]"


    }
}

set start_time [clock seconds]
set timestring [clock format $start_time -format "%H %M %S"]
puts "Start time $timestring"
puts $LOGFILE "Start time $timestring"
build_virtual_db  [lindex $argv 0] [lindex $argv 1] [lindex $argv 2]

set end_time [clock seconds]
puts "Elapsed time [expr $end_time - $start_time]"
puts $LOGFILE "Elapsed time [expr $end_time - $start_time]"
close $LOGFILE
exit


