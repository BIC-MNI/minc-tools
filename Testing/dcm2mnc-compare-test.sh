#!/bin/bash
# Usage: dcm2mnc-compare-test.sh <dcm2mnc> <mincinfo> <mincstats> \
#                                <outdir> <indir> <manifest>
#
# Converts a DICOM series with dcm2mnc and checks the resulting MINC volume(s)
# against a committed manifest of expected geometry and image statistics.
# This is a regression test: it fails if dcm2mnc changes the dimensions,
# voxel sampling, world coordinates, data type, mean/stddev, or centre of
# mass of the output.
#
# Only DICOM files are fed to dcm2mnc (non-DICOM files such as .nii/.json/
# .bval/.bvec/.csv/.xlsx/README are filtered out and the rest passed via
# -stdin), so mixed-content input directories work transparently.
#
# Manifest format ('#' comments and blank lines ignored):
#   Global directives (before the first "file" marker):
#     n_mnc     <count>         expected number of .mnc files (default 1)
#     tol_step  <v>  tol_start <v>  tol_rel <v>  tol_com <v>   (optional)
#   Per-output blocks (one per produced .mnc, compared in sorted order). A
#   block is introduced by a "file" marker; if no marker is present the whole
#   body is treated as a single block:
#     file      [label]
#     dim       <name> <length> <step> <start>     one line per dimension
#     vartype   <type>          e.g. short
#     signtype  <sign>          e.g. unsigned / signed__
#     mean      <value>
#     stddev    <value>
#     com       <x> <y> <z>     centre of mass, world coordinates
set -u

DCM2MNC="$1"; MINCINFO="$2"; MINCSTATS="$3"
OUTDIR="$4"; INDIR="$5"; MANIFEST="$6"

# ---- tolerances (overridable from manifest globals) ----------------------
tol_step=0.001      # mm, absolute
tol_start=0.01      # mm, absolute
tol_rel=0.002       # relative, for mean/stddev
tol_com=0.2         # mm, absolute, per axis

abs_ok() { awk -v a="$1" -v b="$2" -v t="$3" \
  'BEGIN{d=a-b; if(d<0)d=-d; exit !(d<=t)}'; }
rel_ok() { awk -v a="$1" -v b="$2" -v t="$3" \
  'BEGIN{d=a-b; if(d<0)d=-d; m=(b<0)?-b:b; r=(m==0)?d:d/m; exit !(r<=t)}'; }
q() { "$MINCINFO" "$@" 2>/dev/null | tr -d ' '; }

fail=0
report() {  # status field expected got
  if [ "$1" = ok ]; then printf 'PASS  %-18s %s\n' "$2" "$4"
  else printf 'FAIL  %-18s expected %s  got %s\n' "$2" "$3" "$4"; fail=1; fi
}

[ -f "$MANIFEST" ] || { echo "FAIL: manifest not found: $MANIFEST"; exit 1; }

# ---- parse manifest into globals + per-block serialized strings ----------
exp_nmnc=1
declare -a B_DIMS B_VARTYPE B_SIGNTYPE B_MEAN B_STDDEV B_COM
blk=-1
while read -r key a b c d _; do
  case "$key" in
    ''|\#*) continue ;;
    n_mnc)     exp_nmnc="$a" ;;
    tol_step)  tol_step="$a" ;;
    tol_start) tol_start="$a" ;;
    tol_rel)   tol_rel="$a" ;;
    tol_com)   tol_com="$a" ;;
    file)      blk=$((blk+1)); B_DIMS[$blk]=""; B_VARTYPE[$blk]=""
               B_SIGNTYPE[$blk]=""; B_MEAN[$blk]=""; B_STDDEV[$blk]=""; B_COM[$blk]="" ;;
    dim)       [ "$blk" -lt 0 ] && blk=0
               B_DIMS[$blk]+="$a $b $c $d
" ;;
    vartype)   [ "$blk" -lt 0 ] && blk=0; B_VARTYPE[$blk]="$a" ;;
    signtype)  [ "$blk" -lt 0 ] && blk=0; B_SIGNTYPE[$blk]="$a" ;;
    mean)      [ "$blk" -lt 0 ] && blk=0; B_MEAN[$blk]="$a" ;;
    stddev)    [ "$blk" -lt 0 ] && blk=0; B_STDDEV[$blk]="$a" ;;
    com)       [ "$blk" -lt 0 ] && blk=0; B_COM[$blk]="$a $b $c" ;;
    *) echo "WARN: unknown manifest key '$key'" ;;
  esac
done < "$MANIFEST"
NBLK=$((blk+1))

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

