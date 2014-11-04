#! /bin/bash
if [[ ! -x $MINCAVERAGE_BIN ]]; then
    MINCAVERAGE_BIN=`which mincaverage`;
fi;

if [[ ! -x $MINCSTATS_BIN ]]; then
    MINCSTATS_BIN=`which mincstats`;
fi;
# Test the standard (no-normalize) case. This has always worked.
$MINCAVERAGE_BIN -clobber mincaverage-in0.mnc mincaverage-in1.mnc mincaverage-out.mnc
r1=`$MINCSTATS_BIN -quiet -sum mincaverage-out.mnc`
if [[ $r1 != "-88.5" ]]; then
  echo "Problem with non-normalized average:" $r1
  exit 1;
fi;
# Now test the normalize case. This failed until fixed in Oct 2014.
$MINCAVERAGE_BIN -normalize -clobber mincaverage-in0.mnc mincaverage-in1.mnc mincaverage-out.mnc
r2=`$MINCSTATS_BIN -quiet -sum mincaverage-out.mnc`
if [[ $r2 != "-22.25" ]]; then
  echo "Problem with normalized average:" $r2
  exit 1;
fi;
echo "OK."
exit 0

