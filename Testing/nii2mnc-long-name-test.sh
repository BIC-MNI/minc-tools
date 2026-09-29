#!/bin/bash
# Usage: nii2mnc-long-name-test.sh <nii2mnc> <mincstats> <nii_in>
#
# Verifies that nii2mnc accepts file names longer than 1024 characters.
# nii2mnc used to strcpy() the name into a 1024-byte stack buffer, which
# overflowed it ("stack smashing detected", exit status 134).
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

N2M="$1"; MS="$2"; NII="$3"
ok=1

limit=$(getconf PATH_MAX . 2>/dev/null)
case "$limit" in
  ''|*[!0-9]*) limit=4096 ;;
esac

seg=$(printf 'd%.0s' $(seq 1 200))
DEEP=n2m_long
for level in 1 2 3 4 5 6; do
  p="$DEEP/$seg/out.mnc"
  [ "${#p}" -lt "$limit" ] || break
  DEEP="$DEEP/$seg"
done
[ "$DEEP" != n2m_long ] || { echo "FAIL: PATH_MAX $limit is too small"; exit 1; }
rm -rf n2m_long
mkdir -p "$DEEP" || { echo "FAIL: mkdir"; exit 1; }
p="$DEEP/out.mnc"
echo "path length: ${#p} (PATH_MAX $limit)"

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
