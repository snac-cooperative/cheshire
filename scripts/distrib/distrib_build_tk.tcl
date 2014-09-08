#!/home/ray/Work/cheshire/bin/cheshire2
# This script builds a database of distributed data by
# using the Explain database to find the databases on a server
# and then using scan to read the contents of the indexes for each
# database and store the results as "virtual collections"
#
#set singleDB {}

#if {$argc < 4} {
#    puts "Usage: distrib_build HOST PORT OUTPUT_DIRECTORY LOGFILE <DATABASENAME>"
#    exit
#}
#
#if {$argc == 5} {
#    set singleDB [lindex $argv end]
#    set LOGFILEname [lindex $argv [expr $argc - 2]]
#} else {
#    set LOGFILEname [lindex $argv end]
#}

puts "LOGFILE is tstlogfile"
set LOGFILE [open tstlogfile w]

source zexplain.tcl
#source "c:/Windows/Desktop/zexplain.tcl"


proc restartinfo {w} {
    catch {destroy $w}
    toplevel $w
    wm title $w "Collection Record Display"
    wm iconname $w "Collection Rec"

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Dismiss -command "destroy $w"
    pack $w.buttons.dismiss -side left -expand 1

    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -width 70 \
	    -height 45 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    setstyles $w.f.text

}


