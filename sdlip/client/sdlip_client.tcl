#!/usr/local/bin/wish -f
#
# 
#        File Name:      sdlip_client
# 
#        Programmer:     Ray Larson
#
#        Based on:       opac by Ralph Moon, Jerome McDonough
#                        mkSearch.tcl (by Joachim Martin and
#                          Alicia Abramson)
# 
#        Purpose:        Search interface for Cheshire II via SDLIP
# 
#        Usage:          
# 
#        Variables:      none
# 
#        Return Conditions and Return Codes:	none
# 
#        Date:           9/1/94
#        Revised:
#        Version:        1.3
#        Copyright (c) 1995.  The Regents of the University of California
#          All Rights Reserved
# 
# ***********************************************************************/

set defaultPath "/home/ray/Work/cheshire/sdlip/client";  # Path for Cheshire II stuff

# Includes
source $defaultPath/pDialog.tcl
source $defaultPath/pSearchButtonCmd.tcl
source $defaultPath/chWidgetOn.tcl
source $defaultPath/pClearTerms.tcl
source $defaultPath/pZselect.tcl
source $defaultPath/pResize.tcl
source $defaultPath/pTagCreate.tcl
source $defaultPath/displayRecs.tcl
source $defaultPath/pBooleanTypeDisplay.tcl
source $defaultPath/traceWindow.tcl
source $defaultPath/metadata.tcl

# the following might be needed for some installations
proc loadxmlpackages {args} {
     set dir /home/ray/Work/cheshire/sdlip/TclXML-1.1.1
     source /home/ray/Work/cheshire/sdlip/TclXML-1.1.1/pkgIndex.tcl
 }

package unknown loadxmlpackages

package require xml

# Initialized variables

set titleFont "-*-Courier-Bold-O-Normal-*-140-*"; #Default Title Font
set defaultBG "NavajoWhite1";		# Default background color
set defaultActiveBG "NavajoWhite3";	# Default active bg color
set boolTypeFG "firebrick4";		# Boolean type display fg color
set dialogBG "Wheat1";                  # Dialog box background
set dialogActiveBG "Wheat2";            # Dialog box active bacground
set textBG "white";			# Text window color
set entryBG "Wheat2";			# Entry window color
set buttonBG "NavajoWhite2";            # Button Color
set HTFG "red4";                        # hypertext foreground
set HTActiveFG "red2";                  # active hypertext fg color
set disabledFG "grey65";              # Disabled function label color
set selectFG "IndianRed3";		# active foreground for Menu ticks
set tmpdir "/tmp/";                     # temp directory for log files, etc.
set fixedlogfilename "NULL";            # fixed length record log file
set varlogfilename "NULL";              # variable length record log file
set transnum 1;                         # unique transaction ID for logs
set userid 1;                           # ID for each user "session"
set C2_helpinfo1 "";                    # help info for logs
set C2_helpinfo2 "";
set C2_helpinfo3 "";
set stoplist "";			# stoplist for pTRECtag           

set currServer {};

# !! This variable is a temporary expedient for testing the interface.
# It will be replaced with an array indexed by host. The first item each
# is the index name as it will be give to zfind, and the second is the
# label to appear on the button.  The third is the default mode of search
# interaction.

set hostList { {"bibfile" "UCB Digital Lib" "Ranked" ""} \
	{"scilib" "UCB Physical Sci." "Ranked" ""} \
        {"melvyl" "Melvyl" "Boolean" ""} \
	{"pe" "Melvyl Periodicals" "Boolean" ""} \
	{"mags" "Expanded Acad Index" "Boolean" ""} \
	{"news" "Nat'l Newspaper Index" "Boolean" ""} \
	{"comp" "Computer Database" "Boolean" ""} \
	{"ins" "Inspec" "Boolean" ""} \
        {"abi" "ABI Inform" "Boolean" ""} \
        {"artindex" "ArtIndex (OCLC)" "Boolean" ""} \
	{"lc_books" "Library of Congress" "Boolean" ""} \
	{"stanford" "Stanford SimpleLSP" "Boolean" ""} \
	{"att_cia" "CIA World Factbook" "Boolean" ""}}

