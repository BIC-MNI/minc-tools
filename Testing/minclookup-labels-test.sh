#! /bin/bash

let errors=0;

if [[ ! -x $MINCLOOKUP_BIN ]]; then
    MINCLOOKUP_BIN=`which minclookup`;
fi

if [[ ! -x $MINCEXTRACT_BIN ]]; then
    MINCEXTRACT_BIN=`which mincextract`;
fi

echo -n test label remap
# Test a simple map case
$MINCLOOKUP_BIN -clobber -discrete -quiet -lut_string "1 2; 2 3" test-labels.mnc minclookup-labels-out.mnc

l_out_2=`$MINCEXTRACT_BIN -nonormalize minclookup-labels-out.mnc | grep -c 2`
l_in_1=`$MINCEXTRACT -nonormalize test-labels.mnc | grep -c 1`

if [[ $l_out_2 != "$l_in_1" ]]; then
    echo "Problem with label per-voxel remapping:" $l_in_1 $l_out_1
    let errors+=1;
else
    echo OK
fi

l_out_3=`$MINCEXTRACT_BIN -nonormalize minclookup-labels-out.mnc | grep -c 3`
l_in_2=`$MINCEXTRACT_BIN -nonormalize test-labels.mnc | grep -c 2`

if [[ $l_out_3 != "$l_in_2" ]]; then
    echo "Problem with label per-voxel remapping:" $l_in_2 $l_out_2
    let errors+=1;
else
    echo OK
fi

exit $errors
