# This routine displays the trace info for the SDLIP client

#  -- create the full text window and controls...

set first_meta_pass 1
set searchinterface_pending 0
set resultaccessinterface_pending 0
set metadatainterface_pending 0
set subcol_flag 0
set subcolname_flag 0
set subcoldesc_flag 0
set sprop_pending 0


proc MetaHandleStartTag {name attlist args} {
    global xmlnamespaces
    global response_flag 
    global last_data prop_pending sprop_pending
    global href_pending current_tag current_ns
    global prop_list total_pending recnum
    global searchinterface_pending resultaccessinterface_pending 
    global metadatainterface_pending subcol_flag subcolname_flag 
    global subcoldesc_flag

    

    foreach {attname attval} $attlist {
	if {[string match "xmlns:*" $attname] == 1} {
	    set xmlnamespaces([lindex [split $attname :] 1]) $attval
	}
    }

    set nlist [split $name :]
    if {[llength $nlist] == 1} {
	set tagname $nlist
	set namesp ""
    } else {
	set tagname [lindex $nlist 1]
	set namesp [lindex $nlist 0]
    }

    set current_tag $tagname
    set current_ns  $namesp

    if {[llength $args] > 0} {
	# Empty tags reported here
	# puts "additional args = $args"
    }

    switch -regexp $tagname {
	"multistatus" {
#	    puts "multistatus"
	    if {$recnum > 0} {
		.metawind.f.text insert end "\n==================================================================================\nResponse Record #$recnum\n" red
	    }
	    incr recnum

	}

	"^SDLIPVersion$" {
	}

	"^version$" {
	    set prop_pending 1
	}

	"SearchInterface" {
	    set searchinterface_pending 1
	}

	"ResultAccessInterface" {
	    set resultaccessinterface_pending 1
	}

	"MetadataInterface" {
	    set metadatainterface_pending 1
	}

	"defaultSubcol" {
	    .metawind.f.text insert end "\ndefault Subcollection\n" red
	}

	"subcolInfo" {
	}

	"^subcol$" {
	    .metawind.f.text insert end "\n=================================================" blue
	    set subcol_flag 1
	}
	"subcolName" {
	    .metawind.f.text insert end "\nSubCollection Name" blue
	    set subcolname_flag 1
	}
	"subcolDesc" {
	    .metawind.f.text insert end "\nSubCollection Description" blue
	    set subcoldesc_flag 1
	}
	"queryLangs" {
	    .metawind.f.text insert end "\nQuery Languages" blue
	    set prop_pending 1
	}
	"propList" {
	    .metawind.f.text insert end "\nSubcollection Properties:\n" blue
	    set sprop_pending 1
	}

	"searchable" {
	    .metawind.f.text insert end "searchable\n" 
	}
	"retrievable" {
	    .metawind.f.text insert end "retrievable\n"
	}
	
	
	default {
	  
	    if {$prop_pending == 1} {
		
		.metawind.f.text insert end ":     $current_tag  Namespace: $current_ns"
	    } elseif {$sprop_pending == 1} {
		
		.metawind.f.text insert end "\nProperty: " blue
                .metawind.f.text insert end "$current_tag " red
                .metawind.f.text insert end "Namespace: $current_ns \n" 
	    } else {
		.metawind.f.text insert end "Unknown tag: $current_tag  Namespace: $current_ns" red

	    }

	}
	
    }
    
}

proc MetaHandleEndTag {name args} { 
    global xmlnamespaces
    global response_flag 
    global last_data prop_pending
    global href_pending current_tag current_ns
    global clientsid stateTimeOutReq queryoptions
    global searchinterface_pending resultaccessinterface_pending 
    global metadatainterface_pending subcol_flag subcolname_flag 
    global subcoldesc_flag
    
    set nlist [split $name :]
    if {[llength $nlist] == 1} {
	set tagname $nlist
	set namesp ""
    } else {
	set tagname [lindex $nlist 1]
	set namesp [lindex $nlist 0]
    }
    #    puts "End Tag </$tagname> (namespace $namesp = $xmlnamespaces($namesp))"
    
    if {[llength $args] > 0} {
	# Empty tags reported here
	#	puts "additional args = $args"
    }
    
    switch -regexp $tagname {
	"multistatus" {
	    #	    puts "multistatus"
	}
	
	"^SDLIPVersion$" {
	}

	"^version$" {
	    set prop_pending 0
	}

	"SearchInterface" {
	    set searchinterface_pending 0
	}

	"ResultAccessInterface" {
	    set resultaccessinterface_pending 0
	}

	"MetadataInterface" {
	    set metadatainterface_pending 0
	}

	"subcolInfo" {
	}

	"^subcol$" {
	    set subcol_flag 0
	}
	"subcolName" {
	    set subcolname_flag 0
	}
	"subcolDesc" {
	    set subcoldesc_flag 0
	}
	"queryLangs" {
	    set prop_pending 0
	}
	"propList" {
	    set sprop_pending 0
	}
	
	default {
	    # puts "unknown end tag: $tagname"
	}
	
    }
    
}

