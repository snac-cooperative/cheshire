#!chrome://z3950/content/webcheshire

proc bgerror {message} {
    puts stderr $message
}

proc read_sock {sock} {
    global got_response
    set l [gets $sock]
}


set esvrSock [socket localhost 9999]
fileevent $esvrSock readable [list read_sock $esvrSock]
fconfigure $esvrSock -buffering line

puts stderr "Sending shutdown request...\n"
puts $esvrSock "***SHUTDOWN***"

