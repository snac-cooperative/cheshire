#!/home/cheshire/cheshire/bin_thread/webcheshire
load "/usr/local/lib/libthread2.1.so"

cd "/home/cheshire/.protozilla/foo"
source "z3950-server.tcl"

set flipflop 0

set thread_listener [thread::create]
set thread_ticker [thread::create]
set thread_main [thread::id]

thread::send $thread_ticker "set worker $thread_main"
thread::send -async $thread_ticker "source ticker.tcl"
thread::send $thread_listener "set main $thread_main"
thread::send -async $thread_listener "source listener.tcl"

proc bgerror {message} {
    puts stderr $message
}

proc handle_shutdown {} {
    global thread_listener thread_ticker

    puts stderr "Shutting down z3950 proxy server."
    thread::send -async $thread_ticker "thread::exit"
    thread::send $thread_listener "thread::exit"
    exit

}

puts stderr "Started z3950 proxy server."

set netrequest ""
while { 1 } {
    vwait netrequest
    if {$netrequest != "handle_shutdown"} {
	set flipflop 0
	handle_request $netrequest
    } else {
	handle_shutdown
    }
}