#		  {"cheshire3" "Test Cheshire A" "Ranked"}}

# the following indexLists specify the searchable indexes for each
# host listed above, and a button label to go with the index name.
# Ultimately, the searchable indexes for each server should be given
# *by* the server through Z39.50 explain, rather than keeping a local list.

set indexList(melvyl) {{"dc:Creator" "Author"} \
			   {"dc:Title" "Title"} \
			   {"dc:Subject" "Subject"} \
			   {"dc:Date" "Date"} {"dc:Language" "Language"} \
			   {"unused" "Unused"}}

set indexList(artindex) {{"dc:Creator" "Author"} \
			   {"dc:Title" "Title"} \
			   {"dc:Subject" "Subject"} \
 		           {"contains" "Contains Words"} \
			   {"dc:Date" "Date"} {"dc:Language" "Language"} \
			   {"unused" "Unused"}}

set indexList(pe) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
		       {"dc:Subject" "Subject"} \
		       {"unused" "Unused"}}

set indexList(mags) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			 {"dc:Subject" "Subject"} \
			 {"contains" "Keyword"} \
			 {"dc:Date" "Date"} \
			 {"unused" "Unused"}}

set indexList(comp) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			 {"dc:Subject" "Subject"} \
			 {"contains" "Keyword"} \
			 {"dc:Date" "Date"} \
			 {"unused" "Unused"}}

set indexList(news) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			 {"dc:Subject" "Subject"} \
			 {"any" "Keyword"} \
			 {"dc:Date" "Date"} \
			 {"unused" "Unused"}}

set indexList(ins) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			 {"dc:Subject" "Subject"} \
			 {"contains" "Keyword"} \
			 {"dc:Date" "Date"} \
			 {"unused" "Unused"}}

set indexList(abi) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			 {"dc:Subject" "Subject"} \
			 {"contains" "Keyword"} \
			 {"dc:Date" "Date"} \
			 {"unused" "Unused"}}

set indexList(lc_books) {{"dc:Creator" "Author"} \
			     {"dc:Title" "Title"} \
			     {"dc:Subject" "Subject"} \
			     {"unused" "Unused"}}


set indexList(bibfile) {{"dc:Creator" "Author"} {"dc:Title" "Title"} \
			     {"dc:Subject" "Subject"} \
			     {"dc:Type" "Record type"} \
			     {"contains" "Contents"} \
			     {"dc:Subject" "Subject"} \
                             {"dc:Date" "Date"} \
			     {"unused" "Unused"}}

set indexList(att_cia) {{"dc:Title" "Title"} \
			{"dc:Subject" "Subject"} \
			{"contains" "Contents"} \
			{"unused" "Unused"}}

set indexList(SUNSITE) {{"author" "Author"} {"dc:Title" "Title"} \
			     {"dc:Subject" "Subject"} \
			     {"dc:Type" "Record type"} \
			     {"contains" "Topic Words"} \
			     {"dc:Identifier" "Call Number"} \
			     {"unused" "Unused"}}

set indexList(scilib) {{"author" "Author"} {"dc:Title" "Title"} \
			     {"dc:Subject" "Subject"} \
			     {"dc:Type" "Record type"} \
			     {"contains" "Contains Words"} \
			     {"dc:Identifier" "Call Number"} \
			     {"unused" "Unused"}}

set indexList(stanford) {{"author" "Author"} {"dc:Title" "Title"} \
			     {"dc:Subject" "Subject"} \
			     {"unused" "Unused"}}


# set the initial host to nothing with hostname button showing START
set currHost ""
set currHostName "START"

# set the user interface up for ranked searching
set currSearch "pr"
set currSearchName "Ranked"

