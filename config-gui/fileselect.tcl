
#option add *Listbox*font \
#    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#option add *Entry*font \
#    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile
#option add *Label*font \
#    "-*-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" startupFile

# this is the default proc  called when "OK" is pressed
# to indicate yours, give it as the first arg to "fileselect"
package provide DirSelect 1.0

proc fileselect.default.cmd {f} {
  puts stderr "selected file $f"
}


# this is the default proc called when error is detected
# indicate your own pro as an argument to fileselect

proc fileselect.default.errorHandler {errorMessage} {
    puts stdout "error: $errorMessage"
    catch { cd ~ }
}

# this is the proc that creates the file selector box

proc fileselect {
    bbutton
    {cmd fileselect.default.cmd} 
    {purpose "Select A Directory:"} 
    {w .fileSelectWindow}} { 

    global dbcfg;
    global appGlobal;

    set num $appGlobal(filedefNum);
    set dpath $dbcfg(FILEDEF${num}_0DEFAULTPATH);

    catch {destroy $w}

    toplevel $w
    if {$bbutton != \
	".staffcheshire.fdbcfg.dbcfgfr.gridwin.data.datafr.dpathfr.dpathchck"} {
	grab $w
    }
    wm title $w "Select A Directory"


    # path independent names for the widgets
    global fileselect

    set fileselect(entry) $w.file.eframe.entry
    set fileselect(list) $w.file.sframe.list
    set fileselect(scroll) $w.file.sframe.scroll
    set fileselect(ok) $w.bframe.okframe.ok
    set fileselect(cancel) $w.bframe.cancel
    set fileselect(dirlabel) $w.file.dirlabel
    if {$bbutton == ".staffcheshire.fdbcfg.menuframe.dbenv.envdef"} {
	set fileselect(dir) $appGlobal(initDir)
    } elseif {$dpath == ""} {
	set fileselect(dir) $appGlobal(initDir)
    } else {
	set fileselect(dir) $dpath
    }

    # widgets
    frame $w.file -bd 10 
    frame $w.bframe -bd 10
    pack append $w \
        $w.file {left filly} \
        $w.bframe {left expand frame n}

    frame $w.file.eframe
    frame $w.file.sframe
    label $w.file.dirlabel -anchor w -justify left -width 24 \
          -foreground blue -text $fileselect(dir) 

    pack append $w.file \
        $w.file.eframe {top frame w} \
	$w.file.sframe {top fillx} \
	$w.file.dirlabel {top frame w}


    label $w.file.eframe.label -anchor w -width 24 -text $purpose
    entry $w.file.eframe.entry -relief sunken 

    pack append $w.file.eframe \
		$w.file.eframe.label {top expand frame w} \
                $w.file.eframe.entry {top fillx frame w} 


    scrollbar $w.file.sframe.yscroll -relief sunken \
	 -command "$w.file.sframe.list yview"
    listbox $w.file.sframe.list -selectmode single -relief sunken \
	-yscroll "$w.file.sframe.yscroll set" 

    pack append $w.file.sframe \
        $w.file.sframe.yscroll {right filly} \
 	$w.file.sframe.list {left expand fill} 

    # buttons
    frame $w.bframe.okframe -borderwidth 2 -relief sunken
 
    button $w.bframe.okframe.ok -text OK -relief raised -padx 10 \
        -command "fileselect.ok.cmd $bbutton $w $cmd"

    button $w.bframe.cancel -text cancel -relief raised -padx 10 \
        -command "fileselect.cancel.cmd $w $bbutton"
    pack append $w.bframe.okframe $w.bframe.okframe.ok {padx 10 pady 10}

    pack append $w.bframe $w.bframe.okframe {expand padx 20 pady 20}\
                          $w.bframe.cancel {top}

    # Fill the listbox with a list of the files in the directory (run
    # the "/bin/ls" command to get that information).
    # to not display the "." files, remove the -a option and fileselect
    # will still work
 
    $fileselect(list) insert end ".."
    foreach i [exec /bin/ls -a $fileselect(dir)] {
        if {[string compare $i "."] != 0 && \
	    [string compare $i ".."] != 0 && \
	    [file isdirectory $i]} {
            $fileselect(list) insert end $i
        }
    }

    # Set up bindings for the browser.
    bind $fileselect(entry) <Return> fileselect.click.cmd 
    bind $fileselect(entry) <Control-c> {eval $fileselect(cancel) invoke}

    bind $w <Control-c> {eval $fileselect(cancel) invoke}

    bind $fileselect(list) <Button-1> {
        # puts stderr "button 1 release"
        %W selection anchor [%W nearest %y]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
    }

    bind $fileselect(list) <Key> {
        %W selection anchor [%W nearest %y]
        $fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
    }

    bind $fileselect(list) <Double-ButtonPress-1> {
        # puts stderr "double button 1"
        %W selection anchor [%W nearest %y]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
	fileselect.click.cmd;
    }

    bind $fileselect(list) <Return> {
        %W selection anchor [%W nearest %y]
	$fileselect(entry) delete 0 end
	$fileselect(entry) insert 0 [%W get [%W nearest %y]]
	fileselect.click.cmd;
    }
    # set kbd focus to entry widget
    focus $fileselect(entry);
}

