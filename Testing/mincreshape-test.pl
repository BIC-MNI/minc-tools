#! /usr/bin/env perl
use strict;
my $errors=0;

my $mincstats_bin = `which mincstats`;
chomp($mincstats_bin);
if ($ENV{'MINCSTATS_BIN'})
{
    $mincstats_bin = $ENV{'MINCSTATS_BIN'};
}

my $mincreshape_bin = `which mincreshape`;
chomp($mincreshape_bin);
if ($ENV{'MINCRESHAPE_BIN'})
{
    $mincreshape_bin = $ENV{'MINCRESHAPE_BIN'};
}

my $mincinfo_bin = `which mincinfo`;
chomp($mincinfo_bin);
if ($ENV{'MINCINFO_BIN'})
{
    $mincinfo_bin = $ENV{'MINCINFO_BIN'};
}

my $mincextract_bin = `which mincextract`;
chomp($mincextract_bin);
if ($ENV{'MINCEXTRACT_BIN'})
{
    $mincextract_bin = $ENV{'MINCEXTRACT_BIN'};
}

# Case 1 - Test a very basic case. We reshape the data into unsigned 
# short integer in order to set up the next test.
#
system("$mincreshape_bin -quiet -clobber -unsigned -short test-one.mnc mincreshape-t1.mnc");
my $r1=`$mincstats_bin -clobber -integer_histogram -histogram mincreshape-t1.txt -quiet -sum mincreshape-t1.mnc`;
if ($r1 != 125)
{
    $errors++;
    print "Test 1 failed basic check.\n";
}
my @h1 = read_histogram('mincreshape-t1.txt');
if ($h1[1] != 125)
{
    $errors++;
    print "Test 1 failed histogram check.\n";
}

# Case 2 - Now test the case where the fill data is out of range.

system("$mincreshape_bin -quiet -clobber -start -1,-1,-1 mincreshape-t1.mnc mincreshape-t2.mnc");
my $r1=`$mincstats_bin -clobber -integer_histogram -histogram mincreshape-t2.txt -quiet -sum mincreshape-t2.mnc`;
my @h2 = read_histogram('mincreshape-t2.txt');
if ($h2[1] != 64 || $h2[0] != 61)
{
    $errors++;
    print "Test 2 failed histogram check.\n";
}

# Case 3 - Now test the case where the fill data is out of range.

system("$mincreshape_bin -quiet -fillvalue 2 -clobber -start -1,-1,-1 mincreshape-t1.mnc mincreshape-t3.mnc");
my $r1=`$mincstats_bin -clobber -integer_histogram -histogram mincreshape-t3.txt -quiet -sum mincreshape-t3.mnc`;
my @h3 = read_histogram('mincreshape-t3.txt');
if ($h3[1] != 64 || $h3[2] != 61)
{
    $errors++;
    print "Test 3 failed histogram check.\n";
}

# Case 4 - Verify the operation of some simple manipulations

system("$mincreshape_bin -quiet -clobber -ydirection -unsigned -byte test-rnd.mnc mincreshape-t4.mnc");
my $r1=`$mincstats_bin -clobber -integer_histogram -histogram mincreshape-t4.txt -quiet -sum2 mincreshape-t4.mnc`;
if (int($r1) != 750)
{
    $errors++;
    print "Test 4 failed simple sum^2 check.\n";
}
my $r1 = `$mincinfo_bin -attvalue yspace:step test-rnd.mnc`;
my $r2 = `$mincinfo_bin -attvalue yspace:step mincreshape-t4.mnc`;
if ($r1 != -$r2)
{
    $errors++;
    
    print "Test 4 failed step sign change check.\n";
}
my $r1 = `$mincextract_bin -byte -range 0 4 -start 4,0,0 -count 1,5,1 test-rnd.mnc | od -An -td1`;
my $r2 = `$mincextract_bin -byte -range 0 4 -start 4,0,0 -count 1,5,1 mincreshape-t4.mnc | od -An -td1`;
my @a1 = split(' ', $r1);
my @a2 = split(' ', $r2);
for (my $i = 0; $i < 5; $i++)
{
    if ($a1[$i] != $a2[5-$i-1])
    {
        $errors++;
        print "Test 4 failed data direction check.\n";
    }
}

my @h4 = read_histogram('mincreshape-t4.txt');
if ($h4[0] != 25 || $h4[1] != 25 || $h4[2] != 25 ||
    $h4[3] != 25 || $h4[4] != 25)
{
    $errors++;
    print "Test 4 failed histogram check.\n";
}

# Case 5 - Now test a harder case where the fill data is out of range.

system("$mincreshape_bin -clobber -quiet -unsigned -byte test-rnd.mnc mincreshape-t5a.mnc");
system("$mincreshape_bin -quiet -fillvalue 100 -clobber -start -1,0,0 mincreshape-t5a.mnc mincreshape-t5.mnc");
my $r1=`$mincstats_bin -clobber -integer_histogram -histogram mincreshape-t5.txt -quiet -sum mincreshape-t5.mnc`;
my @h5 = read_histogram('mincreshape-t5.txt');
if ($h5[0] != 20 || $h5[1] != 20 || $h5[2] != 20 ||
    $h5[3] != 20 || $h5[4] != 20 || $h5[100] != 25)
{
    $errors++;
    print "Test 5 failed histogram check.\n";
    for (my $i = 0; $i <= 1000; $i++)
    {
        print "$i $h5[$i]\n" if $h5[$i] != 0;
    }
}

print "OK.\n" if $errors == 0;
print
exit $errors > 0;


sub read_histogram {
    my @hist = ();
   my ($filename) = @_;
    open(my $fh, "<", $filename);
    foreach my $line (<$fh>) {
        if ($line !~ /^#/)
        {
            my @fields = split(' ', $line);
            $hist[$fields[0]] = $fields[1] if @fields == 2;
        }
    }
    close($fh);
    return @hist;
}
