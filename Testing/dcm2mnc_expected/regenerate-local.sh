#!/bin/bash
# Regenerate dcm2mnc regression manifests for LOCAL, non-redistributable DICOM
# data (e.g. clinical exports that cannot be downloaded by CI).
#
# Usage:
#   regenerate-local.sh --build <build-dir> --data <local-data-root> \
#                       [--dcm2niix <path>] [--series <name>:<relpath> ...]
#
# Same ground-truth pipeline as dcm2mnc_expected/regenerate.sh: the reference
# is dcm2niix, never dcm2mnc.
#
#   DICOM --dcm2niix -f %j--> <SeriesInstanceUID>.nii --nii2mnc--> ref.mnc
#         --mincinfo/mincstats--> orientation-invariant reference quantities
#
# <local-data-root> is the directory CMake's MT_DCM2MNC_LOCAL_DATA points at.
# Each --series gives a test name and a path (relative to the data root) of a
# directory holding one DICOM series. If no --series is given, the default set
# below (the CVI BIO0410071/BIO0410141 exams) is used.
#
# The manifests are written NEXT TO THE DATA, under
#   <local-data-root>/dcm2mnc_manifests/<name>.manifest
# and are never committed to the repository -- they are derived from, and only
# meaningful with, the private data. Each manifest is self-describing: it carries
# its series relpath in a "series" line so CMake can discover tests by globbing
# the manifest dir (no index file). dcm2niix is required here at regeneration
# time, never at test time.
set -u

EXPDIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD=""; DATA=""; DCM2NIIX="${DCM2NIIX:-}"
declare -a SERIES=()
while [ $# -gt 0 ]; do
  case "$1" in
    --build)    BUILD="$2"; shift 2 ;;
    --data)     DATA="$2";  shift 2 ;;
    --dcm2niix) DCM2NIIX="$2"; shift 2 ;;
    --series)   SERIES+=("$2"); shift 2 ;;
    *) echo "unknown arg: $1"; exit 2 ;;
  esac
done
[ -n "$BUILD" ] || { echo "need --build <build-dir>"; exit 2; }
[ -n "$DATA"  ] || { echo "need --data <local-data-root>"; exit 2; }

# Default series: two CVI exams that exercised the dcm2mnc dimension-sizing
# fixes. "ORIG" duplicate series are omitted; they are geometrically identical
# to their non-ORIG counterparts.
#   BIO0410071 - GE 3D Cube + PROPELLER, missing ImagesInAcquisition.
#   BIO0410141 - GE 3D series with CardiacNumberOfImages=0.
if [ "${#SERIES[@]}" -eq 0 ]; then
  SERIES=(
    "cvi_bio0410071_sag_t2_flair:BIO0410071_062Y/series0016-unknown"
    "cvi_bio0410071_cor_t1_cube:BIO0410071_062Y/series0018-unknown"
    "cvi_bio0410071_ax_t2_propeller:BIO0410071_062Y/series0019-unknown"
    "cvi_bio0410141_cor_t1_cube:BIO0410141_051Y/series0023-unknown"
    "cvi_bio0410141_sag_t2_flair:BIO0410141_051Y/series0024-unknown"
    "cvi_bio0410141_ax_t2_propeller:BIO0410141_051Y/series0025-unknown"
  )
fi

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

dicom_files() {  # $1 = series dir -> prints DICOM file paths (shared filter)
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
  local dimlen=""
  for d in $("$MI" -dimnames "$F" 2>/dev/null); do
    l=$("$MI" -dimlength "$d" "$F" 2>/dev/null | tr -d ' ')
    [ -n "$l" ] && [ "$l" -gt 1 ] && dimlen+="$l
"
  done
  printf 'dimlen    %s\n' "$(printf '%s' "$dimlen" | sort -n | xargs)"
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

uid_of_nii() { basename "$1" .nii | grep -oE '[0-9]+(\.[0-9]+){3,}' | head -1; }

WORK=$(mktemp -d); trap 'rm -rf "$WORK"' EXIT
MANDIR="$DATA/dcm2mnc_manifests"; mkdir -p "$MANDIR"
kept=0; skipped=0

echo "Regenerating local dcm2mnc manifests from $DATA"
echo "  dcm2niix: $DCM2NIIX ($NIIX_VER)"
echo "  manifests -> $MANDIR"
rm -f "$MANDIR"/*.manifest

for entry in "${SERIES[@]}"; do
  name="${entry%%:*}"; rel="${entry#*:}"
  dir="$DATA/$rel"
  if [ ! -d "$dir" ]; then
    echo "  SKIP(no dir)  $rel"; skipped=$((skipped+1)); continue
  fi
  nii="$WORK/nii_$name"; rm -rf "$nii"; mkdir -p "$nii"
  "$DCM2NIIX" -b y -z n -d 0 -f '%j' -o "$nii" "$dir" >/dev/null 2>&1
  mapfile -t refs < <(find "$nii" -name '*.nii' | sort)
  if [ "${#refs[@]}" -eq 0 ]; then
    echo "  SKIP(noref)   $rel  (dcm2niix produced no NIfTI)"; skipped=$((skipped+1)); continue
  fi
  man="$MANDIR/$name.manifest"
  {
    echo "# $rel"
    echo "# ground truth: DICOM -> dcm2niix -> nii2mnc"
    echo "# $NIIX_VER"
    echo "# auto-generated by regenerate-local.sh -- do not edit by hand"
    echo "series    $rel"
    echo "n_mnc     ${#refs[@]}"
    for R in "${refs[@]}"; do
      uid=$(uid_of_nii "$R")
      "$N2M" -clobber "$R" "$WORK/ref.mnc" >/dev/null 2>&1 || continue
      dump_ref "$WORK/ref.mnc" "$uid"
    done
  } > "$man"
  echo "  KEEP          $rel  (${#refs[@]} ref)"
  kept=$((kept+1))

  if [ -x "$DCM" ]; then
    out="$WORK/mnc_$name"; rm -rf "$out"; mkdir -p "$out"
    if dicom_files "$dir" | timeout 120 "$DCM" -clobber -stdin "$out" >/dev/null 2>&1; then
      nmnc=$(find "$out" -name '*.mnc' | wc -l)
      [ "$nmnc" -eq "${#refs[@]}" ] || echo "      NOTE count differs: dcm2niix=${#refs[@]} dcm2mnc=$nmnc"
    else
      echo "      NOTE dcm2mnc failed to convert (test will FAIL, as intended)"
    fi
  fi
done

echo
echo "SUMMARY: kept=$kept  skipped=$skipped"
echo "manifests in $MANDIR"
