#!/bin/bash
# Usage: mnc2nii-units-test.sh <mnc2nii> <rawtominc> <mincconcat> <minc_modify_header>
#
# Verifies that mnc2nii honours the MINC "units" attribute of each dimension.
# The output is in mm and seconds (xyzt_units = 10). mnc2nii used to write
# the numbers unchanged and label them mm and s, so a file in cm or ms was
# wrong by a factor of 10 or 1000.
#
# Input: time(3, start 10, step 2.5, ms) x zspace(2) x yspace(3) x xspace(4),
# spatial steps 1.5/2/2.5 cm, starts -10/5/20 cm.
# Expected: pixdim[1..4] = 15 20 25 0.0025, s-form diagonal 15 20 25 with
# offsets -100 50 200, xyzt_units = 10.
set -u

M2N="$1"; R2M="$2"; CONCAT="$3"; MMH="$4"
ok=1
rm -f m2n_units_*

field() { od -A n -j "$1" -N "$2" -t "$3" "$4" | xargs; }
close() {  # close LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  awk -v e="$2" -v a="$3" 'BEGIN{
    n = split(e, E, " "); if (split(a, A, " ") != n) exit 1;
    for (i = 1; i <= n; i++) { d = E[i] - A[i]; if (d < 0) d = -d;
      if (d > 1e-5 * (E[i] < 0 ? -E[i] : E[i]) + 1e-6) exit 1 }
    exit 0 }' || ok=0
}

head -c $((2 * 3 * 4 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder zspace,yspace,xspace \
    -xstep 1.5 -ystep 2 -zstep 2.5 -xstart -10 -ystart 5 -zstart 20 \
    m2n_units_3d.mnc 2 3 4 || exit 1
"$CONCAT" -clobber -concat_dimension time -start 10 -step 2.5 \
  m2n_units_3d.mnc m2n_units_3d.mnc m2n_units_3d.mnc m2n_units_4d.mnc \
  >/dev/null 2>&1 || { echo "FAIL: mincconcat"; exit 1; }
"$MMH" -sinsert xspace:units=cm -sinsert yspace:units=cm \
  -sinsert zspace:units=cm -sinsert time:units=ms m2n_units_4d.mnc ||
  { echo "FAIL: minc_modify_header"; exit 1; }

"$M2N" -nii m2n_units_4d.mnc m2n_units_4d.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii"; exit 1; }
F=m2n_units_4d.nii
close "pixdim[1..4]" "15 20 25 0.0025" "$(field 80 16 f4 $F)"
close "srow_x" "15 0 0 -100" "$(field 280 16 f4 $F)"
close "srow_y" "0 20 0 50" "$(field 296 16 f4 $F)"
close "srow_z" "0 0 25 200" "$(field 312 16 f4 $F)"
close "xyzt_units" "10" "$(field 123 1 u1 $F)"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
