#!/home/ray/Work/cheshire/bin/webcheshire
if {$argc < 1} {
    puts "usage: testxml_disp XPATH_VALUE"
}
set CHESHIRE_DATABASE bibfile
set CHESHIRE_CONFIGFILE "testconfig.new"
set CHESHIRE_RECSYNTAX XML
set CHESHIRE_NUM_START 1
set CHESHIRE_ELEMENTSET "XML_ELEMENT_Fld245"
set CHESHIRE_NUMREQUESTED 100


proc getscore {rec} {

    set start [expr [string first "<SCORE>" $rec] + 7]
    set end [expr [string first "</SCORE>" $rec] - 1]
    set score [string range $rec $start $end]

    return $score
}


proc getdocid {recs item} {

    set rec [lindex $recs $item]

    if {$rec == ""} {
	return "00 (0.00000000)"
    }

    set start [expr [string first "DOCID=\"" $rec] + 7]
    set end [expr [string first "\">" $rec] - 1]
    set docid [string range $rec $start $end]

    return "$docid ([getscore $rec])"
}

# --- Process CGI query ---
puts "Query = mathematics programming"

set query {search topic mathematics programming}
puts "BOOL query"
set boolres [eval $query]

set query {search ngtopic mathematics programming}
puts "NGRAM query"
set ngramres [eval $query]

set query {search topic @ mathematics programming}
puts "PROB query"
set probres [eval $query]
# puts [lindex $probres 1]

set query {search topic % mathematics programming}
puts "BLOB query"
set blobres [eval $query]

set query {search topic @+ mathematics programming}
puts "OKAPI query"
set okapires [eval $query]

set query {search topic @\# mathematics programming}
puts "CORI query"
set corires [eval $query]

set query {search topic @@ mathematics programming}
puts "TREC2 Query"
set trec2res [eval $query]

set query {search topic @/ mathematics programming}
puts "TFIDF Query"
set tfidfres [eval $query]


puts "PROB\t\t\OKAPI\t\tTFIDF\t\tTREC2\t\tNGRAM\t\tBOOL"

for {set i 1} {$i < 21} {incr i} {

    
    set ngram [getdocid $ngramres $i]
    set prob [getdocid $probres $i]
    set okapi [getdocid $okapires $i]
    set tfidf [getdocid $tfidfres $i]
    set trec2 [getdocid $trec2res $i]
    set bool [getdocid $boolres $i]

    puts "$prob\t$okapi\t$tfidf\t$trec2\t$ngram\t$bool"

} 

#exit

puts "********************************************************************"

for {set i 1} {$i < 21} {incr i} {
    set prob [getdocid $ngramres $i]
    set rec [lindex $ngramres $i]

    puts "********************************************************************"
    puts "DOCID $prob \n$rec"
    
}
exit

