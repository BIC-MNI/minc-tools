#! /usr/bin/env perl
use strict;
my $errors=0;

my $mincstats_bin = `which mincstats`;
chomp($mincstats_bin);
if ($ENV{'MINCSTATS_BIN'})
{
    $mincstats_bin = $ENV{'MINCSTATS_BIN'};
}

print "Case 1 - Basic output.\n";

my $r1 = `$mincstats_bin test-zero.mnc`;
if ($? != 0)
{
    print "Default operation returned something other than zero.\n";
    $errors++;
}
my $c1result = <<HERE;
File:              test-zero.mnc
Mask file:         (null)
Total voxels:      125
# voxels:          125
% of total:        100
Volume (mm3):      125
Min:               0
Max:               0
Sum:               0
Sum^2:             0
Mean:              0
Variance:          0
Stddev:            0
CoM_voxel(z,y,x):  0 0 0
CoM_real(x,y,z):   0 0 0

Histogram:         (null)
Total voxels:      125
# voxels:          0
% of total:        0
Median:            0
Majority:          0
BiModalT:          0
PctT [  0%]:       0
Entropy :          0

HERE
if ($r1 ne $c1result)
{
    print "Case 1 failed, incorrect output format.\n";
    print "/$r1/\n";
    $errors++;
}

print "Case 2 - Test the sum and sum2 options.\n";

my $r1 = `$mincstats_bin -sum -sum2 test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] !~ /^Sum:\s+250$/ ||
    $arr[1] !~ /^Sum\^2:\s+750$/)
{
    print "Case 2 failed, incorrect output format.\n";
    print "$r1\n";
    $errors++;
}

print "Case 3 - Test the mean option.\n";

my $r1 = `$mincstats_bin -mean test-rnd.mnc`;
if ($r1 !~ /^Mean:\s+2$/)
{
    print "Case 2 failed, incorrect output value.\n";
    print "$r1\n";
    $errors++;
}

print "Case 4 - Test the stddev and variance options.\n";

my $r1 = `$mincstats_bin -stddev -variance test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] !~ /^Variance:\s+2.016129032$/ ||
    $arr[1] !~ /^Stddev:\s+1.419904586$/)
{
    print "Case 4 failed, incorrect output value.\n";
    print "$r1\n";
    $errors++;
}

print "Case 5 - Test the min and max options.\n";

my $r1 = `$mincstats_bin -max -min test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] !~ /^Min:\s+0$/ || $arr[1] !~ /^Max:\s+4$/)
{
    print "Case 5 failed, incorrect output value.\n";
    print "$r1\n";
    $errors++;
}

print "Case 6 - Test the CoM option.\n";

my $r1 = `$mincstats_bin -com test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] ne "CoM_voxel(z,y,x):  2 2 2.132" ||
    $arr[1] ne "CoM_real(x,y,z):   2.132 2 2")
{
    print "Case 6 failed, incorrect output value.\n";
    for (my $i = 0; $i <= $#arr; $i++)
    {
        print "/$arr[$i]/\n";
    }
    $errors++;
}

print "Case 7 - Test the median and entropy options.\n";

my $r1 = `$mincstats_bin -median -entropy test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] !~ /Median:\s+1.99904/ ||
    $arr[1] !~ /Entropy :\s+2.321928095/)
{
    print "Case 7 failed, incorrect output value.\n";
    for (my $i = 0; $i <= $#arr; $i++)
    {
        print "/$arr[$i]/\n";
    }
    $errors++;
}

print "Case 8 - Test the histogram option.\n";

my $r1 = `$mincstats_bin -clobber -histogram test-hist.txt test-rnd.mnc`;
open(FH, "< test-hist.txt") or die "Cannot open histogram for reading: $!";
my $n = 0;
while (<FH>)
{
    # Skip comment and blank lines.
    if (/^#/ || /^$/)
    {
        next;
    }
    my @fields = split;
    my $centre = $fields[0];
    my $count = $fields[1];
    if ($centre == 0.001 || $centre == 1.001 || $centre == 2.001 ||
        $centre == 3.001 || $centre == 3.999)
    {
        if ($count != 25)
        {
            $errors++;
            print "Histogram is incorrect: $n $centre $count.\n";
        }
    }
    else 
    {
        if ($count != 0)
        {
            print "Histogram is incorrect: $n $centre $count.\n";
            $errors++;
        }
    }
    $n++;
}
close FH;
if ($n != 2000)
{
    print "Histogram has the wrong number of bins: $n.\n";
    $errors++;
}

print "Case 9 - Test the integer histogram option.\n";

my $r1 = `$mincstats_bin -clobber -integer_hist -histogram test-ihist.txt test-rnd.mnc`;
open(FH, "< test-ihist.txt") or die "Cannot open histogram for reading: $!";
my $n = 0;
while (<FH>)
{
    # Skip comment and blank lines.
    if (/^#/ || /^$/)
    {
        next;
    }
    my @fields = split;
    my $centre = $fields[0];
    my $count = $fields[1];
    if ($count != 25)
    {
        $errors++;
        print "Histogram is incorrect: $n $centre $count.\n";
    }
    $n++;
}
close FH;
if ($n != 5)
{
    print "Histogram has the wrong number of bins: $n.\n";
    $errors++;
}

print "Case 10 - Test the variance option on volumes that are almost exactly 1.\n";

my $r1 = `$mincstats_bin -stddev -variance test-ratio.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($arr[0] !~ /^Variance:\s+0$/ ||
    $arr[1] !~ /^Stddev:\s+0$/)
{
    print "Case 10 failed, incorrect output value.\n";
    print "$r1\n";
    $errors++;
}

print "OK.\n" if $errors == 0;
print
exit $errors > 0;


