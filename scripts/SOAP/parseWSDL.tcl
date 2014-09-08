
# We should already have required the SOAP package
#package require SOAP
#set filename "wsdl.xml"


set filename "GoogleSearch.wsdl"

set inh [open $filename r]
set xml [read $inh]
close $inh

set doc [dom::DOMImplementation parse $xml]
set defn [SOAP::selectNode $doc "/definitions"]
set uri [dom::element getAttribute $defn "targetNamespace"]

# Get all <message> elements (children of definition)

proc parseMessage {node} {
   # first get name
    set msgname [dom::element getAttribute $node "name"]
    
    # now iter over <part>s
    set partList [list]
    set childNodes [dom::node children $node]
    foreach child $childNodes {
	set cname [dom::node cget $child -nodeName]
	if {$cname == "part"} {
	    set name [dom::element getAttribute $child "name"]
	    set type [dom::element getAttribute $child "type"]
	    regexp ":(.+)$" $type null partType
	    lappend partList $name $partType
	}
    }
    return [list $msgname $partList]
}

proc parseOperation {node} {
    set opName [dom::element getAttribute $node "name"]
    set childNodes [dom::node children $node]
    foreach child $childNodes {
	set cname [dom::node cget $child -nodeName]
	if {$cname == "input"} {
	    set message [dom::element getAttribute $child "message"]
	    regexp ":(.+)$" $message null opMessage
	    return [list $opName $opMessage]
	}
    }
}

proc parseService {node} {
    # Find port, then (soap:)address
    set childNodes [dom::node children $node]
    foreach child $childNodes {
	set cname [dom::node cget $child -nodeName]
	if {$cname == "port"} {
	    set portChildren [dom::node children $child]
	    foreach portChild $portChildren {
		set pcname [dom::node cget $portChild -nodeName]
		if {$pcname == "address"} {
		    return [dom::element getAttribute $portChild "location"]
		}
	    }
	}
    }
}

#We need:  messages, which are 'input', and service endpoint

array set messages [list]
array set inputMessages [list]
set endpoint ""

set childNodes [dom::node children $defn]
foreach child $childNodes {
  set cname [dom::node cget $child -nodeName]   
  if {$cname == "message"} {
      set np [parseMessage $child ]
      set name [lindex $np 0]
      set parts [lindex $np 1]
      set messages($name) $parts
  } elseif {$cname == "portType"} {
      # Find input messages
      set opNodes [dom::node children $child]
      foreach opChild $opNodes {
	  set opNodeName [dom::node cget $opChild -nodeName]
	  if {$opNodeName == "operation"} {
	      set tm [parseOperation $opChild]
	      set type [lindex $tm 0]
	      set message [lindex $tm 1]
	      set inputMessages($type) $message
	  }
      }
 } elseif {$cname == "service"} {
     set endpoint [parseService $child]
 }
}

set soapProcs [list]

foreach name [array names inputMessages] {
    set inputName $inputMessages($name)
    set message $messages($inputName)

    #Should this be name or inputName?
    SOAP::create $inputName -uri $uri -proxy $endpoint -params $message
    lappend soapProcs $inputName    
}
