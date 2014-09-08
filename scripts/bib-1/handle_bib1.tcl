#!/home/ray/Work/cheshire/bin/webcheshire
# 
# 
#
set envvars {SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL \
		 SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED \
		 SCRIPT_NAME QUERY_STRING REMOTE_HOST REMOTE_ADDR \
		 REMOTE_USER AUTH_TYPE CONTENT_TYPE CONTENT_LENGTH \
		 HTTP_ACCEPT HTTP_REFERER HTTP_USER_AGENT}


puts "Content-type: text/HTML\n"
puts "<HTML>"
puts "<HEAD>"
puts "<TITLE>Cheshire II BIB-1 Attribute Values </TITLE>"
puts "</HEAD>"

puts "<BODY VLINK =\"990000\" \
BACKGROUND=\"/GIFS/paper3.gif\">" 
puts "<FONT COLOR=#800000><H1><IMG SRC=\"/GIFS/smallcat.gif\" \
ALIGN=middle>Cheshire II BIB-1 Attribute Values</H1> </FONT>"
puts "<hr>"


if {[string compare $env(REQUEST_METHOD) "POST"]==0} {
    set message [split [read stdin $env(CONTENT_LENGTH)] &]
} else {
    set message [split $env(QUERY_STRING) &]
}

foreach pair $message {
    set name [lindex [split $pair =] 0]
    set val [lindex [split $pair =] 1]
    regsub -all {\+} $val { } val
    # kludge to unescape some chars
    regsub -all {\%0A} $val \n\t val
    regsub -all {\%2C} $val {,} val
    regsub -all {\%27} $val {'} val
    regsub -all {\%28} $val {(} val
    regsub -all {\%29} $val {)} val
    
    set indata($name) "$val"

    #puts "<p>indata($name) == '$val'"
}

if {$indata(use1) != 0} {
    set use_val $indata(use1)
} elseif {$indata(use2) != 0} {
    set use_val $indata(use2)
} elseif {$indata(use3) != 0} {
    set use_val $indata(use3)
} elseif {$indata(use4) != 0} {
    set use_val $indata(use4)
} elseif {$indata(use5) != 0} {
    set use_val $indata(use5)
} elseif {$indata(use6) != 0} {
    set use_val $indata(use6)
} else {

    puts "<p>No USE value set -- return to selection screen and select one"

}


puts "<table>"
puts "<tr><td><H1>USE</H1></td><td><H1> Attribute 1=$use_val</H1></td></tr>"
if {$indata(relation) != 0} {
    puts "<tr><td><H1>RELATION</H1></td><td><H1> Attribute 2=$indata(relation)</H1></td></tr>"
}
if {$indata(position) != 0} {
    puts "<tr><td><H1>POSITION</H1></td><td><H1> Attribute 3=$indata(position)</H1></td></tr>"
}
if {$indata(structure) != 0} {
    puts "<tr><td><H1>STRUCTURE</H1></td><td><H1> Attribute 4=$indata(structure)</H1></td></tr>"
}
if {$indata(truncation) != 0} {
    puts "<tr><td><H1>TRUNCATION</H1></td><td><H1> Attribute 5=$indata(truncation)</H1></td></tr>"
}
if {$indata(completeness) != 0} {
    puts "<tr><td><H1>COMPLETENESS</H1></td><td><H1> Attribute 6=$indata(completeness)</H1></td></tr>"
}
puts "</table>"

puts "<p><hr>"
puts "<P><FONT COLOR=#800000><H2>Cheshire Configuration file INDXMAP entry:</H2></FONT>"
puts "<br>"

puts "<H2>&lt;INDXMAP ATTRIBUTESET=\"1.2.840.10003.3.1\"&gt &lt;USE&gt; $use_val &lt;/USE&gt; "
if {$indata(relation) != 0} {
    puts -nonewline  "&lt;RELAT&gt; $indata(relation) &lt;/RELAT&gt; "
}
if {$indata(position) != 0} {
    puts -nonewline  "&lt;POSIT&gt; $indata(position) &lt;/POSIT&gt; "
}
if {$indata(structure) != 0} {
    puts -nonewline  "&lt;STRUCT&gt; $indata(structure) &lt;/STRUCT&gt; "
}
if {$indata(truncation) != 0} {
    puts -nonewline  "&lt;TRUNC&gt; $indata(truncation) &lt;/TRUNC&gt; "
}
if {$indata(completeness) != 0} {
    puts -nonewline  "&lt;COMPLET&gt; $indata(completeness) &lt;/COMPLET&gt; "
}

puts "&lt;/INDXMAP&gt</H2>"

puts "<p><hr>"
puts "<P><FONT COLOR=#800000><H2>Cheshire Client Search Index Definition:</H2></FONT>"
puts "<br>"

puts "<H2>\[1.2.840.10003.3.1 1=$use_val"
if {$indata(relation) != 0} {
    puts -nonewline  ", 2=$indata(relation)"
}
if {$indata(position) != 0} {
    puts -nonewline  ", 3=$indata(position)"
}
if {$indata(structure) != 0} {
    puts -nonewline  ", 4=$indata(structure)"
}
if {$indata(truncation) != 0} {
    puts -nonewline  ", 5=$indata(truncation)"
}
if {$indata(completeness) != 0} {
    puts -nonewline  ", 6=$indata(completeness)"
}

puts "\]</H2>"

puts "</BODY></HTML>"













