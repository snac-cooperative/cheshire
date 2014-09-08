# mkLog1
#
# Create a top-level window that displays a bunch of entries with
# tabs set up to move between them.
#
# Arguments:
#    w -	Name to use for new top-level window.

proc mkLog1 {{w .form}} {
    global tabList
    catch {destroy $w}
    toplevel $w
    dpos $w
    wm title $w "Create Account"
    wm iconname $w "Create Account"
    message $w.msg -font -Adobe-times-medium-r-normal--*-180* -width 4i \
	    -text "Welcome to Cheshire II's Account Setup Program!
While you do not need an account to use the Cheshire II system, there are several advantagest to creating one. 1)  When you Logon to the system, Cheshire II will know who you are and some of your user preferneces 2) Cheshire two will be able to automatically send any of your requested results to your electronic mailboxes and 3) You will have the capability of picking up where you left off last time you used Cheshire II.  To create an account, simply enter the requested information below.  When you are done, click on the OK button.  To exit without creating an account at this time click on CANCEL.  For help at any time click on the HELP button."
    foreach i {f1 f2 f3 f4 f5} {
	frame $w.$i -bd 1m
	entry $w.$i.entry -relief sunken -width 40
	bind $w.$i.entry <Tab> "Tab \$tabList"
	bind $w.$i.entry <Return> "destroy $w"
	label $w.$i.label
	pack append $w.$i $w.$i.entry right $w.$i.label left
    }
    $w.f1.label config -text Name:
    $w.f2.label config -text "E-mail Address:"
    $w.f5.label config -text "Research Interests:"
    button $w.ok -text OK -command "destroy $w"
    pack append $w $w.msg {top fill} $w.f1 {top fillx} $w.f2 \
	    {top fillx} $w.f3 {top fillx} $w.f4 {top fillx} \
	    $w.f5 {top fillx} $w.ok {bottom fill}
    set tabList "$w.f1.entry $w.f2.entry $w.f3.entry $w.f4.entry $w.f5.entry"
}

# The procedure below is invoked in response to tabs in the entry
# windows.  It moves the focus to the next window in the tab list.
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
	    set i 0
	}
    }
    focus [lindex $list $i]
}
