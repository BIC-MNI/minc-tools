#!/bin/bash
# Usage: nii2mnc-descrip-test.sh <nii2mnc> <mincinfo> <nii_in> <exp_descrip>
#
# Verifies where nii2mnc puts the NIfTI descrip text. descrip is free text
# (nifti1.h: "any text you like"; dcm2niix writes TE, time and phase there),
# so it goes into the global "comments" attribute. nii2mnc used to copy it
# into patient:full_name, a field for identifying data that
# de-identification tools remove or flag.
set -u

N2M="$1"; MI="$2"; NII="$3"; EXP="$4"
OUT="$(basename "$NII" .nii)_descrip.mnc"
rm -f "$OUT"

if ! "$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed on $NII"; exit 1
fi

ok=1
name=$("$MI" -attvalue patient:full_name "$OUT" 2>/dev/null)
comments=$("$MI" -attvalue :comments "$OUT" 2>/dev/null)
echo "patient:full_name: expected '' actual '$name'"
echo ":comments: expected '$EXP' actual '$comments'"
[ -z "$name" ] || ok=0
[ "$comments" = "$EXP" ] || ok=0

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
