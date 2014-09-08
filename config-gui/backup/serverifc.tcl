#####################################################################################
#####################################################################################
## The following commands deal with saving, opening, and closing the server init file 
#####################################################################################
#####################################################################################

#------------------------------------------------------------------------------------ 
## this command opens a work space to create a new server init file 
#------------------------------------------------------------------------------------ 
proc NewServerInit {} {
    global serverArray;
    if {[CheckServerSave] == 2} return;
    set serverInitFr [find-widget servercfgparamframe .];
    foreach index [array names serverArray] {
	if {$index == "filemenu"} {
	    continue;
	}
	set serverArray($index) "";
    }
    set serverArray(saved) 1;
    set serverArray(filename) "Untitled";
    wm title .servercfg "Staffcheshire -- Server Configuration File -- Untitled";

    # disable new, save, save as
    $serverArray(filemenu) entryconfigure 1 -state disable;
    $serverArray(filemenu) entryconfigure 4 -state disable;

    # enable open
    $serverArray(filemenu) entryconfigure 2 -state normal;;
    focus [find-widget pmsentry $serverInitFr];
}

proc TraceServerArray {varname index op} {
    global serverArray;
    if {$index == "saved"} {
	if {$serverArray(saved) == 0} {
	    # enable save
	    $serverArray(filemenu) entryconfigure 4 -state normal;
	} else {
	    $serverArray(filemenu) entryconfigure 4 -state disabled;
	}
	return;
    }
    if {$index == "filename"} {
	return;
    }
    set serverArray(saved) 0;
    # enable new
    $serverArray(filemenu) entryconfigure 1 -state normal;
    # disable open
    $serverArray(filemenu) entryconfigure 2 -state disabled;;
}

#------------------------------------------------------------------------------ 
## these two commands open an existing server.init file for editing 
#------------------------------------------------------------------------------ 
proc OpenServerInit {} {
    set openFile [tk_getOpenFile -initialdir [pwd] -title \
	    "Open File"];
    if {$openFile == ""} {
	return;
    }
    DoOpenServerInit $openFile;
}

