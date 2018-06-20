#! /bin/bash
let errors=0

if [[ ! -x $MINCMORPH_BIN ]]; then
    echo "set MINCMORPH_BIN"
    exit 1
fi

if [[ ! -x $MINCDIFF_BIN ]]; then
    MINCDIFF_BIN=`which mincdiff`  
fi

echo -n Case 1...
# Test a single erosion
$MINCMORPH_BIN  -clobber -erosion mincmorph/test-padded.mnc mincmorphout.mnc
r1=`$MINCDIFF_BIN -body mincmorph/test-erosion.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single erosion:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 2...
# Test a single dilation
$MINCMORPH_BIN  -clobber -dilation mincmorph/test-padded.mnc mincmorphout.mnc
r2=`$MINCDIFF_BIN -body mincmorph/test-dilation.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single dilation:" $r2
    let errors+=1;
else
    echo OK
fi


echo -n Case 3...
# Test a single distance transform
$MINCMORPH_BIN  -clobber -distance mincmorph/test-padded.mnc mincmorphout.mnc
r3=`$MINCDIFF_BIN -body mincmorph/test-distance.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single distance transform:" $r3
    let errors+=1;
else
    echo OK
fi


echo -n Case 4...
# Test a single closure
$MINCMORPH_BIN  -clobber -close mincmorph/test-shell.mnc mincmorphout.mnc
r4=`$MINCDIFF_BIN -body mincmorph/test-close.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single closure:" $r4
    let errors+=1;
else
    echo OK
fi


echo -n Case 5...
# Test a single opening
$MINCMORPH_BIN  -clobber -open mincmorph/test-shell.mnc mincmorphout.mnc
r5=`$MINCDIFF_BIN -body mincmorph/test-open.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single closure:" $r5
    let errors+=1;
else
    echo OK
fi


echo -n Case 6...
# Test a single median dilation
$MINCMORPH_BIN  -clobber -median_dilation mincmorph/test-shell.mnc mincmorphout.mnc
r6=`$MINCDIFF_BIN -body mincmorph/test-mdilate.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single median dilation:" $r6
    let errors+=1;
else
    echo OK
fi


echo -n Case 7...
# Test a single group operation
$MINCMORPH_BIN  -clobber -group mincmorph/test-two-shells.mnc mincmorphout.mnc
r7=`$MINCDIFF_BIN -body mincmorph/test-group.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single group op:" $r7
    let errors+=1;
else
    echo OK
fi


echo -n Case 8...
# Test a single binarisation
$MINCMORPH_BIN  -clobber -binarise -range 0 2 mincmorph/test-distance.mnc mincmorphout.mnc
r8=`$MINCDIFF_BIN -body mincmorph/test-binarise.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single binarise:" $r8
    let errors+=1;
else
    echo OK
fi


echo -n Case 9...
# Test a single clamping
$MINCMORPH_BIN  -clobber -clamp -range 0 2 mincmorph/test-distance.mnc mincmorphout.mnc
r9=`$MINCDIFF_BIN -body mincmorph/test-clamp.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single closure:" $r9
    let errors+=1;
else
    echo OK
fi


echo -n Case 10...
# Test a single padding
$MINCMORPH_BIN  -clobber -pad -background 5 mincmorph/test-two-shells.mnc mincmorphout.mnc
r10=`$MINCDIFF_BIN -body mincmorph/test-pad.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single closure:" $r10
    let errors+=1;
else
    echo OK
fi


echo -n Case 11...
# Test a single convolution
$MINCMORPH_BIN  -clobber -convolve mincmorph/test-shell.mnc mincmorphout.mnc
r11=`$MINCDIFF_BIN -body mincmorph/test-convolve.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single convolution:" $r11
    let errors+=1;
else
    echo OK
fi


echo -n Case 12...
# Test a single lowpass filter
$MINCMORPH_BIN  -clobber -lowpass mincmorph/test-close.mnc mincmorphout.mnc
r12=`$MINCDIFF_BIN -body mincmorph/test-lowpass.mnc mincmorphout.mnc`

if [[ $? != '0' ]]; then
    echo "Problem with single lowpass filter:" $r12
    let errors+=1;
else
    echo OK
fi


echo -n Case 13...
# Test a single highpass filter
# HIGHPASS not implemented yet 
r13=`$MINCMORPH_BIN  -clobber -highpass mincmorph/test-close.mnc mincmorphout.mnc 2>&1`
if  [[ "$r13" == *"Not implemented yet"* ]]; then
    echo $r13 
    echo "This is expected" 
else
    let errors+=1;
    echo "Problem with single highpass filter:" $r13
fi


if [[ $errors = "0" ]]; then
    echo "No errors detected."
else
    echo $errors errors detected.
fi
exit $errors
