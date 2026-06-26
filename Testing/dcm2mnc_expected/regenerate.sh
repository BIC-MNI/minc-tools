#!/bin/bash
# Regenerate dcm2mnc test manifests + series.list from the dcm2niix QA data.
#
# Usage:
#   regenerate.sh --build <build-dir> [--data <dcm2niix_data-dir>]
#                 [--dcm2niix <path>]
#
# Ground-truth pipeline (reference is dcm2niix, never dcm2mnc):
#
#   DICOM --dcm2niix -f %j--> <SeriesInstanceUID>.nii --nii2mnc--> ref.mnc
#         --mincinfo/mincstats--> orientation-invariant reference quantities
#
# For every leaf DICOM series under each downloaded dcm_qa* repo we run
# dcm2niix (which decodes JPEG/JPEG-LS/JPEG2000/RLE) and write one
# <name>.manifest per series, with one reference block per produced NIfTI.
# Each block is keyed by SeriesInstanceUID (dcm2niix %j) so the test harness
# can join dcm2mnc's output to its reference by UID (dcm2mnc stores the same
# UID in dicom_0x0020:el_0x000e) -- no geometric/name guessing.
#
# Stored quantities are orientation-invariant (dcm2mnc and dcm2niix may store
# an axis with opposite polarity for the same physical volume):
#   dimlen   sorted multiset of dimension lengths (length-1 axes squeezed)
#   voxsize  sorted |step| of the spatial axes (mm)
#   com      centre of mass in world coordinates (mm)
#   mean     image mean
#   stddev   image standard deviation
#
# This is a regenerate-time tool only: dcm2niix is required HERE, never at
# test time (the committed manifests are self-contained).  series.list is
# consumed by Testing/CMakeLists.txt to register one test per series.
#
# Series for which dcm2niix produces no NIfTI (no ground truth) are logged
# and skipped.  Compressed series are NOT skipped: dcm2niix decodes them and
# the resulting dcm2mnc test will surface any dcm2mnc-side failure.
set -u

EXPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD=""; DATA=""; DCM2NIIX="${DCM2NIIX:-}"
while [ $# -gt 0 ]; do
  case "$1" in
    --build)    BUILD="$2"; shift 2 ;;
    --data)     DATA="$2";  shift 2 ;;
    --dcm2niix) DCM2NIIX="$2"; shift 2 ;;
    *) echo "unknown arg: $1"; exit 2 ;;
  esac
done
[ -n "$BUILD" ] || { echo "need --build <build-dir>"; exit 2; }
[ -n "$DATA" ] || DATA="$BUILD/minctools/Testing/dcm2niix_data"

find_tool() { find "$BUILD" -name "$1" -type f -executable 2>/dev/null | head -1; }
MI=$(find_tool mincinfo); MS=$(find_tool mincstats); N2M=$(find_tool nii2mnc)
DCM=$(find_tool dcm2mnc)
[ -n "$DCM2NIIX" ] || DCM2NIIX=$(command -v dcm2niix 2>/dev/null || true)
for t in "$MI" "$MS" "$N2M"; do
  [ -x "$t" ] || { echo "missing minc tool (build first?): mincinfo/mincstats/nii2mnc"; exit 1; }
done
[ -x "$DCM2NIIX" ] || { echo "dcm2niix not found; pass --dcm2niix <path> or set \$DCM2NIIX"; exit 1; }
NIIX_VER=$("$DCM2NIIX" --version 2>/dev/null | grep -i 'version' | head -1)
[ -n "$NIIX_VER" ] || NIIX_VER="dcm2niix (version unknown)"

WORK=$(mktemp -d)
trap 'rm -rf "$WORK"' EXIT
LIST="$EXPDIR/series.list"
: > "$LIST"
: > "$WORK/skips"

# non-DICOM file filter shared with the test harness
dicom_files() {  # $1 = series dir -> prints DICOM file paths
  find "$1" -maxdepth 1 -type f \
    -not -iname '*.nii'  -not -iname '*.nii.gz' -not -iname '*.json' \
    -not -iname '*.bval' -not -iname '*.bvec'   -not -iname '*.csv' \
    -not -iname '*.xlsx' -not -iname '*.txt'    -not -iname '*.md' \
    -not -iname 'README*' -not -iname '.*'
}

abs() { awk -v x="$1" 'BEGIN{printf "%.10g", (x<0?-x:x)}'; }