proc build_virtual_db {host port outdir} {
    global DATABASES
    global INDEXES
    global COMBOS
    global URI_HOST
    global URI_PORT
    global LOGFILE
    global singleDB
    global preferred_language


    if {$host == "" || $port == ""} {
	tk_messageBox -icon info -message "You must provide a host and port" -type ok\
	-parent .outframe.text
	return
    }
    
    set URI_HOST $host
    set URI_PORT $port

    puts "Host $URI_HOST Port $URI_PORT"
    puts $LOGFILE "Host $URI_HOST Port $URI_PORT"
    .outframe.text insert end "Host $URI_HOST Port $URI_PORT\n" {big color2 raised}
    update
    puts "CONNECTING"
    
    set connect_result [zselect xhost $URI_HOST IR-Explain-1 $URI_PORT]
    
    if {[lindex $connect_result 0] < 0} {
	puts "Connection failed: $connect_result"
    }
    
    set preferred_language eng
    set DATABASES {}
    
    zset recsyntax explain
    zset attributes explain

    puts "SEARCHING FOR DATABASEINFO"
    search_explain_type databaseinfo

    puts "SEARCHING FOR attributedetail"
    search_explain_type attributedetails
    # puts "DUMPING COMBINATIONS::::::::"
    # parray COMBOS
    zclose
    set dbcount 0
    set TESTDATA(0) 1
    #puts "Starting each database..."
    restartinfo .infowin

    foreach db $DATABASES {
	incr dbcount

	set dbname [lindex $db 0]

	# if looking for a single database... 
	if {$singleDB != {} && $singleDB != $dbname} {
	    continue
	}
	set recs [lindex $db 1]
	set outname "$outdir/$dbname.xml"
	set outfile [open $outname w]
	set db_start_time [clock seconds]
	set timestring [clock format $db_start_time -format "%H %M %S"]
	puts "DB $dbname: Start time $timestring"
	puts $LOGFILE "DB $dbname: Start time $timestring"
	.outframe.text insert end "Database: " {big color1}
	.outframe.text insert end "$dbname " {big color3}
	.outframe.text insert end ": $recs Records  " {big color1}
	set timeloc [.outframe.text index insert]
	# .outframe.text insert end " \n" {big color1}
	update

	zclose 
	set connect_result [zselect xhost $URI_HOST $dbname $URI_PORT]

	if {[lindex $connect_result 0] < 0} {
	    puts "Connection failed: $connect_result"
	    continue
	}

	puts -nonewline "$dbcount: $dbname has $recs records"

	puts -nonewline $LOGFILE "$dbcount: $dbname has $recs records"

	set attributeset [lindex $INDEXES($dbname) 0]
	puts " AttributeSet $attributeset"
	puts $LOGFILE " AttributeSet $attributeset"

	puts $outfile "<CDB>"
	puts $outfile "<DATABASE>$dbname</DATABASE>"
	puts $outfile "<HOSTNAME>$host</HOSTNAME>"
	puts $outfile "<PORT>$port</PORT>"
	puts $outfile "<RECORDCOUNT>$recs</RECORDCOUNT>"
	puts $outfile "<INDEXES>"

	# puts "INDEXES ARE $INDEXES($dbname)"

	set ind [lrange $INDEXES($dbname) 1 end]
	unset TESTDATA
	set indexnum 0
	set testindex 1

	zset database $dbname
	zset attributes $attributeset
	zset recsyntax SGML
	.outframe.text insert end "Indexes:\n " {bold}

	foreach pair $ind {
	    incr indexnum
	    puts "     Scanning $pair"
	    puts $LOGFILE "     Scanning $pair"
	    .outframe.text insert end "      $pair  " {bold}
	    set indexname [lindex $pair 0]
	    set usevalue [lindex $pair 1]

	    if {$indexname == "" && $usevalue != ""} {
		set indexname "ti\[1=$usevalue\]"
	    }
	    
	    set err [catch {set testscan [zscan $indexname 0 1 200 0]} errmsg]
	    if {$err} {
		# Check attribute combinations...
		puts "Error in scanning: Different attribute combo???"
		puts $LOGFILE "Error in scanning: Different attribute combo???"
		foreach combo $COMBOS($dbname) {
		    if {[lindex $combo 0] == "1=$usevalue"} {
			regsub -all { 5=[0-9]*} $combo "" combo
			regsub -all { } $combo "," combo
			puts "COMBO $combo matches"
			puts $LOGFILE "COMBO $combo matches"
			set indexname "$indexname\[$combo\]"
			set err [catch {set testscan [zscan $indexname 0 1 200 0]} errmsg]
			if {$err} {
			    continue
			} else {
			    #puts "TESTSCAN FOR RETRY IS $testscan"
			    set err 0
			    break
			}
			
		    }
		}  
		# if err is not 0, there was no match...
		if {$err} {
		    puts "Error in scanning: $errmsg -- NO ENTRIES??"
		    puts $LOGFILE "Error in scanning: $errmsg -- NO ENTRIES??"
		    puts $outfile "<INDEX NAME=\"$indexname\" ATTRIBUTESET=\"$attributeset\" USE=\"$usevalue\">"
		    puts $outfile "</INDEX>"
		    continue
		}
	    }

	    if {$indexnum == 1} {
		set TESTDATA($testindex) [lrange $testscan 1 end]
		set dupnames($testindex) "{$indexname $usevalue}"
		.outframe.text insert end "\n " {bold}
		incr testindex
	    } else {
		set dup 0
		for {set i 1} {$i < $testindex && $dup == 0} {incr i} {
		    if {[string compare $TESTDATA($i) [lrange $testscan 1 end]] == 0} {
			set dup $i
		    }
		}
		if {$dup} {
		    puts "Duplicate index, skipping..."
		    puts $LOGFILE "Duplicate index, skipping..."
		    lappend dupnames($dup) "$indexname $usevalue"
		    .outframe.text insert end " (duplicate reference)\n " {bold}
		    continue
		} else {
		    set TESTDATA($testindex) [lrange $testscan 1 end]
		    set dupnames($testindex) "{$indexname $usevalue}"
		    incr testindex
		    .outframe.text insert end "\n " {bold}
		}    
	    }

	    puts $outfile "<INDEX NAME=\"$indexname\" ATTRIBUTESET=\"$attributeset\" USE=\"$usevalue\">"

	    set moredata 1
	    set lastterm 0
	    set firstpass 1

	    set numseen 0
	    # .infowin.f.text delete 0.0 end
	    .infowin.f.text insert end "INDEX: $indexname\n" bold2

	    while {$moredata} {
		update
		set err [catch {set scandata [zscan $indexname $lastterm 0 5000 0]} errmsg]
		if {$err} {
		    puts "Error in scanning: $errmsg"
		    set moredata 0
		}
		incr numseen 5000

		if {[lindex [lindex [lindex $scandata 0] 1] 1] == 5} {
		    set moredata 0
		}
		set lastterm [lindex [lindex $scandata end] 0]
		if {$lastterm == ""} {
		    puts "Error in scanning or parsing"
		    puts "scandata is $scandata"
		    set moredata 0
		} else {
		    puts "Lastterm is $lastterm"
		    puts $LOGFILE "Lastterm is $lastterm"
		    .infowin.f.text insert end "$numseen -- Last term $lastterm\n" bold
		    if {$firstpass} {
			puts $outfile [lrange $scandata 1 end]
			set firstpass 0
		    } else {
			# skip the term searched
			puts $outfile [lrange $scandata 2 end]
		    }
		}
	    }

	    unset scandata
	    unset errmsg
	    puts $outfile "</INDEX>"
	}
	
	puts $outfile "</INDEXES>"
	puts $outfile "<ALIASES>"
	for {set i 1} {$i < $testindex} {incr i} {
	    
	    if {[llength $dupnames($i)] > 1} {
		puts -nonewline $outfile "<ALIAS INDEX=\""
		if {[llength [lindex $dupnames($i) 0]] == 2} {
		    puts -nonewline $outfile [lindex [lindex $dupnames($i) 0] 0]
		} else {
		    puts -nonewline $outfile [lrange [lindex $dupnames($i) 0] 0 [expr [llength [lindex $dupnames($i) 0]] - 1]]
		}
		puts -nonewline $outfile "\" USE=\""
		puts -nonewline $outfile [lindex [lindex $dupnames($i) 0] end]
		puts $outfile "\">"

		foreach dn [lrange $dupnames($i) 1 end] {
		    if {[llength $dn] == 2} {
			puts $outfile "<ALTNAME USE=\"[lindex $dn end]\">[lindex $dn 0]</ALTNAME>"
		    } else {
			puts $outfile "<ALTNAME USE=\"[lindex $dn end]\">[lrange $dn 0 [expr [llength $dn]-1]]</ALTNAME>"
		    }
		}
		puts $outfile "</ALIAS>"
	    }
	}
	puts $outfile "</ALIASES>"
	puts $outfile "<ATTRIBCOMBO DEFAULTATTRSET=\"$COMBOS($dbname..DEFAULTATTR)\">"
	
	foreach combo $COMBOS($dbname) {
	    regsub -all { } $combo "," combo
	    puts $outfile "<COMBO>$combo</COMBO>"
	} 
	puts $outfile "</ATTRIBCOMBO>"
	puts $outfile "</CDB>"
	unset dupnames
	close $outfile
	set db_end_time [clock seconds]
	puts "Elapsed time for $dbname [expr $db_end_time - $db_start_time]"
	puts $LOGFILE "Elapsed time for $dbname [expr $db_end_time - $db_start_time]"
	.outframe.text insert $timeloc "Elapsed time [expr $db_end_time - $db_start_time] sec.\n" {big color1}
	
	button .outframe.text.[string tolower $dbname] -text "Show Created Collection Record for $dbname" -command "showrec $outname" -cursor top_left_arrow -font labelfont
	.outframe.text window create end -window .outframe.text.[string tolower $dbname]
	.outframe.text insert end " \n"
	.outframe.text insert end " \n" {big color1}

    }
}


