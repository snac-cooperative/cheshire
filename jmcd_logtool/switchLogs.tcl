# switchLogs -- Get the names of the log files and open the new logs

proc switchLogs {e1 e2} {
    global fixlogfilename varlogfilename

    set fixlogfilename [$e1 get]
    set varlogfilename [$e2 get]

    pLogTool o $fixlogfilename $varlogfilename

    return
}
