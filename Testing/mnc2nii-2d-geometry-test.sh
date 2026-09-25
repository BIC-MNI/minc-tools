#!/bin/bash
# Usage: mnc2nii-2d-geometry-test.sh <mnc2nii> <rawtominc>
#
# Verifies that mnc2nii keeps the world geometry of a MINC file that has no
# zspace dimension (a single oblique 2-D slice). mnc2nii used to pass an
# uninitialized spatial_axes[] entry for the missing axis to
# compute_world_transform(), which gave an identity or otherwise wrong s-form,
# or a crash, depending on the stack contents.
#
# The input is yspace(5) x xspace(6) with steps 2 and 1.5, starts 5 and -10,
# and direction cosines rotated by 30 degrees about z. The expected s-form rows
# (NIfTI header offsets 280, 296, 312) are:
#   srow_x = 1.299  -1.0   0  -11.16
#   srow_y = 0.75    1.732 0   -0.67
#   srow_z = 0       0     1    0
set -u

M2N="$1"; R2M="$2"
IN=m2n_2d_slice.mnc
OUT=m2n_2d_slice.nii
rm -f "$IN" "$OUT"

head -c $((5 * 6 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder yspace,xspace \
    -xstep 1.5 -ystep 2 -xstart -10 -ystart 5 \
    -xdircos 0.866025 0.5 0 -ydircos -0.5 0.866025 0 "$IN" 5 6 ||
  { echo "FAIL: rawtominc"; exit 1; }

if ! "$M2N" -nii "$IN" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: mnc2nii failed on $IN"; exit 1
fi

# row OFFSET -> the 4 float32 values of one s-form row
row() { od -A n -j "$1" -N 16 -t f4 "$OUT" | xargs; }

# close EXPECTED... ACTUAL... -> 0 if every pair is within 1e-3
close() {
  awk -v e="$1" -v a="$2" 'BEGIN{
    n = split(e, E, " "); split(a, A, " ");
    for (i = 1; i <= n; i++) { d = E[i] - A[i]; if (d < 0) d = -d; if (d > 1e-3) exit 1 }
    exit 0 }'
}

ok=1
for spec in "280:1.299038 -1.0 0 -11.160254" \
            "296:0.75 1.732051 0 -0.669873" \
            "312:0 0 1 0"; do
  off=${spec%%:*}; exp=${spec#*:}
  act=$(row "$off")
  echo "srow at $off: expected '$exp' actual '$act'"
  close "$exp" "$act" || ok=0
done

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
