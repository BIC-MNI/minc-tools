#!/bin/bash
# Usage: nii2mnc-float-scaling-test.sh <nii2mnc> <mincinfo> <mincstats> \
#            <nii_in> <exp_min> <exp_max> <exp_sum>
#
# Verifies that nii2mnc applies scl_slope/scl_inter to float data when it
# keeps the float type (no type flag). NIfTI-1 applies scaling to float data
# too. nii2mnc used to write the stored values and put the scaled range only
# into image-min/image-max, which MINC ignores for floating-point images, so
# the scaling was lost.
#
# Checks: MINC type "float", and real min/max/sum (tolerance 1e-3).
set -u

N2M="$1"; MI="$2"; MS="$3"; NII="$4"
EXP_MIN="$5"; EXP_MAX="$6"; EXP_SUM="$7"

OUT="$(basename "$NII" .nii)_native.mnc"
rm -f "$OUT"

within() {
  awk -v e="$1" -v a="$2" 'BEGIN{ d=a-e; if(d<0)d=-d; exit (d<=1e-3)?0:1 }'
}

if ! "$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed on $NII"; exit 1
fi
VT=$("$MI" -vartype image "$OUT" 2>/dev/null)
A_MIN=$("$MS" -quiet -min "$OUT" 2>/dev/null)
A_MAX=$("$MS" -quiet -max "$OUT" 2>/dev/null)
A_SUM=$("$MS" -quiet -sum "$OUT" 2>/dev/null)

echo "nii: $NII"
echo "  type: expected 'float' actual '$VT'"
echo "  min:  expected $EXP_MIN actual $A_MIN"
echo "  max:  expected $EXP_MAX actual $A_MAX"
echo "  sum:  expected $EXP_SUM actual $A_SUM"

ok=1
[ "$VT" = float ] || ok=0
within "$EXP_MIN" "$A_MIN" || ok=0
within "$EXP_MAX" "$A_MAX" || ok=0
within "$EXP_SUM" "$A_SUM" || ok=0

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
