set CHESHIRE_DATABASE bibfile
set CHESHIRE_NUMREQUESTED 1
set CHESHIRE_CONFIGFILE "CONFIG.DL"
set RESULTS [tbsearch any {sierras} {water stream}]
puts "************ Cooked Results *****************"
set header [lindex $RESULTS 0]
set recs  [lrange $RESULTS 1 end]

puts "$header"

foreach record $recs {

	set rank [lindex $record 0]
	set rec  [lindex $record 1]
	set bar1 [lindex $record 2]
	set bar2 [lindex $record 3]

	puts "++++++++++++++++++ DOC ++++++++++++++++++"
	puts "Doc rank: $rank"
	puts "$rec"
	puts ""
	puts "sorted Bar1 = [lsort $bar1]"
	puts "sorted Bar2 = [lsort $bar2]"
}

set RESULTS [tbsearch any {water stream} {sierras}]
puts "************ Cooked Results *****************"
set header [lindex $RESULTS 0]
set recs  [lrange $RESULTS 1 end]

puts "$header"

foreach record $recs {

	set rank [lindex $record 0]
	set rec  [lindex $record 1]
	set bar1 [lindex $record 2]
	set bar2 [lindex $record 3]

	puts "++++++++++++++++++ DOC ++++++++++++++++++"
	puts "Doc rank: $rank"
	puts "$rec"
	puts ""
	puts "sorted Bar1 = [lsort $bar1]"
	puts "sorted Bar2 = [lsort $bar2]"
}

close_cheshire
