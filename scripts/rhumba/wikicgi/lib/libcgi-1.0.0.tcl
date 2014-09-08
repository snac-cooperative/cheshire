

# Contains the following procedures:
#   cgi_decode <value>   -> decode CGI encoded string to plain text
#   generate_cgihref <No of slashes>  -> generate href form of current form
#   textsub <value>  -> decode entities to plain text
#   throwfile <filename> <substitutions> -> throw file to stdout with substituions.

proc cgi_decode { value } {
    set ascii "                                "
    append ascii " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ\[\\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
    append ascii "                                                                                                                                                                                                                                                                                "
    set eol "<EOL>"
    set hexa "0123456789ABCDEF"

    # Do some that we can't let fall to the script
    regsub -all {\+} $value " " value;
    regsub -all "%0D%0A" $value $eol value;
    regsub -all "%0A" $value $eol value;
    regsub -all "%0D" $value $eol value;
    regsub -all "%26" $value {\&} value;

    set done 0
    while {[regexp {%([0-9A-F][0-9A-F])} [string range $value $done end] full code ]} {
	set coda [string index $code 0]
	set codb [string index $code 1]
	set idx [expr 16 * [string first $coda $hexa] + [string first $codb $hexa]]
	set char [string index $ascii $idx]
	set done [expr [string first $full $value] + 1]
	regsub -all "%$code" $value $char value
    }

    regsub -all "<EOL>" $value "\n" value
    return $value
}

proc generate_cgihref numslashes {
    global indata SCRIPT_NAME;
    set cgiquery $SCRIPT_NAME
    set inlist [array names indata]
    set firstterm 1;
    set slashes ""

    for {set n 1} {$n <= $numslashes} {incr n} {
	append slashes "\\"
    }
    foreach key $inlist {
	if {!$firstterm} {
	    append cgiquery "$slashes&"
	} else {
	    append cgiquery "?"
	    set firstterm 0
	}
	set val $indata("$key");
	set val [cgisub $val]
	append cgiquery "$key=$val"
    } 

    return $cgiquery
} 

proc textsub {content} {
    regsub -all "\&apos;" $content {'} content
    regsub -all "\&lsqb;" $content {[} content
    regsub -all "\&rsqb;" $content {]} content
    regsub -all "\&commat;" $content {,} content
    regsub -all "\&lsquo;" $content {'} content
    regsub -all "\&rsquo;" $content {'} content
    regsub -all "\&sol;" $content {/} content
    regsub -all "\&rdquo;" $content {"} content
    regsub -all "\&mdash;" $content {-} content
    regsub -all "\&ndash;" $content {-} content
    regsub -all "\&ldquo;" $content {"} content

    return $content;
}

proc throwfile {fn substs} {
    global CGI_PATH

    lappend substs "%CGI% $CGI_PATH"

    set fnH [open $fn]
    while {[gets $fnH line] >= 0} {
	foreach sub $substs {
	    regsub -all {\"} $sub {\\\"} sub
	    set subout [lindex $sub 0]
	    if [regexp $subout $line] {
		set subin [join [lrange $sub 1 [llength $sub]]]
		regsub -all [lindex $sub 0] $line $subin line
	    }
	}
	puts $line
    }
    close $fnH;
}


# Not really sure where this should go, but it seems globally useful.
# XXX Should really be abstracted further.
proc get_max_docid {db} {
    global global_variables
    foreach var $global_variables {
	global $var
    }

    if {$db == "user"} {
	set ccf $CHESHIRE_CONFIGFILE_USER
	set cdb $CHESHIRE_DATABASE_USER
    } elseif {$db == "data"} {
	set ccf $CHESHIRE_CONFIGFILE_DATA
	set cdb $CHESHIRE_DATABASE_DATA
    } else {
	set ccf $CHESHIRE_CONFIGFILE
	set cdb $CHESHIRE_DATABASE
    }
	
    set CHESHIRE_DATABASE METADATA
    set CHESHIRE_NUMREQUESTED 1
    set CHESHIRE_CONFIGFILE $ccf
    set results [search maxdocid $cdb]
    set docid [lindex $results 1]

    return $docid
    
}




# ----------- Stuff to be done for ALL calls of the library ----------------

# Set up environment for all scripts

set REMOTE_USER ""
foreach key [array names env] {
    set $key $env($key)
}

# Read in any information
if {[string compare $REQUEST_METHOD "POST"]==0} {
    set message [split [read stdin $CONTENT_LENGTH] &]
} else {
    # GET with arguments. Need to check if valid user as we're not protected
    set message [split $QUERY_STRING &]
}

# Set up some sensible defaults

set indata(numreq) 20
set indata(firstrec) 1
set indata(bool) AND
set indata(request) "null"
set indata(searchdb) "data"
set indata(format) "titles"
set maxfield 0

foreach pair $message {
    set name [lindex [split $pair =] 0]
    set val [lindex [split $pair =] 1]
    set val [cgi_decode $val]
    set indata($name) "$val"

    if { [ string range $name 0 7 ] == "fieldidx" && [set newidx [string range $name 8 end]] > $maxfield } {
	set maxfield $newidx
    }
}

# Sanity check numbers 
# NB we don't necessarily use these.
set firstrec $indata(firstrec)
set numreq $indata(numreq)

set errf [catch {expr $firstrec}]
set errm [catch {expr $numreq}]
if { $errf != 0 || $firstrec < 1} {
    throwfile "err.html" [list "\%ERR\% The start record, '$firstrec', is not an appropriate value. It s
hould be a number greater than 0"]
    exit
}
if { $errm != 0 || $numreq < 1} {
    throwfile "err.html" [list "\%ERR\% The number of records requested, '$numreq', is not an appropriate value. It should be a number greater than 0"]
    exit
}

