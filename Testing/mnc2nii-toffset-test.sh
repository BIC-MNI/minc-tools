#!/bin/bash
# Usage: mnc2nii-toffset-test.sh <mnc2nii> <rawtominc> <mincconcat>
#
# Verifies that mnc2nii writes the MINC time start into the NIfTI toffset
# field (nifti1.h: time point m is at t = toffset + m*pixdim[4]). mnc2nii
# used to leave toffset at 0, so the frame times were lost.
#
# Input: time(3, start 10, step 2.5) x zspace(2) x yspace(3) x xspace(4).
# Expected: toffset = 10, pixdim[4] = 2.5. A file with no time dimension
# must keep toffset = 0.
set -u

M2N="$1"; R2M="$2"; CONCAT="$3"
ok=1
rm -f m2n_toff_*

field() { od -A n -j "$1" -N "$2" -t "$3" "$4" | xargs; }
check() {  # check LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  [ "$2" = "$3" ] || ok=0
}

head -c $((2 * 3 * 4 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder zspace,yspace,xspace m2n_toff_3d.mnc 2 3 4 ||
  exit 1
"$CONCAT" -clobber -concat_dimension time -start 10 -step 2.5 \
  m2n_toff_3d.mnc m2n_toff_3d.mnc m2n_toff_3d.mnc m2n_toff_4d.mnc \
  >/dev/null 2>&1 || { echo "FAIL: mincconcat"; exit 1; }

"$M2N" -nii m2n_toff_4d.mnc m2n_toff_4d.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii on 3-D + time"; exit 1; }
check "3-D+time toffset" "10" "$(field 136 4 f4 m2n_toff_4d.nii)"
check "3-D+time pixdim[4]" "2.5" "$(field 92 4 f4 m2n_toff_4d.nii)"

"$M2N" -nii m2n_toff_3d.mnc m2n_toff_3d.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii on 3-D"; exit 1; }
check "3-D toffset" "0" "$(field 136 4 f4 m2n_toff_3d.nii)"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
