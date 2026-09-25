#!/bin/bash
# Usage: mnc2nii-extra-dim-test.sh <mnc2nii> <mincconcat> <mnc_in>
#
# Verifies that mnc2nii refuses a MINC image that has a dimension it cannot
# map to NIfTI (here "echo", made with mincconcat). mnc2nii used to size its
# buffer from the known dimensions only and then read every dimension into
# it, which overflowed the heap and usually crashed.
#
# Expected: a non-zero exit status that is not a crash, an error message that
# names the dimension, and no output file.
set -u

M2N="$1"; CONCAT="$2"; MNC="$3"
IN=m2n_echo.mnc
OUT=m2n_echo.nii
rm -f "$IN" "$OUT" m2n_echo.err

if ! "$CONCAT" -clobber -concat_dimension echo -start 1 -step 1 \
     "$MNC" "$MNC" "$IN" >/dev/null 2>&1; then
  echo "FAIL: mincconcat"; exit 1
fi

"$M2N" -nii "$IN" "$OUT" >/dev/null 2>m2n_echo.err
rc=$?
echo "mnc2nii exit status: $rc"
cat m2n_echo.err

ok=1
# 129..192 is a signal (139 is SIGSEGV); mnc2nii returns -1 (255) on error.
if [ "$rc" = 0 ] || { [ "$rc" -gt 128 ] && [ "$rc" -le 192 ]; }; then
  echo "FAIL: expected an error exit, got $rc"; ok=0
fi
grep -q echo m2n_echo.err || { echo "FAIL: message does not name 'echo'"; ok=0; }
[ -e "$OUT" ] && { echo "FAIL: $OUT was made"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
