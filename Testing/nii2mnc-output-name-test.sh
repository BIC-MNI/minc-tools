#!/bin/bash
# Usage: nii2mnc-output-name-test.sh <nii2mnc> <mincstats> <nii2mnc_data dir>
#
# Verifies that nii2mnc never writes its output over its own input:
#
#   * with one file argument, the output name is the input name with the
#     NIfTI extension (.nii, .nii.gz, .hdr, .img, ...) replaced by .mnc;
#   * an explicit output name that is the input file, or the image file of a
#     .hdr/.img pair, is refused (also with -clobber).
#
# nii2mnc used to remove only .nii and .hdr, so "nii2mnc -clobber x.nii.gz"
# and "nii2mnc -clobber x.img" replaced the input with the MINC file.
set -u

N2M="$1"; MS="$2"; DATA="$3"
ok=1
fail() { echo "FAIL: $*"; ok=0; }

rm -f on_*
cp "$DATA/nii_3d_ras.nii" on_ras.nii
gzip -c "$DATA/nii_3d_ras.nii" > on_gz.nii.gz
cp on_gz.nii.gz on_gz.orig
cp "$DATA/nii_pair.hdr" on_pair.hdr
cp "$DATA/nii_pair.img" on_pair.img

# unchanged FILE ORIGINAL
unchanged() { cmp -s "$1" "$2" || fail "$1 was changed"; }

# --- one argument: the output name replaces the NIfTI extension ---
"$N2M" -quiet -clobber on_gz.nii.gz >/dev/null 2>&1
unchanged on_gz.nii.gz on_gz.orig
[ -s on_gz.mnc ] && "$MS" -quiet -sum on_gz.mnc >/dev/null 2>&1 ||
  fail "on_gz.nii.gz did not give on_gz.mnc"

"$N2M" -quiet -clobber on_pair.img >/dev/null 2>&1
unchanged on_pair.img "$DATA/nii_pair.img"
unchanged on_pair.hdr "$DATA/nii_pair.hdr"
sum=$("$MS" -quiet -sum on_pair.mnc 2>/dev/null)
[ "$sum" = 276 ] || fail "on_pair.img did not give on_pair.mnc with sum 276 (got '$sum')"

rm -f on_pair.mnc
"$N2M" -quiet -clobber on_pair.hdr >/dev/null 2>&1
[ -s on_pair.mnc ] || fail "on_pair.hdr did not give on_pair.mnc"

# --- explicit output that is an input file: refused ---
for args in "on_ras.nii on_ras.nii" "on_ras.nii ./on_ras.nii" \
            "on_pair.hdr on_pair.img" "on_pair.hdr on_pair.hdr"; do
  # args is word-split on purpose
  if "$N2M" -quiet -clobber $args >/dev/null 2>&1; then
    fail "nii2mnc -clobber $args succeeded"
  fi
done
unchanged on_ras.nii "$DATA/nii_3d_ras.nii"
unchanged on_pair.hdr "$DATA/nii_pair.hdr"
unchanged on_pair.img "$DATA/nii_pair.img"

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