proc MetaHandleData {data} {
    global last_data current_attrval lit_pending href_pending limit_flag from
    global current_tag current_ns last_tag total_pending nHits nRetr
    global numRecsReq 
    global metadatainterface_pending subcol_flag subcolname_flag 
    global subcoldesc_flag

    set data [string trim $data " \n\t"]

    if {[string length $data] > 0} {
	if {$subcolname_flag == 1} {
	    .metawind.f.text insert end ":    " blue
	    .metawind.f.text insert end "$data" hlink
	} else {
	    .metawind.f.text insert end ":    $data"
	}
    }
    set last_tag $current_tag
    set last_data $data
}

proc subcolmetadata {data} {
    global first_meta_pass mparser nHits nRetr recnum
    global currServer sdlip_trace

    set lineno [lindex [split $data .] 0]
    set line [.metawind.f.text get hot.first hot.last]
    set x_start [string first "    " $line]
    set x [string trim [string range $line $x_start end] " "]
    # puts "invoking subcolmetadata on $x"

    .metawind.f.text configure -state normal
    .metawind.f.text delete 1.0 end 
    .metawind.f.text insert end "\nNew Search for Subcollection"
    .metawind.f.text insert end " $x:\n" red

    pActionLabelOn "Searching" 1 flash

    set search "<s:getPropertyInfo xmlns:s=\"http://interlib.org/SDLIP/1.0#\">
<s:subcolName> $x </s:subcolName></s:getPropertyInfo>"

    set header2 "Content-Encoding: utf-8"
    set header3 "Content-Length: [string length $search]"
    set header4 "Content-Type: text/xml"

    set serverinfo [split $currServer :]
    set serverAddress [lindex $serverinfo 0]
    set serverPort [lindex $serverinfo 1]
    set serverPath [lindex $serverinfo 2]

    if {$sdlip_trace} {
	.twind.f.text insert end "Metadata Search serverAddress: $serverAddress\n"
	.twind.f.text insert end "Metadata Search serverPort: $serverPort\n"
	.twind.f.text insert end "Metadata Search serverPath: $serverPath\n"
    }

    .metawind.f.text insert end "Metadata Search serverAddress: $serverAddress\n"
    .metawind.f.text insert end "Metadata Search serverPort: $serverPort\n"
    .metawind.f.text insert end "Metadata Search serverPath: $serverPath\n"

    set header1 "SEARCH /$serverPath HTTP/1.1"
    
    set sock [socket $serverAddress $serverPort]

    puts $sock "$header1"
    puts $sock "$header2"
    puts $sock "$header3"
    puts $sock "$header4"
    puts $sock ""
    puts $sock "$search"
    flush $sock

    if {$sdlip_trace} {
	.twind.f.text insert end "\n******************************************* \n"
	.twind.f.text insert end "Metadata QUERY:\n"
	.twind.f.text insert end "$header1 \n"
	.twind.f.text insert end "$header2 \n"
	.twind.f.text insert end "$header3 \n"
	.twind.f.text insert end "$header4 \n"
	.twind.f.text insert end "\n"
	.twind.f.text insert end "$search \n"
	.twind.f.text insert end "\n******************************************* \n"
    }


    # now turn around and wait for results...
    # grab the http headers...
    set line 1
    set err 0
    set nRetr 0

    if {$sdlip_trace} {
	.twind.f.text insert end "Results from Query:\n"
    }
    while {[gets $sock instuff] > 0} {
	incr line	
	if {$sdlip_trace} {
	    .twind.f.text insert end "$instuff\n"
	}
	if {$line == 1} {
	    if {[string match "HTTP/1.1 207 Multi-Status" $instuff] == 1} {
		#    puts $outfile "Matched Search header: $instuff"
	    } else {
		set err 1
		# error handling....
		$t configure -state normal
		$t insert end  "Search not successful:\n"
		$t insert end "$instuff \n"
		while {[gets $sock instuff] >= 0}  {
		    .metawind.f.text insert end "$instuff \n"
		}
		.metawind.f.text configure -state disabled
	    }
	} else {
	    set type [string trim [lindex [split $instuff :] 0] " "]
	    set value [string trim [lindex [split $instuff :] 1] " "]
	    set header($type) $value
	}
    }

    set maxheaders $line
    # we get the length of the message from the header...
    set verify [catch {set content_length $header(Content-Length)}]

    if {$verify == 0} {
	# Use content-length to read the data
	set message [read $sock $content_length]
    } else {
	set message {}
	while {[gets $sock instuff] >= 0} {
	    incr line	
	    if {$sdlip_trace} {
		.twind.f.text insert end "$instuff\n"
	    }
	    append message $instuff
	}
    }
    if {$sdlip_trace} {
	.twind.f.text insert end "\n$message\n"
    }

    if {$err == 0} {
	displayMetadata $message
    } else {
	.metawind.f.text configure -state normal
	.metawind.f.text insert end "Search not successful:"
	.metawind.f.text insert end $message
	.metawind.f.text configure -state disabled
	if {$sdlip_trace} {
	    .twind.f.text insert end "\nSearch not successful\n"
	}
    }

    pActionLabelOn "Inactive" 0 noflash


}

