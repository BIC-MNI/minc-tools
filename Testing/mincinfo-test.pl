#! /usr/bin/env perl
use strict;
my $errors=0;

my $mincinfo_bin = `which mincinfo`;
chomp($mincinfo_bin);
if ($ENV{'MINCINFO_BIN'})
{
    $mincinfo_bin = $ENV{'MINCINFO_BIN'};
}

print "Case 1 - Test the essential command line options.\n";

my $r1 = `$mincinfo_bin -version`;
if ($? != 0)
{
    print "-version returned something other than zero.\n";
    $errors++;
}
my @arr = split(/^/m, $r1);
if ($#arr < 1 || $arr[0] !~ /^program: \d+\.\d+\.\d+/)
{
    print "-version message line 1 has wrong format.\n";
    $errors++;
}
    
my $r1 = `$mincinfo_bin -help 2>&1`;
if ($? != 256)
{
    print "-help returned something other than 256 (-1 as unsigned char).\n";
    $errors++;
}
my @arr = split(/^/m, $r1);
if ($arr[0] !~ /^Command-specific options:$/)
{
    print "-help message line 1 has wrong format.\n";
    $errors++;
}

print "Case 2 - test operation with no command-line options.\n";

my $r1 = `$mincinfo_bin test-zero.mnc`;
my $c2result = <<HERE;
file: test-zero.mnc
image: signed__ float -1000 to 1000
image dimensions: zspace yspace xspace
    dimension name         length         step        start
    --------------         ------         ----        -----
    zspace                      5            1            0
    yspace                      5            1            0
    xspace                      5            1            0
HERE
if ($r1 ne $c2result)
{
    print "Case 2 failed, incorrect output format.\n";
    $errors++;
}

print "Case 3 - test the dimnames option.\n";

my $r1 = `$mincinfo_bin -dimnames test-zero.mnc`;
if ($r1 ne "xspace yspace zspace \n")
{
    print "Case 3 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 4 - test the varnames option.\n";

my $r1 = `$mincinfo_bin -varnames test-zero.mnc`;
if ($r1 ne "image image-min image-max xspace yspace zspace acquisition patient study \n")
{
    print "Case 4 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 5 - test the dimlength option.\n";

my $r1 = `$mincinfo_bin -dimlength xspace test-zero.mnc`;
if ($r1 ne "5\n")
{
    print "Case 5 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 6 - test the vartype option.\n";

my $r1 = `$mincinfo_bin -vartype image-max -vartype image -vartype study test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 2 || $arr[0] ne "double" || $arr[1] ne "float" || $arr[2] ne "long")
{
    print "Case 6 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 7 - test the vardims option.\n";

my $r1 = `$mincinfo_bin -vardims study -vard image test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 1 || $arr[0] ne "" || $arr[1] ne "zspace yspace xspace ")
{
    print "Case 7 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 8 - test the varatts option.\n";

my $r1 = `$mincinfo_bin -varatt xspace test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 0 || $arr[0] ne "length varid vartype version comments spacing alignment step start ")
{
    print "Case 8 failed, incorrect output format.\n";
    $errors++;
}
my $r1 = `$mincinfo_bin -varatt image test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 0 || $arr[0] ne "dimorder varid vartype version valid_range complete ")
{
    print "Case 8 failed, incorrect output format.\n";
    $errors++;
}

print "Case 9 - test the varatts option.\n";

my $r1 = `$mincinfo_bin -varval xspace test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 0 || $arr[0] ne "0")
{
    print "Case 9 failed, incorrect output value: $arr[0].\n";
    $errors++;
}
my $r1 = `$mincinfo_bin -varvalues image-max test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 0 || $arr[0] ne "4")
{
    print "Case 9 failed, incorrect output value: $arr[0].\n";
    $errors++;
}

print "Case 10 - test the atttype option.\n";

my $r1 = `$mincinfo_bin -atttype image:valid_range -atttype xspace:length -atttype image:dimorder test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 2 || $arr[0] ne "float" || $arr[1] ne "long" || $arr[2] ne "char")
{
    print "Case 10 failed, incorrect output values.\n";
    print "$r1\n";
    $errors++;
}

print "Case 11 - test the attvalue option.\n";

my $r1 = `$mincinfo_bin -attvalue image:valid_range -attvalue xspace:length -attvalue image:dimorder test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 2 || $arr[0] ne "-1000 1000 " || $arr[1] ne "5 " || $arr[2] ne "zspace,yspace,xspace")
{
    print "Case 11 failed, incorrect output values.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "/$arr[$i]/\n"; }
    $errors++;
}

print "Case 12 - test the minc_version option.\n";

my $r1 = `$mincinfo_bin -minc_ver test-zero.mnc test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 5 || $arr[0] ne "Version: 2 (HDF5)" || $arr[0] ne $arr[3])
{
    print "Case 12 failed, incorrect output values.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "/$arr[$i]/\n"; }
    $errors++;
}

print "Case 13 - test the error_string option.\n";

my $r1 = `$mincinfo_bin -error_string Testing -attvalue image:xyzzy test-zero.mnc test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr != 5 || $arr[0] ne "Testing" || $arr[0] ne $arr[3])
{
    print "Case 13 failed, incorrect output values.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "/$arr[$i]/\n"; }
    $errors++;
}

print "OK.\n" if $errors == 0;
print
exit $errors > 0;


