#!/bin/bash
# Usage: nii2mnc-vector-test.sh <nii2mnc> <mincinfo> <mincextract> <nii_vector.nii>
#
# Verifies that nii2mnc keeps the components of each voxel together for a
# NIfTI dim[5] vector image (the layout of an ITK/ANTs displacement field).
# NIfTI stores dim[5] slowest; MINC's vector_dimension is the fastest MINC
# dimension, so nii2mnc must interleave the components. It used to write the
# NIfTI buffer unchanged, so each MINC voxel got 3 neighbouring values of
# component 0.
#
# The fixture (make_nii_vector_fixture.py) is 4x3x2 with 3 components;
# component c of voxel n (x fastest) holds 100*c + n.
set -u

N2M="$1"; MI="$2"; MX="$3"; NII="$4"
OUT=n2m_vector.mnc
rm -f "$OUT"

if ! "$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed on $NII"; exit 1
fi

ok=1
dims=$("$MI" -vardims image "$OUT" | xargs)
echo "dimensions: '$dims'"
[ "$dims" = "zspace yspace xspace vector_dimension" ] || ok=0

# components START (z,y,x) -> the 3 values at that voxel
comps() {
  "$MX" -float -start "$1,0" -count 1,1,1,3 "$OUT" 2>/dev/null |
    od -A n -t f4 | xargs
}

for spec in "0,0,0:0 100 200" "0,0,1:1 101 201" "0,1,0:4 104 204" \
            "1,2,3:23 123 223"; do
  start=${spec%%:*}; exp=${spec#*:}
  act=$(comps "$start")
  echo "voxel (z,y,x)=($start): expected '$exp' actual '$act'"
  [ "$act" = "$exp" ] || ok=0
done

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
