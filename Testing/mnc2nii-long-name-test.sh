#!/bin/bash
# Usage: mnc2nii-long-name-test.sh <mnc2nii> <mnc_in>
#
# Verifies that mnc2nii writes to the requested path when the file name is
# longer than 1024 characters. mnc2nii used to strncpy() the name into an
# uninitialized 1024-byte buffer: the name was cut, had no terminating NUL,
# and the output went to a different (cut) path, with exit status 0.
#
# The test makes a directory tree whose path is about 1220 characters long
# (each name is 200 characters) and converts:
#   * with an explicit output name in that tree, and
#   * with one argument, a copy of the input in that tree.
set -u

M2N="$1"; MNC="$2"
ok=1

seg=$(printf 'd%.0s' $(seq 1 200))
DEEP="m2n_long/$seg/$seg/$seg/$seg/$seg/$seg"
rm -rf m2n_long
mkdir -p "$DEEP" || { echo "FAIL: mkdir"; exit 1; }
echo "path length: $(printf '%s' "$DEEP/out.nii" | wc -c)"

# magic FILE -> the NIfTI magic string
magic() { dd if="$1" bs=1 skip=344 count=3 2>/dev/null; }

"$M2N" "$MNC" "$DEEP/out.nii" >/dev/null 2>&1
rc=$?
echo "explicit output name: exit status $rc, magic '$(magic "$DEEP/out.nii")'"
[ "$rc" = 0 ] && [ "$(magic "$DEEP/out.nii")" = "n+1" ] ||
  { echo "FAIL: explicit long output name"; ok=0; }

cp "$MNC" "$DEEP/in.mnc"
"$M2N" "$DEEP/in.mnc" >/dev/null 2>&1
rc=$?
echo "one argument: exit status $rc, magic '$(magic "$DEEP/in.nii")'"
[ "$rc" = 0 ] && [ "$(magic "$DEEP/in.nii")" = "n+1" ] ||
  { echo "FAIL: long input name"; ok=0; }

rm -rf m2n_long
if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
