#!/home/ray/Work/cheshire/bin/webcheshire
set CHESHIRE_DATABASE ray
set CHESHIRE_CONFIGFILE "/home/ray/Work/cheshire/config/testconfig.dbms"
set CHESHIRE_RECSYNTAX SUTRS
set CHESHIRE_NUM_START 1
set CHESHIRE_NUMREQUESTED 10


# --- Process CGI query ---

set query {sql insert into indx (accno, subcode) VALUES ('Z888', 8888);}

set results [eval $query]

foreach part $results {

    puts $part
    puts "------------------------------------------------------------------"

}
