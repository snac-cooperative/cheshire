#!/usr/local/bin/tclsh8.3
 set filename [lindex $argv 0]
if [catch {open $filename r} fileId] {
    puts stderr "Cannot open $filename: $fileId"
}
set filestr [read $fileId]

regsub {\$base\$w\(14\) configure -widget \$base\$w\(16\)} $filestr {ShowDataFrame} filestr;
regsub {\$base\$w\(14\) configure -widget \$base\$w\(60\)} $filestr {ShowIndexFrame} filestr;
regsub {\$base\$w\(14\) configure -widget \$base\$w\(96\)} $filestr {ShowClusterFrame} filestr;
regsub {\$base\$w\(14\) configure -widget \$base\$w\(124\)} $filestr {ShowDisplayFrame} filestr;
regsub {\$base\$w\(14\) configure -widget \$base\$w\(15\)} $filestr {ShowExplainFrame} filestr;
regsub {\$base\$w\(14\) configure -widget \$base\$w\(161\)} $filestr {ShowComponentFrame} filestr;
regsub {(\$base\$w\(300\) setselection)} $filestr {#\1} filestr;
if [catch {open "sc.tcl" w} fileId2] {
    puts stderr "Cannot open sc.tcl for write: $fileId2";
}

puts $fileId2 $filestr;
close $fileId;
close $fileId2;