# start with all boolean buttons set to a value of "and" or "AND"
set boolSwitch1 and
set boolSwitch2 and
set boolSwitch3 and

# start with a bunch of useful global variables set to empty or 0
set currSelectList {};			# currently selected records
set currSaveList {};			# Saved records from current search
set permSaveList {};			# Saved records from prev searches
set currHistory {};                     # List of previous searches
set nHits 0;                            # total hits for search
set nRetr 0;                            # records retrieved for this search
set totRcvd 0
set defaultPacketSize 10;               # approx. records to grab at 1 time
set currPacketSize $defaultPacketSize
set unusedButtonName "Index?";          # boolean index button
set current_full_display 0;
set sdlip_trace 0;

# list of user interface configurations
set searchList {{"bl" "Boolean"} {"pr" "Ranked"}}

# list of boolean terms for buttons
set boolList {"and" "or" "AND" "OR" "not" "NOT"}

# list of server names and button labels
set serverList {{"sherlock.berkeley.edu:9999:" "CDL Gateway (Sherlock)"} {"dlp.cs.berkeley.edu:8888:" "ELIB (DLP)"} {"galaxy.cs.berkeley.edu:8888:" "ELIB (Galaxy)"} {"coke.stanford.edu:8080:simplelsp" "Stanford SimpleLSP"}}

option add *background $defaultBG
option add *activeBackground $defaultActiveBG
option add *disabledForeground $disabledFG
option add *Dialog*background $dialogBG
option add *Dialog*activeBackground $dialogActiveBG

#
# Set interface size for current display
#
set screenheight [winfo screenheight .]
set screenwidth [winfo screenwidth .]
if {($screenheight >= 768) && ($screenwidth >= 1024)} {
    set windowsize large
    set newwindowsize large
    wm minsize . 650 650
    wm maxsize . 950 950
    set titleFont "-adobe-courier-bold-o-normal--17-120-100-100-m-100-iso8859-1"
    set defaultFont "-adobe-helvetica-bold-r-normal--17-120-100-100-p-92-iso8859-1"; # for Buttons
    set entryFont "*-adobe-courier-bold-r-normal--17-120-100-100-m-100-iso8859-1";     # for entry widgets
    set smallFont "-adobe-times-bold-i-normal--14-100-100-100-p-77-iso8859-1";       # for small labels
    set statusFont "-adobe-helvetica-bold-r-normal--14-100-100-100-p-82-iso8859-1";  # for status bar
} elseif {($screenheight >=600) && ($screenwidth >= 800)} {
    set windowsize medium
    set newwindowsize medium
    wm minsize . 525 525
    wm maxsize . 850 850
    set titleFont "-adobe-courier-bold-o-normal--14-100-100-100-m-90-iso8859-1"
    set defaultFont "-adobe-helvetica-bold-r-normal-14-100-100-100-p-82-iso8859-1"; # for Buttons
    set entryFont "-adobe-courier-bold-r-normal--14-100-100-100-m-90-iso8859-1";     # for entry widgets
    set smallFont "-adobe-times-bold-i-normal--11-80-100-100-p-57-iso8859-1";       # for small labels
    set statusFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1";  # for status bar
} elseif {($screenheight >=480) && ($screenwidth >= 640)} {
    set windowsize small
    set newwindowsize small
    wm minsize . 400 400
    wm maxsize . 750 750
    set titleFont "-adobe-courier-bold-o-normal--11-80-100-100-m-60-iso8859-1"
    set defaultFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1"; # for Buttons
    set entryFont "-adobe-courier-bold-r-normal--11-80-100-100-m-60-iso8859-1";     # for entry widgets
    set smallFont "-adobe-times-bold-i-normal--11-80-100-100-p-57-iso8859-1";       # for small labels
    set statusFont "-adobe-helvetica-bold-r-normal--11-80-100-100-p-60-iso8859-1";  # for status bar
}

#
# Basic structure of search interface
# 

