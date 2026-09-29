#!/bin/bash
# Usage: mnc2nii-dim-limit-test.sh <mnc2nii> <rawtominc>
#
# Verifies that mnc2nii refuses a dimension longer than 32767, the largest
# length that the NIfTI-1 header (short dim[8]) can hold. mnc2nii used to
# write the length into the short field without a check (40000 became
# -25536), which gave an invalid header, and it exited with status 0.
#
# Expected: an error exit (not a signal), a message, and no output file.
# A dimension of exactly 32767 must still convert.
set -u

M2N="$1"; R2M="$2"
ok=1
rm -f m2n_dimlim_*

head -c $((40000 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder zspace,yspace,xspace \
    m2n_dimlim_big.mnc 1 1 40000 || exit 1
"$M2N" -nii m2n_dimlim_big.mnc m2n_dimlim_big.nii >/dev/null 2>m2n_dimlim.err
rc=$?
echo "40000: mnc2nii exit status $rc"
cat m2n_dimlim.err
if [ "$rc" = 0 ] || { [ "$rc" -gt 128 ] && [ "$rc" -le 192 ]; }; then
  echo "FAIL: expected an error exit"; ok=0
fi
grep -q 32767 m2n_dimlim.err || { echo "FAIL: no message"; ok=0; }
[ -e m2n_dimlim_big.nii ] && { echo "FAIL: output file was made"; ok=0; }

head -c $((32767 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder zspace,yspace,xspace \
    m2n_dimlim_max.mnc 1 1 32767 || exit 1
if ! "$M2N" -nii m2n_dimlim_max.mnc m2n_dimlim_max.nii >/dev/null 2>&1; then
  echo "FAIL: 32767 did not convert"; ok=0
fi
d1=$(od -A n -j 42 -N 2 -t d2 m2n_dimlim_max.nii 2>/dev/null | xargs)
echo "32767: dim[1] = $d1"
[ "$d1" = 32767 ] || ok=0

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
