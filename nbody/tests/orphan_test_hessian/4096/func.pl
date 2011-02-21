#!/usr/bin/perl

$M = $ARGV[0];
$rs = $ARGV[1];
$tback = $ARGV[2];
$tfor = $ARGV[3];

`cat orphan_test_head.js > orphan_test.js`;

open (MYFILE, '>>orphan_test.js');
print MYFILE "            \"mass\" : $M,\n";
print MYFILE "            \"nbody\" : 4096,\n";
print MYFILE "            \"scale-radius\" : $rs,\n";
print MYFILE "            \"time-orbit\" : $tback,\n";
print MYFILE "            \"time-dwarf\" : $tfor,\n";
close (MYFILE); 

`cat orphan_test_tail.js >> orphan_test.js`;

`/data1/willeb/research/milkywayathome_client/bin/milkyway_nbody -f orphan_test.js -o orphan_test.out -z histout >out`;

open(MYFILE, 'out');
@rawdata = <MYFILE>;
close(MYFILE);

foreach $line (@rawdata)
{
	$chisq = -1.0 * $rawdata[0];
} 

chomp($chisq);

print $chisq; 

	print STDERR "XXX FIT = $chisq\n";
	print STDERR "XXX FITNESS = $chisq # $M $rs $tback $tfor\n"; 

`rm out`;
