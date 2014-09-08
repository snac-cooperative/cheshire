#!/home/ray/Work/cheshire/bin/webcheshire

# this format is for REAL MARC records...
zmakeformat test USMARC {{{Record #} {#} {} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 0} {{Author:    } {100} {ab} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15} {{Title:    } {245} {ab} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15} {{Subjects: } {6xx} {abcxyz} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15} {{Sub9xx:   } {960} {abxz} {} { } {
} TRUE FALSE FALSE FALSE FALSE FALSE 15}}


# --- Process CGI query ---
zselect sherlock
set query "zfind su mathematics"

set results [eval $query]

set results [zdisplay]

foreach rec [lrange $results 1 end] {

    puts "raw record: $rec"

    set tmp [zformat full $rec marc 1 70 /home/ray/Work/cheshire/doc/USMARC11.DTD]
    
    puts "tmp is $tmp"
    
    puts [highlight -stem $query $tmp "<START>" "<END>"]

}
