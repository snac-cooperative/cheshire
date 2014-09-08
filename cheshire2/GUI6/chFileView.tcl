# chFileView
# Stolen from Ray Larson's GeoBrowser Help system
# This isn't bad, but we should probably either make tkinfo our
# one and only help viewing system, or create our own MEGA-SGML
# file viewing widget that can be used both for displaying records
# AND for viewing help.  Hmm.  Version 2?

proc chFileView {title Filename} {
    global entryBG defaultBG defaultActiveBG
    set acTextBG AntiqueWhite2;# Text widget background

    	createHelpListBox {$title $Filename}

}

proc createHelpListBox {title Filename} {
    global entryBG defaultBG defaultActiveBG buttonBG entryFont textBG
    global C2_helpinfo1 C2_helpinfo2 C2_helpinfo3

    # Log the Help interaction -- we're assuming that we only use
    # this function with quick start text
    set C2_helpinfo1 1
    set C2_helpinfo2 001
    set C2_helpinfo3 "Quick Start Text"
    pLogCmd 4

    # create the file viewing window
    set pname .help
    
    catch {destroy .help}
    toplevel .help
    wm title .help "$title"
    wm geometry  .help -200+200

    frame .help.f1
    scrollbar .help.f1.scroll -command ".help.f1.txt yview" -relief sunken \
	-background $defaultBG \
	-activebackground $defaultActiveBG \
	-troughcolor $buttonBG \
	-highlightcolor $buttonBG \
	-highlightbackground $defaultBG
    text .help.f1.txt -yscrollcommand ".help.f1.scroll set" \
        -borderwidth 1 -relief sunken -width 60 -height 24 \
        -padx 20 -pady 5 -wrap word -setgrid 1 \
        -font -*-Courier-Medium-R-Normal-*-140-* \
	-background $textBG \
	-highlightbackground $defaultBG \
	-highlightcolor $defaultBG \
	-selectbackground $textBG \
	-selectborderwidth 0
    pack append .help.f1 .help.f1.txt {left expand fill}\
        .help.f1.scroll {left filly}
    
    button .help.ok -text "QUIT" -relief raised -pady 6 \
        -command "pLogCmd 34; destroy .help" 
    
    pack append .help .help.f1 {top}\
        .help.ok {top expand fillx}
    
    # read in the contents of the $Filename document file
    #------------------------------------------------------------------
    global defaultPath
    
    readFileIntoText $defaultPath/$Filename .help.f1.txt
    
    .help.f1.txt config -state disabled

#end createHelpListBox
}


