#!/home/cheshire/cheshire/bin/ztcl

#First, move into the the .protozilla directory from /home/azaroth
# Get rid of this for the real use
cd "/home/cheshire/.protozilla"

set preferred_language "eng"

set relevance_indexes {topic subject subj any anywhere}
set within_indexes {date}
set information_terms {"bool" "numrec" "firstrec" "recsyntax"}

set explain_database "IR-Explain-1"
set command_map(search) "zfind"
set command_map(scan) "zscan"
set command_map(explain) "zfind"


set DEBUG 1

# ------------- High Level Routines --------------

proc init_explain_query {database} {
    global URI_HOST URI_PORT
    
    if {$database == ""} {
	set database "IR-Explain-1"
    }

    #Set up our connection and load procedures
    zselect explain $URI_HOST $database $URI_PORT
    zset attributes explain
    zset recsyntax explain
    source "zexplain.tcl"

}


proc strobe_database {} {
    global URI_HOST database URI_PORT
    #load tables & proceedure
    source "zstrobe.tcl"
    do_strobe $URI_HOST $URI_PORT $database
} 

# ------------  Low Level Routines  -------------

proc throw_html_type {} {
    puts "Content-type: text/html\n\n"
}

proc throw_file { filename } {
    set fh [open $filename]
    while {[gets $fh line] >= 0} {
	puts $line
    }
    close $fh
}

proc exit_error {reason} {
    global DEBUG envvars env
    set debugtxt ""
    if { $DEBUG == 1 } {
	append debugtxt  "\n<p>Environment Variables:<p>\n<ul>"
	foreach name $envvars {
	    append debugtxt "<li>$name: $env($name)\n"
	}
	append debugtxt "</ul>\n"
    }
    #   #Currently Mozilla doesn't handle app/x-js type files :P
     puts stdout "Content-type: text/html\n\n<html><head><script>alert(\"$reason\");</script></head><body></body></html>"
    # puts stdout "Content-type: application/x-javascript\n\nalert(\"$reason\");\nexit;\n";
    exit
    puts stdout "Content-type: text/html\n\n<html>\n<head><title>Z39.50 Error</title></head>\n<body bgcolor = white>\n<center><h3>Z39.50 Error: $reason</h3></center>\n$debugtxt</body>\n</html>"
    exit
}

