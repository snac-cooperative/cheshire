#!/home/ray/Work/cheshire/bin/webcheshire

set CHESHIRE_DATABASE paleo
set CHESHIRE_CONFIGFILE "DBCONFIGFILE"
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 10


# this format is for SGMLMARC record....
zmakeformat test USMARC {{{Record #} {#} {} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 0} {{Author:   } {Fld100 Fld110} {A B} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15} {{Title:   } {Fld245} {A B} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15} {{Sub9xx:   } {Fld960} {A B X Z} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15}}


# --- Process CGI query ---
set query "search docid 1"

set results [eval $query]

foreach rec [lrange $results 1 end] {

    puts [zformat test $rec sgml 1 70 /home/ray/Work/cheshire/doc/USMARC11.DTD]
	
}
