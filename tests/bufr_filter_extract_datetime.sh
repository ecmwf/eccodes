#!/bin/sh
# Copyright 2005-2016 ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
# In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
# virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
#

set -x
. ./include.sh

cd ${data_dir}/bufr

#Define a common label for all the tmp files
label="bufr_filter_extract_datetime_test"

#Create log file
fLog=${label}".log"
rm -f $fLog
touch $fLog

#Define tmp bufr file
fBufrTmp=${label}".bufr.tmp"

#Define filter rules file
fRules=${label}.filter

#-----------------------------------------------------------
# Test: Datetime extraction
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;

 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=10;
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=31;
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
 if (extractedDateTimeNumberOfSubsets!=0) {
   write;
 }
 print "extracted [extractedDateTimeNumberOfSubsets] of [originalNumberOfSubsets] subsets";
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out
outputFilt=${label}.txt
outputRef=${label}.ref
rm -f $outputFilt

echo "Test: Datetime extraction" >> $fLog
echo "file: $inputBufr" >> $fLog

${tools_dir}bufr_filter -o $outputBufr $fRules $inputBufr  > $outputFilt
[ -f $outputBufr ]

cat > $fRules <<EOF
set unpack=1;
print "year=[year!15]";
print "===========";
print "month=[month!15]";
print "===========";
print "day=[day!15]";
print "===========";
print "hour=[hour!15]";
print "===========";
print "minute=[minute!15]";
print "===========";
print "second=[second!15]";
print "===========";
EOF
${tools_dir}bufr_filter $fRules $inputBufr $outputBufr  >> $outputFilt

cat > $outputRef <<EOF
extracted 30 of 128 subsets
year=2012
===========
month=10
===========
day=31
===========
hour=0
===========
minute=1
===========
second=23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 23.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 31.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 39.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 47.54 
55.54 55.54 55.54 55.54 55.54 55.54 55.54 55.54
===========
year=2012
===========
month=10
===========
day=31
===========
hour=0
===========
minute=1
===========
second=31.54
===========
EOF

diff $outputRef $outputFilt

#-----------------------------------------------------------
# Test invalid date
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;
 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=19; # Bad month
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=31;
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out

set +e
${tools_dir}bufr_filter -o $outputBufr $fRules $inputBufr
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "Bad start date: bufr_filter should have failed!"
   exit 1
fi

#-----------------------------------------------------------
# Test end date < start date
#-----------------------------------------------------------
cat > $fRules <<EOF
 transient originalNumberOfSubsets=numberOfSubsets;
 transient extractDateTimeYearStart=2012;
 transient extractDateTimeMonthStart=10;
 transient extractDateTimeDayStart=31;
 transient extractDateTimeHourStart=0;
 transient extractDateTimeMinuteStart=1;
 transient extractDateTimeSecondStart=31.6;

 transient extractDateTimeYearEnd=2012;
 transient extractDateTimeMonthEnd=10;
 transient extractDateTimeDayEnd=30; #Error
 transient extractDateTimeHourEnd=0;
 transient extractDateTimeMinuteEnd=1;
 transient extractDateTimeSecondEnd=39.6;

 set doExtractDateTime=1;
EOF

inputBufr="amsa_55.bufr"
outputBufr=${label}.${inputBufr}.out

set +e
${tools_dir}bufr_filter -o $outputBufr $fRules $inputBufr
status=$?
set -e
if [ $status -eq 0 ]; then
   echo "End date before start date: bufr_filter should have failed!"
   exit 1
fi



rm -f $outputRef $outputFilt $outputBufr $fLog $fRules
