#!/home/cheshire/cheshire/bin/webcheshire

cd "/home/cheshire/.protozilla/foo"
source "z3950-server.tcl"
set PROCESS_FINISHED 1

proc bgerror {message} {
    puts stderr $message
}

proc handleLine {socket line} {
    if {$line == "***SHUTDOWN***"} {
	puts stderr "Shutting down z3950 proxy server."
	exit
    } else {
	handle_request $line
    }
}

proc  svcHandler {sock} {
    set l [gets $sock]    
    if {[eof $sock]} {    
	close $sock        
    } else {
	handleLine $sock $l
    }
}


proc accept {channel address port} {
    global OUT_SOCKET
    set OUT_SOCKET $channel
    fileevent $channel readable [list svcHandler $channel]
    puts stderr "Accept from [fconfigure $channel -peername]"
    fconfigure $channel -buffering line -blocking 0
}

puts stderr "Started z3950 proxy server."

socket -server accept 9999
vwait events
