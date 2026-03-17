#! /bin/sh
# Test that mincmorph hole-fill (B[1:1:0:1]GB[1:1:0:1]) preserves the
# total foreground volume when applied to a solid binary cube that does
# not touch the volume boundaries.
#
# The bug being guarded against: group_kernel used to skip all boundary
# voxels in its labelling pass, leaving them with value 0.  The final
# B[1:1:0:1] step then set those 0-valued boundary voxels to 1 (bg),
# erroneously inflating the output volume count.

errors=0

TMPDIR=$(mktemp -d)

# ---------------------------------------------------------------------------
# Case 14: hole-fill on a solid cube must preserve the voxel count
#
# A 30x30x30 binary volume (1 mm isotropic) with a single solid cube
# centred well away from all six faces.  -no_partial keeps voxels 0 or 1.
# Expected: output voxel sum == input voxel sum (no boundary voxels added).
# ---------------------------------------------------------------------------
CUBE="$TMPDIR/cube.mnc"
CUBE_FILLED="$TMPDIR/cube-filled.mnc"

make_phantom -clobber -rectangle \
    -nelements 30 30 30 -step 1 1 1 -start 0 0 0 \
    -center 15 15 15 -width 10 10 10 \
    -fill_value 1 -background 0 -no_partial -byte \
    "$CUBE" || { echo "make_phantom failed"; exit 1; }

input_sum=$(mincstats -sum -q "$CUBE")

echo -n "Case 14 (hole-fill preserves solid cube volume)..."
mincmorph -clobber -successive 'B[1:1:0:1]GB[1:1:0:1]' "$CUBE" "$CUBE_FILLED"
if [ $? -ne 0 ]; then
    echo "FAIL: mincmorph returned non-zero exit code"
    errors=$((errors + 1))
else
    output_sum=$(mincstats -sum -q "$CUBE_FILLED")
    if [ "$input_sum" = "$output_sum" ]; then
        echo "OK (volume = $input_sum voxels)"
    else
        echo "FAIL: input sum=$input_sum output sum=$output_sum (boundary voxels corrupted?)"
        errors=$((errors + 1))
    fi
fi

# ---------------------------------------------------------------------------
# Case 15: hole-fill on a hollow cube must fill the interior
#
# Create a shell by subtracting a small inner cube from a larger outer cube.
# After hole-fill the result must equal the solid outer cube voxel count.
# ---------------------------------------------------------------------------
OUTER="$TMPDIR/outer.mnc"
INNER="$TMPDIR/inner.mnc"
SHELL_VOL="$TMPDIR/shell.mnc"
SHELL_FILLED="$TMPDIR/shell-filled.mnc"

make_phantom -clobber -rectangle \
    -nelements 30 30 30 -step 1 1 1 -start 0 0 0 \
    -center 15 15 15 -width 14 14 14 \
    -fill_value 1 -background 0 -no_partial -byte \
    "$OUTER" || { echo "make_phantom (outer) failed"; exit 1; }

make_phantom -clobber -rectangle \
    -nelements 30 30 30 -step 1 1 1 -start 0 0 0 \
    -center 15 15 15 -width 8 8 8 \
    -fill_value 1 -background 0 -no_partial -byte \
    "$INNER" || { echo "make_phantom (inner) failed"; exit 1; }

# shell = outer - inner  (both are binary 0/1, so result is 0 or 1)
mincmath -clobber -sub "$OUTER" "$INNER" "$SHELL_VOL" \
    || { echo "mincmath failed"; exit 1; }

outer_sum=$(mincstats -sum -q "$OUTER")
shell_sum=$(mincstats -sum -q "$SHELL_VOL")

echo -n "Case 15 (hole-fill fills hollow cube)..."
mincmorph -clobber -successive 'B[1:1:0:1]GB[1:1:0:1]' "$SHELL_VOL" "$SHELL_FILLED"
if [ $? -ne 0 ]; then
    echo "FAIL: mincmorph returned non-zero exit code"
    errors=$((errors + 1))
else
    filled_sum=$(mincstats -sum -q "$SHELL_FILLED")
    if [ "$filled_sum" = "$outer_sum" ]; then
        echo "OK (shell=$shell_sum filled=$filled_sum = solid outer=$outer_sum)"
    else
        echo "FAIL: filled sum=$filled_sum expected=$outer_sum (hole not filled?)"
        errors=$((errors + 1))
    fi
fi

# ---------------------------------------------------------------------------
rm -rf "$TMPDIR"

if [ "$errors" = "0" ]; then
    echo "No errors detected."
else
    echo "$errors errors detected."
fi
exit $errors
