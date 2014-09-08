# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Description: Utilities usfull and needed by the Runtiome Library
# -----------------------------------------------------------------------
package provide rtl_utils 1.2;


# ---------------------------------------------------------------------------
# misc utilities:
# replace this procedure with your own version:
# ---------------------------------------------------------------------------
# Function    : getSettings - 
# 
# Returns     : 
# Parameters  : 
# 
# Description : Look if there is a defined value for name stored
#               in the user/group settings file
# ---------------------------------------------------------------------------
proc getSettings {name def} {
    variable ::rat;
    if {[info exists rat(settings,$name)]} {
	set def $rat(settings,$name);
    }
    return $def;
}

