
#    w -        Name to use for new top-level


source histfuncs.tcl

proc mkHistory {{w .history}} {
    catch {destroy $w}
    toplevel $w
    dpos $w
    global S_NUM
    global bigfontb medfontb
    set medfont "-Adobe-times-medium-r-normal--*-140*"
    set medfontb "-Adobe-times-Bold-r-normal--*-140*"
    wm title $w "Search History"
    wm iconname $w "History"
    global RES HIST H_HITS
       message $w.msg -font -Adobe-times-medium-r-normal--*-180* \
       -width 5i  -relief sunken -text "To Redo a search from your history list, highlight the search and click the first mouse button." -pady 20



   text $w.t -relief raised -bd 2 -yscrollcommand \
        "$w.s set" -wrap word -setgrid true -width 85 -font $medfontb
put_h_in_win $w.t
   text $w.t.a -relief raised -bd 2 -yscrollcommand \
	"$w.s set" -wrap word -setgrid true -width 4 -font $medfont
    frame $w.frame -borderwidth 10 
	pack append $w.frame \
	 [button $w.frame.b3 -text "Add term" \
        -width 15 -command h_add_term] \
        {top pady 20 expand} \
        [button $w.frame.b2 -text "Save History" \
        -width 15 -command {h_save}] \
	{top pady 20 expand} \
	[button $w.frame.b1 -text "Load Previous" \
        -width 15 -command "h_load $w.t.a $w.t"] \
	{top pady 20 expand} \
       [button $w.frame.b4 -text "Help" -width 15 \
	-command mkSort] \
        {top pady 20 expand} 
	
	frame $w.head -borderwidth 10 
		pack append $w.head \
	[label $w.head.1  -relief flat -anchor w \
	-text "Search Query" -font $medfontb] {left expand fill} \
	[label $w.head.2  -relief flat -anchor w \
	-text "No.Retrieved" -font $medfontb] {right fillx} \
	[label $w.head.3  -relief flat -anchor e \
	-text "Database\t" -font $medfontb] {padx 350}


    scrollbar $w.s -relief flat -command "$w.t.a $w.t yview"
	$w.t configure -yscrollcommand "$w.s set"
	$w.t.a configure -yscrollcommand "$w.s set"

    button $w.ok -text OK -command "destroy $w"
   pack append $w $w.msg {top fill} $w.ok {bottom fillx} \
	$w.frame  {left filly} \
 $w.head {top fillx} \
        $w.s {right filly}  $w.t {expand fill} 
}

proc ScrollTwo {left right args} {
    eval $left $args
    eval $right $args
}

