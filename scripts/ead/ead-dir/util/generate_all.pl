#!/usr/bin/perl


my $id = 0;
do {
    my $retval = system "generate_single.tcl $id";
    $id++;
} while ($retval == 0)