wm title . "Cheshire II SDLIP Client"
wm iconname . "Cheshire II"

frame .mbar -relief raised -bd 1;	# Background for menu bar
frame .search -relief raised -bd 1;	# Background for entry windows
frame .search.panel0 -relief flat;	#   Label
frame .search.panel1 -relief flat;	#   space for prob. search panel
frame .search.panel2 -relief flat;	#   Empty???
frame .search.panel3 -relief flat;      #   boolean entry windows
frame .search.bbar -relief flat;        # Background for button bar
frame .retrieve -relief raised -bd 1;	# Background for text window
frame .statusbar -relief raised -bd 1;	#   Control panel
frame .statusbar.top -relief flat;
frame .statusbar.bottom -relief flat;
frame .statusbar.top.bbar2 -relief flat -bd 1;  #Background for lower button bar
frame .statusbar.bottom.a -relief flat;	#     Retrieval counter
frame .statusbar.bottom.b -relief flat;	#     Action label -- blinkenlight


#
#   Status Bar
#

scale .statusbar.bottom.a.scale -label Retrievals -length 10c -sliderlength 8 \
    -background $defaultBG -orient horizontal -from 0 \
    -foreground $disabledFG -state disabled -troughcolor $disabledFG \
    -highlightbackground $defaultBG -highlightcolor $defaultBG \
    -font $statusFont

label .statusbar.bottom.a.hitsLabel -textvariable nHits \
    -foreground $defaultBG -font $statusFont

button .statusbar.bottom.b.actionLabel -text "Inactive" -width 12 \
    -relief sunken -foreground $defaultBG \
    -disabledforeground $defaultBG -state disabled \
    -highlightbackground $defaultBG -highlightcolor $defaultBG \
    -font $statusFont


#
#   Lower Button Bar
#
#   Retrieval window
#   

text .retrieve.text -relief sunken -bd 1 -width 86 -height 22 \
    -background $textBG \
    -highlightbackground $defaultBG \
    -highlightcolor $defaultBG \
    -selectbackground $textBG \
    -selectborderwidth 0 \
    -font $entryFont \
    -cursor {} \
    -wrap word \
    -state disabled \
    -yscrollcommand ".retrieve.scroll set";	# Text window

scrollbar .retrieve.scroll \
    -background $defaultBG \
    -activebackground $defaultActiveBG \
    -relief sunken \
    -troughcolor $buttonBG \
    -highlightcolor $buttonBG \
    -highlightbackground $defaultBG \
    -command ".retrieve.text yview";		# Scroll bar

#bind .retrieve.scroll <Any-ButtonRelease> {pMoreButtonCmd $currPacketSize}
#pTagCreate .retrieve.text


#
#   Entry window
#   

#     Create 4 menubuttons and entry windows.  The search terms
#     entered are associated with the index currently listed on
#     the button

label .search.title -text "SEARCH TERMS" -font $defaultFont
label .search.problabel -font $smallFont -text "Keyword (contains):"
text .search.probentry -relief sunken -width 82 -height 2  \
    -font $entryFont -highlightbackground $defaultBG \
    -background $entryBG  -wrap word -highlightcolor black -state disabled
bind .search.probentry <Return> pSearchButtonCmd

label .search.boollabel -font $smallFont -text "Boolean Searching:"
label .search.booltype0 -text "A" -foreground $boolTypeFG \
	-font $defaultFont
button .search.bool1 -relief raised -bd 1 -textvariable boolSwitch1 \
	-bg $buttonBG -fg $boolTypeFG -command "pBooleanTypeDisplay 1" \
	-highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont -state disabled
label .search.booltype1 -text "B" -foreground $boolTypeFG \
	-font $defaultFont