proc setstyles {w} {

    $w tag configure bold -font {Helvetica 12 bold italic}
    $w tag configure bold2 -font {Helvetica 14 bold}
    $w tag configure big -font {Helvetica 20 bold}
    $w tag configure verybig -font {Helvetica 24 bold}
    if {[winfo depth $w] > 1} {
	$w tag configure color1 -background #a0b7ff
	$w tag configure color2 -foreground red
	$w tag configure color3 -background RoyalBlue1 -foreground white
	$w tag configure raised -relief raised -borderwidth 1
	$w tag configure sunken -relief sunken -borderwidth 1
    } else {
	$w tag configure color1 -background black -foreground white
	$w tag configure color2 -background black -foreground white
	$w tag configure color3 -background black -foreground white
	$w tag configure raised -background white -relief raised \
		-borderwidth 1
	$w tag configure sunken -background white -relief sunken \
		-borderwidth 1
    }
    $w tag configure bgstipple -background black -borderwidth 0 \
	    -bgstipple gray12
    $w tag configure fgstipple -fgstipple gray50
    $w tag configure underline -underline on
    $w tag configure overstrike -overstrike on
    $w tag configure right -justify right
    $w tag configure center -justify center
    $w tag configure super -offset 4p -font {Courier 10}
    $w tag configure sub -offset -2p -font {Courier 10}
    $w tag configure margins -lmargin1 12m -lmargin2 6m -rmargin 10m
    $w tag configure spacing -spacing1 10p -spacing2 2p \
	    -lmargin1 12m -lmargin2 6m -rmargin 10m
}


