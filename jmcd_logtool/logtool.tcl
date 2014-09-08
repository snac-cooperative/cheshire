#!./logtool -f
#
#
#   File Name:     logtool.tcl
#
#   Programmer:    Jerome McDonough
#
#   Purpose:       Browse Cheshire II transaction log records
#
#   Usage:         logtool fixedlogfilename varlogfilename
#
#   Variables:     None
#
#   Date:          6/19/1996
#
#   Version:       1.0
#
#   Copyright (c) 1996.  The Regents of the University of California
#   All Rights Reserved

set auto_path [linsert $auto_path 0 ./]
set fixlogfilename "NULL";   # fixed length record transaction log file
set varlogfilename "NULL";   # variable length record trans. log file
set logrec "";               # string holding combined transaction log record 
set recnum 1;                # integer of current record number
set buttonFont "-*-helvetica-bold-r-normal-*-120-*"; # font for buttons
set entryFont "-*-courier-medium-r-normal-*-120-*";  # font for entry widgets
set displayFont "-*-courier-bold-r-normal-*-140-*";  # font for record display

# if user doesn't supply filename arguments, don't open files for them
if {$argc != 2} {
    set OPENFILES 0
} else {
    set OPENFILES 1
}

set fixlogfilename [lindex $argv 0]
set varlogfilename [lindex $argv 1]

# setup window manager
wm title . "Cheshire II Transaction Logs"
wm iconname . "C2 Transactions"

# build the basic framework
frame .files -relief raised
frame .records -relief raised
frame .controls -relief raised

# build the file control region
frame .files.labels -relief flat
frame .files.display -relief flat
frame .files.buttons -relief flat
pack .files.labels .files.display -in .files -side left
pack .files.buttons -in .files -side right -padx 5m -expand 1

label .files.labels.fixfile -relief flat -font $entryFont \
	-text "Fixed Length Record Transaction File: "
label .files.labels.varfile -relief flat -font $entryFont \
	-text "Variable Length Record Transaction File: "
pack .files.labels.fixfile .files.labels.varfile -in .files.labels -side top \
	-anchor w

entry .files.display.fixfile -width 40 -relief sunken -font $entryFont
entry .files.display.varfile -width 40 -relief sunken -font $entryFont
pack .files.display.fixfile .files.display.varfile -in .files.display \
	-side top -fill x -anchor w

button .files.buttons.open -relief raised -font $buttonFont -text "OPEN" \
    -command "switchLogs .files.display.fixfile .files.display.varfile; changeRec n"
button .files.buttons.quit -relief raised -font $buttonFont \
	-text "QUIT" -command "pLogTool c; destroy ."
pack .files.buttons.open .files.buttons.quit -in .files.buttons -side left

# build the record display region
text .records.display -relief sunken -width 80 -height 30 -wrap char \
    -font $displayFont -state disabled -yscrollcommand ".records.scroll set"
scrollbar .records.scroll -relief sunken -command ".records.display yview"
pack .records.scroll -in .records -side right -fill y -expand 1
pack .records.display -in .records -side left -fill x -expand 1

# build the record control region
button .controls.backwards -relief raised -font $buttonFont -text "BACKWARD" \
    -command "changeRec b"
button .controls.forwards -relief raised -font $buttonFont -text "FORWARD" \
    -command "changeRec f"
label .controls.current -relief flat -font $entryFont \
	-text "Current Record #: "
entry .controls.record -width 6 -relief sunken -font $entryFont
bind .controls.record <Return> "changeRec 0"
pack .controls.backwards .controls.current .controls.record \
	.controls.forwards -in .controls -side left -expand 1

# Finish Building the Whole Thing
pack .files .records .controls -in . -side top -fill y

# if user supplies filenames, put them in

if {$OPENFILES} {
    .files.display.fixfile insert end $fixlogfilename
    .files.display.varfile insert end $varlogfilename
    .controls.record insert end $recnum
    pLogTool o $fixlogfilename $varlogfilename
    pLogTool r $recnum $fixlogfilename $varlogfilename 
    logDisp .records.display "%s" $logrec
}












