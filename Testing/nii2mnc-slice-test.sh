#!/bin/bash
# Usage: nii2mnc-slice-test.sh <nii2mnc> <mincinfo> <nii_in> <exp_dims>
#
# Verifies that nii2mnc keeps a spatial axis of length 1 (a single slice),
# with its start, so that the slice keeps its position. nii2mnc used to make
# spatial dimensions only when the length was more than 1, and then wrote
# the start, step and direction cosines of the missing axis as global
# attributes (ncvarid() returned -1, which is NC_GLOBAL), which MINC ignores.
#
# The fixtures (make_nii_slice_fixtures.py) have one z slice at z = 20 mm,
# z step 4 mm. Expected: the image dimensions <exp_dims>, and
# zspace:start = 20, zspace:step = 4.
set -u

N2M="$1"; MI="$2"; NII="$3"; EXP_DIMS="$4"
OUT="$(basename "$NII" .nii)_slice.mnc"
rm -f "$OUT"

if ! "$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed on $NII"; exit 1
fi

ok=1
check() {  # check LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  [ "$2" = "$3" ] || ok=0
}
check "dimensions" "$EXP_DIMS" "$("$MI" -vardims image "$OUT" | xargs)"
check "zspace:start" "20" "$("$MI" -attvalue zspace:start "$OUT" 2>/dev/null | xargs)"
check "zspace:step" "4" "$("$MI" -attvalue zspace:step "$OUT" 2>/dev/null | xargs)"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
