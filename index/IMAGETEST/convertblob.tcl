#!/usr/sww/bin/tclsh
# convert.tcl -- converts bibliographic records from elib docs to
# SGML markup along with insertion of the path for the full OCRed document
# 
#

if {$argc != 1} {
    puts "Usage: tclsh convertblob.tcl blobdata_file"
    exit
} else {
    set infile [open $argv r]
    set outfile [open $argv.sgml w]
    puts "infile is $argv : outfile is $argv.sgml"
}

while {[set linesize [gets $infile line]] >= 0} { 
    if {[scan $line {%s %d %d %s %d %d %d %d %s %[-.0-9 ]} idtag imageid blobid bintag color texture location shape featuretag features]} { 
	puts $outfile "<BLOB><ID>$imageid $blobid</ID><BINS> COL$color TEX$texture LOC$location SHP$shape </BINS><FEATURES> $features </FEATURES></BLOB>"
    }
}


