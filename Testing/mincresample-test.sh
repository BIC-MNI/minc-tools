#! /bin/bash

let errors=0;

if [[ ! -x $MINCSTATS_BIN ]]; then
    MINCSTATS_BIN=`which mincstats`;
fi

if [[ ! -x $MINCRESAMPLE_BIN ]]; then
    MINCRESAMPLE_BIN=`which mincresample`;
fi

# Test the standard (no-normalize) case. This has always worked.
$MINCRESAMPLE_BIN -clobber test-rnd.mnc mincresample-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum mincresample-out.mnc`
if [[ $r1 != "250" ]]; then
  echo "Problem with default operation:" $r1
  exit 1;
fi;
# Now test the keep_real_range case. This failed until fixed in Feb 2015.
$MINCRESAMPLE_BIN -keep_real_range -clobber test-rnd.mnc mincresample-out.mnc
r2=`$MINCSTATS_BIN -quiet -sum mincresample-out.mnc`
if [[ $r2 != "250" ]]; then
  echo "Problem with -keep_real_range operation:" $r2
  exit 1;
fi;
echo "OK."
exit 0


