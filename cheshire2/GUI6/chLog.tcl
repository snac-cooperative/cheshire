#chLog.tcl -- functions for performing transaction loggin of
#             Cheshire II

proc pLogCmd {trancode} {
    global fixedlogfile fixedlogfilename varlogfile varlogfilename
    global tmpdir
    global transnum userid currHost currSearch currFormat
    global boolSwitch1 boolSwitch2 boolSwitch3 cmd
    global host SLi searchSet tmpfname
    global currSaveList C2_helpinfo1 C2_helpinfo2 C2_helpinfo3

    # log the transaction

    pTranLog $trancode

    # increment the unique transaction ID number

    set transnum [expr $transnum+1]

    
}
