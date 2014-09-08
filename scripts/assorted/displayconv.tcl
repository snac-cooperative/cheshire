#!/usr/local/bin/tclsh
# sample external conversion program
# takes sgml as stdin and converts all tags to "BLOTZ"

set data [read stdin]

foreach line [split $data "\n"] {

    regsub -all "(<)(\[^>\]*)(>)" $line {*BLOTZ*} outline
    puts stdout $outline
}

exit
