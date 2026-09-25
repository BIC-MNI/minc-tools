#!/bin/bash
# Usage: nii2mnc-extra-dims-test.sh <nii2mnc> <nii_in>
#
# Verifies that nii2mnc refuses a NIfTI file with dim[6] or dim[7] larger
# than 1. nii2mnc makes MINC dimensions only for dim[1..5], so it used to
# write the first part of the data only, with no warning and exit status 0.
#
# Expected: an error exit (not a signal), a message, and no output file.
set -u

N2M="$1"; NII="$2"
OUT="$(basename "$NII" .nii)_extra.mnc"
rm -f "$OUT"

"$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>n2m_extra.err
rc=$?
echo "$NII: exit status $rc"
cat n2m_extra.err

ok=1
if [ "$rc" = 0 ] || { [ "$rc" -gt 128 ] && [ "$rc" -le 192 ]; }; then
  echo "FAIL: expected an error exit"; ok=0
fi
grep -q 'dim\[6\]' n2m_extra.err || { echo "FAIL: no message"; ok=0; }
[ -e "$OUT" ] && { echo "FAIL: $OUT was made"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
