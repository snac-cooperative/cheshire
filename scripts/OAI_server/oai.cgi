#!/home/cheshire/cheshire/bin/webcheshire

# Here are some personal identification strings

set repositoryName "Special Collections and Archives, University of Liverpool - Cheshire II Retrieval System"
set adminEmail "azaroth@liv.ac.uk"

# This is default if none given for individual dbs
# Can we gather this from Explain automatically?
set databaseIdentifier "ead_components"
set databaseDescription "No description of Cheshire Z database"
set metadataPolicy "No Metadata Policy."
set dataPolicy "No Data Policy."
set submissionPolicy "No Submission Policy."

# Can we configure for multiple databases in a single script?
# id {1 configfile {format format2 format3...} {recsyntax:elemset:function}} 
# id {0 host:port {format...} {r:e:f} searchIndex}

array set zDatabaseInfo {"ead_components" {1 "/home/cheshire/cheshire/ead_components/DBCONFIGFILE" {dc} {"GRS1:DC:convertDCGRS"}} "mailarchive" {1 "/home/cheshire/cheshire/mailarchive/DBCONFIGFILE" {dc} {"GRS1:DC:convertDCGRS"}} "paleo" {1 "/home/cheshire/cheshire/paleo/DBCONFIGFILE" {dc} {"GRS1:DC:convertDCGRS"}}}



# Here are some versioned strings
set oaiURL "http://www.openarchives.org/OAI/1.1/"
set xsiURL "http://www.w3.org/2001/XMLSchema-instance"
set dcURL "http://purl.org/dc/elements/1.1/"

# Load our environment
set LIB_PATH "/home/cheshire/public_html/cgi-bin/lib"
source "lib/libcgi-1.0.0.tcl"
source "lib/libsearch-1.0.0.tcl"

#Generate req_uri for returned data
set request_uri $REQUEST_URI
regsub -all {&} $request_uri "\&amp;" request_uri
if {[string range $request_uri 0 4] != "http:"} {
    set request_uri "http://$SERVER_NAME$request_uri"
}

# ---------------------------------------------------
# Subroutines
#

proc responsedate {} {

    set time [clock seconds]
    set local [string trimleft [clock format $time -format "%H"] 0]
    set GMT [string trimleft [clock format $time -format "%H" -gmt 1] 0]
    set diff [expr $GMT - $local]
    set newtime [clock format $time -format "%Y-%m-%dT%H:%M:%S"]
    return [format "%s-%02d:00" $newtime $diff]

}

proc generateHeader {verb} {
    global oaiURL xsiURL request_uri
    set date [responsedate]

    set txt "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    append txt "<$verb"
    append txt " xmlns=\"${oaiURL}OAI_$verb\""
    append txt " xmlns:xsi=\"$xsiURL\""
    append txt " xsi:schemaLocation=\"${oaiURL}OAI_$verb   ${oaiURL}OAI_$verb.xsd\">\n"
    append txt "  <responseDate>$date</responseDate>\n"
    append txt "  <requestURL>$request_uri</requestURL>\n"

    return $txt
}


proc get_max_docid_oai {dbid} {
    global zDatabaseInfo CHESHIRE_CONFIGFILE CHESHIRE_DATABASE CHESHIRE_NUMREQUESTED

    if {[lsearch [array names zDatabaseInfo] $dbid] == -1} {
	return 0
    }
    set info $zDatabaseInfo($dbid)

    if {[lindex $info 0] == 1} {
	set ccf [lindex $info 1]
	set CHESHIRE_CONFIGFILE $ccf
	set CHESHIRE_DATABASE $dbid
	set CHESHIRE_NUMREQUESTED 1
	return [get_max_docid $ccf $dbid]
    } else {
	set serverinfo [split [lindex $info 1] :]
	return [zget_max_docid [lindex $serverinfo 0] [lindex $serverinfo 1] $dbid]
    }
}



# Here we actually do the interogation and reformatting.
# This should be abstract enough to allow any database to be used as a data source
# Initial approach as experiment is to define a DC formatdef, and map into XML from GRS
# Also need map from database name to actual location.

