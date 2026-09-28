#!/bin/bash
# Usage: nii2mnc-long-name-test.sh <nii2mnc> <mincstats> <nii_in>
#
# Verifies that nii2mnc accepts file names longer than 1024 characters.
# nii2mnc used to strcpy() the name into a 1024-byte stack buffer, which
# overflowed it ("stack smashing detected", exit status 134).
#
# The test makes a directory tree whose path is about 1220 characters long
# (each name is 200 characters) and converts:
#   * with an explicit output name in that tree, and
#   * with one argument, a copy of the input in that tree.
set -u

N2M="$1"; MS="$2"; NII="$3"
ok=1

seg=$(printf 'd%.0s' $(seq 1 200))
DEEP="n2m_long/$seg/$seg/$seg/$seg/$seg/$seg"
rm -rf n2m_long
mkdir -p "$DEEP" || { echo "FAIL: mkdir"; exit 1; }
echo "path length: $(printf '%s' "$DEEP/out.mnc" | wc -c)"

"$N2M" -quiet -clobber "$NII" "$DEEP/out.mnc" >/dev/null 2>&1
rc=$?
echo "explicit output name: exit status $rc"
[ "$rc" = 0 ] && "$MS" -quiet -sum "$DEEP/out.mnc" >/dev/null 2>&1 ||
  { echo "FAIL: explicit long output name"; ok=0; }

cp "$NII" "$DEEP/in.nii"
"$N2M" -quiet -clobber "$DEEP/in.nii" >/dev/null 2>&1
rc=$?
echo "one argument: exit status $rc"
[ "$rc" = 0 ] && "$MS" -quiet -sum "$DEEP/in.mnc" >/dev/null 2>&1 ||
  { echo "FAIL: long input name"; ok=0; }

rm -rf n2m_long
if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
