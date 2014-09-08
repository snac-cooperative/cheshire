#!/home/ray/Work/cheshire/bin/ztcl
# This script builds indexes for a database of distributed data by
# creating a BATCHTEMP file and using the batch_load program to
# load it. This is possible because the data in the "records" comes
# from a scan and therefore already has the frequency information
# for the terms. The continuation file is used as input to get the
# file names of the data, and the ID number. This assumes that the
# cont file is the correct one and that there is a single record per
# file -- which is the cast if the distrib_build script was used to
# create the files.
#
set singleDB {}

if {$argc < 3} {
    puts "Usage: distrib_index CONT_FILE TEMPINDEXFILE USEVAL"
    puts "      CONT_FILE is the .cont file for the records and "
    puts "      TEMPINDEXFILE is the pathname for the output _BATCHTEMP file"
    puts "         for the index being created."
    puts "      USEVAL is the USE value to match for the index data in the records"
    puts " Use batch_load to actually load the files created by this program"
    exit
}

set contfile [open [lindex $argv 0] r]
set outfile  [open [lindex $argv 1] w]
set useval [lindex $argv 2]

if {$contfile == "" || $outfile == "" || $useval == ""} {
    puts "Incorrect parameters (unable to open files or get useval)"
}

set start_time [clock seconds]
set timestring [clock format $start_time -format "%H %M %S"]
puts "Start time $timestring"

while {[gets $contfile contline] != -1} {
    set docid [lindex [split [lindex $contline 1] =] 1]
    set infilename [lindex $contline 4]
    puts "DOCID #$docid is $infilename"

    set infile [open $infilename r]
    while {[gets $infile dataline] != -1} {
	if {[string compare -nocase -length 6 "<INDEX" $dataline] == 0
	&& [string first "USE=\"$useval\"" $dataline] > 0} {
	    # We are at the start of the matching info...
	    while {[gets $infile dataline] != -1 
	    && [string compare -length 8 "</INDEX>" $dataline] != 0} {
		foreach element $dataline {
		    set termfreq [lindex $element end]
		    if {$termfreq == 0} {
			set termfreq 1
		    }
		    set term [lrange $element 0 [expr [llength $element] - 2]]
		    puts $outfile "$term\t$docid\t$termfreq"
		}
	    }
	}
    }
    
    close $infile
}

close $outfile

set end_time [clock seconds]
puts "Elapsed time [expr $end_time - $start_time]"

puts "Once all of the distributed indexes have been created by this program"
puts "Use the 'batch_load' program to actually load the indexes into the DB"

exit


