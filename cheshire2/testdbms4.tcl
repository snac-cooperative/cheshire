#!/home/ray/Work/cheshire/bin/webcheshire
set CHESHIRE_DATABASE ray
set CHESHIRE_CONFIGFILE "/home/ray/Work/cheshire/config/testconfig.dbms"
set CHESHIRE_RECSYNTAX SUTRS
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 10


# --- Process CGI query ---

set query {sql select b.author, b.title from bibview b where b.author like '%BARZUN%';}

set results [eval $query]

foreach part $results {

    puts $part
    puts "------------------------------------------------------------------"

}
