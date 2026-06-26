#!/bin/bash
# Usage: dcm2mnc-compare-test.sh <dcm2mnc> <mincinfo> <mincstats> \
#                                <outdir> <indir> <manifest>
#
# Converts a DICOM series with dcm2mnc and checks the resulting MINC volume(s)
# against a committed manifest whose reference values were derived from the
# dcm2niix ground truth (DICOM -> dcm2niix -> nii2mnc; see
# dcm2mnc_expected/regenerate.sh).  This is a regression test against an
# independent converter, not against dcm2mnc's own past output.
#
# Each manifest block is keyed by SeriesInstanceUID.  Outputs are joined to
# reference blocks by UID -- dcm2mnc records it in dicom_0x0020:el_0x000e --
# never by filename or by nearest centre of mass.  Within one UID (e.g. a
# multi-echo split) blocks and outputs are paired in ascending-mean order.
#
# Only DICOM files are fed to dcm2mnc (non-DICOM companions such as
# .nii/.json/.bval/.bvec/.csv/.xlsx/README are filtered out and the rest
# passed via -stdin), so mixed-content input directories work transparently.
#
# Compared quantities (orientation-invariant; dcm2mnc and dcm2niix may store
# an axis with opposite polarity for the same physical volume):
#   dimlen   sorted multiset of dimension lengths (length-1 axes squeezed)
#   voxsize  sorted |step| of the spatial axes (mm)
#   com      centre of mass in world coordinates (mm)
#   mean / stddev   image statistics
#
# Manifest format ('#' comments and blank lines ignored):
#   n_mnc     <count>                       expected number of .mnc outputs
#   tol_vox <v>  tol_com <v>  tol_rel <v>   (optional tolerance overrides)
#   ref       <SeriesInstanceUID>           starts a reference block
#   dimlen    <l1> <l2> ...
#   voxsize   <s1> <s2> ...
#   com       <x> <y> <z>
#   mean      <value>
#   stddev    <value>
set -u

DCM2MNC="$1"; MINCINFO="$2"; MINCSTATS="$3"
OUTDIR="$4"; INDIR="$5"; MANIFEST="$6"

# ---- tolerances (overridable from manifest) ------------------------------
tol_vox=0.001       # mm, absolute, per spatial voxel size
tol_com=0.05        # mm, absolute, per world axis
tol_rel=0.001       # relative, for mean/stddev

abs_ok() { awk -v a="$1" -v b="$2" -v t="$3" \
  'BEGIN{d=a-b; if(d<0)d=-d; exit !(d<=t)}'; }
rel_ok() { awk -v a="$1" -v b="$2" -v t="$3" \
  'BEGIN{d=a-b; if(d<0)d=-d; m=(b<0)?-b:b; r=(m==0)?d:d/m; exit !(r<=t)}'; }
trim()  { tr -d ' '; }

fail=0
report() {  # status field expected got
  if [ "$1" = ok ]; then printf 'PASS  %-22s %s\n' "$2" "$4"
  else printf 'FAIL  %-22s expected %s  got %s\n' "$2" "$3" "$4"; fail=1; fi
}

[ -f "$MANIFEST" ] || { echo "FAIL: manifest not found: $MANIFEST"; exit 1; }

# ---- parse manifest ------------------------------------------------------
exp_nmnc=1
declare -a R_UID R_DIM R_VOX R_COM R_MEAN R_STD
blk=-1
while read -r key rest; do
  rest=$(echo "$rest" | xargs)        # collapse alignment whitespace
  case "$key" in
    ''|\#*)  continue ;;
    n_mnc)   exp_nmnc="$rest" ;;
    tol_vox) tol_vox="$rest" ;;
    tol_com) tol_com="$rest" ;;
    tol_rel) tol_rel="$rest" ;;
    ref)     blk=$((blk+1)); R_UID[$blk]="$rest"
             R_DIM[$blk]=""; R_VOX[$blk]=""; R_COM[$blk]=""
             R_MEAN[$blk]=""; R_STD[$blk]="" ;;
    dimlen)  R_DIM[$blk]="$rest" ;;
    voxsize) R_VOX[$blk]="$rest" ;;
    com)     R_COM[$blk]="$rest" ;;
    mean)    R_MEAN[$blk]="$rest" ;;
    stddev)  R_STD[$blk]="$rest" ;;
    *) echo "WARN: unknown manifest key '$key'" ;;
  esac