proc unencode_cgistring {string} {
    regsub -all {\+} $string { } string
    regsub -all {\%0A} $string \n\t string
    regsub -all {\%2C} $string {,} string
    regsub -all {\%27} $string {'} string
    regsub -all {\%22} $string "\"" string
    regsub -all {\%40} $string {@} string
    regsub -all {\%7D} $string {\}} string
    regsub -all {\%7B} $string {\{} string
    regsub -all {\%20} $string { } string
    regsub -all {\%23} $string {\#} string
    if {[regexp "^ +$" $string]} {
        set string "";
    }
    return $string;
}

proc encode_cgistring {string} {
    regsub -all { } $string {+} string
    regsub -all "\n\t" $string {\%0A} string
    regsub -all {,} $string {\%2C} string
    regsub -all {'} $string {\%27} string
    regsub -all "\"" $string {\%22} string
    regsub -all {@} $string {\%40} string
    regsub -all {\#} $string {\%23} string
    regsub -all {\}} $string {\%7D} string
    regsub -all {\{} $string  {\%7B} string
    regsub -all { } $string  {\%20} string
    return $string;
}

proc generate_query {} {
    global searchterms relevance_indexes within_indexes information_terms command;

    # This should support figuring out what type of query the  indexes support via Explain
    # Also check if Cheshire or not - if not, don't try relational etc.

    set bool $searchterms("bool")
    set firstterm 1;
    set searchterms("bool") ""

    set inlist [array names searchterms]
    set query "$command"
    foreach name $inlist {
	set value $searchterms($name)
	set name [string range $name 1 [expr [string length $name] -2]]
	if {$name != "none" && $name != "" && $value != ""} {
	    if {$firstterm == 0 && [lsearch $information_terms $name] == -1 } {
		append query " $bool"
	    }
	    if {[lsearch $relevance_indexes $name] != -1} {
		append query " ($name @ \{$value\} )"
	    } elseif { [lsearch $within_indexes $name] != -1} {
		append query " ($name <=> $value )"
	    } elseif { [lsearch $information_terms $name] == -1 } {
		append query " ($name \{$value\} )"
	    }
	    set firstterm 0
	}
    }
    return $query
}

proc process_query_string {} {
    global QUERY_STRING searchterms
    
    set arglist [split $QUERY_STRING &]
    set searchterms("bool") "AND"

    foreach pair $arglist {
	set name [lindex [set pairlist [split $pair =]] 0]
	set val [lindex $pairlist 1]
	set name [unencode_cgistring $name]
	set val [unencode_cgistring $val]
	set searchterms("$name") "$val"
    }

}


# ---------- MAIN ROUTINE ----------

# Initialise and Set the environment variables
set QUERY_STRING ""
set URI_HOST ""
set URI_PORT 210
set PATH_INFO ""

set envvars [array names env]
foreach var $envvars {
  set $var $env($var)
}

# Set up what the URL actually means.
# z3950://host:port/command/database?arguments
# arguments --  index=term & index=term ...

set pathlist [split $PATH_INFO /]

set command ""
set database ""
if {[llength $pathlist] > 1} {
    set command [lindex $pathlist 1]
}
if {[llength $pathlist] > 2} {
    set database [lindex $pathlist 2]
}


if {$URI_HOST == ""} {
    exit_error "Malformed URL; No host given."

} elseif {$command == "" || ($command == "explain" && $database == "" && $QUERY_STRING == "")} {
    init_explain_query ""
    search_explain_host
    exit
    #    exit_error "Malformed URL;  No command given."
} elseif {[lsearch [array names command_map] $command] == -1} {
    exit_error "Malformed URL; Unknown Z39.50 command '$command'."
} elseif {$database == "" && $command == "search" } {
    # No database, but search, query explain for dbs to search
    init_explain_query ""
    search_explain_databases
    exit
} elseif {$command == "explain" && $QUERY_STRING == ""} {
    init_explain_query ""
    search_explain_type $database
    exit 
} elseif {$command == "explain" } {
    # Process query string. NB: database can be "" here
    set searchterms("db") "IR-Explain-1"

    process_query_string

    # Search for db=(name) and if so, initialise with it.
    # Otherwise we need to generate a proper query

    
    puts [array names searchterms]
    
    if {$searchterms("db") != "IR-Explain-1"} {
	set explain_database $searchterms("db")
	init_explain_query $explain_database
	if {$database != ""} {
	    search_explain_type $database
	} else {
	    search_explain_host
	}
    } else {
	puts "Not finding db"
    }

    exit

} elseif {$QUERY_STRING == "" } {
    init_explain_query ""
    set success [search_explain_indexes]
    if {!$success} {
	strobe_database
   }
    exit
}

if {[lsearch [array names command_map] $command] == -1} {
    #Malformed URL, error
    exit_error "Malformed URL; unknown command '$command'"
}

set command $command_map($command)

# --- Process query terms ---
process_query_string

# --- Initialize ---

zset timeout 15
set opencmd "zselect host $URI_HOST $database $URI_PORT"
set err [catch {eval "$opencmd"} connresults]
if { $err != 0 } {
    exit_error "Connection Error; Could not connect to $URI_HOST"
}

# --- Search ---

set query [generate_query]

set err [catch {eval $query} qresults]
if {$err != 0} {
    set errmsg [lindex $qresults 1]
    exit_error "Search Error for $query: $errmsg"
}



# --- Display ---

set hits [lindex [lindex [lindex $qresults 0] 2] 1]

puts stdout "\n\nhits: $hits"

if {$hits == 0 || $err != 0} {
  puts stdout "No Matches for <code>$query</code>."
  puts "$qresults"
} else {
  zset RECSYNTAX SGML
  set err [catch {zdisplay} presults]
  puts stdout "$presults"
}



