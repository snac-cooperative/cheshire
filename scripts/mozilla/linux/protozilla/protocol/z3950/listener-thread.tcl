set OUT_SOCKET 0

proc returnHTML {html} {
    global OUT_SOCKET

    if { $OUT_SOCKET != 0 } {
	set err [catch {puts $OUT_SOCKET $html}]
	if { $err } {
	    puts stderr "Could not throw result to $OUT_SOCKET."
	}
    }
}

proc handleLine {line} {
    global main

    if {$line == "***SHUTDOWN***"} {
	thread::send -async $main "set netrequest \"handle_shutdown\""
    } else {
	thread::send -async $main "set flipflop -1"
	thread::send -async $main "set netrequest \"$line\""
    }
}


proc svcHandler {sock} {
    set l [gets $sock]

    if {[eof $sock]} {
	close $sock
    } else {
	handleLine $l
    }
}

proc accept {channel address port} {
    global main OUT_SOCKET

    set OUT_SOCKET $channel
    fconfigure $channel -buffering line -blocking 0
    thread::send -async $main "set OUT_SOCKET $channel"
    fileevent $channel readable [list svcHandler $channel]
    puts stderr "Accepted from [fconfigure $channel -peername]"
}

socket -server accept 9999
vwait events
