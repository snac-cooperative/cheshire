#!/home/cheshire/cheshire/bin/webcheshire
#No faff implementation ported from my MOO-SOAP

set handlerdir "/home/cheshire/public_html/cgi-bin/soap/"
set namespace "AzSOAP"
set namespaceURI "http://www.o-r-g.org/~cheshire/"

# SOAP will import all the xml stuff
package require SOAP
package require SOAP::Utils

# Turn XML into list with types.  Stupid TCL without proper typing :P
proc deserialise {elem} {
    set result {}

    set childNodes [SOAP::Utils::getElements $elem]
    if {[llength $childNodes] == 0} {
	# single value
	set name [dom::node cget $elem -nodeName]
	set type [dom::element getAttribute $elem "xsi:type"]
	set value [SOAP::Utils::getElementValue $elem]

	# Any futher deserialisation here

	set result [list $name $type $value]
    } else {
	set result [list [dom::node cget $elem -nodeName] ""]
	set temp [list]
	foreach child $childNodes {
	    lappend temp [deserialise $child]
	}
	lappend result $temp
    }
    
    return $result

}



# Return empty header by default.  Override in sourced files.
# Cunning, neh?
proc construct_soap_headers {} {
    return "<SOAP-ENV:Header></SOAP-ENV:Header>"
}
# Also override namespace and namespaceURI in sourced files for this function
proc package_without_namespace {} {
    return 0
}

# Override for specific header handlers
# Step over headers and look for must understand=1. If yes, fail.
proc handle_soap_header {element} {
    return 0
}
proc handle_soap_headers {} {

    set headersInfo [list]
    foreach hdr [selectNode $doc "/Envelope/Header/*"] {
	set mustUnderstand [SOAP::Utils::getElementAttribute $hdr mustUnderstand]
	set hdrName [dom::node cget $hdr -nodeName]
	if {[set hdrinfo [handle_soap_header $hdr]] == 0 && $mustUnderstand == 1} {
	    error "MustUnderstand" $hdrName
	} else {
	    lappend headersInfo [list $hdrName $hdrInfo]
	}
    }
    return $headersInfo
}



# Take XML, namespace it if not namespaced and desired, wrap in envelope
proc package_to_send {xml {noNS 0}} {
    global namespace namespaceURI

    set soap "<?xml version='1.0'?>\n<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">\n"
    append soap [construct_soap_headers]
     append soap "\n<SOAP-ENV:Body xmlns:$namespace=\"$namespaceURI\">"
    
    if {$noNS == 0} {
	regsub -all {<([^/: ])} $xml "<$namespace:\\1" xml
	regsub -all "</" $xml "</$namespace:" xml
    }

    append soap $xml

    append soap "</SOAP-ENV:Body>\n</SOAP-ENV:Envelope>"
    return $soap

}



# package errors in SOAP.  Take type and detail, return XML
# If type has a : in it, leave as is.  Otherwise prepend SOAP-ENV
proc package_error {type {message ""} {full ""} {inHeader 0}} {
    
    set xml "<SOAP-ENV:Fault>"

    if { [string first ":" $type] == -1 } {
	set type "SOAP-ENV:$type"
    }
    append xml "<faultcode>$type</faultcode>"
    append xml "<faultstring>$message</faultstring>"

    if {$inHeader == 0} {
	append xml "<detail>"
	if {$full != ""} {
	    append xml "<tcl:traceback xmlns:tcl=\"http://www.tcl.tk/\">$full</tcl:traceback>"
	}
	append xml "</detail>"
    }

    append xml "</SOAP-ENV:Fault>"
    return $xml
}



# --- Main Script ---

# I hate using $env()  =)
# SOAPAction Mandatory, but not always provided ;P
set HTTP_SOAPACTION ""
foreach key [array names env] {
    set $key $env($key)
} 

set err 0
set inxml [read stdin $CONTENT_LENGTH]
set doc [dom::DOMImplementation parse $inxml]

set body [SOAP::selectNode $doc "/Envelope/Body"]
if {$body == ""} {
    # Unparsable
    set xml [package_error Client "Unparsable Request"]
    set err 1
}

# If not http://schemas.xmlsoap.org/soap/envelope namespace, then fail
# FOO:Envelope xmlns:FOO=(above) 
set envelope [SOAP::selectNode $doc "/Envelope"]
set envns [SOAP::Utils::namespaceURI $envelope]
if { $envns != "http://schemas.xmlsoap.org/soap/envelope/"} {
    set xml [package_error VersionMismatch "Server supports SOAP 1.1"]
    set err 1
}    


set childNodes [SOAP::Utils::getElements $body]

if {[llength $childNodes] != 1} {
    set xml [package_error Client "Unparsable Request"]
    set err 1

} elseif {$err == 0} {
    set child [lindex $childNodes 0]

    set err [catch  { deserialise $child }  structure]
    if {$err != 0} {
	set xml [package_error "Client" "Unparsable Request"]
    } else {


	set request [lindex [lindex $structure 0] 0]
	set file ""
	
	# Look for SOAPAction and request.
	set reqfile "${handlerdir}${request}.tcl"
	set action "${handlerdir}[file rootname [file tail $HTTP_SOAPACTION]].tcl"
	if { [file exists $reqfile] && [file readable $reqfile]} {
	    set file $reqfile
	} elseif { [file exists $action] && [file readable $action] } {
	    set file $action
	} else {
	    # No handler. =(
	    set xml [package_error "Server" "Unsupported Request."]
	    set err 1
	}
	
	if {$file != "" } {
	    set err [ catch { source $file } sourced ]
	    if {$err != 0} {
		# Couldn't source file
		set xml [package_error "Server" "Server Error" $xml]
	    } else {

		# Check if we have headers. If we do, try to handle them
		set header [SOAP::selectNode $doc "/Envelope/Header"]
		if {$header != ""} {
		    set err [catch { handle_soap_headers $header } headerinfo ]
		    if {$err != 0} {
			if {$headerinfo == "MustUnderstand"} {
			    set xml [package_error "MustUnderstand" "Failed to understand $errorInfo"]
			} else {
			    set xml [package_error "Server" $headerinfo]
			}
		    }
		}


		set err [ catch { handle_soap $structure } xml ]
		if {$err != 0} {
		    # Couldn't handle request
		    set xml [package_error "Server" "Server Error" $xml]
		} else {
		    # Allow sourced files to override autonamespacing
		    set err [package_without_namespace]
		}
	    }
	}
    }
}

set xml [ package_to_send $xml $err]

set len [expr [string length $xml] +1]

puts "Content-type: text/xml"
puts "Date: [clock format [clock seconds]]"
puts "SOAPServer: AzTclSOAP/1.0"
puts "Content-length: $len"
puts ""
puts $xml
