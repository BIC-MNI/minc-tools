#! /bin/bash

let errors=0;

if [[ ! -x $MINCSTATS_BIN ]]; then
    MINCSTATS_BIN=`which mincstats`;
fi

if [[ ! -x $MINCRESAMPLE_BIN ]]; then
    MINCRESAMPLE_BIN=`which mincresample`;
fi

if [[ ! -x $MINCRESHAPE_BIN ]]; then
    MINCRESHAPE_BIN=`which mincreshape`;
fi

if [[ ! -x $MINCCONCAT_BIN ]]; then
    MINCCONCAT_BIN=`which mincconcat`;
fi

if [[ ! -x $MINCCMP_BIN ]]; then
    MINCCMP_BIN=`which minccmp`;
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
# Now test mincresample converted properly a 4D float volume
# Make a input volume
$MINCRESHAPE_BIN -float -colsize 125 -rowsize 1 test-rnd.mnc test-rnd-reshaped.mnc -clobber
$MINCCONCAT_BIN -concat_dimension time test-rnd-reshaped.mnc test-rnd-reshaped-4d.mnc -clobber
$MINCRESAMPLE_BIN -clobber test-rnd-reshaped-4d.mnc mincresample-out.mnc
r3=`$MINCCMP_BIN test-rnd-reshaped-4d.mnc mincresample-out.mnc`
# Verify if xcorr is 1
xcorr=`echo $r3 | grep -o 'xcorr: [^ ]*' | cut -d ' ' -f2`
if [[ $xcorr != 1 ]]; then
  echo "Problem with output file compared with input file, xcorr is" $xcorr
  exit 1;
fi;
echo "OK."
exit 0