proc displayMetadata {data} {
    global first_meta_pass mparser nHits nRetr recnum

    if {$first_meta_pass} {

	set mparser [xml::parser mparser]
	$mparser configure -elementstartcommand MetaHandleStartTag
	$mparser configure -elementendcommand MetaHandleEndTag
	$mparser configure -characterdatacommand MetaHandleData
	$mparser configure -reportempty 1
	set first_meta_pass 0
    } else {
	$mparser reset
    }
    set recnum 0

    .metawind.f.text configure -state normal
    .metawind.f.text tag configure blue -foreground blue
    .metawind.f.text tag configure red -foreground red
    .metawind.f.text tag configure hlink \
	-foreground blue -underline 1
    .metawind.f.text tag configure visited \
	-foreground "#303080" -underline 1
    .metawind.f.text tag configure hot -foreground red -underline 1

    .metawind.f.text tag bind hlink <ButtonRelease-1> {
	subcolmetadata [.metawind.f.text index {@%x,%y linestart}]
    }

    set lastLine ""
    .metawind.f.text tag bind hlink <Enter> {
	set lastLine [.metawind.f.text index {@%x,%y linestart}]
	
	.metawind.f.text tag add hot "$lastLine +0 chars" "$lastLine lineend -0 chars"
	.metawind.f.text config -cursor hand2
    }
    .metawind.f.text tag bind hlink <Leave> {
	.metawind.f.text tag remove hot 1.0 end
	.metawind.f.text config -cursor xterm
    }
    .metawind.f.text tag bind hlink <Motion> {
	set newLine [.metawind.f.text index {@%x,%y linestart}]
	if {[string compare $newLine $lastLine] != 0} {
	    .metawind.f.text tag remove hot 1.0 end
	    set lastLine $newLine

	    set tags [.metawind.f.text tag names {@%x,%y}]
	    set i [lsearch -glob $tags demo-*]
	    if {$i >= 0} {
		.metawind.f.text tag add hot "$lastLine +1 chars" "$lastLine lineend -1 chars"
	    }
	}
    }


    if {[catch {$mparser parse $data} err] != 0} {

	pDialog .d {XML Parsing Error} \
	    "XML Parsing Error. The data returned from the server contains an XML error: $err." \
	    {error} 0 OK
	set ERROR "User submitted search with no server."
	return "";

    } 

    .metawind.f.text configure -state disabled
  
}



