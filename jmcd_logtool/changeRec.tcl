# changeRec -- Change the current record number and display the new record

proc changeRec {direc} {
    global recnum logrec fixlogfilename varlogfilename

    # save current recnum and then set recnum to new value
    set oldrecnum $recnum

    if {"$direc" == "b"} {
	if {"$recnum" > 1} {
	    set recnum [expr $recnum-1]
	}
    } elseif {"$direc" == "f"} {
	set recnum [expr $recnum+1]
    } elseif {"$direc" == "n"} {
	# we're faking things to open a new log file set
	set recnum 1
    } elseif {"$direc" == "0"} {
	set recnum [.controls.record get]
    }

    # grab the new record
    set err [catch {pLogTool r $recnum $fixlogfilename $varlogfilename} results]

    if {$err == 1} {
	pDialog .d {Record Retrieval Error} $results {error} 0 OK
	set recnum $oldrecnum
	.controls.record delete 0 end
	.controls.record insert end $recnum
	return
    } else {
	.controls.record delete 0 end
	.controls.record insert end $recnum
	logDisp .records.display "%s" $logrec
    }

    return
}
