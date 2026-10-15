#!/bin/bash
# Usage: mnc2nii-long-name-test.sh <mnc2nii> <mnc_in>
#
# Verifies that mnc2nii writes to the requested path when the file name is
# longer than 1024 characters. mnc2nii used to strncpy() the name into an
# uninitialized 1024-byte buffer: the name was cut, had no terminating NUL,
# and the output went to a different (cut) path, with exit status 0.
#
# The test makes a directory tree of 200-character names and converts:
#   * with an explicit output name in that tree, and
#   * with one argument, a copy of the input in that tree.
#
# The tree is six names deep (a path of about 1220 characters) where the
# system allows it. A path must be shorter than PATH_MAX: Linux allows
# 4096 bytes, but macOS allows only 1024, so there the tree has fewer
# names and the path is about 1020 characters. That still checks a long
# name, but it cannot reproduce the old overflow, which needs a name of
# 1024 characters or more.
set -u

M2N="$1"; MNC="$2"
ok=1

limit=$(getconf PATH_MAX . 2>/dev/null)
case "$limit" in
  ''|*[!0-9]*) limit=4096 ;;
esac

seg=$(printf 'd%.0s' $(seq 1 200))
DEEP=m2n_long
for level in 1 2 3 4 5 6; do
  p="$DEEP/$seg/out.nii"
  [ "${#p}" -lt "$limit" ] || break
  DEEP="$DEEP/$seg"
done
[ "$DEEP" != m2n_long ] || { echo "FAIL: PATH_MAX $limit is too small"; exit 1; }
rm -rf m2n_long
mkdir -p "$DEEP" || { echo "FAIL: mkdir"; exit 1; }
p="$DEEP/out.nii"
echo "path length: ${#p} (PATH_MAX $limit)"

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
