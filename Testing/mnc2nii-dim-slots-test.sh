#!/bin/bash
# Usage: mnc2nii-dim-slots-test.sh <mnc2nii> <rawtominc> <mincconcat>
#
# Verifies that mnc2nii puts each MINC dimension into its NIfTI slot by name
# (nifti1.h: dimensions 1-3 are space, 4 is time, 5 holds the values at each
# voxel), and not by position. mnc2nii used to fill the slots in order, so a
# missing dimension moved the others: 2-D + time put time in dim[3] (as a
# spatial axis in mm), and a vector with no time went into dim[4] (time).
#
# Case 1: time(3, step 2.5) x yspace(5) x xspace(6)
#   expected dim = 4 6 5 1 3, pixdim[4] = 2.5, xyzt_units = 10 (mm, s)
# Case 2: zspace(2) x yspace(3) x xspace(4) x vector_dimension(3), unsigned
#   byte, component c of voxel n (x fastest) = 30*c + n
#   expected dim = 5 4 3 2 1 3, intent_code = 1007 (NIFTI_INTENT_VECTOR),
#   and dim[5] stored slowest: float32 value at 352 + 4*(n + 24*c)
set -u

M2N="$1"; R2M="$2"; CONCAT="$3"
ok=1
rm -f m2n_slot_*

# header OFF COUNT TYPE FILE -> values
field() { od -A n -j "$1" -N "$2" -t "$3" "$4" | xargs; }

# dims FILE -> dim[0] .. dim[dim[0]]
dims() { field 40 16 d2 "$1" | awk '{ for (i = 1; i <= $1 + 1; i++) printf "%s%s", (i > 1 ? " " : ""), $i; print "" }'; }

check() {  # check LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  [ "$2" = "$3" ] || ok=0
}

# --- case 1: 2-D + time ---
head -c $((5 * 6 * 4)) /dev/zero |
  "$R2M" -float -clobber -dimorder yspace,xspace m2n_slot_sl.mnc 5 6 || exit 1
"$CONCAT" -clobber -concat_dimension time -start 10 -step 2.5 \
  m2n_slot_sl.mnc m2n_slot_sl.mnc m2n_slot_sl.mnc m2n_slot_xyt.mnc \
  >/dev/null 2>&1 || { echo "FAIL: mincconcat"; exit 1; }
"$M2N" -nii m2n_slot_xyt.mnc m2n_slot_xyt.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii on 2-D + time"; exit 1; }
F=m2n_slot_xyt.nii
check "2-D+time dim" "4 6 5 1 3" "$(dims $F)"
check "2-D+time pixdim[4]" "2.5" "$(field 92 4 f4 $F)"
check "2-D+time xyzt_units" "10" "$(field 123 1 u1 $F)"

# --- case 2: 3-D + vector ---
for z in 0 1; do for y in 0 1 2; do for x in 0 1 2 3; do for c in 0 1 2; do
  printf "\\$(printf '%03o' $((30 * c + x + 4 * (y + 3 * z))))"
done; done; done; done |
  "$R2M" -byte -unsigned -clobber -real_range 0 255 \
    -dimorder zspace,yspace,xspace -vector 3 m2n_slot_vec.mnc 2 3 4 ||
  exit 1
"$M2N" -nii m2n_slot_vec.mnc m2n_slot_vec.nii >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii on vector"; exit 1; }
F=m2n_slot_vec.nii
check "vector dim" "5 4 3 2 1 3" "$(dims $F)"
check "vector intent_code" "1007" "$(field 68 2 d2 $F)"
for spec in "0:0" "1:1" "23:23" "24:30" "25:31" "48:60" "71:83"; do
  k=${spec%%:*}; exp=${spec#*:}
  check "vector value $k" "$exp" "$(field $((352 + 4 * k)) 4 f4 $F)"
done

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
