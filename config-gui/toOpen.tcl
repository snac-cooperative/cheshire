#!/usr/local/bin/tclsh8.3
set filename [lindex $argv 0]
if [catch {open $filename r} fileId] {
    puts stderr "Cannot open $filename: $fileId"
}
set filestr [read $fileId]

regsub {ShowDataFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(16\)" filestr; 
regsub {ShowIndexFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(60\)" filestr;
regsub {ShowClusterFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(96\)" filestr;
regsub {ShowDisplayFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(124\)" filestr;
regsub {ShowExplainFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(15\)" filestr;
regsub {ShowComponentFrame} $filestr "\$base\$w\(14\) configure -widget \$base\$w\(161\)" filestr;

if [catch {open "sc.tcl" w} fileId2] {
    puts stderr "Cannot open sc.tcl for write: $fileId2";
}

puts $fileId2 $filestr;
close $fileId;
close $fileId2;
