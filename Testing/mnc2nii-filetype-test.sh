#!/bin/bash
# Usage: mnc2nii-filetype-test.sh <mnc2nii> <nii2mnc> <mincstats> <mnc_in> <exp_sum>
#
# Verifies the mnc2nii output file types other than the default one-file .nii:
#
#   -dual            -> <stem>.hdr + <stem>.img, magic "ni1"
#   name <stem>.hdr  -> <stem>.hdr + <stem>.img, magic "ni1"
#   name <stem>.img  -> <stem>.hdr + <stem>.img, magic "ni1"
#   -analyze         -> <stem>.hdr + <stem>.img, no NIfTI magic
#   -ASCII           -> <stem>.nia
#
# For each case, no <stem>.nii may be made, and nii2mnc must read the output
# back with the same voxel sum. mnc2nii used to make the file names before it
# set the file type, so each case wrote a single <stem>.nii that held only the
# voxel data (the header was overwritten).
set -u

M2N="$1"; N2M="$2"; MS="$3"; MNC="$4"; EXP_SUM="$5"
ok=1

within() {
  awk -v e="$1" -v a="$2" 'BEGIN{ d=a-e; if(d<0)d=-d; exit (d<=1e-6)?0:1 }'
}

# check STEM FLAGS NAME READ_FILE MAGIC
check() {
  local stem=$1 flags=$2 name=$3 readf=$4 magic=$5
  rm -f "$stem".nii "$stem".hdr "$stem".img "$stem".nia "$stem"_rt.mnc
  # flags is word-split on purpose (it can be empty)
  if ! "$M2N" $flags -float "$MNC" "$name" >/dev/null 2>&1; then
    echo "FAIL [$stem]: mnc2nii $flags failed"; ok=0; return
  fi
  if [ -e "$stem.nii" ]; then
    echo "FAIL [$stem]: $stem.nii was made"; ok=0
  fi
  if [ ! -s "$readf" ]; then
    echo "FAIL [$stem]: $readf is missing"; ok=0; return
  fi
  if [ -n "$magic" ]; then
    local m
    m=$(dd if="$readf" bs=1 skip=344 count=3 2>/dev/null)
    [ "$m" = "$magic" ] || { echo "FAIL [$stem]: magic '$m', expected '$magic'"; ok=0; }
  fi
  if ! "$N2M" -quiet -clobber -double "$readf" "$stem"_rt.mnc >/dev/null 2>&1; then
    echo "FAIL [$stem]: nii2mnc cannot read $readf"; ok=0; return
  fi
  local sum
  sum=$("$MS" -quiet -sum "$stem"_rt.mnc 2>/dev/null)
  echo "[$stem] $flags $name: read $readf, sum $sum (expected $EXP_SUM)"
  within "$EXP_SUM" "$sum" || { echo "FAIL [$stem]: sum"; ok=0; }
}

check ft_dual    "-dual"    ft_dual       ft_dual.hdr    ni1
check ft_hdrname ""         ft_hdrname.hdr ft_hdrname.hdr ni1
check ft_imgname ""         ft_imgname.img ft_imgname.hdr ni1
check ft_analyze "-analyze" ft_analyze    ft_analyze.hdr ""
check ft_ascii   "-ASCII"   ft_ascii      ft_ascii.nia   ""

if [ "$ok" = 1 ]; then echo "RESULT: PASS"; exit 0; fi
echo "RESULT: FAIL"; exit 1
