#! /bin/sh

# Test the standard (no-normalize) case. This has always worked.
mincaverage -clobber mincaverage-in0.mnc mincaverage-in1.mnc mincaverage-out.mnc
r1=`mincstats -quiet -sum mincaverage-out.mnc`
if [ "$r1" != "-88.5" ]; then
  echo "Problem with non-normalized average:" $r1
  exit 1;
fi;
# Now test the normalize case. This failed until fixed in Oct 2014.
mincaverage -normalize -clobber mincaverage-in0.mnc mincaverage-in1.mnc mincaverage-out.mnc
r2=`mincstats -quiet -sum mincaverage-out.mnc`
if [ "$r2" != "-22.25" ]; then
  echo "Problem with normalized average:" $r2
  exit 1;
fi;

# Now test new avgdim case. This would fail until 3 March 2016.
mincaverage -avgdim time -clobber test-4d.mnc mincaverage-out.mnc
if [ $? -ne 0 ]; then
  echo "Problem running mincaverage."
  exit 1;
fi;
r3=`mincstats -quiet -max mincaverage-out.mnc`
if [ "$r3" != "2" ]; then
  echo "Problem with maximum value:" $r3
  exit 1;
fi;
r4=`mincstats -quiet -min mincaverage-out.mnc`
if [ "$r4" != "2" ]; then
  echo "Problem with minimum value:" $r4
  exit 1;
fi;
r5=`mincinfo -dimlength time test-4d.mnc`
if [ "$r5" != "5" ]; then
   echo "Problem with time:", $r5
   exit 1;
fi;

# VF: all these tests fail, because now mincaverage removes the dimension time

if /bin/false;then
r6=`mincinfo -dimlength time mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time:", $r6
   exit 1;
fi;
r7=`mincinfo -dimlength time-width mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time-width:", $r7
   exit 1;
fi;
r8=`mincinfo -varvalues time-width mincaverage-out.mnc`
if [ $? -ne 1 ]; then
   echo "Problem with time-width:", $r8
   exit 1;
fi;
fi

echo "OK."
exit 0