for {set i 0} {$i<2} {incr i} {
    set index($i) "unused"
    set searchTerm($i) ""
    menubutton .search.button$i -relief raised -width 11 -height 1 \
	-text $unusedButtonName -menu .search.button$i.menu \
	-bg $buttonBG -font $defaultFont -state disabled
    entry .search.entry$i -width 25 -relief sunken \
	-font $entryFont -highlightbackground $defaultBG \
	-background $entryBG -highlightcolor black -state disabled
    bind .search.entry$i <Return> pSearchButtonCmd
}


#
#   Upper Button bar
#   

button .search.bbar.history -relief raised -bd 2 -text "View Metadata" \
    -command "createMetadataWindow" -bg $buttonBG  \
    -highlightbackground $defaultBG -highlightcolor $defaultBG \
    -font $defaultFont 
button .search.bbar.search -relief raised -bd 2 -padx 20 -pady 5 \
    -text "SEARCH" -command pSearchButtonCmd -bg $buttonBG -state disabled \
    -highlightbackground $defaultBG -highlightcolor $defaultBG \
	-font $defaultFont
button .search.bbar.clear -relief raised -bd 2 -text "Clear Terms" \
    -bg $buttonBG -command "ClearTerms" -highlightbackground $defaultBG \
    -highlightcolor $defaultBG -font $defaultFont

#
#   Menubar
#  

menubutton .mbar.exit -text "Exit  " -menu .mbar.exit.menu -underline 0 \
	-font $defaultFont
label .mbar.hostDisp -relief flat -text "Host:" -font $defaultFont
menubutton .mbar.host -width 20 -relief groove -bd 3 -textvariable \
   	 currHostName -bg $buttonBG -fg red -menu .mbar.host.menu \
 	 -font $defaultFont
label .mbar.nrecs -relief flat -text "  No. of Records wanted:" \
	 -font $defaultFont

set numRecsReq "20"

entry .mbar.nrecsentry -width 8 -relief sunken \
	-font $entryFont -highlightbackground $defaultBG\
	-background $entryBG -highlightcolor black -textvariable numRecsReq

label .mbar.serverDisp -relief flat -text "  Server:" -font $defaultFont
menubutton .mbar.server -width 7 -relief groove -bd 3 \
    -textvariable currServerDisp -bg $buttonBG -menu .mbar.server.menu \
	-font $defaultFont
menubutton .mbar.special -text "Options" -menu .mbar.special.menu \
    -underline 0 -font $defaultFont
menubutton .mbar.help -text "Help" -menu .mbar.help.menu \
    -underline 0 -font $defaultFont


#   Exit menu

menu .mbar.exit.menu -tearoff 0
.mbar.exit.menu add command -label "Yes" -underline 0 \
    -command "destroy .; exit" -font $defaultFont
.mbar.exit.menu add command -label "No" -underline 0 -font $defaultFont

#   Host menu

menu .mbar.host.menu -selectcolor $selectFG -tearoff 0
foreach l $hostList {
    .mbar.host.menu add radiobutton -indicatoron 1 -selectcolor $selectFG \
	-label [lindex $l 1] \
	-variable currHostName -value [lindex $l 0] \
	-command {pZselect $currHostName} -font $defaultFont
}
.mbar.host.menu add radiobutton -label "None" -variable currHostName \
    -value "START" -font $defaultFont


#   Search interface menu

#   server menu

menu .mbar.server.menu -selectcolor $selectFG -tearoff 0
foreach l $serverList {
    .mbar.server.menu add radiobutton -label [lindex $l 1] \
	-variable currServer -value [lindex $l 0] \
        -font $defaultFont -command "set currServerDisp [lindex [lindex $l 1] 0]"
}

#   Options menu
menu .mbar.special.menu -tearoff 0 -selectcolor $selectFG
.mbar.special.menu add command -label "Trace Window" -underline 0 \
    -font $defaultFont -command "set sdlip_trace 1; createTraceWindow ;"
.mbar.special.menu add cascade -label "Window Size" -underline 0 \
    -menu .mbar.special.menu.win -font $defaultFont
