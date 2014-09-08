#!/usr/local/bin/tclsh
# convert.tcl -- converts bibliographic records from elib docs to
# SGML markup along with insertion of the path for the full OCRed document
# 
#

if {$argc != 1} {
    puts "Usage: tclsh convert.tcl bibrecords_file"
    exit
} else {
    set infile [open $argv r]
    set outfile [open $argv.sgml w]
    puts "infile is $argv : outfile is $argv.sgml"
}

set idnum 1
set failflag 0
set last_tag ""
set current_id ""

while {[set linesize [gets $infile line]] >= 0} {
    if {$linesize > 0} {
	if {[regexp {([^:]*)::[	 ]*(.*)$} $line match tag datline]} {
	    if {$failflag} {
		set failflag 0
	    }
	    if {$tag == "END"} {
		puts $outfile "</$last_tag>" 
	        if {$current_id != [string trim $datline]} {
		    puts stderr "RECORD ID MATCH PROBLEM current $current_id end $datline"
		    if {[string length $current_id] < 6} {
			puts stderr "Using $current_id"
			puts $outfile "<TEXT-REF>/elib/data/disk/disk5/documents/$current_id/HYPEROCR/hyperocr.html</TEXT-REF>"
			puts $outfile "<PAGED-REF>/elib/data/disk/disk5/documents/$current_id/OCR-ASCII-NOZONE</PAGED-REF>"
			set tmpline [string trimleft $datline $current_id]
			if {[regexp {([^:]*)::[	 ]*(.*)$} $tmpline match tag datline]} {

			    if {$tag == "BIB-VERSION"} { 
				# What happened was no newline at the end
                                # of the record -- so start a new record
				puts $outfile "</ELIB-BIB>"
				puts $outfile "<ELIB-BIB>"
				set last_tag $tag
				set last_line $datline
				puts $outfile "<$tag>$datline" nonewline 
				puts stderr "Fixed missing newline"
			    }
			} else {
			    # weird problems -- throw in the end tag and
                            # hope for the best
			    puts $outfile "</ELIB-BIB>"
                            
			}
		    } else {
			put stderr "Unable to use $current_id"
			exit
		    }
		} else {
		    puts $outfile "<TEXT-REF>/elib/data/disk/disk5/documents/$current_id/HYPEROCR/hyperocr.html</TEXT-REF>"
		    puts $outfile "<PAGED-REF>/elib/data/disk/disk5/documents/$current_id/OCR-ASCII-NOZONE</PAGED-REF>"
		    puts $outfile "</ELIB-BIB>"
		}
	    } else {
		if {$tag == "BIB-VERSION"} {
		    puts $outfile "<ELIB-BIB>"
		} else {
		    puts $outfile "</$last_tag>" 
		}
		# tidy up some flaky tagging
		if {$tag == "AUTHOR-INSTITUTIONALA" ||
		    $tag == "AUTHOR-INSTITUTIONALB" ||
		    $tag == "AUTHOR-INSTITITIONAL" ||
		    $tag == "AUTHOR-INSTITUTIONALC"} {
		     set tag "AUTHOR-INSTITUTIONAL"
		}
		if {$tag == "KEYWORD"} {
		     set tag "KEYWORDS"
		}
		if {$tag == "ID"} {
		    set current_id [string trim $datline]
		}

		set last_tag $tag
		set last_line $datline
		puts $outfile "<$tag>$datline" nonewline 
	    }

	} else {
	    set failflag 1
	    puts $outfile $line nonewline
	}
    } else { puts "blank line"}
}

close $infile
close $outfile
exit










