#!/home/cheshire/cheshire/bin/webcheshire

# Check if we have mail. If so, write to sgml file and reindex.
# Need to support pop3 and direct from file
# To be run every day or so from cron (Or a sleeping shell script)
# Notes:  For a mailing list, the fromaddress should always be the same (ie the list)
# To filter only the messages from a list on a real account, check on $user and $host

#--- Configuration Variables ---

set USE_POP 0
set POP_NAME "cheshire"
set POP_PASS "fill_in_password"
set MAILFILE "/var/spool/mail/cheshire"
set SGMLFILE "quickstep.sgml"
set DELETE_MAILFILE 0
set DO_REINDEX 0

# --- End of Configuration ---

set header_list {"Return-Path" "Received" "Date" "Sender" "To" "Subject" "Message-ID" "MIME-Version" "Content-Type" "Status" "X-Status" "X-Keywords" "X-UID" "X-Authentication-Warning" "Message-Id" "CC" "In-reply-to" "References" "Precedence" "X-Mailer" "Organization" "X-Sender"}
set record {"Return_Path" "Date" "Sender" "To" "Subject" "From"}

proc do_index_cheshire {newrecs} {

    set LRINH [open "last_record_number"]
    set last_record [gets $LRINH]
    close $LRINH

    puts stdout "Rebuilding... this may take a while."
    exec "./rebuild.sh" [expr $last_record + 1]

    puts stdout "Done."
    set LROUTH [open "last_record_number" w]
    puts $LROUTH [expr $last_record + $newrecs]
    close $LROUTH

}

proc sgmlise {text fillout} {

    if {$text == "" && $fillout == 1} {
	return "Null"
    }

    # Need to check for all non letter/number characters. How?
    if {$fillout == 1 && [regexp {^(\[ :\(\);!-=.;/\\<>\ ])*$} $text null]} {
	print stdout "Failed to find text in '$text'"
	append text "(null)"
    }

    regsub -all {&} $text {\&amp;} text
    regsub -all {>} $text {\&gt;} text
    regsub -all {<} $text {\&lt;} text
    regsub -all "\n" $text "" text
    return $text
}

proc process_message {} {
    global body user host fromdate record idx SGMLFILE
    foreach var $record {
	global $var
    }

    puts  -nonewline "."
    flush stdout

    set sgml "<message>\n"

    append sgml "  <fromaddress>[sgmlise $user 1]@[sgmlise $host 1]</fromaddress>\n"
    append sgml "  <fromdate>[sgmlise $fromdate 1]</fromdate>\n"
    append sgml "  <sender>[sgmlise $Sender 1]</sender>\n"
    append sgml "  <from>[sgmlise $From 1]</from>\n"
    append sgml "  <date>[sgmlise $Date 1]</date>\n"
    append sgml "  <returnpath>[sgmlise $Return_Path 1]</returnpath>\n"
    append sgml "  <to>[sgmlise $To 1]</to>\n"
    append sgml "  <subject>[sgmlise $Subject 1]</subject>\n"
    append sgml "\n  <body>\n"
    
    foreach line $body {
	append sgml "[sgmlise $line 0]\n"
    }

    append sgml "  </body>\n"
    append sgml "</message>\n"

    set OUTH [open "$SGMLFILE" a]
    puts $OUTH $sgml
    close $OUTH
}


set message_idxs {}

if {$USE_POP == 0} {
    # Read from file
    if { [ file exists $MAILFILE ] && [file size $MAILFILE] > 0} {
	set INH [open $MAILFILE r]
	set text [read $INH]
	close $INH

	# Process $text into $messages
	set textlines [split $text "\n"]
	set length [llength $textlines]
	set in_body 0
	set recording 1
	foreach var $record {
	    set $var ""
	}
	
 	for {set idx 0} {$idx < $length} {incr idx} {
	    set line [lindex $textlines $idx];

	    if {[regexp {^From ([^ ]+)@([^ ]+)( )+(.+)$} $line null user host nullb fromdate] } {
		#start new message
		if {$idx != 0} {
		    process_message
		}
		lappend message_idxs $idx
		set in_body 0
		set recording 1
		set body {}
		foreach var $record {
		    set $var ""
		}
	    } elseif {[regexp {^From ([^ ]+)( )+(.+)$} $line null user nullb fromdate] } {
		set host "none"
		if {$idx != 0} {
		    process_message
		}
		lappend message_idxs $idx
		set in_body 0
		set recording 1
		set body {}
		foreach var $record {
		    set $var ""
		}
	    } elseif {[regexp {^--------------([0-9A-Z]+)$} $line null id] } {
		set recording 0
	    } elseif {[regexp -nocase {^Content-Transfer-Encoding: base64} $line null]} {
		set recording 0
	    } elseif {[ regexp -nocase {^Content-Disposition: attachment} $line null]} {
		set recording 0
	    } elseif {$line == "" && $in_body == 0} {
		set in_body 1
	    } elseif {$in_body == 1 && $recording == 1} {
		lappend body $line
	    } elseif {$recording == 0} {
		#ignore attachment line
	    } else {
		foreach hvar $record {
		    regsub -all "_" $hvar "-" header
		    if {[string range $line 0 [string length $header]] == "$header:"} {
			set $hvar [string range $line [expr [string length $header] +2] end]
		    }
		}
	    }
	}
	process_message
	if {$DO_REINDEX == 1} {
	  do_index_cheshire [llength $message_idxs]
	}
        if {$DELETE_MAILFILE == 1} {
	    # Can't actually delete /var/spool/mail files. Just zero it
	    puts stdout "Flushing mail file."
	    exec "cat" "/dev/null" ">" $MAILFILE
	}
	
    } else {
	#No mail, nothing to do.
    }
} else {
    # Do POP query
    # This is pretty easy to write, I'll do it later. (uhh huh)

}