.mbar.special.menu add command -label "Restart" -underline 0 \
	-command pResetButtonCmd -font $defaultFont

menu .mbar.special.menu.win -tearoff 0 -selectcolor $selectFG
.mbar.special.menu.win add radiobutton -label "Small" \
    -variable newwindowsize -value "small" -command pResize \
	-font $defaultFont
.mbar.special.menu.win add radiobutton -label "Medium" \
    -variable newwindowsize -value "medium" -command pResize \
	-font $defaultFont
.mbar.special.menu.win add radiobutton -label "Large" \
    -variable newwindowsize -value "large" -command pResize \
	-font $defaultFont


#
#   Pack
#

pack .mbar .search .retrieve .statusbar -side top -fill x
pack .search.panel0 -side top -fill x
pack .search.panel1 -side top -fill x -pady 1m
pack .search.panel2 .search.panel3 .search.bbar -side top -fill x
pack .mbar.exit -side left -pady 2
pack .mbar.hostDisp .mbar.host -side left -pady 2
pack .mbar.nrecs .mbar.nrecsentry -side left -pady 2
pack .mbar.serverDisp .mbar.server -side left -pady 2
pack .mbar.special -side right -ipadx 2m -pady 2
pack .search.bbar.clear -side left -pady 12 -ipadx 3m -expand 1
pack .search.bbar.search -side left -pady 12 -ipadx 3m -ipady 2 -expand 1
pack .search.bbar.history -side left -pady 12 -ipadx 3m -expand 1
pack .retrieve.scroll -in .retrieve -side right -fill y -padx 2 -pady 2
pack .retrieve.text -in .retrieve -side left -padx 2 -pady 2 -fill x -expand 1
pack .statusbar.top .statusbar.bottom -side top -fill x -in .statusbar
pack .statusbar.top.bbar2 -side top -fill x -in .statusbar.top

pack .statusbar.bottom.a -in .statusbar.bottom -side left -fill x -pady 1
pack .statusbar.bottom.b -in .statusbar.bottom -side right -fill x -pady 1
pack .search.title  -in .search.panel0 -side top
pack .search.problabel -in .search.panel1 -side top -anchor w
pack .search.probentry -in .search.panel1 \
	-side top -padx 1m -pady 1m
pack .search.boollabel -in .search.panel2 -side top -anchor w
pack .search.button0 -in .search.panel2 \
    -side left -padx 1m
pack .search.entry0 -in .search.panel2 \
    -side left -padx 1m -pady 1m
pack .search.booltype0 .search.bool1 .search.booltype1 -in .search.panel2 \
    -side left -fill x -expand 1
pack .search.entry1 -in .search.panel2 \
    -side right -padx 1m -pady 1m
pack .search.button1 -in .search.panel2 \
    -side right -padx 1m
pack .statusbar.bottom.a.scale -in .statusbar.bottom.a -side left -padx 3m \
    -ipadx 5 -ipady 3
pack .statusbar.bottom.a.hitsLabel -in .statusbar.bottom.a -side left -ipady 3
pack .statusbar.bottom.b.actionLabel -in .statusbar.bottom.b -side right \
    -padx 10m -ipadx 1m -ipady 3

# set cursor keys to move record display window 
bind . <KeyPress-Up> {.retrieve.text yview scroll -1 units}
bind . <KeyPress-Down> {.retrieve.text yview scroll 1 units}
bind . <KeyPress-Prior> {.retrieve.text yview scroll -1 pages}
bind . <KeyPress-Next> {.retrieve.text yview scroll 1 pages}
bind . <KeyRelease-Up> {pMoreButtonCmd $currPacketSize}
bind . <KeyRelease-Down> {pMoreButtonCmd $currPacketSize}
bind . <KeyRelease-Prior> {pMoreButtonCmd $currPacketSize}
bind . <KeyRelease-Next> {pMoreButtonCmd $currPacketSize}
.mbar.server.menu invoke 1





