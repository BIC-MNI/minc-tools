#! /bin/sh
#
# Test gzip-compressed NIfTI output from mnc2nii.
# Covers three scenarios:
#   1. -compress flag with .nii output name
#   2. Explicit .nii.gz output filename (no flag)
#   3. Uncompressed output is unaffected
#   4. Round-trip: compressed file readable by nii2mnc
#
# The working directory is set by ctest and already contains the test
# MINC files (see Testing/CMakeLists.txt).

INPUT=test-one.mnc

# Clean up any leftovers from previous runs
rm -f compress_flag_out.nii compress_flag_out.nii.gz \
      explicit_gz_out.nii.gz uncompressed_out.nii roundtrip.mnc

fail() {
    echo "FAIL: $*"
    exit 1
}

# Verify that a file begins with the gzip magic bytes 0x1f 0x8b.
is_gzip() {
    magic=$(od -A n -N 2 -t x1 "$1" | tr -d ' \n')
    [ "$magic" = "1f8b" ]
}

# --- Test 1: -compress flag produces .nii.gz ---
mnc2nii -compress "$INPUT" compress_flag_out.nii 2>/dev/null
[ $? -eq 0 ] || fail "-compress: mnc2nii returned non-zero"
[ -f compress_flag_out.nii.gz ] || fail "-compress: compress_flag_out.nii.gz not created (found: $(ls compress_flag_out* 2>&1))"
[ ! -f compress_flag_out.nii ] || fail "-compress: uncompressed compress_flag_out.nii should not exist"
is_gzip compress_flag_out.nii.gz || fail "-compress: compress_flag_out.nii.gz is not a valid gzip file"
gunzip -t compress_flag_out.nii.gz 2>/dev/null || fail "-compress: compress_flag_out.nii.gz fails gzip integrity check"

# --- Test 2: explicit .nii.gz filename produces .nii.gz ---
mnc2nii "$INPUT" explicit_gz_out.nii.gz 2>/dev/null
[ $? -eq 0 ] || fail "explicit .nii.gz: mnc2nii returned non-zero"
[ -f explicit_gz_out.nii.gz ] || fail "explicit .nii.gz: explicit_gz_out.nii.gz not created"
is_gzip explicit_gz_out.nii.gz || fail "explicit .nii.gz: explicit_gz_out.nii.gz is not a valid gzip file"
gunzip -t explicit_gz_out.nii.gz 2>/dev/null || fail "explicit .nii.gz: explicit_gz_out.nii.gz fails gzip integrity check"

# --- Test 3: uncompressed output is unaffected ---
mnc2nii "$INPUT" uncompressed_out.nii 2>/dev/null
[ $? -eq 0 ] || fail "uncompressed: mnc2nii returned non-zero"
[ -f uncompressed_out.nii ] || fail "uncompressed: uncompressed_out.nii not created"
is_gzip uncompressed_out.nii && fail "uncompressed: uncompressed_out.nii should not be gzip"

# --- Test 4: round-trip through nii2mnc ---
nii2mnc compress_flag_out.nii.gz roundtrip.mnc 2>/dev/null
[ $? -eq 0 ] || fail "round-trip: nii2mnc returned non-zero"
[ -f roundtrip.mnc ] || fail "round-trip: roundtrip.mnc not created"
# Check that the round-tripped file has the same voxel sum as the original.
orig_sum=$(mincstats -quiet -sum "$INPUT" 2>/dev/null)
rt_sum=$(mincstats -quiet -sum roundtrip.mnc 2>/dev/null)
[ "$orig_sum" = "$rt_sum" ] || fail "round-trip: voxel sum mismatch (orig=$orig_sum rt=$rt_sum)"

echo "OK."
exit 0
