#! /usr/bin/env perl
use strict;
my $errors=0;

my $minccmp_bin = `which minccmp`;
chomp($minccmp_bin);
if ($ENV{'MINCCMP_BIN'})
{
    $minccmp_bin = $ENV{'MINCCMP_BIN'};
}

print "Case 1 - Test the essential command line options.\n";
my $r1 = `$minccmp_bin -version`;
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

my $r1 = `$minccmp_bin -help 2>&1`;
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


print "Case 2 - Test operation on two identical volumes.\n";
my $r1 = `$minccmp_bin mincmorph/test-open.mnc mincmorph/test-open.mnc`;
my $expected = <<HERE;
file[0]:      mincmorph/test-open.mnc
mask file:    (null)
file[1]:      mincmorph/test-open.mnc
ssq:          0
rmse:         0
xcorr:        0
zscore:       nan
maxdiff:      0

HERE
if ($r1 ne $expected)
{
    print "Case 2 failed, incorrect output format.\n";
    $errors++;
}


print "Case 3 - Test operation on two identical volumes with a mask.\n";
my $r1 = `$minccmp_bin mincmorph/test-open.mnc mincmorph/test-open.mnc -mask mincmorph/test-shell.mnc`;
my $expected = <<HERE;
file[0]:      mincmorph/test-open.mnc
mask file:    mincmorph/test-shell.mnc
file[1]:      mincmorph/test-open.mnc
ssq:          0
rmse:         0
xcorr:        0
zscore:       nan
maxdiff:      0

HERE
if ($r1 ne $expected)
{
    print "Case 3 failed, incorrect output format.\n";
    $errors++;
}


print "Case 4 - Test operation on two different volumes.\n";
my $r1 = `$minccmp_bin mincmorph/test-open.mnc mincmorph/test-close.mnc`;
my $expected = <<HERE;
file[0]:      mincmorph/test-open.mnc
mask file:    (null)
file[1]:      mincmorph/test-close.mnc
ssq:          172
rmse:         0.485736187
xcorr:        0
zscore:       nan
maxdiff:      1

HERE
if ($r1 ne $expected)
{
    print "Case 4 failed, incorrect output format.\n";
    $errors++;
}


print "Case 5 - Test operation on two different volumes with mask.\n";
my $r1 = `$minccmp_bin mincmorph/test-open.mnc mincmorph/test-close.mnc -mask mincmorph/test-shell.mnc`;
my $expected = <<HERE;
file[0]:      mincmorph/test-open.mnc
mask file:    mincmorph/test-shell.mnc
file[1]:      mincmorph/test-close.mnc
ssq:          98
rmse:         1
xcorr:        0
zscore:       nan
maxdiff:      1

HERE
if ($r1 ne $expected)
{
    print "Case 5 failed, incorrect output format.\n";
    $errors++;
}


print "Case 6 - Test operation on three volumes.\n";
my $r1 = `$minccmp_bin mincmorph/test-open.mnc mincmorph/test-close.mnc mincmorph/test-shell.mnc`;
my $expected = <<HERE;
file[0]:      mincmorph/test-open.mnc
mask file:    (null)
file[1]:      mincmorph/test-close.mnc
ssq:          172
rmse:         0.485736187
xcorr:        0
zscore:       nan
maxdiff:      1

file[2]:      mincmorph/test-shell.mnc
ssq:          98
rmse:         0.3666479606
xcorr:        0
zscore:       nan
maxdiff:      1

HERE
if ($r1 ne $expected)
{
    print "Case 6 failed, incorrect output format.\n";
    $errors++;
}


print "Case 7 - Test dimension checking. \n";
my $r1 = `$minccmp_bin  test-one.mnc test-dimname.mnc 2>&1`;
my $expected = <<HERE;
Files test-dimname.mnc and test-one.mnc have different dimension names
HERE
if ($r1 ne $expected)
{
    print "Case 7 failed, incorrect error message: $r1.\n";
    $errors++;
}


print "Case 8 - Test dimension checking. \n";
my $r1 = `$minccmp_bin  -nocheck_dimensions -quiet -ssq test-one.mnc test-dimname.mnc 2>&1`;
my $expected = "0\n";
if ($r1 ne $expected)
{
    print "Case 8 failed, output: $r1.\n";
    $errors++;
}


print "Case 9 - Test floor option. \n";
my $r1 = `$minccmp_bin  -quiet -floor 3 test-rnd.mnc test-one.mnc`;
my $expected = <<HERE;
325
2.549509757
0.9899494937
nan
3
HERE
if ($r1 ne $expected)
{
    print "Case 9 failed, output: $r1.\n";
    $errors++;
}


print "Case 10 - Test ceil option. \n";
my $r1 = `$minccmp_bin -quiet -ceil 3 test-rnd.mnc test-one.mnc`;
my $expected = <<HERE;
150
1.224744871
0.8017837257
nan
2
HERE
if ($r1 ne $expected)
{
    print "Case 10 failed, output: $r1.\n";
    $errors++;
}

print "Case 11 - Test range option. \n";
my $r1 = `$minccmp_bin -quiet -range 1 3 test-rnd.mnc test-one.mnc`;
my $expected = <<HERE;
125
1.290994449
0.9258200998
nan
2
HERE
if ($r1 ne $expected)
{
    print "Case 11 failed, output: $r1.\n";
    $errors++;
}

print "Case 12 - Test max difference option with floats. \n";
my $r1 = `$minccmp_bin -quiet -maxdiff test-one.mnc test-float.mnc`;

if ($r1 >= 1e-6)
{
    print "Case 12 failed. Max difference greater than expected: $r1\n";
    $errors++;
}

print "Case 13 - Test max difference option with doubles. \n";
my $r1 = `$minccmp_bin -quiet -maxdiff test-one.mnc test-double.mnc`;

if ($r1 >= 1e-14)
{
    print "Case 13 failed. Max difference greater than expected: $r1\n";
    $errors++;
}



print "OK.\n" if $errors == 0;
print
exit $errors > 0;