proc convertDCGRS {record} {
    # Convert to <dc> tags
    # Tagset-G, increment to ftag, set dctags to correspond
    # Handles nested tags (in theory)

    set dctags {title publisher description identifier subject author date}
    set dcxml ""
    set grs [lindex $record 1]
    set grs [lrange $grs 1 end]

    for {set idx 0} {$idx < 7} {incr idx} {
	set taginfo [lindex $grs $idx]

	set type [lindex [lindex [lindex $taginfo 0] 1] 1]
	if {$type != ""} {
	    set tag [lindex $dctags [expr $type -1]]

	    set contents [lindex [lindex $taginfo 1] 1]
	    regsub -all {<[^>]+>} $contents "" contents
	    # Look for \n(whitespace),(whitespace)\n
	    regsub -all "\n( )*,( )*\n" $contents "</$tag><$tag>" contents
	    
	    regsub -all "\n" $contents "" contents
	    
	    if {$tag != "" } {
		append dcxml  "<$tag>$contents</$tag>\n"
	    }
	}
    }

    append dcxml "<type>Archival Finding Aid</type>\n"
    append dcxml "<format>XML</format>\n"

    return $dcxml

}


proc getSGMLRecord {database docid format} {
    global zDatabaseInfo CHESHIRE_CONFIGFILE CHESHIRE_NUMREQUESTED CHESHIRE_NUMSTART CHESHIRE_DATABASE CHESHIRE_ELEMENTSET CHESHIRE_RECSYNTAX

    if {[lsearch [array names zDatabaseInfo] $database] == -1} {
	# Utoh.
	return 0
    }
    set info $zDatabaseInfo($database)
    set formats [lindex $info 2]
    set finfos [lindex $info 3]
    
    if {[lindex $info 0] == 1} {
	# local
	set CHESHIRE_CONFIGFILE [lindex $info 1]
	set CHESHIRE_NUMREQUESTED 1
	set CHESHIRE_NUMSTART 1
	set CHESHIRE_DATABASE $database

	if {[set idx [lsearch $formats $format]] != -1} {
	    set fcstr [lindex $finfos $idx]
	    set fclist [split $fcstr :]
	    set CHESHIRE_RECSYNTAX [lindex $fclist 0]
	    set CHESHIRE_ELEMENTSET [lindex $fclist 1]
	    set function [lindex $fclist 2]
	    
	} else {
	    # Default to Dublin Core
	    set CHESHIRE_RECSYNTAX "GRS1"
	    set CHESHIRE_ELEMENTSET "DC"
	    set function "convertDCGRS"
	}

	set sresults [search docid $docid]
    } else {
	# remote
	set serverinfo [split [lindex $info 1] :]
	zselect $database [lindex $serverinfo 0] $database [lindex $serverinfo 1]
	zset numrequested 1

	if {[set idx [lsearch $formats $format]] != -1} {
	    set fcstr [lindex $finfos $idx]
	    set fclist [split $fcstr :]
	    zset recsyntax [lindex $fclist 0]
	    zset elementset [lindex $fclist 1]
	    set function [lindex $fclist 2]
	} else {
	    zset recsyntax "GRS1"
	    zset elementset "DC"
	    set function "convertDCGRS"
	}

	zfind [lindex $info 4] $docid
	set sresults [zdisplay 1]
    }

    set xmlrec [$function $sresults]
    return $xmlrec

}

proc getOAIRecord {database docid format} {
    global dcURL xsiURL oaiURL

    set record [getSGMLRecord $database $docid $format] 

    set basedocid "oai:$database:$docid"
    set outtxt "<record>\n<header>\n<identifier>$basedocid</identifier>\n<datestamp>2001-01-05</datestamp>\n</header>\n"
    append outtxt "<metadata><dc xmlns=\"${dcURL}\" xmlns:xsi=\"${xsiURL}\" xsi:schemaLocation=\"${dcURL} ${oaiURL}dc.xsd\">\n"
    append outtxt $record
    append outtxt "</dc>\n</metadata>\n</record>\n"

    return $outtxt
}


