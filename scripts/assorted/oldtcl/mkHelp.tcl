# mkHelp w context
#
# Search screen
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkHelp {w context} {
    global tabList RES SRECS HOST CURHOST smfont medfont bigfont
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Help"
    wm iconname $w "Help"

    message $w.msg -font -Adobe-times-medium-r-normal--*-180* \
       -width 5i  -text "Welcome to the search screen.\
         Enter Author, title or subject. Click the \"SEARCH\" \
         button when you're done." -pady 20
	 
    frame $w.bigbuts
    pack append $w.bigbuts \
       [radiobutton $w.bigbuts.expl -text "Explanation" -relief raised \
       -width 20 -height 4 -variable foo -value epl] \
        {top left padx 50 pady 50} \
       [radiobutton $w.bigbuts.def -text "Definition" -relief raised \
       -width 20 -height 4 -variable foo -value def] {top right padx 50 pady 50}

    frame $w.side
    pack append $w.side \
       [button $w.side.incontext -text "In Context" -relief raised \
       -width 10] {top padx 20 expand } \
       [button $w.side.general -text "General" -relief raised \
       -width 10] {top padx 20 expand}  

    frame $w.entry
    pack append $w.entry \
       [label $w.entry.label -text "Word:" -width 13 -anchor w] \
        {top left expand} \
       [entry $w.entry.entry -width 30 -relief sunken -bd 2 \
        -textvariable entryName] {top left expand} \
       [button $w.entry.button -text "Search"  -command ""] \
         {top right expand} 


    frame $w.display
    pack append $w.display \
       [text $w.display.t -relief raised -bd 2 -yscrollcommand \
        "$w.display.s set" -setgrid true -width 40 ] {bottom left} \
       [scrollbar $w.display.s -relief flat -command "$w.display.t yview"] \
        {bottom right filly} 
 

    bind $w.entry.entry <Return> "
        TextLoadFile $w.display.t \$entryName
        focus $w.entry.entry
    "
    button $w.ok -relief raised -text Quit -command "exit"

# final pack
    pack append $w $w.bigbuts {top fillx} $w.entry {top pady 20} \
     $w.side {bottom left} $w.display {bottom fillx right} \
     $w.ok {bottom fillx}


    $w.display.t mark set insert 0.0
    bind $w <Any-Enter> "focus $w.entry.entry"
    bind $w <Control-Return> "" 
    bind $w <Return> "" 

}


# The procedure below is invoked in response to tabs in the entry # windows.  It moves the focus to the next window in the tab list.   TAKEN FROM DEMO
# Arguments:
#
# list -	Ordered list of windows to receive focus

proc Tab {list} {
    set i [lsearch $list [focus]]
    if {$i < 0} {
	set i 0
    } else {
	incr i
	if {$i >= [llength $list]} {
	    set i 0 }
    }
    focus [lindex $list $i]
}

# The utility procedure below loads a file into a text widget,
# discarding the previous contents of the widget. Tags for the
# old widget are not affected, however.
# Arguments:
#
# w -           The window into which to load the file.  Must be a
#               text widget.
# file -        The name of the file to load.  Must be readable.

proc TextLoadFile {w file} {
    set f [open $file]
    $w delete 1.0 end
    while {![eof $f]} {
        $w insert end [read $f 10000]
    }
    close $f
}