# ---- compare each output against its block (sorted order) ----------------
check_block() {  # $1=file  $2=block index
  local F="$1" bi="$2" pfx=""
  [ "$NBLK" -gt 1 ] && pfx="[$((bi+1))] "
  echo "file: $F"

  # dimension names + per-dim geometry
  local exp_dims="" line name len step start
  while IFS= read -r line; do
    [ -z "$line" ] && continue
    read -r name len step start <<< "$line"
    exp_dims+="$name "
    local glen gstep gstart
    glen=$(q -dimlength "$name" "$F")
    gstep=$(q -attvalue "$name:step" "$F")
    gstart=$(q -attvalue "$name:start" "$F")
    [ "$glen" = "$len" ] && report ok "${pfx}$name.length" "$len" "$glen" \
                         || report bad "${pfx}$name.length" "$len" "$glen"
    abs_ok "$gstep" "$step" "$tol_step" \
      && report ok "${pfx}$name.step" "$step" "$gstep" \
      || report bad "${pfx}$name.step" "$step" "$gstep"
    abs_ok "$gstart" "$start" "$tol_start" \
      && report ok "${pfx}$name.start" "$start" "$gstart" \
      || report bad "${pfx}$name.start" "$start" "$gstart"
  done <<< "${B_DIMS[$bi]}"
  local got_dims
  got_dims=$("$MINCINFO" -dimnames "$F" 2>/dev/null | xargs)
  exp_dims=$(echo "$exp_dims" | xargs)
  [ "$got_dims" = "$exp_dims" ] && report ok "${pfx}dimnames" "$exp_dims" "$got_dims" \
                               || report bad "${pfx}dimnames" "$exp_dims" "$got_dims"

  # data type
  if [ -n "${B_VARTYPE[$bi]}" ]; then
    local gv; gv=$(q -vartype image "$F")
    [ "$gv" = "${B_VARTYPE[$bi]}" ] && report ok "${pfx}vartype" "${B_VARTYPE[$bi]}" "$gv" \
                                    || report bad "${pfx}vartype" "${B_VARTYPE[$bi]}" "$gv"
  fi
  if [ -n "${B_SIGNTYPE[$bi]}" ]; then
    local gs; gs=$(q -attvalue image:signtype "$F")
    [ "$gs" = "${B_SIGNTYPE[$bi]}" ] && report ok "${pfx}signtype" "${B_SIGNTYPE[$bi]}" "$gs" \
                                     || report bad "${pfx}signtype" "${B_SIGNTYPE[$bi]}" "$gs"
  fi

  # statistics
  if [ -n "${B_MEAN[$bi]}" ]; then
    local gm; gm=$("$MINCSTATS" -quiet -mean "$F" 2>/dev/null)
    rel_ok "$gm" "${B_MEAN[$bi]}" "$tol_rel" && report ok "${pfx}mean" "${B_MEAN[$bi]}" "$gm" \
                                             || report bad "${pfx}mean" "${B_MEAN[$bi]}" "$gm"
  fi
  if [ -n "${B_STDDEV[$bi]}" ]; then
    local gd; gd=$("$MINCSTATS" -quiet -stddev "$F" 2>/dev/null)
    rel_ok "$gd" "${B_STDDEV[$bi]}" "$tol_rel" && report ok "${pfx}stddev" "${B_STDDEV[$bi]}" "$gd" \
                                               || report bad "${pfx}stddev" "${B_STDDEV[$bi]}" "$gd"
  fi
  if [ -n "${B_COM[$bi]}" ]; then
    local ex ey ez cx cy cz
    read -r ex ey ez <<< "${B_COM[$bi]}"
    read -r cx cy cz < <("$MINCSTATS" -quiet -com -world_only "$F" 2>/dev/null)
    abs_ok "$cx" "$ex" "$tol_com" && report ok "${pfx}com.x" "$ex" "$cx" || report bad "${pfx}com.x" "$ex" "$cx"
    abs_ok "$cy" "$ey" "$tol_com" && report ok "${pfx}com.y" "$ey" "$cy" || report bad "${pfx}com.y" "$ey" "$cy"
    abs_ok "$cz" "$ez" "$tol_com" && report ok "${pfx}com.z" "$ez" "$cz" || report bad "${pfx}com.z" "$ez" "$cz"
  fi
}

for i in "${!MNC[@]}"; do
  check_block "${MNC[$i]}" "$i"
done

[ "$fail" -eq 0 ] && { echo "RESULT: PASS"; exit 0; } || { echo "RESULT: FAIL"; exit 1; }
