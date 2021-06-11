#!/bin/sh
# (C) Copyright 2005- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

. ./include.sh

set -u
cd ${data_dir}/bufr

# Define a common label for all the tmp files
label="bufr_filter_extract_subsets"

# Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

fBufrTmp1=temp1.${label}.bufr
fBufrTmp2=temp2.${label}.bufr

# Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# Test:  extract subsets uncompressed data
#-----------------------------------------------------------
cat > $fRules <<EOF
 set unpack=1;

 set extractSubset=4;
 set doExtractSubsets=1;
 write;

 set extractSubset=2;
 set doExtractSubsets=1;
 write;

 set extractSubsetIntervalStart=5;
 set extractSubsetIntervalEnd=8;
 set doExtractSubsets=1;
 write;

 set extractSubsetList={1,3};
 set doExtractSubsets=1;
 write;
EOF

f="synop_multi_subset.bufr"
fOut="extract.bufr"

echo "Test: extract subsets uncompressed data" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/codes_bufr_filter -o ${fOut} $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;
print "stationNumber=[stationNumber!13]";
EOF

${tools_dir}/codes_bufr_filter $fRules $f $fOut > ${fOut}.log

cat > ${fOut}.log.ref <<EOF
stationNumber=27 84 270 272 308 371 381 382 387 413 464 485
stationNumber=272
stationNumber=84
stationNumber=308 371 381 382 84
stationNumber=308 371 381 382 84 27 270
EOF

diff ${fOut}.log.ref ${fOut}.log 

rm -f ${fOut}.log ${fOut}.log.ref
rm -f $fLog $fRules ${fOut}


#-----------------------------------------------------------
# Test:  extract subsets compressed data
#-----------------------------------------------------------
cat > $fRules <<EOF
set unpack=1;

set extractSubset=10;
set doExtractSubsets=1;
write;

set extractSubsetIntervalStart=3;
set extractSubsetIntervalEnd=8;
set doExtractSubsets=1;
write;
EOF

f="g2nd_208.bufr"
fOut="$label.g2nd_208.bufr.out"

echo "Test: extract subsets compressed data" >> $fLog
echo "file: $f" >> $fLog
${tools_dir}/codes_bufr_filter -o $fOut $fRules $f 2>> $fLog 1>> $fLog

cat > ${fRules} <<EOF
set unpack=1;
print "=== message number [count]";
print "numberOfSubsets=[numberOfSubsets]";
print "solarElevation=[solarElevation!10]";
print "fieldOfViewNumber=[fieldOfViewNumber!10]";
print "orbitNumber=[orbitNumber!10]";
print "casRegistryNumber=[casRegistryNumber!10]";
EOF

${tools_dir}/codes_bufr_filter $fRules $fOut  > ${f}.log

cat > ${f}.log.ref <<EOF
=== message number 1
numberOfSubsets=1
solarElevation=33.2
fieldOfViewNumber=1
orbitNumber=2147483647
casRegistryNumber=10102-44-0
=== message number 2
numberOfSubsets=7
solarElevation=29.71 29.23 37.21 36.78 36.34 35.46 33.2
fieldOfViewNumber=2 2 0 0 0 1 1
orbitNumber=2147483647
casRegistryNumber=10102-44-0
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref
rm -f $fLog $fOut $fRules 

#-----------------------------------------------------------
# Test: Simple thinning
#-----------------------------------------------------------
cat > $fRules <<EOF
set simpleThinningSkip=36;
set doSimpleThinning=1;
write;
assert(numberOfSubsets == 5);
EOF

f="imssnow.bufr"

echo "Test: Simple thinning" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}/codes_bufr_filter -o $fBufrTmp1 $fRules $f

cat > $fRules <<EOF
set unpack=1;
print "latitude=[latitude]";
print "longitude=[longitude]";
print "height=[height]";
EOF

${tools_dir}/codes_bufr_filter $fRules $fBufrTmp1 > ${f}.log

cat > ${f}.log.ref <<EOF
latitude=4.93301 5.17216 5.40243 5.62361 7.86075
longitude=118.16205 117.41896 116.66977 115.91467 99.56805
height=119 231 587 187 23
EOF

diff ${f}.log.ref ${f}.log 

rm -f ${f}.log ${f}.log.ref $fBufrTmp1 $fLog $fRules
#-----------------------------------------------------------
# Test: subset extraction constant values
#-----------------------------------------------------------
cat > $fRules <<EOF
set numberOfSubsets=10;
set compressedData=1;
set unexpandedDescriptors={5002};
set latitude={0,0,0,0,0,0,0,1,0,0};
set pack=1;
write;
EOF

f="go15_87.bufr"

echo "Test: subset extraction constant values" >> $fLog
echo "file: $f" >> $fLog

${tools_dir}/codes_bufr_filter -o $fBufrTmp1 $fRules $f

cat > $fRules <<EOF
set unpack=1;
set extractSubsetIntervalStart=1;
set extractSubsetIntervalEnd=4;
set doExtractSubsets=1;
write;
EOF

${tools_dir}/codes_bufr_filter -o $fBufrTmp2 $fRules $fBufrTmp1

cat > $fRules <<EOF
set unpack=1;
print "latitude=[latitude]";
EOF

${tools_dir}/codes_bufr_filter $fRules $fBufrTmp2 > ${f}.log

cat > ${f}.log.ref <<EOF
latitude=0
EOF
diff ${f}.log.ref ${f}.log 

# Clean up
rm -f ${f}.log ${f}.log.ref $fBufrTmp1 $fBufrTmp2 $fLog $fRules
