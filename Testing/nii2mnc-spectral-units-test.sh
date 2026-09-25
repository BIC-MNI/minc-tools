#!/bin/bash
# Usage: nii2mnc-spectral-units-test.sh <nii2mnc> <mincinfo> <nii_in> <exp_units>
#
# Verifies that nii2mnc converts the 4th NIfTI dimension when its units are
# spectral (nifti1.h: NIFTI_UNITS_HZ, _PPM, _RADS). nii2mnc used to print
# "Unknown time units value", leave the time step and start uninitialized,
# and label them seconds.
#
# The fixtures (make_nii_spectral_fixtures.py) have pixdim[4] = 2.5 and
# toffset = 100. Expected: time:step = 2.5, time:start = 100, time:units =
# <exp_units>.
set -u

N2M="$1"; MI="$2"; NII="$3"; EXP_UNITS="$4"
OUT="$(basename "$NII" .nii)_spec.mnc"
rm -f "$OUT"

if ! "$N2M" -quiet -clobber "$NII" "$OUT" >/dev/null 2>&1; then
  echo "FAIL: nii2mnc failed on $NII"; exit 1
fi

ok=1
check() {  # check LABEL EXPECTED ACTUAL
  echo "$1: expected '$2' actual '$3'"
  [ "$2" = "$3" ] || ok=0
}
check "time:step" "2.5" "$("$MI" -attvalue time:step "$OUT" | xargs)"
check "time:start" "100" "$("$MI" -attvalue time:start "$OUT" | xargs)"
check "time:units" "$EXP_UNITS" "$("$MI" -attvalue time:units "$OUT" | xargs)"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
