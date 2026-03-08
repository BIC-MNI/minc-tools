#! /bin/sh
let errors=0

echo -n Case 1...
# Test a single erosion
mincmorph -clobber -erosion mincmorph/test-padded.mnc mincmorphout.mnc
r1=`mincdiff -body mincmorph/test-erosion.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single erosion:" $r1
    let errors+=1;
else
    echo OK
fi

echo -n Case 2...
# Test a single dilation
mincmorph -clobber -dilation mincmorph/test-padded.mnc mincmorphout.mnc
r2=`mincdiff -body mincmorph/test-dilation.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single dilation:" $r2
    let errors+=1;
else
    echo OK
fi


echo -n Case 3...
# Test a single distance transform
mincmorph -clobber -distance mincmorph/test-padded.mnc mincmorphout.mnc
r3=`mincdiff -body mincmorph/test-distance.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single distance transform:" $r3
    let errors+=1;
else
    echo OK
fi


echo -n Case 4...
# Test a single closure
mincmorph -clobber -close mincmorph/test-shell.mnc mincmorphout.mnc
r4=`mincdiff -body mincmorph/test-close.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single closure:" $r4
    let errors+=1;
else
    echo OK
fi


echo -n Case 5...
# Test a single opening
mincmorph -clobber -open mincmorph/test-shell.mnc mincmorphout.mnc
r5=`mincdiff -body mincmorph/test-open.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single opening:" $r5
    let errors+=1;
else
    echo OK
fi


echo -n Case 6...
# Test a single median dilation
mincmorph -clobber -median_dilation mincmorph/test-shell.mnc mincmorphout.mnc
r6=`mincdiff -body mincmorph/test-mdilate.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single median dilation:" $r6
    let errors+=1;
else
    echo OK
fi


echo -n Case 7...
# Test a single group operation
mincmorph -clobber -group mincmorph/test-two-shells.mnc mincmorphout.mnc
r7=`mincdiff -body mincmorph/test-group.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single group op:" $r7
    let errors+=1;
else
    echo OK
fi


echo -n Case 8...
# Test a single binarisation
mincmorph -clobber -binarise -range 0 2 mincmorph/test-distance.mnc mincmorphout.mnc
r8=`mincdiff -body mincmorph/test-binarise.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single binarise:" $r8
    let errors+=1;
else
    echo OK
fi


echo -n Case 9...
# Test a single clamping
mincmorph -clobber -clamp -range 0 2 mincmorph/test-distance.mnc mincmorphout.mnc
r9=`mincdiff -body mincmorph/test-clamp.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single clamping:" $r9
    let errors+=1;
else
    echo OK
fi


echo -n Case 10...
# Test a single padding
mincmorph -clobber -pad -background 5 mincmorph/test-two-shells.mnc mincmorphout.mnc
r10=`mincdiff -body mincmorph/test-pad.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single padding:" $r10
    let errors+=1;
else
    echo OK
fi


echo -n Case 11...
# Test a single convolution
mincmorph -clobber -convolve mincmorph/test-shell.mnc mincmorphout.mnc
r11=`mincdiff -body mincmorph/test-convolve.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single convolution:" $r11
    let errors+=1;
else
    echo OK
fi


echo -n Case 12...
# Test a single lowpass filter
mincmorph -clobber -lowpass mincmorph/test-close.mnc mincmorphout.mnc
r12=`mincdiff -body mincmorph/test-lowpass.mnc mincmorphout.mnc`

if [ $? != '0' ]; then
    echo "Problem with single lowpass filter:" $r12
    let errors+=1;
else
    echo OK
fi


echo -n Case 13...
# Test a single highpass filter
# HIGHPASS not implemented yet
r13=`mincmorph -clobber -highpass mincmorph/test-close.mnc mincmorphout.mnc 2>&1`
case "$r13" in
    *"Not implemented yet"*)
        echo $r13
        echo "This is expected"
        ;;
    *)
        let errors+=1;
        echo "Problem with single highpass filter:" $r13
        ;;
esac


if [ $errors = "0" ]; then
    echo "No errors detected."
else
    echo $errors errors detected.
fi
exit $errors