# ---------------
# Handlers 
# 
proc GetRecord {} {
    global indata dcURL xsiURL oaiURL

    # identifer (required) : separated id
    # metadataPrefix (required) : format to return in (eg oai_dc)

    if {[lsearch [array names indata] identifier] != -1 } {
	# OAI:databasename:docid
	set basedocid $indata(identifier)
	set docidlist [split $basedocid :]

	if { [lsearch [array names indata] metadataPrefix] != -1 } {
	    set fmt $indata(metadataPrefix)
	    if { [string range $fmt 0 3] == "oai_" } {
		set fmt [string range $fmt 4 end]
	    }
	}  else {
	    set fmt ""
	}

	set record [getOAIRecord [lindex $docidlist 1] [lindex $docidlist 2] $fmt]

    } else {
	set basedocid ""
	set record ""
    }

    if {$record != ""} {
	set outtxt $record
    } else {
	set outtxt ""
    }

    append outtxt "</GetRecord>"
    return $outtxt
}

proc Identify {} {
    global indata xsiURL SERVER_NAME SCRIPT_NAME databaseIdentifier databaseDescription metadataPolicy dataPolicy submissionPolicy repositoryName adminEmail oaiURL

    set outtxt "<repositoryName>$repositoryName</repositoryName>\n"
    append outtxt "<baseURL>http://$SERVER_NAME$SCRIPT_NAME</baseURL>\n"
    append outtxt "<protocolVersion>1.1</protocolVersion>\n"
    append outtxt "<adminEmail>mailto:$adminEmail</adminEmail>\n"
    append outtxt "<description>\n<oai-identifier xmlns=\"${oaiURL}oai-identifier\" xmlns:xsi=\"$xsiURL\"  xsi:schemaLocation=\"${oaiURL}oai-identifier  ${oaiURL}oai-identifier.xsd\">\n"
    append outtxt "<scheme>oai</scheme>\n"
    append outtxt "<repositoryIdentifier>$databaseIdentifier</repositoryIdentifier>\n"
    append outtxt "<delimiter>:</delimiter>\n"
    append outtxt "<sampleIdentifier>oai:$databaseIdentifier:123</sampleIdentifier>\n"
    append outtxt "</oai-identifier>\n"
    append outtxt "</description>\n\n"

    append outtxt "<description>\n"
    append outtxt "<eprints xmlns=\"${oaiURL}eprints\" xmlns:xsi=\"$xsiURL\" xsi:schemaLocation=\"${oaiURL}eprints    ${oaiURL}eprints.xsd\">\n"
    append outtxt "<content><text>$databaseDescription</text></content>\n"
    append outtxt "<metadataPolicy><text>$metadataPolicy</text></metadataPolicy>\n"
    append outtxt "<dataPolicy><text>$dataPolicy</text></dataPolicy>\n"
    append outtxt "<submissionPolicy><text></text></submissionPolicy>\n"
    append outtxt "</eprints>\n"
    append outtxt "</description>\n</Identify>\n"
    
    return $outtxt
    
}

proc ListIdentifiers {} {
    global indata databaseIdentifier

    #    until : date value, to search until
    #    from : date value to search after
    #    set : only return from 'set'
    #    resumptionToken : return ids after token

    # Only handling resumptionToken at the moment.
    
    set outtxt ""

    set err [catch {set indata(resumptionToken)} resumetoken]
    if {$err == 1} {
	set db $databaseIdentifier
	set resumepoint 1
    } else {
	set rs [split $resumetoken -]
	set db [lindex $rs 1]
	set resumepoint [lindex $rs 2]
    }

    set maxdocid [get_max_docid_oai $db]

    set endpoint [expr $resumepoint + 200]

    for {set i $resumepoint} {$i < $endpoint && $i < $maxdocid} {incr i} {
	append outtxt "<identifier>oai:$databaseIdentifier:$i</identifier>\n"
    }
    append outtxt "<resumptionToken>RS-$databaseIdentifier-$i</resumptionToken>\n"
    append outtxt "</ListIdentifiers>"

    return $outtxt

}

