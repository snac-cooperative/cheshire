# pBooleanTypeDisplay {} -- Display text indicating boolean arrangement
#   of keyword entry windows.  Basically all this procedure does
#   is change the text appearing in the boolean buttons from AND to OR
#   and back when the user clicks on a particular boolean button.

proc pBooleanTypeDisplay {switch_num} {
    global boolList boolSwitch1 boolSwitch2 boolSwitch3

    switch $switch_num {
	"1" {
	    if {"$boolSwitch1" == "[lindex $boolList 0]"} {
		set boolSwitch1 [lindex $boolList 1]
	    } else {
		set boolSwitch1 [lindex $boolList 0]
	    }
	}
	"2" {
	    if {"$boolSwitch2" == "[lindex $boolList 2]"} {
		set boolSwitch2 [lindex $boolList 3]
	    } else {
		set boolSwitch2 [lindex $boolList 2]
	    }

	}
	"3" {
	    if {"$boolSwitch3" == "[lindex $boolList 0]"} {
		set boolSwitch3 [lindex $boolList 1]
	    } else {
		set boolSwitch3 [lindex $boolList 0]
	    }
	}
    }
}