proc showrec {filename} {
    set f [open $filename r]
    set w .twind
    catch {destroy $w}
    toplevel $w
    wm title $w "Collection Record Display"
    wm iconname $w "Collection Rec"

    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Dismiss -command "destroy $w"
    pack $w.buttons.dismiss -side left -expand 1

    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -width 70 \
	    -height 45 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    setstyles $w.f.text

    while {[gets $f inline] > 0} { 
	$t insert end "$inline\n" bold
    }
}

proc quit {} {
    global start_time LOGFILE
    
    destroy .title .sysdata
    set end_time [clock seconds]
    puts "Elapsed time [expr $end_time - $start_time]"
    puts $LOGFILE "Elapsed time [expr $end_time - $start_time]"
    close $LOGFILE
    exit
}

set hostname {}
set port "210"
set singleDB {}
set dirname [pwd]

set start_time [clock seconds]
set timestring [clock format $start_time -format "%H %M %S"]
#puts "Start time $timestring"
puts $LOGFILE "Start time $timestring"
#build_virtual_db  [lindex $argv 0] [lindex $argv 1] [lindex $argv 2]

font create titlefont -family Helvetica -size 22 -weight bold -slant roman
font create labelfont -family Helvetica -size 14 -weight bold -slant roman
wm title . "Distributed Collection Scanner" 
label .title -text "Distributed Collection Scanner" -font titlefont -relief raised 
pack .title -side top -fill both
frame .sysdata
label .sysdata.sysname -text "Host Address" -padx 1m  -font labelfont
label .sysdata.sysport -text "Z39.50 Port" -padx 1m  -font labelfont
label .sysdata.dbname -text "Database Name (opt.)" -padx 1m -font labelfont
label .sysdata.direct -text "Output Directory" -padx 1m -font labelfont
button .sysdata.quitbutton -text "Quit" -command "quit"  -font labelfont
grid .sysdata.sysname .sysdata.sysport .sysdata.dbname .sysdata.direct .sysdata.quitbutton
entry .sysdata.host -textvariable hostname
entry .sysdata.port -textvariable port
entry .sysdata.db -textvariable singleDB
entry .sysdata.dirname -textvariable dirname
button .sysdata.gobutton -text "Go" -command {build_virtual_db $hostname $port $dirname} -font labelfont -background RoyalBlue1 -foreground white
grid .sysdata.host .sysdata.port .sysdata.db .sysdata.dirname .sysdata.gobutton -padx 1m
pack .sysdata -side top

frame .outframe

text .outframe.text -yscrollcommand ".outframe.scroll set" -setgrid true \
	-width 120 -height 48 -wrap word
scrollbar .outframe.scroll -command ".outframe.text yview"
pack .outframe.scroll -side right -fill y
pack .outframe.text -expand yes -fill both
pack .outframe -side top

# Set up display styles.

if {[winfo depth .outframe] > 1} {
    set bold "-background #43ce80 -relief raised -borderwidth 1"
    set normal "-background {} -relief flat"
} else {
    set bold "-foreground white -background black"
    set normal "-foreground {} -background {}"
}
# Set up display styles
setstyles .outframe.text

#exit







