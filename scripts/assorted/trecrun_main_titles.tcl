#
# do a TREC run -- take the QUERY file and run name from the args
#

set CHESHIRE_DATABASE trec
set CHESHIRE_CONFIGFILE /projects/metadata/cheshire/TREC/TREC.DBCONFIGFILE
set CHESHIRE_ELEMENTSET B
set CHESHIRE_RECSYNTAX GRS1
set CHESHIRE_NUMREQUESTED 1000
set CHESHIRE_NUM_START 1

if {$argc != 2} {
    puts "Required args are QUERY_INPUT_FILE_NAME RUN_NAME"
    exit
}

set queryfile [lindex $argv 0]
set run_name [lindex $argv 1]

puts "Query file name '$queryfile'"
puts "Run name '$run_name'"

#open the input file
if [catch {open $queryfile r} q_in] {
        puts stderr "Cannot open $queryfile for input: $topics"
        exit
}



# read through the query file, building the queries
set sectionname {}
set workstring {}

while {[gets $q_in inline] != -1} {
    if {$inline == "<top>" || $inline == "</top>"} {
	
    }
    if {[string compare -length 5 "<num>" $inline] == 0} {
	set q_number [lindex $inline 2]
    }
    if {[string compare -length 7 "<title>" $inline] == 0} {
	set q_titles($q_number) [lrange $inline 1 end]
    }
    if {[string compare -length 6 "<desc>" $inline] == 0} {
	set q_desc($q_number) ""
	while {[gets $q_in inline] != 0} {
	    append q_desc($q_number) $inline " "
	}
    }
    if {[string compare -length 6 "<narr>" $inline] == 0} {
	set q_narr($q_number) ""
	while {[gets $q_in inline] != 0} {
	    append q_narr($q_number) $inline " "
	}
    }
}

#puts "+++++++++++++++++++++++++TITLES++++++++++++++++++++++++++++++++"
#parray q_titles
#puts "+++++++++++++++++++++++++DESCRIPTIONS++++++++++++++++++++++++++"
#parray q_desc
#puts "+++++++++++++++++++++++++NARRATIVES++++++++++++++++++++++++++++"
#parray q_narr

if [catch {open ${run_name}_titles w} outfile] {
        puts stderr "Cannot open $name for output: $outfile"
        exit
}


foreach query [array names q_titles] {
    set q_string {}
    append q_string "{" $q_titles($query) "}"
puts "query content is '$q_string'"
    set results [search topic @ $q_string]
    set numrecs [lindex [lindex [lindex $results 0] 0] 1]
    set results [lrange $results 1 end]

    set rank 0
    set sim 1000
    foreach rec $results {
        set docno [lindex [lindex [lindex $rec 2] 1] 1]
        puts $outfile "$query 0 $docno $rank $sim ${run_name}_ti"
        incr rank
        incr sim -1
    }

}

exec sort -k 1.n,1 -k 4.n,4 ${run_name}_titles > result_tmp
exec mv result_tmp ${run_name}_titles

exit
