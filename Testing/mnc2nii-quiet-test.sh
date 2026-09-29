#!/bin/bash
# Usage: mnc2nii-quiet-test.sh <mnc2nii> <mnc_in>
#
# Verifies that mnc2nii prints nothing to stdout unless -verbose is given
# (quiet is the default). mnc2nii used to print debug lines ("found xspace
# at ...", a dimension table, "Restructuring...") in every mode.
set -u

M2N="$1"; MNC="$2"
ok=1
rm -f m2n_quiet*.nii

for flags in "" "-quiet"; do
  # flags is word-split on purpose (it can be empty)
  out=$("$M2N" $flags "$MNC" m2n_quiet.nii 2>/dev/null)
  echo "mnc2nii $flags: $(printf '%s' "$out" | wc -l) stdout lines"
  [ -z "$out" ] || { echo "FAIL: stdout is not empty:"; echo "$out"; ok=0; }
done

out=$("$M2N" -verbose "$MNC" m2n_quiet_v.nii 2>/dev/null)
echo "mnc2nii -verbose: $(printf '%s\n' "$out" | wc -l) stdout lines"
[ -n "$out" ] || { echo "FAIL: -verbose printed nothing"; ok=0; }

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