proc DoOpenServerInit {file} {
    global serverArray;
    if [catch {open $file r} fileId] {
	tk_messageBox -message $fileId;
    } else {
        wm title .servercfg \
	"Staffcheshire -- Server Configuration File -- [file tail $file]";
	set serverArray(filename) $file; 
        set fr [find-widget servercfgparamframe .];
        foreach line [split [read $fileId] \n] {
            if ![regexp {^ *#} $line] {
                switch -regexp $line {
                ^PREFERRED_MESSAGE_SIZE {
		    set serverArray(pms) [ProcessEachLine $line];
		}

                ^MAXIMUM_RECORD_SIZE {
		    set serverArray(mrs) [ProcessEachLine $line];
		}

                ^IMPLEMENTATION_ID {
		    set serverArray(iid) [ProcessEachLine $line];
		}

                ^IMPLEMENTATION_NAME {
		    set serverArray(iname) [ProcessEachLine $line];
		}

                ^IMPLEMENTATION_VERSION {
		    set serverArray(iver) [ProcessEachLine $line];
		}

                ^PROTOCOL_VERSION {
		    set serverArray(pver) [ProcessEachLine $line];
		}

                ^OPTIONS {
		    set serverArray(opt) [ProcessEachLine $line];
		}

                ^PORT {
		    set serverArray(port) [ProcessEachLine $line];
		}

                ^DATABASE_NAMES {
		    set serverArray(dbnames) [ProcessEachLine $line];
		}

                ^DATABASE_DIRECTORIES {
		    set serverArray(dbdir) [ProcessEachLine $line];
		}

                ^DATABASE_CONFIGFILES {
		    set serverArray(dbcffiles) [ProcessEachLine $line];
		}

                ^SUPPORT_NAMED_RESULT_SET {
		    set serverArray(support,snrs) [ProcessEachLine $line];
		}

                ^SUPPORT_MULTIPLE_DATABASE_SEARCH {
		    set serverArray(support,smds) [ProcessEachLine $line];
		}

                ^TIMEOUT {
		    set serverArray(timeout) [ProcessEachLine $line];
		}

                ^LOG_FILE_NAME {
		    set serverArray(logfile) [ProcessEachLine $line];
		}

                ^RESULT_SET_DIRECTORY {
		    set serverArray(rsdir) [ProcessEachLine $line];
		}
		
                ^ATTRIBUTE_SET_ID {
		    set serverArray(asid) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_0_QUERY {
		    set serverArray(support,st0q) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_1_QUERY {
		    set serverArray(support,st1q) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_2_QUERY {
		    set serverArray(support,st2q) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_100_QUERY {
		    set serverArray(support,st100q) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_101_QUERY {
		    set serverArray(support,st101q) [ProcessEachLine $line];
		}

                ^SUPPORT_TYPE_102_QUERY {
		    set serverArray(support,st102q) [ProcessEachLine $line];
		}

                ^SUPPORT_ELEMENT_SET_NAMES {
		    set serverArray(support,sesn) [ProcessEachLine $line];
		}

                ^SUPPORT_SINGLE_ELEMENT_SET_NAME {
		    set serverArray(support,ssesn) [ProcessEachLine $line];
		}

                ^TARGETINFO_NEWS {
		    set serverArray(explain,news) [ProcessEachLine $line];
		}

                ^TARGETINFO_WELCOME_MSG {
		    set serverArray(explain,welcome) [ProcessEachLine $line]
		}

                ^TARGETINFO_CONTACT_NAME {
		    set serverArray(explain,contact) [ProcessEachLine $line];
		}

                ^TARGETINFO_CONTACT_DESCRIPTION {
		    set serverArray(explain,contdes) [ProcessEachLine $line];
		}

                ^TARGETINFO_CONTACT_ADDRESS {
		    set serverArray(explain,contaddr) [ProcessEachLine $line];
		}

                ^TARGETINFO_CONTACT_EMAIL {
		    set serverArray(explain,contemail) [ProcessEachLine $line];
		}

                ^TARGETINFO_CONTACT_PHONE {
		    set serverArray(explain,contphone) [ProcessEachLine $line];
		}

                ^TARGETINFO_DESCRIPTION {
		    set serverArray(explain,serdes) [ProcessEachLine $line];
		}

                ^TARGETINFO_USAGE_RESTRICTION {
		    set serverArray(explain,useres) [ProcessEachLine $line];
		}

                ^TARGETINFO_PAYMENT_ADDRESS {
		    set serverArray(explain,payaddr) [ProcessEachLine $line];
		}

                ^TARGETINFO_HOURS {
		    set serverArray(explain,hour) [ProcessEachLine $line];
		}

                ^TARGETINFO_LANGUAGES  {
		    set serverArray(explain,lang) [ProcessEachLine $line];
		}
                }
            }
        }
        close $fileId;
	set serverArray(saved) 1;
    }
}

proc ProcessEachLine {line} {
    if [regexp "\t+(\[^\t#]+)" $line match value] {
        regexp  "\"(.*)\"" $value match value;
        return $value;
    } else {
        return "";
    }
}

proc ServerToDb {} {
    global serverArray;
    if {[CheckServerSave] == 2} return;
    destroy .servercfg;
    unset serverArray;
    OpenDbInterface;
}

proc ServerToCmd {cmdname} {
    global serverArray;
    if {[CheckServerSave] == 2} return;
    destroy .servercfg;
    unset serverArray;
    # if the user changes from dbcfg->server->cmd
    source cmd.tcl;
    source cmdifc.tcl;
    OpenCmdInterface $cmdname;
}
proc OpenServerInterface {} {
    global serverArray;
    servercfg;
    wm title .servercfg "Staffcheshire -- Server Configuration\
	    File -- Untitled";
    wm geometry . 800x600+50+50;
    wm withdraw .;
    wm protocol .servercfg WM_DELETE_WINDOW {exit};
    set serverArray(filemenu) [find-widget servfilecascade .];
    set serverArray(filename) "Untitled";
    set serverArray(saved) 1;
    $serverArray(filemenu) entryconfigure 1 -state disabled;
    $serverArray(filemenu) entryconfigure 4 -state disabled;
    trace variable serverArray w TraceServerArray;
}
       
#-------------------------------------------------------------------------------------- 
## this command saves the current server.init file
#-------------------------------------------------------------------------------------- 
proc SaveServerInitFileAs {} {
    global serverArray;
    set fn [tk_getSaveFile -initialdir [pwd]];
    if {$fn == ""} return;
    set serverArray(filename) $fn;
    SaveServerInitFile $fn;
    # update the window title
    set wintitle [wm title .servercfg];
    regexp {Staffcheshire -- Server Configuration File -- (.+)$}\
	    $wintitle match oldname;
    set newname [file tail $serverArray(filename)];
    regsub $oldname $wintitle $newname wintitle;
    wm title .servercfg $wintitle;
}

#--------------------------------------------------------------------------------------
## this procedure writes out the server.init file
#--------------------------------------------------------------------------------------
proc SaveServerInitFile {{filename ""}} {
    # the array that contains information on server init parameters
    global serverArray;
    if {$filename == ""} {
	# see if a file name has been provided
	if {$serverArray(filename) == "Untitled"} {
	    SaveServerInitFileAs;
	    return;
	} else {
	    set filename $serverArray(filename);
	}
    }
    # open the file
    if [catch {open $filename w} fileID] {
	tk_messageBox -message $fileID;
    } else {
        foreach index [array names serverArray] {
            # get rid of the leading spaces
            regsub {^ +} $serverArray($index) {} serverArray($index);
            # get rid of the trailing spaces
            regsub { +$} $serverArray($index) {} serverArray($index);
            # replace multiple spaces with a single space 
            regsub -all { +} $serverArray($index) { } serverArray($index);
            ## need to quote strings
            if {[regexp {pms|mrs|support|timeout} $index] != 1} { 
                ## if not already quoted
                if {[regexp {^".*"$} $serverArray($index)] != 1} {
                    regsub .* $serverArray($index) {"&"} serverArray(${index}_m);
                } else {
                    set serverArray(${index}_m) $serverArray($index);
                } 
            }
        }
        puts $fileID "\#The following initialization file contains\
		the following sorts of information:";

        puts $fileID "\#";
        puts $fileID "\#  z3950 Server Initialization File.";
        puts $fileID "\#  The name and value must be in one\
		line. Double quote should be";
        puts $fileID "\#  placed around a value if the value contains\
		more than one word.";
        puts $fileID "\#";
        puts $fileID "\#\tFIELD_NAME\t\t\tVALUE";    
        puts $fileID "PREFERRED_MESSAGE_SIZE\t\t\t$serverArray(pms)"; 
        puts $fileID "MAXIMUM_RECORD_SIZE\t\t\t$serverArray(mrs)";
        puts $fileID "IMPLEMENTATION_ID\t\t\t$serverArray(iid_m)";
        puts $fileID "IMPLEMENTATION_NAME\t\t\t$serverArray(iname_m)";
        puts $fileID "IMPLEMENTATION_VERSION\t\t\t$serverArray(iver_m)";
        puts $fileID "PROTOCOL_VERSION\t\t\t$serverArray(pver_m)";
        puts $fileID "OPTIONS\t\t\t\t\t$serverArray(opt_m)";
        puts $fileID "PORT\t\t\t\t\t$serverArray(port_m)";
        puts $fileID "DATABASE_NAMES\t\t\t\t$serverArray(dbnames_m)";
        puts $fileID "DATABASE_DIRECTORIES\t\t\t$serverArray(dbdir_m)";
        puts $fileID "DATABASE_CONFIGFILES\t\t\t$serverArray(dbcffiles_m)";
        puts $fileID "SUPPORT_NAMED_RESULT_SET\t\t$serverArray(support,snrs)";
        puts $fileID \
		"SUPPORT_MULTIPLE_DATABASE_SEARCH\t$serverArray(support,smds)";
        puts $fileID "TIMEOUT\t\t\t\t\t$serverArray(timeout)";
        puts $fileID "LOG_FILE_NAME\t\t\t\t$serverArray(logfile_m)";
        puts $fileID "RESULT_SET_DIRECTORY\t\t\t$serverArray(rsdir_m)";
        puts $fileID "ATTRIBUTE_SET_ID\t\t\t$serverArray(asid_m)";
        puts $fileID "\#";
        puts $fileID "\#";
        puts $fileID "SUPPORT_TYPE_0_QUERY\t\t\t$serverArray(support,st0q)";
        puts $fileID "SUPPORT_TYPE_1_QUERY\t\t\t$serverArray(support,st1q)";
        puts $fileID "SUPPORT_TYPE_2_QUERY\t\t\t$serverArray(support,st2q)";
        puts $fileID "SUPPORT_TYPE_100_QUERY\t\t\t$serverArray(support,st100q)";
        puts $fileID "SUPPORT_TYPE_101_QUERY\t\t\t$serverArray(support,st101q)";
        puts $fileID "SUPPORT_TYPE_102_QUERY\t\t\t$serverArray(support,st102q)";
        puts $fileID "\#";
        puts $fileID "\#";
        puts $fileID "SUPPORT_ELEMENT_SET_NAMES\t\t$serverArray(support,sesn)";
        puts $fileID\
		"SUPPORT_SINGLE_ELEMENT_SET_NAME\t\t$serverArray(support,ssesn)";
        puts $fileID "\#";
        puts $fileID "\#";
        puts $fileID "\#  Information for generation of explain database";
        puts $fileID "\#";
        puts $fileID "TARGETINFO_NEWS\t\t\t\t$serverArray(explain,news_m)";
        puts $fileID "TARGETINFO_WELCOME_MSG\t\t\t$serverArray(explain,welcome_m)";
        puts $fileID "TARGETINFO_CONTACT_NAME\t\t\t$serverArray(explain,contact_m)";
        puts $fileID\
		"TARGETINFO_CONTACT_DESCRIPTION\t\t$serverArray(explain,contdes_m)";
        puts $fileID "TARGETINFO_CONTACT_ADDRESS\t\t$serverArray(explain,contaddr_m)";
        puts $fileID "TARGETINFO_CONTACT_EMAIL\t\t$serverArray(explain,contemail_m)";
        puts $fileID "TARGETINFO_CONTACT_PHONE\t\t$serverArray(explain,contphone_m)";
        puts $fileID "TARGETINFO_DESCRIPTION\t\t\t$serverArray(explain,serdes_m)";
        puts $fileID "TARGETINFO_USAGE_RESTRICTION\t\t$serverArray(explain,useres_m)";
        puts $fileID "TARGETINFO_PAYMENT_ADDRESS\t\t$serverArray(explain,payaddr_m)";
        puts $fileID "TARGETINFO_HOURS\t\t\t$serverArray(explain,hour_m)";
        puts $fileID "TARGETINFO_LANGUAGES\t\t\t$serverArray(explain,lang_m)";
        puts $fileID "\#";
        puts $fileID "\#";
        puts $fileID "\#  End of the Server Initialization File";
        close $fileID;
        set serverArray(saved) 1;
    }
}

#--------------------------------------------------------------------------------------
## this procedure checks if the file has been saved and acts accordingly
#--------------------------------------------------------------------------------------
proc CheckServerSave {} {
    global serverArray;
    if {$serverArray(saved) == 0} {
	set filename $serverArray(filename);
	set saveornot [tk_dialog .save "Save File?"\
		"Do you want to save the change you made to $filename?"\
		{} 0 "Yes" "No" "Cancel"];
	if {$saveornot == 0} {
	    SaveServerInitFile;
	    return 0;
	} elseif {$saveornot == 2} {
	    return 2;
	}
	return 1;
    }
    return 3;
}

#--------------------------------------------------------------------------------------
## this procedure closes the server.init file editing frame 
#--------------------------------------------------------------------------------------
proc CloseServerInit {} {
    global serverArray;
    if {[CheckServerSave] == 2} return;
    # finalize the saved file if any
    # that is, comment out unspecified values
    if {$serverArray(filename) != "Untitled"} {
	if [catch {open $serverArray(filename) r} fileId] {
	    tk_messageBox -message $fileId;
	    return;
	} else {
	    set content [read $fileId];
	    close $fileId;
	    if [catch {open $serverArray(filename) w} fileId2] {
		tk_messageBox -message $fileId2;
	    } else {
		foreach line [split $content \n] {
		    # if the line is not a line of comment
		    if ![regexp "^ *\#" $line] { 
			# see if the value exists for the 'name - value' pair
			if ![regexp "\t+\[^\t\#]+\$" $line] {
			    puts $fileId "\#$line";
			} else { 
			    puts $fileId $line;
			}
		    } else {
			puts $fileId $line;
		    }
		}
		close $fileId2;
	    }
	}
    }
    exit 0;
}







