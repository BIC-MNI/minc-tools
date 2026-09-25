#!/bin/bash
# Usage: nii2mnc-unsupported-datatype-test.sh <nii2mnc> <nii_in>
#
# Verifies that nii2mnc stops with an error for a NIfTI datatype that it
# cannot convert (for example complex64 or int64). nii2mnc used to print
# "Data type N not handled", continue with an uninitialized MINC type, write
# an unreadable file, and exit with status 0.
#
# Expected: an error exit (not a signal), a message, and no output file.
set -u

N2M="$1"; NII="$2"
OUT="$(basename "$NII" .nii)_unsup.mnc"
rm -f "$OUT"

"$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>n2m_unsup.err
rc=$?
echo "$NII: exit status $rc"
cat n2m_unsup.err

ok=1
if [ "$rc" = 0 ] || { [ "$rc" -gt 128 ] && [ "$rc" -le 192 ]; }; then
  echo "FAIL: expected an error exit"; ok=0
fi
grep -qi 'data type' n2m_unsup.err || { echo "FAIL: no message"; ok=0; }
[ -e "$OUT" ] && { echo "FAIL: $OUT was made"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