proc readFileIntoText {Filename t_name} {

    # check whether file exists
    if [file exists $Filename]==0 {
        popMessage .errorF FILE ERROR "File does not exist"
        return
    }
    #endif

    set FID [open $Filename r]

    # Set up display styles
    
    $t_name tag configure <normal> -font -*-Courier-Medium-R-Normal-*-140-*
    $t_name tag configure <bold> -font -*-Courier-Bold-R-Normal-*-140-*
    $t_name tag configure <italic> -font -*-Courier-Medium-O-Normal-*-140-*
    $t_name tag configure <big> -font  -*-Courier-Medium-R-Normal-*-180-*
    $t_name tag configure <verybig> -font -*-Courier-Medium-R-Normal-*-240-*
    if {[winfo screendepth $t_name] > 4} {
        $t_name tag configure <bgcolor> -background #eed5b7
        $t_name tag configure <red> -foreground #d00
        $t_name tag configure <yellow> -foreground #ff0
        $t_name tag configure <green> -foreground #0a0
        $t_name tag configure <white> -foreground #fff
        $t_name tag configure <black> -foreground #000
        $t_name tag configure <blue> -foreground #33f
        $t_name tag configure <green2> -foreground #5f5
        $t_name tag configure <lightblue> -foreground #aaf
        $t_name tag configure <lightgreen> -foreground #5f5
        $t_name tag configure <bred> -foreground #d00 -background #000
        $t_name tag configure <byellow> -foreground #ff0 -background #000
        $t_name tag configure <bgreen> -foreground #0a0 -background #000
        $t_name tag configure <bwhite> -foreground #fff -background #000
        $t_name tag configure <bblack> -foreground #000 -background #000
        $t_name tag configure <bblue> -foreground #33f -background #000
        $t_name tag configure <bgreen2> -foreground #5f5 -background #000
        $t_name tag configure <blightblue> -foreground #aaf -background #000
        $t_name tag configure <blightgreen> -foreground #5f5 -background #000
        $t_name tag configure <raised> -background #eed5b7 -relief raised \
            -borderwidth 1
        $t_name tag configure <sunken> -background #eed5b7 -relief sunken \
            -borderwidth 1
        $t_name tag configure <TITLE> -font -*-courier-medium-r-normal--17-140-*-*-*-*-*-* -foreground #33f
        $t_name tag configure <SUBTITLE> -font -*-courier-medium-r-normal--17-140-*-*-*-*-*-* -foreground #33f
    } else {
        $t_name tag configure <bgcolor> -background black -foreground white
        $t_name tag configure <red> -background black -foreground white
        $t_name tag configure <yellow>  -background black -foreground white
        $t_name tag configure <green> -background black -foreground white
        $t_name tag configure <white> -background black -foreground white
        $t_name tag configure <black> -background black -foreground white
        $t_name tag configure <blue> -background black -foreground white
        $t_name tag configure <lightblue> -background black -foreground white
        $t_name tag configure <green2> -background black -foreground white
        $t_name tag configure <bred> -foreground white -background black
        $t_name tag configure <byellow> -foreground white -background black
        $t_name tag configure <bgreen> -foreground white -background black
        $t_name tag configure <bwhite> -foreground white -background black
        $t_name tag configure <bblack> -foreground white -background black
        $t_name tag configure <bblue> -foreground white -background black
        $t_name tag configure <bgreen2> -foreground white -background black
        $t_name tag configure <blightblue> -foreground white -background black
        $t_name tag configure <blightgreen> -foreground white -background black
        $t_name tag configure <raised> -background white -relief raised \
                -borderwidth 1
        $t_name tag configure <sunken> -background white -relief sunken \
                -borderwidth 1
        $t_name tag configure <TITLE> -font -*-courier-medium-r-normal--17-140-*-*-*-*-*-*
-* -foreground black
        $t_name tag configure <SUBTITLE> -font -*-courier-medium-r-normal--17-140-*-*-*-*-*-*
140-* -foreground black
    }
    $t_name tag configure <bgstipple> -background black -borderwidth 0 \
        -bgstipple gray25
    $t_name tag configure <fgstipple> -fgstipple gray50
    $t_name tag configure <underline> -underline on
    set currenttags {}
    while {1==1} { 

        set cnt [gets $FID inline]
        # check for eof
        if {$cnt==-1} {break}
        if {[string length $inline]==0} {
            insertWithTags $t_name "\n\n" $currenttags 
        }
        set all ""
        set prv ""
        set tagcmd ""
        set rem ""
        set match [regexp (^\[^<>\]*)(<\[0-0a-zA-Z\]*>)(.*\$) $inline all prv tagcmd rem]
        if {$match} {
            # puts stdout [format "all= %s : prv = %s : tagcmd = %s : rem = %s" $all $prv $tagcmd $rem ]
            
            if {[string length "$tagcmd"] > 0} {
                if {[string compare "<normal>" $tagcmd]==0} {
                    insertWithTags $t_name $prv
                    set currenttags {}
                    append rem " "
                    insertWithTags $t_name $rem
                } else {
                    insertWithTags $t_name $prv $currenttags
                    set currenttags $tagcmd
                    append rem " "
                    insertWithTags $t_name $rem $currenttags
                }
            }
        } else { 
            append inline " "
            insertWithTags $t_name $inline $currenttags 
        }
    }
    #end while
    
    close $FID
    
    #end readFileIntoText
}

proc popMessage {w title messag } {
    # pops up a message box with an ok button for the
    # users information - typically used to warn users of
    # the occurrence of errors, exceptional conditions
    
    catch "destroy $w"
    toplevel $w
    wm title $w $title

    message $w.mess -text $messag -relief raised

    button $w.ok -text "OK" -relief raised -pady 5 -command "destroy $w"

    pack append $w $w.mess {top expand fill}\
        $w.ok {top expand fillx filly}
    
    #end popMessage
}

# this is borrowed from the widget demo
# The procedure below inserts text into a given text widget and
# applies one or more tags to that text.  The arguments are:
#
# w             Window in which to insert
# text          Text to insert (it's inserted at the "insert" mark)
# args          One or more tags to apply to text.  If this is empty
#               then all tags are removed from the text.

proc insertWithTags {w text args} {
    set start [$w index insert]
    $w insert insert $text
    foreach tag [$w tag names $start] {
        $w tag remove $tag $start insert
    }
    foreach i $args {
        $w tag add $i $start insert
    }
}