done < "$MANIFEST"
NBLK=$((blk+1))
# normalize whitespace in the multi-value fields
for i in $(seq 0 $((NBLK-1))); do
  R_DIM[$i]=$(echo "${R_DIM[$i]}" | xargs)
  R_VOX[$i]=$(echo "${R_VOX[$i]}" | xargs)
  R_COM[$i]=$(echo "${R_COM[$i]}" | xargs)
done

# ---- convert (DICOM-only via -stdin) -------------------------------------
mkdir -p "$OUTDIR"
find "$OUTDIR" -name '*.mnc' -delete 2>/dev/null
find "$INDIR" -maxdepth 1 -type f \
  -not -iname '*.nii'  -not -iname '*.nii.gz' -not -iname '*.json' \
  -not -iname '*.bval' -not -iname '*.bvec'   -not -iname '*.csv' \
  -not -iname '*.xlsx' -not -iname '*.txt'    -not -iname '*.md' \
  -not -iname 'README*' -not -iname '.*' \
  | "$DCM2MNC" -clobber -stdin "$OUTDIR"

mapfile -t MNC < <(find "$OUTDIR" -name '*.mnc' | sort)
NMNC=${#MNC[@]}

# ---- output count --------------------------------------------------------
if [ "$NMNC" -eq "$exp_nmnc" ]; then report ok n_mnc "$exp_nmnc" "$NMNC"
else report bad n_mnc "$exp_nmnc" "$NMNC"; echo "RESULT: FAIL"; exit 1; fi

# ---- measure each produced volume ----------------------------------------
declare -a M_UID M_DIM M_VOX M_COM M_MEAN M_STD
measure() {  # $1 = mnc file ; $2 = index
  local F="$1" i="$2" d l s dim="" vox=""
  M_UID[$i]=$("$MINCINFO" -attvalue dicom_0x0020:el_0x000e "$F" 2>/dev/null | trim)
  for d in $("$MINCINFO" -dimnames "$F" 2>/dev/null); do
    l=$("$MINCINFO" -dimlength "$d" "$F" 2>/dev/null | trim)
    [ -n "$l" ] && [ "$l" -gt 1 ] && dim+="$l
"
  done
  M_DIM[$i]=$(printf '%s' "$dim" | sort -n | xargs)
  for d in xspace yspace zspace; do
    s=$("$MINCINFO" -attvalue "$d:step" "$F" 2>/dev/null | trim)
    [ -n "$s" ] && vox+="$(awk -v x="$s" 'BEGIN{printf "%.10g",(x<0?-x:x)}')
"
  done
  M_VOX[$i]=$(printf '%s' "$vox" | sort -g | xargs)
  M_COM[$i]=$("$MINCSTATS" -quiet -com -world_only "$F" 2>/dev/null | xargs)
  M_MEAN[$i]=$("$MINCSTATS" -quiet -mean "$F" 2>/dev/null)
  M_STD[$i]=$("$MINCSTATS" -quiet -stddev "$F" 2>/dev/null)
}
for i in "${!MNC[@]}"; do measure "${MNC[$i]}" "$i"; done

# ---- compare one output against one reference block ----------------------
cmp_block() {  # $1 = block idx ; $2 = mnc idx ; $3 = label prefix
  local b="$1" m="$2" pfx="$3"
  [ "${M_DIM[$m]}" = "${R_DIM[$b]}" ] \
    && report ok "${pfx}dimlen" "${R_DIM[$b]}" "${M_DIM[$m]}" \
    || report bad "${pfx}dimlen" "${R_DIM[$b]}" "${M_DIM[$m]}"

  # voxel sizes: same count, each within tol
  local -a ev mv; read -r -a ev <<< "${R_VOX[$b]}"; read -r -a mv <<< "${M_VOX[$m]}"
  if [ "${#ev[@]}" -ne "${#mv[@]}" ]; then
    report bad "${pfx}voxsize" "${R_VOX[$b]}" "${M_VOX[$m]}"
  else
    local k okv=1
    for k in "${!ev[@]}"; do abs_ok "${mv[$k]}" "${ev[$k]}" "$tol_vox" || okv=0; done
    [ "$okv" = 1 ] && report ok "${pfx}voxsize" "${R_VOX[$b]}" "${M_VOX[$m]}" \
                   || report bad "${pfx}voxsize" "${R_VOX[$b]}" "${M_VOX[$m]}"
  fi

  # centre of mass: per world axis
  local ex ey ez cx cy cz
  read -r ex ey ez <<< "${R_COM[$b]}"
  read -r cx cy cz <<< "${M_COM[$m]}"
  abs_ok "$cx" "$ex" "$tol_com" && report ok "${pfx}com.x" "$ex" "$cx" || report bad "${pfx}com.x" "$ex" "$cx"
  abs_ok "$cy" "$ey" "$tol_com" && report ok "${pfx}com.y" "$ey" "$cy" || report bad "${pfx}com.y" "$ey" "$cy"
  abs_ok "$cz" "$ez" "$tol_com" && report ok "${pfx}com.z" "$ez" "$cz" || report bad "${pfx}com.z" "$ez" "$cz"

  rel_ok "${M_MEAN[$m]}" "${R_MEAN[$b]}" "$tol_rel" \
    && report ok "${pfx}mean" "${R_MEAN[$b]}" "${M_MEAN[$m]}" \
    || report bad "${pfx}mean" "${R_MEAN[$b]}" "${M_MEAN[$m]}"
  rel_ok "${M_STD[$m]}" "${R_STD[$b]}" "$tol_rel" \
    && report ok "${pfx}stddev" "${R_STD[$b]}" "${M_STD[$m]}" \
    || report bad "${pfx}stddev" "${R_STD[$b]}" "${M_STD[$m]}"
}

# ---- join outputs to reference blocks by SeriesInstanceUID ---------------
# Collect the distinct UIDs, then within each UID pair blocks and outputs in
# ascending-mean order (handles same-UID multi-output splits deterministically).
uids=$(for i in $(seq 0 $((NBLK-1))); do echo "${R_UID[$i]}"; done | sort -u)
for uid in $uids; do
  # block indices for this uid, sorted by reference mean
  bidx=$(for i in $(seq 0 $((NBLK-1))); do
           [ "${R_UID[$i]}" = "$uid" ] && echo "${R_MEAN[$i]} $i"; done | sort -g | awk '{print $2}')
  midx=$(for i in "${!MNC[@]}"; do
           [ "${M_UID[$i]}" = "$uid" ] && echo "${M_MEAN[$i]} $i"; done | sort -g | awk '{print $2}')
  mapfile -t BA <<< "$bidx"; mapfile -t MA <<< "$midx"
  # drop possible empty lines
  BA=($(printf '%s\n' "${BA[@]}")); MA=($(printf '%s\n' "${MA[@]}"))
  if [ "${#BA[@]}" -ne "${#MA[@]}" ]; then
    report bad "uid:$uid count" "${#BA[@]}" "${#MA[@]}"; continue
  fi
  for k in "${!BA[@]}"; do
    pfx=""; [ "$NBLK" -gt 1 ] && pfx="[$uid] "
    echo "match uid=$uid  ref-block=${BA[$k]}  -> ${MNC[${MA[$k]}]}"
    cmp_block "${BA[$k]}" "${MA[$k]}" "$pfx"
  done
done

# any output whose UID matched no reference block?
for i in "${!MNC[@]}"; do
  hit=0
  for j in $(seq 0 $((NBLK-1))); do [ "${M_UID[$i]}" = "${R_UID[$j]}" ] && hit=1; done
  [ "$hit" = 0 ] && report bad "unmatched-output" "(a ref UID)" "${M_UID[$i]:-<none>} ${MNC[$i]}"
done

[ "$fail" -eq 0 ] && { echo "RESULT: PASS"; exit 0; } || { echo "RESULT: FAIL"; exit 1; }
