#! /bin/sh

let errors=0;

# Test the standard (no-normalize) case. This has always worked.
mincresample -clobber test-rnd.mnc mincresample-out.mnc
r1=`mincstats -quiet -sum mincresample-out.mnc`
if [ "$r1" != "250" ]; then
  echo "Problem with default operation:" $r1
  exit 1;
fi;
# Now test the keep_real_range case. This failed until fixed in Feb 2015.
mincresample -keep_real_range -clobber test-rnd.mnc mincresample-out.mnc
r2=`mincstats -quiet -sum mincresample-out.mnc`
if [ "$r2" != "250" ]; then
  echo "Problem with -keep_real_range operation:" $r2
  exit 1;
fi;
# Now test mincresample converted properly a 4D float volume
# Make a input volume
mincreshape -float -colsize 125 -rowsize 1 test-rnd.mnc test-rnd-reshaped.mnc -clobber
mincconcat -concat_dimension time test-rnd-reshaped.mnc test-rnd-reshaped-4d.mnc -clobber
mincresample -clobber test-rnd-reshaped-4d.mnc mincresample-out.mnc
r3=`minccmp test-rnd-reshaped-4d.mnc mincresample-out.mnc`
# Verify if xcorr is 1
xcorr=`echo $r3 | grep -o 'xcorr: [^ ]*' | cut -d ' ' -f2`
if [ "$xcorr" != 1 ]; then
  echo "Problem with output file compared with input file, xcorr is" $xcorr
  exit 1;
fi;
echo "OK."
exit 0