# auxiliary button procedures

proc fileselect.cancel.cmd {w path} {
    # puts stderr "Cancel"
    global dbcfg;
    global appGlobal;
    destroy $w;
    if {$path == ".staffcheshire.fdbcfg.menuframe.dbenv.envdef"} {
	set dbcfg(Defineenv) 0;
    }
    if {$path ==
    ".staffcheshire.fdbcfg.dbcfgfr.gridwin.data.datafr.dpathfr.dpathchck"} {
	set dbcfg(dpathchck_var) 0;
    }
    cd $appGlobal(initDir)
}

proc fileselect.click.cmd {
    {w .fileSelectWindow} 
    {errorHandler fileselect.default.errorHandler}} {

    global fileselect
    set selected [$fileselect(entry) get]

    # some nasty file names may cause "file isdirectory" to return an error
    set sts [catch { 
	file isdirectory $selected
    }  errorMessage ]

    if { $sts != 0 } then {
	$errorHandler $errorMessage
	destroy $w
	return
    }

    # clean the text entry and prepare the list
    $fileselect(entry) delete 0 end
    $fileselect(list) delete 0 end
    $fileselect(list) insert end ".."

    # selection may be a directory. Expand it.
    if {[file isdirectory $selected] != 0} {
	cd $selected
	set dir [set fileselect(dir) [pwd]]
	$fileselect(dirlabel) configure -text $dir

	foreach i [exec /bin/ls -a $dir] {
	    if {[string compare $i "."] != 0 && \
		[string compare $i ".."] != 0 && \
		[file isdirectory $i]} {
		$fileselect(list) insert end $i
	    }
	}
    }
}


proc fileselect.ok.cmd {
    bbutton
    {w .fileSelectWindow} 
    {cmd fileselect.default.cmd}} {

    global fileselect
    global appGlobal
    set num $appGlobal(filedefNum)
    set dpath ""
    if [info exists dbcfg(FILEDEF${num}_DEFAULTPATH)] {
	set dpath $dbcfg(FILEDEF${num}_DEFAULTPATH)
    }

    set tmpdir [$fileselect(entry) get]
    set dir $fileselect(dir)
    set fullpath "${dir}/$tmpdir"
   
    # strip off the default path prefix
    if {$dpath != ""} {
	regsub "${dpath}/" $fullpath "" fullpath;
    }

    $cmd $bbutton $fullpath
    destroy $w
    set appGlobal(dbcfgSaved) 0;
    cd $appGlobal(initDir)
}