proc createMetadataWindow {} {
    global currServer sdlip_trace

    set textFont "-adobe-courier-bold-r-normal--17-120-*-*-*-*-*-*"

    set w .metawind
    catch {destroy $w}
    
    toplevel $w
    wm title $w "Metadata Window"
    wm iconname $w "Metadata"
    wm geometry $w +10+10
    frame $w.buttons
    pack $w.buttons -side bottom -fill x -pady 2m
    button $w.buttons.dismiss -text Close -command "destroy $w"
    pack $w.buttons.dismiss -side left -expand 1
    
    frame $w.f -highlightthickness 2 -borderwidth 2 -relief sunken
    set t $w.f.text
    text $t -yscrollcommand "$w.scroll set" -setgrid true -font $textFont \
         -width 50 -height 30 -wrap word -highlightthickness 0 -borderwidth 0
    pack $t -expand  yes -fill both
    scrollbar $w.scroll -command "$t yview"
    pack $w.scroll -side right -fill y
    pack $w.f -expand yes -fill both
    $t tag configure center -justify center -spacing1 5m -spacing3 5m
    $t tag configure red -foreground red
    $t tag configure blue -foreground blue
    $t tag configure buttons -lmargin1 1c -lmargin2 1c -rmargin 1c \
	    -spacing1 3m -spacing2 0 -spacing3 0

    # get the metadata for the current database
    if {[string length $currServer] == 0} {
	
	pDialog .d {No Server Selected} \
	    "Cannot submit metadata search. You must select a server from the server menu before searching." \
	    {error} 0 OK
	set ERROR "User submitted search with no server."
	return "";
    }

    .metawind.f.text insert end "\nNew Search:\n"

    pActionLabelOn "Searching" 1 flash

    set search "<s:getSubcollectionInfo xmlns:s=\"http://interlib.org/SDLIP/1.0#\"/>"
    
    set header2 "Content-Encoding: utf-8"
    set header3 "Content-Length: [string length $search]"
    set header4 "Content-Type: text/xml"



    set serverinfo [split $currServer :]
    set serverAddress [lindex $serverinfo 0]
    set serverPort [lindex $serverinfo 1]
    set serverPath [lindex $serverinfo 2]

    if {$sdlip_trace} {
	.twind.f.text insert end "Metadata Search serverAddress: $serverAddress\n"
	.twind.f.text insert end "Metadata Search serverPort: $serverPort\n"
	.twind.f.text insert end "Metadata Search serverPath: $serverPath\n"
    }

    $t insert end "Metadata Search serverAddress: $serverAddress\n"
    $t insert end "Metadata Search serverPort: $serverPort\n"
    $t insert end "Metadata Search serverPath: $serverPath\n"

    set header1 "SEARCH /$serverPath HTTP/1.1"
    
    set sock [socket $serverAddress $serverPort]

    puts $sock "$header1"
    puts $sock "$header2"
    puts $sock "$header3"
    puts $sock "$header4"
    puts $sock ""
    puts $sock "$search"
    flush $sock

    if {$sdlip_trace} {
	.twind.f.text insert end "\n******************************************* \n"
	.twind.f.text insert end "Metadata QUERY:\n"
	.twind.f.text insert end "$header1 \n"
	.twind.f.text insert end "$header2 \n"
	.twind.f.text insert end "$header3 \n"
	.twind.f.text insert end "$header4 \n"
	.twind.f.text insert end "\n"
	.twind.f.text insert end "$search \n"
	.twind.f.text insert end "\n******************************************* \n"
    }


    # now turn around and wait for results...
    # grab the http headers...
    set line 1
    set err 0
    set nRetr 0

    if {$sdlip_trace} {
	.twind.f.text insert end "Results from Query:\n"
    }
    while {[gets $sock instuff] > 0} {
	incr line	
	if {$sdlip_trace} {
	    .twind.f.text insert end "$instuff\n"
	}
	if {$line == 1} {
	    if {[string match "HTTP/1.1 207 Multi-Status" $instuff] == 1} {
		#    puts $outfile "Matched Search header: $instuff"
	    } else {
		set err 1
		# error handling....
		$t configure -state normal
		$t insert end  "Search not successful:\n"
		$t insert end "$instuff \n"
		while {[gets $sock instuff] >= 0}  {
		    $t insert end "$instuff \n"
		}
		$t configure -state disabled
	    }
	} else {
	    set type [string trim [lindex [split $instuff :] 0] " "]
	    set value [string trim [lindex [split $instuff :] 1] " "]
	    set header($type) $value
	}
    }

    set maxheaders $line
    # we get the length of the message from the header...
    set verify [catch {set content_length $header(Content-Length)}]

    if {$verify == 0} {
	# Use content-length to read the data
	set message [read $sock $content_length]
    } else {
	set message {}
	while {[gets $sock instuff] >= 0} {
	    incr line	
	    if {$sdlip_trace} {
		.twind.f.text insert end "$instuff\n"
	    }
	    append message $instuff
	}
    }
    if {$sdlip_trace} {
	.twind.f.text insert end "\n$message\n"
    }

    if {$err == 0} {
	displayMetadata $message
    } else {
	$t configure -state normal
	$t insert end "Search not successful:"
	$t insert end $message
	$t configure -state disabled
	if {$sdlip_trace} {
	    .twind.f.text insert end "\nSearch not successful\n"
	}
    }

    pActionLabelOn "Inactive" 0 noflash

}



