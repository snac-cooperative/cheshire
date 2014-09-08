# dumpAllRecs is used to generate the TREC "rich format data"

proc dumpAllRecs {} {
    
    global recnum logrec fixlogfilename varlogfilename

    # open or reopen the logs
    if {$fixlogfilename == "" || $varlogfilename == ""} {
	pDialog .d {Log Files Not Open} {Must select and open files before using DUMP} {error} 0 OK
	
	return
    }

    # Should have logs, so we select an output file ...

    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"All files"		*}
	{"Text files"		{.txt .doc}	}
	{"Text files"		{}		TEXT}
	{"Tcl Scripts"		{.tcl}		TEXT}
	{"C Source Files"	{.c .h}		}
	{"All Source Files"	{.tcl .c .h}	}
	{"Image Files"		{.gif}		}
	{"Image Files"		{.jpeg .jpg}	}
	{"Image Files"		""		{GIFF JPEG}}
    }

    set file [tk_getSaveFile -filetypes $types -parent .files \
	    -initialfile Untitled -defaultextension .txt]

    puts "output file is $file"

    set outfile [open $file w]
    set err 0
    set tmpnum 1

    while {$err == 0} {
	# note -- calling pLogTool sets logrec to the formatted record
	set err [catch {pLogTool r $tmpnum $fixlogfilename $varlogfilename} results]

	if {$err == 1} {
	    pDialog .d {Record Retrieval Error} $results {error} 0 OK
	    return
	} else {
	    # just put the results into the dump file
	    puts $outfile "============= Transaction \# $tmpnum =============================================="
	    puts $outfile "$logrec"
	}
	incr tmpnum
    }

    return
}


