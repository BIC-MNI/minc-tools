#!/bin/bash
# Usage: mnc2nii-default-step-test.sh <mnc2nii> <rawtominc>
#
# Verifies that mnc2nii writes a positive pixdim when a MINC dimension has no
# step attribute (nifti1.h: pixdim[i] is the voxel width along dimension i,
# "positive"). mnc2nii used to write 0.
#
# Case 1: no step attributes (MINC default step is 1)
#   expected pixdim[1..3] = 1 1 1
# Case 2: irregular time from -frame_times 10,12.5,16 (no time step)
#   expected pixdim[4] = 3 (mean spacing), and a warning on stderr
set -u

M2N="$1"; R2M="$2"
ok=1
rm -f m2n_step_*

field() { od -A n -j "$1" -N "$2" -t "$3" "$4" | xargs; }
check() {  # check LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  [ "$2" = "$3" ] || ok=0
}

head -c $((2 * 3 * 4 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder zspace,yspace,xspace m2n_step_3d.mnc 2 3 4 ||
  exit 1
"$M2N" -nii m2n_step_3d.mnc m2n_step_3d.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii on 3-D"; exit 1; }
check "no step pixdim[1..3]" "1 1 1" "$(field 80 12 f4 m2n_step_3d.nii)"

head -c $((3 * 2 * 3 * 4 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder time,zspace,yspace,xspace \
    -xstep 1.5 -ystep 2 -zstep 2.5 -frame_times 10,12.5,16 \
    m2n_step_irr.mnc 3 2 3 4 || exit 1
"$M2N" -nii m2n_step_irr.mnc m2n_step_irr.nii >/dev/null 2>m2n_step_irr.err ||
  { echo "FAIL: mnc2nii on irregular time"; exit 1; }
check "irregular pixdim[1..4]" "1.5 2 2.5 3" "$(field 80 16 f4 m2n_step_irr.nii)"
cat m2n_step_irr.err
grep -qi 'irregular' m2n_step_irr.err || { echo "FAIL: no warning"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
