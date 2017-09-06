#! /bin/bash
let errors=0

if [[ ! -x $MINCCALC_BIN ]]; then
    echo "set MINCCALC_BIN"
    exit 1
fi

if [[ ! -x $MINCSTATS_BIN ]]; then
    echo "set MINCSTATS_BIN"
    exit 1 
fi

echo "MINC_FORCE_V2=${MINC_FORCE_V2}"

echo -n Case 1...
# Test a simple addition case.
$MINCCALC_BIN -clobber -quiet -expression "A[0]+A[1];" test-one.mnc test-two.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '375' ]]; then
    echo "Problem with basic per-voxel addition:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 2...
# Test another basic multiplication case.
$MINCCALC_BIN -clobber -quiet -expression "A[0]*A[1]" test-one.mnc test-two.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '250' ]]; then
    echo "Problem with basic per-voxel multiplication:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 3...
# Test a more complex example including a function. The result here should
# be equal to exp(-0.5)*125.
#
$MINCCALC_BIN -clobber -quiet -expression "ratio = A[0]/A[1]; exp(-ratio)" test-one.mnc test-two.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '75.81633329' ]]; then
    echo "Problem with first function test:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 4...
# Test another example including a function. The result here should be 
# equal to sqrt(3)*125
#
$MINCCALC_BIN -clobber -quiet -expression "x = cos(A[0])+A[1]; sqrt(x)" test-zero.mnc test-two.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '216.5063471' ]]; then
    echo "Problem with second function test:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 5...
# Test a more complex script which sends its results to two different 
# output files. This is straight out of the man page.
#
$MINCCALC_BIN -clobber -quiet -expression "s0=s1=s2=0; for {i in [0:len(A))} { v=A[i]; s0 = s0 + 1; s1 = s1 + v; s2 = s2 + v*v; }; stdev = (s0>1) ? sqrt((s2 - s1*s1/s0) / (s0-1)) : (s0 > 0) ? 0 : NaN ; mean = (s0 > 0) ? s1 / s0 : NaN;" test-one.mnc test-one.mnc test-two.mnc -outfile mean minccalc-out.mnc -outfile stdev minccalc-out2.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '166.6666716' ]]; then
    echo "Problem with elaborate script mean:" $r1
    let errors+=1;
else
    echo -n OK...
fi
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out2.mnc`
if [[ $r1 != '72.16878235' ]]; then
    echo "Problem with elaborate script stddev:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 6...
# Test clamp with the pseudorandom input file...
$MINCCALC_BIN -clobber -quiet -expression "clamp(A[0]+A[1], 1, 4)" test-one.mnc test-rnd.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -min minccalc-out.mnc`
if [[ $r1 != '1' ]]; then
    echo "Problem with clamped min:" $r1
    let errors+=1;
else
    echo -n OK...
fi
r1=`$MINCSTATS_BIN -quiet -max minccalc-out.mnc`
if [[ $r1 != '4' ]]; then
    echo "Problem with clamped max:" $r1
    let errors+=1;
else
    echo -n OK...
fi
# Since the "random" file contains exactly 25 voxels of value 0,1,2,3, or 4, 
# the result here should contain 25 each of 1, 2, and 3 and 50 of 4, or 
# (6*25+4*50)=350.
#
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '350' ]]; then
    echo "Problem with clamped sum:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 7...
# Test segment with the pseudorandom input file...
$MINCCALC_BIN -clobber -quiet -expression "segment(A[0]*A[1], 1.5, 4.5)" test-two.mnc test-rnd.mnc minccalc-out.mnc
# Since the "random" file contains exactly 25 voxels of value 0,1,2,3, or 4, 
# the output should contain exactly 25 voxels with value of either 2 or 4.
#
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '50' ]]; then
    echo "Problem with segmented image:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 8...
# Test the currently failing case - modification of lower range limit.
$MINCCALC_BIN -clobber -quiet -expression 'm=0;n=len(A); for {i in (m:n)} { 1; }; m;' test-zero.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '0' ]]; then
    echo "Problem with alteration of lower exclusive range limits:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 9...
# Test for modification of upper range limit.
$MINCCALC_BIN -clobber -quiet -expression 'm=0;n=len(A); for {i in (m:n)} { 1; }; n;' test-zero.mnc minccalc-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum minccalc-out.mnc`
if [[ $r1 != '125' ]]; then
    echo "Problem with alteration of upper exclusive range limits:" $r1
    let errors+=1;
else
    echo OK
fi

if [[ $errors = "0" ]]; then
    echo "No errors detected."
else
    echo $errors errors detected.
fi
exit $errors



