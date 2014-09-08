#!/usr/local/cheshire/bin/webcheshire


#Author:  Rob Sanderson (azaroth@liv.ac.uk)
#Copyright (c) The University of Liverpool All Rights Reserved
#Last Modified: 6/10/99
#Description:  EAD searching with ead2html and cheshire2
#Version: 2.0.0 (Fully functional, no longer relies on ead2html-cgi, caching and DSC checking)
#Based initially on Ray Larson's SFSearch.tcl script.


#Server Config
set CHESHIRE_DATABASE archives
set CHESHIRE_CONFIGFILE "/home/ray/EADCONF/CONFIG.ARCH_HUB"


cd "/opt/APhttpd/htdocs/ead/htmldev"
set output_to_file 1

set cont 1

while {$cont < 600} {

    #first grab the summary tpgrs data for contains_dsc and eadid.
    
    set CHESHIRE_ELEMENTSET TPGRS
    set CHESHIRE_RECSYNTAX GRS1
    set CHESHIRE_NUM_START 1
    set CHESHIRE_NUMREQUESTED 1
    
    set query "search docid $cont"
    
    set err [catch {eval $query} qresults]
    set sresults [lrange $qresults 1 end]
    set hits [lindex [lindex [lindex $qresults 0] 0] 1]

    if {!$hits} {
	set cont 0
	puts "Finished db."
	exit
    }
    incr cont

    set qresults [lindex $qresults 1]
    set toc_eadid [lindex [lindex [lindex $qresults 1] 1] 1]
    set llen [llength $qresults]

    if {$llen == 4} {

	set fname $toc_eadid
	append fname ".html"
	if {![file exists $fname]} {
	    puts "*** ID $cont ($toc_eadid) does not exist and should. ***"
	} else {
	    puts "ID $cont ($toc_eadid) exists."
	}
    } else {
	puts "ID $cont ( $toc_eadid )  doesn't contain DSC"
    }
}


