# logDisp -- display transaction log record in text window w

proc logDisp {w f record} {
    global logrec recnum displayFont

    $w configure -state normal

    $w delete 0.0 end
    $w insert end [format $f $record]
    
    $w configure -state disabled
    update
    return
}
