#!/bin/bash
# Usage: nii2mnc-dimnames-test.sh <nii2mnc> <mincinfo> <nii_in> <expected dimname...>
#
# Converts a NIfTI fixture with nii2mnc and checks that the resulting MINC has
# exactly the expected set of dimension names. This guards the file-axis ->
# MINC spatial-axis (xspace/yspace/zspace) assignment: a degenerate s-form
# slice column must still yield three distinct spatial axes, not a duplicated
# "xspace" with "zspace" dropped. Fixtures live in nii2mnc_data/ and are built
# by make_nii_fixtures.py.
set -u

N2M="$1"; MI="$2"; NII="$3"; shift 3
EXPECTED="$*"

OUT="$(basename "$NII" .nii).mnc"
rm -f "$OUT"
if ! "$N2M" -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed to convert $NII"; exit 1
fi

norm() { tr ' ' '\n' | sed '/^$/d' | sort | xargs; }
ACTUAL=$("$MI" -dimnames "$OUT" 2>/dev/null | norm)
EXP=$(printf '%s' "$EXPECTED" | norm)

echo "nii: $NII"
echo "  expected dimnames: $EXP"
echo "  actual   dimnames: $ACTUAL"
if [ "$ACTUAL" = "$EXP" ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
