# chCredits -- Ad Majorem Feles Gloriam


proc chCredits {w type} {
    global entryBG defaultBG defaultActiveBG defaultPath
    global C2_helpinfo1 C2_helpinfo2 C2_helpinfo3

    set acTextBG AntiqueWhite2;  # Text widget background

    toplevel $w -class ac
    wm title $w "About Cheshire II"
    wm iconname $w "About Cheshire II"
    wm geometry $w -200+200

    # Framework

    label $w.cat  -bitmap @$defaultPath/bitmaps/medcat.xbm;	# you-know-who
    message $w.actext1 -justify center \
	-font *-times-medium-r-normal--*-180-*-*-*-*-*-* \
	-text "Cheshire II
Version 1.0"
    message $w.actext2 -justify center \
	-font *-times-medium-r-normal--*-120-*-*-*-*-*-* \
	-text "The Piratical Band:

Prof. Ray Larson
Dr. Ralph Moon
Lucy Kuntz
Jerome McDonough
Paul O'Leary"; # Background for Cheshire II text
    message $w.actext3 -justify center \
	-font *-times-medium-i-normal--*-120-*-*-*-*-*-* \
	-text "We're all mad here.  I'm mad.  You're mad."
    button $w.button -relief raised -bd 2 -text "OK" \
	-command "pLogCmd 34; destroy $w"


    pack $w.cat $w.actext1 $w.actext2 $w.actext3 $w.button -side top -fill both

    # Log transaction
    set C2_helpinfo1 5
    set C2_helpinfo2 003
    set C2_helpinfo3 "Ad Majorem Feles Gloriam"
    pLogCmd 4
}