proc ListMetadataFormats {} {
    global indata oaiURL databaseIdentifier zDatabaseInfo
    
    set outdata ""
    set err [catch {set indata(identifier)} basedocid]
    if {$err == 0} {
	set docid [split $basedocid :]
	set dbid [lindex $docid 1]
    } else {
	set dbid $databaseIdentifier
    }

    set handlesRFC 0
    set handlesDC 0
    set handlesMARC 0
    
    if {[lsearch [array names zDatabaseInfo] $dbid] != -1 } {
	set info $zDatabaseInfo($dbid)
	set formats [lindex $info 2]
	if {[lsearch $formats "dc"] != -1} { set handlesDC 1}
	if {[lsearch $formats "rfc"] != -1} { set handlesRFC 1}
	if {[lsearch $formats "marc"] != -1} { set handlesMARC 1}
    }

    if {$handlesRFC == 1} {
	append outdata "    <metadataFormat> \n"
	append outdata "      <metadataPrefix>oai_rfc1807</metadataPrefix>\n"
	append outdata "      <schema>${oaiURL}rfc1807.xsd</schema> \n"
	append outdata "    </metadataFormat>\n"
    }
    if {$handlesDC == 1} {
	append outdata "    <metadataFormat>\n"
	append outdata "      <metadataPrefix>oai_dc</metadataPrefix>\n"
	append outdata "      <schema>${oaiURL}dc.xsd</schema> \n"
	append outdata "      <metadataNamespace>http://purl.org/dc/elements/1.1/</metadataNamespace>\n"
	append outdata "    </metadataFormat>\n"
    }
    if {$handlesMARC == 1} {
	append outdata "    <metadataFormat>\n"
	append outdata "      <metadataPrefix>oai_marc</metadataPrefix>\n"
	append outdata "      <schema>${oaiURL}oai_marc.xsd</schema> \n"
	append outdata "    </metadataFormat>\n"
    }

    append outdata "</ListMetadataFormats>\n"
    return $outdata

}

proc ListRecords {} {
    global indata databaseIdentifier
    set outtxt ""

    # until / from / set : As above
    # resumptionToken : As above
    # metadataPrefix : Format of metadata part of record
    
    set err [catch {set indata(resumptionToken)} resumetoken]
    if {$err == 1} {
	set db $databaseIdentifier
	set resumepoint 1
    } else {
	set rs [split $resumetoken -]
	set db [lindex $rs 1]
	set resumepoint [lindex $rs 2]
    }

    if { [lsearch [array names indata] metadataPrefix] != -1 } {
	set fmt $indata(metadataPrefix)
	if { [string range $fmt 0 3] == "oai_" } {
	    set fmt [string range $fmt 4 end]
	}
    } else {
	set fmt "dc"
    }
    

    set maxdocid [get_max_docid_oai $db]
    set endpoint [expr $resumepoint + 50]
    for {set i $resumepoint} {$i < $endpoint && $i < $maxdocid} {incr i} {
	append outtxt [getOAIRecord $db $i $fmt] 
    }
    append outtxt "<resumptionToken>RS-$databaseIdentifier-$i</resumptionToken>\n"
    append outtxt "</ListRecords>"

    return $outtxt
}

proc ListSets {} {
    
    # No sets. Not exactly sure how you'd set this up anyway.
    
    return "</ListSets>"
}


# -----------------------------------------------------
# Main Routine
# 

puts "Content-Type: text/xml\n"

set verb $indata(verb)
set outrec [generateHeader $verb]

set err [catch "$verb" fsres]
if {$err} {
    puts stderr "Error: $fsres"
    #Error handling in OAI?? Return 404?? 403??
}
append outrec $fsres

set outrec [encoding convertto utf-8 $outrec]

puts $outrec
