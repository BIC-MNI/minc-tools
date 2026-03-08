#! /bin/sh
set -x
let errors=0;

echo -n test label arithmetics
# Test a simple addition case.
minccalc -clobber -labels -quiet -expression "A[0]+A[1];" test-zero-byte.mnc test-labels.mnc minccalc-labels-out.mnc

l_out_1=`mincstats -quiet -vol -binvalue 1 minccalc-labels-out.mnc`
l_in_1=`mincstats -quiet -vol -binvalue 1 test-labels.mnc`

if [ "$l_out_1" != "$l_in_1" ]; then
    echo "Problem with label per-voxel addition:" $l_in_1 $l_out_1
    let errors+=1;
else
    echo OK
fi

l_out_2=`mincstats -quiet -vol -binvalue 2 minccalc-labels-out.mnc`
l_in_2=`mincstats -quiet -vol -binvalue 2 test-labels.mnc`

if [ "$l_out_2" != "$l_in_2" ]; then
    echo "Problem with label per-voxel addition:" $l_in_2 $l_out_2
    let errors+=1;
else
    echo OK
fi

exit $errors
