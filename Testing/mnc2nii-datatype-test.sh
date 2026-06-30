#!/bin/bash
# Usage: mnc2nii-datatype-test.sh <mnc2nii> <nii2mnc> <mincstats> \
#            <mnc_in> <flag> <exp_dtcode> <exp_sum> <tol>
#
# Verifies that mnc2nii honors a requested output voxel datatype on disk and
# preserves intensities:
#
#   * run "mnc2nii <flag> <mnc_in> out.nii" (flag is e.g. "-short" or
#     "-byte -unsigned", passed through unquoted on purpose);
#   * read the NIfTI datatype field (little-endian int16 at header offset 70)
#     and check it equals <exp_dtcode> (UINT8=2, INT16=4, INT32=8, FLOAT32=16,
#     FLOAT64=64, INT8=256, UINT16=512, UINT32=768); and
#   * round-trip the .nii back through "nii2mnc -double" and check the voxel
#     sum is within <tol> of <exp_sum>.
#
# Run in a ctest working directory; the input MINC is given by absolute path.
set -u

M2N="$1"; N2M="$2"; MS="$3"; MNC="$4"; FLAG="$5"
EXP_DT="$6"; EXP_SUM="$7"; TOL="$8"

# A safe filename stem from the flag (strip dashes/spaces).
tag=$(printf '%s' "$FLAG" | tr -cd 'a-z')
OUT="m2n_${tag}.nii"
RT="m2n_${tag}_rt.mnc"
rm -f "$OUT" "$RT"

within() {
  awk -v e="$1" -v a="$2" -v t="$3" \
    'BEGIN{ d=a-e; if(d<0)d=-d; exit (d<=t)?0:1 }'
}

# FLAG is intentionally word-split (it may be two tokens like "-byte -unsigned").
if ! "$M2N" $FLAG "$MNC" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: mnc2nii $FLAG failed on $MNC"; exit 1
fi
[ -f "$OUT" ] || { echo "FAIL: $OUT not created"; exit 1; }

# NIfTI datatype: signed int16 at byte offset 70.
A_DT=$(od -A n -j 70 -N 2 -t d2 "$OUT" | tr -d ' ')

if ! "$N2M" -clobber -double "$OUT" "$RT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc round-trip failed on $OUT"; exit 1
fi
A_SUM=$("$MS" -quiet -sum "$RT" 2>/dev/null)

echo "mnc2nii flag: '$FLAG'"
echo "  datatype: expected $EXP_DT actual $A_DT"
echo "  sum:      expected $EXP_SUM actual $A_SUM (tol $TOL)"

ok=1
[ "$A_DT" = "$EXP_DT" ] || ok=0
within "$EXP_SUM" "$A_SUM" "$TOL" || ok=0

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
