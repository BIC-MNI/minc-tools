#!/bin/bash
# Usage: nii2mnc-intensity-test.sh <nii2mnc> <mincinfo> <mincstats> \
#            <nii_in> <exp_vartype> <exp_min> <exp_max> <exp_sum> <tol>
#
# Verifies that nii2mnc reads a given NIfTI voxel datatype correctly and
# preserves real intensities (including scl_slope/scl_inter scaling):
#
#   * convert with -double so the *real* values are compared without
#     re-quantization, and check min/max/sum against the expected values
#     (within <tol>); and
#   * convert with no type flag and check that the default MINC voxel type
#     (sign + netCDF type, e.g. "unsigned byte", "signed__ short", "float")
#     matches <exp_vartype>, proving the datatype mapping.
#
# Fixtures live in nii2mnc_data/ and are built by make_nii_fixtures.py.
set -u

N2M="$1"; MI="$2"; MS="$3"; NII="$4"; EXP_VT="$5"
EXP_MIN="$6"; EXP_MAX="$7"; EXP_SUM="$8"; TOL="$9"

stem="$(basename "$NII" .nii)"
OUT="${stem}_dbl.mnc"
OUTN="${stem}_native.mnc"
rm -f "$OUT" "$OUTN"

# within EXPECTED ACTUAL TOL -> 0 if |actual-expected| <= tol
within() {
  awk -v e="$1" -v a="$2" -v t="$3" \
    'BEGIN{ d=a-e; if(d<0)d=-d; exit (d<=t)?0:1 }'
}

# --- intensity check: convert forcing -double, compare real min/max/sum ---
if ! "$N2M" -clobber -double "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc -double failed to convert $NII"; exit 1
fi
A_MIN=$("$MS" -quiet -min "$OUT" 2>/dev/null)
A_MAX=$("$MS" -quiet -max "$OUT" 2>/dev/null)
A_SUM=$("$MS" -quiet -sum "$OUT" 2>/dev/null)

echo "nii: $NII"
echo "  min: expected $EXP_MIN actual $A_MIN (tol $TOL)"
echo "  max: expected $EXP_MAX actual $A_MAX (tol $TOL)"
echo "  sum: expected $EXP_SUM actual $A_SUM (tol $TOL)"

ok=1
within "$EXP_MIN" "$A_MIN" "$TOL" || ok=0
within "$EXP_MAX" "$A_MAX" "$TOL" || ok=0
within "$EXP_SUM" "$A_SUM" "$TOL" || ok=0

# --- datatype check: convert with default type, compare sign + netCDF type ---
if ! "$N2M" -clobber "$NII" "$OUTN" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc (default type) failed to convert $NII"; exit 1
fi
SG=$("$MI" -attvalue image:signtype "$OUTN" 2>/dev/null)
VT=$("$MI" -vartype image "$OUTN" 2>/dev/null)
A_VT=$(printf '%s %s' "$SG" "$VT" | xargs)
echo "  vartype: expected '$EXP_VT' actual '$A_VT'"
[ "$A_VT" = "$EXP_VT" ] || ok=0

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
