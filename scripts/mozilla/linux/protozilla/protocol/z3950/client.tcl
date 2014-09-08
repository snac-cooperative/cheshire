#!chrome://z3950/content/webcheshire

proc bgerror {message} {
    puts stderr $message
}

proc read_sock {sock} {
    global got_response
    set l [gets $sock]

    regsub -all "%%N%%" $l "\n" html
    puts stdout $html

    set got_response 1
}


set got_response 0

set esvrSock [socket localhost 9999]
fileevent $esvrSock readable [list read_sock $esvrSock]
fconfigure $esvrSock -buffering line

set envlist {}
foreach name [array names env] {
  lappend envlist [list $name $env($name)]
}

puts stderr "Sending request..."
puts $esvrSock "$envlist"

vwait got_response
puts stderr "...Got response!"
