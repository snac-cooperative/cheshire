#!chrome://z3950/content/webcheshire


# --- Get Thread lib from Chrome
set MOZHOME $env(MOZILLA_FIVE_HOME)

# --- Uncomment if we have libthread  (PITA) ---
#set threaddir "$MOZHOME/chrome/z3950/content/z3950"
#load "$threaddir/libthread2.1.so"
#set flipflop 0
#set thread_listener [thread::create]
#set thread_ticker [thread::create]
#set thread_main [thread::id]
#thread::send $thread_ticker "set worker $thread_main"
#thread::send -async $thread_ticker "source ticker.tcl"
#thread::send $thread_listener "set main $thread_main"
#thread::send -async $thread_listener "source listener.tcl"

# --- Move to source dir in Profiles
cd $env(SCRIPT_DIRECTORY)

source "z3950-server.tcl"
source "listener.tcl"

proc bgerror {message} {
    puts stderr $message
}

proc handle_shutdown {} {
    #   global thread_listener thread_ticker
    #    thread::send -async $thread_ticker "thread::exit"
    #    thread::send $thread_listener "thread::exit"
    puts stderr "Shutting down z3950 proxy server."
    exit

}




puts stderr "Started z3950 proxy server."

#set netrequest ""
#while { 1 } {
#    vwait netrequest
#    if {$netrequest != "handle_shutdown"} {
	# set flipflop 0
#	set cmd "handle_request \{$netrequest\}"
#	eval $cmd
#	if { 0 } {
#	set err [catch {eval $cmd} cmdresults]
#	if {$err != 0} {
#	    puts stderr "Error: $cmdresults"
#	}
#	}
#    } else {
#	handle_shutdown
#    }
#}


