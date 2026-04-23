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
if ($r1 ne "zspace yspace xspace \n")
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
if ($#arr != 5 || $arr[0] ne "Version: 1 (netCDF)" || $arr[0] ne $arr[3])
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

print "Case 14 - test the -ls option (multiple files, abbreviated headers).\n";

my $r1 = `$mincinfo_bin -ls test-zero.mnc test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
# Expect: header line, dash line, two data rows (one per file)
# size and steps are now per-dimension columns (z, y, x / dz, dy, dx)
if ($#arr < 3
    || $arr[0] !~ /\bfile\b/ || $arr[0] !~ /\bdims\b/ || $arr[0] !~ /\bprotocol\b/
    || $arr[0] =~ /acquisition:protocol/
    || $arr[0] !~ /\bz\b/ || $arr[0] !~ /\bx\b/
    || $arr[0] !~ /\bdz\b/ || $arr[0] !~ /\bdx\b/
    || $arr[0] =~ /\bsize\b/ || $arr[0] =~ /\bsteps\b/
    || $arr[2] !~ /3D/ || $arr[2] !~ /\b5\b/
    || $arr[3] !~ /3D/)
{
    print "Case 14 failed, incorrect -ls output.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "  |$arr[$i]|\n"; }
    $errors++;
}

print "Case 15 - test the -csv option (full attribute names in header).\n";

my $r1 = `$mincinfo_bin -csv test-zero.mnc test-rnd.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr < 2
    || $arr[0] !~ /^file,dims,zspace,yspace,xspace,zspace_step,yspace_step,xspace_step,study:field_value,acquisition:protocol,acquisition:series_description/
    || $arr[1] !~ /^test-zero\.mnc,3D,5,5,5,1,1,1/
    || $arr[2] !~ /^test-rnd\.mnc,3D/)
{
    print "Case 15 failed, incorrect -csv output.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "  |$arr[$i]|\n"; }
    $errors++;
}

print "Case 16 - test the -json option.\n";

my $r1 = `$mincinfo_bin -json test-zero.mnc test-rnd.mnc`;
if ($r1 !~ /^\s*\[/ || $r1 !~ /\]\s*$/
    || $r1 !~ /"filename"/ || $r1 !~ /"ndims"\s*:\s*3/
    || $r1 !~ /"dimensions"/ || $r1 !~ /"attributes"/)
{
    print "Case 16 failed, incorrect -json output.\n";
    print $r1;
    $errors++;
}

print "Case 17 - test -F custom format.\n";

my $r1 = `$mincinfo_bin -ls -F dims,size test-zero.mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr < 2
    || $arr[0] !~ /\bfile\b/ || $arr[0] !~ /\bdims\b/
    || $arr[0] !~ /\bz\b/ || $arr[0] !~ /\bx\b/
    || $arr[0] =~ /\bdz\b/ || $arr[0] =~ /protocol/
    || $arr[2] !~ /3D/)
{
    print "Case 17 failed, incorrect custom -F output.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "  |$arr[$i]|\n"; }
    $errors++;
}

print "Case 18 - test field strength normalization (3T scan).\n";

use File::Basename;
my $modify_bin = "";
if ($ENV{'MINC_MODIFY_HEADER_BIN'}) {
    $modify_bin = $ENV{'MINC_MODIFY_HEADER_BIN'};
} else {
    my $sibling = dirname($mincinfo_bin) . "/minc_modify_header";
    if (-x $sibling) {
        $modify_bin = $sibling;
    } else {
        $modify_bin = `which minc_modify_header`;
        chomp($modify_bin);
    }
}

my $test_field_mnc = "test-field3T.mnc";
`cp test-zero.mnc $test_field_mnc`;
`$modify_bin -dinsert study:field_value=3 $test_field_mnc`;

my $r1 = `$mincinfo_bin -ls $test_field_mnc`;
my @arr = split(/^/m, $r1);
chomp(@arr);
if ($#arr < 2 || $arr[2] !~ /\b3T\b/) {
    print "Case 18 failed, expected '3T' in -ls field column.\n";
    for (my $i = 0; $i <= $#arr; $i++) { print "  |$arr[$i]|\n"; }
    $errors++;
}
# CSV must preserve the raw numeric value, not the normalised "3T"
my $r2 = `$mincinfo_bin -csv -F field $test_field_mnc`;
my @arr2 = split(/^/m, $r2);
chomp(@arr2);
if ($#arr2 < 1 || $arr2[1] !~ /,3$/) {
    print "Case 18 failed, CSV should show raw numeric value.\n";
    for (my $i = 0; $i <= $#arr2; $i++) { print "  |$arr2[$i]|\n"; }
    $errors++;
}
unlink $test_field_mnc;

print "OK.\n" if $errors == 0;
print
exit $errors > 0;


