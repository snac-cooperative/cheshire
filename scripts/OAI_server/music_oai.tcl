#!/home/ray/Work/cheshire/bin/webcheshire
# 
# Prototype implementation of OAI Server using Cheshire
# This entire implementation was done in the Tcl scripting 
# language for webcheshire.
# 
# Supports direct searching of local databases
# and a Z39.50 gateway to other databases (especially CDL's MELVYL
# and related DBs).
#
# 

set CHESHIRE_DATABASE CSMP
set CHESHIRE_SERVER sherlock.berkeley.edu
set CHESHIRE_PORT 2100

set hits 0
set hittype ""
set errmsg ""
set recvd 0
set indata(NULL) ""


set CURRENTURL ""

set request_uri $env(REQUEST_URI)
regsub -all {&} $request_uri "\&amp;" request_uri

#remaining code sequence at end of this file

# read the HTTP request and set up the indata array
proc GETARGS {} {

    global env indata

    if {[string compare $env(REQUEST_METHOD) "POST"]==0} {
	set message [split [read stdin $env(CONTENT_LENGTH)] &]
    } else {
	set message [split $env(QUERY_STRING) &]
    }
    
    # puts "$message"
    
    foreach pair $message {
	set name [lindex [split $pair =] 0]
	set val [lindex [split $pair =] 1]
	regsub -all {\+} $val { } val
	# kludge to unescape some chars
	regsub -all {\%0A} $val \n\t val
	regsub -all {\%2C} $val {,} val
	regsub -all {\%27} $val {'} val
	regsub -all {\%3F} $val {} val
	regsub -all {\%3A} $val {:} val
	
	set indata($name) "$val"
    }
}

# Submit searches and parse results

proc LIBSEARCH {docid format} {
    global hits hittype recvd errmsg indata
    global CHESHIRE_DATABASE CHESHIRE_SERVER CHESHIRE_PORT
    
    set err [catch {zselect xxx $CHESHIRE_SERVER $CHESHIRE_DATABASE $CHESHIRE_PORT} sresults]
    
    if {$err != 0} {
	# unable to connect to db
	return ""
    }
    
    set query "zfind docid $docid"
    
    if {$format == "oai_dc"} {
	zset recsyntax MARC
    } elseif {$format == "oai_marc"} {
	set format "oai_marc"
	zset recsyntax MARC
    } elseif {$format == "Cheshire_USMARC"} {
	zset recsyntax XML
    } else {
	#default to DC MARC
	set format "oai_dc"
	zset recsyntax MARC
    }
    
    set err [catch {eval $query} qresults]
    
    if {$err != 0} {
	return ""
    }
    
    set err [catch {zdisplay} dresults]
    
    if {$err != 0} {
	return ""
    }
    
    #should only be one record 
    set sresults [lindex $dresults 1]

    if {$format == "oai_dc"} {
	
	set outrec0 [zformat dc $sresults MARC 1 80]
	#since this is the elib server change the hard pointers...
	
	regsub -all {\[|\]|\$[0-9]} $outrec0 {\\&} outrec0
	regsub -all -nocase "\[a-z\]+:creator" $outrec0 "creator" outrec0
	regsub -all -nocase "\[a-z\]+:title" $outrec0 "title" outrec0
	regsub -all -nocase "\[a-z\]+:type" $outrec0 "type" outrec0
	regsub -all -nocase "\[a-z\]+:format" $outrec0 "format" outrec0
	regsub -all -nocase "\[a-z\]+:source" $outrec0 "source" outrec0
	regsub -all -nocase "\[a-z\]+:language" $outrec0 "language" outrec0
	regsub -all -nocase "\[a-z\]+:publisher" $outrec0 "publisher" outrec0
	regsub -all -nocase "\[a-z\]+:relation" $outrec0 "relation" outrec0
	regsub -all -nocase "\[a-z\]+:identifier" $outrec0 "identifier" outrec0
	regsub -all -nocase "\[a-z\]+:date" $outrec0 "date" outrec0
	regsub -all -nocase "\[a-z\]+:coverage" $outrec0 "coverage" outrec0
	regsub -all -nocase "\[a-z\]+:rights" $outrec0 "rights" outrec0
	regsub -all -nocase "\[a-z\]+:subject" $outrec0 "subject" outrec0
	regsub -all -nocase "\[a-z\]+:description" $outrec0 "description" outrec0
	regsub -all -nocase "\[a-z\]+:contributor" $outrec0 "contributor" outrec0
	regsub -all -nocase "\[a-z\]+:identifier" $outrec0 "identifier" outrec0

	# this UTF encoding is required for the verifying explorer 
    } elseif {$format == "oai_marc"} {
	set temp [zformat marc $sresults MARC 1 20000]q
	set outrec0 [marc_to_oai_marc $temp]

    } elseif {$format == "Cheshire_USMARC"} {
	set outrec0 $sresults
    } else {
	set outrec0 ""
    }
    
    set outrec0 [encoding convertto utf-8 $outrec0]
    
    return $outrec0
}

proc numdbdocs {server db} {
    
    set err [catch {zselect md sherlock.berkeley.edu METADATA 2100} sresults]
    set query "zfind numdocs $db"
    catch {eval $query} qresults
    set dresults [zdisplay]
    set ndocs [lindex $dresults 1]
    zclose 
    return $ndocs
}


proc marc_to_oai_marc {inrec} {

    set lines [split $inrec "\n"] 
    set record {}
    append record "<oai_marc status=\"n\" type=\"c\" level=\"m\" catForm=\"a\"" "\n"
    append record "xmlns=\"http://www.openarchives.org/OAI/1.1/oai_marc\"" "\n"
    append record "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" "\n"
    append record "xsi:schemaLocation=\"http://www.openarchives.org/OAI/1.1/oai_marc\n"
    append record "                    http://www.openarchives.org/OAI/1.1/oai_marc.xsd\">" "\n"

    foreach line $lines {
	set tag [string range $line 0 2]
	set ind1 [string range $line 4 4]
	set ind2 [string range $line 5 5]

	if {[string is integer $tag] == 0} {
	   continue
	}	

	#puts "line is '$line'"
	#puts "tag is $tag ind1 = '$ind1' ind2 = '$ind2'"

	if {$tag < "010"} {
	    # This is a fixed field 
	    set fdata [string range $line 7 end]
	    append record "<fixfield id=\"$tag\">\"$fdata\"</fixfield>" "\n"
	} else {
	    # it is a varfield 
	    append record "<varfield id=\"$tag\" i1=\"$ind1\" i2=\"$ind2\">" "\n"
	    set subf [split [string range $line 7 end] "\$"]
	    foreach sub $subf { 
		append record "<subfield label=\"[string range $sub 0 0]\">[string range $sub 1 end]</subfield>"  "\n"
	    }  
	    append record "</varfield>" "\n"
	}
       	
    }

    append record "</oai_marc>" "\n"

    regsub -all "&" $record "&amp;" record

    return $record

}


proc responsedate {} {
    
    set time [clock seconds]
    set local [string trimleft [clock format $time -format "%H"] 0]
    set GMT [string trimleft [clock format $time -format "%H" -gmt 1] 0]
    set diff [expr $GMT - $local]
    if {$diff < 0} {
	set diff [expr ($GMT + 24) - $local]
    }
    set newtime [clock format $time -format "%Y-%m-%dT%H:%M:%S"]
    return [format "%s-%02d:00" $newtime $diff]

}
	
proc GetRecord {} {
    global request_uri indata
    global CHESHIRE_DATABASE
    # puts "GetRecord Command "


    set date [responsedate]
    set outdata {}
    set format "oai_dc"


    catch {set indata(metadataPrefix)} format

    set err [catch {set indata(identifier)} basedocid]
    
    if {$err == 0} {
	# puts "basedocid is $basedocid"
	
	set docid [split $basedocid :]
	
	set database [lindex $docid 1]
	set CHESHIRE_DATABASE [lindex $docid 1]
	set docnum [lindex $docid 2]

     
	switch $database {
	    "CSMP" { 
		set outrecord [LIBSEARCH $docnum $format]
	    }
	}
    } else {
	set outrecord ""
    }
    
    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata { <GetRecord xmlns="http://www.openarchives.org/OAI/1.1/OAI_GetRecord" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_GetRecord     http://www.openarchives.org/OAI/1.1/OAI_GetRecord.xsd">} "\n"
    append outdata "  <responseDate>$date</responseDate>" "\n"
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    
    if {$outrecord != ""} {
	append outdata {  <record>} "\n"
	append outdata {   <header>} "\n"
	append outdata "    <identifier>$basedocid</identifier>" "\n"
	append outdata {    <datestamp>2001-01-05</datestamp>} "\n"
	append outdata {   </header>} "\n"
	append outdata {   <metadata>} "\n"

	if {$format == "oai_dc"} {
	    append outdata {   <dc xmlns="http://purl.org/dc/elements/1.1/"  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://purl.org/dc/elements/1.1/   http://www.openarchives.org/OAI/1.1/dc.xsd">} "\n"
	    append outdata $outrecord "\n"
	    append outdata {    </dc>} "\n"

	} elseif {$format == "oai_marc"} {
	    append outdata $outrecord
	}
	append outdata {   </metadata>} "\n"
	append outdata {  </record>} "\n"
    }
	append outdata { </GetRecord>} "\n"

    
    puts "Content-Type: text/xml"
    puts ""
    

    puts $outdata

}

proc Identify {} {
    global request_uri
    #	puts "Identify Command "
    set date [responsedate]

    set outdata {}

    #output the boilerplate...
    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata {  <Identify xmlns="http://www.openarchives.org/OAI/1.1/OAI_Identify" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_Identify   http://www.openarchives.org/OAI/1.1/OAI_Identify.xsd">} "\n"
    append outdata "    <responseDate>$date</responseDate>"
    
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    
    append outdata {    <repositoryName>California Sheet Music Project at UC Berkeley - Cheshire II Retrieval System</repositoryName>} "\n"
    append outdata {    <baseURL>http://sherlock.berkeley.edu/cgi-bin/musicoai.tcl</baseURL>} "\n"
    append outdata {    <protocolVersion>1.1</protocolVersion>} "\n"
    append outdata {    <adminEmail>mailto:ray@sherlock.berkeley.edu</adminEmail>} "\n"
    append outdata {    <description>} "\n"
    append outdata {      <oai-identifier xmlns="http://www.openarchives.org/OAI/oai-identifier"  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/oai-identifier                 http://www.openarchives.org/OAI/1.1/oai-identifier.xsd">} "\n"
    append outdata {        <scheme>oai</scheme>} "\n"
    append outdata {        <repositoryIdentifier>CSMP</repositoryIdentifier>} "\n"
    append outdata {        <delimiter>:</delimiter>} "\n"
    append outdata {        <sampleIdentifier>oai:CSMP:123</sampleIdentifier>} "\n"
    append outdata {      </oai-identifier>} "\n"
    append outdata {    </description>} "\n"

    append outdata {    <description>} "\n"
    append outdata {      <eprints xmlns="http://www.openarchives.org/OAI/eprints"  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"         xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/eprints    http://www.openarchives.org/OAI/1.1/eprints.xsd">} "\n"
    append outdata {        <content>} "\n"
    append outdata {          <text>Catalog of the California Sheet Music Project (CSMP) on the UC Berkeley Campus.} "\n"
    append outdata {          Descriptions of 19th-century sheet music printed in California.} "\n"
    append outdata {          Search support is available at http://sherlock.berkeley.edu/sheet_music </text>} "\n"
    append outdata {        </content>} "\n"
    append outdata {        <metadataPolicy>} "\n"
    append outdata {          <text>Metadata can be used by commercial and non-commercial service providers </text>} "\n"
    append outdata {        </metadataPolicy>} "\n"
    append outdata {        <dataPolicy>} "\n"
    append outdata {          <text>No current policy</text>} "\n"
    append outdata {        </dataPolicy>} "\n"
    append outdata {        <submissionPolicy>} "\n"
    append outdata {          <text>Only internal submission accepted</text>} "\n"
    append outdata {        </submissionPolicy>} "\n"
    append outdata {      </eprints>} "\n"
    append outdata {    </description>} "\n"
    append outdata {  </Identify>} "\n"

    puts "Content-Type: text/xml"
    puts ""
    puts $outdata

}

proc ListIdentifiers {} {
    global request_uri indata
    #	puts "ListIdentifiers Command "
    set date [responsedate]
    set outdata {}

    set err [catch {set indata(from)} fromdate]
    if {$err == 1} {
	set fromdate ""
    }

    set err [catch {set indata(until)} untildate]
    if {$err == 1} {
	set untildate ""
    }

    
    set err [catch {set indata(set)} setval]
    if {$err == 1} {
	set setval ""
    }
    
    set err [catch {set indata(resumptionToken)} resumetoken]
    if {$err == 1} {
	set resumepoint 1
    } else {
	set rs [split $resumetoken -]
	set db [lindex $rs 1]
	set resumepoint [lindex $rs 2]
    }

    set totaldocs [numdbdocs sherlock.berkeley.edu CSMP]

    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata {  <ListIdentifiers  xmlns="http://www.openarchives.org/OAI/1.1/OAI_ListIdentifiers" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_ListIdentifiers     http://www.openarchives.org/OAI/1.1/OAI_ListIdentifiers.xsd">} "\n"
    append outdata "    <responseDate>$date</responseDate>" "\n"
    
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    
    set endpoint [expr $resumepoint + 200]
    for {set i $resumepoint} {$i < $endpoint} {incr i} {
	append outdata "     <identifier>oai:CSMP:$i</identifier>" "\n"
    }
    append outdata "    <resumptionToken>RS-CSMP-$i</resumptionToken> " "\n"
    append outdata {  </ListIdentifiers>  } "\n"

    puts "Content-Type: text/xml"
    puts ""
    puts $outdata
    
}

proc ListMetadataFormats {} {
    global request_uri indata
    # puts "ListMetadataFormats Command "
    set date [responsedate]
    set rfc 0
    set dc 0
    set marc 0
    set usmarc 0
    set outdata {}
    
    set err [catch {set indata(identifier)} basedocid]

    if {$err == 0} {
	#puts "basedocid is $basedocid"

	set docid [split $basedocid :]

	set database [lindex $docid 1]
	set docnum [lindex $docid 2]

	switch $database {
	    "CSMP" { 
		set dc 1
		set marc 1
		#set usmarc 1
	    }
	}
    } else {
	set dc 1
	set marc 1
	#set usmarc 1
    }

    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata {  <ListMetadataFormats xmlns="http://www.openarchives.org/OAI/1.1/OAI_ListMetadataFormats" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_ListMetadataFormats     http://www.openarchives.org/OAI/1.1/OAI_ListMetadataFormats.xsd">} "\n"
    append outdata "    <responseDate>$date</responseDate> " "\n"
    
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    

    if {$rfc == 1} {
	append outdata {    <metadataFormat>} "\n"
	append outdata {      <metadataPrefix>oai_rfc1807</metadataPrefix>} "\n"
	append outdata {      <schema>http://www.openarchives.org/OAI/rfc1807.xsd</schema>} "\n"
	append outdata {    </metadataFormat>} "\n"
    }
    if {$dc == 1} {
	append outdata {    <metadataFormat>} "\n"
	append outdata {      <metadataPrefix>oai_dc</metadataPrefix>} "\n"
	append outdata {      <schema>http://www.openarchives.org/OAI/1.1/dc.xsd</schema>} "\n"
	append outdata {      <metadataNamespace>http://purl.org/dc/elements/1.1/</metadataNamespace>} "\n"
	append outdata {    </metadataFormat>} "\n"
    }
    if {$marc == 1} {
	append outdata {    <metadataFormat>} "\n"
	append outdata {      <metadataPrefix>oai_marc</metadataPrefix>} "\n"
	append outdata {      <schema>http://www.openarchives.org/OAI/1.1/oai_marc.xsd</schema>} "\n"
	append outdata {    </metadataFormat>} "\n"
    }
    if {$usmarc == 1} {
	append outdata {    <metadataFormat>} "\n"
	append outdata {      <metadataPrefix>Cheshire_USMARC</metadataPrefix>} "\n"
	append outdata {      <schema>NONE: DTD Available</schema>} "\n"
	append outdata {    </metadataFormat>} "\n"
    }
    append outdata {</ListMetadataFormats>} "\n"
    
 #   puts "Content-Type: text/xml"
 #   puts ""
    puts "Content-Type: text/xml"
    puts ""
    puts $outdata

}

proc ListRecords {} {
    global request_uri indata
    #	puts "ListRecords Command "
    set date [responsedate]
    set outdata {}
    
    set err [catch {set indata(from)} fromdate]
    if {$err == 1} {
	set fromdate ""
    }
    
    set err [catch {set indata(until)} untildate]
    if {$err == 1} {
	set untildate ""
    }
    
    set err [catch {set indata(set)} setval]
    if {$err == 1} {
	set setval ""
    }
    
    set err [catch {set indata(metadataPrefix)} metadataprefix]
    if {$err == 1} {
	set metadataprefix ""
    }
    
    set err [catch {set indata(resumptionToken)} resumetoken]
    if {$err == 1} {
	set resumepoint 1
    } else {
	set rs [split $resumetoken -]
	set db [lindex $rs 1]
	set resumepoint [lindex $rs 2]
    }
    
    set totaldocs [numdbdocs sherlock.berkeley.edu CSMP]
    
    
    puts "Content-Type: text/xml"
    puts ""
    
    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata {  <ListRecords xmlns="http://www.openarchives.org/OAI/1.1/OAI_ListRecords" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_ListRecords  http://www.openarchives.org/OAI/1.1/OAI_ListRecords.xsd">} "\n"
    
    append outdata "    <responseDate>$date</responseDate> " "\n"
    
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    
    set format $metadataprefix
    if {$format == ""} {
	set format "oai_dc"
    }
    
    
    set endpoint [expr $resumepoint + 50]
    for {set i $resumepoint} {$i < $endpoint} {incr i} {
	
	set outrecord [LIBSEARCH $i $format]
	
	if {$outrecord != ""} {
	    append outdata {  <record>} "\n"
	    append outdata {   <header>} "\n"
	    append outdata "    <identifier>oai:CSMP:$i</identifier>" "\n"
	    append outdata {    <datestamp>2001-01-05</datestamp>} "\n"
	    append outdata {   </header>} "\n"
	    append outdata {   <metadata>} "\n"
	    if {$format == "oai_dc"} {
		append outdata {   <dc xmlns="http://purl.org/dc/elements/1.1/"  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://purl.org/dc/elements/1.1/   http://www.openarchives.org/OAI/1.1/dc.xsd">} "\n"
		append outdata $outrecord "\n"
		append outdata {    </dc>} "\n"
	    } elseif {$format == "oai_marc"} {
		append outdata $outrecord "\n"
	    } else {
		
	    }
	    append outdata {   </metadata>} "\n"
	    append outdata {  </record>} "\n"
	} 
    }
    
    append outdata "    <resumptionToken>RS-CSMP-$i</resumptionToken> " "\n"
    append outdata {  </ListRecords>} "\n"
    
    puts $outdata

}

proc ListSets {} {

    global request_uri indata
    # puts "ListSets Command "
    set date [responsedate]
    set outdata {}
    # no set hierarchy 

    append outdata {<?xml version="1.0" encoding="UTF-8"?>} "\n"
    append outdata {  <ListSets xmlns="http://www.openarchives.org/OAI/1.1/OAI_ListSets" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"  xsi:schemaLocation="http://www.openarchives.org/OAI/1.1/OAI_ListSets    http://www.openarchives.org/OAI/1.1/OAI_ListSets.xsd">} "\n"
    append outdata "    <responseDate>$date</responseDate>" "\n"
    
    if {[string compare -nocase -length 7 "http://" $request_uri] == 0} {
	append outdata "    <requestURL>$request_uri</requestURL>" "\n"
    } else {
	append outdata "    <requestURL>http://sherlock.berkeley.edu$request_uri</requestURL>" "\n"
    }
    
    append outdata {  </ListSets>} "\n"

    puts "Content-Type: text/xml"
    puts ""
    puts $outdata

}




GETARGS 

#parray indata
#parray env

switch $indata(verb) {
    
    GetRecord {
	GetRecord
    }
    
    Identify {
	Identify
    }

    ListIdentifiers {
	ListIdentifiers
    }

    ListMetadataFormats {
	ListMetadataFormats
    }

    ListRecords {
	ListRecords
    }

    ListSets {
	ListSets 
    }

}






















