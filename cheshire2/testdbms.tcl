#!/home/ray/Work/cheshire/bin/webcheshire
set CHESHIRE_DATABASE ray
set CHESHIRE_CONFIGFILE "/home/ray/Work/cheshire/config/testconfig.dbms"
set CHESHIRE_RECSYNTAX SGML
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 10


# --- Process CGI query ---

set query {search author BARZUN, JACQUES}

set results [eval $query]

foreach part $results {

    puts $part
    puts "------------------------------------------------------------------"

}
