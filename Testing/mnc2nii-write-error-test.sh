#!/bin/bash
# Usage: mnc2nii-write-error-test.sh <mnc2nii> <mnc_in>
#
# Verifies that mnc2nii exits with a non-zero status when it cannot write
# the output. nifti_image_write() returns void, and mnc2nii used to return 0
# after the library printed "cannot open output file".
set -u

M2N="$1"; MNC="$2"
ok=1
rm -rf m2n_wr_ok.nii

for out in m2n_no_such_dir/out.nii m2n_no_such_dir/out.nii.gz; do
  "$M2N" -nii "$MNC" "$out" >/dev/null 2>&1
  rc=$?
  echo "$out: exit status $rc"
  if [ "$rc" = 0 ] || { [ "$rc" -gt 128 ] && [ "$rc" -le 192 ]; }; then
    echo "FAIL: expected an error exit"; ok=0
  fi
done

"$M2N" -nii "$MNC" m2n_wr_ok.nii >/dev/null 2>&1
rc=$?
echo "m2n_wr_ok.nii: exit status $rc"
[ "$rc" = 0 ] && [ -s m2n_wr_ok.nii ] || { echo "FAIL: normal write"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
