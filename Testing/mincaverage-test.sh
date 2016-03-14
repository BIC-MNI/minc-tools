#! /bin/bash
if [[ ! -x $MINCAVERAGE_BIN ]]; then
    MINCAVERAGE_BIN=`which mincaverage`;
fi;

if [[ ! -x $MINCSTATS_BIN ]]; then
    MINCSTATS_BIN=`which mincstats`;
fi;

if [[ ! -x $MINCINFO_BIN ]]; then
    MINCINFO_BIN=`which mincinfo`;
fi

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

# Now test new avgdim case. This would fail until 3 March 2016.
$MINCAVERAGE_BIN -avgdim time -clobber test-4d.mnc mincaverage-out.mnc
if [$? -ne 0 ]; then
  echo "Problem running mincaverage."
  exit 1;
fi;
r3=`$MINCSTATS_BIN -quiet -max mincaverage-out.mnc`
if [[ $r3 != "2" ]]; then
  echo "Problem with maximum value:" $r3
  exit 1;
fi;
r4=`$MINCSTATS_BIN -quiet -min mincaverage-out.mnc`
if [[ $r4 != "2" ]]; then
  echo "Problem with minimum value:" $r4
  exit 1;
fi;
r5=`$MINCINFO_BIN -dimlength time test-4d.mnc`
if [[ $r5 != "5" ]]; then
   echo "Problem with time:", $r5
   exit 1;
fi;
r6=`$MINCINFO_BIN -dimlength time mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time:", $r6
   exit 1;
fi;
r7=`$MINCINFO_BIN -dimlength time-width mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time-width:", $r7
   exit 1;
fi;
r8=`$MINCINFO_BIN -varvalues time-width mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time-width:", $r8
   exit 1;
fi;
echo "OK."
exit 0