dump_ref() {  # $1 = ref.mnc ; $2 = UID  -> append one manifest block
  local F="$1" uid="$2" d l s
  echo "ref       $uid"
  # dimension lengths, length-1 axes squeezed, sorted ascending
  local dimlen=""
  for d in $("$MI" -dimnames "$F" 2>/dev/null); do
    l=$("$MI" -dimlength "$d" "$F" 2>/dev/null | tr -d ' ')
    [ -n "$l" ] && [ "$l" -gt 1 ] && dimlen+="$l
"
  done
  printf 'dimlen    %s\n' "$(printf '%s' "$dimlen" | sort -n | xargs)"
  # spatial voxel sizes |step|, sorted ascending
  local vox=""
  for d in xspace yspace zspace; do
    s=$("$MI" -attvalue "$d:step" "$F" 2>/dev/null | tr -d ' ')
    [ -n "$s" ] && vox+="$(abs "$s")
"
  done
  printf 'voxsize   %s\n' "$(printf '%s' "$vox" | sort -g | xargs)"
  printf 'com       %s\n' "$("$MS" -quiet -com -world_only "$F" 2>/dev/null)"
  printf 'mean      %s\n' "$("$MS" -quiet -mean "$F" 2>/dev/null)"
  printf 'stddev    %s\n' "$("$MS" -quiet -stddev "$F" 2>/dev/null)"
}

uid_of_nii() {  # $1 = nii path -> SeriesInstanceUID (strip series prefix/echo suffix)
  basename "$1" .nii | grep -oE '[0-9]+(\.[0-9]+){3,}' | head -1
}

process_repo() {  # $1 = repo name
  local repo="$1" root="$DATA/$1/In"
  [ -d "$root" ] || { echo "  (no $root, skipping repo)"; return; }
  # leaf series = dirs that directly contain at least one DICOM file
  find "$root" -type f | while read -r f; do dirname "$f"; done | sort -u | \
  while read -r dir; do
    local cnt; cnt=$(dicom_files "$dir" | wc -l)
    [ "$cnt" -ge 1 ] || continue
    local rel="${dir#"$DATA/$repo/"}"                 # In/.../series
    local name; name=$(echo "${repo}_${rel#In/}" | tr '/ ' '__' | tr -cd 'A-Za-z0-9_.-')

    # ground truth: convert this series only (-d 0) with dcm2niix, name by UID
    local nii="$WORK/nii_$name"; rm -rf "$nii"; mkdir -p "$nii"
    "$DCM2NIIX" -b y -z n -d 0 -f '%j' -o "$nii" "$dir" >/dev/null 2>&1
    mapfile -t refs < <(find "$nii" -name '*.nii' | sort)
    if [ "${#refs[@]}" -eq 0 ]; then
      echo "  SKIP(noref) $repo/$rel  (dcm2niix produced no NIfTI)"
      echo "NOREF $repo/$rel" >> "$WORK/skips"; continue
    fi

    # write manifest: one block per reference NIfTI
    local man="$EXPDIR/$name.manifest"
    {
      echo "# $repo/$rel"
      echo "# ground truth: DICOM -> dcm2niix -> nii2mnc"
      echo "# $NIIX_VER"
      echo "# auto-generated by regenerate.sh -- do not edit by hand"
      echo "n_mnc     ${#refs[@]}"
      local R uid
      for R in "${refs[@]}"; do
        uid=$(uid_of_nii "$R")
        "$N2M" -clobber "$R" "$WORK/ref.mnc" >/dev/null 2>&1 || continue
        dump_ref "$WORK/ref.mnc" "$uid"
      done
    } > "$man"
    echo "$name|$repo|$rel" >> "$LIST"
    echo "  KEEP        $repo/$rel  (${#refs[@]} ref)"

    # optional cross-check: does dcm2mnc agree on count? (logged only, non-blocking)
    if [ -x "$DCM" ]; then
      local out="$WORK/mnc_$name"; rm -rf "$out"; mkdir -p "$out"
      if dicom_files "$dir" | timeout 120 "$DCM" -clobber -stdin "$out" >/dev/null 2>&1; then
        local nmnc; nmnc=$(find "$out" -name '*.mnc' | wc -l)
        [ "$nmnc" -eq "${#refs[@]}" ] || echo "      NOTE count differs: dcm2niix=${#refs[@]} dcm2mnc=$nmnc"
      else
        echo "      NOTE dcm2mnc failed to convert (test will FAIL, as intended)"
      fi
    fi
  done
}

echo "Regenerating dcm2mnc manifests from $DATA"
echo "  dcm2niix: $DCM2NIIX ($NIIX_VER)"
rm -f "$EXPDIR"/*.manifest
for repo in dcm_qa dcm_qa_nih dcm_qa_uih; do
  echo "== $repo =="
  process_repo "$repo"
done

sort -o "$LIST" "$LIST"
kept=$(wc -l < "$LIST")
noref=$(grep -c '^NOREF' "$WORK/skips" 2>/dev/null || echo 0)
echo
echo "SUMMARY: kept=$kept  skipped_noref=$noref"
echo "series.list -> $LIST"
