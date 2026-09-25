#!/bin/bash
# Usage: mnc2nii-integer-range-test.sh <mnc2nii> <rawtominc>
#
# Verifies that mnc2nii copies integer voxels exactly when the stored values
# of an integer MINC file fit the requested integer output type. mnc2nii used
# to map the input onto a "nearest power of two" output range, which
# requantized the data: labels 0..116 became 0..127 with scl_slope 0.913, so
# the labels read back as non-integers.
#
# Case 1: unsigned byte labels 0..116 (real = stored)
#   -short, -byte -unsigned, -int: stored value k at voxel k, and the NIfTI
#   scaling gives real = stored (scl_slope 1, scl_inter 0)
# Case 2: signed short 0..100, real range 0..10 (step 0.1)
#   -short: stored value k at voxel k, scl_slope 0.1, scl_inter 0
set -u

M2N="$1"; R2M="$2"
ok=1
rm -f m2n_int_*

field() { od -A n -j "$1" -N "$2" -t "$3" "$4" | xargs; }
close() {  # close LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  awk -v e="$2" -v a="$3" 'BEGIN{
    n = split(e, E, " "); if (split(a, A, " ") != n) exit 1;
    for (i = 1; i <= n; i++) { d = E[i] - A[i]; if (d < 0) d = -d;
      if (d > 1e-6) exit 1 }
    exit 0 }' || ok=0
}

# bytes N -> N little-endian integers 0..N-1 of SIZE bytes (1 or 2)
ramp() {
  local k
  for ((k = 0; k < $1; k++)); do
    printf "\\$(printf '%03o' "$k")"
    [ "$2" = 2 ] && printf '\000'
  done
}

ramp 117 1 | "$R2M" -byte -unsigned -clobber -range 0 116 -real_range 0 116 \
  -dimorder zspace,yspace,xspace m2n_int_labels.mnc 1 1 117 || exit 1
for spec in "-short:d2:2" "-byte -unsigned:u1:1" "-int:d4:4"; do
  flags=${spec%%:*}; rest=${spec#*:}; type=${rest%%:*}; size=${rest#*:}
  tag=$(printf '%s' "$flags" | tr -cd 'a-z')
  F=m2n_int_labels_$tag.nii
  # flags is word-split on purpose
  "$M2N" -nii $flags m2n_int_labels.mnc "$F" >/dev/null 2>&1 ||
    { echo "FAIL: mnc2nii $flags"; ok=0; continue; }
  close "labels $flags stored 0 37 116" "0 37 116" \
    "$(field 352 "$size" "$type" $F) $(field $((352 + 37 * size)) "$size" "$type" $F) $(field $((352 + 116 * size)) "$size" "$type" $F)"
  close "labels $flags scl_slope scl_inter" "1 0" "$(field 112 8 f4 $F)"
done

ramp 101 2 | "$R2M" -short -signed -clobber -range 0 100 -real_range 0 10 \
  -dimorder zspace,yspace,xspace m2n_int_scaled.mnc 1 1 101 || exit 1
F=m2n_int_scaled.nii
"$M2N" -nii -short m2n_int_scaled.mnc $F >/dev/null 2>&1 ||
  { echo "FAIL: mnc2nii -short"; exit 1; }
close "scaled -short stored 0 37 100" "0 37 100" \
  "$(field 352 2 d2 $F) $(field 426 2 d2 $F) $(field 552 2 d2 $F)"
close "scaled -short scl_slope scl_inter" "0.1 0" "$(field 112 8 f4 $F)"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
