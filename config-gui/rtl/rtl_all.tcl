# -----------------------------------------------------------------------
#   $Author: yzhang $
#   $Locker:  $
# 
# Copyright (c) 1998-1999 Patzschke + Rasp GmbH
#               1999-2000 Patzschke + Rasp Software AG
# 
# Runtime Library Loading modul.
# -----------------------------------------------------------------------
package provide rtl_all 1.2;

foreach pkg [list rtl rtl_gridwin rtl_customize rtl_spinbox\
	 rtl_imagelist rtl_makefont rtl_preferences rtl_question\
	 rtl_scrollwin rtl_shell rtl_statusbar rtl_symbolbar\
	 rtl_symbolbarcustomize rtl_symbutton rtl_tree\
	 rtl_choosefont rtl_combobox rtl_mlistbox rtl_href\
	 rtl_tooltip rtl_decoratedFrame rtl_tabset] {
     package require $pkg;
}
